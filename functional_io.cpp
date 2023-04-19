/*
******************************************************************************
	Functional IO Mapping Implementation
	eHarley Project
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "functional_io.h"

// Reference to Main Board Object in vcu.cpp
extern VCU1200_Board board;

// Input Polarity Settings
SignalPolarity input_polarities[NUM_INPUT_SIGNALS];

// Local Storage of Input Signal States (fdor Secondary VCU Inputs)
SignalState input_states[NUM_INPUT_SIGNALS];

// Local Storage of Output Signal States (for Secondary VCU Outputs)
SignalState output_states[NUM_OUTPUT_SIGNALS];

SignalState getInput(InputSignal input)
{
	//Trick Way to Utilize Thermistor inputs as a Digitial ON/OFF Singal
	/*switch (input)
	{
	case KEYSWITCH:
		return (board.getAnalogValue(VCU1200_Board::THERMISTOR_1) > 10000.0f ? ON : OFF);

	case YELLOW_BUTTON:
		return (board.getAnalogValue(VCU1200_Board::THERMISTOR_2) < 10000.0f ? ON : OFF);

	case BLUE_BUTTON:
		return (board.getAnalogValue(VCU1200_Board::THERMISTOR_3) < 10000.0f ? ON : OFF);

	case TURN_KNOB_1:
		return (board.getAnalogValue(VCU1200_Board::THERMISTOR_4) < 10000.0f ? ON : OFF);

	case TURN_KNOB_2:
		return (board.getAnalogValue(VCU1200_Board::THERMISTOR_5) < 10000.0f ? ON : OFF);
	}*/


	// Continue to Normal Digital Inputs

	if (input_polarities[input] == ACTIVE_HIGH)
	{
		// Invert Raw Pin State
		return (SignalState)(!board.getInput(mapInput(input)));
	}
	else
	{
		// Use Raw Pin State
		return (SignalState)(board.getInput(mapInput(input)));
	}
}

// Return the Raw Voltage of the Analog Input
float getAnalogInput(AnalogSignal input)
{
	switch (input)
	{
	//Setup the Potentiometer as an Analog Input and Read then return Value
	case ANALOG_SIGNAL_1:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_1);

	case ANALOG_SIGNAL_2:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_2);

	case ANALOG_SIGNAL_3:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_3);

	case ANALOG_SIGNAL_4:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_4);

	case ANALOG_SIGNAL_5:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_5);

	case ANALOG_SIGNAL_6:
		return board.getAnalogValue(VCU1200_Board::ANALOG_INPUT_6);

	case THERMISTOR_SIGNAL_1:
		return board.getAnalogValue(VCU1200_Board::THERMISTOR_1);

	case THERMISTOR_SIGNAL_2:
		return board.getAnalogValue(VCU1200_Board::THERMISTOR_2);

	case THERMISTOR_SIGNAL_3:
		return board.getAnalogValue(VCU1200_Board::THERMISTOR_3);

	case THERMISTOR_SIGNAL_4:
		return board.getAnalogValue(VCU1200_Board::THERMISTOR_4);

	case THERMISTOR_SIGNAL_5:
		return board.getAnalogValue(VCU1200_Board::THERMISTOR_5);

	default:
		// Program should never get here
		error("Unimplemented Input in getAnalogInput() call", "functional_io.cpp", __LINE__);
		return 0.0f;
	}
}

SignalState getOutput(OutputSignal output)
{
	// Get State of Corresponding Output Pin on Board
	return (SignalState)board.getOutput(mapOutput(output));
}

void initializeInputs()
{
	// Set Pins to to Pull Up Resistor, but still Active High

	board.setPinResistor(mapInput(BUTTON1), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON1,								ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON2), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON2, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON3), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON3, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON4), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON4, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON5), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON5, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON6), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON6, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON7), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON7, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON8), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON8, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON9), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON9, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON10), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON10, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON11), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON11, 						ACTIVE_HIGH);
	board.setPinResistor(mapInput(BUTTON12), 		VCU1200_Board::PULL_DOWN);
	setInputPolarity(BUTTON12, 						ACTIVE_HIGH);
}

VCU1200_Board::Input mapInput(InputSignal input)
{
	switch (input)
	{
	case BUTTON1:
		return VCU1200_Board::DIGITAL_INPUT_1;

	case BUTTON2:
		return VCU1200_Board::DIGITAL_INPUT_2;

	case BUTTON3:
		return VCU1200_Board::DIGITAL_INPUT_3;

	case BUTTON4:
		return VCU1200_Board::DIGITAL_INPUT_4;

	case BUTTON5:
		return VCU1200_Board::DIGITAL_INPUT_5;

	case BUTTON6:
		return VCU1200_Board::DIGITAL_INPUT_6;

	case BUTTON7:
		return VCU1200_Board::DIGITAL_INPUT_7;

	case BUTTON8:
		return VCU1200_Board::DIGITAL_INPUT_8;

	case BUTTON9:
		return VCU1200_Board::DIGITAL_INPUT_9;

	case BUTTON10:
		return VCU1200_Board::DIGITAL_INPUT_10;

	case BUTTON11:
		return VCU1200_Board::DIGITAL_INPUT_11;

	case BUTTON12:
		return VCU1200_Board::DIGITAL_INPUT_12;

	default:
		// Program should never get here
		error("Unimplemented Input Signal in mapInput() call", "functional_io.cpp", __LINE__);
		return VCU1200_Board::DIGITAL_INPUT_1;
	}
}

VCU1200_Board::Output mapOutput(OutputSignal output)
{
	switch (output)
	{
		case LED1:
			return VCU1200_Board::RELAY_DRIVER_1;

		case LED2:
			return VCU1200_Board::RELAY_DRIVER_2;

		case LED3:
			return VCU1200_Board::RELAY_DRIVER_3;

		case LED4:
			return VCU1200_Board::RELAY_DRIVER_4;

		case LED5:
			return VCU1200_Board::RELAY_DRIVER_5;

		case LED6:
			return VCU1200_Board::RELAY_DRIVER_6;

		case LED7:
			return VCU1200_Board::RELAY_DRIVER_7;

		case LED8:
			return VCU1200_Board::RELAY_DRIVER_8;

		case LED9:
			return VCU1200_Board::RELAY_DRIVER_9;

		case LED10:
			return VCU1200_Board::RELAY_DRIVER_10;

		case LED11:
			return VCU1200_Board::RELAY_DRIVER_11;

		case LED12:
			return VCU1200_Board::RELAY_DRIVER_12;

		case LED13:
			return VCU1200_Board::RELAY_DRIVER_13;

		case LED14:
			return VCU1200_Board::RELAY_DRIVER_14;

		case HS_LED1:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED2:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED3:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED4:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED5:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED6:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED7:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case HS_LED8:
			return VCU1200_Board::HIGH_SIDE_DRIVER_1;

		case ANALOGPWR_5V_1:
			return VCU1200_Board::ANALOG_5V_OUTPUT_1;

		case ANALOGPWR_5V_2:
			return VCU1200_Board::ANALOG_5V_OUTPUT_1;

		default:
			// Program should never get here
			error("Unimplemented Output Signal in mapOutput() call", "functional_io.cpp", __LINE__);
			return VCU1200_Board::RELAY_DRIVER_1;
	}
}

void setInputPolarity(InputSignal input, SignalPolarity polarity)
{
	input_polarities[input] = polarity;
}

void setOutput(OutputSignal output, SignalState out_state)
{
	// Set Corresponding Pin on Board
	board.setOutput(mapOutput(output), (PinState)out_state);
}

