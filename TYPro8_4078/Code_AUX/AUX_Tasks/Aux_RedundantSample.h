#pragma once
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
//#include "../Entry/includes.h"

#define _Gain  -1.8407f    //-2.349f
#define _Offset 5.3269f //5.1521f   //DKQ 
#define  MAX_SAMPLE_CHN 8  //3 DKQ  03.11.2015

enum Redundant_Type
{
    en_VolType = 0,
    en_TempType = 1,
};
typedef struct
{
    unsigned char m_Type;
    float m_Gain;
    float m_Offset;
} St_ADCGainOffset;

enum  en_SampleState
{
    eOpenADG = 0,
    eWaitStable,
    eSample,
    eCloseADG,
};
typedef struct
{
    unsigned char  	m_bValid;
    unsigned char  	m_bRefreshed[M4_MAX_ADCCHANNELNO];
    float  					m_fValue[M4_MAX_ADCCHANNELNO];
    unsigned int		m_DataCode[M4_MAX_ADCCHANNELNO];
} St_RedundantData;

typedef struct
{
    unsigned char uc_start;
    unsigned char uc_length;
    float ul_data[MAX_ADC_CODE_QUE];
} st_M4ADC_Code_Que;


class RedundantSample_Task
{
public:
    static const St_Port ADC_ADD1;
    static const St_Port ADC_ADD2;
    static const St_Port ADC_ADD3;
    static MsTime LastSampleTime;
    static UInt16 Priority;
    static UInt16 TaskID;
    static unsigned char 		m_MaxChnCount;
    static st_M4ADC_Code_Que m_M4_adc_history[MAX_SAMPLE_CHN]; //M4 ADC ÀúÊ·ÂëÖµ

    //function
    static void Init(void);
    static void DoWork(void);
    static void EndTask(void);
private:
    static MsTime m_SampleTick;

public:
    static St_ADCGainOffset st_ADCPara[REDUNDANT_TYPE];
    static St_RedundantData st_SampleData[REDUNDANT_TYPE];
    static void HardWare_init(void);

    static bool SetGainOffset(const St_Uart_SetGainOffset* AdcInfo);
    static bool SetChannelMap(const St_Uart_SetChannelMap * ChannelMapInfo);
    static void SampleOneChn(unsigned char uc_Type);
    static void ScanIVchn(unsigned char uc_ChannelNo);

    static float ADCRead(unsigned char uc_ADCchannel);
    static bool SetGainOffset(St_Uart_SetGainOffset* AdcInfo);
    static bool SetChannelMap(St_Uart_SetChannelMap *ChannelMapInfo);
    static float avgQueue(unsigned char chn, unsigned char filter_depth);
    static void  enQueue( unsigned char chn, float code);


};
