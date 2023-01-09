
#include "../../Entry/includes.h"

#define  LBT22013_FINE_TUNE_STARTTIME          0.2f
#define  LBT22013_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT22013_FINE_TUNE_STEPFACTOR         5E-5  //20E-5
#define  LBT22013_FINE_TUNE_MAXADJUSTFACTOR    0.06f


#define  LBT22013_ACCURACY                     0.0005f   //精度
#define  LBT22013_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT22013_SETTING_TIME                 100//500       //电流上升沿时间，单位为100us
#define  LBT22013_RISING_TIME                  100
#define  LBT22013_Tpid_Tsys           0.3          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

#define LBT22013_VOLTAGE_RANGE_CHANGE_TIME	   30

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT22013_Base::SetCtrlOpenCh[6];
St_DisposableTask LBT22013_Base::SetCtrlCloseCh[5];
St_DisposableTask LBT22013_Base::SetCtrlCloseChToRest[5];
St_DisposableTask LBT22013_Base::SetCCToCC[8];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT22013_Base::SetRstToOff[2];
St_DisposableTask LBT22013_Base::SetOffToRst[2];
St_DisposableTask LBT22013_Base::SetVoltRelay[2];  //增加冗余电压恢复时序 用于中电/Maxwell/ 20140827 dirui
St_DisposableTask LBT22013_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT22013_Base::SetCtrlCloseAcim[1];

St_DisposableTask LBT22013_Base::SetRstToOffChn[1];
St_DisposableTask LBT22013_Base::SetRstToCur[5];
St_DisposableTask LBT22013_Base::SetRstToVlt[6];

St_DisposableTask LBT22013_Base::SetOffToRstChn[1];
St_DisposableTask LBT22013_Base::SetOffToCur[5];
St_DisposableTask LBT22013_Base::SetOffToVlt[6];

St_DisposableTask LBT22013_Base::SetCurToOff[5];
St_DisposableTask LBT22013_Base::SetCurToRst[5];
St_DisposableTask LBT22013_Base::SetCurToCur[9];
St_DisposableTask LBT22013_Base::SetCurToVlt[10];


St_DisposableTask LBT22013_Base::SetVltToOff[4];
St_DisposableTask LBT22013_Base::SetVltToRst[4];
St_DisposableTask LBT22013_Base::SetVltToVlt[10];
St_DisposableTask LBT22013_Base::SetVltToCur[9];
#if (IV_AUTORANGE == 1)
St_DisposableTask LBT22013_Base::SetAutoRangeSwitch[8];
#endif
SSPI_Para  LBT22013_Base::LBT22013_SSP0DAC;
SSPI_Para  LBT22013_Base::LBT22013_SSP1ADC;
#pragma arm section
// unsigned char LBT22013_Base::SetCtrlOpenCh_Len = 6;
// unsigned char LBT22013_Base::SetCtrlCloseCh_Len = 5;
// unsigned char LBT22013_Base::SetCtrlCloseChToRest_Len = 5;
// unsigned char LBT22013_Base::SetCCToCC_Len = 7;
// unsigned char LBT22013_Base::SetRstToOff_Len = 2;
// unsigned char LBT22013_Base::SetOffToRst_Len = 2;
// unsigned char LBT22013_Base::SetVoltRelay_Len=2;

