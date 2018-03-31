#ifndef __graphics_h
#define __graphics_h

#include <stdint.h>

#define MapWidth 128
#define MapHeight 128
#define infoOffset 32
#define MapArraySize 16384 //128 x 128 pixels
#define TileArraySize 48 //16 * 16 tiles

void PrintTile(int tilex, int tiley);
	
void GraphicsInit(void);

void ShowStartupScreen(void);

void ShowMissionScreen(void);

void PrintMissionCursor(void);

void SetMap(char mapId); // what image to load, character sprites, map size if we implement scrolling, ect.

void ShowMap(void);

void ShowInfo(char* name, char id, uint16_t lvl,
	uint16_t hp, uint16_t hpMax, uint16_t atk, uint16_t def);

void PrintMapChars(void);

#endif /* __graphics_h */
