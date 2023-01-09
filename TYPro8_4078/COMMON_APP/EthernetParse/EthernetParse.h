/*****************************************************************************
// 程序文件                   :EthernetParse.h     主要用于网络处理
// 文件描述                   :为MCU board服务
// 编写日期(MM.DD.YYYY)       :10.15.2012
*****************************************************************************/

#ifndef _ETHERNETPARSE_H_
#define _ETHERNETPARSE_H_

#include "../../common_OS/protocols/SF_Hwf.h"
#include "../../COMMON_OS/TCP-NET/Net_Config.h"
#include "../../COMMON_OS/TCP-NET/RTL.h"
#include "../../COMMON_OS/TCP-NET/TcpTxRx.h"


#define MAX_RTC_CALIB_INTERVAL_TIME              (3600*1) //(3600*4)    // (3600*24) //24小时   (3600*4)    (10*60)   设置范围：3分钟---24小时

#define RTC_CALIB_COST_TIME                      (60+10)     // 12个点处理70秒
#define RTC_CALIB_INTERVAL_TIME_REQUEST          (MAX_RTC_CALIB_INTERVAL_TIME-60-RTC_CALIB_COST_TIME)  // 提前60秒请求外网对时，12个点处理70秒
#define RTC_CALIB_INTERVAL_TIME_PARSE            (MAX_RTC_CALIB_INTERVAL_TIME-RTC_CALIB_COST_TIME)     // 提前12个点处理70秒
#define PORT_NUM 5678
#define PORT_NUM2    (PORT_NUM + 1)

extern U8 own_hw_adr[ETH_ADRLEN];
extern LOCALM nlocalm[3];
extern U8  lhost_name[16];
extern U8  uc_remotemIpAdr[4];


#define CMD_SYNC_LEN			   (8)
//#define MAX_CMD_BUFFER_SIZE	    (0x4000)
//#define MAX_CMD_BUFFER             (1536)
#define CMDTXASYNC_BUFFTOTAL       (32)  // (36)  // (16)
#define MAX_CMD_LOGDATA_BUFFER     (MAX_PACKLIMIT_SIZE - CMDTXASYNC_BUFFTOTAL)  // 1000
#define CMDTXSEND_RESERVED         (8)
#define CMD_HOLDHAND_LENGTH		   (16) // 16字节[8Byte(AA)+4Byte(Length)+4Byte(Reserved]
#define CMD_TXSEND_SDL_POINT       (7) // 取7种数据变量类型;


//#define MAX_PACKLIMIT_SIZE            (1500)  //  最大单包字节数1500
#define MAX_PACKLIMIT_SIZE            (1460)  //  最大单包字节数1460;因[SYN] MSS=1460 

//#define MAX_TESTDATA_BUFFER_SIZE      (50 * MAX_PACKLIMIT_SIZE)  //  大于43个包已溢出长度16位管理
//#define MAX_EVENT_BUFFER_SIZE         (50 * MAX_PACKLIMIT_SIZE)  //  队列不能大于43个包
#define MAX_TESTDATA_BUFFER_SIZE      (MAX_PACK_NUMBER * MAX_PACKLIMIT_SIZE)  //  大于44个包已溢出长度16位管理	// 44 -> 43 should be more reliable //最大可以管理2,941,758个包，但是100M网口最多支持100,000包/秒，因为内存不足，所以暂定为500个包 170505 zyx
#define MAX_EVENT_BUFFER_SIZE         (MAX_PACK_NUMBER * MAX_PACKLIMIT_SIZE)  //  队列不能大于44个包 //最大可以管理2,941,758个包，但是100M网口最多支持100,000包/秒，因为内存不足，所以暂定为1,000个包 170505 zyx
//#define MAX_BUFFERTXINDEX_SIZE        8000
#define MAX_PACK_NUMBER					500	// 队列不能大于43个包 //最大可以管理2,941,758个包，但是100M网口最多支持100,000包/秒，因为内存不足，所以暂定为500个包 170505 zyx

#define CODE_EMPTY_NUMBER		              (6)
#define CODE_MD5_NUMBER		                  (32)
#define CODE_MD5_USE		                  (16)
#define CODE_EMPTY		                      (0)

