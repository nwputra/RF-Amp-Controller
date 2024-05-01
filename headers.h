#ifndef _HEADERS_H
#define	_HEADERS_H

#include <stdint.h>

typedef union {
	struct {
		unsigned int	relays:3;
		unsigned int	trx:1;
		unsigned int	oper:1;
		unsigned int	nc:1;
		unsigned int	bias:1;
		unsigned int	ptt:1;
	} b;

	uint8_t	c;
} LPFctrl_t;



typedef union {
	struct {
		unsigned int	opersw:1;
		unsigned int	pttline:1;
		unsigned int	autoband:1;
		unsigned int	alc:1;
		unsigned int	temph:1;
		unsigned int	overdrv:1;
		unsigned int	highswr:1;
		unsigned int	bdsel:1;
	} b;

	uint8_t	c;
} AMPstate_t;


#define		LPFADDR		0x20			// LPF control relay
#define		BDTADDR		0x21			// Band Data input
#define		LCDADDR		0x27			// LCD address
#define		ADCADDR		0xEF			// ADC
#define		LCDLINELENGTH	16			// LCD Characters length


// LCD interface
//
//
#define		DSRS		0x01			// Display RS bit
#define		DSEN		0x04			// Display EN bit
#define		DSBL		0x08			// Display backlit



//
// PORTD definitions
// Set 7-5 as output, 4 as input
//
//
#define		DOWN_RST	7
#define		UP_AUTO		6
#define		OPER_STBY	5
#define		ALCEN		4
#define		BIASEN		1
#define		TXEN		0



// Band Data and Status LEDs
//
//
#define		BANDA		0x01
#define		BANDB		0x02
#define		BANDC		0x04
#define		BANDD		0x08
#define		PTTLINE		0x10
#define		LEDPWR		0x20
#define		LEDSTBY		0x40
#define		LEDPROT		0x80


// Band definition
//
#define		B80M		0
#define		B40M		1
#define		B20M		2
#define		BGEN		3
#define		MAXBAND		B20M

#define		BIASDRV		6			// 
#define		PTTDRV		7			// Active high


// State Machine
//
#define		STBY		0
#define		OPER		1
#define		TX1		2
#define		TX2		3
#define		TX3		4
#define		RX1		5
#define		RX2		6
#define		PROT1		7
#define		PROT2		8


// Semaphore Flags
//
//
#define		SMTIME		0x01
#define		SM1LCD		0x02
#define		SM2LCD		0x04
#define		SMTICK		0x08


extern LPFctrl_t		LPFctrl;
extern AMPstate_t		AMPstate;
extern volatile unsigned char	band;
extern volatile unsigned char	tmp;
extern volatile unsigned char	sema;
extern char			state;
extern char			buff[17];

extern unsigned char		DSbutton;
extern unsigned char		DSled;

// Measurements
//
extern unsigned int 		mFWD;
extern unsigned int		mREF;
extern unsigned int 		pINP;
extern unsigned int 		pFWD;
extern unsigned int		pREF;
extern unsigned int 		vINP;
extern unsigned int 		vFWD;
extern unsigned int		vREF;
extern unsigned int		vBDT;
extern float 			Rho;
extern float 			SWR;
extern float			Vdd;
extern float			Temp;

extern const char	ST03[];
extern const char	ST04[];
extern const char	ST05[];
extern const char	ST06[];
extern const char	ST07[];
extern const char	ST08[];
extern const char	ST09[];

#endif
