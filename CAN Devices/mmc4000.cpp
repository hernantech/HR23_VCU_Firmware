/*
******************************************************************************
	mmc4000.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "MMC4000.h"

MMC4000::MMC4000(CANInterface* can_interface)
{
	can = can_interface;
}

void MMC4000::disable()
{
	enabled = false;
}

void MMC4000::enable()
{
	enabled = true;
}

void MMC4000::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Charger Feedback Messages
	can_rx_message.ui32MsgID = 0x7C0;
	can_rx_message.ui32MsgIDMask = 0x7F0;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool MMC4000::isEnabled()
{
	return enabled;
}

bool MMC4000::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;

	// True if Message has been Processed
	bool processed = false;

	// Receive Messages
	switch (message->ui32MsgID)
	{
	case 0x7C2:
		// Variable Data
		measured_temperature 		= (float)message->pui8MsgData[5] - 40.0f;
		measured_power_percent 		= (float)message->pui8MsgData[4];
		utemp						= message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		measured_heater_power		= utemp * 10.0f;
		utemp						= message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		measured_battery_voltage	= utemp;
		processed = true;
		break;

	case 0x7C3:
		// Fixed Data
		processed = true;
		break;

	case 0x7C4:
		// Status Data
		hardware_fault				= (message->pui8MsgData[2] & 0x02) >> 1;
		processed = true;
		break;

	default:
		break;
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void MMC4000::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 3);

	uint16_t utemp;

	// Send Charger Specific Command Message
	msg.setID(0x7C1);
	msg.setLength(3);

	// Battery Current Limit
	if (battery_current_limit > 31)
	{
		utemp = 31;
	}
	else
	{
		utemp = battery_current_limit;
	}
	msg.getData()[2] = utemp;
	// Power Demand
	msg.getData()[1] = power_demand;
	// Heater Enable
	msg.getData()[0] = enabled;

	// Send CAN Message
	can->sendMessage(&msg);
}

void MMC4000::tick()
{
	// Send Command Message
	if (isPowered())
	{
		if (tick_counter % 50 == 0)
		{
			sendCommandMessage();
		}
	}

	// Increment Counters
	incrementCounters();
	tick_counter++;
}

