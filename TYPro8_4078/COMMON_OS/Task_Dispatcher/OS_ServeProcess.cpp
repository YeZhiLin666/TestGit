/*****************************************************************************
// 程序文件      :OS_ServeProcess.cpp     主要用于操作系统服务、调试处理。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

/*
Comment out函数指针
void  (* CpFun::pFun) (void);    //定义一个函数指针
CpFun::pFun =  & ( FF::v );    //指向了FF::v这个函数
(* CpFun::pFun) ();    //调用
*/

#include "includes.h"

BYTE VERSION[]	=  "TY2212301";    // IV and AUX Update date

#if  USE_AS_AUXMCU == 0
BYTE IV_DETAIL[] = {"LPC4078 Pro8 TY Build 2212301"}; //Only have 99 chars, please wirte simply.
#else
BYTE AUX_DETAIL[] = {"LPC4078 Pro8 TY Build 2212301"}; //Only have 99 chars, please wirte simply.
#endif
extern REMOTEM rm;

OS_TaskCtrBlock   OS_ServeProcess::OS_Task[TASK_COUNT_MAX];    //任务控制块

unsigned char     OS_ServeProcess::TaskCount;    //FetchNext用来返回一个TaskCount+1 来说明都没找到
BYTE              OS_ServeProcess::Version[16];
St_DETAIL         OS_ServeProcess::Detail;
unsigned char     OS_ServeProcess::Eth_TimeCalied;
unsigned char     OS_ServeProcess::CAN_TimeCalied;
UInt16            OS_ServeProcess::TickFlag = 0;
OS_Task_Online    OS_ServeProcess::Online_Task[MAX_ONLINE_TASK_COUNT];
TASKID_QUEUE      OS_ServeProcess::m_qOnlineTaskID;
unsigned short    OS_ServeProcess::m_OnlineTaskIDmap[MAX_ONLINE_TASK_COUNT];
short             OS_ServeProcess::OnlineTaskCount;

MsTime            OS_ServeProcess::OS_Time = {0x12cff780,0};   //stand for 2010y/1m/1d 0h/0m/0s.0
bool              OS_ServeProcess::OS_Time_Carried = false;
UInt16            OS_ServeProcess::SmallTickCount;
//unsigned char     OS_ServeProcess::TimeCali;
unsigned long     OS_ServeProcess::TimeCaliSecond;
unsigned short    OS_ServeProcess::TimeCaliTicks;
unsigned short    OS_ServeProcess::TimeCaliTickCount;
unsigned char     OS_ServeProcess::AUX_ADC_Idx;
unsigned char     OS_ServeProcess::AUX_ADC_GOTO_Idx;
bool              OS_ServeProcess::BuiltIn_Flag=false;        // 是否存在BUILTIN 功能
unsigned char     OS_ServeProcess::m_SmoothRTCSM;
MsTime            OS_ServeProcess::m_SmoothRTCtime;
signed long long  OS_ServeProcess::m_SmoothRTCtimeErrorInms[20];
unsigned char     OS_ServeProcess::m_SmoothRTCtimeCounter;
//unsigned char     OS_ServeProcess::m_SmoothRTCSetCalibrateUpdate;
short             OS_ServeProcess::m_SmoothRTCSetCalibrateValue = 0;

////for os////
Int16             OS_ServeProcess::Ptr_TaskP1;		// current pointer of Task with priority 1, may be invalid when being used.
Int16             OS_ServeProcess::Ptr_TaskP2;		// these pointers are used for convenience, but need to be checked each time before using
Int16             OS_ServeProcess::Ptr_TaskP4;		// and also, these pointers will avoid a task to be run for multiple time when others with the same priorities
Int16             OS_ServeProcess::Ptr_TaskP8;		// are not executed at all or being executed with less times.
Int16             OS_ServeProcess::Ptr_TaskPBG;
UInt16            OS_ServeProcess::BGCount;			// control the counts for BG work
UInt16            OS_ServeProcess::P8BGSwitch;
UInt16            OS_ServeProcess::Messages[64];		// Messages system: buffer, pointer and count.
UInt16            OS_ServeProcess::MessageCount;
UInt16            OS_ServeProcess::Ptr_Message;

unsigned short   OS_ServeProcess::OTID_4_Reg_SamplePro;   //OnlineTaskID for Regular Sample Process //Add by DKQ 08.20.2014
unsigned short   OS_ServeProcess::OTID_4_Reg_CheckLimit4;
unsigned short   OS_ServeProcess::OTID_4_Reg_CheckLimit8;
unsigned short   OS_ServeProcess::OTID_4_Reg_SafetyCheck;
unsigned short   OS_ServeProcess::OTID_4_Reg_MetaVariable;

signed short     OS_ServeProcess::OTID_4_Pulse_SamplePro;   //OnlineTaskID for Pulse Sample Process
signed short     OS_ServeProcess::OTID_4_Pulse_Check_V_Limit;
signed short     OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit;
signed short     OS_ServeProcess::OTID_4_Pulse_SafetyCheck;
signed short     OS_ServeProcess::OTID_4_Pulse_MetaVariable;
signed short     OS_ServeProcess::OTID_4_Pulse_LogLimit;
bool			 OS_ServeProcess::bUpdateSystemTime_Enable = 0;

unsigned short   OS_ServeProcess::uw16_Counter_RTC_Cali = 0;
unsigned short   OS_ServeProcess::uw16_Limit_RTC_Cali = 0;
unsigned long    OS_ServeProcess::uw32_Cycle1s_Count = 0;   // 秒大循环
unsigned char    OS_ServeProcess::uc_IfAdjust_RTC_Start = 0;
////end////

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

#if DEBUG_TASK_STATISTIC == 1
unsigned long     OS_ServeProcess::PeriodCount;
unsigned long     OS_ServeProcess::PeriodSet;
unsigned long     OS_ServeProcess::Task_Count[MAX_TASK_COUNT+2];
MsTime            OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+2];
#endif

#pragma arm section //结束外存变量定义


