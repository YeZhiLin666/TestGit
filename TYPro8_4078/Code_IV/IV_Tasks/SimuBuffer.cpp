
#include "../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_BufInfo          SIMU_BUFFER::m_Bufs[MAX_SIMU_BUFFER_COUNT];
St_FragInfo         SIMU_BUFFER::m_Frags[MAX_SIMU_BUFFER_COUNT];
unsigned short      SIMU_BUFFER::m_BufIdx_Check;
MsTime              SIMU_BUFFER::m_LastPackTime;

#pragma arm section

void SIMU_BUFFER::DoWork(void)
{
    m_BufIdx_Check++;
    if(m_BufIdx_Check >= MAX_SIMU_BUFFER_COUNT)
        m_BufIdx_Check = 0;

    if(m_Bufs[m_BufIdx_Check].m_bValid && (m_Bufs[m_BufIdx_Check].m_u32SchRegister != 0))
        return;  //合法 而且 有schedule在用, 则不用维护

    //合法 而且 没有schedule在用，等不够的时候再维护

    if(m_Bufs[m_BufIdx_Check].m_bUsed && (m_Bufs[m_BufIdx_Check].m_bValid == 0))
    {   //包还没有收全
        if(CommonFunc::GetTimeTicks(&m_LastPackTime) < 5000)
            return;

        if(m_Bufs[m_BufIdx_Check].m_RequestCount > 10)
            DeleteBuf(m_BufIdx_Check);//请求太多次都没有收齐，则删除掉

        if(CommonFunc::GetTimeTicks(&m_Bufs[m_BufIdx_Check].m_LastRequestTime) > 2000)  //200ms超时
            RequestPacks(m_BufIdx_Check);
    }
}



void SIMU_BUFFER::DeleteBuf(signed short sBufIdx)
{
    if( (sBufIdx < 0) || (sBufIdx >= MAX_SIMU_BUFFER_COUNT) )
        return;

    unsigned short sFragIdx = 0;
    for(int i = 0; i < m_Bufs[sBufIdx].m_FragmentCount; i++)
    {
        sFragIdx = m_Bufs[sBufIdx].m_SimuIdx[i];
        if( (sFragIdx < 0) || (sFragIdx >= MAX_SIMU_BUFFER_COUNT) )
            continue;

        memset(&m_Frags[sFragIdx],0,sizeof(m_Frags[sFragIdx]));
        m_Frags[sFragIdx].m_BufferIdx = -1;
    }

    memset(&m_Bufs[sBufIdx],0,sizeof(m_Bufs[sBufIdx]));
}

void SIMU_BUFFER::Init(void)
{
    m_BufIdx_Check = 0;
    memset(&m_Frags,0,sizeof(m_Frags));
    memset(&m_Bufs,0,sizeof(m_Bufs));

    for(int i=0; i<MAX_SIMU_BUFFER_COUNT; i++)
    {
        m_Frags[i].m_BufferIdx = -1;
    }
}

void SIMU_BUFFER::RequestPacks(unsigned short BufIdx)
{
    unsigned short FragIdx,FragCount,FragIdx_Physical;
    unsigned short PackIdx,PackCount,PackIdx_Physical;

    FragCount = m_Bufs[BufIdx].m_FragmentCount;
    PackCount = m_Bufs[BufIdx].m_PackCount;

    m_Bufs[BufIdx].m_LastRequestTime = CommonFunc::GetSystemTime();//OS_Time;
    m_Bufs[BufIdx].m_RequestCount++;

    EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_simu_MD5   = m_Bufs[BufIdx].m_MD5;
    EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout = 0;

    for(PackIdx = 0; PackIdx < PackCount; PackIdx++)
    {
        FragIdx          = PackIdx / MAX_SIMU_PACK_COUNT_PER_FRAGMENT;
        FragIdx_Physical = m_Bufs[BufIdx].m_SimuIdx[FragIdx];
        PackIdx_Physical = PackIdx % MAX_SIMU_PACK_COUNT_PER_FRAGMENT;

        if(!CheckBit(m_Frags[FragIdx_Physical].m_bPackValid,PackIdx_Physical))
        {
            EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wPackIndex[EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout] = PackIdx;
            EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout++;

            if(EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout >= MAX_SIMU_REPORT_PACKINDEX)
            {
                EthernetParse::MPReportData(0,SEND_REPORT_SIMULATION_REQUESTDATA);
                EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout = 0;
            }
        }
    }

    if(EthernetParse::m_pReport->m_Report_Simulation_RequestData.m_wTotalCout > 0)
        EthernetParse::MPReportData(0,SEND_REPORT_SIMULATION_REQUESTDATA);
}

