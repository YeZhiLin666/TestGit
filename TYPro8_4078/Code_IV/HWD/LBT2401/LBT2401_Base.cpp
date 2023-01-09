
#include "../../Entry/includes.h"

// #define  _LBT2401_VERSION_A_         0         //this is a temperory MACRO,and valish somedays later

//#if MAXWELL_MACHINE == 1
//#define  LBT2401_FINE_TUNE_STARTTIME          0.005f   //微调时间 单位 S  0.1f=0.1s  20140727
//#define  LBT2401_FINE_TUNE_MINERRORFACTOR     2E-5
//#define  LBT2401_FINE_TUNE_STEPFACTOR         3E-4
//#define  LBT2401_FINE_TUNE_MAXADJUSTFACTOR    0.06f
//#else
//#define  LBT2401_FINE_TUNE_STARTTIME          0.01f
//#define  LBT2401_FINE_TUNE_MINERRORFACTOR     2E-5
//#define  LBT2401_FINE_TUNE_STEPFACTOR         6E-6
//#define  LBT2401_FINE_TUNE_MAXADJUSTFACTOR    0.06f
//#endif

float LBT2401_Fine_Tune_Starttime;
float LBT2401_Fine_Tune_Mineerrorfactor;
float LBT2401_Fine_Tune_Stepfactor;
float LBT2401_Fine_Tune_Maxadjustfactor;

#define  LBT2401_ACCURACY                     0.0005f   //精度
#define  LBT2401_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT2401_SETTING_TIME                 500       //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask LBT2401_Base::SetCtrlOpenCh[6];
St_DisposableTask LBT2401_Base::SetCtrlCloseCh[5];
St_DisposableTask LBT2401_Base::SetCCToCC[7];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT2401_Base::SetRstToOff[2];
St_DisposableTask LBT2401_Base::SetVoltRelay[2];  //增加冗余电压恢复时序 用于中电/Maxwell/ 20140827 dirui 

SSPI_Para  LBT2401_Base::LBT2401_SSP0DAC;
SSPI_Para  LBT2401_Base::LBT2401_SSP1ADC;
#pragma arm section 

unsigned char LBT2401_Base::SetCtrlOpenCh_Len = 6;
unsigned char LBT2401_Base::SetCtrlCloseCh_Len = 5;
unsigned char LBT2401_Base::SetCCToCC_Len = 7;
unsigned char LBT2401_Base::SetRstToOff_Len = 2;
unsigned char LBT2401_Base::SetVoltRelay_Len=2;

