#ifndef _HWIO_H
#define	_HWIO_H

void SetLPF( uint8_t band );
void SetOPER( uint8_t voper );
void SetALC( uint8_t valc );
unsigned char AMPgetCtrl( void );
unsigned char GetBandData( void );

#endif

