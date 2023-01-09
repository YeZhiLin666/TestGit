#include "includes.h"

#define  BT11_FINE_TUNE_STARTTIME          0.1f
#define  BT11_FINE_TUNE_MINERRORFACTOR     2E-5
#define  BT11_FINE_TUNE_STEPFACTOR         6E-6
#define  BT11_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  BT11_ACCURACY                     0.0005f   //精度
#define  BT11_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  BT11_SETTING_TIME                 500       //电流上升沿时间，单位为100us

#pragma arm section rwdata = "SRAM",zidata = "SRAM" 
St_DisposableTask BT11_Base::SetRstToCur[5];
St_DisposableTask BT11_Base::SetCurToOff[5];
St_DisposableTask BT11_Base::SetCCToCC[6];
St_DisposableTask BT11_Base::SetRstToOff[1];

SSPI_Para  BT11_Base::BT11_SSP0DAC;
#pragma arm section 

unsigned char BT11_Base::SetRstToCur_Len = 5;  //6;    //this is maybe problem 20131107 chen
unsigned char BT11_Base::SetCurToOff_Len = 5;
unsigned char BT11_Base::SetCCToCC_Len = 6;
unsigned char BT11_Base::SetRstToOff_Len = 1;



const St_Port BT11_Base::PinGroup[BT11_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType          Function               Address  

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
	{3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.21
	{0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.22
	{2,		    2,		     eCPTYPE_MCUGPIO,	  PWM1,				           0},
	{2,		    3,		     eCPTYPE_MCUGPIO,	  PWM1,				           0},
	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
};

const unsigned char BT11_Base::m_bHaveVoltageDac = 0;
const unsigned char BT11_Base::m_bHaveClampVdac = 1;
const unsigned char BT11_Base::m_bChannelBasedClampVdac = 1;

void BT11_Base::Hardware_Init(void)
{  
	BT11_FeatureEnable_Init();
	IO_Init();    

	//SSP0 for DAC    AD5754
	BT11_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
	BT11_SSP0DAC.uc_Speed = 5;
	BT11_SSP0DAC.uc_CPOL = 0;
	BT11_SSP0DAC.uc_CPHA = 1;
	BT11_SSP0DAC.uc_MSTR = 1;    //master
	BT11_SSP0DAC.uc_BITS = 8;
	BT11_SSP0DAC.uc_LSBF = 0;

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	//RANGE INIT      
	ControlPin::SetMeHigh(PinGroup[BT11_Pin_IVSD]);

	//DAC INIT
	BT11_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
	OS_LogicProcess::InitBoardEngine(	&SetRstToCur[0],	  sizeof(SetRstToCur)		/		sizeof(St_DisposableTask),
		&SetCurToOff[0],    sizeof(SetCurToOff)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	//	AdditionalEngine();

	//拉高钳压    // 20140613  电压保护
	BT11_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	BT11_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
}
void BT11_Base::BT11_FeatureEnable_Init(void) 
{
	static St_Hardware_Feature_Enable para;
	memset(&para,0,sizeof(para));
	
	if(board_config::IVBoardTypeversion==BOARDTYPE_VER_E)
	{
	
	para.OS_Tick_Interval=200;
		
	}
	
	board_config::Feature_Enable_init=&para;
} 
void BT11_Base::IO_Init(void)
{
	for(int i=0;i<BT11_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
} 
void BT11_Base::ADC_Init(void)
{	
	BT11_ADC5DAC::Adc_Init();
}
void BT11_Base::DAC_Init(void)
{
	BT11_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 3;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = BT11_SETTING_TIME;

	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void BT11_Base::Software_Init(void)
{
	Func_Init();
	EngineStuff(); 

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = BT11_FINE_TUNE_STARTTIME;   //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = BT11_FINE_TUNE_MINERRORFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = BT11_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = BT11_FINE_TUNE_STEPFACTOR;

	safety_check::m_CtrlValueSetting.fErrorFactor       = BT11_ACCURACY * BT11_CTRL_VALUE_ERROR_FACTOR;

	board_config::Accuracy = BT11_ACCURACY;

	BT11_SampleManage::Init();
}

void BT11_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&BT11_Base::DAC_out;
	board_config::Func_SetRange=&BT11_Base::SetRange;
	board_config::Func_SetRelay=&BT11_Base::SetRelay;
	board_config::Func_Sample =&BT11_SampleManage::SampleISR;    //timer0 中断

	board_config::FeedWatchDog = &BT11_Base::Feed_CpldWD;
}

//void BT11_Base::AdditionalEngine(void)
//{
//}

// void BT11_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {

// }
// void BT11_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {

// }
void BT11_Base::EngineStuff(void)
{
	memset(&SetRstToCur,  0,    sizeof(SetRstToCur));
	memset(&SetCurToOff,  0,    sizeof(SetCurToOff));
	memset(&SetCCToCC,    0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,  0,    sizeof(SetRstToOff));


	//---------------------------------------------//
	//电流开通道
	SetRstToCur[0].uw32_DelayTime = 100;
	SetRstToCur[0].RelayMove.InactiveFlag = BT11_RLY_IVSD;  // I/V切换到I
	SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

	SetRstToCur[1].uw32_DelayTime = 100;    
	SetRstToCur[1].RelayMove.ActiveFlag = BT11_RLY_Protect;

	SetRstToCur[2].uw32_DelayTime = 100;  	
	SetRstToCur[2].DacMove.DacFlag =  ACTIVE_DAC_VH | ACTIVE_DAC_VL;
	SetRstToCur[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetRstToCur[3].uw32_DelayTime = 100;
	SetRstToCur[3].RelayMove.InactiveFlag = BT11_RLY_Protect;

	SetRstToCur[4].uw32_DelayTime = 100; 
	SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

	//电压开通道 

	//---------------------------------------------//
	//电流关通道
	SetCurToOff[0].uw32_DelayTime = 100;
	SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

	SetCurToOff[1].uw32_DelayTime = 100;
	SetCurToOff[1].RelayMove.InactiveFlag = BT11_RLY_IVSD;  //I/V继电器切换至I    

	SetCurToOff[2].uw32_DelayTime = 100;
	SetCurToOff[2].RelayMove.InactiveFlag = BT11_RLY_Output;


	SetCurToOff[3].uw32_DelayTime = 100;

	SetCurToOff[4].uw32_DelayTime = 100;
	SetCurToOff[4].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

	//电压关通道	

	//---------------------------------------------//
	//电流切换到电流
	SetCCToCC[0].uw32_DelayTime = 100;
	SetCCToCC[0].DacMove.DacFlag =  ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 100;
	SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 100; 
	SetCCToCC[2].RelayMove.ActiveFlag = BT11_RLY_Protect;
	SetCCToCC[2].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 100;
	SetCCToCC[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	//SetCCToCC[4].RelayMove.InactiveFlag = BT11_RLY_IVSD;    
	SetCCToCC[3].RangeChangedFlag = 1;

	SetCCToCC[4].uw32_DelayTime = 100;    
	SetCCToCC[4].RelayMove.InactiveFlag = BT11_RLY_Protect; 
	SetCCToCC[4].RangeChangedFlag = 1;

	SetCCToCC[5].uw32_DelayTime = 200; 
	SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	//电压切换到电压

	//电压切换到电流

	//电流切换到电压

	SetRstToOff[0].uw32_DelayTime = 10;
	SetRstToOff[0].DacMove.DacFlag = 0;
	SetRstToOff[0].RangeChangedFlag = 0;
}
char BT11_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
	unsigned long value = Data;    //强制保护

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		BT11_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
		break;
		//钳位
	case ACTIVE_DAC_VH:
		BT11_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTA - DACH  // 20140613  电压保护
		break;
	case ACTIVE_DAC_VL:
		//			unsigned long temp2;
		BT11_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTC - DACL
	default:
		break;
	}
	return 1;
}

char BT11_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
	case CURRENT_RANGE_1:
		ControlPin::SetMeHigh(PinGroup[BT11_Pin_H]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_M]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_L]);
		return 1;
	case CURRENT_RANGE_2:
		ControlPin::SetMeHigh(PinGroup[BT11_Pin_M]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_H]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_L]);
		return 1;
	case CURRENT_RANGE_3:
		ControlPin::SetMeHigh(PinGroup[BT11_Pin_L]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_M]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_H]);
		return 1;
	default:
		ControlPin::SetMeLow(PinGroup[BT11_Pin_H]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_M]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_L]);
		return 0;
	}
	//	return 1;
}
void BT11_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_BT11_Relay MyRly;

	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeHigh(PinGroup[BT11_Pin_Protect]);
	if(MyRly.m_Bits.IVSD)
		ControlPin::SetMeHigh(PinGroup[BT11_Pin_IVSD]);

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeLow(PinGroup[BT11_Pin_Protect]);
	if(MyRly.m_Bits.IVSD)
		ControlPin::SetMeLow(PinGroup[BT11_Pin_IVSD]);

	if(MyRly.m_Bits.Output)
	{
		ControlPin::SetMeLow(PinGroup[BT11_Pin_L]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_M]);
		ControlPin::SetMeLow(PinGroup[BT11_Pin_H]);
	}
}

void BT11_Base::Feed_CpldWD(void)
{

}

void BT11_Base::ADC_ADG_open(unsigned char ChannelIdx)
{
	return;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
