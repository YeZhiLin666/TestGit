// *****************************************************
// File Name: CanbusWatchdog.cpp
// Version:        V1.0
// Date:                2015/08/19
// Description:
//              Can通讯看门狗
// Others:
// History：
//
// ******************************************************/

#include "../Entry/includes.h"

UInt16 CAN_WatchdogTask::Priority = PRIORITY4;
UInt16 CAN_WatchdogTask::TaskID = CANWATCHDOG_TASK;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
#if (USE_AS_AUXMCU == 0 )
ST_CANWATCHDOGACK CAN_WatchdogTask::m_CanWatchdogACK;
unsigned char CAN_WatchdogTask::m_CanWatchdogMasterUnitACKStatus;
unsigned char CAN_WatchdogTask::m_CanWatchdogSubMasterUnitACKStatus;
//unsigned char CAN_WatchdogTask::m_CanWatchdogIVUnitACKStatus;
unsigned char CAN_WatchdogTask::m_CanWatchdogSafetyStatus;

bool		  CAN_WatchdogTask::MasterUnit_beNeedWatchdogflg=0;

// bool          CAN_WatchdogTask::IV_beNeedWatchdogflg;

unsigned char CAN_WatchdogTask::m_NextMasterSendSNidx;
// unsigned char CAN_WatchdogTask::m_NextSubmasterAckidx;
// unsigned char CAN_WatchdogTask::m_NextIVSendSNidx;
// unsigned char CAN_WatchdogTask::m_NextAuxAckidx;
unsigned char CAN_WatchdogTask::SubMasterACKRefreshFlg;
bool CAN_WatchdogTask::MasterReSendRefreshFlg;
//unsigned long CAN_WatchdogTask::DelayCounter;   //加入延时计数器，防止频繁的发出狗命令。
MsTime        CAN_WatchdogTask::DelayWaitTime;   //加入延时，防止频繁的发出狗命令。

MsTime   CAN_WatchdogTask::m_MasterUnitAckWaitTime;
// MsTime   CAN_WatchdogTask::m_IVReceiveAuxAckWaitTime;
MsTime   CAN_WatchdogTask::LastSyncTime;  //161028
bool   	 CAN_WatchdogTask::IsSubMasterSetOpen;//161028
ST_CANWATCHDOG_CHANNEL_STATE   CAN_WatchdogTask::st_chan_state[MAXCHANNELNO];
// MsTime        CAN_WatchdogTask::SubUnitOutWatchDogTime[MAXCHANNELNO];
#else
unsigned char   CAN_WatchdogTask::m_CanWatchdogAuxUnitACKstatus;
bool            CAN_WatchdogTask::Aux_beNeedWatchdogflg;
unsigned char   CAN_WatchdogTask::AuxACKRefershFlg;
unsigned char   CAN_WatchdogTask::m_IVUnitID;
unsigned char   CAN_WatchdogTask::m_Ack;

#endif
MsTime   				m_SubMasterWaitACKTime;
bool   SubMaster_WaitNextACKFlg=false;
#pragma arm section
void   CAN_WatchdogTask::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CAN_WATCHDOG]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CAN_WATCHDOG);
#endif

#if (USE_AS_AUXMCU == 0 )
    if(ChannelInfo_Manager::Serial2ParaMod_IsInMasterUnit())//master unint
    {
        ProcessMasterACK();
    }
    if (ChannelInfo_Manager::Serial2ParaMod_IsInSubmasterUnit()) //submaster unint
    {
        ProcessSubMasterACK();
//         SuMasterOutCheck();

    }

// //#if(SUPPORT_AUX==1)
//     if(CommonFunc::uc_Support_AUX)
// 	    ProcessIVAck();

