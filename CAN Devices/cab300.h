/*
******************************************************************************
	cab300.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef CAB300_H_
#define CAB300_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class CAB300 : public CANReceiver
{
public:
	// Default Constructor
	CAB300(CANInterface* can_interface, unsigned int can_id_address);

	// Get Reported Current
	float getCurrent();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if Device Fault is Active
	bool isFaultActive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Reset Current
	void reset();

private:
	// CAN Interface for Message Reception
	CANInterface* can;

	// CAN Address of Device to Receive Messages
	unsigned int can_address;

	// Reported Device Current
	float current;		// A

	// True if Device is Reporting an Error
	bool error;
};

#endif /* CAB300_H_ */
