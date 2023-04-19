/*
******************************************************************************
	active_bms_board.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "active_bms_board.h"

ActiveBMSBoard::ActiveBMSBoard() : message(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 2)
{

}

float ActiveBMSBoard::getCellVoltage(unsigned int position)
{
	return cell_voltages[position];
}

float ActiveBMSBoard::getCellTemperature(unsigned int position)
{
	return cell_temperatures[position];
}

bool ActiveBMSBoard::isCellInputConnected(unsigned int local_index)
{
	if (local_index >= 12)
	{
		return false;
	}

	return connected_cells[local_index];
}

bool ActiveBMSBoard::isCommunicationGood()
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

bool ActiveBMSBoard::isTemperatureInputConnected(unsigned int local_index)
{
	if (local_index >= 12)
	{
		return false;
	}

	return connected_temperatures[local_index];
}

bool ActiveBMSBoard::receiveCANMessage(tCANMsgObject* message)
{
	// Verify Device Address
	if ((message->ui32MsgID & 0x00F) == address)
	{
		uint16_t utemp;
		int16_t stemp;

		bool processed = false;

		// Detemine Message Type
		switch (message->ui32MsgID & 0x0F0)
		{
		case 0x000:
			// Board Status Message
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			sense_12v = utemp / 1000.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			isolated_temp = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			accessory_temp = stemp / 10.0f;
			processed = true;
			break;

		case 0x080:
			// Cell 1-4 Voltage Data
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_voltages[0] = utemp / 10000.0f;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_voltages[1] = utemp / 10000.0f;
			utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_voltages[2] = utemp / 10000.0f;
			utemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_voltages[3] = utemp / 10000.0f;
			processed = true;
			break;

		case 0x090:
			// Cell 5-8 Voltage Data
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_voltages[4] = utemp / 10000.0f;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_voltages[5] = utemp / 10000.0f;
			utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_voltages[6] = utemp / 10000.0f;
			utemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_voltages[7] = utemp / 10000.0f;
			processed = true;
			break;

		case 0x0A0:
			// Cell 9-12 Voltage Data
			utemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_voltages[8] = utemp / 10000.0f;
			utemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_voltages[9] = utemp / 10000.0f;
			utemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_voltages[10] = utemp / 10000.0f;
			utemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_voltages[11] = utemp / 10000.0f;
			processed = true;
			break;

		case 0x0B0:
			// Cell 1-4 Temperature Data
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_temperatures[0] = stemp / 10.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_temperatures[1] = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_temperatures[2] = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_temperatures[3] = stemp / 10.0f;
			processed = true;
			break;

		case 0x0C0:
			// Cell 5-8 Temperature Data
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_temperatures[4] = stemp / 10.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_temperatures[5] = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_temperatures[6] = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_temperatures[7] = stemp / 10.0f;
			processed = true;
			break;

		case 0x0D0:
			// Cell 9-12 Temperature Data
			stemp = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			cell_temperatures[8] = stemp / 10.0f;
			stemp = message->pui8MsgData[5] << 8 | message->pui8MsgData[4];
			cell_temperatures[9] = stemp / 10.0f;
			stemp = message->pui8MsgData[3] << 8 | message->pui8MsgData[2];
			cell_temperatures[10] = stemp / 10.0f;
			stemp = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			cell_temperatures[11] = stemp / 10.0f;
			processed = true;
			break;

		case 0x0F0:
			// BMS Board Firmware Version
			version_major 		= message->pui8MsgData[2];
			version_minor 		= message->pui8MsgData[1];
			version_revision 	= message->pui8MsgData[0];
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
	else
	{
		// Wrong Address
		return false;
	}
}

void ActiveBMSBoard::sendCANMessage()
{
	// Set ID and Size
	message.setIDSize(CANMessage::ID_STANDARD);
	message.setID(0x110 + address);
	message.setLength(6);

	// Active Charging and Discharging Commands
	message.getData()[5] = 0;
	message.getData()[4] = 0;
	message.getData()[3] = 0;
	message.getData()[2] = 0;
	// Cell Shunting Command 9-12
	message.getData()[1] = cell_shunting[8];
	message.getData()[1] |= cell_shunting[9] 	<< 1;
	message.getData()[1] |= cell_shunting[10] 	<< 2;
	message.getData()[1] |= cell_shunting[11] 	<< 3;
	// Cell Shunting Command 1-8
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
}

void ActiveBMSBoard::setAddress(unsigned int board_address)
{
	address = board_address;
}

void ActiveBMSBoard::setBalancingCommand(unsigned int local_index, bool charging, bool discharging, bool shunting)
{
	if (local_index < 12)
	{
		cell_shunting[local_index] = shunting;
	}
}

void ActiveBMSBoard::setCANInterface(CANInterface* can_interface)
{
	can = can_interface;
}

void ActiveBMSBoard::setConnectedInput(unsigned int index, bool voltage, bool temperature)
{
	if (index < 12)
	{
		connected_cells[index] 			= voltage;
		connected_temperatures[index] 	= temperature;
	}
}

void ActiveBMSBoard::setTemperaturePosition(unsigned int local_index, unsigned int global_index)
{
	if (local_index < 12)
	{
		cell_map[local_index] = global_index;
	}
}

void ActiveBMSBoard::setVoltagePosition(unsigned int local_index, unsigned int global_index)
{
	if (local_index < 12)
	{
		temperature_map[local_index] = global_index;
	}
}

void ActiveBMSBoard::tick()
{
	last_message_counter++;
}
