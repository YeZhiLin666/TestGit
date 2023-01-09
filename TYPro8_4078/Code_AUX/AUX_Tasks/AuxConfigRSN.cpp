#include "../Entry/includes.h"

//public:
unsigned short               AuxConfigRSN::Priority = PRIORITY8;  // PRIORITY4
unsigned short               AuxConfigRSN::TaskID = AUXBOARD_I2CRSN_TASK;

//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif


unsigned char                AuxConfigRSN::uc_FRAMRSN_Load[MAX_BOARD_PER_AUXMCU];  // RSN���Ѿ����ع�   // yy 20140912  ���T����Ϊ�����⡣
unsigned char                AuxConfigRSN::uc_BoardRSN_Counter;  // ȫ����ʹ��RSN�ŵļ�����
unsigned char                AuxConfigRSN::uc_Board_RSNStatus[MAX_BOARD_PER_AUXMCU];      //FRAM ���״̬
unsigned char                AuxConfigRSN::uc_Channel_CALIBtatus[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   //У�� ͨ����״̬
unsigned char                AuxConfigRSN::uc_RSN_JustWrite[MAX_BOARD_PER_AUXMCU];   // RSN�ճɹ�д��
unsigned char                AuxConfigRSN::uc_WaitI2C_WriteBoard[MAX_BOARD_PER_AUXMCU]; // �ȴ�I2Cд�����
unsigned char                AuxConfigRSN::uc_WaitI2C_WriteCh[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // �ȴ�I2Cд����ͨ������
unsigned char                AuxConfigRSN::uc_ReadBoard_Count[MAX_BOARD_PER_AUXMCU];    // �ظ�I2C�������������
unsigned char                AuxConfigRSN::uc_ReadCh_Count[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // �ظ�I2C������ͨ��У������������
unsigned char                AuxConfigRSN::uc_Report_RSNError[MAX_BOARD_PER_AUXMCU]; //  ����������
unsigned char                AuxConfigRSN::uc_Check_BoardRSN[MAX_BOARD_PER_AUXMCU];     // ���RSN��־
unsigned char                AuxConfigRSN::uc_Case2_temp[MAX_BOARD_PER_AUXMCU];         // ��ʱ��Ϊ���ݱ�־��
unsigned char                AuxConfigRSN::uc_ScanBoardID;  //ɨ����
unsigned char                AuxConfigRSN::uc_ScanBChID;   //ɨ���ͨ����
unsigned short               AuxConfigRSN::uw16_I2CHandleID_BD[MAX_BOARD_PER_AUXMCU];     //  I2C��ѯ����Ϣ��
unsigned short               AuxConfigRSN::uw16_I2CHandleID_CH[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   //  I2C��ѯͨ����Ϣ��
MsTime                       AuxConfigRSN::st_IntervalTime_RSN[MAX_BOARD_PER_AUXMCU];       // ���ʱ����RSN
MsTime                       AuxConfigRSN::st_IntervalTime_JustWriteRSN[MAX_BOARD_PER_AUXMCU];       // RSN�ճɹ�д�����ʱ��
MsTime                       AuxConfigRSN::st_Timeout_I2C_Board[MAX_BOARD_PER_AUXMCU];          // I2C��д��/�������ȴ���ʱʱ��
MsTime                       AuxConfigRSN::st_Timeout_I2C_Ch[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];       // I2C��дͨ���ȴ���ʱʱ��
St_MSG_Manage                AuxConfigRSN::st_RSNMSG_Manage[MAX_BOARD_PER_AUXMCU];      // ��MSG ����
St_MSG_Manage                AuxConfigRSN::st_CALIBCh_Manage[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];      // ͨ��MSG ����
unsigned short               AuxConfigRSN::uw16_CALIBCh_TotalMSGCount;
St_AUXChannelCFG             AuxConfigRSN::st_RSN_ReadCfg[MAX_AUXCH_PER_BOARD];  //  I2C��дͨ����Ϣ
St_Check_RSN                 AuxConfigRSN::st_BoardRSN[MAX_BOARD_PER_AUXMCU];    // I2C����FRAM���RSN


static St_AUXChannelCFG  m_ChanCFG;

#pragma arm section //�������������� 



////////
//    Init(void)
//    : SDLģ���ʼ��������
// Input:  N/A
// Return: N/A
///////
void AuxConfigRSN::Init(void)
{
    uc_BoardRSN_Counter = 1;
    uc_ScanBoardID = 0;
    uc_ScanBChID = 0;
    uw16_CALIBCh_TotalMSGCount = 0;
    for(unsigned char uc_BoardID = 0; uc_BoardID < MAX_BOARD_PER_AUXMCU; uc_BoardID ++)
    {
        uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;

        uc_FRAMRSN_Load[uc_BoardID] = 0;
        uc_RSN_JustWrite[uc_BoardID] = 0;
        st_BoardRSN[uc_BoardID].uc_BoardID = uc_BoardID;
        st_BoardRSN[uc_BoardID].uc_FRAMReadFlag = 0;
        st_BoardRSN[uc_BoardID].uc_I2CReadFlag = 0;
        st_BoardRSN[uc_BoardID].uc_RSN_Stata = 0;
        st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C = 0;
        st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM = 0;
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_FRAM_STATA;
        uc_ReadBoard_Count[uc_BoardID] = 0;
        uc_Report_RSNError[uc_BoardID] = ERROR_RSN_NONE;
        uc_Check_BoardRSN[uc_BoardID] = 0;

        CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_RSN[uc_BoardID]); // ���¼��ϵͳʱ��
        CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // ���¼��ϵͳʱ��
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        memset((void*) &st_RSNMSG_Manage[uc_BoardID], 0, sizeof(St_MSG_Manage));
        for(unsigned char uc_j = 0; uc_j < MAX_BUFFER_MESSAGE; uc_j ++)
            st_RSNMSG_Manage[uc_BoardID].st_MSG[uc_j].uc_MsgCode = 0XFF;

        Check_RSN_Init( uc_BoardID );


        for(unsigned char uc_ChID = 0; uc_ChID < MAX_AUXCH_PER_AUXMCU; uc_ChID ++)
        {
            uc_Channel_CALIBtatus[uc_BoardID][uc_ChID] = READY;
            uc_WaitI2C_WriteCh[uc_BoardID][uc_ChID] = 0;
            uc_ReadCh_Count[uc_BoardID][uc_ChID] = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_ChID]); // ���¼��ϵͳʱ��
            memset((void*) &st_CALIBCh_Manage[uc_BoardID][uc_ChID], 0, sizeof(St_MSG_Manage));
            for(unsigned char uc_k = 0; uc_k < MAX_BUFFER_MESSAGE; uc_k ++)
                st_CALIBCh_Manage[uc_BoardID][uc_ChID].st_MSG[uc_k].uc_MsgCode = 0XFF;
        }
    }
}

////////
//    DoWork(void)
//    : SDLģ�����й���������
// Input:  N/A
// Return: N/A
///////
void AuxConfigRSN::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CONFIGRSN] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CONFIGRSN);
#endif

    //   #if AUX_MULTI_FUNCTION_BOARD_DIDO == 1
    //   unsigned char uc_DIDOFlag = 0;
    //   #endif
    unsigned char uc_BoardID = 0;
    uc_BoardID = uc_ScanBoardID;
    if(++uc_ScanBoardID >= MAX_BOARD_PER_AUXMCU)
        uc_ScanBoardID = 0;
    //for(unsigned char uc_BoardID = 0; uc_BoardID < MAX_BOARD_PER_AUXMCU; uc_BoardID ++)
    //{
    if(Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_valid == BEUSED)
    {
        PreprocessMsg_RSN(uc_BoardID);       // ����Ϣ
        ProcessState_BoardRSN(uc_BoardID);   // ��״̬
        PreprocessMsg_ChCALIB(uc_BoardID);   // ͨ����Ϣ
        ProcessState_ChCALIB(uc_BoardID);    // ͨ��״̬
    }

    /*				 #if AUX_MULTI_FUNCTION_BOARD_DIDO == 1

    if( uc_DIDOFlag == 0)
    {
    if( (Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType == AUX_DI)
    || (Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType == AUX_DO) )    // ģ��������һ��DIDO��  (��0�ŵ�6�Ű壬��Ҫ�����7�Ű壩
    {
    st_BoardRSN[(uc_BoardID+1)%MAX_BOARD_PER_AUXMCU].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
    st_BoardRSN[(uc_BoardID+1)%MAX_BOARD_PER_AUXMCU].uw16_BoardRSN_FRAM = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
    uc_BoardID ++;  // ģ��������һ��DIDO�� (��0�ŵ�6�Ű壬��Ҫ�����7�Ű壩
    uc_DIDOFlag = 1;
    }
    }
    #endif            */
    //}

