/*
******************************************************************************
	Fault Manager Header
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/24/2016

	Fault Manager Class Encapsulates Fault Tracking

	Written for All Platforms
	Language: Embedded C++
******************************************************************************
*/

#ifndef FAULT_MANAGER_H_
#define FAULT_MANAGER_H_

#include "configuration.h"
#include "fault_conditions.h"

// Fault Manager Class
class FaultManager
{
public:
	// Default Constructor with Fault Check Function
	FaultManager(bool clear_immediately);

	// Clear All Fault and Warning Flags
	void clearAll();

	// Clear a Given Fault Condition
	void clearFault(Condition fault);

	// Clear a Given Warning Condition
	void clearWarning(Condition warning);

	// Return True if a Fault Condition is Active
	bool isFaultActive();

	// Return True if a Warning Condition is Active
	bool isWarningActive();

	// Return True if Contactor Must Open for Protection
	bool getContactorOpen();

	// Return True if Fault Flag is Set
	bool getFault(Condition fault);

	// Return True if Warning Flag is Set
	bool getWarning(Condition warning);

	// Update Status of a Given Fault Condition - Does Not Set Fault Active if Time Delay is Configured
	void setFault(Condition fault, bool state);

	// Set the Length of Time a Fault Condition must be Present before a Fault is Triggered
	void setFaultTime(Condition fault, unsigned int time_ms);

	// Update Status of a Given Warning Condition
	void setWarning(Condition warning, bool state);

	// Reset Fault Manager
	void reset();

	// Perform Regular Processing
	void tick();

private:
	// True if Contactor Must Open
	bool contactor_open;

	// Whether to Clear Faults Immediately
	bool immediate_clear;

	// Master Fault Flag
	bool fault_active;

	// Master Warning Flag
	bool warning_active;

	// Fault Flags
	bool active_faults[MAX_FAULT_NUMBER];					// Fault has Persisted for Minimum Period of Time
	bool fault_conditions[MAX_FAULT_NUMBER];				// Condition is Active
	unsigned int fault_counters[MAX_FAULT_NUMBER];			// How Long a Condition has Been Active
	unsigned int fault_counter_limits[MAX_FAULT_NUMBER];	// Fault Windowing Time

	// Warning Flags
	bool warnings[MAX_FAULT_NUMBER];
	unsigned int warning_counters[MAX_FAULT_NUMBER];		// How Long a Condition has Been Active

	// Master Tick Counter
	unsigned int tick_counter;

	// Contactor Open Countdown Timer
	unsigned int contactor_fault_timer;
};

#endif /* FAULT_MANAGER_H_ */
