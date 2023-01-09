#include "includes.h"

#define  HPT200_FINE_TUNE_STARTTIME          0.1f
#define  HPT200_FINE_TUNE_MINERRORFACTOR     30E-6// modified by Ron 080715, real ppm is one half of this number
#define  HPT200_FINE_TUNE_STEPFACTOR         3E-6 // was 3E-6. Modified by Ron 081215 

#define  HPT200_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  HPT200_ACCURACY                     0.0005f   //精度
#define  HPT200_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  HPT200_SETTING_TIME                 500       //电流上升沿时间，单位为100us
#define HPT200_RISING_TIME  (500)// Rising time,  unit is 100us
#define  HPT200_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask HPT200_Base::SetRstToCur[5];
St_DisposableTask HPT200_Base::SetCurToOff[5];
St_DisposableTask HPT200_Base::SetCCToCC[6];
St_DisposableTask HPT200_Base::SetRstToOff[1];

St_DisposableTask HPT200_Base::SetCaliBoardRstToVlt[3];
St_DisposableTask HPT200_Base::SetCaliBoardVltToOff[2];
St_DisposableTask HPT200_Base::SetCaliBoardVltToVlt[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask HPT200_Base::SetAutoRangeSwitch[6];
#endif
SSPI_Para  HPT200_Base::HPT200_SSP0DAC;
SSPI_Para  HPT200_Base::HPT200_SSP1ADC;
#pragma arm section

// unsigned char HPT200_Base::SetRstToCur_Len = 5;  //6;    //this is maybe problem 20131107 chen
// unsigned char HPT200_Base::SetCurToOff_Len = 5;
// unsigned char HPT200_Base::SetCCToCC_Len = 6;
// unsigned char HPT200_Base::SetRstToOff_Len = 1;

// unsigned char HPT200_Base::SetCaliBoardRstToVlt_Len=3;
// unsigned char HPT200_Base::SetCaliBoardVltToOff_Len=2;
// unsigned char HPT200_Base::SetCaliBoardVltToVlt_Len = 1;

const St_Port HPT200_Base::PinGroup[HPT200_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port      Bits       PortType           Function               Address

    {0,       7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
    {0,       8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
    {0,       9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
    {4,       29,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {4,       28,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,       1,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,       6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,       0,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,       2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,       16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,       3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,       5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,       16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,       17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,       18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,       19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,       22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,       22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,       28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.28
    {3,       23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P3.23
    {3,       21,        eCPTYPE_MCUGPIO,   PWM0,                  0},  //P3.21 Ron 7-24-15
    {0,       21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.21
    {0,       22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.22
    {2,       5,         eCPTYPE_MCUGPIO,   PWM1,                  0},
    {2,       11,        eCPTYPE_MCUGPIO,   GPIO_I,                0},  //P2.11
    {2,       9,         eCPTYPE_MCUGPIO,   GPIO_I,                0},  //P2.9
    {0,       22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,       21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,       15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,       17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,       18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
    {0,       11,        eCPTYPE_MCUGPIO,   GPIO_O,                1},  //P0.11
    {0,       10,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,       3,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
};

const unsigned char HPT200_Base::m_bHaveVoltageDac = 0;
const unsigned char HPT200_Base::m_bHaveClampVdac = 1;
// const unsigned char HPT200_Base::m_bChannelBasedClampVdac = 1;

void HPT200_Base::Hardware_Init(void)
{
    HPT200_FeatureEnable_Init();
    IO_Init();

    UnitDO_Reset(0,0);

    //SSP0 for DAC    AD5754
    HPT200_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    HPT200_SSP0DAC.uc_Speed = 5;
    HPT200_SSP0DAC.uc_CPOL = 0;
    HPT200_SSP0DAC.uc_CPHA = 1;
    HPT200_SSP0DAC.uc_MSTR = 1;    //master
    HPT200_SSP0DAC.uc_BITS = 8;
    HPT200_SSP0DAC.uc_LSBF = 0;

    //ADS1259
    HPT200_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    HPT200_SSP1ADC.uc_Speed = 2;
    HPT200_SSP1ADC.uc_CPOL = 0;
    HPT200_SSP1ADC.uc_CPHA = 1;
    HPT200_SSP1ADC.uc_MSTR = 1;    //master
    HPT200_SSP1ADC.uc_BITS = 8;
    HPT200_SSP1ADC.uc_LSBF = 0;

    ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();

    //RANGE INIT

    ControlPin::SetMeHigh(PinGroup[HPT200_Pin_IVSD]);
    ControlPin::SetMeHigh(PinGroup[HPT200_Pin_A_V]);

    //DAC INIT
    HPT200_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
    OS_LogicProcess::InitBoardEngine(	&SetRstToCur[0],	  sizeof(SetRstToCur)		/		sizeof(St_DisposableTask),
                                        &SetCurToOff[0],    sizeof(SetCurToOff)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
    AdditionalEngine();

    //拉高钳压    // 20140613  电压保护
    HPT200_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
    HPT200_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
    //Begin adding by Ron 7-27-15
    LPC4078PWM::CLKPWR_ConfigPPWR (0, ENABLE); // Ron 7-24-15
    LPC4078PWM::PWM_ChannelCmd(0, 6, ENABLE);// Ron 7-24-15
    LPC4078PWM::PWM_ChannelConfig(0,6,PWM_CHANNEL_SINGLE_EDGE); // Ron 7-24-15
    LPC4078PWM::PWM_MatchUpdate(0,0,24e2, PWM_MATCH_UPDATE_NOW); // Ron 7-24-15
    LPC4078PWM::PWM_ResetCounter(0);
    LPC4078PWM::PWM_CounterCmd(0, ENABLE);
    LPC4078PWM::PWM_Cmd(0, ENABLE);
    board_config::bFanAdj=1; // added by Ron 7-27-15
    FanAdj_task::AdjMod=PWMMOD; // Added by Ron 7-27-15
}

void HPT200_Base::HPT200_FeatureEnable_Init(void)
{
// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C)
// 	{
// 		board_config::Feature_Enable_init.Redundant_Safety_Mode=1;
    board_config::Feature_Enable_init.Power_Off_Enable=0;  //  04.27.2018 ZC
// 		board_config::Feature_Enable_init.Fan_Safety_Mode=1;
// 	}
    board_config::Feature_Enable_init.OS_Tick_Interval=200;

// 	board_config::Feature_Enable_init->Power_Off_Enable =1;
// 	board_config::Feature_Enable_init->Fan_Safety_Mode =1;
// 	board_config::Feature_Enable_init->Redundant_Safety_Mode = 1;
// 	board_config::Feature_Enable_init->OS_Tick_Interval =200;
}

void HPT200_Base::IO_Init(void)
{
    for(int i=0; i<HPT200_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup[i]);
}
void HPT200_Base::ADC_Init(void)
{
    HPT200_ADC5DAC::Adc_Init();
}
void HPT200_Base::DAC_Init(void)
{
    HPT200_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 8;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = HPT200_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = HPT200_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = HPT200_Tpid_Tsys;

    ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void HPT200_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();

    // 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)
    // 	{
    // 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);
    // 		regEngine(i,board_config::st_channel_info[i].Channel_type);
    // 	}

    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = HPT200_FINE_TUNE_STARTTIME;   //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = HPT200_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = HPT200_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = HPT200_FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime =0.7168;


//	safety_check::m_CtrlValueSetting.fErrorFactor       = HPT200_ACCURACY * HPT200_CTRL_VALUE_ERROR_FACTOR; //180614 zyx

    board_config::Accuracy = HPT200_ACCURACY;
    HPT200_SampleManage::Init();
    IR::pIR_ADC = &(HPT200_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(HPT200_SampleManage::renew_IR_ADC);
//	  pIR_Trigger_ADC = &(HPT200_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(HPT200_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=HPT200SETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=HPT200SETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=HPT200SETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=HPT200SETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=HPT200MINIMUMPULSE;
}

void HPT200_Base::Func_Init(void)
{
    //board_config::Func_Adc_rd=&HPT200_Base::ADC_rd;
    board_config::Func_Dac_out=&HPT200_Base::DAC_out;
    board_config::Func_SetRange=&HPT200_Base::SetRange;
    board_config::Func_SetRelay=&HPT200_Base::SetRelay;
    board_config::Func_Sample =&HPT200_SampleManage::SampleISR;    //timer0 中断
    board_config::Func_UnitDO=&HPT200_Base::OUTPUT_UnitDO;
    board_config::Func_UnitDI=&HPT200_Base::Read_UnitDI;
    board_config::Func_UnitDI_ReadAll = &HPT200_Base::UnitDI_ReadAll;
    board_config::Func_UnitDO_Reset = &HPT200_Base::UnitDO_Reset;
    board_config::Func_RTCProcedureForIV=&HPT200_Base::RTCProcedureForHPT200;
    board_config::FeedWatchDog = &HPT200_Base::Feed_CpldWD;
    board_config::Func_FanAdj = &HPT200_Base::FanAdjOperate; // Added by Ron 7-27-15
    PowerLostCheck::Func_PowerOffSafetCheck = &HPT200_Base::PowerOffCheck;

}

void HPT200_Base::AdditionalEngine(void)
{
    OS_LogicProcess::SetCaliBoardRstToVlt.st_Disposabletask_ptr = &HPT200_Base::SetCaliBoardRstToVlt[0];
    OS_LogicProcess::SetCaliBoardRstToVlt.stepLen =  sizeof(SetCaliBoardRstToVlt) / sizeof(St_DisposableTask); //HPT200_Base::SetCaliBoardRstToVlt_Len;

    OS_LogicProcess::SetCaliBoardVltToOff.st_Disposabletask_ptr =&HPT200_Base::SetCaliBoardVltToOff[0];
    OS_LogicProcess::SetCaliBoardVltToOff.stepLen =  sizeof(SetCaliBoardVltToOff) / sizeof(St_DisposableTask); //HPT200_Base::SetCaliBoardVltToOff_Len;

    OS_LogicProcess::SetCaliBoardVltToVlt.st_Disposabletask_ptr =&HPT200_Base::SetCaliBoardVltToVlt[0];
    OS_LogicProcess::SetCaliBoardVltToVlt.stepLen =  sizeof(SetCaliBoardVltToVlt) / sizeof(St_DisposableTask);  //HPT200_Base::SetCaliBoardVltToVlt_Len;
}
void HPT200_Base::EngineStuff(void)
{
    memset(&SetRstToCur,  0,    sizeof(SetRstToCur));
    memset(&SetCurToOff,  0,    sizeof(SetCurToOff));
    memset(&SetCCToCC,    0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,  0,    sizeof(SetRstToOff));
    memset(&SetCaliBoardRstToVlt,  0,    sizeof(SetCaliBoardRstToVlt));
    memset(&SetCaliBoardVltToOff,  0,    sizeof(SetCaliBoardVltToOff));
    memset(&SetCaliBoardVltToVlt,  0,    sizeof(SetCaliBoardVltToVlt));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    /// Start: by Philip 3/9/2015
    //电流开通道
    SetRstToCur[0].uw32_DelayTime = 100;
    SetRstToCur[0].RelayMove.ActiveFlag = HPT200_RLY_Protect;  // Protect goes 1
    SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetRstToCur[1].uw32_DelayTime = 100;
    SetRstToCur[1].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetRstToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetRstToCur[2].uw32_DelayTime = 100;
    SetRstToCur[2].RelayMove.ActiveFlag = HPT200_RLY_V;
    SetRstToCur[3].uw32_DelayTime = 100;
    SetRstToCur[3].RelayMove.InactiveFlag = HPT200_RLY_Protect;
    SetRstToCur[4].uw32_DelayTime = 100;
    SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
    // End: by Philip 3/9/2015

    // Start: by Philip 3/9/2015
    //电流关通道
    SetCurToOff[0].uw32_DelayTime = 100;
    SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCurToOff[1].uw32_DelayTime = 100;
    SetCurToOff[1].RelayMove.InactiveFlag = HPT200_RLY_V;  //CH-ON 信号
    SetCurToOff[2].uw32_DelayTime = 100;
    SetCurToOff[2].RelayMove.InactiveFlag = HPT200_RLY_Output;
    SetCurToOff[3].uw32_DelayTime = 100;
    SetCurToOff[4].uw32_DelayTime = 100;
    SetCurToOff[4].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    // End: by Philip



    //---------------------------------------------//
    //电流切换到电流
    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag =  ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetCCToCC[0].RangeChangedFlag = 1;
    SetCCToCC[1].uw32_DelayTime = 100;
    SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCC[1].RangeChangedFlag = 1;
    SetCCToCC[2].uw32_DelayTime = 100;
    SetCCToCC[2].RelayMove.ActiveFlag = HPT200_RLY_Protect;
    SetCCToCC[2].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetCCToCC[2].RangeChangedFlag = 1;
    SetCCToCC[3].uw32_DelayTime = 100;
    SetCCToCC[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[3].RangeChangedFlag = 1;
    SetCCToCC[4].uw32_DelayTime = 100;
    SetCCToCC[4].RelayMove.InactiveFlag = HPT200_RLY_Protect;
    SetCCToCC[4].RangeChangedFlag = 1;
    SetCCToCC[5].uw32_DelayTime = 200;
    SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetCaliBoardRstToVlt[0].uw32_DelayTime =100;
    SetCaliBoardRstToVlt[0].DacMove.DacFlag =  ACTIVE_DAC_VLT_ZERO;
    SetCaliBoardRstToVlt[1].uw32_DelayTime =300;
    SetCaliBoardRstToVlt[1].RelayMove.ActiveFlag = HPT200_RLY_V_R | HPT200_RLY_A_V_R ;
    SetCaliBoardRstToVlt[2].uw32_DelayTime =100;
    SetCaliBoardRstToVlt[2].DacMove.DacFlag = ACTIVE_DAC_VLT;

    SetCaliBoardVltToOff[0].uw32_DelayTime = 100;
    SetCaliBoardVltToOff[0].DacMove.DacFlag =  ACTIVE_DAC_VLT_ZERO;
    SetCaliBoardVltToOff[1].uw32_DelayTime = 100;
    SetCaliBoardVltToOff[1].RelayMove.InactiveFlag = HPT200_RLY_V_R | HPT200_RLY_A_V_R ;

    SetCaliBoardVltToVlt[0].uw32_DelayTime =100;
    SetCaliBoardVltToVlt[0].DacMove.DacFlag =  ACTIVE_DAC_VLT;

    SetRstToOff[0].uw32_DelayTime = 10;
    //SetRstToOff[0].DacMove.DacFlag = 0;
    //SetRstToOff[0].RangeChangedFlag = 0;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag =  ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;
    SetAutoRangeSwitch[1].uw32_DelayTime = 100;
    SetAutoRangeSwitch[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;
    SetAutoRangeSwitch[2].uw32_DelayTime = 100;
    SetAutoRangeSwitch[2].RelayMove.ActiveFlag = HPT200_RLY_Protect;
    SetAutoRangeSwitch[2].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;
    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[3].RangeChangedFlag = 1;
    SetAutoRangeSwitch[4].uw32_DelayTime = 100;
    SetAutoRangeSwitch[4].RelayMove.InactiveFlag = HPT200_RLY_Protect;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;
    SetAutoRangeSwitch[5].uw32_DelayTime = 200;
    SetAutoRangeSwitch[5].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		#endif
}
char HPT200_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    unsigned long value = Data;    //强制保护

    switch(type)
    {
    case ACTIVE_DAC_CUR:
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HPT200_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
        break;

    case ACTIVE_DAC_VLT:
        if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bCaliBoard)
        {
            if(value > 0x800000)
                value = ((value - 0x800000)>>1) + 0x800000;
            else
                value = 0x800000 - ((0x800000 - value)>>1);
        }

        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        HPT200_ADC5DAC::Dac_out_Vlt(ChannelIdx,value);
        break;
        //钳位
    case ACTIVE_DAC_VH:
        HPT200_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTA - DACH  // 20140613  电压保护
        break;
    case ACTIVE_DAC_VL:
        //			unsigned long temp2;
        HPT200_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTC - DACL
        break;
    default:
        break;
    }
    return 1;
}

char HPT200_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{   ///Modified by Philip 3/9/2015
    switch(CurrRange)
    {
    case CURRENT_RANGE_1:     //CURRENT_RANGE_HIGH:
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_2:  //CURRENT_RANGE_MED:
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_3:          //CURRENT_RANGE_LOW:
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_4:           //CURRENT_RANGE_MMD:
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_5:          //CURRENT_RANGE_LMD:
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_6:
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_7:
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    case CURRENT_RANGE_8:
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_ADDA]);
        return 1;
    default:
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
        return 0;
    }
    //	return 1;
}
void HPT200_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_HPT200_Relay MyRly;

    MyRly.m_Reg = SetRly;
    if(MyRly.m_Bits.Protect)
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_Protect]);
    if(MyRly.m_Bits.IVSD)
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_IVSD]);
    if(MyRly.m_Bits.V)								// by Philip 3/9/2015
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_V]);	// by Philip 3/9/2015
    if(MyRly.m_Bits.A_V_R)
    {
        ControlPin::SetMeHigh(PinGroup[HPT200_Pin_A_V]);
    }

    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Protect)
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_Protect]);
    if(MyRly.m_Bits.IVSD)
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_IVSD]);
    if(MyRly.m_Bits.V)								// by Philip 3/9/2015
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_V]);	// by Philip 3/9/2015
    if(MyRly.m_Bits.A_V_R)
    {
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_A_V]);
    }

    if(MyRly.m_Bits.Output)
    {
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDC]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDB]);
        ControlPin::SetMeLow(PinGroup[HPT200_Pin_ADDA]);
    }
}

