#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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

#define numMissions 4
#define pcVector 0x07
#define npcVector 0xF8

/* ============= Gameplay Registers ====================
* Values initialized at the beginning, stored until the game is restarted.
*/
struct Unit units[8]; //[0, 1, 2] used for player characters, [3, ... 7] used for enemies

//cursor positions for character select screen
uint8_t buildTeamIndex;
uint8_t buildOldIndex;

/* ============== Map Registers ====================
* Global Values used during fights each time a map is loaded up.
*/
uint8_t unitXLocations[8]; 	//index is character id number
uint8_t unitYLocations[8]; 	//index is character id number
int16_t unitsOnMap[8][8]; 	//-1 indicates empty, else id number of character in that space
const uint8_t (*tilesOnMap);		//8x8 array describing the terrain
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
struct Unit* selectedUnit; 		//set in ScanMap A, used in selectAttackTarget
uint16_t targetX;							//set in TentativeMove, use for ApplyTentMove
uint16_t targetY;
uint8_t attackIndex;					//set in ApplyTentMove, used in ChangeAttackTarget
uint16_t attackTargetId;			//set in ChangeAttackTarget, used in SelectAttackTarget


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
//const struct State chooseMap;
//const struct State initializeMap;
const struct State toNextMap;

const struct State scanMap;
const struct State charSelected;
const struct State charMoved;
const struct State checkWin;
const struct State waitForEnemy;
const struct State checkLose;


//declared functions for out-of-order organization
void PrintMapAll(void);
void ScanMapA(void);
void PrintOldSquare(uint8_t mx, uint8_t my);
void MoveEnemies(void);


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
		case 1:
			currentState = &selectTeam;
			PrintOnTeamBuild((const uint16_t**) proPortraits);
			ShowTeamSelectCursor(buildTeamIndex);
			break;
		case 4: 
			currentState = &scanMap;
			unitsOnMap[unitXLocations[selectedUnit->id]][unitYLocations[selectedUnit->id]] = selectedUnit->id;
			PrintMapAll(); break;
		case 5:
			mapX = unitXLocations[selectedUnit->id];
			mapY = unitYLocations[selectedUnit->id];
			currentState = &charSelected;
			PrintMapAll();
			PrintSprite(selectedUnit->id, mapX, mapY);
			break;
		default:
			break;
	}
}


void NextState(){
//	currentState = currentState->nextState;
		switch(currentState->StateNum){
			case 0:
				currentState = &previewStats; break;
			case 1:
				currentState = &addToTeam;
				break;
			case 2:
				currentState = &selectTeam;
				if((alive & pcVector) != pcVector){
					PrintOnTeamBuild((const uint16_t**) proPortraits);
					ShowTeamSelectCursor(buildTeamIndex);
				}
				break;
			case 3: currentState = &charSelected; break;
			case 4: currentState = &charMoved; break;
			case 5: currentState = &checkWin; break;
			case 6:
				if((moved & pcVector) == 0) { //all characters have moved
					currentState = &waitForEnemy;
					PrintMapAll();
					ShowWaitForServer(0);
					MoveEnemies();
					SysTick_Wait10ms(10);
				}
				else{
					currentState = &scanMap;
					PrintMapAll();
				}
				break;
			case 7: currentState = &checkLose; break;
			case 8:
				currentState = &scanMap;
				moved = alive & pcVector;
				break;
			case 9: break;
			case 10: break;
			case 11: break;
		default:
			break; //cannot go back
	}
	ClearButtonPush(); //in case of lag
}

/*LevelUp - finished, test for balance*/
void LevelUp(uint8_t i){
		units[i].MHP += 2;
		units[i].HP = units[i].MHP;
		units[i].ATK += 3;
		units[i].DEF += 2;
		units[i].RES += 2;
		units[i].SPD += (units[i].SPD * 2)/10 + 1;
}

/* checkLose state -  */
void CheckLose(){
	if((alive & pcVector) == 0){
			StopSong();
			ShowLoseScreen();
			SetSong(5);
			PlaySong();
			SysTick_Wait10ms(100);
			while(GetButtonPush() == 0) {}
			special |= loseVector;
	}
	else{
		return;
	}
}

