
#include "../../Entry/includes.h"



void LBT20083_ADC5DAC::Adc_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
#if ADC_CS5560_ENABLE == 1
        Adc24bit_CS5560::Pin_Busy = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy1];
        Adc24bit_CS5560::Pin_Conver = LBT20083_Base::PinGroup[LBT20083_Pin_AdcConver];
        Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];
        Adc24bit_CS5560::Pin_Rst = LBT20083_Base::PinGroup[LBT20083_Pin_AdcRst];
        Adc24bit_CS5560::SPI_Idx = 1;
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

        Adc24bit_CS5560::Pin_Busy = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy2];
        Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

        LBT20083_Base::ADG_Reset();
#endif
    }
    else
    {
        Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];
        SSPIFunc::setPara(&LBT20083_Base::LBT20083_SSP1ADC,1);
        SSPIFunc::my_Init(1);
        ARBUS_DELAY(10);
        Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //初始化电流采样ADC7175

        Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
        SSPIFunc::setPara(&LBT20083_Base::LBT20083_SSP1ADC,1);
        SSPIFunc::my_Init(1);
        ARBUS_DELAY(10);
        Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//初始化电压采样ADC7175
    }
}
void LBT20083_ADC5DAC::Dac_Init(void)
{
    DAC16bit_AD5360::Pin_DacCS = LBT20083_Base::PinGroup[LBT20083_Pin_DacCS];
    DAC16bit_AD5360::Pin_DacBusy = LBT20083_Base::PinGroup[LBT20083_Pin_DacBusy];
    DAC16bit_AD5360::Pin_DacLoad = LBT20083_Base::PinGroup[LBT20083_Pin_DacLoad];
    DAC16bit_AD5360::Pin_DacRst = LBT20083_Base::PinGroup[LBT20083_Pin_DacRst];

    DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);
}

// unsigned long LBT20083_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&LBT20083_Base::LBT20083_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);
// 		ARBUS_DELAY(10);
// 	}
//
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = LBT20083_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = LBT20083_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = LBT20083_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = LBT20083_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }

// void LBT20083_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
// 		SSPIFunc::setPara(&LBT20083_Base::LBT20083_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5360::Dac_out(chn,value);
// }

// //此为pid电压
// void LBT20083_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {
// 	Dac_out_Cur(chn,value); //使用电流输出
// }

// //clamp voltage
// void LBT20083_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
// {

// }

/*********************************************************************************
**                            End Of File
*********************************************************************************/
