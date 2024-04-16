#case
#use delay (clock=8MHZ)
#include "hardware.h"
#include "ISR_Lib.h"

// ======================
//
// ISR related functions.
//
// ======================
#nolist

#int_timer0
static void ISR_Ticker( void )
{    
/** Reset Timer0.  (the pic does not have and automatic reload).
  *
  */
	delay_cycles( 11 );
  
	set_timer0( TICKRATE );
  
 /** Update counters
    *
    */
  	if( systick > 0 ) systick--;

    
/** Update encoder state
 * 
 */
    last_encstate = encstate;
    
    encstate = GetEncA() | ( GetEncB() << 1 );
    
    if( encstate == 0 )
    {
        if( last_encstate == 1 )
        {
            enc_counts--;
        }
        
        if( last_encstate == 2 )
        {
            enc_counts++;
        }
    }
    
    
/** Handle Button
 *
 */
    if( btn_reset_flag )
    {
        btn_counter = BTN_OPEN;
        btn_state = BTN_OPEN;
        btn_reset_flag = 0;
    }
    
    if( GetEncBtn() )
    {
        btn_counter++;

        if (btn_counter >= BTN_LONGPRESSED )
        {
            btn_state = BTN_LONGPRESSED;
            if( btn_counter > BTN_LONGPRESSED ) btn_counter= BTN_LONGPRESSED;
        }
    }
    
    if( !GetEncBtn() )
    {
        if ( btn_counter >= BTN_PRESSED )
        {
            btn_state = BTN_PRESSED;
        }
    }
    
    if( minutes )
    {
        if( one_minute )
        {
            one_minute--;
            if( !one_minute )
            {
                one_minute = ONE_MIN_C;
                minutes--;
            }
        }            
    }

        
        
/** Tidy up after ourselves
  *
  */	  
	clear_interrupt(INT_TIMER0);
}


void ServiceInterruptInit( void )
{
	setup_timer_0(RTCC_DIV_1 | RTCC_INTERNAL);		// Setup interrupts

	set_timer0(TICKRATE);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);
}


/** Waits [delay] number of milliseconds.
 *
 *  ... or rather will when I'm done messing around (looks down and shuffles sand)
 */
void Delay_Millis(unsigned int delay)
{
  systick=delay;
  while(systick);
}


void WaitBtnLow( void )
{
    
    while( GetEncBtn() )
    {
        Delay_Millis( 100 );
        ResetBtnState();
    }
    
}


unsigned long int GetButtonPress( void )
{    
    if( btn_state == BTN_OPEN )
    {
        return BTN_OPEN;
    }
    
    if( btn_state == BTN_LONGPRESSED )
    {
        ResetBtnState();
        return BTN_LONGPRESSED;
    }
    
    ResetBtnState();
    return BTN_PRESSED;
}


void ResetBtnState( void )
{
    btn_reset_flag = 1;
    while( btn_reset_flag );
}

#list