#include "includes.h"

#define  LBT22XXX_FINE_TUNE_STARTTIME          0.1f
#define  LBT22XXX_FINE_TUNE_MINERRORFACTOR     2E-5
#define  LBT22XXX_FINE_TUNE_STEPFACTOR         5E-5  //6E-6
//#define  LBT22XXX_FINE_TUNE_MAXADJUSTFACTOR    0.03f   //  20140617 微调
#define  LBT22XXX_FINE_TUNE_MAXADJUSTFACTOR    0.06f

#define   LBT22XXX_VOLTAGE_RANGE_CHANGE_TIME	30

#define  LBT22XXX_ACCURACY                     0.0005f   //精度
#define  LBT22XXX_CTRL_VALUE_ERROR_FACTOR      100       //超出精度100倍，被认为是控制值异常发生

#define  LBT22XXX_SETTING_TIME                 500       //电流上升沿时间，单位为100us
#define  LBT22XXX_RISING_TIME                  50       //5ms
#define  LBT22XXX_Tpid_Tsys           0.3          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

#define LBT22XXX_PID_SAMPLE_DELAY_COUNTER       5

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT22XXX_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT22XXX_Base::SetCtrlCloseCh[5];
St_DisposableTask LBT22XXX_Base::SetCtrlCloseChToRest[5];
St_DisposableTask LBT22XXX_Base::SetCCToCC[8];
St_DisposableTask LBT22XXX_Base::SetRstToOff[1];
St_DisposableTask LBT22XXX_Base::SetOffToRst[1];       //do nothing
St_DisposableTask LBT22XXX_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT22XXX_Base::SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask LBT22XXX_Base::SetAutoRangeSwitch[8];
#endif
SSPI_Para  LBT22XXX_Base::LBT22XXX_SSP0DAC;
#pragma arm section