void  SIMU_BUFFER::DeleteBuf_NoIVregister(void)
{
    signed short SimuBufIdx;  //  20140611 he  修改Simulation assign的BUG。

    for(SimuBufIdx = 0; SimuBufIdx < MAX_SIMU_BUFFER_COUNT; SimuBufIdx++)
    {   //delete all unused buffers
        if(!m_Bufs[SimuBufIdx].m_bValid)
            continue;

        if(ChannelInfo_Manager::bCanDeleteSchBuf(m_Bufs[SimuBufIdx].m_u32SchRegister))
        {   //kill this simuBuf;
            DeleteBuf(SimuBufIdx);
        }
    }
}

bool SIMU_BUFFER::InsertSimuPack(unsigned long PackIdx,unsigned long PackCount,St_MD5 MD5,unsigned long Size,void *Data)
{
    m_LastPackTime = CommonFunc::GetSystemTime();//OS_Time;

    signed short sFragCount,sFragIdx_Physical,sFragIdx_Virtual;
    unsigned long PackIdx_Physical = PackIdx % MAX_SIMU_PACK_COUNT_PER_FRAGMENT;
    sFragCount = PackCount / MAX_SIMU_PACK_COUNT_PER_FRAGMENT;
    if( (PackCount % MAX_SIMU_PACK_COUNT_PER_FRAGMENT) > 0 )
        sFragCount++;
    sFragIdx_Virtual = PackIdx / MAX_SIMU_PACK_COUNT_PER_FRAGMENT;

    signed short sBufIdx = FindMD5(MD5);
    if(sBufIdx < 0)
    {   //全新的MD5
        sBufIdx = Init_Buf(sFragCount,MD5);
        if(sBufIdx < 0)  //没有足够的空间了
        {
            DeleteBuf_NoIVregister(); //170705 zyx
            sBufIdx = Init_Buf(sFragCount,MD5);
            if(sBufIdx < 0)
                return false;
        }
        //有足够的空间，实施存储
        m_Bufs[sBufIdx].m_PointCount      += (Size>>3);
        sFragIdx_Physical = m_Bufs[sBufIdx].m_SimuIdx[sFragIdx_Virtual];

        memcpy(&m_Frags[sFragIdx_Physical].m_Points[PackIdx_Physical*MAX_SIMU_POINT_COUNT_PER_PACK],
               Data,
               Size);
        SetBit(&m_Frags[sFragIdx_Physical].m_bPackValid[0],PackIdx_Physical,1);
        m_Bufs[sBufIdx].m_LastRequestTime = CommonFunc::GetSystemTime();//OS_Time;
        m_Bufs[sBufIdx].m_PackCounter++;
        m_Bufs[sBufIdx].m_PackCount = PackCount;
        if(m_Bufs[sBufIdx].m_PackCounter == PackCount)
        {
            m_Bufs[sBufIdx].m_bValid = 1;
            for(int i =0; i<m_Bufs[sBufIdx].m_FragmentCount; i++)
                m_Frags[m_Bufs[sBufIdx].m_SimuIdx[i]].m_bValid = 1;
            ChannelInfo_Manager::RegisterSimu2Schedule(MD5);
        }
        return true;
    }

    if(m_Bufs[sBufIdx].m_bValid)
        return false;  //这个MD5已经完整了，不再需要

    //有buffer，不完整:
    sFragIdx_Physical = m_Bufs[sBufIdx].m_SimuIdx[sFragIdx_Virtual];
    if(CheckBit(&m_Frags[sFragIdx_Physical].m_bPackValid[0],PackIdx_Physical))
        return false;  //此包已经存在，无需再存

    //此包不存在，实施存储
    m_Bufs[sBufIdx].m_PointCount      += (Size>>3);
    memcpy(&m_Frags[sFragIdx_Physical].m_Points[PackIdx_Physical*MAX_SIMU_POINT_COUNT_PER_PACK],
           Data,
           Size);
    SetBit(&m_Frags[sFragIdx_Physical].m_bPackValid[0],PackIdx_Physical,1);
    m_Bufs[sBufIdx].m_LastRequestTime = CommonFunc::GetSystemTime();//OS_Time;
    m_Bufs[sBufIdx].m_PackCounter++;
    if(m_Bufs[sBufIdx].m_PackCounter == PackCount)
    {
        m_Bufs[sBufIdx].m_bValid = 1;
        for(int i =0; i<m_Bufs[sBufIdx].m_FragmentCount; i++)
            m_Frags[m_Bufs[sBufIdx].m_SimuIdx[i]].m_bValid = 1;
        ChannelInfo_Manager::RegisterSimu2Schedule(MD5);
    }
    return true;
}