/* checkWin state - finished, TEST */
void CheckWin(){
		if((alive & npcVector) == 0){
			StopSong();
			SetSong(6);
			PlaySong();
			ShowWinScreen();
			while(GetButtonPush() == 0) {}
			special |= winVector;
	}
	else{
		return;
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

/* PrintOldSquare - reprint old squares and overlapped characters */
void PrintOldSquare(uint8_t mx, uint8_t my){
	//reprint old squares
	PrintTile(mx, my);
	PrintMoveTile(mx, my);
	//reprint people if you stood behind them
	if(unitsOnMap[mx][my] > -1){
		PrintSprite(unitsOnMap[mx][my], mx, my);
	}
	int i = 1;
	while(unitsOnMap[mx][my + i] > -1){
		PrintSprite(unitsOnMap[mx][my + i], mx, my + i);
		i++;
		if((my + i) > 7) { break; }
	}
	//reprint squares you stood in front of
	PrintTile(mx, my - 1);
	if(CheckInValidMoves(mx, my - 1)){
		PrintMoveTile(mx, my - 1);
	}
	if(unitsOnMap[mx][my - 1] > -1){
		PrintSprite(unitsOnMap[mx][my - 1], mx, my -1);
	}	
}


/* ======= FUNCTIONS FOR MAP & MAP STATES =============*
* UpdateCursor(dX, dY), UpdateInfoScreen(character id)
* scanMap: onA, onScroll
* charSelected: ApplyTentMove (A), TentativeMove (Scroll),
* charMoved: SelectAttack Target (A), ChangeAttackTarget (Scroll), Calculate Combat for preview
* waitForEnemy: idle and wait for the server i guess
*/

void UpdateCursor(int8_t dX, int8_t dY){
	mapOldX = mapX;
	mapOldY = mapY;
	//change cursor location, NO PRINTING HERE!!!	
	switch(currentState->StateNum){
		case 0: //selectTeam scroll
			buildOldIndex = buildTeamIndex;
			if(buildTeamIndex < 4 && dX > 0) { buildTeamIndex = buildTeamIndex + 4; }
			if(buildTeamIndex > 3 && dX < 0) { buildTeamIndex = buildTeamIndex - 4; }
			if((buildTeamIndex %  4) < 3 && dY > 0) { buildTeamIndex = buildTeamIndex + 1; }
			if((buildTeamIndex % 4) > 0 && dY < 0) { buildTeamIndex = buildTeamIndex - 1; }
			if(buildTeamIndex > 7) { buildTeamIndex = 0; } //if I fucked up
		case 3: //scanMap scroll
			if(mapX == Xmin && dX < 0){ mapX = Xmin; }
			else if(mapX == Xmax && dX > 0){ mapX = Xmax; }
			else{mapX = mapX + dX; }
	
			if(mapY == Ymin && dY < 0){ mapY = Ymin; }
			else if(mapY == Ymax && dY > 0){ mapY = Ymax; }
			else{mapY = mapY + dY; }
			break;
		case 4: //charSelected scroll
			if(CheckInValidMoves(mapX + dX, mapY + dY)){ //should contain no out of bounds movement
				mapX = mapX + dX;
				mapY = mapY + dY;
				break;
			}
			else if(CheckInValidMoves(mapX + 2*dX, mapY + dY)){ //should contain no out of bounds movement
				mapX = mapX + 2*dX;
				mapY = mapY + dY;
				break;
			}
			else if(CheckInValidMoves(mapX + dX, mapY + 2*dY)){ //should contain no out of bounds movement
				mapX = mapX + dX;
				mapY = mapY + 2*dY;
			}
			break;
		case 5: //charMoved - iterate through list of enemies
			if(dY > 0 && validTargets[attackIndex] != 0xFF) {
				attackIndex++;
			}
			else if(dY < 0 && attackIndex > 0) {
				attackIndex--;
			}
			else if(dY > 0 && validTargets[attackIndex] == 0xFF){
				attackIndex = 0;
			}
		default: return;
	}
}

void UpdateInfoScreen(uint8_t id){
		ShowInfo(units[id].name, id, units[id].lvl,
			units[id].HP, units[id].MHP, units[id].ATK, units[id].DEF,
			units[id].RES, units[id].SPD);
}

/* scanMap state A - finished & verified */
void ScanMapA(){
	//select a char
	if((unitsOnMap[mapX][mapY] > -1) && (unitsOnMap[mapX][mapY] < 3)){
		selectedUnit = &units[unitsOnMap[mapX][mapY]];
		if((IdToVector(selectedUnit->id) & moved) == 0){
			return; //character has already moved this turn
		}
		unitsOnMap[mapX][mapY] = -1;
	}
	else{ //not a valid character
		return; 
	}
	//generate movement grid on global: validMoves
	GetValidMoves(mapX, mapY, selectedUnit->MOV, selectedUnit->id);
	int16_t i = 0;
	while(validMoves[0][i] != END_SENTINAL){
		PrintMoveTile(validMoves[0][i], validMoves[1][i]);
		i++;
	};
	NextState();
}

/* scanMap state onScroll - finished & verified */
void ScanMapScroll(){
	//reprint map square
	PrintTile(mapOldX, mapOldY);
	//reprint characters
	if(unitsOnMap[mapOldX][mapOldY] > -1){
		PrintSprite(unitsOnMap[mapOldX][mapOldY], mapOldX, mapOldY);
	}
	int i = 1;
	while(unitsOnMap[mapOldX][mapOldY + i] > -1){
		PrintSprite(unitsOnMap[mapOldX][mapOldY + i], mapOldX, mapOldY + i);
		i++;
		if((mapOldY + i) > 7) { break; }
	}
	//print cursor
	PrintCursor(mapX, mapY);
	//check & update info window
	if(unitsOnMap[mapX][mapY] > -1){
		UpdateInfoScreen(unitsOnMap[mapX][mapY]);
	}
}

/* ApplyTentMove() */
void ApplyTentMove(){
	//set global for the selected characters location
	targetX = mapX;
	targetY = mapY;
	
	GetValidTargets(targetX, targetY, selectedUnit->id, 1); //TODO: add range to character stats
	//initialize for attack targets
	attackIndex = 0;
	while(validTargets[attackIndex] != END_SENTINAL){ attackIndex++; }
	ShowNonCombat();
	//go to action state
	NextState();
}

/* Tenatative Move - scroll characters move - finished, need to test */
void TentativeMove(){
	//Potential Bug Notice: UpdateCursor should keep in bounds pls
	PrintOldSquare(mapOldX, mapOldY);
	PrintCursor(mapX, mapY);
	PrintSprite(selectedUnit->id, mapX, mapY);
}

//Preview values calculated from combat

/* Calculate and Resolve Combat - finished, need to test */
uint16_t defenderNewHP;
uint16_t attackerNewHP;

void CalculateCombat(uint16_t attackerId, uint16_t defenderId){
	//damage done to each unit
	int16_t damage = units[attackerId].ATK;
	int16_t recoil = units[defenderId].ATK;
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
			default: recoil -= units[attackerId].DEF;
		}
		//weapon triangle 
// 1) sword > axe > lance > sword
// 2) tome > armor > staff > tome
		switch(units[attackerId].weapon){
			case sword:
				if(units[defenderId].weapon == lance){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == axe){
					damage = (damage * 4)/3;
					recoil = (recoil * 3)/4;
				}
				break;
			case lance:
				if(units[defenderId].weapon == axe){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == sword){
					damage = (damage * 4)/3;
					recoil = (recoil * 3)/4;
				}
				break;
			case axe:
				if(units[defenderId].weapon == sword){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == lance){
					damage = (damage * 4)/3;
				}
				break;
			case armor:
				if(units[defenderId].weapon == tome){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == staff){
					damage = (damage * 4)/3;
					recoil = (recoil * 3)/4;
				}
				break;
			case tome:
				if(units[defenderId].weapon == staff){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == armor){
					damage = (damage * 4)/3;
					recoil = (recoil * 3)/4;
				}
				break;
			case staff:
				if(units[defenderId].weapon == armor){
					damage = (damage * 3)/4;
					recoil = (damage * 4)/3;
				}
				if(units[defenderId].weapon == tome){
					damage = (damage * 4)/3;
					recoil = (recoil * 3)/4;
				}
				break;
		}
		
		if(damage < 1) { damage = 1; } //can't take 0 damage, 0 recoil is ok
		if(recoil < 1) { recoil = 0; } 
		
		//hp remaining after the combat
		defenderNewHP = units[defenderId].HP;
		attackerNewHP = units[attackerId].HP;
		if(damage > defenderNewHP) { defenderNewHP = 0; }
		else{ defenderNewHP-=damage;
			if(recoil > attackerNewHP){
				attackerNewHP = 0;
			}
			else{
				attackerNewHP-=recoil;
			}
		}
		if(defenderNewHP == 0 && attackerNewHP == 0){ //small bug fix??
				attackerNewHP = units[attackerId].HP;
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
		unitsOnMap[unitXLocations[attackerId]][unitYLocations[attackerId]] = -1;
		PrintOldSquare(targetX, targetY);
	}
	if(defenderNewHP == 0) {
		alive &= ~IdToVector(defenderId); 
		unitsOnMap[unitXLocations[defenderId]][unitYLocations[defenderId]] = -1;
		PrintOldSquare(unitXLocations[defenderId], unitYLocations[defenderId]); 
	}
}


/*Change Attack Target - to check */
void ChangeAttackTarget(){
	//iterate through validTargets, preview results of combat
	if(validTargets[attackIndex] == END_SENTINAL) {
		ShowNonCombat();
	}
	else{
		uint8_t TargetId = validTargets[attackIndex];
		CalculateCombat(selectedUnit->id, TargetId);
		ShowCombatPreview(units[TargetId].name, 	defenderNewHP, 	units[TargetId].MHP,
		selectedUnit->name, attackerNewHP, 	selectedUnit->MHP, selectedUnit->id);
	}
}

/* Select Attack Target - */
void SelectAttackTarget(){	
	//update unit's location in global, can't undo
	moved &= ~(IdToVector(selectedUnit->id));
	unitsOnMap[unitXLocations[selectedUnit->id]][unitYLocations[selectedUnit->id]] = -1; 
	unitsOnMap[mapX][mapY] = selectedUnit->id;
	unitXLocations[selectedUnit->id] = mapX;
	unitYLocations[selectedUnit->id] = mapY;
	
	//write back combat changes
	if(validTargets[attackIndex] != END_SENTINAL){
		ResolveCombat(selectedUnit->id, validTargets[attackIndex]);
	}
	
	//TODO: combat animations?
	PrintOldSquare(mapOldX, mapOldY);
	NextState();
}

void PrintMapAll(){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			PrintTile(i, j);
			if(unitsOnMap[i][j] > -1){ //print units
				PrintSprite(unitsOnMap[i][j], unitXLocations[unitsOnMap[i][j]], unitYLocations[unitsOnMap[i][j]]);
			}
		}
	}
	UpdateInfoScreen(selectedUnit->id);
	if(currentState->StateNum == 4){ //move grid
			uint16_t i = 0;
			while(validMoves[0][i] != END_SENTINAL){
				PrintMoveTile(validMoves[0][i], validMoves[1][i]);
				i++;
		}
	}
	PrintCursor(mapX, mapY);
}

