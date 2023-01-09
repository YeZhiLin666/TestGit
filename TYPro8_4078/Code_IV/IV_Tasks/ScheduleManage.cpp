// *****************************************************
// File Name: Schedule.cpp
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule管理操作
// Others:
// History：
//
// ******************************************************/
#include "includes.h"

//需要增加内容
//大数据体:
//
//解析: PCASSIGNOK
//

UInt16 Schedule_Manager_task::Priority = PRIORITY4;
UInt16 Schedule_Manager_task::TaskID = SCHEDULE_TASK;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_ScheduleMessage Schedule_Manager_task::ScheduleMessageBuf[MAXMESSAGE][MAXCHANNELNO];  //建立通道Schedule信息队列
St_ScheduleMessage 	Schedule_Manager_task::UnsafeContent[MAXCHANNELNO];    //异常信息内容
St_DoMessage       	Schedule_Manager_task::DoMessageBuf[MAXCHANNELNO];    //建立当前处理的信息缓冲
St_ScheduleMessage  Schedule_Manager_task::StartScheduleUnsafe[MAXCHANNELNO];//200420 zyx
#pragma arm section

bool Schedule_Manager_task::IsNeedSetStopPoint[MAXCHANNELNO];

unsigned char       Schedule_Manager_task::bMetaSyncSend[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::SchTimeTrigger[MAXCHANNELNO];   //计时标记用于超时
St_MsgBufManage    	Schedule_Manager_task::MsgBufManage[MAXCHANNELNO];
MsTime             	Schedule_Manager_task::SysTimeEngineTask[MAXCHANNELNO];
//MsTime 		      	Schedule_Manager_task::SysTimeSendTask[MAXCHANNELNO];
MsTime            	Schedule_Manager_task::MsTimeEngineTask[MAXCHANNELNO];
unsigned short     	Schedule_Manager_task::EngineHandle[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::EngineOverTimeTriger[MAXCHANNELNO];
//unsigned char 	   	Schedule_Manager_task::SendTirgger[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::SendGoStopFlag[MAXCHANNELNO];
StopMsg			      	Schedule_Manager_task::SendFinishFlag[MAXCHANNELNO];
//bool                Schedule_Manager_task::SCH_RESUME_Flag[MAXCHANNELNO];   //RESUME 启动标志  add by qjm 20141016
St_ResumeManage    	Schedule_Manager_task::ResumeManage[MAXCHANNELNO];
unsigned char	   	Schedule_Manager_task::LockModeSwitch[MAXCHANNELNO];   //这个锁暂时保留用于运行时判断状态 0901 dirui
unsigned char		Schedule_Manager_task::IRSpecialTestFlg[MAXCHANNELNO];
unsigned char       Schedule_Manager_task::IRHasBeenRun[MAXCHANNELNO];    //标记IR是否已经被运行过了
//MsTime 			   	Schedule_Manager_task::UnsafeTimeTicksForFixed[MAXCHANNELNO];
//MsTime 			   	Schedule_Manager_task::UnsafeTimeTicksForDynamic[MAXCHANNELNO];
//MsTime 				Schedule_Manager_task::FixedUnsafeMsTime[MAXCHANNELNO];
//MsTime 				Schedule_Manager_task::DynamicUnsafeMsTime[MAXCHANNELNO];

//MsTime 			   	Schedule_Manager_task::SDLTimeTicksForIdle[MAXCHANNELNO];				//显示/闲时 数据发送定时参数
//MsTime 			   	Schedule_Manager_task::SDLTimeTicksForShow[MAXCHANNELNO];
MsTime 		       	Schedule_Manager_task::SDLIdleMsTime[MAXCHANNELNO];
MsTime 		       	Schedule_Manager_task::SDLShowMsTime[MAXCHANNELNO];

unsigned char 		Schedule_Manager_task::IR_UnsafeFlg[MAXCHANNELNO];
unsigned long       Schedule_Manager_task::m_dwTestID[MAXCHANNELNO];  //mits8

//#if SUPPORT_AUX == 1
unsigned char  Schedule_Manager_task::SCH_SendTOAUXStatus[MAXCHANNELNO];
unsigned short Schedule_Manager_task::IV_AUXTYPE[MAXCHANNELNO];
unsigned char  Schedule_Manager_task::bSendedClrLimit[MAXCHANNELNO];
unsigned short Schedule_Manager_task::STEP_AUXLIMITBIT[MAXCHANNELNO];
unsigned char  Schedule_Manager_task::STEP_NEXT[MAXCHANNELNO];
unsigned short Schedule_Manager_task::SendAUX_SafetyFlg[MAXCHANNELNO];
unsigned char  Schedule_Manager_task::SendClearLimitStatus[MAXCHANNELNO];
unsigned long  Schedule_Manager_task::TirggerIDLECounter[MAXCHANNELNO];
// bool  Schedule_Manager_task::b_start2resumeflag[MAXCHANNELNO];     //0: Start 命令，      1： resume 命令
// unsigned char  Schedule_Manager_task::M4WaitACKOvertimeFlag[MAXCHANNELNO]; //add by qjm 20180710
// unsigned char  Schedule_Manager_task::uc_StartOrRestartFlag;
// unsigned char  Schedule_Manager_task::M4ReceiveACKCnt;        //add by qjm 20180803
// unsigned char  Schedule_Manager_task::M4RestartSentCnt;   //受到反馈电压不合格计数器
// MsTime  Schedule_Manager_task::M0FeedBackCommon[MAXCHANNELNO];            // add by qjm 20180710
// unsigned short  Schedule_Manager_task::M4WaitTimeCnt[MAXCHANNELNO];
// unsigned char   Schedule_Manager_task::M4SendComCounter[MAXCHANNELNO];
// unsigned char   Schedule_Manager_task::M4SendStartStatus[MAXCHANNELNO];
// bool            Schedule_Manager_task::Receive_M0_StartedACKFlag;
// unsigned short  Schedule_Manager_task::M4WaitPacketCnt[MAXCHANNELNO];
// unsigned char   Schedule_Manager_task::M4SendPacketCounter[MAXCHANNELNO];
// unsigned long  Schedule_Manager_task::BuiltInSDL_TriggerCounter[MAXCHANNELNO];
unsigned short Schedule_Manager_task::M_SysALarmEmergencyFlg=0;
// unsigned short test_SN1[20];
// unsigned short test_Count[20];
// unsigned short test_type[20];
GlobalDeviceSwitch Schedule_Manager_task::TemperatureChamberSwitch[MAXMCUUNIT];  //changed by zc 04.25.2018

unsigned long m_counter=0;
float m_backupMvVoltage;
//#endif
//extern int Dbg_IVChn;
/*****************************************************
// Function Name: MyInit
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule 任务初始化函数
// Others:
//                //任务初始化
//
// ******************************************************/
void Schedule_Manager_task::MyInit(void)
{
    unsigned char uc_Channel;
    unsigned char uc_i;
    for(uc_Channel = 0; uc_Channel < MAXCHANNELNO; uc_Channel++)
    {
        for(uc_i = 0; uc_i < MAXMESSAGE; uc_i++)
        {
            memset(&ScheduleMessageBuf[uc_i][uc_Channel], 0, sizeof(St_ScheduleMessage));
            MsgBufManage[uc_Channel].RecordMsgEmpty[uc_i] = uc_i; //{0,1,2,3,4,5,6,7}
            MsgBufManage[uc_Channel].RecordMsgFull[uc_i] = 0;
            ResumeManage[uc_Channel].m_ResumeStatus = 0;
        }
        ClearScheduleMessage(&DoMessageBuf[uc_Channel]);
        ChannelInfo_Manager::ChannelInfo[uc_Channel].ScheduleStatus = NOTREADY;
        SendGoStopFlag[uc_Channel] = 0;
        SendFinishFlag[uc_Channel].StopStatus = 0;
        SendFinishFlag[uc_Channel].SubStopCode = 0;
        MsgBufManage[uc_Channel].FullCount = 0;
        // MsgBufManage[uc_Channel].DeletingCount=0;
        MsgBufManage[uc_Channel].EmptyCount = EMPTYCOUNT;
        SchTimeTrigger[uc_Channel] = 0;
        bMetaSyncSend[uc_Channel] = 0;
// 		M4SendComCounter[uc_Channel] = 0;
        //		SCH_RESUME_Flag[uc_Channel] = 0;

//#if SUPPORT_AUX == 1
        STEP_AUXLIMITBIT[uc_Channel] = 0;
        SendAUX_SafetyFlg[uc_Channel] = 0;
// 		Step_Manager_task::AuxLimitAckMgr[uc_Channel].bNeedWattingAck=0;
        for(unsigned char  uc_j = 0; uc_j < 2; uc_j++)
            CANBusParse::st_SNCheckTab[uc_Channel][uc_j].m_SNCount = 0;

        ChannelInfo_Manager::m_ChannelIRJudge[uc_Channel] = 0;
        IR_UnsafeFlg[uc_Channel] = 0;
        IRHasBeenRun[uc_Channel] = 0;
        TirggerIDLECounter[uc_Channel] = 0;
        ChannelInfo_Manager::ChannelSchedule[uc_Channel].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck=0;
    }
    //初始化温控箱为关闭状态  changed by zc 04/25/2018
    for(int auxUnitIdx = 0; auxUnitIdx < MAXMCUUNIT; auxUnitIdx++)
    {
        for(int auxChnNo = 0; auxChnNo < MAXMTCCHAMBERNO; auxChnNo++)
        {
            Schedule_Manager_task::CloseTemperatureChamber( auxUnitIdx, auxChnNo );
            Schedule_Manager_task::RelsaseTemperatureChamber( auxUnitIdx, auxChnNo );
        }
    }
//#endif

    //初始化相关标志位
// 	M4RestartSentCnt =0;
// 	M4ReceiveACKCnt =0;

}
/*****************************************************
// Function Name: DoWork
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule 任务执行函数
// Others:
//                处理过程：分配时间片，提取Msg信息，运行Schedule状态机
// History：
//
// ******************************************************/
void Schedule_Manager_task::DoWork(void)  //150us
{
    unsigned char uc_channel;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SCHEDULE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_SCHEDULE);
#endif

    for(uc_channel = 0; uc_channel < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++) //通道循环处理Message
    {
        //        1）遍历信息队列，一次取出全部的Message信息；
        //        2）信息要有预处理过程，因为可能队列中会有几条信息；
        //        3）状态机是一直在运行的；无论有无信息，它不应被message左右；

        //	#if (SUPPORT_PARALLEL == 1)
        //if ((ChannelInfo_Manager::IsSlaveChannel(uc_channel)) || (ChannelInfo_Manager::IsInterUnitSubmasterChannel(uc_channel)))
        PreprocessMsg(uc_channel);
        if ( (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()) && (! ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_channel)))
        {
            //PreprocessMsg(uc_channel);  dirui del 2017/12/18
            PreprocessSlaveUnsafeMsg(uc_channel);
            //设定从通道滤波参数  dirui 2020/12/21
            unsigned char m_GroupMastereID=ChannelInfo_Manager::FindMasterChanIdx(uc_channel);
            ChannelInfo_Manager::SetFilterStatus(uc_channel,ChannelInfo_Manager::GetFilterStatus(m_GroupMastereID));
            continue;
        }
        //	#endif  //support_parallel
        //信息预处理过程

        //PreprocessMsg(uc_channel); dirui del 2017/12/18

        //从上面的信息处理过程得到的操作信息存放于DoMessageBuf，执行状态机处理，如果没有信息也继续执行状态机
        ProcessState(uc_channel);


    }
#if SOFTWARE_DOG_ENABLE == 1
    SoftWare_WatchDog();  //为下位没有应答情况处理服务  0829 dirui 设定时间60s可修改参数 测试代码屏蔽不影响运行
#endif

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SCHEDULE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_SCHEDULE);
#endif
}

void Schedule_Manager_task::PreprocessSlaveUnsafeMsg(unsigned char uc_ChannelNo)
{
    // 	 unsigned char MessageNumber=DoMessageBuf[uc_ChannelNo].ScheduleMessage.MsgNumber;
    // 	 switch(MessageNumber)
    // 	 {
    // 		case Sch_OnUnsafe_Msg:
    // 		//	SchOnUnsafeMsgRoutine(uc_ChannelNo);
    // 	//	  EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = PC_Unsafe_Msg;
    // 	//    EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
    // 		 //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
    // 			break;
    // 		case Sch_OnSafe_Msg:
    // 			//
    // 		   //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
    // 			break;
    // 		default:
    // 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
    // 			 break;
    // 	 }

    ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
}
/*****************************************************
// Function Name: PreprocessorMsg
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule 信息预处理
// Others:
//信息预处理部分则是提取自身Message队列中信息进行处理的过程。
//这部分内容分成两部分，第一是信息过滤，不合法或者过时的消息将被过滤掉；
//第二步是信息合并及处理，即将可能出现的多条信息进行仲裁，
//将最终胜出的信息交由本任务（Schedule管理任务）的状态机进行相应处理。
// History：
//
// ******************************************************/
int Schedule_Manager_task::PreprocessMsg(unsigned char uc_ChannelNo)   //以下程序暂调试试用，最终要按优先级顺序处理而不是简单的直接清除；
{
    //    unsigned char 	Idx_First;
    unsigned char 	Idx_Msg;
    unsigned char 	MsgIndex;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];

    if(MsgBufManage[uc_ChannelNo].FullCount == 0)
        return -1;        //无信息

    //首先要判断信息是否合法;
    for(int Idx_Full = 0; Idx_Full < MsgBufManage[uc_ChannelNo].FullCount; Idx_Full++)
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Full];
        switch(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].MsgNumber)
        {
        case Sch_JumpA_Msg:
            if(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].FromStepID != ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)
                DeleteScheduleMsg(uc_ChannelNo, Idx_Full);
            if(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].StepID > ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_ItemCount.m_u16StepCount)
                DeleteScheduleMsg(uc_ChannelNo, Idx_Full); //删除信息缓冲区中这条信息
            break;
        default:
            break;
        }
    }
    //以下优先级扫描
    //优先级顺序： 非排:   |        0        |      1     |     2     |     2       |      2      |      3       |      3       |     3      |
    //                     | Sch_OnUnsafe_Msg|Sch_Stop_Msg|Sch_Run_Msg|Sch_Pause_Msg|Sch_JumpM_Msg|Sch_Resume_Msg|Sch_Assign_Msg|Sch_JumpAMsg|
    //             排序：  |                                                     4                                                                                                                                                        |
    //                     |Sch_LimitArrive_Msg|Sch_LimitPropertyChange_Msg|Sch_LimitStatusChanged|Sch_Started_Msg|Sch_Stopped_Msg|Sch_OnSafe_Msg|

    if(p_DoMesBuf->Used == 1)          //没有Sch_Stop_Msg那么要根据Message是否处理完来决定下一步操作
        return 1;            //Message没有处理完，不去取新信息

    if (MsgBufManage[uc_ChannelNo].FullCount == 1) //只有一条
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount - 1];
        p_DoMesBuf->ScheduleMessage = ScheduleMessageBuf[MsgIndex][uc_ChannelNo];
        p_DoMesBuf->Used = 1;
        //MsgBufManage[uc_ChannelNo].FullCount=0;  // 清空队列
        //MsgBufManage[uc_ChannelNo].EmptyCount=EMPTYCOUNT;//MAXMESSAGE;
        DeleteScheduleMsg(uc_ChannelNo,0);  // 20190302 hp
        return 1;
    }

    //more than one msg:
    unsigned char Idx_Use    = 0;
    unsigned char HiPriority = ScheduleMessageBuf[MsgBufManage[uc_ChannelNo].RecordMsgFull[0]][uc_ChannelNo].MsgPriority; //get first msg's priority

    for(unsigned char Idx_Full = 1; Idx_Full < MsgBufManage[uc_ChannelNo].FullCount; Idx_Full++)
    {
        Idx_Msg = MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Full];
        if(ScheduleMessageBuf[Idx_Msg][uc_ChannelNo].MsgPriority < HiPriority)
        {   //msg with higher priority
            HiPriority = ScheduleMessageBuf[Idx_Msg][uc_ChannelNo].MsgPriority;
            Idx_Use = Idx_Full;
        }
    }//get the highest priority msg here

    p_DoMesBuf->ScheduleMessage = ScheduleMessageBuf[MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Use]][uc_ChannelNo];
    p_DoMesBuf->Used = 1; //use the msg with highest priority

    DeleteScheduleMsg(uc_ChannelNo, Idx_Use); //删除取出完毕的消息

    //删除优先级不为4的信息
    for(unsigned char Idx_Full = 0; Idx_Full < MsgBufManage[uc_ChannelNo].FullCount; Idx_Full++)
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Full];
        if(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].MsgPriority < 4)
            DeleteScheduleMsg(uc_ChannelNo, Idx_Full);
    }

    return 1;
}


/*****************************************************
// Function Name: ProcessState
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule 状态机处理
// Others:
//                        Schedule状态：
//        NOTREADY / IDLE / START / RUNNING / GOPAUSE / RESUME / GOSTOP
// History：
//
// *****************************************************/
void Schedule_Manager_task::ProcessState(unsigned char uc_ChannelNo)
{
    //对于当前的各种状态进行处理，执行状态处理过程，响应命令，转换状态
    unsigned char uc_SchStatus = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus;               //当前Schedule的状态
    unsigned char MessageNumber = DoMessageBuf[uc_ChannelNo].ScheduleMessage.MsgNumber;      //取得当前响应信息编号

    St_ChannelInfo			*		p_Info	=		&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    St_ChannelStep			*		p_Step	=		&ChannelInfo_Manager::ChannelStep[uc_ChannelNo];
    St_ChannelSchedule		*		p_Sch	=		&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo];
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];

    switch(uc_SchStatus)
    {
    case NOTREADY:
// 		SystemDelayTask::beSystemDelayFlg=true;
// 		SystemDelayTask::TriggerDelay=1;
// 		SystemDelayTask::m_DelayTime=15;
        //NOTREADY 里面目前只执行闲事数据上发
        //hepeiqing add 20130828, idle data should available in not ready statusmachine,in case the
        SendSDLShowData(uc_ChannelNo);  //闲时数据一秒上发测试
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
        break;
    case IDLE:              //IDLE响应Sch_Assign_Msg；Sch_Run_Msg    ；    Sch_Resume_Msg；Sch_OnUnsafe_Msg；
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:

            Schedule_Manager_task::M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            safety_check::ClearUnsafeIfNotRunning(uc_ChannelNo);
            //CANBusParse::IV_SendTOSysCtrl_BackToNormal();
            break;
        case Sch_Run_Msg:
        {
            Smart_Battery::SetSMBusFirstLog(); //add by zc 04/01/2019
            SchRunMsgRoutine(uc_ChannelNo);
        }
        break;
        case Sch_Resume_Msg:
        {
            Smart_Battery::SetSMBusFirstLog(); //add by zc 04/01/2019
            SchResumeMsgRoutine(uc_ChannelNo);
        }
        break;
        case Sch_OnUnsafe_Msg:
            // SchOnUnsafeMsgRoutine(uc_ChannelNo);   //Add by DKQ on 09_09_2013 for not reporting unsfate to PC
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); // Add by DKQ on 09_09_2013
            p_Info->ScheduleStatus = DYNAMICUNSAFE;
            break;
        default:
            SendSDLShowData(uc_ChannelNo);  //闲时数据一秒上发测试
            ScheduleToStopIdle(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
            break;
        }
        break;