const St_Port LBT22013_Base::PinGroupVA[LBT22013_VA_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{   //Port      Bits       PortType         Function               Address
    {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        4,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
    {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
    {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
    {0,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
    {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
    {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
    {2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
    {1,        6,         eCPTYPE_MCUGPIO,    GPIO_I,               0},//用于中电，MAXWLL
    {3,        16,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {4,        29,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
};
const St_Port LBT22013_Base::PinGroupVB[LBT22013_VB_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
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
    {3,        16,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        17,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.2
    {2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.3
    {2,        5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.5
// 	{0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.22
    {2,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        4,         eCPTYPE_MCUGPIO,   PWM1,                  0},
    {2,        11,        eCPTYPE_MCUGPIO,   GPIO_I,                0}, //P2.11
    {3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,         9,        eCPTYPE_MCUGPIO,   GPIO_I,                0},  //风扇报警检测，0为有效
    {0,        9,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {4,        29,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        7,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        21,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        22,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {4,        28,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {0,        27,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        28,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
};

MsTime     LBT22013_Base::FanOverTime;
volatile St_LBT22013_Relay LBT22013_Base::LBT22013_relaystatus[MAXCHANNELNO];

unsigned char LBT22013_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT22013_Base::m_bHaveClampVdac = 1;
// const unsigned char LBT22013_Base::m_bChannelBasedClampVdac = 0;

unsigned char LBT22013_Base::VoltageRangeFlag[MAXCHANNELNO];  //0:低量程    1：中量程  2：高量程

void LBT22013_Base::Hardware_Init(void)
{
    //unsigned char temp;

    LBT22013_FeatureEnable_Init();
    IO_Init();
    Feed_CpldWD();

    //SSP0 for DAC    AD5754
    LBT22013_SSP0DAC.uc_Module_Name = SSP0_DEVICE3;    //name
    LBT22013_SSP0DAC.uc_Speed = 7;//5;
    LBT22013_SSP0DAC.uc_CPOL = 0;
    LBT22013_SSP0DAC.uc_CPHA = 1;
    LBT22013_SSP0DAC.uc_MSTR = 1;    //master
    LBT22013_SSP0DAC.uc_BITS = 8;
    LBT22013_SSP0DAC.uc_LSBF = 0;

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        board_config::bHaveRedundantCheck = 0;    //LBT22013有冗余电压检查
// 	    board_config::ClampH_Outputflag = 0;
// 	    board_config::ClampL_Outputflag = 0;

        CPLD_FUNC::Init();
        memset((void *)(&LBT22013_relaystatus), 0, sizeof(LBT22013_relaystatus));
        Feed_CpldWD();
        //SSP1 for ADC
        LBT22013_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
        LBT22013_SSP1ADC.uc_Speed = 5;
        LBT22013_SSP1ADC.uc_CPOL = 0;
        LBT22013_SSP1ADC.uc_CPHA = 0;    //rising trig
        LBT22013_SSP1ADC.uc_MSTR = 1;    //master
        LBT22013_SSP1ADC.uc_BITS = 8;
        LBT22013_SSP1ADC.uc_LSBF = 0;

        TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
        Feed_CpldWD();
        LBT22013_relaystatus[0].Bits.R_OFFGH=1;
        CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[0].Reg);  //硬件改变控制时序，改成高有效 20140331

        LBT22013_Base::changeADCRange(CURRENT_RANGE_1);
        CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0x01);//电压控制开   20140331

    }
    else
    {
        ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_H]);

				if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C)
					LBT22013_Base::m_bHaveVoltageDac = 1;
					
    }
		for (unsigned char i=0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
    {
            VoltageRangeFlag[i]=2;   //初始化为高量程
    }
    ADC_Init();
    DAC_Init();
    ControlPin::ReInitPullUP();

    Feed_CpldWD();  // 硬件看门狗复位
    if((ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
			||(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B))
		{
			OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                        &SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
			AdditionalEngine();
		}
		else
		{
			Engine(); 
    }

    //拉高钳压     // 20140613  电压保护
    LBT22013_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
    LBT22013_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);

    LBT22013_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);

    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
    {
        if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        {
            //风扇调速  初始化时设定为100%
            MsTime MyMsTime;
            unsigned long long TimeTicks;
            MyMsTime.Second =LBT22013_Base::FanOverTime.Second;
            MyMsTime.Us100 = LBT22013_Base::FanOverTime.Us100;
            while(1)
            {
                if(PCA9501::SetExpander(34,1))
                {
                    I2CBusNew::AD5245_SET(0,0,I2C2);  //
                    PCA9501::SetExpander(34,0);
                    safety_check::Fan_FaultCheckFlag=0;
                    break;
                }
                else
                {
                    TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
                    if(TimeTicks >= 1000)
                    {
// 				              temp =ControlPin::ReadMe(PinGroup[LBT22013_Pin_FanCheckSignal]);
// 				              if(temp==0)
// 					               safety_check::Fan_FaultCheckFlag=1;
// 				              else
                        safety_check::Fan_FaultCheckFlag=0;
                        break;
                    }
                    else
                        continue;
                }
            }
        }
        else   //VER_B
        {
            safety_check::Fan_FaultCheckFlag=0;
            board_config::bFanAdj=1;
// 	       FanAdj_task::FANADJ_INTERVAL=3000;
// 	       FanAdj_task::VF=63; //gai
// 	       FanAdj_task::VR=0; //gai
// 	       FanAdj_task::HeatSinkCounter=4;
            FanAdj_task::AdjMod=PWMMOD;
// 	       FanAdj_task::PerAdjPower=1;  //gai

            LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,20,1000);
            unsigned char FanCtrlValue[8]= {30,40,50,60,70,80,90,100};
            memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
            memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue[0], 8);
            memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue[0], 8);
            memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue[0], 8);

        }
    }

}
void LBT22013_Base::LBT22013_FeatureEnable_Init(void)
{
    board_config::Feature_Enable_init.Power_Off_Enable =1;
    board_config::Feature_Enable_init.Fan_Safety_Mode =1;
    board_config::Feature_Enable_init.Temperature_Safety_Mode=1;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        board_config::Feature_Enable_init.Redundant_Safety_Mode =1;
        board_config::Feature_Enable_init.RedundantVoltMode =REDUNDANT_M0;
        board_config::Feature_Enable_init.OS_Tick_Interval =800;
    }
    else
    {
//          board_config::Feature_Enable_init.Mcu_PWMFan_Enable=1;
        board_config::Feature_Enable_init.OS_Tick_Interval =200;            //VER_B
    }
}
void LBT22013_Base::IO_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        for(int i=0; i<LBT22013_VA_Total_Pins; i++)
            ControlPin::SetIObyCP(PinGroupVA[i]);
        ControlPin::SetMeHigh(PinGroupVA[LBT22013_VA_Pin_PowerOn]);
    }
    else
    {
        for(int i=0; i<LBT22013_VB_Total_Pins; i++)
            ControlPin::SetIObyCP(PinGroupVB[i]);
        ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_H]);//初始化为高
        ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Prec]);


    }
}

