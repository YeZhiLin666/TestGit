// *****************************************************
// File Name: Schedule.cpp
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule�������
// Others:
// History��
//
// ******************************************************/
#include "includes.h"

//��Ҫ��������
//��������:
//
//����: PCASSIGNOK
//

UInt16 Schedule_Manager_task::Priority = PRIORITY4;
UInt16 Schedule_Manager_task::TaskID = SCHEDULE_TASK;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
St_ScheduleMessage Schedule_Manager_task::ScheduleMessageBuf[MAXMESSAGE][MAXCHANNELNO];  //����ͨ��Schedule��Ϣ����
St_ScheduleMessage 	Schedule_Manager_task::UnsafeContent[MAXCHANNELNO];    //�쳣��Ϣ����
St_DoMessage       	Schedule_Manager_task::DoMessageBuf[MAXCHANNELNO];    //������ǰ�������Ϣ����
St_ScheduleMessage  Schedule_Manager_task::StartScheduleUnsafe[MAXCHANNELNO];//200420 zyx
#pragma arm section

bool Schedule_Manager_task::IsNeedSetStopPoint[MAXCHANNELNO];

unsigned char       Schedule_Manager_task::bMetaSyncSend[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::SchTimeTrigger[MAXCHANNELNO];   //��ʱ������ڳ�ʱ
St_MsgBufManage    	Schedule_Manager_task::MsgBufManage[MAXCHANNELNO];
MsTime             	Schedule_Manager_task::SysTimeEngineTask[MAXCHANNELNO];
//MsTime 		      	Schedule_Manager_task::SysTimeSendTask[MAXCHANNELNO];
MsTime            	Schedule_Manager_task::MsTimeEngineTask[MAXCHANNELNO];
unsigned short     	Schedule_Manager_task::EngineHandle[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::EngineOverTimeTriger[MAXCHANNELNO];
//unsigned char 	   	Schedule_Manager_task::SendTirgger[MAXCHANNELNO];
unsigned char      	Schedule_Manager_task::SendGoStopFlag[MAXCHANNELNO];
StopMsg			      	Schedule_Manager_task::SendFinishFlag[MAXCHANNELNO];
//bool                Schedule_Manager_task::SCH_RESUME_Flag[MAXCHANNELNO];   //RESUME ������־  add by qjm 20141016
St_ResumeManage    	Schedule_Manager_task::ResumeManage[MAXCHANNELNO];
unsigned char	   	Schedule_Manager_task::LockModeSwitch[MAXCHANNELNO];   //�������ʱ������������ʱ�ж�״̬ 0901 dirui
unsigned char		Schedule_Manager_task::IRSpecialTestFlg[MAXCHANNELNO];
unsigned char       Schedule_Manager_task::IRHasBeenRun[MAXCHANNELNO];    //���IR�Ƿ��Ѿ������й���
//MsTime 			   	Schedule_Manager_task::UnsafeTimeTicksForFixed[MAXCHANNELNO];
//MsTime 			   	Schedule_Manager_task::UnsafeTimeTicksForDynamic[MAXCHANNELNO];
//MsTime 				Schedule_Manager_task::FixedUnsafeMsTime[MAXCHANNELNO];
//MsTime 				Schedule_Manager_task::DynamicUnsafeMsTime[MAXCHANNELNO];

//MsTime 			   	Schedule_Manager_task::SDLTimeTicksForIdle[MAXCHANNELNO];				//��ʾ/��ʱ ���ݷ��Ͷ�ʱ����
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
// bool  Schedule_Manager_task::b_start2resumeflag[MAXCHANNELNO];     //0: Start ���      1�� resume ����
// unsigned char  Schedule_Manager_task::M4WaitACKOvertimeFlag[MAXCHANNELNO]; //add by qjm 20180710
// unsigned char  Schedule_Manager_task::uc_StartOrRestartFlag;
// unsigned char  Schedule_Manager_task::M4ReceiveACKCnt;        //add by qjm 20180803
// unsigned char  Schedule_Manager_task::M4RestartSentCnt;   //�ܵ�������ѹ���ϸ������
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
//                         Schedule �����ʼ������
// Others:
//                //�����ʼ��
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
    //��ʼ���¿���Ϊ�ر�״̬  changed by zc 04/25/2018
    for(int auxUnitIdx = 0; auxUnitIdx < MAXMCUUNIT; auxUnitIdx++)
    {
        for(int auxChnNo = 0; auxChnNo < MAXMTCCHAMBERNO; auxChnNo++)
        {
            Schedule_Manager_task::CloseTemperatureChamber( auxUnitIdx, auxChnNo );
            Schedule_Manager_task::RelsaseTemperatureChamber( auxUnitIdx, auxChnNo );
        }
    }
//#endif

    //��ʼ����ر�־λ
// 	M4RestartSentCnt =0;
// 	M4ReceiveACKCnt =0;

}
/*****************************************************
// Function Name: DoWork
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule ����ִ�к���
// Others:
//                ������̣�����ʱ��Ƭ����ȡMsg��Ϣ������Schedule״̬��
// History��
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

    for(uc_channel = 0; uc_channel < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++) //ͨ��ѭ������Message
    {
        //        1��������Ϣ���У�һ��ȡ��ȫ����Message��Ϣ��
        //        2����ϢҪ��Ԥ������̣���Ϊ���ܶ����л��м�����Ϣ��
        //        3��״̬����һֱ�����еģ�����������Ϣ������Ӧ��message���ң�

        //	#if (SUPPORT_PARALLEL == 1)
        //if ((ChannelInfo_Manager::IsSlaveChannel(uc_channel)) || (ChannelInfo_Manager::IsInterUnitSubmasterChannel(uc_channel)))
        PreprocessMsg(uc_channel);
        if ( (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()) && (! ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_channel)))
        {
            //PreprocessMsg(uc_channel);  dirui del 2017/12/18
            PreprocessSlaveUnsafeMsg(uc_channel);
            //�趨��ͨ���˲�����  dirui 2020/12/21
            unsigned char m_GroupMastereID=ChannelInfo_Manager::FindMasterChanIdx(uc_channel);
            ChannelInfo_Manager::SetFilterStatus(uc_channel,ChannelInfo_Manager::GetFilterStatus(m_GroupMastereID));
            continue;
        }
        //	#endif  //support_parallel
        //��ϢԤ�������

        //PreprocessMsg(uc_channel); dirui del 2017/12/18

        //���������Ϣ������̵õ��Ĳ�����Ϣ�����DoMessageBuf��ִ��״̬���������û����ϢҲ����ִ��״̬��
        ProcessState(uc_channel);


    }
#if SOFTWARE_DOG_ENABLE == 1
    SoftWare_WatchDog();  //Ϊ��λû��Ӧ������������  0829 dirui �趨ʱ��60s���޸Ĳ��� ���Դ������β�Ӱ������
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
    // 		 //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
    // 			break;
    // 		case Sch_OnSafe_Msg:
    // 			//
    // 		   //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
    // 			break;
    // 		default:
    // 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
    // 			 break;
    // 	 }

    ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
}
/*****************************************************
// Function Name: PreprocessorMsg
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Schedule ��ϢԤ����
// Others:
//��ϢԤ������������ȡ����Message��������Ϣ���д���Ĺ��̡�
//�ⲿ�����ݷֳ������֣���һ����Ϣ���ˣ����Ϸ����߹�ʱ����Ϣ�������˵���
//�ڶ�������Ϣ�ϲ��������������ܳ��ֵĶ�����Ϣ�����ٲã�
//������ʤ������Ϣ���ɱ�����Schedule�������񣩵�״̬��������Ӧ����
// History��
//
// ******************************************************/
int Schedule_Manager_task::PreprocessMsg(unsigned char uc_ChannelNo)   //���³����ݵ������ã�����Ҫ�����ȼ�˳��������Ǽ򵥵�ֱ�������
{
    //    unsigned char 	Idx_First;
    unsigned char 	Idx_Msg;
    unsigned char 	MsgIndex;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];

    if(MsgBufManage[uc_ChannelNo].FullCount == 0)
        return -1;        //����Ϣ

    //����Ҫ�ж���Ϣ�Ƿ�Ϸ�;
    for(int Idx_Full = 0; Idx_Full < MsgBufManage[uc_ChannelNo].FullCount; Idx_Full++)
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[Idx_Full];
        switch(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].MsgNumber)
        {
        case Sch_JumpA_Msg:
            if(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].FromStepID != ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)
                DeleteScheduleMsg(uc_ChannelNo, Idx_Full);
            if(ScheduleMessageBuf[MsgIndex][uc_ChannelNo].StepID > ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_ItemCount.m_u16StepCount)
                DeleteScheduleMsg(uc_ChannelNo, Idx_Full); //ɾ����Ϣ��������������Ϣ
            break;
        default:
            break;
        }
    }
    //�������ȼ�ɨ��
    //���ȼ�˳�� ����:   |        0        |      1     |     2     |     2       |      2      |      3       |      3       |     3      |
    //                     | Sch_OnUnsafe_Msg|Sch_Stop_Msg|Sch_Run_Msg|Sch_Pause_Msg|Sch_JumpM_Msg|Sch_Resume_Msg|Sch_Assign_Msg|Sch_JumpAMsg|
    //             ����  |                                                     4                                                                                                                                                        |
    //                     |Sch_LimitArrive_Msg|Sch_LimitPropertyChange_Msg|Sch_LimitStatusChanged|Sch_Started_Msg|Sch_Stopped_Msg|Sch_OnSafe_Msg|

    if(p_DoMesBuf->Used == 1)          //û��Sch_Stop_Msg��ôҪ����Message�Ƿ�������������һ������
        return 1;            //Messageû�д����꣬��ȥȡ����Ϣ

    if (MsgBufManage[uc_ChannelNo].FullCount == 1) //ֻ��һ��
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount - 1];
        p_DoMesBuf->ScheduleMessage = ScheduleMessageBuf[MsgIndex][uc_ChannelNo];
        p_DoMesBuf->Used = 1;
        //MsgBufManage[uc_ChannelNo].FullCount=0;  // ��ն���
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

    DeleteScheduleMsg(uc_ChannelNo, Idx_Use); //ɾ��ȡ����ϵ���Ϣ

    //ɾ�����ȼ���Ϊ4����Ϣ
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
//                         Schedule ״̬������
// Others:
//                        Schedule״̬��
//        NOTREADY / IDLE / START / RUNNING / GOPAUSE / RESUME / GOSTOP
// History��
//
// *****************************************************/
void Schedule_Manager_task::ProcessState(unsigned char uc_ChannelNo)
{
    //���ڵ�ǰ�ĸ���״̬���д���ִ��״̬������̣���Ӧ���ת��״̬
    unsigned char uc_SchStatus = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus;               //��ǰSchedule��״̬
    unsigned char MessageNumber = DoMessageBuf[uc_ChannelNo].ScheduleMessage.MsgNumber;      //ȡ�õ�ǰ��Ӧ��Ϣ���

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
        //NOTREADY ����Ŀǰִֻ�����������Ϸ�
        //hepeiqing add 20130828, idle data should available in not ready statusmachine,in case the
        SendSDLShowData(uc_ChannelNo);  //��ʱ����һ���Ϸ�����
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
        break;
    case IDLE:              //IDLE��ӦSch_Assign_Msg��Sch_Run_Msg    ��    Sch_Resume_Msg��Sch_OnUnsafe_Msg��
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
            SendSDLShowData(uc_ChannelNo);  //��ʱ����һ���Ϸ�����
            ScheduleToStopIdle(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
            break;
        }
        break;
// 	case	WAITPowerReady:
// 		switch(MessageNumber)
//     {
// 			case Sch_Stop_Msg:
// 			     SchStopMsgRoutine(uc_ChannelNo);
// 			     SchTimeTrigger[uc_ChannelNo]=0;  //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
// 			     break;
// 			case Sch_OnUnsafe_Msg:
// 				   p_Info->m_Event_Source=S_START_ONUNSAFEMSG;
// 			     SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal��Ϣ�������Ҫת��IDLE״̬
// 			     p_Info->ScheduleStatus=FIXEDUNSAFE;
// 			    SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

// 					//SchReportMessageToPC(uc_ChannelNo,PC_StopRpt_Msg, &UnsafeContent[uc_ChannelNo]);
// 			     break;
// // 				case Sch_JumpM_Msg:           //��ʱ����ǣ����������?
// // 					 if(CommonFunc::uc_Support_AUX)
// // 			        ClearAUXLimits(uc_ChannelNo,0);
// 			default:
// 				  SchWaitRoutine(uc_ChannelNo);
// 			    break;
//     }
//
//         break;
    case START:            //START��ӦSch_Stop_Msg��Sch_Pause_Msg��Sch_JumpM_Msg��Sch_LimitPropertyChange_Msg��Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            SchStopMsgRoutine(uc_ChannelNo);
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            break;
        //                 case Sch_Pause_Msg:        //�������Ϣ���벿�ִ���ʱ��������Msg��Step.Mgr����ͬʱת��GOPAUSE״̬��������RUNNING״̬
        //                         ChannelInfo_Manager::LoadNextStep(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID); //stepװ�ص�����������������ӿ�
        //                         p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
        //                         p_Info->ScheduleStatus=GOPAUSE;
        // 						#if SUPPORT_AUX==1
        // 						ClearAUXLimits(uc_ChannelNo,0);
        // 						#endif
        //                         //��ϢҪ�Ŷ����Բ��������Ϣ
        //                         break;
        case Sch_JumpM_Msg:
            //������Ҫ��������Ϣ��Ŀǰ��Ա�����������½�������Ϣ����ʱ�Ĺ�����
            //�������˲ֻ̬��Ҫһ��ʱ�����ڽ��д�������ʵ���϶�������Ϣ���벿�ֲ���Ӱ�졣
//#if SUPPORT_AUX==1
            if(CommonFunc::uc_Support_AUX)
                ClearAUXLimits(uc_ChannelNo, 0);
//#endif

            // ���µĲ����������֧�ָ��� �Ͳ�֧�ָ���ʱ�ķ��롣������������������� 05.08.2014  DKQ
            //����ط��������ж��и���״̬�£��Ƿ�ִ���˸���������������Ƿ�ȴ�����Ӧ��û�и������õĻ�Ҫִ�в��Ը���Ӧ��״̬�ж��������еĲ���

            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
            p_DoMesBuf->ScheduleMessage.GotoStepID =  p_DoMesBuf->ScheduleMessage.StepID;
            p_Info->m_Event_Source = S_START_JUMPMMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            p_Sch->m_RunningStepIdx = p_DoMesBuf->ScheduleMessage.StepID;
            ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_Sch->m_RunningStepIdx); //stepװ�ص�����������������ӿ�

            // #endif
            //IR�쳣�ж�
            if(IRspecial_Judge(uc_ChannelNo))
            {
                ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_Sch->m_ItemCount.m_u16StepCount); //stepװ��IR�������ⲽ������������ӿ�
                IRSpecialTestFlg[uc_ChannelNo] = 0;
            }
            p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
            p_Info->ScheduleStatus = RUNNING;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //�����Ϣ
            break;
