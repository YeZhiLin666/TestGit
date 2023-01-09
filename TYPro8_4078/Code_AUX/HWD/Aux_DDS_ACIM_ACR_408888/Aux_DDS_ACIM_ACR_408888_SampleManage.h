
#ifndef _Aux_DDS_ACIM_ACR_408888_SampleManage_H_
#define _Aux_DDS_ACIM_ACR_408888_SampleManage_H_

// #define Aux_DDS_ACIM_ACR_408888_ADC_NUM 2
// #define Aux_DDS_ACIM_ACR_408888_ADC_I_CHN_MAX  1
enum
{
    ADC_IDLE,
    ADC_START,
    ADC_Assign,
    ADC_Trigger,
    ADC_Read,
    ADC_WAIT,
    ADC_ReadV,
// 	ADC_NoWork,
};

// #define Aux_DDS_ACIM_ACR_408888_ADC_DG_BUILT_UP_TIME_TICKS    6    //ģ���Ž���ʱ��  600us
#define MAXSAMPLETIMECOUNTER    4500
#define MINSAMPLETIMECOUNTER    15

class Aux_DDS_ACIM_ACR_408888_SampleManage
{
public:
    static char m_ADC_chn;
    static void Init(void);
    static char SampleISR(void);
//      static void   Timer0HandlerISR(void);

    static unsigned long WaitForSampleCounter;

    //end /IR���������ADC����

protected:

private:
    //for �ӿں���
    static unsigned long Sampledata[3][4];
    static unsigned char SampleCnt;
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;
    static const unsigned short DelayTickCount_ADG;

    //�ڲ���ת
    //static char m_I_chn;
    //static char m_V_chn;
// 		static char m_ADC_chn;

    //ʱ��
    static SysTime sample_time;

    //IR
    static char chn_history;
    static char ADC_Status;
    static unsigned short TimeTicks;
// 		static MsTime RecordTime;
// 		static MsTime LastTime;
    static unsigned long SampleDelayCounter;
    static unsigned long ResetADC;
    static unsigned long ResetADC_Counter;

// 		static MsTime TimeT(MsTime a,MsTime b);
};


#endif
