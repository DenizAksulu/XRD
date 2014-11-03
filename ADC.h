/*
 * ADC.h
 *
 *  Created on: 10 Tem 2013
 *      Author: Deniz
 */

#include <msp430f5438a.h>

/*!\file ADC.h
 * \brief Contains the functions for the ADC of the MSP430F5438A and the interrupt for the ADC.
 */
#ifndef ADC_H_
#define ADC_H_
namespace MSP430
{
	/*!\enum ADCChannel
	 * \brief Represents the channels of the ADC
	 *
	 * Contains the channels through 1 to 16. Their values are equal to their channel select value.
	 */
	enum ADCChannel {
		Channel1 = ADC12INCH_0, Channel2= ADC12INCH_1, Channel3= ADC12INCH_2, Channel4= ADC12INCH_3,
		Channel5= ADC12INCH_4, Channel6= ADC12INCH_5, Channel7= ADC12INCH_6, Channel8= ADC12INCH_7,
		Channel9= ADC12INCH_8, Channel10= ADC12INCH_9, Channel11= ADC12INCH_10, Channel12= ADC12INCH_11,
		Channel13= ADC12INCH_12, Channel14= ADC12INCH_13, Channel15= ADC12INCH_14, Channel16= ADC12INCH_15
	};

	/*!\class ADC
	 * \brief Initializes the ADC of the MSP430F5438A.
	 *
	 * Only needs to be defined once. One object can be used to access all of the channels of the MSP430F5438A.
	 */
	class ADC
	{
		public:
			/*!\fn ADC()
			 * \brief The only constructor of the ADC class.
			 *
			 * Does not do anything. The constructor is empty.
			 */
			ADC();

			/*!\fn unsigned char ReadAllValuesToCharArray(unsigned char* Channels)
			 * \brief Reads all of the raw ADC values to a char array.
			 *
			 * The char array is formatted, LSB first and starts from Channel 1. Uses the ADC12 interrupt.
			 * \param The results of the conversion.
			 */
			unsigned char ReadAllValuesToCharArray(unsigned char* Channels);

			/*!\fn unsigned char* SingleReadValueAsCharArray(ADCChannel Channel)
			 * \brief Reads the specified channel and outputs a byte array.
			 *
			 * The byte array is formatted, LSB first and contains 2 bytes.
			 * \param Channel The channel that is going to be read.
			 * \return Returns the pointer of the char array.
			 */
			unsigned char* SingleReadValueAsCharArray(ADCChannel Channel);

			/*!\fn unsigned int SingleReadValue(ADCChannel Channel)
			 * \brief This function outputs a single ADC value of the specifed channel.
			 *
			 * \param Channel The channel that is going to be read.
			 * \return Returns the ADC value as an unsigned integer.
			 */
			unsigned int SingleReadValue(ADCChannel Channel);

			/*!\fn double SingleReadVoltage(ADCChannel Channel)
			 * \brief This function outputs a single ADC voltage value of the specifed channel.
			 *
			 * \param Channel The channel that is going to be read.
			 * \return Returns the ADC voltage value as a double.
			 */
			double SingleReadVoltage(ADCChannel Channel);
	};

} /* namespace MSP430 */
#endif /* ADC_H_ */