//osinit
void OS_ServeProcess::OS_Init(void)
{
    unsigned char i;

    memcpy(OS_ServeProcess::Version,VERSION,16);    //版本 //zyx 180305
    memset(&OS_ServeProcess::Detail,0,MAX_DETAIL);
#if  USE_AS_AUXMCU == 0
    int length = sizeof(IV_DETAIL) / sizeof(IV_DETAIL[0]);
    if(length > MAX_DETAIL)
        length = MAX_DETAIL;
    Detail.length = length;
    memcpy(OS_ServeProcess::Detail.Detail,IV_DETAIL, Detail.length);
    if(length == MAX_DETAIL)
        OS_ServeProcess::Detail.Detail[length-1] = '\0';
#else
    int length = sizeof(AUX_DETAIL) / sizeof(AUX_DETAIL[0]);
    if(length > MAX_DETAIL)
        length = MAX_DETAIL;
    Detail.length = length;
    memcpy(OS_ServeProcess::Detail.Detail,AUX_DETAIL,Detail.length);
    if(length == MAX_DETAIL)
        OS_ServeProcess::Detail.Detail[length-1] = '\0';
#endif
    for(i=0; i<TASK_COUNT_MAX; i++)  //静态分配
    {
        //os init
        OS_Task[i].TaskID = 0;
        OS_Task[i].TaskAddress= NULL;    //隐患
        OS_Task[i].Priority = 0;
        OS_Task[i].IDFlag = 0;    //用于creat重复检测
        //for 信号量
        //SemTable[i].SemID  = 0 ;
        //SemTable[i].IDFlag = 0 ;

        P8BGSwitch = 0;
    }

    TaskCount = TASK_COUNT_MAX;

    //全都初始化为-1，这样就能从0开始
    Ptr_TaskP1 = -1;		// current pointer of Task with priority 1, may be invalid when being used.
    Ptr_TaskP2 = -1;		// these pointers are used for convenience, but need to be checked each time before using
    Ptr_TaskP4 = -1;		// and also, these pointers will avoid a task to be run for multiple time when others with the same priorities
    Ptr_TaskP8 = -1;		// are not executed at all or being executed with less times.
    Ptr_TaskPBG = -1;

    Eth_TimeCalied = 0;
    SmallTickCount = 0;
    m_SmoothRTCSetCalibrateValue = 0;
    uw32_Cycle1s_Count = 0;   // 秒大循环
    uc_IfAdjust_RTC_Start = 0;
    uw16_Counter_RTC_Cali = 0;
    uw16_Limit_RTC_Cali = 0;

    memset(&Online_Task[0],0,sizeof(Online_Task));
    OnlineTaskCount = 0;
    INIT_QUEUE(m_qOnlineTaskID);
    for (unsigned short i = 0; i < MAX_ONLINE_TASK_COUNT; i++)
    {
        ENQUEUE(m_qOnlineTaskID,MAX_ONLINE_TASK_COUNT,i);
        m_OnlineTaskIDmap[i] = MAX_ONLINE_TASK_COUNT+1;
    }

    OTID_4_Pulse_SamplePro         = -1;
    OTID_4_Pulse_Check_V_Limit     = -1;
    OTID_4_Pulse_Check_non_V_Limit = -1;
    OTID_4_Pulse_SafetyCheck       = -1;
    OTID_4_Pulse_MetaVariable      = -1;
    OTID_4_Pulse_LogLimit          = -1;
}

/*函数指针void fun(void)    ,优先级    ticks    ID
//从0开始,ID要按顺序
//1003修改*/
void OS_ServeProcess::OSCreateTask( void (*fDoWork)(void),void (*fInit)(void),unsigned char priority,unsigned char TaskID )    //创建任务
{
    //	static unsigned char prio1ID,prio2ID,prio4ID,prio8ID,prio64ID ;

    if(TaskID < TASK_COUNT_MAX)    //不超过最大任务数
    {
        if(OS_Task[TaskID].IDFlag == 0)	   //重复问题，使用IDFlag，如果重复报错
        {   //  ID1					ID2					ID3					ID4
            //TaskID
            OS_Task[TaskID].TaskID = TaskID;
            OS_Task[TaskID].TaskAddress = (fDoWork);    //函数指针
            OS_Task[TaskID].TaskInitAddress = fInit;    //任务初始化指针
            OS_Task[TaskID].Priority = priority;

            OS_Task[TaskID].IDFlag = 1;    //已经创建过了
        }
        else    //创建重复
        {
            //error
        }
    }
    else    //创建失败    超出最大任务数字
    {
        //error
    }
}

