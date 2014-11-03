/*
 * ADC.cpp
 *
 *  Created on: 10 Tem 2013
 *      Author: Deniz
 */

#include "ADC.h"
#include "msp430f5438a.h"

namespace MSP430
{
	unsigned int Value = 0;
	unsigned char ADCInterruptFlag = 0;

	ADC::ADC()
	{
	}

	unsigned char ADC::ReadAllValuesToCharArray(unsigned char* Channels)
	{
		unsigned char* TempVal;
		TempVal = SingleReadValueAsCharArray(Channel1);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel2);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel3);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel4);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel5);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel6);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel7);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel8);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel9);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel10);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel11);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel12);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel13);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel14);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel15);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		TempVal = SingleReadValueAsCharArray(Channel16);
		*Channels++ = TempVal[0];
		*Channels++ = TempVal[1];
		return 1;
	}

	unsigned char CharValue[2];
	unsigned char* ADC::SingleReadValueAsCharArray(ADCChannel Channel)
	{
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		ADC12CTL0 = ADC12ON + ADC12MSC;         // Sampling time, ADC12 on
		P6SEL |= 0xFF;                           			 // P6.0 ADC option select
		P7SEL |= 0xF0;
		ADC12MCTL0 = Channel;
		ADC12CTL1 = ADC12SHP; //+ ADC12SSEL_2;                     			// Use sampling timer
		ADC12IE = ADC12IFG0;                           		// Enable interrupt
		ADC12CTL0 |= ADC12ENC;
		ADC12CTL0 |= ADC12SC;                   			// Start sampling/conversion
		while(!ADCInterruptFlag);
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		*(unsigned int*) CharValue = Value;
		return CharValue;
	}

	unsigned int ADC::SingleReadValue(ADCChannel Channel)
	{
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		ADC12CTL0 = ADC12ON + ADC12MSC;         // Sampling time, ADC12 on
		P6SEL |= 0xFF;                           			 // P6.0 ADC option select
		P7SEL |= 0xF0;
		ADC12MCTL0 = Channel;
		ADC12CTL1 = ADC12SHP;                     			// Use sampling timer
		ADC12IE = ADC12IFG0;                           		// Enable interrupt
		ADC12CTL0 |= ADC12ENC;
	    ADC12CTL0 |= ADC12SC;                   			// Start sampling/conversion
		while(!ADCInterruptFlag);
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		return Value;
	}

	double ADC::SingleReadVoltage(ADCChannel Channel)
	{
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		ADC12CTL0 = ADC12ON + ADC12MSC;         // Sampling time, ADC12 on
		P6SEL |= 0xFF;                           			 // P6.0 ADC option select
		P7SEL |= 0xF0;
		ADC12MCTL0 = Channel;
		ADC12CTL1 = ADC12SHP;                     			// Use sampling timer
		ADC12IE = ADC12IFG0;                           		// Enable interrupt
		ADC12CTL0 |= ADC12ENC;
	    ADC12CTL0 |= ADC12SC;                   			// Start sampling/conversion
		while(!ADCInterruptFlag);
		ADCInterruptFlag = 0;
		ADC12CTL0 &= ~ADC12ENC;
		return (((double)Value*3.3)/4095);
	}

	unsigned char i = 0;
	#pragma vector = ADC12_VECTOR
	interrupt void ADC12_ISR(void)
	{
		switch(__even_in_range(ADC12IV,34))
		{
		case  0: break;                           // Vector  0:  No interrupt
		case  2: break;                           // Vector  2:  ADC overflow
		case  4: break;                           // Vector  4:  ADC timing overflow

		case  6:                           // Vector  6:  ADC12IFG0
			Value = ADC12MEM0;
			ADCInterruptFlag = 1;
			break;
		default: break;
		}
	}
} /* namespace MSP430 */
