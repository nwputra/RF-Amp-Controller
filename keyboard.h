#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Keyboard Inputs */
#define		F1		0				// ENTER
#define		F2		1				// UP
#define		F3		2				// DOWN

/* Keyboard timers */
#define		KB_TYPMWT	200				// 1000mS delay and interval ( 1000mS / 5mS )
#define		KBSCLR		20				//  100mS scan interval      (  100mS / 5mS )

/* Keyboard FSM State definition */
#define		KBIDLE		0
#define		KBWTREL		1


extern unsigned char   KBstate;
extern unsigned char   KBinkey;
extern unsigned char   KBpres;
extern unsigned char   KBrels;
extern unsigned char   KBkeypressed;
extern unsigned char   KBtypcnt;


void KBfsm( void );
void KBinit( void );

#endif

