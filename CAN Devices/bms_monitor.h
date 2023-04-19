/*
 * bms_monitor.h
 *
 *  Created on: Jan 14, 2016
 *      Author: Erik
 */

#ifndef BMS_MONITOR_H_
#define BMS_MONITOR_H_

#include <stdint.h>

#include "Library/can_system.h"

class BMSMonitor
{
public:
	// Default Constructor
	BMSMonitor();

	// CAN Message Types
	enum CANMessageType
	{
		BOARD_STATUS 			= 0x10,
		CELL_1_4_VOLTAGE		= 0x20,
		CELL_5_8_VOLTAGE		= 0x21,
		CELL_9_12_VOLTAGE		= 0x22,
		CELL_1_4_TEMPERATURE	= 0x30,
		CELL_5_8_TEMPERATURE	= 0x31,
		CELL_9_12_TEMPERATURE	= 0x32,
		CELL_SHUNTING_COMMAND	= 0x40,
		FIRMWARE_VERSION		= 0xA0
	};

	// BMS Monitor Board Status Flags
	enum StatusFlag
	{
		SHUNTING_ACTIVE = 0,
		SHUNTING_PAUSED_FOR_TEMP,
		NUM_STATUS_FLAGS
	};

	// BMS Monitor Board Fault Flags
	enum FaultFlag
	{
		LTC6804_COMMUNICATION_FAILURE = 0,
		BOARD_OVERTEMP_FAULT,
		ADDRESS_CONFLICT,
		NUM_FAULT_FLAGS
	};

	// Return True if Cell Channel is Shunting
	bool getBalancingCommand(unsigned int local_index);

	// Return Cell Temperature
	float getCellTemperature(unsigned int local_index);

	// Return Cell Voltage
	float getCellVoltage(unsigned int local_index);

	// Get Whether Cell Input is Connected
	bool isCellInputConnected(unsigned int local_index);

	// Return True if Communication is Good
	bool isCommunicationGood();

	// Get Whether Temperature Input is Connected
	bool isTemperatureInputConnected(unsigned int local_index);

	// Process Received CAN Message
	bool receiveCANMessage(tCANMsgObject* msg);

	// Send CAN Message to BMS Monitor Board
	void sendCANMessage(CANMessageType msg_type);

	// Set Board Address
	void setAddress(unsigned int new_address);

	// Set Balancing Command for Given Cell Input
	void setBalancingCommand(unsigned int local_index, bool shunting);

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

	// Board Address
	unsigned int address;			// 0 - (N-1)

	// Cell Voltages
	float cell_voltages[12];		// Volts

	// Cell Temperatures
	float cell_temperatures[12];	// DegC

	// Current Cell Shunting Command
	bool cell_shunting[12];

	// Map of Connected Inputs - True for Valid Signals
	bool connected_cells[12];
	bool connected_temperatures[12];

	// Global Index of Local Value
	unsigned int cell_map[12];
	unsigned int temperature_map[12];

	// Board Status Information
	float sense_12v;				// Volts
	float board_temperature;		// DegC

	// Board Status Flags
	bool status_flags[NUM_STATUS_FLAGS];

	// Board Fault Flags
	bool fault_flags[NUM_FAULT_FLAGS];

	// Firmware Version Numbers
	unsigned int version_major;
	unsigned int version_minor;
	unsigned int version_revision;

	// Number of Ticks Since Last Message Reception
	unsigned int last_message_counter;

	// True if a Message has been Received from this Board
	bool message_received;
};

#endif /* BMS_MONITOR_H_ */
