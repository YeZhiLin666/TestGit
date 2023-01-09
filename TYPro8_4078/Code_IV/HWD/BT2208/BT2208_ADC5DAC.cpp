
#include "../../Entry/includes.h"

void BT2208_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_Conver = BT2208_Base::PinGroup[BT2208_Pin_AdcConver];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS1];
	Adc24bit_CS5560::Pin_Rst = BT2208_Base::PinGroup[BT2208_Pin_AdcRst];
	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS2]; 
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	BT2208_Base::ADG_Reset();
}
void BT2208_ADC5DAC::Dac_Init(void)
{
	DAC16bit_AD5360::Pin_DacCS = BT2208_Base::PinGroup[BT2208_Pin_DacCS];
	DAC16bit_AD5360::Pin_DacBusy = BT2208_Base::PinGroup[BT2208_Pin_DacBusy];
	DAC16bit_AD5360::Pin_DacLoad = BT2208_Base::PinGroup[BT2208_Pin_DacLoad];
	DAC16bit_AD5360::Pin_DacRst = BT2208_Base::PinGroup[BT2208_Pin_DacRst];

	DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);    
}

// unsigned long BT2208_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&BT2208_Base::BT2208_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);               
// 		ARBUS_DELAY(10);
// 	}
// 	
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = BT2208_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = BT2208_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = BT2208_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = BT2208_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }

// void BT2208_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
// 		SSPIFunc::setPara(&BT2208_Base::BT2208_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);              
// 		ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5360::Dac_out(chn,value);     
// }

//´ËÎªpidµçÑ¹
// void BT2208_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {

// }

// //clamp voltage
// void BT2208_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
// {
//     
// }

/*********************************************************************************
**                            End Of File
*********************************************************************************/
