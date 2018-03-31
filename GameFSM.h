
#include <stdint.h>

struct State{
	struct State* nextState;
	struct State* prevState;
	void (*onA)(void);
	void (*onB)(void);
	void (*onScroll)(void);
	
}State;

#define sword 1
#define lance 2
#define axe 3

#define tomea 4
#define tomeb 5
#define tomec 6

#define bow 7

struct Unit{
	uint8_t id;
	char name[16];
	uint8_t weapon;
	uint16_t MHP; //max hp
	uint16_t HP;
	uint16_t ATK;
	uint16_t DEF;
	uint16_t RES;
	uint16_t SPD;
}Unit;