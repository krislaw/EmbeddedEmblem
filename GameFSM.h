#include <stdint.h>

#define maxEnemies 5
#define winVector 0x10
#define loseVector 0x01

struct State{
//	const struct State* nextState;
//	const struct State* prevState;
	int StateNum;
	void (*onA)(void);
	void (*onB)(void);
	void (*onScroll)(void);
	
}State;

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
}Unit;

/* ===== BALANCING =====
* Level 1 Min Damage 2, Max Damage 9 without multipliers
* HP is 16 to 26
* Attack is 15 to 18
* Def/Res is 9 to 13
* Speed { not used }
* MOV 1 to 3
* Range 1 to 2 { not implemented }
*/

const struct Unit protagonists[8] = {
	{0, 1, "Kris", lance, 20, 20, 17, 9, 11, 8, 2}, 			//use slance1
	{0, 1, "Kyle", lance, 18, 18, 18, 10, 10, 10, 3},	 		//use slance3
	{0, 1, "Hanah", staff, 17, 17, 16, 10, 10, 13, 3},	 	//use scleric1
	{0, 1, "Max", axe, 17, 17, 18, 12, 9, 12, 2}, 					//use saxe1
	{0, 1, "Jen", sword, 22, 22, 16, 11, 11, 6, 2},					//use ssword1
	{0, 1, "Brand", sword, 16, 16, 18, 9, 10, 6, 2},				//use ssword3
	{0, 1, "Steph", tome, 17, 17, 17, 10, 12, 9, 2},			//use smage1
	{0, 1, "Ali", armor, 26, 26, 16, 13, 9, 4, 1}					//use sarmor1
};

const struct Unit villains[6] = { //TODO: add lance options
	{3, 1, "", armor, 25, 25, 15, 13, 10, 6, 1},			//marmor1
	{3, 1, "", axe, 21, 21, 18, 11, 9, 10, 2},				//maxe3
	{3, 1, "", staff, 18, 18, 15, 12, 10, 8, 2},			//mcleric1
	{3, 1, "", tome, 20, 20, 16, 9, 11, 10, 2},				//mmage1
	{3, 1, "", sword, 16, 16, 18, 9, 9, 15, 2},				//mthief1
	{3, 1, "", sword, 19, 19, 16, 12, 9, 6, 2}				//mASSassin1
};

const char villainNames[30][6] = { //randomly generate villain names, shoutout to SEC Eweek 2018 for theme
	"Zeus", "Hera", "Poise", "Demtr", "Ares",
	"Athen", "Armis", "Heph", "Dite", "Eres",
	"Dions", "Hades", "Hypno", "Nike", "Janus",
	"Nemis", "Iris", "Hecat", "Tyche", "Percy",
	"Calip", "Clio", "Eutre", "Erato", "Mepho",
	"Hymn", "Techo", "Thali", "Urnia", "Dia"
};

