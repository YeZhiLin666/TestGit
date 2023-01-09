
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short LBT21024_ADC5DAC::NeedRefreshDac = 0x0000;



void LBT21024_ADC5DAC::Adc_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C)
    {
        Adc24bit_CS5560::Pin_Busy = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcBusy1];
        Adc24bit_CS5560::Pin_Conver = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcConver];
        Adc24bit_CS5560::Pin_CS = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcCS1];
        Adc24bit_CS5560::Pin_Rst = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcRst];
        Adc24bit_CS5560::SPI_Idx = 1;
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

        Adc24bit_CS5560::Pin_Busy = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcBusy2];
        Adc24bit_CS5560::Pin_CS = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcCS2];
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);
    }
    else
    {
        Adc24bit_AD7175::ADC_START = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2START];

        Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS1];
        SSPIFunc::setPara(&LBT21024_Base::LBT21024_SSP1ADC,1);
        SSPIFunc::my_Init(1);
        ARBUS_DELAY(10);
        Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //初始化电流采样ADC7175

        Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];
        SSPIFunc::setPara(&LBT21024_Base::LBT21024_SSP1ADC,1);
        SSPIFunc::my_Init(1);
        ARBUS_DELAY(10);
        Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//初始化电压采样ADC7175
    }
}
void LBT21024_ADC5DAC::Dac_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <=BOARDTYPE_VER_B )
    {   //AD5754
        DAC16bit_AD5754::Pin_DacCS = LBT21024_Base::PinGroup_DAC1[LBT21024_Pin_Dac_CS];
        DAC16bit_AD5754::Pin_DacLoad = LBT21024_Base::PinGroup_DAC1[LBT21024_Pin_Dac_Load];
        DAC16bit_AD5754::Pin_DacClr = LBT21024_Base::PinGroup_DAC1[LBT21024_Pin_Dac_Clr];
        DAC16bit_AD5754::SPI_Idx = 0;

        //初始化，power up AD5754
        SSPIFunc::ConfirmDevice(0, &LBT21024_Base::LBT21024_SSP0DAC);
        DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN     BIN COMP
    }
    else
    {
        //AD5360
        DAC16bit_AD5360::Pin_DacCS = LBT21024_Base::PinGroup_DAC2[LBT21024_Pin_DacCS];
        DAC16bit_AD5360::Pin_DacBusy = LBT21024_Base::PinGroup_DAC2[LBT21024_Pin_DacBusy];
        DAC16bit_AD5360::Pin_DacLoad = LBT21024_Base::PinGroup_DAC2[LBT21024_Pin_DacLoad];
        DAC16bit_AD5360::Pin_DacRst = LBT21024_Base::PinGroup_DAC2[LBT21024_Pin_DacRst];

        DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR);
// 	    ARBUS_DELAY(10000);    //300us reset time  AD5360??
    }
}


void LBT21024_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{

    SSPIFunc::ConfirmDevice(0, &LBT21024_Base::LBT21024_SSP0DAC);
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <=BOARDTYPE_VER_B )
    {
        value = 0xFFFFFF-value;
        DAC16bit_AD5754::Dac_Out(chn,value);
    }
    else
    {
        DAC16bit_AD5360::Dac_out(chn,value);
    }
}

//clamp voltage
void LBT21024_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &LBT21024_Base::LBT21024_SSP0DAC);
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <=BOARDTYPE_VER_B )
    {
        unsigned long temp = value;   // 20140613  电压保护
        switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
        {
        case CHN_LOCK_SCH:
            board_config::bHaveRedundantCheck = 1;
            if((board_config::ClampH_Outputflag==0) && (chn==0))
            {
                DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
                board_config::ClampH_Outputflag=1;
            }
            if((board_config::ClampL_Outputflag==0) && (chn==1))
            {
                DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
                board_config::ClampL_Outputflag=1;
            }
            break;

        case CHN_LOCK_IDLE:
        case CHN_LOCK_THIRD_PARTY:
            //break;
        default:
            DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
            break;
        }
    }
    else
    {
        DAC16bit_AD5360::Dac_out(chn,value);
    }
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
