/*
 * can_message.h
 *
 *  Created on: Dec 13, 2015
 *      Author: Erik
 */

#ifndef CAN_MESSAGE_H_
#define CAN_MESSAGE_H_

#include <stdint.h>

class
{
public:
	// Message ID Size
	enum IDSize
	{
		ID_STANDARD = 0,
		ID_EXTENDED
	};

	// Message Type
	enum MessageType
	{
		DATA_FRAME = 0,
		REMOTE_FRAME
	};

	// Default Constructor
	CANMessage();

	// Setup Constructor
	CANMessage(IDSize idsize, MessageType msgtype, unsigned char datalength);

	// Return Pointer to Data Array
	unsigned char* getData();

	// Get Message ID
	uint32_t getID();

	// Get Message ID Size (Standard or Extended)
	IDSize getIDSize();

	// Get Data Length (0-8)
	unsigned char getLength();

	// Set Message ID
	void setID(uint32_t new_id);

	// Set Message ID Size
	void setIDSize(IDSize size);

	// Set Message Length
	void setLength(unsigned char length);

private:
	// ID Type
	IDSize id_size;

	// Message Type
	MessageType message_type;

	// CAN ID
	uint32_t id;

	// Data Length
	unsigned char data_length;

	// Message Data
	unsigned char data[8];
};

#endif /* CAN_MESSAGE_H_ */
