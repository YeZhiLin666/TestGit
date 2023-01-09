#pragma once

// #define HPM4_POWER_OFF_PORT    2
// #define HPM4_POWER_OFF_PIN    3

// #define HPM4_IO_NUM 55

#define HPM4SETLOWPULSEDACTIME  75
#define HPM4SETLOWPULSEADCTIME  (100-HPM4SETLOWPULSEDACTIME)
#define HPM4SETHIGHPULSEDACTIME 75
#define HPM4SETHIGHPULSEADCTIME (100-HPM4SETHIGHPULSEDACTIME)
#define HPM4MINIMUMPULSE  10

#define HPM4_RLY_R_P_R         (0x01<< 0)
#define HPM4_RLY_R_IVSD        (0x01<< 1)
#define HPM4_RLY_R_CH          (0x01<< 2)
#define HPM4_RLY_R_CM          (0x01<< 3)
#define HPM4_RLY_R_CL          (0x01<< 4)
#define HPM4_RLY_R_UL          (0x01<< 5)

// modifed by Ron 111215
typedef struct
{
// 	unsigned short   P_R:1;
// 	unsigned short   IVSD:1;
    unsigned short   Reserved1:2;
    unsigned short   R_CH:1;
    unsigned short   R_CM:1;
    unsigned short   R_CL:1;
    unsigned short   R_UL:1;
    unsigned short   IVSD:1;
    unsigned short   P_R:1;
    unsigned short   Reserved:8;
} St_HPM4_Relay_Bit;

typedef struct
{
    unsigned short   Protect:1;
    unsigned short   Output:1;
    unsigned short   IVSD:1;
    unsigned short   Reserved:13;
} St_HPM4_Relay_Bit2;
typedef union
{
    St_HPM4_Relay_Bit2   m_Bits;
    unsigned short       m_Reg;
} Un_HPM4_Relay2;

enum
{
    HPM4_RLY_Protect  = (0x01 << 0),
    HPM4_RLY_Output   = (0X01 << 1),
    HPM4_RLY_IVSD = (0X01 << 2),
};//这个enum需要和上面的BIT结构保持高度一致

typedef union
{
    unsigned short   Reg;
    St_HPM4_Relay_Bit Bits;
} St_HPM4_Relay;

enum
{
    HPM4_SSP0_DAC_NAME = 0xA0,
    HPM4_SSP0_AD5754_NAME,
    HPM4_SSP1_ADI_NAME,
    HPM4_SSP1_ADV_NAME ,
    HPM4_SSP0_FRAM_NAME
};

enum
{
    HPM4_ADC_IDLE,
    HPM4_ADC_AssignIV,
    HPM4_ADC_TriggerIV,
    HPM4_ADC_WAIT,
    HPM4_ADC_ReadI,
    HPM4_ADC_ReadV,
    HPM4_ADC_NoWork,
};


enum
{
    HPM4_Pin_ADCDRDY = 0,
    //HPM4_Pin_ADCBUSY_V,
    HPM4_Pin_ADCSTART,
    HPM4_Pin_CS_C,
    HPM4_Pin_CS_V,
    HPM4_Pin_DacRst,
    HPM4_Pin_DacCS,
    HPM4_Pin_DacLoad,
    HPM4_Pin_DacBusy,

    HPM4_Pin_CTR,
    HPM4_Pin_IVSR,
    HPM4_PWM_FAN,
    HPM4_Pin_WERR2,
    HPM4_Pin_WERR1,
    HPM4_PWM_TEMP,

    HPM4_Pin_DI1,
    HPM4_Pin_DI2,
    HPM4_Pin_DO1,
    HPM4_Pin_DO2,

//	HPM4_Pin_SCK0,
//	HPM4_Pin_MISO0,
//	HPM4_Pin_MOSI0,
    HPM4_Pin_SCK1,
    HPM4_Pin_MISO1,
    HPM4_Pin_MOSI1,

    HPM4_Pin_PWM_DIR,
    HPM4_Pin_TC_ENB,
    HPM4_Pin_PowerOff,
    HPM4_Total_Pins,
};


