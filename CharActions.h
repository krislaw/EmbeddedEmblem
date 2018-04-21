#ifndef CHAR_ACTIONS_H
#define CHAR_ACTIONS_H

#include <stdint.h>

#define END_SENTINAL 0xFF

//end sentinal is 0xFF
extern uint8_t validMoves[2][25];
extern uint8_t validTargets[2][8];

void getValidMoves(uint8_t x, uint8_t y, int16_t mapGrid[8][8], int16_t charGrid[8][8], uint8_t moveAmt);

void getValidTargets(uint8_t x, uint8_t y, int16_t charGrid[8][8]);

#endif
