
#include "main.h"

using namespace MSP430;

void TS_Interrupt(void);

/*OUTPUTS*/
pulsar TCLK((unsigned char*)&P8OUT, 4, 0);
pulsar READ((unsigned char*)&P8OUT, 2, 0);
pulsar SHRCLK((unsigned char*)&P8OUT, 1, 0);
pulsar SIN((unsigned char*)&P7OUT, 8, 0);
pulsar TIN((unsigned char*)&P7OUT, 4, 0);
pulsar CIN((unsigned char*)&P5OUT, 128, 0);
pulsar CSHIFT((unsigned char*)&P5OUT, 64, 0);
pulsar CS((unsigned char*)&P5OUT, 32, 1);		// initial 1
pulsar CLS((unsigned char*)&P8OUT, 8, 0); // deðiþti //LVDS'de ters baðlanmýþ...
pulsar CLF((unsigned char*)&P11OUT, 2, 0);
pulsar ACQ((unsigned char*)&P8OUT, 16, 0); //deðiþti
pulsar ADCCLK((unsigned char*)&P2OUT, 4, 0);
pulsar ENABLE_5V((unsigned char*)&P2OUT, 8, 0);		//BAÞLANGIÇ DURUMUNA DÝKKAT ET
pulsar ENABLE_HV((unsigned char*)&P5OUT, 16, 0); // Pin Numaralarý deðiþecek!!
pulsar ENABLE_ADC((unsigned char*)&P6OUT, 32, 0); // Pin Numaralarý deðiþecek!

/*INPUTS*/
pulsar TS1((unsigned char*)&P2OUT, 16, FallingEdge, &TS_Interrupt);
pulsar TS2((unsigned char*)&P9OUT, 128);
pulsar TOUT((unsigned char*)&P9OUT, 16); // Pin Numarasýna Dikkat!
pulsar SOUT((unsigned char*)&P9OUT, 32);
pulsar OVRFLOW((unsigned char*)&P9OUT, 8);
pulsar OTR((unsigned char*)&P7OUT, 32);
pulsar BITS[14] = {
		pulsar((unsigned char*)&P7OUT, 64),
		pulsar((unsigned char*)&P7OUT, 128),
		pulsar((unsigned char*)&P5OUT, 1),
		pulsar((unsigned char*)&P5OUT, 2),
		pulsar((unsigned char*)&P1OUT, 1),
		pulsar((unsigned char*)&P1OUT, 2),
		pulsar((unsigned char*)&P1OUT, 4),
		pulsar((unsigned char*)&P1OUT, 8),
		pulsar((unsigned char*)&P1OUT, 16),
		pulsar((unsigned char*)&P1OUT, 32),
		pulsar((unsigned char*)&P1OUT, 64),
		pulsar((unsigned char*)&P1OUT, 128),
		pulsar((unsigned char*)&P2OUT, 1),
		pulsar((unsigned char*)&P2OUT, 2)};

struct channelconfig ChannelConfigs[36];

UART_Mode CurrentUARTMode = NoOp;
unsigned int MeasurementPeriod = 0;
unsigned char ReceivedData[512]={};

unsigned char TriggeredChannels[36] = {};
double AcquiredChannelValue[36] = {};
unsigned char HitBuffer[10800] = {};
bool ACQCompleted = false;

void ADCCLOCK(void);
ADC adc;

AD9243 ad9243(BITS, &ADCCLK, &OTR);


Timer ADCACQTimer(TimerA0, &ADCACQTimerInterrupt);

rena RENA;

UART pc(UCA0, 115200, &CommandVector);

unsigned int SpectrumData[36][NUMBER_OF_ENERGY_INTERVALS] = {};

/*
 * System Info Variables
 */
unsigned char RTC_AsCharArray[14] = {0};
unsigned char RTC_AsIntArray[7] = {0};
Operation_Mode LastOperationMode = Idle;
Operation_Mode CurrentOperationMode = Idle;
unsigned int ExecutionNumber = 0; //
unsigned int WDTNumber = 0;
unsigned long RawDataNumber = 0;
unsigned long SpectrumSingleNumber = 0;
unsigned long SpectrumDoubleNumber = 0;
unsigned long ConfigNumber = 0;
/*
 *
 */


