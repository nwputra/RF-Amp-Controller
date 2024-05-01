#ifndef ADC_INC
#define ADC_INC

//#define		ADCBGV		1054
//#define		ADCBDD		40

#define		ADSMP		8
#define		ADMXC		7
#define		ADMAXST		7


// ADC State
//
#define		ADIDLE		0
#define		ADSTART		1
#define		AD1SMP		2
#define		AD2SMP		3
#define		AD3SMP		4
#define		ADRMUX		5
#define		ADRGND		6
#define		ADCCPL		7



// ADC global variables
// Volatile due to update by ISR
//
extern volatile unsigned char	ADstate;
extern volatile unsigned char	ADmux;
extern volatile unsigned char	ADidx;
extern volatile uint16_t	ADsum[ADMXC];

extern unsigned int		ADbgv;
extern unsigned int		ADref;
// extern float			ADref;
extern unsigned char		ADbdd;


// Subroutines
//
void ADCinit( void );
void ADCproc( void );

#endif
