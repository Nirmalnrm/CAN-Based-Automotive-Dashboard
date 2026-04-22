/*
 * File:   ecu3_main.c
 * Author: nirma
 *
 * Created on 28 May, 2025, 2:07 PM
 */

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "message_handler.h"

#define _XTAL_FREQ 20000000

// Function to initialize CAN, CLCD, and set LED pins as output
void config() {
    init_can();
    init_clcd();
    // Set PORTB pins as output (RB0, RB1, RB6, RB7 used for indicator LEDs)
    TRISB = 0x00;
    PORTB = 0x00;
}


void main(void) {
    config(); //Configuring the peripherals
    clcd_print("SP G TM  RPM IND", LINE1(0)); // Display header on first line of CLCD
    uint8_t speed = 0, gear_pos = 0, engine_temp = 0,delay = 0;// Declare variables to store received values
    int i=0;
    uint8_t rx_data[8];  // Buffer to hold received CAN data
    char buff[8];
    while (1) {
        uint16_t rx_id; // Variable to store received CAN ID
        uint8_t rx_len = sizeof (rx_data); // Length of the receive buffer
        if (can_receive(&rx_id, rx_data, &rx_len)) {
            /*sprintf(buff, "%04X", rx_id);       // Convert ID to hex string
            clcd_print((const unsigned char *)buff, LINE2(0)); 
            __delay_ms(300);   */                // Delay to make sure it's visible

            switch (rx_id) {    
                case SPEED_MSG_ID:  // Handle speed message
                    rx_data[2] = '\0'; // null-terminate  the data
                    clcd_print(rx_data, LINE2(0)); // Display speed at position 0 on second line
                    break;

                case GEAR_MSG_ID:   // Handle gear position message
                    char temp = rx_data[0]; // Read gear byte
                    switch (temp) {
                        case '1': clcd_print("N", LINE2(3));
                            break;
                        case '2': clcd_print("1", LINE2(3));
                            break;
                        case '3': clcd_print("2", LINE2(3));
                            break;
                        case '4': clcd_print("3", LINE2(3));
                            break;
                        case '5': clcd_print("4", LINE2(3));
                            break;
                        case '6': clcd_print("5", LINE2(3));
                            break;
                        case '7': clcd_print("6", LINE2(3));
                            break;
                        case '8': clcd_print("R", LINE2(3));
                            break;
                        case '9': clcd_print("C", LINE2(3));
                            break;
                    } 
                    break;

                case ENG_TEMP_MSG_ID: // Handle engine temperature message
                    rx_data[rx_len] = '\0'; // Null-terminate the data
                    clcd_print(rx_data, LINE2(5)); // Display temperature at position 5
                    break;

                case RPM_MSG_ID:     // Handle RPM message
                    rx_data[rx_len] = '\0'; // Null-terminate the data
                    clcd_print(rx_data, LINE2(9)); // Display RPM at position 9
                    break;

                case INDICATOR_MSG_ID:   // Handle turn indicator messages
                    if (rx_data[0] == '1'){ // Turn on right indicator LEDs (RB6, RB7)
//                        if(++delay == 5000)
//                        {
//                            if(i++<8)
//                            {
//                                PORTB = 0xC0;
//                            }
//                            else if(i++>16)
//                            {
//                                PORTB = 0x00;
//                                delay = 0;
//                                i=0;
//                            }
//                        }
                        clcd_print("L ", LINE2(14));  // Display "R " at position 14
                    } 
                    else if (rx_data[0] == '2') { // Turn on left indicator LEDs (RB0, RB1)
                        PORTB = 0x00;
                        clcd_print("O ", LINE2(14)); // Display "L " at position 14
                    }
                    else if (rx_data[0] == '3') { // Turn off all indicator LEDs
//                        if(++delay == 5000)
//                        {
//                            if(i++<8)
//                            {
//                                PORTB = 0x03;
//                            }
//                            else if(i++>16)
//                            {
//                                PORTB = 0x00;
//                                delay = 0;
//                                i=0;
//                            }
//                        }
                        clcd_print("R", LINE2(14)); // Display "OF" for off
                    }
                    else if (rx_data[0] == '4') { // Turn off all indicator LEDs
//                        if(++delay == 5000)
//                        {
//                            if(i++<8)
//                            {
//                                PORTB = 0xC3;
//                            }
//                            else if(i++>16)
//                            {
//                                PORTB = 0x00;
//                                delay = 0;
//                                i=0;
//                            }
//                        }
                        clcd_print("H", LINE2(14)); // Display "OF" for off
                    }
                    break;
            }
        }

        __delay_ms(10); // Small delay
    }
}

