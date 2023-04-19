/*
******************************************************************************
	Functions for Critical Code Sections and Protected Loops
	Generic Board Library
	Erik Stafl
	12/13/2015

	Written for Tiva Series MCUs
	Language: C++

	Copyright (c) 2013-2015 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef CRITICAL_H_
#define CRITICAL_H_

#include <stdint.h>

#include "driverlib/interrupt.h"

// Disable interrupts to prevent code from being disrupted
void enter_critical_section();

// Reenable interrupts to resume normal program flow
void exit_critical_section();

/*
	Loop until either condition exists or cycle count is exceeded
	Return true if condition satisfied, return false if timeout
*/
bool timeout_loop(volatile unsigned char& reg, unsigned char bit_mask, bool condition, unsigned int cycle_count = 1000);

#endif /* CRITICAL_H_ */
