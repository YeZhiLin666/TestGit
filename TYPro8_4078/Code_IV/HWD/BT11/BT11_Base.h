#pragma once

#define BT11_IO_NUM 55


typedef struct
{
	unsigned short   Protect:1;
	unsigned short   Output:1;
	unsigned short   IVSD:1;
	unsigned short   Reserved:13;
}St_BT11_Relay_Bit;
typedef union
{
	St_BT11_Relay_Bit m_Bits;
	unsigned short       m_Reg;
}Un_BT11_Relay;
enum
{
	BT11_RLY_Protect  = (0x01 << 0),
	BT11_RLY_Output   = (0X01 << 1),
	BT11_RLY_IVSD = (0X01 << 2)
};//���enum��Ҫ�������BIT�ṹ���ָ߶�һ��


enum
{
	BT11_SSP0_DAC_NAME = 0xA0,
	BT11_SSP0_AD5754_NAME,
	BT11_SSP1_ADI_NAME,
	BT11_SSP1_ADV_NAME ,
	BT11_SSP0_FRAM_NAME  //AD5754
};

enum
{
	BT11_ADC_IDLE,
	BT11_ADC_WAIT,
	BT11_ADC_CONV,
	BT11_ADC_ReadIV,
};
enum
{
	BT11_Pin_AdcConverA,
	BT11_Pin_AdcConverB,
	BT11_Pin_AdcBusy,
	BT11_Pin_AdcRst,
	BT11_Pin_OS0,
	BT11_Pin_OS1,
	BT11_Pin_OS2,
	BT11_Pin_FDATA,
	BT11_Pin_DacBIN2COMP,
	BT11_Pin_DacCS,
	BT11_Pin_DacLoad,
	BT11_Pin_DacCLR,
	BT11_Pin_H,
	BT11_Pin_M,
	BT11_Pin_L,
	BT11_Pin_Protect,
	BT11_Pin_V,
	BT11_Pin_IVSD,
	BT11_Pin_WERR2,
	BT11_Pin_WERR1,
	BT11_PWM_FAN,
	BT11_PWM_TEMP,
	BT11_Pin_SCK0,
	BT11_Pin_MISO0,
	BT11_Pin_MOSI0,
	BT11_Total_Pins,
};

#define ACTIVE_A0_ 	ACTIVE_CUR_GAIN
#define ACTIVE_A1_	ACITVE_RESERV1

class BT11_Base
{
private:
	//hardware  ���ⲿ���ӵ�����ļ���ȥ
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
    static void BT11_FeatureEnable_Init(void);
public:
	//һ����ʱ��
	static St_DisposableTask SetRstToCur[5];    //off_to_cur ������ͨ��
	static St_DisposableTask SetCurToOff[5];    //�����ر�ͨ��
	static St_DisposableTask SetCCToCC[6];         //CC to CC
	static St_DisposableTask SetRstToOff[1];       //do nothing

	static unsigned char SetRstToCur_Len;
	static unsigned char SetCurToOff_Len;
	static unsigned char SetCCToCC_Len;
	static unsigned char SetRstToOff_Len;   

	static SSPI_Para     BT11_SSP0DAC;
	static SSPI_Para     BT11_SSP1ADC;

	static const St_Port PWM_FAN;
	static const St_Port PWM_TEMP;

	static const St_Port PinGroup[BT11_Total_Pins]; 
	static const unsigned char m_bHaveVoltageDac;
	static const unsigned char m_bHaveClampVdac;
	static const unsigned char m_bChannelBasedClampVdac;
	//��ʼ��
	static void Hardware_Init(void);
	static void Software_Init(void);
	//    static void AdditionalEngine(void);

	//�ĸ���������
	//static char ADC_rd(unsigned long ChannelIdx,unsigned long type);
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
	static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
	static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);

	//��ʱ�ŵ�publice��    comment out chen 20130106
	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

	//��BT11_SampleMagage.cppʹ��
	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);

	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

	static void Feed_CpldWD(void);    //BT11ι���ź�  ���6sһ��
};


