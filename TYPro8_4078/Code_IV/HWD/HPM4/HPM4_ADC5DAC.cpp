#include "includes.h"
////////////////////////////////////////////////////DAC
//unsigned short HPM4_ADC5DAC::NeedRefreshDac = 0x0000;        //????bit????DAC??


void HPM4_ADC5DAC::Adc_Init(void)
{
    //   Adc24bit_AD7175::ADC_DRDY = HPM4_Base::PinGroup[HPM4_ADC_DRDY];
    Adc24bit_AD7175::ADC_START = HPM4_Base::PinGroup[HPM4_Pin_ADCSTART];

    Adc24bit_AD7175::ADC_CS = HPM4_Base::PinGroup[HPM4_Pin_CS_C];
    SSPIFunc::setPara(&HPM4_Base::HPM4_SSP1ADC,1);
    SSPIFunc::my_Init(1);
    ARBUS_DELAY(10);
    Adc24bit_AD7175::Init(eCONVCODETYPE_BIPOLAR,1);  //初始化电流采样ADC7175

    Adc24bit_AD7175::ADC_CS = HPM4_Base::PinGroup[HPM4_Pin_CS_V];
    SSPIFunc::setPara(&HPM4_Base::HPM4_SSP1ADC,1);
    SSPIFunc::my_Init(1);
    ARBUS_DELAY(10);

    Adc24bit_AD7175::Init(eCONVCODETYPE_BIPOLAR,1);  //初始化电压采样ADC7175

}
void HPM4_ADC5DAC::Dac_Init(void)
{
    DAC16bit_AD5360::Pin_DacCS = HPM4_Base::PinGroup[HPM4_Pin_DacCS];
    DAC16bit_AD5360::Pin_DacBusy =  HPM4_Base::PinGroup[HPM4_Pin_DacBusy];
    DAC16bit_AD5360::Pin_DacLoad = HPM4_Base::PinGroup[HPM4_Pin_DacLoad];
    DAC16bit_AD5360::Pin_DacRst = HPM4_Base::PinGroup[HPM4_Pin_DacRst];

    DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);   //IBT°?ààDíμ?DACD???μ?ó2?t1ü??±??????aá? 21??·?ê?

    ARBUS_DELAY(10000);    //300us reset time  AD5360重置
}

/*unsigned long HPM4_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
{


	return 0;
}*/

void HPM4_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
    //unsigned char bOK = SSPIFunc::getModuleName(0);
    //if( bOK != HPM4_SSP0_DAC_NAME )
    //{
    //    SSPIFunc::setPara(&HPM4_Base::HPM4_SSP0DAC,0);
    //    SSPIFunc::my_Init(0);
    //    ARBUS_DELAY(10);
    //}
    SSPIFunc::ConfirmDevice(0, &HPM4_Base::HPM4_SSP0DAC);
    ARBUS_DELAY(10);
    DAC16bit_AD5360::Dac_out(chn,value);
}

//电压为ADC1
void HPM4_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
    //unsigned char bOK = SSPIFunc::getModuleName(0);
    //if( bOK != HPM4_SSP0_DAC_NAME )
    //{
    //   SSPIFunc::setPara(&HPM4_Base::HPM4_SSP0DAC,0);
    //   SSPIFunc::my_Init(0);
    //   ARBUS_DELAY(10);
    //}
    SSPIFunc::ConfirmDevice(0, &HPM4_Base::HPM4_SSP0DAC);
    DAC16bit_AD5360::Dac_out(chn,value);

}

//clamp voltage
void HPM4_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    unsigned long temp = value;    // 20140613  电压保护
    //unsigned char bOK = SSPIFunc::getModuleName(0);
    //if( bOK!= HPM4_SSP0_DAC_NAME )
    //{
    //	SSPIFunc::setPara(&HPM4_Base::HPM4_SSP0DAC,0);
    //	SSPIFunc::my_Init(0);
    //	ARBUS_DELAY(10);
    //}
    SSPIFunc::ConfirmDevice(0, &HPM4_Base::HPM4_SSP0DAC);
    DAC16bit_AD5360::Dac_out(chn,temp);    //AD5360

}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
