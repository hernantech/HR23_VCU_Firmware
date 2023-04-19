/*
******************************************************************************
	active_bms_array.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef ACTIVE_BMS_ARRAY_H_
#define ACTIVE_BMS_ARRAY_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

#include "CAN Devices/active_bms_board.h"

#include "configuration.h"

class ActiveBMSArray : public CANReceiver
{
public:
	// Default Constructor
	ActiveBMSArray(CANInterface* can_interface, unsigned int can_id_offset);

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if Communcation is Good
	bool isCommunicationGood();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// CAN Message ID Offset
	unsigned int can_offset;

	// Array of BMS Boards
	ActiveBMSBoard bms_boards[EXPECTED_MONITOR_BOARDS];

	// Number of Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* ACTIVE_BMS_ARRAY_H_ */
