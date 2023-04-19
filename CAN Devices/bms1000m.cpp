/*
******************************************************************************
	bms1000m.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "bms1000m.h"

BMS1000M::BMS1000M(CANInterface* can_interface, unsigned int can_id_offset)
{
	// Reset Class
	reset();

	// Store CAN Interface Pointer
	can 		= can_interface;
	can_offset 	= can_id_offset;
}

float BMS1000M::getMaxCellTemp()
{
	return max_cell_temperature;
}

float BMS1000M::getMaxCellVoltage()
{
	return max_cell_voltage;
}

float BMS1000M::getMinCellTemp()
{
	return min_cell_temperature;
}

float BMS1000M::getMinCellVoltage()
{
	return min_cell_voltage;
}

float BMS1000M::getPackCurrent()
{
	return pack_current;
}

float BMS1000M::getPackVoltage()
{
	return pack_voltage;
}

float BMS1000M::getPrechargeVoltage()
{
	return precharge_voltage;
}

float BMS1000M::getSOC()
{
	return soc;
}

BMS1000M::BMSState BMS1000M::getState()
{
	return bms_state;
}

bool BMS1000M::getStatusFlag(BMSStatusFlag status)
{
	if (status == MONITOR_COMMUNICATION_GOOD)
	{
		// Check for Faults
		bool error = false;

		for (int i = 0; i < NUM_MONITOR_ERRORS; i++)
		{
			if (monitor_errors[i])
			{
				error = true;
			}
		}

		status_flags[MONITOR_COMMUNICATION_GOOD] = !error;
	}

	return status_flags[status];
}

float BMS1000M::getSumOfCellsVoltage()
{
	return sum_of_cells_voltage;
}

void BMS1000M::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All Inverter Feedback Messages
	can_rx_message.ui32MsgID = can_offset;
	can_rx_message.ui32MsgIDMask = 0x1FFFFF00;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool BMS1000M::isFaultActive()
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

bool BMS1000M::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t utemp;
	int16_t stemp;
	uint32_t utemp32;
	int32_t stemp32;

	// True if Message has been Processed
	bool processed = false;

	switch (message->ui32MsgID - can_offset)
	{
	case 0x002:
		// BMS Master Faults
		contactor_open_imminent 			= message->pui8MsgData[2];
		// Fault Flags 2
		faults[CELL_VOLTAGE_HIGH] 			= message->pui8MsgData[1] & 0x01;
		faults[CELL_VOLTAGE_LOW]			= (message->pui8MsgData[1] & 0x02) >> 1;
		faults[HIGH_TEMPERATURE]			= (message->pui8MsgData[1] & 0x04) >> 2;
		faults[LOW_TEMPERATURE]				= (message->pui8MsgData[1] & 0x08) >> 3;
		faults[PACK_VOLTAGE_HIGH]			= (message->pui8MsgData[1] & 0x10) >> 4;
		faults[PACK_VOLTAGE_LOW]			= (message->pui8MsgData[1] & 0x20) >> 5;
		faults[HIGH_PACK_DISCHARGE_CURRENT]	= (message->pui8MsgData[1] & 0x40) >> 6;
		faults[HIGH_PACK_CHARGE_CURRENT]	= (message->pui8MsgData[1] & 0x80) >> 7;
		// Fault Flags 1
		faults[ISOLATION_LEAKAGE_FAULT] 	= message->pui8MsgData[0] & 0x01;
		faults[BMS_COMMUNICATION_FAULT]		= (message->pui8MsgData[0] & 0x02) >> 1;
		faults[PRECHARGE_FAULT]				= (message->pui8MsgData[0] & 0x04) >> 2;
		faults[DISCHARGE_ENVELOPE_FAULT]	= (message->pui8MsgData[0] & 0x08) >> 3;
		faults[CHARGE_ENVELOPE_FAULT]		= (message->pui8MsgData[0] & 0x10) >> 4;

		// Ignore Isolation Leakage Fault and Operating Envelope Faults
		if ((message->pui8MsgData[1] & 0xFF) == 0 && (message->pui8MsgData[0] & 0x02) == 0)
		{
			fault_active = false;
		}
		else
		{
			fault_active = true;
		}
		processed = true;
		break;

	case 0x010:
		// BMS Master System Status
		if (message->pui8MsgData[6] < NUM_BMS_STATES)
		{
			bms_state = (BMSState)message->pui8MsgData[6];
		}
		else
		{
			// Bad State Received
			error("Bad BMS State Received", "bms1000m.cpp", __LINE__);
		}
		num_monitor_boards 					= message->pui8MsgData[5];
		// Monitor Errors
		monitor_errors[MONITOR_INCORRECT_NUMBER]			= message->pui8MsgData[4] & 0x01;
		monitor_errors[MONITOR_INCOMPATIBLE_FIRMWARE]		= (message->pui8MsgData[4] & 0x02) >> 1;
		monitor_errors[MONITOR_LOSS_OF_ALL_COMMUNICATION]	= (message->pui8MsgData[4] & 0x04) >> 2;
		monitor_errors[MONITOR_ADDRESS_CONFLICT]			= (message->pui8MsgData[4] & 0x08) >> 3;
		monitor_errors[MONITOR_UNEXPECTED_ADDRESS]			= (message->pui8MsgData[4] & 0x10) >> 4;
		// Status Flags 2
		status_flags[PRECHARGE_RELAY]		= message->pui8MsgData[3] & 0x01;
		status_flags[MAIN_CONTACTOR_POS]	= (message->pui8MsgData[3] & 0x02) >> 1;
		status_flags[MAIN_CONTACTOR_NEG]	= (message->pui8MsgData[3] & 0x04) >> 2;
		status_flags[HVIL_PRESENT]			= (message->pui8MsgData[3] & 0x08) >> 3;
		// Status Flags 1
		status_flags[BMS_MONITOR_POWER]		= message->pui8MsgData[2] & 0x01;
		status_flags[CELL_BALANCING]		= (message->pui8MsgData[2] & 0x02) >> 1;
		processed = true;
		break;

	case 0x011:
		// Current Pack Operating Envelope
		utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
		max_discharge_current = utemp / 10.0f;
		utemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		max_charge_current = utemp / 10.0f;
		processed = true;
		break;

	case 0x020:
		// Pack Level Measurements 1
		utemp32 = message->pui8MsgData[7] << 24 | message->pui8MsgData[6] << 16 | message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		pack_voltage = utemp32 / 1000.0f;
		stemp32 = message->pui8MsgData[3] << 24 | message->pui8MsgData[2] << 16 | message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		pack_current = stemp32 / 1000.0f;
		processed = true;
		break;

	case 0x021:
		// Pack Level Measurements 2
		utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		soc = utemp / 10.0f;
		// Lowest Cell Voltage
		utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		soh = utemp / 10.0f;
		processed = true;
		break;

	case 0x022:
		// Highest Cell Voltage
		utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		max_cell_voltage = utemp / 1000.0f;
		// Lowest Cell Voltage
		utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		min_cell_voltage = utemp / 1000.0f;
		processed = true;
		break;

	case 0x023:
		// Highest Cell Temperature
		stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
		max_cell_temperature = stemp / 10.0f;
		// Lowest Cell Temperature
		stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		min_cell_temperature = stemp / 10.0f;
		processed = true;

	case 0x024:
		// Sum of Cell Voltage
		utemp32 = message->pui8MsgData[7] << 24 | message->pui8MsgData[6] << 16 | message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
		sum_of_cells_voltage = utemp32 / 1000.0f;
		// Precharge Voltage
		utemp32 = message->pui8MsgData[3] << 24 | message->pui8MsgData[2] << 16 | message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
		precharge_voltage = utemp32 / 1000.0f;
		processed = true;

	case 0x0F1:
		// Firmware Version
		version_major = message->pui8MsgData[2];
		version_minor = message->pui8MsgData[1];
		version_revision = message->pui8MsgData[0];
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

void BMS1000M::requestStateChange(BMSCommand command)
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 2);

	// Send BMS State Command Message (ID Offset + 0x000)
	msg.setID(can_offset + 0x000);
	msg.setLength(2);

	// Reserved Byte
	msg.getData()[1] = 0x00;
	// BMS State Command
	msg.getData()[0] = command;

	// Send CAN Message
	can->sendMessage(&msg);
}

void BMS1000M::requestFirmwareVersion()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 0);

	// Send Request Firmware Version Command Message (ID Offset + 0x0F0)
	msg.setID(can_offset + 0x0F0);
	msg.setLength(0);

	// Send CAN Message
	can->sendMessage(&msg);
}

void BMS1000M::reset()
{
	// Reset All Data
	fault_active = false;

	// Fault Flags
	for (int i = 0; i < NUM_BMS_FAULTS; i++)
	{
		faults[i] = false;
	}

	// Monitor Errors
	for (int i = 0; i < NUM_MONITOR_ERRORS; i++)
	{
		monitor_errors[i] = false;
	}

	// BMS Status Flags
	for (int i = 0; i < NUM_STATUS_FLAGS; i++)
	{
		status_flags[i] = false;
	}

	contactor_open_imminent = false;
	bms_state 				= SLEEP;
	num_monitor_boards 		= 0;

	max_discharge_current 	= 0.0f;
	max_charge_current 		= 0.0f;

	pack_current 			= 0.0f;
	soc 					= 0.0f;
	soh 					= 0.0f;

	powered					= false;

	// Reset Pack Voltage to Non-Zero Value to Protect Precharge Logic
	pack_voltage			= 450.0f;

	version_major			= 0;
	version_minor			= 0;
	version_revision		= 0;
}

void BMS1000M::tick()
{
	// Periodically Poll for Version Number
	if (isPowered() && tick_counter % 100000 == 50)
	{
		//requestFirmwareVersion();
	}

	// Base Class Regular Processing
	CANReceiver::tick();

	// Increment Counters
	tick_counter++;
}

