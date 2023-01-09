#include "../Entry/includes.h"
UInt16        AuxCheckLimit8_task::Priority=PRIORITY8;
UInt16        AuxCheckLimit8_task::TaskID=CHECKLIMIT_8_TASK;

void AuxCheckLimit8_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CHECKLIMIT8] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CHECKLIMIT8);
#endif

    AuxCheckLimit4_task::Limit_Check(1);
    AuxCheckLimit4_task::IfAny_Limit_Check(1);

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CHECKLIMIT8] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CHECKLIMIT8);
#endif
}

void AuxCheckLimit8_task::MyInit(void)
{

}

void AuxCheckLimit8_task::EndTask(void)
{

}