// 	case	WAITPowerReady:
// 		switch(MessageNumber)
//     {
// 			case Sch_Stop_Msg:
// 			     SchStopMsgRoutine(uc_ChannelNo);
// 			     SchTimeTrigger[uc_ChannelNo]=0;  //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
// 			     break;
// 			case Sch_OnUnsafe_Msg:
// 				   p_Info->m_Event_Source=S_START_ONUNSAFEMSG;
// 			     SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal信息处理完后要转入IDLE状态
// 			     p_Info->ScheduleStatus=FIXEDUNSAFE;
// 			    SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

// 					//SchReportMessageToPC(uc_ChannelNo,PC_StopRpt_Msg, &UnsafeContent[uc_ChannelNo]);
// 			     break;
// // 				case Sch_JumpM_Msg:           //暂时不考牵笮胰鸫?
// // 					 if(CommonFunc::uc_Support_AUX)
// // 			        ClearAUXLimits(uc_ChannelNo,0);
// 			default:
// 				  SchWaitRoutine(uc_ChannelNo);
// 			    break;
//     }
//
//         break;
    case START:            //START响应Sch_Stop_Msg；Sch_Pause_Msg；Sch_JumpM_Msg；Sch_LimitPropertyChange_Msg；Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            SchStopMsgRoutine(uc_ChannelNo);
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
        //                 case Sch_Pause_Msg:        //完成无信息输入部分处理时，不发送Msg给Step.Mgr任务，同时转入GOPAUSE状态，而不是RUNNING状态
        //                         ChannelInfo_Manager::LoadNextStep(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID); //step装载到副本，调大数据体接口
        //                         p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
        //                         p_Info->ScheduleStatus=GOPAUSE;
        // 						#if SUPPORT_AUX==1
        // 						ClearAUXLimits(uc_ChannelNo,0);
        // 						#endif
        //                         //信息要排队所以不能清除信息
        //                         break;
        case Sch_JumpM_Msg:
            //更新所要跳步的信息到目前成员变量，并重新进行无信息输入时的工作。
            //由于这个瞬态只需要一个时钟周期进行处理，所以实际上对于无信息输入部分并无影响。
//#if SUPPORT_AUX==1
            if(CommonFunc::uc_Support_AUX)
                ClearAUXLimits(uc_ChannelNo, 0);
//#endif

            // 以下的部分如何做到支持辅助 和不支持辅助时的分离。像下面这样处理可以吗？ 05.08.2014  DKQ
            //这个地方是用于判断有辅助状态下，是否执行了给辅助发命令，并且是否等待辅助应答，没有辅助配置的话要执行不对辅助应答状态判断正常运行的操作

            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  // PC_Switching_Msg信息必需先存跳转信息
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
            p_DoMesBuf->ScheduleMessage.GotoStepID =  p_DoMesBuf->ScheduleMessage.StepID;
            p_Info->m_Event_Source = S_START_JUMPMMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            p_Sch->m_RunningStepIdx = p_DoMesBuf->ScheduleMessage.StepID;
            ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_Sch->m_RunningStepIdx); //step装载到副本，调大数据体接口

            // #endif
            //IR异常判断
            if(IRspecial_Judge(uc_ChannelNo))
            {
                ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_Sch->m_ItemCount.m_u16StepCount); //step装载IR测试特殊步，调大数据体接口
                IRSpecialTestFlg[uc_ChannelNo] = 0;
            }
            p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
            p_Info->ScheduleStatus = RUNNING;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //清除信息
            break;
// 		case Sch_LimitPropertyChange_Msg:
// 			//完成无信息输入部分处理时，在发送Msg前，按需求更新Step副本中相应Limit的属性。
// 			ChannelInfo_Manager::LoadNextStep(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID); //step装载到副本，调大数据体接口
// 			p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
// 			//ChannelInfo_Manager::Init_Metavariable(uc_ChannelNo);             //MV初始化，调大数据体接口
// 			//根据Parse下发的信息遍历当前副本的LimitID，更新相对应的属性；
// 			//p_Step->.m_Info[1-ActiveIdx].m_Data.m_Limits[].m_ucIfCanSelfJump=DoMessageBuf.ScheduleMessage.LimitPropertyChange;    //更新大数据体Step副本中本limit是否可以独立跳步状态
// 			p_Info->ScheduleStatus=RUNNING;
// 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //清除信息
// 			break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_START_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal信息处理完后要转入IDLE状态
            p_Info->ScheduleStatus = FIXEDUNSAFE; //Add by DKQ 09_09_2013
            SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);
            break;
        default:
            safety_check::InitAbnormalCheck(uc_ChannelNo);
            //<<<LATER>>>  comment out chen safety sch value
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_CURRENT, &safety_check::Check_S[uc_ChannelNo].checkSCH_I);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_VOLTAGE, &safety_check::Check_S[uc_ChannelNo].checkSCH_V);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_POWER, &safety_check::Check_S[uc_ChannelNo].checkSCH_P);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_CAPACITY, &safety_check::Check_S[uc_ChannelNo].checkSCH_C);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_STEPTIME, &safety_check::Check_S[uc_ChannelNo].checkSCH_t);

            MetaVariable_Task::InitDvdt(uc_ChannelNo);////1901042 zyx
            if (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22013)
            {
                ControlPin::SetMeHigh(LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_IVSD]);
                if(m_counter>1000)
                {
                    m_counter=0;
                    float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
                    if(abs(m_backupMvVoltage - ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage)>(V_High*0.01))
                    {
                        p_Info->m_Event_Source=S_START_ONUNSAFEMSG;

                        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
                        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_START_ONUNSAFEMSG;
                        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = IV_CABLE_CONNECTION_UNSAFE;
                        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = IV_UNSAFE_TYPE;
                        safety_check::SetUnsafe(uc_ChannelNo,IV_CABLE_CONNECTION_UNSAFE,0, IV_UNSAFE_TYPE);
                        SchOnUnsafeMsgRoutine(uc_ChannelNo);
                        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_UNSAFE;

                        SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

                        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;
                        ControlPin::SetMeLow(LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_IVSD]);
                        return;
                    }
                }
                else
                {
                    m_counter++;
                    break;
                }
                ControlPin::SetMeLow(LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_IVSD]);
            }
            SchStartRoutine(uc_ChannelNo);                     //START瞬态响应处理
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
            break;
        }
        break;
//#if SUPPORT_AUX == 1
    case WAITFORAUX:
        switch(MessageNumber)
        {
        case Sch_JumpM_Msg:
            //	#if SUPPORT_AUX==1
            ClearAUXLimits(uc_ChannelNo, 0);

            //		#endif
            //p_Info->m_Event_Source=S_RUNNING_JUMPMMSG;
            SchJumpMMsgRoutine(uc_ChannelNo);
            break;
        case Sch_Stop_Msg:
            SchStopMsgRoutine(uc_ChannelNo);
            break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_START_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal信息处理完后要转入IDLE状态
            p_Info->ScheduleStatus = GOSTOP;//FIXEDUNSAFE; //Add by DKQ 09_09_2013
            SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
        case Sch_Started_Msg:
            //p_Info->m_Event_Source=S_RUNNING_STARTEDMSG;
            SchStartedMsgRoutine(uc_ChannelNo); //不能跳出，继续执行下面。
        default:
            SchWaitForAuxRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);

            if(p_DoMesBuf->ScheduleMessage.MsgNumber == Sch_LimitArrive_Msg)  //解决首步带辅助时
                return;

            break;
        }
        break;
//#endif

    case RUNNING:    //RUNNING响应Sch_Stop_Msg；Sch_Pause_Msg；Sch_JumpA_Msg；Sch_JumpM_Msg；
        //Sch_LimitArrive_Msg；Sch_LimitPropertyChange_Msg；Sch_LimitStatusChange_Msg；Sch_Started_Msg；Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            p_Info->ScheduleStatus = GOSTOP;
            Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0;

            //chen add 20131017
            if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_POWER_OFF)  //dirui del 2017/12/18
            {
// 				//PowerOFF 处理
            }
            else
            {
                SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;
            }
            ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, LAST_POINT); //180309 zyx

            IVSDL::LimitArrivedRoutine(uc_ChannelNo);
            EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; // 2013/10/24 yang  修改关键点记录数据

            Smart_Battery::SetSMBusLastLog(uc_ChannelNo, LASTPOINT);//add by zc 04/01/2019
            Step_Manager_task::SendLog(uc_ChannelNo);
            //Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);

            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
        case Sch_Continue_Msg:
        {
            unsigned char activeStepIdx = p_Step->m_ucActiveIdx;
            if( p_Step->m_Info[activeStepIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE )
            {
                Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0;
                p_Info->m_Event_Source = S_RUNNING_JUMPMMSG;
                SchContinueMsgRoutine(uc_ChannelNo);
            }
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);
            break;
        }
        case Sch_Pause_Msg:
            p_Info->ScheduleStatus = GOPAUSE;
            break;
        case Sch_JumpA_Msg:
            //p_Info->m_Event_Source=S_RUNNING_JUMPAMSG;
            ChannelInfo_Manager::Copy_PVmeta2LSmeta(uc_ChannelNo);
            SchJumpAMsgRoutine(uc_ChannelNo);
            break;
        case Sch_JumpM_Msg:
            ChannelInfo_Manager::Copy_PVmeta2LSmeta(uc_ChannelNo);
            Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0;
            ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, LAST_POINT); //180515 zyx
            SchJumpMMsgRoutine(uc_ChannelNo);
            if(p_Sch->m_Settings.m_AbnormalCheck.IIR_IsNeedCheck)
            {
                if(IRspecial_Judge(uc_ChannelNo))
                {
                    IRspecial_Judge_Process(uc_ChannelNo);//zyx 2105291
                }
            }
            break;
//			}
        case Sch_LimitArrive_Msg:
//            ChannelInfo_Manager::Copy_PVmeta2LSmeta(uc_ChannelNo);
            SchLimitArriveMsgRoutine(uc_ChannelNo);
            break;
// 		case Sch_LimitPropertyChange_Msg:
// 			//SchLimitPropertyChangeMsgRoutine(uc_ChannelNo);
// 			break;
        case Sch_LimitStatusChange_Msg:
            //p_Info->m_Event_Source=S_RUNNING_LIMITSTATUSCHANGEMSG;
            SchLimitStatusChangeMsgRoutine(uc_ChannelNo);
            break;
        case Sch_Started_Msg:
            //p_Info->m_Event_Source=S_RUNNING_STARTEDMSG;
// 		  PowerAdj::SchRunningFlag[uc_ChannelNo]= true;
            SchStartedMsgRoutine(uc_ChannelNo);
            break;
        case Sch_OnUnsafe_Msg:
            CANBusParse::IV_SendTOSysCtrl_Abnormal();
            p_Info->m_Event_Source = S_RUNNING_ONUNSAGEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP; //Abnormal处理过程完成后要进入GOSTOP状态
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
//#if SUPPORT_AUX == 1
        case AUX_Unsafe_Msg:
            AuxOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0;
            break;
        case AUX_ACKFAIL_Msg:
            AuxOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0;
            break;
//#endif
        default:
            SendSDLShowData(uc_ChannelNo); //显示数据3秒上传测试
            SchRunningRoutine(uc_ChannelNo);               //RUNNING瞬态响应处理
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //清除信息操作
            break;
        }
        break;
    case GOPAUSE:    //GOPAUSE响应Sch_Stop_Msg；Sch_Stopped_Msg；Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            Schedule_Manager_task::M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
        case Sch_Stopped_Msg:
            p_Info->m_Event_Source = S_GOPAUSE_STOPPEDMSG;
            SchStoppedMsgRoutine(uc_ChannelNo);
            break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_GOPAUSE_ONUNSAGEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP; //Abnormal处理过程完成后要进入GOSTOP状态
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
        default:
            SchGopauseRoutine(uc_ChannelNo);                     //GOPAUSE瞬态响应处理
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除信息操作
            break;
        }
        break;
    case RESUME:    //RESUME响应Sch_Stop_Msg；Sch_LimitPropertyChange_Msg；Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
// 		case Sch_LimitPropertyChange_Msg:  //进行"无信息输入"处置后，更改Step.Mgr的Step副本中相应Limit的属性；
// 			SchResumeRoutine(uc_ChannelNo);                 //无信息输入处理
// 			//根据Parse下发的信息遍历当前副本的LimitID，更新相对应的属性；
// 			//    p_Step->.m_Info[1-ActiveIdx].m_Data.m_Limits.m_ucIfCanSelfJump=DoMessageBuf.ScheduleMessage.LimitPropertyChange;    //更新大数据体Step副本中本limit是否可以独立跳步状态
// 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
// 			break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_RESUME_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP; //Abnormal处理过程完成后要进入GOSTOP状态
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            break;
//#if SUPPORT_AUX == 1
        case AUX_Unsafe_Msg:
            AuxOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0;
            break;
//#endif
        default:
            safety_check::InitAbnormalCheck(uc_ChannelNo);
            //<<<LATER>>>  comment out chen safety sch value
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_CURRENT, &safety_check::Check_S[uc_ChannelNo].checkSCH_I);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_VOLTAGE, &safety_check::Check_S[uc_ChannelNo].checkSCH_V);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_POWER, &safety_check::Check_S[uc_ChannelNo].checkSCH_P);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_CAPACITY, &safety_check::Check_S[uc_ChannelNo].checkSCH_C);
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, MP_SCH_SAFTY_TYPE_STEPTIME, &safety_check::Check_S[uc_ChannelNo].checkSCH_t);
            SchResumeRoutine(uc_ChannelNo);                 //RESUME瞬态响应处理
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
        }
        break;
    case GOSTOP:    //GOSTOP响应Sch_Stopped_Msg；Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stopped_Msg:
            p_Info->m_Event_Source = S_GOSTOP_STOPPEDMSG;
// 			Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
            SchStoppedMsgRoutine(uc_ChannelNo);
            break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_GOSTOP_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            break;
        default:
            SchGoStopRoutine(uc_ChannelNo);                //GOSTOP瞬态响应处理
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
        }
//#if(SUPPORT_AUX == 1)
        if(CommonFunc::uc_Support_AUX)
        {
            if(Step_Manager_task:: bExtChargeFlag[uc_ChannelNo])
            {
                Step_Manager_task:: bExtChargeFlag[uc_ChannelNo] = 0;
                CANBusParse::IV_SendExtChargeOut(uc_ChannelNo, 0);
            }
        }
//#endif
        if(Schedule_Manager_task::IsNeedSetStopPoint[uc_ChannelNo])
        {
            Schedule_Manager_task::IsNeedSetStopPoint[uc_ChannelNo] = false;
            Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
        }
        p_Info->m_SimuInfo.m_bValid = 0;
        IVSDL::ScheduleEndedRoutine(uc_ChannelNo);
        break;
    case FIXEDUNSAFE:
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            p_Info->ScheduleStatus = IDLE;
            CANBusParse::IV_SendTOSysCtrl_BackToNormal();
            Schedule_Manager_task::M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            Schedule_Manager_task::LockModeSwitch[uc_ChannelNo]=0;  //dirui add 2015/07/15
            safety_check::ClearUnsafeIfNotRunning(uc_ChannelNo);

            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_StopRpt_Msg;    //Add by DKQ 09.09.2013
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_MANUAL;		//Add by DKQ 09.09.2013
            //p_Info->m_Event_Source=S_FIXEDUNSAFE_STOPMSG;
            //EthernetParse::m_ReportStatus.m_btSCHEvent_Source = p_Info->m_Event_Source;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_MANUAL;
            p_Info->m_Event_Source = S_FIXEDUNSAFE_STOPMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_StopRpt_Msg, &UnsafeContent[uc_ChannelNo]);

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;  // Add by DKQ 09.09.2013
            break;
        case Sch_OnSafe_Msg:
            p_Info->ScheduleStatus = IDLE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
        default:
            SendSDLShowData(uc_ChannelNo);  //安全状态下加入闲时数据上发处理 0905 dirui
            Sch_FixedUnsafeRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
        }
        break;
    case DYNAMICUNSAFE:
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            p_Info->ScheduleStatus = IDLE;
            M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            LockModeSwitch[uc_ChannelNo]=0;  //dirui add 2015/07/15
            safety_check::ClearUnsafeIfNotRunning(uc_ChannelNo);

            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_StopRpt_Msg;    //Add by DKQ 09.09.2013
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_MANUAL;		//Add by DKQ 09.09.2013
            //p_Info->m_Event_Source=S_FIXEDUNSAFE_STOPMSG;
            //EthernetParse::m_ReportStatus.m_btSCHEvent_Source = p_Info->m_Event_Source;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_MANUAL;
            p_Info->m_Event_Source = S_FIXEDUNSAFE_STOPMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_StopRpt_Msg, &UnsafeContent[uc_ChannelNo]);

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;  // Add by DKQ 09.09.2013
            break;
        case Sch_OnSafe_Msg:
            p_Info->ScheduleStatus = IDLE;
            thirdParty::UnsafeLockSwitch[uc_ChannelNo] = 0;  //add by qjm20150808
            safety_check::Process_SM_Ptr[uc_ChannelNo] = SAFE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
				case Sch_Resume_Msg:
        case Sch_Run_Msg:
        {
            //加入钳位处理内容
            // 									IR::DAC_Flag=1;
            // 									board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,p_Info->m_fVclampHigh),ACTIVE_DAC_VH);
            // 									board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,p_Info->m_fVclampLow),ACTIVE_DAC_VL);
            // 									IR::DAC_Flag=0;
            unsigned long Code = ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, p_Info->m_fVclampHigh);
            Step_Manager_task::StepDAC_Out(uc_ChannelNo, Code, ACTIVE_DAC_VH);
            Code = ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, p_Info->m_fVclampLow);
            Step_Manager_task::StepDAC_Out(uc_ChannelNo, Code, ACTIVE_DAC_VL);

            //直接执行报错处理,直接调用安全处理函数    0904 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //设定回复信息为Abnormal信息
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = p_Info->m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
            //p_Info->m_Event_Source=S_DYNAMICUNSAGE_RUNMSG;
            //EthernetParse::m_ReportStatus.m_btSCHEvent_Source = p_Info->m_Event_Source;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
            p_Info->m_Event_Source = S_DYNAMICUNSAGE_RUNMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
            p_Info->ScheduleStatus = FIXEDUNSAFE; //Add by DKQ 09_09_2013
            break;
        }
        default:
            SendSDLShowData(uc_ChannelNo);  //安全状态下加入闲时数据上发处理 0905 dirui
            //	Sch_DynamicUnsafeRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
            break;
        }
        break;
    default:
        //break;
        return;
    }
    ClearScheduleMessage(p_DoMesBuf);  //清除信息操作
}


