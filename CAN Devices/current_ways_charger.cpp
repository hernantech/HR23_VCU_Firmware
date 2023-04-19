/*
******************************************************************************
	current_ways_charger.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "current_ways_charger.h"

CurrentWaysCharger::CurrentWaysCharger(CANInterface* can_interface)
{
	can = can_interface;
}

void CurrentWaysCharger::disable()
{
	if (output_enable_command)
	{
		output_enable_command 	= false;
		send_message_flag 		= true;
	}
}

void CurrentWaysCharger::enable()
{
	if (!output_enable_command)
	{
		output_enable_command 	= true;
		send_message_flag 		= true;
	}
}

float CurrentWaysCharger::getACVoltage()
{
	return ac_input_voltage;
}

unsigned int CurrentWaysCharger::getContactorCounter()
{
	return contactor_counter;
}

float CurrentWaysCharger::getDCVoltage()
{
	return dc_output_voltage;
}

float CurrentWaysCharger::getMaxJ1772Current()
{
	return j1772_max_current;
}

void CurrentWaysCharger::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// Charger Status Messages
	can_rx_message.ui32MsgID = 0x271;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool CurrentWaysCharger::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;

	// True if Message has been Processed
	bool processed = false;

	// Only Process Messages from Master Charger
	if (message->pui8MsgData[1] == 0)
	{
		// Status Messages are Paged by First Data Byte
		switch (message->pui8MsgData[0])
		{
		case 0xF0:
			// MSG Status Temp
			processed = true;
			break;

		case 0xF1:
			// Message Status Output
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			dc_output_power = utemp;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			dc_output_current = utemp / 10.0f;
			utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			dc_output_voltage = utemp;
			processed = true;
			break;

		case 0xF2:
			// Message Status AC
			utemp = message->pui8MsgData[2];
			if (utemp != 0xFF)
			{
				j1772_max_current = utemp;
			}
			else
			{
				j1772_max_current = 30.0f;
			}

			utemp = message->pui8MsgData[3];
			j1772_proximity_voltage = utemp * 5.0f / 255.0f;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			if (utemp > 275)
			{
				// Default to Low Voltage
				ac_input_voltage = 120.0f;
			}
			else
			{
				ac_input_voltage = utemp;
			}
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			ac_input_current = utemp / 10.0f;
			processed = true;
			break;

		default:
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

void CurrentWaysCharger::reset()
{
	// Reset All Data
	send_message_flag = false;
	output_enable_command = false;
	output_enabled = false;
	output_voltage_setting = false;
	output_current_setting = false;

	dc_output_voltage 	= 0.0f;
	dc_output_current 	= 0.0f;
	dc_output_power 	= 0.0f;
	ac_input_voltage 	= 0.0f;

	contactor_counter = 0;
}

void CurrentWaysCharger::resetContactorCounter()
{
	contactor_counter = 0;
}

void CurrentWaysCharger::setOutputCurrent(float current)
{
	output_current_setting = current;

	send_message_flag = true;
}

void CurrentWaysCharger::setOutputVoltage(float voltage)
{
	output_voltage_setting = voltage;

	send_message_flag = true;
}

void CurrentWaysCharger::sendCommandMessage()
{
	uint16_t utemp;

	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 7);

	// Send Charger Command Message
	msg.setID(0x618);
	msg.setLength(7);

	// MSB First
	utemp = output_current_setting * 10.0f;
	msg.getData()[6] = utemp;
	msg.getData()[5] = utemp >> 8;
	utemp = output_voltage_setting * 10.0f;
	msg.getData()[4] = utemp;
	msg.getData()[3] = utemp >> 8;
	msg.getData()[2] = 0;
	msg.getData()[1] = 0;
	msg.getData()[0] = output_enable_command << 7;

	// Send CAN Message
	can->sendMessage(&msg);
}

void CurrentWaysCharger::sendStartupMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 2);

	// Send Charger Command Message
	msg.setID(0x270);
	msg.setLength(2);

	msg.getData()[1] = 0xFF;
	msg.getData()[0] = 0xF8;

	// Send CAN Message
	can->sendMessage(&msg);
}

void CurrentWaysCharger::tick()
{
	if (isPowered() && (send_message_flag || tick_counter % 150 == 0))
	{
		// Send Command Message
		sendCommandMessage();

		// Clear Message Flag
		send_message_flag = false;
	}

	// Base Class Regular Processing
	CANReceiver::tick();

	// Increment Counters
	contactor_counter++;
	tick_counter++;
}
