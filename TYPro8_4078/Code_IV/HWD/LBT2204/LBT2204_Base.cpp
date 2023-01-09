#include "includes.h"

#define  LBT2204_FINE_TUNE_STARTTIME          0.1f
#define  LBT2204_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT2204_FINE_TUNE_STEPFACTOR         6E-6
#define  LBT2204_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  LBT2204_ACCURACY                     0.0005f   //精度
#define  LBT2204_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT2204_SETTING_TIME                 500       //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask LBT2204_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT2204_Base::SetCtrlCloseCh[5];
St_DisposableTask LBT2204_Base::SetCCToCC[6];
St_DisposableTask LBT2204_Base::SetRstToOff[1];

SSPI_Para  LBT2204_Base::LBT2204_SSP0DAC;
#pragma arm section 

unsigned char LBT2204_Base::SetCtrlOpenCh_Len = 5;    //this is maybe problem 20131107 chen
unsigned char LBT2204_Base::SetCtrlCloseCh_Len = 5;
unsigned char LBT2204_Base::SetCCToCC_Len = 6;
unsigned char LBT2204_Base::SetRstToOff_Len = 1;

const St_Port LBT2204_Base::PinGroup[LBT2204_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType           Function               Address               
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
	{3,        17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
};

const unsigned char LBT2204_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT2204_Base::m_bHaveClampVdac = 1;
const unsigned char LBT2204_Base::m_bChannelBasedClampVdac = 0;
void LBT2204_Base::Hardware_Init(void)
{  
	IO_Init();    

	board_config::bHaveRedundantCheck = 0;
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;

	//SSP0 for DAC    AD5754
	LBT2204_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
	LBT2204_SSP0DAC.uc_Speed = 5;
	LBT2204_SSP0DAC.uc_CPOL = 0;
	LBT2204_SSP0DAC.uc_CPHA = 1;
	LBT2204_SSP0DAC.uc_MSTR = 1;    //master
	LBT2204_SSP0DAC.uc_BITS = 8;
	LBT2204_SSP0DAC.uc_LSBF = 0;

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	//RANGE INIT      
	ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_H]);
	ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_M]);
	ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_L]);
	ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_Protect]);

	//DAC INIT
	LBT2204_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
	Feed_CpldWD();
	//	LBT2204_TempSample::AD_Init(4);
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	//	AdditionalEngine();

	//拉高钳压    // 20140613  电压保护
	LBT2204_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	LBT2204_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
}
void LBT2204_Base::IO_Init(void)
{
	for(int i=0;i<LBT2204_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
	for(unsigned char pin = LBT2204_Pin_H;pin<=LBT2204_Pin_CL_Cal;pin++)
	{
		GPIOFunc::InitPortFunc(PinGroup[pin].Port,
			PinGroup[pin].Bits,
			PinGroup[pin].Function,
			1);
	}
	//	GPIOFunc::InitPortFunc(PinGroup[LBT2204_Pin_H].Port,
	//		PinGroup[LBT2204_Pin_H].Bits,
	//		PinGroup[LBT2204_Pin_H].Function,
	//		1);
	//	GPIOFunc::InitPortFunc(PinGroup[LBT2204_Pin_M].Port,
	//		PinGroup[LBT2204_Pin_M].Bits,
	//		PinGroup[LBT2204_Pin_M].Function,
	//		1);
	//	GPIOFunc::InitPortFunc(PinGroup[LBT2204_Pin_L].Port,
	//		PinGroup[LBT2204_Pin_L].Bits,
	//		PinGroup[LBT2204_Pin_L].Function,
	//		1);
	//	GPIOFunc::InitPortFunc(PinGroup[LBT2204_Pin_Protect].Port,
	//		PinGroup[LBT2204_Pin_Protect].Bits,
	//		PinGroup[LBT2204_Pin_Protect].Function,
	//		1);
	//	GPIOFunc::InitPortFunc(PinGroup[LBT2204_Pin_V].Port,
	//		PinGroup[LBT2204_Pin_V].Bits,
	//		PinGroup[LBT2204_Pin_V].Function,
	//		1);
} 
void LBT2204_Base::ADC_Init(void)
{	
	LBT2204_ADC5DAC::Adc_Init();
}
void LBT2204_Base::DAC_Init(void)
{
	LBT2204_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 3;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT2204_SETTING_TIME;

	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT2204_Base::Software_Init(void)
{
	Func_Init();
	EngineStuff(); 

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT2204_FINE_TUNE_STARTTIME;   //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT2204_FINE_TUNE_MINERRORFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT2204_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT2204_FINE_TUNE_STEPFACTOR;

	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT2204_ACCURACY * LBT2204_CTRL_VALUE_ERROR_FACTOR;

	board_config::Accuracy = LBT2204_ACCURACY;

	LBT2204_SampleManage::Init();
}

void LBT2204_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&LBT2204_Base::DAC_out;
	board_config::Func_SetRange=&LBT2204_Base::SetRange;
	board_config::Func_SetRelay=&LBT2204_Base::SetRelay;
	board_config::Func_Sample =&LBT2204_SampleManage::SampleISR;    //timer0 中断
	//	board_config::Func_TempSample = &LBT2204_TempSample::Temp_rd;   // 20140613  温度测量

	board_config::FeedWatchDog = &LBT2204_Base::Feed_CpldWD;
}

//void LBT2204_Base::AdditionalEngine(void)
//{
//}

// void LBT2204_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {

// }
// void LBT2204_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {

// }
void LBT2204_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));

	//---------------------------------------------//
	SetCtrlOpenCh[0].uw32_DelayTime = 200;    
	SetCtrlOpenCh[0].RelayMove.ActiveFlag = (LBT2204_RLY_Protect | LBT2204_RLY_OffGate);
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlOpenCh[1].uw32_DelayTime = 200;  	
	SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetCtrlOpenCh[2].uw32_DelayTime = 200;
	SetCtrlOpenCh[2].RelayMove.InactiveFlag = (LBT2204_RLY_Protect);

	SetCtrlOpenCh[3].uw32_DelayTime = 200;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT2204_RLY_OffGate;  

	//SetCtrlOpenCh[4].uw32_DelayTime = 0; 
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[1].uw32_DelayTime = 200;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT2204_RLY_OffGate;

	SetCtrlCloseCh[2].uw32_DelayTime = 200;
	SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT2204_RLY_Protect;    

	SetCtrlCloseCh[3].uw32_DelayTime = 200;
	SetCtrlCloseCh[3].RelayMove.InactiveFlag = LBT2204_RLY_Output;

	//SetCtrlCloseCh[4].uw32_DelayTime = 0;
	SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT2204_RLY_Protect;

	//---------------------------------------------//

	SetCCToCC[0].uw32_DelayTime = 10;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 100;
	SetCCToCC[1].RelayMove.ActiveFlag = (LBT2204_RLY_Protect|LBT2204_RLY_OffGate);
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 100;
	SetCCToCC[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;   
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 100;
	SetCCToCC[3].RelayMove.InactiveFlag = LBT2204_RLY_Protect;     
	SetCCToCC[3].RangeChangedFlag = 1;

	SetCCToCC[4].uw32_DelayTime = 100;
	SetCCToCC[4].RelayMove.InactiveFlag = LBT2204_RLY_OffGate;    
	SetCCToCC[4].RangeChangedFlag = 1;

	SetCCToCC[5].uw32_DelayTime = 100;    
	SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 10;
	//SetRstToOff[0].DacMove.DacFlag = 0;
	//SetRstToOff[0].RangeChangedFlag = 0;
}
char LBT2204_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
	unsigned long value = Data;    //强制保护

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		LBT2204_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
		break;
		///////////////////钳位
	case ACTIVE_DAC_VH:
	case ACTIVE_DAC_VL:

