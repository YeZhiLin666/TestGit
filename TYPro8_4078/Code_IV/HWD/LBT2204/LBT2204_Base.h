#pragma once

#define LBT2204_IO_NUM 55


typedef struct
{
	unsigned short   Protect:1;
	unsigned short   OffGate:1;
	unsigned short   Output:1;
	unsigned short   Reserved:13;
}St_LBT2204_Relay_Bit;
typedef union
{
	St_LBT2204_Relay_Bit m_Bits;
	unsigned short       m_Reg;
}Un_LBT2204_Relay;
enum
{
	LBT2204_RLY_Protect  = (0x01 << 0),
	LBT2204_RLY_OffGate  = (0X01 << 1),
	LBT2204_RLY_Output   = (0X01 << 2),
};//这个enum需要和上面的BIT结构保持高度一致


enum
{
	LBT2204_SSP0_DAC_NAME = 0xA0,
	LBT2204_SSP0_AD5754_NAME,
	LBT2204_SSP1_ADI_NAME,
	LBT2204_SSP1_ADV_NAME ,
	LBT2204_SSP0_FRAM_NAME  //AD5754
};

enum
{
	LBT2204_ADC_IDLE,
	LBT2204_ADC_WAIT,
	LBT2204_ADC_CONV,
	LBT2204_ADC_ReadIV,
};
enum
{
	LBT2204_Pin_AdcConverA,
	LBT2204_Pin_AdcConverB,
	LBT2204_Pin_AdcBusy,
	LBT2204_Pin_AdcRst,
	LBT2204_Pin_OS0,
	LBT2204_Pin_OS1,
	LBT2204_Pin_OS2,
	LBT2204_Pin_FDATA,
	LBT2204_Pin_DacBIN2COMP,
	LBT2204_Pin_DacCS,
	LBT2204_Pin_DacLoad,
	LBT2204_Pin_DacCLR,
	LBT2204_Pin_H,
	LBT2204_Pin_M,
	LBT2204_Pin_L,
	LBT2204_Pin_Protect,
	LBT2204_Pin_V,
	LBT2204_Pin_CL_Cal,
	LBT2204_Pin_OFFG_H,
	LBT2204_Pin_OFFG_L,
	LBT2204_Pin_A0,
	LBT2204_Pin_A1,
	LBT2204_Pin_WD,
//	LBT2204_Pin_SCK0,
//	LBT2204_Pin_MISO0,
//	LBT2204_Pin_MOSI0,
	LBT2204_Total_Pins,
};
#define ACTIVE_A0_ 	ACTIVE_CUR_GAIN
#define ACTIVE_A1_	ACITVE_RESERV1
class LBT2204_Base
{
private:
	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);

	//software
	static void Func_Init(void);

// 	static void regCHNInfo(unsigned char CH_Number,unsigned long type);
// 	static void regEngine(unsigned char CH_Number,unsigned long type);

	static void EngineStuff(void);

	static  void WritePGABus(char chn,uint16 PGABD);
	static  uint32 ReadInfoBus(char chn);

public:
	//一次性时序
	static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
	static St_DisposableTask SetCtrlCloseCh[5];    //关闭通道
	static St_DisposableTask SetCCToCC[6];         //CC to CC
	static St_DisposableTask SetRstToOff[1];       //do nothing

	static unsigned char SetCtrlOpenCh_Len;
	static unsigned char SetCtrlCloseCh_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;        

	static SSPI_Para     LBT2204_SSP0DAC;
	static SSPI_Para     LBT2204_SSP1ADC;  

	static const St_Port PinGroup[LBT2204_Total_Pins];
	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;
	//初始化
	static void Hardware_Init(void);
	static void Software_Init(void);
	//    static void AdditionalEngine(void);

	//四个基本操作
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
	static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

	//暂时放到publice里    comment out chen 20130106
	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

	//给LBT2204_SampleMagage.cpp使用
	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);

	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

	static void Feed_CpldWD(void);    //LBT2204喂狗信号  大概6s一次
};


