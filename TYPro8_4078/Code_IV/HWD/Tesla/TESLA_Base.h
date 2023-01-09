#pragma once

// #include "../../IV_Tasks/OS_LogicProcess.h"
// #include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
// #include "../IV_Tasks/BSP.h"
// #include "../../common_OS/Data_Manipulation/Datatypes.h"

// #define TESLA_IO_NUM 55
// #define TESLA_POWER_OFF_PORT 2
// #define TESLA_POWER_OFF_PIN    0

#define TESLASETLOWPULSEDACTIME  50 
#define TESLASETLOWPULSEADCTIME  (100-TESLASETLOWPULSEDACTIME)
#define TESLASETHIGHPULSEDACTIME 50
#define TESLASETHIGHPULSEADCTIME (100-TESLASETHIGHPULSEDACTIME)
#define TESLAMINIMUMPULSE      100

// #define DCS4  0x0002    //AD5754

typedef struct
{
	unsigned short   Vin:1;
	unsigned short   IH:1;
	unsigned short   EN:1;
	unsigned short   PRT:1;
	unsigned short   CAL:1;
	unsigned short   Reserved:11;
}St_Tesla_Relay_Bit;    //total 16bit
enum
{
	TESLA_RLY_Vin  = (0x01 << 0),
	TESLA_RLY_IH   = (0x01 << 1),
	TESLA_RLY_EN   = (0x01 << 2),
	TESLA_RLY_PRT  = (0x01 << 3),
	TESLA_RLY_CALi = (0x01 << 4),
};
typedef union
{
	unsigned short   Reg;
	St_Tesla_Relay_Bit Bits;
}St_Tesla_Relay;

enum
{
	TESLA_SSP0_DAC_NAME = 0xA0,
	TESLA_SSP0_AD5754_NAME, //AD5754
	TESLA_SSP1_ADI_NAME,
	TESLA_SSP1_ADV_NAME ,
	TESLA_SSP0_FRAM_NAME  //AD5754
};

//采样状态机
enum
{
	TESLA_ADC_IDLE,
	TESLA_ADC_OpenADG,
	TESLA_ADC_OPENING,
	TESLA_ADC_TriggerI,
	TESLA_ADC_ReadI,
	TESLA_ADC_ReadV,
	TESLA_ADC_NoWork,
};
enum
{
	TESLA_Pin_AdcConver,
	TESLA_Pin_AdcBusy1,
	TESLA_Pin_AdcBusy2,
	TESLA_Pin_AdcCS1,
	TESLA_Pin_AdcCS2,
	TESLA_Pin_AdcRst,
	TESLA_Pin_DacCS,
	TESLA_Pin_DacRst,
	TESLA_Pin_DacLoad,
	TESLA_Pin_DacBusy,
	TESLA_Pin_Clamp_Dac_Bin,
	TESLA_Pin_Clamp_Dac_CS,
	TESLA_Pin_Clamp_Dac_Load,
	TESLA_Pin_Clamp_Dac_Clr,
	TESLA_Pin_WD,
//	TESLA_Pin_SCK0,
//	TESLA_Pin_MISO0,
//	TESLA_Pin_MOSI0,
	TESLA_Pin_SCK1,
	TESLA_Pin_MISO1,
	TESLA_Pin_MOSI1,
	TESLA_Pin_AdgRst,
	TESLA_Pin_PowerOff,
	TESLA_Total_Pins,
};
//由16转变成2个1-4
typedef struct
{
	unsigned char chId;
	unsigned char Bid;
}st_Tesla_ADD;

class TESLA_Base
{
private:
	static volatile St_Tesla_Relay Tesla_relaystatus[MAXCHANNELNO];

	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[2];    //关闭通道
	static St_DisposableTask SetCCToCC[2];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing
	//static St_DisposableTask SetCVToCV[2];     //comment out chen 20130123

// 	static unsigned char SetCtrlOpenCh_Len;
// 	static unsigned char SetCtrlCloseCh_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;        
// 	static unsigned char SetCVToCV_Len;     //comment out chen 20130123

	static St_DisposableTask VCaliWithNOEn_On[2];
	static St_DisposableTask VCaliWithNOEn_Off[2];
// 	static unsigned char VCaliWithNOEn_On_Len;
// 	static unsigned char VCaliWithNOEn_Off_Len;

	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;

	static void IO_Init(void);
	//hardware  把这部分扔到ADC5DAC.cpp中
	static void ADC_Init(void);
	static void DAC_Init(void);

	//software
	static void Func_Init(void);

	static void AdditionalEngine(void);
	static void EngineStuff(void);
	static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);
	static  void Adjust_ADC_Gain(char type,char chnType);
	static void Feed_CpldWD(void);    //IBT喂狗信号  大概6s一次

	static unsigned char PowerOn(void);    //PowerOn
    static void TESLA_FeatureEnable_Init(void);
public:
	static SSPI_Para  Telsa_SSP0VDAC;    //DAC
	static SSPI_Para  Tesla_SSP0VCDAC;    //VCLAMP
	static SSPI_Para  Tesla_SSP1IADC;    //根据张军杰的外设不占时间，不考虑SSP的CS口，comment out chen 20130702

	static const St_Port PinGroup[TESLA_Total_Pins];

	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);

	static char DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type);

	//给TESLA_SampleMagage.cpp使用
	static  void ADC_ADG_open(char chn);
	static  void ADG_Reset(void);
  static unsigned char PowerOffCheck(void);
};