// 		case Sch_LimitPropertyChange_Msg:
// 			//�������Ϣ���벿�ִ���ʱ���ڷ���Msgǰ�����������Step��������ӦLimit�����ԡ�
// 			ChannelInfo_Manager::LoadNextStep(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID); //stepװ�ص�����������������ӿ�
// 			p_Sch->m_RunningStepIdx = p_Sch->m_NextStepIdx;
// 			//ChannelInfo_Manager::Init_Metavariable(uc_ChannelNo);             //MV��ʼ��������������ӿ�
// 			//����Parse�·�����Ϣ������ǰ������LimitID���������Ӧ�����ԣ�
// 			//p_Step->.m_Info[1-ActiveIdx].m_Data.m_Limits[].m_ucIfCanSelfJump=DoMessageBuf.ScheduleMessage.LimitPropertyChange;    //���´�������Step�����б�limit�Ƿ���Զ�������״̬
// 			p_Info->ScheduleStatus=RUNNING;
// 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //�����Ϣ
// 			break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_START_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal��Ϣ�������Ҫת��IDLE״̬
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
            SchStartRoutine(uc_ChannelNo);                     //START˲̬��Ӧ����
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
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
            SchOnUnsafeMsgRoutine(uc_ChannelNo);                           //Abnormal��Ϣ�������Ҫת��IDLE״̬
            p_Info->ScheduleStatus = GOSTOP;//FIXEDUNSAFE; //Add by DKQ 09_09_2013
            SchReportMessageToPC(uc_ChannelNo,PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            break;
        case Sch_Started_Msg:
            //p_Info->m_Event_Source=S_RUNNING_STARTEDMSG;
            SchStartedMsgRoutine(uc_ChannelNo); //��������������ִ�����档
        default:
            SchWaitForAuxRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);

            if(p_DoMesBuf->ScheduleMessage.MsgNumber == Sch_LimitArrive_Msg)  //����ײ�������ʱ
                return;

            break;
        }
        break;
//#endif

    case RUNNING:    //RUNNING��ӦSch_Stop_Msg��Sch_Pause_Msg��Sch_JumpA_Msg��Sch_JumpM_Msg��
        //Sch_LimitArrive_Msg��Sch_LimitPropertyChange_Msg��Sch_LimitStatusChange_Msg��Sch_Started_Msg��Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            p_Info->ScheduleStatus = GOSTOP;
            Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0;

            //chen add 20131017
            if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_POWER_OFF)  //dirui del 2017/12/18
            {
// 				//PowerOFF ����
            }
            else
            {
                SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;
            }
            ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, LAST_POINT); //180309 zyx

            IVSDL::LimitArrivedRoutine(uc_ChannelNo);
            EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; // 2013/10/24 yang  �޸Ĺؼ����¼����

            Smart_Battery::SetSMBusLastLog(uc_ChannelNo, LASTPOINT);//add by zc 04/01/2019
            Step_Manager_task::SendLog(uc_ChannelNo);
            //Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);

            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
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
            p_Info->ScheduleStatus = GOSTOP; //Abnormal���������ɺ�Ҫ����GOSTOP״̬
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
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
            SendSDLShowData(uc_ChannelNo); //��ʾ����3���ϴ�����
            SchRunningRoutine(uc_ChannelNo);               //RUNNING˲̬��Ӧ����
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);   //�����Ϣ����
            break;
        }
        break;
    case GOPAUSE:    //GOPAUSE��ӦSch_Stop_Msg��Sch_Stopped_Msg��Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            Schedule_Manager_task::M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            break;
        case Sch_Stopped_Msg:
            p_Info->m_Event_Source = S_GOPAUSE_STOPPEDMSG;
            SchStoppedMsgRoutine(uc_ChannelNo);
            break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_GOPAUSE_ONUNSAGEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP; //Abnormal���������ɺ�Ҫ����GOSTOP״̬
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            break;
        default:
            SchGopauseRoutine(uc_ChannelNo);                     //GOPAUSE˲̬��Ӧ����
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //�����Ϣ����
            break;
        }
        break;
    case RESUME:    //RESUME��ӦSch_Stop_Msg��Sch_LimitPropertyChange_Msg��Sch_OnUnsafe_Msg
        switch(MessageNumber)
        {
        case Sch_Stop_Msg:
            M_SysALarmEmergencyFlg&=~(1<<uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP;
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            break;
// 		case Sch_LimitPropertyChange_Msg:  //����"����Ϣ����"���ú󣬸���Step.Mgr��Step��������ӦLimit�����ԣ�
// 			SchResumeRoutine(uc_ChannelNo);                 //����Ϣ���봦��
// 			//����Parse�·�����Ϣ������ǰ������LimitID���������Ӧ�����ԣ�
// 			//    p_Step->.m_Info[1-ActiveIdx].m_Data.m_Limits.m_ucIfCanSelfJump=DoMessageBuf.ScheduleMessage.LimitPropertyChange;    //���´�������Step�����б�limit�Ƿ���Զ�������״̬
// 			//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
// 			break;
        case Sch_OnUnsafe_Msg:
            p_Info->m_Event_Source = S_RESUME_ONUNSAFEMSG;
            SchOnUnsafeMsgRoutine(uc_ChannelNo);
            p_Info->ScheduleStatus = GOSTOP; //Abnormal���������ɺ�Ҫ����GOSTOP״̬
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
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
            SchResumeRoutine(uc_ChannelNo);                 //RESUME˲̬��Ӧ����
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            break;
        }
        break;
    case GOSTOP:    //GOSTOP��ӦSch_Stopped_Msg��Sch_OnUnsafe_Msg
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
            SchGoStopRoutine(uc_ChannelNo);                //GOSTOP˲̬��Ӧ����
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
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

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;  // Add by DKQ 09.09.2013
            break;
        case Sch_OnSafe_Msg:
            p_Info->ScheduleStatus = IDLE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            break;
        default:
            SendSDLShowData(uc_ChannelNo);  //��ȫ״̬�¼�����ʱ�����Ϸ����� 0905 dirui
            Sch_FixedUnsafeRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
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

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;  // Add by DKQ 09.09.2013
            break;
        case Sch_OnSafe_Msg:
            p_Info->ScheduleStatus = IDLE;
            thirdParty::UnsafeLockSwitch[uc_ChannelNo] = 0;  //add by qjm20150808
            safety_check::Process_SM_Ptr[uc_ChannelNo] = SAFE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            break;
				case Sch_Resume_Msg:
        case Sch_Run_Msg:
        {
            //����ǯλ��������
            // 									IR::DAC_Flag=1;
            // 									board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,p_Info->m_fVclampHigh),ACTIVE_DAC_VH);
            // 									board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,p_Info->m_fVclampLow),ACTIVE_DAC_VL);
            // 									IR::DAC_Flag=0;
            unsigned long Code = ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, p_Info->m_fVclampHigh);
            Step_Manager_task::StepDAC_Out(uc_ChannelNo, Code, ACTIVE_DAC_VH);
            Code = ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo, VOLTAGE_RANGE_HIGH, p_Info->m_fVclampLow);
            Step_Manager_task::StepDAC_Out(uc_ChannelNo, Code, ACTIVE_DAC_VL);

            //ֱ��ִ�б�����,ֱ�ӵ��ð�ȫ������    0904 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //�趨�ظ���ϢΪAbnormal��Ϣ
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = p_Info->m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
            //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
            //p_Info->m_Event_Source=S_DYNAMICUNSAGE_RUNMSG;
            //EthernetParse::m_ReportStatus.m_btSCHEvent_Source = p_Info->m_Event_Source;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
            p_Info->m_Event_Source = S_DYNAMICUNSAGE_RUNMSG;
            SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
            p_Info->ScheduleStatus = FIXEDUNSAFE; //Add by DKQ 09_09_2013
            break;
        }
        default:
            SendSDLShowData(uc_ChannelNo);  //��ȫ״̬�¼�����ʱ�����Ϸ����� 0905 dirui
            //	Sch_DynamicUnsafeRoutine(uc_ChannelNo);
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
            break;
        }
        break;
    default:
        //break;
        return;
    }
    ClearScheduleMessage(p_DoMesBuf);  //�����Ϣ����
}


