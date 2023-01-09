///////////////////////////////////////////////////////////////////////////////////
//File Name: CheckLimit_8.cpp
//Version:        V1.0
//Date:                2013/9/17
//Description:
//                        Step/Log Limit Check
//Others:
//History£º

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
UInt16        Check_Limit8_task::Priority=PRIORITY8;
UInt16        Check_Limit8_task::TaskID=CHECKLIMIT_8_TASK;

void Check_Limit8_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CHECKLIMIT8] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CHECKLIMIT8);
#endif

    Check_Limit4_task::SL_Check(1);

    if(Check_Limit4_task::m_CheckedChannelCount_LL == 0)
	{
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_CHECKLIMIT8);
		#endif
        return;
	}

    Check_Limit4_task::LL_Check(1);
    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        // 		if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_SimuInfo.m_bValid
        // 			&& (!ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_SimuInfo.m_bLogLimitValid))
        // 			continue;

        if(Step_Manager_task::bNeedCheck[ChannelIdx] && Step_Manager_task::bLogLimitChecked[ChannelIdx]&&(!ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_IR_Stop_Logging))  // DKQ do not log data during CCCV relax
        {
            Check_Limit4_task::LL_DoLog(ChannelIdx);
            Step_Manager_task::bLogLimitChecked[ChannelIdx] = 0;
        }
    }
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CHECKLIMIT8] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CHECKLIMIT8);
#endif
}

void Check_Limit8_task::MyInit(void)
{

}

// void Check_Limit8_task::EndTask(void)
// {

// }

