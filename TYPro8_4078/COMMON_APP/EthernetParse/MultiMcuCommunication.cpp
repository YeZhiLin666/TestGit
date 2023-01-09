/*****************************************************************************
// 程序文件                   :MCUCOMMUNICATION.cpp     主要用于MCU间UDP通信
// 文件描述                   :为 MCU board服务
// 编写日期(MM.DD.YYYY)       :04.26.2016
*****************************************************************************/

#include "../Entry/includes.h"

UInt16 MultiMcuComm::Priority = PRIORITY4;
UInt16 MultiMcuComm::TaskID = MULTIMCUCOMMUNICATION_TASK;

// 设置变量定义在外存
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

WaitQueue2          MultiMcuComm::WaitQueue[MCU_MAX_NUMBER];            // 发送窗口队列个数管理,相当于指针
SendWindow2         MultiMcuComm::SendWindowQueue[MCU_MAX_NUMBER];      // 发送窗口队列个数管理,相当于指针
ReceiveWindow2      MultiMcuComm::ReceiveWindowQueue[MCU_MAX_NUMBER];   // 接收窗口队列个数管理,相当于指针

St_MCU_Data 	        MultiMcuComm::St_WaitBuffer[MCU_MAX_NUMBER][MCU_MAX_WAITBUFFER_SIZE];     // 待发报文缓冲区
St_MCU_InfoPack 	    MultiMcuComm::St_ReceiveBuffer[MCU_MAX_NUMBER][MCU_MAX_RECEIVEBUFFER_SIZE];     // 接收报文缓冲区
St_MCU_InfoPack         MultiMcuComm::St_ConfirmBuffer[MCU_MAX_NUMBER][MCU_MAX_CONFIRMBUFFER_SIZE];  // 确认数据报文缓冲区

unsigned char       MultiMcuComm::uc_SlicingStatus;                               // MCU发送限制状态
unsigned char       MultiMcuComm::uc_SlicingEnable;                               // MCU发送限制使能

// MsTime              MultiMcuComm::m_PackProduceTime[MCU_MAX_NUMBER];              // 生成报文间隔时间
MsTime              MultiMcuComm::m_SlicingOutTime;                               // MCU发送时间限制超时
// unsigned short int  MultiMcuComm::uw16_LimitSendWindowSize_m;     // 限制发送窗口大小发送
// unsigned short int  MultiMcuComm::uw16_AddSendWindowSize_n;       // 增加发送窗口大小发送
unsigned short int  MultiMcuComm::uw16_AbsolveDieLockCounter[MCU_MAX_NUMBER];

// unsigned char       MultiMcuComm::uc_Scan_Index;
St_NewTable         MultiMcuComm::st_UnitNewTable[MCU_MAX_NUMBER];
unsigned char       MultiMcuComm::uc_UnitNewTable_Total;

unsigned short int         MultiMcuComm::uw16_CharFailCounter;   // 字节发送失败计数器
CharSend1       MultiMcuComm::CharSend;         // 字节发送队列
CharMGE2        MultiMcuComm::CharMGE;          // 字节发送队列个数管理,相当于指针
St_MCU_WindowMessage   MultiMcuComm::WinMess;
St_MCU_Unit     MultiMcuComm::St_Unit[MCU_MAX_NUMBER];
uint8 	        MultiMcuComm::uc_CmdBuffer_Temp[MAX_PACKLIMIT_SIZE_UDP];
unsigned char   MultiMcuComm::uc_udp_Temp[100];
unsigned char   MultiMcuComm::uc_udp_data[MCU_MAX_CMD_WAITBUFFER_SIZE];
unsigned char   MultiMcuComm::uc_udp_BroadCastdata[MCU_MAX_CMD_WAITBUFFER_SIZE];

// uint8      *    MultiMcuComm::this_ReceiveBuff_Data;

UWord32         MultiMcuComm::uw32_Time_Connect = MCU_MAX_RECONNECT_TIME;
UWord32         MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_NORMAL;

/*             unsigned long int MCUCommun::uw32_Scounter[MCU_MAX_NUMBER]={0,0,0};
               unsigned long int MCUCommun::uw32_Rcounter[MCU_MAX_NUMBER]={0,0,0};
               unsigned short int MCUCommun::uw16_ST1=0;
               unsigned short int MCUCommun::uw16_RT1=0;
               unsigned short     MCUCommun::s_d_SN[40];
               unsigned long      MCUCommun::s_d_Second[40];
               unsigned long      MCUCommun::s_d_Us100[40];
               unsigned short     MCUCommun::r_d_SN[40];
               unsigned long      MCUCommun::r_d_Second[40];
               unsigned long      MCUCommun::r_d_Us100[40];*/


#pragma arm section //结束外存变量定义

extern          uint8 	   uc_UDP_SendLock;

// 暂时测试用PC广播包  MCU_UDP_BROADCAST

void MultiMcuComm::MyInit(void)
{
    //memset((void *)&St_Unit,0,sizeof(St_MCU_Unit));  // 已扫过MAP表不能再初始化
    for(unsigned char uc_j=0; uc_j<MCU_MAX_NUMBER; uc_j++)
    {
        INIT_QUEUE(SendWindowQueue[uc_j]);
        INIT_QUEUE(WaitQueue[uc_j]);
        INIT_QUEUE(ReceiveWindowQueue[uc_j]);
        St_Unit[uc_j].uw16_SeqNum  = 1;
        St_Unit[uc_j].uw16_StarNum= 1 ;
        St_Unit[uc_j].uw16_EndNum = 1;
        St_Unit[uc_j].uw16_ConnectNum = 1;
        St_Unit[uc_j].uw16_ConfirmSeqNum  = 1;
        St_Unit[uc_j].uw16_ConfirmStarNum = 0;
        St_Unit[uc_j].uw16_ConfirmEndNum = 0;
        St_Unit[uc_j].uc_ErrorType = ERROR_NONE;
        St_Unit[uc_j].uc_ErrorCode = UDP_NONE;
        St_Unit[uc_j].uc_ErrorSeqNumCounter = 0;
        St_Unit[uc_j].uc_ConnectSuccess  = false;
        St_Unit[uc_j].uc_ReConnectCounter = 0;
        St_Unit[uc_j].uc_SendPackCounter = 0;
        St_Unit[uc_j].uc_SendTimeEN_On = false;
        St_Unit[uc_j].uc_ReceiveTimeEN_On = false;
        //St_Unit[uc_j].uc_Active  = false;    // 任务中初始化慢于MAP表加载，不能在此初始化重要信息。
        uw16_AbsolveDieLockCounter[uc_j] = 0;
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime1); // 更新时间间隔器
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime2); // 更新时间间隔器
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime3); // 更新时间间隔器
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime1); // 更新时间间隔器
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime2); // 更新时间间隔器
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime3); // 更新时间间隔器
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ConfirmConnectOutTime); // 更新时间间隔器
        St_Unit[uc_j].uw16_ReceiveWaitOutTime1_Counter = 0;
    }
	MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_NORMAL;

//     OS_ServeProcess::GetTimeTicksUpdate( &m_PackProduceTime); // 更新时间间隔器
    CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime);  // 更新时间间隔器
//     uw16_LimitSendWindowSize_m  = MCU_LIMITSENDSIZE;
//     uw16_AddSendWindowSize_n  = 1;

//    uc_Scan_Index = 0;
    uc_SlicingStatus = SLICING_STATUS_IDLE;
    uc_SlicingEnable = false;
    uw32_Time_Connect = MCU_MAX_RECONNECT_TIME;
    uw16_CharFailCounter = 0;
}

void MultiMcuComm::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_MCU_RUDP] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_MCU_RUDP);
#endif

    if(uc_UnitNewTable_Total <= 0)
        return;

    main_TcpNet(); //  网络管理

    for(unsigned char uc_i = 0; uc_i < uc_UnitNewTable_Total; uc_i ++)
    {
        unsigned char uc_addr = st_UnitNewTable[uc_i].uc_Address;
        if(St_Unit[uc_addr].uc_Active)
        {
            MCU_ReconnectToMCU(uc_addr);
            ReceiveProcess(uc_addr);
            TransferProcess(uc_addr);
            ErrorProcess(uc_addr);
            AbsolveDieLockProcess(uc_addr);
        }
    }

    SlicingProcess();

    if(!uc_SlicingEnable)
    {
        unsigned long uw32_time = (uc_UnitNewTable_Total/5)*MCU_MAX_PRODUCE_PACKTIME; // 多于5个MCU降速发送
        if(CommonFunc::GetTimeTicks( &m_SlicingOutTime) >= uw32_time)    //超过4ms // 按时间限制
        {
            //NVIC_DisableIRQ(ENET_IRQn);
            MultiMcuComm::PackToNetLinkRoad();
            //NVIC_EnableIRQ(ENET_IRQn);
            CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // 更新时间间隔器
        }
    }

    main_TcpNet(); //  网络管理

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_MCU_RUDP] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_MCU_RUDP);
#endif
}

int MultiMcuComm::PushingPackToWaitBuffer(uint8 uc_DestID, uint8 * content, unsigned int DataLen)// ,unsigned char VitalFlag)
{
    // 装载数据进入等待缓冲区
    Word16 w16_index = Seek_Unit(uc_DestID);   // 查表 优化20170227
    if(w16_index < 0)
        return -1;

    unsigned char uc_Error = 0;
    if(IS_QUEUE_FULL(WaitQueue[w16_index],MCU_MAX_WAITBUFFER_SIZE))
        uc_Error = 1;
    if(DataLen > MCU_MAX_CMD_WAITBUFFER_SIZE)
        uc_Error = 1;

    unsigned long  uw32_Num = (WaitQueue[w16_index].m_u32Start + WaitQueue[w16_index].m_u32Length)%MCU_MAX_WAITBUFFER_SIZE;
    St_WaitBuffer[w16_index][uw32_Num].uw16_PackLength = DataLen;
    St_WaitBuffer[w16_index][uw32_Num].uc_DestID = uc_DestID;
    memcpy(&St_WaitBuffer[w16_index][uw32_Num].m_UDPWaitBuffer,content,DataLen);

    if(uc_Error)
    {
        St_Unit[w16_index].uc_ErrorType = ERROR_QUEUE_FAIL;
        St_Unit[w16_index].uc_ErrorCode = UDP_QUEUE_UNIT_FULL;
        return -1;
    }

    St_MCU_WAIT_MQueue  st_W;
    st_W.uw16_PackAddress = uw32_Num;
    ENQUEUE(WaitQueue[w16_index],MCU_MAX_WAITBUFFER_SIZE,st_W);  // 队列管理


    /*  CAN_Clear_Limit  *Limit= (CAN_Clear_Limit *) &content[4];
        uw32_Scounter[w16_index]++;
        s_d_SN[uw16_ST1] = Limit->SN;
        s_d_Second[uw16_ST1] = OS_ServeProcess::OS_Time.Second;
        s_d_Us100[uw16_ST1] = OS_ServeProcess::OS_Time.Us100;
        if(++uw16_ST1 >= 40)
                uw16_ST1=0;*/

    return 1;
}