/*****************************************************
// Function Name: SchAssignMsgRoutineWithNotReady
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Assign_Msg��Ϣ��Ӧ���̴���(NOTREADY)״̬��Ӧ��
// Others:
//                        ����ģʽ������/���������£�Ѱ����ӦSchedule��Simulation��Pulse��
//                        ��ʧ���򷢻�ʧ�ܵ�MD5���PC���ɹ��򷵻سɹ���Ϣ��PC�������漰��
//                        Schedule��Simulation��Pulse��־Ϊ��Assigned����ռ�ã�����ͬʱ��
//                        Schedule��Simulation��Pulse��Index����ͨ���ϵǼǡ���Simulation
//                        ��Pulse��Ҫ��������MD5��-Index���ձ���ͬʱ����IDLE״̬��
// transitional information:
//                        PC_AssignRpt_Msg
// History���ݲ�ʹ��
//
// ******************************************************/
// void Schedule_Manager_task::SchAssignMsgRoutineWithNOTREADY(unsigned char uc_ChannelNo)
// {
//     memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD=PC_AssignRpt_Msg;                  //�趨�ظ���ϢΪ AssignRpt��Ϣ
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
// 	EthernetParse::m_st_Assign = & DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign;
//
// 	if(ChannelInfo_Manager::FindScheduleBuf(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5)>=0)
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_u16ChannelIdx == uc_ChannelNo)
//         {
//             for(UChar8 uc_i = 0; uc_i < DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_ucMD5Count; uc_i++)
//                 DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS;  //  ��ʱ������д
//         }
//
// 		if(SendFinishFlag[uc_ChannelNo].StopStatus==STOP_UNSAFE)  //�������ͱ���SystemUnsafe
// 		    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=DYNAMICUNSAFE;
// 		else
// 		    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;                   //����ɹ�ת��IDLE״̬
//
//     }
//     else         //Schedule���Ϸ��������
//     {
//
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //�趨�ظ���ϢΪ������Ϣ
//         EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOAD_SCH_FAIL; // ERROR_ASSIGN_FAIL;
//     }
//     EthernetParse::MPReportData(uc_ChannelNo,SEND_REPORT_ASSIGN);   //��PC���ͻظ���Ϣ����Parse��س���
//     ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
// }
/*****************************************************
// Function Name: SchAssignMsgRoutineWithIDLE
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Assign_Msg��Ϣ��Ӧ���̴���(IDLE)״̬��Ӧ��
// Others:
//                    ����״̬������/���������£�Ѱ����ӦSchedule��Simulation��Pulse����ʧ���򷢻�ʧ�ܵ�MD5���PC���ɹ��򷵻سɹ���Ϣ��PC��
//                    ���Ŀǰ��Assign��Schedule��Simulation����Pulse����ı�ռ�ñ�־������ָ����Schedule��Simulation��Pulse��־Ϊ��Assigned����ռ�ã���
//                    ��ͬʱ��Schedule��Simulation��Pulse��Index����ͨ���ϵǼǡ���Simulation��Pulse��Ҫ��������MD5��-Index���ձ���
// transitional information:
//                        PC_AssignRpt_Msg
// History���ݲ�ʹ��
//
// ******************************************************/
// void Schedule_Manager_task::SchAssignMsgRoutineWithIDLE(unsigned char uc_ChannelNo)
// {
//     memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD = PC_AssignRpt_Msg;                    //�趨�ظ���ϢΪ AssignRpt��Ϣ
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
//     EthernetParse::m_st_Assign = & DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign;
//
//     if (ChannelInfo_Manager::RegisterSchedule(&DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign))   //Ѱ����Ӧ��Schedule   ����������ʵ��
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_u16ChannelIdx == uc_ChannelNo)
//         {
//             for(UChar8 uc_i = 0; uc_i < DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_ucMD5Count; uc_i++)
//                 DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS; //  ��ʱ������д
//         }
//         ChannelInfo_Manager::UnregisterSchedule(uc_ChannelNo);                         //�ɹ���Ҫע���µ�Schedule(�趨��־λ����ͨ���ϵǼ�Index��); �ϵı��λ����� ����������ʵ��
//         ChannelInfo_Manager::RegisterSchedule(&DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign);
//     }
//     else       //Schedule���Ϸ��������
//     {
//
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //�趨�ظ���ϢΪ������Ϣ
//         EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_LOAD_SCH_FAIL; // ERROR_ASSIGN_FAIL;
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = NOTREADY;
//     }
//
//     EthernetParse::MPReportData(uc_ChannelNo,SEND_REPORT_ASSIGN);   //��PC���ͻظ���Ϣ����Parse��س���
//
//     ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
// }

