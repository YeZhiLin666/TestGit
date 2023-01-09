#include "../../Entry/includes.h"

//_DAC_RAMP_NEGTIVE_ Macro should be 1 in PWM1 system,do not change it until a new system added
#define _DAC_RAMP_NEGTIVE_    1

#define _DEBUG_TIMING_        0

#define  PWM_FINE_TUNE_STARTTIME          0.25f
#define  PWM_FINE_TUNE_MINERRORFACTOR     2E-5
#define  PWM_FINE_TUNE_STEPFACTOR         6E-6
#define  PWM_FINE_TUNE_MAXADJUSTFACTOR    0.06f

#define  PWM_ACCURACY                     0.0005f   //精度
#define  PWM_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  PWM_SETTING_TIME                 500       //PWM的电流上升沿时间，单位为100us
#define  PWM_RISING_TIME                  (50)          //PWM Rising time 5ms, unit is 100us
#define  PWM_Tpid_Tsys           0.1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask PWM_Base::SetCtrl_RstToCur[1];
St_DisposableTask PWM_Base::SetCtrl_RstToVlt[1];
St_DisposableTask PWM_Base::SetCtrl_RstToOff[1];
St_DisposableTask PWM_Base::SetCtrl_OffToRst[1];
St_DisposableTask PWM_Base::SetCtrl_OffToCur[1];
St_DisposableTask PWM_Base::SetCtrl_OffToVlt[1];
St_DisposableTask PWM_Base::SetCtrl_CurToCur[1];
St_DisposableTask PWM_Base::SetCtrl_CurToVlt[1];
St_DisposableTask PWM_Base::SetCtrl_CurToOff[1];
St_DisposableTask PWM_Base::SetCtrl_CurToRst[1];
St_DisposableTask PWM_Base::SetCtrl_VltToVlt[1];
St_DisposableTask PWM_Base::SetCtrl_VltToCur[1];
St_DisposableTask PWM_Base::SetCtrl_VltToOff[1];
St_DisposableTask PWM_Base::SetCtrl_VltToRst[1];
St_DisposableTask PWM_Base::SetCtrlOpenAcim[1];	//Open ACIM Relay
St_DisposableTask PWM_Base::SetCtrlCloseAcim[1];	//Close ACIM Relay
#if (IV_AUTORANGE == 1)
St_DisposableTask PWM_Base::SetAutoRangeSwitch[1];
#endif
SSPI_Para  PWM_Base::PWM_SSP0DAC;
SSPI_Para  PWM_Base::PWM_SSP1ADC;

St_TransitionInfo PWM_Base::TransitionInfo[2];
DSP_Basic_Parameter  PWM_Base::Record_DSP_Basic_Parameter[MAX_DSP_NUMBER];
DSP_Adjust_Parameter PWM_Base::Record_DSP_Adjust_Parameter[MAX_DSP_NUMBER];
DSP_Indentity_Parameter PWM_Base::Record_DSP_Indentity_Parameter[MAX_DSP_NUMBER];
#pragma arm section

// unsigned char PWM_Base::Length_RstToCur = 1;
// unsigned char PWM_Base::Length_RstToVlt = 1;
// unsigned char PWM_Base::Length_RstToOff = 1;
// unsigned char PWM_Base::Length_OffToRst = 1;
// unsigned char PWM_Base::Length_OffToCur = 1;
// unsigned char PWM_Base::Length_OffToVlt = 1;
// unsigned char PWM_Base::Length_CurToCur = 1;
// unsigned char PWM_Base::Length_CurToVlt = 1;
// unsigned char PWM_Base::Length_CurToOff = 1;
// unsigned char PWM_Base::Length_CurToRst = 1;
// unsigned char PWM_Base::Length_VltToVlt = 1;
// unsigned char PWM_Base::Length_VltToCur = 1;
// unsigned char PWM_Base::Length_VltToOff = 1;
// unsigned char PWM_Base::Length_VltToRst = 1;

