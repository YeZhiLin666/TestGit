#pragma once

// #define LBT22XXX_POWER_OFF_PORT    4
// #define LBT22XXX_POWER_OFF_PIN    29

// #define LBT22XXX_IO_NUM 55
#define LBT22XXXSETLOWPULSEDACTIME  50
#define LBT22XXXSETLOWPULSEADCTIME  (100-LBT22XXXSETLOWPULSEDACTIME)
#define LBT22XXXSETHIGHPULSEDACTIME 50
#define LBT22XXXSETHIGHPULSEADCTIME (100-LBT22XXXSETHIGHPULSEDACTIME)
#define LBT22XXXMINIMUMPULSE     50


typedef struct
{
    unsigned short   Protect:1;
    unsigned short   OffGate:1;
    unsigned short   Output:1;
    unsigned short   V_D:1;
    unsigned short   OffGateM:1;
	  unsigned short   PreC:1;
    unsigned short   Reserved:2;
    unsigned short   Acim:1;
    unsigned short   Damping:1;
	  unsigned short   Ivs:1;
    unsigned short   Prot:1;
    unsigned short   Reserved1:4;
} St_LBT22XXX_Relay_Bit;
typedef union
{
    St_LBT22XXX_Relay_Bit m_Bits;
    unsigned short       m_Reg;
} Un_LBT22XXX_Relay;
enum
{
    LBT22XXX_RLY_Protect  = (0x01 << 0),
    LBT22XXX_RLY_OffGate  = (0X01 << 1),
    LBT22XXX_RLY_Output   = (0X01 << 2),
    LBT22XXX_RLY_VoltSet  = (0X01 << 3),
    LBT22XXX_RLY_OffGateM = (0X01 << 4),
	  LBT22XXX_RLY_PreC     = (0X01 << 5),
    LBT22XXX_RLY_ACIM     = (0X01 << 8),
    LBT22XXX_RLY_DAMPING  = (0X01 << 9),
		LBT22XXX_RLY_IVS      = (0X01 << 10),
	  LBT22XXX_RLY_PROT     = (0X01 << 11)
};//这个enum需要和上面的BIT结构保持高度一致


enum
{
    LBT22XXX_SSP0_DAC_NAME = 0xA0,
    LBT22XXX_SSP0_AD5754_NAME,
    LBT22XXX_SSP1_ADI_NAME,
    LBT22XXX_SSP1_ADV_NAME ,
    LBT22XXX_SSP0_FRAM_NAME  //AD5754
};

enum
{
    LBT22XXX_ADC_IDLE,
    LBT22XXX_ADC_WAIT,
    LBT22XXX_ADC_CONV,
    LBT22XXX_ADC_ReadIV,
};
enum
{
    LBT22XXX_Pin_AdcConverA,
    LBT22XXX_Pin_AdcConverB	,
    LBT22XXX_Pin_AdcBusy,
    LBT22XXX_Pin_AdcRst,
    LBT22XXX_Pin_OS0,
    LBT22XXX_Pin_OS1,
    LBT22XXX_Pin_OS2,
    LBT22XXX_Pin_FDATA,
    LBT22XXX_Pin_DacBIN2COMP,
    LBT22XXX_Pin_DacCS,
    LBT22XXX_Pin_DacLoad,
    LBT22XXX_Pin_DacCLR,
    LBT22XXX_Pin_HD,
    LBT22XXX_Pin_MD,
    LBT22XXX_Pin_LD,
    LBT22XXX_Pin_LLD,
    LBT22XXX_Pin_Protect,
    LBT22XXX_Pin_VD,
    LBT22XXX_Pin_IVSD,
    LBT22XXX_Pin_CL_A0,
    LBT22XXX_Pin_CL_A1,
    LBT22XXX_Pin_OFFG_HL,
    LBT22XXX_Pin_OFFG_M,
    LBT22XXX_Pin_OFFG_HM,
    LBT22XXX_Pin_A0,
    LBT22XXX_Pin_A1,
    LBT22XXX_Pin_LEDDriver,
    LBT22XXX_Pin_WD ,
    LBT22XXX_Pin_FAN,
//	LBT22044_Pin_SCK0,
//	LBT22044_Pin_MISO0,
//	LBT22044_Pin_MOSI0,
//新增
    LBT22XXX_Pin_VCEN,
    LBT22XXX_Pin_DI1,
    LBT22XXX_Pin_DO1,
//#if(FAN_SAFETY_MODE==1)
    LBT22XXX_Pin_FanCheckSignal,
//#endif
    LBT22XXX_Pin_Acim,
    LBT22XXX_Pin_Damping,
    LBT22XXX_Pin_PowerOff,
    LBT22XXX_Pin_Prec,
    LBT22XXX_Pin_Temperature,
    LBT22XXX_Pin_IVS,
    LBT22XXX_Pin_PROT,
    LBT22XXX_Total_Pins,
};

typedef struct
{
    unsigned short OffGateM_Enable:1;

} St_LBT22XXX_Feature_BoolBit;
typedef struct
{
    St_LBT22XXX_Feature_BoolBit BoolFlag;
} St_LBT22XXX_Feature;

class LBT22XXX_Base
{
public:
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[5];    //关闭通道
    static St_DisposableTask SetCtrlCloseChToRest[5];
    static St_DisposableTask SetCCToCC[8];         //CC to CC
    static St_DisposableTask SetRstToOff[1];       //do nothing
    static St_DisposableTask SetOffToRst[1];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay
#if (IV_AUTORANGE == 1)
	static St_DisposableTask SetAutoRangeSwitch[8];
#endif
// 	static unsigned char SetCtrlOpenCh_Len;
// 	static unsigned char SetCtrlCloseCh_Len;
//  	static unsigned char SetCtrlCloseChToRest_Len;
// 	static unsigned char SetCCToCC_Len;
// 	static unsigned char SetRstToOff_Len;

//#if(FAN_SAFETY_MODE==1)
    static const St_Port Pin_FanCheckSignal;
//#endif

    static unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

    //static void Engine(void);
// 	static void AdditionalEngine(void);
// 	static void EngineStuff(void);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);
		static unsigned char Set_Volt_Range(unsigned char uc_ChannelNo,unsigned char VoltRange);

// 	static  void WritePGABus(char chn,uint16 PGABD);
// 	static  uint32 ReadInfoBus(char chn);

    static void Feed_CpldWD(void);    //LBT22044喂狗信号  大概6s一次
    static char RedundantSafety(unsigned long uc_ChannelNo,unsigned long type);
    static char TemperatureSafety(unsigned long CH_Number,unsigned long type);
    static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);
    static char ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type);
    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
    static char Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static void UnitDI_ReadAll(void);
    static char UnitDO_Reset(unsigned long CH_Number,unsigned long Value);
    static void LBT22XXX_FeatureEnable_Init(void);
public:

    static unsigned char VoltageRangeFlag[MAXCHANNELNO];
    static SSPI_Para     LBT22XXX_SSP0DAC;
    static SSPI_Para     LBT22XXX_SSP1ADC;

    static const St_Port PinGroup[LBT22XXX_Total_Pins];
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);


    //四个基本操作
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

    //暂时放到publice里    comment out chen 20130106
// 	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
// 	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

    //给LBT22044_SampleMagage.cpp使用
// 	static  void ADC_ADG_open(unsigned char ChannelIdx);
// 	static  void ADG_Reset(void);

// 	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

//#if(FAN_SAFETY_MODE==1)
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
//#endif
    static unsigned char PowerOffCheck(void);

    static St_LBT22XXX_Feature BoardFeature;
};