/*****************************************************
// Function Name: SchAssign&StarRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         ���Assign+Start�ӿ�
// Others:
//                   Parse���ݽӿڷ��ص����ݻظ�PC
// transitional information:
//
// History��
//
// ******************************************************/
unsigned char Schedule_Manager_task::SchAssignAddStarRoutine(St_Assign * AssignMsg)
{
    St_ScheduleMessage   SchMessage;
    unsigned char uc_ChannelNo = AssignMsg->m_u16ChannelIdx;
    unsigned char *Status = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus;

    //     if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==RUNNING)
    // 	{
    // 		return SCHEDULERUNNING;           //�Ѿ�ָ�ɣ�Ҫ����,��PC����StartFail������ΪSCHEDULERUNNING
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==DYNAMICUNSAFE)
    // 	{
    // 		SchMessage.MsgNumber = Sch_Run_Msg;  //�������������Schedule�ϱ�unsafe���Ӷ��ŵ��� dirui 0904
    // 		Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,SchMessage); // ����SCh������Ϣ
    // 		return SCHEDULEUNSAGE;           //�����ˣ�����������PC����StartFail������ΪSCHEDULEUNSAGE
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==NOTREADY)
    // 	{
    // 		if(SchAssignRoutine(uc_ChannelNo,AssignMsg)==1)
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=IDLE;
    // 			return ASSIGNOK;   //��PC����ASSIGNOK ͬʱ��Schedule����Start��Ϣ
    // 		}
    // 		else
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=NOTREADY;
    // 			return ASSIGNFAIL;  //��PC����ASSIGNFAIL
    // 		}
    // 	}
    // 	else if((ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==IDLE))
    // 	{
    // 		if(SchAssignRoutine(uc_ChannelNo,AssignMsg)==1)
    // 			return ASSIGNOK;   //��PC����ASSIGNOK ͬʱ��Schedule����Start��Ϣ
    // 		else
    // 		{
    // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=NOTREADY;
    // 			return ASSIGNFAIL;  //��PC����ASSIGNFAIL
    // 		}
    // 	}
    // 	else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus==FIXEDUNSAFE)  //20130831 hepeiqing add,a case unhandled
    // 	{
    // 		SchMessage.MsgNumber = Sch_Run_Msg;  //�������������Schedule�ϱ�unsafe���Ӷ��ŵ��� dirui 0904
    // 		Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,SchMessage); // ����SCh������Ϣ
    // 		return SCHEDULEUNSAGE;
    // 	}
    // 	//else  //20130831 hepeiqing add,Dirui: pls make sure that all cases are handled!
    // 	return ERROR;    //ʲôҲû�д��������·��� ERROR״̬ 0901 dirui

    switch(*Status)
    {
    case RUNNING:
        return SCHEDULERUNNING;           //�Ѿ�ָ�ɣ�Ҫ����,��PC����StartFail������ΪSCHEDULERUNNING
    case DYNAMICUNSAFE:
    case FIXEDUNSAFE:
//         SchMessage.MsgNumber = Sch_Run_Msg;  //�������������Schedule�ϱ�unsafe���Ӷ��ŵ��� dirui 0904
//         Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage); // ����SCh������Ϣ
        return SCHEDULEUNSAGE;           //�����ˣ�����������PC����StartFail������ΪSCHEDULEUNSAGE
    case NOTREADY:
    case IDLE:
        if(SchAssignRoutine(AssignMsg) == 1)
        {
            *Status = IDLE;
            return ASSIGNOK;   //��PC����ASSIGNOK ͬʱ��Schedule����Start��Ϣ
        }
        else
        {
            *Status = NOTREADY;
            return ASSIGNFAIL;  //��PC����ASSIGNFAIL
        }
    default:
        return ERROR;    //ʲôҲû�д��������·��� ERROR״̬ 0901 dirui
    }

}
/*****************************************************
// Function Name: SchAssignRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         ����Schedule����
// Others:
//
// transitional information:
//
// History��
//
// ******************************************************/
int Schedule_Manager_task::SchAssignRoutine(St_Assign * AssignMsg)
{
    //unsigned char uc_ChannelNo = AssignMsg->m_u16ChannelIdx;
    if (ChannelInfo_Manager::RegisterSchedule(AssignMsg))   //Ѱ����Ӧ��Schedule   ����������ʵ��
    {
        //if(AssignMsg->m_u16ChannelIdx == uc_ChannelNo)
        //{
        for(UChar8 uc_i = 0; uc_i < AssignMsg->m_ucMD5Count; uc_i++)
            AssignMsg->m_MD5Assign[uc_i].m_bOk = CONFIRM_RESULT_SUCCESS;
        //}
        //ChannelInfo_Manager::UnregisterSchedule(uc_ChannelNo);                         //�ɹ���Ҫע���µ�Schedule(�趨��־λ����ͨ���ϵǼ�Index��); �ϵı��λ����� ����������ʵ��
        //ChannelInfo_Manager::RegisterSchedule(AssignMsg);
        return 1;
    }
    else       //Schedule���Ϸ��������
    {
        return -1;
    }
}
/*****************************************************
// Function Name: SchOnUnsafeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_OnUnsafe_Msg��Ϣ��Ӧ���̴���
// Others:
//                        ��������ģʽ
// transitional information:
//                        PC_Abnormal_Msg
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchOnUnsafeMsgRoutine(unsigned char uc_ChannelNo)
{

    //EthernetParse::m_ReportStatus.m_st_RunningStatus��ֵҪ����ǰ�棬�����б���������
    //�¶�20130828
    //UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;
//     memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));

//     EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //�趨�ظ���ϢΪAbnormal��Ϣ
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType;
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
//     UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit = UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                   //20131023 dirui �������ķ���
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
}
/*****************************************************
// Function Name: SchOnSafeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_OnSafe_Msg��Ϣ��Ӧ���̴���
// Others:
//                        �������ģʽ
// transitional information:
//                        PC_BackToNormal_Msg
// History��
//
// ******************************************************/
// void Schedule_Manager_task::SchOnSafeMsgRoutine(unsigned char uc_ChannelNo)
// {
//     //UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;  // 08/28/2013  �¶� �����ȫ��������
//     //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
//     //EthernetParse::m_ReportStatus.m_st_RunningStatus = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running;
//     //EthernetParse::m_ReportStatus.m_btCMD=PC_BackToNormal_Msg;    //�趨�ظ���ϢΪBackToNormal��Ϣ
// 	   //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;
//     //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);   //��PC���ͻظ���Ϣ����Parse��س���
//
//     UnsafeContent[uc_ChannelNo].st_status_sch_running = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running;
//     ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_FIXEDUNSAFE_STOPMSG;
//     SchReportMessageToPC(uc_ChannelNo,PC_BackToNormal_Msg, &UnsafeContent[uc_ChannelNo]);
//
//     //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
// }
/*****************************************************
// Function Name: SchRunMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Run_Msg��Ϣ��Ӧ���̴���
// Others:
//                        ������ģʽ����Ӧ��ת��START״̬
// transitional information:
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchRunMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_LastOutput_1B = 0;   //170307 add by hpq
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_fLastErr_1B = 0;     //170307 add by hpq
    ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = 0.0f;        //170307 add by hpq
    m_backupMvVoltage=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
    if(LockModeSwitch[uc_ChannelNo] == 1)  //���ڴ��ڰ�ȫ״̬    0902 dirui
    {
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //�趨�ظ���ϢΪAbnormal��Ϣ
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART; // Add by DKQ on 09.10.2013
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_IDLE_RUNMSG_LOCK;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);


        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_IDLE_RUNMSG_LOCK;
        SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = DYNAMICUNSAFE; //dirui add 2015/07/15
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
        return;
    }
// 	if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	{
// 	    b_start2resumeflag[uc_ChannelNo]=0;
//         PowerAdjNeedCheck(uc_ChannelNo,0);
// 	}
// 	else
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=START;   //������ģʽ��ת��START״̬
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  //��λ�������Ҵ���һ����ʼ���Ҿʹ���һ����ʼ
    //����ǯλ����
    // 	if(board_config::DacInfo.m_bHaveClampVdac)  //�������Ѿ��������Բ�����Ҫ
    // 	{
    // 	    IR::DAC_Flag=1;
    // 	    board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh),ACTIVE_DAC_VH);
    // 	    board_config::DAC_out(uc_ChannelNo,ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo,VOLTAGE_RANGE_HIGH,ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow),ACTIVE_DAC_VL);
    //         IR::DAC_Flag=0;
    // 	}
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui ��Schedule����֮�����AUX�İ�ȫ��Ϣ
        ClearAUXLimits(uc_ChannelNo, 0);
    }
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
}
/*****************************************************
// Function Name: SchResumeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Resume_Msg��Ϣ��Ӧ���̴���
// Others:
//                        ������ģʽ����Ӧ��ת��RESUME״̬
// transitional information:
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchResumeMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_LastOutput_1B = 0;   //170307 add by hpq
    ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo].m_fLastErr_1B = 0;     //170307 add by hpq
    ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = 0.0f;        //170307 add by hpq
    if(LockModeSwitch[uc_ChannelNo] == 1)  //���ڴ��ڰ�ȫ״̬    0902 dirui
    {
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;        //�趨�ظ���ϢΪAbnormal��Ϣ
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_SCHMANAGE;
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source=S_IDLE_RESUMEMSG_LOCK;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
        //memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);

        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_NOTSFTOSTART;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_IDLE_RESUMEMSG_LOCK;
        SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
        return;
    }

    //    SCH_RESUME_Flag[uc_ChannelNo] = 1;
// 	 if( ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	 {
// 		b_start2resumeflag[uc_ChannelNo]=1;
// 		PowerAdjNeedCheck(uc_ChannelNo,1);
// 	 }
// 	 else
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus=RESUME;     //������ģʽ��ת��RESUME״̬
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  //��λ�������Ҵ���һ����ʼ���Ҿʹ���һ����ʼ

    //����ǯλ����
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
        ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui ��Schedule����֮�����AUX�İ�ȫ��Ϣ
    }

    if(Smart_Battery::BeSupportSmbus)
    {
        MetaVariable_Task::My_SMBLog_Status=SMBLOG_WAIT;

    }
//#endif
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                                      //��ǰ��Ϣ�������Ҫ���
}

/*****************************************************
// Function Name: SchStopMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Stop_Msg��Ϣ��Ӧ���̴���
// Others:
//                        ת��GOSTOP״̬
// transitional information:
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchStopMsgRoutine(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
    SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_MANUAL;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                                      //��ǰ��Ϣ�������Ҫ���
}

/*****************************************************
// Function Name: SchJumpAMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_JumpA_Msg��Ϣ��Ӧ���̴���
// Others:
//                ���ж�Sch.MD5��ToStepID��FromStepID�Ƿ�Ϸ�����Ϸ���
//                ��Step.Mgr���񲢲��ڵ���һ����ʱ������ʱ����Ӧ�����
//                װ�������Step��Step.Mgr����ĸ����У��������ʼ�������ã�
//                ����MV������Msg��Step.Mgr���������д�Step
//
// transitional information:
//                Step_JumpA_Msg
// History��
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

//    if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo, p_DoMesBuf->ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))  //���Schedule��MD5�룻  ����������ʵ��
//    {
    if(FromStepID == ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)    //�Ӵ���������ȡ��Step_ID���ж�FromStepID�Ƿ�Ϸ�
    {
        if(ToStepID < ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_ItemCount.m_u16StepCount) //�ж�Tostep�Ƿ�Ϸ�
        {
            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 0 ) //���Step�����Ƿ��ڵ���һ����ʱ������
            {
                //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
                //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;				//��������׼������ǰ����JUMP
                //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
                //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = ToStepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
                //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
                p_DoMesBuf->ScheduleMessage.GotoStepID = ToStepID;
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_JUMPAMSG;
                SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);

                ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ToStepID); //װ�������Step��������
                ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = ToStepID;
//					RecordLastStepData(uc_ChannelNo);	//20130723��¼��һ���������
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
//                         Sch_JumpM_Msg��Ϣ��Ӧ���̴���
// Others:
//                �ж�Sch.MD5�Ƿ�Ϸ�����Ϸ����ж�Step.Mgr�����Ƿ��ڵ���һ����ʱ�����棬
//                �����ڵ������棬���������������ϣ�����Ӧ���������������Ӧ��
//                װ�������Step��Step.Mgr����ĸ����У��������ʼ�������ã�����MV������Msg��
//                Step.Mgr���������д�Step
// transitional information:
//                Step_JumpM_Msg
// History��
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

//    if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo, DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))  //Ѱ����Ӧ��Schedule ��  ����������ʵ��
//    {
    if( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 1) //���Step�����Ƿ��ڵ���һ����ʱ������   �����ڴ��������Channel_Info�в���
        return;    //������������б���������
    else
    {
        EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; // 2013/10/24 yang  �޸Ĺؼ����¼����
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
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;    	//�ӵ��ֹ�����������֮ǰ����JUMP
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
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
        ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //װ�������Step��Step.Mgr����ĸ�����
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
// 			ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue = DoMessageBuf[uc_ChannelNo].ScheduleMessage.CycleID;
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
        IVSDL::LimitArrivedRoutine(uc_ChannelNo);  //????,??Limit??????
        //EthernetParse::uc_LogCriticalPoint = LOG_DATA_CRITICALPOINT; // 2013/10/24 yang  �޸Ĺؼ����¼����
        //Step_Manager_task::SendLog(uc_ChannelNo);
//			RecordLastStepData(uc_ChannelNo);	//20130723��¼��һ���������
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
    if( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus == 1) //���Step�����Ƿ��ڵ���һ����ʱ������   �����ڴ��������Channel_Info�в���
        return;    //������������б���������
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
//                          Sch_LimitArrive_Msg��Ϣ��Ӧ���̴���
// Others:
//
//                ֪ͨPCLimit�������Limitȥ�������
//                1.ֹͣ���У�ת��GOSTOP״̬������Ҫ����PC����PC_Unsafe_Msg��
//                2.װ����Step��Step.Mgr����ĸ����У��������ʼ�������ã�����MV������Msg��Step.Mgr���������д�Step��
//                3.��������ִ��SetVariable_Step������MV��Sync�仯��MV��PC��ͬʱ��SetVariableȥ������ִ�б�Sch_LimitArrive_Msg��Ӧ�߼���
// transitional information:
//
//                PC_LimitArrive_Msg, PC_UnsafeRpt_Msg, Step_Stop_Msg, Step_Run_Msg
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchLimitArriveMsgRoutine(unsigned char uc_ChannelNo)
{
    unsigned char ucActiveIdx;
    St_StepMessage StepMessage;
    St_DoMessage * p_DoMesBuf = &DoMessageBuf[uc_ChannelNo];
    St_ChannelInfo	* p_Info =	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];

    IRSpecialTestFlg[uc_ChannelNo] = 0; //�����Ƿ����������Զ�����ֱ������ñ��
    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = false;
    ChannelInfo_Manager::m_RC_CurrentFilterDeepth[uc_ChannelNo] = 1;
    ChannelInfo_Manager::m_RC_VoltageFilterDeepth[uc_ChannelNo] = 1;

    if(ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo])
    {
// 		if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.m_ImpedanceGoto == AbnormalGoto_Stop)
// 		{
        p_DoMesBuf->ScheduleMessage.GotoStepID = MCU_GOTOUNSAFE;
        //���unsafe����ΪIr�쳣
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

        // ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
    }
    ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo] = 0; //����ñ��

//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        ucActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
            CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][AuxIdx].m_bShiedCheck = 1;
        ClearAUXLimits(uc_ChannelNo, 1); //20131121 dirui ��Step����֮���������ͨ��Limits
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

    switch(p_DoMesBuf->ScheduleMessage.GotoStepID)           //ȡ����һ������ķ�������ж�
    {
    case MCU_GOTONEXTSTEP:

        if(ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, MCU_GOTONEXTSTEP) == 1) //װ�������Step��Step.Mgr����ĸ�����
        {
            bMetaSyncSend[uc_ChannelNo] = 0;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;        //Limit��λ������Ҫ����������ǰ��JUMP
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID ��Parse��Ҫ���
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID ��Parse��Ҫ���
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
            p_Info->m_Event_Source = S_RUNNING_LIMITARRIVEMSG_GOTONEXTSTEP;
            //2016.05.10
// 			if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE && \
// 				(int)(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]+0.1f) == ACIM_PAUSE)
// 					Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_START;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                //��PC���ͻظ�
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;


            if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_REST)
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0.0f;

            SchTimeTrigger[uc_ChannelNo] = 0;     //Limit�����Ҫ��Step����ִ����һ������Ϣ����ʱҪ�峬ʱ��ǣ��ڵ�ǰ��Schedule״̬��ִ�г�ʱ�����ȴ�Step�ɹ����з���Started��ǡ�0911 dirui
            StepMessage.MsgNumber = Step_JumpA_Msg;
            //�˴���������һ�ν�������������Ͳ�ʱIR�쳣�жϴ���
            if((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck) && (Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] == 0))
            {
                if(IRspecial_Judge(uc_ChannelNo))
                    IRspecial_Judge_Process(uc_ChannelNo);
            }


            //////////////////////////////////////////////////
        }
        else
        {
            //װ��ʧ��ֹͣͨ��
            StepMessage.MsgNumber = Step_Stop_Msg;

            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;   //Limit_ID ��Parse��Ҫ���

        }

        if(StepMessage.MsgNumber == Step_Stop_Msg)
            p_Info->ScheduleStatus = GOSTOP;
        else
            Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
        // #endif

        break;
    case MCU_GOTOSTOPSTEP:
        //  StepMessage.MsgNumber=Step_Stop_Msg;
        //  Step_Manager_task::SendMsgToStep(uc_ChannelNo,StepMessage);     //����Msg��Stepֹͣ
        p_Info->ScheduleStatus = GOSTOP;
        bMetaSyncSend[uc_ChannelNo] = 0; //zyx 171010
        SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
        p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID ��Parse��Ҫ���
        Smart_Battery::SetSMBusLastLog(uc_ChannelNo, LASTPOINT);//add by zc 04/01/2019
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
        break;

    case MCU_GOTOUNSAFE:

        //StepMessage.MsgNumber=Step_Stop_Msg;
        //Step_Manager_task::SendMsgToStep(uc_ChannelNo,StepMessage);     //����Msg��Stepֹͣ
        p_Info->ScheduleStatus = GOSTOP;
        SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
        if(IR_UnsafeFlg[uc_ChannelNo])
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_CHANNELABNORMAL;
        else
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_UNSAFE;
        //����Ҫ����λ��ϵUnsafe�ı������� ��������monitor��ֻ��ʾCODE-0 UNSAFE,������ʾLimitID��
        p_Info->m_UnsafeType = IV_UNSAFE_TYPE;
        p_Info->m_SafetyStatus = LIMIT_UNSAFE; //��Ӵ����� ��ʱ����Ϊ64
        safety_check::SetUnsafe(uc_ChannelNo, LIMIT_UNSAFE, 0,IV_UNSAFE_TYPE);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CHECKLIMIT;
        p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
        break;
    case MCU_GOTOLOG:  //LogLimit��������
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
    //break;
    case MCU_GOTOGROUPWAITFORNEXT:
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
    //break;
    case MCU_GOTOGROUPFOLLOWNEXT:
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
        break;
    default:

        if(ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, p_DoMesBuf->ScheduleMessage.GotoStepID) == 1)
        {
            bMetaSyncSend[uc_ChannelNo] = 0;
            //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Jump_Msg;    //Limit��λ������Ҫ����������ǰ��JUMP
            //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;  //Limit_ID ��Parse��Ҫ���
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID ��Parse��Ҫ���
            //EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
            p_Info->m_Event_Source = S_RUNNING_LIMITARRIVEMSG_GOTOSTEP;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                //��PC���ͻظ�
            SchReportMessageToPC(uc_ChannelNo, PC_Jump_Msg, &UnsafeContent[uc_ChannelNo]);
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;


            if (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_REST)
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0.0f;

            SchTimeTrigger[uc_ChannelNo] = 0;                     //Limit�����Ҫ��Step����ִ����һ������Ϣ����ʱҪ�峬ʱ��ǣ��ڵ�ǰ��Schedule״̬��ִ�г�ʱ�����ȴ�Step�ɹ����з���Started��ǡ� 0911 dirui
            StepMessage.MsgNumber = Step_JumpA_Msg;
            //�˴���������һ�ν�������������Ͳ�ʱIR�쳣�жϴ���
            if((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck) && (Schedule_Manager_task::IRHasBeenRun == 0))
            {

                if(IRspecial_Judge(uc_ChannelNo))
                    IRspecial_Judge_Process(uc_ChannelNo);
            }


            //////////////////////////////////////////////////
        }
        else
        {
            //װ��ʧ��ֹͣͨ��
            StepMessage.MsgNumber = Step_Stop_Msg;
            SchTimeTrigger[uc_ChannelNo] = 0; //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ��������峬ʱ��־λ ��Gostop������ִ�г�ʱ����  0911 dirui
            SendFinishFlag[uc_ChannelNo].StopStatus = STOP_FINISHED;
            p_Info->Report_LimitID = p_DoMesBuf->ScheduleMessage.m_Message.m_RunMsg.LimitID;   //Limit_ID ��Parse��Ҫ���

        }


        if(StepMessage.MsgNumber == Step_Stop_Msg)
            p_Info->ScheduleStatus = GOSTOP; //GoStop send Step_Stop_Msg too.  dirui  2015/09/16
        else
            Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);

        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���

        //#endif
        break;

    }

    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���

}
/*****************************************************
// Function Name: SchLimitPropertyChangeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                          Sch_LimitPropertyChange_Msg��Ϣ��Ӧ���̴���
// Others:
//
//                �ж�Sch.MD5��StepID��LimitID�Ƿ�Ϸ�������Msg���ݸ���Step.Mgr����ӦLimit����
// transitional information:
//
//
//
// History���ݲ�ʹ��
//
// ******************************************************/
// void Schedule_Manager_task::SchLimitPropertyChangeMsgRoutine(unsigned char uc_ChannelNo)
// {
//     unsigned char ucActiveIdx;
//     ucActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
// //    unsigned char LimitID=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.LimitID;
// //	unsigned char EquationID=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;
//     if (ChannelInfo_Manager::CheckScheduleMD5(uc_ChannelNo,DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_Assign.m_MD5Assign[0].m_MD5))   //Ѱ����Ӧ��Schedule ��  ����������ʵ��
//     {
//         if(DoMessageBuf[uc_ChannelNo].ScheduleMessage.StepID==ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)  //StepID �Ϸ�
//         {
// 					  //LimitIDֵ����λ��Limit������ֱ�ӹ�ϵ����ʱ�޷��ж�
//            // if(LimitID<=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo)//
//           //  {
// 				for(unsigned char uc_i=0;uc_i<(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo
// 						+ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo);uc_i++)
// 				{
// 					//if((LimitID==ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_u16LimitID)
// 					//	&& (EquationID==ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_ucEquationID))
// 					//ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ucActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i].m_ucIfCanSelfJump=DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.LimitPropertyChange;    //���´��������б�limit�Ƿ���Զ�������״̬

