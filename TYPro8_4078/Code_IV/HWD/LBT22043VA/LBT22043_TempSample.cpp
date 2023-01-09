#include "../../Entry/includes.h"
//#include "../HWD/LBT2401/LBT2401_TempSample.h"

void  LBT22043_TempSample::AD_Init(unsigned char ADCH_No)
{
	for(unsigned char uc_i=0;uc_i<ADCH_No;uc_i++)
	{
		ChannelInfo_Manager::m_TempProtectData[uc_i]=25.0;
	}
	LPC_IOCON->P0_23=1;
	LPC_IOCON->P0_24=1;
	LPC_IOCON->P0_25=1;
	LPC_IOCON->P0_26=1;
	LPC_SC->PCONP |=  (1<<12);            // Enable power to ADC block //

}

float LBT22043_TempSample::Temp_rd(unsigned char ADCH_Number)
{
	float ADC_Value,Result;

	ADC_Value = ADCFunc::Read(ADCH_Number);
	Result =ADC_Value/10;
	ChannelInfo_Manager::m_TempProtectData[ADCH_Number]=Result;
	return  Result;		 	  
}