void LBT22013_Base::ADC_Init(void)
{
    LBT22013_ADC5DAC::Adc_Init();
}
void LBT22013_Base::DAC_Init(void)
{
    LBT22013_ADC5DAC::Dac_Init();
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        board_config::DacInfo.m_bCurrRngCount = 3;
    }
    else
        board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = LBT22013_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = LBT22013_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT22013_Tpid_Tsys;

    ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT22013_Base::Software_Init(void)
{
    Func_Init();
    EngineStuff();

    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT22013_FINE_TUNE_STARTTIME;
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT22013_FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT22013_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT22013_FINE_TUNE_STEPFACTOR;

//	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT22013_ACCURACY * LBT22013_CTRL_VALUE_ERROR_FACTOR;  //180614 zyx

    board_config::Accuracy = LBT22013_ACCURACY;

    LBT22013_SampleManage::Init();
    IR::pIR_ADC = &(LBT22013_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(LBT22013_SampleManage::renew_IR_ADC);
// 	  pIR_Trigger_ADC = &(LBT22013_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(LBT22013_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=LBT22013SETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=LBT22013SETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=LBT22013SETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=LBT22013SETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=LBT22013MINIMUMPULSE;
    PowerLostCheck::Func_PowerOffSafetCheck = &LBT22013_Base::PowerOffCheck;
}

void LBT22013_Base::Func_Init(void)
{
    board_config::Func_Dac_out=&LBT22013_Base::DAC_out;
    board_config::Func_SetRange=&LBT22013_Base::SetRange;
    board_config::Func_SetRelay=&LBT22013_Base::SetRelay;
		board_config::Func_Set_Volt_Range = &LBT22013_Base::Set_Volt_Range;
		board_config::Voltage_Range_Change_Time = LBT22013_VOLTAGE_RANGE_CHANGE_TIME;
    board_config::Func_Sample = &LBT22013_SampleManage::SampleISR;
    board_config::Func_RedundantSafety= &LBT22013_Base::RedundantSafety; //冗余电压保护
    board_config::Func_ReleaseRedundantSafety=&LBT22013_Base::ReleaseRedundantSafety;//冗余解除操作
//#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
    {
        board_config::Func_FanSafety= &LBT22013_Base::FanCheckSafety; //风扇保护
    }
    if(board_config::Feature_Enable_init.Temperature_Safety_Mode==1)
    {
        board_config::Func_TemperatureSafety = &LBT22013_Base::TemperatureSafety;
    }
//#endif
    board_config::FeedWatchDog = &Feed_CpldWD;
    if((ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)||
			(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C))
    {
        board_config::Func_FanAdj=&LBT22013_Base::FanAdjOperate;
        board_config::Func_UnitDO=&LBT22013_Base::OUTPUT_UnitDO;
        board_config::Func_UnitDI=&LBT22013_Base::Read_UnitDI;
        board_config::Func_UnitDI_ReadAll = &LBT22013_Base::UnitDI_ReadAll;
        board_config::Func_UnitDO_Reset = &LBT22013_Base::UnitDO_Reset;
    }
}
void LBT22013_Base::Engine(void)
{
  OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr =& LBT22013_Base::SetRstToCur[0];
	OS_LogicProcess::SetCtrlRstToCur.stepLen = sizeof(SetRstToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr =&LBT22013_Base::SetRstToVlt[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr =&LBT22013_Base::SetRstToOffChn[0];;
  OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOffChn)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr =&LBT22013_Base::SetOffToRstChn[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRstChn)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr =& LBT22013_Base::SetOffToCur[0];
	OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetOffToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr =&LBT22013_Base::SetOffToVlt[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr =&LBT22013_Base::SetCurToCur[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCurToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr =&LBT22013_Base::SetCurToVlt[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCurToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr =&LBT22013_Base::SetCurToOff[0];
	OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr =&LBT22013_Base::SetCurToRst[0];
	OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCurToRst)	/	sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr =&LBT22013_Base::SetVltToVlt[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetVltToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr =&LBT22013_Base::SetVltToCur[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetVltToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr =&LBT22013_Base::SetVltToOff[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr =&LBT22013_Base::SetVltToRst[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToRst)	/	sizeof(St_DisposableTask); 


	// ACIM Engine
  OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlOpenAcim[0];
  OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlCloseAcim[0];
  OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);    
 
}
void LBT22013_Base::AdditionalEngine(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        OS_LogicProcess::SetVoltRelay.st_Disposabletask_ptr=&LBT22013_Base::SetVoltRelay[0];
        OS_LogicProcess::SetVoltRelay.stepLen= sizeof(SetVoltRelay)	/	sizeof(St_DisposableTask); //LBT22013_Base::SetVoltRelay_Len;
    }
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
    {
        OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &LBT22013_Base::SetOffToRst[0];
        OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRst)	/	sizeof(St_DisposableTask); //LBT22013_Base::SetOffToRst_Len;

        OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlCloseChToRest[0];
        OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCtrlCloseChToRest)/	sizeof(St_DisposableTask);//LBT22013_Base::SetCtrlCloseChToRest_Len;

        OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlCloseChToRest[0];
        OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetCtrlCloseChToRest)/	sizeof(St_DisposableTask); //LBT22013_Base::SetCtrlCloseChToRest_Len;

        // ACIM Engine
        OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlOpenAcim[0];
        OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

        OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT22013_Base::SetCtrlCloseAcim[0];
        OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
    }		
}
void LBT22013_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
    memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
    memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    memset(&SetOffToRst,     0,    sizeof(SetOffToRst));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        SetCtrlOpenCh[0].uw32_DelayTime = 100;
        SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlOpenCh[1].uw32_DelayTime = 200;
        SetCtrlOpenCh[1].RelayMove.ActiveFlag = (LBT22013_RLY_Protect | LBT22013_RLY_OffGateH);

        SetCtrlOpenCh[2].uw32_DelayTime = 200;
        SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetCtrlOpenCh[3].uw32_DelayTime = 200;
        SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT22013_RLY_Protect;

        SetCtrlOpenCh[4].uw32_DelayTime = 200;
        SetCtrlOpenCh[4].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

        SetCtrlOpenCh[5].uw32_DelayTime = 100;
        SetCtrlOpenCh[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetCtrlCloseCh[0].uw32_DelayTime = 100;
        SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlCloseCh[1].uw32_DelayTime = 200;
        SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;

        SetCtrlCloseCh[2].uw32_DelayTime = 200;
        SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;

        SetCtrlCloseCh[3].uw32_DelayTime = 200;
        SetCtrlCloseCh[3].RelayMove.InactiveFlag = LBT22013_RLY_Output;

        SetCtrlCloseCh[4].uw32_DelayTime = 200;
        SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect;

        SetCCToCC[0].uw32_DelayTime = 100;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetCCToCC[0].RangeChangedFlag = 1;

        SetCCToCC[1].uw32_DelayTime = 200;
        SetCCToCC[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
        SetCCToCC[1].RangeChangedFlag = 1;

        SetCCToCC[2].uw32_DelayTime = 200;
        SetCCToCC[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
        SetCCToCC[2].RangeChangedFlag = 1;

        SetCCToCC[3].uw32_DelayTime = 200;
        SetCCToCC[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[3].RangeChangedFlag = 1;

        SetCCToCC[4].uw32_DelayTime = 200;
        SetCCToCC[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
        SetCCToCC[4].RangeChangedFlag = 1;

        SetCCToCC[5].uw32_DelayTime = 200;
        SetCCToCC[5].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
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
        SetVoltRelay[0].RelayMove.ActiveFlag=LBT22013_RLY_VoltSet;
        //SetVoltRelay[0].DacMove.DacFlag = 0;
        //SetVoltRelay[0].RangeChangedFlag = 0;

        //SetVoltRelay[1].uw32_DelayTime = 0;
        SetVoltRelay[1].RelayMove.InactiveFlag =LBT22013_RLY_VoltSet;
        //SetVoltRelay[1].DacMove.DacFlag = 0;
        //SetVoltRelay[1].RangeChangedFlag = 0;
				#if (IV_AUTORANGE == 1)
		SetAutoRangeSwitch[0].uw32_DelayTime = 100;
        SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetAutoRangeSwitch[0].RangeChangedFlag = 1;

        SetAutoRangeSwitch[1].uw32_DelayTime = 200;
        SetAutoRangeSwitch[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
        SetAutoRangeSwitch[1].RangeChangedFlag = 1;

        SetAutoRangeSwitch[2].uw32_DelayTime = 200;
        SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
        SetAutoRangeSwitch[2].RangeChangedFlag = 1;

        SetAutoRangeSwitch[3].uw32_DelayTime = 200;
        SetAutoRangeSwitch[3].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;
        SetAutoRangeSwitch[3].RangeChangedFlag = 1;

        SetAutoRangeSwitch[4].uw32_DelayTime = 200;
        SetAutoRangeSwitch[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
        SetAutoRangeSwitch[4].RangeChangedFlag = 1;

        SetAutoRangeSwitch[5].uw32_DelayTime = 200;
        SetAutoRangeSwitch[5].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
        SetAutoRangeSwitch[5].RangeChangedFlag = 1;

        SetAutoRangeSwitch[6].uw32_DelayTime = 100;
        SetAutoRangeSwitch[6].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        #endif
    }
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
    {
        memset(&SetCtrlCloseChToRest,     0,    sizeof(SetCtrlCloseChToRest));
//---------------------------------------------//
        SetCtrlOpenCh[0].uw32_DelayTime = 200;
        SetCtrlOpenCh[0].RelayMove.ActiveFlag = LBT22013_RLY_Protect| LBT22013_RLY_DAMPING;
        SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

        SetCtrlOpenCh[1].uw32_DelayTime = 200;
        SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet | LBT22013_RLY_PreC;
        SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCtrlOpenCh[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetCtrlOpenCh[2].uw32_DelayTime = 200;
        SetCtrlOpenCh[2].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetCtrlOpenCh[2].RelayMove.InactiveFlag = LBT22013_RLY_Protect;

        SetCtrlOpenCh[3].uw32_DelayTime = 200;
        SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

        SetCtrlOpenCh[4].uw32_DelayTime = 100;
        SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

        //---------------------------------------------//
        //I to off v TO OFF
        SetCtrlCloseCh[0].uw32_DelayTime = 10;
        SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlCloseCh[1].uw32_DelayTime = 200;
        SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;

        SetCtrlCloseCh[2].uw32_DelayTime = 200;
        SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
        SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetCtrlCloseCh[3].uw32_DelayTime = 200;
        SetCtrlCloseCh[3].RelayMove.InactiveFlag = (LBT22013_RLY_Output | LBT22013_RLY_VoltSet | LBT22013_RLY_PreC);
        SetCtrlCloseCh[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

        //SetCtrlCloseCh[4].uw32_DelayTime = 0;
        SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING | LBT22013_RLY_ACIM;

        //I To rest V TO rest
        SetCtrlCloseChToRest[0].uw32_DelayTime = 10;
        SetCtrlCloseChToRest[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlCloseChToRest[1].uw32_DelayTime = 200;
        SetCtrlCloseChToRest[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
        
        SetCtrlCloseChToRest[2].uw32_DelayTime = 200;
        SetCtrlCloseChToRest[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
				SetCtrlCloseChToRest[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;

        SetCtrlCloseChToRest[3].uw32_DelayTime = 200;
        SetCtrlCloseChToRest[3].RelayMove.InactiveFlag = LBT22013_RLY_Output;
        SetCtrlCloseChToRest[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

        //SetCtrlCloseChToRest[4].uw32_DelayTime = 0;
        SetCtrlCloseChToRest[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
				SetCtrlCloseChToRest[4].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;

        //---------------------------------------------//

        SetCCToCC[0].uw32_DelayTime = 10;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetCCToCC[0].RangeChangedFlag = 1;

        SetCCToCC[1].uw32_DelayTime = 200;
        SetCCToCC[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
        SetCCToCC[1].RangeChangedFlag = 1;

        SetCCToCC[2].uw32_DelayTime = 200;
        SetCCToCC[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;
        SetCCToCC[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetCCToCC[2].RangeChangedFlag = 1;

        SetCCToCC[3].uw32_DelayTime = 100;
        SetCCToCC[3].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;


        SetCCToCC[4].uw32_DelayTime = 200;
        SetCCToCC[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[4].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
        SetCCToCC[4].RangeChangedFlag = 1;

        SetCCToCC[5].uw32_DelayTime = 200;
        SetCCToCC[5].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
        SetCCToCC[5].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetCCToCC[5].RangeChangedFlag = 1;

        SetCCToCC[6].uw32_DelayTime = 200;
        SetCCToCC[6].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
        SetCCToCC[6].RangeChangedFlag = 1;

        SetCCToCC[7].uw32_DelayTime = 100;
        SetCCToCC[7].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetRstToOff[0].uw32_DelayTime = 10;
        //SetRstToOff[0].DacMove.DacFlag = 0;
        SetRstToOff[0].RelayMove.InactiveFlag = LBT22013_RLY_VoltSet | LBT22013_RLY_ACIM | LBT22013_RLY_PreC | LBT22013_RLY_DAMPING;
        //SetRstToOff[0].RangeChangedFlag = 0;

        SetOffToRst[0].uw32_DelayTime = 1000;
        //SetOffToRst[0].DacMove.DacFlag = 0;
        //     SetOffToRst[0].RelayMove.ActiveFlag  |= LBT22013FR_RLY_VoltSet;
        //SetOffToRst[0].RangeChangedFlag = 0;

        SetOffToRst[1].uw32_DelayTime = 10;
        //SetOffToRst[1].DacMove.DacFlag = 0;
        SetOffToRst[1].RelayMove.ActiveFlag  = LBT22013_RLY_VoltSet | LBT22013_RLY_DAMPING;
        //SetOffToRst[1].RangeChangedFlag = 0;

        SetCtrlOpenAcim[0].uw32_DelayTime = 100;
        SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT22013_RLY_ACIM;

        SetCtrlCloseAcim[0].uw32_DelayTime = 100;
        SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT22013_RLY_ACIM;
				#if (IV_AUTORANGE == 1)
		SetAutoRangeSwitch[0].uw32_DelayTime = 200;
        SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetAutoRangeSwitch[0].RangeChangedFlag = 1;

        SetAutoRangeSwitch[1].uw32_DelayTime = 200;
        SetAutoRangeSwitch[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
        SetAutoRangeSwitch[1].RangeChangedFlag = 1;

        SetAutoRangeSwitch[2].uw32_DelayTime = 200;
        SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;
        SetAutoRangeSwitch[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetAutoRangeSwitch[2].RangeChangedFlag = 1;

        SetAutoRangeSwitch[3].uw32_DelayTime = 100;
        SetAutoRangeSwitch[3].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;


        SetAutoRangeSwitch[4].uw32_DelayTime = 200;
        SetAutoRangeSwitch[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetAutoRangeSwitch[4].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
        SetAutoRangeSwitch[4].RangeChangedFlag = 1;

        SetAutoRangeSwitch[5].uw32_DelayTime = 200;
        SetAutoRangeSwitch[5].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
        SetAutoRangeSwitch[5].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
        SetAutoRangeSwitch[5].RangeChangedFlag = 1;

        SetAutoRangeSwitch[6].uw32_DelayTime = 200;
        SetAutoRangeSwitch[6].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
        SetAutoRangeSwitch[6].RangeChangedFlag = 1;

        SetAutoRangeSwitch[7].uw32_DelayTime = 200;
        SetAutoRangeSwitch[7].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
				#endif
    }
		else
		{			
			//---------------------------------------------//
			//rst--cur ok
			SetRstToCur[0].uw32_DelayTime = 200;
			SetRstToCur[0].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;
			SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetRstToCur[1].uw32_DelayTime = 200;
// 			SetRstToCur[1].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
			SetRstToCur[1].RelayMove.ActiveFlag = LBT22013_RLY_PreC;
			SetRstToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
			SetRstToCur[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

			SetRstToCur[2].uw32_DelayTime = 200;
			SetRstToCur[2].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetRstToCur[2].RelayMove.InactiveFlag = LBT22013_RLY_Protect;

			SetRstToCur[3].uw32_DelayTime = 200;
			SetRstToCur[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

			SetRstToCur[4].uw32_DelayTime = 100;
			SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
			
			//rst--Vlt ok
			SetRstToVlt[0].uw32_DelayTime = 200;
			SetRstToVlt[0].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;
      

			SetRstToVlt[1].uw32_DelayTime = 200;
			SetRstToVlt[1].RelayMove.ActiveFlag = LBT22013_RLY_PreC;
			SetRstToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
			SetRstToVlt[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

			SetRstToVlt[2].uw32_DelayTime = 200;
			SetRstToVlt[2].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
      SetRstToVlt[2].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;	
			
			SetRstToVlt[3].uw32_DelayTime = 200;
			SetRstToVlt[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

			SetRstToVlt[4].uw32_DelayTime = 10;
			SetRstToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;    
			
			SetRstToVlt[5].uw32_DelayTime = 100;
			SetRstToVlt[5].RelayMove.ActiveFlag = LBT22013_RLY_IVS;		
			
			//rst--off ok
			SetRstToOffChn[0].uw32_DelayTime = 10;    
			SetRstToOffChn[0].RelayMove.InactiveFlag =  LBT22013_RLY_PreC | LBT22013_RLY_ACIM | LBT22013_RLY_DAMPING;
		 
			//off--rst ok
			SetOffToRstChn[0].uw32_DelayTime = 10;    
			SetOffToRstChn[0].RelayMove.ActiveFlag  =  LBT22013_RLY_DAMPING;
			
			//off--cur	ok		
			SetOffToCur[0].uw32_DelayTime = 200;
			SetOffToCur[0].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;
			SetOffToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetOffToCur[1].uw32_DelayTime = 200;
// 			SetOffToCur[1].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
			SetOffToCur[1].RelayMove.ActiveFlag = LBT22013_RLY_PreC;
			SetOffToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
			SetOffToCur[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

			SetOffToCur[2].uw32_DelayTime = 200;
			SetOffToCur[2].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetOffToCur[2].RelayMove.InactiveFlag = LBT22013_RLY_Protect;

			SetOffToCur[3].uw32_DelayTime = 200;
			SetOffToCur[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

			SetOffToCur[4].uw32_DelayTime = 100;
			SetOffToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
			
			//off--Vlt ok			
			SetOffToVlt[0].uw32_DelayTime = 200;
			SetOffToVlt[0].RelayMove.ActiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING;	

			SetOffToVlt[1].uw32_DelayTime = 200;
			SetOffToVlt[1].RelayMove.ActiveFlag = LBT22013_RLY_PreC;
			SetOffToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
			SetOffToVlt[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

			SetOffToVlt[2].uw32_DelayTime = 200;
			SetOffToVlt[2].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
			SetOffToVlt[2].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;	

			SetOffToVlt[3].uw32_DelayTime = 200;
			SetOffToVlt[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;

			SetOffToVlt[4].uw32_DelayTime = 10;
			SetOffToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;    
			
			SetOffToVlt[5].uw32_DelayTime = 100;
			SetOffToVlt[5].RelayMove.ActiveFlag = LBT22013_RLY_IVS;		
			
			//CC-CC OK			
			SetCurToCur[0].uw32_DelayTime = 10;
      SetCurToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
      SetCurToCur[0].RangeChangedFlag = 1;

      SetCurToCur[1].uw32_DelayTime = 200;
      SetCurToCur[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
      SetCurToCur[1].RangeChangedFlag = 1;
			
			SetCurToCur[2].uw32_DelayTime = 10;
			SetCurToCur[2].RelayMove.ActiveFlag = LBT22013_RLY_DAMPING;

      SetCurToCur[3].uw32_DelayTime = 100;
      SetCurToCur[3].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
      SetCurToCur[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
      SetCurToCur[3].RangeChangedFlag = 1;

      SetCurToCur[4].uw32_DelayTime = 90;

      SetCurToCur[5].uw32_DelayTime = 200;
      SetCurToCur[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
//       SetCurToCur[5].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
      SetCurToCur[5].RangeChangedFlag = 1;

      SetCurToCur[6].uw32_DelayTime = 200;
      SetCurToCur[6].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
      SetCurToCur[6].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
      SetCurToCur[6].RangeChangedFlag = 1;

      SetCurToCur[7].uw32_DelayTime = 200;
      SetCurToCur[7].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
      SetCurToCur[7].RangeChangedFlag = 1;

      SetCurToCur[8].uw32_DelayTime = 100;
      SetCurToCur[8].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
			
			//CC-CV	 OK
			SetCurToVlt[0].uw32_DelayTime = 10;
			SetCurToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
			SetCurToVlt[0].RangeChangedFlag = 1;

			SetCurToVlt[1].uw32_DelayTime = 200;
			SetCurToVlt[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
			SetCurToVlt[1].RangeChangedFlag = 1;
			
			SetCurToVlt[2].uw32_DelayTime = 10;
			SetCurToVlt[2].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;

			SetCurToVlt[3].uw32_DelayTime = 100;
			SetCurToVlt[3].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
			SetCurToVlt[3].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetCurToVlt[3].RangeChangedFlag = 1;

			SetCurToVlt[4].uw32_DelayTime = 90;
			
			SetCurToVlt[5].uw32_DelayTime = 200;
			SetCurToVlt[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
// 			SetCurToVlt[5].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
			SetCurToVlt[5].RangeChangedFlag = 1;

			SetCurToVlt[6].uw32_DelayTime = 200;
			SetCurToVlt[6].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
			SetCurToVlt[6].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML  | LBT22013_RLY_OffGateHH;
			SetCurToVlt[6].RangeChangedFlag = 1;

			SetCurToVlt[7].uw32_DelayTime = 200;
			SetCurToVlt[7].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
			SetCurToVlt[7].RangeChangedFlag = 1;
			
			SetCurToVlt[8].uw32_DelayTime = 10;		
			SetCurToVlt[8].DacMove.DacFlag = ACTIVE_DAC_VLT| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

			SetCurToVlt[9].uw32_DelayTime = 100;
			SetCurToVlt[9].RelayMove.ActiveFlag = LBT22013_RLY_IVS;    
			
			
			
			//CC-OFF ok
			SetCurToOff[0].uw32_DelayTime = 10;
			SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

			SetCurToOff[1].uw32_DelayTime = 200;
			SetCurToOff[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;

			SetCurToOff[2].uw32_DelayTime = 200;
			SetCurToOff[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
			SetCurToOff[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;

			SetCurToOff[3].uw32_DelayTime = 200;
			SetCurToOff[3].RelayMove.InactiveFlag = LBT22013_RLY_Output | LBT22013_RLY_PreC ;
			SetCurToOff[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetCurToOff[4].uw32_DelayTime = 0;
			SetCurToOff[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING | LBT22013_RLY_ACIM ;
			
			//CC-RST ok
			SetCurToRst[0].uw32_DelayTime = 10;
			SetCurToRst[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

			SetCurToRst[1].uw32_DelayTime = 200;
			SetCurToRst[1].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;

			SetCurToRst[2].uw32_DelayTime = 200;
			SetCurToRst[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
			SetCurToRst[2].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;

			SetCurToRst[3].uw32_DelayTime = 200;
			SetCurToRst[3].RelayMove.InactiveFlag = LBT22013_RLY_Output;
			SetCurToRst[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetCurToRst[4].uw32_DelayTime = 0;			
			SetCurToRst[4].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
			SetCurToRst[4].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;
			
			//CV-CV OK
			SetVltToVlt[0].uw32_DelayTime = 200;
			SetVltToVlt[0].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
			SetVltToVlt[0].RangeChangedFlag = 1;

			SetVltToVlt[1].uw32_DelayTime = 10;
			SetVltToVlt[1].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;

			SetVltToVlt[2].uw32_DelayTime = 100;
			SetVltToVlt[2].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
			SetVltToVlt[2].RelayMove.InactiveFlag = LBT22013_RLY_IVS | LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToVlt[2].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
			SetVltToVlt[2].RangeChangedFlag = 1;

			SetVltToVlt[3].uw32_DelayTime = 90;
			
			SetVltToVlt[4].uw32_DelayTime = 200;
			SetVltToVlt[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
// 			SetVltToVlt[4].RelayMove.ActiveFlag = LBT22013_RLY_VoltSet;
			SetVltToVlt[4].RangeChangedFlag = 1;

			SetVltToVlt[5].uw32_DelayTime = 200;
			SetVltToVlt[5].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
			SetVltToVlt[5].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToVlt[5].RangeChangedFlag = 1;

			SetVltToVlt[6].uw32_DelayTime = 200;
			SetVltToVlt[6].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
			SetVltToVlt[6].RangeChangedFlag = 1;

			SetVltToVlt[7].uw32_DelayTime = 10;
			SetVltToVlt[7].DacMove.DacFlag = ACTIVE_DAC_VLT| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
			
			SetVltToVlt[8].uw32_DelayTime = 10;
			SetVltToVlt[8].RelayMove.ActiveFlag = LBT22013_RLY_IVS;
			SetVltToVlt[8].RangeChangedFlag = 1;
			
			SetVltToVlt[9].uw32_DelayTime = 90;
			
			
			//CV-CC OK
			SetVltToCur[0].uw32_DelayTime = 200;
			SetVltToCur[0].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
			SetVltToCur[0].RangeChangedFlag = 1;
			
			SetVltToCur[1].uw32_DelayTime = 100;
			SetVltToCur[1].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;
				
			SetVltToCur[2].uw32_DelayTime = 10;
			SetVltToCur[2].DacMove.DacFlag = ACTIVE_DAC_CUR;
			SetVltToCur[2].RelayMove.InactiveFlag = LBT22013_RLY_IVS;
			
			SetVltToCur[3].uw32_DelayTime = 10;
			SetVltToCur[3].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
			
			SetVltToCur[4].uw32_DelayTime = 200;
			SetVltToCur[4].RelayMove.ActiveFlag = LBT22013_RLY_Protect;
			SetVltToCur[4].RelayMove.InactiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToCur[4].RangeChangedFlag = 1;
			
			SetVltToCur[5].uw32_DelayTime = 200;
			SetVltToCur[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
			SetVltToCur[5].RangeChangedFlag = 1;
			
			SetVltToCur[6].uw32_DelayTime = 200;
			SetVltToCur[6].RelayMove.InactiveFlag = LBT22013_RLY_Protect;
			SetVltToCur[6].RelayMove.ActiveFlag = LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToCur[6].RangeChangedFlag = 1;
			
			SetVltToCur[7].uw32_DelayTime = 190;
			SetVltToCur[7].RelayMove.InactiveFlag = LBT22013_RLY_OffGateH;
			SetVltToCur[7].RangeChangedFlag = 1;
			
			SetVltToCur[8].uw32_DelayTime = 1;
			
			//CV-OFF ok
			SetVltToOff[0].uw32_DelayTime = 200;
			SetVltToOff[0].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
			
			SetVltToOff[1].uw32_DelayTime = 200;
			SetVltToOff[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VLT_ZERO;			
			SetVltToOff[1].RelayMove.InactiveFlag = LBT22013_RLY_IVS | LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToOff[1].RelayMove.ActiveFlag = LBT22013_RLY_Protect;  

			SetVltToOff[2].uw32_DelayTime = 200;
			SetVltToOff[2].RelayMove.InactiveFlag = (LBT22013_RLY_Output | LBT22013_RLY_PreC);
			SetVltToOff[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetVltToOff[3].uw32_DelayTime = 0;
			SetVltToOff[3].RelayMove.InactiveFlag = LBT22013_RLY_Protect | LBT22013_RLY_DAMPING | LBT22013_RLY_ACIM;
			
			//CV-RST	ok		
			SetVltToRst[0].uw32_DelayTime = 200;
			SetVltToRst[0].RelayMove.ActiveFlag = LBT22013_RLY_OffGateH;
			
			SetVltToRst[1].uw32_DelayTime = 200;
			SetVltToRst[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VLT_ZERO;			
			SetVltToRst[1].RelayMove.InactiveFlag = LBT22013_RLY_IVS | LBT22013_RLY_OffGateML | LBT22013_RLY_OffGateHH;
			SetVltToRst[1].RelayMove.ActiveFlag = LBT22013_RLY_Protect;  

			SetVltToRst[2].uw32_DelayTime = 200;
			SetVltToRst[2].RelayMove.InactiveFlag = LBT22013_RLY_Output;
			SetVltToRst[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

			SetVltToRst[3].uw32_DelayTime = 0;
			SetVltToRst[3].RelayMove.InactiveFlag = LBT22013_RLY_Protect ;
			SetVltToRst[3].RelayMove.ActiveFlag =  LBT22013_RLY_DAMPING;
								

			SetCtrlOpenAcim[0].uw32_DelayTime = 100;
			SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT22013_RLY_ACIM;

			SetCtrlCloseAcim[0].uw32_DelayTime = 100;
			SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT22013_RLY_ACIM;
    }
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
char LBT22013_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        switch(type)
        {
        case ACTIVE_DAC_CUR:
            ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
            LBT22013_ADC5DAC::Dac_out_Cur(ChannelIdx+2,Data);
            break;
            //         case ACTIVE_DAC_VLT:
            //             LBT22013_ADC5DAC::Dac_out_Vlt(ChannelIdx+2,Data);
            //             break;
        case ACTIVE_DAC_VH:
            LBT22013_ADC5DAC::Dac_out_Clamp_Vlt(0,Data);   // 20140613  电压保护
            break;
        case ACTIVE_DAC_VL:
            LBT22013_ADC5DAC::Dac_out_Clamp_Vlt(1,Data);    //VOUTC - DACL  3?
            break;
        default:
            break;
        }
    }
    else
    {
        unsigned long value = Data;    //强制保护
        switch(type)
        {
        case ACTIVE_DAC_CUR:
// 	              value = 0xFFFFFF-value;
            ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
            LBT22013_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
            break;
				case ACTIVE_DAC_VLT:
						value = 0xFFFFFF-value;
						LBT22013_ADC5DAC::Dac_out_Vlt(1,value);
						break;
            ///////////////////钳位
        case ACTIVE_DAC_VH:
            LBT22013_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTA - DACH  // 20140613  电压保护
            break;
        case ACTIVE_DAC_VL:
            LBT22013_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTC - DACL
            break;
        default:
            break;
        }
    }
    return 1;
}


char LBT22013_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        switch(CurrRange)
        {
        case CURRENT_RANGE_1:
            LBT22013_relaystatus[ChannelIdx].Bits.R_CH = 1;
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Reg &= ~(LBT22013_RLY_R_CM | LBT22013_RLY_R_CL);
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //          CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            break;
        case CURRENT_RANGE_2:
            LBT22013_relaystatus[ChannelIdx].Bits.R_CM = 1;
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Reg &= ~(LBT22013_RLY_R_CH | LBT22013_RLY_R_CL);
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //         CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            break;
        case CURRENT_RANGE_3:
            LBT22013_relaystatus[ChannelIdx].Bits.R_CL = 1;
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Reg &= ~(LBT22013_RLY_R_CH | LBT22013_RLY_R_CM);
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            break;
        default:
            LBT22013_relaystatus[ChannelIdx].Reg &= ~(LBT22013_RLY_R_CH | LBT22013_RLY_R_CM | LBT22013_RLY_R_CL);
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //             LBT22013_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            break;
        }
        ARBUS_DELAY(10);
        CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
    }
    else
    {
//        ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯
        switch(CurrRange)
        {
        case CURRENT_RANGE_1:
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HHD]);
//             ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Prec]);
            return 1;
        case CURRENT_RANGE_2:
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HHD]);
//             ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Prec]);
            return 1;
        case CURRENT_RANGE_3:
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_LD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HHD]);
//             ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Prec]);
            return 1;
        case CURRENT_RANGE_4:
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_HHD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LD]);
//             ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Prec]);
            return 1;
        default:
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HHD]);
//             ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Prec]);
            return 0;
        }
    }
    return 1;
}
void LBT22013_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        Un_LBT22013_Relay2 MyRly;
        MyRly.m_Reg = SetRly;
        if(MyRly.m_Bits.Voltage_VD)    //放开冗余电压保护操作
        {
            cpu_iowr_16(NBUS_WRITE_VOLTAGERELAY, 0);
        }
        if(MyRly.m_Bits.Protect)
        {
            LBT22013_relaystatus[ChannelIdx].Bits.P_R = 1;
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
        }
        if(MyRly.m_Bits.OffGateH)
        {
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGH = 1;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGM = 0;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGL = 0;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
        }

        MyRly.m_Reg = ClrRly;
        if(MyRly.m_Bits.Voltage_VD)
        {
            cpu_iowr_16(NBUS_WRITE_VOLTAGERELAY, 1);
        }
        if(MyRly.m_Bits.Protect)
        {
            LBT22013_relaystatus[ChannelIdx].Bits.P_R = 0;
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
        }
        if(MyRly.m_Bits.Output)
        {
            LBT22013_relaystatus[ChannelIdx].Bits.R_CH = 0;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_CM = 0;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_CL = 0;
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
        }
        if(MyRly.m_Bits.OffGateH)
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
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGH = R[0];
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGM = R[1];
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
            LBT22013_relaystatus[ChannelIdx].Bits.R_OFFGL = R[2];
            ARBUS_DELAY(10);
            CPLD_FUNC::WriteDOBus(0,LBT22013_relaystatus[ChannelIdx].Reg);
        }
    }
    else
    {
        Un_LBT22013_Relay2 MyRly;

        MyRly.m_Reg = SetRly;
        if(MyRly.m_Bits.Protect)
				{
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Protect]);
					  ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_PROT]);
				}
        if(MyRly.m_Bits.OffGateH)
        {
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_H]);
        }
        if(MyRly.m_Bits.OffGateML)
        {
            switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
            {
            case CURRENT_RANGE_1:
            case CURRENT_RANGE_4:
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_M]);
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_L]);
                break;
            case CURRENT_RANGE_2:
                ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_M]);
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_L]);
                break;
            case CURRENT_RANGE_3:
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_M]);
                ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_L]);
                break;
            default:
                break;
            }
        }
        if(MyRly.m_Bits.OffGateHH)
        {
            switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
            {
            case CURRENT_RANGE_1:
                ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_OFFG_HH]);
                break;
            case CURRENT_RANGE_2:
            case CURRENT_RANGE_3:
            case CURRENT_RANGE_4:
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_HH]);
                break;
            default:
                break;
            }
        }
//         if(MyRly.m_Bits.Voltage_VD)
//         {
//             if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange ==VOLTAGE_RANGE_HIGH)
//             {
//                 ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_VD]);
//                 VoltageRangeFlag[ChannelIdx] = 2;
//             }
//             else
//             {
//                 ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_VD]);
//                 VoltageRangeFlag[ChannelIdx] = 0;
//             }
//         }
        if(MyRly.m_Bits.ACIM)
        {
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_ACIM]);
        }
        if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_Running_Flag != CHN_LOCK_THIRD_PARTY)
        {
            if((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOn))
            {
                ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Damping]);
            }
            else if ((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOff))
            {
                ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Damping]);
            }
        }
				if(MyRly.m_Bits.Ivs)
				{
						ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_IVS]);
				}
				if(MyRly.m_Bits.PreC)
				{
					 ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Prec]);    
				}

        MyRly.m_Reg = ClrRly;
        if(MyRly.m_Bits.Protect)
				{
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Protect]);
					  ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_PROT]);
				}

        if(MyRly.m_Bits.OffGateH)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_H]);
        }
        if(MyRly.m_Bits.OffGateHH)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_HH]);
        }
        if(MyRly.m_Bits.OffGateML)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_M]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_OFFG_L]);
        }
