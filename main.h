/*
 * main.h
 *
 *  Created on: 06 Þub 2014
 *      Author: Asus
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <msp430F5438a.h>
#include <stdint.h>
#include "PARAMETERS.h"
#include "UART.h"
#include "pulsar.h"
#include "rena.h"
#include "ADC.h"
#include "ByteFunctions.h"
#include "AD9243.h"
#include "Timer.h"
#include "RTC.h"
#include "ff.h"
#include "diskio.h"
#include "MemoryHandling.h"

#define CLOCK_DELAY 25000

/*Spectrum Parameters*/
#define MAX_ENERGY_LEVEL 4095
#define MIN_ENERGY_LEVEL 0
#define NUMBER_OF_ENERGY_INTERVALS 100 //100

#define RAW_DATA_HIT_NUMBER 5
#define SPECTRUM_DATA_HIT_NUMBER 60000

/*
 * Function Prototypes
 */
void delay(unsigned int );
void ADCACQTimerInterrupt();
void CommandVector(unsigned char Phase, unsigned char* Data, unsigned int Length);
void RTCSecondInterrupt();
void RunOperationMode(Operation_Mode mode);
/*
 * Enumarations
 */
enum UART_Mode {HeaderError, DataError, Success, NoOp}; // UART Modes for Data Handling


/*!\fn void UARTCommandHandler(UART_Mode mode)
 * \brief This function handles the received command and is regularly called in a while loop.
 *
 * Processes the received data and responds to the commands accordingly.
 * \param mode This parameter should be set to CurrentUARTMode.
 */
void UARTCommandHandler(UART_Mode mode);


#endif /* MAIN_H_ */
