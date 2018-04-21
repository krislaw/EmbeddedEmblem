// SysTickTestMain.c
// Runs on LM4F120/TM4C123
// Test the SysTick functions by activating the PLL, initializing the
// SysTick timer, and flashing an LED at a constant rate.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 4.7

   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 2.11, Section 2.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/ST7735.h"
#include "PLL.h"

#include "GameController.h"
#include "Graphics.h"
#include "Sound.h"
#include "Input.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

/* ==== HARDWARE TESTER FUNCTIONS
* Tests for Music, Graphics, and Analog Inputs
*
*
*/

void SoundTest(){
	PlaySong();
//	SysTick_Wait10ms(200000);
	StopSong();
}

void GraphicsTest(){
	/* Print the Map as 8 x 8 of Tiles */
	//SetMap((const uint16_t *) &EasterMap);
	/*
	for(int y = 0; y < 8; y ++){
		for(int x = 0; x < 8; x++){
			PrintTile(x, y);
		}
	}
	
	PrintSprite(0, 0, 0);
	PrintSprite(0, 7, 7);
	*/
}


void ButtonTest(){
	uint8_t vector = GetButtonPush();
	if((vector & 0x1) > 0){
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x02;
	}
	
	if((vector & 0x2) > 0){
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x01;
	}
}

void JSTest(){
		uint32_t x = JSgetX();
		uint32_t y = JSgetY();
		
		ST7735_SetCursor(0, 0);
		for(int i = 10000; i > 0; i = i / 10){
			ST7735_OutChar((x/i) + '0');
			x = x % i;
		}
		ST7735_SetCursor(0, 1);
		for(int i = 10000; i > 0; i = i / 10){
			ST7735_OutChar((y/i) + '0');
			y = y % i;
		}
}

void RunTests(){
	  while(1){
    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2		
		SoundTest();
//		SysTick_Wait10ms(100);
		GraphicsTest();
//		SysTick_Wait10ms(100);
		JSTest();
//		SysTick_Wait10ms(100);
//		SoundTest();
		//return 0;
		/*
		volatile int x;
		volatile int y;
		for(int i = 0; i < 100; i++){
			if(JSgetX() > 3000) { x = 1; }
			if(JSgetX() < 3000) { x = -1; }
		CursorTest(x, 0);
		}
		*/
  }
}
	/* === MAIN INITIALIZATIONS ====
	*
	*
	*/
	
int main(void){
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	DisableInterrupts();
	GraphicsInit(); //ST7735 and resources, will use Timer 4
	ShowStartupScreen();
	InputInit();	
	SoundInit();	//SSI DAC, uses Timer0A, Timer1, Timer2
	GameInit();
	
	//LED SETUP
	SYSCTL_RCGCGPIO_R |= 0x00000020; // activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
  GPIO_PORTF_DIR_R |= 0x0E;     // PF3,PF2,PF1 outputs
  GPIO_PORTF_DEN_R |= 0x0E;     // enable digital I/O on PF3,PF2,PF1  

	EnableInterrupts();
	//DisableInterrupts(); //we out here testin'
		
	ClearButtonPush(); //ignore presses during setup
	while(GetButtonPush == 0){ }
	
		/* === MAIN CODE FOR TESTING OR RUNNING ====
	* Run Tests will run the above tests on the hardware
	* Run Game will start the actual project
	*/
	
	//RunTests(); //check hardware functionality.
	RunGame();
}
