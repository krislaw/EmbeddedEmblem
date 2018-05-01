#ifndef GAME_FSM_H
#define GAME_FSM_H
#include <stdint.h>

struct State{
//	const struct State* nextState;
//	const struct State* prevState;
	int StateNum;
	void (*onA)(void);
	void (*onB)(void);
	void (*onScroll)(void);
	
};

#define sword 0
#define lance 1
#define axe 2

#define tome 4
#define staff 5
#define armor 6

//weapon triangle 
// 1) sword > axe > lance > sword
// 2) tome > armor > staff > tome
// TODO: weapon damage * 1.5, taken damage * 0.75 minimum 1

struct Unit{
	uint8_t id; //used for loading graphics, moving, ect.
	uint8_t lvl;
	char* name;
	uint8_t weapon;
	uint16_t MHP; //max hp
	uint16_t HP;
	uint16_t ATK;
	uint16_t DEF;
	uint16_t RES;
	uint16_t SPD;
	uint16_t MOV; //TODO: horses move 3, but cant go through trees
};

//TODO: balance stats bc how do?
extern const struct Unit protagonists[8];
extern const struct Unit villains[6];

extern const char villainNames[30][6];

#endif
