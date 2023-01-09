#include "../Entry/includes.h"

//public:
unsigned short               AuxConfigRSN::Priority = PRIORITY8;  // PRIORITY4
unsigned short               AuxConfigRSN::TaskID = AUXBOARD_I2CRSN_TASK;

//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif


unsigned char                AuxConfigRSN::uc_FRAMRSN_Load[MAX_BOARD_PER_AUXMCU];  // RSN号已经加载过   // yy 20140912  解决T采样为零问题。
unsigned char                AuxConfigRSN::uc_BoardRSN_Counter;  // 全部板使用RSN号的计数器
unsigned char                AuxConfigRSN::uc_Board_RSNStatus[MAX_BOARD_PER_AUXMCU];      //FRAM 板的状态
unsigned char                AuxConfigRSN::uc_Channel_CALIBtatus[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   //校正 通道的状态
unsigned char                AuxConfigRSN::uc_RSN_JustWrite[MAX_BOARD_PER_AUXMCU];   // RSN刚成功写过
unsigned char                AuxConfigRSN::uc_WaitI2C_WriteBoard[MAX_BOARD_PER_AUXMCU]; // 等待I2C写板结束
unsigned char                AuxConfigRSN::uc_WaitI2C_WriteCh[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // 等待I2C写板中通道结束
unsigned char                AuxConfigRSN::uc_ReadBoard_Count[MAX_BOARD_PER_AUXMCU];    // 重复I2C读板次数计算器
unsigned char                AuxConfigRSN::uc_ReadCh_Count[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // 重复I2C读板中通道校正次数计算器
unsigned char                AuxConfigRSN::uc_Report_RSNError[MAX_BOARD_PER_AUXMCU]; //  报告错误代码
unsigned char                AuxConfigRSN::uc_Check_BoardRSN[MAX_BOARD_PER_AUXMCU];     // 检查RSN标志
unsigned char                AuxConfigRSN::uc_Case2_temp[MAX_BOARD_PER_AUXMCU];         // 临时作为传递标志用
unsigned char                AuxConfigRSN::uc_ScanBoardID;  //扫描板号
unsigned char                AuxConfigRSN::uc_ScanBChID;   //扫描板通道号
unsigned short               AuxConfigRSN::uw16_I2CHandleID_BD[MAX_BOARD_PER_AUXMCU];     //  I2C查询板信息号
unsigned short               AuxConfigRSN::uw16_I2CHandleID_CH[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   //  I2C查询通道信息号
MsTime                       AuxConfigRSN::st_IntervalTime_RSN[MAX_BOARD_PER_AUXMCU];       // 间隔时间检查RSN
MsTime                       AuxConfigRSN::st_IntervalTime_JustWriteRSN[MAX_BOARD_PER_AUXMCU];       // RSN刚成功写过间隔时间
MsTime                       AuxConfigRSN::st_Timeout_I2C_Board[MAX_BOARD_PER_AUXMCU];          // I2C读写板/报告错误等待超时时间
MsTime                       AuxConfigRSN::st_Timeout_I2C_Ch[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];       // I2C读写通道等待超时时间
St_MSG_Manage                AuxConfigRSN::st_RSNMSG_Manage[MAX_BOARD_PER_AUXMCU];      // 板MSG 管理
St_MSG_Manage                AuxConfigRSN::st_CALIBCh_Manage[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];      // 通道MSG 管理
unsigned short               AuxConfigRSN::uw16_CALIBCh_TotalMSGCount;
St_AUXChannelCFG             AuxConfigRSN::st_RSN_ReadCfg[MAX_AUXCH_PER_BOARD];  //  I2C读写通道信息
St_Check_RSN                 AuxConfigRSN::st_BoardRSN[MAX_BOARD_PER_AUXMCU];    // I2C板与FRAM板的RSN


static St_AUXChannelCFG  m_ChanCFG;

#pragma arm section //结束外存变量定义 



////////
//    Init(void)
//    : SDL模块初始化处理函数
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

        CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_RSN[uc_BoardID]); // 更新间隔系统时间
        CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // 更新间隔系统时间
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        memset((void*) &st_RSNMSG_Manage[uc_BoardID], 0, sizeof(St_MSG_Manage));
        for(unsigned char uc_j = 0; uc_j < MAX_BUFFER_MESSAGE; uc_j ++)
            st_RSNMSG_Manage[uc_BoardID].st_MSG[uc_j].uc_MsgCode = 0XFF;

        Check_RSN_Init( uc_BoardID );


        for(unsigned char uc_ChID = 0; uc_ChID < MAX_AUXCH_PER_AUXMCU; uc_ChID ++)
        {
            uc_Channel_CALIBtatus[uc_BoardID][uc_ChID] = READY;
            uc_WaitI2C_WriteCh[uc_BoardID][uc_ChID] = 0;
            uc_ReadCh_Count[uc_BoardID][uc_ChID] = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_ChID]); // 更新间隔系统时间
            memset((void*) &st_CALIBCh_Manage[uc_BoardID][uc_ChID], 0, sizeof(St_MSG_Manage));
            for(unsigned char uc_k = 0; uc_k < MAX_BUFFER_MESSAGE; uc_k ++)
                st_CALIBCh_Manage[uc_BoardID][uc_ChID].st_MSG[uc_k].uc_MsgCode = 0XFF;
        }
    }
}

