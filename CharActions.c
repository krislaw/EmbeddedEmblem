#include "CharActions.h"
#include "GameController.h"
#include "GameFSM.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h> 

uint8_t validMoves[2][maxMoves + 1];
uint8_t validTargets[maxTargets + 1];
uint16_t numMoves;

uint8_t enemyMove[2];

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
	uint8_t targetVal;
	if(unitsOnMap[x][y] == PLAYER_VAL){
		targetVal = ENEMY_VAL;
	}
	else{ 
		targetVal = PLAYER_VAL;
	}
	
	int16_t xmin = x;
	int16_t xmax = x + 1;
	uint16_t numTargets = 0;
	bool dflag = false; //hit halfway for y
	
	for(int16_t i = (y - range); i < (y + range + 1); i++){
		for(int16_t j = xmin; j < xmax; j++){
			if(i > 0 && i < 8 && j > -1 && j < 8){
				if(unitsOnMap[j][i] == targetVal){ //on the other team
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

void FindEnemyWithNearestTarget(){
	uint8_t minDist = 10;

    uint8_t minServerIdx, minClientIdx;
    uint8_t i;
    uint8_t j;
		//find enemy who is closest to client
    for(i = 4; i< 8; i++){
			if(units[i].HP<1) continue;
        for (j = 0; j < 4; j++){
						if(units[j].HP<1) continue;
            uint8_t dist = pow(unitXLocations[i] - unitXLocations[j], 2) +pow(unitYLocations[i] - unitXLocations[j], 2);
            dist = sqrt(dist);
            if(dist<minDist){
                minDist = dist;
                minServerIdx = i;
                minClientIdx = j;
               
            }
        }
    }
		enemyMove[0]= minServerIdx;
		enemyMove[1] = minClientIdx;
}

void MoveEnemyToTarget(){
		uint8_t enemyIdx = enemyMove[0];
		uint8_t targetIdx = enemyMove[1];
    
	
    uint8_t distX = unitXLocations[enemyIdx] - unitXLocations[targetIdx];
    uint8_t distY = unitYLocations[enemyIdx]  - unitXLocations[targetIdx];
	
    //move closer to client
    uint8_t newX = unitXLocations[enemyIdx];
    uint8_t newY = unitYLocations[enemyIdx];
		
		
    for(int i =0; i< units[enemyIdx].MOV; i++){
        if(abs(distX) > 0){
            int8_t unit = distX/distX;
            if((unit < 0 && newX >0) || (unit > 0 && newX < 7)){
                if(unitsOnMap[newX+unit][newY]) break; //if non zero occupied
                newX += unit;
                distX -= unit;
            }
        }else if (abs(distY) >0){
            int8_t unit = distY/distY;
            if ((unit < 0 && newY >0) || (unit > 0 && newY<7)){
                if(unitsOnMap[newX][newY+unit]) break;//if non zero, occupied
                newY += unit;
                distY-= unit;
            }
        }else{ break;}
    }
		//save new location
		unitsOnMap[unitXLocations[enemyIdx]][unitYLocations[enemyIdx]] = 0x00;
		unitsOnMap[newX][newY] = 0xF1;
		unitXLocations[enemyIdx] = newX;
		unitYLocations[enemyIdx] = newY;
		 
}
