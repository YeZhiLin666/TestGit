#ifndef _IBT_Base_
#define _IBT_Base_

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

//#define DCS3 0x0001


//2012 11 26    现在使用的newArbinBus 1126版本  使用必须按照锁存时序操作
//即通道DO输出锁存时序    ADC开关选择时序    和16位通道信息读入时序

//短延迟
#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );
//Arbin Bus Write
#define cpu_iowr_8(ADDR,DATA)   *((volatile uint8 *)(ADDR)) = (DATA)
#define cpu_iowr_16(ADDR,DATA)  *((volatile uint16 *)(ADDR)) = (DATA)
#define cpu_iowr_32(ADDR,DATA)  *((volatile uint32 *)(ADDR)) = (DATA)
//Arbin bus read
#define cpu_iord_8(ADDR)        *((volatile uint8 *)(ADDR))
#define cpu_iord_16(ADDR)       *((volatile uint16 *)(ADDR))
#define cpu_iord_32(ADDR)       *((volatile uint32 *)(ADDR))

//DO data    最终结果就是把data当成数据
//ADC

//这部分可以参考arbin bus

//--------------------------------for DAC------------------------------//
//comment out chen
#define DAC_LOAD1      (1<<3)    //P1.3
#define DAC_CLR        (1<<5)    //P1.5
#define DAC_RST        (1<<25)  //P1.25
#define DAC_BUSY3      (1<<2)    //P1.2

#define ADC_CONV       (1<<28)   //P1.28
#define BUSY1          (1<<12)   //P2.12
#define BUSY2          (1<<13)   //P2.13
#define DCS1           (1<<21)   //P1.21
#define DCS2           (1<<6)
#define DCS3            0x0001

enum
{
	IBT_Pin_AdcConver,
	IBT_Pin_AdcBusy1,
	IBT_Pin_AdcBusy2,
	IBT_Pin_AdcCS1,
	IBT_Pin_AdcCS2,
	IBT_Pin_AdcRst,
	IBT_Pin_DacCS,
	IBT_Pin_DacRst,
	IBT_Pin_DacLoad,
	IBT_Pin_DacBusy,
	IBT_Pin_WD,
//	IBT_Pin_SCK0,
//	IBT_Pin_MISO0,
//	IBT_Pin_MOSI0,
	IBT_Pin_SCK1,
	IBT_Pin_MISO1,
	IBT_Pin_MOSI1,
	IBT_Pin_PowerOn,
	IBT_Pin_AdgRst,
	IBT_Pin_Cali_B,
	IBT_Total_Pins,
};

typedef struct
{
	unsigned short   AdcGain:2;
	unsigned short   Reserved:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved2:6;
	unsigned short   R_Cali:1;
	unsigned short   R_ISO:1;
	unsigned short   Reserved3:2;
	unsigned short   R_CHL:1;
}St_IBT_Relay_Bit;
enum
{
	IBT_RLY_Cali = 0x800,     
	IBT_RLY_ISO  = 0x1000,         
	IBT_RLY_CHL  = 0x8000,
};//这个enum需要和上面的BIT结构保持高度一致   Cali -->Volt Range    CHL-> OUT   ISO-> VIR

typedef union
{
	unsigned short   Reg;
	St_IBT_Relay_Bit Bits;
}St_IBT_Relay;

typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_IBT_PGA_Bit;
typedef union
{
	unsigned short   Reg;
	St_IBT_PGA_Bit   Bits;
}St_IBT_PGA;

////////////////
enum
{
	IBT_SSP0_DAC_NAME = 0xA0,
	IBT_SSP0_DAC_CLAMP,
	IBT_SSP0_FRAM_NAME,
	IBT_SSP1_ADI_NAME,
	IBT_SSP1_ADV_NAME	
};
enum
{
	IBT_ADC_TYPE_I = 0xA0,
	IBT_ADC_TYPE_V,
};
enum
{
	IBT_ADC_IDLE,
	IBT_ADC_OpenADG,
	IBT_ADC_OPENING,
	IBT_ADC_TriggerI,
	IBT_ADC_ReadI,
	IBT_ADC_ReadV,
	IBT_ADC_NoWork,
};




class IBT_Base
{
private:
	static volatile St_IBT_Relay IBT_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[2];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[2];    //关闭通道
	static St_DisposableTask SetCCToCC[2];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing
	//static St_DisposableTask SetCVToCV[2];    

	static unsigned char SetCtrlOpenCh_Len;
	static unsigned char SetCtrlCloseCh_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;        
	static unsigned char SetCVToCV_Len;    

	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;

	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);

	//software
	static void Func_Init(void);

	//    static void AdditionalEngine(void);
	static  void EngineStuff(void);
	static  void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);
	static  char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);
	static  void changeADCRange(unsigned char ChannelIdx,unsigned char range);
	static  void Feed_CpldWD(void);    //IBT喂狗信号  大概6s一次
    static void IBT_FeatureEnable_Init(void);  //add by xudong
public:
	static SSPI_Para     IBT_SSP0DAC;
	static SSPI_Para     IBT_SSP1ADC;

	static const St_Port PinGroup[IBT_Total_Pins];

	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);

	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

	//给IBT_SampleMagage.cpp使用
	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);
};

#endif

