/*
*****************************************************************************
	Functions for Critical Code Sections and Protected Loops Implementation
	Erik Stafl
	10/16/13

	Written for Stellaris or Tiva Series MCUs
	Language: Embedded C++

	Copyright (c) 2013 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "critical.h"

// Static variable that keeps track of whether or not interrupts were enabled
static bool c_interrupts_enabled;

void enter_critical_section()
{
	c_interrupts_enabled = !IntMasterDisable();
}

void exit_critical_section()
{
	if (c_interrupts_enabled)
	{
		IntMasterEnable();
	}
}

bool timeout_loop(volatile unsigned char& reg, unsigned char bit_mask, bool condition, unsigned int cycle_count)
{
	unsigned int i = 0;

	if (condition)
	{
		while (i < cycle_count)
		{
			if (reg & bit_mask)
			{
				// Condition satisfied, return
				return true;
			}
			i++;
		}
	}
	else
	{
		while (i < cycle_count)
		{
			if (!(reg & bit_mask))
			{
				// Condition satisfied, return
				return true;
			}
			i++;
		}
	}

	// Timeout
	return false;
}