const St_Port LBT2401_Base::PinGroup[LBT2401_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType           Function               Address    

	{1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{1,        1,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{3,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
	{0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
	{0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
	{0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
	{2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{3,			  17,				 eCPTYPE_MCUGPIO,	  PWM0,						  		 0},
	{3,        20,        eCPTYPE_MCUGPIO,   GPIO_I,                0},//用于中电，MAXWLL 
};
volatile St_LBT2401_Relay LBT2401_Base::LBT2401_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

const unsigned char LBT2401_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT2401_Base::m_bHaveClampVdac = 1;
const unsigned char LBT2401_Base::m_bChannelBasedClampVdac = 0;
void LBT2401_Base::Hardware_Init(void)
{  
    LBT2401_FeatureEnable_Init();
	IO_Init();    //内部IO初始化
	Feed_CpldWD();  //喂狗信号

	board_config::bHaveRedundantCheck = 1;    //目前只有LBT2401才会检查雍余电压
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;

	CPLD_FUNC::Init();
	memset((void *)(&LBT2401_relaystatus), 0, sizeof(LBT2401_relaystatus));
	Feed_CpldWD();  //喂狗信号

	//此处powerOn是否还需要
	ControlPin::SetMeHigh(PinGroup[LBT2401_Pin_PowerOn]);

	//SSP0 for DAC    AD5754
	LBT2401_SSP0DAC.uc_Module_Name = SSP0_DEVICE3;    //name
	LBT2401_SSP0DAC.uc_Speed = 7;//5;
	LBT2401_SSP0DAC.uc_CPOL = 0;
	LBT2401_SSP0DAC.uc_CPHA = 1;
	LBT2401_SSP0DAC.uc_MSTR = 1;    //master
	LBT2401_SSP0DAC.uc_BITS = 8;
	LBT2401_SSP0DAC.uc_LSBF = 0;

	//SSP1 for ADC
	LBT2401_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	LBT2401_SSP1ADC.uc_Speed = 5;
	LBT2401_SSP1ADC.uc_CPOL = 0;
	LBT2401_SSP1ADC.uc_CPHA = 0;       //rising trig
	LBT2401_SSP1ADC.uc_MSTR = 1;    //master
	LBT2401_SSP1ADC.uc_BITS = 8;
	LBT2401_SSP1ADC.uc_LSBF = 0;

	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	Feed_CpldWD();  //喂狗信号

	ADC_Init();
	DAC_Init();
	LBT2401_relaystatus[0].Bits.R_OFFGH=1;

	CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[0].Reg);  //硬件改变控制时序，改成高有效 20140331

	ControlPin::ReInitPullUP();
	LBT2401_Base::changeADCRange(CURRENT_RANGE_1);
	Feed_CpldWD();  //喂狗信号
	CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0x01);//电压控制开   20140331
	//	LBT2401_TempSample::AD_Init(1);//add by qjm20140613
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	AdditionalEngine();

	////////拉高钳压     // 20140613  电压保护
	LBT2401_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	LBT2401_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);

	///////风扇调速  初始化时设定为60%
//#if MCU_PWMFAN_ENABLE == 1
    if(board_config::Feature_Enable_init->Mcu_PWMFan_Enable==1) 
	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,5);
//#else
    else
	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,100);
//#endif

	// 风扇初始化 
	I2CBusNew::AD5245_SET(0,0,I2C1);  //此处的100 改为0~255的数即可
}

void LBT2401_Base::LBT2401_FeatureEnable_Init(void)
{
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
	{
	
	}
	else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
	{
		
	}
	
	if(board_config::Feature_Enable_init->Maxwell_Machine==1)
    {
	    LBT2401_Fine_Tune_Starttime=0.005f;
        LBT2401_Fine_Tune_Mineerrorfactor=2E-5;
        LBT2401_Fine_Tune_Stepfactor=3E-4;
        LBT2401_Fine_Tune_Maxadjustfactor=0.06f;
    }
    else
    {
	    LBT2401_Fine_Tune_Starttime=0.01f;
        LBT2401_Fine_Tune_Mineerrorfactor=2E-5;
        LBT2401_Fine_Tune_Stepfactor=6E-6;
        LBT2401_Fine_Tune_Maxadjustfactor=0.06f;
    }
} 
void LBT2401_Base::IO_Init(void)
{
	for(int i=0;i<LBT2401_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
} 

void LBT2401_Base::ADC_Init(void)
{	
	LBT2401_ADC5DAC::Adc_Init();
}
void LBT2401_Base::DAC_Init(void)
{
	LBT2401_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 3;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT2401_SETTING_TIME;

	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT2401_Base::Software_Init(void)
{
	Func_Init();//函数指针初始化  四个基本操作
	EngineStuff();   // //引擎结构指针初始化


	// 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)    // //从0开始
	// 	{
	// 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//注册chn info
	// 		regEngine(i,board_config::st_channel_info[i].Channel_type);//注册引擎					
	// 	}

	//Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT2401_FINE_TUNE_STARTTIME;
	//Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT2401_FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
	//Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT2401_FINE_TUNE_MAXADJUSTFACTOR;
	//Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT2401_FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT2401_Fine_Tune_Starttime; 
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT2401_Fine_Tune_Mineerrorfactor;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT2401_Fine_Tune_Maxadjustfactor; 
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT2401_Fine_Tune_Stepfactor; 

	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT2401_ACCURACY * LBT2401_CTRL_VALUE_ERROR_FACTOR;

	board_config::Accuracy = LBT2401_ACCURACY;

	LBT2401_SampleManage::Init();
}

void LBT2401_Base::Func_Init(void)
{
	// 	board_config::Func_Adc_rd=&LBT2401_Base::ADC_rd;
	board_config::Func_Dac_out=&LBT2401_Base::DAC_out;
	board_config::Func_SetRange=&LBT2401_Base::SetRange;
	board_config::Func_SetRelay=&LBT2401_Base::SetRelay;
	board_config::Func_Sample = &LBT2401_SampleManage::SampleISR;
	//	board_config::Func_TempSample = &LBT2401_TempSample::Temp_rd;  // 20140613  温度测量
	board_config::Func_RedundantSafety= &LBT2401_Base::RedundantSafety; //冗余电压保护
	board_config::Func_ReleaseRedundantSafety=&LBT2401_Base::ReleaseRedundantSafety;//冗余解除操作

	board_config::FeedWatchDog = &Feed_CpldWD;
}

//commnet out chen 20130216
void LBT2401_Base::AdditionalEngine(void)
{
	OS_LogicProcess::SetVoltRelay.st_Disposabletask_ptr=&LBT2401_Base::SetVoltRelay[0];
	OS_LogicProcess::SetVoltRelay.stepLen=LBT2401_Base::SetVoltRelay_Len;
}
void LBT2401_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));

	SetCtrlOpenCh[0].uw32_DelayTime = 100;
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

	SetCtrlOpenCh[1].uw32_DelayTime = 200;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag = (LBT2401_RLY_Protect | LBT2401_RLY_OffGate);
	SetCtrlOpenCh[1].DacMove.DacFlag =  ACTIVE_DAC_VH | ACTIVE_DAC_VL;   //add by qjm

	SetCtrlOpenCh[2].uw32_DelayTime = 200;
	SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetCtrlOpenCh[3].uw32_DelayTime = 200;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT2401_RLY_Protect;

	SetCtrlOpenCh[4].uw32_DelayTime = 200;
	SetCtrlOpenCh[4].RelayMove.InactiveFlag = LBT2401_RLY_OffGate;

	SetCtrlOpenCh[5].uw32_DelayTime = 100;
	SetCtrlOpenCh[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

	SetCtrlCloseCh[1].uw32_DelayTime = 200;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT2401_RLY_OffGate;


	SetCtrlCloseCh[2].uw32_DelayTime = 200;
	SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT2401_RLY_Protect;

	SetCtrlCloseCh[3].uw32_DelayTime = 200;
	SetCtrlCloseCh[3].RelayMove.InactiveFlag = LBT2401_RLY_Output;

	SetCtrlCloseCh[4].uw32_DelayTime = 200;
	SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT2401_RLY_Protect;

	SetCCToCC[0].uw32_DelayTime = 100;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 200;
	SetCCToCC[1].RelayMove.ActiveFlag = LBT2401_RLY_OffGate;   
	SetCCToCC[1].RangeChangedFlag = 1;	

	SetCCToCC[2].uw32_DelayTime = 200;
	SetCCToCC[2].RelayMove.ActiveFlag = LBT2401_RLY_Protect; 
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 200;
	SetCCToCC[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[3].RangeChangedFlag = 1;

	SetCCToCC[4].uw32_DelayTime = 200;
	SetCCToCC[4].RelayMove.InactiveFlag = LBT2401_RLY_Protect;
	SetCCToCC[4].RangeChangedFlag = 1;

	SetCCToCC[5].uw32_DelayTime = 200;
	SetCCToCC[5].RelayMove.InactiveFlag = LBT2401_RLY_OffGate;
	SetCCToCC[5].RangeChangedFlag = 1;

	SetCCToCC[6].uw32_DelayTime = 100;
	SetCCToCC[6].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 100;
	//SetRstToOff[0].RelayMove.ActiveFlag =0;
	SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp
	//SetRstToOff[0].RangeChangedFlag = 0;

	//SetRstToOff[1].uw32_DelayTime = 0;
	//SetRstToOff[1].RelayMove.InactiveFlag =0;
	SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	//SetRstToOff[1].RangeChangedFlag = 0;

	SetVoltRelay[0].uw32_DelayTime = 2000;
	SetVoltRelay[0].RelayMove.ActiveFlag = LBT2401_RLY_VOLTAGE;
	//SetVoltRelay[0].DacMove.DacFlag = 0;
	//SetVoltRelay[0].RangeChangedFlag = 0;

	//SetVoltRelay[1].uw32_DelayTime = 0;
	SetVoltRelay[1].RelayMove.InactiveFlag =LBT2401_RLY_VOLTAGE;
	//SetVoltRelay[1].DacMove.DacFlag = 0;
	//SetVoltRelay[1].RangeChangedFlag = 0;
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
char LBT2401_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{ 
	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		LBT2401_ADC5DAC::Dac_out_Cur(ChannelIdx,Data);
		break;
	case ACTIVE_DAC_VH:
		LBT2401_ADC5DAC::Dac_out_Clamp_Vlt(1,Data);   // 20140613  电压保护
		break;
	case ACTIVE_DAC_VL:
		LBT2401_ADC5DAC::Dac_out_Clamp_Vlt(3,Data);    //VOUTC - DACL
		break;
	default:
		break;
	}
	return 1;
}


char LBT2401_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
	case CURRENT_RANGE_1:
		LBT2401_relaystatus[ChannelIdx].Bits.R_CH = 1;
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		//ARBUS_DELAY(10);
		LBT2401_relaystatus[ChannelIdx].Reg &=  ~(LBT2401_R_CM | LBT2401_R_CL);
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CM = 0;
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CL = 0;
		//CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		break;
	case CURRENT_RANGE_2:
		LBT2401_relaystatus[ChannelIdx].Bits.R_CM = 1;
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		//ARBUS_DELAY(10);
		LBT2401_relaystatus[ChannelIdx].Reg &=  ~(LBT2401_R_CH | LBT2401_R_CL);
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CH = 0;
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CL = 0;
		//CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		break;
	case CURRENT_RANGE_3:
		LBT2401_relaystatus[ChannelIdx].Bits.R_CL = 1;
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		//ARBUS_DELAY(10);
		LBT2401_relaystatus[ChannelIdx].Reg &=  ~(LBT2401_R_CH | LBT2401_R_CM);
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CH = 0;
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CM = 0;
		//CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		break;
	default:
		LBT2401_relaystatus[ChannelIdx].Reg &=  ~(LBT2401_R_CH | LBT2401_R_CM | LBT2401_R_CL);
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CH = 0;
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CM = 0;
		//             LBT2401_relaystatus[ChannelIdx].Bits.R_CL = 0;
		//CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		break;
	}
	ARBUS_DELAY(10);
	CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	return 1;
}
void LBT2401_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT2401_Relay2 MyRly;
	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Voltage)    //放开冗余电压保护操作
	{
		cpu_iowr_16(NBUS_WRITE_VOLTAGERELAY, 0); 
	}
	if(MyRly.m_Bits.Protect)
	{
		LBT2401_relaystatus[ChannelIdx].Bits.P_R = 1;
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.OffGate)
	{
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGH = 1;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGM = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Voltage)
	{
		cpu_iowr_16(NBUS_WRITE_VOLTAGERELAY, 1); 
	}
	if(MyRly.m_Bits.Protect)
	{
		LBT2401_relaystatus[ChannelIdx].Bits.P_R = 0;
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Output)
	{
		LBT2401_relaystatus[ChannelIdx].Bits.R_CH = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_CM = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_CL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.OffGate)
	{
		bool R[3] = {0,0,0};
		switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
		{
		case CURRENT_RANGE_1:
			break;
		case CURRENT_RANGE_2:
			R[1] = 1;
			break;
		case CURRENT_RANGE_3:
			R[2] = 1;
			break;
		default:
			R[0] = 1;
			R[1] = 1;
			R[2] = 1;
			break;
		}
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGH = R[0];
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGM = R[1];
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
		LBT2401_relaystatus[ChannelIdx].Bits.R_OFFGL = R[2];
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(0,LBT2401_relaystatus[ChannelIdx].Reg);
	}
	// 	#endif
}
void LBT2401_Base::Adjust_ADC_Gain(char type,char chnType)
{
	if(chnType == ADC_TYPE_I)
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

void LBT2401_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void LBT2401_Base::ADG_Reset(void)
{
	ControlPin::SetMeLow(LBT2401_Base::PinGroup[LBT2401_Pin_AdgRst]);
	ARBUS_DELAY(5);
	ControlPin::SetMeHigh(LBT2401_Base::PinGroup[LBT2401_Pin_AdgRst]);
}

void LBT2401_Base::changeADCRange(unsigned char range)
{
	ARBUS_DELAY(10);
	LBT2401_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void LBT2401_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[LBT2401_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0);
	ControlPin::SetMeLow(PinGroup[LBT2401_Pin_WD]);
}

char  LBT2401_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
	char temp;
	temp=ControlPin::ReadMe(PinGroup[LBT2401_Pin_RedundantSafety]);
//#if K2==1
    if(board_config::Feature_Enable_init->K2==1)
	{
	    if(!temp)
		    return 0;
	    else
		    return 1;
	}
//#else
    else
	{
	    if(temp)
		    return 0;
	    else
		    return 1;
	}
//#endif
}

