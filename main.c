// By Jennifer Gunn (JennyDigital)
// Built using CCS C
//
// This is open-source under the CC Share-Alike with Attribution License.
//
// This includes the HD44780 driver BTW.


#include "hardware.h"
#include "ISR_Lib.h"
#include "HD44780.h"
#include "UDG_Symbols.h"

#define HEATING_DEFAULT     25
#define COOLING_DEFAULT     5
#define MAX_SETTING         60
#define ANIM_RELOAD_VALUE   350

#if ( MAX_SETTING < HEATING_DEFAULT )
#error "Cannot set heating default higher than max setting."
#endif

char                msg[ 20 ];
int                 heating_mins; // = HEATING_DEFAULT;
int                 cooling_mins; // = COOLING_DEFAULT;
int                 setting  = 0;
int                 last_setting = 255;

unsigned int        last_minutes;
                      

enum states { s_idle, s_setting_heating, s_setting_cooling, s_heating, s_cooling };
enum states state;


void writeSettingsToEEPROM  ( int new_heating, int new_cooling );
void readSettingsFromEEPROM ( void );
void animRunning            ( void );
void setAnimCharsHeating    ( void );
void setAnimCharsCooling    ( void );
void setAnimCharsHeating    ( void );
void setAnimCharsCooling    ( void );
void initPlatform           ( void );
void capSetting             ( void );
void printTimer             ( void );
void printValue             ( unsigned int v_to_print );
int  askResume              ( void );
void updateSetting          ( void );

void state_Idle             ( void );
void state_SetHeating       ( void );
void state_SetCooling       ( void );
void state_DoHeating        ( void );




void initPlatform( void )
{
    setup_oscillator( OSC_8MHZ );
    
    serviceInterruptInit();
    systick=1000;
    
    initTimer2();
    setup_ccp1( CCP_PWM );
    setRunning( 0 );
    setHeating( 0 );
    setBacklight( BL_LOW );
    LCD_Init();
    LCD_Clear();
    LCD_Cursor( 0 );
    LCD_Clear();     
    
   readSettingsFromEEPROM();
}


void setAnimCharsHeating( void )
{
    LCD_Defchar( 0,   &heating_symbol );
    LCD_Defchar( 1, ( &heating_symbol ) + 8 );
    LCD_Defchar( 2, ( &heating_symbol ) + 16 );
    LCD_Defchar( 3, ( &heating_symbol ) + 24 );
}

void setAnimCharsCooling( void )
{
    LCD_Defchar( 0,   &cooling_symbol );
    LCD_Defchar( 1, ( &cooling_symbol ) + 8 );
    LCD_Defchar( 2, ( &cooling_symbol ) + 16 );
    LCD_Defchar( 3, ( &cooling_symbol ) + 24 );
}


void animRunning( void )
{
    static char runner = 0;
    
    if( !anim_timer )
    {
        runner = ( ++runner % 3 );
        anim_timer = ANIM_RELOAD_VALUE;
        
        LCD_Locate( 15 , 0 );
        LCD_Putchar( runner );
    }
}


void capSetting( void )
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


void printTimer( void )
{
    LCD_Locate( 0, 1 );
    if(minutes > 1 )
        sprintf( msg, "%u minutes  ", minutes );
    else 
         sprintf( msg, "%u minute  ", minutes );

    LCD_Print( msg );
    
    last_minutes = minutes; 
}


void printValue( unsigned int v_to_print )
{
    LCD_Locate( 0, 1 );
    if( v_to_print > 1 )
        sprintf( msg, "%u minutes  ", v_to_print );
    else 
        sprintf( msg, "%u minute   ", v_to_print );
    LCD_Print( msg );
}


int askResume( void )
{
    signed int option;
    
    enc_counts = 1;     // Ensure starts with Yes.

    
    LCD_Clear();
    LCD_Locate( 0, 0 );
    LCD_Print("Resume?");
    
    waitBtnLow();

    resetBtnState();
    
    while( getButtonPress() == BTN_OPEN )
    {
        option = ( enc_counts & 1);
        
        LCD_Locate( 0,1 );
        if( option )
            LCD_Print("Yes");
        else
            LCD_Print("No ");
    }
    
    while( getButtonPress() != BTN_OPEN );
    return option;
}


void updateSetting( void )
{
    capSetting();
    setting = enc_counts;
}


