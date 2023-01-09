#include "../../Entry/includes.h"

#define  LBT20083_FINE_TUNE_STARTTIME          0.1f
#define  LBT20083_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT20083_FINE_TUNE_STEPFACTOR         6E-6
#define  LBT20083_FINE_TUNE_MAXADJUSTFACTOR    0.06f

#define  LBT20083_ACCURACY                     0.0005f   //精度
#define  LBT20083_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT20083_SETTING_TIME                 50        //电流上升沿时间，单位为100us
#define  LBT20083_RISING_TIME     (50)// Rising time,  unit is 100us
#define  LBT20083_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT20083_Base::SetCtrlOpenCh[3];
St_DisposableTask LBT20083_Base::SetCtrlCloseCh[2];
St_DisposableTask LBT20083_Base::SetCCToCC[3];
St_DisposableTask LBT20083_Base::SetRstToOff[2];
//St_DisposableTask LBT20083_Base::SetCVToCV[2];

SSPI_Para  LBT20083_Base::LBT20083_SSP0DAC;
SSPI_Para  LBT20083_Base::LBT20083_SSP1ADC;
#pragma arm section

// unsigned char LBT20083_Base::SetCtrlOpenCh_Len = 3;
// unsigned char LBT20083_Base::SetCtrlCloseCh_Len = 2;
// unsigned char LBT20083_Base::SetCCToCC_Len = 3;
// unsigned char LBT20083_Base::SetRstToOff_Len = 2;
//unsigned char LBT20083_Base::SetCVToCV_Len = 2;

const St_Port LBT20083_Base::PinGroup[LBT20083_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port     Bits       PortType         Function               Address
    {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        0,         eCPTYPE_CPLD,      CPLD_O,               NBUS_WRITE_DCS},
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
    {2,        4,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
};
const unsigned char LBT20083_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT20083_Base::m_bHaveClampVdac = 0;
// const unsigned char LBT20083_Base::m_bChannelBasedClampVdac = 0;

volatile St_LBT20083_Relay LBT20083_Base::LBT20083_relaystatus[MAXCHANNELNO];

void LBT20083_Base::Hardware_Init(void)
{
    LBT20083_FeatureEnable_Init();
    IO_Init();    //内部IO初始化
    Feed_CpldWD();  //喂狗信号

    board_config::bHaveRedundantCheck = 0;

    CPLD_FUNC::Init();
    memset((void *)(&LBT20083_relaystatus[0]), 0, MAXCHANNELNO);
    Feed_CpldWD();  //喂狗信号

    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_Cali = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
    //此处powerOn是否还需要
    ControlPin::SetMeLow(PinGroup[LBT20083_Pin_PowerOn]);

    //SSP0 for DAC    AD5360
    LBT20083_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    LBT20083_SSP0DAC.uc_Speed = 5;
    LBT20083_SSP0DAC.uc_CPOL = 0;
    LBT20083_SSP0DAC.uc_CPHA = 1;
    LBT20083_SSP0DAC.uc_MSTR = 1;    //master
    LBT20083_SSP0DAC.uc_BITS = 8;
    LBT20083_SSP0DAC.uc_LSBF = 0;

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        //SSP1 for ADC
        LBT20083_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
        LBT20083_SSP1ADC.uc_Speed = 5;
        LBT20083_SSP1ADC.uc_CPOL = 0;
        LBT20083_SSP1ADC.uc_CPHA = 0;       //rising trig
        LBT20083_SSP1ADC.uc_MSTR = 1;    //master
        LBT20083_SSP1ADC.uc_BITS = 8;
        LBT20083_SSP1ADC.uc_LSBF = 0;
    }
    else
    {
        //SSP1 for ADC    AD7175
        LBT20083_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
        LBT20083_SSP1ADC.uc_Speed = 7;
        LBT20083_SSP1ADC.uc_CPOL = 1;
        LBT20083_SSP1ADC.uc_CPHA = 1;
        LBT20083_SSP1ADC.uc_MSTR = 1;    //master
        LBT20083_SSP1ADC.uc_BITS = 8;
        LBT20083_SSP1ADC.uc_LSBF = 0;
    }

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
    Feed_CpldWD();  //喂狗信号

    ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();

    Feed_CpldWD();  //喂狗信号
    OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                        &SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
    //	AdditionalEngine();
}

