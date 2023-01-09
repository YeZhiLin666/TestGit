#include "../../Entry/includes.h"

void PWM_ADC5DAC::Adc_Init(void)
{
    Adc16bit_AD7656::Pin_Busy = PWM_Base::PinGroup[PWM_Pin_AdcBusy];
    Adc16bit_AD7656::Pin_ConverA = PWM_Base::PinGroup[PWM_Pin_AdcConverA];
    Adc16bit_AD7656::Pin_ConverB = PWM_Base::PinGroup[PWM_Pin_AdcConverB];
    Adc16bit_AD7656::Pin_ConverC = PWM_Base::PinGroup[PWM_Pin_AdcConverC];
    Adc16bit_AD7656::Pin_CS = PWM_Base::PinGroup[PWM_Pin_AdcCS1];
    Adc16bit_AD7656::Pin_Rst = PWM_Base::PinGroup[PWM_Pin_AdcRst];
//#if ( _PWM_VERSION_ ==0)
    if(board_config::Feature_Enable_init.PWM_Version_Control==0)
        Adc16bit_AD7656::Address = 0X81000002;
//#else
    else
        Adc16bit_AD7656::Address = 0X90000002;
//#endif
    Adc16bit_AD7656::Init(eCONVCODETYPE_BIPOLAR);
//////////////////////初始化AD1259/////////////////////////////
    unsigned short AdcSSP;
    AdcSSP=0x0004;   //后续要从配置文件中读出   dirui  2015/03/20

    unsigned short count[8] = {4500, 2500, 810, 700, 110, 45, 20, 15};	// 10Hz, 16.6Hz, 50Hz, 60Hz, 400Hz, 1200Hz, 3600Hz, 14400Hz
    if(AdcSSP>7)
        PWM_SampleManage::WaitForSampleCounter = 4500;
    else
        PWM_SampleManage::WaitForSampleCounter = count[AdcSSP];

    SSPIFunc::setPara(&PWM_Base::PWM_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化
    Adc24bit_ADS1259::Pin_Conver=PWM_Base::PinGroup[PWM_Pin_Cover];

    Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_C];
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,AdcSSP);   //初始化电流
    Adc24bit_ADS1259::WriteReg(1);

    Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_V];
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,AdcSSP);
    Adc24bit_ADS1259::WriteReg(1);      //初始化电压
    //////////////////////初始化AD1259/////////////////////////////
}
void PWM_ADC5DAC::Dac_Init(void)
{	  	
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_E)
	{
	DAC16bit_AD5764::SPI_Idx     = 0;
	DAC16bit_AD5764::Pin_DacBin  = PWM_Base::PinGroup[PWM_Pin_DacBinComp];
	DAC16bit_AD5764::Pin_DacCS   = PWM_Base::PinGroup[PWM_Pin_DacCS];
	DAC16bit_AD5764::Pin_DacLoad = PWM_Base::PinGroup[PWM_Pin_DacLoad];
	DAC16bit_AD5764::Pin_DacClr  = PWM_Base::PinGroup[PWM_Pin_DacClr];

	SSPIFunc::ConfirmDevice(0, &PWM_Base::PWM_SSP0DAC);
  
  DAC16bit_AD5764::Init(eCONVCODETYPE_UNIPOLAR); 
		
  }
  else
  {
    DAC16bit_AD5754::SPI_Idx     = 0;
    DAC16bit_AD5754::Pin_DacBin  = PWM_Base::PinGroup[PWM_Pin_DacBinComp];
    DAC16bit_AD5754::Pin_DacCS   = PWM_Base::PinGroup[PWM_Pin_DacCS];
    DAC16bit_AD5754::Pin_DacLoad = PWM_Base::PinGroup[PWM_Pin_DacLoad];
    DAC16bit_AD5754::Pin_DacClr  = PWM_Base::PinGroup[PWM_Pin_DacClr];

    SSPIFunc::ConfirmDevice(0, &PWM_Base::PWM_SSP0DAC);

    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);
	}
}

// unsigned long PWM_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;
// 	return Result;
// }

// void PWM_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
// 		SSPIFunc::setPara(&PWM_Base::PWM_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5754::Dac_Out(0,value);
// }
// void PWM_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
//         SSPIFunc::setPara(&PWM_Base::PWM_SSP0DAC,0);
//         SSPIFunc::my_Init(0);
//         ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5754::Dac_Out(1,value);
// }
// void PWM_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
// 		SSPIFunc::setPara(&PWM_Base::PWM_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5754::Dac_Out(2,value);
// }
// void PWM_ADC5DAC::Dac_out_Clamp_Vlt_L(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if(bOK  != SSP0_DEVICE0)
// 	{
// 		SSPIFunc::setPara(&PWM_Base::PWM_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}

// 	DAC16bit_AD5754::Dac_Out(3,value);
// }

/*********************************************************************************
**                            End Of File
*********************************************************************************/
