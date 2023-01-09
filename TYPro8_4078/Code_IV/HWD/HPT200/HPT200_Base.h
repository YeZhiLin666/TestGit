#pragma once

// #define HPT200_POWER_OFF_PORT    2
// #define HPT200_POWER_OFF_PIN    3

// #define HPT200_IO_NUM 55

#define HPT200SETLOWPULSEDACTIME  75
#define HPT200SETLOWPULSEADCTIME  (100-HPT200SETLOWPULSEDACTIME)
#define HPT200SETHIGHPULSEDACTIME 75
#define HPT200SETHIGHPULSEADCTIME (100-HPT200SETHIGHPULSEDACTIME)
#define HPT200MINIMUMPULSE        10

typedef struct
{
    unsigned short   Protect:1;
    unsigned short   Output:1;
    unsigned short   IVSD:1;
    unsigned short   V:1;	// by Philip 3/9/2015
    unsigned short   V_R:1;
    unsigned short   A_V_R:1;
    unsigned short   A_I_R:1;
    unsigned short   Reserved:9;	// by Philip 3/9/2015
} St_HPT200_Relay_Bit;
typedef union
{
    St_HPT200_Relay_Bit m_Bits;
    unsigned short       m_Reg;
} Un_HPT200_Relay;
enum
{
    HPT200_RLY_Protect  = (0x01 << 0),
    HPT200_RLY_Output   = (0X01 << 1),
    HPT200_RLY_IVSD = (0X01 << 2),
    HPT200_RLY_V = (0X01 << 3),	// by Philip 3/9/2015
    HPT200_RLY_V_R = (0x01 << 4),
    HPT200_RLY_A_V_R = (0x01<<5),
    HPT200_RLY_A_I_R = (0x01<<6)
};//这个enum需要和上面的BIT结构保持高度一致


enum
{
    HPT200_SSP0_DAC_NAME = 0xA0,
    HPT200_SSP0_AD5754_NAME,
    HPT200_SSP1_ADI_NAME,
    HPT200_SSP1_ADV_NAME ,
    HPT200_SSP0_FRAM_NAME  //AD5754
};

enum
{
    HPT200_ADC_IDLE,
    HPT200_ADC_WAIT,
    HPT200_ADC_Trigger,
    HPT200_ADC_ReadI,
    HPT200_ADC_ReadV,
};
enum
{
    HPT200_Pin_SCK1,
    HPT200_Pin_MISO1,
    HPT200_Pin_MOSI1,
    HPT200_Pin_ADCBUSY_C,
    HPT200_Pin_ADCBUSY_V,
    HPT200_Pin_Cover,
    HPT200_Pin_CS_C,
    HPT200_Pin_CS_V,
    HPT200_Pin_DacBIN2COMP,
    HPT200_Pin_DacCS,
    HPT200_Pin_DacLoad,
    HPT200_Pin_DacCLR,
    HPT200_Pin_ADDA,    // 04.27.2018 ZC
    HPT200_Pin_ADDB,
    HPT200_Pin_ADDC,
    HPT200_Pin_Protect,
    HPT200_Pin_V,
    HPT200_Pin_IVSD,
    HPT200_Pin_V_R,
    HPT200_Pin_A_V,
    HPT200_PWM_FAN,
    HPT200_Pin_WERR2,
    HPT200_Pin_WERR1,
    HPT200_PWM_TEMP,
    HPT200_Pin_DI1,
    HPT200_Pin_DI2,
    HPT200_Pin_DO1,
    HPT200_Pin_DO2,
//	HPT200_Pin_SCK0,
//	HPT200_Pin_MISO0,
//	HPT200_Pin_MOSI0,
    HPT200_Pin_PWM_DIR,
    HPT200_Pin_TC_ENB,
    HPT200_Pin_PowerOff,
    HPT200_Total_Pins,
};

#define ACTIVE_A0_ 	ACTIVE_CUR_GAIN
#define ACTIVE_A1_	ACITVE_RESERV1
class HPT200_Base
{
private:

    //一次性时序
    static St_DisposableTask SetRstToCur[5];    //off_to_cur 电流打开通道
    static St_DisposableTask SetCurToOff[5];    //电流关闭通道
    static St_DisposableTask SetCCToCC[6];         //CC to CC
    static St_DisposableTask SetRstToOff[1];       //do nothing

    static St_DisposableTask SetCaliBoardRstToVlt[3];
    static St_DisposableTask SetCaliBoardVltToOff[2];
    static St_DisposableTask SetCaliBoardVltToVlt[1];
	static St_DisposableTask SetAutoRangeSwitch[6];

// 	static unsigned char SetRstToCur_Len;
// 	static unsigned char SetCurToOff_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;

// 	static unsigned char SetCaliBoardRstToVlt_Len;
// 	static unsigned char SetCaliBoardVltToOff_Len;
// 	static unsigned char SetCaliBoardVltToVlt_Len;

    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

    static void AdditionalEngine(void);
    static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

// 	static  void WritePGABus(char chn,uint16 PGABD);
// 	static  uint32 ReadInfoBus(char chn);

    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
    static char RTCProcedureForHPT200(unsigned long CH_Number,unsigned long Value);
// 	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
// 	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);
// 	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

    static void Feed_CpldWD(void);    //HPT200喂狗信号  大概6s一次
    static void HPT200_FeatureEnable_Init(void);
public:


    static SSPI_Para     HPT200_SSP0DAC;
    static SSPI_Para     HPT200_SSP1ADC;



    //     static const St_Port Pin_ADCBUSY_C;
    //     static const St_Port Pin_ADCBUSY_V;
    //     static const St_Port Pin_Cover;
    //     static const St_Port Pin_CS_C;
    //     static const St_Port Pin_CS_V;

    //     static const St_Port Pin_DacBIN2COMP ;
    //     static const St_Port Pin_DacCS ;
    //     static const St_Port Pin_DacLoad ;
    //     static const St_Port Pin_DacCLR ;
    //     static const St_Port Pin_TC_ENB;
    //     static const St_Port Pin_PWM_DIR;

    //初始化
    static const St_Port PinGroup[HPT200_Total_Pins];
    static void Hardware_Init(void);
    static void Software_Init(void);

    //四个基本操作
    // 		static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

    //暂时放到publice里    comment out chen 20130106
    //给HPT200_SampleMagage.cpp使用
// 	static  void ADC_ADG_open(unsigned char ChannelIdx);
// 	static  void ADG_Reset(void);

    static char FanAdjOperate(unsigned long m_Level,unsigned long FanAdjMod); //added by Ron 7-27-15
    static unsigned char PowerOffCheck(void);
};


