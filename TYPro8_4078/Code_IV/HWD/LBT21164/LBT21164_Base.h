#pragma once

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

#define  LBT21164_USE_5VDACADC                 0      //???1,??DAC?5V;  ???0,??DAC?10V;
#define  LBT21164_I2C_USE_TASK                 0

#define LBT21164_POWER_OFF_PORT    1
#define LBT21164_POWER_OFF_PIN    11

typedef struct
{
	unsigned short   Reserved1:2;
	unsigned short   A1:1;
	unsigned short   A0:1;
	unsigned short   L:1;
	unsigned short   Reserved2:6;
	unsigned short   Cali:1;
	unsigned short   Reserved3:4;
}St_LBT21164_Relay_Bit;

typedef struct
{
	unsigned short   Protect:1;
	unsigned short   Output:1;
	unsigned short   Reserved:14;
}St_LBT21164_Relay_Bit2;
typedef union
{
	St_LBT21164_Relay_Bit2   m_Bits;
	unsigned short           m_Reg;
}Un_LBT21164_Relay2;
enum
{
	LBT21164_RLY_Protect  = (0x01 << 0),
	LBT21164_RLY_Output   = (0X01 << 1),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
	unsigned short        Reg;
	St_LBT21164_Relay_Bit Bits;
}St_LBT21164_Relay;

typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_LBT21164_PGA_Bit;
typedef union
{
	unsigned short       Reg;
	St_LBT21164_PGA_Bit   Bits;
}St_LBT21164_PGA;

////////////////
enum
{
	LBT21164_SSP0_DAC_NAME = 0xA0,
	LBT21164_SSP0_AD5754_NAME,
	LBT21164_SSP1_ADI_NAME,
	LBT21164_SSP1_ADV_NAME ,
	LBT21164_SSP0_FRAM_NAME  //AD5754
};

enum
{
	LBT21164_ADC_IDLE,
	LBT21164_ADC_OpenADG,
	LBT21164_ADC_OPENING,
	LBT21164_ADC_CONV,
	LBT21164_ADC_TriggerI,
	LBT21164_ADC_ReadI,
	LBT21164_ADC_TriggerV,
	LBT21164_ADC_ReadV,
	LBT21164_ADC_NoWork,
}; 
enum
{
	LBT21164_HIGH_RANGE_NUM = 0xF1,
	LBT21164_MID_RANGE_NUM = 0xF2,
	LBT21164_LOW_RANGE_NUM = 0xF3,
};
typedef struct 
{
	unsigned char LowRange:1;  //if Need fan control;
	unsigned char Rng20:1;    //20%;
	unsigned char Rng40:1;    //40%;
	unsigned char Rng50:1;    //50%;
	unsigned char Rng60:1;    //60%;
	unsigned char Rng70:1;    //70%;
	unsigned char Rng90:1;    //90%;
	unsigned char Rng100:1;   //100%;
}St_FanRngBit;
typedef union
{
	St_FanRngBit  Bits;
	unsigned char Reg;
}Un_FanRng;
typedef struct
{
	Un_FanRng     Last_Duty[MAXCHANNELNO];
	Un_FanRng     Cur_Duty;
	Un_FanRng     New_Duty;
	unsigned long LastSetTime_Second; 
}St_LBT21164_FanCtrl;

class LBT21164_Base
{
private:
	static volatile St_LBT21164_Relay LBT21164_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);

	//software
	static void Func_Init(void);
	static void regCHNInfo(unsigned char CH_Number,unsigned long type);
	static void regEngine(unsigned char CH_Number,unsigned long type);
	static void EngineStuff(void);
	static  void WritePGABus(char chn,uint16 PGABD);
	static  uint32 ReadInfoBus(char chn);

public:
	static const St_Port Pin_DacRst;
	static const St_Port Pin_DacCS;
	static const St_Port Pin_DacLoad; 
	static const St_Port Pin_DacBusy;

	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[6];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[3];    //关闭通道
	static St_DisposableTask SetCCToCC[5];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing

	static unsigned char SetCtrlOpenCh_Len;
	static unsigned char SetCtrlCloseCh_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;        

	static SSPI_Para     LBT21164_SSP0DAC;       //DAC
	static SSPI_Para     LBT21164_SSP0VCDAC;    //VCLAMP
	static SSPI_Para     LBT21164_SSP1ADC;

	static St_LBT21164_FanCtrl m_FanCtrl; 

	static const St_Port Pin_AdcConver;
	static const St_Port Pin_AdcBusy1;
	static const St_Port Pin_AdcBusy2;
	static const St_Port Pin_AdcCS1;
	static const St_Port Pin_AdcCS2;
	static const St_Port Pin_AdcRst;
	static const St_Port Pin_Clamp_Dac_Bin;
	static const St_Port Pin_Clamp_Dac_CS;
	static const St_Port Pin_Clamp_Dac_Load; 
	static const St_Port Pin_Clamp_Dac_Clr;
	static const St_Port Pin_WD;
	static const St_Port Pin_SCK0;
	static const St_Port Pin_MISO0;
	static const St_Port Pin_MOSI0;
	static const St_Port Pin_SCK1;
	static const St_Port Pin_MISO1;
	static const St_Port Pin_MOSI1;
	static const St_Port Pin_PowerOn;
	static const St_Port Pin_PowerOff;
	static const St_Port Pin_RedundantSafety;
	static const St_Port Pin_FanCtrl;

	static unsigned short      m_ChannelOn;

	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;
	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);
	static void Engine(void);

	//四个基本操作
	static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
	static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);
	static void changeADCRange(unsigned char range);

	static void Feed_CpldWD(void);    //LBT21164喂狗信号  大概6s一次
	static void Adjust_ADC_Gain(char type,char chnType);

	static char RedundantSafety(unsigned long CH_Number,unsigned long type);
	static char ReleaseRedundantSafety(unsigned long  uc_ChannelNo,unsigned long type);	
	static void SetFanDuty_RNG(unsigned long uc_ChannelNo,unsigned char CurrRange);
	static void SetFanDuty_DAC(unsigned long uc_ChannelNo,unsigned long DacCode);
	static void SetFanDuty_PWM(void);
	static void SetChannelON(void);
};


