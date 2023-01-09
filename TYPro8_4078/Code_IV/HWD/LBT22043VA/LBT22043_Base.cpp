#include "includes.h"

#define  LBT22043_FINE_TUNE_STARTTIME          0.1f
#define  LBT22043_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT22043_FINE_TUNE_STEPFACTOR         6E-6
#define  LBT22043_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  LBT22043_ACCURACY                     0.0005f   //精度
#define  LBT22043_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT22043_SETTING_TIME                 500       //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask LBT22043_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT22043_Base::SetCtrlCloseCh[5];
St_DisposableTask LBT22043_Base::SetCCToCC[7];
St_DisposableTask LBT22043_Base::SetRstToOff[1];

SSPI_Para  LBT22043_Base::LBT22043_SSP0DAC;
#pragma arm section 

unsigned char LBT22043_Base::SetCtrlOpenCh_Len = 5;    //this is maybe problem 20131107 chen
unsigned char LBT22043_Base::SetCtrlCloseCh_Len = 5;
unsigned char LBT22043_Base::SetCCToCC_Len = 7;
unsigned char LBT22043_Base::SetRstToOff_Len = 1;

const St_Port LBT22043_Base::PinGroup[LBT22043_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType         Function               Address              
	{1,        12,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        19,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        18,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P3.23
	{0,        10,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.3
	{0,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.2
	{2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.21
	{2,        5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.5
	{0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.5
	{2,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
};

const unsigned char LBT22043_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT22043_Base::m_bHaveClampVdac = 1;
const unsigned char LBT22043_Base::m_bChannelBasedClampVdac = 0;
void LBT22043_Base::Hardware_Init(void)
{
  FeatureEnable_Init();  
	IO_Init();    

	board_config::bHaveRedundantCheck = 1;
	board_config::bFanAdj=1;

	//SSP0 for DAC    AD5754
	LBT22043_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
	LBT22043_SSP0DAC.uc_Speed = 5;
	LBT22043_SSP0DAC.uc_CPOL = 0;
	LBT22043_SSP0DAC.uc_CPHA = 1;
	LBT22043_SSP0DAC.uc_MSTR = 1;    //master
	LBT22043_SSP0DAC.uc_BITS = 8;
	LBT22043_SSP0DAC.uc_LSBF = 0;

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	//RANGE INIT      
	ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_H]);
	ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_L]);
	//DAC INIT
	LBT22043_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
	Feed_CpldWD();
	//	LBT22043_TempSample::AD_Init(4);
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	//	AdditionalEngine();

	//拉高钳压    // 20140613  电压保护
	LBT22043_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	LBT22043_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);


	FanAdj_task::FANADJ_INTERVAL=3000;
	FanAdj_task::VF=50.3;
	FanAdj_task::VR=0;
	FanAdj_task::HeatSinkCounter=8;
	FanAdj_task::AdjMod=I2CMOD;
	// 风扇初始化 
	I2CBusNew::AD5245_SET(0,160,I2C1);  //此处的100 改为0~255的数即可
}
void LBT22043_Base::FeatureEnable_Init(void)
{
   board_config::Feature_Enable_init->Power_Off_Enable =1;
	 board_config::Feature_Enable_init->Fan_Safety_Mode =1;
// 	 board_config::Feature_Enable_init->Mcu_PWMFan_Enable = 1;
	 board_config::Feature_Enable_init->OS_Tick_Interval =800;

}
void LBT22043_Base::IO_Init(void)
{
	for(int i=0;i<LBT22043_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);

	GPIOFunc::InitPortFunc(PinGroup[LBT22043_Pin_CL_Cal].Port,
		PinGroup[LBT22043_Pin_CL_Cal].Bits,
		PinGroup[LBT22043_Pin_CL_Cal].Function,
		1);
} 
void LBT22043_Base::ADC_Init(void)
{	
	LBT22043_ADC5DAC::Adc_Init();
	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}
void LBT22043_Base::DAC_Init(void)
{
	LBT22043_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 3;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT22043_SETTING_TIME;

	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT22043_Base::Software_Init(void)
{
	Func_Init();
	EngineStuff(); 

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT22043_FINE_TUNE_STARTTIME;   //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT22043_FINE_TUNE_MINERRORFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT22043_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT22043_FINE_TUNE_STEPFACTOR;

	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT22043_ACCURACY * LBT22043_CTRL_VALUE_ERROR_FACTOR;

	LBT22043_SampleManage::Init();
}

void LBT22043_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&LBT22043_Base::DAC_out;
	board_config::Func_SetRange=&LBT22043_Base::SetRange;
	board_config::Func_SetRelay=&LBT22043_Base::SetRelay;
	board_config::Func_Sample =&LBT22043_SampleManage::SampleISR;    //timer0 中断
	//	board_config::Func_TempSample = &LBT22043_TempSample::Temp_rd;   // 20140613  温度测量
	board_config::Func_RedundantSafety= &LBT22043_Base::RedundantSafety; //冗余电压保护
	board_config::Func_ReleaseRedundantSafety=&LBT22043_Base::ReleaseRedundantSafety;//冗余解除操作
	board_config::Func_FanAdj=&LBT22043_Base::FanAdjOperate;
	board_config::FeedWatchDog = &LBT22043_Base::Feed_CpldWD;
}

//void LBT22043_Base::AdditionalEngine(void)
//{
//}

// void LBT22043_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {

// }
// void LBT22043_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {

// }
void LBT22043_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));

	//---------------------------------------------//
	SetCtrlOpenCh[0].uw32_DelayTime = 200;    
	SetCtrlOpenCh[0].RelayMove.ActiveFlag = (LBT22043_RLY_Protect | LBT22043_RLY_OffGate);
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlOpenCh[1].uw32_DelayTime = 200;  	
	SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetCtrlOpenCh[2].uw32_DelayTime = 200;
	SetCtrlOpenCh[2].RelayMove.InactiveFlag = (LBT22043_RLY_Protect);

	SetCtrlOpenCh[3].uw32_DelayTime = 200;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT22043_RLY_OffGate;  

	//SetCtrlOpenCh[4].uw32_DelayTime = 0; 
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[1].uw32_DelayTime = 200;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT22043_RLY_OffGate;

	SetCtrlCloseCh[2].uw32_DelayTime = 200;
	SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT22043_RLY_Protect;    

	SetCtrlCloseCh[3].uw32_DelayTime = 200;
	SetCtrlCloseCh[3].RelayMove.InactiveFlag = LBT22043_RLY_Output;

	//SetCtrlCloseCh[4].uw32_DelayTime = 0;
	SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT22043_RLY_Protect;

	//---------------------------------------------//

	SetCCToCC[0].uw32_DelayTime = 200;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 200;
	SetCCToCC[1].RelayMove.ActiveFlag = LBT22043_RLY_OffGate;
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 200;
	SetCCToCC[2].RelayMove.ActiveFlag = LBT22043_RLY_Protect;
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 200;
	SetCCToCC[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;   
	SetCCToCC[3].RangeChangedFlag = 1;

	SetCCToCC[4].uw32_DelayTime = 200;
	SetCCToCC[4].RelayMove.InactiveFlag = LBT22043_RLY_Protect;     
	SetCCToCC[4].RangeChangedFlag = 1;

	SetCCToCC[5].uw32_DelayTime = 200;
	SetCCToCC[5].RelayMove.InactiveFlag = LBT22043_RLY_OffGate;    
	SetCCToCC[5].RangeChangedFlag = 1;

	SetCCToCC[6].uw32_DelayTime = 200;    
	SetCCToCC[6].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 10;
	//SetRstToOff[0].DacMove.DacFlag = 0;
	//SetRstToOff[0].RangeChangedFlag = 0;
}
char LBT22043_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
	unsigned long value = Data;    //强制保护

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		LBT22043_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
		break;
		//钳位
	case ACTIVE_DAC_VH:
		LBT22043_ADC5DAC::Dac_out_Clamp_Vlt(2,0xffffff);    //VOUTA - DACH  // 20140613  电压保护
		break;
	case ACTIVE_DAC_VL:
		LBT22043_ADC5DAC::Dac_out_Clamp_Vlt(3,0);    //VOUTC - DACL
		break;
	default:
		break;
	}
	return 1;
}

