
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short LBT21024FR_ADC5DAC::NeedRefreshDac = 0x0000;         



void LBT21024FR_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_Conver = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcConver];
	Adc24bit_CS5560::Pin_CS = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcCS1];
	Adc24bit_CS5560::Pin_Rst = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcRst];

	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdcCS2]; 

	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

}
void LBT21024FR_ADC5DAC::Dac_Init(void)
{
	//AD53604
		DAC16bit_AD5360::Pin_DacCS = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_DacCS];
	  DAC16bit_AD5360::Pin_DacBusy = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_DacBusy];
	  DAC16bit_AD5360::Pin_DacLoad = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_DacLoad];
	  DAC16bit_AD5360::Pin_DacRst = LBT21024FR_Base::PinGroup[LBT21024FR_Pin_DacRst];

	  DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR); 

	  ARBUS_DELAY(10000);    //300us reset time  AD5360??
	
}

// unsigned long LBT21024FR_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&LBT21024FR_Base::LBT21024FR_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);             
// 		ARBUS_DELAY(10);
// 	}
// 	
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = LBT21024FR_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = LBT21024FR_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = LBT21024FR_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = LBT21024FR_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }

void LBT21024FR_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{


	//unsigned long temp = value;
	SSPIFunc::ConfirmDevice(0, &LBT21024FR_Base::LBT21024FR_SSP0DAC);


   DAC16bit_AD5360::Dac_out(chn,value);
}

//��Ϊpid��ѹ
// void LBT21024FR_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {

// }

//clamp voltage
void LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
//   unsigned long temp = value;   // 20140613  ��ѹ����
	SSPIFunc::ConfirmDevice(0, &LBT21024FR_Base::LBT21024FR_SSP0DAC);
   DAC16bit_AD5360::Dac_out(chn,value);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
