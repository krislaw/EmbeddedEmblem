#ifndef CHAR_ACTIONS_H
#define CHAR_ACTIONS_H

#include <stdint.h>
#include <stdbool.h>

#define END_SENTINAL 0xFF
#define maxMoves 26
#define maxTargets

//end sentinal is 0xFF
extern uint8_t validMoves[2][maxMoves];
extern uint8_t validTargets[2][8];

void GetValidMoves(uint8_t x, uint8_t y, uint8_t moveAmt);
bool CheckInValidMoves(uint8_t x, uint8_t y);

void GetValidTargets(uint8_t x, uint8_t y, int16_t charGrid[8][8]);

#endif
