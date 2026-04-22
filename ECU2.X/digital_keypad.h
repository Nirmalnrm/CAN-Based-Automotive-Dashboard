/* 
 * File:   digital_keypad.h
 * Author: nirma
 *
 * Created on 28 May, 2025, 10:39 AM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

#define EDGE 1
#define LEVEL 0
#define KEY_PORT PORTC
#define KEY_PORT_DDR TRISC
#define SW1 0x0E
#define SW2 0x0D
#define SW3 0x0B
#define SW4 0x07
#define ALL_RELEASED 0x0F
void init_digital_keypad();
unsigned char read_switch(char );

#endif	/* KEYPAD_H */


