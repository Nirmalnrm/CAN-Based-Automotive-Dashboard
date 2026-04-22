/*
 * File:   ecu1_main.c
 * Author: nirma
 *
 * Created on 28 May, 2025, 8:40 PM
 */

#include <xc.h>
#include "ecu1_sensor.h"
#define _XTAL_FREQ 20000000

// Function to read and convert ADC value from SPEED channel to speed in km/h
uint16_t get_speed()
{
    __delay_us(5);
    read_adc(SPEED_ADC_CHANNEL);  // Dummy read
    uint16_t adc_reg_val = read_adc(SPEED_ADC_CHANNEL); // Actual ADC value
    return (adc_reg_val / 10.33); // Scale ADC to speed (approximate)
}

// Function to determine gear position based on switch inputs
unsigned char get_gear_pos()
{
    static unsigned char gear_pos = 1; // Initial gear position (neutral)
    char key = read_switches(STATE_CHANGE);
    if(key == GEAR_UP)
	{
        if(gear_pos < MAX_GEAR) // Increment gear if not at max
            gear_pos++;
    }
    else if(key == GEAR_DOWN) // Decrement gear if not at min
	{
        if(gear_pos > 1)
            gear_pos--;
    }
    else if(key == COLLISION) // Collision detected, set to 9
	    gear_pos = 9;

    return gear_pos;

}

//Checking engine temperature from the channel and updating
uint16_t get_engine_temp()
{
    __delay_us(10); 
    read_adc(ENG_TEMP_ADC_CHANNEL);  // Dummy read
    uint16_t adc_reg_val = read_adc(ENG_TEMP_ADC_CHANNEL); // Actual ADC value
    return (((adc_reg_val * ((float)5 / 1023)) * 100));  // Convert ADC to temperature: (ADC * Vref / 1023) * 100
}

// Function to initialize all required peripherals
static void init_config()
{
    init_can();
    init_adc();
    init_matrix_keypad();
}
void main()
{
    init_config(); //Configuring the peripherals
    unsigned char gear_pos = 0;
    while(1)
    {
        //Read speed, convert to string, transmit to ECU3
        uint16_t speed = get_speed();
        uint8_t speed_buff[] = {((speed/10) + '0'), ((speed % 10) + '0'), '\0'};
        can_transmit(SPEED_MSG_ID, speed_buff, 3);
        __delay_ms(1);

        //Read gear position, convert to string, transmit to ECU3
        gear_pos = get_gear_pos();
        uint8_t gear_buff[1] = { (gear_pos % 10) + '0' , '\0'};
        can_transmit(GEAR_MSG_ID, gear_buff, 2);
        __delay_ms(1);

        //Read engine temperature, convert to string, transmit to ECU3
        uint16_t engine_temp = get_engine_temp();
        const uint8_t engine_temp_buff[] = {((engine_temp/10) + '0'), ((engine_temp % 10) + '0'), 'c', '\0'};
        can_transmit(ENG_TEMP_MSG_ID, engine_temp_buff, 4);
        __delay_ms(1);
    }
}	


