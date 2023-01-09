/*****************************************************************************
// �����ļ�                   :MCUCOMMUNICATION.cpp     ��Ҫ����MCU��UDPͨ��
// �ļ�����                   :Ϊ MCU board����
// ��д����(MM.DD.YYYY)       :04.26.2016
*****************************************************************************/

#include "../Entry/includes.h"

UInt16 MultiMcuComm::Priority = PRIORITY4;
UInt16 MultiMcuComm::TaskID = MULTIMCUCOMMUNICATION_TASK;

// ���ñ������������
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

WaitQueue2          MultiMcuComm::WaitQueue[MCU_MAX_NUMBER];            // ���ʹ��ڶ��и�������,�൱��ָ��
SendWindow2         MultiMcuComm::SendWindowQueue[MCU_MAX_NUMBER];      // ���ʹ��ڶ��и�������,�൱��ָ��
ReceiveWindow2      MultiMcuComm::ReceiveWindowQueue[MCU_MAX_NUMBER];   // ���մ��ڶ��и�������,�൱��ָ��

St_MCU_Data 	        MultiMcuComm::St_WaitBuffer[MCU_MAX_NUMBER][MCU_MAX_WAITBUFFER_SIZE];     // �������Ļ�����
St_MCU_InfoPack 	    MultiMcuComm::St_ReceiveBuffer[MCU_MAX_NUMBER][MCU_MAX_RECEIVEBUFFER_SIZE];     // ���ձ��Ļ�����
St_MCU_InfoPack         MultiMcuComm::St_ConfirmBuffer[MCU_MAX_NUMBER][MCU_MAX_CONFIRMBUFFER_SIZE];  // ȷ�����ݱ��Ļ�����

unsigned char       MultiMcuComm::uc_SlicingStatus;                               // MCU��������״̬
unsigned char       MultiMcuComm::uc_SlicingEnable;                               // MCU��������ʹ��

// MsTime              MultiMcuComm::m_PackProduceTime[MCU_MAX_NUMBER];              // ���ɱ��ļ��ʱ��
MsTime              MultiMcuComm::m_SlicingOutTime;                               // MCU����ʱ�����Ƴ�ʱ
// unsigned short int  MultiMcuComm::uw16_LimitSendWindowSize_m;     // ���Ʒ��ʹ��ڴ�С����
// unsigned short int  MultiMcuComm::uw16_AddSendWindowSize_n;       // ���ӷ��ʹ��ڴ�С����
unsigned short int  MultiMcuComm::uw16_AbsolveDieLockCounter[MCU_MAX_NUMBER];

// unsigned char       MultiMcuComm::uc_Scan_Index;
St_NewTable         MultiMcuComm::st_UnitNewTable[MCU_MAX_NUMBER];
unsigned char       MultiMcuComm::uc_UnitNewTable_Total;

unsigned short int         MultiMcuComm::uw16_CharFailCounter;   // �ֽڷ���ʧ�ܼ�����
CharSend1       MultiMcuComm::CharSend;         // �ֽڷ��Ͷ���
CharMGE2        MultiMcuComm::CharMGE;          // �ֽڷ��Ͷ��и�������,�൱��ָ��
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


#pragma arm section //��������������

extern          uint8 	   uc_UDP_SendLock;

// ��ʱ������PC�㲥��  MCU_UDP_BROADCAST

void MultiMcuComm::MyInit(void)
{
    //memset((void *)&St_Unit,0,sizeof(St_MCU_Unit));  // ��ɨ��MAP�����ٳ�ʼ��
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
        //St_Unit[uc_j].uc_Active  = false;    // �����г�ʼ������MAP����أ������ڴ˳�ʼ����Ҫ��Ϣ��
        uw16_AbsolveDieLockCounter[uc_j] = 0;
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime1); // ����ʱ������
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime2); // ����ʱ������
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_WaitConfirmOutTime3); // ����ʱ������
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime1); // ����ʱ������
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime2); // ����ʱ������
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ReceiveWaitOutTime3); // ����ʱ������
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_j].m_ConfirmConnectOutTime); // ����ʱ������
        St_Unit[uc_j].uw16_ReceiveWaitOutTime1_Counter = 0;
    }
	MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_NORMAL;

