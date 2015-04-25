/*
 * MemoryHandling.cpp
 *
 *  Created on: 04 Tem 2014
 *      Author: Deniz
 */
#include "ff.h"
#include "main.h"
#include "diskio.h"
#include "ByteFunctions.h"
#include "MemoryHandling.h"


FATFS fatfs;                                            /* File system object */
DIRS dir;                                               /* Directory object   */
FRESULT errCode;                                        /* Error code object  */
FIL file;


void fat_init(void)
{
    errCode = (FRESULT)-1;
    while (errCode != FR_OK){                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");				    	//root directory
    }
}

unsigned char GetSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode &LastOperationMode,
							unsigned int &ExecutionNumber, unsigned int &WDTNumber,
							unsigned long &RawDataNumber, unsigned long &SpectrumSingleNumber,
							unsigned long &SpectrumDoubleNumber, unsigned long &ConfigNumber)
{
	unsigned char SDBuffer[35] = {};
	unsigned long Buffer[4] = {};
	unsigned char index = 0;
	unsigned int bytesread = 0;

	if(f_open(&file, "/SYS_INFO.sys", FA_READ) != FR_OK) return 0;

	if(f_read(&file, SDBuffer, 35, &bytesread) != FR_OK) return 0;

	if(f_close(&file) != FR_OK) return 0;

	/*RTC*/
	for(int i = 0; i < 14; i++)
	{
		RTC_AsCharArray[i] = SDBuffer[index++];
	}
	/*Operation Mode*/
	LastOperationMode = (Operation_Mode)SDBuffer[index++];
	/*Execution Number*/
	ExecutionNumber = SDBuffer[index++] + (SDBuffer[index++] << 8);
	/*WDT Number*/
	WDTNumber = SDBuffer[index++] + (SDBuffer[index++] << 8);
	/*Raw Data Number Number*/
	Buffer[0] = SDBuffer[index++];
	Buffer[1] = SDBuffer[index++];
	Buffer[2] = SDBuffer[index++];
	Buffer[3] = SDBuffer[index++];
	RawDataNumber = Buffer[0] + (Buffer[1] << 8) +
			(Buffer[2] << 16) + (Buffer[3] << 24);
	/*Spectrum Single Number Number*/
	Buffer[0] = SDBuffer[index++];
	Buffer[1] = SDBuffer[index++];
	Buffer[2] = SDBuffer[index++];
	Buffer[3] = SDBuffer[index++];
	SpectrumSingleNumber = Buffer[0] + (Buffer[1] << 8) +
			(Buffer[2] << 16) + (Buffer[3] << 24);
	/*Spectrum Double Number Number*/
	Buffer[0] = SDBuffer[index++];
	Buffer[1] = SDBuffer[index++];
	Buffer[2] = SDBuffer[index++];
	Buffer[3] = SDBuffer[index++];
	SpectrumDoubleNumber = Buffer[0] + (Buffer[1] << 8) +
			(Buffer[2] << 16) + (Buffer[3] << 24);
	/*RENA Config Number Number*/
	Buffer[0] = SDBuffer[index++];
	Buffer[1] = SDBuffer[index++];
	Buffer[2] = SDBuffer[index++];
	Buffer[3] = SDBuffer[index++];
	ConfigNumber = Buffer[0] + (Buffer[1] << 8) +
			(Buffer[2] << 16) + (Buffer[3] << 24);

	return 1;
}

