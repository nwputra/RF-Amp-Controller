#ifndef _OStimer
#define _OStimer

void TMtmr2init( void );

void delayMS(   unsigned char ms );
void delay10US( unsigned char tus );

extern unsigned char	OSALDIS;
extern unsigned char	OSALNUM;
extern unsigned char	OSSTATE;
extern unsigned char	TMcnt;

extern volatile unsigned int	TMtick;
extern volatile unsigned int	TMdelay;
extern volatile unsigned char	TMsec;

#endif
