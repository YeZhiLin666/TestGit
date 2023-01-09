/*****************************************************************************
// 程序文件                   :EventsBuffering.h     主要用于事件缓存和重发
// 文件描述                   :为MCU board服务
// 编写日期(MM.DD.YYYY)       :08.05.2013
*****************************************************************************/

#ifndef _EVENTSBUFFERING_H_
#define _EVENTSBUFFERING_H_

#define EVENTSBUFFERSIZE	600
#define EVENTLENGTH			500

struct EventsBuffer
{
    uint32 EventNo;
    int EventLen;
    uint8 EventContent[EVENTLENGTH];

};

struct EventsBuffer_Map
{
    unsigned char bValid;
    UWord16  uw16_Array;
    uint32 EventNo;
};
class EventsBuffering
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
    static void MyInit(void);
    static void DoWork(void);
    static int ConfirmEvent(uint32 eventNo);
    static int PushingEvents(uint8 * content, int eventLen, uint32 eventNo);



private:
    static UWord16             uw16_Events_Total[2];
    static UWord16             uw16_Map_Point[2];
    static MsTime              MyCounter;
    static struct EventsBuffer EV_Buf[2][EVENTSBUFFERSIZE];
    static struct EventsBuffer_Map   EV_Map[2][EVENTSBUFFERSIZE];
    static int Buf_Active;
    static int Buf_Inactive;  // Add by DKQ 08.05.2013
    static int InUse_Idx;

    static void MoveEvent2Active(int source, int destination);	// move(not copy) from inactive buffer to active buffer.
    static int FindNextEmpty(int startPoint);									// only in Active Buffer
    static int EVBufferMap_FindEventNo(unsigned char uc_back,unsigned short int eventNo);
    static void EVBufferMap_MoveEvent(unsigned char uc_back,unsigned short int uw16_Point);
    static void EVBufferMap_Clear(unsigned char uc_back,unsigned char uc_valid);
    static void EVBufferMap_Insert(unsigned char uc_back,unsigned short int Index,unsigned short int eventNo);

};


#endif
