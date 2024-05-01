#include <avr/pgmspace.h>
#include "twi.h"
#include "headers.h"
#include "globals.h"
#include "keyboard.h"
#include "keybreak.h"
#include "hwio.h"
#include "lcd.h"
#include "linearize.h"
#include "ampctrl.h"



void F1_B( void )
{
	switch( OSstate )
	{
		case STBY : 	// Switch to OPERATE
				// STANDBY LED = bit 5, make if ON
				VDD_ON;
				DSled   &= ~LEDSTBY;
				OSstate  = OPER;
				I2CwrData( BDTADDR, ~(DSled) | 0x1F );
				break;

		case OPER : 	// Switch to STAND BY
				// STANDBY LED, make if OFF
				// VDD_ON will turn on OPERATE LED
				//
				VDD_OFF;
				DSled   |= LEDSTBY;
				OSstate  = STBY;
				I2CwrData( BDTADDR, ~(DSled) | 0x1F );
				break;
	}
	return;
}



void F2_B( void )
{
	switch( OSstate )
	{
		case STBY : 	
		case OPER : 	// Change band only when not in AUTO Mode
				// Band number 0 up to 5
				//
				if( AMPstate.b.autoband == 0 )
				{
					if( ++band > MAXBAND ) band = 0;
					SetLPF( band );
					LNslctband( band );
				}
				else
				{
					// Toggle the Band Data selection
					//
					//
					if( AMPstate.b.bdsel )
					{
						// Yaesu Band Data
						AMPstate.b.bdsel = 0;
					}
					else
					{
						// Icom Band Data
						AMPstate.b.bdsel = 1;
					}
				}
				break;
	}
	return;
}



void F3_B( void )
{
	switch( OSstate )
	{
		case STBY : 	
		case OPER : 	// Change band only when not in AUTO Mode
				// Band number 0 up to 5
				//
				if( AMPstate.b.autoband == 0 )
				{
					if( band == 0 )
					{
						band = MAXBAND;
					}
					else
					{
						band--;
					}
					SetLPF( band );
					LNslctband( band );
				}
				break;


		case PROT2 :	// Reset the amplifier state
				DSled   &= ~LEDPROT;
				DSled   |= LEDSTBY;
				OSstate  = STBY;
				I2CwrData( BDTADDR, ~(DSled) | 0x1F );

				// Clear protection reason
				DScmd( 0x80 );
				DSstrpr( (PGM_P)ST09 );
				SWR = 1.00;
				pFWD = 0;
				pREF = 0;
				break;
	}
	return;
}



void execBreak()
{
	/* key has been released */

	if( KBrels & (1 << F1) )
	{
		F1_B();				// F1 released
	}

	if( KBrels & (1 << F2) )
	{
		F2_B();				// F2 released
	}

	if( KBrels & (1 << F3) )
	{
		F3_B();				// F3 released
	}
}



