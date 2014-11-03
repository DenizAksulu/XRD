/*
 * MemoryHandling.h
 *
 *  Created on: 04 Tem 2014
 *      Author: Deniz
 */
#ifndef MEMORYHANDLING_H_
#define MEMORYHANDLING_H_


enum Operation_Mode {Idle, Diagnostic, DataAcquisition}; // Operation MOdes of XRD
enum StatusReports {RENA_OK, RENA_FAIL, RENA_FATAL, ADC_OK, ADC_FAIL, ADC_FATAL,
					ETH_OK, ETH_FAIL, ETH_FATAL, ICR_OK, ICR_FAIL, ICR_FATAL, HV_OK, HV_FAIL, HV_FATAL,
					TCR_OK, TCR_FAIL, TCR_FATAL};
/*
 * Function Prototypes
 */
void fat_init(void);

unsigned char GetSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode &LastOperationMode,
							unsigned int &ExecutionNumber, unsigned int &WDTNumber,
							unsigned long &RawDataNumber, unsigned long &SpectrumSingleNumber,
							unsigned long &SpectrumDoubleNumber, unsigned long &ConfigNumber);

unsigned char UpdateSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode LastOperationMode,
							unsigned int ExecutionNumber, unsigned int WDTNumber,
							unsigned long RawDataNumber, unsigned long SpectrumSingleNumber,
							unsigned long SpectrumDoubleNumber, unsigned long ConfigNumber);

unsigned char AddRawData(unsigned char* RawData, unsigned int DataLength, unsigned long FileNumber);

unsigned char ReportEvent(StatusReports report);
/*
 *
 */

#endif /* MEMORYHANDLING_H_ */
