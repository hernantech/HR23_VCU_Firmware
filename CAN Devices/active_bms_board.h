/*
******************************************************************************
	active_bms_board.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef ACTIVE_BMS_BOARD_H_
#define ACTIVE_BMS_BOARD_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

#include "configuration.h"

class ActiveBMSBoard
{
public:
	// Default Constructor
	ActiveBMSBoard();

	// Get Cell Voltage
	float getCellVoltage(unsigned int position);

	// Get Cell Temperature
	float getCellTemperature(unsigned int position);

	// Get Whether Cell Input is Connected
	bool isCellInputConnected(unsigned int local_index);

	// Return True if Communication is Good
	bool isCommunicationGood();

	// Get Whether Temperature Input is Connected
	bool isTemperatureInputConnected(unsigned int local_index);

	// Process CAN Message
	bool receiveCANMessage(tCANMsgObject* message);

	// Reset Class Data
	void reset();

	// Send CAN Message
	void sendCANMessage();

	// Set BMS Address
	void setAddress(unsigned int board_address);

	// Set Balancing Command for Given Cell Input
	void setBalancingCommand(unsigned int local_index, bool charging, bool discharging, bool shunting);

	// Set Pointer to CAN System
	void setCANInterface(CANInterface* can_interface);

	// Set Whether Input has Cell Voltage and Temperature
	void setConnectedInput(unsigned int local_index, bool voltage, bool temperature);

	// Map Local Temperature Index to Global Temperature Index
	void setTemperaturePosition(unsigned int local_index, unsigned int global_index);

	// Map Local Voltage Index to Global Voltage Index
	void setVoltagePosition(unsigned int local_index, unsigned int global_index);

	// Perform Regular Processing
	void tick();

private:
	// Pointer to CAN System to Transmit Messages
	CANInterface* can;

	// Local CAN Message Object for Transmission
	CANMessage message;

	// Board Address
	unsigned int address;

	// 12V Sense on BMS Board
	float sense_12v;

	// On-board Temperature Sensors
	float isolated_temp;
	float accessory_temp;

	// Cell Data
	float cell_voltages[12];		// V
	float cell_temperatures[12];	// Deg C

	// Current Cell Shunting Command
	bool cell_shunting[12];

	// Map of Connected Inputs - True for Valid Signals
	bool connected_cells[12];
	bool connected_temperatures[12];

	// Global Index of Local Value
	unsigned int cell_map[12];
	unsigned int temperature_map[12];

	// Firmware Version
	unsigned int version_major;
	unsigned int version_minor;
	unsigned int version_revision;

	// True if a CAN Message has been Received from this Device
	bool message_received;

	// Ticks Since Last Message Received
	unsigned int last_message_counter;
};

#endif /* ACTIVE_BMS_BOARD_H_ */
