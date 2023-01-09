
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short LBT20162_ADC5DAC::NeedRefreshDac = 0x0000;



void LBT20162_ADC5DAC::Adc_Init(void)
{
    Adc24bit_AD7175::ADC_START = LBT20162_Base::PinGroup[LBT20162_Pin_AdcSTART];

    Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];
    SSPIFunc::setPara(&LBT20162_Base::LBT20162_SSP1ADC,1);
    SSPIFunc::my_Init(1);
    ARBUS_DELAY(10);
    Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //初始化电流采样ADC7175

    Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
    SSPIFunc::setPara(&LBT20162_Base::LBT20162_SSP1ADC,1);
    SSPIFunc::my_Init(1);
    ARBUS_DELAY(10);
    Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//初始化电压采样ADC7175
}
void LBT20162_ADC5DAC::Dac_Init(void)
{
    DAC16bit_AD5360::Pin_DacCS = LBT20162_Base::PinGroup[LBT20162_Pin_DacCS];
    DAC16bit_AD5360::Pin_DacBusy = LBT20162_Base::PinGroup[LBT20162_Pin_DacBusy];
    DAC16bit_AD5360::Pin_DacLoad = LBT20162_Base::PinGroup[LBT20162_Pin_DacLoad];
    DAC16bit_AD5360::Pin_DacRst = LBT20162_Base::PinGroup[LBT20162_Pin_DacRst];

    DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);

    ARBUS_DELAY(10000);    //300us reset time  AD5360重置

}


void LBT20162_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{

    SSPIFunc::ConfirmDevice(0, &LBT20162_Base::LBT20162_SSP0DAC);
    ARBUS_DELAY(10);

    DAC16bit_AD5360::Dac_out(chn,value);
}

//此为pid电压
void LBT20162_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
    Dac_out_Cur(chn,value); //使用电流输出
}

//clamp voltage
// void LBT20162_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
// {
//     unsigned long temp = value;   // 20140613  电压保护
// 	SSPIFunc::ConfirmDevice(0, &LBT20162_Base::LBT20162_SSP0VCDAC);
// 	switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
// 	{
// 		case CHN_LOCK_SCH:
// 			board_config::bHaveRedundantCheck = 1;
// 			if((board_config::ClampH_Outputflag==0) && (chn==0))
// 			{
// 				board_config::ClampH_Outputflag=1;
// 				break;
// 			}
// 			if((board_config::ClampL_Outputflag==0) && (chn==1))
// 			{
// 				board_config::ClampL_Outputflag=1;
// 				break;
// 			}
// 		    return;
// 		case CHN_LOCK_IDLE:
// 		case CHN_LOCK_THIRD_PARTY:
// 	  default:
// 		    break;
// 	}
// 	DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// }

/*********************************************************************************
**                            End Of File
*********************************************************************************/