short  OS_ServeProcess::MountTask(unsigned char TaskID, unsigned char Priority, bool bInit)
{
    if(TaskID >= TASK_COUNT_MAX )
        return -1;
    if(!OS_Task[TaskID].IDFlag)
        return -1;                //TaskID对应的任务在任务仓库里没有找到
    if(OnlineTaskCount >= MAX_ONLINE_TASK_COUNT)
        return -1;                //在线任务盘已经满了，容纳不下更多任务了

    Online_Task[OnlineTaskCount].Task          = OS_Task[TaskID];
    Online_Task[OnlineTaskCount].Task.Priority = Priority;
    Online_Task[OnlineTaskCount].m_bSuspended  = false;

    unsigned short OnlineTaskID;
    unsigned char bOK;
    DEQUEUE(m_qOnlineTaskID,MAX_ONLINE_TASK_COUNT,OnlineTaskID,bOK);
    if(bOK)
    {
        Online_Task[OnlineTaskCount].m_OnlineID = OnlineTaskID;
        if(bInit)
            (*Online_Task[OnlineTaskCount].Task.TaskInitAddress)();
        m_OnlineTaskIDmap[OnlineTaskID] = OnlineTaskCount;   //establish a map,2014/12/29
        OnlineTaskCount++;
        return OnlineTaskID;
    }
    else
        return -1;     //error
}
void   OS_ServeProcess::DisMountTask(unsigned short OnlineTaskID)
{
    unsigned short TaskIdx;
    for (TaskIdx = 0; TaskIdx < OnlineTaskCount; TaskIdx++)
    {
        if(Online_Task[TaskIdx].m_OnlineID == OnlineTaskID)
        {
            for (unsigned short i = TaskIdx; i<(OnlineTaskCount - 1); i++)
            {   //删除一个指定的任务，需要将后面的所有任务前移
                memcpy(&Online_Task[i],&Online_Task[i+1],sizeof(OS_Task_Online));
            }
            m_OnlineTaskIDmap[OnlineTaskID] = MAX_ONLINE_TASK_COUNT+1;   //distroy a map,2014/12/29
            OnlineTaskCount--;
            ENQUEUE(m_qOnlineTaskID,MAX_ONLINE_TASK_COUNT,OnlineTaskID);    //回收废弃的 在线任务ID
            return;
        }
    }
}
void   OS_ServeProcess::SuspendTask(unsigned short OnlineTaskID)
{
    unsigned short TaskIdx;
    for (TaskIdx = 0; TaskIdx < OnlineTaskCount; TaskIdx++)
    {
        if(Online_Task[TaskIdx].m_OnlineID == OnlineTaskID)
        {
            Online_Task[TaskIdx].m_bSuspended = 1;
            return;
        }
    }
}
void   OS_ServeProcess::ResumeTask(unsigned short OnlineTaskID,bool bInit)
{
    unsigned short TaskIdx;
    for (TaskIdx = 0; TaskIdx < OnlineTaskCount; TaskIdx++)
    {
        if(Online_Task[TaskIdx].m_OnlineID == OnlineTaskID)
        {
            if(bInit)
                (*Online_Task[TaskIdx].Task.TaskInitAddress)();
            Online_Task[TaskIdx].m_bSuspended = 0;
            return;
        }
    }
}
#if  USE_AS_AUXMCU == 1
bool OS_ServeProcess::IsMounted(unsigned short TaskID)
{
    unsigned short TaskIdx;
    for (TaskIdx = 0; TaskIdx < OnlineTaskCount; TaskIdx++)
    {
        if(Online_Task[TaskIdx].Task.TaskID == TaskID)
            return true;
    }
    return false;
}
#endif
//unsigned char OS_ServeProcess::Get_TaskPriority(unsigned short OnlineTaskID)
//{
//	unsigned short TaskIdx;
//	for (TaskIdx = 0;TaskIdx < OnlineTaskCount;TaskIdx++)
//	{
//		if(Online_Task[TaskIdx].m_OnlineID == OnlineTaskID)
//			return Online_Task[TaskIdx].Task.Priority;
//	}
//	return 64;	// add this return to avoid warning, never reach in actual running
//}

//void   OS_ServeProcess::Set_TaskPriority(unsigned short OnlineTaskID,unsigned char Priority)
//{
//	unsigned short TaskIdx;
//	for (TaskIdx = 0;TaskIdx < OnlineTaskCount;TaskIdx++)
//	{
//		if(Online_Task[TaskIdx].m_OnlineID == OnlineTaskID)
//		{
//			Online_Task[TaskIdx].Task.Priority = Priority;
//			return;
//		}
//	}
//}
void   OS_ServeProcess::Set_TaskPriority2(unsigned short OnlineTaskID,unsigned char Priority)
{
    if(OnlineTaskID >= MAX_ONLINE_TASK_COUNT)
        return;
    if(m_OnlineTaskIDmap[OnlineTaskID] >= MAX_ONLINE_TASK_COUNT)
        return;

    Online_Task[m_OnlineTaskIDmap[OnlineTaskID]].Task.Priority = Priority;
}

/*os start
// Input: N/A
// Return: N/A
//使能timer0 使ticks运转*/
void OS_ServeProcess::OS_Start(void)
{
    TIMERSFunc::Enable_Timer(0);    //使能Timer0  startOS时间运转
    TIMERSFunc::Enable_Timer(2);    //使能Timer0  startOS时间运转
#if THREAD_TASK_TIME_STATISTIC == 1
    TIMERSFunc::Enable_Timer(1);		//使能Timer1  startOS时间运转
#endif
}

/*重要
调度过程：
优先级之所以定义成为1，2，4，8，64就是二进制的每一位，每位代表一个优先级一共5位，用0代表这个任务消亡
还预留了16和32以备用

先是进行ProcessMessage消息处理，即任务间通讯，可以消亡任务

然后遍历各个优先级任务
(highest_pri & PRIORITY1) != 0    //任务表中有PRIORITY1的任务
执行ProcessPriority1();

具体任务优先级请参照OS_Manual.docx    by Xu

FetchNextTask就是说在OS_Task[]这个任务控制块中，找到符合优先级的任务，并返回该任务ID
但是每次执行会按照当前的proi_Ptr的下一个ID开始寻找，找到表尾，从0开始到proi_Ptr

方案1
使用ifForEach从0开始查到表尾

方案2
就是按照表从proi_Ptr下一个ID一个一个去查，查到表底，从0开始到proi_Ptr

创建任务时Id一定是要依次进行的
*/

/*/ Authorize
// Input: N/A
// Return: N/A*/
void OS_ServeProcess::OSAuthorize(void)
{
    if(OnlineTaskCount == 0)
        return;  //no task

    unsigned char  i;
    unsigned char highest_pri;

//	OS_ServeProcess::ProcessMessage();    //任务通信

    highest_pri = 0;    //clear
    for(i = 0; i < OnlineTaskCount; i ++)			// Find out the lowest Priority in the Task List.
        highest_pri |= Online_Task[i].Task.Priority;//刷新优先级

    if((highest_pri & PRIORITY1) != 0)
    {
        ProcessPriority1();
        return;
    }

    if((highest_pri & PRIORITY2) != 0)
    {
        ProcessPriority2();
        return;
    }
    if((highest_pri & PRIORITY4) != 0)
    {
        ProcessPriority4();
        return;
    }
    if((highest_pri & (PRIORITY8 | PRIORITYBG)) != 0)
    {
        ProcessPriority8GB();
        return;
    }
}//end OSAuthorize

