/*
 * pulsar.h
 *
 *  Created on: 02 Ara 2013
 *      Author: Asus
 */
#include <msp430F5438a.h>
#include "main.h"
#ifndef PULSAR_H_
#define PULSAR_H_

namespace MSP430 {

enum BUS {UCB0, UCB2, UCA1, UCA2, UCA0, UCA3};
enum InterruptType {RisingEdge, FallingEdge, BothEdges};

class pulsar{
public:
	pulsar(unsigned char* port, unsigned char pin, bool init);
	pulsar(unsigned char* port, unsigned char pin );
	pulsar(unsigned char* port, unsigned char pin, InterruptType type, void (*interruptVector)(void));
	void set(bool value);
	bool get();
	void GenerateSinglePulse(unsigned long pulsewidth);
	void GenerateClock(unsigned long, unsigned int);
	void GenerateSHRClock(unsigned long pulsewidth, unsigned int clockcount, pulsar sout, unsigned char* channels);
	void EnableInterrupt();
private:
	unsigned char* CurrentPort;
	unsigned char CurrentPin;
	unsigned char CurrentDir;
};


} /* namespace msp430 */
#endif /* PULSAR_H_ */