const St_Port PWM_Base::PinGroup[PWM_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port      Bits			  PortType					 Function		            Address
    {1,        12,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {0,        31,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        7,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        8,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
    {2,        31,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//#if ( _PWM_VERSION_ ==0)
    {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
//#else
    {2,        11,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        9,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        14,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        5,         eCPTYPE_MCUGPIO,   PWM1,                0},
//#endif
    {2,        30,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
    {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
    {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},

    {0,       30,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//busy2 current
    {1,       19,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//busy1  voltage
    {1,       28,          eCPTYPE_MCUGPIO,   GPIO_O,                0},//cover
    {1,       22,          eCPTYPE_MCUGPIO,   GPIO_O,                0},//dcs2 current
    {1,       25,          eCPTYPE_MCUGPIO,   GPIO_O,                0},//dcs1 voltage
// 	{2,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//led1
// 	{1,        5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//led2
// 	{0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//led3
    {4,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},//VR
    {0,        12,        eCPTYPE_MCUGPIO,   GPIO_O,                0},//ACI
    {1,        2,        eCPTYPE_MCUGPIO,   GPIO_O,                0},//DO1
    {2,        6,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//PWM_Pin_InductanceFanCheckSignal
    {2,        7,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//PWM_Pin_BoardFanCheckSignal
    {0,        6,        	eCPTYPE_MCUGPIO,   GPIO_O,                1},//RED LED
    {0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                1},//ORANGE LED
    {0,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                1},//GREEN LED
    {0,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                1},//BUZZER
};
const unsigned char PWM_Base::m_bHaveVoltageDac = 1;
const unsigned char PWM_Base::m_bHaveClampVdac = 1;
// const unsigned char PWM_Base::m_bChannelBasedClampVdac = 1;
unsigned char  PWM_Base::PWM_LEDRunStatus;
volatile St_PWM_Relay PWM_Base::PWM_relaystatus[MAXCHANNELNO];    //????×?éíμ?relay×′ì?

//add Wzh
St_PWM_Error PWM_Base::m_Error;
unsigned long long PWM_Base::TimePassed;
//end Wzh
void PWM_Base::Hardware_Init(void)
{
    PWM_FeatureEnable_Init();
    IO_Init();
    Feed_CpldWD();

    board_config::bHaveRedundantCheck = 0;
// 	board_config::ClampH_Outputflag = 0;
// 	board_config::ClampL_Outputflag = 0;
    PWM_LEDRunStatus=SYSTEM_IDLE;
    CPLD_FUNC::Init();
    memset((void *)(&PWM_relaystatus[0]), 0, MAXCHANNELNO);
    Feed_CpldWD();

    ControlPin::SetMeHigh(PinGroup[PWM_Pin_VM1]);
    ControlPin::SetMeHigh(PinGroup[PWM_Pin_VR]);
    //SSP0 for DAC    AD5360
    PWM_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    PWM_SSP0DAC.uc_Speed = 5;
    PWM_SSP0DAC.uc_CPOL = 0;
    PWM_SSP0DAC.uc_CPHA = 1;
    PWM_SSP0DAC.uc_MSTR = 1;    //master
    PWM_SSP0DAC.uc_BITS = 8;
    PWM_SSP0DAC.uc_LSBF = 0;
    //ADS1259
    PWM_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    PWM_SSP1ADC.uc_Speed = 2;
    PWM_SSP1ADC.uc_CPOL = 0;
    PWM_SSP1ADC.uc_CPHA = 1;
    PWM_SSP1ADC.uc_MSTR = 1;    //master
    PWM_SSP1ADC.uc_BITS = 8;
    PWM_SSP1ADC.uc_LSBF = 0;

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
    Feed_CpldWD();

    ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();
    Feed_CpldWD();
    //    PWM_TempSample::AD_Init(8); //根据硬件调整通道数值 add by qjm20140613

    DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);     //高钳压最大值
    DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);      //低钳压最小值
    Engine();// 有电压环
//#if ( _PWM_VERSION_ ==0)
    if(board_config::Feature_Enable_init.PWM_Version_Control==0)
    {
        m_Error.CheckCounter = 0;
        m_Error.Counter = 0;
    }
//#else
    else
    {
        m_Error.DlinkErrCounter = 0;
        m_Error.POWERErrCounter = 0;
        m_Error.IGBTErrCounter = 0;
        m_Error.Inverter2FuseCounter = 0;
        m_Error.ErrorTypeFlag = 0;
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_VVS1]);  //add by qjm 20150717
    }
//#endif
    m_Error.Code = 0;
    m_Error.State = ErrorSM_Idle;    //2015/01/23，对未初始化变量进行初始化。
//Init fan
    board_config::bFanAdj=1;
// 	FanAdj_task::FANADJ_INTERVAL=3000;
// 	FanAdj_task::VF=12; //gai
// 	FanAdj_task::VR=-2.5; //gai
// 	FanAdj_task::HeatSinkCounter=4;
    FanAdj_task::AdjMod=PWMMOD;
// 	FanAdj_task::PerAdjPower=1;  //gai
    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH6,100,20,1000);
    unsigned char FanCtrlValue[8]= {40,40,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {40,40,40,40,40,40,40,40};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
}

void PWM_Base::PWM_FeatureEnable_Init(void)
{
// 	static St_Hardware_Feature_Enable para;
// 	memset(&para,0,sizeof(para));
//
// 	if((ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C)||(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B))
// 	{
//     para.Mcu_PWMFan_Enable=1;
// 		para.Fan_Safety_Mode=1;
// 		para.PWM_Version_Control=1;
// 	}
// 	para.OS_Tick_Interval=200;
// 	board_config::Feature_Enable_init=&para;

    board_config::Feature_Enable_init.OS_Tick_Interval =200;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_B)
    {
        board_config::Feature_Enable_init.Fan_Safety_Mode =1;
//        board_config::Feature_Enable_init.Mcu_PWMFan_Enable =1;
        board_config::Feature_Enable_init.PWM_Version_Control =1;
    }

}

void PWM_Base::IO_Init(void)
{
    for(int i=0; i<PWM_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup[i]);
//	ControlPin::SetMeHigh(PinGroup[PWM_Pin_FanCtrl]); //added by niuyasong 2016.5.27
    ControlPin::SetMeHigh(PinGroup[PWM_Pin_AlarmLEDRed]);
    ControlPin::SetMeHigh(PinGroup[PWM_Pin_AlarmLEDGreen]);
    ControlPin::SetMeHigh(PinGroup[PWM_Pin_Buzzer]);

    if(board_config::Feature_Enable_init.PWM_Version_Control==0)
        ControlPin::SetMeLow(PinGroup[PWM_Pin_PROT1]);
    else
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_PROT1]);
}

void PWM_Base::ADC_Init(void)
{
    PWM_ADC5DAC::Adc_Init();
    TIMERSFunc::DELAY_SYS( 10000, 0);	//delay 5ms
}
void PWM_Base::DAC_Init(void)
{
    PWM_ADC5DAC::Dac_Init();

    board_config::DacInfo.m_bCurrRngCount = 3;
    board_config::DacInfo.m_bVoltRngCount = 2;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = PWM_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = PWM_RISING_TIME;//PWM Rising time 5ms, unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = PWM_Tpid_Tsys;
    ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void PWM_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();
		AdditionalEngine();
	
    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = PWM_FINE_TUNE_STARTTIME;
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = PWM_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = PWM_FINE_TUNE_MAXADJUSTFACTOR;    //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = PWM_FINE_TUNE_STEPFACTOR;
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
	   Step_Manager_task::m_CurFineTuneFactor.m_fParaWaitTime =0.7296;
	else
		Step_Manager_task::m_CurFineTuneFactor.m_fParaWaitTime =0.0384;

    IR::Set_LowPulseDacTime=PWMSETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=PWMSETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=PWMSETLOWPULSEDACTIME;//PWMSETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=PWMSETLOWPULSEADCTIME;//PWMSETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=PWMMINIMUMPLSE;

//	safety_check::m_CtrlValueSetting.fErrorFactor       = PWM_ACCURACY * PWM_CTRL_VALUE_ERROR_FACTOR; //180614 zyx

    board_config::Accuracy = PWM_ACCURACY;
    IR::pIR_ADC = &(PWM_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(PWM_SampleManage::renew_IR_ADC);
// 	  IR::pIR_Trigger_ADC = &(PWM_SampleManage::IR_Trigger_ADC);
//	  IR::IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(PWM_SampleManage::IR_Read_ADC);

    PWM_SampleManage::Init();
}

void PWM_Base::Func_Init(void)
{
    //     board_config::Func_Adc_rd=&PWM_Base::ADC_rd;
    board_config::Func_Dac_out=&PWM_Base::DAC_out;
    board_config::Func_Sample = &PWM_SampleManage::SampleISR;

    board_config::Func_SetRelay =&PWM_Base::SetRelay;
    board_config::Func_SetRange = &PWM_Base::SetRange;
    //  	board_config::Func_TempSample = &PWM_TempSample::Temp_rd;   // 20140613  温度测量
    board_config::Func_FanSafety =&PWM_Base::FanCheckSafety; //风扇保护
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
        board_config::Func_FanAdj=&PWM_Base::FanAdjOperate;

    board_config::FeedWatchDog = &PWM_Base::Feed_CpldWD;
    board_config::Func_CheckError = &PWM_Base::ErrorCheck;     //add Wzh
}

void PWM_Base::Engine(void)
{
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &PWM_Base::SetCtrl_RstToCur[0];
    OS_LogicProcess::SetCtrlRstToCur.stepLen = sizeof(SetCtrl_RstToCur)/sizeof(St_DisposableTask);  //PWM_Base::Length_RstToCur;

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &PWM_Base::SetCtrl_RstToVlt[0];
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetCtrl_RstToVlt)/sizeof(St_DisposableTask); //PWM_Base::Length_RstToVlt;

    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &PWM_Base::SetCtrl_RstToOff[0];
    OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetCtrl_RstToOff)/sizeof(St_DisposableTask);//PWM_Base::Length_RstToOff;

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &PWM_Base::SetCtrl_OffToRst[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetCtrl_OffToRst)/sizeof(St_DisposableTask);//PWM_Base::Length_OffToRst;

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &PWM_Base::SetCtrl_OffToCur[0];    //指向数组0
    OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetCtrl_OffToCur)/sizeof(St_DisposableTask);//PWM_Base::Length_OffToCur;

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &PWM_Base::SetCtrl_OffToVlt[0];
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetCtrl_OffToVlt)/sizeof(St_DisposableTask);//PWM_Base::Length_OffToVlt;

    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &PWM_Base::SetCtrl_CurToCur[0];
    OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCtrl_CurToCur)/sizeof(St_DisposableTask);//PWM_Base::Length_CurToCur;

    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &PWM_Base::SetCtrl_CurToVlt[0];
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCtrl_CurToVlt)/sizeof(St_DisposableTask);//PWM_Base::Length_CurToVlt;

    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &PWM_Base::SetCtrl_CurToOff[0];
    OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCtrl_CurToOff)/sizeof(St_DisposableTask);//PWM_Base::Length_CurToOff;

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &PWM_Base::SetCtrl_CurToRst[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCtrl_CurToRst)/sizeof(St_DisposableTask);//PWM_Base::Length_CurToRst;

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &PWM_Base::SetCtrl_VltToVlt[0];
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetCtrl_VltToVlt)/sizeof(St_DisposableTask);//PWM_Base::Length_VltToVlt;

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &PWM_Base::SetCtrl_VltToCur[0];
    OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetCtrl_VltToCur)/sizeof(St_DisposableTask);//PWM_Base::Length_VltToCur;

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &PWM_Base::SetCtrl_VltToOff[0];
    OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetCtrl_VltToOff)/sizeof(St_DisposableTask);//PWM_Base::Length_VltToOff;

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &PWM_Base::SetCtrl_VltToRst[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetCtrl_VltToRst)/sizeof(St_DisposableTask);//PWM_Base::Length_VltToRst;
}
void PWM_Base::EngineStuff(void)
{
    memset(&SetCtrl_RstToCur,      0,    sizeof(SetCtrl_RstToCur));
    memset(&SetCtrl_RstToVlt,      0,    sizeof(SetCtrl_RstToVlt));
    memset(&SetCtrl_RstToOff,      0,    sizeof(SetCtrl_RstToOff));
    memset(&SetCtrl_OffToRst,      0,    sizeof(SetCtrl_OffToRst));
    memset(&SetCtrl_OffToCur,      0,    sizeof(SetCtrl_OffToCur));
    memset(&SetCtrl_OffToVlt,      0,    sizeof(SetCtrl_OffToVlt));
    memset(&SetCtrl_CurToCur,      0,    sizeof(SetCtrl_CurToCur));
    memset(&SetCtrl_CurToVlt,      0,    sizeof(SetCtrl_CurToVlt));
    memset(&SetCtrl_CurToOff,      0,    sizeof(SetCtrl_CurToOff));
    memset(&SetCtrl_CurToRst,      0,    sizeof(SetCtrl_CurToRst));
    memset(&SetCtrl_VltToVlt,      0,    sizeof(SetCtrl_VltToVlt));
    memset(&SetCtrl_VltToCur,      0,    sizeof(SetCtrl_VltToCur));
    memset(&SetCtrl_VltToOff,      0,    sizeof(SetCtrl_VltToOff));
    memset(&SetCtrl_VltToRst,      0,    sizeof(SetCtrl_VltToRst));
	  memset(&SetCtrlOpenAcim,			 0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,			 0,    sizeof(SetCtrlCloseAcim));
	
    //SetCtrl_RstToCur[0].uw32_DelayTime = 0;
    SetCtrl_RstToCur[0].bPWM = 1;
    SetCtrl_RstToCur[0].PWM_TransitionType = eTransitionType_RstToCur;

    //SetCtrl_RstToVlt[0].uw32_DelayTime = 0;
    SetCtrl_RstToVlt[0].bPWM = 1;
    SetCtrl_RstToVlt[0].PWM_TransitionType = eTransitionType_RstToVlt;

    //SetCtrl_RstToOff[0].uw32_DelayTime = 0;
    SetCtrl_RstToOff[0].bPWM = 1;
    SetCtrl_RstToOff[0].PWM_TransitionType = eTransitionType_RstToOff;

    //SetCtrl_OffToRst[0].uw32_DelayTime = 0;
    SetCtrl_OffToRst[0].bPWM = 1;
    SetCtrl_OffToRst[0].PWM_TransitionType = eTransitionType_OffToRst;

    //SetCtrl_OffToCur[0].uw32_DelayTime = 0;
    SetCtrl_OffToCur[0].bPWM = 1;
    SetCtrl_OffToCur[0].PWM_TransitionType = eTransitionType_OffToCur;

    //SetCtrl_OffToVlt[0].uw32_DelayTime = 0;
    SetCtrl_OffToVlt[0].bPWM = 1;
    SetCtrl_OffToVlt[0].PWM_TransitionType = eTransitionType_OffToVlt;

    //SetCtrl_CurToCur[0].uw32_DelayTime = 0;
    SetCtrl_CurToCur[0].bPWM = 1;
    SetCtrl_CurToCur[0].PWM_TransitionType = eTransitionType_CurToCur;
#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].bPWM = 1;
    SetAutoRangeSwitch[0].PWM_TransitionType = eTransitionType_CurToCur;
#endif
    //SetCtrl_CurToVlt[0].uw32_DelayTime = 0;
    SetCtrl_CurToVlt[0].bPWM = 1;
    SetCtrl_CurToVlt[0].PWM_TransitionType = eTransitionType_CurToVlt;

    //SetCtrl_CurToOff[0].uw32_DelayTime = 0;
    SetCtrl_CurToOff[0].bPWM = 1;
    SetCtrl_CurToOff[0].PWM_TransitionType = eTransitionType_CurToOff;

    //SetCtrl_CurToRst[0].uw32_DelayTime = 0;
    SetCtrl_CurToRst[0].bPWM = 1;
    SetCtrl_CurToRst[0].PWM_TransitionType = eTransitionType_CurToRst;

    //SetCtrl_VltToVlt[0].uw32_DelayTime = 0;
    SetCtrl_VltToVlt[0].bPWM = 1;
    SetCtrl_VltToVlt[0].PWM_TransitionType = eTransitionType_VltToVlt;

    //SetCtrl_VltToCur[0].uw32_DelayTime = 0;
    SetCtrl_VltToCur[0].bPWM = 1;
    SetCtrl_VltToCur[0].PWM_TransitionType = eTransitionType_VltToCur;

    //SetCtrl_VltToOff[0].uw32_DelayTime = 0;
    SetCtrl_VltToOff[0].bPWM = 1;
    SetCtrl_VltToOff[0].PWM_TransitionType = eTransitionType_VltToOff;

    //SetCtrl_VltToRst[0].uw32_DelayTime = 0;
    SetCtrl_VltToRst[0].bPWM = 1;
    SetCtrl_VltToRst[0].PWM_TransitionType = eTransitionType_VltToRst;
		
	  SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].uw32_DelayTime = 100;		
}

