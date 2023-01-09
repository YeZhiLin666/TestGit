///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_SampleManage.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样相关处理头文件
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#ifndef _AUX_SAMPLEMANAGE_
#define _AUX_SAMPLEMANAGE_

#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"

//与sf_hwf.h同步
//20131209  使用sf_hwf.h  TE_AUX_CHAN_TYPE和TE_AUX_CHAN_T_SENSOR_TYPE
//#define AUX_V_SAMPLE_TYPE  0xA1    
//#define AUX_T_SAMPLE_TYPE 0xB1
//#define AUX_T_SAMPLE_K_TYPE  0xB2    //K型
//#define AUX_T_SAMPLE_T_TYPE 0xB3      //J型
//#define AUX_PRES_SAMPLE_TYPE  0xC3  //压力预留


#define AUX_LTC_2449_SAMPLE_TIME    10        //50us 单位     50*10=500us
#define AUX_LTC_2498_SAMPLE_TIME    4000      //50us 单位     50*4000=200ms
#define AUX_I2C_LM75_SAMPLE_TIME     20       //800us  //50us 单位     50*20=1ms
//内部维护状态机状态，具体参见流程图
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

//针对于单MCU多板的ADC状态机结构
typedef struct
{
	unsigned char ADCtype;    //标记出特殊的内部通道，比如温度的内部
	unsigned char ADCid;    //if 99 no used
	unsigned char ADCchn;  //内部通道遍历

	unsigned char sspChn;    //ssp chn
	St_Port cs;                      //cs口

	unsigned char stateMachine;    //内部状态机
}st_Aux_ADC_status;

//针对于MCU的Aux channel info结构
typedef struct
{
	unsigned long AdcCode;
	unsigned char NewAdcCode;

	float PV;
}st_Aux_Chn_Info;


////////////以下为renew
typedef struct
{
	bool exist;    //是否真实存在  配置存在的
	bool inUsed;     //是否在应用中  大概是跑sch中的
	bool fastChn;    //是否快速通道
	uint8 counter;    //用来间隔不在应用的采样

}st_Aux_SampleSetting;

typedef struct
{
	st_Aux_SampleSetting sampleSetting[17];    //17 算上温度特殊通道
	unsigned char sspChn;    //ssp chn
	unsigned char csIndex;                      //cs口，用于cs数组

	unsigned char ADCid;    //if 99 no used
	//	unsigned char type;       //预留，同时在温度中用于K，J，等判断
	unsigned char type[MAX_AUXCH_PER_BOARD];       //每个通道是K，还是J 同时在温度中用于K，J，等判断  03.19.2014

	unsigned char ADC_chn;          //当前trigger的通道
	unsigned char stateMachine;    //内部状态机
}st_Aux_Sample;

class AUX_SampleManage
{	
private:
	static unsigned long m_SampleTempData2ndV[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD][3];
	static unsigned long m_SampleRecord[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];
public:
	//static st_Aux_ADC_status st_Aux[MAX_BOARD_PER_AUXMCU];    //1个MCU最大支持8个板，当然大多时候并不支持这么多
	static st_Aux_Chn_Info st_Aux_info[MAX_BOARD_PER_AUXMCU][17];    //类似于channle info  //临时测试
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

	//是否使用和占用的接口
	//将来这些操作，移到LTC2449驱动内去
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

	//电压对于LTC2498系列进行操作
	static unsigned long Aux_V_Trigger(unsigned char Bid,unsigned char Bchn);
	//static unsigned long Aux_V_Trigger_Next_and_read(unsigned char Bid,unsigned char Bchn);    //no used
	static unsigned long Aux_V_Read(unsigned char Bid);
	//温度类似于电压
	static unsigned long Aux_T_Trigger(unsigned char Bid,unsigned char Bchn);
	//static unsigned long Aux_T_Trigger_Next_and_read(unsigned char Bid,unsigned char Bchn);    //no used
	static unsigned long Aux_T_Read(unsigned char Bid);

};	
#endif
