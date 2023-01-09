#include "includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
struct CQueueConcept CQ::m_Queues[MAX_QUEUES_MANAGED];

#pragma arm section //结束外存变量定义

int CQ::m_ManagedQueues = 0;

uint16 CQ::Register(void * QueueStartAddress, int ItemSize, int QueueSizeInItem)
{
    if (m_ManagedQueues >= MAX_QUEUES_MANAGED)
        return 0;
    m_Queues[m_ManagedQueues].m_HandleID = m_ManagedQueues+1;
    m_Queues[m_ManagedQueues].m_QueueStartAddress = QueueStartAddress;
    m_Queues[m_ManagedQueues].m_ItemSize = ItemSize;
    m_Queues[m_ManagedQueues].m_QueueSizeInItem = QueueSizeInItem;
    m_Queues[m_ManagedQueues].m_Start = 0;
    m_Queues[m_ManagedQueues].m_Length = 0;
    m_ManagedQueues ++;
    return m_ManagedQueues;
}
bool CQ::ResetQueueSize(uint16 handle,int QueueSizeInItem)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return 0;

    m_Queues[handle-1].m_QueueSizeInItem = QueueSizeInItem;
    m_Queues[handle-1].m_Start = 0;
    m_Queues[handle-1].m_Length = 0;
    return 1;
}
/*
void * CQ::GetQueueAddress(uint16 handle)
{
if((handle<1)||(handle>m_ManagedQueues+1))
return 0;

return m_Queues[handle-1].m_QueueStartAddress;
}

int CQ::GetItemSizeOfQueue(uint16 handle)
{
if((handle<1)||(handle>m_ManagedQueues+1))
return 0;

return m_Queues[handle-1].m_ItemSize;
}
*/
//int CQ::GetQueueSizeInItem(uint16 handle)
//{
//	if((handle<1)||(handle>m_ManagedQueues+1))
//		return 0;
//
//	return m_Queues[handle-1].m_QueueSizeInItem;
//}

void CQ::InitQueue(uint16 handle)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return;

    m_Queues[handle-1].m_Start = 0;
    m_Queues[handle-1].m_Length = 0;
}

bool CQ::IsQueueEmpty(uint16 handle)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return false;
    return (m_Queues[handle-1].m_Length == 0);
}

bool CQ::IsQueueFull(uint16 handle)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return false;
    return (m_Queues[handle-1].m_Length >= m_Queues[handle-1].m_QueueSizeInItem);
}

int CQ::GetItemNumber(uint16 handle)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return -1;
    return m_Queues[handle-1].m_Length;
}

//int CQ::GetEmptySpaceNumber(uint16 handle)
//{
//	if((handle<1)||(handle>m_ManagedQueues+1))
//		return -1;
//	return m_Queues[handle-1].m_QueueSizeInItem - m_Queues[handle-1].m_Length;
//}

int CQ::Enqueue(uint16 handle, const void * pSourceItem)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return -1;

    struct CQueueConcept *st_Que = &m_Queues[handle-1];
    bool bFull = IsQueueFull(handle);
    if(bFull)
    {
        st_Que->m_Length --;
        st_Que->m_Start ++;
        while(st_Que->m_Start >= st_Que->m_QueueSizeInItem)
            st_Que->m_Start -= st_Que->m_QueueSizeInItem;
    }

    uint32 PositionOfLastItem = st_Que->m_Start + st_Que->m_Length;
    if(PositionOfLastItem >= st_Que->m_QueueSizeInItem)
        PositionOfLastItem -= st_Que->m_QueueSizeInItem;
    //TEST_H;
    memcpy((byte*)(st_Que->m_QueueStartAddress)+PositionOfLastItem*st_Que->m_ItemSize, (byte*)(pSourceItem), st_Que->m_ItemSize);
    st_Que->m_Length++;
    //TEST_L;
    return 1;

}

