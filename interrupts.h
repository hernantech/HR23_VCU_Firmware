/*
******************************************************************************
	Interrupt Handlers Header
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef VCU1200_BASE_1_INTERRUPTS_H_
#define VCU1200_BASE_1_INTERRUPTS_H_

#include <stdint.h>

#include "inc/hw_can.h"

#include "driverlib/adc.h"
#include "driverlib/can.h"

#include "Devices/vcu1200_board.h"

extern VCU1200_Board board;

// Handle System Tick Call for Regular Processing
void SysTickIntHandler();

// Handle Watchdog Expiration
void WatchdogIntHandler();

// Process New ADC0 Conversion Results
void ADC0IntHandler();

// Process New ADC1 Conversion Results
void ADC1IntHandler();

// Process CAN0 Message Event
void CAN0IntHandler();

// Process CAN1 Message Event
void CAN1IntHandler();

// Handle WTimer 1 Events
void WTimer1IntHandler();

#endif /* VCU1200_BASE_1_INTERRUPTS_H_ */
