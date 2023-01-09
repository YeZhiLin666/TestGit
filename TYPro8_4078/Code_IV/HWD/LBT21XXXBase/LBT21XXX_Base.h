#pragma once

// #include "../../IV_Tasks/OS_LogicProcess.h"
// #include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
// #include "../IV_Tasks/BSP.h"
// #include "../../common_OS/Data_Manipulation/Datatypes.h"

// #define LBT21XXX_POWER_OFF_PORT    1
// #define LBT21XXX_POWER_OFF_PIN    11


#define LBT21XXXSETLOWPULSEDACTIME  60
#define LBT21XXXSETLOWPULSEADCTIME  (100-LBT21XXXSETLOWPULSEDACTIME)
#define LBT21XXXSETHIGHPULSEDACTIME 60
#define LBT21XXXSETHIGHPULSEADCTIME (100-LBT21XXXSETHIGHPULSEDACTIME)


#define LBT21XXX_RLY_R_CLL   (0x01<< 1)
#define LBT21XXX_RLY_R_CH    (0x01<< 2)
#define LBT21XXX_RLY_R_CM    (0x01<< 3)
#define LBT21XXX_RLY_R_CL    (0x01<< 4)
#define LBT21XXX_RLY_R_ACIM	 (0x01<< 8)
#define MSTAT21044_RLY_R_IVS (0x01<< 10)
#define LBT21XXX_RLY_R_VM    (0x01<< 11)
#define LBT21XXX_RLY_R_CHON  (0x01<< 13)
#define LBT21XXX_RLY_P_R     (0x01<< 14)

typedef struct
{
    unsigned short   Reserved1:1;
    unsigned short   R_CLL:1;
    unsigned short   R_CH:1;
    unsigned short   R_CM:1;
    unsigned short   R_CL:1;
    unsigned short   Reserved2:3;
    unsigned short	 R_ACIM:1;
    unsigned short   Reserved5:1;
    unsigned short   R_IVS:1;
    unsigned short   R_VM:1;
    unsigned short   Reserved4:1;
    unsigned short   R_CHON:1;
    unsigned short   P_R:1;
    unsigned short   Damping:1;
} St_LBT21XXX_Relay_Bit;

typedef struct
{
    unsigned short   Protect:1;
    unsigned short   Output:1;
    unsigned short   Reserved1:6;
    unsigned short   Acim:1;
    unsigned short   Reserved2:1;
    unsigned short   Ivs:1;
    unsigned short   Reserved3:2;
    unsigned short   CHON:1;
    unsigned short   Reserved4:1;
    unsigned short   Damping:1;
} St_LBT21XXX_Relay_Bit2;
typedef union
{
    St_LBT21XXX_Relay_Bit2   m_Bits;
    unsigned short          m_Reg;
} Un_LBT21XXX_Relay2;
enum
{
    LBT21XXX_RLY_Protect  = (0x01 << 0),
    LBT21XXX_RLY_Output   = (0X01 << 1),
    LBT21XXX_RLY_ACIM   = (0x01 << 8),
    LBT21XXX_RLY_IVS   = (0x01 << 10),
    LBT21XXX_RLY_CHON   	= (0x01 << 13),
    LBT21XXX_RLY_DAMPING  = (0X01 << 15),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
    unsigned short   Reg;
    St_LBT21XXX_Relay_Bit Bits;
} St_LBT21XXX_Relay;

typedef struct
{
    unsigned short   DIDO_CH0:1;
    unsigned short   DIDO_CH1:1;
    unsigned short   DIDO_CH2:1;
    unsigned short   DIDO_CH3:1;
    unsigned short   DIDO_CH4:1;
    unsigned short   DIDO_CH5:1;
    unsigned short   DIDO_CH6:1;
    unsigned short   DIDO_CH7:1;
    unsigned short   Reserved1:8;
} St_LBT21XXX_DIDO_Bit;

typedef union
{
    unsigned short   Reg;
    St_LBT21XXX_DIDO_Bit Bits;
} St_LBT21XXX_DIDO;