//     if(!m_AUX_Running)
//     {
//         if(m_AUX_WriteMap)
//         {
// 					   if(CommonFunc::GetTimeTicks( &m_AUX_Delay) >= 20000)    //����2s
// 						 {
// 							 CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // ���¼��ϵͳʱ��
// 							 MP_AllIVMap  * st_AllIVMap;
// 							 st_AllIVMap = (MP_AllIVMap *) &EthernetParse::m_btPackDataBuffer[0];
// 							 memcpy(&m_AUX_CopyMap[0],&EthernetParse::m_btPackDataBuffer[0],3200);
// 							 AuxCConfig::SaveAuxUnit2IVMap(st_AllIVMap, 3200);  // �������ݵ�FRAM
// 							 m_AUX_WriteMap = 0;
// 						 }
//         }
// 				else
// 					CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // ���¼��ϵͳʱ��
//     }
// 		else
// 			CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // ���¼��ϵͳʱ��

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CONFIGRSN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CONFIGRSN);
#endif
}


/*****************************************************
// Function Name: PreprocessorMsg
// Description:
// History��
//
// ******************************************************/
void AuxConfigRSN::PreprocessMsg_RSN(unsigned char uc_BoardID)
{
    // ������Ϣ����
    unsigned char 	MsgIndex;      //ȡ�õ�ǰ��Ӧ��Ϣ���

    unsigned char uc_RSNStatus = uc_Board_RSNStatus[uc_BoardID];                //��ǰ��״̬
    unsigned char uc_CleanFlag = 1;

    if( st_RSNMSG_Manage[uc_BoardID].uc_TotalMessageCount <= 0 )
        return;
    MsgIndex = st_RSNMSG_Manage[uc_BoardID].uc_MsgConsumePointer;

    // ��ʵ��ţ����紦��Ӧ����λ���߼���ͨ����ת��ʵ�ʶ�Ӧ�š�
    switch(uc_RSNStatus)
    {
    case OPERATIONAL:
        switch(st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex].uc_MsgCode)
        {
        case MESSAGE_UPDATA_RSN:
            if(! uc_RSN_JustWrite[uc_BoardID])   // ����RSN�Ƿ��Ѿ����ɹ�д����  ��ûд��,����RSNд��
            {
                uc_Board_RSNStatus[uc_BoardID] = REQUEST_WRITE_RSN;
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM = AuxConfigRSN::Produce_RSN(uc_BoardID);
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
            }
            else
            {
                uc_CleanFlag = 0;  // ���һ����ʱ���ٴ���
            }
            break;
        case MESSAGE_WRITE_RSN:    // ������Ӧͨ��У����������Ϣ����дRSN
            st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex].uc_MsgCode = MESSAGE_UPDATA_RSN;
            uc_CleanFlag = 0;
            break;
        default:
            break;
        }
        break;

    case REPORTED_TO_PC:
        switch(st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex].uc_MsgCode)
        {
        case MESSAGE_UPDATA_RSN:
            uc_Board_RSNStatus[uc_BoardID] = REQUEST_WRITE_RSN;
            st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM = AuxConfigRSN::Produce_RSN(uc_BoardID);
            st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
            break;
        case MESSAGE_STOP_REPORT:   // ��������λ��Ҫ��ֹͣ���棬�Ͳ�Ҫ�ٱ����ˡ���Ҫ���I2C��д��ʱ����ȡ�
            uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
        default:
            break;
        }
        break;

    case REQUEST_WRITE_RSN:
    case EXECUTE_WRITE_RSN:
    case CONFIRM_WRITE_RSN:
        switch(st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex].uc_MsgCode)
        {
        case MESSAGE_WRITE_RSN:
            // ��Msg���� Operationalר�� Pool,
            st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex] =  st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex];
            uc_CleanFlag = 0;
            //break;
            return;  // ���棬��ȡ������
        default:
            break;
        }
        break;

    default:
        break;
    }

    if(uc_CleanFlag)
    {
        CleanMsg_RSN( MODEL_BOARD, uc_BoardID, 0 );
        if(uc_RSNStatus != uc_Board_RSNStatus[uc_BoardID])
            AuxCConfig::WriteRSNStata(uc_BoardID, &uc_Board_RSNStatus[uc_BoardID]);
    }
}