/* =========== END MAP FUNCTIONS =============*/

/* ======= FUNCTIONS FOR TEAMBUILDER =============*
* Build Team initializes this FSM
* GenerateTeam, GenerateMap for a hard-coded quickstart 
* TeamBuildScroll: handles scrolling through 8 protags
* previewStats: shows info screen & asks u to confirm ur choice
* addToTeam: TODO
*/

/*Generate Team - hard coded player and enemy team */
void GenerateTeam(void){ 
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
	alive = 0x3F;
	moved = 0x7;
}
/* GenerateMap - hard coded map until map select is complete */
void GenerateMap(void) { 
	SetMap((const uint16_t *) &ruinMap);
	tilesOnMap = (const uint8_t *) &ruinArray;
	currentState = &scanMap;
	
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++){
			unitsOnMap[i][j] = -1;
		}
	}
	unitXLocations[0] = 3; unitYLocations[0] = 3; unitsOnMap[3][3] = 0;
	unitXLocations[1] = 3; unitYLocations[1] = 4; unitsOnMap[3][4] = 1;
	unitXLocations[2] = 3; unitYLocations[2] = 5; unitsOnMap[3][5] = 2;
	
	unitXLocations[3] = 5; unitYLocations[3] = 3; unitsOnMap[5][3] = 3;
	unitXLocations[4] = 4; unitYLocations[4] = 4; unitsOnMap[4][4] = 4;
	unitXLocations[5] = 5; unitYLocations[5] = 5; unitsOnMap[5][5] = 5;
}

