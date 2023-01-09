/*****************************************************************************
// �����ļ�      :OS_ServeProcess.cpp     ��Ҫ���ڲ���ϵͳ���񡢵��Դ���
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.11
*****************************************************************************/

/*
Comment out����ָ��
void  (* CpFun::pFun) (void);    //����һ������ָ��
CpFun::pFun =  & ( FF::v );    //ָ����FF::v�������
(* CpFun::pFun) ();    //����
*/

#include "includes.h"

BYTE VERSION[]	=  "TY2212301";    // IV and AUX Update date

#if  USE_AS_AUXMCU == 0
BYTE IV_DETAIL[] = {"LPC4078 Pro8 TY Build 2212301"}; //Only have 99 chars, please wirte simply.
#else
BYTE AUX_DETAIL[] = {"LPC4078 Pro8 TY Build 2212301"}; //Only have 99 chars, please wirte simply.
#endif
extern REMOTEM rm;

OS_TaskCtrBlock   OS_ServeProcess::OS_Task[TASK_COUNT_MAX];    //������ƿ�

unsigned char     OS_ServeProcess::TaskCount;    //FetchNext��������һ��TaskCount+1 ��˵����û�ҵ�
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
bool              OS_ServeProcess::BuiltIn_Flag=false;        // �Ƿ����BUILTIN ����
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
unsigned long    OS_ServeProcess::uw32_Cycle1s_Count = 0;   // ���ѭ��
unsigned char    OS_ServeProcess::uc_IfAdjust_RTC_Start = 0;
////end////

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

#if DEBUG_TASK_STATISTIC == 1
unsigned long     OS_ServeProcess::PeriodCount;
unsigned long     OS_ServeProcess::PeriodSet;
unsigned long     OS_ServeProcess::Task_Count[MAX_TASK_COUNT+2];
MsTime            OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+2];
#endif

#pragma arm section //��������������


