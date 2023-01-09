#include "../Entry/includes.h"
UInt16 Smart_Battery::Priority = PRIORITY4;
UInt16 Smart_Battery::TaskID = SMBUS_TASK;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char 	Smart_Battery::WriteSMBBuf[MAXBYTENUM];
unsigned long  	Smart_Battery::I2Ctimeout;
bool 					 	Smart_Battery::ErrorFlg;
St_SMBMessage		Smart_Battery::SMBMsgBuf[MAXSMBMESSAGE];
St_DoSMBMessage Smart_Battery::DoSMBMessage;
St_MsgBufManage Smart_Battery::MsgBufManage;
unsigned char 	Smart_Battery::m_SMBusStatus;
bool						Smart_Battery::BeSupportSmbus;
bool            Smart_Battery::SMB_RunFlg;
bool						Smart_Battery::OfflineFlg;
unsigned long   Smart_Battery::OfflineCounter;
unsigned char 	Smart_Battery::m_ReadDataType;
unsigned char   Smart_Battery::m_SMBEngineStatus;

//MsTime   				Smart_Battery::m_RecordReadDelayTimeA;
//MsTime   				Smart_Battery::m_RecordReadDelayTimeB;
//MsTime   				Smart_Battery::m_RecordReadDelayTimeC;
unsigned char 	Smart_Battery::m_ReturnNextStatus;
St_SMBData			Smart_Battery::m_SMBData;
//bool   					Smart_Battery::be_Interrupt=false;  //comment out by zc 06/05/2018
ST_SMBBMS_CONFIG_CH  Smart_Battery::DownLoadConfig;

St_SMBBMS_ConfigBuf     		Smart_Battery::m_SMBCfgBuf;
St_SMBConfigAssignTab  	Smart_Battery::m_SMBAssignMD5[MAXCHANNELNO];
St_SMBUnsafeInfo   		Smart_Battery::m_SMB_OverTime[MAXCHANNELNO];  //add by zc 12012017
unsigned char					Smart_Battery::SMBus_InitDelayCounter = 10;

bool Smart_Battery::FirstLog_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
bool Smart_Battery::LastLog_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
bool Smart_Battery::FirstRefresh_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019

unsigned char 	m_FindSignalIdx;
unsigned long   m_DelayCount=0;
unsigned char m_buf[SMB_MAX_BYTE_LENGTH_PER_SIGNAL];
St_SMBBMS_ParsedMeta 	Smart_Battery::m_CurrentSMBSignal;
unsigned char 				Smart_Battery::m_CurrentBufIdx;
unsigned char         Smart_Battery::m_OptWordAddress;
unsigned char         Smart_Battery::m_SMBUnitID;
unsigned long         Smart_Battery::m_RefreshInterval[MAX_SMB_BMS_SIGNAL];
unsigned long      		Smart_Battery::m_SMBSignalErrorCounter[MAXCHANNELNO][MAX_SMB_BMS_SIGNAL];
MsTime 								Smart_Battery::m_RefreshCounter[MAXCHANNELNO][MAX_SMB_BMS_SIGNAL];
MsTime								Smart_Battery::m_PreviousLogUploadTime[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
MsTime								Smart_Battery::m_PreviousIdleUploadTime[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
MsTime 								Smart_Battery::LastIVtime;
bool            			Smart_Battery::m_UpdataFlg[MAX_SMB_BMS_SIGNAL];
unsigned char      		Smart_Battery::m_SMBLogIdx;

bool						Smart_Battery::lastSMBData[MAXCHANNELNO];

MsTime     Smart_Battery::MVOffline_IntervalTime;
unsigned char Smart_Battery::uc_MVOffline[MAX_SMB_BMS_SIGNAL];
unsigned char Smart_Battery::uc_MVOffline_statuse;
#pragma arm section
void        Smart_Battery::DoWork(void)                                                                 //任务执行函数
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SMB] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_SMB);
#endif

    //ControlPin::SetMeHigh(LEDFunc::Pin_HeartBeat);
    if(BeSupportSmbus)// && SMB_RunFlg)
    {
        ParseSMBMessage();
        ProcessSMBStatus();
        MVOffline_SetFlag();  // yy 20190809
    }
//     else
//         Smart_Battery::MyInit();
    // ControlPin::SetMeLow(LEDFunc::Pin_HeartBeat);

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SMB] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_SMB);
#endif
}
void        Smart_Battery::MyInit(void)                                                                 //任务初始化
{
    Smart_Battery::I2Ctimeout=0;
    Smart_Battery::ErrorFlg=false;
    Smart_Battery::OfflineFlg=false;
    Smart_Battery::OfflineCounter=0;
    Smart_Battery::m_SMBEngineStatus=SMBUS_SEND;
    Smart_Battery::m_ReturnNextStatus=SMB_IDLE;
    m_FindSignalIdx=0;
    m_CurrentBufIdx=0;
    SMB_RunFlg=false;
    m_OptWordAddress=0x00;
    m_SMBLogIdx=0;
    memset(&m_CurrentSMBSignal,0,sizeof(ST_SMB_BMS_SIGNAL));
    CommonFunc::GetTimeTicksUpdate(&MVOffline_IntervalTime);
    for(unsigned char uc_i=0; uc_i<MAX_SMB_BMS_SIGNAL; uc_i++)
    {
        m_RefreshInterval[uc_i]=0;
        m_UpdataFlg[uc_i]=false;
        uc_MVOffline[uc_i] = false;
    }
    //msg queue
    for(unsigned char uc_i=0; uc_i<MAXSMBMESSAGE; uc_i++)
    {
        memset(&SMBMsgBuf[uc_i],0,sizeof(St_SMBMessage));
        MsgBufManage.RecordMsgEmpty[uc_i]=uc_i;  //{0,1,2,3,4,5,6,7}
        MsgBufManage.RecordMsgFull[uc_i]=0;
    }

    for(unsigned char uc_i=0; uc_i<MAXCHANNELNO; uc_i++)
    {
        CommonFunc::GetTimeTicksUpdate(&m_SMB_OverTime[uc_i].m_UnsafeTime);	//add by zc 12012017
    }

    ClearCurrentSMBMsg(&DoSMBMessage);

    MsgBufManage.FullCount=0;
    MsgBufManage.DeletingCount=0;
    MsgBufManage.EmptyCount=7;


// 		/***********************************************************模拟写入SMB参数 */
//  	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_BaudRate=100;
//  	I2CBusNew::I2CInit(I2C1,Smart_Battery::m_SMBCfgBuf.m_buf[0].m_BaudRate);
// // 	Smart_Battery::BeSupportSmbus=true;
// // 	    St_SMBMessage SMBMessage;
// // 			SMBMessage.MsgNumber=SMB_Write_Msg;
// // 			SMBMessage.m_SlaveAddr=0x16;   //入口地址
// // 			SMBMessage.m_bufLen=3;         //字节数
// // 			SMBMessage.Send_buf[0]=0x01;   //发送字节
// // 			SMBMessage.Send_buf[1]=0x01;
// // 			SMBMessage.Send_buf[2]=0x01;
// //
// // 			Smart_Battery::SendMsgToSMB(SMBMessage);
// //
// 	Smart_Battery::m_SMBAssignMD5[0].m_cfgBufIdx=0;  //debug 2017/05/03
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount=64;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_WholeSignalNum=64;
// 	for(unsigned char idx=0;idx<64;idx++)
// 	{
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.CtrlByteNum=1;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_BeUsed =true;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_COMM[0]=idx;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_ReadAddr=0x17;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_HeaderBuf.m_WriteHeader=0x16;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataLen=15;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_DataType=TYPE_WORD;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Sign=true;  //有正负
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Endian=0;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SignalID=idx;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_EndDataIndex=2;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_StartDataIndex=1;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_SMB_RX_Meta=idx;
// 	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Gain=1;
//  	Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[idx].m_Offset=0;
// //
// 	}
// //
// //
//   			BeSupportSmbus=true;


    Smart_Battery::m_SMBusStatus=NOTREADY;
    uc_MVOffline_statuse = false;

}
// void        Smart_Battery::EndTask(void)                                                                //任务结束
// {
//
// }