/* BuildTeam - add new units at beginning of game and if units die */
const struct Unit * previewUnit; //used for teambuild, use id 3 for graphics
void BuildTeam(void){
	currentState = &selectTeam;
	PrintOnTeamBuild((const uint16_t**) proPortraits);//
	ShowTeamSelectCursor(buildTeamIndex);
	previewUnit = &protagonists[0];
	SetCharacterGraphics(3, (uint16_t *) proSpritesA[0], (uint16_t *) proSpritesB[0], (uint16_t *) proPortraits[0]);
}

/* Teambuild Scroll scroll through available units */
void TeambuildScroll(void){
	HideTeamSelectCursor(buildOldIndex);
	ShowTeamSelectCursor(buildTeamIndex);
	previewUnit = &protagonists[buildTeamIndex];
	SetCharacterGraphics(3, (uint16_t *) proSpritesA[buildTeamIndex], (uint16_t *) proSpritesB[buildTeamIndex], (uint16_t *) proPortraits[buildTeamIndex]);
}
/* Preview Unit - See stats before selecting */
void PreviewUnit(){
	ShowPreview(previewUnit->name, previewUnit->weapon, previewUnit->MHP,
	previewUnit->ATK, previewUnit->DEF, previewUnit->RES,	previewUnit->SPD);
	NextState();
}


