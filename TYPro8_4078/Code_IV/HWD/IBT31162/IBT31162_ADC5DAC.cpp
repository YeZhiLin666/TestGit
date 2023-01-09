
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short IBT31162_ADC5DAC::NeedRefreshDac = 0x0000;         



void IBT31162_ADC5DAC::Adc_Init(void)
{
	  SSPIFunc::setPara(&IBT31162_Base::IBT31162_SSP1ADC,1);
    SSPIFunc::my_Init(1);         
    ARBUS_DELAY(10);
	
    Adc24bit_AD7175::ADC_START = IBT31162_Base::PinGroup[IBT31162_Pin_ADCSTART];
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START );
	  ControlPin::SetMeHigh(IBT31162_Base::PinGroup[IBT31162_Pin_CS_C]);
	  ControlPin::SetMeHigh(IBT31162_Base::PinGroup[IBT31162_Pin_CS_V]);
	//  Adc24bit_AD7175::SPI_Idx = 1;  // Do not set SPI_Idx this way, please use as a parameter of fucntions
	  
    Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_C];
	    Adc24bit_AD7175::Init(eCONVCODETYPE_UNIPOLAR,1);  //初始化电流采样ADC7175
//  Adc24bit_AD7175::Init2(eCONVCODETYPE_UNIPOLAR);  //初始化电流采样ADC7175
	                          //ChanIdx, PosIdx, NegIdx, bEnable)
	  Adc24bit_AD7175::ChPinMap(0,       1,      0,      true,1);    //channel 0,enable
	  Adc24bit_AD7175::ChPinMap(1,       1,      0,      false,1);   //channel 1,disable
    Adc24bit_AD7175::ChPinMap(2,       1,      0,      false,1);   //channel 1,disable
	  Adc24bit_AD7175::ChPinMap(3,       1,      0,      false,1);   //channel 1,disable
	
    Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_V]; 
    
    Adc24bit_AD7175::Init(eCONVCODETYPE_UNIPOLAR,1);  //初始化电压采样ADC7175
//  Adc24bit_AD7175::Init2(eCONVCODETYPE_UNIPOLAR);  //初始化电压采样ADC7175
	                          //ChanIdx, PosIdx, NegIdx, bEnable)
	  Adc24bit_AD7175::ChPinMap(0,       1,      0,      true,1);    //channel 0,enable
	  Adc24bit_AD7175::ChPinMap(1,       1,      0,      false,1);   //channel 1,disable
    Adc24bit_AD7175::ChPinMap(2,       1,      0,      false,1);   //channel 1,disable
	  Adc24bit_AD7175::ChPinMap(3,       1,      0,      false,1);   //channel 1,disable
}
void IBT31162_ADC5DAC::Dac_Init(void)
{
	DAC16bit_AD5676::Pin_DacCS = IBT31162_Base::PinGroup[IBT31162_Pin_DacCS3];
	DAC16bit_AD5676::Pin_DacLoad = IBT31162_Base::PinGroup[IBT31162_Pin_DacLoad];
	DAC16bit_AD5676::Pin_DacRst = IBT31162_Base::PinGroup[IBT31162_Pin_DacRst];
  DAC16bit_AD5676::SPI_Idx = 0;
	DAC16bit_AD5676::Init(eCONVCODETYPE_UNIPOLAR); 

	ARBUS_DELAY(10000);    //300us reset time  AD5676重置
    
    DAC16bit_AD5676::Pin_DacCS = IBT31162_Base::PinGroup[IBT31162_Pin_DacCS5];
	DAC16bit_AD5676::Pin_DacLoad = IBT31162_Base::PinGroup[IBT31162_Pin_DacLoad];
	DAC16bit_AD5676::Pin_DacRst = IBT31162_Base::PinGroup[IBT31162_Pin_DacRst];
  DAC16bit_AD5676::SPI_Idx = 0;
	DAC16bit_AD5676::Init(eCONVCODETYPE_UNIPOLAR); 

	ARBUS_DELAY(10000);    //300us reset time  AD5676重置
    
    DAC16bit_AD5686::Pin_DacCS = IBT31162_Base::PinGroup[IBT31162_Pin_Clamp_Dac_CS];
	DAC16bit_AD5686::Pin_DacLoad = IBT31162_Base::PinGroup[IBT31162_Pin_Clamp_Dac_Load];
	DAC16bit_AD5686::Pin_DacRst = IBT31162_Base::PinGroup[IBT31162_Pin_DacRst];

	DAC16bit_AD5686::Init(eCONVCODETYPE_UNIPOLAR); 

	ARBUS_DELAY(10000);    //300us reset time  AD5686重置
    
    for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
	{
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_20)
		{
		     IBT31162_ADC5DAC::Dac_out_Cur(ChannelIdx,0);   // I_SET output 0
		}
	    else
	    {
			IBT31162_ADC5DAC::Dac_out_Cur(ChannelIdx,OS_LogicProcess::Set_DAC_Output_MIN);   // I_SET output 0
	    }
	//      IBT31162_ADC5DAC::Dac_out_Clamp_Vlt(1,DAC_OUT_PUT_MIN);    //VOUTC - DACL
	}
	
}


void IBT31162_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
    unsigned char Dac_ch;
    
    SSPIFunc::ConfirmDevice(0, &IBT31162_Base::IBT31162_SSP0DAC);
    ARBUS_DELAY(10);
    
    if(chn>=8)
    {
		Dac_ch = chn%8;  // 切换成第二个芯片输出，因为一芯片最多8通道。
        DAC16bit_AD5676::Pin_DacCS = IBT31162_Base::PinGroup[IBT31162_Pin_DacCS5];
    }
    else
    {
        Dac_ch = chn;
        DAC16bit_AD5676::Pin_DacCS = IBT31162_Base::PinGroup[IBT31162_Pin_DacCS3];
    }

    DAC16bit_AD5676::Dac_Out(Dac_ch,value);
}

//此为pid电压
void IBT31162_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
	Dac_out_Cur(chn,value); //使用电流输出
}

//clamp voltage
void IBT31162_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    unsigned long temp = value;   // 20140613  电压保护
	SSPIFunc::ConfirmDevice(0, &IBT31162_Base::IBT31162_SSP0VCDAC);

	switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
	{
		case CHN_LOCK_SCH:
			board_config::bHaveRedundantCheck = 1;
			if((board_config::ClampH_Outputflag==0) && (chn==2))
			{
				board_config::ClampH_Outputflag=1;
	            break;
			}
			if((board_config::ClampL_Outputflag==0) && (chn==0))
			{
				board_config::ClampL_Outputflag=1;
	            break;
			}
			break;
			
		case CHN_LOCK_IDLE:
		case CHN_LOCK_THIRD_PARTY:
		    default:
		    break;
	}
    DAC16bit_AD5686::Dac_Out(chn,temp);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
