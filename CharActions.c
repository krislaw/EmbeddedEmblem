#include "CharActions.h"
#include "GameController.h"
#include <stdlib.h>
#include <stdbool.h>

uint8_t validMoves[2][maxMoves];
uint8_t validTargets[2][8];

// by Kris
//TODO: can't pass through enemies... that's too much work rn honestly it's gonna be a recursive mess
//either have to use recursion and ignore overlaps that are already in the array
//or do some kind of actual bfs that i can't code in 5 mins so its work

void GetValidMoves(uint8_t x, uint8_t y, uint8_t moveAmt){
	validMoves[0][0] = x;
	validMoves[1][0] = y;
	int numMoves = 1;
	for(int16_t ym = y - moveAmt; ym < (y + moveAmt + 1); ym++){ //start from the bottom
		if(ym < 8 && ym > 0){ //ignore out of bounds
			for(int16_t xm = x - moveAmt; xm < (x + moveAmt + 1); xm++){
				if(xm > - 1 && xm < 8){ //ignore out of bounds
					if(tilesOnMap[0][xm][ym] < 0x02){ //valid location!!
						if(unitsOnMap[xm][ym] == -1){ //checkin which crashed fuk
							validMoves[0][numMoves] = xm;
							validMoves[1][numMoves] = ym;
							numMoves++;
						}
					}
				}
			}
		}
	}
	validMoves[0][numMoves] = 0xFF;
	validMoves[1][numMoves] = 0xFF;
}

//checks if given coordinates are in last generated validMove array
// 0: true, 1: false
bool CheckInValidMoves(uint8_t x, uint8_t y){
	for(int i = 0; i < maxMoves; i++){
		if((validMoves[0][i] == x) && (validMoves[1][i] == y)){ return true; }
		if(validMoves[0][i] == 0xFF){ return false; }
	}
	return false;
}

//doesn't work
void getValidMoves2(uint8_t x, uint8_t y, const char** mapGrid, int16_t** charGrid, uint8_t moveAmt){
	uint8_t xCoor, yCoor;
	int moveIdx = 0;
	int i;
	int j;
	
	for(i = -moveAmt; i< moveAmt; i++){
		if(moveIdx == 25) break;
		j = 0;
		xCoor = i+x;
		if(xCoor > 7) { } //overflow, do nothing
		else{
			for(j = (- (moveAmt-i)); j < (moveAmt-i); j++){
				yCoor = j+y;
				if(yCoor > 7) { } //overflow, do nothing
				else{
					if(abs(i) + abs(j) >= moveAmt) {break;}
					if(mapGrid[xCoor][yCoor] < 2 && charGrid[xCoor][yCoor] == -1){
						validMoves[0][moveIdx] = xCoor;
						validMoves[1][moveIdx] = yCoor;
						moveIdx++;
				}
			}
			}

		}
	}
	
	//add end sentinel
	validMoves[0][moveIdx] = 0xFF;
	validMoves[1][moveIdx] = 0xFF;
}

void GetValidTargets(uint8_t x, uint8_t y, int16_t charGrid[8][8]){
	int16_t i, j;
	int16_t targetIdx = 0;
	for( i = x -1; i< x+1; i ++){
		if(targetIdx ==8) break;
		for(j = y-1; j < y+1; j++){
			if (i == x && j == y) {continue;}
			if(charGrid[i][j] == 1){
				validTargets[0][targetIdx] = i;
				validTargets[1][targetIdx] = j;
				targetIdx++;
			}
		}
	}
	
	if(targetIdx < 8){
		//add end sentinel
		validTargets[0][targetIdx] = 0xFF;
		validTargets[1][targetIdx] = 0xFF;
	}
}
