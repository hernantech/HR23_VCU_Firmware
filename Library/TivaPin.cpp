/*
******************************************************************************
	 Tiva Pin Abstraction Implementation
	 BAIC 93S
	 Erik Stafl, Elijah Newton, Michael Dakin
	 8/19/2015

	 Concrete class for accessing and setting pin values on the Tiva platform.

	 TM4C1294 implementation of cross-platform interface.
	 Language: C++
******************************************************************************
*/

#include "TivaPin.h"

TivaPin::TivaPin(unsigned int pin_number, unsigned int gpio_port, unsigned int gpio_pin, IOType io) : Pin(pin_number,gpio_port,gpio_pin,io)
{
}

PinState TivaPin::getInputPinValue(unsigned int port, unsigned int pin)
{
	return (GPIOPinRead(port, pin) != 0) ? HIGH : LOW;
}

void TivaPin::setStateOnPin(PinState state, unsigned int port, unsigned int pin)
{
	if (state == HIGH)
	{
		GPIOPinWrite(port, pin, pin);
	}
	else
	{
		GPIOPinWrite(port, pin, 0x00);
	}
}
