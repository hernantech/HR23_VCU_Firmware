/*
******************************************************************************
	VCU1200 Main Loop
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

	Alex Hernandez
	4/5/2023; Version 2

	Built for Testing CAN on VCU1200 via CAN Bus 1
    Just sends a 6-byte (I think?) array of 1's with ID 0x201

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "main.h"
#include<stdio.h>
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

    	float pot_com = getAnalogInput(ANALOG_SIGNAL_6);
        //need to find way to debug and get the raw value, some form of print
        printf("%9.6f", pot_com);
    	// Perform Application Level Processing
    	application.tick();

    	// Go to sleep until next tick
    	while (!board.getWakeup())
    	{
    		SysCtlSleep();
    	}
    }
}
