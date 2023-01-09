#include "includes.h"


void LBT2204_ADC5DAC::Adc_Init(void)
{
	Adc16bit_AD7606::OS0=LBT2204_Base::PinGroup[LBT2204_Pin_OS0];       //No use
	Adc16bit_AD7606::OS1=LBT2204_Base::PinGroup[LBT2204_Pin_OS1];       //No use
	Adc16bit_AD7606::OS2=LBT2204_Base::PinGroup[LBT2204_Pin_OS2];       //No use
	Adc16bit_AD7606::BUSY=LBT2204_Base::PinGroup[LBT2204_Pin_AdcBusy];  //No use
	Adc16bit_AD7606::FDATA=LBT2204_Base::PinGroup[LBT2204_Pin_FDATA];   //No use

	Adc16bit_AD7606::CONVA=LBT2204_Base::PinGroup[LBT2204_Pin_AdcConverA];
	Adc16bit_AD7606::CONVB=LBT2204_Base::PinGroup[LBT2204_Pin_AdcConverB];
	Adc16bit_AD7606::Rst =LBT2204_Base::PinGroup[LBT2204_Pin_AdcRst];
	Adc16bit_AD7606::Address=CPLD_BASE_ADDR+2;

	Adc16bit_AD7606::Init(eCONVCODETYPE_BIPOLAR);
}
void LBT2204_ADC5DAC::Dac_Init(void)
{
	//AD5754
	DAC16bit_AD5754::Pin_DacBin = LBT2204_Base::PinGroup[LBT2204_Pin_DacBIN2COMP];
	DAC16bit_AD5754::Pin_DacCS = LBT2204_Base::PinGroup[LBT2204_Pin_DacCS];
	DAC16bit_AD5754::Pin_DacLoad = LBT2204_Base::PinGroup[LBT2204_Pin_DacLoad];
	DAC16bit_AD5754::Pin_DacClr = LBT2204_Base::PinGroup[LBT2204_Pin_DacCLR];
	DAC16bit_AD5754::SPI_Idx = 0;

	//初始化，power up AD5754
	SSPIFunc::ConfirmDevice(0, &LBT2204_Base::LBT2204_SSP0DAC);
	DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP
}

// unsigned long LBT2204_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	return 0;
// }

void LBT2204_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
	SSPIFunc::ConfirmDevice(0, &LBT2204_Base::LBT2204_SSP0DAC);

	DAC16bit_AD5754::Dac_Out(chn,value);
}

// //此为pid电压
// void LBT2204_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {


// }

//clamp voltage
void LBT2204_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{ 
	SSPIFunc::ConfirmDevice(0, &LBT2204_Base::LBT2204_SSP0DAC);
	DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