#define MAX_BATCH_GROUP_BUFFER        (sizeof(MP_BATCH_GROUP))
#define MAX_PACK_LOGPOINT                     (109)  // 13*109=1417

#ifndef MAX_FLOAT
#define MAX_FLOAT   (3.4e38f)
#endif
#ifndef MIN_FLOAT
#define MIN_FLOAT   (-3.4e38f)
#endif

#define FAST_UPDATA_DIPLAY          (50)  // 5ms
#define NORMAL_UPDATA_DIPLAY        (30000)  // 3s

typedef enum
{
    ESMOOTHRTCSM_IDLE,
    ESMOOTHRTCSM_GETTIME,
    ESMOOTHRTCSM_CALCULATION,
} ESMOOTHRTCSM;


typedef enum
{
    SEND_NONE = 0,
    SEND_EVENT_CONTENT,
    SEND_EVENT_SDL,
    SEND_METAVARIABLESYNCH,
    SEND_REPORT_ASSIGN,
    SEND_REQUEST_SYSTEMTIME,
    SEND_DISPLAY_DATA,
    SEND_LOG_DATA,
    SEND_IDLE_DATA,
    SEND_LOG_SDL,
    SEND_GET_CHANNELBOARD_MG,
    SEND_WFRAM_EVENT,
    SEND_AUX_LOGDATA,
    SEND_ERROR_MESSAGE,
    SEND_GET_SCHSTATUS,
    SEND_PULSE_LOG_DATA,
    SEND_LOG_BMS_DATA,
    SEND_IDLE_BMS_DATA,
    SEND_IV_BUILDINAUX_LOGDATA,
    SEND_GET_IV_BOARDTYPE_MG,
    SEND_REPORT_SIMULATION_REQUESTDATA,
    SEND_LOG_SDL_CYCLE,
    SEND_AUX_ACIMDATA,
    SEND_LOG_SMB_DATA_FLOAT,
    SEND_LOG_SMB_DATA_STRING,
    SEND_IDLE_SMB_DATA_FLOAT,
    SEND_IDLE_SMB_DATA_STRING,
    SEND_BUILTIN_DATA,
    SEND_AUX_GROUPLOGDATA,
    SEND_MVUDSYNCH,

} TE_SEND_MESSAGETYPE;

typedef enum
{
    SEND_REQUEST_NONE = 0,
    SEND_REQUEST_CONTENT,
    SEND_REQUEST_DATA,

} TE_SEND_LOGMESSAGETYPE;

typedef enum
{
    LOG_NORMAL_DATA_POINT     = 0,
    LOG_CRITICAL_DATA_POINT = 0X01,
} TE_LOGDATA_MESSAGETYPE;

typedef enum
{
    VClampUnsafe				 = 0,
    TemperatureChamberUnsafe,
} TE_REPORT_UNSATE;


/*  //----------------------------------------------------------------------------------------------------------------------------------
包命令总体结构：
0-7	         8-11	            12   13    14-21	         22	…	                                               N+1	N+2	N+3	N+4
包头（8字节）数据总长度（4字节）预留 预留  命令码字（8字节） 命令数据（N字节）(少于1.5K字节,最好控制在1K字节左右)  校验（4字节）
//----------------------------------------------------------------------------------------------------------------------------------
//=================
// 包命令详细结构
//=================
包命令详细结构：
0-3	                  4-7	                   8-11	             12     13
AAAA5555(32位)        9999AAAA(32位)           数据总长度(32位)  预留   预留
14-17	              18-21	                   22…N	                (N+1)…(N+4)
执行命令码字（32位）  扩展命令码字（32位预留） 命令数据（少于1.5K*8位） 校验(32位)
*/  //----------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
#define CMD_FULL	                          (0XFFFFFFFF)
#define CMD_EMPTY		                      (0X00000000)
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
#define CMD_HEADFIELD1_HAND_FIRST		      (0XAAAAAAAA) //(0XAAAA5555)
#define CMD_HEADFIELD1_HAND_SECOND	          (0XAAAAAAAA) //(0X9999AAAA)
#define CMD_HEADFIELD2_PACKLENGTH		      (CMD_FULL)
#define CMD_HEADFIELD3_ORDERCODE_FIRST		  (CMD_FULL)
#define CMD_HEADFIELD3_ORDERCODE_SECOND		  (CMD_EMPTY)
#define CMD_HEADFIELD4_ORDERDATA		      (CMD_FULL)
#define CMD_HEADFIELD5_CHECKSUM		          (CMD_FULL)

