// Amplifier Controller
// YB1NWP (c) 2022
// version 1.0
//
//

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"
#include "timer.h"
#include "headers.h"
#include "lcd.h"


unsigned char		lcdt;



/********* DSinit ***********
;
; Subroutine	: DSinit
; Author	: N. Putra
; Created	: 04/29/03
;
; Description   	: This routine will initialize LCD with
;			: 4 bit interface
;			: 
; Register Affected     : Flags set accordingly
; Returns 		: Non zero in case of error
;
*/
int DSinit( void )
{
	int rc = 0;

	/* Delay > 15mS, may be power up condition
	 * Specification mentions of sending Init Command 3 times
	 * With delay >15mS, >4mS, >100uS
	 *
	 */
	delayMS( 20 );

	/* Software Reset command (0x30) */
	rc += I2CwrData( LCDADDR, 0x30 );
	rc += I2CwrData( LCDADDR, 0x30 | DSEN );
	rc += I2CwrData( LCDADDR, 0x30 );

	/* Delay >4 mS */
	delayMS( 5 );

	/* Software Reset command (0x30) */
	rc += I2CwrData( LCDADDR, 0x30 );
	rc += I2CwrData( LCDADDR, 0x30 | DSEN );
	rc += I2CwrData( LCDADDR, 0x30 );

	/* Delay >100 uS */
	delay10US( 25 );

	/* Software Reset command (0x30) */
	rc += I2CwrData( LCDADDR, 0x30 );
	rc += I2CwrData( LCDADDR, 0x30 | DSEN );
	rc += I2CwrData( LCDADDR, 0x30 );

	/* Delay >100 uS */
	delay10US( 25 );

	/* Software Initialization command (0x20) = 4databit */
	rc += I2CwrData( LCDADDR, 0x20 );
	rc += I2CwrData( LCDADDR, 0x20 | DSEN );
	rc += I2CwrData( LCDADDR, 0x20 );

	/* Delay 5 mS */
	delayMS( 5 );

	/* Clear LCD control */
	lcdt = 0;

	/* Return non zero on errors */
	return rc;
}



/********** DScmd ***********
;
; Subroutine	: DScmd
; Author	: N. Putra
; Created	: 04/28/03
;
; Description   	: This routine will send command to LCD
;			: via 4 bit interface
;			: B7-4 = Data
;			: B3   = Backlit
;			: B2   = EN
;			: B1   = RW LOW (write)
;			: B0   = RS LOW
;			:
;
; Register Affected     : Flags set accordingly
; Cycle Time		: 23
*/
void DScmd( unsigned char cmd  )
{
	unsigned char tmp;

	tmp = cmd << 4;

	cli();
	lcdt = (lcdt & 0x0F) | (cmd & 0xF0);
	I2CwrData( LCDADDR, lcdt );				// Send high nibble first
	I2CwrData( LCDADDR, lcdt | DSEN );			// Strobe it
	I2CwrData( LCDADDR, lcdt );				//

	lcdt = (lcdt & 0x0F) | (tmp & 0xF0);
	I2CwrData( LCDADDR, lcdt );				// Now send the lower nibble 
	I2CwrData( LCDADDR, lcdt | DSEN );			// Strobe it
	I2CwrData( LCDADDR, lcdt );				//
	sei();

	/* Delay 5mS, we do not read BUSY flag here */
	delayMS( 5 );
}




/********** DSdata ***********
;
; Subroutine	: LCDdata
; Author	: N. Putra
; Created	: 04/28/03
;
; Description   	: This routine will send data to LCD
;			: via 4 bit interface
;			: B7-4 = Data
;			: B3   = Backlit
;			: B2   = EN
;			: B1   = RW LOW (write)
;			: B0   = RS HIGH
;			:
;
; Register Affected     : Flags set accordingly
; Cycle Time		: 23
;
*/
void DSdata( unsigned char dta )
{
	unsigned char tmp;

	tmp = dta << 4;

	cli();
	lcdt = (lcdt & 0x0F) | (dta & 0xF0);
	I2CwrData( LCDADDR, lcdt | DSRS );			// Send high nibble first, RS=1
	I2CwrData( LCDADDR, lcdt | DSRS | DSEN );		// Strobe it
	I2CwrData( LCDADDR, lcdt | DSRS );			//

	lcdt = (lcdt & 0x0F) | (tmp & 0xF0);
	I2CwrData( LCDADDR, lcdt | DSRS );			// Now send the lower nibble 
	I2CwrData( LCDADDR, lcdt | DSRS | DSEN );		// Strobe it
	I2CwrData( LCDADDR, lcdt | DSRS );			//
	sei();

	/* Delay 5mS, we do not read BUSY flag here */
	delayMS( 5 );
}



/********** DSlit ***********
;
; Subroutine	: DSlit
; Author	: N. Putra
; Created	: 08/30/22
;
; Description   	: This routine will set backlit of LCD
;			: 0 = turn off backlit
;			: non zero = turn on backlit
;			:
;
; Register Affected     : Flags set accordingly
;
*/
void DSlit( unsigned char val )
{
	if( val )
	{
		// Turn on backlit
		lcdt |= DSBL;
	}
	else
	{
		// Backlit off
		lcdt &= ~DSBL;
	}

	// Send to LCD
	I2CwrData( LCDADDR, lcdt );
}



/********** DSsetCursor ***********
;
; Subroutine	: DSsetCursor
; Author	: N. Putra
; Created	: 08/31/22
;
; Description   	: This routine will set cursor position 
;			: 0 = turn off backlit
;			: non zero = turn on backlit
;			:
;
; Register Affected     : Flags set accordingly
;
*/
void DSsetCursor( unsigned char row, unsigned char col )
{
	col = col * 0x40;

	DScmd( 0x80 | row | col );
}