// //#endif
// #else
// 	ProcessAUXAck();
#endif

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CAN_WATCHDOG] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CAN_WATCHDOG);
#endif

}
void   CAN_WatchdogTask:: MyInit(void)
{
#if (USE_AS_AUXMCU == 0 )
    //MasterUnit_beNeedWatchdogflg=0;
    //DelayCounter=0;
// 	IV_beNeedWatchdogflg=0;

    m_NextMasterSendSNidx=0;
// 	m_NextIVSendSNidx=0;
    SubMasterACKRefreshFlg=0;
    MasterReSendRefreshFlg = false;
    m_CanWatchdogMasterUnitACKStatus=CAN_WATCHDOG_MASTERUNIT_IDLE;
    m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN;
// 	m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_IDLE;
    m_CanWatchdogSafetyStatus = eCANBUSWATCHDOG_ERROR_Null;

    memset(&m_CanWatchdogACK,0,sizeof(ST_CANWATCHDOGACK));


// 	m_CanWatchdogACK.m_SubUnitCount=0;
// 	m_CanWatchdogACK.m_SubACKSN=0;
// 	for(unsigned char j=0;j<MAXIVMAPAUXUNIT;j++)
// 	{

// 		m_CanWatchdogACK.m_IVSendSN[j]=0;
// 		m_CanWatchdogACK.m_IVReSendSNCount[j]=0;
// 	}

    for(unsigned char j=0; j<MAXSUBUNIT; j++)
    {
// 		m_CanWatchdogACK.m_MasterReceiveACK[j]=0;
// 		m_CanWatchdogACK.m_MasterReSendCount[j]=0;
// 		m_CanWatchdogACK.m_MasterSendSN[j]=0;
// 		m_CanWatchdogACK.m_SubUnitID[j]=0;
        m_CanWatchdogACK.m_SubUnitLastSyncTime[j] =  GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
    }

    m_CanWatchdogACK.m_bIsSubStatusSync = true;
    IsSubMasterSetOpen = false;//161028
    CommonFunc::GetTimeTicksUpdate(&LastSyncTime);
    CommonFunc::GetTimeTicksUpdate(&DelayWaitTime);

//     LastSyncTime = OS_ServeProcess::OS_Time;
//     DelayWaitTime = OS_ServeProcess::OS_Time;

#else
    Aux_beNeedWatchdogflg=1;
    AuxACKRefershFlg=0;
    m_Ack=0;
    m_CanWatchdogAuxUnitACKstatus=CAN_WATCHDOG_AUXUNIT_IDLE;
#endif

}
#if (USE_AS_AUXMCU == 0 )
void	CAN_WatchdogTask:: ProcessMasterACK(void)
{
    switch(m_CanWatchdogMasterUnitACKStatus)
    {
    case CAN_WATCHDOG_MASTERUNIT_IDLE:
        if (MasterUnit_beNeedWatchdogflg)
        {
            CAN_WatchdogTask:: MyInit();  //20190201 yy
// 			MasterUnit_beNeedWatchdogflg = 0;
            m_CanWatchdogACK.m_SubUnitCount= ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount-1;//减一是去除自己
            for (unsigned char uc_i=0; uc_i<m_CanWatchdogACK.m_SubUnitCount; uc_i++)
            {
                m_CanWatchdogACK.m_SubUnitID[uc_i]=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx;
				m_CanWatchdogACK.m_bIsNeedSubStatusSync[uc_i] = true;
				if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode==eMixMode)
				{
					if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit >= eSeriesBoosterRank_Serial)
						m_CanWatchdogACK.m_bIsNeedSubStatusSync[uc_i] = false;//booster和series不需要同步
				}
            }
            m_CanWatchdogMasterUnitACKStatus=CAN_WATCHDOG_MASTERUNIT_WATIFORSENDACK;//CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT;
        }
        break;
    case CAN_WATCHDOG_MASTERUNIT_WATIFORSENDACK:
        //CAN_WatchdogTask::DelayCounter++;
        //MsTime TimeDiff = OS_ServeProcess::TimeDiff(DelayWaitTime);
        unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&DelayWaitTime);
        //if(CAN_WatchdogTask::DelayCounter>20*UDP_PARALLEL_WATCHDOGRATE)
        if(TimeDiff > 200 * UDP_PARALLEL_WATCHDOGRATE)
        {
            //CAN_WatchdogTask::DelayCounter=0;
            //OS_ServeProcess::recordCurTime(&DelayWaitTime);
            CommonFunc::GetTimeTicksUpdate(&DelayWaitTime);
            m_CanWatchdogMasterUnitACKStatus=CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT;
        }
        break;
    case CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT:
        //m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]=2;
        if(MasterReSendRefreshFlg==false)  //20190201 yy
        {
            m_CanWatchdogACK.m_MasterSendSN[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]]++;
            if (m_CanWatchdogACK.m_MasterSendSN[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]]>=PARALLEL_WATCHDOG_MAX_SN) // 255
            {
                m_CanWatchdogACK.m_MasterSendSN[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]]=0;
            }
        }
        //send to submasterunit;
        MasterOrSubMasterSendACK(m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx],m_CanWatchdogACK.m_MasterSendSN[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]],WATCHDOG_MASTERSEND);
        //OS_ServeProcess::recordCurTime(&m_MasterUnitAckWaitTime);
        CommonFunc::GetTimeTicksUpdate(&m_MasterUnitAckWaitTime);
        m_CanWatchdogMasterUnitACKStatus=CAN_WATCHDOG_MASTERUNIT_WAITSUBUNITACK;
        break;
    case CAN_WATCHDOG_MASTERUNIT_WAITSUBUNITACK:
    {
//			if((OS_ServeProcess::OS_Time.Second - LastSyncTime.Second) > 5)//161028
        if(	m_CanWatchdogACK.m_bIsSubStatusSync == false )
        {
            m_CanWatchdogMasterUnitACKStatus = CAN_WATCHDOG_MASTERUNIT_ACKFAILD;
            m_CanWatchdogSafetyStatus = eCANBUSWATCHDOG_ERROR_StatusSync;
            break;
        }
        unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&m_MasterUnitAckWaitTime);
        //MsTime TimeDiff = OS_ServeProcess::TimeDiff(m_MasterUnitAckWaitTime);
        if(MasterCheckACK(m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]))
        {
            MasterReSendRefreshFlg = false; //20190201 yy
            //CAN_WatchdogTask::DelayCounter = 0;
            //OS_ServeProcess::recordCurTime(&DelayWaitTime);
            CommonFunc::GetTimeTicksUpdate(&DelayWaitTime);
            m_CanWatchdogMasterUnitACKStatus = CAN_WATCHDOG_MASTERUNIT_WATIFORSENDACK; //CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT;
            m_CanWatchdogACK.m_MasterReSendCount[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]] = 0;
            m_NextMasterSendSNidx++;
            if (m_NextMasterSendSNidx >= (m_CanWatchdogACK.m_SubUnitCount)) //减一是为了去除自己
            {
                m_NextMasterSendSNidx = 0;
            }
        }
        if(TimeDiff > (2000 * UDP_PARALLEL_WATCHDOGRATE))
        {
            if(m_CanWatchdogACK.m_MasterReSendCount[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]] >= MAXDRESENDCOUNTER) //20  //200
            {

                m_CanWatchdogMasterUnitACKStatus = CAN_WATCHDOG_MASTERUNIT_ACKFAILD;
                m_CanWatchdogSafetyStatus = eCANBUSWATCHDOG_ERROR_Connection;
                m_CanWatchdogACK.m_MasterReSendCount[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]] = 0;
                unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
                Schedule_Manager_task::UnsafeContent[uMasterChannel].st_status_sch_running.m_btLost_Unit = m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx] + 1;
            }
            else
            {
                m_CanWatchdogACK.m_MasterReSendCount[m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx]]++;
                //CAN_WatchdogTask::DelayCounter = 0;
                //OS_ServeProcess::recordCurTime(&DelayWaitTime);
                CommonFunc::GetTimeTicksUpdate(&DelayWaitTime);
                MasterReSendRefreshFlg = true;  //20190201 yy
                m_CanWatchdogMasterUnitACKStatus = CAN_WATCHDOG_MASTERUNIT_WATIFORSENDACK; //CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT;
            }

        }


        break;
    }
    case CAN_WATCHDOG_MASTERUNIT_ACKFAILD:
    {
        //send unsafe to schedule ;
        St_ScheduleMessage SchMessage;
        memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
        unsigned char uMasterChannel=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
        SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
        SchMessage.st_status_sch_running.m_bUnsafeType  = IV_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = CAN_PARA_ACK_UNSAFE;
        SchMessage.st_status_sch_running.m_btLost_Unit = Schedule_Manager_task::UnsafeContent[uMasterChannel].st_status_sch_running.m_btLost_Unit;
        SchMessage.st_status_sch_running.m_btSubCode = 	m_CanWatchdogSafetyStatus;
        m_CanWatchdogSafetyStatus = eCANBUSWATCHDOG_ERROR_Null;
        ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType = IV_UNSAFE_TYPE;
        ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus = CAN_PARA_ACK_UNSAFE;
        safety_check::SetUnsafe(uMasterChannel,  CAN_PARA_ACK_UNSAFE, 0,IV_UNSAFE_TYPE);
        Schedule_Manager_task::UnsafeContent[uMasterChannel]=SchMessage;
        if(ChannelInfo_Manager::ChannelInfo[uMasterChannel].ScheduleStatus==RUNNING)
            Schedule_Manager_task::SendMsgToSchedule(uMasterChannel,SchMessage);//?????IV??
        CAN_WatchdogTask::IsSubMasterSetOpen = false;//161028
        m_CanWatchdogACK.m_bIsSubStatusSync  = true;
        MasterReSendRefreshFlg = false;  //20190201 yy
        m_NextMasterSendSNidx = 0;
        // 0 send stop CAN pack to Sub Unit
        for(UWord16 uw16_i=0; uw16_i<m_CanWatchdogACK.m_SubUnitCount; uw16_i++)
        {
            m_CanWatchdogACK.m_MasterSendSN[uw16_i]=0; //20190201 yy
            if(ChannelInfo_Manager::ChannelInfo[uMasterChannel].ScheduleStatus==RUNNING)
                MasterOrSubMasterSendACK(m_CanWatchdogACK.m_SubUnitID[uw16_i],PARALLEL_WATCHDOG_ACKFAILD_SN,WATCHDOG_MASTERSEND);
        }
        m_CanWatchdogMasterUnitACKStatus=CAN_WATCHDOG_MASTERUNIT_IDLE;
        break;
    }
    default:
        break;
    }
}

