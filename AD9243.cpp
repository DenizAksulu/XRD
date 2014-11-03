/*
 * AD9243.cpp
 *
 *  Created on: 20 Þub 2014
 *      Author: Asus
 */

#include "AD9243.h"

namespace MSP430 {

AD9243::AD9243(pulsar* bits, pulsar* clock, pulsar* otr)
{
	SelectedBits=bits;
	SelectedClock=clock;
	SelectedOTR=otr;
	SelectedClock->set(0);
}

unsigned char AD9243::ReadValue(unsigned int* value)
{
	*value = 0;
	SelectedClock->set(1);
	if(!SelectedOTR->get())
	{
		for(int i=0; i<14; i++)
		{
			*value += SelectedBits[i].get() * (0x01 << (13 - i));
		}
		SelectedClock->set(0);
		return 1;
	}

	SelectedClock->set(0);
	return 0;

}
unsigned char AD9243::ReadVoltage(double* voltage)
{
	unsigned int value;

	if(!ReadValue(&value))
		return 0;

	*voltage = ((double) value / 16383);
	return 1;
}

unsigned char AD9243::CheckAD9243()
{
	return 1;
}

} /* namespace MSP430 */
