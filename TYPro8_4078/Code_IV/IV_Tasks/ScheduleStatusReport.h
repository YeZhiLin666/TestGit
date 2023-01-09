

#pragma once


#define BREAKNET_NUMBER  (10)  // SCH ״̬���ı��� ����������������

class ScheduleStatusReport
{
public:
    // 		static unsigned short               Priority;
    // 	    static unsigned short               TaskID;
    static unsigned char                uc_All_ReportFlag;       //  ȫ��ͨ��״̬��¼����
    static unsigned char                uc_uc_BreakNet_Counter;               //  ����������
    static float                        m_StepCtrlValue[MAXCHANNELNO];        //  ��ǰ������Ŀ���ֵ

private:
    static unsigned char                uc_Ch_ReportFlag[MAXCHANNELNO];       //  ͨ��״̬��¼����
    static UWord32                      uw32_ChInterval[MAXCHANNELNO];        //  �趨ͨ��״̬��¼���ʱ��
    static MsTime                       m_ChInterval_Report[MAXCHANNELNO];    //  ͨ��״̬��¼���ʱ��

public:
    static void Init(void);
    //inline static void DoWork(void);
    //inline static void SCHStatusReportDoWork(void);
    static void SCHStatusReport_DoWork(void);
    static void Set_ChIntervalTime(unsigned short ChID ,unsigned short Set_Time);

private:

};

