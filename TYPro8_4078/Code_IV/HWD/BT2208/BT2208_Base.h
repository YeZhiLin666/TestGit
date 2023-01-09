#ifndef _BT2208_Base_
#define _BT2208_Base_

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"
//#include "../../COMMON/includes.h"


#define BT2208_IO_NUM 55


// //具体请参考文档 地址空间规划
#define DCS3 0x0001


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
#define DCS3  0x0001

typedef struct
{
	unsigned short   AdcGain:2;
	unsigned short   Reserved:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved2:6;
	unsigned short   R_Cali:1;
	unsigned short   R_ISO:1;
	unsigned short   Reserved3:1;    
	unsigned short   P_R:1;
	unsigned short   R_CHL:1;
}St_BT2208_Relay_Bit;
enum
{
	BT2208_RLY_Cali   = (0x01 << 11),
	BT2208_RLY_ISO    = (0x01 << 12),
	BT2208_RLY_Prot   = (0x01 << 14),
	BT2208_RLY_CHL    = (0x01 << 15),
};
typedef union
{
	unsigned short   Reg;
	St_BT2208_Relay_Bit Bits;
}St_BT2208_Relay;

typedef struct
{
	unsigned short   AdcGain_I:2;
	unsigned short   AdcGain_V:2;
	unsigned short   AdcGainEnable:1;
	unsigned short   Reserved:11;
}St_BT2208_PGA_Bit;
typedef union
{
	unsigned short   Reg;
	St_BT2208_PGA_Bit   Bits;
}St_BT2208_PGA;

////////////////
enum
{
	BT2208_SSP0_DAC_NAME = 0xA0,
	BT2208_SSP0_DAC_CLAMP,
	BT2208_SSP0_FRAM_NAME,
	BT2208_SSP1_ADI_NAME,
	BT2208_SSP1_ADV_NAME	
};

enum
{
	BT2208_ADC_IDLE,
	BT2208_ADC_OpenADG,
	BT2208_ADC_OPENING,
	BT2208_ADC_CONV,
	BT2208_ADC_TriggerI,
	BT2208_ADC_ReadI,
	BT2208_ADC_TriggerV,
	BT2208_ADC_ReadV,
	BT2208_ADC_CloseADG,
	BT2208_ADC_NoWork,
};

#define     BT2208_821_CHN_NUM  8    ///change to 9chn
#define     BT2208_1_CHN_NUM     14  //15chn为当成1个通道来用

enum
{
	BT2208_HIGH_RANGE_NUM = 0xF1,
	BT2208_MID_RANGE_NUM = 0xF2,
	BT2208_LOW_RANGE_NUM = 0xF3,
};
enum
{
	BT2208_Pin_AdcConver,
	BT2208_Pin_AdcBusy1,
	BT2208_Pin_AdcBusy2,
	BT2208_Pin_AdcCS1,
	BT2208_Pin_AdcCS2,
	BT2208_Pin_AdcRst,
	BT2208_Pin_DacCS,
	BT2208_Pin_DacRst,
	BT2208_Pin_DacLoad,
	BT2208_Pin_DacBusy,
	BT2208_Pin_WD,
	BT2208_Pin_SCK0,
	BT2208_Pin_MISO0,
	BT2208_Pin_MOSI0,
	BT2208_Pin_SCK1,
	BT2208_Pin_MISO1,
	BT2208_Pin_MOSI1,
	BT2208_Pin_PowerOn,
	BT2208_Pin_AdgRst,
	BT2208_Total_Pins,
};
class BT2208_Base
{
private:
	static volatile St_BT2208_Relay BT2208_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
	static unsigned char whichRange;    //哪个量程

	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);

	//software
	static void Func_Init(void);

//	static void regCHNInfo(unsigned char CH_Number,unsigned long type);
//	static void regEngine(unsigned char CH_Number,unsigned long type);

	static void EngineStuff(void);

	static  void WritePGABus(char chn,uint16 PGABD);
	static  uint32 ReadInfoBus(char chn);

public:
	static unsigned char MyIO_BT2208[6][4];

	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[4];    //关闭通道
	static St_DisposableTask SetCCToCC[10];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing

	//    static St_DisposableTask SetOpenISO[2];    //电压打开ISO    //comment out chen 20130123
	//    static St_DisposableTask SetCloseISO[2];    //电压关闭ISO    //comment out chen 20130123
	//    static St_DisposableTask SetCVToCV[2];     //comment out chen 20130123

	static unsigned char SetCtrlOpenCh_Len;
	static unsigned char SetCtrlCloseCh_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;        
	static unsigned char SetOpenISO_Len;    //comment out chen 20130123
	static unsigned char SetCloseISO_Len;    //comment out chen 20130123
	static unsigned char SetCVToCV_Len;     //comment out chen 20130123

	static SSPI_Para     BT2208_SSP0DAC;
	static SSPI_Para     BT2208_SSP1ADC;
	static const St_Port PinGroup[BT2208_Total_Pins];
	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;
	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);
	//    static void AdditionalEngine(void);

	//四个基本操作
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
	static char Relay_on(unsigned long ChannelIdx,unsigned long type);
	static char Relay_off(unsigned long ChannelIdx,unsigned long type);


	//暂时放到publice里   
	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

	//给BT2208_SampleMagage.cpp使用
	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);

	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

	static void Feed_CpldWD(void);    //BT2208喂狗信号  大概6s一次
};

#endif