/*****************************************************
// Function Name: SchAssignMsgRoutineWithNotReady
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Assign_Msg信息响应过程处理(NOTREADY)状态响应下
// Others:
//                        任意模式（锁定/非锁定）下，寻找相应Schedule，Simulation，Pulse，
//                        如失败则发回失败的MD5码给PC，成功则返回成功信息给PC，将所涉及的
//                        Schedule，Simulation，Pulse标志为被Assigned（被占用），并同时将
//                        Schedule，Simulation与Pulse的Index号在通道上登记。（Simulation
//                        与Pulse需要建立简易MD5码-Index对照表），同时进入IDLE状态。
// transitional information:
//                        PC_AssignRpt_Msg
// History：暂不使用
//
// ******************************************************/
// void Schedule_Manager_task::SchAssignMsgRoutineWithNOTREADY(unsigned char uc_ChannelNo)
// {
//     memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD=PC_AssignRpt_Msg;                  //设定回复信息为 AssignRpt信息
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
// 	EthernetParse::m_st_Assign = & DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign;
//
// 	if(ChannelInfo_Manager::FindScheduleBuf(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5)>=0)
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_u16ChannelIdx == uc_ChannelNo)
//         {
//             for(UChar8 uc_i = 0; uc_i < DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_ucMD5Count; uc_i++)
//                 DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS;  //  暂时调试填写
//         }
//
// 		if(SendFinishFlag[uc_ChannelNo].StopStatus==STOP_UNSAFE)  //若上来就报了SystemUnsafe
// 		    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=DYNAMICUNSAFE;
// 		else
// 		    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;                   //处理成功转入IDLE状态
//
//     }
//     else         //Schedule不合法情况处理
//     {
//
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
//         EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOAD_SCH_FAIL; // ERROR_ASSIGN_FAIL;
//     }
//     EthernetParse::MPReportData(uc_ChannelNo,SEND_REPORT_ASSIGN);   //向PC发送回复信息调用Parse相关程序
//     ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
// }
/*****************************************************
// Function Name: SchAssignMsgRoutineWithIDLE
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Assign_Msg信息响应过程处理(IDLE)状态响应下
// Others:
//                    任意状态（锁定/非锁定）下，寻找相应Schedule，Simulation，Pulse，如失败则发回失败的MD5码给PC，成功则返回成功信息给PC，
//                    清除目前被Assign的Schedule，Simulation，与Pulse上面的被占用标志，将新指定的Schedule，Simulation，Pulse标志为被Assigned（被占用），
//                    并同时将Schedule，Simulation与Pulse的Index号在通道上登记。（Simulation与Pulse需要建立简易MD5码-Index对照表）。
// transitional information:
//                        PC_AssignRpt_Msg
// History：暂不使用
//
// ******************************************************/
// void Schedule_Manager_task::SchAssignMsgRoutineWithIDLE(unsigned char uc_ChannelNo)
// {
//     memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD = PC_AssignRpt_Msg;                    //设定回复信息为 AssignRpt信息
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
//     EthernetParse::m_st_Assign = & DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign;
//
//     if (ChannelInfo_Manager::RegisterSchedule(&DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign))   //寻找相应的Schedule   大数据体中实现
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_u16ChannelIdx == uc_ChannelNo)
//         {
//             for(UChar8 uc_i = 0; uc_i < DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_ucMD5Count; uc_i++)
//                 DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS; //  暂时调试填写
//         }
//         ChannelInfo_Manager::UnregisterSchedule(uc_ChannelNo);                         //成功后要注册新的Schedule(设定标志位，在通道上登记Index号); 老的标记位清除； 大数据体中实现
//         ChannelInfo_Manager::RegisterSchedule(&DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign);
//     }
//     else       //Schedule不合法情况处理
//     {
//
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
//         EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOAD_SCH_FAIL; // ERROR_ASSIGN_FAIL;
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = NOTREADY;
//     }
//
//     EthernetParse::MPReportData(uc_ChannelNo,SEND_REPORT_ASSIGN);   //向PC发送回复信息调用Parse相关程序
//
//     ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
// }

/*****************************************************
// Function Name: SchAssign&StarRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         针对Assign+Start接口
// Others:
//                   Parse根据接口返回的内容回复PC
// transitional information:
//
// History：
//
// ******************************************************/
unsigned char Schedule_Manager_task::SchAssignAddStarRoutine(St_Assign * AssignMsg)
{
    St_ScheduleMessage   SchMessage;
    unsigned char uc_ChannelNo = AssignMsg->m_u16ChannelIdx;
    unsigned char *Status = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus;

    //     if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==RUNNING)
    // 	{
    // 		return SCHEDULERUNNING;           //已经指派，要保持,向PC报告StartFail，内容为SCHEDULERUNNING
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==DYNAMICUNSAFE)
    // 	{
    // 		SchMessage.MsgNumber = Sch_Run_Msg;  //此命令可以驱动Schedule上报unsafe（从二信道） dirui 0904
    // 		Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,SchMessage); // 发出SCh命令信息
    // 		return SCHEDULEUNSAGE;           //保护了，不动作，向PC报告StartFail，内容为SCHEDULEUNSAGE
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==NOTREADY)
    // 	{
    // 		if(SchAssignRoutine(uc_ChannelNo,AssignMsg)==1)
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;
    // 			return ASSIGNOK;   //向PC报告ASSIGNOK 同时给Schedule发送Start信息
    // 		}
    // 		else
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=NOTREADY;
    // 			return ASSIGNFAIL;  //向PC报告ASSIGNFAIL
    // 		}
    // 	}
    // 	else if((ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==IDLE))
    // 	{
    // 		if(SchAssignRoutine(uc_ChannelNo,AssignMsg)==1)
    // 			return ASSIGNOK;   //向PC报告ASSIGNOK 同时给Schedule发送Start信息
    // 		else
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=NOTREADY;
    // 			return ASSIGNFAIL;  //向PC报告ASSIGNFAIL
    // 		}
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==FIXEDUNSAFE)  //20130831 hepeiqing add,a case unhandled
    // 	{
    // 		SchMessage.MsgNumber = Sch_Run_Msg;  //此命令可以驱动Schedule上报unsafe（从二信道） dirui 0904
    // 		Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,SchMessage); // 发出SCh命令信息
    // 		return SCHEDULEUNSAGE;
    // 	}
    // 	//else  //20130831 hepeiqing add,Dirui: pls make sure that all cases are handled!
    // 	return ERROR;    //什么也没有处理的情况下返回 ERROR状态 0901 dirui

    switch(*Status)
    {
    case RUNNING:
        return SCHEDULERUNNING;           //已经指派，要保持,向PC报告StartFail，内容为SCHEDULERUNNING
    case DYNAMICUNSAFE:
    case FIXEDUNSAFE:
//         SchMessage.MsgNumber = Sch_Run_Msg;  //此命令可以驱动Schedule上报unsafe（从二信道） dirui 0904
//         Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage); // 发出SCh命令信息
        return SCHEDULEUNSAGE;           //保护了，不动作，向PC报告StartFail，内容为SCHEDULEUNSAGE
    case NOTREADY:
    case IDLE:
        if(SchAssignRoutine(AssignMsg) == 1)
        {
            *Status = IDLE;
            return ASSIGNOK;   //向PC报告ASSIGNOK 同时给Schedule发送Start信息
        }
        else
        {
            *Status = NOTREADY;
            return ASSIGNFAIL;  //向PC报告ASSIGNFAIL
        }
    default:
        return ERROR;    //什么也没有处理的情况下返回 ERROR状态 0901 dirui
    }

}
/*****************************************************
// Function Name: SchAssignRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         加载Schedule过程
// Others:
//
// transitional information:
//
// History：
//
// ******************************************************/
int Schedule_Manager_task::SchAssignRoutine(St_Assign * AssignMsg)
{
    //unsigned char uc_ChannelNo = AssignMsg->m_u16ChannelIdx;
    if (ChannelInfo_Manager::RegisterSchedule(AssignMsg))   //寻找相应的Schedule   大数据体中实现
    {
        //if(AssignMsg->m_u16ChannelIdx == uc_ChannelNo)
        //{
        for(UChar8 uc_i = 0; uc_i < AssignMsg->m_ucMD5Count; uc_i++)
            AssignMsg->m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS;
        //}
        //ChannelInfo_Manager::UnregisterSchedule(uc_ChannelNo);                         //成功后要注册新的Schedule(设定标志位，在通道上登记Index号); 老的标记位清除； 大数据体中实现
        //ChannelInfo_Manager::RegisterSchedule(AssignMsg);
        return 1;
    }
    else       //Schedule不合法情况处理
    {
        return -1;
    }
}
/*****************************************************
// Function Name: SchOnUnsafeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_OnUnsafe_Msg信息响应过程处理
// Others:
//                        进入锁定模式
// transitional information:
//                        PC_Abnormal_Msg
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchOnUnsafeMsgRoutine(unsigned char uc_ChannelNo)
{

    //EthernetParse::m_ReportStatus.m_st_RunningStatus赋值要放在前面，否则有被清零现象
    //陈东20130828
    //UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;
//     memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));

//     EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //设定回复信息为Abnormal信息
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType;
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
//     UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                   //20131023 dirui 清除冗余的发送
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
}
/*****************************************************
// Function Name: SchOnSafeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_OnSafe_Msg信息响应过程处理
// Others:
//                        解除锁定模式
// transitional information:
//                        PC_BackToNormal_Msg
// History：
//
// ******************************************************/
// void Schedule_Manager_task::SchOnSafeMsgRoutine(unsigned char uc_ChannelNo)
// {
//     //UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;  // 08/28/2013  陈东 解决安全报警问题
//     //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     //EthernetParse::m_ReportStatus.m_st_RunningStatus = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running;
//     //EthernetParse::m_ReportStatus.m_btCMD=PC_BackToNormal_Msg;    //设定回复信息为BackToNormal信息
// 	   //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;
//     //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);   //向PC发送回复信息调用Parse相关程序
//
//     UnsafeContent[uc_ChannelNo].st_status_sch_running = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running;
//     ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_FIXEDUNSAFE_STOPMSG;
//     SchReportMessageToPC(uc_ChannelNo,PC_BackToNormal_Msg, &UnsafeContent[uc_ChannelNo]);
//
//     //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
// }
/*****************************************************
// Function Name: SchRunMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Run_Msg信息响应过程处理
// Others:
//                        非锁定模式下响应。转入START状态
// transitional information:
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchRunMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_LastOutput_1B = 0;   //170307 add by hpq
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_fLastErr_1B = 0;     //170307 add by hpq
    ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = 0.0f;        //170307 add by hpq
    m_backupMvVoltage=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
    if(LockModeSwitch[uc_ChannelNo] == 1)  //还在处于安全状态    0902 dirui
    {
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //设定回复信息为Abnormal信息
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_IDLE_RUNMSG_LOCK;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);


        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_IDLE_RUNMSG_LOCK;
        SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = DYNAMICUNSAFE; //dirui add 2015/07/15
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
        return;
    }
// 	if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	{
// 	    b_start2resumeflag[uc_ChannelNo]=0;
//         PowerAdjNeedCheck(uc_ChannelNo,0);
// 	}
// 	else
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=START;   //非锁定模式下转入START状态
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  //上位机告诉我从哪一步开始，我就从哪一步开始
    //加入钳位处理
    // 	if(board_config::DacInfo.m_bHaveClampVdac)  //引擎中已经处理，所以不再需要
    // 	{
    // 	    IR::DAC_Flag=1;
    // 	    board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh),ACTIVE_DAC_VH);
    // 	    board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow),ACTIVE_DAC_VL);
    //         IR::DAC_Flag=0;
    // 	}
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui 在Schedule结束之后清除AUX的安全信息
        ClearAUXLimits(uc_ChannelNo, 0);
    }
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
}
/*****************************************************
// Function Name: SchResumeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Resume_Msg信息响应过程处理
// Others:
//                        非锁定模式下响应。转入RESUME状态
// transitional information:
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchResumeMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_LastOutput_1B = 0;   //170307 add by hpq
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_fLastErr_1B = 0;     //170307 add by hpq
    ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = 0.0f;        //170307 add by hpq
    if(LockModeSwitch[uc_ChannelNo] == 1)  //还在处于安全状态    0902 dirui
    {
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //设定回复信息为Abnormal信息
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_IDLE_RESUMEMSG_LOCK;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_IDLE_RESUMEMSG_LOCK;
        SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
        return;
    }

    //    SCH_RESUME_Flag[uc_ChannelNo] = 1;
// 	 if( ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	 {
// 		b_start2resumeflag[uc_ChannelNo]=1;
// 		PowerAdjNeedCheck(uc_ChannelNo,1);
// 	 }
// 	 else
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=RESUME;     //非锁定模式下转入RESUME状态
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  //上位机告诉我从哪一步开始，我就从哪一步开始

    //加入钳位处理
    if(board_config::DacInfo.m_bHaveClampVdac)
    {
        IR::DAC_Flag = 1;
        board_config::DAC_out(uc_ChannelNo, ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh), ACTIVE_DAC_VH);
        board_config::DAC_out(uc_ChannelNo, ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow), ACTIVE_DAC_VL);
        IR::DAC_Flag = 0;
    }
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ClearAUXLimits(uc_ChannelNo, 0);
        ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui 在Schedule结束之后清除AUX的安全信息
    }

    if(Smart_Battery::BeSupportSmbus)
    {
        MetaVariable_Task::My_SMBLog_Status=SMBLOG_WAIT;

    }
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                                      //当前信息处理完毕要清除
}

/*****************************************************
// Function Name: SchStopMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Stop_Msg信息响应过程处理
// Others:
//                        转入GOSTOP状态
// transitional information:
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchStopMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
    SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                                      //当前信息处理完毕要清除
}

/*****************************************************
// Function Name: SchJumpAMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_JumpA_Msg信息响应过程处理
// Others:
//                先判断Sch.MD5、ToStepID和FromStepID是否合法，如合法，
//                且Step.Mgr任务并不在调用一次性时序引擎时，响应此命令：
//                装载所需的Step到Step.Mgr任务的副本中，按需求初始化（重置）
//                所需MV，发出Msg给Step.Mgr任务来运行此Step
//
// transitional information:
//                Step_JumpA_Msg
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchJumpAMsgRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;
    unsigned short ToStepID;
    unsigned short FromStepID;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];

//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
        ClearAUXLimits(uc_ChannelNo, 0);
//#endif

    ToStepID = p_DoMesBuf->ScheduleMessage.StepID;
    FromStepID = p_DoMesBuf->ScheduleMessage.FromStepID;

//    if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo, p_DoMesBuf->ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))  //检查Schedule的MD5码；  大数据体中实现
//    {
    if(FromStepID == ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)    //从大数据体中取出Step_ID，判断FromStepID是否合法
    {
        if(ToStepID < ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_ItemCount.m_u16StepCount) //判断Tostep是否合法
        {
            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 0 ) //检查Step任务是否在调用一次性时序引擎
            {
                //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
                //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;				//跳步命令准备跳步前发送JUMP
                //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
                //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = ToStepID;  // PC_Switching_Msg信息必需先存跳转信息
                //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
                p_DoMesBuf->ScheduleMessage.GotoStepID = ToStepID;
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_JUMPAMSG;
                SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);

                ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ToStepID); //装载所需的Step到副本中
                ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = ToStepID;
//					RecordLastStepData(uc_ChannelNo);	//20130723记录上一步骤的数据
                StepMessage.MsgNumber = Step_JumpA_Msg;
                Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);
                //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);
            }
        }
    }
//    }
}
/*****************************************************
// Function Name: SchJumpMMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_JumpM_Msg信息响应过程处理
// Others:
//                判断Sch.MD5是否合法，如合法，判断Step.Mgr任务是否在调用一次性时序引擎，
//                如正在调用引擎，则保留此命令到调用完毕，再响应此命令，否则立刻响应：
//                装载所需的Step到Step.Mgr任务的副本中，按需求初始化（重置）所需MV，发出Msg给
//                Step.Mgr任务来运行此Step
// transitional information:
//                Step_JumpM_Msg
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchJumpMMsgRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
        ClearAUXLimits(uc_ChannelNo, 0);
//#endif

//#if(SUPPORT_AUX == 1)//qjm20150901
    if(CommonFunc::uc_Support_AUX)
    {
        if(Step_Manager_task:: bExtChargeFlag[uc_ChannelNo])
        {
            Step_Manager_task:: bExtChargeFlag[uc_ChannelNo] = 0;
            CANBusParse::IV_SendExtChargeOut(uc_ChannelNo, 0);
        }
    }
//#endif
    if(Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVARIABLE\
            && Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVALUE)  //mits8
        EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_SDL_CYCLE); // 20180202 yy

    SendMetaSync(uc_ChannelNo);
    Schedule_Manager_task::bMetaSyncSend[uc_ChannelNo] = 0;//220607 zyx

    //ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;

//    if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo, DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))  //寻找相应的Schedule ；  大数据体中实现
//    {
    if( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 1) //检查Step任务是否在调用一次性时序引擎   可以在大数据体的Channel_Info中查找
        return;    //调用引擎过程中保留此命令
    else
    {
        EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; // 2013/10/24 yang  修改关键点记录数据
        unsigned char activeStepIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        if( ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[activeStepIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_PAUSE )
        {
            Step_Manager_task::SendLog(uc_ChannelNo);   //last step's last log point,it should be send before step jump event,by hepeiqing 20140926
            if(Smart_Battery::BeSupportSmbus)//add by zc 03.04.2019
            {
                Step_Manager_task::Log_SMBusData(uc_ChannelNo);//add by zc 03.04.2019
            }
        }

        //161019 zyx
        IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_DISPLAY, LOG_NORMAL_DATA_POINT);
        ChannelInfo_Manager::ClearPV_CV_StageTime(uc_ChannelNo);
        ChannelInfo_Manager::ClearPV_CV_StageCurrent(uc_ChannelNo);
        //161019 zyx
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;    	//接到手工跳步，跳步之前发送JUMP
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  // PC_Switching_Msg信息必需先存跳转信息
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].Report_LimitID = 65535;   //20140212 add by hepeiqing,this is a special Limit,Manual Jump
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        p_DoMesBuf->ScheduleMessage.GotoStepID = p_DoMesBuf->ScheduleMessage.StepID;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_JUMPMMSG;
        SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = p_DoMesBuf->ScheduleMessage.StepID;
        float f_CycleIndex = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;
        if(f_CycleIndex != p_DoMesBuf->ScheduleMessage.CycleID)
        {
            EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_SDL_CYCLE);
            ChannelInfo_Manager::Copy_PVmeta2LCmeta(uc_ChannelNo);
            ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_VmaxOnCycle].m_fValue = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
        }
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue = p_DoMesBuf->ScheduleMessage.CycleID;
        ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //装载所需的Step到Step.Mgr任务的副本中
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
// 			ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue = DoMessageBuf[uc_ChannelNo].ScheduleMessage.CycleID;
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
        IVSDL::LimitArrivedRoutine(uc_ChannelNo);  //????,??Limit??????
        //EthernetParse::uc_LogCriticalPoint = LOG_DATA_CRITICALPOINT; // 2013/10/24 yang  修改关键点记录数据
        //Step_Manager_task::SendLog(uc_ChannelNo);
//			RecordLastStepData(uc_ChannelNo);	//20130723记录上一步骤的数据
        StepMessage.MsgNumber = Step_JumpA_Msg;
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);
    }
//    }

}
void Schedule_Manager_task::SchContinueMsgRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
        ClearAUXLimits(uc_ChannelNo, 0);
//#endif
    if( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 1) //检查Step任务是否在调用一次性时序引擎   可以在大数据体的Channel_Info中查找
        return;    //调用引擎过程中保留此命令
    else
    {
        signed char sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
        if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
            return;

        if((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx + 1) < ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount)
        {
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx++;
            ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //?????Step?Step.Mgr??????
            StepMessage.MsgNumber = Step_JumpA_Msg;
        }
        else
        {
            StepMessage.MsgNumber = Step_Stop_Msg;
            IVSDL::ScheduleEndedRoutine(uc_ChannelNo);
            St_ScheduleMessage   SchMessage;
            SchMessage.MsgNumber = Sch_Stop_Msg;
            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage);
        }

        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
        IVSDL::LimitArrivedRoutine(uc_ChannelNo);
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);
    }
}
/*****************************************************
// Function Name: SchLimitArriveMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                          Sch_LimitArrive_Msg信息响应过程处理
// Others:
//
//                通知PCLimit到达；根据Limit去向决定：
//                1.停止运行，转入GOSTOP状态，如需要则向PC发出PC_Unsafe_Msg；
//                2.装载新Step到Step.Mgr任务的副本中，按需求初始化（重置）所需MV，发出Msg给Step.Mgr任务来运行此Step；
//                3.在任务内执行SetVariable_Step并更新MV，Sync变化的MV给PC，同时按SetVariable去向重新执行本Sch_LimitArrive_Msg响应逻辑；
// transitional information:
//
//                PC_LimitArrive_Msg, PC_UnsafeRpt_Msg, Step_Stop_Msg, Step_Run_Msg
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchLimitArriveMsgRoutine(unsigned char uc_ChannelNo)
{
    unsigned char ucActiveIdx;
    St_StepMessage StepMessage;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];
    St_ChannelInfo	* p_Info =	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];

    IRSpecialTestFlg[uc_ChannelNo] = 0; //无论是否进行内阻测试都可以直接清掉该标记
    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = false;
    ChannelInfo_Manager::m_RC_CurrentFilterDeepth[uc_ChannelNo] = 1;
    ChannelInfo_Manager::m_RC_VoltageFilterDeepth[uc_ChannelNo] = 1;

    if(ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo])
    {
// 		if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.m_ImpedanceGoto == AbnormalGoto_Stop)
// 		{
        p_DoMesBuf->ScheduleMessage.GotoStepID = MCU_GOTOUNSAFE;
        //添加unsafe种类为Ir异常
        IR_UnsafeFlg[uc_ChannelNo] = 1;
// 		}
        //  if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.m_ImpedanceGoto == AbnormalGoto_Warning)
        //{
        //	Un_ERROR_MESSAGE * st_SPB;
        //	st_SPB = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;
        //	st_SPB->m_IV_ERROR.m_btChannel_ID = uc_ChannelNo;
        //	st_SPB->m_IV_ERROR.m_btCode_Type = IV_ERROR_TYPE_WARNNING;
        //	st_SPB->m_IV_ERROR.m_btType_ErrorNum = IV_ERROR_WARNNING_IR ;
        //    st_SPB->m_IV_ERROR.m_btSubCode = eIRCHECK_ERROR_Unsafe; //safety_check::CtrlValueCheck[safety_check::m_CtrlValueSetting.ChannelIdx].ErrorCode;
        //    st_SPB->m_IV_ERROR.m_fIR_Value = 100; //hpq/20150814
        //	EthernetParse::MPReportData(uc_ChannelNo,SEND_ERROR_MESSAGE);
        //}

        // ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
    }
    ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo] = 0; //清除该标记

//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ucActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
            CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][AuxIdx].m_bShiedCheck = 1;
        ClearAUXLimits(uc_ChannelNo, 1); //20131121 dirui 在Step结束之后清除辅助通道Limits
        CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNCount = 0;
    }
    else
    {
        ucActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    }
    St_StepCtrl * p_Ctrl	=		&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl;  //171102 zyx
    unsigned long uw32_Increment    =		p_Ctrl->m_CtrlValues.m_fControlValue[1];
    if(p_Ctrl->m_u16ControlType == CT_IVCH_SETVARIABLE && (GetSetVariable(uw32_Increment, MP_MV_INC_CHAN_CYCLE_INDEX) == 1))       //CycleIndex
    {
        ChannelInfo_Manager::ResetNormalMV(uc_ChannelNo, MetaCode_PV_ChargeCapacityTime);   //zyx 171010
        ChannelInfo_Manager::ResetNormalMV(uc_ChannelNo, MetaCode_PV_DischargeCapacityTime);
    }
//    MetaVariable_Task::calculate_capacity(1, uc_ChannelNo);
    if(Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVARIABLE\
            && Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVALUE) //mits8
        EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_SDL_CYCLE); // 20180202 yy
//    SendMetaSync(uc_ChannelNo);

    switch(p_DoMesBuf->ScheduleMessage.GotoStepID)           //取出下一步处理的方向进行判断
    {
    case MCU_GOTONEXTSTEP:

        if(ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, MCU_GOTONEXTSTEP) == 1) //装载所需的Step到Step.Mgr任务的副本中
        {
            bMetaSyncSend[uc_ChannelNo] = 0;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;        //Limit到位导致需要跳步，跳步前发JUMP
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg信息必需先存跳转信息
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID 在Parse中要添加
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID 在Parse中要添加
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
            p_Info->m_Event_Source = S_RUNNING_LIMITARRIVEMSG_GOTONEXTSTEP;
            //2016.05.10
// 			if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE && \
// 				(int)(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]+0.1f) == ACIM_PAUSE)
// 					Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_START;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                //向PC发送回复
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;


            if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_REST)
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0.0f;

            SchTimeTrigger[uc_ChannelNo] = 0;     //Limit到达后要向Step发出执行下一步的信息，此时要清超时标记，在当前的Schedule状态下执行超时处理，等待Step成功运行返回Started标记。0911 dirui
            StepMessage.MsgNumber = Step_JumpA_Msg;
            //此处插入若第一次进入电流控制类型步时IR异常判断处理
            if((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck) && (Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] == 0))
            {
                if(IRspecial_Judge(uc_ChannelNo))
                    IRspecial_Judge_Process(uc_ChannelNo);
            }


            //////////////////////////////////////////////////
        }
        else
        {
            //装载失败停止通道
            StepMessage.MsgNumber = Step_Stop_Msg;

            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;   //Limit_ID 在Parse中要添加

        }

        if(StepMessage.MsgNumber == Step_Stop_Msg)
            p_Info->ScheduleStatus = GOSTOP;
        else
            Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
        // #endif

        break;
    case MCU_GOTOSTOPSTEP:
        //  StepMessage.MsgNumber=Step_Stop_Msg;
        //  Step_Manager_task::SendMsgToStep(uc_ChannelNo,StepMessage);     //发出Msg给Step停止
        p_Info->ScheduleStatus = GOSTOP;
        bMetaSyncSend[uc_ChannelNo] = 0; //zyx 171010
        SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
        p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID 在Parse中要添加
        Smart_Battery::SetSMBusLastLog(uc_ChannelNo, LASTPOINT);//add by zc 04/01/2019
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
        break;

    case MCU_GOTOUNSAFE:

        //StepMessage.MsgNumber=Step_Stop_Msg;
        //Step_Manager_task::SendMsgToStep(uc_ChannelNo,StepMessage);     //发出Msg给Step停止
        p_Info->ScheduleStatus = GOSTOP;
        SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
        if(IR_UnsafeFlg[uc_ChannelNo])
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_CHANNELABNORMAL;
        else
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
        //这里要和上位联系Unsafe的编码问题 ，否则在monitor上只显示CODE-0 UNSAFE,并不显示LimitID号
        p_Info->m_UnsafeType = IV_UNSAFE_TYPE;
        p_Info->m_SafetyStatus = LIMIT_UNSAFE; //添加错误码 暂时定义为64
        safety_check::SetUnsafe(uc_ChannelNo, LIMIT_UNSAFE, 0,IV_UNSAFE_TYPE);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CHECKLIMIT;
        p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
        break;
    case MCU_GOTOLOG:  //LogLimit不做处理
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
    //break;
    case MCU_GOTOGROUPWAITFORNEXT:
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
    //break;
    case MCU_GOTOGROUPFOLLOWNEXT:
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
        break;
    default:

        if(ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_DoMesBuf->ScheduleMessage.GotoStepID) == 1)
        {
            bMetaSyncSend[uc_ChannelNo] = 0;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;    //Limit到位导致需要跳步，跳步前发JUMP
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg信息必需先存跳转信息
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID 在Parse中要添加
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID 在Parse中要添加
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
            p_Info->m_Event_Source = S_RUNNING_LIMITARRIVEMSG_GOTOSTEP;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                //向PC发送回复
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;


            if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_REST)
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0.0f;

            SchTimeTrigger[uc_ChannelNo] = 0;                     //Limit到达后要向Step发出执行下一步的信息，此时要清超时标记，在当前的Schedule状态下执行超时处理，等待Step成功运行返回Started标记。 0911 dirui
            StepMessage.MsgNumber = Step_JumpA_Msg;
            //此处插入若第一次进入电流控制类型步时IR异常判断处理
            if((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck) && (Schedule_Manager_task::IRHasBeenRun == 0))
            {

                if(IRspecial_Judge(uc_ChannelNo))
                    IRspecial_Judge_Process(uc_ChannelNo);
            }


            //////////////////////////////////////////////////
        }
        else
        {
            //装载失败停止通道
            StepMessage.MsgNumber = Step_Stop_Msg;
            SchTimeTrigger[uc_ChannelNo] = 0; //接下来要转到Gostop状态，执行向Step发送Stop信息的处理，所以清超时标志位 ，Gostop处理中执行超时处理  0911 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;   //Limit_ID 在Parse中要添加

        }


        if(StepMessage.MsgNumber == Step_Stop_Msg)
            p_Info->ScheduleStatus = GOSTOP; //GoStop send Step_Stop_Msg too.  dirui  2015/09/16
        else
            Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除

        //#endif
        break;

    }

    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除

}
/*****************************************************
// Function Name: SchLimitPropertyChangeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                          Sch_LimitPropertyChange_Msg信息响应过程处理
// Others:
//
//                判断Sch.MD5、StepID、LimitID是否合法，根据Msg内容更改Step.Mgr中相应Limit属性
// transitional information:
//
//
//
// History：暂不使用
//
// ******************************************************/
// void Schedule_Manager_task::SchLimitPropertyChangeMsgRoutine(unsigned char uc_ChannelNo)
// {
//     unsigned char ucActiveIdx;
//     ucActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
// //    unsigned char LimitID=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.LimitID;
// //	unsigned char EquationID=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;
//     if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))   //寻找相应的Schedule ；  大数据体中实现
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID==ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)  //StepID 合法
//         {
// 					  //LimitID值和下位机Limit数量无直接关系，暂时无法判断
//            // if(LimitID<=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo)//
//           //  {
// 				for(unsigned char uc_i=0;uc_i<(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo
// 						+ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo);uc_i++)
// 				{
// 					//if((LimitID==ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_u16LimitID)
// 					//	&& (EquationID==ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_ucEquationID))
// 					//ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_ucIfCanSelfJump=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.LimitPropertyChange;    //更新大数据体中本limit是否可以独立跳步状态