void Smart_Battery::ProcessSMBStatus(void)
{

    unsigned char MessageNumber;

    MessageNumber=DoSMBMessage.SMBMessage.MsgNumber;                            //取得当前响应信息编号
    /*
    	SMB_Start_Msg,
    	SMB_Write_Msg,
    	SMB_Stop_Msg,
    	*/
    switch(m_SMBusStatus)
    {
    case SMB_NOTREADY:
        if(BeSupportSmbus==true)
        {
            m_SMBusStatus=SMB_IDLE;
// 					OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeA);  //A/B/C三类读数据定时处理  //暂时不用 dirui 2017/12/22
// 					OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeB);
// 					OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeC);
            for(unsigned char uc_i=0; uc_i<MAX_SMB_BMS_SIGNAL; uc_i++)
                m_RefreshCounter[0][uc_i] = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
// 					St_SMBMessage SMBMessage;
// 			SMBMessage.MsgNumber=SMB_Write_Msg;
// 			SMBMessage.m_SlaveAddr=0x16;   //入口地址
// 			SMBMessage.m_bufLen=3;         //字节数
// 			SMBMessage.Send_buf[0]=0x00;   //发送字节
// 			SMBMessage.Send_buf[1]=0x08;
// 			SMBMessage.Send_buf[2]=0x00;
//
// 			Smart_Battery::SendMsgToSMB(SMBMessage);
        }

        break;
    case SMB_IDLE:
        switch(MessageNumber)
        {
        case SMB_Start_Msg:
            m_SMBusStatus=SMB_WAIT;

            break;
        case SMB_Write_Msg:
            m_SMBusStatus=SMB_WRITE;
            break;
        default:
            SMBIdleRoutine();
            break;

        }
        break;
    case SMB_WAIT:
        switch(MessageNumber)
        {
        case SMB_Write_Msg:
            m_SMBusStatus=SMB_WRITE;
            break;
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        default:
            SMBWaitRoutine();
            break;

        }
        break;
    case SMB_LOADSIGNAL:
        switch(MessageNumber)
        {
        case SMB_Write_Msg:
            m_SMBusStatus=SMB_WRITE;
            break;
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        default:
            // ControlPin::SetMeHigh(LEDFunc::Pin_HeartBeat);
            SMBLoadSignalRoutine();
            break;

        }
        break;
    case SMB_ONLINE:
        switch(MessageNumber)
        {
        case SMB_Write_Msg:
            //m_SMBusStatus=SMB_WRITE;
            DoSMBMessage.Used=1;
            SMBOnlineRoutine();
            return;
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        default:
            SMBOnlineRoutine();
            break;

        }
        break;
    case SMB_WRITE:
        switch(MessageNumber)
        {
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        default:
            SMBWriteRoutine();
            break;

        }
        break;
    case SMB_I2CENGINE:   //先设计简易的I2C引擎状态机制，后续再考虑统一归入I2C任务
        switch(MessageNumber)
        {
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        case SMB_Write_Msg:
            //m_SMBusStatus=SMB_WRITE;
            DoSMBMessage.Used=1;
            Smart_Battery::SMBEngineRoutine(0,I2C1,m_ReturnNextStatus,m_SMBData);
            return;
        default:
            Smart_Battery::SMBEngineRoutine(0,I2C1,m_ReturnNextStatus,m_SMBData);
            break;

        }
        break;
    case SMB_ERROR:
        switch(MessageNumber)
        {
        case SMB_Stop_Msg:
            m_SMBusStatus=SMB_IDLE;
            break;
        case SMB_Write_Msg:
            m_SMBusStatus=SMB_WRITE;
            break;
        default:
            SMBErrorRoutine();
            break;

        }
        break;
    case SMB_END:
    default:
        break;
    }
    Smart_Battery::ClearCurrentSMBMsg(&DoSMBMessage);
}
int Smart_Battery::ParseSMBMessage(void)
{

    unsigned char MsgIndex;

    if(MsgBufManage.FullCount==0)
        return -1;


    for(unsigned char uc_i=0; uc_i<(MsgBufManage.FullCount); uc_i++)
    {
        MsgIndex=MsgBufManage.RecordMsgFull[uc_i];
        if (SMBMsgBuf[MsgIndex].MsgNumber==SMB_Stop_Msg)
        {
            GetSMBMsg(MsgIndex);
            return 0;
        }
    }

    if(DoSMBMessage.Used==1)
        return 1;
    else
    {
        MsgIndex=MsgBufManage.RecordMsgFull[0];
        GetSMBMsg(MsgIndex);
        return 1;
    }
}

int Smart_Battery::SendMsgToSMB(St_SMBMessage SMBMessage)
{

    unsigned char MsgRecordCount;


    if ((MsgBufManage.FullCount>7)||(MsgBufManage.EmptyCount<0))
    {
        //error
        return -1;
    }
    MsgBufManage.RecordMsgFull[MsgBufManage.FullCount] = MsgBufManage.RecordMsgEmpty[MsgBufManage.EmptyCount];
    MsgRecordCount=MsgBufManage.RecordMsgFull[MsgBufManage.FullCount];  //记录在Message队列中哪个地方；
    Smart_Battery::SMBMsgBuf[MsgRecordCount]=SMBMessage;

    MsgBufManage.FullCount++;
    MsgBufManage.EmptyCount--;

    return 1;
}

void 	Smart_Battery::SMBIdleRoutine(void)
{
    m_SMBusStatus=SMB_WAIT;
}
void 	Smart_Battery::SMBWaitRoutine(void)
{
// 	MsTime TimeDiffA = OS_ServeProcess::TimeDiff(m_RecordReadDelayTimeA);
// 	MsTime TimeDiffB = OS_ServeProcess::TimeDiff(m_RecordReadDelayTimeB);
// 	MsTime TimeDiffC = OS_ServeProcess::TimeDiff(m_RecordReadDelayTimeC);
//
// 	if(TimeDiffC.Second >SMBDELAYTIMIE_C)  //根据不同的时间条件判断是否进入读信号流程
// 	{
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeC);
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeB);
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeA);
// 		//m_SMBusStatus=SMB_LOADSIGNAL;
// 		m_ReadDataType=SMB_DATATYPEC;
// 	}
// 	else if(TimeDiffB.Second >SMBDELAYTIMIE_B)
// 	{
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeB);
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeA);
// 		//m_SMBusStatus=SMB_LOADSIGNAL;
// 		m_ReadDataType=SMB_DATATYPEB;
// 	}
// 	else if(TimeDiffA.Us100 >SMBDELAYTIMIE_A)
// 	{
// 		OS_ServeProcess::recordCurTime(&m_RecordReadDelayTimeA);
// 		//m_SMBusStatus=SMB_LOADSIGNAL;
// 		m_ReadDataType=SMB_DATATYPEA;
// 	}
//	if(Smart_Battery::be_Interrupt==true)  //comment out by zc 06/05/2018
//		return;           //comment out by zc 06/05/2018

