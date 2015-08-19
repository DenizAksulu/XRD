/*
 * I2C.h
 *
 *  Created on: Aug 16, 2015
 *      Author: Deniz
 */

#ifndef I2C_H_
#define I2C_H_

#include "pulsar.h"
#define I2C_ADDRESS 0x48
#define MAX_I2C_BYTE 256

namespace MSP430
{

class I2C
{
public:
	I2C(BUS bus, void (*I2C_Receive) (unsigned char* Data, unsigned int Length));
	unsigned char Send(unsigned char* Buffer);
	virtual ~I2C();
private:
	BUS selectedBUS;
};

} /* namespace MSP430 */

#endif /* I2C_H_ */
