/*
******************************************************************************
	Functional IO Mapping
	Lithos Energy Weld Center - Primary VCU
	Erik Stafl
	3/22/2017

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2017 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef VCU1200_TUTORIAL_1_BLINKINGLIGHTS_FUNCTIONAL_IO_H_
#define VCU1200_TUTORIAL_1_BLINKINGLIGHTS_FUNCTIONAL_IO_H_

#include "Devices/vcu1200_board.h"

// Signal States
enum SignalState
{
	OFF = 0,
	ON
};

// Signal Polarity
enum SignalPolarity
{
	ACTIVE_HIGH = 0,
	ACTIVE_LOW
};

// Functional Digital Inputs
enum InputSignal
{
	// Primary VCU Input Signals
	BUTTON1 = 0,
	BUTTON2,
	BUTTON3,
	BUTTON4,
	BUTTON5,
	BUTTON6,
	BUTTON7,
	BUTTON8,
	BUTTON9,
	BUTTON10,
	BUTTON11,
	BUTTON12,
	NUM_INPUT_SIGNALS
};

// Functional Analog Inputs
enum AnalogSignal
{
	ANALOG_SIGNAL_1 = 0,
	ANALOG_SIGNAL_2,
	ANALOG_SIGNAL_3,
	ANALOG_SIGNAL_4,
	ANALOG_SIGNAL_5,
	ANALOG_SIGNAL_6,
	THERMISTOR_SIGNAL_1,
	THERMISTOR_SIGNAL_2,
	THERMISTOR_SIGNAL_3,
	THERMISTOR_SIGNAL_4,
	THERMISTOR_SIGNAL_5,
	NUM_ANALOG_SIGNALS
};

// Functional Outputs
enum OutputSignal
{
	// Primary VCU Output Signals
	LED1 = 0,
	LED2,
	LED3,
	LED4,
	LED5,
	LED6,
	LED7,
	LED8,
	LED9,
	LED10,
	LED11,
	LED12,
	LED13,
	LED14,
	HS_LED1,
	HS_LED2,
	HS_LED3,
	HS_LED4,
	HS_LED5,
	HS_LED6,
	HS_LED7,
	HS_LED8,
	ANALOGPWR_5V_1,
	ANALOGPWR_5V_2,
	NUM_OUTPUT_SIGNALS
};

// Return the Raw Voltage of the Analog Input
float getAnalogInput(AnalogSignal input);

// Return the Current State of the Functional Input
SignalState getInput(InputSignal input);

// Return the Current State of the Functional Output
SignalState getOutput(OutputSignal output);

// Setup Pull-up / Pull-down Resistors on Inputs
void initializeInputs();

// Return the Corresponding Digital Input Pin for the Given Input Signal
VCU1200_Board::Input mapInput(InputSignal input);

// Return the Corresponding Output Pin for the Given Output Signal
VCU1200_Board::Output mapOutput(OutputSignal output);

// Set Logical Signal Polarity of an Input Pin (Active High or Active Low)
void setInputPolarity(InputSignal input, SignalPolarity polarity);

// Handles Turning On or Off Outputs
void setOutput(OutputSignal output, SignalState out_state);

#endif /* VCU1200_TUTORIAL_1_BLINKINGLIGHTS_FUNCTIONAL_IO_H_ */
