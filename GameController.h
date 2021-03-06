#ifndef __gamecontroller_h
#define __gamecontroller_h

//used for CharActions
extern int16_t unitsOnMap[8][8];
extern const uint8_t *tilesOnMap;
	
void GameInit(void);

void GeneratePlayerTeam(void);

void GenerateEnemyTeam(void);

void RunGame(void);
	

#endif /* __stdint_h */
