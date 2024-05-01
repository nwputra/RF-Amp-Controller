#ifndef	_AMPCTRL_H
#define	_AMPCTRL_H
// Amplifier Controller
// YB1NWP - 2022
//
//


// IO Macros
//
//#define	BIAS_OFF	SetBias( 0 )
//#define	BIAS_ON		SetBias( 1 )
//#define	TRX_RLY_OFF	SetTRX( 0 )
//#define	TRX_RLY_ON	SetTRX( 1 )

#define		BIAS_ON		( PORTD |=  (1<<BIASEN) )	//
#define		BIAS_OFF	( PORTD &= ~(1<<BIASEN) )	//
#define		TRX_RLY_ON	( PORTD |=  (1<<TXEN)   )	//
#define		TRX_RLY_OFF	( PORTD &= ~(1<<TXEN)   )	// 
#define		ALC_OFF		SetALC( 0 )
#define		ALC_ON		SetALC( 1 )
#define		VDD_OFF		SetOPER( 0 );
#define		VDD_ON		SetOPER( 1 );


#endif