/*/ process prio1
// Input: N/A
// Return: N/A*/
void OS_ServeProcess::ProcessPriority1(void)
{
    Ptr_TaskP1 = FetchNextTask(PRIORITY1, 0);
    if(Ptr_TaskP1 < OnlineTaskCount)
        (* Online_Task[Ptr_TaskP1].Task.TaskAddress) ();		// call the task DoWork()
}
/*/process prio2
// Input: N/A
// Return: N/A*/
void OS_ServeProcess::ProcessPriority2(void)
{
    Ptr_TaskP2 = FetchNextTask(PRIORITY2, 0);
    if(Ptr_TaskP2 < OnlineTaskCount)
        (* Online_Task[Ptr_TaskP2].Task.TaskAddress) ();				// call the task DoWork()

    if(TickFlag)    //还有点时间
    {
        TickFlag = 0;
        BGCount ++;
        if(BGCount >= BGTICKS)    //背景任务
        {
            BGCount = 0;
            Ptr_TaskPBG = FetchNextTask(PRIORITYBG, 0);//找不到返回TaskCount+1
            if(Ptr_TaskPBG < OnlineTaskCount)
            {
                (* Online_Task[Ptr_TaskPBG].Task.TaskAddress) ();	// call the task DoWork()
            }
        }
    }
}
/*/Task4是每ticks都要做一遍
//process prio4
// Input: N/A
// Return: N/A*/
void OS_ServeProcess::ProcessPriority4(void)
{
    //static int no8BGError = 0;

    if(TickFlag)
    {
        Ptr_TaskP4 = 0;
        if((Online_Task[0].Task.Priority == PRIORITY4)		// Don't miss task 0 if it is a priority 4 task.
                && (!Online_Task[0].m_bSuspended))
        {
            (* Online_Task[0].Task.TaskAddress) ();			// call the task DoWork()
        }
        Ptr_TaskP4 = FetchNextTask(PRIORITY4,1);//从0开始
        while(Ptr_TaskP4 < OnlineTaskCount)//都没找到
        {
// 			if(Ptr_TaskP4 < OnlineTaskCount)
// 			{
            (* Online_Task[Ptr_TaskP4].Task.TaskAddress) ();	// call the task DoWork()
            Ptr_TaskP4 = FetchNextTask(PRIORITY4,1);
// 			}
        }
        TickFlag = 0;								// clear the Tick Flag and relocate resource to low priority tasks
        return;
    }

    OS_ServeProcess::ProcessPriority8GB();    //????
}
/*/process prio 8BG
// Input: N/A
// Return: N/A*/
void OS_ServeProcess::ProcessPriority8GB(void)
{
    if(P8BGSwitch)    //8和64来回切换
    {
        P8BGSwitch = 0;
        Ptr_TaskP8 = FetchNextTask(PRIORITY8, 0);
        if(Ptr_TaskP8 < OnlineTaskCount)
        {
            (* Online_Task[Ptr_TaskP8].Task.TaskAddress) ();	// call the task DoWork()
        }
    }
    else
    {
        P8BGSwitch = 1;
        Ptr_TaskPBG = FetchNextTask(PRIORITYBG, 0);
        if(Ptr_TaskPBG < OnlineTaskCount)
        {
            (* Online_Task[Ptr_TaskPBG].Task.TaskAddress) ();	// call the task DoWork()
        }
    }
}
////消息处理////
// void OS_ServeProcess::ProcessMessage(void)
// {
// 	int i;

// 	if(MessageCount == 0)
// 		return;

// 	for(i = 0; i < MessageCount; i ++)
// 	{
// 		switch(Messages[(Ptr_Message + i) & MESSAGEMASK])
// 		{
// need to define messages and a lot of cases here.
// <<<end>>> need to define messages and a lot of cases here.
// 		default:
// 			break;
// 		}
// 		Ptr_Message ++;
// 		MessageCount --;
// 	}
// }
//int OS_ServeProcess::SendMessages(UInt16 myMessage)
//{
//	if(MessageCount >= MESSAGEMASK + 1)
//		return 1;

//	Messages[(Ptr_Message + MessageCount) & MESSAGEMASK] = myMessage;
//	MessageCount ++;

//	return 1;
//}
////end 消息处理////

/*///////
// FetchNextTask(): get the next task which has the required priority in the task list
//    Current Positions provided by Ptr_TaskPxx, and this positon is not included in searching
// Input: myPriority - We are trying to look for a task with priority "myPriority"
//        ifForEach - 0, search from current position, the whole list, with index roll over
//                  - 1, search from current position, till the end of the list.
// Return: the index of searched task. if no task meets the requirements, return a value larger than TaskCount.
////////
//因为优先级4的任务需要在一轮呢都查询一边，所以添加了ifForEach
//从0开始查到表尾
//如果没找到 返回TaskCount+1
//所以基本的任务需要判断if( (FetchNextTask)这个值 >TaskCount)*/
UInt16 OS_ServeProcess::FetchNextTask(UInt16 myPriority,UInt16 ifForEach)
{
    unsigned char ptr_task;

    switch(myPriority)
    {
    case PRIORITY1:
        ptr_task = Ptr_TaskP1;
        break;
    case PRIORITY2:
        ptr_task = Ptr_TaskP2;
        break;
    case PRIORITY4:
        ptr_task = Ptr_TaskP4;
        break;
    case PRIORITY8:
        ptr_task = Ptr_TaskP8;
        break;
    case PRIORITYBG:
        ptr_task = Ptr_TaskPBG;    //背景
        break;
    default:
        ptr_task = Ptr_TaskPBG;    //背景
        break;
    }
    if(ptr_task>=OnlineTaskCount)
        ptr_task = 0;    //clear


    if(ifForEach == 1)//到结尾就结束了
    {
        unsigned char j;
        for(j = (ptr_task+1); j<OnlineTaskCount; j++)    //从当前的到结尾
        {
            if(Online_Task[j].m_bSuspended)
                continue;
            if(Online_Task[j].Task.Priority == myPriority)
                return j;
        }
        return TaskCount+1;//都没找到
    }
    else//ifForEach为0应该是翻转的
    {
        unsigned char ptr_task_next;
        unsigned char i;

        ptr_task_next = ptr_task+1;    //指向下一个任务表    所有ptr开始都为-1
        while(ptr_task_next < OnlineTaskCount)
        {
            if(Online_Task[ptr_task_next].m_bSuspended)
            {
                ptr_task_next++;
                continue;
            }
            if(Online_Task[ptr_task_next].Task.Priority == myPriority)    //满足要求
                return ptr_task_next;
            ptr_task_next++;
        }
        //从ptr_task_next到末尾没有找到    从0头开始到ptr_task_next
        for(i = 0; i< ptr_task_next; i++)
        {
            if(Online_Task[i].m_bSuspended)
                continue;
            if( Online_Task[i].Task.Priority == myPriority)
            {
                return i;
            }
        }
        //也就是说以ptr_task_next为界，先找后面的，然后找前面的

        //都没找到
        return OnlineTaskCount+1;
    }
}