//         if(MyRly.m_Bits.Voltage_VD)
//         {
//             ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_VD]);
//             VoltageRangeFlag[ChannelIdx] = 2;
//         }
        if(MyRly.m_Bits.Output)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_MD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HD]);
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_HHD]);
//             ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Prec]);
// 		       ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯
        }
        if(MyRly.m_Bits.ACIM)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_ACIM]);
        }
        if(MyRly.m_Bits.Damping)
        {
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_Damping]);
        }
				if(MyRly.m_Bits.Ivs)
				{
						ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_IVS]);
				}
				if(MyRly.m_Bits.PreC)
				{
					 ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_Prec]);    
				}
    }
}

unsigned char LBT22013_Base::Set_Volt_Range(unsigned char uc_ChannelNo,unsigned char VoltRange)
{
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
	{
			VoltageRangeFlag[uc_ChannelNo] = 2;
	}
	else
	{
		if(VoltRange == VOLTAGE_RANGE_LOW)
		{		
			ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_VD]);
			VoltageRangeFlag[uc_ChannelNo] = 0;
		}
		else
		{
			ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_VD]);
			VoltageRangeFlag[uc_ChannelNo] = 2;
		}
	}
    return VoltageRangeFlag[uc_ChannelNo];
}

void LBT22013_Base::Adjust_ADC_Gain(char type,unsigned char chnType)
{
    if(chnType == ADC_TYPE_I)
    {
        if(type == 1)    //单倍
        {
            cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x299);  //modify by qjm20140925
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

// void LBT22013_Base::ADC_ADG_open(unsigned char ChannelIdx)
// {
// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
// 	   CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
// 	else
// 		return;
// }

void LBT22013_Base::ADG_Reset(void)
{
    ControlPin::SetMeLow(LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdgRst]);
    ARBUS_DELAY(5);
    ControlPin::SetMeHigh(LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdgRst]);
}

