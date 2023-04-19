/*
 * can_manager.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#include "can_system.h"

CANSystem::CANSystem() : can_1(CAN0_BASE)
#ifdef USE_CAN1_INTERFACE
						, can_2(CAN1_BASE)
#endif
{

}

void CANSystem::initialize()
{
	// Initialize CAN 1
	can_1.initialize(CANInterface::CAN_1_BAUD_RATE);

#ifdef USE_CAN1_INTERFACE
	// Initialize CAN 2
	can_2.initialize(CANInterface::CAN_2_BAUD_RATE);
#endif
}

bool CANSystem::initializeReceiveMessage(CANPort can_port, CANReceiver* handler, tCANMsgObject* rx_message)
{
	bool success = false;

	if (can_port == CAN_1)
	{
		success = can_1.initializeReceiveMessage(handler, rx_message);
	}

#ifdef USE_CAN1_INTERFACE
	else if (can_port == CAN_2)
	{
		success = can_2.initializeReceiveMessage(handler, rx_message);
	}
#endif

	return success;
}

void CANSystem::sendMessage(CANPort port, CANMessage* msg)
{
	if (port == CAN_1)
	{
		can_1.sendMessage(msg);
	}

#ifdef USE_CAN1_INTERFACE
	else if (port == CAN_2)
	{
		can_2.sendMessage(msg);
	}
#endif
}

void CANSystem::tick()
{
	// Clear CAN mailbox counters if TX mailboxes are clear
	can_1.tick();

#ifdef USE_CAN1_INTERFACE
	can_2.tick();
#endif
}

