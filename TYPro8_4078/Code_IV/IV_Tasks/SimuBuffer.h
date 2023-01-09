#pragma once

#include "../IV_Tasks/ScheduleDataDefine.h"


typedef struct
{
    float         m_fTime;
    float         m_fValue;
} St_SimuPoint;

typedef struct
{
    unsigned char      m_bValid;
    unsigned char      m_bUsed;
    signed short       m_BufferIdx;
    unsigned long      m_PointCount;
    unsigned char      m_bPackValid[MAX_SIMU_PACK_COUNT_PER_FRAGMENT * MAX_SIMU_BUFFER_COUNT / 8 +1];

    St_SimuPoint       m_Points[MAX_SIMU_FRAGMENT_POINT_COUNT];
} St_FragInfo;
typedef struct
{
    St_MD5             m_MD5;
    unsigned short     m_FragmentCount;
    unsigned char      m_bValid;
    unsigned char      m_bUsed;
    unsigned long      m_PointCount;
    unsigned long      m_PackCounter;
    unsigned long      m_PackCount;

    MsTime             m_LastRequestTime;
    unsigned char      m_RequestCount;

    unsigned long      m_u32SchRegister;
    signed short       m_SimuIdx[MAX_SIMU_BUFFER_COUNT];
} St_BufInfo;

class SIMU_BUFFER
{
private:
    static St_BufInfo          m_Bufs[MAX_SIMU_BUFFER_COUNT];
    static St_FragInfo         m_Frags[MAX_SIMU_BUFFER_COUNT];
    static unsigned short      m_BufIdx_Check;
    static MsTime              m_LastPackTime;

public:
    static void                DoWork(void);
    static void                DeleteBuf(signed short sBufIdx);
    static void                Init(void);
    static void                RequestPacks(unsigned short BufIdx);
    static void                DeleteBuf_NoIVregister(void);
    static bool                InsertSimuPack(unsigned long PackIdx,unsigned long PackCount,St_MD5 MD5,unsigned long Size,void *Data);
    static signed short        Init_Buf(signed short FragCount,St_MD5 MD5);
    static signed short        FindMD5(St_MD5 MD5);
    static bool                InsertSimuFragment(unsigned long FragmentIdx,unsigned long FragmentCount,St_MD5 MD5,unsigned long Size,void *Data);
    static signed short        GetSimuIdx(St_MD5 MD5);                  //when load new step
    static unsigned long       GetSimuPointCount(signed short BufIdx);
    static bool                RegisterIV(unsigned char SchBufIdx,St_MD5 MD5);
    static bool                UnRegisterIV(unsigned char SchBufIdx,St_MD5 MD5);
    static bool                GetNextValue(unsigned char ChannelIdx);  //when load new point
};