#define CMD_HEADFIELD1_HAND_LENGTH	          (2)
#define CMD_HEADFIELD2_SUMLONG_LENGTH	      (1)
#define CMD_HEADFIELD3_ORDERCODE_LENGTH	      (2)
#define CMD_HEADFIELD4_ORDERDATA_LENGTH	      (375)
#define CMD_HEADFIELD5_CHECKSUM_LENGTH	      (1)


/*typedef struct	 // ST_ASYNC_CMDTXBUFF
{
uint8     m_CmdTxAsync_Buffer[MAX_CMD_BUFFER_SIZE];
uint16    m_CmdTxAsync_Length; //  发送@异步信道数组缓冲区数据长度
}ST_ASYNC_CMDTXBUFF;*/

//typedef CIRCULAR_QUEUE(ST_ASYNC_CMDTXBUFF,CMDTXASYNC_BUFFTOTAL)  St_Async_TxBuffS;

typedef   CIRCULAR_QUEUE(uint8, MAX_TESTDATA_BUFFER_SIZE) TestDataTxBuffer11;
typedef   CIRCULAR_QUEUE(uint8, MAX_EVENT_BUFFER_SIZE)    TestEventTxBuffer12;

//typedef   CIRCULAR_QUEUE(uint32, MAX_BUFFERTXINDEX_SIZE)  TestDataTxIndexQueue13; // 数据命令结构个数管理,相当于指针
typedef   CIRCULAR_QUEUE(uint32, MAX_PACK_NUMBER)  TestDataTxPackEndQueue13;	// 数据命令结构个数管理,相当于指针 // uint16 -> uint32 zyx 170505

typedef struct
{
    ST_MultPack_SecondField        m_stMSecondField;   // （多包信息管理第二区域)
    WORD     m_wChannelNumber;
    WORD     m_wMessage_Type;
    BYTE     m_btEvent;
    BYTE     m_ItemCount_SDL;
    WORD     m_wStepID;
    DWORD    m_dwCycleID;
    BYTE     m_btCMD;
    BYTE     m_btSCHEvent_Source;
    ST_EVENT_SCHEDULE_RUNNING     m_st_RunningStatus;
    ST_EVENT_SCHEDULE_LIMITREACH  m_st_LimitReachStatus;
} ST_STATUS_SCHEDULE;

typedef struct
{
    St_AUX2NDV    st_2NDV;
    BYTE  m_btNone_11[20];    // 预留reserved
} ST_2NDV_U;				  		//二次电压的配置信息结构 共计40字节


// typedef struct
// {
// 	signed long                  m_FragIdx;
// 	signed long                  m_FragCount;
// 	signed long                  m_ByteCount;
// 	signed long                  m_PointCount;
//
// // 	signed long                  m_PackCount;
// // 	signed long                  m_PackCount_Received;

// 	unsigned char                m_bDownloading;  //正在下载？

// 	St_MD5                       m_MD5;
// }St_SimuDownMgr;


class EthernetParse
{
public:
    static UInt16 MyStatus;
    static UInt16 Priority;
    static UInt16 TaskID;			// ETHERNETPARS_ETASK should be a definition which positioning this task in OS_Task[]

#if  USE_AS_AUXMCU == 1
    static unsigned char  m_AUX_WriteMap;
#endif

// 	static unsigned char  m_bP1Broken;
// 	static unsigned char  m_bP2Broken;
    static unsigned short m_u16Token;
    static unsigned char  m_ucTokenBrokenCount;
    static MsTime      m_WatchDogTime;
    static unsigned char  m_bResetSchedule;
    static unsigned char  m_bResetThirdParty;
    static unsigned char  m_IfRequest_NTP;
    static unsigned short uw16_MyIP[2];             // "MYIP1.MYIP2.MYIP3.MYIP4"    网址
    static unsigned short uw16_RemoteIP[2];
    static unsigned char  uc_MyMAC[6];             // "M1-M2-M3-M4-M5-M6"
// 	static UChar8         uc_EEP_RemoteIP[4];
// 	static unsigned char  LastDigitOfIPMAC;
    static unsigned char  uc_Total_Flag;
    static unsigned char  uc_LogCriticalPoint;  // 记录关键点标志
    static Un_MC_REPORT*	    m_pReport;
    static MP_CONFIRM_FEEDBACK   m_ResultFeedback;

