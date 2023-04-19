/*
 * bms_monitor_array.h
 *
 *  Created on: Jan 18, 2016
 *      Author: Erik
 */

#ifndef BMS_MONITOR_ARRAY_H_
#define BMS_MONITOR_ARRAY_H_

#include "Library/can_interface.h"
#include "Library/can_receiver.h"

#include "bms_monitor.h"
#include "configuration.h"

// Class for Managing Array of Monitor Boards
class BMSMonitorArray : public CANReceiver
{
public:
	// Default Constructor
	BMSMonitorArray(CANInterface* can_interface);

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

	// Return the Number of Detected Monitor Boards
	unsigned int getNumDetectedBoards();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if Communcation is Good
	bool isCommunicationGood();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Perform Regular Processing
	void tick();

public:
	// Array of Monitor Board Objects
	BMSMonitor monitor_array[EXPECTED_MONITOR_BOARDS];

public:
	// Pointer to CAN System Object
	CANInterface* can;

	// Array of All Possible Board Addresses
	bool detected_boards[64];

	// Number of Monitor Boards Detected
	unsigned int num_detected_boards;

	// Monitor System Errors
	bool monitor_errors[NUM_MONITOR_ERRORS];
};

#endif /* BMS_MONITOR_ARRAY_H_ */
