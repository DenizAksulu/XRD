/*
 * Timer.cpp
 *
 *  Created on: 11 Tem 2013
 *      Author: Deniz
 */

#include "Timer.h"
#include "msp430f5438a.h"

namespace MSP430
{
	unsigned char SelectedTA0Timer = 0;
	unsigned char SelectedTA1Timer = 0;
	void (*TimerA0Interrupt)(void);
	unsigned int TA0Period = 0;
	unsigned int TA0Counter = 0;
	void (*TimerA1Interrupt)(void);
	unsigned int TA1Period = 0;
	unsigned int TA1Counter = 0;
	void (*TimerB0Interrupt)(void);
	unsigned int TB0Period = 0;
	unsigned int TB0Counter = 0;
	void (*TimerA0_1Interrupt)(void);
	unsigned int TA0_1Period = 0;
	unsigned int TA0_1Counter = 0;
	void (*TimerA1_1Interrupt)(void);
	unsigned int TA1_1Period = 0;
	unsigned int TA1_1Counter = 0;

	Timer::Timer(TIMER timer, void (*TimerInterruptFunction)(void))
	{
		SelectedTimer = timer;
		switch(SelectedTimer)
		{
		case TimerA0:
			TimerA0Interrupt = TimerInterruptFunction;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
			break;
		case TimerA0_1:
			TimerA0_1Interrupt = TimerInterruptFunction;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
			break;
		case TimerA1:
			TimerA1Interrupt = TimerInterruptFunction;
			TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
			break;
		case TimerA1_1:
			TimerA1_1Interrupt = TimerInterruptFunction;
			TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
			break;
		case TimerB0:
			TimerB0Interrupt = TimerInterruptFunction;
			TB0CTL = TBSSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
			break;
		}
	}

	void Timer::Start(unsigned int Period)
	{
		switch(SelectedTimer)
		{
		case TimerA0:
			TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
			TA0CCR0 = 1000;
			TA0Period = Period;
			TA0Counter = 0;
			TA0_1Counter = 0;
			SelectedTA0Timer = 1;
			break;
		case TimerA0_1:
			TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
			TA0CCR0 = 1000;
			TA0_1Period = Period;
			TA0Counter = 0;
			TA0_1Counter = 0;
			SelectedTA0Timer = 2;
			break;
		case TimerA1:
			TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
			TA1CCR0 = 1000;
			TA1Period = Period;
			TA1Counter = 0;
			TA1_1Counter = 0;
			SelectedTA1Timer = 1;
			break;
		case TimerA1_1:
			TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
			TA1CCR0 = 1000;
			TA1_1Period = Period;
			TA1Counter = 0;
			TA1_1Counter = 0;
			SelectedTA1Timer = 2;
			break;
		case TimerB0:
			TB0CCTL0 = CCIE;                          // CCR0 interrupt enabled
			TB0CCR0 = 1000;
			TB0Period = Period;
			TB0Counter = 0;
			break;
		}
	}

	void Timer::Stop()
	{
		switch(SelectedTimer)
		{
		case TimerA0:
			TA0CCTL0 &= ~CCIE;                          // CCR0 interrupt enabled
			TA0Counter = 0;
			TA0_1Counter = 0;
			break;
		case TimerA0_1:
			TA0CCTL0 &= ~CCIE;                          // CCR0 interrupt enabled
			TA0Counter = 0;
			TA0_1Counter = 0;
			break;
		case TimerA1:
			TA1CCTL0 &= ~CCIE;                          // CCR0 interrupt enabled
			TA1Counter = 0;
			TA1_1Counter = 0;
			break;
		case TimerA1_1:
			TA1CCTL0 &= ~CCIE;                          // CCR0 interrupt enabled
			TA1Counter = 0;
			TA1_1Counter = 0;
			break;
		case TimerB0:
			TB0CCTL0 &= ~CCIE;                          // CCR0 interrupt enabled
			TB0Counter = 0;
			break;
		}
	}


	//! Timer A0 interrupt service routine
	#pragma vector=TIMER0_A0_VECTOR
	interrupt void TIMER0_A0_ISR(void)
	{
		__bis_SR_register(GIE);
		if(SelectedTA0Timer == 1)
		{
			TA0Counter++;
			if(TA0Counter == TA0Period)
			{
				TimerA0Interrupt();
				TA0Counter = 0;
			}
		}
		else if(SelectedTA0Timer == 2)
		{
			TA0_1Counter++;
			if(TA0_1Counter == TA0_1Period)
			{
				TimerA0_1Interrupt();
				TA0_1Counter = 0;
			}
		}
	}
	//! Timer A1 interrupt service routine
	#pragma vector=TIMER1_A0_VECTOR
	interrupt void TIMER1_A1_ISR(void)
	{
		__bis_SR_register(GIE);
		if(SelectedTA1Timer == 1)
		{
			TA1Counter++;
			if(TA1Counter == TA1Period)
			{
				TimerA1Interrupt();
				TA1Counter = 0;
			}
		}
		else if(SelectedTA1Timer == 2)
		{
			TA1_1Counter++;
			if(TA1_1Counter == TA1_1Period)
			{
				TimerA1_1Interrupt();
				TA1_1Counter = 0;
			}
		}
	}

	//! Timer B0 interrupt service routine
	#pragma vector=TIMER0_B0_VECTOR
	interrupt void TIMER0_B0_ISR(void)
	{
		__bis_SR_register(GIE);
		TB0Counter++;
		if(TB0Counter == TB0Period)
		{
			TimerB0Interrupt();
			TB0Counter = 0;
		}
	}

} /* namespace MSP430 */