// 				}
//            // }
//         }
//     }
//     //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
// }
/*****************************************************
// Function Name: SchLimitStatusChangeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       Sch_LimitStatusChange_Msg信息响应过程处理
// Others:
//
//                 向PC发出相应Msg
// transitional information:
//                PC_LimitStatusChanged_Msg
//
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchLimitStatusChangeMsgRoutine(unsigned char uc_ChannelNo)
{
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD=PC_LimitChange_Msg;  //Limit状态变更，不导致跳步，发送LimitChange
    //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //向PC发送回复
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_LIMITSTATUSCHANGEMSG;
    SchReportMessageToPC(uc_ChannelNo, PC_LimitChange_Msg, &UnsafeContent[uc_ChannelNo]);
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
}
/*****************************************************
// Function Name: SchStartedMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Started_Msg信息响应过程处理
// Others:
//                向PC发出相应Msg，退出超时机制
//
// transitional information:
//
//                PC_Started_Msg,PC_ResumeRpt_Msg,PC_JumpARpt_Msg,PC_JumpMRpt_Msg
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchStartedMsgRoutine(unsigned char uc_ChannelNo)
{

    SchTimeTrigger[uc_ChannelNo] = 0; //现在已经确认Step正常运行起来了 ，不需要继续执行超时计算处理，清掉标记      0911 dirui
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD=PC_Started_Msg; //执行完毕要的切换过程后，发送Started
    //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //EthernetParse::m_ReportStatus.m_stMSecondField.m_wChannelNumber=uc_ChannelNo;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //向PC发送回复
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_STARTEDMSG;
    SchReportMessageToPC(uc_ChannelNo, PC_Started_Msg, &UnsafeContent[uc_ChannelNo]);
    IVSDL::StepStartedRoutine(uc_ChannelNo, true);
    if(Smart_Battery::BeSupportSmbus)
        MetaVariable_Task::My_SMBLog_Status = SMBLOG_WAIT;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
}
/*****************************************************
// Function Name: SchStoppedMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Stopped_Msg信息响应过程处理
// Others:
//                GOPAUSE状态时转入IDLE状态；在GOSTOP状态时转入IDLE状态并且清除掉Assign所有内容；
//
// transitional information:
//
//                PC_StopRpt_Msg
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchStoppedMsgRoutine(unsigned char uc_ChannelNo)
{
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD = PC_StopRpt_Msg; //Schedule停止，执行完必要的切换过程后，发送Stopped；
    //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    unsigned char uc_CMD = PC_StopRpt_Msg;
    unsigned char uc_ReleaseRedundantSafety = 0;
    unsigned char uc_IDLE = IDLE;
// 	St_PowerData m_data;

    ST_EVENT_SCHEDULE_RUNNING *st_status = &UnsafeContent[uc_ChannelNo].st_status_sch_running;

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
    {
        //     if(M0RedundantFunc::m_bIsM0ReleaseCheck[uc_ChannelNo])
        // 	{
        //          board_config::ReleaseRedundantSafety(uc_ChannelNo,0);	     //解除冗余保护处理
        // 	     safety_check::RedundantSafetyFlg[uc_ChannelNo]=0;
        //     }
        if(M0RedundantFunc::m_bIsM0ReleaseCheck[uc_ChannelNo])
            uc_ReleaseRedundantSafety = 1;
    }

    if(Smart_Battery::SMB_RunFlg )
    {
        Smart_Battery::SMB_RunFlg = false;

    }

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge = false;
    switch(SendFinishFlag[uc_ChannelNo].StopStatus)
    {
    case STOP_FINISHED:
        st_status->m_btStopped_Status = STOP_FINISHED;
        PWM_Base::PWM_LEDRunStatus = SYSTEM_IDLE; //06.19.2018 add by zc 解決跑完schedule后等不會變回黃色的BUG
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //响应处理完毕后转到IDLE状态；
        break;
    case STOP_UNSAFE:
        //	if((safety_check::ErrorFlag[uc_ChannelNo]&REDUNDANT_SAFETY)!=0)  //Schedule 在停完通道后根据标记决定做恢复电压保护时序  dirui 20140730
        // 		    if(safety_check::RedundantSafetyFlg[uc_ChannelNo])
        // 		    {
        // 				board_config::ReleaseRedundantSafety(uc_ChannelNo,0);	     //解除冗余保护处理
        // 				safety_check::RedundantSafetyFlg[uc_ChannelNo]=0;
        // 		    }
        if(safety_check::RedundantSafetyFlg[uc_ChannelNo])
            uc_ReleaseRedundantSafety = 1;

        //uc_CMD = PC_Unsafe_Msg;        //设定回复信息为Abnormal信息
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
        st_status->m_btStopped_Status = STOP_UNSAFE;  //  20130830 狄
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_GOSTOP_STOPPEDMSG;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_EMERGENCY:
        st_status->m_btStopped_Status = STOP_EMERGENCY;
        st_status->m_btSubCode = SendFinishFlag[uc_ChannelNo].SubStopCode;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;            // 20141020 di Fix code 0 for gotostop over time.
        break;
    case STOP_MANUAL:
        st_status->m_btStopped_Status = STOP_MANUAL;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //响应处理完毕后转到IDLE状态；
        break;
    case STOP_POWER_OFF: //add by dirui 20131016
        st_status->m_btStopped_Status = STOP_POWER_OFF;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //?????????IDLE??;
        break;

    case STOP_AUXACKFAIL:   //add by dirui on 20131120
        //uc_CMD = PC_Unsafe_Msg;     //  yang  20150605  解决显示Finish问题
        st_status->m_btStopped_Status = STOP_AUXACKFAIL;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = AUX_ACK_FAILED;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = AUX_ACK_FAILED;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_GOSTOP_STOPPEDMSG;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_AUXUNSAFE:    //add by dirui on 20131120
        //uc_CMD = PC_Unsafe_Msg;
        st_status->m_btStopped_Status = STOP_AUXUNSAFE;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_AUXACKFAILINSTART:
        //uc_CMD = PC_Unsafe_Msg;  //暂时认为开始时应答失败同运行中辅助应答失败处理相同  dirui 20140530
        st_status->m_btStopped_Status = STOP_AUXACKFAILINSTART;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_SMBUS_LOST:
        //uc_CMD = PC_Unsafe_Msg;  //暂时认为开始时应答失败同运行中辅助应答失败处理相同  dirui 20140530
        st_status->m_uw32Safe_Code = SMB_LOST_UNSAFE;
        st_status->m_btStopped_Status = STOP_SMBUS_LOST;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_CAN_BMS_LOST:
        //uc_CMD = PC_Unsafe_Msg;  //暂时认为开始时应答失败同运行中辅助应答失败处理相同  dirui 20140530
        st_status->m_uw32Safe_Code = CAN_BMS_UNSAFE;
        st_status->m_btStopped_Status = STOP_CAN_BMS_LOST;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_CHANNELABNORMAL:
        //uc_CMD = PC_StopRpt_Msg;   //  20140703 yy  解决无法正确显示报警问题。
        st_status->m_btStopped_Status = STOP_CHANNELABNORMAL;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
        uc_IDLE = FIXEDUNSAFE;
        break;
    /*case STOP_ACR_OVERTIME:    //  ACR超时报警，无法抢到定时器资源
    	//uc_CMD = PC_Unsafe_Msg;  //暂时认为开始时应答失败同运行中辅助应答失败处理相同  dirui 20140530
    	//UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ACR_OVERTIME_UNSAFE;
      ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = IV_UNSAFE_TYPE;
    	ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = ACR_OVERTIME_UNSAFE;
      safety_check::SetUnsafe(uc_ChannelNo, ACR_OVERTIME_UNSAFE, 0,IV_UNSAFE_TYPE);
      st_status->m_bUnsafeType = IV_UNSAFE_TYPE;
      st_status->m_uw32Safe_Code = ACR_OVERTIME_UNSAFE;
    	st_status->m_btStopped_Status = STOP_ACR_OVERTIME;
    	//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
    	//EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726信息内容
    	uc_IDLE = FIXEDUNSAFE;
    	break;*/
    default:
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //20131023 dirui 添加其他条件下转换成IDLE状态的处理
        break;
    }
//     bMetaSyncSend[uc_ChannelNo] = 0;
//     SendMetaSync(uc_ChannelNo);
    if(SendFinishFlag[uc_ChannelNo].StopStatus > STOP_MANUAL)
    {
        ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, UNSAFE_POINT); //180309 zyx
        IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT); //180503 zyx //mits8
    }
    bMetaSyncSend[uc_ChannelNo] = 0;//2209161 zyx
    SendMetaSync(uc_ChannelNo);  //2209161 zyx

    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = uc_IDLE;
    if((SendFinishFlag[uc_ChannelNo].StopStatus >= STOP_UNSAFE)
            && (SendFinishFlag[uc_ChannelNo].StopStatus != STOP_EMERGENCY)
            && (SendFinishFlag[uc_ChannelNo].StopStatus != STOP_POWER_OFF)
            && (SendFinishFlag[uc_ChannelNo].StopStatus != STOP_CHANNELABNORMAL))
        uc_CMD = PC_Unsafe_Msg;

    if(uc_ReleaseRedundantSafety)
    {
        board_config::ReleaseRedundantSafety(uc_ChannelNo, 0);	    //解除冗余保护处理
        safety_check::RedundantSafetyFlg[uc_ChannelNo] = 0;
    }
// 	if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	{
// 		PowerAdj::SchRunningFlag[uc_ChannelNo]= false;
// 		bool bNeedStopFlag = false;
// 		unsigned char uc_runningcnt=0;
// 		for(unsigned char uc_channel=0;uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_channel++)
// 		{
// 			  if(PowerAdj::SchRunningFlag[uc_channel])
// 				{
// 					   uc_runningcnt++;
// 			   break;
// 		  }
// 		}
// 		if(uc_runningcnt==0)
// 		{
// 			bNeedStopFlag =true;
// 		}
// 		if(bNeedStopFlag)
// 		{
// 			PowerAdj::m_PowerValue.m_Vpp = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_fVpp;
// 			PowerAdj::m_PowerValue.m_Vmm = abs(ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_fVmm);
// 			PowerAdj::m_PowerAdjStatus =POWERADJ_SENDSTOPTOM0_1;  //POWERADJ_IDLE;
// 		}
// 	}
    SendFinishFlag[uc_ChannelNo].StopStatus = 0;
    SendFinishFlag[uc_ChannelNo].SubStopCode = eEMERGENCYSTOP_ERROR_Null;
    SendGoStopFlag[uc_ChannelNo] = 0;

    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].chn_Running_Flag = CHN_LOCK_IDLE;
