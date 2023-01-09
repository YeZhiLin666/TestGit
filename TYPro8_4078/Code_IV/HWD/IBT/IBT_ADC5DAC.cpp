
#include "../../Entry/includes.h"

void IBT_ADC5DAC::Adc_Init(void)
{
	Adc24bit_CS5560::Pin_Busy = IBT_Base::PinGroup[IBT_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_Conver = IBT_Base::PinGroup[IBT_Pin_AdcConver];
	Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS1];
	Adc24bit_CS5560::Pin_Rst = IBT_Base::PinGroup[IBT_Pin_AdcRst];
	Adc24bit_CS5560::SPI_Idx = 1;
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	Adc24bit_CS5560::Pin_Busy = IBT_Base::PinGroup[IBT_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS2];
	Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

	IBT_Base::ADG_Reset();
}
void IBT_ADC5DAC::Dac_Init(void)
{
	DAC16bit_AD5360::Pin_DacCS = IBT_Base::PinGroup[IBT_Pin_DacCS];
	DAC16bit_AD5360::Pin_DacBusy = IBT_Base::PinGroup[IBT_Pin_DacBusy];
	DAC16bit_AD5360::Pin_DacLoad = IBT_Base::PinGroup[IBT_Pin_DacLoad];
	DAC16bit_AD5360::Pin_DacRst = IBT_Base::PinGroup[IBT_Pin_DacRst];

	DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);   
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