void OS_ServeProcess::Adjust_RTC_Time(void)
{
    if( uw16_Counter_RTC_Cali >= uw16_Limit_RTC_Cali)
    {
        OS_ServeProcess::RTC_SetNewSystemTime();
        if(--uw32_Cycle1s_Count <= 0)
        {
            uw32_Cycle1s_Count = 0;
            uc_IfAdjust_RTC_Start = 0;  // 秒大循环结束
            OverTimeStartRTC(uw16_Counter_RTC_Cali-2);
        }
        uw16_Counter_RTC_Cali = 0;
    }
}
/*
Timer0HandlerISR(void): Tick Interrupt Service Routine.
This update the OS_Time Tick section and set Tick Flag.
Input: N/A
Return: N/A
*/
void OS_ServeProcess::Timer0HandlerISR(void)//	__irq    //for cpu ticks
{
    uw16_Counter_RTC_Cali ++;
//#if (((_BOARD_TYPE_  == BOARDTYPE_BT11 )||(_BOARD_TYPE_  == BOARDTYPE_HS21044 )) && (USE_AS_AUXMCU == 0))
    //单通道系统里只对系统时间的100us 加2， 不再做采样驱动. 普通的采样当作一个
    //普通任务. ，也就是在200us内花15us来采三次样。码值放到大数据体去。
#if (USE_AS_AUXMCU == 0)
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )
    {
        OverTimeStartRTC(15000);  // 超过3秒保护打开RTC中断  200us 3000*5

        if(OS_Time.Us100 < 9998)
            OS_Time.Us100 += 2;
        else
        {
            if(bUpdateSystemTime_Enable == 1)
            {
                CommonFunc::SetSystemTimeCarried();
                RTCFunc::ReStartRTC();
                bUpdateSystemTime_Enable = 0;
            }

        }

        if(uc_IfAdjust_RTC_Start)
        {
            OS_ServeProcess::Adjust_RTC_Time();
        }

        TickFlag = 1;          // Set Tick Flag: another Tick happened.
    }
    else
    {
        OverTimeStartRTC(60000);  // 超过3秒保护打RTC中断  50us 20*3000

        SmallTickCount++;
        //if(SmallTickCount >= 19999)
        //TIMERSFunc::Init_Timer(TIME_INTERVAL_50US * 3 ,0);   //change to 150us when RTC ISR is soon to come//  zyx 2004271

        if((SmallTickCount & 0x01) == 0)
        {
//             if(TimeCali)
//             {
//                 TimeCaliTicks++;
//                 if(TimeCaliTicks >= TimeCaliTickCount)
//                 {
//                     OS_Time.Second = TimeCaliSecond;
//                     OS_Time.Us100 = 0;
//                     RTCFunc::RTC_CTCReset();
//                     TimeCali = 0;
//                     return;
//                 }
//             }
            if(OS_Time.Us100 < 9999)  // 20130910 何  修补准确时间
            {
                OS_Time.Us100++;
            }
            else
            {
                if(bUpdateSystemTime_Enable == 1)
                {
                    CommonFunc::SetSystemTimeCarried();
                    RTCFunc::ReStartRTC();
                    bUpdateSystemTime_Enable = 0;
                }
            }

            if(uc_IfAdjust_RTC_Start)
            {
                OS_ServeProcess::Adjust_RTC_Time();
            }

            if(board_config::Feature_Enable_init.OS_Tick_Interval==200)
            {
                if((SmallTickCount & 0x03) == 0)   //200us time tick
                    TickFlag = 1;
            }
            else if(board_config::Feature_Enable_init.OS_Tick_Interval==400)
            {
                if((SmallTickCount & 0x07) == 0)   //400us time tick
                    TickFlag = 1;
            }
            else if(board_config::Feature_Enable_init.OS_Tick_Interval==800)
            {
                if((SmallTickCount & 0x0F) == 0)   //20131009 hepeiqing,TimeTick as 800us
                    TickFlag = 1;// Set Tick Flag: another Tick happened.
            }
            else
            {
                if((SmallTickCount & 0x1F) == 0)   //20131009 hepeiqing,TimeTick as 1.6ms
                    TickFlag = 1;// Set Tick Flag: another Tick happened.
            }
        }
    }
#else

    OverTimeStartRTC(60000);  // 超过3秒保护打RTC中断  50us 20*3000

    SmallTickCount++;

    if((SmallTickCount & 0x01) == 0)
    {
        if(OS_Time.Us100 < 9999)  // 20130910 何  修补准确时间
            OS_Time.Us100++;
        else
        {
            if(bUpdateSystemTime_Enable == 1)
            {
                CommonFunc::SetSystemTimeCarried();
                RTCFunc::ReStartRTC();
                bUpdateSystemTime_Enable = 0;
            }
        }

        if(uc_IfAdjust_RTC_Start)
        {
            OS_ServeProcess::Adjust_RTC_Time();
        }

#if OS_TICK_INTERVAL == 200
        if((SmallTickCount & 0x03) == 0)   //200us time tick
#elif OS_TICK_INTERVAL == 400
        if((SmallTickCount & 0x07) == 0)   //400us time tick
#else
        if((SmallTickCount & 0x0F) == 0)   //20131009 hepeiqing,TimeTick as 800us
#endif
            TickFlag = 1;// Set Tick Flag: another Tick happened.
    }

    // 	if((OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
    // 		ChannelInfo_Manager::Set_TimeRecalculateSign();  //这部分移到board_config::Func_Sample函数里处理，何培青20131118
#endif  //#if (_BOARD_TYPE_  == BOARDTYPE_BT11 )

