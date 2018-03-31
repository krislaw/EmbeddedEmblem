#include "Graphics.h"
#include "../inc/ST7735.h"

#include "HardCoded.h"

uint16_t * CurrentMap;

void PrintCursor(int x, int y){
	if(x > 7 || y > 6 || x < 0 || y < 0) { return; }
	
	x = x * 16;
	y = (y * 16);
	
	ST7735_DrawFastHLine(x, y, 3, ST7735_RED);
	ST7735_DrawFastVLine(x, y, 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x + (15-2), y, 3, ST7735_RED);
	ST7735_DrawFastVLine(x + 15, y, 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x, y + 15, 3, ST7735_RED);
	ST7735_DrawFastVLine(x, y + (15-2), 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x + (15-2), y + 15, 3, ST7735_RED);
	ST7735_DrawFastVLine(x + 15, y + (15-2), 3, ST7735_RED);
	
}

void PrintTile(int x, int y){
	//top left is 0,0 and bottom right is 7, 7
	x = x * 16;
	y = (y * 16);
	//ST7735_SetCursor(0, 0);
	//ST7735_OutString("Map Test");	
	for(int i = 0; i < 16; i++){
			uint16_t mapIndex = (MapHeight * y) + x;
			ST7735_DrawBitmap(x, y + infoOffset, &(CurrentMap[mapIndex]), 16, 1);
			y = y + 1;
	}
	
	PrintCursor(3,5);
}

void GraphicsInit(){
	Output_Init();
	
	
}

void ShowStartupScreen(void);

void ShowMissionScreen(void);

void PrintMissionCursor(void);

void SetMap(char mapId){ // what image to load, 128 x 128 pixels
	CurrentMap = (uint16_t *) &EasterMap; 
}

void ShowMap(void);

void ShowInfo(char* name, char id, uint16_t lvl,
	uint16_t hp, uint16_t hpMax, uint16_t atk, uint16_t def);

void PrintMapChars(void);
