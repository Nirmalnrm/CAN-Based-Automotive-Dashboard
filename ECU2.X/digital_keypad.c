#include <xc.h>
#include "digital_keypad.h"

void init_digital_keypad()
{
    KEY_PORT_DDR |= ALL_RELEASED;
}

unsigned char read_switch(char detect_type)
{
    unsigned char flag = 1;
    if(detect_type == EDGE)
    {
        if((KEY_PORT & ALL_RELEASED) != ALL_RELEASED && flag)
        {
            flag = 0; 
            return KEY_PORT & ALL_RELEASED;
        }
        else if(KEY_PORT == ALL_RELEASED)
            flag = 1;
    }
    if(detect_type == LEVEL)
        return KEY_PORT & ALL_RELEASED;
    return ALL_RELEASED;
}