/********** DSprint ***********
;
; Subroutine	: DSprint
; Author	: N. Putra
; Created	: 08/31/22
;
; Description   	: This routine will print null terminated string
;			: at current cursor position
;			: 
;			: 
;			:
;
; Register Affected     : Flags set accordingly
;
*/
void DSprint( char *str )
{
	// While there is something to print
	while( *str )
	{
		// print and increment pointer
		DSdata( *str++ );
	}
}


void DSstrpr( const char *ptr )
{
	unsigned char i;

	strcpy_P( (char *)buff, ptr );
	for( i=0; buff[i] != 0x00; i++ )
	{
		DSdata( buff[i] );
	}
}




/********** DSbar ***********
;
; Subroutine	: DSbar
; Created	: 
;
; Description   : Downloads character generator RAM (used for bar indicator)
;		: address should be set before - lcdSetAddr
;		: 
*/
void DSbar(uint8_t upper, uint8_t lower)
{
	// 8 line of 5 bit dots
	DSdata(upper);
	DSdata(upper);
	DSdata(upper);
	DSdata(0);					// separator
	DSdata(lower);
	DSdata(lower);
	DSdata(lower);
	DSdata(0);					// separator
}



/********** DScginit ***********
;
; Subroutine	: DScginit
; Created	: 
; Description   : Create bar graph in CGRAM
;		: Consists of 8 special character that represents 2 bar display
;		:
*/
void DScginit()
{
	// init LCD Graphic Character Generator
	DScmd(0x40);					// set initial address GCRAM address
	DSbar(0x18, 0);					// code 0 = fwd half, rev nil
	DSbar(0, 0x18);					// code 1 = fwd nil,  rev half
	DSbar(0x18, 0x18);				// code 2 = fwd half, rev half
	DSbar(0x1F, 0x1F);				// code 3 = fwd full, rev full
	DSbar(0x1F, 0);					// code 4 = fwd full, rev nil
	DSbar(0x18, 0x1F);				// code 5 = fwd half, rev full
	DSbar(0x1F, 0x18);				// code 6 = fwd full, rev half
	DSbar(0, 0x1F);					// code 7 = fwd nil,  rev full
}



// Show level bars for forward/reverse signal on 1st display line
// Input value is from 0 to 1024
//
void DSshowbar(uint16_t Fwd, uint16_t Rev)
{
	uint8_t  idx;
	uint8_t  c = 0;

	/*
	// Initial range from 0 to 1024 Watts
	// One LCD character represent 1024 / LCDLINELENGTH
	// and each half is 1024 / LCDLINELENGTH / 2
	//
	//
	uint8_t	 step = 1024 / LCDLINELENGTH / 2;	// how much value is per character 1024/16 = 64
							// one character has two level = 32
	*/

	uint8_t	 step = 512 / LCDLINELENGTH / 2;	// make it 512 according to safe power limit
	uint16_t level = step;				// initial level value

	DScmd( 0x80 );					// Set DDRAM at first line

	for( idx = 0; idx < LCDLINELENGTH; idx++ )
	{
		c = 0;

		if( Fwd > level ) c =1; 		// set the bar if value is more than level step 
		if( Rev > level ) c|=2;			//
		level += step;				// next level step

		if( Fwd > level ) c|=4;			// same as above
		if( Rev > level ) c|=8;			//
		level += step;				//


		/* Possible value	MoveUpper2lower to save space
			0 - 0/0		
			1 - H/0		H/0
			2 - 0/H		0/H
			3 - H/H		H/H
			4 *****		F/F	(F)
			5 - F/0		F/0
			6 *****		H/F	(B)
			7 - F/H		F/H
			8 *****		0/F	(A)
			9 *****
			A - 0/F
			B - H/F
			C *****
			D *****
			E *****
			F - F/F
		*/


		// Move value from A,B and F to lower placement
		//
		if( c == 15 )      c = 3;		// F/F
		else if( c == 11 ) c = 5;		// H/F
		else if( c == 10 ) c = 7;		// 0/F
		else if( c == 0 )  c = 32;		// space
		else
			c--;				// CGRAM array index is 0, so offset this by -1

		DSdata( c );				// Print the character
	}
}



// conversion of unsigned int (value 0 to 9999) into ASCII string, including decimal point
// decimal = location of '.' symbol. If =0, not used.
// fills the Display buffer (5 characters) at position pointed by idx.
//
void DSprnum( uint16_t x, uint8_t decimal, uint8_t idx )
{
	uint16_t base = 1000;
	uint16_t a, i;

	for( i=0; i<=4; )
	{
		a     = x / base;
		x     = x % base;
		base /= 10;

		if( a > 9 ) a = 9;
		buff[i] = (uint8_t)a + '0';
		i++;

		if( i == decimal )
		{
			buff[i] = '.';
			i++;
		}
	}

	if( decimal == 0 )
	{
		// decimal point not used, append space instead
		buff[i] = ' ';
	}

	i = 0;
	while( i < 3 )
	{
		// if( (BFbuff[i] != '0') || (BFbuff[i] == '.') ) break;
		if( buff[i] != '0' ) break;

		// replace leading zeroes by space
		buff[i] = ' ';
		i++;
	}

	// Print the value at second line of LCD
	// Changed : idx should contains index PLUS 0x80 = first line, or 0xC0 = second line
	//
	DScmd( idx );

	// Print either 4 or 5 character depends on with decimal or not
	a = 4;
	if( decimal ) a++;

	for( i=0; i<a; i++ )
	{
		DSdata( buff[i] );
	}
}

