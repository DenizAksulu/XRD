/*
 * rena.cpp
 *
 *  Created on: 05 Þub 2014
 *      Author: Asus
 */

#include "rena.h"

namespace MSP430
{

rena::rena()
{
	// TODO Auto-generated constructor stub

}

void rena::ConfigureRena(channelconfig* config, unsigned char channelcount, pulsar cin, pulsar cshift, pulsar cs)
{
	cin.set(0);
	cshift.set(0);
	//cs.set(1);

	//delay


	//delay

	for(unsigned char i=0;i<channelcount;i++)
	{
		cs.set(0);

		for(unsigned char adr=0;adr<6;adr++)
		{
			cin.set((config[i].address & ( 32 >> adr)) );
			//delay
			cshift.set(1);
			//delay
			cshift.set(0);
			//delay
		}

		cin.set(config[i].FB_TC);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].ECAL);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].FPDWN);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].FETSEL);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		for(unsigned char gs=0;gs<2;gs++)
		{
			cin.set(config[i].gainselect & ( 2 >> gs));
			//delay
			cshift.set(1);
			//delay
			cshift.set(0);
			//delay
		}

		cin.set(config[i].PDWN);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].PZSEL);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].RANGE);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay


		cin.set(config[i].RSEL);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		for(unsigned char sl = 0; sl < 4; sl++)
		{

			cin.set((config[i].SEL & (8 >> sl)));
			//delay
			cshift.set(1);
			//delay
			cshift.set(0);
			//delay

		}

		cin.set(config[i].SIZEA);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		for(unsigned char df = 0; df < 8; df++)
		{
			cin.set(config[i].DF & (128 >> df));
			//delay
			cshift.set(1);
			//delay
			cshift.set(0);
			//delay
		}

		cin.set(config[i].POL);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		for(unsigned char ds = 0; ds < 8; ds++)
		{
			cin.set(config[i].DS & (128 >> ds));
			//delay
			cshift.set(1);
			//delay
			cshift.set(0);
			//delay
		}

		cin.set(config[i].ENF);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		cin.set(config[i].ENS);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		cin.set(config[i].FM);
		//delay
		cshift.set(1);
		//delay
		cshift.set(0);
		//delay

		//delay(10);
		cs.set(1);
		delay(5); //Dikkat!
	}


	//delay

}

unsigned char rena::CheckConfig()
{
	/*When threshold is set to 50 -> -> THRESH[0] & THRESH[35] = 2.8 V*/
	struct channelconfig TestConfigs[36];
	ADC TriggerADC;
	pulsar CIN((unsigned char*)&P5OUT, 128, 0);
	pulsar CSHIFT((unsigned char*)&P5OUT, 64, 0);
	pulsar CS((unsigned char*)&P5OUT, 32, 1);		// initial 1

	for(int i = 0; i < 36; i++)
	{
		TestConfigs[i].DS = 50;        // Slow DAC Value
		TestConfigs[i].DF = 0;         // Fast DAC Value
		TestConfigs[i].ECAL = 0;       // Disable Channel Calibration
		TestConfigs[i].ENF = 0;        // Disable Fast Trigger
		TestConfigs[i].ENS = 0;        // Disable Slow Trigger
		TestConfigs[i].FB_TC = 0;      // Feedback Time Constant is 200 mOhm
		TestConfigs[i].FETSEL = 0;     // Set to Resistive multiplier circuit
		TestConfigs[i].FM = 0;         // Disable Follower Mode
		TestConfigs[i].FPDWN = 1;      // Power down Fast Circuits
		TestConfigs[i].PDWN = 0;       // Power up Slow Circuits
		TestConfigs[i].POL = 1;        // Polerity is positive
		TestConfigs[i].PZSEL = 0;      // Disable Pole-Zero cancellation circuit
		TestConfigs[i].RANGE = 0;      // Feedback capacitor size is 15 fF
		TestConfigs[i].RSEL = 0;       // Reference Selection is VREFLOW
		TestConfigs[i].SEL = 12;       // Time Constant selection is 1.9 uS
		TestConfigs[i].SIZEA = 0;      // Input FET size is 450 um
		TestConfigs[i].address = i;    // Channel Address
		TestConfigs[i].gainselect = 3; // Gain is 5.0
	}

	ConfigureRena(TestConfigs, 36, CIN, CSHIFT, CS); // Configure RENA

	THRESH35 = TriggerADC.SingleReadVoltage(Channel4);

	if(THRESH35 <= (2.8 + 0.1) && THRESH35 >= (2.8 - 0.1))
		return 1;
	else return 0;
}

unsigned char rena::StandardPeakDetectionSequance(pulsar acq, pulsar cls, pulsar ts, unsigned int clspulsewidth, unsigned int traptime, unsigned int timoutdelay, unsigned int acqdelay)
{
	cls.set(1);
	delay(clspulsewidth);
	acq.set(1);
	cls.set(0);

	//early trigger control

	/*while(traptime--)
	{
		delay(1);
		if()
	}*/


	return 1;
}


} /* namespace msp430 */