#if(USE_AS_AUXMCU==1) //&& ( _USER_BOARD_TYPE_ ==  AUX_TEMPLM75_FOR_ZHONGDIAN))   

    unsigned char Last_chn;
    //if(AUX_Unit_Base::Aux_Init_Finished_flag==0)
    if(AUX_Unit_Base::m_EnableISRsample==false)

        return;

    if(!Aux_DataManager::m_Enable_2ndV_LC_Sample)
    {
        if(EthernetParse::m_AUX_WriteMap)
            return;

        AUX_ADC_Idx = Aux_DataManager::FindNextAuxADCid();
        switch(Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_StateMachine)
        {
        case AUX_Sample_IDLE:
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_StateMachine = AUX_Sample_TRIGGER;
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].b_Finished=0;
            break;
        case AUX_Sample_TRIGGER:
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].counter=0;
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].b_Triggered=1;
            Aux_DataManager::SettingCS_Low(Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_CS_Address.m_Value);//拉低片选
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_BoardADC_Trigger(AUX_ADC_Idx,Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_WorkingChn);
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_StateMachine = AUX_Sample_READ;
            break;
        case AUX_Sample_READ:
            Last_chn= Aux_DataManager::findLastChn(AUX_ADC_Idx);
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_BoardADC_Rd(AUX_ADC_Idx,Last_chn);

            Aux_DataManager::SettingCS_High();

            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].counter=0;
            Aux_DataManager::Aux_ADC_TimeOut_Flag &= ~(1<<AUX_ADC_Idx);				//读取数据后清标志
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].b_Finished=0;
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_StateMachine = AUX_Sample_WAIT;

            AUX_ADC_Idx = AUX_ADC_GOTO_Idx;
            break;

        case  AUX_Sample_WAIT:
            break;
        default:
            break;

        }
    }
    else	// For LC HIM Board
    {
//		static unsigned long long ticks = 0;
//		static MsTime TimeStamp1;
        switch(Aux_Sample_2ndV_LC::TaskStatus)
        {
        case  AUX_2ndV_LC_Sample_IDLE:
            Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_SWITCH;
            break;

        case  AUX_2ndV_LC_Sample_SWITCH:
            Aux_Sample_2ndV_LC::SetChip(Aux_Sample_2ndV_LC::ChanIdx);
            Aux_Sample_2ndV_LC::TimeCount = 0;
            Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_WAIT1;
            break;

        case  AUX_2ndV_LC_Sample_WAIT1:
            if (++Aux_Sample_2ndV_LC::TimeCount > Aux_Sample_2ndV_LC::Sample_Wait_Count)		// 12ms	// normally, we wait 3ms and go to next step. But if the board does not have slave board(only 8 channel), we need to rise the wait time. Need futher test
                Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_TRIGGER;
            else
                Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_WAIT1;
            break;

        case	AUX_2ndV_LC_Sample_TRIGGER:
            CommonFunc::GetTimeTicksUpdate(&Aux_Sample_2ndV_LC::WaitTime);
            if(Aux_Sample_2ndV_LC::Board_Adc_Trigger != 0)
                Aux_Sample_2ndV_LC::Board_Adc_Trigger();	// Trigger for all 2ndV_LC boards
            //TimeStamp1.Second = OS_ServeProcess::OS_Time.Second;
            //TimeStamp1.Us100	= OS_ServeProcess::OS_Time.Us100;
            //OS_ServeProcess::recordCurTime(&Aux_Sample_2ndV_LC::TimeStamp);
            Aux_Sample_2ndV_LC::TimeCount = 0;
            Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_WAIT2;
        //Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_READ;

        case  AUX_2ndV_LC_Sample_WAIT2:
            if (++Aux_Sample_2ndV_LC::TimeCount > Aux_Sample_2ndV_LC::Sample_Wait_Count2)		//0.6ms
                Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_READ;
            else
                Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_WAIT2;
            break;

        case 	AUX_2ndV_LC_Sample_READ:
            for(;;)    //while(1)
            {
                if(Aux_Sample_2ndV_LC::CS_Idx>7)	// At most 8 LC boards
                    break;


                if ((Aux_DataManager::Aux_ADC_CS_Info[Aux_Sample_2ndV_LC::CS_Idx].m_Valid)&&\
                        Aux_DataManager::Aux_ADC_CS_Info[Aux_Sample_2ndV_LC::CS_Idx].b_ADCCH_Valid[Aux_Sample_2ndV_LC::ChanIdx])  //only sample for a valid board and valid channels
                    break;
                else
                    Aux_Sample_2ndV_LC::CS_Idx++;

            }

            if(Aux_Sample_2ndV_LC::CS_Idx>7)	// Finish one cycle read, go to next channel scan
            {
                Aux_Sample_2ndV_LC::CS_Idx = 0;
                Aux_Sample_2ndV_LC::ChanIdx++;

                if(Aux_Sample_2ndV_LC::ChanIdx > 15)
                    Aux_Sample_2ndV_LC::ChanIdx = 0;

                Aux_Sample_2ndV_LC::TaskStatus = AUX_2ndV_LC_Sample_SWITCH;
                break;
            }

            Aux_DataManager::SettingCS_Low(Aux_DataManager::Aux_ADC_CS_Info[Aux_Sample_2ndV_LC::CS_Idx].m_CS_Address.m_Value);
            if(Aux_Sample_2ndV_LC::Board_Adc_Read != 0)
                Aux_Sample_2ndV_LC::Board_Adc_Read(Aux_Sample_2ndV_LC::CS_Idx, Aux_Sample_2ndV_LC::ChanIdx);
            Aux_DataManager::SettingCS_High();
            Aux_Sample_2ndV_LC::CS_Idx++;
            break;

        default:
            break;
        }
    }

#else
    if(board_config::Func_Sample != 0)
        (* board_config::Func_Sample)(0,0);
    if(BuiltIn_Flag)
    {
        BuiltIn_DataManager::SampleISR();
    }
#endif
}
/*
// RTCHandlerISR(void): RTC Interrupt Service Routine.
//   This update the OS_Time RTC sections and reset Tick section.
// Input: N/A
// Return: N/A
*/
void OS_ServeProcess::RTCHandlerISR(void)// __irq    //暂时只用来复位timer0
{
    //复位Timer
    TIMERSFunc::Reset_Timer(0);
    OS_ServeProcess::RTC_SetNewSystemTime();

    if(OS_ServeProcess::uw32_Cycle1s_Count>0)
    {
        RTCFunc::StopRTC();  // 为 0 时，时间计数器都被禁止，这时可对其进行初始化。
        OS_ServeProcess::uc_IfAdjust_RTC_Start = 1;
    }
    OS_ServeProcess::uw16_Counter_RTC_Cali = 0;
}