//#if( FAN_SAFETY_MODE==1)
//#else
    if(board_config::Feature_Enable_init.Fan_Safety_Mode != 1)
        Schedule_Manager_task::LockModeSwitch[uc_ChannelNo] = 0; //解除安全标记为脉冲安全触发后不能通过恢复清除处理  20140919 dirui
//#endif
    //chen 20130906 add
    unsigned char j = 0;
    for(unsigned char i = 0; i < MAXCHANNELNO; i++)
    {
        if(ChannelInfo_Manager::ChannelInfo[i].chn_Running_Flag != CHN_LOCK_IDLE)
            j++;
    }
    if(j == 0)
    {
        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;   //????why是否有问题
    }

    SchTimeTrigger[uc_ChannelNo] = 0;                   //现在已经确认Step部分完成正常停止了，不需要执行超时计算，清掉标记    0911 dirui
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = CCS_OFF; //通道状态为停止
    //暂时为Resume服务
    ResumeManage[uc_ChannelNo].m_ResumeStepID = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;

    ResumeManage[uc_ChannelNo].m_ResumeCycleID = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;

    ResumeManage[uc_ChannelNo].m_ResumeTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_TestTime);
    ResumeManage[uc_ChannelNo].m_ResumeStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);
    ResumeManage[uc_ChannelNo].m_ResumeStatus = 0;

//	SendMetaSync(uc_ChannelNo);  //  20131029 he //zyx 171010

    ChannelInfo_Manager::Init_ChannelStep(uc_ChannelNo);  //清除通道step 防止历史数据传递到下次运行 dirui 20140923
    //EthernetParse::m_ReportStatus.m_stMSecondField.m_wChannelNumber=uc_ChannelNo;

    //if(EthernetParse::m_ReportStatus.m_btCMD  == ST_UNKNOWN)
    //    EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //if(EthernetParse::m_ReportStatus.m_btCMD == PC_Unsafe_Msg)
    //    memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //向PC发送回复
    SchReportMessageToPC(uc_ChannelNo, uc_CMD, &UnsafeContent[uc_ChannelNo]);   //向PC发送回复
    memset((void *)&UnsafeContent[uc_ChannelNo].st_status_sch_running, 0, sizeof(ST_EVENT_SCHEDULE_RUNNING)); //dirui 20161128
//#if MCU_PWMFAN_ENABLE == 1
//     if(board_config::Feature_Enable_init.Mcu_PWMFan_Enable==1)
// 	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,5);   //when channel has been stopped ,set PWM value 60%   dirui 2014/07/31
//#endif


//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        //停止SCH时判断是否SyncStop被勾选 zyx
        //直接判断温控箱标志位是否为开&&该通道是否为开温控箱的通道
        if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_bUseGroupTChamber)
            return;
        else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucSyncStop == 1)
        {
            for(int auxUnitIdx = 0; auxUnitIdx < MAXMCUUNIT; auxUnitIdx++)        //changed by zc 04.25.2018 遍历每个温控器标志位
            {
                for(int auxChnNo = 0; auxChnNo < MAXMTCCHAMBERNO; auxChnNo++)
                {
                    if( (uc_ChannelNo == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]) &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].OpenFlag>>auxChnNo)&0x01) == 0x01) )
                    {
                        /*勾选了温控箱随SCH同步停止*/
                        unsigned char runON = 0;
                        CANBusParse::IV_SendTempChamberOutRunON(uc_ChannelNo,
                                                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo],
                                                                runON);
                        //停止MTC温度保护
                        CANBusParse::IV_SendMTCTemperatureSaftyCheckOFF(uc_ChannelNo,
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo]);
                        //将温控箱设置为关状态
                        Schedule_Manager_task::CloseTemperatureChamber( auxUnitIdx, auxChnNo );

                    }
                    if(uc_ChannelNo == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]  &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].usingFlag>>auxChnNo)&0x01) == 0x01) )
                        Schedule_Manager_task::RelsaseTemperatureChamber( auxUnitIdx, auxChnNo );  //释放温控箱的占用
                }
            }
        }
        //ClearMapAUXSafetyInfo(uc_ChannelNo,ALLTYPE,0,ALLMAPCHANNEL);//20131121 dirui 在Schedule结束之后清除AUX的安全信息
    }
//#endif

    for(unsigned short int uw16_n = 0; uw16_n <  MCU_MAX_NUMBER; uw16_n++)
    {
        if(!MultiMcuComm::St_Unit[uw16_n].uc_Active)  // 整个表查一次
            continue;
        for(unsigned char uc_m = 0; uc_m < 3; uc_m++)
            MultiMcuComm::UnregisterChannel(uw16_n, uc_ChannelNo,((UDP_MAP_AUX|UDP_MAP_IV_PARALLEL|UDP_MAP_CAN_BMS)>>uc_m));
    }

    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //当前信息处理完毕要清除
}
/*****************************************************
// Function Name: SchStartRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         START状态的无信息输入处理过程
// Others:
//                        装载所需的Step到Step.Mgr任务的副本中，
//                        按需求初始化（重置）所有MV，发出Msg给Step.Mgr任务，启动step，转入RUNNING状态
// transitional information:
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchStartRoutine(unsigned char uc_ChannelNo)
{
//    Channel_MapAuxInfo::InitAuxMap(uc_ChannelNo);//190114 zyx
    ChannelInfo_Manager::LoadTestObject(uc_ChannelNo);

    St_StepMessage StepMessage;
    if(CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_BMSStatus != e_BMS_Normal)
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=GOSTOP;
        Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_CAN_BMS_LOST;
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //设定回复信息为错误信息触发上报
// 		EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_CANPARSE;
//         EthernetParse::m_ReportStatus.m_st_RunningStatus.m_bUnsafeType = CANBMS_UNSAFE_TYPE;
// 		EthernetParse::m_ReportStatus.m_st_RunningStatus.m_uw32Safe_Code = CAN_BMS_UNSAFE;
// 		EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_CAN_BMS_LOST;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType = CANBMS_UNSAFE_TYPE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = CAN_BMS_UNSAFE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_CAN_BMS_LOST;
        safety_check::SetUnsafe(uc_ChannelNo,CAN_BMS_UNSAFE,0, CANBMS_UNSAFE_TYPE);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code;
        return;
    }
    CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_UnsafeTime);

    if(Smart_Battery::BeSupportSmbus)
    {
        Smart_Battery::m_SMB_OverTime[uc_ChannelNo].m_SMBusStatus = e_SMB_Normal;
    }

    CANBusParse::IV_InitStatusMgr();
    if(CommonFunc::uc_Support_AUX)
    {
        Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].bAuxAckFailed = 0; //reset  temp
        if(!CANBusParse::IV_IfAuxNormal(uc_ChannelNo))
        {
            SchTimeTrigger[uc_ChannelNo] = 0;
            CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, LIMITSN);
//             CANBusParse::IV_InitStatusMgr();  // 20200730  clear Aux connect status after start
            return;
        }
        CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNCount = 0;
        CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNCount = 0;
//         CANBusParse::IV_InitStatusMgr();  // 20200730  clear Aux connect status after start
        SendScheduleMapToAUX(uc_ChannelNo);
        SendSafetyInfoToAUX(uc_ChannelNo);   //AUX  add by dirui  20131120
//         SendSDLParaToAUX(uc_ChannelNo);
//         CAN_WatchdogTask::IV_beNeedWatchdogflg=1;
        CAN_WatchdogTask::m_CanWatchdogACK.m_IVChannelMapMask |= (1 << uc_ChannelNo);
    }
// 		if(BuiltIn_DataManager::b_BuiltInAux)  //有Builtin 辅助存在时  qjm20180306
    if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
            || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))
    {   //190524,hpq
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck =0;
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck =0;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag = 0;
//       	if(CheckBit_32Bit(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_u16SafetyEnabed,MP_SCH_SAFTY_TYPE_AuxiVoltage)) // 设置BUILT-IN 电压安全
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_bEnabled)
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck |= (1 << 0); //AUX_VOLTAGE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 0);
//         }
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Temperature.m_bEnabled) // 设置BUILT-IN 温度安全
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Temperature.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck |= (1 << 1); //AUX TEMPERATURE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 1);
//         }

        Sch_BuiltinAuxSafety_Load(uc_ChannelNo);
    }
//#endif

    //	#if (SUPPORT_PARALLEL == 1)
    //if(ChannelInfo_Manager::IsInterUnitMasterChannel(uc_ChannelNo))
    // 	if ((ChannelInfo_Manager::gUnit_stUnitInfo.m_eUnitGroupingMode == eGroupMode_ParaInterUnit)
    // 		&& (ChannelInfo_Manager::gUnit_stUnitInfo.m_eUnitMode == eUnitMode_Para_Master)
    // 		&& ( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupingInfo.m_eChannelMode == eChannelMode_Master))
    if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType = CAN_CMD_PARA_START;
    }
    //	#endif
    OS_LogicProcess::be_NeedWaitflg = true;
    bMetaSyncSend[uc_ChannelNo] = 0;  //20131104,add by hepeiqing
    ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //step装载到副本，调大数据体接口
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 1;    //告诉STEP，这是一个SCHEDULE START/RESUME，需要初始化TEST TIME

    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;    //Resume的时候也初始化的话，前面运行的循环数就不会接续了。dirui/2014/10/29
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_fValue         = 0.0f;

    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_Value.m_uInt32 = 0;
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_fValue         = 0.0f;

//     ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_from_IRStep = false;  // We are not sure the previous IR still valid or not every time we start or resume experiment.
//     ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_from_Trans = false;  // We are not sure the previous IR still valid or not every time we start or resume experiment.
//     ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_from_Relax = false;  // We are not sure the previous IR still valid or not every time we start or resume experiment.

    // memset(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_IR_from_Schedule,false,sizeof(bool)*5);
    memset(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From, 0, sizeof(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From));
//================08232017 by ZC================
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV_TB = false;
//===============================================
    Step_Manager_task::ACCURATE_IR[uc_ChannelNo] = 0;  //清除之前测试的IR值  add by ZC 08.03.2018
    Step_Manager_task::INACCURATE_IR[uc_ChannelNo] = 0;   //清除之前测试的IR值  add by ZC 08.03.2018
    Step_Manager_task::SIMU_I_IR[uc_ChannelNo] = 0;				  	//清除之前测试的IR值  add by ZC 08.03.2018
    Step_Manager_task::SIMU_P_IR[uc_ChannelNo] = 0;				  	//清除之前测试的IR值  add by ZC 08.05.2019

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;

    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck)
    {
        //IR内阻测试
        if(IRspecial_Judge(uc_ChannelNo))
        {
            IRspecial_Judge_Process(uc_ChannelNo);//zyx 2105291
        }
    }
    StepMessage.MsgNumber = Step_Run_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);    //向step传递信息Step_Run_Msg
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING;
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendAUX_SafetyFlg[uc_ChannelNo] != 0)
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = WAITFORAUX; //进入等待其他应答过程
    }
//#endif
    Step_Manager_task::TimeRefreshFlag[uc_ChannelNo] = 0;
//    SendTirgger[uc_ChannelNo] = 0;
    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = false;
//#if MCU_PWMFAN_ENABLE == 1
//     if(board_config::Feature_Enable_init.Mcu_PWMFan_Enable==1)
// 	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,100);   //set PWM value 100%   dirui 2014/07/31
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);
    //#endif
    unsigned char uIntraLatstChanIdx = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;//ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
    for(unsigned char uc_i = uc_ChannelNo; uc_i < uIntraLatstChanIdx + uc_ChannelNo ; uc_i++)
        safety_check::Process_SM_Ptr[uc_i] = SAFE;

    if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive)
        CANBusParse::m_BMS_SendBuf_RunOn[uc_ChannelNo] = 1;

    SendGoStopFlag[uc_ChannelNo] = 0;
    IVSDL::SetLogMVUDFlag(uc_ChannelNo, LOG_ALL_MVUD); //190711 zyx

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_RisingEdge=0;
//			Pid_Ctrl::m_KiDomain[uc_ChannelNo]=0;
}

/*****************************************************
// Function Name: SchRunningRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                        RUNNING状态的无信息输入处理过程
// Others:
//                刚进入此状态，或发出Msg给Step.Mgr运行新Step后，以超时机制等待Step.Mgr发回的Msg，
//                其它时刻保持在此状态下。(超时引发错误，转入GOSTOP状态)
// transitional information:
//
//
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchRunningRoutine(unsigned char uc_ChannelNo)
{
    if ((SchTimeTrigger[uc_ChannelNo] == 0) && (ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus < 3) )
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;  //0819

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule计时标记
    }
    if(!Smart_Battery::SMB_RunFlg && Smart_Battery::BeSupportSmbus )
    {
        Smart_Battery::SMB_RunFlg = true;

    }

//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)       //超时？
    {
        SchTimeTrigger[uc_ChannelNo] = 0;       //已经超时了就不必再处理超时计算了，清掉标记。0911 dirui
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_START_FAIL;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_START_FAIL;;
        SchReportMessageToPC(uc_ChannelNo, PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_EMERGENCY;
        SendFinishFlag[uc_ChannelNo].SubStopCode = eEMERGENCYSTOP_ERROR_SchOverTime;
    }

}
/*****************************************************
// Function Name:SchResumeRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       RESUME状态的无信息输入处理过程
// Others:
//                装载所需的Step到Step.Mgr任务的副本中，按Sync信息来设置和计算初始MV，
//                发出Msg给Step.Mgr任务，启动step，转入RUNNING状态
// transitional information:
//                Step_Run_Msg
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchResumeRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::LoadTestObject(uc_ChannelNo);

    St_StepMessage StepMessage;
    // 	#if(SUPPORT_CAN_BMS == 1)
    if(CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_BMSStatus != e_BMS_Normal)
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
        Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_CAN_BMS_LOST;
//         EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //设定回复信息为错误信息触发上报
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType = CANBMS_UNSAFE_TYPE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = CAN_BMS_UNSAFE;
        safety_check::SetUnsafe(uc_ChannelNo,CAN_BMS_UNSAFE,0, CANBMS_UNSAFE_TYPE);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_CAN_BMS_LOST;
        return;
    }
    CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_UnsafeTime);
    //	#endif

    if(Smart_Battery::BeSupportSmbus)
    {
        Smart_Battery::m_SMB_OverTime[uc_ChannelNo].m_SMBusStatus = e_SMB_Normal;
    }

//#if SUPPORT_AUX==1
    CANBusParse::IV_InitStatusMgr();
    if(CommonFunc::uc_Support_AUX)
    {
        Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].bAuxAckFailed = 0; //reset  temp
        if(!CANBusParse::IV_IfAuxNormal(uc_ChannelNo))
        {
            SchTimeTrigger[uc_ChannelNo] = 0;
            CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, LIMITSN);
//             CANBusParse::IV_InitStatusMgr();  // 20200730  clear Aux connect status after start
            return;
        }
        CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNCount = 0;
        CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNCount = 0;
//         CANBusParse::IV_InitStatusMgr();  // 20200730  clear Aux connect status after start
        SendScheduleMapToAUX(uc_ChannelNo);
        SendSafetyInfoToAUX(uc_ChannelNo);	 //AUX	add by dirui  20131120
//         SendSDLParaToAUX(uc_ChannelNo);
    }
// 		if(BuiltIn_DataManager::b_BuiltInAux)  //有Builtin 辅助存在时  qjm20180306
    if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
            || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))
    {   //190524,hpq
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck =0;
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck =0;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag = 0;
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyVoltage.m_bEnabled) // 设置BUILT-IN 电压安全
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyVoltage.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck |= (1 << 0); //AUX_VOLTAGE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 0);
//         }
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyTemperature.m_bEnabled) // 设置BUILT-IN 温度安全
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyTemperature.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fLow  = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck |= (1 << 1); //AUX TEMPERATURE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 1);
//         }
        Sch_BuiltinAuxSafety_Load(uc_ChannelNo);
    }
//#endif

    if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType = CAN_CMD_PARA_START;
    }
    ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //step装载到副本，调大数据体接口
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 1;	//告诉STEP，这是一个SCHEDULE START/RESUME，需要初始化TEST TIME

    if(ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_InternalResistance].m_fValue > 0)
    {
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep = true;
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_InternalResistance].m_fValue;
    }
    else
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep = false;  // We are not sure the previous IR still valid or not every time we start or resume experiment.
    //memset(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_IR_from_Trans,false,sizeof(bool)*3);

//    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Trans = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Relax = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge = false;
//================08232017 by ZC================
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV_TB = false;
//===============================================
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV = false;

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;

    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck)
    {
        //IR内阻测试
        if(IRspecial_Judge(uc_ChannelNo))
        {
            IRspecial_Judge_Process(uc_ChannelNo);//zyx 2105291
        }
    }
    StepMessage.MsgNumber = Step_Run_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage); 									 //向step传递信息Step_Run_Msg
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING; //进入等待其他应答过程   20140609 di  解决RESUME问题
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendAUX_SafetyFlg[uc_ChannelNo] != 0)
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = WAITFORAUX; //进入等待其他应答过程
    }
//#endif
    Step_Manager_task::TimeRefreshFlag[uc_ChannelNo] = 0;
//    SendTirgger[uc_ChannelNo] = 0;
//#if MCU_PWMFAN_ENABLE == 1
//     if(board_config::Feature_Enable_init.Mcu_PWMFan_Enable==1)
// 	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,100);   //set PWM value 100%   dirui 2014/07/31
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);
    //#endif
    OS_LogicProcess::be_NeedWaitflg = true;

    if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive)
        CANBusParse::m_BMS_SendBuf_RunOn[uc_ChannelNo] = 1;

    SendGoStopFlag[uc_ChannelNo] = 0;
    IVSDL::SetLogMVUDFlag(uc_ChannelNo, LOG_ALL_MVUD); //190711 zyx
}
/*****************************************************
// Function Name:SchGopauseRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       GOPAUSE状态的无信息输入处理过程
// Others:
//                记录当前所有MV，Sync到PC；记录最后数据点，并上传到PC；
//                发出停止Msg到Step.Mgr，进入超时机制；收到Step.Mgr的回复后转入IDLE状态，
//                或超时之后直接调用一次性引擎结束通道后转入IDLE状态。
// transitional information:
//                Step_Stop_Msg
//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchGopauseRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;


    StepMessage.MsgNumber = Step_Stop_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);  //发送Step停止信息；
    if (SchTimeTrigger[uc_ChannelNo] == 0)
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule计时标记
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息 准备接收Step回复信息
    }

//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ClearAUXLimits(uc_ChannelNo, 0);
    }
//#endif
    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)                                                                           //超时？
    {
        SchTimeTrigger[uc_ChannelNo] = 0; //已经超时了就不必再处理超时计算了，清掉标记。0911 dirui
        //调用一次性引擎结束通道
        StopChannel(uc_ChannelNo);                             //调用一次性引擎结束通道
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_STOP_FAIL;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_START_FAIL;;
        SchReportMessageToPC(uc_ChannelNo, PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE; //结束通道后转入IDLE状态

    }
}
/*****************************************************
// Function Name:SchGoStopRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       GOSTOP状态的无信息输入处理过程
// Others:
//                记录当前所需记录的MV，Sync到PC；记录最后数据点，
//                并上传到PC；发出停止Msg到Step.Mgr，进入超时机制；
//                收到Step.Mgr的回复后转入NOTREADY状态，或超时之后直接调用一次性引擎结束通道后转入NOTREADY状态。
// transitional information:
//                Step_Stop_Msg,(PC_StopRpt_Msg)

//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SchGoStopRoutine(unsigned char uc_ChannelNo)
{
    if((ChannelInfo_Manager::BoardCFG.m_Global.m_bHaveVoltLowRng) && (board_config::Func_Set_Volt_Range != 0))
    {
        ChannelInfo_Manager::Volt_Range_Change_Flag[uc_ChannelNo] = 1;
        board_config::Func_Set_Volt_Range(uc_ChannelNo,VOLTAGE_RANGE_HIGH);
// 			ControlPin::SetMeLow(LBT22XXX_Base::PinGroup[LBT22XXX_Pin_VD]);
// 			LBT22XXX_Base::VoltageRangeFlag[uc_ChannelNo] = 2;

        CommonFunc::GetTimeTicksUpdate(&ChannelInfo_Manager::Voltage_Range_Change_Time[uc_ChannelNo]);
    }

    St_StepMessage StepMessage;
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    if(SendGoStopFlag[uc_ChannelNo] == 0)
    {

        // 			switch(SendFinishFlag[uc_ChannelNo].StopStatus)  //各种Stop原因处理
        // 			{
        // 				case STOP_UNKNOWN:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_UNKNOWN;
        // 					break;
        // 				case STOP_FINISHED:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_FINISHED;
        // 					break;
        // 				case STOP_MANUAL:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status =  STOP_MANUAL;
        // 					break;
        // 				case STOP_UNSAFE:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_UNSAFE;
        // 					break;
        // 				case STOP_EMERGENCY:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_EMERGENCY;
        // 					break;
        // 			   	case STOP_POWER_OFF:  //add by dirui 20131016
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_POWER_OFF;
        //                     break;
        // 				case STOP_AUXACKFAIL:   //add by dirui on 20131120
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_AUXACKFAIL;
        // 					break;
        // 				case STOP_CAN_BMS_LOST:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_CAN_BMS_LOST;
        // 					break;
        // 				case STOP_AUXUNSAFE:    //add by dirui on 20131120
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_AUXUNSAFE;
        // 					break;
        // 				case STOP_AUXACKFAILINSTART:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_AUXACKFAILINSTART;
        // 					break;
        // 				case STOP_CHANNELABNORMAL:
        // 					EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status =STOP_CHANNELABNORMAL;
        // 					break;
        //                 case STOP_ACR_OVERTIME:
        // 				    EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_ACR_OVERTIME;
        // 					break;
        // 				default:
        // 					break;
        // 			}
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = SendFinishFlag[uc_ChannelNo].StopStatus;
        //EthernetParse::m_ReportStatus.m_btCMD = PC_GoStop_Msg;
        //if( EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source == ST_UNKNOWN)
        //   EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source  =  ST_SCHMANAGE;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_GOSTOP_MAINTAINCE;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = SendFinishFlag[uc_ChannelNo].StopStatus;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_MAINTAINCE;
        SchReportMessageToPC(uc_ChannelNo, PC_GoStop_Msg, &UnsafeContent[uc_ChannelNo]);
        SendGoStopFlag[uc_ChannelNo] = 1;

//#if SUPPORT_AUX==1
        if(CommonFunc::uc_Support_AUX)
        {
            ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui 在Schedule结束之后清除AUX的安全信息
            ClearAUXLimits(uc_ChannelNo, 0);
        }
//#endif
        //以下为MV部分服务
        MetaVariable_Task::calculate_capacity(1, uc_ChannelNo);
        MetaVariable_Task::capacityStatus[uc_ChannelNo] = 0;
    }
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_AUXACKFAILINSTART)    //若是开始就没跑起来那么不必在发stop给step，否则要超时保护   dirui 20140604
        {
// 			Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
            CANBusParse::IV_IfAuxNormal(uc_ChannelNo);
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //设定回复信息为错误信息触发上报
            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_AUXACKFAILINSTART;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_MAINTAINCE;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
            SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息
            return;
        }
    }
//#endif

    //#if(SUPPORT_CAN_BMS == 1)
    if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_CAN_BMS_LOST)
    {
// 		Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //设定回复信息为错误信息触发上报
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_MAINTAINCE;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = CAN_BMS_UNSAFE;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = CAN_BMS_UNSAFE;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_CAN_BMS_LOST;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息
        return;
    }
    //#endif

//========ADD BY ZC 12012017===========
// 	if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_SMBUS_LOST)
// 	{
// 		//EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //设定回复信息为错误信息触发上报
// 		UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SMBPARSE;
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_GOSTOP_MAINTAINCE;
// 		//UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = CAN_BMS_UNSAFE;
// 		//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code;
//
// 		UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_SMBUS_LOST;
// 		//EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
// 		//EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
// 		SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;
// 		//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息
// 		return;
// 	}
//=======================================
    if (SchTimeTrigger[uc_ChannelNo] == 0)
    {
        StepMessage.MsgNumber = Step_Stop_Msg;
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage); //发送Step停止信息

        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule计时标记
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息 准备接收Step回复信息
    }

//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    //ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = OS_ServeProcess::TimeDiff(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].SysTimeScheduleTask);    //算出时间差
    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)   //判断是否超时                                                                         //超时？
    {
        SchTimeTrigger[uc_ChannelNo] = 0;    //已经超时了就不必再处理超时计算了，清掉标记。0911 dirui
        StopChannel(uc_ChannelNo);                        //这里调用一次性引擎结束通道

        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //设定回复信息为错误信息
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_STOP_FAIL;
        //EthernetParse::m_ReportStatus.m_btCMD=PC_StopRpt_Msg; //装载回复命令
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_EMERGENCY;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_OVERTIME;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME;
        SchReportMessageToPC(uc_ChannelNo, PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //清除DoMessageBuf信息
    }
    Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] = 0; //下次开始要加入内阻测试步
    IR_UnsafeFlg[uc_ChannelNo] = 0;

    //加入对于脉冲的关闭处理
    if ( Pulse::Pulse_is_Running)  //如果有脉冲输出的话，尽快停止输出脉冲。
    {   Pulse::End(uc_ChannelNo);
        Pulse::Pulse_is_Running = false ;
    }

    //暂时先强行执行关闭DIDO操作，以后要考虑加入引擎中  dirui 2015/03/19
    // 	 board_config::OUTPUT_UnitDO(uc_ChannelNo,0,0);
    // 	 board_config::OUTPUT_UnitDO(uc_ChannelNo,1,0);
    if(board_config::Func_UnitDO_Reset != 0)
        (*board_config::Func_UnitDO_Reset)(uc_ChannelNo, 0);

// 	if(CAN_WatchdogTask::IV_beNeedWatchdogflg)
// 	{
// 		CAN_WatchdogTask::IV_beNeedWatchdogflg=0;
// 		CAN_WatchdogTask::m_CanWatchdogIVUnitACKStatus=CAN_WATCHDOG_IVUNIT_IDLE;
// 	}
// 	board_config::ClampH_Outputflag = 0;
// 	board_config::ClampL_Outputflag = 0;


    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = false;
//     ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 1;
    ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, true);
    ChannelInfo_Manager::m_RC_CurrentFilterDeepth[uc_ChannelNo] = 1;
    ChannelInfo_Manager::m_RC_VoltageFilterDeepth[uc_ChannelNo] = 1;
//    ChannelInfo_Manager::m_StepIndexUp[uc_ChannelNo] = 0XFFFF;
}
/*****************************************************
// Function Name:Sch_FixedUnsafeRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       FixedUnsafe状态处理
// Others:
//
// transitional information:
//
（测试）定时发送Unsafe
//
// History：暂不使用
//
// ******************************************************/
void Schedule_Manager_task::Sch_FixedUnsafeRoutine(unsigned char uc_ChannelNo)
{
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        static unsigned char bitReset = 1;
        //对aux 安全的处理 ,表面can已经恢复连接 ,但limitcount不对
        unsigned short bitSafeCode = 0;
        if(bitReset)  //reset一次，防止aux状态来不及更新(100ms)
        {
            for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
                CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bRefreshStatus = 0; //等待canBus更新状态
            bitReset = 0;
        }

        for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
        {
            if(!CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bExist)
                continue;
            if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bRefreshStatus)   //确定是已恢复
            {
                bitSafeCode |= 0 << UnitIDIdx;
                CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bRefreshStatus = 0;
            }
            else
                bitSafeCode |= 1 << UnitIDIdx;
        }
        if(bitSafeCode == 0)
        {
            bitReset = 1;
            ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL);
            ClearAUXLimits(uc_ChannelNo, 0);
            St_ScheduleMessage sch_msg;
            sch_msg.m_Message.m_Assign.m_u16ChannelIdx = uc_ChannelNo;
            sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = DYNAMICUNSAFE;
            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, sch_msg);
        }
    }