unsigned char LBT22XXX_Pin_OFFG_M_State = 0;
const St_Port LBT22XXX_Base::PinGroup[LBT22XXX_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
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
    {3,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {3,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    // const St_Port LBT22XXX_Base::Pin_CL_Cal      = {3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0};  //P3.23
    {2,        0,        eCPTYPE_MCUGPIO,    GPIO_O,                0}, //P3.23
    {2,        1,        eCPTYPE_MCUGPIO,    GPIO_O,                0},  //P3.23
    {2,        2,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.2
    {2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.3
    {2,        5,         eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.5
    {0,        10,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.21
    {0,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P2.5
    {0,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0},  //P0.22
    {2,        26,         eCPTYPE_MCUGPIO,  GPIO_O,                0},
    {2,        4,          eCPTYPE_MCUGPIO,  PWM1,                  0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
//新增
    {0,        8,        eCPTYPE_MCUGPIO,   GPIO_O,             0},

    {2,        11,        eCPTYPE_MCUGPIO,   GPIO_I,                0},  //P2.11
    {3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//#if(FAN_SAFETY_MODE==1)
    {2,        9,         eCPTYPE_MCUGPIO,   GPIO_I,                0},  //风扇报警检测，0为有效
//#endif
    {0,        9,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        6,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {4,        29,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {0,        7,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {4,        28,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
    {0,        27,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
    {0,        28,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
};


unsigned char LBT22XXX_Base::VoltageRangeFlag[MAXCHANNELNO];  //0:低量程   1：中量程    2：高量程
unsigned char LBT22XXX_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT22XXX_Base::m_bHaveClampVdac = 1;
// const unsigned char LBT22XXX_Base::m_bChannelBasedClampVdac = 0;

St_LBT22XXX_Feature LBT22XXX_Base::BoardFeature;
void LBT22XXX_Base::Hardware_Init(void)
{
    LBT22XXX_FeatureEnable_Init();
    IO_Init();

    board_config::bHaveRedundantCheck = 1;

    //SSP0 for DAC    AD5754
    LBT22XXX_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
    LBT22XXX_SSP0DAC.uc_Speed = 5;
    LBT22XXX_SSP0DAC.uc_CPOL = 0;
    LBT22XXX_SSP0DAC.uc_CPHA = 1;
    LBT22XXX_SSP0DAC.uc_MSTR = 1;    //master
    LBT22XXX_SSP0DAC.uc_BITS = 8;
    LBT22XXX_SSP0DAC.uc_LSBF = 0;

    ADC_Init();
    DAC_Init();

    ControlPin::ReInitPullUP();

    //RANGE INIT
    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);

    if(!BoardFeature.BoolFlag.OffGateM_Enable)
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);

    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_CL_A0]);

    //新增
    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_VCEN]);
    //DAC INIT
    LBT22XXX_Base::DAC_out(0,0x00800000,ACTIVE_DAC_CUR);
    Feed_CpldWD();

// 	AdditionalEngine();

    ////////拉高钳压    // 20140613  电压保护
    LBT22XXX_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
    LBT22XXX_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);

//#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
    {
        // 风扇初始化
        safety_check::Fan_FaultCheckFlag=0;
        // I2CBusNew::AD5245_SET(0,0,I2C1);  //此处的100 改为0~255的数即可
    }
//#endif
    board_config::bFanAdj=1;

    FanAdj_task::AdjMod=PWMMOD;

    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,20,1000);
    unsigned char FanCtrlValue[8]= {30,40,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue[0], 8);
    for (unsigned char i=0; i<16; i++)
    {
        VoltageRangeFlag[i]=2;   //初始化为高量程
    }
}

void LBT22XXX_Base::LBT22XXX_FeatureEnable_Init(void)
{
    board_config::Feature_Enable_init.Power_Off_Enable =1;
    board_config::Feature_Enable_init.Fan_Safety_Mode =1;
    board_config::Feature_Enable_init.Temperature_Safety_Mode=1;
    board_config::Feature_Enable_init.OS_Tick_Interval =200;
}

void LBT22XXX_Base::IO_Init(void)
{
    for(int i=0; i<LBT22XXX_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup[i]);

    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Prec]);//初始化为高

}
void LBT22XXX_Base::ADC_Init(void)
{
    LBT22XXX_ADC5DAC::Adc_Init();
}
void LBT22XXX_Base::DAC_Init(void)
{
    LBT22XXX_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
    board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
    board_config::DacInfo.m_uSettingTime = LBT22XXX_SETTING_TIME;
    board_config::DacInfo.m_uRisingTime = LBT22XXX_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT22XXX_Tpid_Tsys;

    ARBUS_DELAY(10000);    //300us reset time  AD5360????
}

void LBT22XXX_Base::Software_Init(void)
{
    Func_Init();
//	EngineStuff();
    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = 0.2f;
    Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT22XXX_FINE_TUNE_STARTTIME;   //  20140617 微调
    Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT22XXX_FINE_TUNE_MINERRORFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT22XXX_FINE_TUNE_MAXADJUSTFACTOR;
    Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT22XXX_FINE_TUNE_STEPFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime =0.0128;

//	safety_check::m_CtrlValueSetting.fErrorFactor       = LBT22XXX_ACCURACY * LBT22XXX_CTRL_VALUE_ERROR_FACTOR;  //180614 zyx

    board_config::Accuracy = LBT22XXX_ACCURACY;

//    Ctrl_Pid_Coef::m_SampleDelayCounter = LBT22XXX_PID_SAMPLE_DELAY_COUNTER;

    LBT22XXX_SampleManage::Init();
    IR::pIR_ADC = &(LBT22XXX_SampleManage::IR_ADC);
    IR::pRenew_IR_ADC = &(LBT22XXX_SampleManage::renew_IR_ADC);
//	  pIR_Trigger_ADC = &(LBT22044_SampleManage::IR_Trigger_ADC);
//	  IR_TriggerFlag=1;
    IR::pIR_Read_ADC = &(LBT22XXX_SampleManage::IR_Read_ADC);
    IR::Set_LowPulseDacTime=LBT22XXXSETLOWPULSEDACTIME;
    IR::Set_LowPulseAdcTime=LBT22XXXSETLOWPULSEADCTIME;
    IR::Set_HighPulseDacTime=LBT22XXXSETHIGHPULSEDACTIME;
    IR::Set_HighPulseAdcTime=LBT22XXXSETHIGHPULSEADCTIME;
    IR::Set_MinimumPulseTime=LBT22XXXMINIMUMPULSE;
}

void LBT22XXX_Base::Func_Init(void)
{
    board_config::Func_Dac_out=&LBT22XXX_Base::DAC_out;
    board_config::Func_SetRange=&LBT22XXX_Base::SetRange;
    board_config::Func_SetRelay=&LBT22XXX_Base::SetRelay;
	
		board_config::Func_Set_Volt_Range = &LBT22XXX_Base::Set_Volt_Range;
		board_config::Voltage_Range_Change_Time = LBT22XXX_VOLTAGE_RANGE_CHANGE_TIME;
	
    board_config::Func_Sample =&LBT22XXX_SampleManage::SampleISR;    //timer0 中断
    //	board_config::Func_TempSample = &LBT22044_TempSample::Temp_rd;   // 20140613  温度测量
    board_config::Func_RedundantSafety= &LBT22XXX_Base::RedundantSafety; //冗余电压保护
    board_config::Func_ReleaseRedundantSafety=&LBT22XXX_Base::ReleaseRedundantSafety;//冗余解除操作
//#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
    {
        board_config::Func_FanSafety= &LBT22XXX_Base::FanCheckSafety; //风扇保护
    }
    if(board_config::Feature_Enable_init.Temperature_Safety_Mode==1)
    {
        board_config::Func_TemperatureSafety = &LBT22XXX_Base::TemperatureSafety;
    }
//#endif
    board_config::Func_FanAdj=&LBT22XXX_Base::FanAdjOperate;

    board_config::Func_UnitDO=&LBT22XXX_Base::OUTPUT_UnitDO;
    board_config::Func_UnitDI=&LBT22XXX_Base::Read_UnitDI;
    board_config::Func_UnitDI_ReadAll = &LBT22XXX_Base::UnitDI_ReadAll;
    board_config::Func_UnitDO_Reset = &LBT22XXX_Base::UnitDO_Reset;
    board_config::FeedWatchDog = &LBT22XXX_Base::Feed_CpldWD;
    PowerLostCheck::Func_PowerOffSafetCheck = &LBT22XXX_Base::PowerOffCheck;
}



char LBT22XXX_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
    unsigned long value = Data;    //强制保护

    switch(type)
    {
    case ACTIVE_DAC_CUR:

        value = 0xFFFFFF-value;
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
        LBT22XXX_ADC5DAC::Dac_out_Cur(ChannelIdx,value);
        break;
		case ACTIVE_DAC_VLT:
			  value = 0xFFFFFF-value;
		    LBT22XXX_ADC5DAC::Dac_out_Vlt(1,value);
		    break;
        ///////////////////钳位
    case ACTIVE_DAC_VH:
        LBT22XXX_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTA - DACH  // 20140613  电压保护
        break;
    case ACTIVE_DAC_VL:
        LBT22XXX_ADC5DAC::Dac_out_Clamp_Vlt(3,value);    //VOUTC - DACL
    default:
        break;
    }
    return 1;
}

char LBT22XXX_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯

    switch(CurrRange)
    {
    case CURRENT_RANGE_1:
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LLD]);
//         ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A0]);
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A0]);
        return 1;
    case CURRENT_RANGE_2:
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LLD]);
//         ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_A0]);
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A0]);
        return 1;
    case CURRENT_RANGE_3:
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_LD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LLD]);
//         ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A0]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_CL_A0]);
        return 1;
    case CURRENT_RANGE_4:
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_LLD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LD]);
//         ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A0]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A0]);
        return 1;
    default:
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LLD]);
//         ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A0]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A0]);
        return 0;
    }
    //	return 1;
}

