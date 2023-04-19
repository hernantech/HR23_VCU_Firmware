/*
******************************************************************************
	delphi_u2c.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "delphi_u2c.h"

DelphiU2C::DelphiU2C(CANInterface* can_interface)
{
	can = can_interface;
}

void DelphiU2C::disable()
{
	output_enable = false;
}

void DelphiU2C::enable()
{
	output_enable = true;
}

unsigned int DelphiU2C::getContactorCounter()
{
	return contactor_counter;
}

float DelphiU2C::getInputCurrent()
{
	return input_current;
}

float DelphiU2C::getInputVoltage()
{
	return input_voltage;
}

float DelphiU2C::getOutputCurrent()
{
	return output_current;
}

float DelphiU2C::getOutputVoltage()
{
	return output_voltage;
}

void DelphiU2C::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// Status Message (0x1D5)
	can_rx_message.ui32MsgID = 0x1D5;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);

	// Status Message (0x48A)
	can_rx_message.ui32MsgID = 0x48A;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool DelphiU2C::isFaultActive()
{
	return master_fault;
}

bool DelphiU2C::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	bool processed = false;

	switch (message->ui32MsgID)
	{
	case 0x1D5:
		master_fault 	= (message->pui8MsgData[0] & 0x80) >> 7;
		input_current 	= message->pui8MsgData[1] * 0.1f;
		output_voltage 	= message->pui8MsgData[3] * 0.078431f;
		output_current 	= (message->pui8MsgData[6] & 0x7F) * 2.0f;
		input_voltage 	= message->pui8MsgData[7] * 1.956f;
		processed = true;
		break;

	case 0x48A:
		input_low_voltage_fault = (message->pui8MsgData[0] & 0x80) >> 7;
		power_loss				= message->pui8MsgData[2] * 2.0f;
		electronics_temp		= message->pui8MsgData[4] - 40.0f;
		coolant_temp			= message->pui8MsgData[5] - 40.0f;
		processed = true;
		break;
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void DelphiU2C::reset()
{
	output_enable			= false;
	master_fault 			= false;
	input_low_voltage_fault = false;

	tick_counter			= 0;
	contactor_counter		= 0;

	input_voltage 			= 0.0f;
	input_current 			= 0.0f;
	output_voltage 			= 0.0f;
	output_current			= 0.0f;
	electronics_temp		= 0.0f;
	coolant_temp			= 0.0f;
	power_loss				= 0.0f;
	output_voltage_setting 	= 0.0f;
}

void DelphiU2C::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	// Send DC-DC Command Message (0x1D7)
	msg.setID(0x1D7);
	msg.setLength(8);

	msg.getData()[0] = output_enable << 7;
	msg.getData()[1] = (unsigned char)(output_voltage_setting * 45.3329f - 473.161f + 0.5f);
	msg.getData()[2] = 0;
	msg.getData()[3] = 0;
	msg.getData()[4] = 0;
	msg.getData()[5] = 0;
	msg.getData()[6] = 0;
	msg.getData()[7] = 0;

	// Send CAN Message
	can->sendMessage(&msg);
}

void DelphiU2C::setOutputVoltage(float voltage)
{
	if (voltage > 11.0f && voltage < 15.5f)
	{
		output_voltage_setting = voltage;
	}
}

void DelphiU2C::resetContactorCounter()
{
	contactor_counter = 0;
}

void DelphiU2C::tick()
{
	if (isPowered() && tick_counter % 2 == 0)
	{
		// Send Command Message Every 20ms
		sendCommandMessage();
	}

	// Base Class Processing
	CANReceiver::tick();

	// Increment Counters
	tick_counter++;
	contactor_counter++;
}