short int MultiMcuComm::MoveWaitBufferToSendBuffer(unsigned char  uc_Index, unsigned short int Length)
{
// 	if(CANFunc::IsCANopen())
// 		NVIC_DisableIRQ(CAN_IRQn);

    // 从等待缓冲区装载数据进入发送缓冲区
    bool bOK;
    unsigned char uc_Error = 0;
// 	unsigned short int  uw16_i = 0;
    if(IS_QUEUE_EMPTY(WaitQueue[uc_Index]))
        uc_Error = 1;

    //for(unsigned short uw16_i=0;uw16_i<Length;uw16_i++)
    {
        St_MCU_MQueue_Window  st_W;
        St_MCU_WAIT_MQueue    st_S;
        if(IS_QUEUE_FULL(SendWindowQueue[uc_Index],MCU_MAX_SENDWINDOW_SIZE))
            uc_Error = 1;

        if((!St_Unit[uc_Index].uc_ConnectSuccess)||(St_Unit[uc_Index].uw16_EndNum >= MCU_MAX_SEQUENCE_NUMBER))
            uc_Error = 1;

        Word16  w16_P = FindConfirmBuffer(uc_Index);
        if(w16_P >= 0)
            st_W.st_Address.uw16_PackAddress = w16_P;
        else
        {
            // this is full.  改为出错处理
            uc_Error = 1;
        }

        if(uc_Error)
        {
            // St_Unit[uc_Index].uc_ErrorType = ERROR_QUEUE_FAIL;
            // St_Unit[uc_Index].uc_ErrorCode = UDP_QUEUE_SENDWINDOW_FULL;
// 						if(CANFunc::IsCANopen())
// 							NVIC_EnableIRQ(CAN_IRQn);
            return -1;
        }

        unsigned short  uw16_Num;
        //st_W.st_Address.uc_VitalFlag = 0;
        st_W.st_Address.uw16_PackLength = 0;
        unsigned long  uw32_Item = WaitQueue[uc_Index].m_u32Length;
//         if(uw32_Item > MCU_MAX_CMD_ITEM_SIZE)
//             uw32_Item = MCU_MAX_CMD_ITEM_SIZE;
        unsigned long uw32_i = 0;
        for(uw32_i = 0; uw32_i < uw32_Item; uw32_i++)
        {
            PEEKQUEUE(WaitQueue[uc_Index], MCU_MAX_WAITBUFFER_SIZE, st_S, bOK);
            uw16_Num = st_S.uw16_PackAddress;
            if(st_W.st_Address.uw16_PackLength + St_WaitBuffer[uc_Index][uw16_Num].uw16_PackLength <= MCU_MAX_CMD_BUFFER_SIZE)//MAX_PACKLIMIT_SIZE_UDP
            {
                DEQUEUE(WaitQueue[uc_Index], MCU_MAX_WAITBUFFER_SIZE, st_S, bOK);
                uw16_Num = st_S.uw16_PackAddress;
                memcpy(&St_ConfirmBuffer[uc_Index][w16_P].m_Data[st_W.st_Address.uw16_PackLength],&St_WaitBuffer[uc_Index][uw16_Num].m_UDPWaitBuffer[0],St_WaitBuffer[uc_Index][uw16_Num].uw16_PackLength);   //  存入实体中
                st_W.st_Address.uw16_PackLength += St_WaitBuffer[uc_Index][uw16_Num].uw16_PackLength;
            }
            else
                break;
        }
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_PackItem = uw32_i;//uw32_Item; //zyx 1901081
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_PackLength = st_W.st_Address.uw16_PackLength;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_SeqNum = St_Unit[uc_Index].uw16_SeqNum;
        St_ConfirmBuffer[uc_Index][w16_P].uc_IfSaveFlag = 1;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uc_IfFirstFlag = 1;
        St_ConfirmBuffer[uc_Index][w16_P].uc_DestID = St_Unit[uc_Index].uc_ID;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uc_StatusFlag = PACKET_STATUS_DATA;
        St_ConfirmBuffer[uc_Index][w16_P].uw16_ReSendCount = 0;
        for(unsigned char uc_i=0; uc_i<8; uc_i++)
            St_ConfirmBuffer[uc_Index][w16_P].m_btNone_11[uc_i] = 0XBB;    // 包头前填空8个"0XBB"

        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_ConnectNum= St_Unit[uc_Index].uw16_ConnectNum;

        st_W.st_Address.uc_IfSaveFlag = 1;
        st_W.st_Address.uw16_SeqNum = St_Unit[uc_Index].uw16_SeqNum;  //  窗口队列管理
        ENQUEUE(SendWindowQueue[uc_Index],MCU_MAX_SENDWINDOW_SIZE,st_W);
//        (uw16_ConfirmBufferTotal[uc_Index] < MCU_MAX_SENDWINDOW_SIZE) ? uw16_ConfirmBufferTotal[uc_Index]++ : uw16_ConfirmBufferTotal[uc_Index] = MCU_MAX_SENDWINDOW_SIZE;

        St_Unit[uc_Index].uw16_StarNum = St_Unit[uc_Index].uw16_SeqNum;
        St_Unit[uc_Index].uw16_EndNum = St_Unit[uc_Index].uw16_SeqNum;
        St_Unit[uc_Index].uw16_SeqNum ++;
    }
// 		if(CANFunc::IsCANopen())
// 			NVIC_EnableIRQ(CAN_IRQn);

    return 1;
}

short int MultiMcuComm::FindConfirmBuffer(unsigned char uc_Index)
{
    // 查找空的确认缓冲，以便能存储和方便发送数据信息
    UWord16 uw16_i;
    for(uw16_i=0; uw16_i<MCU_MAX_CONFIRMBUFFER_SIZE; uw16_i++)
    {
        if(!St_ConfirmBuffer[uc_Index][uw16_i].uc_IfSaveFlag)
        {
            return uw16_i;
        }
    }

    if(uw16_i >= MCU_MAX_CONFIRMBUFFER_SIZE)
    {
        //St_Unit[uc_Index].uc_ErrorType = ERROR_QUEUE_FAIL;
        //St_Unit[uc_Index].uc_ErrorCode = UDP_QUEUE_SENDWINDOW_FULL;
        return -1;  //  无法找到出错处理
    }
}

short int MultiMcuComm::Seek_Unit(uint8 uc_DestID)
{
    // 查找Unit存放位置
    unsigned short uw16_i;
    for(uw16_i=0; uw16_i<uc_UnitNewTable_Total; uw16_i++)
    {
        if(st_UnitNewTable[uw16_i].w16_D_IP == uc_DestID) //  查新表
        {
            return st_UnitNewTable[uw16_i].uc_Address;
        }
    }

    if(uw16_i >= uc_UnitNewTable_Total)
    {
        return -1;  //  无法找到出错处理
    }
}

void MultiMcuComm::Build_NewTable(void)
{
    // 建立新表，方便查找ID
    unsigned short uw16_i;
    uc_UnitNewTable_Total = 0;
    for(uw16_i=0; uw16_i<MCU_MAX_NUMBER; uw16_i++)
    {
        st_UnitNewTable[uw16_i].uc_Address = 0;
        st_UnitNewTable[uw16_i].w16_D_IP = -1;  // 不管任何情况，先清理干净新表。
    }

    unsigned char uc_n = 0;
    REMOTEM rm_UDP;
    for(uw16_i=0; uw16_i<MCU_MAX_NUMBER; uw16_i++)
    {
        if(St_Unit[uw16_i].uc_Active) //  查表
        {
            st_UnitNewTable[uc_n].uc_Address = uw16_i;
            st_UnitNewTable[uc_n].w16_D_IP = St_Unit[uw16_i].uc_ID;
            uc_n++;

            memcpy(rm_UDP.IpAdr, uc_remotemIpAdr, IP_ADRLEN);
            rm_UDP.IpAdr[3] = St_Unit[uw16_i].uc_ID;
            if(!arp_get_info (& rm_UDP))
            {
                arp_notify();
            }
            main_TcpNet(); //  网络管理
        }
    }
    uc_UnitNewTable_Total = uc_n;



}

short int MultiMcuComm::ReSendSingleMessageProcess(unsigned char uc_Index,UWord16 uw16_SN)
{
    // 重发单报文
    UWord16 uw16_i=0;
    short int W16_result = 0;

    for(uw16_i=0; uw16_i<MCU_MAX_CONFIRMBUFFER_SIZE; uw16_i++)
    {

        if(St_ConfirmBuffer[uc_Index][uw16_i].uc_IfSaveFlag
                && (!St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uc_IfFirstFlag))
        {
            if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum != uw16_SN)
                continue;
            if(St_ConfirmBuffer[uc_Index][uw16_i].uc_DestID != St_Unit[uc_Index].uc_ID)
            {
                MCU_ClearConfirmBuffer(uc_Index,uw16_i);
                continue;
            }

            unsigned short uw16_length = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_PackLength+MCU_UDP_SN_PACKHEAD;
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uc_IfFirstFlag = 0;
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
            St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount ++;
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_StarNum = St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount;// 为测试重发判断增加次数20170401
            if(St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount >= MAX_RESENDPACK_NUM)
            {
                St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount = 0;
                St_Unit[uc_Index].uc_ConnectSuccess  = false;   // 多次重发失败后，重新连接UDP
                uw32_Time_Connect = MCU_MAX_RECONNECT_TIME/10;
            }
            else
            {
                if((St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount&0X0007 )>= 0X0007)
                {
                    uc_SlicingStatus = SLICING_STATUS_TIME;   // 每7次限制一次通信 350ms=7*50
                    CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // 更新时间间隔器
                }
            }
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // 更新时间间隔器
            unsigned char * uc_p = (unsigned char *) &St_ConfirmBuffer[uc_Index][uw16_i].m_btNone_11[0];  // 发送失败缓冲入字节发送区
            unsigned char resultok = 0;
            resultok = PackToNetSendByte(St_ConfirmBuffer[uc_Index][uw16_i].uc_DestID,uc_p,uw16_length);
            if(resultok)
                St_Unit[uc_Index].uc_SendPackCounter++;
            W16_result = uw16_i;
            break;
        }
    }

    if(uw16_i >= MCU_MAX_CONFIRMBUFFER_SIZE)
    {
        //St_Unit[uc_Index].uc_ErrorType = ERROR_TRANSFER_FAIL;
        //St_Unit[uc_Index].uc_ErrorCode = UDP_TRANSFER_GREAT_RESEND;
        W16_result = -1;  //  无法找到出错处理
    }

    return W16_result;
}