// 				}
//            // }
//         }
//     }
//     //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
// }
/*****************************************************
// Function Name: SchLimitStatusChangeMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       Sch_LimitStatusChange_Msg��Ϣ��Ӧ���̴���
// Others:
//
//                 ��PC������ӦMsg
// transitional information:
//                PC_LimitStatusChanged_Msg
//
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchLimitStatusChangeMsgRoutine(unsigned char uc_ChannelNo)
{
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD=PC_LimitChange_Msg;  //Limit״̬���������������������LimitChange
    //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //��PC���ͻظ�
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_LIMITSTATUSCHANGEMSG;
    SchReportMessageToPC(uc_ChannelNo, PC_LimitChange_Msg, &UnsafeContent[uc_ChannelNo]);
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
}
/*****************************************************
// Function Name: SchStartedMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Started_Msg��Ϣ��Ӧ���̴���
// Others:
//                ��PC������ӦMsg���˳���ʱ����
//
// transitional information:
//
//                PC_Started_Msg,PC_ResumeRpt_Msg,PC_JumpARpt_Msg,PC_JumpMRpt_Msg
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchStartedMsgRoutine(unsigned char uc_ChannelNo)
{

    SchTimeTrigger[uc_ChannelNo] = 0; //�����Ѿ�ȷ��Step�������������� ������Ҫ����ִ�г�ʱ���㴦��������      0911 dirui
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD=PC_Started_Msg; //ִ�����Ҫ���л����̺󣬷���Started
    //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //EthernetParse::m_ReportStatus.m_stMSecondField.m_wChannelNumber=uc_ChannelNo;
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //��PC���ͻظ�
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_RUNNING_STARTEDMSG;
    SchReportMessageToPC(uc_ChannelNo, PC_Started_Msg, &UnsafeContent[uc_ChannelNo]);
    IVSDL::StepStartedRoutine(uc_ChannelNo, true);
    if(Smart_Battery::BeSupportSmbus)
        MetaVariable_Task::My_SMBLog_Status = SMBLOG_WAIT;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
}
/*****************************************************
// Function Name: SchStoppedMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Sch_Stopped_Msg��Ϣ��Ӧ���̴���
// Others:
//                GOPAUSE״̬ʱת��IDLE״̬����GOSTOP״̬ʱת��IDLE״̬���������Assign�������ݣ�
//
// transitional information:
//
//                PC_StopRpt_Msg
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchStoppedMsgRoutine(unsigned char uc_ChannelNo)
{
    //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
    //EthernetParse::m_ReportStatus.m_btCMD = PC_StopRpt_Msg; //Scheduleֹͣ��ִ�����Ҫ���л����̺󣬷���Stopped��
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
        //          board_config::ReleaseRedundantSafety(uc_ChannelNo,0);	     //������ౣ������
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
        PWM_Base::PWM_LEDRunStatus = SYSTEM_IDLE; //06.19.2018 add by zc ��Q����schedule��Ȳ���׃���Sɫ��BUG
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //��Ӧ������Ϻ�ת��IDLE״̬��
        break;
    case STOP_UNSAFE:
        //	if((safety_check::ErrorFlag[uc_ChannelNo]&REDUNDANT_SAFETY)!=0)  //Schedule ��ͣ��ͨ������ݱ�Ǿ������ָ���ѹ����ʱ��  dirui 20140730
        // 		    if(safety_check::RedundantSafetyFlg[uc_ChannelNo])
        // 		    {
        // 				board_config::ReleaseRedundantSafety(uc_ChannelNo,0);	     //������ౣ������
        // 				safety_check::RedundantSafetyFlg[uc_ChannelNo]=0;
        // 		    }
        if(safety_check::RedundantSafetyFlg[uc_ChannelNo])
            uc_ReleaseRedundantSafety = 1;

        //uc_CMD = PC_Unsafe_Msg;        //�趨�ظ���ϢΪAbnormal��Ϣ
        //UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
        st_status->m_btStopped_Status = STOP_UNSAFE;  //  20130830 ��
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
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //��Ӧ������Ϻ�ת��IDLE״̬��
        break;
    case STOP_POWER_OFF: //add by dirui 20131016
        st_status->m_btStopped_Status = STOP_POWER_OFF;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //?????????IDLE??;
        break;

    case STOP_AUXACKFAIL:   //add by dirui on 20131120
        //uc_CMD = PC_Unsafe_Msg;     //  yang  20150605  �����ʾFinish����
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
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_AUXACKFAILINSTART:
        //uc_CMD = PC_Unsafe_Msg;  //��ʱ��Ϊ��ʼʱӦ��ʧ��ͬ�����и���Ӧ��ʧ�ܴ�����ͬ  dirui 20140530
        st_status->m_btStopped_Status = STOP_AUXACKFAILINSTART;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_SMBUS_LOST:
        //uc_CMD = PC_Unsafe_Msg;  //��ʱ��Ϊ��ʼʱӦ��ʧ��ͬ�����и���Ӧ��ʧ�ܴ�����ͬ  dirui 20140530
        st_status->m_uw32Safe_Code = SMB_LOST_UNSAFE;
        st_status->m_btStopped_Status = STOP_SMBUS_LOST;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_CAN_BMS_LOST:
        //uc_CMD = PC_Unsafe_Msg;  //��ʱ��Ϊ��ʼʱӦ��ʧ��ͬ�����и���Ӧ��ʧ�ܴ�����ͬ  dirui 20140530
        st_status->m_uw32Safe_Code = CAN_BMS_UNSAFE;
        st_status->m_btStopped_Status = STOP_CAN_BMS_LOST;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        uc_IDLE = FIXEDUNSAFE;
        break;
    case STOP_CHANNELABNORMAL:
        //uc_CMD = PC_StopRpt_Msg;   //  20140703 yy  ����޷���ȷ��ʾ�������⡣
        st_status->m_btStopped_Status = STOP_CHANNELABNORMAL;
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
        uc_IDLE = FIXEDUNSAFE;
        break;
    /*case STOP_ACR_OVERTIME:    //  ACR��ʱ�������޷�������ʱ����Դ
    	//uc_CMD = PC_Unsafe_Msg;  //��ʱ��Ϊ��ʼʱӦ��ʧ��ͬ�����и���Ӧ��ʧ�ܴ�����ͬ  dirui 20140530
    	//UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = ACR_OVERTIME_UNSAFE;
      ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = IV_UNSAFE_TYPE;
    	ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = ACR_OVERTIME_UNSAFE;
      safety_check::SetUnsafe(uc_ChannelNo, ACR_OVERTIME_UNSAFE, 0,IV_UNSAFE_TYPE);
      st_status->m_bUnsafeType = IV_UNSAFE_TYPE;
      st_status->m_uw32Safe_Code = ACR_OVERTIME_UNSAFE;
    	st_status->m_btStopped_Status = STOP_ACR_OVERTIME;
    	//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus =FIXEDUNSAFE;
    	//EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;  //dirui 20130726��Ϣ����
    	uc_IDLE = FIXEDUNSAFE;
    	break;*/
    default:
        //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;        //20131023 dirui �������������ת����IDLE״̬�Ĵ���
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
        board_config::ReleaseRedundantSafety(uc_ChannelNo, 0);	    //������ౣ������
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
        Schedule_Manager_task::LockModeSwitch[uc_ChannelNo] = 0; //�����ȫ���Ϊ���尲ȫ��������ͨ���ָ��������  20140919 dirui
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
        ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;   //????why�Ƿ�������
    }

    SchTimeTrigger[uc_ChannelNo] = 0;                   //�����Ѿ�ȷ��Step�����������ֹͣ�ˣ�����Ҫִ�г�ʱ���㣬������    0911 dirui
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = CCS_OFF; //ͨ��״̬Ϊֹͣ
    //��ʱΪResume����
    ResumeManage[uc_ChannelNo].m_ResumeStepID = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;

    ResumeManage[uc_ChannelNo].m_ResumeCycleID = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;

    ResumeManage[uc_ChannelNo].m_ResumeTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_TestTime);
    ResumeManage[uc_ChannelNo].m_ResumeStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);
    ResumeManage[uc_ChannelNo].m_ResumeStatus = 0;

