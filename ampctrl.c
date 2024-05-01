// Amplifier Controller
// YB1NWP (c) 2022
// version 1.0
//
//

#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <util/delay.h>

#include "headers.h"
#include "globals.h"
#include "twi.h"
#include "lcd.h"
#include "adc.h"
#include "dac.h"
#include "hwio.h"
#include "timer.h"
#include "scheduler.h"
#include "keyboard.h"
#include "linearize.h"
#include "ampproc.h"
#include "ampctrl.h"
#include "eeprom-data.h"


/* Labels/Text arrays */
const char      ST01[]  PROGMEM = "  HF AMPLIFIER  ";
const char      ST02[]  PROGMEM = "  -* GEBLEX *-  ";
const char      ST03[]  PROGMEM = "F: ";
const char      ST04[]  PROGMEM = " SWR";
const char      ST05[]  PROGMEM = "R:";
const char      ST06[]  PROGMEM = "TEMP";
const char	ST07[]  PROGMEM = "OVERHEAT ";
const char	ST08[]  PROGMEM = "HI SWR   ";
const char	ST09[]  PROGMEM = "         ";





/*
0	0
1	85
2	115
3	140
4	160
5	183
6	203
7	220
8	230
9	240
10	255
*/


int main( )
{
	// Port directions
	DDRB	= 0x2F;				// PORT B input for MISO only
	PORTB	= 0x10;				// Initial data, all LOW
	DDRC	= 0x3C;				// PORT C set SDA, SCL as outputs
	PORTC	= 0x30;
	DDRD	= 0x0B;				// PORT D4, D0..1 as outputs
	PORTD	= 0xFC;				// Initial state is HIGH, except PD1..0 for ULN2003


	// Variables
	//
	//
	AMPctrl		= 0;
	AMPctchg	= 0;
	AMPstate.c	= 0;
	band		= 2;
	sema		= SMTIME;


	// Initialize Hardware systems
	// 
	TWIInit();
	TMtmr2init();
	OSschinit();
	DSinit();
	KBinit();
	ADCinit();
	//
	// DACinit();
	// LNdacinit( &LNdacr, &EEdacr );
	// LNdacinit( &LNdacf, &EEdacf );
	// 

	// Initialize linearization table
	//
	SetLPF( band );
	LNslctband( band );
	//
	DSled		= LEDPWR;
	I2CwrData( BDTADDR, ~(DSled) | 0x1F );


	
	DScmd( 0x28 );				// 4 bit interface, 2 Lines, 5x8 Dot LCD
	DScmd( 0x0C );				// Display ON, Cursor ON, Blink (DCB)
	DScmd( 0x06 );				// No Shift & Direction Up
	DScmd( 0x02 );				// Cursor Home
	DScmd( 0x01 );				// Clear Display
	delayMS( 10 );				// Some delay needed ...
	DScginit();				// Special character initialization

	vFWD 		= 0;
	vREF 		= 0;
	vINP 		= 0;
	pFWD 		= 0;
	pREF 		= 0;
	pINP 		= 0;
	Rho		= 0.0;
	SWR		= 1.0;
	Temp 		= 33.1;



	// Setup hardware state
	VDD_OFF;
	TRX_RLY_OFF;
	BIAS_OFF;
	ALC_OFF;

	// Disable interrupts
	cli();

	// INT0 setup
	EICRA   = EICRA | (1<<ISC01);			// Interrupt on Falling edge of INT0
	EIFR	= EIFR  | (1<<INTF0);			// Clear INT0 flag
	EIMSK	= EIMSK | (1<<INT0);			// Enable external interrupt INT0

	// Enable interrupts
	sei();




	/* Display POST message */

	// Print initial message
	DScmd( 0x01 );					// Clear Display
	DScmd( 0x80 );					// Set DDRAM at first line
	DSstrpr( (PGM_P)ST01 );
	DScmd( 0x80 | 0x40 );				// Set DDRAM at second line
	DSstrpr( (PGM_P)ST02 );

	TMtimer[0] = 3;
	while( TMtimer[0] > 0 )
	{
		// Nothing
	}

	//  Clear Display
	DScmd( 0x02 );
	DScmd( 0x01 );

	// Turn on STBY LED
	DSled		|= LEDSTBY;
	I2CwrData( BDTADDR, ~(DSled) | 0x1F );

	while( 1 )
	{
		AMPproc();
	}
}



