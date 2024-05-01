#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "linearize.h"
#include "eeprom-data.h"


// RAM data copy of EEPROM
//
LNtable_t	LNfwd;
LNtable_t	LNref;
LNtable_t	LNdacf;
LNtable_t	LNdacr;


// At VDD=115V

const LNtable_t PMTfwd160 PROGMEM =
{
        15,
        {
		{    0,   0},
                {  380,   5},
                {  580,  10},
                {  870,  20},
                { 1190,  30},
                { 1370,  40},
                { 1530,  50},
                { 1720,  60},
                { 2120,  90},
                { 2250, 100},
                { 4300, 200},
                { 4970, 300},
                { 4980, 400},
                { 4990, 500},
                { 5020, 600}
        }
};

const LNtable_t PMTfwd80 PROGMEM =
{
        15,
        {
		{    0,   0},
                {  485,   4},
                {  845,  10},
                { 1270,  20},
                { 1685,  30},
                { 1965,  40},
                { 2210,  50},
                { 2450,  60},
                { 2860,  80},
                { 3170, 100},
                { 3900, 150},
                { 4990, 300},
                { 5000, 400},
                { 5010, 500},
                { 5150, 600}
        }
};

const LNtable_t PMTfwd40 PROGMEM =
{
        15,
        {
		{    0,   0},
                {  580,   5},
                {  870,  10},
                { 1320,  20},
                { 1720,  30},
                { 1990,  40},
                { 2220,  50},
                { 2500,  60},
                { 2880,  80},
                { 3200, 100},
                { 4600, 200},
                { 4980, 300},
                { 5050, 400},
                { 5100, 500},
                { 5150, 600}
        }
};



const LNtable_t PMTref160 PROGMEM =
{
        10,
        {
		{    0,   0},
                {  380,   5},
                {  580,  10},
                {  870,  20},
                { 1190,  30},
                { 1370,  40},
                { 1530,  50},
                { 1720,  60},
                { 2120,  90},
                { 2250, 100}
        }
};

const LNtable_t PMTref80 PROGMEM =
{
        10,
        {
		{    0,   0},
                {  365,   5},
                {  595,  10},
                {  890,  20},
                { 1180,  30},
                { 1395,  40},
                { 1570,  50},
                { 1760,  60},
                { 2060,  80},
                { 2260, 100}
        }
};

const LNtable_t PMTref40 PROGMEM =
{
        10,
        {
		{    0,   0},
                {  365,   5},
                {  595,  10},
                {  890,  20},
                { 1180,  30},
                { 1395,  40},
                { 1570,  50},
                { 1760,  60},
                { 2060,  80},
                { 2260, 100}
        }
};



void * const LNfwdp[]	PROGMEM = {
	(void *) &PMTfwd160,
	(void *) &PMTfwd80,
	(void *) &PMTfwd40
};



void * const LNrefp[]	PROGMEM = {
	(void *) &PMTref160,
	(void *) &PMTref80,
	(void *) &PMTref40
};



// linearize the value according to the gradient between 
// two closest points known.
//
uint16_t LNlinearize( LNtable_t *lst, uint16_t adc )
{
	uint32_t grad = 1;

	uint16_t pADC;
	uint16_t pVal;
	uint16_t nADC = 0;
	uint16_t nVal = 0;

	uint8_t  p;
   
	// shortcuts for more speed
	pADC = lst->pt[0].adc;
	pVal = lst->pt[0].val;

	if( adc <= pADC )
	{
		// adc is less than minimum on the table
		// return the minimum

		return pVal;
	}

	// if we got here the adc.val is greater than the first one in the table
	// compute the leg's gradient and interpolate
	//

	for( p = 1; p < lst->size; p++ )
	{
		// Upper point
		//
		nADC = lst->pt[p].adc;
		nVal = lst->pt[p].val;
		
		if( adc == nADC )
		{
			return nVal;
		}

		// Setup gradient
		// 
		pADC    = lst->pt[ p-1 ].adc;
		pVal    = lst->pt[ p-1 ].val;

		grad 	= (unsigned long)1024 * (nVal - pVal) / (nADC - pADC);

		if( adc < nADC )
		{
			return (uint16_t)((1024L * nVal - grad * (nADC - adc)) / 1024);
		}
	}

	// the adc is greater than any given in the table
	// compute using last leg gradient
	// 
	// NOTE: Ignore compiler warning about uninitialized nVal, nADC and grad.
	return (uint16_t)( ((unsigned long)1024 * nVal + grad * (adc - nADC)) / 1024 );
}



void LNinit( LNtable_t *dst, LNxtable_t *src )
{
	// Copy EEPROM to memory
	//
	uint8_t	i,j;

	// Get table size
	j = eeprom_read_byte( &(src->size) );
	dst->size = j;

        // First entry is for 0W power
        //
	dst->pt[0].adc = eeprom_read_word( &(src->pt[0]) );
	dst->pt[0].val = 0;

	// Loop filling the destination table
	//
	for( i=1; i<j; i++ )
	{
		dst->pt[i].adc = eeprom_read_word( &(src->pt[i]) );
		dst->pt[i].val = (2*i-1)*100;		// 0, 100, 300, 500, 700, 900

		if( i == j-1 )
		{
			// Add 10 to last entry
			dst->pt[i].val += 100;
		}
	}

	// eeprom_read_block( dst, src, sizeof( LNtable_t ) );
}



void LNdacinit( LNtable_t *dst, LNdtable_t *src )
{
	// Copy EEPROM to memory
	//
	uint8_t	i,j;

	// Get table size
	j = eeprom_read_byte( &(src->size) );
	dst->size = j;

	// Loop filling the destination table
	//
	for( i=0; i<j; i++ )
	{
		dst->pt[i].adc = 100*i;			// 0, 100, 200, ...
		dst->pt[i].val = eeprom_read_word( &(src->pt[i]) );

	}
}



void LNslctband( uint8_t band )
{
	PGM_P	p;

	// Only 3 table entry
	//
	if( band > 3 )
	{
		return;
	}

	// Forward Linearization table
	//
	memcpy_P( &p, &LNfwdp[band], sizeof( PGM_P ) );

	// p now holds pointer to FWD band table
	// copy the table's content to RAM variable
	//
	memcpy_P( (void *) &LNfwd, p, sizeof( LNtable_t ) );
	
	
	// Reflected Linearization table
	//
	memcpy_P( &p, &LNrefp[band], sizeof( PGM_P ) );

	// p now holds pointer to REF band table
	// copy the table's content to RAM variable
	//
	memcpy_P( (void *) &LNref, p, sizeof( LNtable_t ) );
}
