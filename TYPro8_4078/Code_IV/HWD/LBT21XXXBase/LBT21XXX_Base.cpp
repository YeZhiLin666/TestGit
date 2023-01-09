
#include "../../Entry/includes.h"

// #define  _LBT21XXX_VERSION_A_         0         //this is a temperory MACRO,and valish somedays later


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif


SSPI_Para  LBT21XXX_Base::LBT21XXX_SSP0_DAC_5360;
SSPI_Para  LBT21XXX_Base::LBT21XXX_SSP0_DAC_5754;    //VCLAMP
SSPI_Para  LBT21XXX_Base::LBT21XXX_SSP1_ADC_5560;
SSPI_Para  LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175;
#pragma arm section

const St_Port LBT21XXX_Base::PinGroup[LBT21XXX_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{
    {0,				13,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WD
    {0,				7,				eCPTYPE_MCUGPIO,		SSP1_SCK,			0},	//Pin_SCK1
    {0,				8,				eCPTYPE_MCUGPIO,		SSP1_MISO,		0},	//Pin_MISO1
    {0,				9,				eCPTYPE_MCUGPIO,		SSP1_MOSI,		0},	//Pin_MOSI1

    //{1,				2,				eCPTYPE_CPLD,				CPLD_O,				NBUS_WRITE_DCS},	//Pin_AdgRst

    {1,				6,        eCPTYPE_MCUGPIO,		GPIO_I,				0},	// Pin_RedundantSafety, 用于中电，MAXWLL
    {3,				18,				eCPTYPE_MCUGPIO,		PWM0,					0},	//Pin_FAN

//#if(FAN_SAFETY_MODE==1)
    {3,				16,				eCPTYPE_MCUGPIO,		GPIO_I,				0}, //Pin_FanCheckSignal, 风扇运行与否的检测引脚
//#endif
    {1,				11,				eCPTYPE_MCUGPIO,		GPIO_I,				0},
};
const St_Port LBT21XXX_Base::PinGroup_ADC[LBT21XXX_ADC5560_Total_Pins] =
{   //Port      Bits       PortType           Function               Address
    {1,				28,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcConver
    {2,				12,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_AdcBusy1
    {2,				13,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_AdcBusy2
    {1,				21,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcCS1
    {0,				6,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcCS2
    {1,				26,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcRst
};

const St_Port LBT21XXX_Base::PinGroup_DAC1[LBT21XXX_DAC5754_Total_Pins] =
{   //Port      Bits       PortType           Function               Address
    {1,        4,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
    {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
    {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
};
const St_Port LBT21XXX_Base::PinGroup_DAC2[LBT21XXX_DAC5360_Total_Pins] =
{   //Port      Bits       PortType           Function               Address
    {1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},//Pin_DacCS
    {2,        8,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//Pin_DacRst
    {1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//Pin_DacLoad
    {1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//Pin_DacBus
};

volatile St_LBT21XXX_Relay LBT21XXX_Base::LBT21XXX_relaystatus[MAXCHANNELNO];    //记录自身的relay状态



volatile St_LBT21XXX_DIDO     LBT21XXX_Base::CPLD_DO;    //记录DO的channel状态
volatile St_LBT21XXX_DIDO     LBT21XXX_Base::CPLD_DI;    //记录DI的channel状态


St_LBT21XXX_Feature LBT21XXX_Base::BoardFeature;

void LBT21XXX_Base::Hardware_Init(void)
{
	board_config::Feature_Enable_init.ADC_CS5560_Enable = BoardFeature.BoolFlag.Adc24bit_CS5560_Enable;
    board_config::bHaveRedundantCheck = 1;
// 	board_config::ClampH_Outputflag = 0;
// 	board_config::ClampL_Outputflag = 0;
    board_config::Feature_Enable_init.Fan_Safety_Mode=1;
    board_config::Feature_Enable_init.Power_Off_Enable =1;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveClampVdac = 1;

    Step_Manager_task::ACIM_CH_ON = 8;	// 没有通道（0-7）的ACIM回路接通, 这个初始化需要在Relay初始化前完成

    //FeatureEnable_Init();
    IO_Init();    //内部IO初始化
    Feed_CpldWD();  // 喂狗信号

    if(BoardFeature.BoolFlag.UnitDIDO_Enable)
        UnitDO_Reset(0,0);

    CPLD_FUNC::Init();

    if(BoardFeature.BoolFlag.R_VM_Enable)
        R_VM_RelayState_Init();

    Feed_CpldWD();  // 喂狗信号

    DAC_SPI_Set();
    ADC_SPI_Set();

    if(BoardFeature.BoolFlag.ACIM_Enable)
        ACIM_Relay_Init();
    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
    Feed_CpldWD();  // 喂狗信号

    //Step_Manager_task::ACIM_CH_ON = 8;	// 没有通道（0-7）的ACIM回路接通
    ADC_Init();
    DAC_Init();


    LBT21XXX_Base::changeADCRange(CURRENT_RANGE_1);
    Feed_CpldWD();  //  喂狗信号
    CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0xFF);//电压控制开   modify by qjm 20140922

    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        ////////拉高钳压     // 20140613  电压保护
        DAC_out(ChannelIdx,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
        DAC_out(ChannelIdx,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);        
    }


    FAN_Init();

}
// void LBT21XXX_Base::FeatureEnable_Init(void)
// {
//
// }
void LBT21XXX_Base::ACIM_Relay_Init(void)
{
    ControlPin::SetMeLow(board_config::Pin_ACIM);
    for(int i=0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)	// reset all ACIM relay
    {
        ARBUS_DELAY(10);
        CPLD_FUNC::WriteSpecialACIM(i,0);
    }
}
void LBT21XXX_Base::R_VM_RelayState_Init(void)
{
    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        LBT21XXX_relaystatus[ChannelIdx].Reg = 0;
        if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 0;
        }
        else
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 1;
        }
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
}
void LBT21XXX_Base::ADC_SPI_Set(void)
{
    if(BoardFeature.BoolFlag.Adc24bit_CS5560_Enable)
    {
        //SSP1 for ADC
        LBT21XXX_SSP1_ADC_5560.uc_Module_Name = SSP1_DEVICE1;    //name
        LBT21XXX_SSP1_ADC_5560.uc_Speed = 5;
        LBT21XXX_SSP1_ADC_5560.uc_CPOL = 0;
        LBT21XXX_SSP1_ADC_5560.uc_CPHA = 0;       //rising trig
        LBT21XXX_SSP1_ADC_5560.uc_MSTR = 1;    //master
        LBT21XXX_SSP1_ADC_5560.uc_BITS = 8;
        LBT21XXX_SSP1_ADC_5560.uc_LSBF = 0;
    }
    if(BoardFeature.BoolFlag.Adc24bit_AD7175_Enable)
    {
        //SSP1 for ADC    AD7175
        LBT21XXX_SSP1_ADC_7175.uc_Module_Name = SSP1_DEVICE1;    //name
        LBT21XXX_SSP1_ADC_7175.uc_Speed = 8;
        LBT21XXX_SSP1_ADC_7175.uc_CPOL = 1;
        LBT21XXX_SSP1_ADC_7175.uc_CPHA = 1;
        LBT21XXX_SSP1_ADC_7175.uc_MSTR = 1;    //master
        LBT21XXX_SSP1_ADC_7175.uc_BITS = 8;
        LBT21XXX_SSP1_ADC_7175.uc_LSBF = 0;


    }
}
void LBT21XXX_Base::DAC_SPI_Set(void)
{
    if(BoardFeature.BoolFlag.DAC16bit_AD5360_Enable)
    {
        //SSP0 for DAC    AD5360
        LBT21XXX_SSP0_DAC_5360.uc_Module_Name = SSP0_DEVICE1;    //name
        LBT21XXX_SSP0_DAC_5360.uc_Speed = 7;//7;
        LBT21XXX_SSP0_DAC_5360.uc_CPOL = 0;
        LBT21XXX_SSP0_DAC_5360.uc_CPHA = 1;
        LBT21XXX_SSP0_DAC_5360.uc_MSTR = 1;    //master
        LBT21XXX_SSP0_DAC_5360.uc_BITS = 8;
        LBT21XXX_SSP0_DAC_5360.uc_LSBF = 0;
    }
    if(BoardFeature.BoolFlag.DAC16bit_AD5754_Enable)
    {
        //SSP0 for DAC    AD5754
        LBT21XXX_SSP0_DAC_5754.uc_Module_Name = SSP0_DEVICE3;    //name
        LBT21XXX_SSP0_DAC_5754.uc_Speed = 7;//5;
        LBT21XXX_SSP0_DAC_5754.uc_CPOL = 0;
        LBT21XXX_SSP0_DAC_5754.uc_CPHA = 1;
        LBT21XXX_SSP0_DAC_5754.uc_MSTR = 1;    //master
        LBT21XXX_SSP0_DAC_5754.uc_BITS = 8;
        LBT21XXX_SSP0_DAC_5754.uc_LSBF = 0;
    }

}
void LBT21XXX_Base::FAN_Init(void)
{
    if(BoardFeature.BoolFlag.FAN_IIC_Enable)
    {
        if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
        {
            // 风扇初始化
            ///////风扇调速  初始化时设定为100%
            MsTime MyMsTime;
            unsigned long long TimeTicks;
// 	        MyMsTime.Second =LBT21XXX_Base::FanOverTime.Second;
// 	        MyMsTime.Us100 = LBT21XXX_Base::FanOverTime.Us100;
            MyMsTime = CommonFunc::GetSystemTime();//OS_Time;
            while(1)
            {
                if(PCA9501::SetExpander(34,1))
                {
                    I2CBusNew::AD5245_SET(0,0,I2C2);
                    PCA9501::SetExpander(34,0);
                    safety_check::Fan_FaultCheckFlag=0;
                    break;
                }
                else
                {
                    TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
                    if(TimeTicks >= 1000)
                    {
                        safety_check::Fan_FaultCheckFlag=0;
                        break;
                    }
                    else
                        continue;
                }
            }
        }
    }
    if(BoardFeature.BoolFlag.FAN_PWM_Enable)
    {
        board_config::bFanAdj=1;

        FanAdj_task::AdjMod=PWMMOD;
			  if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044)
					LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,20,100);
				else
					LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,20,1000);
    }
}

void LBT21XXX_Base::IO_Init(void)
{
    for(int i=0; i<LBT21XXX_Total_Pins; i++)
    {
        ControlPin::SetIObyCP(PinGroup[i]);
    }

    for(int i=0; i<LBT21XXX_ADC5560_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup_ADC[i]);


    if(BoardFeature.BoolFlag.DAC16bit_AD5754_Enable)
    {
        for(int i=0; i<LBT21XXX_DAC5754_Total_Pins; i++)
            ControlPin::SetIObyCP(PinGroup_DAC1[i]);

    }
    if(BoardFeature.BoolFlag.DAC16bit_AD5360_Enable)
    {
        for(int i=0; i<LBT21XXX_DAC5360_Total_Pins; i++)
            ControlPin::SetIObyCP(PinGroup_DAC2[i]);
    }

    ControlPin::SetIObyCP(board_config::Pin_ACIM);
}

void LBT21XXX_Base::ADC_Init(void)
{
    LBT21XXX_ADC5DAC::Adc_Init();
    TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}
void LBT21XXX_Base::DAC_Init(void)
{
    LBT21XXX_ADC5DAC::Dac_Init();

    TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}

void LBT21XXX_Base::Software_Init(void)
{
    Func_Init();//函数指针初始化，四个基本操作
	
	
	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = 0.3f;
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = BoardFeature.FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = BoardFeature.FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = BoardFeature.FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fParaWaitTime = BoardFeature.FINE_TUNE_PARAMasterWaitTime;

    LBT21XXX_SampleManage::Init();

    IR::pIR_ADC = &(LBT21XXX_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(LBT21XXX_SampleManage::renew_IR_ADC);
    IR::pIR_Trigger_ADC = &(LBT21XXX_SampleManage::IR_Trigger_ADC);
// 	IR::IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(LBT21XXX_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=LBT21XXXSETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=LBT21XXXSETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=LBT21XXXSETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=LBT21XXXSETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=BoardFeature.LBT21XXXMINIMUMPLSE;

}

void LBT21XXX_Base::Func_Init(void)
{
    // 	board_config::Func_Adc_rd=&LBT21XXX_Base::ADC_rd;
    board_config::Func_Dac_out=&LBT21XXX_Base::DAC_out;
    board_config::Func_SetRange=&LBT21XXX_Base::SetRange;
    board_config::Func_SetRelay=&LBT21XXX_Base::SetRelay;
    board_config::Func_Sample = &LBT21XXX_SampleManage::SampleISR;
    if(BoardFeature.BoolFlag.UnitDIDO_Enable)
    {
        board_config::Func_UnitDO=&LBT21XXX_Base::OUTPUT_UnitDO;
        board_config::Func_UnitDI=&LBT21XXX_Base::Read_UnitDI;
        board_config::Func_UnitDI_ReadAll = &LBT21XXX_Base::UnitDI_ReadAll;
        board_config::Func_UnitDO_Reset = &LBT21XXX_Base::UnitDO_Reset;
    }

    board_config::Func_RedundantSafety= &LBT21XXX_Base::RedundantSafety; //冗余电压保护
    board_config::Func_ReleaseRedundantSafety=&LBT21XXX_Base::ReleaseRedundantSafety;//冗余解除操作
    board_config::Func_FanAdj=&LBT21XXX_Base::FanAdjOperate;
    //#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
        board_config::Func_FanSafety= &LBT21XXX_Base::FanCheckSafety; //风扇保护
    //#endif
    board_config::FeedWatchDog = &Feed_CpldWD;
    PowerLostCheck::Func_PowerOffSafetCheck = &LBT21XXX_Base::PowerOffCheck;
    //#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
    {
        M0RedundantFunc::M0_RedundantMap.m_Type = eType_Voltage;
        M0RedundantFunc::M0_RedundantMap.m_fAdcGain = M0_ADC_Gain;
        M0RedundantFunc::M0_RedundantMap.m_fAdcOffset = M0_ADC_Offset;
        // place here
        St_SaftyCheckItem safetyInfo;
        for(unsigned char IV_ChanIdx = 0; IV_ChanIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; IV_ChanIdx++)
        {
            M0RedundantFunc::M0_RedundantMap.m_bValid[IV_ChanIdx] = M0RedundantFunc::m_M0MapChnValid[IV_ChanIdx];
            M0RedundantFunc::GetM0SafetyInfo(IV_ChanIdx,M0RedundantFunc::M0_RedundantMap.m_Type,&safetyInfo);
            M0RedundantFunc::M0_RedundantMap.m_fSafetyH[IV_ChanIdx] = safetyInfo.m_fHigh;
            M0RedundantFunc::M0_RedundantMap.m_fSafetyL[IV_ChanIdx] = safetyInfo.m_fLow;
            M0RedundantFunc::M0_RedundantMap.m_ChanNo[IV_ChanIdx] = M0RedundantFunc::m_M0MapTable[IV_ChanIdx];
            M0RedundantFunc::m_bIsM0ReleaseCheck[IV_ChanIdx] = true;
            M0RedundantFunc::m_bClampValid[IV_ChanIdx] = false;
        }
        M0RedundantFunc::m_RedundantChnMap[M0RedundantFunc::M0_RedundantMap.m_Type]  = M0RedundantFunc::M0_RedundantMap;
    }
    if(board_config::DacInfo.m_bHaveBuildInVolt == 1)
    {
        board_config::Func_BuildInVolt_ReadAll = &ReadAllBuildInVolt;
        if( (ChannelInfo_Manager::BoardCFG.m_Global.m_uBuildInVoltCount > 0)
                &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044) )
        {   //190524,hpq
            BuiltIn_DataManager::b_BuiltInAux = 1;
            BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV = 1;
            BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = 1;
            //ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN = 1;
        }
    }
    //#endif
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
char LBT21XXX_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    unsigned long value = Data;    //强制保护
    unsigned char chan = ChannelIdx;
    chan = ChannelIdxChange(ChannelIdx,type);
    switch(type)
    {
    case ACTIVE_DAC_CUR:
//         if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
//         {
//             if(LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS != 0)
//             {
//                 LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS = 0;
//                 CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
//             }
//         }
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = value;    //此值为偏移码
        if(BoardFeature.BoolFlag.ACTIVE_DAC_CUR_NeedNegation_Enable)
            value = 0xFFFFFF-value;

        LBT21XXX_ADC5DAC::Dac_out_Cur(chan,value);
        break;

        //comment out chen
    case ACTIVE_DAC_VLT:
//         if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
//         {
//             if(LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS != 1)
//             {
//                 LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS = 1;
//                 CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
//             }
//         }
        if(BoardFeature.BoolFlag.ACTIVE_DAC_VLT_NeedNegation_Enable)
            value = 0xFFFFFF-value;

        if(BoardFeature.BoolFlag.Have_DAC_Vlt_Enable)
            LBT21XXX_ADC5DAC::Dac_out_Vlt(chan,value);
        break;
        ///////////////////钳位
    case ACTIVE_DAC_VH:

    if(BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable)
		{
			if(!(ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(ChannelIdx) || ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()))
			  value = 0xFFFFFF;
    }

		if(BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable)
			value = 0xFFFFFF-value;

		if(BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable)
		  LBT21XXX_ADC5DAC::Dac_out_Clamp_Vlt(chan,value);    //VOUTA - DACH

		break;
    case ACTIVE_DAC_VL:
        if(BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable)
        {
            if(!(ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(ChannelIdx) || ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()))
            {
                if(type == ACTIVE_DAC_VH)
                    value = 0xFFFFFF;
                else
                    value = 0;
            }
        }
        if(BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable)
            value = 0xFFFFFF-value;

        if(BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable)
            LBT21XXX_ADC5DAC::Dac_out_Clamp_Vlt(chan,value);    //VOUTA - DACH

        break;
    default:
        break;
    }
    return 1;
}
unsigned long LBT21XXX_Base::ChannelIdxChange(unsigned  long SourceChannel,unsigned char type)
{
    unsigned long chan = SourceChannel;

    switch(type)
    {
    case ACTIVE_DAC_CUR:
        switch(BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode)
        {
        case LBT21XXX_Chan_JumpMode_21084CUR:
            if(chan >= 4)
                chan += 4;
            break;
        case LBT21XXX_Chan_JumpMode_21024CUR:
            chan+=2;
            break;
        case LBT21XXX_Chan_JumpMode_MSTAT21044CUR:
            chan*=4;
            break;
		case LBT21XXX_Chan_JumpMode_21044CUR:
        default:
            break;

        }
        break;
    case ACTIVE_DAC_VLT:
        switch(BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode)
        {
        case LBT21XXX_Chan_JumpMode_21084CUR:
            if(chan < 4)
                chan += 4;
			else
				chan += 8;
            break;
        case LBT21XXX_Chan_JumpMode_MSTAT21044VLT:
            chan = chan*4+1;
            break;
        case LBT21XXX_Chan_JumpMode_20084VLT:
            if(chan < 4)
                chan += 4;
            else if(chan >= 4)
                chan += 8;
            break;
		case LBT21XXX_Chan_JumpMode_21044VLT:
			if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044) && (ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
			{
				chan += 8;
            }
					
			break;
        default:
            break;

        }
        break;
    case ACTIVE_DAC_VH:
        switch(BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode)
        {
        case LBT21XXX_Chan_JumpMode_21024VH:
            if(chan==0)
                chan=9;
            else
                chan=11;
            break;
        case LBT21XXX_Chan_JumpMode_21084VH:
            chan = 0;
            break;
        case LBT21XXX_Chan_JumpMode_MSTAT21044VH:			
            chan = chan*4+3;
		    break;
		case LBT21XXX_Chan_JumpMode_21044VH:
		    if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044) && (ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
			{
				chan += 12;
            }
            break;
        default:
            break;

        }
        break;
    case ACTIVE_DAC_VL:
        switch(BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode)
        {
        case LBT21XXX_Chan_JumpMode_21024VL:
            if(chan==0)
                chan=8;
            else
                chan=10;
            break;
        case LBT21XXX_Chan_JumpMode_21084VL:
            chan = 1;
            break;
        case LBT21XXX_Chan_JumpMode_MSTAT21044VL:
            chan = chan*4+2;
            break;
		case LBT21XXX_Chan_JumpMode_21044VL:
			if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044) && (ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
			{
				chan += 4;
			}
			break;
        default:
            break;

        }
        break;
    default:
        break;
    }
    return chan;
}

char LBT21XXX_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
    switch(CurrRange)
    {
    case CURRENT_RANGE_1:
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CH = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        LBT21XXX_relaystatus[ChannelIdx].Reg &= ~(LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CL | LBT21XXX_RLY_R_CLL);
        break;
    case CURRENT_RANGE_2:
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CM = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        LBT21XXX_relaystatus[ChannelIdx].Reg &= ~(LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CL | LBT21XXX_RLY_R_CLL);
        break;
    case CURRENT_RANGE_3:
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CL = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        LBT21XXX_relaystatus[ChannelIdx].Reg &= ~(LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CLL);
        break;
    case CURRENT_RANGE_4:
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CLL = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        LBT21XXX_relaystatus[ChannelIdx].Reg &= ~(LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CL);
        break;
    default:
        LBT21XXX_relaystatus[ChannelIdx].Reg &= ~(LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CL | LBT21XXX_RLY_R_CLL);
        break;
    }
    I_ADG_Address_Change(ChannelIdx, CurrRange);

    ARBUS_DELAY(10);
    CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    if(BoardFeature.BoolFlag.R_VM_Enable)
    {
        if( (LBT21XXX_relaystatus[ChannelIdx].Reg & (LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CL | LBT21XXX_RLY_R_CLL)) == 0x0000)	// if CH=0 & CM=0 & CL=0 & CLL=0
        {

            if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
            {
                LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 0;
            }
            else
            {
                LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 1;
            }
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }
    return 1;
}
void LBT21XXX_Base::I_ADG_Address_Change(unsigned char ChannelIdx, unsigned char Current_Range)
{
    switch (BoardFeature.I_ADG_Address_Change_Mode)
    {
    case LBT21XXX_I_ADG_Address_Change_Mode_None:
        break;
    case LBT21XXX_I_ADG_Address_Change_Mode_21084:
        LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]= (Current_Range == CURRENT_RANGE_1? 32+ChannelIdx : ChannelIdx);
        break;
    case LBT21XXX_I_ADG_Address_Change_Mode_21024_VA_VB:
        LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]= (Current_Range > CURRENT_RANGE_4? 64+ChannelIdx : Current_Range*32+ChannelIdx);
        break;
    case LBT21XXX_I_ADG_Address_Change_Mode_21024_VC_VD:
        switch(Current_Range)
        {
        case CURRENT_RANGE_1:
            LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]=ChannelIdx;
            break;
        case CURRENT_RANGE_2:
            LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]= 32+ChannelIdx;
            break;
        case CURRENT_RANGE_3:
            LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]= 64+ChannelIdx;
            break;
        case CURRENT_RANGE_4:
            LBT21XXX_SampleManage::I_ADG_Address[ChannelIdx]= 96+ChannelIdx;
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }
}
void LBT21XXX_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_LBT21XXX_Relay2 MyRly;
    if(BoardFeature.BoolFlag.R_VM_Enable)
    {
        if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = bAutoCali;
        }
		else if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084) &&(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
		{			
		     if(ChannelInfo_Manager::m_st_BoadType.m_PowerLevel == POWER_LEVEL_HC)
			 {
				 LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM =1;
			 }
			 else
			 {
				  LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = ~bAutoCali;
			 }
		}
        else
        {
          if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21044) \
						&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21024) \
						&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21014))
					{				
						LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = ~bAutoCali;
					}
        }
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        if(bAutoCali)
            ControlPin::SetMeHigh(board_config::Pin_ACIM);
    }

    MyRly.m_Reg = SetRly;
    if(MyRly.m_Bits.Protect)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.P_R = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(BoardFeature.BoolFlag.ACIM_Enable)
    {
        if(MyRly.m_Bits.Acim)
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.R_ACIM = 1;
            if(ChannelInfo_Manager::m_st_BoadType.m_PowerLevel== 1)	// This board is 2108MC or 2108HC, with special ACIM control
            {
                Step_Manager_task::ACIM_CH_ON = ChannelIdx;
                CPLD_FUNC::WriteSpecialACIM(ChannelIdx,1);
            }
            else	// This board is 2108LC, with common ACIM control(one bit for each ACIM relay)
                CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }
    if(MyRly.m_Bits.CHON)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CHON = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(MyRly.m_Bits.Ivs)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_Running_Flag != CHN_LOCK_THIRD_PARTY)
    {
        if((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOn))
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.Damping = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
        else if ((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOff))
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.Damping = 0;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }

    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Protect)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.P_R = 0;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(BoardFeature.BoolFlag.ACIM_Enable)
    {
        if(MyRly.m_Bits.Acim)
        {
            LBT21XXX_relaystatus[ChannelIdx].Bits.R_ACIM = 0;
            if(ChannelInfo_Manager::m_st_BoadType.m_PowerLevel== 1)	// This board is 2108MC or 2108HC, with special ACIM control
            {
                if(Step_Manager_task::ACIM_CH_ON == ChannelIdx)	// 如果待关闭的通道不是已打开的通道，直接跳过
                {
                    Step_Manager_task::ACIM_CH_ON = 8;	// 无ACIM通道连接
                    CPLD_FUNC::WriteSpecialACIM(ChannelIdx,0);
                }
            }
            else	// This board is 2108LC, with common ACIM control(one bit for each ACIM relay)
                CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }
    if(MyRly.m_Bits.CHON)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_CHON = 0;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(MyRly.m_Bits.Output)
    {
        unsigned short relay[4] = {LBT21XXX_RLY_R_CH, LBT21XXX_RLY_R_CM, LBT21XXX_RLY_R_CL, LBT21XXX_RLY_R_CLL};
        for(int i=0; i<4; i++)
        {
            LBT21XXX_relaystatus[ChannelIdx].Reg &= ~relay[i];	// CH=0 then CM=0 then CL=0 then CLL=0
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }
    if(BoardFeature.BoolFlag.R_VM_Enable)
    {
        if( (LBT21XXX_relaystatus[ChannelIdx].Reg & (LBT21XXX_RLY_R_CH | LBT21XXX_RLY_R_CM | LBT21XXX_RLY_R_CL | LBT21XXX_RLY_R_CLL)) == 0x0000)	// if CH=0 & CM=0 & CL=0 & CLL=0
        {
            if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
            {
                LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 0;
            }
            else
            {
                LBT21XXX_relaystatus[ChannelIdx].Bits.R_VM = 1;
            }
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
        }
    }
    if(MyRly.m_Bits.Ivs)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.R_IVS = 0;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }
    if(MyRly.m_Bits.Damping)
    {
        LBT21XXX_relaystatus[ChannelIdx].Bits.Damping = 0;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21XXX_relaystatus[ChannelIdx].Reg);
    }

}

