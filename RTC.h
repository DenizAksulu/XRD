/*
 * RTC.h
 *
 *  Created on: 19 Tem 2013
 *      Author: Deniz
 */

#ifndef RTC_H_
#define RTC_H_

void initRTC(int year, int month, int day, int dow, int hour, int min, int sec);
void startRTC();
void stopRTC();
void getRTC(unsigned int *RTC);
void getRTCasByteArray(unsigned char *PACKET);
void setAlarm(void (*AlarmCallback)(void), int day, int dow, int hour, int min);
void enableSec(void (*SecCallback)(void));
void disableSec();


#endif /* RTC_H_ */
