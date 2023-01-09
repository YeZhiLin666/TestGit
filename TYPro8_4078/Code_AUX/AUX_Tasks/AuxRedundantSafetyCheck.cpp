#include "../Entry/includes.h"
#define MAX_UNSAFE_COUNT  0   //100
UInt16 AuxRedundantSafetyCheck::Priority = PRIORITY4;
UInt16 AuxRedundantSafetyCheck::TaskID = AUXREDUNDANTSAFETYCHECK_TASK;

St_M4SafetyCeck AuxRedundantSafetyCheck::st_RedundantSafetyCheck[M4_MAX_CHANNELNO];

unsigned char bCheckSafetyInfo = 1;



void AuxRedundantSafetyCheck::Init(void)
{
    unsigned char chnIdx = 0;
    RedundantSample_Task::m_MaxChnCount = 8; //3;

    for(; chnIdx<M4_MAX_CHANNELNO; chnIdx++)
    {
        st_RedundantSafetyCheck[chnIdx].m_safeStatus = en_Safe;
        st_RedundantSafetyCheck[chnIdx].m_SafetyValid = false;
    }
}
void AuxRedundantSafetyCheck::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_REDUNDANTSAFETY] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_REDUNDANTSAFETY);
#endif
    unsigned char uc_IVChnIdx = 0,uc_Type=0;
    signed char M4ADC_ChnNo;


    for(; uc_IVChnIdx<M4_MAX_CHANNELNO; uc_IVChnIdx++)

    {
        M4ADC_ChnNo = get_M4ChnNo(uc_IVChnIdx);
        if(M4ADC_ChnNo<0)
            continue;
        if((RedundantSample_Task::st_SampleData[uc_Type].m_bRefreshed[uc_IVChnIdx]==0) || (st_RedundantSafetyCheck[uc_IVChnIdx].m_SafetyValid==0))
            continue;
        RedundantSample_Task::st_SampleData[uc_Type].m_bRefreshed[M4ADC_ChnNo] = 0;	// Add DKQ 03.12.2015 to clear the refrashed flag
        if(RedundantSample_Task::st_SampleData[uc_Type].m_fValue[M4ADC_ChnNo]<st_RedundantSafetyCheck[uc_IVChnIdx].m_SafetyL
                || RedundantSample_Task::st_SampleData[uc_Type].m_fValue[M4ADC_ChnNo]>st_RedundantSafetyCheck[uc_IVChnIdx].m_SafetyH)
        {
            st_RedundantSafetyCheck[uc_IVChnIdx].m_UnsafeCount++;


            if(st_RedundantSafetyCheck[uc_IVChnIdx].m_UnsafeCount>MAX_UNSAFE_COUNT)
            {
                st_RedundantSafetyCheck[uc_IVChnIdx].m_UnsafeCount = 0;
                if(st_RedundantSafetyCheck[uc_IVChnIdx].m_safeStatus!=en_Unsafe)
                {
                    CommonFunc::GetTimeTicksUpdate(&st_RedundantSafetyCheck[uc_IVChnIdx].m_waitReleaseTick);
                    ReportUnsafe(uc_IVChnIdx,RedundantSample_Task::st_SampleData[uc_Type].m_fValue[M4ADC_ChnNo]);
                    st_RedundantSafetyCheck[uc_IVChnIdx].m_safeStatus = en_Unsafe;
                }

            }
        }
        else
        {
            st_RedundantSafetyCheck[uc_IVChnIdx].m_ReportCnt = 0;
            st_RedundantSafetyCheck[uc_IVChnIdx].m_bReported = false;
            st_RedundantSafetyCheck[uc_IVChnIdx].m_UnsafeCount = 0;
            st_RedundantSafetyCheck[uc_IVChnIdx].m_safeStatus = en_Safe;

        }
        MsTime tempT = CommonFunc::TimeDiff(st_RedundantSafetyCheck[uc_IVChnIdx].m_waitReleaseTick);
        if( st_RedundantSafetyCheck[uc_IVChnIdx].m_safeStatus == en_Unsafe)  //locked waitting release safety cmd
        {

            if(st_RedundantSafetyCheck[uc_IVChnIdx].m_bReported && tempT.Us100>10000 )
            {
                st_RedundantSafetyCheck[uc_IVChnIdx].m_ReportCnt++;
                if(st_RedundantSafetyCheck[uc_IVChnIdx].m_ReportCnt>4)
                {
                    return;
                }
                if(st_RedundantSafetyCheck[uc_IVChnIdx].m_bReleaseCheck)
                    st_RedundantSafetyCheck[uc_IVChnIdx].m_bReported = true;
                else
                    ReportUnsafe(uc_IVChnIdx,RedundantSample_Task::st_SampleData[uc_Type].m_fValue[M4ADC_ChnNo]);
                CommonFunc::GetTimeTicksUpdate(&st_RedundantSafetyCheck[uc_IVChnIdx].m_waitReleaseTick);
            }
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_REDUNDANTSAFETY] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_REDUNDANTSAFETY);
#endif

}

signed char AuxRedundantSafetyCheck::get_M4ChnNo(unsigned char IV_ChnIdx)
{
    unsigned char idx = 0;
    for(idx = 0; idx<M4_MAX_ADCCHANNELNO; idx++)
    {
        if(Aux_Safety_Check::st_IV2M4ADCChnMap.m_IVChnNo[idx] == IV_ChnIdx)
            return Aux_Safety_Check::st_IV2M4ADCChnMap.m_bValid[idx] ? idx:-1;
    }
    return -1;
}


void AuxRedundantSafetyCheck::ReportUnsafe(unsigned char IV_ChnNo,float f_unsafeVal)
{
    St_UartCmdBuf reportUnsafe;

    memset((unsigned char*)&reportUnsafe,0,MAX_UART_RX_LENGTH);

    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_ChnNo = IV_ChnNo;
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_UnsafeType = eType_Voltage;
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_UnsafeValue = f_unsafeVal;
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_Length = sizeof(St_Uart_ReportUnsafe);
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdHead = CMD_UART_REPORT_UNSAFE;
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_CheckSum = 0;
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdStart = UART_CMD_START;  // °üÍ·
    reportUnsafe.m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdEnd = UART_CMD_END;   // °üÎ²

    AuxUartParse::Send_UartCmd(&reportUnsafe);
}