/*****************************************************
// Function Name:
// Description:
// History��
//
// *****************************************************/
void AuxConfigRSN::ProcessState_BoardRSN(unsigned char uc_BoardID)
{
    //���ڵ�ǰ�ĸ���״̬���д���ִ��״̬������̣���Ӧ���ת��״̬

    unsigned char uc_RSNStatus = uc_Board_RSNStatus[uc_BoardID];                //��ǰ��״̬
    unsigned char uc_I2CReadFlag = 0;
    //UWord16       uw16_BoardRSN;
    UWord16       uw16_Addr;
    Un_ERROR_MESSAGE * st_SPB2;

    switch(uc_Board_RSNStatus[uc_BoardID])
    {
    case OPERATIONAL:
        if(uc_Check_BoardRSN[uc_BoardID])
        {
            Check_BoardRSN_TaskCase(uc_BoardID);
            if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_R_SUCCESS)
            {
                uc_Check_BoardRSN[uc_BoardID] = 0;
                uc_WaitI2C_WriteBoard[uc_BoardID] = 0;
            }
        }

        if(CommonFunc::GetTimeTicks(&st_IntervalTime_RSN[uc_BoardID]) >= MAX_INTERVAL_TIME)    // ���4Сʱ
        {
            if(st_BoardRSN[uc_BoardID].uc_I2CReadFlag)
            {
                uc_Check_BoardRSN[uc_BoardID] = 1;
                st_BoardRSN[uc_BoardID].uc_I2CReadFlag = 0;
            }
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_FRAM_STATA;
            CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_RSN[uc_BoardID]); // ���¼��ϵͳʱ��
            return;
        }

        if(uc_RSN_JustWrite[uc_BoardID])   // ����RSN�Ƿ��Ѿ����ɹ�д����  ��д��,��Ҫ���һ����ʱ���ٴ���
        {
            if(CommonFunc::GetTimeTicks(&st_IntervalTime_JustWriteRSN[uc_BoardID]) >= TIMEOUT_1s)    // ���1��
            {
                uc_RSN_JustWrite[uc_BoardID] = 0;
                CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // ���¼��ϵͳʱ��
            }
        }
        break;
    case RSN_DIRTY_WO_INFO:
        uc_Board_RSNStatus[uc_BoardID] = RSN_DIRTY_READING_INFO;
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_W_BID;
        break;
    case RSN_DIRTY_READING_INFO:
        switch(uc_WaitI2C_WriteBoard[uc_BoardID])
        {
        case I2C_TASK_W_BID:
            E2PROM_24LC128::WriteBuf[0]='B';   //������
            E2PROM_24LC128::WriteBuf[1]='I';
            E2PROM_24LC128::WriteBuf[2]='D';
            E2PROM_24LC128::WriteBuf[3]=MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID;  // ���������I2C����Խ�
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,0x02,0x04);
            uc_I2CReadFlag = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // �а��Ƿ����
            break;
        case I2C_TASK_R_BID:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,0x02,0x04);
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // �а��Ƿ����
            break;
        case I2C_TASK_R_B_INFO:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
            uw16_Addr = AUXBOARDCFGSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(ST_AUX_BOARD_INFO_SAVE));
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
            uc_ReadBoard_Count[uc_BoardID] = 0;   // ��ʱ��Ϊ����ͨ����Ϣ��
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // ����ڶ�������Ϣ
            break;
        case I2C_TASK_R_B_CH_INFO:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
            uw16_Addr = AUXBOARDCHANNELCFGADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,MAX_I2C_BUFFER*Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Total_Channels);
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
            uc_ReadBoard_Count[uc_BoardID] ++;   // ��ʱ��Ϊ����ͨ����Ϣ��
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // ����ڶ���ͨ����Ϣ
            break;
        default:
            // һֱѯ��I2Cģ���Ƿ���ɹ���
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //�����
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������

                if(uc_Case2_temp[uc_BoardID] == I2C_TASK_W_BID)
                {
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_BID;
                }
                else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_BID)
                {
                    E2PROM_24LC128::WriteBuf[0]='B';   //������
                    E2PROM_24LC128::WriteBuf[1]='I';
                    E2PROM_24LC128::WriteBuf[2]='D';
                    E2PROM_24LC128::WriteBuf[3]=MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID;
                    if(!memcmp(& E2PROM_24LC128::WriteBuf[0],&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],4))
                    {
                        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_B_INFO;
                    }
                    else
                    {
                        uc_Report_RSNError[uc_BoardID] = ERROR_RSN_HAVE_LOSTBOARD;// ����������
                        uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
                    }

                }
                else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_B_INFO)
                {
                    //memcpy(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_valid,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf,sizeof(ST_AUX_BOARD_INFO_SAVE));
                    ST_AUX_BOARD_INFO_SAVE * m_Readcfg = (ST_AUX_BOARD_INFO_SAVE * )&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf;
                    Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe = m_Readcfg->m_abi_globe;
                    Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_assemble = m_Readcfg->m_assemble;
                    Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_valid = m_Readcfg->m_valid;
                    Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_RSN=m_Readcfg->m_RSN;
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_B_CH_INFO;
                }
                else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_B_CH_INFO)
                {
                    for(uc_I2CReadFlag = 0; uc_I2CReadFlag < Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Max_Channels; uc_I2CReadFlag++)
                    {
                        //uw16_Addr = 4+uc_I2CReadFlag*MAX_I2C_BUFFER;  // �ṹǰ����2��Ԥ���ֽڣ���2���Ͳ���
                        //memcpy(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_Populated,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[uw16_Addr],sizeof(St_AUXChannelCFG));
                        uw16_Addr = uc_I2CReadFlag*MAX_I2C_BUFFER;
                        St_AUXChannelCFG  * m_Readcfg = (St_AUXChannelCFG * )&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[uw16_Addr];

                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_Populated = m_Readcfg->m_valid;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_ChnCoreInfo.m_AuxSensorCoreInfo = m_Readcfg->m_ChnCoreInfo.m_AuxSensorCoreInfo;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_AuxType= m_Readcfg->m_AuxType;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_AuxSubType = m_Readcfg->m_AuxSubType;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_avg_num = m_Readcfg->m_AVG_num;
                    }
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS; // ����ڶ���ͨ����Ϣ
                }
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //����1��
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // ʱ�仹δ��ʱ�ȵȰɣ�
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //����1��
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// ����������
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }

            if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_R_SUCCESS)
            {
                uc_WaitI2C_WriteBoard[uc_BoardID] = 0;// �ɹ���������
                uc_Board_RSNStatus[uc_BoardID] = RSN_DIRTY_W_INFO;
            }
            break;
        }
        break;
    case RSN_DIRTY_W_INFO:
        uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
        break;
    case REPORT_TO_PC:
        st_SPB2 = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;
        st_SPB2->m_AuxBoard.m_btBoard_ID = uc_BoardID;
        st_SPB2->m_AuxBoard.m_btAuxType =0;// Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
        st_SPB2->m_AuxBoard.m_btAux_SubType =0; //Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[0].m_AuxTemp.m_AuxThermoCouple.m_btAuxSubType;
        st_SPB2->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Total_Channels;

        EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        uc_Board_RSNStatus[uc_BoardID] = REPORTED_TO_PC;
        break;
    case REPORTED_TO_PC:
        if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_REPORT_TIMEOUT)    //����10��
        {
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
            //uc_Report_RSNError[uc_BoardID] = ERROR_RSN_WAITPC_TIMEOUT;// ����������
            uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;

            if(uc_Report_RSNError[uc_BoardID] == ERROR_RSN_NONE)
            {
                uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;  // �ɹ��ı��棬�ɻظ�ԭʼ״̬���Ա���ɹ�ҲƵƵ����Ҫ��λ��ָ��ִ�С�
            }
        }

        St_DataMessage  st_Message;     //  Ĭ����λ���ѻظ�һ��Ҫ�����RSNȷ��
        st_Message.uc_AuxType = AUX_VOLTAGE;
        st_Message.uc_BoardID = uc_BoardID;
        st_Message.uc_MsgSource = MESSAGE_SOURCE_PC_ORDER;
        st_Message.uw16_ChannelID = 0;
        st_Message.uc_MsgCode = MESSAGE_UPDATA_RSN;
        AuxConfigRSN::PushMsg_RSN( MODEL_BOARD, &st_Message );
        break;

    case REQUEST_WRITE_RSN:
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WRITE_RSN;
        uc_I2CReadFlag = AuxCConfig::WriteRSN(uc_BoardID ,&st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM);
        uc_Board_RSNStatus[uc_BoardID] = EXECUTE_WRITE_RSN;
        break;

    case EXECUTE_WRITE_RSN:
        if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_WRITE_RSN)
        {
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WRITE_RSN_WAIT;
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
            E2PROM_24LC128::WriteI2cBuf((UChar8 *) &st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,sizeof(UWord16));
            uw16_Addr = AUXBOARDRSNSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
            uc_I2CReadFlag = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        }
        else
        {
            // һֱѯ��I2Cģ���Ƿ���ɹ���
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //�����
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN;  // �ɹ�����д��
                uc_ReadBoard_Count[uc_BoardID] = 0;
                uc_Board_RSNStatus[uc_BoardID] = CONFIRM_WRITE_RSN;
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //����1��
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // ʱ�仹δ��ʱ�ȵȰɣ�
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //����1��
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// ����������
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }
        }
        break;
    case CONFIRM_WRITE_RSN:
        if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_READ_RSN)
        {
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN_WAIT;
            uc_ReadBoard_Count[uc_BoardID] = 0;
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
            uw16_Addr = AUXBOARDRSNSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
            uc_I2CReadFlag = 0;
            uc_RSN_JustWrite[uc_BoardID] = 1;
            CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // ���¼��ϵͳʱ��
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        }
        else
        {
            // һֱѯ��I2Cģ���Ƿ���ɹ���
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //�����
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������
                memcpy(&st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf,sizeof(UWord16));
                if(st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM == st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C)
                {
                    uc_WaitI2C_WriteBoard[uc_BoardID] = 0; // �ɹ���������
                    uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
                    break;
                }

                if(uc_ReadBoard_Count[uc_BoardID] < MAX_READ_RSN_TRY)
                {
                    uc_ReadBoard_Count[uc_BoardID]++ ;
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN;  // �ظ�ȥ����γ��ԱȽϽ��
                }
                else
                {
                    uc_ReadBoard_Count[uc_BoardID] = 0;  // ʧ�ܲ�������
                    uc_Report_RSNError[uc_BoardID] = ERROR_RSN_WRITERSN_CMP;// ����������
                    uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
                }
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //����1��
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // ʱ�仹δ��ʱ�ȵȰɣ�
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //����1��
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// ����������
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }
        }
        break;

    default:
        uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
        break;
    }

    if(uc_RSNStatus != uc_Board_RSNStatus[uc_BoardID])
        AuxCConfig::WriteRSNStata(uc_BoardID, &uc_Board_RSNStatus[uc_BoardID]); // ��ʱ����Ҫ���ϴ棬��ʱ���ٵĿ��ں���档
}