/* Add Unit - confirm add to team, cannot undo */
void AddUnit(){
	if((alive & pcVector) == pcVector) { while(1) {} } 
	for(uint8_t i = 0; i < 3; i++){
		if( (IdToVector(i)&alive) == 0){
			units[i] = protagonists[buildTeamIndex];
			units[i].id = i;
			SetCharacterGraphics(i, (uint16_t *) proSpritesA[buildTeamIndex], (uint16_t *) proSpritesB[buildTeamIndex],  (uint16_t *) proPortraits[buildTeamIndex]); //
			alive |= IdToVector(i);
			i = 3; 
		}
	}
	NextState();
}

/*Game Init - sets some variables to prepare for gameplay*/
void GameInit(){
	SysTick_Init();
	//cursor initialization
	mapX = 0;
	mapY = 7;
	buildTeamIndex = 0;
	special = 0;
	alive = 0;
}

/* Mission Init - clean registers and initialize mission */
void MissionInit(uint8_t missionId){
	mapX = 0;
	mapY = 7;
	buildTeamIndex = 0;
	special = 0;
	
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++){
			unitsOnMap[i][j] = -1;
		}
	}
		
	uint8_t* startLocation;	
	switch(missionId){ //tried hard not to hardcode this but pointer types conflict too much
		case 1:
			SetMap((const uint16_t*) &valleyMap);
			tilesOnMap = (const uint8_t *) &valleyArray;
			numCharacters = 6;
			startLocation = (uint8_t *) &valleyStart;
			break;
		case 2:
			SetMap((const uint16_t*) &templeMap);
			tilesOnMap = (const uint8_t *) &templeArray;
			numCharacters = 7;
			startLocation = (uint8_t *) &templeStart;
			break;
		case 3:
			SetMap((const uint16_t*) (uint8_t *) &ruinMap);
			tilesOnMap = (const uint8_t *) &ruinArray;
			numCharacters = 8;
			startLocation = (uint8_t *) &ruinStart;
			break;
		default:
			SetMap((const uint16_t*) &desertMap);
			tilesOnMap = (const uint8_t *) &desertArray;
			numCharacters = 6;
			startLocation = (uint8_t *) &desertStart;
			break;
	}
	
	for(int i = 3; i < numCharacters; i++){
		uint8_t indx = rand() % 6;
		units[i] = villains[indx];
		units[i].id = i;
		SetCharacterGraphics(i, (uint16_t *) vilSpritesA[indx], (uint16_t *) vilSpritesB[indx], (uint16_t *) vilPortraits[indx]);
		units[i].name = (char*) villainNames + ((rand() % 30) * 6);
	}
	
	for(int i = 0; i < numCharacters; i++){
		unitXLocations[i] = startLocation[i * 2];
		unitYLocations[i] = startLocation[(i * 2) + 1];
		unitsOnMap[unitXLocations[i]][unitYLocations[i]] = i;
		alive |= (1 << i);
	}
	
	//increase enemy difficulty on later missions
	for(uint8_t j = 0; j < missionId; j++){
			for(uint8_t k = 3; k < numCharacters; k++){
				LevelUp(k);
			}
	}
		
	moved = pcVector; //mark all units not moved
	currentState = &scanMap;
	selectedUnit = &units[3];
	PrintMapAll();
}