    static St_Assign*                   m_st_Assign;

    static St_MD5Assign* 				m_simu_MD5Assign; //12.22.2013ADD

    static ST_STATUS_SCHEDULE           m_ReportStatus;
    static const ST_SCHEDULE_LOGSETTING_DATA  m_ReportLogMetaVariable;
    __align(8) static uint8 			m_btPackDataBuffer[MAX_MULTPACK_SIZE]; // 用于多包下载缓冲暂存
    __align(8) static uint8     		m_btPackDataUploadBuffer[MAX_MULTPACK_UPLOAD_SIZE]; // 用于多包上传缓冲暂存


    static uint8	Pack_Tx_Buff_1500[MAX_PACKLIMIT_SIZE]; // Pack_Tx_Buff_1500[1500];
    static uint16	Pack_Tx_Buff_1500_used_depth;

//	__align(8) static uint8    m_Point_Temp[20]; // 取点结构缓冲区
    __align(8) static uint8    m_Point_Temp[MAX_PACKLIMIT_SIZE]; // 取点结构缓冲区

// 	static MsTime              m_Interval_NetLinkTime;    // 网络连接成功时间间隔器
    static QueueP              Temp_Q;  // 临时使用字符串队列指针
    static unsigned char uc_SendFail;
    static UWord16 Pack_1500_depth_back;
    static UWord16    uw16_SendCounter;
    static unsigned long      AbortTime;
    static unsigned long long uw64_CTI_SetTimeInterval;
	static MsTime    DataTxCounter;
public:
    static void DoWork(void);
    static void MyInit(void);
    static void EndTask(void);
    static void SetRemoteIP(unsigned char ServerIP);
    static void SetLocalIP5MAC(unsigned char LastDigit);
    static void MPReportData(UWord16 uw16_ChannelNumber,unsigned char uc_SendType);
    static void PushToSendingEventBuf(uint8 * pBytedata, uint16 EVENT_LENGTH_IN_BYTE);
    static void PushToSendingDataBuf(uint8 * pByteData,uint16 DATA_LENGTH);
    static void PrepareTxRawDataForSending(void);
    //inline static void SDLPointChang(St_SDL_Point * S, St_SDL_Point_Pack * D); //mits8
    inline static void SDLPointChang(St_SDL_Point_4BytesCycleIdx * S, St_Data_Pairs_4BytesCycleIdx * D);  //mits8
    static void MPReport_WriteFram_Event(St_FramMessage FramMsg, MP_REPORT_FRAM_EVENT *pReport);
#if DEBUG_TEST_COMMDATA_MODE == 1
    static UWord32 uw32_TestDataPack; // 计算已发数据包的包号 //  仅为测试用，成型后要清理丢。
    static UWord32 uw32_TestEventNum; // 计算已发信息号 //  仅为测试用，成型后要清理丢。
    static UWord32 uw32_TestPackSum; // 计算已发包的包号 //  仅为测试用，成型后要清理丢。
    static UWord32 uw32_TestBlock1; // 计算发送阻塞次数 //  仅为测试用，成型后要清理丢。
    static UWord32 uw32_TestLogD[MAXCHANNELNO]; // 计算各个通道已记录的点数 //  仅为测试用，成型后要清理丢。
#endif
    static St_UnsafeLimit   UnsafeLimit[MAXCHANNELNO];   //for  Limit Unsafe  20140211


