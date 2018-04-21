
#include <stdint.h>

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
	char name[6];
	uint8_t weapon;
	uint16_t MHP; //max hp
	uint16_t HP;
	uint16_t ATK;
	uint16_t DEF;
	uint16_t RES;
	uint16_t SPD;
	uint16_t MOV; //TODO: horses move 3, but cant go through trees
}Unit;

//TODO: balance stats bc how do???

const struct Unit protagonists[6] = { //TODO: add tome, armor options
	{0, 1, "Kris", lance, 20, 20, 18, 8, 8, 6, 2}, 			//use slance1
	{0, 1, "Yuriy", lance, 18, 18, 18, 12, 2, 10, 2}, 	//use slance3
	{0, 1, "Hanah", staff, 14, 14, 14, 10, 12, 10, 2}, 	//use scleric1
	{0, 1, "Max", axe, 17, 17, 21, 8, 2, 12, 2}, 				//use saxe1
	{0, 1, "Steph", sword, 15, 15, 18, 6, 6, 6, 2},			//use ssword1
	{0, 1, "Brand", sword, 15, 15, 18, 6, 6, 6, 2}			//use ssword3
};

const struct Unit villains[6] = { //TODO: add lance options
	{3, 1, "", armor, 25, 25, 14, 16, 4, 6, 1},			//marmor1
	{3, 1, "", axe, 13, 13, 21, 10, 10, 10, 2},			//maxe3
	{3, 1, "", staff, 16, 16, 12, 6, 12, 8, 2},			//mcleric1
	{3, 1, "", tome, 13, 13, 16, 10, 11, 10, 2},			//mmage1
	{3, 1, "", sword, 17, 17, 18, 6, 6, 15, 2},			//mthief1
	{3, 1, "", sword, 15, 15, 18, 6, 6, 6, 2}				//mASSasin1 i forgot an s okay sue me
};

const char villainNames[30][6] = { //randomly generate villain names, shoutout to SEC Eweek 2018 for theme
	"Zeus", "Hera", "Poise", "Demtr", "Ares",
	"Athen", "Armis", "Heph", "Dite", "Ermes",
	"Dions", "Hades", "Hypno", "Nike", "Janus",
	"Nemis", "Iris", "Hecat", "Tyche", "Percy",
	"Calip", "Clio", "Eutre", "Erato", "Mepho",
	"Hymn", "Techo", "Thali", "Urnia", "Dia"
};