void LBT22013_Base::changeADCRange(unsigned char range)
{
    ARBUS_DELAY(10);
    Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void LBT22013_Base::Feed_CpldWD(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        ControlPin::SetMeHigh(PinGroupVA[LBT22013_VA_Pin_WD]);
        TIMERSFunc::DELAY_SYS(200, 0);
        ControlPin::SetMeLow(PinGroupVA[LBT22013_VA_Pin_WD]);
    }
    else
    {
        ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_WD]);
        TIMERSFunc::DELAY_SYS(200, 0);
        ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_WD]);
    }
}

char  LBT22013_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        char temp;
        temp=ControlPin::ReadMe(PinGroupVA[LBT22013_VA_Pin_RedundantSafety]);
        if(temp)
            return 0;
        else
            return 1;
    }
    else
    {
        float m_Volt,m_VH,m_VL;
        if(ChannelInfo_Manager::ChannelInfo[CH_Number].StepStatus!=STEPRUNNING)
            return 0;

        m_Volt=ChannelInfo_Manager::Read_SpecifiedMetavariable2(CH_Number,MetaCode_PV_Voltage);
        m_VH=ChannelInfo_Manager::ChannelInfo[CH_Number].m_fVclampHigh;
        m_VL=ChannelInfo_Manager::ChannelInfo[CH_Number].m_fVclampLow;

        if((m_VH<m_Volt-0.05f)||(m_VL>m_Volt+0.05f))
            return 1;
        else
            return 0;
    }
}

