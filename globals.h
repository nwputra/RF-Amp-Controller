#ifndef	_GLOBALS_H
#define	_GLOBALS_H

#include "headers.h"

// LCD
char			buff[17];



// Main Process
volatile unsigned char	tmp;
unsigned char		DSbutton;
unsigned char		DSled;



// OS FSM
unsigned char 		OSstate;
unsigned char 		POSTst;



// Timer
unsigned char 		OSALDIS;
unsigned char		OSALNUM;
unsigned char 		OSSTATE;
unsigned char 		OSALCTR;
unsigned char 		TMcnt;
volatile unsigned char	TMtimer[3];
volatile unsigned int 	TMtick;
volatile unsigned int 	TMdelay;
volatile unsigned char 	TMsec;



// Amplifier Control
volatile unsigned char	band;
volatile unsigned char	sema;

LPFctrl_t 		LPFctrl;
AMPstate_t  		AMPstate;



// Amplifier control inputs 
// and changed bits
//
unsigned char		AMPctrl;
unsigned char		AMPctchg;



// Amplifier measurements
//
unsigned int 		mFWD;
unsigned int		mREF;
unsigned int 		pINP;
unsigned int 		pFWD;
unsigned int		pREF;
unsigned int 		vINP;
unsigned int 		vFWD;
unsigned int		vREF;
unsigned int 		vBDT;

float 			Rho;
float 			SWR;
float			Vdd;
float			Temp;

#endif
