#case

#ignore_warnings 225,238
#device PIC18F2455

#include <18F2455.h>

#fuses PUT, INTRC_IO, NOWDT, NOPROTECT, NOBROWNOUT

#USE DELAY ( CLOCK = 8M )
#device PASS_STRINGS = IN_RAM

#ignore_warnings NONE

#ifndef HARDWARE_H
#define	HARDWARE_H

// Feature Outputs
    
#define BL_PIN      PIN_C2
#define RUN_PIN     PIN_C0
#define HTR_PIN     PIN_C1
    

// Encoder Inputs

#define ENC_BTN     PIN_A0
#define ENC_A       PIN_A2
#define ENC_B       PIN_A1


// Door Input

#define DOOR_PIN    PIN_A6  // This will need to be checked later.


// Backlight Settings

#define BL_OFF      0
#define BL_LOW      32
#define BL_HIGH     200


// Door States

#define DOOR_OPEN   1
#define DOOR_CLOSED 0


// Relay States

#define RELAY_OFF   0
#define RELAY_ON    1


// Display pins are defined in the HD4470.h file #INCLUDEed below.
#include "HD44780.h"    
 
// Global hardware variables.
    
int running = 0;
int heating = 0;
    
void    SetBacklight    ( int blset );
void    SetRunning      ( int htr_runset );
void    SetHeating      ( int htr_heatset );
short   GetEncA         ( void );
short   GetEncB         ( void );
short   GetEncBtn       ( void );
short   GetDoorState    ( void );
void    InitTimer2      ( void );
    

    
#include "hardware.c"
#endif	/* HARDWARE_H */