unsigned char LBT22XXX_Base::Set_Volt_Range(unsigned char uc_ChannelNo,unsigned char VoltRange)
{
    if(VoltRange == VOLTAGE_RANGE_LOW)
	{
		ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_VD]);
		VoltageRangeFlag[uc_ChannelNo] = 0;
	}
    else
    {
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_VD]);
		VoltageRangeFlag[uc_ChannelNo] = 2;
    }
 
	return VoltageRangeFlag[uc_ChannelNo];

}

void LBT22XXX_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
    Un_LBT22XXX_Relay MyRly;

    MyRly.m_Reg = SetRly;
    if(MyRly.m_Bits.Protect)
		{
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Protect]);
				ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_PROT]);
		}
    if(BoardFeature.BoolFlag.OffGateM_Enable)
    {
        if(MyRly.m_Bits.OffGate)
        {
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);

            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);
        }
        if(MyRly.m_Bits.OffGateM)
        {
            ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_M]);
            LBT22XXX_Pin_OFFG_M_State = 0;

        }
    }
    else
    {
        if(MyRly.m_Bits.OffGate)
        {
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
        }
    }
		if(MyRly.m_Bits.PreC)
    {
       ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Prec]);    
    }
//     if(MyRly.m_Bits.V_D)
//     {
//         if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange ==VOLTAGE_RANGE_HIGH)
//         {
//             ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_VD]);
//             VoltageRangeFlag[ChannelIdx] = 2;
//         }
//         else
//         {
//             ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_VD]);
//             VoltageRangeFlag[ChannelIdx] = 0;
//         }
//     }
    if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_Running_Flag != CHN_LOCK_THIRD_PARTY)
    {
        if((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOn))
        {
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Damping]);
        }
        else if ((MyRly.m_Bits.Damping)&&(Step_Manager_task::GetStepRunInfo[ChannelIdx].GetStepInfo.m_Data.m_Ctrl.m_DampingOn == eDampingOn_DampingOff))
        {
            ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Damping]);
        }
    }

    if(MyRly.m_Bits.Acim)
    {
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Acim]);
    }
    if(MyRly.m_Bits.Ivs)
	  {
			  ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_IVS]);
    }


    MyRly.m_Reg = ClrRly;
    if(MyRly.m_Bits.Protect)
		{
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Protect]);
				ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_PROT]);
		}
    if(BoardFeature.BoolFlag.OffGateM_Enable)
    {
        if(MyRly.m_Bits.OffGate)
        {
            switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
            {
            case CURRENT_RANGE_1:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HM]);

                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                break;
            case CURRENT_RANGE_2:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HM]);

                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                break;
            case CURRENT_RANGE_3:
            case CURRENT_RANGE_4:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HM]);

                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                break;
            default:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HM]);

                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                break;
            }
        }
        if(MyRly.m_Bits.OffGateM)
        {
            switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
            {
            case CURRENT_RANGE_1:
                break;
            case CURRENT_RANGE_2:
                if(LBT22XXX_Pin_OFFG_M_State == 0)
                {
                    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);
                    LBT22XXX_Pin_OFFG_M_State = 1;
                }
                break;
            case CURRENT_RANGE_3:
                break;
            case CURRENT_RANGE_4:
                break;
            default:
                break;

            }
        }
    }
    else
    {
        if(MyRly.m_Bits.OffGate)
        {
            switch(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange)
            {
            case CURRENT_RANGE_1:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_M]);
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HM]);
                break;
            case CURRENT_RANGE_2:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_M]);
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
                break;
            case CURRENT_RANGE_3:
            case CURRENT_RANGE_4:
                ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
                break;
            default:
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);
                ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
                break;
            }
        }
    }


