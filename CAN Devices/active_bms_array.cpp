/*
******************************************************************************
	active_bms_array.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "active_bms_array.h"

ActiveBMSArray::ActiveBMSArray(CANInterface* can_interface, unsigned int can_id_offset)
{
	can = can_interface;
	can_offset = can_id_offset;

	// Set Board Addresses
	for (int i = 0; i < EXPECTED_MONITOR_BOARDS; i++)
	{
		bms_boards[i].setAddress(i);
	}
}

void ActiveBMSArray::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Inverter Feedback Messages
	can_rx_message.ui32MsgID = can_offset;
	can_rx_message.ui32MsgIDMask = 0x700;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool ActiveBMSArray::isCommunicationGood()
{
	for (int i = 0; i < EXPECTED_MONITOR_BOARDS; i++)
	{
		if (!bms_boards[i].isCommunicationGood())
		{
			return false;
		}
	}

	return true;
}

bool ActiveBMSArray::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	// Get Address from Message
	unsigned int address = message->ui32MsgID & 0x00F;

	if (address < EXPECTED_MONITOR_BOARDS)
	{
		return bms_boards[address].receiveCANMessage(message);
	}
	else
	{
		return false;
	}
}

void ActiveBMSArray::reset()
{

}

void ActiveBMSArray::tick()
{
	// Perform Regular Processing on All Boards
	for (int i = 0; i < EXPECTED_MONITOR_BOARDS; i++)
	{
		bms_boards[i].tick();
	}

	// Send Balancing Commands
	if (powered_counter % 10 == 0)
	{
		// Boards 0-3
		for (int i = 0; i <= 3; i++)
		{
			bms_boards[i].sendCANMessage();
		}
	}
	else if (powered_counter % 10 == 1)
	{
		// Boards 4-7
		for (int i = 4; i <= 7; i++)
		{
			bms_boards[i].sendCANMessage();
		}
	}

	// Base Class Processing
	CANReceiver::tick();

	// Increment Counters
	tick_counter++;
}