//char PWM_Base::ADC_rd(unsigned long CH_Number,unsigned long type)
//{
//    return 0;
//}
char PWM_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    SSPIFunc::ConfirmDevice(0, &PWM_Base::PWM_SSP0DAC);

    switch(type)
    {
    case ACTIVE_DAC_CUR:
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
	  if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_E)
		{
		DAC16bit_AD5764::Dac_Out(0,Data);
		}
		else
		DAC16bit_AD5754::Dac_Out(0,Data);		
		break;
	case ACTIVE_DAC_VLT:
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_E)
		{
		DAC16bit_AD5764::Dac_Out(1,Data);
		}
		else
		DAC16bit_AD5754::Dac_Out(1,Data);	
		break;
	case ACTIVE_DAC_VH:
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_E)
		{
		DAC16bit_AD5764::Dac_Out(2,Data);
		}
		else
		DAC16bit_AD5754::Dac_Out(2,Data);	
		break;
	case ACTIVE_DAC_VL:
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_E)
		{
		DAC16bit_AD5764::Dac_Out(3,Data);
		}
		else
		DAC16bit_AD5754::Dac_Out(3,Data);	
		break;
    default:
        break;
    }
    return 1;
}

void PWM_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
		St_Disposable_G_Que * p_RunningTask = &OS_LogicProcess::m_RunningTask[ChannelIdx];
    PWM_relaystatus[ChannelIdx].Reg |= SetRly;
    PWM_relaystatus[ChannelIdx].Reg &= ~ClrRly;

    if(PWM_relaystatus[ChannelIdx].Bits.PROT)
    {
//#if ( _PWM_VERSION_ ==0)
        if(board_config::Feature_Enable_init.PWM_Version_Control==0)
            ControlPin::SetMeHigh(PinGroup[PWM_Pin_PROT1]);
//#else
        else
            ControlPin::SetMeLow(PinGroup[PWM_Pin_PROT1]);
//#endif
    }
    else
    {
//#if ( _PWM_VERSION_ ==0)
        if(board_config::Feature_Enable_init.PWM_Version_Control==0)
            ControlPin::SetMeLow(PinGroup[PWM_Pin_PROT1]);
//#else
        else
            ControlPin::SetMeHigh(PinGroup[PWM_Pin_PROT1]);
//#endif
	}
	if(PWM_relaystatus[ChannelIdx].Bits.EXT)
	{
		if(p_RunningTask->logicProcessMSg.next_Range==CURRENT_RANGE_1 )
			{
				ControlPin::SetMeLow(PinGroup[PWM_Pin_EXT1]);
			}
			else
			{
				ControlPin::SetMeHigh(PinGroup[PWM_Pin_EXT1]);
			}
	}
	else
	{
		ControlPin::SetMeLow(PinGroup[PWM_Pin_EXT1]);

    }

    if(PWM_relaystatus[ChannelIdx].Bits.UNLOCK)
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_UNLOCK1]);
    else
        ControlPin::SetMeLow(PinGroup[PWM_Pin_UNLOCK1]);

    if(PWM_relaystatus[ChannelIdx].Bits.IVS)
    {
//#if ( _PWM_VERSION_ ==0)
        if(board_config::Feature_Enable_init.PWM_Version_Control==0)
            ControlPin::SetMeHigh(PinGroup[PWM_Pin_IVS1]);
//#else
        else
            ControlPin::SetMeLow(PinGroup[PWM_Pin_IVS1]);
//#endif
    }
    else
    {
//#if ( _PWM_VERSION_ ==0)
        if(board_config::Feature_Enable_init.PWM_Version_Control==0)
            ControlPin::SetMeLow(PinGroup[PWM_Pin_IVS1]);
//#else
        else
            ControlPin::SetMeHigh(PinGroup[PWM_Pin_IVS1]);
//#endif
    }

    if(PWM_relaystatus[ChannelIdx].Bits.VRL)
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_VRL1]);
    else
        ControlPin::SetMeLow(PinGroup[PWM_Pin_VRL1]);

    if(PWM_relaystatus[ChannelIdx].Bits.IRH)
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRH1]);
    else
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRH1]);

    if(PWM_relaystatus[ChannelIdx].Bits.IRM)
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRM1]);
    else
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRM1]);

    if(PWM_relaystatus[ChannelIdx].Bits.IRL)
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRL1]);
    else
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRL1]);
}
char PWM_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
    return 1;
}
void PWM_Base::SetCurrRange(unsigned char ChannelIdx,unsigned char range)
{
    switch(range)
    {
    case CURRENT_RANGE_1:
        PWM_relaystatus[ChannelIdx].Bits.IRH = 1;
        PWM_relaystatus[ChannelIdx].Bits.IRM = 0;
        PWM_relaystatus[ChannelIdx].Bits.IRL = 0;
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRH1]);
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRM1]);
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRL1]);
        break;
    case CURRENT_RANGE_2:
        PWM_relaystatus[ChannelIdx].Bits.IRH = 0;
        PWM_relaystatus[ChannelIdx].Bits.IRM = 1;
        PWM_relaystatus[ChannelIdx].Bits.IRL = 0;
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRH1]);
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRM1]);
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRL1]);
        break;
    case CURRENT_RANGE_3:
    default:
        PWM_relaystatus[ChannelIdx].Bits.IRH = 0;
        PWM_relaystatus[ChannelIdx].Bits.IRM = 0;
        PWM_relaystatus[ChannelIdx].Bits.IRL = 1;
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRH1]);
        ControlPin::SetMeLow(PinGroup[PWM_Pin_IRM1]);
        ControlPin::SetMeHigh(PinGroup[PWM_Pin_IRL1]);
        break;
    }
}
void PWM_Base::SetVoltRange(unsigned char ChannelIdx,unsigned char range)
{
    switch(range)
    {
    case VOLTAGE_RANGE_LOW:
        PWM_relaystatus[ChannelIdx].Bits.VRL = 1;
        if(ChannelIdx == 0)
            ControlPin::SetMeHigh(PinGroup[PWM_Pin_VRL1]);
        break;
    case VOLTAGE_RANGE_HIGH:
    case VOLTAGE_RANGE_MED:
        PWM_relaystatus[ChannelIdx].Bits.VRL = 0;
        ControlPin::SetMeLow(PinGroup[PWM_Pin_VRL1]);
        break;
    default:
        PWM_relaystatus[ChannelIdx].Bits.VRL = 0;
        if(ChannelIdx == 0)
            ControlPin::SetMeLow(PinGroup[PWM_Pin_VRL1]);
        break;
    }
}