/*****************************************************
// Function Name:
// Description:
// History��
//
// ******************************************************/
void AuxConfigRSN::PreprocessMsg_ChCALIB(unsigned char uc_BoardID)
{
    // ������Ϣ����
    unsigned char 	MsgIndex;    //ȡ�õ�ǰ��Ӧ��Ϣ���

    unsigned char uc_Channel = 0;
    unsigned char uc_ChCLIBtatus = 0;   //��ǰ��״̬
    unsigned char uc_I2CReadFlag = 0;
    unsigned char uc_CleanFlag = 1;

    if(uw16_CALIBCh_TotalMSGCount <= 0)
        return;

    for(uc_Channel = 0; uc_Channel < MAX_AUXCH_PER_BOARD; uc_Channel ++)
    {
        if( st_CALIBCh_Manage[uc_BoardID][uc_Channel].uc_TotalMessageCount == 0 )
            continue;  // û����ҪУ����ͨ����Ϣ

        uc_ChCLIBtatus = uc_Channel_CALIBtatus[uc_BoardID][uc_Channel];               //��ǰ��״̬
        MsgIndex = st_CALIBCh_Manage[uc_BoardID][uc_Channel].uc_MsgConsumePointer;

        // ��ʵ�����ʵͨ���ţ����紦��Ӧ����λ���߼���ͨ����ת��ʵ�ʶ�Ӧ�š�
        switch(uc_ChCLIBtatus)
        {
        case READY:
            switch(st_CALIBCh_Manage[uc_BoardID][uc_Channel].st_MSG[MsgIndex].uc_MsgCode)
            {
            case MESSAGE_WRITE_CALIB:
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM = AuxConfigRSN::Produce_RSN(uc_BoardID);
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
                uc_I2CReadFlag = AuxCConfig::WriteRSN(uc_BoardID ,&st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C);
                uc_Channel_CALIBtatus[uc_BoardID][uc_Channel] = REQUEST_WRITE_CALIB;
                break;
            default:
                break;
            }
            break;
        case EXECUTE_WRITE_CALIB:
        case CONFIRM_WRITE_CALIB:
            switch(st_CALIBCh_Manage[uc_BoardID][uc_Channel].st_MSG[MsgIndex].uc_MsgCode)
            {
            case MESSAGE_WRITE_CALIB:
                uc_Channel_CALIBtatus[uc_BoardID][uc_Channel] = REQUEST_WRITE_CALIB;
                break;
            default:
                break;
            }
            break;

        case REQUEST_WRITE_CALIB:
            switch(st_CALIBCh_Manage[uc_BoardID][uc_Channel].st_MSG[MsgIndex].uc_MsgCode)
            {
            case MESSAGE_WRITE_CALIB:
                uc_Channel_CALIBtatus[uc_BoardID][uc_Channel] = EXECUTE_WRITE_CALIB;
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }

        if(uc_CleanFlag)
        {
            CleanMsg_RSN( MODEL_CHANNEL, uc_BoardID, uc_Channel );
        }
    }
}


/*****************************************************
// Function Name:
// Description:
// History��
//
// *****************************************************/
void AuxConfigRSN::ProcessState_ChCALIB(unsigned char uc_BoardID)
{
    //���ڵ�ǰ�ĸ���״̬���д���ִ��״̬������̣���Ӧ���ת��״̬

    UWord16       uw16_AddrC3;
    unsigned char uc_ChStatus = 0;          // ��ǰ��״̬
    unsigned char uc_Ch;
    unsigned char uc_I2CReadFlag = 0;
    unsigned char uc_CopybackC3;
    unsigned  char  uc_BoardID_Back;
    Un_ERROR_MESSAGE * st_SPC3;
    St_DataMessage  st_MessageC3;

    uc_Ch =uc_ScanBChID;
    if(++uc_ScanBChID >= MAX_AUXCH_PER_BOARD)
        uc_ScanBChID = 0;

    //for(uc_Ch = 0; uc_Ch < MAX_AUXCH_PER_BOARD; uc_Ch ++)
    //{
    uc_ChStatus = uc_Channel_CALIBtatus[uc_BoardID][uc_Ch];
    switch(uc_ChStatus)
    {
    case READY:

        break;
    case REQUEST_WRITE_CALIB:
        uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] =  I2C_TASK_W_CH_CALIB;
        uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = EXECUTE_WRITE_CALIB;
        break;
    case EXECUTE_WRITE_CALIB:
        if(uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] == I2C_TASK_W_CH_CALIB)
        {
            uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = I2C_TASK_W_CH_CALIB_WAIT;

            // 									  m_ChanCFG.m_valid = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_Populated;
            // 									  m_ChanCFG.m_AuxType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
            // 									  m_ChanCFG.m_AuxSubType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSubType;
            // 									  memcpy(&m_ChanCFG.m_AuxSensorCoreInfo.m_PHY_RangeL,&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSensorCoreInfo.m_PHY_RangeL,sizeof(ST_AUX_SENSOR_CORE_INFO));
            // 									 // m_ChanCFG.m_AuxSensorCoreInfo=Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSensorCoreInfo;
            // 									  m_ChanCFG.m_AVG_num =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_avg_num;
            // 									  m_ChanCFG.m_Controllable=Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_Controllable;
            // 									  m_ChanCFG.m_None=0;
            // 									  m_ChanCFG.m_ChannelNumber=uc_Ch;
            // 									  E2PROM_24LC128::WriteI2cBuf((UChar8 *)&m_ChanCFG.m_valid,sizeof(St_AUXChannelCFG));  //St_AUXChannelCFG
            static float temp[2];
            temp[0]=Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
            temp[1]=Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
            E2PROM_24LC128::WriteI2cBuf((UChar8 *)&temp,8);  //modify qjm 20150707
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
            //                     uw16_AddrC3 = 4 + AUXBOARDCHANNELCFGADDR + uc_Ch*MAX_I2C_BUFFER; // ͨ����Ϣǰ�������ֽڿ��У����������͡�[�����ַû���]
            uw16_AddrC3 = 16+uc_Ch*MAX_I2C_BUFFER  + AUXBOARDCHANNELCFGADDR; // ͨ����Ϣǰ�������ֽڿ��У����������͡�

            //                     uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,sizeof(St_AUXChannelCFG));//St_AUXChannelCFG
            uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,8);//St_AUXChannelCFG
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]); // ���¼��ϵͳʱ��
        }
        else
        {
            // һֱѯ��I2Cģ���Ƿ���ɹ���
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqFin) //�����
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������
                E2PROM_24LC128::WriteI2cBuf((UChar8 *) &st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,sizeof(UWord16));
                uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = I2C_TASK_R_CH_CALIB;
                uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = CONFIRM_WRITE_CALIB;
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]) >= MAX_I2C_TIMEOUT)    //����1��
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // ʱ�仹δ��ʱ�ȵȰɣ�
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //����1��
            {
                st_SPC3 = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;    // ��û��PC����״̬��ֻ��ֱ�ӱ���
                st_SPC3->m_AuxBoard.m_btBoard_ID = uc_BoardID;
                //                         st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
                //                         st_SPC3->m_AuxBoard.m_btAux_SubType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[0].m_AuxTemp.m_AuxThermoCouple.m_btAuxSubType;
                //                         st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_ChannelCount;
                st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
                st_SPC3->m_AuxBoard.m_btAux_SubType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSubType;
                st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Max_Channels;
                uc_CopybackC3 = uc_Report_RSNError[uc_BoardID];    // ����������
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;    // ����������
                EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
                uc_Report_RSNError[uc_BoardID] = uc_CopybackC3;    // ����������
                uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = READY;
            }
        }
        break;
    case CONFIRM_WRITE_CALIB:
        if(uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] == I2C_TASK_R_CH_CALIB)
        {
            uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = I2C_TASK_R_CH_CALIB_WAIT;
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
            //uw16_AddrC3 = 4 + AUXBOARDCHANNELCFGADDR + uc_Ch*MAX_I2C_BUFFER; // ͨ����Ϣǰ�������ֽڿ��У����������͡�[�����ַû���]
            uw16_AddrC3 = uc_Ch*MAX_I2C_BUFFER + 4 + AUXBOARDCHANNELCFGADDR; // ͨ����Ϣǰ�������ֽڿ��У����������͡�
            uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,sizeof(St_AUXChannelCFG));
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]); // ���¼��ϵͳʱ��
        }
        else
        {
            // һֱѯ��I2Cģ���Ƿ���ɹ���
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqFin) //�����
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������
                //if(!memcmp(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[uc_Ch].m_AuxTemp.m_AuxThermoCouple.TempChType,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],sizeof(St_AUXChannelCFG)))
                if(!memcmp(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_Populated,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],sizeof(St_AUXChannelCFG)))
                {
                    uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = 0; // �ɹ���������
                    //st_MessageC3.uc_AuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
                    st_MessageC3.uc_AuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
                    st_MessageC3.uc_BoardID = uc_BoardID;
                    st_MessageC3.uc_MsgSource = MESSAGE_SOURCE_CHANNEL_CALIB;
                    st_MessageC3.uw16_ChannelID = uc_Ch;
                    st_MessageC3.uc_MsgCode = MESSAGE_WRITE_RSN;
                    PushMsg_RSN( MODEL_BOARD, &st_MessageC3 );
                }
                uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = READY;
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]) >= MAX_I2C_TIMEOUT)    //����1��
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // ʱ�仹δ��ʱ�ȵȰɣ�
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //����1��
            {
                st_SPC3 = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;      // ��û��PC����״̬��ֻ��ֱ�ӱ���
                st_SPC3->m_AuxBoard.m_btBoard_ID = uc_BoardID;
                //                         st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
                //                         st_SPC3->m_AuxBoard.m_btAux_SubType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[0].m_AuxTemp.m_AuxThermoCouple.m_btAuxSubType;
                //                         st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_ChannelCount;
                st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
                st_SPC3->m_AuxBoard.m_btAux_SubType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSubType;
                st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Max_Channels;
                uc_CopybackC3 = uc_Report_RSNError[uc_BoardID];    // ����������
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;    // ����������
                EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
                uc_Report_RSNError[uc_BoardID] = uc_CopybackC3;    // ����������
                uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = READY;
            }
        }
        break;

    default:
        uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = READY;
        break;
    }
    //}

}


