/*
******************************************************************************
	VCU1200 Board Class Implementation
	VCU1200 Vehicle Control Unit Firmware
	Erik Stafl
	1/23/2015

	Written for Tiva TM4C123BH6PGE
	Language: C++

	Copyright (c) 2013-2016 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#include "vcu1200_board.h"

// VCU1200 Board Object
VCU1200_Board board;

VCU1200_Board::VCU1200_Board()
{
	// Default to Allowing Bootloader Access
	setBootloaderSafe(true);

	// Default to Allowing Output Override
	setOutputOverrideSafe(true);
}

void VCU1200_Board::blinkLED()
{
	if (startup_counter % blink_rate == 0)
	{
		// Turn On LED
		setOutput(STATUS_LED_1, HIGH);
	}
	else if (startup_counter % blink_rate == blink_rate / 2)
	{
		// Turn Off LED
		setOutput(STATUS_LED_1, LOW);
	}
}

float VCU1200_Board::calcRawVoltage(unsigned int adc_value)
{
	float returnval = ((float)adc_value - adc_offset) / 4095.0f * adc_ref / adc_gain;

	// Don't allow negative voltages
	if (returnval < 0.0f)
		returnval = 0.0f;

	return returnval;
}

void VCU1200_Board::clearWakeup()
{
	wakeup = false;
}

void VCU1200_Board::clearWatchdog()
{
	WatchdogIntClear(WATCHDOG0_BASE);
}

float VCU1200_Board::getAnalogValue(AnalogInput input)
{
	return analog_values[input];
}

int VCU1200_Board::getEncoderCycles(Input input)
{
	switch (input)
	{
	case DIGITAL_INPUT_9:
		return encoder_cycles[0];

	case DIGITAL_INPUT_10:
		return encoder_cycles[1];

	case DIGITAL_INPUT_11:
		return encoder_cycles[2];

	case DIGITAL_INPUT_12:
		return encoder_cycles[3];

	default:
		return 0;
	}
}

PinState VCU1200_Board::getInput(Input in_pin)
{
	return digital_inputs[in_pin];
}

PinState VCU1200_Board::getOutput(Output out_pin)
{
	return digital_outputs[out_pin];
}

float VCU1200_Board::getResistance(float voltage, float reference)
{
	float resistance;

	// Check for Open and Short Circuits
	if (voltage >= reference - 0.010f)
	{
		// Open Circuit - Set to 5 MOhm
		resistance = 5000000.0f;
	}
	else if (voltage <= 0.010f)
	{
		// Short to Ground
		resistance = 0.0f;
	}
	else
	{
		// Calculate Value from Resistor Divider
		resistance = 16000.0f / (reference / voltage - 1.000f);
	}

	return resistance;
}

unsigned int VCU1200_Board::getStartupCounter()
{
	return startup_counter;
}

float VCU1200_Board::getTemperature(float voltage, float b_value, float resistance_25c, float reference)
{
	float resistance;
	float temperature;

	// Check for Open or Shorted Thermistors
	if (voltage > reference - 0.100f)
	{
		// Open Thermistor
		temperature = -50.0f;
		return temperature;
	}
	else if (voltage < 0.100f)
	{
		// Shorted Thermistor
		temperature = 150.0f;
		return temperature;
	}

	// Calculate Thermistor Resistance
	resistance = getResistance(voltage, reference);

	// Calculate Temperature
	temperature = (25.0f + 273.15f) * b_value
			/ logf(resistance_25c / resistance)
			/ (b_value / logf(resistance_25c / resistance) - (25.0f + 273.15f));

	return temperature - 273.15f;
}

bool VCU1200_Board::getWakeup()
{
	return wakeup;
}

bool VCU1200_Board::isNewEncoderPulse(Input input, bool clear)
{
	bool return_value;

	switch (input)
	{
	case DIGITAL_INPUT_12:
		return_value = new_encoder_pulse[3];
		if (clear)
		{
			new_encoder_pulse[3] = false;
		}
		return return_value;

	default:
		return false;
	}
}

void VCU1200_Board::incrementCounters()
{
	startup_counter++;
}

void VCU1200_Board::initializeADC()
{
	//////////////////////////////////////////////////////
	// Setup ADC Module 0

	// Enable ADC0 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	// Wait for ADC0 Peripheral to become available
	SysCtlDelay(5);

	// Set ADC Hardware Oversampling to 16x
	ADCHardwareOversampleConfigure(ADC0_BASE, 16);

	// Set ADC0 Phase Delay to 0 Degrees
	ADCPhaseDelaySet(ADC0_BASE, ADC_PHASE_0);

	// Set ADC0 Reference to External
	ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);

	// Setup ADC Sequence 0 on ADC0
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

	// AIN0 (Revision Sense) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);

	// AIN1 (Analog Input 1) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);

	// AIN2 (Analog Input 2) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2);

	// AIN3 (Analog Input 3) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3);

	// AIN4 (Analog Input 4) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH4);

	// AIN5 (Analog Input 5) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH5);

	// AIN6 (Analog Input 6) Configuration
	ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);

	// Enable Sequence 0 on ADC0
	ADCSequenceEnable(ADC0_BASE, 0);

	// Clear Overflow and Underflow
	ADCSequenceOverflowClear(ADC0_BASE, 0);
	ADCSequenceUnderflowClear(ADC0_BASE, 0);

	// Enable interrupts
	ADCIntEnable(ADC0_BASE, 0);
	IntEnable(INT_ADC0SS0);

	//////////////////////////////////////////////////////
	// Setup ADC Module 1

	// Enable ADC0 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

	// Wait for ADC0 Peripheral to become available
	SysCtlDelay(5);

	// Set ADC Hardware Oversampling to 16x
	ADCHardwareOversampleConfigure(ADC1_BASE, 16);

	// Set ADC1 Phase Delay to 0 Degrees
	ADCPhaseDelaySet(ADC1_BASE, ADC_PHASE_0);

	// Set ADC1 Reference to External
	ADCReferenceSet(ADC1_BASE, ADC_REF_EXT_3V);

	// Setup ADC Sequence 0 on ADC1
	ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

	// AIN7 (Temp Input 1) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH7);

	// AIN8 (Temp Input 2) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH8);

	// AIN9 (Temp Input 3) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH9);

	// AIN10 (Temp Input 4) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH10);

	// AIN11 (Temp Input 5) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH11);

	// AIN12 (Board Temp) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH12);

	// AIN13 (12V Sense) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH13);

	// AIN14 (5V Sense) Configuration
	ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH14 | ADC_CTL_IE | ADC_CTL_END);

	// Enable Sequence 0 on ADC0
	ADCSequenceEnable(ADC1_BASE, 0);

	// Clear Overflow and Underflow
	ADCSequenceOverflowClear(ADC1_BASE, 0);
	ADCSequenceUnderflowClear(ADC1_BASE, 0);

	// Enable interrupts
	ADCIntEnable(ADC1_BASE, 0);
	IntEnable(INT_ADC1SS0);
}

void VCU1200_Board::initializeCANReceive()
{
	// Temporary Mailbox for Setup
	tCANMsgObject can_rx_message;

	// Setup Receive Messages on CAN 1

	// VCU IO Debug Message (CAN Offset + 0x0C0)
	can_rx_message.ui32MsgID = can_offset + 0x0C0;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can.initializeReceiveMessage(CAN_1, this, &can_rx_message);

	// VCU Output Override Message (CAN Offset + 0x0C1)
	can_rx_message.ui32MsgID = can_offset + 0x0C1;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can.initializeReceiveMessage(CAN_1, this, &can_rx_message);

	// Firmware Version Request Message (CAN Offset + 0x0F0)
	can_rx_message.ui32MsgID = can_offset + 0x0F0;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can.initializeReceiveMessage(CAN_1, this, &can_rx_message);

	// Jump to Bootloader Request Message (CAN Offset + 0x0F2)
	can_rx_message.ui32MsgID = can_offset + 0x0F2;
	can_rx_message.ui32MsgIDMask = 0x7FF;
	can_rx_message.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	can.initializeReceiveMessage(CAN_1, this, &can_rx_message);
}

void VCU1200_Board::initializeClock()
{
	// Set the clocking to run from the PLL.  System clock set to 40 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ);

	// Setup SysTick System
	// Set to number of ticks per second
	SysTickPeriodSet(SysCtlClockGet() / TICK_FREQUENCY);

	// Enable SysTick Interrupts
	SysTickIntEnable();

	// Enable SysTick Counter
	SysTickEnable();
}

void VCU1200_Board::initializeEEPROM()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	SysCtlDelay(5);

	uint32_t ret = EEPROM_INIT_ERROR;
	unsigned int try_count = 0;

	// Loop Until Success
	do
	{
		ret = EEPROMInit();
		try_count++;

		if (ret != EEPROM_INIT_OK)
		{
			SysCtlDelay(100000000);
		}

	} while (ret != EEPROM_INIT_OK && try_count < 3);
}

void VCU1200_Board::initializeIO()
{
	//////////////////////////////////////////////////////
	// Enable all GPIO banks
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

	//////////////////////////////////////////////////////
	// Setup GPIO Port A pins
	GPIOPinConfigure(GPIO_PA0_CAN1RX);					// CAN1 RX
	GPIOPinConfigure(GPIO_PA1_CAN1TX);					// CAN1 TX
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);					// SPI0 CLK
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);	// OUT: Software Controlled Flash Memory CS
	GPIOPinConfigure(GPIO_PA4_SSI0RX);					// SPI0 MISO
	GPIOPinConfigure(GPIO_PA5_SSI0TX);					// SPI0 MOSI
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6);	// OUT: CAN1 Standby
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);	// OUT: CAN2 Standby
	// Set Flash Memory CS to High, CAN1 and CAN2 Standby to Low
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_3);
	// Enable Alternate Functions
	GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5);

	//////////////////////////////////////////////////////
	// Setup GPIO Port B pins
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);	// IN: MicroSD Card Loaded
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);	// IN: Not Used
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);					// I2C0 SCL
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);					// I2C0 SDA
	GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);		// Analog Input 10 (T_Input 4)
	GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);		// Analog Input 11 (T_Input 5)
	// Enable Alternate Functions
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	// Enable Weak Pull Down Resistors on Pin 1
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	// Enable Weak Pull Up Resistor on Pin 0
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port C pins
	// PC0 - JTAG (TCK)
	// PC1 - JTAG (TMS)
	// PC2 - JTAG (TDI)
	// PC3 - JTAG (TDO)
	//GPIOPinConfigure(GPIO_PC0_TCK);
	//GPIOPinConfigure(GPIO_PC1_TMS);
	//GPIOPinConfigure(GPIO_PC2_TDI);
	//GPIOPinConfigure(GPIO_PC3_TDO);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_5);	// IN: Not Used
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);	// OUT: 5V Analog Power Enable 1
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);	// OUT: 5V Analog Power Enable 2
	// Set 5V Analog Power Outputs to Low
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0x00);
	// Enable Weak Pull Down Resistors on Pins 4 and 5
	GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

	//////////////////////////////////////////////////////
	// Setup GPIO Port D pins
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0);	// IN: Digital Input 1
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);		// Analog Input 14 (5V Sense)
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);		// Analog Input 13 (12V Sense)
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);		// Analog Input 12 (Board Temp)
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);		// Analog Input 7 (T_Input 1)
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_5);		// Analog Input 6 (Analog In 6)
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_6);		// Analog Input 5 (Analog In 5)
	// Special PD7 (NMI) Unlock Sequence
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);		// Analog Input 4 (Analog In 4)
	// Enable Weak Pull Up Resistor on Pin 0
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port E pins
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);		// Analog Input 3 (Analog In 3)
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);		// Analog Input 2 (Analog In 2)
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);		// Analog Input 1 (Analog In 1)
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);		// Analog Input 0 (Revision Sense)
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);		// Analog Input 9 (T_Input 3)
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);		// Analog Input 8 (T_Input 2)
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_6);	// IN: Digital Input 7
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_7);	// IN: Digital Input 6
	// Enable Weak Pull Up Resistors on Pins 6 and 7
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port F pins
	// Special PF0 (NMI) Unlock Sequence
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;
	GPIOPinConfigure(GPIO_PF0_SSI1RX);					// SPI1 MISO
	GPIOPinConfigure(GPIO_PF1_SSI1TX);					// SPI1 MOSI
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);					// SPI1 CLK
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);	// OUT: MicroSD Card CS
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);	// OUT: Flash Memory Write Protect
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_5);	// IN: Power Good
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_6);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_7);	// IN: Not Used
	// Set MicroSD Card CS to High, Turn off Write Protect
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_3);
	// Enable Alternate Functions
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
	// Enable Weak Pull Down Resistors on Pins 6 and 7
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	// Enable Weak Pull Up Resistor on Pin 5
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port G pins
	GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_0);	// IN: Digital Input 1
	GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_1);	// IN: Digital Input 2
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);	// OUT: High Side Driver 1
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_3);	// OUT: High Side Driver 2
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_4);	// OUT: High Side Driver 3
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_5);	// OUT: High Side Driver 4
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_6);	// OUT: High Side Driver 5
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_7);	// OUT: High Side Driver 6
	// Turn off all Outputs
	GPIOPinWrite(GPIO_PORTG_BASE, 0xFC, 0x00);
	// Enable Weak Pull Up Resistors on Pins 1 and 2
	GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port H pins
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_0);	// OUT: Relay Driver 1
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_1);	// OUT: Relay Driver 2
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_2);	// OUT: Relay Driver 3
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_3);	// OUT: Relay Driver 4
	GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_4);	// IN: Digital Input 9
	GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_5);	// IN: Digital Input 10
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_6);	// OUT: CAN2 Termination Set
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_7);	// OUT: CAN2 Termination Reset
	// Turn off all Outputs
	GPIOPinWrite(GPIO_PORTH_BASE, 0xCF, 0x00);
	// Disable Pull-up/down Resistors on Pins 4 and 5
	GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	//////////////////////////////////////////////////////
	// Setup GPIO Port J pins
	GPIOPinTypeGPIOOutput(GPIO_PORTJ_BASE, GPIO_PIN_0);	// OUT: High Side Driver 7
	GPIOPinTypeGPIOOutput(GPIO_PORTJ_BASE, GPIO_PIN_1);	// OUT: High Side Driver 8
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_2);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_3);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_4);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_5);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_6);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_7);	// IN: Not Used
	// Enable Weak Pull Up Resistors on All Input Pins
	GPIOPadConfigSet(GPIO_PORTJ_BASE, 0xFC, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port K pins
	GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_0);	// IN: Digital Input 2
	GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_1);	// IN: Digital Input 3
	GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_2);	// IN: Digital Input 4
	GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_3);	// IN: Digital Input 5
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4);	// OUT: Relay Driver 5
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_5);	// OUT: Relay Driver 6
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_6);	// OUT: Relay Driver 7
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_7);	// OUT: Relay Driver 8
	// Turn off all Outputs
	GPIOPinWrite(GPIO_PORTK_BASE, 0xF0, 0x00);
	// Enable Weak Pull Up Resistors on Input Pins
	GPIOPadConfigSet(GPIO_PORTK_BASE, 0x0F, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//////////////////////////////////////////////////////
	// Setup GPIO Port L pins
	GPIOPinConfigure(GPIO_PL0_WT0CCP0);					// PWM IN: Digital Input 9
	GPIOPinConfigure(GPIO_PL1_WT0CCP1);					// PWM IN: Digital Input 10
	GPIOPinConfigure(GPIO_PL2_WT1CCP0);					// PWM IN: Digital Input 11
	GPIOPinConfigure(GPIO_PL3_WT1CCP1);					// PWM IN: Digital Input 12
	GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_4);	// IN: RTC_CLK
	GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_5);	// IN: RTC_INT
	GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_6);	// OUT: Status LED 1
	GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_7);	// OUT: Status LED 2
	// Turn off all Outputs
	GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0x00);
	// Enable Weak Pull Up Resistors on RTC_CLK and RTC_INT Pins
	GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	// Enable Alternate Functions
	GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	// Enable 8mA Pad Setting on Status LED Outputs
	GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

	//////////////////////////////////////////////////////
	// Setup GPIO Port M pins
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);	// OUT: Relay Driver 9
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_1);	// OUT: Relay Driver 10
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_2);	// OUT: Relay Driver 11
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_3);	// OUT: Relay Driver 12
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_4);	// OUT: Relay Driver 13
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_5);	// OUT: Relay Driver 14
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);	// OUT: CAN1 Termination Set
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_7);	// OUT: CAN1 Termination Reset
	// Turn off all Outputs
	GPIOPinWrite(GPIO_PORTM_BASE, 0xFF, 0x00);

	//////////////////////////////////////////////////////
	// Setup GPIO Port N pins
	GPIOPinConfigure(GPIO_PN0_CAN0RX);					// CAN1 RX
	GPIOPinConfigure(GPIO_PN1_CAN0TX);					// CAN1 TX
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_2);	// IN: CAN2 Termination Sense
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_3);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_4);	// IN: CAN1 Termination Sense
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_5);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_6);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_7);	// IN: Not Used
	// Enable Alternate Functions
	GPIOPinTypeCAN(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	// Enable Weak Pull Up Resistors on Unused Input Pins
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	// Enable Weak Pull Down Resistors on CAN Termination Sense Inputs
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_2 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

	//////////////////////////////////////////////////////
	// Setup GPIO Port P pins
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_0);	// IN: Not Used
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_1);	// IN: Digital Input 8
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_2);	// IN: Not Used
	// Enable Weak Pull Up Resistors on Pin 1
	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	// Enable Weak Pull Down Resistors on Unused Input Pins
	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

void VCU1200_Board::initializeSPI()
{
	//////////////////////////////////////////////////////
	// Enable the SSI0 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

	// Wait for SPI Peripheral to Power Up
	SysCtlDelay(5);

	// Configure SSI0 to 500 kHz, Freescale SPI Master Mode 1, 8 bit data
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 500000, 8);

	// Enable SSI0
	SSIEnable(SSI0_BASE);

	//////////////////////////////////////////////////////
	// Enable the SSI1 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

	// Wait for SPI Peripheral to Power Up
	SysCtlDelay(5);

	// Configure SSI1 to 1MHz, Freescale SPI Master Mode 0, 8 bit data
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

	// Enable SSI1
	SSIEnable(SSI1_BASE);
}

void VCU1200_Board::initializeSystem()
{
	// Initialize the Clock System
	initializeClock();

	// Initialize the Watchdog Timer
	initializeWatchdog();

	// Set the device pinout appropriately for this board
	initializeIO();

	// Load Hard-Coded Calibration Values
	loadCalibration();

	// Initialize ADC Module
	initializeADC();

	// Initialize SPI Module
	initializeSPI();

	// Initialize EEPROM
	initializeEEPROM();

	// Initialize Timers
	initializeTimers();

	// Initialize CAN Interface
	can.initialize();

	// Setup CAN Receive Messages
	initializeCANReceive();

	// Read all Inputs
	readAllPins();

	// Set Blink Rate to 1 Second
	setBlinkRate(100);
}

void VCU1200_Board::initializeTimers()
{
	////////////////////////////////////////////////////
	// Turn On Wide Timer 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);

	// Wait for Timer Peripheral to Power Up
	SysCtlDelay(5);

	// Set Timer A to Capture Time Mode (Count Down)
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP | TIMER_CFG_B_CAP_TIME_UP);

	// Set Timer A to Interrupt on Both Edges
	TimerControlEvent(WTIMER0_BASE, TIMER_BOTH, TIMER_EVENT_POS_EDGE);

	// Set Timer Clock Source to System Clock
	TimerClockSourceSet(WTIMER0_BASE, TIMER_CLOCK_SYSTEM);

	// Set Timer Load Value to CPU Clock Frequency (1 Second Period)
	TimerLoadSet(WTIMER0_BASE, TIMER_BOTH, SysCtlClockGet());

	// Set Timer Prescaler to 0
	TimerPrescaleSet(WTIMER0_BASE, TIMER_BOTH, 0);

	// Set Timer to Stop During Debugging
	TimerControlStall(WTIMER0_BASE, TIMER_BOTH, true);

	////////////////////////////////////////////////////
	// Turn On Wide Timer 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);

	// Wait for Timer Peripheral to Power Up
	SysCtlDelay(5);

	// Set Timer A to Capture Time Mode (Count Down)
	TimerConfigure(WTIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC_UP | TIMER_CFG_B_CAP_TIME_UP);

	// Set Timer A to Interrupt on Both Edges
	TimerControlEvent(WTIMER1_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);

	// Set Timer Clock Source to System Clock
	TimerClockSourceSet(WTIMER1_BASE, TIMER_CLOCK_SYSTEM);

	// Set Timer Load Value to CPU Clock Frequency (1 Second Period)
	TimerLoadSet(WTIMER1_BASE, TIMER_BOTH, SysCtlClockGet());

	// Set Timer Prescaler to 0
	TimerPrescaleSet(WTIMER1_BASE, TIMER_BOTH, 0);

	// Set Timer to Stop During Debugging
	TimerControlStall(WTIMER1_BASE, TIMER_BOTH, true);
}

void VCU1200_Board::initializeWatchdog()
{
	// Turn on Watchdog Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

	// Wait for Watchdog Peripheral to become available
	SysCtlDelay(5);

	// Check to see if Watchdog timer is locked
	if (WatchdogLockState(WATCHDOG0_BASE) == true)
	{
		// Unlock Watchdog if necessary
		WatchdogUnlock(WATCHDOG0_BASE);
	}

	// Set the Watchdog reload value (timeout period = 250msec)
	WatchdogReloadSet(WATCHDOG0_BASE, SysCtlClockGet() / 4);

	// Set Watchdog stall to enable debugging breakpoints to pause the timer
	WatchdogStallEnable(WATCHDOG0_BASE);

	// Enable the reset
	WatchdogResetEnable(WATCHDOG0_BASE);

	// Enable the Watchdog timer
	WatchdogEnable(WATCHDOG0_BASE);
}

void VCU1200_Board::loadCalibration()
{
	adc_offset 	= ADC_OFFSET;
	adc_gain 	= ADC_GAIN;
	adc_ref 	= ADC_REF;

	board_temp_offset 	= BOARD_TEMP_OFFSET;
	board_temp_gain		= BOARD_TEMP_GAIN;

	send_io_debug_messages = SEND_IO_DEBUG_MESSAGES;

	can_offset = VCU_CAN_OFFSET;
}

void VCU1200_Board::manageBootloaderJump()
{
	if (reset_flag)
	{
		// Reset Device to Jump to Bootloader
		SysCtlReset();
	}
}

void VCU1200_Board::manageCANResponses()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	if (can_requests[CAN_FIRMWARE_VERSION])
	{
		// Firmware Version and Status ACK (CAN Offset + 0x0F1)
		msg.setID(can_offset + 0x0F1);
		msg.setLength(4);

		// Bootloader Ready Flag
		msg.getData()[3] = !bootloader_safe;
		// Version Numbers
		msg.getData()[2] = VERSION_MAJOR;
		msg.getData()[1] = VERSION_MINOR;
		msg.getData()[0] = VERSION_REVISION;

		// Send Message
		board.can.sendMessage(CAN_1, &msg);

		// Clear Flag
		can_requests[CAN_FIRMWARE_VERSION] = false;
	}

	if (can_requests[CAN_DEVICE_RESET])
	{
		// Jump to Bootloader ACK (CAN Offset + 0x0F3)
		msg.setID(can_offset + 0x0F3);
		msg.setLength(1);

		msg.getData()[0] = !bootloader_safe;

		// Send Message
		board.can.sendMessage(CAN_1, &msg);

		// Set Flag to Reset on Next Tick
		if (bootloader_safe)
		{
			reset_flag = true;
		}

		// Clear Flag
		can_requests[CAN_DEVICE_RESET] = false;
	}
}

void VCU1200_Board::manageCANTerminations()
{
	if (can_termination_counter == 0 )
	{
		// Turn off CAN Termination Relay Drive Signals
		setOutput(CAN_1_TERM_SET, 	LOW, true);
		setOutput(CAN_1_TERM_RESET, LOW, true);
		setOutput(CAN_2_TERM_SET, 	LOW, true);
		setOutput(CAN_2_TERM_RESET, LOW, true);
	}
	else
	{
		can_termination_counter--;
	}
}

void VCU1200_Board::readAllPins(unsigned int samples)
{
	// Read All Inputs
	for (int i = 0; i < NUM_DIGITAL_INPUTS; i++)
	{
		digital_inputs[(Input)i] = readInput((Input)i, samples);
	}

	// Read All Outputs
	for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++)
	{
		digital_outputs[(Output)i] = readOutput((Output)i, samples);
	}

	// Update CAN Termination Flags
	// Update CAN Termination Resistor State from Sensed Value
	if (readInput(CAN_1_TERM_SENSE))
	{
		can_termination[CAN_1] = true;
	}
	else
	{
		can_termination[CAN_1] = false;
	}

	if (readInput(CAN_2_TERM_SENSE))
	{
		can_termination[CAN_2] = true;
	}
	else
	{
		can_termination[CAN_2] = false;
	}
}

PinState VCU1200_Board::readInput(Input in_pin, unsigned int samples)
{
	switch (in_pin)
	{
	case DIGITAL_INPUT_1:
		return readPinAvg(GPIO_PORTD_BASE, GPIO_PIN_0, samples);

	case DIGITAL_INPUT_2:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_0, samples);

	case DIGITAL_INPUT_3:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_1, samples);

	case DIGITAL_INPUT_4:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_2, samples);

	case DIGITAL_INPUT_5:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_3, samples);

	case DIGITAL_INPUT_6:
		return readPinAvg(GPIO_PORTE_BASE, GPIO_PIN_7, samples);

	case DIGITAL_INPUT_7:
		return readPinAvg(GPIO_PORTE_BASE, GPIO_PIN_6, samples);

	case DIGITAL_INPUT_8:
		return readPinAvg(GPIO_PORTP_BASE, GPIO_PIN_1, samples);

	case DIGITAL_INPUT_9:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_4, samples);

	case DIGITAL_INPUT_10:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_5, samples);

	case DIGITAL_INPUT_11:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_0, samples);

	case DIGITAL_INPUT_12:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_1, samples);

	case POWER_GOOD:
		return readPinAvg(GPIO_PORTF_BASE, GPIO_PIN_5, samples);

	case CARD_LOADED:
		return readPinAvg(GPIO_PORTB_BASE, GPIO_PIN_0, samples);

	case CAN_1_TERM_SENSE:
		return readPinAvg(GPIO_PORTN_BASE, GPIO_PIN_4, samples);

	case CAN_2_TERM_SENSE:
		return readPinAvg(GPIO_PORTN_BASE, GPIO_PIN_2, samples);

	default:
		// Program should never get here
		error("Unimplemented Pin in getInput() call.", "vcu1200_board.cpp", __LINE__);
		return LOW;
	}
}

PinState VCU1200_Board::readOutput(Output out_pin, unsigned int samples)
{
	switch (out_pin)
	{
	case RELAY_DRIVER_1:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_0, samples);

	case RELAY_DRIVER_2:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_1, samples);

	case RELAY_DRIVER_3:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_2, samples);

	case RELAY_DRIVER_4:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_3, samples);

	case RELAY_DRIVER_5:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_4, samples);

	case RELAY_DRIVER_6:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_5, samples);

	case RELAY_DRIVER_7:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_6, samples);

	case RELAY_DRIVER_8:
		return readPinAvg(GPIO_PORTK_BASE, GPIO_PIN_7, samples);

	case RELAY_DRIVER_9:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_0, samples);

	case RELAY_DRIVER_10:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_1, samples);

	case RELAY_DRIVER_11:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_2, samples);

	case RELAY_DRIVER_12:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_3, samples);

	case RELAY_DRIVER_13:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_4, samples);

	case RELAY_DRIVER_14:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_5, samples);

	case HIGH_SIDE_DRIVER_1:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_2, samples);

	case HIGH_SIDE_DRIVER_2:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_3, samples);

	case HIGH_SIDE_DRIVER_3:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_4, samples);

	case HIGH_SIDE_DRIVER_4:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_5, samples);

	case HIGH_SIDE_DRIVER_5:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_6, samples);

	case HIGH_SIDE_DRIVER_6:
		return readPinAvg(GPIO_PORTG_BASE, GPIO_PIN_7, samples);

	case HIGH_SIDE_DRIVER_7:
		return readPinAvg(GPIO_PORTJ_BASE, GPIO_PIN_0, samples);

	case HIGH_SIDE_DRIVER_8:
		return readPinAvg(GPIO_PORTJ_BASE, GPIO_PIN_1, samples);

	case STATUS_LED_1:
		return readPinAvg(GPIO_PORTL_BASE, GPIO_PIN_6, samples);

	case STATUS_LED_2:
		return readPinAvg(GPIO_PORTL_BASE, GPIO_PIN_7, samples);

	case CAN_1_TERM_SET:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_6, samples);

	case CAN_1_TERM_RESET:
		return readPinAvg(GPIO_PORTM_BASE, GPIO_PIN_7, samples);

	case CAN_2_TERM_SET:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_6, samples);

	case CAN_2_TERM_RESET:
		return readPinAvg(GPIO_PORTH_BASE, GPIO_PIN_7, samples);

	case WRITE_PROTECT:
		return readPinAvg(GPIO_PORTF_BASE, GPIO_PIN_4, samples);

	case ANALOG_5V_OUTPUT_1:
		return readPinAvg(GPIO_PORTC_BASE, GPIO_PIN_6, samples);

	case ANALOG_5V_OUTPUT_2:
		return readPinAvg(GPIO_PORTC_BASE, GPIO_PIN_7, samples);

	default:
		// Program should never get here
		error("Unimplemented Pin in getOutput() call.", "vcu1200_board.cpp", __LINE__);
		return LOW;
	}
}

PinState VCU1200_Board::readPinAvg(uint32_t port, uint8_t pin, unsigned int samples)
{
	int count = 0;
	for(int i = 0; i < samples; i++)
	{
		// Delay by 3 cycles
		SysCtlDelay(1);
		if (GPIOPinRead(port, pin) != LOW)
		{
			count++;
		}
	}

	if (count > (samples / 2))
	{
		return HIGH;
	}
	else
	{
		return LOW;
	}
}

bool VCU1200_Board::receiveCANMessage(CANPort can_port, tCANMsgObject* message, unsigned int mailbox)
{
	uint16_t password;

	if (can_port == CAN_1)
	{
		// VCU I/O Debug Output (CAN Offset + 0x0C0)
		if (message->ui32MsgID == can_offset + 0x0C0)
		{
			// Update I/O Debug Enable
			if (message->pui8MsgData[0] == 0x01)
			{
				send_io_debug_messages = true;
			}
			else
			{
				send_io_debug_messages = false;
			}

			return true;
		}
		// VCU Output Override (CAN Offset + 0x0C1)
		else if (message->ui32MsgID == can_offset + 0x0C1)
		{
			password = message->pui8MsgData[7] << 8 | message->pui8MsgData[6];
			if (output_override_safe && password == 0xBEAD)
			{
				// Update Override Values
				// Termination Resistors
				can_termination_override[CAN_1] = message->pui8MsgData[3] & 0x01;
				can_termination_override[CAN_2] = (message->pui8MsgData[3] & 0x02) >> 1;
				// High Side Drivers
				for (int i = HIGH_SIDE_DRIVER_1; i <= HIGH_SIDE_DRIVER_8; i++)
				{
					digital_outputs_override[i] = (PinState)((message->pui8MsgData[2] & 1 << (i - HIGH_SIDE_DRIVER_1)) >> (i - HIGH_SIDE_DRIVER_1));
				}
				// Relay Drivers 9 - 14
				for (int i = RELAY_DRIVER_9; i <= RELAY_DRIVER_14; i++)
				{
					digital_outputs_override[i] = (PinState)((message->pui8MsgData[1] & 1 << (i - RELAY_DRIVER_9)) >> (i - RELAY_DRIVER_9));
				}
				// Analog 5V Outputs
				digital_outputs_override[ANALOG_5V_OUTPUT_1] = (PinState)((message->pui8MsgData[1] & 0x40) >> 6);
				digital_outputs_override[ANALOG_5V_OUTPUT_2] = (PinState)((message->pui8MsgData[1] & 0x80) >> 7);
				// Relay Drivers 1 - 8
				for (int i = RELAY_DRIVER_1; i <= RELAY_DRIVER_8; i++)
				{
					digital_outputs_override[i] = (PinState)((message->pui8MsgData[0] & 1 << (i - RELAY_DRIVER_1)) >> (i - RELAY_DRIVER_1));
				}

				setOutputOverride(true);
			}
			else if (output_override)
			{
				setOutputOverride(false);
			}

			return true;
		}
		// Request Firmware Version (CAN Offset + 0x0F0)
		else if (message->ui32MsgID == can_offset + 0x0F0)
		{
			can_requests[CAN_FIRMWARE_VERSION] = true;

			return true;
		}
		// Request Reset (CAN Offset + 0x0F2)
		else if (message->ui32MsgID == can_offset + 0x0F2)
		{
			password = message->pui8MsgData[1] << 8 | message->pui8MsgData[0];
			if (password == 0xDEAD)
			{
				can_requests[CAN_DEVICE_RESET] = true;
			}

			return true;
		}
	}

	// Message is not handled by Board Object
	return false;
}

bool VCU1200_Board::receiveEncoderPulse(Input input, unsigned int pulse_cycles)
{
	bool processed = false;
	int delta_cycles;

	switch (input)
	{
	case DIGITAL_INPUT_9:
		if (encoder_overflows[0] >= 5)
		{
			encoder_cycles[0] = 0;
		}
		else
		{
			encoder_cycles[0] = pulse_cycles + encoder_overflows[0] * TimerLoadGet(WTIMER0_BASE, TIMER_A) - encoder_cycles[0];
		}
		new_encoder_pulse[0] = true;
		encoder_overflows[0] = 0;
		processed = true;
		break;

	case DIGITAL_INPUT_10:
		if (encoder_overflows[1] >= 5)
		{
			encoder_cycles[1] = 0;
		}
		else
		{
			encoder_cycles[1] = pulse_cycles + encoder_overflows[1] * TimerLoadGet(WTIMER0_BASE, TIMER_B) - encoder_cycles[1];
		}
		new_encoder_pulse[1] = true;
		encoder_overflows[1] = 0;
		processed = true;
		break;

	case DIGITAL_INPUT_11:
		if (encoder_overflows[2] >= 5)
		{
			encoder_cycles[2] = 0;
		}
		else
		{
			encoder_cycles[2] = pulse_cycles + encoder_overflows[2] * TimerLoadGet(WTIMER1_BASE, TIMER_A) - encoder_cycles[2];
		}
		new_encoder_pulse[2] = true;
		encoder_overflows[2] = 0;
		processed = true;
		break;

	case DIGITAL_INPUT_12:
		if (encoder_overflows[3] <= 10)
		{
			delta_cycles = (int)pulse_cycles - last_encoder_pulse[3] + (int)encoder_overflows[3] * TimerLoadGet(WTIMER1_BASE, TIMER_B);
			encoder_cycles[3] = delta_cycles;
		}
		// Skip Short Cycles
		if (delta_cycles < SysCtlClockGet() / 200)
		{
			break;
		}
		last_encoder_pulse[3] = pulse_cycles;
		new_encoder_pulse[3] = true;
		encoder_overflows[3] = 0;
		processed = true;
		break;
	}

	return processed;
}

void VCU1200_Board::reset()
{
	wakeup = false;
	startup_counter = 0;
}

void VCU1200_Board::setBlinkRate(unsigned int rate)
{
	blink_rate = rate;
}

void VCU1200_Board::sendCANData()
{
	// CAN Message Object for Transmission
	CANMessage msg(CANMessage::ID_STANDARD, CANMessage::DATA_FRAME, 8);

	int16_t stemp;
	uint16_t utemp;

	// Every 100ms - Step 1
	if (getStartupCounter() % 10 == 0)
	{
		// VCU Board Status (CAN Offset + 0x0A0)
		msg.setID(can_offset + 0x0A0);
		msg.setLength(8);

		// Board Temperature
		stemp = getAnalogValue(BOARD_TEMP) * 10.0f;
		msg.getData()[7] = stemp >> 8;
		msg.getData()[6] = stemp;
		// 12V Sense
		utemp = getAnalogValue(SENSE_12V) * 1000.0f;
		msg.getData()[5] = utemp >> 8;
		msg.getData()[4] = utemp;
		// 5V Sense
		utemp = getAnalogValue(SENSE_5V) * 1000.0f;
		msg.getData()[3] = utemp >> 8;
		msg.getData()[2] = utemp;
		// Status Flags 1
		msg.getData()[1] = getInput(CARD_LOADED);
		msg.getData()[1] |= can_termination[CAN_1] 	<< 1;
		msg.getData()[1] |= can_termination[CAN_2] 	<< 2;
		msg.getData()[1] |= getInput(POWER_GOOD)				<< 3;
		// Reserved Byte
		msg.getData()[0] = 0;

		// Send Message
		can.sendMessage(CAN_1, &msg);
	}
	// Every 500ms - Step 2
	else if (getStartupCounter() % 50 == 1)
	{
		if (send_io_debug_messages)
		{
			// VCU Digital Inputs (CAN Offset + 0x0B0)
			msg.setID(can_offset + 0x0B0);
			msg.setLength(2);

			// Digital Inputs 2
			msg.getData()[1] = getInput(DIGITAL_INPUT_9);
			msg.getData()[1] |= getInput(DIGITAL_INPUT_10)		<< 1;
			msg.getData()[1] |= getInput(DIGITAL_INPUT_11)		<< 2;
			msg.getData()[1] |= getInput(DIGITAL_INPUT_12)		<< 3;
			msg.getData()[1] |= getInput(POWER_GOOD)			<< 4;
			msg.getData()[1] |= getInput(CARD_LOADED)			<< 5;
			msg.getData()[1] |= getInput(CAN_1_TERM_SENSE)		<< 6;
			msg.getData()[1] |= getInput(CAN_2_TERM_SENSE)		<< 7;
			// Digital Inputs 1
			msg.getData()[0] = getInput(DIGITAL_INPUT_1);
			msg.getData()[0] |= getInput(DIGITAL_INPUT_2)		<< 1;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_3)		<< 2;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_4)		<< 3;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_5)		<< 4;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_6)		<< 5;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_7)		<< 6;
			msg.getData()[0] |= getInput(DIGITAL_INPUT_8)		<< 7;

			// Send Message
			can.sendMessage(CAN_1, &msg);

			// VCU Analog Inputs 1 (CAN Offset + 0x0B2)
			msg.setID(can_offset + 0x0B2);
			msg.setLength(8);

			// Analog Input 1
			utemp = getAnalogValue(ANALOG_INPUT_1) * 1000.0f;
			msg.getData()[7] = utemp >> 8;
			msg.getData()[6] = utemp;
			// Analog Input 2
			utemp = getAnalogValue(ANALOG_INPUT_2) * 1000.0f;
			msg.getData()[5] = utemp >> 8;
			msg.getData()[4] = utemp;
			// Analog Input 3
			utemp = getAnalogValue(ANALOG_INPUT_3) * 1000.0f;
			msg.getData()[3] = utemp >> 8;
			msg.getData()[2] = utemp;
			// Analog Input 4
			utemp = getAnalogValue(ANALOG_INPUT_4) * 1000.0f;
			msg.getData()[1] = utemp >> 8;
			msg.getData()[0] = utemp;

			// Send Message
			can.sendMessage(CAN_1, &msg);

			// VCU Analog Inputs 2 (CAN Offset + 0x0B3)
			msg.setID(can_offset + 0x0B3);
			msg.setLength(4);

			// Analog Input 5
			utemp = getAnalogValue(ANALOG_INPUT_5) * 1000.0f;
			msg.getData()[3] = utemp >> 8;
			msg.getData()[2] = utemp;
			// Analog Input 4
			utemp = getAnalogValue(ANALOG_INPUT_6) * 1000.0f;
			msg.getData()[1] = utemp >> 8;
			msg.getData()[0] = utemp;

			// Send Message
			can.sendMessage(CAN_1, &msg);
		}

		if (send_io_debug_messages || output_override)
		{
			// VCU Digital Outputs (CAN Offset + 0xB1)
			msg.setID(can_offset + 0x0B1);
			msg.setLength(6);

			// Output Override Status
			msg.getData()[5] = output_override;
			// Reserved Byte
			msg.getData()[4] = 0;
			// CAN Termination Resistor Status
			msg.getData()[3] = can_termination[CAN_1];
			msg.getData()[3] |= can_termination[CAN_2] 	<< 1;
			// Digital Outputs 3
			msg.getData()[2] = getOutput(HIGH_SIDE_DRIVER_1);
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_2)		<< 1;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_3)		<< 2;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_4)		<< 3;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_5)		<< 4;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_6)		<< 5;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_7)		<< 6;
			msg.getData()[2] |= getOutput(HIGH_SIDE_DRIVER_8)		<< 7;
			// Digital Outputs 2
			msg.getData()[1] = getOutput(RELAY_DRIVER_9);
			msg.getData()[1] |= getOutput(RELAY_DRIVER_10)			<< 1;
			msg.getData()[1] |= getOutput(RELAY_DRIVER_11)			<< 2;
			msg.getData()[1] |= getOutput(RELAY_DRIVER_12)			<< 3;
			msg.getData()[1] |= getOutput(RELAY_DRIVER_13)			<< 4;
			msg.getData()[1] |= getOutput(RELAY_DRIVER_14)			<< 5;
			msg.getData()[1] |= getOutput(ANALOG_5V_OUTPUT_1)		<< 6;
			msg.getData()[1] |= getOutput(ANALOG_5V_OUTPUT_2)		<< 7;
			// Digital Inputs 1
			msg.getData()[0] = getOutput(RELAY_DRIVER_1);
			msg.getData()[0] |= getOutput(RELAY_DRIVER_2)			<< 1;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_3)			<< 2;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_4)			<< 3;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_5)			<< 4;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_6)			<< 5;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_7)			<< 6;
			msg.getData()[0] |= getOutput(RELAY_DRIVER_8)			<< 7;

			// Send Message
			can.sendMessage(CAN_1, &msg);
		}
	}
	// Every 500ms - Step 3
	else if (getStartupCounter() % 50 == 2)
	{
		if (send_io_debug_messages)
		{
			// VCU Thermistor Inputs 1 (CAN Offset + 0xB4)
			msg.setID(can_offset + 0x0B4);
			msg.setLength(8);

			// Temperature Input 1
			utemp = getAnalogValue(THERMISTOR_1) / 25.0f;
			msg.getData()[7] = utemp >> 8;
			msg.getData()[6] = utemp;
			// Temperature Input 2
			utemp = getAnalogValue(THERMISTOR_2) / 25.0f;
			msg.getData()[5] = utemp >> 8;
			msg.getData()[4] = utemp;
			// Temperature Input 3
			utemp = getAnalogValue(THERMISTOR_3) / 25.0f;
			msg.getData()[3] = utemp >> 8;
			msg.getData()[2] = utemp;
			// Temperature Input 4
			utemp = getAnalogValue(THERMISTOR_4) / 25.0f;
			msg.getData()[1] = utemp >> 8;
			msg.getData()[0] = utemp;

			// Send Message
			can.sendMessage(CAN_1, &msg);

			// VCU Temperature Inputs 2 (CAN Offset + 0xB5)
			msg.setID(can_offset + 0x0B5);
			msg.setLength(4);

			// Temperature Input 5
			utemp = getAnalogValue(THERMISTOR_5) / 25.0f;
			msg.getData()[3] = utemp >> 8;
			msg.getData()[2] = utemp;
			// Reserved
			msg.getData()[1] = 0;
			msg.getData()[0] = 0;

			// Send Message
			can.sendMessage(CAN_1, &msg);
		}
	}
}

void VCU1200_Board::setCANTermination(CANPort can_port, bool active, bool override)
{
	// See if a Change is Required
	if (can_termination[can_port] != active)
	{
		if (can_port == CAN_1)
		{
			if (active)
			{
				setOutput(CAN_1_TERM_RESET, LOW, override);
				setOutput(CAN_1_TERM_SET, HIGH, override);
			}
			else
			{
				setOutput(CAN_1_TERM_SET, LOW, override);
				setOutput(CAN_1_TERM_RESET, HIGH, override);
			}

			// Start Counter
			can_termination_counter = 2;

			// Update Setting Value
			if (!override)
			{
				can_termination_setting[can_port] = active;
			}
		}
		else if (can_port == CAN_2)
		{
			if (active)
			{
				setOutput(CAN_2_TERM_RESET, LOW, override);
				setOutput(CAN_2_TERM_SET, HIGH, override);
			}
			else
			{
				setOutput(CAN_2_TERM_SET, LOW, override);
				setOutput(CAN_2_TERM_RESET, HIGH, override);
			}

			// Start Counter
			can_termination_counter = 2;

			// Update Setting Value
			if (!override)
			{
				can_termination_setting[can_port] = active;
			}
		}
	}
}

void VCU1200_Board::setPinResistor(Input input, PinSetting pin_setting)
{
	uint32_t pin_config;

	// Determine Pin Setting
	switch (pin_setting)
	{
	case PULL_UP:
		pin_config = GPIO_PIN_TYPE_STD_WPU;
		break;

	case PULL_DOWN:
		pin_config = GPIO_PIN_TYPE_STD_WPD;
		break;

	case OPEN_PIN:
		pin_config = GPIO_PIN_TYPE_STD;
		break;
	}

	// Update Pin Pad Configuration
	switch (input)
	{
	case DIGITAL_INPUT_1:
		GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_2:
		GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_3:
		GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_4:
		GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_5:
		GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_6:
		GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_7:
		GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_8:
		GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_9:
		GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_10:
		GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_11:
		GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, pin_config);
		break;

	case DIGITAL_INPUT_12:
		GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, pin_config);
		break;

	default:
		// Program should never get here
		error("Unimplemented Pin in setPinResistor() call.", "vcu1200_board.cpp", __LINE__);
		break;
	}
}

void VCU1200_Board::setOutput(Output out_pin, PinState out_state, bool override)
{
	// Update Local Cache
	if (!override)
	{
		digital_outputs[out_pin] = out_state;

		// Don't Update Pins if In Override Mode
		if (output_override)
		{
			return;
		}
	}

	// Write Value to Pin
	switch (out_pin)
	{
	case RELAY_DRIVER_1:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, out_state);
			break;

	case RELAY_DRIVER_2:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_1, out_state << 1);
			break;

	case RELAY_DRIVER_3:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_2, out_state << 2);
			break;

	case RELAY_DRIVER_4:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_3, out_state << 3);
			break;

	case RELAY_DRIVER_5:
			GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_4, out_state << 4);
			break;

	case RELAY_DRIVER_6:
			GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, out_state << 5);
			break;

	case RELAY_DRIVER_7:
			GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case RELAY_DRIVER_8:
			GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_7, out_state << 7);
			break;

	case RELAY_DRIVER_9:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, out_state);
			break;

	case RELAY_DRIVER_10:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, out_state << 1);
			break;

	case RELAY_DRIVER_11:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, out_state << 2);
			break;

	case RELAY_DRIVER_12:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_3, out_state << 3);
			break;

	case RELAY_DRIVER_13:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, out_state << 4);
			break;

	case RELAY_DRIVER_14:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, out_state << 5);
			break;

	case HIGH_SIDE_DRIVER_1:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, out_state << 2);
			break;

	case HIGH_SIDE_DRIVER_2:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_3, out_state << 3);
			break;

	case HIGH_SIDE_DRIVER_3:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_4, out_state << 4);
			break;

	case HIGH_SIDE_DRIVER_4:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_5, out_state << 5);
			break;

	case HIGH_SIDE_DRIVER_5:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case HIGH_SIDE_DRIVER_6:
			GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_7, out_state << 7);
			break;

	case HIGH_SIDE_DRIVER_7:
			GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_0, out_state);
			break;

	case HIGH_SIDE_DRIVER_8:
			GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_1, out_state << 1);
			break;

	case STATUS_LED_1:
			GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case STATUS_LED_2:
			GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_7, out_state << 7);
			break;

	case CAN_1_TERM_SET:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case CAN_1_TERM_RESET:
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, out_state << 7);
			break;

	case CAN_2_TERM_SET:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case CAN_2_TERM_RESET:
			GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_7, out_state << 7);
			break;

	case WRITE_PROTECT:
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, out_state << 4);
			break;

	case ANALOG_5V_OUTPUT_1:
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, out_state << 6);
			break;

	case ANALOG_5V_OUTPUT_2:
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, out_state << 7);
			break;

	default:
		// Program should never get here
		error("Unimplemented Output Pin called in setOutput()", "vcu1200_board.cpp", __LINE__);
		break;
	}
}

void VCU1200_Board::setOutputOverride(bool override)
{
	// Update Flag
	output_override = override;

	if (output_override)
	{
		// Update Outputs to Override Values
		for (int i = 0; i <= ANALOG_5V_OUTPUT_2; i++)
		{
			setOutput((Output)i, digital_outputs_override[i], true);
		}

		// Update CAN Termination Resistors
		setCANTermination(CAN_1, can_termination_override[CAN_1], true);
		setCANTermination(CAN_2, can_termination_override[CAN_2], true);
	}
	else
	{
		// Update Outputs to Application Values
		for (int i = 0; i <= ANALOG_5V_OUTPUT_2; i++)
		{
			setOutput((Output)i, digital_outputs[i], false);
		}

		// Update CAN Termination Resistors
		setCANTermination(CAN_1, can_termination_setting[CAN_1], false);
		setCANTermination(CAN_2, can_termination_setting[CAN_2], false);
	}
}

void VCU1200_Board::setBootloaderSafe(bool enable)
{
	bootloader_safe = enable;
}

void VCU1200_Board::setOutputOverrideSafe(bool enable)
{
	output_override_safe = enable;
}

void VCU1200_Board::setWakeup()
{
	wakeup = true;
}

void VCU1200_Board::tick()
{
	// Pet the Dog
	clearWatchdog();

	// Jump to Bootloader if Requested and Safe
	manageBootloaderJump();

	// Blink the LED
	blinkLED();

	// Read State of Pins
	readAllPins();

	// Calculate Processed Analog Values
	updateAnalogValues();

	// Process CAN
	can.tick();

	// Send Any CAN Responses
	manageCANResponses();

	// Send Board-level CAN Data
	sendCANData();

	// Process CAN Termination Relay Actions
	manageCANTerminations();

	// Manage Counters
	incrementCounters();

	// Trigger ADC Conversion
	triggerADC();
}

void VCU1200_Board::timerOverflow(Input input)
{
	switch (input)
	{
	case DIGITAL_INPUT_12:
		if (encoder_overflows[3] < 1000000)
		{
			encoder_overflows[3]++;
		}
		break;
	}
}

void VCU1200_Board::triggerADC()
{
	// Trigger ADC0 Sequence 0
	ADCProcessorTrigger(ADC0_BASE, 0);

	// Trigger ADC1 Sequence 0
	ADCProcessorTrigger(ADC1_BASE, 0);
}

void VCU1200_Board::updateADCValues(ADCModule adc)
{
	if (adc == ADC_0)
	{
		ADC0_newdata = true;
	}
	else if (adc == ADC_1)
	{
		ADC1_newdata = true;
	}
}

void VCU1200_Board::updateAnalogValues()
{
	if (ADC0_newdata)
	{
		// Calculate Raw Voltage
		raw_analog_inputs[REVISION_SENSE]	= calcRawVoltage(ADC0_values[0]);
		raw_analog_inputs[ANALOG_INPUT_1]	= calcRawVoltage(ADC0_values[1]);
		raw_analog_inputs[ANALOG_INPUT_2]	= calcRawVoltage(ADC0_values[2]);
		raw_analog_inputs[ANALOG_INPUT_3]	= calcRawVoltage(ADC0_values[3]);
		raw_analog_inputs[ANALOG_INPUT_4]	= calcRawVoltage(ADC0_values[4]);
		raw_analog_inputs[ANALOG_INPUT_5]	= calcRawVoltage(ADC0_values[5]);
		raw_analog_inputs[ANALOG_INPUT_6]	= calcRawVoltage(ADC0_values[6]);

		// Calculate Measured Values
		analog_values[REVISION_SENSE]		= raw_analog_inputs[REVISION_SENSE];			// Pin Voltage
		analog_values[ANALOG_INPUT_1]		= raw_analog_inputs[ANALOG_INPUT_1] / 0.5674f;	// 5V Scale
		analog_values[ANALOG_INPUT_2]		= raw_analog_inputs[ANALOG_INPUT_2] / 0.5674f;	// 5V Scale
		analog_values[ANALOG_INPUT_3]		= raw_analog_inputs[ANALOG_INPUT_3] / 0.5674f;	// 5V Scale
		analog_values[ANALOG_INPUT_4]		= raw_analog_inputs[ANALOG_INPUT_4] / 0.5674f;	// 5V Scale
		analog_values[ANALOG_INPUT_5]		= raw_analog_inputs[ANALOG_INPUT_5] / 0.1111f;	// 24V Scale
		analog_values[ANALOG_INPUT_6]		= raw_analog_inputs[ANALOG_INPUT_6] / 0.1111f;	// 24V Scale

		// Clear New Data Flag
		ADC0_newdata = false;
	}

	if (ADC1_newdata)
	{
		// Calculate Raw Voltage
		raw_analog_inputs[THERMISTOR_1]	= calcRawVoltage(ADC1_values[0]);
		raw_analog_inputs[THERMISTOR_2]	= calcRawVoltage(ADC1_values[1]);
		raw_analog_inputs[THERMISTOR_3]	= calcRawVoltage(ADC1_values[2]);
		raw_analog_inputs[THERMISTOR_4]	= calcRawVoltage(ADC1_values[3]);
		raw_analog_inputs[THERMISTOR_5]	= calcRawVoltage(ADC1_values[4]);
		raw_analog_inputs[BOARD_TEMP]	= calcRawVoltage(ADC1_values[5]);
		raw_analog_inputs[SENSE_12V]	= calcRawVoltage(ADC1_values[6]);
		raw_analog_inputs[SENSE_5V]		= calcRawVoltage(ADC1_values[7]);

		// Calculate Measured Values
		analog_values[THERMISTOR_1]		= getResistance(raw_analog_inputs[THERMISTOR_1]);							// Ohms
		analog_values[THERMISTOR_2]		= getResistance(raw_analog_inputs[THERMISTOR_2]);							// Ohms
		analog_values[THERMISTOR_3]		= getResistance(raw_analog_inputs[THERMISTOR_3]);							// Ohms
		analog_values[THERMISTOR_4]		= getResistance(raw_analog_inputs[THERMISTOR_4]);							// Ohms
		analog_values[THERMISTOR_5]		= getResistance(raw_analog_inputs[THERMISTOR_5]);							// Ohms
		analog_values[BOARD_TEMP]		= (raw_analog_inputs[BOARD_TEMP] - BOARD_TEMP_OFFSET) / BOARD_TEMP_GAIN;	// Deg C
		analog_values[SENSE_12V]		= raw_analog_inputs[SENSE_12V] / 0.0909f;									// V
		analog_values[SENSE_5V]			= raw_analog_inputs[SENSE_5V] / 0.5000f;									// V

		// Clear New Data Flag
		ADC1_newdata = false;
	}
}