void MultiMcuComm::MCU_ReconnectToMCU(unsigned char uc_Index)
{
    // 重建连接流程
    St_Unit[uc_Index].uc_SendPackCounter = 0;
    if(St_Unit[uc_Index].uc_ConnectSuccess)
    {
        St_Unit[uc_Index].uc_ReConnectCounter = 0XFF;
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ConfirmConnectOutTime); // 更新时间间隔器
    }
    else
    {
        UWord32 uw32_Time = uw32_Time_Connect + 50*(EthernetParse::uw16_MyIP[1]>>8);
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_ConfirmConnectOutTime) >= uw32_Time)    //超过5s
        {
            // 发送重建连接报文
            if(++St_Unit[uc_Index].uc_ReConnectCounter > MCU_MAX_RECONNECT_NUM)  // 25s变换一次连接号
            {
                MultiMcuComm::ReceiveProcess(uc_Index);
                St_Unit[uc_Index].uw16_ConnectNum<MCU_MAX_SEQUENCE_NUMBER?St_Unit[uc_Index].uw16_ConnectNum++:St_Unit[uc_Index].uw16_ConnectNum=1;
                St_Unit[uc_Index].uw16_ConfirmStarNum = 0;
                St_Unit[uc_Index].uw16_ConfirmEndNum = 0;
                St_Unit[uc_Index].uw16_SeqNum = 1;
                St_Unit[uc_Index].uw16_StarNum = 1;
                St_Unit[uc_Index].uw16_EndNum = 1;
                St_Unit[uc_Index].uc_ReConnectCounter = 0;
                /*           if(uw32_Time_Connect < MCU_MAX_RECONNECT_TIME)
                             {
                                  St_Unit[uc_Index].uc_ErrorType = ERROR_STOPSCH;
                                  St_Unit[uc_Index].uc_ErrorCode = ERROR_STOPSCH;
                             }*/
                uw32_Time_Connect = MCU_MAX_RECONNECT_TIME;
            }

            REMOTEM rm_UDP;
            memcpy(rm_UDP.IpAdr, uc_remotemIpAdr, IP_ADRLEN);
            rm_UDP.IpAdr[3] = St_Unit[uc_Index].uc_ID;
            if(!arp_get_info (& rm_UDP))  // 如果无物理地址，先获取物理地址。
                arp_notify();

            MCU_ConfirmBuff_UpdataSeqNum(uc_Index);
            uc_SlicingStatus = SLICING_STATUS_IDLE;
            uc_SlicingEnable = false;
            memset(&WinMess, 0, sizeof(St_MCU_WindowMessage));
            WinMess.uc_SourceID = St_Unit[uc_Index].uc_ID;
            WinMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
            WinMess.uc_IfFirstFlag = 1;
            WinMess.uw16_StarNum = 1;
            MCU_Connect_SendRequest(&WinMess);
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ConfirmConnectOutTime); // 更新时间间隔器

//             if((St_Unit[uc_Index].uc_Mapbit&UDP_MAP_CAN_BMS) == UDP_MAP_CAN_BMS)
//             {
//                 for(unsigned char uc_k = 0; uc_k <  ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum; uc_k++)
//                 {
//                     if(St_Unit[uc_Index].uc_ID == ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_k].m_AuxUnitID)
//                     {
//                         unsigned char IV_Ch = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_k].m_IVChnNo;
//                         CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[IV_Ch].m_UnsafeTime);
//                     }
//                 }
//             }
        }
    }
}

unsigned char MultiMcuComm::Scan_IV_AUXMAP(void)
{
    unsigned char m_SeekUnitID = 0;
    MultiMcuComm::Scan_Clear(UDP_MAP_AUX);
#if  USE_AS_AUXMCU == 1

    for (unsigned char i=0; i<MAX_IVMCU_PER_SYS; i++)
    {
        for (unsigned char j=0; j<MAX_IVCH_PER_IVMCU; j++)
        {
            for (unsigned char k=0; k<MAX_AUX_TYPE; k++)
            {
                for(unsigned short m=0; m<MAX_MAP_COUNT_PER_UNIT; m++)  // MAX_AUXCH_PER_AUXMCU
                {
                    if (!Aux_DataManager::m_DownLoadMapFromPC[m].m_valid)
                        continue;

                    if ((Aux_DataManager::m_DownLoadMapFromPC[m].m_IVUnit == Aux_DataManager::m_IVMCU_ID_Info[i].m_ID)
                            &&(Aux_DataManager::m_DownLoadMapFromPC[m].m_IVCh == j)&&(Aux_DataManager::m_DownLoadMapFromPC[m].m_AuxType == k))
                    {
                        m_SeekUnitID = Aux_DataManager::m_DownLoadMapFromPC[m].m_IVUnit ;
                        Scan_EmptySeat(m_SeekUnitID,j,UDP_MAP_AUX);
                    }
                }
            }

        }
    }
#else
    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
    {
        for(unsigned char uc_j = 0; uc_j < MAXAUXTYPE; uc_j++)
        {
            for(unsigned char uc_k = 0; uc_k <  Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Total_Aux_Unit; uc_k++)
            {
                m_SeekUnitID = Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitID[uc_k];
                Scan_EmptySeat(m_SeekUnitID,uc_i,UDP_MAP_AUX);
            }
        }
    }
#endif
    Build_NewTable();
}

unsigned char MultiMcuComm::Scan_IV_CANBMSMAP(void)
{
    MultiMcuComm::Scan_Clear(UDP_MAP_CAN_BMS);
#if  USE_AS_AUXMCU == 1
    unsigned char uc_clear = 1;
    ST_CANBMS_MAP* st_CANBMS_Map = (ST_CANBMS_MAP*)&EthernetParse::m_btPackDataBuffer[0];
    for(unsigned char uc_index=0; uc_index<2; uc_index++) //  CAN1,CAN2两都可用。多功能板中仅用CAN2对接客户BMS电池.    <st_CANBMS_Map->m_AllChnNum
    {
        if(st_CANBMS_Map->m_CANmap[uc_index].m_bIVActive)
        {
            Scan_EmptySeat(st_CANBMS_Map->m_CANmap[uc_index].m_AuxUnitID,st_CANBMS_Map->m_CANmap[uc_index].m_IVChnNo,UDP_MAP_CAN_BMS);   // 先调BMS MAP，再调AUX MAP

            if(CANBusParse::m_CANBMS_AuxInfo.m_IVCANID == st_CANBMS_Map->m_CANmap[uc_index].m_AuxUnitID)
                uc_clear = 0;
        }
    }

    if(uc_clear)
    {
        for(unsigned char Idx = 0; Idx<CANBusParse::m_CANBMS_AuxInfo.m_validMsgCnt; Idx++)
        {
            CANBusParse::m_CANBMS_AuxInfo.m_Msg[Idx].m_bValid = 0;
        }
        CANBusParse::m_CANBMS_AuxInfo.m_validMsgCnt = 0;
    }
#else
    unsigned char m_SeekUnitID = 0;
    unsigned char IV_Ch = 0;

    for(unsigned char uc_k = 0; uc_k <  ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum; uc_k++)
    {
        m_SeekUnitID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_k].m_AuxUnitID;
        IV_Ch = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_k].m_IVChnNo;

        if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_k].m_bIVActive)
            continue;

        Scan_EmptySeat(m_SeekUnitID,IV_Ch,UDP_MAP_CAN_BMS);
    }

#endif
    Build_NewTable();
}

unsigned char MultiMcuComm::Scan_IV_ParallelMAP(void)
{

#if  USE_AS_AUXMCU == 1

#else
    unsigned char m_SeekUnitID = 0;
//     unsigned char IV_Ch = 0;
//     unsigned char m_CANPort = CAN_PORT_USE_PUBLIC;
    MultiMcuComm::Scan_Clear(UDP_MAP_IV_PARALLEL);

    for(unsigned char uc_i = 0; uc_i <  MAXCHANNELNO; uc_i++)
    {   //  如果是Master，上级通道就是自己。如果是SubMaster，上级通道就是master。如果是 slave，上级通道就是 Sub master。
        if(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Master)
        {   // 注册(Master主)管理的下级
            for(unsigned char uc_j = 0; uc_j < IVSDL::m_GrpInfo4SDL[uc_i].SubmastCount-1; uc_j++)
            {
                m_SeekUnitID = IVSDL::m_GrpInfo4SDL[uc_i].Submaster[uc_j].CanID;
                Scan_EmptySeat(m_SeekUnitID,uc_i,UDP_MAP_IV_PARALLEL);
            }
        }
        else if((IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Submaster)||(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Serial)||(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Booster))
        {   // 注册(Submaster从的主、Slave从)管理的的上级
            m_SeekUnitID = IVSDL::m_GrpInfo4SDL[uc_i].Master.CanID;
            Scan_EmptySeat(m_SeekUnitID,uc_i,UDP_MAP_IV_PARALLEL);
        }
    }

#endif
    Build_NewTable();
}

char MultiMcuComm::Scan_EmptySeat(unsigned char UnitID,unsigned char IV_Ch,unsigned char Map)
{
    unsigned char m_find_out = 0;
    short int Empty = -1;
    unsigned char uc_n = 0;
    unsigned char uc_MCU_Unit_Total = 0;
    if((UnitID == CANFunc::SourceBoardID)||(UnitID < 1))
        return -1;

    for(uc_n = 0; uc_n <  MCU_MAX_NUMBER; uc_n++)
    {
        if((!St_Unit[uc_n].uc_Active)&&(Empty == -1))
            Empty = uc_n;

        if((UnitID == St_Unit[uc_n].uc_ID)&&St_Unit[uc_n].uc_Active)  // 整个表查一次
        {
            m_find_out = 1;   // 查到表中已经有记录
            St_Unit[uc_n].uc_Mapbit |= Map;
            RegisterChannel(uc_n, IV_Ch,Map);
            return 1;
        }
    }
    if(!m_find_out)
    {
        if(uc_MCU_Unit_Total >= MCU_MAX_NUMBER)
            return -1;

        if(Empty == -1)
            Empty = 0;

        for(uc_n = Empty; uc_n <  MCU_MAX_NUMBER; uc_n++)
        {
            if(!St_Unit[uc_n].uc_Active) //  查到表中未有记录，再找个空位置记录下来
            {
                St_Unit[uc_n].uc_ID = UnitID;
                St_Unit[uc_n].uc_Active = 1;
                St_Unit[uc_n].uc_Mapbit = Map;
                RegisterChannel(uc_n,IV_Ch,Map);
                uc_MCU_Unit_Total ++;
                break;
            }
        }
    }
    return 1;
}

void MultiMcuComm::Scan_Clear(unsigned char Map)
{
    unsigned char uc_m = 0;
    unsigned char uc_n = 0;
    unsigned char uc_r = 0;
    unsigned long int *  u32_IvRegister;
    for(uc_r=0; uc_r<3; uc_r++)
    {
        if((Map>>uc_r) == 0x01)
            break;
    }
	if(uc_r>=3)
		uc_r = 0;   // 20190829 yy
    for(uc_n = 0; uc_n <  MCU_MAX_NUMBER; uc_n++)
    {
        uc_m = St_Unit[uc_n].uc_Mapbit;
        u32_IvRegister = &St_Unit[uc_n].u32_Aux_IvRegister;
        u32_IvRegister += uc_r;
        if(St_Unit[uc_n].uc_Active && ((uc_m&Map)==Map))  // 整个表查一次
        {
            if((uc_m^Map)==0) // 其它没使用，仅单个使用直接清理
            {
                if((*u32_IvRegister)!=0)
                    continue;
                St_Unit[uc_n].uc_ID = 0;
                St_Unit[uc_n].uc_Active = 0;
                St_Unit[uc_n].uc_Mapbit = 0;
                St_Unit[uc_n].u32_Aux_IvRegister = 0;
                St_Unit[uc_n].u32_Parallel_IvRegister = 0;
                St_Unit[uc_n].u32_BMS_IvRegister = 0;
                St_Unit[uc_n].uc_ErrorType = ERROR_NONE;
                St_Unit[uc_n].uc_ErrorCode = UDP_NONE;
                St_Unit[uc_n].uc_ErrorSeqNumCounter = 0;
                St_Unit[uc_n].uc_ConnectSuccess  = false;
                St_Unit[uc_n].uc_ReConnectCounter = 0;
                St_Unit[uc_n].uc_SendPackCounter = 0;
                St_Unit[uc_n].uc_SendTimeEN_On = false;
                St_Unit[uc_n].uc_ReceiveTimeEN_On = false;
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_WaitConfirmOutTime1); // 更新时间间隔器
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_ReceiveWaitOutTime1); // 更新时间间隔器
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_ConfirmConnectOutTime); // 更新时间间隔器
                St_Unit[uc_n].m_ConfirmConnectOutTime.Second -= MCU_MAX_RECONNECT_TIME/10000*4; //20s 提快重连,马上重连。
                St_Unit[uc_n].uw16_ReceiveWaitOutTime1_Counter = 0;
                uw16_AbsolveDieLockCounter[uc_n] = 0;

                //INIT_QUEUE(SendWindowQueue[uc_n]);
                //INIT_QUEUE(WaitQueue[uc_n]);
                //INIT_QUEUE(ReceiveWindowQueue[uc_n]);
            }
						else//多个map则清除当前map // yzl 2020.10.29
						{
							St_Unit[uc_n].uc_Mapbit &= ~Map;
						}						
        }
    }
}

