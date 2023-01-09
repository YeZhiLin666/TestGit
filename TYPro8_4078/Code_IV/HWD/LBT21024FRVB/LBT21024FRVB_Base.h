#pragma once

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

#define LBT21024FRVB_POWER_OFF_PORT    1
#define LBT21024FRVB_POWER_OFF_PIN    11

#define LBT21024FRVB_RLY_R_CLL   (0x01<< 1)
#define LBT21024FRVB_RLY_R_CH    (0x01<< 2)
#define LBT21024FRVB_RLY_R_CM    (0x01<< 3)
#define LBT21024FRVB_RLY_R_CL    (0x01<< 4)
#define LBT21024FRVB_RLY_P_R     (0x01<< 14)

typedef struct
{
	unsigned short   Reserved1:1;
	unsigned short   R_CLL:1;
	unsigned short   R_CH:1;
	unsigned short   R_CM:1;
	unsigned short   R_CL:1;
	unsigned short   Reserved2:6;
	unsigned short   R_VM:1;
	unsigned short   Reserved4:2;
	unsigned short   P_R:1;
	unsigned short   Reserved3:1;
}St_LBT21024FRVB_Relay_Bit;

typedef struct
{
	unsigned short   Protect:1;
	//unsigned short   OffGate:1;
	unsigned short   Output:1;
	unsigned short   Reserved:14;
}St_LBT21024FRVB_Relay_Bit2;
typedef union
{
	St_LBT21024FRVB_Relay_Bit2   m_Bits;
	unsigned short          m_Reg;
}Un_LBT21024FRVB_Relay2;
enum
{
	LBT21024FRVB_RLY_Protect  = (0x01 << 0),
	LBT21024FRVB_RLY_Output   = (0X01 << 1),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
	unsigned short   Reg;
	St_LBT21024FRVB_Relay_Bit Bits;
}St_LBT21024FRVB_Relay;

typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_LBT21024FRVB_PGA_Bit;
typedef union
{
	unsigned short       Reg;
	St_LBT21024FRVB_PGA_Bit   Bits;
}St_LBT21024FRVB_PGA;

////////////////
enum
{
	LBT21024FRVB_SSP0_DAC_NAME = 0xA0,
    LBT21024FRVB_SSP0_AD5754_NAME,
	LBT21024FRVB_SSP1_ADI_NAME,
	LBT21024FRVB_SSP1_ADV_NAME ,
	LBT21024FRVB_SSP0_FRAM_NAME  //AD5754
};

enum
{
  LBT21024FRVB_ADC_IDLE,
  LBT21024FRVB_ADC_OpenADG,
	LBT21024FRVB_ADC_OPENING,
	LBT21024FRVB_ADC_CONV,
	LBT21024FRVB_ADC_TriggerI_V,
	LBT21024FRVB_ADC_ReadI,
	LBT21024FRVB_ADC_TriggerV,
	LBT21024FRVB_ADC_ReadV,
	LBT21024FRVB_ADC_CloseADG,
	LBT21024FRVB_ADC_NoWork,
}; 
enum
{
	LBT21024FRVB_Pin_AdcSTART=0,
	LBT21024FRVB_Pin_AdcDRDY1,
	LBT21024FRVB_Pin_AdcDRDY2,
	LBT21024FRVB_Pin_AdcCS1,
	LBT21024FRVB_Pin_AdcCS2,
//	LBT21024FRVB_Pin_AdcRst,
	LBT21024FRVB_Pin_DacCS,
	LBT21024FRVB_Pin_DacRst,
	LBT21024FRVB_Pin_DacLoad,
	LBT21024FRVB_Pin_DacBusy,
	LBT21024FRVB_Pin_WD,
//	LBT21024FRVB_Pin_SCK0,
//	LBT21024FRVB_Pin_MISO0,
//	LBT21024FRVB_Pin_MOSI0,
	LBT21024FRVB_Pin_SCK1,
	LBT21024FRVB_Pin_MISO1,
	LBT21024FRVB_Pin_MOSI1,
	LBT21024FRVB_Pin_FAN,
	LBT21024FRVB_Pin_AdgRst,
	LBT21024FRVB_Pin_RedundantSafety,
	//#if(FAN_SAFETY_MODE==1)
	LBT21024FRVB_Pin_FanCheckSignal,
	//#endif
	LBT21024FRVB_Total_Pins,
};

enum
{
	LBT21024FRVB_HIGH_RANGE_NUM = 0xF1,
	LBT21024FRVB_MID_RANGE_NUM = 0xF2,
	LBT21024FRVB_LOW_RANGE_NUM = 0xF3,
};

class LBT21024FRVB_Base
{
private:
    static volatile St_LBT21024FRVB_Relay LBT21024FRVB_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
    //#if(FAN_SAFETY_MODE==1)
    static MsTime     FanOverTime;	
    //#endif 

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

//     static void regCHNInfo(unsigned char CH_Number,unsigned long type);
//     static void regEngine(unsigned char CH_Number,unsigned long type);
    static void AdditionalEngine(void);
    static void EngineStuff(void);

    static  void WritePGABus(char chn,uint16 PGABD);
    static  uint32 ReadInfoBus(char chn);
    static void LBT21024FRVB_FeatureEnable_Init(void) ;

public:
	  static void ADC_Init(void);
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[4];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[2];    //关闭通道
    static St_DisposableTask SetCCToCC[5];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing
    static St_DisposableTask SetOffToRst[2];       //do nothing

    static unsigned char SetCtrlOpenCh_Len;
    static unsigned char SetCtrlCloseCh_Len;
    static unsigned char SetCCToCC_Len;
    static unsigned char SetRstToOff_Len;        
    static unsigned char SetOffToRst_Len; 

    static SSPI_Para     LBT21024FRVB_SSP0DAC;
    static SSPI_Para     LBT21024FRVB_SSP1ADC;

    static const St_Port PinGroup[LBT21024FRVB_Total_Pins];
    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
    static const unsigned char m_bChannelBasedClampVdac;
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);
	//    static void AdditionalEngine(void);

    //四个基本操作
//     static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

    static  void ADC_ADG_open(unsigned char ChannelIdx);
    static  void ADG_Reset(void);


    static void changeADCRange(unsigned char range);

    static void Feed_CpldWD(void);    //LBT21024FRVBFR喂狗信号  大概6s一次

    static void Adjust_ADC_Gain(char type,char chnType);

    static char RedundantSafety(unsigned long CH_Number,unsigned long type);
    static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);
    //#if(FAN_SAFETY_MODE==1)
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
    //#endif
    static char FanAdjOperature(unsigned long m_Level,unsigned long FanAdjMod);
};


