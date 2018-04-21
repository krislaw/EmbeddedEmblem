#include "DAC.h"
#include "inc/tm4c123gh6pm.h"
/*SSI3 B4, 5, 6, 7*/
#include <stdint.h>


//********DAC_Init*****************
// Initialize MAX5353 12-bit DAC using port B
// inputs:  initial voltage output (0 to 4095)
// outputs: none
// assumes: system clock rate of 80 MHz
void DAC_Init(uint16_t data){
  volatile uint32_t delay;
  SYSCTL_RCGCSSI_R |= 0x08;  // activate SSI3
  SYSCTL_RCGCGPIO_R |= 0x08; // activate port D
  delay = SYSCTL_RCGC2_R;               // allow time to finish activating
  GPIO_PORTD_AFSEL_R |= 0x0F;           // enable alt funct on PD 0 1 2 3
  GPIO_PORTD_DEN_R |= 0x0F;             // enable digital I/O on PD 0 1 2 3
                                        // configure Pb 7 6 5 4
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x0000FFFF)+0x000011111; //use ssi3
  GPIO_PORTD_AMSEL_R = 0;               // disable analog functionality on PA
	//GPIO_PORTD_DIR_R |= 0x0F;							// port D 0 1 2 3 is output
	
  SSI3_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
  SSI3_CR1_R &= ~SSI_CR1_MS;            // master mode
                                        // clock divider for 8 MHz SSIClk (assumes 80 MHz PLL)
  //SSI3_CPSR_R = (SSI3_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10;
	SSI3_CPSR_R = 0x02; //8MHz SSI CLK
	
  SSI3_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
                  SSI_CR0_SPH |         // SPH = 0
                 SSI_CR0_SPO);         // SPO = 0
	
                                        // FRF = Freescale format
  SSI3_CR0_R = (SSI3_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 16-bit data
  SSI3_CR0_R = (SSI3_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_16;
	SSI3_CR0_R |= 0x0F;
	
  SSI3_DR_R = data;                     // load 'data' into transmit FIFO
  SSI3_CR1_R |= SSI_CR1_SSE;            // enable SSI
}


// ********DAC_Out*****************
// Send data to MAX5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
void DAC_Out(uint16_t code){
  while((SSI3_SR_R&SSI_SR_TNF)==0){};// wait until room in FIFO
  SSI3_DR_R = code;                  // data out
//  while((SSI0_SR_R&SSI_SR_RNE)==0){};// wait until response
//  return SSI0_DR_R;                  // acknowledge response
}
