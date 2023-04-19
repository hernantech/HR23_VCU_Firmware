/*
 * bms_monitor.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: Erik
 */

#include "bms_monitor.h"

BMSMonitor::BMSMonitor()
{
	message_received = false;
}

bool BMSMonitor::getBalancingCommand(unsigned int local_index)
{
	if (local_index < 12)
	{
		return cell_shunting[local_index];
	}
	else
	{
		return false;
	}
}

float BMSMonitor::getCellTemperature(unsigned int local_index)
{
	if (local_index < 12)
	{
		return cell_temperatures[local_index];
	}
	else
	{
		return 0.0f;
	}
}

float BMSMonitor::getCellVoltage(unsigned int local_index)
{
	if (local_index < 12)
	{
		return cell_voltages[local_index];
	}
	else
	{
		return 0.0f;
	}
}

bool BMSMonitor::isCellInputConnected(unsigned int local_index)
{
	if (local_index >= 12)
	{
		return false;
	}

	return connected_cells[local_index];
}

bool BMSMonitor::isCommunicationGood()
{
	if (message_received && last_message_counter < 200)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BMSMonitor::isTemperatureInputConnected(unsigned int local_index)
{
	if (local_index >= 12)
	{
		return false;
	}

	return connected_temperatures[local_index];
}

bool BMSMonitor::receiveCANMessage(tCANMsgObject* msg)
{
	// Temporary Reception Variables
	uint16_t utemp;
	int16_t stemp;

	bool processed = false;

	// Determine Message Type
	switch ((msg->ui32MsgID & 0x0000FF00) >> 8)
	{
	case BOARD_STATUS:
		// 12V Sense
		utemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		sense_12v = utemp / 1000.0f;
		// Board Temperature
		stemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		board_temperature = stemp / 10.0f;
		// Status Flags
		status_flags[SHUNTING_ACTIVE] 				= (bool)(msg->pui8MsgData[1] & 0x01);
		status_flags[SHUNTING_PAUSED_FOR_TEMP] 		= (bool)(msg->pui8MsgData[1] & 0x02);
		// Fault Flags
		fault_flags[LTC6804_COMMUNICATION_FAILURE] 	= (bool)(msg->pui8MsgData[0] & 0x01);
		fault_flags[BOARD_OVERTEMP_FAULT]			= (bool)(msg->pui8MsgData[0] & 0x02);
		fault_flags[ADDRESS_CONFLICT]				= (bool)(msg->pui8MsgData[0] & 0x04);
		processed = true;
		break;

	case CELL_1_4_VOLTAGE:
		// Cell 1
		utemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_voltages[0] = utemp / 10000.0f;
		// Cell 2
		utemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_voltages[1] = utemp / 10000.0f;
		// Cell 3
		utemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_voltages[2] = utemp / 10000.0f;
		// Cell 4
		utemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_voltages[3] = utemp / 10000.0f;
		processed = true;
		break;

	case CELL_5_8_VOLTAGE:
		// Cell 5
		utemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_voltages[4] = utemp / 10000.0f;
		// Cell 6
		utemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_voltages[5] = utemp / 10000.0f;
		// Cell 7
		utemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_voltages[6] = utemp / 10000.0f;
		// Cell 8
		utemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_voltages[7] = utemp / 10000.0f;
		processed = true;
		break;

	case CELL_9_12_VOLTAGE:
		// Cell 9
		utemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_voltages[8] = utemp / 10000.0f;
		// Cell 10
		utemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_voltages[9] = utemp / 10000.0f;
		// Cell 11
		utemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_voltages[10] = utemp / 10000.0f;
		// Cell 12
		utemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_voltages[11] = utemp / 10000.0f;
		processed = true;
		break;

	case CELL_1_4_TEMPERATURE:
		// Cell 1
		stemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_temperatures[0] = stemp / 10.0f;
		// Cell 2
		stemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_temperatures[1] = stemp / 10.0f;
		// Cell 3
		stemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_temperatures[2] = stemp / 10.0f;
		// Cell 4
		stemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_temperatures[3] = stemp / 10.0f;
		processed = true;
		break;

	case CELL_5_8_TEMPERATURE:
		// Cell 5
		stemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_temperatures[4] = stemp / 10.0f;
		// Cell 6
		stemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_temperatures[5] = stemp / 10.0f;
		// Cell 7
		stemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_temperatures[6] = stemp / 10.0f;
		// Cell 8
		stemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_temperatures[7] = stemp / 10.0f;
		processed = true;
		break;

	case CELL_9_12_TEMPERATURE:
		// Cell 9
		stemp = msg->pui8MsgData[7] << 8 | msg->pui8MsgData[6];
		cell_temperatures[8] = stemp / 10.0f;
		// Cell 10
		stemp = msg->pui8MsgData[5] << 8 | msg->pui8MsgData[4];
		cell_temperatures[9] = stemp / 10.0f;
		// Cell 11
		stemp = msg->pui8MsgData[3] << 8 | msg->pui8MsgData[2];
		cell_temperatures[10] = stemp / 10.0f;
		// Cell 12
		stemp = msg->pui8MsgData[1] << 8 | msg->pui8MsgData[0];
		cell_temperatures[11] = stemp / 10.0f;
		processed = true;
		break;

	case FIRMWARE_VERSION:
		version_major 		= msg->pui8MsgData[2];
		version_minor 		= msg->pui8MsgData[1];
		version_revision 	= msg->pui8MsgData[0];
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

void BMSMonitor::sendCANMessage(CANMessageType msg_type)
{
	// Check to Verify CAN System is Initialized
	if (can == 0)
	{
		return;
	}

	// CAN Message Object for Transmission
	CANMessage message(CANMessage::ID_EXTENDED, CANMessage::DATA_FRAME, 2);

	switch (msg_type)
	{
	case CELL_SHUNTING_COMMAND:
		// Set ID and Size
		message.setIDSize(CANMessage::ID_EXTENDED);
		message.setID(0x4000 + address);
		message.setLength(2);
		// Upper Byte
		message.getData()[1] = cell_shunting[8];
		message.getData()[1] |= cell_shunting[9] 	<< 1;
		message.getData()[1] |= cell_shunting[10] 	<< 2;
		message.getData()[1] |= cell_shunting[11] 	<< 3;
		// Lower Byte
		message.getData()[0] = cell_shunting[0];
		message.getData()[0] |= cell_shunting[1] 	<< 1;
		message.getData()[0] |= cell_shunting[2] 	<< 2;
		message.getData()[0] |= cell_shunting[3] 	<< 3;
		message.getData()[0] |= cell_shunting[4] 	<< 4;
		message.getData()[0] |= cell_shunting[5] 	<< 5;
		message.getData()[0] |= cell_shunting[6] 	<< 6;
		message.getData()[0] |= cell_shunting[7] 	<< 7;

		// Send Message
		can->sendMessage(&message);
		break;

	default:
		break;
	}
}

void BMSMonitor::setAddress(unsigned int new_address)
{
	// Check for Valid Address
	if (new_address <= 63)
	{
		address = new_address;
	}
}

void BMSMonitor::setBalancingCommand(unsigned int local_index, bool shunting)
{
	if (local_index < 12)
	{
		cell_shunting[local_index] = shunting;
	}
}

void BMSMonitor::setCANInterface(CANInterface* can_interface)
{
	can = can_interface;
}

void BMSMonitor::setConnectedInput(unsigned int index, bool voltage, bool temperature)
{
	if (index < 12)
	{
		connected_cells[index] 			= voltage;
		connected_temperatures[index] 	= temperature;
	}
}

void BMSMonitor::setTemperaturePosition(unsigned int local_index, unsigned int global_index)
{
	if (local_index < 12)
	{
		cell_map[local_index] = global_index;
	}
}

void BMSMonitor::setVoltagePosition(unsigned int local_index, unsigned int global_index)
{
	if (local_index < 12)
	{
		temperature_map[local_index] = global_index;
	}
}

void BMSMonitor::tick()
{
	// Increment Counters without Overflow
	if (last_message_counter < 100000000)
	{
		last_message_counter++;
	}
}
