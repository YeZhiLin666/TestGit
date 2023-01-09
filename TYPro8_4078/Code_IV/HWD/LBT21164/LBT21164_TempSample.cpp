#include "../../Entry/includes.h"
//#include "../HWD/LBT2108/LBT2108_TempSample.h"

//const St_Port LBT2108_TempSample::Pin_ADTurnOn   = {0,       23,        eCPTYPE_MCUGPIO,   ADC0_IN,               0};

void  LBT2108_TempSample::AD_Init(unsigned char ADCH_No)
{
	for(unsigned char uc_i=0;uc_i<ADCH_No;uc_i++)
	{
		ChannelInfo_Manager::m_TempProtectData[uc_i]=25.0;
	}
	LPC_IOCON->P0_23=1;
	// 	  LPC_IOCON->P0_24=1;
	// 	  LPC_IOCON->P0_25=1;
	// 	  LPC_IOCON->P0_26=1;
	// 	  LPC_IOCON->P1_30=1;
	// 	  LPC_IOCON->P1_31=1;
	// 	  LPC_IOCON->P0_12=1;
	// 	  LPC_IOCON->P0_13=1;
	LPC_SC->PCONP |=  (1<<12);            /* Enable power to ADC block */

}

float LBT2108_TempSample::Temp_rd(unsigned char ADCH_Number)
{
	//unsigned long ADC_CODE;
	float ADC_Value,Result;

	//LPC4078ADC::Init();
	ADC_Value = LPC4078ADC::Read(ADCH_Number);
	Result =ADC_Value/10;
	ChannelInfo_Manager::m_TempProtectData[ADCH_Number]= Result;
	return  Result;		 	  
}