//     OS_ServeProcess::GetTimeTicksUpdate( &m_PackProduceTime); // ����ʱ������
    CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime);  // ����ʱ������
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

    main_TcpNet(); //  �������

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
        unsigned long uw32_time = (uc_UnitNewTable_Total/5)*MCU_MAX_PRODUCE_PACKTIME; // ����5��MCU���ٷ���
        if(CommonFunc::GetTimeTicks( &m_SlicingOutTime) >= uw32_time)    //����4ms // ��ʱ������
        {
            //NVIC_DisableIRQ(ENET_IRQn);
            MultiMcuComm::PackToNetLinkRoad();
            //NVIC_EnableIRQ(ENET_IRQn);
            CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // ����ʱ������
        }
    }

    main_TcpNet(); //  �������

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_MCU_RUDP] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_MCU_RUDP);
#endif
}

int MultiMcuComm::PushingPackToWaitBuffer(uint8 uc_DestID, uint8 * content, unsigned int DataLen)// ,unsigned char VitalFlag)
{
    // װ�����ݽ���ȴ�������
    Word16 w16_index = Seek_Unit(uc_DestID);   // ��� �Ż�20170227
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
    ENQUEUE(WaitQueue[w16_index],MCU_MAX_WAITBUFFER_SIZE,st_W);  // ���й���


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

    // �ӵȴ�������װ�����ݽ��뷢�ͻ�����
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
            // this is full.  ��Ϊ������
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
                memcpy(&St_ConfirmBuffer[uc_Index][w16_P].m_Data[st_W.st_Address.uw16_PackLength],&St_WaitBuffer[uc_Index][uw16_Num].m_UDPWaitBuffer[0],St_WaitBuffer[uc_Index][uw16_Num].uw16_PackLength);   //  ����ʵ����
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
            St_ConfirmBuffer[uc_Index][w16_P].m_btNone_11[uc_i] = 0XBB;    // ��ͷǰ���8��"0XBB"

        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_StarNum = St_Unit[uc_Index].uw16_ConfirmStarNum;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_EndNum = St_Unit[uc_Index].uw16_ConfirmEndNum;
        St_ConfirmBuffer[uc_Index][w16_P].st_PackMess.uw16_ConnectNum= St_Unit[uc_Index].uw16_ConnectNum;

        st_W.st_Address.uc_IfSaveFlag = 1;
        st_W.st_Address.uw16_SeqNum = St_Unit[uc_Index].uw16_SeqNum;  //  ���ڶ��й���
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
    // ���ҿյ�ȷ�ϻ��壬�Ա��ܴ洢�ͷ��㷢��������Ϣ
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
        return -1;  //  �޷��ҵ�������
    }
}

short int MultiMcuComm::Seek_Unit(uint8 uc_DestID)
{
    // ����Unit���λ��
    unsigned short uw16_i;
    for(uw16_i=0; uw16_i<uc_UnitNewTable_Total; uw16_i++)
    {
        if(st_UnitNewTable[uw16_i].w16_D_IP == uc_DestID) //  ���±�
        {
            return st_UnitNewTable[uw16_i].uc_Address;
        }
    }

    if(uw16_i >= uc_UnitNewTable_Total)
    {
        return -1;  //  �޷��ҵ�������
    }
}

void MultiMcuComm::Build_NewTable(void)
{
    // �����±��������ID
    unsigned short uw16_i;
    uc_UnitNewTable_Total = 0;
    for(uw16_i=0; uw16_i<MCU_MAX_NUMBER; uw16_i++)
    {
        st_UnitNewTable[uw16_i].uc_Address = 0;
        st_UnitNewTable[uw16_i].w16_D_IP = -1;  // �����κ������������ɾ��±�
    }

    unsigned char uc_n = 0;
    REMOTEM rm_UDP;
    for(uw16_i=0; uw16_i<MCU_MAX_NUMBER; uw16_i++)
    {
        if(St_Unit[uw16_i].uc_Active) //  ���
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
            main_TcpNet(); //  �������
        }
    }
    uc_UnitNewTable_Total = uc_n;



}

