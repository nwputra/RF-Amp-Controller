#ifndef	_LINEARIZE_H
#define	_LINEARIZE_H

#include <avr/eeprom.h>

typedef struct
{
	uint16_t	adc;
	uint16_t	val;
} LNpoint_t;


typedef struct
{
	uint8_t size;
	LNpoint_t pt[15];
} LNtable_t;


// Table for Directional Coupler
// Max 7 entries
//
typedef struct
{
	uint8_t 	size;
	uint16_t	pt[7];
} LNxtable_t;


// Table for DAC (11 entries)
//
typedef struct
{
	uint8_t 	size;
	uint16_t	pt[11];
} LNdtable_t;


extern LNtable_t	LNfwd;
extern LNtable_t	LNref;
extern LNtable_t	LNdacf;
extern LNtable_t	LNdacr;

extern const LNtable_t  PMTfwd80;
extern const LNtable_t  PMTfwd40;
extern const LNtable_t  PMTfwd20;


uint16_t LNlinearize( LNtable_t *lst, uint16_t adc );
void LNinit( LNtable_t *dst, LNxtable_t *src );
void LNdacinit( LNtable_t *dst, LNdtable_t *src );
void LNslctband( uint8_t band );

#endif
