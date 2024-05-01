#ifndef	_LCD_H
#define	_LCD_H

// Amplifier Controller
// YB1NWP (c) 2022
// version 1.0
//
//

#include "twi.h"
#include "timer.h"
#include "headers.h"

// LCD interface
//
//
#define		DSRS		0x01			// Display RS bit
#define		DSEN		0x04			// Display EN bit
#define		DSBL		0x08			// Display backlit


unsigned char		lcdt;

int  DSinit( void );
void DScmd( unsigned char cmd  );
void DSdata( unsigned char dta );
void DSlit( unsigned char val );
void DSsetCursor( unsigned char row, unsigned char col );
void DSprint( char *str );
void DSprnum(uint16_t x, uint8_t decimal, uint8_t idx );
void DSbar(uint8_t upper, uint8_t lower);
void DScginit();
void DSshowbar(uint16_t Fwd, uint16_t Rev);
void DSstrpr( const char *ptr );
#endif
