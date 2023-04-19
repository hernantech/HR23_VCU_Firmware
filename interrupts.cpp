/*
******************************************************************************
	Interrupt Handlers
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "interrupts.h"

void SysTickIntHandler()
{
	board.setWakeup();
}

void WatchdogIntHandler()
{

}

void ADC0IntHandler()
{
	// Clear the interrupt flag
	ADCIntClear(ADC0_BASE, 0);

	// Update measured values from ADC0
	ADCSequenceDataGet(ADC0_BASE, 0, board.ADC0_values);

	// Flag New Data for Processing ADC Values
	board.updateADCValues(VCU1200_Board::ADC_0);
}

void ADC1IntHandler()
{
	// Clear the interrupt flag
	ADCIntClear(ADC1_BASE, 0);

	// Update measured values from ADC1
	ADCSequenceDataGet(ADC1_BASE, 0, board.ADC1_values);

	// Flag New Data for Processing ADC Values
	board.updateADCValues(VCU1200_Board::ADC_1);
}

void CAN0IntHandler()
{
	unsigned long status;
	status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

	if (status == CAN_INT_INTID_STATUS)
	{
		//
		// Read the controller status.  This will return a field of status
		// error bits that can indicate various errors.  Error processing
		// is not done in this example for simplicity.  Refer to the
		// API documentation for details about the error status bits.
		// The act of reading this status will clear the interrupt.  If the
		// CAN peripheral is not connected to a CAN bus with other CAN devices
		// present, then errors will occur and will be indicated in the
		// controller status.
		//
		status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
		if (status & CAN_STATUS_BUS_OFF || status & CAN_STATUS_EPASS)
		{
			// Request Re-initialization of CAN Bus
			CANEnable(CAN0_BASE);
		}
		return;
	}

	// Receive Messages
	board.can.can_1.can_rx_message.pui8MsgData = board.can.can_1.can_rx_data;

	if (status >= 17 && status <= 32)
	{
		// Receive Messages
		board.can.can_1.can_rx_message.pui8MsgData = board.can.can_1.can_rx_data;

		// Retrieve Message Data for Valid CAN Messages
		CANMessageGet(CAN0_BASE, status, &board.can.can_1.can_rx_message, 0);

		// Process Received Message
		CANReceiver* receiver = board.can.can_1.getHandler(status);
		if (receiver != 0)
		{
			board.can.can_1.getHandler(status)->receiveCANMessage(CAN_1, &board.can.can_1.can_rx_message, status);
		}
	}

	if (status != 0 && status <= 32)
	{
		// Clear the interrupt
		CANIntClear(CAN0_BASE, status);
	}
}

void CAN1IntHandler()
{
	unsigned long status;
	status = CANIntStatus(CAN1_BASE, CAN_INT_STS_CAUSE);

	if (status == CAN_INT_INTID_STATUS)
	{
		//
		// Read the controller status.  This will return a field of status
		// error bits that can indicate various errors.  Error processing
		// is not done in this example for simplicity.  Refer to the
		// API documentation for details about the error status bits.
		// The act of reading this status will clear the interrupt.  If the
		// CAN peripheral is not connected to a CAN bus with other CAN devices
		// present, then errors will occur and will be indicated in the
		// controller status.
		//
		status = CANStatusGet(CAN1_BASE, CAN_STS_CONTROL);
		if (status & CAN_STATUS_BUS_OFF || status & CAN_STATUS_EPASS)
		{
			// Request Re-initialization of CAN Bus
			CANEnable(CAN1_BASE);
		}
		return;
	}

	// Receive Messages
	board.can.can_2.can_rx_message.pui8MsgData = board.can.can_2.can_rx_data;

	if (status >= 17 && status <= 32)
	{
		// Receive Messages
		board.can.can_2.can_rx_message.pui8MsgData = board.can.can_2.can_rx_data;

		// Retrieve Message Data for Valid CAN Messages
		CANMessageGet(CAN1_BASE, status, &board.can.can_2.can_rx_message, 0);

		// Process Received Message
		CANReceiver* receiver = board.can.can_2.getHandler(status);
		if (receiver != 0)
		{
			board.can.can_2.getHandler(status)->receiveCANMessage(CAN_2, &board.can.can_2.can_rx_message, status);
		}
	}

	if (status != 0 && status <= 32)
	{
		// Clear the interrupt
		CANIntClear(CAN1_BASE, status);
	}
}

void WTimer1IntHandler()
{
	// Digital Input 12
	if (TimerIntStatus(WTIMER1_BASE, true) & TIMER_CAPB_EVENT)
	{
		// Get Value of Timer B
		if (board.readInput(VCU1200_Board::DIGITAL_INPUT_12, 3))
		{
			board.receiveEncoderPulse(VCU1200_Board::DIGITAL_INPUT_12, TimerValueGet(WTIMER1_BASE, TIMER_B));
		}

		// Clear Interrupt
		TimerIntClear(WTIMER1_BASE, TIMER_CAPB_EVENT);
	}
	if (TimerIntStatus(WTIMER1_BASE, true) & TIMER_TIMA_TIMEOUT)
	{
		// Process Timer Overflow
		board.timerOverflow(VCU1200_Board::DIGITAL_INPUT_12);

		// Clear Interrupt
		TimerIntClear(WTIMER1_BASE, TIMER_TIMA_TIMEOUT);
	}
}

