
#include "../../Entry/includes.h"

////////////////////////////////////////////////////DAC
//unsigned short LBT21084VB_ADC5DAC::NeedRefreshDac = 0x0000;         



void LBT21084VB_ADC5DAC::Adc_Init(void)
{
	   Adc24bit_AD7175::ADC_START = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcSTART];
    
     Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];
	   SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP1ADC,1);
		 SSPIFunc::my_Init(1);         
		 ARBUS_DELAY(10);
	   Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //初始化电流采样ADC7175
    
     Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];
	   SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP1ADC,1);
		 SSPIFunc::my_Init(1);         
		 ARBUS_DELAY(10);
	   Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//初始化电压采样ADC7175
}
void LBT21084VB_ADC5DAC::Dac_Init(void)
{
	DAC16bit_AD5360::Pin_DacCS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_DacCS];
	DAC16bit_AD5360::Pin_DacBusy = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_DacBusy];
	DAC16bit_AD5360::Pin_DacLoad = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_DacLoad];
	DAC16bit_AD5360::Pin_DacRst = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_DacRst];

	DAC16bit_AD5360::Init(eCONVCODETYPE_BIPOLAR); 

	ARBUS_DELAY(10000);    //300us reset time  AD5360重置
	
	
	//AD5754
// 	DAC16bit_AD5754::Pin_DacBin = LBT21084VB_Base::PinGroup[LBT21084VB_Clamp_Dac_Bin];
	DAC16bit_AD5754::Pin_DacCS = LBT21084VB_Base::PinGroup[LBT21084VB_Clamp_Dac_CS];
	DAC16bit_AD5754::Pin_DacLoad = LBT21084VB_Base::PinGroup[LBT21084VB_Clamp_Dac_Load];
	DAC16bit_AD5754::Pin_DacClr = LBT21084VB_Base::PinGroup[LBT21084VB_Clamp_Dac_Clr];
    DAC16bit_AD5754::SPI_Idx = 0;
	
	//初始化，power up AD5754
	//unsigned char bOK = SSPIFunc::getModuleName(DAC16bit_AD5754::SPI_Idx);
	//if(bOK  != LBT21084VB_SSP0_AD5754_NAME)
	//{
 	//	SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP0VCDAC,DAC16bit_AD5754::SPI_Idx);    //设置参数
 	//	SSPIFunc::my_Init(DAC16bit_AD5754::SPI_Idx);              //初始化
 	//}
	SSPIFunc::ConfirmDevice(DAC16bit_AD5754::SPI_Idx, &LBT21084VB_Base::LBT21084VB_SSP0VCDAC);
	DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN      BIN COMP
	
}

// unsigned long LBT21084VB_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);             
// 		ARBUS_DELAY(10);
// 	}
// 	
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = LBT21084VB_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = LBT21084VB_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = LBT21084VB_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = LBT21084VB_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }

void LBT21084VB_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
	//unsigned char bOK = SSPIFunc::getModuleName(0);
	//if( bOK != LBT21084VB_SSP0_DAC_NAME )
	//{
	//	SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP0DAC,0);
	//	SSPIFunc::my_Init(0);          
	//	ARBUS_DELAY(10);
	//}
    SSPIFunc::ConfirmDevice(0, &LBT21084VB_Base::LBT21084VB_SSP0DAC);
	ARBUS_DELAY(10);
	
	/*if(chn<4)
	    DAC16bit_AD5360::Dac_out(chn,value);
	else
		DAC16bit_AD5360::Dac_out((chn+4),value);*/
	if(chn>=4)
		chn += 4;
	DAC16bit_AD5360::Dac_out(chn,value);
}

//此为pid电压
void LBT21084VB_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
	Dac_out_Cur(chn,value); //使用电流输出
}

//clamp voltage
void LBT21084VB_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    unsigned long temp = value;   // 20140613  电压保护
	//unsigned char bOK = SSPIFunc::getModuleName(0);
	//if( SSPIFunc::getModuleName(0)  != LBT21084VB_SSP0_DAC_NAME )
	//{
	//	SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP0VCDAC,0);
	//	SSPIFunc::my_Init(0);         
	//	ARBUS_DELAY(10);
	//}
	SSPIFunc::ConfirmDevice(0, &LBT21084VB_Base::LBT21084VB_SSP0VCDAC);
	switch(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2)
	{
		case CHN_LOCK_SCH:
			board_config::bHaveRedundantCheck = 1;
			if((board_config::ClampH_Outputflag==0) && (chn==0))
			{
				//DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
				board_config::ClampH_Outputflag=1;
				break;
			}
			if((board_config::ClampL_Outputflag==0) && (chn==1))
			{
				//DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
				board_config::ClampL_Outputflag=1;
				break;
			}

		    return;
			//break;
			
		case CHN_LOCK_IDLE:
		case CHN_LOCK_THIRD_PARTY:
            //DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
		    //break;
	    default:
		//	board_config::bHaveRedundantCheck = 0;    DKQ 03.11.2015    Dont know why reset to 0!
			//DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
		    break;
	}
	DAC16bit_AD5754::Dac_Out(chn,temp);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
