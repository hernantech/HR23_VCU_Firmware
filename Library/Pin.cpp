/*
******************************************************************************
	 TM4C1294 Pin Abstraction Implementation
	 BAIC 93S
	 Erik Stafl, Elijah Newton, Michael Dakin
	 8/19/2015

	 Concrete class for accessing and setting pin values on the TM4C1294 platform.

	 TM4C1294 implementation of cross-platform interface.
	 Language: C++
******************************************************************************
*/

#include "Pin.h"

Pin::Pin(unsigned int pin_number, unsigned int gpio_port, unsigned int gpio_pin, IOType io)
{
	pinNumber = pin_number;
	port = gpio_port;
	pin  = gpio_pin;
	type = io;
}

PinState Pin::getInput(unsigned int num_samples)
{
	unsigned int total = 0;
	for (int i = 0; i < num_samples; i++)
	{
		total += getInputPinValue(port, pin);
	}

	if (total > num_samples / 2)
	{
		return HIGH;
	}
	else
	{
		return LOW;
	}
}

void Pin::setOutput(PinState state)
{
	setStateOnPin(state, port, pin);
}