//	SendMetaSync(uc_ChannelNo);  //  20131029 he //zyx 171010

    ChannelInfo_Manager::Init_ChannelStep(uc_ChannelNo);  //���ͨ��step ��ֹ��ʷ���ݴ��ݵ��´����� dirui 20140923
    //EthernetParse::m_ReportStatus.m_stMSecondField.m_wChannelNumber=uc_ChannelNo;

    //if(EthernetParse::m_ReportStatus.m_btCMD  == ST_UNKNOWN)
    //    EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
    //if(EthernetParse::m_ReportStatus.m_btCMD == PC_Unsafe_Msg)
    //    memset((void *)&Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running,0,sizeof(ST_EVENT_SCHEDULE_RUNNING));
    //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);                 //��PC���ͻظ�
    SchReportMessageToPC(uc_ChannelNo, uc_CMD, &UnsafeContent[uc_ChannelNo]);   //��PC���ͻظ�
    memset((void *)&UnsafeContent[uc_ChannelNo].st_status_sch_running, 0, sizeof(ST_EVENT_SCHEDULE_RUNNING)); //dirui 20161128
//#if MCU_PWMFAN_ENABLE == 1
//     if(board_config::Feature_Enable_init.Mcu_PWMFan_Enable==1)
// 	    LPC4078PWM::SetPWM(PWM_HD1,PWM_CH5,100,5);   //when channel has been stopped ,set PWM value 60%   dirui 2014/07/31
//#endif


//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        //ֹͣSCHʱ�ж��Ƿ�SyncStop����ѡ zyx
        //ֱ���ж��¿����־λ�Ƿ�Ϊ��&&��ͨ���Ƿ�Ϊ���¿����ͨ��
        if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_bUseGroupTChamber)
            return;
        else if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucSyncStop == 1)
        {
            for(int auxUnitIdx = 0; auxUnitIdx < MAXMCUUNIT; auxUnitIdx++)        //changed by zc 04.25.2018 ����ÿ���¿�����־λ
            {
                for(int auxChnNo = 0; auxChnNo < MAXMTCCHAMBERNO; auxChnNo++)
                {
                    if( (uc_ChannelNo == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]) &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].OpenFlag>>auxChnNo)&0x01) == 0x01) )
                    {
                        /*��ѡ���¿�����SCHͬ��ֹͣ*/
                        unsigned char runON = 0;
                        CANBusParse::IV_SendTempChamberOutRunON(uc_ChannelNo,
                                                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo],
                                                                runON);
                        //ֹͣMTC�¶ȱ���
                        CANBusParse::IV_SendMTCTemperatureSaftyCheckOFF(uc_ChannelNo,
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_uwAuxVirtIndx[auxChnNo],
                                Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].m_ucType[auxChnNo]);
                        //���¿�������Ϊ��״̬
                        Schedule_Manager_task::CloseTemperatureChamber( auxUnitIdx, auxChnNo );

                    }
                    if(uc_ChannelNo == Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].uc_channel[auxChnNo]  &&
                            (((Schedule_Manager_task::TemperatureChamberSwitch[auxUnitIdx].usingFlag>>auxChnNo)&0x01) == 0x01) )
                        Schedule_Manager_task::RelsaseTemperatureChamber( auxUnitIdx, auxChnNo );  //�ͷ��¿����ռ��
                }
            }
        }
        //ClearMapAUXSafetyInfo(uc_ChannelNo,ALLTYPE,0,ALLMAPCHANNEL);//20131121 dirui ��Schedule����֮�����AUX�İ�ȫ��Ϣ
    }
