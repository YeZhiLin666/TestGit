///////////////////////////////////////////////////////////////////////////////////
//File Name: Pulse_LogLimit.h
//Version:        V1.0
//Date:                2014/8/21
//Description:
//                     Pulse Log Limit Check
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#define PULSE_LOGLIMITCHECK_COUNT  1   //��ʱֻ��һ��logLimit
// enum
// {
// 	PV_PULSE_CYCLE_INDEX=300,  //��ʱ�������ڵ���
// 	DV_PULSE_CYCLE_INDEX
// };
class Pulse_LogLimit_task
{
public:
    static UInt16 MyStatus;                                                  //����״̬
    static UInt16 TaskID;                                                    //����ID
    static UInt16 Priority;                                                  //���ȼ�
    static PULSE_RAW_DATA  Backup_raw_data[2][2][MAX_PULSE_STAGE_NUM];
    static MsTime LastReportTime[MAXADCHANNELNO]; //����pulse���ʱ���ʵ��OStime��ƥ��

private:
    static unsigned char m_CheckedChannelCount_LL;

public:
    static void DoWork(void);
    static void MyInit(void);
// 	static void EndTask(void);

    static void        Pulse_LL_Check(unsigned char MaxLimitCount);
    static signed char Pulse_FindNextLogLimit(void);
    static void        Pulse_LL_DoLog(unsigned char ChannelIdx);
    static void        SendLog(unsigned char uc_ChannelNo);
    static void 			 Pulse_LL_FindNextLimit(void);
};

