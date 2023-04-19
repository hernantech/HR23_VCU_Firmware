/*
******************************************************************************
	VCU1200 Main Loop
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

    Alex Hernandez, Highlander Racing
    4/6/23 ; version 3
    First iteration (blueprint w/stubs) of CAN I/O with predefined IDs

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "main.h"

extern Application application;
extern VCU1200_Board board;

// Main Program Entry
void main(void)
{
	// Set VTABLE Address to APP_BASE
	HWREG(NVIC_VTABLE) = APP_BASE;

	// Initialize Board System
	board.initializeSystem();

	// Initialize Application
	application.initialize();

    // Main System Loop
    while(true)
    {
    	// Clear wakeup bit since we're at start of loop
    	board.clearWakeup();

    	// Perform Board Level Processing
    	board.tick();

    	// Perform Application Level Processing
    	application.tick();

    	// Go to sleep until next tick
    	while (!board.getWakeup())
    	{
    		SysCtlSleep();
    	}
    }
}
