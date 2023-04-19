/*
 * rms_fast_charger.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: Erik
 */

#include "rms_fast_charger.h"

RMSFastCharger::RMSFastCharger(CANInterface* can_interface, unsigned int inverter_offset, unsigned int dcdc_offset)
{
	reset();

	// Save Pointer to CAN Interface
	can = can_interface;

	// CAN Offsets
	inverter_can_offset = inverter_offset;
	dcdc_can_offset		= dcdc_offset;
}

void RMSFastCharger::disable()
{
	converter_enable = false;
}

void RMSFastCharger::enable()
{
	converter_enable = true;
}

float RMSFastCharger::getDCVoltage()
{
	return lv_voltage;
}

void RMSFastCharger::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// Line Inverter Feedback Messages
	can_rx_message.ui32MsgID = inverter_can_offset;
	can_rx_message.ui32MsgIDMask = 0x7F0;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);

	// DC-DC Converter Feedback Messages
	can_rx_message.ui32MsgID = dcdc_can_offset;
	can_rx_message.ui32MsgIDMask = 0x7F0;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool RMSFastCharger::isCommunicationGood()
{
	if (message_received && last_message_dcdc_counter < 200 && last_message_line_inverter_counter < 200)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool RMSFastCharger::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	//uint16_t utemp;
	int16_t stemp;

	// True if Message has been Processed
	bool processed = false;

	// Determine if Message is from Line Inverter or DC-DC Converter
	if ((message->ui32MsgID & 0x7F0) == inverter_can_offset)
	{
		// Line Inverter Message
		switch (message->ui32MsgID - inverter_can_offset)
		{
		case 0x7:
			// Voltage Info
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			inverter_input_frequency = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			inverter_vac_rms = stemp / 10.0f;
			processed = true;
			break;

		case 0xB:
			// Fault Info
			line_inverter_post_faults_low	= message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			line_inverter_post_faults_low	= message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			line_inverter_faults_low 		= message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			line_inverter_faults_high 		= message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			processed = true;
			break;

		default:
			break;
		}

		if (processed)
		{
			// Valid Message Received
			last_message_line_inverter_counter = 0;
		}
	}
	else if ((message->ui32MsgID & 0x7F0) == dcdc_can_offset)
	{
		// DC-DC Converter Message
		switch (message->ui32MsgID - dcdc_can_offset)
		{
		case 0x2:
			// Temperature Set 3
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			dcdc_gdb3_temp = stemp / 10.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			dcdc_inductor_3_temp = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			dcdc_inductor_2_temp = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			dcdc_inductor_1_temp = stemp / 10.0f;
			processed = true;
			break;

		case 0x6:
			// Current Info
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			dc_bus_current = stemp / 10.0f;
			processed = true;
			break;

		case 0x7:
			// Voltage Info
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			reference_voltage = stemp / 10.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			hv_voltage = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			lv_voltage = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			lv_internal_voltage = stemp / 10.0f;
			processed = true;
			break;

		case 0xB:
			// Fault Info
			dcdc_post_faults_low 	= message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			dcdc_post_faults_low 	= message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			dcdc_faults_low 		= message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			dcdc_faults_high 		= message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			processed = true;
			break;

		default:
			break;
		}

		if (processed)
		{
			// Valid Message Received
			last_message_dcdc_counter = 0;
		}
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void RMSFastCharger::reset()
{
	converter_enable 			= false;

	dcdc_post_faults_low		= 0;
	dcdc_post_faults_high		= 0;
	line_inverter_post_faults_low	= 0;
	line_inverter_post_faults_high	= 0;

	dcdc_faults_low 			= 0;
	dcdc_faults_high 			= 0;
	line_inverter_faults_low 	= 0;
	line_inverter_faults_high 	= 0;
}

void RMSFastCharger::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	// Send Fast Charger DC-DC Converter Command Message (ID Offset + 0x020)
	msg.setID(dcdc_can_offset + 0x020);
	msg.setLength(8);

	enter_critical_section();

	// Reserved Bytes
	msg.getData()[7] = 0;
	msg.getData()[6] = 0;
	// Converter Enable Command
	msg.getData()[5] = converter_enable;
	// Mode Command
	msg.getData()[4] = boost_mode;
	// Current Command
	short current = current_command * 10.0f;
	msg.getData()[3] = current >> 8;
	msg.getData()[2] = current;
	// Voltage Command
	short voltage = voltage_command * 10.0f;
	msg.getData()[1] = voltage >> 8;
	msg.getData()[0] = voltage;

	// Send CAN Message
	can->sendMessage(&msg);

	exit_critical_section();
}

void RMSFastCharger::setOutputLimits(float voltage, float current)
{
	voltage_command = voltage;
	current_command = current;
}

void RMSFastCharger::setPowered(bool device_power)
{
	if (!device_power)
	{
		last_message_dcdc_counter 			= 0;
		last_message_line_inverter_counter 	= 0;
	}

	// Base Class Call
	CANReceiver::setPowered(device_power);
}

void RMSFastCharger::tick()
{
	// Send Command Message
	if (isPowered())
	{
		if (tick_counter % 10 == 0)
		{
			// Send Command Message
			sendCommandMessage();
		}
	}

	// Base Class Processing
	CANReceiver::tick();

	// Increment Counters
	tick_counter++;

	if (powered)
	{
		last_message_dcdc_counter++;
		last_message_line_inverter_counter++;
	}
}
