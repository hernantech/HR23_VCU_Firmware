/*
******************************************************************************
	can_receiver.cpp
	VCU1200_Base
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "can_receiver.h"

CANReceiver::CANReceiver()
{
	last_message_counter = 10000;
}

unsigned int CANReceiver::getLastMessageCounter()
{
	return last_message_counter;
}

bool CANReceiver::getMessageReceived()
{
	return message_received;
}

unsigned int CANReceiver::getPoweredCounter()
{
	return powered_counter;
}

void CANReceiver::incrementCounters()
{
	// Increment Message Counter with Controlled Overflow
	if (last_message_counter < 1000000000)
	{
		last_message_counter++;
	}
	else
	{
		last_message_counter = 1000001;
	}

	if (powered)
	{
		// Increment Powered Counter with Controlled Overflow
		if (powered_counter < 1000000000)
		{
			powered_counter++;
		}
		else
		{
			last_message_counter = 1000001;
		}
	}
	else
	{
		powered_counter = 0;
	}
}

bool CANReceiver::isPowered()
{
	return powered;
}

bool CANReceiver::isCommunicationGood()
{
	if (message_received && last_message_counter < 200)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CANReceiver::setPowered(bool device_power)
{
	powered = device_power;

	if (!powered)
	{
		message_received = false;
		last_message_counter = 1000001;
		powered_counter = 0;
	}
}

void CANReceiver::tick()
{
	// Increment Counters
	incrementCounters();
}