/*****************************************************
// Function Name:
// Description:
// History��
//
// *****************************************************/
UWord16 AuxConfigRSN::Produce_RSN(unsigned char uc_BoardID)
{
    // ����RSN����
    // RSN��16bit�������ɰ취�� ǰ8λ��ϵͳ��ǰʱ���΢�������Ϊ�������
    //                         �м�5λ��Ϊ������������������е��Ƿ�Ϊ��ͬ��
    //                         �����ͬ����������Ϊ�����Ŀ�ʼ����ʼ�㣬��3λ��Ϊ��Ž����������㡣
    UWord16 uw16_RSN = 0;
    MsTime SysTime = CommonFunc::GetSystemTime();
    uw16_RSN  =  (SysTime.Us100 & 0XFF) << 8;
    uw16_RSN  |=   (uc_BoardRSN_Counter & 0X1F) << 5;
    uw16_RSN  |=   (uc_BoardID & 0X07);

    if( ++uc_BoardRSN_Counter == 0)
        uc_BoardRSN_Counter = 1;

    return    uw16_RSN;
}


/*****************************************************
// Function Name:
// Description:
// History��
//
// *****************************************************/
void AuxConfigRSN::Check_BoardRSN_TaskCase(unsigned char uc_BoardID)
{
    // ��FRAM�����RSN�ţ���I2C���RSN�Ž��бȽ�

    unsigned char  uc_I2CReadFlag = 0;
    //     unsigned char  uc_FRAMReadFlag = 0;
    //     unsigned char  uc_RSN_Stata = 0;
    //     unsigned short int uw16_RSN_I2C = 0;
    //     unsigned short int uw16_RSN_FRAM = 0;
    //     unsigned short int uw16_TempValue = 0;        //�Ƚ�ֵ
    unsigned short int uw16_Addr = 0;

    switch(uc_WaitI2C_WriteBoard[uc_BoardID])
    {
    case I2C_TASK_FRAM_STATA:
        if(!uc_FRAMRSN_Load[uc_BoardID])   // yy 20140912  ���T����Ϊ�����⡣
        {
            AuxCConfig::ReadRSNStata(uc_BoardID, &st_BoardRSN[uc_BoardID].uc_RSN_Stata); //����״̬
        }
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    case I2C_TASK_FRAM_RSN:
        if(!uc_FRAMRSN_Load[uc_BoardID])   // yy 20140912  ���T����Ϊ�����⡣
        {
            st_BoardRSN[uc_BoardID].uc_FRAMReadFlag = AuxCConfig::ReadRSN(uc_BoardID ,&st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM); //����RSN
            uc_FRAMRSN_Load[uc_BoardID] = 1;
        }
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    case I2C_TASK_R_RSN:
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // ���¼��ϵͳʱ��
        uw16_Addr = AUXBOARDRSNSRARTADDR;
        uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // ��ʱ��Ϊ���ݱ�־��
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    default:
        if((uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_STATA)||(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_RSN))
        {
            //if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //����1��   //  20140915  yy  ����������RSN�Ƚ�������Ϊ45�����������10��������ɡ�
            //{
            if(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_STATA)
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_FRAM_RSN;
            else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_RSN)
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN;
            //}
            return;
        }

        // һֱѯ��I2Cģ���Ƿ���ɹ���
        unsigned char  uc_I2CReadFlag = 0;
        if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //�����
        {
            I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������

            if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_RSN)
            {
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS; // �ɹ���������
                st_BoardRSN[uc_BoardID].uc_I2CReadFlag = 1;
                uc_I2CReadFlag = I2C_TASK_R_SUCCESS;
                memcpy(&st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf,sizeof(UWord16));
            }
        }
        else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
        {
            uc_I2CReadFlag = I2C_TASK_R_ERROR;
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS;
        }
        else
        {
            if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= (MAX_I2C_TIMEOUT + 0.01*TIMEOUT_1s*TIMEOUT_1s))    //����1��
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS;
            }
            // ʱ�仹δ��ʱ�ȵȰɣ�
        }

        if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_R_SUCCESS)
        {
            Check_BoardRSN( &st_BoardRSN[uc_BoardID]);
        }
        break;
    }
}