void PWM_Base::Feed_CpldWD(void)
{
    ControlPin::SetMeHigh(PinGroup[PWM_Pin_WD]);
    TIMERSFunc::DELAY_SYS(200, 0);
    ControlPin::SetMeLow(PinGroup[PWM_Pin_WD]);
}


unsigned char PWM_Base::DoTransition(unsigned char ChannelIdx,St_DisposableMSG *Task)
{
    St_TranTask *pTranTask;

    St_TransitionInfo * p_TransInfo = &TransitionInfo[ChannelIdx];
    if(p_TransInfo->bStepInited)
    {
        //Do Steps;
        pTranTask = &p_TransInfo->StepInfo[p_TransInfo->StepIdx];
        if(!p_TransInfo->bActionDone)
        {
            DoStep(ChannelIdx,Task);
            CommonFunc::GetTimeTicksUpdate(&p_TransInfo->StartTime);
            // p_TransInfo->StartTime = OS_ServeProcess::OS_Time;
        }
        else
        {   //handle delay time
            //MsTime TimePassed;
            //TimePassed = OS_ServeProcess::TimeDiff(p_TransInfo->StartTime);
            unsigned long long TimePassed = CommonFunc::GetTimeTicks(&p_TransInfo->StartTime);
            if(TimePassed >= (pTranTask->DelayTime * 10)) //Transition Time will not exceed 1 second,so we use just Us100 for comparison
            {
                p_TransInfo->StepIdx++;
                p_TransInfo->bActionDone = 0;

                if(p_TransInfo->StepIdx >= p_TransInfo->StepCount)
                {
                    p_TransInfo->bStepInited = 0;
#if(_DEBUG_TIMING_ == 1)
                    ControlPin::SetMeLow(Pin_EXT1);
#endif
                    return 1; //finished all step;
                }
            }
        }
    }
    else
    {
#if(_DEBUG_TIMING_ == 1)
        ControlPin::SetMeHigh(Pin_EXT1);
#endif
        pTranTask = &p_TransInfo->StepInfo[0];
        memset(&TransitionInfo[ChannelIdx],0,sizeof(TransitionInfo[ChannelIdx]));
        unsigned char TransitionType = Task->Disposable_task.st_Disposabletask_ptr->PWM_TransitionType;
        p_TransInfo->bStepInited = 1;
        //p_TransInfo->StartTime = OS_ServeProcess::OS_Time;
        CommonFunc::GetTimeTicksUpdate(&p_TransInfo->StartTime);

        switch(TransitionType)
        {
        case eTransitionType_RstToCur:
        case eTransitionType_OffToCur:
            DoTurnChannelOn(ChannelIdx,Task->next_Range,Task->next_VRange,true);
            break;
        case eTransitionType_RstToVlt:
        case eTransitionType_OffToVlt:
            DoTurnChannelOn(ChannelIdx,Task->next_Range,Task->next_VRange,false);
            break;
        case eTransitionType_CurToCur:
            if(Task->next_Range != Task->last_Range)
                DoChangeCurrentRange(ChannelIdx,Task->next_Range,Task->next_VRange,true);
            else
            {
                pTranTask->bSetCurrDacActive = 1;
                pTranTask->bSetCVHdacActive  = 1;
                pTranTask->bSetCVLdacActive  = 1;
                pTranTask->DelayTime = 10;
                pTranTask++;
                p_TransInfo->StepCount++;
            }
            break;
        case eTransitionType_CurToVlt:
            if(Task->next_Range != Task->last_Range)
                DoChangeCurrentRange(ChannelIdx,Task->next_Range,Task->next_VRange,false);
            else
            {
                pTranTask->bSetVoltDacActive = 1;
                pTranTask->DelayTime = 10;
                pTranTask++;
                p_TransInfo->StepCount++;

                pTranTask->Rly_Set |= PWM_RLY_Vctrl;
                pTranTask->bSetVoltDacActive = 1;
                pTranTask->DelayTime = 10;
                pTranTask++;
                p_TransInfo->StepCount++;
            }
            break;
        case eTransitionType_CurToOff:
        case eTransitionType_CurToRst:
            DoTurnChannelOff(ChannelIdx,Task->next_Range,Task->next_VRange,true);
            break;
        case eTransitionType_VltToVlt:
            if(Task->next_Range != Task->last_Range)
                DoChangeCurrentRange(ChannelIdx,Task->next_Range,Task->next_VRange,false);
            else
            {
                pTranTask->bSetVoltDacActive = 1;
                pTranTask->DelayTime = 10;
                pTranTask++;
                p_TransInfo->StepCount++;
            }
            break;
        case eTransitionType_VltToCur:
            if(Task->next_Range != Task->last_Range)
                DoChangeCurrentRange(ChannelIdx,Task->next_Range,Task->next_VRange,true);
            else
            {
                pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
                pTranTask->bSetCurrDacActive = 1;
                pTranTask->bSetCVHdacActive  = 1;
                pTranTask->bSetCVLdacActive  = 1;
                pTranTask->DelayTime = 10;
                pTranTask++;
                p_TransInfo->StepCount++;
            }
            break;

        case eTransitionType_VltToOff:
        case eTransitionType_VltToRst:
            DoTurnChannelOff(ChannelIdx,Task->next_Range,Task->next_VRange,false);
            break;
        case eTransitionType_OffToRst:
            DoOff2Rest(ChannelIdx);
            break;
        case eTransitionType_RstToOff:
            DoRest2Off(ChannelIdx);
            break;
        case eTransitionType_NULL:
            TransitionInfo[ChannelIdx].bStepInited = 0;    //20140527,add by hepeiqing,
            //Do nothing,clear bStepInited.
            return 1;   //DONE
//                 break;
        default:
            p_TransInfo->bStepInited = 0;    //20140527,add by hepeiqing,
            //Do nothing,clear bStepInited.
            return 1;   //DONE
            //                break;
        }
    }

    return 0;  //not finished
}

