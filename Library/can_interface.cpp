/*
 * can_manager.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#include "can_interface.h"

CANInterface::CANInterface(uint32_t can_base_address)
{
	base_address = can_base_address;

	// Enable CAN interrupt on the processor (NVIC)
	if (base_address == CAN0_BASE)
	{
		interrupt_address = INT_CAN0;
		sysctl_address = SYSCTL_PERIPH_CAN0;
	}
	else if (base_address == CAN1_BASE)
	{
		interrupt_address = INT_CAN1;
		sysctl_address = SYSCTL_PERIPH_CAN1;
	}

	can_tx_box = 1;

	// Set Initial CAN Receive Mailbox to 17
	can_rx_box = 17;
}

uint32_t CANInterface::getBitRate(CANBaudRate baud)
{
	switch (baud)
	{
	case CAN_125_KBPS:
		return 125000;

	case CAN_250_KBPS:
		return 250000;

	case CAN_500_KBPS:
		return 500000;

	case CAN_1000_KBPS:
		return 1000000;

	default:
		return 250000;
	}
}

CANReceiver* CANInterface::getHandler(unsigned int mailbox)
{
	return handler_map[mailbox - 16];
}

void CANInterface::initialize(CANBaudRate baud)
{
	// Enable CAN Peripheral
	SysCtlPeripheralEnable(sysctl_address);

	// Wait for CAN peripherals to become available
	SysCtlDelay(5);

	// Clear CAN memory
	CANInit(base_address);

	// Set CAN baud rate
	CANBitRateSet(base_address, SysCtlClockGet(), getBitRate(baud));

	// Enable Automatic Retransmission if Error
	CANRetrySet(base_address, true);

	// Enable interrupts on CAN peripheral
	CANIntEnable(base_address, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

	// Enable CAN interrupt on the processor (NVIC)
	IntEnable(interrupt_address);

	// Turn On CAN Transceiver
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);

	// Enable the CAN port
	CANEnable(base_address);
}

bool CANInterface::initializeReceiveMessage(CANReceiver* handler, tCANMsgObject* message)
{
	if (can_rx_box >= 31)
	{
		// No More Available Mailboxes
		return false;
	}

	// Configure Message Box Reception
	CANMessageSet(base_address, can_rx_box, message, MSG_OBJ_TYPE_RX);

	// Set Message Handler
	handler_map[can_rx_box - 16] = handler;

	// Increment to next Available Mailbox
	can_rx_box++;

	// Mailbox is Configured
	return true;
}

void CANInterface::sendMessage(CANMessage* msg)
{
	// Setup CAN Packet
	can_tx_message.ui32MsgID = msg->getID();
	can_tx_message.ui32MsgIDMask = 0;					// No Mask Needed on TX
	can_tx_message.ui32MsgLen = msg->getLength();		// Data Length Code
	can_tx_message.ui32Flags = MSG_OBJ_NO_FLAGS;		// Enable Interrupts
	can_tx_message.pui8MsgData = msg->getData();		// Pointer to message data

	// Check for Extended ID
	if (msg->getIDSize() == CANMessage::ID_EXTENDED)
	{
		can_tx_message.ui32Flags |= MSG_OBJ_EXTENDED_ID;
	}

	// Make sure we are only using valid mailboxes
	if (can_tx_box > 16)
	{
		return;	// Too many messages have been sent this tick
	}

	// Enter Critical Section for CAN Transmission
	IntDisable(interrupt_address);

	// Send CAN Message
	CANMessageSet(base_address, can_tx_box, &can_tx_message, MSG_OBJ_TYPE_TX);

	// This mailbox has been used in this tick
	can_tx_box++;

	// Exit Critical Section
	IntEnable(interrupt_address);
}

void CANInterface::tick()
{
	// Clear CAN mailbox counters if TX mailboxes are clear
	if (CANStatusGet(base_address, CAN_STS_TXREQUEST) == 0x00)
	{
		can_tx_box = 1;
	}
}

