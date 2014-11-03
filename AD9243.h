/*
 * AD9243.h
 *
 *  Created on: 20 Þub 2014
 *      Author: Asus
 */

#include "main.h"
#include "pulsar.h"
#include "Timer.h"
#ifndef AD9243_H_
#define AD9243_H_

namespace MSP430 {

class AD9243
{
public:
	AD9243(pulsar* bits, pulsar* clock, pulsar* otr);
	unsigned char CheckAD9243();
	unsigned char ReadValue(unsigned int* value);
	unsigned char ReadVoltage(double* voltage);

private:
	pulsar* SelectedBits;
	pulsar* SelectedClock;
	pulsar* SelectedOTR;

};

} /* namespace MSP430 */
#endif /* AD9243_H_ */
