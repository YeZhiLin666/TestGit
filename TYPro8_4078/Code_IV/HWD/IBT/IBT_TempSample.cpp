#include "../../Entry/includes.h"

void  IBT_TempSample::AD_Init(unsigned char ADCH_No)
{
	for(unsigned char uc_i=0;uc_i<ADCH_No;uc_i++)
	{
		ChannelInfo_Manager::m_TempProtectData[uc_i]=25.0;
	}
	LPC_IOCON->P0_23=1;
	LPC_IOCON->P0_24=1;
	LPC_IOCON->P0_25=1;
	LPC_IOCON->P0_26=1;
	LPC_IOCON->P1_30=1;
	LPC_IOCON->P1_31=1;
	LPC_IOCON->P0_12=1;
	LPC_IOCON->P0_13=1;
	LPC_SC->PCONP |=  (1<<12);            // Enable power to ADC block //

}

float IBT_TempSample::Temp_rd(unsigned char ADCH_Number)
{
	//unsigned long ADC_CODE;
	float ADC_Value,Result;

	ADC_Value = ADCFunc::Read(ADCH_Number);
	Result =ADC_Value/10;
	ChannelInfo_Manager::m_TempProtectData[ADCH_Number]=Result;

	return  Result;		 	  
}