    static void MPBatchGrouping(MP_BATCH_GROUP *pCmd, unsigned char uc_Write_Flag);
#if  USE_AS_AUXMCU == 0
    static void SendTasksMsgProcess(UWord16 Chan, UChar8 SourceType, unsigned char UnsafeType, float value);
#endif
    __align(8) static uint8    m_CmdTxBuffer_Async_Temp[MAX_CMD_BUFFER_SIZE]; // @异步通道 async 暂时组包缓冲区
    static uint16    DataTx_Length;      //  发送@异步信道数据缓冲区数据长度
    static void HeadFieldSaveBuff( BYTE *pBuf, uint16 *CmdTxAsync_Length, unsigned char uc_SendType);
	static void MPReport_Request_NTPCalib(signed short offset);

private:
    __align(8) static uint8    m_CmdRxBuffer[MAX_CMD_BUFFER_SIZE];
    __align(8) static uint8    m_CmdTxBuffer_SynchroChannel[MAX_CMD_BUFFER_SIZE];
// 	__align(8) static uint8    m_CmdTxBuffer_Async_Temp[MAX_CMD_BUFFER_SIZE]; // @异步通道 async 暂时组包缓冲区

    static uint8* 			m_CmdRxBuf;         //  接收同步信道缓冲区指针
    static uint8* 			m_CmdTxBufSynchro;  //  发送同步信道缓冲区指针

    static uint32			m_CmdRxLen;
    static uint32			m_SyncByteCnt;

    static Un_PC_MESSAGE* 		m_pCmd;
    static Un_MC_FEEDBACK_MESSAGE* 	m_pFeedBack;

    //static St_RecordTime			m_CmdLastStamp;
    //static St_RecordTime			m_CmdIVLastStamp;
    //static St_RecordTime			m_CommLastStamp;

// 	static uint8 			m_btPackDataBuffer_IsUseFlag;
// 	static uint32 			m_dwPackDataBuffer_TotalLength;
// 	static uint32 			m_dwPackDataBuffer_SavePoint;
// 	static uint32 			m_dwPackDataBuffer_GetPoint;
// 	static uint8* 			m_pPackDataBuffer;

    static MsTime  m_IntervalTime_RequestSystemtime;
// 	static uint8 			m_ucFull_Count;
    static ST_MultPack      st_MultPackManage;
    //static St_SimuDownMgr   m_SimuDownMgr;

    static TestDataTxBuffer11           TestDataTxBuffer;
    static TestEventTxBuffer12          TestEventTxBuffer;
    //static TestDataTxIndexQueue13		TestDataTxIndexQueue; // 数据命令结构个数管理,相当于指针
    static TestDataTxPackEndQueue13		TestDataTxPackEndQueue; // store the length of each Sending Data Pack, <1460, Yifei

// 	static uint16    DataTx_Length;      //  发送@异步信道数据缓冲区数据长度
    static uint16    EventTx_Length;     //  发送@异步信道事件缓冲区数据长度
    static uint32    m_dwSend_SN;        //  发送事件编号

    static MsTime    EventTxCounter;
// 	static MsTime    DataTxCounter;
    static MsTime              SetIVBoardTypeTime;
    static unsigned char       SetIVBoardTypeFlag;
		static MsTime              SetAuxBoardTypeTime;
		static unsigned char       SetAuxBoardTypeFlag;