short int MultiMcuComm::ReSendSingleMessageProcess(unsigned char uc_Index,UWord16 uw16_SN)
{
    // �ط�������
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
            St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_StarNum = St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount;// Ϊ�����ط��ж����Ӵ���20170401
            if(St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount >= MAX_RESENDPACK_NUM)
            {
                St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount = 0;
                St_Unit[uc_Index].uc_ConnectSuccess  = false;   // ����ط�ʧ�ܺ���������UDP
                uw32_Time_Connect = MCU_MAX_RECONNECT_TIME/10;
            }
            else
            {
                if((St_ConfirmBuffer[uc_Index][uw16_i].uw16_ReSendCount&0X0007 )>= 0X0007)
                {
                    uc_SlicingStatus = SLICING_STATUS_TIME;   // ÿ7������һ��ͨ�� 350ms=7*50
                    CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // ����ʱ������
                }
            }
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // ����ʱ������
            unsigned char * uc_p = (unsigned char *) &St_ConfirmBuffer[uc_Index][uw16_i].m_btNone_11[0];  // ����ʧ�ܻ������ֽڷ�����
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
        W16_result = -1;  //  �޷��ҵ�������
    }

    return W16_result;
}

void MultiMcuComm::MCU_ReconnectToMCU(unsigned char uc_Index)
{
    // �ؽ���������
    St_Unit[uc_Index].uc_SendPackCounter = 0;
    if(St_Unit[uc_Index].uc_ConnectSuccess)
    {
        St_Unit[uc_Index].uc_ReConnectCounter = 0XFF;
        //CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ConfirmConnectOutTime); // ����ʱ������
    }
    else
    {
        UWord32 uw32_Time = uw32_Time_Connect + 50*(EthernetParse::uw16_MyIP[1]>>8);
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_ConfirmConnectOutTime) >= uw32_Time)    //����5s
        {
            // �����ؽ����ӱ���
            if(++St_Unit[uc_Index].uc_ReConnectCounter > MCU_MAX_RECONNECT_NUM)  // 25s�任һ�����Ӻ�
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
            if(!arp_get_info (& rm_UDP))  // ����������ַ���Ȼ�ȡ�����ַ��
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
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ConfirmConnectOutTime); // ����ʱ������

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
    for(unsigned char uc_index=0; uc_index<2; uc_index++) //  CAN1,CAN2�������á��๦�ܰ��н���CAN2�Խӿͻ�BMS���.    <st_CANBMS_Map->m_AllChnNum
    {
        if(st_CANBMS_Map->m_CANmap[uc_index].m_bIVActive)
        {
            Scan_EmptySeat(st_CANBMS_Map->m_CANmap[uc_index].m_AuxUnitID,st_CANBMS_Map->m_CANmap[uc_index].m_IVChnNo,UDP_MAP_CAN_BMS);   // �ȵ�BMS MAP���ٵ�AUX MAP

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
    {   //  �����Master���ϼ�ͨ�������Լ��������SubMaster���ϼ�ͨ������master������� slave���ϼ�ͨ������ Sub master��
        if(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Master)
        {   // ע��(Master��)������¼�
            for(unsigned char uc_j = 0; uc_j < IVSDL::m_GrpInfo4SDL[uc_i].SubmastCount-1; uc_j++)
            {
                m_SeekUnitID = IVSDL::m_GrpInfo4SDL[uc_i].Submaster[uc_j].CanID;
                Scan_EmptySeat(m_SeekUnitID,uc_i,UDP_MAP_IV_PARALLEL);
            }
        }
        else if((IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Submaster)||(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Serial)||(IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Booster))
        {   // ע��(Submaster�ӵ�����Slave��)����ĵ��ϼ�
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

        if((UnitID == St_Unit[uc_n].uc_ID)&&St_Unit[uc_n].uc_Active)  // �������һ��
        {
            m_find_out = 1;   // �鵽�����Ѿ��м�¼
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
            if(!St_Unit[uc_n].uc_Active) //  �鵽����δ�м�¼�����Ҹ���λ�ü�¼����
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
        if(St_Unit[uc_n].uc_Active && ((uc_m&Map)==Map))  // �������һ��
        {
            if((uc_m^Map)==0) // ����ûʹ�ã�������ʹ��ֱ������
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
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_WaitConfirmOutTime1); // ����ʱ������
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_ReceiveWaitOutTime1); // ����ʱ������
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_n].m_ConfirmConnectOutTime); // ����ʱ������
                St_Unit[uc_n].m_ConfirmConnectOutTime.Second -= MCU_MAX_RECONNECT_TIME/10000*4; //20s �������,����������
                St_Unit[uc_n].uw16_ReceiveWaitOutTime1_Counter = 0;
                uw16_AbsolveDieLockCounter[uc_n] = 0;

                //INIT_QUEUE(SendWindowQueue[uc_n]);
                //INIT_QUEUE(WaitQueue[uc_n]);
                //INIT_QUEUE(ReceiveWindowQueue[uc_n]);
            }
						else//���map�������ǰmap // yzl 2020.10.29
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
    main_TcpNet(); //  �������
    if(IS_QUEUE_EMPTY(ReceiveWindowQueue[uc_Index]))
        return;

    Word16  w16_P;
    unsigned char bOk;
    St_MCU_WAIT_MQueue  st_W;
    do {
        NVIC_DisableIRQ(ENET_IRQn);
        DEQUEUE(ReceiveWindowQueue[uc_Index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W,bOk);  //  ���ڶ��й���
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
                // �����һ�ν������е�����
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
                    St_Unit[uc_Index].uc_SendTimeEN_On = false; //�ӿ��ط�
                    St_Unit[uc_Index].m_WaitConfirmOutTime1.Second>=1?St_Unit[uc_Index].m_WaitConfirmOutTime1.Second--:St_Unit[uc_Index].m_WaitConfirmOutTime1.Second=0;
                    St_ConfirmBuffer[uc_Index][0].uw16_ReSendCount = 0;
                    MCU_ConfirmBuff_UpdataSeqNum(uc_Index);
                }
                else
                    St_Unit[uc_Index].uc_SendTimeEN_On = false;
            }
            break;
        case PACKET_STATUS_DATA:
            // ������ʱ��CAN��֡�Խ�
            unsigned char uc_feedback = 1;
            uc_feedback = UpdataConfirmStarNum(uc_Index,Message); // ������Ŵ���

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
// ���øú���������CAN����
unsigned char MultiMcuComm::NET_Push_to_Send(const MP_MCU_MSG McuMsg)
{
// 		if(CANFunc::IsCANopen())
// 			NVIC_DisableIRQ(CAN_IRQn);
    //����������ȷ���� Ȼ������¼����� �ȴ�ȷ��
    if(McuMsg.Un_CANID.m_CANID.DestID == 0XFF)  // �㲥��ת����Ե�
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
    //����������ȷ���� Ȼ������¼����� �ȴ�ȷ��
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
    //����������ȷ���� Ȼ������¼�����
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
    //memcpy(&uc_udp_data, &uc_d.uc_member, 4);    // 4+8=12�ֽ�CAN��

// 		uc_udp_data[12]='Z';
// 		uc_udp_data[13]='Z';
// 		uc_udp_data[14]='Z';
// 		uc_udp_data[15]='Z';

//     unsigned char uc_Success = 0;
//     unsigned char uc_L_Flag = 0;

//     UWord16 uw16_i = 0;

    St_MCU_Head * st_Head = (St_MCU_Head*)&uc_udp_Temp[0];
    st_Head->st_PackMess.uc_StatusFlag = PACKET_STATUS_DATA;

    for(unsigned char uc_i=0; uc_i<8; uc_i++) // 8�ֽ�CAN��
        st_Head->m_btNone_11[uc_i] = 0XBB;

    st_Head->st_PackMess.uw16_SeqNum =  1;   // 12�ֽ�CAN��
    st_Head->st_PackMess.uc_IfFirstFlag = 1;
    st_Head->st_PackMess.uw16_ConnectNum= 1;
    st_Head->st_PackMess.uw16_StarNum = 1;
    st_Head->st_PackMess.uw16_EndNum = 1;
    st_Head->st_PackMess.uw16_PackLength = 0;
    st_Head->st_PackMess.uc_SourceID = EthernetParse::uw16_MyIP[1]>>8;
    memcpy(&uc_udp_Temp[MCU_UDP_SN_PACKHEAD], &uc_udp_data[0], McuMsg.ByteLength);    // 4+8+4=16�ֽ�CAN��
    uw16_length = st_Head->st_PackMess.uw16_PackLength + MCU_UDP_SN_PACKHEAD + McuMsg.ByteLength;  // 20+16=36�ֽ�CAN��

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
    CommonFunc::GetTimeTicksUpdate( &St_Unit[uw16_Idx].m_ReceiveWaitOutTime1); // ����ʱ������
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // ����ʧ�ܻ������ֽڷ�����
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
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // ����ʧ�ܻ������ֽڷ�����
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
    unsigned char * uc_p = (unsigned char *) &uc_udp_Temp[0];  // ����ʧ�ܻ������ֽڷ�����
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
                CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // ����ʱ������
                //break;
                uw16_counter++;
            }

//             if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum == WinMess.uw16_EndNum+1)
//                uw16_index = uw16_i; // �����ط�����
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

    main_TcpNet(); //  �������
    //����֮ǰ��Ԥ����
    if(!IS_QUEUE_EMPTY(SendWindowQueue[uc_Index]))  //���ʹ��ڶ��в���
    {
//         QLength = MCU_MAX_WINDOW - SendWindowQueue[uc_Index].m_u32Length;
//         if(QLength >= MCU_LIMITSENDSIZE)
//             QLength = MCU_LIMITSENDSIZE;
//          do{
        u32_DataLength = QUEUE_DEPTH(SendWindowQueue[uc_Index]);
        if(u32_DataLength >= 1)   //���ʹ��ڶ��з��ͱ���
        {
            DEQUEUE(SendWindowQueue[uc_Index], MCU_MAX_SENDWINDOW_SIZE, st_D, bOK);
//                         unsigned char uc_Success = 0;
            unsigned long int uw32_length = st_D.st_Address.uw16_PackLength+MCU_UDP_SN_PACKHEAD;
            if(St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].uc_IfSaveFlag)
            {
                // ����ȷ�ϰ�����Ϣ
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

                    unsigned char * uc_p = (unsigned char *) &St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].m_btNone_11[0];;  // ����ʧ�ܻ������ֽڷ�����
                    unsigned char resultok = 0;
                    resultok = PackToNetSendByte(St_Unit[uc_Index].uc_ID,uc_p,uw32_length);
                    if(resultok)
                        St_Unit[uc_Index].uc_SendPackCounter++;
                    St_Unit[uc_Index].uc_SendTimeEN_On = true;
                    CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // ����ʱ������
//                                 OS_ServeProcess::GetTimeTicksUpdate( &m_PackProduceTime[uc_Index]); // ����ʱ������
                    St_ConfirmBuffer[uc_Index][st_D.st_Address.uw16_PackAddress].st_PackMess.uc_IfFirstFlag = 0;
                    St_Unit[uc_Index].uc_ReceiveTimeEN_On = false;
                }
            }
        }