//#endif

    //#if(SUPPORT_CAN_BMS == 1)
    if((ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType==CANBMS_UNSAFE_TYPE)&&(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus == CAN_BMS_UNSAFE))
    {
        if(CANBusParse::m_BMS_OverTime[uc_ChannelNo].m_BMSStatus == e_BMS_Normal)
        {
            St_ScheduleMessage sch_msg;
            sch_msg.m_Message.m_Assign.m_u16ChannelIdx = uc_ChannelNo;
            sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = DYNAMICUNSAFE;
            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,sch_msg);
        }
        CANBusParse::m_BMSAckConfirmMgr[uc_ChannelNo].m_ChnStatus = en_BMS_ACK_Safe;
    }
    //	#endif

    //	 #if(SUPPORT_PARALLEL == 1)
    unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;

    if((ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType == IV_UNSAFE_TYPE) && (ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus == CAN_PARA_ACK_UNSAFE))
    {
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = 0xFF;
// 		ChannelInfo_Manager::ChannelInfo[0].m_SafetyStatus = 0;
        St_ScheduleMessage sch_msg;
        sch_msg.m_Message.m_Assign.m_u16ChannelIdx = uc_ChannelNo;
        sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = DYNAMICUNSAFE;
        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,sch_msg);
    }
    //	#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //清除信息
}
/*****************************************************
// Function Name:Sch_DynamicUnsafeRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       DynamicUnsafe状态处理
// Others:
//
// transitional information:
//                （测试）定时发送Unsafe

//
// History：暂不使用
//
// ******************************************************/
// void Schedule_Manager_task:: Sch_DynamicUnsafeRoutine(unsigned char uc_ChannelNo)
// {


// }
/*****************************************************
// Function Name:ClearScheduleMessage
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       清除ScheduleMessage
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/
void Schedule_Manager_task:: ClearScheduleMessage(St_DoMessage *Message)
{

    Message->Used = 0;
    Message->ScheduleMessage.MsgNumber = 0;
}

/*****************************************************
// Function Name:StopChannel
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                        调用引擎关闭通道
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/

//这里需要关闭通道时引入stepLen，否则unsafe时候有跑飞情况
//陈东修改 20130828
void Schedule_Manager_task::StopChannel(unsigned char uc_ChannelNo)
{
    St_DisposableMSG    DisposableMSG;
    DisposableMSG.AutoCaliFlag = 0;     //20141011,add by hepeiqing,  incase of causing IBT auto-cali relay mal-function
    unsigned char uIntraLatstChanIdx;
    if(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        uIntraLatstChanIdx = uc_ChannelNo;
    else if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
    else if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)) || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)))
        uIntraLatstChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1;

    for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
    {
        switch(ChannelInfo_Manager::ChannelInfo[uc_idx].m_CtrlState)   //调用相关的关通道时序
        {
        case CCS_I:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr; //&IBT_Base::SetCtrlCloseCh[0];      //临时调用IBT测试以后要统一到OS_LogicProcess::SetCtrlVltToOff_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
//             DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            break;
        case CCS_V:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
//             DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            break;
        case CCS_REST:
        case CCS_OFF:
        case CCS_OTHER:
        case CCS_EXTERNAL:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
            DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlRstToOff.stepLen;
//             DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            break;
        default:
            break;
        }
        DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
        DisposableMSG.TargetCtrlType = CT_OFF;  // 20200810 yy
        DisposableMSG.CH_Number = uc_idx;
        EngineHandle[uc_idx] = OS_LogicProcess::requestLogicPorcess(uc_idx, &DisposableMSG);

        switch(OS_LogicProcess::getHandleStatus(EngineHandle[uc_idx]))
        {
        case DISPOSABLE_ASSIGNED:                        //已经交付，待运行
            EngineOverTimeTriger[uc_idx] = 1;
            MsTimeEngineTask[uc_idx] = CommonFunc::GetSystemTime(); //OS_ServeProcess::OS_Time;
            break;
        case DISPOSABLE_RUNNING:          //运行中
            unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&SysTimeEngineTask[uc_idx]);
//             MsTime MyMsTime;
//             MyMsTime.Second = SysTimeEngineTask[uc_idx].Second;
//             MyMsTime.Us100 = SysTimeEngineTask[uc_idx].Us100;
//             TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);

            //MsTimeEngineTask[uc_idx]=OS_ServeProcess::TimeDiff(SysTimeEngineTask[uc_idx]);    //算出时间差

            if(EngineOverTimeTriger[uc_idx] == 1 && TimeTicks >= uw32_OverTime)       //超时？
            {
                EngineOverTimeTriger[uc_idx] = 0;
                //引擎调用失败处理

            }
            break;
        case DISPOSABLE_FINISHED:           //结束
            EngineOverTimeTriger[uc_idx] = 0;
            ChannelInfo_Manager::ChannelInfo[uc_idx].m_CtrlState = CCS_OFF;
            break;
        case DISPOSABLE_ERROR:            //出错
            //引擎调用失败处理
            break;
        case DISPOSABLE_DEAD:             //历史记录已经消亡
            break;
        default:
            break;
        }

        // 这里应该 通道Schedule初始化
        ResumeManage[uc_idx].m_ResumeStatus = 0;
    }
}
/*****************************************************
// Function Name: SendMsgToSchedule
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   向Schedule传递信息以后大数据体中实现
// Others:
//
// History：
//
// ******************************************************/
int Schedule_Manager_task::SendMsgToSchedule(unsigned char uc_ChannelNo, St_ScheduleMessage ScheduleMessage)
{
    if ((MsgBufManage[uc_ChannelNo].FullCount > MAXMESSAGE) || (MsgBufManage[uc_ChannelNo].EmptyCount < 0))
    {   //error
        for(unsigned short int uw16_i=0; uw16_i<MAXMESSAGE; uw16_i++)
        {
            MsgBufManage[uc_ChannelNo].RecordMsgEmpty[uw16_i]=uw16_i;//  {0,1,2,3,4,5,6,7}
            MsgBufManage[uc_ChannelNo].RecordMsgFull[uw16_i]=0;  //  20190302 hp
        }
        MsgBufManage[uc_ChannelNo].FullCount = 0;
        MsgBufManage[uc_ChannelNo].EmptyCount = EMPTYCOUNT; //MAXMESSAGE;
        return     -1;
    }

    if(MsgBufManage[uc_ChannelNo].EmptyCount <= 0)
        return     -1;   // 信息区满时保护处理

    //设置Msg优先级
    switch(ScheduleMessage.MsgNumber)
    {
    case Sch_Stop_Msg:
        ScheduleMessage.MsgPriority = 1;
        break;
    case Sch_Continue_Msg:
        ScheduleMessage.MsgPriority = 2;
        break;
    case Sch_Assign_Msg:
        ScheduleMessage.MsgPriority = 3;
        break;
    case Sch_LimitArrive_Msg:
//	case Sch_LimitPropertyChange_Msg:
    case Sch_LimitStatusChange_Msg:
    case Sch_Stopped_Msg:
    case Sch_OnSafe_Msg:
        ScheduleMessage.MsgPriority = 4;
        break;

    case Sch_Pause_Msg:
    case Sch_JumpM_Msg:
//#if (POWER_OFF_ENABLE == 1)
        if(board_config::Feature_Enable_init.Power_Off_Enable == 1)
        {
            //<<<LATER comment out chen 20131017  if powerOff do not response sch active
            if(!PowerLostCheck::PowerIsOn)    //PowerIsOn normal is 1
                return -1;
        }
//#endif
        ScheduleMessage.MsgPriority = 2;
        break;
    case Sch_JumpA_Msg:
//#if (POWER_OFF_ENABLE == 1)
        if(board_config::Feature_Enable_init.Power_Off_Enable == 1)
        {
            //<<<LATER comment out chen 20131017  if powerOff do not response sch active
            if(!PowerLostCheck::PowerIsOn)    //PowerIsOn normal is 1
                return -1;
        }
//#endif
        ScheduleMessage.MsgPriority = 3;
        break;
    case Sch_Started_Msg:
//#if (POWER_OFF_ENABLE == 1)
        if(board_config::Feature_Enable_init.Power_Off_Enable == 1)
        {
            //<<<LATER comment out chen 20131017  if powerOff do not response sch active
            if(!PowerLostCheck::PowerIsOn)    //PowerIsOn normal is 1
                return -1;
        }
//#endif
        ScheduleMessage.MsgPriority = 4;
        break;

    case Sch_Run_Msg:
    case Sch_Resume_Msg:
//#if (POWER_OFF_ENABLE == 1)
        if(board_config::Feature_Enable_init.Power_Off_Enable == 1)
        {
            //<<<LATER comment out chen 20131017  if powerOff do not response sch active
            if(!PowerLostCheck::PowerIsOn)    //PowerIsOn normal is 1
            {
                UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_POWER_OFF;
                SchReportMessageToPC(uc_ChannelNo, PC_StopRpt_Msg, &UnsafeContent[uc_ChannelNo]);
                return -1;
            }
        }
//#endif
        ScheduleMessage.MsgPriority = 2;
        break;

    case Sch_OnUnsafe_Msg:
//#if SUPPORT_AUX==1
    case AUX_Unsafe_Msg:								//加入AUX异常信息
    case AUX_ACKFAIL_Msg:								//加入AUX应答失败处理
//#endif
        ScheduleMessage.MsgPriority = 0;
        StartScheduleUnsafe[uc_ChannelNo] = UnsafeContent[uc_ChannelNo]; //?? 200420 zyx
        break;
    default:
        return -1;

    }
    unsigned char MsgRecordIndex = MsgBufManage[uc_ChannelNo].RecordMsgEmpty[MsgBufManage[uc_ChannelNo].EmptyCount - 1];
    MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount] = MsgRecordIndex;  //记录在Message队列中哪个地方；
    Schedule_Manager_task::ScheduleMessageBuf[MsgRecordIndex][uc_ChannelNo] = ScheduleMessage;
    MsgBufManage[uc_ChannelNo].FullCount++;
    MsgBufManage[uc_ChannelNo].EmptyCount--;

    return    1;
}
/*****************************************************
// Function Name: DeleteScheduleMsg
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   删除队列中指定一条信息
// Others:
//
// History：
//
// ******************************************************/
void  Schedule_Manager_task::DeleteScheduleMsg(unsigned char uc_ChannelNo, unsigned char Idx_Use)
{
    ScheduleMessageBuf[MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Use]][uc_ChannelNo].MsgNumber = 0;

    MsgBufManage[uc_ChannelNo].RecordMsgEmpty[MsgBufManage[uc_ChannelNo].EmptyCount] = MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Use];
    MsgBufManage[uc_ChannelNo].EmptyCount++;

    for(int i = Idx_Use; i < MsgBufManage[uc_ChannelNo].FullCount; i++)
        MsgBufManage[uc_ChannelNo].RecordMsgFull[i] = MsgBufManage[uc_ChannelNo].RecordMsgFull[i + 1]; //move all full index forward following Idx_Use,

    MsgBufManage[uc_ChannelNo].FullCount--;

