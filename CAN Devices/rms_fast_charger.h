/*
 * rms_fast_charger.h
 *
 *  Created on: Mar 20, 2016
 *      Author: Erik
 */

#ifndef RMS_FAST_CHARGER_H_
#define RMS_FAST_CHARGER_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class RMSFastCharger : public CANReceiver
{
public:
	// Default Constructor
	RMSFastCharger(CANInterface* can_interface, unsigned int inverter_offset, unsigned int dcdc_offset);

	// Disable Fast Charger
	void disable();

	// Enable Fast Charger
	void enable();

	// Return DC Output Voltage
	float getDCVoltage();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if CAN Communication is Good
	bool isCommunicationGood();

	// Return True if Inverter Fault is Active
	bool isFaultActive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Send Command Packet to Fast Charger
	void sendCommandMessage();

	// Set Fast Charger Output Limits
	void setOutputLimits(float voltage, float current);

	// Set Whether Fast Charger is Powered
	void setPowered(bool device_power);

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// CAN ID Offset of Message
	unsigned int inverter_can_offset;
	unsigned int dcdc_can_offset;

	// Command Values
	bool converter_enable;
	float voltage_command;
	float current_command;
	bool boost_mode;

	// Feedback Values
	float lv_internal_voltage;
	float lv_voltage;
	float hv_voltage;
	float reference_voltage;

	float dc_bus_current;

	float dcdc_inductor_1_temp;
	float dcdc_inductor_2_temp;
	float dcdc_inductor_3_temp;
	float dcdc_gdb3_temp;

	float inverter_vac_rms;
	float inverter_input_frequency;

	uint16_t dcdc_post_faults_low;
	uint16_t dcdc_post_faults_high;
	uint16_t line_inverter_post_faults_low;
	uint16_t line_inverter_post_faults_high;

	uint16_t dcdc_faults_low;
	uint16_t dcdc_faults_high;
	uint16_t line_inverter_faults_low;
	uint16_t line_inverter_faults_high;

	// Counters
	unsigned int tick_counter;
	unsigned int last_message_dcdc_counter;
	unsigned int last_message_line_inverter_counter;
};

#endif /* RMS_FAST_CHARGER_H_ */