int MultiMcuComm::RegisterChannel(unsigned char uc_Index, unsigned char uc_ChannelNo,unsigned char Map)
{
    if(uc_ChannelNo >= MAXCHANNELNO)
        return -1;
    unsigned char uc_m = 0;
    unsigned long int *  u32_IvRegister = &St_Unit[uc_Index].u32_Aux_IvRegister;
    for(uc_m=0; uc_m<3; uc_m++)
    {
        if((Map>>uc_m) == 0x01)
            break;
    }
	if(uc_m>=3)
		uc_m = 0;  // 20190829 yy
    u32_IvRegister +=	uc_m;
    SetBit_32Bit(*u32_IvRegister, uc_ChannelNo);
    return  1;
}

int MultiMcuComm::UnregisterChannel(unsigned char uc_Index, unsigned char uc_ChannelNo,unsigned char Map)
{
    if(uc_ChannelNo >= MAXCHANNELNO)
        return -1;
    unsigned char uc_m = 0;
    unsigned long int *  u32_IvRegister = &St_Unit[uc_Index].u32_Aux_IvRegister;
    for(uc_m=0; uc_m<3; uc_m++)
    {
        if((Map>>uc_m) == 0x01)
            break;
    }
	if(uc_m>=3)
		uc_m = 0; // 20190829 yy
    u32_IvRegister +=	uc_m;
    ClrBit_32Bit(*u32_IvRegister,uc_ChannelNo);
    return  1;
}

void MultiMcuComm::ReceiveProcess(unsigned char uc_Index)
{
    main_TcpNet(); //  网络管理
    if(IS_QUEUE_EMPTY(ReceiveWindowQueue[uc_Index]))
        return;

    Word16  w16_P;
    unsigned char bOk;
    St_MCU_WAIT_MQueue  st_W;
    do {
        NVIC_DisableIRQ(ENET_IRQn);
        DEQUEUE(ReceiveWindowQueue[uc_Index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W,bOk);  //  窗口队列管理
        NVIC_EnableIRQ(ENET_IRQn);
        w16_P =st_W.uw16_PackAddress;
        St_MCU_WindowMessage  *  Message = &WinMess;
        *Message = St_ReceiveBuffer[uc_Index][w16_P].st_PackMess;
        unsigned char uc_StatusFlag = St_ReceiveBuffer[uc_Index][w16_P].st_PackMess.uc_StatusFlag & 0X0F;
        switch(uc_StatusFlag)
        {
        case PACKET_STATUS_ASK:
            Message->uw16_SeqNum = Message->uw16_EndNum;
            MCU_ConfirmProcess(uc_Index,Message);
            for(unsigned char uc_i=0; uc_i<MCU_MAX_SENDWINDOW_SIZE; uc_i++)
                ReSendSingleMessageProcess(uc_Index,Message->uw16_EndNum+uc_i);
            break;
        case PACKET_STATUS_CONNECT:
            unsigned char uc_updata = 0;
            WinMess = *Message;
#if  USE_AS_AUXMCU == 0
            if((St_Unit[uc_Index].uc_Mapbit & UDP_MAP_IV_PARALLEL) == UDP_MAP_IV_PARALLEL)
            {
                if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitPara_MasterUnit(0))
                    WinMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
            }
            else
                WinMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
#endif
            if(WinMess.uw16_StarNum  <= 2 )
            {
                if(WinMess.uw16_StarNum == 2)
                    uc_updata = 1;
                WinMess.uw16_StarNum++;
                MCU_Connect_SendRequest(&WinMess);
            }
            else
                uc_updata = 1;
            if(uc_updata)
            {
                // 先清除一次接收区中的内容
                for(unsigned char uc_n=0; uc_n<MCU_MAX_RECEIVEBUFFER_SIZE; uc_n++)
                    MCU_ClearReceiveBuffer(uc_Index,uc_n);
                INIT_QUEUE(ReceiveWindowQueue[uc_Index]);

                St_Unit[uc_Index].uc_ConnectSuccess = true;
                St_Unit[uc_Index].uw16_SeqNum = 1;
                St_Unit[uc_Index].uw16_ConfirmSeqNum = 1;
                St_Unit[uc_Index].uw16_StarNum = 1;
                St_Unit[uc_Index].uw16_EndNum = 1;
                St_Unit[uc_Index].uw16_ConfirmStarNum = 0;
                St_Unit[uc_Index].uw16_ConfirmEndNum = 0;
#if  USE_AS_AUXMCU == 1
                St_Unit[uc_Index].uw16_ConnectNum = Message->uw16_ConnectNum;
#else
                if((St_Unit[uc_Index].uc_Mapbit & UDP_MAP_IV_PARALLEL) == UDP_MAP_IV_PARALLEL)
                {
                    if(!ChannelInfo_Manager::Serial2ParaMod_IsInterUnitPara_MasterUnit(0))
                        St_Unit[uc_Index].uw16_ConnectNum = Message->uw16_ConnectNum;
                }
#endif

                St_Unit[uc_Index].uc_ReceiveTimeEN_On = false;
                if(St_ConfirmBuffer[uc_Index][0].uc_IfSaveFlag)
                {
                    St_Unit[uc_Index].uc_SendTimeEN_On = false; //加快重发
                    St_Unit[uc_Index].m_WaitConfirmOutTime1.Second>=1?St_Unit[uc_Index].m_WaitConfirmOutTime1.Second--:St_Unit[uc_Index].m_WaitConfirmOutTime1.Second=0;
                    St_ConfirmBuffer[uc_Index][0].uw16_ReSendCount = 0;
                    MCU_ConfirmBuff_UpdataSeqNum(uc_Index);
                }
                else
                    St_Unit[uc_Index].uc_SendTimeEN_On = false;
            }
            break;
        case PACKET_STATUS_DATA:
            // 数据暂时用CAN网帧对接
            unsigned char uc_feedback = 1;
            uc_feedback = UpdataConfirmStarNum(uc_Index,Message); // 包请求号处理

            Message->uw16_SeqNum = Message->uw16_EndNum;
            if(Message->uw16_EndNum >= 1)//2009221 yy
                MCU_ConfirmProcess(uc_Index,Message);

            if((!uc_feedback)||(Message->uc_SourceID != St_Unit[uc_Index].uc_ID))
                break;

            MultiMcuComm::NET_Rx_Msg(St_ReceiveBuffer[uc_Index][w16_P].m_Data, St_ReceiveBuffer[uc_Index][w16_P].st_PackMess.uw16_PackItem, St_ReceiveBuffer[uc_Index][w16_P].st_PackMess.uw16_PackLength);
//                for(unsigned char uc_i=0; uc_i<St_ReceiveBuffer[uc_Index][w16_P].st_PackMess.uw16_PackItem; uc_i++)
//                {
// 					this_ReceiveBuff_Data = &St_ReceiveBuffer[uc_Index][w16_P].m_Data[uc_i*MCU_MAX_CAN_CMD_DATA_SIZE];
// 					MCUCommun::NET_Rx_Msg(this_ReceiveBuff_Data);
// 					CANFunc::NETtoCAN_RxProcess();
//                }
            CANBusParse::CANParse(RUDPNET);
            break;
        case PACKET_STATUS_CONFIRM:
            MCU_ConfirmProcess(uc_Index,Message);
            if(Message->uw16_StarNum >= MCU_MAX_SEQUENCE_NUMBER)
            {
                St_Unit[uc_Index].uw16_ConnectNum<MCU_MAX_SEQUENCE_NUMBER?St_Unit[uc_Index].uw16_ConnectNum++:St_Unit[uc_Index].uw16_ConnectNum=1;
                St_Unit[uc_Index].uw16_SeqNum = 1;
                St_Unit[uc_Index].uw16_StarNum = 1;
                St_Unit[uc_Index].uw16_EndNum = 1;
                St_Unit[uc_Index].uw16_ConfirmStarNum = 0;
                St_Unit[uc_Index].uw16_ConfirmEndNum = 0;
            }
            break;
        default:
            break;
        }
        //St_ReceiveBuffer[uc_Index][w16_P].uc_IfSaveFlag = 0;
        //St_ReceiveBuffer[uc_Index][w16_P].uc_DestID = 0;
        MCU_ClearReceiveBuffer(uc_Index,w16_P);
    } while(QUEUE_DEPTH(ReceiveWindowQueue[uc_Index])>0);
}

unsigned char  MultiMcuComm::NET_Rx_Msg(unsigned char * Data, unsigned short PackItem, unsigned int PackTotalLength)		// Receive Data from the IF2 buffer
{
    unsigned char *head = Data;
    U32 totalLen = 0;
    for(unsigned char uc_i=0; uc_i<PackItem; uc_i++)
        //CANFunc::ReadRxReg.RxFrame.Frame = D.RxFrame.Frame;
        //CANFunc::ReadRxReg.RxData.uw32_Data[0] = D.RxData.uw32_Data[0];
        //CANFunc::ReadRxReg.RxData.uw32_Data[1] = D.RxData.uw32_Data[1];
    {
        unsigned short DataLen = (unsigned short)*(head);
        totalLen += DataLen;
        if(totalLen > PackTotalLength)
            break;
        MP_MCU_MSG mcuMsg;
        memset(&mcuMsg, 0, sizeof(MP_MCU_MSG));
        memcpy(&mcuMsg, head, DataLen);
        head += DataLen;

        CANFunc::TempNETtoCANRxData.FrameType = CAN_EXT_FRAME; //RD.RxFrame.BitFrame.FF;
        CANFunc::TempNETtoCANRxData.len = sizeof(UN_CANWORD) ;//RD.RxFrame.BitFrame.DLC;
        CANFunc::TempNETtoCANRxData.RxID = mcuMsg.Un_CANID.m_U32ID;//RD.RxID;
        CANFunc::TempNETtoCANRxData.RxRES = 1;
        memcpy(&CANFunc::TempNETtoCANRxData.RxData.uw32_Data[0], &mcuMsg.Un_Content, sizeof(UN_CANWORD));
//         CANFunc::TempNETtoCANRxData.RxData.uw32_Data[0] =  RD.RxFrame.Frame;
//         CANFunc::TempNETtoCANRxData.RxData.uw32_Data[1] = RD.RxData.uw32_Data[0];

        CANFunc::NETtoCAN_RxProcess();
    }
    return  CAN_RXOK;
}
// 调用该函数来发送CAN报文
unsigned char MultiMcuComm::NET_Push_to_Send(const MP_MCU_MSG McuMsg)
{
// 		if(CANFunc::IsCANopen())
// 			NVIC_DisableIRQ(CAN_IRQn);
    //将命令做正确解析 然后存入事件缓存 等待确认
    if(McuMsg.Un_CANID.m_CANID.DestID == 0XFF)  // 广播包转换点对点
    {
        MultiMcuComm::NET_BroadCast_to_Send(McuMsg);
// 				if(CANFunc::IsCANopen())
// 					NVIC_EnableIRQ(CAN_IRQn);
        return 1;
    }

    Word16 w16_index = Seek_Unit(McuMsg.Un_CANID.m_CANID.DestID);
    if(w16_index < 0)
    {
// 			if(CANFunc::IsCANopen())
// 				NVIC_EnableIRQ(CAN_IRQn);
        return 0;
//     if(IS_QUEUE_FULL(WaitQueue[w16_index],MCU_MAX_WAITBUFFER_SIZE))
    }
    memcpy(&uc_udp_data, &McuMsg, McuMsg.ByteLength);


    PushingPackToWaitBuffer(McuMsg.Un_CANID.m_CANID.DestID, &uc_udp_data[0], McuMsg.ByteLength);
// 		if(CANFunc::IsCANopen())
// 			NVIC_EnableIRQ(CAN_IRQn);
    return 1;
}

