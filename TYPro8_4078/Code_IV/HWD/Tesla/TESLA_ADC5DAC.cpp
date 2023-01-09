
#include "../../Entry/includes.h"

void TESLA_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = TESLA_Base::PinGroup[TESLA_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_Conver = TESLA_Base::PinGroup[TESLA_Pin_AdcConver];
	Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS1];
	Adc24bit_CS5560::Pin_Rst = TESLA_Base::PinGroup[TESLA_Pin_AdcRst];
	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = TESLA_Base::PinGroup[TESLA_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS2];
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	TESLA_Base::ADG_Reset();
}

void TESLA_ADC5DAC::Dac_Init(void)
{
	//AD5360
	DAC16bit_AD5360::Pin_DacCS = TESLA_Base::PinGroup[TESLA_Pin_DacCS];
	DAC16bit_AD5360::Pin_DacBusy = TESLA_Base::PinGroup[TESLA_Pin_DacBusy];
	DAC16bit_AD5360::Pin_DacLoad = TESLA_Base::PinGroup[TESLA_Pin_DacLoad];
	DAC16bit_AD5360::Pin_DacRst = TESLA_Base::PinGroup[TESLA_Pin_DacRst];

	DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);  

	ARBUS_DELAY(10000);    //300us reset time  AD5360重置

	//AD5754
	DAC16bit_AD5754::Pin_DacBin = TESLA_Base::PinGroup[TESLA_Pin_Clamp_Dac_Bin];
	DAC16bit_AD5754::Pin_DacCS = TESLA_Base::PinGroup[TESLA_Pin_Clamp_Dac_CS];
	DAC16bit_AD5754::Pin_DacLoad = TESLA_Base::PinGroup[TESLA_Pin_Clamp_Dac_Load];
	DAC16bit_AD5754::Pin_DacClr = TESLA_Base::PinGroup[TESLA_Pin_Clamp_Dac_Clr];
	DAC16bit_AD5754::SPI_Idx = 0;

	//初始化，power up AD5754
	SSPIFunc::ConfirmDevice(0, &TESLA_Base::Tesla_SSP0VCDAC);
	DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN à-??        BIN COMP
}


// unsigned long TESLA_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != TESLA_SSP1_ADI_NAME)
// 	{
// 		SSPIFunc::setPara(&TESLA_Base::Tesla_SSP1IADC,1);
// 		SSPIFunc::my_Init(1);              //3?ê??ˉ
// 		ARBUS_DELAY(10);
// 	}

// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = TESLA_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = TESLA_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = TESLA_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = TESLA_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }




//调用底层驱动来完成IBT类型的DAC OUT
//此value为偏移码
//ISET
// void TESLA_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if( bOK != TESLA_SSP0_DAC_NAME )
// 	{
// 		SSPIFunc::setPara(&TESLA_Base::Telsa_SSP0VDAC,0);
// 		SSPIFunc::my_Init(0);           
// 		ARBUS_DELAY(10);
// 	}

// 	ARBUS_DELAY(10);
// 	DAC16bit_AD5360::Dac_out(chn,value);
// }

//数字电压，没有电压环
// void TESLA_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {
// 	Dac_out_Cur(chn,value);    //使用电流输出
// }

//clamp voltage
void TESLA_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
	unsigned long temp = value;
	unsigned char bOK = SSPIFunc::getModuleName(0);
	if(bOK  != TESLA_SSP0_AD5754_NAME)
	{
		SSPIFunc::setPara(&TESLA_Base::Tesla_SSP0VCDAC,0);
		SSPIFunc::my_Init(0);            
		ARBUS_DELAY(10);
	}
	DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754   //000 DACA  001 DACB  010 DACC  011 DACD  100 ALLADC  comment out
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
