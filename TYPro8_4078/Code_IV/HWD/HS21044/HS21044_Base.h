#pragma once

// #define HS21044_POWER_OFF_PORT    4
// #define HS21044_POWER_OFF_PIN    29

// #define HS21044_IO_NUM 55

#define HS21044SETLOWPULSEDACTIME  75
#define HS21044SETLOWPULSEADCTIME  (100-HS21044SETLOWPULSEDACTIME)
#define HS21044SETHIGHPULSEDACTIME 75
#define HS21044SETHIGHPULSEADCTIME (100-HS21044SETHIGHPULSEDACTIME)
#define HS21044MINIMUMPULSE        10

typedef struct
{
    unsigned short   Protect:1;
    unsigned short   Output:1;
    unsigned short   IVSD:1;
    unsigned short   Reserved1:5;
    unsigned short   Acim:1;
    unsigned short   Reserved2:7;
} St_HS21044_Relay_Bit;
typedef union
{
    St_HS21044_Relay_Bit m_Bits;
    unsigned short       m_Reg;
} Un_HS21044_Relay;
enum
{
    HS21044_RLY_Protect  = (0x01 << 0),
    HS21044_RLY_Output   = (0X01 << 1),
    HS21044_RLY_IVSD = (0X01 << 2),
    HS21044_RLY_ACIM = (0X01 << 8)
};//这个enum需要和上面的BIT结构保持高度一致


enum
{
    HS21044_SSP0_DAC_NAME = 0xA0,
    HS21044_SSP0_AD5754_NAME,
    HS21044_SSP1_ADI_NAME,
    HS21044_SSP1_ADV_NAME ,
    HS21044_SSP0_FRAM_NAME  //AD5754
};

enum
{
    HS21044_ADC_IDLE,
    HS21044_ADC_WAIT,
    HS21044_ADC_CONV,
    HS21044_ADC_ReadIV,
};
enum
{
    HS21044_Pin_AdcConverA,
    HS21044_Pin_AdcConverB,
    HS21044_Pin_AdcBusy,
    HS21044_Pin_AdcRst,
    HS21044_Pin_OS0,
    HS21044_Pin_OS1,
    HS21044_Pin_OS2,
    HS21044_Pin_FDATA,
    HS21044_Pin_DacBIN2COMP,
    HS21044_Pin_DacCS,
    HS21044_Pin_DacLoad,
    HS21044_Pin_DacCLR,
    HS21044_Pin_IVS,
    HS21044_Pin_A0,
    HS21044_Pin_A1,
    HS21044_Pin_Enable,
    HS21044_Pin_Protect,
    HS21044_Pin_ACR ,
    HS21044_Pin_Cali,
    HS21044_Pin_AuxV,
    HS21044_Pin_FAN,
    HS21044_Pin_LEDDriver,
//#if(FAN_SAFETY_MODE==1)
    HS21044_Pin_FanCheckSignal,
    HS21044_Pin_Acim,
    HS21044_Pin_PowerOff,
//#endif
//	HS21044_Pin_SCK0,
//	HS21044_Pin_MISO0,
//	HS21044_Pin_MOSI0,
    HS21044_Total_Pins,
};
#define ACTIVE_A0_ 	ACTIVE_CUR_GAIN
#define ACTIVE_A1_	ACITVE_RESERV1
class HS21044_Base
{
private:
    //一次性时序
    static St_DisposableTask SetRstToCur[5];    //off_to_cur 电流打开通道
    static St_DisposableTask SetCurToOff[4];    //电流关闭通道
    static St_DisposableTask SetCCToCC[7];         //CC to CC
    static St_DisposableTask SetRstToOff[1];       //do nothing
    static St_DisposableTask SetRstToVlt[6];
    static St_DisposableTask SetCVToCV[4];
    static St_DisposableTask SetCCToCV[7];
    static St_DisposableTask SetCVToCC[7];
    static St_DisposableTask SetVltToOff[4];
    static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay
	static St_DisposableTask SetAutoRangeSwitch[7];

// 	static unsigned char SetRstToCur_Len;
// 	static unsigned char SetCurToOff_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;
// 	static unsigned char SetRstToVlt_Len;
// 	static unsigned char SetCVToCV_Len;
// 	static unsigned char SetCCToCV_Len;
// 	static unsigned char SetCVToCC_Len;
// 	static unsigned char SetVltToOff_Len;

    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;
    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);
    static void Engine(void);  // 有电压环
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

// 	static  void WritePGABus(char chn,uint16 PGABD);
// 	static  uint32 ReadInfoBus(char chn);

// 	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

    static void Feed_CpldWD(void);    //HS21044喂狗信号  大概6s一次
    static void HS21044_FeatureEnable_Init(void);   //add by xudong 04/26/2016
public:


    static SSPI_Para     HS21044_SSP0DAC;
    static SSPI_Para     HS21044_SSP1ADC;

    //     static const St_Port Pin_OS0;
    //     static const St_Port Pin_OS1;
    //     static const St_Port Pin_OS2;
    //     static const St_Port Pin_FDATA;
    //     static const St_Port Pin_AdcConverA;
    //     static const St_Port Pin_AdcConverB;
    //     static const St_Port Pin_AdcBusy;
    //     static const St_Port Pin_AdcRst;

    //     static const St_Port Pin_DacBIN2COMP ;
    //     static const St_Port Pin_DacCS ;
    //     static const St_Port Pin_DacLoad ;
    //     static const St_Port Pin_DacCLR ;




// 	static bool          m_bChanOn;

    static const St_Port PinGroup[HS21044_Total_Pins];
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);


    //四个基本操作
    // 		static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);


// 	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
// 	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

    //给HS21044_SampleMagage.cpp使用
// 	static  void ADC_ADG_open(unsigned char ChannelIdx);
// 	static  void ADG_Reset(void);

//#if(FAN_SAFETY_MODE==1)
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
//#endif
    static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);

    static void ReadAllBuildInVolt(void);
    static unsigned char PowerOffCheck(void);
};