void LBT20083_Base::LBT20083_FeatureEnable_Init(void)
{
// 	static St_Hardware_Feature_Enable para;
// 	memset(&para,0,sizeof(para));
//
// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_D)
// 	{
// 		para.Redundant_Safety_Mode=1;
// 		para.RedundantVoltMode=REDUNDANT_PIN;
// 	}
// 	para.OS_Tick_Interval=400;
// 	board_config::Feature_Enable_init=&para;
    board_config::Feature_Enable_init.OS_Tick_Interval =400;
}

void LBT20083_Base::IO_Init(void)
{
    for(int i=0; i<LBT20083_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup[i]);
}

void LBT20083_Base::ADC_Init(void)
{
    LBT20083_ADC5DAC::Adc_Init();
}
void LBT20083_Base::DAC_Init(void)
{
    LBT20083_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 3;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = LBT20083_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = LBT20083_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT20083_Tpid_Tsys;

    ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT20083_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();

    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT20083_FINE_TUNE_STARTTIME;    //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT20083_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT20083_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT20083_FINE_TUNE_STEPFACTOR;

//	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT20083_ACCURACY * LBT20083_CTRL_VALUE_ERROR_FACTOR;// //180614 zyx

    board_config::Accuracy = LBT20083_ACCURACY;

    LBT20083_SampleManage::Init();
    IR::pIR_ADC = &(LBT20083_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(LBT20083_SampleManage::renew_IR_ADC);
    IR::pIR_Trigger_ADC = &(LBT20083_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(LBT20083_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=LBT20083SETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=LBT20083SETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=LBT20083SETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=LBT20083SETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=LBT20083MINIMUMPULSE;
}

void LBT20083_Base::Func_Init(void)
{
    board_config::Func_Dac_out=&LBT20083_Base::DAC_out;
    board_config::Func_SetRelay =&LBT20083_Base::SetRelay;
    board_config::Func_Sample = &LBT20083_SampleManage::SampleISR;
    board_config::Func_SetRange = &LBT20083_Base::SetRange;
    board_config::FeedWatchDog = &LBT20083_Base::Feed_CpldWD;
}


//void LBT20083_Base::AdditionalEngine(void)
//{
//}

// void LBT20083_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {

// }
// void LBT20083_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {

// }
void LBT20083_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
    memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
    memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    //	memset(&SetCVToCV,       0,    sizeof(SetCVToCV));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    //---------------------------------------------//
    SetCtrlOpenCh[0].uw32_DelayTime = 100;    //加大延迟，防止继电器对AD8253干扰
    SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCtrlOpenCh[1].uw32_DelayTime = 200;
    SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //输出量程继电器

    SetCtrlOpenCh[2].uw32_DelayTime = 100;
    SetCtrlOpenCh[2].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //---------------------------------------------//
    SetCtrlCloseCh[0].uw32_DelayTime = 100;
    SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    //SetCtrlCloseCh[0].RangeChangedFlag = 0;

    SetCtrlCloseCh[1].uw32_DelayTime = 100;
    SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT20083_RLY_Output;
    //SetCtrlCloseCh[1].RangeChangedFlag = 0;

    //---------------------------------------------//
    // 暂时没有加入假的ADC切换过程
    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCC[0].RangeChangedFlag = 1;  	//将来有可能是关掉

    SetCCToCC[1].uw32_DelayTime = 100;
    SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //直接输出下一量程
    SetCCToCC[1].RangeChangedFlag = 1;

    SetCCToCC[2].uw32_DelayTime = 200;
    SetCCToCC[2].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //---------------------------------------------//
    SetRstToOff[0].uw32_DelayTime = 100;
    //SetRstToOff[0].DacMove.DacFlag = 0;
    //SetRstToOff[0].RangeChangedFlag = 0;

    //SetRstToOff[1].uw32_DelayTime = 0;
    //SetRstToOff[1].DacMove.DacFlag = 0;
    //SetRstToOff[1].RangeChangedFlag = 0;
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
char LBT20083_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    //	unsigned long value = Data;    //强制保护

    switch(type)
    {
    case ACTIVE_DAC_CUR:
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        SSPIFunc::ConfirmDevice(0, &LBT20083_Base::LBT20083_SSP0DAC);
        if(ChannelIdx>=4)
            ChannelIdx += 4;
        DAC16bit_AD5360::Dac_out(ChannelIdx,Data);
        break;
    default:
        break;
    }
    return 1;
}

// void LBT20083_Base::Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain)
// {

// }
// void LBT20083_Base::Adjust_ADC_Gain(unsigned char type,unsigned char Gain)
// {
// 	St_LBT20083_PGA MyReg;

// 	MyReg.Bits.AdcGain_I = 0;
// 	MyReg.Bits.AdcGain_V = 0;
// 	MyReg.Bits.AdcGainEnable = 1;
// 	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

// 	MyReg.Bits.AdcGainEnable = 0;
// 	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

// 	MyReg.Bits.AdcGainEnable = 1;
// 	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);
// }


void LBT20083_Base::ADC_ADG_open(unsigned char ChannelIdx)
{
    CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void LBT20083_Base::ADG_Reset(void)
{
    ControlPin::SetMeLow(LBT20083_Base::PinGroup[LBT20083_Pin_AdgRst]);
    ARBUS_DELAY(5);
    ControlPin::SetMeHigh(LBT20083_Base::PinGroup[LBT20083_Pin_AdgRst]);
}

// void LBT20083_Base::changeADCRange(unsigned char ChannelIdx,unsigned char range)
// {
// 	ARBUS_DELAY(10);

// 	switch(range)
// 	{
// 	case CURRENT_RANGE_1:
// 		LBT20083_Base::Adjust_CHN_Gain(ChannelIdx,10);
// 		break;
// 	case CURRENT_RANGE_2:
// 		LBT20083_Base::Adjust_CHN_Gain(ChannelIdx,100);
// 		break;
// 	case CURRENT_RANGE_3:
// 		LBT20083_Base::Adjust_CHN_Gain(ChannelIdx,1000);
// 		break;
// 	default:
// 		LBT20083_Base::Adjust_CHN_Gain(ChannelIdx,10);
// 		break;
// 	}
// }
char LBT20083_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
    switch(CurrRange)
    {
    case CURRENT_RANGE_1:
        LBT20083_relaystatus[ChannelIdx].Bits.R_CH = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        //ARBUS_DELAY(10);
        LBT20083_relaystatus[ChannelIdx].Reg &= ~(LBT20083_RLY_R_CM | LBT20083_RLY_R_CL);
        //             LBT20083_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //             LBT20083_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        break;
    case CURRENT_RANGE_2:
        LBT20083_relaystatus[ChannelIdx].Bits.R_CM = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        //ARBUS_DELAY(10);
        LBT20083_relaystatus[ChannelIdx].Reg &= ~(LBT20083_RLY_R_CH | LBT20083_RLY_R_CL);
        //             LBT20083_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //             LBT20083_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        break;
    case CURRENT_RANGE_3:
        LBT20083_relaystatus[ChannelIdx].Bits.R_CL = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        //ARBUS_DELAY(10);
        LBT20083_relaystatus[ChannelIdx].Reg &= ~(LBT20083_RLY_R_CH | LBT20083_RLY_R_CM);
        //LBT20083_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //LBT20083_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        break;
    default:
        LBT20083_relaystatus[ChannelIdx].Reg &= ~(LBT20083_RLY_R_CH | LBT20083_RLY_R_CM | LBT20083_RLY_R_CL);
        //LBT20083_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //LBT20083_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //LBT20083_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        break;
    }
    ARBUS_DELAY(10);
    CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);

    if( (LBT20083_relaystatus[ChannelIdx].Reg & (LBT20083_RLY_R_CH | LBT20083_RLY_R_CM | LBT20083_RLY_R_CL )) == 0x0000)	// if CH=0 & CM=0 & CL=0 & CLL=0
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_Cali = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
    return 1;
}
void LBT20083_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_LBT20083_Relay2 MyRly;

    if(bAutoCali)
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_Cali = 0;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
    else
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_Cali = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Output)
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_CH = 0;
        ARBUS_DELAY(10);
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        LBT20083_relaystatus[ChannelIdx].Bits.R_CM = 0;
        ARBUS_DELAY(10);
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
        LBT20083_relaystatus[ChannelIdx].Bits.R_CL = 0;
        ARBUS_DELAY(10);
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
    if( (LBT20083_relaystatus[ChannelIdx].Reg & (LBT20083_RLY_R_CH | LBT20083_RLY_R_CM | LBT20083_RLY_R_CL )) == 0x0000)	// if CH=0 & CM=0 & CL=0 & CLL=0
    {
        LBT20083_relaystatus[ChannelIdx].Bits.R_Cali = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT20083_relaystatus[ChannelIdx].Reg);
    }
}

void LBT20083_Base::Feed_CpldWD(void)
{
    ControlPin::SetMeHigh(PinGroup[LBT20083_Pin_WD]);
    TIMERSFunc::DELAY_SYS(100, 0);
    ControlPin::SetMeLow(PinGroup[LBT20083_Pin_WD]);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