int CQ::Dequeue(uint16 handle, void * pTargetItem)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return -1;
    struct CQueueConcept *st_Que = &m_Queues[handle-1];
    bool bEmpty = IsQueueEmpty(handle);
    if(bEmpty)
        return -1;

    memcpy((byte*)pTargetItem, (byte*)st_Que->m_QueueStartAddress+st_Que->m_Start*st_Que->m_ItemSize, st_Que->m_ItemSize);

    st_Que->m_Length --;
    st_Que->m_Start ++;
    while(st_Que->m_Start >= st_Que->m_QueueSizeInItem)
        st_Que->m_Start -= st_Que->m_QueueSizeInItem;

    return 1;
}
// #if (USE_AS_AUXMCU == 1)
int CQ::Peekqueue(uint16 handle, void * pTargetItem)
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return -1;
    struct CQueueConcept *st_Que = &m_Queues[handle-1];
    bool bEmpty = IsQueueEmpty(handle);
    if(bEmpty)
        return -1;

    memcpy((byte*)pTargetItem, (byte*)st_Que->m_QueueStartAddress+st_Que->m_Start*st_Que->m_ItemSize, st_Que->m_ItemSize);

    return 1;
}
// #endif
/*
int CQ::EnqueueArray(uint16 handle, int ArraySize, const void * pSourceArrayAddress)
{
	if((handle<1)||(handle>m_ManagedQueues+1))
		return -1;
	if(ArraySize>GetQueueSizeInItem(handle))
		return -1;
	struct CQueueConcept *st_Que = &m_Queues[handle-1];
	int iEmptySpace = GetEmptySpaceNumber(handle);
	if(ArraySize>iEmptySpace)
	{
		st_Que->m_Length -= (ArraySize-iEmptySpace);
		st_Que->m_Start += (ArraySize-iEmptySpace);
		while(st_Que->m_Start >= st_Que->m_QueueSizeInItem)
			st_Que->m_Start -= st_Que->m_QueueSizeInItem;
	}

	uint32 PositionOfLastItem = st_Que->m_Start + st_Que->m_Length;
	if(PositionOfLastItem >= st_Que->m_QueueSizeInItem)
		PositionOfLastItem -= st_Que->m_QueueSizeInItem;
	int SpaceToPhysicalEnd = st_Que->m_QueueSizeInItem - PositionOfLastItem;
	if(ArraySize>SpaceToPhysicalEnd)
	{
		memcpy((byte*)st_Que->m_QueueStartAddress+PositionOfLastItem*st_Que->m_ItemSize, (byte*)pSourceArrayAddress, SpaceToPhysicalEnd*st_Que->m_ItemSize);
		memcpy((byte*)st_Que->m_QueueStartAddress, (byte*)pSourceArrayAddress+SpaceToPhysicalEnd*st_Que->m_ItemSize, (ArraySize-SpaceToPhysicalEnd)*st_Que->m_ItemSize);
	}
	else
		memcpy((byte*)st_Que->m_QueueStartAddress+PositionOfLastItem*st_Que->m_ItemSize, pSourceArrayAddress, ArraySize*st_Que->m_ItemSize);

	st_Que->m_Length += ArraySize;
	return ArraySize;
}

int CQ::DequeueToArray(uint16 handle, int ArraySize, void * pTargetArrayAddress)
{
	if((handle<1)||(handle>m_ManagedQueues+1))
		return -1;
	if(ArraySize>GetQueueSizeInItem(handle))
		return -1;
	if(IsQueueEmpty(handle))
		return -1;
	struct CQueueConcept *st_Que = &m_Queues[handle-1];
	int iItemsInQue = GetItemNumber(handle);
	if(ArraySize>iItemsInQue)
		ArraySize = iItemsInQue;

	int StartToPhysicalEnd = st_Que->m_QueueSizeInItem - st_Que->m_Start;
	if(ArraySize>StartToPhysicalEnd)
	{
		memcpy((byte*)pTargetArrayAddress, (byte*)st_Que->m_QueueStartAddress+st_Que->m_Start*st_Que->m_ItemSize, StartToPhysicalEnd*st_Que->m_ItemSize);
		memcpy((byte*)pTargetArrayAddress+StartToPhysicalEnd*st_Que->m_ItemSize, (byte*)st_Que->m_QueueStartAddress, (ArraySize-StartToPhysicalEnd)*st_Que->m_ItemSize);
	}
	else
		memcpy((byte*)pTargetArrayAddress, (byte*)st_Que->m_QueueStartAddress+st_Que->m_Start*st_Que->m_ItemSize, ArraySize*st_Que->m_ItemSize);

	st_Que->m_Length -= ArraySize;
	st_Que->m_Start += ArraySize;
	while(st_Que->m_Start >= st_Que->m_QueueSizeInItem)
		st_Que->m_Start -= st_Que->m_QueueSizeInItem;

	return ArraySize;
}
*/
void CQ::BulkDEQUEUE(QueueP  CirQueue,void * Dest,unsigned long LengthInBytes)	// Make sure CirQueue exists and the length is count by Byte
{
    unsigned long  W_L = *CirQueue.Start + LengthInBytes;
    unsigned char * Q = (unsigned char *) CirQueue.Object_0;
    Q = Q + *CirQueue.Start;
    if(W_L < CirQueue.BufferDepth)      //  队列未满
        memcpy(Dest,Q,LengthInBytes);
    else
    {   //  队列已满，再接头部操作
        unsigned long  uw32_L = CirQueue.BufferDepth-*CirQueue.Start;
        memcpy(Dest,Q,uw32_L);
        unsigned char * D = (unsigned char *) Dest;     // 转换字节类型操作，否则编译器提示错误。
        D =  D + uw32_L;
        memcpy(D,CirQueue.Object_0,LengthInBytes-uw32_L);
    }
    *CirQueue.Length -=LengthInBytes;
    *CirQueue.Start = W_L%CirQueue.BufferDepth;
}

