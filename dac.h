#ifndef	_DAC_H
#define _DAC_H

// DAC 
// YB1NWP (c) 2022
// version 1.0
//
//

#include <avr/interrupt.h>

#include "dac.h"
#include "globals.h"
#include "timer.h"


void DACinit( void );
void DACsetA( unsigned int val );
void DACsetB( unsigned int val );

#endif