// 		unsigned long temp1;
// 		if(value > 0xFFFFFF)
// 		{
// 			value = 0xFFFFFF;
// 		}
// 		if( value > 0x800000 )    
// 		{
// 			temp1 = value - 0x800000;
// 			value = 0x800000 - temp1;
// 		}
// 		else                                          
// 		{
// 			temp1 = 0x800000 - value;
// 			value = 0x800000 + temp1;
// 		}
// 		if(value > 0xFFFFFF)
// 		{
// 			value = 0xFFFFFF;
// 		}
    value = 0xFFFFFF-value;
		if(type == ACTIVE_DAC_VH)
			LBT2204_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTA - DACH  // 20140613  电压保护
		else
			LBT2204_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTC - DACL
		break;
	default:
		break;
	}
	return 1;
}

char LBT2204_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
	case CURRENT_RANGE_1:
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_H]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_M]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_L]);

		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A0]);
		return 1;
	case CURRENT_RANGE_2:
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_M]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_H]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_L]);

		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A1]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_A0]);
		return 1;
	case CURRENT_RANGE_3:
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_L]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_M]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_H]);

		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A0]);
		return 1;
	default:
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_H]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_M]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_L]);

		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A0]);
		return 0;
	}
	//	return 1;
}
void LBT2204_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT2204_Relay MyRly;

	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_Protect]);
	if(MyRly.m_Bits.OffGate)
	{
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_H]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_L]);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_Protect]);
	if(MyRly.m_Bits.OffGate)
	{
		switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
		{
		case CURRENT_RANGE_1:
			ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_OFFG_H]);
			ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_OFFG_L]);
			break;
		case CURRENT_RANGE_2:
		case CURRENT_RANGE_3:
			ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_H]);
			ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_OFFG_L]);
			break;
		default:
			ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_H]);
			ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_L]);
			break;
		}
	}
	if(MyRly.m_Bits.Output)
	{
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_L]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_M]);
		ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_H]);

		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A1]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_A0]);

		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_H]);
		ControlPin::SetMeLow(PinGroup[LBT2204_Pin_OFFG_L]);
	}
}

void LBT2204_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[LBT2204_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0);
	ControlPin::SetMeLow(PinGroup[LBT2204_Pin_WD]);
}

void LBT2204_Base::ADC_ADG_open(unsigned char ChannelIdx)
{
	return;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
