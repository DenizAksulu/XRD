
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

pulsar ENABLE_5V((unsigned char*)&P2OUT, 8, 0);		//BAÞLANGIÇ DURUMUNA DÝKKAT ET
pulsar ENABLE_HV((unsigned char*)&P5OUT, 16, 0); // Pin Numaralarý deðiþecek!!
pulsar ENABLE_HV_POWER((unsigned char*)&P10OUT, 4, 1); // Pin Numaralarý deðiþecek!!
pulsar ENABLE_ADC((unsigned char*)&P6OUT, 32, 0); // Pin Numaralarý deðiþecek!
pulsar TEST((unsigned char*)&P2OUT, 1, 0); // Pin Numaralarý deðiþecek!

/*INPUTS*/
pulsar TS1((unsigned char*)&P2OUT, 16, FallingEdge, &TS_Interrupt);
pulsar TS2((unsigned char*)&P9OUT, 128);
pulsar TOUT((unsigned char*)&P9OUT, 16); // Pin Numarasýna Dikkat!
pulsar SOUT((unsigned char*)&P9OUT, 32);
pulsar OVRFLOW((unsigned char*)&P9OUT, 8);

struct channelconfig ChannelConfigs[36];

UART_Mode CurrentUARTMode = NoOp;
unsigned int MeasurementPeriod = 0;
unsigned char ReceivedData[512]={};

unsigned char TriggeredChannels[36] = {};
double AcquiredChannelValue[36] = {};
unsigned char HitBuffer[108*RAW_DATA_HIT_NUMBER] = {};
bool ACQCompleted = false;

ADC adc;


Timer ADCACQTimer(TimerA0, &ADCACQTimerInterrupt);

rena RENA;

I2C OBC(UCB0, &OBC_Handler);

UART pc(UCA0, 115200, &CommandVector);
double SpectrumInterval = (MAX_ENERGY_LEVEL - MIN_ENERGY_LEVEL) / NUMBER_OF_ENERGY_INTERVALS;

unsigned int SingleSpectrumData[15][NUMBER_OF_ENERGY_INTERVALS] = {0};
unsigned int DoubleSpectrumData[15][NUMBER_OF_ENERGY_INTERVALS] = {0};
unsigned int AnodeOnlySpectrumData[15][NUMBER_OF_ENERGY_INTERVALS] = {0};