unsigned char MultiMcuComm::NET_BroadCast_to_Send(MP_MCU_MSG McuMsg)
{
    //将命令做正确解析 然后存入事件缓存 等待确认
    unsigned short int uw16_count = 0;
//     UW_F uc_d;
    UN_CANID  ID = McuMsg.Un_CANID;
//     memcpy(&uc_udp_data[4], (uint8 *) (&can_msg->Un_Content.Data), 8);

    for(unsigned short int uw16_i=0; uw16_i<MCU_MAX_NUMBER; uw16_i++)
    {
        if(St_Unit[uw16_i].uc_Active)
        {
            uw16_count++;
            ID.m_CANID.DestID = St_Unit[uw16_i].uc_ID;
            ID.m_CANID.SourceID = EthernetParse::uw16_MyIP[1]>>8;

            if(IS_QUEUE_FULL(WaitQueue[uw16_i],MCU_MAX_WAITBUFFER_SIZE))
                continue;

//            uc_d.ul_member = ID.m_U32ID;
            McuMsg.Un_CANID.m_U32ID = ID.m_U32ID;
            memcpy(&uc_udp_data, &McuMsg, McuMsg.ByteLength);

//             uc_udp_data[12]='Z';
//             uc_udp_data[13]='Z';
//             uc_udp_data[14]='Z';
//             uc_udp_data[15]='Z';

            PushingPackToWaitBuffer(St_Unit[uw16_i].uc_ID, &uc_udp_data[0], McuMsg.ByteLength);  // , 1);
        }
    }

    return 1;
}


unsigned char MultiMcuComm::NET_Push_to_BroadCastSend(MP_MCU_MSG McuMsg)
{
    //将命令做正确解析 然后存入事件缓存
// 			if(CANFunc::IsCANopen())
// 				NVIC_DisableIRQ(CAN_IRQn);

    unsigned short int uw16_length = 0;
//     UW_F uc_d;
    UN_CANID  ID = McuMsg.Un_CANID;
//   memcpy(&uc_udp_data[4], (uint8 *) (&can_msg->Un_Content.Data), 8);

    ID.m_CANID.DestID = 0xFF;
    ID.m_CANID.SourceID = EthernetParse::uw16_MyIP[1]>>8;

    //uc_d.ul_member = ID.m_U32ID;
    McuMsg.Un_CANID.m_U32ID = ID.m_U32ID;
    memcpy(&uc_udp_data, &McuMsg, McuMsg.ByteLength);
    //memcpy(&uc_udp_data, &uc_d.uc_member, 4);    // 4+8=12字节CAN包

// 		uc_udp_data[12]='Z';
// 		uc_udp_data[13]='Z';
// 		uc_udp_data[14]='Z';
// 		uc_udp_data[15]='Z';

//     unsigned char uc_Success = 0;
//     unsigned char uc_L_Flag = 0;

//     UWord16 uw16_i = 0;

    St_MCU_Head * st_Head = (St_MCU_Head*)&uc_udp_Temp[0];
    st_Head->st_PackMess.uc_StatusFlag = PACKET_STATUS_DATA;

    for(unsigned char uc_i=0; uc_i<8; uc_i++) // 8字节CAN包
        st_Head->m_btNone_11[uc_i] = 0XBB;

    st_Head->st_PackMess.uw16_SeqNum =  1;   // 12字节CAN包
    st_Head->st_PackMess.uc_IfFirstFlag = 1;
    st_Head->st_PackMess.uw16_ConnectNum= 1;
    st_Head->st_PackMess.uw16_StarNum = 1;
    st_Head->st_PackMess.uw16_EndNum = 1;
    st_Head->st_PackMess.uw16_PackLength = 0;
    st_Head->st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
    memcpy(&uc_udp_Temp[MCU_UDP_SN_PACKHEAD], &uc_udp_data[0], McuMsg.ByteLength);    // 4+8+4=16字节CAN包
    uw16_length = st_Head->st_PackMess.uw16_PackLength + MCU_UDP_SN_PACKHEAD + McuMsg.ByteLength;  // 20+16=36字节CAN包

    UdpRxTx::udp_SendToBroadCast(&uc_udp_Temp[0], uw16_length);
// 		if(CANFunc::IsCANopen())
// 			NVIC_EnableIRQ(CAN_IRQn);
    return 1;
}

void MultiMcuComm::MCU_SendConfirm(St_MCU_WindowMessage  *  WindowMessage)
{
    unsigned char UnitID = WindowMessage->uc_SourceID;
    short uw16_Idx = Seek_Unit(UnitID);
    if(uw16_Idx < 0)
        return;

//     unsigned char uc_Success = 0;
    unsigned short int uw16_length = 0;
    St_MCU_Head * st_Head = (St_MCU_Head*)&uc_udp_Temp[0];
    memset(&st_Head->st_PackMess, 0, sizeof(St_MCU_WindowMessage));
    st_Head->st_PackMess.uc_StatusFlag = PACKET_STATUS_CONFIRM;

    for(unsigned char uc_i=0; uc_i<8; uc_i++)
        st_Head->m_btNone_11[uc_i] = 0XBB;
    st_Head->st_PackMess.uw16_SeqNum = WindowMessage->uw16_SeqNum; // WindowBuffering::EndNum;
    st_Head->st_PackMess.uc_IfFirstFlag = 1;
    st_Head->st_PackMess.uw16_PackLength = 0; //sizeof(St_MCU_Head);
    st_Head->st_PackMess.uw16_StarNum =  WindowMessage->uw16_StarNum;
    st_Head->st_PackMess.uw16_EndNum =  WindowMessage->uw16_EndNum;
    st_Head->st_PackMess.uw16_ConnectNum =  WindowMessage->uw16_ConnectNum;
    st_Head->st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
    uw16_length = st_Head->st_PackMess.uw16_PackLength + MCU_UDP_SN_PACKHEAD;
    St_Unit[uw16_Idx].uw16_ReceiveWaitOutTime1_Counter = 0;
    CommonFunc::GetTimeTicksUpdate( &St_Unit[uw16_Idx].m_ReceiveWaitOutTime1); // 更新时间间隔器
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // 发送失败缓冲入字节发送区
    unsigned char resultok = 0;
    resultok = PackToNetSendByte(UnitID,uc_p,uw16_length);
    if(resultok)
        St_Unit[uw16_Idx].uc_SendPackCounter++;
}

void MultiMcuComm::MCU_SendAsk(St_MCU_WindowMessage  *  WindowMessage)
{
    unsigned char UnitID = WindowMessage->uc_SourceID;
    short uw16_Idx = Seek_Unit(UnitID);
    if(uw16_Idx < 0)
        return;

//     unsigned char uc_Success = 0;
//     unsigned char uc_L_Flag = 0;
    unsigned short int uw16_length = 0;
//     UWord16 uw16_i = 0;

    St_MCU_Head * st_Head = (St_MCU_Head*)&uc_udp_Temp[0];
    memset(&st_Head->st_PackMess, 0, sizeof(St_MCU_WindowMessage));
    st_Head->st_PackMess.uc_StatusFlag = PACKET_STATUS_ASK;

    for(unsigned char uc_i=0; uc_i<8; uc_i++)
        st_Head->m_btNone_11[uc_i] = 0XBB;

    st_Head->st_PackMess.uw16_SeqNum =  WindowMessage->uw16_SeqNum;//WindowBuffering::uw16_SeqNum;  // 0
    st_Head->st_PackMess.uc_IfFirstFlag = 1;
    st_Head->st_PackMess.uw16_ConnectNum= WindowMessage->uw16_ConnectNum;
    st_Head->st_PackMess.uw16_StarNum = WindowMessage->uw16_StarNum;
    st_Head->st_PackMess.uw16_EndNum = WindowMessage->uw16_EndNum;
    st_Head->st_PackMess.uw16_PackLength = 0;
    st_Head->st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
    uw16_length = st_Head->st_PackMess.uw16_PackLength + MCU_UDP_SN_PACKHEAD;
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // 发送失败缓冲入字节发送区
    unsigned char resultok = 0;

    for(unsigned char uc_i=0; uc_i<5; uc_i++)
        uc_udp_Temp[MCU_UDP_SN_PACKHEAD+uc_i] = 0X81;
    resultok = PackToNetSendByte(UnitID,uc_p,uw16_length+5);

    //resultok = PackToNetSendByte(UnitID,uc_p,uw16_length);
    if(resultok)
        St_Unit[uw16_Idx].uc_SendPackCounter++;
}

void MultiMcuComm::MCU_Connect_SendRequest(St_MCU_WindowMessage  *  WindowMessage)
{
//     unsigned char uc_Success = 0;
    unsigned short int uw16_length = 0;
    unsigned char UnitID = WindowMessage->uc_SourceID;
    short uw16_Idx = Seek_Unit(UnitID);
    if(uw16_Idx < 0)
        return;

    St_MCU_Head * st_Head = (St_MCU_Head*)&uc_udp_Temp[0];
    memset(&st_Head->st_PackMess, 0, sizeof(St_MCU_WindowMessage));
    st_Head->st_PackMess.uc_StatusFlag = PACKET_STATUS_CONNECT;

    for(unsigned char uc_i=0; uc_i<8; uc_i++)
        st_Head->m_btNone_11[uc_i] = 0XBB;
    st_Head->st_PackMess.uw16_SeqNum = 0; // EndNum;
    st_Head->st_PackMess.uc_IfFirstFlag = WindowMessage->uc_IfFirstFlag;
    st_Head->st_PackMess.uw16_StarNum = WindowMessage->uw16_StarNum;
    st_Head->st_PackMess.uw16_EndNum = 0;// EndNum;
    st_Head->st_PackMess.uw16_ConnectNum = WindowMessage->uw16_ConnectNum;
    st_Head->st_PackMess.uw16_PackLength = 0;
    st_Head->st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
    uw16_length = st_Head->st_PackMess.uw16_PackLength + MCU_UDP_SN_PACKHEAD;
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // 发送失败缓冲入字节发送区
    unsigned char resultok = 0;

    for(unsigned char uc_i=0; uc_i<7; uc_i++)
        uc_udp_Temp[MCU_UDP_SN_PACKHEAD+uc_i] = 0X80;
    resultok = PackToNetSendByte(UnitID,uc_p,uw16_length+7);

    //resultok = PackToNetSendByte(UnitID,uc_p,uw16_length);
    if(resultok)
        St_Unit[uw16_Idx].uc_SendPackCounter++;
}

