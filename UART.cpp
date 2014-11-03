/*
 * UART.cpp
 *
 *  Created on: 05 Tem 2013
 *      Author: Deniz
 */

#include "UART.h"
#include "msp430f5438a.h"
namespace MSP430
{
	void (*CommandVector) (unsigned char Phase, unsigned char* Data, unsigned int Length);
	UART::UART(BUS UCAX, unsigned long BaudRate, void (*commandVector) (unsigned char Phase, unsigned char* Data, unsigned int Length))
	{
		SelectedBus = UCAX;
		CommandVector = commandVector;
		/* Baud Rate Calculations*/
		unsigned int n = 25*(1048576/BaudRate);
		unsigned char char_n[2] = {0, 0};
		*(unsigned int*) char_n = n;
		//**************************
		switch(UCAX)
		{
		case UCA0:
			P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
			UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA0CTL1 |= UCSSEL_2;                     // SMCLK
			UCA0BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA0BR1 = char_n[1];                              // 1MHz 115200
			UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		case UCA1:
			P5SEL |= 0xC0;                             //
			UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA1CTL1 |= UCSSEL_2;                     // SMCLK
			UCA1BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA1BR1 = char_n[1];                              // 1MHz 115200
			UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		case UCA2:
			P9SEL |= 0x30;                              //
			UCA2CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA2CTL1 |= UCSSEL_2;                     // SMCLK
			UCA2BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA2BR1 = char_n[1];                              // 1MHz 115200
			UCA2CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA2IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		}
	}
	UART::UART(BUS UCAX, unsigned long BaudRate)
	{
		SelectedBus = UCAX;
		/* Baud Rate Calculations*/
		unsigned int n = 25*(1048576/BaudRate);
		unsigned char char_n[2] = {0, 0};
		*(unsigned int*) char_n = n;
		//**************************
		switch(UCAX)
		{
		case UCA0:
			P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
			UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA0CTL1 |= UCSSEL_2;                     // SMCLK
			UCA0BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA0BR1 = char_n[1];                              // 1MHz 115200
			UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		case UCA1:
			P5SEL |= 0xC0;                             //
			UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA1CTL1 |= UCSSEL_2;                     // SMCLK
			UCA1BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA1BR1 = char_n[1];                              // 1MHz 115200
			UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		case UCA2:
			P9SEL |= 0x30;                             //
			UCA2CTL1 |= UCSWRST;                      // **Put state machine in reset**
			UCA2CTL1 |= UCSSEL_2;                     // SMCLK
			UCA2BR0 = char_n[0];                              // 1MHz 115200 (see User's Guide)
			UCA2BR1 = char_n[1];                              // 1MHz 115200
			UCA2CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
			UCA2IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
		}
	}

	unsigned char UART::Send(unsigned char* Data, unsigned int Count)
	{
		unsigned int i;
		switch(SelectedBus)
		{
		case UCA0:

			for(i = 0; i < Count; i++)
			{
				int Timeout = 500;
				while (!(UCA0IFG & UCTXIFG))             // USCI_A0 TX buffer ready?
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA0TXBUF = Data[i];
			}
			break;
		case UCA1:

			for(i = 0; i < Count; i++)
			{
				int Timeout = 500;
				while (!(UCA1IFG & UCTXIFG))             // USCI_A0 TX buffer ready?
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA1TXBUF = Data[i];
			}
			break;
		case UCA2:

			for(i = 0; i < Count; i++)
			{
				int Timeout = 500;
				while (!(UCA2IFG & UCTXIFG))             // USCI_A0 TX buffer ready?
				{
					__delay_cycles(100);
					if(!(Timeout--)) return 0;
				}
				UCA2TXBUF = Data[i];
			}
			break;
		}
		return 1;
	}


	/*! The UART receive interrupt for the OBC communication
	 *
	 *
	 *
	 */
	unsigned int TotalData = 0;
	unsigned int DataCounter = 0;
	unsigned int HeaderCounter = 0;
	unsigned char DataReady = 0;
	unsigned char IsDataExcpected = 0;
	unsigned char HeaderBuffer[7];
	unsigned char DataBuffer[512];
	unsigned char UART_Phase = 1;
	/*UCA2 UCA0 oldu*/
	#pragma vector=USCI_A0_VECTOR
	interrupt void USCI_A0_ISR(void)
	{
		switch(__even_in_range(UCA0IV,4))
		{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
			switch(UART_Phase)
			{
			case 1:		//Header
				HeaderBuffer[HeaderCounter] = UCA0RXBUF;
				HeaderCounter++;
				/*!Header check
				 *
				 *
				 */
				if(HeaderCounter == 7)
				{
					if((HeaderBuffer[0] == 'A') && (HeaderBuffer[1] == 'C') && (HeaderBuffer[2] == 'Q') && (HeaderBuffer[5] == 'O') && (HeaderBuffer[6] == 'K'))
					{
						TotalData = HeaderBuffer[3] + HeaderBuffer[4]*256;
						UART_Phase = 2;
						HeaderCounter = 0;
					}
					else
					{
						UART_Phase = 10;
						HeaderCounter = 0;
					}
				}
				else if(HeaderCounter > 7)
				{
					UART_Phase = 10;
					HeaderCounter = 0;
				}
				break;
			case 2:		//Data
				DataBuffer[DataCounter] = UCA0RXBUF;
				DataCounter++;
				/* Data Check
				 *
				 *
				 */
				if(DataCounter == TotalData)
				{
					if((DataBuffer[TotalData - 1] == 'D') && (DataBuffer[TotalData - 2] == 'N') && (DataBuffer[TotalData - 3] == 'E'))
					{
						CommandVector(UART_Phase, DataBuffer, TotalData);
						UART_Phase = 1;
						DataCounter = 0;
						__bic_SR_register_on_exit(LPM4_bits);
					}
					else
					{
						UART_Phase = 11;
						DataCounter = 0;
					}
				}
				else if(DataCounter > TotalData)
				{
					UART_Phase = 11;
					DataCounter = 0;
				}
				break;
			case 10:
				//Header Error
				CommandVector(UART_Phase, (unsigned char*) "Error!", 0);
				DataCounter = 0;
				UART_Phase = 1;
				__bic_SR_register_on_exit(LPM4_bits);
				break;
			case 11:
				//Data Error
				CommandVector(UART_Phase, (unsigned char*) "Error!", 0);
				DataCounter = 0;
				UART_Phase = 1;
				__bic_SR_register_on_exit(LPM4_bits);
				break;
			}
			break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
		}
	}
} /* namespace MSP430 */