////////
//    DoWork(void)
//    : SDL模块运行工作处理函数
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
        PreprocessMsg_RSN(uc_BoardID);       // 板信息
        ProcessState_BoardRSN(uc_BoardID);   // 板状态
        PreprocessMsg_ChCALIB(uc_BoardID);   // 通道信息
        ProcessState_ChCALIB(uc_BoardID);    // 通道状态
    }

    /*				 #if AUX_MULTI_FUNCTION_BOARD_DIDO == 1

    if( uc_DIDOFlag == 0)
    {
    if( (Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType == AUX_DI)
    || (Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType == AUX_DO) )    // 模拟制作多一块DIDO板  (用0号到6号板，不要用最后7号板）
    {
    st_BoardRSN[(uc_BoardID+1)%MAX_BOARD_PER_AUXMCU].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
    st_BoardRSN[(uc_BoardID+1)%MAX_BOARD_PER_AUXMCU].uw16_BoardRSN_FRAM = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
    uc_BoardID ++;  // 模拟制作多一块DIDO板 (用0号到6号板，不要用最后7号板）
    uc_DIDOFlag = 1;
    }
    }
    #endif            */
    //}

//     if(!m_AUX_Running)
//     {
//         if(m_AUX_WriteMap)
//         {
// 					   if(CommonFunc::GetTimeTicks( &m_AUX_Delay) >= 20000)    //超过2s
// 						 {
// 							 CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // 更新间隔系统时间
// 							 MP_AllIVMap  * st_AllIVMap;
// 							 st_AllIVMap = (MP_AllIVMap *) &EthernetParse::m_btPackDataBuffer[0];
// 							 memcpy(&m_AUX_CopyMap[0],&EthernetParse::m_btPackDataBuffer[0],3200);
// 							 AuxCConfig::SaveAuxUnit2IVMap(st_AllIVMap, 3200);  // 保存内容到FRAM
// 							 m_AUX_WriteMap = 0;
// 						 }
//         }
// 				else
// 					CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // 更新间隔系统时间
//     }
// 		else
// 			CommonFunc::GetTimeTicksUpdate(&m_AUX_Delay); // 更新间隔系统时间

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
// History：
//
// ******************************************************/
void AuxConfigRSN::PreprocessMsg_RSN(unsigned char uc_BoardID)
{
    // 接收信息处理
    unsigned char 	MsgIndex;      //取得当前响应信息编号

    unsigned char uc_RSNStatus = uc_Board_RSNStatus[uc_BoardID];                //当前的状态
    unsigned char uc_CleanFlag = 1;

    if( st_RSNMSG_Manage[uc_BoardID].uc_TotalMessageCount <= 0 )
        return;
    MsgIndex = st_RSNMSG_Manage[uc_BoardID].uc_MsgConsumePointer;

    // 真实板号，网络处理应将上位机逻辑号通道号转成实际对应号。
    switch(uc_RSNStatus)
    {
    case OPERATIONAL:
        switch(st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex].uc_MsgCode)
        {
        case MESSAGE_UPDATA_RSN:
            if(! uc_RSN_JustWrite[uc_BoardID])   // 检查该RSN是否已经被成功写过。  若没写过,分配RSN写入
            {
                uc_Board_RSNStatus[uc_BoardID] = REQUEST_WRITE_RSN;
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM = AuxConfigRSN::Produce_RSN(uc_BoardID);
                st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C = st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM;
            }
            else
            {
                uc_CleanFlag = 0;  // 间隔一定的时间再处理
            }
            break;
        case MESSAGE_WRITE_RSN:    // 增加响应通道校正发出的信息请求写RSN
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
        case MESSAGE_STOP_REPORT:   // 新增加上位机要求停止报告，就不要再报告了。主要针对I2C读写超时处理等。
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
            // 将Msg存入 Operational专用 Pool,
            st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex] =  st_RSNMSG_Manage[uc_BoardID].st_MSG[MsgIndex];
            uc_CleanFlag = 0;
            //break;
            return;  // 缓存，不取出处理。
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
// History：
//
// *****************************************************/
void AuxConfigRSN::ProcessState_BoardRSN(unsigned char uc_BoardID)
{
    //对于当前的各种状态进行处理，执行状态处理过程，响应命令，转换状态

    unsigned char uc_RSNStatus = uc_Board_RSNStatus[uc_BoardID];                //当前的状态
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

        if(CommonFunc::GetTimeTicks(&st_IntervalTime_RSN[uc_BoardID]) >= MAX_INTERVAL_TIME)    // 间隔4小时
        {
            if(st_BoardRSN[uc_BoardID].uc_I2CReadFlag)
            {
                uc_Check_BoardRSN[uc_BoardID] = 1;
                st_BoardRSN[uc_BoardID].uc_I2CReadFlag = 0;
            }
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_FRAM_STATA;
            CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_RSN[uc_BoardID]); // 更新间隔系统时间
            return;
        }

        if(uc_RSN_JustWrite[uc_BoardID])   // 检查该RSN是否已经被成功写过。  若写过,需要间隔一定的时间再处理。
        {
            if(CommonFunc::GetTimeTicks(&st_IntervalTime_JustWriteRSN[uc_BoardID]) >= TIMEOUT_1s)    // 间隔1秒
            {
                uc_RSN_JustWrite[uc_BoardID] = 0;
                CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // 更新间隔系统时间
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
            E2PROM_24LC128::WriteBuf[0]='B';   //测试用
            E2PROM_24LC128::WriteBuf[1]='I';
            E2PROM_24LC128::WriteBuf[2]='D';
            E2PROM_24LC128::WriteBuf[3]=MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID;  // 配置任务对I2C任务对接
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,0x02,0x04);
            uc_I2CReadFlag = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // 判板是否存在
            break;
        case I2C_TASK_R_BID:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,0x02,0x04);
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // 判板是否存在
            break;
        case I2C_TASK_R_B_INFO:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
            uw16_Addr = AUXBOARDCFGSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(ST_AUX_BOARD_INFO_SAVE));
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
            uc_ReadBoard_Count[uc_BoardID] = 0;   // 临时作为传递通道信息用
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // 板存在读出板信息
            break;
        case I2C_TASK_R_B_CH_INFO:
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
            uw16_Addr = AUXBOARDCHANNELCFGADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,MAX_I2C_BUFFER*Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Total_Channels);
            uc_I2CReadFlag = 0;
            uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
            uc_ReadBoard_Count[uc_BoardID] ++;   // 临时作为传递通道信息用
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_WR_WAIT; // 板存在读出通道信息
            break;
        default:
            // 一直询问I2C模块是否完成工作
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //已完成
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求

                if(uc_Case2_temp[uc_BoardID] == I2C_TASK_W_BID)
                {
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_BID;
                }
                else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_BID)
                {
                    E2PROM_24LC128::WriteBuf[0]='B';   //测试用
                    E2PROM_24LC128::WriteBuf[1]='I';
                    E2PROM_24LC128::WriteBuf[2]='D';
                    E2PROM_24LC128::WriteBuf[3]=MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID;
                    if(!memcmp(& E2PROM_24LC128::WriteBuf[0],&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],4))
                    {
                        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_B_INFO;
                    }
                    else
                    {
                        uc_Report_RSNError[uc_BoardID] = ERROR_RSN_HAVE_LOSTBOARD;// 报告错误代码
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
                        //uw16_Addr = 4+uc_I2CReadFlag*MAX_I2C_BUFFER;  // 结构前还有2个预留字节，后2类型补齐
                        //memcpy(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_Populated,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[uw16_Addr],sizeof(St_AUXChannelCFG));
                        uw16_Addr = uc_I2CReadFlag*MAX_I2C_BUFFER;
                        St_AUXChannelCFG  * m_Readcfg = (St_AUXChannelCFG * )&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[uw16_Addr];

                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_Populated = m_Readcfg->m_valid;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_ChnCoreInfo.m_AuxSensorCoreInfo = m_Readcfg->m_ChnCoreInfo.m_AuxSensorCoreInfo;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_AuxType= m_Readcfg->m_AuxType;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_AuxSubType = m_Readcfg->m_AuxSubType;
                        Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_I2CReadFlag].m_avg_num = m_Readcfg->m_AVG_num;
                    }
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS; // 板存在读出通道信息
                }
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //超过1秒
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // 时间还未超时等等吧！
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //超过1秒
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// 报告错误代码
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }

            if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_R_SUCCESS)
            {
                uc_WaitI2C_WriteBoard[uc_BoardID] = 0;// 成功操作读出
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
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        uc_Board_RSNStatus[uc_BoardID] = REPORTED_TO_PC;
        break;
    case REPORTED_TO_PC:
        if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_REPORT_TIMEOUT)    //超过10秒
        {
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
            //uc_Report_RSNError[uc_BoardID] = ERROR_RSN_WAITPC_TIMEOUT;// 报告错误代码
            uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;

            if(uc_Report_RSNError[uc_BoardID] == ERROR_RSN_NONE)
            {
                uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;  // 成功的报告，可回复原始状态，以避免成功也频频报告要上位机指令执行。
            }
        }

        St_DataMessage  st_Message;     //  默认上位机已回复一个要求更新RSN确认
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
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
            E2PROM_24LC128::WriteI2cBuf((UChar8 *) &st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,sizeof(UWord16));
            uw16_Addr = AUXBOARDRSNSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
            uc_I2CReadFlag = 0;
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        }
        else
        {
            // 一直询问I2C模块是否完成工作
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //已完成
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN;  // 成功操作写入
                uc_ReadBoard_Count[uc_BoardID] = 0;
                uc_Board_RSNStatus[uc_BoardID] = CONFIRM_WRITE_RSN;
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //超过1秒
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // 时间还未超时等等吧！
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //超过1秒
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// 报告错误代码
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }
        }
        break;
    case CONFIRM_WRITE_RSN:
        if(uc_WaitI2C_WriteBoard[uc_BoardID] == I2C_TASK_READ_RSN)
        {
            uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN_WAIT;
            uc_ReadBoard_Count[uc_BoardID] = 0;
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
            uw16_Addr = AUXBOARDRSNSRARTADDR;
            uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
            uc_I2CReadFlag = 0;
            uc_RSN_JustWrite[uc_BoardID] = 1;
            CommonFunc::GetTimeTicksUpdate(&st_IntervalTime_JustWriteRSN[uc_BoardID]); // 更新间隔系统时间
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        }
        else
        {
            // 一直询问I2C模块是否完成工作
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //已完成
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
                memcpy(&st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf,sizeof(UWord16));
                if(st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM == st_BoardRSN[uc_BoardID].uw16_BoardRSN_I2C)
                {
                    uc_WaitI2C_WriteBoard[uc_BoardID] = 0; // 成功操作读出
                    uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
                    break;
                }

                if(uc_ReadBoard_Count[uc_BoardID] < MAX_READ_RSN_TRY)
                {
                    uc_ReadBoard_Count[uc_BoardID]++ ;
                    uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_READ_RSN;  // 重复去读多次尝试比较结果
                }
                else
                {
                    uc_ReadBoard_Count[uc_BoardID] = 0;  // 失败操作读出
                    uc_Report_RSNError[uc_BoardID] = ERROR_RSN_WRITERSN_CMP;// 报告错误代码
                    uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
                }
            }
            else if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqErr)
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
            }
            else
            {
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //超过1秒
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // 时间还未超时等等吧！
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //超过1秒
            {
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;// 报告错误代码
                uc_Board_RSNStatus[uc_BoardID] = REPORT_TO_PC;
            }
        }
        break;

    default:
        uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
        break;
    }

    if(uc_RSNStatus != uc_Board_RSNStatus[uc_BoardID])
        AuxCConfig::WriteRSNStata(uc_BoardID, &uc_Board_RSNStatus[uc_BoardID]); // 花时间多的要马上存，花时间少的可在后面存。
}