    //Operation
protected:
    static void CmdParse(void);
// 	static void CheckIfResetSystem(void);
// 	static void SwapByteInWord(uint16&);
// 	static void SwapByteInDWord(uint32&);
// 	static float ByteSwapFloat(float&);

// 	static UWord16 MP_GetWord_RxBuf(BYTE * uc_start);
// 	static UWord32 MP_GetDWord_RxBuf(BYTE * uc_start);
// 	static float   MP_GetFloat_RxBuf(BYTE * uc_start);
    static void TotalNumberCountProcess(UWord16 * Chan, UWord16 * Total,UChar8 * Flag);

private:
    static bool SyncParse(void);
    static void SendFeedBack(uint8 AckFlag, uint16 len);
    //	static bool IfCheckSumOk(uint16 len);
    //static void CheckIfResetSystem(void);
#if READ_MEMORY_FUNCTION == 1
    static void MPGetDebugData(MP_GET_DEBUG_DATA *pCmd,MP_GET_DEBUG_DATA_FEEDBACK *pFeedback);
#endif
    static void MPSetSystemTime(MP_SET_SYSTEMTIME *pCmd);
    static void MPSetSystemTimeFeedback(MP_SET_SYSTEMTIME_FEEDBACK *pFeedback);
    static char GetSinglePack(MP_SINGLE_PACKDATA *pCmd,bool IsNeedProtect);
    static void MPSetChannelBoard(MP_SINGLE_PACKDATA *pCmd);
    static void MPSetChannelMap(MP_SINGLE_PACKDATA *pCmd);
    static void MPCMD_Feedback(MP_CONFIRM_FEEDBACK *pFeedback);
    static void MPSetChannelPID(MP_SINGLE_PACKDATA *pCmd);
    static void MPSetChannelCalibrate(MP_SET_CHANNEL_CALIBRATE *pCmd);
    static void MPSetM0ChannelCalibrate(MP_SET_CHANNEL_CALIBRATE *pCmd);
    static void MPSetM0Action(MP_INDEPENDENT_ACTION *pCmd);
    static void MPIndependentActionSwitch(MP_INDEPENDENT_ACTIONSWITCH *pCmd);
    static void MPIndependentAction(MP_INDEPENDENT_ACTION *pCmd);
    static void MPIndependentEmergencyStop(MP_INDEPENDENT_EMERGENCYSTOP *pCmd);
    static void MPIndependentReadData(MP_INDEPENDENT_READDATAORSTATE *pCmd);
    static void MP_M0_ReadData( MP_INDEPENDENT_READDATAORSTATE *pCmd);
    static void MP_M0_ReadCalibPara(MP_M0_READCalibPara *pCmd);
    static void MP_M0_ACTIONSWITCH(MP_SETM0_CalibMode *pCmd );
    static void MPIndependentReadCfg(MP_INDEPENDENT_READ_CFG *pCmd);
    static UWord16 MPIndependentReadDataFeedback(MP_INDEPENDENT_READDATAORSTATE_FEEDBACK *pFeedback);
    static UWord16 MPIndependentReadCfgFeedback(MP_INDEPENDENT_READ_CFG_FEEDBACK *pFeedback);
// 	static void MPIndependentPowerOn(MP_INDEPENDENT_EMERGENCYSTOP *pCmd);
// 	static void MPIndependentResetCAN(MP_INDEPENDENT_EMERGENCYSTOP *pCmd);
    static void MPIndependentMetavariable(MP_INDEPENDENT_METAVARIABLE *pCmd);
    static void MPIndependent_BoardVersion(MP_INDEPENDENT_READ_BOARD_VERSION *pCmd);
    static UWord16 MPIndependent_IVBoardVersionFeedback(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK *pFeedback);
    static UWord16 MPIndependent_AuxBoardVersionFeedback(MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK *pFeedback);
    //static void MPScheduleSetSetting(MP_SCHEDULE_SET_SETTING *pCmd);
    //CAN BMS
    //#if (SUPPORT_CAN_BMS==1)
    static void MPSetBMSChannelConfig(MP_SINGLE_PACKDATA *pCmd);
    static void MPSetIVBoardType(MP_SET_IV_BOARDTYPE *pCmd);
    static void MPGetIVBoard(MP_GET_IV_BOARDTYPE *pCmd);
    static void MPReport_GetIV_BOARDTYPE(void);

    static void MPSetCANBMSMap(MP_SINGLE_PACKDATA  *pCmd);
    static void MPSetCANMessage(MP_SET_CANMSG *pCmd);
#if( USE_AS_AUXMCU == 1)
    static void MPSetMultiCANMessage(MP_SINGLE_PACKDATA  *pCmd);//170828 zyx
#endif
    static void CmdSetToFeedback(DWORD * Cmd, DWORD * Cmd_Extend, unsigned char Success);
    static void CmdGetToFeedback(DWORD * Cmd, DWORD * Cmd_Extend, WORD * ChNumber, WORD * TotalNumber);
    static void MPSetIntervalTime_Logdata(MP_SET_INTERVALTIME_LOGDATA * pCmd);
public:

private:
    static void CombineBMSPackage(unsigned char uc_ChannelNo,unsigned char uc_SendType, unsigned char Interval);
    static void MPReportCANData(unsigned char uc_ChannelNo,unsigned char uc_SendType);
    static void MPCANConfigAssign(MP_CANCONFIG_ASSIGN *pCmd);
    static void MultPackDataToSinglePack(unsigned char uc_SendType);
    //static void MPCAN_ReportSDL_IdleData(unsigned char uc_DataType);
    //#endif

#if (USE_AS_AUXMCU == 0 )
		static void MPGetMetaVariables(MP_GET_META_VARIABLES *pCmd,MP_GET_META_VARIABLES_FEEDBACK *pFeedback);
    static void MPSetSMBChannelConfig(MP_SINGLE_PACKDATA *pCmd);
    static void MPSMBConfigAssign(MP_SMBCONFIG_ASSIGN *pCmd);
    static void MPReportSMBDataFloat(unsigned char uc_ChannelNo,unsigned char uc_SendType);
    static void MPReportSMBDataString(unsigned char uc_ChannelNo,unsigned char uc_SendType,unsigned char uc_MetaIdx);

