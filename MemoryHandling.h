/*
 * MemoryHandling.h
 *
 *  Created on: 04 Tem 2014
 *      Author: Deniz
 */
#ifndef MEMORYHANDLING_H_
#define MEMORYHANDLING_H_


enum Operation_Mode {Idle, Diagnostic, DataAcquisition, DataProcessing}; // Operation MOdes of XRD
enum StatusReports {RENA_OK, RENA_FAIL, RENA_FATAL, ADC_OK, ADC_FAIL, ADC_FATAL,
					ETH_OK, ETH_FAIL, ETH_FATAL, ICR_OK, ICR_FAIL, ICR_FATAL, HV_OK, HV_FAIL, HV_FATAL,
					TCR_OK, TCR_FAIL, TCR_FATAL, MultipleAnodes, MultipleCathodes, CathodeOnly, AnodeOnly};
/*
 * Function Prototypes
 */
// INFINITE LOOP!!!!! NEEDS TIMEOUT!!!!!
void fat_init(void);



/*NEEDS TO BE UPDATED!!*/
unsigned char GetSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode &LastOperationMode,
							unsigned int &ExecutionNumber, unsigned int &WDTNumber,
							unsigned long &RawDataNumber, unsigned long &SpectrumSingleNumber,
							unsigned long &SpectrumDoubleNumber, unsigned long &ConfigNumber);
/*NEEDS TO BE UPDATED!!*/
unsigned char UpdateSystemInfo(unsigned char* RTC_AsCharArray, Operation_Mode LastOperationMode,
							unsigned int ExecutionNumber, unsigned int WDTNumber,
							unsigned long RawDataNumber, unsigned long SpectrumSingleNumber,
							unsigned long SpectrumDoubleNumber, unsigned long ConfigNumber);

unsigned char AddRawData(unsigned char* RawData, unsigned int DataLength, unsigned long FileNumber);
unsigned char ReadRawData(unsigned char* RawData, unsigned int DataLength, unsigned long Offset, unsigned long FileNumber);
unsigned char ReadSpectrumData(unsigned char* RawData, unsigned int DataLength, unsigned long Offset, unsigned long FileNumber);
unsigned char ReadProcessedData(unsigned char* ProcessedData, unsigned int DataLength, unsigned long Offset, unsigned long FileNumber);
unsigned char AddSpectrumSingleData(unsigned int (*SpectrumData)[100] , unsigned long FileNumber);
unsigned char AddSpectrumDoubleData(unsigned int (*SpectrumData)[100] , unsigned long FileNumber);
unsigned char AddAnodeOnlySpectrumData(unsigned int (*SpectrumData)[100] , unsigned long FileNumber);
unsigned char AddLightCurveData(unsigned int* LightCurveData, unsigned long FileNumber);
unsigned long GetRawDataFileLength(unsigned long RawDataNumber);
unsigned long GetSpectrumDataFileLength(unsigned long SpectrumSingleNumber);
unsigned char ReportEvent(StatusReports report);
unsigned char ReportEvent(StatusReports report, unsigned long EventNumber);
/*
 *
 */

#endif /* MEMORYHANDLING_H_ */
