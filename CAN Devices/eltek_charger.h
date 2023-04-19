/*
******************************************************************************
	eltek_charger.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef ELTEK_CHARGER_H_
#define ELTEK_CHARGER_H_

#include "Library/can_interface.h"
#include "Library/can_receiver.h"

class EltekCharger : public CANReceiver
{
public:
	// Charger State
	enum ChargerState
	{
		CHARGER_IDLE = 1,
		CHARGER_CHARGE,
		CHARGER_RECOVERABLE_ERROR,
		CHARGER_NONRECOVERABLE_ERROR
	};

	// Default Constructor
	EltekCharger(CANInterface* can_interface, unsigned int address);

	// Disable Charger
	void disable();

	// Enable Charger
	void enable();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Send Broadcast Command Message
	void sendBroadcastCommandMessage();

	// Send Command Message
	void sendCommandMessage();

	// Set Output Limits
	void setOutputLimits(float voltage, float current, float power);

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// CAN ID Offset of Message
	unsigned int can_address;

	// Time charger has been unpowered
	unsigned int off_time;

	// True to Enable Charger
	bool charger_enable;

	// Output Voltage Limit
	float dc_voltage_limit;

	// Output Current Limit
	float dc_current_limit;

	// Reference Power Demand in %
	float power_reference;

	// Feedback Values
	ChargerState charger_state;

	// Charge Available Power in %
	float charger_available_power;

	// Input Voltage Measurement
	float mains_voltage;

	// Input AC Current
	float mains_current;

	// Input AC Frequency (Hz)
	float mains_frequency;

	// Reported DC Voltage (V)
	float dc_voltage;

	// Reported DC Current (A)
	float dc_current;

	// Maximum Power (W)
	float max_power;

	// Primary Temperature Measurement
	float primary_temp;

	// Secondard Temperature Measurement
	float secondary_temp;

	// Counters
	unsigned int tick_counter;
};

#endif /* ELTEK_CHARGER_H_ */
