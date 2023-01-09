///////////////////////////////////////////////////////////////////////////////////
//File Name: BatteryCells_ADC5DAC.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						BatteryCells辅助采样驱动处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

void BatteryCells_ADC5DAC::Adc_Init(void)
{
    BatteryCells_SampleManage::sampleSettingSet_CS(0);
    ADC16bit_LTC68042::setCS(&BatteryCells_Base::Pin_EOE);     //传递片选信号LTC6804_CS
    ControlPin::SetMeHigh(BatteryCells_Base::Pin_EOE);

    ADC16bit_LTC68042::LTC6804_initialize(eCONVCODETYPE_BIPOLAR);
}
char  BatteryCells_ADC5DAC::ADC_rd(unsigned long BoardID,unsigned char ADCID,unsigned long ChannelID)
{
    return 0;
}

