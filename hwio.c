#include <avr/io.h>

#include "twi.h"
#include "adc.h"
#include "headers.h"


void SetLPF( uint8_t band )
{
	// Input parameter is 0 to 5 for band 160m to 10m
	// 160m will turn on relay at P0,
	// 80m  will turn on relay at P1 and so on ...
	//

	LPFctrl.b.relays = (1 << band);

	I2CwrData( LPFADDR,  	(LPFctrl.b.bias << 6) | 
				(LPFctrl.b.oper << 4) | 
				(LPFctrl.b.trx  << 3) | 
				(LPFctrl.b.relays & 0x07) );
}



void SetOPER( uint8_t voper )
{
	// Input parameter is 0 for standby
	// or 1 for operate
	//
	//
	LPFctrl.b.oper = (voper > 0) ? 1 : 0;

	I2CwrData( LPFADDR,  	(LPFctrl.b.bias << 6) | 
				(LPFctrl.b.oper << 4) | 
				(LPFctrl.b.trx  << 3) | 
				(LPFctrl.b.relays & 0x07) );
}



void SetALC( uint8_t valc )
{
	// Input parameter is 0 for receive/standby
	// or 1 for transmit
	//
	//
	AMPstate.b.alc = (valc > 0);
	if( AMPstate.b.alc != 0 )
	{
		PORTD &= ~( 1 << ALCEN);
	}
	else
	{
		PORTD |= ( 1 << ALCEN);
	}

	// Tongseng there is delicious
	//
}



unsigned char GetBandData( void )
{
	// Stripped down version without automatic band selector
	//

	return band;
}