/*****************************************************
// Function Name:
// Description:
// History��
//
// *****************************************************/
void AuxConfigRSN::Check_BoardRSN(St_Check_RSN * St_RSN)
{
    // ��FRAM�����RSN�ţ���I2C���RSN�Ž��бȽ�
    unsigned short int uw16_TempValue = 0;        //�Ƚ�ֵ

    //     AuxCConfig::ReadRSNStata(uc_BoardID, &uc_RSN_Stata); //����״̬
    //
    //     uc_FRAMReadFlag = AuxCConfig::ReadRSN(uc_BoardID ,&uw16_BoardRSN_FRAM[uc_BoardID]); //����RSN
    //     uw16_RSN_FRAM = uw16_BoardRSN_FRAM[uc_BoardID];    //��ǰ��״̬FRAM
    //
    //     //uc_I2CReadFlag = AuxCConfig::ReadAUXBoardRSN(uc_BoardID ,&uw16_BoardRSN_I2C[uc_BoardID]);;   // ����I2C��24LC128
    //     //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
    //     //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
    //     uc_I2CReadFlag = AuxCConfig::ReadAUXBoardRSN(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID ,&uw16_BoardRSN_I2C[uc_BoardID]);
    //     uw16_RSN_I2C = uw16_BoardRSN_I2C[uc_BoardID];     //��ǰ��״̬I2C
    //

    uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_NONE;// ����û�д���
    uw16_TempValue = St_RSN->uw16_BoardRSN_I2C - St_RSN->uw16_BoardRSN_FRAM;  // �ȽϽ������
    if(St_RSN->uc_I2CReadFlag)
    {   // ����I2C��RSN
        if((St_RSN->uw16_BoardRSN_I2C == 0)||(St_RSN->uw16_BoardRSN_I2C == 0XFFFF))
        {
            uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;  //  ����һ��û����ListenForNetд���İ�
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// �������
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
            return;
        }

        if(St_RSN->uc_FRAMReadFlag)
        {   // ����SPI FRAM��RSN
            switch(uw16_TempValue)
            {
            case RSN_COMPARE_SAME:
                switch(St_RSN->uc_RSN_Stata)   //  ���������
                {
                case OPERATIONAL:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = OPERATIONAL;
                    break;
                case RSN_DIRTY_WO_INFO:
                case RSN_DIRTY_READING_INFO:
                case RSN_DIRTY_W_INFO:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_READING_INFO;
                    break;
                case REPORT_TO_PC:
                case REPORTED_TO_PC:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;
                    break;

                case REQUEST_WRITE_RSN:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;
                    break;

                case EXECUTE_WRITE_RSN:
                case CONFIRM_WRITE_RSN:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = St_RSN->uc_RSN_Stata;
                default:
                    uc_Board_RSNStatus[St_RSN->uc_BoardID] = OPERATIONAL;
                    break;
                }

                if(St_RSN->uc_RSN_Stata != uc_Board_RSNStatus[St_RSN->uc_BoardID])
                    AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_NONE;// ����û�д���
                break;

            case RSN_COMPARE_NEW:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;  // �����°�
                AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// �������
                break;

            case RSN_COMPARE_DIFFE:
            default:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO; // �����ù��İ�
                AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_OTHERSYSTEMUSEBOARD;// �������
                break;
            }
        }
        else
        {   // û�ж���SPI FRAM��RSN
            switch(uw16_TempValue)
            {
            case RSN_COMPARE_NEW:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;  // �����°�
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// �������
                break;

            case RSN_COMPARE_DIFFE:
            default:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO; // SysteConfig����֮ǰ������һ����ϵͳ�ù��İ�
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_OTHERSYSTEMUSEBOARD;// �������
                break;
            }
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
        }
    }
    else
    {   // û�ж���I2C��RSN
        if((St_RSN->uw16_BoardRSN_FRAM == 0)||(St_RSN->uw16_BoardRSN_FRAM == 0XFFFF))
        {
            St_RSN->uc_FRAMReadFlag = 0;  //  ����û�в�İ壬����ûд��FRAM�е�RSN���򲻴���
        }

        if(St_RSN->uc_FRAMReadFlag)
        {
            uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;  //  û�ж���I2C��RSN   ����   ����һ��ԭ�����ڹ��İ�������ϵͳ���Ҳ�����
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_HAVE_LOSTBOARD;// �������
        }
        else
        {   // û�ж���SPI FRAM��RSN
            //uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_LOST;// �������
            return;   // ���߶�û�У���Ϊ����
        }
    }
}

