
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC



void MSTAT21044_ADC5DAC::Adc_Init(void)
{
    Adc24bit_CS5560::Pin_Busy = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcBusy1];
    Adc24bit_CS5560::Pin_Conver = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcConver];
    Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS1];
    Adc24bit_CS5560::Pin_Rst = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcRst];
    Adc24bit_CS5560::SPI_Idx = 1;
    Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

    Adc24bit_CS5560::Pin_Busy = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcBusy2];
    Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS2];
    Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

    MSTAT21044_Base::ADG_Reset();   //delete by qjm 20140708
}
void MSTAT21044_ADC5DAC::Dac_Init(void)
{
    DAC16bit_AD5360::Pin_DacCS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_DacCS];
    DAC16bit_AD5360::Pin_DacBusy = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_DacBusy];
    DAC16bit_AD5360::Pin_DacLoad = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_DacLoad];
    DAC16bit_AD5360::Pin_DacRst = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_DacRst];

    DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);

    ARBUS_DELAY(10000);    //300us reset time  AD5360重置


    //AD5754
    DAC16bit_AD5754::Pin_DacCS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_Clamp_Dac_CS];
    DAC16bit_AD5754::Pin_DacLoad = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_Clamp_Dac_Load];
    DAC16bit_AD5754::Pin_DacClr = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_Clamp_Dac_Clr];
    DAC16bit_AD5754::SPI_Idx = 0;

    //初始化，power up AD5754
    SSPIFunc::ConfirmDevice(0, &MSTAT21044_Base::MSTAT21044_SSP0VCDAC);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN      BIN COMP

}

// unsigned long MSTAT21044_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&MSTAT21044_Base::MSTAT21044_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);
// 		ARBUS_DELAY(10);
// 	}
//
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = MSTAT21044_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = MSTAT21044_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:break;
// 	}

// 	return Result;
// }

// void MSTAT21044_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
// {
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if( bOK != MSTAT21044_SSP0_DAC_NAME )
// 	{
// 		SSPIFunc::setPara(&MSTAT21044_Base::MSTAT21044_SSP0DAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}

// 	ARBUS_DELAY(10);
// 	if(chn<4)
// 	    DAC16bit_AD5360::Dac_out(chn,value);
// 	else
// 		DAC16bit_AD5360::Dac_out((chn+4),value);
// }

// //此为pid电压
// void MSTAT21044_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
// {
// 	Dac_out_Cur(chn,value); //使用电流输出
// }

// //clamp voltage
// void MSTAT21044_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
// {
//     unsigned long temp = value;   // 20140613  电压保护
// 	unsigned char bOK = SSPIFunc::getModuleName(0);
// 	if( SSPIFunc::getModuleName(0)  != MSTAT21044_SSP0_DAC_NAME )
// 	{
// 		SSPIFunc::setPara(&MSTAT21044_Base::MSTAT21044_SSP0VCDAC,0);
// 		SSPIFunc::my_Init(0);
// 		ARBUS_DELAY(10);
// 	}
// 	switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
// 	{
// 		case CHN_LOCK_SCH:
// 			board_config::bHaveRedundantCheck = 1;
// 			if((board_config::ClampH_Outputflag==0) && (chn==0))
// 			{
// 				DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 				board_config::ClampH_Outputflag=1;
// 			}
// 			if((board_config::ClampL_Outputflag==0) && (chn==1))
// 			{
// 				DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 				board_config::ClampL_Outputflag=1;
// 			}
// 			break;
// 		case CHN_LOCK_IDLE:
// 		case CHN_LOCK_THIRD_PARTY:
//             DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
//             break;
// 	    default:
// 			DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 		    break;
//
// 	}
// }

/*********************************************************************************
**                            End Of File
*********************************************************************************/
