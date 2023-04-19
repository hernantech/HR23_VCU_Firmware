/*
******************************************************************************
	VCU1200 Board Class Header
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef VCU1200_BOARD_H_
#define VCU1200_BOARD_H_

#include <stdint.h>
#include <math.h>

#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"

#include "driverlib/adc.h"
#include "driverlib/eeprom.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/watchdog.h"

#include "Library/can_receiver.h"
#include "Library/can_system.h"
#include "Library/error.h"
#include "Library/TivaPin.h"

// Main Board Class
class VCU1200_Board : public CANReceiver
{
public:
	VCU1200_Board();

	// Digital Inputs
	enum Input
	{
		DIGITAL_INPUT_1 = 0,
		DIGITAL_INPUT_2,
		DIGITAL_INPUT_3,
		DIGITAL_INPUT_4,
		DIGITAL_INPUT_5,
		DIGITAL_INPUT_6,
		DIGITAL_INPUT_7,
		DIGITAL_INPUT_8,
		DIGITAL_INPUT_9,
		DIGITAL_INPUT_10,
		DIGITAL_INPUT_11,
		DIGITAL_INPUT_12,
		POWER_GOOD,
		CARD_LOADED,
		CAN_1_TERM_SENSE,
		CAN_2_TERM_SENSE,
		NUM_DIGITAL_INPUTS
	};

	// Digital Outputs
	enum Output
	{
		RELAY_DRIVER_1 = 0,
		RELAY_DRIVER_2,
		RELAY_DRIVER_3,
		RELAY_DRIVER_4,
		RELAY_DRIVER_5,
		RELAY_DRIVER_6,
		RELAY_DRIVER_7,
		RELAY_DRIVER_8,
		RELAY_DRIVER_9,
		RELAY_DRIVER_10,
		RELAY_DRIVER_11,
		RELAY_DRIVER_12,
		RELAY_DRIVER_13,
		RELAY_DRIVER_14,
		HIGH_SIDE_DRIVER_1,
		HIGH_SIDE_DRIVER_2,
		HIGH_SIDE_DRIVER_3,
		HIGH_SIDE_DRIVER_4,
		HIGH_SIDE_DRIVER_5,
		HIGH_SIDE_DRIVER_6,
		HIGH_SIDE_DRIVER_7,
		HIGH_SIDE_DRIVER_8,
		ANALOG_5V_OUTPUT_1,
		ANALOG_5V_OUTPUT_2,
		STATUS_LED_1,
		STATUS_LED_2,
		CAN_1_TERM_SET,
		CAN_1_TERM_RESET,
		CAN_2_TERM_SET,
		CAN_2_TERM_RESET,
		WRITE_PROTECT,
		NUM_DIGITAL_OUTPUTS
	};

	// Analog Input Pins
	enum AnalogInput
	{
		ANALOG_INPUT_1 = 0,
		ANALOG_INPUT_2,
		ANALOG_INPUT_3,
		ANALOG_INPUT_4,
		ANALOG_INPUT_5,
		ANALOG_INPUT_6,
		THERMISTOR_1,
		THERMISTOR_2,
		THERMISTOR_3,
		THERMISTOR_4,
		THERMISTOR_5,
		REVISION_SENSE,
		BOARD_TEMP,
		SENSE_12V,
		SENSE_5V,
		NUM_ANALOG_INPUTS
	};

	// ADC Modules
	enum ADCModule
	{
		ADC_0 = 0,
		ADC_1
	};

	// CAN Message Requests
	enum CANRequests
	{
		CAN_FIRMWARE_VERSION = 0,
		CAN_DEVICE_RESET,
		NUM_CAN_REQUESTS
	};

	// Pin Pullup Settings
	enum PinSetting
	{
		PULL_DOWN = 0,
		PULL_UP,
		OPEN_PIN
	};

	// Blinks the Status LEDs
	void blinkLED();

	// Calculate Raw 0 - 3.3V value for a given ADC result
	float calcRawVoltage(unsigned int adc_value);

	// Clears the Wakeup Bit
	void clearWakeup();

	// Clears the Watchdog Timer (Pets the dog)
	void clearWatchdog();

	// Get the Analog Value of a Given Input
	float getAnalogValue(AnalogInput input);

	// Get Clock Cycles Between Last Pulses
	int getEncoderCycles(Input input);

	// Return Cached Value of Input Pin
	PinState getInput(Input in_pin);

	// Return Cached Value of Output Pin
	PinState getOutput(Output out_pin);

	// Calculate the Thermistor Resistance from a Measured Voltage
	float getResistance(float voltage, float reference = THERMISTOR_REFERENCE);

	// Return the Startup Counter Value
	unsigned int getStartupCounter();

	// Calculate the Temperature from a Thermistor Voltage and Reference Voltage
	float getTemperature(float voltage, float b_value = THERMISTOR_B_VALUE, float resistance_25c = THERMISTOR_RESISTANCE_25C, float reference = ADC_REF);

	// Return the wakeup bit
	bool getWakeup();

	// Return true if new encoder pulse has arrived since last check
	bool isNewEncoderPulse(Input input, bool clear);

	// Initialize the Board Devices and Subsystems
	void initializeSystem();

	// Read All Pins and Update State
	void readAllPins(unsigned int samples = NUM_DIGITAL_INPUT_SAMPLES);

	// Return the Current Measured State of the Given Input Pin
	PinState readInput(Input in_pin, unsigned int samples = NUM_DIGITAL_INPUT_SAMPLES);

	// Return the Current Measured State of the Given Output Pin
	PinState readOutput(Output out_pin, unsigned int samples = NUM_DIGITAL_INPUT_SAMPLES);

	// Read Isolated ADC
	void readIsolatedADC();

	// Process Board Level CAN Message - Return True if Message has been Processed
	bool receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox);

	// Process Encoder Pulse - Return True if Pulse has been Processed
	bool receiveEncoderPulse(Input input, unsigned int pulse_cycles);

	// Resets all board data
	void reset();

	// Set LED Blink Rate
	void setBlinkRate(unsigned int rate);

	// Set Whether it is Safe for Board to be Reset by CAN Command
	void setBootloaderSafe(bool enable);

	// Set Whether CAN Termination Resistor is Active
	void setCANTermination(CANPort can_port, bool active, bool override = false);

	// Set Whether Pin has Pullup or Pulldown Resistor
	void setPinResistor(Input input, PinSetting pin_setting);

	// Handles Turning On or Off Outputs
	void setOutput(Output out_pin, PinState out_state, bool override = false);

	// Set Digital Output Override Setting
	void setOutputOverride(bool override);

	// Set Whether it is Safe for Board to Enter into Output Override Mode
	void setOutputOverrideSafe(bool enable);

	// Set Wakeup Bit to True
	void setWakeup();

	// Called at the top of each Main Loop.  Handles board related functions.
	void tick();

	// Process Timer Overflow Interrupt
	void timerOverflow(Input input);

	// Save New Analog Readings
	void updateADCValues(ADCModule adc);

private:
	// Manage Board Level Counters
	void incrementCounters();

	// Initialize ADC Module
	void initializeADC();

	// Initialize Board-level CAN Messages for Reception
	void initializeCANReceive();

	// Initialize System Clock
	void initializeClock();

	// Initialize On Chip EEPROM
	void initializeEEPROM();

	// Set Pin Directions and Functions
	void initializeIO();

	// Initialize SSI Modules for SPI function
	void initializeSPI();

	// Initialize Timers for Edge Capture
	void initializeTimers();

	// Initialize Watchdog Timer
	void initializeWatchdog();

	// Loads calibration values from configuration header file
	void loadCalibration();

	// Determine Whether or not to Jump to Bootloader
	void manageBootloaderJump();

	// Respond to Board Level Requests
	void manageCANResponses();

	// Process CAN Termination Relay Actions
	void manageCANTerminations();

	// Read Pin State Number of Times and Return Average for Noise Immunity
	PinState readPinAvg(uint32_t port, uint8_t pin, unsigned int samples = NUM_DIGITAL_INPUT_SAMPLES);

	// Send Board-level CAN Data
	void sendCANData();

	// Trigger ADC Conversion
	void triggerADC();

	// Calculate Real Inputs Values from Raw ADC Voltages
	void updateAnalogValues();

public:
	// CAN System
	CANSystem can;

public:
	// Raw ADC results
	uint32_t ADC0_values[8];
	uint32_t ADC1_values[8];

	// True if New Data is Present for Processing
	bool ADC0_newdata;
	bool ADC1_newdata;

	// Board Temperature Sensor Calibrations
	float board_temp_offset;
	float board_temp_gain;

	// Temperature Data
	float mcu_temp;				// DegC

private:
	// VCU CAN Offset
	unsigned int can_offset;

	// Cached Digital Inputs
	PinState digital_inputs[NUM_DIGITAL_INPUTS];

	// Cached Digital Outputs
	PinState digital_outputs[NUM_DIGITAL_OUTPUTS];

	// Digital Output Override Values
	PinState digital_outputs_override[NUM_DIGITAL_OUTPUTS];

	// Raw ADC Inputs
	float raw_analog_inputs[NUM_ANALOG_INPUTS];	// V at MCU Pin

	// Analog Values in Real Units
	float analog_values[NUM_ANALOG_INPUTS];		// V

	// Clock Cycles between last two Encoder Pulses
	int encoder_cycles[4];
	int last_encoder_pulse[4];

	// True if new encoder pulse has arrived since last tick
	bool new_encoder_pulse[4];

	// Number of Encoder Overflows since Last Pulse
	unsigned int encoder_overflows[4];

	// True if CAN Termination is Active
	bool can_termination[2];

	// Application Settings for CAN Termination Resistors
	bool can_termination_setting[2];

	// CAN Termination Override Values
	bool can_termination_override[2];

	// Number of Ticks until Turning Off CAN Termination Relay Set or Reset Signals
	unsigned int can_termination_counter;

	// True if SysTick Timer has Triggered
	bool wakeup;

	// True if it is Safe to Reset Board to Enter Bootloader Mode
	bool bootloader_safe;

	// Device Reset will Occur on Next Tick
	bool reset_flag;

	// True if it is Safe to Change Board to Output Override Mode
	bool output_override_safe;

	// True if Output Override is Active
	bool output_override;

	// True if I/O Debug Messages are Enabled
	bool send_io_debug_messages;

	// True if a Message
	bool can_requests[NUM_CAN_REQUESTS];

	// How Many Ticks the Program has been Running
	unsigned int startup_counter;

	// The Board Address as set by the DIP Switch
	unsigned char address;

	// ADC Calibrations
	float adc_offset;
	float adc_gain;
	float adc_ref;

	// Blink Period in ticks
	unsigned int blink_rate;
};

#endif /* VCU1200_BOARD_H_ */