signed short  SIMU_BUFFER::Init_Buf(signed short FragCount,St_MD5 MD5)
{
    signed short sBufIdx = -1;

    for(signed short i = 0; i < MAX_SIMU_BUFFER_COUNT; i++)
    {
        if(m_Bufs[i].m_bUsed == 0)
        {
            sBufIdx = i;
            break;
        }
    }
    if(sBufIdx < 0)
        return -1;

    signed short sFragFreeCount = 0;
    for(signed short i = 0; i<MAX_SIMU_BUFFER_COUNT; i++)
    {
        if(m_Frags[i].m_bUsed == 0)
            sFragFreeCount++;
    }
    if(sFragFreeCount < FragCount)
    {
        DeleteBuf_NoIVregister();
        sFragFreeCount = 0;
        for(signed short i = 0; i<MAX_SIMU_BUFFER_COUNT; i++)
        {
            if(m_Frags[i].m_bUsed == 0)
                sFragFreeCount++;
        }
        if(sFragFreeCount < FragCount)
            return -1;
    }

    //有合法的buf，有足够的fragments:
    m_Bufs[sBufIdx].m_bValid           = 0;
    m_Bufs[sBufIdx].m_bUsed            = 1;
    m_Bufs[sBufIdx].m_FragmentCount    = 0;
    m_Bufs[sBufIdx].m_MD5              = MD5;
    m_Bufs[sBufIdx].m_PointCount       = 0;
    m_Bufs[sBufIdx].m_PackCounter      = 0;
    m_Bufs[sBufIdx].m_PackCount        = 0;
    m_Bufs[sBufIdx].m_RequestCount     = 0;
    m_Bufs[sBufIdx].m_LastRequestTime  = CommonFunc::GetSystemTime();//OS_Time;
    m_Bufs[sBufIdx].m_u32SchRegister   = 0;  //可能需要找schedule注册一下
    for(signed short FragIdx = 0; FragIdx<MAX_SIMU_BUFFER_COUNT; FragIdx++)
    {
        if(m_Frags[FragIdx].m_bUsed == 0)
        {
            m_Bufs[sBufIdx].m_SimuIdx[m_Bufs[sBufIdx].m_FragmentCount] = FragIdx;
            m_Bufs[sBufIdx].m_FragmentCount++;

            m_Frags[FragIdx].m_bUsed         = 1;
            m_Frags[FragIdx].m_bValid        = 0;
            memset(m_Frags[FragIdx].m_bPackValid,0,sizeof(m_Frags[FragIdx].m_bPackValid));
            m_Frags[FragIdx].m_BufferIdx     = sBufIdx;
            m_Frags[FragIdx].m_PointCount    = 0;
            if(m_Bufs[sBufIdx].m_FragmentCount >= FragCount)
                break;
        }
    }
    return sBufIdx;
}
signed short  SIMU_BUFFER::FindMD5(St_MD5 MD5)
{

    for(signed short i = 0; i < MAX_SIMU_BUFFER_COUNT; i++)
    {
        if( (memcmp(&m_Bufs[i].m_MD5.m_ucData[0],&MD5.m_ucData[0],16) == 0) && (m_Bufs[i].m_bUsed ) )
            return i;
    }

    return -1;
}

