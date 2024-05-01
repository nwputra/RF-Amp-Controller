#include <avr/io.h>
#include <inttypes.h>
#include "twi.h"


void TWIInit(void)
{
	//set SCL to 400kHz
	// f = fcpu / (16 + 2 * TWBR * prescaler )
	//
	TWSR = 0x00;
	TWBR = 17;

	//enable TWI
	TWCR = (1<<TWEN);
}


void TWIStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}


//send stop signal
void TWIStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}


void TWIWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}


uint8_t TWIReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}


//read byte with NACK
uint8_t TWIReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}


uint8_t TWIGetStatus(void)
{
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	return status;
}



int I2CwrData( char addr, char val )
{
	uint8_t rc;

	TWIStart();

	// Check for START has been transmitted
	if( (rc = TWIGetStatus()) != 0x08 && rc != 0x10 )
	{
		TWIStop();
		return 0xF100 | rc;
	}

	// I2C Address and Write mode
	TWIWrite( (addr<<1) | 0x00 );

	// Check for SLA+W sent and ACK has been received
	if( (rc = TWIGetStatus()) != 0x18 )
	{
		TWIStop();
		return 0xF200 | rc;
	}

	// Mode : Command or Data to follow
	TWIWrite( val );

	// Check for DATA has been sent and ACK has been received
	if( (rc = TWIGetStatus()) != 0x28 )
	{
		TWIStop();
		return 0xF300 | rc;
	}

	TWIStop();
	return 0;
}



int I2CrdData( char addr )
{
	uint8_t rc, dt;

	TWIStart();

	// Check for START has been transmitted
	if( (rc = TWIGetStatus()) != 0x08 && rc != 0x10 ) return 0xF100 | rc;

	// I2C Address and Read mode
	TWIWrite( (addr<<1) | 0x01 );

	// Check for SLA+R sent and ACK has been received
	if( (rc = TWIGetStatus()) != 0x40 )
	{
		TWIStop();
		return 0xF200 | rc;
	}

	// Read the port only once 
	dt = TWIReadNACK();

	// Check for DATA has been sent and NACK has been received
	if( (rc = TWIGetStatus()) != 0x58 )
	{
		TWIStop();
		return 0xF300 | rc;
	}

	TWIStop();
	return dt;
}

