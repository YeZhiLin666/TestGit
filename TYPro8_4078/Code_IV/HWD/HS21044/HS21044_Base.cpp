#include "includes.h"

#define  HS21044_FINE_TUNE_STARTTIME          0.1f    //开始微调的时间
#define  HS21044_FINE_TUNE_MINERRORFACTOR     0.5 //3E-4    //允许电流误差最大值
#define  HS21044_FINE_TUNE_STEPFACTOR         5E-5    //每次校准的电流值
#define  HS21044_FINE_TUNE_MAXADJUSTFACTOR    0.06f //超出0.06%则不进行电流微调  // 0.03f   //  20140617 微调


#define  HS21044_ACCURACY                     0.0005f   //精度
// #define  HS21044_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  HS21044_SETTING_TIME                 50//500       //电流上升沿时间，单位为100us
#define  HS21044_RISING_TIME (50)// Rising time,  unit is 100us
#define  HS21044_PULSE_RISE_TIME              0.06f      //脉冲电流上升时间，0.06ms or 60us
#define  HS21044_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask HS21044_Base::SetRstToCur[5];
St_DisposableTask HS21044_Base::SetCurToOff[4];
St_DisposableTask HS21044_Base::SetCCToCC[7];
St_DisposableTask HS21044_Base::SetRstToOff[1];
St_DisposableTask HS21044_Base::SetRstToVlt[6];
St_DisposableTask HS21044_Base::SetCVToCV[4];
St_DisposableTask HS21044_Base::SetCCToCV[7];
St_DisposableTask HS21044_Base::SetCVToCC[7];
St_DisposableTask HS21044_Base::SetVltToOff[4];
St_DisposableTask HS21044_Base::SetCtrlOpenAcim[1];
St_DisposableTask HS21044_Base::SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask HS21044_Base::SetAutoRangeSwitch[7];
#endif
SSPI_Para         HS21044_Base::HS21044_SSP0DAC;

// bool              HS21044_Base::m_bChanOn;



#pragma arm section

// unsigned char HS21044_Base::SetRstToCur_Len = 5;  //6;
// unsigned char HS21044_Base::SetCurToOff_Len = 4;
// unsigned char HS21044_Base::SetCCToCC_Len = 6;
// unsigned char HS21044_Base::SetRstToOff_Len = 1;
// unsigned char HS21044_Base::SetRstToVlt_Len = 6;
// unsigned char HS21044_Base::SetCVToCV_Len=4;
// unsigned char HS21044_Base::SetCCToCV_Len=7;
// unsigned char HS21044_Base::SetCVToCC_Len=7;
// unsigned char HS21044_Base::SetVltToOff_Len=4;


