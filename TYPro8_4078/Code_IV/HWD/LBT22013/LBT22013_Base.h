#pragma once

// #include "../../IV_Tasks/OS_LogicProcess.h"
// #include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
// #include "../IV_Tasks/BSP.h"
// #include "../../common_OS/Data_Manipulation/Datatypes.h"

// #define LBT22013_POWER_OFF_PORT    4
// #define LBT22013_POWER_OFF_PIN    29

#define LBT22013SETLOWPULSEDACTIME  50
#define LBT22013SETLOWPULSEADCTIME  (100-LBT22013SETLOWPULSEDACTIME)
#define LBT22013SETHIGHPULSEDACTIME 50
#define LBT22013SETHIGHPULSEADCTIME (100-LBT22013SETHIGHPULSEDACTIME)
#define LBT22013MINIMUMPULSE     50

#define LBT22013_RLY_R_Voltage   (0x01<< 0)
#define LBT22013_RLY_R_CH        (0x01<< 2)
#define LBT22013_RLY_R_CM        (0x01<< 3)
#define LBT22013_RLY_R_CL        (0x01<< 4)
#define LBT22013_RLY_R_OFFGH     (0x01<< 6)
#define LBT22013_RLY_R_OFFGL     (0x01<< 7)
#define LBT22013_RLY_R_OFFGM     (0x01<< 9)
#define LBT22013_RLY_P_R         (0x01<< 14)

typedef struct
{
    unsigned short   R_Voltage:1;       // 硬件控制单电压  CPLD bit
    unsigned short   Reserved7:1;
    unsigned short   R_CH:1;
    unsigned short   R_CM:1;
    unsigned short   R_CL:1;
    unsigned short   Reserved6:1;
    unsigned short   R_OFFGH:1;
    unsigned short   R_OFFGL:1;
    unsigned short   Reserved5:1;
    unsigned short   R_OFFGM:1;
    unsigned short   Reserved4:1;
    unsigned short   Reserved3:1;
    unsigned short   Reserved2:1;
    unsigned short   Reserved1:1;
    unsigned short   P_R:1;
    unsigned short   Reserved0:1;
} St_LBT22013_Relay_Bit;

typedef struct
{
    unsigned short   Protect:1;
    unsigned short   OffGateH:1;
    unsigned short   Output:1;
    unsigned short   Voltage_VD:1;          // 硬件控制单电压  CPLD bit
    unsigned short   ACIM:1;
    unsigned short   OffGateML:1;
    unsigned short   OffGateHH:1;
    unsigned short   Reserved1:2;
    unsigned short   Damping:1;
	  unsigned short   Ivs:1;
	  unsigned short   PreC:1;
    unsigned short   Reserved2:5;
} St_LBT22013_Relay_Bit2;
typedef union
{
    St_LBT22013_Relay_Bit2   m_Bits;
    unsigned short          m_Reg;
} Un_LBT22013_Relay2;
enum
{
    LBT22013_RLY_Protect  = (0x01 << 0),
    LBT22013_RLY_OffGateH  = (0X01 << 1),
    LBT22013_RLY_Output   = (0X01 << 2),
    LBT22013_RLY_VoltSet = (0X01 << 3),   // 硬件控制单电压  CPLD bit
    LBT22013_RLY_ACIM = (0X01 << 4),
    LBT22013_RLY_OffGateML = (0X01 << 5),
    LBT22013_RLY_OffGateHH = (0X01 << 6),
    LBT22013_RLY_DAMPING  = (0X01 << 9),
	  LBT22013_RLY_IVS      = (0X01 << 10),
	  LBT22013_RLY_PreC     = (0X01 << 11)
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
    unsigned short   Reg;
    St_LBT22013_Relay_Bit Bits;
} St_LBT22013_Relay;

typedef struct
{
    unsigned short   AdcGain_I:2;
    unsigned short   AdcGain_V:2;
    unsigned short   AdcGainEnable:1;
    unsigned short   Reserved:11;
} St_LBT22013_PGA_Bit;
typedef union
{
    unsigned short       Reg;
    St_LBT22013_PGA_Bit   Bits;
} St_LBT22013_PGA;

////////////////
enum
{
    LBT22013_SSP0_DAC_NAME = 0xA0,
    LBT22013_SSP0_AD5754_NAME,
    LBT22013_SSP1_ADI_NAME,
    LBT22013_SSP1_ADV_NAME ,
    LBT22013_SSP0_FRAM_NAME  //AD5754
};

enum
{
    LBT22013_ADC_IDLE,
    LBT22013_ADC_WAIT,
    LBT22013_ADC_TriggerI,
    LBT22013_ADC_ReadI,
    LBT22013_ADC_TriggerV,
    LBT22013_ADC_ReadV,

    LBT22013_ADC_CONV,
    LBT22013_ADC_ReadIV,
};
enum
{
    LBT22013_HIGH_RANGE_NUM = 0xF1,
    LBT22013_MID_RANGE_NUM = 0xF2,
    LBT22013_LOW_RANGE_NUM = 0xF3,
};
enum
{
    LBT22013_VA_Pin_AdcConver,
    LBT22013_VA_Pin_AdcBusy1,
    LBT22013_VA_Pin_AdcBusy2,
    LBT22013_VA_Pin_AdcCS1,
    LBT22013_VA_Pin_AdcCS2,
    LBT22013_VA_Pin_AdcRst,
    LBT22013_VA_Pin_Dac_CS,
    LBT22013_VA_Pin_Dac_Load,
    LBT22013_VA_Pin_Dac_Clr,
    LBT22013_VA_Pin_WD ,
    LBT22013_VA_Pin_SCK1,
    LBT22013_VA_Pin_MISO1,
    LBT22013_VA_Pin_MOSI1,
    LBT22013_VA_Pin_PowerOn,
    LBT22013_VA_Pin_AdgRst,
    LBT22013_VA_Pin_RedundantSafety,
    LBT22013_VA_Pin_FanCheckSignal,
    LBT22013_VA_pin_PowerOff,
    LBT22013_VA_Total_Pins,
};