char LBT22013_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
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
            Step_Manager_task::MsTimeEngineTask[uc_ChannelNo]=CommonFunc::GetSystemTime();//OS_Time;
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
    else
        return 0;
}
char LBT22013_Base::TemperatureSafety(unsigned long CH_Number,unsigned long type)
{
    char temp;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        return 0;
    else
        temp=ControlPin::ReadMe(PinGroupVB[LBT22013_VB_Pin_Temperature]);
    if(temp)
        return 0;
    else
        return 1;
}

//#if(FAN_SAFETY_MODE==1)
char LBT22013_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    char temp;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        temp=ControlPin::ReadMe(PinGroupVA[LBT22013_VA_Pin_FanCheckSignal]);
    else
        temp=ControlPin::ReadMe(PinGroupVB[LBT22013_VB_Pin_FanCheckSignal]);
    if(temp)
        return 0;
    else
        return 1;
}

//#endif

char LBT22013_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{

    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
// 	{
// 		 float FanCtrlValue[8]={30,40,50,60,70,80,90,100};
// // 		 float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
// // 	   float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;
// // 	   FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;
// //
// // 		 FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
//   }

//   FanAdj_task::MAX_Ratio = 30;//先设成最小
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//寻找各通道最大风扇速度
// 	{
// 		if(FanAdj_task::Channel_Duty_Ratio[uc_i]>FanAdj_task::MAX_Ratio)
// 		{
// 			FanAdj_task::MAX_Ratio=FanAdj_task::Channel_Duty_Ratio[uc_i];
//     }
// 	}

    LPC4078PWM::PWM_MatchUpdate(PWM_HD1,5, DutyRatio, PWM_MATCH_UPDATE_NOW);
    return 1;
}