const St_Port HS21044_Base::PinGroup[HS21044_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port      Bits       PortType           Function               Address
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
    {2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        10,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        4,         eCPTYPE_MCUGPIO,   PWM1,                  0},
    {0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//#if(FAN_SAFETY_MODE==1)
    {2,        9,         eCPTYPE_MCUGPIO,   GPIO_I,                0},  //风扇报警检测，0为有效
    {3,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {4,        29,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
//#endif
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
};

const unsigned char HS21044_Base::m_bHaveVoltageDac = 1;
const unsigned char HS21044_Base::m_bHaveClampVdac = 1;
// const unsigned char HS21044_Base::m_bChannelBasedClampVdac = 1;


void HS21044_Base::Hardware_Init(void)
{
    HS21044_FeatureEnable_Init();
    IO_Init();

    //SSP0 for DAC    AD5754
    HS21044_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    HS21044_SSP0DAC.uc_Speed = 5;
    HS21044_SSP0DAC.uc_CPOL = 0;
    HS21044_SSP0DAC.uc_CPHA = 1;
    HS21044_SSP0DAC.uc_MSTR = 1;    //master
    HS21044_SSP0DAC.uc_BITS = 8;
    HS21044_SSP0DAC.uc_LSBF = 0;

    ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();
// 	m_bChanOn = false;

    //RANGE INIT
    ControlPin::SetMeHigh(PinGroup[HS21044_Pin_AuxV]);
    HS21044_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
    Engine();  // 有电压环
    AdditionalEngine();
    //拉高钳压    // 20140613  电压保护
    HS21044_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
    HS21044_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);

    board_config::bFanAdj=1;
// 	FanAdj_task::FANADJ_INTERVAL=3000;
// 	FanAdj_task::VF=7.5;
// 	FanAdj_task::VR=-2.5;
    FanAdj_task::AdjMod=PWMMOD;
// 	FanAdj_task::PerAdjPower=1;
    //风机转速为最低
    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,30,100);
    unsigned char FanCtrlValue[8]= {50,50,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {50,50,50,50,50,50,60,60};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue1[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
    unsigned char FanCtrlValue2[8]= {50,50,50,50,50,50,50,50};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue2[0], 8);
}

void HS21044_Base::HS21044_FeatureEnable_Init(void)
{
// 	static St_Hardware_Feature_Enable para;
// 	memset(&para,0,sizeof(para));
//
// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C)
// 	{
// 		para.Power_Off_Enable=1;
// 		para.Mcu_PWMFan_Enable=1;
// 		para.Fan_Safety_Mode=1;
// 	}
// 	para.OS_Tick_Interval=200;
// 	board_config::Feature_Enable_init=&para;
    board_config::Feature_Enable_init.Fan_Safety_Mode=1;
// 	 board_config::Feature_Enable_init.Mcu_PWMFan_Enable = 1;
    board_config::Feature_Enable_init.Power_Off_Enable =1;
    board_config::Feature_Enable_init.OS_Tick_Interval =200;
// 	BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.0256;
}

void HS21044_Base::IO_Init(void)
{
    for(int i=0; i<HS21044_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup[i]);
}
void HS21044_Base::ADC_Init(void)
{
    HS21044_ADC5DAC::Adc_Init();
}
void HS21044_Base::DAC_Init(void)
{
    HS21044_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        board_config::DacInfo.m_bHaveVoltageDac = 0;
    else
        board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;

    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = HS21044_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = HS21044_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = HS21044_Tpid_Tsys;

    
    board_config::DacInfo.m_fPulseRiseTimeInMS = HS21044_PULSE_RISE_TIME;
    board_config::DacInfo.m_bHaveBuildInVolt = 1;
    ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void HS21044_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();

    // 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)
    // 	{
    // 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);
    // 		regEngine(i,board_config::st_channel_info[i].Channel_type);
    // 	}

    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = HS21044_FINE_TUNE_STARTTIME;   //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = HS21044_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = HS21044_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = HS21044_FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fParaWaitTime =0.0256;
//	safety_check::m_CtrlValueSetting.fErrorFactor       = HS21044_ACCURACY * HS21044_CTRL_VALUE_ERROR_FACTOR; //180614 zyx

    //风机报警时间清为-1，则风机不报警

    board_config::Accuracy = HS21044_ACCURACY;
    HS21044_SampleManage::Init();
    IR::pIR_ADC = &(HS21044_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(HS21044_SampleManage::renew_IR_ADC);
//	  pIR_Trigger_ADC = &(HS21044_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(HS21044_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=HS21044SETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=HS21044SETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=HS21044SETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=HS21044SETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=HS21044MINIMUMPULSE;
}

void HS21044_Base::Func_Init(void)
{
    //board_config::Func_Adc_rd=&HS21044_Base::ADC_rd;
    board_config::Func_Dac_out=&HS21044_Base::DAC_out;
    board_config::Func_SetRange=&HS21044_Base::SetRange;
    board_config::Func_SetRelay=&HS21044_Base::SetRelay;
    board_config::Func_Sample =&HS21044_SampleManage::SampleISR;    //timer0 中断

    board_config::FeedWatchDog = &HS21044_Base::Feed_CpldWD;

//#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
        board_config::Func_FanSafety= &HS21044_Base::FanCheckSafety; //风扇保护
//#endif

    board_config::Func_FanAdj=&HS21044_Base::FanAdjOperate;
    board_config::Func_BuildInVolt_ReadAll = &ReadAllBuildInVolt;

    PowerLostCheck::Func_PowerOffSafetCheck = &HS21044_Base::PowerOffCheck;
}

void HS21044_Base::Engine(void)
{   // 有电压环
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &HS21044_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlRstToCur.stepLen =  sizeof(SetRstToCur) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToCur_Len;

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &HS21044_Base::SetRstToVlt[0];
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToVlt) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToVlt_Len;


    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &HS21044_Base::SetRstToOff[0];
    OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToOff_Len;

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &HS21044_Base::SetRstToOff[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetRstToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToOff_Len;

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &HS21044_Base::SetRstToCur[0];    //指向数组0
    OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetRstToCur) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToCur_Len;

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &HS21044_Base::SetRstToVlt[0];
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetRstToVlt) /	sizeof(St_DisposableTask);//HS21044_Base::SetRstToVlt_Len;

    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &HS21044_Base::SetCCToCC[0];
    OS_LogicProcess::SetCtrlCurToCur.stepLen =sizeof(SetCCToCC) /	sizeof(St_DisposableTask);// HS21044_Base::SetCCToCC_Len;


    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &HS21044_Base::SetCCToCV[0];
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCCToCV) /	sizeof(St_DisposableTask);//HS21044_Base::SetCCToCV_Len;

    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &HS21044_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetCurToOff_Len ;

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &HS21044_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCurToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetCurToOff_Len ;

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &HS21044_Base::SetCVToCV[0];
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetCVToCV) /	sizeof(St_DisposableTask);//HS21044_Base::SetCVToCV_Len;

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &HS21044_Base::SetCVToCC[0];
    OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetCVToCC) /	sizeof(St_DisposableTask);//HS21044_Base::SetCVToCC_Len;

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &HS21044_Base::SetVltToOff[0];
    OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetVltToOff_Len ;

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &HS21044_Base::SetVltToOff[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToOff) /	sizeof(St_DisposableTask);//HS21044_Base::SetVltToOff_Len ;
}

