

/*
����һ����������

adc SampleProcess_period �������ڲ������ο�.net��ԭ����ƣ�����1ms 1�β�����ÿ�β���1��adcͨ��

�������80%
*/

//BurstModeSampling
#ifndef _BURSTMODESAMPLING_H_
#define _BURSTMODESAMPLING_H_

#include "../../COMMON_OS/Protocols/SF_Hwf.h"


#define BURSTMODESAMPLING_MAX_CHANNEL   1       // ���X��ͨ��      // MAXCHANNELNO
#define BURSTMODESAMPLING_MAX_BUFFER    1000    // ���X��������
#define BURSTMODESAMPLING_MAX_DELAY     1000    // �����ʱ1s
#define BURSTMODESAMPLING_MIN_SENDDATA  54 //10      // ��С����������


typedef struct
{
    unsigned long uw32_Voltage;
    unsigned long uw32_Current;

} st_SimpleData;

typedef CIRCULAR_QUEUE(st_SimpleData,BURSTMODESAMPLING_MAX_BUFFER)   St_SIMPLELIST_QUEUE;

class BurstModeSampling
{
public:
    //static UInt16 Priority;
    //static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]

    static bool  uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];    //  ����BurstModeģʽtrue
    static bool  bl_NeedRunning[BURSTMODESAMPLING_MAX_CHANNEL];    // ����������Ҫ����
    static bool  bl_StepResume[BURSTMODESAMPLING_MAX_CHANNEL];     // ���ָ�����
    static bool  b_Logging[BURSTMODESAMPLING_MAX_CHANNEL];
    static bool  b_StepTimeReset[BURSTMODESAMPLING_MAX_CHANNEL];
    static unsigned short int uw16_BurstModeDelayTime[BURSTMODESAMPLING_MAX_CHANNEL];      //  PC����BurstMode�ӳ�ʱ�� 1:100us ��λ��100us [��������0-10000֮��]
    static unsigned long int  uw32_BurstModePointNum[BURSTMODESAMPLING_MAX_CHANNEL];       //  PC����BurstMode�������  [��������100-1000֮��]
    static unsigned long int  uw32_BurstModeSampleSpeed[BURSTMODESAMPLING_MAX_CHANNEL];    //  PC����BurstMode����ٶ� 1:100us��¼һ�� n:n*100us��¼һ�� [��������1-10֮��]
    static bool  bl_ChangedTime[BURSTMODESAMPLING_MAX_CHANNEL];    // ǿ�任ʱ���־
    static bool  bl_Time;           // ʱ�䱸�ݱ�־
    static MsTime OSTime_back;      // ϵͳʱ�䱸�ݱ�־
    static MsTime StartTime[BURSTMODESAMPLING_MAX_CHANNEL];                    //   BurstMode��ʼʱ��
//     static float fStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
//     static float fStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
    static double dStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
    static double dStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
// 	static unsigned int u32_burst_tick;
// 	static bool b_burst_start;
// 	static bool init_Start_Time;

    static void Init(void);
// 	static void DoWork(void);
// 	static void EndTask(void);

    static void ReportSimpleData(void);
    static unsigned char BurstReport_LogSDL(unsigned short ChannelIdx, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport); //mits8
    static void BurstSavePoint(unsigned short ChannelIdx);
    static void OpenBurstModeSampling(unsigned short ChannelIdx);
    static void CloseBurstModeSampling(unsigned short ChannelIdx);
    static void BurstModeSamplingCheck(unsigned short ChannelIdx,unsigned short u16_StepID);
//     static void BurstModeSamplingResume(unsigned short ChannelIdx, double dSetValue);

//protected:

private:

    //static SimpleData  st_BurstSampleList[BURSTMODESAMPLING_MAX_BUFFER];
    static St_SIMPLELIST_QUEUE  st_BurstSampleList[BURSTMODESAMPLING_MAX_CHANNEL];    // MAXCHANNELNO
    static unsigned long int uw32_P_A_Log[BURSTMODESAMPLING_MAX_CHANNEL];      //  ��¼��һ������λ��
    static unsigned long int uw32_P_B_Send[BURSTMODESAMPLING_MAX_CHANNEL];     //  �ϱ���һ������λ��
    //static bool uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];                 //  ����BurstModeģʽtrue
    static bool uc_B_DataFinish[BURSTMODESAMPLING_MAX_CHANNEL];                //  �ϱ��������true
    //static MsTime StartTime[BURSTMODESAMPLING_MAX_CHANNEL];                    //   BurstMode��ʼʱ��

    //static unsigned long int  uw32_SendTotal[BURSTMODESAMPLING_MAX_CHANNEL];   //  ���ϱ���������
    static unsigned char uc_Voltage_range[BURSTMODESAMPLING_MAX_CHANNEL];   // ��¼���ݵĵ�ѹ����
    static unsigned char uc_Current_range[BURSTMODESAMPLING_MAX_CHANNEL];   // ��¼���ݵĵ�������
    static unsigned short int  uw16_IntervalCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // ������ʱ��
    static unsigned short int  uw16_SendTimeCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // ���ͼ��ʱ��

    static void BurstPointChange(unsigned short ChannelIdx,st_SimpleData * S, St_Data_Pairs_4BytesCycleIdx * D); //mits8
    static void BurstParameterCheck(unsigned short ChannelIdx);

};

#endif