void state_Idle( void )
{
    unsigned long int curr_button_state;
    
    setBacklight( BL_LOW );
    LCD_Clear();
    LCD_Print( "Idle\n\r" );
    
    LCD_Locate( 0, 1 );
    sprintf( msg, "H: %u", (unsigned int) heating_mins );
    LCD_Print( msg );
    
    LCD_Locate( 7, 1 );
    sprintf( msg, "C: %u  ", (unsigned int) cooling_mins );
    LCD_Print( msg );

    setHeating( RELAY_OFF );
    setRunning( RELAY_OFF );

    minutes = 0;
    one_minute = 0;
    
    while(state == s_idle )
    {        
        curr_button_state = getButtonPress();
        if(
            ( curr_button_state == BTN_LONGPRESSED ) &&
            ( getDoorState() == DOOR_CLOSED )
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

void state_SetHeating( void )
{
    setBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Print( "Heating Time:" );
    setRunning( RELAY_OFF );
    setHeating( RELAY_OFF );
    setting = heating_mins;
    last_setting = 255;
    enc_counts = last_counts = setting;
    
    printValue( (unsigned int) setting );
    
    while( state == s_setting_heating )
    {
        if( setting != last_setting )
        {
            printValue( (unsigned int) setting );
            last_setting = setting;
        }
        
        if(  getButtonPress() == BTN_PRESSED )
        {
            heating_mins = setting;
            state = s_setting_cooling;
            writeSettingsToEEPROM( heating_mins, cooling_mins );
        }
        
        updateSetting();
    }
    
}

void state_SetCooling( void )
{
    
    setBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Print( "Cooling Time:" );
    setRunning( RELAY_OFF );
    setHeating( RELAY_OFF );
    setting = cooling_mins;
    last_setting = 255;
    enc_counts = last_counts = setting;
    
    printValue( (unsigned int) setting );
    
    while( state == s_setting_cooling )
    {
        if( setting != last_setting )
        {
            printValue( (unsigned int) setting );
            last_setting = setting;
        }

        if( getButtonPress() == BTN_PRESSED )
        {
            cooling_mins = setting;
            state = s_idle;
            writeSettingsToEEPROM( heating_mins, cooling_mins );
        }
        
        updateSetting();
    }
    
    
}

void state_DoHeating( void )
{
    static int rem_heating = 0;
    
    unsigned long int curr_buttonstate;
    last_minutes = MAX_SETTING + 1;
    
    setAnimCharsHeating();
    
    if ( getDoorState() == DOOR_OPEN )
    {
        state=s_idle;
        return;
    }
    
    one_minute = ONE_MIN_C;
    if( ( rem_heating > 0 ) && ( rem_heating < heating_mins ) )
    {
        if( askResume() )
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
    
    setBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Print( "Now heating." );
 
    waitBtnLow();   
    
    if( heating_mins != 0 )
    {
        setRunning( RELAY_ON );
        setHeating( RELAY_ON );
    }
    
    while( state == s_heating )
    {
        animRunning();
        
        if( minutes != last_minutes )
        {
            printTimer();
            rem_heating = minutes;
        }
        
        curr_buttonstate = getButtonPress();
        
        if( getDoorState() == DOOR_OPEN )
        {
            state = s_idle;
    
            rem_heating = minutes;
            minutes = 0;
            one_minute = 0;
            
            setHeating( RELAY_OFF );
            setRunning( RELAY_OFF );
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
    
    setAnimCharsCooling();
    last_minutes = MAX_SETTING + 1;
    
    setBacklight( BL_HIGH );
    LCD_Clear();
    LCD_Print( "Now cooling." );
    if( cooling_mins != 0 )
    {
        setRunning( RELAY_ON );
        setHeating( RELAY_OFF );
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
        animRunning();
        if( minutes != last_minutes ) printTimer();
        
        if(
            (  getButtonPress() == BTN_PRESSED ) ||
            ( getDoorState() == DOOR_OPEN ) 
          )
        {
            state = s_idle;
            
            if( minutes ) rem_cooling = minutes;
            
            // cooling_mins = ;
            
            minutes = 0;
            one_minute = 0;
            
            setHeating( RELAY_OFF );
            setRunning( RELAY_OFF );
        }
        
        if( minutes == 0 )
        {
            state = s_idle;
            
            minutes = 0;
            one_minute = 0;
            
            setHeating( RELAY_OFF );
            setRunning( RELAY_OFF );
        }
    }
    
}


void writeSettingsToEEPROM( int new_heating, int new_cooling )
{
    write_eeprom( 0, new_heating );
    write_eeprom( 1, new_cooling );
    write_eeprom( 2, new_heating ^ 255 );
    write_eeprom( 3, new_cooling ^ 255 );   
    delay_ms( 10 );

}


void readSettingsFromEEPROM( void )
{
    int chk_heating, chk_cooling;
    
    heating_mins    = read_eeprom( 0 );
    cooling_mins    = read_eeprom( 1 );    
    chk_heating     = read_eeprom( 2 ) ^ 255;
    chk_cooling     = read_eeprom( 3 ) ^ 255;

    if(
            ( chk_heating != heating_mins ) |
            ( chk_cooling != cooling_mins )
      )
    { 
        heating_mins = HEATING_DEFAULT;
        cooling_mins = COOLING_DEFAULT;
        writeSettingsToEEPROM( heating_mins, cooling_mins );
    }
    
    
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
            state_Idle();
            break;
            
        case s_setting_heating:
            state_SetHeating();
            break;
            
        case s_setting_cooling:
            state_SetCooling();
            break;
            
        case s_heating:
            state_DoHeating();
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
    initPlatform();
    StateMachine();
}

