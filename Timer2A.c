// TIMER2A.c
// Runs on LM4F120/TM4C123
// Use TIMER2A in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// May 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

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

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask2)(void);   // user function

// ***************** Timer2A_Init ****************
// Activate TIMER2A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer2A_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2A
  PeriodicTask2 = task;          // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER2AA during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER2AA timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  //TIMER2A_CTL_R = 0x00000001;    // 10) enable TIMER2A
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;	// acknowledge TIMER2A timeout
	(*PeriodicTask2)();                 // execute user task
}

void Timer2A_Disable(){
  TIMER2_CTL_R = 0x00000000;    // disable TIMER2A
}

void Timer2A_Enable(){
	TIMER2_CTL_R = 0x00000001;     // enable timer
}
