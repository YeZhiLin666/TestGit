#include "../../Entry/includes.h"

#define  LBT21164_FINE_TUNE_STARTTIME          0.1f
#define  LBT21164_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT21164_FINE_TUNE_STEPFACTOR         6E-6
#define  LBT21164_FINE_TUNE_MAXADJUSTFACTOR    0.03f  //  20140617 微调

#define  LBT21164_SETTING_TIME                 1000       //电流上升沿时间，单位为100us

#pragma arm section rwdata = "SRAM",zidata = "SRAM" 
St_DisposableTask LBT21164_Base::SetCtrlOpenCh[6];
St_DisposableTask LBT21164_Base::SetCtrlCloseCh[3];
St_DisposableTask LBT21164_Base::SetCCToCC[5];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT21164_Base::SetRstToOff[2];

SSPI_Para  LBT21164_Base::LBT21164_SSP0DAC;
SSPI_Para  LBT21164_Base::LBT21164_SSP0VCDAC;    //VCLAMP
SSPI_Para  LBT21164_Base::LBT21164_SSP1ADC;
#pragma arm section 

unsigned char LBT21164_Base::SetCtrlOpenCh_Len = 6;
unsigned char LBT21164_Base::SetCtrlCloseCh_Len = 3;
unsigned char LBT21164_Base::SetCCToCC_Len = 5;
unsigned char LBT21164_Base::SetRstToOff_Len = 2;
St_LBT21164_FanCtrl LBT21164_Base::m_FanCtrl; 
//                                                 Port      Bits       PortType           Function               Address            
const St_Port LBT21164_Base::Pin_AdcConver      = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_AdcBusy1       = {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21164_Base::Pin_AdcBusy2       = {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21164_Base::Pin_AdcCS1         = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_AdcCS2         = {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_AdcRst         = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

const St_Port LBT21164_Base::Pin_DacCS          = {1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};
const St_Port LBT21164_Base::Pin_DacRst         = {2,        8,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_DacLoad        = {1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_DacBusy        = {1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0};

const St_Port LBT21164_Base::Pin_Clamp_Dac_CS   = {1,        4,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};
const St_Port LBT21164_Base::Pin_Clamp_Dac_Load = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                1};
const St_Port LBT21164_Base::Pin_Clamp_Dac_Clr  = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                1};

const St_Port LBT21164_Base::Pin_WD             = {0,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//clamp
const St_Port LBT21164_Base::Pin_SCK0           = {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
const St_Port LBT21164_Base::Pin_MISO0          = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
const St_Port LBT21164_Base::Pin_MOSI0          = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};
const St_Port LBT21164_Base::Pin_SCK1           = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port LBT21164_Base::Pin_MISO1          = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port LBT21164_Base::Pin_MOSI1          = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

const St_Port LBT21164_Base::Pin_PowerOn        = {2,         3,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21164_Base::Pin_PowerOff       = {4,        29,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21164_Base::Pin_RedundantSafety= {1,        6,         eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21164_Base::Pin_FanCtrl        = {3,        18,        eCPTYPE_MCUGPIO,   PWM0,                  0};

volatile St_LBT21164_Relay LBT21164_Base::LBT21164_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
unsigned short LBT21164_Base::m_ChannelOn;

const unsigned char LBT21164_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT21164_Base::m_bHaveClampVdac = 1;
const unsigned char LBT21164_Base::m_bChannelBasedClampVdac = 0;
void LBT21164_Base::Hardware_Init(void)
{  
	IO_Init();    //内部IO初始化
	Feed_CpldWD();  // 喂狗信号
	board_config::bHaveRedundantCheck = 1;    //1;  modify by qjm20140922
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;

	CPLD_FUNC::Init();
	memset((void *)(&LBT21164_relaystatus), 0, sizeof(LBT21164_relaystatus));
	Feed_CpldWD();  // 喂狗信号

	//此处powerOn
	ControlPin::SetMeHigh(Pin_PowerOn,1);

	//SSP0 for DAC    AD5360
	LBT21164_SSP0DAC.uc_Module_Name = SSP0_DEVICE1;    //name
	LBT21164_SSP0DAC.uc_Speed = 20;//7;
	LBT21164_SSP0DAC.uc_CPOL = 0;
	LBT21164_SSP0DAC.uc_CPHA = 1;
	LBT21164_SSP0DAC.uc_MSTR = 1;    //master
	LBT21164_SSP0DAC.uc_BITS = 8;
	LBT21164_SSP0DAC.uc_LSBF = 0;
	//SSP0 for DAC    AD5754
	LBT21164_SSP0VCDAC.uc_Module_Name = SSP0_DEVICE3;    //name
	LBT21164_SSP0VCDAC.uc_Speed = 20;//5;
	LBT21164_SSP0VCDAC.uc_CPOL = 0;
	LBT21164_SSP0VCDAC.uc_CPHA = 1;
	LBT21164_SSP0VCDAC.uc_MSTR = 1;    //master
	LBT21164_SSP0VCDAC.uc_BITS = 8;
	LBT21164_SSP0VCDAC.uc_LSBF = 0;

	//SSP1 for ADC
	LBT21164_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	LBT21164_SSP1ADC.uc_Speed = 5;
	LBT21164_SSP1ADC.uc_CPOL = 0;
	LBT21164_SSP1ADC.uc_CPHA = 0;       //rising trig
	LBT21164_SSP1ADC.uc_MSTR = 1;    //master
	LBT21164_SSP1ADC.uc_BITS = 8;
	LBT21164_SSP1ADC.uc_LSBF = 0;

	TIMERSFunc::DELAY_SYS( 30000 );	//delay 5ms
	Feed_CpldWD();  // 喂狗信号

	ADC_Init();
	DAC_Init();

	for(int i = 0; i < MAXCHANNELNO;i++)
		CPLD_FUNC::WriteDOBus(i,LBT21164_relaystatus[i].Reg);  //硬件改变控制时序，改成高有效 20140331

	LBT21164_Base::changeADCRange(CURRENT_RANGE_1);
	Feed_CpldWD();  // ?11·D?o?
	CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0xFFFF);//电压控制开   modify by qjm 20140922
	Engine();

	//拉高钳压     // 20140613  电压保护
	LBT21164_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	LBT21164_Base::DAC_out(1,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);

	memset(&m_FanCtrl,0,sizeof(m_FanCtrl));
	m_FanCtrl.LastSetTime_Second = OS_ServeProcess::OS_Time.Second;
	LPC4078PWM::SetPWM2(3, 1000, 1.0f);    //Base DutyCycle == 0%
}
void LBT21164_Base::IO_Init(void)
{
	ControlPin::SetIObyCP(Pin_AdcConver);
	ControlPin::SetIObyCP(Pin_AdcBusy1);
	ControlPin::SetIObyCP(Pin_AdcBusy2);
	ControlPin::SetIObyCP(Pin_AdcCS1);
	ControlPin::SetIObyCP(Pin_AdcCS2);
	ControlPin::SetIObyCP(Pin_AdcRst);

	ControlPin::SetIObyCP(Pin_DacCS);
	ControlPin::SetIObyCP(Pin_DacRst);
	ControlPin::SetIObyCP(Pin_DacLoad);
	ControlPin::SetIObyCP(Pin_DacBusy);

	//dac
	ControlPin::SetIObyCP(Pin_Clamp_Dac_CS);
	ControlPin::SetIObyCP(Pin_Clamp_Dac_Load);
	ControlPin::SetIObyCP(Pin_Clamp_Dac_Clr);
	ControlPin::SetIObyCP(Pin_WD);

	ControlPin::SetIObyCP(Pin_MISO0);
	ControlPin::SetIObyCP(Pin_MOSI0);
	ControlPin::SetIObyCP(Pin_SCK0);
	ControlPin::SetIObyCP(Pin_MISO1);
	ControlPin::SetIObyCP(Pin_MOSI1);
	ControlPin::SetIObyCP(Pin_SCK1);

	ControlPin::SetIObyCP(Pin_PowerOn);
	ControlPin::SetIObyCP(Pin_PowerOff);
	ControlPin::SetIObyCP(Pin_FanCtrl);

	TIMERSFunc::DELAY_SYS( 300000 );	//delay 5ms
	st_I2CModule  tempModule;
	tempModule.I2Cx = I2C1;
	tempModule.CtrlByteNum = 1;//POINTER BYTE
	tempModule.BufLen = 2;
	tempModule.Buf[0] = 0;
	tempModule.Buf[1] = 0;
	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x20 << 1;
	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = 6;    
	tempModule.OPMode = I2C_MASTWRITE;
	I2CBusNew::I2C_ImmidiateTrans(&tempModule);

	m_ChannelOn = 0xFFFF;
	tempModule.I2Cx = I2C1;
	tempModule.CtrlByteNum = 1;//POINTER BYTE
	tempModule.BufLen = 2;
	tempModule.Buf[0] = m_ChannelOn & 0xFF;
	tempModule.Buf[1] = (m_ChannelOn>>8) & 0xFF;
	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x20 << 1;
	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = 2;    
	tempModule.OPMode = I2C_MASTWRITE;
	I2CBusNew::I2C_ImmidiateTrans(&tempModule);
} 

void LBT21164_Base::ADC_Init(void)
{	
	LBT21164_ADC5DAC::Adc_Init();

	TIMERSFunc::DELAY_SYS( 10000 );	//delay 5ms
}
void LBT21164_Base::DAC_Init(void)
{
	LBT21164_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 4;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT21164_SETTING_TIME;
	TIMERSFunc::DELAY_SYS( 10000 );	//delay 5ms	
}

void LBT21164_Base::Software_Init(void)
{
	Func_Init();//函数指针初始化，四个基本操作
	EngineStuff();   //引擎结构指针初始化

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT21164_FINE_TUNE_STARTTIME;
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT21164_FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT21164_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT21164_FINE_TUNE_STEPFACTOR;

	LBT21164_SampleManage::Init();
}

void LBT21164_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&LBT21164_Base::DAC_out;
	board_config::Func_SetRange=&LBT21164_Base::SetRange;
	board_config::Func_SetRelay=&LBT21164_Base::SetRelay;
	board_config::Func_Sample = &LBT21164_SampleManage::SampleISR;
	board_config::Func_RedundantSafety= &LBT21164_Base::RedundantSafety; //冗余电压保护
	board_config::Func_ReleaseRedundantSafety=&LBT21164_Base::ReleaseRedundantSafety;//冗余解除操作

	board_config::FeedWatchDog = &Feed_CpldWD;
}

//commnet out chen 20130216
void LBT21164_Base::Engine(void)
{
	OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToCur.stepLen = LBT21164_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = LBT21164_Base::SetCtrlOpenCh_Len;

	//没有动作
	OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &LBT21164_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlRstToOff.stepLen = LBT21164_Base::SetRstToOff_Len;
	OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &LBT21164_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = LBT21164_Base::SetRstToOff_Len;

	OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlOpenCh[0];    //指向数组0
	OS_LogicProcess::SetCtrlOffToCur.stepLen = LBT21164_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = LBT21164_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &LBT21164_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = LBT21164_Base::SetCCToCC_Len;


	OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &LBT21164_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = LBT21164_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToOff.stepLen = LBT21164_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToRst.stepLen = LBT21164_Base::SetCtrlCloseCh_Len ;


	OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &LBT21164_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = LBT21164_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &LBT21164_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = LBT21164_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = LBT21164_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &LBT21164_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = LBT21164_Base::SetCtrlCloseCh_Len ;   
}

void LBT21164_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
{

}
void LBT21164_Base::regEngine(unsigned char CH_Number,unsigned long type)
{

}
void LBT21164_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));

	SetCtrlOpenCh[0].uw32_DelayTime = 200;
	SetCtrlOpenCh[0].DacMove.DacFlag |= ACTIVE_DAC_CUR_ZERO;
	SetCtrlOpenCh[1].uw32_DelayTime = 100;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag |= LBT21164_RLY_Protect;
	SetCtrlOpenCh[2].uw32_DelayTime = 150;
	SetCtrlOpenCh[2].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
	SetCtrlOpenCh[3].uw32_DelayTime = 50;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag |= LBT21164_RLY_Protect;
	SetCtrlOpenCh[4].uw32_DelayTime = 150;
	SetCtrlOpenCh[4].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
	SetCtrlOpenCh[5].uw32_DelayTime = 100;
	SetCtrlOpenCh[5].DacMove.DacFlag |= ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag |= ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetCtrlCloseCh[0].RelayMove.ActiveFlag |= LBT21164_RLY_Protect;
	SetCtrlCloseCh[1].uw32_DelayTime = 150;
	SetCtrlCloseCh[1].RelayMove.InactiveFlag |= LBT21164_RLY_Output;
	SetCtrlCloseCh[2].uw32_DelayTime = 50;
	SetCtrlCloseCh[2].RelayMove.InactiveFlag |= LBT21164_RLY_Protect;

	SetCCToCC[0].uw32_DelayTime = 100;
	SetCCToCC[0].DacMove.DacFlag |= ACTIVE_DAC_CUR_ZERO;
	SetCCToCC[0].RelayMove.ActiveFlag = LBT21164_RLY_Protect;
	SetCCToCC[0].RangeChangedFlag = 1;
	SetCCToCC[1].uw32_DelayTime = 150;
	SetCCToCC[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[1].RangeChangedFlag = 1;
	SetCCToCC[2].uw32_DelayTime = 50;
	SetCCToCC[2].RelayMove.InactiveFlag = LBT21164_RLY_Protect;
	SetCCToCC[2].RangeChangedFlag = 1;
	SetCCToCC[3].uw32_DelayTime = 150;
	SetCCToCC[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[3].RangeChangedFlag = 1;
	SetCCToCC[4].uw32_DelayTime = 50;
	SetCCToCC[4].DacMove.DacFlag |= ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 100;
	SetRstToOff[0].RelayMove.ActiveFlag = 0;
	SetRstToOff[0].DacMove.DacFlag |= ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
	SetRstToOff[0].RangeChangedFlag = 0;
	SetRstToOff[1].uw32_DelayTime = 0;
	SetRstToOff[1].RelayMove.ActiveFlag = 0;
	SetRstToOff[1].DacMove.DacFlag |= ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetRstToOff[1].RangeChangedFlag = 0;
}

char LBT21164_Base::ADC_rd(unsigned long CH_Number,unsigned long type)
{
	return 0;
}
char LBT21164_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{ 
	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		if(SSPIFunc::getModuleName(0)  != LBT21164_SSP0_DAC_NAME)
		{
			SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP0DAC,0);
			SSPIFunc::my_Init(0);              //初始化
			ARBUS_DELAY(10);
		}
		DAC16bit_AD5360::Dac_out(ChannelIdx,Data);
		SetFanDuty_DAC(ChannelIdx,Data);
		break;
	case ACTIVE_DAC_VLT:
		LBT21164_ADC5DAC::Dac_out_Vlt(ChannelIdx,Data);
		break;
	default:
		break;
	}
}

char LBT21164_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	SetFanDuty_RNG(ChannelIdx,CurrRange);
	m_ChannelOn &= ~(0x01<<ChannelIdx);
	SetChannelON();
	switch(CurrRange)
	{
	case CURRENT_RANGE_1:
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 1;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
		break;
	case CURRENT_RANGE_2:
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 1; 
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
		break;
	case CURRENT_RANGE_3:
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 1;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 0; 
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
		break;	
	case CURRENT_RANGE_4:
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.L  = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
		break;
	default:
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.L  = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
		break;
	}
	return 1;
}
void LBT21164_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT21164_Relay2 MyRly;

	if(bAutoCali)
	{
		LBT21164_relaystatus[ChannelIdx].Bits.Cali = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
	}
	else
	{
		LBT21164_relaystatus[ChannelIdx].Bits.Cali = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
	}

	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21164_relaystatus[ChannelIdx].Bits.L = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21164_relaystatus[ChannelIdx].Bits.L = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Output)
	{
		LBT21164_relaystatus[ChannelIdx].Bits.A0 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.A1 = 0;
		LBT21164_relaystatus[ChannelIdx].Bits.L  = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21164_relaystatus[ChannelIdx].Reg);

		m_ChannelOn |= (0x01<<ChannelIdx);
		SetChannelON();
		m_FanCtrl.Last_Duty[ChannelIdx].Reg           = 0;
		SetFanDuty_PWM();    //计算,设置
	}
}
void LBT21164_Base::Adjust_ADC_Gain(char type,char chnType)
{
	if(chnType == ADC_TYPE_I)
	{
		if(type == 1)    //单倍
		{
#if LBT21164_USE_5VDACADC == 0
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);  //modify by qjm20140925
#else
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x21A);  //5V ADC
#endif
			ARBUS_DELAY(5);
		}
	}
	else if(chnType == ADC_TYPE_V)
	{
		if(type == 1)    //单倍
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
}

