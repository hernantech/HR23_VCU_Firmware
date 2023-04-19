/*
******************************************************************************
	eltek_charger.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "eltek_charger.h"

EltekCharger::EltekCharger(CANInterface* can_interface, unsigned int address)
{
	can = can_interface;

	if (address >= 1 && address <= 16)
	{
		can_address = address - 1;
	}
}

void EltekCharger::disable()
{
	charger_enable = false;
}

void EltekCharger::enable()
{
	charger_enable = true;
}

void EltekCharger::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Charger Feedback Messages
	can_rx_message.ui32MsgID = 0x300;
	can_rx_message.ui32MsgIDMask = 0x700;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool EltekCharger::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;
	//int16_t stemp;

	// True if Message has been Processed
	bool processed = false;

	// Check for Address Match
	if ((message->ui32MsgID & 0x0F0) == (can_address << 4))
	{
		// Parse Message Type
		switch (message->ui32MsgID & 0xF0F)
		{
		case 0x305:
			// Status 1
			mains_frequency = message->pui8MsgData[7];
			utemp = message->pui8MsgData[6] << 8 | message->pui8MsgData[5];
			dc_voltage = utemp / 10.0f;
			utemp = message->pui8MsgData[4] << 8 | message->pui8MsgData[3];
			dc_current = utemp / 10.0f;
			utemp = message->pui8MsgData[2] << 8 | message->pui8MsgData[1];
			mains_current = utemp / 10.0f;
			if (message->pui8MsgData[0] >= 1 && message->pui8MsgData[0] <= 4)
			{
				charger_state = (ChargerState)message->pui8MsgData[0];
			}
			processed = true;
			break;

		case 0x306:
			// Status 2
			charger_available_power = message->pui8MsgData[6] / 2;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			max_power = utemp;
			utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			mains_voltage = utemp;
			secondary_temp = (int8_t)message->pui8MsgData[1];
			primary_temp = (int8_t)message->pui8MsgData[0];
			processed = true;
			break;

		case 0x308:
			// Serial Number
			processed = true;
			break;
		}
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void EltekCharger::sendBroadcastCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	uint16_t utemp;

	// Send Broadcast Command Message
	msg.setID(0x2FF);
	msg.setLength(8);

	// Reserved Byte
	msg.getData()[7] = 0xFF;
	// DC Current Limit
	utemp = dc_current_limit * 10.0f;
	msg.getData()[6] = utemp >> 8;
	msg.getData()[5] = utemp;
	// DC Voltage Limit
	utemp = dc_voltage_limit * 10.0f;
	msg.getData()[4] = utemp >> 8;
	msg.getData()[3] = utemp;
	// Charger Power Reference
	utemp = power_reference * 10.0f;
	msg.getData()[2] = utemp >> 8;
	msg.getData()[1] = utemp;
	// Charger Enable
	msg.getData()[0] = charger_enable;

	// Send CAN Message
	can->sendMessage(&msg);
}

void EltekCharger::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	uint16_t utemp;

	// Send Charger Specific Command Message
	msg.setID(0x300 + (can_address << 4));
	msg.setLength(8);

	// Reserved Byte
	msg.getData()[7] = 0xFF;
	// DC Current Limit
	utemp = dc_current_limit * 10.0f;
	msg.getData()[6] = utemp >> 8;
	msg.getData()[5] = utemp;
	// DC Voltage Limit
	utemp = dc_voltage_limit * 10.0f;
	msg.getData()[4] = utemp >> 8;
	msg.getData()[3] = utemp;
	// Charger Power Reference
	utemp = power_reference * 10.0f;
	msg.getData()[2] = utemp >> 8;
	msg.getData()[1] = utemp;
	// Charger Enable
	msg.getData()[0] = charger_enable;

	// Send CAN Message
	can->sendMessage(&msg);
}

void EltekCharger::setOutputLimits(float voltage, float current, float power)
{
	dc_voltage_limit 	= voltage;
	dc_current_limit 	= current;
	power_reference 	= power;
}

void EltekCharger::tick()
{
	// Send Command Message
	if (isPowered())
	{
		if (tick_counter % 50 == 0)
		{
			sendBroadcastCommandMessage();
		}

		off_time = 0;
	}
	else
	{
		off_time++;
	}

	// Base Class Processing
	CANReceiver::tick();

	// Increment Counters
	tick_counter++;
}