enum
{
    LBT22013_VB_Pin_AdcConverA,
    LBT22013_VB_Pin_AdcConverB,
    LBT22013_VB_Pin_AdcBusy,
    LBT22013_VB_Pin_AdcRst,
    LBT22013_VB_Pin_OS0,
    LBT22013_VB_Pin_OS1,
    LBT22013_VB_Pin_OS2,
    LBT22013_VB_Pin_FDATA,
    LBT22013_VB_Pin_DacBIN2COMP,
    LBT22013_VB_Pin_DacCS,
    LBT22013_VB_Pin_DacLoad,
    LBT22013_VB_Pin_DacCLR,
    LBT22013_VB_Pin_HD ,
    LBT22013_VB_Pin_MD,
    LBT22013_VB_Pin_LD,
    LBT22013_VB_Pin_Protect,
    LBT22013_VB_Pin_VD ,
    LBT22013_VB_Pin_OFFG_H,
    LBT22013_VB_Pin_OFFG_M,
    LBT22013_VB_Pin_OFFG_L,
// 	LBT22013_VB_Pin_LEDDriver,
    LBT22013_VB_Pin_WD,
    LBT22013_VB_Pin_FAN,
    LBT22013_VB_Pin_DI1,
    LBT22013_VB_Pin_DO1,
    LBT22013_VB_Pin_IVSD,
    LBT22013_VB_Pin_FanCheckSignal,
    LBT22013_VB_Pin_ACIM,
    LBT22013_VB_Pin_PowerOff,
    LBT22013_VB_Pin_Damping,
    LBT22013_VB_Pin_Prec,
    LBT22013_VB_Pin_HHD,
    LBT22013_VB_Pin_OFFG_HH,
    LBT22013_VB_Pin_Temperature,
    LBT22013_VB_Pin_IVS,
    LBT22013_VB_Pin_PROT,
    LBT22013_VB_Total_Pins,
};

class LBT22013_Base
{
private:
    static volatile St_LBT22013_Relay LBT22013_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
    static MsTime     FanOverTime;


    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[6];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[5];    //关闭通道
    static St_DisposableTask SetCtrlCloseChToRest[5];
    static St_DisposableTask SetCCToCC[8];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing
    static St_DisposableTask SetOffToRst[2];
    static St_DisposableTask SetVoltRelay[2];     //设置冗余电压
    static St_DisposableTask SetCtrlOpenAcim[1];
    static St_DisposableTask SetCtrlCloseAcim[1];

		static St_DisposableTask SetRstToOffChn[1];
		static St_DisposableTask SetRstToCur[5];
		static St_DisposableTask SetRstToVlt[6];

		static St_DisposableTask SetOffToRstChn[1];
		static St_DisposableTask SetOffToCur[5];
		static St_DisposableTask SetOffToVlt[6];

		static St_DisposableTask SetCurToOff[5];
		static St_DisposableTask SetCurToRst[5];
		static St_DisposableTask SetCurToCur[9];
		static St_DisposableTask SetCurToVlt[10];


		static St_DisposableTask SetVltToOff[4];
		static St_DisposableTask SetVltToRst[4];
		static St_DisposableTask SetVltToVlt[10];
		static St_DisposableTask SetVltToCur[9];
#if (IV_AUTORANGE == 1)
	static St_DisposableTask SetAutoRangeSwitch[8];
#endif
// 	static unsigned char SetCtrlOpenCh_Len;
// 	static unsigned char SetCtrlCloseCh_Len;
//    static unsigned char SetCtrlCloseChToRest_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;
//   static unsigned char SetOffToRst_Len;
// 	static unsigned char SetVoltRelay_Len;

    static unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

    static void AdditionalEngine(void);
    static void EngineStuff(void);
		static void Engine(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);
		static unsigned char Set_Volt_Range(unsigned char uc_ChannelNo,unsigned char VoltRange);

// 	static  void WritePGABus(char chn,uint16 PGABD);
// 	static  uint32 ReadInfoBus(char chn);
    static void Feed_CpldWD(void);    //LBT22013喂狗信号  大概6s一次
    static void Adjust_ADC_Gain(char type,unsigned char chnType);
    static char RedundantSafety(unsigned long CH_Number,unsigned long type);
    static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
    static char TemperatureSafety(unsigned long CH_Number,unsigned long type);

public:
    static SSPI_Para     LBT22013_SSP0DAC;
    static SSPI_Para     LBT22013_SSP1ADC;

    static const St_Port PinGroupVA[LBT22013_VA_Total_Pins];
    static const St_Port PinGroupVB[LBT22013_VB_Total_Pins];

    static unsigned char VoltageRangeFlag[MAXCHANNELNO];
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);

    //四个基本操作
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
// 	static void ADC_ADG_open(unsigned char ChannelIdx);
    static void ADG_Reset(void);

    static void changeADCRange(unsigned char range);

    static void LBT22013_FeatureEnable_Init(void);
    static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);
    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
    static unsigned char PowerOffCheck(void);
};