/*****************************************************
// Function Name:
// Description:
// History��
//
// ******************************************************/
void AuxConfigRSN::PushMsg_RSN( unsigned char uc_Mode, St_DataMessage * st_Message )
{
    // ������Ϣ����
    unsigned char SuccessFlag = 0;
    unsigned char MsgIndex;
    unsigned char uc_Type = st_Message->uc_AuxType;
    unsigned char uc_Bid = st_Message->uc_BoardID;
    unsigned short int uw16_Chan = st_Message->uw16_ChannelID;   // �߼���

    if(uc_Mode == MODEL_CHANNEL)
    {
        /* for(uc_Bid = 0; uc_Bid < MAX_BOARD_PER_AUXMCU; uc_Bid ++) // ����˳������ʱ��ͨ����Ϣ���λ��
        {

        //             if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_BType.m_AuxType == uc_Type)  // Aux type
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_Start_InUnit <= uw16_Chan) // Aux Channel
        &&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_End_InUnit >= uw16_Chan))
        {
        st_Message->uc_BoardID = uc_Bid;
        st_Message->uw16_ChannelID = uw16_Chan - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_Start_InUnit;
        SuccessFlag = 1;
        break;    // �ҵ�ָ���İ�
        }
        }*/

        if(st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_TotalMessageCount >= MAX_BUFFER_MESSAGE)
            return;  // ��Ϣ���������������ٴ�����Ϣ��
        MsgIndex = st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_MsgProducePointer;
        memcpy((void *) &st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].st_MSG[MsgIndex] ,(void *)st_Message, sizeof(St_DataMessage));
        ++ st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_TotalMessageCount;  // ���뻺���������ݡ�
        ++ st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_MsgProducePointer %= MAX_BUFFER_MESSAGE;
        ++ uw16_CALIBCh_TotalMSGCount;
    }
    else
    {
        if(st_RSNMSG_Manage[st_Message->uc_BoardID].uc_TotalMessageCount >= MAX_BUFFER_MESSAGE)
            return;  // ��Ϣ���������������ٴ�����Ϣ��
        MsgIndex = st_RSNMSG_Manage[st_Message->uc_BoardID].uc_MsgProducePointer;
        memcpy((void *) &st_RSNMSG_Manage[st_Message->uc_BoardID].st_MSG[MsgIndex] ,(void *)st_Message, sizeof(St_DataMessage));
        ++ st_RSNMSG_Manage[st_Message->uc_BoardID].uc_TotalMessageCount;  // ���뻺���������ݡ�
        ++ st_RSNMSG_Manage[st_Message->uc_BoardID].uc_MsgProducePointer %= MAX_BUFFER_MESSAGE;
    }
}

