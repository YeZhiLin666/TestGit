#pragma once

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

#define LBT21084VB_POWER_OFF_PORT    1
#define LBT21084VB_POWER_OFF_PIN    11

#define LBT21084VB_RLY_R_CLL   (0x01<< 1)
#define LBT21084VB_RLY_R_CH    (0x01<< 2)
#define LBT21084VB_RLY_R_CM    (0x01<< 3)
#define LBT21084VB_RLY_R_CL    (0x01<< 4)
#define LBT21084VB_RLY_R_VM    (0x01<< 11)
#define LBT21084VB_RLY_P_R     (0x01<< 14)

typedef struct
{
	unsigned short   Reserved1:1;
	unsigned short   R_CLL:1;
	unsigned short   R_CH:1;
	unsigned short   R_CM:1;
	unsigned short   R_CL:1;
	unsigned short   Reserved2:3;
	unsigned short	 R_ACIM:1;
	unsigned short   Reserved5:2;
	unsigned short   R_VM:1;
	unsigned short   Reserved4:2;
	unsigned short   P_R:1;
	unsigned short   Reserved3:1;
}St_LBT21084VB_Relay_Bit;

typedef struct
{
	unsigned short   Protect:1;
	unsigned short   Output:1;
	unsigned short   Reserved1:6;
	unsigned short   Acim:1;
	unsigned short   Reserved2:7;
}St_LBT21084VB_Relay_Bit2;
typedef union
{
	St_LBT21084VB_Relay_Bit2   m_Bits;
	unsigned short          m_Reg;
}Un_LBT21084VB_Relay2;
enum
{
	LBT21084VB_RLY_Protect  = (0x01 << 0),
	LBT21084VB_RLY_Output   = (0X01 << 1),
	LBT21084VB_RLY_ACIM   	= (0x01 << 8),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
	unsigned short   Reg;
	St_LBT21084VB_Relay_Bit Bits;
}St_LBT21084VB_Relay;

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
}St_LBT21084VB_DIDO_Bit;

typedef union
{
	unsigned short   Reg;
	St_LBT21084VB_DIDO_Bit Bits;
}St_LBT21084VB_DIDO;


typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_LBT21084VB_PGA_Bit;
typedef union
{
	unsigned short       Reg;
	St_LBT21084VB_PGA_Bit   Bits;
}St_LBT21084VB_PGA;

////////////////
enum
{
	LBT21084VB_SSP0_DAC_NAME = 0xA0,
    LBT21084VB_SSP0_AD5754_NAME,
	LBT21084VB_SSP1_ADI_NAME,
	LBT21084VB_SSP1_ADV_NAME ,
	LBT21084VB_SSP0_FRAM_NAME  //AD5754
};

enum
{
    LBT21084VB_ADC_IDLE,
    LBT21084VB_ADC_OpenADG,
	LBT21084VB_ADC_OPENING,
	LBT21084VB_ADC_CONV,
	LBT21084VB_ADC_TriggerI_V,
	LBT21084VB_ADC_ReadI,
	LBT21084VB_ADC_TriggerV,
	LBT21084VB_ADC_ReadV,
	LBT21084VB_ADC_CloseADG,
	LBT21084VB_ADC_NoWork,
}; 
enum
{
	LBT21084VB_HIGH_RANGE_NUM = 0xF1,
	LBT21084VB_MID_RANGE_NUM = 0xF2,
	LBT21084VB_LOW_RANGE_NUM = 0xF3,
};

enum
{
	LBT21084VB_Pin_AdcSTART = 0,
	LBT21084VB_Pin_AdcDRDY1,
	LBT21084VB_Pin_AdcDRDY2,
	LBT21084VB_Pin_AdcCS1,
	LBT21084VB_Pin_AdcCS2,
	//LBT21084VB_Pin_AdcRst,
	LBT21084VB_Pin_DacCS,

	LBT21084VB_Pin_DacRst,
	LBT21084VB_Pin_DacLoad,
	LBT21084VB_Pin_DacBusy,
    
    // clamp
	LBT21084VB_Clamp_Dac_Bin,
	LBT21084VB_Clamp_Dac_CS,
	LBT21084VB_Clamp_Dac_Load,
    LBT21084VB_Clamp_Dac_Clr,
	LBT21084VB_Pin_WD,
    
//	LBT21084VB_Pin_SCK0,
//	LBT21084VB_Pin_MISO0,
//	LBT21084VB_Pin_MOSI0,
	LBT21084VB_Pin_SCK1,
	LBT21084VB_Pin_MISO1,
	LBT21084VB_Pin_MOSI1,
    
