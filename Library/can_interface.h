/*
 * can_manager.h
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#ifndef LIBRARY_CAN_SYSTEM_H_
#define LIBRARY_CAN_SYSTEM_H_

#include <stdint.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

#include "can_message.h"
#include "can_receiver.h"

// CAN Interface Class
class CANInterface
{
public:
	// Constructor
	CANInterface(uint32_t can_base_address);

	// Supported CAN Baud Rates
	enum CANBaudRate
	{
		CAN_125_KBPS = 0,
		CAN_250_KBPS,
		CAN_500_KBPS,
		CAN_1000_KBPS
	};

	// Return Integer Value of CAN Baud Rate
	uint32_t getBitRate(CANBaudRate baud);

	// Return the Handler for a Given Mailbox
	CANReceiver* getHandler(unsigned int mailbox);

	// Initialize CAN Interface
	void initialize(CANBaudRate baud);

	// Setup CAN Message Reception, Return False if No Mailboxes are Available
	bool initializeReceiveMessage(CANReceiver* handler, tCANMsgObject* message);

	// Send CAN Message
	void sendMessage(CANMessage* msg);

	// Perform Regular Processing
	void tick();

private:
	// CAN Peripheral Base Address
	uint32_t base_address;

	// CAN Peripheral Interrupt Address
	uint32_t interrupt_address;

	// CAN Peripheral System Control Address
	uint32_t sysctl_address;

	// Current CAN Mailbox for Next Send
	unsigned int can_tx_box;

	// Next Available CAN Mailbox for Reception
	unsigned int can_rx_box;

	// Message Handler Map
	CANReceiver* handler_map[16];

public:
	// CAN Data Objects
	tCANMsgObject can_tx_message;
	tCANMsgObject can_rx_message;
	uint8_t can_tx_data[8];
	uint8_t can_rx_data[8];
};

#endif /* LIBRARY_CAN_SYSTEM_H_ */
