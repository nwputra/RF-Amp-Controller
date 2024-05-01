#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "linearize.h"
#include "eeprom-data.h"


#define		 ADCBGV		1054
#define		 ADCBDD		40


// ADC bandgap voltage
//
unsigned int	EEbgv   EEMEM = ADCBGV;


// ADC band data divider
//
unsigned char	EEbdd   EEMEM = ADCBDD;



LNdtable_t EEdacf EEMEM =
{
	11,
	{
		   0,
		 331,
		 460,
		 566,
		 654,
		 741,
		 808,
		 868,
		 926,
		 979,
		1023
	}
};


LNdtable_t EEdacr EEMEM =
{
	11,
	{
		   0,
		 331,
		 460,
		 566,
		 654,
		 741,
		 808,
		 868,
		 926,
		 979,
		1023
	}
};



LNxtable_t EEfwd160 EEMEM =
{
	6,
	{
		   0,
		1025,
		1670,
		2138,
		2560,
		2964
	}
};