//osinit
void OS_ServeProcess::OS_Init(void)
{
    unsigned char i;

    memcpy(OS_ServeProcess::Version,VERSION,16);    //�汾 //zyx 180305
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
    for(i=0; i<TASK_COUNT_MAX; i++)  //��̬����
    {
        //os init
        OS_Task[i].TaskID = 0;
        OS_Task[i].TaskAddress= NULL;    //����
        OS_Task[i].Priority = 0;
        OS_Task[i].IDFlag = 0;    //����creat�ظ����
        //for �ź���
        //SemTable[i].SemID  = 0 ;
        //SemTable[i].IDFlag = 0 ;

        P8BGSwitch = 0;
    }

    TaskCount = TASK_COUNT_MAX;

    //ȫ����ʼ��Ϊ-1���������ܴ�0��ʼ
    Ptr_TaskP1 = -1;		// current pointer of Task with priority 1, may be invalid when being used.
    Ptr_TaskP2 = -1;		// these pointers are used for convenience, but need to be checked each time before using
    Ptr_TaskP4 = -1;		// and also, these pointers will avoid a task to be run for multiple time when others with the same priorities
    Ptr_TaskP8 = -1;		// are not executed at all or being executed with less times.
    Ptr_TaskPBG = -1;

    Eth_TimeCalied = 0;
    SmallTickCount = 0;
    m_SmoothRTCSetCalibrateValue = 0;
    uw32_Cycle1s_Count = 0;   // ���ѭ��
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

/*����ָ��void fun(void)    ,���ȼ�    ticks    ID
//��0��ʼ,IDҪ��˳��
//1003�޸�*/
void OS_ServeProcess::OSCreateTask( void (*fDoWork)(void),void (*fInit)(void),unsigned char priority,unsigned char TaskID )    //��������
{
    //	static unsigned char prio1ID,prio2ID,prio4ID,prio8ID,prio64ID ;

    if(TaskID < TASK_COUNT_MAX)    //���������������
    {
        if(OS_Task[TaskID].IDFlag == 0)	   //�ظ����⣬ʹ��IDFlag������ظ�����
        {   //  ID1					ID2					ID3					ID4
            //TaskID
            OS_Task[TaskID].TaskID = TaskID;
            OS_Task[TaskID].TaskAddress = (fDoWork);    //����ָ��
            OS_Task[TaskID].TaskInitAddress = fInit;    //�����ʼ��ָ��
            OS_Task[TaskID].Priority = priority;

            OS_Task[TaskID].IDFlag = 1;    //�Ѿ���������
        }
        else    //�����ظ�
        {
            //error
        }
    }
    else    //����ʧ��    ���������������
    {
        //error
    }
}

short  OS_ServeProcess::MountTask(unsigned char TaskID, unsigned char Priority, bool bInit)
{
    if(TaskID >= TASK_COUNT_MAX )
        return -1;
    if(!OS_Task[TaskID].IDFlag)
        return -1;                //TaskID��Ӧ������������ֿ���û���ҵ�
    if(OnlineTaskCount >= MAX_ONLINE_TASK_COUNT)
        return -1;                //�����������Ѿ����ˣ����ɲ��¸���������

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
            {   //ɾ��һ��ָ����������Ҫ���������������ǰ��
                memcpy(&Online_Task[i],&Online_Task[i+1],sizeof(OS_Task_Online));
            }
            m_OnlineTaskIDmap[OnlineTaskID] = MAX_ONLINE_TASK_COUNT+1;   //distroy a map,2014/12/29
            OnlineTaskCount--;
            ENQUEUE(m_qOnlineTaskID,MAX_ONLINE_TASK_COUNT,OnlineTaskID);    //���շ����� ��������ID
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
//ʹ��timer0 ʹticks��ת*/
void OS_ServeProcess::OS_Start(void)
{
    TIMERSFunc::Enable_Timer(0);    //ʹ��Timer0  startOSʱ����ת
    TIMERSFunc::Enable_Timer(2);    //ʹ��Timer0  startOSʱ����ת
#if THREAD_TASK_TIME_STATISTIC == 1
    TIMERSFunc::Enable_Timer(1);		//ʹ��Timer1  startOSʱ����ת
#endif
}

/*��Ҫ
���ȹ��̣�
���ȼ�֮���Զ����Ϊ1��2��4��8��64���Ƕ����Ƶ�ÿһλ��ÿλ����һ�����ȼ�һ��5λ����0���������������
��Ԥ����16��32�Ա���

���ǽ���ProcessMessage��Ϣ�����������ͨѶ��������������

Ȼ������������ȼ�����
(highest_pri & PRIORITY1) != 0    //���������PRIORITY1������
ִ��ProcessPriority1();

�����������ȼ������OS_Manual.docx    by Xu

FetchNextTask����˵��OS_Task[]���������ƿ��У��ҵ��������ȼ������񣬲����ظ�����ID
����ÿ��ִ�лᰴ�յ�ǰ��proi_Ptr����һ��ID��ʼѰ�ң��ҵ���β����0��ʼ��proi_Ptr

����1
ʹ��ifForEach��0��ʼ�鵽��β

����2
���ǰ��ձ��proi_Ptr��һ��IDһ��һ��ȥ�飬�鵽��ף���0��ʼ��proi_Ptr

��������ʱIdһ����Ҫ���ν��е�
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

//	OS_ServeProcess::ProcessMessage();    //����ͨ��

    highest_pri = 0;    //clear
    for(i = 0; i < OnlineTaskCount; i ++)			// Find out the lowest Priority in the Task List.
        highest_pri |= Online_Task[i].Task.Priority;//ˢ�����ȼ�

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

    if(TickFlag)    //���е�ʱ��
    {
        TickFlag = 0;
        BGCount ++;
        if(BGCount >= BGTICKS)    //��������
        {
            BGCount = 0;
            Ptr_TaskPBG = FetchNextTask(PRIORITYBG, 0);//�Ҳ�������TaskCount+1
            if(Ptr_TaskPBG < OnlineTaskCount)
            {
                (* Online_Task[Ptr_TaskPBG].Task.TaskAddress) ();	// call the task DoWork()
            }
        }
    }
}
/*/Task4��ÿticks��Ҫ��һ��
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
        Ptr_TaskP4 = FetchNextTask(PRIORITY4,1);//��0��ʼ
        while(Ptr_TaskP4 < OnlineTaskCount)//��û�ҵ�
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
    if(P8BGSwitch)    //8��64�����л�
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
////��Ϣ����////
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
////end ��Ϣ����////

/*///////
// FetchNextTask(): get the next task which has the required priority in the task list
//    Current Positions provided by Ptr_TaskPxx, and this positon is not included in searching
// Input: myPriority - We are trying to look for a task with priority "myPriority"
//        ifForEach - 0, search from current position, the whole list, with index roll over
//                  - 1, search from current position, till the end of the list.
// Return: the index of searched task. if no task meets the requirements, return a value larger than TaskCount.
////////
//��Ϊ���ȼ�4��������Ҫ��һ���ض���ѯһ�ߣ����������ifForEach
//��0��ʼ�鵽��β
//���û�ҵ� ����TaskCount+1
//���Ի�����������Ҫ�ж�if( (FetchNextTask)���ֵ >TaskCount)*/
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
        ptr_task = Ptr_TaskPBG;    //����
        break;
    default:
        ptr_task = Ptr_TaskPBG;    //����
        break;
    }
    if(ptr_task>=OnlineTaskCount)
        ptr_task = 0;    //clear


    if(ifForEach == 1)//����β�ͽ�����
    {
        unsigned char j;
        for(j = (ptr_task+1); j<OnlineTaskCount; j++)    //�ӵ�ǰ�ĵ���β
        {
            if(Online_Task[j].m_bSuspended)
                continue;
            if(Online_Task[j].Task.Priority == myPriority)
                return j;
        }
        return TaskCount+1;//��û�ҵ�
    }
    else//ifForEachΪ0Ӧ���Ƿ�ת��
    {
        unsigned char ptr_task_next;
        unsigned char i;

        ptr_task_next = ptr_task+1;    //ָ����һ�������    ����ptr��ʼ��Ϊ-1
        while(ptr_task_next < OnlineTaskCount)
        {
            if(Online_Task[ptr_task_next].m_bSuspended)
            {
                ptr_task_next++;
                continue;
            }
            if(Online_Task[ptr_task_next].Task.Priority == myPriority)    //����Ҫ��
                return ptr_task_next;
            ptr_task_next++;
        }
        //��ptr_task_next��ĩβû���ҵ�    ��0ͷ��ʼ��ptr_task_next
        for(i = 0; i< ptr_task_next; i++)
        {
            if(Online_Task[i].m_bSuspended)
                continue;
            if( Online_Task[i].Task.Priority == myPriority)
            {
                return i;
            }
        }
        //Ҳ����˵��ptr_task_nextΪ�磬���Һ���ģ�Ȼ����ǰ���

        //��û�ҵ�
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
            uc_IfAdjust_RTC_Start = 0;  // ���ѭ������
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
    //��ͨ��ϵͳ��ֻ��ϵͳʱ���100us ��2�� ��������������. ��ͨ�Ĳ�������һ��
    //��ͨ����. ��Ҳ������200us�ڻ�15us��������������ֵ�ŵ���������ȥ��
#if (USE_AS_AUXMCU == 0)
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )
    {
        OverTimeStartRTC(15000);  // ����3�뱣����RTC�ж�  200us 3000*5

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
        OverTimeStartRTC(60000);  // ����3�뱣����RTC�ж�  50us 20*3000

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
            if(OS_Time.Us100 < 9999)  // 20130910 ��  �޲�׼ȷʱ��
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

    OverTimeStartRTC(60000);  // ����3�뱣����RTC�ж�  50us 20*3000

    SmallTickCount++;

    if((SmallTickCount & 0x01) == 0)
    {
        if(OS_Time.Us100 < 9999)  // 20130910 ��  �޲�׼ȷʱ��
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

    // 	if((OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
    // 		ChannelInfo_Manager::Set_TimeRecalculateSign();  //�ⲿ���Ƶ�board_config::Func_Sample�����ﴦ��������20131118
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
            Aux_DataManager::SettingCS_Low(Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_CS_Address.m_Value);//����Ƭѡ
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_BoardADC_Trigger(AUX_ADC_Idx,Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_WorkingChn);
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_StateMachine = AUX_Sample_READ;
            break;
        case AUX_Sample_READ:
            Last_chn= Aux_DataManager::findLastChn(AUX_ADC_Idx);
            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].m_BoardADC_Rd(AUX_ADC_Idx,Last_chn);

            Aux_DataManager::SettingCS_High();

            Aux_DataManager::Aux_ADC_CS_Info[AUX_ADC_Idx].counter=0;
            Aux_DataManager::Aux_ADC_TimeOut_Flag &= ~(1<<AUX_ADC_Idx);				//��ȡ���ݺ����־
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
void OS_ServeProcess::RTCHandlerISR(void)// __irq    //��ʱֻ������λtimer0
{
    //��λTimer
    TIMERSFunc::Reset_Timer(0);
    OS_ServeProcess::RTC_SetNewSystemTime();

    if(OS_ServeProcess::uw32_Cycle1s_Count>0)
    {
        RTCFunc::StopRTC();  // Ϊ 0 ʱ��ʱ�������������ֹ����ʱ�ɶ�����г�ʼ����
        OS_ServeProcess::uc_IfAdjust_RTC_Start = 1;
    }
    OS_ServeProcess::uw16_Counter_RTC_Cali = 0;
}

/*/������ռ���������
//Timer Cali
//comment out chen 20130315
//��λ��setTime����У׼*/

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INIT_Aϵͳ�����ʼ��A���֣�������ʼ��*/
void OS_ServeProcess::Init_Device_Firmware(void)
{
    ////////////////////////////////TargetRest////////////////////////////////
    // ��������ʹ����ƵCCLK=120M;PCLK=60M //

    Arbin_SC->RSTCON0.BitReg.RSTGPIO = 1;
    Arbin_SC->RSTCON0.BitReg.RSTGPIO = 0;
    Arbin_SC->RSTCON1.BitReg.RSTIOCON = 1;
    Arbin_SC->RSTCON1.BitReg.RSTIOCON = 0;
    for(unsigned long uw32_i = 0; uw32_i < DEV_WAITTIME; uw32_i ++);	//�ȴ�����������ʼ�����



    ///////////////////////////////IRQInit///////////////////////////////
    IRQFunc::init_VIC();
    //����I2C����EEPROM

    ////////////////////////////////Timer////////////////////////////////
    //20150324,hepeiqing:
    //����IVͨ����ֻ��BT11��ʹ��200US TICK��
    //����AUXͨ����ֻ�и߹�ģ���ε�ѹ�����200US TICK��
    //�����κ��������ʹ��50US TICK��
    //#if (((_BOARD_TYPE_  == BOARDTYPE_BT11 ) && (USE_AS_AUXMCU == 0)) || ((USE_AS_AUXMCU == 1) && (AUX_2ndV_FOR_HRNEW == 1)))
#if (((_BOARD_TYPE_  == BOARDTYPE_BT11 )||(_BOARD_TYPE_  == BOARDTYPE_HS21044 )) && (USE_AS_AUXMCU == 0))
    TIMERSFunc::Init_Timer(TIME_INTERVAL_200US,0);     //Timer0 init    //200us �ж�һ��,��Ϊ��ͨ���������ж���������
#else
    TIMERSFunc::Init_Timer(TIME_INTERVAL_50US,0);     //Timer0 init    //50us �ж�һ��
#endif
    //  NVIC_SetPriority(TIMER0_IRQn,0);    //����Ϊ0���ȼ�  chen 20131226

    //TIMERSFunc::Init_Timer(1000*TIME_INTERVAL_0 ,2);    //Timer2 init    //100ms �ж�һ��
    TIMERSFunc::Init_Timer(TIME_INTERVAL_T2,2);     //Timer2 init    //10ms �ж�һ��  // ����ֵ: 10ms, 20ms, 25ms, 40ms, 50ms, 100ms, �� 200 ms��

    ////////////////////////////////GPIO////////////////////////////////
    for(unsigned long uw32_i = 6000000; uw32_i >0; uw32_i --);	//delay

    ////////////////////////////////SDRAM////////////////////////////////
    SDRAMFunc::SDRAM_Init();//���

#if (USE_AS_AUXMCU == 0)
//#if (_BOARD_TYPE_  == BOARDTYPE_IBT31162)   // ����AD8450оƬ����    20161018
//     board_config::Func_Avoid_Hot = &IBT31162_Base::Avoid_Hot;
//     if( board_config::Func_Avoid_Hot != 0)
//         ( *board_config::Func_Avoid_Hot)();

//#endif
#endif

    for(unsigned long uw32_i = 0; uw32_i < 6000000; uw32_i ++);	//�ȴ�����������ʼ�����

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
    OS_ServeProcess::OS_Start();    //����Timer0

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
    // "ͬ���ŵ�":����
    TcpRxTx::socket_tcp = tcp_get_socket ((TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE), 0, 10, TcpRxTx::tcp_callback);
    if (TcpRxTx::socket_tcp != 0)
        tcp_listen (TcpRxTx::socket_tcp, PORT_NUM);
    // "�첽�ŵ�":����
    TcpRxTx::socket_tcp2 = tcp_get_socket ((TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE), 0, 10, TcpRxTx::tcp_callback2);
    if (TcpRxTx::socket_tcp2 != 0)
        tcp_listen (TcpRxTx::socket_tcp2, PORT_NUM2);
    memcpy(rm.IpAdr, uc_remotemIpAdr, IP_ADRLEN);
    arp_get_info (& rm);

    UdpRxTx::Init_McuUdp();
// 	CommonFunc::GetTimeTicksUpdate( &EthernetParse::m_Interval_NetLinkTime); // �����������ӳɹ�ʱ������
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
    m_SmoothRTCtime = CommonFunc::GetSystemTime();  // ��ʼʱ������1Сʱ��ŵ�
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
        m_SmoothRTCtimeErrorInms[m_SmoothRTCtimeCounter] = CommonFunc::TimeDiffInMS(SetSystemTime);   // MCUʱ��-PCʱ��ȡ����ֵ
        m_SmoothRTCtimeCounter++;
        if(m_SmoothRTCtimeCounter >= 12)
            m_SmoothRTCSM = ESMOOTHRTCSM_CALCULATION;
        break;
    case ESMOOTHRTCSM_CALCULATION:
        m_SmoothRTCSM = ESMOOTHRTCSM_IDLE;
        m_SmoothRTCtime = CommonFunc::GetSystemTime();
        // __int64�����ֵ��9223372036854775807
        // __int64����Сֵ��-9223372036854775808
        // unsigned __int64�����ֵ��18446744073709551615
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
        if(errorAvg > 200)  // ���200ms һ��RTC����1���Ӳ�1���룬1Сʱ����һ��
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
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 10010/2;  // �Ӻ�1���� 200us*10010/2=1001000;
                else
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 10010*2;  // �Ӻ�1���� 50us*10010*2=1001000;
#else
                OS_ServeProcess::uw16_Limit_RTC_Cali = 10010*2;  // �Ӻ�1���� 50us*10010*2=1001000;
#endif
            }
            else
            {
#if (USE_AS_AUXMCU == 0)
                if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 9990/2;	// ��ǰ1����  200us*9990/2=999000;
                else
                    OS_ServeProcess::uw16_Limit_RTC_Cali = 9990*2;	    // ��ǰ1����  50us*9990*2=999000;
#else
                OS_ServeProcess::uw16_Limit_RTC_Cali = 9990*2;	// ��ǰ1���� 50us*9990*2=999000;
#endif
            }
        }
        m_SmoothRTCSetCalibrateValue = errorAvg;
        OS_ServeProcess::uc_IfAdjust_RTC_Start = 0;
        OS_ServeProcess::uw32_Cycle1s_Count = abs(errorAvg);
        EthernetParse::m_IfRequest_NTP = 1;   // Ϊ��ǰ����׼��
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
            EthernetParse::m_IfRequest_NTP = 0;   // Ϊ��ǰ����׼��
            EthernetParse::MPReport_Request_NTPCalib(m_SmoothRTCSetCalibrateValue);  // ��ǰ1��������������ʱ
        }
        break;
    }
}
void OS_ServeProcess::OverTimeStartRTC(unsigned short uw16_OverTime)
{
    if(uw16_Counter_RTC_Cali>uw16_OverTime) // ����3�뱣����RTC�ж�  200us/50us*N*3000
    {
        RTCFunc::ReStartRTC();
        uw16_Counter_RTC_Cali = 0;
    }
}