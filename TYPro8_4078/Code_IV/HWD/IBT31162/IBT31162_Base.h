#pragma once

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"

#define IBT31162_POWER_OFF_PORT    1
#define IBT31162_POWER_OFF_PIN    11

#define IBT31162_RLY_R_ACIM     (0x01<< 10)
#define IBT31162_RLY_R_ACAL     (0x01<< 11)
#define IBT31162_RLY_R_L        (0x01<< 12)
#define IBT31162_RLY_R_MODE     (0x01<< 13)
#define IBT31162_RLY_R_CH_EN    (0x01<< 14)
#define IBT31162_RLY_R_OUT      (0x01<< 15)


#define IBT31162_PGA_G1      0 //12   // PGA281的增益为1   (IA0/VA0在最高位，大头序)  
#define IBT31162_PGA_G2      0 //2    // PGA281的增益为2

#define IBT31162_CPLD_FAULT      (0X1F)  // AD8450 故障信号  CPLD地址


//针对IBT31162模块驱动AD8450芯片需要提高一些ISET DAC输出,同时MODE模式为充电状态避免芯片发热。
//#define DAC_OUT_PUT_8450MIN  0x018312    //最小0.001V
#define DAC_8450MIN_LINE  0x024000    //最小0.002V   // 线性模块用
#define DAC_8450MIN_PWM   0x009000    //最小0.0005V  // PWM模块用

typedef struct
{
	unsigned short   Reserved0:10;
	unsigned short   R_ACIM:1;
	unsigned short   R_ACAL:1;
    unsigned short   R_L:1;    
	unsigned short   R_MODE:1;
    unsigned short   R_CH_EN:1; 
    unsigned short   R_OUT:1;
}St_IBT31162_Relay_Bit;

enum
{
	IBT31162_RLY_Mode     = (0x01 << 0),
	IBT31162_RLY_Output   = (0X01 << 1),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
	unsigned short   Reg;
	St_IBT31162_Relay_Bit Bits;
}St_IBT31162_Relay;

typedef struct
{
	unsigned short   Reserved0:8;
    unsigned short   AdcGain_V:4;
	unsigned short   AdcGain_I:4;
}St_IBT31162_PGA_Bit;
typedef union
{
	unsigned short   Reg;
	St_IBT31162_PGA_Bit   Bits;
}St_IBT31162_PGA;

////////////////
// enum
// {
// 	IBT31162_SSP0_DAC_NAME = 0xA0,
//     IBT31162_SSP0_AD5676_NAME0,
//     IBT31162_SSP0_AD5676_NAME1,
//     IBT31162_SSP0_AD5686_NAME,
// 	IBT31162_SSP1_ADI_NAME,
// 	IBT31162_SSP1_ADV_NAME ,
// 	IBT31162_SSP0_FRAM_NAME
// };
enum
{
	IBT31162_ADC_IDLE,
	IBT31162_ADC_OpenADG,
	IBT31162_ADC_OPENING,
	IBT31162_ADC_CONV,
	IBT31162_ADC_TriggerI,
	IBT31162_ADC_ReadI,
	IBT31162_ADC_TriggerV,
	IBT31162_ADC_ReadV,
	IBT31162_ADC_CloseADG,
	IBT31162_ADC_NoWork,
}; 
// enum
// {
//     IBT31162_ADC_IDLE,
// 	IBT31162_ADC_ASSIGNIV,
//     IBT31162_ADC_PGA_GAIN,
// 	IBT31162_ADC_TRIGGERIV,
// 	IBT31162_ADC_WAIT,
// 	IBT31162_ADC_READI,
// 	IBT31162_ADC_READV,
// }; 
enum
{
	IBT31162_HIGH_RANGE_NUM = 0xF1,
	IBT31162_LOW_RANGE_NUM = 0xF2,
};

enum
{
	IBT31162_Pin_ADCDRDYC = 0,
	IBT31162_Pin_ADCDRDYV,
	IBT31162_Pin_ADCSTART,
	IBT31162_Pin_CS_C,
	IBT31162_Pin_CS_V,
	IBT31162_Pin_DacCS3,
    IBT31162_Pin_DacCS5,
	IBT31162_Pin_DacRst,
	IBT31162_Pin_DacLoad,
	IBT31162_Pin_Clamp_Dac_CS,
	IBT31162_Pin_Clamp_Dac_Load,
	IBT31162_Pin_WD,
//  IBT31162_Pin_WO,
//	IBT31162_Pin_SCK0,
//	IBT31162_Pin_MISO0,
//	IBT31162_Pin_MOSI0,
	IBT31162_Pin_SCK1,
	IBT31162_Pin_MISO1,
	IBT31162_Pin_MOSI1,
	IBT31162_Pin_PowerOn,
	IBT31162_Pin_AdgRst,
	IBT31162_Pin_FAN,
    IBT31162_Pin_FANON,
    IBT31162_Pin_FO_INVT,
    IBT31162_Pin_FI_INVT,
    IBT31162_Pin_P_OFF,
    IBT31162_Pin_T_ALARM,
//#if(FAN_SAFETY_MODE==1)
	IBT31162_Pin_FanCheckSignal,
//#endif
	IBT31162_Total_Pins,
};
class IBT31162_Base
{
private:
    
    //#if(FAN_SAFETY_MODE==1)
    static MsTime     FanOverTime;	
    //#endif


    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[3];    //关闭通道
    static St_DisposableTask SetCCToCC[5];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing
	static St_DisposableTask SetCtrlOpenAcim[1];
	static St_DisposableTask SetCtrlCloseAcim[1];

	//    static unsigned char SetCtrlOpenCh_Len;
	//    static unsigned char SetCtrlCloseCh_Len;
	//    static unsigned char SetCCToCC_Len;
	//    static unsigned char SetRstToOff_Len; 

    
    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
    static const unsigned char m_bChannelBasedClampVdac;

    //hardware  把这部分扔到别的文件夹去

    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

//     static void regCHNInfo(unsigned char CH_Number,unsigned long type);
//     static void regEngine(unsigned char CH_Number,unsigned long type);

    static void Engine(void);
    static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

    static  void WritePGABus(char chn,uint16 PGABD);
    static  uint32 ReadInfoBus(char chn);

    static void changeADCRange(unsigned char range);

    static void Feed_CpldWD(void); 
	static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);

//#if(FAN_SAFETY_MODE==1)
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
//#endif

    static void Adjust_ADC_Gain(char type,char chnType);
    static void IBT31162_FeatureEnable_Init(void);  //add by xudong 
public:
	static volatile St_IBT31162_Relay IBT31162_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
    static volatile St_IBT31162_PGA   IBT31162_PGA_IVA; 
    static volatile unsigned char     IBT31162_PGA_DA;
    static unsigned short int  AD8450_FAULT;    //  AD8450 故障信号

    static SSPI_Para     IBT31162_SSP0DAC;       //DAC
    static SSPI_Para     IBT31162_SSP0VCDAC;    //VCLAMP
    static SSPI_Para     IBT31162_SSP1ADC;      //ADC

	static const St_Port PinGroup[IBT31162_Total_Pins];

    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);
    static void IO_Init(void);
	static void AdditionalEngine(void);

    //四个基本操作
    // 		static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);
	static  void Avoid_Hot(void);	
		
};