unsigned char SDBuffer[512] = {0};

int main(void)
{
	/*
	 * SD Initialization
	 */
	fat_init();
	if(!GetSystemInfo(RTC_AsCharArray, LastOperationMode, ExecutionNumber, WDTNumber,
			      RawDataNumber, SpectrumSingleNumber, SpectrumDoubleNumber, ConfigNumber))
		initRTC(0, 0, 0, 0, 0, 0, 0);
	/*
	 *
	 */

	/*
	 * RTC Initialization
	 */
	else
	{
		initRTC(RTC_AsCharArray[0] + 256*RTC_AsCharArray[1],
				RTC_AsCharArray[2] + 256*RTC_AsCharArray[3],
				RTC_AsCharArray[4] + 256*RTC_AsCharArray[5],
				RTC_AsCharArray[6] + 256*RTC_AsCharArray[7],
				RTC_AsCharArray[8] + 256*RTC_AsCharArray[9],
				RTC_AsCharArray[10] + 256*RTC_AsCharArray[11],
				RTC_AsCharArray[12] + 256*RTC_AsCharArray[13]);
	}
	enableSec(&RTCSecondInterrupt);
	startRTC();

	/*
	 *
	 */

	/*
	 * Execution Number Should be increased!
	 */
	ExecutionNumber += 1;
	TS1.EnableInterrupt();
	/*
	 *
	 */
	/*
	 * Configure RENA
	 */
	/*if(ConfigNumber == 0)
	{
		for(int i = 0; i < 1; i++)
		{
			ChannelConfigs[i].DF = 0;
			ChannelConfigs[i].DS = 150;
			ChannelConfigs[i].ECAL = 1;
			ChannelConfigs[i].ENF = 0;
			ChannelConfigs[i].ENS = 1;
			ChannelConfigs[i].FB_TC = 0;
			ChannelConfigs[i].FETSEL = 0;
			ChannelConfigs[i].FM = 0;
			ChannelConfigs[i].FPDWN = 1;
			ChannelConfigs[i].PDWN = 0;
			ChannelConfigs[i].POL = 1;
			ChannelConfigs[i].PZSEL = 0;
			ChannelConfigs[i].RANGE = 0;
			ChannelConfigs[i].RSEL = 0;
			ChannelConfigs[i].SEL = 13;
			ChannelConfigs[i].SIZEA = 0;
			ChannelConfigs[i].address = i;
			ChannelConfigs[i].gainselect = 3;
		}
		for(int i = 1; i < 36; i++)
		{
			ChannelConfigs[i].DF = 0;
			ChannelConfigs[i].DS = 60;
			ChannelConfigs[i].ECAL = 1;
			ChannelConfigs[i].ENF = 0;
			ChannelConfigs[i].ENS = 0;
			ChannelConfigs[i].FB_TC = 0;
			ChannelConfigs[i].FETSEL = 0;
			ChannelConfigs[i].FM = 0;
			ChannelConfigs[i].FPDWN = 1;
			ChannelConfigs[i].PDWN = 1;
			ChannelConfigs[i].POL = 1;
			ChannelConfigs[i].PZSEL = 0;
			ChannelConfigs[i].RANGE = 0;
			ChannelConfigs[i].RSEL = 0;
			ChannelConfigs[i].SEL = 13;
			ChannelConfigs[i].SIZEA = 0;
			ChannelConfigs[i].address = i;
			ChannelConfigs[i].gainselect = 3;
		}
		RENA.ConfigureRena(ChannelConfigs, 36, CIN, CSHIFT, CS);
	}

	delay(1000);*/
	/*
	 *
	 */
	while(1)
	{
		UARTCommandHandler(CurrentUARTMode);
		RunOperationMode(CurrentOperationMode);
	}
}