void HS21044_Base::AdditionalEngine(void)
{
    // ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &HS21044_Base::SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &HS21044_Base::SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}
void HS21044_Base::EngineStuff(void)
{
    memset(&SetRstToCur,  0,    sizeof(SetRstToCur));
    memset(&SetCurToOff,  0,    sizeof(SetCurToOff));
    memset(&SetCCToCC,    0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,  0,    sizeof(SetRstToOff));
    memset(&SetCCToCV,    0,    sizeof(SetCCToCV));
    memset(&SetRstToVlt,  0 ,   sizeof(SetRstToVlt));
    memset(&SetVltToOff,  0 ,   sizeof(SetVltToOff));
    memset(&SetCVToCV,    0 ,   sizeof(SetCVToCV));
    memset(&SetCVToCC,    0 ,   sizeof(SetCVToCC));
    memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    //---------------------------------------------//
    //电流开通道
    SetRstToCur[0].uw32_DelayTime = 100;
    SetRstToCur[0].RelayMove.InactiveFlag = HS21044_RLY_IVSD;  // I/V切换到I
    SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetRstToCur[1].uw32_DelayTime = 100;
    SetRstToCur[1].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetRstToCur[2].uw32_DelayTime = 100;
    SetRstToCur[2].DacMove.DacFlag =  ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetRstToCur[2].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetRstToCur[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetRstToCur[3].uw32_DelayTime = 100;
    SetRstToCur[3].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetRstToCur[4].uw32_DelayTime = 100;
    SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //电压开通道
    SetRstToVlt[0].uw32_DelayTime = 100;
    SetRstToVlt[0].RelayMove.InactiveFlag = HS21044_RLY_IVSD;  // I/V切换到I
    SetRstToVlt[0].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO|ACTIVE_DAC_CUR_ZERO;
    SetRstToVlt[1].uw32_DelayTime = 100;
    SetRstToVlt[1].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetRstToVlt[2].uw32_DelayTime = 100;
    SetRstToVlt[2].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetRstToVlt[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetRstToVlt[3].uw32_DelayTime = 100;
    SetRstToVlt[3].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetRstToVlt[4].uw32_DelayTime = 100;
    SetRstToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;
    SetRstToVlt[5].uw32_DelayTime = 100;
    SetRstToVlt[5].RelayMove.ActiveFlag = HS21044_RLY_IVSD;  // I/V切换到V

    //---------------------------------------------//
    //电流关通道
    SetCurToOff[0].uw32_DelayTime = 100;
    SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCurToOff[1].uw32_DelayTime = 100;
    SetCurToOff[1].RelayMove.InactiveFlag = HS21044_RLY_IVSD;  //I/V继电器切换至I
    SetCurToOff[2].uw32_DelayTime = 200;
    SetCurToOff[2].RelayMove.InactiveFlag = HS21044_RLY_Output | HS21044_RLY_ACIM;
    SetCurToOff[3].uw32_DelayTime = 100;
    SetCurToOff[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    //电压关通道
    SetVltToOff[0].uw32_DelayTime = 100;
    SetVltToOff[0].DacMove.DacFlag =  ACTIVE_DAC_CUR_ZERO;
    SetVltToOff[0].RelayMove.InactiveFlag = HS21044_RLY_IVSD;  //I/V继电器切换至I
    SetVltToOff[1].uw32_DelayTime = 100;
    SetVltToOff[1].DacMove.DacFlag =  ACTIVE_DAC_VLT_ZERO;
    SetVltToOff[2].uw32_DelayTime = 100;
    SetVltToOff[2].RelayMove.InactiveFlag = HS21044_RLY_Output;
    SetVltToOff[3].uw32_DelayTime = 100;
    SetVltToOff[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    //---------------------------------------------//
    //电流切换到电流
    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag =  ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetCCToCC[0].RangeChangedFlag = 1;
    SetCCToCC[1].uw32_DelayTime = 100;
    SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCC[1].RangeChangedFlag = 1;

    SetCCToCC[2].uw32_DelayTime = 200;
    SetCCToCC[2].RelayMove.InactiveFlag = HS21044_RLY_Output ;
    SetCCToCC[2].RangeChangedFlag = 1;

    SetCCToCC[3].uw32_DelayTime = 100;
    SetCCToCC[3].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetCCToCC[3].RangeChangedFlag = 1;

    SetCCToCC[4].uw32_DelayTime = 100;
    SetCCToCC[4].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetCCToCC[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[4].RangeChangedFlag = 1;
    SetCCToCC[5].uw32_DelayTime = 100;
    SetCCToCC[5].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetCCToCC[5].RangeChangedFlag = 1;
    SetCCToCC[6].uw32_DelayTime = 20;   //modified from 20ms to 2ms for yongun's request,160115
    SetCCToCC[6].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    //电压切换到电压
    SetCVToCV[0].uw32_DelayTime =100;
    SetCVToCV[0].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetCVToCV[0].RangeChangedFlag = 1;
    SetCVToCV[1].uw32_DelayTime =200;
    SetCVToCV[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCVToCV[1].RangeChangedFlag = 1;
    SetCVToCV[2].uw32_DelayTime =100;
    SetCVToCV[2].DacMove.DacFlag = ACTIVE_DAC_VLT;
    SetCVToCV[3].uw32_DelayTime =100;
    SetCVToCV[3].RelayMove.InactiveFlag = HS21044_RLY_Protect;

    //电压切换到电流
    SetCVToCC[0].uw32_DelayTime = 50;
    SetCVToCC[0].RelayMove.InactiveFlag = HS21044_RLY_IVSD;     //切换到电流
    //SetCVToCC[0].RangeChangedFlag = 0;
    SetCVToCC[1].uw32_DelayTime = 100;
    SetCVToCC[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
    //SetCVToCC[1].RangeChangedFlag = 0;
    SetCVToCC[2].uw32_DelayTime = 100;
    SetCVToCC[2].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetCVToCC[2].RangeChangedFlag = 1;
    SetCVToCC[3].uw32_DelayTime = 100;
    SetCVToCC[3].RelayMove.InactiveFlag = HS21044_RLY_Output;
    SetCVToCC[3].RangeChangedFlag = 1;
    SetCVToCC[4].uw32_DelayTime = 100;
    SetCVToCC[4].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetCVToCC[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCVToCC[4].RangeChangedFlag = 1;
    SetCVToCC[5].uw32_DelayTime = 100;
    SetCVToCC[5].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetCVToCC[5].RangeChangedFlag = 1;
    SetCVToCC[6].uw32_DelayTime = 100;
    SetCVToCC[6].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //电流切换到电压
    SetCCToCV[0].uw32_DelayTime = 50;
    SetCCToCV[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCV[0].RangeChangedFlag = 0;
    SetCCToCV[1].uw32_DelayTime = 100;
    SetCCToCV[1].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetCCToCV[1].RangeChangedFlag = 1;
    SetCCToCV[2].uw32_DelayTime = 100;
    SetCCToCV[2].RelayMove.InactiveFlag = HS21044_RLY_Output;
    SetCCToCV[2].RangeChangedFlag = 1;
    SetCCToCV[3].uw32_DelayTime = 100;
    SetCCToCV[3].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetCCToCV[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCCToCV[3].RangeChangedFlag = 1;
    SetCCToCV[4].uw32_DelayTime = 100;
    SetCCToCV[4].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetCCToCV[4].RangeChangedFlag = 1;
    SetCCToCV[5].uw32_DelayTime = 100;
    SetCCToCV[5].DacMove.DacFlag = ACTIVE_DAC_VLT;
    //SetCCToCV[5].RangeChangedFlag = 0;
    SetCCToCV[6].uw32_DelayTime = 100;
    SetCCToCV[6].RelayMove.ActiveFlag = HS21044_RLY_IVSD;   //切换到电压
    //SetCCToCV[6].RangeChangedFlag = 0;

    //休息步关闭
    SetRstToOff[0].uw32_DelayTime = 10;
    SetRstToOff[0].RelayMove.InactiveFlag = HS21044_RLY_ACIM;
    //SetRstToOff[0].DacMove.DacFlag = 0;
    //SetRstToOff[0].RangeChangedFlag = 0;

    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = HS21044_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = HS21044_RLY_ACIM;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag =  ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;
    SetAutoRangeSwitch[1].uw32_DelayTime = 100;
    SetAutoRangeSwitch[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 200;
    SetAutoRangeSwitch[2].RelayMove.InactiveFlag = HS21044_RLY_Output ;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.ActiveFlag = HS21044_RLY_Protect;
    SetAutoRangeSwitch[3].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetAutoRangeSwitch[3].RangeChangedFlag = 1;

    SetAutoRangeSwitch[4].uw32_DelayTime = 100;
    SetAutoRangeSwitch[4].RelayMove.ActiveFlag = HS21044_RLY_Output;
    SetAutoRangeSwitch[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;
    SetAutoRangeSwitch[5].uw32_DelayTime = 100;
    SetAutoRangeSwitch[5].RelayMove.InactiveFlag = HS21044_RLY_Protect;
    SetAutoRangeSwitch[5].RangeChangedFlag = 1;
    SetAutoRangeSwitch[6].uw32_DelayTime = 20;   //modified from 20ms to 2ms for yongun's request,160115
    SetAutoRangeSwitch[6].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		#endif
}
char HS21044_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    unsigned long value = Data;    //强制保护

    switch(type)
    {
    case ACTIVE_DAC_CUR:
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HS21044_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
        break;
    case ACTIVE_DAC_VLT:
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HS21044_ADC5DAC::Dac_out_Vlt(ChannelIdx,value);
        break;
    case ACTIVE_DAC_VH:
        HS21044_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTA - DACH  // 20140613  电压保护
        break;
    case ACTIVE_DAC_VL:
        HS21044_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTC - DACL
        break;
    default:
        break;
    }

    return 1;
}

char HS21044_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange) //换量程
{
    ControlPin::SetMeHigh(PinGroup[HS21044_Pin_LEDDriver]);   // add by qjm20150901 量程指示灯
    switch(CurrRange)
    {
    case CURRENT_RANGE_1:
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_A0]);
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_A1]);
        return 1;
    case CURRENT_RANGE_2:
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_A0]);
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_A1]);
        return 1;
    case CURRENT_RANGE_3:
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_A0]);
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_A1]);
        return 1;
    default:
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_A0]);
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_A1]);
        return 0;
    }
}
void HS21044_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_HS21044_Relay MyRly;

    MyRly.m_Reg = SetRly;
    if(MyRly.m_Bits.Protect)
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_Protect]);
    if(MyRly.m_Bits.IVSD)
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_IVS]);
    if(MyRly.m_Bits.Output)
    {
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_Enable]);
// 		m_bChanOn = true;
    }

    if(MyRly.m_Bits.Acim)
    {
        ControlPin::SetMeHigh(PinGroup[HS21044_Pin_Acim]);
    }

    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Protect)
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_Protect]);
    if(MyRly.m_Bits.IVSD)
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_IVS]);

    if(MyRly.m_Bits.Output)
    {
// 		m_bChanOn = false;
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_Enable]);
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_LEDDriver]);   // add by qjm20150901 量程指示灯
    }

