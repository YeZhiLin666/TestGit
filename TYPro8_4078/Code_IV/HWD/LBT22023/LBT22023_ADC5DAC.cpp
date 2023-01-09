#include "includes.h"


void LBT22023_ADC5DAC::Adc_Init(void)
{
    Adc16bit_AD7606::OS0=LBT22023_Base::PinGroup[LBT22023_Pin_OS0];       //No use
    Adc16bit_AD7606::OS1=LBT22023_Base::PinGroup[LBT22023_Pin_OS1];       //No use
    Adc16bit_AD7606::OS2=LBT22023_Base::PinGroup[LBT22023_Pin_OS2];       //No use
    Adc16bit_AD7606::BUSY=LBT22023_Base::PinGroup[LBT22023_Pin_AdcBusy];  //No use
    Adc16bit_AD7606::FDATA=LBT22023_Base::PinGroup[LBT22023_Pin_FDATA];   //No use

    Adc16bit_AD7606::CONVA=LBT22023_Base::PinGroup[LBT22023_Pin_AdcConverA];
    Adc16bit_AD7606::CONVB=LBT22023_Base::PinGroup[LBT22023_Pin_AdcConverB];
    Adc16bit_AD7606::Rst =LBT22023_Base::PinGroup[LBT22023_Pin_AdcRst];
    Adc16bit_AD7606::Address=CPLD_BASE_ADDR+2;

    Adc16bit_AD7606::Init(eCONVCODETYPE_BIPOLAR);
}
void LBT22023_ADC5DAC::Dac_Init(void)
{
    //AD5754
    DAC16bit_AD5754::Pin_DacBin = LBT22023_Base::PinGroup[LBT22023_Pin_DacBIN2COMP];
    DAC16bit_AD5754::Pin_DacCS = LBT22023_Base::PinGroup[LBT22023_Pin_DacCS];
    DAC16bit_AD5754::Pin_DacLoad = LBT22023_Base::PinGroup[LBT22023_Pin_DacLoad];
    DAC16bit_AD5754::Pin_DacClr = LBT22023_Base::PinGroup[LBT22023_Pin_DacCLR];
    DAC16bit_AD5754::SPI_Idx = 0;

    //≥ı ºªØ£¨power up AD5754
    SSPIFunc::ConfirmDevice(0, &LBT22023_Base::LBT22023_SSP0DAC);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN ®§-??        BIN COMP
}

void LBT22023_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &LBT22023_Base::LBT22023_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(chn,value);
}

//clamp voltage
void LBT22023_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &LBT22023_Base::LBT22023_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
