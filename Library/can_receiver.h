/*
******************************************************************************
	can_receiver.h
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/
#ifndef CAN_RECEIVER_H_
#define CAN_RECEIVER_H_

#include <stdint.h>

#include "driverlib/can.h"

#include "can_ports.h"

class CANReceiver
{
public:
	// Default Constructor
	CANReceiver();

	// Return How Many Ticks since Last Successful Message
	unsigned int getLastMessageCounter();

	// Return True if a Message has Ever Been Received
	bool getMessageReceived();

	// Return How Many Ticks since Device was Powered
	unsigned int getPoweredCounter();

	// Initialize CAN Reception
	virtual void initializeCANReceive() = 0;

	// Update Message Counter
	void incrementCounters();

	// Return True if Device is Powered
	bool isPowered();

	// Return True if Communication is Good
	bool isCommunicationGood();

	// Template CAN Receive Method
	virtual bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox) = 0;

	// Set True if Device is Powered
	void setPowered(bool device_power);

	// Perform Regular Processing
	void tick();

protected:
	// True if External Device is Powered
	bool powered;

	// How Many Ticks since Last Successful Message
	unsigned int last_message_counter;

	// How Many Ticks since Device has been Powered
	unsigned int powered_counter;

	// True if a Message has Been Received Since Device Powered Up
	bool message_received;
};

#endif /* CAN_RECEIVER_H_ */