//           }while(u32_DataLength>=1);//(u16_DataLength>=QLength);  // 1����һ�η��ͱ��
    }

    if(St_Unit[uc_Index].uc_SendPackCounter >= MCU_MAX_LIMITSEND_RECEIVEPACK)
        return;

    if(St_Unit[uc_Index].uc_SendTimeEN_On)
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_WaitConfirmOutTime1) >= MCU_MAX_SEND_OUTTIME1)    //����100ms
        {
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_WaitConfirmOutTime1); // ����ʱ������
            short w16_Num = MCU_ConfirmBuff_FindMin(uc_Index);
            if(w16_Num >= 0)
                ReSendSingleMessageProcess(uc_Index,St_ConfirmBuffer[uc_Index][w16_Num].st_PackMess.uw16_SeqNum);
        }

    if(St_Unit[uc_Index].uc_SendPackCounter >= MCU_MAX_LIMITSEND_RECEIVEPACK)
        return;

    if(St_Unit[uc_Index].uc_ReceiveTimeEN_On)
        if(CommonFunc::GetTimeTicks( &St_Unit[uc_Index].m_ReceiveWaitOutTime1) >= uw32_MaxTime_Confirm) //MCU_MAX_SEND_CONFIRM1)    //����6ms
        {
            CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ReceiveWaitOutTime1); // ����ʱ������
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

            if(++St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter >= MCU_MAX_SEND_ASK)    //����600ms
            {
                St_Unit[uc_Index].uw16_ReceiveWaitOutTime1_Counter = 1;
                Message->uw16_SeqNum = St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE;
                MCU_SendAsk(Message);
            }
        }