void PWM_Base::DoStep(unsigned char ChannelIdx,St_DisposableMSG *Task)
{
    St_TranTask *pTranTask = &TransitionInfo[ChannelIdx].StepInfo[TransitionInfo[ChannelIdx].StepIdx];

    TransitionInfo[ChannelIdx].bActionDone = 0;

    if(pTranTask->bSetCurrDacActive)
        DAC_out(ChannelIdx,Task->Dac_Value,ACTIVE_DAC_CUR);
    if(pTranTask->bSetCurrDac2Zero)
        DAC_out(ChannelIdx,0x800000,ACTIVE_DAC_CUR);
    if(pTranTask->bSetVoltDac2Zero)
        DAC_out(ChannelIdx,0x800000,ACTIVE_DAC_VLT);
    if(pTranTask->bSetCurrDac2NegtiveMax)
    {
#if(_DAC_RAMP_NEGTIVE_ == 1)
        DAC_out(ChannelIdx,0xFFFFFF,ACTIVE_DAC_CUR);
#else
        DAC_out(ChannelIdx,0,ACTIVE_DAC_CUR);
#endif
    }
    if(pTranTask->bSetCurrDac2PositiveMax)
    {
#if(_DAC_RAMP_NEGTIVE_ == 1)
        DAC_out(ChannelIdx,0,ACTIVE_DAC_CUR);
#else
        DAC_out(ChannelIdx,0xFFFFFF,ACTIVE_DAC_CUR);
#endif
    }
    if(pTranTask->bSetDacValue)
        DAC_out(ChannelIdx,pTranTask->DacValue,ACTIVE_DAC_CUR);

    if(pTranTask->bSetVoltDacActive)
        DAC_out(ChannelIdx,Task->Volt_DacValue,ACTIVE_DAC_VLT);

    if(pTranTask->bSetCVHdacActive)
        DAC_out(ChannelIdx,Task->Clamp_Value,ACTIVE_DAC_VH);
    if(pTranTask->bSetCVLdacActive)
        DAC_out(ChannelIdx,Task->Clamp_Value_2,ACTIVE_DAC_VL);
	
	if(pTranTask->bSetCVHdacInactive)
		DAC_out(ChannelIdx,0xFFFFFF,ACTIVE_DAC_VH);
	if(pTranTask->bSetCVLdacInactive)
		DAC_out(ChannelIdx,0,ACTIVE_DAC_VL);

    if(pTranTask->bSetDischargeResidual)             //Add DKQ 02.04.2015
        DAC_out(ChannelIdx,0x999900,ACTIVE_DAC_CUR);  //Add DKQ 02.04.2015  to discharge residual in the capactors

    if(pTranTask->Rly_Clr || pTranTask->Rly_Set)
        SetRelay(ChannelIdx,pTranTask->Rly_Set,pTranTask->Rly_Clr,0);

    if(pTranTask->bSetCurrRangeActive)
        SetCurrRange(ChannelIdx,Task->next_Range);

    if(pTranTask->bSetVoltRangeActive)
        SetVoltRange(ChannelIdx,Task->next_VRange);

    if(pTranTask->bSetDacRamp)
    {
        if(!TransitionInfo[ChannelIdx].m_Ramp.bStarted)
        {
            TransitionInfo[ChannelIdx].m_Ramp.bStarted  = 1;
            CommonFunc::GetTimeTicksUpdate(&TransitionInfo[ChannelIdx].m_Ramp.StartTime);
            //TransitionInfo[ChannelIdx].m_Ramp.StartTime = OS_ServeProcess::OS_Time;
        }

        // MsTime TimePassed;
        //TimePassed = OS_ServeProcess::TimeDiff(TransitionInfo[ChannelIdx].m_Ramp.StartTime);
        unsigned long TimeUS100 = CommonFunc::GetTimeTicks(&TransitionInfo[ChannelIdx].m_Ramp.StartTime);//TimePassed.Second * 10000 + TimePassed.Us100;
        float ScanRate = 0x800000 / TURN_ON_RAMP_TIME / 10;
        unsigned long DacValue = 0;
#if _DAC_RAMP_NEGTIVE_ == 1
        DacValue = 0xFFFFFF - ScanRate*TimeUS100;
        if(DacValue < 0x800000)
            DacValue = 0x800000;
#else
        DacValue += ScanRate*TimeUS100;
        if(DacValue > 0x800000)
            DacValue = 0x800000;
#endif
        board_config::DAC_out(ChannelIdx,DacValue,ACTIVE_DAC_CUR);    //output 0

        if(TimeUS100 >= TURN_ON_RAMP_TIME * 10)
            TransitionInfo[ChannelIdx].bActionDone = 1;
        return;
    }
    TransitionInfo[ChannelIdx].bActionDone = 1;
    return;
}