//     if(MyRly.m_Bits.V_D)
//     {
//         //       if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange ==VOLTAGE_RANGE_HIGH)
//         // 				    ControlPin::SetMeLow(Pin_VD);
//         // 			else
//         //            ControlPin::SetMeHigh(Pin_VD);
//         ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_VD]);
//         VoltageRangeFlag[ChannelIdx] = 2;
//     }
    if(MyRly.m_Bits.Output)
    {
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_MD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_HD]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LLD]);
//         ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Prec]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_LEDDriver]);   // add by qjm20141013 量程指示灯

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A1]);
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_A0]);

        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_CL_A1]);
        ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_CL_A0]);

        if(!BoardFeature.BoolFlag.OffGateM_Enable)
        {
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HL]);
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_M]);
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_OFFG_HM]);
        }
    }
    if(MyRly.m_Bits.Acim)
    {
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Acim]);
    }
    if(MyRly.m_Bits.Damping)
    {
        ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_Damping]);
    }
		if(MyRly.m_Bits.Ivs)
		{
			ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_IVS]);
    }
		if(MyRly.m_Bits.PreC)
    {
       ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_Prec]);    
    }

}

void LBT22XXX_Base::Feed_CpldWD(void)
{
    ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_WD]);
    TIMERSFunc::DELAY_SYS(200, 0);
    ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_WD]);
}

