/*
 * ByteFunctions.cpp
 *
 *  Created on: 10 Tem 2013
 *      Author: Deniz
 */
#include "ByteFunctions.h"

unsigned char Packet[512];

bool CompareCharArrayToString(unsigned char* Array, const char* ConstArray, unsigned int ArrayLength)
{
	for(int i = 0; i < ArrayLength; i++)
	{
		if(Array[i] != ConstArray[i])
		{
			return false;
		}
	}
	return true;
}

unsigned char* AssembleDataPacket(unsigned char* Data, unsigned int Length)
{
	unsigned char charLength[2];
	*(unsigned int*)charLength = Length + 3;
	int i = 0;
	Packet[i++] = 'A';
	Packet[i++] = 'C';
	Packet[i++] = 'Q';
	Packet[i++] = charLength[0];
	Packet[i++] = charLength[1];
	Packet[i++] = 'O';
	Packet[i++] = 'K';
	for(unsigned int j = 0; j < Length; j++)
	{
		Packet[i++] = Data[j];
	}
	Packet[i++] = 'E';
	Packet[i++] = 'N';
	Packet[i++] = 'D';
	return Packet;
}


void UlToStr(char *s, unsigned long bin, unsigned char n)
{
   s += n;
   *s = '\0';

   while (n--)
   {
       *--s = (bin % 10) + '0';
       bin /= 10;
   }
}
