#ifndef		_OSscheduler
#define		_OSscheduler

extern		unsigned char	OSstate;

void		OSschinit( void );
void		OSheartbeat( void );

// OS Heartbeat State definition
//
#define		OSGETALM	0
#define		OSONDSP		1
#define		OSOFFDSP	2
#define		OSINTDSP	3

#endif



