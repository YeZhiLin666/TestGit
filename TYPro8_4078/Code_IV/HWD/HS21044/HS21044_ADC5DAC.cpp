#include "includes.h"
////////////////////////////////////////////////////DAC

void HS21044_ADC5DAC::Adc_Init(void)
{
    Adc16bit_AD7606::OS0=HS21044_Base::PinGroup[HS21044_Pin_OS0];       //No use
    Adc16bit_AD7606::OS1=HS21044_Base::PinGroup[HS21044_Pin_OS1];       //No use
    Adc16bit_AD7606::OS2=HS21044_Base::PinGroup[HS21044_Pin_OS2];       //No use
    Adc16bit_AD7606::BUSY=HS21044_Base::PinGroup[HS21044_Pin_AdcBusy];  //No use
    Adc16bit_AD7606::FDATA=HS21044_Base::PinGroup[HS21044_Pin_FDATA];   //No use

    Adc16bit_AD7606::CONVA=HS21044_Base::PinGroup[HS21044_Pin_AdcConverA];
    Adc16bit_AD7606::CONVB=HS21044_Base::PinGroup[HS21044_Pin_AdcConverB];
    Adc16bit_AD7606::Rst =HS21044_Base::PinGroup[HS21044_Pin_AdcRst];
    Adc16bit_AD7606::Address=CPLD_BASE_ADDR+2;

    Adc16bit_AD7606::Init(eCONVCODETYPE_BIPOLAR);
}
void HS21044_ADC5DAC::Dac_Init(void)
{
    //AD5754
    DAC16bit_AD5754::Pin_DacBin = HS21044_Base::PinGroup[HS21044_Pin_DacBIN2COMP];
    DAC16bit_AD5754::Pin_DacCS = HS21044_Base::PinGroup[HS21044_Pin_DacCS];
    DAC16bit_AD5754::Pin_DacLoad = HS21044_Base::PinGroup[HS21044_Pin_DacLoad];
    DAC16bit_AD5754::Pin_DacClr = HS21044_Base::PinGroup[HS21044_Pin_DacCLR];
    DAC16bit_AD5754::SPI_Idx = 0;

    //初始化，power up AD5754
    SSPIFunc::ConfirmDevice(0, &HS21044_Base::HS21044_SSP0DAC);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN    BIN COMP
}

void HS21044_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &HS21044_Base::HS21044_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(0,value);
}

//电压为ADC1
void HS21044_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
//	unsigned long temp = 0;
    SSPIFunc::ConfirmDevice(0, &HS21044_Base::HS21044_SSP0DAC);
    //正负反向
// 	if(value > 0xFFFFFF)
// 		value = 0xFFFFFF;
// 	if( value > 0x800000 )
// 	{
// 		temp = value - 0x800000;
// 		value = 0x800000 - temp;
// 	}
// 	else
// 	{
// 		temp = 0x800000 - value;
// 		value = 0x800000 + temp;
// 	}
// 	if(value > 0xFFFFFF)
// 		value = 0xFFFFFF;
    value = 0xFFFFFF-value;
    DAC16bit_AD5754::Dac_Out(1,value);
}

//clamp voltage
void HS21044_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &HS21044_Base::HS21044_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
