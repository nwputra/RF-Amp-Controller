/* ADC

	The ADC has several state :

	1. IDLE

		Nothing will be done

	2. ADSTART

		ADC will initiate the variable (ADC Channel, Sampling Collector) and then initiate the start of conversion

	3. AD1SMP

		Reading the first sample

	4. AD2SMP

		Reading the second sample

	5. AD3SMP

		Reading the third sample. Then using ADCmedian function, select which one is the mid value between those three.
		This will reduce ADC reading spikes.

		The channel reading will be repeated until ADSMP times.

		If all ADC channels are read, the next state is ADCCPL.

	6. ADRGND

		Reading Ground to discharge ADC internal Capacitor before switching to the next channel

	7. ADRMUX

		Read the next channel. This state will select the next ADC Channel and then set the state to read first sample (AD1SMP)

	8. ADCCPL

		ADC Conversion completed. All channels have been read and the FSM will stays in this state until user
		set the FSM either to IDLE, or start the conversion again (ADSTART)


	The main program checks if ADC is in IDLE state and then set the state to ADSTART.
	Then, it poll the state to see if conversion has been completed where the state would be ADCCPL.

	All reading result is stored in array ADsum[ADMXC].
	The stored result is ADSMP times of the reading. So, it must be divided by ADSMP to get the average value of each channel.
*/



#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "headers.h"
#include "globals.h"
#include "eeprom-data.h"
#include "timer.h"
#include "adc.h"



// ADC readout of internal bandgap value
// ADC computed REF voltage
// ADC band data divider
//
unsigned int 			ADbgv;
unsigned int  			ADref;
//
// R26 = 3k3, R27 = 5k6
// Division is = 5k6 / (3k3 + 5k6) = 0.629
// Make it 40/64 and save 40 at EEPROM
//
unsigned char			ADbdd;

volatile unsigned char		ADstate;
volatile unsigned char		ADmux;
volatile unsigned char		ADidx;
volatile uint16_t		ADsum[ADMXC];
volatile uint16_t		ADCs1;
volatile uint16_t		ADCs2;
volatile uint16_t		ADCs3;



void ADCinit( void )
{
	// Initialize PORTC as needed
	// Other than PC4 and PC5 are used as inputs
	// and set them LOW
	//
	DDRC  &= 0x30;
	PORTC &= 0x30;

	// Clear ADC Result value
	for( ADmux = 0; ADmux < ADMXC; ADmux++ )
	{
		ADsum[ ADmux ] = 0;
	}

	ADmux		= 	0;
	ADidx		=	0;
	ADMUX		=	0;								// Read from ADC0-1 input

	ADMUX 	|= (1 << REFS0);								// use AVcc as the reference
												// 10 bit resolution ADCH:ADCL

	// ADCSRA	 = ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));			// 128 prescale for 12Mhz (ATMEGA8)
	ADCSRA	 = ((1 << ADPS2) | (1 << ADPS1));						// 64 prescale for 12Mhz (ATMEGA8)
	ADCSRA	|= ((1 << ADEN)  | (1 << ADIE)  | (1 << ADIF));					// Enable the ADC with Interrupt

	// Start ADC
	ADCSRA  |= (1<<ADSC);

	// Copy EEPROM variables
	ADbgv = eeprom_read_word( &EEbgv );
	ADbdd = eeprom_read_byte( &EEbdd );
}




// Get median value from 3 consecutive ADC read
// to eliminate noise
//
uint16_t ADCmedian( void )
{
	if( (ADCs1 <= ADCs2) && (ADCs1 <= ADCs3) )
	{
		return (ADCs2 <= ADCs3) ? ADCs2 : ADCs3;
	}
	else
	{
		if( (ADCs2 <= ADCs1) && (ADCs2 <= ADCs3) )
		{
			return (ADCs1 <= ADCs3) ? ADCs1 : ADCs3;
		}
		else
		{
			return (ADCs1 <= ADCs2) ? ADCs1 : ADCs2;
		}
	}
}




ISR( ADC_vect )
{
	if( ADstate > ADMAXST )
	{
		ADstate = ADIDLE;
	}

	switch( ADstate )
	{
		case AD1SMP :	// First sample done
				// Read the result
				//
				ADCs1 = ADCL + ADCH * 256;
				
				// Start ADC for next sample
				ADCSRA  |= (1<<ADSC);
				ADstate = AD2SMP;
				break;


		case AD2SMP :	// Second sample done
				// Read the result
				//
				ADCs2 = ADCL + ADCH * 256;
				
				// Start ADC for next sample
				ADCSRA  |= (1<<ADSC);
				ADstate = AD3SMP;
				break;


		case AD3SMP :	// Third sample done
				// Read the result
				//
				ADCs3 = ADCL + ADCH * 256;
			
				// Find the median and store it
				ADsum[ ADmux ] += ADCmedian();

				// Next sampling
				if( ++ADidx >= ADSMP )
				{
					ADidx = 0;

					// Done with this channel, get next one
					if( ++ADmux >= ADMXC )
					{
						// Finish all channel
						ADmux = 0;
						ADstate = ADCCPL;
					}
					else
					{
						// Discharge capacitor and wait 
						// MUX4 MUX3 MUX2 MUX1 MUX0     --> 1111 GROUND
						ADMUX    = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
						ADstate = ADRGND;
					}
				}
				else
				{
					// Start ADC for next sample
					ADCSRA  |= (1<<ADSC);
					ADstate = AD1SMP;
				}
				break;
	}
}




void ADCproc( void )
{
	if( ADstate > ADMAXST )
	{
		ADstate = ADIDLE;
	}

	switch( ADstate )
	{
		case ADSTART :	// Start from the beginning
				ADmux	= 	0;
				ADidx	=	0;
				ADMUX	=	0;
				ADsum[ 0 ] = 	0;

				// Ready for first sample 
				ADstate = AD1SMP;

				// Start ADC
				ADCSRA  |= (1<<ADSC);
				break;

		case ADRGND :	// Read Ground to clear Capacitive
				ADstate = ADRMUX;

		case ADRMUX :	// Continue with next channel

				// Set the ADC Channel
				if( ADmux < 4 )
				{
					// Channel 0 to 3
					ADMUX = (ADMUX & 0xF0) | ADmux;
				}
				else if( ADmux < 6 )
				{
					// Channel 6 and 7
					ADMUX = (ADMUX & 0xF0) | (ADmux + 2);
				}
				else
				{
					// Bandgap Channel
					ADMUX = (ADMUX & 0xF0) | 0x0E;
				}

				// Clear cumulative value
				ADsum[ ADmux ] = 0;
		
				// Ready for first sample 
				ADstate = AD1SMP;

				// Start ADC
				ADCSRA  |= (1<<ADSC);
				break;
	}
}