//#endif

    for(unsigned short int uw16_n = 0; uw16_n <  MCU_MAX_NUMBER; uw16_n++)
    {
        if(!MultiMcuComm::St_Unit[uw16_n].uc_Active)  // �������һ��
            continue;
        for(unsigned char uc_m = 0; uc_m < 3; uc_m++)
            MultiMcuComm::UnregisterChannel(uw16_n, uc_ChannelNo,((UDP_MAP_AUX|UDP_MAP_IV_PARALLEL|UDP_MAP_CAN_BMS)>>uc_m));
    }

    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //��ǰ��Ϣ�������Ҫ���
}
/*****************************************************
// Function Name: SchStartRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         START״̬������Ϣ���봦�����
// Others:
//                        װ�������Step��Step.Mgr����ĸ����У�
//                        �������ʼ�������ã�����MV������Msg��Step.Mgr��������step��ת��RUNNING״̬
// transitional information:
//
// History��
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
// 		EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //�趨�ظ���ϢΪ������Ϣ�����ϱ�
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
// 		if(BuiltIn_DataManager::b_BuiltInAux)  //��Builtin ��������ʱ  qjm20180306
    if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
            || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))
    {   //190524,hpq
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck =0;
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck =0;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag = 0;
//       	if(CheckBit_32Bit(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_u16SafetyEnabed,MP_SCH_SAFTY_TYPE_AuxiVoltage)) // ����BUILT-IN ��ѹ��ȫ
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_bEnabled)
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck |= (1 << 0); //AUX_VOLTAGE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 0);
//         }
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Temperature.m_bEnabled) // ����BUILT-IN �¶Ȱ�ȫ
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
    ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //stepװ�ص�����������������ӿ�
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 1;    //����STEP������һ��SCHEDULE START/RESUME����Ҫ��ʼ��TEST TIME

    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;    //Resume��ʱ��Ҳ��ʼ���Ļ���ǰ�����е�ѭ�����Ͳ�������ˡ�dirui/2014/10/29
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
    Step_Manager_task::ACCURATE_IR[uc_ChannelNo] = 0;  //���֮ǰ���Ե�IRֵ  add by ZC 08.03.2018
    Step_Manager_task::INACCURATE_IR[uc_ChannelNo] = 0;   //���֮ǰ���Ե�IRֵ  add by ZC 08.03.2018
    Step_Manager_task::SIMU_I_IR[uc_ChannelNo] = 0;				  	//���֮ǰ���Ե�IRֵ  add by ZC 08.03.2018
    Step_Manager_task::SIMU_P_IR[uc_ChannelNo] = 0;				  	//���֮ǰ���Ե�IRֵ  add by ZC 08.05.2019

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;

    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_IsNeedCheck)
    {
        //IR�������
        if(IRspecial_Judge(uc_ChannelNo))
        {
            IRspecial_Judge_Process(uc_ChannelNo);//zyx 2105291
        }
    }
    StepMessage.MsgNumber = Step_Run_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);    //��step������ϢStep_Run_Msg
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING;
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendAUX_SafetyFlg[uc_ChannelNo] != 0)
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = WAITFORAUX; //����ȴ�����Ӧ�����
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
//                        RUNNING״̬������Ϣ���봦�����
// Others:
//                �ս����״̬���򷢳�Msg��Step.Mgr������Step���Գ�ʱ���Ƶȴ�Step.Mgr���ص�Msg��
//                ����ʱ�̱����ڴ�״̬�¡�(��ʱ��������ת��GOSTOP״̬)
// transitional information:
//
//
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchRunningRoutine(unsigned char uc_ChannelNo)
{
    if ((SchTimeTrigger[uc_ChannelNo] == 0) && (ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus < 3) )
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;  //0819

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule��ʱ���
    }
    if(!Smart_Battery::SMB_RunFlg && Smart_Battery::BeSupportSmbus )
    {
        Smart_Battery::SMB_RunFlg = true;

    }

//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)       //��ʱ��
    {
        SchTimeTrigger[uc_ChannelNo] = 0;       //�Ѿ���ʱ�˾Ͳ����ٴ���ʱ�����ˣ������ǡ�0911 dirui
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //�趨�ظ���ϢΪ������Ϣ
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
//                       RESUME״̬������Ϣ���봦�����
// Others:
//                װ�������Step��Step.Mgr����ĸ����У���Sync��Ϣ�����úͼ����ʼMV��
//                ����Msg��Step.Mgr��������step��ת��RUNNING״̬
// transitional information:
//                Step_Run_Msg
//
// History��
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
//         EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //�趨�ظ���ϢΪ������Ϣ�����ϱ�
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
// 		if(BuiltIn_DataManager::b_BuiltInAux)  //��Builtin ��������ʱ  qjm20180306
    if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
            || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))
    {   //190524,hpq
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck =0;
// 			  safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck =0;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag = 0;
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyVoltage.m_bEnabled) // ����BUILT-IN ��ѹ��ȫ
//         {
//             int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyVoltage.m_SafetyStartIndex;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck |= (1 << 0); //AUX_VOLTAGE;
//             safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 0);
//         }
//         if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_SafetyTemperature.m_bEnabled) // ����BUILT-IN �¶Ȱ�ȫ
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
    ChannelInfo_Manager::LoadNextStep(uc_ChannelNo, ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx); //stepװ�ص�����������������ӿ�
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_NextStepIdx;
    ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 1;	//����STEP������һ��SCHEDULE START/RESUME����Ҫ��ʼ��TEST TIME

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
        //IR�������
        if(IRspecial_Judge(uc_ChannelNo))
        {
            IRspecial_Judge_Process(uc_ChannelNo);//zyx 2105291
        }
    }
    StepMessage.MsgNumber = Step_Run_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage); 									 //��step������ϢStep_Run_Msg
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING; //����ȴ�����Ӧ�����   20140609 di  ���RESUME����
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendAUX_SafetyFlg[uc_ChannelNo] != 0)
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = WAITFORAUX; //����ȴ�����Ӧ�����
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
//                       GOPAUSE״̬������Ϣ���봦�����
// Others:
//                ��¼��ǰ����MV��Sync��PC����¼������ݵ㣬���ϴ���PC��
//                ����ֹͣMsg��Step.Mgr�����볬ʱ���ƣ��յ�Step.Mgr�Ļظ���ת��IDLE״̬��
//                ��ʱ֮��ֱ�ӵ���һ�����������ͨ����ת��IDLE״̬��
// transitional information:
//                Step_Stop_Msg
//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SchGopauseRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;


    StepMessage.MsgNumber = Step_Stop_Msg;
    Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);  //����Stepֹͣ��Ϣ��
    if (SchTimeTrigger[uc_ChannelNo] == 0)
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule��ʱ���
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ ׼������Step�ظ���Ϣ
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
    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)                                                                           //��ʱ��
    {
        SchTimeTrigger[uc_ChannelNo] = 0; //�Ѿ���ʱ�˾Ͳ����ٴ���ʱ�����ˣ������ǡ�0911 dirui
        //����һ�����������ͨ��
        StopChannel(uc_ChannelNo);                             //����һ�����������ͨ��
        //memset((void *)&EthernetParse::m_ReportStatus,0,sizeof(ST_STATUS_SCHEDULE));
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //�趨�ظ���ϢΪ������Ϣ
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_STOP_FAIL;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_START_FAIL;;
        SchReportMessageToPC(uc_ChannelNo, PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE; //����ͨ����ת��IDLE״̬

    }
}
/*****************************************************
// Function Name:SchGoStopRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       GOSTOP״̬������Ϣ���봦�����
// Others:
//                ��¼��ǰ�����¼��MV��Sync��PC����¼������ݵ㣬
//                ���ϴ���PC������ֹͣMsg��Step.Mgr�����볬ʱ���ƣ�
//                �յ�Step.Mgr�Ļظ���ת��NOTREADY״̬����ʱ֮��ֱ�ӵ���һ�����������ͨ����ת��NOTREADY״̬��
// transitional information:
//                Step_Stop_Msg,(PC_StopRpt_Msg)

//
// History��
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

        // 			switch(SendFinishFlag[uc_ChannelNo].StopStatus)  //����Stopԭ����
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
            ClearMapAUXSafetyInfo(uc_ChannelNo, ALLTYPE, 0, ALLMAPCHANNEL); //20131121 dirui ��Schedule����֮�����AUX�İ�ȫ��Ϣ
            ClearAUXLimits(uc_ChannelNo, 0);
        }
//#endif
        //����ΪMV���ַ���
        MetaVariable_Task::calculate_capacity(1, uc_ChannelNo);
        MetaVariable_Task::capacityStatus[uc_ChannelNo] = 0;
    }
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_AUXACKFAILINSTART)    //���ǿ�ʼ��û��������ô�����ڷ�stop��step������Ҫ��ʱ����   dirui 20140604
        {
// 			Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
            CANBusParse::IV_IfAuxNormal(uc_ChannelNo);
            //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //�趨�ظ���ϢΪ������Ϣ�����ϱ�
            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
            UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btStopped_Status = STOP_AUXACKFAILINSTART;
            //EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_MAINTAINCE;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
            SchReportMessageToPC(uc_ChannelNo, PC_Unsafe_Msg, &UnsafeContent[uc_ChannelNo]);

            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
            //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ
            return;
        }
    }
//#endif

    //#if(SUPPORT_CAN_BMS == 1)
    if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_CAN_BMS_LOST)
    {
// 		Step_Manager_task::SendLastLogforCSV(uc_ChannelNo);
        //EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //�趨�ظ���ϢΪ������Ϣ�����ϱ�
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
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ
        return;
    }
    //#endif

//========ADD BY ZC 12012017===========
// 	if(SendFinishFlag[uc_ChannelNo].StopStatus == STOP_SMBUS_LOST)
// 	{
// 		//EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;    //�趨�ظ���ϢΪ������Ϣ�����ϱ�
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
// 		//ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ
// 		return;
// 	}
//=======================================
    if (SchTimeTrigger[uc_ChannelNo] == 0)
    {
        StepMessage.MsgNumber = Step_Stop_Msg;
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage); //����Stepֹͣ��Ϣ

        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;

        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule��ʱ���
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ ׼������Step�ظ���Ϣ
    }

//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    //ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = OS_ServeProcess::TimeDiff(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].SysTimeScheduleTask);    //���ʱ���
    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)   //�ж��Ƿ�ʱ                                                                         //��ʱ��
    {
        SchTimeTrigger[uc_ChannelNo] = 0;    //�Ѿ���ʱ�˾Ͳ����ٴ���ʱ�����ˣ������ǡ�0911 dirui
        StopChannel(uc_ChannelNo);                        //�������һ�����������ͨ��

        //EthernetParse::m_ReportStatus.m_btCMD = PC_Error_Msg;    //�趨�ظ���ϢΪ������Ϣ
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btCMD_Source =  ST_SCHMANAGE;
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btErrorCode = MP_ERROR_OVERTIME; // ERROR_STOP_FAIL;
        //EthernetParse::m_ReportStatus.m_btCMD=PC_StopRpt_Msg; //װ�ػظ�����
        //EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btStopped_Status = STOP_EMERGENCY;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source = S_GOSTOP_OVERTIME;
        //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_CONTENT);
        UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btErrorCode = MP_ERROR_OVERTIME;
        SchReportMessageToPC(uc_ChannelNo, PC_Error_Msg, &UnsafeContent[uc_ChannelNo]);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = IDLE;
        //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);  //���DoMessageBuf��Ϣ
    }
    Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] = 0; //�´ο�ʼҪ����������Բ�
    IR_UnsafeFlg[uc_ChannelNo] = 0;

    //�����������Ĺرմ���
    if ( Pulse::Pulse_is_Running)  //�������������Ļ�������ֹͣ������塣
    {   Pulse::End(uc_ChannelNo);
        Pulse::Pulse_is_Running = false ;
    }

    //��ʱ��ǿ��ִ�йر�DIDO�������Ժ�Ҫ���Ǽ���������  dirui 2015/03/19
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
//                       FixedUnsafe״̬����
// Others:
//
// transitional information:
//
�����ԣ���ʱ����Unsafe
//
// History���ݲ�ʹ��
//
// ******************************************************/
void Schedule_Manager_task::Sch_FixedUnsafeRoutine(unsigned char uc_ChannelNo)
{
//#if SUPPORT_AUX==1
    if(CommonFunc::uc_Support_AUX)
    {
        static unsigned char bitReset = 1;
        //��aux ��ȫ�Ĵ��� ,����can�Ѿ��ָ����� ,��limitcount����
        unsigned short bitSafeCode = 0;
        if(bitReset)  //resetһ�Σ���ֹaux״̬����������(100ms)
        {
            for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
                CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bRefreshStatus = 0; //�ȴ�canBus����״̬
            bitReset = 0;
        }

        for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
        {
            if(!CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bExist)
                continue;
            if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bRefreshStatus)   //ȷ�����ѻָ�
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
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]); //�����Ϣ
}
/*****************************************************
// Function Name:Sch_DynamicUnsafeRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                       DynamicUnsafe״̬����
// Others:
//
// transitional information:
//                �����ԣ���ʱ����Unsafe

//
// History���ݲ�ʹ��
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
//                       ���ScheduleMessage
// Others:
//
// transitional information:
//

//
// History��
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
//                        ��������ر�ͨ��
// Others:
//
// transitional information:
//

//
// History��
//
// ******************************************************/

//������Ҫ�ر�ͨ��ʱ����stepLen������unsafeʱ�����ܷ����
//�¶��޸� 20130828
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
        switch(ChannelInfo_Manager::ChannelInfo[uc_idx].m_CtrlState)   //������صĹ�ͨ��ʱ��
        {
        case CCS_I:
            DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr; //&IBT_Base::SetCtrlCloseCh[0];      //��ʱ����IBT�����Ժ�Ҫͳһ��OS_LogicProcess::SetCtrlVltToOff_ptr;
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
        case DISPOSABLE_ASSIGNED:                        //�Ѿ�������������
            EngineOverTimeTriger[uc_idx] = 1;
            MsTimeEngineTask[uc_idx] = CommonFunc::GetSystemTime(); //OS_ServeProcess::OS_Time;
            break;
        case DISPOSABLE_RUNNING:          //������
            unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&SysTimeEngineTask[uc_idx]);