#define ACTIVE_A0_ 	ACTIVE_CUR_GAIN
#define ACTIVE_A1_	ACITVE_RESERV1
class HPM4_Base
{
private:
    static volatile St_HPM4_Relay HPM4_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
    //一次性时序
    static St_DisposableTask SetRstToCur[4];    //off_to_cur 电流打开通道
    static St_DisposableTask SetCurToOff[2];    //电流关闭通道
    static St_DisposableTask SetCCToCC[5];         //CC to CC
    static St_DisposableTask SetRstToOff[1];       //do nothing
    static St_DisposableTask SetRstToVlt[6];
    static St_DisposableTask SetCVToCV[4];
    static St_DisposableTask SetCCToCV[7];
    static St_DisposableTask SetCVToCC[7];
    static St_DisposableTask SetVltToOff[3];
	static St_DisposableTask SetAutoRangeSwitch[5];
//     static unsigned char SetRstToCur_Len;
//     static unsigned char SetCurToOff_Len;
//     static unsigned char SetCCToCC_Len;
//     static unsigned char SetRstToOff_Len;
//     static unsigned char SetRstToVlt_Len;
//     static unsigned char SetCVToCV_Len;
//     static unsigned char SetCCToCV_Len;
//     static unsigned char SetCVToCC_Len;
//     static unsigned char SetVltToOff_Len;

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);
    static void Engine(void);// 有电压环
    static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

    static  void WriteDOBus(char chn,uint16 BD);
//     static  void WritePGABus(char chn,uint16 PGABD);
//     static  uint32 ReadInfoBus(char chn);

    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
//     static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);
    static void Feed_CpldWD(void);    //HPM4喂狗信号  大概1s一次
    static void HPM4_FeatureEnable_Init(void);
public:
    static SSPI_Para     HPM4_SSP0DAC;
    static SSPI_Para     HPM4_SSP1ADC;
    //static const St_Port Pin_SCK1;
    //static const St_Port Pin_MISO1;
    //static const St_Port Pin_MOSI1;
    //static const St_Port Pin_ADCDRDY;
    // 		static const St_Port Pin_ADCBUSY_V;
    //static const St_Port Pin_ADCSTART;
    //static const St_Port Pin_CS_C;
    //static const St_Port Pin_CS_V;

    //static const St_Port Pin_DacRst;
    //static const St_Port Pin_DacCS;
    //static const St_Port Pin_DacLoad;
    //static const St_Port Pin_DacBusy;

    //static const St_Port Pin_CTR;  //P3.20
    //static const St_Port Pin_IVSR;  //p3.23

    //static const St_Port Pin_WERR2;	  //P0.21
    //static const St_Port Pin_WERR1;     //P0.22

    //static const St_Port PWM_FAN;
    //static const St_Port PWM_TEMP;

    //static const St_Port Pin_DI1;
    //static const St_Port Pin_DI2;

    //static const St_Port Pin_DO1;
    //static const St_Port Pin_DO2;

    //static const St_Port Pin_SCK0 ;
    //static const St_Port Pin_MISO0 ;
    //static const St_Port Pin_MOSI0;

    //static const St_Port Pin_PWM_DIR;
    //static const St_Port Pin_TC_ENB;
    static const St_Port PinGroup[HPM4_Total_Pins];

    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
//     static const unsigned char m_bChannelBasedClampVdac;
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);

    //四个基本操作
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

    static char RTCProcedureForHPM4(unsigned long CH_Number,unsigned long Value);

    //暂时放到publice里    comment out chen 20130106
    // 		static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);

    //给HPM4_SampleMagage.cpp使用
//     static  void ADC_ADG_open(unsigned char ChannelIdx);
//     static  void ADG_Reset(void);

//     static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

    static unsigned char getBID(void);    //获得拨码开关

    // 	  static char FanAdjOperature(unsigned long m_Level,unsigned long FanAdjMod); //added by Ron 7-27-15
    static unsigned char PowerOffCheck(void);
};