char LBT22043_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯
	switch(CurrRange)
	{
	case CURRENT_RANGE_1:
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_H]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_M]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_L]);
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A0]);
		return 1;
	case CURRENT_RANGE_2:
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_M]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_H]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_L]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A1]);
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_A0]);
		return 1;
	case CURRENT_RANGE_3:
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_L]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_M]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_H]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A0]);
		return 1;
	default:
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_H]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_M]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_L]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A0]);
		return 0;
	}
	//	return 1;
}
void LBT22043_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT22043_Relay MyRly;

	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_Protect]);
	if(MyRly.m_Bits.OffGate)
	{
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_H]);
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_L]);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_Protect]);
	if(MyRly.m_Bits.OffGate)
	{
		switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
		{
		case CURRENT_RANGE_1:
			ControlPin::SetMeLow(PinGroup[LBT22043_Pin_OFFG_H]);
			ControlPin::SetMeLow(PinGroup[LBT22043_Pin_OFFG_L]);
			break;
		case CURRENT_RANGE_2:
		case CURRENT_RANGE_3:
			ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_H]);
			ControlPin::SetMeLow(PinGroup[LBT22043_Pin_OFFG_L]);
			break;
		default:
			ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_H]);
			ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_L]);
			break;
		}
	}
	if(MyRly.m_Bits.Output)
	{
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_L]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_M]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_H]);

		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯

		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT22043_Pin_A0]);

		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_H]);
		ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_OFFG_L]);
	}
}

void LBT22043_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[LBT22043_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0);
	ControlPin::SetMeLow(PinGroup[LBT22043_Pin_WD]);
}

void LBT22043_Base::ADC_ADG_open(unsigned char ChannelIdx)
{
	return;
}
char  LBT22043_Base::RedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{

	float m_Volt,m_VH,m_VL;

	if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus!=STEPRUNNING)
		return 0;

	m_Volt=ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);
	m_VH=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh;
	m_VL=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow;

	if((m_VH<m_Volt-0.05f)||(m_VL>m_Volt+0.05f))
		return 1;
	else
		return 0;


}

char LBT22043_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
	return 0;
}

char LBT22043_Base::FanAdjOperate(unsigned long m_Level,unsigned long FanAdjMod)
{
	if(FanAdjMod!=I2CMOD)
	{
		board_config::bFanAdj=0;
		I2CBusNew::AD5245_SET(0,0,1);
		return 0;
	}
	switch (m_Level)
	{
	case 0:
		FanAdj_task::FanSetValue=180;
		break;
	case 1:
		FanAdj_task::FanSetValue=170;
		break;
	case 2:
		FanAdj_task::FanSetValue=160;
		break;
	case 3:
		FanAdj_task::FanSetValue=128;
		break;
	case 4:
		FanAdj_task::FanSetValue=96;
		break;
	case 5:
		FanAdj_task::FanSetValue=64;
		break;
	case 6:
		FanAdj_task::FanSetValue=32;
		break;
	case 7:
		FanAdj_task::FanSetValue=0;
		break;
	default:
		FanAdj_task::FanSetValue=0;
		break;
	}
	I2CBusNew::AD5245_SET(0,FanAdj_task::FanSetValue,1);
	return 1;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