typedef struct  //unsigned int
{
    unsigned short Adc24bit_CS5560_Enable:1;
    unsigned short Adc24bit_AD7175_Enable:1;
    unsigned short DAC16bit_AD5360_Enable:1;
    unsigned short DAC16bit_AD5754_Enable:1;
    unsigned short FAN_IIC_Enable:1;
    unsigned short FAN_PWM_Enable:1;
    unsigned short UnitDIDO_Enable:1;
    unsigned short R_VM_Enable:1;
    unsigned short ACIM_Enable:1;
    unsigned short PowerOffCheck_NeedNegation_Enable:1;
    unsigned short ACTIVE_DAC_CUR_NeedNegation_Enable:1;
    unsigned short ACTIVE_DAC_VH_VL_NeedNegation_Enable:1;
    unsigned short Have_DAC_Vlt_Enable:1;
    unsigned short Have_DAC_Vclamp_Enable:1;
    unsigned short ClampV_Concern_Parallel_Enable:1;
    unsigned short ACTIVE_DAC_VLT_NeedNegation_Enable:1;
    
} St_LBT21XXX_Feature_BoolBit;

typedef struct
{
    float  FINE_TUNE_MINERRORFACTOR;
    float  FINE_TUNE_STEPFACTOR ;
    float  FINE_TUNE_MAXADJUSTFACTOR;
	float  FINE_TUNE_PARAMasterWaitTime;
    unsigned char ACTIVE_DAC_CUR_Chan_JumpMode;
    unsigned char ACTIVE_DAC_VLT_Chan_JumpMode;
    unsigned char ACTIVE_DAC_VH_Chan_JumpMode;
    unsigned char ACTIVE_DAC_VL_Chan_JumpMode;

    unsigned char LBT21XXXMINIMUMPLSE;
    unsigned char I_ADG_Address_Change_Mode;
    St_LBT21XXX_Feature_BoolBit BoolFlag;
} St_LBT21XXX_Feature;
////////////////

enum
{
    LBT21XXX_ADC_IDLE,
    LBT21XXX_ADC_OpenADG,
    LBT21XXX_ADC_OPENING,
    LBT21XXX_ADC_CONV,
    LBT21XXX_ADC_TriggerI,
    LBT21XXX_ADC_TriggerI_V,
    LBT21XXX_ADC_WAIT,
    LBT21XXX_ADC_ReadI,
    LBT21XXX_ADC_TriggerV,
    LBT21XXX_ADC_ReadV,
    LBT21XXX_ADC_CloseADG,
    LBT21XXX_ADC_NoWork,
};
enum
{
    LBT21XXX_Pin_AdcConver = 0,
    LBT21XXX_Pin_AdcBusy1,
    LBT21XXX_Pin_AdcBusy2,
    LBT21XXX_Pin_AdcCS1,
    LBT21XXX_Pin_AdcCS2,
    LBT21XXX_Pin_AdcRst,
    LBT21XXX_ADC5560_Total_Pins,
};
enum
{
    LBT21XXX_Pin_Adc2START=0,
    LBT21XXX_Pin_Adc2DRDY1,
    LBT21XXX_Pin_Adc2DRDY2,
    LBT21XXX_Pin_Adc2CS1,
    LBT21XXX_Pin_Adc2CS2,
    LBT21XXX_AD7175_Total_Pins,
};
enum
{
    LBT21XXX_Pin_Dac_CS=0,
    LBT21XXX_Pin_Dac_Load,
    LBT21XXX_Pin_Dac_Clr,
    LBT21XXX_DAC5754_Total_Pins,
};
enum
{
    LBT21XXX_Pin_DacCS=0,
    LBT21XXX_Pin_DacRst,
    LBT21XXX_Pin_DacLoad,
    LBT21XXX_Pin_DacBusy,
    LBT21XXX_DAC5360_Total_Pins,
};
enum
{
    LBT21XXX_Pin_WD,
    LBT21XXX_Pin_SCK1,
    LBT21XXX_Pin_MISO1,
    LBT21XXX_Pin_MOSI1,
    //LBT21XXX_Pin_AdgRst,
    LBT21XXX_Pin_RedundantSafety,
    LBT21XXX_Pin_FAN,
    LBT21XXX_Pin_FanCheckSignal,
    LBT21XXX_Pin_PowerOff,
    LBT21XXX_Total_Pins,
};
enum
{
    LBT21XXX_Chan_JumpMode_None=0,
    LBT21XXX_Chan_JumpMode_21084CUR,
    LBT21XXX_Chan_JumpMode_21024CUR,
    LBT21XXX_Chan_JumpMode_21084VH,
    LBT21XXX_Chan_JumpMode_21084VL,
    LBT21XXX_Chan_JumpMode_21024VH,
    LBT21XXX_Chan_JumpMode_21024VL,
    LBT21XXX_Chan_JumpMode_21044CUR,
    LBT21XXX_Chan_JumpMode_21044VLT,
    LBT21XXX_Chan_JumpMode_21044VH,
    LBT21XXX_Chan_JumpMode_21044VL,
    LBT21XXX_Chan_JumpMode_20084VLT,
	LBT21XXX_Chan_JumpMode_MSTAT21044CUR,
    LBT21XXX_Chan_JumpMode_MSTAT21044VLT,
	LBT21XXX_Chan_JumpMode_MSTAT21044VH,
	LBT21XXX_Chan_JumpMode_MSTAT21044VL,

};
enum
{
    LBT21XXX_I_ADG_Address_Change_Mode_None=0,
    LBT21XXX_I_ADG_Address_Change_Mode_21084,
    LBT21XXX_I_ADG_Address_Change_Mode_21024_VA_VB,
    LBT21XXX_I_ADG_Address_Change_Mode_21024_VC_VD,
};