/*****************************************************
// Function Name:
// Description:
// History：
//
// ******************************************************/
void AuxConfigRSN::PreprocessMsg_ChCALIB(unsigned char uc_BoardID)
{
    // 接收信息处理
    unsigned char 	MsgIndex;    //取得当前响应信息编号

    unsigned char uc_Channel = 0;
    unsigned char uc_ChCLIBtatus = 0;   //当前的状态
    unsigned char uc_I2CReadFlag = 0;
    unsigned char uc_CleanFlag = 1;

    if(uw16_CALIBCh_TotalMSGCount <= 0)
        return;

    for(uc_Channel = 0; uc_Channel < MAX_AUXCH_PER_BOARD; uc_Channel ++)
    {
        if( st_CALIBCh_Manage[uc_BoardID][uc_Channel].uc_TotalMessageCount == 0 )
            continue;  // 没有需要校正的通道信息

        uc_ChCLIBtatus = uc_Channel_CALIBtatus[uc_BoardID][uc_Channel];               //当前的状态
        MsgIndex = st_CALIBCh_Manage[uc_BoardID][uc_Channel].uc_MsgConsumePointer;

        // 真实板号真实通道号，网络处理应将上位机逻辑号通道号转成实际对应号。
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
// History：
//
// *****************************************************/
void AuxConfigRSN::ProcessState_ChCALIB(unsigned char uc_BoardID)
{
    //对于当前的各种状态进行处理，执行状态处理过程，响应命令，转换状态

    UWord16       uw16_AddrC3;
    unsigned char uc_ChStatus = 0;          // 当前的状态
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
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
            //                     uw16_AddrC3 = 4 + AUXBOARDCHANNELCFGADDR + uc_Ch*MAX_I2C_BUFFER; // 通道信息前还有两字节空闲，两个字类型。[出错地址没变过]
            uw16_AddrC3 = 16+uc_Ch*MAX_I2C_BUFFER  + AUXBOARDCHANNELCFGADDR; // 通道信息前还有两字节空闲，两个字类型。

            //                     uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,sizeof(St_AUXChannelCFG));//St_AUXChannelCFG
            uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::WriteRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,8);//St_AUXChannelCFG
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]); // 更新间隔系统时间
        }
        else
        {
            // 一直询问I2C模块是否完成工作
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqFin) //已完成
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
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
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]) >= MAX_I2C_TIMEOUT)    //超过1秒
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // 时间还未超时等等吧！
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //超过1秒
            {
                st_SPC3 = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;    // 因没有PC报告状态，只能直接报告
                st_SPC3->m_AuxBoard.m_btBoard_ID = uc_BoardID;
                //                         st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
                //                         st_SPC3->m_AuxBoard.m_btAux_SubType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[0].m_AuxTemp.m_AuxThermoCouple.m_btAuxSubType;
                //                         st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_ChannelCount;
                st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
                st_SPC3->m_AuxBoard.m_btAux_SubType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSubType;
                st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Max_Channels;
                uc_CopybackC3 = uc_Report_RSNError[uc_BoardID];    // 报告错误代码
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;    // 报告错误代码
                EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
                uc_Report_RSNError[uc_BoardID] = uc_CopybackC3;    // 报告错误代码
                uc_Channel_CALIBtatus[uc_BoardID][uc_Ch] = READY;
            }
        }
        break;
    case CONFIRM_WRITE_CALIB:
        if(uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] == I2C_TASK_R_CH_CALIB)
        {
            uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = I2C_TASK_R_CH_CALIB_WAIT;
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
            //uw16_AddrC3 = 4 + AUXBOARDCHANNELCFGADDR + uc_Ch*MAX_I2C_BUFFER; // 通道信息前还有两字节空闲，两个字类型。[出错地址没变过]
            uw16_AddrC3 = uc_Ch*MAX_I2C_BUFFER + 4 + AUXBOARDCHANNELCFGADDR; // 通道信息前还有两字节空闲，两个字类型。
            uw16_I2CHandleID_CH[uc_BoardID][uc_Ch] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_AddrC3,sizeof(St_AUXChannelCFG));
            CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]); // 更新间隔系统时间
        }
        else
        {
            // 一直询问I2C模块是否完成工作
            uc_I2CReadFlag = 0;
            if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch]) == I2C_ReqFin) //已完成
            {
                I2CBusNew::SetRequestStatus(uw16_I2CHandleID_CH[uc_BoardID][uc_Ch],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
                //if(!memcmp(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[uc_Ch].m_AuxTemp.m_AuxThermoCouple.TempChType,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],sizeof(St_AUXChannelCFG)))
                if(!memcmp(&Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_Populated,&I2CBusNew::RunningBuffer[E2PROM_I2C_PORT].Buf[0],sizeof(St_AUXChannelCFG)))
                {
                    uc_WaitI2C_WriteCh[uc_BoardID][uc_Ch] = 0; // 成功操作读出
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
                if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Ch[uc_BoardID][uc_Ch]) >= MAX_I2C_TIMEOUT)    //超过1秒
                    uc_I2CReadFlag = I2C_TASK_R_ERROR;

                // 时间还未超时等等吧！
            }

            if(uc_I2CReadFlag == I2C_TASK_R_ERROR)    //超过1秒
            {
                st_SPC3 = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;      // 因没有PC报告状态，只能直接报告
                st_SPC3->m_AuxBoard.m_btBoard_ID = uc_BoardID;
                //                         st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_BType.m_AuxType;
                //                         st_SPC3->m_AuxBoard.m_btAux_SubType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_AuxSensor_ChInfo[0].m_AuxTemp.m_AuxThermoCouple.m_btAuxSubType;
                //                         st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_ChannelCount;
                st_SPC3->m_AuxBoard.m_btAuxType = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxType;
                st_SPC3->m_AuxBoard.m_btAux_SubType =Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_chn[uc_Ch].m_AuxSubType;
                st_SPC3->m_AuxBoard.m_u8ChCount = Aux_DataManager::m_AllAuxBoards[uc_BoardID].m_BoardInfo.m_abi_globe.m_Max_Channels;
                uc_CopybackC3 = uc_Report_RSNError[uc_BoardID];    // 报告错误代码
                uc_Report_RSNError[uc_BoardID] = ERROR_RSN_I2CFUN_TIMEOUT;    // 报告错误代码
                EthernetParse::MPReportData(0,SEND_ERROR_MESSAGE);
                uc_Report_RSNError[uc_BoardID] = uc_CopybackC3;    // 报告错误代码
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
// History：
//
// *****************************************************/
UWord16 AuxConfigRSN::Produce_RSN(unsigned char uc_BoardID)
{
    // 编制RSN号码
    // RSN的16bit整数生成办法： 前8位用系统当前时间的微秒计算作为随机数，
    //                         中间5位作为递增加运算检查各个板中的是否为不同，
    //                         如果不同保留最大的作为公共的开始数起始点，后3位作为板号进行相与运算。
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
// History：
//
// *****************************************************/
void AuxConfigRSN::Check_BoardRSN_TaskCase(unsigned char uc_BoardID)
{
    // 用FRAM里面的RSN号，与I2C里的RSN号进行比较

    unsigned char  uc_I2CReadFlag = 0;
    //     unsigned char  uc_FRAMReadFlag = 0;
    //     unsigned char  uc_RSN_Stata = 0;
    //     unsigned short int uw16_RSN_I2C = 0;
    //     unsigned short int uw16_RSN_FRAM = 0;
    //     unsigned short int uw16_TempValue = 0;        //比较值
    unsigned short int uw16_Addr = 0;

    switch(uc_WaitI2C_WriteBoard[uc_BoardID])
    {
    case I2C_TASK_FRAM_STATA:
        if(!uc_FRAMRSN_Load[uc_BoardID])   // yy 20140912  解决T采样为零问题。
        {
            AuxCConfig::ReadRSNStata(uc_BoardID, &st_BoardRSN[uc_BoardID].uc_RSN_Stata); //读出状态
        }
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    case I2C_TASK_FRAM_RSN:
        if(!uc_FRAMRSN_Load[uc_BoardID])   // yy 20140912  解决T采样为零问题。
        {
            st_BoardRSN[uc_BoardID].uc_FRAMReadFlag = AuxCConfig::ReadRSN(uc_BoardID ,&st_BoardRSN[uc_BoardID].uw16_BoardRSN_FRAM); //读出RSN
            uc_FRAMRSN_Load[uc_BoardID] = 1;
        }
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    case I2C_TASK_R_RSN:
        CommonFunc::GetTimeTicksUpdate(&st_Timeout_I2C_Board[uc_BoardID]); // 更新间隔系统时间
        uw16_Addr = AUXBOARDRSNSRARTADDR;
        uw16_I2CHandleID_BD[uc_BoardID] = E2PROM_24LC128::ReadRequest(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID,uw16_Addr,sizeof(UWord16));
        uc_Case2_temp[uc_BoardID] = uc_WaitI2C_WriteBoard[uc_BoardID];  // 临时作为传递标志用
        uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN_WAIT;
        break;
    default:
        if((uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_STATA)||(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_RSN))
        {
            //if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= MAX_I2C_TIMEOUT)    //超过1秒   //  20140915  yy  解决启动后读RSN比较慢问题为45秒提升到多板10秒左右完成。
            //{
            if(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_STATA)
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_FRAM_RSN;
            else if(uc_Case2_temp[uc_BoardID] == I2C_TASK_FRAM_RSN)
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_RSN;
            //}
            return;
        }

        // 一直询问I2C模块是否完成工作
        unsigned char  uc_I2CReadFlag = 0;
        if(I2CBusNew::GetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID]) == I2C_ReqFin) //已完成
        {
            I2CBusNew::SetRequestStatus(uw16_I2CHandleID_BD[uc_BoardID],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求

            if(uc_Case2_temp[uc_BoardID] == I2C_TASK_R_RSN)
            {
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS; // 成功操作读出
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
            if(CommonFunc::GetTimeTicks(&st_Timeout_I2C_Board[uc_BoardID]) >= (MAX_I2C_TIMEOUT + 0.01*TIMEOUT_1s*TIMEOUT_1s))    //超过1秒
            {
                uc_I2CReadFlag = I2C_TASK_R_ERROR;
                uc_WaitI2C_WriteBoard[uc_BoardID] = I2C_TASK_R_SUCCESS;
            }
            // 时间还未超时等等吧！
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
// History：
//
// *****************************************************/
void AuxConfigRSN::Check_BoardRSN(St_Check_RSN * St_RSN)
{
    // 用FRAM里面的RSN号，与I2C里的RSN号进行比较
    unsigned short int uw16_TempValue = 0;        //比较值

    //     AuxCConfig::ReadRSNStata(uc_BoardID, &uc_RSN_Stata); //读出状态
    //
    //     uc_FRAMReadFlag = AuxCConfig::ReadRSN(uc_BoardID ,&uw16_BoardRSN_FRAM[uc_BoardID]); //读出RSN
    //     uw16_RSN_FRAM = uw16_BoardRSN_FRAM[uc_BoardID];    //当前的状态FRAM
    //
    //     //uc_I2CReadFlag = AuxCConfig::ReadAUXBoardRSN(uc_BoardID ,&uw16_BoardRSN_I2C[uc_BoardID]);;   // 存入I2C的24LC128
    //     //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
    //     //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
    //     uc_I2CReadFlag = AuxCConfig::ReadAUXBoardRSN(MAX_BOARD_PER_AUXMCU - 1 - uc_BoardID ,&uw16_BoardRSN_I2C[uc_BoardID]);
    //     uw16_RSN_I2C = uw16_BoardRSN_I2C[uc_BoardID];     //当前的状态I2C
    //

    uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_NONE;// 报告没有错误
    uw16_TempValue = St_RSN->uw16_BoardRSN_I2C - St_RSN->uw16_BoardRSN_FRAM;  // 比较结果处理
    if(St_RSN->uc_I2CReadFlag)
    {   // 读到I2C的RSN
        if((St_RSN->uw16_BoardRSN_I2C == 0)||(St_RSN->uw16_BoardRSN_I2C == 0XFFFF))
        {
            uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;  //  发现一块没有用ListenForNet写过的板
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// 报告错误
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
            return;
        }

        if(St_RSN->uc_FRAMReadFlag)
        {   // 读到SPI FRAM的RSN
            switch(uw16_TempValue)
            {
            case RSN_COMPARE_SAME:
                switch(St_RSN->uc_RSN_Stata)   //  板正常情况
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
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_NONE;// 报告没有错误
                break;

            case RSN_COMPARE_NEW:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;  // 发现新板
                AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// 报告错误
                break;

            case RSN_COMPARE_DIFFE:
            default:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO; // 发现用过的板
                AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_OTHERSYSTEMUSEBOARD;// 报告错误
                break;
            }
        }
        else
        {   // 没有读到SPI FRAM的RSN
            switch(uw16_TempValue)
            {
            case RSN_COMPARE_NEW:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO;  // 发现新板
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_SYSTEMFINDBOARD;// 报告错误
                break;

            case RSN_COMPARE_DIFFE:
            default:
                uc_Board_RSNStatus[St_RSN->uc_BoardID] = RSN_DIRTY_WO_INFO; // SysteConfig配置之前发现了一块别的系统用过的板
                uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_OTHERSYSTEMUSEBOARD;// 报告错误
                break;
            }
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
        }
    }
    else
    {   // 没有读到I2C的RSN
        if((St_RSN->uw16_BoardRSN_FRAM == 0)||(St_RSN->uw16_BoardRSN_FRAM == 0XFFFF))
        {
            St_RSN->uc_FRAMReadFlag = 0;  //  发现没有插的板，并且没写过FRAM中的RSN，则不处理。
        }

        if(St_RSN->uc_FRAMReadFlag)
        {
            uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;  //  没有读到I2C的RSN   丢板   发现一块原来存在过的板现在在系统里找不到了
            AuxCConfig::WriteRSNStata(St_RSN->uc_BoardID, &uc_Board_RSNStatus[St_RSN->uc_BoardID]);
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_HAVE_LOSTBOARD;// 报告错误
        }
        else
        {   // 没有读到SPI FRAM的RSN
            //uc_Board_RSNStatus[St_RSN->uc_BoardID] = REPORT_TO_PC;
            uc_Report_RSNError[St_RSN->uc_BoardID] = ERROR_RSN_LOST;// 报告错误
            return;   // 两者都没有，则为正常
        }
    }
}

/*****************************************************
// Function Name:
// Description:
// History：
//
// ******************************************************/
void AuxConfigRSN::PushMsg_RSN( unsigned char uc_Mode, St_DataMessage * st_Message )
{
    // 发送信息处理
    unsigned char SuccessFlag = 0;
    unsigned char MsgIndex;
    unsigned char uc_Type = st_Message->uc_AuxType;
    unsigned char uc_Bid = st_Message->uc_BoardID;
    unsigned short int uw16_Chan = st_Message->uw16_ChannelID;   // 逻辑号

    if(uc_Mode == MODEL_CHANNEL)
    {
        /* for(uc_Bid = 0; uc_Bid < MAX_BOARD_PER_AUXMCU; uc_Bid ++) // 计算顺序排列时的通道信息存放位置
        {

        //             if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_BType.m_AuxType == uc_Type)  // Aux type
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_Start_InUnit <= uw16_Chan) // Aux Channel
        &&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_End_InUnit >= uw16_Chan))
        {
        st_Message->uc_BoardID = uc_Bid;
        st_Message->uw16_ChannelID = uw16_Chan - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_Start_InUnit;
        SuccessFlag = 1;
        break;    // 找到指定的板
        }
        }*/

        if(st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_TotalMessageCount >= MAX_BUFFER_MESSAGE)
            return;  // 信息缓冲区已满，则不再存入信息。
        MsgIndex = st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_MsgProducePointer;
        memcpy((void *) &st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].st_MSG[MsgIndex] ,(void *)st_Message, sizeof(St_DataMessage));
        ++ st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_TotalMessageCount;  // 存入缓冲区内数据。
        ++ st_CALIBCh_Manage[st_Message->uc_BoardID][st_Message->uw16_ChannelID].uc_MsgProducePointer %= MAX_BUFFER_MESSAGE;
        ++ uw16_CALIBCh_TotalMSGCount;
    }
    else
    {
        if(st_RSNMSG_Manage[st_Message->uc_BoardID].uc_TotalMessageCount >= MAX_BUFFER_MESSAGE)
            return;  // 信息缓冲区已满，则不再存入信息。
        MsgIndex = st_RSNMSG_Manage[st_Message->uc_BoardID].uc_MsgProducePointer;
        memcpy((void *) &st_RSNMSG_Manage[st_Message->uc_BoardID].st_MSG[MsgIndex] ,(void *)st_Message, sizeof(St_DataMessage));
        ++ st_RSNMSG_Manage[st_Message->uc_BoardID].uc_TotalMessageCount;  // 存入缓冲区内数据。
        ++ st_RSNMSG_Manage[st_Message->uc_BoardID].uc_MsgProducePointer %= MAX_BUFFER_MESSAGE;
    }
}

/*****************************************************
// Function Name:
// Description:
// History：
//
// ******************************************************/
void AuxConfigRSN::CleanMsg_RSN( unsigned char uc_Mode, unsigned char uc_Bid ,unsigned short int uw16_Chan)
{
    // 清除信息处理
    unsigned char MsgIndex = 0;

    if(uc_Mode == MODEL_CHANNEL)
    {
        if(st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_TotalMessageCount <= 0)
            return;  // 信息缓冲区已空，则不再取出信息。

        st_CALIBCh_Manage[uc_Bid][uw16_Chan].uc_TotalMessageCount --;  // 清除缓冲区内数据。
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
            return;  // 信息缓冲区已空，则不再取出信息。

        st_RSNMSG_Manage[uc_Bid].uc_TotalMessageCount --;  // 清除缓冲区内数据。
        MsgIndex = st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer;
        st_RSNMSG_Manage[uc_Bid].st_MSG[MsgIndex].uc_MsgCode = 0XFF;
        st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer ++;
        st_RSNMSG_Manage[uc_Bid].uc_MsgConsumePointer %= MAX_BUFFER_MESSAGE;
    }

}

/*****************************************************
// Function Name:
// Description:
// History：
//
// ******************************************************/
void AuxConfigRSN::Check_RSN_Init( unsigned char uc_BoardID )
{
    // 初始化，检查RSN号FRAM与I2C里的是否一致
    uc_Check_BoardRSN[uc_BoardID] = 1;
    uc_Board_RSNStatus[uc_BoardID] = OPERATIONAL;
}