unsigned char UpdateSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode LastOperationMode,
							unsigned int ExecutionNumber, unsigned int WDTNumber,
							unsigned long RawDataNumber, unsigned long SpectrumSingleNumber,
							unsigned long SpectrumDoubleNumber, unsigned long ConfigNumber)
{
	unsigned char SDBuffer[35] = {};
	unsigned char index = 0;
	unsigned int byteswritten = 0;
	if(f_open(&file, "/SYS_INFO.sys", FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) return 0;

	/*RTC*/
	for(int i = 0; i < 14; i++)
	{
		SDBuffer[index++] = RTC_AsCharArray[i];
	}
	/*Operation Mode*/
	SDBuffer[index++] = LastOperationMode;
	/*Execution Number*/
	unsigned char ExecutionNumber_AsCharArray[2] = {};
	*(unsigned int*)ExecutionNumber_AsCharArray = ExecutionNumber;
	SDBuffer[index++] = ExecutionNumber_AsCharArray[0];
	SDBuffer[index++] = ExecutionNumber_AsCharArray[1];
	/*WDT Number*/
	unsigned char WDTNumber_AsCharArray[2] = {};
	*(unsigned int*)WDTNumber_AsCharArray = WDTNumber;
	SDBuffer[index++] = WDTNumber_AsCharArray[0];
	SDBuffer[index++] = WDTNumber_AsCharArray[1];
	/*Raw Data Number Number*/
	unsigned char RawDataNumber_AsCharArray[4] = {};
	*(unsigned long*)RawDataNumber_AsCharArray = RawDataNumber;
	SDBuffer[index++] = RawDataNumber_AsCharArray[0];
	SDBuffer[index++] = RawDataNumber_AsCharArray[1];
	SDBuffer[index++] = RawDataNumber_AsCharArray[2];
	SDBuffer[index++] = RawDataNumber_AsCharArray[3];
	/*Spectrum Single Number Number*/
	unsigned char SpectrumSingleNumber_AsCharArray[4] = {};
	*(unsigned long*)SpectrumSingleNumber_AsCharArray = SpectrumSingleNumber;
	SDBuffer[index++] = SpectrumSingleNumber_AsCharArray[0];
	SDBuffer[index++] = SpectrumSingleNumber_AsCharArray[1];
	SDBuffer[index++] = SpectrumSingleNumber_AsCharArray[2];
	SDBuffer[index++] = SpectrumSingleNumber_AsCharArray[3];
	/*Spectrum Double Number Number*/
	unsigned char SpectrumDoubleNumber_AsCharArray[4] = {};
	*(unsigned long*)SpectrumDoubleNumber_AsCharArray = SpectrumDoubleNumber;
	SDBuffer[index++] = SpectrumDoubleNumber_AsCharArray[0];
	SDBuffer[index++] = SpectrumDoubleNumber_AsCharArray[1];
	SDBuffer[index++] = SpectrumDoubleNumber_AsCharArray[2];
	SDBuffer[index++] = SpectrumDoubleNumber_AsCharArray[3];
	/*RENA Config Number Number*/
	unsigned char ConfigNumber_AsCharArray[4] = {};
	*(unsigned long*)ConfigNumber_AsCharArray = ConfigNumber;
	SDBuffer[index++] = ConfigNumber_AsCharArray[0];
	SDBuffer[index++] = ConfigNumber_AsCharArray[1];
	SDBuffer[index++] = ConfigNumber_AsCharArray[2];
	SDBuffer[index++] = ConfigNumber_AsCharArray[3];

	if(f_write(&file, SDBuffer, 35, &byteswritten)) return 0;
	if(f_close(&file) != FR_OK) return 0;
	return 1;
}

unsigned char AddRawData(unsigned char* RawData, unsigned int DataLength, unsigned long FileNumber)
{
	/*Adjusting File Name*/
	unsigned int byteswritten = 0;
	unsigned char index = 0;
	char FileName[11];
	char Number[8];
	FileName[index++] = '/';
	UlToStr(Number, FileNumber, 8);
	for(int i = 0; i < 8; i++)
	{
		FileName[index++] = Number[i];
	}
	FileName[index++] = '.';
	FileName[index++] = 'r';
	FileName[index++] = 'a';
	FileName[index++] = 'w';
	FileName[index++] = '\0';
	/**/
	/*Open File*/
	if(f_open(&file, FileName, FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) return 0;
	/**/
	/*Write into File*/
	DWORD FileLength = f_size(&file);
	if(f_lseek(&file, FileLength) != FR_OK) return 0;
	if(f_write(&file, RawData, DataLength, &byteswritten) != FR_OK) return 0;
	/**/
	if(f_close(&file) != FR_OK) return 0;
	return 1;
}

unsigned char ReportEvent(StatusReports report)
{
	unsigned char index = 0;
	unsigned char RTC_AsCharArray[14];
	unsigned char Report[15] = {};

	getRTCasByteArray(RTC_AsCharArray);
	for(int i = 0; i < 14; i++)
	{
		Report[index++] = RTC_AsCharArray[i];
	}

	Report[index++] = report;
	/*Adjusting File Name*/
	unsigned int byteswritten = 0;
	/**/
	/*Open File*/
	if(f_open(&file, "REPORTS.sys", FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) return 0;
	/**/
	/*Write into File*/
	DWORD FileLength = f_size(&file);
	if(f_lseek(&file, FileLength) != FR_OK) return 0;
	if(f_write(&file, Report, 15, &byteswritten) != FR_OK) return 0;
	/**/
	if(f_close(&file) != FR_OK) return 0;
	return 1;
}




