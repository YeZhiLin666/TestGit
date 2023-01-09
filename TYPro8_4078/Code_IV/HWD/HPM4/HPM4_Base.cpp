#include "includes.h"

#define  HPM4_FINE_TUNE_STARTTIME          0.1f
//#define  HPM4_FINE_TUNE_MINERRORFACTOR     2E-5
#define  HPM4_FINE_TUNE_MINERRORFACTOR     30E-6// modified by Ron 080715, real ppm is one half of this number
#define  HPM4_FINE_TUNE_STEPFACTOR         3E-6 // was 3E-6. Modified by Ron 081215 

#define  HPM4_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  HPM4_ACCURACY                     0.0005f   //精度
#define  HPM4_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  HPM4_SETTING_TIME                 50//500       //电流上升沿时间，单位为100us
#define  HPM4_RISING_TIME (50)// Rising time,  unit is 100us
#define  HPM4_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask HPM4_Base::SetRstToCur[4];
St_DisposableTask HPM4_Base::SetCurToOff[2];
St_DisposableTask HPM4_Base::SetCCToCC[5];
St_DisposableTask HPM4_Base::SetRstToOff[1];
St_DisposableTask HPM4_Base::SetRstToVlt[6];
St_DisposableTask HPM4_Base::SetCVToCV[4];
St_DisposableTask HPM4_Base::SetCCToCV[7];
St_DisposableTask HPM4_Base::SetCVToCC[7];
St_DisposableTask HPM4_Base::SetVltToOff[3];
#if (IV_AUTORANGE == 1)
St_DisposableTask HPM4_Base::SetAutoRangeSwitch[5];
#endif

SSPI_Para  HPM4_Base::HPM4_SSP0DAC;
SSPI_Para  HPM4_Base::HPM4_SSP1ADC;
// #pragma arm section

// unsigned char HPM4_Base::SetRstToCur_Len = 4;  //6;    //this is maybe problem 20131107 chen
// unsigned char HPM4_Base::SetCurToOff_Len = 2;
// unsigned char HPM4_Base::SetCCToCC_Len = 5;
// unsigned char HPM4_Base::SetRstToOff_Len = 1;
// unsigned char HPM4_Base::SetRstToVlt_Len=6;
// unsigned char HPM4_Base::SetCVToCV_Len=4;
// unsigned char HPM4_Base::SetCCToCV_Len=7;
// unsigned char HPM4_Base::SetCVToCC_Len=7;
// unsigned char HPM4_Base::SetVltToOff_Len=3;

