/* 
 * File:   projectsetup.h
 * Author: Jennifer Gunn
 *
 * Created on 26 April 2024, 18:15
 */

#opt 0
#case
#use delay (clock=8MHZ)
#ignore_warnings 225, 238
#include <18F2455.h>
#ignore_warnings none

#fuses PUT, INTRC_IO, NOWDT, NOPROTECT, NOLVP

#USE DELAY ( CLOCK = 8M )
#device PASS_STRINGS = IN_RAM
