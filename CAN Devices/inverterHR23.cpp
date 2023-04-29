/*
******************************************************************************
	Rinehart Motion Systems PM100 Inverter Class Implementation
	Erik Stafl
	10/23/2013

	Written for All Processors
	Language: Embedded C++

	Copyright (c) 2013 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "rms_inverter.h"

RMSInverter::RMSInverter(CANInterface* can_interface, unsigned int can_id_offset)
{
	reset();

	// Save Pointer to CAN Interface
	can 		= can_interface;
	can_offset 	= can_id_offset;
}

void RMSInverter::disable()
{
	enabled = false;
	inverter_enabled = false;
	torque_command = 0.0f;

	// Send Command Message
	sendCommandMessage();
}

void RMSInverter::enable()
{
	enabled = true;
	vsm_state = VSM_STATE_1;
	inverter_lockout_clear = false;
}

float RMSInverter::getDCBusCurrent()
{
	return dc_bus_current;
}

float RMSInverter::getDCBusVoltage()
{
	return dc_bus_voltage;
}

float RMSInverter::getDeltaResolver()
{
	return delta_resolver;
}

RMSInverter::Direction RMSInverter::getDirection()
{
	return direction;
}

float RMSInverter::getMaxTemperature()
{
	float max_temp = module_a_temperature;

	// Find Highest Internal Temperature
	if (module_b_temperature > max_temp)
	{
		max_temp = module_b_temperature;
	}

	if (module_c_temperature > max_temp)
	{
		max_temp = module_c_temperature;
	}

	if (gate_drive_temperature > max_temp)
	{
		max_temp = gate_drive_temperature;
	}

	return max_temp;
}

float RMSInverter::getMotorSpeed()
{
	return motor_speed;
}

float RMSInverter::getMotorTemperature()
{
	return motor_temperature;
}

void RMSInverter::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Inverter Feedback Messages
	can_rx_message.ui32MsgID = can_offset;
	can_rx_message.ui32MsgIDMask = 0x1FFFFFF0;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool RMSInverter::isFaultActive()
{
	if (powered)
	{
		return fault_active;
	}
	else
	{
		return false;
	}
}

bool RMSInverter::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;
	int16_t stemp;

	// True if Message has been Processed
	bool processed = false;

	switch (message->ui32MsgID - can_offset)
	{
	case 0x00:
		// Temperatures 1
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		gate_drive_temperature = stemp / 10.0f;
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		module_c_temperature = stemp / 10.0f;
		stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		module_b_temperature = stemp / 10.0f;
		stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		module_a_temperature = stemp / 10.0f;
		processed = true;
		break;

	case 0x01:
		// Temperatures 2
		stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		control_board_temperature = stemp / 10.0f;
		processed = true;
		break;

	case 0x02:
		// Temperatures 3 and Torque Shudder
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		motor_temperature = stemp / 10.0f;
		processed = true;
		break;

	case 0x05:
		// Motor Position Information
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		delta_resolver = stemp / 10.0f;
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		electrical_output_f = stemp / 10.0f;
		stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		motor_speed = stemp;
		stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		motor_angle = stemp / 10.0f;
		processed = true;
		break;

	case 0x06:
		// Current Information
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		dc_bus_current = stemp / 10.0f;
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		phase_c_current = stemp / 10.0f;
		stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		phase_b_current = stemp / 10.0f;
		stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		phase_a_current = stemp / 10.0f;
		processed = true;
		break;

	case 0x07:
		// Voltage Information
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		bc_voltage = stemp / 10.0f;
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		ab_voltage = stemp / 10.0f;
		stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		output_voltage = stemp / 10.0f;
		stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		dc_bus_voltage = stemp / 10.0f;
		processed = true;
		break;

	case 0x0A:
		// Internal States
		inverter_lockout = message->pui8MsgData[6] & 0x80 >> 7;
		utemp = message->pui8MsgData[3];
		precharge_relay = utemp & 0x01;
		main_contactor = utemp & 0x02 >> 1;
		utemp = message->pui8MsgData[0];
		if (utemp <= 7)
		{
			vsm_state = (VSMState)utemp;
		}
		processed = true;
		break;

	case 0x0B:
		// Fault Codes
		post_faults_low 	= message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		post_faults_high 	= message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		run_faults_low 		= message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		run_faults_high 	= message->pui8MsgData[7] << 8 | message->pui8MsgData[6];

		if (run_faults_low == 0 && run_faults_high == 0)
		{
			fault_active = false;
		}
		else
		{
			fault_active = true;
		}
		processed = true;
		break;

	case 0x0F:
		// Diagnostic Data
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

void RMSInverter::reset()
{
	enabled				= false;
	direction_counter	= 0;
	torque_command 		= 0.0f;
	direction			= NEUTRAL;
	requested_direction = NEUTRAL;
	inverter_enabled 	= false;
	fault_active		= false;
}

void RMSInverter::sendCommandMessage()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	// Send Inverter Command Message (ID Offset + 0x020)
	msg.setID(can_offset + 0x020);
	msg.setLength(8);

	enter_critical_section();

	short torque = torque_command * 10.0f;
	msg.getData()[7] = 0;
	msg.getData()[6] = 0;
	msg.getData()[6] = 0;
	msg.getData()[5] = inverter_enabled;
	if (direction == FORWARD)
	{
		msg.getData()[4] = 1;
	}
	else
	{
		msg.getData()[4] = 0;
	}
	msg.getData()[3] = 0;
	msg.getData()[2] = 0;
	msg.getData()[1] = torque >> 8;
	msg.getData()[0] = torque;

	// Send CAN Message
	can->sendMessage(&msg);

	exit_critical_section();
}

void RMSInverter::setDirection(Direction dir)
{
	// Minimum Time in Each Direction is 10 ticks
	if (direction_counter < 10 || direction == dir)
		return;

	// Move Through Neutral Before Switching Directions
	if ((dir == FORWARD && direction == REVERSE) || dir == REVERSE && direction == FORWARD)
	{
		direction = NEUTRAL;
	}
	else
	{
		direction = dir;
	}

	requested_direction = dir;
	direction_counter = 0;

	if (direction == NEUTRAL)
	{
		torque_command = 0.0f;
		inverter_enabled = false;
	}
	else if (direction == FORWARD || direction == REVERSE)
	{
		inverter_enabled = true;
	}
}

void RMSInverter::setTorqueCommand(float tq_cmd)
{
	torque_command = tq_cmd;
}

void RMSInverter::tick()
{
	if (enabled)
	{
		// Clear Inverter Lockout
		if (!inverter_lockout_clear && vsm_state == VSM_STATE_4)
		{
			direction = FORWARD;
			inverter_enabled = false;
			sendCommandMessage();

			// Inverter can now be Enabled
			direction = NEUTRAL;
			inverter_lockout_clear = true;
		}

		// Manage Delayed Direction Changes
		if (direction != requested_direction && direction_counter >= 10)
			setDirection(requested_direction);

		if (tick_counter % 2 == 0)
		{
			sendCommandMessage();
		}
	}

	// Increment Counters
	incrementCounters();
	direction_counter++;
	tick_counter++;
}