void LBT21XXX_Base::Adjust_ADC_Gain(char type,char chnType)
{
    float VF =  ChannelInfo_Manager::BoardCFG.m_Channel[0].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;

    if(chnType == ADC_TYPE_I)
    {
        if(type == 1)    //单倍
        {
            if(ChannelInfo_Manager::m_st_BoadType.m_PowerLevel== 1)
            {
                if(VF<=5)
                {
                    cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);
                    ARBUS_DELAY(5);
                }
                else if((VF>5)&&(VF<=10))
                {
                    cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x29A);
                    ARBUS_DELAY(5);
                }
                else if(VF>10)
                {
                    cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x21A);
                    ARBUS_DELAY(5);
                }
            }
            else
            {
				if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType ==BOARDTYPE_MSTAT21044)
				{
				   cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x299);				
				}
                else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType ==BOARDTYPE_LBT20084)
                {
					if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);
					else
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x299);
                }
                else
                {
                    cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);  //modify by qjm20140925
                }

                ARBUS_DELAY(5);
            }
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

void LBT21XXX_Base::ADC_ADG_open(unsigned char ChannelIdx)
{
    CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

// void LBT21XXX_Base::ADG_Reset(void)
// {

// }

void LBT21XXX_Base::changeADCRange(unsigned char range)
{
    ARBUS_DELAY(10);

    LBT21XXX_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void LBT21XXX_Base::Feed_CpldWD(void)
{
    ControlPin::SetMeHigh(LBT21XXX_Base::PinGroup[LBT21XXX_Pin_WD]);
    TIMERSFunc::DELAY_SYS(200, 0);
    ControlPin::SetMeLow(LBT21XXX_Base::PinGroup[LBT21XXX_Pin_WD]);
}


char LBT21XXX_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
{
    unsigned short int temp = 0;

    if(Value>0)
    {
        temp = ~(1<<CH_Number);   //保持和Arbin定义的DO规则 即 DO置1时，外部触点闭合
        CPLD_DO.Reg &= temp;

// 	    CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
//         ARBUS_DELAY(10);
        ChannelInfo_Manager::BuildIn_DO[CH_Number][0] = 1;
    }
    else
    {
        temp = (1<<CH_Number);    ////保持和Arbin定义的DO规则 即 DO置0时，外部触点断开
        CPLD_DO.Reg |= temp;

//         CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
//         ARBUS_DELAY(10);
        ChannelInfo_Manager::BuildIn_DO[CH_Number][0] = 0;
    }

    CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
    ARBUS_DELAY(10);
    return 1;
}

char LBT21XXX_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
    char ReturnValue=0;
    unsigned short int temp = 0;
    if(DI_Idx >= MAX_BUILDIN_DI_COUNT)
        return 0;

    temp =  CPLD_FUNC::Read(NBUS_CPLD2_DI);
    temp &= 0X00FF;
    temp = (temp>>CH_Number) & 0X0001;

    ReturnValue= temp;

    return ReturnValue;
}

void LBT21XXX_Base::UnitDI_ReadAll(void)
{
    unsigned short int temp = CPLD_FUNC::Read(NBUS_CPLD2_DI);
    CPLD_DI.Reg = temp;
    temp  &= 0X00FF;
    for(unsigned char uc_i=0; uc_i<8; uc_i++) //8为最大通道数	//161125 zyx
    {
        if((temp>>uc_i)& 0X0001)
        {
            if(Sample_Pro::Digital_SampleCounter[uc_i][0]>3)
            {
                ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_PV_UNIT_DI1].m_fValue=1;  //modified by xp	//161125 zyx
                Sample_Pro::Digital_SampleCounter[uc_i][0]=3;
            }
            Sample_Pro::Digital_SampleCounter[uc_i][0]++;
        }
        else
        {
            Sample_Pro::Digital_SampleCounter[uc_i][0]=0;
            ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_PV_UNIT_DI1].m_fValue=0;		//161125 zyx
        }
    }

}
char LBT21XXX_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    for(unsigned char uc_i=0; uc_i<MAX_BUILDIN_DI_COUNT; uc_i++)
        OUTPUT_UnitDO(uc_i,0,Value); //DKQ modified
    return 0;
}

