#include "includes.h"

#define MAX_QUEUES_MANAGED 200
#define DEFAULT_QUEUE_SIZE 64

enum QUEUE_TYPE
{
    CUR_QUEUE,
    VOL_QUEUE,
};

struct QueueP
{
    void * Object_0;
    unsigned long int * Start;
    unsigned long int * Length;
    unsigned long int   BufferDepth;
};

struct CQueueConcept
{
    uint16 m_HandleID;
    void * m_QueueStartAddress;
    int m_ItemSize;
    int m_QueueSizeInItem;

    uint32 m_Start;
    uint32 m_Length;
};

class CQ
{
public:
    static uint16 Register(void * QueueStartAddress, int ItemSize, int QueueSizeInItem = DEFAULT_QUEUE_SIZE);
    static bool ResetQueueSize(uint16 handle,int QueueSizeInItem);
    static void * GetQueueAddress(uint16 handle);
    static int GetItemSizeOfQueue(uint16 handle);
    //	static int GetQueueSizeInItem(uint16 handle);
    static void InitQueue(uint16 handle);
    static bool IsQueueFull(uint16 handle);
    static bool IsQueueEmpty(uint16 handle);
    static int GetItemNumber(uint16 handle);
    //	static int GetEmptySpaceNumber(uint16 handle);
    static int Enqueue(uint16 handle, const void * pSourceItem);
    static int Dequeue(uint16 handle, void * pTargetItem);
    static int EnqueueArray(uint16 handle, int ArraySize, const void * pSourceArrayAddress);
    static int DequeueToArray(uint16 handle, int ArraySize, void * pTargetArrayAddress);
    static void BulkDEQUEUE(QueueP  CirQueue,void * Dest,unsigned long LengthInByte);
    static void BulkENQUEUE(QueueP  CirQueue,void * Dest,unsigned long LengthInByte);
#if (USE_AS_AUXMCU == 0)
    static void ModifyQueueDepth(uint16 handle, unsigned int depth, unsigned long *sum, unsigned char queue_type);	// 2016.02.08
#endif
    static int Peekqueue(uint16 handle, void * pTargetItem);

private:
    static struct CQueueConcept m_Queues[MAX_QUEUES_MANAGED];
    static int m_ManagedQueues;
};