//  	m_DelayCount++;
//  	if(m_DelayCount>100)  //统一处理，便于测试异常
// 	{
    m_SMBusStatus=SMB_LOADSIGNAL;



//  		m_DelayCount=0;
//  	}


}

St_SMBBMS_ParsedMeta Smart_Battery::FindNextSignal(void)
{
    if(m_FindSignalIdx<m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_WholeSignalNum)
    {
        m_FindSignalIdx++;
        return m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[m_FindSignalIdx-1];
    }
    else
    {
        m_FindSignalIdx=1;
        return m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[m_FindSignalIdx-1];
    }
}
void 	Smart_Battery::SMBLoadSignalRoutine(void)
{
// 	for(unsigned char uc_i=0;uc_i<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount;uc_i++)
// 				m_RefreshInterval[uc_i]++;

    m_CurrentSMBSignal=Smart_Battery::FindNextSignal();
    if(ChannelInfo_Manager::m_SMBusMetaVariable[0][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError==true)
    {
        m_SMBusStatus=SMB_WAIT;
        return;
    }
    //根据配置信息设定刷新标识
//		MsTime TimeDiff; // = OS_ServeProcess::TimeDiff(m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);
//		MsTime TimeDiff = OS_ServeProcess::TimeDiff(m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);
    unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);
//		TimeDiff.Second=OS_ServeProcess::OS_Time.Second-m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta].Second;
//		TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta].Us100;
//	if((TimeDiff.Second*1000/*+TimeDiff.Us100/10*/)<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[m_CurrentSMBSignal.m_SMB_RX_Meta].m_Refresh) //by zc 03.04.2019
    float RefreshInterval;
    if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[m_CurrentSMBSignal.m_SMB_RX_Meta].m_Refresh == FAST)//add by zc 03.04.2019
        RefreshInterval = Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Fast;//add by zc 03.04.2019
//			RefreshInterval = Smart_Battery::m_Refresh_Fast;//add by zc 03.04.2019 for test
    else if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[m_CurrentSMBSignal.m_SMB_RX_Meta].m_Refresh == SLOW)//add by zc 03.04.2019
        RefreshInterval = Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Slow;//add by zc 03.04.2019
//			RefreshInterval = Smart_Battery::m_Refresh_Slow;	//add by zc 03.04.2019 for test
    else if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[m_CurrentSMBSignal.m_SMB_RX_Meta].m_Refresh == ONE_TIME)//add by zc 03.04.2019
    {
        if(FirstRefresh_Flag[m_CurrentSMBSignal.m_SMB_RX_Meta])
            RefreshInterval = 0.98; //WAIT AT LEAST 1 SECOND
        else
        {
            m_SMBusStatus=SMB_WAIT;
            return;
        }
    }
    if(TimeDiff < RefreshInterval * 10000)
//		if((TimeDiff.Second*10000 + TimeDiff.Us100)< RefreshInterval * 10000 )//add by zc 03.04.2019
//	  if((TimeDiff.Second*1000)<RefreshInterval)//add by zc 03.04.2019 for test
    {
        m_SMBusStatus=SMB_WAIT;
        return;
    }
    else
        m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta] = CommonFunc::GetSystemTime(); //OS_ServeProcess::OS_Time;
// 			OS_ServeProcess::recordCurTime(&m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);


// 	if(m_RefreshInterval[m_CurrentSMBSignal.m_SMB_RX_Meta]<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[m_CurrentSMBSignal.m_SMB_RX_Meta].m_Refresh)
// 		return;
// 	else
// 		m_RefreshInterval[m_CurrentSMBSignal.m_SMB_RX_Meta]=0;
	
	m_SMBData.m_Len=m_CurrentSMBSignal.m_DataLen;
	if(m_CurrentSMBSignal.m_DataLen <= (m_CurrentSMBSignal.m_EndByteIndex+1))
    {
		m_CurrentSMBSignal.m_DataLen += 1;	//尾字节为校验字节
	}
    I2CBusNew::RunningBuffer[I2C1].OPMode =I2C_MASTREAD; //I2C_MASTWRITE;
    I2CBusNew::m_I2CBusMgr[I2C1].CtrlByteNum =1; //m_CurrentSMBSignal.m_HeaderBuf.CtrlByteNum;
    I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr =m_CurrentSMBSignal.m_DataLen;//2; //0;
    I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr =m_CurrentSMBSignal.m_HeaderBuf.m_WriteHeader ;
    for(unsigned char uc_i=0; uc_i<m_CurrentSMBSignal.m_HeaderBuf.CtrlByteNum; uc_i++)
        I2CBusNew::m_I2CTaskMgr[I2C1].CurrentAdr[uc_i] = m_CurrentSMBSignal.m_HeaderBuf.m_COMM[uc_i];

    memset(I2CBusNew::RunningBuffer[I2C1].Buf,0,I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr);

    m_SMBData.m_type=m_CurrentSMBSignal.m_DataType;  //09.28.2018
//     m_SMBData.m_Len=m_CurrentSMBSignal.m_DataLen;
    m_SMBData.m_Endian=m_CurrentSMBSignal.m_Endian;
    m_SMBData.m_EndDataIndex=m_CurrentSMBSignal.m_EndDataIndex;
    m_SMBData.m_StartDataIndex=m_CurrentSMBSignal.m_StartDataIndex;
    m_SMBData.m_EndByteIndex=m_CurrentSMBSignal.m_EndByteIndex;
    m_SMBData.m_StartByteIndex=m_CurrentSMBSignal.m_StartByteIndex;
    m_SMBData.m_EndBitIndex=m_CurrentSMBSignal.m_EndBitIndex;
    m_SMBData.m_StartBitIndex=m_CurrentSMBSignal.m_StartBitIndex;

    Smart_Battery::I2CEngine(I2C1,SMB_ONLINE,m_SMBData);


}

