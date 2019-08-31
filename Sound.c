// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"
#define eighth 8000000
#define dotted 12000000
#define quarter 16000000

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

//sin wave
const unsigned short wave[64] = {				
  32,35,38,41,44,47,49,52,54,56,58,				
  59,61,62,62,63,63,63,62,62,61,59,				
  58,56,54,52,49,47,44,41,38,35,				
  32,29,26,23,20,17,15,12,10,8,				
  6,5,3,2,2,1,1,1,2,2,3,				
  5,6,8,10,12,15,17,20,23,26,29};		

	

//song notes
const unsigned short hp[30] = {
	2531, 1896, 1594, 1689, 1896, 1265, 
	1420, 1689, 1896, 1594, 1689, 2009, 
	1789, 2531, 2531, 1896, 1594, 1689,
	1896, 1265, 1064, 1127, 1194, 1505, 
	1194, 1265, 1340, 2681, 1594, 1896}; 

//song note durations
const unsigned long hpTempo[30] = {
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
	0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF};

//envelope
const unsigned short envelope[10] = {
	1,2,3,4,5,6,7,8,9,10};

//array indices
uint8_t idx;
uint8_t id;
uint8_t i;
uint8_t div = 1;
uint32_t count = 0;

// ********LED_Init***************
void LED_Init(void){
	volatile int delay;
	SYSCTL_RCGCGPIO_R |= 0x20;	//activate LED on PF2
	delay = 42;
	GPIO_PORTF_DIR_R |= 0x4;
	GPIO_PORTF_DEN_R |= 0x4;
}

// ************Timer0A_Init**************
void Timer0A_Init(){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 0x0;    			// 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  EndCritical(sr);
}
	
// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();
	LED_Init();
	NVIC_ST_CTRL_R = 0;              			// disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x0;  				 			// maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x400000000;		//set priority to 3
  NVIC_ST_CTRL_R = 0x7;                  // enable SysTick with core clock	
}


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){		
	NVIC_ST_RELOAD_R = 0;
  NVIC_ST_RELOAD_R = period;			// set reload value to note
}

void Song() {
	TIMER0_TAILR_R = hpTempo[i];			//set Timer reload to note duration
	if(TIMER0_CTL_R != 0x00000001) {	//check if timer is enabled
		TIMER0_CTL_R = 0x00000001;    	// 10) enable TIMER0A
		}

	Sound_Play(hp[id]);					

}

void SysTick_Handler() {
	DAC_Out(wave[idx]);		// output sin wave value
	idx = (idx + 1) % 64;	// increment index
/*	if(div < 10 && div > 1) {
			if(count == 1000) {
				div = (div + 1) % 10;
				div = envelope[div];				
				count = 0;
			} else {
				count++;				
		}
	}
*/
}

void Timer0A_Handler(){
  TIMER0_ICR_R = 0x1;// acknowledge timer0A timeout
//	div = 2;
	if(id < 30) {					// increment array indices for song
		id++;
		i++;
	} else {
		id = 0;							// if song finished, loop back
		i = 0;
	}
	Sound_Play(hp[id]);	
}

