/*
******************************************************************************
	delphi_u2c.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef DELPHI_U2C_H_
#define DELPHI_U2C_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class DelphiU2C : public CANReceiver
{
public:
	// Default Constructor
	DelphiU2C(CANInterface* can_interface);

	// Disable DC-DC Converter
	void disable();

	// Enable DC-DC Converter
	void enable();

	// Number of Ticks Since Contactor Closed
	unsigned int getContactorCounter();

	// Get DC Bus Current
	float getInputCurrent();

	// Get DC Bus Voltage at Converter
	float getInputVoltage();

	// Get 12V Output Current
	float getOutputCurrent();

	// Get Output Voltage
	float getOutputVoltage();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if Inverter Fault is Active
	bool isFaultActive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Send Command Packet to DC-DC Converter
	void sendCommandMessage();

	// Set Output Voltage
	void setOutputVoltage(float voltage);

	// Reset Contactor Counter
	void resetContactorCounter();

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission and Reception
	CANInterface* can;

	// Output Enable Command
	bool output_enable;

	// Master Fault Bit
	bool master_fault;

	// Undervoltage Fault
	bool input_low_voltage_fault;

	// Input Voltage
	float input_voltage;			// V

	// Input Current
	float input_current;			// A

	// Reported Output Voltage
	float output_voltage;			// V

	// Reported Output Current
	float output_current;			// A

	// Reported Internal Elecronics Temperature
	float electronics_temp;			// DegC

	// Reported Coolant Temperature
	float coolant_temp;				// DegC

	// Reported Power Loss
	float power_loss;				// W

	// Output Voltage Setpoint
	float output_voltage_setting;	// V

	// Ticks since Contactor Closed
	unsigned int contactor_counter;

	// Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* DELPHI_U2C_H_ */
