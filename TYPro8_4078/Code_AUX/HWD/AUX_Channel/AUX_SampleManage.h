///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_SampleManage.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						����������ش���ͷ�ļ�
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////
#ifndef _AUX_SAMPLEMANAGE_
#define _AUX_SAMPLEMANAGE_

#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"

//��sf_hwf.hͬ��
//20131209  ʹ��sf_hwf.h  TE_AUX_CHAN_TYPE��TE_AUX_CHAN_T_SENSOR_TYPE
//#define AUX_V_SAMPLE_TYPE  0xA1    
//#define AUX_T_SAMPLE_TYPE 0xB1
//#define AUX_T_SAMPLE_K_TYPE  0xB2    //K��
//#define AUX_T_SAMPLE_T_TYPE 0xB3      //J��
//#define AUX_PRES_SAMPLE_TYPE  0xC3  //ѹ��Ԥ��


#define AUX_LTC_2449_SAMPLE_TIME    10        //50us ��λ     50*10=500us
#define AUX_LTC_2498_SAMPLE_TIME    4000      //50us ��λ     50*4000=200ms
#define AUX_I2C_LM75_SAMPLE_TIME     20       //800us  //50us ��λ     50*20=1ms
//�ڲ�ά��״̬��״̬������μ�����ͼ
typedef enum
{
	AUX_SAMPLE_IDLE_ENUM = 0xC0,
	AUX_SAMPLE_WAITTING_TRIGGER,
	AUX_SAMPLE_WAITTING_SAMPLE,
	AUX_SAMPLE_TRIGGER_NEXT,
	AUX_SAMPLE_WAITTING_TRIGGER_NEXT,

	AUX_SAMPLE_RESERVE1,
	AUX_SAMPLE_RESERVE2,
}AUX_SAMPLE_STATE;

typedef enum
{
	AUX_T_SSP_CHN = 0,
	AUX_V_SSP_CHN,
}AUX_SSP_CHN;

//����ڵ�MCU����ADC״̬���ṹ
typedef struct
{
	unsigned char ADCtype;    //��ǳ�������ڲ�ͨ���������¶ȵ��ڲ�
	unsigned char ADCid;    //if 99 no used
	unsigned char ADCchn;  //�ڲ�ͨ������

	unsigned char sspChn;    //ssp chn
	St_Port cs;                      //cs��

	unsigned char stateMachine;    //�ڲ�״̬��
}st_Aux_ADC_status;

//�����MCU��Aux channel info�ṹ
typedef struct
{
	unsigned long AdcCode;
	unsigned char NewAdcCode;

	float PV;
}st_Aux_Chn_Info;


////////////����Ϊrenew
typedef struct
{
	bool exist;    //�Ƿ���ʵ����  ���ô��ڵ�
	bool inUsed;     //�Ƿ���Ӧ����  �������sch�е�
	bool fastChn;    //�Ƿ����ͨ��
	uint8 counter;    //�����������Ӧ�õĲ���

}st_Aux_SampleSetting;

typedef struct
{
	st_Aux_SampleSetting sampleSetting[17];    //17 �����¶�����ͨ��
	unsigned char sspChn;    //ssp chn
	unsigned char csIndex;                      //cs�ڣ�����cs����

	unsigned char ADCid;    //if 99 no used
	//	unsigned char type;       //Ԥ����ͬʱ���¶�������K��J�����ж�
	unsigned char type[MAX_AUXCH_PER_BOARD];       //ÿ��ͨ����K������J ͬʱ���¶�������K��J�����ж�  03.19.2014

	unsigned char ADC_chn;          //��ǰtrigger��ͨ��
	unsigned char stateMachine;    //�ڲ�״̬��
}st_Aux_Sample;

class AUX_SampleManage
{	
private:
	static unsigned long m_SampleTempData2ndV[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD][3];
	static unsigned long m_SampleRecord[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];
public:
	//static st_Aux_ADC_status st_Aux[MAX_BOARD_PER_AUXMCU];    //1��MCU���֧��8���壬��Ȼ���ʱ�򲢲�֧����ô��
	static st_Aux_Chn_Info st_Aux_info[MAX_BOARD_PER_AUXMCU][17];    //������channle info  //��ʱ����
	static SSPI_Para st_Aux_Para[2];
	static st_Aux_Sample st_aux_sample_setting[MAX_BOARD_PER_AUXMCU];

	// 		////////ssp
	// 		static const St_Port Pin_SCK0;
	// 		static const St_Port Pin_MISO0;
	// 		static const St_Port Pin_MOSI0;

	static const St_Port Pin_SCK1;
	static const St_Port Pin_MISO1;
	static const St_Port Pin_MOSI1;

	//static const St_Port Pin_AD_CS1;
	//static const St_Port Pin_AD_CS2;
	//static const St_Port Pin_AD_CS3;
	//static const St_Port Pin_AD_CS4;
	//static const St_Port Pin_AD_CS5;
	//static const St_Port Pin_AD_CS6;
	//static const St_Port Pin_AD_CS7;
	//static const St_Port Pin_AD_CS8;

	static const St_Port Pin_ECODE1;
	static const St_Port Pin_ECODE2;
	static const St_Port Pin_ECODE3;
	static const St_Port Pin_EOE;
	static const St_Port Pin_ADCBUSY;
	static const St_Port Pin_Cover;
	static const St_Port Pin_CS;

	static void Aux_Sample_init(void);    //init
	//static char Aux_Sample_ISR(unsigned long  Value_A,unsigned long Value_B);    //Timer0 ISR

	//static void AuxT_Sample(void);
	//static void AuxV_Sample(void);

	//�Ƿ�ʹ�ú�ռ�õĽӿ�
	//������Щ�������Ƶ�LTC2449������ȥ
	//chen 20131120
	static unsigned char IssspUsed(unsigned char sspIdx);
	static void sspUsed(unsigned char sspIdx);
	static void clearsspUsed(unsigned char sspIdx);



	//chen 20131128  philip sample
	static  char Sample4ZhongDian(unsigned long  Value_A,unsigned long Value_B);
	static char testSample(unsigned long  Value_A,unsigned long Value_B);
	static signed char findUsedChn(unsigned char Bid);
	static void sampleSettingSet(unsigned char Bid,unsigned char chn,st_Aux_SampleSetting * ss);
	static void sampleSettingSet_CS(unsigned char Bid);
	static unsigned char TemperChannel;
private:

	static unsigned char sspIsUsedFlag[3];    //0,1,2chn
	//static unsigned char Aux_V_valid;
	//static unsigned char Aux_T_valid;

	//��ѹ����LTC2498ϵ�н��в���
	static unsigned long Aux_V_Trigger(unsigned char Bid,unsigned char Bchn);
	//static unsigned long Aux_V_Trigger_Next_and_read(unsigned char Bid,unsigned char Bchn);    //no used
	static unsigned long Aux_V_Read(unsigned char Bid);
	//�¶������ڵ�ѹ
	static unsigned long Aux_T_Trigger(unsigned char Bid,unsigned char Bchn);
	//static unsigned long Aux_T_Trigger_Next_and_read(unsigned char Bid,unsigned char Bchn);    //no used
	static unsigned long Aux_T_Read(unsigned char Bid);

};	
#endif