bool    CAN_WatchdogTask::IsSubMasterOpen(void)
{
   //if(ChannelInfo_Manager::ChannelInfo[0].chn_State == LOGIC_PROCESS_CHN_OPEN )
   if(st_chan_state[0].m_bIsChanOpen)  //  // 20191230 yy
        return true;
    else
        return false;
}

bool    CAN_WatchdogTask::IsMasterAndSubMasterSync(void)//161028
{
    if(CommonFunc::TimeDiffInSecond( LastSyncTime.Second) > 2*UDP_PARALLEL_WATCHDOGRATE)//3)
        return false;
    else
        return true;
}


void    CAN_WatchdogTask::ProcessSubMasterACK(void)
{
    switch(m_CanWatchdogSubMasterUnitACKStatus)
    {
    case CAN_WATCHDOG_SUBUNIT_IDLE:
		if(st_chan_state[0].m_bIsChanOpen)  // 从Unit并联从第一通道开始。
		{
			if(CommonFunc::GetTimeTicks(&m_SubMasterWaitACKTime) >= 300000)   // 30s
                m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_ACKFAILD; //20191230 yy
		}
        break;
    case CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN:
        if (SubMasterACKRefreshFlg)
        {
            SubMasterACKRefreshFlg=0;

            if(IsMasterAndSubMasterSync())
                m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_SENDACKTOMASTERUNIT;
            else
            {
				if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode!=eMixMode)//非复杂串并联模式
				{
                MasterOrSubMasterSendACK(m_CanWatchdogACK.m_MasterUnitID,m_CanWatchdogACK.m_SubACKSN,WATCHDOG_SUBSEND);
                m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_ACKFAILD;
				}				
				else if((!ChannelInfo_Manager::Serial2ParaMod_IsBoosterUnit())&&(!ChannelInfo_Manager::Serial2ParaMod_IsSerialUnit()))//复杂串并联slave处理
				{
					 MasterOrSubMasterSendACK(m_CanWatchdogACK.m_MasterUnitID,m_CanWatchdogACK.m_SubACKSN,WATCHDOG_SUBSEND);
					 m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_ACKFAILD;
				}
				else//booster和series 处理
				{
					m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_SENDACKTOMASTERUNIT;
        }
        
            }
// 			SubMaster_WaitNextACKFlg=false;
// 			m_SubMasterWaitACKTime=OS_ServeProcess::OS_Time;
        }
        else if(SubMaster_WaitNextACKFlg==true)
        {
// 			unsigned long long TimeTicks;
// 			MsTime MyMsTime;
// 			MyMsTime.Second = m_SubMasterWaitACKTime.Second;
// 			MyMsTime.Us100 = m_SubMasterWaitACKTime.Us100;
// 			TimeTicks = CommonFunc::GetTimeTicks(&m_SubMasterWaitACKTime);
            if(CommonFunc::GetTimeTicks(&m_SubMasterWaitACKTime) >= 30000 * UDP_PARALLEL_WATCHDOGRATE)//30000)
                m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_ACKFAILD;
        }

        break;
    case CAN_WATCHDOG_SUBUNIT_SENDACKTOMASTERUNIT:

        //send ack to master unit.
        MasterOrSubMasterSendACK(m_CanWatchdogACK.m_MasterUnitID,m_CanWatchdogACK.m_SubACKSN,WATCHDOG_SUBSEND);
        m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN;
        //m_SubMasterWaitACKTime = OS_ServeProcess::OS_Time;
        CommonFunc::GetTimeTicksUpdate(&m_SubMasterWaitACKTime);
        SubMaster_WaitNextACKFlg=true;
        break;
    case CAN_WATCHDOG_SUBUNIT_ACKFAILD:
        unsigned char uc_ChannelNo = 0;
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stGroupStatusInfo.m_bGroupCheckSafety = false;   //subunit's channels use this flag to start sys cfg safety checking
        IR::IRChannelFlg&=~(1<<uc_ChannelNo); //?3y标????  ?'?瓯???DDIR??膀a??3y标??,秉?a???'2??
        Step_Manager_task::PrepareForStop(uc_ChannelNo);
        SubUnitStopChannel(uc_ChannelNo);   // 20191230 yy
        IVSDL::SubmasterStopRoutine(0);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus=0;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus=STEPIDLE;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus=STEPTRANSITION;
        Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));          //??3yD???
        // send stop CAN pack to Master Unit
        unsigned char uc_back = ChannelInfo_Manager::ChannelInfo[0].chn_State; // 备份现场
        ChannelInfo_Manager::ChannelInfo[0].chn_State = LOGIC_PROCESS_CHN_CLOSE;
        MasterOrSubMasterSendACK(m_CanWatchdogACK.m_MasterUnitID,PARALLEL_WATCHDOG_ACKFAILD_SN,WATCHDOG_SUBSEND); //20190201 yy
        ChannelInfo_Manager::ChannelInfo[0].chn_State = uc_back; // 恢复现场
        m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_IDLE;
        //LastSyncTime = OS_ServeProcess::OS_Time; //161028
        CommonFunc::GetTimeTicksUpdate(&LastSyncTime);
		CommonFunc::GetTimeTicksUpdate(&m_SubMasterWaitACKTime);
        break;
    default:
        break;
    }
}