void LBT21164_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void LBT21164_Base::ADG_Reset(void)
{

}

void LBT21164_Base::changeADCRange(unsigned char range)
{
	ARBUS_DELAY(10);
	switch(range)
	{
	case CURRENT_RANGE_1:
		LBT21164_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
		break;
	case CURRENT_RANGE_2:
		LBT21164_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
		break;
	case CURRENT_RANGE_3:
		LBT21164_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
		break;
	default:
		LBT21164_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
		break;			
	}
}

void LBT21164_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(Pin_WD,1);
	TIMERSFunc::DELAY_SYS(200);
	ControlPin::SetMeLow(Pin_WD,0);

	if(m_FanCtrl.New_Duty.Reg < m_FanCtrl.Cur_Duty.Reg)
	{
		if((OS_ServeProcess::OS_Time.Second - m_FanCtrl.LastSetTime_Second) > 9) 
		{
			if(m_FanCtrl.New_Duty.Bits.Rng90)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.9f);
			else if(m_FanCtrl.New_Duty.Bits.Rng70)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.7f);
			else if(m_FanCtrl.New_Duty.Bits.Rng60)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.6f);
			else if(m_FanCtrl.New_Duty.Bits.Rng50)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.5f);
			else if(m_FanCtrl.New_Duty.Bits.Rng40)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.4f);
			else if(m_FanCtrl.New_Duty.Bits.Rng20)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.2f);
			else if(m_FanCtrl.New_Duty.Bits.LowRange)
				LPC4078PWM::SetPWM2(3, 1000, 1 - 0.1f);
			else
				LPC4078PWM::SetPWM2(3,1000,1 - 0.0f);

			m_FanCtrl.Cur_Duty = m_FanCtrl.New_Duty;
			m_FanCtrl.LastSetTime_Second = OS_ServeProcess::OS_Time.Second;
		}
	}
}