//Light Curve variables
unsigned int LightCurve[600] = {0};
unsigned int second = 0;
unsigned int TriggerNumber = 0;
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
unsigned long AnodeOnlySpectrumNumber = 0; // NOT IMPLEMENTED!
unsigned long LightCurveDataNumber = 0; //
unsigned long ConfigNumber = 0;
/*
 *
 *
 */
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
	//enableSec(&RTCSecondInterrupt); Canceled...
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
	/*(ConfigNumber == 0)
	{
		for(int i = 0; i < 36; i++)
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
		for(int i = 11; i < 13; i++)
		{
			ChannelConfigs[i].DF = 0;
			ChannelConfigs[i].DS = 160;
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
		for(int i = 25; i < 29; i++)
		{
			ChannelConfigs[i].DF = 0;
			ChannelConfigs[i].DS = 160;
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
		RENA.ConfigureRena(ChannelConfigs, 36, CIN, CSHIFT, CS);
	}

	delay(1000);*/
	/*
	 *
	 */
	//CurrentOperationMode = DataAcquisition;
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
		ENABLE_HV_POWER.set(0); // Disable HV
		//__bis_SR_register(LPM4_bits); // Sleep
		break;
	case Diagnostic:
		ENABLE_5V.set(1); // Enable 5V
		/*
		 *	ADC silinecek.
		 *
		 */
		while(ADC_CheckConter--) // decrease the counter
		{
			ENABLE_ADC.set(1); // Enable ADC
			/* Check ADC */ /* Function is empty... */
			if(1)
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
		second = 0;
		TriggerNumber = 0;
		enableSec(&RTCSecondInterrupt);
		for(int i = 0; i < 100; i++)
		{
			CLS.set(1);
			__delay_cycles(500);
			ACQ.set(1);
			READ.set(1);
			CLS.set(0);
			while(!ACQCompleted && CurrentOperationMode == DataAcquisition)
			{
				UARTCommandHandler(CurrentUARTMode);
				/*
				 * In case TS gets stuck
				 */
				if(!TS1.get())
				{
					CLS.set(1);
					__delay_cycles(500);
					ACQ.set(1);
					READ.set(1);
					CLS.set(0);
				}
			}
			ACQCompleted = false;
			if(CurrentOperationMode != DataAcquisition) return;
			while(!AddRawData(HitBuffer, 108*RAW_DATA_HIT_NUMBER, RawDataNumber));
		}
		disableSec();
		second = 0;
		TriggerNumber = 0;
		RawDataNumber++;
		CurrentOperationMode = DataProcessing; // Set CurrentOperationMode to Idle
		break;
	case DataProcessing:
		unsigned long MultipleAnodesEventNumber = 0;
		unsigned long MultipleCathodesEventNumber = 0;
		unsigned long CathodeOnlyEventNumber = 0;
		for(int i = 0; i < 100; i++)
		{
			while(!ReadRawData(HitBuffer, 108*RAW_DATA_HIT_NUMBER, 108*RAW_DATA_HIT_NUMBER*i, SpectrumSingleNumber));
			for(int m = 0; m < RAW_DATA_HIT_NUMBER; m++)
			{
				int NumberOfTriggeredChannels = 0;
				int NumberOfTriggeredAnodes = 0;
				int NumberOfTriggeredCathodes = 0;
				unsigned int ADCVoltage = 0;
				unsigned int DoubleADCVoltage[2] = {0};
				double DoubleEnergyLevel[2] = {0};
				double EnergyLevel = 0;
				unsigned char n_counter = 0;

				for(n_counter = 19; n_counter < 35; n_counter++) // Check Anodes
				{
					if(HitBuffer[n_counter + 108*m] == 1)
					{
						NumberOfTriggeredAnodes++;
					}
				}
				for(n_counter = 1; n_counter < 15; n_counter++) // Check Cathodes
				{
					if(HitBuffer[n_counter + 108*m] == 1)
					{
						NumberOfTriggeredCathodes++;
					}
				}
				for(n_counter = 1; n_counter < 35; n_counter++) // Check All channels
				{
					if(HitBuffer[n_counter + 108*m] == 1)
					{
						NumberOfTriggeredChannels++;
					}
				}

				/*Single Spectrum Case*/
				if(NumberOfTriggeredAnodes == 1 && (NumberOfTriggeredCathodes > 0 && NumberOfTriggeredCathodes < 4))
				{
					for(n_counter = 19; n_counter < 35; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							ADCVoltage = (HitBuffer[108*m + 36 + (NumberOfTriggeredChannels-1)*2] + 256*HitBuffer[108*m + 36 + (NumberOfTriggeredChannels-1)*2 + 1]);
							EnergyLevel = ConvertToEnergyAnode(ADCVoltage, 35 - n_counter);
							SingleSpectrumData[34 - n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
						}
					}
				}

				/*Double Spectrum Case*/
				else if(NumberOfTriggeredAnodes == 2 && (NumberOfTriggeredCathodes > 0 && NumberOfTriggeredCathodes < 4))
				{
					unsigned char index = 0;
					for(n_counter = 19; n_counter < 35; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							index++;
							DoubleADCVoltage[index - 1] = (HitBuffer[108*m + 36 + (NumberOfTriggeredChannels - index)*2] + 256*HitBuffer[108*m + 36 + (NumberOfTriggeredChannels - index)*2 + 1]);
						}
					}
					index = 0;
					if(DoubleADCVoltage[0] >= DoubleADCVoltage[1])
					{
						for(n_counter = 34; n_counter > 18; n_counter--) // Check Anodes
						{
							if(HitBuffer[n_counter + 108*m] == 1)
							{
								DoubleEnergyLevel[0] = ConvertToEnergyAnode(DoubleADCVoltage[0], (35 - n_counter));
								DoubleEnergyLevel[1] = ConvertToEnergyAnode(DoubleADCVoltage[1], (35 - n_counter));
								EnergyLevel = DoubleEnergyLevel[0] + DoubleEnergyLevel[1];
								DoubleSpectrumData[34 - n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
								break;
							}
						}
					}
					else
					{
						for(n_counter = 34; n_counter > 18; n_counter--) // Check Anodes
						{
							if(HitBuffer[n_counter + 108*m] == 1)
							{
								index++;
								if(index == 2)
								{
									DoubleEnergyLevel[0] = ConvertToEnergyAnode(DoubleADCVoltage[0], 35 - n_counter);
									DoubleEnergyLevel[1] = ConvertToEnergyAnode(DoubleADCVoltage[1], 35 - n_counter);
									EnergyLevel = DoubleEnergyLevel[0] + DoubleEnergyLevel[1];
									DoubleSpectrumData[34 - n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
									break;
								}
							}
						}
					}
				}

				/*Anode Only Case*/ /*The condition may be wrong!! Check with Emrah Hoca*/
				else if(NumberOfTriggeredCathodes == 0)
				{
					for(n_counter = 19; n_counter < 35; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							ADCVoltage = (HitBuffer[108*m + 36 + (NumberOfTriggeredChannels-1)*2] + 256*HitBuffer[108*m + 36 + (NumberOfTriggeredChannels-1)*2 + 1]);
							EnergyLevel = ConvertToEnergyAnode(ADCVoltage, 35 - n_counter);
							AnodeOnlySpectrumData[34 - n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
						}
					}
				}

				/*Multiple Events Report*/
				else if(NumberOfTriggeredAnodes > 2)
					MultipleAnodesEventNumber++;

				/*Cathodes only Report*/
				else if(NumberOfTriggeredAnodes == 0)
					CathodeOnlyEventNumber++;

				else
					MultipleCathodesEventNumber++;
			}
		}
		while(!AddSpectrumSingleData(SingleSpectrumData, SpectrumSingleNumber));
		SpectrumSingleNumber++;
		while(!AddSpectrumDoubleData(DoubleSpectrumData, SpectrumDoubleNumber));
		SpectrumDoubleNumber++;
		while(!AddAnodeOnlySpectrumData(AnodeOnlySpectrumData, AnodeOnlySpectrumNumber));
		AnodeOnlySpectrumNumber++;
		while(!AddLightCurveData(LightCurve, LightCurveDataNumber)); // Maybe it is better to write it in data acquisition mode...
		LightCurveDataNumber++;

		if(MultipleAnodesEventNumber != 0)
			while(!ReportEvent(MultipleAnodes, MultipleAnodesEventNumber));
		if(MultipleAnodesEventNumber != 0)
			while(!ReportEvent(MultipleCathodes, MultipleCathodesEventNumber));
		if(CathodeOnlyEventNumber != 0)
			while(!ReportEvent(CathodeOnly, CathodeOnlyEventNumber));

		getRTCasByteArray(RTC_AsCharArray);
		UpdateSystemInfo(RTC_AsCharArray, LastOperationMode, ExecutionNumber, WDTNumber,
				      RawDataNumber, SpectrumSingleNumber, SpectrumDoubleNumber, ConfigNumber);
		for(int counter = 0; counter < 600; counter++)
		{
			LightCurve[counter] = 0;
		}
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
	if(ACQ.get() && !CLS.get())
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
			/*
			 *
			 * Must be reported!!
			 * Error Counter should be increased
			 *
			 */
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
		__bic_SR_register(GIE);
		TIN.set(0);

		for(int h = 0; h < 36; h++)
		{
			HitBuffer[HitNumber*108 + h] = TriggeredChannels[h];
		}
		TriggerNumber++;
		HitNumber++;
		if(HitNumber == RAW_DATA_HIT_NUMBER)
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

unsigned char RawDataFileNumberChar[4];
unsigned char SpectrumDataFileNumberChar[4];
unsigned char RawDataOffsetNumberChar[4];
unsigned char SpectrumDataOffsetNumberChar[4];
unsigned char RawDataLengthNumberChar[4];
unsigned char SpectrumDataLengthNumberChar[4];
unsigned long CommandFileNumber = 0;
unsigned long CommandOffsetNumber = 0;
unsigned long CommandLengthNumber = 0;
void OBC_Handler(unsigned char* Data, unsigned int Length)
{
	switch(Data[0])
	{
	case 0x01: // Get version command
		ReceivedData[0] = 0x48;
		OBC.Send(ReceivedData);
		break;
	case 0x02: // Change Mode command
		switch(Data[1])
		{
		case 0x01: // Idle
			CurrentOperationMode = Idle;
			break;
		case 0x02: // Diagnostic
			CurrentOperationMode = Diagnostic;
			break;
		case 0x03: // Data Acquisition
			CurrentOperationMode = DataAcquisition;
			break;
		}
		break;
	case 0x03: // Get Mode command
		ReceivedData[0] = CurrentOperationMode + 1;
		OBC.Send(ReceivedData);
		break;
	case 0x04: // Get Raw Data number command
		*(unsigned long*)RawDataFileNumberChar = RawDataNumber;
		OBC.Send(RawDataFileNumberChar);
		break;
	case 0x05: // Get Spectrum Data number command
		*(unsigned long*)SpectrumDataFileNumberChar = SpectrumSingleNumber;
		OBC.Send(SpectrumDataFileNumberChar);
		break;
	case 0x06: // Get Length of Raw Data file command
		CommandFileNumber = Data[1] + (Data[2] << 8) +
				((unsigned long)Data[3] << 16) + ((unsigned long)Data[4] << 24);
		*(unsigned long*)RawDataLengthNumberChar = GetRawDataFileLength(CommandFileNumber);
		OBC.Send(RawDataLengthNumberChar);
		break;
	case 0x07: // Get Length of Spectrum Data file command
		CommandFileNumber = Data[1] + (Data[2] << 8) +
				((unsigned long)Data[3] << 16) + ((unsigned long)Data[4] << 24);
		*(unsigned long*)SpectrumDataLengthNumberChar = GetSpectrumDataFileLength(CommandFileNumber);
		OBC.Send(SpectrumDataLengthNumberChar);
		break;
	case 0x08: // Get Raw Data command
		// First 4 bytes = raw data number
		CommandFileNumber = Data[1] + (Data[2] << 8) +
				((unsigned long)Data[3] << 16) + (unsigned long)((unsigned long)Data[4] << 24);
		// Next 4 bytes = offset
		CommandOffsetNumber = Data[5] + (Data[6] << 8) +
				((unsigned long)Data[7] << 16) + ((unsigned long)Data[8] << 24);
		// Next 4 bytes = length (MAX: 256)
		CommandLengthNumber = Data[9] + (Data[10] << 8) +
				((unsigned long)Data[11] << 16) + ((unsigned long)Data[12] << 24);
		while(!ReadRawData(ReceivedData, CommandLengthNumber, CommandOffsetNumber, CommandFileNumber));
		OBC.Send(ReceivedData);
		break;
	case 0x09: // Get Spectrum Data command
		// First 4 bytes = raw data number
		CommandFileNumber = Data[1] + (Data[2] << 8) +
				((unsigned long)Data[3] << 16) + (unsigned long)((unsigned long)Data[4] << 24);
		// Next 4 bytes = offset
		CommandOffsetNumber = Data[5] + (Data[6] << 8) +
				((unsigned long)Data[7] << 16) + ((unsigned long)Data[8] << 24);
		// Next 4 bytes = length (MAX: 256)
		CommandLengthNumber = Data[9] + (Data[10] << 8) +
				((unsigned long)Data[11] << 16) + ((unsigned long)Data[12] << 24);
		while(!ReadSpectrumData(ReceivedData, CommandLengthNumber, CommandOffsetNumber, CommandFileNumber));
		OBC.Send(ReceivedData);
		break;
	}
}

void RTCSecondInterrupt()
{
	LightCurve[second++] = TriggerNumber;
	TriggerNumber = 0;
}











