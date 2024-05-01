#ifndef _EEPROMDATA_H
#define	_EEPROMDATA_H

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "linearize.h"


// ADC bandgap voltage
//
extern unsigned int	EEbgv   	EEMEM;


// ADC band data divider
//
extern unsigned char	EEbdd   	EEMEM;


// FWD and REF band table
//
extern LNxtable_t	EEfwd160	EEMEM;
extern LNxtable_t	EEfwd80		EEMEM;
extern LNxtable_t	EEfwd40		EEMEM;
extern LNxtable_t	EEfwd30		EEMEM;
extern LNxtable_t	EEfwd20		EEMEM;
extern LNxtable_t	EEfwd15		EEMEM;
extern LNxtable_t	EEfwd15		EEMEM;

extern LNxtable_t	EEref160	EEMEM;
extern LNxtable_t	EEref80		EEMEM;
extern LNxtable_t	EEref40		EEMEM;
extern LNxtable_t	EEref30		EEMEM;
extern LNxtable_t	EEref20		EEMEM;
extern LNxtable_t	EEref15		EEMEM;
extern LNxtable_t	EEref15		EEMEM;


extern void * const 	LNfwdp[]	PROGMEM;
extern void * const 	LNrefp[]	PROGMEM;

extern LNdtable_t	EEdacf		EEMEM;
extern LNdtable_t	EEdacr		EEMEM;

#endif
