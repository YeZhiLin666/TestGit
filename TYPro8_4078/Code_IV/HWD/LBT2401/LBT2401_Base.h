#pragma once

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

#define MY_PWMFANCHANNEL     5
#define LBT2401_POWER_OFF_PORT    3
#define LBT2401_POWER_OFF_PIN    21

#define  LBT2401_R_Voltage  (0x01<<0)
#define  LBT2401_R_CH       (0x01<<2)
#define  LBT2401_R_CM       (0x01<<3)
#define  LBT2401_R_CL       (0x01<<4)
#define  LBT2401_R_OFFGH    (0x01<<6)
#define  LBT2401_R_OFFGL    (0x01<<7)
#define  LBT2401_R_OFFGM    (0x01<<9)
#define  LBT2401_P_R        (0x01<<14)
typedef struct
{
	unsigned short   R_Voltage:1;       //0 硬件控制单电压  CPLD bit
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
}St_LBT2401_Relay_Bit;

typedef struct
{
	unsigned short   Protect:1;
	unsigned short   OffGate:1;
	unsigned short   Output:1;
	unsigned short   Voltage:1;          // 硬件控制单电压  CPLD bit
	unsigned short   Reserved:12;
}St_LBT2401_Relay_Bit2;
typedef union
{
	St_LBT2401_Relay_Bit2   m_Bits;
	unsigned short          m_Reg;
}Un_LBT2401_Relay2;
enum
{
	LBT2401_RLY_Protect  = (0x01 << 0),
	LBT2401_RLY_OffGate  = (0X01 << 1),
	LBT2401_RLY_Output   = (0X01 << 2),
	LBT2401_RLY_VOLTAGE  = (0X01 << 3),   // 硬件控制单电压  CPLD bit
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
	unsigned short   Reg;
	St_LBT2401_Relay_Bit Bits;
}St_LBT2401_Relay;

typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_LBT2401_PGA_Bit;
typedef union
{
	unsigned short       Reg;
	St_LBT2401_PGA_Bit   Bits;
}St_LBT2401_PGA;

////////////////
enum
{
	LBT2401_SSP0_DAC_NAME = 0xA0,
	LBT2401_SSP0_AD5754_NAME,
	LBT2401_SSP1_ADI_NAME,
	LBT2401_SSP1_ADV_NAME ,
	LBT2401_SSP0_FRAM_NAME  //AD5754
};

enum
{
	LBT2401_ADC_IDLE,
	LBT2401_ADC_WAIT,
	LBT2401_ADC_TriggerI,
	LBT2401_ADC_ReadI,
	LBT2401_ADC_TriggerV,
	LBT2401_ADC_ReadV,
}; 
enum
{
	LBT2401_HIGH_RANGE_NUM = 0xF1,
	LBT2401_MID_RANGE_NUM = 0xF2,
	LBT2401_LOW_RANGE_NUM = 0xF3,
};
enum
{
	LBT2401_Pin_AdcConver,
	LBT2401_Pin_AdcBusy1,
	LBT2401_Pin_AdcBusy2,
	LBT2401_Pin_AdcCS1, 
	LBT2401_Pin_AdcCS2,  
	LBT2401_Pin_AdcRst, 
	LBT2401_Pin_Dac_Bin, 
	LBT2401_Pin_Dac_CS,
	LBT2401_Pin_Dac_Load,
	LBT2401_Pin_Dac_Clr,
	LBT2401_Pin_WD, 
//	LBT2401_Pin_SCK0, 
//	LBT2401_Pin_MISO0, 
//	LBT2401_Pin_MOSI0, 
	LBT2401_Pin_SCK1, 
	LBT2401_Pin_MISO , 
	LBT2401_Pin_MOSI1, 
	LBT2401_Pin_PowerOn, 
	LBT2401_Pin_AdgRst,
	LBT2401_PWM_FAN,	
	LBT2401_Pin_RedundantSafety,
	LBT2401_Total_Pins,
};

class LBT2401_Base
{
private:
	static volatile St_LBT2401_Relay LBT2401_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[6];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[5];    //关闭通道
	static St_DisposableTask SetCCToCC[7];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing
	static St_DisposableTask SetVoltRelay[2];     //设置冗余电压

	static unsigned char SetCtrlOpenCh_Len;
	static unsigned char SetCtrlCloseCh_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;        
	static unsigned char SetVoltRelay_Len;

	//     static const St_Port Pin_WD;
	//     static const St_Port Pin_SCK0;
	//     static const St_Port Pin_MISO0;
	//     static const St_Port Pin_MOSI0;
	//     static const St_Port Pin_SCK1;
	//     static const St_Port Pin_MISO1;
	//     static const St_Port Pin_MOSI1;
	//     static const St_Port Pin_PowerOn;

	//     static const St_Port Pin_AdgRst;
	//     static const St_Port PWM_FAN;
	//     static const St_Port Pin_RedundantSafety;
	//static const St_Port Pin_ClpDacCS;   //clmap dac
	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;

	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);
    static void LBT2401_FeatureEnable_Init(void);
	//software
	static void Func_Init(void);

	// 	  static void regCHNInfo(unsigned char CH_Number,unsigned long type);
	// 	  static void regEngine(unsigned char CH_Number,unsigned long type);
	static void AdditionalEngine(void);
	static void EngineStuff(void);
	static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

	static  void WritePGABus(char chn,uint16 PGABD);
	static  uint32 ReadInfoBus(char chn);
	static void Feed_CpldWD(void);    //LBT2401喂狗信号  大概6s一次
	static void Adjust_ADC_Gain(char type,char chnType);

	static char RedundantSafety(unsigned long CH_Number,unsigned long type);
	static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);

public:
	//     static const St_Port Pin_Dac_Bin;
	//     static const St_Port Pin_Dac_CS;
	//     static const St_Port Pin_Dac_Load; 
	//     static const St_Port Pin_Dac_Clr;

	static SSPI_Para     LBT2401_SSP0DAC;
	static SSPI_Para     LBT2401_SSP1ADC;

	//     static const St_Port Pin_AdcConver;
	//     static const St_Port Pin_AdcBusy1;
	//     static const St_Port Pin_AdcBusy2;
	//     static const St_Port Pin_AdcCS1;
	//     static const St_Port Pin_AdcCS2;
	//     static const St_Port Pin_AdcRst;


	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);
	static const St_Port PinGroup[LBT2401_Total_Pins];


	//四个基本操作
	// 		static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);


	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);

	static void changeADCRange(unsigned char range);
};