	LBT21084VB_Pin_PowerOn,
	LBT21084VB_Pin_AdgRst,
    LBT21084VB_Pin_RedundantSafety,
	LBT21084VB_Pin_FAN,
    //#if(FAN_SAFETY_MODE==1)
    LBT21084VB_Pin_FanCheckSignal,
    //#endif
	LBT21084VB_Total_Pins,
};



class LBT21084VB_Base
{
private:
    
    //#if(FAN_SAFETY_MODE==1)
    static MsTime     FanOverTime;	
    //#endif


    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[3];    //关闭通道
    static St_DisposableTask SetCCToCC[4];         //CC to CC
    static St_DisposableTask SetRstToOff[3];       //do nothing
	static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
	static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay

    static unsigned char SetCtrlOpenCh_Len;
    static unsigned char SetCtrlCloseCh_Len;
    static unsigned char SetCCToCC_Len;
    static unsigned char SetRstToOff_Len; 

    //static const St_Port Pin_SCK0;
    //static const St_Port Pin_MISO0;
    //static const St_Port Pin_MOSI0;
    //static const St_Port Pin_SCK1;
    //static const St_Port Pin_MISO1;
    //static const St_Port Pin_MOSI1;
    //static const St_Port Pin_PowerOn;
    //static const St_Port Pin_AdgRst;
    //static const St_Port Pin_RedundantSafety;
    //static const St_Port Pin_WD;
	//static const St_Port Pin_FAN;    
    static volatile St_LBT21084VB_DIDO     CPLD_DO;    //记录DO的channel状态
    static volatile St_LBT21084VB_DIDO     CPLD_DI;    //记录DI的channel状态
    //#if(FAN_SAFETY_MODE==1)
    //static const St_Port Pin_FanCheckSignal;
    //#endif
    //static const St_Port Pin_ClpDacCS;   //clmap dac
    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
    static const unsigned char m_bChannelBasedClampVdac;

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

//     static void regCHNInfo(unsigned char CH_Number,unsigned long type);
//     static void regEngine(unsigned char CH_Number,unsigned long type);

    static void Engine(void);
	static void AdditionalEngine(void);
    static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

    static  void WriteDOBus(char chn,uint16 BD);
    static  void WritePGABus(char chn,uint16 PGABD);
    static  uint32 ReadInfoBus(char chn);

    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
    static void changeADCRange(unsigned char range);

    static void Feed_CpldWD(void); 
    static char RedundantSafety(unsigned long CH_Number,unsigned long type);
    static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);
	static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);

    //#if(FAN_SAFETY_MODE==1)
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
    //#endif

    static void Adjust_ADC_Gain(char type,char chnType);
	static void LBT21084VB_FeatureEnable_Init(void); 
public:
	static void ADC_Init(void);
	static volatile St_LBT21084VB_Relay LBT21084VB_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
    //static const St_Port Pin_DacRst;
    //static const St_Port Pin_DacCS;
    //static const St_Port Pin_DacLoad; 
    //static const St_Port Pin_DacBusy;


    static SSPI_Para     LBT21084VB_SSP0DAC;       //DAC
    static SSPI_Para     LBT21084VB_SSP0VCDAC;    //VCLAMP
    static SSPI_Para     LBT21084VB_SSP1ADC;

    //static const St_Port Pin_AdcSTART;
    //static const St_Port Pin_AdcDRDY1;
    //static const St_Port Pin_AdcDRDY2;
    //static const St_Port Pin_AdcCS1;
    //static const St_Port Pin_AdcCS2;
//     static const St_Port Pin_AdcRst;
    //static const St_Port Pin_Clamp_Dac_Bin;
    //static const St_Port Pin_Clamp_Dac_CS;
    //static const St_Port Pin_Clamp_Dac_Load; 
    //static const St_Port Pin_Clamp_Dac_Clr;
	static const St_Port PinGroup[LBT21084VB_Total_Pins];



    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);


    //四个基本操作
    // 		static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);



    static  void ADC_ADG_open(unsigned char ChannelIdx);
    static  void ADG_Reset(void);

    //LBT21084VB喂狗信号  大概6s一次

    static unsigned char getBID(void);    //获得拨码开关
	
		
		
};