char  LBT21164_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
	char temp;
	temp=ControlPin::ReadMe(Pin_RedundantSafety);
	if(temp)
		return 0;
	else
		return 1;


}

char LBT21164_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
	//Hardware can't set the voltage relay.
}

void LBT21164_Base::SetFanDuty_RNG(unsigned long uc_ChannelNo,unsigned char CurrRange)
{
	m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.LowRange = (CurrRange > CURRENT_RANGE_1);
	if(m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.LowRange)
	{
		m_FanCtrl.Last_Duty[uc_ChannelNo].Reg           = 0;
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.LowRange = 1;
	}

	SetFanDuty_PWM();    //计算,设置
}
void LBT21164_Base::SetFanDuty_DAC(unsigned long uc_ChannelNo,unsigned long DacCode)
{
	if(m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.LowRange)
		return;

	unsigned long DutyCycle = 0;
	if(DacCode > 0x800000)
		DacCode -= 0x800000;
	else
		DacCode = 0x800000 - DacCode;

	DutyCycle = DacCode * 100 / 0x800000;

	m_FanCtrl.Last_Duty[uc_ChannelNo].Reg = 0;
	if(DutyCycle >= 90)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng100  = 1;
	else if(DutyCycle >= 70)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng90   = 1;
	else if(DutyCycle >= 60)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng70   = 1;
	else if(DutyCycle >= 50)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng60   = 1;
	else if(DutyCycle >= 40)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng50   = 1;
	else if(DutyCycle >= 20)
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng40   = 1;
	else
		m_FanCtrl.Last_Duty[uc_ChannelNo].Bits.Rng20   = 1;

	SetFanDuty_PWM();    //计算,设置
}
void LBT21164_Base::SetFanDuty_PWM(void)
{
#pragma region 综合16个通道的最大风力
	Un_FanRng MyRng;
	MyRng.Reg = 0;
	for(int ChannelIdx = 0;ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
		MyRng.Reg |= m_FanCtrl.Last_Duty[ChannelIdx].Reg;
	if(MyRng.Bits.Rng100)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng100  = 1;
	}
	else if(MyRng.Bits.Rng90)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng90  = 1;
	}
	else if(MyRng.Bits.Rng70)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng70  = 1;
	}
	else if(MyRng.Bits.Rng60)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng60  = 1;
	}
	else if(MyRng.Bits.Rng50)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng50  = 1;
	}
	else if(MyRng.Bits.Rng40)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng40  = 1;
	}
	else if(MyRng.Bits.Rng20)
	{
		MyRng.Reg = 0;
		MyRng.Bits.Rng20  = 1;
	}
	else if(MyRng.Bits.LowRange)
	{
		MyRng.Reg = 0;
		MyRng.Bits.LowRange = 1;
	}
	else
		MyRng.Reg = 0;