// void 	CAN_WatchdogTask:: MasterSendSN(unsigned char m_SubmasterUnitID,unsigned char m_SN)
// {
// 	MP_CAN_MSG MyMsg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;
// 	MyMsg.Un_CANID.m_CANID.DestID = m_SubmasterUnitID+1;//can id = unint id +1
// 	MyMsg.Un_CANID.m_CANID.SourceID = ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID;// source id 发送时自动+1
// 	MyMsg.Un_CANID.m_CANID.Cmd=CAN_WATCHDOG;
// 	MyMsg.Un_CANID.m_CANID.CmdType=Parallel_CMD_TYPE;
// 	MyMsg.Un_Content.m_WatchdogACK.SN=m_SN;
// 	MyMsg.Un_Content.m_WatchdogACK.SubStatus.IsSubMasterSetOpen = IsSubMasterSetOpen;//161028
// 	MyMsg.Un_Content.m_WatchdogACK.AckType=WATCHDOG_MASTERSEND;

// 	CANBusParse::Send_CAN_Cmd(m_CANType,&MyMsg);
// }

// int 	CAN_WatchdogTask:: SubMasterReceiveSN(MP_CAN_MSG* MyMsg)
// {
// 	m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN;
// 	SubMasterACKRefreshFlg=1;
// 	m_CanWatchdogACK.m_MasterUnitID=MyMsg->Un_CANID.m_CANID.SourceID-1;		//unint id = can id -1
// 	m_CanWatchdogACK.m_SubACKSN=MyMsg->Un_Content.m_WatchdogACK.SN+1;
//
// 	bool bSubMasterOpen = IsSubMasterOpen();
// 	if(bSubMasterOpen == MyMsg->Un_Content.m_WatchdogACK.SubStatus.IsSubMasterSetOpen)
// 		LastSyncTime = OS_ServeProcess::OS_Time;
// 	return 1;

