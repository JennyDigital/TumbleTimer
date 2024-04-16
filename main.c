// By Jennifer Gunn (JennyDigital)
// Built using CCS C
//
// This is open-source under the CC Share-Alike with Attribution Licence.
//
// This includes the HD44780 driver BTW.

#case

#include <18F2455.h>

#fuses PUT, INTRC_IO, NOWDT, NOPROTECT, NOLVP

#USE DELAY ( CLOCK = 8M )
#device PASS_STRINGS = IN_RAM

#include "HD44780.h"
#include "hardware.h"

#include "ISR_Lib.h"

#define HEATING_DEFAULT 25
#define COOLING_DEFAULT 5
#define MAX_SETTING     60


char                msg[ 20 ];
int                 heating_mins; // = HEATING_DEFAULT;
int                 cooling_mins; // = COOLING_DEFAULT;
int                 setting  = 0;
int                 last_setting = 255;

unsigned int        last_minutes;

enum states { s_idle, s_setting_heating, s_setting_cooling, s_heating, s_cooling };
enum states state;


void WriteSettingsToEEPROM( int new_heating, int new_cooling );
void GetSettingsFromEEPROM( void );

void InitPlatform( void )
{
    setup_oscillator( OSC_8MHZ );
    
    ServiceInterruptInit();
    systick=1000;
    
    InitTimer2();
    setup_ccp1( CCP_PWM );
    SetRunning( 0 );
    SetHeating( 0 );
    SetBacklight( BL_LOW );
    LCD_Init();
    LCD_Clear();
    LCD_Cursor( 0 );
    LCD_Clear();     
    
   GetSettingsFromEEPROM();
}


void CapSetting( void )
{
    disable_interrupts(GLOBAL);
    
    if( enc_counts < 0 )
        {
            enc_counts = 0;
        }
        
        if( enc_counts > MAX_SETTING )
        {
            enc_counts = MAX_SETTING;
        }
    
    enable_interrupts(GLOBAL);
}


void PrintTimer( void )
{
    LCD_Locate( 0, 1 );
    if(minutes > 1 )
        sprintf( msg, "%u minutes  ", minutes );
    else 
         sprintf( msg, "%u minute  ", minutes );

    LCD_Printf( msg );
    
    last_minutes = minutes; 
}


void PrintValue( int v_to_print )
{
    LCD_Locate( 0, 1 );
    if( v_to_print > 1 )
        sprintf( msg, "%u minutes  ", v_to_print );
    else 
        sprintf( msg, "%u minute   ", v_to_print );
    LCD_Printf( msg );
}


int AskResume( void )
{
    signed int option;
    
    enc_counts = 1;     // Ensure starts with Yes.

    
    LCD_Clear();
    LCD_Locate( 0, 0 );
    LCD_Printf("Resume?");
    
    WaitBtnLow();

    ResetBtnState();
    
    while( GetButtonPress() == BTN_OPEN )
    {
        option = ( enc_counts & 1);
        
        LCD_Locate( 0,1 );
        if( option )
            LCD_Printf("Yes");
        else
            LCD_Printf("No ");
    }
    
    while( GetButtonPress() != BTN_OPEN );
    return option;
}


void UpdateSetting( void )
{
    CapSetting();
    setting = enc_counts;
}


void S_Idle( void )
{
    unsigned long int curr_button_state;
    
    SetBacklight( BL_LOW );
    LCD_Clear();
    LCD_Printf( "Idle\n\r" );
    
    LCD_Locate( 0, 1 );
    sprintf( msg, "H: %u", heating_mins );
    LCD_Printf( msg );
    
    LCD_Locate( 7, 1 );
    sprintf( msg, "C: %u  ", cooling_mins );
    LCD_Printf( msg );

    SetHeating( 0 );
    SetRunning( 0 );

    minutes = 0;
    one_minute = 0;
    
    while(state == s_idle )
    {        
        curr_button_state = GetButtonPress();
        if(
            ( curr_button_state == BTN_LONGPRESSED ) &&
            ( GetDoorState() == DOOR_CLOSED )
          )
        {
            state = s_heating;
        }
        
        if( curr_button_state == BTN_PRESSED )
        {
            state = s_setting_heating;
        }
    }
}

void S_SetHeating( void )
{
    SetBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Printf( "Heating Time:" );
    SetRunning( 0 );
    SetHeating( 0 );
    setting = heating_mins;
    last_setting = 255;
    enc_counts = last_counts = setting;
    
    PrintValue( setting );
    
    while( state == s_setting_heating )
    {
        if( setting != last_setting )
        {
            PrintValue( setting );
            last_setting = setting;
        }
        
        if(  GetButtonPress() == BTN_PRESSED )
        {
            heating_mins = setting;
            state = s_setting_cooling;
            WriteSettingsToEEPROM( heating_mins, cooling_mins );
        }
        
        UpdateSetting();
    }
    
}

