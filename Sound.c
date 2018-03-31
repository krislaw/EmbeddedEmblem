#include "Sound.h"

#include <stdint.h>
#include <stdio.h>

#include "..//inc//tm4c123gh6pm.h"
#include "Timer0A.h"
#include "Timer1.h"
#include "Timer2.h"
#include "DAC.h"
#include "Instruments.h"
#include "../inc/ST7735.h"

#define wavesize 64 
#define MAX_NOTES 100
#define MAX_SONGS 5

struct Song{
	uint32_t numNotes;
	uint32_t note[MAX_NOTES];
	uint32_t noteDuration[MAX_NOTES];
}Song;

struct Song songs[MAX_SONGS];

volatile uint16_t songIndex;
volatile uint16_t numOfSongs;
volatile uint16_t noteIndex;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

uint16_t sindex;
uint16_t noteAmp;
uint16_t songindex;

uint16_t* currentInstrument;

uint16_t playing = 0;

void PlaySong(){
	//enable timer to play current song
	Timer2_Enable();
	Timer1_Enable();
	Timer0A_Enable();
}

void StopSong(){
	Timer2_Disable();
	Timer1_Disable();
	Timer0A_Disable();
}

#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))
	

	/* plays the current note: outputs next value of sine wave */
void Timer0Task(void){	
	sindex = (sindex + 1) % 64;
	DAC_Out(currentInstrument[sindex] * noteAmp / 1600 );
}	

	/* controls envelope for each note of the song */
void Timer1Task(void){
	noteAmp = (noteAmp * 95) / 100;
}

	/* switches to a new note */
void Timer2Task(void){
	PF3 ^= 1;
	noteAmp = 400; //default amplitude 3.00 max is 4.00
	noteIndex = (noteIndex + 1) % songs[songindex].numNotes; 
	if(songs[songIndex].note[noteIndex] == 0) { } //Timer0A_Disable(); }
	else{
	/* re-inits timer 0 and 2 for the next note */
	Timer0A_Init((&Timer0Task), songs[1].note[noteIndex] / 2); //divide by 2 becuause those notes were for 32 wave array
	Timer2_Init((&Timer2Task), songs[1].noteDuration[noteIndex]);
	}
}

void SongInit(){
	/* Music for "Tetris Theme" arranged by Frank Yi
	*Each line is 1 measures of the song */
	uint32_t song1notes[36] = {
		0, 0,
		E0, B0, C0, D0, C0, B0,
		A0, A0, C0, E0, D0, C0,
		B0, C0, D0, E0,
		C0, A0, A0,
		0, D0, G0, B, A, G0,
		E0, C0, E0, D0, C0,
		B0, C0, D0, E0
		};
	
	uint32_t song1dur[36] = {
		whole * 2, whole,
		qtr, eit, eit, qtr, eit, eit,
		qtr, eit, eit, qtr, eit, eit,
		qtrdot, eit, qtr, qtr,
		qtr, qtr, hlf,
		eit, qtr, eit, qtr, eit, eit,
		qtrdot, eit, qtr, eit, eit,
		qtrdot, eit, qtr, qtr
	};
	
	//copying because songs won't let me write to the whole array at once ),:
	for(int i = 0; i < 36; i++){
	songs[0].note[i] = song1notes[i];
	songs[0].noteDuration[i] = song1dur[i];
	}
	songs[0].numNotes = 36;
	
	
		/* Music for "Lost Woods" 
	*Each line is 1 measure of the song */
		uint32_t song2notes[89] = {
			0, 0,
			D1, F1, G1, D1, F2, G1,
			D1, F1, G1, C2, B2, G1, A2,
			G1, E1, C1, C1, B1,
			C1, E1, C1,
			
			D1, F1, G1, D1, F1, G1,
			D1, F1, G1, C2, B2, G1, A2,
			B2, G1, E1, E1, G1,
			E1, B1, C1,
			
			B1, C1, D1, E1, F1, G1,	
			A2, G1, C1,
			B1, C1, D1, E1, F1, G1,
			A2, B2, C2,
			
			B1, C1, D1, E1, F1, G1,
			A2, G1, C1,
			B1, A1, D1, C1, E1, D1, F1, E1,
			G1, F1, A2, F1, B2, A2, C1, D1, B1,
			C1
		};
	
	uint32_t song2dur[89] = {
		whole * 2, whole,
		eit, eit, qtr, eit, eit, qtr,
		eit, eit, eit, eit, qtr, eit, eit,
		eit, eit, qtr, qtrdot, eit,
		eit, eit, hlfdot,
		
		eit, eit, qtr, eit, eit, qtr,
		eit, eit, eit, eit, qtr, eit, eit,
		eit, eit, qtr, qtrdot, eit,
		eit, eit, hlfdot,
		
		eit, eit, qtr, eit, eit, qtr,
		qtr, qtr, hlfdot,
		eit, eit, qtr, eit, eit, qtr,
		eit, eit, hlfdot,
		
		eit, eit, qtr, eit, eit, qtr,
		qtr, qtr, hlfdot,
		eit, eit, eit, eit, eit, eit, eit, eit,
		eit, eit, eit, eit, eit, eit, eit/2, eit, eit/2,
		whole
	};
	
	for(int i = 0; i < 89; i++){
	songs[1].note[i] = song2notes[i];
	songs[1].noteDuration[i] = song2dur[i];
	}
	
	songs[1].numNotes = 89;
	numOfSongs = 2;
	
	StopSong();
	}


void Restart(){
	//change time in song to 0
	StopSong();
	songIndex = 0;
	sindex = 0;
	PlaySong();
}

int CycleInstrument(){
if(currentInstrument == Guitar){
	currentInstrument = (uint16_t *) Bassoon;
	return 0;
}
if(currentInstrument == Bassoon){
	currentInstrument = (uint16_t *) Trumpet;
	return 0;
}
if(currentInstrument == Trumpet){
	currentInstrument = (uint16_t *) Oboe;
	return 0;
}
if(currentInstrument == Oboe){
	currentInstrument = (uint16_t *) Guitar;
	return 1;
}
else{
	return 0;
}
}

void SoundInit(void){
	currentInstrument = (uint16_t *) Guitar;
	Timer0A_Init((&Timer0Task), songs[songIndex].note[noteIndex]);
	Timer1_Init((&Timer1Task), songs[songIndex].noteDuration[noteIndex] >> 10);
	Timer2_Init((&Timer2Task), songs[songIndex].noteDuration[noteIndex]);
	SongInit(); //load songs as global vars
	songIndex = 0;
	sindex = 0;
	
}

void SetSong(uint16_t num){
	songIndex = num;
}

uint16_t sefindex;

void SetSoundEffect(uint16_t num){
	sefindex = num;
}

void PlaySoundEffect(void){
	//not sure how to implement this yet
}

