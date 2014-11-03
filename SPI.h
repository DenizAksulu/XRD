/*
 * SPI.h
 *
 *  Created on: 11 Tem 2013
 *      Author: Deniz
 */

/*!\file SPI.h
 * \brief Contains the SPI module initializer for UCB0 module.
 */
#ifndef SPI_H_
#define SPI_H_
#include "main.h"
#include "pulsar.h"

namespace MSP430
{
	/*!\enum BUS
	 * \brief The available SPI buses for initialization.
	 *
	 * Only contains UCB0 in this version.
	 */

	/*!\class SPI
	 * \brief Initializes the specified SPI module upon creating an object.

	 * Includes both Read and Send functions. Does not use any interrupts. Only checks for interrupt flags.
	 */
	class SPI
	{
		public:
			/*!\fn SPI(BUS bus)
			 * \brief The only constructor for the SPI class.
			 *
			 * The SPI pins and registers are adjusted. The clock divider is set to 2
			 * and the clock source is SMCLK. When changing the clock frequencies these specifications must not be dismissed.
			 */
		    SPI(BUS bus, pulsar* ChipSelect);

			/*!\fn unsigned char Send(unsigned char* Data, unsigned int Length)
			 * \brief Sends the specified amount of the specified data via SPI.
			 *
			 * \param Data The pointer of the data that is going to be sent.
			 * \param Length The amount of the bytes that are going to be sent.
			 * \return Returns '1' if the communication is successfull. If else it will return '0'.
			 */
			unsigned char Send(unsigned char* Data, unsigned int Length);

			/*!\fn unsigned char Get(unsigned char* Data, unsigned int Length)
			 * \brief Gets the specified amount of the specified data via SPI.
			 *
			 * \param Data The received data.
			 * \param Length The amount of bytes that are expected.
			 * \return Returns '1' if the communication is successfull. If else it will return '0'.
			*/
			unsigned char Get(unsigned char* Data, unsigned int Length);

			/*!\fn unsigned char SendWithoutCS(unsigned char* Data, unsigned int Length)
			 * \brief Sends the specified amount of the specified data via SPI.
			 *
			 * \param Data The pointer of the data that is going to be sent.
			 * \param Length The amount of the bytes that are going to be sent.
			 * \return Returns '1' if the communication is successfull. If else it will return '0'.
			 */
			unsigned char SendWithoutCS(unsigned char* Data, unsigned int Length);

			/*!\fn unsigned char GetWithoutCS(unsigned char* Data, unsigned int Length)
			 * \brief Gets the specified amount of the specified data via SPI.
			 *
			 * \param Data The received data.
			 * \param Length The amount of bytes that are expected.
			 * \return Returns '1' if the communication is successfull. If else it will return '0'.
			 */
			unsigned char GetWithoutCS(unsigned char* Data, unsigned int Length);
			BUS SelectedBus;
			pulsar* CS;
	};

} /* namespace MSP430 */
#endif /* SPI_H_ */
