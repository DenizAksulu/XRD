/*
 * Timer.h
 *
 *  Created on: 11 Tem 2013
 *      Author: Deniz
 */

/*!\file Timer.h
 * \brief Contains the functions for the TimerA0, TimerA1 and TimerB0 modules.
 *
 * Contains the functions needed for initialization, start and stop of the timers.
 * Also contains the interrupts and counter for the Timers.
 */
#ifndef TIMER_H_
#define TIMER_H_
#define CLOCK_FREQ_TIMER 25000
namespace MSP430
{
	/*! \enum TIMER
	 *	\brief Select which timer to use
	 *
	 *	Note that TimerA0 and TimerA0_1, TimerA1 and TimerA1_1 share the same interrupts.
	 */
	enum TIMER {TimerA0, TimerA1, TimerB0, TimerA0_1, TimerA1_1};

	/*! \class Timer Timer.h "\Timer.h"
	 *	\brief Creates a new timer object
	 *
	 */
	class Timer
	{
		public:
			/*! \fn Timer(TIMER timer, void (*TimerInterruptFunction)(void))
			 *  \brief The constructor of the Timer class
			 *
			 * 	Selects the clock (SMCLK) and timer mode (up mode) of the timer.
			 *	\param timer The timer which is going to be initialized
			 *	\param (*TimerInterruptFunction)(void) The pointer of the function which is going to be called when an interrupt occurs
			 */

			Timer(TIMER timer, void (*TimerInterruptFunction)(void));

			/*! \fn void Start(unsigned int Period)
			 *  \brief Enables the timeout interrupt of the specified timer.
			 *
			 *	When this function is called the timer starts counting and when there is an timeout the specified function will be called.
			 * 	Is designed for the operating frequency of 1MHz. If the operating freqency changes the TxxCCRx should also change (Frequency/1000).
			 *	\param Period The timeout period in miliseconds
			 */
			void Start(unsigned int Period);

			/*! \fn void Stop()
			 *  \brief Disables the timeout interrupt of the specified timer.
			 *
			 *  Disables the interrupt and also resets the counter of the specified timer.
			 */
			void Stop();

			/*! \var TIMER SelectedTimer
			 *	\brief The specified timer
			 */
			TIMER SelectedTimer;
	};

} /* namespace MSP430 */
#endif /* TIMER_H_ */