void	Smart_Battery::SMBOnlineRoutine(void)
{



    if(m_CurrentSMBSignal.m_DataType==TYPE_STRING || m_CurrentSMBSignal.m_DataType==TYPE_BITS_CONTROL)
        Get_SmartBatteryString(m_CurrentSMBSignal.m_DataLen,m_CurrentSMBSignal,&m_buf[0]);
    else
        Get_SmartBatteryWord(m_CurrentSMBSignal);

    m_SMBData.m_type=m_CurrentSMBSignal.m_DataType;
//     m_SMBData.m_Len=m_CurrentSMBSignal.m_DataLen;
    m_SMBData.m_Endian=m_CurrentSMBSignal.m_Endian;
    m_SMBData.m_EndDataIndex=m_CurrentSMBSignal.m_EndDataIndex;
    m_SMBData.m_StartDataIndex=m_CurrentSMBSignal.m_StartDataIndex;
    m_SMBData.m_EndByteIndex=m_CurrentSMBSignal.m_EndByteIndex;
    m_SMBData.m_StartByteIndex=m_CurrentSMBSignal.m_StartByteIndex;
    m_SMBData.m_EndBitIndex=m_CurrentSMBSignal.m_EndBitIndex;
    m_SMBData.m_StartBitIndex=m_CurrentSMBSignal.m_StartBitIndex;
//		Smart_Battery::I2CEngine(I2C1,SMB_WAIT,m_SMBData);  //18.09.28
    m_SMBusStatus = SMB_WAIT; //18.09.28

}

void 	Smart_Battery::SMBWriteRoutine(void)
{
    I2CBusNew::I2CStop(I2C1);//  20181220yy
    I2CBusNew::RunningBuffer[I2C1].OPMode = I2C_MASTWRITE;
    I2CBusNew::m_I2CBusMgr[I2C1].CtrlByteNum = DoSMBMessage.SMBMessage.m_bufLen;
    I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr = 0;
    I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr =DoSMBMessage.SMBMessage.m_SlaveAddr;
    for(unsigned char uc_i=0; uc_i<DoSMBMessage.SMBMessage.m_bufLen; uc_i++)
        I2CBusNew::m_I2CTaskMgr[I2C1].CurrentAdr[uc_i] =DoSMBMessage.SMBMessage.Send_buf[uc_i];

    unsigned char uc_counter =0;
    do {
        I2CEngine_Exclusive(I2C1);
        if(!Smart_Battery::ErrorFlg)  //  20181220yy
            break;
    } while(++uc_counter<10);


    m_SMBusStatus=SMB_LOADSIGNAL;
}
void 	Smart_Battery::SMBErrorRoutine(void)
{
    OfflineFlg=false;
    //m_SMBusStatus=SMB_LOADSIGNAL;
    m_SMBusStatus=SMB_WAIT;

}
void Smart_Battery::GetSMBMsg(unsigned char MsgIndex)
{
    St_SMBMessage GetMessage;
    GetMessage=SMBMsgBuf[MsgIndex];    // 取得Message响应信息
    DoSMBMessage.SMBMessage=GetMessage;
    MsgBufManage.FullCount=0;   //收到Stop信息取出，其余信息一律不要；
    MsgBufManage.DeletingCount=0;
    MsgBufManage.EmptyCount=7;
}

void Smart_Battery::ClearCurrentSMBMsg(St_DoSMBMessage *Message)
{
    Message->Used=0;
    Message->SMBMessage.MsgNumber=0;
}

Word16 Smart_Battery::Get_SmartBatteryWord(St_SMBBMS_ParsedMeta  m_Signal)
{

// 	Word16 m_getValue;


    if(m_Signal.m_HeaderBuf.m_BeUsed)
    {
        I2CBusNew::RunningBuffer[I2C1].OPMode = I2C_MASTREAD;
        I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr =m_Signal.m_DataLen;
        I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr = m_Signal.m_HeaderBuf.m_ReadAddr;
    }
    else
    {   // 待确认
        I2CBusNew::RunningBuffer[I2C1].OPMode = I2C_MASTREAD;
        I2CBusNew::m_I2CBusMgr[I2C1].CtrlByteNum = m_Signal.m_HeaderBuf.CtrlByteNum;
        I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr =m_Signal.m_DataLen;
        I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr = m_Signal.m_HeaderBuf.m_ReadAddr;
        for(unsigned char uc_i=0; uc_i<m_Signal.m_HeaderBuf.CtrlByteNum; uc_i++)
            I2CBusNew::m_I2CTaskMgr[I2C1].CurrentAdr[uc_i] = m_Signal.m_HeaderBuf.m_COMM[uc_i];

    }

}
void Smart_Battery::Get_SmartBatteryString(unsigned char m_StrLen,St_SMBBMS_ParsedMeta  m_Signal,unsigned char *Str)
{
    if(m_Signal.m_HeaderBuf.m_BeUsed)
    {

        I2CBusNew::RunningBuffer[I2C1].OPMode = I2C_MASTREAD;

        I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr =m_Signal.m_DataLen;
        I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr = m_Signal.m_HeaderBuf.m_ReadAddr;
    }
    else
    {   // 待确认
        I2CBusNew::RunningBuffer[I2C1].OPMode = I2C_MASTREAD;
        I2CBusNew::m_I2CBusMgr[I2C1].CtrlByteNum = m_Signal.m_HeaderBuf.CtrlByteNum;
        I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr =m_Signal.m_DataLen;
        I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr = m_Signal.m_HeaderBuf.m_ReadAddr;
        for(unsigned char uc_i=0; uc_i<m_Signal.m_HeaderBuf.CtrlByteNum; uc_i++)
            I2CBusNew::m_I2CTaskMgr[I2C1].CurrentAdr[uc_i] = m_Signal.m_HeaderBuf.m_COMM[uc_i];
    }

}
// void 	Smart_Battery::Write_SmartBatteryByte(unsigned char Comm,unsigned char m_Len,unsigned char *m_byte)
// {
// 		I2CBusNew::RunningBuffer[I2C1].OPMode=I2C_MASTWRITE;
// 		I2CBusNew::m_I2CBusMgr[I2C1].CtrlByteNum = m_Len;
// 		I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr = 0;  //我们暂时按照控制字节处理写动作即可，暂时用不到块写处理
// 		I2CBusNew::m_I2CBusMgr[I2C1].SlaveAdr = Comm;
// 	for(unsigned char uc_i=0;uc_i<m_Len;uc_i++)
// 	{
// 		I2CBusNew::m_I2CTaskMgr[I2C1].CurrentAdr[uc_i] = *m_byte;
// 		m_byte++;
// 	}
//

// }

