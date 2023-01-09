/*
��ʱֻ�Ǽ򵥵�1s�ϴ�һ��
updata 2012 11 15
*/

#include "../Entry/includes.h"


MsTime logLimit::log_period;
unsigned char logLimit::log_chn;

UInt16 logLimit::Priority = PRIORITY4;
UInt16 logLimit::TaskID = LOG_TASK;			// should be a definition which positioning this task in OS_Task[]

void logLimit::Init(void)
{
    log_chn = 0;
}

//EthernetParse::m_ReportLogMetaVariable
void logLimit::DoWork(void)
{
    MsTime time;
    static unsigned char status = 0;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Count_Log ++;
#endif

    if(status == 0)
    {
        OS_ServeProcess::recordCurTime(&log_period);
        status = 1;
    }
    else if(status == 1)
    {
        time = OS_ServeProcess::TimeDiff(log_period);    //�뵱ǰ��RTCϵͳʱ��Ƚ�
        if(time.Second >= 1)    //����1s
        {
            //log һ��

            OS_ServeProcess::recordCurTime(&log_period);

            //for(unsigned char i = 0,i<MAXCHANNELNO,i++)
            sendMSGToEth(log_chn);    //EthernetParse::MPReportData(log_chn,SEND_LOG_DATA);

            //����
            log_chn++;
            //			if(log_chn >= MAXCHANNELNO)
            if(log_chn >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount )
            {
                log_chn = 0;
            }
        }

    }
}

/*
typedef struct
{
WORD   m_wCustomLogVariables_Count;
WORD   m_wCustomerLogValue_Type[MAX_SEND_METAVARIABLE];
} ST_SCHEDULE_LOGSETTING_DATA;
*/
void logLimit::sendMSGToEth(unsigned char chn)   //֪ͨethernet��������
{
    //EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type
    EthernetParse::m_ReportLogMetaVariable.m_wCustomLogVariables_Count = 1;
    EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[0] = MetaCode_PV_Voltage;
    //EthernetParse::MPReportData(chn,SEND_DISPLAY_DATA);

    //���ϴ�һ����������
    //comment out chen 20130124
}



