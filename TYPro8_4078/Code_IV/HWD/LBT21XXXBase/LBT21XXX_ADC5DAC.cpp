
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short LBT21XXX_ADC5DAC::NeedRefreshDac = 0x0000;



void LBT21XXX_ADC5DAC::Adc_Init(void)
{
#if ADC_CS5560_ENABLE == 1
    if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_CS5560_Enable)
    {

//       Adc24bit_CS5560::Pin_Busy = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcBusy1];
        Adc24bit_CS5560::Pin_Conver = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcConver];
// 	    Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS1];
        Adc24bit_CS5560::Pin_Rst = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcRst];
        Adc24bit_CS5560::SPI_Idx = 1;
// 	    Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

        for(unsigned char uc_m=0; uc_m<2; uc_m++)
        {
            Adc24bit_CS5560::Pin_Busy = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcBusy1+uc_m];
            Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS1+uc_m];
            Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);
        }
    }
#endif
	
    if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_AD7175_Enable)
    {
        Adc24bit_AD7175::ADC_START = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2START];

//      Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1];
// 	   SSPIFunc::setPara(&LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175,1);
// 		 SSPIFunc::my_Init(1);
// 		 ARBUS_DELAY(10);
// 	   Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //��ʼ����������ADC7175

        for(unsigned char uc_m=0; uc_m<2; uc_m++)
        {
            Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1+uc_m];
            SSPIFunc::setPara(&LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175,1);
            SSPIFunc::my_Init(1);
            ARBUS_DELAY(10);
            Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//��ʼ����ѹ����ADC7175
        }

    }

}
void LBT21XXX_ADC5DAC::Dac_Init(void)
{
    if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5754_Enable)
    {
        //AD5754
        // 	DAC16bit_AD5754::Pin_DacBin = TESLA_Base::Pin_Clamp_Dac_Bin;
        DAC16bit_AD5754::Pin_DacCS = LBT21XXX_Base::PinGroup_DAC1[LBT21XXX_Pin_Dac_CS];
        DAC16bit_AD5754::Pin_DacLoad = LBT21XXX_Base::PinGroup_DAC1[LBT21XXX_Pin_Dac_Load];
        DAC16bit_AD5754::Pin_DacClr = LBT21XXX_Base::PinGroup_DAC1[LBT21XXX_Pin_Dac_Clr];
        DAC16bit_AD5754::SPI_Idx = 0;

        //��ʼ����power up AD5754
        SSPIFunc::ConfirmDevice(0, &LBT21XXX_Base::LBT21XXX_SSP0_DAC_5754);
        DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN      BIN COMP
    }
    if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5360_Enable)
    {
        DAC16bit_AD5360::Pin_DacCS = LBT21XXX_Base::PinGroup_DAC2[LBT21XXX_Pin_DacCS];
        DAC16bit_AD5360::Pin_DacBusy = LBT21XXX_Base::PinGroup_DAC2[LBT21XXX_Pin_DacBusy];
        DAC16bit_AD5360::Pin_DacLoad = LBT21XXX_Base::PinGroup_DAC2[LBT21XXX_Pin_DacLoad];
        DAC16bit_AD5360::Pin_DacRst = LBT21XXX_Base::PinGroup_DAC2[LBT21XXX_Pin_DacRst];

        DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);
        ARBUS_DELAY(10000);    //300us reset time  AD5360����
    }
}

void LBT21XXX_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{

    if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5360_Enable)
    {
        SSPIFunc::ConfirmDevice(0, &LBT21XXX_Base::LBT21XXX_SSP0_DAC_5360);
        ARBUS_DELAY(10);
        DAC16bit_AD5360::Dac_out(chn,value);
    }
    else if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5754_Enable)
    {
        SSPIFunc::ConfirmDevice(0, &LBT21XXX_Base::LBT21XXX_SSP0_DAC_5754);
        ARBUS_DELAY(10);
        DAC16bit_AD5754::Dac_Out(chn,value);
    }
}

//��Ϊpid��ѹ
void LBT21XXX_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
    Dac_out_Cur(chn,value); //ʹ�õ������
}

//clamp voltage
void LBT21XXX_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    unsigned long temp = value;   // 20140613  ��ѹ����


// 	switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
// 	{
// 	case CHN_LOCK_SCH:
// 		board_config::bHaveRedundantCheck = 1;
// 		if((board_config::ClampH_Outputflag==0) && (chn==0))
// 		{
    //DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 			board_config::ClampH_Outputflag=1;
//             break;
// 		}
// 		if((board_config::ClampL_Outputflag==0) && (chn==1))
// 		{
    //DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 			board_config::ClampL_Outputflag=1;
//             break;
// 		}
// 		return;

// 	case CHN_LOCK_IDLE:
// 	case CHN_LOCK_THIRD_PARTY:
    //DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
    //break;
// 	default:
    //	board_config::bHaveRedundantCheck = 0;    DKQ 03.11.2015    Dont know why reset to 0!
    //DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
// 		break;
// 	}
    if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5754_Enable)
    {
        SSPIFunc::ConfirmDevice(0, &LBT21XXX_Base::LBT21XXX_SSP0_DAC_5754);
        DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
    }
    else if(LBT21XXX_Base::BoardFeature.BoolFlag.DAC16bit_AD5360_Enable)
    {
        SSPIFunc::ConfirmDevice(0, &LBT21XXX_Base::LBT21XXX_SSP0_DAC_5360);
        DAC16bit_AD5360::Dac_out(chn,temp);
    }
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
