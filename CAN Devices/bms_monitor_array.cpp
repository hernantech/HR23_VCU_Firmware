/*
 * bms_monitor_array.cpp
 *
 *  Created on: Jan 18, 2016
 *      Author: Erik
 */

#include "bms_monitor_array.h"

BMSMonitorArray::BMSMonitorArray(CANInterface* can_interface)
{
	can = can_interface;
}

unsigned int BMSMonitorArray::getNumDetectedBoards()
{
	return num_detected_boards;
}


void BMSMonitorArray::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN Interface

	// All BMS Messages
	can_rx_message.ui32MsgID = 0x00000000;
	can_rx_message.ui32MsgIDMask = 0x1FFF0000;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_EXT_FILTER | MSG_OBJ_EXTENDED_ID;
	can->initializeReceiveMessage(this, &can_rx_message);
}

bool BMSMonitorArray::isCommunicationGood()
{
	for (int i = 0; i < EXPECTED_MONITOR_BOARDS; i++)
	{
		if (!monitor_array[i].isCommunicationGood())
		{
			return false;
		}
	}

	return true;
}

bool BMSMonitorArray::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	// True if Message has been Processed
	bool processed = false;

	// Send CAN Message for Processing to Given Address
	unsigned int address = message->ui32MsgID & 0x000000FF;

	// Check for Valid Address
	if (address < 64)
	{
		// Update Array of Detected Boards
		if (!detected_boards[address])
		{
			detected_boards[address] = true;

			// Update Number of Detected Boards
			num_detected_boards++;
		}

		// Check for Expected Address in this Application
#pragma diag_suppress 188
		if (address < EXPECTED_MONITOR_BOARDS + MONITOR_BOARD_BASE_ADDRESS && address >= MONITOR_BOARD_BASE_ADDRESS)
#pragma diag_default 188
		{
			// Only Receive Messages if BMS has been powered for > 2 seconds
			if (isPowered() && getPoweredCounter() > 200)
			{
				// Process Valid Message
				processed = monitor_array[address - MONITOR_BOARD_BASE_ADDRESS].receiveCANMessage(message);

				// Reset Last Message Counter
				last_message_counter = 0;
			}
		}
		else
		{
			// Invalid Address for this Application
			monitor_errors[MONITOR_UNEXPECTED_ADDRESS] = true;
		}
	}

	if (processed)
	{
		last_message_counter = 0;
		message_received = true;
	}

	return processed;
}

void BMSMonitorArray::tick()
{
	// Only Perform this Processing if Array is Active
	if (isPowered())
	{
		// Check for Complete Loss of Communication
		if (last_message_counter >= 200)
		{
			monitor_errors[MONITOR_LOSS_OF_ALL_COMMUNICATION] = true;
		}
		else
		{
			monitor_errors[MONITOR_LOSS_OF_ALL_COMMUNICATION] = false;
		}

		// Wait 4 Seconds before Flagging Faults
		if (powered_counter > 400)
		{
			if (num_detected_boards != EXPECTED_MONITOR_BOARDS)
			{
				monitor_errors[MONITOR_INCORRECT_NUMBER] = true;
			}
			else
			{
				monitor_errors[MONITOR_INCORRECT_NUMBER] = false;
			}
		}

		// Send Balancing Commands
		if (powered_counter % 10 == 0)
		{
			// Boards 0-3
			for (int i = 0; i <= 3; i++)
			{
				if (i < EXPECTED_MONITOR_BOARDS)
				{
					monitor_array[i].sendCANMessage(BMSMonitor::CELL_SHUNTING_COMMAND);
				}
			}
		}
		else if (powered_counter % 10 == 1)
		{
			// Boards 4-7
			for (int i = 4; i <= 7; i++)
			{
				if (i < EXPECTED_MONITOR_BOARDS)
				{
					monitor_array[i].sendCANMessage(BMSMonitor::CELL_SHUNTING_COMMAND);
				}
			}
		}
		else if (powered_counter % 10 == 2)
		{
			// Boards 8-11
			for (int i = 8; i <= 11; i++)
			{
				if (i < EXPECTED_MONITOR_BOARDS)
				{
					monitor_array[i].sendCANMessage(BMSMonitor::CELL_SHUNTING_COMMAND);
				}
			}
		}
	}

	// Increment Counters
	incrementCounters();
}
