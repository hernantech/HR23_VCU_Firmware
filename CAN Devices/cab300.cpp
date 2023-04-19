/*
******************************************************************************
	cab300.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "cab300.h"

CAB300::CAB300(CANInterface* can_interface, unsigned int can_id_address)
{
	can = can_interface;
	can_address = can_id_address;
}

float CAB300::getCurrent()
{
	return current;
}

void CAB300::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Inverter Feedback Messages
	can_rx_message.ui32MsgID = can_address;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool CAB300::isFaultActive()
{
	return error;
}

bool CAB300::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint32_t utemp32;

	// True if Message has been Processed
	bool processed = false;

	if (message->ui32MsgID == can_address)
	{
		// Read Measured Current
		utemp32 = message->pui8MsgData[0] << 24 | message->pui8MsgData[1] << 16 | message->pui8MsgData[2] << 8 | message->pui8MsgData[3];
		// Filter Out Error Messages
		if (utemp32 != 0xFFFFFFFF)
		{
			utemp32 ^= 0x80000000;
			float temp_current = (int32_t)utemp32 / 1000.0f;

			if (temp_current > -2000.0f && temp_current < 2000.0f)
			{
				current = temp_current;
			}
		}
		// Read Error Indication
		error = message->pui8MsgData[4] & 0x01;
		processed = true;
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void CAB300::reset()
{
	current = 0.0f;
	error = 0;
}