/*/加入快照及保护机制
//Timer Cali
//comment out chen 20130315
//上位机setTime进行校准*/

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INIT_A系统级别初始化A部分，基础初始化*/
void OS_ServeProcess::Init_Device_Firmware(void)
{
    ////////////////////////////////TargetRest////////////////////////////////
    // 启动函数使用主频CCLK=120M;PCLK=60M //

    Arbin_SC->RSTCON0.BitReg.RSTGPIO = 1;
    Arbin_SC->RSTCON0.BitReg.RSTGPIO = 0;
    Arbin_SC->RSTCON1.BitReg.RSTIOCON = 1;
    Arbin_SC->RSTCON1.BitReg.RSTIOCON = 0;
    for(unsigned long uw32_i = 0; uw32_i < DEV_WAITTIME; uw32_i ++);	//等待其他器件初始化完毕



    ///////////////////////////////IRQInit///////////////////////////////
    IRQFunc::init_VIC();
    //测试I2C总线EEPROM

    ////////////////////////////////Timer////////////////////////////////
    //20150324,hepeiqing:
    //对于IV通道，只有BT11会使用200US TICK；
    //对于AUX通道，只有高共模二次电压板会用200US TICK；
    //其他任何情况，均使用50US TICK；
    //#if (((_BOARD_TYPE_  == BOARDTYPE_BT11 ) && (USE_AS_AUXMCU == 0)) || ((USE_AS_AUXMCU == 1) && (AUX_2ndV_FOR_HRNEW == 1)))
#if (((_BOARD_TYPE_  == BOARDTYPE_BT11 )||(_BOARD_TYPE_  == BOARDTYPE_HS21044 )) && (USE_AS_AUXMCU == 0))
    TIMERSFunc::Init_Timer(TIME_INTERVAL_200US,0);     //Timer0 init    //200us 中断一次,因为单通道，不用中断驱动采样
#else
    TIMERSFunc::Init_Timer(TIME_INTERVAL_50US,0);     //Timer0 init    //50us 中断一次
#endif
    //  NVIC_SetPriority(TIMER0_IRQn,0);    //设置为0优先级  chen 20131226

    //TIMERSFunc::Init_Timer(1000*TIME_INTERVAL_0 ,2);    //Timer2 init    //100ms 中断一次
    TIMERSFunc::Init_Timer(TIME_INTERVAL_T2,2);     //Timer2 init    //10ms 中断一次  // 允许值: 10ms, 20ms, 25ms, 40ms, 50ms, 100ms, 或 200 ms。

    ////////////////////////////////GPIO////////////////////////////////
    for(unsigned long uw32_i = 6000000; uw32_i >0; uw32_i --);	//delay

    ////////////////////////////////SDRAM////////////////////////////////
    SDRAMFunc::SDRAM_Init();//外存

#if (USE_AS_AUXMCU == 0)
//#if (_BOARD_TYPE_  == BOARDTYPE_IBT31162)   // 避免AD8450芯片发热    20161018
//     board_config::Func_Avoid_Hot = &IBT31162_Base::Avoid_Hot;
//     if( board_config::Func_Avoid_Hot != 0)
//         ( *board_config::Func_Avoid_Hot)();

//#endif
#endif

    for(unsigned long uw32_i = 0; uw32_i < 6000000; uw32_i ++);	//等待其他器件初始化完毕

    ////////////////////////////////RTC////////////////////////////////
    RTCFunc::RTCInit();

    ////////////////////////////////SPI////////////////////////////////
    ////////////////////////////////I2C////////////////////////////////
    ////////////////////////////////CAN////////////////////////////////
    CANFunc::InitMyCAN();
    ////////////////////////////////UART////////////////////////////////
    ////////////////////////////////EINT////////////////////////////////
    ////////////////////////////////Others////////////////////////////////
    I2CBusNew::I2CInit(I2C0,100);
    I2CBusNew::I2CInit(I2C1,100);
    I2CBusNew::I2CInit(I2C2,100);
#if (USE_AS_AUXMCU == 1)
    ADCFunc::Init ();
#endif
    ///////////////////////////////////////////////////////////////////
    INITTEST_PIN;

    TIMERSFunc::DELAY_SYS( 30000,0);	//delay 5ms
    TIMERSFunc::DELAY_SYS( 5000,1);	//delay 5s     // 20161123

    ////////////////////////////////Start////////////////////////////////
    RTCFunc::RTCStart();    //start RTC
    //     #if MCU_WATCHDOG_ENABLE == 1
    //     WDTFunc::Watchdog_Init();     //Add by DKQ 06_17_2014
    //     #endif
    OS_ServeProcess::OS_Start();    //启动Timer0

#if  USE_AS_AUXMCU == 1
    AuxCConfig::MyInit();
#endif

//#if((REDUNDANT_VOLT_MODE == REDUNDANT_M0) && (USE_AS_AUXMCU == 0))
#if(USE_AS_AUXMCU == 0)
//if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)
    //UARTFunc::UART1_Init(115200);
    //UARTFunc::UART1_Init(9600);
    UARTFunc::UARTInit( 1, TC_BR_9600, TC_DATA_BIT_8, TC_PARITY_NONE, TC_STOP_BIT_ONE );
    //DMAFunc::SetupUart0();
#endif

// Initial SSPI0 to read FRAM later, Yifei, 2016.04.29

}

void OS_ServeProcess::Init_NetWork(void)
{
    ////////////////////////////////Ethernet////////////////////////////////
    //TIMERSFunc::Init_Timer(TIME_INTERVAL_T3 ,3);  // init timer 3 to 100ms used by TCPnet.
    init_TcpNet();
    // "同步信道":工作
    TcpRxTx::socket_tcp = tcp_get_socket ((TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE), 0, 10, TcpRxTx::tcp_callback);
    if (TcpRxTx::socket_tcp != 0)
        tcp_listen (TcpRxTx::socket_tcp, PORT_NUM);
    // "异步信道":工作
    TcpRxTx::socket_tcp2 = tcp_get_socket ((TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE), 0, 10, TcpRxTx::tcp_callback2);
    if (TcpRxTx::socket_tcp2 != 0)
        tcp_listen (TcpRxTx::socket_tcp2, PORT_NUM2);
    memcpy(rm.IpAdr, uc_remotemIpAdr, IP_ADRLEN);
    arp_get_info (& rm);

    UdpRxTx::Init_McuUdp();
// 	CommonFunc::GetTimeTicksUpdate( &EthernetParse::m_Interval_NetLinkTime); // 更新网络连接成功时间间隔器
}
// void OS_ServeProcess::GetTimeTicksUpdate(MsTime * time)
// {
//     MsTime os_Time;
//     unsigned long irq1,irq2;
//     IRQFunc::DisableIRQ(&irq1,&irq2);
//     os_Time = OS_ServeProcess::OS_Time;
//     IRQFunc::EnableIRQ(irq1,irq2);
//     *time = os_Time;
// }