void InfoScreenTest(){
	GenerateTeam();
	int i = 0;
	while(1){
		UpdateInfoScreen(i);
		i = (i + 1) % 6;
	}
}

/* Enemy Movement Logic - replacing the server */
uint8_t AbsoluteDistance(int16_t dX, int16_t dY){
	if(dX < 0) { dX *= (-1); }
	if(dY < 0) { dY *= (-1); }
	return dY + dX;
}

void MoveEnemies(){
	struct Unit* weakest;
	uint16_t lowestHP = 99;
	for(int i = 0; i < 3; i++){ //group will move toward weakest character
		if(units[i].HP > 0 && units[i].HP < lowestHP){ //alive and weaker
			weakest = &units[i];
			lowestHP = units[i].HP;
		}
	}
	for(int i = 3; i < numCharacters; i++){ //closest move towards the enemy
		if((IdToVector(i) & alive) != 0){ //only move living enemies
			GetValidMoves(unitXLocations[units[i].id], unitYLocations[units[i].id], units[i].MOV, units[i].id);
			uint8_t bestX;
			uint8_t bestY;
			uint8_t closest = 16;
			for(int j = 0; validMoves[0][j] != END_SENTINAL; j++){
				uint8_t distance = AbsoluteDistance(unitXLocations[weakest->id] - validMoves[0][j], unitYLocations[weakest->id] - validMoves[1][j]);
				if(distance < closest){
					bestX = validMoves[0][j];
					bestY = validMoves[1][j];
					closest = distance;
				}
			} //end for J, generating best move
			//move the Unit
			unitsOnMap[unitXLocations[units[i].id]][unitYLocations[units[i].id]] = -1;
			unitsOnMap[bestX][bestY] = units[i].id;
			unitXLocations[units[i].id] = bestX;
			unitYLocations[units[i].id] = bestY;
		}
	}
	PrintMapAll(); //show enemies on Map
	ShowWaitForServer(2);
	while(GetButtonPush() == 0) {}
	for(int i = 3; i < numCharacters; i++){
		if((IdToVector(i) & alive) != 0){
			ShowWaitForServer(0);
			//choose a target
			uint8_t targetHeroId = 0;
			bool combat = false; //can't attack anyone
			//check the cardinal directions
			const int8_t xxa[] = { 0, 0, 1, -1 };
			const int8_t yya[] = { 1, -1, 0, 0 };
			for(int j = 0; j < 4; j++){
					uint8_t xx = unitXLocations[i] + xxa[j];
					uint8_t yy = unitYLocations[i] + yya[j];
					if(xx < 7 && yy < 7){
						if(unitsOnMap[xx][yy] > -1 && unitsOnMap[xx][yy] < 3){ //hero found!
							combat = true;
							targetHeroId = unitsOnMap[xx][yy];
					}
				}
			} //end direction checking
			
			if(combat){
				//combat preview
				CalculateCombat(i, targetHeroId);
				ShowCombatPreview(units[targetHeroId].name, defenderNewHP, units[targetHeroId].MHP, units[i].name, attackerNewHP, units[i].MHP, i);
				while(GetButtonPush() == 0) {}
					//resolve combat
					ResolveCombat(i, targetHeroId);
					PrintMapAll(); //TODO: smaller graphics fixes
			}
		}
	}
	
		/*	//bug fix: just always check for kills because I don't have time to fix this
		for(int i = 3; i < numCharacters; i++){
			if(units[i].HP < 1){
				if(unitsOnMap[unitXLocations[i]][unitYLocations[i]] == (int16_t) units[i].id){ //signed compare
					unitsOnMap[unitXLocations[i]][unitYLocations[i]] = -1;
				}
				alive &= IdToVector(i);
			}
		}*/
}

	/* ======== RUN FROM GAME'S MAIN WHILE LOOP ============
	*/
