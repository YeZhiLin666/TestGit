/*****************************************************************************
// 程序文件      :CircularQueue.cpp     主要用于各种自定义循环队列定义、维护。
// 文件描述      :为MCU board服务
// 编写日期      :2012.12.11
*****************************************************************************/

#ifndef _CircularQueue_
#define _CircularQueue_

#define CIRCULAR_QUEUE(ObjType,BufferDepth)                                                  \
struct                                                                                       \
{                                                                                            \
	ObjType        Object[BufferDepth];                                                      \
	unsigned long int m_u32Start;                                                           \
	unsigned long int m_u32Length;                                                          \
}

#define INIT_QUEUE(CirQueue)                                                                 \
{                                                                                            \
	CirQueue.m_u32Start  = 0;                                                                \
	CirQueue.m_u32Length = 0;                                                                \
}

#define IS_QUEUE_EMPTY(CirQueue)              (CirQueue.m_u32Length == 0)

#define IS_QUEUE_FULL(CirQueue,QUEUE_DEPTH)    (CirQueue.m_u32Length == QUEUE_DEPTH)

#define QUEUE_DEPTH(CirQueue)                (CirQueue.m_u32Length)

#define ENQUEUE(CirQueue,BufferDepth,m_Object)                                               \
{                                                                                            \
	unsigned char bFull;                                                                     \
	bFull = IS_QUEUE_FULL(CirQueue,BufferDepth);                                             \
	if(bFull)                                                                                \
	{                                                                                        \
	CirQueue.m_u32Length--;                                                              \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + 1)%BufferDepth;                         \
	}                                                                                        \
	\
	CirQueue.Object[(CirQueue.m_u32Start + CirQueue.m_u32Length)%BufferDepth] = m_Object;    \
	CirQueue.m_u32Length ++;                                                                 \
}

#define ENQUEUE2(CirQueue,BufferDepth,m_Object,m_ObjectDe,bFull)                             \
{                                                                                            \
	bFull = IS_QUEUE_FULL(CirQueue,BufferDepth);                                             \
	if(bFull)                                                                                \
	{                                                                                        \
	CirQueue.m_u32Length--;                                                              \
	m_ObjectDe = CirQueue.Object[CirQueue.m_u32Start];                                   \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + 1)%BufferDepth;                         \
	}                                                                                        \
	\
	CirQueue.Object[(CirQueue.m_u32Start + CirQueue.m_u32Length)%BufferDepth] = m_Object;    \
	CirQueue.m_u32Length ++;                                                                 \
}

#define ENQUEUEBACK(CirQueue,BufferDepth,m_Object)                                           \
{                                                                                            \
	CirQueue.Object[(CirQueue.m_u32Start + BufferDepth - 1)%BufferDepth] = m_Object;         \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + BufferDepth - 1)%BufferDepth;               \
	CirQueue.m_u32Length ++;                                                                 \
}

#define DEQUEUE(CirQueue,BufferDepth,m_Object,bOK)                                           \
{                                                                                            \
	bOK = !IS_QUEUE_EMPTY(CirQueue);                                                         \
	\
	if(bOK)                                                                                  \
	{                                                                                        \
	m_Object = CirQueue.Object[CirQueue.m_u32Start];                                     \
	CirQueue.m_u32Length--;                                                              \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + 1)%BufferDepth;                         \
	}                                                                                        \
}

#define DEQUEUEDROP(CirQueue,BufferDepth)                                                    \
{                                                                                            \
	bOK = !IS_QUEUE_EMPTY(CirQueue);                                                         \
	\
	if(bOK)                                                                                  \
	{                                                                                        \
	CirQueue.m_u32Length--;                                                              \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + 1)%BufferDepth;                         \
	}                                                                                        \
}

#define DEQUEUE_TO_POINTER(CirQueue,BufferDepth,m_Pointer,bOK,St_Define)                     \
{                                                                                            \
	bOK = !IS_QUEUE_EMPTY(CirQueue);                                                         \
	\
	if(bOK)                                                                                  \
	{                                                                                        \
	memcpy(m_Pointer,&CirQueue.Object[CirQueue.m_u32Start],sizeof(St_Define));           \
	CirQueue.m_u32Length--;                                                              \
	CirQueue.m_u32Start = (CirQueue.m_u32Start + 1)%BufferDepth;                         \
	}                                                                                        \
}

#define GET_QUEUE_FRONT(CirQueue,bOK,m_Object,BufferDepth)                                   \
{                                                                                            \
	bOK = !IS_QUEUE_EMPTY(CirQueue);                                                        \
	if(bOK)                                                                                  \
	m_Object = CirQueue.Object[(CirQueue.m_u32Start + CirQueue.m_u32Length - 1)%BufferDepth];   \
}



#define ENQUEUEARRAY(CirQueue,BufferDepth,pArray,ArrayCount)   \
{                                                            \
    unsigned long int i;                                 \
	for(i = 0;i<ArrayCount;i++)                          \
	ENQUEUE(CirQueue, BufferDepth, pArray[i]);      \
}

#define PEEKQUEUE(CirQueue,BufferDepth,m_Object,bOK)                                           \
{                                                                                            \
	bOK = !IS_QUEUE_EMPTY(CirQueue);                                                         \
	\
	if(bOK)                                                                                  \
	m_Object = CirQueue.Object[CirQueue.m_u32Start];                                     \
}


/*  以下为示例代码,供参考
typedef struct
{
char flag1;
float         flag2;
signed   char flag3;
unsigned long flag4;

unsigned char mychar[10];
}MyObject;

CircularQueue(MyObject,16) CurrentLoopBuffer;

unsigned char bOK = 0;

for(unsigned char i = 0; i<16; i++)
{
MyObject haha;
haha.flag1 = i;

EnQueue(CurrentLoopBuffer,16,haha);
}

for(unsigned char i = 0; i<2; i++)
{
MyObject haha;

DeQueue(CurrentLoopBuffer,16,haha,bOK);
}

for(unsigned char i = 0; i<16; i++)
{
MyObject haha;
haha.flag1 = i;

EnQueue(CurrentLoopBuffer,16,haha);
}

*/

#endif
