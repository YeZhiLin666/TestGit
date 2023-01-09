///////////////////////////////////////////////////////////////////////////////////
//File Name: HighMode2ndV_ADC5DAC.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						HighMode2ndV辅助采样驱动处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

void HighMode2ndV_ADC5DAC::Adc_Init(void)
{
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR);

    Adc24bit_ADS1259::Pin_Conver=HighMode2ndV_Base::Pin_Cover;
    Adc24bit_ADS1259::Pin_CS= HighMode2ndV_Base::Pin_CS;
    SSPIFunc::setPara(&HighMode2ndV_Base::HighMode2ndV_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化
    for(unsigned char i=0; i<16; i++)
    {
        Adc24bit_ADS1259::SetChannel(i);
        Adc24bit_ADS1259::WriteReg(1,i);

    }
}
char  HighMode2ndV_ADC5DAC::ADC_rd(unsigned long BoardID,unsigned long ChannelID)
{
    return 0;
}

