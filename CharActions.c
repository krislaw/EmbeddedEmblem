#include "CharActions.h"
#include <stdlib.h>

uint8_t validMoves[2][26];
uint8_t validTargets[2][8];

void getValidMoves(uint8_t x, uint8_t y, const char** mapGrid, int16_t** charGrid, uint8_t moveAmt){
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

void getValidTargets(uint8_t x, uint8_t y, int16_t charGrid[8][8]){
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