#pragma  endregion
	if(MyRng.Reg == m_FanCtrl.New_Duty.Reg)
		return;

	if(MyRng.Reg > m_FanCtrl.Cur_Duty.Reg)
	{
		if(MyRng.Bits.Rng100)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 1.0f);
		else if(MyRng.Bits.Rng90)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.9f);
		else if(MyRng.Bits.Rng70)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.7f);
		else if(MyRng.Bits.Rng60)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.6f);
		else if(MyRng.Bits.Rng50)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.5f);
		else if(MyRng.Bits.Rng40)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.4f);
		else if(MyRng.Bits.Rng20)
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.2f);
		else 
			LPC4078PWM::SetPWM2(3, 1000, 1 - 0.1f);

		m_FanCtrl.New_Duty = m_FanCtrl.Cur_Duty = MyRng;
		m_FanCtrl.LastSetTime_Second = OS_ServeProcess::OS_Time.Second;
	}
	else
	{
		m_FanCtrl.New_Duty = MyRng;
		m_FanCtrl.LastSetTime_Second = OS_ServeProcess::OS_Time.Second;
	}
}

void LBT21164_Base::SetChannelON(void)
{
#if LBT21164_I2C_USE_TASK == 0
	st_I2CModule  tempModule;
	tempModule.I2Cx = I2C1;
	tempModule.CtrlByteNum = 1;//POINTER BYTE
	tempModule.BufLen = 2;
	tempModule.Buf[0] = m_ChannelOn & 0xFF;
	tempModule.Buf[1] = (m_ChannelOn>>8) & 0xFF;
	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x20 << 1;
	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = 2;    
	tempModule.OPMode = I2C_MASTWRITE;
	I2CBusNew::I2C_ImmidiateTrans(&tempModule);
#else  
	st_I2CModule  I2Cmodule_a;
	I2Cmodule_a.I2Cx = I2C1;
	I2Cmodule_a.CtrlByteNum = 1;
	I2Cmodule_a.BufLen = 2;
	I2Cmodule_a.Buf[0] = m_ChannelOn & 0xFF;
	I2Cmodule_a.Buf[1] = (m_ChannelOn>>8) & 0xFF;
	I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =0x20<<1;  //0xa0 /x
	I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = 2;
	I2Cmodule_a.PageSize = 256;
	I2Cmodule_a.OPMode = I2C_MASTWRITE;
	I2CBusNew::I2C_Task_Request(I2C_MASTWRITE,&I2Cmodule_a);
#endif
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
