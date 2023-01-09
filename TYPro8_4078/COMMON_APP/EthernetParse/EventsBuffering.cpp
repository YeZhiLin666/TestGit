/*****************************************************************************
// �����ļ�                   :EventsBuffering.cpp     ��Ҫ�����¼�������ط�
// �ļ�����                   :Ϊ MCU board����
// ��д����(MM.DD.YYYY)       :08.05.2013
*****************************************************************************/

#include "../Entry/includes.h"

UInt16 EventsBuffering::Priority = PRIORITY4;  //  PRIORITYBG
UInt16 EventsBuffering::TaskID = EVENTSBUFFERING_TASK;

// ���ñ������������
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

MsTime              EventsBuffering::MyCounter;
EventsBuffer        EventsBuffering::EV_Buf[2][EVENTSBUFFERSIZE];
int                 EventsBuffering::Buf_Active; // ��Ծ��ר���ڴ洢���͵��¼�
//int					EventsBuffering::Buf_Inactive;  // �ǻ�Ծ��ר�����ط���

int                 EventsBuffering::InUse_Idx; // ��Ծ��ָ���λ��

UWord16            EventsBuffering::uw16_Events_Total[2];
UWord16            EventsBuffering::uw16_Map_Point[2];
EventsBuffer_Map   EventsBuffering::EV_Map[2][EVENTSBUFFERSIZE];
#pragma arm section //��������������

void EventsBuffering::MyInit(void)
{
    int i;
    CommonFunc::GetTimeTicksUpdate(&MyCounter);
    Buf_Active = 0;
    for (i = 0; i < EVENTSBUFFERSIZE; i ++)
    {   // ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط���
        EV_Buf[0][i].EventNo = 0;
        EV_Buf[1][i].EventNo = 0;
    }
    InUse_Idx = 0;
    EVBufferMap_Clear(0,false);
    EVBufferMap_Clear(1,false);
}

