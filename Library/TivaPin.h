/*
******************************************************************************
	 Tiva Pin Abstraction Header
	 BAIC 93S
	 Erik Stafl, Elijah Newton, Michael Dakin
	 8/19/2015

	 Concrete class for accessing and setting pin values on the Tiva platform.

	 Tiva implementation of cross-platform interface.
	 Language: C++
******************************************************************************
*/

#ifndef TIVA_PIN_H_
#define TIVA_PIN_H_

#include <stdint.h>
#include "driverlib/gpio.h"

#include "Pin.h"

class TivaPin : public Pin
{
public:
	TivaPin(unsigned int pin_number, unsigned int gpio_port, unsigned int gpio_pin, IOType io);

protected:
	// Template Methods:
	virtual PinState getInputPinValue(unsigned int port, unsigned int pin);
	virtual void setStateOnPin(PinState state, unsigned int port, unsigned int pin);
};

#endif /* TIVA_PIN_H_ */