signed short SIMU_BUFFER::GetSimuIdx(St_MD5 MD5)
{
    signed short BufIdx;

    for(BufIdx = 0; BufIdx < MAX_SIMU_BUFFER_COUNT; BufIdx++)
    {
        if((m_Bufs[BufIdx].m_bValid)
                &&(memcmp(&m_Bufs[BufIdx].m_MD5,&MD5,16) == 0))
            return BufIdx;
    }

    if(BufIdx >= MAX_SIMU_BUFFER_COUNT)
        return -1;
}


unsigned long SIMU_BUFFER::GetSimuPointCount(signed short BufIdx)
{
    if((BufIdx < 0)||(BufIdx > MAX_SIMU_BUFFER_COUNT))
        return 0;

    if(!m_Bufs[BufIdx].m_bValid)
        return 0;
    else
        return m_Bufs[BufIdx].m_PointCount;
}


bool  SIMU_BUFFER::RegisterIV(unsigned char SchBufIdx,St_MD5 MD5)
{
    signed short BufIdx;

    for(BufIdx = 0; BufIdx < MAX_SIMU_BUFFER_COUNT; BufIdx++)
    {
        if((m_Bufs[BufIdx].m_bValid)
                && (memcmp(&m_Bufs[BufIdx].m_MD5,&MD5,16) == 0))
            break;
    }
    if(BufIdx >= MAX_SIMU_BUFFER_COUNT)
        return 0;
    else
    {
        SetBit_32Bit(m_Bufs[BufIdx].m_u32SchRegister,SchBufIdx);
        return 1;
    }
}

bool SIMU_BUFFER::UnRegisterIV(unsigned char SchBufIdx,St_MD5 MD5)
{
    signed short BufIdx;

    for(BufIdx = 0; BufIdx < MAX_SIMU_BUFFER_COUNT; BufIdx++)
    {
        if((m_Bufs[BufIdx].m_bValid)
                && (memcmp(&m_Bufs[BufIdx].m_MD5,&MD5,16) == 0))
            break;
    }
    if(BufIdx >= MAX_SIMU_BUFFER_COUNT)
        return 0;
    else
    {
        ClrBit_32Bit(m_Bufs[BufIdx].m_u32SchRegister,SchBufIdx);
        if(m_Bufs[BufIdx].m_u32SchRegister == 0)    //delete this simulation file
        {
//             m_Bufs[BufIdx].m_bValid = 0;
//             for(int i = 0; i < m_Bufs[BufIdx].m_FragmentCount; i++)
//                 m_Frags[m_Bufs[BufIdx].m_SimuIdx[i]].m_bValid = 0;
						DeleteBuf(BufIdx);
        }
        return 1;
    }
}

bool SIMU_BUFFER::GetNextValue(unsigned char ChannelIdx)
{
    unsigned long FragIdx,PointIdx;
    St_SimuRunInfo *SimuInfo = &ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_SimuInfo;

    if(SimuInfo->m_PointIdx >= m_Bufs[SimuInfo->m_BufIdx].m_PointCount)
        return 0;

    FragIdx = SimuInfo->m_PointIdx / MAX_SIMU_FRAGMENT_POINT_COUNT;
    PointIdx = SimuInfo->m_PointIdx % MAX_SIMU_FRAGMENT_POINT_COUNT;

    SimuInfo->m_fValue = m_Frags[m_Bufs[SimuInfo->m_BufIdx].m_SimuIdx[FragIdx]].m_Points[PointIdx].m_fValue;
    SimuInfo->m_fEndTime = m_Frags[m_Bufs[SimuInfo->m_BufIdx].m_SimuIdx[FragIdx]].m_Points[PointIdx].m_fTime;

    return 1;
}
