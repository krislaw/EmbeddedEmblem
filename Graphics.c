#include "Graphics.h"
#include "inc/ST7735.h"

#include "HardCoded.h"
//#include "Sprites.h"
//#include "Portraits.h"

const uint16_t * currentMap;
uint16_t ** charSprites;
uint8_t animate;
uint16_t * charSpritesA[8];
uint16_t * charSpritesB[8];
uint16_t * charPortraits[8];

void GraphicsInit(){ Output_Init(); }

//input: character id, 2 25x15 sprites and 1 32x32 portrait
void setCharacterGraphics(uint8_t id, uint16_t* sprite1, uint16_t* sprite2, uint16_t* portrait){
	charSprites[id] = sprite1;
	charSpritesA[id] = sprite1;
	charSpritesB[id] = sprite2;
	animate = 0;
	charPortraits[id] = portrait;
}

void PrintCursor(uint16_t x, uint16_t y){
	if(x > 7 || y > 7 || y < 1) { return; }
	
	x = x * 16;
	y = (y * 16) + 32;
	
	ST7735_DrawFastHLine(x, y, 3, ST7735_RED);
	ST7735_DrawFastVLine(x, y, 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x + (15-2), y, 3, ST7735_RED);
	ST7735_DrawFastVLine(x + 15, y, 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x, y + 15, 3, ST7735_RED);
	ST7735_DrawFastVLine(x, y + (15-2), 3, ST7735_RED);
	
	ST7735_DrawFastHLine(x + (15-2), y + 15, 3, ST7735_RED);
	ST7735_DrawFastVLine(x + 15, y + (15-2), 3, ST7735_RED);
}


void PrintTile(int x, int y){ //print a map tile
	//top left is 0,0 and bottom right is 7, 7
	x = x * 16;
	y = (y * 16);
	for(int i = 0; i < 16; i++){
			uint16_t mapIndex = (MapHeight * y) + x;
			ST7735_DrawBitmap(x, y + infoOffset, &(currentMap[mapIndex]), 16, 1);
			y = y + 1;
	}
}



#define SpriteYOffset 47
#define transparency 0x4D84

void PrintSprite(uint8_t id, uint16_t x, uint16_t y){ //which sprite to print and where 
//	uint16_t sprite[SpriteArraySize];
	uint16_t xs = (x * 16);
	uint16_t ys = (y * 16) + SpriteYOffset;
	
	for(uint16_t i = 0; i < SpriteHeight; i++){
		for(int j = 0; j < SpriteWidth; j++){
			ST7735_DrawPixel(xs + i, ys + j, charSprites[id][j*i + j]);
		}
	}
}

void AnimeSprite(){ //alternate the graphics so next print sprite looks animated
	animate ^= 0x01;
	switch(animate){
		case 0: charSprites = charSpritesA;
		default: charSprites = charSpritesB;
	}
}

void ShowStartupScreen(){
ST7735_DrawBitmap(0, 159, (const uint16_t *) LoadScreen, 128, 160);
}

void ShowWinScreen(){
	Output_Clear();
	ST7735_SetCursor(3,5);
	ST7735_OutString("YOU WIN!");
}

void ShowLoseScreen(){
	Output_Clear();
	ST7735_SetCursor(3,5);
	ST7735_OutString("YOU LOSE!");
}

void ShowMissionScreen(char** Missions, uint8_t numMissions){
	Output_Clear();
	ST7735_SetCursor(2,2);
	for(int i = 0; i < numMissions; i++){
	ST7735_OutString(Missions[i]);
	ST7735_OutChar('\n');
	}
}

void ClearMissionCursor(uint8_t ypos){
	if(ypos > 14) { return; }
	ST7735_SetCursor(0, ypos + 2);
	ST7735_OutChar(' ');
}

void PrintMissionCursor(uint8_t ypos){
	if(ypos > 14) { return; } 
	ST7735_SetCursor(0, ypos + 2);
	ST7735_OutChar('>');
}

void SetMap(const uint16_t * map){ // what image to load, 128 x 128 pixels
	currentMap = map;
}

void ShowMap(void){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			PrintTile(i, j);
		}
	}
}

void StatOut(uint16_t stat){
	if(stat > 10) { 	ST7735_OutChar(stat/10 + '0'); 	}
	else { ST7735_OutChar(' '); }
	ST7735_OutChar(stat % 10 + '0');
}

void ShowInfo(char* name, char id, uint16_t lvl,
	uint16_t hp, uint16_t hpMax, uint16_t atk, uint16_t def,
	uint16_t res, uint16_t spd){
	uint16_t teamColor;
	if(id > 2){ 		//villain
		teamColor = ST7735_RED;
	}
	else{ 					//hero
		teamColor = ST7735_BLUE;
	}
		
	ST7735_FillRect(32, 0, 96, 31, ST7735_BLACK);
	ST7735_FillRect(32, 31, 96, 1, teamColor);
		
	ST7735_DrawBitmap(0, 31, charPortraits[id], 32, 32);

	ST7735_SetCursor(6, 0);
	ST7735_OutString(name);
	ST7735_SetCursor(13, 0);
	ST7735_OutString("H:");
	ST7735_OutUDec(hp);
	ST7735_OutChar('/');
	ST7735_OutUDec(hpMax);
	ST7735_SetCursor(6, 1);
	ST7735_OutString("ATK:");
	StatOut(atk);
	ST7735_OutString(" DEF:");
	StatOut(def);
	
	ST7735_SetCursor(6, 2);
	ST7735_OutString("RES:");
	StatOut(res);
	ST7735_OutString(" SPD:");
	StatOut(spd);
	
	ST7735_SetTextColor(ST7735_WHITE);
}