    static void MPReport_SimulationRequestData(MP_REPORT_SIMULATION_REQUESTDATA *pReport);
    static void MPSetChannelBoard_BuiltIn(MP_SINGLE_PACKDATA *pCmd);
    static void MPGetChannelBoard_BuiltIn(MP_GET_CHANNEL_BOARD *pCmd);
#if (SCH_STATUS_REPORT_ENABLE  == 1)
    static void MPScheduleGetStatus(MP_SCHEDULE_GET_STATUS *pCmd);
#endif
#endif
    static void MPScheduleDownload(MP_SINGLE_PACKDATA  *pCmd);
    static void MPSimulationDownload(MP_SIMULATION_DOWNLOAD  *pCmd); //12.22.2013ADD
    static void MPScheduleStop(MP_SCHEDULE_STOP *pCmd);
    static void MPSetMetaVariable(MP_SET_META_VARIABLE *pCmd);
	static void MPUpdateMetaVariable(MP_UPDATE_META_VARIABLE_ADVANCEMODEL * pCmd);
    static void MPScheduleContinue(MP_SCHEDULE_CONTINUE *pCmd);
    static void  MPScheduleStopAll(void);  //Add by DKQ on 08262013 for MP_RESET_Status
    static void MPScheduleJump(MP_SCHEDULE_JUMP *pCmd);
    static void MPScheduleAssign(MP_SCHEDULE_ASSIGN *pCmd);
    static void MPSimulationAssign(MP_SIMULATION_ASSIGN *pCmd);      //12.22.2013ADD
    static UWord16 MPScheduleAssignFeedback(MP_ASSIGN_FEEDBACK *pFeedback);
    static UWord16 MPSimulationAssignFeedback(MP_SIMULATION_ASSIGN_FEEDBACK *pFeedback); //12.22.2013ADD
    static void MPScheduleReport_Event(UWord16 uw16_ChannelNumber, unsigned char uc_SendType, MP_REPORT_EVENT *pFeedback, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_MetaVariableSynch(UWord16 uw16_ChannelNumber, MP_REPORT_METAVARIABLESYNCH *pReport, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_MVUDSynch(UWord16 uw16_ChannelNumber, MP_REPORT_METAVARIABLESYNCH *pReport, uint16 *CmdTxAsync_Length);
// 	static void MPScheduleReport_LogData(UWord16 uw16_ChannelNumber, UChar8 uc_LogType,MP_REPORT_LOGDATA *pReport, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_LogData_TC_Counter(UWord16 uw16_ChannelNumber, UChar8 uc_LogType,MP_REPORT_LOGDATA_TC_Counter *pReport, uint16 *CmdTxAsync_Length);
    //static void MPScheduleReport_Assign(UWord16 uw16_ChannelNumber, MP_CONFIRM_ASSIGN *pReport, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_Pulse_LogData(MP_REPORT_PULSE_LOGDATA *pReport, uint16 *CmdTxAsync_Length);
// 	static void MP_Request_SystemTime(MP_REQUEST_SYSTEMTIME *pReport, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_EventSDL(UWord16 uw16_ChannelNumber, unsigned char uc_SendType, MP_REPORT_EVENT_SDL *pReport, uint16 *CmdTxAsync_Length);
    //static void MPScheduleReport_LogSDL(UWord16 uw16_ChannelNumber, MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length); //mits8
    static void MPScheduleReport_LogSDL(UWord16 uw16_ChannelNumber, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport, uint16 *CmdTxAsync_Length); //mits8
    static void MPReport_IV_BuildInAux_LogData(MP_REPORT_BUILDINAUX_LOGDATA *pReport, uint16 *CmdTxAsync_Length);
    static void MPScheduleReport_LogSDL_Cycle(UWord16 uw16_ChannelNumber, MP_REPORT_LOGDATA_SDL_CYCLE *pReport, uint16 *CmdTxAsync_Length);
// 	static void HeadFieldSaveBuff( BYTE *pBuf, uint16 *CmdTxAsync_Length, unsigned char uc_SendType);
    static inline BYTE Char2Num( BYTE Byte ); //zyx 180305
    static void VersionProcess(UNIT_VER_INFO    *st_Ver);
    static void MPScheduleAckEvent(MP_SCHEDULE_ACK_EVENT *pCmd);
    static void ChangeAssignResult(unsigned char uc_Flag,St_Assign *Assign);
    static void ChangeSimulationAssignResult(unsigned char uc_Flag,St_MD5Assign *Assign);  //12.22.2013ADD
    static void MPScheduleOnLineUpdate(MP_SINGLE_PACKDATA  *pCmd);

    static uint16  MoveEventBuftoTxBuf1500(void);
    static uint16  MoveDataBuftoTxBuf1500(void);
    static uint16  Fill_Pack_Tx_Buf_1500(void);

    static uint8  AddCheckSum( BYTE *pbtBuf, UINT uSize );
    static uint8  IsCheckSumOK( BYTE *pbtBuf, UINT uSize );

    static void MPGetChannelBoard(MP_GET_CHANNEL_BOARD *pCmd);
    static void MPReport_GetChannelBoard(void);
    static void MPReport_ErrorMessage(MP_REPORT_TOPC_ERROR *pReport);
    static void MPSetDspBasicParameter(MP_SET_DSP_BASIC_INFO *pCmd);
    static void MPSetDspAdjustParameter(MP_SET_DSP_ADJUST_INFO *pCmd);
    static void MPReadDspBasicParameter(MP_READ_DSP_BASIC_INFO *pCmd);
    static void MPReadDspAdjustParameter(MP_READ_DSP_ADJUST_INFO *pCmd);
    static void MPSetDspIndentityParameter(MP_SET_DSP_INDENTITY_INFO *pCmd);
    static void MPReadDspIndentityParameter(MP_READ_DSP_INDENTITY_INFO *pCmd);
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    static void MPReport_GetSCH_Status(UWord16 uw16_ChannelNumber, MP_REPORT_GET_SCHSTATUS *pReport);
#endif
    static void MPReport_BuiltInLogData(MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length);
#if  USE_AS_AUXMCU == 1
    static void MPReport_AuxLogData(MP_REPORT_LOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length);
    static void MPReport_AuxGroupLogData(MP_REPORT_GRPLOGDATA_SDL *pReport, uint16 *CmdTxAsync_Length);
    static void MPSetChannelBoard_Aux(MP_SINGLE_PACKDATA *pCmd);
    static void MPGetChannelBoard_Aux(MP_GET_CHANNEL_BOARD *pCmd);
    static void MP_MTCI_GroupSetting(MP_MTCI_SET_GROUP_SETTING *pCmd);
    static void MP_RSN_ErrorAckEvent(MP_RSN_ERROR_ACK_EVENT *pCmd);
    static void MPAuxACIMStartedACKEvent(MP_SENT_ACIMTEST_START *pCmd);
    static void MPAuxACIMStopACKEvent(MP_SENT_ACIMTEST_STOP *pCmd);
    static void MPReport_AuxACIMData(MP_REPORT_ACIM_DATA *pReport, uint16 *CmdTxAsync_Length);
#endif
    //CANAUX_BROADCAST START//
    static void MPSetCANAUX_Broadcast_Config(MP_SINGLE_PACKDATA *pCmd);//MP_SET_CANAUX_BROADCAST_CONFIG
    //CANAUX_BROADCAST END//
};


#endif





