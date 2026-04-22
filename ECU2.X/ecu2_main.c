/*
 * File:   ecu2_main.c
 * Author: nirma
 *
 * Created on 28 May, 2025, 10:40 AM
 */


#include <xc.h>
#include "ecu2_sensor.h"
#include <stdio.h>
#define _XTAL_FREQ 20000000

// Function to read and calculate RPM from the ADC channel
uint16_t get_RPM() {
    __delay_ms(10);
    read_adc(RPM_ADC_CHANNEL);  // Dummy read
    uint16_t adc_reg_val = read_adc(RPM_ADC_CHANNEL); // Actual ADC value
    return (1500 + ((uint32_t)adc_reg_val *5500) / 1023); // Scale the ADC value to an RPM value ranging from 1500 to 7000
}

IndicatorStatus prev = e_ind_off;
// Function to determine the current indicator status based on switch input
IndicatorStatus process_indicator()
{
    IndicatorStatus cur_ind_status = prev; //  Retain last indicator status
    unsigned char key = read_switch(EDGE);
    
    switch (key)
    {
        case SW1:
            cur_ind_status = e_ind_left; // Set indicator to left
            break;
        case SW2:
            cur_ind_status = e_ind_off; // Set indicator to right
            break;
        case SW3:
            cur_ind_status = e_ind_right; // Turn off indicator
            break;
        case SW4:
            cur_ind_status = e_ind_hazard;
        case ALL_RELEASED:
        default:
            // Do not change the status; retain the last one
            break;
    }
    prev = cur_ind_status;
    return cur_ind_status;
}

// Initialization function to set up CAN, ADC, and keypad peripherals
static void init_config()
{
    init_can();
    init_adc();
    init_digital_keypad();
}

void main()
{
    init_config(); //Configuring the peripherals
 
    while (1)
    {
        // Read RPM, convert to string, transmit to ECU3
        uint16_t RPM = get_RPM();
        uint8_t rpm_buff[] = {(RPM / 1000) % 10 + '0', (RPM / 100) % 10 + '0', (RPM / 10) % 10 + '0', RPM % 10 + '0', '\0'};
        can_transmit(RPM_MSG_ID, rpm_buff, 5);
        __delay_ms(1);

        // Process switch input and show direction
        IndicatorStatus Ind_dir = process_indicator();
        switch (Ind_dir)
        {
            uint8_t ind_buff[1]; // Buffer to hold indicator direction
            case e_ind_left:
                ind_buff[0] = '1'; // Left indicator signal
                can_transmit(INDICATOR_MSG_ID, ind_buff, 1);
                break;
            case e_ind_right:
                ind_buff[0] = '3'; // Right indicator signal
                can_transmit(INDICATOR_MSG_ID, ind_buff, 1);
                break;
            case e_ind_off:
                ind_buff[0] = '2'; // Turn off indicator signal
                can_transmit(INDICATOR_MSG_ID, ind_buff, 1);
                break;
            case e_ind_hazard:
                ind_buff[0] = '4'; // Turn off indicator signal
                can_transmit(INDICATOR_MSG_ID, ind_buff, 1);
                break;
        }
        __delay_ms(1); 
    }
}
