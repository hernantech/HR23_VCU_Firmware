/*
******************************************************************************
	BMS Software Library 2.00 Fault Manager Implementation
	Erik Stafl, Elijah Newton
	6/8/2015

	Manages Fault Conditions

	Written for All Platforms
	Language: C++
******************************************************************************
*/

#include "fault_manager.h"

FaultManager::FaultManager(bool clear_immediately)
{
	// Perform Object Reset
	reset();

	// Save Configuration Settings
	immediate_clear = clear_immediately;
}

void FaultManager::clearAll()
{
	for (int i = 0; i < MAX_FAULT_NUMBER; i++)
	{
		active_faults[i] 	= false;
		fault_conditions[i] = false;
		fault_counters[i]	= 0;

		warnings[i]			= false;
		warning_counters[i]	= 0;
	}

	fault_active = false;
}

void FaultManager::clearFault(Condition fault)
{
	active_faults[fault] 	= false;
	fault_conditions[fault] = false;
	fault_counters[fault]	= 0;
}

void FaultManager::clearWarning(Condition warning)
{
	warnings[warning]			= false;
	warning_counters[warning]	= 0;
}

bool FaultManager::isFaultActive()
{
	return fault_active;
}

bool FaultManager::isWarningActive()
{
	return warning_active;
}

bool FaultManager::getContactorOpen()
{
	return contactor_open;
}

bool FaultManager::getFault(Condition fault)
{
	return active_faults[fault];
}

bool FaultManager::getWarning(Condition warning)
{
	return warnings[warning];
}

void FaultManager::setFault(Condition fault, bool state)
{
	fault_conditions[fault] = state;

	if (state)
	{
		// Trigger Fault Immediately if this is not a Time-Delay Fault
		if (fault_counter_limits[fault] == 0)
		{
			active_faults[fault] = true;

			// Master Fault Active
			fault_active = true;
		}
	}
	else
	{
		// Clear Fault Immediately if this is not a Time-Delay Fault
		if (fault_counter_limits[fault] == 0)
		{
			active_faults[fault] = false;
		}
	}
}

void FaultManager::setFaultTime(Condition fault, unsigned int time_ms)
{
	fault_counter_limits[fault] = time_ms / (1000 / TICK_FREQUENCY);
}

void FaultManager::setWarning(Condition warning, bool state)
{
	warnings[warning] = state;

	// Master Warning Active
	warning_active = true;
}

void FaultManager::reset()
{
	// Clear All Flags
	clearAll();

	// Clear Counter Limits
	for (int i = 0; i < MAX_FAULT_NUMBER; i++)
	{
		fault_counter_limits[i] = 0;
	}

	// Set Contactor Opening Timer to 2 Seconds
	contactor_fault_timer = 200;
}

void FaultManager::tick()
{
	bool active_f = false;
	bool active_w = false;

	// Run Through Faults and Warnings
	for (int i = 0; i < MAX_FAULT_NUMBER; i++)
	{
		// Check for Active Faults
		if (fault_conditions[i])
		{
			if (fault_counters[i] >= fault_counter_limits[i])
			{
				// Fault is Now Active
				active_faults[i] = true;
			}
			else
			{
				// Increment Counter
				fault_counters[i]++;
			}
		}
		else
		{
			if (immediate_clear)
			{
				// Clear Fault Condition and Reset Counters
				active_faults[i] = false;
				fault_counters[i] = 0;
			}
			else
			{
				if (fault_counters[i] == 0)
				{
					// Clear Fault Condition
					active_faults[i] = false;
				}
				else
				{
					// Decrement Counter
					fault_counters[i]--;
				}
			}
		}

		// Update Master Active Flag
		if (active_faults[i])
		{
			active_f = true;
		}

		// Increment Warning Counters
		if (warnings[i])
		{
			// Prevent Timer Overflow
			if (warning_counters[i] < 60000)
			{
				warning_counters[i]++;
			}

			// Update Master Active Flag
			active_w = true;
		}
		else
		{
			if (immediate_clear)
			{
				// Clear Warning Counter
				warning_counters[i] = 0;
			}
			else if (warning_counters[i] > 0)
			{
				// Decrement Warning Counter
				warning_counters[i]--;
			}
		}
	}

	// Update Master Flags
	fault_active = active_f;
	warning_active = active_w;

	if (fault_active)
	{
		if (contactor_fault_timer > 0)
		{
			// Decrement Contactor Opening Timer
			contactor_fault_timer--;
		}
		else
		{
			contactor_open = true;
		}
	}
	else
	{
		// Reset Contactor Fault Timer
		contactor_fault_timer = 200;
	}

	// Increment Tick Counter
	tick_counter++;
}
