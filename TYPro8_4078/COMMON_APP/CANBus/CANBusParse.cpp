/*****************************************************************************
// 程序文件      :CANBusParse.cpp     CAN收发命令的解析
// 文件描述      :LPC4078 MCU board
// 编写日期(MM.DD.YYYY)      :2013.11.18
*****************************************************************************/
#define __USE_C99_MATH
#include "../Entry/includes.h"
//--------------------------------------------------------------------------------------------------------------
UInt16 CANBusParse::Priority = PRIORITY4;
UInt16 CANBusParse::TaskID = CANBUSPARSE_TASK;

ST_CAN_SN  CANBusParse::st_CAN_SN;                             //各种报文的SN号
unsigned short CANBusParse::StepID[MAXCHANNELNO];
unsigned int CANBusParse::INFU_Error;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

//Aux
//#if(SUPPORT_AUX==1)
#if(USE_AS_AUXMCU == 1) //aux
St_AuxStatusRpt  CANBusParse::st_AuxRptInfoMgr[MAX_IVMCU_PER_SYS];
#else  //IV
// St_TimeDiff      CANBusParse::SyncTimediff;
ST_IVAskTable    CANBusParse::st_IV2AuxInfoMgr[MAXCHANNELNO][MAXAUXUNIT]; //IV 状态管理(安全检查)
St_SNTable       CANBusParse::st_SNResTable[MAXCHANNELNO][MAXSNTYPE];     //limit SN号回传结果
St_SNTable       CANBusParse::st_SNCheckTab[MAXCHANNELNO][MAXSNTYPE];   //limit SN号检查表
St_AuxInfoRes    CANBusParse::st_AuxInfoRes[MAXCHANNELNO];                //辅助上报信息匹配   IV
St_ClrLimitAck   CANBusParse::st_ClrLimitAckMgr[MAXCHANNELNO];          // clear limit ack 检查
#endif
// MsTime           CANBusParse::SyncAuxTime;
unsigned char    CANBusParse::AuxInfoShiftBit[MAXCHANNELNO];           //状态结构移位bit后的固定值(防止每次计算)
//#endif

//CAN BMS //
//#if(SUPPORT_CAN_BMS==1)
#if(USE_AS_AUXMCU == 1)
St_CANBMS_AuxInfo      CANBusParse::m_CANBMS_AuxInfo;                //AUX BMS info
unsigned char CANBusParse::DIDO_Ctrl = 0;
ST_AuxMultiMsgManage   CANBusParse::m_BMS_AuxMultiMsgManage;        //170828 zyx
unsigned char CANBusParse::AuxBrdCstReciveIdex = 0;
#else
bool                   CANBusParse::CANBMS_bWork; // zyx 180518
bool                   CANBusParse::CANBMS_bMap = false; //zyx 181010 make sure can Map was init.
St_CANBMS_ConfigBuf    CANBusParse::m_CANConfig;           //IV Metavariable table
St_CANConfigAssignTab  CANBusParse::m_CANAssignMD5[MAXCHANNELNO];
St_BMS_SendInfo        CANBusParse::m_BMS_SendInfoMgr[MAXCHANNELNO];	               //发送信息管理
St_BMS_SendUnit        CANBusParse::m_BMS_BroadCstBuf[MAXCHANNELNO][BMS_BROAD_CST_COUNT];
St_CANBMS_SendBuf      CANBusParse::m_BMS_SendBuf[MAXCHANNELNO] ;              //IV MCU Send Buf
St_BMSUnsafeInfo       CANBusParse::m_BMS_OverTime[MAXCHANNELNO];  //
St_BMS_AckConfirmMgr   CANBusParse::m_BMSAckConfirmMgr[MAXCHANNELNO];

unsigned char  CANBusParse::m_BMS_SendBuf_RunOn[MAXCHANNELNO];              //IV MCU Send Buf Run Enable
unsigned char  CANBusParse::m_ParaBufIdx[CAN_CMD_PARA_MAX];
St_ParaAckMgr  CANBusParse::m_Para_MasterAckMgr[CAN_CMD_PARA_MAX][PARA_MAX_BUF];  //10个相同命令缓存
St_SubMaster_Confirm CANBusParse::m_SubMasterAckMgr[3];
MsTime          CANBusParse::m_ParaDataUpdateTick;
const unsigned int   CANBusParse::Parallel_BroadcastID = 0xFF;
unsigned char CANBusParse::m_BroadCstIdex[MAXCHANNELNO];
bool 					CANBusParse::m_SubACKUnitFlg[MAXMCUUNIT];   //dirui 2017 06/30
St_PowerAdjAckMgr CANBusParse::m_Para_PowerAdjAckMgr;
UWord32         CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK;
float           CANBusParse::f_DeltaValueTime_back[MAXCHANNELNO];
MP_CAN_MSG CANBusParse::AssembleMsg;
#endif
MP_CAN_MSG CANBusParse::CAN_ParseRxMsg;
//#endif

MsTime CANBusParse::lastAuxTriggerTime[MAXCHANNELNO];
bool CANBusParse::lastAuxTrigger[MAXCHANNELNO];
bool CANBusParse::lastCANBMSData[MAXCHANNELNO];
St_CANAUX_Broadcast_Info CANBusParse::m_CANAUX_Broadcast;
#pragma arm section //结束外存变量定义

//--------------------------------------------------------------------------------------------------------------


void CANBusParse::Init()
{
    CANFunc::BMS_Filter.bufIndexEnqueue = 0; //1712211 hpq
    CANFunc::BMS_Filter.CanID_Count = 0;
    CANFunc::BMS_Filter.bufCounter[0] = 0;
    CANFunc::BMS_Filter.bufCounter[1] = 0;
    st_CAN_SN.m_MsgSN  = 0;
    st_CAN_SN.m_LimitSN = 0x800;  //LIMIT SN号初始化
    st_CAN_SN.m_AnyLimitSN = 0x800;

    for(int i = 0; i < MAXCHANNELNO; i++)
        lastAuxTriggerTime[i] = CommonFunc::GetSystemTime();
#if (SUPPORT_PARALLEL == 1)
    CommonFunc::GetTimeTicksUpdate(&m_ParaDataUpdateTick);
    memset((void*)m_ParaBufIdx, 0, CAN_CMD_PARA_MAX);
    for(unsigned char cmdIdx = 0; cmdIdx < CAN_CMD_PARA_MAX; cmdIdx++)
    {
        for(unsigned char bufIdx = 0; bufIdx < PARA_MAX_BUF; bufIdx++)
        {
            CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_MasterID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx;
            CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].CmdStatus = eParaAckNone;
            CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_SubUnitCnt = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1;
            CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_ReSendCnt = 0;
            CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].SN = 0;
            memset((void*)&CANBusParse::m_Para_MasterAckMgr[cmdIdx][bufIdx].m_SubStatus [0], eParaAckNone, MAXUNITNO);
        }
    }
#endif


#if(USE_AS_AUXMCU == 1)  //aux mcu
    //----------------------------CAN BMS for Aux-----------------------------//
    //#if(SUPPORT_CAN_BMS == 1)
    m_CANBMS_AuxInfo.m_validMsgCnt = 0;
    m_CANBMS_AuxInfo.m_bAuxForward = 0;
    m_CANBMS_AuxInfo.m_IVCANID = 0;
    memset(&m_BMS_AuxMultiMsgManage, 0, sizeof(ST_AuxMultiMsgManage));        //170828 zyx

    //#endif
    //----------------------------Aux MCU-----------------------------//
#if (USE_AUX_STATUS_REPORT == 1)
    MsTime intervalTick;
    intervalTick.Us100 = 0;
    intervalTick.Second = 0;
    for(unsigned char mcu_idx = 0; mcu_idx < MAX_IVMCU_PER_SYS; mcu_idx++)
    {
        if(!Aux_DataManager::m_IVMCU_ID_Info[mcu_idx].m_Valid)
            continue;
        intervalTick.Us100 = 200 * mcu_idx;
        CommonFunc::GetTimeTicksUpdate(&st_AuxRptInfoMgr[mcu_idx].m_RptTick);	 // reset counter
        CommonFunc::add_MsTime(&st_AuxRptInfoMgr[mcu_idx].m_RptTick, &intervalTick);

        for(unsigned char IvChnIdx = 0; IvChnIdx < MAXCHANNELNO; IvChnIdx++)
        {
            AuxInfoShiftBit[IvChnIdx] = 6 * (IvChnIdx % 8);
            st_AuxRptInfoMgr[mcu_idx].m_bRptInfo[IvChnIdx] = 1;//初始化为都需要上传标志
        }
    }
#endif

#else  //IV MCU

    //-------------------------CAN BMS for IV---------------------------//
    // #if(SUPPORT_CAN_BMS == 1)
    for(unsigned char IvChnIdx = 0; IvChnIdx < MAXCHANNELNO; IvChnIdx++)
    {
        m_CANAssignMD5[IvChnIdx].m_cfgBufIdx = -1; //
        m_CANConfig.m_bufCnt = 0;
        m_CANAssignMD5[IvChnIdx].m_insertPos = -1;
        m_BMS_SendInfoMgr[IvChnIdx].m_bBroadcast = 0;
        m_BMS_SendBuf[IvChnIdx].m_validMsgCnt = 0;
        m_BMS_SendBuf[IvChnIdx].m_glbStopAll = 0;
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_SendIdleDataTick);
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_UnsafeTime);
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_IdleDataRptTick);
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_CANBMS_RptLimitTick.m_CANBMS_FastRptLimitTick);
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_CANBMS_RptLimitTick.m_CANBMS_MidRptLimitTick);
        CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IvChnIdx].m_CANBMS_RptLimitTick.m_CANBMS_SlowRptLimitTick);
        m_BroadCstIdex[IvChnIdx] = 0;
        f_DeltaValueTime_back[IvChnIdx] = IV_SUB_SDL_TIME_NORMAL;
    }
    for(unsigned char bufidx = 0; bufidx < 20; bufidx++)
    {
        m_CANConfig.m_buf[bufidx].m_bEnable = 0;
        m_CANConfig.m_buf[bufidx].m_RegisterBit = 0;

    }
    CANBusParse::CheckCanBmsMap();
    if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
    {
        m_Para_PowerAdjAckMgr.m_IV_ID = CANFunc::SourceBoardID;
        m_Para_PowerAdjAckMgr.m_M0UnitCnt = ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits;
        // 	m_Para_PowerAdjAckMgr.m_M0UnitIdx = ;
        m_Para_PowerAdjAckMgr.CmdStatus = eParaAckNone;
        m_Para_PowerAdjAckMgr.m_ReSendCnt =0;
        for(unsigned char i=0; i<4; i++)
        {
            m_Para_PowerAdjAckMgr.m_M0ID[i] = 129+i;
            m_Para_PowerAdjAckMgr.m_M0Status[i] = eParaAckNone;
        }
    }
    CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK;
    // #endif
    //--------------------Aux (IV MCU)-------------------------//
//#if (SUPPORT_AUX == 1 && USE_AUX_STATUS_REPORT == 1)
#if (USE_AUX_STATUS_REPORT == 1)
    if(CommonFunc::uc_Support_AUX)
    {
        IV_InitStatusMgr();
//         CommonFunc::GetTimeTicksUpdate(&SyncAuxTime);
    }
#endif
#endif

}


void CANBusParse::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CAN] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_CAN);
#endif
// 	MsTime test_time = {0,0};  ;
    //  static unsigned long long counts;
// 	CommonFunc::GetTimeTicksUpdate(&test_time);





    //for BMS DoWork
    // #if(SUPPORT_CAN_BMS == 1)
#if (USE_AS_AUXMCU == 0) //IV  
    Parallel_DoMasterAckCheck();
    if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
        CANBusParse::PowerAdj_DoAckCheck();

//    if(ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum > 0)
    if(CANBMS_bWork)
        CANBMS_IVDoWork();
    else
    {
        for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
        {
            if(!m_BMS_OverTime[ChnIdx].m_bCANBMSAlive)
                m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Normal;
        }
    }
    for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
    {
        if(IVSDL::m_GrpInfo4SDL[ChnIdx].State == eGroupState_Submaster)//1903191 zyx
            IVSDL::Check_Submaster_Report(ChnIdx);//zyx 181015
    }
#else
    if(Aux_DataManager::CANBMSEnableFlag)
    {
        CANBMS_AuxDoWork();
    }
#endif

    //#endif

    for(unsigned char CANx = CAN1; CANx <= RUDPNET; CANx++)
        CANParse(CANx);

    CANBMS_BroadcastAuxMsg();
//#if (SUPPORT_AUX == 1)
    if(CommonFunc::uc_Support_AUX)
    {
#if (USE_AS_AUXMCU == 0) //IV       
        for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
        {
            if(Channel_MapAuxInfo::m_Total_In_OneIVCh[ChnIdx] > 0)
                IV_DoAuxStatusCheck(ChnIdx);
        }
#else                        //aux 	
        Aux_CANStatusRpt();
#endif
    }
//#endif

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CAN] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_CAN);
#endif

    //		counts= CommonFunc::GetTimeTicks(&test_time);
}

/******************************************************************************
** Function name:		CANParse
**
** Descriptions:    CAN命令的解析（IV和辅助公用）
**
** parameters:		 none
**
** Returned value:	 none
**
** Created Date:      2013/11/22
**
** Revision Dates:

******************************************************************************/
void CANBusParse::CANParse(unsigned char CANx)
{
    //1. 从接收缓冲区 拿数据 2.将解析后的命令/数据缓存，至事件确认缓冲区 （活跃），
// 	unsigned long ParseNum = 0,RxBufLength;
    unsigned long ParseTotal = 0;
    unsigned char DequeueOK = 0;
    //MP_CAN_MSG  ParseRxMsg;
		memset(&CANBusParse::CAN_ParseRxMsg,0,sizeof(MP_CAN_MSG));
    //RxBufLength = CANFunc::CQ_CanRxBuffer[CANx].m_u32Length;
    unsigned char CmdType;
    if(CANFunc::CQ_CanRxBuffer[CANx].m_u32Length == 0)
        return;
// 	ParseNum = CANFunc::CQ_CanRxBuffer[CANx].m_u32Length;
    ParseTotal = CANFunc::CQ_CanRxBuffer[CANx].m_u32Length;

//    for(unsigned long ParseIdx = 0; ParseIdx < CANFunc::CQ_CanRxBuffer[CANx].m_u32Length; ParseIdx++)
    for(unsigned long ParseIdx = 0; ParseIdx < ParseTotal; ParseIdx++)
    {
        DequeueOK = 0;
        if(CANx != RUDPNET)
            NVIC_DisableIRQ(CAN_IRQn);
        DEQUEUE(CANFunc::CQ_CanRxBuffer[CANx],CAN_RX_BUF_LENGTH,CANBusParse::CAN_ParseRxMsg,DequeueOK);
        if(CANx != RUDPNET)
            NVIC_EnableIRQ(CAN_IRQn);
        //IRQFunc::EnableIRQ(IRQ1,IRQ2);
        if(!DequeueOK)
            return;
        if(CANBusParse::CAN_ParseRxMsg.bBMSCmd)
            CmdType = BMS_CMD_TYPE;
        else
            CmdType = CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.CmdType;


        switch(CmdType)
        {
        case AUX_CMD_TYPE:
//#if (SUPPORT_AUX == 1)
            if(CommonFunc::uc_Support_AUX)
                Aux_CmdParse(&CANBusParse::CAN_ParseRxMsg);
//#endif
            break;
        case Parallel_CMD_TYPE:

#if (USE_AS_AUXMCU == 0 )   //只有IV　ｕｎｉｔ才处理并联信息
            Parallel_CmdParse(&CANBusParse::CAN_ParseRxMsg);
#endif
            break;
        case BMS_CMD_TYPE:
            //	#if( SUPPORT_CAN_BMS == 1)
#if (USE_AS_AUXMCU == 0) //IV  
            CANBMS_IVParseMsg(CANx, &CANBusParse::CAN_ParseRxMsg);
#else
            CANBMS_AuxParseMsg(CANx, &CANBusParse::CAN_ParseRxMsg);
#endif
            //	#endif
            break;
        case DSP_PARA_SET:
#if (USE_AS_AUXMCU == 0 )
            DSP_Feedback_CmdParse(CANx, &CANBusParse::CAN_ParseRxMsg);
#endif
            break;
        case Power_M0_ADJ:
#if (USE_AS_AUXMCU == 0 )   //只有IV　ｕｎｉｔ才处理并联信息
            Power_M0_CmdParse(CANx,&CANBusParse::CAN_ParseRxMsg);
#else
            Power_AUX_CmdParse(CANx, &CANBusParse::CAN_ParseRxMsg);
#endif
            break;
#if (USE_AS_AUXMCU == 1 )
        case SYSTEMCTRL_SET:
            switch(CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.Cmd)
            {
            case CAN_WATCHDOG:
            case CAN_CMD_SYSTEM_ABNORMAL:
                AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=true;
                //	 AuxSysCtrl_task::SysCtrl_ErrorCode[ParseRxMsg.Un_CANID.m_CANID.SourceID-1]|=SYSALARM;
                break;
            case CAN_CMD_SYSTEM_BACKTONORMAL:
                AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=false;
                //	AuxSysCtrl_task::SysCtrl_ErrorCode[ParseRxMsg.Un_CANID.m_CANID.SourceID-1]&=SYSALARM;
                break;
            case CAN_CMD_SYSTEM_RUN:
                AuxSysCtrl_task::SysCtrl_RunFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=true;
                if(CANBusParse::CAN_ParseRxMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus>0)
                    AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=true;
                else
                    AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=false;
                break;
            case CAN_CMD_SYSTEM_STOP:
                AuxSysCtrl_task::SysCtrl_RunFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=false;
                if(CANBusParse::CAN_ParseRxMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus>0)
                    AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=true;
                else
                    AuxSysCtrl_task::SysErrorFlg[CANBusParse::CAN_ParseRxMsg.Un_CANID.m_CANID.SourceID-1]=false;
                break;
            }
#endif
        default:
            break;
        }
    }//for

}


/************************************************************************/
/*                       AUX CMD Parse                                  */
/************************************************************************/
//#if(SUPPORT_AUX == 1 )
void CANBusParse::Aux_CmdParse( MP_CAN_MSG * CAN_Msg)
{
//     unsigned short  CAN_ConfirmNo	;
    switch(CAN_Msg->Un_CANID.m_CANID.Cmd)
    {
    case CAN_WATCHDOG:
    {
#if (USE_AS_AUXMCU == 1 )
        CAN_WatchdogTask::AuxACKRefershFlg = 1;
        CAN_WatchdogTask::m_IVUnitID = CAN_Msg->Un_CANID.m_CANID.SourceID;

        CAN_WatchdogTask::m_Ack = CAN_Msg->Un_Content.m_WatchdogACK.SN + 1;
        CAN_WatchdogTask::AuxAckIVSN(CAN_WatchdogTask::m_IVUnitID, CAN_WatchdogTask::m_Ack, CAN2);
#else
// 			unsigned m_AuxUnitID=CAN_Msg->Un_CANID.m_CANID.SourceID;
// 			if(CAN_Msg->Un_Content.m_WatchdogACK.AckType==WATCHDOG_AUXSEND)
// 				CAN_WatchdogTask::m_CanWatchdogACK.m_AuxAck[m_AuxUnitID]=CAN_Msg->Un_Content.m_WatchdogACK.SN;
#endif
        break;
    }
    case CAN_CMD_CONFIRM_ACK:
//         CAN_ConfirmNo = CAN_Msg->Un_Content.m_ConfirmAck.SN;
        //	CANMsgBuffering::ConfirmMsg(CAN1,CAN_ConfirmNo );   //清除事件缓存EventBuffering 中的活跃区标志 告知其不要存入非活跃区
#if(USE_AS_AUXMCU ==0)
        IV_AckProcess(CAN_Msg);  //  Di 20140521
#endif
        break;
        //---------------AUX MCU接收命令--------------//
#if(USE_AS_AUXMCU == 1)
    case CAN_CMD_DOWNLOAD_LIMITS:
        //测试
        Aux_AddLimitToBank(CAN_Msg);
        break;

//     case  CAN_CMD_DOWNLOAD_SDL:
//         Aux_DownloadSdlPara(CAN_Msg);
//         break;
    case CAN_CMD_DOWNLOAD_SAFETY:
        Aux_AddSafetyCfg(CAN_Msg);
        break;
    case CAN_CMD_CLEAR_LIMITS:
        Aux_ClearIVsLimit(CAN_Msg);
        break;
    case CAN_CMD_CLEAR_SAFETY:
        Aux_ClearSafetyCfg(CAN_Msg);
        break;
    case CAN_CMD_REQUEST_DATA:
        Aux_RequestDataProc(CAN_Msg);
        break;
    case CAN_CMD_CLEAR_REQUEST_DATA:
        Aux_ClearRequestData(CAN_Msg);
        break;
//     case CAN_CMD_TRIGGER_LOG:
//         Aux_TrigLogProc(CAN_Msg);
//         break;
    case CAN_CMD_TRIGGER_GROUPLOG:
        Aux_TrigGroupLogProc(CAN_Msg);
        break;
    case CAN_CMD_TRIGGER_GROUPLOG_LAST:
        Aux_TrigGroupLogProcLast(CAN_Msg);
        break;
//     case CAN_CMD_ENABLE_SDL:
//         Aux_SdlEnable(CAN_Msg);
//         break;
    case CAN_CMD_SET_CTRL:
        Aux_RxSetCtrl(CAN_Msg);
        break;
    case CAN_CMD_CTRL_RUNON:         //控制温控箱开关
        Aux_RxCtrlRunON(CAN_Msg);
        break;
    case CAN_CMD_MTC_SAFETY_CHECK:
        Aux_RxSetMTCSafetyCheck(CAN_Msg);		//收到温度保护时间，设置MTC温度保护  //add by zc for MTC 07.17.2018
        break;
    case CAN_CMD_MTC_SAFETY_STOP:  //停止MTC UNSAFETY CHECK  //add by zc for MTC 08.20.2018
        Aux_RxClearMTCSafetyCheck(CAN_Msg);
        break;
    case CAN_CMD_AIO_CTRL:
        Aux_RxAOCtrl(CAN_Msg);
        break;
    case CAN_CMD_DOWNLOAD_SCHMAP:
        Aux_DownloadScheduleMap(CAN_Msg);
        break;
// 	case CAN_CMD_TIME_SYNC:   //此命令为广播包 CANID == FilterBrdCstID
// 		Aux_Sync_Time(CAN_Msg->Un_Content.m_SyncTime.m_MsTime,CAN_Msg->Un_Content.m_SyncTime.m_CaliStatus);
// 		break;
// 	case CAN_CMD_ASK_STATUS:
// #if (USE_AUX_STATUS_REPORT == 1)
// 		Aux_UpdateStatus(CAN_Msg);
// #endif
// 		break;
// 	case CAN_CMD_REQUEST_TIME:  //辅助接收到请求时间命令直接退出
// 		break;
//#elif (SUPPORT_AUX ==1 )
#else
    //------------------IV MCU接收命令----------------//
    case CAN_CMD_REQUEST_DATA_ACK:
        IV_RxLimitData(CAN_Msg);
        break;
    case CAN_CMD_STATUS_RPT:
#if (USE_AUX_STATUS_REPORT == 1)
        IV_ProcessAuxStatusInfo(CAN_Msg);
#endif
        break;
    case CAN_CMD_REPORT_UNSAFE:
        IV_AuxUnsafeRpt(CAN_Msg);
        break;
    case CAN_CMD_REPORT_MTC_TEMP_UNSAFE: // add by zc 08.08.2018 for MTC
    {
        IV_AuxMTCTempUnsafeRpt(CAN_Msg);
        break;
    }
    case CAN_CMD_REPORT_MTC_FAN_UNSAFE: // add by zc 08.31.2018 for MTC
    {
        IV_AuxMTCFanUnsafeRpt(CAN_Msg);
        break;
    }
    case CAN_CMD_REPORT_LIMIT_STATUS:
        IV_AuxLimitRpt(CAN_Msg);
        break;
// 	case CAN_CMD_REQUEST_TIME://IV接收到请求时间命令
// 		IV_TimeSyncToAux(&OS_ServeProcess::OS_Time,1,CAN_PORT_USE_PUBLIC); //发送对时命令
// 		break;
    case CAN_CMD_REPORT_MV:
        IV_RxMetaVariable(CAN_Msg);
        break;
// 	case CAN_CMD_TIME_SYNC:
// 		break;
#endif
    default:
        break;
    }
}

//#endif
/************************************************************************/
/*               parall CMD Parse                                       */
/************************************************************************/
//#if(SUPPORT_PARALLEL == 1)
//---------------------------------- PARALLEL -----------------------------------

#if USE_AS_AUXMCU == 0      //只有IV unit 需要处理并联信息

void CANBusParse::Parallel_CmdParse(MP_CAN_MSG * CAN_Msg)
{
    unsigned char errCode = 0;

    switch(CAN_Msg->Un_CANID.m_CANID.Cmd)
    {
    case	CAN_WATCHDOG:
// 		if(CAN_Msg->Un_Content.m_WatchdogACK.AckType==WATCHDOG_MASTERSEND)
// 		   CAN_WatchdogTask:: SubMasterReceiveSN(CAN_Msg);
// 		if(CAN_Msg->Un_Content.m_WatchdogACK.AckType==WATCHDOG_SUBSEND)
// 			 CAN_WatchdogTask:: MasterReceiveACK(CAN_Msg);
        CAN_WatchdogTask:: MasterOrSuMasterReceiveACK(CAN_Msg);
        break;
    //#if   SUPPORT_PARALLEL == 1
    case CAN_CMD_PARA_ACK:
        Parallel_AckProcess(CAN_Msg);
        //CANMsgBuffering::ConfirmMsg(CAN1,CAN_Msg->Un_Content.m_Parallel_Ack.SN );   //清除事件缓存EventBuffering 中的活跃区标志 告知其不要存入非活跃区

        break;
    case CAN_CMD_PARA_START:  //master --> submaster: start

        if((ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID + 1) == CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1  \
                != CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(Parallel_SubMasterConfirm(CAN_Msg))
        {
            errCode = Parse_Parallel_Master_RequestStart(CAN_Msg);
            if(board_config::Feature_Enable_init.Power_Off_Enable==1)
                PowerLostCheck::ParallelPowerCheckFlag=1;
        }
        //判断是否已接收过（SN)
        Parallel_ConfirmAck(CAN_Msg, errCode);

        break;

    case CAN_CMD_PARA_MASTER_STOP:  //master --> submaster: stop

        if((ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID + 1) == CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1  \
                != CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(Parallel_SubMasterConfirm(CAN_Msg))
        {
            Parse_Parallel_Master_RequestStop(CAN_Msg) ;
            if(board_config::Feature_Enable_init.Power_Off_Enable==1)
                PowerLostCheck::ParallelPowerCheckFlag=0;
        }

        Parallel_ConfirmAck(CAN_Msg, 0);

        break;
    case 	CAN_CMD_PARA_SETCLAMP:
        Parse_Parallel_Master_SetVolClamp(CAN_Msg);
        break;
    case CAN_CMD_PARA_SUB_RUN:
        if(ChannelInfo_Manager::ChannelInfo[0].m_CtrlState >= CCS_I && ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank > 2)
            break;
        Parse_Parallel_Sub_RequestRun(CAN_Msg);
        break;
    case CAN_CMD_PARA_SUB_STOP:
        Parse_Parallel_Sub_RequestStop(CAN_Msg);
        break;
    case CAN_CMD_PARA_SERIES2BOOSTER_REPORT_VOLTAGE:
        Parse_Parallel_Master_Request_Series2Booster_OutVoltage(CAN_Msg);
        break;
//	case 	CAN_CMD_PARA_SET_PSIMUCLAMP:   //09.25.2017 add by zc
//		Parse_Parallel_Master_SetPowerSimulationVolClamp(CAN_Msg); //09.25.2017 add by zc
//		break;
    case CAN_CMD_PARA_MASTER_STEPCHANGE:  //master --> submaster: jump step

        if((ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID + 1) == CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
// 		if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx+1  \
// 			!=CAN_Msg->Un_CANID.m_CANID.SourceID)
// 			return;
        if(Parallel_SubMasterConfirm(CAN_Msg) && ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank < 3)
            Parse_Parallel_Master_RequestStepChange(CAN_Msg) ;

        Parallel_ConfirmAck(CAN_Msg, 0);

        break;

    case CAN_CMD_PARA_MASTER_UPDATEOUTPUT:  //master --> submaster: update control value

        if((ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID + 1) == CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1  \
                != CAN_Msg->Un_CANID.m_CANID.SourceID)
            return;
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode != eMixMode) //190122 yasongniu
        {
            Parse_Parallel_Master_RequestUpdateOutput(CAN_Msg);
        }
        else
        {
            for(unsigned char uc_i = 0; uc_i < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount - 1; uc_i++)
            {
                if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit == eSeriesBoosterRank_SubMaster
                        && ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx == ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID) //.m_UnitGroupInfo.m_Supervisor.m_beSlaveUnit==false) ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID
                    Parse_Parallel_Master_RequestUpdateOutput(CAN_Msg) ;
                ////03.21.2018 CAN传输电压=================
                if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit==eSeriesBoosterRank_Serial//3
                        ||	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit==eSeriesBoosterRank_Booster)//4
                {
                    PWM_SampleManage::m_InsideCtrlVoltageValue=CAN_Msg->Un_Content.m_ParallelUpdateOutput.m_fSeries2BoosterVoltage;
                    IndependentVoltageCtrl_task::Series2BoosterSendVoltageEnable = false;
//                     CommonFunc::GetTimeTicksUpdate(&Step_Manager_task::Series2BoosterWatchDogTime[0]);
                }

                //=========================================
            }
        }
//         if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT22013)
//         {
//             CommonFunc::GetTimeTicksUpdate(&CAN_WatchdogTask::SubUnitOutWatchDogTime[0]);
//         }
        break;

// 	case CAN_CMD_PARA_REQUESTDATA:    //master --> submaster:  request data from submaster
// 		//TODO
// 		break;

    case CAN_CMD_PARA_SUB_REPORTDATA:   //submaster --> master: report data (SDL, resolution current&voltage pair)
        Parse_Parallel_Submaster_ReportData(CAN_Msg) ;
        break;

    case CAN_CMD_PARA_SUB_UNSAFE:   //submaster --> master: report unsafe
        Parse_Parallel_Submaster_ReportUnsafe(CAN_Msg) ;
        break;

    // 		case CAN_CMD_PARA_SUB_CURR_ABN:   //submaster --> master: report current abnormal
    // 			Parse_Parallel_Submaster_ReportCurrentAbnormal(CAN_Msg) ;
    // 			break;

    // 		case CAN_CMD_PARA_SUB_VOLT_ABN:   //submaster --> master: report voltage has big different in sub-unit's parallelled channels
    // 			//TODO: only for multi-channel, inter-unit parallel. Will support this setting.
    // 		   Parse_Parallel_Submaster_ReportVoltageAbnormal(CAN_Msg);
    // 			break;

    case CAN_CMD_PARA_SUB_IRREGULAR:   //submaster --> master: report irregulation and submaster alreayd setunsafe
        Parse_Parallel_Submaster_ReportIrregulation(CAN_Msg) ;
        Parallel_ConfirmAck(CAN_Msg, 0);
        break;

//     case CAN_CMD_PARA_SUB_CTRLERROR:   //submaster --> master: report irregulation and submaster alreayd setunsafe
//         Parse_Parallel_Submaster_ReportCtrlError(CAN_Msg) ;
//         Parallel_ConfirmAck(CAN_Msg, 0);
//         break;

    //#endif
    case CAN_CMD_PARA_MASTER_SET_FILTER:
        Parse_Parallel_Master_SetFilter(CAN_Msg);
        break;
    default:
        break;
    }

}
#endif


/******************************************************************************
** Function name:		Send_CAN_Cmd
**
** Descriptions:    CAN命令的发送（IV和辅助公用）
**
** parameters:		MP_CAN_MSG
**
** Returned value:	SN号（部分需要调用者填入，IV部分主动生成）
**
** Created Date:      2013/11/28
**
** Revision Dates:

******************************************************************************/
unsigned short CANBusParse::Send_CAN_Cmd(MP_CAN_MSG* CAN_msg)
{
    MP_CAN_MSG * Temp_CAN_Msg = CAN_msg;  //需添加BoardID和SN
//     unsigned char m_CANPort;
    unsigned short SN;
    //CAN类型转换
//     if(CAN_Type == CAN_PORT_USE_EXCLUSIVE)
//         m_CANPort = CAN2; //暂时
//     else if(CAN_Type == CAN_PORT_USE_PUBLIC)
//         m_CANPort = CAN1;
//     else
//         return 0;//add
    Temp_CAN_Msg->Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;//load source ID
    Temp_CAN_Msg->FrameType = CAN_EXT_FRAME;// extend frame;
    Temp_CAN_Msg->DLC = 8;    //8 byte;
    switch(Temp_CAN_Msg->Un_CANID.m_CANID.CmdType)
    {
    case AUX_CMD_TYPE:
//#if(SUPPORT_AUX==1)
        if(CommonFunc::uc_Support_AUX)
            SN = Send_Aux_Cmd(Temp_CAN_Msg);
//#endif
        break;
    case SYSTEMCTRL_SET:
#if(USE_AS_AUXMCU == 0)
        Send_SYSCtrl_Cmd(Temp_CAN_Msg);
#endif
        break;
    case Parallel_CMD_TYPE:
#if(USE_AS_AUXMCU == 0)
        SN = Send_Para_Cmd(Temp_CAN_Msg);
#endif
        break;
//     case BMS_CMD_TYPE:
// 		break;
    case DSP_PARA_SET:
        //if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_PWM)
        CANFunc::CAN_Push_to_Send(CAN1,CAN_msg);
        break;
    case Power_M0_ADJ:
        CANFunc::CAN_Push_to_Send(CAN1,Temp_CAN_Msg);
        break;
    default:
        break;

    }
    return SN;
}
MP_MCU_MSG CANBusParse::CanMsg2McuMsg(const MP_CAN_MSG CAN_msg)
{
    MP_MCU_MSG mcuMsg;
    memset(&mcuMsg, 0, sizeof(MP_MCU_MSG));
    if(CAN_msg.Un_CANID.m_CANID.CmdType == BMS_CMD_TYPE && CAN_msg.Un_CANID.m_CANID.Cmd == CAN_CMD_BMS_MSG)
        mcuMsg.ByteLength = sizeof(ST_CANMsg);
    else if((CAN_msg.Un_CANID.m_CANID.CmdType == AUX_CMD_TYPE) && (CAN_msg.Un_CANID.m_CANID.Cmd == CAN_CMD_REQUEST_DATA_ACK))
        mcuMsg.ByteLength = sizeof(ST_Request_Data_Ack);		
    else
        mcuMsg.ByteLength = 8;//CAN_msg.DLC;
    mcuMsg.Un_CANID.m_U32ID = CAN_msg.Un_CANID.m_U32ID;
    memcpy(&mcuMsg.Un_Content,&CAN_msg.Un_Content, mcuMsg.ByteLength);
    mcuMsg.ByteLength += 4+sizeof(UN_CANID);
    return mcuMsg;
}
//#if(SUPPORT_AUX==1)
unsigned short CANBusParse::Send_Aux_Cmd(MP_CAN_MSG* CAN_msg)
{
    unsigned short ReturnSN = 0;
    unsigned char Cmd = CAN_msg->Un_CANID.m_CANID.Cmd;
#if(USE_AS_AUXMCU == 0)//IV  
    if(Cmd == CAN_CMD_DOWNLOAD_LIMITS )
    {
        if( st_CAN_SN.m_LimitSN < 0x3FF ) //防止临界(anylimit)
        {
            if(!CAN_msg->Un_Content.m_DownLimit.All)
                st_CAN_SN.m_LimitSN++;
        }
        else
            st_CAN_SN.m_LimitSN = 0x201;
    }
    else
    {
        if((st_CAN_SN.m_MsgSN ) < 0x200)
            st_CAN_SN.m_MsgSN++;
        else
            st_CAN_SN.m_MsgSN = 1;
    }
#endif

    switch(Cmd)
    {
    case CAN_WATCHDOG:
        break;
    case CAN_CMD_CONFIRM_ACK:                    //IV Aux公用
        Aux_Confirm_Ack(CAN_msg, NO_ERROR);
        ReturnSN = CAN_msg->Un_Content.m_ConfirmAck.SN;  // SN号为收到命令的SN 调用者填充
        break;
        //--------------------------------------IV用------SN号自加-------------------------
#if(USE_AS_AUXMCU == 0)
    case CAN_CMD_DOWNLOAD_LIMITS:    // DOWNLOAD LIMIT命令 ( 0x800)>2048
        if(CAN_msg->Un_Content.m_DownLimit.All)
            CAN_msg->Un_Content.m_DownLimit.SN = st_CAN_SN.m_AnyLimitSN;
        else
            CAN_msg->Un_Content.m_DownLimit.SN =  st_CAN_SN.m_LimitSN ;//SN号作用
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_DownLimit.SN;
        break;
    case CAN_CMD_DOWNLOAD_SDL:
    case CAN_CMD_DOWNLOAD_SAFETY:
    case CAN_CMD_CLEAR_LIMITS:
    case CAN_CMD_CLEAR_SAFETY:
//     case CAN_CMD_ENABLE_SDL:
    case CAN_CMD_DOWNLOAD_SCHMAP:
    case CAN_CMD_SET_CTRL:
    case CAN_CMD_CTRL_RUNON:
    case CAN_CMD_AIO_CTRL:
        CAN_msg->Un_Content.m_AddSafety.SN = st_CAN_SN.m_MsgSN & 0x1FF;  //	其他命令<1024
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_AddSafety.SN;
        break;
    case CAN_CMD_MTC_SAFETY_CHECK:		//08.06.2018 add by zc for MTC
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_SetCtrl.SN;
        break;
    case CAN_CMD_MTC_SAFETY_STOP:		//08.20.2018 add by zc for MTC
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_SetCtrl.SN;
        break;
    case CAN_CMD_REQUEST_DATA:
        CAN_msg->Un_Content.m_ReqstData.SN = st_CAN_SN.m_MsgSN & 0x1FF;  //	其他命令<1024
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_ReqstData.SN;
        break;
//     case CAN_CMD_TRIGGER_LOG:              //不需要ACK和被确认
    case CAN_CMD_TRIGGER_GROUPLOG:
    case CAN_CMD_TRIGGER_GROUPLOG_LAST:
//         CAN_msg->Un_CANID.m_CANID.Priority = CAN_PRIO_LOW;  //低优先级
// 	  if(CheckCAN_Port_Property(m_CANPort) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
        //CANFunc::CAN_Push_to_Send(m_CANPort,CAN_msg);
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN =  0;//此结构不包含SN //ReturnSN = Temp_CAN_Msg->Un_Content.m_AddSafety.SN;
        break;
#else
    //-----------------------------------------AUX用----------------------------------//
    case CAN_CMD_STATUS_RPT:
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        break;
    case CAN_CMD_REPORT_UNSAFE:             //调用者填入SN（或不在乎）
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        break;
    case CAN_CMD_REPORT_MTC_TEMP_UNSAFE:             //调用者填入SN（或不在乎）// add by zc 08.08.2018 for MTC
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        break;
    case CAN_CMD_REPORT_MTC_FAN_UNSAFE:             //调用者填入SN（或不在乎）// add by zc 08.31.2018 for MTC
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        break;
    case CAN_CMD_REQUEST_DATA_ACK:         // 不需要ACK   调用者填入SN
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_DataAck.SN;
        break;
    case CAN_CMD_REPORT_LIMIT_STATUS:        //SN为下载时获得的SN  需要存储，然后读出
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_LimitRpt.SN;
        break;
    case CAN_CMD_REPORT_MV:      // 不需要ACK   调用者填入SN
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        ReturnSN = CAN_msg->Un_Content.m_MetaVariable.SN;
        break;
    default:
        break;
#endif
    }
    return ReturnSN;
}


//确认接收 confirm_ack 接收到报文后 调用此函数做ack
unsigned char  CANBusParse::Aux_Confirm_Ack(const MP_CAN_MSG * RecvMsg, unsigned char uc_errorcode)
{
    MP_CAN_MSG CAN_ConfirmMsg;
//     CAN_ConfirmMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_LOW;

    if(RecvMsg->Un_CANID.m_CANID.Cmd == CAN_CMD_DOWNLOAD_LIMITS)
    {
        CAN_ConfirmMsg.Un_Content.m_ConfirmAck.StepID = StepID[RecvMsg->Un_Content.m_DownLimit.IV_ChnNo];
//         CAN_ConfirmMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
        if(RecvMsg->Un_Content.m_DownLimit.All == 0)
            CAN_ConfirmMsg.Un_Content.m_ConfirmAck.IfAnyLimit = 0;
        else
            CAN_ConfirmMsg.Un_Content.m_ConfirmAck.IfAnyLimit = 1;
    }
    else if(RecvMsg->Un_CANID.m_CANID.Cmd == CAN_CMD_CLEAR_LIMITS)
    {
//         CAN_ConfirmMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
        CAN_ConfirmMsg.Un_Content.m_ConfirmAck.StepID = StepID[RecvMsg->Un_Content.m_DownLimit.IV_ChnNo];
    }

    CAN_ConfirmMsg.Un_CANID.m_CANID.DestID = RecvMsg->Un_CANID.m_CANID.SourceID;  // ID 交换
    CAN_ConfirmMsg.Un_CANID.m_CANID.SourceID = RecvMsg->Un_CANID.m_CANID.DestID;  //这里需添加广播包

    CAN_ConfirmMsg.Un_Content.m_DownLimit.AuxChnNo = RecvMsg->Un_Content.m_DownLimit.AuxChnNo;
    CAN_ConfirmMsg.Un_Content.m_DownLimit.AuxChnType = RecvMsg->Un_Content.m_DownLimit.AuxChnType;
    CAN_ConfirmMsg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    CAN_ConfirmMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_CONFIRM_ACK;
    CAN_ConfirmMsg.Un_Content.m_ConfirmAck.RecvCmd = RecvMsg->Un_CANID.m_CANID.Cmd;
    CAN_ConfirmMsg.Un_Content.m_ConfirmAck.CmdStatus = 1; //ok
    CAN_ConfirmMsg.Un_Content.m_ConfirmAck.IV_ChnNo = RecvMsg->Un_Content.m_DownLimit.IV_ChnNo;
    CAN_ConfirmMsg.Un_Content.m_ConfirmAck.SN =  RecvMsg->Un_Content.m_DownLimit.SN;   //由于SN位置一致
//     CAN_ConfirmMsg.DLC = 8;
//     CAN_ConfirmMsg.FrameType =  CAN_EXT_FRAME;
// 	#if(USE_AS_AUXMCU == 0)
// 	if(CheckCAN_Port_Property(CAN1) == USE_AS_NORMAL_CAN)
// 	#endif
    //CANFunc::CAN_Push_to_Send(CAN1,&CAN_ConfirmMsg);
    MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(CAN_ConfirmMsg));
    //这里要添加时间标记 ?
    return 0;
}
//#endif
//#if(SUPPORT_AUX == 1 )
// void CANBusParse::AddToConfirmBuf(unsigned char CANx,const MP_CAN_MSG * can_msg)
// {
// //	unsigned short CAN_ConfirmNo =  can_msg->Un_Content.m_ConfirmAck.SN;
// //	CANMsgBuffering::Push2ConfirmBuf(CANx,(uint8 *)&can_msg->FrameType,sizeof(MP_CAN_MSG),CAN_ConfirmNo);
// }
//#endif

/************************************************************************/
/**              IV MCU Send Parallel Command Part                     **/
/************************************************************************/
//------------------- INTER UNIT PARALLEL GROUP CAN PARSE -----------------------
//#if(SUPPORT_PARALLEL == 1)

//Check Parallel Ack Status/result -->report error

#if USE_AS_AUXMCU == 0
void CANBusParse::Parallel_DoMasterAckCheck()
{

    for(unsigned char cmdIdx = 0; cmdIdx < CAN_CMD_PARA_MAX; cmdIdx++)
    {
        //no ACK need to send to master for updateoutput.
        //But submaster should receive ACK from master for unsafe/irregulation/ctrlerror report
        if(cmdIdx > CAN_CMD_PARA_MASTER_STEPCHANGE)
            continue;
        for(unsigned char BufIdx = 0; BufIdx < PARA_MAX_BUF; BufIdx++)
        {
            if(m_Para_MasterAckMgr[cmdIdx][BufIdx].CmdStatus != eParaAckWait)
                continue;
            if(CommonFunc::GetTimeTicks(&m_Para_MasterAckMgr[cmdIdx][BufIdx].m_OverTime) > PARA_MAX_OVERTIME)
            {   //resend MSG
                if(m_Para_MasterAckMgr[cmdIdx][BufIdx].m_ReSendCnt < 5 * UDP_PARALLEL_WATCHDOGRATE) // 200*20*5*3=6s  UDP_Parallel_WatchdogRate
                {
                    m_Para_MasterAckMgr[cmdIdx][BufIdx].m_ReSendCnt++;

                    MP_CAN_MSG ResendMsg = m_Para_MasterAckMgr[cmdIdx][BufIdx].m_Msg;
                    st_CAN_SN.m_bResendPara = 1;			//is resend msg
                    ResendMsg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
                    ResendMsg.Un_Content.m_ParallelStart.SN = m_Para_MasterAckMgr[cmdIdx][BufIdx].SN;   //resend sn
                    Send_CAN_Cmd(&ResendMsg);
                    st_CAN_SN.m_bResendPara = 0;    //resume normal send sn increase
                    CommonFunc::GetTimeTicksUpdate(&m_Para_MasterAckMgr[cmdIdx][BufIdx].m_OverTime);
                }
                else
                {   //失败应该报error
//                     unsigned char uMasterChannel;
//                     uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID;
                    unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;

                    if((ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType==IV_UNSAFE_TYPE)&&(ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus == CAN_PARA_ACK_UNSAFE))
                        return;
                    St_ScheduleMessage SchMessage;
                    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
                    SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
                    SchMessage.st_status_sch_running.m_bUnsafeType = IV_UNSAFE_TYPE;
                    SchMessage.st_status_sch_running.m_uw32Safe_Code = CAN_PARA_ACK_UNSAFE;
                    for(unsigned char uc_i = 0; uc_i < m_Para_MasterAckMgr[cmdIdx][BufIdx].m_SubUnitCnt; uc_i++)
                    {
                        if(m_Para_MasterAckMgr[cmdIdx][BufIdx].m_SubStatus[uc_i] == eParaAckWait)
                        {
                            SchMessage.st_status_sch_running.m_btLost_Unit = m_Para_MasterAckMgr[cmdIdx][BufIdx].m_SubID[uc_i];
                            break;
                        }
                    }
                    SchMessage.st_status_sch_running.m_btSubCode = eCANBUSWATCHDOG_ERROR_DoMasterAckCheck;
                    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
                    ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType = IV_UNSAFE_TYPE;
                    ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus = CAN_PARA_ACK_UNSAFE;
                    safety_check::SetUnsafe(uMasterChannel,  CAN_PARA_ACK_UNSAFE, 0,IV_UNSAFE_TYPE);
                    Schedule_Manager_task::UnsafeContent[uMasterChannel] = SchMessage;
                    Schedule_Manager_task::SendMsgToSchedule(uMasterChannel, SchMessage); //停止对应的IV通道
                    m_Para_MasterAckMgr[cmdIdx][BufIdx].m_ReSendCnt = 0;
                    m_Para_MasterAckMgr[cmdIdx][BufIdx].CmdStatus = eParaAckNone;
                    memset((void*)&m_Para_MasterAckMgr[cmdIdx][BufIdx].m_SubStatus [0], eParaAckNone, MAXUNITNO);
                }
            }
            else
                Parallel_CheckAck(cmdIdx, BufIdx); //未超时，则做检查
        }	//end for BufIdx
    }//end for cmdIdx

}


//
//void CANBusParse::Parallel_ResetSubAckInfo()
//{
//	for(unsigned char cmdIdx = 0;cmdIdx<CAN_CMD_PARA_MAX;cmdIdx++)
//	{
//		if(cmdIdx>= CAN_CMD_PARA_MASTER_STEPCHANGE)  //CAN_CMD_PARA_START,CAN_CMD_PARA_MASTER_STOP, CAN_CMD_PARA_MASTER_STEPCHANGE only
//			continue;
//		for(unsigned char BufIdx = 0;BufIdx <PARA_MAX_BUF;BufIdx++)
//		{
//			CANBusParse::m_Para_MasterAckMgr[cmdIdx][BufIdx].m_ReSendCnt = 0;
//			CANBusParse::m_Para_MasterAckMgr[cmdIdx][BufIdx].CmdStatus = eParaAckNone;
//			CANBusParse::m_Para_MasterAckMgr[cmdIdx][BufIdx].SN = 0;
//			m_SubMasterAckMgr[cmdIdx].m_RxRes[BufIdx].bValid = 0;
//			memset((void*)&CANBusParse::m_Para_MasterAckMgr[cmdIdx][BufIdx].m_SubStatus [0],eParaAckNone,MAXUNITNO);//reset
//		}
//	}
//}

//ACK 处理函数 <Master>
void CANBusParse::Parallel_AckProcess(MP_CAN_MSG* CAN_msg)
{

    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    uint32 unsafecode;
    unsigned char OrgCmd = CAN_msg->Un_Content.m_Parallel_Ack.RecvCmd;
    unsigned char SubIdx = 0;
    switch(OrgCmd)  //收集全部的ack
    {
    case CAN_CMD_PARA_START:
        if( CAN_msg->Un_Content.m_Parallel_Ack.errCode != eParaSubOk)
        {
            if(CAN_msg->Un_Content.m_Parallel_Ack.errCode == eParaSubCurUnsafe)
                unsafecode = PARA_SUB_CUR_UNSAFE;
            else if(CAN_msg->Un_Content.m_Parallel_Ack.errCode == eParaSubVolUnsafe)
                unsafecode = PARA_SUB_VOL_UNSAFE;
            else if (CAN_msg->Un_Content.m_Parallel_Ack.errCode==eParaSubIrregularUnsafe)
                unsafecode=IRREGULAR_UNSAFE;
            else if (CAN_msg->Un_Content.m_Parallel_Ack.errCode==eParaSubFan_FaultUnsafe)
                unsafecode=PARA_FAN_FAULT_UNSAFE;
            else if (CAN_msg->Un_Content.m_Parallel_Ack.errCode==eParaSubPowerOffUnsafe)
                unsafecode=PARA_POWER_OFF_UNSAFE;
            else if(CAN_msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubUnknowUnsafe)
                return;  //temp
            SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
            unsigned char uMasterChannel;
//             uMasterChannel=ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID;
			uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
            ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType = IV_UNSAFE_TYPE;
            ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus = unsafecode;
            safety_check::SetUnsafe(uMasterChannel, unsafecode, 0,IV_UNSAFE_TYPE);
            SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
            SchMessage.st_status_sch_running.m_bUnsafeType = IV_UNSAFE_TYPE;
            SchMessage.st_status_sch_running.m_uw32Safe_Code = unsafecode;
            SchMessage.st_status_sch_running.m_btLost_Unit = CAN_msg->Un_CANID.m_CANID.SourceID;
            Schedule_Manager_task::UnsafeContent[uMasterChannel] = SchMessage;
            Schedule_Manager_task::SendMsgToSchedule(uMasterChannel, SchMessage); //停止aux Unit对应的所有IV通道
        }
    case CAN_CMD_PARA_MASTER_STOP:
    case CAN_CMD_PARA_MASTER_STEPCHANGE:
        //case CAN_CMD_PARA_REQUESTDATA:
        for(unsigned char BufIdx = 0; BufIdx < PARA_MAX_BUF; BufIdx++)
        {
            if(m_Para_MasterAckMgr[OrgCmd][BufIdx].CmdStatus != eParaAckWait)
                continue;
            SubIdx = 0;
            unsigned char MaxUnitCnt = m_Para_MasterAckMgr[OrgCmd][BufIdx].m_SubUnitCnt;
            while(MaxUnitCnt--)
            {
                if(m_Para_MasterAckMgr[OrgCmd][BufIdx].SN == CAN_msg->Un_Content.m_Parallel_Ack.SN)
                {
                    if(m_Para_MasterAckMgr[OrgCmd][BufIdx].m_SubID[SubIdx] == CAN_msg->Un_CANID.m_CANID.SourceID)
                    {
                        m_Para_MasterAckMgr[OrgCmd][BufIdx].m_SubStatus[SubIdx] = eParaAckOK;
                        m_SubACKUnitFlg[CAN_msg->Un_CANID.m_CANID.SourceID] = true;
                    }
                }
                SubIdx++;
            }
        }
        break;
    default:
        break;
    }

}


//ack check 函数 <Master>
//return bool (true/false)
bool CANBusParse::Parallel_CheckAck(unsigned char Cmd, unsigned char BufIdx)
{
    //aux clear limit状态检查

    unsigned char bAllReceive = 1;

    if(m_Para_MasterAckMgr[Cmd][BufIdx].CmdStatus == eParaAckOK || m_Para_MasterAckMgr[Cmd][BufIdx].CmdStatus == eParaAckNone)
        return true;

    for(unsigned char subIdx = 0; subIdx < m_Para_MasterAckMgr[Cmd][BufIdx].m_SubUnitCnt; subIdx++)
    {
        if(m_Para_MasterAckMgr[Cmd][BufIdx].m_SubStatus[subIdx] == eParaAckWait) //false
            bAllReceive = 0;
    }

    if(bAllReceive)
    {
        m_Para_MasterAckMgr[Cmd][BufIdx].CmdStatus = eParaAckOK;
        return true;
    }
    else
        return false;
}

//false already received or cmd error
//true  a new cmd whitout same SN
bool CANBusParse::Parallel_SubMasterConfirm(const MP_CAN_MSG * RecvMsg)
{
    unsigned char Cmd = RecvMsg->Un_CANID.m_CANID.Cmd;
    unsigned short SN = RecvMsg->Un_Content.m_ParallelStart.SN; //181115 zyx

    unsigned char CmdIdx = 0;
    if(Cmd == CAN_CMD_PARA_START)  //1
        CmdIdx = 0;
    else if(Cmd == CAN_CMD_PARA_MASTER_STOP) //2
        CmdIdx = 1;
    else if(Cmd == CAN_CMD_PARA_MASTER_STEPCHANGE) //3
        CmdIdx = 2;
    else
        return false;
    //unsigned char StartUnitIdx = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx+1;
    //unsigned char EndUnitIdx = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx+ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount-1;
    //if( (CANFunc::SourceBoardID>EndUnitIdx)&&(CANFunc::SourceBoardID<StartUnitIdx) )
    //	return false;
    if(!beSubMasterAckUnitID(RecvMsg->Un_CANID.m_CANID.SourceID))
        return false;
    for(unsigned char i = 0; i < m_SubMasterAckMgr[CmdIdx].m_CurrentIdx; i++)
    {
        if ((m_SubMasterAckMgr[CmdIdx].m_RxRes[i].SN == SN) && m_SubMasterAckMgr[CmdIdx].m_RxRes[i].bValid)
        {
            return false;
        }
    }

    if(Cmd == CAN_CMD_PARA_START)  //reset submaster's info
    {
        for(unsigned char cmdIdx = 0; cmdIdx < CAN_CMD_PARA_MAX; cmdIdx++)
        {
            if(cmdIdx >= CAN_CMD_PARA_MASTER_STEPCHANGE) //CAN_CMD_PARA_START,CAN_CMD_PARA_MASTER_STOP, CAN_CMD_PARA_MASTER_STEPCHANGE only
            {
                continue;
            }

            for(unsigned char idx = 0; idx <SUBMASTER_SN_RES_COUNT; idx++)
            {
                m_SubMasterAckMgr[cmdIdx].m_RxRes[idx].SN = 0;
                m_SubMasterAckMgr[cmdIdx].m_RxRes[idx].bValid = 0;
            }
            m_SubMasterAckMgr[CmdIdx].m_CurrentIdx = 0;
        }
    }

    m_SubMasterAckMgr[CmdIdx].m_RxRes[m_SubMasterAckMgr[CmdIdx].m_CurrentIdx].SN = SN;
    m_SubMasterAckMgr[CmdIdx].m_RxRes[m_SubMasterAckMgr[CmdIdx].m_CurrentIdx].bValid = 1;

    m_SubMasterAckMgr[CmdIdx].m_CurrentIdx = (m_SubMasterAckMgr[CmdIdx].m_CurrentIdx+1)%SUBMASTER_SN_RES_COUNT;

    return true;
}

//发送并联命令
unsigned short CANBusParse::Send_Para_Cmd(MP_CAN_MSG* CAN_msg)
{
    unsigned short SN;
    unsigned char Cmd = CAN_msg->Un_CANID.m_CANID.Cmd;

    if(st_CAN_SN.m_bResendPara)
    {
        SN = CAN_msg->Un_Content.m_ParallelStart.SN;
    }
    else
    {
        if((Cmd != CAN_CMD_PARA_MASTER_UPDATEOUTPUT) && (Cmd != CAN_WATCHDOG) && (Cmd != CAN_CMD_PARA_SUB_REPORTDATA))
        {
            if((st_CAN_SN.ParaSN ) < 0x7FF)
                st_CAN_SN.ParaSN++;
            else
                st_CAN_SN.ParaSN = 1;
            SN = st_CAN_SN.ParaSN;
        }
        else
            SN = 0;
    }
    switch(Cmd)
    {
    case CAN_CMD_PARA_START :	//1
    case CAN_CMD_PARA_MASTER_STOP:  //2
    case CAN_CMD_PARA_MASTER_STEPCHANGE: //3
    case CAN_CMD_PARA_SUB_RUN:
    case CAN_CMD_PARA_SUB_STOP:
    case CAN_CMD_PARA_MASTER_SET_FILTER:
        CAN_msg->Un_Content.m_ParallelStart.SN = SN; //09.24
    case CAN_CMD_PARA_MASTER_UPDATEOUTPUT:	//4
    case CAN_CMD_PARA_SETCLAMP:
    case CAN_CMD_PARA_SERIES2BOOSTER_REPORT_VOLTAGE:
    case CAN_WATCHDOG:
    //------------- sub --------------//
    case CAN_CMD_PARA_SUB_REPORTDATA:
    case CAN_CMD_PARA_SUB_UNSAFE:
    case CAN_CMD_PARA_SUB_IRREGULAR:
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(*CAN_msg));
        break;

    default:
        break;
    }
    return SN;

}

void CANBusParse::Send_SYSCtrl_Cmd(MP_CAN_MSG* CAN_msg)
{
    unsigned char Cmd = CAN_msg->Un_CANID.m_CANID.Cmd;

    switch(Cmd)
    {
    case CAN_CMD_SYSTEM_ABNORMAL:
    case CAN_CMD_SYSTEM_BACKTONORMAL:
    case CAN_CMD_SYSTEM_RUN:
    case CAN_CMD_SYSTEM_STOP:
// 		 CAN_msg->Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
// 	   if(CheckCAN_Port_Property(m_CANPort) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
        //CANFunc::CAN_Push_to_Send(m_CANPort,CAN_msg);
        MultiMcuComm::NET_Push_to_BroadCastSend(CanMsg2McuMsg(*CAN_msg));

        break;
    default:
        break;
    }

}
bool CANBusParse::Parallel_ConfirmAck( const MP_CAN_MSG * RecvMsg, unsigned char uc_errorcode)
{
    MP_CAN_MSG  Para_Ack;
    Para_Ack.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_ACK;
    Para_Ack.Un_Content.m_Parallel_Ack.RecvCmd = RecvMsg->Un_CANID.m_CANID.Cmd;
    Para_Ack.Un_CANID.m_CANID.DestID = RecvMsg->Un_CANID.m_CANID.SourceID;
    Para_Ack.Un_CANID.m_CANID.CmdType = RecvMsg->Un_CANID.m_CANID.CmdType;
    //Para_Ack.Un_CANID.m_CANID.ChannelNo = RecvMsg->Un_CANID.m_CANID.ChannelNo;
    Para_Ack.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    Para_Ack.Un_Content.m_Parallel_Ack.SN = RecvMsg->Un_Content.m_ParallelStart.SN;
    Para_Ack.Un_Content.m_Parallel_Ack.errCode = uc_errorcode;
//     Para_Ack.DLC = 8;
//     Para_Ack.FrameType = CAN_EXT_FRAME;
// 	if(CheckCAN_Port_Property(CAN1) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
    //CANFunc::CAN_Push_to_Send(CAN1,&Para_Ack);
    MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(Para_Ack));
    return 0;

}
//------------------------------- CAN RECEIVING MSG ---------------------

unsigned char CANBusParse::Parse_Parallel_Master_RequestStart(MP_CAN_MSG *Msg) //CAN_CMD_PARA_START:  //master --> submaster: start
{
    // 	//??′??￥è???…￥STEPLOAD stepstatus
    // 	ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus=STEPLOADSTEP;
    // 	Step_Manager_task::ClearStepMessage(&DoStepMessageBuf[uc_ChannelNo]);//?2?msg?????′??￥è°??”¨Step_LoadRoutine

    unsigned char uc_ChannelNo;
    uc_ChannelNo = 0;
    //if already started, ignore.
    // 	if (ChannelInfo_Manager::IfGroupStartSafetyCheck(uc_ChannelNo))
    // 		return;
    if(safety_check::IsUnsafe(0,PARA_SUB_CUR_UNSAFE,IV_UNSAFE_TYPE))
        return eParaSubCurUnsafe;
    else if(safety_check::IsUnsafe(0,PARA_SUB_VOL_UNSAFE,IV_UNSAFE_TYPE))
        return eParaSubVolUnsafe;
    else if(safety_check::IsUnsafe(0,IV_POWER_OFF_UNSAFE,IV_UNSAFE_TYPE))
        return eParaSubPowerOffUnsafe;
    else if(safety_check::IsUnsafe(0,FAN_FAULT_UNSAFE,IV_UNSAFE_TYPE))
        return eParaSubFan_FaultUnsafe;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stGroupStatusInfo.m_bGroupCheckSafety = true;   //subunit's channels use this flag to start sys cfg safety checking
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = Msg->Un_Content.m_ParallelStart.m_uCtrlType;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
    Step_Manager_task::m_Range[uc_ChannelNo] = ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = 0;
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = Msg->Un_Content.m_ParallelStart.m_uCtrlType;
    ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, Msg->Un_Content.m_ParallelStart.bNeedFilter);

    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode != eMixMode)
    {
        Step_Manager_task::PrepareForParallelSubGroupRun();   //???????????–?????ˉ???

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPRUNNING;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
        Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));          //??3yD??￠
    }
    return 0;

}
void CANBusParse::Parse_Parallel_Sub_RequestRun(MP_CAN_MSG *Msg) //CAN_CMD_PARA_START:  //master --> submaster: start
{
    unsigned char uc_ChannelNo;
    uc_ChannelNo = 0;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = Msg->Un_Content.m_ParallelStart.m_uCtrlType;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
    Step_Manager_task::m_Range[uc_ChannelNo] = ChannelInfo_Manager::CurrRngPhysicalRange1;//CURRENT_RANGE_1; 17252 zyx
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1; //CURRENT_RANGE_1; 17252 zyx
    if(	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank == eSeriesBoosterRank_Serial)
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stGroupStatusInfo.m_bGroupCheckSafety = true;   //subunit's channels use this flag to start sys cfg safety checking
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_VOLTAGE;

        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue / 2;
        Step_Manager_task::m_Range[uc_ChannelNo] = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = 0;
        unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = Msg->Un_Content.m_ParallelStart.m_fCtrlValue / 2;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = CT_IVCH_VOLTAGE;
//         ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = Msg->Un_Content.m_ParallelStart.bNeedFilter;
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, Msg->Un_Content.m_ParallelStart.bNeedFilter);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange = VOLTAGE_RANGE_HIGH;
        IndependentVoltageCtrl_task::be_ChannelRunFlg[uc_ChannelNo] = true;
        IndependentVoltageCtrl_task::Series2BoosterSendVoltageEnable = true;
        PWM_SampleManage::m_InsideCtrlVoltageValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
//         IndependentVoltageCtrl_task::m_CtrlValue[uc_ChannelNo] =		\
//                 ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
//         CommonFunc::GetTimeTicksUpdate(&Step_Manager_task::Series2BoosterWatchDogTime[uc_ChannelNo]);
    }
    if(	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank == eSeriesBoosterRank_Booster)
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stGroupStatusInfo.m_bGroupCheckSafety = true;   //subunit's channels use this flag to start sys cfg safety checking
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_VOLTAGE;
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue / 2;
        Step_Manager_task::m_Range[uc_ChannelNo] = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = 0;
        unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1; //VOLTAGE_RANGE_HIGH;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = Msg->Un_Content.m_ParallelStart.m_fCtrlValue / 2;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = CT_IVCH_VOLTAGE;
//         ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = Msg->Un_Content.m_ParallelStart.bNeedFilter;
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, Msg->Un_Content.m_ParallelStart.bNeedFilter);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange = VOLTAGE_RANGE_HIGH;
        IndependentVoltageCtrl_task::be_ChannelRunFlg[uc_ChannelNo] = true;
        IndependentVoltageCtrl_task::Series2BoosterSendVoltageEnable = true;
        PWM_SampleManage::m_InsideCtrlVoltageValue = Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
//         IndependentVoltageCtrl_task::m_CtrlValue[uc_ChannelNo] =		\
//                 Msg->Un_Content.m_ParallelStart.m_fCtrlValue;
//         CommonFunc::GetTimeTicksUpdate(&Step_Manager_task::Series2BoosterWatchDogTime[uc_ChannelNo]);
    }
    Step_Manager_task::PrepareForParallelSubGroupRun();   //???????????–?????ˉ???
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPRUNNING;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
    Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));          //??3yD??￠
}
void CANBusParse::Parse_Parallel_Master_SetVolClamp(MP_CAN_MSG *Msg)
{

    ChannelInfo_Manager::ChannelInfo[0].m_fVclampHigh = Msg->Un_Content.m_ParallelClampVolSet.m_ClampVolHigh;// * 1.10f;
    ChannelInfo_Manager::ChannelInfo[0].m_fVclampLow = Msg->Un_Content.m_ParallelClampVolSet.m_ClampVolLow;// - Msg->Un_Content.m_ParallelClampVolSet.m_ClampVolLow * 0.1f;

    if(ChannelInfo_Manager::ChannelInfo[0].EngineStatus != 0)
    {
        unsigned long Code_VH = ChannelInfo_Manager::CalibrateClampDAC(0, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::ChannelInfo[0].m_fVclampHigh);
        unsigned long Code_VL = ChannelInfo_Manager::CalibrateClampDAC(0, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::ChannelInfo[0].m_fVclampLow);

        OS_LogicProcess::m_RunningTask[0].logicProcessMSg.Clamp_Value = Code_VH;
        OS_LogicProcess::m_RunningTask[0].logicProcessMSg.Clamp_Value_2 = Code_VL;

        Step_Manager_task::StepDAC_Out(0, Code_VH, ACTIVE_DAC_VH);
        Step_Manager_task::StepDAC_Out(0, Code_VL, ACTIVE_DAC_VL);
    }
}

void CANBusParse::Parse_Parallel_Master_SetFilter(MP_CAN_MSG *Msg)
{
    unsigned char uc_ChannelNo = 0;
    unsigned char uIntraLatstChanIdx = 0;

//     if(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
//          return ;  //uIntraLatstChanIdx = uc_ChannelNo;
//     else if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
//         uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
//     else
    if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)))// || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)))
        uIntraLatstChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1;

    for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
    {
// 	    ChannelInfo_Manager::ChannelInfo[uc_idx].m_bNeedFilter = Msg->Un_Content.m_ParallelSetFilter.bNeedFilter;
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, Msg->Un_Content.m_ParallelSetFilter.bNeedFilter);
    }
    if(Msg->Un_Content.m_ParallelSetFilter.bRapidlyCommunicate)
    {
        CommonFunc::GetTimeTicksUpdate(&Step_Manager_task::m_RepidlyCommunOverTimeCheck);
        MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_RAPIDLY_COMMUNICATE;
        CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK_RAPIDLY_COMMUNICATE;
        for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
        {
            f_DeltaValueTime_back[uc_idx] = IVSDL::DeltaValue[uc_idx].f_Time;
            IVSDL::SetFactor_Time(uc_idx, IV_SUB_SDL_TIME_RAPIDLY_COMMUNICATE);
            SetRapidlyCommun(uc_idx);
            //SetRapidlyCommun(uc_ChannelNo);
        }
    }
    else
    {
// 		 MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_NORMAL;
// 		 CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK;
        for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
        {
            ClearRapidlyCommun(uc_idx);
            IVSDL::SetFactor_Time(uc_idx, f_DeltaValueTime_back[uc_idx]);
            //ClearRapidlyCommun(uc_ChannelNo);
        }
    }
}
//09.25.2017 add by zc
//void CANBusParse::Parse_Parallel_Master_SetCurrentSimulationVolClamp(MP_CAN_MSG *Msg)
//{
//	ChannelInfo_Manager::ChannelInfo[0].m_fCurrentSimulationVclampHigh = Msg->Un_Content.m_ParallelCurrentSimulationClampVolSet.m_ClampVolHigh*1.10f;
//	ChannelInfo_Manager::ChannelInfo[0].m_fCurrentSimulationVclampLow = Msg->Un_Content.m_ParallelCurrentSimulationClampVolSet.m_ClampVolLow - Msg->Un_Content.m_ParallelCurrentSimulationClampVolSet.m_ClampVolLow*0.1f;
//}

//10.04.2017 add by zc
//void CANBusParse::Parse_Parallel_Master_SetPowerSimulationVolClamp(MP_CAN_MSG *Msg)
//{
//	ChannelInfo_Manager::ChannelInfo[0].m_fPowerSimulationVclampHigh = Msg->Un_Content.m_ParallelPowerSimulationClampVolSet.m_ClampVolHigh*1.10f;
//	ChannelInfo_Manager::ChannelInfo[0].m_fPowerSimulationVclampLow = Msg->Un_Content.m_ParallelPowerSimulationClampVolSet.m_ClampVolLow - Msg->Un_Content.m_ParallelPowerSimulationClampVolSet.m_ClampVolLow*0.1f;
//}


void CANBusParse::Parse_Parallel_Master_RequestStop(MP_CAN_MSG *Msg)//CAN_CMD_PARA_MASTER_STOP:  //master --> submaster: stop
{

    unsigned char uc_ChannelNo = 0;

    // 	//if already stopped, ignore.
    // 	if ( ! ChannelInfo_Manager::IfGroupStartSafetyCheck(uc_ChannelNo))
    // 		return;

    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stGroupStatusInfo.m_bGroupCheckSafety = false;   //subunit's channels use this flag to start sys cfg safety checking

    //	Step_Manager_task::StepTimeTrigger[uc_ChannelNo]=0;
    IR::IRChannelFlg &= ~(1 << uc_ChannelNo); //è￥3y±ê????  è?′?ê±?ú??DDIRμ??°òa??3y±ê??,±ü?a???′2é?ˉ

//     if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
// 	{
// 		PowerAdj::SchRunningFlag[uc_ChannelNo]= false;
// 		bool bNeedStopFlag = false;
// 		unsigned char uc_runningcnt=0;
// 		for(unsigned char uc_channel=0;uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_channel++)
// 		{
// 			if(PowerAdj::SchRunningFlag[uc_channel])
// 			{
// 				uc_runningcnt++;
// 				break;
// 			}
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
//     }
    IVSDL::SubmasterStopRoutine(0);
    IndependentVoltageCtrl_task::be_ChannelRunFlg[uc_ChannelNo] = false;
    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode != eMixMode)
    {
        Step_Manager_task::PrepareForStop(uc_ChannelNo);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 0;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPIDLE;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
        Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));
    }
}
void CANBusParse::Parse_Parallel_Sub_RequestStop(MP_CAN_MSG *Msg) //CAN_CMD_PARA_START:  //master --> submaster: start
{
    unsigned char uc_ChannelNo = 0;
    Step_Manager_task::PrepareForStop(uc_ChannelNo);
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 0;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPIDLE;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
//     IndependentVoltageCtrl_task::m_CtrlValue[uc_ChannelNo] = 0;
    PWM_SampleManage::m_InsideCtrlVoltageValue = 0;
    IndependentVoltageCtrl_task::be_ChannelRunFlg[uc_ChannelNo]=false;
    IndependentVoltageCtrl_task::Series2BoosterSendVoltageEnable = false;
    Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));          //??3yD??￠

}
void CANBusParse::Parse_Parallel_Master_Request_Series2Booster_OutVoltage(MP_CAN_MSG *Msg)
{
    for(unsigned char uc_i  = 0; uc_i<ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter; uc_i++)
    {
        if(ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_Unit_Idx+1 == Msg->Un_CANID.m_CANID.SourceID)
        {
            ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].OutputVoltage = 	Msg->Un_Content.m_Parallel_SubReport_Data.m_SubUnitVoltage;
            return;
        }
    }
}
void CANBusParse::Parse_Parallel_Master_RequestStepChange(MP_CAN_MSG *Msg)//CAN_CMD_PARA_MASTER_STEPCHANGE:  //master --> submaster: jump step
{

    unsigned char uc_ChannelNo;
    uc_ChannelNo = 0;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = Msg->Un_Content.m_ParallelStepChange.m_uCtrlType;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = Msg->Un_Content.m_ParallelStepChange.m_fCtrlValue;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Step_Manager_task::m_Range[uc_ChannelNo] = Msg->Un_Content.m_ParallelStart.m_uRange;

    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] = Msg->Un_Content.m_ParallelStepChange.m_fCtrlValue;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = Msg->Un_Content.m_ParallelStepChange.m_uCtrlType;
    ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, Msg->Un_Content.m_ParallelStepChange.bNeedFilter);

    Step_Manager_task::PrepareForParallelSubGroupRun();   //???????????–?????ˉ???
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = Msg->Un_Content.m_ParallelStart.m_uRange;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPRUNNING;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 0;
    Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[uc_ChannelNo]));          //??3yD??￠

}


void CANBusParse::Parse_Parallel_Master_RequestUpdateOutput(MP_CAN_MSG *Msg)//CAN_CMD_PARA_MATER_UPDATEOUTPUT:  //master --> submaster: update control value
{
    //根据本Unit下实际并联参数计算

    float fNeedValue;
    if((ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_WholeGroupSize  \
            -ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter) > 0)
    {
        fNeedValue =  \
                      Msg->Un_Content.m_ParallelUpdateOutput.m_fCtrlValue * ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_InUnitGroupSize \
                      / (ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_WholeGroupSize  \
                         -ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter) ;
    }
    else
        fNeedValue = Msg->Un_Content.m_ParallelUpdateOutput.m_fCtrlValue * ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_InUnitGroupSize;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = fNeedValue;
    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_stSubGroupCtrl.m_bSubGroupCtrlUpdated = true;  //

}

// void CANBusParse::Parse_Parallel_Master_RequestData(MP_CAN_MSG *Msg)// CAN_CMD_PARA_REQUESTDATA:    //master --> submaster:  request data from submaster
// {
// 	//TODO
// }

//submaster ---->master//

void CANBusParse::Parse_Parallel_Submaster_ReportData(MP_CAN_MSG *Msg)//CAN_CMD_PARA_SUB_REPORTDATA:   //submaster --> master: report data (SDL, resolution current&voltage pair)
{

    ChannelInfo_Manager::ProcessParaSubgroupDataReport(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_Parallel_SubData.m_SubUnitCurrent, MetaCode_PV_Current);
    ChannelInfo_Manager::ProcessParaSubgroupDataReport(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_Parallel_SubData.m_SubUnitVoltage, MetaCode_PV_Voltage);

}



void CANBusParse::Parse_Parallel_Submaster_ReportUnsafe(MP_CAN_MSG *Msg)//CAN_CMD_PARA_SUB_UNSAFE:   //submaster --> master: report unsafe
{

    //can only happen in inter-unit parallel, the master channel must be the first channel, chan idx is 0.
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    uint32 unsafecode;
    if(Msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubCurUnsafe)
        unsafecode = PARA_SUB_CUR_UNSAFE;
    else if(Msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubVolUnsafe)
        unsafecode = PARA_SUB_VOL_UNSAFE;
    else if(Msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubIrregularUnsafe)
        unsafecode = IRREGULAR_UNSAFE;
    else if(Msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubFan_FaultUnsafe)
        unsafecode = PARA_FAN_FAULT_UNSAFE;
    else if(Msg->Un_Content.m_Parallel_SubUnsafe.m_uUnsafeType == eParaSubPowerOffUnsafe)
        unsafecode = PARA_POWER_OFF_UNSAFE;
    else
        return;  //temp

	unsigned short uUnsafeChanIdx = Msg->Un_Content.m_Parallel_SubReport_Unsafe.IV_ChnNo;
    

//     uUnsafeChanIdx = (ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID + 1) * ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount
//                      + uUnsafeChanIdx;
    //TODO:(1)Dèòa?óμ?sdu msg?D
    //(2)??????·?￡?
    unsigned char MasterChannel;
    MasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
    SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
    ChannelInfo_Manager::ChannelInfo[MasterChannel].m_UnsafeType = IV_UNSAFE_TYPE;
    ChannelInfo_Manager::ChannelInfo[MasterChannel].m_SafetyStatus = unsafecode;
    safety_check::SetUnsafe(MasterChannel, unsafecode, 0,IV_UNSAFE_TYPE);
    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
    SchMessage.st_status_sch_running.m_bUnsafeType = IV_UNSAFE_TYPE;
    SchMessage.st_status_sch_running.m_uw32Safe_Code = unsafecode;
    SchMessage.st_status_sch_running.m_btLost_Unit = Msg->Un_CANID.m_CANID.SourceID;
//     SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSpecial[0] = Msg->Un_Content.m_Parallel_SubUnsafe.m_fData;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeValue = Msg->Un_Content.m_Parallel_SubUnsafe.m_fData;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID = uUnsafeChanIdx + 1;
    Schedule_Manager_task::UnsafeContent[MasterChannel] = SchMessage;
    Schedule_Manager_task::SendMsgToSchedule(MasterChannel, SchMessage); //停止aux Unit对应的所有IV通道
    //safety_check::ErrorFlag[0] |= unsafecode;
    //safety_check::ClrUnsafe(0,);
    //safety_check::SetUnsafe(0, unsafecode, Msg->Un_Content.m_Parallel_SubUnsafe.m_fData);

}

void CANBusParse::Parse_Parallel_Submaster_ReportIrregulation(MP_CAN_MSG *Msg)  //CAN_CMD_PARA_SUB_IRREGULAR //submaster --> master: report irregulation
{

// 	unsigned short uUnsafeChanIdx = Msg->Un_Content.m_Parallel_SubReport_Irregulation.m_uChanIdx;

// 	uUnsafeChanIdx = (ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID+1) * ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount
// 		+ uUnsafeChanIdx;

    //safety_check::SetUnsafe(0, PARALLEL_UNIT_IRREGULATION, Msg->Un_Content.m_Parallel_SubReport_Irregulation.m_uType);
    safety_check::SetUnsafe(Msg->Un_Content.m_Parallel_SubReport_Irregulation.m_uChanIdx, IRREGULAR_UNSAFE, 0,IV_UNSAFE_TYPE); //zyx 180705
//     safety_check::SetUnsafe(0, IRREGULAR_UNSAFE, 0); //stop submaster channel first.

}

// void CANBusParse::Parse_Parallel_Submaster_ReportCtrlError(MP_CAN_MSG *Msg)  //CAN_CMD_PARA_SUB_CTRLERROR //submaster --> master: report ctrl value problem
// {

// 	unsigned short uUnsafeChanIdx = Msg->Un_Content.m_Parallel_SubReport_Irregulation.m_uChanIdx;

// 	uUnsafeChanIdx = (ChannelInfo_Manager::gUnit_stUnitInfo.m_MyUnitID +1) * ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount
// 		+ uUnsafeChanIdx;

//safety_check::SetUnsafe(0, PARALLEL_UNIT_IRREGULATION, Msg->Un_Content.m_Parallel_SubReport_Irregulation.m_uType);
//     safety_check::SetUnsafe(0, CTRL_VALUE_CHK_UNSAFE, 0,IV_UNSAFE_TYPE);  //stop submaster channel first.

// }


// void CANBusParse::Parse_Parallel_Submaster_ReportCurrentAbnormal(MP_CAN_MSG *Msg) //CAN_CMD_PARA_SUB_CURR_ABN:   //submaster --> master: report current abnormal
// {
// }

// void CANBusParse::Parse_Parallel_Submaster_ReportVoltageAbnormal(MP_CAN_MSG *Msg)// CAN_CMD_PARA_SUB_VOLT_ABN:   //submaster --> master: report voltage has big different in sub-unit's parallelled channels
// {
// 	//TODO: only for multi-channel, inter-unit parallel. Will support this setting.
// }

#if (USE_AS_AUXMCU == 0 )
bool CANBusParse::SubMasterIfFilter(unsigned short ChanIdx, unsigned short ControlType) //用于判断从IV是否需要使用滤波
{
    bool m_bNeedFilter = 1;
    switch(ControlType)
    {
    case CT_IVCH_SIMU_I:   //12.22.2013ADD     //simulationÊÇ·ñÓ¦¸ÃÆôÓÃSDL£¬ÕâÊÇ¸öÎÊÌâ
    case CT_IVCH_SIMU_P:   //12.22.2013ADD
    case CT_IVCH_SIMU_L:   //12.22.2013ADD
    case CT_IVCH_CURRENT_TB:   //09.05.2018ADD  zc
    case CT_IVCH_CCCV_TB:      //09.05.2018ADD  zc
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_C_CV:
    case CT_IVCH_V_CV:
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_CSTAIRCASE:
    case CT_IVCH_VSTAIRCASE:
    case CT_IVCH_POWER:
    case CT_IVCH_LOAD:
    case CT_IVCH_PRAMP:
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_CRAMP:
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_VRAMP:
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_CPULSE:
//         m_bNeedFilter = 0;
//         break;
    case CT_IVCH_IR:
    case CT_IVCH_ACIR:
    case CT_SPECIAL_IRTEST:
        m_bNeedFilter = 0;
        break;
    case CT_IVCH_SETVARIABLE:
    case CT_IVCH_SETVALUE: // //200911 zyx  need Filter, Or it will lead to a unreasonable value in log data.
    case CT_IVCH_REST:
        break;
    case CT_IVCH_CURRENT:
    case CT_IVCH_VOLTAGE:
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
    case CT_IVCH_SIMU_Battery:  //190218 zyx
        if(ChannelInfo_Manager::ChannelInfo[ChanIdx].m_bDynamicControl)
            m_bNeedFilter = 0;
        break;
// 	case CT_IVCH_REST:
//     case CT_TYPE_DO_SETTING:
//     case CT_IVCH_EXTCHARGE:
    default:
        break;
    }

    return m_bNeedFilter;
}

#endif

//----------------------- SENDING CAN MSG -------------------------------------
//

void CANBusParse::Parallel_MasterSend_Start(bool bNeedfilter, unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range)
{

    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_START;
    Msg.Un_Content.m_Parallel_MsRequest_Start.m_uRange = uc_Range;
    Msg.Un_Content.m_Parallel_MsRequest_Start.m_uCtrlType = ucCtrlType;
    Msg.Un_Content.m_Parallel_MsRequest_Start.m_fCtrlValue = fCtrlValue;
    Msg.Un_Content.m_Parallel_MsRequest_Start.bNeedFilter = bNeedfilter;

    unsigned char  BufIdx = m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd];
	unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
    m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] = (m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] + 1) % PARA_MAX_BUF;
    if(m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus != eParaAckWait)
    {
//         unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID;
        ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType = IV_UNSAFE_TYPE;//0xFF;
        ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus = 0;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus = eParaAckWait;
    }
    else
    {
        return;
    }
    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubUnitCnt = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    memcpy((void*)&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_Msg, (void*)&Msg, sizeof(MP_CAN_MSG));
    for(unsigned char Idx = 0; Idx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1; Idx++)
    {
        OS_LogicProcess::m_bSlaveActive[Idx] = 0;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubStatus[Idx] = eParaAckWait;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubID[Idx] =  \
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[Idx].m_Unit_Idx + 1; //CANFunc::SourceBoardID+Idx+1;
    }
    st_CAN_SN.m_bResendPara = 0; //new msg
    //send can cmd
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].SN = Send_CAN_Cmd(&Msg);

    CommonFunc::GetTimeTicksUpdate(&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_OverTime);
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_ReSendCnt = 0;

    Parallel_MasterSend_Clamp_Volt(uMasterChannel);
//	Parallel_MasterSend_CurrentSimulation_Clamp_Volt(); //09.25.2017 add by zc
//	Parallel_MasterSend_PowerSimulation_Clamp_Volt(); //10.04.2017 add by zc

    if(ucCtrlType == CT_IVCH_CURRENT || ucCtrlType == CT_IVCH_VOLTAGE )
        CAN_WatchdogTask::IsSubMasterSetOpen = true;//161028
    else
        CAN_WatchdogTask::IsSubMasterSetOpen = false;

}
void CANBusParse::Parallel_MasterSend_RUN(unsigned char SubUnitID, bool bNeedfilter, unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range)
{
    MP_CAN_MSG m_Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;
    m_Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    m_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_RUN;
    m_Msg.Un_CANID.m_CANID.DestID = SubUnitID;
    m_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    m_Msg.Un_Content.m_Parallel_MsRequest_Start.m_uRange = uc_Range;
    m_Msg.Un_Content.m_Parallel_MsRequest_Start.m_uCtrlType = ucCtrlType;
    m_Msg.Un_Content.m_Parallel_MsRequest_Start.m_fCtrlValue = fCtrlValue;
    m_Msg.Un_Content.m_Parallel_MsRequest_Start.bNeedFilter = bNeedfilter;
    st_CAN_SN.m_bResendPara = 0; //new msg
    Send_CAN_Cmd(&m_Msg);
}

void CANBusParse::Parallel_MasterSend_Clamp_Volt(unsigned char uc_ChannelNo)
{

    MP_CAN_MSG m_ClampMsg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC,
//     unsigned char chn = 0; //
    m_ClampMsg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    m_ClampMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SETCLAMP;
    m_ClampMsg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;
    m_ClampMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    st_CAN_SN.m_bResendPara = 0; //new msg

    m_ClampMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh;
    m_ClampMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow;
    Send_CAN_Cmd(&m_ClampMsg);

}

//09.25.2017 add by zc
//void CANBusParse::Parallel_MasterSend_CurrentSimulation_Clamp_Volt(void)
//{

//	MP_CAN_MSG m_ClampMsg;
//	unsigned char m_CANType = CAN_PORT_USE_PUBLIC,chn = 0;  //
//	m_ClampMsg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
//	m_ClampMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SET_ISIMUCLAMP;
//	m_ClampMsg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;
//	m_ClampMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//	st_CAN_SN.m_bResendPara = 0; //new msg

//	m_ClampMsg.Un_Content.m_ParallelCurrentSimulationClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fCurrentSimulationVclampHigh;
//	m_ClampMsg.Un_Content.m_ParallelCurrentSimulationClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fCurrentSimulationVclampLow;
//	Send_CAN_Cmd(m_CANType,&m_ClampMsg);

//}

//10.04.2017 add by zc
//void CANBusParse::Parallel_MasterSend_PowerSimulation_Clamp_Volt(void)
//{

//	MP_CAN_MSG m_ClampMsg;
//	unsigned char m_CANType = CAN_PORT_USE_PUBLIC,chn = 0;  //
//	m_ClampMsg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
//	m_ClampMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SET_PSIMUCLAMP;
//	m_ClampMsg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;
//	m_ClampMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//	st_CAN_SN.m_bResendPara = 0; //new msg

//	m_ClampMsg.Un_Content.m_ParallelPowerSimulationClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fPowerSimulationVclampHigh;
//	m_ClampMsg.Un_Content.m_ParallelPowerSimulationClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fPowerSimulationVclampLow;
//	Send_CAN_Cmd(m_CANType,&m_ClampMsg);

//}


void CANBusParse::Parallel_MasterSend_StepChange(bool bNeedfilter, unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range)
{

    MP_CAN_MSG Msg;

//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_MASTER_STEPCHANGE;
    Msg.Un_Content.m_Parallel_MsRequest_StepChange.m_uRange = uc_Range;
    Msg.Un_Content.m_Parallel_MsRequest_StepChange.m_uCtrlType = ucCtrlType;
    Msg.Un_Content.m_Parallel_MsRequest_StepChange.bNeedFilter = bNeedfilter;

    if (ucCtrlType == CT_IVCH_REST)
    {
        Msg.Un_Content.m_Parallel_MsRequest_StepChange.m_fCtrlValue = 0.0f;
    }
    else
    {
        Msg.Un_Content.m_Parallel_MsRequest_StepChange.m_fCtrlValue = fCtrlValue;
    }

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID

    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.

    unsigned char  BufIdx = m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd];

    if(m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus != eParaAckWait)
    {
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus = eParaAckWait;
    }
    else
    {
        return;
    }
    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.
    m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] = (m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] + 1) % PARA_MAX_BUF;
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubUnitCnt = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    for(unsigned char Idx = 0; Idx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1; Idx++)
    {
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubStatus[Idx] = eParaAckWait;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubID[Idx] =  \
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[Idx].m_Unit_Idx + 1; //CANFunc::SourceBoardID+Idx+1;
    }
    //send can cmd
    memcpy((void*)&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_Msg, (void*)&Msg, sizeof(MP_CAN_MSG));
    st_CAN_SN.m_bResendPara = 0; //new msg
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].SN = Send_CAN_Cmd(&Msg);

    CommonFunc::GetTimeTicksUpdate(&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_OverTime);
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_ReSendCnt = 0;
    if(ucCtrlType == CT_IVCH_CURRENT || ucCtrlType == CT_IVCH_VOLTAGE )
        CAN_WatchdogTask::IsSubMasterSetOpen = true;//161028
    else
        CAN_WatchdogTask::IsSubMasterSetOpen = false;

}


void CANBusParse::Parallel_MasterSend_UpdateOutput(float fCtrlValue, bool IsKeyPiont)
{

    //m_ParaDataUpdateTick
    if(!IsKeyPiont) //如果是关键点，必须马上发送新的更新值
    {
        if(CommonFunc::GetTimeTicks( &m_ParaDataUpdateTick) >= CANBusParse::uw32_MaxTime_Para_UpdateOut)
        {   //防止发送给SubMaster的纠错点过于密集
            CommonFunc::GetTimeTicksUpdate(&m_ParaDataUpdateTick);
        }
        else
            return;
    }
    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_MASTER_UPDATEOUTPUT;
    Msg.Un_Content.m_Parallel_MsRequest_UpdateOutput.m_fSeries2BoosterVoltage=ChannelInfo_Manager::IV_Buffer1[0].m_fVoltage;  //03.21.2018 CAN传输电压
    Msg.Un_Content.m_Parallel_MsRequest_UpdateOutput.m_fCtrlValue = fCtrlValue;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID

    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.

    //send can cmd
    Send_CAN_Cmd(&Msg);

}

void CANBusParse::Parallel_MasterSend_Stop()
{

    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_MASTER_STOP;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID

    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.
    unsigned char  BufIdx =  m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd];
    m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] = (m_ParaBufIdx[Msg.Un_CANID.m_CANID.Cmd] + 1) % PARA_MAX_BUF;

//     unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID;
	unsigned char uMasterChannel = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_HeadChn;
    if((ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_UnsafeType==IV_UNSAFE_TYPE )&&(ChannelInfo_Manager::ChannelInfo[uMasterChannel].m_SafetyStatus & CAN_PARA_ACK_UNSAFE == 0))//170510 zyx
    {
        if(m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus != eParaAckWait)
            m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus = eParaAckWait;
        else
            return;
    }
    else
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].CmdStatus = eParaAckOK;

    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubUnitCnt = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    for(unsigned char Idx = 0; Idx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount - 1; Idx++)
    {
        OS_LogicProcess::m_bSlaveActive[Idx] = 0;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubStatus[Idx] = eParaAckWait;
        m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_SubID[Idx] =  \
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[Idx].m_Unit_Idx + 1; //CANFunc::SourceBoardID+Idx+1;
    }
    memcpy((void*)&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_Msg, (void*)&Msg, sizeof(MP_CAN_MSG)); //backup msg
    //send can cmd
    st_CAN_SN.m_bResendPara = 0;  //new msg
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].SN = Send_CAN_Cmd(&Msg);
    CommonFunc::GetTimeTicksUpdate(&m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_OverTime);
    m_Para_MasterAckMgr[Msg.Un_CANID.m_CANID.Cmd][BufIdx].m_ReSendCnt = 0;

    CAN_WatchdogTask::IsSubMasterSetOpen = false;//161028
}

void CANBusParse::Parallel_MasterSend_DoStop(unsigned char SubUnitID)
{
    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_STOP;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID
    Msg.Un_CANID.m_CANID.DestID = SubUnitID;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.
    Send_CAN_Cmd(&Msg);
}
void CANBusParse::Parallel_MasterSend_SetFilter(unsigned char bIfFilter,unsigned char IsRapidlyCommunicate)
{
    MP_CAN_MSG Msg;
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_MASTER_SET_FILTER;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    Msg.Un_CANID.m_CANID.DestID = Parallel_BroadcastID;
    Msg.Un_Content.m_ParallelSetFilter.bNeedFilter = bIfFilter;
    Msg.Un_Content.m_ParallelSetFilter.bRapidlyCommunicate= IsRapidlyCommunicate;
    Send_CAN_Cmd(&Msg);
}
void CANBusParse::Parallel_Series2BoosterSend_OutVoltage(float fVoltageValue)
{
    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SERIES2BOOSTER_REPORT_VOLTAGE;
    Msg.Un_CANID.m_CANID.DestID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx+1;;
    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    Msg.Un_Content.m_Parallel_SubReport_Data.m_SubUnitVoltage = fVoltageValue;
    Send_CAN_Cmd(&Msg);
}
void CANBusParse::Parallel_SubmasterSend_ReportData(float fCurrentValue, float fVoltageValue)
{

    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_REPORTDATA;
    //貌似不需要再3bytes-float
    Msg.Un_Content.m_Parallel_SubReport_Data.m_SubUnitCurrent = fCurrentValue;
    Msg.Un_Content.m_Parallel_SubReport_Data.m_SubUnitVoltage = fVoltageValue;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID

    Msg.Un_CANID.m_CANID.DestID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1; //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.

    //send can cmd
    Send_CAN_Cmd(&Msg);

}

void CANBusParse::Parallel_SubmasterSend_ReportUnsafe(unsigned int uint32_UnsafeType, float fValue,unsigned char uChanIdx)
{

    MP_CAN_MSG Msg;
    unsigned short trnasUnsafeCode = 0;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //è?…???unité????￥CAN0 ??￥é€?è?ˉ?€?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_UNSAFE;
    //change unsafe code type
    if(uint32_UnsafeType == PARA_SUB_CUR_UNSAFE)
        trnasUnsafeCode = eParaSubCurUnsafe;
    else if(uint32_UnsafeType == PARA_SUB_VOL_UNSAFE)
        trnasUnsafeCode = eParaSubVolUnsafe;
    else if(uint32_UnsafeType == IRREGULAR_UNSAFE)
        trnasUnsafeCode = eParaSubIrregularUnsafe;
    else if(uint32_UnsafeType == FAN_FAULT_UNSAFE)
        trnasUnsafeCode = eParaSubFan_FaultUnsafe;
    else if(uint32_UnsafeType == IV_POWER_OFF_UNSAFE)
        trnasUnsafeCode = eParaSubPowerOffUnsafe;
    else
        trnasUnsafeCode = eParaSubUnknowUnsafe;
    Msg.Un_Content.m_Parallel_SubReport_Unsafe.m_uUnsafeType = trnasUnsafeCode;
    Msg.Un_Content.m_Parallel_SubReport_Unsafe.m_fData = fValue;
	Msg.Un_Content.m_Parallel_SubReport_Unsafe.IV_ChnNo = uChanIdx;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ??￥???è?…???unit??? ??¨?????o?o?CNID

    Msg.Un_CANID.m_CANID.DestID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1; //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.

    //send can cmd
    Send_CAN_Cmd(&Msg);

}

void CANBusParse::Parallel_SubmasterSend_ReportIrregulation(unsigned short uChanIdx, unsigned short uType)
{

    MP_CAN_MSG Msg;
//    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //杈呭姪unit閮戒互CAN0 鏉ラ€氳銆?
    Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_IRREGULAR;
    Msg.Un_Content.m_Parallel_SubReport_Irregulation.m_uChanIdx = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Chn_Idx;//uChanIdx; //zyx 180705
    Msg.Un_Content.m_Parallel_SubReport_Irregulation.m_uType = uType;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  浠ユ湰杈呭姪unit鐨?鎷ㄧ爜涓烘簮CNID

    Msg.Un_CANID.m_CANID.DestID = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx;  //for inter unit parallel, CAN cmd is broadcasting, all unit will hear it, but after (SouceID == MasterUnitID, set from ethernet PC batch file cmd) filter, only sub-unit will process the cmd.

    //send can cmd
    Send_CAN_Cmd(&Msg);

}


// void CANBusParse::Parallel_SubmasterSend_ReportCtrlProblem(unsigned short uChanIdx, unsigned short uType)
// {

//     MP_CAN_MSG Msg;
//     unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //杈呭姪unit閮戒互CAN0 鏉ラ€氳銆?
//     Msg.Un_CANID.m_CANID.CmdType = Parallel_CMD_TYPE;
//     Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_PARA_SUB_CTRLERROR;
//     Msg.Un_Content.m_Parallel_SubReport_CtrlError.m_uChanIdx = uChanIdx;
//     Msg.Un_Content.m_Parallel_SubReport_CtrlError.m_uType = uType;




//send can cmd
//     Send_CAN_Cmd(m_CANType, &Msg);

// }
unsigned char CANBusParse::beSubMasterAckUnitID(unsigned char uc_UnitID)
{

    // 	for(unsigned char uc_i=0;
    // 		uc_i<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount;
    // 		uc_i++)
    // 	{
    // 		if(uc_UnitID==ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx)
    // 			return true;
    // 	}
    if(uc_UnitID == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + 1)
        return true;
    else
        return false;

}

#endif    //只有IV unit才对并联响应

/**                    AUX MCU Send Command Part                       **/
/************************************************************************/
#if(USE_AS_AUXMCU == 1)   //aux用

//--------------------------------- Aux Parse Part----------------------------------//
bool CANBusParse::Aux_AddLimitToBank(MP_CAN_MSG *Msg)
{
    unsigned char IvChannelIdx = Msg->Un_Content.m_DownLimit.IV_ChnNo;
    unsigned char uc_EOB;
    unsigned char uc_error=0;
    unsigned char IV_MCUID = Msg->Un_CANID.m_CANID.SourceID;
    unsigned char IV_MCUIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(IV_MCUID);
    if(Msg->Un_Content.m_DownLimit.All)
    {   //this is an any_limit
        //uc_EOB = Aux_DataManager::m_AuxIfAnyLimitCount[IvChannelIdx];
        //uc_EOB用于获取有多少个AUX的Limit

// 		uc_EOB = Aux_DataManager::Aux_GetIfAnyLimitCount(IvChannelIdx); //已合并到Aux_AddIfAnyLimit中 20160811 zyx
// 		if(uc_EOB >= MAX_IFANY_LIMIT_PER_IVCH)
// 		{
// 			uc_error = IFANY_LIMIT_BANK_FULL;
// 			Aux_Confirm_Ack(CANx,Msg,uc_error);
// 			return 0;
// 		}

        //if(Msg->Un_Content.m_DownLimit.FirstLimit == 1)   //启动limit下载时开始屏蔽status上传
        //	st_AuxRptInfoMgr[IV_MCUIdx].m_bRptInfo[IvChannelIdx] = 0;
        ST_AUX_IFANY_LIMIT  st_ifanylimit;
        st_ifanylimit.m_Source_CANID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
        st_ifanylimit.m_limit_SN = Msg->Un_Content.m_DownLimit.SN;
        st_ifanylimit.m_IV_index = Msg->Un_Content.m_DownLimit.IV_ChnNo;
        st_ifanylimit.m_aux_type = Msg->Un_Content.m_DownLimit.AuxChnType;
        st_ifanylimit.m_left_type = Msg->Un_Content.m_DownLimit.Dt + Msg->Un_Content.m_DownLimit.D2t;
        st_ifanylimit.m_ComparsionSign = Msg->Un_Content.m_DownLimit.Sign;
        st_ifanylimit.m_right_value = Msg->Un_Content.m_DownLimit.FloatData;
        st_ifanylimit.m_bReached = 0;
        st_ifanylimit.m_limit_Reached[0] = 0;
        st_ifanylimit.m_limit_Reached[1] = 0;   //初始化成还没有limit到达
        st_ifanylimit.m_limit_alive = 1;
        uc_error = Aux_DataManager:: Aux_AddIfAnyLimit ( st_ifanylimit);
//         Aux_Confirm_Ack(CANx, Msg,	uc_error); //无出错
//         return 1;
    }
    else
    {   //this is a single_limit
        //uc_EOB = Aux_DataManager::m_AuxIfAnyLimitCount[IvChannelIdx];
//         uc_EOB = Aux_DataManager::Aux_GetLimitCount(IvChannelIdx);

//         if(uc_EOB >= IV_MAX_LIMIT_NUMBER)
//         {
//             uc_error = LIMIT_BANK_FULL;
//             Aux_Confirm_Ack(Msg, uc_error);
//             return 0;
//         }
        //if(Msg->Un_Content.m_DownLimit.FirstLimit == 1)
        //	st_AuxRptInfoMgr[IV_MCUIdx].m_bRptInfo[IvChannelIdx] = 0;
        ST_AUX_LIMIT  st_limit;
        st_limit.m_limit_SN = Msg->Un_Content.m_DownLimit.SN;
        st_limit.m_Source_CANID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
        st_limit.m_IV_index = Msg->Un_Content.m_DownLimit.IV_ChnNo;
        st_limit.m_aux_type = Msg->Un_Content.m_DownLimit.AuxChnType;
        st_limit.m_chn_index = Msg->Un_Content.m_DownLimit.AuxChnNo;
        st_limit.m_left_type = Msg->Un_Content.m_DownLimit.Dt + Msg->Un_Content.m_DownLimit.D2t;
        st_limit.m_ComparsionSign = Msg->Un_Content.m_DownLimit.Sign;
        st_limit.m_right_value = Msg->Un_Content.m_DownLimit.FloatData;
        st_limit.m_limit_Reached = 0;
        st_limit.m_limit_alive = 1;
        uc_error = Aux_DataManager:: Aux_AddLimit(st_limit);
//         Aux_Confirm_Ack(CANx, Msg, uc_error);
//         return 1;

    }
    Aux_Confirm_Ack( Msg, uc_error);
    return 1;
}
void CANBusParse::Aux_ClearIVsLimit(MP_CAN_MSG *Msg)
{
    unsigned char IvChannelIdx = Msg->Un_Content.m_DownLimit.IV_ChnNo;
    unsigned char uc_IVunitID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
    StepID[IvChannelIdx] = Msg->Un_Content.m_ClearLimit.StepID;
    unsigned char IVUnitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(uc_IVunitID);

    //原来设计清除可以分别清除单独limit和IFANYlimit。现在看来没有必要。因为两者总是同时消亡。
    // 原来那个ALL也不是指所有IV通道，是指所有辅助通道。
    //    if(Msg->Un_Content.m_ClearLimit.All == 0)             //one All 需要定义是0/1 还是0 0xff
    //    {
    // 	 uc_EOB = Aux_DataManager::Clear_OneLimit(IvChannelIdx);
    // 	 if(uc_EOB == IV_CHNNO_ERROR)
    // 		{
    // 			Aux_Confirm_Ack(CANx,Msg,IV_CHNNO_ERROR);
    // 			return;
    // 	    }
    //    }
    //    else if(Msg->Un_Content.m_ClearLimit.All == 1)   //如果为ALL 则不判断IvChannelIdx
    //    {
    // 	 Aux_DataManager::Clear_AllLimit();
    //    }
    //    else
    //    {
    // 	    Aux_Confirm_Ack(CANx,Msg,ALLBIT_ERROR);
    // 		return;
    //    }

    if(IvChannelIdx >= MAX_IVCH_PER_IVMCU)
        Aux_Confirm_Ack(Msg, IV_CHNO_OVERFLOW);
    else
    {
        Aux_Confirm_Ack(Msg, Aux_DataManager::Aux_ClearLimits(uc_IVunitID, IvChannelIdx));
        Aux_DataManager::Aux_ClearDataRequests(uc_IVunitID, IvChannelIdx); //在这里清除RequestBank
    }
}


// void CANBusParse::Aux_SdlEnable(MP_CAN_MSG *Msg)
// {
//     //MP_CAN_MSG Cmd_Enable_Sdl;
//     unsigned char Enable;
//     unsigned char IV_MCUIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);

//     Enable = Msg->Un_Content.m_EnableSdl.Enable;  //
//     //st_AuxRptInfoMgr[IV_MCUIdx].m_bRptInfo[Msg->Un_Content.m_EnableSdl.IV_ChnNo] = 1;    // 利用enableSDL判断limit是否下载完成
//     // 	if(All == 0)
//     // 	{
//     //if(Enable)
//     //    AuxSDL::SwitchLog( Msg->Un_Content.m_EnableSdl.IV_ChnNo, Msg->Un_CANID.m_CANID.ChannelType, Msg->Un_CANID.m_CANID.AuxChn, LOG_PC_DATA, LOGSWITCH_ON);
//     //else
//     //    AuxSDL::SDLEndedRoutine(Msg->Un_Content.m_EnableSdl.IV_ChnNo, Msg->Un_CANID.m_CANID.ChannelType, Msg->Un_CANID.m_CANID.AuxChn);
//     if(Enable)
//         Enable = LOGSWITCH_ON;
//     else
//         Enable = LOGSWITCH_OFF;
//     AuxSDL::SwitchLog(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_EnableSdl.IV_ChnNo,  Msg->Un_Content.m_EnableSdl.AuxChnType, Msg->Un_Content.m_EnableSdl.AuxChn, LOG_PC_DATA, Enable);

//     Aux_Confirm_Ack(Msg, NO_ERROR);
// }

void CANBusParse::Aux_RxSetCtrl(MP_CAN_MSG *Msg)
{
    //MP_CAN_MSG Cmd_Enable_Sdl;
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo;
    unsigned char uc_AuxCh = 0;
    unsigned char bValid = 0;
    if(unitIdx >= MAX_IVMCU_PER_SYS || IV_ChnIdx >= MAX_IVCH_PER_IVMCU)
        return;

    //transfer virtual aux chn to physical chn
    if( Msg->Un_Content.m_SetCtrl.AuxChnType == AUX_DO)
        Aux_RxDOCtrl(Msg);
    else if(( Msg->Un_Content.m_SetCtrl.AuxChnType == AUX_TEMPERATURE) || ( Msg->Un_Content.m_SetCtrl.AuxChnType == AUX_HUMIDITY)) //改AUX_FLOW_RATE为AUX_HUMIDITY
    {
        if(Aux_Chamber_TEC_Base::Aux_Chamber_TEC_Board == true)  //Control Arbin TEC Chamber, add by ZC 01.02.2018
            Aux_RxTEC_ChamberCtrl(Msg);
        else if(Aux_MTC_Base::Aux_MTC_Board == true)  //Control Arbin MTC, add by ZC 04.24.2018
            Aux_RxMTCCtrl(Msg);
        else
            Aux_RxTempChamberCtrl(Msg);
    }
    else if ( Msg->Un_Content.m_SetCtrl.AuxChnType == AUX_EXTERNALCHARGE)
        CANBusParse::Aux_RxExtChargeCtrl(Msg);
}

/*用于控制温控箱开关*/
void CANBusParse::Aux_RxCtrlRunON(MP_CAN_MSG *Msg)
{
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_CtrlRunOn.IV_ChnNo;
    unsigned char uc_AuxCh = 0;
    unsigned char bValid = 0;
    if(unitIdx >= MAX_IVMCU_PER_SYS || IV_ChnIdx >= MAX_IVCH_PER_IVMCU) //如果unitIdx大于每个系统最大IVMCU（16）或大于每个IVMCU的最大通道数
        return;

    if(( Msg->Un_Content.m_CtrlRunOn.AuxChnType == AUX_TEMPERATURE) || ( Msg->Un_Content.m_CtrlRunOn.AuxChnType == AUX_HUMIDITY)) //改AUX_FLOW_RATE为AUX_HUMIDITY
    {
        //目前AUX和温控箱通信只用到usart0  20160612 zyx
        if(Msg->Un_Content.m_CtrlRunOn.RunON == 1)
        {
            if(Aux_MTC_Base::Aux_MTC_Board == true) //add by zc 08.09.2018 for MTC
            {
                Aux_MTC_Base::TurnOnMTCChn( Msg->Un_Content.m_SetCtrl.AuxChnNo );
            }
            else
            {
                //发送运行命令开启温控箱
                for(unsigned char b_id = 0; b_id<MAX_BOARD_PER_AUXMCU; b_id++)
                {
                    if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_UART != 0)
                    {
                        //目前AUX和温控箱通信只用到usart0  20160612 zyx
                        MTCI_CTRL::SetPointValueCmd(0,SETPOINTVALUE_TEMPERATURE, Msg->Un_Content.m_CtrlRunOn.RunON);	//该函数第二个参数无意义
                        Aux_Confirm_Ack(Msg,NO_ERROR);
                    }
                }
            }
        }
        else
        {
            if(Aux_MTC_Base::Aux_MTC_Board == true) //add by zc 08.09.2018 for MTC
            {
                Aux_MTC_Base::TurnOffMTCChn( Msg->Un_Content.m_SetCtrl.AuxChnNo );
                Aux_MTC_Base::MTCMode[Msg->Un_Content.m_SetCtrl.AuxChnNo] = 0;
            }
            else
            {
                //发送停止命令关闭温控箱
                for(unsigned char b_id = 0; b_id<MAX_BOARD_PER_AUXMCU; b_id++)
                {
                    if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_UART != 0)
                    {
                        //目前AUX和温控箱通信只用到usart0  20160612 zyx
                        MTCI_CTRL::ExecStopCmd(0,SETPOINTVALUE_TEMPERATURE);	//该函数第二个参数无意义
                        Aux_Confirm_Ack(Msg,NO_ERROR);
                    }
                }
            }
        }
    }
}

void CANBusParse::Aux_RxSetMTCSafetyCheck(MP_CAN_MSG *Msg) //add by zc for MTC 07.17.2018
{
    //MP_CAN_MSG Cmd_Enable_Sdl;
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo;
    unsigned char uc_AuxCh = 0;
    unsigned char bValid = 0;
    unsigned char aux_type = 0;
    unsigned char AuxChanNo = Msg->Un_Content.m_SetCtrl.AuxChnNo;
//	Aux_MTC_Base::MtcCanDestID[AuxChanNo] = Msg->Un_CANID.m_CANID.SourceID;
//	Aux_MTC_Base::MtcDestChnNo[AuxChanNo] = Msg->Un_Content.m_Unsafe.IV_ChnNo;
    if(unitIdx>=MAX_IVMCU_PER_SYS || IV_ChnIdx>=MAX_IVCH_PER_IVMCU)
        return;

    if( (Msg->Un_Content.m_SetCtrl.AuxChnType!= AUX_TEMPERATURE) ) //only support temperature control
        return;
    else
        aux_type = Msg->Un_Content.m_SetCtrl.AuxChnType;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    Aux_MTC_Base::MTCSafetyCheckTimeValue[AuxChanNo] = Msg->Un_Content.m_SetCtrl.Set_Value.m_uMTCSafetyTimeValue;
    Aux_MTC_Base::MTCTempSafetyGoto[AuxChanNo] = Msg->Un_Content.m_SetCtrl.SN;
    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[AuxChanNo] = 1;
    Aux_MTC_Base::MTCSafetyCheckTimeBuffer[AuxChanNo] = CommonFunc::GetSystemTime();
}

void CANBusParse::Aux_RxClearMTCSafetyCheck(MP_CAN_MSG *Msg) //add by zc for MTC 08.1202018
{
    //MP_CAN_MSG Cmd_Enable_Sdl;
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo;
    unsigned char aux_type = 0;
    unsigned char AuxChanNo = Msg->Un_Content.m_SetCtrl.AuxChnNo;
//	Aux_MTC_Base::MtcCanDestID[AuxChanNo] = Msg->Un_CANID.m_CANID.SourceID;
//	Aux_MTC_Base::MtcDestChnNo[AuxChanNo] = Msg->Un_Content.m_Unsafe.IV_ChnNo;
    if(unitIdx>=MAX_IVMCU_PER_SYS || IV_ChnIdx>=MAX_IVCH_PER_IVMCU)
        return;

    if( (Msg->Un_Content.m_SetCtrl.AuxChnType!= AUX_TEMPERATURE) ) //only support temperature control
        return;
    else
        aux_type = Msg->Un_Content.m_SetCtrl.AuxChnType;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[AuxChanNo] = 0;

}

void CANBusParse::Aux_RxDOCtrl(MP_CAN_MSG *Msg)
{
    //MP_CAN_MSG Cmd_Enable_Sdl;
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID); //从源ID找到IVunit ID 在buffer中的序号
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo; //IV通道号
    //unsigned char uc_AuxCh = 0;　//初始化DO通道
    unsigned char bValid = 0;
    if(unitIdx >= MAX_IVMCU_PER_SYS || IV_ChnIdx >= MAX_IVCH_PER_IVMCU)
        return;
    //transfer virtual aux chn to physical chn
    if( Msg->Un_Content.m_SetCtrl.AuxChnType != AUX_DO)
        return;
    //	ST_AUX_DOCtrl * pOneDOCtrl = &Aux_DataManager::m_AuxDOCtrlMgr[unitIdx][IV_ChnIdx];
    //	pOneDOCtrl->m_DoCount = 0;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_DO);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    unsigned char ActualDOCount = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap;
    unsigned char uc_DOChnIdx = 0;
    unsigned char uc_DOChInAuxUnit = 0;
    bool bIO;
    //	for(unsigned char boardIdx = 0;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)
    //	{
    //		if((Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType!= AUX_DO)   \
    || !Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_AliveFlg)
    //		{
    //			continue;
    //		}
    while(uc_DOChnIdx < ActualDOCount)
{

    if(uc_DOChnIdx >= MAX_DIDO_PIN_PER_BOARD)
            break;
        //	unsigned char uc_DOChInUint = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_DO].m_MapInfo[DOChnIdx].m_Index_InAuxUnit;
        //	if((uc_DOChInUint>=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit)
        //		&&(uc_AuxChInUint<=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_End_InUnit))
        //{
        // 				unsigned char phyChnIdx = uc_AuxChInUint- Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit;
        // 				pOneDOCtrl->m_DOValue[auxChnIdx].m_BID = boardIdx;
        // 				pOneDOCtrl->m_DOValue[auxChnIdx].m_CID = Aux_DataManager::Convert_PhysicalChID(boardIdx, phyChnIdx);
        // 				pOneDOCtrl->m_DoCount++;
        //bool bIO = Msg->Un_Content.m_SetDOCtrl.m_32bitDO &(1<<uc_DOChInUint);
        // 				bool bIO = Msg->Un_Content.m_SetCtrl.Set_Value.m_32bitDO &(1<<uc_DOChnIdx);

        unsigned char uc_data_group_idx = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[uc_DOChnIdx].m_data_group_idx;
        unsigned char uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[uc_DOChnIdx].m_chn;
        // 					pOneDOCtrl->m_DOValue[auxChnIdx].m_bValid = 1;
        // 					if(bIO)
        // 						pOneDOCtrl->m_DOValue[auxChnIdx].m_bOut |= bIO;
        // 					else
        // 					{
        // 						pOneDOCtrl->m_DOValue[auxChnIdx].m_bOut = bIO;
        // 					}
        if (DIDO_Ctrl == 1)
        {
            uc_DOChInAuxUnit = uc_chn - 10;
            bIO = Msg->Un_Content.m_SetCtrl.Set_Value.m_32bitDO & (1 << uc_DOChInAuxUnit);
        }
        else if(DIDO_Ctrl == 2)
        {
//             uc_DOChInAuxUnit=uc_chn;
            uc_DOChInAuxUnit = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[uc_DOChnIdx].m_Index_InAuxUnit;
            bIO = Msg->Un_Content.m_SetCtrl.Set_Value.m_32bitDO & (1 << uc_DOChInAuxUnit);
        }
        Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].updated = 1;

        Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_data = bIO;     //160429,no need of an if else
        Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].value_update_time = CommonFunc::GetSystemTime();  //160429

        uc_DOChnIdx++;
        //}
    }
    //end for
    Aux_Confirm_Ack(Msg, NO_ERROR);
}

void CANBusParse::Aux_DownloadScheduleMap(MP_CAN_MSG *Msg)
{
// 	AUX_SampleProcess::b_AuxTempNeedFilterFlag = true;	//打开辅助温度的滤波 MPSetChannelCalibrate()时会将这个标志置0
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_DownSchMap.IV_ChnNo;
    if(unitIdx >= MAX_IVMCU_PER_SYS || IV_ChnIdx >= MAX_IVCH_PER_IVMCU)
        return;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(Msg->Un_Content.m_DownSchMap.AuxChnType);
    if(TempAuxTypeIdex == 0xFF)
        return;//辅助限制  查找该辅助类型是否存在，
//     if(Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_ActualCounts >= Msg->Un_Content.m_DownSchMap.AuxCount) //180905 zyx
//     Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_ActualCounts = Msg->Un_Content.m_DownSchMap.AuxCount;
	
	Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap = Msg->Un_Content.m_DownSchMap.AuxCount;
    return;
}
void CANBusParse::Aux_RxAOCtrl(MP_CAN_MSG *Msg)
{
    unsigned long data = 0;
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo;
    if(unitIdx >= MAX_IVMCU_PER_SYS || IV_ChnIdx >= MAX_IVCH_PER_IVMCU)
        return;
    unsigned char uc_AIChn = 0;
    uc_AIChn = Msg->Un_Content.m_SetAIOCtrl.Aux_AI_ChnNo;
    unsigned short value1 = (unsigned short)(Msg->Un_Content.m_SetAIOCtrl.m_PID_Value.Set_value);
    float value;
    if(value1 >= 0x4000)
    {
        value1 -= 0x4000;
        value = ((float)value1) / 100;
    }
    else
    {
        value1 = 0x4000 - value1;
        value = 0 - ((float)value1 / 100);
    }


    unsigned char uc_data_group_idx = 0;
    unsigned char uc_chn = 0;
// 	  for (unsigned char uc_i = 0; uc_i < Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_ActualCounts; uc_i++)
// 	  {
// 		  if (Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_MapInfo[uc_i].m_Index_InAuxUnit == Msg->Un_Content.m_SetAIOCtrl.AUX_AO_ChnNO)
// 		  {
// 			  uc_data_group_idx = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_MapInfo[uc_i].m_data_group_idx;
// 			  uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_MapInfo[uc_i].m_chn;
// 			  break;
// 		  }
// 	  }

    Aux_DataManager::TransAuxChnFromSequence2Physical(Msg->Un_CANID.m_CANID.SourceID, \
            IV_ChnIdx, \
            AUX_AO, \
            Msg->Un_Content.m_SetAIOCtrl.AUX_AO_ChnNO, \
            &uc_data_group_idx, \
            &uc_chn); // 20170228 zyx









    //unsigned char uc_data_group_idx = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_MapInfo[Msg->Un_Content.m_SetAIOCtrl.AUX_AO_ChnNO].m_data_group_idx;
    //unsigned char uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_AO].m_MapInfo[Msg->Un_Content.m_SetAIOCtrl.AUX_AO_ChnNO].m_chn;
    Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_data = value;
    Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].updated = 1;
    Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].value_update_time = CommonFunc::GetSystemTime();  //160429
    data = Aux_DataManager::CalibrateVoltageDAC(uc_data_group_idx, uc_chn, value);
    Aux_AIO_DIO_602178_WMADC602548_Base::Dac_out(uc_chn, data);
    if(!(Msg->Un_Content.m_SetAIOCtrl.On_Off_Flag & 0x0001))
    {
        AuxPID_task::beNeedPID[uc_chn] = 0x00;
    }
    else
    {
        if(uc_AIChn == 0xFF)
        {
            AuxPID_task::mAuxPID_par[uc_chn].m_SetValue_Open =  data;
            AuxPID_task::beNeedPID[uc_chn] = 0x01; //开环不做PID处理
        }
        else
        {
            AuxPID_task::beNeedPID[uc_chn] = 0x02;
            AuxPID_task::mAuxPID_par[uc_chn].m_DataBank = uc_data_group_idx;
            AuxPID_task::mAuxPID_par[uc_chn].m_SetValue_1B =  Aux_DataManager::UnifyVoltage(uc_chn, value);
            AuxPID_task::mAuxPID_par[uc_chn].m_LastOutput_1B =  AuxPID_task::mAuxPID_par[uc_chn].m_SetValue_1B;
            AuxPID_task::mAuxPID_par[uc_chn].m_fLastErr_1B  =  0;
            AuxPID_task::mAuxPID_par[uc_chn].Kp = Msg->Un_Content.m_SetAIOCtrl.m_PID_Value.kP;
            AuxPID_task::mAuxPID_par[uc_chn].Ki = (float)(Msg->Un_Content.m_SetAIOCtrl.m_PID_Value.kI) / 100;
            AuxPID_task::mAuxPID_par[uc_chn].Kd = 0;

            unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(Msg->Un_Content.m_SetAIOCtrl.Aux_AI_ChnType);
            if(TempAuxTypeIdex == 0xFF)//辅助限制 牛亚松
                return ;

            for(int i = 0; i < Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //180825 zyx
            {
                if(uc_AIChn == Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit)
                {
                    AuxPID_task::mAuxPID_par[uc_chn].m_AI_DataBank = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[i].m_data_group_idx;
                    AuxPID_task::mAuxPID_par[uc_chn].m_AI_Chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[i].m_chn;
                }
            }
        }
    }
}
void CANBusParse::Aux_RxTempChamberCtrl(MP_CAN_MSG *Msg)
{
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID); //从源ID找到IVunit ID 在buffer中的序号
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo; //IV通道号
    unsigned char bValid = 0;
    unsigned char aux_type = 0;
    if( (unitIdx >= MAX_IVMCU_PER_SYS) || (IV_ChnIdx >= MAX_IVCH_PER_IVMCU) )
        return;
    //transfer virtual aux chn to physical chn
    if( (Msg->Un_Content.m_SetCtrl.AuxChnType != AUX_TEMPERATURE) && (Msg->Un_Content.m_SetCtrl.AuxChnType != AUX_HUMIDITY) )
        return;
    else
        aux_type = Msg->Un_Content.m_SetCtrl.AuxChnType;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    unsigned char ActualTCCount = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap;
    unsigned char uc_TCChnIdx = 0;
    while(uc_TCChnIdx < ActualTCCount)
    {

        if( (aux_type == AUX_TEMPERATURE) && (uc_TCChnIdx >= MAX_CH_IN_AUX_UNIT) )
            break;
        if( (aux_type == AUX_HUMIDITY) && (uc_TCChnIdx >= MAX_HUMIDITY_PER_BOARD)  )
            break;

        float SetPoint = (Msg->Un_Content.m_SetCtrl.Set_Value.f_SP * 10);

        unsigned char uc_data_group_idx = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[uc_TCChnIdx].m_data_group_idx;
        unsigned char uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[uc_TCChnIdx].m_chn;
        if(aux_type == AUX_TEMPERATURE)
        {
            for(unsigned idx = 0; idx < MAX_UART_NUM; idx++)
            {
                for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[idx].uc_Chamber_TempCount; uc_Chamber++)
                {
                    if( (MTCI_CTRL::str_Uart[idx].T_in_group_id[uc_Chamber] == uc_data_group_idx) || (MTCI_CTRL::str_Uart[idx].T_in_chan_id[uc_Chamber] == uc_chn) )
                    {
                        if(MTCI_CTRL::st_GroupControler[idx].m_UseGroupTChamber)
                        {
                            MTCI_CTRL::st_GroupControler[idx].m_ChamberSetValue[unitIdx][IV_ChnIdx].IsSetTempValue = true;
                            MTCI_CTRL::st_GroupControler[idx].m_ChamberSetValue[unitIdx][IV_ChnIdx].TempValue = SetPoint;
                            if( MTCI_CTRL::st_GroupControler[idx].m_UnitIP != Msg->Un_CANID.m_CANID.SourceID || MTCI_CTRL::st_GroupControler[idx].m_IVChnIndexInUnit != IV_ChnIdx)//1910101 zyx
                                return;
                        }

                        MTCI_CTRL::gW_TemperatureValue[idx][uc_Chamber] = SetPoint;
                        MTCI_CTRL::gW_HumiFlag[idx] = 0;
                        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_HumiInterval[idx]);  // 对串口发出过命令则更新时间
                        MTCI_CTRL::gW_IsWriteCmdOk[idx] = false;
                        MTCI_CTRL::gW_WriteCmd_Flag[idx] = SETPOINTVALUE_MAX;
                        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[idx]);  // 对串口发出过命令则更新时间
                    }
                    //	Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Set_Value = SetPoint;
                }
            }
        }
        else
        {
            for(unsigned idx = 0; idx < MAX_UART_NUM; idx++)
            {
                for(unsigned char uc_Chamber = 0; uc_Chamber < MTCI_CTRL::str_Uart[idx].uc_Chamber_HumiCount; uc_Chamber++)
                {
                    if( (MTCI_CTRL::str_Uart[idx].H_in_group_id[uc_Chamber] == uc_data_group_idx) || (MTCI_CTRL::str_Uart[idx].H_in_chan_id[uc_Chamber] == uc_chn) )
                    {
                        if(MTCI_CTRL::st_GroupControler[idx].m_UseGroupTChamber)
                        {
                            MTCI_CTRL::st_GroupControler[idx].m_ChamberSetValue[unitIdx][IV_ChnIdx].IsSetHumiValue = true;
                            MTCI_CTRL::st_GroupControler[idx].m_ChamberSetValue[unitIdx][IV_ChnIdx].HumiValue = SetPoint;
                            if( MTCI_CTRL::st_GroupControler[idx].m_UnitIP != Msg->Un_CANID.m_CANID.SourceID || MTCI_CTRL::st_GroupControler[idx].m_IVChnIndexInUnit != IV_ChnIdx)//1910101 zyx
                                return;
                        }
                        MTCI_CTRL::gW_HumiValue[idx][uc_Chamber] = SetPoint;
                        MTCI_CTRL::gW_HumiFlag[idx] = 1;
                        //CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_HumiInterval[idx]);  // 对串口发出过命令则更新时间
                    }
                    //	Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Set_Value = SetPoint;
                }
            }
        }
// 			Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Set_Value = SetPoint; //没有必要放进循环中 //1908271 dyy
        uc_TCChnIdx++;
    }
    //end for
    Aux_Confirm_Ack(Msg, NO_ERROR);

}

void CANBusParse::Aux_RxTEC_ChamberCtrl(MP_CAN_MSG *Msg) //Add by ZC 01.02.2018
{
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID); //从源ID找到IVunit ID 在buffer中的序号
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo; //IV通道号
    unsigned char aux_type = 0;
    unsigned char AuxChanNo = Msg->Un_Content.m_SetCtrl.AuxChnNo;
    //只有 1，带副板时的第17个通道  或者 2，不带副板时的第9个通道才会允许执行，否则不会改变温度
    if((Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave) && (AuxChanNo != 0x10))
        return;
    else if((!Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave) && (AuxChanNo != 0x08))
        return;

    if( (unitIdx >= MAX_IVMCU_PER_SYS) || (IV_ChnIdx >= MAX_IVCH_PER_IVMCU) )
        return;
    //transfer virtual aux chn to physical chn
    if( (Msg->Un_Content.m_SetCtrl.AuxChnType != AUX_TEMPERATURE) ) //TEC chamber only supports temperature control
        return;
    else
        aux_type = Msg->Un_Content.m_SetCtrl.AuxChnType;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    TMP_KEYFunc::TempTemp = Msg->Un_Content.m_SetCtrl.Set_Value.f_SP * 1.0f;  //取得温度设定值
    if(TMP_KEYFunc::TempTemp >= 60.0f)
        TMP_KEYFunc::TempTemp = 60.0f;
    if(TMP_KEYFunc::TempTemp <= 10.0f)
        TMP_KEYFunc::TempTemp = 10.0f;
    HT16K33::DispTemp(TMP_KEYFunc::TempTemp);
    for(int digit_i = 0; digit_i < 3; digit_i++)
        Aux_Chamber_TEC_Base::TempRec[digit_i] = HT16K33::Digit[digit_i];
    Aux_Chamber_TEC_Base::Set_TempPoint();	//设定温度
    HT16K33::DispTemp(Aux_Chamber_TEC_Base::Temperature1);
    //end for
    Aux_Confirm_Ack(Msg, NO_ERROR);
}
void CANBusParse::Aux_RxMTCCtrl(MP_CAN_MSG *Msg) //Add by ZC 04.24.2018
{
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID); //从源ID找到IVunit ID 在buffer中的序号
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo; //IV通道号
    unsigned char aux_type = 0;
    unsigned char AuxChanNo = Msg->Un_Content.m_SetCtrl.AuxChnNo;

    if( (unitIdx>=MAX_IVMCU_PER_SYS) || (IV_ChnIdx>=MAX_IVCH_PER_IVMCU) )
        return;
    //transfer virtual aux chn to physical chn
    if( (Msg->Un_Content.m_SetCtrl.AuxChnType!= AUX_TEMPERATURE) ) //only support temperature control
        return;
    else
        aux_type = Msg->Un_Content.m_SetCtrl.AuxChnType;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    unsigned char chamberIdx = 0;
    unsigned char numberOfGroupT = 0;

    while(chamberIdx < MTC_MAX_CHANNELNO) //add by WL 11.05.19 for MTC Group T-Chamber control
    {
        if(Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_IVChnIndexInUnit == IV_ChnIdx &&
                Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_UnitIP == Msg->Un_CANID.m_CANID.SourceID &&
                Aux_MTC_Base::st_MTC_Group_Ctrl[chamberIdx].m_UseGroupTChamber == true)
        {
            //isMasterIV_ChnIdx = true; //for debugging purposes
            //testval[IV_ChnIdx] = Msg->Un_Content.m_SetCtrl.Set_Value.f_SP;
            //test[IV_ChnIdx] = 1;

            Aux_MTC_Base::MTCMode[AuxChanNo] = 1; //MTC Remote Mode
            DWIN_UartParse::MITSwitchRemote(AuxChanNo);

            if(Msg->Un_Content.m_SetCtrl.Set_Value.f_SP > 60)
                Aux_MTC_Base::Temperature_set[AuxChanNo] = 600;
            else if(Msg->Un_Content.m_SetCtrl.Set_Value.f_SP < 10)
                Aux_MTC_Base::Temperature_set[AuxChanNo] = 100;
            else
                Aux_MTC_Base::Temperature_set[AuxChanNo] = (long long) (Msg->Un_Content.m_SetCtrl.Set_Value.f_SP * 10.0f);  //取得温度设定值

            Aux_MTC_Base::Set_TempPoint(AuxChanNo);
            Aux_Confirm_Ack(Msg,NO_ERROR);
            return;
        }
        chamberIdx++;
    }

    if(Aux_MTC_Base::st_MTC_Group_Ctrl[AuxChanNo].m_UseGroupTChamber == false)
    {
        Aux_MTC_Base::MTCMode[AuxChanNo] = 1; //MTC Remote Mode
        DWIN_UartParse::MITSwitchRemote(AuxChanNo);

        if(Msg->Un_Content.m_SetCtrl.Set_Value.f_SP > 60)
            Aux_MTC_Base::Temperature_set[AuxChanNo] = 600;
        else if(Msg->Un_Content.m_SetCtrl.Set_Value.f_SP < 10)
            Aux_MTC_Base::Temperature_set[AuxChanNo] = 100;
        else
            Aux_MTC_Base::Temperature_set[AuxChanNo] = (long long) (Msg->Un_Content.m_SetCtrl.Set_Value.f_SP * 10.0f);  //取得温度设定值

        Aux_MTC_Base::Set_TempPoint(AuxChanNo);
    }

    Aux_Confirm_Ack(Msg,NO_ERROR);
}
void CANBusParse::Aux_RxExtChargeCtrl(MP_CAN_MSG *Msg)
{
    unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID); //从源ID找到IVunit ID 在buffer中的序号
    unsigned char IV_ChnIdx = Msg->Un_Content.m_SetCtrl.IV_ChnNo; //IV通道号

    if( Msg->Un_Content.m_SetCtrl.AuxChnType != AUX_EXTERNALCHARGE)
        return;

    // 	unsigned char ActualExChargeCount = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_EXTERNALCHARGE].m_ActualCounts;
    // 	unsigned char uc_ChnIdx = 0;
    // 	unsigned char uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_EXTERNALCHARGE].m_MapInfo[0].m_chn;
    // 	MetaVariable_Task::MetaTaskRunCounter=0;

    // 	while(uc_ChnIdx < ActualExChargeCount)
    // 	{
    // 	    if(uc_ChnIdx> 8)
    // 			    break;
    // 			else
    // 			{
    unsigned int value = Msg->Un_Content.m_SetCtrl.Set_Value.m_32bitDO;
    //unsigned char uc_data_group_idx = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][AUX_EXTERNALCHARGE].m_MapInfo[uc_ChnIdx].m_data_group_idx;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_EXTERNALCHARGE);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    unsigned char uc_chn = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[0].m_Index_InAuxUnit;	 //AUX_EXTERNALCHARGE
    if(value == 1)
    {
        MetaVariable_Task::capacityStatus[uc_chn] = 1;
        if(MetaVariable_Task::MetaTaskRunCounter < 8)
            MetaVariable_Task::MetaTaskRunCounter += 1;
        MetaVariable_Task::ChargeEnergy[uc_chn] = 0;
        MetaVariable_Task::ChargeCapacity[uc_chn] = 0;
        MetaVariable_Task::DischargeEnergy[uc_chn] = 0;
        MetaVariable_Task::DisChargeCapacity[uc_chn] = 0;
    }
    else
    {
        MetaVariable_Task::capacityStatus[uc_chn] = 0;
        if(MetaVariable_Task::MetaTaskRunCounter > 0)
            MetaVariable_Task::MetaTaskRunCounter -= 1;
        // 					 	ChannelInfo_Manager::ProcessParaSubgroupDataReport(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_Parallel_SubData.m_SubUnitCurrent, PV_ChargeCapacity);

    }
		unsigned char DatabankChan = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[0].m_chn;
    Aux_ExtCharge_601618_Base::Set_ExtCharge_Relay(value, DatabankChan/2);
    // 		    uc_ChnIdx++;
    // // 			}
    //    }
}

// void CANBusParse::Aux_DownloadSdlPara(MP_CAN_MSG *Msg)
// {
//     unsigned char AuxType, AuxChnIdx;
//     unsigned char SDLType;
//     //float fValue;
//     St_SDL_DeltaValue  fSdlPara;
//     AuxType = Msg->Un_Content.m_DownSdl.AuxChnType;   //AUX TYPE´íÎó´¦Àí£¿
//     //AuxChnIdx = Msg->Un_CANID.m_CANID.AuxChn;
//     if(AuxChnIdx > MAX_CH_IN_AUX_UNIT)
//     {
//         Aux_Confirm_Ack(Msg, AUX_CHNO_OVERFLOW);
//         return;
//     }
//     SDLType = Msg->Un_Content.m_DownSdl.ParaType;
//     fSdlPara.fLogDeltaRate = Msg->Un_Content.m_DownSdl.FloatData;
//     if(SDLType == LOG_DELTA_TIME)// 暂时
//     {
//         if(fSdlPara.fLogDeltaRate == 0)
//             fSdlPara.uc_PCFlag = LOG_DELTA_TIME_DEFAULT;//要添加不同辅助类型的默认sdl参数
//         else
//             fSdlPara.uc_PCFlag = LOG_DELTA_TIME;
//         fSdlPara.LogInterval = Msg->Un_Content.m_DownSdl.FloatData;
//         AuxSDL::SDLSetParameterRoutine(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_DownSdl.IV_ChnNo, Msg->Un_Content.m_DownSdl.AuxChnType, Msg->Un_Content.m_DownSdl.AuxChnNo, & fSdlPara);
//         Aux_Confirm_Ack(Msg, NO_ERROR);
//     }
//     else if(SDLType == LOG_DELTA_META)
//     {
//         if(fSdlPara.fLogDeltaRate == 0)
//             fSdlPara.uc_PCFlag = LOG_DELTA_META_DEFAULT;//要添加不同辅助类型的默认sdl参数
//         else
//             fSdlPara.uc_PCFlag = LOG_DELTA_META;
//         fSdlPara.fLogDeltaRate = Msg->Un_Content.m_DownSdl.FloatData;

//         AuxSDL::SDLSetParameterRoutine(Msg->Un_CANID.m_CANID.SourceID, Msg->Un_Content.m_DownSdl.IV_ChnNo, Msg->Un_Content.m_DownSdl.AuxChnType, Msg->Un_Content.m_DownSdl.AuxChnNo, & fSdlPara);
//         Aux_Confirm_Ack(Msg, NO_ERROR);
//     }
//     else
//         //添加错误处理
//         Aux_Confirm_Ack(Msg, NO_ERROR);
// }

void CANBusParse::Aux_AddSafetyCfg(MP_CAN_MSG *Msg)
{
    ST_AUX_HALF_SAFETY Aux_Safety;

    Aux_Safety.m_Source_CANID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
    Aux_Safety.m_IV_index = Msg->Un_Content.m_AddSafety.IV_ChnNo;
    if(Aux_Safety.m_IV_index > MAX_IVCH_PER_IVMCU)
    {
        Aux_Confirm_Ack(Msg, IV_CHNO_OVERFLOW);
        return;
    }
    Aux_Safety.m_aux_type = Msg->Un_Content.m_AddSafety.AuxChnType;
    //if(Aux_Safety.m_aux_type > MAX_AUXCH_PER_BOARD)
    if(Aux_Safety.m_aux_type > MAX_AUX_TYPE)

    {
        Aux_Confirm_Ack(Msg, AUX_TYPE_OVERFLOW);
        return;
    }
    Aux_Safety.m_safe_value = Msg->Un_Content.m_AddSafety.FloatData;
    //Aux_Safety.m_aux_type = Msg->Un_CANID.m_CANID.ChannelType;  和上面重复  //DKQ 12.16.2013
    Aux_Safety.m_High_or_Low = Msg->Un_Content.m_AddSafety.SafetyType;

    Aux_DataManager::Aux_AddSafety (Aux_Safety);
    Aux_Confirm_Ack(Msg, NO_ERROR);

// 	for(unsigned char b_id = 0; b_id<8; b_id++)
// 	{
// 		if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_UART != 0)
// 		{
// 			MTCI_CTRL::SetPointValueCmd(0,SETPOINTVALUE_TEMPERATURE);	//该函数第二个参数无意义
// 			MTCI_CTRL::SetPointValueCmd(1,SETPOINTVALUE_TEMPERATURE);
// 			MTCI_CTRL::SetPointValueCmd(2,SETPOINTVALUE_TEMPERATURE);
// 			MTCI_CTRL::SetPointValueCmd(3,SETPOINTVALUE_TEMPERATURE);

// 		}
// 	}

#if AUX_MULTI_FUNCTION_BOARD == 1
    if((Aux_Safety.m_aux_type == AUX_TEMPERATURE) || (Aux_Safety.m_aux_type == AUX_HUMIDITY)) //改AUX_FLOW_RATE为AUX_HUMIDITY    zyx
    {
        unsigned char auxChnIdx = 0;
        unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
        unsigned char IV_ChnIdx = Msg->Un_Content.m_AddSafety.IV_ChnNo;

        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(Msg->Un_Content.m_AddSafety.AuxChnType);
        if(TempAuxTypeIdex == 0xFF)//辅助限制
            return ;

        unsigned char ActualCount = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap;
        for(unsigned char boardIdx = 4; boardIdx < MAX_BOARD_PER_AUXMCU; boardIdx++)
        {
            if((Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE)   \
                    || (Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType ==  AUX_HUMIDITY))		//改AUX_FLOW_RATE为AUX_HUMIDITY   zyx
            {
                while(auxChnIdx < ActualCount)
                {
                    unsigned char uc_AuxChInUint = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[auxChnIdx].m_Index_InAuxUnit;
                    if((uc_AuxChInUint >= Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit)
                            && (uc_AuxChInUint <= Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_End_InUnit))
                    {
                        if(uc_AuxChInUint == Msg->Un_Content.m_SetCtrl.AuxChnNo)
                        {
                            unsigned char phyChnIdx = uc_AuxChInUint - Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit;
                            ctrl_SetPointValueCmd(phyChnIdx, SETPOINTVALUE_TEMPERATURE);  // 启动温控器 yy 5.6.2015
                        }
                        auxChnIdx++;
                    }
                }
            }
        }
    }
#endif
}

void CANBusParse::Aux_ClearSafetyCfg(MP_CAN_MSG *Msg)
{
    unsigned char AuxChnIdx, IVChnIdx;
    IVChnIdx = Msg->Un_Content.m_ClearSafety.IV_ChnNo;
    unsigned char uc_IVunitID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
    if(IVChnIdx > MAX_IVCH_PER_IVMCU) //只有单个clear才判断IVChnIdx?
    {
        Aux_Confirm_Ack(Msg, IV_CHNO_OVERFLOW);
        return ;
    }

    // 	AuxChnIdx = Msg->Un_CANID.m_CANID.AuxChn;
    // 	if(Msg->Un_Content.m_ClearSafety.All == 0 ||Msg->Un_Content.m_ClearSafety.All == 1)  //无0、1
    // 	{
    // 		if(AuxChnIdx > MAX_AUXCH_PER_BOARD)
    // 		{
    // 			Aux_Confirm_Ack(CANx,Msg,AUX_CHNO_OVERFLOW);
    // 			return;
    // 		}
    Aux_DataManager::Aux_ClearSafeties(uc_IVunitID, IVChnIdx); //此函数待填充
    // 	}
    // 	else
    // 	{
    // 		Aux_Confirm_Ack(CANx,Msg,ALLBIT_ERROR);
    // 		return;
    // 	}
    Aux_Confirm_Ack(Msg, NO_ERROR);

// 	for(unsigned char b_id = 0; b_id<8; b_id++)
// 	{
// 		if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_UART != 0)
// 		{
// 			MTCI_CTRL::ExecStopCmd(0,SETPOINTVALUE_TEMPERATURE);	//该函数第二个参数无意义
// 			MTCI_CTRL::ExecStopCmd(1,SETPOINTVALUE_TEMPERATURE);
// 			MTCI_CTRL::ExecStopCmd(2,SETPOINTVALUE_TEMPERATURE);
// 			MTCI_CTRL::ExecStopCmd(3,SETPOINTVALUE_TEMPERATURE);
// 		}
// 	}
#if AUX_MULTI_FUNCTION_BOARD == 1
    if((Msg->Un_Content.m_ClearSafety.AuxChnType == AUX_TEMPERATURE) || (Msg->Un_Content.m_ClearSafety.AuxChnType == AUX_HUMIDITY)) //AUX_FLOW_RATE改为AUX_HUMIDITY
    {
        unsigned char auxChnIdx = 0;
        unsigned char unitIdx = Aux_DataManager::Aux_LocateIVUnitBufferIndex(Msg->Un_CANID.m_CANID.SourceID);
        unsigned char IV_ChnIdx = Msg->Un_Content.m_ClearSafety.IV_ChnNo;

        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(Msg->Un_Content.m_ClearSafety.AuxChnType);
        if(TempAuxTypeIdex == 0xFF)//辅助限制 牛亚松
            return ;

        unsigned char ActualCount = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap;
        for(unsigned char boardIdx = 4; boardIdx < MAX_BOARD_PER_AUXMCU; boardIdx++)
        {
            if((Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE)   \
                    || (Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType ==  AUX_HUMIDITY)) //AUX_FLOW_RATE改为AUX_HUMIDITY
            {
                while(auxChnIdx < ActualCount)
                {
                    unsigned char uc_AuxChInUint = Aux_DataManager::m_MapFromPC[unitIdx][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[auxChnIdx].m_Index_InAuxUnit;
                    if((uc_AuxChInUint >= Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit)
                            && (uc_AuxChInUint <= Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_End_InUnit))
                    {
                        unsigned char phyChnIdx = uc_AuxChInUint - Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit;
                        ctrl_ExecStopCmd(phyChnIdx, SETPOINTVALUE_TEMPERATURE);  // 停止温控器 yy 5.6.2015
                        auxChnIdx++;
                    }

                }
            }
        }
    }
#endif
}

// void CANBusParse::Aux_TrigLogProc( MP_CAN_MSG *Msg)
// {   //这部分是直接调用Ethernet发送一次数据给PC
//     unsigned char IV_ChnIdx = Msg->Un_Content.m_TrigLog.IV_ChnNo;
//     if(IV_ChnIdx > MAX_IVCH_PER_IVMCU) //只有单个clear才判断IVChnIdx?
//         return ;

//     St_SDL_Point_Pack * st_Point;
//     st_Point = (St_SDL_Point_Pack *) &EthernetParse::m_Point_Temp[0];

//     st_Point->ucType = SDL_DATA_FOR_LOG;
//     for(unsigned char uc_IVMCU = 0; uc_IVMCU < MAX_IVMCU_PER_SYS; uc_IVMCU++)
//     {
//         if(!Aux_DataManager::m_IVMCU_ID_Info[uc_IVMCU].m_Valid || Aux_DataManager::m_IVMCU_ID_Info[uc_IVMCU].m_ID != Msg->Un_CANID.m_CANID.SourceID)
//             continue;
//         for (unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType ++)
//         {
//             unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
//             if(TempAuxTypeIdex == 0xFF)//辅助限制
//                 continue ;

//             unsigned char Chn_Count = Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_ActualCounts;
//             for(unsigned char AuxCh = 0; AuxCh < Chn_Count; AuxCh++)
//             {
//                 if (Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[AuxCh].m_valid == 1)  //如果该辅助类型不存在，则跳过
//                 {   //若存在
//                     ST_AUX_DATA_TRIG DataTrigInfo;
//                     DataTrigInfo.m_IVChnIdx = IV_ChnIdx;
//                     DataTrigInfo.m_IV_unitID = Msg->Un_CANID.m_CANID.SourceID;
//                     DataTrigInfo.m_AuxType = uc_AuxType;
//                     DataTrigInfo.TrigTime = Msg->Un_Content.m_TrigLog.m_Mstime;
//                     DataTrigInfo.m_AuxChnIdx = Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[AuxCh].m_Index_InAuxUnit;
//                     st_Point->u32Second = Aux_DataManager::m_TriggerLogTime[uc_IVMCU][IV_ChnIdx].Second;
//                     st_Point->u16Us100 = Aux_DataManager::m_TriggerLogTime[uc_IVMCU][IV_ChnIdx].Us100;
//                     Aux_DataManager::m_bLogAuxDxdt = Msg->Un_Content.m_TrigLog.bLogDxdt;
//                     Aux_DataManager::Aux_TrigLogPoint(&DataTrigInfo, st_Point);
//                     Aux_DataManager::m_bLogAuxDxdt = 0;
//                 }
//             }
//         }
//         Aux_DataManager::m_TriggerLogTime[uc_IVMCU][IV_ChnIdx] = CommonFunc::GetSystemTime();  //160429
//     }
// }

void CANBusParse::Aux_TrigGroupLogProc_DataPack(MP_CAN_MSG *Msg,unsigned char uc_lastPoint)
{
    unsigned char IV_ChnIdx = Msg->Un_Content.m_TrigLog.IV_ChnNo;
    unsigned char idxCount = 0;
    short         w16_RemainPoint =0;
    if(IV_ChnIdx > MAX_IVCH_PER_IVMCU) //只有单个clear才判断IVChnIdx?
        return ;

    St_SDL_GroupPoints* st_gPoint;
    st_gPoint = (St_SDL_GroupPoints*)&EthernetParse::m_Point_Temp[0];

    idxCount = 0;
    st_gPoint = (St_SDL_GroupPoints*)&EthernetParse::m_Point_Temp[0];
    memset(st_gPoint,0,sizeof(St_SDL_GroupPoints));
    st_gPoint->ucType = SDL_DATA_FOR_LOG;
    st_gPoint->u16Us100 = Msg->Un_Content.m_TrigLog.m_Mstime.Us100;
    st_gPoint->u32Second = Msg->Un_Content.m_TrigLog.m_Mstime.Second;

    st_gPoint->bIV = 0;
    st_gPoint->IvChnIndex = Msg->Un_Content.m_TrigLog.IV_ChnNo;
    st_gPoint->IvUnitIP = Msg->Un_CANID.m_CANID.SourceID;
    st_gPoint->itemCount = 0;
    st_gPoint->lastPoint = uc_lastPoint;
    unsigned char uc_updata_New = 0;
    for(unsigned char uc_IVMCU = 0; uc_IVMCU < MAX_IVMCU_PER_SYS; uc_IVMCU++)
    {
        if(!Aux_DataManager::m_IVMCU_ID_Info[uc_IVMCU].m_Valid || Aux_DataManager::m_IVMCU_ID_Info[uc_IVMCU].m_ID != Msg->Un_CANID.m_CANID.SourceID)
            continue;
        for (unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType ++)
        {
            unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
            if(TempAuxTypeIdex == 0xFF)//辅助限制
                continue ;

            unsigned char Chn_Count = Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap;
            if(Chn_Count > MAX_CH_IN_AUX_UNIT)
                continue;

            //idxCount *= 2;
            for(unsigned char AuxCh = 0; AuxCh < Chn_Count; AuxCh++)
            {
                w16_RemainPoint=MAX_AUX_TRIGGROUPLOG_POINT - st_gPoint->itemCount;
                switch(uc_AuxType)  // 20210312  yy
                {
                case AUX_DI:
                case AUX_DO:
                case AUX_AO:
                case AUX_HUMIDITY:
                case AUX_FLOW_RATE:
                case AUX_EXTERNALCHARGE:
                    if(w16_RemainPoint <= 0)
                        w16_RemainPoint = MAX_AUX_TRIGGROUPLOG_POINT;
                    break;
                default:
                    if(w16_RemainPoint < 2)
                        w16_RemainPoint = MAX_AUX_TRIGGROUPLOG_POINT;
                    break;
                }
                if((w16_RemainPoint >= MAX_AUX_TRIGGROUPLOG_POINT) && (st_gPoint->itemCount > 0)) // 20210312  yy  // St_SDL_OnePoint points[32]最大32个点，奇数时提前发避免同种类型同个通道数据分拆两包
                {
                    EthernetParse::MPReportData(IV_ChnIdx, SEND_AUX_GROUPLOGDATA);
                    uc_updata_New = 1;
                }
                if(uc_updata_New)
                {
                    idxCount = 0;
                    st_gPoint = (St_SDL_GroupPoints*)&EthernetParse::m_Point_Temp[0];
                    memset(st_gPoint,0,sizeof(St_SDL_GroupPoints));
                    st_gPoint->ucType = SDL_DATA_FOR_LOG;
                    st_gPoint->u16Us100 = Msg->Un_Content.m_TrigLog.m_Mstime.Us100;
                    st_gPoint->u32Second = Msg->Un_Content.m_TrigLog.m_Mstime.Second;

                    st_gPoint->bIV = 0;
                    st_gPoint->IvChnIndex = Msg->Un_Content.m_TrigLog.IV_ChnNo;
                    st_gPoint->IvUnitIP = Msg->Un_CANID.m_CANID.SourceID;
                    st_gPoint->itemCount = 0;
                    st_gPoint->lastPoint = uc_lastPoint;
                    uc_updata_New = 0;
                }

                if (Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[AuxCh].m_valid == 1)  //如果该辅助类型不存在，则跳过
                {   //若存在
                    ST_AUX_DATA_TRIG DataTrigInfo;
                    DataTrigInfo.m_IVChnIdx = IV_ChnIdx;
                    DataTrigInfo.m_IV_unitID = Msg->Un_CANID.m_CANID.SourceID;
                    DataTrigInfo.m_AuxType = uc_AuxType;
                    DataTrigInfo.TrigTime = Msg->Un_Content.m_TrigLog.m_Mstime;
                    DataTrigInfo.m_AuxChnIdx = Aux_DataManager::m_MapFromPC[uc_IVMCU][IV_ChnIdx][TempAuxTypeIdex].m_MapInfo[AuxCh].m_Index_InAuxUnit;
                    Aux_DataManager::m_bLogAuxDxdt = 1;
                    Aux_DataManager::Aux_TrigLogGroupPoint(&DataTrigInfo, &st_gPoint->points[idxCount]);
                    idxCount += 1;
                    switch(uc_AuxType)  // 20210312  yy
                    {
                    case AUX_DI:
                    case AUX_DO:
                    case AUX_AO:
                    case AUX_HUMIDITY:
                    case AUX_FLOW_RATE:
                    case AUX_EXTERNALCHARGE:
                        st_gPoint->itemCount += 1;
                        break;
                    default:
                        Aux_DataManager::Aux_TrigLogGroupPoint_dxdt(&DataTrigInfo, &st_gPoint->points[idxCount]);
                        idxCount += 1;
                        st_gPoint->itemCount += 2;

                        break;
                    }
                    Aux_DataManager::m_bLogAuxDxdt = 0;
                }
            }
        }
        Aux_DataManager::m_TriggerLogTime[uc_IVMCU][IV_ChnIdx] = CommonFunc::GetSystemTime();  //160429

    }
    if(st_gPoint->itemCount > 0)
        EthernetParse::MPReportData(IV_ChnIdx, SEND_AUX_GROUPLOGDATA);
}
void CANBusParse::Aux_TrigGroupLogProc(MP_CAN_MSG *Msg)
{
    CANBusParse::Aux_TrigGroupLogProc_DataPack(Msg,0);
}

void CANBusParse::Aux_TrigGroupLogProcLast(MP_CAN_MSG *Msg)
{
    CANBusParse::Aux_TrigGroupLogProc_DataPack(Msg,1);
}


//数据上传处理（For IV Limit Check）
void CANBusParse::Aux_RequestDataProc( MP_CAN_MSG *Msg)
{
    ST_AUX_DATA_REQUEST st_data_request;
    st_data_request.m_Source_CANID = Msg->Un_CANID.m_CANID.SourceID;
    st_data_request.m_aux_chn_index =  Msg->Un_Content.m_ReqstData.AuxChnNo;
    st_data_request.m_aux_type = Msg->Un_Content.m_ReqstData.AuxChnType;
    st_data_request.m_IV_index = Msg->Un_Content.m_ReqstData.IV_ChnNo;
    st_data_request.m_data_type = Msg->Un_Content.m_ReqstData.data_type;
    st_data_request.m_single_or_repeat = Msg->Un_Content.m_ReqstData.single_or_repeat;
    st_data_request.m_TimeDelta = Msg->Un_Content.m_ReqstData.TimeDelta;
    st_data_request.m_MetaDelta = Msg->Un_Content.m_ReqstData.MetaDelta;
    Aux_DataManager:: Aux_AddDataRequest( st_data_request);
}

void CANBusParse::Aux_ClearRequestData(MP_CAN_MSG *Msg)
{
    unsigned char IVChnIdx;
    IVChnIdx = Msg->Un_Content.m_Clear_RequestData.IV_ChnNo;
    unsigned char uc_IVunitID = Msg->Un_CANID.m_CANID.SourceID; // Add by DKQ on 03.11.2014
    Aux_DataManager::Aux_ClearDataRequests(uc_IVunitID, IVChnIdx);
}
// void CANBusParse::Aux_Sync_Time(MsTime SyncTime, unsigned char SyncStatus)
// {
//     if(OS_ServeProcess::TimeCali)
//         return;

//      int Second, Us100;
// //     Second = OS_ServeProcess::OS_Time.Second;
// //     Us100 = OS_ServeProcess::OS_Time.Us100;

//     Second = SysTime.Second - SyncTime.Second;
//     Us100 = SysTime.Us100 - SyncTime.Us100;
//     Second *= 10000;
//     Us100 += Second;

//     if((Us100 < TIME_PRECISION) && (Us100 > - TIME_PRECISION))
//         return;

//     OS_ServeProcess::TimeCaliTickCount = 10000 - SyncTime.Us100;
//     OS_ServeProcess::TimeCaliSecond = SyncTime.Second + 1;
//     OS_ServeProcess::TimeCaliTicks = 0;
//     OS_ServeProcess::TimeCali = 1;
//     OS_ServeProcess::CAN_TimeCalied = 1;
//     return;
// Aux_Confirm_Ack(Msg,NO_ERROR);
// }


void CANBusParse::Aux_UpdateStatus(MP_CAN_MSG *Msg)
{
    //IV_AskStatusFlag = 1;
    //StatusAck.Un_CANID.m_CANID.DestID =  Msg->Un_CANID.m_CANID.SourceID;
    unsigned char IV_ChnIdx = Msg->Un_Content.m_AskStatus.IV_ChnNo;
    //此为触发状态上传   激活IV ID
    if(IV_ChnIdx < MAXCHANNELNO)
    {
        //st_AuxRptInfoMgr[IV_ChnIdx].m_bIVAskFlag = Msg->Un_Content.m_AskStatus.IV_AskFlag;

        //Aux_DataManager::m_AuxLimitCount[IV_ChnIdx] = Msg->Un_Content.m_AskStatus.LimitCount;
        // Aux_DataManager::m_AuxSafetyCount[IV_ChnIdx] = Msg->Un_Content.m_AskStatus.SafetyCount;
    }


}
//--------------------end of Aux Parse part ------------------------//

/************************************************************************/
/*  Aux Send Command  Part                                              */
/************************************************************************/
void CANBusParse::Aux_RptLimitStatus(ST_AUX_LIMIT *Limit, float fValue)
{
    MP_CAN_MSG Msg;
    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //辅助unit都以CAN0 来通讯。
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_LIMIT_STATUS;

    Msg.Un_Content.m_LimitRpt.IV_ChnNo = Limit->m_IV_index;
    Msg.Un_Content.m_LimitRpt.SN     = Limit->m_limit_SN;

    Msg.Un_Content.m_LimitRpt.Reached = Limit->m_limit_Reached;
    //Msg.Un_Content.m_LimitRpt.T2F = !Limit->m_limit_Reached;
    Msg.Un_Content.m_LimitRpt.FloatData = fValue;

    Msg.Un_Content.m_LimitRpt.AuxChnNo = Limit->m_chn_index;
    Msg.Un_Content.m_LimitRpt.AuxChnType = Limit->m_aux_type;
    //Msg.Un_CANID.m_CANID.DestID = 1;
    //Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //add DKQ 03.11.2014  ÒÔ±¾¸¨ÖúunitµÄ ²¦ÂëÎªÔ´CNID
    Msg.Un_CANID.m_CANID.DestID = Limit->m_Source_CANID;

    //send can cmd
    Send_CAN_Cmd(&Msg);
}
void CANBusParse::Aux_RptAnyLimitStatus(ST_AUX_IFANY_LIMIT *Limit, float fValue)
{
    MP_CAN_MSG Msg;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_LIMIT_STATUS;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_Content.m_LimitRpt.IV_ChnNo = Limit->m_IV_index;
    Msg.Un_Content.m_LimitRpt.SN     = Limit->m_limit_SN;

    Msg.Un_Content.m_LimitRpt.Reached = Limit->m_bReached;
    //Msg.Un_Content.m_LimitRpt.T2F = !Limit->m_limit_Reached;
    Msg.Un_Content.m_LimitRpt.FloatData = fValue;

    Msg.Un_Content.m_LimitRpt.AuxChnNo = 0;
    Msg.Un_Content.m_LimitRpt.AuxChnType = Limit->m_aux_type;

    //	Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //注释掉。统一在send_can_cmd 里加了。
    //add DKQ 03.11.2014  以本辅助unit的 拨码为源CNID
    Msg.Un_CANID.m_CANID.DestID = Limit->m_Source_CANID;
    //	Msg.Un_CANID.m_CANID.DestID = 1;

    //send can cmd
    Send_CAN_Cmd(&Msg);
}
//初始化完毕后调用
// void CANBusParse::Aux_RequestTime()
// {
// 	MP_CAN_MSG Msg;
// 	Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;
// 	Msg.Un_CANID.m_U32ID = CANFunc::FilterBrdCstID;   //广播包（因为不知道IV MCU CANID 若辅助接收到广播则return )
// 	Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REQUEST_TIME;
// 	OS_ServeProcess::CAN_TimeCalied = 0;
// 	Send_CAN_Cmd(m_CANType,&Msg);

// }
/*void CANBusParse::ReportAnyLimitStatus(ST_AUX_ANY_LIMIT *Limit,float fValue)
{
MP_CAN_MSG Msg;

Msg.Un_Content.m_LimitRpt.Cmd = CAN_CMD_REPORT_LIMIT_STATUS;

Msg.Un_Content.m_LimitRpt.IV_ChnNo = Limit->IV_index;
Msg.Un_Content.m_LimitRpt.SN     = Limit->limit_SN;

Msg.Un_Content.m_LimitRpt.F2T = Limit->bReached;
Msg.Un_Content.m_LimitRpt.T2F = !Limit->bReached;
Msg.Un_Content.m_LimitRpt.FloatData = fValue;

Msg.Un_CANID.m_CANID.AuxChn = 0xFF;
Msg.Un_CANID.m_CANID.ChannelType = Limit->aux_type;

//send can cmd
Send_CAN_Cmd(&Msg);
}*/

void CANBusParse::Aux_RptUnsafe(unsigned char IVunitID, unsigned char IVChn, unsigned char AuxChn, unsigned char AuxType, unsigned char UnsafeStatus, float f_Value) //参数待确定
{
    MP_CAN_MSG Msg;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_UNSAFE;
    Msg.Un_Content.m_Unsafe.AuxChnNo = AuxChn;
    Msg.Un_Content.m_Unsafe.AuxChnType = AuxType;          //´ý¼ÓÈë
    Msg.Un_Content.m_Unsafe.IV_ChnNo =  IVChn;
    Msg.Un_Content.m_Unsafe.FloatData = f_Value;
    Msg.Un_Content.m_Unsafe.OverrunStatus = UnsafeStatus;
    Msg.Un_CANID.m_CANID.DestID = IVunitID;
    //Msg.Un_Content.m_Unsafe.IV_SN.SN_Bit.IV_Chn = IV_No;       //´ý¼ÓÈë
    Send_CAN_Cmd(&Msg);
}

//上报MTC温度unsafe
void CANBusParse::Aux_RptMTCTempUnsafe(unsigned char IVunitID,unsigned char IVChn,unsigned char AuxChn,unsigned char AuxType,float f_Value, unsigned short SafetyGotoTag) //参数待确定
{
    MP_CAN_MSG Msg;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_MTC_TEMP_UNSAFE;
    Msg.Un_Content.m_Unsafe.AuxChnNo = AuxChn;
    Msg.Un_Content.m_Unsafe.AuxChnType = AuxType;
    Msg.Un_Content.m_Unsafe.IV_ChnNo =  IVChn;
    Msg.Un_Content.m_Unsafe.FloatData = f_Value;
    Msg.Un_Content.m_Unsafe.OverrunStatus = SafetyGotoTag;
    Msg.Un_CANID.m_CANID.DestID = IVunitID;
    //Msg.Un_Content.m_Unsafe.IV_SN.SN_Bit.IV_Chn = IV_No;
    Send_CAN_Cmd(&Msg);
}

//上报MTC Fan Fault unsafe
void CANBusParse::Aux_RptMTCFanUnsafe(unsigned char IVunitID,unsigned char IVChn,unsigned char AuxChn,unsigned char AuxType)
{
    MP_CAN_MSG Msg;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_MTC_FAN_UNSAFE;
    Msg.Un_Content.m_Unsafe.AuxChnNo = AuxChn;
    Msg.Un_Content.m_Unsafe.AuxChnType = AuxType;
    Msg.Un_Content.m_Unsafe.IV_ChnNo =  IVChn;
    Msg.Un_CANID.m_CANID.DestID = IVunitID;
    //Msg.Un_Content.m_Unsafe.IV_SN.SN_Bit.IV_Chn = IV_No;
    Send_CAN_Cmd(&Msg);
}


//Aux MCU上报Request Data



void CANBusParse::Aux_RptData(ST_AUX_DATAPOINT Aux_DataPoint)
{
    MP_CAN_MSG Data_Msg;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Data_Msg.Un_Content.m_DataAck.AuxChnNo = Aux_DataPoint.AuxChn;
    Data_Msg.Un_Content.m_DataAck.AuxChnType = Aux_DataPoint.Auxtype;
    Data_Msg.Un_CANID.m_CANID.DestID = Aux_DataPoint.DestID;
//     Data_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_LOW;//×îµÍÓÅÏÈ¼¶

    Data_Msg.Un_Content.m_DataAck.IV_ChnNo = Aux_DataPoint.ChnNo;
    Data_Msg.Un_Content.m_DataAck.SN = 0;
    Data_Msg.Un_Content.m_DataAck.AckStatus = Aux_DataPoint.AckStatus;   //??
    Data_Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Data_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REQUEST_DATA_ACK;
    Data_Msg.Un_Content.m_DataAck.DataType = Aux_DataPoint.DataType;  //Ô­Ê¼Êý¾Ý£¿dxdt...?
    Data_Msg.Un_Content.m_DataAck.FloatData = Aux_DataPoint.f_Value;

    Send_CAN_Cmd(&Data_Msg);

}

void CANBusParse::Aux_CANStatusRpt()
{
#if(USE_AUX_STATUS_REPORT == 1)
    MP_CAN_MSG StatusRpt;
    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;//CAN1 as Aux MCU's CAN Port
    //StatusRpt.Un_Content.m_AuxStatus.AuxInfo = 0;
    unsigned long long CurrentAuxSum = 0;
    unsigned char bDiscard = false;
    for(unsigned char mcu_idx = 0; mcu_idx < MAX_IVMCU_PER_SYS; mcu_idx++)
    {
        if(!Aux_DataManager::m_IVMCU_ID_Info[mcu_idx].m_Valid)
            continue;
        //inter unit 100ms interval
        if(CommonFunc::GetTimeTicks( &st_AuxRptInfoMgr[mcu_idx].m_RptTick) > AUX_REPROT_INFO_TICK)
        {
            CommonFunc::GetTimeTicksUpdate(&st_AuxRptInfoMgr[mcu_idx].m_RptTick);	 // reset counter
        }
        else if(st_AuxRptInfoMgr[mcu_idx].m_bIVAskFlg)
        {
            st_AuxRptInfoMgr[mcu_idx].m_bIVAskFlg = 0;
        }
        else
            continue;
        StatusRpt.Un_Content.m_AuxStatus.AuxInfo = 0;

        for( unsigned char chn_idx = 0; chn_idx < MAX_IVCH_PER_IVMCU; chn_idx++) //根据alive exist判断？
        {
            CurrentAuxSum =  Aux_DataManager::m_AuxLimitCount[mcu_idx][chn_idx] + Aux_DataManager::m_AuxIfAnyLimitCount[mcu_idx][chn_idx]
                             + Aux_DataManager::m_AuxSafetyCount[mcu_idx][chn_idx];
            StatusRpt.Un_Content.m_AuxStatus.AuxInfo |= (CurrentAuxSum << (AuxInfoShiftBit[chn_idx])); //移位6bit数据
            if((chn_idx + 1) % 8 == 0) // 7 15
            {
                StatusRpt.Un_CANID.m_CANID.DestID = Aux_DataManager::m_IVMCU_ID_Info[mcu_idx].m_ID;
                StatusRpt.Un_CANID.m_CANID.Cmd =  CAN_CMD_STATUS_RPT;
                StatusRpt.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
                StatusRpt.Un_Content.m_AuxStatus.startIVIdx = chn_idx - 7; //8
                Send_CAN_Cmd(&StatusRpt);   //每隔100ms发送一次每次只发送
                st_AuxRptInfoMgr[mcu_idx].m_UnRptCnt[chn_idx] = 0;
                StatusRpt.Un_Content.m_AuxStatus.AuxInfo = 0;
            }
        }	//	return;
    }	//end for
#endif
}
void CANBusParse::Aux_RptMetaData(unsigned char AuxChn, unsigned char MV_Type, float f_Value)
{
    MP_CAN_MSG Msg;
    unsigned char LongSnakeChn, uc_IVchn;
    unsigned char IVunitID;
    unsigned short uw16_PointID;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REPORT_MV;
    Msg.Un_Content.m_MetaVariable.AuxChnNo = AuxChn;
    Msg.Un_Content.m_MetaVariable.AuxChnType = AUX_EXTERNALCHARGE;
    for(unsigned short i = 0; i < MAX_MAP_COUNT_PER_UNIT; i++)  // 128
    {
        if(Aux_DataManager::m_DownLoadMapFromPC[i].m_AuxType == AUX_EXTERNALCHARGE)
        {
            if(Aux_DataManager::m_DownLoadMapFromPC[i].m_Index_InAuxUnit == AuxChn)
            {
                uw16_PointID = i;
                break;
            }
            else
                continue;
        }
        else
            continue;
    }
    // 	 LongSnakeChn = Aux_DataManager::m_AuxChnLookupTable[AUX_EXTERNALCHARGE].m_StartInSnake + AuxChn;
    uc_IVchn = Aux_DataManager::m_DownLoadMapFromPC[uw16_PointID].m_IVCh;
    IVunitID = Aux_DataManager::m_DownLoadMapFromPC[uw16_PointID].m_IVUnit;
    Msg.Un_Content.m_MetaVariable.IV_ChnNo =  uc_IVchn;
    Msg.Un_Content.m_MetaVariable.MV_Type = MV_Type ;
    Msg.Un_Content.m_MetaVariable.FloatData = f_Value;
    Msg.Un_CANID.m_CANID.DestID = IVunitID;
    Send_CAN_Cmd(&Msg);
}
#endif

/************************************************************************/
/**                     IV MCU Send Command Part                       **/
/************************************************************************/
//#if((USE_AS_AUXMCU == 0) &&(SUPPORT_AUX == 1)) //IV
#if(USE_AS_AUXMCU == 0)
int  CANBusParse::IV_SendSchMapToAUX(unsigned char uc_channelNo, unsigned char Auxtype)
{   //add by dirui 20131204
    MP_CAN_MSG m_can_msg;
    unsigned char AuxUnitCount = 0;
    unsigned char SchMapAuxUnit[MAXAUXUNIT] = {0};
    unsigned char SchMapAuxUnitCan[MAXAUXUNIT] = {0};
    unsigned char SchMapAuxTotalChan[MAXAUXUNIT] = {0};
//    unsigned short SN;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return 0;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[Auxtype];
    for(int AuxChn = 0; AuxChn < Channel_MapAuxInfo::IVChanMapAuxCount[uc_channelNo][Auxtype]; AuxChn++) //Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[Auxtype]
    {
        int AuxUnitIndex = 0;
        for(; AuxUnitIndex < AuxUnitCount; AuxUnitIndex++)
        {
            if(Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxChn].m_Aux_UnitID == SchMapAuxUnit[AuxUnitIndex])
            {
                SchMapAuxTotalChan[AuxUnitIndex]++;
                break;
            }
        }
        if(AuxUnitIndex >= AuxUnitCount)
        {
            SchMapAuxUnit[AuxUnitCount] = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxChn].m_Aux_UnitID;
            SchMapAuxUnitCan[AuxUnitCount] = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxChn].m_CAN_Port;
            SchMapAuxTotalChan[AuxUnitIndex]++;
            AuxUnitCount++;
        }
    }
    for(int AuxUnitIndex = 0; AuxUnitIndex < AuxUnitCount; AuxUnitIndex++)
    {
        m_can_msg.Un_CANID.m_CANID.DestID = SchMapAuxUnit[AuxUnitIndex];
        m_CANType = SchMapAuxUnitCan[AuxUnitIndex]; //要对于IV配置的各种Unit情况分别处理
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:   // 内部CAN网处理
        case CAN_PORT_USE_PUBLIC:
            if(m_can_msg.Un_CANID.m_CANID.DestID >= MAXAUXUNIT)
                return 0;
            m_can_msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            m_can_msg.Un_CANID.m_CANID.Cmd = CAN_CMD_DOWNLOAD_SCHMAP;
            m_can_msg.Un_Content.m_DownSchMap.AuxChnType = Auxtype;
            m_can_msg.Un_Content.m_DownSchMap.IV_ChnNo = uc_channelNo;
            m_can_msg.Un_Content.m_DownSchMap.AuxCount = SchMapAuxTotalChan[AuxUnitIndex];
            Send_CAN_Cmd(&m_can_msg);
            break;
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return 0;
        }
    }
    return 1;
}
int  CANBusParse::IV_SendSafetyInfoToAUX(unsigned char uc_channelNo, unsigned char Auxtype, float fHigh, float fLow, unsigned char uc_AuxSafe_Type)
{   //add by dirui 20131204
    MP_CAN_MSG m_can_msg;
    unsigned short SN;
    unsigned char  m_CANType = CAN_PORT_USE_PUBLIC;
    for(unsigned char uc_i = 0; uc_i < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit; uc_i++)
    {
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            continue;
        m_can_msg.Un_CANID.m_CANID.DestID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_i];
        m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitCANPort[uc_i]; //要对于IV配置的各种Unit情况分别处理
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:   // 内部CAN网处理
        case CAN_PORT_USE_PUBLIC:
            if(m_can_msg.Un_CANID.m_CANID.DestID >= MAXAUXUNIT)
                return 0;
            Channel_MapAuxInfo::CurrentSafetyCount[uc_channelNo][m_can_msg.Un_CANID.m_CANID.DestID]++;
            m_can_msg.Un_Content.m_AddSafety.AuxChnType = Auxtype;
            m_can_msg.Un_Content.m_AddSafety.IV_ChnNo = uc_channelNo;
            m_can_msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            m_can_msg.Un_CANID.m_CANID.Cmd = CAN_CMD_DOWNLOAD_SAFETY;
            switch(uc_AuxSafe_Type)
            {
            default:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYLOW; // 普通电压、温度、压力、湿度通道低值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYVoltageLow; // 监控板电压通道低值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyCurrent:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYCurrentLow; // 监控板电流通道低值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyTemperature:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYTemperatureLow; // 监控板内部温度通道低值
                break;
// 					default:
// 						  m_can_msg.Un_Content.m_AddSafety.SafetyType=SAFETYLOW;
// 					    break;
            }
            m_can_msg.Un_Content.m_AddSafety.FloatData = fLow;
            SN = Send_CAN_Cmd(&m_can_msg);  // 送出低值
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNInfo[st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount].m_SN = SN; //dirui add 20131220
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNInfo[st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount].m_AuxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_i];
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount++;
            //  CANBusParse::SafetySN_ResTable[uc_channelNo][SN]=1;  //dirui add for test 20131223
            switch(uc_AuxSafe_Type)
            {
            default:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYHIGH; // 普通电压、温度、压力、湿度通道高值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYVoltageHigh; // 监控板电压通道高值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyCurrent:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYCurrentHigh; // 监控板电流通道高值
                break;
            case MP_SCH_SAFTY_TYPE_AuxiSafetyTemperature:
                m_can_msg.Un_Content.m_AddSafety.SafetyType = SAFETYTemperatureHigh; // 监控板内部温度通道高值
                break;
// 					default:
// 						  m_can_msg.Un_Content.m_AddSafety.SafetyType=SAFETYHIGH;
// 					    break;
            }
            m_can_msg.Un_Content.m_AddSafety.FloatData = fHigh;
            SN = Send_CAN_Cmd(&m_can_msg);	  // 送出高值
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNInfo[st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount].m_SN = SN; //dirui add 20131220
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNInfo[st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount].m_AuxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_i];
            st_SNCheckTab[uc_channelNo][SAFETYSN].m_SNCount++;
            //  CANBusParse::SafetySN_ResTable[uc_channelNo][SN]=1;  //dirui add for test 20131223

            break;
        //case CAN_PORT_USE_PUBLIC:
        //  break;
        case CAN_PORT_USE_BUILD_IN:           //IV专属辅助通道的处理
            Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safe_value[0] = fLow;
            Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safe_value[1] = fHigh;
            Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safety_alive[0] = 1;
            Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safety_alive[1] = 1;

            break;
        default:
            return 0;
        }
    }
    return 1;
}

int	 CANBusParse::IV_ClearAuxSafetyInfo(unsigned char uc_channelNo, unsigned char Auxtype, unsigned char VirtualID, unsigned char Type)
{
    unsigned char uc_AUXCANID;
    MP_CAN_MSG  m_can_msg;
    unsigned char m_CANType;

    memset((void*)&Channel_MapAuxInfo::CurrentSafetyCount[uc_channelNo][0], 0, sizeof(unsigned char)*MAXUNITID);
    switch(Type)
    {
    case ALLMAPCHANNEL:  //向所有辅助发出清除命令
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            return 0;
        for(unsigned char uc_i = 0; uc_i < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit; uc_i++)
        {
            uc_AUXCANID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitID[uc_i];
            m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitCANPort[uc_i];

            switch(m_CANType)
            {
            case CAN_PORT_USE_EXCLUSIVE:
            case CAN_PORT_USE_PUBLIC:
                m_can_msg.Un_Content.m_ClearSafety.IV_ChnNo = uc_channelNo;    //需要加上
                m_can_msg.Un_Content.m_ClearSafety.All = ALLMAPCHANNEL;
                //m_can_msg.Un_Content.m_ClearSafety.
                m_can_msg.Un_Content.m_ClearSafety.AuxChnType = Auxtype;
                m_can_msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
                m_can_msg.Un_CANID.m_CANID.Cmd = CAN_CMD_CLEAR_SAFETY;
                m_can_msg.Un_CANID.m_CANID.DestID = uc_AUXCANID;
                Send_CAN_Cmd(&m_can_msg);
                break;
            //	case CAN_PORT_USE_PUBLIC:
            //		break;
            case CAN_PORT_USE_BUILD_IN://暂时设计IV专属辅助为全部清除模式
                Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safe_value[0] = 0;
                Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safe_value[1] = 0;
                Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safety_alive[0] = 0;
                Channel_MapAuxInfo::m_IV4AuxSafetyBank[uc_channelNo][Auxtype].m_safety_alive[1] = 0;
                break;
            default:
                return 0;
            }
        }
        return 1;
    case SINGLECHANNEL:
        unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[Auxtype];
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            return 0;
        uc_AUXCANID = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+VirtualID].m_Aux_UnitID;
        m_CANType = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+VirtualID].m_CAN_Port;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            m_can_msg.Un_Content.m_ClearSafety.All = SINGLECHANNEL;
            m_can_msg.Un_CANID.m_CANID.DestID = uc_AUXCANID;
            //m_can_msg.Un_Content.m_ClearSafety.Rsvd[0]=Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[Type][VirtualID].m_Map.m_Index_InAuxUnit;
            m_can_msg.Un_Content.m_ClearSafety.AuxChnType = Auxtype;
            m_can_msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            m_can_msg.Un_CANID.m_CANID.Cmd = CAN_CMD_CLEAR_SAFETY;
            Send_CAN_Cmd(&m_can_msg);
            return 1;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            return 1;
        default:
            return 0;
        }
    default:
        break;
    }


    return 0;
}

// int  CANBusParse::IV_DownloadSDLToAUX(unsigned char uc_channelNo, unsigned char Auxtype, unsigned char Type, float SDLFACTOR, unsigned char DestId, unsigned char UnitId)
// {
//     MP_CAN_MSG MyMsg;
//     unsigned char m_CANType;
//     if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
//         return 0;
//     m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][Auxtype].m_AuxUnitCfg.m_Aux_UnitCANPort[UnitId];
//     switch(m_CANType)
//     {
//     case CAN_PORT_USE_EXCLUSIVE:
//     case CAN_PORT_USE_PUBLIC:
//         MyMsg.Un_Content.m_DownSdl.AuxChnNo = 0; //不需要
//         MyMsg.Un_Content.m_DownSdl.AuxChnType = Auxtype ;
//         MyMsg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
//         MyMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_DOWNLOAD_SDL;
//         MyMsg.Un_Content.m_DownSdl.FloatData = SDLFACTOR ;
//         MyMsg.Un_Content.m_DownSdl.IV_ChnNo = uc_channelNo;
//         MyMsg.Un_Content.m_DownSdl.ParaType = Type ; //dxdt暂时不考虑
//         MyMsg.Un_CANID.m_CANID.DestID = DestId;

//         Send_CAN_Cmd(&MyMsg);
//         return 1;
//     //	case CAN_PORT_USE_PUBLIC:
//     //		break;
//     case CAN_PORT_USE_BUILD_IN:
//         if((Auxtype >= MAXAUXTYPE) || (uc_channelNo >= MAXCHANNELNO))
//             return 0;
//         Channel_MapAuxInfo::m_IV4AuxSDLBank[uc_channelNo][Auxtype].m_log_interval = SDLFACTOR;
//         return 1;
//     default:
//         return 0;
//     }
// }
void  CANBusParse::IV_DownloadLimitToAUX(unsigned char uc_channelNo, unsigned short u16_StepID, unsigned char LimitIdx) //,unsigned char VirtualID
{   //add by hepeiqing ,20131128
    //static MP_CAN_MSG MyMsg;
    MP_CAN_MSG MyMsg;
    unsigned char m_CANType;
    St_ChannelStep *st_cstep = &ChannelInfo_Manager::ChannelStep[uc_channelNo];
    unsigned char ActiveIdx = st_cstep->m_ucActiveIdx;
    unsigned char DataType;
    unsigned short VirtualIdx;
    int SN;
    int * sn;
    St_AUXchannelInfo MyInfo;
    St_Limit *MyLimit = &st_cstep->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx];

    MyMsg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    MyMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_DOWNLOAD_LIMITS;

    if(MyLimit->m_ucLeftValue_DataType != MP_DATA_TYPE_FIGURE)
    {
        DataType = MyLimit->m_ucLeftValue_DataType;
        VirtualIdx = MyLimit->m_fLeftValue;
        MyMsg.Un_Content.m_DownLimit.FloatData = MyLimit->m_fRightValue;
        MyMsg.Un_Content.m_DownLimit.Sign = Channel_MapAuxInfo::AuxLimit_GetComPareSign(MyLimit->m_ucCompareSign, uc_channelNo, LimitIdx);
    }
    else
    {
        DataType = MyLimit->m_ucRightValue_DataType;
        VirtualIdx = MyLimit->m_fRightValue;
        MyMsg.Un_Content.m_DownLimit.FloatData = MyLimit->m_fLeftValue;
        MyMsg.Un_Content.m_DownLimit.Sign = Channel_MapAuxInfo::AuxLimit_GetComPareSign(MyLimit->m_ucCompareSign, uc_channelNo, LimitIdx);
    }
    if((MyMsg.Un_Content.m_DownLimit.Sign == CAN_COMPARE_SIGN_EQUAL) && (DataType != MP_DATA_TYPE_AUX_DI) && (DataType != MP_DATA_TYPE_AUX_DO))
    {   //add by hepeiqing,170314,for DI suport equal sign
        MyMsg.Un_Content.m_DownLimit.Sign = CAN_COMPARE_SIGN_USELESS;
        ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_AnyLimitReachedCount = 1;
        ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_bChecked = 1;
        ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_bReached = 1;
    }

    unsigned char DT;
    unsigned short D1 = 0, D2 = 0;
    switch(DataType)
    {
    case MP_DATA_TYPE_AUX_T:
        DT = AUX_TEMPERATURE;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dTdt:
        DT = AUX_TEMPERATURE;
        D1 = 1;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_V:
        DT = AUX_VOLTAGE;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dVdt:
        DT = AUX_VOLTAGE;
        D1 = 1;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_P:
        DT = AUX_PRESSURE;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dPdt:
        DT = AUX_PRESSURE;
        D1 = 1;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_FlowRate:
        DT = AUX_FLOW_RATE;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dFRdt:
        DT = AUX_FLOW_RATE;
        D1 = 1;
// 		D2 = 0;
        break;

    case MP_DATA_TYPE_AUX_Humidity: //zyx
        DT = AUX_HUMIDITY;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dHumdt:
        DT = AUX_HUMIDITY;
        D1 = 1;
// 		D2 = 0;
        break;

    case MP_DATA_TYPE_AUX_Concentration:
        //DT = AUX_DENSITY;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dConcentrationdt:
        //DT = AUX_DENSITY;
        D1 = 1;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_DI:
        DT = AUX_DI;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_DO:
        DT = AUX_DO;
// 		D1 = 0;
// 		D2 = 0;
        break;
    case MP_DATA_TYPE_EC:
        DT = AUX_EXTERNALCHARGE;
// 		D1 = 0;
// 		D2 = 0;
        break;
    default:
        break;
    }

    MyMsg.Un_Content.m_DownLimit.AuxChnType = DT;
    MyMsg.Un_Content.m_DownLimit.Dt = D1;
    MyMsg.Un_Content.m_DownLimit.D2t = D2;

    unsigned char AuxType = MyMsg.Un_Content.m_DownLimit.AuxChnType;
//     MyMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    MyMsg.Un_Content.m_DownLimit.SN = 0;
    MyMsg.Un_Content.m_DownLimit.IV_ChnNo = uc_channelNo;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return ;

    St_SNTable *st_snt = &st_SNCheckTab[uc_channelNo][LIMITSN];
    St_SNInfo * st_SNinfo = &(st_snt->m_SNInfo[st_SNCheckTab[uc_channelNo][LIMITSN].m_SNCount]);

    sn = &ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_SN;
    if(MyLimit->m_bAnyValid)
    {
        //st_CAN_SN.m_AnyLimitSN = st_CAN_SN.m_LimitSN + 1;//any limit
        if( st_CAN_SN.m_LimitSN < 0x3FF ) //防止临界(anylimit)	// 20170303 yy
            st_CAN_SN.m_AnyLimitSN = st_CAN_SN.m_LimitSN + 1;
        else
            st_CAN_SN.m_AnyLimitSN = 0x201;
        ST_IV_MAPCONFIG *iv_map_cfg = &Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType];

        for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < iv_map_cfg->m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
        {
            MyMsg.Un_Content.m_DownLimit.AuxChnNo = 0xFF;
            MyMsg.Un_CANID.m_CANID.DestID = iv_map_cfg->m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
            MyMsg.Un_Content.m_DownLimit.All = 1;
            m_CANType = iv_map_cfg->m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
            Channel_MapAuxInfo::CurrentStepAuxSum[uc_channelNo][MyMsg.Un_CANID.m_CANID.DestID] ++;  //??anylimit??unit ??
            SN = Send_CAN_Cmd(&MyMsg);
            *sn = SN;

            st_SNinfo->m_SN = SN; //dirui add 20131220
            st_SNinfo->m_IfAnyLimit = 1;
            st_SNinfo->m_AuxUintBit |= 1 << MyMsg.Un_CANID.m_CANID.DestID;
        }

        st_CAN_SN.m_LimitSN++;  //????
        st_snt->m_SNCount++;
    }
    else
    {
        unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[MyMsg.Un_Content.m_DownLimit.AuxChnType];
        MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo, AuxType, VirtualIdx);

        MyMsg.Un_Content.m_DownLimit.AuxChnNo = MyInfo.m_PhysicalChannelIdx;
        MyMsg.Un_CANID.m_CANID.DestID = MyInfo.m_UnitId;
        MyMsg.Un_Content.m_DownLimit.All = 0;
        m_CANType = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+VirtualIdx].m_CAN_Port;
        Channel_MapAuxInfo::CurrentStepAuxSum[uc_channelNo][MyInfo.m_UnitId]++;
        SN = Send_CAN_Cmd(&MyMsg);
        *sn = SN;

        st_SNinfo->m_SN = SN; //dirui add 20131220
        st_SNinfo->m_AuxUnitID = MyMsg.Un_CANID.m_CANID.DestID;
        st_snt->m_SNCount++;
    }
}

void  CANBusParse::IV_DownloadLimitToAUX_Retransmission(unsigned char uc_channelNo, unsigned short u16_StepID, unsigned char LimitIdx) //,unsigned char VirtualID
{
    //static MP_CAN_MSG MyMsg;
    MP_CAN_MSG MyMsg;
//    unsigned char m_CANType;
    St_ChannelStep *st_cstep = &ChannelInfo_Manager::ChannelStep[uc_channelNo];
    unsigned char ActiveIdx = st_cstep->m_ucActiveIdx;
    unsigned char DataType;
    unsigned short VirtualIdx;
    int SN;
    int * sn;
    St_AUXchannelInfo MyInfo;
    St_Limit *MyLimit = &st_cstep->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx];

    MyMsg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    MyMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_DOWNLOAD_LIMITS;

    if(MyLimit->m_ucLeftValue_DataType != MP_DATA_TYPE_FIGURE)
    {
        DataType = MyLimit->m_ucLeftValue_DataType;
        VirtualIdx = MyLimit->m_fLeftValue;
        MyMsg.Un_Content.m_DownLimit.FloatData = MyLimit->m_fRightValue;
        MyMsg.Un_Content.m_DownLimit.Sign = Channel_MapAuxInfo::AuxLimit_GetComPareSign(MyLimit->m_ucCompareSign, uc_channelNo, LimitIdx);
    }
    else
    {
        DataType = MyLimit->m_ucRightValue_DataType;
        VirtualIdx = MyLimit->m_fRightValue;
        MyMsg.Un_Content.m_DownLimit.FloatData = MyLimit->m_fLeftValue;
        MyMsg.Un_Content.m_DownLimit.Sign = Channel_MapAuxInfo::AuxLimit_GetComPareSign(MyLimit->m_ucCompareSign, uc_channelNo, LimitIdx);
    }
    if((MyMsg.Un_Content.m_DownLimit.Sign == CAN_COMPARE_SIGN_EQUAL) && (DataType != MP_DATA_TYPE_AUX_DI) && (DataType != MP_DATA_TYPE_AUX_DO))
    {   //add by hepeiqing,170314,for DI suport equal sign
        MyMsg.Un_Content.m_DownLimit.Sign = CAN_COMPARE_SIGN_USELESS;
    }

    unsigned char DT;
    unsigned short D1, D2;
    switch(DataType)
    {
    case MP_DATA_TYPE_AUX_T:
        DT = AUX_TEMPERATURE;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dTdt:
        DT = AUX_TEMPERATURE;
        D1 = 1;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_V:
        DT = AUX_VOLTAGE;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dVdt:
        DT = AUX_VOLTAGE;
        D1 = 1;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_P:
        DT = AUX_PRESSURE;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dPdt:
        DT = AUX_PRESSURE;
        D1 = 1;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_FlowRate:
        DT = AUX_FLOW_RATE;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dFRdt:
        DT = AUX_FLOW_RATE;
        D1 = 1;
        D2 = 0;
        break;

    case MP_DATA_TYPE_AUX_Humidity: //zyx
        DT = AUX_HUMIDITY;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dHumdt:
        DT = AUX_HUMIDITY;
        D1 = 1;
        D2 = 0;
        break;

    case MP_DATA_TYPE_AUX_Concentration:
        //DT = AUX_DENSITY;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_dConcentrationdt:
        //DT = AUX_DENSITY;
        D1 = 1;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_DI:
        DT = AUX_DI;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_AUX_DO:
        DT = AUX_DO;
        D1 = 0;
        D2 = 0;
        break;
    case MP_DATA_TYPE_EC:
        DT = AUX_EXTERNALCHARGE;
        D1 = 0;
        D2 = 0;
        break;
    default:
        break;
    }

    MyMsg.Un_Content.m_DownLimit.AuxChnType = DT;
    MyMsg.Un_Content.m_DownLimit.Dt = D1;
    MyMsg.Un_Content.m_DownLimit.D2t = D2;

    unsigned char AuxType = MyMsg.Un_Content.m_DownLimit.AuxChnType;
//     MyMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    MyMsg.Un_Content.m_DownLimit.IV_ChnNo = uc_channelNo;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return ;

//    St_SNTable *st_snt = &st_SNCheckTab[uc_channelNo][LIMITSN];
//    St_SNInfo * st_SNinfo = &(st_snt->m_SNInfo[0]);

    sn = &ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_SN;
    SN = *sn;
    MyMsg.Un_Content.m_DownLimit.SN = SN;

    if(MyLimit->m_bAnyValid)
    {
        ST_IV_MAPCONFIG *iv_map_cfg = &Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType];

        for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < iv_map_cfg->m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
        {
            MyMsg.Un_CANID.m_CANID.DestID = iv_map_cfg->m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
            MyMsg.Un_Content.m_DownLimit.AuxChnNo = 0xFF;
            MyMsg.Un_Content.m_DownLimit.All = 1;
            MyMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;//load source ID
//             MyMsg.FrameType = CAN_EXT_FRAME;// extend frame;
//             MyMsg.DLC = 8;    //8 byte;
            MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(MyMsg));
        }
    }
    else
    {
        MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo, AuxType, VirtualIdx);
        MyMsg.Un_CANID.m_CANID.DestID = MyInfo.m_UnitId;
        MyMsg.Un_Content.m_DownLimit.AuxChnNo = MyInfo.m_PhysicalChannelIdx;
        MyMsg.Un_Content.m_DownLimit.All = 0;
        MyMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;//load source ID
//         MyMsg.FrameType = CAN_EXT_FRAME;// extend frame;
//         MyMsg.DLC = 8;    //8 byte;
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(MyMsg));
    }
}

int  CANBusParse::IV_ClearLimitsToAUX(unsigned char uc_channelNo, unsigned char Clear4New, unsigned short StepID)
{   //20131202,add by hepeiqing,step finished,clear all AUX limits
    MP_CAN_MSG Msg;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_CLEAR_LIMITS;
    Msg.Un_Content.m_ClearLimit.IV_ChnNo = uc_channelNo;
    Msg.Un_Content.m_ClearLimit.SN = 0;
    Msg.Un_Content.m_ClearLimit.NewClr = Clear4New;
    StepID = 0x1234;// test step id
    Msg.Un_Content.m_ClearLimit.StepID = StepID;
//     Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    unsigned char m_CANType = 0;
    unsigned char ClrLimitIdx = 0;
    // initial clear limit ack status
    memset((void*)&st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[0], 0, sizeof(St_ClrLimitInfo)*MAXAUXUNIT); //CLRLIMIT_NONE
    st_ClrLimitAckMgr[uc_channelNo].StepID = StepID;

    for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType ++) //如果只有1个aux type也要发MAXAUXTYPE么？
    {
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            continue;
        for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
        {
            m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
            switch(m_CANType)
            {
            case CAN_PORT_USE_EXCLUSIVE:
            case CAN_PORT_USE_PUBLIC:
                Msg.Un_CANID.m_CANID.DestID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                Channel_MapAuxInfo::CurrentStepAuxSum[uc_channelNo][Msg.Un_CANID.m_CANID.DestID] = Channel_MapAuxInfo::CurrentSafetyCount[uc_channelNo][Msg.Un_CANID.m_CANID.DestID];
                //clear limit collection
                st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[ClrLimitIdx].m_SN =  CANBusParse::Send_CAN_Cmd(&Msg);
                st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[ClrLimitIdx].m_AuxID = Msg.Un_CANID.m_CANID.DestID;
                st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[ClrLimitIdx].m_Check = CLRLIMIT_WAIT;
                st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[ClrLimitIdx].m_bValid = 1;
                ClrLimitIdx++;    // called one time by this fuction
                break;

            case CAN_PORT_USE_BUILD_IN:
                break;
            //return 1;
            default:
                return 0;
            }
        }

    }
    return 0;
}


/*void  CANBusParse::IV_ClearLimitsToAUX_Retransmission(unsigned char uc_channelNo, unsigned char Clear4New, unsigned short StepID)
{   //step finished,clear all AUX limits
    MP_CAN_MSG Msg;
    Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
    Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_CLEAR_LIMITS;
    Msg.Un_Content.m_ClearLimit.IV_ChnNo = uc_channelNo;
    Msg.Un_Content.m_ClearLimit.NewClr = Clear4New;
    Msg.Un_Content.m_ClearLimit.StepID = StepID;
    Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;

    for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
    {
        if(st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[AuxIdx].m_bValid)
            if(st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[AuxIdx].m_Check == CLRLIMIT_WAIT)
            {
                Msg.Un_CANID.m_CANID.DestID = st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[AuxIdx].m_AuxID;
                Msg.Un_Content.m_ClearLimit.SN = st_ClrLimitAckMgr[uc_channelNo].m_AuxInfo[AuxIdx].m_SN;
                Msg.Un_CANID.m_CANID.SourceID =  CANFunc::SourceBoardID;//load source ID
                Msg.FrameType = CAN_EXT_FRAME;// extend frame;
                Msg.DLC = 8;    //8 byte;
                Msg.Un_Content.m_AddSafety.SN = Msg.Un_Content.m_ClearLimit.SN & 0x1FF;  //	其他命令<1024
                MCUCommun::NET_Push_to_Send(CanMsg2McuMsg(Msg));
            }
    }
}*/

int  CANBusParse::IV_RequestDataToAUX(ST_REQUEST_AUXDATA Set_DataRpt)
{   //20131202,hepeiqing
    MP_CAN_MSG Msg;
    unsigned char m_CANType;

    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(Set_DataRpt.m_IV_ChnNo, Set_DataRpt.m_aux_type, Set_DataRpt.m_aux_virtual_chn);
    if(Channel_MapAuxInfo::m_AUXmap[Set_DataRpt.m_IV_ChnNo][Set_DataRpt.m_aux_type].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return 0;
    if(BuiltIn_DataManager::b_BuiltInAux)
    {
        unsigned char myIP = board_config::getBID();
        myIP += 1;
        if(myIP == MyInfo.m_UnitId)
            return 0;
    }
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[Set_DataRpt.m_IV_ChnNo].m_aux_type_ptr[Set_DataRpt.m_aux_type];
    Msg.Un_Content.m_ReqstData.AuxChnNo = MyInfo.m_PhysicalChannelIdx;
    Msg.Un_CANID.m_CANID.DestID = MyInfo.m_UnitId;
    m_CANType = Channel_MapAuxInfo::PC2IVUnit4AuxMap[Set_DataRpt.m_IV_ChnNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+Set_DataRpt.m_aux_virtual_chn].m_CAN_Port;

    switch(m_CANType)
    {
    case CAN_PORT_USE_EXCLUSIVE:
    case CAN_PORT_USE_PUBLIC:
        Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
        Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_REQUEST_DATA;
        Msg.Un_Content.m_ReqstData.IV_ChnNo = Set_DataRpt.m_IV_ChnNo;
        Msg.Un_Content.m_ReqstData.SN = 0;
        Msg.Un_Content.m_ReqstData.single_or_repeat = Set_DataRpt.m_single_or_repeat;//repeat
        Msg.Un_Content.m_ReqstData.TimeDelta = Set_DataRpt.m_TimeDelta;
        Msg.Un_Content.m_ReqstData.MetaDelta = Set_DataRpt.m_MetaDelta;
        Msg.Un_Content.m_ReqstData.data_type = Set_DataRpt.m_data_type; // LOG_DELTA_META,LOG_DELTA_ALL,
        Msg.Un_Content.m_ReqstData.AuxChnType = Set_DataRpt.m_aux_type;

        CANBusParse::Send_CAN_Cmd(&Msg);

        return 1;
    //	case CAN_PORT_USE_PUBLIC:
    case CAN_PORT_USE_BUILD_IN:
        return 1;
    default:
        return 0;
    }
}
int  CANBusParse::IV_TriggerLogToAUX(unsigned char uc_channelNo)
{   //20131202,hepeiqing
    MP_CAN_MSG Msg;
    unsigned char m_CANType;
    unsigned char sendedUnitID[MAXUNIT], validIdx = 0;
    bool bInvalid = false;
    if(CommonFunc::GetTimeTicks(&lastAuxTriggerTime[uc_channelNo]) < 500)
    {
        return -1;
    }
    CommonFunc::GetTimeTicksUpdate(&lastAuxTriggerTime[uc_channelNo]);

    Msg.Un_Content.m_TrigLog.m_Mstime = CommonFunc::GetSystemTime();
    //totaltriggers++;
    //lastAuxTriggerTime = Msg.Un_Content.m_TrigLog.m_Mstime;
    for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType ++)
    {
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            continue;
        for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
        {
            //Msg.Un_Content.m_TrigLog.m_Mstime = CommonFunc::GetSystemTime();
            m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
            unsigned char AuxUnitId = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
            bInvalid = false;
            for(unsigned char oldIdx = 0; oldIdx < validIdx; oldIdx++)
            {
                if(AuxUnitId == sendedUnitID[oldIdx])
                {
                    bInvalid = true;
                    break;
                }
            }
            if(bInvalid)
                continue;

            switch(m_CANType)
            {
            case CAN_PORT_USE_EXCLUSIVE:
            case CAN_PORT_USE_PUBLIC:
                Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
                //Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRIGGER_LOG;
                Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRIGGER_GROUPLOG;
//				Msg.Un_Content.m_TrigLog.IV_GlobalChnNo = ChannelInfo_Manager::BoardCFG.m_Global.m_uFrom + uc_channelNo;
                Msg.Un_Content.m_TrigLog.IV_ChnNo = uc_channelNo;
                //Msg.Un_Content.m_TrigLog.bLogDxdt = 1;//ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_bAuxDxdtLog;//171113,hpq,add for pro8
                //Msg.Un_Content.m_TrigLog.lastPoint = false;
                //Msg.Un_Content.m_TrigLog.m_Mstime = CommonFunc::GetSystemTime(); //WL 11.25.2019 moved log time to outside the loop
//                 Msg.Un_Content.m_TrigLog.m_Mstime.Second = OS_ServeProcess::OS_Time.Second;
//                 Msg.Un_Content.m_TrigLog.m_Mstime.Us100 = OS_ServeProcess::OS_Time.Us100;
//				Msg.Un_Content.m_TrigLog.AuxChnNo = 0xFF;
                Msg.Un_CANID.m_CANID.DestID = AuxUnitId;
                sendedUnitID[validIdx++] = AuxUnitId;
                CANBusParse::Send_CAN_Cmd(&Msg);
                break;
            //case CAN_PORT_USE_PUBLIC:
            case CAN_PORT_USE_BUILD_IN:
                break;
            default:
                return 0;
            }
        }
    }

    return 1;
}

int CANBusParse::IV_TriggerLastLogToAux(unsigned char uc_channelNo)
{
    MP_CAN_MSG Msg;
    unsigned char m_CANType;
    unsigned char sendedUnitID[8], validIdx = 0;
    bool bInvalid = false;
    /*if(CommonFunc::GetTimeTicks(&lastAuxTriggerTime) < 1000/8)
    {
    	return -1;
    }*/
    Msg.Un_Content.m_TrigLog.m_Mstime = CommonFunc::GetSystemTime();
    //totaltriggers++;
    //lastAuxTriggerTime = Msg.Un_Content.m_TrigLog.m_Mstime;
    //CommonFunc::GetTimeTicksUpdate(&lastAuxTriggerTime);
    for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType ++)
    {
        if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
            continue;
        for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
        {
            m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
            unsigned char AuxUnitId = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
            bInvalid = false;
            for(unsigned char oldIdx = 0; oldIdx < validIdx; oldIdx++)
            {
                if(AuxUnitId == sendedUnitID[oldIdx])
                {
                    bInvalid = true;
                    break;
                }
            }
            if(bInvalid)
                continue;

            switch(m_CANType)
            {
            case CAN_PORT_USE_EXCLUSIVE:
            case CAN_PORT_USE_PUBLIC:
                Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
                //Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRIGGER_LOG;
                Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRIGGER_GROUPLOG_LAST;
//				Msg.Un_Content.m_TrigLog.IV_GlobalChnNo = ChannelInfo_Manager::BoardCFG.m_Global.m_uFrom + uc_channelNo;
                Msg.Un_Content.m_TrigLog.IV_ChnNo = uc_channelNo;
                //Msg.Un_Content.m_TrigLog.bLogDxdt = 1;//ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_bAuxDxdtLog;//171113,hpq,add for pro8
                //Msg.Un_Content.m_TrigLog.lastPoint = true;
                //Msg.Un_Content.m_TrigLog.m_Mstime = CommonFunc::GetSystemTime(); //WL 11.25.2019 moved log time to outside the loop
//                 Msg.Un_Content.m_TrigLog.m_Mstime.Second = OS_ServeProcess::OS_Time.Second;
//                 Msg.Un_Content.m_TrigLog.m_Mstime.Us100 = OS_ServeProcess::OS_Time.Us100;
//				Msg.Un_Content.m_TrigLog.AuxChnNo = 0xFF;
                Msg.Un_CANID.m_CANID.DestID = AuxUnitId;
                sendedUnitID[validIdx++] = AuxUnitId;
                CANBusParse::Send_CAN_Cmd(&Msg);
                break;
            //case CAN_PORT_USE_PUBLIC:
            case CAN_PORT_USE_BUILD_IN:
                break;
            default:
                return 0;
            }
        }
    }

    return 1;
}
// int  CANBusParse::IV_EnableSDLToAUX(unsigned char uc_channelNo, unsigned short u16_StepID, unsigned char bEnable)
// {   //20131202,hepeiqing
//     MP_CAN_MSG Msg;
//     unsigned char m_CANType;

//     for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType ++)
//     {
//         if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
//             continue;
//         for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
//         {
//             m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
//             switch(m_CANType)
//             {
//             case CAN_PORT_USE_EXCLUSIVE:
//             case CAN_PORT_USE_PUBLIC:
//                 Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
//                 Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_ENABLE_SDL;
//                 Msg.Un_Content.m_EnableSdl.IV_ChnNo = uc_channelNo;
//                 Msg.Un_Content.m_EnableSdl.SN = 0;
//                 Msg.Un_Content.m_EnableSdl.Enable = (bEnable > 0);
//                 Msg.Un_Content.m_EnableSdl.All = 0;
//                 Msg.Un_Content.m_EnableSdl.StepID = u16_StepID;
//                 Msg.Un_Content.m_EnableSdl.AuxChnType = AuxType;
//                 Msg.Un_Content.m_EnableSdl.AuxChn = 0;  // 0xFF;
//                 Msg.Un_CANID.m_CANID.DestID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
//                 CANBusParse::Send_CAN_Cmd(&Msg);
//                 break;
//             //case CAN_PORT_USE_PUBLIC:
//             case CAN_PORT_USE_BUILD_IN:
//                 break;
//             default:
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }
// void  CANBusParse::IV_TimeSyncToAux(MsTime * SyncTime,unsigned char RequestType,unsigned char CanPort)
// {
// 	MP_CAN_MSG Msg;
// 	unsigned char m_CANType=CanPort;
// 	static int SyncCounter = 0;
// 	if(OS_ServeProcess::Eth_TimeCalied)
// 	{
// 		//Msg.Un_Content.m_SyncTime.Cmd = CAN_CMD_TIME_SYNC;
// 		Msg.Un_Content.m_SyncTime.m_MsTime.Second =  SyncTime->Second;
// 		Msg.Un_Content.m_SyncTime.m_MsTime.Us100 =  SyncTime->Us100;
// 		if(RequestType == 0) //粗，直接绝对值对时
// 			Msg.Un_Content.m_SyncTime.m_CaliStatus = 1;
// 		else
// 			Msg.Un_Content.m_SyncTime.m_CaliStatus = 0;
// 		OS_ServeProcess::CAN_TimeCalied = 1;
// 		CANBusParse::Send_CAN_Cmd(m_CANType,&Msg);
// 		SyncCounter++;
// 	}
// }


void CANBusParse::IV_SendDigitOut(unsigned char uc_channelNo, unsigned char  bSet)
{   //20131202,hepeiqing
    MP_CAN_MSG Msg;
    unsigned char m_CANType, auxUnitID;

    unsigned int AuxUnitBitOut;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return;

    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
    {
        //Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_iv4aux.
        m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
        unsigned char myDOIdx = 0;
        for(; myDOIdx < MAXUNITID; myDOIdx++)
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_channelNo].m_AuxUnitInfo[myDOIdx].m_ID)
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                AuxUnitBitOut =  Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_channelNo].m_AuxUnitInfo[myDOIdx].m_BitOut;
                break;
            }
        }
        if(myDOIdx == MAXUNITID)
            return;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_SET_CTRL;
            Msg.Un_Content.m_SetCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetCtrl.SN = 0;
            if(!bSet)
                AuxUnitBitOut = 0;
            Msg.Un_Content.m_SetCtrl.Set_Value.m_32bitDO = AuxUnitBitOut;
            Msg.Un_Content.m_SetCtrl.AuxChnType = AUX_DO;
            Msg.Un_Content.m_SetCtrl.AuxChnNo = 0;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;
}
void CANBusParse::IV_SendAnalogSingnalOut(unsigned char uc_channelNo, unsigned char bSet)
{
// add by qjm 20160819
    MP_CAN_MSG Msg;
    unsigned char m_CANType;  //,auxUnitID;
    unsigned short w_set_value = 0;
    for(unsigned char elementIdx = 0; elementIdx < Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_ChnCount; elementIdx++)
    {
        m_CANType = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].m_CANType;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_AIO_CTRL;
            Msg.Un_Content.m_SetAIOCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetAIOCtrl.Aux_AI_ChnNo = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].Aux_AI_ChnNO;
            Msg.Un_Content.m_SetAIOCtrl.Aux_AI_ChnType = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].Aux_AI_ChnType;
            Msg.Un_Content.m_SetAIOCtrl.SN = 0;
            Msg.Un_Content.m_SetAIOCtrl.AUX_AO_ChnNO = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].Aux_AO_ChnNO;
            if(!bSet)
            {
                Msg.Un_Content.m_SetAIOCtrl.On_Off_Flag = 0;
                w_set_value = 16384;
            }
            else
            {
                Msg.Un_Content.m_SetAIOCtrl.On_Off_Flag = 1;
                w_set_value = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].Set_Value;
            }
            Msg.Un_Content.m_SetAIOCtrl.m_PID_Value.Set_value = w_set_value;
            Msg.Un_Content.m_SetAIOCtrl.m_PID_Value.kP = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].kP;
            Msg.Un_Content.m_SetAIOCtrl.m_PID_Value.kI = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].kI;
            Msg.Un_CANID.m_CANID.DestID = Channel_MapAuxInfo::m_AuxAOCtrlMgr[uc_channelNo].m_Value[elementIdx].m_ID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }

}

void CANBusParse::IV_SendTempChamberOut(unsigned char uc_channelNo, unsigned short  AuxCh, unsigned char  AuxType, float SetValue)
{   //20131202,hepeiqing
    MP_CAN_MSG Msg;
    unsigned char m_CANType, auxUnitID;

    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo, AuxType, AuxCh);

    float AuxUnitBitOut;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AuxType];

    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
    {
        //Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_iv4aux.
        m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
        unsigned char myIdx = 0;
        for(; myIdx < MAXUNITID; myIdx++)
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxCh].m_Aux_UnitID)
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                AuxUnitBitOut =  SetValue; // Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_channelNo].m_AuxUnitInfo[myIdx].m_BitOut;
                break;
            }
        }
        if(myIdx == MAXUNITID)
            continue;  //return;	解决温控箱必须map到第一个通道的bug, Yifei, 08.11.2015
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_SET_CTRL;
            Msg.Un_Content.m_SetCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetCtrl.SN = 0;
            Msg.Un_Content.m_SetCtrl.Set_Value.f_SP = AuxUnitBitOut;
            Msg.Un_Content.m_SetCtrl.AuxChnType = AuxType;
            Msg.Un_Content.m_SetCtrl.AuxChnNo = MyInfo.m_PhysicalChannelIdx;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;
}

void CANBusParse::IV_SendTempChamberOutRunON(unsigned char uc_channelNo, unsigned short  AuxCh, unsigned char  AuxType, unsigned char  RunON) //带RunON的温控箱,控制温控箱开关 zyx
{
    MP_CAN_MSG Msg;
    unsigned char m_CANType, auxUnitID;

    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo, AuxType, AuxCh); //获取AUX物理通道号和所属ID（ip）

    unsigned char AuxUnitBitOut;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0)
        return;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AuxType];

    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++) //在小于本IV通道下该AUX类型的UINT数前做以下操作
    {
        //Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_iv4aux.
        m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
        unsigned char myIdx = 0;
        for(; myIdx < MAXUNITID; myIdx++) //寻找合适的单元的ID(IP)地址
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxCh].m_Aux_UnitID)
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                AuxUnitBitOut =  RunON; // Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_channelNo].m_AuxUnitInfo[myIdx].m_BitOut;
                break;
            }
        }
        if(myIdx == MAXUNITID)
            continue;  //return;	解决温控箱必须map到第一个通道的bug, Yifei, 08.11.2015
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_CTRL_RUNON;
            Msg.Un_Content.m_CtrlRunOn.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_CtrlRunOn.SN = 0;
            Msg.Un_Content.m_CtrlRunOn.RunON = AuxUnitBitOut;
            Msg.Un_Content.m_CtrlRunOn.AuxChnType = AuxType;
            Msg.Un_Content.m_CtrlRunOn.AuxChnNo = MyInfo.m_PhysicalChannelIdx;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;

}

//停止MTC温度保护
void CANBusParse::IV_SendMTCTemperatureSaftyCheckOFF(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType)
{
    MP_CAN_MSG Msg;
    unsigned char m_CANType,auxUnitID;

    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo,AuxType,AuxCh);

    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit==0)
        return;

    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AuxType];
    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
    {
        m_CANType=Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
        unsigned char myIdx = 0;
        for(; myIdx<MAXUNITID; myIdx++)
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxCh].m_Aux_UnitID)
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                break;
            }
        }
        if(myIdx==MAXUNITID)
            continue;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_MTC_SAFETY_STOP;
            Msg.Un_Content.m_SetCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetCtrl.AuxChnType = AuxType;
            Msg.Un_Content.m_SetCtrl.AuxChnNo = MyInfo.m_PhysicalChannelIdx;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;
}

/*设置温度保护时长 07.13.2018 by zc */
void CANBusParse::IV_SendMTCSafetyTimeOut(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType,unsigned short MTCSafetyTimeValue, unsigned char SafetyGotoTag)
{
    MP_CAN_MSG Msg;
    unsigned char m_CANType,auxUnitID;

    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_channelNo,AuxType,AuxCh);

    unsigned short AuxUnitBitOut;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit==0)
        return;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AuxType];

    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++)
    {
        m_CANType=Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx];
        unsigned char myIdx = 0;
        for(; myIdx<MAXUNITID; myIdx++)
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxCh].m_Aux_UnitID)
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];
                AuxUnitBitOut =  MTCSafetyTimeValue;  //温度保护时长
                break;
            }
        }
        if(myIdx==MAXUNITID)
            continue;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_MTC_SAFETY_CHECK;
            Msg.Un_Content.m_SetCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetCtrl.SN = SafetyGotoTag;
            Msg.Un_Content.m_SetCtrl.Set_Value.m_uMTCSafetyTimeValue = AuxUnitBitOut;   //发送温度保护时长出去
            Msg.Un_Content.m_SetCtrl.AuxChnType = AuxType;
            Msg.Un_Content.m_SetCtrl.AuxChnNo = MyInfo.m_PhysicalChannelIdx;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;
}

void CANBusParse::IV_SendExtChargeOut(unsigned char uc_channelNo,unsigned char bSet)
{   //20180825,qijingming
    MP_CAN_MSG Msg;
    unsigned char m_CANType,auxUnitID;

    //   unsigned int AuxUnitBitOut;
    if(Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Total_Aux_Unit == 0) //AUX_EXTERNALCHARGE
        return;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AUX_EXTERNALCHARGE];

    for(unsigned char AuxUnitIdx = 0; AuxUnitIdx < Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Total_Aux_Unit; AuxUnitIdx++) //AUX_EXTERNALCHARGE
    {
        //Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_DO].m_iv4aux.
        m_CANType = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Aux_UnitCANPort[AuxUnitIdx]; //AUX_EXTERNALCHARGE
        unsigned char myDOIdx = 0;
        for(; myDOIdx < MAXUNITID; myDOIdx++)
        {
            if( Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx] == Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+AuxUnitIdx].m_Aux_UnitID)  //AUX_EXTERNALCHARGE
            {
                auxUnitID = Channel_MapAuxInfo::m_AUXmap[uc_channelNo][AUX_EXTERNALCHARGE].m_AuxUnitCfg.m_Aux_UnitID[AuxUnitIdx];  //AUX_EXTERNALCHARGE
                //						AuxUnitBitOut =  Channel_MapAuxInfo::m_AuxDoCtrlMgr[uc_channelNo].m_AuxUnitInfo[myDOIdx].m_BitOut;
                break;
            }
        }
        if(myDOIdx == MAXUNITID)
            return;
        switch(m_CANType)
        {
        case CAN_PORT_USE_EXCLUSIVE:
        case CAN_PORT_USE_PUBLIC:
            Msg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
            Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_SET_CTRL;
            Msg.Un_Content.m_SetCtrl.IV_ChnNo = uc_channelNo;
            Msg.Un_Content.m_SetCtrl.SN = 0;
            Msg.Un_Content.m_SetCtrl.Set_Value.m_32bitDO = bSet;    //(bSet<<uc_channelNo);
            Msg.Un_Content.m_SetCtrl.AuxChnType = AUX_EXTERNALCHARGE;
            Msg.Un_Content.m_SetCtrl.AuxChnNo = 0;  // 0xFF;
            Msg.Un_CANID.m_CANID.DestID = auxUnitID;
            CANBusParse::Send_CAN_Cmd(&Msg);
            break;
        //case CAN_PORT_USE_PUBLIC:
        case CAN_PORT_USE_BUILD_IN:
            break;
        default:
            return;
        }

    }
    return;
}
//------------------------------end of IV Send Command---------------------------//

/************************************************************************/
/* IV　Recv Command Parse  Function                                     */
/************************************************************************/

//将请求数据存入大数据体（供limitcheck任务查询，当做采样缓冲区）
int  CANBusParse::IV_RxLimitData(MP_CAN_MSG *Msg)
{
    unsigned char status;
    ST_AUX_UPLOADATA UploadData;

    UploadData.Aux_Type = Msg->Un_Content.m_DataAck.AuxChnType;
    if(Msg->Un_Content.m_DataAck.AuxChnNo > MAXVIRTUALCHANNEL) //这里没对应？
    {
        //Aux_Confirm_Ack(Msg, AUX_CHNO_OVERFLOW);
        return -1;
    }
    UploadData.Aux_ChnIdx = Msg->Un_Content.m_DataAck.AuxChnNo;
    UploadData.Aux_Data = Msg->Un_Content.m_DataAck.FloatData;
    UploadData.IV_ChnIdx = Msg->Un_Content.m_DataAck.IV_ChnNo;
    UploadData.Data_type = Msg->Un_Content.m_DataAck.DataType;
    UploadData.AUX_CANID = Msg->Un_CANID.m_CANID.SourceID;

    status = Channel_MapAuxInfo::RecvAuxData( UploadData);
    if(status == DATA_TYPE_ERROR)
    {
        // Aux_Confirm_Ack(Msg, AUX_DATA_TYPE_OVERFLOW);  //???不需要ack
        return -1;
    }
    return 0;
}

int CANBusParse::IV_AuxUnsafeRpt(MP_CAN_MSG *Msg)
{
    Aux_Confirm_Ack(Msg, NO_ERROR); //先回复
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    unsigned char IV_ChIdx, Aux_Type, SafetyFlag;
    IV_ChIdx = Msg->Un_Content.m_Unsafe.IV_ChnNo;
    if(IV_ChIdx > MAXCHANNELNO)
    {
        Aux_Confirm_Ack(Msg, IV_CHNO_OVERFLOW);
        return -1;
    }
    //Aux_Chn = Msg->Un_CANID.m_CANID.AuxChn;
    Aux_Type = Msg->Un_Content.m_Unsafe.AuxChnType;
    float UnsafVlue =  Msg->Un_Content.m_Unsafe.FloatData;
    //ErrorFlag = Msg->Un_Content.m_Unsafe.OverrunStatus;
    SafetyFlag = Msg->Un_Content.m_Unsafe.OverrunStatus;
    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
    //SchMessage.MsgNumber = AUX_Unsafe_Msg;
//     SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSpecial[0] = UnsafVlue;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeValue = UnsafVlue;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID =  Msg->Un_Content.m_Unsafe.AuxChnNo + 1;
    SchMessage.st_status_sch_running.m_btLost_Unit = Msg->Un_CANID.m_CANID.SourceID;
    // 	switch(ErrorFlag)     //add by dirui 20131209  //原来的ErrorFlag的信息和AuxType的信息重复。
    // 	{
    // 		case AUX_TEMPERATURE:
    // 			//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<9);
    // 			SchMessage.st_status_sch_running.m_btSafe_Code = CURRENT_SCH_UNSAFE;
    // 			//important    comment out chen 20130814
    // 		  ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = CURRENT_SCH_UNSAFE;
    // 		break;
    // 		case AUX_VOLTAGE:
    // 			SchMessage.st_status_sch_running.m_btSafe_Code = CURRENT_SCH_UNSAFE;
    // 			//important    comment out chen 20130814
    // 		  ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = CURRENT_SCH_UNSAFE;
    // 			//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<11);
    // 		break;
    // 		default:
    // 			//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=ALL_SAFE;
    // 	}

    switch(SafetyFlag)     //add by dirui 20131209  //原来的ErrorFlag的信息和AuxType的信息重复。现在SafetyFlag是报非安全还是回到安全了
    {
    case AUX_UNSAFE_TOIV:
        SchMessage.MsgNumber = AUX_Unsafe_Msg;
// 			 switch (Aux_Type)
// 				{
// 				case AUX_TEMPERATURE:
// 					//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<9);
// 					SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_TEMPERATURE_UNSAFE;
// 					//important    comment out chen 20130814
// 					ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_TEMPERATURE_UNSAFE;
// 				break;
// 				case AUX_VOLTAGE:
// 					SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_VOLTAGE_UNSAFE;
// 					//important    comment out chen 20130814
// 					ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_VOLTAGE_UNSAFE;
// 					//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<11);
// 				break;
//                 case AUX_FLOW_RATE:
// 					SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_FLOW_RATE_UNSAFE;
// 					ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_FLOW_RATE_UNSAFE;  // 用于温控器湿度SCH报警   20150609 yang
// 				break;
// 				case AUX_EXTERNALCHARGE:
// 					SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_EXTCHARGE_UNSAFE;
// 					ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_EXTCHARGE_UNSAFE;  // 用于温控器湿度SCH报警   20150609 yang
// 				  break;
// 				case AUX_PRESSURE:
// 					SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_PRESSURE_UNSAFE;
// 					ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_PRESSURE_UNSAFE;  //用于辅助压力报警
// 				   break;
// 				default:
// 					break;
// 					//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=ALL_SAFE;
//
// 				 }
        SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_UNSAFE;  //  回收辅助报警代号重新处理    20160331 yy
        SchMessage.st_status_sch_running.m_btSubCode = Aux_Type;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_UNSAFE;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_UnsafeType = AUX_UNSAFE_TYPE;
        safety_check::SetUnsafe(IV_ChIdx,AUX_UNSAFE,0, AUX_UNSAFE_TYPE);
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_SafetyStatus = ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus;
        Schedule_Manager_task::UnsafeContent[IV_ChIdx] = SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(IV_ChIdx, SchMessage); //待狄工加入消息结构内容
        break;
    case OnlineRSMVol_UNSAFE:
    case OnlineRSMCur_UNSAFE:
    case OnlineRSMTemp_UNSAFE:
        SchMessage.MsgNumber = AUX_Unsafe_Msg;
        SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_UNSAFE;
        SchMessage.st_status_sch_running.m_btSubCode = Aux_Type;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = AUX_UNSAFE;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_UnsafeType = AUX_UNSAFE_TYPE;
        safety_check::SetUnsafe(IV_ChIdx,AUX_UNSAFE,0, AUX_UNSAFE_TYPE);
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_SafetyStatus = ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus;
        Schedule_Manager_task::UnsafeContent[IV_ChIdx] = SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(IV_ChIdx, SchMessage); //待狄工加入消息结构内容
        break;
        /*
        case AUX_SAFE:    //回复安全时，该如何处理？

        	switch (Aux_Type)
        	{
        	case AUX_TEMPERATURE:
        	//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<9);
        	SchMessage.st_status_sch_running.m_btSafe_Code = CURRENT_SCH_UNSAFE;
        	//important    comment out chen 20130814
        	ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = CURRENT_SCH_UNSAFE;
        	break;
        	case AUX_VOLTAGE:
        	SchMessage.st_status_sch_running.m_btSafe_Code = CURRENT_SCH_UNSAFE;
        	//important    comment out chen 20130814
        	ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = CURRENT_SCH_UNSAFE;
        	//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=(1<<11);
        	break;
        	default:
        	//Schedule_Manager_task::UnsafeMessage[IV_ChIdx].st_status_sch_running=ALL_SAFE;
        	Schedule_Manager_task::SendMsgToSchedule(IV_ChIdx,SchMessage);//待狄工加入消息结构内容
        	}
        	break;

        	*/

    }


    return 0;
}

//add by zc 08082018 for MTC
int CANBusParse::IV_AuxMTCTempUnsafeRpt(MP_CAN_MSG *Msg)
{
    Aux_Confirm_Ack(Msg,NO_ERROR);  //先回复
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
    unsigned char IV_ChIdx,Aux_Type;//,SafetyFlag;
    IV_ChIdx = Msg->Un_Content.m_Unsafe.IV_ChnNo;
    if(IV_ChIdx>MAXCHANNELNO)
    {
        Aux_Confirm_Ack(Msg,IV_CHNO_OVERFLOW);
        return -1;
    }
//	if(safety_check::MTCTempSafetyGoto[IV_ChIdx] == 0)
    if(Msg->Un_Content.m_Unsafe.OverrunStatus == 0)  //unsafe
    {
        Aux_Type = Msg->Un_Content.m_Unsafe.AuxChnType;
        float UnsafVlue =  Msg->Un_Content.m_Unsafe.FloatData;
//		SafetyFlag = Msg->Un_Content.m_Unsafe.OverrunStatus;
        SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
//         SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSpecial[0] = UnsafVlue;
		SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.f_UnsafeSpecial[0] = UnsafVlue;
        SchMessage.st_status_sch_running.m_btLost_Unit = Msg->Un_CANID.m_CANID.SourceID;

        SchMessage.MsgNumber = AUX_Unsafe_Msg;

        SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = MTC_TEMPERATURE_UNSAFE;
        SchMessage.st_status_sch_running.m_btSubCode = Aux_Type;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = MTC_TEMPERATURE_UNSAFE;
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_UnsafeType = AUX_UNSAFE_TYPE;
        safety_check::SetUnsafe(IV_ChIdx,MTC_TEMPERATURE_UNSAFE,0, AUX_UNSAFE_TYPE);
        ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_SafetyStatus = ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus;
        Schedule_Manager_task::UnsafeContent[IV_ChIdx]=SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(IV_ChIdx,SchMessage);//待狄工加入消息结构内容

        return 0;
    }
    else   //warning
    {
        Un_ERROR_MESSAGE * st_SPB;
        st_SPB = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;
        st_SPB->m_IV_ERROR.m_btChannel_ID = IV_ChIdx;
        st_SPB->m_IV_ERROR.m_btCode_Type = IV_ERROR_TYPE_WARNNING;
        st_SPB->m_IV_ERROR.m_btType_ErrorNum = IV_ERROR_WANRING_MTC;
        st_SPB->m_IV_ERROR.m_btSubCode = Msg->Un_CANID.m_CANID.SourceID;
        st_SPB->m_IV_ERROR.m_fIR_Value = Msg->Un_Content.m_Unsafe.FloatData;

        EthernetParse::MPReportData(IV_ChIdx,SEND_ERROR_MESSAGE);
        return 0;
    }
}

//add by zc 08312018 for MTC
int CANBusParse::IV_AuxMTCFanUnsafeRpt(MP_CAN_MSG *Msg)
{
    Aux_Confirm_Ack(Msg,NO_ERROR);  //先回复
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
    unsigned char IV_ChIdx,Aux_Type;//,SafetyFlag;
    IV_ChIdx = Msg->Un_Content.m_Unsafe.IV_ChnNo;
    if(IV_ChIdx>MAXCHANNELNO)
    {
        Aux_Confirm_Ack(Msg,IV_CHNO_OVERFLOW);
        return -1;
    }
//	if(safety_check::MTCTempSafetyGoto[IV_ChIdx] == 0)
    Aux_Type = Msg->Un_Content.m_Unsafe.AuxChnType;
//		float UnsafVlue =  Msg->Un_Content.m_Unsafe.FloatData;
//		SafetyFlag = Msg->Un_Content.m_Unsafe.OverrunStatus;
    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
    SchMessage.st_status_sch_running.m_btLost_Unit = Msg->Un_CANID.m_CANID.SourceID;

    SchMessage.MsgNumber = AUX_Unsafe_Msg;

    SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
    SchMessage.st_status_sch_running.m_uw32Safe_Code = MTC_FAN_UNSAFE;
    SchMessage.st_status_sch_running.m_btSubCode = Aux_Type;
    ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus = MTC_FAN_UNSAFE;
    ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_UnsafeType = AUX_UNSAFE_TYPE;
    safety_check::SetUnsafe(IV_ChIdx,MTC_FAN_UNSAFE,0, AUX_UNSAFE_TYPE);
    ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_SafetyStatus = ChannelInfo_Manager::ChannelInfo[IV_ChIdx].m_AUXSafetyStatus;
    Schedule_Manager_task::UnsafeContent[IV_ChIdx]=SchMessage;
    Schedule_Manager_task::SendMsgToSchedule(IV_ChIdx,SchMessage);//待狄工加入消息结构内容

    return 0;
}

int CANBusParse::IV_RxMetaVariable(MP_CAN_MSG *Msg)
{
    unsigned char IV_ChIdx, MV_Flag;
    IV_ChIdx = Msg->Un_Content.m_MetaVariable.IV_ChnNo;
    if(IV_ChIdx > MAXCHANNELNO)
    {
        // 	    	Aux_Confirm_Ack(CANx,Msg,IV_CHNO_OVERFLOW);
        return -1;
    }
    MV_Flag = Msg->Un_Content.m_MetaVariable.MV_Type;
    float MV_Value =  Msg->Un_Content.m_MetaVariable.FloatData;

    Channel_MapAuxInfo::RecvAuxMetaData(IV_ChIdx, MV_Flag, MV_Value);

    return 0;
}
int   CANBusParse::IV_AuxLimitRpt(MP_CAN_MSG *Msg)
{
    unsigned char IV_chnIdx, ReachStatus;
    unsigned short SN;
//     if(Msg->Un_Content.m_LimitRpt.AuxChnNo > MAXVIRTUALCHANNEL)
//     {
//         Aux_Confirm_Ack(Msg, AUX_CHNO_OVERFLOW);
//         return -1;
//     }
    ReachStatus = Msg->Un_Content.m_LimitRpt.Reached ;
    IV_chnIdx =  Msg->Un_Content.m_LimitRpt.IV_ChnNo;
    SN = Msg->Un_Content.m_LimitRpt.SN;
    if(IV_chnIdx > MAXCHANNELNO)
    {
        Aux_Confirm_Ack(Msg, IV_CHNO_OVERFLOW);
        return -1;
    }
    Check_Limit4_task::LimitStatusChanged(IV_chnIdx, ReachStatus, SN);
    Aux_Confirm_Ack(Msg, NO_ERROR);
    return 0;
}


//iv sn号确认 给schedule运行做check用
int  CANBusParse::IV_AckProcess(MP_CAN_MSG *Msg)
{
    static unsigned char SNType;
    bool bNewSN = 1;
    unsigned char AnyLmtPosInRes = 0;
    unsigned char IV_ChnNo = Msg->Un_Content.m_ConfirmAck.IV_ChnNo;
    if(IV_ChnNo >= MAXCHANNELNO)
        return -1;
    unsigned char AuxUintID = Msg->Un_CANID.m_CANID.SourceID;
    unsigned short SN = Msg->Un_Content.m_ConfirmAck.SN;
    unsigned char RxCmd = Msg->Un_Content.m_ConfirmAck.RecvCmd;
    St_SNTable *st_S;
    int Residx;
    St_ClrLimitAck *st_CLR = &st_ClrLimitAckMgr[IV_ChnNo];
    St_SNInfo *st_SN;
    St_AnyLimitSNCount *st_ALSNC;

    switch(RxCmd)
    {
    case CAN_CMD_DOWNLOAD_LIMITS:
        SNType = 1;  //limit SNType
        st_S = &st_SNResTable[IV_ChnNo][SNType];
        Residx = st_SNResTable[IV_ChnNo][SNType].m_SNCount;

        if(Msg->Un_Content.m_ConfirmAck.IfAnyLimit)
        {
            for(unsigned char uc_Idx = 0; uc_Idx < MAXIFANYLIMIT; uc_Idx++)
            {
                st_ALSNC = &st_S->m_stAnyLimit.m_SNInfo[uc_Idx];
                if(SN == st_ALSNC->m_SN)
                {   //找到该anyLimit的SN号并返回在SN_ResTable中的位置
                    bNewSN = 0;  //
                    AnyLmtPosInRes = st_ALSNC->m_PosInResTab;
                    break;
                }
            }
            if(bNewSN != 0)//没找到对应SN号则新建一个Count
            {
                unsigned char CurrentCnt =  st_S->m_stAnyLimit.m_Cnt; //AnyLimit个数
                st_ALSNC = &st_S->m_stAnyLimit.m_SNInfo[CurrentCnt];

                AnyLmtPosInRes = st_S->m_SNCount;    //返回该AnyLimit在ResTable中的位置
                st_ALSNC->m_PosInResTab = AnyLmtPosInRes;  //记录该limit SN位置
                st_ALSNC->m_SN = SN;	              //记录anylimit SN
                st_S->m_stAnyLimit.m_Cnt++;
                if(st_S->m_stAnyLimit.m_Cnt == MAXIFANYLIMIT)
                {
                    st_S->m_stAnyLimit.m_Cnt = 0;  //Schedule检查完毕要清零 这里是保护
                    return -1;
                }

                st_SN = &st_S->m_SNInfo[AnyLmtPosInRes];
                st_SN->m_StepID = Msg->Un_Content.m_ConfirmAck.StepID;
                st_SN->m_SN = SN;
                st_SN->m_IfAnyLimit = 1;
                st_S->m_SNCount++;

            }
            st_SN = &st_S->m_SNInfo[AnyLmtPosInRes];
            st_SN->m_AuxUintBit |= 1 << Msg->Un_CANID.m_CANID.SourceID;
        }
        else
        {
            //非Anylimit

            st_SN = &st_S->m_SNInfo[Residx];
            st_SN->m_SN = SN;
            st_SN->m_IfAnyLimit = 0;
            st_SN->m_AuxUintBit = 0;
            st_SN->m_AuxUnitID = Msg->Un_CANID.m_CANID.SourceID;
            st_S->m_SNCount ++;
        }
        break;

    case CAN_CMD_CLEAR_LIMITS: //process ClrLimit Ack

        for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
        {
            St_ClrLimitInfo *st_AuxInfo = &st_CLR->m_AuxInfo[AuxIdx];
            if(!st_AuxInfo->m_bValid)
                continue;
            if( (st_AuxInfo->m_AuxID == AuxUintID) && (st_AuxInfo->m_SN == SN) ) //search the same AuxUintID Number
            {
                //if(st_ClrLimitAckMgr[IV_ChnNo].m_StepID == Msg->Un_Content.m_ConfirmAck.StepID)  //暂时屏蔽
                st_AuxInfo->m_Check = CLRLIMIT_OK;
                break;  //already find
            }
        }
        break;

    case CAN_CMD_DOWNLOAD_SAFETY:
        SNType = 0;  //SAFETY SNTYPE  LIMITSN
        st_S = &st_SNResTable[IV_ChnNo][SNType];
        Residx = st_SNResTable[IV_ChnNo][SNType].m_SNCount;
        st_S->m_SNInfo[Residx].m_SN = SN;
        st_S->m_SNCount ++;
        break;
    default:
        return -1;
    }
    return 0;
}

int  CANBusParse::IV_ProcessAuxStatusInfo(MP_CAN_MSG *Msg)
{
    unsigned char AuxUintID = Msg->Un_CANID.m_CANID.SourceID;
    unsigned char IV_Idx = Msg->Un_Content.m_AuxStatus.startIVIdx;
    unsigned char IVChnCnt = 8 + IV_Idx;
    for(; IV_Idx < IVChnCnt; IV_Idx++)
    {
        if(st_AuxInfoRes[IV_Idx].m_IVChnExist != 1)
            continue;
        unsigned char AuxIdx = 0;   //aux 索引   20140828 ZH
        for(; AuxIdx < MAXAUXUNIT; AuxIdx++) //获取AuxIdx
        {
            if(st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_bExist && (st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_auxID == AuxUintID))
                break;
        }
        if(AuxIdx == MAXAUXUNIT)
            continue;
        st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_auxAlive = 1;
        if(st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_bShiedCheck == 1)
        {
            //屏蔽limit下载时与LimitCount冲突
            if(Channel_MapAuxInfo::CurrentStepAuxSum[IV_Idx][AuxUintID] == st_AuxInfoRes[IV_Idx].m_CurrentAuxSum[AuxUintID])  //aux id sum compare
            {
                st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_askedCount = 0;
                st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_errorCount = 0;
                st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_startAskFlag = 0;//询问标志（代表恢复normal状态)
            }
            continue;
        }
        /////////Aux Count Compare///////////////
        st_AuxInfoRes[IV_Idx].m_CurrentAuxSum[AuxUintID] = (Msg->Un_Content.m_AuxStatus.AuxInfo >> AuxInfoShiftBit[IV_Idx]) & 0x3f;
        if(Channel_MapAuxInfo::CurrentStepAuxSum[IV_Idx][AuxUintID] == st_AuxInfoRes[IV_Idx].m_CurrentAuxSum[AuxUintID])
        {
            st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_askedCount = 0;
            st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_errorCount = 0;
            st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_startAskFlag = 0;//启动询问标志（代表恢复normal状态)
        }
        else   // 需要发送报文确认
        {
            st_IV2AuxInfoMgr[IV_Idx][AuxIdx].m_errorCount++;
            //if(st_IV2AuxInfoMgr[IV_Idx][AuxIdx].ErrorCount >=4)
            //  dbg_AuxRptSum = Msg->Un_Content.m_AuxStatus.AuxInfo;
        }
    }
    return 0;
}
/***********************************************
**  function：Detect aux status information
**  and check aux CAN net is alive
***********************************************/
void CANBusParse::IV_DoAuxStatusCheck(unsigned char IV_Idx)
{
#if (USE_AUX_STATUS_REPORT == 1)
    for(unsigned char uc_AuxIdx = 0; uc_AuxIdx < MAXAUXUNIT; uc_AuxIdx++)
    {
        if(st_IV2AuxInfoMgr[IV_Idx][uc_AuxIdx].m_bExist != 1)
            return; //continue zyx 180828 严重拖慢任务调度次数，约拖慢一倍
        if(CommonFunc::GetTimeTicks( &st_IV2AuxInfoMgr[IV_Idx][uc_AuxIdx].m_checkAuxInfoTick) > IV_CHECK_AUX_TICK)  //超过200ms查询辅助状态
        {
            CommonFunc::GetTimeTicksUpdate(&st_IV2AuxInfoMgr[IV_Idx][uc_AuxIdx].m_checkAuxInfoTick);	 // reset counter
            IV_CheckCANConnected(IV_Idx, uc_AuxIdx);
            IV_CheckAuxCount(IV_Idx, uc_AuxIdx);
        }
        //if(st_IV2AuxInfoMgr[IV_Idx][uc_AuxIdx].AuxStatus == 0)
        //	st_IV2AuxInfoMgr[IV_Idx][uc_AuxIdx].AuxStatus  = AUX_STATUS_NORMAL;  //此状态下可以启动实验
    }
#endif
}
/***********************************************
**  function：check aux status information
**  judgement it's correct
**  allowed error count is 4 times
***********************************************/
void CANBusParse::IV_CheckAuxCount(unsigned char IV_Idx, unsigned char AuxIdx) //
{
    St_ScheduleMessage SchMessage;
    ST_IVAskTable * p_Mgr = &st_IV2AuxInfoMgr[IV_Idx][AuxIdx];
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    if(p_Mgr->m_errorCount >= MAX_AUX_STATUS_ERR_COUNT && !p_Mgr->m_startAskFlag) //count数错误超过4，且没启动iv询问
    {
        p_Mgr->m_startAskFlag = 1;
        CommonFunc::GetTimeTicksUpdate(&p_Mgr->m_IVAskTick);	 // reset counter
    }
    //发起询问
    if(p_Mgr->m_startAskFlag)
    {
        if(p_Mgr->m_askedCount >= 2) //
        {
            //return;
            p_Mgr->m_errorCount = 0;
            p_Mgr->m_askedCount = 0;
            if( (p_Mgr->m_auxStatus & AUX_COUNT_ERROR) == AUX_COUNT_ERROR)
                return;  //已经进入count错误状态 ,无需再发送msg
            p_Mgr->m_auxStatus |= AUX_COUNT_ERROR;  //转入count_error错误状态
            SchMessage.MsgNumber = AUX_Unsafe_Msg;
            SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
            SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
            SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_STATUS_ERROR;
            SchMessage.st_status_sch_running.m_btLost_Unit = p_Mgr->m_auxID;
            ChannelInfo_Manager::ChannelInfo[IV_Idx].m_UnsafeType = AUX_UNSAFE_TYPE;
            ChannelInfo_Manager::ChannelInfo[IV_Idx].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
            safety_check::SetUnsafe(IV_Idx,AUX_STATUS_ERROR,0, AUX_UNSAFE_TYPE);
            Schedule_Manager_task::UnsafeContent[IV_Idx] = SchMessage;
            Schedule_Manager_task::SendMsgToSchedule(IV_Idx, SchMessage); //停止对应的IV通道
        }
        else if(CommonFunc::GetTimeTicks(&p_Mgr->m_IVAskTick) > 300)
        {
            if( (p_Mgr->m_auxStatus & AUX_COUNT_ERROR) == AUX_COUNT_ERROR)
                return;  //已经进入count错误状态 ,无需再发送询问
            p_Mgr->m_askedCount++;	//询问次数累加
            //unsigned char IV_chnIdx = p_Mgr->IV_ChnNo;
            //IV_LaunchStatusAsk(IV_chnIdx,AuxUint,Channel_MapAuxInfo::CurrentStepAuxLimit[IV_chnIdx][AuxIdx].StepID);
            CommonFunc::GetTimeTicksUpdate(&p_Mgr->m_IVAskTick);	 // reset counter
        }
    }
    else
    {
        p_Mgr->m_auxStatus &= AUX_COUNT_NORMAL; //恢复正常count状态
    }
}
/***********************************************
** Check whether the information is uploaded
** Allowed error count is ten ten times
***********************************************/
void CANBusParse::IV_CheckCANConnected(unsigned char IV_Idx, unsigned char AuxIdx)
{   //判断是否失去连接
    St_ScheduleMessage SchMessage;
    ST_IVAskTable * p_Mgr = &st_IV2AuxInfoMgr[IV_Idx][AuxIdx];
    if(p_Mgr->m_auxID ==CANFunc::SourceBoardID)
        return;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    if(p_Mgr->m_auxAlive)
    {
        p_Mgr->m_lostCnt = 0;
        p_Mgr->m_auxAlive = 0;
        p_Mgr->m_auxStatus &= AUX_CAN_CONNECT_NORMAL;//恢复正常状态
    }
    else
        p_Mgr->m_lostCnt++;
    if(p_Mgr->m_lostCnt >= MAX_CAN_LOST_COUNT) //lostCnt*0.1S
    {
        p_Mgr->m_lostCnt = 0;
        if(p_Mgr->m_auxStatus & AUX_CAN_DISCONNECTED)
            return;  //已经进入连接失败状态 ，无需再发送msg
        SchMessage.MsgNumber = AUX_Unsafe_Msg;
        unsigned char status = p_Mgr->m_auxStatus | AUX_CAN_DISCONNECTED;  //转入CAN连接失败状态
		if(Schedule_Manager_task::IfSchRunning(IV_Idx))
		{
        p_Mgr->m_auxStatus = status;
        SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
        SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
        SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_CAN_LOST;
        SchMessage.st_status_sch_running.m_btLost_Unit = p_Mgr->m_auxID;
        ChannelInfo_Manager::ChannelInfo[IV_Idx].m_UnsafeType = AUX_UNSAFE_TYPE;
        ChannelInfo_Manager::ChannelInfo[IV_Idx].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
        safety_check::SetUnsafe(IV_Idx,AUX_CAN_LOST,0, AUX_UNSAFE_TYPE);
        Schedule_Manager_task::UnsafeContent[IV_Idx] = SchMessage;
        Schedule_Manager_task::SendMsgToSchedule(IV_Idx, SchMessage); //停止aux Unit对应的所有IV通道
		}
    }
}


//要确保MAP信息下载OK方能运行！！！
void CANBusParse::IV_InitStatusMgr()
{
    unsigned char validIdx = 0;
    unsigned char AuxUnitID;

    memset((void *)&st_IV2AuxInfoMgr, 0, sizeof(ST_IVAskTable)*MAXCHANNELNO * MAXAUXUNIT);    //  20150925 ying

    for(unsigned char IV_Chn = 0; IV_Chn < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; IV_Chn++)
    {
        validIdx = 0;
        AuxUnitID = 0;
        //LastAuxUnitID = 0;
        AuxInfoShiftBit[IV_Chn] = 6 * (IV_Chn % 8);
        st_AuxInfoRes[IV_Chn].m_IVChnExist = 1;
        for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType++)
        {
            if(Channel_MapAuxInfo::m_AUXmap[IV_Chn][AuxType].m_AuxUnitCfg.m_Total_Aux_Unit == 0) //empty
            {
                // 				 for(unsigned char idx = 0;idx<MAXAUXUNIT;idx++)
                // 				 {
                // 					 if(st_IV2AuxInfoMgr[IV_Chn][idx].m_bExist &&(st_IV2AuxInfoMgr[IV_Chn][idx].m_auxID) )  //clear this auxtype's map
                // 					 {
                // 						 st_IV2AuxInfoMgr[IV_Chn][idx].m_bExist = 0;     //disable aux status check
                // 						 st_IV2AuxInfoMgr[IV_Chn][idx].m_auxID = 0;
                // 						 st_IV2AuxInfoMgr[IV_Chn][idx].m_auxAlive = 1;
                // 					 }
                // 				 }
                continue;
            }
            // Aux channel has been mapped
            bool bInvalid = false;
            for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
            {
								bInvalid = false; // zyx 2112291
                AuxUnitID = Channel_MapAuxInfo::m_AUXmap[IV_Chn][AuxType].m_AuxUnitCfg.m_Aux_UnitID[AuxIdx];
                if(AuxUnitID == 0)
                    bInvalid = true;
                for(unsigned char oldIdx = 0; oldIdx < validIdx; oldIdx++)
                {
                    if(AuxUnitID == st_IV2AuxInfoMgr[IV_Chn][oldIdx].m_auxID)
                    {
                        bInvalid = true;
                        break;
                    }
                }
                if(!bInvalid)  //辅助unit不能为0
                {
                    // LastAuxUnitID = AuxUnitID;
                    st_IV2AuxInfoMgr[IV_Chn][validIdx].m_bExist = 1;   //初始化auxUnit存在表
                    st_IV2AuxInfoMgr[IV_Chn][validIdx].m_auxID = AuxUnitID;
                    //st_IV2AuxInfoMgr[IV_Chn][validIdx].m_auxType[AuxType] = AuxType;
                    st_IV2AuxInfoMgr[IV_Chn][validIdx].m_auxAlive = 1;
                    st_IV2AuxInfoMgr[IV_Chn][validIdx].m_auxStatus = AUX_STATUS_NORMAL;
                    CommonFunc::GetTimeTicksUpdate(&st_IV2AuxInfoMgr[IV_Chn][validIdx].m_checkAuxInfoTick);
                    validIdx++;
                }
                // else
                //	 st_IV2AuxInfoMgr[IV_Chn][validIdx].m_bExist = 0;
            }
        }
    }
}

//void CANBusParse::IV_LaunchStatusAsk(unsigned char IV_ChnIdx,unsigned char AuxUint,unsigned short StepID)
//{
//	MP_CAN_MSG AskStatusMsg;
//	unsigned char m_CANType = CAN1;//xx
//	AskStatusMsg.Un_CANID.m_CANID.DestID = AuxUint;
//	AskStatusMsg.Un_CANID.m_CANID.CmdType = AUX_CMD_TYPE;
//	AskStatusMsg.Un_CANID.m_CANID.Cmd =  CAN_CMD_ASK_STATUS;
//	AskStatusMsg.Un_Content.m_AskStatus.IV_ChnNo = IV_ChnIdx;//1 byte
//	AskStatusMsg.Un_Content.m_AskStatus.IV_AskFlag = 1;   //IV 重启 辅助未重启，limit处理？？？
//
//	//AskStatusMsg.Un_Content.m_AskStatus.SafetyCount = ChannelInfo_Manager::CurrentSafetyCount[IV_ChnIdx][MAXUNITID];
//	CANBusParse::Send_CAN_Cmd(m_CANType,&AskStatusMsg);
//}

int CANBusParse::IV_IfAuxNormal(unsigned char uc_ChannelNo)
{
#if(USE_AUX_STATUS_REPORT == 1)
    for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
    {
        if(!CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bExist)
            continue;
        if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_auxStatus != AUX_STATUS_NORMAL)
        {
            // St_ScheduleMessage SchMessage;
            memset((void *)&EthernetParse::m_ReportStatus, 0, sizeof(ST_STATUS_SCHEDULE));
            EthernetParse::m_ReportStatus.m_btCMD = PC_Unsafe_Msg;
            Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
            Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
            if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_auxStatus & AUX_CAN_DISCONNECTED)
                Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = AUX_CAN_LOST;
            else if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_auxStatus & AUX_COUNT_ERROR)
                Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = AUX_STATUS_ERROR;
            else
                Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code = AUX_CAN_LOST; //???
            Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_btLost_Unit = st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_auxID;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = AUX_UNSAFE_TYPE;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code;
            safety_check::SetUnsafe(uc_ChannelNo,Schedule_Manager_task::UnsafeContent[uc_ChannelNo].st_status_sch_running.m_uw32Safe_Code,0, AUX_UNSAFE_TYPE);
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
            Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_AUXACKFAILINSTART;
            //EthernetParse::MPReportData(uc_ChannelNo,SEND_EVENT_Un_Content);
            return 0;  //abnormal
        }

    }
#endif
    return 1; //Normal
}

bool CANBusParse::IV_CheckAuxSNRes(unsigned char uc_ChannelNo, unsigned char SNtype)  //AUX SN检查函数
{
    for(unsigned char uc_i = 0; uc_i < st_SNCheckTab[uc_ChannelNo][SNtype].m_SNCount; uc_i++)
    {
        if( st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_Checked)
            continue;

        for(unsigned char AckSN = 0; AckSN < st_SNResTable[uc_ChannelNo][SNtype].m_SNCount; AckSN++)
        {
            if(st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_SN == st_SNResTable[uc_ChannelNo][SNtype].m_SNInfo[AckSN].m_SN)
            {
                if(st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_IfAnyLimit)
                {
                    if(st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_AuxUintBit == st_SNResTable[uc_ChannelNo][SNtype].m_SNInfo[AckSN].m_AuxUintBit)
                    {
                        st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_Checked = 1;
                        st_SNCheckTab[uc_ChannelNo][SNtype].m_CheckedCount++;
                        st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_IfAnyLimit = 0;
                        break;
                    }
                    else
                        continue;
                }
                else
                {
                    st_SNCheckTab[uc_ChannelNo][SNtype].m_CheckedCount++;
                    st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[uc_i].m_Checked = 1;
                    break; //找到即跳出
                }
            }
        }
    }
    if(st_SNCheckTab[uc_ChannelNo][SNtype].m_CheckedCount == st_SNCheckTab[uc_ChannelNo][SNtype].m_SNCount) // SN检查OK
    {
        if(SNtype == LIMITSN)
        {
            //安全检查屏蔽
            for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
                if( st_IV2AuxInfoMgr[uc_ChannelNo][AuxIdx].m_bExist)
                    st_IV2AuxInfoMgr[uc_ChannelNo][AuxIdx].m_bShiedCheck = 0;
        }
        return 1;   //ok
    }
    else
        return 0;   //failed
}


bool CANBusParse::IV_ClrAuxLimitRes(unsigned char uc_ChannelNo)
{
    //aux clear limit状态检查
    unsigned char bAllReceive = 1;
    for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
    {
        if(st_ClrLimitAckMgr[uc_ChannelNo].m_AuxInfo[AuxIdx].m_bValid)
            if(st_ClrLimitAckMgr[uc_ChannelNo].m_AuxInfo[AuxIdx].m_Check == CLRLIMIT_WAIT) //¼ÌÐøµÈ´ý
                bAllReceive = 0;
    }

    if(bAllReceive)  //Collected all SN
        return true;
    else
        return false;
}

void CANBusParse::IV_ClearAuxSNList(unsigned char uc_ChannelNo, unsigned char SNtype)
{
    //Aux download limit相关信息Reset
    memset(&st_SNCheckTab[uc_ChannelNo][SNtype].m_SNInfo[0], 0, sizeof(St_SNInfo)*SN_LENGTH);
    memset(&st_SNResTable[uc_ChannelNo][SNtype].m_SNInfo[0], 0, sizeof(St_SNInfo)*SN_LENGTH);

    for(unsigned char uc_j = 0; uc_j < MAXIFANYLIMIT; uc_j++) //  20140218  Zhang fix limit bug;
        st_SNResTable[uc_ChannelNo][SNtype].m_stAnyLimit.m_SNInfo[uc_j].m_SN = 0;
    st_SNCheckTab[uc_ChannelNo][SNtype].m_CheckedCount = 0;
    st_SNCheckTab[uc_ChannelNo][SNtype].m_SNCount = 0;  //
    st_SNResTable[uc_ChannelNo][SNtype].m_SNCount = 0;
    st_SNResTable[uc_ChannelNo][SNtype].m_stAnyLimit.m_Cnt = 0;  //

}
//------------------------------end of IV Recv Command Function-----------//

#endif
////////////////////////////////////CANAUX_BROADCAST START////////////////
void CANBusParse::CANBMS_BroadcastAuxMsg(void)
{
    static unsigned char MessageIndex = 0;
    if(MessageIndex>=m_CANAUX_Broadcast.m_Total_Message)
        MessageIndex = 0;
    for(; MessageIndex<m_CANAUX_Broadcast.m_Total_Message; MessageIndex++)
    {
        St_CANAUX_Broadcast_MSG_Info  *CANAUX_Brst_Mess = &m_CANAUX_Broadcast.m_AuxBroadcast_Message[MessageIndex];
        if(CANAUX_Brst_Mess->m_bValid == 0)
            continue;
        if((CANAUX_Brst_Mess->m_bUpdated == 1)||((CommonFunc::GetTimeTicks(&CANAUX_Brst_Mess->m_CurrentTick)>CANAUX_Brst_Mess->m_Period*10)))
        {
            MP_CAN_MSG AuxBroadcastMessage;
						memset(&AuxBroadcastMessage,0,sizeof(AuxBroadcastMessage));
            AuxBroadcastMessage.DLC = CANAUX_Brst_Mess->m_DLC;
            AuxBroadcastMessage.FrameType = CANAUX_Brst_Mess->m_FrameType;
            AuxBroadcastMessage.Un_CANID.m_U32ID  = CANAUX_Brst_Mess->m_U32ID;
#if(USE_AS_AUXMCU == 1)   //aux用
            AuxBroadcastMessage.Un_Content.m_BMSBrdCast.m_Data1 = Aux_DataManager::m_AuxDataBank[CANAUX_Brst_Mess->m_DataGroup1][CANAUX_Brst_Mess->m_Chan1].aux_data;
            AuxBroadcastMessage.Un_Content.m_BMSBrdCast.m_Data2 = Aux_DataManager::m_AuxDataBank[CANAUX_Brst_Mess->m_DataGroup2][CANAUX_Brst_Mess->m_Chan2].aux_data;
#else
            AuxBroadcastMessage.Un_Content.m_BMSBrdCast.m_Data1 = BuiltIn_DataManager::m_BuiltInDataBank[CANAUX_Brst_Mess->m_DataGroup1][CANAUX_Brst_Mess->m_Chan1].aux_data;
            AuxBroadcastMessage.Un_Content.m_BMSBrdCast.m_Data2 = BuiltIn_DataManager::m_BuiltInDataBank[CANAUX_Brst_Mess->m_DataGroup2][CANAUX_Brst_Mess->m_Chan2].aux_data;
#endif
            if(CANAUX_Brst_Mess->m_Endian == e_BigOrder)
            {
                unsigned int EndianLow,EndianHigh = 0;
                EndianLow = AuxBroadcastMessage.Un_Content.LLData&0xFFFFFFFF;	// 8 Byte get 4 low Byte
                EndianHigh = (AuxBroadcastMessage.Un_Content.LLData>>32)&0xFFFFFFFF;	// 8 Byte get 4 high Byte
                AuxBroadcastMessage.Un_Content.LLData = __REV(EndianHigh);  // Reverse, 32bit---0bit exchange 0bit---32bit
                AuxBroadcastMessage.Un_Content.LLData = ( AuxBroadcastMessage.Un_Content.LLData <<32)|__REV(EndianLow);	// 8 Byte high 4 Byte, low 4 Byte;
            }
            CANFunc::CAN_Push_to_Send(CANAUX_Brst_Mess->m_CAN_Port,&AuxBroadcastMessage);
            CommonFunc::GetTimeTicksUpdate(&CANAUX_Brst_Mess->m_CurrentTick);//更新间隔时间
            CANAUX_Brst_Mess->m_bUpdated = 0;
            return;
        }
    }
}
// #if(USE_AS_AUXMCU ==1)  //
// bool CANBusParse::ForwardFDCANRxData(ST_CAN_RxMSG OrgMsg)
// {
//     MP_MCU_MSG Temp_MCU_Msg;
//     Temp_MCU_Msg.Un_CANID.m_CANID.DestID = CANBusParse::m_CANBMS_AuxInfo.m_IVCANID;//Aux MCU ID /IV MCU ID
//     Temp_MCU_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//     Temp_MCU_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
//     if(CANBusParse::st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
//         CANBusParse::st_CAN_SN.m_CANBMS_SN++;
//     else
//         CANBusParse::st_CAN_SN.m_CANBMS_SN = 0;
//     Temp_MCU_Msg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
//     Temp_MCU_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_BMS_MSG;//FDCAN_CMD_FORWARD_MSG;
//     Temp_MCU_Msg.Un_Content.m_BMSData.m_SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
//     Temp_MCU_Msg.Un_Content.m_BMSData.CANid = OrgMsg.RxID;
//     memcpy((void*)&Temp_MCU_Msg.Un_Content.m_BMSData.CANdata.ucCANData[0] , (void*)&OrgMsg.RxData.uc_Data[0], sizeof(CANDATA));
//     Temp_MCU_Msg.ByteLength = 4+sizeof(UN_CANID) + sizeof(ST_CANMsg);
//     MCUCommun::NET_Push_to_Send(Temp_MCU_Msg);
// }
// #endif
/************************************************************************/
/**                                      can bms                       **/
/************************************************************************/
//#if(SUPPORT_CAN_BMS == 1)
/******************************************************
** Module name:      CANBMS_ProcessOrigMsg(IV &Aux)
** Descriptions:		used in CAN rx interrupt func
**                   IV: assemble Msg
**                   Aux:set baudrate and assemble Msg
**Created Date:      2014/08/05
*******************************************************/
bool CANBusParse::CANBMS_ProcessOrigMsg(unsigned char CANx,ST_CAN_RxMSG OrgMsg)
{

#if(USE_AS_AUXMCU ==1)  //
    if(CANx == CAN2)   //CAN2(BMS_ to CAN1)
    {
        //    ForwardFDCANRxData(OrgMsg);
        CANBMS_ForwardMsg(CAN1,&OrgMsg);
        return true;
    }
#else
    if(!CANBusParse::CANBMS_bMap)
        return true;
    //Built_in CANBMS  Start//
    //if((CheckCAN_Port_Property(CANx) ==	USE_AS_BUILT_IN_CANBMS)&&(NetWork==MCU_TO_MCU_CAN))//如果用在builtinCANBMS直接放进缓存
    if(CheckCAN_Port_Property(CANx) ==	USE_AS_BUILT_IN_CANBMS)//如果用在builtinCANBMS直接放进缓存
    {
        unsigned char chan;
        chan = CANBMS_GetChanOfCANx(CANx);
        if(chan != 0xFF)
            CANBMS_IVAssembleBuiltInMsgs(CANx, chan, &OrgMsg);
        return true;
    }
    //Built_in CANBMS  End//
#endif
    UN_CANID CANID;
    CANID.m_U32ID = OrgMsg.RxID;

    if(CANID.m_CANID.CmdType == BMS_CMD_TYPE)
    {
#if(USE_AS_AUXMCU ==0)  //IV
        CANBMS_IVAssembleMsgs(CANx, &OrgMsg);
#else  //aux
        CANBMS_AuxAssembleMsgs(CANx, &OrgMsg);
#endif
        return true;
    }

    return false;
}



#if(USE_AS_AUXMCU ==0)

//原则，先删除解除绑定的config（ m_CANConfig.m_buf要加入绑定Idx = -1表示无绑定 还要加入一个timestamp表示解绑时间
//然后没有的话，则add进config buffer
//通过m_CANConfig.m_buf[bufidx]的m_RegisterBit 知道该bufIdx被哪个IV通道注册过
bool CANBusParse::Assign_CANConfig(unsigned char uc_ChannelNo)
{
    //最先解绑对应的IV通道的buffer
    int oldidx = m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;  //全部初始化为-1

    for(unsigned char idx = 0; idx < m_CANConfig.m_bufCnt; idx++) //首先判断是否有此MD5的config
    {
        if(0 == memcmp((void*)&m_CANAssignMD5[uc_ChannelNo].m_MD5, (void*)&m_CANConfig.m_buf[idx].m_MD5, 16))//find this config
        {
            //			  if(oldidx<0) //有，但无任何绑定
            //				  break;
            if(oldidx != idx) //需要更新绑定
            {
                if(oldidx > 0) //如果以前没有assign，就不存在跟旧的解绑
                {
                    m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1 << uc_ChannelNo); //跟旧config解绑
                    m_CANConfig.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
                }
                m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = idx;  //重新绑定
                m_CANAssignMD5[uc_ChannelNo].m_insertPos = idx;
                m_CANConfig.m_buf[idx].m_RegisterBit |= 1 << uc_ChannelNo; //绑定
            }
            else  //表明与之前绑定一致
            {
                m_CANConfig.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time; //可以不要
            }

            int cfgidx = m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
            for(unsigned char i = 0; i < BMS_BROAD_CST_COUNT; i++)
            {
                St_BMS_SendUnit * p_BrdCstBuf = &m_BMS_BroadCstBuf[uc_ChannelNo][i];
                m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_bBrdCstUpdate = 1;
                if(cfgidx >= 0 && cfgidx <= MAX_LEN_CAN_CONFIG)
                {

                    p_BrdCstBuf->m_bUpdated = 1;  //updateflag == this msg is needed send 3 can msg
                    p_BrdCstBuf->m_CANMsg.FrameType = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_FrameType;
                    p_BrdCstBuf->m_CANMsg.DLC = 8;
                    p_BrdCstBuf->m_CANMsg.Un_CANID.m_U32ID = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCst_ID;
                    p_BrdCstBuf->m_bBroadcast = 1;
                    p_BrdCstBuf->m_Period = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCst_interval;
                    p_BrdCstBuf->m_SignalStop = 0;
// 					for(unsigned j = 0;j<2;j++)
// 					{
// 						p_BrdCstBuf->m_Data_Mode[j] = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_DataMode[j];
// 						p_BrdCstBuf->m_BrdCstData[j] = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCstData[j];
// 					}
                    memcpy(&p_BrdCstBuf->m_Data_Mode[0], &m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_DataMode[0], 2);
                    memcpy(&p_BrdCstBuf->m_BrdCstData[0], &m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCstData[0], 2 * sizeof(float));

                    if(!p_BrdCstBuf->m_Period )  //if period = 0
                        p_BrdCstBuf->m_bValid = 0;
                    else
                    {
                        p_BrdCstBuf->m_bValid = 1;
                        p_BrdCstBuf->m_bUpdated = 1;//update send
                    }
                    CommonFunc::GetTimeTicksUpdate(&p_BrdCstBuf->m_CurrentTick);
                }
            }
            m_BroadCstIdex[uc_ChannelNo] = 0;
            //memset((void*)&m_CANMetaVariable[uc_ChannelNo][0],0,sizeof(ST_BMS_MetaValue)*MAX_CAN_BMS_SIGNAL);
// 			if(CANBMS_IsUseBuiltInCAN(uc_ChannelNo))
// 			 CANBMS_setBroadcastWithBuiltInCAN(uc_ChannelNo);
// 			else
// 			 CANBMS_setBroadcastWithAuxBorad(uc_ChannelNo);
            //Built_in CANBMS  End//
            ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive = true; //180518 zyx
            CANBusParse::CheckCanBmsMap();
            CANBMS_SetBaudRate(uc_ChannelNo);
            //CANBusParse::CANBMS_StopAllSend(uc_ChannelNo);
            return true;
        }
    }
    // 该MD5码的CANCONFIG 不存在
    if(oldidx >= 0)
    {
        m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1 << uc_ChannelNo); //跟旧config解绑
        m_CANConfig.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
    }
    if(m_CANConfig.m_bufCnt < MAX_LEN_CAN_CONFIG) //无绑定
    {
        unsigned char myIdx = m_CANAssignMD5[uc_ChannelNo].m_insertPos = CANBusParse::m_CANConfig.m_bufCnt++;  // 表示下一次存入的位置是此位置
        m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = myIdx;  //
        memcpy(& m_CANConfig.m_buf[myIdx].m_MD5, &m_CANAssignMD5[uc_ChannelNo].m_MD5, sizeof(St_MD5)); //给对应config存入MD5
        m_CANConfig.m_buf[myIdx].m_RegisterBit |= 1 << uc_ChannelNo; //绑定
				m_CANConfig.m_buf[m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx].m_bEnable = 0;
				
        return false;  //表示要求下载
    }
    else
    {   //不存在，且缓冲满了，则尝试删除buffer中的config
        unsigned char oldestIdx = 0;
        for(unsigned char idx = 1; idx < MAX_LEN_CAN_CONFIG; idx++)
        {
            if(m_CANConfig.m_buf[idx].m_RegisterBit == 0) //先查找无绑定的(或已全部解绑的）因为 20>16,总有一个缓冲是空的
                if(memcmp((void*)&m_CANConfig.m_buf[idx].m_AbondonTime.Second, (void*)&m_CANConfig.m_buf[oldestIdx].m_AbondonTime.Second, 8) < 0)
                    oldestIdx = idx;   //最小的os time
        }
        m_CANAssignMD5[uc_ChannelNo].m_insertPos = oldestIdx;
        m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = oldestIdx;  //
        memcpy(&m_CANConfig.m_buf[oldestIdx].m_MD5, &m_CANAssignMD5[uc_ChannelNo].m_MD5, sizeof(St_MD5)); //给对应config存入MD5
        m_CANConfig.m_buf[oldestIdx].m_RegisterBit |= 1 << uc_ChannelNo; //绑定
				m_CANConfig.m_buf[m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx].m_bEnable = 0;

        return false;
    }


}

bool CANBusParse::Clear_CANConfig(unsigned char uc_ChannelNo)  //Add by DKQ 01.04.2015
{
    int oldidx = m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1 << uc_ChannelNo); //跟旧config解绑
    m_CANConfig.m_buf[oldidx].m_AbondonTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
    m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = -1;
    for(unsigned char i = 0; i < BMS_BROAD_CST_COUNT; i++)
    {
        m_BMS_BroadCstBuf[uc_ChannelNo][i].m_bValid = 0;
        m_BMS_BroadCstBuf[uc_ChannelNo][i].m_Period = 0;
        m_BMS_BroadCstBuf[uc_ChannelNo][i].m_bUpdated = 0;
    }
    if(!CANBMS_IsUseBuiltInCAN(uc_ChannelNo))//Built_in CANBMS  Start////Built_in CANBMS  End//
        CANBMS_ClearBMS(uc_ChannelNo);
    m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll = 1;
    ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive = false; //180518 zyx
		m_BMS_OverTime[uc_ChannelNo].m_BMSStatus = e_BMS_Normal; //20210617 yzl
    CANBusParse::CheckCanBmsMap();
    return true;
}


MetaItemsBufAddr CANBusParse::GetMetaItemsBufAddr(int cfgIdx, unsigned char Interval) //170606 zyx
{
    MetaItemsBufAddr BufAddr;
    BufAddr.MetaItemsBufFrom = 0;
    BufAddr.MetaItemsBufTo = 0;

    switch(Interval)
    {
    case INTERVAL_500MS:
        BufAddr.MetaItemsBufFrom = 0;
        BufAddr.MetaItemsBufTo = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.FastItems;
        break;
    case INTERVAL_1S:
        BufAddr.MetaItemsBufFrom = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.FastItems;
        BufAddr.MetaItemsBufTo = BufAddr.MetaItemsBufFrom + CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.MidItems;
        break;
    case INTERVAL_30S:
        BufAddr.MetaItemsBufFrom = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.MidItems + CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.FastItems;
        BufAddr.MetaItemsBufTo = BufAddr.MetaItemsBufFrom + CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.SlowItems;
        break;
    case NO_DATALOG:
        BufAddr.MetaItemsBufFrom = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.SlowItems + CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.MidItems + CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.FastItems;
        BufAddr.MetaItemsBufTo = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.TotalItems;
        break;
    default:
        BufAddr.MetaItemsBufFrom = 0;
        BufAddr.MetaItemsBufTo = CANBusParse::m_CANConfig.m_buf[cfgIdx].m_MetaCount.TotalItems;
        break;
    }
    return BufAddr;
}


// CAN BMS解析函数
/******************************************************
** Module name:      CANBMS_IVParseMsg
** Descriptions:		parse CAN BMS pack from Aux Unit
**
**Created Date:      2014/08/05
*******************************************************/
void CANBusParse::CANBMS_IVParseMsg(unsigned char CANx, const MP_CAN_MSG * Parse_Msg)
{
    //这里应该转换成chn
    unsigned char IV_Chn = Parse_Msg->m_ChnNo;
    float fMetaValue = 0.0f;
    int cfgIdx = m_CANAssignMD5[Parse_Msg->m_ChnNo].m_cfgBufIdx;


    if(cfgIdx < 0 && cfgIdx >= MAX_LEN_CAN_CONFIG)
        return;
    else if(! m_CANConfig.m_buf[cfgIdx].m_bEnable)
        return;

    m_BMS_OverTime[IV_Chn].m_bCANBMSAlive = true;   // detect if have BMS msg
    CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[IV_Chn].m_UnsafeTime);
    Un_Data OrigMetaValue;

    //----function A  MetaTab ID is ascending order-----//
    for(unsigned char Interval = NO_DATALOG; Interval < INTERVAL_Total; Interval++)
    {
        MetaItemsBufAddr MetaItemsBuf = GetMetaItemsBufAddr(cfgIdx, Interval); //170606 zyx
        int IDIdx = CANBMS_binarySearchCfg(cfgIdx, Parse_Msg->Un_CANID.m_U32ID, MetaItemsBuf.MetaItemsBufFrom, MetaItemsBuf.MetaItemsBufTo-1);//1910161 zyx
        //get first idx
        if(IDIdx < MetaItemsBuf.MetaItemsBufFrom || IDIdx >= MetaItemsBuf.MetaItemsBufTo)
            continue;  //return; 170616 zyx

        int firstIDIdx = 0;
        while(Parse_Msg->Un_CANID.m_U32ID == m_CANConfig.m_buf[cfgIdx].m_ValidSignal[IDIdx].m_CANMessageID)	 //find first ID_Idx
        {
            firstIDIdx = IDIdx--;
            if(firstIDIdx <= MetaItemsBuf.MetaItemsBufFrom)
                break;
        }
        if(firstIDIdx >= MetaItemsBuf.MetaItemsBufTo)
            continue;  //return; 170616 zyx

        St_CANBMS_ParsedMeta * p_Signal = &m_CANConfig.m_buf[cfgIdx].m_ValidSignal[firstIDIdx];
        while(Parse_Msg->Un_CANID.m_U32ID == p_Signal->m_CANMessageID)	 //同一ID可能有几个metavalue
        {   //20150215,modified by hepeiqing
            OrigMetaValue = CANBMS_endianParse(&Parse_Msg->Un_Content, cfgIdx, firstIDIdx);
            if(p_Signal->m_DataType == eType_Float)         //float    data
                fMetaValue = OrigMetaValue.fData[0] * p_Signal->m_ScaleFactor + p_Signal->m_Offset;
            else if(p_Signal->m_DataType == eType_Signed)   //signed   data
            {
                signed long long MySignedData = OrigMetaValue.LLData;
                unsigned char    BitLength = 0;
                if(p_Signal->m_EndDataIndex > p_Signal->m_StartDataIndex)
                    BitLength = p_Signal->m_EndDataIndex - p_Signal->m_StartDataIndex + 1;
                else
                    BitLength = p_Signal->m_StartDataIndex - p_Signal->m_EndDataIndex + 1;
                BitLength = 64 - BitLength;

                MySignedData = OrigMetaValue.LLData;
                MySignedData <<= BitLength;
                MySignedData >>= BitLength;

                fMetaValue = MySignedData * p_Signal->m_ScaleFactor + p_Signal->m_Offset;
            }
            else                                                                     //unsigned data
                fMetaValue = OrigMetaValue.LLData * p_Signal->m_ScaleFactor + p_Signal->m_Offset;

            ChannelInfo_Manager::Set_CANBMSMetaVariable(IV_Chn, p_Signal->m_CAN_RX_Meta, fMetaValue);
            firstIDIdx++;
            p_Signal = &m_CANConfig.m_buf[cfgIdx].m_ValidSignal[firstIDIdx];
        }
    }
}
/*********************************************************
** Module name:      CANBMS_IVDoWork
** Descriptions:		do CAN BMS safety check & ack Check
**                  and msg send (just send thrid msg)
**Created Date:      2014/9/10
**********************************************************/
void CANBusParse::CANBMS_IVDoWork()
{
    for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
    {
        //Built_in CANBMS  Start//
        if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive)
            continue;
//        if(CANBMS_IsUseBuiltInCAN(ChnIdx))
//        {
//            CANBMS_MsgSendWithBuiltInCAN(ChnIdx);
//        }
//        else
//            CANBMS_MsgSendWithAuxBorad(ChnIdx);  //send split BMS msg
        CANMBMS_SendMsg( ChnIdx);//1910091 zyx
        //Built_in CANBMS  End//
        int cfgIdx = m_CANAssignMD5[ChnIdx].m_cfgBufIdx;
        if(cfgIdx < 0 || cfgIdx >= MAX_LEN_CAN_CONFIG)
            continue;
        if(! m_CANConfig.m_buf[cfgIdx].m_bEnable)
            continue;

//     if(!CANBMS_IsUseBuiltInCAN(ChnIdx))
// 		CANBMS_DoAckCheck(ChnIdx);
        /////////////////////BMS Signal Lost time out/////////////////////////////////
        if(m_CANConfig.m_buf[cfgIdx].m_UnsafeTimeout == 0)
        {
            m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Normal;
            continue;
        }
        if(CommonFunc::GetTimeTicks(&m_BMS_OverTime[ChnIdx].m_UnsafeTime) >= m_CANConfig.m_buf[cfgIdx].m_UnsafeTimeout)
        {
            // CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[ChnIdx].m_UnsafeTime);
            if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_bIVActive)
            {
                m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Normal;
                continue;
            }
						
						m_BMS_OverTime[ChnIdx].m_bCANBMSAlive = false;		//reset flag
						
						if(m_BMS_OverTime[ChnIdx].m_BMSStatus == e_BMS_Unsafe)//report unsafe
								continue;
						
						if( ChannelInfo_Manager::ChannelInfo[ChnIdx].ScheduleStatus == RUNNING)
						{
								St_ScheduleMessage SchMessage;
								memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
								SchMessage.MsgNumber = Sch_OnUnsafe_Msg;
								SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
								SchMessage.st_status_sch_running.m_bUnsafeType = CANBMS_UNSAFE_TYPE;
								SchMessage.st_status_sch_running.m_uw32Safe_Code = CAN_BMS_UNSAFE;
								SchMessage.st_status_sch_running.m_btLost_Unit = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnIdx].m_AuxUnitID;
								ChannelInfo_Manager::ChannelInfo[ChnIdx].m_UnsafeType = CANBMS_UNSAFE_TYPE;
								ChannelInfo_Manager::ChannelInfo[ChnIdx].m_SafetyStatus = CAN_BMS_UNSAFE;
								safety_check::SetUnsafe(ChnIdx,CAN_BMS_UNSAFE,0, CANBMS_UNSAFE_TYPE);
								Schedule_Manager_task::UnsafeContent[ChnIdx] = SchMessage;
								Schedule_Manager_task::SendMsgToSchedule(ChnIdx, SchMessage); //停止对应的IV通道
						}
						
						m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Unsafe;

						CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_OverTime[ChnIdx].m_UnsafeTime);
						continue;
        }
        else if(m_BMS_OverTime[ChnIdx].m_bCANBMSAlive)
        {
            m_BMS_OverTime[ChnIdx].m_BMSStatus = e_BMS_Normal;
            continue;
        }
    }
    ////////////////////MutliPackets overtime judgement//////////////////////////////
//     for(unsigned char ChnIdx = 0; ChnIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChnIdx++)
//     {
//         if(OS_ServeProcess::GetTimeTicks(&m_BMS_OverTime[ChnIdx].m_SubMsgOvertime) < MAX_MULTI_PACK_OVERTIME)
//             continue;
//         for(unsigned char SNIdx = 0; SNIdx < CAN_MULTI_PACK_BUF_LEN; SNIdx++)
//         {
//             ST_Multi_Pack * p_MultiPack = &CANFunc::m_MutiPackets[ChnIdx][SNIdx];
//             if(p_MultiPack->Status == MULTI_PACK_EMPTY || p_MultiPack->Status == MULTI_PACK_FILL)
//                 continue;
//             if(p_MultiPack->Status & FIRST_PACK_FILL)
//             {
//                 if(OS_ServeProcess::GetTimeTicks(&p_MultiPack->m_FirstPack.m_Overtime) >= MAX_MULTI_PACK_OVERTIME)
//                 {
// 					p_MultiPack->Status |=FIRST_PACK_EMPTY;
//                     p_MultiPack->Status = MULTI_PACK_EMPTY;  // 1705231 zyx
//                     memset(&p_MultiPack->m_FirstPack.m_DataA, 0, sizeof(St_FirstData));
//                     OS_ServeProcess::GetTimeTicksUpdate(&p_MultiPack->m_FirstPack.m_Overtime);  //1707051 zyx
//                 }
//             }
//             if(p_MultiPack->Status & SECOND_PACK_FILL)
//             {
//                 if(OS_ServeProcess::GetTimeTicks(&p_MultiPack->m_SecondPack.m_Overtime) >= MAX_MULTI_PACK_OVERTIME)
//                 {
// 					p_MultiPack->Status |=SECOND_PACK_EMPTY;
//                     p_MultiPack->Status = MULTI_PACK_EMPTY;  // 1705231 zyx
//                     memset(&p_MultiPack->m_SecondPack.m_DataB, 0, sizeof(St_SecondData));
//                     OS_ServeProcess::GetTimeTicksUpdate(&p_MultiPack->m_SecondPack.m_Overtime); //1707051 zyx
//                 }
//             }
//         }
//     }
}

//use aux id to get IV chnNo
unsigned char CANBusParse::CANBMS_GetChn(unsigned char AuxID)
{
    for(unsigned char chnidx = 0; chnidx < MAXCHANNELNO; chnidx++)
    {

        if ( ChannelInfo_Manager::m_CANMapInfo.m_CANmap[chnidx].m_bIVActive && (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[chnidx].m_AuxUnitID == AuxID))
            return ChannelInfo_Manager::m_CANMapInfo.m_CANmap[chnidx].m_IVChnNo;
    }
    return 0xFF; //error

}
/*********************************************************
** Module name:      CANBMS_LogData
** Descriptions:		used in step Mgr's SendLog func
**                   report CAN LogData
**Created Date:      2014/9/10
**********************************************************/
//void CANBusParse::CANBMS_LogData(unsigned char uc_ChannelNo)
//{
//	CommonFunc::GetTimeTicksUpdate(&m_BMS_OverTime[uc_ChannelNo].m_SendIdleDataTick); //update idle data tick
//	EthernetParse::MPReportData(uc_ChannelNo,SEND_LOG_BMS_DATA);
//}
//

/******************************************************
** Module name:      CANBMS_MsgSend
** Descriptions:		IV chnNo
**                   IV MCU Send CAN BMS Msg (from Sch)
**Created Date:      2014/09/09
*******************************************************/
// void CANBusParse::CANBMS_MsgSendWithAuxBorad(unsigned char uc_ChannelNo)
// {
//     St_BMS_SendInfo * p_Mgr = &m_BMS_SendInfoMgr[uc_ChannelNo];
//     CANBMS_setBroadcastWithAuxBorad(uc_ChannelNo);
//     if(m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll)   //send stop all
//     {
//         if(m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt > 0)
//         {
//             CANBMS_StopAllSend(uc_ChannelNo);
//             m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt = 0;
//         }
//     }

//     if((!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive) || (!m_BMS_SendBuf_RunOn[uc_ChannelNo]))
//         return;

//     for (unsigned char idx = 0; idx < m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt; idx++)
//     {
//         St_BMS_SendUnit * p_BMSMsg = &m_BMS_SendBuf[uc_ChannelNo].m_Msg[idx];
//             if(p_BMSMsg->m_OutboundMsgInfoIdx >= 0)//190617,hpq
//                 OutboundMsgPack(uc_ChannelNo,p_BMSMsg->m_OutboundMsgInfoIdx,(void *)p_BMSMsg->m_CANMsg.Un_Content.Data);
//         p_Mgr->m_Period = p_BMSMsg->m_Period;
//         p_Mgr->m_SignalStop = p_BMSMsg->m_SignalStop;
//         p_Mgr->m_AllStop = m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll;  //keep the last stopAll flag
//         p_Mgr->m_bBroadcast = 0;
//         p_Mgr->m_bOnceTime = 0;
//         p_Mgr->m_DestID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
//         p_BMSMsg->m_CANMsg.DLC = p_BMSMsg->m_CANMsg.DLC; //DLC;
//         p_BMSMsg->m_CANMsg.FrameType =  p_BMSMsg->m_CANMsg.FrameType; //Std//ext frame
//         if(p_BMSMsg->m_bUpdated) //this msg is needed send 3 can msg(almost is first time)
//         {
//             p_Mgr->m_SendAll =  1;
//             p_BMSMsg->m_bUpdated = 0;
//         }
//         else
//             p_Mgr->m_SendAll = 0;
//         if(CommonFunc::GetTimeTicks(&p_BMSMsg->m_CurrentTick) > p_BMSMsg->m_Period * 10)
//         {   //190617,hpq
//             p_Mgr->m_SendAll = 1;
//             CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);//更新间隔时间
//         }

//         if((p_Mgr->m_SendAll) || (p_BMSMsg->m_SendTick % 1200 == 0))
//             CANBMS_Split2SendMsg(uc_ChannelNo, &p_BMSMsg->m_CANMsg, m_BMS_SendInfoMgr[uc_ChannelNo]);
//         p_BMSMsg->m_SendTick++;
//     }
// }
// //Built_in CANBMS  Start//
// void CANBusParse::CANBMS_MsgSendWithBuiltInCAN(unsigned char uc_ChannelNo)
// {
//     unsigned char m_CANPort = CANBMS_GetBuiltInCanPort( uc_ChannelNo);
//     if(m_CANPort == 0xFF)
//         return;
//     CANBMS_setBroadcastWithBuiltInCAN(uc_ChannelNo);
//     St_BMS_SendInfo * p_Mgr = &m_BMS_SendInfoMgr[uc_ChannelNo];
//     for (unsigned char idx = 0; idx < m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt; idx++)
//     {
//         St_BMS_SendUnit * p_BMSMsg = &m_BMS_SendBuf[uc_ChannelNo].m_Msg[idx];

//         MP_CAN_MSG *CAN_Msg = &m_BMS_SendBuf[uc_ChannelNo].m_Msg[idx].m_CANMsg;
//         p_Mgr->m_SignalStop = p_BMSMsg->m_SignalStop;
//         p_Mgr->m_AllStop = m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll;
//         if((p_Mgr->m_SignalStop) || (p_BMSMsg->m_Period == 0))
//             continue;
//         if(p_Mgr->m_AllStop)
//             return;

//         if(p_BMSMsg->m_bUpdated)
//         {
//             p_Mgr->m_SendAll =  1;
//             p_BMSMsg->m_bUpdated = 0;
//             CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);//更新间隔时间
//         }
//         else
//         {
//             p_Mgr->m_SendAll = 0;
//         }

//         if((p_Mgr->m_SendAll) || (CommonFunc::GetTimeTicks(&p_BMSMsg->m_CurrentTick) > p_BMSMsg->m_Period * 10))
//         {
//             CANFunc::CAN_Push_to_Send(m_CANPort, CAN_Msg);
//             CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);		//更新间隔时间
//         }
//     }


// }
void CANBusParse::CANMBMS_SendMsg(unsigned char uc_ChannelNo)//zyx 1910091
{
    St_BMS_SendInfo * p_Mgr = &m_BMS_SendInfoMgr[uc_ChannelNo];
    unsigned char m_CANPort = CANBMS_GetBuiltInCanPort( uc_ChannelNo);
    if(CANBMS_IsUseBuiltInCAN(uc_ChannelNo))
    {
        if(m_CANPort == 0xFF)
            return;
        CANBMS_setBroadcastWithBuiltInCAN(uc_ChannelNo);
        if(m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll)   //send stop all
            return;
    }
    else
    {
        CANBMS_setBroadcastWithAuxBorad(uc_ChannelNo);
        if(m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll)   //send stop all
        {
            if(m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt > 0)
            {
                CANBMS_StopAllSend(uc_ChannelNo);
                m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt = 0;
            }
        }
        if((!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive) || (!m_BMS_SendBuf_RunOn[uc_ChannelNo]))
            return;
    }

    for(unsigned char idx = 0; idx < m_BMS_SendBuf[uc_ChannelNo].m_validMsgCnt; idx++)
    {
        St_BMS_SendUnit * p_BMSMsg = &m_BMS_SendBuf[uc_ChannelNo].m_Msg[idx];
        if(p_BMSMsg->m_OutboundMsgInfoIdx >= 0)//190617,hpq
            OutboundMsgPack(uc_ChannelNo, p_BMSMsg->m_OutboundMsgInfoIdx, (void *)p_BMSMsg->m_CANMsg.Un_Content.Data);

        p_Mgr->m_Period = p_BMSMsg->m_Period;
        p_Mgr->m_SignalStop = p_BMSMsg->m_SignalStop;
        p_Mgr->m_AllStop = m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll;  //keep the last stopAll flag
        p_Mgr->m_bBroadcast = 0;
        p_Mgr->m_bOnceTime = 0;

        if(p_BMSMsg->m_bUpdated) //this msg is needed send 3 can msg(almost is first time)
        {
            p_Mgr->m_SendAll =  1;
            p_BMSMsg->m_bUpdated = 0;
        }
        else
            p_Mgr->m_SendAll = 0;

        if(CANBMS_IsUseBuiltInCAN(uc_ChannelNo))
        {
            if(p_BMSMsg->m_bUpdated)
                CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);//更新间隔时间
            if((p_Mgr->m_SendAll) || (CommonFunc::GetTimeTicks(&p_BMSMsg->m_CurrentTick) > p_BMSMsg->m_Period * 10))
            {
                MP_CAN_MSG *CAN_Msg = &m_BMS_SendBuf[uc_ChannelNo].m_Msg[idx].m_CANMsg;
                CANFunc::CAN_Push_to_Send(m_CANPort, CAN_Msg);
                CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);		//更新间隔时间
            }
        }
        else
        {
            p_Mgr->m_DestID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
            if(CommonFunc::GetTimeTicks(&p_BMSMsg->m_CurrentTick) > p_BMSMsg->m_Period * 10)
            {   //190617,hpq
                p_Mgr->m_SendAll = 1;
                CommonFunc::GetTimeTicksUpdate(&p_BMSMsg->m_CurrentTick);//更新间隔时间
            }

            if((p_Mgr->m_SendAll) || (p_BMSMsg->m_SendTick % 1200 == 0))
                CANBMS_Split2SendMsg(uc_ChannelNo, &p_BMSMsg->m_CANMsg, m_BMS_SendInfoMgr[uc_ChannelNo]);
            p_BMSMsg->m_SendTick++;

        }
    }

}
/******************************************************
** Module name:      CANBMS_StopAllSend
** Descriptions:		IV chnNo
**                   IV MCU Stop All CAN Msg Send
**                   (from Sch &from dialog )
**Created Date:      2014/09/09
*******************************************************/
void CANBusParse::CANBMS_StopAllSend(unsigned char uc_ChannelNo)
{
    //Built_in CANBMS  Start//
    if(CANBMS_IsUseBuiltInCAN( uc_ChannelNo))
        return;
    //Built_in CANBMS  End//
    MP_CAN_MSG CAN_Msg;
//    unsigned char m_CANPort = CAN_BMS_PORT;
    CAN_Msg.Un_CANID.m_CANID.DestID =   ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;//Aux MCU ID /IV MCU ID
    CAN_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//     CAN_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
//     CAN_Msg.DLC = 8;
//     CAN_Msg.FrameType = CAN_EXT_FRAME;
    CAN_Msg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
    CAN_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRANS_THRID_PACK;
    CAN_Msg.Un_Content.m_BMSThirdData.m_CANID = 0;
    CAN_Msg.Un_Content.m_BMSThirdData.m_AllStop = 1; // last stopall falg
    CAN_Msg.Un_Content.m_BMSThirdData.SN = st_CAN_SN.m_CANBMS_SN;
    CAN_Msg.Un_Content.m_BMSThirdData.m_bBroadcast = 0;
// 	if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
    //CANFunc::CAN_Push_to_Send(m_CANPort,&CAN_Msg);
    MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(CAN_Msg));
}


/*****************************************************
** Module name:      CANBMS_setBroadcast
** Descriptions:		IV chnNo
**                   Trigger this IV Chn's Broadcast Send
**                   (BroadCast Voltage and Current)
**Created Date:      2014/09/09
*******************************************************/
void CANBusParse::CANBMS_setBroadcastWithAuxBorad(unsigned char uc_ChannelNo)
{
    St_BMS_SendInfo SendInfo;
    SendInfo.m_Period = 0;
    SendInfo.m_SendAll = 0;
    float m_Data[2];
    int cfgidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    if(cfgidx < 0 && cfgidx >= MAX_LEN_CAN_CONFIG)
        return;
    if(! m_CANConfig.m_buf[cfgidx].m_bEnable)
        return;		
		
    unsigned char CheckBroadCstNumber = 0;
    if(m_BroadCstIdex[uc_ChannelNo] >= BMS_BROAD_CST_COUNT)
        m_BroadCstIdex[uc_ChannelNo] = 0;

    while(!CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bValid)
    {
        m_BroadCstIdex[uc_ChannelNo]++;
        m_BroadCstIdex[uc_ChannelNo] = m_BroadCstIdex[uc_ChannelNo] % BMS_BROAD_CST_COUNT;
        CheckBroadCstNumber++;
        if(CheckBroadCstNumber >= BMS_BROAD_CST_COUNT) //扫了一遍
            return;
    }

    //update current and voltage
    for(unsigned j = 0; j < 2; j++)
    {
        if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_FIGURE )
        {
            m_Data[j] = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
        }
        else if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_METAVARIABLE )
        {
            unsigned char PV_MetaCode = (unsigned char)m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
            m_Data[j] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, PV_MetaCode);
        }
        else if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_FORMULA )
        {
            unsigned char Formula_Idex = (unsigned char)m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
//             St_FormulaData BMS_Bst_Formula = CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaData[Formula_Idex];
            for(unsigned char FormulaNum = 0; FormulaNum < 16; FormulaNum++)
            {
                CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaReflash[FormulaNum] =false;
                CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaValue[FormulaNum] = 0;
            }
            m_Data[j] = ChannelInfo_Manager::Calculate_CANConfig_Formula(uc_ChannelNo, cfgidx,Formula_Idex);
        }
        else;
    }

    m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.m_BMSBrdCast.m_Data1 = m_Data[0];
    m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.m_BMSBrdCast.m_Data2 = m_Data[1];

    SendInfo.m_DestID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
    SendInfo.m_Period = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Period;
    SendInfo.m_SignalStop = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_SignalStop;
    SendInfo.m_AllStop = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_SignalStop;  //keep the last stopAll flag
    SendInfo.m_Endian = m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[m_BroadCstIdex[uc_ChannelNo]].m_Info.m_Endian;
    SendInfo.m_bBroadcast = 1;
    SendInfo.m_bOnceTime = 1;//doesn't need send third msg

    if(CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bUpdated ||
            CommonFunc::GetTimeTicks(&m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CurrentTick) >= (m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Period * 10)	)
    {
        CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bUpdated = 0;
        CommonFunc::GetTimeTicksUpdate(&m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CurrentTick);
        SendInfo.m_SendAll = 1;
        CANBMS_Split2SendMsg(uc_ChannelNo, &m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg, SendInfo);
    }

    m_BroadCstIdex[uc_ChannelNo]++;
}
//Built_in CANBMS  Start//
void CANBusParse::CANBMS_setBroadcastWithBuiltInCAN(unsigned char uc_ChannelNo)
{
    int cfgidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
    if(cfgidx < 0 && cfgidx >= MAX_LEN_CAN_CONFIG)
        return;
    if(! m_CANConfig.m_buf[cfgidx].m_bEnable)
        return;		

    float m_Data[2];
    unsigned char CheckBroadCstNumber = 0;

    if(m_BroadCstIdex[uc_ChannelNo] >= BMS_BROAD_CST_COUNT)
        m_BroadCstIdex[uc_ChannelNo] = 0;

    while(!CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bValid)
    {
        m_BroadCstIdex[uc_ChannelNo]++;
        m_BroadCstIdex[uc_ChannelNo] = m_BroadCstIdex[uc_ChannelNo] % BMS_BROAD_CST_COUNT;
        CheckBroadCstNumber++;
        if(CheckBroadCstNumber >= BMS_BROAD_CST_COUNT) //扫了一遍
            return;
    }
    unsigned char m_CANPort = CANBMS_GetBuiltInCanPort( uc_ChannelNo);
    if(m_CANPort == 0xFF)
        return;
    //update current and voltage
    for(unsigned j = 0; j < 2; j++)
    {
        if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_FIGURE )
        {
            m_Data[j] = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
        }
        else if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_METAVARIABLE )
        {
            unsigned char PV_MetaCode = (unsigned char)m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
            m_Data[j] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, PV_MetaCode);
        }
        else if(m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Data_Mode[j] == BMS_FORMULA )
        {
            unsigned char Formula_Idex = (unsigned char)m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_BrdCstData[j];
//             St_FormulaData BMS_Bst_Formula = CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaData[Formula_Idex];
            for(unsigned char FormulaNum = 0; FormulaNum < 16; FormulaNum++)
            {
                CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaReflash[FormulaNum] =false;
                CANBusParse::m_CANConfig.m_buf[cfgidx].m_FormulaValue[FormulaNum] = 0;
            }
            m_Data[j] = ChannelInfo_Manager::Calculate_CANConfig_Formula(uc_ChannelNo, cfgidx,Formula_Idex);
        }
        else;
    }
    m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.m_BMSBrdCast.m_Data1 = m_Data[0];
    m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.m_BMSBrdCast.m_Data2 = m_Data[1];
    if(m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[m_BroadCstIdex[uc_ChannelNo]].m_Info.m_Endian == e_BigOrder)
    {
        unsigned int EndianLow, EndianHigh;
        EndianLow = m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.LLData & 0xFFFFFFFF;	// 8 Byte get 4 low Byte
        EndianHigh = ( m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.LLData >> 32) & 0xFFFFFFFF;	// 8 Byte get 4 high Byte

        m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.LLData = __REV(EndianHigh);  // Reverse, 32bit---0bit exchange 0bit---32bit   // 20170811 yang  Lock Current First
        m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.LLData = ( m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg.Un_Content.LLData << 32) | __REV(EndianLow);	// 8 Byte high 4 Byte, low 4 Byte; Curren and Voltage.
    }
    if(CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bUpdated ||
            CommonFunc::GetTimeTicks(&m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CurrentTick) >= (m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_Period * 10)	)
    {
        CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_bUpdated = 0;
        CommonFunc::GetTimeTicksUpdate(&m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CurrentTick);

        CANFunc::CAN_Push_to_Send(m_CANPort, &m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg);
        //MCUCommun::NET_Push_to_Send(&m_BMS_BroadCstBuf[uc_ChannelNo][m_BroadCstIdex[uc_ChannelNo]].m_CANMsg);
    }
    m_BroadCstIdex[uc_ChannelNo]++;
}
/***********************************************************
** Module name:      CANBMS_Split2SendMsg
** Descriptions:		IV MCU Send split CAN BMS Msg,
for CANBMS_MsgSend funciton()
**Created Date:      2014/09/17
***********************************************************/

void CANBusParse::CANBMS_Split2SendMsg(unsigned char IV_ChanNo, const MP_CAN_MSG *msg, St_BMS_SendInfo configInfo)
{
    MP_CAN_MSG ThirdMsg;
//    unsigned char m_CANPort = CAN_BMS_PORT;
    if(st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
        st_CAN_SN.m_CANBMS_SN++;
    else
        st_CAN_SN.m_CANBMS_SN = 0;
    if(configInfo.m_SendAll) //已发送过
    {
        MP_CAN_MSG Temp_CAN_Msg;//190211 zyx
        //first Data
        if(msg->DLC > 64)
            return;
        Temp_CAN_Msg.Un_CANID.m_CANID.DestID = configInfo.m_DestID;//Aux MCU ID /IV MCU ID
        Temp_CAN_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//     Temp_CAN_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
        Temp_CAN_Msg.DLC = sizeof(Un_Data);
        Temp_CAN_Msg.FrameType = CAN_EXT_FRAME;

//     if(CANBusParse::st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
//         CANBusParse::st_CAN_SN.m_CANBMS_SN++;

// 		if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
        //CANFunc::CAN_Push_to_Send(m_CANPort,&FirstMsg);

        //second Data
        Temp_CAN_Msg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
        Temp_CAN_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_BMS_MSG;
        Temp_CAN_Msg.Un_Content.m_BMSData.m_SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
        Temp_CAN_Msg.Un_Content.m_BMSData.CANid = msg->Un_CANID.m_U32ID;
        memcpy((void*)&Temp_CAN_Msg.Un_Content.m_BMSData.CANdata.ucCANData[0], (void*)msg->Un_Content.Data, msg->DLC);
        Temp_CAN_Msg.Un_Content.m_BMSData.bBroadCast =configInfo.m_bBroadcast;
        Temp_CAN_Msg.Un_Content.m_BMSData.FrameType = msg->FrameType; //Std//ext frame
        Temp_CAN_Msg.Un_Content.m_BMSData.DLC = msg->DLC; //length
        Temp_CAN_Msg.Un_Content.m_BMSData.bOnceTime = configInfo.m_bOnceTime;
        Temp_CAN_Msg.Un_Content.m_BMSData.bBigEndian = configInfo.m_Endian;
// 		if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
        //CANFunc::CAN_Push_to_Send(m_CANPort,&SecondMsg);
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(Temp_CAN_Msg));
    }
    //third Data
    if(configInfo.m_bOnceTime)
        return;
    ThirdMsg.Un_CANID.m_CANID.DestID =  configInfo.m_DestID;//Aux MCU ID /IV MCU ID
    ThirdMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//     ThirdMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    ThirdMsg.DLC = 8;
    ThirdMsg.FrameType = CAN_EXT_FRAME;
    ThirdMsg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
    ThirdMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRANS_THRID_PACK;
    ThirdMsg.Un_Content.m_BMSThirdData.m_CANID = msg->Un_CANID.m_U32ID;
    ThirdMsg.Un_Content.m_BMSThirdData.m_Period = configInfo.m_Period;
    ThirdMsg.Un_Content.m_BMSThirdData.m_SignalSotp = configInfo.m_SignalStop;//
    ThirdMsg.Un_Content.m_BMSThirdData.m_AllStop = configInfo.m_AllStop;  // last stopall falg
    ThirdMsg.Un_Content.m_BMSThirdData.SN = st_CAN_SN.m_CANBMS_SN;
    ThirdMsg.Un_Content.m_BMSThirdData.m_bBroadcast = configInfo.m_bBroadcast;
    ThirdMsg.Un_Content.m_BMSThirdData.m_Endian = configInfo.m_Endian;
// 	if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)	//Built_in CANBMS  Start////Built_in CANBMS  End//
    //CANFunc::CAN_Push_to_Send(m_CANPort,&ThirdMsg);
    MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(ThirdMsg));

}
/*********************************************************
** Module name:      CANBMS_addToConfirm
** Descriptions:		add 2 msg's to confirm buffer's
**                   and assemble a complete msg
**Created Date:      2015/03/04
**********************************************************/
// bool CANBusParse::CANBMS_addToConfirm(unsigned char IV_ChnNo,MP_CAN_MSG *FirstMsg,MP_CAN_MSG *SecondMsg)
// {
//create a datastruct
//     if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[IV_ChnNo].m_bIVActive)
//         return false;

// 	int bvalid = false;
// 	unsigned char validIdx = 0;
// 	St_BMS_BackupUnit * p_Buf;
// 	for(validIdx = 0;validIdx<MAX_CAN_BMS_ACK_BUF;validIdx++)
// 	{
// 		p_Buf = &m_BMSAckConfirmMgr[IV_ChnNo].m_Buf[validIdx];
// 		if(p_Buf->m_AckStatus !=en_AckWait) //find new buf
// 		{
// 			bvalid = true;
// 			break;
// 		}
// 	}
// 	if(!bvalid)
// 		return false;

// 	p_Buf->m_AckStatus = en_AckWait;
// 	p_Buf->m_FirstMsg = *FirstMsg;
// 	p_Buf->m_SecondMsg = *SecondMsg;
// 	p_Buf->m_OverTick = 0;
// 	p_Buf->m_ReTryCnt = 0;
// 	p_Buf->m_SN = FirstMsg->Un_Content.m_BMSFirstData.SN;
// 	return true;
// }
/*********************************************************
** Module name:      CANBMS_InitConfirmBuf
** Descriptions:		Initial  confirm buffer
**
**Created Date:      2015/03/04
**********************************************************/
// Never used
//bool CANBusParse::CANBMS_InitConfirmBuf(void)
//{
//	for(unsigned char IV_ChnNo = 0;IV_ChnNo<MAXCHANNELNO;IV_ChnNo++)
//	 {
//			m_BMSAckConfirmMgr[IV_ChnNo].m_ChnStatus = en_BMS_ACK_Safe;
//		 for(unsigned char i = 0;i<MAX_CAN_BMS_ACK_BUF-1;i++)
//		 {
//			m_BMSAckConfirmMgr[IV_ChnNo].m_Buf[i].m_AckStatus = en_AckNULL;
//			m_BMSAckConfirmMgr[IV_ChnNo].m_Buf[i].m_ReTryCnt = 0;
//			m_BMSAckConfirmMgr[IV_ChnNo].m_Buf[i].m_OverTick = 0;
//		}
//	}
//	return true;
//}

/*********************************************************
** Module name:      CANBMS_DoAckProcess
** Descriptions:		Process the msg's received
**                   from Aux Unit's AckMsg
**
**Created Date:      2015/03/04
**********************************************************/
void CANBusParse::CANBMS_DoAckProcess(MP_CAN_MSG *ackmsg)
{
    unsigned char Chn = CANBMS_GetChn(ackmsg->Un_CANID.m_CANID.SourceID);
    //  static unsigned int DbgCANID = ackmsg->Un_Content.m_CANBMS_Ack.m_CANID;
    if(Chn == 0xFF)
        return;
    if(ackmsg->Un_Content.m_CANBMS_Ack.m_RxCmd == CAN_CMD_TRANS_FIRST_PACK
            || ackmsg->Un_Content.m_CANBMS_Ack.m_RxCmd == CAN_CMD_TRANS_SECOND_PACK)
    {
        for(unsigned char i = 0; i < MAX_CAN_BMS_ACK_BUF; i++)
        {
            St_BMS_BackupUnit * p_Buf = &m_BMSAckConfirmMgr[Chn].m_Buf[i];
            if(p_Buf->m_AckStatus == en_AckWait && p_Buf->m_SN == ackmsg->Un_Content.m_CANBMS_Ack.m_SN )
            {
                p_Buf->m_ReTryCnt = 0;
                p_Buf->m_OverTick = 0;
                p_Buf->m_AckStatus = en_AckOK;
                break;
            }
        }
    }
}

/*********************************************************
** Module name:      CANBMS_DoAckCheck
** Descriptions:		Do Ack Msg's Check
**                   this is a sub task
**
**Created Date:      2015/03/04
*********************************************************/

// void CANBusParse::CANBMS_DoAckCheck(unsigned char IV_ChnNo)
// {
//     if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[IV_ChnNo].m_bIVActive)
//                  return;

// 	unsigned char m_CANPort = CAN_BMS_PORT;
// 	for(unsigned char i = 0;i<MAX_CAN_BMS_ACK_BUF;i++)
// 	{
// 		St_BMS_BackupUnit * p_Buf = &m_BMSAckConfirmMgr[IV_ChnNo].m_Buf[i];
// 		if(p_Buf->m_AckStatus != en_AckWait)
// 			continue;
// 		p_Buf->m_OverTick++;
//if( p_Buf->m_OverTick>= MAX_MULTI_PACK_ACK_OVERTIME
// 		if( p_Buf->m_OverTick>= (UDP_CANBMS_RATE*10*(800/board_config::Feature_Enable_init.OS_Tick_Interval))  // 10ms
// 			&& p_Buf->m_ReTryCnt<5) //os time* count
// 		{
// 			p_Buf->m_OverTick = 0;
// 			p_Buf->m_ReTryCnt++;
// 			if(st_CAN_SN.m_CANBMS_SN<CAN_MULTI_PACK_BUF_LEN-1)
// 				st_CAN_SN.m_CANBMS_SN++;
// 			else
// 				st_CAN_SN.m_CANBMS_SN = 0;
// 			p_Buf->m_SN = st_CAN_SN.m_CANBMS_SN;
// 			p_Buf->m_FirstMsg.Un_Content.m_BMSFirstData.SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
// 			p_Buf->m_SecondMsg.Un_Content.m_BMSSecondData.SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
// 			if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)//Built_in CANBMS  Start////Built_in CANBMS  End//
// 			{
//CANFunc::CAN_Push_to_Send(m_CANPort,& p_Buf->m_FirstMsg);
//CANFunc::CAN_Push_to_Send(m_CANPort,& p_Buf->m_SecondMsg);
//             MCUCommun::NET_Push_to_Send(& p_Buf->m_FirstMsg);
//             MCUCommun::NET_Push_to_Send(& p_Buf->m_SecondMsg);
// 			}
// 		}
// 		if(p_Buf->m_ReTryCnt>=5)
// 		{
// 			if(m_BMSAckConfirmMgr[IV_ChnNo].m_ChnStatus == en_BMS_ACK_Unsafe)
// 				return;
// 			m_BMSAckConfirmMgr[IV_ChnNo].m_ChnStatus = en_BMS_ACK_Unsafe;
// 			p_Buf->m_AckStatus = en_AckNULL;
// 			p_Buf->m_OverTick = 0;
// 			p_Buf->m_ReTryCnt = 0;
// 			St_ScheduleMessage SchMessage;
// 			memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
// 			SchMessage.MsgNumber = AUX_ACKFAIL_Msg;
// 			SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
// 			SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
// 			SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_ACK_FAILED;
//             SchMessage.st_status_sch_running.m_btLost_Unit = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[IV_ChnNo].m_AuxUnitID;
//       ChannelInfo_Manager::ChannelInfo[IV_ChnNo].m_UnsafeType = AUX_UNSAFE_TYPE;
// 			ChannelInfo_Manager::ChannelInfo[IV_ChnNo].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
// 			safety_check::SetUnsafe(IV_ChnNo,AUX_ACK_FAILED,0, AUX_UNSAFE_TYPE);
// 			Schedule_Manager_task::UnsafeContent[IV_ChnNo] = SchMessage;
// 			Schedule_Manager_task::SendMsgToSchedule(IV_ChnNo,SchMessage);//??aux Unit?????IV??
// 		}
// 	}
// }
/*********************************************************
** Module name:      CANBMS_IVAssembleMsgs
** Descriptions:		IV Rx CAN Original Msg's   and assemble
**                   a complete msg(original msg from BMS)
**Created Date:      2014/9/05
**********************************************************/

void CANBusParse::CANBMS_IVAssembleMsgs(unsigned char CANx, const ST_CAN_RxMSG * OrgMsg)
{
//    unsigned char SN = OrgMsg->RxData.uc_Data[0];
    MP_CAN_MSG  MyMsg;
    MyMsg.Un_CANID.m_U32ID = OrgMsg->RxID;
    MyMsg.bBMSCmd = 1;
    MyMsg.SubBmsCmd =  MyMsg.Un_CANID.m_CANID.Cmd;
    unsigned char chn = CANBMS_GetChn(MyMsg.Un_CANID.m_CANID.SourceID);   //这里应该转换成chn
    // ST_Multi_Pack * p_MultiPack = &CANFunc::m_MutiPackets[chn][SN];

    if(chn >= MAXCHANNELNO)
        return;//error
    switch(MyMsg.Un_CANID.m_CANID.Cmd)
    {
// 	case CAN_CMD_TRANS_FIRST_PACK:
// 		if(SN>=CAN_MULTI_PACK_BUF_LEN)
// 			return;//error
// 		ST_First_Pack FirstPack;
// 		CommonFunc::GetTimeTicksUpdate(&FirstPack.m_Overtime);
// 		memcpy((void*)&FirstPack.m_DataA.CANID.m_U32ID,(void*)&OrgMsg->RxData.uc_Data[1],7);
// 		if( (p_MultiPack->Status& FIRST_PACK_FILL) == 0)
// 		{
// 			p_MultiPack->m_FirstPack = FirstPack;
// 			p_MultiPack->Status |= FIRST_PACK_FILL;
// 		}
// 		break;
// 	case CAN_CMD_TRANS_SECOND_PACK:
// 		if(SN>=CAN_MULTI_PACK_BUF_LEN)
// 			return;//error
// 		ST_Second_Pack SecondPack;
// 		CommonFunc::GetTimeTicksUpdate(&SecondPack.m_Overtime);
// 		memcpy((void*)&SecondPack.m_DataB.Bytes[0],(void*)&OrgMsg->RxData.uc_Data[1],5);
// 		if( (p_MultiPack->Status&SECOND_PACK_FILL) ==0)
// 		{
// 			p_MultiPack->m_SecondPack = SecondPack;
// 			p_MultiPack->Status |= SECOND_PACK_FILL;
// 		}
// 		break;
    case CAN_CMD_BMS_ACK:
        memcpy(MyMsg.Un_Content.Data, OrgMsg->RxData.uc_Data, 8);
        CANBMS_DoAckProcess(&MyMsg);
        return;
    case CAN_CMD_BMS_MSG://FDCAN_CMD_FORWARD_MSG:
        MP_CAN_MSG AssembleMsg;
        AssembleMsg.m_ChnNo = chn;
        AssembleMsg.bBMSCmd = 1;
        AssembleMsg.Un_CANID.m_U32ID =  OrgMsg->RxData.m_BMSData.CANid;
        memcpy(&AssembleMsg.Un_Content.Data[0], &OrgMsg->RxData.m_BMSData.CANdata.ucCANData[0], sizeof(CANDATA));
// 		if(CANFunc::IsCANopen())
// 			NVIC_DisableIRQ(CAN_IRQn);
        ENQUEUE(CANFunc::CQ_CanRxBuffer[CANx],CAN_RX_BUF_LENGTH,AssembleMsg);
// 		if(CANFunc::IsCANopen())
// 			NVIC_EnableIRQ(CAN_IRQn);
        return;
    default:
        break;
    }
    return;
}
//Built_in CANBMS  Start//
void CANBusParse::CANBMS_IVAssembleBuiltInMsgs(unsigned char CANx, unsigned char Chan, const ST_CAN_RxMSG * OrgMsg)
{
    //static MP_CAN_MSG AssembleMsg;
		memset(&CANBusParse::AssembleMsg,0,sizeof(MP_CAN_MSG));
    AssembleMsg.m_ChnNo = Chan;
    AssembleMsg.Un_CANID.m_U32ID = OrgMsg->RxID;
    memcpy((void*)&AssembleMsg.Un_Content.Data[0], (void*)&OrgMsg->RxData.uw32_Data[0],8);

    AssembleMsg.bBMSCmd = 1;
    NVIC_DisableIRQ(CAN_IRQn);
    ENQUEUE(CANFunc::CQ_CanRxBuffer[CANx], CAN_RX_BUF_LENGTH, AssembleMsg);
    NVIC_EnableIRQ(CAN_IRQn);
}
/******************************************************
** Module name:      CANBMS_EndianParse
** Descriptions:		get Original MetaValue
Use M4 core function to Convert endian into little order
**Created Date:      2014/09/09
*******************************************************/
Un_Data CANBusParse::CANBMS_endianParse(const Un_Data* Data, unsigned char cfgidx, unsigned short uc_SignalIdx) //char 170511 zyx
{
    Un_Data BmsData = {0};
    //int cfgidx =  CANBusParse::m_CANAssignMD5[uc_BMSChn].m_cfgBufIdx;
    //unsigned long long VCode =  m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_SignalIdx].m_lowValidBit;
    unsigned int EndIndex = CANBusParse:: m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_SignalIdx].m_EndDataIndex;
    unsigned char EndianOrder =  m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_SignalIdx].m_ByteOrder;
    unsigned int StartIndex =  m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_SignalIdx].m_StartDataIndex;
    if(StartIndex > 511 ||EndIndex > 511)
        return BmsData;
    if(EndianOrder == e_LittleOrder)
    {
        unsigned int StartBit = StartIndex %32;
        unsigned int EndBit = EndIndex %32;

        unsigned int StartBytes = StartIndex/32;
        unsigned int EndBytes = EndIndex/32;
        if(StartBytes < EndBytes)
        {
            memcpy(&BmsData, &Data->fData[StartBytes], 8);
            BmsData.LLData = (( 0xFFFFFFFFFFFFFFFF >>  (31 - EndBit))&BmsData.LLData)>>StartBit;
        }
        else //(StartBytes == EndBytes)
        {
            memcpy(&BmsData.fData[0], &Data->fData[StartBytes], 4);
            BmsData.LLData = ((0xFFFFFFFF >> (31 - EndBit))&BmsData.LLData)>>StartBit;
        }
        return BmsData;
    }
    else
    {
        Un_Data TmpData = {0};
        if(m_CANConfig.m_buf[cfgidx].m_IsFDCAN)// || m_CANConfig.m_buf[cfgidx].m_ValidSignal[uc_SignalIdx].m_DLC > 8)
        {
            for(int i = 0; i < 64; i++)
            {
                TmpData.Data[i] = Data->Data[63-i];
            }
        }
        else
        {
            for(int i = 0; i < 8; i++)
            {
                TmpData.Data[i] = Data->Data[7-i];
            }
        }
        unsigned int StartBit = StartIndex %32;
        unsigned int EndBit = EndIndex %32;

        unsigned int StartBytes = StartIndex/32;
        unsigned int EndBytes = EndIndex/32;
        if(StartBytes < EndBytes)
        {
            memcpy(&BmsData, &TmpData.fData[StartBytes], 8);
            BmsData.LLData = (( 0xFFFFFFFFFFFFFFFF >>  (31 - EndBit))&BmsData.LLData)>>StartBit;
        }
        else
        {
            memcpy(&BmsData.fData[0], &TmpData.fData[StartBytes], 4);
            BmsData.LLData = ((0xFFFFFFFF >> (31 - EndBit))&BmsData.LLData)>>StartBit;
        }
        return BmsData;
    }
// 		BmsData.LLData = (VCode&BigData)>>StartBit;

// 	return BmsData;
}

/***********************************************************
** Module name:      CANBMS_QuickSortCfg
** Descriptions:		for CAN Config solution
Converts Config table to a ascending table (message's ID)
**Created Date:      2014/09/17
***********************************************************/
// void CANBusParse::CANBMS_QuickSortCfg(unsigned char SignalCnt, St_CANBMS_ParsedMeta CAN_Signal[])
// {
// 	int i = 0,j= SignalCnt-1;
// 	St_CANBMS_ParsedMeta refMeta = CAN_Signal[0]; //指定参考值val
// 	if(SignalCnt>1)   //确保数组长度至少为2
// 	{
// 		while(i<j)
// 		{
// 			//从后向前搜索比refMeta.m_CANMessageID 小的ID，找到后填入a[i]并填出循环
// 			for(;j>i;j--)
// 				if(CAN_Signal[j].m_CANMessageID<refMeta.m_CANMessageID)
// 				{
// 					CAN_Signal[i] = CAN_Signal[j];
// 					break;
// 				}
// 				//从前往后搜索比大的ID，找到后填入CAN_Signal[j]中并跳出循环
// 				for(;i<j;i++)
// 					if(CAN_Signal[i].m_CANMessageID>refMeta.m_CANMessageID)
// 					{
// 						CAN_Signal[j] = CAN_Signal[i];
// 						break;
// 					}
// 		}
// 		CAN_Signal[i] = refMeta; //将保存在val中的数放到a[i]中
// 		CANBMS_QuickSortCfg(i,CAN_Signal);
// 		CANBMS_QuickSortCfg(SignalCnt-1-i,CAN_Signal+i+1);
// 	}
// }
/***********************************************************
** Module name:      CANBMS_binarySearchCfg
** Descriptions:		for CAN BMS msg solution
**Created Date:      2014/09/17
***********************************************************/
int CANBusParse::CANBMS_binarySearchCfg(unsigned char cfgidx, unsigned int searchID, CANBMS_METAIdx low, CANBMS_METAIdx high)
{
    //CANBMS_METAIdx high =  m_CANConfig.m_buf[cfgidx].m_MetaCount - 1, low = 0,mid; //170505 zyx
    CANBMS_METAIdx mid = 0; //170606 zyx
    while(low <= high)
    {
        if( m_CANConfig.m_buf[cfgidx].m_ValidSignal[low].m_CANMessageID == searchID)
            return low;
        else if( m_CANConfig.m_buf[cfgidx].m_ValidSignal[high].m_CANMessageID == searchID)
            return high;
        mid = low + (high - low) / 2;
        if( m_CANConfig.m_buf[cfgidx].m_ValidSignal[mid].m_CANMessageID == searchID)
            return mid;
        if( m_CANConfig.m_buf[cfgidx].m_ValidSignal[mid].m_CANMessageID > searchID)
            high = mid - 1;
        else if( m_CANConfig.m_buf[cfgidx].m_ValidSignal[mid].m_CANMessageID < searchID)
            low = mid + 1;
        //protect
        if(low >= high)
            return -1;
    }
    return -1;
}
/***********************************************************
** Module name:      CANBMS_SetBaudRate
** Descriptions:		set Aux's BMS port BaudRate.
**Created Date:      2014/09/18
***********************************************************/
void CANBusParse::CANBMS_SetBaudRate(unsigned char uc_ChannelNo)
{
    if(CANBMS_IsUseBuiltInCAN(uc_ChannelNo))//Built_in CANBMS  Start//
    {
        int cfgidx =  CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
        if(! m_CANConfig.m_buf[cfgidx].m_bEnable)
            return;
        unsigned char CANPort = CANBusParse::CANBMS_GetBuiltInCanPort(uc_ChannelNo);
        if(CANPort != 0xFF)
            CANFunc::CAN_Reset_BaudRate(CANPort, m_CANConfig.m_buf[cfgidx].m_BaudRate);
    }
    else//Built_in CANBMS  End//
    {
        int cfgidx =  CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
        if(! m_CANConfig.m_buf[cfgidx].m_bEnable)
            return;
        MP_CAN_MSG setBaudMsg;
        setBaudMsg.DLC = 8;
        setBaudMsg.FrameType = CAN_EXT_FRAME;
        setBaudMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_SET_BMS_BAUDRATE;
        setBaudMsg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
        setBaudMsg.Un_CANID.m_CANID.DestID =  ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
        setBaudMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//         setBaudMsg.Un_CANID.m_CANID.Priority = 0;
        setBaudMsg.Un_Content.m_BMSBaudMsg.m_IsFDCAN = m_CANConfig.m_buf[cfgidx].m_IsFDCAN;
        setBaudMsg.Un_Content.m_BMSBaudMsg.m_baudRate =  m_CANConfig.m_buf[cfgidx].m_BaudRate;
        setBaudMsg.Un_Content.m_BMSBaudMsg.m_BitRate = m_CANConfig.m_buf[cfgidx].m_BitRate;//181225 zyx
        setBaudMsg.Un_Content.m_BMSBaudMsg.m_bForward = 1;
        if(st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
            st_CAN_SN.m_CANBMS_SN++;
        else
            st_CAN_SN.m_CANBMS_SN = 0;
        setBaudMsg.Un_Content.m_BMSBaudMsg.m_SN = st_CAN_SN.m_CANBMS_SN;
        // 	if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)//Built_in CANBMS  Start////Built_in CANBMS  End//
        //CANFunc::CAN_Push_to_Send(CAN_BMS_PORT,&setBaudMsg);
        MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(setBaudMsg));
    }
}

/***********************************************************
** Module name:      CANBMS_ClearBMS
** Descriptions:		 Stop CANBMS function.
**Created Date:      2016/01/04
***********************************************************/
void CANBusParse::CANBMS_ClearBMS(unsigned char uc_ChannelNo)
{

    MP_CAN_MSG ClearBMSMsg;
//     ClearBMSMsg.DLC = 8;
//     ClearBMSMsg.FrameType = CAN_EXT_FRAME;
    ClearBMSMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_CLEAR_BMS;
    ClearBMSMsg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
    ClearBMSMsg.Un_CANID.m_CANID.DestID =  ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID;
    ClearBMSMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//     ClearBMSMsg.Un_CANID.m_CANID.Priority = 0;
    ClearBMSMsg.Un_Content.m_BMSBaudMsg.m_baudRate =  0;  // The barudrate is not used here, can be ignored
    ClearBMSMsg.Un_Content.m_BMSBaudMsg.m_bForward = 0;  //Stop forward message
    if(st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
        st_CAN_SN.m_CANBMS_SN++;
    else
        st_CAN_SN.m_CANBMS_SN = 0;
    ClearBMSMsg.Un_Content.m_BMSBaudMsg.m_SN = st_CAN_SN.m_CANBMS_SN;
// 	if(CheckCAN_Port_Property(CAN2) == USE_AS_NORMAL_CAN)//Built_in CANBMS  Start////Built_in CANBMS  End//
    //CANFunc::CAN_Push_to_Send(CAN_BMS_PORT,&ClearBMSMsg);
    MultiMcuComm::NET_Push_to_Send(CanMsg2McuMsg(ClearBMSMsg));
}
signed long CANBusParse::GetOutboundMessageIndex(unsigned char ChnNo,unsigned long MsgID)
{   //190617,hpq
    if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnNo].m_bIVActive)
        return -1;

    int insertIdx = CANBusParse::m_CANAssignMD5[ChnNo].m_insertPos;
    if(insertIdx < 0)
        return -1;
    signed long OutboundMsgInfoIdx = 0;
    St_CANBMS_Config * p_CAN_Cfg = &CANBusParse::m_CANConfig.m_buf[insertIdx];
    for(OutboundMsgInfoIdx = 0; OutboundMsgInfoIdx<p_CAN_Cfg->m_CAN_OutboundMsgCount; OutboundMsgInfoIdx++)
    {
        if(p_CAN_Cfg->m_CAN_OutboundMsgInfos[OutboundMsgInfoIdx].m_CANID == MsgID)
            return OutboundMsgInfoIdx;
    }
    return -1;
}
typedef union //Un_BMS_Data
{
    unsigned long long                              LongData;
    unsigned char                                   uData[8];
} Un_BMS_Data;
signed char CANBusParse::OutboundMsgPack(unsigned char ChnNo,unsigned long MsgInfoIdx,void *data)
{   //190617,hpq
    if(!ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnNo].m_bIVActive)
        return -1;
    int insertIdx = CANBusParse::m_CANAssignMD5[ChnNo].m_insertPos;
    if(insertIdx < 0)
        return -1;
    memset(data,0,8);
    St_CANBMS_Config * p_CAN_Cfg = &CANBusParse::m_CANConfig.m_buf[insertIdx];
    if(p_CAN_Cfg->m_CAN_OutboundMsgInfos[MsgInfoIdx].m_MemberCount <= 0)
        return 0;
    St_CANBMS_OutboundMsgInfo *pMsg = &p_CAN_Cfg->m_CAN_OutboundMsgInfos[MsgInfoIdx];
    float fValue = 0;
    Un_BMS_Data Data_BigEndian,Data_LittleEndian;
    Data_BigEndian.LongData = 0;
    Data_LittleEndian.LongData = 0;
    signed long long   sRawData = 0;
    unsigned long long uRawData = 0;
    for(int msgidx = 0; msgidx < p_CAN_Cfg->m_CAN_OutboundMsgInfos[MsgInfoIdx].m_MemberCount; msgidx++)
    {
        if(p_CAN_Cfg->m_CAN_OutboundSignals[pMsg->m_MsgMember[msgidx].m_u16OutboundSignalIndex].m_u8ValueType == MP_DATA_TYPE_FORMULA)
        {
            unsigned char Formula_Idex = (unsigned char)p_CAN_Cfg->m_CAN_OutboundSignals[pMsg->m_MsgMember[msgidx].m_u16OutboundSignalIndex].m_fValue;
//             St_FormulaData BMS_Bst_Formula = p_CAN_Cfg->m_FormulaData[Formula_Idex];
            for(unsigned char FormulaNum = 0; FormulaNum < 16; FormulaNum++)
            {
                CANBusParse::m_CANConfig.m_buf[insertIdx].m_FormulaReflash[FormulaNum] =false;
                CANBusParse::m_CANConfig.m_buf[insertIdx].m_FormulaValue[FormulaNum] = 0;
            }
            fValue = ChannelInfo_Manager::Calculate_CANConfig_Formula(ChnNo, insertIdx,Formula_Idex);
        }
        else
            fValue = ChannelInfo_Manager::Get_MetaVariable(ChnNo,
                     p_CAN_Cfg->m_CAN_OutboundSignals[pMsg->m_MsgMember[msgidx].m_u16OutboundSignalIndex].m_u8ValueType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,
                     p_CAN_Cfg->m_CAN_OutboundSignals[pMsg->m_MsgMember[msgidx].m_u16OutboundSignalIndex].m_fValue);
        fValue = pMsg->m_MsgMember[msgidx].m_fGain * fValue + pMsg->m_MsgMember[msgidx].m_fOffset;
        if(fValue > pMsg->m_MsgMember[msgidx].m_fMaxValue)
            fValue = pMsg->m_MsgMember[msgidx].m_fMaxValue;
        if(fValue < pMsg->m_MsgMember[msgidx].m_fMinValue)
            fValue = pMsg->m_MsgMember[msgidx].m_fMinValue;
        sRawData = 0;
        uRawData = 0;
        switch(pMsg->m_MsgMember[msgidx].m_u8DataType)
        {
        case eType_Float:
            memcpy((void *)&uRawData, (const void *)&fValue, 4);
            break;
        case eType_Signed:
            fValue = pMsg->m_MsgMember[msgidx].m_lineTranGain * fValue + pMsg->m_MsgMember[msgidx].m_lineTranOffset;
            sRawData = llround(fValue);
            memcpy((void *)&uRawData, (const void *)&sRawData, 8);
            break;
        default:
            fValue = pMsg->m_MsgMember[msgidx].m_lineTranGain * fValue + pMsg->m_MsgMember[msgidx].m_lineTranOffset;
            uRawData = llround(fValue);
            break;
        }
        int bitCount = 0;
        if(pMsg->m_MsgMember[msgidx].m_u8ByteOrder == e_LittleOrder)
        {
            bitCount =  pMsg->m_MsgMember[msgidx].m_u8EndByte*8 \
                        + pMsg->m_MsgMember[msgidx].m_u8EndBit \
                        - pMsg->m_MsgMember[msgidx].m_u8StartByte*8\
                        - pMsg->m_MsgMember[msgidx].m_u8StartBit + 1;
            if(bitCount < 0)
                bitCount = 0;
            uRawData = uRawData << (64-bitCount);
            uRawData = uRawData >> (64-bitCount);
            Data_LittleEndian.LongData  |= uRawData << (pMsg->m_MsgMember[msgidx].m_u8StartByte*8 + pMsg->m_MsgMember[msgidx].m_u8StartBit);
        }
        else
        {
            bitCount = (pMsg->m_MsgMember[msgidx].m_u8StartByte - pMsg->m_MsgMember[msgidx].m_u8EndByte)*8 \
                       + pMsg->m_MsgMember[msgidx].m_u8EndBit - pMsg->m_MsgMember[msgidx].m_u8StartBit + 1;
            if(bitCount < 0)
                bitCount = 0;
            uRawData = uRawData << (64-bitCount);
            uRawData = uRawData >> (64-bitCount);
            Data_BigEndian.LongData     |= uRawData << ((7 - pMsg->m_MsgMember[msgidx].m_u8StartByte)*8 + pMsg->m_MsgMember[msgidx].m_u8StartBit);
        }
    }
    memcpy(data, (const void *)&Data_LittleEndian,8);
    ((unsigned char*)data)[0] |= Data_BigEndian.uData[7];
    ((unsigned char*)data)[1] |= Data_BigEndian.uData[6];
    ((unsigned char*)data)[2] |= Data_BigEndian.uData[5];
    ((unsigned char*)data)[3] |= Data_BigEndian.uData[4];
    ((unsigned char*)data)[4] |= Data_BigEndian.uData[3];
    ((unsigned char*)data)[5] |= Data_BigEndian.uData[2];
    ((unsigned char*)data)[6] |= Data_BigEndian.uData[1];
    ((unsigned char*)data)[7] |= Data_BigEndian.uData[0];
}

void CANBusParse::CANOutboundSignalLinearTransformation(St_CANBMS_Config * p_CAN_Cfg,int msginfoidx)//zyx 1910161
{
    St_BMS_CFG_OutboundMessage *pOutboundInfo = &p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MsgMember[p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MemberCount];
    if(pOutboundInfo->m_u8DataType == eType_Float)
        return;
    else
    {
        int bitCount = 0;
        long long BitMax = 1, BitMin = 0;
        if(pOutboundInfo->m_u8ByteOrder == e_LittleOrder)
        {
            bitCount =  pOutboundInfo->m_u8EndByte*8 \
                        + pOutboundInfo->m_u8EndBit \
                        - pOutboundInfo->m_u8StartByte*8\
                        - pOutboundInfo->m_u8StartBit + 1;
            if(bitCount < 0)
                bitCount = 0;
        }
        else
        {
            bitCount = (pOutboundInfo->m_u8StartByte - pOutboundInfo->m_u8EndByte)*8 \
                       + pOutboundInfo->m_u8EndBit - pOutboundInfo->m_u8StartBit + 1;
            if(bitCount < 0)
                bitCount = 0;
        }

        if(pOutboundInfo->m_u8DataType == eType_Unsigned)
        {
            // BitMax = (1 << bitCount) -1;
            BitMax = (BitMax << bitCount) -1;

        }
        else
        {
//             BitMax = (1 << (bitCount-1))-1;
//             BitMin = -(1 << (bitCount-1));
            BitMin = -(BitMax << (bitCount-1));
            BitMax = -1*BitMin - 1;
        }

        pOutboundInfo->m_lineTranGain = (BitMax - BitMin)/(pOutboundInfo->m_fMaxValue - pOutboundInfo->m_fMinValue);

        pOutboundInfo->m_lineTranOffset = -1*pOutboundInfo->m_lineTranGain * pOutboundInfo->m_fMinValue + BitMin;

    }
}

/***********************************************************
** Module name:      SetCanBmsData
** Descriptions:		set CAN Send Msg
**							(called by sch & Dialog)
**Created Date:      2014/09/18
***********************************************************/
void CANBusParse::SetCanBmsData(MP_SET_CANMSG *pCmd)
{
#if  USE_AS_AUXMCU == 0
    // bool bSetNewMsg = true;
    BYTE ChnNo = pCmd->m_IVChnNo;
    //Built_in CANBMS  Start//
    bool bIsNeedSendtoAuxCANBMS = true;//是否需要发送给辅助
    if(CANBMS_IsUseBuiltInCAN(ChnNo))
        bIsNeedSendtoAuxCANBMS = false;
    //Built_in CANBMS  End//
    int OutboundMsgInfoIdx = -1;//190617,hpq
    if(pCmd->m_CANMsg.m_bOutboundMsg)
    {   //190617,hpq
        OutboundMsgInfoIdx = GetOutboundMessageIndex(ChnNo,pCmd->m_CANMsg.m_ID);
        OutboundMsgPack(ChnNo,OutboundMsgInfoIdx,(void*)&pCmd->m_CANMsg.m_Data[0]);
    }

    unsigned char msgIdx = CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt;

    if(pCmd->m_CANMsg.m_AllStop)
    {   //stop all msg
        //		if(CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt >0)
        {
            if(bIsNeedSendtoAuxCANBMS)//Built_in CANBMS  Start////Built_in CANBMS  End//
                CANBMS_StopAllSend(ChnNo);
            m_BMS_SendBuf[ChnNo].m_validMsgCnt = 0;
        }
        CANBusParse::m_BMS_SendBuf[ChnNo].m_glbStopAll = 1;
    }
    else if(pCmd->m_CANMsg.m_SignalStop)
    {   //single stop
        if(bIsNeedSendtoAuxCANBMS)//Built_in CANBMS  Start////Built_in CANBMS  End//
        {   //single stop
            pCmd->m_CANMsg.m_Period = 0; //stop msg do not need periodical sending
            MP_CAN_MSG Msg;
            St_BMS_SendInfo SendInfo;
            SendInfo.m_Period = 0;
            SendInfo.m_SendAll = 1;
            SendInfo.m_bOnceTime = 0;  //??????
            SendInfo.m_bBroadcast = 0;
            SendInfo.m_AllStop = pCmd->m_CANMsg.m_AllStop;
            SendInfo.m_SignalStop = pCmd->m_CANMsg.m_SignalStop;
            SendInfo.m_DestID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnNo].m_AuxUnitID;
            Msg.Un_CANID.m_U32ID = pCmd->m_CANMsg.m_ID;
            Msg.FrameType = pCmd->m_CANMsg.m_FrameType;
            Msg.DLC = pCmd->m_CANMsg.m_Length;
            memcpy((void*)&Msg.Un_Content.Data[0], (void*)&pCmd->m_CANMsg.m_Data[0], Msg.DLC);
            CANBMS_Split2SendMsg(ChnNo, &Msg, SendInfo);
        }
        //µ¥¸ö±¨ÎÄÍ£Ö¹£¬ÐèÒªÌÞ³ý´ËIDËùÓÐ±¨ÎÄ
        for(unsigned char idx = 0; idx < CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt; idx++)
        {   //find out same ID msg buffer
            if( CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[idx].m_CANMsg.Un_CANID.m_U32ID == pCmd->m_CANMsg.m_ID )
            {
                for(unsigned char destIdx = idx; destIdx < CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt; destIdx++)  //memcpy all following msgs forward
                    memcpy(&CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[destIdx], &CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[destIdx + 1], sizeof(St_BMS_SendUnit));
                CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt--;
            }
        }
    }
    else if(pCmd->m_CANMsg.m_Period == 0)
    {   //single msg
        if(bIsNeedSendtoAuxCANBMS)//Built_in CANBMS  Start////Built_in CANBMS  End//
        {   //single msg
            pCmd->m_CANMsg.m_Period = 0; //stop msg do not need periodical sending
            MP_CAN_MSG Msg;
            St_BMS_SendInfo SendInfo;
            SendInfo.m_Period = 0;
            SendInfo.m_SendAll = 1;
            SendInfo.m_bOnceTime = 1;
            SendInfo.m_bBroadcast = 0;
            SendInfo.m_AllStop = pCmd->m_CANMsg.m_AllStop;
            SendInfo.m_SignalStop = pCmd->m_CANMsg.m_SignalStop;
            SendInfo.m_DestID = ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnNo].m_AuxUnitID;
            Msg.Un_CANID.m_U32ID = pCmd->m_CANMsg.m_ID;
            Msg.FrameType = pCmd->m_CANMsg.m_FrameType;
            Msg.DLC = pCmd->m_CANMsg.m_Length;
            memcpy((void*)&Msg.Un_Content.Data[0], (void*)&pCmd->m_CANMsg.m_Data[0], Msg.DLC);
            CANBusParse::CANBMS_Split2SendMsg(ChnNo, &Msg, SendInfo);
        }
        else//Built_in CANBMS  Start//
        {
            MP_CAN_MSG OnceTimeMsg;//如果周期为0只发送一次。
            unsigned char CANPort = CANBMS_GetBuiltInCanPort(ChnNo);
            OnceTimeMsg.DLC = pCmd->m_CANMsg.m_Length;
            OnceTimeMsg.FrameType = pCmd->m_CANMsg.m_FrameType;
            OnceTimeMsg.Un_CANID.m_U32ID = pCmd->m_CANMsg.m_ID;
            memcpy((void*)&OnceTimeMsg.Un_Content.Data[0], (void*)&pCmd->m_CANMsg.m_Data[0], OnceTimeMsg.DLC);
            if( CANPort != 0xFF)
                CANFunc::CAN_Push_to_Send(CANPort, &OnceTimeMsg);
            //MCUCommun::NET_Push_to_Send(&OnceTimeMsg);
        }//Built_in CANBMS  End//
    }
    else
    {   //periodical data msg,no stop flag
        bool bSetNewMsg = true;  //190617,hpq
        for(unsigned char idx = 0; idx < CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt; idx++)
        {   //find out same ID msg buffer
            if( CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[idx].m_CANMsg.Un_CANID.m_U32ID == pCmd->m_CANMsg.m_ID )
            {
                msgIdx = idx;
                bSetNewMsg = false;
                break;
            }
        }
        if(msgIdx >= SEND_CAN_BMS_MAX_LEN)
            return; // continue;
        CANBusParse::m_BMS_SendBuf[ChnNo].m_glbStopAll = 0;
        if( pCmd->m_CANMsg.m_Period > 3600000)
            CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_Period = 3600000;
        else
            CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_Period = pCmd->m_CANMsg.m_Period;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_SignalStop = 0;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_CANMsg.DLC = pCmd->m_CANMsg.m_Length;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_CANMsg.FrameType  = pCmd->m_CANMsg.m_FrameType;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_CANMsg.Un_CANID.m_U32ID = pCmd->m_CANMsg.m_ID;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_bUpdated = 1;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_bBroadcast = 0;
        m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_OutboundMsgInfoIdx = OutboundMsgInfoIdx;  //190617,hpq  //normal msg,not outbound msg
        memcpy(CANBusParse::m_BMS_SendBuf[ChnNo].m_Msg[msgIdx].m_CANMsg.Un_Content.Data, pCmd->m_CANMsg.m_Data, 8);
//             if(CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt[CAN_Idex] >= SEND_CAN_BMS_MAX_LEN)
//                 CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt[CAN_Idex] = SEND_CAN_BMS_MAX_LEN;
//             else
//             {
        if(bSetNewMsg)
            CANBusParse::m_BMS_SendBuf[ChnNo].m_validMsgCnt++;
//             }

        if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[ChnNo].m_bIVActive)  // 手动发送BMS包也开放定时自动发送功能
            CANBusParse::m_BMS_SendBuf_RunOn[ChnNo] = 1;
    }

#endif
}

/***********************************************************
** Module name:      CheckCanBmsMap
** Descriptions:		Check CAN BMS  MAP
**
**Created Date:
***********************************************************/
void CANBusParse::CheckCanBmsMap(void)
{
    unsigned char uc_UseFlag = 0;
    CANBMS_bWork = true;
    if(ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum > 255)
        ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum = 0;
    for(unsigned char bufidx = 0; bufidx < ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum; bufidx++)
    {
        if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[bufidx].m_bIVActive)
        {
            uc_UseFlag = 1;
            CommonFunc::uc_Support_AUX = 1;
        }
    }
    if(!uc_UseFlag)
        CANBMS_bWork = false;

    CANBusParse::CANBMS_bMap = true; //181010 zyx
    //ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum = 0;
}
//Built_in CANBMS  Start//
unsigned char CANBusParse::CheckCAN_Port_Property(unsigned char CANx)
{
    if(CANx == CAN1)
    {
        if(ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN1PortUsedAsCANBMS == true)
            return USE_AS_BUILT_IN_CANBMS;
        else
            return USE_AS_NORMAL_CAN;
    }
    else if(CANx == CAN2)
    {
        if(ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN2PortUsedAsCANBMS == true)
            return USE_AS_BUILT_IN_CANBMS;
        else
            return USE_AS_NORMAL_CAN;
    }
    else
        return USE_AS_NORMAL_CAN;
}
bool CANBusParse::CANBMS_IsUseBuiltInCAN(unsigned char uc_ChannelNo)
{
    if ( ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive\
            && (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_IVChnNo == uc_ChannelNo)\
            && (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_AuxUnitID == CANFunc::SourceBoardID))
        return true;

    return false;
}
unsigned char CANBusParse::CANBMS_GetBuiltInCanPort(unsigned char uc_ChannelNo)
{
    if(CANBMS_IsUseBuiltInCAN(uc_ChannelNo))
    {
        if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_CANBMSChnIdx == ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN1PortCANBMSChnIdx)
            return CAN1;
        else if(ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_CANBMSChnIdx == ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN2PortCANBMSChnIdx)
            return CAN2;
        else
            return 0xFF;
    }
    return 0xFF;
}
// unsigned char CANBusParse::CANBMS_GetCANBMSChn(unsigned char uc_ChannelNo)
// {
// 		if ( ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_bIVActive\
// 			&& (ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_IVChnNo == uc_ChannelNo))
// 			return ChannelInfo_Manager::m_CANMapInfo.m_CANmap[uc_ChannelNo].m_CANBMSChnIdx;
//
// 	return 0xFF;
// }
unsigned char CANBusParse::CANBMS_GetChanOfCANx(unsigned char CANx)//找到CANx对应的通道号。
{
    for(unsigned char chnidx = 0; chnidx < MAXCHANNELNO; chnidx++)
    {
        if(CANBMS_GetBuiltInCanPort(chnidx) == CANx)
            return chnidx;
    }
    return 0xFF;
}
//Built_in CANBMS  End//

#else

/*-------------------------AUX CAN BMS -------------------*/

/***********************************************************
** Module name:      CANBMS_ParseMsg
** Descriptions:		aux Reassemble msg | set BMS baudrate
**Created Date:      2014/09/16
***********************************************************/
void CANBusParse::CANBMS_AuxParseMsg(unsigned char CANx, const MP_CAN_MSG * Parse_Msg)
{
    switch(Parse_Msg->SubBmsCmd)
    {
    case CAN_CMD_SET_BMS_BAUDRATE:
        m_CANBMS_AuxInfo.m_IsFDCAN = false;
        m_CANBMS_AuxInfo.m_BaudRate = Parse_Msg->Un_Content.m_BMSBaudMsg.m_baudRate;
        m_CANBMS_AuxInfo.m_bAuxForward = Parse_Msg->Un_Content.m_BMSBaudMsg.m_bForward;
        m_CANBMS_AuxInfo.m_IVCANID =  Parse_Msg->Un_CANID.m_CANID.SourceID;
        m_CANBMS_AuxInfo.m_BitRate = 0;
        CANFunc::CAN_Reset_BaudRate(CAN2, Parse_Msg->Un_Content.m_BMSBaudMsg.m_baudRate);
        break;
    case CAN_CMD_TRANS_THRID_PACK:    //Third Msg		find the canid attach information(period and stop info)
        if(Parse_Msg->Un_Content.m_BMSThirdData.m_AllStop)
        {
            for(unsigned char Idx = 0; Idx < m_CANBMS_AuxInfo.m_validMsgCnt; Idx++)
            {
                m_CANBMS_AuxInfo.m_Msg[Idx].m_bValid = 0;
            }
            m_CANBMS_AuxInfo.m_validMsgCnt = 0;
            return;
        }
        else if(Parse_Msg->Un_Content.m_BMSThirdData.m_SignalSotp)
        {   //剔除sigle stop msg
            for(unsigned char Idx = 0; Idx < m_CANBMS_AuxInfo.m_validMsgCnt; Idx++)
                if(Parse_Msg->Un_Content.m_BMSThirdData.m_CANID == m_CANBMS_AuxInfo.m_Msg[Idx].m_CANMsg.Un_CANID.m_U32ID)  //赋值
                {
                    for(unsigned char destIdx = Idx; destIdx < m_CANBMS_AuxInfo.m_validMsgCnt; destIdx++)  //memcpy all following msgs forward
                        memcpy(&m_CANBMS_AuxInfo.m_Msg[destIdx], &m_CANBMS_AuxInfo.m_Msg[destIdx + 1], sizeof(St_BMS_SendUnit));
                    m_CANBMS_AuxInfo.m_validMsgCnt--;
                }
        }
        else
        {   //replace or attach this msg's period
            for(unsigned char Idx = 0; Idx < m_CANBMS_AuxInfo.m_validMsgCnt; Idx++)
                if(Parse_Msg->Un_Content.m_BMSThirdData.m_CANID == m_CANBMS_AuxInfo.m_Msg[Idx].m_CANMsg.Un_CANID.m_U32ID)  //赋值
                {
                    m_CANBMS_AuxInfo.m_Msg[Idx].m_Period = (unsigned long long)Parse_Msg->Un_Content.m_BMSThirdData.m_Period * 10;
                    m_CANBMS_AuxInfo.m_Msg[Idx].m_SignalStop = Parse_Msg->Un_Content.m_BMSThirdData.m_SignalSotp;
                    m_CANBMS_AuxInfo.m_Msg[Idx].m_bBroadcast = 0;
                    m_CANBMS_AuxInfo.m_Msg[Idx].m_bValid = 1;
                }
        }
        break;
    case CAN_CMD_TRANS_FIRST_PACK:   //new assemble msg(first msg & second msg)
        for(unsigned char Idx = 0; Idx < m_CANBMS_AuxInfo.m_validMsgCnt; Idx++)		//find same CANID message
        {
            if(Parse_Msg->Un_CANID.m_U32ID == m_CANBMS_AuxInfo.m_Msg[Idx].m_CANMsg.Un_CANID.m_U32ID)
            {
                m_CANBMS_AuxInfo.m_Msg[Idx].m_CANMsg = *Parse_Msg;
                CommonFunc::GetTimeTicksUpdate(&m_CANBMS_AuxInfo.m_Msg[Idx].m_Overtime);
                m_CANBMS_AuxInfo.m_Msg[Idx].m_bUpdate = 1;
                return;
            }
        }
        if(m_CANBMS_AuxInfo.m_validMsgCnt >= SEND_CAN_BMS_MAX_LEN)
        {
            m_CANBMS_AuxInfo.m_validMsgCnt = SEND_CAN_BMS_MAX_LEN;
            return;
        }
        else
        {
            m_CANBMS_AuxInfo.m_Msg[m_CANBMS_AuxInfo.m_validMsgCnt++].m_CANMsg = *Parse_Msg;
            m_CANBMS_AuxInfo.m_Msg[m_CANBMS_AuxInfo.m_validMsgCnt].m_bUpdate = 1;
            CommonFunc::GetTimeTicksUpdate(&m_CANBMS_AuxInfo.m_Msg[m_CANBMS_AuxInfo.m_validMsgCnt].m_Overtime);
        }
        break;
    case CAN_CMD_CLEAR_BMS:
        m_CANBMS_AuxInfo.m_bAuxForward = Parse_Msg->Un_Content.m_BMSBaudMsg.m_bForward ;  //stop forward can messages
        m_CANBMS_AuxInfo.m_IVCANID =  Parse_Msg->Un_CANID.m_CANID.SourceID;
        for(unsigned char i = 0; i<BMS_BROAD_CST_COUNT; i++)
        {
            m_CANBMS_AuxInfo.m_BrdCstInfo[i].m_bValidBrdcst = false ;  // Stop Broadcast
        }
        m_CANBMS_AuxInfo.m_validMsgCnt = 0 ;  // stop sending prodic messages
        break;

    default:
        break;
    }

}


MP_CAN_MSG CANBusParse::ToNormalCanMsg( ST_SEND_MULTI_CANBMS_MSG m_MultiCanMsg ) //170828 zyx
{
    MP_CAN_MSG m_CanMsg;
    memset(&m_CanMsg, 0, sizeof(MP_CAN_MSG));
    m_CanMsg.DLC = m_MultiCanMsg.DLC;
    m_CanMsg.FrameType = m_MultiCanMsg.FrameType;
    m_CanMsg.Un_CANID.m_U32ID = m_MultiCanMsg.m_CANID;
    memcpy(m_CanMsg.Un_Content.Data, m_MultiCanMsg.CANData, 8);  //sizeof(unsigned char)*8
    return m_CanMsg;
}

/******************************************************
** Module name:      CANBMS_AuxDoWork
** Descriptions:		aux periodicity to Send CAN_MSG,
(& sch's Msg & customer manual configuration's msg)
**Created Date:      2014/09/16
*******************************************************/
static unsigned char bSendBms = 0;
void CANBusParse::CANBMS_AuxDoWork()
{
    static unsigned char bSet = 0xff;
    for(unsigned char i = 0; i<BMS_BROAD_CST_COUNT; i++)
    {
        if(m_CANBMS_AuxInfo.m_BrdCstInfo[i].m_bValidBrdcst)
        {
            //CAN2
            //if(CANFunc::IsCANopen())
            NVIC_DisableIRQ(CAN_IRQn);
            m_CANBMS_AuxInfo.m_BrdCstInfo[i].m_bValidBrdcst = 0;
            //if(CANFunc::IsCANopen())
            NVIC_EnableIRQ(CAN_IRQn);
            m_CANBMS_AuxInfo.m_BrdCstInfo[i].m_BradCastMsg.DLC = 8;
            CANFunc::CAN_Push_to_Send(CAN2,& m_CANBMS_AuxInfo.m_BrdCstInfo[i].m_BradCastMsg);
        }
    }
    if(bSendBms ==0 )
        CANBMS_ForwardMsg2(CAN1);
    bSendBms++;
    if(bSendBms>4)
        bSendBms = 0;
    for(unsigned char MsgIdx = 0; MsgIdx < m_CANBMS_AuxInfo.m_validMsgCnt; MsgIdx++)
    {
        if(m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_SignalStop)
            continue;
        if(!m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_bValid)
            continue;
        if( m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_bUpdate || CommonFunc::GetTimeTicks(&m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_Overtime) >= m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_Period)
        {
            //CAN2
            m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_bUpdate = 0;
            CommonFunc::GetTimeTicksUpdate(&m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_Overtime);
            CANFunc::CAN_Push_to_Send(CAN2, &m_CANBMS_AuxInfo.m_Msg[MsgIdx].m_CANMsg);
        }
    }

    if(m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_AllStop || m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_validMsgCnt == 0) //170828 zyx
        return;
    else
    {
        if(m_BMS_AuxMultiMsgManage.bIsOneCycle && CommonFunc::GetTimeTicks(&m_BMS_AuxMultiMsgManage.m_SendInterval) >= m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_set_SendInterval * 10)
        {
            m_BMS_AuxMultiMsgManage.bIsOneCycle = 0;
            MP_CAN_MSG m_CanMsg = ToNormalCanMsg(m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_Msg[m_BMS_AuxMultiMsgManage.MsgIdx++]);
            CANFunc::CAN_Push_to_Send(CAN2, &m_CanMsg);
            CommonFunc::GetTimeTicksUpdate(&m_BMS_AuxMultiMsgManage.m_FrameInterval);
        }
        else if(!m_BMS_AuxMultiMsgManage.bIsOneCycle && CommonFunc::GetTimeTicks(&m_BMS_AuxMultiMsgManage.m_FrameInterval) >= m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_set_FrameInterval * 10)
        {
            MP_CAN_MSG m_CanMsg = ToNormalCanMsg(m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_Msg[m_BMS_AuxMultiMsgManage.MsgIdx++]);
            CANFunc::CAN_Push_to_Send(CAN2, &m_CanMsg);
            CommonFunc::GetTimeTicksUpdate(&m_BMS_AuxMultiMsgManage.m_FrameInterval);
        }
        if(m_BMS_AuxMultiMsgManage.MsgIdx == m_BMS_AuxMultiMsgManage.m_BMSAuxMultiMsg.m_validMsgCnt)
        {
            m_BMS_AuxMultiMsgManage.MsgIdx = 0;
            m_BMS_AuxMultiMsgManage.bIsOneCycle = true;
            CommonFunc::GetTimeTicksUpdate(&m_BMS_AuxMultiMsgManage.m_SendInterval);
        }

    }
}
/******************************************************
** Module name:      CANBMS_ForwardMsg
** Descriptions:		AUX RX CAN BMS pack ,and
**							transfer to IV Unit
**Created Date:      2014/09/09
*******************************************************/
void CANBusParse::CANBMS_ForwardMsg(unsigned char CANx, const ST_CAN_RxMSG * OrgMsg)
{
    if(!m_CANBMS_AuxInfo.m_bAuxForward)
        return;

    // NVIC_DisableIRQ(CAN_IRQn);
    int bufidx = CANFunc::BMS_Filter.bufIndexEnqueue;
    int bufCounter = CANFunc::BMS_Filter.bufCounter[bufidx];
    if(bufidx > 1)
        memset(&CANFunc::BMS_Filter, 0, sizeof(CANFunc::BMS_Filter));
    if(bufCounter >= 1024)
    {
        NVIC_EnableIRQ(CAN_IRQn);
        return;
    }
    CANFunc::BMS_Filter.buf[bufidx][bufCounter] = *OrgMsg;
    CANFunc::BMS_Filter.bufCounter[bufidx]++;
    //  NVIC_EnableIRQ(CAN_IRQn);
// 	ENQUEUE(CANFunc::CQ_CanBmsRxBuf2,1024,*OrgMsg);
}
void CANBusParse::CANBMS_TrySwitchBuffer(void) //1712211 hpq
{
    NVIC_DisableIRQ(CAN_IRQn);
    CANFunc::BMS_Filter.bufIndexEnqueue = !CANFunc::BMS_Filter.bufIndexEnqueue;  //交换缓冲区
    CANFunc::BMS_Filter.bufCounter[CANFunc::BMS_Filter.bufIndexEnqueue] = 0;
    NVIC_EnableIRQ(CAN_IRQn);
    int bufidx = !CANFunc::BMS_Filter.bufIndexEnqueue;
    int bufCounter = CANFunc::BMS_Filter.bufCounter[bufidx];
    INIT_QUEUE(CANFunc::CQ_CanBmsRxBuf2);
    for(int i = 0; i < bufCounter; i++)
    {
        int queueLen = CANFunc::CQ_CanBmsRxBuf2.m_u32Length;
        bool bReplaced = false;
        for(int j = 0; j < queueLen; j++)
        {
            if(CANFunc::CQ_CanBmsRxBuf2.Object[j].RxID == CANFunc::BMS_Filter.buf[bufidx][i].RxID)
            {
                CANFunc::CQ_CanBmsRxBuf2.Object[j] = CANFunc::BMS_Filter.buf[bufidx][i];    //replace
                bReplaced = true;
                break;
            }
        }
        if(bReplaced == false)
        {
            ENQUEUE(CANFunc::CQ_CanBmsRxBuf2, 1024, CANFunc::BMS_Filter.buf[bufidx][i]); //insert
        }
    }
}
void CANBusParse::CANBMS_ForwardMsg2(unsigned char CANx)
{
    unsigned char m_CANPort = CANx;
    MP_MCU_MSG Temp_MCU_Msg;
    ST_CAN_RxMSG Msg; //zyx
    ST_CAN_RxMSG * OrgMsg = &Msg;//zyx
    bool bOK = true;
    DEQUEUE(CANFunc::CQ_CanBmsRxBuf2, 1024, *OrgMsg, bOK);
    if(!bOK)
    {
        if(CANFunc::BMS_Filter.bufCounter[CANFunc::BMS_Filter.bufIndexEnqueue] > 0)
            CANBMS_TrySwitchBuffer();
        return;
    }
    Temp_MCU_Msg.Un_CANID.m_CANID.DestID = CANBusParse::m_CANBMS_AuxInfo.m_IVCANID;//Aux MCU ID /IV MCU ID
    Temp_MCU_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
//   Temp_MCU_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    if(CANBusParse::st_CAN_SN.m_CANBMS_SN < CAN_MULTI_PACK_BUF_LEN - 1)
        CANBusParse::st_CAN_SN.m_CANBMS_SN++;
    else
        CANBusParse::st_CAN_SN.m_CANBMS_SN = 0;
    Temp_MCU_Msg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
    Temp_MCU_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_BMS_MSG;//FDCAN_CMD_FORWARD_MSG;
    Temp_MCU_Msg.Un_Content.m_BMSData.m_SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
    Temp_MCU_Msg.Un_Content.m_BMSData.CANid = OrgMsg->RxID;
    memcpy((void*)&Temp_MCU_Msg.Un_Content.m_BMSData.CANdata.ucCANData[0], (void*)&OrgMsg->RxData.uc_Data[0], sizeof(CANDATA));
    Temp_MCU_Msg.ByteLength = 4+sizeof(UN_CANID) + sizeof(ST_CANMsg);
    MultiMcuComm::NET_Push_to_Send(Temp_MCU_Msg);
}
// void CANBusParse::CANBMS_ForwardMsg2(void)
// {
//unsigned char m_CANPort = CANx;
//   MP_CAN_MSG Temp_CAN_Msg;
//           ST_CAN_RxMSG Msg; //zyx
//   ST_CAN_RxMSG * OrgMsg = &Msg;//zyx
//NVIC_DisableIRQ(CAN_IRQn);
// 	bool bOK = true;
//   DEQUEUE(CANFunc::CQ_CanBmsRxBuf2,1024,*OrgMsg,bOK);
//NVIC_EnableIRQ(CAN_IRQn);
// 	if(!bOK)
// 	{
// 		CANBMS_TrySwitchBuffer();
// 		return;
// 	}
// 	Temp_CAN_Msg.Un_CANID.m_CANID.DestID = m_CANBMS_AuxInfo.m_IVCANID;//Aux MCU ID /IV MCU ID
// 	Temp_CAN_Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
// 	Temp_CAN_Msg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
// 	Temp_CAN_Msg.DLC = 8;
// 	Temp_CAN_Msg.FrameType = 1;
// 	if(st_CAN_SN.m_CANBMS_SN<CAN_MULTI_PACK_BUF_LEN-1)
// 		st_CAN_SN.m_CANBMS_SN++;
// 	else
// 		st_CAN_SN.m_CANBMS_SN = 0;
// 	Temp_CAN_Msg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
// 	Temp_CAN_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRANS_FIRST_PACK;
// 	Temp_CAN_Msg.Un_Content.m_BMSFirstData.SN = CANBusParse::st_CAN_SN.m_CANBMS_SN;
// 	memcpy((void*)&Temp_CAN_Msg.Un_Content.m_BMSFirstData.CANID,(void*)&OrgMsg->RxID,4);
// 	memcpy((void*)&Temp_CAN_Msg.Un_Content.m_BMSFirstData.Bytes[0],(void*)&OrgMsg->RxData.uc_Data[0],3);
//CANFunc::CAN_Push_to_Send(m_CANPort,&Temp_CAN_Msg);
//     MCUCommun::NET_Push_to_Send(&Temp_CAN_Msg);
//second pack
// 	Temp_CAN_Msg.Un_CANID.m_CANID.Cmd = CAN_CMD_TRANS_SECOND_PACK;
// 	memcpy((void*)&Temp_CAN_Msg.Un_Content.m_BMSSecondData.Bytes[0],(void*)&OrgMsg->RxData.uc_Data[3],5);
// 	Temp_CAN_Msg.Un_Content.m_BMSSecondData.FrameType = OrgMsg->FrameType;
// 	Temp_CAN_Msg.Un_Content.m_BMSSecondData.DLC = OrgMsg->len;
// 	Temp_CAN_Msg.Un_Content.m_BMSSecondData.SN =  CANBusParse::st_CAN_SN.m_CANBMS_SN;
//CANFunc::CAN_Push_to_Send(m_CANPort,&Temp_CAN_Msg);
//     MCUCommun::NET_Push_to_Send(&Temp_CAN_Msg);
// }

// 如需要ack回复，则调用该函数(aux rx and feedback)
void CANBusParse::CANBMS_ConfirmAck(const MP_CAN_MSG *Rxmsg)
{
    MP_CAN_MSG AckMsg;
    unsigned char bufIdx = 0;

    AckMsg.Un_CANID.m_CANID.DestID = Rxmsg->Un_CANID.m_CANID.SourceID;
    AckMsg.Un_CANID.m_CANID.SourceID = Rxmsg->Un_CANID.m_CANID.DestID;
    AckMsg.Un_CANID.m_CANID.CmdType = BMS_CMD_TYPE;
    AckMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_BMS_ACK;
    AckMsg.DLC = 8;
    AckMsg.FrameType = CAN_EXT_FRAME;
//     AckMsg.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    AckMsg.Un_Content.m_CANBMS_Ack.m_CANID = Rxmsg->Un_Content.m_CANBMS_Ack.m_CANID;
    AckMsg.Un_Content.m_CANBMS_Ack.m_RxCmd = Rxmsg->Un_CANID.m_CANID.Cmd;
    AckMsg.Un_Content.m_CANBMS_Ack.m_SN = Rxmsg->Un_Content.m_CANBMS_Ack.m_SN;
    //CANFunc::CAN_Push_to_Send(CAN1,&AckMsg);
    MultiMcuComm::NET_Push_to_Send(CANBusParse::CanMsg2McuMsg(AckMsg));
}

/***********************************************************
** Module name:      CANBMS_AssemblePackets
** Descriptions:		aux rx multi packets
**
**Created Date:      2014/09/12
***********************************************************/
void CANBusParse::CANBMS_AuxAssembleMsgs(unsigned char CANx, const ST_CAN_RxMSG * OrgMsg)
{
    unsigned char SN = OrgMsg->RxData.uc_Data[0];
    if(SN >= CAN_MULTI_PACK_BUF_LEN)
        return;
    UN_CANID  MyCANID;
    MyCANID.m_U32ID = OrgMsg->RxID;
    bool bMultiMsg = false;
    switch(MyCANID.m_CANID.Cmd )
    {
// 	case CAN_CMD_TRANS_FIRST_PACK:
// 		bMultiMsg = true;
// 		ST_First_Pack FirstPack;
// 		CommonFunc::GetTimeTicksUpdate(&FirstPack.m_Overtime);
// 		memcpy((void*)&FirstPack.m_DataA.SN,(void*)&OrgMsg->RxData.uc_Data[0],8);
// 		if( (CANFunc::m_AuxMutiPackets[SN].Status& FIRST_PACK_FILL) == 0)
// 		{
// 			CANFunc::m_AuxMutiPackets[SN].m_FirstPack = FirstPack;
// 			CANFunc::m_AuxMutiPackets[SN].Status |= FIRST_PACK_FILL;
// 		}
// 		break;

// 	case CAN_CMD_TRANS_SECOND_PACK:
// 		bMultiMsg = true;
// 		ST_Second_Pack SecondPack;
// 		CommonFunc::GetTimeTicksUpdate(&SecondPack.m_Overtime);
// 		memcpy((void*)&SecondPack.m_DataB.SN,(void*)&OrgMsg->RxData.uc_Data[0],8);
// 		if( (CANFunc::m_AuxMutiPackets[SN].Status&SECOND_PACK_FILL) ==0)
// 		{
// 			CANFunc::m_AuxMutiPackets[SN].m_SecondPack = SecondPack;  //赋值
// 			CANFunc::m_AuxMutiPackets[SN].Status |= SECOND_PACK_FILL;
// 		}
// 		break;

    case CAN_CMD_TRANS_THRID_PACK:
        MP_CAN_MSG thirdMsg;
        thirdMsg.bBMSCmd = 1;
        thirdMsg.SubBmsCmd = CAN_CMD_TRANS_THRID_PACK;
        thirdMsg.Un_CANID.m_U32ID = MyCANID.m_U32ID;
        memcpy((void*)&thirdMsg.Un_Content.Data[0], (void*)&OrgMsg->RxData.uc_Data[0], 8);
        //CANFunc::m_AuxMutiPackets[SN].m_ThirdPack = thirdMsg.Un_Content.m_BMSThirdData;
        //NVIC_DisableIRQ(CAN_IRQn);
        ENQUEUE(CANFunc::CQ_CanRxBuffer[CANx], CAN_RX_BUF_LENGTH, thirdMsg);
        //NVIC_EnableIRQ(CAN_IRQn);
        break;
    case CAN_CMD_SET_BMS_BAUDRATE:
        MP_CAN_MSG SetBaudMsg;
        SetBaudMsg.bBMSCmd = 1;
        SetBaudMsg.SubBmsCmd = CAN_CMD_SET_BMS_BAUDRATE;
        SetBaudMsg.Un_CANID.m_U32ID = MyCANID.m_U32ID;
        memcpy((void*)&SetBaudMsg.Un_Content.Data[0], (void*)&OrgMsg->RxData.uc_Data[0], 8);
        //NVIC_DisableIRQ(CAN_IRQn);
        ENQUEUE(CANFunc::CQ_CanRxBuffer[CANx], CAN_RX_BUF_LENGTH, SetBaudMsg);
        //NVIC_EnableIRQ(CAN_IRQn);
        break;
    case CAN_CMD_BMS_MSG:
        MP_CAN_MSG AssembleMsg;
        AssembleMsg.Un_CANID.m_U32ID = OrgMsg->RxData.m_BMSData.CANid;
        memcpy(&AssembleMsg.Un_Content.Data[0], &OrgMsg->RxData.m_BMSData.CANdata,OrgMsg->RxData.m_BMSData.DLC);
        AssembleMsg.FrameType = OrgMsg->RxData.m_BMSData.FrameType;
        AssembleMsg.bBroadCast = OrgMsg->RxData.m_BMSData.bBroadCast;

        AssembleMsg.DLC = OrgMsg->RxData.m_BMSData.DLC;
        AssembleMsg.bBMSCmd = 1;
        AssembleMsg.SubBmsCmd = CAN_CMD_TRANS_FIRST_PACK;   //divide  different commands in parse module
        AssembleMsg.bBMSOnceTimeMsg = OrgMsg->RxData.m_BMSData.bOnceTime;
        //update broadcast msg

        if(OrgMsg->RxData.m_BMSData.bBroadCast)
        {
            m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_Endian = OrgMsg->RxData.m_BMSData.bBigEndian;
            CANBMS_UpdateBrdCstMsg(&AssembleMsg);
            return;
        }
        else
        {
            MP_CAN_MSG confirmMsg;
            confirmMsg.Un_CANID.m_U32ID = OrgMsg->RxID;
            confirmMsg.Un_Content.m_CANBMS_Ack.m_SN = CANFunc::m_AuxMutiPackets[SN].m_FirstPack.m_DataA.SN;
            confirmMsg.Un_Content.m_CANBMS_Ack.m_CANID = CANFunc::m_AuxMutiPackets[SN].m_FirstPack.m_DataA.CANID.m_U32ID;
            CANBMS_ConfirmAck(&confirmMsg);  //add ack
        }
        if( AssembleMsg.bBMSOnceTimeMsg)
        {
            CANFunc::CAN_Push_to_Send(CAN2, &AssembleMsg);
            return;
        }
        // NVIC_DisableIRQ(CAN_IRQn);
        ENQUEUE(CANFunc::CQ_CanRxBuffer[CANx], CAN_RX_BUF_LENGTH, AssembleMsg);
        return;
    //  NVIC_EnableIRQ(CAN_IRQn);
    default:
        break;
    }
}

/***********************************************************
** Module name:      CANBMS_UpdateBrdCstMsg
** Descriptions:		aux Send Broadcast Msg
**                    it is triggered by IV
**Created Date:      2014/09/12
***********************************************************/
void CANBusParse::CANBMS_UpdateBrdCstMsg(MP_CAN_MSG *Parse_Msg)
{
    m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.FrameType =  Parse_Msg->FrameType;
    // m_CANBMS_AuxInfo.m_BrdCstInfo.m_BradCastMsg.Un_CANID.m_U32ID = Parse_Msg->Un_CANID.m_U32ID;
    if( m_CANBMS_AuxInfo.m_bAuxForward)
        m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_bValidBrdcst = 1;
    else
        m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_bValidBrdcst = 0;
    m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.DLC = 8;
    m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_FrameType = Parse_Msg->FrameType;
    //m_CANBMS_AuxInfo.m_BrdCstInfo.m_Endian = Parse_Msg->Un_Content.m_BMSThirdData.m_Endian;
    m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg = *Parse_Msg;
    if(m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_Endian == e_BigOrder)
    {
        //need Add process
        unsigned int EndianLow,EndianHigh;
        EndianLow = m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.Un_Content.LLData&0xFFFFFFFF;	// 8 Byte get 4 low Byte
        EndianHigh = ( m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.Un_Content.LLData>>32)&0xFFFFFFFF;	// 8 Byte get 4 high Byte
        //m_CANBMS_AuxInfo.m_BrdCstInfo.m_BradCastMsg.Un_Content.LLData = __REV(EndianLow);  // Reverse, 32bit---0bit exchange 0bit---32bit
        //m_CANBMS_AuxInfo.m_BrdCstInfo.m_BradCastMsg.Un_Content.LLData = ( m_CANBMS_AuxInfo.m_BrdCstInfo.m_BradCastMsg.Un_Content.LLData <<32)|__REV(EndianHigh);	// 8 Byte 4 change to 4, low 4 Byte change to high 4 Byte; Curren and Voltage exchange.
        m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.Un_Content.LLData = __REV(EndianHigh);  // Reverse, 32bit---0bit exchange 0bit---32bit   // 20170811 yang  Lock Current First
        m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.Un_Content.LLData = ( m_CANBMS_AuxInfo.m_BrdCstInfo[AuxBrdCstReciveIdex].m_BradCastMsg.Un_Content.LLData <<32)|__REV(EndianLow);	// 8 Byte high 4 Byte, low 4 Byte; Curren and Voltage.
    }
    AuxBrdCstReciveIdex++;
    if(AuxBrdCstReciveIdex>=BMS_BROAD_CST_COUNT)
        AuxBrdCstReciveIdex = 0;
}
#endif
#if(USE_AS_AUXMCU ==0)
void CANBusParse::DSP_IV_SendBasicParameterToDsp(DSP_Basic_Parameter * DspBasicPara,unsigned char DSP_ID)
{
    unsigned char TempSendData[16];
    WORD CRCValue;
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_BASIC_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    DSP_FirstMSG.Un_Content.m_Dsp_Basic_First_Pack.m_wSecondaryVoltage = (Word16)(DspBasicPara->m_fSecondaryVoltage*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Basic_First_Pack.m_wChannelVoltage = (Word16)(DspBasicPara->m_fChannelVoltage*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Basic_First_Pack.m_wDCVoltageOffset = (Word16)(DspBasicPara->m_fDCVoltageOffset*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Basic_First_Pack.m_btDspBoardType = DspBasicPara->m_btDspBoardType;
    DSP_FirstMSG.Un_Content.m_Dsp_Basic_First_Pack.m_btNone = 0;
    memcpy(&TempSendData[0], &DSP_FirstMSG.Un_Content.Data[0], 8);
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_BASIC_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.m_btSw_Freq = DspBasicPara->m_btSw_Freq;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.m_btGrid_Voltage = DspBasicPara->m_btGrid_Voltage;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.m_btHZ_XMFR = DspBasicPara->m_btHZ_XMFR;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.m_btDSPBoardType = DspBasicPara->m_btDSPBoardType;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.m_btNone[0] = 0;
    memcpy(&TempSendData[8], &DSP_SecondMSG.Un_Content.Data[0], 6);
    CRCValue = CANCalCRC16(TempSendData, 14);
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCL = CRCValue&0xFF;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCH = CRCValue>>8;
    Send_CAN_Cmd(&DSP_SecondMSG);
}
void CANBusParse::DSP_IV_SendAdjustParameterToDsp(DSP_Adjust_Parameter * DspAdjustPara,unsigned char DSP_ID)
{
    unsigned char TempSendData[16];
    WORD CRCValue;
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_ADJUST_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    DSP_FirstMSG.Un_Content.m_Dsp_Adjust_First_Pack.m_wV_KP = (Word16)(DspAdjustPara->m_fV_KP*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Adjust_First_Pack.m_wV_KI = (Word16)(DspAdjustPara->m_fV_KI*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Adjust_First_Pack.m_wI_KP = (Word16)(DspAdjustPara->m_fI_KP*10);
    DSP_FirstMSG.Un_Content.m_Dsp_Adjust_First_Pack.m_wI_KI	= (Word16)(DspAdjustPara->m_fI_KI*10);
    memcpy(&TempSendData[0], &DSP_FirstMSG.Un_Content.Data[0], 8);
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_ADJUST_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[0] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[1] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[2] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[3] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[4] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[5] = 0;
    memcpy(&TempSendData[8], &DSP_SecondMSG.Un_Content.Data[0], 6);
    CRCValue = CANCalCRC16(TempSendData, 14);
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCL = CRCValue&0xFF;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCH = CRCValue>>8;
    Send_CAN_Cmd(&DSP_SecondMSG);
}
void CANBusParse::DSP_IV_ReadBasicParameterFromDsp(unsigned char DSP_ID)
{
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_BASIC_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_BASIC_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_SecondMSG);
}
void CANBusParse::DSP_IV_ReadAdjustParameterFromDsp(unsigned char DSP_ID)
{
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_ADJUST_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_ADJUST_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_SecondMSG);
}

void CANBusParse::DSP_IV_SendIndentityParameterToDsp(DSP_Indentity_Parameter * DspIndentityPara,unsigned char DSP_ID)
{
    unsigned char TempSendData[16];
    WORD CRCValue;
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_INDENTITY_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    DSP_FirstMSG.Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_Indentity = DspIndentityPara->m_DSP_Indentity;
    DSP_FirstMSG.Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_SlaveCountOrMasterID = DspIndentityPara->m_DSP_SlaveCountOrMasterID;
    for(unsigned char i = 0; i< 6; i++)
        DSP_FirstMSG.Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_SlaveID[i] = DspIndentityPara->m_DSP_SlaveID[i];
    memcpy(&TempSendData[0], &DSP_FirstMSG.Un_Content.Data[0], 8);
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_SET_INDENTITY_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    for(unsigned char i = 0; i< 2; i++)
        DSP_FirstMSG.Un_Content.m_Dsp_Indentity_Second_Pack.m_DSP_SlaveID[i] = DspIndentityPara->m_DSP_SlaveID[6+i];

    DSP_SecondMSG.Un_Content.m_Dsp_Indentity_Second_Pack.m_btNone[0] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[1] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[2] = 0;
    DSP_SecondMSG.Un_Content.m_Dsp_Adjust_Second_Pack.m_btNone[3] = 0;

    memcpy(&TempSendData[8], &DSP_SecondMSG.Un_Content.Data[0], 6);
    CRCValue = CANCalCRC16(TempSendData, 14);
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCL = CRCValue&0xFF;
    DSP_SecondMSG.Un_Content.m_Dsp_Basic_Second_Pack.CRCH = CRCValue>>8;
    Send_CAN_Cmd(&DSP_SecondMSG);
}
void CANBusParse::DSP_IV_ReadIndentityParameterFromDsp(unsigned char DSP_ID)
{
    MP_CAN_MSG DSP_FirstMSG,DSP_SecondMSG;
    DSP_FirstMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_INDENTITY_FIRST_PACK;
    DSP_FirstMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_FirstMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_FirstMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_FirstMSG);
    DSP_SecondMSG.Un_CANID.m_CANID.Cmd = CAN_CMD_DSP_READ_INDENTITY_SECOND_PACK;
    DSP_SecondMSG.Un_CANID.m_CANID.CmdType = DSP_PARA_SET;
// 	DSP_SecondMSG.Un_CANID.m_CANID.Priority = CAN_PRIO_HIGH;
    DSP_SecondMSG.Un_CANID.m_CANID.DestID = DSP_ID;
    Send_CAN_Cmd(&DSP_SecondMSG);
}

void CANBusParse::DSP_Feedback_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg)
{
    if(CANx!=CAN1)
        return;
    switch(CAN_Msg->Un_CANID.m_CANID.Cmd )
    {
    case CAN_CMD_DSP_SEND_ERROR_PACK:
        if(CAN_Msg->Un_Content.m_Dsp_Report_Error.Command_identifier ==0x93)
        {
            uint32 INFU_Error=0;

            INFU_Error=CAN_Inverter_Fuse_Error(CANx,CAN_Msg);
            if(INFU_Error!=0)
            {
                safety_check::SetUnsafe(0, INFU_Error, 0,INVERTER_UNSAFE_TYPE);

                Schedule_Manager_task::UnsafeContent[0].st_status_sch_running. m_btLost_Unit = CAN_Msg->Un_CANID.m_CANID.SourceID-130;
            }
        }
        return;
    default:
        break;
    }
    if(CAN_Msg->Un_CANID.m_CANID.DestID != CANFunc::SourceBoardID )
        return;
    WORD CRCValue = 0;
    static unsigned char TempReciveData[16];
    unsigned char DSP_ID = CAN_Msg->Un_CANID.m_CANID.SourceID-130;
    if(DSP_ID>=MAX_DSP_NUMBER)
        DSP_ID = MAX_DSP_NUMBER-1;
    switch(CAN_Msg->Un_CANID.m_CANID.Cmd )
    {
    case CAN_CMD_DSP_SET_BASIC_FIRST_PACK:
    case CAN_CMD_DSP_READ_BASIC_FIRST_PACK:
        memcpy(&TempReciveData[0], &CAN_Msg->Un_Content.Data[0], 8);
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDspBoardType = CAN_Msg->Un_Content.m_Dsp_Basic_First_Pack.m_btDspBoardType  ;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fDCVoltageOffset = CAN_Msg->Un_Content.m_Dsp_Basic_First_Pack.m_wDCVoltageOffset/10.0 ;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fChannelVoltage = CAN_Msg->Un_Content.m_Dsp_Basic_First_Pack.m_wChannelVoltage/10.0 ;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_fSecondaryVoltage = CAN_Msg->Un_Content.m_Dsp_Basic_First_Pack.m_wSecondaryVoltage/10.0 ;
        break;
    case CAN_CMD_DSP_SET_BASIC_SECOND_PACK:
    case CAN_CMD_DSP_READ_BASIC_SECOND_PACK:
        memcpy(&TempReciveData[8], &CAN_Msg->Un_Content.Data[0], 8);
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btSw_Freq = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_btSw_Freq;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btGrid_Voltage = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_btGrid_Voltage;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btHZ_XMFR = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_btHZ_XMFR;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btDSPBoardType = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_btDSPBoardType;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_wDSP_Version = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_wDSP_Version;
        PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_wDSP_TYorZY = CAN_Msg->Un_Content.m_Dsp_Basic_Second_Pack.m_wDSP_TYorZY;
        CRCValue = CANCalCRC16(TempReciveData, 14);
        if((CRCValue>>8==CAN_Msg->Un_Content.Data[7])&&((CRCValue&0xFF) == CAN_Msg->Un_Content.Data[6]))
        {
            PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btReadResult = true;
        }
        else
        {
            PWM_Base::Record_DSP_Basic_Parameter[DSP_ID].m_btReadResult = false;
        }
        memset(TempReciveData,0,16);
        break;
    case CAN_CMD_DSP_SET_ADJUST_FIRST_PACK:
    case CAN_CMD_DSP_READ_ADJUST_FIRST_PACK:
        memcpy(&TempReciveData[0], &CAN_Msg->Un_Content.Data[0], 8);
        PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KP = CAN_Msg->Un_Content.m_Dsp_Adjust_First_Pack.m_wV_KP/10.0 ;
        PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fV_KI = CAN_Msg->Un_Content.m_Dsp_Adjust_First_Pack.m_wV_KI/10.0 ;
        PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KP = CAN_Msg->Un_Content.m_Dsp_Adjust_First_Pack.m_wI_KP/10.0 ;
        PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_fI_KI = CAN_Msg->Un_Content.m_Dsp_Adjust_First_Pack.m_wI_KI/10.0 ;
        break;
    case CAN_CMD_DSP_SET_ADJUST_SECOND_PACK:
    case CAN_CMD_DSP_READ_ADJUST_SECOND_PACK:
        memcpy(&TempReciveData[8], &CAN_Msg->Un_Content.Data[0], 8);
        CRCValue = CANCalCRC16(TempReciveData, 14);
        if((CRCValue>>8==CAN_Msg->Un_Content.Data[7])&&((CRCValue&0xFF) == CAN_Msg->Un_Content.Data[6]))
        {
            PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_btReadResult = true;
        }
        else
        {
            PWM_Base::Record_DSP_Adjust_Parameter[DSP_ID].m_btReadResult = false;
        }
        memset(TempReciveData,0,16);
        break;
    case CAN_CMD_DSP_SET_INDENTITY_FIRST_PACK:
    case CAN_CMD_DSP_READ_INDENTITY_FIRST_PACK:
        memcpy(&TempReciveData[0], &CAN_Msg->Un_Content.Data[0], 8);
        PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_Indentity = CAN_Msg->Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_Indentity;
        PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveCountOrMasterID = CAN_Msg->Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_SlaveCountOrMasterID;
        for(unsigned char i = 0; i<6; i++)
            PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveID[i] = CAN_Msg->Un_Content.m_Dsp_Indentity_First_Pack.m_DSP_SlaveID[i];
        break;
    case CAN_CMD_DSP_SET_INDENTITY_SECOND_PACK:
    case CAN_CMD_DSP_READ_INDENTITY_SECOND_PACK:
        memcpy(&TempReciveData[8], &CAN_Msg->Un_Content.Data[0], 8);
        for(unsigned char i = 0; i<2; i++)
            PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_DSP_SlaveID[6+i] = CAN_Msg->Un_Content.m_Dsp_Indentity_Second_Pack.m_DSP_SlaveID[i];
        CRCValue = CANCalCRC16(TempReciveData, 14);
        if((CRCValue>>8==CAN_Msg->Un_Content.Data[7])&&((CRCValue&0xFF) == CAN_Msg->Un_Content.Data[6]))
        {
            PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_btReadResult = true;
        }
        else
        {
            PWM_Base::Record_DSP_Indentity_Parameter[DSP_ID].m_btReadResult = false;
        }
        memset(TempReciveData,0,16);
        break;
    }
}

unsigned short CANBusParse::CANCalCRC16(unsigned char *ptr,unsigned char count)
{

    unsigned short crc =0;
    unsigned char i;
    while(count-- >0)
    {
        crc = ( crc^(((unsigned short)*ptr)<<8));
        for(i=0; i<8; i++)
        {
            if(crc&0x8000)
                crc= ((crc<<1)^0x1021);
            else
                crc <<= 1;
        }
        ptr++;
    }
    return crc;
}
void CANBusParse::IV_SendTOSysCtrl_BackToNormal(void)
{
    MP_CAN_MSG m_BackToNormalMsg;
    //unsigned char m_CANType = CAN_PORT_USE_PUBLIC,
    unsigned char chn = 0;  //
    m_BackToNormalMsg.Un_CANID.m_CANID.CmdType = SYSTEMCTRL_SET;
    m_BackToNormalMsg.Un_CANID.m_CANID.Cmd = CAN_CMD_SYSTEM_BACKTONORMAL;
    m_BackToNormalMsg.Un_CANID.m_CANID.DestID = SYSTEMCTRLBOARD_ID;
    m_BackToNormalMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    m_BackToNormalMsg.Un_Content.m_SysAlarmMsg.sys_unitID=CANFunc::SourceBoardID;
    m_BackToNormalMsg.Un_Content.m_SysAlarmMsg.sys_RUNStatus=LEDFunc::m_SysStatus;
    m_BackToNormalMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus=Schedule_Manager_task::M_SysALarmEmergencyFlg;
    st_CAN_SN.m_bResendPara = 0; //new msg

    m_BackToNormalMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampHigh;
    m_BackToNormalMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampLow;
    //Send_CAN_Cmd(m_CANType,&m_BackToNormalMsg);
}
void CANBusParse::IV_SendTOSysCtrl_Abnormal(void)
{
    MP_CAN_MSG m_AbnormalMsg;
    //unsigned char m_CANType = CAN_PORT_USE_PUBLIC,
    unsigned char chn = 0;  //
    m_AbnormalMsg.Un_CANID.m_CANID.CmdType = SYSTEMCTRL_SET;
    m_AbnormalMsg.Un_CANID.m_CANID.Cmd =CAN_CMD_SYSTEM_ABNORMAL;
    m_AbnormalMsg.Un_CANID.m_CANID.DestID = SYSTEMCTRLBOARD_ID;
    m_AbnormalMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    m_AbnormalMsg.Un_Content.m_SysAlarmMsg.sys_unitID=CANFunc::SourceBoardID;
    m_AbnormalMsg.Un_Content.m_SysAlarmMsg.sys_RUNStatus=LEDFunc::m_SysStatus;
    m_AbnormalMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus=Schedule_Manager_task::M_SysALarmEmergencyFlg;
    st_CAN_SN.m_bResendPara = 0; //new msg

    m_AbnormalMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampHigh;
    m_AbnormalMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampLow;
    //Send_CAN_Cmd(m_CANType,&m_AbnormalMsg);
}
void CANBusParse::IV_SendTOSysCtrl_RUN(void)
{
    MP_CAN_MSG m_RunMsg;
    //unsigned char m_CANType = CAN_PORT_USE_PUBLIC,chn = 0;  //
    m_RunMsg.Un_CANID.m_CANID.CmdType = SYSTEMCTRL_SET;
    m_RunMsg.Un_CANID.m_CANID.Cmd =CAN_CMD_SYSTEM_RUN;
    m_RunMsg.Un_CANID.m_CANID.DestID = SYSTEMCTRLBOARD_ID;
    m_RunMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    m_RunMsg.Un_Content.m_SysAlarmMsg.sys_unitID=CANFunc::SourceBoardID;
    m_RunMsg.Un_Content.m_SysAlarmMsg.sys_RUNStatus=LEDFunc::m_SysStatus;
    m_RunMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus=Schedule_Manager_task::M_SysALarmEmergencyFlg;
    st_CAN_SN.m_bResendPara = 0; //new msg

    //m_RunMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampHigh;
    //m_RunMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampLow;
    Send_CAN_Cmd(&m_RunMsg);
}
void CANBusParse::IV_SendTOSysCtrl_STOP(void)
{
    MP_CAN_MSG m_StopMsg;
    //unsigned char m_CANType = CAN_PORT_USE_PUBLIC,chn = 0;  //
    m_StopMsg.Un_CANID.m_CANID.CmdType = SYSTEMCTRL_SET;
    m_StopMsg.Un_CANID.m_CANID.Cmd =CAN_CMD_SYSTEM_STOP;
    m_StopMsg.Un_CANID.m_CANID.DestID = SYSTEMCTRLBOARD_ID;
    m_StopMsg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID;
    m_StopMsg.Un_Content.m_SysAlarmMsg.sys_unitID=CANFunc::SourceBoardID;
    m_StopMsg.Un_Content.m_SysAlarmMsg.sys_RUNStatus=LEDFunc::m_SysStatus;
    m_StopMsg.Un_Content.m_SysAlarmMsg.sys_ALARMStatus=Schedule_Manager_task::M_SysALarmEmergencyFlg;
    st_CAN_SN.m_bResendPara = 0; //new msg

    //m_StopMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolHigh = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampHigh;
    //m_StopMsg.Un_Content.m_ParallelClampVolSet.m_ClampVolLow = ChannelInfo_Manager::ChannelInfo[chn].m_fVclampLow;
    Send_CAN_Cmd(&m_StopMsg);
}
#endif
uint32 CANBusParse::CAN_Inverter_Fuse_Error(unsigned char CANx, const MP_CAN_MSG * Parse_Msg)
{
    uint32 Error=0;
    Error = Parse_Msg->Un_Content.m_Dsp_Report_Error.ErrorCodeHigh8Bit;
    Error = Error<<24;
    Error = Error | Parse_Msg->Un_Content.m_Dsp_Report_Error.ErrorCodeLow24Bit;

    return Error;

    /*
        unsigned char Error = 0;
        if((CANx == CAN1) && (Parse_Msg->Un_Content.Data[0] == 0x93) && (Parse_Msg->Un_CANID.m_CANID.DestID == 0xff)\
                && (Parse_Msg->Un_Content.Data[7] == 0x00) && (Parse_Msg->Un_CANID.m_CANID.SourceID <= 32))
        {
            if(Parse_Msg->Un_Content.Data[1] != 0)
            {
                Error = 0;
                for(unsigned char i = 0; i < 3; i++)
                    if(Parse_Msg->Un_Content.Data[1] >> i == 1)
                    {
                        Error = i + 1;

                    }

            }

            if(Parse_Msg->Un_Content.Data[2] != 0)
            {
                Error = 0;
                for(unsigned char i = 0; i < 8; i++)
                    if(Parse_Msg->Un_Content.Data[2] >> i == 1)
                    {
                        Error = i + 4;

                    }
            }

            if(Parse_Msg->Un_Content.Data[3] != 0)
            {
                Error = 0;
                for(unsigned char i = 0; i < 2; i++)
                    if(Parse_Msg->Un_Content.Data[3] >> i == 1)
                    {
                        Error = i + 19;

                    }
            }

            if(Parse_Msg->Un_Content.Data[4] != 0)
            {
                Error = 0;
                for(unsigned char i = 0; i < 7; i++)
                    if(Parse_Msg->Un_Content.Data[4] >> i == 1)
                    {
                        Error = i + 12;

                    }
            }

            return ((Parse_Msg->Un_CANID.m_CANID.SourceID - 0x82) << 8) | Error;
        }
        else
            return 0xff;*/
}


#if  USE_AS_AUXMCU == 0
void CANBusParse::Send_M0_COMMON2Data(unsigned char m_com,St_PowerData m_data)
{

    MP_CAN_MSG Msg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //公有CAN
    Msg.Un_CANID.m_CANID.CmdType = Power_M0_ADJ;
    Msg.Un_CANID.m_CANID.Cmd = m_com;

    Msg.Un_Content.m_IV_SendToM0.m_Vpp = m_data.m_Vpp;
    Msg.Un_Content.m_IV_SendToM0.m_Vmm = m_data.m_Vmm;

    Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //
    Msg.Un_CANID.m_CANID.DestID = 0xFF;   //128+m_ID;  //暂时定死， 在IV IP 基础上加200
// 	Msg.Un_CANID.m_CANID.Priority =CAN_PRIO_HIGH;
    for(unsigned char uc_Idx = 0; uc_Idx<ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits; uc_Idx++)
    {
        if(m_Para_PowerAdjAckMgr.m_M0Status[uc_Idx]!=eParaAckWait)
        {
            m_Para_PowerAdjAckMgr.m_M0Status[uc_Idx]=eParaAckWait;
        }
        else
            return;
    }
    m_Para_PowerAdjAckMgr.CmdStatus=eParaAckWait;
    memcpy((void*)&m_Para_PowerAdjAckMgr.m_Msg,(void*)&Msg,sizeof(MP_CAN_MSG));
    //send can cmd
    Send_CAN_Cmd(&Msg);
    CommonFunc::GetTimeTicksUpdate(&m_Para_PowerAdjAckMgr.m_OverTime);
    m_Para_PowerAdjAckMgr.m_ReSendCnt = 0;

}
void CANBusParse::PowerAdj_DoAckCheck(void)
{
    if(m_Para_PowerAdjAckMgr.CmdStatus != eParaAckWait)
        return;
    if(CommonFunc::GetTimeTicks(&m_Para_PowerAdjAckMgr.m_OverTime)>PARA_MAX_OVERTIME)
    {   //resend MSG
        if(m_Para_PowerAdjAckMgr.m_ReSendCnt<5)  // 200*20*5*3=6s  UDP_Parallel_WatchdogRate
        {
            m_Para_PowerAdjAckMgr.m_ReSendCnt++;
            MP_CAN_MSG ResendMsg = m_Para_PowerAdjAckMgr.m_Msg;
            st_CAN_SN.m_bResendPara = 1;			//is resend msg
            ResendMsg.Un_CANID.m_CANID.CmdType = Power_M0_ADJ;
// 			ResendMsg.Un_Content.m_ParallelStart.SN = m_Para_MasterAckMgr[cmdIdx][BufIdx].SN;   //resend sn
            Send_CAN_Cmd(&ResendMsg);
            st_CAN_SN.m_bResendPara = 0;    //resume normal send sn increase
            CommonFunc::GetTimeTicksUpdate(&m_Para_PowerAdjAckMgr.m_OverTime);
        }
        else
        {   //失败应该报error
            PowerAdj::m_PowerAdjStatus = POWERADJ_COMMUNATION_ERROR;
// 			m_Para_PowerAdjAckMgr.m_ReSendCnt = 0;
            m_Para_PowerAdjAckMgr.CmdStatus = eParaAckNone;
            memset((void*)&m_Para_PowerAdjAckMgr.m_M0Status[0],eParaAckNone,4);
        }
    }
    else
        PowerAdj_CheckAck();  //未超时，则做检查
}
//ack check 函数 <Master>
//return bool (true/false)
bool CANBusParse::PowerAdj_CheckAck(void)
{
    unsigned char bAllReceive=1;

    if(m_Para_PowerAdjAckMgr.CmdStatus == eParaAckOK || m_Para_PowerAdjAckMgr.CmdStatus ==eParaAckNone)
        return true;

    for(unsigned char subIdx = 0; subIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits; subIdx++)
    {
        if(m_Para_PowerAdjAckMgr.m_M0Status[subIdx] == eParaAckWait) //false
            bAllReceive = 0;
    }

    if(bAllReceive)
    {
        m_Para_PowerAdjAckMgr.CmdStatus = eParaAckOK;
        return true;
    }
    else
        return false;
}
void CANBusParse::PowerAdj_AckProcess(MP_CAN_MSG* CAN_msg)
{
    unsigned char SubIdx = 0;
    for(unsigned char BufIdx = 0; BufIdx <m_Para_PowerAdjAckMgr.m_M0UnitCnt; BufIdx++)
    {
        if(m_Para_PowerAdjAckMgr.m_M0Status[BufIdx] !=eParaAckWait)
            continue;
        SubIdx = 0;
        unsigned char MaxUnitCnt = m_Para_PowerAdjAckMgr.m_M0UnitCnt;
        while(MaxUnitCnt--)
        {
            if(m_Para_PowerAdjAckMgr.m_M0ID[SubIdx] == CAN_msg->Un_CANID.m_CANID.SourceID)
                m_Para_PowerAdjAckMgr.m_M0Status[SubIdx] =eParaAckOK;
            SubIdx++;
        }
    }
}
// void CANBusParse::Send_M0_Config(unsigned char m_com,unsigned char m_Unit)
// {
// 	MP_CAN_MSG Msg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //公有CAN
// 	Msg.Un_CANID.m_CANID.CmdType = Power_M0_ADJ;
// 	Msg.Un_CANID.m_CANID.Cmd = m_com;

// 	Msg.Un_Content.m_IV_SendConfig.m_Units = m_Unit;
// 	memset(Msg.Un_Content.m_IV_SendConfig.m_Reserve,0, 7);
// // 	Msg.Un_Content.m_IV_SendConfig.m_Reserve ;
//
// 	Msg.Un_CANID.m_CANID.SourceID = CANFunc::SourceBoardID; //
// 	Msg.Un_CANID.m_CANID.DestID = 129;  //暂时定死， 在IV IP 基础上加200
// 		//send can cmd
// 	Send_CAN_Cmd(m_CANType,&Msg);
// }
// void CANBusParse::Send_M0_WatchDog(unsigned char m_SN)
// {
//   MP_CAN_MSG Msg;
// 	unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //公有CAN
// 	Msg.Un_CANID.m_CANID.CmdType = Power_M0_ADJ;

// 	Msg.Un_CANID.m_CANID.DestID = CANFunc::SourceBoardID+100;  //暂时定死， 在IV IP 基础上加200

// 	//send can cmd
// 	Send_CAN_Cmd(m_CANType,&Msg);
// }

void CANBusParse::Power_M0_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg)
{
//     unsigned char errCode = 0;
// 	float m_Vpp0, m_Vmm0;
// 	unsigned char SourceID_M0 = CAN_Msg->Un_CANID.m_CANID.SourceID;
// 	if(CANFunc::SourceBoardID!=CAN_Msg->Un_CANID.m_CANID.DestID)
// 	{
// 		if( CAN_Msg->Un_CANID.m_CANID.DestID !=255)
// 		return;
// 	}
// 	m_Vpp0 = PowerAdj::m_PowerValue.m_Vpp - 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
// 	m_Vmm0 =  abs(PowerAdj::m_PowerValue.m_Vmm-0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm);
    switch(CAN_Msg->Un_CANID.m_CANID.Cmd)
    {
    case	CAN_CMD_M0_REQUIREACK:
// 					    PowerAdj::ReceiveSN = CAN_Msg->Un_Content.m_WatchdogACK.SN;
        PowerAdj_AckProcess(CAN_Msg);
        break;
    case CAM_CMD_M0_REPORT:
// 			   PowerAdj::Receive_ACK_Status =1;
        unsigned char uc_id=CAN_Msg->Un_CANID.m_CANID.SourceID-129;
        PowerAdj::m_M0Feedback_Value[uc_id].m_Vpp = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;
        PowerAdj::m_M0Feedback_Value[uc_id].m_Vmm = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
        PowerAdj::uc_M0ADData_UpdataFlag=1;
        break;
    /*		case CAN_CMD_M0_START_ACK:
    			PowerAdj::m_M0Feedback_Value.m_Vpp = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;
    			PowerAdj::m_M0Feedback_Value.m_Vmm = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
    			if(( PowerAdj::m_M0Feedback_Value.m_Vpp < m_Vpp0) || (PowerAdj::m_M0Feedback_Value.m_Vmm <m_Vmm0))
    			{
    				PowerAdj::PowerAdjWorkFlag = false;
                    PowerAdj::b_M0ADCValueUnsafeFlag = true;
                }
    			else
    			{
    				  PowerAdj::Receive_M0_StartedACKFlag = true;
    				  switch(CAN_Msg->Un_CANID.m_CANID.SourceID)
    				     {
    					      case 0x81:
    									PowerAdj::Receive_M0_IDACKFlag[0]=true;
    									break;
    					      case 0x82:
    									PowerAdj::Receive_M0_IDACKFlag[1] = true;
    									break;
    					      case 0x83:
    									PowerAdj::Receive_M0_IDACKFlag[2] = true;
    									break;
    					     case 0x84:
    									PowerAdj::Receive_M0_IDACKFlag[3] = true;
    									break;
    							 default:
    									PowerAdj::PowerAdjWorkFlag = false;
    									PowerAdj::b_M0ADCValueUnsafeFlag = true;
    									break;
    				    }
    			}
    		    break;
    		case CAN_CMD_M0_RESTART_ACK:
    	        PowerAdj::m_M0Feedback_Value.m_Vpp = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;
    			PowerAdj::m_M0Feedback_Value.m_Vmm = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
    			if(( PowerAdj::m_M0Feedback_Value.m_Vpp < m_Vpp0) || (PowerAdj::m_M0Feedback_Value.m_Vmm <m_Vmm0))
    			{
    				Schedule_Manager_task::M4ReceiveACKCnt++;
    				if(Schedule_Manager_task::M4ReceiveACKCnt <Schedule_Manager_task::M4RestartSentCnt)
    				{
    					break;
                    }
    				else
                        PowerAdj::b_M0ADCValueUnsafeFlag = true;
                 }
    			else
    			{
    				PowerAdj::Receive_M0_StartedACKFlag =true;
    				switch(CAN_Msg->Un_CANID.m_CANID.SourceID)
    				{
    					 case 0x81:
    						PowerAdj::Receive_M0_IDACKFlag[0]=true;
    						break;
    					 case 0x82:
    						PowerAdj::Receive_M0_IDACKFlag[1] = true;
    						break;
    					 case 0x83:
    						PowerAdj::Receive_M0_IDACKFlag[2] = true;
    						break;
    					case 0x84:
    						PowerAdj::Receive_M0_IDACKFlag[3] = true;
    						break;
    					default:
    						PowerAdj::PowerAdjWorkFlag = false;
    						PowerAdj::b_M0ADCValueUnsafeFlag = true;
    						break;
    				}
    			PowerAdj::m_M0Feedback_Value.m_Vpp = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;
    			PowerAdj::m_M0Feedback_Value.m_Vmm = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
    			if(( PowerAdj::m_M0Feedback_Value.m_Vpp < m_Vpp0) &&(PowerAdj::m_M0Feedback_Value.m_Vmm <m_Vmm0))  //电源电压调节太小了，通道unsafe
    			{
    	            PowerAdj::Receive_ACK_Status=2;
                }
    			break;
    		case CAN_CMD_M0_STOP_ACK:  //master --> submaster: start
    // 			  PowerAdj::m_PowerAdjStatus = POWERADJ_IDLE;
                 PowerAdj::Receive_ACK_Status =3;
    	         PowerAdj::m_M0Feedback_Value.m_Vpp = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;
    			 PowerAdj::m_M0Feedback_Value.m_Vmm = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
    		     break;
    // 		case CAN_CMD_M0_Config:
    //             CANBusParse::Send_M0_Config(CAN_CMD_M0_Config_ACK,ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits);
    // 		    break;*/
    default:
        break;
    }
}
#else


void CANBusParse::Send_AUXToIV_COMMON2Data(unsigned char m_com,ST_M4_Value m_data)//
{

    MP_CAN_MSG Msg;
    unsigned char m_CANType = CAN_PORT_USE_PUBLIC;  //公有CAN
    Msg.Un_CANID.m_CANID.CmdType = Power_M0_ADJ;
    Msg.Un_CANID.m_CANID.Cmd = m_com;

    Msg.Un_Content.m_IV_SendToM0.m_Vpp = m_data.m_Vpp_Adc;
    Msg.Un_Content.m_IV_SendToM0.m_Vmm = m_data.m_Vmm_Adc;

// 	Msg.Un_CANID.m_CANID.SourceID = POWERADJ::BoardID+128; //
    Msg.Un_CANID.m_CANID.DestID = 0xFF;  //1+m_ID;  //暂时定死， 在IV IP 基础上加200
//	Msg.Un_CANID.m_CANID.Priority =CAN_PRIO_HIGH;

    //send can cmd
    Send_CAN_Cmd(&Msg);

}

void CANBusParse::Power_AUX_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg)
{
    unsigned char DequeueOK = 0;
    unsigned char CmdType;
    unsigned char ParseIdx;
// 	unsigned char m_MapIndex=0;
    unsigned char  cnt;
    unsigned char m_TableIndex=0;
    unsigned char mm;
    float data;
    unsigned char uc_Units;
    if(CANx!=CAN1)
        return;
//     if(CAN_Msg->Un_CANID.m_CANID.DestID!=CANFunc::SourceBoardID)
// 		return;
    switch(CAN_Msg->Un_CANID.m_CANID.Cmd)
    {
// 			case CAN_CMD_M0_START:
// 				POWERADJ::uc_SourceID =CAN_Msg->Un_CANID.m_CANID.SourceID-1;
// 				if(POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID]==0xFF)    //注册上该unit
// 				{
// 					POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID]=POWERADJ::m_UnitCnt;
// 					m_TableIndex = POWERADJ::m_UnitCnt;
// 					POWERADJ::m_UnitCnt++;
// 				}
// 				else
// 				{
// 					m_TableIndex =POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID];
// 				}
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_SourceID = POWERADJ::uc_SourceID;
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_Updataflag =true;
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_bNeedACK =true;
//         POWERADJ::m_PowerAdjData[m_TableIndex].m_ACK_Code	 = 	CAN_CMD_M0_START_ACK;
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_VppValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;//0.038462f; // +0.0297f;
//         POWERADJ::m_PowerAdjData[m_TableIndex].m_VmmValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
//
// 				break;
// 			case CAN_CMD_M0_RESTART:
// 				POWERADJ::uc_SourceID =CAN_Msg->Un_CANID.m_CANID.SourceID-1;
// 				m_TableIndex =POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID];
//
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_Updataflag =true;
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_bNeedACK =true;
//         POWERADJ::m_PowerAdjData[m_TableIndex].m_ACK_Code	 = 	CAN_CMD_M0_RESTART_ACK;
// 				POWERADJ::m_PowerAdjData[m_TableIndex].m_VppValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;//0.038462f; // +0.0297f;
//         POWERADJ::m_PowerAdjData[m_TableIndex].m_VmmValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
// //
// 				break;
// 			case CAN_CMD_M0_STOP:
// 				POWERADJ::uc_SourceID =CAN_Msg->Un_CANID.m_CANID.SourceID-1;
// 				m_TableIndex =POWERADJ:: m_DataSaveID[POWERADJ::uc_SourceID];
// 			  if((m_TableIndex!=0xFF)&&(POWERADJ::m_UnitCnt>0))//注销掉
// 				{
// 					POWERADJ::m_PowerAdjData[m_TableIndex] = POWERADJ::m_PowerAdjData[POWERADJ::m_UnitCnt-1];//把最尾部一个挪过来覆盖
// 					POWERADJ:: m_DataSaveID[POWERADJ::m_PowerAdjData[m_TableIndex].m_SourceID] = m_TableIndex;//把最后一个的位置号改一下
// 					POWERADJ:: m_DataSaveID[POWERADJ::uc_SourceID] = 0xFF;//把自己的位置号注销
// 					memset(&POWERADJ::m_PowerAdjData[POWERADJ::m_UnitCnt-1],  0,    sizeof(ST_PowerAdj_Para));//把最后一个位置注销掉
//           POWERADJ::m_UnitCnt = POWERADJ::m_UnitCnt-1;//总数减一
//         }
// 				CANBusParse::Send_AUXToIV_COMMON2Data(CAN_CMD_M0_STOP_ACK,POWERADJ::uc_SourceID,POWERADJ::m_M4_Value);
// 				break;
// 			case CAN_CMD_M0_REFRESHDATA:
    case CAN_CMD_M0_REQUIRE	:
        POWERADJ::uc_SourceID =CAN_Msg->Un_CANID.m_CANID.SourceID-1;
        if(POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID]==0xFF)    //注册上该unit
        {
            POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID]=POWERADJ::m_UnitCnt;
            m_TableIndex = POWERADJ::m_UnitCnt;
            POWERADJ::m_UnitCnt++;
        }
        else
        {
            m_TableIndex =POWERADJ::m_DataSaveID[POWERADJ::uc_SourceID];
        }
        POWERADJ::m_PowerAdjData[m_TableIndex].m_SourceID = POWERADJ::uc_SourceID;
// 			POWERADJ::m_PowerAdjData[m_TableIndex].m_Updataflag =true;
        POWERADJ::m_PowerAdjData[m_TableIndex].m_VppValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vpp;//0.038462f; // +0.0297f;
        POWERADJ::m_PowerAdjData[m_TableIndex].m_VmmValue = CAN_Msg->Un_Content.m_IV_SendToM0.m_Vmm;
        CANBusParse::Send_AUXToIV_COMMON2Data(CAN_CMD_M0_REQUIREACK,POWERADJ::m_M4_Value);//
        POWERADJ::RefreshFlag=1;
        POWERADJ::SendFeedBackFlag=true;
        break;
//             CANBusParse::Send_M0_Config(CAN_CMD_M0_Config_ACK,ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits);
    default:
        break;
    }
}
#endif
