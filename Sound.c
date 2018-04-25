
#include <stdint.h>
#include <stdio.h>

#include "Sound.h"
#include "Instruments.h"


#include "inc/tm4c123gh6pm.h"
#include "Timer0A.h"
#include "Timer1A.h"
#include "Timer2A.h"
#include "DAC.h"

#include "inc/ST7735.h"

#define wavesize 64 
uint16_t waveIndex;

struct Song* currentSong; //treble
uint16_t* currentInstrument;
uint16_t noteIndex;
uint16_t noteAmp;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


/* MUSIC
*
* Play one note at a time, pushed to an SSI DAC
*/

void PlaySong(){
	Timer2A_Enable();
	Timer1A_Enable();
	Timer0A_Enable();
}

void StopSong(){
	Timer2A_Disable();
	Timer1A_Disable();
	Timer0A_Disable();
}

#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))
	

/* plays the current note: outputs next value of sine wave */
void Timer0Task(void){	
	waveIndex++;
	if(waveIndex >= wavesize) { waveIndex = 0; }
	DAC_Out(currentInstrument[waveIndex] * noteAmp / 1800 );
}	

	/* controls envelope for each note of the song */
void Timer1Task(void){
	noteAmp = (noteAmp * 95) / 100;
}

	/* switches to a new note */
void Timer2Task(void){
	static uint8_t rest;
	noteAmp = 400;
	noteIndex++;
	if(noteIndex >= currentSong->numNotes) { noteIndex = 0; }
	if(currentSong->note[noteIndex] == 0) { Timer0A_Disable(); Timer1A_Disable(); rest = 1;}
	else{
		Timer0A_Init((&Timer0Task), currentSong->note[noteIndex] / 2); //divide by 2 becuause those notes were for 32 wave array
		Timer1A_Init((&Timer1Task), currentSong->noteDuration[noteIndex] >> 8);
		Timer2A_Init((&Timer2Task), currentSong->noteDuration[noteIndex]);
		if(rest == 1){
		Timer0A_Enable();
		Timer1A_Enable();
			rest = 0;
		}
	Timer2A_Enable();
	}
}

/*
* Main is 0
* Maps are desert 1, valley 2, temple 3, ruin 4
* Lose is 5, Win is 6
*/
void SetSong(uint16_t num){
	switch(num){
		case 0: currentSong = (struct Song *) &Main_Treble;
		case 1: currentSong = (struct Song *) &Main_Bass;
		case 2: currentSong = (struct Song *) &Main_Bass;
		case 3: currentSong = (struct Song *) &Main_Bass;
		case 4: currentSong = (struct Song *) &Main_Bass;
		case 5: currentSong = (struct Song *) &Main_Bass;
		case 6: currentSong = (struct Song *) &Main_Bass;
		case 15: currentSong = (struct Song *) &Scales;
		default: currentSong = (struct Song *) &Scales;
	}
}

void SongInit(){
	//setup timers with the current song
	SetSong(15);
	Timer0A_Init((&Timer0Task), currentSong->note[noteIndex]);
	Timer1A_Init((&Timer1Task), currentSong->noteDuration[noteIndex] >> 10);
	Timer2A_Init((&Timer2Task), currentSong->noteDuration[noteIndex]);
}


void Restart(){
	StopSong();
	waveIndex = 0;
	noteIndex = 0;
	PlaySong();
}

void SoundInit(void){
	currentInstrument = (uint16_t *) Trumpet;
	SongInit();
	waveIndex = 0;
	noteIndex = 0;
	DAC_Init(0);
}

/* SOUND EFFECTS
*
* These are not currently implemented
*/

uint16_t sefindex;

void SetSoundEffect(uint16_t num){
	sefindex = num;
}

void PlaySoundEffect(void){
	//not sure how to implement this yet
}