// }


// int 	CAN_WatchdogTask:: SubMasterSendACK(unsigned char m_MasterUnitID,unsigned char m_ACK)
// {
// 	MP_CAN_MSG MyMsg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;
// 	MyMsg.Un_CANID.m_CANID.DestID=m_MasterUnitID+1;
// 	MyMsg.Un_CANID.m_CANID.SourceID = ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID;//source id 发送时自动+1
// 	MyMsg.Un_CANID.m_CANID.CmdType=Parallel_CMD_TYPE;
// 	}
//
// // 	if(IsSubMasterSetOpen == MyMsg->Un_Content.m_WatchdogACK.SubStatus.IsSubMasterOpen)
// // 		LastSyncTime = OS_ServeProcess::OS_Time;
// }

int 	CAN_WatchdogTask:: MasterOrSubMasterSendACK(unsigned char m_MasterUnitID,unsigned char m_ACK,unsigned char AckType)
{
    MP_CAN_MSG MyMsg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;
    MyMsg.Un_CANID.m_CANID.DestID=m_MasterUnitID+1;
    MyMsg.Un_CANID.m_CANID.SourceID = ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID;//source id 发送时自动+1
    MyMsg.Un_CANID.m_CANID.CmdType=Parallel_CMD_TYPE;
    MyMsg.Un_CANID.m_CANID.Cmd=CAN_WATCHDOG;
    MyMsg.Un_Content.m_WatchdogACK.SN=m_ACK;
    MyMsg.Un_Content.m_WatchdogACK.SubStatus.IsSubMasterOpen = IsSubMasterSetOpen; //161028
    MyMsg.Un_Content.m_WatchdogACK.AckType=AckType;
    if(AckType == WATCHDOG_SUBSEND)
        MyMsg.Un_Content.m_WatchdogACK.SubStatus.IsSubMasterOpen = IsSubMasterOpen(); //161028
    CANBusParse::Send_CAN_Cmd(&MyMsg);
    return 1;
}