char LBT2401_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
	St_DisposableMSG    DisposableMSG;

	DisposableMSG.Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetVoltRelay.st_Disposabletask_ptr; 
	DisposableMSG.Disposable_task.stepLen=OS_LogicProcess::SetVoltRelay.stepLen;
	DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;

	DisposableMSG.CH_Number=uc_ChannelNo;
	Step_Manager_task::EngineHandle[uc_ChannelNo]=OS_LogicProcess::requestLogicPorcess(uc_ChannelNo,&DisposableMSG);

	switch(OS_LogicProcess::getHandleStatus(Step_Manager_task::EngineHandle[uc_ChannelNo]))
	{
	case DISPOSABLE_ASSIGNED:                        //已经交付，待运行
		Step_Manager_task::EngineOverTimeTriger[uc_ChannelNo]=1;    
		Step_Manager_task::MsTimeEngineTask[uc_ChannelNo]=OS_ServeProcess::OS_Time;
		break;
	case DISPOSABLE_RUNNING:          //运行中
		unsigned long long TimeTicks;
		MsTime MyMsTime;
		MyMsTime.Second = Step_Manager_task::SysTimeEngineTask[uc_ChannelNo].Second;
		MyMsTime.Us100 = Step_Manager_task::SysTimeEngineTask[uc_ChannelNo].Us100;
		TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);

		if(Step_Manager_task::EngineOverTimeTriger[uc_ChannelNo]==1 && TimeTicks >= uw32_OverTime)         //超时？
		{
			Step_Manager_task::EngineOverTimeTriger[uc_ChannelNo]=0; 
			//引擎调用失败处理
		}
		break;
	case DISPOSABLE_FINISHED:           //结束
		Step_Manager_task::EngineOverTimeTriger[uc_ChannelNo]=0; 

		break;
	case DISPOSABLE_ERROR:            //出错
		//引擎调用失败处理
		break;
	case DISPOSABLE_DEAD:             //历史记录已经消亡
		break;
	default:
		break;
	}
	return 1;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
