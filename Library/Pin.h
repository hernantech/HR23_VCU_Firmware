/*
******************************************************************************
	 GPIO Pin Abstraction Header
	 BAIC 93S
	 Erik Stafl, Elijah Newton, Michael Dakin
	 8/19/2015

	 Abstract interface for accessing and setting pin values.

	 Written for cross-platform use.
	 Language: C++
******************************************************************************
*/

#ifndef CROSS_PLATFORM_PIN_H_
#define CROSS_PLATFORM_PIN_H_

// IO Type
enum IOType
{
	INPUT = 0,
	OUTPUT
};

// Input and Output States
enum PinState
{
	LOW = 0,
	HIGH
};

// Main Pin Class
class Pin
{
public:

	Pin(unsigned int pin_number, unsigned int gpio_port, unsigned int gpio_pin, IOType io);

	// Get Pin Input State
	PinState getInput(unsigned int num_samples = 1);

	// Set Output
	void setOutput(PinState state);

private:

	// The pinNumber of the Pin.
	unsigned int pinNumber;

	// The GPIO Port the Pin Belongs to (e.g. P5.7 is Port 5)
	unsigned int port;

	// The Specific Pin in the Port (e.g. P5.7 is Pin 7)
	unsigned int pin;

	// Input or Output
	IOType type;

protected:

	// Template methods implemented on a per-platform basis:
	virtual PinState getInputPinValue(unsigned int port, unsigned int pin)=0;           // Reads the current state of an input-pin.
	virtual void setStateOnPin(PinState state, unsigned int port, unsigned int pin)=0;  // Sets the current state of an output-pin.
};

#endif /* CROSS_PLATFORM_PIN_H_ */