char  LBT21XXX_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{

    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
        return (safety_check::RedundantSafetyFlg[CH_Number]);
    else
        return (ControlPin::ReadMe(LBT21XXX_Base::PinGroup[LBT21XXX_Pin_RedundantSafety]) == 0? 0 :1);
}

char LBT21XXX_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{

    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
        return (excheck::M0ReleaseRedundantSafety(uc_ChannelNo,type));
    else
        return 1;
}

void LBT21XXX_Base::ReadAllBuildInVolt(void)
{
    for(int ChanIdx = 0; ChanIdx < 4; ChanIdx++)
    {
        if(ChannelInfo_Manager::ChannelInfo[ChanIdx].m_bBuildInVoltNewCode[0])
        {
            ChannelInfo_Manager::ChannelInfo[ChanIdx].m_bBuildInVoltNewCode[0] = 0;
            ChannelInfo_Manager::Insert_BuildInVolt2HistoryData(ChanIdx,0,ChannelInfo_Manager::ChannelInfo[ChanIdx].m_u32BuildInVolt[0]);
        }
    }
}
char LBT21XXX_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    return (excheck::ExFanCheckSafety(PinGroup[LBT21XXX_Pin_FanCheckSignal]));
}


char LBT21XXX_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{

    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }

    LPC4078PWM::PWM_MatchUpdate(PWM_HD0,PWM_CH3, DutyRatio, PWM_MATCH_UPDATE_NOW);
    return 1;
}
unsigned char LBT21XXX_Base::PowerOffCheck(void)
{
    unsigned char temp;
    temp = ControlPin::ReadMe(LBT21XXX_Base::PinGroup[LBT21XXX_Pin_PowerOff]);
    if(BoardFeature.BoolFlag.PowerOffCheck_NeedNegation_Enable)
        return !temp;
    else
        return temp;
}



// void program::test(void)
// {
//  	 NewLBT21084 test;// = new  NewLBT21084;

// 	 NewLBT21084 c;
// 	 c.test1();
// 	 test = &c;
// 	 test->test1();
// 	 test.FeatureEnable_Init();
// 	 test.Hardware_Init();

// }
/*********************************************************************************
**                            End Of File
*********************************************************************************/
