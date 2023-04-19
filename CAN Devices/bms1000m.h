/*
******************************************************************************
	bms1000m.h
	VCU1200_Base
	Erik Stafl
	1/23/2016

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef BMS1000M_H_
#define BMS1000M_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class BMS1000M : public CANReceiver
{
public:
	// State of the BMS
	enum BMSState
	{
		INTERNAL_ERROR = 0,
		SLEEP,
		STARTUP,
		IDLE,
		PRECHARGE,
		ACTIVE,
		FAULT,
		NUM_BMS_STATES
	};

	// BMS Commands
	enum BMSCommand
	{
		NO_COMMAND = 0,
		BMS_SLEEP,
		BMS_IDLE,
		BMS_PRECHARGE,
		BMS_CLEAR_FAULTS,
		BMS_PRECHARGE_COMPLETE,
		NUM_BMS_COMMANDS
	};

	// BMS Fault Conditions
	enum BMSFault
	{
		PACK_VOLTAGE_HIGH = 0,
		PACK_VOLTAGE_LOW,
		CELL_VOLTAGE_HIGH,
		CELL_VOLTAGE_LOW,
		HIGH_PACK_DISCHARGE_CURRENT,
		HIGH_PACK_CHARGE_CURRENT,
		HIGH_TEMPERATURE,
		LOW_TEMPERATURE,
		PRECHARGE_FAULT,
		BMS_COMMUNICATION_FAULT,
		DISCHARGE_ENVELOPE_FAULT,
		CHARGE_ENVELOPE_FAULT,
		ISOLATION_LEAKAGE_FAULT,
		NUM_BMS_FAULTS
	};

	// Monitor Board Communication Status
	enum MonitorError
	{
		MONITOR_INCORRECT_NUMBER = 0,
		MONITOR_INCOMPATIBLE_FIRMWARE,
		MONITOR_LOSS_OF_ALL_COMMUNICATION,
		MONITOR_ADDRESS_CONFLICT,
		MONITOR_UNEXPECTED_ADDRESS,
		NUM_MONITOR_ERRORS
	};

	// State of the BMS
	enum BMSStatusFlag
	{
		PRECHARGE_RELAY = 0,
		MAIN_CONTACTOR_POS,
		MAIN_CONTACTOR_NEG,
		HVIL_PRESENT,
		BMS_MONITOR_POWER,
		CELL_BALANCING,
		MONITOR_COMMUNICATION_GOOD,
		NUM_STATUS_FLAGS
	};

	// Default Constructor
	BMS1000M(CANInterface* can_interface, unsigned int can_id_offset);

	// Return Highest Cell Temperature
	float getMaxCellTemp();

	// Return Highest Cell Voltage
	float getMaxCellVoltage();

	// Return Lowest Cell Temperature
	float getMinCellTemp();

	// Return Lowest Cell Voltage
	float getMinCellVoltage();

	// Return Measured Pack Current
	float getPackCurrent();

	// Return Measured Pack Voltage
	float getPackVoltage();

	// Return Measured Precharge Voltage
	float getPrechargeVoltage();

	// Return Reported State of Charge
	float getSOC();

	// Return Current State of BMS
	BMSState getState();

	// Return a BMS Status Flag
	bool getStatusFlag(BMSStatusFlag status);

	// Return Pack Sum of Cells Voltage
	float getSumOfCellsVoltage();

	// Initialize CAN Message Reception
	void initializeCANReceive();

	// Return True if Fault is Active
	bool isFaultActive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Request BMS Firmware Version
	void requestFirmwareVersion();

	// Request BMS State Change
	void requestStateChange(BMSCommand command);

	// Resets All Class Data
	void reset();

	// Perform Regular Processing
	void tick();

private:
	// CAN ID Offset
	unsigned int can_offset;

	// CAN Interface Pointer
	CANInterface* can;

	// True if Fault Condition is Active
	bool fault_active;

	// Fault Flags
	bool faults[NUM_BMS_FAULTS];

	// Monitor Error Flags
	bool monitor_errors[NUM_MONITOR_ERRORS];

	// BMS Status Flags
	bool status_flags[NUM_STATUS_FLAGS];

	// True if Contactor Opening is Imminent
	bool contactor_open_imminent;

	// Current BMS State
	BMSState bms_state;

	// Number of Monitor Boards Detected
	unsigned int num_monitor_boards;

	// Maximum Discharge Current
	float max_discharge_current;	// A
	float max_charge_current;		// A

	// Pack Data
	float pack_voltage;				// V
	float sum_of_cells_voltage;		// V
	float precharge_voltage;		// V
	float pack_current;				// A
	float soc;						// %
	float soh;						// %

	// Cell Data
	float max_cell_voltage;			// V
	float min_cell_voltage;			// V
	float max_cell_temperature;		// Deg C
	float min_cell_temperature;		// Deg C

	// Version Numbers
	unsigned char version_major;
	unsigned char version_minor;
	unsigned char version_revision;

	// Number of Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* BMS1000M_H_ */
