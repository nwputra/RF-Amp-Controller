#include "keyboard.h"
#include "keymake.h"
#include "headers.h"
#include "globals.h"
#include "twi.h"
#include "hwio.h"
#include "linearize.h"




void F1_M( void )
{
	switch( OSstate )
	{
		case STBY : 	// Switch to MENU
				break;

		case OPER : 	// Switch to STAND BY
				break;
	}
	return;
}



void F2_M( void )
{
	switch( OSstate )
	{
		case STBY : 	// Change band to auto
		case OPER :	//
				//
				/*
				 * NOT IMPLEMENTED YET !!!
				 *
				if( AMPstate.b.autoband )
				{
					DSled &= ~LEDAUTO;
					AMPstate.b.autoband = 0;
				}
				else
				{
					DSled |= LEDAUTO;
					AMPstate.b.autoband = 1;
				}
				I2CwrData( BDTADDR, ~(DSled) | 0x1F );

				// Set the band based on input
				//
				band = GetBandData();
				SetLPF( band );
				LNslctband( band );
				*
				*
				*/
				break;
	}
	return;
}



void F3_M( void )
{
}



void execMake()
{

	if( KBpres & (1 << F1) )
	{
		F1_M();				// F1 pressed
	}

	if( KBpres & (1 << F2) )
	{
		F2_M();				// F2 pressed
	}

	if( KBpres & (1 << F3) )
	{
		F3_M();				// F3 pressed
	}
}