// void LBT22XXX_Base::ADC_ADG_open(unsigned char ChannelIdx)
// {
// 	return;
// }
char  LBT22XXX_Base::RedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{

    float m_Volt,m_VH,m_VL;

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus!=STEPRUNNING)
        return 0;

    m_Volt=ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);
    m_VH=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh;
    m_VL=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow;

    if((m_VH<m_Volt-0.05f)||(m_VL>m_Volt+0.05f))
        return 1;
    else
        return 0;


}

char LBT22XXX_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
    return 0;
}
char LBT22XXX_Base::TemperatureSafety(unsigned long CH_Number,unsigned long type)
{
    char temp;
    temp=ControlPin::ReadMe(PinGroup[LBT22XXX_Pin_Temperature]);
    if(temp)
        return 0;
    else
        return 1;
}

//#if(FAN_SAFETY_MODE==1)
char LBT22XXX_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    char temp;
    temp=ControlPin::ReadMe(PinGroup[LBT22XXX_Pin_FanCheckSignal]);
    if(temp)
        return 0;
    else
        return 1;
}
//#endif

char LBT22XXX_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{
    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
// 		for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
// 	{
// 		 float FanCtrlValue[8]={30,40,50,60,70,80,90,100};
// // 		 float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
// // 	   float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;
// // 	   FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;
// // 	//				{
// // 		 FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
//   }

    //				case 0:
//   FanAdj_task::MAX_Ratio = 30;//先设成最小
// 	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//寻找各通道最大风扇速度
// 	{
// 		if(FanAdj_task::Channel_Duty_Ratio[uc_i]>FanAdj_task::MAX_Ratio)
// 		{
// 			FanAdj_task::MAX_Ratio=FanAdj_task::Channel_Duty_Ratio[uc_i];
//     }
// 	}
    //LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,FanAdj_task::FanSetValue);
    LPC4078PWM::PWM_MatchUpdate(PWM_HD1,PWM_CH5, DutyRatio, PWM_MATCH_UPDATE_NOW);
    return 1;
}

char LBT22XXX_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
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
            ControlPin::SetMeHigh(PinGroup[LBT22XXX_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 1;
        }
        else
        {
            ControlPin::SetMeLow(PinGroup[LBT22XXX_Pin_DO1]);
            ChannelInfo_Manager::BuildIn_DO[0][0] = 0;
        }
        break;

    default:
        break;
    }

    return 1;
}
char LBT22XXX_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
    char ReturnValue=0;
    switch (DI_Idx)
    {
    case 0:
        ReturnValue=GPIOFunc::RD_G_Port(PinGroup[LBT22XXX_Pin_DI1].Port ,PinGroup[LBT22XXX_Pin_DI1].Bits);
        break;

    default:
        break;
    }
    return ReturnValue;
}
void LBT22XXX_Base::UnitDI_ReadAll(void)
{
    if(ControlPin::ReadMe(PinGroup[LBT22XXX_Pin_DI1]))
    {
        if(Sample_Pro::Digital_SampleCounter[0][0]>3)
        {
            ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_UNIT_DI1].m_fValue=1;  //modified by xp	//161125 zyx
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
char LBT22XXX_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    OUTPUT_UnitDO(0,0,Value);
    OUTPUT_UnitDO(0,1,Value);
    return 1;
}
unsigned char LBT22XXX_Base::PowerOffCheck(void)
{
    unsigned char temp;
    temp = !(ControlPin::ReadMe(LBT22XXX_Base::PinGroup[LBT22XXX_Pin_PowerOff]));
    return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