unsigned char MultiMcuComm::MCU_ConfirmProcess(unsigned char uc_Index,St_MCU_WindowMessage  *  WindowMessage)
{
    unsigned short uw16_counter = 0;
//     unsigned short uw16_index = 0;
    St_MCU_WindowMessage    WinMess = *WindowMessage;
//     unsigned char UnitID = WinMess.uc_SourceID;
//     short uw16_Idx = Seek_Unit(UnitID);
//     if(uw16_Idx < 0)
//         return uw16_counter;

    for(unsigned short uw16_i = 0; uw16_i < MCU_MAX_CONFIRMBUFFER_SIZE; uw16_i ++)
    {
        if(St_ConfirmBuffer[uc_Index][uw16_i].uc_IfSaveFlag)
        {
            if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum != WinMess.uw16_ConnectNum)
                continue;

            if((St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum == WinMess.uw16_SeqNum)
                    ||(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum <= WinMess.uw16_EndNum))
            {
                MCU_ClearConfirmBuffer(uc_Index,uw16_i);
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // 更新时间间隔器
                //break;
                uw16_counter++;
            }

//             if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum == WinMess.uw16_EndNum+1)
//                uw16_index = uw16_i; // 方便重发处理
        }
    }

    if(St_Unit[uc_Index].uw16_EndNum == WinMess.uw16_EndNum)
        St_Unit[uc_Index].uc_SendTimeEN_On = false;

    return uw16_counter;
}

void MultiMcuComm::MCU_ClearConfirmBuffer(unsigned char uc_Index,unsigned char uc_Numb)
{
    St_ConfirmBuffer[uc_Index][uc_Numb].uc_IfSaveFlag = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].uc_DestID = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].st_PackMess.uw16_SeqNum = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].st_PackMess.uw16_StarNum = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].st_PackMess.uw16_EndNum = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].st_PackMess.uw16_ConnectNum = 0;
    St_ConfirmBuffer[uc_Index][uc_Numb].uw16_ReSendCount = 0;
//    (uw16_ConfirmBufferTotal[uc_Index] > 0) ? uw16_ConfirmBufferTotal[uc_Index]--: uw16_ConfirmBufferTotal[uc_Index] = 0;
}

void MultiMcuComm::MCU_ClearReceiveBuffer(unsigned char uc_Index,unsigned char uc_Numb)
{
    St_ReceiveBuffer[uc_Index][uc_Numb].uc_IfSaveFlag = 0;
    St_ReceiveBuffer[uc_Index][uc_Numb].uc_DestID = 0;
    //St_ReceiveBuffer[uc_Index][uc_Numb].uw16_ReSendCount = 0;
}

void MultiMcuComm::TransferProcess(unsigned char uc_Index)
{
    bool bOK = true;
//     unsigned char uc_SendUDP = 0 ;
    uint32 u32_DataLength = 0;
//     unsigned long int QLength;
    St_MCU_MQueue_Window  st_D;
    if((!St_Unit[uc_Index].uc_ConnectSuccess)
            ||St_Unit[uc_Index].uc_SendPackCounter >= MCU_MAX_LIMITSEND_RECEIVEPACK
            ||uc_SlicingEnable)
        return;
    ProduceMessageToSendBuffer(uc_Index,1);

    main_TcpNet(); //  网络管理
    //发送之前的预处理
    if(!IS_QUEUE_EMPTY(SendWindowQueue[uc_Index]))  //发送窗口队列不空
    {
//         QLength = MCU_MAX_WINDOW - SendWindowQueue[uc_Index].m_u32Length;
//         if(QLength >= MCU_LIMITSENDSIZE)
//             QLength = MCU_LIMITSENDSIZE;
//          do{
        u32_DataLength = QUEUE_DEPTH(SendWindowQueue[uc_Index]);
        if(u32_DataLength >= 1)   //发送窗口队列发送报文
        {
            DEQUEUE(SendWindowQueue[uc_Index], MCU_MAX_SENDWINDOW_SIZE, st_D, bOK);
//                         unsigned char uc_Success = 0;
            unsigned long int uw32_length = st_D.st_Address.uw16_PackLength+MCU_UDP_SN_PACKHEAD;
            if(St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].uc_IfSaveFlag)
            {
                // 更新确认包号信息
                if(St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].uc_DestID != St_Unit[uc_Index].uc_ID)
                {
                    MCU_ClearConfirmBuffer(uc_Index,st_D.st_Address.uw16_PackAddress);
                }
                else
                {
                    St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].st_PackMess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
                    St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].st_PackMess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
                    St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].st_PackMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;

                    static  unsigned long  int uw32_TCountB =0;
                    uw32_TCountB ++;

                    unsigned char * uc_p = (unsigned char *) &St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].m_btNone_11[0];;  // 发送失败缓冲入字节发送区
                    unsigned char resultok = 0;
                    resultok = PackToNetSendByte(St_Unit[uc_Index].uc_ID,uc_p,uw32_length);
                    if(resultok)
                        St_Unit[uc_Index].uc_SendPackCounter++;
                    St_Unit[uc_Index].uc_SendTimeEN_On = true;
                    CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // 更新时间间隔器
//                                 OS_ServeProcess::GetTimeTicksUpdate( &m_PackProduceTime[uc_Index]); // 更新时间间隔器
                    St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].st_PackMess.uc_IfFirstFlag = 0;
                    St_Unit[uc_Index].uc_ReceiveTimeEN_On = false;
                }
            }
        }
//           }while(u32_DataLength>=1);//(u16_DataLength>=QLength);  // 1毫秒一次发送便可
    }

    if(St_Unit[uc_Index].uc_SendPackCounter >= MCU_MAX_LIMITSEND_RECEIVEPACK)
        return;

    if(St_Unit[uc_Index].uc_SendTimeEN_On)
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_WaitConfirmOutTime1) >= MCU_MAX_SEND_OUTTIME1)    //超过100ms
        {
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // 更新时间间隔器
            short w16_Num = MCU_ConfirmBuff_FindMin(uc_Index);
            if(w16_Num >= 0)
                ReSendSingleMessageProcess(uc_Index,St_ConfirmBuffer[uc_Index][w16_Num].st_PackMess.uw16_SeqNum);
        }

    if(St_Unit[uc_Index].uc_SendPackCounter >= MCU_MAX_LIMITSEND_RECEIVEPACK)
        return;

    if(St_Unit[uc_Index].uc_ReceiveTimeEN_On)
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_ReceiveWaitOutTime1) >= uw32_MaxTime_Confirm) //MCU_MAX_SEND_CONFIRM1)    //超过6ms
        {
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ReceiveWaitOutTime1); // 更新时间间隔器
            St_MCU_WindowMessage  *  Message = &WinMess;
            Message->uc_SourceID  = St_Unit[uc_Index].uc_ID;
            Message->uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
            Message->uw16_SeqNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
            Message->uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
            Message->uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;

            if(St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter <= 0)
            {
                if(Message->uw16_StarNum > Message->uw16_EndNum)
                    Message->uw16_StarNum = Message->uw16_EndNum;
                Message->uw16_SeqNum = Message->uw16_StarNum;
                MCU_SendConfirm(Message);
                St_Unit[uc_Index].uw16_ConfirmStarNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
                St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter = 1;
                return;
            }

            if(++St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter >= MCU_MAX_SEND_ASK)    //超过600ms
            {
                St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter = 1;
                Message->uw16_SeqNum = St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE;
                MCU_SendAsk(Message);
            }
        }

// 	main_TcpNet(); //  网络管理
//     MCUCommun::PackToNetLinkRoad();
//     main_TcpNet(); //  网络管理
}

char MultiMcuComm::SlicingProcess(void)
{
    char  uc_result = -1;
    if(uc_SlicingStatus == SLICING_STATUS_IDLE)
        return uc_result;

    switch(uc_SlicingStatus)
    {
    case SLICING_STATUS_NUM:
        uc_SlicingEnable = true;
//             uw16_LimitSendWindowSize_m = MCU_MAX_WINDOW;  // 按包量限制
        uc_result = 1;
        break;
    case SLICING_STATUS_TIME:
        uc_SlicingEnable = true;
        if(CommonFunc::GetTimeTicks( &m_SlicingOutTime) >= MCU_MAX_PRODUCE_PACKTIME)    //超过4ms // 按时间限制
        {
            uc_SlicingEnable = false;
            uc_SlicingStatus = SLICING_STATUS_IDLE;
            CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // 更新时间间隔器
        }
        uc_result = 1;
        break;

    case SLICING_STATUS_IDLE:
    default:
        break;
    }
    return  uc_result;
}

void MultiMcuComm::ErrorProcess(unsigned char uc_Index)
{
    // 错误处理
    if(St_Unit[uc_Index].uc_ErrorType == ERROR_NONE)
        return;

    unsigned char  uc_Error = 0;
    unsigned short int  uw16_i = 0;
    switch(St_Unit[uc_Index].uc_ErrorType)
    {
    case ERROR_CONNECT_LOST:
        if(!St_Unit[uc_Index].uc_ConnectSuccess)  // 未连接
        {
            uc_Error = 1;     //  记录相关错误信息，待通信恢复后上传。
        }
        break;
    case ERROR_CHECK_FAIL:
        uc_Error = 1;     //  记录相关错误信息，待通信恢复后上传。
        break;
    case ERROR_STOPSCH:
#if  USE_AS_AUXMCU == 0
        unsigned long int *  u32_IvRegister;
        for(uw16_i=0; uw16_i<MAXCHANNELNO; uw16_i++)
        {
            if(Schedule_Manager_task::IfSchRunning(uw16_i))           //运行的通道要执行stop动作，不必考虑各种回复，上位此时已经挂了
            {
                for(unsigned char uc_r=0; uc_r<3; uc_r++)
                {
                    u32_IvRegister = &St_Unit[uc_Index].u32_Aux_IvRegister;  // 20190829 yy
                    u32_IvRegister += uc_r;
                    if(((*u32_IvRegister) >>uw16_i) == 0X01)
                    {
                        if(ChannelInfo_Manager::ChannelInfo[uw16_i].ScheduleStatus == GOSTOP)
                            continue;
                        ChannelInfo_Manager::ChannelInfo[uw16_i].ScheduleStatus=GOSTOP;
                        Schedule_Manager_task::SendFinishFlag[uw16_i].StopStatus = STOP_EMERGENCY;
                        Schedule_Manager_task::SchTimeTrigger[uw16_i]=0;  //清超时标志位
                    }
                }

            }
        }
        uc_Error = 1;
#endif
        break;
    case ERROR_QUEUE_FAIL:
        uc_Error = 1;     //  记录相关错误信息，待通信恢复后上传。
        switch(St_Unit[uc_Index].uc_ErrorCode)
        {
        case UDP_QUEUE_SENDWINDOW_FULL:
        case UDP_QUEUE_UNIT_FULL:
            break;
        default:
            break;
        }
        break;
    case ERROR_TRANSFER_FAIL:
        uc_Error = 1;     //  记录相关错误信息，待通信恢复后上传。
        switch(St_Unit[uc_Index].uc_ErrorCode)
        {
        case UDP_TRANSFER_BUFFER_FULL:
        case UDP_TRANSFER_OFF_LINE:
        case UDP_TRANSFER_GREAT_RESEND:
            break;
        default:
            break;
        }
        break;
    case ERROR_RECEIVE_FAIL:
        uc_Error = 1;     //  记录相关错误信息，待通信恢复后上传。
        switch(St_Unit[uc_Index].uc_ErrorCode)
        {
        case UDP_RECEIVE_BUFFER_FULL:
        case UDP_RECEIVE_UNKNOW_UNIT:
            break;
        default:
            break;
        }
        break;
    case ERROR_OTHER:
        uc_Error = 1;
        break;
    default:
        break;
    }

    /*   if(uc_Error)
       {
           Un_ERROR_MESSAGE * st_SPB;
           st_SPB = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;
           st_SPB->m_IV_ERROR.m_btChannel_ID = 0;
           st_SPB->m_IV_ERROR.m_btCode_Type = St_Unit[uc_Index].uc_ErrorType;
           st_SPB->m_IV_ERROR.m_btType_ErrorNum = St_Unit[uc_Index].uc_ErrorCode;
           st_SPB->m_IV_ERROR.m_btSubCode = CTRL_VALUE_ERROR_Null;
           EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
       }*/
    St_Unit[uc_Index].uc_ErrorType = ERROR_NONE;
    St_Unit[uc_Index].uc_ErrorCode = UDP_NONE;
}