void 	CAN_WatchdogTask:: MasterOrSuMasterReceiveACK(MP_CAN_MSG* MyMsg)
{
    if(MyMsg->Un_Content.m_WatchdogACK.AckType==WATCHDOG_SUBSEND)
    {
// 		unsigned char mMasterUnitID = MyMsg->Un_CANID.m_CANID.DestID-1;
        unsigned char mSubMasterUnitID =MyMsg->Un_CANID.m_CANID.SourceID -1;//unint id = can id -1
		unsigned char mIdexInGroup = 0;
		for (unsigned char uc_i=0;uc_i<m_CanWatchdogACK.m_SubUnitCount;uc_i++)
			{
				if(m_CanWatchdogACK.m_SubUnitID[uc_i] == mSubMasterUnitID)
				{
					mIdexInGroup = uc_i;
					break;
				}
			}
        m_CanWatchdogACK.m_MasterReceiveACK[mSubMasterUnitID]=MyMsg->Un_Content.m_WatchdogACK.SN;

        if(IsSubMasterSetOpen == MyMsg->Un_Content.m_WatchdogACK.SubStatus.IsSubMasterOpen)
            m_CanWatchdogACK.m_SubUnitLastSyncTime[mSubMasterUnitID] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
        else if((CommonFunc::TimeDiffInSecond( m_CanWatchdogACK.m_SubUnitLastSyncTime[mSubMasterUnitID]) > 5*UDP_PARALLEL_WATCHDOGRATE )
                || (MyMsg->Un_Content.m_WatchdogACK.SN >= PARALLEL_WATCHDOG_ACKFAILD_SN))  //20190201 yy
        {
			if(m_CanWatchdogACK.m_bIsNeedSubStatusSync[mIdexInGroup]==true)
            m_CanWatchdogACK.m_bIsSubStatusSync = false;
            m_CanWatchdogACK.m_SubUnitLastSyncTime[mSubMasterUnitID] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
            unsigned char uMasterChannel=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
            Schedule_Manager_task::UnsafeContent[uMasterChannel].st_status_sch_running.m_btLost_Unit = MyMsg->Un_CANID.m_CANID.SourceID;
        }
    }
    else if(MyMsg->Un_Content.m_WatchdogACK.AckType==WATCHDOG_MASTERSEND)
    {
        m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN;
        SubMasterACKRefreshFlg=1;
        m_CanWatchdogACK.m_MasterUnitID=MyMsg->Un_CANID.m_CANID.SourceID-1;		//unint id = can id -1
        m_CanWatchdogACK.m_SubACKSN=MyMsg->Un_Content.m_WatchdogACK.SN+1;

        bool bSubMasterOpen = IsSubMasterOpen();
        if(bSubMasterOpen == MyMsg->Un_Content.m_WatchdogACK.SubStatus.IsSubMasterSetOpen)
            //LastSyncTime = OS_ServeProcess::OS_Time;
            CommonFunc::GetTimeTicksUpdate(&LastSyncTime);
        else if(MyMsg->Un_Content.m_WatchdogACK.SN >= PARALLEL_WATCHDOG_ACKFAILD_SN)
            m_CanWatchdogSubMasterUnitACKStatus=CAN_WATCHDOG_SUBUNIT_ACKFAILD;   //20190201 yy
    }
}

int		CAN_WatchdogTask::MasterCheckACK(unsigned char m_SubUnitID)
{

    unsigned char ReciveSN = m_CanWatchdogACK.m_MasterReceiveACK[m_SubUnitID];
    unsigned char SendSN = m_CanWatchdogACK.m_MasterSendSN[m_SubUnitID];
    if(ReciveSN==SendSN+1)
    {
        return 1;

    }

    return 0;
}

void CAN_WatchdogTask::SubUnitStopChannel(unsigned char uc_ChannelNo)
{
    St_DisposableMSG    DisposableMSG;
    DisposableMSG.AutoCaliFlag = 0;     //20141011,add by hepeiqing,  incase of causing IBT auto-cali relay mal-function
    unsigned char uIntraLatstChanIdx;
    if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)) || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)))
        uIntraLatstChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1;

    for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
    {
        switch(st_chan_state[uc_idx].m_ucChanCtrl)   //调用相关的关通道时序
        {
        case CCS_I:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr; //&IBT_Base::SetCtrlCloseCh[0];      //临时调用IBT测试以后要统一到OS_LogicProcess::SetCtrlVltToOff_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
            break;
        case CCS_V:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
            break;
        case CCS_REST:
        case CCS_OFF:
        case CCS_OTHER:
        case CCS_EXTERNAL:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlRstToOff.stepLen;
            break;
        default:
            break;
        }
        DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
		DisposableMSG.TargetCtrlType = CT_OFF;  // 20200810 yy
        DisposableMSG.CH_Number = uc_idx;
        OS_LogicProcess::requestLogicPorcess(uc_idx, &DisposableMSG);
    }
}
// void CAN_WatchdogTask::SuMasterOutCheck(void)
// {
//     //MsTime TempTime;
//     unsigned char IsRunnig = 0;
//     if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22013)
//     {
//         IsRunnig = !ControlPin::ReadMe(LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_Prec]);
//     }
//     //TempTime = OS_ServeProcess::TimeDiff(SubUnitOutWatchDogTime[0]);
//     unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&SubUnitOutWatchDogTime[0]);
//     if(IsRunnig == true)
//     {
//         //if(TempTime.Second > 3)
//         if(TimeDiff > 60000)
//         {
//             Schedule_Manager_task::StopChannel(0);
//             //OS_ServeProcess::recordCurTime(&SubUnitOutWatchDogTime[0]);
//             CommonFunc::GetTimeTicksUpdate(&SubUnitOutWatchDogTime[0]);
//         }
//     }
//     else
//     {
//         //OS_ServeProcess::recordCurTime(&SubUnitOutWatchDogTime[0]);
//         CommonFunc::GetTimeTicksUpdate(&SubUnitOutWatchDogTime[0]);
//     }