unsigned char	Smart_Battery::SMBEngineRoutine(unsigned char uc_ChannelIdx,unsigned char I2Cx,unsigned char m_SMBGotoStatus,St_SMBData m_SMBData)
{
	unsigned char crc_check[68] = {0};
	unsigned char crc_result = 0;

    switch (Smart_Battery::m_SMBEngineStatus)  //分时处理I2C，根据设定需要读出数值或字符串
    {
    case SMBUS_SEND:
        Smart_Battery::I2Ctimeout=0;
        Smart_Battery::ErrorFlg = false;
        I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;//set i2c masterstate flag as idle
        I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
        I2CBusNew::RunningBuffer[I2Cx].BufIdx = 0;
        memset(I2CBusNew::RunningBuffer[I2C1].Buf,0,I2CBusNew::m_I2CTaskMgr[I2C1].SubBufEndingAdr);//  20181220yy
        if ( I2CBusNew::I2CStart(I2Cx) != TRUE )//if do not start,then stop .
        {
            I2CBusNew::I2CStop(I2Cx);
            uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = true;   // yy 20190925
//						Smart_Battery::m_SMBEngineStatus=SMBUS_ERROR;
            m_SMBusStatus=SMB_WAIT;
        }
        else
            Smart_Battery::m_SMBEngineStatus=SMBUS_TRAN;
        break;
    case SMBUS_TRAN:
        if(I2Ctimeout >= 200)  //大于20个周期，约4ms
        {
            I2CBusNew::I2CStop(I2Cx);
            Smart_Battery::I2Ctimeout=0;;
            m_SMBusStatus=SMB_WAIT;
            uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = true;   // yy 20190925
//					Smart_Battery::m_SMBEngineStatus=SMBUS_ERROR; //9:38
            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_NACK)
        {
            // I2CBusNew::I2CStop(I2Cx);
            uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = true;   // yy 20190925
            Smart_Battery::m_SMBEngineStatus=SMBUS_ERROR;

            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_STOPPED && I2CBusNew::m_I2CBusMgr[I2Cx].StopCondition == I2CNormalStop)
        {

            Smart_Battery::m_SMBEngineStatus=SMBUS_FINISH;
            I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;
            break;
        }
        I2Ctimeout++;

        break;
    case SMBUS_FINISH:
    {
        //	ControlPin::SetMeLow(LEDFunc::Pin_HeartBeat);
        m_UpdataFlg[m_CurrentSMBSignal.m_SMB_RX_Meta]=true;  //数据刷新标识
        m_SMBSignalErrorCounter[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta] = 0; //清除读取失败计数器
        CommonFunc::GetTimeTicksUpdate(&m_SMB_OverTime[uc_ChannelIdx].m_UnsafeTime);	//add by zc 12012017
        m_SMB_OverTime[uc_ChannelIdx].m_SMBusStatus = e_SMB_Normal;
        m_SMBusStatus=m_SMBGotoStatus;
        FirstRefresh_Flag[m_CurrentSMBSignal.m_SMB_RX_Meta] = false;
		uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = false;
//         memset( &uc_MVOffline[0],0,MAX_SMB_BMS_SIGNAL);  // yy 20190809
        uc_MVOffline_statuse = false; // Clear one instead of all,Simplified PC Processing.

		//CRC check
		crc_check[0] = m_CurrentSMBSignal.m_HeaderBuf.m_ReadAddr;
		crc_check[1] = m_CurrentSMBSignal.m_HeaderBuf.m_COMM[0];
		crc_check[2] = m_CurrentSMBSignal.m_HeaderBuf.m_ReadAddr + 1;
		memcpy(&crc_check[3],&I2CBusNew::RunningBuffer[I2Cx].Buf[0],m_CurrentSMBSignal.m_EndByteIndex + 1);
			
		crc_result = CommonFunc::CRC8_Normal(&crc_check[0],m_CurrentSMBSignal.m_EndByteIndex + 4);
		if(crc_result != I2CBusNew::RunningBuffer[I2Cx].Buf[m_CurrentSMBSignal.m_EndByteIndex + 1])
		{
			uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = true;
			break;
		}
        switch(m_SMBData.m_type)
        {
        case TYPE_STRING:   //如果为字符串类型  DATATYPE = 2
        {
//					memcpy( m_buf,&I2CBusNew::RunningBuffer[I2C1].Buf[0],m_SMBData.m_Len);  //debug parament
//				  memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_SValue[0],&m_buf[0+m_SMBData.m_StartDataIndex],(m_SMBData.m_EndDataIndex-m_SMBData.m_StartDataIndex ) );//m_SMBData.m_Len);
//				  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_STRING;
//					ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=m_SMBData.m_EndDataIndex-m_SMBData.m_StartDataIndex;
//				  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
            memcpy( m_buf,&I2CBusNew::RunningBuffer[I2C1].Buf[0],m_SMBData.m_Len);  //debug parament
            memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_SValue[0],&m_buf[m_SMBData.m_StartByteIndex],m_SMBData.m_Len);//(m_SMBData.m_EndByteIndex - m_SMBData.m_StartByteIndex + 1 ) );//changed by zc 08.14.2018
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_STRING;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=m_SMBData.m_EndByteIndex-m_SMBData.m_StartByteIndex+1;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
			ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError = false;
        }
        break;
        case TYPE_WORD:  // 如果是WORD类型，
        {
            if(!m_SMBData.m_Endian)  //大小端序处理
            {
                m_SMBData.m_Word =I2CBusNew::RunningBuffer[I2C1].Buf[m_SMBData.m_EndByteIndex];  //debug parament
                m_SMBData.m_Word<<=8;
                m_SMBData.m_Word+=I2CBusNew::RunningBuffer[I2C1].Buf[m_SMBData.m_StartByteIndex];
            }
            else
            {
                m_SMBData.m_Word=I2CBusNew::RunningBuffer[I2C1].Buf[m_SMBData.m_StartByteIndex];
                m_SMBData.m_Word<<=8;
                m_SMBData.m_Word+=I2CBusNew::RunningBuffer[I2C1].Buf[m_SMBData.m_EndByteIndex];
            }

            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_WORD;
            if(m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[m_CurrentSMBSignal.m_SignalID].m_Sign==0)  //unsigned 类型
            {
                if((m_SMBData.m_Word & 0x8000) == 0x8000)
                {
                    short Word;
                    Word = m_SMBData.m_Word & 0x7FFF;
                    ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_fValue[0] = (Word + 32768) * m_CurrentSMBSignal.m_Gain+m_CurrentSMBSignal.m_Offset;
                }
                else
                    ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_fValue[0] = m_SMBData.m_Word*m_CurrentSMBSignal.m_Gain+m_CurrentSMBSignal.m_Offset;
            }
            else//signed 类型
            {
                ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_fValue[0]= m_SMBData.m_Word*m_CurrentSMBSignal.m_Gain+m_CurrentSMBSignal.m_Offset;
            }

//					if( (m_SMBData.m_Word & 0xFFFF) != 0xFFFF)
//						ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_fValue[0]= m_SMBData.m_Word*m_CurrentSMBSignal.m_Gain+m_CurrentSMBSignal.m_Offset;
//					else
//						ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_fValue[0]= 65535*m_CurrentSMBSignal.m_Gain+m_CurrentSMBSignal.m_Offset;

            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError=false;
        }
        break;
        case TYPE_DATE:  //如果是日期类型的数据
        {
            unsigned char tmp;
            memcpy( m_buf,&I2CBusNew::RunningBuffer[I2C1].Buf[0],2);  //debug parament
            if(!m_SMBData.m_Endian)  //大小端序处理
            {
                tmp=m_buf[0];
                m_buf[0]=m_buf[1];
                m_buf[1]=tmp;
            }
            unsigned short m_Dword;
            unsigned long ml_year,ml_month,ml_date;
            unsigned char mS_date[10];
// 					ml_year=(m_buf[0]&0xfe)*256/512+1980;
// 					ml_date=m_buf[1]&0x1f;
// 					ml_month=(m_buf[1]>>5)
            m_Dword=m_buf[0];
            m_Dword=(m_Dword<<8)+m_buf[1];
            ml_year=((m_Dword>>8)&0x00fe)*256/512+1980;
            ml_month=(m_Dword>>5)&0x000f;
            ml_date=m_Dword&0x001f;

            mS_date[0]=ml_year/1000+0x30;
            mS_date[1]=ml_year%1000/100+0x30;
            mS_date[2]=ml_year%100/10+0x30;
            mS_date[3]=ml_year%10+0x30;
            mS_date[4]='.';
            mS_date[5]=ml_month/10+0x30;
            mS_date[6]=ml_month%10+0x30;
            mS_date[7]='.';
            mS_date[8]=ml_date/10+0x30;
            mS_date[9]=ml_date%10+0x30;
            memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_SValue[0],&mS_date[0],10 );//m_SMBData.m_Len);
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_STRING;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=10;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError=false;

        }
        break;
        case TYPE_SIRIALNUMBER:  //如果是序列号类型的数据
        {
            unsigned char tmp;
            memcpy( m_buf,&I2CBusNew::RunningBuffer[I2C1].Buf[0],2);  //debug parament
            if(!m_SMBData.m_Endian)  //大小端序处理
            {
                tmp=m_buf[0];
                m_buf[0]=m_buf[1];
                m_buf[1]=tmp;
            }
            unsigned short m_Dword1;

            m_Dword1=m_buf[0];
            m_Dword1=(m_Dword1<<8)+m_buf[1];
            unsigned char m_sData[5];
            m_sData[0]=m_Dword1/10000+0x30;
            m_sData[1]=m_Dword1%10000/1000+0x30;
            m_sData[2]=m_Dword1%1000/100+0x30;
            m_sData[3]=m_Dword1%100/10+0x30;
            m_sData[4]=m_Dword1%10+0x30;
            memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_SValue[0],&m_sData[0],5);//m_SMBData.m_Len);
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_STRING;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=5;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError=false;
        }
        break;
        case TYPE_BITS_CONTROL:  //SMBus位操作 add by zc 08.14.2018
        {
            memcpy( m_buf,&I2CBusNew::RunningBuffer[I2C1].Buf[0],m_SMBData.m_Len);  //debug parament
//						unsigned char Temp_Buffer[SMB_MAX_BYTE_LENGTH_PER_SIGNAL];
//						memcpy( Temp_Buffer,&I2CBusNew::RunningBuffer[I2C1].Buf[0],m_SMBData.m_Len);  //debug parament
            unsigned int DataLength = m_SMBData.m_EndDataIndex - m_SMBData.m_StartDataIndex + 1;
            if(DataLength > 8)
            {
                for(int i = m_SMBData.m_EndByteIndex ; i > m_SMBData.m_StartByteIndex; i--)
                {
                    m_buf[i] = m_buf[i] >> ( 7 - m_SMBData.m_EndBitIndex);  //ABCDEFGH >>  0000ABCD
                    m_buf[i] = ( m_buf[i-1] << ( m_SMBData.m_EndBitIndex + 1) ) | m_buf[i];
                }
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex] << m_SMBData.m_StartBitIndex;
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex] >> ( 7 + m_SMBData.m_StartBitIndex - m_SMBData.m_EndBitIndex);
            }
            else if(m_SMBData.m_EndByteIndex != m_SMBData.m_StartByteIndex)
            {
                m_buf[m_SMBData.m_EndByteIndex] = m_buf[m_SMBData.m_EndByteIndex] >> ( 7 - m_SMBData.m_EndBitIndex);
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex] << m_SMBData.m_StartBitIndex;
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex]	>> (7 + m_SMBData.m_StartBitIndex - m_SMBData.m_EndBitIndex);
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex] | m_buf[m_SMBData.m_EndByteIndex];
            }
            else
            {
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex] << m_SMBData.m_StartBitIndex;
                m_buf[m_SMBData.m_StartByteIndex] = m_buf[m_SMBData.m_StartByteIndex]	>> (7 + m_SMBData.m_StartBitIndex - m_SMBData.m_EndBitIndex);
            }

            memcpy( &ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Data.m_SValue[0],
                    &m_buf[m_SMBData.m_StartByteIndex],m_SMBData.m_Len);//(m_SMBData.m_EndByteIndex - m_SMBData.m_StartByteIndex + 1) );

            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_type=TYPE_STRING;
            if(m_SMBData.m_EndBitIndex >= m_SMBData.m_StartBitIndex)
            {
                ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=m_SMBData.m_EndByteIndex-m_SMBData.m_StartByteIndex+1;//m_SMBData.m_Len;
            }
            else
            {
                ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_Len=m_SMBData.m_EndByteIndex-m_SMBData.m_StartByteIndex;//m_SMBData.m_Len;
            }
            ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bNeedRecal=true;
			ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError = false;
        }
        break;
        default:
            break;
        }
    }
    break;
    case SMBUS_ERROR:
        m_SMBusStatus=SMB_ERROR;
        uc_MVOffline[m_CurrentSMBSignal.m_SMB_RX_Meta] = true;   // yy 20190809