void RunOperationMode(Operation_Mode mode)
{
	unsigned char ADC_CheckConter = ADC_CHECK_LIMIT; // look at parameters.h
	unsigned char RENA_CheckCounter = RENA_CHECK_LIMIT; // look at parameters.h
	switch(mode)
	{
	case Idle:
		/* Pinler doðru olmayabilir!!*/
		ENABLE_HV.set(0); // Disable HV
		ENABLE_ADC.set(0); // Disable ADC
		ENABLE_5V.set(1); // Disable 5V
		__bis_SR_register(LPM4_bits); // Sleep
		break;
	case Diagnostic:
		ENABLE_5V.set(1); // Enable 5V
		while(ADC_CheckConter--) // decrease the counter
		{
			ENABLE_ADC.set(1); // Enable ADC
			/* Check ADC */ /* Function is empty... */
			if(ad9243.CheckAD9243())
			{
				ReportEvent(ADC_OK); // Report ADC_OK

				while(RENA_CheckCounter--) // decrease the counter
				{
					ENABLE_5V.set(1); // Enable 5V
					/* Check RENA Config */
					if(RENA.CheckConfig())
					{
						RENA.ConfigureRena(ChannelConfigs, 36, CIN, CSHIFT, CS); // Configure RENA
						ReportEvent(RENA_OK); // Report RENA OK
						CurrentOperationMode = Idle; // Set CurrentOperationMode to Idle
						break; // Get out of switch
					}
					else
					{
						ReportEvent(RENA_FAIL); // Report RENA Fail
						ENABLE_5V.set(0); // Disable 5V
						delay(5000); // Delay duration unknown
					}
					/*************************************************/
				}
				if(RENA_CheckCounter <= 0)
				{
					ReportEvent(RENA_FATAL);
					break;
				}
			}
			else
			{
				ReportEvent(ADC_FAIL); // Report ADC Fail
				ENABLE_ADC.set(0); // Disable ADC
				delay(100); // Delay duration unknown
			}
			/*****************************************/
		}
		if(ADC_CheckConter <= 0)
		{
			ReportEvent(ADC_FATAL); // Report ADC Fail
		}
		CurrentOperationMode = Idle; // Set CurrentOperationMode to Idle
		break;
	case DataAcquisition:
		disableSec();
		for(int i = 0; i < 100; i++)
		{
			CLS.set(1);
			__delay_cycles(500);
			ACQ.set(1);
			READ.set(1);
			CLS.set(0);
			while(!ACQCompleted && CurrentOperationMode == DataAcquisition);
			ACQCompleted = false;
			AddRawData(HitBuffer, 10800, RawDataNumber);
		}
		if(RawDataNumber > 0)
		{
			for(int j = 0; j < 100*100; j++)
			{

			}
		}
		RawDataNumber++;
		enableSec(&RTCSecondInterrupt);
		CurrentOperationMode = Idle; // Set CurrentOperationMode to Idle
		break;
	}
}

void delay(unsigned int wait)
{
	for(unsigned int i = 0; i < wait; i++)
	{
		__delay_cycles(CLOCK_DELAY);
	}
}



void CommandVector(unsigned char Phase, unsigned char* Data, unsigned int Length)
{
	switch(Phase)
	{
		case 2:
			CurrentUARTMode = Success;
			for(int i = 0; i < Length - 3; i++)
			{
				ReceivedData[i] = Data[i];
			}
			break;
		case 10:
			CurrentUARTMode = HeaderError;
			break;
		case 11:
			CurrentUARTMode = DataError;
			break;
		}
}