void HPT200_Base::Feed_CpldWD(void)
{

}

// void HPT200_Base::ADC_ADG_open(unsigned char ChannelIdx)
// {
// 	return;
// }

// Begin adding by Ron 7-27-15
char HPT200_Base::FanAdjOperate(unsigned long m_Level,unsigned long FanAdjMod)
{

    // added by Ron 081415 fan speed adjustment
    float I_current= ChannelInfo_Manager::Read_SpecifiedMetavariable2(0,MetaCode_PV_Current);
    unsigned long SetValue = 1900; //Fan PMW adjustment
    unsigned char CRange = OS_LogicProcess::m_RunningTask[0].logicProcessMSg.next_Range;
    if (CRange == CURRENT_RANGE_2)
    {
        SetValue = 2400*(I_current*I_current*3.04e-4f+0.333f);
        if (abs(I_current) < 25 || ((abs(abs(ChannelInfo_Manager::m_CurFineTune_Info[0].m_LastSetValue)-25) <= 2)
                                    && abs(I_current) >= 25))
            SetValue = 1300;
    }
    else
    {
        SetValue = 2400*(I_current*I_current*0.19e-4f+0.333f);
        if (abs(I_current) < 100 || ((abs(abs(ChannelInfo_Manager::m_CurFineTune_Info[0].m_LastSetValue)-100) <= 2)
                                     && abs(I_current) >= 100))
            SetValue = 1300;
    }
    LPC4078PWM::PWM_ChannelCmd(0, 6, ENABLE);// Ron 7-24-15
    LPC4078PWM::PWM_ResetCounter(0);
    LPC4078PWM::PWM_CounterCmd(0, ENABLE);
    LPC4078PWM::PWM_Cmd(0, ENABLE);
    LPC4078PWM::PWM_MatchUpdate(0,6, SetValue, PWM_MATCH_UPDATE_NOW);
    return 1;
}
// End adding by Ron 7-27-15
char HPT200_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
{
    unsigned char m_OutPutValue;

    if(Value>0)
        m_OutPutValue=1;
    else
        m_OutPutValue=0;

    switch (DO_Idx)
    {
    case 0:
        if(m_OutPutValue)
        {
            ControlPin::SetMeHigh(PinGroup[HPT200_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 1;
        }
        else
        {
            ControlPin::SetMeLow(PinGroup[HPT200_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 0;
        }
        break;
    case 1:
        if(m_OutPutValue)
        {
            ControlPin::SetMeHigh(PinGroup[HPT200_Pin_DO2]);
            ChannelInfo_Manager::BuildIn_DO[0][1] = 1;
        }
        else
        {
            ControlPin::SetMeLow(PinGroup[HPT200_Pin_DO2]);
            ChannelInfo_Manager::BuildIn_DO[0][1] = 0;
        }
        break;
    default:
        break;
    }

    return 1;
}
char HPT200_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
    char ReturnValue=0;
    switch (DI_Idx)
    {
    case 0:
        ReturnValue=GPIOFunc::RD_G_Port(PinGroup[HPT200_Pin_DI1].Port ,PinGroup[HPT200_Pin_DI1].Bits);
        break;
    case 1:
        ReturnValue=GPIOFunc::RD_G_Port(PinGroup[HPT200_Pin_DI2].Port ,PinGroup[HPT200_Pin_DI2].Bits);
        break;
    default:
        break;
    }
    return ReturnValue;
}
void HPT200_Base::UnitDI_ReadAll(void)
{
    if(ControlPin::ReadMe(PinGroup[HPT200_Pin_DI1]))
    {
        if(Sample_Pro::Digital_SampleCounter[0][0]>3)
        {
            ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI1].m_fValue=1;  //modified by xp //161125 zyx
            Sample_Pro::Digital_SampleCounter[0][0]=3;
        }
        Sample_Pro::Digital_SampleCounter[0][0]++;
    }
    else
    {
        Sample_Pro::Digital_SampleCounter[0][0]=0;
        ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI1].m_fValue=0;			//161125 zyx
    }

    if(ControlPin::ReadMe(PinGroup[HPT200_Pin_DI2]))
    {
        if(Sample_Pro::Digital_SampleCounter[0][1]>3)
        {
            ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI2].m_fValue=1;  //modified by xp		//161125 zyx
            Sample_Pro::Digital_SampleCounter[0][1]=3;
        }
        Sample_Pro::Digital_SampleCounter[0][1]++;
    }
    else
    {
        Sample_Pro::Digital_SampleCounter[0][1]=0;
        ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI2].m_fValue=0;				//161125 zyx
    }
}
char HPT200_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    OUTPUT_UnitDO(0,0,Value);
    OUTPUT_UnitDO(0,1,Value);
    return 1;
}
char HPT200_Base::RTCProcedureForHPT200(unsigned long CH_Number,unsigned long Value)
{
    static int myFlag = 0;
    if(myFlag == 0)
    {
        ControlPin::SetMeLow(HPT200_Base::PinGroup[HPT200_Pin_IVSD]);//added by RON, 3-24-2015
        myFlag = 1;
    }
    else
    {
        ControlPin::SetMeHigh(HPT200_Base::PinGroup[HPT200_Pin_IVSD]);
        myFlag = 0;
    }
    return 1;
}
unsigned char HPT200_Base::PowerOffCheck(void)
{
    unsigned char temp;
    temp = !(ControlPin::ReadMe(HPT200_Base::PinGroup[HPT200_Pin_PowerOff]));
    return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