// 	return 1;
}


/*****************************************************
// Function Name: SimulationAssignAddStartRotuine
// Version:        V1.0
// Date:           2013/12/12
// Description:
//
// Others:
//
// transitional information:
//
// History：
//
// ******************************************************/
unsigned char Schedule_Manager_task::SimulationAssignAddStarRoutine(unsigned char uc_ChannelNo, St_MD5Assign * AssignMsg) //12.22.2013ADD
{
    if(SimulationAssignRoutine(uc_ChannelNo, AssignMsg) == 1)  //12.22.2013ADD
        return ASSIGNOK;
    else
        return ASSIGNFAIL;

    St_ScheduleMessage   SchMessage;

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == RUNNING)
    {
        return SCHEDULERUNNING;
    }
    else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == DYNAMICUNSAFE)
    {
        SchMessage.MsgNumber = Sch_Run_Msg;
        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage);
        return SCHEDULEUNSAGE;
    }
    else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == NOTREADY)
    {
        if(SimulationAssignRoutine(uc_ChannelNo, AssignMsg) == 1)  //12.22.2013ADD
        {
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
            return ASSIGNOK;
        }
        else
        {
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = NOTREADY;
            return ASSIGNFAIL;
        }
    }
    else if((ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == IDLE))
    {
        if(SimulationAssignRoutine(uc_ChannelNo, AssignMsg) == 1) //12.22.2013ADD
            return ASSIGNOK;
        else
        {
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = NOTREADY;
            return ASSIGNFAIL;
        }
    }
    else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == FIXEDUNSAFE)
    {
        SchMessage.MsgNumber = Sch_Run_Msg;
        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage);
        return SCHEDULEUNSAGE;
    }

    return ERROR;

}


/*****************************************************
// Function Name: SimulationAssignRoutine
// Version:        V1.0
// Date:            2013/12/12/
// Description:
//
// Others:
//
// transitional information:
//
// History：
//
// ******************************************************/
int Schedule_Manager_task::SimulationAssignRoutine(unsigned char uc_ChannelNo, St_MD5Assign * AssignMsg) //12.22.2013ADD
{
    signed short BufIdx = -1; //190218 zyx
    if(AssignMsg->m_ucType == MD5CODE_TYPE_SIMULATION)
        BufIdx = SIMU_BUFFER::GetSimuIdx(AssignMsg->m_MD5);
    else if(AssignMsg->m_ucType == MD5CODE_TYPE_BATTERY_SIMULATION)
        return;
    if(BufIdx != -1)
    {
        AssignMsg->m_bOk = CONFIRM_RESULT_SUCCESS;
        return 1;
    }
    else
        return -1;
    // 	if (ChannelInfo_Manager::RegisterSimulation(uc_ChannelNo,AssignMsg)) //12.22.2013ADD
    //     {
    //
    //         AssignMsg->m_bOk = CONFIRM_RESULT_SUCCESS;
    //
    //         ChannelInfo_Manager::UnregisterSimulation(uc_ChannelNo);     //12.22.2013ADD

    //         ChannelInfo_Manager::RegisterSimulation(uc_ChannelNo,AssignMsg); //12.22.2013ADD

    // 				return 1;
    //     }
    //     else
    //     {
    // 				return -1;
    //     }
}

void Schedule_Manager_task::SendMetaSync(unsigned char uc_ChannelNo)
{
    if(bMetaSyncSend[uc_ChannelNo])
        return;   //已经发送过一次了，没必要再发一次。
    bMetaSyncSend[uc_ChannelNo] = 1;
// 	int i = 0;

// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_CycleIndex;

// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter2;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter3;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter4;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_ChargeCapacity1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_ChargeCapacity2;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_DischargeCapacity1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_DischargeCapacity2;
//
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_ChargeEnergy1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_ChargeEnergy2;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_DischargeEnergy1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_DischargeEnergy2;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Timer1;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Timer2;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Timer3;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Timer4;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_Voltage;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_Current;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_StepTime;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_TestTime;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_StepIndex;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_CycleIndex;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_ChargeCapacity;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_DischargeCapacity;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_ChargeEnergy;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_DischargeEnergy;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_InternalResistance;
//
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_ACR;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_dVdt;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_dIdt;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_AC_Impedance;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LS_VmaxOnCycle;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_Voltage;
//
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_Current;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_StepTime;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_TestTime;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_StepIndex;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_CycleIndex;
//
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_ChargeCapacity;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_DischargeCapacity;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_ChargeEnergy;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_DischargeEnergy;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_InternalResistance;
//
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_ACR;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_dVdt;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_dIdt;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_AC_Impedance;
// 	EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_LC_VmaxOnCycle;
//
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_StepIndex; //zyx 171010
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_Steptime;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_TestTime;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_CV_StageTime;
//
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_ChargeCapacityTime; //zyx 171010
//
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_DischargeCapacityTime;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_ChargeCapacity;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_DischargeCapacity;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_ChargeEnergy;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_PV_DischargeEnergy;
//
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter1; //zyx 171010
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter2;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter3;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_TC_Counter4;

//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_MV_UD1; //171109 zyx
//
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_MV_UD2;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_MV_UD3;
//     EthernetParse::m_ReportLogMetaVariable.m_wCustomerLogValue_Type[i++] = MetaCode_MV_UD4;

//     EthernetParse::m_ReportLogMetaVariable.m_wCustomLogVariables_Count  = i;

    EthernetParse::MPReportData(uc_ChannelNo, SEND_METAVARIABLESYNCH);
}

/*****************************************************
// Function Name:GetSetVaribale
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      取得SetVariable条件
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/
unsigned char Schedule_Manager_task::GetSetVariable(unsigned long uw32_Var, unsigned char uc_Pos)
{
    //MP_NUM_OF_META_VAR_RESET_VALUE

    unsigned char uc_ret;
    UWord32 uw32_temp;
    uw32_temp = uw32_Var >> uc_Pos;
    uw32_temp = uw32_temp & 0x01;

    uc_ret = (unsigned char)uw32_temp;
    return uc_ret;
}

/*****************************************************
// Function Name:GetSchStatus
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      取得Schedule状态
// Others:
//
// transitional information:
//
//
// History：
//
// ******************************************************/
unsigned char Schedule_Manager_task::IfSchRunning(unsigned char uc_ChannelNo)
{
    switch(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus)
    {
    case NOTREADY:
    case IDLE:
    case DYNAMICUNSAFE:
        return 0;
// 	case START:
// 	case RUNNING:
// 	case GOPAUSE:
// 	case RESUME:
// 	case GOSTOP:
// 	case FIXEDUNSAFE:
// 		return 1;
    default:
        return 1;
    }

}
/*****************************************************
// Function Name:RecordLastStepData
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                     记录LS相关变量
// Others:
//
// transitional information:
//

//  LS_Current,LS_Voltage,LS_InternalResistance
// History：
//
// ******************************************************/
// unsigned char Schedule_Manager_task::RecordLastStepData(unsigned char uc_ChannelNo)
// {
// 	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_Current].m_fValue
// 		= ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;

// 	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_Voltage].m_fValue
// 		= ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;

// 	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_InternalResistance].m_fValue
// 		= ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue;

// 	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_LS_ACR].m_fValue
// 		= ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_ACR].m_fValue;

// 	return 1;
// }
/*****************************************************
// Function Name:SetSetVaribale
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      改变Setvariable条件
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SetSetVariable(unsigned char uc_ChannelNo)
{
    unsigned char ucActiveIdx				=		ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Un_CtrlValues * p_Ctrl_Values		=		&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues;
    unsigned long uw32_Reset        =		p_Ctrl_Values->m_fControlValue[0];
    unsigned long uw32_Increment    =		p_Ctrl_Values->m_fControlValue[1];
    unsigned long uw32_Decrement    =		p_Ctrl_Values->m_fControlValue[2];

    //Increment for check if copy last cycle metas//151120,add by hepeiqing,for LC_DischargeCapacity as 0 debug
    if(GetSetVariable(uw32_Increment, MP_MV_INC_CHAN_CYCLE_INDEX) == 1)       //CycleIndex
    {
//         EthernetParse::MPReportData(uc_ChannelNo,SEND_LOG_SDL_CYCLE);  // 20170919 yy  //mits8
        ChannelInfo_Manager::Copy_PVmeta2LCmeta(uc_ChannelNo);
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_VmaxOnCycle].m_fValue = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
//         ChannelInfo_Manager::ResetNormalMV(uc_ChannelNo,MetaCode_PV_ChargeCapacityTime);    //zyx 171010
//         ChannelInfo_Manager::ResetNormalMV(uc_ChannelNo,MetaCode_PV_DischargeCapacityTime);
    }

    //Reset
    for(unsigned char reset_type = MP_MV_RESET_CHAN_CHARGE_CAP; reset_type < MP_NUM_OF_META_VAR_RESET_VALUE; reset_type++)
    {
        if(GetSetVariable(uw32_Reset, reset_type) == 1)
            ChannelInfo_Manager::Reset(uc_ChannelNo, reset_type);
    }

    //Increment
    for(unsigned char incre_type = MP_MV_INC_CHAN_CYCLE_INDEX; incre_type < MP_NUM_OF_META_VAR_INC_VALUE; incre_type++)
    {
        if(GetSetVariable(uw32_Increment, incre_type) == 1)
            ChannelInfo_Manager::Increment(uc_ChannelNo, incre_type);
    }

    //Decrement
    for(unsigned char decre_type = MP_MV_DEC_COUNTER_COUNTER1; decre_type < MP_NUM_OF_META_VAR_DEC_VALUE; decre_type++)
    {
        if(GetSetVariable(uw32_Decrement, decre_type) == 1)
            ChannelInfo_Manager::Decrement(uc_ChannelNo, decre_type);
    }
}

/*****************************************************
// Function Name:SetSetValue
// Version:        V1.0
// Date:                2017/11/09
// Description:
//                      改变Setvalue条件
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SetSetValue(unsigned char uc_ChannelNo) //171109 zyx
{
    unsigned char ucActiveIdx	=	ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_Ctrl        =	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    unsigned char DesMetaCode   =	(unsigned char)p_Ctrl->m_CtrlValues.m_fControlValue[0];
    float fValue                =	p_Ctrl->m_CtrlValues.m_fControlValue[1];
    unsigned char Ctrltype      =   p_Ctrl->m_ucCtrlValue_DataType[1];
    unsigned char Operator = p_Ctrl->m_CtrlValueOperator[1];
    float fCoefficient = p_Ctrl->m_CtrlValueCoefficients[1];// 20190917 yy
    float fRetValue = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype,Operator,fCoefficient, fValue);

    if((DesMetaCode >= MetaCode_MV_UD1 && DesMetaCode <= MetaCode_MV_UD4)\
            || (DesMetaCode >= MetaCode_MV_UD5 && DesMetaCode <= MetaCode_MV_UD16))
    {
        if(fRetValue != ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][DesMetaCode].m_fValue)
            ChannelInfo_Manager::uc_MV_Udx_Update[uc_ChannelNo] = 1;
    }

    ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, DesMetaCode, fRetValue);
    Step_Manager_task::bStepLimitChecked[uc_ChannelNo] = 1;
    IVSDL::SetLogMVUDFlag(uc_ChannelNo, DesMetaCode);
}
/*****************************************************
// Function Name:SendSDLIdleData
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      发送闲时数据
// Others:
//
// transitional information:
//                时间按配置设定值处理

//
// History：
//
// ******************************************************/
//void Schedule_Manager_task::SendSDLIdleData(unsigned char uc_ChannelNo,unsigned long long TimeInterval)
void Schedule_Manager_task::SendSDLIdleData(unsigned char uc_ChannelNo)
{
// 	unsigned char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH 状态定阅报告 断网计算器计数。
    {
        //ScheduleStatusReport::uc_uc_BreakNet_Counter = 2*BREAKNET_NUMBER;
        return;      // yy 20141030  Fix buffer overflow problem bug.
    }
#endif
    //    #if SUPPORT_PARALLEL == 1
    if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        if (!ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo))
            return;
    //   #endif

    unsigned long long uw64_IntervalTime = 1000;
    unsigned long long TimeInterval = ChannelInfo_Manager::BoardCFG.m_Global.m_fSdlTime;
    if(EthernetParse::uw64_CTI_SetTimeInterval < (TimeInterval*10000))
    {
        if(EthernetParse::uw64_CTI_SetTimeInterval != NORMAL_UPDATA_DIPLAY)
        {
            uw64_IntervalTime = EthernetParse::uw64_CTI_SetTimeInterval;
            TirggerIDLECounter[uc_ChannelNo]=TimeInterval*10;
        }
    }
//     SDLTimeTicksForIdle[uc_ChannelNo] = OS_ServeProcess::TimeDiff(SDLIdleMsTime[uc_ChannelNo]);
//     if(SDLTimeTicksForIdle[uc_ChannelNo].Us100>1000)//TimeInterval)
    if(CommonFunc::GetTimeTicks(&SDLIdleMsTime[uc_ChannelNo]) >= uw64_IntervalTime)   // 20220421 yang
    {
        //		if(USE_SDL)
        TirggerIDLECounter[uc_ChannelNo]++;
// 		BuiltInSDL_TriggerCounter[uc_ChannelNo]++;

// 		 if(BuiltInSDL_TriggerCounter[uc_ChannelNo]/10>=ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlTime && ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlTime>0)  //dirui add for builtin SDL
// 		 {
// 			 if(IVSDL::Valid)
// 			 IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo,SDL_DATA_TYPE_LOG);
// 			 BuiltInSDL_TriggerCounter[uc_ChannelNo]=0;
// 		 }

        if(TirggerIDLECounter[uc_ChannelNo] / 10 >= TimeInterval)
        {

            IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_IDLE, LOG_NORMAL_DATA_POINT);
// 		if(IVSDL::Valid)
// 		{
            IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //循环发送 xS
// 		}
            //SDL::TriggerIdle((unsigned short)uc_ChannelNo);
            //else
            //    EthernetParse::MPReportData(uc_ChannelNo,SEND_IDLE_DATA);
//       if(ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum > 0)
            if(CANBusParse::CANBMS_bWork)
                EthernetParse::MPReportData(uc_ChannelNo, SEND_IDLE_BMS_DATA);
            TirggerIDLECounter[uc_ChannelNo] = 0;
        }
        else if(!IVSDL::Valid)
            IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //循环发送 100ms

        CommonFunc::GetTimeTicksUpdate( &SDLIdleMsTime[uc_ChannelNo]);
// 		SDLIdleMsTime[uc_ChannelNo].Us100 = OS_ServeProcess::OS_Time.Us100;
    }
}
/*****************************************************
// Function Name:SendSDLShowData
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      发送显示数据
// Others:
//
// transitional information:
//                间隔3s

//
// History：
//
// ******************************************************/
//void Schedule_Manager_task::SendSDLShowData(unsigned char uc_ChannelNo,unsigned long long TimeInterval)
void Schedule_Manager_task::SendSDLShowData(unsigned char uc_ChannelNo)
{
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH 状态定阅报告 断网计算器计数。
        return;    // yy 20141030  Fix buffer overflow problem bug.
#endif

    if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        if (!ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo))
            return;

//    TimeInterval = MCU_SET_META_VARIABLE_INTERVAL_TIME;// 500ms
// 	if(uw64_DiplayTime_100us != MCU_SET_META_VARIABLE_INTERVAL_TIME)
//		   TimeInterval = uw64_DiplayTime_100us;

// 	SDLTimeTicksForShow[uc_ChannelNo] = OS_ServeProcess::TimeDiff(SDLShowMsTime[uc_ChannelNo]);
// 	if(SDLTimeTicksForShow[uc_ChannelNo].Second>TimeInterval)
//    unsigned long long DiffTick_100us = CommonFunc::GetTimeTicks(&SDLShowMsTime[uc_ChannelNo]);
//    if(DiffTick_100us > TimeInterval)

//	SDLTimeTicksForShow[uc_ChannelNo] = OS_ServeProcess::TimeDiff(SDLShowMsTime[uc_ChannelNo]);
// 	if(SDLTimeTicksForShow[uc_ChannelNo].Second>TimeInterval)
    if(CommonFunc::GetTimeTicks(&SDLShowMsTime[uc_ChannelNo]) >= EthernetParse::uw64_CTI_SetTimeInterval)   // 20220421 yang
    {
        IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_DISPLAY, LOG_NORMAL_DATA_POINT);
        IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //循环发送
        //if(ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum > 0)
        if(CANBusParse::CANBMS_bWork)
            EthernetParse::MPReportData(uc_ChannelNo, SEND_IDLE_BMS_DATA);

        if(Smart_Battery::BeSupportSmbus)
        {
            EthernetParse::MPReportData(uc_ChannelNo, SEND_IDLE_SMB_DATA_FLOAT);
            EthernetParse::MPReportData(uc_ChannelNo, SEND_IDLE_SMB_DATA_STRING);

        }

