/*
Input.c
Controls all buttons and Joystick Logic
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "Input.h"
#include "Timer3A.h"

//Control Pad on PE 1, 2, 3, 5
#define upB 0x02
#define downB 0x04
#define leftB 0x08
#define rightB 0x20

//AB on PF 3,4
#define Abut 0x08
#define Bbut 0x10
uint8_t AB;

//Joystick on PE 1, 2
#define js 0x06
#define jsPeriod 100000

void SysTickInit(){
	SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
  SysTick_Init();             // initialize SysTick timer
  GPIO_PORTF_DIR_R |= 0x04;   // make PF2 out (built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
}

void ButtonInit(){ //AB: PF3, PF4
		/* Initialize button PF 3 and 4 */	
  SYSCTL_RCGCGPIO_R |= 0x00000020;     // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
  GPIO_PORTF_AMSEL_R &= ~(Abut | Bbut);      // 3) disable analog
  GPIO_PORTF_PCTL_R &= ~0x000FF000; 				// 4) PCTL GPIO 
  GPIO_PORTF_DIR_R &= ~(Abut | Bbut);        // 5) direction input
  GPIO_PORTF_AFSEL_R &= ~(Abut | Bbut);      // 6) regular port function
  GPIO_PORTF_DEN_R |= (Abut | Bbut);         // 7) enable digital port
	GPIO_PORTF_IS_R &= ~(Abut | Bbut);     // (d)   is edge-sensitive
  GPIO_PORTF_IBE_R &= ~(Abut | Bbut);    //     _ is not both edges
  GPIO_PORTF_IEV_R &= ~(Abut | Bbut);    //     _ falling edge event
  GPIO_PORTF_ICR_R = (Abut | Bbut);      // (e) clear flag5 & 6
  GPIO_PORTF_IM_R |= (Abut | Bbut);      // (f) arm interrupt on Port F

	NVIC_PRI7_R= (NVIC_PRI7_R& ~0xE0000)|0x00600000; // Port F, bits 23-21 current at priority 3
  NVIC_EN0_R = 0x40000000; // enable interrupt 4 in NVIC
		
	AB = 0;
}

void GPIOPortF_Handler(void){
  if(GPIO_PORTF_RIS_R&Abut){  // poll PE4
    GPIO_PORTE_ICR_R = 0x10;  // acknowledge flag4
    AB |= 1;                  // signal SW1 occurred
  }
  if(GPIO_PORTE_RIS_R&0x20){  // poll PE5
    GPIO_PORTE_ICR_R = 0x20;  // acknowledge flag5
    AB |= 2;                  // signal SW2 occurred
  }
}

uint8_t GetButtonPush(void){
	uint8_t ret = AB;
	AB = 0;
	return ret;
}

//------------ADC_In23------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 12-bit result of ADC conversions
// Samples ADC8 and ADC9 
// 125k max sampling
// software trigger, busy-wait sampling
// data returned by reference
// data[0] is ADC2 (PE1) 0 to 4095
// data[1] is ADC3 (PE2) 0 to 4095

uint32_t XrawOld;
uint32_t YrawOld;
uint32_t Xraw;
uint32_t Yraw;
	
void ADC_In23(){
	XrawOld = Xraw;
	YrawOld = Yraw;
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  Yraw = ADC0_SSFIFO2_R&0xFFF;  // 3A) read first result
  Xraw = ADC0_SSFIFO2_R&0xFFF;  // 3B) read second result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}


void JSInit(){ //ADC
	Timer3A_Init(&ADC_In23, jsPeriod); // how often the adc gets read i guess
	
	 volatile unsigned long delay;
	SYSCTL_RCGCADC_R |= 0x00000001; // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
	
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	while((SYSCTL_RCGCADC_R & 0x1) == 0){}
		
  GPIO_PORTE_DIR_R &= ~js;      // 2) make PE4 input
  GPIO_PORTE_AFSEL_R |= js;     // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~js;      // 4) disable digital I/O on PE2
	
	GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFFFFF00F;
  GPIO_PORTE_AMSEL_R |= js;     // 5) enable analog function on PE2
  ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field //HELP: crashing on line
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0023;         // 12) set channels for SS2 // 2 and 3 is differential pair 1
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
	ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2 !!! TODO: probably using sequencer 1 or 0
	
	
	/* //not working ):
  volatile unsigned long delay;
	
  SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	
	SYSCTL_RCGC0_R |= 0x00000001;   // 6) activate ADC0 ???
  delay = SYSCTL_RCGC2_R; 
	
  GPIO_PORTE_DIR_R &= ~js;      // 2) make PE4 input
  GPIO_PORTE_AFSEL_R |= js;     // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~js;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= js;     // 5) enable analog function on PE2
	
	//SYSCTL_RCGCADC_R |= 0x00000001; // 6) activate ADC0 ???
 
	GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFFFFF00F;
	
  SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K
	

	*/
}


/* ====== INPUT INIT =======
* Initialize buttons in stuff
* 
*/
void InputInit(){
	SysTickInit(); //use for debouncing?
	ButtonInit();
	//ControlPadInit();
	JSInit(); // 4/2/2018: Hardfaults everywhere, pls fix
}



uint32_t JSgetX(void){
	ADC_In23();
	return Xraw;
}

uint32_t JSgetY(void){
	ADC_In23();
	return Yraw;
}

 #define highThreshold 3000
 #define lowThreshold 1000

int8_t GetX(void){ //1 or -1
	if(Xraw > highThreshold && XrawOld > highThreshold){
		return 1;
	}
	if(Xraw < lowThreshold && XrawOld < lowThreshold){
		return -1;
	}
	return 0;
}

int8_t GetY(void){ //1 or -1
	if(Yraw > highThreshold && YrawOld > highThreshold){
		return 1;
	}
	if(Yraw < lowThreshold && YrawOld < lowThreshold){
		return -1;
	}
	return 0;
}

