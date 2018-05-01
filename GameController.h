#ifndef __gamecontroller_h
#define __gamecontroller_h

#include "GameFSM.h"

//used for CharActions
extern int16_t unitsOnMap[8][8];
extern uint8_t unitXLocations[8]; 	//index is character id number
extern uint8_t unitYLocations[8];
extern struct Unit units[8];

extern const uint8_t *tilesOnMap;

void GameInit(void);

void GeneratePlayerTeam(void);

void GenerateEnemyTeam(void);

void RunGame(void);
	

#endif 
