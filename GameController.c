#include <stdint.h>
#include "inc/SysTick.h"
#include "inc/tm4c123gh6pm.h"
#include "GameController.h"
#include "Graphics.h"
#include "Sprites.h"
#include "Portraits.h"
#include "Maps.h"
#include "Sound.h"
#include "GameFSM.h"
#include "Input.h"
#include "CharActions.h"

#define pcVector 0x07
#define npcVector 0xF1

/* ============= Gameplay Registers ====================
* Values initialized at the beginning, stored until the game is restarted.
*/
struct Unit units[8]; //[0, 1, 2] used for player characters, [3, ... 7] used for npcs when a map is selected.

//cursor positions for mission screen
uint16_t missionScreenY;
uint16_t missionScreenYmax;

//cursor positions for character select screen
uint16_t buildTeamX;
uint16_t buildTeamY;

/* ============== Map Registers ====================
* Global Values used during fights each time a map is loaded up.
*/
uint8_t unitXLocations[8]; 	//index is character id number
uint8_t unitYLocations[8]; 	//index is character id number
int16_t unitsOnMap[8][8]; 	//-1 indicates empty, else id number of character in that space
const char** tilesOnMap;		//8x8 array describing the terrain
uint8_t numCharacters; 			//number of characters on the current map, max 8
uint8_t alive; 							//npc + pc vector
uint8_t moved; 							//npc + pc vector
uint16_t special; 					// win, lose, ect.

//cursor positions for map
uint16_t mapOldX;
uint16_t mapOldY;
uint16_t mapX;
uint16_t mapY;

//state-specific regs
struct Unit* selectedUnit; 		//used for charSelectedState
uint16_t selectedX;				
uint16_t selectedY;
uint16_t attackTargetId;

//allowable locations for characters/cursors
#define Xmin 0
#define Ymin 1
#define Xmax 7
#define Ymax 7

/* ================ End Registers ==================== */

//declare state machine
const struct State* currentState;

const struct State selectTeam;
const struct State previewStats;
const struct State addToTeam;

const struct State viewTutorial; //push B from the scan 
const struct State chooseMap;
const struct State initializeMap;

const struct State scanMap;
const struct State charSelected;
const struct State charActionState;
const struct State checkWin;
const struct State waitForEnemy;
const struct State checkLose;

/* ======= GENERAL STATE MACHINE FUNCTIONS =============*
* Undo State is to go back when a B button is pressed, not always applicable
* Next State goes to the next state, should check that next state is valid BEFORE calling
*/

void EmptyFunc(){
	return;
}

uint16_t IdToVector(uint16_t id){ //id for indexes to vector for alive/moved reg
	uint16_t ret;
	switch(id) {
		case 0: ret = 0x1; break;
		case 1: ret = 0x2; break;
		case 2: ret = 0x4; break;
		case 3: ret = 0x8; break;
		case 4: ret = 0x10; break;
		case 5: ret = 0x20; break;
		case 6: ret = 0x40; break;
		case 7: ret = 0x80; break;
	}
	return ret;
}

void UndoState(){
	//clear any between state flags
	switch(currentState->StateNum){
		case 1: currentState = &previewStats; break;
		case 4: currentState = &scanMap; break;
		default:
			break; //cannot go back
	}
}

void NextState(){
//	currentState = currentState->nextState;
		switch(currentState->StateNum){
			case 0: currentState = &previewStats; break;
			case 1: currentState = &addToTeam; break;
			case 2: currentState = &selectTeam; break;
			
			case 3: currentState = &charSelected; break;
			case 4: currentState = &charActionState; break;
			case 5: currentState = &checkWin; break;
			case 6: currentState = &waitForEnemy; break;
			case 7: currentState = &checkLose; break;
			case 8: currentState = &scanMap; break;
			
			case 9: break;
			case 10: break;
			case 11: break;
		default:
			break; //cannot go back
	}
}

/* checkLose state - finished, TEST */
void CheckLose(){
	if((alive & pcVector) == 0){
		ShowLoseScreen();
		//set lose flag, do stuff to restart game
	}
	else{
		NextState();
	}
}

/* checkWin state - finished, TEST */
void CheckWin(){
		if((alive & npcVector) == 0){
		ShowWinScreen();
		//level up players and shit idk
	}
	else{
		NextState();
	}
}

void AnimateCharacters(){
	
}