char LBT22013_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
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
            ControlPin::SetMeLow(PinGroupVB[LBT22013_VB_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 1;
        }
        else
        {
            ControlPin::SetMeHigh(PinGroupVB[LBT22013_VB_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 0;
        }
        break;

    default:
        break;
    }

    return 1;
}
char LBT22013_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
    char ReturnValue=0;
    switch (DI_Idx)
    {
    case 0:
        ReturnValue=GPIOFunc::RD_G_Port(PinGroupVB[LBT22013_VB_Pin_DI1].Port ,PinGroupVB[LBT22013_VB_Pin_DI1].Bits);
        break;

    default:
        break;
    }
    return ReturnValue;
}
void LBT22013_Base::UnitDI_ReadAll(void)
{
    if(ControlPin::ReadMe(PinGroupVB[LBT22013_VB_Pin_DI1]))
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
        ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI1].m_fValue=0;	//161125 zyx
    }


}
char LBT22013_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    OUTPUT_UnitDO(0,0,Value);
    OUTPUT_UnitDO(0,1,Value);
    return 1;
}
unsigned char LBT22013_Base::PowerOffCheck(void)
{
    unsigned char temp;
    bool bcheckPort;

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        bcheckPort = ControlPin::ReadMe(LBT22013_Base::PinGroupVA[LBT22013_VA_pin_PowerOff]);
    }
    else
    {
        bcheckPort = ControlPin::ReadMe(LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_PowerOff]);
    }
    temp = (!bcheckPort);
    return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
