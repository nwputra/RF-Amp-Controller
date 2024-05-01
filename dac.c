// DAC 
// YB1NWP (c) 2022
// version 1.0
//
//

#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "dac.h"
#include "globals.h"
#include "timer.h"



/*		Fwd	Ref
0	0	0	0
1	85	1580	1660
2	115	2230	2270
3	140	2750	2750
4	160	3190	3170
5	183	3590	3560
6	203	3930	3850
7	220	4210	4170
8	230	4480	4430
9	240	4720	4710
10	255	4980	4980
*/


void DACinit( void )
{
	// Disable interrupts
	cli();

	// Timer 1 setup
	//
	TCCR1B  = 0x00;					// Disable Timer2 while we set it up
	TCNT2   = (256 - 157);				// Generate interrupt @ F_CPU / 128 / 157 or about every 1mS
	TIFR1   = 0x27;					// Timer1 INT Flag Reg: Clear all flags
	TIMSK1  = 0x00;					// Disable all Timer 1 interrupt


	TCCR1A  = (1 << COM1A1) | (1 << COM1B1);	// Toggle OC1A and OC1B on match
	TCCR1A |= (1 << WGM10);				// Timer2 Control Reg : Fast PWM 8-bit
	TCCR1B  = (1 << WGM12);				// WGM12:10 = 101
	TCCR1B |= (1 << CS11) | (1 << CS10);		// CS12:10  = 011 - Prescaler 64 

	// Enable interrupts
	sei();
}


void DACsetA( unsigned int val )
{
	OCR1AH = val >> 8; 
	OCR1AL = val & 0xFF;
}



void DACsetB( unsigned int val )
{
	OCR1BH = val >> 8; 
	OCR1BL = val & 0xFF;
}