// 	main_TcpNet(); //  �������
//     MCUCommun::PackToNetLinkRoad();
//     main_TcpNet(); //  �������
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
//             uw16_LimitSendWindowSize_m = MCU_MAX_WINDOW;  // ����������
        uc_result = 1;
        break;
    case SLICING_STATUS_TIME:
        uc_SlicingEnable = true;
        if(CommonFunc::GetTimeTicks( &m_SlicingOutTime) >= MCU_MAX_PRODUCE_PACKTIME)    //����4ms // ��ʱ������
        {
            uc_SlicingEnable = false;
            uc_SlicingStatus = SLICING_STATUS_IDLE;
            CommonFunc::GetTimeTicksUpdate( &m_SlicingOutTime); // ����ʱ������
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
    // ������
    if(St_Unit[uc_Index].uc_ErrorType == ERROR_NONE)
        return;

    unsigned char  uc_Error = 0;
    unsigned short int  uw16_i = 0;
    switch(St_Unit[uc_Index].uc_ErrorType)
    {
    case ERROR_CONNECT_LOST:
        if(!St_Unit[uc_Index].uc_ConnectSuccess)  // δ����
        {
            uc_Error = 1;     //  ��¼��ش�����Ϣ����ͨ�Żָ����ϴ���
        }
        break;
    case ERROR_CHECK_FAIL:
        uc_Error = 1;     //  ��¼��ش�����Ϣ����ͨ�Żָ����ϴ���
        break;
    case ERROR_STOPSCH:
#if  USE_AS_AUXMCU == 0
        unsigned long int *  u32_IvRegister;
        for(uw16_i=0; uw16_i<MAXCHANNELNO; uw16_i++)
        {
            if(Schedule_Manager_task::IfSchRunning(uw16_i))           //���е�ͨ��Ҫִ��stop���������ؿ��Ǹ��ֻظ�����λ��ʱ�Ѿ�����
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
                        Schedule_Manager_task::SchTimeTrigger[uw16_i]=0;  //�峬ʱ��־λ
                    }
                }

            }
        }
        uc_Error = 1;