void SysTickInit(){
	SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
  SysTick_Init();             // initialize SysTick timer
  GPIO_PORTF_DIR_R |= 0x04;   // make PF2 out (built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
}

/* ======= FUNCTIONS FOR MAP & MAP STATES =============*
* UpdateCursor(dX, dY), UpdateInfoScreen(character id)
* scanMap: onA, onScroll
* charSelected: checkValidAction (A), TentativeMove (Scroll),
* charActionState: SelectAttack Target (A), ChangeAttackTarget (Scroll), Calculate Combat for preview
* waitForEnemy: idle and wait for the server i guess
*/

void UpdateCursor(int8_t dX, int8_t dY){
	//change cursor location, NO PRINTING HERE!!!	
	switch(currentState->StateNum){
		case 3: //scanMap scroll
			if(mapX == Xmin && dX < 0){ mapX = Xmin; }
			else if(mapX == Xmax && dX > 0){ mapX = Xmax; }
			else{mapX = mapX + dX; }
	
			if(mapY == Ymin && dY < 0){ mapY = Ymin; }
			else if(mapY == Ymax && dY > 0){ mapY = Ymax; }
			else{mapY = mapY + dY; }
			break;
		default: return;
	}
}

void UpdateInfoScreen(uint8_t id){
		ShowInfo(units[id].name, id, units[id].lvl,
			units[id].HP, units[id].MHP, units[id].ATK, units[id].DEF,
			units[id].RES, units[id].SPD);
}

void ScanMapA(){
	//select a char
	if((unitsOnMap[mapX][mapY] > -1) && (unitsOnMap[mapX][mapY] < 3)){
		selectedUnit = &units[unitsOnMap[mapX][mapY]];
		selectedX = mapX;
		selectedY = mapY;
	}
	else{ //not a valid character
		return; 
	}
	//generate movement grid on global: validMoves
	getValidMoves(mapX, mapY, tilesOnMap, (int16_t **) unitsOnMap, units[unitsOnMap[mapX][mapY]].id);
	
	int16_t i = 0;
	while(validMoves[0][i] != 0xFF){
		PrintMoveTile(validMoves[0][i], validMoves[1][i]);
		i++;
	};
	
	//go to next state
	NextState();
}

/* scanMap state onScroll - finished, TEST */
void ScanMapScroll(){
	//reprint map square
	PrintTile(mapOldX, mapOldY);
	//reprint characters
	if(unitsOnMap[mapOldX][mapOldY] > -1){
		PrintSprite(unitsOnMap[mapOldX][mapOldY], mapOldX, mapOldY);
	}
	if((mapOldY < 7) && (unitsOnMap[mapOldX][mapOldY + 1] > (-1))){
		PrintSprite(unitsOnMap[mapOldX][mapOldY + 1], mapOldX, mapOldY + 1);
	}
	
	//print cursor
	PrintCursor(mapX, mapY);
	//check & update info window
	if(unitsOnMap[mapX][mapY] > -1){
		UpdateInfoScreen(unitsOnMap[mapX][mapY]);
	}
}

void CheckValidAction(){
	//check if move is valid
	
	//go to action state
	NextState();
}

void TentativeMove(){
	//don't let cursor move outside of move grid
	
	//reprint move grid from character's original location
	
	//reprint cursor
}

//Preview values calculated from combat
uint16_t defenderNewHP;
uint16_t attackerNewHP; 