// }
/*void	CAN_WatchdogTask::ProcessIVAck(void)
{
	static unsigned char m_AuxUnitID;
	switch(m_CanWatchdogIVUnitACKStatus)
	{
	case CAN_WATCHDOG_IVUNIT_IDLE:

		if (IV_beNeedWatchdogflg)
		{
			for(unsigned char j=0;j<MAXIVMAPAUXUNIT;j++)
			{
				m_CanWatchdogACK.m_IVSendSN[j]=0;
				m_CanWatchdogACK.m_IVReSendSNCount[j]=0;
				m_CanWatchdogACK.m_IVChannelMapFlg[j]=0;
			}
//#if(SUPPORT_AUX == 1)
            if(CommonFunc::uc_Support_AUX)
            {
                for (unsigned char uc_i=0;uc_i<MAXCHANNELNO;uc_i++)  //统计IV对应的辅助,UNIT级别；
                {
                    for (unsigned char uc_j=0;uc_j<MAXAUXTYPE;uc_j++)
                    {
                        if (Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Total_Aux_Unit>0)
                        {

                            for (unsigned char uc_k=0;uc_k<Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Total_Aux_Unit;uc_k++)
                            {
                                CAN_WatchdogTask::m_CanWatchdogACK.m_IVChannelMapFlg[Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitID[uc_k]]|=(1<<uc_i);  //辅助unit对应的IV

                                if (m_CanWatchdogACK.m_AuxUnitID[uc_k]==Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitID[uc_k])
                                {
                                    m_CanWatchdogACK.m_AuxUnitCanType[m_CanWatchdogACK.m_AuxUnitID[uc_k]]=Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitCANPort[uc_k];
                                    break;
                                }
                                else
                                {
                                    m_CanWatchdogACK.m_AuxUnitID[uc_k]=Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitID[uc_k];
                                    m_CanWatchdogACK.m_AuxUnitCanType[m_CanWatchdogACK.m_AuxUnitID[uc_k]]=Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j].m_AuxUnitCfg.m_Aux_UnitCANPort[uc_k];
                                    m_CanWatchdogACK.m_AuxUnitCount++;

                                }

                            }
                        }
                    }
                }
            }
//#endif
			m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_SENDTOAUXUNIT;
		}
		break;
	case CAN_WATCHDOG_IVUNIT_SENDTOAUXUNIT:

		m_AuxUnitID=m_CanWatchdogACK.m_AuxUnitID[m_NextIVSendSNidx];

		m_CanWatchdogACK.m_IVSendSN[m_AuxUnitID]++;
		if (m_CanWatchdogACK.m_IVSendSN[m_AuxUnitID]>=255)
		{
			m_CanWatchdogACK.m_IVSendSN[m_AuxUnitID]=0;
		}


		IVSendSNToAux(m_AuxUnitID,m_CanWatchdogACK.m_IVSendSN[m_AuxUnitID],m_CanWatchdogACK.m_AuxUnitCanType[m_AuxUnitID]);
		OS_ServeProcess::recordCurTime(&m_IVReceiveAuxAckWaitTime);
		m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_WAITAUXUNITACK;
		break;

	case CAN_WATCHDOG_IVUNIT_WAITAUXUNITACK:
		{
			//	unsigned char SubUnitID=m_CanWatchdogACK.m_SubUnitID[m_NextMasterSendSNidx];
			MsTime TimeDiff = OS_ServeProcess::TimeDiff(m_IVReceiveAuxAckWaitTime);
			if(IVCheckAuxACK(m_CanWatchdogACK.m_MasterSendSN[m_AuxUnitID],m_CanWatchdogACK.m_AuxUnitID[m_NextIVSendSNidx]))
			{
				m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_SENDTOAUXUNIT;
				m_NextIVSendSNidx++;
				m_CanWatchdogACK.m_IVReSendSNCount[m_AuxUnitID]=0;
				if (m_NextIVSendSNidx>=m_CanWatchdogACK.m_AuxUnitCount)
				{
					m_NextIVSendSNidx=0;
				}
			}
			if(TimeDiff.Us100>200)
			{
				if(m_CanWatchdogACK.m_IVReSendSNCount[m_AuxUnitID]>=3)
				{

					m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_ACKFAILD;
				}
				else
				{
					m_CanWatchdogACK.m_IVReSendSNCount[m_AuxUnitID]++;
					m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_SENDTOAUXUNIT;
				}

			}

			break;
		}
	case CAN_WATCHDOG_IVUNIT_ACKFAILD:
		m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_IDLE;
		CAN_WatchdogTask::IV_beNeedWatchdogflg=0;
		St_ScheduleMessage SchMessage;
		SchMessage.MsgNumber = AUX_ACKFAIL_Msg;
		SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
		SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_ACK_FAILED;
		for (unsigned char uc_i=0;uc_i<MAXCHANNELNO;uc_i++)
		{
			m_CanWatchdogACK.m_IVChannelMapFlg[m_CanWatchdogACK.m_AuxUnitID[m_NextIVSendSNidx]]&=m_CanWatchdogACK.m_IVChannelMapMask;//不运行辅助的通道被屏蔽
			if((m_CanWatchdogACK.m_IVChannelMapFlg[m_CanWatchdogACK.m_AuxUnitID[m_NextIVSendSNidx]]>>uc_i)&1)
			{
				Schedule_Manager_task::UnsafeContent[uc_i]=SchMessage;
				if(ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus==RUNNING)
				Schedule_Manager_task::SendMsgToSchedule(uc_i,SchMessage);
			}
		}

		break;
	default:
		break;
	}
}*/
// int		CAN_WatchdogTask::IVCheckAuxACK(unsigned char m_Sn,unsigned char m_AuxUnitID)
// {
// 	if (m_CanWatchdogACK.m_AuxAck[m_AuxUnitID]==m_CanWatchdogACK.m_IVSendSN[m_AuxUnitID]+1)   //SN累加了
// 	{
// 		return 1;
// 	}

