#include "../../Entry/includes.h"

#define  IBT_FINE_TUNE_STARTTIME          0.1f
#define  IBT_FINE_TUNE_MINERRORFACTOR     2E-5
#define  IBT_FINE_TUNE_STEPFACTOR         6E-6
#define  IBT_FINE_TUNE_MAXADJUSTFACTOR    0.06f

#define  IBT_ACCURACY                     0.0005f   //精度
#define  IBT_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  IBT_SETTING_TIME                 50        //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask IBT_Base::SetCtrlOpenCh[2];
St_DisposableTask IBT_Base::SetCtrlCloseCh[2];
St_DisposableTask IBT_Base::SetCCToCC[2];
St_DisposableTask IBT_Base::SetRstToOff[2]; 
//St_DisposableTask IBT_Base::SetCVToCV[2];

SSPI_Para  IBT_Base::IBT_SSP0DAC;
SSPI_Para  IBT_Base::IBT_SSP1ADC;
#pragma arm section

unsigned char IBT_Base::SetCtrlOpenCh_Len = 2;
unsigned char IBT_Base::SetCtrlCloseCh_Len = 2;
unsigned char IBT_Base::SetCCToCC_Len = 2;
unsigned char IBT_Base::SetRstToOff_Len = 2;
unsigned char IBT_Base::SetCVToCV_Len = 2;


const St_Port IBT_Base::PinGroup[IBT_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType           Function               Address         
	{1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
	{0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
	{0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
	{0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
	{2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{2,        4,        eCPTYPE_MCUGPIO,    GPIO_O,                0},
};

const unsigned char IBT_Base::m_bHaveVoltageDac = 0;
const unsigned char IBT_Base::m_bHaveClampVdac = 0;
const unsigned char IBT_Base::m_bChannelBasedClampVdac = 0;

volatile St_IBT_Relay IBT_Base::IBT_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

void IBT_Base::Hardware_Init(void)
{
    IBT_FeatureEnable_Init();  //added by xudong
	IO_Init();    //内部IO初始化
	Feed_CpldWD();  //喂狗信号

	board_config::bHaveRedundantCheck = 0;

	CPLD_FUNC::Init();
	memset((void *)(&IBT_relaystatus[0]), 0, MAXCHANNELNO);
	Feed_CpldWD();  // 喂狗信号

	for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
	{
		IBT_relaystatus[ChannelIdx].Bits.R_ISO = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);

//#if _IBT_VERSION_ == IBT_VER_C    
        if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_C )
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
		    GPIOFunc::Set_G_Port(2,4);
		}
//#elif _IBT_VERSION_ == IBT_VER_D
       else if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_D )
       {
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
		    GPIOFunc::Set_G_Port(2,4);
	   }
//#else
       else
	   {
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 0;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
       }
//#endif
	}

	//IBT A,B板为高有效，C，D板为低有效   comment out chen 20130913
//#if _IBT_VERSION_ == IBT_VER_C                             //总的relay继电器
    if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_C )
	     ControlPin::SetMeLow(IBT_Base::PinGroup[IBT_Pin_PowerOn]);
//#elif _IBT_VERSION_ == IBT_VER_D
    else if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_D )
	    ControlPin::SetMeLow(IBT_Base::PinGroup[IBT_Pin_PowerOn]);
//#else
    else
	    ControlPin::SetMeHigh(IBT_Base::PinGroup[IBT_Pin_PowerOn]);
//#endif

	//SSP0 for DAC    AD5360
	IBT_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
	IBT_SSP0DAC.uc_Speed = 5;
	IBT_SSP0DAC.uc_CPOL = 0;
	IBT_SSP0DAC.uc_CPHA = 1;
	IBT_SSP0DAC.uc_MSTR = 1;    //master
	IBT_SSP0DAC.uc_BITS = 8;
	IBT_SSP0DAC.uc_LSBF = 0;

	//SSP1 for ADC
	IBT_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	IBT_SSP1ADC.uc_Speed = 5;
	IBT_SSP1ADC.uc_CPOL = 0;
	IBT_SSP1ADC.uc_CPHA = 0;       //rising trig
	IBT_SSP1ADC.uc_MSTR = 1;    //master
	IBT_SSP1ADC.uc_BITS = 8;
	IBT_SSP1ADC.uc_LSBF = 0;

	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	Feed_CpldWD();  // 喂狗信号

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	Feed_CpldWD();  // 喂狗信号

	for(unsigned char ChannelIdx =0;ChannelIdx<MAXCHANNELNO;ChannelIdx++)
		IBT_Base::Adjust_CHN_Gain(ChannelIdx,10);

	IBT_Base::Adjust_ADC_Gain(0,0);
	//	IBT_TempSample::AD_Init(8);  //根据硬件调整通道数值 add by qjm20140613

	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	//	AdditionalEngine();
}