// 	if(m_bChanOn)
// 	{
// 		if(bAutoCali)
// 			ControlPin::SetMeHigh(PinGroup[HS21044_Pin_Cali]);
// 		else
// 			ControlPin::SetMeLow(PinGroup[HS21044_Pin_Cali]);
// 	}
// 	else
// 		ControlPin::SetMeLow(PinGroup[HS21044_Pin_Cali]);

    if(MyRly.m_Bits.Acim)
    {
        ControlPin::SetMeLow(PinGroup[HS21044_Pin_Acim]);
    }
}

void HS21044_Base::Feed_CpldWD(void)
{
    //调节风扇
    //HS21044_Base::SetFanPwm();//20150918 sunshine
    return;
}

//#if(FAN_SAFETY_MODE==1)
char HS21044_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    return (excheck::ExFanCheckSafety(PinGroup[HS21044_Pin_FanCheckSignal]));
}
//#endif

char HS21044_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{
    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
    //float FanCtrlValue[8];
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
// 	{
// 		  float FanCtrlValue[8]={50,60,70,80,90,100,100,100};
// // 			float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
// // 	    float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;
// //
// // 	    FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;
// 			switch(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange)
// 			{
// 				case CURRENT_RANGE_2:
// 				case CURRENT_RANGE_3:
//             for(unsigned char uc_j=0;uc_j<8;uc_j++)
//             FanCtrlValue[uc_j] = 50;
// 						if(ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_High>0.5f)
// 						{
// 							FanCtrlValue[6]=60;
// 							FanCtrlValue[7]=60;
// 						}
// 						break;
// 				case CURRENT_RANGE_4:
// 				       for(unsigned char uc_j=0;uc_j<8;uc_j++)
//                             FanCtrlValue[uc_j] = 50;
// 						break;
// 				default:
// 						break;
// 			}
// // 			FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
//   }


//   FanAdj_task::MAX_Ratio = 50;//先设成最小
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//寻找各通道最大风扇速度
// 	{
// 		if(FanAdj_task::Channel_Duty_Ratio[uc_i]>FanAdj_task::MAX_Ratio)
// 		{
// 			FanAdj_task::MAX_Ratio=FanAdj_task::Channel_Duty_Ratio[uc_i];
//     }
// 	}

    LPC4078PWM::PWM_MatchUpdate(PWM_HD1,PWM_CH5, DutyRatio, PWM_MATCH_UPDATE_NOW);
    return 1;
}



void HS21044_Base::ReadAllBuildInVolt(void)
{
    if(ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode[0])
    {
        ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode[0] = 0;
        ChannelInfo_Manager::Insert_BuildInVolt2HistoryData(0,0,ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0]);
    }
}
unsigned char HS21044_Base::PowerOffCheck(void)
{
    unsigned char temp;
    temp = ControlPin::ReadMe(HS21044_Base::PinGroup[HS21044_Pin_PowerOff]);
    return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