//             MsTime MyMsTime;
//             MyMsTime.Second = SysTimeEngineTask[uc_idx].Second;
//             MyMsTime.Us100 = SysTimeEngineTask[uc_idx].Us100;
//             TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);

            //MsTimeEngineTask[uc_idx]=OS_ServeProcess::TimeDiff(SysTimeEngineTask[uc_idx]);    //���ʱ���

            if(EngineOverTimeTriger[uc_idx] == 1 && TimeTicks >= uw32_OverTime)       //��ʱ��
            {
                EngineOverTimeTriger[uc_idx] = 0;
                //�������ʧ�ܴ���

            }
            break;
        case DISPOSABLE_FINISHED:           //����
            EngineOverTimeTriger[uc_idx] = 0;
            ChannelInfo_Manager::ChannelInfo[uc_idx].m_CtrlState = CCS_OFF;
            break;
        case DISPOSABLE_ERROR:            //����
            //�������ʧ�ܴ���
            break;
        case DISPOSABLE_DEAD:             //��ʷ��¼�Ѿ�����
            break;
        default:
            break;
        }

        // ����Ӧ�� ͨ��Schedule��ʼ��
        ResumeManage[uc_idx].m_ResumeStatus = 0;
    }
}
/*****************************************************
// Function Name: SendMsgToSchedule
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   ��Schedule������Ϣ�Ժ����������ʵ��
// Others:
//
// History��
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
        return     -1;   // ��Ϣ����ʱ��������

    //����Msg���ȼ�
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
    case AUX_Unsafe_Msg:								//����AUX�쳣��Ϣ
    case AUX_ACKFAIL_Msg:								//����AUXӦ��ʧ�ܴ���
//#endif
        ScheduleMessage.MsgPriority = 0;
        StartScheduleUnsafe[uc_ChannelNo] = UnsafeContent[uc_ChannelNo]; //?? 200420 zyx
        break;
    default:
        return -1;

    }
    unsigned char MsgRecordIndex = MsgBufManage[uc_ChannelNo].RecordMsgEmpty[MsgBufManage[uc_ChannelNo].EmptyCount - 1];
    MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount] = MsgRecordIndex;  //��¼��Message�������ĸ��ط���
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
//                   ɾ��������ָ��һ����Ϣ
// Others:
//
// History��
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
// History��
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
// History��
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
        return;   //�Ѿ����͹�һ���ˣ�û��Ҫ�ٷ�һ�Ρ�
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
//                      ȡ��SetVariable����
// Others:
//
// transitional information:
//

//
// History��
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
//                      ȡ��Schedule״̬
// Others:
//
// transitional information:
//
//
// History��
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
//                     ��¼LS��ر���
// Others:
//
// transitional information:
//

//  LS_Current,LS_Voltage,LS_InternalResistance
// History��
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
//                      �ı�Setvariable����
// Others:
//
// transitional information:
//

//
// History��
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
//                      �ı�Setvalue����
// Others:
//
// transitional information:
//

//
// History��
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
//                      ������ʱ����
// Others:
//
// transitional information:
//                ʱ�䰴�����趨ֵ����

//
// History��
//
// ******************************************************/
//void Schedule_Manager_task::SendSDLIdleData(unsigned char uc_ChannelNo,unsigned long long TimeInterval)
void Schedule_Manager_task::SendSDLIdleData(unsigned char uc_ChannelNo)
{
// 	unsigned char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH ״̬���ı��� ����������������
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
            IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //ѭ������ xS
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
            IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //ѭ������ 100ms

        CommonFunc::GetTimeTicksUpdate( &SDLIdleMsTime[uc_ChannelNo]);
// 		SDLIdleMsTime[uc_ChannelNo].Us100 = OS_ServeProcess::OS_Time.Us100;
    }
}
/*****************************************************
// Function Name:SendSDLShowData
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      ������ʾ����
// Others:
//
// transitional information:
//                ���3s

//
// History��
//
// ******************************************************/
//void Schedule_Manager_task::SendSDLShowData(unsigned char uc_ChannelNo,unsigned long long TimeInterval)
void Schedule_Manager_task::SendSDLShowData(unsigned char uc_ChannelNo)
{
#if ((USE_AS_AUXMCU == 0 ) && (SCH_STATUS_REPORT_ENABLE  == 1))
    if(ScheduleStatusReport::uc_uc_BreakNet_Counter > BREAKNET_NUMBER)  // SCH ״̬���ı��� ����������������
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
        IVSDL::Check_BuiltInAuxForLog(uc_ChannelNo, SDL_DATA_FOR_IDLE); //ѭ������
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
//                      �������������λ��ʧ����
// Others:
//
// transitional information:
//               ����������1����û����Ӧ��ǿ�йر�ͨ��

//
// History��
//
// ******************************************************/
void Schedule_Manager_task::SoftWare_WatchDog(void)   //20130904,modified by hepeiqing
{
    if(EthernetParse::m_bResetSchedule)
    {
        for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
        {
            if(IfSchRunning(uc_i))           //���е�ͨ��Ҫִ��stop���������ؿ��Ǹ��ֻظ�����λ��ʱ�Ѿ�����
            {
                if(ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus == GOSTOP)
                    continue;
                ChannelInfo_Manager::ChannelInfo[uc_i].ScheduleStatus = GOSTOP;
                //SendFinishFlag[uc_i].StopStatus = STOP_MANUAL;    // Comment out by DKQ on 09.11.2013
                SendFinishFlag[uc_i].StopStatus = STOP_EMERGENCY;  // Add by DKQ on 09.11.2013
                SendFinishFlag[uc_i].SubStopCode = EthernetParse::m_bResetSchedule;

                SchTimeTrigger[uc_i] = 0; //�峬ʱ��־λ
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
//                      �����жϴ���
// Others:
//
// transitional information:
//

//
// History��
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
    EthernetParse::m_ReportStatus.m_btCMD = CMD;        //�趨�ظ���Ϣ
    EthernetParse::m_ReportStatus.m_btSCHEvent_Source = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_Event_Source;
    if(CMD == PC_Jump_Msg)
    {
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_wNextStep_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.GotoStepID;  // PC_Switching_Msg��Ϣ�����ȴ���ת��Ϣ
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btEquation_Index = DoMessageBuf[uc_ChannelNo].ScheduleMessage.m_Message.m_RunMsg.EquationID;   //EquationID ��Parse��Ҫ���
        EthernetParse::m_ReportStatus.m_st_LimitReachStatus.m_btIfReached = 1;
    }
    else if((CMD == PC_StopRpt_Msg) || (CMD == PC_Unsafe_Msg) || (CMD == PC_Error_Msg) || (CMD == PC_GoStop_Msg))
    {
        UnsafeContent->st_status_sch_running.m_bUnsafeType = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType;
        UnsafeContent->st_status_sch_running.m_uw32Safe_Code = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus;    //�ظ���Safety״̬(��ʱ���ڴ�������channelinfo)
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
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[NoActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = 0.1f * ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity); //?Y�?��??a0.1C
    Schedule_Manager_task::IRHasBeenRun[uc_ChannelNo] = 1;
}


//#if SUPPORT_AUX==1
//AUX�����ش���///////////////////////////////////////////////////////////////////////
/*****************************************************
// Function Name:SendSafetyInfoToAUX
// Version:        V1.0
// Date:                2013/11/20
// Description:
//                     ���Ͱ�ȫ��Ϣ������ͨ��
// Others:
//
// transitional information:
//
//
// History��
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
//     //�޸�ing
// //     unsigned char FactorType ;
//     float AuxFactor_META = 1;
//     unsigned char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
//     float AuxFactor_TIME = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_DV_Time_Max;

//     // ����map��Ϣ����AuxType
//     for(unsigned char uc_Auxtype = 0; uc_Auxtype < MAXAUXTYPE; uc_Auxtype++) //ֻ�ں����͵ĸ������ͣ������Ķ��ٸ�ͨ��
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
//             AuxFactor_META = ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Humidity.m_fDelta;		//ʪ�ȱ���
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
// //             FactorType = LOG_DELTA_META;   //dxdt original data �Ȳ�����
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
    //������ȫ�쳣����
    //	UnsafeMessage[uc_ChannelNo]=DoMessageBuf[uc_ChannelNo].ScheduleMessage;
//     memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
//     EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;
    UnsafeContent[uc_ChannelNo].m_btCMD = PC_Unsafe_Msg;
    UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source = ST_CANPARSE;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus = UnsafeContent[uc_ChannelNo].st_status_sch_running;
//     EthernetParse::m_ReportStatus.m_st_RunningStatus.m_btSubCode = DoMessageBuf[uc_ChannelNo].ScheduleMessage.st_status_sch_running.m_btSubCode;
    SendFinishFlag[uc_ChannelNo].StopStatus = STOP_AUXUNSAFE;
    //ClearScheduleMessage(&DoMessageBuf[uc_ChannelNo]);                              //��ǰ��Ϣ�������Ҫ���
}

void Schedule_Manager_task::ClearMapAUXSafetyInfo(unsigned char uc_ChannelNo, unsigned char Auxtype, unsigned char VirtualID, unsigned char ClearType)
{
    //����Map �������ذ�ȫ��Ϣ
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
    //�ú���ȷʵSTEP ID��
    unsigned short StepID = 0;
    Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].bNeedWattingAck = 1;
    CANBusParse::IV_ClearLimitsToAUX(uc_ChannelNo, Clear4New, StepID);	//���AUX���Limits
    return 0;
}
void Schedule_Manager_task::SchWaitForAuxRoutine(unsigned char uc_ChannelNo)
{
    St_StepMessage StepMessage;

    if (SchTimeTrigger[uc_ChannelNo] == 0) //Limit ����LoadStepͬʱ��飬�����Ѿ����������� ���Բ��ٲ���dirui 20131220
    {
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        SchTimeTrigger[uc_ChannelNo] = 1;                    //Schedule��ʱ���
    }

    //�����������safetyӦ���
    if(CANBusParse::IV_CheckAuxSNRes(uc_ChannelNo, SAFETYSN))
    {
        SchTimeTrigger[uc_ChannelNo] = 0;
        CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, SAFETYSN); //������Ҫ���
        StepMessage.MsgNumber = Step_Run_Msg;
        Step_Manager_task::SendMsgToStep(uc_ChannelNo, StepMessage);                                     //��step������ϢStep_Run_Msg
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = RUNNING;
        Step_Manager_task::TimeRefreshFlag[uc_ChannelNo] = 0;
//        SendTirgger[uc_ChannelNo] = 0;
        return;
    }
//     MsTime MyMsTime;
//     MyMsTime.Second = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Second;
//     MyMsTime.Us100 = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask.Us100;
    unsigned long long TimeTicks = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask);

    if(SchTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)    //Aux_OverTime)  //��ʱ��
    {
        St_ScheduleMessage SchMessage;
        memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
        SchTimeTrigger[uc_ChannelNo] = 0;       //�ȴ�����û�аѱ����

        for(unsigned char uc_i = 0; uc_i < CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNCount; uc_i++)
        {
            if(!CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNInfo[uc_i].m_Checked)
            {
                SchMessage.st_status_sch_running.m_btLost_Unit = CANBusParse::st_SNCheckTab[uc_ChannelNo][SAFETYSN].m_SNInfo[uc_i].m_AuxUnitID;
                break;
            }
        }

        CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, SAFETYSN); //������Ҫ���
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
        SendFinishFlag[uc_ChannelNo].StopStatus = STOP_AUXACKFAIL;  //���ҪͬPC ������ʲô�����ϱ�
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_AUXSafetyStatus = AUX_ACK_FAILED;
        Schedule_Manager_task::UnsafeContent[uc_ChannelNo] = SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage);

    }

}


void Schedule_Manager_task::UseTemperatureChamber( unsigned char uc_channel, unsigned char AuxUnitIdx, unsigned char AuxChnNo ) //ռ���¿���  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].usingFlag |= 0x01<<AuxChnNo;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].uc_channel[AuxChnNo] = uc_channel;
}


void Schedule_Manager_task::RelsaseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ) //�ͷ��¿����ռ��  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].usingFlag &= ~(0x01<<AuxChnNo);
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].uc_channel[AuxChnNo] = 0xFF;
}

void Schedule_Manager_task::OpenTemperatureChamber( unsigned short m_uwAuxVirtIndx, unsigned char m_ucType, unsigned char AuxUnitIdx, unsigned char AuxChnNo) //��¼�¿��俪״̬ 20160707  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].OpenFlag |= 0x01<<AuxChnNo;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_ucType[AuxChnNo] = m_ucType;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_uwAuxVirtIndx[AuxChnNo] = m_uwAuxVirtIndx;

}


void Schedule_Manager_task::CloseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo )   //��¼�¿����״̬ 20160707  changed by zc 04.25.2018
{
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].OpenFlag &= ~(0x01<<AuxChnNo);
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_ucType[AuxChnNo] = 0xFF;
    Schedule_Manager_task::TemperatureChamberSwitch[AuxUnitIdx].m_uwAuxVirtIndx[AuxChnNo] = 0xFFFF;
}

void Schedule_Manager_task::ScheduleToStopIdle(unsigned char uc_ChannelNo)
{
    if(Step_Manager_task::bNeedCheck[uc_ChannelNo]
            ||ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_LimitCount != 0
            ||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus != STEPIDLE)  //20180420 LOG������Ȼ������BUG
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
        SchTimeTrigger[uc_ChannelNo]=0;  //������Ҫת��Gostop״̬��ִ����Step����Stop��Ϣ�Ĵ���
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
