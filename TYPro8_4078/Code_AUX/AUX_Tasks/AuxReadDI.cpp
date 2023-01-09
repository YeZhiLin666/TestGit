#include "../Entry/includes.h"

//public:
unsigned short               Aux_ReadDI::Priority = PRIORITY4;
unsigned short               Aux_ReadDI::TaskID = AUX_DI_TASK;
void  (*Aux_ReadDI::m_ReadDI)(void);
void  (*Aux_ReadDI::m_InitDI)(void);
//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

#pragma arm section //结束外存变量定义 


////////
//    Init(void)
//    : DI模块初始化处理函数
// Input:  N/A
// Return: N/A
///////
void Aux_ReadDI::Init(void)
{
    (*m_InitDI)();
}

void Aux_ReadDI::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_DI] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_DI);
#endif
    (*m_ReadDI)();

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_DI] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_DI);
#endif
}