#include "CharActions.h"
#include <stdlib.h>

uint8_t validMoves[2][25];
uint8_t validTargets[2][8];

void getValidMoves(uint8_t x, uint8_t y, uint16_t mapGrid[7][7], uint16_t charGrid[7][7], uint8_t moveAmt){
	uint8_t xCoor, yCoor;
	int moveIdx = 0;
	int i;
	int j;
	
	for(i = -moveAmt; i< moveAmt; i++){
		if(moveIdx == 25) break;
		j = 0;
		xCoor = i;
		for(j = (- (moveAmt-i)); j < (moveAmt-i); j++){
			yCoor = j;
			if(abs(i) + abs(j) >= moveAmt) {break;}
			if(mapGrid[xCoor][yCoor] == 0 && charGrid[xCoor][yCoor] == 0){
				validMoves[0][moveIdx] = xCoor;
				validMoves[1][moveIdx] = yCoor;
				moveIdx++;
			}
		}
	}
	
	if(moveIdx<25){
		//add end sentinel
		validMoves[0][moveIdx] = 0xFF;
		validMoves[0][moveIdx] = 0xFF;
	}
}

void getValidTargets(uint8_t x, uint8_t y, uint16_t charGrid[7][7]){
	int i, j;
	int targetIdx = 0;
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
