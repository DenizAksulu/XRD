
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
struct channelconfig NullConfigs[36];

UART_Mode CurrentUARTMode = NoOp;
unsigned int MeasurementPeriod = 0;
unsigned char ReceivedData[512]={};

unsigned char TriggeredChannels[36] = {};
double AcquiredChannelValue[36] = {};
unsigned char HitBuffer[108*RAW_DATA_HIT_NUMBER] = {};
bool ACQCompleted = false;
unsigned long HIT_LIMIT = 100;

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
int TimeoutCounter = 0;
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
unsigned long AcquisitionNumber = 0;
unsigned long ConfigNumber = 0;
unsigned char XRDStatus = XRD_OK;
bool FollowerMode = false;
/*
 *
 *
 */
/*DUMMY Variable*/
bool SDEnabled = 1;
/****************/

int main(void)
{
	/*
	 * SD Initialization
	 */
	fat_init();
	if(!GetSystemInfo(RTC_AsCharArray, LastOperationMode, ExecutionNumber, WDTNumber,
			      RawDataNumber, AcquisitionNumber, SpectrumDoubleNumber, ConfigNumber))
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
	/*NULL CONFIG*/
	for(int i = 0; i < 36; i++)
	{
		NullConfigs[i].DS = 50;        // Slow DAC Value
		NullConfigs[i].DF = 0;         // Fast DAC Value
		NullConfigs[i].ECAL = 0;       // Disable Channel Calibration
		NullConfigs[i].ENF = 0;        // Disable Fast Trigger
		NullConfigs[i].ENS = 0;        // Disable Slow Trigger
		NullConfigs[i].FB_TC = 0;      // Feedback Time Constant is 200 mOhm
		NullConfigs[i].FETSEL = 0;     // Set to Resistive multiplier circuit
		NullConfigs[i].FM = 0;         // Disable Follower Mode
		NullConfigs[i].FPDWN = 1;      // Power down Fast Circuits
		NullConfigs[i].PDWN = 1;       // Power up Slow Circuits
		NullConfigs[i].POL = 1;        // Polerity is positive
		NullConfigs[i].PZSEL = 0;      // Disable Pole-Zero cancellation circuit
		NullConfigs[i].RANGE = 0;      // Feedback capacitor size is 15 fF
		NullConfigs[i].RSEL = 0;       // Reference Selection is VREFLOW
		NullConfigs[i].SEL = 12;       // Time Constant selection is 1.9 uS
		NullConfigs[i].SIZEA = 0;      // Input FET size is 450 um
		NullConfigs[i].address = i;    // Channel Address
		NullConfigs[i].gainselect = 3; // Gain is 5.0
	}
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
	CurrentOperationMode = Idle;
	while(1)
	{
		WDTCTL = WDT_ADLY_1000;
		UARTCommandHandler(CurrentUARTMode);
		RunOperationMode(CurrentOperationMode);
	}
}

