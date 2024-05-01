// Amplifier Control Process
// YB1NWP - 2022
//
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "headers.h"
#include "globals.h"
#include "hwio.h"
#include "lcd.h"
#include "adc.h"
#include "scheduler.h"
#include "keyboard.h"
#include "linearize.h"
#include "ampctrl.h"
#include "ampproc.h"



#define CALIBRATE
#undef	CALIBRATE



/********** AMPproc ***********
;
; Subroutine	: AMPproc
; Author	: N. Putra
; Created	: 08/31/22
;
; Description   	: This routine is the FSM amplifier process
;			: 
;			:
;
*/
void AMPproc() 
{
	// Periodically executed 
	// every 5mS
	//
	if( sema & SMTICK )
	{
		// Keyboard scan function
		KBfsm();

		// ADC
		ADCproc();

		// Clear semaphore
		sema &= ~SMTICK;


		// Skip calculation while in FAULT state
		//
		if( OSstate >= PROT1 )
		{
			return;
		}

		// Audit the state
		//
		if( (OSstate == TX2) && (PIND & 0x04) )
		{
			// TX2 and PTT is inactive (HIGH)
			ALC_ON;
			BIAS_OFF;
			OSstate	 = RX1;
			PORTB &= ~0x01;

			// Wait 25mS
			TMdelay  = 25;
		}



		// Faster and can be calibrated in EEPROM table
		// pFWD and pREF are mapped to 0 - 1000
		//
		mFWD = LNlinearize( &LNfwd, vFWD );
		mREF = LNlinearize( &LNref, vREF );
	
		if( mFWD >= pFWD )
		{
			// New reading is higher than before
			pFWD = mFWD;
		}
		else
		{
			// make power decay slower using median 
			// of previous reading and the new one
			//
			pFWD = (pFWD * 242 / 256) + (mFWD * 16 / 256);
		}
	
		// Do the same here ... 
	
		if( mREF >= pREF )
		{
			// New reading is lower than before
			pREF = mREF;
		}
		else
		{
			// make power INCREASE slower using median 
			// of previous reading and the new one
			//
			pREF = (pREF * 242 / 256) + (mREF * 16 / 256);
		}

	}



	// Called when timer expires
	// which set to be 250mS
	//
	if( sema & SMTIME )
	{
		// Clear semaphore
		//
		sema   &= ~SMTIME;


		// Compute SWR only if pFWD is not 0
		//
		if( pFWD > 0 )
		{
			Rho  = 1.0  * sqrt( 1.0 * pREF / pFWD );
		}
		else
		{
			Rho = 0;
			SWR = 1.00;
		}

	
		// Value must not go or equal 1
		if( Rho < 1.0 )
		{
			// ... need to prevent spikes ...
			// 128 * 5mS = 640mS average
			//
			SWR  = (SWR * (242 / 256)) +  ((1 + Rho) / (1 - Rho) * (14 / 256));
			if( SWR > 9.99 ) SWR = 9.99;
		}
	
	
		// Check for abnormal conditions
		//
		//
		if( Temp > 470 )
		{
			// Overheat at 47C
			DScmd( 0x80 );
			DSstrpr( (PGM_P)ST07 );
			OSstate = PROT1;
		}

	
		if( SWR > 3.0 )
		{
			// High SWR
			DScmd( 0x80 );
			DSstrpr( (PGM_P)ST08 );
	
			DScmd( 0x8C );
			DSprnum( SWR * 100, 2, 0x8B );
	
			// Display FWD & REF
			DSprnum( pFWD, 0, 0xC0 );
			DSdata( 'W' );
			DSprnum( pREF, 0, 0xC5 );
			DSdata( 'W' );
	
			OSstate = PROT1;
		}


		// ADC process
		switch ( ADstate )
		{
			case ADCCPL :	// Idle
					// Compute ADC ref voltage
					ADref = (unsigned int)(1023L * ADbgv * ADSMP / ADsum[6]);

					// Temperature approximation. Temp holds 10x the actual temperature value.
					Temp  = 1230 - ADsum[0]/ADSMP;

					// Compute from measurement of analog inputs
					vFWD  = (unsigned int)(1L * ADref * ADsum[2] / ADSMP / 1023);
					vREF  = (unsigned int)(1L * ADref * ADsum[3] / ADSMP / 1023);
					// 16 = 1024 / 64
					vBDT  = (unsigned int)(1L * ADref * ADsum[4] / ADSMP / 16 / ADbdd);	
					vINP  = (unsigned int)(1L * ADref * ADsum[5] / ADSMP / 1023);


			case ADIDLE :	// Completed
					// Start the conversion
					ADstate = ADSTART;
					break;
		}


		switch( OSstate )
		{
			case STBY :	// Stand by,
					// Power to controller, LPF. But not to Bias
					//
					// Fall through ...
					// .
					// .


			case OPER :	// Operate,
					// PA is powered, and ready to activated
					//
					// Update first line of LCD
					sema |= SM1LCD;

					/*
					// Get the band from GetBandData function
					//
					tmp = GetBandData();

					// If changed, set the LPF
					if( tmp != band )
					{
						band = tmp;
						SetLPF( band );
					}
					*/
					break;


			case TX2 :	// Transmit
			case RX1 :	// Receive
					//

					// Update first line of LCD
					sema |= SM1LCD;
					break;


			case PROT1 :	// Enter protection mode
					// Turn off VDD, TRX Relay and bias
					VDD_OFF;
					TRX_RLY_OFF;
					BIAS_OFF;

					// Turn on PROT LED
					DSled   |= LEDPROT;
					I2CwrData( BDTADDR, ~(DSled) | 0x1F );
					OSstate = PROT2;

			case PROT2 :	// Stay here until being reset
					break;
		}

	}



	// Check for LCD update request
	// First line
	//
	if( sema & SM1LCD )
	{
#ifndef	CALIBRATE
		// Display the bar meter
		//
		// Use mapped value so that full scale of bar graph
		// represent full scale of power
		//
		DSshowbar( mFWD, mREF );

		/*
		if( OSstate == TX2 )
		{
			DSshowbar( mFWD, mREF );
		}
		else if( OSstate < PROT1 )
		{
			// Do not alter first line when FAULT
			DSprnum( Temp, 3, 0x8A );
			DSdata( 'C' );
		}
		*/
#else
		// For calibration of EEPROM
		// Display FWD & REF
		//
		// Do not alter first line when FAULT
		if( OSstate < PROT1 )
		{
			DScmd( 0x80 );
			DSprnum( vFWD, 0, 0x80 );
			DSprnum( vREF, 0, 0x85 );
			DSprnum( Temp, 3, 0x8A );
			DSdata( 'C' );
		}
#endif

		// Clear flag and request for update second line
		sema &= ~SM1LCD;
		sema |=  SM2LCD;

		// Debug status
		// DSled   ^= LEDAUTO;
		// I2CwrData( BDTADDR, ~(DSled) | 0x1F );
	}

	if( sema & SM2LCD )
	{
		// Information on second line 
		// 0123456789012345
		// F: 9999W SWR 1.2
		//

		/*
		if( OSstate == TX2 )
		{
			DScmd( 0xC0 );
			DSstrpr( (PGM_P)ST03 );
			DSprnum( pFWD, 0, 0xC3 );
			DSdata( 'W' );

			DScmd( 0xC8 );
			DSstrpr( (PGM_P)ST04 );	
			DSprnum( SWR * 100, 2, 0xCC );
		}
		else
		{
			if( OSstate < PROT1 )
			{
				// Display FWD & REF
				// 0123456789012345
				// F: 9999W R:9999W 

				DScmd( 0xC0 );
				DSstrpr( (PGM_P)ST04 );
				DSprnum( pFWD, 0, 0xC3 );
				DSdata( 'W' );

				DScmd( 0xC9 );
				DSstrpr( (PGM_P)ST05 );
				DSprnum( pREF, 0, 0xCB );
				DSdata( 'W' );
			}
		}
		*/

			if( OSstate < PROT1 )
			{
				// Display FWD, SWR and TEMP
				// 0123456789012345
				// 9999W 1.20 29.3C
				// 9999W9999W 29.3C

				DSprnum( pFWD, 0, 0xC0 );
				DSdata( 'W' );

				//DSprnum( SWR * 100, 2, 0xC5 );
				DSprnum( pREF, 0, 0xC5 );
				DSdata( 'W' );

				DSprnum( Temp, 3, 0xCA );
				DSdata( 'C' );
			}

		// Clear flag
		sema &= ~SM2LCD;
	}
}




