/*
******************************************************************************
	mmc4000.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef MMC4000_H_
#define MMC4000_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class MMC4000 : public CANReceiver
{
public:
	// Default Constructor
	MMC4000(CANInterface* can_interface);

	// Disable Heater and Pump
	void disable();

	// Enable Heater and Pump
	void enable();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return true if Heater is Enabled
	bool isEnabled();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Send Command Message
	void sendCommandMessage();

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// True if Heater is Enabled
	bool enabled;

	// Commands
	unsigned int battery_current_limit;		// A
	unsigned int power_demand;				// %

	// True if Hardware Fault is Active
	bool hardware_fault;
	float measured_temperature;				// DegC
	float measured_power_percent;			// %
	float measured_heater_power;			// W
	float measured_battery_voltage;			// V

	// Counters
	unsigned int tick_counter;
};

#endif /* MMC4000_H_ */
