#ifndef _BT2208_Base_
#define _BT2208_Base_

#include "../../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../IV_Tasks/BSP.h"
#include "../../common_OS/Data_Manipulation/Datatypes.h"
//#include "../../COMMON/includes.h"


#define BT2208_IO_NUM 55


// //������ο��ĵ� ��ַ�ռ�滮
#define DCS3 0x0001


//2012 11 26    ����ʹ�õ�newArbinBus 1126�汾  ʹ�ñ��밴������ʱ�����
//��ͨ��DO�������ʱ��    ADC����ѡ��ʱ��    ��16λͨ����Ϣ����ʱ��

//���ӳ�
#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );
//Arbin Bus Write
#define cpu_iowr_8(ADDR,DATA)   *((volatile uint8 *)(ADDR)) = (DATA)
#define cpu_iowr_16(ADDR,DATA)  *((volatile uint16 *)(ADDR)) = (DATA)
#define cpu_iowr_32(ADDR,DATA)  *((volatile uint32 *)(ADDR)) = (DATA)
//Arbin bus read
#define cpu_iord_8(ADDR)        *((volatile uint8 *)(ADDR))
#define cpu_iord_16(ADDR)       *((volatile uint16 *)(ADDR))
#define cpu_iord_32(ADDR)       *((volatile uint32 *)(ADDR))


//DO data    ���ս�����ǰ�data��������
//ADC


//�ⲿ�ֿ��Բο�arbin bus

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
#define     BT2208_1_CHN_NUM     14  //15chnΪ����1��ͨ������

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
	static volatile St_BT2208_Relay BT2208_relaystatus[MAXCHANNELNO];    //��¼�����relay״̬
	static unsigned char whichRange;    //�ĸ�����

	//hardware  ���ⲿ���ӵ�����ļ���ȥ
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

	//һ����ʱ��
	static St_DisposableTask SetCtrlOpenCh[5];    //��ͨ��
	static St_DisposableTask SetCtrlCloseCh[4];    //�ر�ͨ��
	static St_DisposableTask SetCCToCC[10];         //CC to CC
	static St_DisposableTask SetRstToOff[2];       //do nothing

	//    static St_DisposableTask SetOpenISO[2];    //��ѹ��ISO    //comment out chen 20130123
	//    static St_DisposableTask SetCloseISO[2];    //��ѹ�ر�ISO    //comment out chen 20130123
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
	//��ʼ��
	static void Hardware_Init(void);
	static void Software_Init(void);
	//    static void AdditionalEngine(void);

	//�ĸ���������
	static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);
	static char Relay_on(unsigned long ChannelIdx,unsigned long type);
	static char Relay_off(unsigned long ChannelIdx,unsigned long type);


	//��ʱ�ŵ�publice��   
	static  void Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain);
	static  void Adjust_ADC_Gain(unsigned char type,unsigned char Gain);

	//��BT2208_SampleMagage.cppʹ��
	static  void ADC_ADG_open(unsigned char ChannelIdx);
	static  void ADG_Reset(void);

	static void changeADCRange(unsigned char ChannelIdx,unsigned char range);

	static void Feed_CpldWD(void);    //BT2208ι���ź�  ���6sһ��
};

#endif