void MultiMcuComm::AbsolveDieLockProcess(unsigned char uc_Index)
{
    // 解除死锁处理流程
//     unsigned char uc_ON = 0;
    if(IS_QUEUE_FULL(WaitQueue[uc_Index],MCU_MAX_WAITBUFFER_SIZE))
    {
        if(!St_Unit[uc_Index].uc_ConnectSuccess)
        {
            if(St_Unit[uc_Index].uc_ReConnectCounter >= MCU_MAX_RECONNECT_NUM)
            {
                if(++uw16_AbsolveDieLockCounter[uc_Index] > MCU_MAX_DIELOCKNUM)  // 2分钟
                {
                    uw16_AbsolveDieLockCounter[uc_Index] = 0;
                    INIT_QUEUE(WaitQueue[uc_Index]);
                    for(unsigned char uc_j=0; uc_j<MCU_MAX_NUMBER; uc_j++)
                    {
                        INIT_QUEUE(SendWindowQueue[uc_j]);
                        //INIT_QUEUE(ReSendQueue);
                    }
                }
            }
        }
    }
    else
    {
        uw16_AbsolveDieLockCounter[uc_Index] = 0;
    }
}

void MultiMcuComm::ReceiveData_ToBuffer (U8 *buf, U16 len, U8 *remip, U16 port)
{
    uc_UDP_SendLock = 1;
    //U16 uw16_Lenth = (buf[19]<<8) + buf[18];
    //uw16_Lenth = uw16_Lenth>>5;
    U16 uw16_Lenth = (buf[MCU_UDP_SN_PACKHEAD-1]<<8) + buf[MCU_UDP_SN_PACKHEAD-2];
    if(uw16_Lenth > MAX_PACKLIMIT_SIZE_UDP)
    {
        uc_UDP_SendLock = 0;
        return;
    }

//    if(buf[3] != EthernetParse::uw16_MyIP[1]>>8)      // 暂时测试用PC广播包 MCU_UDP_BROADCAST
//         return;    // 暂时测试用PC广播包 MCU_UDP_BROADCAST

    for(unsigned char uc_i=0; uc_i<8; uc_i++)
    {
        if(buf[uc_i] != 0XBB)
        {
            uc_UDP_SendLock = 0;
            return;
        }
    }

    unsigned char DestinationIp = 0;
    DestinationIp = *(remip + 7);
    if(DestinationIp == 255)  // BroadCast
    {
#if  USE_AS_AUXMCU == 1
        ReceiveData_ToBroadCastBuffer(buf, len);
#endif
        uc_UDP_SendLock = 0;
        return;
    }

    unsigned char Ip = 0;
    Ip = *(remip + 3);
    Word16 w16_index = Seek_Unit(Ip);
    if(w16_index < 0)
    {
        uc_UDP_SendLock = 0;
        return;
    }

    unsigned char uc_Error = 0;
    if(IS_QUEUE_FULL(ReceiveWindowQueue[w16_index],MCU_MAX_RECEIVEBUFFER_SIZE))
        uc_Error = 1 ;    // 需要出错处理

    St_MCU_WAIT_MQueue  st_W;
    Word16  w16_P = FindReceiveBuffer(w16_index); // 查实体数据存储
    if(w16_P >= 0)
        st_W.uw16_PackAddress = w16_P;
    else
        uc_Error = 1;

    if(uc_Error)
    {
        St_Unit[w16_index].uc_ErrorType = ERROR_QUEUE_FAIL;
        St_Unit[w16_index].uc_ErrorCode = UDP_RECEIVE_BUFFER_FULL;

        PEEKQUEUE(ReceiveWindowQueue[w16_index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W,uc_Error);
        w16_P = st_W.uw16_PackAddress;// 保持最新的在最后面
        memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[MCU_UDP_SN_PACKHEAD],St_ReceiveBuffer[w16_index][w16_P].st_PackMess.uw16_PackLength); // 实体数据存储
        uc_UDP_SendLock = 0;
        return;
    }

    St_MCU_WindowMessage * winMess = (St_MCU_WindowMessage *) &buf[8];
    //st_W.uw16_PackLength = winMess->uw16_PackLength;
    St_ReceiveBuffer[w16_index][w16_P].st_PackMess = * winMess;
    if(St_ReceiveBuffer[w16_index][w16_P].st_PackMess.uw16_PackLength > MCU_MAX_CMD_BUFFER_SIZE)
    {
        uc_UDP_SendLock = 0;
        return;
    }
    ENQUEUE(ReceiveWindowQueue[w16_index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W);  //  窗口队列管理
    St_ReceiveBuffer[w16_index][w16_P].uc_IfSaveFlag = 1;
    St_ReceiveBuffer[w16_index][w16_P].uc_DestID = Ip;
    //St_ReceiveBuffer[w16_index][w16_P].uw16_ReSendCount = 0;
    memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[MCU_UDP_SN_PACKHEAD],St_ReceiveBuffer[w16_index][w16_P].st_PackMess.uw16_PackLength); // 实体数据存储

    //ReceiveProcess(w16_index);  // 需要加中断锁
    //TransferProcess(w16_index);
    uc_UDP_SendLock = 0;
}

void MultiMcuComm::ReceiveData_ToBroadCastBuffer (U8 *buf, U16 len)
{
    uc_UDP_SendLock = 1;
//    if(buf[3] != EthernetParse::uw16_MyIP[1]>>8)      // 暂时测试用PC广播包 MCU_UDP_BROADCAST
//         return;    // 暂时测试用PC广播包 MCU_UDP_BROADCAST

//     unsigned char Ip = 0;
//     Ip = *(remip + 3);
//     if(Ip == 255)
//     {
//         uc_UDP_SendLock = 0;
//         return;
//     }

    //St_MCU_WindowMessage * winMess = (St_MCU_WindowMessage *) &buf[8];

    //memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[20],12); // 实体数据存储

    //ReceiveProcess(w16_index);  // 需要加中断锁
    //TransferProcess(w16_index);

    memcpy(&uc_udp_BroadCastdata[0],&buf[MCU_UDP_SN_PACKHEAD],len);//12
    MultiMcuComm::NET_Rx_Msg(&uc_udp_BroadCastdata[0],1,len);
    CANFunc::NETtoCAN_RxProcess();
    // CANBusParse::CANParse(RUDPNET);

    uc_UDP_SendLock = 0;
}

short int MultiMcuComm::FindReceiveBuffer(unsigned char uc_Index)
{
    // 查找空的接收缓冲，以便能存储和方便处理数据信息
    UWord16 uw16_i;
    for(uw16_i=0; uw16_i<MCU_MAX_RECEIVEBUFFER_SIZE; uw16_i++)
    {
        if(!St_ReceiveBuffer[uc_Index][uw16_i].uc_IfSaveFlag)
        {
            return uw16_i;
        }
    }

    if(uw16_i >= MCU_MAX_RECEIVEBUFFER_SIZE)
    {
        return -1;  //  无法找到出错处理
    }
    return -1;
}

unsigned char MultiMcuComm::UpdataConfirmStarNum(unsigned char uc_Index,St_MCU_WindowMessage  *  Message)
{
    unsigned char uc_check = 0;
    St_MCU_WindowMessage  Mess = *  Message;
    St_Unit[uc_Index].uc_ErrorSeqNumCounter++;
    if(St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE >= Mess.uw16_SeqNum) // 对方发送包号
    {
        if(St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE == Mess.uw16_SeqNum) // 比对是否为自己需要的同步包号
        {
            uc_check = 1;
            St_Unit[uc_Index].uc_ErrorSeqNumCounter = 0;
            St_Unit[uc_Index].uw16_ConfirmEndNum = Mess.uw16_SeqNum;
            if(St_Unit[uc_Index].uw16_ConfirmEndNum >= MCU_MAX_SEQUENCE_NUMBER)
            {
                Mess.uw16_SeqNum = Mess.uw16_StarNum;  // 自己需要的同步包号到限制值，反转处理
                MCU_ConfirmProcess(uc_Index,&Mess);

                Mess.uw16_StarNum = MCU_MAX_SEQUENCE_NUMBER;//Message->uw16_SeqNum;
                Mess.uw16_EndNum = MCU_MAX_SEQUENCE_NUMBER;
                Mess.uw16_SeqNum = Message->uw16_SeqNum;
                MultiMcuComm::MCU_SendConfirm(&Mess);
                St_Unit[uc_Index].uw16_ConnectNum<MCU_MAX_SEQUENCE_NUMBER?St_Unit[uc_Index].uw16_ConnectNum++:St_Unit[uc_Index].uw16_ConnectNum=1;
                St_Unit[uc_Index].uw16_ConfirmStarNum = 0;
                St_Unit[uc_Index].uw16_ConfirmEndNum = 0;
                St_Unit[uc_Index].uw16_SeqNum = 1;
                St_Unit[uc_Index].uw16_StarNum = 1;
                St_Unit[uc_Index].uw16_EndNum = 1;
                MCU_ConfirmBuff_UpdataSeqNum(uc_Index);
            }
            St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter = 0;
        }
        St_Unit[uc_Index].uc_ReceiveTimeEN_On = true;
        St_Unit[uc_Index].uw16_ConfirmStarNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ReceiveWaitOutTime1); // 更新时间间隔器
    }
    else
    {
        Mess.uw16_SeqNum = St_Unit[uc_Index].uw16_ConfirmStarNum + MCU_LIMITSENDSIZE;
        Mess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
        Mess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
        Mess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
        MCU_SendAsk(&Mess);
    }

    if(Mess.uc_IfFirstFlag == 0)
    {
        if(St_Unit[uc_Index].uc_ErrorSeqNumCounter >= MAX_RESENDPACK_NUM)
        {   // 对重发包处理
            uc_check = 1; // 提前执行一次重连前的数据处理
            St_Unit[uc_Index].uc_ErrorSeqNumCounter = 0; // 对多次重发，但又无法对上同步号的特殊处理
            Mess.uw16_StarNum = Message->uw16_SeqNum;
            Mess.uw16_EndNum = Message->uw16_SeqNum;
            Mess.uw16_SeqNum = Message->uw16_SeqNum;

            St_Unit[uc_Index].uw16_ConfirmEndNum = Message->uw16_SeqNum;
            if(St_Unit[uc_Index].uw16_ConfirmEndNum >= MCU_MAX_SEQUENCE_NUMBER)
            {
                St_Unit[uc_Index].uw16_ConfirmEndNum = 0;
            }
            St_Unit[uc_Index].uw16_ConfirmStarNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
            if(Message->uw16_ConnectNum != St_Unit[uc_Index].uw16_ConnectNum)
            {
                St_Unit[uc_Index].uc_ConnectSuccess  = false;   // 已经失控了，重新连接UDP
                uw32_Time_Connect = MCU_MAX_RECONNECT_TIME/10;
            }
            else
                MultiMcuComm::MCU_SendConfirm(&Mess);  // 同样回一个确认给对方
        }
    }
    return uc_check;
}