void RunStates() {
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
			UpdateCursor(dX, dY);
			(*currentState->onScroll)();
		}	
		switch(currentState->StateNum){
		//special things for special states
			case 7: //Wait For Enemy
				ShowWaitForServer(1);
				NextState();
				break;
			case 6: //Check Win
			case 8: //Check Lose
				CheckWin();
				CheckLose();
				NextState();
				break;
			default: break;
		} //end switch
}

/* ================= Run Game ===================
* This is the main for all of our software in the game
* It loops until you lose or beat the whole game
*
*/

void RunGame(){
	for(int i = 0; i < numMissions; i++){
		if((alive& pcVector) < pcVector) { //slow on printing, so don't unless we have to
		BuildTeam();
			while((alive & pcVector) < pcVector){
				RunStates();
			}
		}
		ShowStory(i);
		while(GetButtonPush() == 0);
		MissionInit(i);

		// debug
		/*
		alive = 0x3F;
		GenerateTeam();
		GenerateMap();
		for(int i = 0; i < 8; i++){ //check Lose testing
			LevelUp(3);
			LevelUp(4);
			LevelUp(5);
		}
		PrintMapAll();
		ClearButtonPush(); 
		*/
		// end debug
		SetSong(i);
		PlaySong();
		while((special & (winVector | loseVector)) == 0){
			RunStates();
		}
		StopSong();
		if((special & loseVector) > 0){
			i = -1; //restart game, everyone died
		}
		else{
		//reach here if you won the mission
			while(GetButtonPush() == 0) { }
			LevelUp(0);
			LevelUp(1);
			LevelUp(2);
			if(i > 1){ //level up twice for harder missions
				LevelUp(0);
				LevelUp(1);
				LevelUp(2);
			}
		}
	}
	ShowStory(4); //final screen
	while(1) { }
}

/* ==== Game Flow State Machine ====
*	1st set of states: build team
* 2nd set of states: play mission
* 3rd set of states: ??
*/
	
const struct State selectTeam = {0, &PreviewUnit, &EmptyFunc, &TeambuildScroll };
const struct State previewStats = {1, &NextState, &UndoState, &EmptyFunc };
const struct State addToTeam = {2, &AddUnit, &EmptyFunc, &EmptyFunc };

const struct State scanMap = {3, &ScanMapA, &EmptyFunc, &ScanMapScroll};
const struct State charSelected = {4, &ApplyTentMove, &UndoState, &TentativeMove};
const struct State charMoved = {5, &SelectAttackTarget, &UndoState, &ChangeAttackTarget};
const struct State checkWin = {6, &EmptyFunc, &EmptyFunc, &EmptyFunc };
const struct State waitForEnemy = {7, &EmptyFunc, &EmptyFunc, &EmptyFunc };
const struct State checkLose = {8, &EmptyFunc, &EmptyFunc, &EmptyFunc};

/* End State Machine */
