/*****************************************************************************
// 程序文件                   :EthernetParse.cpp     主要用于网络处理
// 文件描述                   :为 MCU board服务
// 编写日期(MM.DD.YYYY)       :10.15.2012
*****************************************************************************/

#include "../Entry/includes.h"


//====================
UInt16 EthernetParse::MyStatus;
UInt16 EthernetParse::Priority = PRIORITY4;  // PRIORITY2;
UInt16 EthernetParse::TaskID = ETHERNETPARS_ETASK;	 // ETHERNETPARSETASK should be a definition which positioning this task in OS_Task[]

// unsigned char  EthernetParse::m_bP1Broken;
// unsigned char  EthernetParse::m_bP2Broken;
unsigned short EthernetParse::m_u16Token;
unsigned char  EthernetParse::m_ucTokenBrokenCount;
MsTime         EthernetParse::m_WatchDogTime;
unsigned char  EthernetParse::m_bResetSchedule;
unsigned char  EthernetParse::m_bResetThirdParty;
// unsigned char  Debug_Unsafeflg = 0;
QueueP         EthernetParse::Temp_Q;             // 临时使用字符串队列指针
// 设置变量定义在外存

// unsigned char   EthernetParse::LastDigitOfIPMAC = 55;
unsigned char   EthernetParse::uc_LogCriticalPoint = 0;    // 记录关键点标志
unsigned char   EthernetParse::m_IfRequest_NTP = 1;

U8 uc_Net_Err = 0xA5;
U32 uc_Net_Err_Counter = 0;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
__align(8) uint8 			EthernetParse::m_CmdRxBuffer[MAX_CMD_BUFFER_SIZE];
__align(8) uint8 			EthernetParse::m_CmdTxBuffer_SynchroChannel[MAX_CMD_BUFFER_SIZE]; // 同步通道  synchro
__align(8) uint8 			EthernetParse::m_CmdTxBuffer_Async_Temp[MAX_CMD_BUFFER_SIZE]; // @异步通道 async 暂时组包缓冲区

//__align(8) uint8 			EthernetParse::m_Point_Temp[20]; // 取点结构缓冲区
__align(8) uint8 			EthernetParse::m_Point_Temp[MAX_PACKLIMIT_SIZE]; // 取点结构缓冲区

uint8* 			EthernetParse::m_CmdRxBuf;         //  接收同步信道缓冲区指针
uint8* 			EthernetParse::m_CmdTxBufSynchro;  //  发送同步信道缓冲区指针

uint32			EthernetParse::m_CmdRxLen;
uint32			EthernetParse::m_SyncByteCnt;

St_UnsafeLimit  EthernetParse::UnsafeLimit[MAXCHANNELNO];   //for  Limit Unsafe  20140211

unsigned short  EthernetParse::uw16_MyIP[2];             // "MYIP1.MYIP2.MYIP3.MYIP4"    网址
unsigned short  EthernetParse::uw16_RemoteIP[2];
unsigned char   EthernetParse::uc_MyMAC[6];             // "M1-M2-M3-M4-M5-M6"
// UChar8          EthernetParse::uc_EEP_RemoteIP[4];


unsigned char   EthernetParse::uc_Total_Flag;

Un_PC_MESSAGE*		         EthernetParse::m_pCmd;
Un_MC_FEEDBACK_MESSAGE*	     EthernetParse::m_pFeedBack;

Un_MC_REPORT*	             EthernetParse::m_pReport;

//St_RecordTime			     EthernetParse::m_CmdLastStamp;
//St_RecordTime			     EthernetParse::m_CmdIVLastStamp;
//St_RecordTime			     EthernetParse::m_CommLastStamp;

// uint32 			             	EthernetParse::m_dwPackDataBuffer_TotalLength;
// uint32 			             	EthernetParse::m_dwPackDataBuffer_SavePoint;
// uint32 			             	EthernetParse::m_dwPackDataBuffer_GetPoint;
// uint8 			             	EthernetParse::m_btPackDataBuffer_IsUseFlag;
// uint8* 			             	EthernetParse::m_pPackDataBuffer;
// uint8 			             	EthernetParse::m_ucFull_Count;
MsTime                  	EthernetParse::m_IntervalTime_RequestSystemtime;

MP_CONFIRM_FEEDBACK          	EthernetParse::m_ResultFeedback;
ST_MultPack                  	EthernetParse::st_MultPackManage;
//St_SimuDownMgr               	EthernetParse::m_SimuDownMgr;
St_Assign*                  	EthernetParse::m_st_Assign;
St_MD5Assign*				 					EthernetParse::m_simu_MD5Assign;  //12.22.2013ADD
ST_STATUS_SCHEDULE           	EthernetParse::m_ReportStatus;

__align(8) uint8 			 				EthernetParse::m_btPackDataBuffer[MAX_MULTPACK_SIZE]; // 用于多包下载时的缓冲暂存
__align(8) uint8 			 				EthernetParse::m_btPackDataUploadBuffer[MAX_MULTPACK_UPLOAD_SIZE]; // 用于多包上传时的缓冲暂存



TestDataTxBuffer11				EthernetParse::TestDataTxBuffer;
TestEventTxBuffer12				EthernetParse::TestEventTxBuffer;
//TestDataTxIndexQueue13		EthernetParse::TestDataTxIndexQueue; // 数据命令结构个数管理,相当于指针
TestDataTxPackEndQueue13			EthernetParse::TestDataTxPackEndQueue;

uint8	            EthernetParse::Pack_Tx_Buff_1500[MAX_PACKLIMIT_SIZE]; //  Pack_Tx_Buff_1500[1500];
uint16	            EthernetParse::Pack_Tx_Buff_1500_used_depth;
uint16              EthernetParse::DataTx_Length;      //  发送@异步信道数据缓冲区数据长度
uint16              EthernetParse::EventTx_Length;     //  发送@异步信道事件缓冲区数据长度
uint32              EthernetParse::m_dwSend_SN;      //  发送事件编号
MsTime              EthernetParse::EventTxCounter;
MsTime              EthernetParse::DataTxCounter;
// MsTime              EthernetParse::m_Interval_NetLinkTime;    // 网络连接成功时间间隔器
MsTime              EthernetParse::SetIVBoardTypeTime;
unsigned char       EthernetParse::SetIVBoardTypeFlag;
MsTime              EthernetParse::SetAuxBoardTypeTime;
unsigned char       EthernetParse::SetAuxBoardTypeFlag;
unsigned char       EthernetParse::uc_SendFail = 0;
UWord16             EthernetParse::Pack_1500_depth_back = 0;
UWord16             EthernetParse::uw16_SendCounter = 0;
unsigned long       EthernetParse::AbortTime;
unsigned long long  EthernetParse::uw64_CTI_SetTimeInterval;

#if  USE_AS_AUXMCU == 1
unsigned char       EthernetParse::m_AUX_WriteMap;
#endif

#pragma arm section //结束外存变量定义

#if DEBUG_TEST_COMMDATA_MODE == 1
UWord32 EthernetParse::uw32_TestDataPack; // 计算已发数据包的包号 //  仅为测试用，成型后要清理丢。
UWord32 EthernetParse::uw32_TestEventNum; // 计算已发信息号 //  仅为测试用，成型后要清理丢。
UWord32 EthernetParse::uw32_TestPackSum; // 计算已发包的包号 //  仅为测试用，成型后要清理丢。
UWord32 EthernetParse::uw32_TestBlock1; // 计算发送阻塞次数 //  仅为测试用，成型后要清理丢。
UWord32 EthernetParse::uw32_TestLogD[MAXCHANNELNO]; // 计算各个通道已记录的点数 //  仅为测试用，成型后要清理丢。
//extern U32 uw32_TestSumRevice;
#endif
// extern U32 uw32_TestSumRevice;

extern REMOTEM rm;
// extern unsigned char m_NetLinkFlag;
// extern          uint8 	   uc_SendLock;



//=======================================//

const ST_SCHEDULE_LOGSETTING_DATA  	EthernetParse::m_ReportLogMetaVariable = { 75,
                                                                               MetaCode_PV_CycleIndex, \
                                                                               MetaCode_TC_ChargeCapacity1, \
                                                                               MetaCode_TC_ChargeCapacity2, \
                                                                               MetaCode_TC_DischargeCapacity1, \
                                                                               MetaCode_TC_DischargeCapacity2, \

                                                                               MetaCode_TC_ChargeEnergy1, \
                                                                               MetaCode_TC_ChargeEnergy2, \
                                                                               MetaCode_TC_DischargeEnergy1, \
                                                                               MetaCode_TC_DischargeEnergy2, \
                                                                               MetaCode_TC_Timer1, \

                                                                               MetaCode_TC_Timer2, \
                                                                               MetaCode_TC_Timer3, \
                                                                               MetaCode_TC_Timer4, \
                                                                               MetaCode_LS_Voltage, \
                                                                               MetaCode_LS_Current, \

                                                                               MetaCode_LS_StepTime, \
                                                                               MetaCode_LS_TestTime, \
                                                                               MetaCode_LS_StepIndex, \
                                                                               MetaCode_LS_CycleIndex, \
                                                                               MetaCode_LS_ChargeCapacity, \

                                                                               MetaCode_LS_DischargeCapacity, \
                                                                               MetaCode_LS_ChargeEnergy, \
                                                                               MetaCode_LS_DischargeEnergy, \
                                                                               MetaCode_LS_InternalResistance, \
                                                                               MetaCode_LS_ACR, \

                                                                               MetaCode_LS_dVdt, \
                                                                               MetaCode_LS_dIdt, \
                                                                               MetaCode_LS_AC_Impedance, \
                                                                               MetaCode_LS_VmaxOnCycle, \
                                                                               MetaCode_LC_Voltage, \

                                                                               MetaCode_LC_Current, \
                                                                               MetaCode_LC_StepTime, \
                                                                               MetaCode_LC_TestTime, \
                                                                               MetaCode_LC_StepIndex, \
                                                                               MetaCode_LC_CycleIndex, \

                                                                               MetaCode_LC_ChargeCapacity, \
                                                                               MetaCode_LC_DischargeCapacity, \
                                                                               MetaCode_LC_ChargeEnergy, \
                                                                               MetaCode_LC_DischargeEnergy, \
                                                                               MetaCode_LC_InternalResistance, \

                                                                               MetaCode_LC_ACR, \
                                                                               MetaCode_LC_dVdt, \
                                                                               MetaCode_LC_dIdt, \
                                                                               MetaCode_LC_AC_Impedance, \
                                                                               MetaCode_LC_VmaxOnCycle, \

                                                                               MetaCode_PV_StepIndex, \
                                                                               MetaCode_PV_Steptime, \
                                                                               MetaCode_PV_TestTime, \
                                                                               MetaCode_PV_CV_StageTime, \
                                                                               MetaCode_PV_ChargeCapacityTime, \

                                                                               MetaCode_PV_DischargeCapacityTime, \
                                                                               MetaCode_PV_ChargeCapacity, \
                                                                               MetaCode_PV_DischargeCapacity, \
                                                                               MetaCode_PV_ChargeEnergy, \
                                                                               MetaCode_PV_DischargeEnergy, \

                                                                               MetaCode_TC_Counter1, \
                                                                               MetaCode_TC_Counter2, \
                                                                               MetaCode_TC_Counter3, \
                                                                               MetaCode_TC_Counter4, \
                                                                               MetaCode_MV_UD1, \

                                                                               MetaCode_MV_UD2, \
                                                                               MetaCode_MV_UD3, \
                                                                               MetaCode_MV_UD4, \
                                                                               MetaCode_MV_UD5, \
                                                                               MetaCode_MV_UD6, \
                                                                               MetaCode_MV_UD7, \
                                                                               MetaCode_MV_UD8, \
                                                                               MetaCode_MV_UD9, \
                                                                               MetaCode_MV_UD10, \
                                                                               MetaCode_MV_UD11, \
                                                                               MetaCode_MV_UD12, \
                                                                               MetaCode_MV_UD13, \
                                                                               MetaCode_MV_UD14, \
                                                                               MetaCode_MV_UD15, \
                                                                               MetaCode_MV_UD16, \
                                                                               0, \
                                                                               0, \
                                                                             };   //1807131

////////
// DoWork(void) : 网络命令解析任务处理主函数
// Input: N/A
// Return: N/A
///////
void EthernetParse::DoWork(void)
{
    // 网络命令解析任务处理主函数
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_ETHERNET] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_ETHERNET);
#endif

// 	if(!m_NetLinkFlag)
// 	{
// 		if(CommonFunc::GetTimeTicks( &m_Interval_NetLinkTime) >= 250000)    //超过25s
// 		{
// #if MCU_WATCHDOG_ENABLE == 1
// 			WDTFunc::Watchdog_SetLongTime(WDT_FEED_VALUE_67s);
// #endif

// 			OS_ServeProcess::Init_NetWork();    //  Fix bug link net.  201407029  yangy  i
// 			CommonFunc::GetTimeTicksUpdate( &m_Interval_NetLinkTime); // 更新网络连接成功时间间隔器

// #if MCU_WATCHDOG_ENABLE == 1
// 			WDTFunc::Watchdog_SetLongTime(WDT_FEED_VALUE);
// #endif
// 		}
// 		else
// 			return;
// 	}


    main_TcpNet(); //  网络管理
    CmdParse(); //  网络解析包
    //CheckIfResetEthernet();

#if (USE_AS_AUXMCU == 0 )
    SIMU_BUFFER::DoWork();

#if (SCH_STATUS_REPORT_ENABLE  == 1)
    ScheduleStatusReport::SCHStatusReport_DoWork();	 // SCH 状态定阅报告
#endif
#endif
    /*if(!OS_ServeProcess::m_bTimeOK)
    {   // 判断如果从没有更新过系统时间，则请求更新一次系统时间。
    if(CommonFunc::GetTimeTicks( &m_IntervalTime_RequestSystemtime) >= 600000)    //超过60s
    {
    EthernetParse::MPReportData(0, SEND_REQUEST_SYSTEMTIME);
    CommonFunc::GetTimeTicksUpdate( &m_IntervalTime_RequestSystemtime); // 更新间隔请求系统时间
    }

    }*/

    //以太网DoWork处理中，发送之前的预处理
    PrepareTxRawDataForSending();

    //20130904,add by hepeiqing,for software watchdog function
    if(CommonFunc::TimeDiffInSecond(m_WatchDogTime.Second) > COM_CHANNEL1_TIMEOUT)
    {
        m_WatchDogTime = CommonFunc::GetSystemTime();

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
        return;       //  20141014  yy  Fix of  60s stop problem.
#endif

        // m_bP1Broken = 1;
        m_bResetSchedule = eEMERGENCYSTOP_ERROR_WatchDogOverTime;
        m_bResetThirdParty = 1;
    }

#if USE_AS_AUXMCU==0
    if(SetIVBoardTypeFlag)
    {
        if(CommonFunc::GetTimeTicks( &SetIVBoardTypeTime) >= 100000)    //超过10s
        {
            SetIVBoardTypeFlag = 0;
            CommonFunc::GetTimeTicksUpdate( &SetIVBoardTypeTime);
            WDTFunc::ResetMcu();
        }
    }
#else
    if(SetAuxBoardTypeFlag)
    {
        if(CommonFunc::GetTimeTicks( &SetAuxBoardTypeTime) >= 30000)    //超过3s
        {
            SetAuxBoardTypeFlag = 0;
            CommonFunc::GetTimeTicksUpdate( &SetAuxBoardTypeTime);
            WDTFunc::ResetMcu();
        }
    }
#endif

    if(uc_Net_Err != 0xA5)
    {
        uc_Net_Err_Counter ++;
        uc_Net_Err = 0xA5;
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_ETHERNET] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_ETHERNET);
#endif
}


////////
// MyInit(void) : 网络命令解析任务处理初始化函数
// Input: N/A
// Return: N/A
///////
void EthernetParse::MyInit(void)
{
    // 网络命令解析任务处理初始化函数,缓冲区数据初始化函数
    m_SyncByteCnt = CODE_EMPTY;
    m_CmdRxLen = CODE_EMPTY;

    m_CmdRxBuf = &m_CmdRxBuffer[CODE_EMPTY];
    m_CmdTxBufSynchro =  &m_CmdTxBuffer_SynchroChannel[CODE_EMPTY];

    m_dwSend_SN = 0;
// 	for(uint16 k = 0; k < MAX_CMD_BUFFER_SIZE; k++)
// 	{
// 		m_CmdRxBuf[k] = CODE_EMPTY;
// 	}
    memset(&m_CmdRxBuf[0], CODE_EMPTY, MAX_CMD_BUFFER_SIZE);

    m_pCmd = (Un_PC_MESSAGE*)&m_CmdRxBuf[4];  //  接收包指针

    m_pFeedBack = (Un_MC_FEEDBACK_MESSAGE*)&m_CmdTxBufSynchro[CMD_SYNC_LEN];  //  发送同步包指针 [1Byte(0X06)]

    m_pReport = (Un_MC_REPORT*)&m_CmdTxBuffer_Async_Temp[CMD_HOLDHAND_LENGTH];  //  发送异步包指针 [8Byte(AA)+4Byte(Length)+4Byte(Reserved]

    m_CmdRxLen = CODE_EMPTY;
// 	m_ucFull_Count = CODE_EMPTY;
    m_SyncByteCnt = CODE_EMPTY;
    DataTx_Length = CODE_EMPTY;
    EventTx_Length = CODE_EMPTY;

    // 初始化
    // 三个缓冲区清空。
    INIT_QUEUE(TestDataTxBuffer);
    INIT_QUEUE(TestEventTxBuffer);
    //INIT_QUEUE(TestDataTxIndexQueue);
    INIT_QUEUE(TestDataTxPackEndQueue);
    Pack_Tx_Buff_1500_used_depth = 0;

    //	m_CmdLastStamp = OS_ServeProcess::OS_Time;
    //	m_CommLastStamp = OS_ServeProcess::OS_Time;

//     m_bP1Broken = 0;
//     m_bP2Broken = 0;
    m_u16Token = 0xFFFE;
    m_ucTokenBrokenCount = 0;
    m_WatchDogTime = CommonFunc::GetSystemTime();
    m_bResetSchedule = 0;
    m_bResetThirdParty = 0;
//     m_NetLinkFlag = 0;
    m_IfRequest_NTP = 1;

    CommonFunc::GetTimeTicksUpdate( &m_IntervalTime_RequestSystemtime); // 更新间隔请求系统时间
// 	CommonFunc::GetTimeTicksUpdate( &m_Interval_NetLinkTime); // 更新网络连接成功时间间隔器

    memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
    memset((void *)&m_btPackDataUploadBuffer, 0, sizeof(uint8) * MAX_MULTPACK_UPLOAD_SIZE); //170815 zyx


    SetIVBoardTypeFlag = 0;
    SetAuxBoardTypeFlag = 0;
    CommonFunc::GetTimeTicksUpdate( &SetIVBoardTypeTime);
    CommonFunc::GetTimeTicksUpdate( &SetAuxBoardTypeTime);

    uc_SendFail = 0;
    Pack_1500_depth_back = 0;
    uw16_SendCounter = 0;
    AbortTime = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
    uw64_CTI_SetTimeInterval = NORMAL_UPDATA_DIPLAY;
#if  USE_AS_AUXMCU == 0  //mits8
    for(U16 chn = 0; chn < MAXCHANNELNO; chn++)
        ChannelInfo_Manager::ClearDataFlag(chn);  //mits8
#endif
}


////////
// EndTask(void) : 网络命令解析任务处理结束函数
// Input: N/A
// Return: N/A
///////
// void EthernetParse::EndTask(void)
// {
// 	return;
// }


////////
// CmdParse(void) : 网络命令解析主入口再分支命令处理
// Input: N/A
// Return: N/A
///////
void EthernetParse::CmdParse(void)
{
    // 网络命令解析主入口再分支命令处理
    uint8 my_state;
    my_state = tcp_get_state(TcpRxTx::socket_tcp);

    if(my_state != TCP_STATE_CONNECT) // check if somebody has connected to our TCP
    {
        return;
    }
    else if(TcpRxTx::ENET_BufLen == 0)
    {
        return;
    } //  check if remote TCP sent data

    // Sync Bytes
    if(m_SyncByteCnt < CMD_SYNC_LEN)
    {
        if(m_SyncByteCnt + TcpRxTx::ENET_BufLen < CMD_SYNC_LEN)
            return;
        if(!SyncParse())
            return;
    }

    // Cmd Len    // 统一使用小头序，先传低位数据（小地址）再传高位数据（大地址）。
    if(m_CmdRxLen == 0)
    {
        if(TcpRxTx::ENET_BufLen < 12)  //  (CMD_HEADFIELD3_ORDERCODE_LENGTH + CMD_HEADFIELD5_CHECKSUM_LENGTH)*4 =12
            return;
        //TcpRxTx::GetBFromTCP(& m_CmdRxLen);// 统一使用小头序。
        unsigned char uc1, uc2, uc3;
        TcpRxTx::GetBFromTCP(&uc1);
        TcpRxTx::GetBFromTCP(&uc2);
        TcpRxTx::GetBFromTCP(&uc3);
        m_CmdRxLen |= (uc1 << 16);
        m_CmdRxLen |= (uc2 << 8);
        m_CmdRxLen |= uc3;
        if(m_CmdRxLen > MAX_CMD_BUFFER_SIZE)
        {
            m_SyncByteCnt = 0;
            return;
        }
    }

    // Read Cmd
    if(TcpRxTx::ENET_BufLen < m_CmdRxLen)
        return;

    for(uint16 k = 0; k <= m_CmdRxLen; k ++)
        TcpRxTx::GetBFromTCP(& m_CmdRxBuffer[k]);

    m_SyncByteCnt = 0;

    // Check sum
    //if(!IfCheckSumOk(m_CmdRxLen))
    if(!IsCheckSumOK(&m_CmdRxBuffer[0], m_CmdRxLen))
    {
        return;
    }

    //	m_CmdLastStamp = OS_ServeProcess::OS_Time;	 // 更新通信成功记录时间
    //	m_CommLastStamp = OS_ServeProcess::OS_Time;


    //------------------------------------------------
    for(uint16 i = 0; i < m_CmdRxLen; i ++)  // 暂时为连接上位机，要DAQ连接成功打点使用代码段。
        m_CmdTxBuffer_SynchroChannel[i + 1] = m_CmdRxBuffer[i];
    switch(m_CmdRxBuffer[1])
    {
    case 23: // MPC_POWER_ON:	  //0x17:
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[3]))  = 0;  //  pFeedBack->m_btFeedBackStatus = MPPOFS_SUCCESS; //0
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        SendFeedBack(MPCS_ACK, 6); //size = 6
        return;

    case 18: // MPC_UNIT_INFO:	  //0x12
        /*	*(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[3])) = 0XC8;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[4])) = 0XC8;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[5])) = 0X0C;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[6])) = 1; */
        //1610142 zyx
        MP_UNIT_INFO_FEEDBACK m_UnitInfoFeedBack;
        memcpy(&m_UnitInfoFeedBack, 0, sizeof(MP_UNIT_INFO_FEEDBACK));
        VersionProcess(&m_UnitInfoFeedBack.m_unitVerInfo);
        memcpy(&m_CmdTxBuffer_SynchroChannel[3], &m_UnitInfoFeedBack.m_unitVerInfo, sizeof(UNIT_VER_INFO));
//1610142 zyx
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        SendFeedBack(MPCS_ACK, 12);
        return;

    case 44:  //MPC_UNIT_RESET_STATUS:	  // 0x2C
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[3]))  = 0X00;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[4]))  = 0X08;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[5]))  = 0X6F;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[6]))  = 0XFF;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[7]))  = 0XFF;
        *(unsigned char *)(& (m_CmdTxBuffer_SynchroChannel[8]))  = 0XFF;
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        SendFeedBack(MPCS_ACK, 12);
        MPScheduleStopAll();  //Add by DKQ on 08_26_2013 for replacing mutliple STOP commands
        return;

    case 152:  //MPC_PRO7_CMD:  0X98
    case 153:
    case 154:
        break;

    case 210: // MPC_HEX_RESET=210,// 0xD2
        SendFeedBack(MPCS_ACK, 10);
        WDTFunc::ResetMcu();
        break;

    case 211: // MPC_HEX_GO2BOOTMODE=211, // 0xD3
        SendFeedBack(MPCS_ACK, 0);
        LEDFunc::uw32_GotoBootMode = 0xABCDABCD;
        WDTFunc::ResetMcu();
        break;

    default:
        //memset(&m_CmdTxBuffer_SynchroChannel[0], 0, 30);
        //m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        SendFeedBack(MPCS_ACK, 0);
        return;
    }
    //------------------------------------------------

    // Parse cmd
    UWord16 uw16_Length;
    switch(m_pCmd->m_HeaderNew.m_dwCmd)
    {
#if (USE_AS_AUXMCU == 0 )
    case CMD_GET_META_VARIABLES:
        MPGetMetaVariables(&m_pCmd->m_GetMetaVariables,&m_pFeedBack->m_GetMetaVariables_Feedback);
        break;
#endif

#if READ_MEMORY_FUNCTION == 1
    case CMD_GET_DEBUG_DATA:
        MPGetDebugData(&m_pCmd->m_GetDebugData,&m_pFeedBack->m_DebugFeedBack);
        break;
#endif

    //  系统时间类
    case CMD_SET_SYSTEMTIME:
        MPSetSystemTime(&m_pCmd->m_Set_Systemtime);
        MPSetSystemTimeFeedback(&m_pFeedBack->m_Set_Systemtime_Feedback);
        //SendFeedBack(MPCS_ACK,sizeof(MP_SET_SYSTEMTIME_FEEDBACK)); //comment out by DKQ on 08_29_2013
        SendFeedBack(MPCS_ACK, 0); //Add by DKQ on 08_29_2013, ACK only one byte is enough

        //Schedule_Manager_task::WatchDog.Second=OS_ServeProcess::OS_Time.Second;   //为下位没有应答情况处理服务  0828 dirui
        //Schedule_Manager_task::WatchDog.Us100=OS_ServeProcess::OS_Time.Us100;
        //thirdParty::WatchDog.Second=OS_ServeProcess::OS_Time.Second;
        //thirdParty::WatchDog.Us100=OS_ServeProcess::OS_Time.Us100;		//20130904,deleted by hepeiqing
        break;
#if  (USE_AS_AUXMCU == 0)
    case CMD_SET_VOLTAGE_RANGE :
// 		for(unsigned char i=0;i<16;i++)
// 		{
        //           unsigned short range;
        // 			  range= m_pCmd->m_Set_VoltageRang.m_uVotalgeRange[i];
// 			ChannelInfo_Manager::ChannelInfo[i].m_ucVoltageRange= m_pCmd->m_Set_VoltageRang.m_uVotalgeRange[i];
// 			if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_PWM)
//        PWM_Base::SetVoltRange(i,ChannelInfo_Manager::ChannelInfo[i].m_ucVoltageRange);

// 		}//171113,hpq,add for pro8
        SendFeedBack(MPCS_ACK, 0);
        break;
#endif
    //  设置通道配置类
    case CMD_SET_CHANNEL_BOARD:
        //------多包传输时接收包转存到二级缓冲区---------
#if  USE_AS_AUXMCU == 1   //为辅助unit下载通道信息
        MPSetChannelBoard_Aux(&m_pCmd->m_Set_Channel_Board);
#else
        MPSetChannelBoard(&m_pCmd->m_Set_Channel_Board);
#endif

        break;

//#if  (USE_AS_AUXMCU == 1) ||(SUPPORT_AUX == 1)
    case CMD_SET_CHANNEL_MAP:
        MPSetChannelMap(&m_pCmd->m_Set_Channel_Board);
        break;
#if USE_AS_AUXMCU == 1
    case CMD_MTCI_SET_GROUP_SETTING:
        MP_MTCI_GroupSetting(&m_pCmd->m_MTCI_SetGroupSetting);//1910101 zyx
        break;
#endif
//#endif	  //如果是辅助unit 或者如果是IVUNIT　并且支持辅助的话，接受下MAP的命令。
    /////////////////////BMS //////////////////////
    //#if  (USE_AS_AUXMCU == 0 &&SUPPORT_CAN_BMS == 1)
    case CMD_SET_CAN_MAP:
        MPSetCANBMSMap(&m_pCmd->m_SetCANBMS_Map);
        break;
#if  (USE_AS_AUXMCU == 0)
    case CMD_SET_CAN_CONFIG:
        MPSetBMSChannelConfig(&m_pCmd->m_Set_CANSignal_Config);
        break;
    case CMD_SET_CAN_MESSAGE:
        MPSetCANMessage(&m_pCmd->m_Set_CANMessage);
        break;
    case CMD_ASSIGN_CAN_CONFIG:
        MPCANConfigAssign(&m_pCmd->m_CANConfig_Assign);
        break;
    case CMD_SET_BUILTIN_CHANNEL_BOARD:
        MPSetChannelBoard_BuiltIn(&m_pCmd->m_Set_Channel_Board);
        break;
    case CMD_GET_BUILTIN_CHANNEL_BOARD:
        MPGetChannelBoard_BuiltIn(&m_pCmd->m_Get_Channel_Board);
        break;
    case CMD_SET_SMB_CONFIG:
        MPSetSMBChannelConfig(&m_pCmd->m_Set_SMBSignal_Config);
        break;
    case CMD_ASSIGN_SMB_CONFIG:
        MPSMBConfigAssign(&m_pCmd->m_SMBConfig_Assign);
        break;
#endif
#if ( USE_AS_AUXMCU == 1 )
    case CMD_SET_MULTI_CAN_MESSAGE:
        MPSetMultiCANMessage(&m_pCmd->m_Set_CANBMSAuxMultiMsg); //170828 zyx
        break;
#endif
    case CMD_SET_META_VARIABLE:
        MPSetMetaVariable(&m_pCmd->m_SetMetaVariable);
        break;
    case CMD_UPDATE_META_VARIABLE_ADVANCED:
        MPUpdateMetaVariable(&m_pCmd->m_UpdateMetaVariableAdvMode);
        //SendFeedBack(MPCS_ACK,0);
        break;
    case CMD_SET_CHANNEL_PID:
        //------多包传输时接收包转存到二级缓冲区---------
        MPSetChannelPID(&m_pCmd->m_Set_Channel_PID);
        break;

    case CMD_SET_CHANNEL_CALIBRATE:
        MPSetChannelCalibrate(&m_pCmd->m_Set_Channel_Calibrate);
        break;
    case CMD_SET_M0_CHANNEL_CALIBRATE:
        MPSetM0ChannelCalibrate(&m_pCmd->m_Set_Channel_Calibrate);
        break;
    case CMD_M0_READDATAORSTATE:
        MP_M0_ReadData( &m_pCmd->m_Independent_ReadDataOrState);
        break;
    case CMD_Read_M0_CalibPara:
        MP_M0_ReadCalibPara(&m_pCmd->m_Read_M0_CalibPara);
        break;
    case CMD_GET_IV_BOARDTYPE:
        MPGetIVBoard(&m_pCmd->m_Get_IV_BoardType);
        break;

    case CMD_GET_CHANNEL_BOARD:
#if  USE_AS_AUXMCU == 1
        MPGetChannelBoard_Aux(&m_pCmd->m_Get_Channel_Board);
#else
        MPGetChannelBoard(&m_pCmd->m_Get_Channel_Board);
#endif
        break;
    case CMD_SET_M0_ACTIONSWITCH:
        MP_M0_ACTIONSWITCH(&m_pCmd->m_Set_M0_ActionSwitch);
        break;
    //  独立运行类(第三方命令)
    case CMD_INDEPENDENT_ACTIONSWITCH:
        MPIndependentActionSwitch(&m_pCmd->m_Independent_ActionSwitch);
        break;

    case CMD_INDEPENDENT_ACTION:
    case CMD_INDEPENDENT_ACTION_AUTOCALIBRATE:   //独立控制自校准板
        MPIndependentAction(&m_pCmd->m_Independent_Action);
        break;

    case CMD_INDEPENDENT_EMERGENCYSTOP:
        MPIndependentEmergencyStop(&m_pCmd->m_Independent_EmergencyStop);
        break;

    case CMD_INDEPENDENT_READDATAORSTATE:
        MPIndependentReadData(&m_pCmd->m_Independent_ReadDataOrState);
        uw16_Length = MPIndependentReadDataFeedback(&m_pFeedBack->m_Independent_ReadDataOrState_Feedback); // 18+12*1=30;单独1个通道3+30=33个字节。// 实际长度=总结构数-全部未填数组+已经填写数据数组;
        uw16_Length += (sizeof(MP_INDEPENDENT_READDATAORSTATE_FEEDBACK) - (sizeof(ST_READDATAORSTATE_VARIABLE) * MAXCHANNELNO));  //  前有1个字节预留发送(ACK);
        SendFeedBack(MPCS_ACK, uw16_Length);
        break;  // 读取数据已完成
    case CMD_INDEPENDENT_READ_CFG:
        MPIndependentReadCfg(&m_pCmd->m_Independant_Read_CFG);
        uw16_Length = MPIndependentReadCfgFeedback(&m_pFeedBack->m_Independant_Read_CFG); // 18+12*1=30;单独1个通道3+30=33个字节。// 实际长度=总结构数-全部未填数组+已经填写数据数组;
        uw16_Length += (sizeof(MP_INDEPENDENT_READ_CFG_FEEDBACK) - sizeof(St_ChanBriefCfg));  //  前有1个字节预留发送(ACK);
        SendFeedBack(MPCS_ACK, uw16_Length);
        break;
//     case CMD_INDEPENDENT_POWERON:
    //MPIndependentPowerOn(&m_pCmd->m_Independent_EmergencyStop);
//         break;

//     case CMD_INDEPENDENT_RESETCAN:
    //MPIndependentResetCAN(&m_pCmd->m_Independent_EmergencyStop);
//         break;

    case CMD_INDEPENDENT_READ_BOARD_DETAIL:
        //DAQ开启时询问板信息的时候返回板配置信息
        MPIndependent_BoardVersion(&m_pCmd->m_Independent_ReadBoardVersion);
#if USE_AS_AUXMCU == 0
        uw16_Length = MPIndependent_IVBoardVersionFeedback(&m_pFeedBack->m_Independent_ReadBoardVersion_Feedback);
#else
        uw16_Length = MPIndependent_AuxBoardVersionFeedback(&m_pFeedBack->m_Independent_ReadBoardVersion_Feedback);
#endif

        SendFeedBack(MPCS_ACK, uw16_Length);
        break;

        //  Schedule设置配置类
        // 	    case CMD_SCHEDULE_SET_SETTING:
        // 			MPScheduleSetSetting(&m_pCmd->m_Schedule_Set_Setting);
        // 			break;

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    case CMD_SCHEDULE_GET_STATUS:
        MPScheduleGetStatus(&m_pCmd->m_Schedule_Get_Status);
        break;
#endif

    //  Schedule准备数据类
    case CMD_SCHEDULE_DOWNLOAD:
    case CMD_PULSE_DOWNLOAD:
    case CMD_STEP_DOWNLOAD:
        //------多包传输时接收包转存到二级缓冲区---------
        MPScheduleDownload(&m_pCmd->m_Schedule_Download);
        break;

    case CMD_SIMULATION_DOWNLOAD:	//12.22.2013ADD //Download SIMULATION Data
        MPSimulationDownload(&m_pCmd->m_Simulation_Download); //12.22.2013ADD
        break;

    //  Schedule运行类
    case CMD_SCHEDULE_START:
    case CMD_SCHEDULE_JUMP:
    case CMD_SCHEDULE_RESUME:
        MPScheduleJump(&m_pCmd->m_Schedule_Jump);
        break;

    case CMD_SCHEDULE_STOP:
    case CMD_SCHEDULE_PAUSE:
#if  (USE_AS_AUXMCU == 0)
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM )   //add by dirui 20171101 add alarm for pwm
            PWM_Base::PWM_LEDRunStatus = SYSTEM_IDLE;
#endif
        MPScheduleStop(&m_pCmd->m_Schedule_Stop);
        break;

    case CMD_SCHEDULE_CONTINUE:
        MPScheduleContinue(&m_pCmd->m_Schedule_Continue);
        break;
#if (USE_AS_AUXMCU == 0)
    case CMD_SCHEDULE_ACIM_ON:
        // OPEN ACIM RELAY
        MPScheduleContinue(&m_pCmd->m_Schedule_Continue);
        if(Step_Manager_task::ACIM_Status[m_pCmd->m_Schedule_Continue.m_stMPack.m_stMSecondField.m_wChannelNumber] == ACIM_WAIT)
            Step_Manager_task::ACIM_Status[m_pCmd->m_Schedule_Continue.m_stMPack.m_stMSecondField.m_wChannelNumber] = ACIM_WAIT_RELAY_ON;
        break;
    case CMD_SCHEDULE_ACIM_OFF:
        // CLOSE ACIM RELAY
        MPScheduleContinue(&m_pCmd->m_Schedule_Continue);
        if(Step_Manager_task::ACIM_Status[m_pCmd->m_Schedule_Continue.m_stMPack.m_stMSecondField.m_wChannelNumber] == ACIM_TEST)
            Step_Manager_task::ACIM_Status[m_pCmd->m_Schedule_Continue.m_stMPack.m_stMSecondField.m_wChannelNumber] = ACIM_WAIT_RELAY_OFF;
        break;
    case CMD_SET_INTERVALTIME_LOGDATA:
        MPSetIntervalTime_Logdata(&m_pCmd->m_SetIntervalTimeLogData);
        break;
#endif
    case CMD_SCHEDULE_ASSIGN:
        MPScheduleAssign(&m_pCmd->m_Schedule_Assign);
        uw16_Length = MPScheduleAssignFeedback(&m_pFeedBack->m_Assign_Feedback);
        uw16_Length += (sizeof(MP_ASSIGN_FEEDBACK)   // 实际长度=总结构数-全部未填数组+已经填写数据数组;
                        - (sizeof(St_MD5Assign) * IV_MAX_ASSIGN_MD5_NUMBER));  // 去丢过多的数组成员数据
        SendFeedBack(MPCS_ACK, uw16_Length);
        break;

    case CMD_SIMULATION_ASSIGN:                                                               //12.22.2013ADD
        MPSimulationAssign(&m_pCmd->m_Simulation_Assign);                                       //12.22.2013ADD
        uw16_Length = MPSimulationAssignFeedback(&m_pFeedBack->m_Simulation_Assign_Feedback);   //12.22.2013ADD
        SendFeedBack(MPCS_ACK, uw16_Length);                                                    //12.22.2013Add
        break;

    case CMD_SCHEDULE_ONLINEUPDATE:
        MPScheduleOnLineUpdate(&m_pCmd->m_Schedule_Online_Updata);
        break;

    //  Schedule自动回馈类
    //case CMD_SCHEDULE_REPORT_EVENT:
    //case CMD_SCHEDULE_REPORT_LOGDATA:
    //  break;

    //  Schedule获取信息类
    case CMD_SCHEDULE_ACK_EVENT:
        MPScheduleAckEvent(&m_pCmd->m_Schedule_Ack_Event);
        break;

    //  对辅助通道I2C配置管理任务RSN错误信息回应处理
    case CMD_RSN_ERROR_ACK_EVENT:
#if  USE_AS_AUXMCU == 1
        MP_RSN_ErrorAckEvent(&m_pCmd->m_RSN_Error_Ack_Event);
#endif
        break;


    case CMD_BATCH_GROUP:
        MPBatchGrouping(&m_pCmd->m_Batch_Group, true);
// #if (USE_AS_AUXMCU == 0)
//         MPSMBConfigAssign(&m_pCmd->m_SMBConfig_Assign);
// #endif
        SendFeedBack(MPCS_ACK, 0);
        break;

    case CMD_SET_IV_BOARDTYPE:
        MPSetIVBoardType(&m_pCmd->m_Set_IV_BoardType);
        break;
#if  USE_AS_AUXMCU == 1
    case CMD_SENT_ACIMTEST_START:
        MPAuxACIMStartedACKEvent(&m_pCmd->m_Start_ACIMTest);

        break;

    case CMD_SENT_ACIMTEST_STOP:
        MPAuxACIMStopACKEvent(&m_pCmd->m_Stop_ACIMTest);
        break;
#endif
    //CANAUX_BROADCAST START//
    case CMD_SET_CANAUX_BROADCAST_CONFIG:
        MPSetCANAUX_Broadcast_Config(&m_pCmd->m_CANAUX_Broadcast_Config_Set);
        break;
    //CANAUX_BROADCAST END//
    case CMD_SET_DSP_BASIC_PARA:
        MPSetDspBasicParameter(&m_pCmd->m_SetDspBasicInfo);
        break;
    case CMD_SET_DSP_ADJUST_PARA:
        MPSetDspAdjustParameter(&m_pCmd->m_SetDspAdjustInfo);
        break;
    case CMD_READ_DSP_BASIC_PARA:
        MPReadDspBasicParameter(&m_pCmd->m_ReadDspBasicInfo);
        break;
    case CMD_READ_DSP_ADJUST_PARA:
        MPReadDspAdjustParameter(&m_pCmd->m_ReadDspAdjustInfo);
        break;
    case CMD_SET_DSP_INDENTITY_PARA:
        MPSetDspIndentityParameter(&m_pCmd->m_SetDspIndentityInfo);
        break;
    case CMD_READ_DSP_INDENTITY_PARA:
        MPReadDspIndentityParameter(&m_pCmd->m_ReadDspIndentityInfo);
        break;
    default:
        //memset(&m_CmdTxBuffer_SynchroChannel[0], 0, 30);
        //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
        //SendFeedBack(MPCS_PRO7NACK,0);
        break;
    }
}

////////
// MPSetSystemTime(MP_SET_SYSTEMTIME *pCmd) : 网络命令对时处理函数
// Input: pCmd 网络对时命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetSystemTime(MP_SET_SYSTEMTIME *pCmd)  //20130904,modified by hepeiqing,
{
    // 网络命令对时处理函数
#if SOFTWARE_DOG_ENABLE == 1
    if((m_u16Token + 2) == pCmd->m_u16Token)
    {
//         m_bP1Broken = 0;
//         m_bP2Broken = 0;
        m_ucTokenBrokenCount = 0;
    }
    else
    {
        m_ucTokenBrokenCount ++;
//         m_bP1Broken = 0;
        if(m_ucTokenBrokenCount >= COM_CHANNEL1_TIMEOUT / PC_SET_TIME_INTERVAL) //20130905,add by hepeiqing
        {
//             m_bP2Broken = 1;
            m_bResetSchedule = eEMERGENCYSTOP_ERROR_TokenBroken;
            m_bResetThirdParty = 1;
        }
    }
    m_u16Token = pCmd->m_u16Token;

    m_WatchDogTime = CommonFunc::GetSystemTime();//OS_Time;
#endif

#if  USE_AS_AUXMCU == 0   //针对IVMCU
    if(pCmd->f_SetIntervalTimeLog != 0)
    {
        unsigned long long uw64_SetTime = pCmd->f_SetIntervalTimeLog*10000;   // s->100us
        if(uw64_SetTime < FAST_UPDATA_DIPLAY)  // 5ms
            uw64_CTI_SetTimeInterval = FAST_UPDATA_DIPLAY;  // 5ms
        else if(uw64_SetTime > NORMAL_UPDATA_DIPLAY)
            uw64_CTI_SetTimeInterval = NORMAL_UPDATA_DIPLAY;
        else
            uw64_CTI_SetTimeInterval = uw64_SetTime;  // 1000us
#if  USE_AS_AUXMCU == 0   //针对IVMCU
        Schedule_Manager_task::SDLIdeShowMsTime_Updata();
#endif
    }
    else
    {
        if(uw64_CTI_SetTimeInterval != NORMAL_UPDATA_DIPLAY)
        {
#if  USE_AS_AUXMCU == 0   //针对IVMCU
            Schedule_Manager_task::SDLIdeShowMsTime_Updata();
#endif
        }
        uw64_CTI_SetTimeInterval = NORMAL_UPDATA_DIPLAY;  // 3s
    }
#endif

    if(OS_ServeProcess::Eth_TimeCalied)
    {
        OS_ServeProcess::SetSystemTimeSmooth(pCmd->Time);
        return;
    }
    else
    {
        // 1 置位对时标志 2 关闭RTC中断
        OS_ServeProcess::RestartSystemTime(pCmd->Time);
    }
    OS_ServeProcess::Eth_TimeCalied = 1;
    m_WatchDogTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

#if  USE_AS_AUXMCU == 0   //针对IVMCU
    /*以下代码为解决21系列控制板，在硬件初始化时写的R_VM ，由于风扇信号保护导致CPLD 写不成功，需要延时15秒以上再重新写一次。
      下面代码替代了原来的SystemDelay.cpp  任务。
    */
    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
    case BOARDTYPE_LBT21014:
    case BOARDTYPE_LBT21024:
    case BOARDTYPE_LBT21044:
    case BOARDTYPE_LBT21084:
    case BOARDTYPE_LBT21162:
    case BOARDTYPE_LBT20084:
        for(unsigned char ChannelIdx = 0; ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
        {
            unsigned short RelayStatus = 0x0800;     //R_VM 位置1  打开电压继电器     // modify by qjm20171218
            CPLD_FUNC::WriteDOBus(ChannelIdx, RelayStatus);
        }
        break;
    default:
        break;

    }
#endif
    MPReport_Request_NTPCalib(0);
}

void EthernetParse::MPReport_Request_NTPCalib(signed short offset)
{
    MP_REPORT_REQUEST_NTPCALIB *pReport =(MP_REPORT_REQUEST_NTPCALIB *)&m_pReport->m_Request_SystemTime;
    pReport->m_dwCmd = CMD_REPORT_REQUEST_NTPCALIB;  // 将大数据体信息转存到通信协议包中，然后打包发送
    pReport->m_Request_NTP   = 0x01;
    pReport->m_Offset_ms = offset;
    pReport->m_RTC_CalibSet = (-1)*offset;
    EventTx_Length = sizeof(MP_REPORT_REQUEST_NTPCALIB); // 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &EventTx_Length, SEND_REQUEST_SYSTEMTIME);
    EthernetParse::MPReportData(0, SEND_REQUEST_SYSTEMTIME);
}


////////
#if READ_MEMORY_FUNCTION == 1
void EthernetParse::MPGetDebugData(MP_GET_DEBUG_DATA *pCmd,MP_GET_DEBUG_DATA_FEEDBACK *pFeedback)
{
    unsigned int src_addr = 				pCmd->m_src_addr;
    unsigned int src_addr_len = 		pCmd->m_src_addr_len;
    unsigned char src_data_type = 	pCmd->m_src_data_type;
    unsigned int remainder = 0;

    if(src_addr_len > 1400)
        src_addr_len = 1400;

//     if(pCmd->m_src_addr % 4 != 0)
//     {
//         remainder = pCmd->m_src_addr % 4;
//         src_addr += 4 - remainder;
//     }

    memset(pFeedback,0,sizeof(MP_GET_DEBUG_DATA_FEEDBACK));

    pFeedback->m_dwCmd = 						CMD_GET_DEBUG_DATA_FEEDBACK;
    pFeedback->m_src_addr = 				src_addr;
    pFeedback->m_src_addr_len = 		src_addr_len;
    pFeedback->m_src_data_type = 		src_data_type;

    pFeedback->m_error_code = 	DebugReadMemory::GetData(pFeedback->m_btPackData,src_addr,src_addr_len,src_data_type);

    if(pFeedback->m_error_code)
        pFeedback->m_src_addr_len = 0;

    memcmp(m_CmdTxBuffer_SynchroChannel+1,pFeedback,sizeof(MP_GET_DEBUG_DATA_FEEDBACK));
    SendFeedBack(MPCS_ACK, sizeof(MP_GET_DEBUG_DATA_FEEDBACK));

}
#endif

////////
// GetSinglePack(MP_SINGLE_PACKDATA *pCmd) 对多包函数进行单包组包
// Input: pCmd 单包结构指针;
// Return: 0-正常单包 1-全部单包组包完成 -1-异常：重复包
///////
char EthernetParse::GetSinglePack(MP_SINGLE_PACKDATA *pCmd,bool IsNeedProtect)
{
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制

    if(IsNeedProtect == true)
    {
        TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                                & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);
    }

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

    if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
    {
        st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
        st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
    }
    else
    {
        if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
        {
            // 判断是否为同一个多包的内容。
            m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
            MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
            return (char)(-1);
        }
        st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
    }

    st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
    st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

    // 存包中的SCH数据
    memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据

    if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
    {
        // 判断是否为同一个多包的内容接收完成。
        m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
        return 1;
    }

    return 0;

}


// MPSetSystemTimeFeedback(MP_SET_SYSTEMTIME_FEEDBACK *pFeedback)
//      : 网络命令对时处理回应函数
// Input: pFeedback 网络对时命令回应结构指针;
// Return: N/A
///////
void EthernetParse::MPSetSystemTimeFeedback(MP_SET_SYSTEMTIME_FEEDBACK *pFeedback)
{
    // 网络命令对时处理回应函数
    //for(UChar8 uc_i = 0; uc_i < 100;uc_i ++)
    //    m_CmdTxBufSynchro[uc_i] = CODE_EMPTY; // 清空回传数据区

    // pFeedback->m_btNone_111 =  1;  // pFeedback->m_btUnitId = Interface_LED::uc_dsp_id;
    // pFeedback->m_btNone_112 =  0X9A ; // pFeedback->m_btCmd = MPC_MCU2PC_CMD;

    pFeedback->m_dwCmd = CMD_SET_SYSTEMTIME_FEEDBACK;

// 	pFeedback->m_dwCopyCmd = m_ResultFeedback.m_dwCopyCmd;  // 传递备份信息
// 	pFeedback->m_dwCopyCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;

    //pFeedback->m_st_SYSTEMTIME_DATA.m_btTime = OS_ServeProcess::OS_Time.InSeconds; //  回传系统时间

    CmdGetToFeedback(&pFeedback->m_dwCopyCmd, &pFeedback->m_dwCopyCmd_Extend,
                     &m_ResultFeedback.m_stMSecondField.m_wChannelNumber, &m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber);
}



#if  USE_AS_AUXMCU == 1
////////
// MPSetChannelBoard_AUX(MP_SET_CHANNEL_BOARD *pCmd) : 网络命令设置板的通道信息处理函数
// Input: pCmd 网络设置板的通道信息命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetChannelBoard_Aux(MP_SINGLE_PACKDATA *pCmd)
{
    unsigned char uc_Chan_Idx[16];
    unsigned char uc_DownLoad_Success = 0;
    unsigned char uc_BID_NumbChange;
    bool WriteAuxBoardSuccess = false;
    UWord16       u16Total;
    //     UWord16       uw16_PhysicalCh;
    MP_AuxUnitConfigDownload * st_Config;

    AUX_Unit_Base::m_EnableISRsample = false; //Add by DKQ 10_05_2015

    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
        st_Config = (MP_AuxUnitConfigDownload *) &m_btPackDataBuffer[0];

        for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
            uc_Chan_Idx[uc_idx] = st_Config->m_ucChannelIdx[uc_idx];  // 缓冲数组，列表用
        if(st_Config->m_u16ChannelCount == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
        {
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
                uc_Chan_Idx[uc_idx] = 0XFF; // 缓冲数组，列表用
        }
        if(st_Config->m_ucCommonSource == 0xBB)//Mist7 send//1807131
        {
            if( st_Config->m_Global.m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL)
            {
                //先检查一下下载的通道辅助类型与原辅助类型是否一致，不一致不保存，并报错。（还未考虑AO的情况）
                for(unsigned char i = 0; i < st_Config->m_u16ChannelCount; i++)
                {
                    unsigned char uc_data_group_idx = 0;
                    unsigned char uc_chn = 0;
                    unsigned char uc_Bid = 0;

                    uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[st_Config->m_Channel[i].m_AuxType].m_StartInSnake + st_Config->m_Channel[i].m_ChannelNumber].m_data_group_idx;
                    uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[st_Config->m_Channel[i].m_AuxType].m_StartInSnake + st_Config->m_Channel[i].m_ChannelNumber].m_chn;

                    uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];  //需要BID 信息。 得建立一个data_group_idx 与 ＢＩＤ拨码的关系
                    if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
                        uc_Bid = 0;
                    ST_ABI_CHN m_abi_chn;
                    if(st_Config->m_Channel[i].m_AuxType == AUX_AO)
                        m_abi_chn = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn];
                    else
                        m_abi_chn = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)];

                    if((st_Config->m_Channel[i].m_AuxType == m_abi_chn.m_AuxType) && (st_Config->m_Channel[i].m_AuxSubType == m_abi_chn.m_AuxSubType))
                    {
                        continue;
                    }
                    else
                    {
                        //出错 上报
                        m_ResultFeedback.m_btResult = i; //180720 zyx
                        m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_MAP_SENSOR_TYPE;
                        MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                        return;
                    }

                }
                //上面检查都没问题了开始保存
                uc_DownLoad_Success = 0;
                AuxConfigRSN::Init();  //  20150611  yang    解决PC提示过慢问题
                for(unsigned char i = 0; i < st_Config->m_u16ChannelCount; i++)
                {
                    unsigned char uc_data_group_idx = 0;
                    unsigned char uc_chn = 0;
                    unsigned char uc_Bid = 0;
                    unsigned char uc_ChID = 0;

                    uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[st_Config->m_Channel[i].m_AuxType].m_StartInSnake + st_Config->m_Channel[i].m_ChannelNumber].m_data_group_idx;
                    uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[st_Config->m_Channel[i].m_AuxType].m_StartInSnake + st_Config->m_Channel[i].m_ChannelNumber].m_chn;

                    uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];  //需要BID 信息。 得建立一个data_group_idx 与 ＢＩＤ拨码的关系
                    if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
                        uc_Bid = 0;
                    if(st_Config->m_Channel[i].m_AuxType == AUX_AO)
                    {
                        uc_ChID = MAX_CH_IN_AUX_UNIT + 8 + uc_chn;
                        st_Config->m_Channel[i].m_ChannelNumber = uc_chn;//更新一下通道号
                    }
                    else
                    {
                        uc_ChID = ((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn;
                        st_Config->m_Channel[i].m_ChannelNumber = uc_ChID;//更新一下通道号
                    }

                    uc_DownLoad_Success += AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_Bid), uc_ChID, &st_Config->m_Channel[i]);   // 通道信息存入I2C的24LC128

                }
                if( uc_DownLoad_Success < st_Config->m_u16ChannelCount)
                {
                    // 判断是否写板通道信息失败。
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD_CHANNEL;  // 配置错误代码
                }
                else
                {
                    WriteAuxBoardSuccess = true;
//                     PrepareForInitAUX();
// 						SelectiveMountTask();		//无需再重整函数
                }

            }
        }
        else
        {
            if( st_Config->m_Global.m_btCtrl_Type == CONTROLTYPE_USE_BOARD)
            {   // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息 （预留开发 3：单独下载Map表 4：单独下载校正值）
                // 单个板信息保存
                unsigned char uc_BoardTotal = 1;
                UChar8   uc_BoardIdx =  st_Config->m_Global.m_abi_globe.m_BID;  // 按板号顺序排序。
                if(uc_BoardIdx >= 0XFF)
                {
                    uc_BoardIdx =  0;
                    uc_BoardTotal = MAX_BOARD_PER_AUXMCU;
                }
                else
                {
                    if(uc_BoardIdx >= MAX_BOARD_PER_AUXMCU)
                        uc_BoardIdx = 0; // 单板保护
                }
                //static ST_AUX_BOARD_INFO   m_AuxBoards;
                memset(&AuxCConfig::m_AuxBoards,0,sizeof(ST_AUX_BOARD_INFO));
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe = st_Config->m_Global.m_abi_globe;
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID = uc_BoardIdx;
// 			m_AuxBoards.m_BoardInfo.m_abi_globe.m_BoardType= st_Config->m_Global.m_abi_globe.m_BoardType;
// 			m_AuxBoards.m_BoardInfo.m_abi_globe.m_AuxFuncType =st_Config->m_Global.m_abi_globe.m_AuxFuncType;
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_valid = st_Config->m_Global.m_valid;
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_assemble.m_ConsistentOrder =  st_Config->m_Global.m_assemble.m_ConsistentOrder;      // 面板通道号是否与电气特性排序的通道序号一致
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_assemble.m_SlaveGoesFirst =  st_Config->m_Global.m_assemble.m_SlaveGoesFirst;      // 有无副板时，面板通道号是主板在前还是副板在前
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_assemble.m_WithSlave =  st_Config->m_Global.m_assemble.m_WithSlave;      // 有无副板
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_RSN =  st_Config->m_Global.m_u16Board_RSN;            // 注册序号RSN
                AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_Total_Channels = (unsigned char) st_Config->m_u16ChannelCount; //待确认
                //             m_AuxBoards.m_Start_InUnit = 0;
                // 						m_AuxBoards.m_End_InUnit =m_AuxBoards.m_BoardInfo.m_abi_globe.m_Total_Channels - 1;

                uc_DownLoad_Success = 0;
                AuxConfigRSN::Init();     //  20150611  yang    解决PC提示过慢问题
                for(unsigned char uc_Count = 0; uc_Count < uc_BoardTotal; uc_Count ++)
                {
                    AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID = uc_BoardIdx + uc_Count;
                    //E2PROMFunction::WriteAUXBoardCfg(uc_jdx + uc_Count,& m_AuxBoards);  // 板信息存入I2C的24LC128
                    //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                    //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
                    uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count;
                    uc_DownLoad_Success += AuxCConfig::WriteAUXBoardCfg(uc_BID_NumbChange, & AuxCConfig::m_AuxBoards); // 板信息存入I2C的24LC128
                    AuxConfigRSN::st_BoardRSN[AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
                    AuxCConfig::WriteAUXBoardRSN(uc_BID_NumbChange, &AuxConfigRSN::st_BoardRSN[AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_I2C);
                    //AuxConfigRSN::Check_RSN_Init( m_AuxBoards.m_BoardInfo.m_BType.m_BID );
                    AuxConfigRSN::st_BoardRSN[AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_FRAM = 0;
                    AuxCConfig::WriteRSN(AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID, &AuxConfigRSN::st_BoardRSN[AuxCConfig::m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN
                }
                if( uc_DownLoad_Success < uc_BoardTotal)
                {
                    // 判断是否写板失败。
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD;  // 配置错误代码
                }
                else
                {
                    WriteAuxBoardSuccess = true;
//                     PrepareForInitAUX();
//                     SelectiveMountTask();
                }
                //Hardware_Init();    //准备初始化辅助通道操作
            }
            else if( st_Config->m_Global.m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL)
            {
                // 单个通道信息保存
                unsigned char uc_ChTotal = 1;
                unsigned char uc_BoardTotal = 1;
                unsigned char uc_ChID = 0;
                unsigned char uc_UARTChID = 0;
                unsigned char uc_DACChID = 0;
                unsigned char uc_I2CChID=0; //add by zc 04.20.2018
                unsigned char uc_Count = 0;
                UChar8   uc_BoardIdx =  st_Config->m_Global.m_abi_globe.m_BID;  // 按板号顺序排序。
                unsigned char uc_AuxFuncType = st_Config->m_Global.m_abi_globe.m_AuxFuncType;

                if(uc_BoardIdx >= 0XFF)
                {
                    uc_BoardIdx = 0;
                    st_Config->m_Channel[0].m_ChannelNumber = 0XFF;
                    uc_BoardTotal  =  MAX_BOARD_PER_AUXMCU;
// 				for(unsigned char uc_Count = 0; uc_Count < uc_BoardTotal; uc_Count ++)
// 				{
// 					uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count;
// 					AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
// 					AuxCConfig::WriteAUXBoardRSN(uc_BID_NumbChange, &AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_I2C);
// 					//AuxConfigRSN::Check_RSN_Init( uc_Count );
// 					AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_FRAM = 0;
// 					AuxCConfig::WriteRSN(uc_Count, &AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN
// 				}
                }
                else
                {
                    if(uc_BoardIdx >= MAX_BOARD_PER_AUXMCU)
                        uc_BoardIdx = 0; // 单板保护
// 				uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx;
// 				AuxConfigRSN::st_BoardRSN[uc_BoardIdx].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
// 				AuxCConfig::WriteAUXBoardRSN(uc_BID_NumbChange ,&AuxConfigRSN::st_BoardRSN[uc_BoardIdx].uw16_BoardRSN_I2C);
// 				//AuxConfigRSN::Check_RSN_Init( uc_BoardIdx );
// 				AuxConfigRSN::st_BoardRSN[uc_BoardIdx].uw16_BoardRSN_FRAM = 0;
// 				AuxCConfig::WriteRSN(uc_BoardIdx, &AuxConfigRSN::st_BoardRSN[uc_BoardIdx].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN

                    uc_Count = uc_BoardIdx;
                    uc_BoardTotal = uc_Count + 1;
                }

                for(; uc_Count < uc_BoardTotal; uc_Count ++)
                {
                    if(st_Config->m_Channel[0].m_ChannelNumber  ==  0XFF)
                        uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count;
                    else
                        uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx;
                    AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
                    AuxCConfig::WriteAUXBoardRSN(uc_BID_NumbChange, &AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_I2C);
                    //AuxConfigRSN::Check_RSN_Init( uc_Count );
                    AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_FRAM = 0;
                    AuxCConfig::WriteRSN(uc_Count, &AuxConfigRSN::st_BoardRSN[uc_Count].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN
                }

                uc_DownLoad_Success = 0;
                AuxConfigRSN::Init();  //  20150611  yang    解决PC提示过慢问题
                for(unsigned char uc_Count = 0; uc_Count < uc_BoardTotal; uc_Count ++)
                {
                    uc_ChTotal = st_Config-> m_u16ChannelCount;
                    if(uc_ChTotal > 1)
                    {
                        uc_ChID = 0;
                    }
                    else
                    {
                        uc_ChID = st_Config->m_Channel[0].m_ChannelNumber;
                    }

                    for(unsigned char uc_k = 0; uc_k < uc_ChTotal; uc_k ++)
                    {
                        //                     uw16_PhysicalCh = Aux_DataManager::Convert_PhysicalChID(uc_BoardIdx, st_Config->m_Channel[0].m_ChannelNumber + uc_k); // 同一块板中面板对应通道号处理
                        //                     uc_ChID = uw16_PhysicalCh;
                        //                      if(uc_MultiCopyFlag ==0x01)
                        //                      {
                        //                           uc_ChID = uc_CopyStartChan+uc_k;
                        //                      }
                        //                      else
                        //                      {
                        //                              uc_ChID = st_Config->m_Channel[0].m_ChannelNumber;
                        //                      }
                        //E2PROMFunction::WriteAUXChannelCfg( uc_jdx, uc_ChID, &st_Config->m_Channel[0]);   // 通道信息存入I2C的24LC128
                        //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                        //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
                        if((uc_AuxFuncType & 0x01) == 0x01)
                        {
                            uc_DownLoad_Success += AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count), uc_ChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128
                        }
                        else if((uc_AuxFuncType & 0x10) == 0x10)
                        {
                            uc_UARTChID = uc_ChID + MAX_CH_IN_AUX_UNIT;
                            uc_DownLoad_Success += AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count), uc_UARTChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128
                        }
                        else if((uc_AuxFuncType&0x20)==0x20)  //I2C add by zc 04.20.2018
                        {
                            uc_I2CChID =uc_ChID+MAX_CH_IN_AUX_UNIT;
                            uc_DownLoad_Success += AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count), uc_I2CChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128
                        }
                        else if((uc_AuxFuncType & 0x40) == 0x40)
                        {
                            uc_DACChID = uc_ChID + MAX_CH_IN_AUX_UNIT + 8;
                            uc_DownLoad_Success += AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx - uc_Count), uc_DACChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128
                        }
                        uc_ChID++;
                    }
                }

                uc_ChTotal = uc_BoardTotal * uc_ChTotal;
                if( uc_DownLoad_Success < uc_ChTotal)
                {
                    // 判断是否写板通道信息失败。
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD_CHANNEL;  // 配置错误代码
                }
                else
                {
                    WriteAuxBoardSuccess = true;
//                     PrepareForInitAUX();
//                     SelectiveMountTask();
                }
                //Hardware_Init();    //准备初始化辅助通道操作
            }
            else
            {

                //Aux_DataManager::m_AuxUnitCFG = st_Config->m_Global;  // 更新板类信息
                //AuxCConfig::SaveAuxUnitCFG( &st_Config->m_Global);
                u16Total = st_Config->m_u16ChannelCount;
                if(u16Total > MAX_AUXCH_PER_AUXMCU)
                {
// 				m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
                    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAX_AUXCH_PER_AUXMCU; // 通道号最大值保护
                    u16Total = MAX_AUXCH_PER_AUXMCU;
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD;  // 配置错误代码
                    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                    return;
                }
                else
                {
                    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
                    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

//                    Aux_DataManager::SaveChannelConfigPara(st_Config->m_u16ChannelCount, &st_Config->m_ucChannelIdx[0]); // 通道信息保存入I2C
                    //             Aux_DataManager::Aux_GetMapfromPC(0);    // 转换PCMap表以便管理
                    //             Aux_DataManager::Aux_BuildIv2AuxMap();  // 转换IVMap表以便管理
                    //Hardware_Init();    //准备初始化辅助通道操作

                    AuxCConfig::Load_I2C_Config();
                    return;
                }
            }
            memset(&st_MultPackManage, 0, sizeof(ST_MultPack));  // 管理信息
        }
    }

    if( m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN )
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    if(uc_DownLoad_Success >= 0)
        MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    if(WriteAuxBoardSuccess)// 2022.04.02 yzl
    {
        WriteAuxBoardSuccess = false;
        SetAuxBoardTypeFlag = 1;
        CommonFunc::GetTimeTicksUpdate( &SetAuxBoardTypeTime);
    }

    if(Aux_DataManager::Aux_ADC_TotalCnt != 0)
        AUX_Unit_Base::m_EnableISRsample = true;    //qjm
    if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType ==Aux_PowerADJ_2US6K_422108)
        AUX_Unit_Base::m_EnableISRsample = false;
}
#else
////////
// MPSetChannelBoard(MP_SET_CHANNEL_BOARD *pCmd) : 网络命令设置板的通道信息处理函数
// Input: pCmd 网络设置板的通道信息命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetChannelBoard(MP_SINGLE_PACKDATA *pCmd)
{
    UChar8 uc_jdx = 0;
    unsigned char uc_Chan_Idx[16];
    char uc_DownLoad_Success = 0;
    MP_UnitConfigDownload * st_Config;

    uc_DownLoad_Success = GetSinglePack(pCmd,true);
    if(uc_DownLoad_Success == 1)
    {
        st_Config = (MP_UnitConfigDownload *) &m_btPackDataBuffer[0];

        for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
            uc_Chan_Idx[uc_idx] = st_Config->m_ucChannelIdx[uc_idx];  // 缓冲数组，列表用

        if(st_Config->m_u16ChannelCount == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
        {
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
                uc_Chan_Idx[uc_idx] = 0XFF; // 缓冲数组，列表用
        }

        ChannelInfo_Manager::BoardCFG.m_Global = st_Config->m_Global;  // 更新板类信息
        IVSDL::Valid = ChannelInfo_Manager::BoardCFG.m_Global.m_bUseSDL;
        LBT21XXX_SampleManage::ResetSampleChanNumber();

        //Built_in CANBMS  Start//
        CANFunc::InitMyCAN();
        CANFunc::CAN_ChangeACCF();//根据新的config情况重新对CAN端口滤波
        //Built_in CANBMS  End//

        if(!ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
            PowerAdj::m_PowerAdjStatus = POWERADJ_IDLE;

        //10.02 add. Submaster use SDL for data reporting to master:
        //if (ChannelInfo_Manager::IsInterUnitPara_SubmasterUnit())
        //if((ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo \
        //	.m_InUnitGroupSize==ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        //    &&	(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo \
        //	 .m_uGroupUnitCount>0))	//如果该unit下所有通道全是slave,就任为是完全意义上的从unit
        //{
        //	IVSDL::Valid = false;
        //}
        //else
        //	IVSDL::Valid=true;


        //ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType = _BOARD_TYPE_;  //20140729 linhuomei exposed this bug,lead to a function malInit of IR,by hepeiqing
//		ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType = board_config::IVBoardType;
        //if(st_Config->m_u16ChannelCount > MAXCHANNELNO)
        if(st_Config->m_u16ChannelCount > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        {
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
            //m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAXCHANNELNO; // 通道号最大值保护
            m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
        }

        uc_jdx = 0;
        //for(UChar8 uc_idx = 0;uc_idx < MAXCHANNELNO;uc_idx++)
        for(UChar8 uc_idx = 0; uc_idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_idx++)
        {
            ChannelInfo_Manager::Init_HistoryBufQueue(uc_idx);
            if(MYBITTRUE(uc_Chan_Idx, uc_idx))
            {
                if (st_Config->m_Channel[uc_jdx].m_ucIRangeCount == 0) // 量程保护
                {
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_IRANG_EMPTY;  // 配置错误代码
                    st_Config->m_Channel[uc_jdx].m_ucIRangeCount  = 1;
                }
                if (st_Config->m_Channel[uc_jdx].m_ucIRangeCount > 8 )
                {
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXIRANG;  // 配置错误代码
                    st_Config->m_Channel[uc_jdx].m_ucIRangeCount  = 8;
                }
                if (st_Config->m_Channel[uc_jdx].m_ucVRangeCount == 0)
                {
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_VRANG_EMPTY;  // 配置错误代码
                    st_Config->m_Channel[uc_jdx].m_ucVRangeCount = 1;
                }
                if (st_Config->m_Channel[uc_jdx].m_ucVRangeCount > 4 )
                {
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXVRANG;  // 配置错误代码
                    st_Config->m_Channel[uc_jdx].m_ucVRangeCount = 4;
                }
                //		if (st_Config->m_Channel[uc_jdx].m_ucVClampCount == 0)
                //		    st_Config->m_Channel[uc_jdx].m_ucVClampCount = 1;
                //		if (st_Config->m_Channel[uc_jdx].m_ucVClampCount > 4 )
                //		    st_Config->m_Channel[uc_jdx].m_ucVClampCount = 4;

                //ChannelInfo_Manager::BoardCFG.m_Channel[uc_idx] = st_Config->m_Channel[uc_jdx];  // 更新通道信息
                ChannelInfo_Manager::SetChanConfig(uc_idx, &st_Config->m_Channel[uc_jdx]);  //170320,add by hepeiqing //zyx 180808

                ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SYS_SAFTY_TYPE_CURRENT, ChannelInfo_Manager::CurrRngPhysicalRange1, & safety_check::Check_S[uc_idx].checkSYS_I); //170320,add by hepeiqing
                //ChannelInfo_Manager::GetSystemSafty(uc_idx,MP_SCH_SAFTY_TYPE_CURRENT,CURRENT_RANGE_1,& safety_check::checkSYS_I[uc_idx]);
                ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SYS_SAFTY_TYPE_VOLTAGE, VOLTAGE_RANGE_HIGH, & safety_check::Check_S[uc_idx].checkSYS_V);
                ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SYS_SAFTY_TYPE_POWER, 0, & safety_check::Check_S[uc_idx].checkSYS_P);

                if(++uc_jdx >= st_Config->m_u16ChannelCount) // 保护功能，用完数据退出。
                    break;
            }
        }
        if(BuiltIn_DataManager::m_Sample_Flag)
        {
            BuiltIn_DataManager::m_WriteFRAM_Flag = true;
            while(BuiltIn_DataManager::m_Sample_Flag);
// 			  BuiltIn_DataManager::SettingCS_High();
// 			  TIMERSFunc::DELAY_SYS( 1, 1 );	//delay 100ms
        }
        ChannelInfo_Manager::SaveConfigPara(st_Config->m_u16ChannelCount, &st_Config->m_ucChannelIdx[0]);
    }


//     if(uc_DownLoad_Success >= 0)
//     {
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//     }

}
void EthernetParse::MPSetChannelBoard_BuiltIn(MP_SINGLE_PACKDATA *pCmd)
{
//     unsigned char uc_Chan_Idx[16];
    unsigned char uc_DownLoad_Success = 0;
// 	  unsigned char uc_BID_NumbChange;
    UWord16       u16Total;

    MP_AuxUnitConfigDownload * st_Config;

//     m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
//     TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
//                             & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// // 	  m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// // 	  m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// // 	  m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// // 	  m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// // 	  m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

//     CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

//     if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
//     {
//         st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
//         st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
//     }
//     else
//     {
//         if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
//         {
//             // 判断是否为同一个多包的内容。
//             m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
//             MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//             return;
//         }
//         st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
//     }

//     st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
//     st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

//     if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
//     {
//         // 判断是否为同一个多包的内容接收完成。
//         uc_DownLoad_Success = 1;
//         m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
//     }
//     // 存包中的SCH数据
//     memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
        st_Config = (MP_AuxUnitConfigDownload *) &m_btPackDataBuffer[0];

//         for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
//             uc_Chan_Idx[uc_idx] = st_Config->m_ucChannelIdx[uc_idx];  // 缓冲数组，列表用
        if(st_Config->m_u16ChannelCount == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
        {
//             for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
//                 uc_Chan_Idx[uc_idx] = 0XFF; // 缓冲数组，列表用
        }
        if( st_Config->m_Global.m_btCtrl_Type == CONTROLTYPE_USE_BOARD)
        {   // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息 （预留开发 3：单独下载Map表 4：单独下载校正值）
            // 单个板信息保存
            unsigned char uc_BoardTotal = 1;
            UChar8   uc_BoardIdx =  st_Config->m_Global.m_abi_globe.m_BID;  // 按板号顺序排序。
// 			          if(uc_BoardIdx >= 0XFF)
// 			          {
// 				              uc_BoardIdx =  0;
// 				              uc_BoardTotal = MAX_BOARD_PER_AUXMCU;
// 			          }
// 			          else
// 			          {
// 				              if(uc_BoardIdx >= MAX_BOARD_PER_AUXMCU)
// 					                  uc_BoardIdx = 0; // 单板保护
// 			          }
            //static ST_BUILTIN_BOARD_INFO_IN_EEPROM   m_AuxBoards;
            memset(&BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo,0,sizeof(ST_BUILTIN_BOARD_INFO_IN_EEPROM));
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_abi_globe = st_Config->m_Global.m_abi_globe;
// 			          m_AuxBoards.m_abi_globe.m_BID = uc_BoardIdx;
// 			          m_AuxBoards.m_abi_globe.m_BoardType= st_Config->m_Global.m_abi_globe.m_BoardType;
// 			          m_AuxBoards.m_abi_globe.m_AuxFuncType =st_Config->m_Global.m_abi_globe.m_AuxFuncType;
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_valid = st_Config->m_Global.m_valid;
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_assemble.m_ConsistentOrder =  st_Config->m_Global.m_assemble.m_ConsistentOrder;      // 面板通道号是否与电气特性排序的通道序号一致
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_assemble.m_SlaveGoesFirst =  st_Config->m_Global.m_assemble.m_SlaveGoesFirst;      // 有无副板时，面板通道号是主板在前还是副板在前
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_assemble.m_WithSlave =  st_Config->m_Global.m_assemble.m_WithSlave;      // 有无副板
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_RSN =  st_Config->m_Global.m_u16Board_RSN;            // 注册序号RSN
// 			          m_AuxBoards.m_abi_globe.m_Total_Channels = (unsigned char) st_Config->m_u16ChannelCount; //待确认


            uc_DownLoad_Success = 0;
// 			          AuxConfigRSN::Init();     //  20150611  yang    解决PC提示过慢问题
// 			          for(unsigned char uc_Count = 0; uc_Count < uc_BoardTotal; uc_Count ++)
// 			          {
            BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo.m_abi_globe.m_BID = uc_BoardIdx ;
            //E2PROMFunction::WriteAUXBoardCfg(uc_jdx + uc_Count,& m_AuxBoards);  // 板信息存入I2C的24LC128
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
// 				               uc_BID_NumbChange = MAX_BOARD_PER_AUXMCU - 1 - uc_BoardIdx;
            uc_DownLoad_Success += CConfig::WriteBuiltInBoardCfg(uc_BoardIdx, & BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo); // 板信息存入I2C的24LC128
// 				               AuxConfigRSN::st_BoardRSN[m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
// 				               AuxCConfig::WriteAUXBoardRSN(uc_BID_NumbChange, &AuxConfigRSN::st_BoardRSN[m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_I2C);
// 				               AuxConfigRSN::st_BoardRSN[m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_FRAM = 0;
// 				               AuxCConfig::WriteRSN(m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID, &AuxConfigRSN::st_BoardRSN[m_AuxBoards.m_BoardInfo.m_abi_globe.m_BID].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN
// 			           }
            if( uc_DownLoad_Success < uc_BoardTotal)
            {
                // 判断是否写板失败。
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD;  // 配置错误代码
            }
            else
            {
// 				            PrepareForInitAUX();
//                     SelectiveMountTask();
                //暂时屏蔽 齐静明20171012     重新初始化硬件信息
            }
            //Hardware_Init();    //准备初始化辅助通道操作
        }
        else if( st_Config->m_Global.m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL)
        {
            // 单个通道信息保存
            unsigned char uc_ChTotal = 1;
            unsigned char uc_BoardTotal = 1;
            unsigned char uc_ChID = 0;
            uc_DownLoad_Success = 0;
            if(st_Config->m_ucCommonSource == 0xBB)//Mist7 send//1807131
            {
                unsigned char uc_BoardIdx = 0;//暂时就支持一个拨码，固定为0.
                uc_ChTotal = st_Config-> m_u16ChannelCount;
                for(unsigned char uc_k = 0; uc_k < uc_ChTotal; uc_k ++)
                {
                    uc_ChID = st_Config->m_Channel[uc_k].m_ChannelNumber;
                    uc_DownLoad_Success += CConfig::WriteBuiltInChannelCfg( uc_BoardIdx, uc_ChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128
                }
            }
            else
            {
                UChar8   uc_BoardIdx =  st_Config->m_Global.m_abi_globe.m_BID;  // 按板号顺序排序。
                uc_ChTotal = st_Config-> m_u16ChannelCount;
                if(uc_ChTotal > 1)
                {
                    uc_ChID = 0;
                }
                else
                {
                    uc_ChID = st_Config->m_Channel[0].m_ChannelNumber;
                }

                for(unsigned char uc_k = 0; uc_k < uc_ChTotal; uc_k ++)
                {
                    //E2PROMFunction::WriteAUXChannelCfg( uc_jdx, uc_ChID, &st_Config->m_Channel[0]);   // 通道信息存入I2C的24LC128
                    //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                    //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。

                    uc_DownLoad_Success += CConfig::WriteBuiltInChannelCfg( uc_BoardIdx, uc_ChID, &st_Config->m_Channel[uc_k]);   // 通道信息存入I2C的24LC128


                    uc_ChID++;
                }
// 			}

            }
            uc_ChTotal = uc_BoardTotal * uc_ChTotal;
            if( uc_DownLoad_Success < uc_ChTotal)
            {
                // 判断是否写板通道信息失败。
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD_CHANNEL;  // 配置错误代码
            }
            else
            {
                if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
                {
                    BuiltIn_DataManager::BuiltIn_HardwareInit();
                }
            }

        }
        else
        {

            //Aux_DataManager::m_AuxUnitCFG = st_Config->m_Global;  // 更新板类信息
            //AuxCConfig::SaveAuxUnitCFG( &st_Config->m_Global);
            u16Total = st_Config->m_u16ChannelCount;
            if(u16Total > MAX_AUXCH_PER_AUXMCU)
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
                m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAX_AUXCH_PER_AUXMCU; // 通道号最大值保护
                u16Total = MAX_AUXCH_PER_AUXMCU;
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_WRITE_BOARD;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }
            else
            {
                m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

// 				Aux_DataManager::SaveChannelConfigPara(st_Config->m_u16ChannelCount,&st_Config->m_ucChannelIdx[0]);  // 通道信息保存入I2C
                //             Aux_DataManager::Aux_GetMapfromPC(0);    // 转换PCMap表以便管理
                //             Aux_DataManager::Aux_BuildIv2AuxMap();  // 转换IVMap表以便管理
                //Hardware_Init();    //准备初始化辅助通道操作

                CConfig::Load_I2C_Config();
                return;
            }
        }

    }


    if( m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN )
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
#endif
//#if(SUPPORT_CAN_BMS==1)
void EthernetParse::MPSetCANBMSMap(MP_SINGLE_PACKDATA  *pCmd)
{
    unsigned char uc_DownLoad_Success = 0;

//	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    //TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
    //	                    & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

//    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_FAIL);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{
// 		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
// 		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
// 	}
// 	else
// 	{
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
// 		{
// 			// 判断是否为同一个多包的内容。
// 			m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 			m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 			MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 			return;
// 		}
// 		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
// 	}

// 	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
// 	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

// 	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
// 	{
// 		// 判断是否为同一个多包的内容接收完成。
// 		uc_DownLoad_Success = 1;
// 		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	}

// 	if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
// 	{
// 		m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	}
// 	MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

// 	// 存包中的数据
// 	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
        //#if (USE_AS_AUXMCU == 0  && SUPPORT_CAN_BMS==1)
#if (USE_AS_AUXMCU == 0 )
        ST_CANBMS_MAP* st_CANBMS_Map = (ST_CANBMS_MAP*)&m_btPackDataBuffer[0];

        for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
        {   //170122,per YongUn/LG's request
            if(  ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive && (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_AuxUnitID > 0)
                    && (  (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive != st_CANBMS_Map->m_CANmap[ChnIdx].m_bIVActive)
                          || (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_AuxUnitID != st_CANBMS_Map->m_CANmap[ChnIdx].m_AuxUnitID) )
              )//old is valid,and new map is changed,need to stop old BMS outbounding msgs
            {
                CANBusParse::CANBMS_StopAllSend(ChnIdx);		              //170122,stop all sending msg when .can changed,per YongUn/LG's request
                CANBusParse::m_BMS_SendBuf[ChnIdx].m_validMsgCnt = 0;
                CANBusParse::m_BMS_SendBuf[ChnIdx].m_glbStopAll = 1;
            }
        }
        unsigned char bIVActive_back[MAXCHANNELNO]; // 备份未更新前的
        for(unsigned char ChnIdx = 0; ChnIdx < MAXCHANNELNO; ChnIdx++)
        {
            bIVActive_back[ChnIdx] = 0;
            bIVActive_back[ChnIdx] = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive;
        }

        memcpy((void*)&ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum, (void*)&st_CANBMS_Map->m_AllChnNum, sizeof(ST_CANBMS_MAP));
        if(BuiltIn_DataManager::m_Sample_Flag)
        {
            BuiltIn_DataManager::m_WriteFRAM_Flag = true;
            while(BuiltIn_DataManager::m_Sample_Flag);
        }
        CConfig::WriteContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum, FRAM_CANBMS_MAP_BASE, sizeof(ST_CANBMS_MAP));
        if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
        {
            BuiltIn_DataManager::m_Sample_Flag = true;
            BuiltIn_DataManager::m_WriteFRAM_Flag = false;
        }
        CANBusParse::CheckCanBmsMap();

        for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
        {
            CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[ChnIdx].m_UnsafeTime);
            memset((void*)&ChannelInfo_Manager::m_CANMetaVariable[ChnIdx][0], 0, sizeof(ST_BMS_MetaValue)*MAX_CAN_BMS_SIGNAL); //170621 zyx

            if((ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive) && (bIVActive_back[ChnIdx])) // 更新前与更新后一致，不用清理。
                continue;
            CANBusParse::m_BMS_OverTime[ChnIdx].m_bCANBMSAlive = false;		//reset flag	 20160321 yy
            CANBusParse::m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Normal;
            CANBusParse::m_BMS_SendBuf_RunOn[ChnIdx] = 0;

            for(unsigned short validIdx = 0; validIdx < MAX_CAN_BMS_ACK_BUF; validIdx++)
            {
                CANBusParse::m_BMSAckConfirmMgr[ChnIdx].m_Buf[validIdx].m_AckStatus = en_AckNULL;
                CANBusParse::m_BMSAckConfirmMgr[ChnIdx].m_Buf[validIdx].m_OverTick = 0;
                CANBusParse::m_BMSAckConfirmMgr[ChnIdx].m_Buf[validIdx].m_ReTryCnt = 0;
            }
        }
        MultiMcuComm::Scan_IV_CANBMSMAP();
#else
        ST_CANBMS_MAP* st_CANBMS_Map = (ST_CANBMS_MAP*)&m_btPackDataBuffer[0];
        AuxCConfig::WriteContent_ForFRAM((unsigned char *)&m_btPackDataBuffer[0], FRAM_AUXCANBMS_MAP_BASE, FRAM_AUXCANBMS_MAP_LENGTH);
        MultiMcuComm::Scan_IV_CANBMSMAP();
#endif

    }

//     if(uc_DownLoad_Success >= 0)
//     {
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//     }
}

//设置BMS配置表
void EthernetParse::MPSetBMSChannelConfig(MP_SINGLE_PACKDATA *pCmd)
{

    unsigned char uc_DownLoad_Success = 0;

//	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    //TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
    //	                    & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

//    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_FAIL);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{
// 		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
// 		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
// 	}
// 	else
// 	{
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
// 		{
// 			// 判断是否为同一个多包的内容。
// 			m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 			m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 			MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 			return;
// 		}
// 		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
// 	}

// 	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
// 	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

// 	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
// 	{
// 		// 判断是否为同一个多包的内容接收完成。
// 		uc_DownLoad_Success = 1;
// 		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	}
// 	if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
// 	{
// 		m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	}
// 	MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);


// 	// 存包中的数据
// 	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
        //#if( USE_AS_AUXMCU == 0 && SUPPORT_CAN_BMS==1)
#if( USE_AS_AUXMCU == 0)
        /*ST_CANBMS_CfgUnit *st_tmpConfigUnit;
        		memcpy((void*)&ChannelInfo_Manager::m_CANCofigItem.m_IVChnIdx,(void*)&m_btPackDataBuffer[0],sizeof(ST_Config_Item));
        		unsigned int CfgShiftAdr = sizeof(ST_Config_Item); //20; //20(m_AllChnNum+m_ItemCount*MAXCHANNELNO) 170505 zyx
        		unsigned char chn_Idx = ChannelInfo_Manager::m_CANCofigItem.m_IVChnIdx;

        		st_tmpConfigUnit = (ST_CANBMS_CfgUnit*)&m_btPackDataBuffer[CfgShiftAdr];
        //		st_tmpConfigUnit->m_GlobalCfg.m_ItemCount = ChannelInfo_Manager::m_CANCofigItem.m_Items[chn_Idx];
        		ChannelInfo_Manager::Convert_CAN_MetaSignal(chn_Idx,st_tmpConfigUnit);
        		//CfgShiftAdr += sizeof(ST_CAN_GlobalCfg)+sizeof(ST_CAN_BMS_SIGNAL) * (ChannelInfo_Manager::m_CANCofigItem.m_Items[chn_Idx].TotalItems); //170616 zyx
        		CfgShiftAdr += sizeof(ST_CAN_GlobalCfg)+sizeof(ST_CAN_BMS_SIGNAL) * (ChannelInfo_Manager::m_CANCofigItem.m_Items[chn_Idx]);	 //170616 zyx 		*/

        ST_CANBMS_CONFIG  *st_Config = (ST_CANBMS_CONFIG*)&m_btPackDataBuffer[0];
        unsigned char chn = st_Config->m_IVChnIdx;
        unsigned int count = st_Config->m_ItemCount[chn];
        st_Config->m_BMS_Chn[0].m_GlobalCfg.m_ItemCount.TotalItems = count;
        ChannelInfo_Manager::Convert_CAN_MetaSignal(chn, &st_Config->m_BMS_Chn[0]);
#endif
    }

//     if(uc_DownLoad_Success >= 0)
//     {
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//     }
}
//手动发送CAN消息
void EthernetParse::MPSetCANMessage(MP_SET_CANMSG *pCmd)
{
#if  USE_AS_AUXMCU == 0

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd; // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
// 	m_ResultFeedback.m_btResult = BTUPDATE_CHECK_SUCCESS;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    //unsigned char msgIdx = CANBusParse::m_BMS_SendBuf[pCmd->m_IVChnNo].m_validMsgCnt;

    //if(CANBusParse::m_BMS_SendBuf[pCmd->m_IVChnNo].m_glbStopAll)   //if glb flag is stopped
    //    return;//20150226,hpq
    pCmd->m_CANMsg.m_bOutboundMsg = 0;  //190617,hpq
    CANBusParse::SetCanBmsData(pCmd);


#endif
}

#if( USE_AS_AUXMCU == 1)
void EthernetParse::MPSetMultiCANMessage(MP_SINGLE_PACKDATA  *pCmd) //170828 zyx
{

    unsigned char uc_DownLoad_Success = 0;

//     m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
//     //TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
//     //	                    & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// // 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// // 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// // 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// // 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// // 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

//     CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

//     if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
//     {
//         st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
//         st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
//     }
//     else
//     {
//         if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
//         {
//             // 判断是否为同一个多包的内容。
//             m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//             m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
//             MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//             return;
//         }
//         st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
//     }

//     st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
//     st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

//     if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
//     {
//         // 判断是否为同一个多包的内容接收完成。
//         uc_DownLoad_Success = 1;
//         m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
//     }
//     if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
//     {
//         m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//     }

//     // 存包中的数据
//     memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据

    uc_DownLoad_Success = GetSinglePack(pCmd,false);

    if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    if(uc_DownLoad_Success == 1)
    {
        ST_CANBMS_AuxMultiMsg * st_tmpAuxMulitMsg = (ST_CANBMS_AuxMultiMsg*)&m_btPackDataBuffer[0];
        if(st_tmpAuxMulitMsg->m_AllStop)
        {
            memset((void*)&CANBusParse::m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg, 0, sizeof(ST_CANBMS_AuxMultiMsg));
        }
        else
        {
            memcpy(&CANBusParse::m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg, st_tmpAuxMulitMsg, sizeof(ST_CANBMS_AuxMultiMsg));
        }
        CANBusParse::m_BMS_AuxMultiMsgManage.bIsOneCycle = 0; //1709111 zyx
        CANBusParse::m_BMS_AuxMultiMsgManage.MsgIdx = 0;
        CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_AuxMultiMsgManage.m_SendInterval);
        CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_AuxMultiMsgManage.m_FrameInterval);
    }
}
#endif

void EthernetParse::MPSetMetaVariable(MP_SET_META_VARIABLE* pCmd)
{
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    int MetaType = pCmd->m_MV_Type;
    int MetaCode = pCmd->m_MV_MetaCode;
    int MetaValueType = pCmd->m_MV_ValueType;

    float fMetaValue = pCmd->m_MV_Data.fMV_Value;
    int Channel = pCmd->m_Channel;
    float MetaVariableValue;
    float getVariableValue = 0;

    switch (MetaValueType)
    {
    case MP_DATA_TYPE_METAVALUE:
    {
        MetaVariableValue = fMetaValue;
        break;
    }

    default:
        break;
    }

    switch (MetaType)
    {
    case MP_DATA_TYPE_METAVALUE:
    {
#if USE_AS_AUXMCU==0
        getVariableValue = ChannelInfo_Manager::MetaVariable2[Channel][MetaCode].m_fValue;
        if((MetaCode >= MetaCode_MV_UD1 && MetaCode <= MetaCode_MV_UD4)\
                || (MetaCode >= MetaCode_MV_UD5 && MetaCode <= MetaCode_MV_UD16))
        {

            if(getVariableValue != MetaVariableValue)
                ChannelInfo_Manager::uc_MV_Udx_Update[Channel] = 1;

            MetaVariable_Task::bSendMVUD[Channel] = true;
        }

        ChannelInfo_Manager::Write_SpecifiedMetavariable2(Channel,MetaCode,MetaVariableValue);
        IVSDL::SetLogMVUDFlag(Channel, MetaCode);
#endif
        break;
    }

    default:
        break;
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}

void EthernetParse::MPUpdateMetaVariable(MP_UPDATE_META_VARIABLE_ADVANCEMODEL * pCmd)
{
    UWord16  uw16_Channel = 0;
    UWord16  uw16_MetaCode = 0;
    float fMetaValue = 0;
    float getVariableValue = 0;

    //CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    for(unsigned char uc_Meta = 0; (uc_Meta < pCmd->m_wMV_Total) && (uc_Meta < MAX_METAVARIABLE_SINGLE_PACK); uc_Meta++)
    {
#if USE_AS_AUXMCU==0
        uw16_Channel = pCmd->m_MV_Data[uc_Meta].m_Channel;
        uw16_MetaCode = pCmd->m_MV_Data[uc_Meta].m_MV_MetaCode;
        fMetaValue = pCmd->m_MV_Data[uc_Meta].fMV_Value;

        getVariableValue = ChannelInfo_Manager::MetaVariable2[uw16_Channel][uw16_MetaCode].m_fValue;
        if((uw16_MetaCode >= MetaCode_MV_UD1 && uw16_MetaCode <= MetaCode_MV_UD4)\
                || (uw16_MetaCode >= MetaCode_MV_UD5 && uw16_MetaCode <= MetaCode_MV_UD16))
        {
            if(getVariableValue != fMetaValue)
                ChannelInfo_Manager::uc_MV_Udx_Update[uw16_Channel] = 1;

            MetaVariable_Task::bSendMVUD[uw16_Channel] = true;
        }

        ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Channel,uw16_MetaCode,fMetaValue);
        IVSDL::SetLogMVUDFlag(uw16_Channel, LOG_ALL_MVUD);
#endif
    }

    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    //MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
void EthernetParse::MPSetIntervalTime_Logdata(MP_SET_INTERVALTIME_LOGDATA * pCmd)
{
    unsigned long long uw64_SetTime = pCmd->f_SetIntervalTimeLog*10000;   // s->100us
    if(uw64_SetTime < FAST_UPDATA_DIPLAY)  // 5ms
        uw64_CTI_SetTimeInterval = FAST_UPDATA_DIPLAY;
    else
        uw64_CTI_SetTimeInterval = uw64_SetTime;
#if  USE_AS_AUXMCU == 0   //针对IVMCU
    Schedule_Manager_task::SDLIdeShowMsTime_Updata();
#endif
    SendFeedBack(MPCS_ACK,0);
}

#if USE_AS_AUXMCU==0
void EthernetParse::MPGetMetaVariables(MP_GET_META_VARIABLES *pCmd,MP_GET_META_VARIABLES_FEEDBACK *pFeedback)
{
    UWord16  uw16_Channel = 0;
    UWord16  uw16_MetaCode = 0;

    memset(pFeedback,0,sizeof(MP_GET_META_VARIABLES_FEEDBACK));

    for(unsigned char uc_Meta = 0; (uc_Meta < pCmd->m_wMV_Total) && (uc_Meta < MAX_GET_METAVARIABLES_SINGLE_PACK); uc_Meta++)
    {
        uw16_Channel = pCmd->m_MV_Data[uc_Meta].m_Channel;
        uw16_MetaCode = pCmd->m_MV_Data[uc_Meta].m_MV_MetaCode;

        pFeedback->m_MV_Data[uc_Meta].m_Channel = uw16_Channel;
        pFeedback->m_MV_Data[uc_Meta].m_MV_MetaCode = uw16_MetaCode;
        pFeedback->m_MV_Data[uc_Meta].m_MV_Error = CTI_GET_MV_ERROR;
        pFeedback->m_MV_Data[uc_Meta].m_MV_DataType = pCmd->m_MV_Data[uc_Meta].m_MV_DataType;

        if((uw16_Channel >= MAXCHANNELNO) || (uw16_Channel >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount))//超出IV通道
        {
            pFeedback->m_MV_Data[uc_Meta].m_MV_Error = CTI_GET_MV_CHANNEL_INDEX_ERROR;
            continue;
        }

        if(uw16_MetaCode >= MetaCode_MV_COUNT)
        {
            pFeedback->m_MV_Data[uc_Meta].m_MV_Error = CTI_GET_MV_METACODE_NOTEXIST;
            continue;
        }

        pFeedback->m_MV_Data[uc_Meta].m_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_Channel,uw16_MetaCode);
        pFeedback->m_MV_Data[uc_Meta].m_MV_Error = CTI_GET_MV_SUCCESS;
    }

    pFeedback->m_dwCmd = 						CMD_GET_META_VARIABLES_FEEDBACK;
    pFeedback->m_wMV_Total = 				pCmd->m_wMV_Total;

    memcmp(m_CmdTxBuffer_SynchroChannel+1,pFeedback,sizeof(MP_GET_META_VARIABLES_FEEDBACK));
    SendFeedBack(MPCS_ACK, sizeof(MP_GET_META_VARIABLES_FEEDBACK));
}
#endif

void EthernetParse::MPSetIVBoardType(MP_SET_IV_BOARDTYPE *pCmd)
{
//   m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
//   m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = 0;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = 1;
//   m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
//   m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

#if USE_AS_AUXMCU==0
    ChannelInfo_Manager::m_st_BoadType = pCmd->m_st_BoadType;
    if(BuiltIn_DataManager::m_Sample_Flag)
    {
        BuiltIn_DataManager::m_WriteFRAM_Flag = true;
        while(BuiltIn_DataManager::m_Sample_Flag);
    }
    CConfig::WriteContent_ForFRAM((unsigned char *) &ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG));
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        BuiltIn_DataManager::m_Sample_Flag = true;
        BuiltIn_DataManager::m_WriteFRAM_Flag = false;
    }
    ChannelInfo_Manager::InitVirtualCurrentRange();//170320,add by hepeiqing
//   m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    SetIVBoardTypeFlag = 1;
#else
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
#endif
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
    CommonFunc::GetTimeTicksUpdate( &SetIVBoardTypeTime);
}

void EthernetParse::MPSetDspBasicParameter(MP_SET_DSP_BASIC_INFO *pCmd)
{
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDspBoardType = pCmd->m_btDspBoardType;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fDCVoltageOffset = pCmd->m_fDCVoltageOffset;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fChannelVoltage = pCmd->m_fChannelVoltage;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fSecondaryVoltage = pCmd->m_fSecondaryVoltage;

    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btSw_Freq = pCmd->m_btSw_Freq;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btHZ_XMFR = pCmd->m_btHZ_XMFR;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btGrid_Voltage = pCmd->m_btGrid_Voltage;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDSPBoardType = pCmd->m_btDSPBoardType;
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btReadResult = false;
    CANBusParse::DSP_IV_SendBasicParameterToDsp(&PWM_Base::Record_DSP_Basic_Parameter[DSP_ID], DSP_ID + 130);
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
#else
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
#endif
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
void EthernetParse::MPSetDspAdjustParameter(MP_SET_DSP_ADJUST_INFO *pCmd)
{
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KP = pCmd->m_fV_KP;
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KI = pCmd->m_fV_KI;
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KP = pCmd->m_fI_KP;
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KI = pCmd->m_fI_KI;
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_btReadResult = false;
    CANBusParse::DSP_IV_SendAdjustParameterToDsp(&PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID], DSP_ID + 130);
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
#else
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
#endif
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
void EthernetParse::MPReadDspBasicParameter(MP_READ_DSP_BASIC_INFO *pCmd)
{
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_dwCmd = pCmd->m_dwCmd;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btResult = 1;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btReadSuccess = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btReadResult;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btDspBoardType = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDspBoardType;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_fSecondaryVoltage = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fSecondaryVoltage;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_fChannelVoltage = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fChannelVoltage;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_fDCVoltageOffset = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fDCVoltageOffset;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btSw_Freq = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btSw_Freq;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btGrid_Voltage = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btGrid_Voltage;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btHZ_XMFR = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btHZ_XMFR;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_btDSPBoardType = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDSPBoardType;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_wDSP_Version = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_wDSP_Version;
    m_pFeedBack->m_Read_Dsp_Basic_Parameter_Feedback.m_wDSP_TYorZY = PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_wDSP_TYorZY;
    SendFeedBack(MPCS_ACK, sizeof(MP_SET_DSP_BASIC_INFO));
    PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btReadResult = false;//读之前先清除一下。
    CANBusParse::DSP_IV_ReadBasicParameterFromDsp(DSP_ID + 130);
#else
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}
void EthernetParse::MPReadDspAdjustParameter(MP_READ_DSP_ADJUST_INFO *pCmd)
{
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_dwCmd = pCmd->m_dwCmd;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_btResult = 1;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_btReadSuccess = PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_btReadResult;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_fV_KP = PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KP;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_fV_KI = PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KI;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_fI_KP = PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KP;
    m_pFeedBack->m_Read_Dsp_Adjust_Parameter_Feedback.m_fI_KI = PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KI;
    SendFeedBack(MPCS_ACK, sizeof(MP_SET_DSP_ADJUST_INFO));
    PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_btReadResult = false;//读之前先清除一下。
    CANBusParse::DSP_IV_ReadAdjustParameterFromDsp(DSP_ID + 130);
#else
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}
void EthernetParse::MPSetDspIndentityParameter(MP_SET_DSP_INDENTITY_INFO *pCmd)
{
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;

    PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_Indentity = pCmd->m_DSP_Indentity;
    PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveCountOrMasterID = pCmd->m_DSP_SlaveCountOrMasterID;
    for(unsigned char i = 0; i<8; i++)
        PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveID[i] = pCmd->m_DSP_SlaveID[i];


    PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_btReadResult = false;
    CANBusParse::DSP_IV_SendIndentityParameterToDsp(&PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID], DSP_ID + 130);
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
#else
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
#endif
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
void EthernetParse::MPReadDspIndentityParameter(MP_READ_DSP_INDENTITY_INFO *pCmd)
{
#if USE_AS_AUXMCU==0
    unsigned char DSP_ID;
    DSP_ID = pCmd->m_btDspID;
    if(DSP_ID >= MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER - 1;
    m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_dwCmd = pCmd->m_dwCmd;
    m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_btResult = 1;
    m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_btReadSuccess = PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_btReadResult;
    m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_DSP_Indentity = PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_Indentity;
    m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_DSP_SlaveCountOrMasterID = PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveCountOrMasterID;
    for(unsigned char i = 0; i<8; i++)
        m_pFeedBack->m_Read_Dsp_Indentity_Parameter_Feedback.m_DSP_SlaveID[i] = PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveID[i];

    SendFeedBack(MPCS_ACK, sizeof(MP_SET_DSP_INDENTITY_INFO));
    PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_btReadResult = false;//读之前先清除一下。
    CANBusParse::DSP_IV_ReadIndentityParameterFromDsp(DSP_ID + 130);
#else
    m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}
//#if (USE_AS_AUXMCU == 0 && SUPPORT_CAN_BMS==1)
#if (USE_AS_AUXMCU == 0)
void EthernetParse::CombineBMSPackage(unsigned char uc_ChannelNo, unsigned char uc_SendType, unsigned char Interval)
{
    int cfgidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    if( cfgidx < 0 )//  20160317  yy  解决IV不带CANBMS通道时，发送空包问题。
        return;
    if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive)
        return;

    unsigned char uc_DataType = SDL_DATA_FOR_IDLE;

    if(uc_SendType ==  SEND_LOG_BMS_DATA)
    {
        uc_DataType = SDL_DATA_FOR_LOG;
    }

    MetaItemsBufAddr MetaItemsBuf = CANBusParse::GetMetaItemsBufAddr(cfgidx, Interval);

    MP_REPORT_CANBMS_Data *CANBmsData = (MP_REPORT_CANBMS_Data*)&m_btPackDataUploadBuffer[16]; //170815 zyx
    CANBmsData->m_CANData.m_bBmsLost = (CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_BMSStatus == e_BMS_Unsafe);  //170122
    MsTime SysTime = CommonFunc::GetSystemTime();
    CANBmsData->m_CANData.m_Second = SysTime.Second;
    CANBmsData->m_CANData.m_Us100  = SysTime.Us100;
    //CANBmsData->m_CANData.m_IVChnNo = uc_ChannelNo + ChannelInfo_Manager::BoardCFG.m_Global.m_uFrom; //WL add m_uFrom 02.17.2020
    CANBmsData->m_CANData.m_IVChnNo = uc_ChannelNo;
    CANBmsData->m_CANData.m_DataCnt = 0;
    CANBmsData->m_CANData.m_DataType = uc_DataType;

    unsigned char IsLastPoint = 0;

    if(CANBusParse::lastCANBMSData[uc_ChannelNo])
    {
        IsLastPoint = 1;
        CANBmsData->m_CANData.m_bStepLastPoint = 1;
        CANBusParse::lastCANBMSData[uc_ChannelNo] = false;
    }
    else
    {
        CANBmsData->m_CANData.m_bStepLastPoint = 0;
    }

    CANBMS_METACODE currentMetaCode;
//    CANBMS_METACOUNT uw16_sum = MetaItemsFrom;
    for(CANBMS_METAIdx uc_MetaIdx = MetaItemsBuf.MetaItemsBufFrom; uc_MetaIdx < MetaItemsBuf.MetaItemsBufTo; uc_MetaIdx++) //170505 zyx
    {   //judegement DataLog is enable  Meta tab search is too slowly!!!
        currentMetaCode = CANBusParse::m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_MetaIdx].m_CAN_RX_Meta;
        CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].m_Value    = ChannelInfo_Manager::m_CANMetaVariable[uc_ChannelNo][currentMetaCode].m_fValue;
        CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].m_MetaCode = currentMetaCode;
        // 		CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].m_bLogData = (  ChannelInfo_Manager::m_CANMetaVariable[uc_ChannelNo][uc_MetaIdx].m_bNeedLog
        // 		                                                                           && (uc_DataType == SDL_DATA_TYPE_LOG) );
//         CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].m_bLogData = (  CANBusParse::m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_MetaIdx].m_DataLog \
//             && (uc_DataType == SDL_DATA_TYPE_LOG) ); //170608 zyx
        CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].b_IsLogData = (uc_DataType == SDL_DATA_FOR_LOG); // 170608 zyx
        CANBmsData->m_CANData.m_Meta[CANBmsData->m_CANData.m_DataCnt].m_LogDataInterval = Interval;


        CANBmsData->m_CANData.m_DataCnt++;
        //uw16_sum ++;

        if(CANBmsData->m_CANData.m_DataCnt >= 130)   // 56+12+8*130=1108
        {
            CANBmsData->m_CANData.m_bStepLastPoint = IsLastPoint;
            MultPackDataToSinglePack(uc_SendType);
            CANBmsData->m_CANData.m_DataCnt = 0;
//             if(CANBusParse::m_CANConfig.m_buf[cfgidx].m_MetaCount == 130)
//                 return;
        }

//         if(uw16_sum >= CANBusParse::m_CANConfig.m_buf[cfgidx].m_MetaCount)
//             break;
    }
    //	 PushToSendingEventBuf(&m_btPackDataBuffer[0],EventTx_Length);  //
    if((MetaItemsBuf.MetaItemsBufTo - MetaItemsBuf.MetaItemsBufFrom) % 130 != 0)
        MultPackDataToSinglePack(uc_SendType);

}

void EthernetParse::MPReportCANData(unsigned char uc_ChannelNo, unsigned char uc_SendType)
{
    if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive)
        return;

    if(uc_SendType == SEND_LOG_BMS_DATA)
    {
        if(CANBusParse::lastCANBMSData[uc_ChannelNo])//
        {
            CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_Total);
        }
        else
        {
            //20150216,add by hepeiqing,
            if(ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1)// 170606 zyx
            {
                if(CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_FastRptLimitTick) > (CAN_BMS_DATA_RPT_FAST_INTERVAL))
                {
                    CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_500MS);
                    CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_FastRptLimitTick);
                }
            }
            else //170623 zyx
            {
                CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_500MS);
                CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_FastRptLimitTick);
            }

            if(CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_MidRptLimitTick) > (CAN_BMS_DATA_RPT_MID_INTERVAL))
            {
                CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_1S);
                CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_MidRptLimitTick);
            }
            if (CommonFunc::TimeDiffInSecond(CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_SlowRptLimitTick.Second) > (CAN_BMS_DATA_RPT_SLOW_INTERVAL ))
            {
                CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_30S);
                CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick.m_CANBMS_SlowRptLimitTick);
            }
        }
    }
    else   //SDL_DATA_TYPE_DISPLAY and SDL_DATA_TYPE_IDLE
    {
        if( CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick) < CAN_BMS_NOTLOGGED_RPT_INTERVAL )
            return;
        else
        {
            CombineBMSPackage(uc_ChannelNo, uc_SendType, INTERVAL_Total);
            CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick);
        }
    }


}
void EthernetParse::MPSetSMBChannelConfig(MP_SINGLE_PACKDATA *pCmd)
{
    unsigned char uc_DownLoad_Success = 0;

//	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{
// 		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
// 		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
// 	}
// 	else
// 	{
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
// 		{
// 			// 判断是否为同一个多包的内容。
// 			m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 			m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 			MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 			return;
// 		}
// 		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
// 	}

// 	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
// 	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

// 	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
// 	{
// 		// 判断是否为同一个多包的内容接收完成。
// 		uc_DownLoad_Success = 1;
// 		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	}

//   //memcpy(&Smart_Battery::DownLoadConfig.m_IVChnIdx,0,sizeof(ST_SMBBMS_CfgUnit));
// 	// 存包中的数据
// 	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength);
    uc_DownLoad_Success = GetSinglePack(pCmd,true);



    if(uc_DownLoad_Success == 1)
    {
        //#if( USE_AS_AUXMCU == 0 && SUPPORT_CAN_BMS==1)
#if( USE_AS_AUXMCU == 0)
        // pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

        //memcpy(&Smart_Battery::DownLoadConfig.m_IVChnIdx,&m_btPackDataBuffer[0],sizeof(ST_SMBBMS_CONFIG));  //待优化
        ST_SMBBMS_CONFIG *  st_config = (ST_SMBBMS_CONFIG *) &m_btPackDataBuffer[0];

        unsigned long m_Chn = st_config->m_IVChnIdx;
        //unsigned long m_Chn=Smart_Battery::DownLoadConfig.m_IVChnIdx;
        memcpy(&Smart_Battery::DownLoadConfig.m_BMS_Chn[m_Chn], 0, sizeof(ST_SMBBMS_CfgUnit));
        Smart_Battery::DownLoadConfig.m_BMS_Chn[m_Chn] = st_config->m_BMS_Chn;
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_BaudRate = st_config->m_BMS_Chn.m_GlobalCfg.m_BaudRate  ;
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Fast = st_config->m_BMS_Chn.m_GlobalCfg.m_Refresh_Fast;  //add by zc 03.04.2019
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Slow = st_config->m_BMS_Chn.m_GlobalCfg.m_Refresh_Slow;  //add by zc 03.04.2019
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_UnsafeTimeout = st_config->m_BMS_Chn.m_GlobalCfg.m_UnsafeTimeout * 10;	 //ADDED BY ZC 12012017

        I2CBusNew::I2CInit(I2C1, Smart_Battery::m_SMBCfgBuf.m_buf[0].m_BaudRate);
        Smart_Battery::BeSupportSmbus = true;
// 	    St_SMBMessage SMBMessage;
// 			SMBMessage.MsgNumber=SMB_Write_Msg;
// 			SMBMessage.m_SlaveAddr=0x16;   //入口地址
// 			SMBMessage.m_bufLen=3;         //字节数
// 			SMBMessage.Send_buf[0]=0x01;   //发送字节
// 			SMBMessage.Send_buf[1]=0x01;
// 			SMBMessage.Send_buf[2]=0x01;
//
// 			Smart_Battery::SendMsgToSMB(SMBMessage);

        Smart_Battery::m_SMBAssignMD5[0].m_cfgBufIdx = 0; //debug 2017/05/03
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount = st_config->m_BMS_Chn.m_GlobalCfg.m_ItemCount;
        Smart_Battery::m_SMBCfgBuf.m_buf[0].m_WholeSignalNum = st_config->m_BMS_Chn.m_GlobalCfg.m_ItemCount;
        for(unsigned char idx=0; idx<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_WholeSignalNum; idx++)
        {
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.CtrlByteNum=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_HeaderBuf.CtrlByteNum;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_BeUsed =st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Enable;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_COMM[0]=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_HeaderBuf.m_COMM[0];
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_COMM[1]=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_HeaderBuf.m_COMM[1];
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_ReadAddr=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_HeaderBuf.m_ReadAddr;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_WriteHeader=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_HeaderBuf.m_WriteHeader;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataLen=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataLen;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SMB_RX_Meta=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_SMB_Meta;

            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType>0)
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Sign=1;
            else
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Sign=0;
            if(st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType<3)
            {
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_WORD;//st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType;
                ChannelInfo_Manager::m_SMBusMetaVariable[m_Chn][Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SMB_RX_Meta].m_type = TYPE_WORD;
            }
            else
            {
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_STRING;
                ChannelInfo_Manager::m_SMBusMetaVariable[m_Chn][Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SMB_RX_Meta].m_type = TYPE_STRING;
            }

            if(st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType==4)
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_DATE;
            if(st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType==5)
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_SIRIALNUMBER;
            if(st_config->m_BMS_Chn.m_SMB_Signals[idx].m_DataType==6)
                Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_BITS_CONTROL; //add by zc 08.13.2018
            //st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Sign;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Endian=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Endian;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SignalID=idx;//st_config->m_BMS_Chn.m_SMB_Signals[idx].m_SignalID;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_EndDataIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_EndDataIndex;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_StartDataIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_StartDataIndex;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_EndByteIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_EndByteIndex;    //by zc 08.13.2018
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_StartByteIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_StartByteIndex;//by zc 08.13.2018
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_EndBitIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_EndBitIndex;//by zc 08.13.2018
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_StartBitIndex=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_StartBitIndex;//by zc 08.13.2018
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Gain=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Gain;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Offset=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Offset;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Datalog=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Enable;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Refresh=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Refresh;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_PECEnable=st_config->m_BMS_Chn.m_SMB_Signals[idx].m_PECEnable;
            Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Logging_Interval = st_config->m_BMS_Chn.m_SMB_Signals[idx].m_Logging_Interval; //add by zc 03.04.2019
            ChannelInfo_Manager::m_SMBusMetaVariable[m_Chn][idx].m_bNeedLog=Smart_Battery::m_SMBCfgBuf.m_buf[m_Chn].m_ValidSignal[idx].m_Datalog;
            ChannelInfo_Manager::m_SMBusMetaVariable[m_Chn][idx].m_bError=false;
            Smart_Battery::m_SMBSignalErrorCounter[m_Chn][idx]=0;
            Smart_Battery::FirstRefresh_Flag[idx] = true;
        }
        if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_WholeSignalNum == 0)
        {
            Smart_Battery::BeSupportSmbus = false;
        }

        ChannelInfo_Manager::SMB_SetMetaLog(m_Chn); //ADDED BY SML 20181213


        //	memcpy((void*)&ChannelInfo_Manager::m_SMBCofigItem.m_IVChnIdx,(void*)&m_btPackDataBuffer[0],sizeof(ST_Config_Item));
        //	unsigned int CfgShiftAdr = 20; //20(m_AllChnNum+m_ItemCount*MAXCHANNELNO)
        //	unsigned char chn_Idx = ChannelInfo_Manager::m_SMBCofigItem.m_IVChnIdx;

// 		st_tmpConfigUnit = (ST_SMBBMS_CfgUnit*)&m_btPackDataBuffer[CfgShiftAdr];
// 		st_tmpConfigUnit->m_GlobalCfg.m_ItemCount = ChannelInfo_Manager::m_SMBCofigItem.m_Items[chn_Idx];
// 		ChannelInfo_Manager::Convert_SMB_MetaSignal(chn_Idx,st_tmpConfigUnit);
// 		CfgShiftAdr += sizeof(ST_SMB_GlobalCfg)+sizeof(ST_SMB_BMS_SIGNAL)*ChannelInfo_Manager::m_SMBCofigItem.m_Items[chn_Idx];
#endif

        memset(&st_MultPackManage, 0, sizeof(ST_MultPack));  // 管理信息
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}

void EthernetParse::MPReportSMBDataFloat(unsigned char uc_ChannelNo, unsigned char uc_SendType)
{

    int cfgidx = 0;//Smart_Battery::m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    if( cfgidx < 0 )//  20160317  yy  解决IV不带CANBMS通道时，发送空包问题。
        return;


    unsigned char uc_DataType = SDL_DATA_FOR_IDLE;
    if(uc_SendType ==  SEND_LOG_SMB_DATA_FLOAT)
        uc_DataType = SDL_DATA_FOR_LOG;
// 	if(uc_DataType == SDL_DATA_TYPE_LOG)
// 	{//20150216,add by hepeiqing,
//     if(  (ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1)
//        &&(CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick)<CAN_BMS_DATA_RPT_INTERVAL) )
//       return;
//     else
//       CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick);
//     }
// 	else   //SDL_DATA_TYPE_DISPLAY and SDL_DATA_TYPE_IDLE
// 	{
// 		if( CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick) < 30000 )
// 			return;
// 		else
// 			CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick);
// 	}
    //bool bRptIdleData = false;
    MP_REPORT_SMB_Data_Float *SMBData = (MP_REPORT_SMB_Data_Float*)&m_btPackDataUploadBuffer[16];//m_btPackDataBuffer[16];
// 	SMBData->m_SMBData.m_Second = OS_ServeProcess::OS_Time.Second;
// 	SMBData->m_SMBData.m_Us100  = OS_ServeProcess::OS_Time.Us100;

    SMBData->m_SMBData.m_IVChnNo = uc_ChannelNo;
    SMBData->m_SMBData.m_IVUnitID = Smart_Battery::m_SMBUnitID;
    SMBData->m_SMBData.m_DataCnt = 0;
    if(Smart_Battery::lastSMBData[uc_ChannelNo])
    {
        SMBData->m_SMBData.m_IsLastPoint = 1;
        Smart_Battery::lastSMBData[uc_ChannelNo] = false;
    }
    else
        SMBData->m_SMBData.m_IsLastPoint = 0;
// 	SMBData->m_SMBData.m_DataType = uc_DataType;
// 	if((CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_IdleDataRptTick)>CAN_BMS_NOTLOGGED_RPT_INTERVAL)&&(uc_DataType != SDL_DATA_TYPE_LOG))
// 	{//20150216,modified by hepeiqing
// 		CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_IdleDataRptTick);
// 		//bRptIdleData = true;
// 	}
// 	//unsigned char  Idx = 0;
    unsigned char  currentMetaCode;
    UWord16 uw16_sum = 0;
// 	for(unsigned char uc_MetaIdx = 0;uc_MetaIdx< Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount;uc_MetaIdx++)
// 	{//judegement DataLog is enable  Meta tab search is too slowly!!!

    currentMetaCode = MetaVariable_Task::m_SMBLogData.MySMBMv_ID;//Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_ValidSignal[uc_MetaIdx].m_SMB_RX_Meta;

    if(MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_type != TYPE_WORD) // || Smart_Battery::m_UpdataFlg[currentMetaCode]==false)
        return;
    Smart_Battery::m_UpdataFlg[currentMetaCode] = false;
    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_MetaCode = currentMetaCode;
    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_DateTime = MetaVariable_Task::m_SMBLogData.MyRecorderTime; //MetaVariable_Task::m_SMBLog_Time[currentMetaCode];//OS_ServeProcess::OS_Time;
    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_bError = MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_bError; //ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bError;
    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_bError_Offlinebit = false;  // yy 20190430
//     if(Smart_Battery::uc_MVOffline_statuse)  // Check one instead of all,Simplified PC Processing.
    if(Smart_Battery::uc_MVOffline[currentMetaCode])
        SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_bError_Offlinebit = true;
// 			 SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_Value.m_Len=ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Len;
//        memcpy(&SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_Value.m_Data.m_SValue[0],&ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_SValue[0], \
// 							SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_Value.m_Len);  //= ChannelInfo_Manager::m_CANMetaVariable[uc_ChannelNo][currentMetaCode].m_fValue;
// 		}
// 		else
    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_fData = MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_Data.m_fValue[0];   //ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_fValue[0];

    SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_bLogData = (MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_bNeedLog  //ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bNeedLog
            && (uc_DataType == SDL_DATA_FOR_LOG) );

    SMBData->m_SMBData.m_DataCnt++;
    uw16_sum ++;

//         if(SMBData->m_SMBData.m_DataCnt >= 65)   // 56+12+15*65=1108
// 	 {
//             MultPackDataToSinglePack(uc_SendType);
//             SMBData->m_SMBData.m_DataCnt = 0;
//             if(Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount  >= MAX_SMB_BMS_SIGNAL)
//                 return;
//         }
//
// 		if(uw16_sum >= Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount)
// 		    break;
// 	 }
//	 PushToSendingEventBuf(&m_btPackDataBuffer[0],EventTx_Length);  //
    MultPackDataToSinglePack(uc_SendType);

}

void EthernetParse::MPReportSMBDataString(unsigned char uc_ChannelNo, unsigned char uc_SendType, unsigned char uc_MetaIdx)
{

    int cfgidx = 0;//Smart_Battery::m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    if( cfgidx < 0 )//  20160317  yy  解决IV不带CANBMS通道时，发送空包问题。
        return;


    unsigned char uc_DataType = SDL_DATA_FOR_IDLE;
    if(uc_SendType == SEND_LOG_SMB_DATA_STRING)
        uc_DataType = SDL_DATA_FOR_LOG;
// 	if(uc_DataType == SDL_DATA_TYPE_LOG)
// 	{//20150216,add by hepeiqing,
//     if(  (ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1)
//        &&(CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick)<CAN_BMS_DATA_RPT_INTERVAL) )
//       return;
//     else
//       CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_CANBMS_RptLimitTick);
//     }
// 	else   //SDL_DATA_TYPE_DISPLAY and SDL_DATA_TYPE_IDLE
// 	{
// 		if( CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick) < 30000 )
// 			return;
// 		else
// 			CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick);
// 	}
    //bool bRptIdleData = false;
    MP_REPORT_SMB_Data_String *SMBData = (MP_REPORT_SMB_Data_String*)&m_btPackDataUploadBuffer[16];//m_btPackDataBuffer[16]; zyx 171229
// 	SMBData->m_SMBData.m_Second = OS_ServeProcess::OS_Time.Second;
// 	SMBData->m_SMBData.m_Us100  = OS_ServeProcess::OS_Time.Us100;
    SMBData->m_SMBData.m_IVChnNo = uc_ChannelNo;
    SMBData->m_SMBData.m_IVUnitID = Smart_Battery::m_SMBUnitID;

    if(Smart_Battery::lastSMBData[uc_ChannelNo])
    {
        SMBData->m_SMBData.m_IsLastPoint = 1;
        Smart_Battery::lastSMBData[uc_ChannelNo] = false;
    }
    else
        SMBData->m_SMBData.m_IsLastPoint = 0;
    //SMBData->m_SMBData.m_DataCnt = 0;
// 	SMBData->m_SMBData.m_DataType = uc_DataType;
// 	if((CommonFunc::GetTimeTicks(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_IdleDataRptTick)>CAN_BMS_NOTLOGGED_RPT_INTERVAL)&&(uc_DataType != SDL_DATA_TYPE_LOG))
// 	{//20150216,modified by hepeiqing
// 		CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_IdleDataRptTick);
// 		//bRptIdleData = true;
// 	}
// 	//unsigned char  Idx = 0;
    unsigned char  currentMetaCode;
//     UWord16 uw16_sum=0;
// 	for(unsigned char uc_MetaIdx = 0;uc_MetaIdx< Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount;uc_MetaIdx++)
// 	{//judegement DataLog is enable  Meta tab search is too slowly!!!

    currentMetaCode = MetaVariable_Task::m_SMBLogData.MySMBMv_ID;//Smart_Battery::m_SMBLogIdx;//Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_ValidSignal[uc_MetaIdx].m_SMB_RX_Meta;
    if(MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_type == TYPE_WORD) // || Smart_Battery::m_UpdataFlg[currentMetaCode]!=true)
        return;
    Smart_Battery::m_UpdataFlg[currentMetaCode] = false;
    //SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_Value.m_type= ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_type;
    //SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_MetaCode = currentMetaCode;

    //	if(ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_type==TYPE_STRING)
    //	{
// 			unsigned short m_Dword;
// 				  unsigned long ml_year,ml_month,ml_date;
// 					unsigned char mS_date[10];
// 			m_Dword=0x43;
// 					m_Dword=(m_Dword<<8)+0x39;
// 					ml_year=((m_Dword>>8)&0x00fe)*256/512+1980;
// 					ml_month=(m_Dword>>5)&0x000f;
// 					ml_date=m_Dword&0x001f;
//
// 					mS_date[0]=ml_year/1000+0x30;
// 					mS_date[1]=ml_year%1000/100+0x30;
// 					mS_date[2]=ml_year%100/10+0x30;
// 					mS_date[3]=ml_year%10+0x30;
// 					mS_date[4]='.';
// 					mS_date[5]=ml_month/10+0x30;
// 					mS_date[6]=ml_month%10+0x30;
// 					mS_date[7]='.';
// 					mS_date[8]=ml_date/10+0x30;
// 					mS_date[9]=ml_date%10+0x30;
// 				  memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_SValue[0],&mS_date[0],10 );//m_SMBData.m_Len);
// 				  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_type=TYPE_STRING;
// 					ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Len=10;
// 				  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bNeedRecal=true;
// 					ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bError=false;


    SMBData->m_SMBData.m_DateTime = MetaVariable_Task::m_SMBLogData.MyRecorderTime;
    SMBData->m_SMBData.m_DataLength = MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_Len;
    SMBData->m_SMBData.m_MetaCode = currentMetaCode;
    SMBData->m_SMBData.m_bError = MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_bError;
    SMBData->m_SMBData.m_bError_Offlinebit = false;  // yy 20190430
//     if(Smart_Battery::uc_MVOffline_statuse)  // Check one instead of all,Simplified PC Processing.
    if(Smart_Battery::uc_MVOffline[currentMetaCode])
        SMBData->m_SMBData.m_bError_Offlinebit = true;
// 			 if(ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bNeedRecal==true)
// 				 ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bNeedRecal=false;
// 			 else
// 				 return;
//         for(unsigned char uc_i=0;uc_i<10;uc_i++)
// 					ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_SValue[uc_i]='1';

    memcpy(&SMBData->m_SMBData.m_sData[0], &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_SValue[0], \
           SMBData->m_SMBData.m_DataLength);  //= ChannelInfo_Manager::m_CANMetaVariable[uc_ChannelNo][currentMetaCode].m_fValue;
    //}
// 		else
    //SMBData->m_SMBData.m_Meta[SMBData->m_SMBData.m_DataCnt].m_fData =ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_Data.m_fValue[0];
    //	}
    SMBData->m_SMBData.m_bLogData = (  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][currentMetaCode].m_bNeedLog
                                       && (uc_DataType == SDL_DATA_FOR_LOG) );

// 		SMBData->m_SMBData.m_DataCnt++;
//       uw16_sum ++;
//
//         if(SMBData->m_SMBData.m_DataCnt >= 130)   // 56+12+8*130=1108
// 	 {
//             MultPackDataToSinglePack(uc_SendType);
//             SMBData->m_SMBData.m_DataCnt = 0;
//             if(Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount  == 130)
//                 return;
//         }

//		if(uw16_sum >= Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount)
    //	    return;
    MultPackDataToSinglePack(uc_SendType);
//	 }
//	 PushToSendingEventBuf(&m_btPackDataBuffer[0],EventTx_Length);  //


}

void EthernetParse::MPCANConfigAssign(MP_CANCONFIG_ASSIGN *pCmd)
{
    //St_ScheduleMessage   SchMessage;
    //UWord16 uw16_Chan;
    UChar8  uc_SuccessFlag = CONFIRM_RESULT_FAIL;
    pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber = pCmd->m_st_AssignCAN.m_u16ChannelIdx;   // 20140127 yangy  增加保护保持一致通道号
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	//m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;
    // m_st_Assign = & pCmd->m_CANConfig_Assign;  // 转存地址   //20130823
    unsigned char uc_ChannelNo = pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;
    if(memcmp(&CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_MD5, &pCmd->m_st_AssignCAN.m_MD5Assign.m_MD5, 16) != 0)
    {   //Reset BMS signal fValue when can config file changed  //170122
        if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive && (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID > 0))
        {
            CANBusParse::CANBMS_StopAllSend(uc_ChannelNo);		              //170122,stop all sending msg when .can changed,per YongUn/LG's request
            CANBusParse::m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt = 0;
            CANBusParse::m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll = 1;
        }

        for(int SigIdx = 0; SigIdx < MAX_CAN_BMS_SIGNAL; SigIdx++)
            ChannelInfo_Manager::m_CANMetaVariable[uc_ChannelNo][SigIdx].m_fValue = 0.0f;
    }
    CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_MD5 = pCmd->m_st_AssignCAN.m_MD5Assign.m_MD5; //  MD5码数组复制;

    St_MD5 EmptyMD5 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (memcmp(&pCmd->m_st_AssignCAN.m_MD5Assign.m_MD5, &EmptyMD5, sizeof(St_MD5)) == 0) // Add by DKQ 12.30.2015
        uc_SuccessFlag = CANBusParse::Clear_CANConfig(uc_ChannelNo);
    else
        uc_SuccessFlag = CANBusParse::Assign_CANConfig(uc_ChannelNo);
    if(uc_SuccessFlag == true)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    unsigned short  uw16_Length = 0;
    uw16_Length += (sizeof(MP_ASSIGN_CAN_FEEDBACK)   // 实际长度=总结构数-全部未填数组+已经填写数据数组;
                    - (sizeof(St_MD5Assign) * IV_MAX_ASSIGN_MD5_NUMBER));  // 去丢过多的数组成员数据
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

}
void EthernetParse::MPSMBConfigAssign(MP_SMBCONFIG_ASSIGN *pCmd)
{

    UChar8  uc_SuccessFlag = CONFIRM_RESULT_FAIL;
    pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber = pCmd->m_st_AssignSMB.m_u16ChannelIdx;
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;

    unsigned char uc_ChannelNo = pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;
    Smart_Battery::m_SMBAssignMD5[uc_ChannelNo].m_MD5 = pCmd->m_st_AssignSMB.m_MD5Assign.m_MD5; //  MD5码数组复制;

    St_MD5 EmptyMD5 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (memcmp(&pCmd->m_st_AssignSMB.m_MD5Assign.m_MD5, &EmptyMD5, sizeof(St_MD5)) == 0)
        uc_SuccessFlag = Smart_Battery::Clear_SMBConfig(uc_ChannelNo);
    else
        uc_SuccessFlag = Smart_Battery::Assign_SMBConfig(uc_ChannelNo);
    if(uc_SuccessFlag == true)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    unsigned short  uw16_Length = 0;
    uw16_Length += (sizeof(MP_ASSIGN_CAN_FEEDBACK)   // 实际长度=总结构数-全部未填数组+已经填写数据数组;
                    - (sizeof(St_MD5Assign) * IV_MAX_ASSIGN_MD5_NUMBER));  // 去丢过多的数组成员数据
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);



}
#endif
void EthernetParse::MPSetCANAUX_Broadcast_Config( MP_SINGLE_PACKDATA *pCmd)
{
    unsigned char uc_DownLoad_Success = 0;
    MP_SET_CANAUX_BROADCAST_CONFIG * st_Config;

    uc_DownLoad_Success = GetSinglePack(pCmd,true);
    if(uc_DownLoad_Success == 1)
    {
        st_Config = (MP_SET_CANAUX_BROADCAST_CONFIG *) &m_btPackDataBuffer[0];
        memset(&CANBusParse::m_CANAUX_Broadcast, 0, sizeof(St_CANAUX_Broadcast_Info));
        CANBusParse::m_CANAUX_Broadcast.m_Total_Message = st_Config->m_Total_Message;
        for(unsigned char MessageIndex = 0; MessageIndex < CANBusParse::m_CANAUX_Broadcast.m_Total_Message; MessageIndex++)
        {
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_U32ID = st_Config->m_AuxBroadcast_Message[MessageIndex].m_U32ID;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Period = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Period;
            if((CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Period != 0) && (CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Period < 500))
                CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Period = 500;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_FrameType = st_Config->m_AuxBroadcast_Message[MessageIndex].m_FrameType;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Endian = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Endian;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_CAN_Port = st_Config->m_AuxBroadcast_Message[MessageIndex].m_CAN_Port;

            unsigned char uc_idx = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Channel_Index1;
#if(USE_AS_AUXMCU == 1)   //aux用
            unsigned char uc_AuxType = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Aux_Type1;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_DataGroup1 = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_idx].m_data_group_idx;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Chan1 = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_idx].m_chn;
#else
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_DataGroup1 = 0;//暂时是都放到0组
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Chan1 = uc_idx;
#endif

            uc_idx = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Channel_Index2;
#if(USE_AS_AUXMCU == 1)   //aux用
            uc_AuxType = st_Config->m_AuxBroadcast_Message[MessageIndex].m_Aux_Type2;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_DataGroup2 = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_idx].m_data_group_idx;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Chan2 = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_idx].m_chn;
#else
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_DataGroup2 = 0;//暂时是都放到0组
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Chan2 = uc_idx;
#endif
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_DLC = st_Config->m_AuxBroadcast_Message[MessageIndex].m_DLC;
            if(CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_Period > 0)
                CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_bValid = 1;
            CANBusParse::m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex].m_bUpdated = 1;
        }
        if(CANBusParse::m_CANAUX_Broadcast.m_Total_Message > 0)
        {
            if(st_Config->m_CAN2FrontBaudRate != CANFunc::CAN2RecordBaudRate)
            {
                if(CANFunc::CAN2Init)
                    CANFunc::CAN_Reset_BaudRate(CAN2, st_Config->m_CAN2FrontBaudRate);
            }
            if(st_Config->m_CAN1RearBaudRate != CANFunc::CAN1RecordBaudRate)
            {
                if(CANFunc::CAN1Init)
                    CANFunc::CAN_Reset_BaudRate(CAN1, st_Config->m_CAN1RearBaudRate);
            }
        }
    }
    if( m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN )
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
//     if(uc_DownLoad_Success >= 0)
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
//#if (USA_AS_AUXMCU == 1 )||(SUPPORT_AUX == 1)
////////
// MPSetChannelMap(MP_SET_CHANNEL_BOARD *pCmd) : 网络命令设置通道Map信息处理函数
// Input: pCmd 网络设置板的通道信息命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetChannelMap(MP_SINGLE_PACKDATA *pCmd)
{
    //     UChar8 uc_jdx = 0;
    // 	unsigned char uc_Chan_Idx[16];
    unsigned char uc_DownLoad_Success = 0;

//     m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
//     TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
//                             & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// // 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// // 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// // 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
// // 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
// // 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

//     CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

//     if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
//     {
//         st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
//         st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
//     }
//     else
//     {
//         if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
//         {
//             // 判断是否为同一个多包的内容。
//             m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//             m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
//             MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//             return;
//         }
//         st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
//     }

//     st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
//     st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

//     if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
//     {
//         // 判断是否为同一个多包的内容接收完成。
//         uc_DownLoad_Success = 1;
//         m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
//     }

//     // 存包中的数据
//     memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据

    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
#if  USE_AS_AUXMCU == 1
        MP_AllIVMap  * st_AllIVMap;

        st_AllIVMap = (MP_AllIVMap *) &m_btPackDataBuffer[0];
        // 处理结构 todo: ???

        for(unsigned uc_i = 0; uc_i < MAX_IVUNIT; uc_i ++)
        {
            if((st_AllIVMap->m_ucUnitCount > MAX_MAP_COUNT_PER_UNIT) || (st_AllIVMap->m_ucIVChCount[uc_i] >  MAX_MAP_COUNT_PER_UNIT))
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
                m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAX_MAP_COUNT_PER_UNIT; //MAX_AUXCH_PER_AUXMCU; // 最大辅助通道数量保护
                //st_AllIVMap->m_ucUnitCount = MAX_IVMCU_PER_SYS;
                uc_DownLoad_Success = 0;
                break;
            }
        }


        if(uc_DownLoad_Success)
        {
            unsigned long uw32_Getlength = Aux_DataManager::Aux_GetMapfromPC( &m_ResultFeedback.m_btError_Code);    // 转换PCMap表以便管理
            Aux_DataManager::Aux_TranslateMapFromPC();  // add by qjm 2015/5/18
            m_AUX_WriteMap = 1;
            AuxCConfig::SaveAuxUnit2IVMap(st_AllIVMap, uw32_Getlength);  // 保存内容到FRAM
            m_AUX_WriteMap = 0;
            MTCI_CTRL::m_btRefreshMap = true;
        }
#else
        UWord16 uw16_jdx = Channel_MapAuxInfo::ConvertAuxMap();   // 转存到大数据体，一维转三维
        if(BuiltIn_DataManager::m_Sample_Flag)
        {
            BuiltIn_DataManager::m_WriteFRAM_Flag = true;
            while(BuiltIn_DataManager::m_Sample_Flag);
        }
        Channel_MapAuxInfo::SaveAuxMap_ForFRAM(uw16_jdx);  // 保存内容到FRAM中
        if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
        {
            BuiltIn_DataManager::m_Sample_Flag = true;
            BuiltIn_DataManager::m_WriteFRAM_Flag = false;
        }

        bool IsIVChnRunning = false;
        for(int IvChn = 0; IvChn < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; IvChn++)
        {
            if(Schedule_Manager_task::IfSchRunning(IvChn))
                IsIVChnRunning = true;
        }
        if(!IsIVChnRunning)//reconnect network will cause to redownload AuxMap. It will change Schedule Global Aux channel counts when this channel is running. zyx 2103021
            Channel_MapAuxInfo::AuxMapParse();  //dirui add 0313
#endif


    }

    if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}




#if  USE_AS_AUXMCU == 1
////////
// MPGetChannelBoard_Aux(MP_SET_CHANNEL_BOARD *pCmd) : 网络命令获取辅助板的通道信息处理函数
// Input: pCmd 网络获取板的通道信息命令结构指针;
// Return: N/A
///////
void EthernetParse::MP_MTCI_GroupSetting(MP_MTCI_SET_GROUP_SETTING *pCmd)//1910101 zyx
{
    unsigned char uc_DownLoad_Success = 0;
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);
    MP_MTCI_SET_GROUP_SETTING * st_MTCI_GroupSetting = pCmd;//(MP_MTCI_SET_GROUP_SETTING *) &m_btPackDataBuffer[16];
    //看看unit号，chn号是否有map过来
//     unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(st_MTCI_GroupSetting->m_uIvUnitIP);
//     if (st_MTCI_GroupSetting->m_uIvChanIndexInUnit > MAXCHANNELNO || unitIdx >= MAX_IVMCU_PER_SYS )
//     {
//         m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;
//     }
    /*add by WL 11.05.19 for MTC Group T-Chamber control*/
    if(Aux_MTC_Base::Aux_MTC_Board == true)
    {
        Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_UnitIP = st_MTCI_GroupSetting->m_uIvUnitIP;
        Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_IVChnIndexInUnit = st_MTCI_GroupSetting->m_uIvChanIndexInUnit;
        Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_UseGroupTChamber = st_MTCI_GroupSetting->m_btUseGroupTChamber;
        Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_DefaultTempSetting = st_MTCI_GroupSetting->m_fDefaultTemperatureSetting;
        Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_UseDefaultValue = st_MTCI_GroupSetting->m_bUseDefaultValue;
        for(int chamberIdx = 0; chamberIdx < MTC_MAX_CHANNELNO; chamberIdx++)
        {
            if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_UseGroupTChamber)
            {
                Aux_MTC_Base::groupCount++;
                if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_IVChnIndexInUnit > MAXCHANNELNO ||
                        Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_UnitIP > MAX_IVMCU_PER_SYS)
                {
                    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;
                }
                if( !Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_IsOpenChamber )
                {
                    Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_IsOpenChamber = true;
                }
                if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_UseDefaultValue)
                {
                    Aux_MTC_Base::MTCMode[chamberIdx] = 1; //MTC Remote Mode
                    DWIN_UartParse::MITSwitchRemote(chamberIdx);
                    Aux_MTC_Base::TurnOnMTCChn(chamberIdx);
                    if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_DefaultTempSetting > 60)
                        Aux_MTC_Base::Temperature_set[chamberIdx] = 600;
                    else if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_DefaultTempSetting < 10)
                        Aux_MTC_Base::Temperature_set[chamberIdx] = 100;
                    else
                        Aux_MTC_Base::Temperature_set[chamberIdx] = (long long) (Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_DefaultTempSetting * 10.0f);  //???????
                    //CANBusParse::testval[Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_IVChnIndexInUnit] = Aux_MTC_Base::st_MTC_Group_Ctrl[st_MTCI_GroupSetting->m_uChamberIndex].m_DefaultTempSetting = st_MTCI_GroupSetting->m_fDefaultTemperatureSetting;
                }
            }
            /*else
            {
            	Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_IsOpenChamber = false;
            	Aux_MTC_Base::MTCMode[chamberIdx] = 0; //MTC Remote Mode off
            	DWIN_UartParse::MITSwitchRemote(chamberIdx);
            	Aux_MTC_Base::TurnOffMTCChn(chamberIdx);
            }
            */
        }

        if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
        {
            m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        }
        MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
        return;
    }
    else
    {
        MTCI_CTRL::st_GroupControler[0].m_UnitIP = st_MTCI_GroupSetting->m_uIvUnitIP;
        MTCI_CTRL::st_GroupControler[0].m_IVChnIndexInUnit = st_MTCI_GroupSetting->m_uIvChanIndexInUnit;
        MTCI_CTRL::st_GroupControler[0].m_UseGroupTChamber = st_MTCI_GroupSetting->m_btUseGroupTChamber;

        if(st_MTCI_GroupSetting->m_btUseGroupTChamber)
        {
            unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(st_MTCI_GroupSetting->m_uIvUnitIP);
            if (st_MTCI_GroupSetting->m_uIvChanIndexInUnit > MAXCHANNELNO || unitIdx >= MAX_IVMCU_PER_SYS )
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;
            }
            MTCI_CTRL::st_GroupControler[0].m_UseDefaultValue = st_MTCI_GroupSetting->m_bUseDefaultValue;
            MTCI_CTRL::st_GroupControler[0].m_IgnoreDefaultValue = st_MTCI_GroupSetting->m_bIgnoreDefaultValue;
            MTCI_CTRL::st_GroupControler[0].m_DefaultTempSetting = st_MTCI_GroupSetting->m_fDefaultTemperatureSetting*10;
            MTCI_CTRL::st_GroupControler[0].m_DefaultHumiSetting = 550;

            if(MTCI_CTRL::st_GroupControler[0].m_UseDefaultValue)
            {
                MTCI_CTRL::st_GroupControler[0].m_IsOpenChamber = 0;
                if(!MTCI_CTRL::st_GroupControler[0].m_IgnoreDefaultValue)//2206071
                {
                    for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[0].uc_Chamber_TempCount; uc_Chamber++)//yzl 20200407
                    {
                        MTCI_CTRL::gW_TemperatureValue[0][uc_Chamber] = MTCI_CTRL::st_GroupControler[0].m_DefaultTempSetting;//zyx 1910101  GroupTChamber need to control TChamber to default value.
                    }

                    for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[0].uc_Chamber_HumiCount; uc_Chamber++)//yzl 20200407
                    {
                        MTCI_CTRL::gW_HumiValue[0][uc_Chamber] = MTCI_CTRL::st_GroupControler[0].m_DefaultHumiSetting;
                    }

                    MTCI_CTRL::SendWriteCmd(0,MTCI_CTRL::Get_SetPoinState(0,SEND_COM_WRITE_CMD));
                }
            }
            else
            {
                if( !MTCI_CTRL::st_GroupControler[0].m_IsOpenChamber)
                {
                    MTCI_CTRL::st_GroupControler[0].m_IsOpenChamber = 1;
                    MTCI_CTRL::SetPointValueCmd(0,SETPOINTVALUE_TEMPERATURE, 1);	//该函数第二个参数无意义
                }
                int unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(MTCI_CTRL::st_GroupControler[0].m_UnitIP);

                for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[0].uc_Chamber_TempCount; uc_Chamber++)//yzl 20200407
                {
                    MTCI_CTRL::gW_TemperatureValue[0][uc_Chamber] =  MTCI_CTRL::st_GroupControler[0].m_ChamberSetValue[unitIdx][MTCI_CTRL::st_GroupControler[0].m_IVChnIndexInUnit].TempValue;
                }

                for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[0].uc_Chamber_HumiCount; uc_Chamber++)//yzl 20200407
                {
                    if(MTCI_CTRL::st_GroupControler[0].m_ChamberSetValue[unitIdx][MTCI_CTRL::st_GroupControler[0].m_IVChnIndexInUnit].IsSetHumiValue)
                        MTCI_CTRL::gW_HumiValue[0][uc_Chamber] = MTCI_CTRL::st_GroupControler[0].m_ChamberSetValue[unitIdx][MTCI_CTRL::st_GroupControler[0].m_IVChnIndexInUnit].HumiValue;
                }

                MTCI_CTRL::SendWriteCmd(0,MTCI_CTRL::Get_SetPoinState(0,SEND_COM_WRITE_CMD));
            }
        }
        else
        {
            MTCI_CTRL::st_GroupControler[0].m_IsOpenChamber = 0;
            MTCI_CTRL::st_GroupControler[0].m_UseDefaultValue = 0;
        }

        if(m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
        {
            m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
        }
        MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
        return;
    }
}
void EthernetParse::MPGetChannelBoard_Aux(MP_GET_CHANNEL_BOARD *pCmd)
{
    UChar8    uc_jdx = 0;
    UChar8    uc_BInit = 0;
    UChar8    uc_BMax = MAX_BOARD_PER_AUXMCU;
    UChar8    uc_AuxFuncType = 0;
    UWord16   u16ChannelTotal = 0;
    //     UWord16   uw16_PhysicalCh;
    //     UWord16   uw16_AuxChennal = 0;
    MP_REPORT_GET_AUX_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_AUX_CHANNEL_BOARD *) &m_btPackDataBuffer[16];

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    if(pCmd->m_btBoradID == 0xBB) //如果命令来自Mitspro//1807131
    {
        unsigned char Aux_Type = 0;
        unsigned char m_ucScanChannelIdx[16];
        u16ChannelTotal = pCmd->m_btChannelCount;
        Aux_Type = pCmd->m_btCtrl_Type;

        st_Config->m_stMPack.m_stMFirstField.m_wMultPackFlag = 1;
        st_Config->m_u16ChannelCount = u16ChannelTotal;

        UWord32 uw32_Length = sizeof(MP_REPORT_GET_AUX_CHANNEL_BOARD) + CMD_HOLDHAND_LENGTH; // 多包总字节数  // 计算多少个包时已加入包头
        uw32_Length -= ((MAX_AUXCH_PER_AUXMCU - st_Config->m_u16ChannelCount) * (sizeof(St_AUXChannelCFG))); // 多包总字节数减去空字节数，仅发实际使用字节数。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = (uw32_Length / MAX_PACKLIMIT_SIZE); // 没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = ((uw32_Length % MAX_PACKLIMIT_SIZE) > 0) ? st_Config->m_stMPack.m_stMFirstField.m_wPackTotal + 1 : st_Config->m_stMPack.m_stMFirstField.m_wPackTotal; // 有余数要多一包，没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength = uw32_Length;
        st_Config->m_stMPack.m_stMFirstField.m_dwPackLength = uw32_Length > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : uw32_Length;


        for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
        {
            m_ucScanChannelIdx[uc_idx] = pCmd->m_ucChannelIdx[uc_idx]; // 缓冲数组，列表用
            st_Config->m_ucChannelIdx[uc_idx] = pCmd->m_ucChannelIdx[uc_idx];
        }

        for(unsigned char uc_ChanIdex = 0; uc_ChanIdex < u16ChannelTotal; uc_ChanIdex++) //通道总数
        {
            unsigned char AuxChannelInUnit;
            unsigned char FindChanSuccess = 0;
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) //16个unsigned char 值
            {
                for(UChar8 uc_bit = 0; uc_bit < 8; uc_bit++) //每个值8位
                {
                    if(((m_ucScanChannelIdx[uc_idx] >> uc_bit) & 1) == 1)
                    {
                        m_ucScanChannelIdx[uc_idx] &= (~(1 << uc_bit)); //一旦找到一位清除
                        AuxChannelInUnit = uc_idx * 8 + uc_bit;
                        FindChanSuccess = 1;
                        break;
                    }
                }
                if(FindChanSuccess == 1)
                    break;
            }
            if(FindChanSuccess == 0)//没找到直接返回
                return;

            //处理通道信息
            unsigned char uc_data_group_idx = 0;
            unsigned char uc_chn = 0;
            unsigned char uc_Bid = 0;

            uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[Aux_Type].m_StartInSnake + AuxChannelInUnit].m_data_group_idx;
            uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[Aux_Type].m_StartInSnake + AuxChannelInUnit].m_chn;

            uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];  //需要BID 信息。 得建立一个data_group_idx 与 ＢＩＤ拨码的关系
            if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
                uc_Bid = 0;
            if(Aux_Type == AUX_AO)
            {
                st_Config->m_Channel[uc_ChanIdex].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_Populated;
                st_Config->m_Channel[uc_ChanIdex].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_AuxType;
                st_Config->m_Channel[uc_ChanIdex].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_AuxSubType;
                st_Config->m_Channel[uc_ChanIdex].m_ChannelNumber = AuxChannelInUnit;//在该辅助类型在该unit下的通道号。
                st_Config->m_Channel[uc_ChanIdex].m_ChnCoreInfo.m_AuxSensorCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo;
                st_Config->m_Channel[uc_ChanIdex].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_avg_num;
            }
            else
            {
                st_Config->m_Channel[uc_ChanIdex].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) * 16 + uc_chn].m_Populated;
                st_Config->m_Channel[uc_ChanIdex].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) * 16 + uc_chn].m_AuxType;
                st_Config->m_Channel[uc_ChanIdex].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) * 16 + uc_chn].m_AuxSubType;
                st_Config->m_Channel[uc_ChanIdex].m_ChannelNumber = AuxChannelInUnit;//在该辅助类型在该unit下的通道号。
                st_Config->m_Channel[uc_ChanIdex].m_ChnCoreInfo.m_AuxSensorCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) * 16 + uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo;
                st_Config->m_Channel[uc_ChanIdex].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) * 16 + uc_chn].m_avg_num;
            }
        }

    }
    else//命令来自listenfornet
    {
        unsigned char uc_ok = AuxCConfig::Load_I2C_Config();
        if(uc_ok == 0)
        {
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD;  // 配置错误代码
            MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
            return;
        }
        //u16ChannelTotal = Aux_DataManager::m_AuxUnitCFG.m_u16AuxChannelCount;
        for(unsigned char uc_idx = 0; uc_idx < MAX_BOARD_PER_AUXMCU ; uc_idx ++) // 统计通道数  从下位机存储结构转换为上位机结构发送
        {
            u16ChannelTotal += Aux_DataManager::m_AllAuxBoards[uc_idx].m_BoardInfo.m_abi_globe.m_Total_Channels;  // 扫出一字列表中的通道参数
        }


        for (unsigned char uc_auxtype = AUX_VOLTAGE; uc_auxtype < MAXAUXTYPE; uc_auxtype++)//zyx 190418
        {
            st_Config->m_Global.m_u8ChCountPerType[uc_auxtype] = 0;
            for (unsigned char uc_BID = 0; uc_BID < MAX_BOARD_PER_AUXMCU; uc_BID++)
            {
                for(unsigned char uc_ch = 0; uc_ch < Aux_DataManager::m_AllAuxBoards[uc_BID].m_BoardInfo.m_abi_globe.m_Total_Channels; uc_ch++)	// 统计通道数
                {
                    if( Aux_DataManager::m_AllAuxBoards[uc_BID].m_BoardInfo.m_abi_chn[uc_ch].m_AuxType == uc_auxtype )   //  类型数通道统计
                    {
                        st_Config->m_Global.m_u8ChCountPerType[uc_auxtype] ++;	// 统计通道数
                    }
                }
            }
        }    //暂时屏蔽 qjm 20150603

        if(u16ChannelTotal == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
        {
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
                st_Config->m_ucChannelIdx[uc_idx] = 0XFF; // 缓冲数组，列表用
        }
        else
        {
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
                st_Config->m_ucChannelIdx[uc_idx] = 0X00; // 缓冲数组，列表用
        }

        if(u16ChannelTotal > MAX_AUXCH_PER_AUXMCU)
        {
            u16ChannelTotal = MAX_AUXCH_PER_AUXMCU;
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
            m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAX_AUXCH_PER_AUXMCU; // 通道号最大值保护
        }

        if(pCmd->m_btBoradID >= 0XFF) //要读的板地址
        {
            pCmd->m_btCtrl_Type =  CONTROLTYPE_USE_ALL;
            pCmd->m_btBoradID = 0; // 读取单板单通道保护
        }
        else
        {
            if(pCmd->m_btBoradID >= 8)
                pCmd->m_btBoradID = 0; // 读取单板保护
        }
        st_Config->m_Global.m_btCtrl_Type = pCmd->m_btCtrl_Type;   // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息 （预留开发 3：单独下载Map表 4：单独下载校正值）
        uc_AuxFuncType = pCmd->m_btAuxFuncType;
        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {
            st_Config->m_Global.m_abi_globe.m_BID =  pCmd->m_btBoradID;
            st_Config->m_Global.m_abi_globe.m_BoardType = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_BoardType;
            if((( st_Config->m_Global.m_abi_globe.m_BoardType == NUM_OF_AUX_BOARD_NULL) && ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD))
                    || (!Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_valid))
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }

            UWord16 u16ChannelValid;
            if((uc_AuxFuncType & 0x01) == 0x01)
            {
                u16ChannelValid = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_Total_Channels;
            }
            else if((uc_AuxFuncType & 0x10) == 0x10)
            {
                u16ChannelValid = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_Total_UART;
            }
            if((uc_AuxFuncType&0x20)==0x20)  //add by zc 04.26.2018
            {
                u16ChannelValid = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_SlaveBoardType;
            }
            else if((uc_AuxFuncType & 0x40) == 0x40)
            {
                u16ChannelValid = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_Total_DAC;
            }
            //u16ChannelTotal = Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_BoardInfo.m_abi_globe.m_Max_Channels;
            if(( u16ChannelValid == 0) && ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD_CHANNEL;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }
            // memcpy(&st_Config->m_Global.m_abi_globe.m_BID,&Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_abi_globe.m_BID,10);  // 板P/N Ver:
            st_Config->m_Global.m_abi_globe = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe;
            // st_Config->m_Global.m_u8ChCountPerType[st_Config->m_Global.m_BoardType.m_AuxType] = Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_BoardInfo.m_ChannelCount;
            st_Config->m_Global.m_assemble.m_ConsistentOrder = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_assemble.m_ConsistentOrder;      // 面板通道号是否与电气特性排序的通道序号一致
            st_Config->m_Global.m_assemble.m_SlaveGoesFirst =  Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_assemble.m_SlaveGoesFirst;      // 有无副板时，面板通道号是主板在前还是副板在前
            st_Config->m_Global.m_assemble.m_WithSlave =  Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_assemble.m_WithSlave;      // 有无副板
            //st_Config->m_Global.m_BoardType.m_u16Board_RSN =  Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_BoardInfo.m_BType.m_u16Board_RSN;            // 注册序号RSN
            st_Config->m_Global.m_u16Board_RSN =  AuxConfigRSN::st_BoardRSN[pCmd->m_btBoradID].uw16_BoardRSN_I2C;            // 注册序号RSN
            uc_BInit = st_Config->m_Global.m_abi_globe.m_BID;
            uc_BMax = st_Config->m_Global.m_abi_globe.m_BID + 1;
        }
        else
        {
            uc_BInit = 0;
            uc_BMax = MAX_BOARD_PER_AUXMCU;
        }

        if((uc_AuxFuncType & 0x01) == 0x01)
        {
            u16ChannelTotal = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_Max_Channels;
        }
        else
        {
            u16ChannelTotal = 8;
        }
        st_Config->m_stMPack.m_stMFirstField.m_wMultPackFlag = 1;
        st_Config->m_u16ChannelCount = u16ChannelTotal;
        //     st_Config->m_Global.m_u16AuxChannelCount = u16ChannelTotal;
        UWord32 uw32_Length = sizeof(MP_REPORT_GET_AUX_CHANNEL_BOARD) + CMD_HOLDHAND_LENGTH; // 多包总字节数  // 计算多少个包时已加入包头
        uw32_Length -= ((MAX_AUXCH_PER_AUXMCU - st_Config->m_u16ChannelCount) * (sizeof(St_AUXChannelCFG))); // 多包总字节数减去空字节数，仅发实际使用字节数。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = (uw32_Length / MAX_PACKLIMIT_SIZE); // 没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = ((uw32_Length % MAX_PACKLIMIT_SIZE) > 0) ? st_Config->m_stMPack.m_stMFirstField.m_wPackTotal + 1 : st_Config->m_stMPack.m_stMFirstField.m_wPackTotal; // 有余数要多一包，没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength = uw32_Length;
        st_Config->m_stMPack.m_stMFirstField.m_dwPackLength = uw32_Length > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : uw32_Length;


        UChar8 uc_TypeCHTotal = 0;
        UChar8 uc_Return = 0;
        uc_jdx = 0;
        for(UChar8 uc_idx = uc_BInit; uc_idx < uc_BMax; uc_idx++) // 类型数组通道数列表
        {
            //uc_TypeCHTotal = Aux_DataManager::m_AuxUnitCFG.m_u8AuxChCountPerAuxType[uc_idx];
            uc_TypeCHTotal = Aux_DataManager::m_AllAuxBoards[uc_idx].m_BoardInfo.m_abi_globe.m_Max_Channels;
            if(uc_TypeCHTotal == 0)
            {
                uc_jdx += 2;   // 按每板16通道，共8块板，每块板补足16通道。 相当于占用两个字节，16位16个通道。
                if(uc_jdx >= 16)
                    break;

                continue;
            }

            unsigned char uc_tempint;
            uc_tempint = uc_TypeCHTotal / 8;  // 先做整数次扫描。
            for(UChar8 uc_udx = 0; uc_udx < uc_tempint; uc_udx++)
            {
                for(UChar8 uc_kdx = 0; uc_kdx < 8; uc_kdx++)
                {
                    st_Config->m_ucChannelIdx[uc_jdx] |= (1 << uc_kdx);
                }

                if(++uc_jdx >= 16)  // 保护处理。
                {
                    uc_Return = 1;
                    break;  // 多重循环时无法退出，为保证退出多重循环。
                }
            }
            if(uc_Return)
                break; // 多重循环时无法退出，为保证退出多重循环。
            unsigned char uc_tempremain = uc_TypeCHTotal % 8;  // 再做余数次扫描。
            for(UChar8 uc_udx = 0; uc_udx < uc_tempremain; uc_udx++)
            {
                st_Config->m_ucChannelIdx[uc_jdx] |= (1 << uc_udx);
            }
            if(uc_tempremain > 0)
            {
                uc_tempint += 1; // 有余数相当多一次整数。
                if(++uc_jdx >= 16)   // 保护处理。
                    break;
            }

            /*uc_tempremain = MAX_AUXCH_PER_BOARD/8;  // 按每板16通道，共8块板，每块板补足16通道。
            uc_tempint = uc_tempint % uc_tempremain;  // 同种类型多于16个时，相当使用下一板。

            for(UChar8 uc_udx = 0;uc_udx < uc_tempint;uc_udx++)
            {
            if(++uc_jdx >= 16)  // 跳表处理和保护处理。
            {
            uc_Return = 1;
            break;  // 多重循环时无法退出，为保证退出多重循环。
            }
            }
            if(uc_Return)
            break; // 多重循环时无法退出，为保证退出多重循环。*/
        }

        unsigned char uc_Bidx = 0;
        //     unsigned char uc_Chidx = 0;
        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {
            u16ChannelTotal = u16ChannelTotal;
            uc_Bidx = pCmd->m_btBoradID;
        }
        else
        {
            u16ChannelTotal = MAX_AUXCH_PER_AUXMCU;
        }
        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {
            /* unsigned char uc_BChCount = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_globe.m_Total_Channels;
            for(UChar8 uc_idx = 0,uc_jdx = 0; uc_idx < u16ChannelTotal; uc_idx++)
            {
            if(MYBITTRUE(st_Config->m_ucChannelIdx,uc_idx))
            {
            if(uc_Chidx >= uc_BChCount)
            {
            do{
            uc_Bidx ++;
            if(uc_Bidx >= MAX_BOARD_PER_AUXMCU)
            {
            uc_Bidx = MAX_BOARD_PER_AUXMCU - 1;
            break;
            }
            }while(Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_globe.m_Total_Channels == 0);
            uc_Chidx = 0;
            uc_BChCount = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_globe.m_Total_Channels;
            }
            st_Config->m_Channel[uc_jdx].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_jdx].m_Populated;
            st_Config->m_Channel[uc_jdx].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_jdx].m_AuxType;
            st_Config->m_Channel[uc_jdx].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_jdx].m_AuxSubType;
            st_Config->m_Channel[uc_jdx].m_ChannelNumber = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
            // uw16_PhysicalCh = Aux_DataManager::Convert_PhysicalChID(uc_Bidx, uc_Chidx); // 同一块板中面板对应通道号处理
            st_Config->m_Channel[uc_jdx].m_AuxSensorCoreInfo=Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_jdx].m_AuxSensorCoreInfo;
            // st_Config->m_Channel[uc_jdx].m_AuxChInfo =  Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_AuxSensor_ChInfo[uw16_PhysicalCh];  // 更新通道信息
            ++uc_Chidx;
            if(++uc_jdx >= st_Config->m_u16ChannelCount) // 保护功能，用完数据退出。
            break;
            }
            }*/
            // st_Config->m_Global.m_abi_globe.m_AuxOtherType=1;  //add by qjm 20150707. 与改版的Listenfornet相匹配
            uc_Bidx = pCmd->m_btBoradID;
            if((uc_AuxFuncType & 0x01) == 0x01)
            {
                if(pCmd->m_btCtrl_Type == 2)
                    st_Config->m_Global.m_abi_globe.m_AuxFuncType = 0x01;
                u16ChannelTotal = Aux_DataManager::m_AllAuxBoards[pCmd->m_btBoradID].m_BoardInfo.m_abi_globe.m_Max_Channels;
                for(unsigned char uc_idx = 0; uc_idx < u16ChannelTotal; uc_idx++)
                {
                    st_Config->m_Channel[uc_idx].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_idx].m_Populated;
                    st_Config->m_Channel[uc_idx].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_idx].m_AuxType;
                    st_Config->m_Channel[uc_idx].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_idx].m_AuxSubType;
                    st_Config->m_Channel[uc_idx].m_ChannelNumber = uc_idx;//Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
                    st_Config->m_Channel[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo;
                    st_Config->m_Channel[uc_idx].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_chn[uc_idx].m_avg_num;
                }
            }
            else if((uc_AuxFuncType & 0x10) == 0x10)
            {
                if(pCmd->m_btCtrl_Type == 2)
                    st_Config->m_Global.m_abi_globe.m_AuxFuncType = 0x10;
                u16ChannelTotal  = 8;
                for(unsigned char uc_idx = 0; uc_idx < u16ChannelTotal; uc_idx++)
                {
                    st_Config->m_Channel[uc_idx].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_Populated;
                    st_Config->m_Channel[uc_idx].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_AuxType;
                    st_Config->m_Channel[uc_idx].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_AuxSubType;
                    st_Config->m_Channel[uc_idx].m_ChannelNumber = uc_idx;//Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
                    st_Config->m_Channel[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo;
                    st_Config->m_Channel[uc_idx].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_avg_num;
                }
            }
            else if((uc_AuxFuncType&0x20)==0x20)  //修复LISTENFORNET读取不正确的BUG add by zc 08.10.2018 for MTC
            {
                if(pCmd->m_btCtrl_Type==2)
                    st_Config->m_Global.m_abi_globe.m_AuxFuncType= 0x20;
                u16ChannelTotal  = 8;
                for(unsigned char uc_idx=0; uc_idx<u16ChannelTotal; uc_idx++)
                {
                    st_Config->m_Channel[uc_idx].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_Populated;
                    st_Config->m_Channel[uc_idx].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_AuxType;
                    st_Config->m_Channel[uc_idx].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_AuxSubType;
                    st_Config->m_Channel[uc_idx].m_ChannelNumber = uc_idx;//Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
                    st_Config->m_Channel[uc_idx].m_ChnCoreInfo.m_I2CChnCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_ChnCoreInfo.m_I2CChnCoreInfo;
                    st_Config->m_Channel[uc_idx].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_avg_num;
                }
            }
            else if((uc_AuxFuncType & 0x40) == 0x40)
            {
                if(pCmd->m_btCtrl_Type == 2)
                    st_Config->m_Global.m_abi_globe.m_AuxFuncType = 0x40;
                u16ChannelTotal  = 8;
                for(unsigned char uc_idx = 0; uc_idx < u16ChannelTotal; uc_idx++)
                {
                    st_Config->m_Channel[uc_idx].m_valid = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_DAC_chn[uc_idx].m_Populated;
                    st_Config->m_Channel[uc_idx].m_AuxType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_DAC_chn[uc_idx].m_AuxType;
                    st_Config->m_Channel[uc_idx].m_AuxSubType = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_DAC_chn[uc_idx].m_AuxSubType;
                    st_Config->m_Channel[uc_idx].m_ChannelNumber = uc_idx;//Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
                    st_Config->m_Channel[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_DAC_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo;
// 				st_Config->m_Channel[uc_idx].m_AVG_num = Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_BoardInfo.m_abi_uart_chn[uc_idx].m_avg_num;
                }
            }
        }
    }
    if( m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    MPReportData(0, SEND_GET_CHANNELBOARD_MG);
}
#else
////////
// MPGetChannelBoard(MP_SET_CHANNEL_BOARD *pCmd) : 网络命令获取板的通道信息处理函数
// Input: pCmd 网络获取板的通道信息命令结构指针;
// Return: N/A
///////
void EthernetParse::MPGetChannelBoard(MP_GET_CHANNEL_BOARD *pCmd)
{
    //UChar8 uc_jdx = 0;
    MP_REPORT_GET_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_CHANNEL_BOARD *) &m_btPackDataBuffer[16];

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

    if(pCmd->m_u16ChannelCount == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
    {
        for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
            pCmd->m_ucChannelIdx[uc_idx] = 0XFF; // 缓冲数组，列表用
    }

    //if(pCmd->m_u16ChannelCount > MAXCHANNELNO)
    if(pCmd->m_u16ChannelCount > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
    {
        //pCmd->m_u16ChannelCount = MAXCHANNELNO;
        pCmd->m_u16ChannelCount = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
        m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
        m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAXCHANNELNO; // 通道号最大值保护
    }

    st_Config->m_stMPack.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;
    st_Config->m_stMPack.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;
    st_Config->m_stMPack.m_stMFirstField.m_wMultPackFlag = 1;
    st_Config->m_u16ChannelCount = pCmd->m_u16ChannelCount;
    UWord32 uw32_Length = sizeof(MP_REPORT_GET_CHANNEL_BOARD) + CMD_HOLDHAND_LENGTH; // 多包总字节数  // 计算多少个包时已加入包头
    //uw32_Length -= ((16 - st_Config->m_u16ChannelCount) * (sizeof(St_ChannelCFG) + sizeof(St_IV_ChannelPID))); // 多包总字节数减去空字节数，仅发实际使用字节数。
    st_Config->m_u16ChannelCount = 16;     // 20161220
    pCmd->m_ucChannelIdx[0] = 0XFF; // 缓冲数组，列表用

    st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = (uw32_Length / MAX_PACKLIMIT_SIZE); // 没余数原包。
    st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = ((uw32_Length % MAX_PACKLIMIT_SIZE) > 0) ? st_Config->m_stMPack.m_stMFirstField.m_wPackTotal + 1 : st_Config->m_stMPack.m_stMFirstField.m_wPackTotal; // 有余数要多一包，没余数原包。
    st_Config->m_stMPack.m_stMFirstField.m_wPackNo = 0;
    st_Config->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;
    st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength = uw32_Length;
    st_Config->m_stMPack.m_stMFirstField.m_dwPackLength = uw32_Length > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : uw32_Length;
    for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
    {
        st_Config->m_ucChannelIdx[uc_idx] = pCmd->m_ucChannelIdx[uc_idx];  // 缓冲数组，列表用
    }

    st_Config->m_Global = ChannelInfo_Manager::BoardCFG.m_Global;  // 更新板类信息

    for(UChar8 uc_idx = 0, uc_jdx = 0; uc_idx < 16; uc_idx++)
    {
        if(MYBITTRUE(st_Config->m_ucChannelIdx, uc_idx))
        {
            //st_Config->m_Channel[uc_jdx] = ChannelInfo_Manager::BoardCFG.m_Channel[uc_idx];
            ChannelInfo_Manager::GetChanConfig(uc_jdx, &st_Config->m_Channel[uc_jdx]);   //170320,add by hepeiqing
            //st_Config->m_stChannelPID[uc_jdx] = ChannelInfo_Manager::ChannelPID[uc_idx];    // 更新通道信息
            ChannelInfo_Manager::GetChanPID(uc_idx, &st_Config->m_stChannelPID[uc_jdx]); //170320,add by hepeiqing
            if(++uc_jdx >= st_Config->m_u16ChannelCount) // 保护功能，用完数据退出。
                break;
        }
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    MPReportData(0, SEND_GET_CHANNELBOARD_MG);
}


void EthernetParse::MPGetChannelBoard_BuiltIn(MP_GET_CHANNEL_BOARD *pCmd)
{

// 	UChar8    uc_jdx = 0;
// 	UChar8    uc_BInit = 0;
// 	UChar8    uc_BMax = MAX_BOARD_PER_AUXMCU;
// 	UChar8    uc_AuxFuncType=0;
    UWord16   u16ChannelTotal = 0;
    //     UWord16   uw16_PhysicalCh;
    //     UWord16   uw16_AuxChennal = 0;
    MP_REPORT_GET_AUX_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_AUX_CHANNEL_BOARD *) &m_btPackDataBuffer[16];

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    if(pCmd->m_btBoradID == 0xBB) //如果命令来自Mitspro//1807131
    {
        unsigned char uc_ok = CConfig::Load_I2C_Config();
        if(uc_ok == 0)
        {
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD;  // 配置错误代码
            MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
            return;
        }



//         unsigned char Aux_Type = 0;
        unsigned char m_ucScanChannelIdx[16];
        u16ChannelTotal = pCmd->m_u16ChannelCount;
//         Aux_Type = pCmd->m_btCtrl_Type;

        st_Config->m_stMPack.m_stMFirstField.m_wMultPackFlag = 1;
        st_Config->m_u16ChannelCount = u16ChannelTotal;
        UWord32 uw32_Length = sizeof(MP_REPORT_GET_AUX_CHANNEL_BOARD) + CMD_HOLDHAND_LENGTH; // 多包总字节数  // 计算多少个包时已加入包头
        uw32_Length -= ((MAX_AUXCH_PER_AUXMCU - st_Config->m_u16ChannelCount) * (sizeof(St_AUXChannelCFG))); // 多包总字节数减去空字节数，仅发实际使用字节数。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = (uw32_Length / MAX_PACKLIMIT_SIZE); // 没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = ((uw32_Length % MAX_PACKLIMIT_SIZE) > 0) ? st_Config->m_stMPack.m_stMFirstField.m_wPackTotal + 1 : st_Config->m_stMPack.m_stMFirstField.m_wPackTotal; // 有余数要多一包，没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength = uw32_Length;
        st_Config->m_stMPack.m_stMFirstField.m_dwPackLength = uw32_Length > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : uw32_Length;


        for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) // 缓冲数组，列表用
        {
            m_ucScanChannelIdx[uc_idx] = pCmd->m_ucChannelIdx[uc_idx]; // 缓冲数组，列表用
            st_Config->m_ucChannelIdx[uc_idx] = pCmd->m_ucChannelIdx[uc_idx];
        }

        for(unsigned char uc_ChanIdex = 0; uc_ChanIdex < u16ChannelTotal; uc_ChanIdex++) //通道总数
        {
            unsigned char AuxChannelInUnit;
            unsigned char FindChanSuccess = 0;
            for(UChar8 uc_idx = 0; uc_idx < 16; uc_idx++) //16个unsigned char 值
            {
                for(UChar8 uc_bit = 0; uc_bit < 8; uc_bit++) //每个值8位
                {
                    if(((m_ucScanChannelIdx[uc_idx] >> uc_bit) & 1) == 1)
                    {
                        m_ucScanChannelIdx[uc_idx] &= (~(1 << uc_bit)); //一旦找到把该位清除
                        AuxChannelInUnit = uc_idx * 8 + uc_bit;
                        FindChanSuccess = 1;
                        break;
                    }
                }
                if(FindChanSuccess == 1)
                    break;
            }
            if(FindChanSuccess == 0)//没找到直接返回
                return;

            //处理通道信息
// 				 st_Config->m_btAuxBuiltInFlag = 0xBB;//用于上位机区分标志。
            st_Config->m_Channel[uc_ChanIdex].m_valid = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[AuxChannelInUnit].m_Populated;
            st_Config->m_Channel[uc_ChanIdex].m_AuxType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[AuxChannelInUnit].m_AuxType;
            st_Config->m_Channel[uc_ChanIdex].m_AuxSubType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[AuxChannelInUnit].m_AuxSubType;
            st_Config->m_Channel[uc_ChanIdex].m_ChannelNumber = AuxChannelInUnit;//在该辅助类型在该unit下的通道号。
            st_Config->m_Channel[uc_ChanIdex].m_ChnCoreInfo.m_AuxSensorCoreInfo = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[AuxChannelInUnit].m_ChnCoreInfo.m_AuxSensorCoreInfo;
            st_Config->m_Channel[uc_ChanIdex].m_AVG_num = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[AuxChannelInUnit].m_avg_num;

        }

    }
    else//命令来自listenfornet
    {
        unsigned char uc_ok = CConfig::Load_I2C_Config();
        if(uc_ok == 0)
        {
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD;  // 配置错误代码
            MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
            return;
        }
        u16ChannelTotal = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels;
        if(u16ChannelTotal > MAX_AUXCH_PER_AUXMCU)
        {
            u16ChannelTotal = MAX_AUXCH_PER_AUXMCU;
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
            m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAX_AUXCH_PER_AUXMCU; // 通道号最大值保护
        }
        if(pCmd->m_btBoradID >= 0XFF) //要读的板地址
        {
            pCmd->m_btCtrl_Type =  CONTROLTYPE_USE_ALL;
            pCmd->m_btBoradID = 0; // 读取单板单通道保护
        }
        else
        {
            if(pCmd->m_btBoradID >= 8)
                pCmd->m_btBoradID = 0; // 读取单板保护
        }
        st_Config->m_Global.m_btCtrl_Type = pCmd->m_btCtrl_Type;//上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息 （预留开发 3：单独下载Map表 4：单独下载校正值）
// 	uc_AuxFuncType=pCmd->m_btAuxFuncType;

        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {
            st_Config->m_Global.m_abi_globe.m_BID =  pCmd->m_btBoradID;
            st_Config->m_Global.m_abi_globe.m_BoardType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BoardType;
            if((( st_Config->m_Global.m_abi_globe.m_BoardType == NUM_OF_AUX_BOARD_NULL) && ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD))
                    || (!BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_valid))
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }

            UWord16 u16ChannelValid;
            u16ChannelValid = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels;

            if(( u16ChannelValid == 0) && ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_AUX_READ_BOARD_CHANNEL;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }
            // memcpy(&st_Config->m_Global.m_abi_globe.m_BID,&Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_abi_globe.m_BID,10);  // 板P/N Ver:
            st_Config->m_Global.m_abi_globe = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe;
            // st_Config->m_Global.m_u8ChCountPerType[st_Config->m_Global.m_BoardType.m_AuxType] = Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_BoardInfo.m_ChannelCount;
            st_Config->m_Global.m_assemble.m_ConsistentOrder = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_ConsistentOrder;      // 面板通道号是否与电气特性排序的通道序号一致
            st_Config->m_Global.m_assemble.m_SlaveGoesFirst =  BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_SlaveGoesFirst;      // 有无副板时，面板通道号是主板在前还是副板在前
            st_Config->m_Global.m_assemble.m_WithSlave =  BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_WithSlave;      // 有无副板
            //st_Config->m_Global.m_BoardType.m_u16Board_RSN =  Aux_DataManager::m_AllAuxBoards[pCmd->m_btReserved].m_BoardInfo.m_BType.m_u16Board_RSN;            // 注册序号RSN
            st_Config->m_Global.m_u16Board_RSN = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_RSN;            // 注册序号RSN
// 		uc_BInit = st_Config->m_Global.m_abi_globe.m_BID;
// 		uc_BMax = st_Config->m_Global.m_abi_globe.m_BID + 1;
        }
        u16ChannelTotal = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Max_Channels;
        st_Config->m_stMPack.m_stMFirstField.m_wMultPackFlag = 1;
        st_Config->m_u16ChannelCount = u16ChannelTotal;
        UWord32 uw32_Length = sizeof(MP_REPORT_GET_AUX_CHANNEL_BOARD) + CMD_HOLDHAND_LENGTH; // 多包总字节数  // 计算多少个包时已加入包头
        uw32_Length -= ((MAX_AUXCH_PER_AUXMCU - st_Config->m_u16ChannelCount) * (sizeof(St_AUXChannelCFG))); // 多包总字节数减去空字节数，仅发实际使用字节数。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = (uw32_Length / MAX_PACKLIMIT_SIZE); // 没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackTotal = ((uw32_Length % MAX_PACKLIMIT_SIZE) > 0) ? st_Config->m_stMPack.m_stMFirstField.m_wPackTotal + 1 : st_Config->m_stMPack.m_stMFirstField.m_wPackTotal; // 有余数要多一包，没余数原包。
        st_Config->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;
        st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength = uw32_Length;
        st_Config->m_stMPack.m_stMFirstField.m_dwPackLength = uw32_Length > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : uw32_Length;
//         unsigned char uc_Bidx = 0;
        //     unsigned char uc_Chidx = 0;
        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {
            u16ChannelTotal = u16ChannelTotal;
//             uc_Bidx = pCmd->m_btBoradID;
        }
        else
        {
            u16ChannelTotal = MAX_AUXCH_PER_AUXMCU;
        }
        if(( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_BOARD) || ( pCmd->m_btCtrl_Type == CONTROLTYPE_USE_CHANNEL))
        {

            u16ChannelTotal =  BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Max_Channels;
            for(unsigned char uc_idx = 0; uc_idx < u16ChannelTotal; uc_idx++)
            {
                st_Config->m_Channel[uc_idx].m_valid = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_Populated;
                st_Config->m_Channel[uc_idx].m_AuxType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_AuxType;
                st_Config->m_Channel[uc_idx].m_AuxSubType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_AuxSubType;
                st_Config->m_Channel[uc_idx].m_ChannelNumber = uc_idx;//Aux_DataManager::m_AllAuxBoards[uc_Bidx].m_Start_InUnit + uc_Chidx;
                st_Config->m_Channel[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo;
                st_Config->m_Channel[uc_idx].m_AVG_num = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_avg_num;
            }

        }
    }
    if( m_ResultFeedback.m_btError_Code == MP_CONFIRM_ERROR_UNKNOWN)
    {
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    MPReportData(0, SEND_GET_CHANNELBOARD_MG);
}

#endif

void EthernetParse::MPGetIVBoard(MP_GET_IV_BOARDTYPE *pCmd)
{
// 	UChar8 uc_jdx = 0;
    MP_REPORT_GET_IV_BOARDTYPE * st_Config = (MP_REPORT_GET_IV_BOARDTYPE *) &m_btPackDataBuffer[16];

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

#if USE_AS_AUXMCU==0
    if(BuiltIn_DataManager::m_Sample_Flag)
    {
        BuiltIn_DataManager::m_WriteFRAM_Flag = true;
        while(BuiltIn_DataManager::m_Sample_Flag);
    }
    CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG));
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        BuiltIn_DataManager::m_Sample_Flag = true;
        BuiltIn_DataManager::m_WriteFRAM_Flag = false;
    }
    ChannelInfo_Manager::InitVirtualCurrentRange();//170320,add by hepeiqing
    st_Config->m_st_BoadType = ChannelInfo_Manager::m_st_BoadType;
    st_Config->m_st_BoadType.b_IsMapIRangeVir2Phy = 1; //170428,zyx 防止ListenForNet读取旧版下位机时产生兼容问题
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    MPReportData(0, SEND_GET_IV_BOARDTYPE_MG);
    CommonFunc::GetTimeTicksUpdate( &SetIVBoardTypeTime);
    CConfig::IV_BoardType_OldTransformToNew();
#else
    m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif

}
#if USE_AS_AUXMCU==1
void EthernetParse::MPAuxACIMStartedACKEvent(MP_SENT_ACIMTEST_START *pCmd)
{
    if(Aux_DDS_ACIM_ACR_408888_Base::isACIM) // ACIM board
    {
        Aux_ACIMTest::PointPerDecade = pCmd->m_Point_Decade;
        float m_FREQINIT = pCmd->m_FREQINIT;
        float m_FREQFINAL = pCmd->m_FREQFINAL;
        Aux_ACIMTest::ACIM_5160_Value =pCmd->m_IACValue;
        if(m_FREQINIT<=m_FREQFINAL)
        {
            Aux_ACIMTest::f_FrequencyL = m_FREQINIT;
            Aux_ACIMTest::f_FrequencyH = m_FREQFINAL;
        }
        else
        {
            Aux_ACIMTest::f_FrequencyL = m_FREQFINAL;
            Aux_ACIMTest::f_FrequencyH = m_FREQINIT;
        }
    }
    else	// ACR board
    {
        float m_IACValue = pCmd->m_IACValue;
        if(Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag==0x01)
            Aux_ACIMTest::ACIM_5160_Value =m_IACValue;
        else
            Aux_ACIMTest::ACIM_SineAmplitude_value=m_IACValue;
    }

    Aux_ACIMTest::b_ACIM_StartTest_Flag=true;
    Aux_ACIMTest::uc_ACIM_TEST_Source = 0xBB;
    SendFeedBack(MPCS_ACK,0);
}

void EthernetParse::MPAuxACIMStopACKEvent(MP_SENT_ACIMTEST_STOP *pCmd)
{

    // float m_IACValue = pCmd->m_IACValue;
    WORD  IV_CHN_ID =  pCmd->m_ChannelID;
    Aux_ACIMTest::b_ACIM_StartTest_Flag=false;
    Aux_ACIMTest::uc_ACIM_TEST_Source = 0xBB;
    if(Aux_DDS_ACIM_ACR_408888_Base::isACIM)
        Aux_ACIMTest::Set_ACIM_Status(ACIM_STOP_TEST);
    else
        Aux_ACIMTest::Set_ACIM_Status(ACR_STOP_TEST);
    memcpy(&m_CmdTxBuffer_SynchroChannel[1], &IV_CHN_ID, 2);
    SendFeedBack(MPCS_ACK,4);
}


#endif
////////
// MPCMD_Feedback(MP_CONFIRM_FEEDBACK *pFeedback)
//      : 网络命令回应函数
// Input: pFeedback 网络命令回应结构指针;
// Return: N/A
///////
void EthernetParse::MPCMD_Feedback(MP_CONFIRM_FEEDBACK *pFeedback)
{
    //UWord16 uw16_Chan;
    //UWord16 uw16_Total;
    //for(UChar8 uc_i = 0; uc_i < 100;uc_i ++)
    //    m_CmdTxBufSynchro[uc_i] = CODE_EMPTY;

    // pFeedback->m_btNone_111 =  1;  // pFeedback->m_btUnitId = Interface_LED::uc_dsp_id;
    // pFeedback->m_btNone_112 =  0X9A ; // pFeedback->m_btCmd = MPC_MCU2PC_CMD;

    //uw16_Chan = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
    //uw16_Total = m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber;

    TotalNumberCountProcess(& m_ResultFeedback.m_stMSecondField.m_wChannelNumber,
                            & m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	pFeedback->m_dwCopyCmd = m_ResultFeedback.m_dwCopyCmd; // 传递备份信息
// 	pFeedback->m_dwCopyCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;

    CmdGetToFeedback(&pFeedback->m_dwCopyCmd, &pFeedback->m_dwCopyCmd_Extend,
                     &m_ResultFeedback.m_stMSecondField.m_wChannelNumber, &m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber);

    pFeedback->m_stMSecondField = m_ResultFeedback.m_stMSecondField;
    //pFeedback->m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    pFeedback->m_btResult = m_ResultFeedback.m_btResult;
    pFeedback->m_btMultPack_OK = m_ResultFeedback.m_btMultPack_OK;
    pFeedback->m_btError_Code = m_ResultFeedback.m_btError_Code;

    switch(m_ResultFeedback.m_dwCopyCmd)
    {
    //  设置通道配置类
    case CMD_SET_CHANNEL_BOARD:
    case CMD_SET_CHANNEL_MAP:
    case CMD_SET_META_VARIABLE:
    case CMD_SET_CHANNEL_PID:
    case CMD_SET_CHANNEL_CALIBRATE:

    //  独立运行类(第三方命令)
    case CMD_INDEPENDENT_ACTIONSWITCH:
    case CMD_INDEPENDENT_ACTION:
    case CMD_INDEPENDENT_EMERGENCYSTOP:
//    case CMD_INDEPENDENT_POWERON:
//    case CMD_INDEPENDENT_RESETCAN:

    //  Schedule设置配置类
    case CMD_SCHEDULE_SET_SETTING:

    //  Schedule准备数据类
    case CMD_SCHEDULE_DOWNLOAD:
    case CMD_STEP_DOWNLOAD:
    case CMD_SIMULATION_DOWNLOAD:
    case CMD_PULSE_DOWNLOAD:

    //  Schedule运行类
    //case CMD_SCHEDULE_START:          //  20130904  yang    //  不同于普通单包或多包结果回复
    case CMD_SCHEDULE_STOP:
    case CMD_SCHEDULE_JUMP:
    case CMD_SCHEDULE_PAUSE:
    //case CMD_SCHEDULE_RESUME:
    case CMD_GET_CHANNEL_BOARD:
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    case CMD_SCHEDULE_GET_STATUS:
#endif
#if (USE_AS_AUXMCU == 0 )
    case CMD_SET_CAN_CONFIG:
    case CMD_SET_CAN_MAP:
    case CMD_SET_CAN_MESSAGE:
    case CMD_ASSIGN_CAN_CONFIG:
    case CMD_ASSIGN_SMB_CONFIG:
#endif
        pFeedback->m_dwCmd = m_ResultFeedback.m_dwCopyCmd + 0x50000000;
        break;
    default:
        break;
    }

    SendFeedBack(MPCS_ACK, sizeof(MP_CONFIRM_FEEDBACK));
}


////////
// MPSetChannelPID(MP_SET_CHANNEL_PID *pCmd) : 网络命令设置PID参数处理函数
// Input: pCmd 网络设置PID参数命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetChannelPID(MP_SINGLE_PACKDATA *pCmd)
{
#if  USE_AS_AUXMCU == 0
    UChar8 uc_jdx = 0;
    unsigned char uc_Chan_Idx[MAXCHANNELNO];
    unsigned char uc_DownLoad_Success = 0;
    MP_UnitPIDDownload * st_PID;

// 	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
// 	TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
// 		& pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;   // 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

//    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_FAIL);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{
// 		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
// 		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
// 	}
// 	else
// 	{
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
// 		{
// 			// 判断是否为同一个多包的内容。
// 			m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 			m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 			MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 			return;
// 		}
// 		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
// 	}

// 	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
// 	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

// 	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
// 	{
// 		// 判断是否为同一个多包的内容接收完成。
// 		uc_DownLoad_Success = 1;
// 		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	}

// 	// 存包中的SCH数据
// 	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)
    {
        st_PID = (MP_UnitPIDDownload *) &m_btPackDataBuffer[0];

        for(UChar8 uc_idx = 0; uc_idx < MAXCHANNELNO; uc_idx++) // 缓冲数组，列表用
            //for(UChar8 uc_idx = 0;uc_idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_idx++) // 缓冲数组，列表用
            uc_Chan_Idx[uc_idx] = st_PID->m_ucChannelIdx[uc_idx];  // 缓冲数组，列表用

        if(st_PID->m_u16ChannelCount == 0xFFFF)  // 如果为全部通道信息一致，则复制信息到所有通道中。
        {
            for(UChar8 uc_idx = 0; uc_idx < MAXCHANNELNO; uc_idx++) // 缓冲数组，列表用
                //for(UChar8 uc_idx = 0;uc_idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_idx++) // 缓冲数组，列表用
                uc_Chan_Idx[uc_idx] = 0XFF; // 缓冲数组，列表用
        }

        if(st_PID->m_u16ChannelCount > MAXCHANNELNO)
            //if(st_PID->m_u16ChannelCount > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        {
            m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_OVER_MAXCH;  // 配置错误代码
            m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = MAXCHANNELNO; // 通道号最大值保护
            //m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
        }

        uc_jdx = 0;
        for(UChar8 uc_idx = 0; uc_idx < MAXCHANNELNO; uc_idx++)
            //for(UChar8 uc_idx = 0;uc_idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_idx++)
        {
            if(MYBITTRUE(uc_Chan_Idx, uc_idx))
            {
                //ChannelInfo_Manager::ChannelPID[uc_idx] = st_PID->m_stChannelPID[uc_jdx];    // 更新通道信息
                ChannelInfo_Manager::SetChanPID(uc_idx, &st_PID->m_stChannelPID[uc_jdx]);  //170320,add by hepeiqing
                for(unsigned char RngIdx = 0; RngIdx < NUM_OF_IV_RANGE_I; RngIdx++)
                {
                    St_PID * myPID = &ChannelInfo_Manager::ChannelPID[uc_idx].m_VoltagePID[RngIdx];
                    if( (myPID->m_fKp <= 0 && myPID->m_fKi <= 0 && myPID->m_fKd <= 0)
                            || (IsFiniteNumber(myPID->m_fKp) == 0)
                            || (IsFiniteNumber(myPID->m_fKi) == 0)
                            || (IsFiniteNumber(myPID->m_fKd) == 0)) //非数值或无穷大
                    {
                        myPID->m_fKp = 0;
                        myPID->m_fKi = 0.01;
                        myPID->m_fKd = 0;
                    }
                }
                uc_jdx ++;
                if(uc_jdx >= st_PID->m_u16ChannelCount) // 保护功能，用完数据退出。
                    break;
            }
        }

        ChannelInfo_Manager::SavePidPara(st_PID->m_u16ChannelCount, &st_PID->m_ucChannelIdx[0]);
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//     if(uc_DownLoad_Success >= 0)
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}


////////
// MPSetChannelCalibrate(MP_SET_CHANNEL_CALIBRATE *pCmd) : 网络命令设置通道校正参数处理函数
// Input: pCmd 网络设置通道校正参数命令结构指针;
// Return: N/A
///////
void EthernetParse::MPSetChannelCalibrate(MP_SET_CHANNEL_CALIBRATE *pCmd)
{
#if  USE_AS_AUXMCU == 1   //AUX
    UWord16 uw16_Chan;
    UWord16 uw16_Total;
    unsigned char uc_AuxType = MAXAUXTYPE;//NUM_OF_AUX_CHAN_TYPE_NULL;
    unsigned char uc_Bid = 0;
    //   unsigned char uc_BChid = 0;
    //   unsigned char uc_I2CSuccessCHFlag = 0;
    unsigned char uc_ChIdx = 0;
    unsigned char m_MessageSource;//00:Mits7, AA:Listen For Net.niuyasong2016.8.26
    //   UWord16  uw16_PhysicalCh;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan = pCmd->m_wChannelNumber;
    uc_ChIdx = pCmd->m_wChannelNumber;
    m_MessageSource = pCmd->m_btAuxIdx ;//00:Mits7, AA:Listen For Net
    //     if((pCmd->m_btControlType | CALIBRATE_CONTROLTYPE_2NDV) == CALIBRATE_CONTROLTYPE_2NDV)
    //     {
    //        m_ResultFeedback.m_btNone_11[0] = AUX_VOLTAGE;  // Auto convert aux type
    //     }
    //     else if((pCmd->m_btControlType | CALIBRATE_CONTROLTYPE_TEMPERATURE) == CALIBRATE_CONTROLTYPE_TEMPERATURE)
    //     {
    //         m_ResultFeedback.m_btNone_11[0] = AUX_TEMPERATURE;   // Auto convert aux type
    //      }
    //     else
    //     {
    //         m_ResultFeedback.m_btNone_11[0] = AUX_ExCharge;
    //     }

    if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_2NDV)
    {
        m_ResultFeedback.m_btNone_11[0] = AUX_VOLTAGE;  // Auto convert aux type
        uc_AuxType = AUX_VOLTAGE;
    }
    else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_TEMPERATURE)
    {
        m_ResultFeedback.m_btNone_11[0] = AUX_TEMPERATURE;   // Auto convert aux type
        uc_AuxType = AUX_TEMPERATURE;
//         AUX_SampleProcess::b_AuxTempNeedFilterFlag =0;
        AUX_SampleProcess::m_AuxTempUpdataTime =CommonFunc::GetSystemTime();
    }
    else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_EXTCHARGE)
    {
        m_ResultFeedback.m_btNone_11[0] = AUX_EXTERNALCHARGE;
        uc_AuxType = AUX_EXTERNALCHARGE;
    }
    else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_PRESSURE)
    {
        m_ResultFeedback.m_btNone_11[0] = CALIBRATE_CONTROLTYPE_PRESSURE;
        uc_AuxType = AUX_PRESSURE;
    }
    else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_BUILDIN_VOLTAGE)
    {
        m_ResultFeedback.m_btNone_11[0] = CALIBRATE_CONTROLTYPE_BUILDIN_VOLTAGE;//借用一下，与listen for net 对应niuyasong
        uc_AuxType = AUX_AO;
    }
    else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_CURRENT)
    {
        m_ResultFeedback.m_btNone_11[0] = CALIBRATE_CONTROLTYPE_CURRENT;
        uc_AuxType = AUX_CURRENT;
    }
    unsigned char uc_data_group_idx, uc_chn;

    if(uc_AuxType < MAXAUXTYPE)
    {
        unsigned char uc_GetIdx = 0;
        for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
        {   // This is for AuxChannel
            if(m_MessageSource == 0xAA) //Listen for net calibration
            {
                uc_Bid = pCmd->m_btRange;
                if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
                    uc_Bid = 0;
                uc_data_group_idx = 	((uc_idx >> 4) + Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start);	 //1807131
                uc_chn = uc_idx & 15;   //uc_idx%16;
            }
            else //mits calibration
            {
                uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_GetIdx + uc_idx].m_data_group_idx;
                uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_GetIdx + uc_idx].m_chn;

                uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];  //需要BID 信息。 得建立一个data_group_idx 与 ＢＩＤ拨码的关系
                if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
                    uc_Bid = 0;
            }
            //       St_AUXChannelCFG ReadCfg_I2C;
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
            unsigned char uc_DeviceAddr =  (MAX_BOARD_PER_AUXMCU - 1 - uc_Bid);

            // 处理内容 // 设置通道量程校正参数信息
            if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_2NDV)    // 用"或"对应枚举，用"与"相当于位操作，不能用与。    yangy 20140414
            {
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain; //1807131
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                //暂时写成这样看采样是否变化
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                uc_GetIdx ++;

            }
            else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_TEMPERATURE)
            {
                // 实施校准电压方式处理办法
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain; //1807131
                pCmd->m_st_ADC5DAC[uc_GetIdx][CALIBRATE_ADC].m_Offset *= 1.0e6f;  // 因上位机计算以伏特为单位结果也以伏，所以要将伏转换成微伏作为单位。
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset; //1807131
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                if((pCmd->m_st_ADC5DAC[uw16_Chan][CALIBRATE_ADC].m_Gain == 1.0f)
                        && (pCmd->m_st_ADC5DAC[uw16_Chan][CALIBRATE_ADC].m_Offset == 0.0f))
                {   // 实施校准仅加外偏移办法
                    //                 Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_BChid].m_AuxTemp.m_AuxThermoCouple.InExtra = pCmd->m_st_ADC5DAC[uc_GetIdx%MAXCHANNELNO][CALIBRATE_DAC].m_Offset;  // 特殊处理InExtra
                    // ReadCfg_I2C.m_AuxChInfo.m_AuxTemp.m_AuxThermoCouple.InExtra = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_BChid].m_AuxTemp.m_AuxThermoCouple.InExtra;
                }
                uc_GetIdx ++;

            }
            else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_EXTCHARGE)    //
            {
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain; //180711 niu
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                //暂时写成这样看采样是否变化
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                uc_GetIdx ++;

            }
            else if (pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_PRESSURE)
            {
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain; //180711 niu
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                //暂时写成这样看采样是否变化
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                uc_GetIdx ++;
            }
            else if (pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_CURRENT)
            {
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain; //180711 niu
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[(((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn)].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                //暂时写成这样看采样是否变化
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                uc_GetIdx ++;
            }
            else if (pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_BUILDIN_VOLTAGE)//AO
            {
                unsigned char Temp_uc_data_group_idx;
                unsigned char Temp_uc_chn;
                if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_AIO_DIO_602178) || \
                        (Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_WMADC_AIO_DIO_602548)||\
                        (Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_200A_412206)||\
                        (Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_2US6K_422108))
                {
                    Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                    Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                    //查找DATA BANK 位置

                    Temp_uc_data_group_idx = 3;
                    Temp_uc_chn = uc_chn;
                }
                else if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_ACR_408888) || \
                        (Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_ACR_603612)||
                        (Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_Sinwave_Current_Source_604700)  //add 604700 board by DKQ
                       )	//1807131
                {
                    Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                    Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_DAC_chn[uc_chn].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                    Temp_uc_data_group_idx = 2;
                    Temp_uc_chn = uc_chn;
                }

                Aux_DataManager::m_AuxDataBank[Temp_uc_data_group_idx][Temp_uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                Aux_DataManager::m_AuxDataBank[Temp_uc_data_group_idx][Temp_uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                uc_data_group_idx = 0;//1807131
                uc_chn = uc_chn + MAX_CH_IN_AUX_UNIT + 8;
                uc_GetIdx ++;
            }
            St_DataMessage  st_Message;
            st_Message.uc_AuxType = 0; // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_BType.m_AuxType;//暂时屏蔽 齐静明20150603
            st_Message.uc_BoardID = uc_Bid;
            st_Message.uc_MsgSource = MESSAGE_SOURCE_PC_ORDER;
            st_Message.uw16_ChannelID = ((uc_data_group_idx - Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start) << 4) + uc_chn; //1807131
            st_Message.uc_MsgCode = MESSAGE_WRITE_CALIB;
            AuxConfigRSN::PushMsg_RSN( MODEL_CHANNEL, &st_Message );
        }
    }
    //     Aux_DataManager::Aux_ObtainGainOffset();  // 对接更新采样校正值

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
#if  USE_AS_AUXMCU == 0   //IV
    UWord16 uw16_Chan;
    UWord16 uw16_Total;
    UChar8 uc_Range;
    unsigned char m_MessageSource;//00:Mits7, AA:Listen For Net.niuyasong2017.11.1
//     unsigned char uc_AuxType = NUM_OF_AUX_CHAN_TYPE_NULL;
    unsigned char uc_data_group_idx, uc_chn;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);


    uw16_Chan = pCmd->m_wChannelNumber;
    uc_Range = pCmd->m_btRange;
    m_MessageSource = pCmd->m_btAuxIdx ;//00:Mits7, AA:Listen For Net
    if(m_MessageSource == 0xAA)
    {
        if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_2NDV)
        {
            m_ResultFeedback.m_btNone_11[0] = AUX_VOLTAGE;  // Auto convert aux type
//             uc_AuxType = AUX_VOLTAGE;
        }
        else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_TEMPERATURE)
        {
            m_ResultFeedback.m_btNone_11[0] = AUX_TEMPERATURE;   // Auto convert aux type
//             uc_AuxType = AUX_TEMPERATURE;
        }
        unsigned char uc_GetIdx = 0;
        for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
        {
            uc_data_group_idx = 	(uc_idx >> 4);
            uc_chn = uc_idx % 16;
            St_AUXChannelCFG  m_Channel_Info;
            memset (&m_Channel_Info, 0, sizeof(St_AUXChannelCFG));
            // 处理内容 // 设置通道量程校正参数信息
            if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_2NDV)    // 用"或"对应枚举，用"与"相当于位操作，不能用与。    yangy 20140414
            {
                BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                //暂时写成这样看采样是否变化
                BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                uc_GetIdx ++;

            }
            else if(pCmd->m_btControlType == CALIBRATE_CONTROLTYPE_TEMPERATURE)
            {
                // 实施校准电压方式处理办法
                BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                pCmd->m_st_ADC5DAC[uc_GetIdx][CALIBRATE_ADC].m_Offset *= 1.0e6f;  // 因上位机计算以伏特为单位结果也以伏，所以要将伏转换成微伏作为单位。
                BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;
                BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Gain;
                BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = pCmd->m_st_ADC5DAC[uc_GetIdx % MAXCHANNELNO][CALIBRATE_ADC].m_Offset;

                if((pCmd->m_st_ADC5DAC[uw16_Chan][CALIBRATE_ADC].m_Gain == 1.0f)
                        && (pCmd->m_st_ADC5DAC[uw16_Chan][CALIBRATE_ADC].m_Offset == 0.0f))
                {   // 实施校准仅加外偏移办法
                    //                 Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_BChid].m_AuxTemp.m_AuxThermoCouple.InExtra = pCmd->m_st_ADC5DAC[uc_GetIdx%MAXCHANNELNO][CALIBRATE_DAC].m_Offset;  // 特殊处理InExtra
                    // ReadCfg_I2C.m_AuxChInfo.m_AuxTemp.m_AuxThermoCouple.InExtra = Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_BChid].m_AuxTemp.m_AuxThermoCouple.InExtra;
                }
                uc_GetIdx ++;

            }

            m_Channel_Info.m_valid = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_Populated;
            m_Channel_Info.m_AuxType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_AuxType;
            m_Channel_Info.m_AuxSubType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_AuxSubType;
            m_Channel_Info.m_AVG_num = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_avg_num;
            m_Channel_Info.m_ChannelNumber = uc_idx;
            m_Channel_Info.m_ChnCoreInfo = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_idx].m_ChnCoreInfo;

            CConfig::WriteBuiltInChannelCfg( 0, uc_idx, &m_Channel_Info);   // 通道信息存入I2C的24LC128
        }
    }
    else
    {
        St_ChannelCFG * p_Chn_CFG = &ChannelInfo_Manager::BoardCFG.m_Channel[uw16_Chan];
        St_RangeCali * p_Para = &(pCmd->m_st_ADC5DAC[uw16_Chan][0]);
        for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
        {   //This is for IVChannel
            if(uc_Total_Flag == 1)
            {
                uw16_Chan = uc_idx; // 多通道处理
                p_Chn_CFG = &ChannelInfo_Manager::BoardCFG.m_Channel[uw16_Chan];
            }

            // 处理内容 // 设置通道量程校正参数信息
            switch(pCmd->m_btControlType)
            {
            case CALIBRATE_CONTROLTYPE_CURRENT:
                unsigned char PhysicalRng;
// 		  if(Step_Manager_task::Unipolar.BoardFlag)
// 				PhysicalRng = uc_Range;
// 			else
                PhysicalRng = ChannelInfo_Manager::GetPhysicalCurrentRange(uc_Range);  //170320,add by hepeiqing
                p_Chn_CFG->m_IRange[PhysicalRng].m_Adc = p_Para[CALIBRATE_ADC];
                p_Chn_CFG->m_IRange[PhysicalRng].m_Dac = p_Para[CALIBRATE_DAC];
// 			ChannelInfo_Manager::SaveCalibrateCurrentPara(uw16_Chan,PhysicalRng);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_CURRENT, uw16_Chan, PhysicalRng);
                break;
            case CALIBRATE_CONTROLTYPE_VOLTAGE:
                p_Chn_CFG->m_VRange[uc_Range].m_Adc = p_Para[CALIBRATE_ADC];
                p_Chn_CFG->m_VRange[uc_Range].m_Dac = p_Para[CALIBRATE_DAC];
// 			ChannelInfo_Manager::SaveCalibrateVoltagePara(uw16_Chan,uc_Range);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_VOLTAGE, uw16_Chan, uc_Range);
                break;
            case CALIBRATE_CONTROLTYPE_VCLAMP_HIGH:
                p_Chn_CFG->m_VRange[uc_Range].m_ClampDac = p_Para[CALIBRATE_DAC];
// 			ChannelInfo_Manager::SaveCalibrateClampVoltagePara(uw16_Chan,uc_Range);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_CLAMPVOLH, uw16_Chan, uc_Range);

//             if(Step_Manager_task::Unipolar.BoardFlag)
//             { // 解决共用钳压问题一个代表全部
//                 for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
//                 {
//                     ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[uc_Range].m_ClampDac =  p_Para[CALIBRATE_DAC];
//                     ChannelInfo_Manager::SaveCalibrateClampVoltagePara(uc_i,uc_Range);
//                 }
//             }
                break;
            case CALIBRATE_CONTROLTYPE_VCLAMP_LOW:
                p_Chn_CFG->m_VRange[uc_Range].m_LowClampDac = p_Para[CALIBRATE_DAC];
// 			ChannelInfo_Manager::SaveCalibrateLowClampVoltagePara(uw16_Chan,uc_Range);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_CLAMPVOLL, uw16_Chan, uc_Range);
//             if(Step_Manager_task::Unipolar.BoardFlag)
//             {  // 解决共用钳压问题一个代表全部
//                 for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
//                 {
//                     ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[uc_Range].m_LowClampDac =  p_Para[CALIBRATE_DAC];
//                     ChannelInfo_Manager::SaveCalibrateLowClampVoltagePara(uc_i,uc_Range);
//                 }
//             }
                break;
            case CALIBRATE_CONTROLTYPE_ACR_GAIN:
                p_Chn_CFG->m_ACR_R = p_Para[CALIBRATE_ADC];
// 			ChannelInfo_Manager::SaveCalibrateACRPara(uw16_Chan);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_ACR, uw16_Chan, 0);
                break;
            case CALIBRATE_CONTROLTYPE_BUILDIN_VOLTAGE:
                p_Chn_CFG->m_BuildInV[pCmd->m_btAuxIdx] = p_Para[CALIBRATE_ADC];
// 			ChannelInfo_Manager::SaveCalibrateBuildInVoltagePara(uw16_Chan,pCmd->m_btAuxIdx);
                ChannelInfo_Manager::SaveCalibratePara(WRITE_BUILDINVOLT, uw16_Chan, pCmd->m_btAuxIdx);
                break;
            default:
                break;
            }
        }
    }
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
//   }
#endif
}

void  EthernetParse::MPSetM0ChannelCalibrate(MP_SET_CHANNEL_CALIBRATE *pCmd)
{
#if  USE_AS_AUXMCU == 0   //IV
    UWord16 uw16_Chan;
    UWord16 uw16_Total;
    St_RangeCali  m_para;
// 	 unsigned char data[132];
// 	St_RangeCali       m_st_ADC5DAC[MAXCHANNELNO];
    uw16_Total = pCmd->m_wTotalChannelNumber;
    uw16_Chan = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_SUCCESS);
    for(UChar8 uc_idx = 0; uc_idx < (pCmd->m_wTotalChannelNumber); uc_idx++)
    {
// 		St_RangeCali  m_para;
        m_para =  pCmd->m_st_ADC5DAC[uc_idx][0];
        M0RedundantFunc::m_M0_CalibPara[uw16_Chan+uc_idx] = m_para;
// 		M0RedundantFunc::SetM0CalibPara(uw16_Chan,uw16_Total,m_para);
    }
    m_para=pCmd->m_st_ADC5DAC[0][0];
    M0RedundantFunc::SetM0CalibPara(uw16_Chan,uw16_Total,m_para);
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}
////////
// MPIndependentActionSwitch(MP_INDEPENDENT_ACTIONSWITCH *pCmd) : 网络命令独立控制行为开关处理函数
// Input: pCmd 网络独立控制行为开关命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentActionSwitch(MP_INDEPENDENT_ACTIONSWITCH *pCmd)
{
    //  独立控制属于第三方控制命令，为打开第三方服务开关。
#if  USE_AS_AUXMCU == 1
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = pCmd->m_wTotalChannelNumber;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#else
    st_Third_Party_Msg    st_Msg;
    UWord16 uw16_Total;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
    {
        if(PowerAdj::uc_DefaultVoltageFlag==0)
        {
            PowerAdj::uc_DefaultVoltageFlag=1;
            PowerAdj::m_PowerAdjStatus =POWERADJ_GOTOThirdParty;
        }
    }
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    UWord16  uw16_Chan  =  pCmd->m_wChannelNumber;
    UChar8   uc_OnSwitch = pCmd->m_btOnSwitch;

#if ADC_CS5560_ENABLE == 0
    if(board_config::Feature_Enable_init.ADC_CS5560_Enable == 1) //
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOTHIRD_FAIL;//MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
        return;
    }
#endif


    for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
    {   // This is for IVChannel
        if(uc_Total_Flag == 1)
        {
            uw16_Chan = uc_idx; // 多通道处理
        }

        // 处理内容  // 发送信息到第三方
        st_Msg.TaskID =  ETHERNETPARS_ETASK;
        st_Msg.chn = uw16_Chan;


        if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_SCH)    //  20130906 陈东  实现单MCU内全部通道互锁问题
        {
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_bCaliBoard = pCmd->m_bCaliBoard;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_LastCaliTime = CommonFunc::GetSystemTime();//OS_Time;
            thirdParty::UnsafeLockSwitch[uw16_Chan] = 0;  //add by yy20161216
            if(uc_OnSwitch)
            {
                //if(thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_INACTIVE)
                {
                    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_SET_3_RD_MODE;
                    //thirdParty::setStatus(uw16_Chan_Idx,THIRD_PARTY_STATUS_IDLE);    //激活到idle状态

                    //<<<LATER>>>   add by chen 防止校准时能run sch  同样在第三方也有此处理
                    //comment out chen 20130822
                    ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_THIRD_PARTY;
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].chn_Running_Flag = CHN_LOCK_THIRD_PARTY;

                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBoardOperateType = pCmd->m_ucBoardOperateType;
// 					if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_AUTO_CALI)
// 					{
// 						if((pCmd->m_ucBoardOperateType & THIRD_PARTY_AUTO) == THIRD_PARTY_AUTO)
// 							ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBoardOperateType = THIRD_PARTY_AUTO;
// 					}
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBattery_Module = pCmd->m_ucBattery_Module;
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucCOM_Module = pCmd->m_ucCOM_Module;
                    if ((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uw16_Chan) )&&(ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter>0))
                    {
                        OS_LogicProcess::LogicStatus[uw16_Chan] = LOGIC_IDLE;
                        OS_LogicProcess::LogicCtrlType = DO_RUN;
                        OS_LogicProcess::m_bSlaveActive[uw16_Chan] = 0;
                        OS_LogicProcess::be_NeedWaitflg = true;
                        OS_LogicProcess::be_StartThirdParty = true;
                    }

                    thirdParty::sendMsgToMe(& st_Msg);
                }
            }
            else
            {
                if((thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_IDLE)
                        || (thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_TRAN)
                        || (thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_OUTPUT))
                {
                    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CLR_3_RD_MODE;
                    //<<<LATER>>>   add by chen 防止校准时能run sch  同样在第三方也有此处理
                    //comment out chen 20130828修改

                    ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].chn_Running_Flag = CHN_LOCK_IDLE;  // 20131010 Chen 解决安全报警后无法启动问题。

                    //放开上下钳压<<<LATER>>>   add by chen
                    // 					IR::DAC_Flag=1;
                    // 					board_config::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
                    // 					board_config::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
                    // 					IR::DAC_Flag=0;
                    Step_Manager_task::StepDAC_Out(0, DAC_OUT_PUT_MAX, ACTIVE_DAC_VH);
// 		           if(Step_Manager_task::Unipolar.Charge[uw16_Chan])
// 								Step_Manager_task::StepDAC_Out(0,0,ACTIVE_DAC_VL);
// 		           else
                    Step_Manager_task::StepDAC_Out(0, DAC_OUT_PUT_MIN, ACTIVE_DAC_VL);
                    //end <<<LATER>>>
                    if ((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uw16_Chan) )&&(ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter>0))
                    {
                        OS_LogicProcess::LogicStatus[uw16_Chan] = LOGIC_IDLE;
                        OS_LogicProcess::LogicCtrlType = DO_STOP;
                        OS_LogicProcess::m_bSlaveActive[uw16_Chan] = 0;
                    }
                    thirdParty::sendMsgToMe(& st_Msg);
                }
                safety_check::ClearUnsafeIfNotRunning(uw16_Chan);
            }
        }
        else
        {
            m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
            m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = ST_ETHERNETPARSE;
            m_ReportStatus.m_st_RunningStatus.m_btErrorCode =  MP_ERROR_LOCK_TOTHIRD_FAIL;
            m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
            //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
            MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
        }

    }

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPIndependentAction(MP_INDEPENDENT_ACTION *pCmd) : 网络命令独立控制行为处理函数
// Input: pCmd 网络独立控制行为命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentAction(MP_INDEPENDENT_ACTION *pCmd)
{
    //  独立控制属于第三方控制命令，为第三方操作控制服务。
#if  USE_AS_AUXMCU == 1
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = pCmd->m_wTotalChannelNumber;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    float m_CtrlVal;
    unsigned char m_AuxType, m_MessageSource, m_CurrentRange, m_VoltageRange, m_Bid;
    m_AuxType = pCmd->m_IAInfo.IA_AuxInfo.m_AuxType;
    m_MessageSource = pCmd->m_IAInfo.IA_AuxInfo.m_MessageSource;
    m_CurrentRange = pCmd->m_st_CtrlData.m_btCurrentRange;
    m_VoltageRange = pCmd->m_st_CtrlData.m_btVoltageRange;
    m_CtrlVal = pCmd->m_st_CtrlData.m_fCtrlVal[0];
    m_Bid = pCmd->m_btAutoCali;//暂时借用 牛亚松
    if(m_AuxType == AUX_AO )
    {
        unsigned char uc_data_group_idx;
        unsigned char uc_chn;
        if(m_MessageSource == 0xAA)
        {
            if((Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_AIO_DIO_602178) || \
                    (Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_WMADC_AIO_DIO_602548)||\
                    (Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_200A_412206)||
                    (Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_2US6K_422108))
            {
                uc_data_group_idx = 3;//暂时写死
                uc_chn = pCmd->m_wChannelNumber;
            }
            else
            {
                uc_data_group_idx = 2;
                uc_chn = pCmd->m_wChannelNumber;
            }
        }
        else
        {
            uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[m_AuxType].m_StartInSnake + pCmd->m_wChannelNumber].m_data_group_idx;
            uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[m_AuxType].m_StartInSnake + pCmd->m_wChannelNumber].m_chn;
        }

        if(Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_Sinwave_Current_Source_604700)  // Add by DKQ for calibration 604700
        {
            Aux_ACIMTest::ACIM_5160_Value = m_CtrlVal;

            Aux_ACIMTest::b_ACIM_StartTest_Flag = true;
        }

        else
        {
            unsigned long data = Aux_DataManager::CalibrateVoltageDAC(uc_data_group_idx, uc_chn, m_CtrlVal);


            if(board_config::Func_AUX_Dac_Out != 0)
                (*board_config::Func_AUX_Dac_Out)(uc_chn, data);
        }
    }
    if( m_AuxType == AUX_ACIM )
    {
        unsigned char uc_data_group_idx;
        unsigned char uc_chn;
        if(m_MessageSource == 0xAA)
        {
            if((Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_ACR_408888) || \
                    (Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_ACR_603612))
            {
                if(m_CtrlVal < 0.5)
                {
                    Aux_ACIMTest::ACIM_5160_Value = 1;
                    Aux_ACIMTest::ACIM_SineAmplitude_value = 1.0f;
                }
                else
                {
                    Aux_ACIMTest::ACIM_5160_Value = 0.1;
                    Aux_ACIMTest::ACIM_SineAmplitude_value = 0.1f;
                }

                Aux_ACIMTest::uc_ACIM_TEST_Source = 0xAA;
                if ( pCmd->m_wChannelNumber == 0)
                    Aux_ACIMTest:: uc_ACIM_CALI_OhmRange = ACIM_FROM_10_TO_100_mOhm;
                else
                    Aux_ACIMTest:: uc_ACIM_CALI_OhmRange = ACIM_FROM_100_TO_1000_mOhm;

                Aux_ACIMTest::b_ACIM_StartTest_Flag = true;
            }

        }
    }
    if(m_AuxType == AUX_EXTERNALCHARGE )
    {
        unsigned char uc_data_group_idx;
        unsigned char uc_chn;
        unsigned char value;
        value = m_CtrlVal;
        if(m_MessageSource == 0xAA)
        {
            if(Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType == Aux_ExtCharge_601618)
            {
                uc_data_group_idx = m_Bid;
                uc_chn = pCmd->m_wChannelNumber;
            }
        }
        else
        {
            uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[m_AuxType].m_StartInSnake + pCmd->m_wChannelNumber].m_data_group_idx;
            uc_chn = Aux_DataManager::m_AuxChanLongSnake[Aux_DataManager::m_AuxChnLookupTable[m_AuxType].m_StartInSnake + pCmd->m_wChannelNumber].m_chn;
        }
        uc_chn /= 2;
        Aux_ExtCharge_601618_Base::Set_ExtCharge_Relay(value, uc_chn);
    }
    if(m_AuxType ==AUX_VOLTAGE)//Aux_PowerADJ_200A_412206校准电压时需要给出DAC动作配合。
    {
        unsigned char uc_data_group_idx;
        unsigned char uc_chn;
        if((Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_200A_412206)||
                (Aux_DataManager::m_AllAuxBoards[m_Bid].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_2US6K_422108))
        {
            if(m_MessageSource==0xAA)
            {
                uc_data_group_idx = 3;//暂时写死
                uc_chn = pCmd->m_wChannelNumber;
            }
            else
            {
                uc_data_group_idx = 3;//暂时写死
                uc_chn = pCmd->m_wChannelNumber;
            }
            unsigned long data=Aux_DataManager::CalibrateVoltageDAC(uc_data_group_idx,uc_chn,m_CtrlVal);
            if(board_config::Func_AUX_Dac_Out != 0)
                (*board_config::Func_AUX_Dac_Out)(uc_chn, data);
        }
    }
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#else
    UWord16 uw16_Chan;
    st_Third_Party_Msg    st_Msg;
    UWord16 uw16_Total;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan  =  pCmd->m_wChannelNumber;

#if ADC_CS5560_ENABLE == 0
    if(board_config::Feature_Enable_init.ADC_CS5560_Enable == 1) //
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOTHIRD_FAIL;//MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
        return;
    }
#endif

    if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_SCH)
    {
        for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
        {   // This is for IVChannel
            if(uc_Total_Flag == 1)
            {
                uw16_Chan = uc_idx; // 多通道处理
            }

            St_ChannelInfo * p_Info = &ChannelInfo_Manager::ChannelInfo[uw16_Chan];

            p_Info->m_LastCaliTime = CommonFunc::GetSystemTime();//OS_Time;
            // 处理内容  // 发送信息到第三方
            /*ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_u16ControlType  = pCmd->m_st_CtrlData.m_wCtrlType;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucCurrentRange  = pCmd->m_st_CtrlData.m_btCurrentRange;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucVoltageRange  = pCmd->m_st_CtrlData.m_btVoltageRange;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_fControlValue   = pCmd->m_st_CtrlData.m_fCtrlVal[0];*/
            p_Info->m_IndependantCtrl.m_fClampVH = pCmd->m_fVclampHigh;
            p_Info->m_IndependantCtrl.m_fClampVL = pCmd->m_fVclampLow;
            ChannelInfo_Manager::ChannelInfo[uc_idx].m_PowerClampRange.clampHigh =  MAX_ARBIN_VALUE;   //160518
            ChannelInfo_Manager::ChannelInfo[uc_idx].m_PowerClampRange.clampLow  = -MAX_ARBIN_VALUE;

            //comment out chen
            //ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucCurrentRange  = pCmd->m_st_CtrlData.m_btCurrentRange;
            //ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucVoltageRange  = pCmd->m_st_CtrlData.m_btVoltageRange;

            st_Msg.TaskID =  ETHERNETPARS_ETASK;
            st_Msg.chn = uw16_Chan;
            st_Msg.st_third_party_cmd.ctl_cmd_ex.IDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
            if(pCmd->m_st_CtrlData.m_wCtrlType == CT_IVCH_CURRENT)
            {   //190917 hpq
                ChannelInfo_Manager::ChannelInfo[uc_idx].m_bThirdParty_FineTuning = pCmd->m_IAInfo.IA_IvInfo.m_bFineTuning;
                ChannelInfo_Manager::ChannelInfo[uc_idx].m_bThirdParty_FineTuningDone = 0;
                ChannelInfo_Manager::ChannelInfo[uc_idx].m_fThirdParty_SetCurrent = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                ChannelInfo_Manager::ChannelInfo[uc_idx].m_fThirdParty_FineTuningLastOffset = 0;
                CommonFunc::GetTimeTicksUpdate(&ChannelInfo_Manager::ChannelInfo[uc_idx].m_tLastFineTuningTime);
            }
// 			if(Step_Manager_task::Unipolar.BoardFlag)
// 				st_Msg.st_third_party_cmd.ctl_cmd_ex.I_Range = pCmd->m_st_CtrlData.m_btCurrentRange;
// 			else
            st_Msg.st_third_party_cmd.ctl_cmd_ex.I_Range = ChannelInfo_Manager::GetPhysicalCurrentRange(pCmd->m_st_CtrlData.m_btCurrentRange);  //170320,add by hepeiqing

            st_Msg.st_third_party_cmd.ctl_cmd_ex.V_Range = pCmd->m_st_CtrlData.m_btVoltageRange;
            st_Msg.st_third_party_cmd.ctl_cmd_ex.ctrl_Type = pCmd->m_st_CtrlData.m_wCtrlType;
            if(pCmd->m_st_CtrlData.m_btVoltageRange != VOLTAGE_RANGE_HIGH)
            {
                if(board_config::Func_Set_Volt_Range != 0)
                {
                    board_config::Func_Set_Volt_Range(uc_idx,pCmd->m_st_CtrlData.m_btVoltageRange);
                }
            }

            //st_Msg.st_third_party_cmd.ctl_cmd_ex.BoardType = THIRD_PARTY_MANUAL;    //普通校准comment out chen 20130423
            st_Msg.st_third_party_cmd.ctl_cmd_ex.BoardType = p_Info->m_IndependantCtrl.m_ucBoardOperateType;
            st_Msg.st_third_party_cmd.ctl_cmd_ex.Battery_Module = p_Info->m_IndependantCtrl.m_ucBattery_Module;
            st_Msg.st_third_party_cmd.ctl_cmd_ex.COM_Module = p_Info->m_IndependantCtrl.m_ucCOM_Module;
            st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_SET_3_RD_MODE;

            if(pCmd->m_btAutoCali)   // 冗余处理，以便支持日后的Rest类型控制。
                ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBoardOperateType = THIRD_PARTY_AUTO;
            else
                ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBoardOperateType = THIRD_PARTY_MANUAL;  // yy 20161216

//             if(Step_Manager_task::Unipolar.BoardFlag)   // 20161025
//             {
//                 unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uw16_Chan].m_ucActiveIdx = 0;
//                 ChannelInfo_Manager::ChannelStep[uw16_Chan].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = st_Msg.st_third_party_cmd.ctl_cmd_ex.I_Range;
//                 Step_Manager_task::Unipolar_CheckDataType(uw16_Chan,pCmd->m_st_CtrlData.m_wCtrlType,pCmd->m_st_CtrlData.m_fCtrlVal[0]);
//             }

            switch(pCmd->m_dwCmd)
            {
            //独立控制自校准板行为    // Begin-----------------------------------------//
            case CMD_INDEPENDENT_ACTION_AUTOCALIBRATE:
                switch(p_Info->m_IndependantCtrl.m_ucBoardOperateType)
                {
                case THIRD_PARTY_AUTO_CALI_HP_3RD:  //  HP表工作模式
                    switch(pCmd->m_st_CtrlData.m_wCtrlType)
                    {
                    case CT_IVCH_CURRENT:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CC;
                        break;
                    case CT_IVCH_VOLTAGE:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CV;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.CaliType = pCmd->m_st_CtrlData.m_btExtend_Def0;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                        if(pCmd->m_st_CtrlData.m_btExtend_Def0 == AUTO_CALI_VOLTAGE_MODULE_DIGIT)
                        {
                            st_Msg.st_third_party_cmd.ctl_cmd_ex.isVDAC = 1;
                        }
                        break;
                    case CT_IVCH_C_CV:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_SETCLAMPV;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.isVDAC = 1;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                        break;
                    default:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;

                        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
                        p_Info->chn_Running_Flag = CHN_LOCK_IDLE;
                        break;
                    }
                    break;

                case THIRD_PARTY_AUTO_CALI_SELF_3RD:  //  自身校准工作模式
                    switch(pCmd->m_st_CtrlData.m_wCtrlType)
                    {
                    case CT_IVCH_CURRENT:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CC;
                        break;
                    case CT_IVCH_VOLTAGE:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CV;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.CaliType = pCmd->m_st_CtrlData.m_btExtend_Def0;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                        if(pCmd->m_st_CtrlData.m_btExtend_Def0 == AUTO_CALI_VOLTAGE_MODULE_DIGIT)
                        {
                            st_Msg.st_third_party_cmd.ctl_cmd_ex.isVDAC = 1;
                        }
                        break;
                    default:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;

                        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
                        p_Info->chn_Running_Flag = CHN_LOCK_IDLE;
                        break;
                    }
                    break;

                case THIRD_PARTY_AUTO_CALI_ADC_3RD:  //  ADC工作模式
                    switch(pCmd->m_st_CtrlData.m_wCtrlType)
                    {
                    case CT_IVCH_CURRENT:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CC;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.I_Range = pCmd->m_st_CtrlData.m_btCurrentRange;
                        break;
                    case CT_IVCH_VOLTAGE:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CV;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.V_Range = pCmd->m_st_CtrlData.m_btVoltageRange;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.CaliType = pCmd->m_st_CtrlData.m_btExtend_Def0;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                        if(pCmd->m_st_CtrlData.m_btExtend_Def0 == AUTO_CALI_VOLTAGE_MODULE_DIGIT)
                        {
                            st_Msg.st_third_party_cmd.ctl_cmd_ex.isVDAC = 1;
                        }
                        break;
                    case CT_IVCH_C_CV:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_SETCLAMPV;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.V_Range = pCmd->m_st_CtrlData.m_btVoltageRange;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.isVDAC = 1;
                        st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                        break;
                    default:
                        st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;

                        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
                        p_Info->chn_Running_Flag = CHN_LOCK_IDLE;
                        break;
                    }
                    break;
                }
                break;
            //独立控制自校准板行为    // End-----------------------------------------//

            //独立控制通道板第三方行为    // Begin-----------------------------------------//
            default:
                switch(pCmd->m_st_CtrlData.m_wCtrlType)
                {
                case CT_IVCH_CURRENT:
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CC;
                    //st_Msg.st_third_party_cmd.ctl_cmd_ex.I_Range = pCmd->m_st_CtrlData.m_btCurrentRange;
                    break;
                case CT_IVCH_VOLTAGE:
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CV;
                    st_Msg.st_third_party_cmd.ctl_cmd_ex.V_Range = pCmd->m_st_CtrlData.m_btVoltageRange;
                    st_Msg.st_third_party_cmd.ctl_cmd_ex.VDAC_Set = pCmd->m_st_CtrlData.m_fCtrlVal[0];
                    break;

                case CT_IVCH_DIGIV:       //04.23.2013
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CV;
                    st_Msg.st_third_party_cmd.ctl_cmd_ex.V_Range = pCmd->m_st_CtrlData.m_btVoltageRange;
                    break;

                case CT_IVCH_POWER:
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_CP;
                    break;
                default:
                    st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;

                    ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
                    p_Info->chn_Running_Flag = CHN_LOCK_IDLE;
                    thirdParty::UnsafeLockSwitch[uw16_Chan] = 0;  //add by yy20161216
                    break;
                }
                break;
                //独立控制通道板第三方行为    // End-----------------------------------------//
            }

            thirdParty::sendMsgToMe(& st_Msg);

        }
    }
    else
    {
        /*m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode =  MP_ERROR_LOCK_TOTHIRD_FAIL;
        m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan,SEND_EVENT_CONTENT);   //向PC发送回复信息*/
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}


////////
// MPIndependentEmergencyStop(MP_INDEPENDENT_EMERGENCYSTOP *pCmd) : 网络命令独立控制紧急停止处理函数
// Input: pCmd 网络独立控制紧急停止命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentEmergencyStop(MP_INDEPENDENT_EMERGENCYSTOP *pCmd)
{
    //  独立控制属于第三方控制命令，为第三方操作控制停止服务/或者紧急停止。
#if  USE_AS_AUXMCU == 0
    UWord16  uw16_Chan;
    st_Third_Party_Msg    st_Msg;
    UWord16 uw16_Total;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan =  pCmd->m_wChannelNumber;
    for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
    {   // This is for IVChannel
        if(uc_Total_Flag == 1)
        {
            uw16_Chan = uc_idx; // 多通道处理
        }

        // 处理内容  // 发送信息到第三方
        if((thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_INACTIVE))
        {   // 为第三方操作控制停止服务。
            st_Msg.TaskID =  ETHERNETPARS_ETASK;
            st_Msg.chn = uw16_Chan;

            if((thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_TRAN)
                    || (thirdParty::getStatus(uw16_Chan) == THIRD_PARTY_STATUS_OUTPUT))
                st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;
            else
            {
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }

            thirdParty::sendMsgToMe(& st_Msg);
        }
        //else
        //{  // 紧急停止。
        //}
    }

    //温控箱随SCH同步紧急停止	zyx
//	if(pCmd->m_bt_Ctrl_Stop == 1 && Schedule_Manager_task::TemperatureChamberSwitch.OpenFlag == true)  //comment out by zc 04.25.2018
    if(pCmd->m_bt_Ctrl_Stop == 1)  //changed by zc 04.25.2018 遍历每个温控器标志位
    {
        uw16_Chan =  pCmd->m_wChannelNumber;
        for(UChar8 uc_idx = uw16_Chan; uc_idx < pCmd->m_wTotalChannelNumber; uc_idx++)
        {   // This is for IVChannel
            if(uc_Total_Flag == 1)
            {
                uw16_Chan = uc_idx; // 多通道处理
            }
            if(ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_bUseGroupTChamber)
                continue;
            for(int auxUnitIdx = 0; auxUnitIdx < MAXMCUUNIT; auxUnitIdx++)
            {
                for(int auxChnNo = 0; auxChnNo < MAXMTCCHAMBERNO; auxChnNo++)
                {
                    if( (uw16_Chan == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]) &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].OpenFlag>>auxChnNo)&0x01) == 0x01) )
                    {
                        unsigned char runON = 0;
                        CANBusParse::IV_SendTempChamberOutRunON(  Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo],
                                runON);
                        //停止MTC温度保护
                        CANBusParse::IV_SendMTCTemperatureSaftyCheckOFF(Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo]);

                        //将温控箱设置为关状态
                        Schedule_Manager_task::CloseTemperatureChamber( auxUnitIdx, auxChnNo );


                    }

                    if( (uw16_Chan == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]) &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].usingFlag>>auxChnNo)&0x01) == 0x01) )
                    {
                        unsigned char runON = 0;
                        CANBusParse::IV_SendTempChamberOutRunON(  Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo],
                                runON);
                        //停止MTC温度保护
                        CANBusParse::IV_SendMTCTemperatureSaftyCheckOFF(Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo]);

                        //释放温控箱的占用
                        Schedule_Manager_task::RelsaseTemperatureChamber( auxUnitIdx, auxChnNo );

                    }
                }
            }
        }
    }


// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPIndependentReadData(MP_INDEPENDENT_READDATAORSTATE *pCmd) : 网络命令独立控制行为读取数据或者状态处理函数
// Input: pCmd 网络独立控制行为读取数据或者状态命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentReadData(MP_INDEPENDENT_READDATAORSTATE *pCmd)
{
    //  独立控制属于第三方控制命令，为打开第三方服务开关。
    UWord16 uw16_Total;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    /*UWord16  uw16_Chan_Idx  =  pCmd->m_wChannelNumber;
    UChar8   uc_IsReadData  =  pCmd->m_btIsReadData;
    UChar8   uc_IsState     =  pCmd->m_btIsState;*/
    m_ResultFeedback.m_btNone_11[0] = pCmd->m_btNone_11[0];  // AUX TYPE
    m_ResultFeedback.m_btNone_11[1] = pCmd->m_btIsReadData; // IsReadData
    m_ResultFeedback.m_btNone_11[2] = pCmd->m_btIsState;   // IsState
    m_ResultFeedback.m_btNone_11[3] = pCmd->m_btNone_11[1] ;//00:Mist AA:ListenForNet

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
}
void EthernetParse::MP_M0_ACTIONSWITCH(MP_SETM0_CalibMode *pCmd )
{
#if  USE_AS_AUXMCU == 0   //IV
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = pCmd->m_wTotalChannelNumber;
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);
    UWord16  uw16_Chan  =  pCmd->m_wChannelNumber;
    UChar8   uc_OnSwitch = pCmd->m_btOnSwitch;
    M0RedundantFunc::SetM0DataRefresh(uw16_Chan, uc_OnSwitch);
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

void EthernetParse::MPIndependentReadCfg(MP_INDEPENDENT_READ_CFG *pCmd)
{
    UWord16 uw16_Total;

    uw16_Total = pCmd->m_wTotalChannelNumber;
    TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;


// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);
}
/*
////////
// MPIndependentPowerOn(MP_INDEPENDENT_EMERGENCYSTOP *pCmd) : 网络命令独立控制打开电源处理函数
// Input: pCmd 网络独立控制紧急停止命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentPowerOn(MP_INDEPENDENT_EMERGENCYSTOP *pCmd)
{
	//  独立控制属于第三方控制命令，为第三方操作控制停止服务/或者紧急停止。
	UWord16  uw16_Chan;
	UWord16 uw16_Total;

	uw16_Total = pCmd->m_wTotalChannelNumber;
	TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
	m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
	m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
	//m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
	//m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_SUCCESS);

	uw16_Chan =  pCmd->m_wChannelNumber;
	for(UChar8 uc_idx = uw16_Chan;uc_idx < pCmd->m_wTotalChannelNumber;uc_idx++)
	{
		// 处理内容 todo: ???
	}

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
	MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}*/
/*
////////
// MPIndependentResetCAN(MP_INDEPENDENT_EMERGENCYSTOP *pCmd) : 网络命令独立控制复位CAN处理函数
// Input: pCmd 网络独立控制紧急停止命令结构指针;
// Return: N/A
///////
void EthernetParse::MPIndependentResetCAN(MP_INDEPENDENT_EMERGENCYSTOP *pCmd)
{
	//  独立控制属于第三方控制命令，为第三方操作控制停止服务/或者紧急停止。
	//    UWord16  uw16_Chan;
	UWord16 uw16_Total;

	uw16_Total = pCmd->m_wTotalChannelNumber;
	TotalNumberCountProcess(& pCmd->m_wChannelNumber, & pCmd->m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
	m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
	m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
	//m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
	//m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_SUCCESS);
	//	uw16_Chan =  pCmd->m_wChannelNumber;

// 	{
// 		// 处理内容 todo: ???
// 	}

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
	MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}*/

void EthernetParse::MPIndependent_BoardVersion(MP_INDEPENDENT_READ_BOARD_VERSION *pCmd) //1610142 zyx
{
// 	m_ResultFeedback.m_dwCopyCmd        = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult         = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK    = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code     = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);
}
#if  USE_AS_AUXMCU == 0
UWord16 EthernetParse::MPIndependent_IVBoardVersionFeedback(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK *pFeedback) //1610142 zyx
{
    memset(pFeedback, 0, sizeof(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK));

    CmdGetToFeedback(&pFeedback->m_dwCmd, &pFeedback->m_dwCmd_Extend,
                     &m_ResultFeedback.m_stMSecondField.m_wChannelNumber, &m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber);

    pFeedback->m_dwCmd = CMD_INDEPENDENT_READ_BOARD_DETAIL_FEEDBACK;
// 	pFeedback->m_dwCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;

    VersionProcess(&pFeedback->m_unitVerInfo); //读版本日期等信息
    pFeedback->BoardType = IV_BOARD;
    if(BuiltIn_DataManager::m_Sample_Flag)
    {
        BuiltIn_DataManager::m_WriteFRAM_Flag = true;
        while(BuiltIn_DataManager::m_Sample_Flag);
    }
//    CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG)); //读IV板信息 //zyx 180831
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        BuiltIn_DataManager::m_Sample_Flag = true;
        BuiltIn_DataManager::m_WriteFRAM_Flag = false;
    }
    pFeedback->BoardInfo.IV_Info.m_u16BoardType = ChannelInfo_Manager::m_st_BoadType.m_u16BoardType;
    pFeedback->BoardInfo.IV_Info.m_btBoardTypeVersion = ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion;
    pFeedback->BoardInfo.IV_Info.m_btIs32M_SDRAM = HARDWARE_SDRAM_32M;//170927 zyx
    pFeedback->BoardInfo.IV_Info.m_wOutOf_FRAM_error = CConfig::uwOutOf_FRAM_error; //170927 zyx

    pFeedback->BoardInfo.IV_Info.m_btIsMapIRange = 1;//ChannelInfo_Manager::m_st_BoadType.b_IsMapIRangeVir2Phy;//170927 zyx
//     if(pFeedback->BoardInfo.IV_Info.m_btIsMapIRange)
//     {
    memcpy(pFeedback->BoardInfo.IV_Info.m_IRangeVir2PhyMap, ChannelInfo_Manager::m_st_BoadType.m_IRangeVirtual2PhysicalMap, sizeof(unsigned char)*NUM_OF_IV_RANGE_I);
//    }
    CConfig::IV_BoardType_OldTransformToNew();
    memcpy(&pFeedback->BoardInfo.IV_Info.m_Detail, &OS_ServeProcess::Detail, sizeof(St_DETAIL));
    return (sizeof(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK));

}
#endif

#if  USE_AS_AUXMCU == 1
UWord16 EthernetParse::MPIndependent_AuxBoardVersionFeedback(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK *pFeedback) //1610142 zyx
{
    memset(pFeedback, 0, sizeof(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK));

    CmdGetToFeedback(&pFeedback->m_dwCmd, &pFeedback->m_dwCmd_Extend,
                     &m_ResultFeedback.m_stMSecondField.m_wChannelNumber, &m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber);

    pFeedback->m_dwCmd = CMD_INDEPENDENT_READ_BOARD_DETAIL_FEEDBACK;
// 	pFeedback->m_dwCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;

    VersionProcess(&pFeedback->m_unitVerInfo); //读版本日期等信息

    pFeedback->BoardType = AUX_BOARD;
    pFeedback->BoardInfo.AUX_Info.m_btIs32M_SDRAM = HARDWARE_SDRAM_32M; //170925 zyx
//     if(	AuxCConfig::Reg_AUXBoardInfo() )  //读AUX板信息 //zyx 180831
//     {
    int tmp = 0;
    for(int i = 0; i < MAX_BOARD_PER_AUXMCU; i++)
    {   //将存在的Board排列好，方便上位机读取
        if((( E2PROM_24LC128::BoardCfgBit >> i) & 1 ) == 1 )
            pFeedback->BoardInfo.AUX_Info.AuxBoard[tmp++] = Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - i].m_BoardInfo.m_abi_globe;
    }
    pFeedback->BoardInfo.AUX_Info.BoardCount = tmp;
//     }
    memcpy(&pFeedback->BoardInfo.AUX_Info.m_Detail, &OS_ServeProcess::Detail, MAX_DETAIL);
    return (sizeof(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK));

}
#endif

////////
// MPIndependentReadDataFeedback(MP_INDEPENDENT_READDATAORSTATE_FEEDBACK *pFeedback)
//      : 网络命令独立控制行为读取数据或者状态处理回应函数
// Input: pFeedback 网络独立控制行为读取数据或者状态命令回应结构指针;
// Return: UWord16
///////
UWord16 EthernetParse::MPIndependentReadDataFeedback(MP_INDEPENDENT_READDATAORSTATE_FEEDBACK *pFeedback)
{
    //  独立控制属于第三方控制命令，为打开第三方服务开关回复成功是否。
#if  USE_AS_AUXMCU == 1
    unsigned char uc_Bid = 0;
    UWord16  uw16_Chan;
    UWord16  uw16_PhysicalCh;
// 	UWord16 uw16_Total;
    //校正辅助
    unsigned char uc_AuxType;  // DKQ 06.18.2015
    unsigned char uc_ChIdx;
    unsigned char uc_data_group_idx, uc_chn;
    unsigned char uc_MessageSource;
    CmdGetToFeedback(&pFeedback->m_dwCmd, &pFeedback->m_dwCmd_Extend, //1807131
                     &pFeedback->m_wChannelNumber, &pFeedback->m_wTotalChannelNumber);
// 	uw16_Total = m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber;
    TotalNumberCountProcess(& m_ResultFeedback.m_stMSecondField.m_wChannelNumber,
                            & m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);



    pFeedback->m_dwCmd = CMD_INDEPENDENT_READDATAORSTATE_FEEDBACK;
// 	pFeedback->m_dwCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;
// 	pFeedback->m_wChannelNumber = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
// 	pFeedback->m_wTotalChannelNumber = uw16_Total;

    pFeedback->m_st_Read_Value[0].m_btNone_11[0] = m_ResultFeedback.m_btNone_11[0]; // AUX TYPE
    pFeedback->m_btIsReadData = m_ResultFeedback.m_btNone_11[1];   //  IsReadData
    pFeedback->m_btIsState = m_ResultFeedback.m_btNone_11[2];	//  IsState

    /* for(uc_Bid = 0; uc_Bid < MAX_BOARD_PER_AUXMCU; uc_Bid ++) // 计算顺序排列时的通道信息存放位置
    {
    if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_BType.m_AuxType == m_ResultFeedback.m_btNone_11[0])  // Aux type
    &&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_Start_InUnit <= pFeedback->m_wChannelNumber) // Aux Channel
    &&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_End_InUnit >= pFeedback->m_wChannelNumber))
    {
    break;    // 找到指定的板
    }
    }*/  //暂时屏蔽 齐静明20150603
    uc_AuxType = m_ResultFeedback.m_btNone_11[0];
    uc_ChIdx = pFeedback->m_wChannelNumber;
    uc_MessageSource = m_ResultFeedback.m_btNone_11[3];
    if(uc_MessageSource == 0xAA) //ListenForNet
    {
        uc_Bid = m_ResultFeedback.m_btNone_11[2];
        if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
            uc_Bid = 0;
        uc_data_group_idx = 	uc_ChIdx / 16 + Aux_DataManager::m_AllAuxBoards[uc_Bid].m_data_group_start;	 //1807131
        uc_chn = uc_ChIdx % 16;
    }
    else
    {
        uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_ChIdx)].m_data_group_idx;
        uc_chn = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake + uc_ChIdx)].m_chn;

        uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];  //需要BID 信息。 得建立一个data_group_idx 与 ＢＩＤ拨码的关系

        if(uc_Bid >= MAX_BOARD_PER_AUXMCU)
            uc_Bid = 0;
    }

    uw16_Chan = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
    for(UWord16 uw16_i = 0; uw16_i < pFeedback->m_wTotalChannelNumber; uw16_i++)
    {
        pFeedback->m_st_Read_Value[uw16_i].m_btControlState = 0;
        if(uc_MessageSource == 0xAA) //ListenForNet
        {
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[0] =  Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_avg_code & 0xff;
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[1] =  Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_avg_code >> 8 & 0xff;
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[2] =  Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_avg_code >> 16 & 0xff;
        }
        else
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[0] =  m_ResultFeedback.m_btNone_11[0]; // AUX TYPE  [uw16_Chan + uw16_i];

        if(uc_AuxType == AUX_ACIM)
        {
            if(Aux_ACIMTest::b_ACIM_TestFinished_Flag)  // 上传测量的ACR的值
            {
                if(Aux_DDS_ACIM_ACR_408888_Base::isACIM)
                    pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = Aux_ACIMTest::ACIM_Mac[0];
                else
                    pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = Aux_ACIMTest::ACIM_Rac[0]; //辅助ACIR 值
                pFeedback->m_st_Read_Value[uw16_i].m_fCurrent = Aux_DataManager::m_AuxDataBank[0][1].aux_data; // 电压值
            }
            else                                 //回复失败， ACIM 尚未测试完成。
            {
                pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = 0; //上传电阻值0 ，上位机认为下位机未测试完成
                pFeedback->m_st_Read_Value[uw16_i].m_fCurrent = Aux_DataManager::m_AuxDataBank[0][1].aux_data; // 电压值
            }
        }
        else
        {
            if(uc_MessageSource == 0xAA && Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_AuxType == AUX_TEMPERATURE)//ListenForNet && AUX_TEMPERATURE
                pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = AUX_SampleProcess::Convert_Chn(uc_data_group_idx,uc_chn);
            else
                pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_data;   // 辅助通道上显示的物理值
            pFeedback->m_st_Read_Value[uw16_i].m_fCurrent = Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_avg_voltage; // adc电压值
        }
    }
    return  (sizeof(ST_READDATAORSTATE_VARIABLE) * pFeedback->m_wTotalChannelNumber);
#else
    //for(UChar8 uc_i = 0; uc_i < 100;uc_i ++)
    //m_CmdTxBufSynchro[uc_i] = CODE_EMPTY; // 清空回传数据区

    // pFeedback->m_btNone_111 =  1;  // pFeedback->m_btUnitId = Interface_LED::uc_dsp_id;
    // pFeedback->m_btNone_112 =  0X9A ; // pFeedback->m_btCmd = MPC_MCU2PC_CMD;
    UWord16  uw16_Chan;
    //UWord16 uw16_Total;
    unsigned char uc_MessageSource;
    unsigned char uc_data_group_idx, uc_chn;

// 	uw16_Total = m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber;
    CmdGetToFeedback(&pFeedback->m_dwCmd, &pFeedback->m_dwCmd_Extend,
                     &pFeedback->m_wChannelNumber, &pFeedback->m_wTotalChannelNumber);

    TotalNumberCountProcess(& m_ResultFeedback.m_stMSecondField.m_wChannelNumber,
                            & m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);



    pFeedback->m_dwCmd = CMD_INDEPENDENT_READDATAORSTATE_FEEDBACK;
// 	pFeedback->m_dwCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;
// 	pFeedback->m_wChannelNumber = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
// 	pFeedback->m_wTotalChannelNumber = uw16_Total;

    pFeedback->m_btIsReadData = m_ResultFeedback.m_btNone_11[1];
    pFeedback->m_btIsState = m_ResultFeedback.m_btNone_11[2];

    uc_MessageSource = m_ResultFeedback.m_btNone_11[3];
    uw16_Chan = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
    if(uc_MessageSource == 0xAA) //ListenForNet
    {
        uc_data_group_idx = 	(uw16_Chan >> 4);
        uc_chn = uw16_Chan % 16;

        for(UWord16 uw16_i = 0; uw16_i < pFeedback->m_wTotalChannelNumber; uw16_i++)
        {
            pFeedback->m_st_Read_Value[uw16_i].m_btControlState = 0;
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[0] =  BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_avg_code & 0xff;
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[1] =  BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_avg_code >> 8 & 0xff;
            pFeedback->m_st_Read_Value[uw16_i].m_btNone_11[2] =  BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_avg_code >> 16 & 0xff;
            if(BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_AuxType == AUX_TEMPERATURE)
                pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = Sample_Pro::Convert_Chn(uc_data_group_idx,uc_chn);
            else
                pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_data;
//             pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_data;   // 辅助通道上显示的物理值
            pFeedback->m_st_Read_Value[uw16_i].m_fCurrent = BuiltIn_DataManager::m_BuiltInDataBank[uc_data_group_idx][uc_chn].aux_avg_voltage; // adc电压值
        }
    }
    else
    {
        for(UWord16 uw16_i = 0; uw16_i < pFeedback->m_wTotalChannelNumber; uw16_i++)
        {
            pFeedback->m_st_Read_Value[uw16_i].m_btControlState = thirdParty::getStatus(uw16_Chan + uw16_i);
            pFeedback->m_st_Read_Value[uw16_i].m_fCurrent = ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[uw16_Chan + uw16_i];
            //ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_Chan + uw16_i,MetaCode_PV_Current);
            pFeedback->m_st_Read_Value[uw16_i].m_fVoltage = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_Chan + uw16_i, MetaCode_PV_Voltage);

            for(int i = 0; i < MAX_BUILDIN_2V_COUNT; i++)
                pFeedback->m_st_Read_Value[uw16_i].m_fBuildInVolt[i] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_Chan + uw16_i, MetaCode_PV_BuiltIn_Data1 + i);

        }
    }
    return  (sizeof(ST_READDATAORSTATE_VARIABLE) * pFeedback->m_wTotalChannelNumber);
#endif
}

void EthernetParse::MP_M0_ReadData(MP_INDEPENDENT_READDATAORSTATE *pCmd)
{
#if  USE_AS_AUXMCU == 0   //IV
    UWord16 uw16_Total;
//   MP_M0_READDATAORSTATE_FEEDBACK *pFeedback;
// 		m_pFeedBack->m_M0_ReadData_Feedback->
    uw16_Total = pCmd->m_wTotalChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);
    CmdGetToFeedback(&m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd, &m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd_Extend,
                     &m_pFeedBack->m_M0_ReadData_Feedback.m_wChannelNumber, &m_pFeedBack->m_M0_ReadData_Feedback.m_wTotalChannelNumber);

    m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd = CMD_M0_READDATAORSTATE_FEEDBACK;
    for(unsigned char i = 0; i < uw16_Total; i++)
    {
        m_pFeedBack->m_M0_ReadData_Feedback.m_ucRead_Value[i] = M0RedundantFunc::m_SampleData[pCmd->m_wChannelNumber];
    }
    UWord16 uw16_length = sizeof(MP_M0_READDATAORSTATE_FEEDBACK);
    SendFeedBack(MPCS_ACK, uw16_length);
// 	SendFeedBack(MPCS_ACK,uw16_Length);
#endif
}

void EthernetParse::MP_M0_ReadCalibPara(MP_M0_READCalibPara *pCmd)
{
#if  USE_AS_AUXMCU == 0   //IV
    UWord16 uw16_Total;
    uw16_Total = pCmd->m_wTotalChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wChannelNumber = pCmd->m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber = uw16_Total;
    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);
    CmdGetToFeedback(&m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd, &m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd_Extend,
                     &m_pFeedBack->m_M0_ReadData_Feedback.m_wChannelNumber, &m_pFeedBack->m_M0_ReadData_Feedback.m_wTotalChannelNumber);
    m_pFeedBack->m_M0_ReadData_Feedback.m_dwCmd = CMD_Read_M0_CalibPara_FEEDBACK;
    for(unsigned char i = 0; i < uw16_Total; i++)
    {
        m_pFeedBack->m_M0_ReadCalib_Feedback.m_st_Read_Para[i] = M0RedundantFunc::m_M0_CalibPara[i];
    }
    m_pFeedBack->m_M0_ReadCalib_Feedback.m_wChannelNumber = 0;
    m_pFeedBack->m_M0_ReadCalib_Feedback.m_wTotalChannelNumber = uw16_Total;
    UWord16 uw16_length = sizeof(MP_M0_READCalibPara_FEEDBACK);
    SendFeedBack(MPCS_ACK, uw16_length);
#endif
}
UWord16 EthernetParse::MPIndependentReadCfgFeedback(MP_INDEPENDENT_READ_CFG_FEEDBACK *pFeedback)
{
#if  USE_AS_AUXMCU == 0
    UWord16  uw16_Chan;//uw16_Total;

    //uw16_Total = m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber;
    TotalNumberCountProcess(& m_ResultFeedback.m_stMSecondField.m_wChannelNumber,
                            & m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

    CmdGetToFeedback(&pFeedback->m_dwCmd, &pFeedback->m_dwCmd_Extend,
                     &pFeedback->m_wChannelNumber, &pFeedback->m_wTotalChannelNumber);

    pFeedback->m_dwCmd = CMD_INDEPENDENT_READ_CFG_FEEDBACK;
// 	pFeedback->m_dwCmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;
// 	pFeedback->m_wChannelNumber = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
    //pFeedback->m_wTotalChannelNumber = uw16_Total;

    pFeedback->m_BriefCfg.m_uBoardType = 111;
    pFeedback->m_BriefCfg.m_uCurrRngCount = board_config::DacInfo.m_bCurrRngCount;
    pFeedback->m_BriefCfg.m_uVoltRngCount = board_config::DacInfo.m_bVoltRngCount;

    uw16_Chan = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;

    float fHigh, fLow;
    St_ChannelCFG * p_Chn_CFG = &ChannelInfo_Manager::BoardCFG.m_Channel[uw16_Chan];

    for(UWord16 RngIdx = 0; RngIdx < pFeedback->m_BriefCfg.m_uCurrRngCount; RngIdx++)
    {
        fHigh = ::abs(p_Chn_CFG->m_IRange[RngIdx].m_Bound.m_High);
        fLow  = ::abs(p_Chn_CFG->m_IRange[RngIdx].m_Bound.m_Low);
        if(fLow > fHigh)
            fHigh = fLow;
        pFeedback->m_BriefCfg.m_fCurrBound[RngIdx] = fHigh;
    }

    if(pFeedback->m_BriefCfg.m_uVoltRngCount > 1)
    {
        pFeedback->m_BriefCfg.m_uVoltRngCount = 2;
        fHigh = ::abs(p_Chn_CFG->m_IRange[VOLTAGE_RANGE_LOW].m_Bound.m_High);
        fLow  = ::abs(p_Chn_CFG->m_IRange[VOLTAGE_RANGE_LOW].m_Bound.m_Low);
        if(fLow > fHigh)
            fHigh = fLow;
        pFeedback->m_BriefCfg.m_fVoltBound[0] = fHigh;

        fHigh = ::abs(p_Chn_CFG->m_IRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High);
        fLow  = ::abs(p_Chn_CFG->m_IRange[VOLTAGE_RANGE_HIGH].m_Bound.m_Low);
        if(fLow > fHigh)
            fHigh = fLow;
        pFeedback->m_BriefCfg.m_fVoltBound[1] = fHigh;
    }
    else
    {
        fHigh = ::abs(p_Chn_CFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High);
        fLow  = ::abs(p_Chn_CFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_Low);
        if(fLow > fHigh)
            fHigh = fLow;
        pFeedback->m_BriefCfg.m_fVoltBound[0] = fHigh;
    }

    return  (sizeof(St_ChanBriefCfg));
#endif
}
////////
// MPScheduleSetSetting(MP_SCHEDULE_SET_SETTING *pCmd) : 网络命令设置Schedule Setting信息处理函数
// Input: pCmd 网络设置Schedule Setting信息命令结构指针;
// Return: N/A
///////
/*void EthernetParse::MPScheduleSetSetting(MP_SCHEDULE_SET_SETTING *pCmd)
{
#if  USE_AS_AUXMCU == 0
UWord16 uw16_Chan;

m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
& pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
//m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
//m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;

uw16_Chan =  pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

for(UChar8 uc_idx = uw16_Chan;uc_idx < pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber;uc_idx++)
{   // This is for IVChannel
if(uc_Total_Flag == 1)
{
uw16_Chan = uc_idx; // 多通道处理
}

ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Isafty.m_fMin = pCmd->m_st_ScheduleSetting.m_stCurrent.m_fMin; // 暂存数据
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Isafty.m_fMax = pCmd->m_st_ScheduleSetting.m_stCurrent.m_fMax;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Vsafty.m_fMin = pCmd->m_st_ScheduleSetting.m_stVoltage.m_fMin;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Vsafty.m_fMax = pCmd->m_st_ScheduleSetting.m_stVoltage.m_fMax;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Psafty.m_fMin = pCmd->m_st_ScheduleSetting.m_stPower.m_fMin;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Psafty.m_fMax = pCmd->m_st_ScheduleSetting.m_stPower.m_fMax;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Reserved2[0].m_fMin = pCmd->m_st_ScheduleSetting.m_stVoltageClamp.m_fMin;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_Reserved2[0].m_fMax = pCmd->m_st_ScheduleSetting.m_stVoltageClamp.m_fMax;

ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_u16UnsafeCheckCount = pCmd->m_st_ScheduleSetting.m_fSafetyChecking_Count;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_fDvDtInterval = pCmd->m_st_ScheduleSetting.m_fdVdt_Interval;

//#if SUPPORT_AUX == 1
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_AuxTempSafty.m_fMax=pCmd->m_st_ScheduleSetting.m_AuxTempSafty.m_fMax;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_AuxTempSafty.m_fMin=pCmd->m_st_ScheduleSetting.m_AuxTempSafty.m_fMin;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_AuxVoltSafty.m_fMax=pCmd->m_st_ScheduleSetting.m_AuxVoltSafty.m_fMax;
ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_AuxVoltSafty.m_fMin=pCmd->m_st_ScheduleSetting.m_AuxVoltSafty.m_fMin;
//#endif
}

m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}*/

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
////////
// MPScheduleGetStatus(MP_SCHEDULE_GET_STATUS *pCmd) : 网络命令设置Schedule status信息定阅处理函数
// Input: pCmd 网络设置Schedule status信息定阅命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleGetStatus(MP_SCHEDULE_GET_STATUS *pCmd)
{
// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend = pCmd->m_dwCmd_Extend;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;

    //     if(pCmd->m_Channel_Number == 0XFFFF)
    //     {
    // 	    //ScheduleStatusReport::uc_All_ReportFlag = 1;
    //         //for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO ; uc_i ++)
    //         for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; uc_i ++)
    //         {    //  yy 20141113   time 0s report status to close
    //             ScheduleStatusReport::Set_ChIntervalTime(uc_i ,pCmd->m_Interval);
    //         }
    //     }
    //     else
    //     {
    //         ScheduleStatusReport::Set_ChIntervalTime(pCmd->m_Channel_Number ,pCmd->m_Interval);
    //     }

    ScheduleStatusReport::Set_ChIntervalTime(pCmd->m_Channel_Number, pCmd->m_Interval);

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);

    MPReportData(pCmd->m_Channel_Number, SEND_GET_SCHSTATUS);
}
#endif

////////
// MPScheduleDownload(MP_SCHEDULE_DOWNLOAD *pCmd) : 网络命令Schedule全部步内容下载处理函数
// Input: pCmd 网络Schedule全部步内容下载命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleDownload(MP_SINGLE_PACKDATA  *pCmd)
{
#if  USE_AS_AUXMCU == 0
    UChar8 uc_DownLoad_Success = 0;
    St_MD5 ScheduleID;

// 	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
// 	TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
// 		& pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

//    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_FAIL);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{
// 		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
// 		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
// 	}
// 	else
// 	{
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
// 		{
// 			// 判断是否为同一个多包的内容。
// 			for(UChar8 uc_i = 0; uc_i < sizeof(St_MD5); uc_i ++)
// 			{
// 				if(pCmd->m_stMPack.m_stMSecondField.m_StMD5Code.m_ucData[uc_i] != st_MultPackManage.m_stMSecondField.m_StMD5Code.m_ucData[uc_i])
// 				{
// 					m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 					MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 					return;
// 				}
// 			}
// 		}
// 		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
// 	}

// 	if(pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber != st_MultPackManage.m_stMSecondField.m_wChannelNumber)
// 	{
// 		// 判断是否为同一个通道的多包内容接收。
// 		MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 		return;
// 	}

// 	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
// 	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

// 	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
// 	{
// 		// 判断是否为同一个多包的内容接收完成。
// 		uc_DownLoad_Success = 1;
// 		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	}

// 	// 存包中的SCH数据
// 	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)  //  使用区域判断  0：单包或无意义 1：第一区域多包信息使用标志 2：第二区域多包信息使用标志 4：第三区域多包信息使用标志
    {
        // 已经接收完成
        if(st_MultPackManage.m_stMFirstField.m_dwPackLength == st_MultPackManage.m_stMFirstField.m_dwTotalLength)
        {
            ScheduleID = st_MultPackManage.m_stMSecondField.m_StMD5Code;
            unsigned char uc_result_i = 0;
            switch(pCmd->m_dwCmd)
            {
            case CMD_STEP_DOWNLOAD:
                break;

            case CMD_PULSE_DOWNLOAD:
                uc_result_i = ChannelInfo_Manager::InsertPulse2Buf(ScheduleID, &m_btPackDataBuffer);
                break;

            default:
                uc_result_i = ChannelInfo_Manager::InsertSchedule2Buf(ScheduleID, &m_btPackDataBuffer);
                break;
            }
            if(uc_result_i != 1)
            {
                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_SAVESCH;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;  // 失败处理
            }
        }
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//     if(uc_DownLoad_Success >= 0)
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}
void EthernetParse::MPSimulationDownload(MP_SIMULATION_DOWNLOAD  *pCmd)  //12.22.2013ADD
{
#if  USE_AS_AUXMCU == 0
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;
// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

//    CmdSetToFeedback(&pCmd->m_dwCmd,&pCmd->m_dwCmd_Extend,CONFIRM_RESULT_FAIL);

    SIMU_BUFFER::InsertSimuPack(pCmd->m_stMPack.m_stMFirstField.m_wPackNo,
                                pCmd->m_stMPack.m_stMFirstField.m_wPackTotal,
                                pCmd->m_stMPack.m_stMSecondField.m_StMD5Code,
                                pCmd->m_stMPack.m_stMFirstField.m_dwPackLength,
                                &pCmd->m_btPackData_N);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
// 	{//First package,make some initialization
// 		m_SimuDownMgr.m_MD5 = pCmd->m_stMPack.m_stMSecondField.m_StMD5Code;
// 		m_SimuDownMgr.m_bDownloading = 1;
// 		m_SimuDownMgr.m_PointCount = pCmd->m_stMPack.m_stMFirstField.m_dwTotalLength / 8;
// 		m_SimuDownMgr.m_FragCount = m_SimuDownMgr.m_PointCount / MAX_SIMU_FRAGMENT_POINT_COUNT;
// 		if((m_SimuDownMgr.m_PointCount % MAX_SIMU_FRAGMENT_POINT_COUNT) > 0)
// 			m_SimuDownMgr.m_FragCount++;
// 		m_SimuDownMgr.m_FragIdx   = 0;
// 		m_SimuDownMgr.m_ByteCount = 0;
// 		st_MultPackManage = pCmd->m_stMPack;
// 		m_SimuDownMgr.m_PackCount = pCmd->m_stMPack.m_stMFirstField.m_wPackTotal;
// 		m_SimuDownMgr.m_PackCount_Received = 0;
// 	}
// 	else
// 	{
// 		if((memcmp(&m_SimuDownMgr.m_MD5,&pCmd->m_stMPack.m_stMSecondField.m_StMD5Code,16) != 0)
// 			|| (m_SimuDownMgr.m_bDownloading == 0))
// 		{
// 			m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 			MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 			return;
// 		}
//
// 		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo)
// 	  {//重复包
// 		  m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
// 		  MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
// 		  return;
// 	  }
// 	}

// 	m_SimuDownMgr.m_PackCount_Received++;
// 	if(pCmd->m_stMPack.m_stMFirstField.m_dwPackLength > 1024)
// 		pCmd->m_stMPack.m_stMFirstField.m_dwPackLength = 100;
// 	memcpy(&m_btPackDataBuffer[m_SimuDownMgr.m_ByteCount],
// 		&pCmd->m_btPackData_N,
// 		pCmd->m_stMPack.m_stMFirstField.m_dwPackLength);
// 	m_SimuDownMgr.m_ByteCount += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength;
//
// 	unsigned long PointCount = m_SimuDownMgr.m_ByteCount / 8;
// 	unsigned long FragCount = PointCount / MAX_SIMU_FRAGMENT_POINT_COUNT;
// 	unsigned char *Address = m_btPackDataBuffer;

// 	for(int i = 0;i < FragCount;i++)
// 	{
// 		SIMU_BUFFER::InsertSimuFragment(m_SimuDownMgr.m_FragIdx,
// 			m_SimuDownMgr.m_FragCount,
// 			m_SimuDownMgr.m_MD5,
// 			MAX_SIMU_FRAGMENT_POINT_COUNT * 8,
// 			Address);

// 		Address += MAX_SIMU_FRAGMENT_POINT_COUNT * 8;
// 		m_SimuDownMgr.m_FragIdx ++;
// 	}

// 	m_SimuDownMgr.m_ByteCount -= FragCount * MAX_SIMU_FRAGMENT_POINT_COUNT * 8;
// 	if(m_SimuDownMgr.m_ByteCount > 0)
// 	    memcpy(m_btPackDataBuffer,Address,m_SimuDownMgr.m_ByteCount);

// 	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == (pCmd->m_stMPack.m_stMFirstField.m_wPackTotal - 1))
// 	{//Last package,
// 		m_SimuDownMgr.m_bDownloading = 0;

//     if(m_SimuDownMgr.m_PackCount_Received != pCmd->m_stMPack.m_stMFirstField.m_wPackTotal)
// 		{//报文没有受全，等同于什么也没收着
// 			SIMU_BUFFER::DeleteUnusedFragment();
// 		}
// 		else if(m_SimuDownMgr.m_ByteCount > 0)
// 		{
// 			SIMU_BUFFER::InsertSimuFragment(m_SimuDownMgr.m_FragIdx,
// 				m_SimuDownMgr.m_FragCount,
// 				m_SimuDownMgr.m_MD5,
// 				m_SimuDownMgr.m_ByteCount,
// 				m_btPackDataBuffer);
// 		}
// 	}

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPScheduleOnLineUpdate(MP_SCHEDULE_DOWNLOAD *pCmd) : 网络命令Schedule在线更新内容下载处理函数
// Input: pCmd 网络Schedule在线更新内容下载命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleOnLineUpdate(MP_SINGLE_PACKDATA  *pCmd)
{
#if  USE_AS_AUXMCU == 0
    UChar8 uc_DownLoad_Success = 0;
//     St_MD5 ScheduleID;
    /*
    	m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    	TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
    		& pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

    	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
    	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
    	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
    	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    	if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == 0)
    	{
    		st_MultPackManage.m_stMFirstField = pCmd->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
    		st_MultPackManage.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
    	}
    	else
    	{
    		if(pCmd->m_stMPack.m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
    		{
    			// 判断是否为同一个多包的内容。
    			for(UChar8 uc_i = 0; uc_i < sizeof(St_MD5); uc_i ++)
    			{
    				if(pCmd->m_stMPack.m_stMSecondField.m_StMD5Code.m_ucData[uc_i] != st_MultPackManage.m_stMSecondField.m_StMD5Code.m_ucData[uc_i])
    				{
    					m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
    					MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
    					return;
    				}
    			}
    		}
    		st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMPack.m_stMFirstField.m_dwPackLength; // 非首包为累加值
    	}

    	if(pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber != st_MultPackManage.m_stMSecondField.m_wChannelNumber)
    	{
    		// 判断是否为同一个通道的多包内容接收。
    		MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
    		return;
    	}

    	st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMPack.m_stMFirstField.m_wPackNo;
    	st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

    	if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
    	{
    		// 判断是否为同一个多包的内容接收完成。
    		uc_DownLoad_Success = 1;
    		m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    	}

    	// 存包中的SCH数据
    	memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    */
    uc_DownLoad_Success = GetSinglePack(pCmd,true);

    if(uc_DownLoad_Success == 1)  //  使用区域判断  0：单包或无意义 1：第一区域多包信息使用标志 2：第二区域多包信息使用标志 4：第三区域多包信息使用标志
    {
        // 已经接收完成
        if(st_MultPackManage.m_stMFirstField.m_dwPackLength == st_MultPackManage.m_stMFirstField.m_dwTotalLength)
        {
            unsigned char uc_result_i = 0;
            uc_result_i = ChannelInfo_Manager::OnlineUpdata_ScanScheduleBuf(&m_btPackDataBuffer);
            if(uc_result_i != 1)
            {
                St_Sche_Onlineupdata  * st_Onlineupdata = (St_Sche_Onlineupdata  *) m_btPackDataBuffer;
                for(unsigned short uw16_ch = 0; uw16_ch < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uw16_ch++)
                {
                    if(((st_Onlineupdata->uw32_Channelbit >> uw16_ch) & 0X01) == 0X01)
                    {
                        if(ChannelInfo_Manager::ChannelInfo[uw16_ch].ScheduleStatus == RUNNING)
                        {   // SCH在线更新出错将以安全第一为准，先停止通道运行。
                            ChannelInfo_Manager::ChannelInfo[uw16_ch].ScheduleStatus = GOSTOP;
                            Schedule_Manager_task::SendFinishFlag[uw16_ch].StopStatus = STOP_EMERGENCY;
                            Schedule_Manager_task::SendFinishFlag[uw16_ch].SubStopCode = eEMERGENCYSTOP_ERROR_SchOnlineupdate;
                            Schedule_Manager_task::SchTimeTrigger[uw16_ch] = 0; //清超时标志位
                        }
                    }
                }

                m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_SAVESCH;  // 配置错误代码
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;  // 失败处理
            }
        }
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
//     if(uc_DownLoad_Success >= 0)
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}



////////
// MPScheduleStop(MP_SCHEDULE_STOP *pCmd) : 网络命令Schedule结束命令处理函数
// Input: pCmd 网络Schedule结束命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleStop(MP_SCHEDULE_STOP *pCmd)
{
#if  USE_AS_AUXMCU == 0
    St_ScheduleMessage   SchMessage;
    UWord16 uw16_Chan;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan =  pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

    // SCH与第三方进行互锁
    if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_THIRD_PARTY)
    {
        ChannelInfo_Manager::m_Online_UpdataEnable[uw16_Chan] = false;
        switch(pCmd->m_dwCmd)
        {
        case CMD_SCHEDULE_PAUSE:
            SchMessage.MsgNumber = Sch_Pause_Msg;
            break;

        //case CMD_SCHEDULE_STOP:
        default:
            Step_Manager_task::ACIM_Status[uw16_Chan] = ACIM_NONE;
            Step_Manager_task::AcimRequest_Send[uw16_Chan] = false;
            SchMessage.MsgNumber = Sch_Stop_Msg;
            CANBusParse::IV_SendTOSysCtrl_BackToNormal();
            break;
        }
        SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan;
        SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5 = pCmd->m_stMPack.m_stMSecondField.m_StMD5Code; //  MD5码数组复制;

        Schedule_Manager_task::SendMsgToSchedule(uw16_Chan, SchMessage); // 发出SCh命令信息
        IVSDL::LimitArrivedRoutine(uw16_Chan);

        ChannelInfo_Manager::ChannelInfo[uw16_Chan].chn_Running_Flag = CHN_LOCK_IDLE;
        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
    }
    else
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
    }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

void EthernetParse::MPScheduleContinue(MP_SCHEDULE_CONTINUE *pCmd)
{
#if  USE_AS_AUXMCU == 0
    St_ScheduleMessage   SchMessage;
    UWord16 uw16_Chan;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan =  pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

    // SCH与第三方进行互锁
    if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_THIRD_PARTY)
    {
        SchMessage.MsgNumber = Sch_Continue_Msg;
        SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan;
        SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5 = pCmd->m_stMPack.m_stMSecondField.m_StMD5Code; //  MD5码数组复制;
        if(Step_Manager_task::ACIM_Status[uw16_Chan] == ACIM_END || Step_Manager_task::ACIM_Status[uw16_Chan] == ACIM_NONE)
            Schedule_Manager_task::SendMsgToSchedule(uw16_Chan, SchMessage); // 发出SCh命令信息
    }
    else
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
    }

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPScheduleStopAll(void) :  无条件停止本Unit中所有通道。这个是用来代替DAQ启动时连发多个Stop命令的
// Input: N/A
// Return: N/A
///////
void EthernetParse::MPScheduleStopAll( void )
{
#if  USE_AS_AUXMCU == 0
    St_ScheduleMessage   SchMessage;
    UWord16 uw16_Chan;

    for (uw16_Chan = 0; uw16_Chan < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uw16_Chan++)
    {
        if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_THIRD_PARTY)     // SCH与第三方进行互锁
        {
            SchMessage.MsgNumber = Sch_Stop_Msg;
            SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan;
            //SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5 = 0; //
            Schedule_Manager_task::SendMsgToSchedule(uw16_Chan, SchMessage); // 发出SCh命令信息
            //IVSDL::LimitArrivedRoutine(uw16_Chan);  是否需要这一句？  DKQ
        }
        else
        {
            // 				m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = PC_Error_Msg;    //设定回复信息为错误信息
            // 						m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOSCH_FAIL;
            // 				m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
            // 					MPReportData(uw16_Chan,SEND_EVENT_CONTENT);   //向PC发送回复信息
        }

        //关闭校准窗口 170516 zyx
        if(THIRD_PARTY_STATUS_OUTPUT == thirdParty::getStatus(uw16_Chan))
        {
            st_Third_Party_Msg    st_Msg;
            st_Msg.chn = uw16_Chan;
            st_Msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;
            ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].chn_Running_Flag = CHN_LOCK_IDLE;
            thirdParty::UnsafeLockSwitch[uw16_Chan] = 0;  //add by yy20161216
            thirdParty::sendMsgToMe(& st_Msg);
        }
    }
#endif
}
////////
// MPScheduleJump(MP_SCHEDULE_JUMP *pCmd) : 网络命令Schedule跳步命令处理函数
// Input: pCmd 网络Schedule跳步命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleJump(MP_SCHEDULE_JUMP *pCmd)
{
#if  USE_AS_AUXMCU == 0
		unsigned char err_flag = 0;
    ST_SCHEDULE_VARIABLE   m_st_Variable;
    St_ScheduleMessage   SchMessage;
    UWord16 uw16_Chan;
    UChar8  uc_SuccessFlag = CONFIRM_RESULT_FAIL;
//     unsigned char uc_CheckVClamp = 1;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan = pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

#if ADC_CS5560_ENABLE == 0
    if(board_config::Feature_Enable_init.ADC_CS5560_Enable == 1) //
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_ADCTYPE;//MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
        return;
    }
#endif

    if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 != CHN_LOCK_THIRD_PARTY)    // SCH与第三方进行互锁
    {
        ChannelInfo_Manager::m_Online_UpdataEnable[uw16_Chan] = false;

        if((pCmd->m_dwCmd == CMD_SCHEDULE_START || pCmd->m_dwCmd == CMD_SCHEDULE_RESUME) && (OS_ServeProcess::Eth_TimeCalied == 0)) //zyx 181019 SystemTime would not correct without CMD_SET_SYSTEMTIME
        {
            m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
            m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
            m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_TIME_CALI_FAIL;//MP_ERROR_LOCK_TOSCH_FAIL;
            m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
            //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
            MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
            return;
        }

        switch(pCmd->m_dwCmd)
        {
        case CMD_SCHEDULE_JUMP:
        {
            if(pCmd->m_stStart.m_btForceFlag == 1)
                SchMessage.MsgNumber = Sch_JumpM_Msg;
            else
                SchMessage.MsgNumber = Sch_JumpA_Msg;

            pCmd->m_wVariableValueCount = 0; //  保护跳转时不下发变量。

            unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uw16_Chan].m_ucActiveIdx;  // 解决手动跳步后，无法退出ACR步问题。  20151016 yy
            UWord16 uw16_CtrlType = ChannelInfo_Manager::ChannelStep[uw16_Chan].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;
            switch(uw16_CtrlType)
            {
            case CT_IVCH_IR:
            case CT_IVCH_ACIR:
// 					if((((IR::IRChannelFlg>>uw16_Chan)&0x0001) == 0x0001)
// 						||(((IR::IRChannelFlg_CopyInit[0]>>uw16_Chan)&0x0001) == 0x0001)) // 通道标记位检查   // [X]  0:原值 1：过滤值
                if(((IR::IRChannelFlg >> uw16_Chan) & 0x0001) == 0x0001)
                {
                    IR::IRChannelFlg &= ~(1 << uw16_Chan); //去除标记位
// 						IR::IRChannelFlg_CopyInit[0]&=~(1<<uw16_Chan); //去除标记位
// 						IR::IRChannelFlg_CopyInit[1]|=(1<<uw16_Chan); //开放屏蔽位
                    if(IR::IR_TimerEn && (IR::IRChannelNo == uw16_Chan))
                    {
                        IR::IR_TimerEn = false;
                    }
                }
                break;
            default:
                break;
            }
            if(pCmd->m_dwTestID != Schedule_Manager_task::m_dwTestID[uw16_Chan])  //mits8
            {
                MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
                return;
            }
            break;
        }
        case CMD_SCHEDULE_START:
            safety_check::g_dChargeCapacity[uw16_Chan] = 0;
            safety_check::g_dDischargeCapacity[uw16_Chan] = 0;
            Schedule_Manager_task::IsNeedSetStopPoint[uw16_Chan] = true;

            SchMessage.MsgNumber = Sch_Run_Msg;
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM )   //add by dirui 20171101 add alarm for pwm
                PWM_Base::PWM_LEDRunStatus = SYSTEM_RUN;
#if DEBUG_TEST_COMMDATA_MODE == 1
            uw32_TestLogD[uw16_Chan] = 0; //  仅为测试用，成型后要清理丢。
#endif

#if (_PARA_DEBUG_)
            ChannelInfo_Manager::gUnit_stUnitInfo.m_uUnitID;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_eUnitGroupingMode;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_eUnitMode;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_bMasterUnit;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_uMasterUnitID;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_uGroupUnitCount;
#endif
//            ChannelInfo_Manager::m_StepIndexUp[uw16_Chan] = 0XFFFF;
            ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_PV_DataPoints].m_Value.m_uInt32 = 0; //DKQ , when start, logged data point is 0
            if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
            {
                if(PowerAdj::uc_DefaultVoltageFlag ==1)
                {
                    PowerAdj::uc_DefaultVoltageFlag=0;
                    PowerAdj::m_PowerAdjStatus = POWERADJ_IDLE;//POWERADJ_REQUIRE;
                }
            }
//		IVSDL::m_dwStepPoint[uw16_Chan] = 1;
            break;

        case CMD_SCHEDULE_RESUME:
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM )
                PWM_Base::PWM_LEDRunStatus=SYSTEM_RUN;
            SchMessage.MsgNumber = Sch_Resume_Msg;
            Schedule_Manager_task::IsNeedSetStopPoint[uw16_Chan] = true;

            ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_PV_DataPoints].m_Value.m_uInt32 =  pCmd->m_dwDataPoint; //DKQ , when resume, this is the last logged data point
//             char sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_scBufIdx;
//             unsigned short StepCount  = ChannelInfo_Manager::ScheduleBuf[uw16_Chan].m_ItemCount.m_u16StepCount;//该Schedule总步数
//            IVSDL::m_dwStepPoint[uw16_Chan] = pCmd->m_stStart.m_dwLast_CycleID * StepCount + pCmd->m_stStart.m_wLast_StepID;
            ChannelInfo_Manager::SetDataFlag(uw16_Chan, RESUME_POINT); //180309 zyx
//            ChannelInfo_Manager::m_StepIndexUp[uw16_Chan] = 0XFFFF;
            break;

        default:
            SchMessage.MsgNumber = Sch_JumpA_Msg;
            break;
        }
        SchMessage.FromStepID = pCmd->m_stStart.m_wLast_StepID;// 暂时转存数据
        //SchMessage.ToStepID = pCmd->m_stStart.m_wTarget_StepID;
        SchMessage.StepID = pCmd->m_stStart.m_wTarget_StepID;
        SchMessage.FromCycleID = pCmd->m_stStart.m_dwLast_CycleID;
        //SchMessage.ToCycleID = pCmd->m_stStart.m_dwNew_CycleID;
        SchMessage.CycleID = pCmd->m_stStart.m_dwNew_CycleID;
        SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan;
        SchMessage.ForceJumpFlag = pCmd->m_stStart.m_btForceFlag;
        SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5 = pCmd->m_stMPack.m_stMSecondField.m_StMD5Code; //  MD5码数组复制;

        m_ReportStatus.m_wStepID = pCmd->m_stStart.m_wLast_StepID;
        m_ReportStatus.m_dwCycleID = pCmd->m_stStart.m_dwNew_CycleID;
        ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IndependantCtrl.m_ucBoardOperateType = THIRD_PARTY_MANUAL;  // yy 20161216

        //如果不加这句，会导致steptime和testtime对不上的情况
        //陈东20130828
        if(!Schedule_Manager_task::IfSchRunning(uw16_Chan))
        {

// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampHigh = pCmd->m_fVclampHigh;
//			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampLow = pCmd->m_fVclampLow;

//			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampLow = pCmd->m_fVclampLow-20;
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampLow = pCmd->m_fVclampLow;

// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fCurrentSimulationVclampHigh = pCmd->m_fCurrentSimulationVclampHigh;
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fCurrentSimulationVclampLow = pCmd->m_fCurrentSimulationVclampLow;

//	ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_DataType_DigiVclampHigh = pCmd->m_DataType_DigiVclampHigh;   //171016     hpq
//	ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_DataType_DigiVclampLow = pCmd->m_DataType_DigiVclampLow;  //171016     hpq

// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_PclampHighControlType = pCmd->m_PclampHighControlType;  //170609 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fPclampHigh = pCmd->m_fPclampHigh;

// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_PclampLowControlType = pCmd->m_PclampLowControlType; //170609 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fPclampLow  = pCmd->m_fPclampLow;

// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IclampHighControlType = pCmd->m_IclampHighControlType;  //170719 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fIclampHigh = pCmd->m_fIclampHigh;		    //170719 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_IclampLowControlType = pCmd->m_IclampLowControlType; //170719 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fIclampLow  = pCmd->m_fIclampLow;             //170719 zyx
// 			ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucVoltageRange = pCmd->m_btVoltage_Range;

            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucSyncStop = pCmd->m_btSyncStop;  //保存SyncStop信号 zyx   0x01

            switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
            {
//                 case BOARDTYPE_BT2204:
            case BOARDTYPE_LBT22043:
            case BOARDTYPE_LBT22013:
            case BOARDTYPE_LBT22023:
//                 case BOARDTYPE_LBT22024:
//                 case BOARDTYPE_LBT22013FR:
//                 case BOARDTYPE_LBT22044:
                BurstModeSampling::uw16_BurstModeDelayTime[uw16_Chan] = pCmd->uw16_BurstModeDelayTime;      //  PC下载BurstMode延迟时间 1:100us 单位：100us [允许输入0-10000之间] //  2017.6.8  yy
                BurstModeSampling::uw32_BurstModePointNum[uw16_Chan] = pCmd->uw32_BurstModePointNum;       //  PC下载BurstMode打点总数  [允许输入100-1000之间]
                BurstModeSampling::uw32_BurstModeSampleSpeed[uw16_Chan] = pCmd->uw32_BurstModeSampleSpeed;    //  PC下载BurstMode打点速度 1:100us记录一点 n:n*100us记录一点 [允许输入1-10之间]
                break;
            default:
                break;
            }

            if(pCmd->m_dwCmd != CMD_SCHEDULE_JUMP)
            {
                //if(!Schedule_Manager_task::IfSchRunning(uw16_Chan))
                //{
                ChannelInfo_Manager::Init_Metavariable(uw16_Chan); //MV初始化，调大数据体接口

                for(unsigned char data_group_idx = 0; data_group_idx < 2; data_group_idx++) //180825 yy
                {
                    if(!BuiltIn_DataManager::m_DataGroupMgr[data_group_idx])    //no used
                    {
                        continue;
                    }
                    for(unsigned char chn = 0; chn < MAX_AUXCH_PER_DATA_GROUP; chn++)
                    {
                        unsigned char CHN = BuiltIn_DataManager::m_PChn2EChn[data_group_idx][chn].e_data_group_id * 16 + chn;
                        unsigned char BuiltInCnt = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
                        unsigned char IV_ch = CHN / BuiltInCnt;
                        unsigned char BuiltInIdx = CHN % BuiltInCnt;
                        ChannelInfo_Manager::ChannelInfo[IV_ch].m_fBulitInData[BuiltInIdx] = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data;
                        ChannelInfo_Manager::MetaVariable2[IV_ch][MetaCode_PV_BuiltIn_Data1 + BuiltInIdx].m_fValue = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data;
                    }
                }

//                 IVSDL::uc_IR_SendCount[uw16_Chan] = 0;  // 仅当为IR功能时，才向上位机传送IR内阻值。
                IVSDL::uc_ACR_SendCount[uw16_Chan] = 0;  // 仅当为ACR功能时，才向上位机传送ACR内阻值。
                IVSDL::uw32_Interval_4_ETable[uw16_Chan] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;   //  2013/10/23  Yangy 解决无记录点问题。
//				IVSDL::uw32_IntervalTEnergy[uw16_Chan] = OS_ServeProcess::OS_Time.Second;     //  2013/10/23  Yangy 解决无记录点问题。
                //}
                ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_PV_StepIndex].m_fValue = pCmd->m_stStart.m_wTarget_StepID;
                ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_PV_CycleIndex].m_fValue = pCmd->m_stStart.m_dwNew_CycleID;
                Schedule_Manager_task::m_dwTestID[uw16_Chan] = pCmd->m_dwTestID;  //mits8
            }

            //ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan,MetaCode_MV_NorminalCapacity,pCmd->m_fCapacity);
// 			ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan,MetaCode_MV_UD1,pCmd->m_fUD1);
// 			ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan,MetaCode_MV_UD2,pCmd->m_fUD2);
// 			ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan,MetaCode_MV_UD3,pCmd->m_fUD3);
// 			ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan,MetaCode_MV_UD4,pCmd->m_fUD4);
            for(int index = 0; index < MAX_UD; index++) ////1807131 zyx
            {
                if(index < 4)
                    ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan, (MetaCode_MV_UD1 + index), pCmd->m_fUD[index]);
                else
                    ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan, (MetaCode_MV_UD5 + (index - 4)), pCmd->m_fUD[index]);
            }
            //ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fCapacity= pCmd->m_fCapacity;   //  电池额定容量
            UWord16 uw16_MetaVariablesCount = pCmd->m_wVariableValueCount;
            if(uw16_MetaVariablesCount >= 200) // 1000/4=250 最大保护值
                uw16_MetaVariablesCount = 200;

            for(UWord16 uw16_Count = 0; uw16_Count < uw16_MetaVariablesCount; uw16_Count ++)
            {   // 标(额)定电压
                m_st_Variable.m_wVariable_Code  = pCmd->m_mvReset[uw16_Count].m_wVariable_Code;
                m_st_Variable.m_fVariable_Value = pCmd->m_mvReset[uw16_Count].m_fVariable_Value;
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan, m_st_Variable.m_wVariable_Code, m_st_Variable.m_fVariable_Value);

                switch(m_st_Variable.m_wVariable_Code)
                {
                case MetaCode_PV_Steptime:
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeStepTime = m_st_Variable.m_fVariable_Value;
                    IVSDL::uw32_Interval_4_ETable[uw16_Chan] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;   //  2013/10/23  Yangy 解决无记录点问题。
                    //IVSDL::uw32_IntervalTEnergy[uw16_Chan] = OS_ServeProcess::OS_Time.Second;     //  2013/10/23  Yangy 解决无记录点问题。
                    break;
                case MetaCode_PV_TestTime:
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeTestTime = m_st_Variable.m_fVariable_Value;
                    IVSDL::uw32_Interval_4_ETable[uw16_Chan] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;   //  2013/10/23  Yangy 解决无记录点问题。
//					IVSDL::uw32_IntervalTEnergy[uw16_Chan] = OS_ServeProcess::OS_Time.Second;     //  2013/10/23  Yangy 解决无记录点问题。
                    break;
                case MetaCode_PV_InternalResistance:
//                     IVSDL::uc_IR_SendCount[uw16_Chan] = 1;  // 仅当为IR功能时，才向上位机传送IR内阻值。
                    break;
                case MetaCode_PV_ACR:
                    IVSDL::uc_ACR_SendCount[uw16_Chan] = 1;  // 仅当为ACR功能时，才向上位机传送ACR内阻值。
                    break;
                case MetaCode_LS_InternalResistance: //zyx 210513  Lost IR value after resume.
                    ChannelInfo_Manager::Write_SpecifiedMetavariable2(uw16_Chan, MetaCode_PV_InternalResistance, m_st_Variable.m_fVariable_Value);
                    break;
                case MetaCode_TC_Timer1://2111011 zyx
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeTC_Timer1 = m_st_Variable.m_fVariable_Value;
                    break;
                case MetaCode_TC_Timer2:
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeTC_Timer2 = m_st_Variable.m_fVariable_Value;
                    break;
                case MetaCode_TC_Timer3:
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeTC_Timer3 = m_st_Variable.m_fVariable_Value;
                    break;
                case MetaCode_TC_Timer4:
                    ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fResumeTC_Timer4 = m_st_Variable.m_fVariable_Value;
                    break;								
                default:
                    break;
                }
            }

// 			if(ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_MV_NorminalCapacity].m_fValue == 0)
// 			{
// 				ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_MV_NorminalCapacity].m_fValue =
// 					ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_MV_Mass].m_fValue *
// 					ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_MV_SpecificCapacity].m_fValue;
// 			}//171113,hpq,add for pro8

            //chen 20130906
            ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_SCH;
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].chn_Running_Flag = CHN_LOCK_SCH;

            memcpy(&ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_TestObject, &pCmd->m_datTestObject, sizeof(St_SchTestObject)); //1804201 zyx
//             Channel_MapAuxInfo::InitAuxMap(uw16_Chan); //190114 zyx
        }

        if(pCmd->m_dwCmd == CMD_SCHEDULE_JUMP)   //  20130828  yangying   解决多次向SCH管理发送信息问题
        {
//                ChannelInfo_Manager::m_StepIndexUp[uw16_Chan] = SchMessage.FromStepID;   // 04.27.2018  yy   //To Protect Cycle Add 1.
            if(ChannelInfo_Manager::CheckJumpStep_ToProtectCycle(uw16_Chan, SchMessage.FromStepID, SchMessage.StepID))
                SchMessage.CycleID ++; // add 1 to jump setvariable.
            m_ReportStatus.m_dwCycleID = SchMessage.CycleID;
            Schedule_Manager_task::SendMsgToSchedule(uw16_Chan, SchMessage); // 发出SCh命令信息
        }
    }
    else
    {
        m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_ETHERNETPARSE;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOCK_TOSCH_FAIL;
        m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = 0xFF;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber = uw16_Chan;  //回复的通道号
        MPReportData(uw16_Chan, SEND_EVENT_CONTENT);  //向PC发送回复信息
    }
    if((pCmd->m_dwCmd == CMD_SCHEDULE_START) || (pCmd->m_dwCmd == CMD_SCHEDULE_RESUME)) //  20130822
    {
// #if  USE_AS_AUXMCU == 0
//         if(!Schedule_Manager_task::IfSchRunning(uw16_Chan))//191217 zyx
//             memset(&ChannelInfo_Manager::MetaVariable2[uw16_Chan][MetaCode_PV_Error_IV].m_fValue,0,sizeof(st_GetMetaValue)*6);  // MetaCode_PV_Error_IV---MetaCode_PV_Error_INVERTER
// #endif

// 		float voltage = ChannelInfo_Manager::Get_MetaVariable(uw16_Chan, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage);	// Get Voltage
// 		uc_CheckVClamp = safety_check::VClampRangeCheck(uw16_Chan,voltage);
// 		if(!uc_CheckVClamp)   // 增加提前检查所设置的钳压是否正常 yy  20160216
// 			EthernetParse::SendTasksMsgProcess(uw16_Chan, ST_ETHERNETPARSE, VClampUnsafe ,voltage); //钳压异常返回

//     const St_SchTestObject TestObject = ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_TestObject; //180725 zyx //180825 zyx
//
// 	ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucVoltageRange= ChannelInfo_Manager::GetMatchedVoltageRange(uw16_Chan,TestObject.m_fMaxVoltageCharge);
//
// 	ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampHigh = TestObject.m_fVoltClampHigh;
// 	ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_fVclampLow = TestObject.m_fVoltClampLow;
//   float voltage = ChannelInfo_Manager::Get_MetaVariable(uw16_Chan, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage);	// Get Voltage
//   unsigned char uc_CheckVClamp = safety_check::VClampRangeCheck(uw16_Chan,voltage);
//     if(!uc_CheckVClamp)   // 增加提前检查所设置的钳压是否正常 yy  20160216
//     {
//         EthernetParse::SendTasksMsgProcess(uw16_Chan, ST_ETHERNETPARSE, VClampUnsafe ,voltage); //钳压异常返回
//     }


        Schedule_Manager_task::bMetaSyncSend[uw16_Chan] = 0;
        SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan; // 填写一些参数以便传送下一层处理
        SchMessage.m_Message.m_Assign.m_ucMD5Count = 1;
        SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_bOk = 0;
        SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_ucType = MD5CODE_TYPE_SCHEDULE;

        m_st_Assign = & SchMessage.m_Message.m_Assign;  // 转存地址   //20130823

        uc_SuccessFlag = Schedule_Manager_task::SchAssignAddStarRoutine(m_st_Assign);//将ChannelSchedule[]和ScheduleBuf[]内容相关联，将Channel和Schedule绑定
				
        unsigned char uc_CheckVClamp = 1;
        unsigned char TCCresult = 1;
				
        if(uc_SuccessFlag == ASSIGNOK)
        {

            TCCresult = (ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_Settings.m_bUseGroupTChamber || safety_check::TemperatureChamberCheck(uw16_Chan));//判断是否需要开启温控箱 zyx20160713
            if(!TCCresult )
						{
                EthernetParse::SendTasksMsgProcess(uw16_Chan, ST_ETHERNETPARSE, TemperatureChamberUnsafe, 0);  //温控箱多通道复用出错返回
								err_flag = 1;
						}

            ChannelInfo_Manager::LoadScheduleClamp(uw16_Chan); //180825 zyx
            const St_SchTestObject TestObject = ChannelInfo_Manager::ChannelSchedule[uw16_Chan].m_TestObject; //180725 zyx
            ChannelInfo_Manager::ChannelInfo[uw16_Chan].m_ucVoltageRange = ChannelInfo_Manager::GetMatchedVoltageRange(uw16_Chan, TestObject.m_fMaxAllowVoltage);
            float voltage = ChannelInfo_Manager::Get_MetaVariable(uw16_Chan, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);	// Get Voltage
            uc_CheckVClamp = safety_check::VClampRangeCheck(uw16_Chan, voltage);
            if(!uc_CheckVClamp)   // 增加提前检查所设置的钳压是否正常 yy  20160216
            {
								if(!err_flag)
								{								
									EthernetParse::SendTasksMsgProcess(uw16_Chan, ST_ETHERNETPARSE, VClampUnsafe, voltage);  //钳压异常返回
									err_flag = 1;
								}
            }

            if(!uc_CheckVClamp || !TCCresult )
                uc_SuccessFlag = ASSIGNFAIL;
        }//220712 zyx
        ChangeAssignResult(uc_SuccessFlag, m_st_Assign);
        m_pFeedBack->m_Assign_Feedback.m_st_Assign = SchMessage.m_Message.m_Assign; // 转存数据

        if((uc_SuccessFlag == ASSIGNOK) || (uc_SuccessFlag == SCHEDULEUNSAGE)) // 20140127 di 保护报警
        {
            Channel_MapAuxInfo::InitAuxMap(uw16_Chan); //190118 zyx
//            ChannelInfo_Manager::CheckJumpStep_ToProtectCycle(uw16_Chan, SchMessage.StepID);
            Schedule_Manager_task::SendMsgToSchedule(uw16_Chan, SchMessage); // 发出SCh命令信息

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
            if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
            {
                M0RedundantFunc::m_bClampValid[uw16_Chan] = true;
                M0RedundantFunc::SetRedundantSafety(uw16_Chan, eType_Voltage);
            }
//#endif
        }

        UWord16 uw16_Length = MPScheduleAssignFeedback(&m_pFeedBack->m_Assign_Feedback);
        uw16_Length += (sizeof(MP_ASSIGN_FEEDBACK)   // 实际长度=总结构数-全部未填数组+已经填写数据数组;
                        - (sizeof(St_MD5Assign) * IV_MAX_ASSIGN_MD5_NUMBER));  // 去丢过多的数组成员数据
        SendFeedBack(MPCS_ACK, uw16_Length);  //  不同于普通单包或多包结果回复

        if(m_pFeedBack->m_Assign_Feedback.m_st_Assign.m_MD5Assign[0].m_bOk == CONFIRM_RESULT_FAIL)
				{
						if(!err_flag)
						{					
								Schedule_Manager_task::SchReportStartScheduleUnsafeMsgToPC(uw16_Chan, \
											&Schedule_Manager_task::StartScheduleUnsafe[uw16_Chan]);
						}
				}
				
        return;
    }

// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPScheduleAssign(MP_SCHEDULE_ASSIGN *pCmd) : 网络命令Schedule指派命令处理函数
// Input: pCmd 网络Schedule指派命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleAssign(MP_SCHEDULE_ASSIGN *pCmd)
{
#if  USE_AS_AUXMCU == 0
    //St_ScheduleMessage   SchMessage;
    //UWord16 uw16_Chan;
    UChar8  uc_SuccessFlag = CONFIRM_RESULT_FAIL;

    pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber = pCmd->m_st_Assign.m_u16ChannelIdx;   // 20140127 yangy  增加保护保持一致通道号
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField; //  MD5码数组复制
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
    //m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
    //m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    //uw16_Chan =  pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;
    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;

    //SchMessage.MsgNumber = Sch_Assign_Msg;
    //SchMessage.m_Message.m_Assign.m_u16ChannelIdx = uw16_Chan_Idx;
    //SchMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5 = pCmd->m_stMPack.m_stMSecondField.m_StMD5Code; //  MD5码数组复制;
    pCmd->m_st_Assign.m_u16ChannelIdx = pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;
    m_st_Assign = & pCmd->m_st_Assign;  // 转存地址   //20130823

// #if  USE_AS_AUXMCU == 0
//     if(!Schedule_Manager_task::IfSchRunning(pCmd->m_st_Assign.m_u16ChannelIdx))//191217 zyx
//         memset(&ChannelInfo_Manager::MetaVariable2[pCmd->m_st_Assign.m_u16ChannelIdx][MetaCode_PV_Error_IV].m_fValue,0,sizeof(st_GetMetaValue)*6);  // MetaCode_PV_Error_IV---MetaCode_PV_Error_INVERTER
// #endif

    uc_SuccessFlag = Schedule_Manager_task::SchAssignAddStarRoutine(m_st_Assign);
    ChangeAssignResult(uc_SuccessFlag, m_st_Assign);

    //SchMessage.m_Message.m_Assign = pCmd->m_st_Assign; // 转存数据

// 	if(uc_SuccessFlag == ASSIGNOK)
// 	{
// 		m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 		//Schedule_Manager_task::SendMsgToSchedule(uw16_Chan,SchMessage); // 发出SCh命令信息
// 	}

    if(uc_SuccessFlag != ASSIGNOK)
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;

    //MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
#endif
}

////////
// MPScheduleAssignReport(UWord16 uw16_ChannelNumber, MP_CONFIRM_ASSIGN *pReport)
// : 网络命令第一信道报告Schedule指派信息处理函数
// Input: uw16_ChannelNumber 网络第一信道报告Schedule指派信息通道号;
//        pReport 网络第一信道报告Schedule指派信息结构指针;
// Return: UWord16
///////
UWord16 EthernetParse::MPScheduleAssignFeedback(MP_ASSIGN_FEEDBACK *pFeedback)
{
    UWord16 uw16_Length;
#if  USE_AS_AUXMCU == 0
    uw16_Length = sizeof(St_Assign) - sizeof(St_MD5Assign) * IV_MAX_ASSIGN_MD5_NUMBER;
    uw16_Length += sizeof(St_MD5Assign) * m_st_Assign->m_ucMD5Count;
    pFeedback->m_dwCmd = CMD_SCHEDULE_ASSIGN_FEEDBACK;
    if(m_ResultFeedback.m_stMSecondField.m_wChannelNumber >= MAXCHANNELNO)
        m_ResultFeedback.m_stMSecondField.m_wChannelNumber %= MAXCHANNELNO; // 通道号最大值保护

    // 	memcpy(&pFeedback->m_stMSecondField.m_StMD5Code.m_ucData[0], &m_st_Assign->m_MD5Assign[0].m_MD5.m_ucData[0], sizeof(St_MD5));
    // 	pFeedback->m_stMSecondField.m_wChannelNumber = m_st_Assign->m_u16ChannelIdx;
    // 	pFeedback->m_stMSecondField.m_wTotalChannelNumber = 1;

    memcpy((void *) &pFeedback->m_st_Assign, m_st_Assign, uw16_Length);	 // 传递信息

    //memset((void *) &pReport->m_stMSecondField, 0, sizeof(ST_MultPack_SecondField));
    memcpy(&pFeedback->m_stMSecondField.m_StMD5Code.m_ucData[0], &m_st_Assign->m_MD5Assign[0].m_MD5.m_ucData[0], sizeof(St_MD5));
    pFeedback->m_stMSecondField.m_wChannelNumber = m_st_Assign->m_u16ChannelIdx;
    pFeedback->m_stMSecondField.m_wTotalChannelNumber = 1;

    uw16_Length = (sizeof(St_MD5Assign) * m_st_Assign->m_ucMD5Count);  // 实际数组成员数据
#endif
    return (uw16_Length);
}
/////////////////////////////////////////////////////////////////////////////////////////
// MPSimulationAssign(MP_SIMULATION_ASSIGN *pCmd) :
// Input: pCmd
// Return: N/A
/////////////////////////////////////////////////////////////////////////////////////////
void EthernetParse::MPSimulationAssign(MP_SIMULATION_ASSIGN *pCmd) //12.22.2013ADD
{
#if  USE_AS_AUXMCU == 0

    //St_ScheduleMessage   SchSimuMessage;

    UWord16 uw16_Chan;

    UChar8  uc_SuccessFlag = CONFIRM_RESULT_FAIL;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;
    TotalNumberCountProcess(& pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber,
                            & pCmd->m_stMPack.m_stMSecondField.m_wTotalChannelNumber, & uc_Total_Flag);

// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;

// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_SUCCESS);

    uw16_Chan =  pCmd->m_stMPack.m_stMSecondField.m_wChannelNumber;

    m_ResultFeedback.m_stMSecondField = pCmd->m_stMPack.m_stMSecondField;

    //SchSimuMessage.MsgNumber = SIMU_ASSIGN_MSG; //12.22.2013ADD

    m_simu_MD5Assign = & pCmd->m_simu_MD5Assign;

    uc_SuccessFlag = Schedule_Manager_task::SimulationAssignAddStarRoutine(uw16_Chan, m_simu_MD5Assign); //12.22.2013ADD 03.05.2014

    ChangeSimulationAssignResult(uc_SuccessFlag, m_simu_MD5Assign); //12.22.2013ADD  //03.05.2014

    //SchSimuMessage.m_Message.m_MD5Assign = pCmd->m_simu_MD5Assign;   //12.22.2013ADD

// 	if(uc_SuccessFlag == ASSIGNOK)
// 	{
// 		m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
// 		//Schedule_Manager_task::SendMsgToSchedule(uw16_Chan,SchSimuMessage); //12.22.2013ADD
// 	}

    if(uc_SuccessFlag != ASSIGNOK)
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;

#endif
}

///////////////////////////////////////////////////////////////////////////////
// MPSimulationAssignFeedback(MP_SIMULATION_ASSIGN_FEEDBACK *pFeedback):
// Input:   pFeedback
// Return: UWord16
/////////////////////////////////////////////////////////////////////////////
UWord16 EthernetParse::MPSimulationAssignFeedback(MP_SIMULATION_ASSIGN_FEEDBACK *pFeedback)  //12.22.2013ADD
{
    UWord16 uw16_Length;
#if  USE_AS_AUXMCU == 0

    uw16_Length = sizeof(St_MD5Assign);

    pFeedback->m_dwCmd = CMD_SIMULATION_ASSIGN_FEEDBACK;

    memcpy((void *) &pFeedback->m_simu_MD5Assign, m_simu_MD5Assign, uw16_Length);

    uw16_Length = sizeof(MP_SIMULATION_ASSIGN_FEEDBACK);
#endif
    return (uw16_Length);
}

////////
// ChangeAssignResult(unsigned char uc_SuccessFlag,St_Assign *Assign) : 从Schedule管理结果转换通信错误代码
// Input: Assign :Schedule指派结构指针;
// Return: N/A
///////
void EthernetParse::ChangeAssignResult(unsigned char uc_Flag, St_Assign *Assign)
{
#if  USE_AS_AUXMCU == 0
    if(uc_Flag == ASSIGNOK)
    {
        for(UChar8 uc_i = 0; uc_i < Assign->m_ucMD5Count; uc_i++)
        {
            Assign->m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS;//  调试用，以后将有SCH直管理
            Assign->m_MD5Assign[uc_i].m_bError = MP_CONFIRM_ERROR_UNKNOWN;
        }
    }
    else
    {
        switch(uc_Flag)  // 从SCH管理接口转换成通信接口，进行转换操作。
        {
        case ASSIGNFAIL:
            uc_Flag = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
            break;
        case SCHEDULERUNNING:
            uc_Flag = MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULERUNNING;  // 配置错误代码
            break;
        case SCHEDULEUNSAGE:
            uc_Flag = MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULEUNSAGE;  // 配置错误代码
            break;
        default:
            uc_Flag = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码
            break;
        }

        for(UChar8 uc_i = 0; uc_i < Assign->m_ucMD5Count; uc_i++)
        {
            Assign->m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_FAIL;//  调试用，以后将有SCH直管理
            Assign->m_MD5Assign[uc_i].m_bError = uc_Flag;
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
// ChangeSimulationAssignResult(unsigned char uc_Flag,St_MD5Assign *Assign) :
// Input: uc_Flag, Assign
// Return: N/A
////////////////////////////////////////////////////////////////////////////////////////////
void EthernetParse::ChangeSimulationAssignResult(unsigned char uc_Flag, St_MD5Assign *Assign) //12.22.2013ADD
{
#if  USE_AS_AUXMCU == 0
    if(uc_Flag == ASSIGNOK)
    {

        Assign->m_bOk = CONFIRM_RESULT_SUCCESS;
        Assign->m_bError = MP_CONFIRM_ERROR_UNKNOWN;

    }
    else
    {
        switch(uc_Flag)
        {
        case ASSIGNFAIL:
            uc_Flag = MP_CONFIRM_ERROR_UNKNOWN;
            break;
        case SCHEDULERUNNING:
            uc_Flag = MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULERUNNING;
            break;
        case SCHEDULEUNSAGE:
            uc_Flag = MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULEUNSAGE;
            break;
        default:
            uc_Flag = MP_CONFIRM_ERROR_UNKNOWN;
            break;
        }


        Assign->m_bOk = CONFIRM_RESULT_FAIL;
        Assign->m_bError = uc_Flag;

    }
#endif
}

////////
// MPScheduleAckEvent(MP_SCHEDULE_ASSIGN *pCmd) : 网络命令Schedule确认事件号命令处理函数
// Input: pCmd 网络Schedule确认事件号命令结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleAckEvent(MP_SCHEDULE_ACK_EVENT *pCmd)
{

//     UChar8 uc_DownLoad_Success = 0;


// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

    /*	if(pCmd->m_stMFirstField.m_wPackNo == 0)
    {
    st_MultPackManage.m_stMFirstField = pCmd->m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
    }
    else
    {
    if(pCmd->m_stMFirstField.m_wPackNo == st_MultPackManage.m_stMFirstField.m_wPackNo )
    {
    // 判断是否为同一个多包的内容。
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_PACK_RESEND;  // 配置错误代码
    MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
    return;
    }
    st_MultPackManage.m_stMFirstField.m_dwPackLength += pCmd->m_stMFirstField.m_dwPackLength; // 非首包为累加值
    }

    st_MultPackManage.m_stMFirstField.m_wPackNo = pCmd->m_stMFirstField.m_wPackNo;
    st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress;

    if(st_MultPackManage.m_stMFirstField.m_wPackTotal == (st_MultPackManage.m_stMFirstField.m_wPackNo + 1))
    {
    // 判断是否为同一个多包的内容接收完成。
    uc_DownLoad_Success = 1;
    m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;
    }

    // 存包中的SCH数据
    memcpy(&m_btPackDataBuffer[pCmd->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress], &pCmd->m_btPackData_N, pCmd->m_stMPack.m_stMFirstField.m_dwPackLength); // 存SCH数据
    */
//     uc_DownLoad_Success = 1;
//     if(uc_DownLoad_Success == 1)  //  使用区域判断  0：单包或无意义 1：第一区域多包信息使用标志
//     {
    // 已经接收完成
    for(UWord16 uw16_i = 0; uw16_i < pCmd->m_btTotal; uw16_i ++)
    {
        // 比较重发事件缓冲中的确认编号，如果有相同的清除编号同时清除发送包。
        if(pCmd->m_dwRcvdSN[uw16_i] == 0)
            continue;

        EventsBuffering::ConfirmEvent(pCmd->m_dwRcvdSN[uw16_i]);
    }
//     }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    //MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}



void EthernetParse::MPBatchGrouping(MP_BATCH_GROUP *pCmd, unsigned char uc_Write_Flag)
{
#if USE_AS_AUXMCU == 0
    unsigned short uGroupCounter;
    //unsigned short uInterParaIdx;
    uGroupCounter = pCmd->m_GroupCounter;
    if((uGroupCounter == 0) || (uGroupCounter > MAXCHANNELNO))
        return;
    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
    {
//		ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus=IDLE;	  //改变组状态，所有通道强制复位Schedule状态
        safety_check::Process_SM_Ptr[uc_i] = SAFE;
    }

    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount = uGroupCounter;
    for(unsigned char uc_i = 0; uc_i < uGroupCounter; uc_i++) //按照组解析
    {
        ST_UNIT_GROUP_INFO * p_Grp_Info = &ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_i].m_UnitGroupInfo;

        *p_Grp_Info = pCmd->m_UnitGroupInfo[uc_i];

        if(p_Grp_Info->m_Supervisor.m_Unit_Idx > 0)
            p_Grp_Info->m_Supervisor.m_Unit_Idx--;

        if(p_Grp_Info->m_uGroupUnitCount == 0)
            p_Grp_Info->m_Supervisor.m_Unit_Idx = ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID;

        if(p_Grp_Info->m_WholeGroupSize == 0)
            p_Grp_Info->m_WholeGroupSize = 1;

        for(unsigned char uc_j = p_Grp_Info->m_HeadChn; \
                uc_j < p_Grp_Info->m_HeadChn +  \
                p_Grp_Info->m_InUnitGroupSize; uc_j++)
            ChannelInfo_Manager::ChannelInfo[uc_j].m_GroupID = uc_i; //注册组编号到每个通道
        if(p_Grp_Info->m_uGroupUnitCount > 0)
        {
            for(unsigned char cmdIdx = 0; cmdIdx < CAN_CMD_PARA_MAX; cmdIdx++)
            {
                for(unsigned char bufIdx = 0; bufIdx < PARA_MAX_BUF; bufIdx++)
                {
                    CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_MasterID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_i].m_uUnitID;
                    CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].CmdStatus = eParaAckNone;
                    CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_SubUnitCnt = p_Grp_Info->m_uGroupUnitCount;
                    for(unsigned char Idx = 0; Idx < MAXUNITNO; Idx++)
                        CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_SubStatus[Idx] = eParaAckNone;
                }
            }
        }
    }
    unsigned char ParState_Previous[MAXCHANNELNO];            //161028
    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)  //161028
        ParState_Previous[uc_i] = IVSDL::m_GrpInfo4SDL[uc_i].State;   //161028
    ChannelInfo_Manager::InitGroup4SDL(pCmd);
    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)  //161028
    {   //161028
        ChannelInfo_Manager::ChannelInfo[uc_i].EngineStatus = 0; //zyx 170719

        if( ((ParState_Previous[uc_i] == eGroupState_Slave) || (ParState_Previous[uc_i] == eGroupState_Submaster))
                && ((IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Normal) || (IVSDL::m_GrpInfo4SDL[uc_i].State == eGroupState_Master))
                && (ChannelInfo_Manager::ChannelInfo[uc_i].chn_State == LOGIC_PROCESS_CHN_OPEN ) )
        {
            Step_Manager_task::PrepareForStop(uc_i);
// 	    ChannelInfo_Manager::ChannelInfo[uc_i].EngineStatus=0;
            ChannelInfo_Manager::ChannelInfo[uc_i].StepTransationToStatus = STEPIDLE;
            ChannelInfo_Manager::ChannelInfo[uc_i].StepStatus = STEPTRANSITION;
            Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_i]));
        }
    }//161028 end
    if(uc_Write_Flag)
    {
        unsigned char uc_ReadData[MAX_BATCH_GROUP_BUFFER];   // BATCH PARALLEL GROUP 636字节
        unsigned char uc_updata = 0;
        unsigned char * uc_psave = (unsigned char *)pCmd;
        if(BuiltIn_DataManager::m_Sample_Flag)
        {
            BuiltIn_DataManager::m_WriteFRAM_Flag = true;
            while(BuiltIn_DataManager::m_Sample_Flag);
        }
        CConfig::ReadContent_ForFRAM(&uc_ReadData[0], FRAM_PARALLEL_BASE, MAX_BATCH_GROUP_BUFFER);
        uc_updata = memcmp(&uc_ReadData[0], uc_psave, MAX_BATCH_GROUP_BUFFER); // 比对数据如果不同则存一次,相同跳过.
        if(uc_updata)
            CConfig::WriteContent_ForFRAM(uc_psave, FRAM_PARALLEL_BASE, MAX_BATCH_GROUP_BUFFER);
        if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
        {
            BuiltIn_DataManager::m_Sample_Flag = true;
            BuiltIn_DataManager::m_WriteFRAM_Flag = false;
        }
    }


    ST_UNIT_GROUP_INFO * p_Grp_Info_First = &ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo;
    ST_UNIT_GROUP_INFO * p_Grp_Info_Last = &ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount - 1].m_UnitGroupInfo;

    if((ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)\
            && (p_Grp_Info_First->m_WholeGroupSize == 1)   \
            && (p_Grp_Info_Last->m_WholeGroupSize == 1))

    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode = eSingleMode;
        CAN_WatchdogTask::MasterUnit_beNeedWatchdogflg = 0;
        CAN_WatchdogTask::m_CanWatchdogSubMasterUnitACKStatus = CAN_WATCHDOG_SUBUNIT_IDLE;
    }
    else if ((p_Grp_Info_First->m_WholeGroupSize == p_Grp_Info_First->m_InUnitGroupSize)   \
             && (p_Grp_Info_Last->m_WholeGroupSize ==	\
                 p_Grp_Info_Last->m_InUnitGroupSize))
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode = eIntraMode;
        CAN_WatchdogTask::MasterUnit_beNeedWatchdogflg = 0;
        CAN_WatchdogTask::m_CanWatchdogSubMasterUnitACKStatus = CAN_WATCHDOG_SUBUNIT_IDLE;
    }
    else if ((p_Grp_Info_First->m_WholeGroupSize > p_Grp_Info_First->m_InUnitGroupSize)   \
             || (p_Grp_Info_Last->m_WholeGroupSize >  \
                 p_Grp_Info_Last->m_InUnitGroupSize))
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode = eInterMode;
        CAN_WatchdogTask::MasterUnit_beNeedWatchdogflg = 1;
        CAN_WatchdogTask::m_CanWatchdogMasterUnitACKStatus = CAN_WATCHDOG_MASTERUNIT_IDLE;
        CAN_WatchdogTask::m_CanWatchdogSubMasterUnitACKStatus = CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN;

        if(p_Grp_Info_First->m_HeadChnRank == 0)
            ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID = 0;
        else
            ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID = 0;


        if(p_Grp_Info_Last->m_HeadChnRank == 0)
            ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount - 1;
        else
            ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount - 1;
    }

    for(unsigned char uc_i = 0; uc_i < uGroupCounter; uc_i++)
    {
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_i].m_UnitGroupInfo.m_InUnitGroupSize > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_i].m_UnitGroupInfo.m_InUnitGroupSize = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
    }
    //	p_Grp_Info_0->m_uGroupUnitCount=2;
    //	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[6].m_UnitGroupInfo.m_SubMasterChannels[0].m_Unit_Idx=1;
    //	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[6].m_UnitGroupInfo.m_SubMasterChannels[0].m_Chn_Idx=0;
    //IVSDL::Valid = true;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter = 0;
    for(unsigned char uc_i  = 0; uc_i < MAX_UNIT_PER_GROUP; uc_i++) //记录series和booster控制电压
    {
        ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_Unit_Idx = 0;
        ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_beSlaveUnit = 0;
    }
    for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount - 1; uc_i++) //同步所有的SubMasterUnit
    {
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit >= eSeriesBoosterRank_Serial)
        {
            ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode = eMixMode;
            ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter++;
        }
    }

    unsigned long int *  u32_IvRegister;
    for(unsigned short int uw16_n = 0; uw16_n <  MCU_MAX_NUMBER; uw16_n++)// yzl 2020.10.29
    {
        u32_IvRegister = &MultiMcuComm::St_Unit[uw16_n].u32_Parallel_IvRegister;
        *u32_IvRegister = 0;
    }

    MultiMcuComm::Scan_IV_ParallelMAP();
    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
    {
        unsigned char uc_Clear = 0;
        if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())    //  04.28.2018 yy  解决并联解除乱报超时问题，及单通道无法再次运行问题。
        {
            if((!Schedule_Manager_task::IfSchRunning(uc_i))
                    &&(!Step_Manager_task::IfParaModStepRunning(uc_i))) //20191230 yy
                uc_Clear = 1;
        }
        else if(!Schedule_Manager_task::IfSchRunning(uc_i))
            uc_Clear = 1;

        if(uc_Clear)
        {
            Step_Manager_task::bNeedCheck[uc_i] = 0;
            ChannelInfo_Manager::ChannelStep[uc_i].m_LimitCount = 0;
//          ChannelInfo_Manager::ChannelInfo[uc_i].StepStatus = STEPIDLE;
// 			ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus = IDLE;	 //改变组状态，所有通道强制复位Schedule状态
            St_ScheduleMessage   SchMessage;
            SchMessage.MsgNumber = Sch_Stop_Msg;
            Schedule_Manager_task::SendMsgToSchedule(uc_i, SchMessage); // 发出SCh命令信息  //改变组状态，所有通道强制复位Schedule状态
            St_StepMessage StepMessage;
            StepMessage.MsgNumber = Step_Stop_Msg;
            Step_Manager_task::SendMsgToStep(uc_i, StepMessage);  //发送Step停止信息；  //20191230 yy
        }

    }

//     for(UChar8 uc_idx = 0; uc_idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_idx++) //zyx 181015
//     {
//         ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SCH_SAFTY_TYPE_CURRENT, ChannelInfo_Manager::CurrRngPhysicalRange1, & safety_check::Check_S[uc_idx].checkSYS_I); //170320,add by hepeiqing
//         ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SCH_SAFTY_TYPE_VOLTAGE, VOLTAGE_RANGE_HIGH, & safety_check::Check_S[uc_idx].checkSYS_V);
//         ChannelInfo_Manager::GetSystemSafty(uc_idx, MP_SCH_SAFTY_TYPE_POWER, 0, & safety_check::Check_S[uc_idx].checkSYS_P);
//     }
#endif
}



////////
// MPReportData(UWord16 uw16_ChannelNumber,unsigned char uc_SendType)
// : 网络命令第二信道报告信息接口处理函数
// Input: uw16_ChannelNumber 网络第二信道报告信息通道号;
//        uc_SendType 网络第二信道报告信息类型;
// Return: N/A
///////
void EthernetParse::MPReportData(UWord16 uw16_ChannelNumber, unsigned char uc_SendType)
{
    //  网络第二信道"异步信道"数据发送主函数。
    //UWord16 uw16_i;
    UChar8   uc_Temp;

#if  USE_AS_AUXMCU == 0
    //if(IVSDL::NeedSDL(uw16_ChannelNumber))
    if(IVSDL::Valid)  //  SDL::Valid实际就是(USE_SDL)
    {
        if (uc_SendType == SEND_EVENT_CONTENT)
            uc_SendType = SEND_EVENT_SDL;
    }
    else
    {
        if(uc_SendType == SEND_EVENT_SDL)
            uc_SendType = SEND_EVENT_CONTENT;
    }
#endif

    switch(uc_SendType)
    {
        // 1、事件信息内容单包发送
#if  USE_AS_AUXMCU == 1
    case SEND_AUX_ACIMDATA:
#endif
    case SEND_EVENT_CONTENT:
    case SEND_METAVARIABLESYNCH:
    case SEND_MVUDSYNCH:
    //case SEND_REPORT_ASSIGN:
    case SEND_REQUEST_SYSTEMTIME:
    case SEND_EVENT_SDL:
#if DEBUG_TEST_COMMDATA_MODE == 1
        uw32_TestEventNum++; // 计算已发信息号 //  仅为测试用，成型后要清理丢。
#endif

        if(++m_dwSend_SN == 0)  //  发送事件编号自动增加
            m_dwSend_SN = 1;

        switch(uc_SendType)
        {
        case SEND_EVENT_CONTENT:
            MPScheduleReport_Event(uw16_ChannelNumber, uc_SendType, &m_pReport->m_Report_Event, &EventTx_Length);
            break;

// 		case SEND_REPORT_ASSIGN:
// 			//MPScheduleReport_Assign(uw16_ChannelNumber, &m_pReport->m_Report_Assign, &EventTx_Length);
// 			break;
        case SEND_MVUDSYNCH:
            MPScheduleReport_MVUDSynch(uw16_ChannelNumber, &m_pReport->m_Report_Meta, &EventTx_Length);
            break;
        case SEND_METAVARIABLESYNCH:
            MPScheduleReport_MetaVariableSynch(uw16_ChannelNumber, &m_pReport->m_Report_Meta, &EventTx_Length);
            break;

// 		case SEND_REQUEST_SYSTEMTIME:
// 			MP_Request_SystemTime( &m_pReport->m_Request_SystemTime, &EventTx_Length);
// 			break;

        case SEND_EVENT_SDL:
            MPScheduleReport_EventSDL(uw16_ChannelNumber, uc_SendType, &m_pReport->m_Report_Event_SDL, &EventTx_Length);
            break;

#if  USE_AS_AUXMCU == 1
        case SEND_AUX_ACIMDATA:
            MPReport_AuxACIMData(&m_pReport->m_Report_Acim_Data, &EventTx_Length);
            break;
#endif
        case SEND_REQUEST_SYSTEMTIME:
            break;
        default:
            EventTx_Length = 0; // 没有状态信息内容。
            break;
        }

        PushToSendingEventBuf(& m_CmdTxBuffer_Async_Temp[0], EventTx_Length); // 发送测试事件
        EventsBuffering::PushingEvents(& m_CmdTxBuffer_Async_Temp[0], EventTx_Length, m_dwSend_SN); // 备份到EventBuff中去,以便5秒后重发。
        break;

    // 2、数据内容单包发送
    case SEND_LOG_DATA:
    case SEND_DISPLAY_DATA:
    case SEND_IDLE_DATA:
    case SEND_LOG_SDL:
        if(uc_SendType == SEND_DISPLAY_DATA)
            uc_Temp = SDL_DATA_FOR_DISPLAY;
        else if(uc_SendType == SEND_LOG_DATA)
            uc_Temp = SDL_DATA_FOR_LOG;
        else
            uc_Temp = SDL_DATA_FOR_IDLE;

        if(uc_SendType == SEND_LOG_SDL)
        {
#if  USE_AS_AUXMCU == 0
            if(IS_QUEUE_EMPTY(IVSDL::Buffer))
                return ;   //stand for empty;
            //MPCAN_ReportSDL_IdleData(SDL_DATA_TYPE_IDLE);  //20150216,modified by hepeiqing
#endif
            MPScheduleReport_LogSDL(uw16_ChannelNumber, &m_pReport->m_Report_LogData_SDL, &DataTx_Length);
        }
        else
        {
#if  USE_AS_AUXMCU == 0
            if(IVSDL::Valid)
            {   //如果SDL功能被使能，从这里将不发送如何传统LOG数据点，而是将数据点压栈进入SDL的buffer
                IVSDL::TriggerLog(uw16_ChannelNumber, SDL_DATA_FOR_LOG, uc_LogCriticalPoint);
                IVSDL::Check_BuiltInAuxForLog(uw16_ChannelNumber, SDL_DATA_FOR_LOG); //循环发送
                uc_LogCriticalPoint = LOG_NORMAL_DATA_POINT;
                return;
            }
            //MPCAN_ReportSDL_IdleData(SDL_DATA_TYPE_IDLE);   //idle data
#endif
            //MPScheduleReport_LogData(uw16_ChannelNumber, uc_Temp, &m_pReport->m_Report_LogData, &DataTx_Length);
            MPScheduleReport_LogData_TC_Counter(uw16_ChannelNumber, uc_Temp, &m_pReport->m_Report_LogData_TC_Counter, &DataTx_Length);
        }
        //send idle CAN Meta Value(if log data is running ,doesn't need send idle data)


        PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
        break;
        // can bms data send
#if (USE_AS_AUXMCU == 0)
    case SEND_LOG_BMS_DATA:
    case SEND_IDLE_BMS_DATA:
        MPReportCANData(uw16_ChannelNumber, uc_SendType);
        break;

    case SEND_LOG_SMB_DATA_FLOAT:
    case SEND_IDLE_SMB_DATA_FLOAT:
// 			for(unsigned char uc_MetaIdx = 0;uc_MetaIdx< Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount;uc_MetaIdx++)
// 				{
// 					if(ChannelInfo_Manager::m_SMBusMetaVariable[uw16_ChannelNumber][uc_MetaIdx].m_type==TYPE_WORD)
        MPReportSMBDataFloat(uw16_ChannelNumber, uc_SendType);
// 				}
        break;
    case SEND_LOG_SMB_DATA_STRING:
    case SEND_IDLE_SMB_DATA_STRING:

// 			for(unsigned char uc_MetaIdx = 0;uc_MetaIdx< Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount;uc_MetaIdx++)
// 				{
// 					if(ChannelInfo_Manager::m_SMBusMetaVariable[uw16_ChannelNumber][uc_MetaIdx].m_type==TYPE_STRING)
        MPReportSMBDataString(uw16_ChannelNumber, uc_SendType, 0);
// 				}
        break;
#endif
    // 3、特殊内容包发送
    // 无发送事件的编号m_dwSendSN = m_dwSend_SN ; 上位机无需回复; 下位机无需重发。
    case SEND_GET_CHANNELBOARD_MG:
        MPReport_GetChannelBoard();
        break;
    case SEND_GET_IV_BOARDTYPE_MG:
        MPReport_GetIV_BOARDTYPE();
        break;
//     case SEND_WFRAM_EVENT:
        //MPReport_WriteFram_Event();  // 独立外部管理接口。
//         break;
#if  USE_AS_AUXMCU == 0
    case SEND_BUILTIN_DATA:
        MPReport_BuiltInLogData(&m_pReport->m_Report_LogData_SDL_AUX, &DataTx_Length);//180518 zyx m_Report_LogData_SDL
        break;
#endif
#if  USE_AS_AUXMCU == 1
    case SEND_AUX_LOGDATA:
        MPReport_AuxLogData(&m_pReport->m_Report_LogData_SDL_AUX, &DataTx_Length);
        break;

    case SEND_AUX_GROUPLOGDATA:
        MPReport_AuxGroupLogData(&m_pReport->m_Report_GroupLogData_SDL_AUX, &DataTx_Length);
        break;
#endif

    case SEND_ERROR_MESSAGE:
        MPReport_ErrorMessage(&m_pReport->m_Report_ToPC_Error);
        break;

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    case SEND_GET_SCHSTATUS:
        MPReport_GetSCH_Status(uw16_ChannelNumber, &m_pReport->m_Report_GetSCH_Status);
        break;
#endif
    case SEND_PULSE_LOG_DATA:
        MPScheduleReport_Pulse_LogData( &m_pReport->m_Report_Pulse_LogData, &DataTx_Length);
        break;

    case SEND_IV_BUILDINAUX_LOGDATA:
        MPReport_IV_BuildInAux_LogData( &m_pReport->m_Report_BuildInAux_LogData, &DataTx_Length);
        break;

    case SEND_LOG_SDL_CYCLE:
        MPScheduleReport_LogSDL_Cycle(uw16_ChannelNumber, &m_pReport->m_Report_Event_SDL_Cycle, &DataTx_Length); // 20170919 yy
        break;

#if ( USE_AS_AUXMCU == 0 )
    case SEND_REPORT_SIMULATION_REQUESTDATA:
        MPReport_SimulationRequestData(&m_pReport->m_Report_Simulation_RequestData);
        break;
#endif
    default:
        break;

    }
}

///////////////////////////
//第二信道上发FRAM被写信息
///////////////////////////
#ifdef REPORT_FRAM_EVENT
void EthernetParse::MPReport_WriteFram_Event(St_FramMessage FramMsg, MP_REPORT_FRAM_EVENT *pReport)
{
    unsigned char uc_Event;
    uc_Event = SEND_WFRAM_EVENT;
    pReport->m_dwCmd = CMD_REPORT_WRITE_FRAM;  // 将大数据体信息转存到通信协议包中，然后打包发送

    pReport->m_wChannelBitIdx   = FramMsg.m_wChannelBitIdx;
    pReport->m_wChannelCount = FramMsg.m_wChannelCount;
    pReport->m_wFramType = FramMsg.m_wFramType;                   //写FRAM类型
    pReport->m_StartAddress = FramMsg.m_StartAddress;
    pReport->m_u16Length = FramMsg.m_u16Length;
    pReport->m_MsTime = CommonFunc::GetSystemTime();
//     pReport->m_MsTime.Second = OS_ServeProcess::OS_Time.Second;
//     pReport->m_MsTime.Us100 = OS_ServeProcess::OS_Time.Us100;

    //pReport->m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
    //pReport->m_btEvent = uc_Event;
    EventTx_Length = sizeof(MP_REPORT_FRAM_EVENT);  // 保存准备发送数据包的数据长度
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &EventTx_Length, uc_Event);

    PushToSendingEventBuf(& m_CmdTxBuffer_Async_Temp[0], EventTx_Length); // 发送测试数据
}
#endif

////////
// MPScheduleReportEvent(UWord16 uw16_ChannelNumber, MP_REPORT_EVENT *pReport)
//  : 网络命令第二信道报告状态处理函数
// Input: uw16_ChannelNumber 网络第二信道报告状态通道号;
//        pReport 网络第二信道报告状态结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_Event(UWord16 uw16_ChannelNumber, unsigned char uc_SendType, MP_REPORT_EVENT *pReport, uint16 *CmdTxAsync_Length)
{
    // Schedule回复 //
#if  USE_AS_AUXMCU == 0
    unsigned char uc_Event;

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_EVENT;  // 将大数据体信息转存到通信协议包中，然后打包发送
    // 	if(uw16_ChannelNumber >= MAXCHANNELNO)
    // 		uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
    if(uw16_ChannelNumber >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        uw16_ChannelNumber %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
    pReport->m_wChannelNumber   = uw16_ChannelNumber;
    pReport->m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
    /*pReport->m_StMD5Code.m_ucData[0] = 1;
    pReport->m_StMD5Code.m_ucData[1] = 2;
    pReport->m_StMD5Code.m_ucData[2] = 3;
    pReport->m_StMD5Code.m_ucData[3] = 4;
    pReport->m_StMD5Code.m_ucData[12] = 5;
    pReport->m_StMD5Code.m_ucData[13] = 6;
    pReport->m_StMD5Code.m_ucData[14] = 7;
    pReport->m_StMD5Code.m_ucData[15] = 8;*/

    /*ChannelInfo_Manager::Read_SpecifiedMetavariable(uw16_ChannelNumber,MetaCode_PV_StepIndex);
    pReport->m_wStepID = ChannelInfo_Manager::MetaValue.m_fValue;*/

    //pReport->m_wStepID = m_ReportStatus.m_wStepID;
    pReport->m_TestID = Schedule_Manager_task::m_dwTestID[uw16_ChannelNumber];
    pReport->m_wStepID = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_RunningStepIdx;
    pReport->m_ItemCount_SDL = 0;
    //if(uc_SendType == SEND_EVENT_SDL)
    //{
    //	if(IVSDL::NeedSDL(uw16_ChannelNumber))  //  (m_ucLogLimitNo == 0)-->(bNeedSDL = 1)
    //	    pReport->m_ItemCount_SDL = CMD_TXSEND_SDL_POINT;
    //}
    //pReport->m_st_SYSTEMTIME_DATA.m_btTime = OS_ServeProcess::OS_Time.Us100 * (10E3) + OS_ServeProcess::OS_Time.InSeconds * (10E7);
    //memcpy(&pReport->m_st_SYSTEMTIME_DATA, &OS_ServeProcess::OS_Time, 12);
    MsTime SysTime = CommonFunc::GetSystemTime();
    pReport->m_stTime.Us100 = SysTime.Us100;
    pReport->m_stTime.Second = SysTime.Second;

    //pReport->m_st_TestTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_TestTime) * (10E5f);
    double d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_TestTime,SysTime);
    pReport->m_st_TestTime.m_U64Time = d_Value * (10E5f);

    BurstModeSampling::bl_Time = false; //  2017.6.8  yy
    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
//         case BOARDTYPE_BT2204:
    case BOARDTYPE_LBT22043:
    case BOARDTYPE_LBT22013:
    case BOARDTYPE_LBT22023:
//         case BOARDTYPE_LBT22024:
//         case BOARDTYPE_LBT22013FR:
//         case BOARDTYPE_LBT22044:
        if(BurstModeSampling::uc_A_BurstMode[uw16_ChannelNumber]
                || BurstModeSampling::bl_NeedRunning[uw16_ChannelNumber]
                || BurstModeSampling::bl_ChangedTime[uw16_ChannelNumber])
        {
            if(m_ReportStatus.m_btCMD == PC_Started_Msg)
                BurstModeSampling::bl_Time = true;
        }
        break;
    default:
        break;
    }

    //pReport->m_st_StepTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_Steptime) * (10E5f);
    d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_Steptime,SysTime);
    pReport->m_st_StepTime.m_U64Time = d_Value * (10E5f);

    pReport->m_dwCycleID = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_CycleIndex);
    //pReport->m_dwCycleID = m_ReportStatus.m_dwCycleID;
    if(pReport->m_dwCycleID > 0)
        pReport->m_dwCycleID --; // 暂时为上位机显示正常循环次数提供的处理。
    //pReport->m_btSCHEvent_Source = m_ReportStatus.m_btSCHEvent_Source;
    //170314,add by hepeiqing,for TCcounter logging in sql,start
    pReport->m_TC_Counter[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter1);
    pReport->m_TC_Counter[1] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter2);
    pReport->m_TC_Counter[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter3);
    pReport->m_TC_Counter[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter4);
    //170314,add by hepeiqing,for TCcounter logging in sql,end
    pReport->m_btSCHEvent_Source = ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].m_Event_Source;  //将事件源上传0917 diruiADD   m_Event_Source为SCH小范围/m_btCMD_Source为模块大范围
    ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].m_Event_Source = UNKNOWN;                 //清除事件源
    switch(m_ReportStatus.m_btCMD)  // 暂时转换对应状态
    {
    case PC_LimitArrive_Msg:
        uc_Event = ST_EVENT_LIMITREACH;
        break;

    case PC_StopRpt_Msg:
        uc_Event = ST_EVENT_STOPPED;
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Result = PCSTOPOK;
        ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].chn_Running_Flag = CHN_LOCK_IDLE;
//         IVSDL::uc_IR_SendCount[uw16_ChannelNumber] = 0;   // 仅当为IR功能时，才向上位机传送IR内阻值。
        IVSDL::uc_ACR_SendCount[uw16_ChannelNumber] = 0;  // 仅当为ACR功能时，才向上位机传送ACR内阻值。
        break;

    case PC_Unsafe_Msg:
    case PC_Abnormal_Msg:
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM )   //add by dirui 20171101 add alarm for pwm
            PWM_Base::PWM_LEDRunStatus = SYSTEM_ALARM;
        Schedule_Manager_task::M_SysALarmEmergencyFlg|=(1<<uw16_ChannelNumber);
        CANBusParse::IV_SendTOSysCtrl_Abnormal();
        ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].chn_Running_Flag = CHN_LOCK_IDLE;
        uc_Event = ST_EVENT_STOPPED;// 暂时用代替显示测试办法，以后更改正式方式。
        //uc_Stop_Status = ST_EVENT_UNSAFE;
        m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_UNSAFE;
        //unsigned char uc_bit,uc_Safe_Code;
        //uc_Safe_Code = m_ReportStatus.m_st_RunningStatus.m_btSafe_Code;
        //m_ReportStatus.m_st_RunningStatus.m_btSafe_Code &= 0X07; //  屏蔽无用的位报警信息，仅报I,V,P
        //for(unsigned char uc_i = SAFETY_SCH; uc_i < SAFETY_MAXVALUE; uc_i++ )// 扫描系统报警位
        //{
        //	uc_bit = MYBITMOVEL(uc_i);

        //	if((uc_Safe_Code & uc_bit) == (MYBITMOVEL(SAFETY_SCH)))
        //	{
        //		m_ReportStatus.m_st_RunningStatus.m_btSafe_Type = UNSAFE_SCHEDULE;
        //	}
        //	if((uc_Safe_Code & uc_bit) == (MYBITMOVEL(SAFETY_SYS)))
        //	{
        //		m_ReportStatus.m_st_RunningStatus.m_btSafe_Type = UNSAFE_SYSTEM;
        //	}
        //	if((uc_Safe_Code & uc_bit) == (MYBITMOVEL(SAFETY_THIRD)))
        //	{
        //		m_ReportStatus.m_st_RunningStatus.m_btSafe_Type = UNSAFE_SYSTEM;
        //	}
        //}
        break;

    case PC_Started_Msg:
        uc_Event = ST_EVENT_STARTED;
        m_ReportStatus.m_st_RunningStatus.m_btCMD_Result = PCSTARTOK;

        if( (Step_Manager_task::GetStepRunInfo[uw16_ChannelNumber].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE)\
                && ((unsigned int)(Step_Manager_task::GetStepRunInfo[uw16_ChannelNumber].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]+0.1f) == ACIM_PAUSE) )
        {
            pReport->m_ACIM[0] = 0x0A;
            pReport->m_ACIM[1] = 0x0C;
            Step_Manager_task::ACIM_Status[uw16_ChannelNumber] = ACIM_WAIT;
        }
        else
        {
            pReport->m_ACIM[0] = 0x00;
            pReport->m_ACIM[1] = 0x00;
        }
        switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
        {
//             case BOARDTYPE_BT2204:
        case BOARDTYPE_LBT22043:
        case BOARDTYPE_LBT22013:
        case BOARDTYPE_LBT22023:
//             case BOARDTYPE_LBT22024:
//             case BOARDTYPE_LBT22013FR:
//             case BOARDTYPE_LBT22044:
            if(BurstModeSampling::uc_A_BurstMode[uw16_ChannelNumber]
                    || BurstModeSampling::bl_NeedRunning[uw16_ChannelNumber]
                    || BurstModeSampling::bl_ChangedTime[uw16_ChannelNumber])
            {
                BurstModeSampling::bl_ChangedTime[uw16_ChannelNumber] = false;
                BurstModeSampling::bl_Time = false;//  2017.6.8  yy
                if((pReport->m_stTime.Us100 != BurstModeSampling::OSTime_back.Us100)
                        || (pReport->m_stTime.Second != BurstModeSampling::OSTime_back.Second))
                {
                    pReport->m_stTime.Us100 = BurstModeSampling::OSTime_back.Us100;  // 系统时间进位，进行特殊处理
                    pReport->m_stTime.Second = BurstModeSampling::OSTime_back.Second;
                }

                //             if(BurstModeSampling::uw16_BurstModeDelayTime[uw16_ChannelNumber] <= 0)
                //             {
                //                     pReport->m_st_StepTime.m_U64Time = 0;
                //             }
                //             else
                //             {
                //                     pReport->m_st_StepTime.m_U64Time = BurstModeSampling::uw16_BurstModeDelayTime[uw16_ChannelNumber]*100;
                //                     pReport->m_st_TestTime.m_U64Time += pReport->m_st_StepTime.m_U64Time;
                //             }

                pReport->m_st_StepTime.m_U64Time = 0;

                pReport->m_stTime.Us100 = BurstModeSampling::StartTime[uw16_ChannelNumber].Us100;
                pReport->m_stTime.Second = BurstModeSampling::StartTime[uw16_ChannelNumber].Second;
                unsigned char activeindex = ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx;
                ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[activeindex].m_StartTime = BurstModeSampling::StartTime[uw16_ChannelNumber];  // 变更系统时间
            }
            break;
        default:
            break;
        }

        break;

    case PC_BackToNormal_Msg:

        //St_UseTxBuff->m_CmdTxAsync_Length  = 0; // 没有状态信息内容。
        //return;  // 暂时不报，从不安全回复到安全状态。

        uc_Event = ST_EVENT_BACKTONORMAL;

        /*uc_Event = ST_EVENT_STOPPED;  // 暂时用代替显示测试办法，以后更改正式方式。
        m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_UNSAFE;
        m_ReportStatus.m_st_RunningStatus.m_btSafe_Type = UNSAFE_LOGIC;
        m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_UNKNOWN;
        ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].chn_Running_Flag = CHN_LOCK_IDLE;*/
        break;

    case PC_Jump_Msg:
        uc_Event = ST_EVENT_JUMP;
        break;

    case PC_GoStop_Msg:
        uc_Event = ST_EVENT_GOSTOP;
        pReport->m_ItemCount_SDL = 7;
        //if(IVSDL::Valid)  //  SDL::Valid实际就是(USE_SDL)
        //{
        //       pReport->m_ItemCount_SDL = CMD_TXSEND_SDL_POINT;     //  2013/10/23  Yangy 解决无记录点问题。
        //}
        break;

    case PC_AcimRelayOn_Msg:
        uc_Event = ST_EVENT_ACIMRELAYON;
        break;

    case PC_AcimRelayOff_Msg:
        uc_Event = ST_EVENT_ACIMRELAYOFF;
        break;
    case PC_LimitChange_Msg:
        uc_Event = ST_EVENT_LIMITCHANGE;
        break;

    case PC_Error_Msg:
    default:
        uc_Event = ST_EVENT_STOPPED;
        m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_UNSAFE;
        m_ReportStatus.m_st_RunningStatus.m_btSafe_Type = UNSAFE_LOGIC;
        //m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_UNKNOWN;
        //pReport->m_st_TestTime.m_U64Time = 0;  //这里不应该清0，何培青  20131030
        break;
    }

    m_ReportStatus.m_st_LimitReachStatus.m_wLimit_Index = ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].Report_LimitID;  // 20130925 狄瑞 解决存储问题
    m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].Report_EquationID;

    pReport->m_btEvent = uc_Event;
    pReport->m_st_RunningStatus = m_ReportStatus.m_st_RunningStatus;
    pReport->m_st_LimitReachStatus = m_ReportStatus.m_st_LimitReachStatus;

    // 2015.09.22
//    if(Step_Manager_task::ACIM_Status[uw16_ChannelNumber] == ACIM_START)
//    {
//        pReport->m_btNone_11[0] = 0x0A;
//        pReport->m_btNone_11[1] = 0x0C;
//        Step_Manager_task::ACIM_Status[uw16_ChannelNumber] = ACIM_WAIT;
//    }



    if((uc_Event == ST_EVENT_STOPPED) && (Schedule_Manager_task::SendFinishFlag[uw16_ChannelNumber].StopStatus == STOP_AUXACKFAILINSTART)) //  20140530  CLean  Aux  ACK Time
    {
        pReport->m_st_TestTime.m_U64Time = 0;  //ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_TestTime) * (10E5f);
        pReport->m_st_StepTime.m_U64Time = 0;  //ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_Steptime) * (10E5f);
    }

    /*pReport->m_st_LimitReachStatus.f_LimitReserve[0] = 1.1;
    pReport->m_st_LimitReachStatus.f_LimitReserve[1] = 2.2;
    pReport->m_st_LimitReachStatus.f_LimitReserve[2] = 3.3;*/

    if((m_ReportStatus.m_btCMD == PC_Unsafe_Msg)
            //    &&(m_ReportStatus.m_st_RunningStatus.m_uw32Safe_Code == LIMIT_UNSAFE))
            && ((m_ReportStatus.m_st_RunningStatus.m_uw32Safe_Code & LIMIT_UNSAFE) == LIMIT_UNSAFE))
    {
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fLeft_Value1 = UnsafeLimit[uw16_ChannelNumber].m_fLeft_Value1;
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fRight_Value1 = UnsafeLimit[uw16_ChannelNumber].m_fRight_Value1;
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fLeft_Value2 = UnsafeLimit[uw16_ChannelNumber].m_fLeft_Value2;
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fRight_Value2 = UnsafeLimit[uw16_ChannelNumber].m_fRight_Value2;  // unsafe  Limit Unsafe  20140211
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fLeft_Value3 = UnsafeLimit[uw16_ChannelNumber].m_fLeft_Value3;
        pReport->m_st_RunningStatus.m_Value.st_UnsafeLimit.m_fRight_Value3 = UnsafeLimit[uw16_ChannelNumber].m_fRight_Value3;
    }

    pReport->m_dwSendSN = m_dwSend_SN;         //  发送事件编号

    *CmdTxAsync_Length = sizeof(MP_REPORT_EVENT);  // 保存准备发送数据包的数据长度
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, uc_SendType);

    memset((void *)&m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));

#endif
}

/*
////////
// MPScheduleAssignReport(UWord16 uw16_ChannelNumber, MP_CONFIRM_ASSIGN *pReport)
// : 网络命令第二信道报告Schedule指派信息处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule指派信息通道号;
//        pReport 网络第二信道报告Schedule指派信息结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_Assign(UWord16 uw16_ChannelNumber, MP_CONFIRM_ASSIGN *pReport, uint16 *CmdTxAsync_Length)
{
UWord16 uw16_Count;
UWord16 uw16_Length;
uw16_Length = sizeof(St_Assign);
pReport->m_dwCmd = CMD_SCHEDULE_ASSIGN_FEEDBACK;
// 	if(uw16_ChannelNumber >= MAXCHANNELNO)
// 		uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
if(uw16_ChannelNumber >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
uw16_ChannelNumber %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
if(m_st_Assign->m_u16ChannelIdx == uw16_ChannelNumber)
{
memcpy((void *) &pReport->m_st_Assign, m_st_Assign, uw16_Length);	 // 传递信息
}
else
{
*CmdTxAsync_Length = 0;
return;  // 传递失败
}
//memset((void *) &pReport->m_stMSecondField, 0, sizeof(ST_MultPack_SecondField));
memcpy(&pReport->m_stMSecondField.m_StMD5Code.m_ucData[0], &m_st_Assign->m_MD5Assign[0].m_MD5.m_ucData[0], sizeof(St_MD5));
pReport->m_stMSecondField.m_wChannelNumber = m_st_Assign->m_u16ChannelIdx;
pReport->m_stMSecondField.m_wTotalChannelNumber = 1;

*CmdTxAsync_Length = sizeof(MP_CONFIRM_ASSIGN);  // 实际长度=总结构数-全部未填数组+已经填写数据数组;
}
*/

void EthernetParse::MPScheduleReport_MVUDSynch(UWord16 uw16_ChannelNumber, MP_REPORT_METAVARIABLESYNCH *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    UWord16 uw16_Count = 0;
    UWord16 uw16_i;
    UWord16 uw16_MetaCode;

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_METAVARIABLESYNCH;  // 将大数据体信息转存到通信协议包中，然后打包发送
    if(uw16_ChannelNumber >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        uw16_ChannelNumber %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
    pReport->m_wChannelNumber   = uw16_ChannelNumber;

    pReport->Time = CommonFunc::GetSystemTime();
    pReport->m_DataPoint =  ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DataPoints].m_Value.m_uInt32; // This is current logged datapoints

    pReport->m_dwSendSN = m_dwSend_SN;         //  发送事件编号
    pReport->m_wVariable_Count = m_ReportLogMetaVariable.m_wCustomLogVariables_Count;
    pReport->m_ChannelStatus = Schedule_Manager_task::SendFinishFlag[uw16_ChannelNumber].StopStatus; //zyx 171010
    //uw16_Count = 4;
    int count = 0;

    for(uw16_i = 0; uw16_i < 4; uw16_i ++)
    {   // 转换数据类型
        if(uw16_i >= MAX_SEND_METAVARIABLE)
        {
            uw16_Count = MAX_SEND_METAVARIABLE;
            break;  //  保护一次传送太多MetaVariable数据
        }
        uw16_MetaCode = MetaCode_MV_UD1+uw16_i;
        pReport->m_st_Variable[count].m_wVariable_Code = uw16_MetaCode;
        pReport->m_st_Variable[count].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,uw16_MetaCode);
        count++;
    }

    for(uw16_i = 0; uw16_i < 12; uw16_i ++)
    {   // 转换数据类型
        if(uw16_i >= MAX_SEND_METAVARIABLE)
        {
            uw16_Count = MAX_SEND_METAVARIABLE;
            break;  //  保护一次传送太多MetaVariable数据
        }
        uw16_MetaCode = MetaCode_MV_UD5+uw16_i;
        pReport->m_st_Variable[count].m_wVariable_Code = uw16_MetaCode;
        pReport->m_st_Variable[count].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,uw16_MetaCode);
        count++;
    }

    uw16_Count = 16;
    UWord16 uw16_Length = sizeof(ST_SCHEDULE_VARIABLE)*(MAX_SEND_METAVARIABLE - uw16_Count);   //  结构中空的节数;  20130910 yang 修复校验时同步变量数据更新命令校验错误问题
    *CmdTxAsync_Length = sizeof(MP_REPORT_METAVARIABLESYNCH) - uw16_Length; // 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_METAVARIABLESYNCH);
#endif

}

////////
// MPScheduleReportMetaVariableSynch(UWord16 uw16_ChannelNumber, MP_REPORT_METAVARIABLESYNCH *pReport)
// : 网络命令第二信道报告同步Metavariable处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule指派信息通道号;
//        pReport 网络第二信道报告Schedule同步Metavariable结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_MetaVariableSynch(UWord16 uw16_ChannelNumber, MP_REPORT_METAVARIABLESYNCH *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    UWord16 uw16_Count = 0;
    UWord16 uw16_i;
    UWord16 uw16_MetaCode;

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_METAVARIABLESYNCH;  // 将大数据体信息转存到通信协议包中，然后打包发送
    //     if(uw16_ChannelNumber >= MAXCHANNELNO)
    // 		uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
    if(uw16_ChannelNumber >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        uw16_ChannelNumber %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护
    pReport->m_wChannelNumber   = uw16_ChannelNumber;
    //pReport->m_st_SYSTEMTIME_DATA.m_btTime = OS_ServeProcess::OS_Time.Us100 * (10E3) + OS_ServeProcess::OS_Time.InSeconds * (10E7);
    //memcpy(&pReport->m_st_SYSTEMTIME_DATA, &OS_ServeProcess::OS_Time, 12);
    pReport->Time = CommonFunc::GetSystemTime();
//     pReport->Time.Us100 = OS_ServeProcess::OS_Time.Us100;
//     pReport->Time.Second = OS_ServeProcess::OS_Time.Second;

    pReport->m_DataPoint =  ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DataPoints].m_Value.m_uInt32; // This is current logged datapoints
    pReport->m_dwSendSN = m_dwSend_SN;         //  发送事件编号
    pReport->m_wVariable_Count = m_ReportLogMetaVariable.m_wCustomLogVariables_Count;
    pReport->m_ChannelStatus = Schedule_Manager_task::SendFinishFlag[uw16_ChannelNumber].StopStatus; //zyx 171010
    uw16_Count = m_ReportLogMetaVariable.m_wCustomLogVariables_Count;
    for(uw16_i = 0; uw16_i < uw16_Count; uw16_i ++)
    {   // 转换数据类型
        if(uw16_i >= MAX_SEND_METAVARIABLE)
        {
// 			m_ReportLogMetaVariable.m_wCustomLogVariables_Count = MAX_SEND_METAVARIABLE;
            uw16_Count = MAX_SEND_METAVARIABLE;
            break;  //  保护一次传送太多MetaVariable数据
        }
        uw16_MetaCode = m_ReportLogMetaVariable.m_wCustomerLogValue_Type[uw16_i];
        pReport->m_st_Variable[uw16_i].m_wVariable_Code = uw16_MetaCode;
        if(pReport->m_st_Variable[uw16_i].m_wVariable_Code == MetaCode_PV_StepIndex)
        {
            pReport->m_st_Variable[uw16_i].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, uw16_MetaCode) - 1;
        }
        else
            pReport->m_st_Variable[uw16_i].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, uw16_MetaCode);

        if(uw16_MetaCode == MetaCode_PV_CycleIndex)
        {
            if(pReport->m_st_Variable[uw16_i].m_fVariable_Value > 0)
                pReport->m_st_Variable[uw16_i].m_fVariable_Value --; // 暂时为上位机显示正常循环次数提供的处理。
        }
    }

    UWord16 uw16_Length = sizeof(ST_SCHEDULE_VARIABLE) * (MAX_SEND_METAVARIABLE - uw16_Count); //  结构中空的节数;  20130910 yang 修复校验时同步变量数据更新命令校验错误问题
    //memset((void *)&pReport->m_st_Variable[uw16_Count], 0, uw16_Length); // 空的以0填补

    *CmdTxAsync_Length = sizeof(MP_REPORT_METAVARIABLESYNCH) - uw16_Length; // 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_METAVARIABLESYNCH);
#endif
}

////////
// MPScheduleReportLogData(UWord16 uw16_ChannelNumber, UChar8 uc_LogOrDisplayFlag,MP_REPORT_LOGDATA *pReport)
//  : 网络命令第二信道报告Schedule数据处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule数据通道号;
//        uc_LogOrDisplayFlag 网络第二信道报告Schedule数据是否为记录数据;
//        pReport 网络第二信道报告Schedule数据结构指针;
// Return: N/A
///////
/*void EthernetParse::MPScheduleReport_LogData(UWord16 uw16_ChannelNumber, UChar8 uc_LogType,MP_REPORT_LOGDATA *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
	pReport->m_dwCmd = CMD_SCHEDULE_REPORT_LOGDATA;  // 将大数据体信息转存到通信协议包中，然后打包发送
	if(uw16_ChannelNumber >= MAXCHANNELNO)
		uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
	pReport->m_wChannelNumber   = uw16_ChannelNumber;
	pReport->m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
	pReport->m_wStepID = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_RunningStepIdx;

	pReport->m_dwCycleID = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_CycleIndex].m_fValue;
	if(pReport->m_dwCycleID > 0)
		pReport->m_dwCycleID --; // 暂时为上位机显示正常循环次数提供的处理。

	pReport->uc_LogType = uc_LogType;
#if SOFTWARE_DOG_ENABLE == 1
	pReport->m_u16Token = m_u16Token+1;
#endif
	pReport->m_wVariable_Count = m_ReportLogMetaVariable.m_wCustomLogVariables_Count;

	// 	pReport->m_st_TestTime.m_U64Time = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_TestTime].m_fValue * (10E5f);
	// 	pReport->m_st_StepTime.m_U64Time = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Steptime].m_fValue * (10E5f);
	pReport->m_st_TestTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_TestTime) * (10E5f);
	pReport->m_st_StepTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_Steptime) * (10E5f);
	pReport->m_st_SampleTime.m_U64Time = OS_ServeProcess::OS_Time.Us100 * (10E3f) + OS_ServeProcess::OS_Time.Second * (10E7f);

	if(ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint
		&&
		(ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_CPULSE))
		ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint = 0;

	if(ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint==0)
	{
		pReport->fCurrent = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Current].m_fValue;
		pReport->fVoltage = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
	}
	else
	{
		pReport->fCurrent = ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].fValue;
		pReport->fVoltage = ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].fValue2;
		ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint=0;  //报告完毕此点不需要了   dirui 2014/12/26
	}
	pReport->fChargeCapacity = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacity].m_fValue;
	pReport->fDischargeCapacity = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacity].m_fValue;

	pReport->fChargeCapacityTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_ChargeCapacityTime); //zyx 171010
	pReport->fDischargeCapacityTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_DischargeCapacityTime);

    pReport->fChargeEnergy = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeEnergy].m_fValue;
	pReport->fDischargeEnergy = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeEnergy].m_fValue;
	pReport->fIR = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_InternalResistance].m_fValue;

	if(board_config::DacInfo.m_bHaveBuildInVolt)
	{
		for(int i = 0;i < ChannelInfo_Manager::BoardCFG.m_Global.m_uBuildInVoltCount;i++)
			pReport->fBuildInVolt[i] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_BuildIn_V1 + i);
	}
  	if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
	{
       unsigned char uc_ChanCnt = (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels/ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount);
		   for(int i = 0;i <uc_ChanCnt;i++)
		   {
          pReport->fBuiltInData[i]=ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_BuiltIn_Data1 + i);
       }
  }
	pReport->fdVdt = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_dVdt].m_fValue;

#if DEBUG_TEST_COMMDATA_MODE == 1
	pReport->fChargeCapacity = ++uw32_TestLogD[uw16_ChannelNumber];  // 通道已记录的点数
	pReport->fDischargeCapacity = uw32_TestPackSum;  //  已发包的包号
	pReport->fChargeEnergy = uw32_TestEventNum;  // 已发信息号
	pReport->fDischargeEnergy = uw32_TestBlock1; // 阻塞次数
	pReport->fIR = uw32_TestDataPack;   // 已发数据包的包号
	//pReport->fIR = uw32_TestSumRevice;
#endif

	*CmdTxAsync_Length = sizeof(MP_REPORT_LOGDATA);// 保存准备发送数据包的数据长度;
	HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_LOG_DATA);
#endif
}*/
void EthernetParse::MPScheduleReport_LogData_TC_Counter(UWord16 uw16_ChannelNumber, UChar8 uc_LogType, MP_REPORT_LOGDATA_TC_Counter *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_LOGDATA_TC_Counter;  // 将大数据体信息转存到通信协议包中，然后打包发送
    if(uw16_ChannelNumber >= MAXCHANNELNO)
        uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
    pReport->m_wChannelNumber   = uw16_ChannelNumber;
    pReport->m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
    pReport->m_wStepID = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_RunningStepIdx;

    pReport->m_dwCycleID = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_CycleIndex].m_fValue;
    if(pReport->m_dwCycleID > 0)
        pReport->m_dwCycleID --; // 暂时为上位机显示正常循环次数提供的处理。

    pReport->uc_LogType = uc_LogType;
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif
    pReport->m_wVariable_Count = m_ReportLogMetaVariable.m_wCustomLogVariables_Count;

    MsTime SysTime = CommonFunc::GetSystemTime();
//     pReport->m_st_TestTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_TestTime) * (10E5f);
//     pReport->m_st_StepTime.m_U64Time = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_Steptime) * (10E5f);
    double  d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_TestTime,SysTime);
    pReport->m_st_TestTime.m_U64Time = d_Value * (10E5f);

    d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_Steptime,SysTime);
    pReport->m_st_StepTime.m_U64Time = d_Value * (10E5f);

    pReport->m_st_SampleTime.m_U64Time = SysTime.Us100 * (10E3f) + SysTime.Second * (10E7f);

    if(ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint
            &&
            (ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_CPULSE))
        ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint = 0;

    if(ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint == 0)
    {
        pReport->fCurrent = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Current].m_fValue;
        pReport->fVoltage = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
    }
    else
    {
        pReport->fCurrent = ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].fValue;
        pReport->fVoltage = ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].fValue2;
        ChannelInfo_Manager::PulseLimitArriveData[uw16_ChannelNumber].bCriticalPoint = 0; //报告完毕此点不需要了   dirui 2014/12/26
    }
    pReport->fChargeCapacity = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacity].m_fValue;
    pReport->fDischargeCapacity = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacity].m_fValue;

    pReport->fChargeCapacityTime =  ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_ChargeCapacityTime); //zyx 171010
    pReport->fDischargeCapacityTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_DischargeCapacityTime);

    pReport->fChargeEnergy = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeEnergy].m_fValue;
    pReport->fDischargeEnergy = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeEnergy].m_fValue;
    pReport->fIR = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_InternalResistance].m_fValue;
    pReport->fACIR = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ACR].m_fValue;

    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_uBuildInVoltCount; i++)
            pReport->fBuildInVolt[i] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_BuildIn_V1 + i);
    }

    pReport->fdVdt = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_dVdt].m_fValue;
    pReport->fdQdV = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_dQdV].m_fValue;
    pReport->fdVdQ = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_dVdQ].m_fValue;

    pReport->fTC_Counter1 = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_TC_Counter1].m_fValue;
    pReport->fTC_Counter2 = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_TC_Counter2].m_fValue;
    pReport->fTC_Counter3 = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_TC_Counter3].m_fValue;
    pReport->fTC_Counter4 = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_TC_Counter4].m_fValue;

#if DEBUG_TEST_COMMDATA_MODE == 1
    pReport->fChargeCapacity = ++uw32_TestLogD[uw16_ChannelNumber];  // 通道已记录的点数
    pReport->fDischargeCapacity = uw32_TestPackSum;  //  已发包的包号
    pReport->fChargeEnergy = uw32_TestEventNum;  // 已发信息号
    pReport->fDischargeEnergy = uw32_TestBlock1; // 阻塞次数
    pReport->fIR = uw32_TestDataPack;   // 已发数据包的包号
    //pReport->fIR = uw32_TestSumRevice;
#endif

    *CmdTxAsync_Length = sizeof(MP_REPORT_LOGDATA_TC_Counter);// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_LOG_DATA);
#endif
}
/*
////////
// MP_Request_SystemTime(MP_REQUEST_SYSTEMTIME *pReport, uint16 *CmdTxAsync_Length)
//  : 网络命令第二信道请求系统时间对时处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule数据通道号;
//        uc_LogOrDisplayFlag 网络第二信道报告Schedule数据是否为记录数据;
//        pReport 网络第二信道报告Schedule数据结构指针;
// Return: N/A
///////
void EthernetParse::MP_Request_SystemTime( MP_REQUEST_SYSTEMTIME *pReport, uint16 *CmdTxAsync_Length)
{
	UWord32    uw32_Cmd = CMD_REPORT_REQUEST_SYSTEMTIME;
	memcpy(&m_CmdTxBuffer_Async_Temp[0], &uw32_Cmd, 4);
	memset(&m_CmdTxBuffer_Async_Temp[4], 0, 12);
	pReport->m_dwSendSN = m_dwSend_SN;         //  发送事件编号

	*CmdTxAsync_Length = sizeof(MP_REQUEST_SYSTEMTIME);// 保存准备发送数据包的数据长度;
	HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_REQUEST_SYSTEMTIME);
}*/

////////
// MPReport_GetChannelBoard(void)
//  : 网络命令第二信道获取板的通道信息处理函数
// Input: N/A
// Return: N/A
///////
void EthernetParse::MPReport_GetChannelBoard(void)
{
#if  USE_AS_AUXMCU == 1
    MP_REPORT_GET_AUX_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_AUX_CHANNEL_BOARD *) &m_btPackDataBuffer[16];
#else
    MP_REPORT_GET_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_CHANNEL_BOARD *) &m_btPackDataBuffer[16];
    if(m_ResultFeedback.m_dwCopyCmd == CMD_GET_BUILTIN_CHANNEL_BOARD)
        MP_REPORT_GET_AUX_CHANNEL_BOARD * st_Config = (MP_REPORT_GET_AUX_CHANNEL_BOARD *) &m_btPackDataBuffer[16];//1807131
#endif

    //     if(st_Config->m_stMPack.m_stMFirstField.m_wPackNo == 0)
    // 	{
    st_MultPackManage.m_stMFirstField = st_Config->m_stMPack.m_stMFirstField;// 转传管理信息  // （多包信息管理第一区域)  // m_dwPackLength; // 首包为原值
    st_MultPackManage.m_stMSecondField = st_Config->m_stMPack.m_stMSecondField; // 转传管理信息  // （多包信息管理第二区域)
    st_Config->m_dwCmd = CMD_REPORT_GET_CHANNEL_BOARD;  //  单个结构多包串联涵接发送。(MP_GET_REPORT_CHANNEL_BOARD 整结构8包11K)
    st_Config->m_dwCmd_Extend = 0;

    st_MultPackManage.m_stMFirstField.m_dwPackLength =
        st_MultPackManage.m_stMFirstField.m_dwPackLength > MAX_PACKLIMIT_SIZE ? MAX_PACKLIMIT_SIZE : st_MultPackManage.m_stMFirstField.m_dwPackLength; // 非首包为累加值
    st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;  // 首包地址从0开始
    // 	}
    // 	else
    // 	{
    // 	    st_MultPackManage.m_stMFirstField.m_wPackNo ++;
    // 	    if(st_MultPackManage.m_stMFirstField.m_wPackNo < st_Config->m_stMPack.m_stMFirstField.m_wPackTotal)
    // 		    st_MultPackManage.m_stMFirstField.m_dwPackLength = MAX_PACKLIMIT_SIZE;
    // 		else
    // 			st_MultPackManage.m_stMFirstField.m_dwPackLength = MAX_PACKLIMIT_SIZE - st_Config->m_stMPack.m_stMFirstField.m_wPackTotal;
    //         st_MultPackManage.m_stMFirstField.m_dwPack_BufferArrayAddress += MAX_PACKLIMIT_SIZE; // 非首包地址累加
    // 	}

    EventTx_Length = st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength - CMD_HOLDHAND_LENGTH; // 计算多少个包时已加入包头，应去除
    HeadFieldSaveBuff(m_btPackDataBuffer, &EventTx_Length, SEND_GET_CHANNELBOARD_MG);

    if (st_Config->m_stMPack.m_stMFirstField.m_dwTotalLength > (MAX_EVENT_BUFFER_SIZE - QUEUE_DEPTH(TestEventTxBuffer)))
    {
        //虽然没满，但是剩余空间也不够填写了，失败
        return;
    }
    PushToSendingEventBuf(&m_btPackDataBuffer[0], EventTx_Length); // 发送测试事件

    //EventsBuffering::PushingEvents(& m_btPackDataBuffer[0],EventTx_Length,++ m_dwSend_SN);// 备份到EventBuff中去,以便5秒后重发。
    //m_dwSend_SN --; // 多包暂不作为重发事件发送。   // 20141014  yy   Fix of lost one Event problem.
}

void EthernetParse::MPReport_GetIV_BOARDTYPE(void)
{
    MP_REPORT_GET_IV_BOARDTYPE * st_Config = (MP_REPORT_GET_IV_BOARDTYPE *) &m_btPackDataBuffer[16];

    st_Config->m_dwCmd = CMD_REPORT_GET_IV_BOARDTYPE;  //  单个结构发送。
    st_Config->m_dwCmd_Extend = 0;

    if((st_Config->m_st_BoadType.m_ucIRangeCount > NUM_OF_IV_RANGE_I) || (st_Config->m_st_BoadType.m_ucVRangeCount > NUM_OF_IV_RANGE_V))
        memset(&st_Config->m_st_BoadType, 0, sizeof(IV_BOARDTYPE_CFG));
    EventTx_Length = sizeof(MP_REPORT_GET_IV_BOARDTYPE);
    HeadFieldSaveBuff(m_btPackDataBuffer, &EventTx_Length, SEND_GET_IV_BOARDTYPE_MG);

    PushToSendingEventBuf(&m_btPackDataBuffer[0], EventTx_Length); // 发送测试事件
}

////////
// MPScheduleReportLogData(UWord16 uw16_ChannelNumber, UChar8 uc_LogOrDisplayFlag,MP_REPORT_LOGDATA *pReport)
//  : 网络命令第二信道报告Schedule数据处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule数据通道号;
//        uc_LogOrDisplayFlag 网络第二信道报告Schedule数据是否为记录数据;
//        pReport 网络第二信道报告Schedule数据结构指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_EventSDL(UWord16 uw16_ChannelNumber, unsigned char uc_SendType, MP_REPORT_EVENT_SDL *pReport, uint16 *CmdTxAsync_Length)
{
    //MP_REPORT_EVENT_SDL    st_Event_SDL;  //  先状态后紧跟7点SDL数据电压、电流、充电容量、充电能量、放电容量、放电能量、内阻;
#if  USE_AS_AUXMCU == 0
//     UWord16 uw16_Count = sizeof(St_Data_Pairs);  //  单点长度:20字节;
    UWord16 uw16_Length = CMD_HOLDHAND_LENGTH + sizeof(MP_REPORT_EVENT) - 4; //  包前增加16字节 + 128字节
    if(uw16_Length > MAX_CMD_LOGDATA_BUFFER)
    {
        *CmdTxAsync_Length = 0;
        return;
    }

    MPScheduleReport_Event(uw16_ChannelNumber, uc_SendType, &m_pReport->m_Report_Event, CmdTxAsync_Length);

    //memcpy(&St_UseTxBuff->m_CmdTxAsync_Buffer[uw16_Length], &pReport->m_btNone_16[0], 4);

    //St_SDL_Point MySdlPoint;
//	UWord16 uw_MetaType = MetaCode_PV_Voltage;

//    UWord16 uw16_k = 1;
// 	for(UWord16 uw16_k = 0; uw16_k < pReport->m_ItemCount_SDL; uw16_k++)   //  取7种数据变量类型;
// 	{
// 		switch(uw16_k)  //  转换获取变量类型
// 		{
// 		case 0:
// 			uw_MetaType = MetaCode_PV_Voltage;
// 			break;
// 		case 2:
// 			uw_MetaType = MetaCode_PV_ChargeCapacity;
// 			break;
// 		case 6:
// 			uw_MetaType = MetaCode_PV_InternalResistance;
// 			break;
// 		default:
// 			++uw_MetaType;   //  取7种数据变量类型:电压、电流、充电容量、充电能量、放电容量、放电能量、内阻;
// 			break;
// 		}


//     unsigned long DataPoints = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DataPoints].m_Value.m_uInt32 ++;  // increae log data point
// 		unsigned long DataFlag = ChannelInfo_Manager::m_DataFlag[uw16_ChannelNumber];//1804251 zyx //ul_SpecialPoint;
//     ChannelInfo_Manager::ClearDataFlag(uw16_ChannelNumber, 0xFFFFFFFF );
// 		IVSDL::GetPoint(uw16_ChannelNumber,SDL_DATA_FOR_LOG,SDL_DATA_TYPE_LOG_C_V_CAP_ENG,DataPoints, DataFlag, &pReport->m_st_SDLData);  // 烤备数据
    IVSDL::GetPoint(uw16_ChannelNumber, SDL_DATA_FOR_LOG, SDL_DATA_TYPE_LOG_C_V_CAP_ENG, 0, 0, &pReport->m_st_SDLData);


    //uw16_Length = *CmdTxAsync_Length + uw16_Count;   // 状态数据先去除增加的检验,加上预留补齐。;// 多点总长度 = 单点长度 * 点数;  20*7=140字节   // * uw16_k
    //SDLPointChang(&MySdlPoint, &pReport->m_st_SDLData);
// 	}

    //if(pReport->m_ItemCount_SDL == CMD_TXSEND_SDL_POINT)   //  20130930  yy  解决上传过多的数据问题
// 	if(pReport->m_ItemCount_SDL > 0)   //  20130930  yy  解决上传过多的数据问题
// 	{
    *CmdTxAsync_Length = sizeof(MP_REPORT_EVENT_SDL);// 保存准备发送数据包的数据长度;
    //}
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_EVENT_CONTENT);
#endif
}

////////
// MPScheduleReport_LogSDL(UWord16 uw16_ChannelNumber, MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length)
//  : 网络命令第二信道报告Schedule记录SDL数据处理函数
// Input: uw16_ChannelNumber 网络第二信道报告Schedule数据通道号;
//        pReport 网络第二信道报告Schedule数据结构指针;
//        CmdTxAsync_Length 网络第二信道报告Schedule数据包长度指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_LogSDL(UWord16 uw16_ChannelNumber, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport, uint16 *CmdTxAsync_Length) //mits8
{
#if  USE_AS_AUXMCU == 0
    //MP_REPORT_LOGDATA_SDL st_SDLData;
    UWord16 uw16_Count = sizeof(St_Data_Pairs_4BytesCycleIdx);  //  单点长度:13字节;
    UWord16 uw16_Max = QUEUE_DEPTH(IVSDL::Buffer);	  //  队列中已有的点数
    UWord16 uw16_Min = (MAX_CMD_LOGDATA_BUFFER / uw16_Count) & MAX_SDL_PAIRS_NUMBER; // 一次性可取的点数
    Word16 w16_Remain = uw16_Max - uw16_Min;  //  单点长度
    if(w16_Remain > 0)
    {
        uw16_Max = uw16_Min;
    }

    pReport->m_dwCmd = CMD_REPORT_BASIC_TABLE_4BYTES_CYCLE_IDX;//CMD_REPORT_BASIC_TABLE; //CMD_SCHEDULE_REPORT_SDL;
    pReport->ItemCount = (unsigned char) uw16_Max;
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif
    UWord16 uw16_Length = sizeof(MP_REPORT_SDL_4BYTESCYCLEIdx_PACK) - (uw16_Count * MAX_SDL_PAIRS_NUMBER);   //  总字节数 = 包头包信息; MP_REPORT_LOGDATA_SDL
    uw16_Length += uw16_Count * uw16_Max;  //  取点总字节数=单点长度*已取的点数;

    unsigned char  bEmpty;
    St_SDL_Point_4BytesCycleIdx   St_ObjBuff;
    for(uint16 uw16_k = 0; uw16_k < uw16_Max; uw16_k++)
    {
        DEQUEUE(IVSDL::Buffer, SDL_QUEUE_DEPTH, St_ObjBuff, bEmpty);
        SDLPointChang(&St_ObjBuff, &pReport->Point[uw16_k]);
//        if(pReport->Point[uw16_k].u32DataBitFlag > 0xFFFF)
//            int i = 0;
//        else
//           if((pReport->Point[uw16_k].u32DataBitFlag & SET_VALUE_POINT) != 0)
//                int i = 2;
    }

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_LOG_SDL);
#endif
}


inline void EthernetParse::SDLPointChang(St_SDL_Point_4BytesCycleIdx * S, St_Data_Pairs_4BytesCycleIdx * D) //mits8
{
    D->u16Us100 = S->Time.Us100;
    D->u32Second = S->Time.Second;
    D->ucType = S->ucType;//display = 0;idle = 1;log = 2;
    D->bIV = 1;
    D->ucSensor = 0;
    D->ucDataLogFormat = S->ucDataLogFormat;
    D->ucChannelIdx =  S->u16ChannelIdx;
    D->ucDataCount = S->ucDataCount;
    //if( S->ucType == SDL_DATA_FOR_LOG) //zyx 2212081
    D->u32DataPoint = S->u32DataPoint;
    D->u32TestID = S->u32TestID;
//     if(D->u32TestID == 0)
//         int i = 0;
    D->dTestTime = S->dTestTime;
    D->dStepTime = S->dStepTime;
    D->u32Cycle_Idx = S->u32Cycle_Idx;
    D->u16Step_Idx = S->u16Step_Idx;
    D->u32DataBitFlag = S->u32DataBitFlag;
    D->ucDataCount = S->ucDataCount;
    if(D->ucDataLogFormat != SDL_DATA_TYPE_LOG_3_META_VAR)
    {
        for(int i = 0; i < 6; i++)
            D->un_data.fValue[i] = S->un_data.fValue[i];
    }
    else
    {
        for(int i = 0; i < D->ucDataCount/2; i++)
        {
            D->un_data.dat[i].ucType = S->un_data.dat[i].ucType;
            D->un_data.dat[i].fValue = S->un_data.dat[i].fValue;
        }
    }

#if  USE_AS_AUXMCU == 0
    if(S->Time.Us100 == 0)//Record last IV logging time point
    {
        Smart_Battery::LastIVtime.Us100 = 9999;
        Smart_Battery::LastIVtime.Second = S->Time.Second - 1;
    }
    else
    {
        Smart_Battery::LastIVtime.Us100 = S->Time.Us100 - 1;
        Smart_Battery::LastIVtime.Second = S->Time.Second;
    }
#endif
}


////////
// SDLPointChang(St_SDL_Point * S, St_SDL_Point_Pack * D)
//  : 网络命令第二信道存储缓冲点转换发送点处理函数
// Input: S 存储缓冲点数据结构指针;
//        D 发送点指针;
// Return: N/A
///////
/*inline void EthernetParse::SDLPointChang(St_SDL_Point * S, St_SDL_Point_Pack * D)  //mits8
{
	D->u16Us100 = S->Time.Us100;
	D->u32Second = S->Time.Second;
	D->ucType = S->ucType;//display = 0;idle = 1;log = 2;
	D->bIV = 1;
	D->ucValueType = S->ucValueType;
	D->fValue = S->fValue;
	D->ucChannelIdx =  S->u16ChannelIdx;

#if DEBUG_TEST_COMMDATA_MODE == 1
	switch(S->ucValueType)
	{
	case MetaCode_PV_Voltage:
		if(S->ucType == SDL_DATA_TYPE_LOG)
		{
			uw32_TestLogD[S->u16ChannelIdx]++;  // 通道已记录的点数
			D->fValue = uw32_TestLogD[S->u16ChannelIdx] * 0.000001;  // 通道已记录的点数
			//D->fValue = uw32_TestEventNum * 0.000001;  // 已发信息号
			//D->fValue = uw32_TestBlock1 * 0.000001; // 阻塞次数
			//D->fValue = uw32_TestDataPack * 0.000001;   // 已发数据包的包号
			//D->fValue = uw32_TestSumRevice * 0.000001;
		}
		break;
	case MetaCode_PV_Current:
		D->fValue = uw32_TestPackSum  * 0.000001;  //  已发包的包号
		break;

		//----------------------------------------
	case MetaCode_PV_ChargeCapacity:
		if(S->ucType == SDL_DATA_TYPE_LOG)
			D->fValue = uw32_TestLogD[S->u16ChannelIdx];  // 通道已记录的点数
		break;
	case MetaCode_PV_DischargeCapacity:
		D->fValue = uw32_TestPackSum;  //  已发包的包号
		break;
	case MetaCode_PV_ChargeEnergy:
		D->fValue = uw32_TestEventNum;  // 已发信息号
		break;
	case MetaCode_PV_DischargeEnergy:
		D->fValue = uw32_TestBlock1; // 阻塞次数
		break;
	case MetaCode_PV_InternalResistance:
		D->fValue = uw32_TestDataPack;   // 已发数据包的包号
		// D->fValue = uw32_TestSumRevice;
		break;
	default:
		break;
	}
#endif
*/


////////
// MPScheduleReport_Pulse_LogData(MP_REPORT_PULSE_LOGDATA *pReport, uint16 *CmdTxAsync_Length)
//  : 网络命令第二信道报告Schedule记录脉冲数据处理函数
// Input: pReport 网络第二信道报告Schedule数据结构指针;
//        CmdTxAsync_Length 网络第二信道报告Schedule数据包长度指针;
// Return: N/A
///////
void EthernetParse::MPScheduleReport_Pulse_LogData(MP_REPORT_PULSE_LOGDATA *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    St_Pulse_PackInfo * st_PackInfo = (St_Pulse_PackInfo *) &m_Point_Temp[0];

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_PULSE_LOGDATA;
    pReport->m_wChannelNumber = st_PackInfo->m_wChannelNumber;
    pReport->m_btIfPartialPusle = st_PackInfo->m_btIfPartialPusle;
    pReport->m_dwPulseCycleID = st_PackInfo->m_dwPulseCycleID;
    pReport->m_stPulseBeginTime.Second = st_PackInfo->m_PulseStartTime.Second;
    pReport->m_stPulseBeginTime.Us100 = st_PackInfo->m_PulseStartTime.Us100;
    pReport->m_Time.Second = st_PackInfo->m_PulseStartTime.Second;
    pReport->m_Time.Us100 = st_PackInfo->m_PulseStartTime.Us100;

    pReport->m_btFirstPointID = st_PackInfo->m_btFirstPointID;
    pReport->m_btIfHaveCapEng = st_PackInfo->m_btIfHaveCapEng;

    //2014/12/15,add start by hepeiqing,to fix pulse log data bug in access mode
    pReport->u32TestID = Schedule_Manager_task::m_dwTestID[0];//1806081 zyx
    pReport->m_wStepID = ChannelInfo_Manager::ChannelSchedule[0].m_RunningStepIdx;
    pReport->m_dwCycleID = ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_CycleIndex].m_fValue;
    if(pReport->m_dwCycleID > 0)
        pReport->m_dwCycleID --;



    pReport->m_StepStartTime.Second = ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StartTime.Second;
    pReport->m_StepStartTime.Us100 = ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StartTime.Us100;

    pReport->m_TestStartTime.Second = ChannelInfo_Manager::ChannelSchedule[0].m_StartTime.Second;
    pReport->m_TestStartTime.Us100 = ChannelInfo_Manager::ChannelSchedule[0].m_StartTime.Us100;

    //2014/12/15,add end   by hepeiqing,to fix pulse log data bug in access mode

    if(Pulse::m_Info.m_SampType == 0)
        pReport->m_btPointCount = Pulse::m_Info.m_StageCount * 2;
    else
        pReport->m_btPointCount = Pulse::m_Info.m_StageCount;

    pReport->u32DataPoint = ++ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_DataPoints].m_Value.m_uInt32;
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_DataPoints].m_Value.m_uInt32 += pReport->m_btPointCount - 1;


#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif
    UWord16 uw16_Length = sizeof(MP_REPORT_PULSE_LOGDATA) - (sizeof(St_PULSE_VCT) * PACK_MAX_PULSE_POINT);   //  总字节数 = 包头包信息;
    uw16_Length += sizeof(St_PULSE_VCT) * pReport->m_btPointCount;  //  取点总字节数=单点长度*已取的点数;

    unsigned char  PointIdx = 0;
    for(unsigned char StgIdx = 0; StgIdx < Pulse::m_Info.m_StageCount; StgIdx++)
    {
        if(Pulse::m_Info.m_SampType == 0)
        {   //
            pReport->m_fV_C_T[PointIdx].m_fCurrent = Pulse_LogLimit_task::Backup_raw_data[0][SAVE_2_HEAD][StgIdx].m_fCurrent;
            pReport->m_fV_C_T[PointIdx].m_fVoltage = Pulse_LogLimit_task::Backup_raw_data[0][SAVE_2_HEAD][StgIdx].m_fVoltage;
            pReport->m_fV_C_T[PointIdx].m_dwPointTime = Pulse::stage_info[SAVE_2_HEAD][StgIdx].m_TimeInUS;
            PointIdx ++;
        }

        pReport->m_fV_C_T[PointIdx].m_fCurrent = Pulse_LogLimit_task::Backup_raw_data[0][SAVE_2_TAIL][StgIdx].m_fCurrent;
        pReport->m_fV_C_T[PointIdx].m_fVoltage = Pulse_LogLimit_task::Backup_raw_data[0][SAVE_2_TAIL][StgIdx].m_fVoltage;
        pReport->m_fV_C_T[PointIdx].m_dwPointTime = Pulse::stage_info[SAVE_2_TAIL][StgIdx].m_TimeInUS;
        PointIdx ++;
    }
    //////////////////////////////////////////////////////////////////
//     if(Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber].Second == 0 && Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber].Us100 == 0)
//     {
//         CommonFunc::GetTimeTicksUpdate( &Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber] );
//     }
//     else
//     {
//         MsTime OStime = {0,0};
//         unsigned long long PulseTime = Pulse::stage_info[SAVE_2_TAIL][Pulse::m_Info.m_StageCount-1].m_TimeInUS/100;
//         if(PulseTime > 10000)
//             OStime.Second = PulseTime/10000;
//         OStime.Us100 = PulseTime % 10000;
//         CommonFunc::add_MsTime(&Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber],&OStime);
// //         if(OS_ServeProcess::bTimeCompare(OS_ServeProcess::OS_Time,Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber]) != 0)
// //          {
//             memcpy(&OS_ServeProcess::OS_Time, &Pulse_LogLimit_task::LastReportTime[st_PackInfo->m_wChannelNumber], sizeof(MsTime));
// //          }
//     }
//      CommonFunc::GetTimeTicksUpdate( &pReport->m_Time );
    //////////////////////////////////////////////////////////////////
    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_uBuildInVoltCount; i++)
            pReport->m_fBuildInVolt[i] = ChannelInfo_Manager::ChannelInfo[0].m_fBuildInVolt_Pulse[i];
        //pReport->m_fBuildInVolt[i] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(0,MetaCode_PV_BuildIn_V1 + i);
    }

    if(st_PackInfo->m_btIfHaveCapEng)   // 本包是否包含电量能量数据。
    {
        pReport->m_fCCapOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_ChargeCapacity].m_Value.m_double;
        pReport->m_fDCapOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_DischargeCapacity].m_Value.m_double;
        pReport->m_fCEngOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_ChargeEnergy].m_Value.m_double;
        pReport->m_fDEngOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_DischargeEnergy].m_Value.m_double;
    }

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_PULSE_LOG_DATA);
    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
#endif
    //下面是09/09之前的代码
    //	#if  USE_AS_AUXMCU == 0
    //	UWord16 uw16_Count = sizeof(St_PULSE_VCT);  //  单点长度:12字节;
    //	UWord16 uw16_Max = PACK_MAX_PULSE_POINT;	  //  最大点数
    //    UWord16 uw16_Min;
    //    St_Pulse_PackInfo * st_PackInfo = (St_Pulse_PackInfo *) &m_Point_Temp[0];
    //    if( st_PackInfo->m_btIfPartialPusle )  //  特殊标志说明分批指定上传点
    //	    uw16_Min = (Pulse::m_Info.m_StageCount * 2) & PACK_MAX_PULSE_POINT; // 一次性可取的点数
    //    else
    //    {
    //         uw16_Min = st_PackInfo->m_btPointCount; // 特殊标志说明分批指定上传点，一次性可取的点数
    //    }
    //	Word16 w16_Remain = uw16_Max - uw16_Min;  //  单点长度
    //	if(w16_Remain > 0)
    //	{
    //		uw16_Max = uw16_Min;
    //	}
    //
    //	pReport->m_dwCmd = CMD_SCHEDULE_REPORT_PULSE_LOGDATA;
    //    pReport->m_btPointCount = (unsigned char) uw16_Max;
    //    pReport->m_wChannelNumber = st_PackInfo->m_wChannelNumber;
    //    pReport->m_btIfPartialPusle = st_PackInfo->m_btIfPartialPusle;
    //    pReport->m_dwPulseCycleID = st_PackInfo->m_dwPulseCycleID;
    //    pReport->m_stPulseBeginTime.Second =  Pulse::stage_info[st_PackInfo->m_btbuf_Idex][st_PackInfo->m_btFirstPointID].m_StageStartTime.Second;
    //    pReport->m_stPulseBeginTime.Us100 =  Pulse::stage_info[st_PackInfo->m_btbuf_Idex][st_PackInfo->m_btFirstPointID].m_StageStartTime.Us100;
    //    pReport->m_btFirstPointID = st_PackInfo->m_btFirstPointID;
    //    pReport->m_btIfHaveCapEng = st_PackInfo->m_btIfHaveCapEng;
    //
    //#if SOFTWARE_DOG_ENABLE == 1
    //	pReport->m_u16Token = m_u16Token + 1;
    //#endif
    //	UWord16 uw16_Length = sizeof(MP_REPORT_PULSE_LOGDATA) - (uw16_Count * PACK_MAX_PULSE_POINT);   //  总字节数 = 包头包信息;
    //    uw16_Length += uw16_Count * uw16_Max;  //  取点总字节数=单点长度*已取的点数;
    //
    //	unsigned char  bEmpty;
    //    unsigned char  m_uStageNum = 0;
    //	for(uint16 uw16_k = 0; uw16_k < uw16_Max; uw16_k++)
    //	{
    //        if( st_PackInfo->m_btIfPartialPusle )  //  特殊标志说明分批指定上传点
    //           m_uStageNum += uw16_k;
    //        else
    //        {
    //           if(m_uStageNum != 0)
    //               m_uStageNum = st_PackInfo->m_btFirstPointID%2 + st_PackInfo->m_btFirstPointID/2 - 1;   // 从定点位置转变成台阶编号
    //        }
    //
    //		pReport->m_fV_C_T[uw16_k].m_fCurrent = Pulse::raw_data[st_PackInfo->m_btbuf_Idex][uw16_k%2][m_uStageNum].m_fCurrent;
    //        pReport->m_fV_C_T[uw16_k].m_fVoltage = Pulse::raw_data[st_PackInfo->m_btbuf_Idex][uw16_k%2][m_uStageNum].m_fVoltage;
    //        pReport->m_fV_C_T[uw16_k].m_dwPointTime = Pulse::raw_data[st_PackInfo->m_btbuf_Idex][uw16_k%2][m_uStageNum].m_uElapsedTimeWithinStage;  // 时间需要进行转换
    //
    //        if((m_uStageNum == 0)&&( (uw16_k%2) == 1))
    //           m_uStageNum ++;   //  保正能够转换
    //    }
    //
    //    if(st_PackInfo->m_btIfHaveCapEng)   // 本包是否包含电量能量数据。
    //    {
    //        pReport->m_fCCapOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_ChargeCapacity].m_Value.m_double;
    //        pReport->m_fDCapOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_DischargeCapacity].m_Value.m_double;
    //        pReport->m_fCEngOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_ChargeEnergy].m_Value.m_double;
    //        pReport->m_fDEngOfLastPoint = ChannelInfo_Manager::MetaVariable2[st_PackInfo->m_wChannelNumber][MetaCode_PV_DischargeEnergy].m_Value.m_double;
    //    }
    //
    //	*CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    //    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_PULSE_LOG_DATA);
    //    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
    //	#endif
}


////////
// MPReport_IV_BuildInAux_LogData(MP_REPORT_BUILDINAUX_LOGDATA *pReport, uint16 *CmdTxAsync_Length)
//  : 网络命令第二信道报告Schedule记录脉冲数据处理函数
// Input: pReport 网络第二信道报告Schedule数据结构指针;
//        CmdTxAsync_Length 网络第二信道报告Schedule数据包长度指针;
// Return: N/A
///////
void EthernetParse::MPReport_IV_BuildInAux_LogData(MP_REPORT_BUILDINAUX_LOGDATA *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    //St_BUILDINAUX_VALUE * st_Point[8];  // 暂时用不到8，仅用1
    //st_Point[0] = (St_BUILDINAUX_VALUE *) &EthernetParse::m_Point_Temp[0];

    St_BUILDINAUX_VALUE * st_Point = (St_BUILDINAUX_VALUE *) &EthernetParse::m_Point_Temp[0];

    pReport->m_dwCmd = CMD_REPORT_BUILDINAUX_LOGDATA;
    MsTime SysTime = CommonFunc::GetSystemTime();
    pReport->m_stTime.Second = SysTime.Second;     //  04/22/2015  yy
    pReport->m_stTime.Us100 = SysTime.Us100;
    pReport->ItemCount = 1;

#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif

    UWord16 uw16_Length = sizeof(MP_REPORT_BUILDINAUX_LOGDATA) - (sizeof(St_BUILDINAUX_VALUE) << 3); //  总字节数 = 包头包信息;
    uw16_Length += sizeof(St_BUILDINAUX_VALUE) * pReport->ItemCount;  //  取通道总字节数=单点长度*已取的通道数;


    unsigned char  PointIdx = 0;
    for(unsigned char StgIdx = 0; StgIdx < pReport->ItemCount; StgIdx++)
    {
        pReport->m_stBuildInAux_Value[StgIdx] = *st_Point;
        PointIdx ++;
    }

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_IV_BUILDINAUX_LOGDATA);
    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
#endif
}

void EthernetParse::MPScheduleReport_LogSDL_Cycle(UWord16 uw16_ChannelNumber, MP_REPORT_LOGDATA_SDL_CYCLE *pReport, uint16 *CmdTxAsync_Length)
{
#if  USE_AS_AUXMCU == 0
    //MP_REPORT_LOGDATA_SDL st_SDLData;
    UWord16 uw16_Count = sizeof(ST_SCHEDULE_VARIABLE);  //  单点长度:13字节;
//     UWord16 uw16_Max = MAX_SEND_METAVARIABLE;	  //  队列中已有的点数
//     UWord16 uw16_Min = 10; // 一次性可取的点数
//     Word16 w16_Remain = uw16_Max - uw16_Min;  //  单点长度
//     if(w16_Remain > 0)
//     {
//         uw16_Max = uw16_Min;
//     }

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_LOGDATA_CYCLE;
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif
    UWord16 uw16_Length = sizeof(MP_REPORT_LOGDATA_SDL_CYCLE) - (uw16_Count * MAX_SEND_METAVARIABLE);   //  总字节数 = 包头包信息;

    if(uw16_ChannelNumber >= MAXCHANNELNO)
        uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
    pReport->u32TestID = Schedule_Manager_task::m_dwTestID[uw16_ChannelNumber]; //mits8
    pReport->u32DataPoint = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DataPoints].m_Value.m_uInt32;//IVSDL::m_dwStepPoint[uw16_ChannelNumber] ++;
    if(pReport->u32DataPoint == 0) //zyx 1810152
        pReport->u32DataPoint += 1;
    pReport->m_wChannelNumber   = uw16_ChannelNumber;
    pReport->m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
    pReport->m_wStepID = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_RunningStepIdx;

    pReport->m_dwCycleID = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_CycleIndex].m_fValue;
    if(pReport->m_dwCycleID > 0)
        pReport->m_dwCycleID --; // 暂时为上位机显示正常循环次数提供的处理。

    MsTime SysTime = CommonFunc::GetSystemTime();
    double  d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_TestTime,SysTime);
    pReport->m_st_TestTime.m_U64Time = d_Value * (10E5f);
    d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_Steptime,SysTime);
    pReport->m_st_StepTime.m_U64Time = d_Value * (10E5f);
    pReport->m_st_DataTime.Second = SysTime.Second;
    pReport->m_st_DataTime.Us100 = SysTime.Us100;

    //uw16_Count = 0;
    int Count = 0;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_Voltage;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_Current;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Current].m_fValue;

    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_ChargeCapacityTime;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_ChargeCapacityTime); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacityTime].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_DischargeCapacityTime;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_DischargeCapacityTime); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacityTime].m_fValue;

    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_ChargeCapacity;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacity].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_DischargeCapacity;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacity].m_fValue;

    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_ChargeEnergy;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeEnergy].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_DischargeEnergy;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeEnergy].m_fValue;

    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_InternalResistance;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_InternalResistance].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_PV_ACR;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ACR].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_TC_Counter1;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter1); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacityTime].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_TC_Counter2;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter2); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacityTime].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_TC_Counter3;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter3); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_ChargeCapacityTime].m_fValue;
    pReport->m_st_Variable[Count].m_wVariable_Code = MetaCode_TC_Counter4;
    pReport->m_st_Variable[Count++].m_fVariable_Value = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_TC_Counter4); //ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_DischargeCapacityTime].m_fValue;
    uw16_Length += (uw16_Count * Count);  //  取点总字节数=单点长度*已取的点数;
    pReport->m_wVariable_Count = Count;

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_LOG_SDL_CYCLE);
    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
#endif
}
#if  USE_AS_AUXMCU == 0
void EthernetParse::MPReport_BuiltInLogData(MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length)
{
    UWord16 uw16_Count = sizeof(St_SDL_Point_Pack);  //  单点长度:13字节;

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_SDL;
    UWord16 uw16_Length = sizeof(MP_REPORT_LOGDATA_SDL) - (uw16_Count * MAX_AUX_SDL_POINT);   //  总字节数 = 包头包信息;
    memcpy((void *)&pReport->Point[0], (void *)&m_Point_Temp[0], sizeof(St_SDL_Point_Pack)); // 传送结构对结构  V/T
    pReport->ItemCount = 2; // 仅2点传送  V/T, DXDT,
    uw16_Length += uw16_Count * 2;  //  取点总字节数=单点长度*已取的点数;
    memcpy((void *)&pReport->Point[1], (void *)&m_Point_Temp[0], sizeof(St_SDL_Point_Pack)); // 传送结构对结构  DXDT
    switch(IVSDL::st_BuiltIn_Aux_Data.m_Chn_Info.m_AuxType)
    {
    case AUX_VOLTAGE:
        pReport->Point[1].ucValueType = AUX_VOLTAGE_DVDT_DATA;
        break;
    case AUX_TEMPERATURE:
        pReport->Point[1].ucValueType = AUX_TEMPERATURE_DTDT_DATA;
        break;
    default:
        break;
    }
    pReport->Point[1].fValue = IVSDL::st_BuiltIn_Aux_Data.aux_dx_dt;  //  DXDT
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif
    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_AUX_LOGDATA);
    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
}
#endif
#if  USE_AS_AUXMCU == 1
////////
// MPReport_AuxLogData(MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length)
//  : 网络命令第二信道报告辅助通道记录数据处理函数
// Input: pReport 网络第二信道报告辅助通道数据结构指针;
//        CmdTxAsync_Length 网络第二信道报告辅助通道数据结构包长度指针;
// Return: N/A
///////
void EthernetParse::MPReport_AuxLogData(MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length)
{
    UWord16 uw16_Count = sizeof(St_SDL_Point_Pack);  //  单点长度:13字节;

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_SDL;
    UWord16 uw16_Length = sizeof(MP_REPORT_LOGDATA_SDL) - (uw16_Count * MAX_PACK_LOGPOINT);   //  总字节数 = 包头包信息;
    memcpy((void *)&pReport->Point[0], (void *)&m_Point_Temp[0], sizeof(St_SDL_Point_Pack)); // 传送结构对结构  V/T
    if(  (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_DI)
            || (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_DO) || (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_AO)
            || (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_HUMIDITY)
            || (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_FLOW_RATE)
            || (Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType == AUX_EXTERNALCHARGE))
    {
        pReport->ItemCount = 1; // 仅1点传送
        uw16_Length += uw16_Count * 1;  //  取点总字节数=单点长度*已取的点数;
    }
    else
    {
        pReport->ItemCount = 2; // 仅2点传送  V/T, DXDT,
        uw16_Length += uw16_Count * 2;  //  取点总字节数=单点长度*已取的点数;
        memcpy((void *)&pReport->Point[1], (void *)&m_Point_Temp[0], sizeof(St_SDL_Point_Pack)); // 传送结构对结构  DXDT
        switch(Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType)
        {
        case AUX_VOLTAGE:
            pReport->Point[1].ucValueType = AUX_VOLTAGE_DVDT_DATA;
            break;
        case AUX_TEMPERATURE:
            pReport->Point[1].ucValueType = AUX_TEMPERATURE_DTDT_DATA;
            break;
        case AUX_PRESSURE:
            pReport->Point[1].ucValueType = AUX_PRESSURE_DPDT_DATA;
            break;
        default:
            break;
        }
        pReport->Point[1].fValue = Aux_DataManager::st_Aux_Data.aux_dx_dt;  //  DXDT
        if(Aux_DataManager::m_bLogAuxDxdt == 0)                  //160429
            pReport->Point[1].ucType = SDL_DATA_FOR_DISPLAY;
    }

#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_AUX_LOGDATA);
    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
}

void EthernetParse::MPReport_AuxGroupLogData(MP_REPORT_GRPLOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length)
{
    UWord16 uw16_Count = sizeof(St_SDL_OnePoint);
    UWord16 cmdsize = sizeof(MP_REPORT_GRPLOGDATA_SDL);
    UWord16 grouppointsize = sizeof(St_SDL_GroupPoints);

    pReport->m_dwCmd = CMD_SCHEDULE_REPORT_GROUPLOGDATA;

    UWord16 uw16_Length = sizeof(MP_REPORT_GRPLOGDATA_SDL) - (uw16_Count * 32);
    memcpy((void*)&pReport->groupedPoints, (void*)&m_Point_Temp[0], sizeof(St_SDL_GroupPoints));
    //pReport->itemcount = pReport->groupedPoints.itemCount;

#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = m_u16Token + 1;
#endif

    UWord16 itemcount = pReport->groupedPoints.itemCount;
    *CmdTxAsync_Length = uw16_Length + (uw16_Count * itemcount);
    //*CmdTxAsync_Length = sizeof(MP_REPORT_GRPLOGDATA_SDL);
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_AUX_GROUPLOGDATA);

    PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据

    //St_SDL_GroupPoints* st_gPoint;
    //st_gPoint = (St_SDL_GroupPoints*)&EthernetParse::m_Point_Temp[0];
    //st_gPoint->itemCount = 0;
}

void EthernetParse::MPReport_AuxACIMData(MP_REPORT_ACIM_DATA *pReport, uint16 *CmdTxAsync_Length)
{
    pReport->m_dwCmd = CMD_REPORT_ACIM_DATA ;
    UWord16 uw16_Length = sizeof(MP_REPORT_ACIM_DATA);
    pReport->m_dwSendSN = m_dwSend_SN;         //  发送时间编号信息
    if(Aux_ACIMTest::b_lastPoint)
    {
        pReport->m_lastPoint = true;
        Aux_ACIMTest::b_lastPoint = false;
    }
    else
        pReport->m_lastPoint = false;
//     memset((void *)&pReport->m_Point[0],0,sizeof(St_ACIM_Result_data));
    memset((void *)&pReport->m_Point[0], 0, sizeof(MP_REPORT_ACIM_DATA));

    if(Aux_DDS_ACIM_ACR_408888_Base::isACIM)
    {
        for (unsigned char i=0; i<Aux_ACIMTest::ACIMTestNum; i++)
        {
            pReport->m_Point[i].m_Frequency = Aux_ACIMTest::Frequency_List[i];// 1000;
            pReport->m_Point[i].m_Zmod= Aux_ACIMTest::ACIM_Mac[i];
            pReport->m_Point[i].m_Zphase = Aux_ACIMTest::ACIM_Pac[i];
        }
        pReport->m_Point[Aux_ACIMTest::ACIMTestNum].m_Frequency = 0;	// 标记无效点
    }
    else
    {
        pReport->m_Point[0].m_Frequency =1000;
        pReport->m_Point[0].m_Zmod= Aux_ACIMTest::ACIM_Rac_Avg;	//2019.01.16
        pReport->m_Point[0].m_Zphase= 0;	//2019.01.16
        pReport->m_Point[1].m_Frequency =0;
    }

    *CmdTxAsync_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, CmdTxAsync_Length, SEND_AUX_ACIMDATA);
    //PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据
}
////////
// MP_RSN_ErrorAckEvent(MP_SCHEDULE_ASSIGN *pCmd) : 网络命令辅助通道配置任务管理RSN错误信息确认事件号命令处理函数
// Input: pCmd 辅助通道配置任务管理RSN错误信息确认事件号命令结构指针;
// Return: N/A
///////
void EthernetParse::MP_RSN_ErrorAckEvent(MP_RSN_ERROR_ACK_EVENT *pCmd)
{

    UChar8 uc_DownLoad_Success = 0;


// 	m_ResultFeedback.m_dwCopyCmd = pCmd->m_dwCmd;// 备份准备回传信息
// 	m_ResultFeedback.m_dwCopyCmd_Extend =	pCmd->m_dwCmd_Extend;
// 	m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;
// 	m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 配置错误代码

    CmdSetToFeedback(&pCmd->m_dwCmd, &pCmd->m_dwCmd_Extend, CONFIRM_RESULT_FAIL);

//     uc_DownLoad_Success = 1;
//     if(uc_DownLoad_Success == 1)  //  使用区域判断  0：单包或无意义 1：第一区域多包信息使用标志
//     {
    // 已经接收完成
    for(unsigned char uc_i = 0; uc_i < pCmd->m_btTotal; uc_i ++)
    {
        if(pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_btResult)
        {
            if(( pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_btControl == MESSAGE_UPDATA_RSN)
                    || (pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_btControl == MESSAGE_STOP_REPORT))
            {
                St_DataMessage  st_Message;
                st_Message.uc_AuxType = pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_Error_Message.m_AuxBoard.m_btAuxType;
                st_Message.uc_BoardID = pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_Error_Message.m_AuxBoard.m_btBoard_ID;
                st_Message.uc_MsgSource = MESSAGE_SOURCE_PC_ORDER;
                st_Message.uw16_ChannelID = 0; // pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_Error_Message.m_AuxBoard.m_u8ChCount;
                st_Message.uc_MsgCode = pCmd->m_Confirm_Message[uc_i].m_AuxConfirm.m_btControl;  // MESSAGE_UPDATA_RSN;
                AuxConfigRSN::PushMsg_RSN( MODEL_BOARD, &st_Message );
            }
        }
    }
//     }

    m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;
    //MPCMD_Feedback(&m_pFeedBack->m_Confirm_Feedback);
}
#endif

////////
// MPReport_ErrorMessage(St_REPORT_TOPC_ERROR *pReport)
//  : 网络命令第二信道报告错误处理函数
// Input: pReport 网络第二信道报告数据结构指针;
//
// Return: N/A
///////
void EthernetParse::MPReport_ErrorMessage(MP_REPORT_TOPC_ERROR *pReport)
{
    Un_ERROR_MESSAGE * st_SPA1 = (Un_ERROR_MESSAGE *) &m_Point_Temp[0];
    UWord16 uw16_Count = sizeof(Un_ERROR_MESSAGE);  //  单个长度:4字节;
    pReport->m_dwCmd = CMD_REPORT_TOPC_ERROR;

#if  USE_AS_AUXMCU == 1
    pReport->m_btTotalCout = 1; // 仅单个传送
    pReport->m_btSendSource = MESSAGE_SOURCE_BOARD_RSN;

    UWord16 uw16_Length = sizeof(MP_REPORT_TOPC_ERROR) - (uw16_Count * MAXIV4AUXCHANNELNO);   //  总字节数 = 包头包信息;
    uw16_Length += uw16_Count * pReport->m_btTotalCout;  //  取点总字节数=单点长度*已取的点数;

    for(unsigned char uc_i = 0; uc_i < pReport->m_btTotalCout ; uc_i ++)
    {
        memcpy(&pReport->m_Error_Message[uc_i], st_SPA1, sizeof(Un_ERROR_MESSAGE)); // 仅单个传送结构对结构
        pReport->m_btError_Code[uc_i] = AuxConfigRSN::uc_Report_RSNError[pReport->m_Error_Message[uc_i].m_AuxBoard.m_btBoard_ID];// 报告错误代码
    }

    if(pReport->m_btError_Code[0] == ERROR_RSN_NONE)
    {
        pReport->m_btResult = CONFIRM_RESULT_SUCCESS;
    }
    else
    {
        pReport->m_btResult = CONFIRM_RESULT_FAIL;
    }
#else
    pReport->m_btTotalCout = 1; // 仅单个传送
    pReport->m_btSendSource = MESSAGE_SOURCE_IV_WARNNING;

    UWord16 uw16_Length = sizeof(MP_REPORT_TOPC_ERROR) - (uw16_Count * MAXIV4AUXCHANNELNO);   //  总字节数 = 包头包信息;
    uw16_Length += uw16_Count * pReport->m_btTotalCout;  //  取点总字节数=单点长度*已取的点数;

    for(unsigned char uc_i = 0; uc_i < pReport->m_btTotalCout ; uc_i ++)
    {
        memcpy(&pReport->m_Error_Message[uc_i], st_SPA1, sizeof(Un_ERROR_MESSAGE)); // 仅单个传送结构对结构
    }

    pReport->m_btResult = CONFIRM_RESULT_FAIL;
#endif

    // 	DataTx_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    //  HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &DataTx_Length, SEND_ERROR_MESSAGE);
    //  PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据


    EventTx_Length  = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &EventTx_Length, SEND_ERROR_MESSAGE);
    PushToSendingEventBuf(&m_CmdTxBuffer_Async_Temp[0], EventTx_Length); // 发送测试事件
    //EventsBuffering::PushingEvents(&m_CmdTxBuffer_Async_Temp[0],EventTx_Length,++ m_dwSend_SN);// 备份到EventBuff中去,以便5秒后重发。
    //m_dwSend_SN --; // 多包暂不作为重发事件发送。

}
#if  USE_AS_AUXMCU == 0
void EthernetParse::MPReport_SimulationRequestData(MP_REPORT_SIMULATION_REQUESTDATA *pReport)
{
    UWord16 uw16_Count = sizeof(WORD);  //  单个长度:4字节;
    pReport->m_dwCmd = CMD_REPORT_SIMULATION_REQUESTDATA;

    UWord16 uw16_Length = sizeof(MP_REPORT_SIMULATION_REQUESTDATA) - (uw16_Count * MAX_SIMU_REPORT_PACKINDEX);   //  总字节数 = 包头包信息;
    uw16_Length += uw16_Count * pReport->m_wTotalCout;  //  取点总字节数=单点长度*已取的点数;

    EventTx_Length  = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &EventTx_Length, SEND_ERROR_MESSAGE);
    PushToSendingEventBuf(&m_CmdTxBuffer_Async_Temp[0], EventTx_Length); // 发送测试事件
}
#endif

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
////////
// MPReport_GetSCH_Status(St_REPORT_TOPC_ERROR *pReport)
//  : 网络命令第二信道报告SCH status处理函数
// Input: pReport 网络第二信道报告数据结构指针;
//
// Return: N/A
///////


void EthernetParse::MPReport_GetSCH_Status(UWord16 uw16_ChannelNumber, MP_REPORT_GET_SCHSTATUS *pReport)
{
#if  USE_AS_AUXMCU == 0
    unsigned char uc_PoinIdex = 0;
    pReport->m_dwCmd = CMD_REPORT_GET_SCHSTATUS;  // 将大数据体信息转存到通信协议包中，然后打包发送

    if(uw16_ChannelNumber >= 0XFFFF)
    {
        uw16_ChannelNumber = 0;
        //pReport->m_Status_Count = MAXCHANNELNO;
        pReport->m_Status_Count = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
    }
    else
    {
        pReport->m_Status_Count = 1;
        // 			 if(uw16_ChannelNumber >= MAXCHANNELNO)
        // 			     uw16_ChannelNumber %= MAXCHANNELNO; // 通道号最大值保护
        if(uw16_ChannelNumber >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            uw16_ChannelNumber %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; // 通道号最大值保护

    }

    pReport->m_stMPack.m_stMSecondField.m_wChannelNumber = uw16_ChannelNumber; //  多包信息暂为1包上传
    pReport->m_stMPack.m_stMSecondField.m_StMD5Code = ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_ID;
    pReport->m_stMPack.m_stMSecondField.m_wTotalChannelNumber = pReport->m_Status_Count;
    pReport->m_stMPack.m_stMFirstField.m_wMultPackFlag = 0;  //  多包信息暂为1包上传
    pReport->m_stMPack.m_stMFirstField.m_wPackTotal = 1;
    pReport->m_stMPack.m_stMFirstField.m_wPackNo = 0;
    pReport->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;

    do {
        uw16_ChannelNumber = pReport->m_stMPack.m_stMSecondField.m_wChannelNumber + uc_PoinIdex;
        pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Number   = uw16_ChannelNumber;
        MsTime SysTime = CommonFunc::GetSystemTime();
        pReport->m_Ch_Status[uc_PoinIdex].m_System_Time.Us100 =  SysTime.Us100;
        pReport->m_Ch_Status[uc_PoinIdex].m_System_Time.Second =  SysTime.Second;
        double  d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_TestTime,SysTime);
        pReport->m_Test_Time.m_U64Time = d_Value * (10E5f);
        d_Value = ChannelInfo_Manager::Read_DoubleTime(uw16_ChannelNumber,MetaCode_PV_Steptime,SysTime);
        pReport->m_st_StepTime.m_U64Time = d_Value * (10E5f);
        pReport->m_Ch_Status[uc_PoinIdex].m_Cycle_ID = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber, MetaCode_PV_CycleIndex);
        if(pReport->m_Ch_Status[uc_PoinIdex].m_Cycle_ID > 0)
            pReport->m_Ch_Status[uc_PoinIdex].m_Cycle_ID --; // 暂时为上位机显示正常循环次数提供的处理
        pReport->m_Ch_Status[uc_PoinIdex].m_Step_ID =  ChannelInfo_Manager::ChannelSchedule[uw16_ChannelNumber].m_RunningStepIdx;

        pReport->m_Ch_Status[uc_PoinIdex].m_Schedule_Status = ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].ScheduleStatus;
        //pReport->m_Channel_Status = ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].StepStatus;

        UWord16 uw16_CtrlType = ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;
        switch(uw16_CtrlType)  // 通道状态转换处理
        {
        case CT_IVCH_REST:
        case CT_IVCH_SETVARIABLE: //180503 zyx //mits8
        case CT_IVCH_STOP:
            pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_IDLE;
            break;

        case CT_IVCH_CURRENT:
        case CT_IVCH_DIGII:
        case CT_IVCH_CRATE:
        case CT_IVCH_CRAMP:
        case CT_IVCH_CSTAIRCASE:
        case CT_IVCH_POWER:
        case CT_IVCH_LOAD:
        case CT_IVCH_PRAMP:
//         case CT_IVCH_PSTAIRCASE:
        case CT_IVCH_DIGIPOWER:
        case CT_IVCH_DIGILOAD:
        case CT_IVCH_ACS:
        case CT_IVCH_C_CV:
        case CT_IVCH_V_CV:
        case CT_IVCH_CURRENT_TB:	// 2018.09.05 zc
            //float fCurrent = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Current].m_fValue;
            if(ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber] < 0.0f)
                pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
            else
                pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            break;

        case CT_IVCH_VOLTAGE:
        case CT_IVCH_DIGIV:
        case CT_IVCH_VRAMP:
        case CT_IVCH_VSTAIRCASE:
            float fVoltage;
            fVoltage = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
            if(fVoltage > ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber])
                pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
            else
                pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            break;

        case CT_IVCH_CCCV:
        case CT_IVCH_CCCV_WRM:
            if(ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].m_bCVstage)
            {   //Constant Voltage
                float fVoltage = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
                if(fVoltage > ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber])
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
                else
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            }
            else
            {   //Constant Current
                if(ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber] < 0.0f)
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
                else
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            }
            break;
        case CT_IVCH_CCCV_TB:
            if(ChannelInfo_Manager::ChannelInfo[uw16_ChannelNumber].m_bCVstage)
            {   //Constant Voltage
                float fVoltage = ChannelInfo_Manager::MetaVariable2[uw16_ChannelNumber][MetaCode_PV_Voltage].m_fValue;
                if(fVoltage > ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber])
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
                else
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            }
            else
            {   //Constant Current
                if(ScheduleStatusReport::m_StepCtrlValue[uw16_ChannelNumber] < 0.0f)
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_DISCHARGE;
                else
                    pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_CHARGE;
            }
            break;

        case CT_IVCH_SIMU_I:
        case CT_IVCH_SIMU_P:
        case CT_IVCH_SIMU_L:
        case CT_IVCH_SIMU_V:
        case CT_IVCH_SIMU_Battery:  //190218 zyx
            pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_SIMULATION;
            break;

        case CT_IVCH_IR:
        case CT_IVCH_ACIR:
        case SPECIAL_IRTEST:
            pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_IR;
            break;

        default:
            //pReport->m_Channel_Status = CH_STATUE_CHARGE;
            pReport->m_Ch_Status[uc_PoinIdex].m_Channel_Status = CH_STATUE_IDLE;
            break;
        }
        uc_PoinIdex ++;
    } while(uc_PoinIdex < pReport->m_Status_Count);

    UWord16 uw16_Length = sizeof(MP_REPORT_GET_SCHSTATUS) - (sizeof(REPORT_GET_STATUS) * (32 - pReport->m_Status_Count));// 保存准备发送数据包的数据长度;

    pReport->m_stMPack.m_stMFirstField.m_dwTotalLength = uw16_Length + CMD_HOLDHAND_LENGTH;
    pReport->m_stMPack.m_stMFirstField.m_dwPackLength = uw16_Length + CMD_HOLDHAND_LENGTH;  //  多包信息暂为1包上传

    //  DataTx_Length = uw16_Length;
    // 	HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &DataTx_Length, SEND_GET_SCHSTATUS);
    //  PushToSendingDataBuf(& m_CmdTxBuffer_Async_Temp[0], DataTx_Length); // 发送测试数据

    EventTx_Length  = uw16_Length;// 保存准备发送数据包的数据长度;
    HeadFieldSaveBuff(m_CmdTxBuffer_Async_Temp, &EventTx_Length, SEND_GET_SCHSTATUS);
    PushToSendingEventBuf(&m_CmdTxBuffer_Async_Temp[0], EventTx_Length); // 发送测试事件
    //EventsBuffering::PushingEvents(&m_CmdTxBuffer_Async_Temp[0],EventTx_Length,++ m_dwSend_SN);// 备份到EventBuff中去,以便5秒后重发。
    //m_dwSend_SN --; // 多包暂不作为重发事件发送。
#endif
}
#endif

////////
// SyncParse(void) : 网络包头检查
// Input: N/A
// Return: N/A
///////
bool EthernetParse::SyncParse(void)
{
    //  网络包头检查
    unsigned char ch;

    while(TcpRxTx::ENET_BufLen > 0)		//
    {
        TcpRxTx::GetBFromTCP(&ch);
        if(m_SyncByteCnt < CMD_SYNC_LEN - 2)
        {
            if(ch == 0xAA)
                m_SyncByteCnt++;
            else
                m_SyncByteCnt = 0;
        }
        else if(m_SyncByteCnt < CMD_SYNC_LEN + 2 && ch == 0xAA) // 解决多次同上位机连接时，出现错位现象。
        {
            m_SyncByteCnt++;
        }
        else if(ch > 0)
        {
#if defined(CMD_ERROR_PRINT)
            printf("CMD the first byte (Sync=%d)of Length(%X) \n", m_SyncByteCnt, ch);
#endif
            m_SyncByteCnt = 0;
            return false;
        }
        else
        {
            m_CmdRxLen = 0;
            return true;
        }
    }
#if defined(CMD_ERROR_PRINT)
    if(m_SyncByteCnt > 0)
        printf("CMD SYNC Len=%d Lastbyte=%x\n", m_SyncByteCnt, ch);
#endif
    return false;

    // 网络包头检查 8 byte message header of: 0-7byte AAAA5555(32bit) 9999AAAA(32bit)
    // 	uint8 ch;
    // 	//uint8 uc_Count;
    // 	uint32 uw32_Head[CMD_HEADFIELD1_HAND_LENGTH];
    // 	for(ch = 0; ch < CMD_HEADFIELD1_HAND_LENGTH; ch++)
    // 		uw32_Head[ch] = 0;
    //
    // 	if(m_SyncByteCnt < CMD_SYNC_LEN)
    // 	{
    // 	    TcpRxTx::GetBFromTCP(& uw32_Head[0]);// 统一使用小头序。
    // 		TcpRxTx::GetBFromTCP(& uw32_Head[1]);// 统一使用小头序。
    // 		m_SyncByteCnt =  CMD_HEADFIELD1_HAND_LENGTH * 4;
    // 	}
    //
    //     if((uw32_Head[0] == CMD_HEADFIELD1_HAND_FIRST) && (uw32_Head[1] == CMD_HEADFIELD1_HAND_SECOND))
    // 	{
    // 		m_CmdRxLen = 0;
    // 		return true;
    // 	}
    //
    // 	m_SyncByteCnt = 0;
    // 	return false;

}


////////
// SendFeedBack(uint8 AckFlag, uint16 len) : 加入校验发送数据到上位机
// Input: AckFlag :FeedBack  success or fail
//        len:FeedBack data length
// Return: N/A
///////
void EthernetParse::SendFeedBack(uint8 AckFlag, uint16 len)
{
    //  加入校验发送数据到上位机

    if((AckFlag == MPCS_ACK) || (AckFlag == MPCS_NACK))
    {   // 为适应PRO4.0预留代码
        AckFlag = MPCS_ACK;

        m_CmdTxBuffer_SynchroChannel[0] = AckFlag;

        if(len > 0)
        {
            uint8 checksum = 0;
            for(uint16 k = 1; k < len - 1; k++)
            {
                if(len > 20)
                {
                    // 为新PRO7命令处理结构，避免8字节对齐去丢7字节后频繁出现死机问题。20130904 yang
                    m_CmdTxBuffer_SynchroChannel[k] = m_CmdTxBuffer_SynchroChannel[k + 7]; //平移去丢多加的7字节，不能直接将结构指到数组[1]。
                }
                checksum += m_CmdTxBuffer_SynchroChannel[k];
            }
            m_CmdTxBufSynchro[len - 1] = checksum;
            m_CmdTxBufSynchro[len] = 0xFF - checksum;
        }

        TcpRxTx::SendTo(&m_CmdTxBuffer_SynchroChannel[0], len + 1);
    }
}

////////
// HeadFieldSaveBuff(uint16 *CmdTxAsync_Length,  unsigned char uc_SendType)
//  : 网络命令增加包头预存入缓冲区处理函数
// Input: len 网络第二信道报告数据预存入缓冲区发送长度;
//        uc_SendType 网络第二信道报告数据预存入缓冲区发送类型;
// Return: N/A
///////
void EthernetParse::HeadFieldSaveBuff( BYTE *pBuf, uint16 *CmdTxAsync_Length, unsigned char uc_SendType)
{
    //  网络命令增加包头预存入缓冲区处理函数
    UWord32 uw32_checksum;
    UWord16 uw32_SumCount;

    for(unsigned char uc_i = 0; uc_i < 4; uc_i ++)
    {
        switch(uc_i)  //  包前增加16字节[8Byte(AA)+4Byte(Length)+4Byte(Reserved]
        {
        case 0:
            uw32_checksum = CMD_HEADFIELD1_HAND_FIRST;
            break;
        case 1:
            uw32_checksum = CMD_HEADFIELD1_HAND_SECOND;
            break;
        case 2:
            uw32_checksum =  *CmdTxAsync_Length;
            break;
        default:
            uw32_checksum = CMD_EMPTY;
            break;
        }

        memcpy(&pBuf[uc_i * 4], & uw32_checksum, 4 );   //  包前增加16字节[8Byte(AA)+4Byte(Length)+4Byte(Reserved]
    }

    if(*CmdTxAsync_Length > 0)
    {
        if(uc_SendType != SEND_EVENT_SDL)
        {
            AddCheckSum(&pBuf[CMD_HOLDHAND_LENGTH], *CmdTxAsync_Length); //  校验码
        }
    }
    else
    {
        return;
    }

    uw32_SumCount = CMD_HOLDHAND_LENGTH + *CmdTxAsync_Length;
    *CmdTxAsync_Length = (uint16) uw32_SumCount;
}

void EthernetParse::MultPackDataToSinglePack(unsigned char uc_SendType)
{
    MP_REPORT_CANBMS_Data *CANBmsData;
    MP_REPORT_SMB_Data_Float *SMBData;
    MP_REPORT_SMB_Data_String *SMBData_Str;
    UWord32 uw32_Total=0;
    switch(uc_SendType)
    {
    case SEND_IDLE_BMS_DATA:
    case SEND_LOG_BMS_DATA:
#if (USE_AS_AUXMCU == 0)
        CANBmsData = (MP_REPORT_CANBMS_Data*)&m_btPackDataUploadBuffer[16]; //170815 zyx
        CANBmsData->m_stMPack.m_stMFirstField.m_dwTotalLength = CMD_HOLDHAND_LENGTH + sizeof(MP_REPORT_CANBMS_Data) - ((MAX_CAN_BMS_SIGNAL - CANBmsData->m_CANData.m_DataCnt) << 3); // 56+12+8*130=1108
        CANBmsData->m_stMPack.m_stMFirstField.m_wMultPackFlag = 0;//单包
        CANBmsData->m_stMPack.m_stMFirstField.m_dwPackLength = CANBmsData->m_stMPack.m_stMFirstField.m_dwTotalLength;
        CANBmsData->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        CANBmsData->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;      // 首包地址从0开始
        CANBmsData->m_dwCmd = CMD_REPORT_CANBMS_DATA;  //  单个结构多包串联涵接发送。(MP_GET_REPORT_CHANNEL_BOARD 整结构8包11K)
        CANBmsData->m_dwCmd_Extend = 0;
//      st_MultPackManage = CANBmsData->m_stMPack; // 转传管理信息  // （多包信息管理)
        uw32_Total = CANBmsData->m_stMPack.m_stMFirstField.m_dwTotalLength;
#endif
        break;
    case SEND_IDLE_SMB_DATA_FLOAT:
    case SEND_LOG_SMB_DATA_FLOAT:
#if (USE_AS_AUXMCU == 0)
        SMBData = (MP_REPORT_SMB_Data_Float*)&m_btPackDataUploadBuffer[16];//m_btPackDataBuffer[16];
        SMBData->m_stMPack.m_stMFirstField.m_dwTotalLength = CMD_HOLDHAND_LENGTH + sizeof(MP_REPORT_SMB_Data_Float) - (MAX_SMB_BMS_SIGNAL - SMBData->m_SMBData.m_DataCnt) * sizeof(ST_SMB_RptData_Float::SmbFloatMeta); // 56+12+15*65=1108
        SMBData->m_stMPack.m_stMFirstField.m_wMultPackFlag = 0;//单包
        SMBData->m_stMPack.m_stMFirstField.m_dwPackLength = SMBData->m_stMPack.m_stMFirstField.m_dwTotalLength;
        SMBData->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        SMBData->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;      // 首包地址从0开始
        SMBData->m_dwCmd = CMD_REPORT_SMB_DATA;  //  单个结构多包串联涵接发送。(MP_GET_REPORT_CHANNEL_BOARD 整结构8包11K)
        SMBData->m_dwCmd_Extend = 0;
//      st_MultPackManage = CANBmsData->m_stMPack; // 转传管理信息  // （多包信息管理)
        uw32_Total = SMBData->m_stMPack.m_stMFirstField.m_dwTotalLength;
#endif
        break;
    case SEND_IDLE_SMB_DATA_STRING:
    case SEND_LOG_SMB_DATA_STRING:
#if (USE_AS_AUXMCU == 0)
        SMBData_Str = (MP_REPORT_SMB_Data_String*)&m_btPackDataUploadBuffer[16];//m_btPackDataBuffer[16]; //
        SMBData_Str->m_stMPack.m_stMFirstField.m_dwTotalLength = CMD_HOLDHAND_LENGTH+ sizeof(MP_REPORT_SMB_Data_String)- MAX_SMB_SIGNAL_STRING + SMBData_Str->m_SMBData.m_DataLength;   // 56+12+512=600
        SMBData_Str->m_stMPack.m_stMFirstField.m_wMultPackFlag = 0;//单包
        SMBData_Str->m_stMPack.m_stMFirstField.m_dwPackLength = SMBData->m_stMPack.m_stMFirstField.m_dwTotalLength;
        SMBData_Str->m_stMPack.m_stMFirstField.m_wPackNo = 0;
        SMBData_Str->m_stMPack.m_stMFirstField.m_dwPack_BufferArrayAddress = 0;      // 首包地址从0开始
        SMBData_Str->m_dwCmd = CMD_REPORT_SMB_STRING;  //  单个结构多包串联涵接发送。(MP_GET_REPORT_CHANNEL_BOARD 整结构8包11K)
        SMBData_Str->m_dwCmd_Extend = 0;
//      st_MultPackManage = CANBmsData->m_stMPack; // 转传管理信息  // （多包信息管理)
        uw32_Total = SMBData->m_stMPack.m_stMFirstField.m_dwTotalLength;

#endif
        break;
    default:
        return;
    }
    //DataTx_Length = st_MultPackManage.m_stMFirstField.m_dwTotalLength - CMD_HOLDHAND_LENGTH; // 计算多少个包时已加入包头，应去除
    DataTx_Length = uw32_Total - CMD_HOLDHAND_LENGTH; // 计算多少个包时已加入包头，应去除
    HeadFieldSaveBuff(m_btPackDataUploadBuffer, &DataTx_Length, SEND_GET_CHANNELBOARD_MG);    //  20160426  dekui //170815 zyx
//    if (st_MultPackManage.m_stMFirstField.m_dwTotalLength > (MAX_TESTDATA_BUFFER_SIZE - QUEUE_DEPTH(TestDataTxBuffer)))
    if (uw32_Total > (MAX_TESTDATA_BUFFER_SIZE - QUEUE_DEPTH(TestDataTxBuffer)))
    {
        return;
    }
    PushToSendingDataBuf(&m_btPackDataUploadBuffer[0], DataTx_Length); //170815 zyx
}
// There is another function replace it
////////
// IfCheckSumOk(uint16 len)  : 网络包检查校验是否正确处理函数
// Input: len 网络包长度;
// Return: N/A
///////
//bool EthernetParse::IfCheckSumOk(uint16 len)
//{
//// 检查校验是否正确
//	uint32 checksum = 0;
//	uint32 u32_Value = 0;

//	/*uint8 checksum = 0;
//	for(int16 k=1; k<len-1; k++)
//	{
//		checksum += m_CmdRxBuf[k];
//	}
//	if(m_CmdRxBuf[len-1] != checksum)
//	{
//		return false;
//	}
//	if(m_CmdRxBuf[len] != (0xFF - checksum))
//	{
//		return false;
//	}
//	return true;*/

//
//	for(int16 k = 1; k < len - 4; k++)
//		checksum += m_CmdRxBuf[k];

//    u32_Value |= (m_CmdRxBuf[len-3]  << 24);  // 统一使用小头序，先传低位数据（小地址）再传高位数据（大地址）。
//	u32_Value |= (m_CmdRxBuf[len-2]  << 16);
//	u32_Value |= (m_CmdRxBuf[len-1]  << 8);
//	u32_Value |= m_CmdRxBuf[len];
//
//	//if(u32_Value != checksum)
//	//	return false;
//	//else
//	    return true;
//}

uint8  EthernetParse::AddCheckSum( BYTE *pbtBuf, UINT uSize )
{
    //ASSERT( pbtBuf );

    if( uSize <= 2 )
    {
        return false;
    }

    //UINT uRealSize = uSize-2;
    UINT uRealSize = uSize;  //  2013.08.21 yang
    UWord16 uw16_poit = 2 * sizeof(DWORD); // 执行命令码字+扩展命令码字= 8字节
    pbtBuf[uw16_poit] = 0;
    pbtBuf[uw16_poit + 1] = 0;

    BYTE btSum = 0;
    for( UINT i = 0; i < uRealSize; i++ )
    {
        btSum += pbtBuf[i];
    }


    //pbtBuf[uRealSize] = btSum;
    //pbtBuf[uRealSize+1] = (BYTE)~btSum;

    pbtBuf[uw16_poit] = btSum;
    pbtBuf[uw16_poit + 1] = (BYTE)~btSum;

    return true;
}

uint8  EthernetParse::IsCheckSumOK( BYTE *pbtBuf, UINT uSize )
{
    //ASSERT( pbtBuf );

    if( uSize <= 2  )
    {
        return false;
    }

    UINT uRealSize = uSize - 2;
    BYTE btSum = 0;

    for( UINT i = 0; i < uRealSize; i++ )
    {
        btSum += pbtBuf[i];
    }

    return ((pbtBuf[uRealSize] == btSum)
            && (pbtBuf[uRealSize + 1] == (BYTE)~btSum));

}



////////
// CheckIfResetSystem(void) : 检查通信，通信失败复位系统函数
// Input:  N/A
// Return: N/A
///////
// void EthernetParse::CheckIfResetSystem(void)
// {
// 	// 检查通信，通信失败复位系统

// }

/******************************************************************************
** Function name:        MP_GetWord_RxBuf
**
** Descriptions:         得到命令缓冲区字内容
**
** parameters:           uc_start
**
** Returned value:       字
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
// UWord16 EthernetParse::MP_GetWord_RxBuf(BYTE * uc_start)
// {
// 	// 得到命令缓冲区字内容

// 	CommonFunc::uc_F2UC[0] = * (uc_start);
// 	CommonFunc::uc_F2UC[1] = * (uc_start + 1);
// 	return CommonFunc::UC_2_Word();
// }

/******************************************************************************
** Function name:        MP_GetDWord_RxBuf
**
** Descriptions:         得到命令缓冲区双字内容
**
** parameters:           uc_start
**
** Returned value:       双字
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
// UWord32 EthernetParse::MP_GetDWord_RxBuf(BYTE * uc_start)
// {
// 	// 得到命令缓冲区双字内容
// 	CommonFunc::uc_F2UC[0] = * (uc_start);
// 	CommonFunc::uc_F2UC[1] = * (uc_start + 1);
// 	CommonFunc::uc_F2UC[2] = * (uc_start + 2);
// 	CommonFunc::uc_F2UC[3] = * (uc_start + 3);
// 	return CommonFunc::UC_2_DWord();
// }

/******************************************************************************
** Function name:        IVCH_GetFloat_RxBuf
**
** Descriptions:         得到命令缓冲区浮点内容
**
** parameters:           uc_start
**
** Returned value:       浮点数据
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
// float EthernetParse::MP_GetFloat_RxBuf(BYTE * uc_start)
// {
// 	// 从通信接收缓冲区取出数据组合成浮点数
// 	CommonFunc::uc_F2UC[0] = * (uc_start);
// 	CommonFunc::uc_F2UC[1] = * (uc_start + 1);
// 	CommonFunc::uc_F2UC[2] = * (uc_start + 2);
// 	CommonFunc::uc_F2UC[3] = * (uc_start + 3);
// 	return CommonFunc::UC_2_Float();
// }
/*
////////
// SwapByteInDWord(uint32 & rUint32) : 对整型双字进行先单字交换，然后字内字节交换。
// Input:  rUint32 整型双字
// Return: N/A
///////
void EthernetParse::SwapByteInDWord(uint32 & rUint32)
{
	// 对整型双字进行先单字交换，然后字内字节交换。
	uint16* p1 = (uint16*)(&rUint32);
	uint16* p2 = p1 + 1;

	uint16 temp = *p1;
	*p1 = *p2;
	*p2 = temp;
	SwapByteInWord(*p1);
	SwapByteInWord(*p2);
}*/
/*
////////
// SwapByteInWord(uint16 & rUint16) : 对整型单字进行字内字节交换
// Input: Input:  rUint16 整型单字
// Return: N/A
///////
void EthernetParse::SwapByteInWord(uint16 & rUint16)
{
	// 对整型单字进行字内字节交换。
	uint8* p1 = (uint8*)(&rUint16);
	uint8* p2 = p1 + 1;

	uint8 temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}*/

////////
// ByteSwapFloat(float &inValue) : 对浮点数先变换成整型双字，然后进行单字交换，最后字内字节交换。
// Input: inValue 浮点数;
// Return: N/A
///////
// float EthernetParse::ByteSwapFloat(float &inValue)
// {
// 	// 对浮点数先变换成整型双字，然后进行单字交换，最后字内字节交换。
// 	union{
// 		float f;
// 		uint32 un;
// 	}u;
// 	u.f = inValue;
// 	SwapByteInDWord(u.un);
// 	return u.f;
// }

////////
// TotalNumberCountProcess : 对通道号和总通道数进行保护。
// Input: Chan 通道号，Total 总通道数，Flag 多个通道标志;
// Return: N/A
///////
void EthernetParse::TotalNumberCountProcess(UWord16 * Chan, UWord16 * Total, UChar8 * Flag)
{
    UWord16 uw16_ChanIdx = * Chan;
    UWord16 uw16_ChanTotal = * Total;
    UWord16 uw16_MaxChannelNum = MAXCHANNELNO;
    UChar8  uc_UsedForAll = 0;

#if  USE_AS_AUXMCU == 1
    uw16_MaxChannelNum = MAX_AUXCH_PER_AUXMCU;
#else
    uw16_MaxChannelNum = MAXCHANNELNO;
#endif

    if((uw16_ChanIdx == 0xFFFF) || (uw16_ChanTotal > 1))
        uc_UsedForAll = 1; // 多通道处理标志
    else
        uc_UsedForAll = 0;  // 仅单通道处理标志

    if(uw16_ChanIdx == 0xFFFF)
        uw16_ChanIdx = 0;

    if(uw16_ChanIdx >= uw16_MaxChannelNum)
        uw16_ChanIdx %= uw16_MaxChannelNum;// 通道号最大值保护

    if(uw16_ChanTotal <= 1)
        uw16_ChanTotal = 1;

    uw16_ChanTotal = uw16_ChanIdx + uw16_ChanTotal;// 取总通道数运算，保护最大值

    if(uw16_ChanTotal >= uw16_MaxChannelNum)
        uw16_ChanTotal = uw16_MaxChannelNum;// 保护最大值

    * Chan = uw16_ChanIdx;
    * Total = uw16_ChanTotal;
    * Flag = uc_UsedForAll;
}

void EthernetParse::CmdSetToFeedback(DWORD * Cmd, DWORD * Cmd_Extend, unsigned char Success)
{
    m_ResultFeedback.m_dwCopyCmd = * Cmd;   // 备份准备回传信息
    m_ResultFeedback.m_dwCopyCmd_Extend = * Cmd_Extend;
    m_ResultFeedback.m_btError_Code = MP_CONFIRM_ERROR_UNKNOWN;  // 第三层信息  配置错误代码

    if(Success == CONFIRM_RESULT_SUCCESS)
    {
        m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_SUCCESS;  // 第二层多包
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_SUCCESS;        // 第一层单包
    }
    else
    {
        m_ResultFeedback.m_btMultPack_OK = CONFIRM_RESULT_FAIL;  // 第二层多包
        m_ResultFeedback.m_btResult = CONFIRM_RESULT_FAIL;        // 第一层单包
    }
}

void EthernetParse::CmdGetToFeedback(DWORD * Cmd, DWORD * Cmd_Extend, WORD * ChNumber, WORD * TotalNumber)
{
    * Cmd = m_ResultFeedback.m_dwCopyCmd;   // 传递备份信息
    * Cmd_Extend = m_ResultFeedback.m_dwCopyCmd_Extend;
    * ChNumber = m_ResultFeedback.m_stMSecondField.m_wChannelNumber;
    * TotalNumber =  m_ResultFeedback.m_stMSecondField.m_wTotalChannelNumber;

}

////////
// SetRemoteIP(unsigned char ServerIP) : 网络设置远程IP处理函数
// Input: ServerIP 远程计算机IP;
// Return: N/A
///////
void EthernetParse::SetRemoteIP(unsigned char ServerIP)
{
    *(unsigned char *)uw16_RemoteIP = 196; //EEP_RemoteIP[0];               // uncomment those lines to get the  远程计算机IP
    *((unsigned char *)uw16_RemoteIP + 1) = 168; //EEP_RemoteIP[1];          // quote of the day from a real
    *((unsigned char *)uw16_RemoteIP + 2) = 1;//EEP_RemoteIP[2];         // internet server! (gateway must be
    *((unsigned char *)uw16_RemoteIP + 3) = ServerIP; //EEP_RemoteIP[3]  ;  //208 203; //       // set to your LAN-router)

    uc_remotemIpAdr[0] = *(unsigned char *)  uw16_RemoteIP;    // uncomment those lines to get the  远程计算机IP
    uc_remotemIpAdr[1] = *((unsigned char *) uw16_RemoteIP + 1);    // quote of the day from a real
    uc_remotemIpAdr[2] = *((unsigned char *) uw16_RemoteIP + 2);    // internet server! (gateway must be
    uc_remotemIpAdr[3] = *((unsigned char *) uw16_RemoteIP + 3);
}

////////
// SetLocalIP(unsigned char LastDigit) : 网络设置本机本地IP处理函数
// Input: LastDigit 本机本地IP;
// Return: N/A
///////
void EthernetParse::SetLocalIP5MAC(unsigned char LastDigit)
{
    *(unsigned char *)uw16_MyIP = 196; // EEP_RemoteIP[0];//192;                    // "MYIP1.MYIP2.MYIP3.MYIP4"    本机IP
    *((unsigned char *)uw16_MyIP + 1) = 168; //EEP_RemoteIP[1];//168;
    *((unsigned char *)uw16_MyIP + 2) = 1; //EEP_RemoteIP[2];// 1;
    *((unsigned char *)uw16_MyIP + 3) = LastDigit;
// 	LastDigitOfIPMAC = LastDigit;

    nlocalm[0].IpAdr[0] = *(unsigned char *)  uw16_MyIP;
    nlocalm[0].IpAdr[1] = *((unsigned char *) uw16_MyIP + 1);
    nlocalm[0].IpAdr[2] = *((unsigned char *) uw16_MyIP + 2);
    nlocalm[0].IpAdr[3] = *((unsigned char *) uw16_MyIP + 3);

    *((unsigned char *)uc_MyMAC  + 0) = own_hw_adr[0]; // 0X00; // _MAC1;
    *((unsigned char *)uc_MyMAC  + 1) = own_hw_adr[1]; //0X00; // _MAC2;
    *((unsigned char *)uc_MyMAC  + 2) = own_hw_adr[2]; //0X00; // _MAC3;
    *((unsigned char *)uc_MyMAC  + 3) = own_hw_adr[3]; //0X00; // _MAC4;
    *((unsigned char *)uc_MyMAC  + 4) = own_hw_adr[4]; //0X00; // _MAC5;
    *((unsigned char *)uc_MyMAC  + 5) = LastDigit;
    own_hw_adr[5]  = LastDigit;
// 	LastDigitOfIPMAC = LastDigit;

    UWord32 uw32_i = LastDigit; // Control_Channel::PI_BoardIDRead() + 1;
    lhost_name[9]  = 48 + uw32_i / 100;   // 48=字符'0'   最大128=字符'�'
    lhost_name[10] = 48 + uw32_i / 10;
    lhost_name[11] = 48 + uw32_i;
    lhost_name[12] = '\0';
}

inline BYTE EthernetParse::Char2Num( BYTE Byte ) //zyx 180305
{
    return (Byte - '0');
}
void EthernetParse::VersionProcess(UNIT_VER_INFO    *st_Ver) //1610142 zyx
{
    BYTE *version = OS_ServeProcess::Version;
    st_Ver->m_btYear = Char2Num(version[2]) * 10 + Char2Num(version[3]);
    st_Ver->m_btMonth = ( Char2Num(version[4]) * 10 + Char2Num(version[5]) ) % 13;
    st_Ver->m_btDay = ( Char2Num(version[6]) * 10 + Char2Num(version[7]) ) % 32;
    st_Ver->m_btBuildNumber = Char2Num(version[8]) % 10;
    switch(version[0])
    {
    case 'Z':
        st_Ver->m_btVersionType = ZY;
        break;
    case 'R':
        st_Ver->m_btVersionType = RD;
        break;
    case 'T':
        st_Ver->m_btVersionType = TY;
        break;
    case 'P':
        st_Ver->m_btVersionType = PV;
        break;
    default:
        st_Ver->m_btVersionType = ERROR_VERSION;
        break;
    }
#if  USE_AS_AUXMCU == 0
    st_Ver->m_wCircuitType = ChannelInfo_Manager::m_st_BoadType.m_u16BoardType;
#endif
}

#if  USE_AS_AUXMCU == 0
void EthernetParse::SendTasksMsgProcess(UWord16 Chan, UChar8 SourceType, unsigned char UnsafeType, float Value)
{
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
    SchMessage.st_status_sch_running.m_btCMD_Source =  SourceType;
    //SchMessage.st_status_sch_running.m_uw32Safe_Code = VOLTAGE_SCH_UNSAFE;
    switch(UnsafeType)  // 20160708 zyx
    {
    case VClampUnsafe:
        SchMessage.st_status_sch_running.m_bUnsafeType  = IV_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = VCLAMP_SAFETY;  // 20160414yy
        safety_check::SetUnsafe(Chan,VCLAMP_SAFETY, Value,IV_UNSAFE_TYPE);
        break;
    case TemperatureChamberUnsafe:  //企图共用温控箱，温控箱只能由一个通道独自控制，否则会混乱
        SchMessage.st_status_sch_running.m_bUnsafeType  = AUX_UNSAFE_TYPE;
        //        SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_UNSAFE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = MTCI_RUN_FAIL;
        safety_check::SetUnsafe(Chan,AUX_UNSAFE,Value, AUX_UNSAFE_TYPE);
        SchMessage.st_status_sch_running.m_btSubCode = AUX_TEMPERATURE;
        break;
    }
    SchMessage.st_status_sch_running.m_btStopped_Status = STOP_UNSAFE;
    SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = Value;
//     SchMessage.st_status_sch_running.m_btLost_Unit = 0;

    switch(SourceType)
    {
    case ST_STEPMANAGE:
        return;
    /*SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_STEPMANAGE;
    ChannelInfo_Manager::ChannelInfo[Chan].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
    Schedule_Manager_task::UnsafeContent[Chan]=SchMessage;
    Schedule_Manager_task::SendMsgToSchedule(Chan,SchMessage);    // 向SCH发送
    break;*/
    case ST_ETHERNETPARSE:
        memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
        m_ReportStatus.m_btCMD = PC_Unsafe_Msg;
        SchMessage.st_status_sch_running.m_btCMD_Source =  ST_ETHERNETPARSE;
        ChannelInfo_Manager::ChannelInfo[Chan].m_Event_Source = S_GOSTOP_STOPPEDMSG;
        m_ReportStatus.m_st_RunningStatus = SchMessage.st_status_sch_running;
        //m_ReportStatus.m_stMSecondField.m_wChannelNumber=Chan;
        MPReportData(Chan, SEND_EVENT_CONTENT);       // 向PC发送
        Schedule_Manager_task::StartScheduleUnsafe[Chan].st_status_sch_running = SchMessage.st_status_sch_running;
        break;
    default:
        break;
    }
}
#endif

//----------------------------------------------------------------------------------
void EthernetParse::PrepareTxRawDataForSending(void)
{
    bool LastTryOK = true;
    uint32 u32_DataLength = LastTryOK; //170505 zyx uint16 -> uint32

    if(!tcp_check_send(TcpRxTx::socket_tcp2))
    {
        if((CommonFunc::TimeDiffInSecond(AbortTime) >= 2) && TcpRxTx::abort_tcp2)
        {
            if(Pack_1500_depth_back != 0)
            {
                uc_SendFail = 1;  // 插拔网线
                uw16_SendCounter = 0;
            }
            TcpRxTx::abort_tcp2 = 0;
        }
        return;
    }

    if(uc_SendFail)
    {
        if(++uw16_SendCounter >= 50)  // 再延时，让插拔网线稳定
        {
            if(TcpRxTx::SendToReport(Pack_Tx_Buff_1500, Pack_1500_depth_back) > 2)	//将临时缓冲内数据发送走
            {
                uc_SendFail = 0;
                uw16_SendCounter = 0;
                Pack_1500_depth_back = 0;
            }
        }
        return;
    }
    //发送之前的预处理
// 	if(IS_QUEUE_EMPTY(TestEventTxBuffer))  //事件队列空
// 	{
// 		// 不做事，下去判断数据队列是否空
// 	}
// 	else  //事件队列不空
    if(!IS_QUEUE_EMPTY(TestEventTxBuffer))  //事件队列
    {
        u32_DataLength = QUEUE_DEPTH(TestEventTxBuffer);
        if((CommonFunc::GetTimeTicks( &EventTxCounter) >= 50) || (u32_DataLength >= MAX_PACKLIMIT_SIZE)) // 5ms延时到了
        {
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
            ScheduleStatusReport::uc_uc_BreakNet_Counter ++;  // SCH 状态定阅报告 断网计算器计数。
#endif

            //while(!(IS_QUEUE_EMPTY(TestEventTxBuffer)))
            if(tcp_check_send(TcpRxTx::socket_tcp2))
            {
                int16 u16ByteFilled;
                u16ByteFilled = Fill_Pack_Tx_Buf_1500(); //填充临时缓冲区
                if(TcpRxTx::SendToReport(Pack_Tx_Buff_1500, u16ByteFilled) < 2)	//将临时缓冲内数据发送走
                {
                    LastTryOK = false;
                }
                else
                {
                    Pack_1500_depth_back = Pack_Tx_Buff_1500_used_depth;
                    AbortTime = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
                    Pack_Tx_Buff_1500_used_depth = 0; //临时缓冲清空

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
                    ScheduleStatusReport::uc_uc_BreakNet_Counter = 0;  // SCH 状态定阅报告 清空断网计算器。
#endif
                }
            }

#if DEBUG_TEST_COMMDATA_MODE == 1
            else
                EthernetParse::uw32_TestBlock1++; //  仅为测试用，成型后要清理丢。
#endif
            CommonFunc::GetTimeTicksUpdate(&EventTxCounter);  // 何时复位5ms计数器
            return;  // 结束 （不再判断数据队列是否空）此处如不合适用return，则使用其它方式跳过下面的代码。
        }
// 		else  // 5ms延时未到
// 		{
// 			// 不做事，下去判断数据队列是否空
// 		}
    }

// 	if(IS_QUEUE_EMPTY(TestDataTxBuffer))  //数据队列空
// 	{
// 		// 不做事
// 	}
// 	else //数据队列不空
    if(!IS_QUEUE_EMPTY(TestDataTxBuffer))  //数据队列
    {
        u32_DataLength = QUEUE_DEPTH(TestDataTxBuffer);
// 		if((u32_DataLength > MAX_PACKLIMIT_SIZE) // 如果数据队列大于1.5k
// 			||(CommonFunc::GetTimeTicks( &DataTxCounter) >= 10000)) //或者1s延时到了 10000 170505 zyx
#if  USE_AS_AUXMCU == 0
        //if((u32_DataLength > MAX_PACKLIMIT_SIZE)||(CommonFunc::GetTimeTicks( &DataTxCounter) >= 500)) //如果数据队列大于1.5k或者1s延时到了 10000 170505 zyx //1808131 hpq
        //if(u32_DataLength > 0)
        // 20220421 yang
//		if((u32_DataLength > MAX_PACKLIMIT_SIZE)||(CommonFunc::GetTimeTicks( &DataTxCounter) >= EthernetParse::uw64_CTI_SetTimeInterval))//500)) //如果数据队列大于1.5k或者1s延时到了 10000 170505 zyx //1808131 hpq
        unsigned char uc_SendPack = 0;
        if(EthernetParse::uw64_CTI_SetTimeInterval > 500)
        {
            if((u32_DataLength > MAX_PACKLIMIT_SIZE)||(CommonFunc::GetTimeTicks( &DataTxCounter) >= 500)) //如果数据队列大于1.5k或者1s延时到了 10000 170505 zyx //1808131 hpq
            {
                uc_SendPack = 1;
            }
        }
        else
            uc_SendPack = 1;

        if(uc_SendPack)
#else
        if(u32_DataLength > 0)
#endif
        {
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
            ScheduleStatusReport::uc_uc_BreakNet_Counter ++;  // SCH 状态定阅报告 断网计算器计数。
#endif

            if(tcp_check_send(TcpRxTx::socket_tcp2))
            {
                uint16 u16ByteFilled = Fill_Pack_Tx_Buf_1500(); //填充临时缓冲区
                if(TcpRxTx::SendToReport(Pack_Tx_Buff_1500, u16ByteFilled) < 2)	//将临时缓冲内数据发送走
                {
                    LastTryOK = false;
                }
                else
                {
                    Pack_1500_depth_back = Pack_Tx_Buff_1500_used_depth;
                    AbortTime = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;

                    Pack_Tx_Buff_1500_used_depth = 0; //临时缓冲清空

#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
                    ScheduleStatusReport::uc_uc_BreakNet_Counter = 0;  // SCH 状态定阅报告 清空断网计算器。
#endif

                }

                CommonFunc::GetTimeTicksUpdate(&DataTxCounter); // 复位1s计数器
                //CommonFunc::GetTimeTicksUpdate(&EventTxCounter);  // 是否同时复位5ms计数器
            }
#if DEBUG_TEST_COMMDATA_MODE == 1
            else
                EthernetParse::uw32_TestBlock1++; //  仅为测试用，成型后要清理丢。
#endif
        }
// 		else // 1s延时未到
// 		{
// 			// 不做事
// 		}
    }
}

void  EthernetParse::PushToSendingEventBuf(uint8 * pByteData, uint16 EVENT_LENGTH_IN_BYTE)
{
    //当有测试事件需要发送的时候
    if (IS_QUEUE_FULL(TestEventTxBuffer, MAX_EVENT_BUFFER_SIZE))
    {
        //满了，失败，
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
        // SCH 状态定阅报告 导致无空余缓冲空间报，触发Unsafe事件，并停止实验。
//         m_bP1Broken = 1;
        m_bResetSchedule = eEMERGENCYSTOP_ERROR_SendEventBufFULL;
        m_bResetThirdParty = 1;

        for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; uc_i ++)
        {
            ScheduleStatusReport::Set_ChIntervalTime(uc_i, 0);
        }
#endif

        return;
    }
    if (EVENT_LENGTH_IN_BYTE > (MAX_EVENT_BUFFER_SIZE - QUEUE_DEPTH(TestEventTxBuffer)))
    {
        //虽然没满，但是剩余空间也不够填写了，失败
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
        // SCH 状态定阅报告 导致无空余缓冲空间报，触发Unsafe事件，并停止实验。
//         m_bP1Broken = 1;
        m_bResetSchedule = eEMERGENCYSTOP_ERROR_SendEventBufNotEnough;
        m_bResetThirdParty = 1;

        for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; uc_i ++)
        {
            ScheduleStatusReport::Set_ChIntervalTime(uc_i, 0);
        }
#endif

        return;
    }

// 	ENQUEUEARRAY(TestEventTxBuffer, MAX_EVENT_BUFFER_SIZE, pByteData, EVENT_LENGTH);

    Temp_Q.BufferDepth = MAX_EVENT_BUFFER_SIZE;
    Temp_Q.Start = &TestEventTxBuffer.m_u32Start;
    Temp_Q.Length = &TestEventTxBuffer.m_u32Length;
    Temp_Q.Object_0 = (void *)&TestEventTxBuffer.Object[0];
    CQ::BulkENQUEUE(Temp_Q, (void *)pByteData, EVENT_LENGTH_IN_BYTE);
}

void  EthernetParse::PushToSendingDataBuf(uint8 * pByteData, uint16 DATA_LENGTH)
{
    // 当有测试数据需要发送的时候
    if (IS_QUEUE_FULL(TestDataTxBuffer, MAX_TESTDATA_BUFFER_SIZE))
    {
        //满了，失败，
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
        // SCH 状态定阅报告 导致无空余缓冲空间报，触发Unsafe事件，并停止实验。
//         m_bP1Broken = 1;
        m_bResetSchedule = eEMERGENCYSTOP_ERROR_SendDataBufFULL;
        m_bResetThirdParty = 1;

        for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; uc_i ++)
        {
            ScheduleStatusReport::Set_ChIntervalTime(uc_i, 0);
        }
#endif

        return;
    }
    if (DATA_LENGTH > (MAX_TESTDATA_BUFFER_SIZE - QUEUE_DEPTH(TestDataTxBuffer)))
    {
        //虽然没满，但是剩余空间也不够填写了，失败
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
        // SCH 状态定阅报告 导致无空余缓冲空间报，触发Unsafe事件，并停止实验。
//         m_bP1Broken = 1;
        m_bResetSchedule = eEMERGENCYSTOP_ERROR_SendDataBufNotEnough;
        m_bResetThirdParty = 1;

        for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; uc_i ++)
        {
            ScheduleStatusReport::Set_ChIntervalTime(uc_i, 0);
        }
#endif

        return;
    }
// 	if (QUEUE_DEPTH(TestDataTxBuffer) > (MAX_TESTDATA_BUFFER_SIZE * 0.8))
// 	{
// 		//快满了，需要瘦身
// 		;
// 	}

    //if (IS_QUEUE_FULL(TestDataTxIndexQueue, MAX_BUFFERTXINDEX_SIZE))
    if (IS_QUEUE_FULL(TestDataTxPackEndQueue, MAX_PACK_NUMBER))
    {
        return;
    }

// 	ENQUEUEARRAY(TestDataTxBuffer, MAX_TESTDATA_BUFFER_SIZE, pByteData, DATA_LENGTH);

    uint32 packed_length = 0;
    if(!IS_QUEUE_EMPTY(TestDataTxPackEndQueue))
    {   // 数据发送管理，值为数据字节开始地址+数据字节长度
        if(TestDataTxPackEndQueue.Object[(TestDataTxPackEndQueue.m_u32Start + TestDataTxPackEndQueue.m_u32Length - 1) % MAX_PACK_NUMBER] > TestDataTxBuffer.m_u32Start)
            packed_length = TestDataTxPackEndQueue.Object[(TestDataTxPackEndQueue.m_u32Start + TestDataTxPackEndQueue.m_u32Length - 1) % MAX_PACK_NUMBER] - TestDataTxBuffer.m_u32Start + 1;
        else
            packed_length = (MAX_TESTDATA_BUFFER_SIZE - TestDataTxBuffer.m_u32Start) + (TestDataTxPackEndQueue.Object[(TestDataTxPackEndQueue.m_u32Start + TestDataTxPackEndQueue.m_u32Length - 1) % MAX_PACK_NUMBER] + 1); // 最尾反转地址变小，加减1指向实际存数位置
    }

    if( (TestDataTxBuffer.m_u32Length - packed_length + DATA_LENGTH) >= MAX_PACKLIMIT_SIZE)  // 保持新包重新开始装数直到装满
        ENQUEUE(TestDataTxPackEndQueue, MAX_PACK_NUMBER, (TestDataTxBuffer.m_u32Start + TestDataTxBuffer.m_u32Length - 1) % MAX_TESTDATA_BUFFER_SIZE);

    Temp_Q.BufferDepth = MAX_TESTDATA_BUFFER_SIZE;
    Temp_Q.Start = &TestDataTxBuffer.m_u32Start;
    Temp_Q.Length = &TestDataTxBuffer.m_u32Length;
    Temp_Q.Object_0 = (void *)&TestDataTxBuffer.Object[0];
    CQ::BulkENQUEUE(Temp_Q, (void *)pByteData, DATA_LENGTH);

    //ENQUEUE(TestDataTxIndexQueue, MAX_BUFFERTXINDEX_SIZE, (TestDataTxBuffer.m_u32Start + TestDataTxBuffer.m_u32Length-1)%MAX_TESTDATA_BUFFER_SIZE); // Save endpoint of the packet into index que  add DKQ 08.09.2013
}

uint16 EthernetParse::MoveEventBuftoTxBuf1500(void)
{
    if(IS_QUEUE_EMPTY(TestEventTxBuffer))
        return 0;
    uint32 u32_length = MAX_PACKLIMIT_SIZE - Pack_Tx_Buff_1500_used_depth;
    uint32 u32BytesToBeMoved = QUEUE_DEPTH(TestEventTxBuffer);

    if(u32BytesToBeMoved <= 0)
        return 0;

    /*if(u16BytesToBeMoved > u16_length)
    {
    uint16 u16_point = TestEventTxBuffer.m_u32Start;
    uint16 u16_count = 0;
    for(int16 w_i = u16_length; w_i >= 0; w_i --) // 从后向前扫，避免一个结构被分成两包发送。
    {
    if(TestEventTxBuffer.Object[u16_point + w_i] == 0XAA)
    {
    if((++u16_count) == 8)  // 查找8个0XAA的包头。
    {
    uint32 uw32_l = TestEventTxBuffer.Object[u16_point + w_i +8] + (TestEventTxBuffer.Object[u16_point + w_i +9] << 8)
    +(TestEventTxBuffer.Object[u16_point + w_i +10] << 16)+(TestEventTxBuffer.Object[u16_point + w_i +11] << 24);
    if(uw32_l > (u16_length - w_i))
    {
    u16_length = w_i;  // 缩短长度。
    break;
    }
    }

    }
    else
    {
    u16_count = 0;
    }
    }
    }*/

    u32BytesToBeMoved = (u32BytesToBeMoved > u32_length) ? u32_length : u32BytesToBeMoved; // 事件同一个命令结构可分两包分送。

// 	for(int i=0; i < u16BytesToBeMoved; i++)
// 	{
// 		uint8 bDQOK=false;
// 		DEQUEUE(TestEventTxBuffer,
// 			MAX_EVENT_BUFFER_SIZE,
// 			Pack_Tx_Buff_1500[Pack_Tx_Buff_1500_used_depth],
// 			bDQOK);
// 		Pack_Tx_Buff_1500_used_depth++;
// 	}

    Temp_Q.BufferDepth = MAX_EVENT_BUFFER_SIZE;
    Temp_Q.Start = &TestEventTxBuffer.m_u32Start;
    Temp_Q.Length = &TestEventTxBuffer.m_u32Length;
    Temp_Q.Object_0 = (void *)&TestEventTxBuffer.Object[0];
    CQ::BulkDEQUEUE(Temp_Q, (void * )&Pack_Tx_Buff_1500[Pack_Tx_Buff_1500_used_depth], u32BytesToBeMoved);
    Pack_Tx_Buff_1500_used_depth += u32BytesToBeMoved;

    return u32BytesToBeMoved;
}

uint16 EthernetParse::MoveDataBuftoTxBuf1500(void)
{
    if(IS_QUEUE_EMPTY(TestDataTxBuffer))
        return 0;

    uint8 bPKOK = false;
    uint32 u32BytesToBeMoved = TestDataTxBuffer.m_u32Length;
    uint32 end_address = 0;
    if(!IS_QUEUE_EMPTY(TestDataTxPackEndQueue))
    {
        DEQUEUE(TestDataTxPackEndQueue, MAX_PACK_NUMBER, end_address, bPKOK);   // 数据发送管理，值为数据字节开始地址+数据字节长度
        if(end_address > TestDataTxBuffer.m_u32Start)
            u32BytesToBeMoved = end_address - TestDataTxBuffer.m_u32Start + 1	;
        else
            u32BytesToBeMoved = (MAX_TESTDATA_BUFFER_SIZE - TestDataTxBuffer.m_u32Start) + (end_address + 1);   // 最尾反转地址变小，加减1指向实际存数位置
    }


//	uint32 u32NextPackEndPoint = 0;
//	uint32 u32NextPackLength = 0;


//	uint16 u16LeftSpace = MAX_PACKLIMIT_SIZE - Pack_Tx_Buff_1500_used_depth;
//	uint32 u32NextPackStartPoint = TestDataTxBuffer.m_u32Start;    //Add DKQ on 08.09.2013


//	while(!(IS_QUEUE_EMPTY(TestDataTxPackEndQueue)))
//	{
//		// 观察查看数据命令结构个数管理情况。
//		PEEKQUEUE(TestDataTxPackEndQueue, MAX_BUFFERTXINDEX_SIZE, u32NextPackEndPoint, bPKOK);
//		if(bPKOK)
//		{
//			if(u32NextPackEndPoint < u32NextPackStartPoint)
//				u32NextPackLength = u32NextPackEndPoint + MAX_TESTDATA_BUFFER_SIZE - u32NextPackStartPoint+1;   // 循环队列第一圈已满处理。
//			else
//				u32NextPackLength = u32NextPackEndPoint - u32NextPackStartPoint + 1;  // 循环队列第一圈未满处理。

//			if(u32NextPackLength <  u16LeftSpace)
//			{   // 出队数据命令结构个数管理成员。
//				//DEQUEUE(TestDataTxIndexQueue, MAX_BUFFERTXINDEX_SIZE, u32NextPackEndPoint, bPKOK);
//				u16BytesToBeMoved += u32NextPackLength; // 发送空间充足拿多次直到放不下为止。
//				u16LeftSpace -= u32NextPackLength;
//				u32NextPackStartPoint = u32NextPackEndPoint+1;
//			}
//			else
//			{
//				break;  // 发送空间不足，不分包。
//			}
//		}
//		else
//		{
//			break;  // 队列空。
//		}
//	}

    if((u32BytesToBeMoved <= 0) || (u32BytesToBeMoved > MAX_PACKLIMIT_SIZE))
        return 0;

// 	for(int i=0;i<u16BytesToBeMoved;i++)
// 	{
// 		uint8 bDQOK=false;  // 填充数据到发送空间。
// 		DEQUEUE(TestDataTxBuffer,
// 			MAX_TESTDATA_BUFFER_SIZE,
// 			Pack_Tx_Buff_1500[Pack_Tx_Buff_1500_used_depth],
// 			bDQOK);
// 		Pack_Tx_Buff_1500_used_depth++;
// 	}

    Temp_Q.BufferDepth = MAX_TESTDATA_BUFFER_SIZE;
    Temp_Q.Start = &TestDataTxBuffer.m_u32Start;
    Temp_Q.Length = &TestDataTxBuffer.m_u32Length;
    Temp_Q.Object_0 = (void *)&TestDataTxBuffer.Object[0];
    CQ::BulkDEQUEUE(Temp_Q, (void * )&Pack_Tx_Buff_1500[Pack_Tx_Buff_1500_used_depth], u32BytesToBeMoved);
    Pack_Tx_Buff_1500_used_depth += u32BytesToBeMoved;
#if DEBUG_TEST_COMMDATA_MODE == 1
    uw32_TestDataPack++;  // 已发数据包的包号 //  仅为测试用，成型后要清理丢。
#endif

    return u32BytesToBeMoved;
}

uint16 EthernetParse::Fill_Pack_Tx_Buf_1500(void)
{
    Pack_Tx_Buff_1500_used_depth = 0;
    // 临时缓冲清空 如果除此二个之外还有别人填写临时缓冲，那就不要在这里清空。
    uint32 u32BytesFilled = 0;
    u32BytesFilled  += MoveEventBuftoTxBuf1500();// 取出事件队列到临时缓冲
    if(u32BytesFilled == 0)
        u32BytesFilled  += MoveDataBuftoTxBuf1500();// 取出数据队列到临时缓冲

    //if(u16BytesFilled <= (MAX_PACKLIMIT_SIZE - 160)) // 单个数据点为120字节左右
    //{
    //    u16BytesFilled  += MoveDataBuftoTxBuf1500();// 取出数据队列到临时缓冲
    //}

#if DEBUG_TEST_COMMDATA_MODE == 1
    uw32_TestPackSum++; //  已发包的包号  //  仅为测试用，成型后要清理丢。
    memcpy(&Pack_Tx_Buff_1500[CMD_HOLDHAND_LENGTH - 4], &uw32_TestPackSum, 4);
#endif

    return u32BytesFilled;
}


//----------------------------------------------------------------------------------


//<<<End>>> EthernetParse Methods

