
#include "../../Entry/includes.h"


void LBT2401_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = LBT2401_Base::PinGroup[LBT2401_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_Conver = LBT2401_Base::PinGroup[LBT2401_Pin_AdcConver];
	Adc24bit_CS5560::Pin_CS = LBT2401_Base::PinGroup[LBT2401_Pin_AdcCS1];
	Adc24bit_CS5560::Pin_Rst = LBT2401_Base::PinGroup[LBT2401_Pin_AdcRst];
	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = LBT2401_Base::PinGroup[LBT2401_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = LBT2401_Base::PinGroup[LBT2401_Pin_AdcCS2]; 
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);
}
void LBT2401_ADC5DAC::Dac_Init(void)
{
	//AD5754
	DAC16bit_AD5754::Pin_DacBin = LBT2401_Base::PinGroup[LBT2401_Pin_Dac_Bin];
	DAC16bit_AD5754::Pin_DacCS = LBT2401_Base::PinGroup[LBT2401_Pin_Dac_CS];
	DAC16bit_AD5754::Pin_DacLoad = LBT2401_Base::PinGroup[LBT2401_Pin_Dac_Load];
	DAC16bit_AD5754::Pin_DacClr = LBT2401_Base::PinGroup[LBT2401_Pin_Dac_Clr];
	DAC16bit_AD5754::SPI_Idx = 0;

	//初始化，power up AD5754
	SSPIFunc::ConfirmDevice(0, &LBT2401_Base::LBT2401_SSP0DAC);
	DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP
}

// unsigned long LBT2401_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&LBT2401_Base::LBT2401_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);             
// 		ARBUS_DELAY(10);
// 	}
// 	
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = LBT2401_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = LBT2401_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = LBT2401_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = LBT2401_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }

void LBT2401_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{


//	unsigned long temp = value;
	SSPIFunc::ConfirmDevice(0, &LBT2401_Base::LBT2401_SSP0DAC);
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
	DAC16bit_AD5754::Dac_Out(chn,value);
}

//此为pid电压
// void LBT2401_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {

// }

//clamp voltage
void LBT2401_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
	SSPIFunc::ConfirmDevice(0, &LBT2401_Base::LBT2401_SSP0DAC);
	DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
