// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 3-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "Dac.h"

const unsigned short note[4] = {2841, 2255, 1896, 1420};  // note frequency values

uint32_t counter;
uint8_t play;
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Stop(void){			//set play to 0 when function is called to resume normal piano function
	play = 0;
}


int main(void){      
  
	TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init();
	DAC_Init();
	Timer0A_Init();
  // other initialization
  EnableInterrupts();
  while(1){ 
		
	if(counter == 200000) {						//flip the LED bit (PF2) every 200000 runs of while loop
		GPIO_PORTF_DATA_R ^= 0x4;
		counter = 0;
	} else {
		counter++;
	}
	uint32_t i;
	uint32_t key = Piano_In();
	if (play == 0) {								// key 1 = first note in array, key 2 = second note in array, key 3 = thrid note in array
				if(key == 0){
					key = note[0];
					Sound_Play(key);
					i = (i+1)%18050;
				}
				else if(key == 1){
					key = note[0];
					Sound_Play(key);
				} 
				else if(key == 2){
					key = note[1];
					Sound_Play(key);
				} 
				else if(key == 4){
					key = note[2];
					Sound_Play(key);
				} 
				else if(key == 8){			// if switch 4 is pressed, play song
					play = 1;
					Song();					
				}
	//			else if(key == 3) {
		//			Song();
		//		}
				else {
					key = 0;
				}
		key = 0;
	} else {
		if(key == 1) {							// if switch 4 is pressed while song is playing, stop song
			Stop();
			key = 0;
			}
		}
	}
}
