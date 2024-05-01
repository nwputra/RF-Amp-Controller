// Keyboard
// Date : Nov 9-2010 */
// 
// FSM for handling key inputs
// Last Update  : Sept 7-2022
//		: Modified for short and long press response
//		: Not applying typematic function
//
//

#include <avr/io.h>

#include "keyboard.h"
#include "keymake.h"
#include "keybreak.h"
#include "headers.h"


unsigned char		KBstate;
unsigned char		KBsched;
unsigned char		KBtimer;
unsigned char		KBoldst;
unsigned char		KBinkey;
unsigned char		KBpres;
unsigned char		KBrels;
unsigned char		KBchg;



void KBfsm( void )
{
	/* Sanity check for KB FSM State */

	if( KBstate > 1 )
	{
		KBstate = 0;
	}

	/* Check for Button pressed */

	DSbutton = (unsigned char)~(PIND | 0x1F) >> 5;
	KBchg = DSbutton ^ KBoldst;							// Find out what has been changed

	if( KBchg )
	{
		KBpres		=  (DSbutton) & KBchg;					// Store changed key (pressed)
		KBrels		= ~(DSbutton) & KBchg;					// Store changed key (released)
		KBoldst 	=  (DSbutton);						// Save new status
	}

	switch( KBstate )
	{
		case 	KBIDLE	:
			if( KBpres )
			{
				KBinkey =  KBinkey | KBpres;				// Update status of active key
				// execMake();						// Perform task
				KBtimer = KB_TYPMWT;					// Set typematic counter
				KBstate = KBWTREL;					// Go wait for release
			}
			break;


		case	KBWTREL	:	/* check key status, see if it has been pressed for
					 * long time, enough for starting the typematic
					 * feature
					 */
			if( KBrels & KBinkey )
			{
				/* key has been released */
				KBinkey &= ~KBrels;					// Update status of active key
				execBreak();
			}


  			if( KBinkey )
  			{
  				// Decrement Typematic counter 
  				if( --KBtimer == 0 )
  				{
  					/* Typematic counter expired, execute the long press response */
					KBpres  = KBinkey;
  					execMake();					// Perform task

					KBinkey = 0;					// Mark as done
					KBpres  = 0;
  				}
  			}
  			else
  			{
  				KBstate = KBIDLE;
  			}

			break;
	}
}




void KBinit()
{
	/* Some variable initializations */

	KBstate = KBIDLE;
	KBsched = KBSCLR;
	KBpres  = 0x00;
	KBrels  = 0x00;
	KBinkey = 0x00;
	KBoldst = 0x00;
}