//         SDLShowMsTime[uc_ChannelNo].Second = OS_ServeProcess::OS_Time.Second;
//         SDLShowMsTime[uc_ChannelNo].Us100 = OS_ServeProcess::OS_Time.Us100;
        CommonFunc::GetTimeTicksUpdate( &SDLShowMsTime[uc_ChannelNo]);
    }
}
/*****************************************************
// Function Name:SofeWare_WatchDog
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      软件狗，处理上位丢失问题
// Others:
//
// transitional information:
//               处理条件：1分钟没有响应则强行关闭通道

//
// History：
//
// ******************************************************/
void Schedule_Manager_task::SoftWare_WatchDog(void)   //20130904,modified by hepeiqing
{
    if(EthernetParse::m_bResetSchedule)
    {
        for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
        {
            if(IfSchRunning(uc_i))           //运行的通道要执行stop动作，不必考虑各种回复，上位此时已经挂了
            {
                if(ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus == GOSTOP)
                    continue;
                ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus = GOSTOP;
                //SendFinishFlag[uc_i].StopStatus = STOP_MANUAL;    // Comment out by DKQ on 09.11.2013
                SendFinishFlag[uc_i].StopStatus = STOP_EMERGENCY;  // Add by DKQ on 09.11.2013
                SendFinishFlag[uc_i].SubStopCode = EthernetParse::m_bResetSchedule;

                SchTimeTrigger[uc_i] = 0; //清超时标志位
            }
        }
        EthernetParse::m_bResetSchedule = 0;
    }
}
/*****************************************************
// Function Name:IRspecial_Judge
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      内阻判断处理
// Others:
//
// transitional information:
//

//
// History：
//
// ******************************************************/
unsigned char Schedule_Manager_task::IRspecial_Judge(unsigned char uc_ChannelNo)
{
    unsigned char NoActive = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    if(IRSpecialTestFlg[uc_ChannelNo])
        return 0;

    switch(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[NoActive].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
    {
// 	case CT_IVCH_CURRENT:
// 	case CT_IVCH_CRAMP:
// 	case CT_IVCH_CRATE:
// 	case CT_IVCH_CSTAIRCASE:
// 	case CT_IVCH_POWER:
// 	case CT_IVCH_LOAD:
// 	case CT_IVCH_PRAMP:
// 	case CT_IVCH_PSTAIRCASE:
// 	case CT_IVCH_IR:
// 	case  CT_IVCH_ACIR:
// 	case CT_IVCH_SIMU_I: //12.22.2013ADD
// 	case CT_IVCH_SIMU_V: //12.22.2013ADD   //We Do Not Offer Voltage Simulation
// 	case CT_IVCH_SIMU_P: //12.22.2013ADD
// 	case CT_IVCH_SIMU_L: //12.22.2013ADD
// 	case CT_IVCH_DIGII:
// 	case CT_IVCH_DIGIV:     //04.23.2013
// 	case CT_IVCH_CCCV:
// 	case CT_IVCH_CCCV_TB: //TURBO BOOST BY ZC 07.07.2017
// 	case CT_IVCH_C_CV:
// 	case CT_IVCH_V_CV:
// 		return 1;
    case CT_IVCH_REST:
    case CT_IVCH_SETVARIABLE:
    case CT_IVCH_SETVALUE: //171109 zyx
    case CT_IVCH_ACS:
    case CT_IVCH_VOLTAGE:
    case CT_IVCH_SIMU_Battery: //190218 zyx

    case CT_IVCH_VRAMP:
    case CT_IVCH_VSTAIRCASE:
    case CT_IVCH_CPULSE:
    case CT_TYPE_DO_SETTING:
    case CT_IVCH_EXTCHARGE:
        return 0;
    //...
    default:
        return 1;
    }
}

void Schedule_Manager_task::SchReportStartScheduleUnsafeMsgToPC(unsigned char uc_ChannelNo, St_ScheduleMessage * UnsafeInfo)// 200420 zyx
{
    memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
    EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //??????
    EthernetParse::m_ReportStatus.m_btSCHEvent_Source = S_GOSTOP_ONUNSAFEMSG;
    EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeInfo->st_status_sch_running;
    EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = ST_SCHMANAGE;
    EthernetParse::MPReportData(uc_ChannelNo, SEND_EVENT_CONTENT);
}

void Schedule_Manager_task::SchReportMessageToPC(unsigned char uc_ChannelNo, unsigned char CMD, St_ScheduleMessage * UnsafeContent)
{
    memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
    EthernetParse::m_ReportStatus.m_btCMD = CMD;        //设定回复信息
    EthernetParse::m_ReportStatus.m_btSCHEvent_Source = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source;
    if(CMD == PC_Jump_Msg)
    {
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg信息必需先存跳转信息
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID 在Parse中要添加
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
    }
    else if((CMD == PC_StopRpt_Msg) || (CMD == PC_Unsafe_Msg) || (CMD == PC_Error_Msg) || (CMD == PC_GoStop_Msg))
    {
        UnsafeContent->st_status_sch_running.m_bUnsafeType = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType;
        UnsafeContent->st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //回复的Safety状态(暂时放在大数据体channelinfo)
        EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent->st_status_sch_running;
//        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source == S_GOSTOP_STOPPEDMSG)    //dirui 20161128
//            memset((void *)&UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));//dirui 20161128
    }
    EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source = ST_SCHMANAGE;
    EthernetParse::MPReportData(uc_ChannelNo, SEND_EVENT_CONTENT);

}

void Schedule_Manager_task::IRspecial_Judge_Process(unsigned char uc_ChannelNo)
{
    unsigned char NoActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    IR::IRSpecialGotoNext[uc_ChannelNo] = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
    ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_ItemCount.m_u16StepCount);//zyx 2105291
    IR::IRSpecialRange[uc_ChannelNo] = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[NoActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[NoActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = IR::IRSpecialRange[uc_ChannelNo];
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[NoActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = 0.1f * ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity); //?Y瓯???a0.1C
    Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] = 1;
}


//#if SUPPORT_AUX==1
//AUX添加相关代码///////////////////////////////////////////////////////////////////////
/*****************************************************
// Function Name:SendSafetyInfoToAUX
// Version:        V1.0
// Date:                2013/11/20
// Description:
//                     发送安全信息到辅助通道
// Others:
//
// transitional information:
//
//
// History：
//
// ******************************************************/

void Schedule_Manager_task::SendSafetyInfoToAUX(unsigned char uc_ChannelNo)
{

    SendAUX_SafetyFlg[uc_ChannelNo] = 0;
//    float fHigh = 0, fLow = 0;
    St_SaftyCheckItem   MySafty;
    unsigned char m_bEnabled = 0;
    unsigned short int p_Total_Aux_Unit = 0;
    unsigned char Auxtype = ALLTYPE;

//    St_ScheduleSetting * p_SchSetting = &ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings;
    for(unsigned char uc_Safe_Type = MP_SCH_SAFTY_TYPE_AuxiVoltage; uc_Safe_Type < MP_SCH_SAFTY_TYPE_TOTAL; uc_Safe_Type ++)
    {
        Auxtype = ChannelInfo_Manager::AuxSaftyTypeChangeAuxChannelType(uc_ChannelNo,uc_Safe_Type,&MySafty);
        if(Auxtype != ALLTYPE)
        {
            m_bEnabled = 0;
            p_Total_Aux_Unit = Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit;
            ChannelInfo_Manager::GetScheduleSafty(uc_ChannelNo, uc_Safe_Type, &MySafty);
            m_bEnabled = MySafty.m_bIfCheck;
            if(m_bEnabled && (p_Total_Aux_Unit > 0))
            {
                SendAUX_SafetyFlg[uc_ChannelNo] |= (1 << Auxtype);//(MP_SCH_SAFTY_TYPE_AuxiVoltage + uc_type));
                CANBusParse::IV_SendSafetyInfoToAUX(uc_ChannelNo,
                                                    Auxtype,
                                                    MySafty.m_fHigh,
                                                    MySafty.m_fLow,
                                                    uc_Safe_Type);
            }
        }
    }
//        if((p_SchSetting->m_SchAux_Glb_DI.m_bEnabled)
//                && (Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_DI].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
//                && (uc_Safe_Type == MP_SCH_SAFTY_TYPE_AuxiDI))//zyx 121215
//        {

//            int AuxIndex = p_SchSetting->m_SchAux_Glb_DI.m_SafetyStartIndex;
//            fHigh = p_SchSetting->m_AuxSafty[AuxIndex].m_fMax;
//            fLow = p_SchSetting->m_AuxSafty[AuxIndex].m_fMin;
//            Auxtype = AUX_DI;
//            SendAUX_SafetyFlg[uc_ChannelNo] |= (1 << Auxtype);
//        }

//        if((p_SchSetting->m_SchAux_Glb_Humidity.m_bEnabled)
//                && (Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_HUMIDITY].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
//                && (uc_Safe_Type == MP_SCH_SAFTY_TYPE_AuxiHumidity))
//        {

//            int AuxIndex = p_SchSetting->m_SchAux_Glb_Humidity.m_SafetyStartIndex;
//            fHigh = p_SchSetting->m_AuxSafty[AuxIndex].m_fMax;
//            fLow = p_SchSetting->m_AuxSafty[AuxIndex].m_fMin;
//            // 		uc_SpecialPara=0;
//            Auxtype = AUX_HUMIDITY;
//            SendAUX_SafetyFlg[uc_ChannelNo] |= (1 << Auxtype);//MP_SCH_SAFTY_TYPE_AuxiHumidity);
//        }

////         if((p_SchSetting->m_SchAux_Glb_ExtCurrent.m_bEnabled)
////                 && (Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
////                 && (uc_Safe_Type == MP_SCH_SAFTY_TYPE_AuxiExternalCharge))
//        p_Aux_Glb = &p_SchSetting->m_SchAux_Glb_SafetyVoltage;
//        p_Total_Aux_Unit = &Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_SAFETY].m_AuxUnitCfg.m_Total_Aux_Unit;
//        for(unsigned char uc_type=0; uc_type<3; uc_type++)   //     AUX_SAFETY,AUX_ACIM,AUX_BATTERYCELLS,                       //"BATTERYCELLS"
//        {
//            //p_Aux_Glb +=uc_type;
//            p_Total_Aux_Unit = &Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_SAFETY].m_AuxUnitCfg.m_Total_Aux_Unit;
//            if((p_Aux_Glb->m_bEnabled)
//                    && (p_Total_Aux_Unit > 0)
//                    && (uc_Safe_Type == (MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage+uc_type)))
//            {
//                int AuxIndex = p_Aux_Glb->m_SafetyStartIndex;
//                // 			AUX_SAFETY,
//                // 			ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafetyVoltage.m_fMax,
//                float * p_fMin = &p_SchSetting->m_AuxSafty[AuxIndex].m_fMin;
//                fLow = * p_fMin;
//                fHigh = * (++p_fMin);
//                uc_SpecialPara = 1+uc_type;
//                Auxtype = AUX_SAFETY;
//                SendAUX_SafetyFlg[uc_ChannelNo] |= (1 << Auxtype);//(MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage + uc_type));
//            }
//						p_Aux_Glb ++;
//        }


//        if(Auxtype != ALLTYPE)
//            CANBusParse::IV_SendSafetyInfoToAUX(uc_ChannelNo,
//                                                Auxtype,
//                                                St_SaftyCheckItem.m_fHigh,
//                                                St_SaftyCheckItem.m_fLow,
//                                                uc_SpecialPara);
}
void Schedule_Manager_task::SendScheduleMapToAUX(unsigned char uc_ChannelNo)
{
    for(int AuxTypeIndex = 0;  AuxTypeIndex < MAXAUXTYPE; AuxTypeIndex++)
    {
        CANBusParse::IV_SendSchMapToAUX(uc_ChannelNo, AuxTypeIndex);
    }
}
// void Schedule_Manager_task::SendSDLParaToAUX(unsigned char uc_ChannelNo)
// {
//     //修改ing
// //     unsigned char FactorType ;
//     float AuxFactor_META = 1;
//     unsigned char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
//     float AuxFactor_TIME = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_DV_Time_Max;

//     // 根据map信息查找AuxType
//     for(unsigned char uc_Auxtype = 0; uc_Auxtype < MAXAUXTYPE; uc_Auxtype++) //只在乎发送的辅助类型，不关心多少个通道
//     {
//         if(Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][uc_Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
//             continue;

//         switch(uc_Auxtype)//171113,hpq,add for pro8
//         {
//         case AUX_VOLTAGE:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Voltage.m_fDelta;
//             break;
//         case AUX_TEMPERATURE:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Temperature.m_fDelta;
//             break;
//         case  AUX_PRESSURE:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Pressure.m_fDelta;
//             break;
//             // 			case  AUX_PH:
//             // 				AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlFactor_AuxPhValue;
//             // 				break;
//         case AUX_EXTERNALCHARGE:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_ExtCurrent.m_fDelta;
//             break;
//         case AUX_FLOW_RATE:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_FlowRate.m_fDelta;
//             break;

//         case AUX_HUMIDITY:
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Humidity.m_fDelta;		//湿度变量
//             break;

//             //case  AUX_DENSITY :
// //         case AUX_DI :
// //         case AUX_DO:
// //             AuxFactor_META = 1;
// //             break;
//         default:
//             break;
//         }
//         for(unsigned char uc_Auxch = 0; uc_Auxch < Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][uc_Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit; uc_Auxch++)
//         {
// //             FactorType = LOG_DELTA_META;   //dxdt original data 先不考虑
// //             CANBusParse::IV_DownloadSDLToAUX(uc_ChannelNo,
// //                                              uc_Auxtype,
// //                                              FactorType,
// //                                              AuxFactor_META,
// //                                              Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][uc_Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_i],
// //                                              uc_i);
// //             FactorType = LOG_DELTA_TIME;

//             //	AuxFactor_TIME = 10.0f;   //set default as 10S
// //             AuxFactor_TIME = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_DV_Time_Max;//171113,hpq,add for pro8
//             for(unsigned char uc_m=0; uc_m<2; uc_m++)
//             {
//                 CANBusParse::IV_DownloadSDLToAUX(uc_ChannelNo,
//                                                  uc_Auxtype,
//                                                  (LOG_DELTA_META-uc_m),
//                                                  (uc_m>0?AuxFactor_TIME:AuxFactor_META),
//                                                  Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][uc_Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_Auxch],
//                                                  uc_Auxch);
//             }
//         }
//     }
// }

void Schedule_Manager_task::AuxOnUnsafeMsgRoutine(unsigned char uc_ChannelNo)
{
    //辅助安全异常处理
    //	UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;
//     memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;
    UnsafeContent[uc_ChannelNo].m_btCMD = PC_Unsafe_Msg;
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source = ST_CANPARSE;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btSubCode = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running.m_btSubCode;
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_AUXUNSAFE;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //当前信息处理完毕要清除
}

void Schedule_Manager_task::ClearMapAUXSafetyInfo(unsigned char uc_ChannelNo, unsigned char Auxtype, unsigned char VirtualID, unsigned char ClearType)
{
    //根据Map 表发清除相关安全信息
    if(Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_DO].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
    {
        CANBusParse::IV_SendDigitOut(uc_ChannelNo, 0);
        Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus = 0;
    }
    if(Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][AUX_AO].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
    {
        CANBusParse::IV_SendAnalogSingnalOut(uc_ChannelNo, 0);
    }
    if(Auxtype == ALLTYPE)
    {
        for(unsigned char uc_i = 0; uc_i < MAXAUXTYPE; uc_i++)
        {
            //if(ChannelInfo_Manager::m_AUXmap[uc_ChanenlNo][Auxtype].m_AcutalTotalAuxUnit>0)
            if(Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][uc_i].m_AuxUnitCfg.m_Total_Aux_Unit > 0)
                CANBusParse::IV_ClearAuxSafetyInfo(uc_ChannelNo, uc_i, VirtualID, ALLMAPCHANNEL);  //add by dirui 20131209
        }
        return;
    }
    if(Channel_MapAuxInfo::m_AUXmap[uc_ChannelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)

        return;

    if(ClearType)
        CANBusParse::IV_ClearAuxSafetyInfo(uc_ChannelNo, Auxtype, VirtualID, ALLMAPCHANNEL);
    else
        CANBusParse::IV_ClearAuxSafetyInfo(uc_ChannelNo, Auxtype, VirtualID, SINGLECHANNEL);
}
int Schedule_Manager_task::ClearAUXLimits(unsigned char uc_ChannelNo, unsigned char Clear4New)
{
    //该函数确实STEP ID；
    unsigned short StepID = 0;
    Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].bNeedWattingAck = 1;
    CANBusParse::IV_ClearLimitsToAUX(uc_ChannelNo, Clear4New, StepID);	//清除AUX相关Limits
    return 0;
}
void Schedule_Manager_task::SchWaitForAuxRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;

    if (SchTimeTrigger[uc_ChannelNo] == 0) //Limit 表再LoadStep同时检查，这里已经查过并清除了 所以不再查了dirui 20131220
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule计时标记
    }

    //查表操作，检查safety应答表
    if(CANBusParse::IV_CheckAuxSNRes(uc_ChannelNo, SAFETYSN))
    {
        SchTimeTrigger[uc_ChannelNo] = 0;
        CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, SAFETYSN); //检查完毕要清除
        StepMessage.MsgNumber = Step_Run_Msg;
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);                                     //向step传递信息Step_Run_Msg
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING;
        Step_Manager_task::TimeRefreshFlag[uc_ChannelNo] = 0;
//        SendTirgger[uc_ChannelNo] = 0;
        return;
    }
//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)    //Aux_OverTime)  //超时？
    {
        St_ScheduleMessage SchMessage;
        memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
        SchTimeTrigger[uc_ChannelNo] = 0;       //等待到了没有把表对上

        for(unsigned char uc_i = 0; uc_i < CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNCount; uc_i++)
        {
            if(!CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNInfo[uc_i].m_Checked)
            {
                SchMessage.st_status_sch_running.m_btLost_Unit = CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNInfo[uc_i].m_AuxUnitID;
                break;
            }
        }

        CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, SAFETYSN); //检查完毕要清除
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = AUX_UNSAFE_TYPE;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = AUX_ACK_FAILED;
        safety_check::SetUnsafe(uc_ChannelNo,AUX_ACK_FAILED,0, AUX_UNSAFE_TYPE);
        SchMessage.MsgNumber = AUX_Unsafe_Msg;
        SchMessage.st_status_sch_running.m_btCMD_Source = ST_CANPARSE;
        SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_ACK_FAILED;
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_AUXACKFAIL;  //这个要同PC 商量用什么定义上报
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_AUXSafetyStatus = AUX_ACK_FAILED;
        Schedule_Manager_task::UnsafeContent[uc_ChannelNo] = SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage);

    }

}


void Schedule_Manager_task::UseTemperatureChamber( unsigned char uc_channel, unsigned char AuxUnitIdx, unsigned char AuxChnNo ) //占用温控箱  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].usingFlag |= 0x01<<AuxChnNo;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].uc_channel[AuxChnNo] = uc_channel;
}


void Schedule_Manager_task::RelsaseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ) //释放温控箱的占用  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].usingFlag &= ~(0x01<<AuxChnNo);
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].uc_channel[AuxChnNo] = 0xFF;
}

void Schedule_Manager_task::OpenTemperatureChamber( unsigned short m_uwAuxVirtIndx, unsigned char m_ucType, unsigned char AuxUnitIdx, unsigned char AuxChnNo) //记录温控箱开状态 20160707  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].OpenFlag |= 0x01<<AuxChnNo;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_ucType[AuxChnNo] = m_ucType;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_uwAuxVirtIndx[AuxChnNo] = m_uwAuxVirtIndx;

}


void Schedule_Manager_task::CloseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo )   //记录温控箱关状态 20160707  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].OpenFlag &= ~(0x01<<AuxChnNo);
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_ucType[AuxChnNo] = 0xFF;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_uwAuxVirtIndx[AuxChnNo] = 0xFFFF;
}

void Schedule_Manager_task::ScheduleToStopIdle(unsigned char uc_ChannelNo)
{
    if(Step_Manager_task::bNeedCheck[uc_ChannelNo]
            ||ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_LimitCount != 0
            ||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus != STEPIDLE)  //20180420 LOG数据依然继续的BUG
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=GOSTOP;
        //SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = IV_UNSAFE_TYPE;//0xFF;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = 0;
        //SchReportMessageToPC(uc_ChannelNo,PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_EMERGENCY;
        SendFinishFlag[uc_ChannelNo].SubStopCode = eEMERGENCYSTOP_ERROR_SchOverTime;
        IVSDL::LimitArrivedRoutine(uc_ChannelNo);
        SchTimeTrigger[uc_ChannelNo]=0;  //接下来要转到Gostop状态，执行向Step发送Stop信息的处理
        Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_LimitCount = 0;
    }
}

void  Schedule_Manager_task::Sch_BuiltinAuxSafety_Load(unsigned char uc_ChannelNo)
{
    for(unsigned char uc_i=0; uc_i<2; uc_i++)
    {
        St_SchAux_Glb * p_AuxGlb = &ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage;
        p_AuxGlb += uc_i;
        if(p_AuxGlb->m_bEnabled)
        {
            int AuxIndex = p_AuxGlb->m_SafetyStartIndex;
            St_SaftyCheckItem * p_saftyItem = &safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V;
            p_saftyItem += uc_i;
            p_saftyItem->m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
            p_saftyItem->m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
            p_saftyItem->m_bIfCheck |= (1 << uc_i);
            safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << uc_i);
        }
    }
}

void  Schedule_Manager_task::SDLIdeShowMsTime_Updata(void)
{
    for(UChar8  uc_Chan_Idx = 0; uc_Chan_Idx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_Chan_Idx ++)
    {
        CommonFunc::GetTimeTicksUpdate( &Schedule_Manager_task::SDLIdleMsTime[uc_Chan_Idx]);
        CommonFunc::GetTimeTicksUpdate( &Schedule_Manager_task::SDLShowMsTime[uc_Chan_Idx]);
    }
    CommonFunc::GetTimeTicksUpdate( &IVSDL::m_IntervalTime_SDL);
    CommonFunc::GetTimeTicksUpdate( &EthernetParse::DataTxCounter);
}
//#endif
