/*
 * I2C.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: Deniz
 */

#include "I2C.h"

namespace MSP430
{

unsigned char *PTxData;
unsigned char RxData[MAX_I2C_BYTE] = {};                     // Pointer to RX data
unsigned char RXByteCtr = 0;
bool busy = false;
void (*I2C_RECEIVE) (unsigned char* Data, unsigned int Length);

I2C::I2C(BUS bus, void (*I2C_Receive) (unsigned char* Data, unsigned int Length))
{
	I2C_RECEIVE = I2C_Receive;
	selectedBUS = bus;
	switch(selectedBUS)
	{
	case UCB0:
		UCB0CTL1 |= UCSWRST; 			// Reset I2C bus
		P3SEL |= 0x06;       			// Assign I2C pins to USCI_B0
		UCB0CTL0 |= UCSYNC + UCMODE_3; 	// I2C slave, 7-bit addressing
		UCB0CTL1 |= UCSSEL_2; 			// SMClock as BR
		//UCB0BR0 = 63; 					// 25 MHz to 400 kHz ??
		//UCB0BR1 = 0; 					// 25 MHz to 400 kHz ??
		UCB0I2COA = I2C_ADDRESS;		// Set I2C address
		UCB0CTL1 &= ~UCSWRST; 			// Start I2C bus
		UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE + UCRXIE; // Interrupt enable
		break;
	}

}
unsigned char I2C::Send(unsigned char* Buffer)
{
	PTxData = Buffer;
	//while(!busy);		// Timeout required!
	return 1;
}
I2C::~I2C()
{

}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	switch(__even_in_range(UCB0IV,12))
	{
	case  0: break;                           		// Vector  0: No interrupts
	case  2: break;                           		// Vector  2: ALIFG
	case  4: break;                           		// Vector  4: NACKIFG
	case  6:                                  		// Vector  6: STTIFG
		UCB0IFG &= ~UCSTTIFG;                   	// Clear start condition int flag
		RXByteCtr = 0;
		busy = 1; 									// Bus busy
		break;
	case  8:                                  		// Vector  8: STPIFG
		UCB0IFG &= ~UCSTPIFG;                   	// Clear stop condition int flag
		if(RXByteCtr)
			I2C_RECEIVE(RxData, RXByteCtr);
		busy = 0;									// Transaction completed
		break;
	case 10: 										// Vector 10: RXIFG
		RxData[RXByteCtr++] = UCB0RXBUF;
		break;
	case 12:                                  		// Vector 12: TXIFG
		UCB0TXBUF = *PTxData++;                 	// Transmit data at address PTxData
		break;
	default: break;
	}
}

} /* namespace MSP430 */
