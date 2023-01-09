
#include "../Entry/includes.h"
#if (SCH_STATUS_REPORT_ENABLE  == 1)

//public:
// unsigned short               ScheduleStatusReport::Priority = PRIORITY8;
// unsigned short               ScheduleStatusReport::TaskID = SDL_TASK;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char                ScheduleStatusReport::uc_All_ReportFlag;       //  ȫ��ͨ��״̬��¼����
unsigned char                ScheduleStatusReport::uc_Ch_ReportFlag[MAXCHANNELNO];       //  ͨ��״̬��¼����
unsigned char                ScheduleStatusReport::uc_uc_BreakNet_Counter;               //  ����������
UWord32                      ScheduleStatusReport::uw32_ChInterval[MAXCHANNELNO];        //  �趨ͨ��״̬��¼���ʱ��
MsTime                       ScheduleStatusReport::m_ChInterval_Report[MAXCHANNELNO];    //  ͨ��״̬��¼���ʱ��
float                        ScheduleStatusReport::m_StepCtrlValue[MAXCHANNELNO];        //  ��ǰ������Ŀ���ֵ

#pragma arm section //�������������� 

void ScheduleStatusReport::Init(void)
{
    //for(unsigned char uc_ch = 0; uc_ch < MAXCHANNELNO; uc_ch ++)
    for(unsigned char uc_ch = 0; uc_ch < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_ch ++)
    {
        uc_Ch_ReportFlag[uc_ch] = 0;
        uw32_ChInterval[uc_ch] = 0;
        CommonFunc::GetTimeTicksUpdate(&m_ChInterval_Report[uc_ch]);
    }
    uc_All_ReportFlag = 0;
    uc_uc_BreakNet_Counter = 0;
}

//void ScheduleStatusReport::DoWork(void)
//inline void ScheduleStatusReport::SCHStatusReportDoWork(void)
void ScheduleStatusReport::SCHStatusReport_DoWork(void)
{
    //for(unsigned char uc_ch = 0; uc_ch < MAXCHANNELNO; uc_ch ++)
    if(uc_All_ReportFlag)
    {
        if(CommonFunc::GetTimeTicks(&m_ChInterval_Report[0]) >= uw32_ChInterval[0])
        {
            EthernetParse::MPReportData(0XFFFF, SEND_GET_SCHSTATUS);
            CommonFunc::GetTimeTicksUpdate(&m_ChInterval_Report[0]);
        }
        return;
    }
    for(unsigned char uc_ch = 0; uc_ch < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_ch ++)
    {
        if(uc_Ch_ReportFlag[uc_ch])
        {
            if(CommonFunc::GetTimeTicks(&m_ChInterval_Report[uc_ch]) >= uw32_ChInterval[uc_ch])
            {
                EthernetParse::MPReportData(uc_ch, SEND_GET_SCHSTATUS);
                CommonFunc::GetTimeTicksUpdate(&m_ChInterval_Report[uc_ch]);
            }
        }
    }
}

void ScheduleStatusReport::Set_ChIntervalTime(unsigned short ChID ,unsigned short Set_Time)
{
    if(Set_Time == 0)
    {
        uc_All_ReportFlag = 0;
        if( ChID == 0XFFFF)
        {
            //for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO ; uc_i ++)
            for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_i ++)
            {
                uc_Ch_ReportFlag[uc_i] = 0;
            }
            return;
        }
        else
        {
            if(ChID < MAXCHANNELNO)
                uc_Ch_ReportFlag[ChID] = 0;
        }
    }
    else
    {
        if(ChID == 0XFFFF)
        {
            uc_All_ReportFlag = 1;
            //for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO ; uc_i ++)
            for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_i ++)
            {
                uc_Ch_ReportFlag[uc_i] = 1;
                uw32_ChInterval[uc_i] = Set_Time*1000*10;  // ��ת������ת100΢��     yy  20141030  Fix event buffer overflow bug.
                CommonFunc::GetTimeTicksUpdate(&m_ChInterval_Report[uc_i]);
            }
        }
        else if(ChID < MAXCHANNELNO)
        {
            uc_Ch_ReportFlag[ChID] = 1;
            uw32_ChInterval[ChID] = Set_Time*1000*10;  // ��ת������ת100΢��
            CommonFunc::GetTimeTicksUpdate(&m_ChInterval_Report[ChID]);
        }
    }

}

#endif