// 				m_SMBSignalErrorCounter[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta]++;
// 		if(m_SMBSignalErrorCounter[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta]>10)
// 		{
// 			m_SMBSignalErrorCounter[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta]=10;
//		  ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelIdx][m_CurrentSMBSignal.m_SMB_RX_Meta].m_bError=true;  //comment out by zc 12012017
//============Add by zc 12012017=========================================
//  如果用户选择丢失SMB信号不保护，则无需报错
        if(m_SMBCfgBuf.m_buf[uc_ChannelIdx].m_UnsafeTimeout == 0)
        {
            //m_SMB_OverTime[uc_ChannelIdx].m_SMBusStatus = e_SMB_Normal;
            break;
        }
//  如果用户设置保护时间，则如果丢失信号超时就进行保护
        else if(CommonFunc::GetTimeTicks(&m_SMB_OverTime[uc_ChannelIdx].m_UnsafeTime)>= m_SMBCfgBuf.m_buf[uc_ChannelIdx].m_UnsafeTimeout)
        {
            //如果已经为unsafe状态，则无需处理
            if(m_SMB_OverTime[uc_ChannelIdx].m_SMBusStatus == e_BMS_Unsafe)//report unsafe
                break;
            //如果不是unsafe状态，且schedule正在运行，则停止通道，并且报错
            if( ChannelInfo_Manager::ChannelInfo[uc_ChannelIdx].ScheduleStatus== RUNNING)
            {
                Schedule_Manager_task::SendFinishFlag[uc_ChannelIdx].StopStatus = STOP_SMBUS_LOST;
                St_ScheduleMessage SchMessage;
                memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
                SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
                SchMessage.st_status_sch_running.m_btCMD_Source =  ST_SMBPARSE;
                SchMessage.st_status_sch_running.m_bUnsafeType = SMB_UNSAFE_TYPE;
                SchMessage.st_status_sch_running.m_uw32Safe_Code = SMB_LOST_UNSAFE;
//                         SchMessage.st_status_sch_running.m_btLost_Unit = 0;
                ChannelInfo_Manager::ChannelInfo[uc_ChannelIdx].m_UnsafeType = SMB_UNSAFE_TYPE;
                ChannelInfo_Manager::ChannelInfo[uc_ChannelIdx].m_SafetyStatus = SMB_LOST_UNSAFE;
                safety_check::SetUnsafe(uc_ChannelIdx,SMB_LOST_UNSAFE,0, SMB_UNSAFE_TYPE);
                Schedule_Manager_task::UnsafeContent[uc_ChannelIdx]=SchMessage;
                Schedule_Manager_task::SendMsgToSchedule(uc_ChannelIdx,SchMessage);//停止对应的IV通道
                m_SMB_OverTime[uc_ChannelIdx].m_SMBusStatus = e_SMB_Unsafe;    //超过时间未响应则触发unsafe
            }
        }
        else
            m_SMBusStatus=SMB_WAIT;