#endif
        break;
    case ERROR_QUEUE_FAIL:
        uc_Error = 1;     //  ��¼��ش�����Ϣ����ͨ�Żָ����ϴ���
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
        uc_Error = 1;     //  ��¼��ش�����Ϣ����ͨ�Żָ����ϴ���
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
        uc_Error = 1;     //  ��¼��ش�����Ϣ����ͨ�Żָ����ϴ���
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
    // ���������������
//     unsigned char uc_ON = 0;
    if(IS_QUEUE_FULL(WaitQueue[uc_Index],MCU_MAX_WAITBUFFER_SIZE))
    {
        if(!St_Unit[uc_Index].uc_ConnectSuccess)
        {
            if(St_Unit[uc_Index].uc_ReConnectCounter >= MCU_MAX_RECONNECT_NUM)
            {
                if(++uw16_AbsolveDieLockCounter[uc_Index] > MCU_MAX_DIELOCKNUM)  // 2����
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

//    if(buf[3] != EthernetParse::uw16_MyIP[1]>>8)      // ��ʱ������PC�㲥�� MCU_UDP_BROADCAST
//         return;    // ��ʱ������PC�㲥�� MCU_UDP_BROADCAST

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
        uc_Error = 1 ;    // ��Ҫ������

    St_MCU_WAIT_MQueue  st_W;
    Word16  w16_P = FindReceiveBuffer(w16_index); // ��ʵ�����ݴ洢
    if(w16_P >= 0)
        st_W.uw16_PackAddress = w16_P;
    else
        uc_Error = 1;

    if(uc_Error)
    {
        St_Unit[w16_index].uc_ErrorType = ERROR_QUEUE_FAIL;
        St_Unit[w16_index].uc_ErrorCode = UDP_RECEIVE_BUFFER_FULL;

        PEEKQUEUE(ReceiveWindowQueue[w16_index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W,uc_Error);
        w16_P = st_W.uw16_PackAddress;// �������µ��������
        memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[MCU_UDP_SN_PACKHEAD],St_ReceiveBuffer[w16_index][w16_P].st_PackMess.uw16_PackLength); // ʵ�����ݴ洢
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
    ENQUEUE(ReceiveWindowQueue[w16_index],MCU_MAX_RECEIVEBUFFER_SIZE,st_W);  //  ���ڶ��й���
    St_ReceiveBuffer[w16_index][w16_P].uc_IfSaveFlag = 1;
    St_ReceiveBuffer[w16_index][w16_P].uc_DestID = Ip;
    //St_ReceiveBuffer[w16_index][w16_P].uw16_ReSendCount = 0;
    memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[MCU_UDP_SN_PACKHEAD],St_ReceiveBuffer[w16_index][w16_P].st_PackMess.uw16_PackLength); // ʵ�����ݴ洢

    //ReceiveProcess(w16_index);  // ��Ҫ���ж���
    //TransferProcess(w16_index);
    uc_UDP_SendLock = 0;
}

void MultiMcuComm::ReceiveData_ToBroadCastBuffer (U8 *buf, U16 len)
{
    uc_UDP_SendLock = 1;
//    if(buf[3] != EthernetParse::uw16_MyIP[1]>>8)      // ��ʱ������PC�㲥�� MCU_UDP_BROADCAST
//         return;    // ��ʱ������PC�㲥�� MCU_UDP_BROADCAST

//     unsigned char Ip = 0;
//     Ip = *(remip + 3);
//     if(Ip == 255)
//     {
//         uc_UDP_SendLock = 0;
//         return;
//     }

    //St_MCU_WindowMessage * winMess = (St_MCU_WindowMessage *) &buf[8];

    //memcpy(&St_ReceiveBuffer[w16_index][w16_P].m_Data[0],&buf[20],12); // ʵ�����ݴ洢

    //ReceiveProcess(w16_index);  // ��Ҫ���ж���
    //TransferProcess(w16_index);

    memcpy(&uc_udp_BroadCastdata[0],&buf[MCU_UDP_SN_PACKHEAD],len);//12
    MultiMcuComm::NET_Rx_Msg(&uc_udp_BroadCastdata[0],1,len);
    CANFunc::NETtoCAN_RxProcess();
    // CANBusParse::CANParse(RUDPNET);

    uc_UDP_SendLock = 0;
}

short int MultiMcuComm::FindReceiveBuffer(unsigned char uc_Index)
{
    // ���ҿյĽ��ջ��壬�Ա��ܴ洢�ͷ��㴦��������Ϣ
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
        return -1;  //  �޷��ҵ�������
    }
    return -1;
}

unsigned char MultiMcuComm::UpdataConfirmStarNum(unsigned char uc_Index,St_MCU_WindowMessage  *  Message)
{
    unsigned char uc_check = 0;
    St_MCU_WindowMessage  Mess = *  Message;
    St_Unit[uc_Index].uc_ErrorSeqNumCounter++;
    if(St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE >= Mess.uw16_SeqNum) // �Է����Ͱ���
    {
        if(St_Unit[uc_Index].uw16_ConfirmEndNum + MCU_LIMITSENDSIZE == Mess.uw16_SeqNum) // �ȶ��Ƿ�Ϊ�Լ���Ҫ��ͬ������
        {
            uc_check = 1;
            St_Unit[uc_Index].uc_ErrorSeqNumCounter = 0;
            St_Unit[uc_Index].uw16_ConfirmEndNum = Mess.uw16_SeqNum;
            if(St_Unit[uc_Index].uw16_ConfirmEndNum >= MCU_MAX_SEQUENCE_NUMBER)
            {
                Mess.uw16_SeqNum = Mess.uw16_StarNum;  // �Լ���Ҫ��ͬ�����ŵ�����ֵ����ת����
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
        CommonFunc::GetTimeTicksUpdate( &St_Unit[uc_Index].m_ReceiveWaitOutTime1); // ����ʱ������
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
        {   // ���ط�������
            uc_check = 1; // ��ǰִ��һ������ǰ�����ݴ���
            St_Unit[uc_Index].uc_ErrorSeqNumCounter = 0; // �Զ���ط��������޷�����ͬ���ŵ����⴦��
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
                St_Unit[uc_Index].uc_ConnectSuccess  = false;   // �Ѿ�ʧ���ˣ���������UDP
                uw32_Time_Connect = MCU_MAX_RECONNECT_TIME/10;
            }
            else
                MultiMcuComm::MCU_SendConfirm(&Mess);  // ͬ����һ��ȷ�ϸ��Է�
        }
    }
    return uc_check;
}

void MultiMcuComm::ProduceMessageToSendBuffer(unsigned char uc_Index,unsigned int DataLen)
{
    // ���ɱ���
    if(IS_QUEUE_EMPTY(WaitQueue[uc_Index]))
        return;

//     unsigned char bOK = 0;
//     unsigned long int  QLength = WaitQueue[uc_Index].m_u32Length;
    short int Result = 0;

    //����һ�����ѳ�ʱ
//     if(QLength >= MCU_MAX_CMD_ITEM_SIZE)
//     {
//         bOK = 1;
//     }
//
//     if(CommonFunc::GetTimeTicks( &m_PackProduceTime[uc_Index]) >= MCU_MAX_PRODUCE_PACKTIME)    //����4ms
//     {
//         bOK = 1;
//         CommonFunc::GetTimeTicksUpdate( &m_PackProduceTime[uc_Index]); // ����ʱ������
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
    //main_TcpNet(); //  �������
    //while(!(IS_QUEUE_EMPTY(CharMGE))) // ���ϴ��޷����͵������������ط���
    if(!(IS_QUEUE_EMPTY(CharMGE)))
    {
        unsigned long int uw32_length,uw32_SL,uw32_L_End;
        unsigned char uc_Success = 0;
        unsigned char * uc_p;
        unsigned char bOK;
        St_MCU_CharMGE  st_CharMge;

        DEQUEUE(CharMGE, MAX_CHARMGE_SIZE, st_CharMge, bOK);
        uw32_length = st_CharMge.uw16_DLength;
        uc_p = (unsigned char *) &CharSend +  CharSend.m_u32Start;   // ���λ�ô�0��ʼ�洢�����ȴ�1��ʼ���������⡣
        uw32_SL = CharSend.m_u32Start+uw32_length;
        if(uw32_SL>MAX_CHAR_SIZE)  // ������ֻع���Ҫ�ر���
        {
            uw32_L_End = MAX_CHAR_SIZE - CharSend.m_u32Start;
            if(uw32_L_End > uw32_length)  // β����ת�����������������
            {
                CharSend.m_u32Length -= uw32_length;   // DEQUEUE(CharSend, MAX_CHAR_SIZE
                CharSend.m_u32Start = uw32_SL%MAX_CHAR_SIZE;  // �����ֽڷ������ɹ����ƶ��ͷ�
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
            {   // �����ڵ�MCU����ϵͳ��ɾ��
                uc_Success = 1;
                //MCU_Connect_Delete(st_CharMge.uc_DestID);
            }
            else
            {
                CharMGE.m_u32Length ++;
                CharMGE.m_u32Start=(CharMGE.m_u32Start >= 1)?(CharMGE.m_u32Start-1):(MAX_CHARMGE_SIZE-1); // �����ֽڷ�����ʧ�ܾͻ�ԭ����
            }
            //break;
        }

        if(uc_Success)
        {
            uw16_CharFailCounter = 0;
            CharSend.m_u32Length -= uw32_length;   // DEQUEUE(CharSend, MAX_CHAR_SIZE
            CharSend.m_u32Start = uw32_SL%MAX_CHAR_SIZE;  // �����ֽڷ������ɹ����ƶ��ͷ�

        }
        //break;   //  һ����һ����
    }

    //main_TcpNet(); //  �������
    NVIC_EnableIRQ(ENET_IRQn);
    uc_UDP_SendLock = 0;
}

unsigned char MultiMcuComm::PackToNetSendByte(unsigned char UnitID,unsigned char * Array,unsigned short int uw16_length)
{
    if((CharSend.m_u32Length+uw16_length>MAX_CHAR_SIZE)||(CharMGE.m_u32Length+1>MAX_CHARMGE_SIZE))
    {
        return 0;
    }

//    *(Array+3) = UnitID;    // ��ʱ������PC�㲥��  MCU_UDP_BROADCAST
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
            if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum < St_Unit[uc_Index].uw16_ConnectNum)  // ��ɨ���Ӻ�
            {   // ��Խ��ͬ���ӺŴ���
//                 uw16_ConnectNum_Min = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_ConnectNum;
                if(uw16_SeqNum_Min[0] == 0)
                    uw16_SeqNum_Min[0] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum <= uw16_SeqNum_Min[0])  // ��ɨ�����,����С�������
                {
                    uw16_counter[0]++;
                    uw16_Num[0] = uw16_i;
                    uw16_SeqNum_Min[0] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                }
            }
            else
            {   // ͬһ���ӺŴ���
                if(uw16_SeqNum_Min[1] == 0)
                    uw16_SeqNum_Min[1] = St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum;
                if(St_ConfirmBuffer[uc_Index][uw16_i].st_PackMess.uw16_SeqNum <= uw16_SeqNum_Min[1])  // ��ɨ�����,����С�������
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
    {   // �п�Խ��ͬ���ӺŴ���
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
        w16_Num = MCU_ConfirmBuff_FindMin(uc_Index);  // ����С�������
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
            // �쳣����
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
//                 Build_NewTable();   // �����ڵ�MCU������ARP����Ҳ�����ڣ�����Ҫ��ϵͳ��ɾ������
//             }
//         }
//     }
// }