void CalculateCombat(uint16_t attackerId, uint16_t defenderId){
	//damage done to each unit
	uint16_t damage = units[attackerId].ATK;
	uint16_t recoil = units[defenderId].ATK;
		switch(units[attackerId].weapon){
			case tome:
				damage-= units[defenderId].RES;
				break;
			case staff:
				damage-= units[defenderId].RES;
			break;
			default: damage-= units[defenderId].DEF;
		}
		switch(units[defenderId].weapon){
			case tome:
				recoil-= units[attackerId].RES;
				break;
			case staff:
				recoil-= units[attackerId].RES;
			break;
			default: damage-= units[defenderId].DEF;
		}
		//weapon triangle 
// 1) sword > axe > lance > sword
// 2) tome > armor > staff > tome
		switch(units[attackerId].weapon){
			case sword:
				if(units[defenderId].weapon == lance){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == axe){
					damage = (damage * 3)/2;
					recoil = (recoil * 3)/4;
				}
				break;
			case lance:
				if(units[defenderId].weapon == axe){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == sword){
					damage = (damage * 3)/2;
					recoil = (recoil * 3)/4;
				}
				break;
			case axe:
				if(units[defenderId].weapon == sword){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == lance){
					damage = (damage * 3)/2;
				}
				break;
			case armor:
				if(units[defenderId].weapon == tome){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == staff){
					damage = (damage * 3)/2;
					recoil = (recoil * 3)/4;
				}
				break;
			case tome:
				if(units[defenderId].weapon == staff){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == armor){
					damage = (damage * 3)/2;
					recoil = (recoil * 3)/4;
				}
				break;
			case staff:
				if(units[defenderId].weapon == armor){
					damage = (damage * 3)/4;
					recoil = (damage * 3)/2;
				}
				if(units[defenderId].weapon == tome){
					damage = (damage * 3)/2;
					recoil = (recoil * 3)/4;
				}
				break;
		}
		if(damage == 0) { damage = 1; } //can't take 0 damage, 0 recoil is ok
		
		//hp remaining after the combat
		defenderNewHP = units[defenderId].HP;
		attackerNewHP = units[attackerId].HP;
		if(damage > defenderNewHP) { defenderNewHP = 0; }
		else{ defenderNewHP-=damage;
			if(recoil > attackerNewHP){
				attackerNewHP = 0;
				//TODO: print warning that the unit will die
			}
			else{
				attackerNewHP-=recoil;
			}
		}
		
		return;
}

void ResolveCombat(uint16_t attackerId, uint16_t defenderId){
	//commit changes from calculated combat
	units[attackerId].HP = attackerNewHP;
	units[defenderId].HP = defenderNewHP;
	
	//mark the dead
	if(attackerNewHP == 0) {
		alive &= ~IdToVector(attackerId); 
		PrintTile(unitXLocations[attackerId], unitYLocations[attackerId]);
		PrintTile(unitXLocations[attackerId], unitYLocations[attackerId] - 1);
	}
	if(defenderNewHP == 0) {
		alive &= ~IdToVector(defenderId); 
		PrintTile(unitXLocations[defenderId], unitYLocations[defenderId]);
		PrintTile(unitXLocations[defenderId], unitYLocations[defenderId] - 1);
	}
}

void ChangeAttackTarget(){
	//generate a list of attackable enemies TODO: move this to run once when you enter charActionState
	//cycle through list of attackable characters + option not to attack
	uint16_t attackerId;
	uint16_t defenderId;
	//reprint cursor/info screen
	CalculateCombat(attackerId, defenderId);
}

void SelectAttackTarget(){ //confirm the attack
	uint16_t attackerId;
	uint16_t defenderId;
	
	ResolveCombat(attackerId, defenderId);
	//do combat animations??
}

void PrintMapAll(){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			PrintTile(i, j);
		}
	}
	for(int i = 0; i < numCharacters; i++){
		PrintSprite(units[i].id, unitXLocations[i], unitYLocations[i]);
	}
	UpdateInfoScreen(3);
	PrintCursor(mapX, mapY);
}

/* =========== END MAP FUNCTIONS =============*/

/* ======= FUNCTIONS FOR TEAMBUILDER =============*
* GenerateTeam, GenerateMap for a hard-coded quickstart 
* selectTeam: TODO
* previewStats: shows info screen & asks u to confirm ur choice
* addToTeam: TODO
*/

void GenerateTeam(void){ //hard coded player and enemy team
	units[0] = protagonists[0]; units[0].id = 0;
	units[1] = protagonists[6]; units[1].id = 1;
	units[2] = protagonists[5]; units[2].id = 2;
	units[3] = villains[0]; units[3].id = 3; units[3].name = (char*) villainNames[11];
	units[4] = villains[1]; units[4].id = 4; units[4].name = (char*) villainNames[17];
	units[5] = villains[5]; units[5].id = 5; units[5].name = (char*) villainNames[19];
	
	SetCharacterGraphics(0, (uint16_t *) &slance1, (uint16_t *) &slance2, (uint16_t *) &slance1face);
	SetCharacterGraphics(1, (uint16_t *) &smage1, (uint16_t *) &smage2, (uint16_t *) &smage1face);
	SetCharacterGraphics(2, (uint16_t *) &ssword3, (uint16_t *) &ssword3, (uint16_t *) &ssword3face);
	SetCharacterGraphics(3, (uint16_t *) &mmage1, (uint16_t *) &mmage1, (uint16_t *) &mmage1face);
	SetCharacterGraphics(4, (uint16_t *) &massassin1, (uint16_t *) &massassin1, (uint16_t *) &massasin1face);
	SetCharacterGraphics(5, (uint16_t *) &marmor1, (uint16_t *) &marmor1, (uint16_t *) &marmor1face);
	
	numCharacters = 6;
}