void IBT_Base::IBT_FeatureEnable_Init(void) 
{
	static St_Hardware_Feature_Enable para;
	memset(&para,0,sizeof(para));
	
// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_D)
// 	{
// 	para.Redundant_Safety_Mode=1;
	para.OS_Tick_Interval=800;
// 	}
	
	board_config::Feature_Enable_init=&para;
} 

void IBT_Base::IO_Init(void)
{
	for(int i=0;i<IBT_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
} 

void IBT_Base::ADC_Init(void)
{	
	IBT_ADC5DAC::Adc_Init();
}
void IBT_Base::DAC_Init(void)
{
	IBT_ADC5DAC::Dac_Init();

	board_config::DacInfo.m_bCurrRngCount = 1;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = IBT_SETTING_TIME;

	ARBUS_DELAY(10000);    //300us reset time  AD5360????
}

void IBT_Base::Software_Init(void)
{
	Func_Init();//函数指针初始化  四个基本操作
	EngineStuff();   //引擎结构指针初始化

	// 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)    //从0开始
	// 	{
	// 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//注册chn info
	// 		regEngine(i,board_config::st_channel_info[i].Channel_type);//注册引擎				
	// 	}

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = IBT_FINE_TUNE_STARTTIME;    //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = IBT_FINE_TUNE_MINERRORFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = IBT_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = IBT_FINE_TUNE_STEPFACTOR;

	safety_check::m_CtrlValueSetting.fErrorFactor       = IBT_ACCURACY * IBT_CTRL_VALUE_ERROR_FACTOR;

	board_config::Accuracy = IBT_ACCURACY;

	IBT_SampleManage::Init();
}

void IBT_Base::Func_Init(void)
{
	// 	board_config::Func_Adc_rd=&IBT_Base::ADC_rd;
	board_config::Func_Dac_out=&IBT_Base::DAC_out;
	board_config::Func_SetRelay =&IBT_Base::SetRelay;
	board_config::Func_Sample = &IBT_SampleManage::SampleISR;
	board_config::Func_SetRange = &IBT_Base::SetRange;
	//	board_config::Func_TempSample = &IBT_TempSample::Temp_rd;  // 20140613  温度测量
	board_config::FeedWatchDog = &IBT_Base::Feed_CpldWD;
}

//void IBT_Base::AdditionalEngine(void)
//{

//}
void IBT_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
	//memset(&SetCVToCV,       0,    sizeof(SetCVToCV));

	//---------------------------------------------//
	SetCtrlOpenCh[0].uw32_DelayTime = 100;    //加大延迟，防止继电器对AD8253干扰  20130227  comment out chen
	SetCtrlOpenCh[0].RelayMove.ActiveFlag = (IBT_RLY_CHL);
	SetCtrlOpenCh[0].RelayMove.InactiveFlag = (IBT_RLY_Cali);

	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlOpenCh[1].uw32_DelayTime = 0;
	SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //输出量程继电器
	SetCtrlOpenCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR;

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 10;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[0].RangeChangedFlag = 0;
	//SetCtrlCloseCh[1].uw32_DelayTime = 0;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = IBT_RLY_Cali;  //清掉Calib
	SetCtrlCloseCh[1].RelayMove.InactiveFlag = IBT_RLY_CHL;    //打开电流电压端检测
	SetCtrlCloseCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[1].RangeChangedFlag = 0;

	//---------------------------------------------//
	SetCCToCC[0].uw32_DelayTime = 10;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR;
	//将来有可能是关掉
	//SetCCToCC[0].RangeChangedFlag = 0;
	//SetCCToCC[1].uw32_DelayTime = 0;
	SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR;
	SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //直接输出下一量程
	//SetCCToCC[1].RangeChangedFlag = 0;

	//---------------------------------------------//
	SetRstToOff[0].uw32_DelayTime = 10;
	//SetRstToOff[0].DacMove.DacFlag = 0;
	//SetRstToOff[0].RangeChangedFlag = 0;

	//SetRstToOff[1].uw32_DelayTime = 0;
	//SetRstToOff[1].DacMove.DacFlag = 0;
	//SetRstToOff[1].RangeChangedFlag = 0;

	// 	SetCVToCV[0].uw32_DelayTime = 10;
	// 	SetCVToCV[0].DacMove.DacFlag  = ACTIVE_DAC_CUR;
	// 	SetCVToCV[0].RangeChangedFlag = 0;

	// 	SetCVToCV[1].uw32_DelayTime = 0;
	// 	SetCVToCV[1].DacMove.DacFlag = ACTIVE_DAC_CUR;
	// 	SetCVToCV[1].RangeChangedFlag = 0;
}
/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char IBT_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
	if(type == ACTIVE_DAC_CUR)
	{
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码

		SSPIFunc::ConfirmDevice(0, &IBT_Base::IBT_SSP0DAC);
		// 		if(SSPIFunc::getModuleName(0)  != SSP0_DEVICE1)
		// 		{
		// 			SSPIFunc::setPara(&IBT_Base::IBT_SSP0DAC,0);
		// 			SSPIFunc::my_Init(0);              //3?ê??ˉ
		// 			ARBUS_DELAY(10);
		// 		}

		ChannelIdx = g_chn_order[ChannelIdx];    //comment out chen
		DAC16bit_AD5360::Dac_out(ChannelIdx,Data);
		return 1;
	}
	else
		return 0;
}