void OS_ServeProcess::RestartSystemTime(MsTime SetSystemTime)
{
    RTCFunc::StopRTC();
    CommonFunc::SetSystemTime(SetSystemTime);
    OS_ServeProcess::bUpdateSystemTime_Enable = 1;
    m_SmoothRTCSM = ESMOOTHRTCSM_IDLE;
    m_SmoothRTCtime = CommonFunc::GetSystemTime();  // 开始时不调，1小时后才调
    m_SmoothRTCtimeCounter = 0;
}

void OS_ServeProcess::RTC_SetNewSystemTime(void)
{
    CommonFunc::SetSystemTimeCarried();// Reset System Time Tick section.

#if DEBUG_TASK_STATISTIC == 1
    for(unsigned char uc_i = 0; uc_i < MAX_TASK_COUNT; uc_i ++)
        Task_Count[uc_i] = 0;
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    if(time_count_reset_flag)
    {
        time_count_reset_flag = 0;
        Time_Count::time_reset();
    }
#endif

    TickFlag = 1;				// "a longer or shorter tick" now happened.

#if USE_AS_AUXMCU ==0
    board_config::RTCProcedureForIV(0,0);
#endif

}

void OS_ServeProcess::SetSystemTimeSmooth(MsTime SetSystemTime)
{
    signed long long errorMax = 0;
    signed long long errorMin = 0;
    signed long long errorAvg = 0;
    double errorPPM = 0;
    switch(m_SmoothRTCSM)
    {
    case ESMOOTHRTCSM_GETTIME:
        m_SmoothRTCtimeErrorInms[m_SmoothRTCtimeCounter] = CommonFunc::TimeDiffInMS(SetSystemTime);   // MCU时间-PC时间取毫秒值
        m_SmoothRTCtimeCounter++;
        if(m_SmoothRTCtimeCounter >= 12)
            m_SmoothRTCSM = ESMOOTHRTCSM_CALCULATION;
        break;
    case ESMOOTHRTCSM_CALCULATION:
        m_SmoothRTCSM = ESMOOTHRTCSM_IDLE;
        m_SmoothRTCtime = CommonFunc::GetSystemTime();
        // __int64的最大值：9223372036854775807
        // __int64的最小值：-9223372036854775808
        // unsigned __int64的最大值：18446744073709551615
        errorMax = -9223372036854775700;
        errorMin = 9223372036854775700;
        errorAvg = 0;
        for(int i = 0; i < 12; i++)
        {
            if(m_SmoothRTCtimeErrorInms[i] > errorMax)
                errorMax = m_SmoothRTCtimeErrorInms[i];
            if(m_SmoothRTCtimeErrorInms[i] < errorMin)
                errorMin = m_SmoothRTCtimeErrorInms[i];
            errorAvg += m_SmoothRTCtimeErrorInms[i];
        }
        errorAvg = errorAvg - errorMax;
        errorAvg = errorAvg - errorMin;
        errorAvg /= 10;
        if(errorAvg > 200)  // 最大200ms 一般RTC晶振1分钟差1毫秒，1小时调整一次
            errorAvg = 200;
        else if(errorAvg < -200)
            errorAvg = -200;

        //do RTC calibration;
        if(errorAvg != 0)
        {
            if(errorAvg>0)
            {
#if (USE_AS_AUXMCU == 0)
                if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 10010/2;  // 延后1毫秒 200us*10010/2=1001000;
                else
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 10010*2;  // 延后1毫秒 50us*10010*2=1001000;
#else
                OS_ServeProcess::uw16_Limit_RTC_Cali = 10010*2;  // 延后1毫秒 50us*10010*2=1001000;
#endif
            }
            else
            {
#if (USE_AS_AUXMCU == 0)
                if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 9990/2;	// 提前1毫秒  200us*9990/2=999000;
                else
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 9990*2;	    // 提前1毫秒  50us*9990*2=999000;
#else
                OS_ServeProcess::uw16_Limit_RTC_Cali = 9990*2;	// 提前1毫秒 50us*9990*2=999000;
#endif
            }
        }
        m_SmoothRTCSetCalibrateValue = errorAvg;
        OS_ServeProcess::uc_IfAdjust_RTC_Start = 0;
        OS_ServeProcess::uw32_Cycle1s_Count = abs(errorAvg);
        EthernetParse::m_IfRequest_NTP = 1;   // 为提前请求准备
        break;
    case ESMOOTHRTCSM_IDLE:
    default:
        MsTime mydiff = CommonFunc::TimeDiff(m_SmoothRTCtime);
        if(mydiff.Second >= RTC_CALIB_INTERVAL_TIME_PARSE) //if(mydiff.Second > 3600*24)
        {
            m_SmoothRTCSM = ESMOOTHRTCSM_GETTIME;
            m_SmoothRTCtimeCounter = 0;
            //m_SmoothRTCtime = CommonFunc::GetSystemTime();
        }

        if((mydiff.Second >= RTC_CALIB_INTERVAL_TIME_REQUEST)&&(EthernetParse::m_IfRequest_NTP == 1))
        {
            EthernetParse::m_IfRequest_NTP = 0;   // 为提前请求准备
            EthernetParse::MPReport_Request_NTPCalib(m_SmoothRTCSetCalibrateValue);  // 提前1分钟请求外网对时
        }
        break;
    }
}
void OS_ServeProcess::OverTimeStartRTC(unsigned short uw16_OverTime)
{
    if(uw16_Counter_RTC_Cali>uw16_OverTime) // 超过3秒保护打开RTC中断  200us/50us*N*3000
    {
        RTCFunc::ReStartRTC();
        uw16_Counter_RTC_Cali = 0;
    }
}