void PWM_Base::DoChangeCurrentRange(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl)
{
    St_TranTask *pTranTask = &TransitionInfo[ChannelIdx].StepInfo[TransitionInfo[ChannelIdx].StepIdx];
    St_TransitionInfo * pTranInfo = &TransitionInfo[ChannelIdx];

    if(CurrRange == CURRENT_RANGE_1)  // 02-06-2006 per Tony
    {
        pTranTask->Rly_Clr |= PWM_RLY_Vctrl;  //current:Vctrl == low;  voltage:Vctrl == high
        pTranTask->bSetCurrDac2Zero = 1;
        pTranTask->DelayTime = 20;
        pTranTask++;                    //MovePointer to next step
        pTranInfo->StepCount++;         //Step count++, a new step added

        pTranTask->Rly_Clr |= (0xFB - PWM_RLY_VL);
        pTranTask->DelayTime = 20;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->Rly_Set |= PWM_RLY_Protect;
        pTranTask->DelayTime = 20;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->bSetCurrRangeActive = 1;
        pTranTask->bSetVoltRangeActive = 1;
        pTranTask->DelayTime = 100;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->Rly_Clr |= PWM_RLY_Protect;
        if(bCurrCtrl)
            pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
        else
            pTranTask->Rly_Set |= PWM_RLY_Vctrl;
        pTranTask->bSetCVHdacActive = 1;
        pTranTask->bSetCVLdacActive = 1;
        pTranTask->DelayTime = 10;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->Rly_Set |= PWM_RLY_Unlock;
        pTranTask->DelayTime = 40;
        pTranTask++;
        pTranInfo->StepCount++;

        if(bCurrCtrl)
            pTranTask->bSetCurrDacActive = 1;
        else
            pTranTask->bSetVoltDacActive = 1;
        pTranTask->DelayTime = 10;
        pTranTask++;
        pTranInfo->StepCount++;
    }
    else
    {
        pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
        pTranTask->bSetCurrDac2Zero = 1;           // per Mingli's request, 2000-07-03
        pTranTask->DelayTime = 20;
        pTranTask++;
        pTranInfo->StepCount++;

        if(CurrRange == CURRENT_RANGE_3)
            pTranTask->Rly_Set |= PWM_RLY_IL;   // 09-28-02 T.J./ H.H
        else if(CurrRange == CURRENT_RANGE_2)
            pTranTask->Rly_Set |= PWM_RLY_IM;
        else
            pTranTask->Rly_Set |= PWM_RLY_IH;
        pTranTask->DelayTime = 100;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->bSetCurrRangeActive = 1;
        pTranTask->bSetVoltRangeActive = 1;  // 7.27.2007
        pTranTask->DelayTime = 150;
        pTranTask++;
        pTranInfo->StepCount++;

        if(bCurrCtrl)
            pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
        else
            pTranTask->Rly_Set |= PWM_RLY_Vctrl;
        pTranTask->bSetCVHdacActive = 1;
        pTranTask->bSetCVLdacActive = 1;
        pTranTask->DelayTime = 10;
        pTranTask++;
        pTranInfo->StepCount++;

        pTranTask->Rly_Set |= PWM_RLY_Unlock;
        if(bCurrCtrl)
            pTranTask->bSetCurrDacActive = 1;
        else
            pTranTask->bSetVoltDacActive = 1;
        pTranTask->DelayTime = 10;      //Henry 09-03-00
        pTranTask++;
        pTranInfo->StepCount++;
    }
}
void PWM_Base::DoTurnChannelOn(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl)
{
    St_TranTask *pTranTask = &TransitionInfo[ChannelIdx].StepInfo[TransitionInfo[ChannelIdx].StepIdx];
    St_TransitionInfo * pTranInfo = &TransitionInfo[ChannelIdx];

    pTranTask->bSetVoltRangeActive = 1;
    pTranTask->DelayTime = 10;
    pTranTask++;
    pTranInfo->StepCount++;

	pTranTask->Rly_Clr |= (0xFF - PWM_RLY_VL-PWM_RLY_Ext);  //ZeroOutSwitch
	pTranTask->Rly_Set |= PWM_RLY_Ext;
    pTranTask->DelayTime = 20;
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->Rly_Clr |= PWM_RLY_Unlock;
    pTranTask->bSetCurrDac2NegtiveMax = 1;
    pTranTask->Rly_Set |= PWM_RLY_Protect;
    pTranTask->DelayTime = 40;
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->Rly_Set |= PWM_RLY_Unlock;
    pTranTask->bSetDacRamp = 1;
    //pTranTask->DelayTime = 200;
    pTranTask->DelayTime = 300;      //add by DKQ 02.02.2015
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->bSetCurrRangeActive = 1;
    pTranTask->bSetVoltRangeActive = 1;    //  7.27.2007
    pTranTask->DelayTime = 150;            //09-28-02 T.J. / H.H //Modify 500->150, 03/04/03 per MingLi
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->bSetCurrDac2Zero = 1;           //09-29-02 MingLi / H.H
    pTranTask->Rly_Clr |= PWM_RLY_Protect;
    if(bCurrCtrl)
        pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
    else
        pTranTask->Rly_Set |= PWM_RLY_Vctrl;
    pTranTask->bSetCVHdacActive = 1;
    pTranTask->bSetCVLdacActive = 1;
    pTranTask->DelayTime = 10;
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->Rly_Set |= PWM_RLY_Unlock;
    pTranTask->DelayTime = 40;    //move to here 02.19.2010
    pTranTask++;
    pTranInfo->StepCount++;

    if(bCurrCtrl)
        pTranTask->bSetCurrDacActive = 1;
    else
        pTranTask->bSetVoltDacActive = 1;
    pTranTask->DelayTime = 10;
    pTranTask++;
    pTranInfo->StepCount++;
}
void PWM_Base::DoTurnChannelOff(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl)
{
    St_TranTask *pTranTask = &TransitionInfo[ChannelIdx].StepInfo[TransitionInfo[ChannelIdx].StepIdx];
    St_TransitionInfo * pTranInfo = &TransitionInfo[ChannelIdx];

    pTranTask->bSetCurrDac2Zero = 1;
	//============TEST CODE 08.21.2019====================
	pTranTask->bSetCVHdacInactive = 1;
	pTranTask->bSetCVLdacInactive = 1;
	//================TEST CODE END=======================
    pTranTask->DelayTime = 10;
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->Rly_Clr |= PWM_RLY_Vctrl;
    pTranTask->bSetCurrDac2Zero = 1;
    //pTranTask->DelayTime = 400;   //change from 200 to 400 for Huawei PWM 02.26.2016  //200;
    pTranTask->DelayTime = 50;   // 02_20_2018 changed from 400 to 50 for SK
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->Rly_Clr |= (0XFF - PWM_RLY_Unlock - PWM_RLY_VL);
    pTranTask->DelayTime = 50; // Change from 20 to 50 for Huawei PWM  02.26.2016;  //
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask-> bSetDischargeResidual = 1;   // DKQ 02.03.2015
    pTranTask->DelayTime = 500;//07.15.2009  11.10.2009 add
    pTranTask++;
    pTranInfo->StepCount++;

    pTranTask->bSetVoltRangeActive = 1;
    pTranTask->Rly_Clr |= PWM_RLY_Unlock;
    pTranTask->bSetCurrDac2Zero = 1;//07.15.2009  11.10.2009 add
    pTranTask->bSetVoltDac2Zero=1;
    pTranTask->DelayTime = 20;
    pTranTask++;
    pTranInfo->StepCount++;
}
void PWM_Base::DoClose2Close(unsigned char ChannelIdx)
{
    St_TranTask *pTranTask = &TransitionInfo[ChannelIdx].StepInfo[TransitionInfo[ChannelIdx].StepIdx];
    St_TransitionInfo * pTranInfo = &TransitionInfo[ChannelIdx];

    pTranTask->bSetVoltRangeActive = 1;
    pTranTask->DelayTime = 10;
    pTranTask++;
    pTranInfo->StepCount++;
}

