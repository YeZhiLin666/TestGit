#ifndef _LBT20083_Base_
#define _LBT20083_Base_

// #include "../../IV_Tasks/OS_LogicProcess.h"
// #include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
// #include "../IV_Tasks/BSP.h"
// #include "../../common_OS/Data_Manipulation/Datatypes.h"

//#define DCS3 0x0001

#define LBT20083SETLOWPULSEDACTIME  75
#define LBT20083SETLOWPULSEADCTIME  (100-LBT20083SETLOWPULSEDACTIME)
#define LBT20083SETHIGHPULSEDACTIME 75
#define LBT20083SETHIGHPULSEADCTIME (100-LBT20083SETHIGHPULSEDACTIME)
#define LBT20083MINIMUMPULSE     10

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

//关闭中断等操作


//这部分可以参考arbin bus

//--------------------------------for DAC------------------------------//
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

#define LBT20083_RLY_R_CH        (0x01<< 1)
#define LBT20083_RLY_R_CM        (0x01<< 2)
#define LBT20083_RLY_R_CL        (0x01<< 3)
#define LBT20083_RLY_R_Cali      (0x01<< 11)

typedef struct
{
    unsigned short   Reserved:1;
    unsigned short   R_CH:1;
    unsigned short   R_CM:1;
    unsigned short   R_CL:1;
    unsigned short   Reserved2:7;
    unsigned short   R_Cali:1;
    unsigned short   Reserved3:4;
} St_LBT20083_Relay_Bit;

typedef struct
{
    unsigned short   Output:1;
    unsigned short   Reserved:15;
} St_LBT20083_Relay_Bit2;
typedef union
{
    St_LBT20083_Relay_Bit2   m_Bits;
    unsigned short          m_Reg;
} Un_LBT20083_Relay2;
enum
{
    LBT20083_RLY_Output   = (0X01 << 0),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
    unsigned short   Reg;
    St_LBT20083_Relay_Bit Bits;
} St_LBT20083_Relay;

typedef struct
{
    unsigned short   AdcGain_I:2;
    unsigned short   AdcGain_V:2;
    unsigned short   AdcGainEnable:1;
    unsigned short   Reserved:11;
} St_LBT20083_PGA_Bit;
typedef union
{
    unsigned short   Reg;
    St_LBT20083_PGA_Bit   Bits;
} St_LBT20083_PGA;

////////////////
enum
{
    LBT20083_SSP0_DAC_NAME = 0xA0,
    LBT20083_SSP0_DAC_CLAMP,
    LBT20083_SSP0_FRAM_NAME,
    LBT20083_SSP1_ADI_NAME,
    LBT20083_SSP1_ADV_NAME
};

enum
{
    LBT20083_ADC_IDLE,
    LBT20083_ADC_OpenADG,
    LBT20083_ADC_OPENING,
    LBT20083_ADC_CONV,
    LBT20083_ADC_TriggerI,
    LBT20083_ADC_TriggerI_V,
    LBT20083_ADC_ReadI,
    LBT20083_ADC_TriggerV,
    LBT20083_ADC_ReadV,
    LBT20083_ADC_CloseADG,
    LBT20083_ADC_NoWork,
};

enum
{
    LBT20083_Pin_AdcConver,
    LBT20083_Pin_AdcBusy1,
    LBT20083_Pin_AdcBusy2,
    LBT20083_Pin_AdcCS1,
    LBT20083_Pin_AdcCS2,
    LBT20083_Pin_AdcRst,
    LBT20083_Pin_DacCS,
    LBT20083_Pin_DacRst,
    LBT20083_Pin_DacLoad,
    LBT20083_Pin_DacBusy,
    LBT20083_Pin_WD,
//	LBT20083_Pin_SCK0,
//	LBT20083_Pin_MISO0,
//	LBT20083_Pin_MOSI0,
    LBT20083_Pin_SCK1,
    LBT20083_Pin_MISO1,
    LBT20083_Pin_MOSI1,
    LBT20083_Pin_PowerOn,
    LBT20083_Pin_AdgRst,
    LBT20083_Pin_Cali_B,
    LBT20083_Total_Pins,
};


class LBT20083_Base
{
private:
    static volatile St_LBT20083_Relay LBT20083_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);

    static void DAC_Init(void);

    //software
    static void Func_Init(void);

// 	static void regCHNInfo(unsigned char CH_Number,unsigned long type);
// 	static void regEngine(unsigned char CH_Number,unsigned long type);

    static void EngineStuff(void);
// 	static  uint32 ReadInfoBus(char chn);
    static void LBT20083_FeatureEnable_Init(void);

public:
    static void ADC_Init(void);
// 	static unsigned char MyIO_LBT20083[6][4];

    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[3];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[2];    //关闭通道
    static St_DisposableTask SetCCToCC[3];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing

// 	static unsigned char SetCtrlOpenCh_Len;
// 	static unsigned char SetCtrlCloseCh_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;

    static SSPI_Para     LBT20083_SSP0DAC;
    static SSPI_Para     LBT20083_SSP1ADC;

    static const St_Port PinGroup[LBT20083_Total_Pins];
    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);
    //    static void AdditionalEngine(void);

    //四个基本操作
    //static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);


    //暂时放到publice里
// 	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
// 	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

    //给LBT20083_SampleMagage.cpp使用
    static  void ADC_ADG_open(unsigned char ChannelIdx);
    static  void ADG_Reset(void);

// 	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);

    static void Feed_CpldWD(void);    //LBT20083喂狗信号  大概6s一次
};

#endif