void CQ::BulkENQUEUE(QueueP  CirQueue,void * Dest,unsigned long LengthInByte)		// Make sure CirQueue exists and the length is count by Byte
{
    unsigned long  W_L = (*CirQueue.Start + *CirQueue.Length)%CirQueue.BufferDepth;   // 取数地址加实存数得到空地区，以便存入数据
    unsigned char * Q = (unsigned char *) CirQueue.Object_0;
    Q = Q + W_L;
    if((W_L + LengthInByte) < CirQueue.BufferDepth)   //  队列未满
        memcpy(Q,Dest,LengthInByte);
    else
    {   //  队列已满，再接头部操作
        unsigned long  uw32_L = CirQueue.BufferDepth-W_L;
        memcpy(Q,Dest,uw32_L);
        unsigned char * D = (unsigned char *) Dest;     // 转换字节类型操作，否则编译器提示错误。
        D =  D + uw32_L;
        memcpy(CirQueue.Object_0,D,LengthInByte-uw32_L);
    }
    *CirQueue.Length +=LengthInByte;
}

#if (USE_AS_AUXMCU == 0)
// For HPT200 Dynamic Filter
void CQ::ModifyQueueDepth(uint16 handle, unsigned int depth, unsigned long *sum, unsigned char queue_type)	// Yifei, 2016.02.26 //将depth由unsigned char改为unsigned int，否则m_uFilterDepth_I为255+1时会超出范围 zyx 170721
{
    if((handle<1)||(handle>m_ManagedQueues+1))
        return;
    if(depth == m_Queues[handle-1].m_QueueSizeInItem)
        return;
    if(depth > m_Queues[handle-1].m_Length && m_Queues[handle-1].m_Start==0)
    {
        m_Queues[handle-1].m_QueueSizeInItem = depth;
        return;
    }
    if(queue_type == VOL_QUEUE)
    {
        if(depth > (ChannelInfo_Manager::BoardCFG.m_Global.m_uFilterDepth_V+1))	// if intended depth > customer preset depth, do nothing
            return;
    }
    else	// CUR_QUEUE
    {
        if(depth > (ChannelInfo_Manager::BoardCFG.m_Global.m_uFilterDepth_I+1))	// if intended depth > customer preset depth, do nothing
            return;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Example1: (16 -> 8) copy the last 8 items to the queue front, and set start position at the beginning
//
//	Queue {0,	1,	2,	3(Last item),	4(Start),	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15}	m_QueueSizeInItem = 16
//
//																		CHANGE TO
//
//				{12(Start),	13,	14,	15,	0,	1,	2,	3(Last item),	-,	-,	-,	-,	-,	-,	-,	-}	m_QueueSizeInItem = 8
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Example2: (8 -> 16) rearrange the first 8 items, and set start position at the beginning
//
//	Queue {0,	1,	2,	3(Last item),	4(Start),	5,	6,	7,	-,	-,	-,	-,	-,	-,	-,	-}	m_QueueSizeInItem = 8
//
//																		CHANGE TO
//
//				{4(Start),	5,	6,	7,	0,	1,	2,	3(Last item),	-,	-,	-,	-,	-,	-,	-,	-}	m_QueueSizeInItem = 16
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    int copynumber = 0;
    struct CQueueConcept *st_Que = &m_Queues[handle-1];
    unsigned long temp_queue[8] = {0,0,0,0,0,0,0,0};

    if(depth < m_Queues[handle-1].m_Length)	// if intended depth < current depth, decrease queue size, see example 1
    {
        unsigned long* tempsum = sum;
        *tempsum = 0;
        copynumber = depth;

        //first copy data to a third queue in case they overwrite other data
        for(int i=0; i<copynumber; i++)
        {
            memcpy(	(byte*)(temp_queue)+i*st_Que->m_ItemSize,\
                    (byte*)(st_Que->m_QueueStartAddress)+((st_Que->m_Start+st_Que->m_Length-depth+i)%m_Queues[handle-1].m_Length)*st_Que->m_ItemSize,\
                    st_Que->m_ItemSize\
                  );
        }

        for(int i=0; i<copynumber; i++)
        {
            memcpy(	(byte*)(st_Que->m_QueueStartAddress)+i*st_Que->m_ItemSize,\
                    &temp_queue[i],\
                    st_Que->m_ItemSize\
                  );
            unsigned long a = *tempsum;
            unsigned long b = temp_queue[i];//*(unsigned long*)((byte*)st_Que->m_QueueStartAddress+i*st_Que->m_ItemSize);
            *tempsum = a+b;
        }
        st_Que->m_Start = 0;
        st_Que->m_QueueSizeInItem = depth;
        st_Que->m_Length = depth;
        return;
    }
    else if(depth > m_Queues[handle-1].m_Length)	// if intended depth >= current depth, increase queue size, see example 2
    {
        copynumber = m_Queues[handle-1].m_Length;

        //first copy data to a third queue in case they overwrite other data
        for(int i=0; i<copynumber; i++)
        {
            int step = (st_Que->m_Start+i)%m_Queues[handle-1].m_Length;

            memcpy(	(byte*)(temp_queue)+i*st_Que->m_ItemSize,\
                    (byte*)(st_Que->m_QueueStartAddress)+ step*st_Que->m_ItemSize,\
                    st_Que->m_ItemSize\
                  );
        }

        for(int i=0; i<copynumber; i++)
        {
            memcpy(	(byte*)(st_Que->m_QueueStartAddress)+i*st_Que->m_ItemSize,\
                    &temp_queue[i],\
                    st_Que->m_ItemSize\
                  );
        }
        st_Que->m_Start = 0;
        st_Que->m_QueueSizeInItem = depth;
    }
}
#endif




