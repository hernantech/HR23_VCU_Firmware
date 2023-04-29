/*
******************************************************************************
	Class for Cascadia Motion Motor Controller, i.e. Inverter
	Alex Hernandez

	04/28/2023

	Written for All Processors
	Language: Embedded C++

	Copyright lol MIT License, y'all can enjoy this
******************************************************************************
*/

#ifndef INVERTERHR23_H_
#define INVERTERHR23_H_

#include "Library/critical.h"
#include "Library/can_interface.h"
#include "Library/can_receiver.h"
#include "Library/error.h"

class InverterHR23 : public CANReceiver
{
public:
    /*
     //
     //Checking whether to implement these (regarding
     //
	// Inverter Directions
	enum Direction
	{
		NEUTRAL = 0,
		FORWARD,
		REVERSE
	};
     */


    // Inverter State Machine State (CANID 0x0AA, internal states)
        // Note that enum implicitly assigns values as the variables get listed
        //i.e. VMS_STATE_2 = 2, VMS_STATE_3 = 3, etc.
	enum VSMState
	{
		VSM_STATE_1 = 1,
		VSM_STATE_2,
		VSM_STATE_3,
		VSM_STATE_4,
		VSM_STATE_5,
		VSM_STATE_6,
		VSM_STATE_7
	};



	// Default Constructor
	InverterHR23(CANInterface* can_interface, unsigned int can_id_offset);

	// Disable Inverter
	void disable();

	// Enable Inverter
	void enable();

	// Get DC Bus Current
	float getDCBusCurrent();

	// Get DC Bus Voltage at Inverter
	float getDCBusVoltage();

	// Get Delta Resolver Angle
	float getDeltaResolver();

	// Return the Current Direction of the Inverter
	Direction getDirection();

	// Return Maximum Temperature within Inverter
	float getMaxTemperature();

	// Get Motor Speed in RPM
	float getMotorSpeed();

	// Get Motor Temperature in Deg C
	float getMotorTemperature();

	// Initialize CAN Messages for Reception
	void initializeCANReceive();

	// Return True if Inverter Fault is Active
	bool isFaultActive();

	// Process Received CAN Message
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Resets All Class Data
	void reset();

	// Send Command Packet to Inverter
	void sendCommandMessage();

	// Set Inverter Direction
	void setDirection(Direction direction);

	// Set Torque Command
	void setTorqueCommand(float tq_cmd);

	// Perform Regular Processing
	void tick();

public:
	// CAN Interface for Message Transmission
	CANInterface* can;

	// CAN ID Offset of Message
	unsigned int can_offset;

	// Current Inverter Direction
	Direction direction;
	Direction requested_direction;

	// How many ticks we've been in the current direction
	unsigned int direction_counter;

	// Current VSM State
	VSMState vsm_state;





    //Device spits out Digital Input Status via CANID 0x0A4
    enum SwitchState{
        IGNITION_STATE,
        START_STATE,
        VALET_STATE

    }

    //DIS = Digital Input Status from 0x0A4
    bool DIS0; // Forward Switch
    bool DIS1; // Reverse Switch
    bool DIS2; // Brake Switch
    bool DIS3; // Regen Disable Switch
    bool DIS4; // Ignition switch
    bool DIS5; // Start Switch
    bool DIS6; // Valet mode




	// True if Inverter is Powered Up
	bool enabled;

	// Inverter Command Enable Bit
	bool inverter_enabled;

	bool inverter_lockout;
	bool inverter_lockout_clear;

	bool enable_discharge;
	bool main_contactor;	// True if Closed
	bool precharge_relay;	// True if Closed

	float torque_command;	// Newton*meters
	float feedback_torque;	// Newton*meters

	// Current Motor Speed in RPM
	float motor_speed;			// RPM
	float motor_angle;			// Degrees
	float electrical_output_f;	// Hz
	float delta_resolver;		// Degrees

	// Current Information
	float phase_a_current;	// Arms
	float phase_b_current;	// Arms
	float phase_c_current;	// Arms
	float dc_bus_current;	// A

	// Voltage Information
	float dc_bus_voltage;	// V
	float output_voltage;	// Vpk
	float ab_voltage;		// V
	float bc_voltage;		// V

	// Temperature Information
	float module_a_temperature;
	float module_b_temperature;
	float module_c_temperature;
	float gate_drive_temperature;
	float control_board_temperature;

	float motor_temperature;

	// Fault Information
	bool fault_active;

	uint16_t post_faults_low;
	uint16_t post_faults_high;
	uint16_t run_faults_low;
	uint16_t run_faults_high;

	// Number of Ticks Since Program Start
	unsigned int tick_counter;
};

#endif /* InverterHR23_H_ */
