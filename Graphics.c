#include "Graphics.h"
#include "inc/ST7735.h"

#include "HardCoded.h"

const uint16_t * currentMap;
uint16_t ** charSprites;
uint8_t animate;
uint16_t* charSpritesA[8];
uint16_t* charSpritesB[8];
uint16_t* charPortraits[8];

void GraphicsInit(){ Output_Init(); }

//input: character id, 2 25x15 sprites and 1 32x32 portrait
void SetCharacterGraphics(uint8_t id, uint16_t* sprite1, uint16_t* sprite2, uint16_t* portrait){
	charSpritesA[id] = sprite1;
	charSpritesB[id] = sprite2;
	//charSprites[id] = charSpritesA; //switch between A,B for idle animation
	animate = 0;
	charPortraits[id] = portrait;
}

void PrintCursor(uint8_t x, uint8_t y){
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


void PrintMoveTile(uint8_t x, uint8_t y){
	if(x > 7 || y > 7 || y < 1) { return; }
	
	x = x * 16 + 1;
	y = (y * 16) + 33;
	
	for(int i = 1; i < 14; i+= 2){
		ST7735_DrawPixel(x + i, y, ST7735_BLUE); //top of square
		ST7735_DrawPixel(x + (13 - i), y + 13, ST7735_BLUE); //bottom of square
		ST7735_DrawPixel(x + 13, y + i, ST7735_BLUE); // right side of square
		ST7735_DrawPixel(x, y + (13 - i), ST7735_BLUE); //left side of square
	}
}

void PrintTile(uint8_t x, uint8_t y){ //print a map tile
	//top left is 0,0 and bottom right is 7, 7
	x = x * 16;
	y = (y * 16);
	for(int i = 0; i < 16; i++){
			uint16_t mapIndex = (MapHeight * y) + x;
			ST7735_DrawBitmap(x, y + infoOffset, &(currentMap[mapIndex]), 16, 1);
			y = y + 1;
	}
}



#define SpriteYOffset 22 //32 + 16 - 25
#define SpriteHeight 25
#define SpriteWidth 15
#define BgColor 0x4D84

void PrintSprite(uint8_t id, uint8_t x, uint8_t y){ //which sprite to print and where 
	uint16_t xs = (x * 16);
	uint16_t ys = (y * 16) + SpriteYOffset;
	
	int k = 0;
	for(int i = 0; i < SpriteHeight; i++){
		for(int j = 0; j < SpriteWidth; j++){
			if(charSpritesA[id][k] != BgColor){
				ST7735_DrawPixel(xs + j, ys + (SpriteHeight - i), charSpritesA[id][k]);
			}
			k++;
		}
	}
}

void AnimateSprite(){ //alternate the graphics so next print sprite looks animated
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
	ST7735_SetCursor(6,5);
	ST7735_OutString("VICTORY!");
	for(int i = 0; i < 3; i++){
		PrintSprite(i, i + 3, 5);
	}
}

void ShowLoseScreen(){
	Output_Clear();
	ST7735_SetCursor(6,5);
	ST7735_OutString("...DEFEATED");
	for(int i = 3; i < 6; i++){
		PrintSprite(i, i, 5);
	}
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
	if(stat > 9) { 	ST7735_OutChar(stat/10 + '0'); 	}
	else { ST7735_OutChar(' '); }
	ST7735_OutChar(stat % 10 + '0');
}

void ShowNonCombat(){
	ST7735_FillRect(0, 0, 128, 32, ST7735_BLACK);
	ST7735_SetCursor(1, 0);
	ST7735_OutString("A: Confirm Move\n B: Move elsewhere\n ^|v Attack Enemy");	
}

const char* serverMessages[] = {
	"Waiting for Enemy..", "Enemy Turn Complete", "Press A to Continue"
};
void ShowWaitForServer(uint8_t i){
	ST7735_FillRect(0, 0, 128, 32, ST7735_BLACK);
	ST7735_SetCursor(1, 1);
	ST7735_OutString((char*) serverMessages[i]);
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
	
	//ST7735_SetTextColor(ST7735_WHITE);
}
	
char * weaponString[] = {	"Sword", "Lance", "Axe", "Dragon", "Tome", "Staff", "Armor"};

void ShowPreview(char* name, uint8_t weapon, uint16_t hpMax,
uint16_t atk, uint16_t def, uint16_t res, uint16_t spd){
	Output_Clear();
	ST7735_DrawBitmap(48, 44, charPortraits[3], 32, 32);
	ST7735_SetCursor(7, 5); //middleish of screen
	ST7735_OutString(name);
	ST7735_SetCursor(7, 6);
	ST7735_OutString((char*) weaponString[weapon]);
	ST7735_SetCursor(3, 7);
	ST7735_OutString("Hit Points: ");
	StatOut(hpMax);
	ST7735_SetCursor(5, 9);
	ST7735_OutString("ATK:");
	StatOut(atk);
	ST7735_OutString(" DEF:");
	StatOut(def);
	ST7735_SetCursor(5, 10);
	ST7735_OutString("RES:");
	StatOut(res);
	ST7735_OutString(" SPD:");
	StatOut(spd);
		
}
	
void ShowCombatPreview(char* defendName, uint16_t defendHP, uint16_t defendMHP,
	char* attackName, uint16_t attackHP, uint16_t attackMHP, uint8_t attackId){
	
	ST7735_FillRect(0, 0, 128, 32, ST7735_BLACK);
		
	ST7735_DrawBitmap(0, 31, charPortraits[attackId], 32, 32);
		
	ST7735_SetCursor(6, 0);
	ST7735_OutString(attackName);
	ST7735_OutString(" vs ");
	ST7735_SetCursor(14, 0);
	ST7735_OutString(defendName);
	ST7735_SetCursor(6, 1);
	ST7735_OutUDec(attackHP);
	ST7735_OutChar('/');
	ST7735_OutUDec(attackMHP);
	ST7735_SetCursor(14, 1);
	ST7735_OutUDec(defendHP);
	ST7735_OutChar('/');
	ST7735_OutUDec(defendMHP);
	if(attackHP == 0 && attackId < 3){
		ST7735_SetCursor(6, 2);
		ST7735_OutString("WARN: Fatality");
	}	
}


const char* story[] = {
	" Chapter 1: Desert\n\n Enemy forces gather\n     in the desert\n     to the west.\n\n      Your team is\n      deployed\n     to stop them.\n\n   Route the Enemy.", 
	" Chapter 2: Valley\n\n   With the enemy\n    defeated, you\nthink yourself safe.\n\nAlong you path home,\n an ambus strikes!\n\n  Defend yourself!",
	" Chapter 3: Temple\n\n You learn that a\n    greater force\n is on it's way to\n the to the castle.\n\n  Heroes, go forth\n  and stop them!\n\n  Route the Enemy.",
	" Chapter 4: Ruin\n\n The ground shakes,\n  the sky cracks,\n   and a chasm opens\n    in the earth.\n\nOne final battle...\n\n  Defeat the Evil.",
	"   Epilogue: \n At long last, the\nkingdom is safe from\n the forces of evil\n\n     But...\n  Another day will\n come where heroes\n   are needed."
};

void ShowStory(uint8_t chapter){
	if(chapter > 4) { return; }
	Output_Clear();
	ST7735_SetCursor(0, 1);
	ST7735_OutString((char*) story[chapter]);
}

 
//cursors for team building states
const uint16_t selectX[8] = { 2, 2, 2, 2, 18, 18, 18, 18 };
const uint16_t selectY[8] = { 3, 6, 10, 14, 3, 6, 10, 14 };

//height 128
//width 64 with all

void HideTeamSelectCursor(uint8_t Index){
	if(Index > 7) { return; }
	ST7735_SetCursor(selectX[Index], selectY[Index]);
	ST7735_OutChar(' ');
}

void ShowTeamSelectCursor(uint8_t Index){
	if(Index > 7) { return; }
	ST7735_SetCursor(selectX[Index], selectY[Index]);
	ST7735_OutChar('^');
}


#define minTBX 20
#define maxTBX 65
#define deltaTBX 44
#define minTBY 44
#define maxTBY 153
#define deltaTBY 36

void PrintOnTeamBuild(const uint16_t * portraits[8]){
	Output_Clear();
	ST7735_SetCursor(2,0);
	ST7735_OutString("Choose Your Team:");
	uint8_t x = 0;
	for(uint16_t i = minTBX; i < maxTBX; i+= deltaTBX){
		for(uint16_t j = minTBY; j < maxTBY; j+= deltaTBY) {
			ST7735_DrawBitmap(i, j, portraits[x], 32, 32);
			x++;
		}
	}
}