void PWM_Base::DoRest2Off(unsigned char ChannelIdx)
{
    DoClose2Close(ChannelIdx);
}
void PWM_Base::DoOff2Rest(unsigned char ChannelIdx)
{
    DoClose2Close(ChannelIdx);
}

void PWM_Base::AdditionalEngine(void)
{
	// ACIM Engine
	OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &PWM_Base::SetCtrlOpenAcim[0];
	OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);
	
	OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &PWM_Base::SetCtrlCloseAcim[0];
	OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}

//add Wzh
char PWM_Base::ErrorCheck(unsigned long ChannelIdx,unsigned long Para)
{
//#if ( _PWM_VERSION_ ==0)
    if(board_config::Feature_Enable_init.PWM_Version_Control==0)
    {
        switch(m_Error.State)
        {
        case ErrorSM_Idle:
            m_Error.Code = 0;
            m_Error.Counter++;
            if(m_Error.Counter > 3750) //delay 3s;
            {
                m_Error.Counter = 0;
                m_Error.State   = ErrorSM_Normal;
                CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
                //m_Error.StartTime = OS_ServeProcess::OS_Time;  //DKQ  01.27.2015
            }
            break;
        case ErrorSM_Normal:
            if(ControlPin::ReadMe(PinGroup[PWM_Pin_ERR]))  //Error = Pin_ERR_Low
            {   //no error
                if(m_Error.Counter >=2)  //Add by DKQ 1.27.2015
                    m_Error.Counter -= 2;
                m_Error.CheckCounter ++;
            }
            else
            {   //error occur
                m_Error.Counter ++;
                m_Error.CheckCounter ++;
                if(m_Error.Counter >= 32760 && m_Error.CheckCounter >= 32760)  //2015.1.29 by hepeiqing,32768 stand for -1
                {
                    m_Error.Counter = 32760;
                    m_Error.CheckCounter = 32760;
                }
            }
            TimePassed = CommonFunc::GetTimeTicks(&m_Error.StartTime);//OS_ServeProcess::TimeDiff(m_Error.StartTime);
            if(TimePassed > 3000) //300ms
            {
                if(m_Error.Counter >= 0.8*m_Error.CheckCounter)
                {
#if _LOCK_ERROR_ == 1     //lock the error state when Error occurred,and never resume to normal other than a MCU reset and Hardware is no ERR.
                    m_Error.State = ErrorSM_Error;
                    break;
#endif
                    CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
                    //m_Error.StartTime = OS_ServeProcess::OS_Time;
                    m_Error.Counter = 0;          //DKQ
                    m_Error.CheckCounter = 0;       //DKQ
                    m_Error.Code = 1;  //error happened :: normal
                }
                else
                {
                    m_Error.Code = 0;  //no error happened;
                    m_Error.Counter = 0;
                    m_Error.CheckCounter = 0;
                    CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
                    //m_Error.StartTime = OS_ServeProcess::OS_Time;
                }
            }
            break;
        case ErrorSM_Error:
            CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
            //m_Error.StartTime = OS_ServeProcess::OS_Time;
            m_Error.Code = 2;            //error happened :: RST
            break;
        default:
            break;
        }
    }
//#else
    else
    {
        switch(m_Error.State)
        {
        case ErrorSM_Idle:
            m_Error.Code = 0;
            m_Error.POWERErrCounter++;
            if(m_Error.POWERErrCounter > 3750) //delay 3s;
            {
                m_Error.POWERErrCounter = 0;
                m_Error.State   = ErrorSM_Normal;
                CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
                //m_Error.StartTime = OS_ServeProcess::OS_Time;  //DKQ  01.27.2015
            }
            break;
        case ErrorSM_Normal:
            if(!ControlPin::ReadMe(PinGroup[PWM_Pin_DRIVEERR]) && !(m_Error.IGBTErrCounter >> 13))// if > 16384
            {   //error occur
                m_Error.IGBTErrCounter ++;
            }
            if (!ControlPin::ReadMe(PinGroup[PWM_Pin_OUDCERR])&& !(m_Error.DlinkErrCounter >> 13))// if > 16384
            {   //error occur
                m_Error.DlinkErrCounter ++;
            }
            if (!ControlPin::ReadMe(PinGroup[PWM_Pin_POWERERR])&& !(m_Error.POWERErrCounter >> 13))// if > 16384
            {   //error occur
                m_Error.POWERErrCounter ++;
            }
            if(!ControlPin::ReadMe(PinGroup[PWM_Pin_OTHERERR])&& !(m_Error.Inverter2FuseCounter >> 13))// if > 16384
            {   //error occur
                m_Error.Inverter2FuseCounter++;
            }

            TimePassed = CommonFunc::GetTimeTicks(&m_Error.StartTime);//OS_ServeProcess::TimeDiff(m_Error.StartTime);
            if(TimePassed > 3000) //300ms
            {
                {
                    m_Error.Code = 0;  //no error happened;
                    m_Error.ErrorTypeFlag =0;  //error happened :: normal
                }

                if(m_Error.IGBTErrCounter >=240)
                {
                    m_Error.Code = 1;
                    m_Error.ErrorTypeFlag =1;  //error happened :: normal
                }
                if(m_Error.POWERErrCounter >=240)
                {
                    m_Error.Code |= 2;
                    m_Error.ErrorTypeFlag =2;   //error happened :: abnormal
                }
                if(m_Error.DlinkErrCounter >=240)
                {
                    m_Error.Code |= 4;
                    m_Error.ErrorTypeFlag =1;  //error happened :: normal
                }
                if (m_Error.Inverter2FuseCounter >=240)
                {
                    m_Error.Code |= 8;
                    m_Error.ErrorTypeFlag =2;   //error happened :: abnormal
                }
                m_Error.IGBTErrCounter =0;
                m_Error.POWERErrCounter = 0;
                m_Error.DlinkErrCounter = 0;
                m_Error.Inverter2FuseCounter = 0;
                CommonFunc::GetTimeTicksUpdate(&m_Error.StartTime);
                //m_Error.StartTime = OS_ServeProcess::OS_Time;
            }
            break;
        default:
            m_Error.State   = ErrorSM_Normal;
            break;
        }
    }
//#endif
    return m_Error.Code;
}

