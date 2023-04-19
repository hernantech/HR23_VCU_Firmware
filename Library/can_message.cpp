/*
 * can_message.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#include "can_message.h"

CANMessage::CANMessage(IDSize idsize, MessageType msgtype, unsigned char datalength)
{
	id_size = idsize;
	message_type = msgtype;

	if (datalength <= 8)
	{
		data_length = datalength;
	}
}

unsigned char* CANMessage::getData()
{
	return data;
}

uint32_t CANMessage::getID()
{
	return id;
}

CANMessage::IDSize CANMessage::getIDSize()
{
	return id_size;
}

// Get Length of Data Section (0-8)
unsigned char CANMessage::getLength()
{
	return data_length;
}

void CANMessage::setID(uint32_t new_id)
{
	// Check to Make Sure ID is Valid
	if (id_size == ID_STANDARD)
	{
		if (new_id <= 0x7FF)
		{
			id = new_id;
		}
	}
	else if (id_size == ID_EXTENDED)
	{
		if (new_id <= 0x1FFFFFFF)
		{
			id = new_id;
		}
	}
}

void CANMessage::setIDSize(IDSize size)
{
	id_size = size;
}

void CANMessage::setLength(unsigned char length)
{
	// Check to Make Sure Length is Valid
	if (length <= 8)
	{
		data_length = length;
	}
}
