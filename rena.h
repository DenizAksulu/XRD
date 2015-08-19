/*
 * rena.h
 *
 *  Created on: 05 Þub 2014
 *      Author: Asus
 */
#include "main.h"
#include "pulsar.h"
#include "ADC.h"
#ifndef RENA_H_
#define RENA_H_

namespace MSP430 {



struct channelconfig
{
	unsigned address : 6;
	unsigned FB_TC : 1;
	unsigned ECAL : 1;
	unsigned FPDWN : 1;
	unsigned FETSEL : 1;
	unsigned gainselect : 2;
	unsigned PDWN : 1;
	unsigned PZSEL : 1;
	unsigned RANGE : 1;
	unsigned RSEL : 1;
	unsigned SEL : 4;
	unsigned SIZEA : 1;
	unsigned DF : 8;
	unsigned POL : 1;
	unsigned DS : 8;
	unsigned ENF : 1;
	unsigned ENS :1;
	unsigned FM :1;
};



class rena {
public:
	rena();
	void ConfigureRena(channelconfig*, unsigned char, pulsar, pulsar, pulsar);
	void ReloadSIN(unsigned long pulsewidth, unsigned int clockcount, pulsar shrclk, pulsar sin, unsigned char* triggeredchannels);
	unsigned char StandardPeakDetectionSequance(pulsar acq, pulsar cls, pulsar ts, unsigned int clspulsewidth, unsigned int traptime, unsigned int timoutdelay, unsigned int acqdelay);
	unsigned char CheckConfig();
	double THRESH35;
};

} /* namespace msp430 */
#endif /* RENA_H_ */
