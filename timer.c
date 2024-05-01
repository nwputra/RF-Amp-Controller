#include <avr/io.h>
#include <avr/interrupt.h>
#include "headers.h"
#include "globals.h"
#include "timer.h"
#include "ampproc.h"

#include <util/delay.h>





/* ********** ATMEGA8 Timer 2 Compare Match Initialization **********
;
; Subroutine	: CTC2init
; Author	: N. Putra
; Created	: no record
; Last Update   : Nov 8-2010  - Initial work on CTC2
;		: Sept 4-2022 - Adapt for ATMEGA 168
*/
void TMtmr2init()
{
   	TCCR2A 	= (1 << COM2A0) | (1 << WGM21);			// Configure timer 2 as CTC
								// With toggle OC2A on compare match
								// For charge pump generator, generating 500Hz signal

	TCCR2B	= (1 << CS22) | (1 << CS21);			// Timer at CLKio/256 (CS22:CS21:CS20 = 110)
								// WGM21:WGM20 = 10 -> CTC Mode

   	TIMSK2 	|= (1 << OCIE2A); 				// Enable CTC Timer 2A interrupt

   	OCR2A	= 78;	 					// Set CTC compare value 
								// 20MHz / 256 = 78125 Hz = 12.8 uS
								// 1mS = 1000uS = 78.125 * 12.8uS
								// Use 78 * 12.8uS =  998.4uS period x 7
								// And 79 * 12.8uS = 1011.2uS period x 1
								// Result in exact 8mS
   	sei(); 							// Enable global interrupts
	TMcnt	= 0;

	OSALCTR = 250;						// 250mS Alarm interval
	TMdelay	= 0;						// ...
	TMtick	= 5;						// ...
	TMsec	= 4;						// 1 S interval timer
	TMtimer[0] = 0;						//
}






/* ********** Timer 2 Compare Match ISR **********
;
; Subroutine	: TIMER2_COMP_vect
; Author	: N. Putra
; Created	: no record
; Last Update   : July 4-2004 - Feature for Keyer
;		: Oct 23-2009 - Port to AVR
;		: May 18-2010 - Use CTC instead OVF
;		: Nov  8-2010 - Use CTC 2 for ATMEGA8
;
; Description   	: Schedule tasks every 1mS
;			: No longer using ISR( TIMER2_OVF_vect )
;
*/
ISR( TIMER2_COMPA_vect )
{
	// When TMcnt has reach 8, adjust the reload value
	// and restore back otherwise
	//
	if( ++TMcnt > 7 )
	{
		TMcnt = 0;
		OCR2A = 79;
	}
	else
	{
		OCR2A = 78;
	}

	/* Refresh speed every 250mS */
	if( (-- OSALCTR) == 0 )
	{
		// Timer expires, set flag
		OSALCTR = 250;
		sema |= SMTIME;

		if( --TMsec == 0 )
		{
			TMsec = 4;				// Prescale to 1 Second

			if( TMtimer[0] ) TMtimer[0]--;		// Timer 0
			if( TMtimer[1] ) TMtimer[1]--;		// Timer 1
			if( TMtimer[2] ) TMtimer[2]--;		// Timer 2
		}
	}

	// Periodic timer
	if( TMtick > 0 )
	{
		if( --TMtick == 0 )
		{
			// Timer expires, set flag
			sema |= SMTICK;
			TMtick = 5;
		}
	}

	// User delay timer
	if( TMdelay )
	{
		if( --TMdelay == 0 )
		{
			// Timer expires
			// Call user function
			AMPtmrhdlr();
		}
	}

	TIFR2 = (1 << OCF2A);				// Timer2 INT Flag Reg: Clear Timer Compare Flag
}




void delayMS( unsigned char ms )
{
	unsigned char volatile i;

	for( i=0; i<ms; i++ )
	{
		_delay_ms( 1 );
	}
}



void delay10US( unsigned char tus )
{
	unsigned char volatile i;

	for( i=0; i<tus; i++ )
	{
		_delay_us( 10 );
	}
}