void EventsBuffering::DoWork(void)
{
    int i, buf_Inactive2, empty_Idx;

#if ((DEBUG_TASK_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
    OS_ServeProcess::Task_Count[COUNT_EVENTS_BUFFER] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if ((THREAD_TASK_TIME_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
	Time_Count::thread_time_init(COUNT_EVENTS_BUFFER);
#endif

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH ״̬���ı��� ����������������
    {
        ScheduleStatusReport::uc_uc_BreakNet_Counter = 2*BREAKNET_NUMBER;   // yy 20141030  Fix buffer overflow problem bug.
#if ((THREAD_TASK_TIME_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
		Time_Count::thread_time(COUNT_EVENTS_BUFFER);
		#endif
        return;
    }
#endif
    if(CommonFunc::GetTimeTicks( &MyCounter) < 50000)	// perform only every 5 second
    {
#if ((THREAD_TASK_TIME_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
		Time_Count::thread_time(COUNT_EVENTS_BUFFER);
		#endif
        return;
    }

    CommonFunc::GetTimeTicksUpdate(&MyCounter);	 // reset counter
    // ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط���
    buf_Inactive2 = !Buf_Active;							// init variables for following operation
    //Buf_Inactive = buf_Inactive2;		// ��Ծ���л����ǻ�Ծ��
    empty_Idx = InUse_Idx; // ��Ծ��ָ���λ��

    /*	for (i = 0; i < EVENTSBUFFERSIZE; i ++)
    	{
    		if (EV_Buf[buf_Inactive2][i].EventNo != 0)		// find the next event in Inactive buffer that is not confirmed
    		{
    			// !!!!!!!THIS IS PSEUDO CODE
    			//Tcp_Buffer::BufferEvents(EV_Buf[buf_Inactive][i]);	// Try to put this in TCP buffer to resend it anyway (in Xiaomin's code)

    			EthernetParse::PushToSendingEventBuf(&EV_Buf[buf_Inactive2][i].EventContent[0], EV_Buf[buf_Inactive2][i].EventLen); // Add by DKQ on 08.05.2013, Try to put this in TCP buffer to resend it anyway

    			// <<<END>>> !!!!!!!THIS IS PSEUDO CODE
    			empty_Idx = FindNextEmpty(empty_Idx);				// find the next empty slot in active buffer

    			if (empty_Idx != -1)								// if there's no empty space, dump this event.
    			{
    				MoveEvent2Active(i, empty_Idx);					// move the event from inactive buffer to Active buffer empty slot.
    				//EV_Buf[buf_Inactive2][i].EventNo = 0;		    // the event in inactive buffer is erased.
    			}
    			else
    			{
    				EV_Buf[buf_Inactive2][i].EventNo = 0;		    // the event in inactive buffer is erased.
    			}
    		}
    	}*/
    if(uw16_Events_Total[buf_Inactive2] <= 0)
	{
#if ((THREAD_TASK_TIME_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
		Time_Count::thread_time(COUNT_EVENTS_BUFFER);
		#endif
        return;
	}
    for (i = 0; i < uw16_Events_Total[buf_Inactive2]; i ++)   // ɨ��ǻ�Ծ���������
    {
        if (EV_Map[buf_Inactive2][i].bValid)		// find the next event in Inactive buffer that is not confirmed
        {
            // !!!!!!!THIS IS PSEUDO CODE
            //Tcp_Buffer::BufferEvents(EV_Buf[buf_Inactive][i]);	// Try to put this in TCP buffer to resend it anyway (in Xiaomin's code)
            UWord16 uw16_P = EV_Map[buf_Inactive2][i].uw16_Array;
            EthernetParse::PushToSendingEventBuf(&EV_Buf[buf_Inactive2][uw16_P].EventContent[0], EV_Buf[buf_Inactive2][uw16_P].EventLen); // Add by DKQ on 08.05.2013, Try to put this in TCP buffer to resend it anyway

            // <<<END>>> !!!!!!!THIS IS PSEUDO CODE
            empty_Idx = FindNextEmpty(empty_Idx);				// find the next empty slot in active buffer

            if (empty_Idx != -1)								// if there's no empty space, dump this event.
            {
                MoveEvent2Active(uw16_P, empty_Idx);					// move the event from inactive buffer to Active buffer empty slot.
                //EV_Buf[buf_Inactive2][i].EventNo = 0;		    // the event in inactive buffer is erased.
            }
            else
            {
                EV_Buf[buf_Inactive2][uw16_P].EventNo = 0;		    // the event in inactive buffer is erased.
            }
        }
    }

    Buf_Active = !Buf_Active;									// Swap the buffers   	// ��Ծ���л����ǻ�Ծ��
    InUse_Idx = 0;												// reset index pointer
    EVBufferMap_Clear(Buf_Active,true);  // ��Ծ������Ϊ�ա�

#if ((DEBUG_TASK_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
    OS_ServeProcess::Task_Count_Time[COUNT_EVENTS_BUFFER] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if ((THREAD_TASK_TIME_STATISTIC == 1) && (USE_AS_AUXMCU ==0))
	Time_Count::thread_time(COUNT_EVENTS_BUFFER);
#endif
}

int EventsBuffering::ConfirmEvent(uint32 eventNo)
{
    int i,j,buf_Inactive2;
    /*	for (i = 0; i < EVENTSBUFFERSIZE; i ++)
    	{    // ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط��� ͬʱ��ȷ�ϵ��¼���
    		if(EV_Buf[0][i].EventNo == eventNo)
    		{
    			EV_Buf[0][i].EventNo = 0;
    			return 1;
    		}

    		if(EV_Buf[1][i].EventNo == eventNo)
    		{
    			EV_Buf[1][i].EventNo = 0;
    			return 1;
    		}
    	}*/
    buf_Inactive2 = Buf_Active;
    for(j=0; j<2; j++)
    {
        i = EVBufferMap_FindEventNo(buf_Inactive2,eventNo); // ɨ���Ծ���ͷǻ�Ծ��
        if(i>=0)
        {
            EVBufferMap_MoveEvent(buf_Inactive2,uw16_Map_Point[buf_Inactive2]);
            EV_Buf[buf_Inactive2][i].EventNo = 0;
            return 1;
        }
        buf_Inactive2 = !Buf_Active;  // ɨ���Ծ���ͷǻ�Ծ��
    }
    return -1;
}

int EventsBuffering::PushingEvents(uint8 * content, int eventLen, uint32 eventNo)
{
    int i;
    if (InUse_Idx >= EVENTSBUFFERSIZE)
        return -1;
    if(eventLen > EVENTLENGTH)
        return -1;

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH ״̬���ı��� ����������������
    {
        ScheduleStatusReport::uc_uc_BreakNet_Counter = 2*BREAKNET_NUMBER;  // yy 20141030  Fix buffer overflow problem bug.
        return;
    }
#endif

    EV_Buf[Buf_Active][InUse_Idx].EventNo = eventNo;    // ��Ծ��ר���ڴ洢���͵��¼��������¼�
    EV_Buf[Buf_Active][InUse_Idx].EventLen = eventLen;
    EVBufferMap_Insert(Buf_Active,InUse_Idx,eventNo);
    for (i = 0; i < eventLen; i ++)
    {
        EV_Buf[Buf_Active][InUse_Idx].EventContent[i] = content[i];
    }

    InUse_Idx ++;  // ��Ծ��ר���ڴ洢���͵��¼����ƶ�ָ�뵽��λ�á�
    return 0;
}

void EventsBuffering::MoveEvent2Active(int source, int destination)
{
    // ��Ծ��ר���ڴ洢���͵��¼���ת�Ƶ��ǻ�Ծ��ר�����ط���
    //memcpy( & EV_Buf[Buf_Active][destination],& EV_Buf[Buf_Inactive][source],sizeof(EventsBuffer));
    memcpy( & EV_Buf[Buf_Active][destination],& EV_Buf[!Buf_Active][source],sizeof(EventsBuffer));
    EventsBuffering::EVBufferMap_Insert(Buf_Active,destination,EV_Buf[Buf_Active][destination].EventNo);
    EV_Buf[!Buf_Active][source].EventNo = 0;
}

int EventsBuffering::FindNextEmpty(int startPoint)
{
    int i;

    if (startPoint == -1)										// if already checked one full round in Active buffer and no empty left.
        return startPoint;

    if (startPoint >= InUse_Idx)								// if the buffer is not used up yet, look into unused slots.
    {
        for(i = startPoint; i < EVENTSBUFFERSIZE; i ++)
        {
            if(EV_Buf[Buf_Active][i].EventNo == 0)  // ��Ծ��ר���ڴ洢���͵��¼����ҵ�û��Ŀ���λ�á�
                return i;
        }
    }

    for (i = 0; i < InUse_Idx; i ++)							// if buffer used up, search confirmed slot which is available
    {
        if(EV_Buf[Buf_Active][i].EventNo == 0)
            return i;
    }

    return -1;
}

int EventsBuffering::EVBufferMap_FindEventNo(unsigned char uc_back,unsigned short int eventNo)
{
    for (unsigned short int i = 0; i < uw16_Events_Total[uc_back]; i ++)
    {
        if(EV_Map[uc_back][i].bValid
                && EV_Map[uc_back][i].EventNo == eventNo)
        {
            uw16_Map_Point[uc_back] = i; // �ҵ�ָ�����¼��ţ����¹������λ����ʵ�ʴ洢λ��(ʵ��λ��)
            return EV_Map[uc_back][i].uw16_Array;
        }
    }
    return -1;
}

void EventsBuffering::EVBufferMap_MoveEvent(unsigned char uc_back,unsigned short int uw16_Point)
{
    if(uw16_Events_Total[uc_back] < 1)
        return;
    if(uw16_Events_Total[uc_back] > 1)
    {
        if(EV_Map[uc_back][uw16_Events_Total[uc_back]-1].bValid)
            EV_Map[uc_back][uw16_Point] = EV_Map[uc_back][uw16_Events_Total[uc_back]-1]; // �ƶ�������е�λ�ã�����β���Ƶ��մ���ʵ��λ�ò��䡣
    }
    uw16_Events_Total[uc_back]--;
    EV_Map[uc_back][uw16_Events_Total[uc_back]].bValid = 0;  // ������������βλ�ã�ʵ��λ�ò��䡣
    EV_Map[uc_back][uw16_Events_Total[uc_back]].uw16_Array = 0;
    EV_Map[uc_back][uw16_Events_Total[uc_back]].EventNo = 0;

}

void EventsBuffering::EVBufferMap_Clear(unsigned char uc_back,unsigned char uc_valid)
{
    UWord16 uw16_Max = EVENTSBUFFERSIZE;
    if(uc_valid)
        uw16_Max = uw16_Events_Total[uc_back];
    uw16_Events_Total[uc_back] = 0;
    uw16_Map_Point[uc_back] = 0;
    for (int i = 0; i < uw16_Max; i ++)
    {
// 			EV_Map[uc_back][i].bValid = 0;
// 		  EV_Map[uc_back][i].uw16_Array = 0;
// 		  EV_Map[uc_back][i].EventNo = 0;
        memset(&EV_Map[uc_back][i],0,sizeof(EventsBuffer_Map));
    }
}
void EventsBuffering::EVBufferMap_Insert(unsigned char uc_back,unsigned short int Index,unsigned short int eventNo)
{
    if(eventNo == 0)
        return;
    if(uw16_Events_Total[uc_back]>=EVENTSBUFFERSIZE)
        return ;
    EV_Map[uc_back][uw16_Events_Total[uc_back]].bValid = 1;  // �����¼��ŵ����������β����ʵ��λ��˳���š�
    EV_Map[uc_back][uw16_Events_Total[uc_back]].uw16_Array = Index;
    EV_Map[uc_back][uw16_Events_Total[uc_back]].EventNo = eventNo;
    if(++uw16_Events_Total[uc_back]>=EVENTSBUFFERSIZE)
        uw16_Events_Total[uc_back] = EVENTSBUFFERSIZE-1;
}
