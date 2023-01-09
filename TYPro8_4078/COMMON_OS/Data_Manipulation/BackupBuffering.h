
#ifndef _BACKUPBUFFERING_H_
#define _BACKUPBUFFERING_H_


typedef struct
{
    void* pBuffer;
    unsigned short BufLen;
    unsigned short uc_Net;
    unsigned int Buf_Inactive;
    unsigned int InUse_Idx;
} BackBufIdx;


class BackupBuffering
{
public:
    //	static void* pBuffer;
    static int ConfirmEvent(uint32 eventNo, uint8 uc_Net);
    static int PushingEvents(uint8 * content, int eventLen, uint32 eventNo, uint8 uc_Net);

    //ÔÝÊ±ÒýÓÃ
    static unsigned long long GetTimeTicks(MsTime myTime);
    static void GetTimeTicksUpdate(MsTime * myTime);

    static void MoveEvent2Active(int source, int destination, uint8 uc_Net);	// move(not copy) from inactive buffer to active buffer.
    static int FindNextEmpty(int startPoint, uint8 uc_Net);									// only in Active Buffer
    static void SendDataPack(uint8 uc_Net);
private:
};

#endif
