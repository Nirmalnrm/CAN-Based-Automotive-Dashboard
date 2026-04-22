/* 
 * File:   can.c
 * Author: nirma
 *
 * Created on 28 May, 2025, 2:08 PM
 */

#include <xc.h>
#include "can.h"

/* Global Variables */
unsigned char can_payload[13];

typedef enum _CanOpMode
{
	e_can_op_mode_bits    = 0xE0,			/* Use this to access opmode bits */
	e_can_op_mode_normal  = 0x00,
	e_can_op_mode_sleep   = 0x20,
	e_can_op_mode_loop    = 0x40,
	e_can_op_mode_listen  = 0x60,
	e_can_op_mode_config  = 0x80
} CanOpMode;

/* Configure the CAN Module */
void init_can(void)
{
	/* CAN_TX = RB2, CAN_RX = RB3 */
	TRISB2 = 0;								/* CAN TX */
	TRISB3 = 1;								/* CAN RX */

	/* Enter CAN module into config mode */
	CANCON &= 0x1F;						/* clear previous mode */
	CANCON |= 0x80;                       	/* set new mode */

	/* Wait untill desired mode is set */
	while (CANSTAT != 0x80);

	/* Enter CAN module into Mode 0 */
	ECANCON = 0x00;

	/* Initialize CAN Timing 8MHz */
	BRGCON1 = 0xE1;							/* 1110 0001, SJW=4, TQ, BRP 4 */
	BRGCON2 = 0x1B;							/* 0001 1011, SEG2PHTS 1 sampled once PS1=4TQ PropagationT 4TQ */
	BRGCON3 = 0x03;							/* 0000 0011, PS2, 4TQ */

	/*
	 * disable Filters
	 * Filter 0
	 */
	RXFCON0 = 0x00;     

	/*
	 * Initialize Receive Filters
	 * Filter 0 = 0xFFC
	 */
	RXF0EIDH = 0x00;
	RXF0EIDL = 0x00;
	RXF0SIDH = 0x00;
	RXF0SIDL = 0x00;

	/* Enter CAN module into Loop back mode */
	CAN_SET_OPERATION_MODE_NO_WAIT(e_can_op_mode_normal);

	/* Set Receive Mode for buffers */
	RXB0CON = 0x60;
}

/* Check the buffers, if it have message */
int can_receive(uint16_t *msg_id, uint8_t *data, uint8_t *len)
{
	if (!RXB0FUL) {
        return FALSE;
    }

    // 1) Reconstruct 11-bit standard ID
    uint16_t sidh = RXB0SIDH;                   // bits [10:3]
    uint16_t sidl = (RXB0SIDL >> 5) & 0x07;     // bits [2:0]
    *msg_id = (sidh << 3) | sidl;

    // 2) Data length
	uint8_t dlc = RXB0DLC & 0x0F;
    if (dlc > *len) dlc = *len;
    *len = dlc;

    // 3) Copy data bytes
    data[0] = RXB0D0;
    data[1] = RXB0D1;
    data[2] = RXB0D2;
    data[3] = RXB0D3;
    data[4] = RXB0D4;
    data[5] = RXB0D5;
    data[6] = RXB0D6;
    data[7] = RXB0D7;

    // 4) Clear buffer and flag
    RXB0FUL = 0;
    RXB0IF  = 0;

    return TRUE;     
}

/* Transmit Sample Mesaage */
    void can_transmit(uint16_t msg_id, const uint8_t *data, uint8_t len)
{
	TXB0EIDH = can_payload[EIDH];		/* Extended Identifier */
	TXB0EIDL = can_payload[EIDL];		/* Extended Identifier */

	/* 0x35E  0110 1011 110 */
	TXB0SIDH = can_payload[SIDH];		/* Standard Identifier */
	TXB0SIDL = can_payload[SIDL];		/* Standard Identifier */

	TXB0DLC = can_payload[DLC];			/* Data Length Count */
	TXB0D0 = can_payload[D0];			/* DataByte 0 */
	TXB0D1 = can_payload[D1];			/* DataByte 1 */
	TXB0D2 = can_payload[D2];			/* DataByte 2 */
	TXB0D3 = can_payload[D3];			/* DataByte 3 */
	TXB0D4 = can_payload[D4];			/* DataByte 4 */
	TXB0D5 = can_payload[D5];			/* DataByte 5 */
	TXB0D6 = can_payload[D6];			/* DataByte 6 */
	TXB0D7 = can_payload[D7];			/* DataByte 7 */

	TXB0REQ = 1;			/* Set the buffer to transmit */
}