/********** AMPtmrhdlr ***********
;
; Subroutine	: AMPtmrhdlr
; Author	: N. Putra
; Created	: 04/13/24
;
; Description   	: This routine is the FSM amplifier process
;			: when timer expires
;			:
;
*/
void AMPtmrhdlr() 
{
	// Timer expires.
	// Proceed with next state accordingly
	//
	//
	switch( OSstate )
	{
		case TX1 :	// Relay should now has been activated 
				// safe to put RF Power
				//
				BIAS_ON;
				ALC_OFF;
				OSstate = TX2;

				// Setup INT0
				EICRA   = 0x00;					// Reset EICRA
				EICRA   = EICRA | (1<<ISC01) | (1<<ISC00);	// Interrupt on Rising edge of INT0
				break;

		case RX1 :	// Disable Bias
				//
				TRX_RLY_OFF;
				OSstate = OPER;

				// Setup INT0
				EICRA   = 0x00;					// Reset EICRA
				EICRA   = EICRA | (1<<ISC01);			// Interrupt on Falling edge of INT0
				break;
	}
}





// ISR for PTT
//
//
ISR( INT0_vect )
{
	// Disable external interrupt INT0
	cli();
	EIFR	= EIFR  | (1<<INTF0);
	EIMSK	= EIMSK & ~(1<<INT0);



	// PTT activated

	switch( OSstate )
	{
		case OPER :	// PTT status is changed to active
				// Transmit,
				// Start transmit sequence
				// Relay ON, ALC ON

				TRX_RLY_ON;
				ALC_ON;
				OSstate	 = TX1;
				PORTB |= 0x01;

				// Wait 25mS as best practice before powering up the amplifier
				TMdelay  = 25;

				break;




		case TX2 : 	// PTT Released
				//
				// Turn off Bias before switching TRX relay
				// So no high power during switching
				//
				ALC_ON;
				BIAS_OFF;
				OSstate	 = RX1;
				PORTB &= ~0x01;

				// Wait 25mS
				TMdelay  = 25;

				break;
	}


	// Enable INT0 again
	EIMSK   = EIMSK | (1<<INT0);
	EIFR    = EIFR  | (1<<INTF0);                   // Clear INT0 flag
	sei();
}

