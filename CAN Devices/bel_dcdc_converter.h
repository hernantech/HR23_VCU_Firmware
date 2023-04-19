/*
******************************************************************************
	bel_dcdc_converter.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef BEL_DCDC_CONVERTER_H_
#define BEL_DCDC_CONVERTER_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class BelDCDCConverter : public CANReceiver
{
public:
	// Status Flags
	enum StatusFlag
	{
		UNIT_ENABLED = 0,
		WAKEUP,
		FAULT,
		LATCHED_FAULT,
		REMOTE_SHUTDOWN,
		STARTUP_DONE,
		INPUT_OK,
		HVIL_OK,
		REVERSE_POLARITY,
		NUM_STATUS_FLAGS
	};

	// Default Constructor
	BelDCDCConverter(CANInterface* can_interface, unsigned int device_can_address);

	// Disable DC DC Converter
	void disable();

	// Enable DC DC Converter
	void enable();

	// Return Number of Ticks Since Contactor Closure
	unsigned int getContactorCounter();

	// Get Input Voltage
	float getInputVoltage();

	// Get Status Flag
	bool getStatusFlag(StatusFlag flag);

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Reset Contactor Counter
	void resetContactorCounter();

	// Set Output Voltage
	void setOutputVoltage(float voltage);

	// Send Command Packet to Inverter
	void sendCommandMessage();

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// CAN ID Offset of Message
	unsigned int can_address;

	// Status Flags as Reported by Converter
	bool status_flags[NUM_STATUS_FLAGS];

	// True if New Settings are Present
	bool send_message_flag;

	// True to Enable Output
	bool output_enable_command;

	// True if Output is Currently On
	bool output_enabled;

	// Output Settings
	float output_voltage_setting;	// V
	float output_current_setting;	// A

	// Output Values
	float output_voltage;	// V
	float output_current;	// A

	// Input Values
	float input_voltage;	// V

	// Temperature
	float cooling_plate_temperature;

	// Number of Ticks Since Contactor Closure
	unsigned int contactor_counter;

	// Number of Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* BEL_DCDC_CONVERTER_H_ */