const St_Port HPM4_Base::PinGroup[HPM4_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port     Bits			PortType					  Function		  Address
    {1,				12,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_ADCDRDY
    //{4,				28,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_ADCBUSY_V
    {1,				18,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_ADCSTART
    {1,				26,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_CS_C
    {1,				27,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_CS_V

    {1,				11,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacRst
    {1,				25,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacCS
    {1,				3,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacLoad
    {1,				19,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_DacBusy

    {3,				20,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_CTR
    {3,				23,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_IVSR   // p3.23

    {3,				21,				eCPTYPE_MCUGPIO,		PWM0,				0},	//PWM_FAN   // p3.21

    {0,				21,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WERR2
    {0,				22,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WERR1
    {2,				5,				eCPTYPE_MCUGPIO,		PWM1,				0},	//PWM_TEMP

    {2,				11,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_DI1
    {2,				9,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_DI2
    {0,				22,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DO1
    {0,				21,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DO2

//	{0,				15,				eCPTYPE_MCUGPIO,		SSP0_SCK,			0},	//Pin_SCK0
//	{0,				17,				eCPTYPE_MCUGPIO,		SSP0_MISO,		    0},	//Pin_MISO0
//	{0,				18,				eCPTYPE_MCUGPIO,		SSP0_MOSI,		    0},	//Pin_MOSI0
    {0,				7,				eCPTYPE_MCUGPIO,		SSP1_SCK,			0},	//Pin_SCK1
    {0,				8,				eCPTYPE_MCUGPIO,		SSP1_MISO,		    0},	//Pin_MISO1
    {0,				9,				eCPTYPE_MCUGPIO,		SSP1_MOSI,		    0},	//Pin_MOSI1

    {0,				11,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_PWM_DIR
    {0,				10,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_TC_ENB
    {2,				3,				eCPTYPE_MCUGPIO,		GPIO_I,				0},
};
volatile St_HPM4_Relay HPM4_Base::HPM4_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
const unsigned char HPM4_Base::m_bHaveVoltageDac = 0;                                       // 03.28.2017 for HPM4:电压改为数字PID控制
const unsigned char HPM4_Base::m_bHaveClampVdac = 1;
// const unsigned char HPM4_Base::m_bChannelBasedClampVdac = 1;

void HPM4_Base::Hardware_Init(void)
{
    HPM4_FeatureEnable_Init();
    IO_Init();

    UnitDO_Reset(0,0);

    // CPLD_FUNC::Init();// Ron 111215
    memset((void *)(&HPM4_relaystatus), 0, sizeof(HPM4_relaystatus));

    // Modified by Ron 111215
    for(unsigned char ChannelIdx = 0; ChannelIdx<MAXCHANNELNO; ChannelIdx++)
    {
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.P_R = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.IVSD = 0;
        HPM4_relaystatus[ChannelIdx].Reg = 0;
        CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);//Ron 111215
    }

    //SSP0 for DAC    AD5360
    HPM4_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    HPM4_SSP0DAC.uc_Speed = 7;
    HPM4_SSP0DAC.uc_CPOL = 0;
    HPM4_SSP0DAC.uc_CPHA = 1;
    HPM4_SSP0DAC.uc_MSTR = 1;    //master
    HPM4_SSP0DAC.uc_BITS = 8;
    HPM4_SSP0DAC.uc_LSBF = 0;

    //ADS7175
    HPM4_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    HPM4_SSP1ADC.uc_Speed = 7; // 1: SPI 1 MHz
    HPM4_SSP1ADC.uc_CPOL = 1;
    HPM4_SSP1ADC.uc_CPHA = 1;
    HPM4_SSP1ADC.uc_MSTR = 1;    //master
    HPM4_SSP1ADC.uc_BITS = 8;
    HPM4_SSP1ADC.uc_LSBF = 0;

    //ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();


    //DAC INIT
    ControlPin::SetMeLow(PinGroup[HPM4_Pin_DacLoad]);
    HPM4_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);

    Engine();// 有电压环


    ////////拉高钳压    // 20140613  电压保护
    for(unsigned char i=0; i<4; i++)
    {
        HPM4_Base::DAC_out(i,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
        HPM4_Base::DAC_out(i,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
    }
// 	//Begin adding by Ron 7-27-15
// 	LPC4078PWM::CLKPWR_ConfigPPWR (0, ENABLE); // Ron 7-24-15
// 	LPC4078PWM::PWM_ChannelCmd(0, 6, ENABLE);// Ron 7-24-15
//   LPC4078PWM::PWM_ChannelConfig(0,6,PWM_CHANNEL_SINGLE_EDGE); // Ron 7-24-15
//   LPC4078PWM::PWM_MatchUpdate(0,0,24e2, PWM_MATCH_UPDATE_NOW); // Ron 7-24-15
// //    LPC4078PWM::PWM_MatchUpdate(0,6,23e2, PWM_MATCH_UPDATE_NOW);  //intialization, 100% duty cycle 7-24-15 by Ron
//   LPC4078PWM::PWM_ResetCounter(0);
//   LPC4078PWM::PWM_CounterCmd(0, ENABLE);
//   LPC4078PWM::PWM_Cmd(0, ENABLE);
// 	board_config::bFanAdj=1; // added by Ron 7-27-15
// 	FanAdj_task::AdjMod=PWMMOD; // Added by Ron 7-27-15
    ADC_Init();
}

void HPM4_Base::IO_Init(void)
{
    for(int i=0; i<HPM4_Total_Pins; i++)
    {
        ControlPin::SetIObyCP(PinGroup[i]);
    }
}
void HPM4_Base::ADC_Init(void)
{
    HPM4_ADC5DAC::Adc_Init();
    TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}
void HPM4_Base::DAC_Init(void)
{
    HPM4_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 8;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = HPM4_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = HPM4_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = HPM4_Tpid_Tsys;

    ARBUS_DELAY(10000);    //300us reset time  AD5360????
}

void HPM4_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();

    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = HPM4_FINE_TUNE_STARTTIME;   //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = HPM4_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = HPM4_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = HPM4_FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime =0.3072;
//  safety_check::m_CtrlValueSetting.fErrorFactor       = HPM4_ACCURACY * HPM4_CTRL_VALUE_ERROR_FACTOR;


    board_config::Accuracy = HPM4_ACCURACY;
    HPM4_SampleManage::Init();

    IR::pIR_ADC = &(HPM4_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(HPM4_SampleManage::renew_IR_ADC);
//	  pIR_Trigger_ADC = &(HPM4_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(HPM4_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=HPM4SETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=HPM4SETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=HPM4SETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=HPM4SETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=HPM4MINIMUMPULSE;
}

void HPM4_Base::HPM4_FeatureEnable_Init(void)
{
    board_config::Feature_Enable_init.Redundant_Safety_Mode=0;                                                // 03.28.2017
    board_config::Feature_Enable_init.Power_Off_Enable=1;
    board_config::Feature_Enable_init.OS_Tick_Interval=400;
}

void HPM4_Base::Func_Init(void)
{
    board_config::Func_Dac_out=&HPM4_Base::DAC_out;
    board_config::Func_SetRange=&HPM4_Base::SetRange;
    board_config::Func_SetRelay=&HPM4_Base::SetRelay;
    board_config::Func_Sample =&HPM4_SampleManage::SampleISR;    //timer0 中断
    board_config::Func_UnitDO=&HPM4_Base::OUTPUT_UnitDO;
    board_config::Func_UnitDI=&HPM4_Base::Read_UnitDI;
    board_config::Func_UnitDI_ReadAll = &HPM4_Base::UnitDI_ReadAll;
    board_config::Func_UnitDO_Reset = &HPM4_Base::UnitDO_Reset;
    board_config::Func_RTCProcedureForIV=&HPM4_Base::RTCProcedureForHPM4;
    board_config::FeedWatchDog = &HPM4_Base::Feed_CpldWD;
    PowerLostCheck::Func_PowerOffSafetCheck = &HPM4_Base::PowerOffCheck;
// 	board_config::Func_FanAdj = &HPM4_Base::FanAdjOperature; // Added by Ron 7-27-15
}

void HPM4_Base::Engine(void)
{
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &HPM4_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlRstToCur.stepLen = sizeof(SetRstToCur) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToCur_Len;

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &HPM4_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToCur) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToCur_Len;


    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &HPM4_Base::SetRstToOff[0];
    OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToOff_Len;

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &HPM4_Base::SetRstToOff[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetRstToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToOff_Len;

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &HPM4_Base::SetRstToCur[0];    //指向数组0
    OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetRstToCur) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToCur_Len;

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &HPM4_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetRstToCur) / sizeof(St_DisposableTask); //HPM4_Base::SetRstToCur_Len;

    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &HPM4_Base::SetCCToCC[0];
    OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCCToCC) / sizeof(St_DisposableTask); //HPM4_Base::SetCCToCC_Len;

    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &HPM4_Base::SetCCToCC[0];
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCCToCC) / sizeof(St_DisposableTask); //HPM4_Base::SetCCToCC_Len;

    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &HPM4_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetCurToOff_Len ;

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &HPM4_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCurToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetCurToOff_Len ;

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &HPM4_Base::SetCCToCC[0];
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetCCToCC) / sizeof(St_DisposableTask); //HPM4_Base::SetCCToCC_Len;

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &HPM4_Base::SetCCToCC[0];
    OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetCCToCC) / sizeof(St_DisposableTask); //HPM4_Base::SetCCToCC_Len;

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &HPM4_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetCurToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetCurToOff_Len ;

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &HPM4_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetCurToOff) / sizeof(St_DisposableTask); //HPM4_Base::SetCurToOff_Len ;
}

void HPM4_Base::EngineStuff(void)
{
    memset(&SetRstToCur,  0,    sizeof(SetRstToCur));
    memset(&SetCurToOff,  0,    sizeof(SetCurToOff));
    memset(&SetCCToCC,    0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,  0,    sizeof(SetRstToOff));
    memset(&SetCCToCV,    0,    sizeof(SetCCToCV));
    memset (&SetRstToVlt, 0 ,   sizeof(SetRstToVlt));
    memset (&SetVltToOff, 0 ,   sizeof(SetVltToOff));
    memset (&SetCVToCV,   0 ,   sizeof(SetCVToCV));
    memset (&SetCVToCC,   0 ,   sizeof(SetCVToCC));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    //电流开通道
    SetRstToCur[0].uw32_DelayTime = 100;
    SetRstToCur[0].RelayMove.ActiveFlag |= HPM4_RLY_Protect;  // Protect goes 1
    SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetRstToCur[1].uw32_DelayTime = 100;
    SetRstToCur[1].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetRstToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

    SetRstToCur[2].uw32_DelayTime = 100;
    SetRstToCur[2].RelayMove.InactiveFlag = HPM4_RLY_Protect;

    SetRstToCur[3].uw32_DelayTime = 100;
    SetRstToCur[3].DacMove.DacFlag = ACTIVE_DAC_CUR;


    //电流关通道
    SetCurToOff[0].uw32_DelayTime = 100;
    SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetCurToOff[1].uw32_DelayTime = 100;
    SetCurToOff[1].RelayMove.InactiveFlag = HPM4_RLY_Output;


    //---------------------------------------------//
    //电流切换到电流
    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetCCToCC[0].RangeChangedFlag = 1;

    SetCCToCC[1].uw32_DelayTime = 100;
    SetCCToCC[1].RelayMove.ActiveFlag = HPM4_RLY_Protect;
    SetCCToCC[1].RangeChangedFlag = 1;

    SetCCToCC[2].uw32_DelayTime = 100;
    SetCCToCC[2].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[2].RangeChangedFlag = 1;

    SetCCToCC[3].uw32_DelayTime = 100;
    SetCCToCC[3].RelayMove.InactiveFlag = HPM4_RLY_Protect;
    SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
// 	SetCCToCC[3].RangeChangedFlag = 1;

    SetCCToCC[4].uw32_DelayTime = 100;
    SetCCToCC[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //电压开通道
    SetRstToVlt[0].uw32_DelayTime = 100;
// 	SetRstToVlt[0].RelayMove.InactiveFlag = BT11_RLY_IVSD;  // I/V切换到I
    SetRstToVlt[0].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO|ACTIVE_DAC_CUR_ZERO;

    SetRstToVlt[1].uw32_DelayTime = 100;
    SetRstToVlt[1].RelayMove.ActiveFlag = HPM4_RLY_Protect;

    SetRstToVlt[2].uw32_DelayTime = 100;
    SetRstToVlt[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

    SetRstToVlt[3].uw32_DelayTime = 100;
    SetRstToVlt[3].RelayMove.InactiveFlag = HPM4_RLY_Protect;

    SetRstToVlt[4].uw32_DelayTime = 100;
    SetRstToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;

    SetRstToVlt[5].uw32_DelayTime = 100;
    SetRstToVlt[5].RelayMove.ActiveFlag = HPM4_RLY_IVSD;  // I/V切换到V

    //电压关通道
    SetVltToOff[0].uw32_DelayTime = 100;
    SetVltToOff[0].DacMove.DacFlag =  ACTIVE_DAC_CUR_ZERO |ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetVltToOff[0].RelayMove.InactiveFlag = HPM4_RLY_IVSD;  //I/V继电器切换至I

    SetVltToOff[1].uw32_DelayTime = 100;
    SetVltToOff[1].DacMove.DacFlag =  ACTIVE_DAC_VLT_ZERO;

    SetVltToOff[2].uw32_DelayTime = 100;
    SetVltToOff[2].RelayMove.InactiveFlag = HPM4_RLY_Output;


    //电压切换到电压  //等待时序验证
    SetCVToCV[0].uw32_DelayTime =100;
    SetCVToCV[0].RelayMove.ActiveFlag = HPM4_RLY_Protect;
    SetCVToCV[0].RangeChangedFlag = 1;

    SetCVToCV[1].uw32_DelayTime =200;
    SetCVToCV[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetCVToCV[1].RangeChangedFlag = 1;

    SetCVToCV[2].uw32_DelayTime =100;
    SetCVToCV[2].DacMove.DacFlag = ACTIVE_DAC_VLT;
    //SetCVToCV[2].RangeChangedFlag = 0;

    SetCVToCV[3].uw32_DelayTime =100;
    SetCVToCV[3].RelayMove.InactiveFlag = HPM4_RLY_Protect;
    SetCVToCV[3].RangeChangedFlag = 1;

    //电压切换到电流
    SetCVToCC[0].uw32_DelayTime = 50;
    SetCVToCC[0].RelayMove.InactiveFlag = HPM4_RLY_IVSD;     //切换到电流
    SetCVToCC[0].RangeChangedFlag = 0;

    SetCVToCC[1].uw32_DelayTime = 100;
    SetCVToCC[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
    SetCVToCC[1].RangeChangedFlag = 0;

    SetCVToCC[2].uw32_DelayTime = 100;
    SetCVToCC[2].RelayMove.ActiveFlag = HPM4_RLY_Protect;
    SetCVToCC[2].RangeChangedFlag = 1;

    SetCVToCC[3].uw32_DelayTime = 100;
    SetCVToCC[3].RelayMove.InactiveFlag = HPM4_RLY_Output;
    SetCVToCC[3].RangeChangedFlag = 1;

    SetCVToCC[4].uw32_DelayTime = 100;
    SetCVToCC[4].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetCVToCC[4].RangeChangedFlag = 1;

    SetCVToCC[5].uw32_DelayTime = 100;
    SetCVToCC[5].RelayMove.InactiveFlag =HPM4_RLY_Protect;
    SetCVToCC[5].RangeChangedFlag = 1;

    SetCVToCC[6].uw32_DelayTime = 100;
    SetCVToCC[6].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //电流切换到电压
    SetCCToCV[0].uw32_DelayTime = 100;
    SetCCToCV[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCV[0].RangeChangedFlag = 0;

    SetCCToCV[1].uw32_DelayTime = 100;
    SetCCToCV[1].RelayMove.ActiveFlag = HPM4_RLY_Protect;
    SetCCToCV[1].RangeChangedFlag = 1;

    SetCCToCV[2].uw32_DelayTime = 100;
    SetCCToCV[2].RelayMove.InactiveFlag = HPM4_RLY_Output;
    SetCCToCV[2].RangeChangedFlag = 1;

    SetCCToCV[3].uw32_DelayTime = 100;
    SetCCToCV[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetCCToCV[3].RangeChangedFlag = 1;

    SetCCToCV[4].uw32_DelayTime = 100;
    SetCCToCV[4].RelayMove.InactiveFlag = HPM4_RLY_Protect;
    SetCCToCV[4].RangeChangedFlag = 1;

    SetCCToCV[5].uw32_DelayTime = 100;
    SetCCToCV[5].DacMove.DacFlag = ACTIVE_DAC_VLT;
    SetCCToCV[5].RangeChangedFlag = 0;

    SetCCToCV[6].uw32_DelayTime = 100;
    SetCCToCV[6].RelayMove.ActiveFlag = HPM4_RLY_IVSD;   //切换到电压
    SetCCToCV[6].RangeChangedFlag = 0;

    SetRstToOff[0].uw32_DelayTime = 10;
    SetRstToOff[0].DacMove.DacFlag = 0;
    SetRstToOff[0].RangeChangedFlag = 0;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 100;
    SetAutoRangeSwitch[1].RelayMove.ActiveFlag = HPM4_RLY_Protect;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 100;
    SetAutoRangeSwitch[2].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.InactiveFlag = HPM4_RLY_Protect;
    SetAutoRangeSwitch[3].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
// 	SetAutoRangeSwitch[3].RangeChangedFlag = 1;

    SetAutoRangeSwitch[4].uw32_DelayTime = 100;
    SetAutoRangeSwitch[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
		#endif
}

char HPM4_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    unsigned long value = Data;    //强制保护
    unsigned long value_Full = (0xFFFFFF-value);
    unsigned char chn =  ChannelIdx*4;;
    switch(type)
    {
    case ACTIVE_DAC_CUR:
        //chn= ChannelIdx*4;
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HPM4_ADC5DAC::Dac_out_Cur(chn,value);
        break;

    case ACTIVE_DAC_VLT:
// 			if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bCaliBoard)
// 			{
// 				if(value > 0x800000)
// 					value = ((value - 0x800000)>>1) + 0x800000;
// 				else
// 					value = 0x800000 - ((0x800000 - value)>>1);
// 			}
        //chn= ChannelIdx*4+1;
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HPM4_ADC5DAC::Dac_out_Vlt(chn+1,value_Full);
        break;
///////////////////钳位
    case ACTIVE_DAC_VH:
        //chn= ChannelIdx*4+2;
        HPM4_ADC5DAC::Dac_out_Clamp_Vlt(chn+2,value_Full);    //VOUTA - DACH  // 20140613  电压保护
        break;
    case ACTIVE_DAC_VL:
        //chn= ChannelIdx*4+3;
        HPM4_ADC5DAC::Dac_out_Clamp_Vlt(chn+3,value_Full);    //VOUTC - DACL
    default:
        break;
    }
    return 1;
}

char HPM4_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{   ///Modified by Philip 3/9/2015
    switch(CurrRange)
    {
    case CURRENT_RANGE_1:     //CURRENT_RANGE_HIGH:
        HPM4_relaystatus[ChannelIdx].Bits.R_CH = 1;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CM | HPM4_RLY_R_CL | HPM4_RLY_R_UL);
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
        break;
    case CURRENT_RANGE_2:  //CURRENT_RANGE_MED:
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        HPM4_relaystatus[ChannelIdx].Bits.R_CM = 1;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CH | HPM4_RLY_R_CL | HPM4_RLY_R_UL);
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
        break;
    case CURRENT_RANGE_3:          //CURRENT_RANGE_LOW:
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        HPM4_relaystatus[ChannelIdx].Bits.R_CL = 1;
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CH | HPM4_RLY_R_CM | HPM4_RLY_R_UL);
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
        break;
    case CURRENT_RANGE_4:           //CURRENT_RANGE_UL:
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        HPM4_relaystatus[ChannelIdx].Bits.R_UL = 1;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CH | HPM4_RLY_R_CM | HPM4_RLY_R_CL);
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
        break;
    default:
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CH | HPM4_RLY_R_CM | HPM4_RLY_R_CL | HPM4_RLY_R_UL);
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
        //return 0;
        break;
    }
    CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    return 1;
}
void HPM4_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_HPM4_Relay2 MyRly;

    MyRly.m_Reg = SetRly;
    if(MyRly.m_Bits.Protect)
    {
        HPM4_relaystatus[ChannelIdx].Bits.P_R = 1;
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    }
    if(MyRly.m_Bits.IVSD)
    {
        HPM4_relaystatus[ChannelIdx].Bits.IVSD= 1;
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    }
    CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);

    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Protect)
    {
        HPM4_relaystatus[ChannelIdx].Bits.P_R = 0;
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    }
    if(MyRly.m_Bits.IVSD)
    {
        HPM4_relaystatus[ChannelIdx].Bits.IVSD= 0;
        //CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    }
    CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
// 	if(MyRly.m_Bits.V)								// by Philip 3/9/2015
// 		ControlPin::SetMeLow(Pin_V,0);	// by Philip 3/9/2015


    if(MyRly.m_Bits.Output)
    {
        //HPM4_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //HPM4_relaystatus[ChannelIdx].Bits.R_UL = 0;
        HPM4_relaystatus[ChannelIdx].Reg &= ~(HPM4_RLY_R_CH | HPM4_RLY_R_CM | HPM4_RLY_R_CL | HPM4_RLY_R_UL);
        ARBUS_DELAY(10);
        CPLD_FUNC::Write((CPLD_BASE_ADDR + 2* ChannelIdx),HPM4_relaystatus[ChannelIdx].Reg);
    }
}

void HPM4_Base::Feed_CpldWD(void)
{
// 	ControlPin::SetMeHigh(Pin_WD,1);
// 	TIMERSFunc::DELAY_SYS(200);
// 	ControlPin::SetMeLow(Pin_WD,0);
}

// void HPM4_Base::ADC_ADG_open(unsigned char ChannelIdx)
// {
// 	return;
// }
unsigned char HPM4_Base::getBID(void)
{
    //BS0-BS7    P3.24-P3.31
    unsigned char BoardID = 0;

    St_Port MyPort;
    MyPort.PortType = eCPTYPE_MCUGPIO;
    MyPort.Function = GPIO_I;
    MyPort.Port = 3;
    MyPort.Bits = 24;

    for(unsigned char i = 0; i<8; i++)
    {
        ControlPin::SetIObyCP(MyPort);
        BoardID |= ControlPin::ReadMe(MyPort)<<i;

        MyPort.Bits++;
    }

    return BoardID;
}

// Begin adding by Ron 7-27-15
// char HPM4_Base::FanAdjOperature(unsigned long m_Level,unsigned long FanAdjMod)
// {
// 		if(FanAdjMod!=I2CMOD)
// 		{
// 			board_config::bFanAdj=0;
// 			I2CBusNew::AD5245_SET(0,0,1);
// 			return 0;
// 		}
// // added by Ron 081415 fan speed adjustment
// 		float I_current= ChannelInfo_Manager::Read_SpecifiedMetavariable2(0,MetaCode_PV_Current);
// 		unsigned long SetValue = 1900; //Fan PMW adjustment
// 		unsigned char CRange = OS_LogicProcess::m_RunningTask[0].logicProcessMSg.next_Range;
// //	if (abs(I_current) > 160 || ((abs(abs(ChannelInfo_Manager::m_CurFineTune_Info[0].m_LastSetValue)-160) <= 2)
// //			&& abs(I_current) <= 160))
// //	if (abs(I_current) > 160)
// 		if (CRange == CURRENT_RANGE_2)
// 		  {
// 			 SetValue = 2400*(I_current*I_current*3.04e-4+0.333);
// 		   if (abs(I_current) < 25 || ((abs(abs(ChannelInfo_Manager::m_CurFineTune_Info[0].m_LastSetValue)-25) <= 2)
// 		      && abs(I_current) >= 25))
//     	    SetValue = 1300;
// 		  }
// 		else
// 			{
// 			 SetValue = 2400*(I_current*I_current*0.19e-4+0.333);
// 		   if (abs(I_current) < 100 || ((abs(abs(ChannelInfo_Manager::m_CurFineTune_Info[0].m_LastSetValue)-100) <= 2)
// 		      && abs(I_current) >= 100))
//     	    SetValue = 1300;
// 		  }
// 		    LPC4078PWM::PWM_ChannelCmd(0, 6, ENABLE);// Ron 7-24-15
//         LPC4078PWM::PWM_ResetCounter(0);
//         LPC4078PWM::PWM_CounterCmd(0, ENABLE);
//         LPC4078PWM::PWM_Cmd(0, ENABLE);
// 		    LPC4078PWM::PWM_MatchUpdate(0,6, SetValue, PWM_MATCH_UPDATE_NOW);
// 		return 1;
// }
// End adding by Ron 7-27-15
char HPM4_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
{
    /* unsigned char m_OutPutValue;

     if(Value>0)
       m_OutPutValue=1;
     else
       m_OutPutValue=0;

     switch (DO_Idx)
     {
       case 0:
         if(m_OutPutValue)
    		{
           ControlPin::SetMeHigh(PinGroup[HPM4_Pin_DO1]);
    			ChannelInfo_Manager::BuildIn_DO[0][0] = 1;
    		}
         else
    		{
           ControlPin::SetMeLow(PinGroup[HPM4_Pin_DO1]);
    			ChannelInfo_Manager::BuildIn_DO[0][0] = 0;
    		}
         break;
       case 1:
         if(m_OutPutValue)
    		{
           ControlPin::SetMeHigh(PinGroup[HPM4_Pin_DO2]);
    			ChannelInfo_Manager::BuildIn_DO[0][1] = 1;
    		}
         else
    		{
           ControlPin::SetMeLow(PinGroup[HPM4_Pin_DO2]);
    			ChannelInfo_Manager::BuildIn_DO[0][1] = 0;
    		}
         break;
       default:
         break;
     }*/

    if(Value > 0)
    {
        ControlPin::SetMeHigh(PinGroup[HPM4_Pin_DO1+DO_Idx]);
        ChannelInfo_Manager::BuildIn_DO[0][DO_Idx] = 1;
    }
    else
    {
        ControlPin::SetMeLow(PinGroup[HPM4_Pin_DO1+DO_Idx]);
        ChannelInfo_Manager::BuildIn_DO[0][DO_Idx] = 0;
    }

    return 1;
}
char HPM4_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
//	char ReturnValue=0;
    /*	switch (DI_Idx)
    	{
    		case 0:
    		  ReturnValue=GPIOFunc::RD_G_Port(PinGroup[HPM4_Pin_DI1].Port ,PinGroup[HPM4_Pin_DI1].Bits);
    			break;
    		case 1:
    			ReturnValue=GPIOFunc::RD_G_Port(PinGroup[HPM4_Pin_DI2].Port ,PinGroup[HPM4_Pin_DI2].Bits);
    			break;
    		default:
    			break;
    	}*/
    return (char)(GPIOFunc::RD_G_Port(PinGroup[HPM4_Pin_DI1+DI_Idx].Port ,PinGroup[HPM4_Pin_DI1+DI_Idx].Bits));
}
void HPM4_Base::UnitDI_ReadAll(void)
{
    /*if(ControlPin::ReadMe(PinGroup[HPM4_Pin_DI1]))
    {
    	if(Sample_Pro::Digital_SampleCounter[0][0]>3)
    	{
    		ChannelInfo_Manager::BuildIn_DI[0][0]=1;  //modified by xp
    		Sample_Pro::Digital_SampleCounter[0][0]=3;
    	}
    	Sample_Pro::Digital_SampleCounter[0][0]++;
    }
    else
    {
    	Sample_Pro::Digital_SampleCounter[0][0]=0;
    	ChannelInfo_Manager::BuildIn_DI[0][0]=0;
    }

    if(ControlPin::ReadMe(PinGroup[HPM4_Pin_DI2]))
    {
    	if(Sample_Pro::Digital_SampleCounter[0][1]>3)
    	{
    		ChannelInfo_Manager::BuildIn_DI[0][1]=1;  //modified by xp
    		Sample_Pro::Digital_SampleCounter[0][1]=3;
    	}
    	Sample_Pro::Digital_SampleCounter[0][1]++;
    }
    else
    {
    	Sample_Pro::Digital_SampleCounter[0][1]=0;
    	ChannelInfo_Manager::BuildIn_DI[0][1]=0;
    }*/

//暂不支持 zyx 161205
// 	for(unsigned char uc_i=0;uc_i<MAX_BUILDIN_DI_COUNT;uc_i++)
// 	{
// 		if(ControlPin::ReadMe(PinGroup[HPM4_Pin_DI1+uc_i]))
// 	{
// 		if(Sample_Pro::Digital_SampleCounter[0][uc_i]>3)
// 		{
// 			//ChannelInfo_Manager::BuildIn_DI[0][uc_i]=1;  //modified by xp
// 			Sample_Pro::Digital_SampleCounter[0][uc_i]=3;
// 		}
// 		Sample_Pro::Digital_SampleCounter[0][uc_i]++;
// 	}
// 	else
// 	{
// 		Sample_Pro::Digital_SampleCounter[0][uc_i]=0;
// 	//	ChannelInfo_Manager::BuildIn_DI[0][uc_i]=0;
// 	}
// 	}
//暂不支持 zyx 161205
}
char HPM4_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    OUTPUT_UnitDO(0,0,Value);
    OUTPUT_UnitDO(0,1,Value);
    return 1;
}
char HPM4_Base::RTCProcedureForHPM4(unsigned long CH_Number,unsigned long Value)
{
    static int myFlag = 0;
    if(myFlag == 0)
    {
        ControlPin::SetMeLow(PinGroup[HPM4_Pin_IVSR]);//added by RON, 3-24-2015
        myFlag = 1;
    }
    else
    {
        ControlPin::SetMeHigh(PinGroup[HPM4_Pin_IVSR]);
        myFlag = 0;
    }
    return 1;
}
unsigned char HPM4_Base::PowerOffCheck(void)
{
    unsigned char temp;
    temp = !(ControlPin::ReadMe(HPM4_Base::PinGroup[HPM4_Pin_PowerOff]));
    return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