/*****************************************************
// Function Name:
// Description:
// History��
//
// ******************************************************/
void AuxConfigRSN::CleanMsg_RSN( unsigned char uc_Mode, unsigned char uc_Bid ,unsigned short int uw16_Chan)
{
    // �����Ϣ����
    unsigned char MsgIndex = 0;

    if(uc_Mode == MODEL_CHANNEL)
    {
        if(st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_TotalMessageCount <= 0)
            return;  // ��Ϣ�������ѿգ�����ȡ����Ϣ��

        st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_TotalMessageCount --;  // ��������������ݡ�
        MsgIndex = st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_MsgConsumePointer;
        st_CALIBCh_Manage[uc_Bid][uw16_Chan].st_MSG[MsgIndex].uc_MsgCode = 0XFF;
        st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_MsgConsumePointer ++;
        st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_MsgConsumePointer %= MAX_BUFFER_MESSAGE;
        if(uw16_CALIBCh_TotalMSGCount > 0)
            uw16_CALIBCh_TotalMSGCount--;
    }
    else
    {
        if(st_RSNMSG_Manage[uc_Bid].uc_TotalMessageCount <= 0)
            return;  // ��Ϣ�������ѿգ�����ȡ����Ϣ��

        st_RSNMSG_Manage[uc_Bid].uc_TotalMessageCount --;  // ��������������ݡ�
        MsgIndex = st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer;
        st_RSNMSG_Manage[uc_Bid].st_MSG[MsgIndex].uc_MsgCode = 0XFF;
        st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer ++;
        st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer %= MAX_BUFFER_MESSAGE;
    }

}

/*****************************************************
// Function Name:
// Description:
// History��
//
// ******************************************************/
void AuxConfigRSN::Check_RSN_Init( unsigned char uc_BoardID )
{
    // ��ʼ�������RSN��FRAM��I2C����Ƿ�һ��
    uc_Check_BoardRSN[uc_BoardID] = 1;
    uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
}