// //  如果未超时，则尝试重置I2C线路，如果继续连续十次未读取正确数值，则再重置，直到超过保护设定时间后进行unsafe保护
// 			else
// 			{
// //						Smart_Battery::MyInit();
// 						I2CBusNew::I2CInit(I2C1,Smart_Battery::m_SMBCfgBuf.m_buf[0].m_BaudRate); //初始化I2C1线路，保留SMB内数据
// 						SMBus_InitDelayCounter = 0;
// 						for(unsigned char uc_i=0;uc_i<MAXSMBMESSAGE;uc_i++)
// 							m_SMBSignalErrorCounter[0][uc_i]=0;
// 			}
// // 		}
        break;
    default:
        break;
    }


    /*debug for sql test*/
    /*
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][0].m_Data.m_fValue[0]=16640;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][1].m_Data.m_fValue[0]=72;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][2].m_Data.m_fValue[0]=24961;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][3].m_Data.m_fValue[0]=4369;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][4].m_Data.m_fValue[0]=48;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][5].m_Data.m_fValue[0]=-1;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][6].m_Data.m_fValue[0]=1;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][7].m_Data.m_fValue[0]=12.327;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_type=0x02;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Len=0x0A;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[0]='S';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[1]='M';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[2]='P';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[3]='-';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[4]='S';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[5]='D';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[6]='I';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[7]='3';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[8]='.';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][8].m_Data.m_SValue[9]='0';

    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_type=0x02;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Len=0x0D;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[0]='D';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[1]='E';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[2]='L';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[3]='L';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[4]=' ';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[5]='4';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[6]='D';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[7]='M';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[8]='N';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[9]='G';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[10]='3';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[11]='1';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][9].m_Data.m_SValue[12]='N';

    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_type=0x02;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_Len=0x04;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_Data.m_SValue[0]='L';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_Data.m_SValue[1]='I';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_Data.m_SValue[2]='O';
    ChannelInfo_Manager::m_SMBusMetaVariable[0][10].m_Data.m_SValue[3]='N';

    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][11].m_Data.m_fValue[0]=4.108;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][12].m_Data.m_fValue[0]=4.11;

    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_bNeedCalculate=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_bNeedRecal=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_bNeedLog=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_type=0x01;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_Len=0x00;
    ChannelInfo_Manager::m_SMBusMetaVariable[0][13].m_Data.m_fValue[0]=4.109;
    */
    //end for SQL debug
    return 1;
}
unsigned char Smart_Battery::I2CEngine(unsigned char I2Cx,unsigned char m_ReturnStatus,St_SMBData SMBData)
{
    Smart_Battery::m_SMBusStatus=SMB_I2CENGINE;  //调用I2C 后设定返回状态，防止重复操作
    Smart_Battery::m_SMBEngineStatus=SMBUS_SEND;
    Smart_Battery::m_ReturnNextStatus=m_ReturnStatus;
    m_SMBData=SMBData;
    return 1;

}