void UARTCommandHandler(UART_Mode mode)
{
	switch(mode)
	{
	case Success:
		/*Connect Command*/
		if(CompareCharArrayToString(ReceivedData, "Connect", 7))
		{
			pc.Send(AssembleDataPacket((unsigned char*)"OK", 2), 12);
		}
		/*************************************************************/
		/*Connect Command*/
		else if(CompareCharArrayToString(ReceivedData, "GetGPIO", 7))
		{
			int index=7;
			ReceivedData[index++]=CLS.get();
			ReceivedData[index++]=ACQ.get();
			ReceivedData[index++]=TCLK.get();
			ReceivedData[index++]=READ.get();
			ReceivedData[index++]=SHRCLK.get();
			ReceivedData[index++]=SIN.get();
			ReceivedData[index++]=TIN.get();
			ReceivedData[index++]=CIN.get();
			ReceivedData[index++]=CSHIFT.get();
			ReceivedData[index++]=CS.get();

			ReceivedData[index++]=TS1.get();
			ReceivedData[index++]=TS2.get();
			ReceivedData[index++]=TOUT.get();
			ReceivedData[index++]=SOUT.get();
			ReceivedData[index++]=OVRFLOW.get();

			pc.Send(AssembleDataPacket(ReceivedData, index), index+10);
		}
		/*************************************************************/
		/*Get All ADC Command*/
		else if(CompareCharArrayToString(ReceivedData, "GetAllADC", 9))
		{
			unsigned char PACKET[41];
			for(int i = 0; i < 9; i++)
			{
				PACKET[i] = ReceivedData[i];
			}
			unsigned char TempValues[32];
			adc.ReadAllValuesToCharArray(TempValues);
			for (int i = 9; i < 41; i++)
			{
				PACKET[i] = TempValues[i - 9];
			}
			pc.Send(AssembleDataPacket(PACKET, 41), 51);
		}
		/****************************************************************/
		/*Start ADC Acquisition Command*/
		else if(CompareCharArrayToString(ReceivedData, "StartADCACQ", 11))
		{
			MeasurementPeriod = ReceivedData[11] + ReceivedData[12] * 256;
			ADCACQTimer.Start(MeasurementPeriod);
		}
		/*****************************************************************/
		/*Stop ADC Acquisition Command*/
		else if(CompareCharArrayToString(ReceivedData, "StopADCACQ", 10))
		{
			ADCACQTimer.Stop();
		}
		/*****************************************************************/
		/*Send Channel Config Command*/
		if(CompareCharArrayToString(ReceivedData, "SendChannelConfig", 17))
		{
			for(int i = 0; i < 36; i++)
			{
				ChannelConfigs[i].FM = ( ( ReceivedData[6*i+17] & 0x01 ) );
				ChannelConfigs[i].ENS = ( ( ReceivedData[6*i+17] & 0x02 ) >> 1 );
				ChannelConfigs[i].ENF = ( ( ReceivedData[6*i+17] & 0x04 ) >> 2 );
				ChannelConfigs[i].DS = ( ( ReceivedData[6*i+17] & 0xF8 ) >> 3 ) | ( (ReceivedData[6*i+18] & 0x07) << 5);
				ChannelConfigs[i].POL = ( ( ReceivedData[6*i+18] & 0x08 ) >> 3 );
				ChannelConfigs[i].DF = ( ( ReceivedData[6*i+18] & 0xF0 ) >> 4) | ((ReceivedData[6*i+19] & 0x0F) << 4);
				ChannelConfigs[i].SIZEA =  ( ( ReceivedData[6*i+19] & 0x10 ) >> 4 );
				ChannelConfigs[i].SEL = ( ( ReceivedData[6*i+19] & 0xE0 ) >> 5 ) | ( ( ReceivedData[6*i+20] & 0x01 ) << 3 ) ;
				ChannelConfigs[i].RSEL = ( ( ReceivedData[6*i+20] & 0x02 ) >> 1 );
				ChannelConfigs[i].RANGE = ( ( ReceivedData[6*i+20] & 0x04 ) >> 2 );
				ChannelConfigs[i].PZSEL = ( ( ReceivedData[6*i+20] & 0x08 ) >> 3 );
				ChannelConfigs[i].PDWN = ( ( ReceivedData[6*i+20] & 0x10 ) >> 4 );
				ChannelConfigs[i].gainselect = ( ( ReceivedData[6*i+20] & 0x60 ) >> 5 );
				ChannelConfigs[i].FETSEL = ( ( ReceivedData[6*i+20] & 0x80 ) >>7 );
				ChannelConfigs[i].FPDWN= ( ( ReceivedData[6*i+21] & 0x01 ));
				ChannelConfigs[i].ECAL = ( ( ReceivedData[6*i+21] & 0x02 ) >> 1);
				ChannelConfigs[i].FB_TC = ( ( ReceivedData[6*i+21] & 0x04 ) >> 2 );
				ChannelConfigs[i].address = ( ( ReceivedData[6*i+21] & 0xF8 ) >> 3 ) | ( ( ReceivedData[6*i+22] & 0x01 ) << 5 );
			}


			RENA.ConfigureRena(ChannelConfigs, 36, CIN, CSHIFT, CS);
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/
		/*Change Mode Command*/
		if(CompareCharArrayToString(ReceivedData, "ChangeMode", 10))
		{
			if(ReceivedData[10] == 0)
			{
				CurrentOperationMode = Idle;
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
			}
			else if(ReceivedData[10] == 1)
			{
				CurrentOperationMode = Diagnostic;
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
			}
			else if(ReceivedData[10] == 2)
			{
				CurrentOperationMode = DataAcquisition;
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
			}
		}
		/*************************************************************/
		CurrentUARTMode = NoOp;
		break;
	case HeaderError:
		pc.Send((unsigned char*)"HeaderError", 11);
		CurrentUARTMode = NoOp;
		break;
	case DataError:
		pc.Send((unsigned char*)"DataError", 9);
		CurrentUARTMode = NoOp;
		break;
	case NoOp:
		delay(1);
		break;
	}
}

void ADCACQTimerInterrupt()
{
    unsigned char PACKET[41];
    unsigned char* Command = (unsigned char*)"GetAllADC";
    for(int i = 0; i < 9; i++)
    {
        PACKET[i] = Command[i];
    }
    unsigned char TempValues[32];
    adc.ReadAllValuesToCharArray(TempValues);
    for (int i = 9; i < 41; i++)
    {
        PACKET[i] = TempValues[i - 9];
    }
    pc.Send(AssembleDataPacket(PACKET, 41), 51);
}

unsigned int HitNumber = 0;
void TS_Interrupt(void)
{
	unsigned char adcvalue[2] = {};
	unsigned char NumberOfTriggeredChannels = 0;
	if(ACQ.get())
	{
		ACQ.set(0);
		SHRCLK.GenerateSHRClock(1, 36, SOUT, TriggeredChannels);

		RENA.ReloadSIN(1, 36, SHRCLK, SIN, TriggeredChannels);

		NumberOfTriggeredChannels = DetermineNumberOfTriggeredChannels(TriggeredChannels, 36);

		if(NumberOfTriggeredChannels == 0)  // Early Trigger
		{
			CLS.set(1);
			__delay_cycles(500);
			ACQ.set(1);
			READ.set(1);
			CLS.set(0);
			__bis_SR_register(GIE);
			return;
		}

		TIN.set(1);
		__bis_SR_register(GIE);
		for(unsigned int j = 0; j < NumberOfTriggeredChannels; j++)
		{
			*(unsigned int*)adcvalue = adc.SingleReadValue(Channel5);
			HitBuffer[HitNumber*108 + 36 + 2*j] = adcvalue[0];
			HitBuffer[HitNumber*108 + 36 + 2*j + 1] = adcvalue[1];
			TCLK.set(!TCLK.get());
			TCLK.set(!TCLK.get());
		}
		TIN.set(0);
		for(int h = 0; h < 36; h++)
		{
			HitBuffer[HitNumber*108 + h] = TriggeredChannels[h];
		}
		HitNumber++;
		if(HitNumber == 100)
		{
			ACQCompleted = true;
			HitNumber = 0;
			return;
		}

		//delay(1);
		CLS.set(1);
		__delay_cycles(500);
		ACQ.set(1);
		READ.set(1);
		CLS.set(0);
		__bis_SR_register(GIE);
		return;
	}
}

void RTCSecondInterrupt()
{
	getRTCasByteArray(RTC_AsCharArray);
	UpdateSystemInfo(RTC_AsCharArray, LastOperationMode, ExecutionNumber, WDTNumber,
			      RawDataNumber, SpectrumSingleNumber, SpectrumDoubleNumber, ConfigNumber);
}