char PWM_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    return (excheck::ExFanCheckSafety(PinGroup[PWM_Pin_InductanceFanCheckSignal])
            ||excheck::ExFanCheckSafety(PinGroup[PWM_Pin_BoardFanCheckSignal]));
}

char PWM_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{

    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }

// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//计算各通道满功率
// 	{
//
// 	  float I_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_High;
//
// 	  FanAdj_task::ChannelPerAdjPower[uc_i] = I_High;
// 	}
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
// 	{
// 		 float FanCtrlValue[8]={30,40,50,60,70,80,90,100};
// 			switch(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange)
// 			{
// 				case CURRENT_RANGE_3:
//             for(unsigned char uc_j=0;uc_j<8;uc_j++)
//                FanCtrlValue[uc_j] = 30;
//
// 						break;
//
// 				default:
// 						break;
// 			}
// // 			FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
//   }

//   FanAdj_task::MAX_Ratio = 30;//先设成最小
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//寻找各通道最大风扇速度
// 	{
// 		if(FanAdj_task::Channel_Duty_Ratio[uc_i]>FanAdj_task::MAX_Ratio)
// 		{
// 			FanAdj_task::MAX_Ratio=FanAdj_task::Channel_Duty_Ratio[uc_i];
//     }
// 	}

    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH6,100,DutyRatio,1000);
    //LPC4078PWM::PWM_MatchUpdate(PWM_HD1,PWM_CH6, DutyRatio, PWM_MATCH_UPDATE_NOW);
    return 1;
}
//Add Wzh end


// void PWM_Base::LEDShow(unsigned char Red_IO,unsigned char Orange_IO,unsigned char Green_IO,unsigned char BuzzSwitch)
// {
// 	if(!Red_IO)
// 		ControlPin::SetMeHigh(PinGroup[PWM_Pin_AlarmLEDRed]);
// 	else
// 		ControlPin::SetMeLow(PinGroup[PWM_Pin_AlarmLEDRed]);

// 	if(!Orange_IO)
// 		ControlPin::SetMeHigh(PinGroup[PWM_Pin_AlarmLEDOrange]);
// 	else
// 		ControlPin::SetMeLow(PinGroup[PWM_Pin_AlarmLEDOrange]);

// 	if(!Green_IO)
// 		ControlPin::SetMeHigh(PinGroup[PWM_Pin_AlarmLEDGreen]);
// 	else
// 		ControlPin::SetMeLow(PinGroup[PWM_Pin_AlarmLEDGreen]);

// 	if(!BuzzSwitch)
// 		ControlPin::SetMeHigh(PinGroup[PWM_Pin_Buzzer]);
// 	else
// 		ControlPin::SetMeLow(PinGroup[PWM_Pin_Buzzer]);
// }
/*********************************************************************************
**                            End Of File
*********************************************************************************/
