/*
******************************************************************************
	bel_dc_dc_converter.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "bel_dcdc_converter.h"

BelDCDCConverter::BelDCDCConverter(CANInterface* can_interface, unsigned int device_can_address)
{
	can = can_interface;
	can_address = device_can_address;
}

void BelDCDCConverter::disable()
{
	output_enable_command = false;

	input_voltage = 0.0f;
}

void BelDCDCConverter::enable()
{
	output_enable_command = true;

	// Near Minimum Current Limit
	output_current_setting = 210.0f;
}

unsigned int BelDCDCConverter::getContactorCounter()
{
	return contactor_counter;
}

float BelDCDCConverter::getInputVoltage()
{
	return input_voltage;
}

bool BelDCDCConverter::getStatusFlag(StatusFlag flag)
{
	return status_flags[flag];
}

void BelDCDCConverter::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// Converter Input/Output Value
	can_rx_message.ui32MsgID = 0x18FFD600 + can_address;
	//can_rx_message.ui32MsgID = 0x18FFD6B3;
	can_rx_message.ui32MsgIDMask = 0x1FFFFFFF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_EXT_FILTER | MSG_OBJ_EXTENDED_ID;
	can->initializeReceiveMessage(this, &can_rx_message);

	// Converter Status Flags
	can_rx_message.ui32MsgID = 0x18FFD400 + can_address;
	//can_rx_message.ui32MsgID = 0x18FFD4B3;
	can_rx_message.ui32MsgIDMask = 0x1FFFFFFF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_EXT_FILTER | MSG_OBJ_EXTENDED_ID;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool BelDCDCConverter::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;
	int16_t stemp;

	// True if Message has been Processed
	bool processed = false;

	switch (message->ui32MsgID - can_address)
	{
	case 0x18FFD600:
		// Converter Input/Output Values
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		cooling_plate_temperature = stemp * 0.1f;
		utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		input_voltage = utemp * 0.05f;
		utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		output_current = utemp * 0.05f;
		utemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		output_voltage = utemp * 0.05f;
		processed = true;
		break;

	case 0x18FFD400:
		status_flags[UNIT_ENABLED]  	= message->pui8MsgData[7] & 0x01;
		status_flags[WAKEUP]			= (message->pui8MsgData[7] & 0x02) >> 1;
		status_flags[FAULT]				= (message->pui8MsgData[7] & 0x04) >> 2;
		status_flags[LATCHED_FAULT]		= (message->pui8MsgData[7] & 0x08) >> 3;
		status_flags[REMOTE_SHUTDOWN]	= (message->pui8MsgData[7] & 0x10) >> 4;
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

void BelDCDCConverter::reset()
{
	// Reset All Data
	send_message_flag = false;
	output_enable_command = false;
	output_enabled = false;
	output_voltage_setting = 0.0f;
	output_current_setting = 0.0f;
	output_voltage = 0.0f;
	output_current = 0.0f;
	input_voltage = 0.0f;
	cooling_plate_temperature = 0.0f;
	contactor_counter = 0;

	// Clear All Status Flags
	for (int i = 0; i < NUM_STATUS_FLAGS; i++)
	{
		status_flags[i] = false;
	}
}

void BelDCDCConverter::resetContactorCounter()
{
	contactor_counter = 0;
}

void BelDCDCConverter::setOutputVoltage(float voltage)
{
	output_voltage_setting = voltage;

	// Set Send Message Flag
	send_message_flag = true;
}

void BelDCDCConverter::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_EXTENDED, CANMessage::DATA_FRAME, 8);

	uint16_t utemp;

	// Send DC-DC Converter Command Message (ID 0x18EF0000 + (can_address << 8))
	msg.setIDSize(CANMessage::ID_EXTENDED);
	msg.setID(0x18EF0000 + (can_address << 8));
	msg.setLength(8);

	// Remote Shutdown
	msg.getData()[7] = 0xFF;
	msg.getData()[6] = 0xFF;
	// Output Enabled
	msg.getData()[5] = output_enable_command;
	// Output Current
	utemp = output_current_setting / 0.05f;
	msg.getData()[4] = utemp >> 8;
	msg.getData()[3] = utemp;
	// Output Voltage
	utemp = output_voltage_setting / 0.05f;
	msg.getData()[2] = utemp >> 8;
	msg.getData()[1] = utemp;
	// Data Page Byte
	msg.getData()[0] = 0;

	// Send CAN Message
	can->sendMessage(&msg);
}

void BelDCDCConverter::tick()
{
	if (isPowered() && (send_message_flag || tick_counter % 500 == 0))
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

