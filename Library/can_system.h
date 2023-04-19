/*
 * can_manager.h
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#ifndef CAN_SYSTEM_H_
#define CAN_SYSTEM_H_

#include <stdint.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

#include "can_interface.h"
#include "can_ports.h"
#include "configuration.h"

// CAN System Class
class CANSystem
{
public:
	// Constructor
	CANSystem();

	// Initialize CAN Interfaces
	void initialize();

	// Initialize Receive Message for a Given CAN Interface, Return False if No Mailboxes Left
	bool initializeReceiveMessage(CANPort can_port, CANReceiver* handler, tCANMsgObject* rx_message);

	// Send CAN Message on Given CAN Interface
	void sendMessage(CANPort port, CANMessage* msg);

	// Perform Regular Processing
	void tick();

public:
	// CAN0 Interface
	CANInterface can_1;

#ifdef USE_CAN1_INTERFACE
	// CAN1 Interface
	CANInterface can_2;
#endif
};

#endif /* CAN_SYSTEM_H_ */
