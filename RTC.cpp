/*
 * RTC.cpp
 *
 *  Created on: 19 Tem 2013
 *      Author: Deniz
 */
#include "msp430x54xA.h"
#include "RTC.h"

void (*rtcAlarm_Callback)(void);
void (*Sec_Callback)(void);

void initRTC(int year, int month, int day, int dow, int hour, int min, int sec)
{
	RTCCTL01 |= RTCHOLD + RTCMODE;						// RTC Calender mode enable, RTC hold

	RTCYEAR = year;                         			// Year
	RTCMON = month;                             		// Month
	RTCDAY = day;                            			// Day
	RTCDOW = dow;                            			// Day of week
	RTCHOUR = hour;                           			// Hour
	RTCMIN = min;                            			// Minute
	RTCSEC = sec;                            			// Seconds
}

void startRTC()
{
	RTCCTL01 &= ~(RTCHOLD);                   			// Start RTC calendar mode
}

void stopRTC()
{
	RTCCTL01 |= RTCHOLD;                   			// Stop RTC calendar mode
}

void getRTC(unsigned int *RTC)
{

	*RTC++ = RTCYEAR;
	*RTC++ = RTCMON;
	*RTC++ = RTCDAY;
	*RTC++ = RTCDOW;
	*RTC++ = RTCHOUR;
	*RTC++ = RTCMIN;
	*RTC++ = RTCSEC;

}
void getRTCasByteArray(unsigned char *PACKET)
{
	char byteArray[2];
	*(int*)byteArray = RTCYEAR;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCMON;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCDAY;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCDOW;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCHOUR;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCMIN;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];

	*(int*)byteArray = RTCSEC;
	*PACKET++ = byteArray[0];
	*PACKET++ = byteArray[1];
}

void setAlarm(void (*AlarmCallback)(void), int day, int dow, int hour, int min)
{
	rtcAlarm_Callback = AlarmCallback;
	RTCCTL01 |= RTCAIE;									//Enable Alarm interrupt
	RTCADOWDAY = dow;                         			// Day of week alarm
	RTCADAY = day;                           			// Day Alarm
	RTCAHOUR = hour;                          			// Hour Alarm
	RTCAMIN = min;                           			// Minute Alarm
}

void enableSec(void (*SecCallback)(void))
{
	Sec_Callback = SecCallback;
	RTCCTL01 |= RTCRDYIE;
}

void disableSec()
{
	RTCCTL01 &= ~RTCRDYIE;
}

#pragma vector=RTC_VECTOR
interrupt void RTC_ISR(void)
{
	switch(__even_in_range(RTCIV,16))
	  {
	    case RTC_NONE:                          		// No interrupts
	      break;
	    case RTC_RTCRDYIFG:                     		// RTCRDYIFG
	    	__bis_SR_register(GIE);
	    	RTCCTL01 &= ~RTCRDYIFG;
	    	Sec_Callback();
	      break;
	    case RTC_RTCTEVIFG:                     		// RTCEVIFG
	      break;
	    case RTC_RTCAIFG:                       		// RTCAIFG
	    	RTCCTL01 &= ~RTCAIFG;
	    	rtcAlarm_Callback();
	      break;
	    case RTC_RT0PSIFG:                      		// RT0PSIFG
	      break;
	    case RTC_RT1PSIFG:                      		// RT1PSIFG
	      break;
	    case 12: break;                         		// Reserved
	    case 14: break;                         		// Reserved
	    case 16: break;                         		// Reserved
	    default: break;
	  }
}




