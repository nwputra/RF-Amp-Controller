#ifndef	_TWI_
#define	_TWI_

#include 	<inttypes.h>


void	TWIInit(void);
void	TWIStart(void);
void	TWIStop(void);
void	TWIWrite(uint8_t u8data);
uint8_t	TWIReadACK(void);
uint8_t	TWIReadNACK(void);
uint8_t TWIGetStatus(void);

int I2CwrData( char addr, char val );
int I2CrdData( char addr );

#endif