class LBT21XXX_Base
{
public:
// 	static MsTime     FanOverTime;
//    static void FeatureEnable_Init(void);
    static volatile St_LBT21XXX_DIDO     CPLD_DO;    //记录DO的channel状态
    static volatile St_LBT21XXX_DIDO     CPLD_DI;    //记录DI的channel状态


    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

    static  void WriteDOBus(char chn,uint16 BD);


    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
    static void changeADCRange(unsigned char range);

    static void Feed_CpldWD(void);
    static char RedundantSafety(unsigned long CH_Number,unsigned long type);
    static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);
    static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);


    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);


    static void Adjust_ADC_Gain(char type,char chnType);

public:
    static volatile St_LBT21XXX_Relay LBT21XXX_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

    static SSPI_Para  LBT21XXX_SSP0_DAC_5360;
    static SSPI_Para  LBT21XXX_SSP0_DAC_5754;    //VCLAMP
    static SSPI_Para  LBT21XXX_SSP1_ADC_5560;
    static SSPI_Para  LBT21XXX_SSP1_ADC_7175;

    static const St_Port PinGroup[LBT21XXX_Total_Pins];
    static const St_Port PinGroup_ADC[LBT21XXX_ADC5560_Total_Pins];
//   static const St_Port PinGroup_ADC2[LBT21XXX_AD7175_Total_Pins];
    static const St_Port PinGroup_DAC1[LBT21XXX_DAC5754_Total_Pins];
    static const St_Port PinGroup_DAC2[LBT21XXX_DAC5360_Total_Pins];
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);

    static void ADC_Init(void);
    //四个基本操作

    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
    static unsigned long ChannelIdxChange(unsigned  long SourceChannel,unsigned char type);
    static void I_ADG_Address_Change(unsigned char ChannelIdx, unsigned char Current_Range);
    static  void ADC_ADG_open(unsigned char ChannelIdx);
    static unsigned char PowerOffCheck(void);
    static void FAN_Init(void);
    static void ADC_SPI_Set(void);
    static void DAC_SPI_Set(void);
    static void R_VM_RelayState_Init(void);
    static void ACIM_Relay_Init(void);
    static void ReadAllBuildInVolt(void);
    static St_LBT21XXX_Feature BoardFeature;

};

// class program
// {
// 	public:
// 	static void test(void);
// };



