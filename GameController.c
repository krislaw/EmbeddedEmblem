#include <stdint.h>
#include "GameController.h"
#include "Graphics.h"
#include "Sound.h"
#include "GameFSM.h"
#include "Input.h"

#define Xmax 8
#define Ymax 7

uint16_t Xpos;
uint16_t Ypos;
struct State* currentState;

uint16_t alive; //one - hot for live
uint16_t moved;	//one - hot for movment, todo: reset at top of turn

uint16_t special; // win, lose, ect.

void EmptyFunc(){
	return;
}

void CheckWin(){
}

void UpdateCursor(int8_t dX, int8_t dY){
	PrintTile(Xpos, Ypos);
	//check if move tiles are active, reprint if applicable
	//check character position, reprint if applicatble
	
	if(Xpos == 0 && dX < 0){ Xpos = 0; }
	else if(Xpos == Xmax && dX > 0){ Xpos = Xmax; }
	else{Xpos = Xpos + dX; }
	
	if(Ypos == 0 && dY < 0){ Ypos = 0; }
	else if(Ypos == Ymax && dY > 0){ Ypos = Ymax; }
	else{Ypos = Ypos + dY; }
	
}
void CheckLose(){
	if(alive == 0){
		
	}
}

void UndoState(){
	//clear any between state flags
	currentState = currentState->prevState;
}

void NextState(){
	currentState = currentState->nextState;
}

void ScanMapA(){
	//select a char
	//generate move grid
	//lock info screen
}

void ScanMapScroll(){
	//reprint map square
	//print cursor
	//check & update info window
}

void CheckValidAction(){
	//check if move is valid
	//go to action state
}

void TentativeMove(){
	//reprint move grid
	//print cursor
}

void ResolveCombat(uint16_t attackerId, uint16_t defenderId){
	//calculate the difference in stats
	
	//if defender = pc, checkLose
	//if defencer = npc, checkWin
	
}

void ChangeAttackTarget(){
	//generate a list of attackable enemies
	//cycle through them + option not to attack
	//reprint cursor/info screen
}

void SelectAttackTarget(){
	//use character from change attack target
	uint16_t attackerId;
	uint16_t defenderId;
	
	ResolveCombat(attackerId, defenderId);
	//do combat animations??
}




void FSMInit(){
	struct State scanMap;
	scanMap.onA = &ScanMapA;
	scanMap.onB = &EmptyFunc;
	scanMap.onScroll = &ScanMapScroll;
	
	struct State charSelected;
	charSelected.onA = &CheckValidAction;
	charSelected.onB = &UndoState;
	charSelected.onScroll = &TentativeMove;

	struct State charActionState;
	charActionState.onA = &SelectAttackTarget;
	charActionState.onB = &UndoState;
	charActionState.onScroll = &ChangeAttackTarget;
	
	struct State waitForEnemy;
  waitForEnemy.onA = &EmptyFunc;
	
	
	scanMap.nextState = &charSelected;
	charSelected.prevState = &scanMap;
	charSelected.nextState = &charActionState;
	
}

void CharacterInit(void){
	
}

void GameInit(){
	FSMInit();
	CharacterInit();
}


void RunGame(){
	while(1){
		// AnimateCharacters();
		// SysticWait10ms(1000);
		
		// AB Buttons
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
		if( (dX | dY) > 0) {
			UpdateCursor(dX, dY);
		}
		
	}
}