// 	return 0;
// }
// int		CAN_WatchdogTask::IVSendSNToAux(unsigned char m_AuxUnitID,unsigned char m_Sn,unsigned m_cantype)
// {
// 	MP_CAN_MSG MyMsg;
// 	unsigned char m_CANType =m_cantype;
// 	MyMsg.Un_CANID.m_CANID.Cmd=CAN_WATCHDOG;
// 	MyMsg.Un_CANID.m_CANID.DestID=m_AuxUnitID;
// 	MyMsg.Un_CANID.m_CANID.CmdType=AUX_CMD_TYPE;
// 	MyMsg.Un_Content.m_WatchdogACK.SN=m_Sn;
// 	MyMsg.Un_Content.m_WatchdogACK.AckType=WATCHDOG_IVSENDTOAUX;
// 	CANBusParse::Send_CAN_Cmd(m_CANType,&MyMsg);

// 	return 1;

// }
#else
void	CAN_WatchdogTask:: ProcessAUXAck(void)
{
    return;
    // 	switch(m_CanWatchdogAuxUnitACKstatus)
    // 	{
    // 	case CAN_WATCHDOG_AUXUNIT_IDLE:
    // 		if (Aux_beNeedWatchdogflg)
    // 		{
    // 			m_CanWatchdogAuxUnitACKstatus=CAN_WATCHDOG_AUXUNIT_WAITIVUNITSN;
    // 		}

    // 		break;
    // 	case CAN_WATCHDOG_AUXUNIT_WAITIVUNITSN:
    // 		if (AuxACKRefershFlg)
    // 		{

    // 			AuxACKRefershFlg=0;
    // 			m_CanWatchdogAuxUnitACKstatus=CAN_WATCHDOG_AUXUNIT_SENDACKTOIVUNIT;
    // 		}
    //
    // 		break;
    // 	case CAN_WATCHDOG_AUXUNIT_SENDACKTOIVUNIT:
    // 		AuxAckIVSN(m_IVUnitID,m_Ack,CAN2);
    // 	m_CanWatchdogAuxUnitACKstatus=CAN_WATCHDOG_AUXUNIT_WAITIVUNITSN;
    // 		break;
    // 	case CAN_WATCHDOG_AUXUNIT_ACKFAILD:
    // 		break;
    // 	default:
    // 		break;
    // 	}

}
int CAN_WatchdogTask::AuxAckIVSN(unsigned char m_IVUnitID,unsigned char m_ACK,unsigned char m_cantype)
{
    MP_CAN_MSG MyMsg;
// 	unsigned char m_CANType =m_cantype;
    MyMsg.Un_CANID.m_CANID.Cmd=CAN_WATCHDOG;
    MyMsg.Un_CANID.m_CANID.DestID=m_IVUnitID;
    MyMsg.Un_CANID.m_CANID.CmdType=AUX_CMD_TYPE;
    MyMsg.Un_Content.m_WatchdogACK.SN=m_ACK;
    MyMsg.Un_Content.m_WatchdogACK.AckType=WATCHDOG_AUXSEND;
    CANBusParse::Send_CAN_Cmd(&MyMsg);
    return 1;
}
#endif
