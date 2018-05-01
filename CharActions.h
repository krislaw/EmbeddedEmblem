#ifndef CHAR_ACTIONS_H
#define CHAR_ACTIONS_H

#include <stdint.h>
#include <stdbool.h>

#define END_SENTINAL 0xFF
#define maxMoves 25
#define maxTargets 4

#define ENEMY_VAL 0xF1
#define PLAYER_VAL 0x1

//end sentinal is 0xFF
extern uint8_t validMoves[2][maxMoves + 1]; //list of spaces a selected char can move to
extern uint8_t validTargets[maxTargets + 1]; //list of ids the character can attack

void GetValidMoves(uint8_t x, uint8_t y, uint8_t moveAmt, uint8_t id);
bool CheckInValidMoves(uint8_t x, uint8_t y);

void GetValidTargets(uint8_t x, uint8_t y, uint8_t attackerId, uint8_t range);

//returns id of enemy moved
uint8_t MoveEnemy(void);

#endif
