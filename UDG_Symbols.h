/* 
 * File:   UDG_Symbols.h
 * Author: Jennifer Gunn
 *
 * Created on April 20, 2024, 2:40 PM
 */

#ifndef UDG_SYMBOLS_H
#define	UDG_SYMBOLS_H

#ifdef	__cplusplus
extern "C" {
#endif

char heating_symbol[] =
                { 
                      0b10010,
                      0b01001,
                      0b01001,
                      0b10010,
                      0b10010,
                      0b01001,
                      0b01001,
                      0b10010,
                      
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      
                      0b01001,
                      0b10010,
                      0b10010,
                      0b01001,
                      0b01001,      
                      0b10010,
                      0b10010,
                      0b01001,
                      
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010,
                      0b01010
                };

char cooling_symbol[] =
                { 
                      0b01110,
                      0b00100,
                      0b00000,
                      0b00000,
                      0b00000,
                      0b00000,
                      0b00000,
                      0b00000,
                      
                      0b00100,
                      0b11111,
                      0b01110,
                      0b00100,
                      0b00000,
                      0b00000,
                      0b00000,
                      0b00000,
                      
                      0b00100,
                      0b00100,
                      0b00100,
                      0b11111,
                      0b01110,      
                      0b00100,
                      0b00000,
                      0b00000,
                      
                      0b00000,
                      0b00100,
                      0b00100,
                      0b00100,
                      0b00100,
                      0b11111,
                      0b01110,
                      0b00100
                };


#ifdef	__cplusplus
}
#endif

#endif	/* UDG_SYMBOLS_H */

