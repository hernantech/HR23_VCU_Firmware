/*
******************************************************************************
	current_ways_charger.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef CURRENT_WAYS_CHARGER_H_
#define CURRENT_WAYS_CHARGER_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class CurrentWaysCharger : public CANReceiver
{
public:
	// Default Constructor
	CurrentWaysCharger(CANInterface* can_interface);

	// Disable DC DC Converter
	void disable();

	// Enable DC DC Converter
	void enable();

	// Get Input Voltage
	float getACVoltage();

	// Return Number of Ticks Since Contactor Closure
	unsigned int getContactorCounter();

	// Get DC Output Voltage
	float getDCVoltage();

	// Get Maximum J1772 Current
	float getMaxJ1772Current();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Reset Contactor Counter
	void resetContactorCounter();

	// Set Charger CC Limit
	void setOutputCurrent(float current);

	// Set Charger CV Limit
	void setOutputVoltage(float voltage);

	// Send Command Packet to Inverter
	void sendCommandMessage();

	// Send 0x270 Startup Message
	void sendStartupMessage();

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

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
	float dc_output_voltage;	// V
	float dc_output_current;	// A
	float dc_output_power;		// W

	// Input Values
	float ac_input_voltage;			// Vrms
	float ac_input_current;			// Arms
	float j1772_max_current;		// A
	float j1772_proximity_voltage;	// V

	// Number of Ticks Since Contactor Closure
	unsigned int contactor_counter;

	// Number of Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* CURRENT_WAYS_CHARGER_H_ */
