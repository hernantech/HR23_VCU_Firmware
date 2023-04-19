/*
******************************************************************************
	application.h
	VCU1200_Base
	Erik Stafl
	1/23/2015


	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef VCU1200_TUTORIAL_1_BLINKINGLIGHTS_APPLICATION_H_
#define VCU1200_TUTORIAL_1_BLINKINGLIGHTS_APPLICATION_H_

#include "Devices/vcu1200_board.h"
#include "Library/can_receiver.h"

#include "functional_io.h"

enum SystemState
{
	STARTUP = 0,
	SYSTEM_OFF,
	SYSTEM_ON
};

class Application : public CANReceiver
{
public:
	// Default Constructor
	Application();

	// Change System to New State
	void changeState(SystemState new_state);

	// Initialize Application
	void initialize();

	// Initialize CAN Message Reception
	void initializeCANReceive();

	// Determine if State Changes are Necessary
	void processState();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Perform Regular Processing
	void tick();

	void calculateLEDS();

private:
	// Increment Counters
	void incrementCounters();

	// Send System CAN Data
	void sendCANData();

private:
/*	// Secondary VCU Object
	SecondaryVCU secondary_vcu;

	// Resistance Welder
	ResistanceWelder resistance_welder;

	// XY Stage Motors
	LinearStage x_axis;
	LinearStage y_axis;

	// Weld Center States
	WeldCenterState state;

	// Command Status
	CommandStatus command_status;

	// Current Command
	Command current_command;

	// Next Command
	Command next_command;*/

	// True if New Command has been Received
	bool new_command;

	SystemState state;

	// The Current Accelerator Position
	float potentiometer_command;		// %

	// How Many Ticks System has been in Current State
	unsigned int state_counter;
/*
	// Turn Knob Positions
	TurnKnobPosition turn_knob;

	// Joystick Positions
	JoystickPosition joystick;

	// True if Joystick had been Released before this Tick
	bool joystick_released;

	// True if Movement is Allowed
	bool movement_allowed;

	// True if Origin has been Set
	bool origin_set;

	// High Frequency Counter
	unsigned int hf_counter;*/
};

#endif /* VCU1200_TUTORIAL_1_BLINKINGLIGHTS_APPLICATION_H_ */
