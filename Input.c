/*
Input.c
Controls all buttons and Joystick Logic
*/

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
//#include "SysTick.h"
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
#define jsPeriod 7000000

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
  GPIO_PORTF_IEV_R |= (Abut | Bbut);    //     _ rising edge event
  GPIO_PORTF_ICR_R = (Abut | Bbut);      // (e) clear flag5 & 6
  GPIO_PORTF_IM_R |= (Abut | Bbut);      // (f) arm interrupt on Port F

	NVIC_PRI7_R= (NVIC_PRI7_R& ~0xE0000)|0x00600000; // Port F, bits 23-21 current at priority 3
  NVIC_EN0_R = 0x40000000; // enable interrupt 4 in NVIC
	AB = 0;
}

void GPIOPortF_Handler(void){
	GPIO_PORTF_ICR_R = (Abut | Bbut);  	// acknowledge flag5
  if((GPIO_PORTF_DATA_R&Abut) > 0){  // poll PE4
    AB |= 1;                  // signal SW1 occurred
  }
  if((GPIO_PORTE_DATA_R&Bbut) > 0){  // poll PE5 
    AB |= 2;                  // signal SW2 occurred
  }
}

uint8_t GetButtonPush(void){
	uint8_t ret = AB;
	AB = 0; //clear vector when button is read
	return ret;
}

void ClearButtonPush(void){
	AB = 0; //clear vector when button is read
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


void JSInit(){ //ADC for the Joystick
	Timer3A_Init(&ADC_In23, jsPeriod); // how often the adc gets read i guess
	
	volatile unsigned long delay;
	SYSCTL_RCGCADC_R |= 0x00000003; // 1) activate ADC0 and ADC1
  SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
	
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	while((SYSCTL_RCGCADC_R & 0x3) == 0){}
		
  GPIO_PORTE_DIR_R &= ~js;      // 2) make PE1, PE2 input
  GPIO_PORTE_AFSEL_R |= js;     // 3) enable alternate function on PE1, PE2
  GPIO_PORTE_DEN_R &= ~js;      // 4) disable digital I/O on PE1, PE2
	
	GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFFFFF00F;
  GPIO_PORTE_AMSEL_R |= js;       // 5) enable analog function on PE2
  ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0012;         // 12) set channels for SS2 // 2 and 3 is differential pair 1
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
	ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2 !!! TODO: probably using sequencer 1 or 0
}


/* ====== INPUT INIT =======
* Initialize buttons and joystick
* Buttons use 
*/
void InputInit(){
	ButtonInit();
	//ControlPadInit();
	JSInit(); //Uses Timer3, PE1, PE2
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

int8_t GetX(void){
	//Tested Values: Right is 0, Left is 4095
	if(Xraw > highThreshold && XrawOld > highThreshold){
		Xraw = 2000;
		return -1; //left
	}
	if(Xraw < lowThreshold && XrawOld < lowThreshold){
		Xraw = 2000;
		return 1; //right
	}
	return 0;
}

int8_t GetY(void){ //1 or -1
	//Tested Values: Down is 0, Up is 4095
	if(Yraw > highThreshold && YrawOld > highThreshold){
		Yraw = 2000;
		return -1; //up
	}
	if(Yraw < lowThreshold && YrawOld < lowThreshold){
		Yraw = 2000;
		return 1; //down
	}
	return 0;
}