void IBT_Base::Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain)    
{
	switch(Gain)
	{
	case 1:
		IBT_relaystatus[ChannelIdx].Bits.AdcGain = 0;
		break;
	case 10:
		IBT_relaystatus[ChannelIdx].Bits.AdcGain = 1;
		break;
	case 100:
		IBT_relaystatus[ChannelIdx].Bits.AdcGain = 2;
		break;
	case 1000:
	default:
		IBT_relaystatus[ChannelIdx].Bits.AdcGain = 3;
		break;
	}
	IBT_relaystatus[ChannelIdx].Bits.AdcGainEnable = 1;

	CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
	IBT_relaystatus[ChannelIdx].Bits.AdcGainEnable = 0;
	CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
}
void IBT_Base::Adjust_ADC_Gain(unsigned char type,unsigned char Gain)
{
	St_IBT_PGA MyReg;

	MyReg.Bits.AdcGain_I = 0;
	MyReg.Bits.AdcGain_V = 0;
	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 0;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);
}

//ADC模拟门打开
void IBT_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void IBT_Base::ADG_Reset(void)
{
	ControlPin::SetMeLow(IBT_Base::PinGroup[IBT_Pin_AdgRst]);
	ARBUS_DELAY(5);
	ControlPin::SetMeHigh(IBT_Base::PinGroup[IBT_Pin_AdgRst]);
}

void IBT_Base::changeADCRange(unsigned char ChannelIdx,unsigned char range)
{
	ARBUS_DELAY(10);

	switch(range)
	{
	case CURRENT_RANGE_1:
		IBT_Base::Adjust_CHN_Gain(ChannelIdx,10);   
		break;
	case CURRENT_RANGE_2:
		IBT_Base::Adjust_CHN_Gain(ChannelIdx,100);   
		break;
	case CURRENT_RANGE_3:
		IBT_Base::Adjust_CHN_Gain(ChannelIdx,1000);    
		break;
	default:
		IBT_Base::Adjust_CHN_Gain(ChannelIdx,10);    
		break;			
	}
}
char IBT_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	changeADCRange(ChannelIdx,CURRENT_RANGE_1);  //IBT have only one Current Range

	return 1;
}
void IBT_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	//  unsigned short FlagTemp = 0;
	ChannelIdx = g_chn_order[ChannelIdx];

	IBT_relaystatus[ChannelIdx].Reg |= SetRly;
	IBT_relaystatus[ChannelIdx].Reg &= ~ClrRly;

	if(bAutoCali)
	{
		IBT_relaystatus[ChannelIdx].Bits.R_CHL = 0;
		//    FlagTemp = IBT_relaystatus[ChannelIdx].Reg;
//#if _IBT_VERSION_ == IBT_VER_C
        if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion == BOARDTYPE_VER_C )
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 0;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);

		    GPIOFunc::Clr_G_Port(2,4);
		}
//#elif _IBT_VERSION_ == IBT_VER_D
		else if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion == BOARDTYPE_VER_D)
		{
		    unsigned short FlagTemp = IBT_relaystatus[ChannelIdx].Reg;
		    if (((FlagTemp & IBT_RLY_Cali) == IBT_RLY_Cali ))   //当需要清掉该通道的自动开
		    {
			    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;  
			    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
			    GPIOFunc::Set_G_Port(2,4);
		    }
		    else
		    {
			    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 0;  
			    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
			    GPIOFunc::Clr_G_Port(2,4);
		    }
		}
//#else
        else
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
        }
//#endif
	}
	else
	{
//#if _IBT_VERSION_ == IBT_VER_C
        if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_C )
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
		    GPIOFunc::Set_G_Port(2,4);
		}
//#elif _IBT_VERSION_ == IBT_VER_D
        else if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_D ) 
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 1;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
		    GPIOFunc::Set_G_Port(2,4);
		}
//#else
        else
		{
		    IBT_relaystatus[ChannelIdx].Bits.R_Cali = 0;
		    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT_relaystatus[ChannelIdx].Reg);
		}
//#endif
	}
}

void IBT_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(IBT_Base::PinGroup[IBT_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0); //100 before 11/16/2015
	ControlPin::SetMeLow(IBT_Base::PinGroup[IBT_Pin_WD]);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
