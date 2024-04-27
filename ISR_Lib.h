#include "hardware.h"

#ifndef ISR_LIB_H
#define ISR_LIB_H

#define ONE_MIN_C   59999       // The normal timing value.
//#define ONE_MIN_C   3000      // This is for testing purposes only

// ISR constants.
//
#define TICKRATE 63535	// This will need tuning

#define BTN_OPEN            1
#define BTN_PRESSED         150
#define BTN_LONGPRESSED     1000


volatile unsigned long int  systick         = 0,
                            anim_timer      = 0;
volatile unsigned int       encstate        = 0,
                            last_encstate   = encstate;

volatile signed int         enc_counts      = 0,
                            last_counts     = 0;

volatile unsigned long int  btn_counter     = BTN_OPEN,
                            btn_state       = BTN_OPEN;

volatile unsigned long int  one_minute      = 0;
volatile unsigned int       minutes         = 0,
                            btn_reset_flag  = 0;


void                ISR_Ticker              ( void );
void                ServiceInterruptInit    ( void );
void                Delay_Millis            ( unsigned int delay );
void                WaitBtnLow              ( void );
unsigned long int   GetButtonPress          ( void );
void                ResetBtnState           ( void );

#include "ISR_Lib.c"
#endif