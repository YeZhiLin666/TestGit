#include "../../Entry/includes.h"

void LBT21164_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = LBT21164_Base::Pin_AdcBusy1;
	Adc24bit_CS5560::Pin_Conver = LBT21164_Base::Pin_AdcConver;
	Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS1;
	Adc24bit_CS5560::Pin_Rst = LBT21164_Base::Pin_AdcRst;
	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = LBT21164_Base::Pin_AdcBusy2;
	Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS2; 
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);
}
void LBT21164_ADC5DAC::Dac_Init(void)
{
	DAC16bit_AD5360::Pin_DacCS = LBT21164_Base::Pin_DacCS;
	DAC16bit_AD5360::Pin_DacBusy = LBT21164_Base::Pin_DacBusy;
	DAC16bit_AD5360::Pin_DacLoad = LBT21164_Base::Pin_DacLoad;
	DAC16bit_AD5360::Pin_DacRst = LBT21164_Base::Pin_DacRst;
	DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);   
	ARBUS_DELAY(10000);    //300us reset time  AD5360重置

#if LBT21164_USE_5VDACADC == 1
	if(SSPIFunc::getModuleName(0)  != LBT21164_SSP0_DAC_NAME)
	{
		SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP0DAC,0);
		SSPIFunc::my_Init(0);              //初始化
		ARBUS_DELAY(10);
	}
	DAC16bit_AD5360::Set_DacGain(0,32767);
#endif

	//AD5754
	DAC16bit_AD5754::Pin_DacCS = LBT21164_Base::Pin_Clamp_Dac_CS;
	DAC16bit_AD5754::Pin_DacLoad = LBT21164_Base::Pin_Clamp_Dac_Load;
	DAC16bit_AD5754::Pin_DacClr = LBT21164_Base::Pin_Clamp_Dac_Clr;
	DAC16bit_AD5754::SPI_Idx = 0;
	//初始化，power up AD5754
	unsigned char bOK = SSPIFunc::getModuleName(DAC16bit_AD5754::SPI_Idx);
	if(bOK  != LBT21164_SSP0_AD5754_NAME)
	{
		SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP0VCDAC,DAC16bit_AD5754::SPI_Idx);    //设置参数
		SSPIFunc::my_Init(DAC16bit_AD5754::SPI_Idx);              //初始化
	}
	DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP
}

unsigned long LBT21164_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
{
	unsigned long Result = 0;

	unsigned char bOK = SSPIFunc::getModuleName(1);
	if(bOK  != SSP1_DEVICE1)
	{
		SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP1ADC,1);
		SSPIFunc::my_Init(1);             
		ARBUS_DELAY(10);
	}

	switch(type)
	{
	case ADC_TYPE_I:
		Adc24bit_CS5560::Pin_Busy = LBT21164_Base::Pin_AdcBusy1;
		Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS1;
		Result = Adc24bit_CS5560::Read();
		break;
	case ADC_TYPE_V:
		Adc24bit_CS5560::Pin_Busy = LBT21164_Base::Pin_AdcBusy2;
		Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS2;
		Result = Adc24bit_CS5560::Read();
		break;
	default:
		break;
	}

	return Result;
}

void LBT21164_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
	unsigned char bOK = SSPIFunc::getModuleName(0);
	if( bOK != LBT21164_SSP0_DAC_NAME )
	{
		SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP0DAC,0);
		SSPIFunc::my_Init(0);          
		ARBUS_DELAY(10);
	}
	ARBUS_DELAY(10);

	DAC16bit_AD5360::Dac_out(chn,value);
}
void LBT21164_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
	Dac_out_Cur(chn,value); //使用电流输出
}

//clamp voltage
void LBT21164_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{

}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
