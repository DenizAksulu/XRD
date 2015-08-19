/*
 * ByteFunctions.h
 *
 *  Created on: 10 Tem 2013
 *      Author: Deniz
 */

/*!\file ByteFunctions.h
 * \brief This document contains some useful functions for byte manupulation.
 */

#ifndef BYTEFUNCTIONS_H_
#define BYTEFUNCTIONS_H_

const double MultiplierCoefficientsAnode[15] =
{
		20.34, 21.45, 19.98, 20.42, 21.15, 22.01, 18.85, 19.93,
		20.54, 21.43, 22.44, 18.34, 21.04, 23.35, 22.46
};
const double AddingCoefficientsAnode[15] =
{
		7.45, 6.93, 8.95, 7.03, 8.34, 6.93, 9.12, 7.85, 8.87,
		7.23, 5.67, 10.47, 9.50, 4.45, 5.67
};

/*!\fn unsigned char* AssembleDataPacket(unsigned char* Data, unsigned int Length)
 * \brief Assembles the specified data into a packet with the right format for UART transmission.
 *
 * This function prepares the data for transmission to the OBC. It includes the header and footer of the data format.
 * \param Data The data that is going to be packed.
 * \param Length The length of the data.
 * \return Returns the pointer of the packed data.
 */
unsigned char* AssembleDataPacket(unsigned char* Data, unsigned int Length);

/*!\fn bool CompareCharArrayToString(unsigned char* Array, const char* ConstArray, unsigned int ArrayLength)
 * \brief Compares a unsigned char arrray to a string (const char*).
 * \param Array the unsigned char array that is going to be compared.
 * \param ConstArray the string that is going to be compared.
 * \param ArrayLength The length of the compared arrays.
 * \return 'True' if the arrays are equal, 'False' if else.
 */
bool CompareCharArrayToString(unsigned char* Array, const char* ConstArray, unsigned int ArrayLength);

unsigned char DetermineNumberOfTriggeredChannels(unsigned char* Array, unsigned char ArrayLength);

void UlToStr(char *s, unsigned long bin, unsigned char n);

double ConvertToEnergyAnode(double ADCValue, unsigned char ChannelNumber);
double ConvertToEnergyCathode(double ADCValue, unsigned char ChannelNumber);
#endif /* BYTEFUNCTIONS_H_ */