void MultiMcuComm::ProduceMessageToSendBuffer(unsigned char uc_Index,unsigned int DataLen)
{
    // 生成报文
    if(IS_QUEUE_EMPTY(WaitQueue[uc_Index]))
        return;

//     unsigned char bOK = 0;
//     unsigned long int  QLength = WaitQueue[uc_Index].m_u32Length;
    short int Result = 0;

    //满足一包或已超时
//     if(QLength >= MCU_MAX_CMD_ITEM_SIZE)
//     {
//         bOK = 1;
//     }
//
//     if(CommonFunc::GetTimeTicks( &m_PackProduceTime[uc_Index]) >= MCU_MAX_PRODUCE_PACKTIME)    //超过4ms
//     {
//         bOK = 1;
//         CommonFunc::GetTimeTicksUpdate( &m_PackProduceTime[uc_Index]); // 更新时间间隔器
//     }
//
//     if(bOK)
    {
        Result = MoveWaitBufferToSendBuffer(uc_Index,1);
        if(Result < 1)
            St_Unit[uc_Index].uc_SendTimeEN_On = true;
    }
}

void MultiMcuComm::PackToNetLinkRoad(void)
{
    if(uc_SlicingEnable)
        return;

    uc_UDP_SendLock = 1;
    NVIC_DisableIRQ(ENET_IRQn);
    //main_TcpNet(); //  网络管理
    //while(!(IS_QUEUE_EMPTY(CharMGE))) // 对上次无法发送到物理层的数据重发。
    if(!(IS_QUEUE_EMPTY(CharMGE)))
    {
        unsigned long int uw32_length,uw32_SL,uw32_L_End;
        unsigned char uc_Success = 0;
        unsigned char * uc_p;
        unsigned char bOK;
        St_MCU_CharMGE  st_CharMge;

        DEQUEUE(CharMGE, MAX_CHARMGE_SIZE, st_CharMge, bOK);
        uw32_length = st_CharMge.uw16_DLength;
        uc_p = (unsigned char *) &CharSend +  CharSend.m_u32Start;   // 解决位置从0开始存储，长度从1开始计数的问题。
        uw32_SL = CharSend.m_u32Start+uw32_length;
        if(uw32_SL>MAX_CHAR_SIZE)  // 如果出现回滚需要特别处理
        {
            uw32_L_End = MAX_CHAR_SIZE - CharSend.m_u32Start;
            if(uw32_L_End > uw32_length)  // 尾部反转出错，放弃这个包处理
            {
                CharSend.m_u32Length -= uw32_length;   // DEQUEUE(CharSend, MAX_CHAR_SIZE
                CharSend.m_u32Start = uw32_SL%MAX_CHAR_SIZE;  // 发送字节发送区成功就移动释放
                NVIC_EnableIRQ(ENET_IRQn);
                uc_UDP_SendLock = 0;
                return;
            }
            memcpy(uc_CmdBuffer_Temp,uc_p,uw32_L_End);
            memcpy(&uc_CmdBuffer_Temp[uw32_L_End],&CharSend,uw32_length - uw32_L_End);
            uc_p = (unsigned char *) &uc_CmdBuffer_Temp;
        }
        uc_Success = UdpRxTx::udp_SendToLink(uc_p,uw32_length, st_CharMge.uc_DestID, MCU_PORT_M);//st_CharMge.uw16_DestPort);

        if(!uc_Success)
        {
            if(++uw16_CharFailCounter >= MCU_MAX_SENDCHARFAIL_NUM)
            {   // 不存在的MCU，从系统中删除
                uc_Success = 1;
                //MCU_Connect_Delete(st_CharMge.uc_DestID);
            }
            else
            {
                CharMGE.m_u32Length ++;
                CharMGE.m_u32Start=(CharMGE.m_u32Start >= 1)?(CharMGE.m_u32Start-1):(MAX_CHARMGE_SIZE-1); // 发送字节发送区失败就还原不变
            }
            //break;
        }

        if(uc_Success)
        {
            uw16_CharFailCounter = 0;
            CharSend.m_u32Length -= uw32_length;   // DEQUEUE(CharSend, MAX_CHAR_SIZE
            CharSend.m_u32Start = uw32_SL%MAX_CHAR_SIZE;  // 发送字节发送区成功就移动释放

        }
        //break;   //  一毫秒一发送
    }

    //main_TcpNet(); //  网络管理
    NVIC_EnableIRQ(ENET_IRQn);
    uc_UDP_SendLock = 0;
}

unsigned char MultiMcuComm::PackToNetSendByte(unsigned char UnitID,unsigned char * Array,unsigned short int uw16_length)
{
    if((CharSend.m_u32Length+uw16_length>MAX_CHAR_SIZE)||(CharMGE.m_u32Length+1>MAX_CHARMGE_SIZE))
    {
        return 0;
    }

//    *(Array+3) = UnitID;    // 暂时测试用PC广播包  MCU_UDP_BROADCAST
    uc_UDP_SendLock = 1;
    NVIC_DisableIRQ(ENET_IRQn);
    ENQUEUEARRAY(CharSend, MAX_CHAR_SIZE, Array, uw16_length);
    St_MCU_CharMGE  st_CharMge;
    st_CharMge.uc_DestID = UnitID;
    //st_CharMge.uw16_DestPort = MCU_PORT_M;
    st_CharMge.uw16_DLength = uw16_length;
    //st_CharMge.uw32_ByteLength = CharSend.m_u32Start+CharSend.m_u32Length;
    ENQUEUE(CharMGE, MAX_CHARMGE_SIZE, st_CharMge);
    NVIC_EnableIRQ(ENET_IRQn);
    uc_UDP_SendLock = 0;
    return 1;
}

short MultiMcuComm::MCU_ConfirmBuff_FindMin(unsigned char uc_Index)
{
    unsigned short uw16_counter[2]= {0,0};
    unsigned short uw16_Num[2];
//     unsigned short uw16_ConnectNum_Min = 0;
    unsigned short uw16_SeqNum_Min[2] = {0,0};

    for(unsigned short uw16_i = 0; uw16_i < MCU_MAX_CONFIRMBUFFER_SIZE; uw16_i ++)
    {
        if(St_ConfirmBuffer[uc_Index][uw16_i].uc_IfSaveFlag)
        {
            if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum < St_Unit[uc_Index].uw16_ConnectNum)  // 先扫连接号
            {   // 跨越不同连接号处理
//                 uw16_ConnectNum_Min = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum;
                if(uw16_SeqNum_Min[0] == 0)
                    uw16_SeqNum_Min[0] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum <= uw16_SeqNum_Min[0])  // 再扫请求号,查最小的请求号
                {
                    uw16_counter[0]++;
                    uw16_Num[0] = uw16_i;
                    uw16_SeqNum_Min[0] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                }
            }
            else
            {   // 同一连接号处理
                if(uw16_SeqNum_Min[1] == 0)
                    uw16_SeqNum_Min[1] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum <= uw16_SeqNum_Min[1])  // 再扫请求号,查最小的请求号
                {
                    uw16_counter[1]++;
                    uw16_Num[1] = uw16_i;
                    uw16_SeqNum_Min[1] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                }
            }
        }
    }

    if((uw16_counter[0] == 0)&&(uw16_counter[1] == 0))
        return -1;

    if(uw16_counter[0] > 0)
    {   // 有跨越不同连接号处理
        return uw16_Num[0];
    }
    else
    {
        return uw16_Num[1];
    }
}

void MultiMcuComm::MCU_ConfirmBuff_UpdataSeqNum(unsigned char uc_Index)
{
    unsigned short uw16_counter =0;
    short w16_Num = 0;
    unsigned short uw16_Num[MCU_MAX_CONFIRMBUFFER_SIZE];

    for(unsigned short uw16_i = 0; uw16_i < MCU_MAX_CONFIRMBUFFER_SIZE; uw16_i ++)
    {
        w16_Num = MCU_ConfirmBuff_FindMin(uc_Index);  // 查最小的请求号
        if(w16_Num >= 0)
        {
            uw16_Num[uw16_counter++] = w16_Num;
            St_ConfirmBuffer[uc_Index][w16_Num].uc_IfSaveFlag = 0;
        }
    }

    for(unsigned short uw16_i = 0; uw16_i < uw16_counter; uw16_i ++)
    {
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].uc_IfSaveFlag = 1;
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].uw16_ReSendCount = 0;
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].st_PackMess.uw16_SeqNum = St_Unit[uc_Index].uw16_SeqNum;
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].st_PackMess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].st_PackMess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
        St_ConfirmBuffer[uc_Index][uw16_Num[uw16_i]].st_PackMess.uw16_ConnectNum = St_Unit[uc_Index].uw16_ConnectNum;
        St_Unit[uc_Index].uw16_StarNum = St_Unit[uc_Index].uw16_SeqNum;
        St_Unit[uc_Index].uw16_EndNum = St_Unit[uc_Index].uw16_SeqNum;
        St_Unit[uc_Index].uw16_SeqNum ++;
        if(St_Unit[uc_Index].uw16_EndNum >= MCU_MAX_SEQUENCE_NUMBER)
        {
            // 异常处理
            St_Unit[uc_Index].uw16_EndNum = MCU_MAX_SEQUENCE_NUMBER;
        }
    }
}

// void MultiMcuComm::MCU_Connect_Delete(unsigned char uc_DestID)
// {
//     REMOTEM rm_UDP;
//     memcpy(rm_UDP.IpAdr, uc_remotemIpAdr, IP_ADRLEN);
//     rm_UDP.IpAdr[3] = uc_DestID;
//     if(!arp_get_info (& rm_UDP))
//     {
//         for(unsigned short uw16_i=0; uw16_i<uc_UnitNewTable_Total; uw16_i++)
//         {
//             if((st_UnitNewTable[uw16_i].w16_D_IP == uc_DestID)
//                 &&(St_Unit[st_UnitNewTable[uw16_i].uc_Address].uc_ID == uc_DestID))
//             {
//                 St_Unit[st_UnitNewTable[uw16_i].uc_Address].uc_Active = 0;
//                 Build_NewTable();   // 不存在的MCU，并且ARP表中也不存在，则需要从系统中删除它。
//             }
//         }
//     }
// }