void RunOperationMode(Operation_Mode mode)
{
	switch(mode)
	{
	case Idle:
		//ENABLE_HV_POWER.set(1); // Disable HV
		//__bis_SR_register(LPM4_bits); // Sleep
		break;
	case Diagnostic:
		if(RENA.CheckConfig())
		{
			if(TS1.get())
			{
				XRDStatus = XRD_OK;
				RENA.ConfigureRena(NullConfigs, 36, CIN, CSHIFT, CS); // Configure RENA
				ReportEvent(RENA_OK); // Report RENA OK
			}
			else
			{
				XRDStatus = XRD_ERROR;
				RENA.ConfigureRena(NullConfigs, 36, CIN, CSHIFT, CS); // Configure RENA
				ReportEvent(RENA_FAIL); // Report RENA Fail
			}
		}
		else
		{
			XRDStatus = XRD_ERROR;
			RENA.ConfigureRena(NullConfigs, 36, CIN, CSHIFT, CS); // Configure RENA
			ReportEvent(RENA_FAIL); // Report RENA Fail
		}
		CurrentOperationMode = Idle; // Set CurrentOperationMode to Idle
		break;
	case DataAcquisition:
		//ENABLE_HV_POWER.set(0); // Enable HV
		second = 0;
		TriggerNumber = 0;
		TimeoutCounter = 0;
		enableSec(&RTCSecondInterrupt);
		for(unsigned long i = 0; i < HIT_LIMIT; i++)
		{
			WDTCTL = WDT_ADLY_1000;
			CLS.set(1);
			__delay_cycles(500);
			ACQ.set(1);
			READ.set(1);
			CLS.set(0);
			while(!ACQCompleted && CurrentOperationMode == DataAcquisition)
			{
				WDTCTL = WDT_ADLY_1000;
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
			if(SDEnabled)
			{
				while(!AddRawData(HitBuffer, 108*RAW_DATA_HIT_NUMBER, RawDataNumber));
			}
		}
		disableSec();
		second = 0;
		TriggerNumber = 0;
		TimeoutCounter = 0;
		//ENABLE_HV_POWER.set(1); // Disable Power
		if(SDEnabled)
			CurrentOperationMode = DataProcessing;
		else CurrentOperationMode = Idle;
		break;
	case DataProcessing:
		unsigned long MultipleAnodesEventNumber = 0;
		unsigned long MultipleCathodesEventNumber = 0;
		unsigned long CathodeOnlyEventNumber = 0;
		for(unsigned long i = 0; i < HIT_LIMIT; i++)
		{
			WDTCTL = WDT_ADLY_1000;
			while(!ReadRawData(HitBuffer, 108*RAW_DATA_HIT_NUMBER, 108*RAW_DATA_HIT_NUMBER*i, RawDataNumber));
			for(int m = 0; m < RAW_DATA_HIT_NUMBER; m++)
			{
				WDTCTL = WDT_ADLY_1000;
				int NumberOfTriggeredChannels = 0;
				int NumberOfTriggeredAnodes = 0;
				int NumberOfTriggeredCathodes = 0;
				unsigned int ADCVoltage = 0;
				unsigned int DoubleADCVoltage[2] = {0};
				double DoubleEnergyLevel[2] = {0};
				double EnergyLevel = 0;
				unsigned char n_counter = 0;

				for(n_counter = 19; n_counter < 35; n_counter++) // Check Anodes // Check Anodes
				{
					if(HitBuffer[n_counter + 108*m] == 1)
					{
						NumberOfTriggeredCathodes++;
					}
				}
				for(n_counter = 1; n_counter < 15; n_counter++) // Check Cathodes // Check Anodes
				{
					if(HitBuffer[n_counter + 108*m] == 1)
					{
						NumberOfTriggeredAnodes++;
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
					for(n_counter = 1; n_counter < 15; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							ADCVoltage = (HitBuffer[108*m + 36] + 256*HitBuffer[108*m + 36 + 1]);
							EnergyLevel = ConvertToEnergyAnode(ADCVoltage, n_counter);
							SingleSpectrumData[n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
						}
					}
				}

				/*Double Spectrum Case*/
				else if(NumberOfTriggeredAnodes == 2 && (NumberOfTriggeredCathodes > 0 && NumberOfTriggeredCathodes < 4))
				{
					unsigned char index = 0;
					for(n_counter = 1; n_counter < 15; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							DoubleADCVoltage[index] = (HitBuffer[108*m + 36 + index*2] + 256*HitBuffer[108*m + 36 + index*2 + 1]);
							index++;
						}
					}
					index = 0;
					if(DoubleADCVoltage[0] >= DoubleADCVoltage[1])
					{
						for(n_counter = 14; n_counter > 0; n_counter--) // Check Anodes
						{
							if(HitBuffer[n_counter + 108*m] == 1)
							{
								DoubleEnergyLevel[0] = ConvertToEnergyAnode(DoubleADCVoltage[0], (n_counter));
								DoubleEnergyLevel[1] = ConvertToEnergyAnode(DoubleADCVoltage[1], (n_counter));
								EnergyLevel = DoubleEnergyLevel[0] + DoubleEnergyLevel[1];
								DoubleSpectrumData[n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
								break;
							}
						}
					}
					else
					{
						for(n_counter = 14; n_counter > 0; n_counter--) // Check Anodes
						{
							if(HitBuffer[n_counter + 108*m] == 1)
							{
								index++;
								if(index == 2)
								{
									DoubleEnergyLevel[0] = ConvertToEnergyAnode(DoubleADCVoltage[0], n_counter);
									DoubleEnergyLevel[1] = ConvertToEnergyAnode(DoubleADCVoltage[1], n_counter);
									EnergyLevel = DoubleEnergyLevel[0] + DoubleEnergyLevel[1];
									DoubleSpectrumData[n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
									break;
								}
							}
						}
					}
				}

				/*Anode Only Case*/
				else if(NumberOfTriggeredCathodes == 0)
				{
					for(n_counter = 1; n_counter < 15; n_counter++) // Check Anodes
					{
						if(HitBuffer[n_counter + 108*m] == 1)
						{
							ADCVoltage = (HitBuffer[108*m + 36] + 256*HitBuffer[108*m + 36 + 1]);
							EnergyLevel = ConvertToEnergyAnode(ADCVoltage, n_counter);
							AnodeOnlySpectrumData[n_counter][(unsigned int)(EnergyLevel/SpectrumInterval)]++;
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
		while(!AddSpectrumSingleData(SingleSpectrumData, RawDataNumber)); // 3000 byte
		while(!AddSpectrumDoubleData(DoubleSpectrumData, RawDataNumber)); // 3000 byte
		while(!AddAnodeOnlySpectrumData(AnodeOnlySpectrumData, RawDataNumber)); // 3000 byte
		while(!AddLightCurveData(LightCurve, RawDataNumber));// 1200 byte // Maybe it is better to write it in data acquisition mode...
		while(!ReportEvent(MultipleAnodes, MultipleAnodesEventNumber, RawDataNumber)); // 19
		while(!ReportEvent(MultipleCathodes, MultipleCathodesEventNumber, RawDataNumber)); // 19
		while(!ReportEvent(CathodeOnly, CathodeOnlyEventNumber, RawDataNumber)); // 19
		RawDataNumber++;

		getRTCasByteArray(RTC_AsCharArray);
		UpdateSystemInfo(RTC_AsCharArray, LastOperationMode, ExecutionNumber, WDTNumber,
				      RawDataNumber, AcquisitionNumber, SpectrumDoubleNumber, ConfigNumber);
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

unsigned char RawDataFileNumberChar[4];
unsigned char SpectrumDataFileNumberChar[4];
unsigned char RawDataOffsetNumberChar[4];
unsigned char SpectrumDataOffsetNumberChar[4];
unsigned char RawDataLengthNumberChar[4];
unsigned char SpectrumDataLengthNumberChar[4];
unsigned long CommandFileNumber = 0;
unsigned long CommandOffsetNumber = 0;
unsigned long CommandLengthNumber = 0;
unsigned char ERROR = 0x0F;
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
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
				CurrentOperationMode = Idle;
			}
			else if(ReceivedData[10] == 1)
			{
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
				CurrentOperationMode = Diagnostic;
			}
			else if(ReceivedData[10] == 2)
			{
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
				CurrentOperationMode = DataAcquisition;
			}
			else if(ReceivedData[10] == 3)
			{
				pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
				CurrentOperationMode = DataProcessing;
			}
		}
		/*************************************************************/
		/*High Voltage On*/
		else if(CompareCharArrayToString(ReceivedData, "HighOn", 6))
		{
			ENABLE_HV_POWER.set(0);
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/
		/*High Voltage Off*/
		else if(CompareCharArrayToString(ReceivedData, "HighOff", 7))
		{
			ENABLE_HV_POWER.set(1);
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/
		/*SD On*/
		else if(CompareCharArrayToString(ReceivedData, "SDOn", 4))
		{
			SDEnabled = true;
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/
		/*SD Off*/
		else if(CompareCharArrayToString(ReceivedData, "SDOff", 5))
		{
			SDEnabled = false;
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		/*************************************************************/
		/*Get Status*/
		else if(CompareCharArrayToString(ReceivedData, "GetStatus", 9))
		{
			int index = 9;
			unsigned char limit_char[4];
			*(unsigned long*)limit_char = HIT_LIMIT;
			ReceivedData[index++] = CurrentOperationMode;
			ReceivedData[index++] = XRDStatus;
			if(!ENABLE_HV_POWER.get())
				ReceivedData[index++] = 0x01;
			else
				ReceivedData[index++] = 0x00;
			if(SDEnabled)
				ReceivedData[index++] = 0x01;
			else
				ReceivedData[index++] = 0x00;
			if(FollowerMode)
				ReceivedData[index++] = 0x01;
			else
				ReceivedData[index++] = 0x00;
			ReceivedData[index++] = limit_char[0];
			ReceivedData[index++] = limit_char[1];
			ReceivedData[index++] = limit_char[2];
			ReceivedData[index++] = limit_char[3];
			pc.Send(AssembleDataPacket(ReceivedData, 18), 28);
		}
		else if(CompareCharArrayToString(ReceivedData, "GetXRDStatus", 12))
		{
			int index = 12;
			ReceivedData[index++] = CurrentOperationMode;
			ReceivedData[index++] = XRDStatus;

			pc.Send(AssembleDataPacket(ReceivedData, 14), 24);
		}
		/*************************************************************/
		/*GetSDData***************************************************/
		else if(CompareCharArrayToString(ReceivedData, "GetSDData", 9))
		{
			//RawData
			if(ReceivedData[9] == 0 && RawDataNumber != 0)
			{
				// First 4 bytes = OFFSET
				CommandOffsetNumber = ReceivedData[10] + (unsigned long)((unsigned long)ReceivedData[11] << 8) +
						((unsigned long)ReceivedData[12] << 16) + (unsigned long)((unsigned long)ReceivedData[13] << 24);
				// Next 4 bytes = length (MAX: 256)
				CommandLengthNumber = ReceivedData[14] + (unsigned long)((unsigned long)ReceivedData[15] << 8) +
						((unsigned long)ReceivedData[16] << 16) + ((unsigned long)ReceivedData[17] << 24);
				while(!ReadRawData(HitBuffer, CommandLengthNumber, CommandOffsetNumber, RawDataNumber - 1));

				pc.Send(HitBuffer, CommandLengthNumber);
			}
			//ProcessedData
			if(ReceivedData[9] == 1 && RawDataNumber != 0)
			{
				// First 4 bytes = OFFSET
				CommandOffsetNumber = ReceivedData[10] + (unsigned long)((unsigned long)ReceivedData[11] << 8) +
						((unsigned long)ReceivedData[12] << 16) + (unsigned long)((unsigned long)ReceivedData[13] << 24);
				// Next 4 bytes = length (MAX: 256)
				CommandLengthNumber = ReceivedData[14] + (unsigned long)((unsigned long)ReceivedData[15] << 8) +
						((unsigned long)ReceivedData[16] << 16) + ((unsigned long)ReceivedData[17] << 24);
				while(!ReadProcessedData(HitBuffer, CommandLengthNumber, CommandOffsetNumber, RawDataNumber - 1));

				pc.Send(HitBuffer, CommandLengthNumber);
			}
		}
		else if(CompareCharArrayToString(ReceivedData, "SetHitLimit", 11))
		{
			HIT_LIMIT = (unsigned long)ReceivedData[11] + (unsigned long)ReceivedData[12]*256 + (unsigned long)ReceivedData[13]*65536 + (unsigned long)ReceivedData[14]*16777216;
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		else if(CompareCharArrayToString(ReceivedData, "FollowerOn", 10))
		{
			CurrentOperationMode = Idle;
			ACQ.set(1);
			TIN.set(1);

			CLS.set(0);
			TCLK.set(0);
			READ.set(1);
			SHRCLK.set(0);
			SIN.set(0);
			CIN.set(0);
			CSHIFT.set(0);
			FollowerMode = true;
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
		else if(CompareCharArrayToString(ReceivedData, "FollowerOff", 11))
		{
			CurrentOperationMode = Idle;
			ACQ.set(0);
			TIN.set(0);

			CLS.set(0);
			TCLK.set(0);
			READ.set(0);
			SHRCLK.set(0);
			SIN.set(0);
			CIN.set(0);
			CSHIFT.set(0);
			FollowerMode = false;
			pc.Send(AssembleDataPacket((unsigned char*)"ACK", 3), 13);
		}
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
	if(ACQ.get() && !CLS.get() && !TIN.get())
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
		ReceivedData[1] = XRDStatus;
		OBC.Send(ReceivedData);
		break;
	case 0x04: // Get Raw Data command
		// First 4 bytes = raw data number
		CommandOffsetNumber = Data[1] + (unsigned long)((unsigned long)Data[2] << 8) +
				((unsigned long)Data[3] << 16) + (unsigned long)((unsigned long)Data[4] << 24);
		// Next 4 bytes = offset
		CommandLengthNumber = Data[5] + (unsigned long)((unsigned long)Data[6] << 8) +
				((unsigned long)Data[7] << 16) + ((unsigned long)Data[8] << 24);
		// Next 4 bytes = length (MAX: 256)
		if(RawDataNumber != 0)
		{
			while(!ReadRawData(ReceivedData, CommandLengthNumber, CommandOffsetNumber, RawDataNumber - 1));
			OBC.Send(ReceivedData);
		}
		else
		{
			for(int i = 0; i < 256; i++)
			{
				ReceivedData[i] = ERROR;
			}
			OBC.Send(ReceivedData);
		}
		break;
	case 0x05: // Get Spectrum Data command
		// First 4 bytes = raw data number
		CommandOffsetNumber = Data[1] + (unsigned long)((unsigned long)Data[2] << 8) +
				((unsigned long)Data[3] << 16) + (unsigned long)((unsigned long)Data[4] << 24);
		// Next 4 bytes = offset
		CommandLengthNumber = Data[5] + (unsigned long)((unsigned long)Data[6] << 8) +
				((unsigned long)Data[7] << 16) + ((unsigned long)Data[8] << 24);
		if(RawDataNumber != 0)
		{
			while(!ReadProcessedData(ReceivedData, CommandLengthNumber, CommandOffsetNumber, RawDataNumber - 1));
			OBC.Send(ReceivedData);
		}
		else
		{
			for(int i = 0; i < 256; i++)
			{
				ReceivedData[i] = ERROR;
			}
			OBC.Send(ReceivedData);
		}
		break;
	default:
		OBC.Send(&ERROR);
		break;
	}
}

//Timeout at 2000 but Light curve ends at 600
void RTCSecondInterrupt()
{
	if(second < 600)
	{
		LightCurve[second++] = TriggerNumber;
		TriggerNumber = 0;
	}
	TimeoutCounter++;
	if(TimeoutCounter >= ACQ_TIMEOUT)
	{
		CurrentOperationMode = Idle;
		TimeoutCounter = 0;
	}
}









