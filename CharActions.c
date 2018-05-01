#include "CharActions.h"
#include "GameController.h"
#include <stdlib.h>
#include <stdbool.h>

uint8_t validMoves[2][maxMoves + 1];
uint8_t validTargets[maxTargets + 1];
uint16_t numMoves;

void GetValidMoves(uint8_t x, uint8_t y, uint8_t moveAmt, uint8_t id){
	if(moveAmt > 3) { return; } //moveAmt must be 0, 1, 2 
	int16_t xmin = x;
	int16_t xmax = x + 1;
	numMoves = 0;
	bool dflag = false; //hit halfway for y
	
	for(int16_t i = (y - moveAmt); i < (y + moveAmt + 1); i++){
		for(int16_t j = xmin; j < xmax; j++){
			if(i > 0 && i < 8 && j > -1 && j < 8){
				if((char) tilesOnMap[(i*8) + j] < 2 && (unitsOnMap[j][i] == -1 || unitsOnMap[j][i] == id)){
					validMoves[0][numMoves] = j;
					validMoves[1][numMoves] = i;
					numMoves++;
				}
			}
		}
		if(i == y) { dflag = true; }
		
		if(dflag){ xmin++; xmax--; }
		else{ xmax++; xmin--; }
	}
	validMoves[0][numMoves] = END_SENTINAL;
	validMoves[1][numMoves] = END_SENTINAL;
}

//checks if given coordinates are in last generated validMove array
bool CheckInValidMoves(uint8_t x, uint8_t y){
	for(int i = 0; i < maxMoves; i++){
		if((validMoves[0][i] == x) && (validMoves[1][i] == y)){ return true; }
		if(validMoves[0][i] == 0xFF){ return false; }
	}
	return false;
}

//list of villains the unit can attack
void GetValidTargets(uint8_t x, uint8_t y, uint8_t attackerId, uint8_t range){ //possible to do: add a range
	if(range != 2) {range = 1; } //range must be 1 or 2
	int16_t xmin = x;
	int16_t xmax = x + 1;
	uint16_t numTargets = 0;
	bool dflag = false; //hit halfway for y
	
	for(int16_t i = (y - range); i < (y + range + 1); i++){
		for(int16_t j = xmin; j < xmax; j++){
			if(i > 0 && i < 8 && j > -1 && j < 8){
				if(unitsOnMap[j][i] > 2){ //is a villain
					validTargets[numTargets] = unitsOnMap[j][i];
					numTargets++;
				}
			}
		}
		if(i == y) { dflag = true; }
		
		if(dflag){ xmin++; xmax--; }
		else{ xmax++; xmin--; }
	}
	validTargets[numTargets] = END_SENTINAL;
	
}
