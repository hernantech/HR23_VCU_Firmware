/*
******************************************************************************
	VCU1200 Configuration Values
	VCU1200 Master BMS Board Firmware
	Erik Stafl
	1/24/2016

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef VCU1200_BASE_1_CONFIGURATION_H_
#define VCU1200_BASE_1_CONFIGURATION_H_

////////////////////////////////////////////////////////////////
// Software Version
#define VERSION_MAJOR		0
#define VERSION_MINOR		7
#define VERSION_REVISION	1

////////////////////////////////////////////////////////////////
// Board Level Configuration

// Base Address of Application
#define APP_BASE 						0x00005000
#define CHECK_BOARD_CODE				0xABCD1200	// VCU1200 ID Code

// VCU Tick Frequency
#define TICK_FREQUENCY					100		// Ticks per Second

// Message Offset ID
#define VCU_CAN_OFFSET					0x200	// Base Message ID

// Define to Use CAN1 in addition to CAN0
#define USE_CAN1_INTERFACE

// Board Temperature Sensor Calibration Constants
#define BOARD_TEMP_OFFSET				0.500f	// Volts
#define BOARD_TEMP_GAIN					0.010f	// Volts / Deg C

// On-Chip ADC12 Calibration Constants
#define ADC_OFFSET						0.000f	// ADC Counts
#define ADC_GAIN						1.000f	// Unitless
#define ADC_REF							3.000f	// Volts

// MCU On-Chip Temperature Sensor Calibration Constants
#define MCU_TEMP_OFFSET					1.633f	// Volts
#define MCU_TEMP_GAIN				-0.013300f	// Volts / Deg C

// Digital Inputs
#define NUM_DIGITAL_INPUT_SAMPLES		5		// How many samples to average for each digital input reading

// Thermistors
#define THERMISTOR_REFERENCE			3.000f	// Volts
#define THERMISTOR_RESISTANCE_25C		10000.0f// Ohms
#define THERMISTOR_B_VALUE				3380.0f	// B Value

// CAN Baud Rate Settings
#define CAN_1_BAUD_RATE					CAN_500_KBPS
#define CAN_2_BAUD_RATE					CAN_500_KBPS

// CAN Termination Resistor Startup Settings
#define CAN_1_TERMINATION				true
#define CAN_2_TERMINATION				true

// Send IO Debug Messages by Default
#define SEND_IO_DEBUG_MESSAGES			true

////////////////////////////////////////////////////////////////
// Vehicle Level Configuration

// Define Which Inverters to Use
#define USE_INVERTER_1					true
#define USE_INVERTER_2					true

// Vehicle Stopped Threshold
#define VEHICLE_STOPPED_RPM				50.0f	// Motor RPM

// Minimum State Time
#define MIN_STATE_TIME					100		// Ticks

// Precharge Complete Threshold
#define PRECHARGE_THRESHOLD				0.95f	// Ratio
#define PRECHARGE_TIMEOUT_FAULT			400		// Ticks

// Accelerator Settings
#define ACCELERATOR_LOW_V_FAULT			0.250f	// V
#define ACCELERATOR_MIN_V				0.825f	// V
#define ACCELERATOR_MAX_V				3.500f	// V
#define ACCELERATOR_HIGH_V_FAULT		4.000f	// V
#define VOLTAGE_HYSTERESIS				0.050f	// V

#define COAST_LOW_PERCENT				5.0f	// %
#define COAST_HIGH_PERCENT				10.0f	// %

// Torque Settings
#define MAX_TORQUE_COMMAND				250.0f	// Nm - Per Inverter
#define OFF_PEDAL_REGEN_TORQUE			50.0f	// Nm
#define REGEN_FADE_SPEED				500.0f	// Motor RPM

////////////////////////////////////////////////////////////////
// Battery Level Configuration
#define EXPECTED_MONITOR_BOARDS			8
#define MONITOR_BOARD_BASE_ADDRESS		0

#endif /* VCU1200_BASE_1_CONFIGURATION_H_ */