void S_SetCooling( void )
{
    
    SetBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Printf( "Cooling Time:" );
    SetRunning( 0 );
    SetHeating( 0 );
    setting = cooling_mins;
    last_setting = 255;
    enc_counts = last_counts = setting;
    
    PrintValue( setting );
    
    while( state == s_setting_cooling )
    {
        if( setting != last_setting )
        {
            PrintValue( setting );
            last_setting = setting;
        }

        if( GetButtonPress() == BTN_PRESSED )
        {
            cooling_mins = setting;
            state = s_idle;
            WriteSettingsToEEPROM( heating_mins, cooling_mins );
        }
        
        UpdateSetting();
    }
    
    
}

void S_DoHeating( void )
{
    static int rem_heating = 0;
    
    unsigned long int curr_buttonstate;
    last_minutes = MAX_SETTING + 1;
    
    if ( GetDoorState() == DOOR_OPEN )
    {
        state=s_idle;
        return;
    }
    
    one_minute = ONE_MIN_C;
    if( ( rem_heating > 0 ) && ( rem_heating < heating_mins ) )
    {
        if( AskResume() )
        {
            minutes = rem_heating;
        }
        else
        {
            minutes = heating_mins;
        }
    }
    else
    {
        minutes = heating_mins;
    }
    
    if( minutes == 0 )
    {
        state = s_cooling;
        rem_heating = 0;
        return;
    }
    
    SetBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Printf( "Now heating." );
 
    WaitBtnLow();   
    
    if( heating_mins != 0 )
    {
        SetRunning( 1 );
        SetHeating( 1 );
    }
    
    while( state == s_heating )
    {
        if( minutes != last_minutes )
        {
            PrintTimer();
            rem_heating = minutes;
        }
        
        curr_buttonstate = GetButtonPress();
        
        if( GetDoorState() == DOOR_OPEN )
        {
            state = s_idle;
    
            rem_heating = minutes;
            minutes = 0;
            one_minute = 0;
            
            SetHeating( 0 );
            SetRunning( 0 );
        }
        else if( minutes == 0 || ( curr_buttonstate == BTN_PRESSED ) )
        {
            state = s_cooling;
            minutes = 0;
            one_minute = 0;
            rem_heating = 0;
        }
    }
}

void S_DoCooling( void )
{
    static int rem_cooling;
    
    last_minutes = MAX_SETTING + 1;
    
    SetBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Printf( "Now cooling." );
    if( cooling_mins != 0 )
    {
        SetRunning( 1 );
        SetHeating( 0 );
    }
    
    if( rem_cooling )
    {
        minutes = rem_cooling;
    }
    else
    {
          minutes = cooling_mins;  
    }
    
    one_minute = ONE_MIN_C;

    
    while( state == s_cooling )
    {
        if( minutes != last_minutes ) PrintTimer();
        
        if(
            (  GetButtonPress() == BTN_PRESSED ) ||
            ( GetDoorState() == DOOR_OPEN ) 
          )
        {
            state = s_idle;
            
            if( minutes ) rem_cooling = minutes;
            
            // cooling_mins = ;
            
            minutes = 0;
            one_minute = 0;
            
            SetHeating( 0 );
            SetRunning( 0 );
        }
        
        if( minutes == 0 )
        {
            state = s_idle;
            
            minutes = 0;
            one_minute = 0;
            
            SetHeating( 0 );
            SetRunning( 0 );
        }
    }
    
}


void WriteSettingsToEEPROM( int new_heating, int new_cooling )
{
    write_eeprom( 0, new_heating );
    write_eeprom( 1, new_cooling );
    delay_ms( 10 );

}


void GetSettingsFromEEPROM( void )
{
    heating_mins    = read_eeprom( 0 );
    cooling_mins    = read_eeprom( 1 );
}


void StateMachine( void )
{    
    ///
    // Possible states:
    //
    // Idle, setting_heating, setting_cooling, Heating, Cooling

    state = s_idle;

#ignore_warnings 203
    while( 1 )
    {
        switch(state)
        {
        case s_idle:
            S_Idle();
            break;
            
        case s_setting_heating:
            S_SetHeating();
            break;
            
        case s_setting_cooling:
            S_SetCooling();
            break;
            
        case s_heating:
            S_DoHeating();
            break;
            
        case s_cooling:
            S_DoCooling();
            break;      
        }
    }
}
#ignore_warnings NONE

void main(void )
{
    InitPlatform();
    StateMachine();
}