bool Smart_Battery::Assign_SMBConfig(unsigned char uc_ChannelNo)
{

    int oldidx = m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    //for(unsigned char idx = 0;idx<m_SMBCfgBuf.m_bufCnt;idx++)   //  Smart_Battery::m_SMBCfgBuf.m_buf[]  LOCK 0
    unsigned char idx = 0;
    {
        if(0 == memcmp((void*)&m_SMBAssignMD5[uc_ChannelNo].m_MD5, (void*)&m_SMBCfgBuf.m_buf[idx].m_MD5, 16))//find this config
        {

            if(oldidx!=idx)
            {
                if(oldidx>0)
                {
                    m_SMBCfgBuf.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);
                    m_SMBCfgBuf.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();
                }
                m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx = idx;
                m_SMBCfgBuf.m_buf[idx].m_RegisterBit |= 1<<uc_ChannelNo;
            }
            else
            {
                m_SMBCfgBuf.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();
            }
            Smart_Battery::BeSupportSmbus = true;
            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_WholeSignalNum == 0)
            {
                Smart_Battery::BeSupportSmbus = false;
            }

            return true;
        }
    }
    //
    if(oldidx>=0)
    {
        m_SMBCfgBuf.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);
        m_SMBCfgBuf.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();
    }
    if(m_SMBCfgBuf.m_bufCnt<MAX_LEN_CAN_CONFIG)
    {
        //unsigned char myIdx= m_SMBAssignMD5[uc_ChannelNo].m_insertPos = m_SMBCfgBuf.m_bufCnt++;   // ??????????????
        unsigned char myIdx= m_SMBAssignMD5[uc_ChannelNo].m_insertPos = 0;
        m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx = myIdx;  //
        memcpy(& m_SMBCfgBuf.m_buf[myIdx].m_MD5,&m_SMBAssignMD5[uc_ChannelNo].m_MD5,sizeof(St_MD5)); //???config??MD5
        m_SMBCfgBuf.m_buf[myIdx].m_RegisterBit |= 1<<uc_ChannelNo;  //??
        return false;
    }
    else
    {
        unsigned char oldestIdx = 0;
        for(unsigned char idx = 1; idx<MAX_LEN_CAN_CONFIG; idx++)
        {
            if(m_SMBCfgBuf.m_buf[idx].m_RegisterBit ==0)
                if(memcmp((void*)&m_SMBCfgBuf.m_buf[idx].m_AbondonTime.Second,(void*)&m_SMBCfgBuf.m_buf[oldestIdx].m_AbondonTime.Second,8)<0)
                    oldestIdx = idx;
        }
        m_SMBAssignMD5[uc_ChannelNo].m_insertPos = oldestIdx;
        m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx = oldestIdx;  //
        memcpy(&m_SMBCfgBuf.m_buf[oldestIdx].m_MD5,&m_SMBAssignMD5[uc_ChannelNo].m_MD5,sizeof(St_MD5)); //???config??MD5
        m_SMBCfgBuf.m_buf[oldestIdx].m_RegisterBit |= 1<<uc_ChannelNo;  //??

        return false;
    }


}
bool Smart_Battery::Clear_SMBConfig(unsigned char uc_ChannelNo)
{
    int oldidx = m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    m_SMBCfgBuf.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);
    m_SMBCfgBuf.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();
    m_SMBAssignMD5[uc_ChannelNo].m_cfgBufIdx = -1;

    BeSupportSmbus = false; //180821 zyx Can't clear the flag before. It will lead to Schedule SMB ACK failed after clear smb file.
    return true;
}
unsigned char Smart_Battery::I2CEngine_Exclusive(unsigned char I2Cx)
{

    Smart_Battery::I2Ctimeout=0;
    Smart_Battery::ErrorFlg = false;

    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;//set i2c masterstate flag as idle

    I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
    I2CBusNew::RunningBuffer[I2Cx].BufIdx = 0;

    if ( I2CBusNew::I2CStart(I2Cx) != TRUE )//if do not start,then stop .
    {
        I2CBusNew::I2CStop(I2Cx);
        return ( FALSE );
    }

    for(;;)//while(1)
    {
        if(I2Ctimeout >= I2CMAX_TIMEOUT)
        {
            I2CBusNew::I2CStop(I2Cx);
            Smart_Battery::I2Ctimeout=0;;
            Smart_Battery::ErrorFlg = true;
            Smart_Battery::OfflineCounter++;
            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_NACK)
        {
            I2CBusNew::I2CStop(I2Cx);
            Smart_Battery::ErrorFlg = true;
            Smart_Battery::OfflineCounter++;
            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_STOPPED && I2CBusNew::m_I2CBusMgr[I2Cx].StopCondition == I2CNormalStop)
        {
            Smart_Battery::OfflineCounter=0;
            Smart_Battery::ErrorFlg = false;
            I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;
            return Smart_Battery::ErrorFlg;
        }
        Smart_Battery::I2Ctimeout++;
    }
    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;
    Smart_Battery::ErrorFlg = true;
    Smart_Battery::OfflineCounter++;
    if(OfflineCounter>10)
        OfflineFlg=true;

    return Smart_Battery::ErrorFlg;
}
void Smart_Battery::SetSMBusFirstLog() //add by zc 04/01/2019
{
    if(Smart_Battery::BeSupportSmbus)
    {
        for(unsigned char uc_i=0; uc_i<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount; uc_i++)
        {
            Smart_Battery::FirstRefresh_Flag[uc_i] = true;
            Smart_Battery::FirstLog_Flag[uc_i] = true;
        }
    }
}
void Smart_Battery::SetSMBusLastLog(unsigned char uc_ChannelNo, unsigned char IfLastPoint) //add by zc 04/01/2019
{
    if(Smart_Battery::BeSupportSmbus)
    {
        for(unsigned char uc_i=0; uc_i<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount; uc_i++)
        {
            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Logging_Interval == SYNC_WITH_IV)//zyx 2110091
            {
                Smart_Battery::LastLog_Flag[uc_i] = true;
                if(IfLastPoint == LASTPOINT)
                    Smart_Battery::lastSMBData[uc_i] = true;
            }

            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh == ONE_TIME)
            {
                if(Smart_Battery::FirstLog_Flag[uc_i]) //如果ONE_TIME的Command通过写操作更新过
                    Smart_Battery::LastLog_Flag[uc_i] = true;//add by zc 03.04.2019
            }
        }
        Step_Manager_task::Log_SMBusData(uc_ChannelNo);//add by zc 03.04.2019
    }
}
void Smart_Battery::ResetSMBusFirstLog(unsigned char SMBMessage_Send_buf_0)//add by zc 04/01/2019
{
    for(unsigned char uc_i=0; uc_i<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount; uc_i++)
    {
        if((Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_HeaderBuf.m_COMM[0] & 0xFF) == SMBMessage_Send_buf_0)
        {
            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh == ONE_TIME)
            {
                Smart_Battery::FirstRefresh_Flag[Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_SMB_RX_Meta] = true;
                Smart_Battery::FirstLog_Flag[Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_SMB_RX_Meta] = true;
                Smart_Battery::m_RefreshCounter[0][uc_i] = CommonFunc::GetSystemTime();
            }
        }
    }
}
void Smart_Battery::UploadSMBusData(unsigned char uc_i, ST_SMBDATABUF mSmbBufDataLog, unsigned char DataType)//add by zc 04/01/2019
{
    mSmbBufDataLog.MyRecorderTime = CommonFunc::GetSystemTime();
    if(DataType == SEND_LOG_SMB_DATA)
    {
        if(Smart_Battery::FirstLog_Flag[uc_i])
            Smart_Battery::FirstLog_Flag[uc_i] = false;
//        if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh != ONE_TIME) //by zc 5.21.2019//zyx 2110091
//            mSmbBufDataLog.MyRecorderTime = LastIVtime;
        Smart_Battery::m_PreviousLogUploadTime[uc_i] = mSmbBufDataLog.MyRecorderTime;
    }
    Smart_Battery::m_PreviousIdleUploadTime[uc_i] = mSmbBufDataLog.MyRecorderTime;
    mSmbBufDataLog.MySMBMv_ID= Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_SMB_RX_Meta;//uc_i;
    mSmbBufDataLog.MySMBRecordData=ChannelInfo_Manager::m_SMBusMetaVariable[0][mSmbBufDataLog.MySMBMv_ID];//uc_i
    mSmbBufDataLog.MySMBRecordType = DataType;
    CQ::Enqueue(MetaVariable_Task::VSMB_Handle[0], &mSmbBufDataLog);
}

void Smart_Battery:: MVOffline_SetFlag(void)
{
    unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&MVOffline_IntervalTime);
    if(TimeDiff > SMB_MVOFFLINE_INTERVAL_TIME)  // 5s
    {
        unsigned char uc_success = false;
        unsigned char uc_send = true;
        unsigned long long TimeSet = 0;
        unsigned short int uw16_RX_Meta = 0;
        unsigned short int Scan_i = 0;
        unsigned short int counter = 0;
        for(Scan_i=0; Scan_i < m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_WholeSignalNum; Scan_i ++)
        {
            uw16_RX_Meta = m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[Scan_i].m_SMB_RX_Meta;
            if(uc_MVOffline[uw16_RX_Meta])
                counter ++;

        }
        if(counter>= m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_WholeSignalNum)
        {
            uc_MVOffline_statuse = true;  // Save one instead of all,Simplified PC Processing.
        }

        uw16_RX_Meta = m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[0].m_SMB_RX_Meta;
        Scan_i = 0;
        //  borrow m_SMBLogData to send PC  //CQ::Dequeue(VSMB_Handle[0], &m_SMBLogData);
        MetaVariable_Task::m_SMBLogData.MyRecorderTime = CommonFunc::GetSystemTime();
        MetaVariable_Task::m_SMBLogData.MySMBMv_ID = uw16_RX_Meta;
        MetaVariable_Task::m_SMBLogData.MySMBRecordData=ChannelInfo_Manager::m_SMBusMetaVariable[m_CurrentBufIdx][uw16_RX_Meta];
// 		MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_Len = m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[Scan_i].m_DataLen;
        MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_type = m_SMBCfgBuf.m_buf[m_CurrentBufIdx].m_ValidSignal[Scan_i].m_DataType;
        MetaVariable_Task::m_SMBLogData.MySMBRecordType = SEND_IDLE_SMB_DATA;
        unsigned char uc_SMB = SEND_NONE;
        if(MetaVariable_Task::m_SMBLogData.MySMBRecordData.m_type==TYPE_WORD)
            uc_SMB = SEND_IDLE_SMB_DATA_FLOAT;
        else
            uc_SMB = SEND_IDLE_SMB_DATA_STRING;
        EthernetParse::MPReportData(0,uc_SMB);
        CommonFunc::GetTimeTicksUpdate(&MVOffline_IntervalTime);
    }
}
