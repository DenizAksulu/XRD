/*
 * SPI.cpp
 *
 *  Created on: 11 Tem 2013
 *      Author: Deniz
 */

#include "SPI.h"
#include "msp430f5438a.h"

namespace MSP430
{
	bool UCB0InterruptFlag = false;
	bool UCA1InterruptFlag = false;
	bool UCA3InterruptFlag = false;
	SPI::SPI(BUS bus, pulsar* ChipSelect)
	{
		CS = ChipSelect;
		SelectedBus = bus;
		CS -> set(1);
		switch(SelectedBus)
		{
		case UCB0:
			/*Pin Configurations*/
			P3OUT |= 0x0A;   //CLK, SIMO
			P3DIR |= 0x0A;
			P3SEL |= 0x0E;   //CLK, MISO, SIMO

			UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCB0CTL0 |= UCMST + UCMSB + UCSYNC;     // 3-pin, 8-bit SPI master
			UCB0CTL0 &= ~UCCKPL;
			UCB0CTL1 |= UCSSEL_2 + UCSWRST;              // SMCLK
			UCB0BR0 = 1;                          // UCLK/2
			UCB0BR1 = 0;
			UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			break;

		case UCA1:
			/*Pin Configurations*/
			P3OUT |= 0x40;   //CLK
			P5OUT |= 0x40;   //SIMO
			P3DIR |= 0x40;	 //CLK
			P5DIR |= 0x40;	 //SIMO
			P3SEL |= 0x40;   //CLK
			P5SEL |= 0xC0;   //MISO, SIMO

			UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA1CTL0 |= UCMST + UCMSB + UCSYNC;     // 3-pin, 8-bit SPI master
			UCA1CTL0 &= ~UCCKPL;
			UCA1CTL1 |= UCSSEL_2 + UCSWRST;              // SMCLK
			UCA1BR0 = 1;                          // UCLK/2
			UCA1BR1 = 0;
			UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			break;
		case UCA3:
			/*Pin Configurations*/
			P10OUT |= 0x01;   //CLK
			P10OUT |= 0x10;   //SIMO
			P10DIR |= 0x01;	 //CLK
			P10DIR |= 0x10;	 //SIMO
			P10SEL |= 0x01;   //CLK
			P10SEL |= 0x30;   //MISO, SIMO

			/*Initialize SPI (UCA3) @ 12.5MHz*/
			UCA3CTL1 |= UCSWRST;                     // **Initialize USCI state machine**
			UCA3CTL0 |= UCMST + UCCKPL + UCMSB + UCSYNC;     // 3-pin, 8-bit SPI master
			UCA3CTL1 |= UCSSEL_2 + UCSWRST;              // SMCLK
			UCA3BR0 = 2;                          // UCLK/2
			UCA3BR1 = 0;
			UCA3MCTL |= 0;
			UCA3CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			break;
		}
	}

	unsigned char SPI::Send(unsigned char* Data, unsigned int Length)
	{
		CS -> set(0);
		switch(SelectedBus)
		{
		case UCB0:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCB0IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCB0TXBUF = Data[i];
			}
			break;

		case UCA1:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA1IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA1TXBUF = Data[i];
			}
			break;
		case UCA3:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA3IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA3TXBUF = Data[i];
			}
			break;
		}
		CS -> set(1);
		return 1;
	}

	unsigned char SPI::Get(unsigned char* Data, unsigned int Length)
	{
		CS -> set(0);
		switch(SelectedBus)
		{
		case UCB0:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCB0IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCB0TXBUF = 0x00;
				Timeout = 500;
				while(!(UCB0IFG&UCRXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCB0RXBUF;
				UCB0InterruptFlag = false;
			}
			break;
		case UCA1:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA1IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA1TXBUF = 0x00;
				Timeout = 500;
				while(!(UCA1IFG&UCRXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCA1RXBUF;
				UCA1InterruptFlag = false;
			}
			break;
		case UCA3:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA3IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA3TXBUF = 0x00;
				Timeout = 500;
				while(!(UCA3IFG&UCRXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCA3RXBUF;
				UCA3InterruptFlag = false;
			}
			break;
		}
		CS -> set(1);
		return 1;
	}

	unsigned char SPI::SendWithoutCS(unsigned char* Data, unsigned int Length)
	{
		switch(SelectedBus)
		{
		case UCB0:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCB0IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCB0TXBUF = Data[i];
			}
			break;
		case UCA1:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA1IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA1TXBUF = Data[i];
			}
			break;
		case UCA3:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA3IFG&UCTXIFG))
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA3TXBUF = Data[i];
			}
			break;
		}
		return 1;
	}

	unsigned char SPI::GetWithoutCS(unsigned char* Data, unsigned int Length)
	{
		switch(SelectedBus)
		{
		case UCB0:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCB0IFG&UCTXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				UCB0TXBUF = 0x00;
				Timeout = 500;
				while(!(UCB0IFG&UCRXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCB0RXBUF;
			}
			break;
		case UCA1:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA1IFG&UCTXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				UCA1TXBUF = 0x00;
				Timeout = 500;
				while(!(UCA1IFG&UCRXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCA1RXBUF;
			}
			break;
		case UCA3:
			for(int i = 0; i < Length; i++)
			{
				int Timeout = 500;
				while(!(UCA3IFG&UCTXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				UCA3TXBUF = 0x00;
				Timeout = 500;
				while(!(UCA3IFG&UCRXIFG))
				{
					__delay_cycles(10);
					if(!(Timeout--)) return 0;
				}
				Data[i] = UCA3RXBUF;
			}
			break;
		}
		return 1;
	}
} /* namespace MSP430 */
