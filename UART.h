/*
 * UART.h
 *
 *  Created on: 05 Tem 2013
 *      Author: Deniz
 */

/*!\file UART.h
 * \brief Contains the UART module initializer for UCA0 and UCA1 modules and some global enumarators.
 *
 * Also contains the receive interrupt for the UCA0 (OBC) module. Does not contain the receive interrupt for the UCA1 (Spectrometer) module!
 * The Spectrometer receive interrupt is located in \file Spectrometer.cpp
 *
 * Does not contain any other inclusions.
 */

#ifndef UART_H_
#define UART_H_
#include "pulsar.h"
namespace MSP430
{

	/*!\enum PacketType
	 * \brief Indicates the possible data packet types.
	 *
	 * Is defined on the UART.h file and is a part of the MSP430 namespace.
	 */
	enum PacketType {ADCPacket = 1, ThermoPacket = 2, SpectroPacket = 3};

	/*!\enum UCAx
	 * \brief Indicates the possible UART modules.
	 *
	 * Is defined on the UART.h file and is a part of the MSP430 namespace.
	 * Only has the members UCA0 and UCA1.
	 * UCA0 is for the OBC communication.
	 * UCA1 is for the Spectrometer communication.
	 */
	//enum UCAx {UCA0};

	/*!\class UART
	 * \brief Initializes the specified UART module upon creating an object.
	 *
	 * Does not include a Read function. The UART library only operates with previously defined interrupts
	 * and is specifically designed for the Acquisition Board application
	 *
	 * Includes Send function.
	 */
	class UART
	{
		public:
			/*!\fn UART(UCAx UCAX, unsigned long BaudRate, void (*commandVector) (unsigned char Phase, unsigned char* Data, unsigned int Length))
			 * \brief The first overload of the constructor for the UART class.
			 *
			 * This overload is used for the initialization of the UCA0 module for the OBC communication.
			 * \param UCAX Indicates the bus that is going to be initialized. Should be UCA0.
			 * \param BaudRate Indicates the wanted baud rate of the UART communication. Depends on the SMCLK frequency.
			 * Should be updated upon changing the frequency.
			 * \param commandVector The pointer of the function that is going to be called upon receiveing data.
			 * The parameters of the function indicate wheter the received data fits the specified format or not
			 * and the parameters also contain the received data and its length.
			 */
			UART(BUS UCAX, unsigned long BaudRate, void (*commandVector) (unsigned char Phase, unsigned char* Data, unsigned int Length));

			/*!\fn UART(UCAx UCAX, unsigned long BaudRate)
			 * \brief The second overload of the constructor for the UART class.
			 *
			 * This overload is used for the initialization of the UCA1 module for the spectrometer communication.
			 * It does not contain any function pointer among its parameters.
			 * However, there is a function that is going to be called upon receiving data
			 * from the spectrometer. This function pointer is set upon the initialization of the spectrometer. See Spectrometer.h
			 * \param UCAX Indicates the bus that is going to be initialized. Should be UCA1.
			 * \param BaudRate Indicates the wanted baud rate of the UART communication. Depends on the SMCLK frequency.
			 * Should be updated upon changing the frequency.
			 */
			UART(BUS UCAX, unsigned long BaudRate);

			/*!\fn unsigned char Send(unsigned char* Data, unsigned int Count);
			 * \brief Sends the specified data via UART.
			 *
			 * This function is accesible via all of the overloads of the UART class. It sends the specified amount of the
			 * specified data with the baudrate of the object.
			 * \param Data The data that is going to be sent.
			 * \param Count The amount of bytes that are goþng to be sent.
			 */
			unsigned char Send(unsigned char* Data, unsigned int Count);

			BUS SelectedBus;
	};
} /* namespace MSP430 */
#endif /* UART_H_ */