void GenerateMap(void) { //hard coded map until map select is complete
	SetMap((const uint16_t *) &templeMap);
	tilesOnMap = (const char**) &valleyArray;
	
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++){
			unitsOnMap[i][j] = -1;
		}
	}
	unitXLocations[0] = Xmin; unitYLocations[0] = Ymax; unitsOnMap[Xmin][Ymax] = 0;
	unitXLocations[1] = Xmin; unitYLocations[1] = Ymin; unitsOnMap[Xmin][Ymin] = 1;
	unitXLocations[2] = Xmin + 1; unitYLocations[2] = 4; unitsOnMap[Xmin + 1][4] = 2;
	
	unitXLocations[3] = Xmax; unitYLocations[3] = Ymax; unitsOnMap[Xmax][Ymax] = 3;
	unitXLocations[4] = Xmax; unitYLocations[4] = Ymin; unitsOnMap[Xmax][Ymin] = 4;
	unitXLocations[5] = Xmax - 1; unitYLocations[5] = 3; unitsOnMap[Xmax - 1][3] = 5;
}

 // TODO: Team Builder 
void TeambuildScroll(void){
	//
}


void BuildTeam(void){
	numCharacters = 0;
	currentState = &selectTeam;
	while(numCharacters < 3){
		//show selection of characters
		
		//choose a chracter
	}
}


void GameInit(){
	SysTick_Init();
	//cursor initialization
	mapX = 0;
	mapY = 7;
	missionScreenY = 0;
	buildTeamX = 0;
	buildTeamY = 0;
	numCharacters = 0;
}

void InfoScreenTest(){
	GenerateTeam();
	int i = 0;
	while(1){
		UpdateInfoScreen(i);
		i = (i + 1) % 6;
	}
}

void RunGame(){
	PlaySong();
	//BuildTeam();
	GenerateTeam();
	//SelectMap();
	GenerateMap();
	
	PrintMapAll();
	
	currentState = &scanMap;
	while(1){
		AnimateCharacters();
//		 SysTick_Wait10ms(500);
		
		// A and B Buttons
		int buttons = GetButtonPush();
		if((buttons & 0x2) > 0) {
			(*currentState->onB)();
		}
		else if((buttons & 0x1) > 0){
			(*currentState->onA)();
		}
		
		//Cursor Update
		int8_t dX = GetX();
		int8_t dY = GetY();
		if( (dX | dY) != 0) {
			mapOldX = mapX;
			mapOldY = mapY;
			UpdateCursor(dX, dY);
			(*currentState->onScroll)();
		}
		
		
		switch(currentState->StateNum){
		//special things for special states
			default: break;
		}
		
	}
}


/* ==== Game Flow State Machine ====
*	1st set of states: build team
* 2nd set of states: select a map
* 3rd set of states: play the map
* 4th set of states: ??? Bonus Features I guess
*/
	
const struct State selectTeam = {0, &NextState, &EmptyFunc, &TeambuildScroll };
const struct State previewStats = {1, &NextState, &UndoState, &EmptyFunc };
const struct State addToTeam = {2, &NextState, &EmptyFunc, &EmptyFunc };

const struct State viewTutorial; //push B from the chooseMap screen, TODO: select map screen
const struct State chooseMap;
const struct State initializeMap;

const struct State scanMap = {3, &ScanMapA, &EmptyFunc, &ScanMapScroll};
const struct State charSelected = {4, &CheckValidAction, &UndoState, &TentativeMove};
const struct State charActionState = {5, &SelectAttackTarget, &UndoState, &ChangeAttackTarget};
const struct State checkWin = {6, &EmptyFunc, &EmptyFunc, &EmptyFunc };
const struct State waitForEnemy = {7, &EmptyFunc, &EmptyFunc, &EmptyFunc };
const struct State checkLose = {8, &EmptyFunc, &EmptyFunc, &EmptyFunc};

/* End State Machine */
