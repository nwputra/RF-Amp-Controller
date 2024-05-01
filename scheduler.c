#include <avr/io.h>
#include "headers.h"
#include "globals.h"
#include "timer.h"
#include "scheduler.h"
#include "twi.h"



/********* OSschinit ***********
;
; Subroutine    : OSschinit
; Author        : N. Putra
; Created       : May 21-2010
; Last Update   : May 21-2010 - Initial design
;               :
;               :
;
; Description           : Initialize OS Scheduler
;                       :
; Return value          : Nothing
;                       :
*/
void OSschinit()
{
	OSstate	= 0;
	OSALDIS	= 0;
	OSALCTR	= 250;
}



/********* OSheartbeat ***********
;
; Subroutine	: OSheartbeat
; Author	: N. Putra
; Created	: no record
; Last Update   : Oct 23-2009 - Ported to AVR
;		: 
;		:
;
; Description   	: Display CPU heartbeat using LED
;			: Where the blink count indicates OSstate
;			:
; Return value		: Nothing
; Register Affected     : Flags set accordingly
;			:
;			:
*/
void OSheartbeat( void )
{
	switch( OSALDIS )
	{
		case OSGETALM :
			OSALNUM = OSstate + 1;			// set alarm number
			OSALDIS = OSONDSP;			// set state
			PORTB |= 0x01;				// turn on display
			break;
	
		case OSONDSP:
			OSALDIS = OSOFFDSP;			// set state
			PORTB &= ~0x01;				// turn off display
			break;
	
		case OSOFFDSP:
			if( (-- OSALNUM) == 0 )			// if alarm count finished
			{
				OSALNUM = 4;			// 4 * 250 mS
				OSALDIS = OSINTDSP;		// set state
			}
			else
			{
				OSALDIS = OSONDSP;		// set state to on display
				PORTB |= 0x01;			// turn on display
			}
			break;

		case OSINTDSP:
			if( (-- OSALNUM) == 0 )
			{
				OSALDIS = OSGETALM;		// back to get alarm value
			}
			break;	
	}
}



