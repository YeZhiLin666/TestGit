///////////////////////////////////////////////////////////////////////////////////
//File Name: Step.cpp
//Version:        V1.0
//Date:                2012/11/12
//Description:
//                        Step管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

#define  DEF_CCCV_RESUME_STAGE_COUNT      100  //  500  // 09.09.2015
#define  DEF_CV_RESUME_DELAY              1   //bool ,1:enable delay      0:disable delay

#define  MAX_CCCV_PARAMOD_TIME                  100 // 10ms




UInt16 Step_Manager_task::Priority = PRIORITY4;
UInt16 Step_Manager_task::TaskID = STEP_TASK;

St_CurFineTuneFactor Step_Manager_task::m_CurFineTuneFactor;
unsigned long       Step_Manager_task::DacOutputCounter;
unsigned char       Step_Manager_task::bDoneSLThisCycle;
unsigned char       Step_Manager_task::bNewSimuLoaded[MAXCHANNELNO];
unsigned char       Step_Manager_task::bNeedCheck[MAXCHANNELNO];
unsigned char       Step_Manager_task::bStepLimitChecked[MAXCHANNELNO];
unsigned char       Step_Manager_task::bLogLimitChecked[MAXCHANNELNO];
St_LimitPointer     Step_Manager_task::SL_StartPtr;
St_LimitPointer     Step_Manager_task::SL_CurrentPtr;
St_LimitPointer     Step_Manager_task::LL_StartPtr;
St_LimitPointer     Step_Manager_task::LL_CurrentPtr;
St_DoStepMessage    Step_Manager_task::DoStepMessageBuf[MAXCHANNELNO];    //建立当前处理的信息缓冲
St_DisposableMSG    Step_Manager_task::StepDisposableMSG[MAXCHANNELNO];                 //引擎任务信息
unsigned char       Step_Manager_task::StepTimeTrigger[MAXCHANNELNO];   //计时标记用于超时

unsigned char       Step_Manager_task::PULSE_bNeedCheck[MAXCHANNELNO];
St_LimitPointer     Step_Manager_task::PULSE_SL_CurrentPtr;
St_LimitPointer		Step_Manager_task::PULSE_LL_CurrentPtr;
unsigned char       Step_Manager_task::PULSE_bStepLimitChecked[MAXCHANNELNO];
unsigned char       Step_Manager_task::PULSE_bLogLimitChecked[MAXCHANNELNO];
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_StepMessage      Step_Manager_task::StepMessageBuf[MAXMESSAGE][MAXCHANNELNO];  //建立通道Step信息队列

St_AuxLimit			Step_Manager_task::AuxLimitBuf[MAXCHANNELNO][MAXAUXLIMIT];
St_GetStepRunInfo   Step_Manager_task::GetStepRunInfo[MAXCHANNELNO];
MsTime Step_Manager_task::Parallel_WaitSlaveChnDisposableFinish_Time[MAXCHANNELNO];
#pragma arm section //结束外存变量定义    

unsigned char       Step_Manager_task::ScanStepLimitNum[MAXCHANNELNO];
unsigned char       Step_Manager_task::StepLimitReach[MAXCHANNELNO];
unsigned char       Step_Manager_task::LimitTimeDelayTirgger[MAXCHANNELNO];
St_MsgBufManage     Step_Manager_task::MsgBufManage[MAXCHANNELNO];

avg          Step_Manager_task::Voltavg[MAXCHANNELNO];
avg          Step_Manager_task::Curravg[MAXCHANNELNO];
bool         Step_Manager_task::IsSetCCCV_StageTime;
float 			 Step_Manager_task::CC_Current[MAXCHANNELNO];


float               Step_Manager_task::m_FSet[MAXCHANNELNO];
unsigned char       Step_Manager_task::m_Range[MAXCHANNELNO];
unsigned long       Step_Manager_task::m_DacCode[MAXCHANNELNO];
unsigned char       Step_Manager_task::StepRunningStatus[MAXCHANNELNO];

MsTime             	Step_Manager_task::SysTimeEngineTask[MAXCHANNELNO];
MsTime              Step_Manager_task::MsTimeEngineTask[MAXCHANNELNO];
unsigned short      Step_Manager_task::EngineHandle[MAXCHANNELNO];
unsigned char       Step_Manager_task::EngineOverTimeTriger[MAXCHANNELNO];
unsigned char       Step_Manager_task::TimeRefreshFlag[MAXCHANNELNO];

St_AuxLimitAckMgr	Step_Manager_task::AuxLimitAckMgr[MAXCHANNELNO];
unsigned char       Step_Manager_task::ChannelCVStatus[MAXCHANNELNO];  //CV状态机 add by qjm 20141016
unsigned int        Step_Manager_task::ChannelCVCounter[MAXCHANNELNO];
float               Step_Manager_task::CV2CCCV_K_gain[MAXCHANNELNO];
unsigned char       Step_Manager_task::CCCV_RAMP_Status[MAXCHANNELNO];
float               Step_Manager_task::f_CCCV_Ramp_V0[MAXCHANNELNO];
float               Step_Manager_task::f_CCCV_Ramp_I0[MAXCHANNELNO];
MsTime              Step_Manager_task::m_CCCV_Ramp_ParaModTime[MAXCHANNELNO];
MsTime              Step_Manager_task::CCCV_RisingTime[MAXCHANNELNO]; //judge channel is it over rising time
bool                Step_Manager_task::b_IsRapidlyCommun[MAXCHANNELNO];
MsTime              Step_Manager_task::m_RepidlyCommunOverTimeCheck;
float               Step_Manager_task::CV_StartVoltage[MAXCHANNELNO];
float               Step_Manager_task::CV_TargetVoltage[MAXCHANNELNO];
float               Step_Manager_task::CV2CCCVRefreshValue[MAXCHANNELNO];
unsigned int        Step_Manager_task::uw16_CVResumeStageCounter[MAXCHANNELNO];   //add by qjm 2014/10/22
unsigned char       Step_Manager_task::NewCodeRefreshed[16];
unsigned char       Step_Manager_task::ChannelCCCVStatus[MAXCHANNELNO]; //CCCV状态机
//====================CCCV_TB 07.07.2017 BY ZC=================================================================
unsigned char       Step_Manager_task::ChannelTBStatus[MAXCHANNELNO]; //06.28.2017 TURBO BOOST状态机 by ZC
float               Step_Manager_task::CCCV_TB_TargetVoltage[MAXCHANNELNO]; //恒压电压值
float               Step_Manager_task::CCCV_TB_ChargeCurrent[MAXCHANNELNO];//充电电流
float               Step_Manager_task::CCCV_TB_ChargeTime[MAXCHANNELNO]; //充电电流每个周期持续时间
float               Step_Manager_task::CCCV_TB_DischargeCurrent[MAXCHANNELNO]; //放电电流
float               Step_Manager_task::CCCV_TB_DischargeTime[MAXCHANNELNO]; //放电电流每个周期持续时间
float               Step_Manager_task::CCCV_TB_DV_PulseCount[MAXCHANNELNO];
float               Step_Manager_task::CCCV_TB_PULSECOUNTER[MAXCHANNELNO];
float								Step_Manager_task::CCCV_TB_IR[MAXCHANNELNO][CCCV_TB_IR_COUNTLIMIT];
int									Step_Manager_task::CCCV_TB_IR_Pointer[MAXCHANNELNO];
int									Step_Manager_task::CCCV_TB_UPDATE_FLAG[MAXCHANNELNO];
bool       					Step_Manager_task::CCCV_TB_NeedExtLogFlg[MAXCHANNELNO];
//====================CC_TB 06.04.2018 BY ZC===========================================================
unsigned char       Step_Manager_task::ChannelCC_TBStatus[MAXCHANNELNO];//06.04.2018 TURBO BOOST BY ZC
float               Step_Manager_task::CC_TB_ChargeCurrent[MAXCHANNELNO];//06.04.2018  充电电流
float               Step_Manager_task::CC_TB_ChargeTime[MAXCHANNELNO]; //06.04.2018  充电电流每个周期持续时间
float               Step_Manager_task::CC_TB_DischargeCurrent[MAXCHANNELNO]; //06.04.2018  放电电流
float               Step_Manager_task::CC_TB_DischargeTime[MAXCHANNELNO]; //06.04.2018  放电电流每个周期持续时间
//=======================================================================================================
unsigned int       	Step_Manager_task::ChannelCCCVCounter[MAXCHANNELNO]; // 09.09.2015
unsigned short      Step_Manager_task::ChannelCCCV_LimitDlyCounter[MAXCHANNELNO];
bool                Step_Manager_task::bCCCV_Initiated[MAXCHANNELNO];
bool                Step_Manager_task::CCCVNoLimitCheckFlag[MAXCHANNELNO];
float               Step_Manager_task::CCCV_TargetVoltage[MAXCHANNELNO];
float               Step_Manager_task:: CCCV_TargetCurrent[MAXCHANNELNO];
unsigned long       Step_Manager_task::AutoRangeLastOutput[MAXCHANNELNO];
float				Step_Manager_task::AutoRangeLastVoltage[MAXCHANNELNO];
bool                Step_Manager_task:: bExtChargeFlag[MAXCHANNELNO];  //add by qjm 20150901 Extcharge 开始标志
bool                Step_Manager_task:: bEC_CalEnergyFlag[MAXCHANNELNO][4];
bool 				Step_Manager_task::AcimRequest_Send[MAXCHANNELNO];
ACIM_STATUS 		Step_Manager_task::ACIM_Status[MAXCHANNELNO];	//2016.05.10
unsigned short		Step_Manager_task::ACIM_CH_ON;
unsigned char       Step_Manager_task::bCV[MAXCHANNELNO];

//============= 09062017 TESLA SIMULATION I VOLTAGE CLAMP =========================
unsigned char        Step_Manager_task::bSimuOutOfDeadBand[MAXCHANNELNO]; //GL
unsigned char        Step_Manager_task::bAfterSimuLoaded[MAXCHANNELNO];                  //GL
unsigned char        Step_Manager_task::bTimePassed[MAXCHANNELNO];
float        Step_Manager_task::AccurateIR[MAXCHANNELNO][10];																			//GL 6ceshi
float        Step_Manager_task::InaccurateIR[MAXCHANNELNO][10];																		//GL
float				 Step_Manager_task::ACCURATE_IR[MAXCHANNELNO];																									//GL
float        Step_Manager_task::INACCURATE_IR[MAXCHANNELNO];																								//GL
int   			 Step_Manager_task::ACCURATE_IR_Size[MAXCHANNELNO];  		 //Accurate IR size
int          Step_Manager_task::INACCURATE_IR_Size[MAXCHANNELNO];
bool         Step_Manager_task::ACCURATE_IR_SizeFlag[MAXCHANNELNO];
bool         Step_Manager_task::INACCURATE_IR_SizeFlag[MAXCHANNELNO];
float 			 Step_Manager_task::LV_Voltage[MAXCHANNELNO];    //Last voltage value
float 			 Step_Manager_task::LV_Current[MAXCHANNELNO];		 //Last current value
float 				Step_Manager_task::ACCURATE_IR_Update[MAXCHANNELNO];     //Update Accurate IR value
float 				Step_Manager_task::INACCURATE_IR_Update[MAXCHANNELNO];   //
float 				Step_Manager_task::delta_voltage[MAXCHANNELNO];
float 				Step_Manager_task::delta_current[MAXCHANNELNO];
float         Step_Manager_task::ACCURATE_IR_Sum[MAXCHANNELNO];
float         Step_Manager_task::INACCURATE_IR_Sum[MAXCHANNELNO];
float					Step_Manager_task::SIMU_I_IR[MAXCHANNELNO];
float					Step_Manager_task::SIMU_P_IR[MAXCHANNELNO];    //10042017 add by zc
MsTime				Step_Manager_task::SIMU_I_TimeBuffer[MAXCHANNELNO];
unsigned char  Step_Manager_task::M4WaitACKOvertimeFlag[MAXCHANNELNO];
unsigned char  Step_Manager_task::uc_StartOrRestartFlag;
unsigned char  Step_Manager_task::M4ReceiveACKCnt;
unsigned char  Step_Manager_task::M4RestartSentCnt;
unsigned short Step_Manager_task::M4WaitTimeCnt[MAXCHANNELNO];  //定时20ms发送一次
unsigned short Step_Manager_task::M4WaitPacketCnt[MAXCHANNELNO];    //定时500ms 发送一次
unsigned char  Step_Manager_task::M4SendComCounter[MAXCHANNELNO];   //最多发送五次， 100ms 后不再发送。
unsigned char  Step_Manager_task::M4SendPacketCounter[MAXCHANNELNO]; //每个500ms 发送一次，最多发送四次， 两秒没有回复报警。
unsigned char  Step_Manager_task::M4SendStartStatus[MAXCHANNELNO];

bool           Step_Manager_task::b_CurCoarseTune[MAXCHANNELNO];  //add by qjm 20181224
bool           Step_Manager_task::b_CurCoarseTuneInit[MAXCHANNELNO];
bool           Step_Manager_task::b_CurCoarseTuneSampleFinish[MAXCHANNELNO];  //粗调采样点完成标志   add by qjm2019.12.28
MsTime 		Step_Manager_task::uc_CC_RampStampTime[MAXCHANNELNO];;
bool         Step_Manager_task::b_CC_RampStampFlag[MAXCHANNELNO];
unsigned char  Step_Manager_task::uc_CurCoarseTuneSampleCnt[MAXCHANNELNO];  //????????;
float          Step_Manager_task::f_CurCoarseTuneADValue[MAXCHANNELNO][SAMPLE_POINT];
unsigned char  Step_Manager_task::uc_CurCoarseTuneCnt[MAXCHANNELNO];
bool            Step_Manager_task::b_CurCoarseTuneFinished[MAXCHANNELNO];
bool            Step_Manager_task::b_FineTuneFinished[MAXCHANNELNO];
bool          Step_Manager_task::b_LEDFlag=false;
float mDebug;
St_AutoRangeSwitchCurPara 	Step_Manager_task::m_CurRangePara[MAXCHANNELNO];
MsTime 				Step_Manager_task::AutoRangeSwitchStartTime[MAXCHANNELNO];
unsigned char       Step_Manager_task::uc_AutoRangeStatus[MAXCHANNELNO];
bool        	Step_Manager_task::b_AutoRangeRequestFlag[MAXCHANNELNO];  //时序请求发送标志

MsTime Step_Manager_task::VoltOutputInterval[MAXCHANNELNO]; //20200706 zyx
MsTime        Step_Manager_task::CV_RampStageKeepTime[MAXCHANNELNO];

//===========================END======================================================
void Step_Manager_task::DoWork(void)
{
    //处理过程：分配时间片，提取Msg信息，运行step状态机
    unsigned char uc_channel;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_STEP] ++;
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_STEP);
#endif

    ChannelInfo_Manager::Set_TimeRecalculateSign();
    DacOutputCounter++;
    for(uc_channel = 0; uc_channel < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++) //通道循环处理Message
    {
        //        1）遍历信息队列，一次取出全部的Message信息；
        //        2）信息要有预处理过程，因为可能队列中会有几条信息；
        //        3）状态机是一直在运行的；无论有无信息，它不应被message左右；

        //信息预处理过程
        PreprocessMsg(uc_channel);
        //从上面的信息处理过程得到的操作信息存放于DoMessageBuf，执行状态机处理，如果没有信息也继续执行状态机
        ProcessState(uc_channel);
    }

    SL_StartPtr = SL_CurrentPtr;
    LL_StartPtr = LL_CurrentPtr;
    Check_Limit4_task::Reset();
    CheckRapidlyCommunicate();
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_STEP);
#endif
}

void Step_Manager_task::MyInit(void)
{
    memset(&Voltavg, 0, sizeof(avg)*MAXCHANNELNO);
    memset(&Curravg, 0, sizeof(avg)*MAXCHANNELNO);
    IsSetCCCV_StageTime = false;
    memset(&CC_Current, 0, sizeof(float)*MAXCHANNELNO);

    RapidlyCommunicateInit();
    unsigned char uc_Channel;
    unsigned char uc_i;
    for(uc_Channel = 0; uc_Channel < MAXCHANNELNO; uc_Channel++)
    {
        bCV[uc_Channel] = 0;
        bNewSimuLoaded[uc_Channel] = 0;
        EngineOverTimeTriger[uc_Channel] = 0;
        ChannelInfo_Manager::ChannelInfo[uc_Channel].DigitCtrl = 0;    //04.23.2013

        ChannelInfo_Manager::m_Pid_Coef[uc_Channel].m_PidTypeFlag = 0;    //04.23.2013
        ChannelInfo_Manager::ChannelInfo[uc_Channel].bNeedDacOutput = 1;  //Dac should output at first
        TimeRefreshFlag[uc_Channel] = 0; //时间刷新标识
        memset(&AuxLimitAckMgr[uc_Channel],      0,    sizeof(St_AuxLimitAckMgr));
        ChannelCVStatus[uc_Channel] = STEP_CV_Init;
        CCCVNoLimitCheckFlag[uc_Channel] = 0;
        ChannelCVCounter[uc_Channel] = 0;
        bExtChargeFlag[uc_Channel] = 0;
        bEC_CalEnergyFlag[uc_Channel][0] = 0; //ChargeEnergy
        bEC_CalEnergyFlag[uc_Channel][1] = 0; //Discharge Energy
        bEC_CalEnergyFlag[uc_Channel][2] = 0;  //ChargeCapacity
        bEC_CalEnergyFlag[uc_Channel][3] = 0;  //DisChargeCapacity
        memset(&MsgBufManage,      0,    sizeof(St_MsgBufManage)*MAXCHANNELNO); //181218 zyx
        for(uc_i = 0; uc_i < MAXMESSAGE; uc_i++)
        {
            memset(&StepMessageBuf[uc_i][uc_Channel], 0, sizeof(St_StepMessage)*MAXCHANNELNO);//181218 zyx
            MsgBufManage[uc_Channel].RecordMsgEmpty[uc_i] = uc_i; //{0,1,2,3,4,5,6,7}
        }
        MsgBufManage[uc_Channel].EmptyCount = 7;
        ClearStepMessage(&DoStepMessageBuf[uc_Channel]);
        ScanStepLimitNum[uc_Channel] = 0;

        ChannelInfo_Manager::ChannelInfo[uc_Channel].StepStatus = CCS_OFF;

        CCCV_TB_NeedExtLogFlg[uc_Channel] = false;

        ACIM_Status[uc_Channel] = ACIM_NONE;
        AcimRequest_Send[uc_Channel] = false;

        b_CurCoarseTune[uc_Channel] = false;
        b_CurCoarseTuneInit[uc_Channel]=false;
        b_CurCoarseTuneSampleFinish[uc_Channel]=false;
        uc_CurCoarseTuneSampleCnt[uc_Channel] =0;
        uc_CurCoarseTuneCnt[uc_Channel] =0;
        b_CC_RampStampFlag[uc_Channel]=true;
        b_AutoRangeRequestFlag[uc_Channel]=false;
        b_CurCoarseTuneFinished[uc_Channel]=false;
        b_FineTuneFinished[uc_Channel]=false;
    }

    DacOutputCounter = 0;
    memset(&SL_CurrentPtr,      0,    sizeof(St_LimitPointer));
    memset(&LL_CurrentPtr,      0,    sizeof(St_LimitPointer));
    Step_Manager_task::ACIM_CH_ON = 8;	// 没有通道（0-7）的ACIM回路接通
}

// void Step_Manager_task::EndTask(void)
// {

// }

int Step_Manager_task::PreprocessMsg(unsigned char uc_ChannelNo)
{
    unsigned char MsgIndex;

    if(MsgBufManage[uc_ChannelNo].FullCount == 0)
        return -1;        //无信息


    for(unsigned char uc_i = 0; uc_i < (MsgBufManage[uc_ChannelNo].FullCount); uc_i++)
    {
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[uc_i];
        if (StepMessageBuf[MsgIndex][uc_ChannelNo].MsgNumber == Step_Stop_Msg)
        {
            ClearStepMsg(uc_ChannelNo, MsgIndex);
            return 0;
        }
    }

    if(DoStepMessageBuf[uc_ChannelNo].Used == 1)          //没有Step_Stop_Msg那么要根据Message是否处理完来决定下一步操作
        return 1;            //Message没有处理完，不去取新信息
    else
    {   //Message处理完毕，要去取一条新的信息
        MsgIndex = MsgBufManage[uc_ChannelNo].RecordMsgFull[0];
        ClearStepMsg(uc_ChannelNo, MsgIndex);
        return 1;
    }
}

void Step_Manager_task::ProcessState(unsigned char uc_ChannelNo)
{
    //对于当前的各种状态进行处理，执行状态处理过程，响应命令，转换状态
    unsigned char uc_StepStatus;
    unsigned char MessageNumber;

    uc_StepStatus = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus;               //当前Step的状态
    MessageNumber = DoStepMessageBuf[uc_ChannelNo].StepMessage.MsgNumber;                          //取得当前响应信息编号

    switch(uc_StepStatus)
    {
    case STEPIDLE:
        switch(MessageNumber)
        {
        case Step_Run_Msg:
            StepRunMsgRoutine(uc_ChannelNo);
            break;
        case Step_JumpA_Msg:
            StepJumpMsgRoutine(uc_ChannelNo);
            break;
        default:
            bNeedCheck[uc_ChannelNo] = 0;  //20130922 add by hepeiqing,修复了手动停止后，LOG数据依然继续的BUG
            break;
        }

        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))
        {
            unsigned short uChanIdx, uLastChanIdx;

            uLastChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
            for (uChanIdx = uc_ChannelNo; uChanIdx <= uLastChanIdx; uChanIdx ++)  //dirui 20150629
            {
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_OFF;
            }
        }
        break;
    case STEPLOADSTEP:
        switch(MessageNumber)
        {
        case Step_Stop_Msg:
            StepStopMsgRoutine(uc_ChannelNo);
            break;
        case Step_JumpA_Msg:
            StepJumpMsgRoutine(uc_ChannelNo);
            break;
        default:
            Step_LoadRoutine(uc_ChannelNo);
            break;
        }
        break;
    case STEPTRANSITION:
        switch(MessageNumber)
        {
        case Step_Stop_Msg: //设定引擎任务为停止，并设定TRANSITION状态后的目的状态为IDLE，
            //重新进入TRANSITION状态，即重置Transition工作
            StepStopMsgRoutine(uc_ChannelNo);
            break;
        case Step_JumpA_Msg://如果TRANSITION后目标状态不为IDLE，
            //则保留此信息直到TRANSITION状态结束再响应，否则丢弃此信息。
            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus == STEPIDLE)
                break;
            else
                return;
        //break;

        default:
            Step_TransitionRoutine(uc_ChannelNo);
            break;
        }
        break;
    case STEPRUNNING:
        switch(MessageNumber)
        {
        case Step_Stop_Msg:
            if(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE\
                    && ((unsigned int)(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]+0.1f) == ACIM_PAUSE))
            {
                if(ACIM_Status[uc_ChannelNo] != ACIM_END && ACIM_Status[uc_ChannelNo] != ACIM_NONE)	//发送ACIM关通道MSG到上位机，ACIM通道在 RSTtoOFF时序中已关
                {
                    Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_NONE;
                    EthernetParse::m_ReportStatus.m_btCMD = PC_AcimRelayOff_Msg;
                    EthernetParse::MPReportData(uc_ChannelNo, SEND_EVENT_CONTENT);
                    AcimRequest_Send[uc_ChannelNo] = false;
                }
            }
            StepStopMsgRoutine(uc_ChannelNo);
            break;
        case Step_JumpA_Msg:
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;
            StepJumpMsgRoutine(uc_ChannelNo);
            break;

        default:
            if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() || ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo)
                    || ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
            {
                Step_RunningRoutine(uc_ChannelNo);
            }
            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)) //submaster
            {
                // only for single channel unit, as submster
                //							unsigned char Updata_Flag = 0;
                float fValue = 0;
                //							unsigned long Code;


                if (ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_bSubGroupCtrlUpdated == TRUE)
                {
                    fValue = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue / ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_ChannelNo].m_UnitGroupInfo.m_InUnitGroupSize;
                    IVSDL::SubMasterSdlLogTimesNormal2Frequently(uc_ChannelNo); //SubMaster改变设定值后进入快速汇报模式，加快采样的时效性
                    Step_OutputCurrDac_Submaster(uc_ChannelNo, fValue);

                    ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_bSubGroupCtrlUpdated = FALSE;
                    safety_check::CtrlValueCheck[uc_ChannelNo].fCtrlValue = fValue;
                }


            }
            break;
        }
        break;
    case STEPWAIT:
        switch(MessageNumber)
        {
        case Step_Stop_Msg:

            StepStopMsgRoutine(uc_ChannelNo);
            break;

        case Step_JumpA_Msg:
            StepJumpMsgRoutine(uc_ChannelNo);
            break;

        default:
            bNeedCheck[uc_ChannelNo] = 0; //20130922 add by hepeiqing
            Step_WaitRoutine(uc_ChannelNo);
            break;
        }
        break;
    default:
        bNeedCheck[uc_ChannelNo] = 0; //20130922 add by hepeiqing
        //break;
        return;
    }
    ClearStepMessage(&DoStepMessageBuf[uc_ChannelNo]);
}
/*****************************************************
// Function Name: ProcessEngineState
// Version:        V1.0
// Date:                2014/07/04
// Description:
//                         引擎状态处理
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::ProcessEngineState(unsigned char uc_ChannelNo)
{
    St_ScheduleMessage ScheduleMessage;
    St_ChannelInfo * p_ChnInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    switch(OS_LogicProcess::getHandleStatus(EngineHandle[uc_ChannelNo]))
    {
    case DISPOSABLE_ASSIGNED:                        //已经交付，待运行
        EngineOverTimeTriger[uc_ChannelNo] = 1;
        SysTimeEngineTask[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        break;
    case DISPOSABLE_RUNNING:          //运行中
    {
        unsigned long long TimeTicks;
        MsTime MyMsTime;
        MyMsTime = SysTimeEngineTask[uc_ChannelNo];
        TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
        if(EngineOverTimeTriger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)       //超时？
        {
            EngineOverTimeTriger[uc_ChannelNo] = 0;
            p_ChnInfo->EngineStatus = 0;
            //引擎调用失败处理
        }
        break;
    }
    case DISPOSABLE_FINISHED:           //结束
    {
        if(ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo))
        {
            CommonFunc::GetTimeTicksUpdate(&Parallel_WaitSlaveChnDisposableFinish_Time[uc_ChannelNo]);
        }

        if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitSlaveChannel(uc_ChannelNo))  //only be used with Intra-Slave channel . dirui 2015/12/30
            safety_check::Process_SM_Ptr[uc_ChannelNo] = SAFE;
        Schedule_Manager_task::LockModeSwitch[uc_ChannelNo] = 0;
        ChannelInfo_Manager::m_ParaFeedbackCtrl[uc_ChannelNo].m_bStartCtrl = false;
        ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[uc_ChannelNo] = 0.0f;

//#if SUPPORT_AUX==1
        if(CommonFunc::uc_Support_AUX)
        {
            if(!AuxLimitAckMgr[uc_ChannelNo].bWattingAckOver)   //aux ack over(ok/error)
                break;
        }
//#endif

        EngineOverTimeTriger[uc_ChannelNo] = 0;
        p_ChnInfo->EngineStatus = 0;
//         Pid_Ctrl::m_Counter[uc_ChannelNo]=0;
//        Pid_Ctrl::m_LastVoltage[uc_ChannelNo]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
//        Pid_Ctrl::LastCal[uc_ChannelNo] =CommonFunc::GetSystemTime();
        p_ChnInfo->StepStatus = p_ChnInfo->StepTransationToStatus; //转入目标状态
        if(p_ChnInfo->StepStatus == STEPRUNNING)
        {
            ScheduleMessage.MsgNumber = Sch_Started_Msg;
            ScheduleMessage.StepID = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
            ScheduleMessage.CycleID = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;

            if(!ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo])
                Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, ScheduleMessage);        //发送信息
            //Step开始的时候要注意写入 ChannelStep的StartTime
            //Schedule开始的时候要注意写入TestTime,StartTime   注意RESUME条件
            if(Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeStatus == 0) //这个标志可以判断是否需要第二次清零
            {
                Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeTestTime = 0;
                Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeStepTime = 0;
            }
            if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime)
            {
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_TestTime, p_ChnInfo->m_fResumeTestTime);
                ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 0;
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_TC_Timer1, p_ChnInfo->m_fResumeTC_Timer1);
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_TC_Timer2, p_ChnInfo->m_fResumeTC_Timer2);
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_TC_Timer3, p_ChnInfo->m_fResumeTC_Timer3);
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_TC_Timer4, p_ChnInfo->m_fResumeTC_Timer4);
                p_ChnInfo->m_fResumeTC_Timer1 = p_ChnInfo->m_fResumeTC_Timer2 = p_ChnInfo->m_fResumeTC_Timer3 = p_ChnInfo->m_fResumeTC_Timer4 = 0;
                IVSDL::ScheduleStartedRoutine(uc_ChannelNo);
            }

            if(!ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo])
            {
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime, p_ChnInfo->m_fResumeStepTime);
                BurstModeSampling::b_StepTimeReset[uc_ChannelNo] = true;//1805281 zyx
                if(!ChannelInfo_Manager::IsSpecialPoint(uc_ChannelNo, RESUME_POINT))  //mits8
                {
                    ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, FIRST_POINT);
                }
            }
            p_ChnInfo->m_fResumeStepTime = 0;   //这是个一次性变量，用完即清0；
            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);

            //以下为MV服务
            CommonFunc::GetTimeTicksUpdate(&MetaVariable_Task::m_capacity_Time[uc_ChannelNo]);
            MetaVariable_Task::capacityStatus[uc_ChannelNo] = 1;
            unsigned char InActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;  //查找上次控制类型定义值 dirui 2014/09/22
            if(ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo])
                InActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
            if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CPULSE)
            {
                if(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[InActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CPULSE)
                    PULSE_safety_check::Init();//初始化安全检查
                else
                    Setup_PulseTask();  //切换脉冲任务
            }
            else
            {
                if(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[InActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CPULSE)
                {
                    //这里是最后一次把关，停止脉冲
                    Pulse_End(uc_ChannelNo); //如果有脉冲输出的话，尽快停止输出脉冲。
                    Remove_PulseTask();    //恢复普通任务
                }
            }
        }
        else if(p_ChnInfo->StepStatus == STEPIDLE)
        {
            ScheduleMessage.MsgNumber = Sch_Stopped_Msg;
            ScheduleMessage.StepID = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
            ScheduleMessage.CycleID = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;

            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, ScheduleMessage);

            if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CPULSE)
            {
                Pulse_End(uc_ChannelNo);  //如果有脉冲输出的话，尽快停止输出脉冲。
                Remove_PulseTask();    //恢复普通任务
            }
        }

        LimitTimeDelayTirgger[uc_ChannelNo] = 0;
        StepLimitReach[uc_ChannelNo] = 0; //清Limit标志
        p_ChnInfo->Voltage_Sampleflg = 0; //清采样刷新标记 0917 dirui
        p_ChnInfo->Current_Sampleflg = 0; //清采样刷新标记 0917 dirui
        p_ChnInfo->StepLimitProcess = STEPLIMITCHECK; //Limit开始检查 不做延时
        StepRunningStatus[uc_ChannelNo] = STEPRUNNING_READY; //准备运行

        UWord16 uw16_Type = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType;
        switch(uw16_Type)
        {
        case  CT_IVCH_IR:
        case  CT_IVCH_ACIR:
        case  CT_IVCH_PAUSE:    //Add by DKQ 03.14.2016
        case  CT_IVCH_SETVALUE: //171109 zyx
            bNeedCheck[uc_ChannelNo] = 0;   // 特殊类型不检查任何Limit.  150901 yy
            break;
        default:
            if(p_ChnInfo->StepStatus != STEPIDLE) //zyx 171010
                bNeedCheck[uc_ChannelNo] = 1;
            break;
        }

        if(ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo])
        {
            ChannelInfo_Manager::OnlineUpdata_Clear(uc_ChannelNo);
        }
        break;
    }
    default:
        break;
    }
}

/*****************************************************
// Function Name: Setup_PulseTask
// Version:        V1.0
// Date:                2014/09/23
// Description:
//                装载脉冲任务处理
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::Setup_PulseTask(void)
{
    Pulse::buf_info[0].m_bDataLogged = 1;
    Pulse::buf_info[0].m_bFull = 0;
    Pulse::buf_info[1].m_bDataLogged = 1;
    Pulse::buf_info[1].m_bFull = 0;

    OS_ServeProcess::SuspendTask(OS_ServeProcess::OTID_4_Reg_SamplePro);
    OS_ServeProcess::SuspendTask(OS_ServeProcess::OTID_4_Reg_CheckLimit4);
    OS_ServeProcess::SuspendTask(OS_ServeProcess::OTID_4_Reg_CheckLimit8);
    OS_ServeProcess::SuspendTask(OS_ServeProcess::OTID_4_Reg_SafetyCheck);
    OS_ServeProcess::SuspendTask(OS_ServeProcess::OTID_4_Reg_MetaVariable);
    //在这里安装脉冲相关的任务
    //                         任务ID                             	指定任务优先级                      是否初始化
    if(OS_ServeProcess::OTID_4_Pulse_SamplePro < 0 )
        OS_ServeProcess::OTID_4_Pulse_SamplePro = \
                OS_ServeProcess::MountTask(SampleProcess_Pulse::TaskID,    	SampleProcess_Pulse::Priority,      1);

    if(OS_ServeProcess::OTID_4_Pulse_Check_V_Limit < 0 )
        OS_ServeProcess::OTID_4_Pulse_Check_V_Limit = \
                OS_ServeProcess::MountTask(Check_V_Limit_task::TaskID,      Check_V_Limit_task::Priority,       1);

    if(OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit < 0 )
        OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit = \
                OS_ServeProcess::MountTask(CheckLimit_Non_V_task::TaskID,   CheckLimit_Non_V_task::Priority,    1);

    if(OS_ServeProcess::OTID_4_Pulse_SafetyCheck < 0 )
        OS_ServeProcess::OTID_4_Pulse_SafetyCheck = \
                OS_ServeProcess::MountTask(PULSE_safety_check::TaskID,      PULSE_safety_check::Priority,       1);

    if(OS_ServeProcess::OTID_4_Pulse_MetaVariable < 0 )
        OS_ServeProcess::OTID_4_Pulse_MetaVariable = \
                OS_ServeProcess::MountTask(PULSE_MV_Task::TaskID,           PULSE_MV_Task::Priority,            1);

    if(OS_ServeProcess::OTID_4_Pulse_LogLimit < 0 )
        OS_ServeProcess::OTID_4_Pulse_LogLimit = \
                OS_ServeProcess::MountTask(Pulse_LogLimit_task::TaskID,     Pulse_LogLimit_task::Priority,      1);


}
/*****************************************************
// Function Name: Remove_PulseTask
// Version:        V1.0
// Date:                2014/09/23
// Description:
//                清除脉冲任务
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::Remove_PulseTask(void)
{
    //恢复普通采样等任务
    OS_ServeProcess::ResumeTask(OS_ServeProcess::OTID_4_Reg_SamplePro, true);
    OS_ServeProcess::ResumeTask(OS_ServeProcess::OTID_4_Reg_CheckLimit4, true);
    OS_ServeProcess::ResumeTask(OS_ServeProcess::OTID_4_Reg_CheckLimit8, true);
    OS_ServeProcess::ResumeTask(OS_ServeProcess::OTID_4_Reg_SafetyCheck, true);
    OS_ServeProcess::ResumeTask(OS_ServeProcess::OTID_4_Reg_MetaVariable, true);

    //在这里去掉脉冲相关的任务
    if(OS_ServeProcess::OTID_4_Pulse_SamplePro >= 0 )
        OS_ServeProcess::DisMountTask( OS_ServeProcess::OTID_4_Pulse_SamplePro);
    if(OS_ServeProcess::OTID_4_Pulse_Check_V_Limit >= 0 )
        OS_ServeProcess::DisMountTask(OS_ServeProcess::OTID_4_Pulse_Check_V_Limit);
    if(OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit >= 0 )
        OS_ServeProcess::DisMountTask(OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit);
    if(OS_ServeProcess::OTID_4_Pulse_SafetyCheck >= 0 )
        OS_ServeProcess::DisMountTask(OS_ServeProcess::OTID_4_Pulse_SafetyCheck);
    if(OS_ServeProcess::OTID_4_Pulse_MetaVariable >= 0 )
        OS_ServeProcess::DisMountTask(OS_ServeProcess::OTID_4_Pulse_MetaVariable);
    if(OS_ServeProcess::OTID_4_Pulse_LogLimit >= 0 )
        OS_ServeProcess::DisMountTask(OS_ServeProcess::OTID_4_Pulse_LogLimit);

    OS_ServeProcess::OTID_4_Pulse_SamplePro = -1;
    OS_ServeProcess::OTID_4_Pulse_Check_V_Limit = -1;
    OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit = -1;
    OS_ServeProcess::OTID_4_Pulse_SafetyCheck = -1;
    OS_ServeProcess::OTID_4_Pulse_MetaVariable = -1;
    OS_ServeProcess::OTID_4_Pulse_LogLimit = -1;
}
/*****************************************************
// Function Name: ProcessLimitAckState
// Version:        V1.0
// Date:                2014/07/05
// Description:
//                CLear和Download Limit应答处理
// Others:
//
// History：
//
// ******************************************************/
//#if SUPPORT_AUX==1
void Step_Manager_task::ProcessLimitAckState(unsigned char uc_ChannelNo)
{
    St_AuxLimitAckMgr * p_AuxLmtAck = &AuxLimitAckMgr[uc_ChannelNo];
    switch(p_AuxLmtAck->AckStatus)
    {
    case AUXACK_IDLE:
        p_AuxLmtAck->WaitAckCnt = 0;
        if(p_AuxLmtAck->bNeedWattingAck)         //启动AUX ACK应答等待
        {
            p_AuxLmtAck->AckStatus = AUXACK_WAITCLEARLIMITACK;
            p_AuxLmtAck->bNeedWattingAck = 0;      //
            p_AuxLmtAck->bWattingAckOver = 0;
            p_AuxLmtAck->WaitAckCnt = 0;
        }
        else
        {
            p_AuxLmtAck->bWattingAckOver = 1;
            break;
        }

    case AUXACK_WAITCLEARLIMITACK:
        //check ack code
        p_AuxLmtAck->ReDwonLimitCnt = 0;
        p_AuxLmtAck->ReClrLimitCnt = 0;
        p_AuxLmtAck->WaitAckCnt = 0;
        if(p_AuxLmtAck->AuxLimitCnt == 0)   //无aux limit 结束
        {
            p_AuxLmtAck->AckStatus = AUXACK_IDLE;
            p_AuxLmtAck->bNeedWattingAck = 0;
            p_AuxLmtAck->bWattingAckOver = 1;
            for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
                CANBusParse::st_ClrLimitAckMgr[uc_ChannelNo].m_AuxInfo[AuxIdx].m_bValid = 0;
        }
        else   //在这发送limit
        {
            for(unsigned char uc_i = 0; uc_i < p_AuxLmtAck->AuxLimitCnt; uc_i++)
                CANBusParse::IV_DownloadLimitToAUX(uc_ChannelNo, AuxLimitBuf[uc_ChannelNo][uc_i].MyAuxStepID, AuxLimitBuf[uc_ChannelNo][uc_i].MyAuxLimitIdx);
            p_AuxLmtAck->AckStatus = AUXACK_WAITDOWNLOADLIMITACK;
            ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;
            for(unsigned char uc_AuxIdx = 0; uc_AuxIdx < MAXAUXUNIT; uc_AuxIdx++)
            {
                if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][uc_AuxIdx].m_bExist != 1)
                    continue;
                CommonFunc::GetTimeTicksUpdate(&CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][uc_AuxIdx].m_checkAuxInfoTick);	 // reset counter
            }
        }
        break;
    case AUXACK_WAITDOWNLOADLIMITACK:
        if(CANBusParse::IV_CheckAuxSNRes(uc_ChannelNo, LIMITSN))	//  aux limit check
        {
            if(CANBusParse::IV_ClrAuxLimitRes(uc_ChannelNo))	//  此处加入Check 函数
            {
                //clear limit Reset
                for(unsigned char AuxIdx = 0; AuxIdx < MAXAUXUNIT; AuxIdx++)
                    CANBusParse::st_ClrLimitAckMgr[uc_ChannelNo].m_AuxInfo[AuxIdx].m_bValid = 0;

                p_AuxLmtAck->AckStatus = AUXACK_IDLE; //发送成功
                p_AuxLmtAck->bNeedWattingAck = 0;
                p_AuxLmtAck->bWattingAckOver = 1;
                p_AuxLmtAck->ReDwonLimitCnt = 0;               //清重发标记
                p_AuxLmtAck->AuxLimitCnt = 0;      //clear AuxLimit Count
                p_AuxLmtAck->WaitAckCnt = 0;
                CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, LIMITSN);
                break;
            }
        }
        p_AuxLmtAck->WaitAckCnt++;
        if(p_AuxLmtAck->WaitAckCnt > 2000) // 2s
        {

            if(p_AuxLmtAck->ReDwonLimitCnt <= 3)
            {
                p_AuxLmtAck->AckStatus = AUXACK_RETRANCLEARLIMIT; // 重发 需要重发clearlimit
                p_AuxLmtAck->ReDwonLimitCnt++;
                p_AuxLmtAck->WaitAckCnt = 0;

            }
            else
            {
                p_AuxLmtAck->ReDwonLimitCnt = 0;
                p_AuxLmtAck->AckStatus = AUXACK_AUXACKFAIL; //
                p_AuxLmtAck->WaitAckCnt = 0;
                p_AuxLmtAck->bAuxAckFailed = 0; //reset  temp
            }
        }
        break;
    case AUXACK_RETRANCLEARLIMIT:         //clear limit & download limit failed
        for(unsigned char uc_i = 0; uc_i < p_AuxLmtAck->AuxLimitCnt; uc_i++)
            CANBusParse::IV_DownloadLimitToAUX_Retransmission(uc_ChannelNo, AuxLimitBuf[uc_ChannelNo][uc_i].MyAuxStepID, AuxLimitBuf[uc_ChannelNo][uc_i].MyAuxLimitIdx);
        p_AuxLmtAck->WaitAckCnt = 0;
        p_AuxLmtAck->AckStatus = AUXACK_WAITDOWNLOADLIMITACK;
        ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();//OS_Time;
        for(unsigned char uc_AuxIdx = 0; uc_AuxIdx < MAXAUXUNIT; uc_AuxIdx++)
        {
            if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][uc_AuxIdx].m_bExist != 1)
                continue;
            CommonFunc::GetTimeTicksUpdate(&CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][uc_AuxIdx].m_checkAuxInfoTick);	 // reset counter
        }
        break;

    case AUXACK_AUXACKFAIL:
        AUXAckFailRoutine(uc_ChannelNo);
        memset((void*)&CANBusParse::st_ClrLimitAckMgr[uc_ChannelNo].m_AuxInfo[0], 0, sizeof(St_ClrLimitInfo)*MAXAUXUNIT); //CLRLIMIT_NONE
        p_AuxLmtAck->AckStatus = AUXACK_IDLE;
        p_AuxLmtAck->bNeedWattingAck = 0;
        p_AuxLmtAck->bWattingAckOver = 0;     // 1   // 20140923 ZJ
        break;
    default:
        break;

    }
}
//#endif
/*****************************************************
// Function Name: StepRunMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Step_Run_Msg信息响应过程处理
// Others:
//                      提取出运行信息，并转入LOADSTEP状态
// History：
//
// ******************************************************/
void Step_Manager_task::StepRunMsgRoutine(unsigned char uc_ChannelNo)
{
    StepTimeTrigger[uc_ChannelNo] = 0;
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Get_StepRunInfo(uc_ChannelNo, ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ActiveIdx].m_StepInfo);
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPLOADSTEP;
}

/*****************************************************
// Function Name: StepStopMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         Step_Stop_Msg信息响应过程处理
// Others:
//                    设定引擎任务为停止，并设定TRANSITION状态后的目的状态为IDLE，
//                    转入TRANSITION状态
// History：
//
// ******************************************************/
void Step_Manager_task::StepStopMsgRoutine(unsigned char uc_ChannelNo)
{
    Pulse_End(uc_ChannelNo);
    StepTimeTrigger[uc_ChannelNo] = 0;
    IR::IRChannelFlg &= ~(1 << uc_ChannelNo); //去除标记位  若此时在运行IR的话要清除标记,避免重复查询
    PrepareForStop(uc_ChannelNo);
    //161019 zyx
    IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_DISPLAY, LOG_NORMAL_DATA_POINT);
    ChannelInfo_Manager::ClearPV_CV_StageTime(uc_ChannelNo);
    ChannelInfo_Manager::ClearPV_CV_StageCurrent(uc_ChannelNo);
    //161019 zyx
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 0;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPIDLE;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
    if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_WORD
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_STRING
            ||GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType ==CT_IVCH_WRITE_SMB_BLOCK
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_OPTWORDADDRESS)
        GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType = CT_IVCH_REST;
    else
    {
        if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_STOP)
            EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_SDL_CYCLE); // 20170919 yy
        Schedule_Manager_task::SendMetaSync(uc_ChannelNo);
    }
}

/*****************************************************
// Function Name: StepJumpMsgRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                     Step_Jump_Msg信息响应过程处理
// Others:
//                    提取出运行信息，并转入LOADSTEP状态
// History：
//
// ******************************************************/
void Step_Manager_task::StepJumpMsgRoutine(unsigned char uc_ChannelNo)
{
    if ( Pulse::Pulse_is_Running)  //如果有脉冲输出的话，尽快停止输出脉冲。
    {
        Pulse::End(uc_ChannelNo);
        Pulse::Pulse_is_Running = false ;
    }
    StepTimeTrigger[uc_ChannelNo] = 0;
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Get_StepRunInfo(uc_ChannelNo, ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[!ActiveIdx].m_StepInfo);
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPLOADSTEP;
    if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
    {
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType = CAN_CMD_PARA_MASTER_STEPCHANGE;
    }
}


/*****************************************************
// Function Name: Step_LoadRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                     LOADSTEP状态的瞬态响应
// Others:
//                    根据（提取的）运行信息分析引擎任务，设定引擎任务，
//                    并设定TRANSITION状态后的目的状态为RUNNING，转入TRANSITION状态
// History：
//
// ******************************************************/
void Step_Manager_task::Step_LoadRoutine(unsigned char uc_ChannelNo)
{
    if(!Smart_Battery::BeSupportSmbus)
    {
        float voltage = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);	// Get Voltage
        unsigned char uc_CheckVClamp = safety_check::VClampRangeCheck(uc_ChannelNo, voltage);
        if(!uc_CheckVClamp)   // 增加提前检查所设置的钳压是否正常 yy  20160216
            EthernetParse::SendTasksMsgProcess(uc_ChannelNo, ST_STEPMANAGE, VClampUnsafe, voltage);
    }

    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx; //反转副本标识
    St_StepCtrl Ctrl = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = Ctrl.m_u16StepID;
    if ((Ctrl.m_u16ControlType == CT_IVCH_CCCV) ||
            (Ctrl.m_u16ControlType == CT_IVCH_VOLTAGE) ||
            (Ctrl.m_u16ControlType == CT_IVCH_DIGIV))
    {
        if( Ctrl.m_CtrlValues.m_fControlValue[2] > 0)
        {
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule = true;
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Schedule = Ctrl.m_CtrlValues.m_fControlValue[2];
        }
        else
        {
            if(!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule )   //if already get IR from other step's, then continue use it
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule = false;

        }

    }
    else if(Ctrl.m_u16ControlType != CT_IVCH_VOLTAGE && Ctrl.m_u16ControlType != CT_IVCH_DIGIV )  //161019 zyx
        ChannelInfo_Manager::ClearPV_CV_StageTime(uc_ChannelNo);   //161019 zyx

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false; //1711061 zyx
    if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVARIABLE && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVALUE
            && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_TYPE_SMB_WRITEREG_WORD
            && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType !=CT_IVCH_WRITE_SMB_BLOCK
            && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_TYPE_SMB_WRITEREG_STRING
            && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_TYPE_SMB_OPTWORDADDRESS) //  20130927 何
        PrepareForRun(uc_ChannelNo);   //分析提取信息；
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPRUNNING;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
}

/*****************************************************
// Function Name: Step_TransitionRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                     TRANSITION状态的瞬态响应
// Others:
//                调用一次性引擎执行预定任务，执行完毕后根据TRANSITION后目标状态转入相应状态，
//                并发回相应信息（Sch_Started / Sch_Stopped）
// History：
//
// ******************************************************/
void Step_Manager_task::Step_TransitionRoutine(unsigned char uc_ChannelNo)
{
    unsigned short uChanIdx, uInterLastChanIdx, uIntraLatstChanIdx;
    St_ChannelInfo * p_Info = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];

    St_ScheduleMessage ScheduleMessage;
    bStepLimitChecked[uc_ChannelNo] = 0;       //转换期间检查limit无效，dirui 2014/10/28
    PULSE_bStepLimitChecked[uc_ChannelNo] = 0;

    if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
    {
        uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
        ChannelInfo_Manager::CopyMasterSpecifiedMetavariable(uc_ChannelNo, uc_ChannelNo + 1, uIntraLatstChanIdx, MetaCode_LC_StepTime);
    }

    if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterSubmasterChannel(uc_ChannelNo))
    {
        uInterLastChanIdx = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_InUnitGroupSize - 1;
        ChannelInfo_Manager::CopyMasterSpecifiedMetavariable(uc_ChannelNo, uc_ChannelNo + 1, uInterLastChanIdx, MetaCode_LC_StepTime);
    }

    if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_WORD
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_STRING
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_WRITE_SMB_BLOCK
            || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_OPTWORDADDRESS)
    {
        EngineOverTimeTriger[uc_ChannelNo] = 0;
        p_Info->EngineStatus = 0;

        p_Info->StepStatus = p_Info->StepTransationToStatus; //转入目标状态

        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        {
            ChannelInfo_Manager::CopyMasterSpecifiedMetavariable(uc_ChannelNo, uc_ChannelNo + 1, uIntraLatstChanIdx, MetaCode_LC_StepTime);

            for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uIntraLatstChanIdx; uChanIdx++)
            {
                EngineOverTimeTriger[uChanIdx] = EngineOverTimeTriger[uc_ChannelNo];
                ChannelInfo_Manager::ChannelInfo[uChanIdx].EngineStatus = p_Info->EngineStatus;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].StepStatus = p_Info->StepStatus;
            }
        }

        ////////////////////////// set up IV SDL parameters,  master from sdu, submaster is fixed using resolution  //////////////////////////
        if ((ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()) || (ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo)) )  //single mode, or intra/inter unit master:
        {
            //TODO
            ScheduleMessage.MsgNumber = Sch_Started_Msg;
            ScheduleMessage.StepID = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;
            ScheduleMessage.CycleID = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CycleIndex].m_fValue;
            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, ScheduleMessage);        //发送信息

            if(Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeStatus == 0) //这个标志可以判断是否需要第二次清零
            {
                Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeTestTime = 0;
                Schedule_Manager_task::ResumeManage[uc_ChannelNo].m_ResumeStepTime = 0;
            }
            if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime)
            {
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_TestTime, p_Info->m_fResumeTestTime);
                ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_bResetTestTime = 0;

                if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()) //single mode
                {
                    IVSDL::ScheduleStartedRoutine(uc_ChannelNo);
                }
                else if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
                {
                    //does not matter much just do the copy to slaves, probably never use:
                    for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uIntraLatstChanIdx; uChanIdx++)
                    {
                        ChannelInfo_Manager::Write_SpecifiedMetavariable2(uChanIdx, MetaCode_PV_TestTime, p_Info->m_fResumeTestTime);
                        ChannelInfo_Manager::ChannelSchedule[uChanIdx].m_bResetTestTime = 0;
                    }

                    IVSDL::ScheduleStartedRoutine(uc_ChannelNo); //SUPPORT_PARALLEL TODO! need to set all slaves SDL too.
                }
                else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
                {
                    IVSDL::ScheduleStartedRoutine(uc_ChannelNo);  //define inter unit parallel only available to single chan/unit system
                }
            }
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))// status error this if is unreachable
        {
            //no sdu for submaster, wait for CAN_CMA_PARA_START/STOP/MASTER_STEPCHANGE to get here.
            IVSDL::SubmasterStartRoutine(uc_ChannelNo);
        }

        //////////////////////////// calculate IVSDL's checking threshold, save as DeltaValue  /////////////////////////////////////////////////////////

        if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() || ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[p_Info->m_GroupID].m_UnitGroupInfo.m_WholeGroupSize == 1)
        {
            ChannelInfo_Manager::Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime, p_Info->m_fResumeStepTime);
            BurstModeSampling::b_StepTimeReset[uc_ChannelNo] = true;//1805281 zyx

            if(!ChannelInfo_Manager::IsSpecialPoint(uc_ChannelNo, RESUME_POINT))  //mits8
            {
                ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, FIRST_POINT);
            }
            p_Info->m_fResumeStepTime = 0;   //这是个一次性变量，用完即清0；
            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);

            bool IsNeedHistoryData = (GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE \
                                      || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE);

            IVSDL::StepStartedRoutine(uc_ChannelNo, !IsNeedHistoryData);

            //以下为MV服务
            CommonFunc::GetTimeTicksUpdate(&MetaVariable_Task::m_capacity_Time[uc_ChannelNo]);
            MetaVariable_Task::capacityStatus[uc_ChannelNo] = 1;

            LimitTimeDelayTirgger[uc_ChannelNo] = 0;
            StepLimitReach[uc_ChannelNo] = 0; //清Limit标志
            p_Info->Voltage_Sampleflg = 0; //清采样刷新标记 0917 dirui
            p_Info->Current_Sampleflg = 0; //清采样刷新标记 0917 dirui
            p_Info->StepLimitProcess = STEPLIMITCHECK; //Limit开始检查 不做延时
            StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
            bNeedCheck[uc_ChannelNo] = 1;
            if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE)
            {
                Schedule_Manager_task::SetSetVariable(uc_ChannelNo);
                ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, (LAST_POINT | SET_VALUABLE_POINT)); //180309 zyx /FIRST_POINT |  //mits8
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE)
            {
                Schedule_Manager_task::SetSetValue(uc_ChannelNo); //171109 zyx
                ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, (LAST_POINT | SET_VALUE_POINT)); //180309 zyx FIRST_POINT |   //mits8
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_WORD)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit

                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber = SMB_Write_Msg;
                memcpy(&SMBMessage.Send_buf[1], &GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Data,  \
                       2);
                SMBMessage.m_SlaveAddr = Smart_Battery::m_OptWordAddress;
                SMBMessage.m_bufLen = 3;       //字节数
                SMBMessage.Send_buf[0] = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Register; //发送字节
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_WRITE_SMB_BLOCK)
            {
                p_Info->StepLimitProcess=STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo]=STEPRUNNING_DOSPECIALCASE;  //准备运行
                bNeedCheck[uc_ChannelNo] = 1;
                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber=SMB_Write_Msg;
                unsigned char uc_Length = GetStepRunInfo[uc_ChannelNo].GetSMBBlockInfo.m_SMBBlockData.m_nElementNum;
                memcpy(&SMBMessage.Send_buf[1],&GetStepRunInfo[uc_ChannelNo].GetSMBBlockInfo.m_SMBBlockData.m_arrElement[0],uc_Length);
                SMBMessage.m_SlaveAddr=Smart_Battery::m_OptWordAddress;
                SMBMessage.m_bufLen = (uc_Length+1);         //字节数
                SMBMessage.Send_buf[0]= GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019

            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_STRING)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit

                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber = SMB_Write_Msg;
                memcpy(&SMBMessage.Send_buf[0], &GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsString.m_Data[0],  \
                       GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsString.m_Len);
                SMBMessage.m_SlaveAddr = Smart_Battery::m_OptWordAddress;
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_OPTWORDADDRESS)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit
                Smart_Battery::m_OptWordAddress = (unsigned char)GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Data;
            }
        }

        if ( (ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo)) && \
                (ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[p_Info->m_GroupID].m_UnitGroupInfo.m_WholeGroupSize > 1))
        {
            for (uChanIdx = uc_ChannelNo; uChanIdx <= uIntraLatstChanIdx; uChanIdx ++)
            {
                ChannelInfo_Manager::Write_SpecifiedMetavariable2(uChanIdx, MetaCode_PV_Steptime, ChannelInfo_Manager::ChannelInfo[uChanIdx].m_fResumeStepTime);
                BurstModeSampling::b_StepTimeReset[uc_ChannelNo] = true;//1805281 zyx
                if(!ChannelInfo_Manager::IsSpecialPoint(uc_ChannelNo, RESUME_POINT))  //mits8
                {
                    ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, FIRST_POINT);
                }
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_fResumeStepTime = 0;   //这是个一次性变量，用完即清0；
                ChannelInfo_Manager::DVmetaClear(uChanIdx);
            }

            bool IsNeedHistoryData = (GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE \
                                      || GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE);

            IVSDL::StepStartedRoutine(uc_ChannelNo, !IsNeedHistoryData);  //以master通道为头，开始所有通道运行

            //以下为MV服务
            for (uChanIdx = uc_ChannelNo; uChanIdx <= uIntraLatstChanIdx; uChanIdx ++)
            {
                CommonFunc::GetTimeTicksUpdate(&MetaVariable_Task::m_capacity_Time[uChanIdx]);  //calculate_capacity使用。capacity都是通道自己计算，所以slave的也要计时
                MetaVariable_Task::capacityStatus[uChanIdx] = 1;

                LimitTimeDelayTirgger[uChanIdx] = 0; //never used???
            }

            StepLimitReach[uc_ChannelNo] = 0; //清Limit标志 //其实只要master的清掉即可。

            p_Info->StepLimitProcess = STEPLIMITCHECK; //Limit开始检查 不做延时
            StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
            bNeedCheck[uc_ChannelNo] = 1;

            if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE)
                Schedule_Manager_task::SetSetVariable(uc_ChannelNo);  //capacity, energy, Cycle ets.
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE)
                Schedule_Manager_task::SetSetValue(uc_ChannelNo); //171017 zyx
            //只有主通道负责维护,电流电量也是主通道根据groupcurrent来计算的。
            //函数需要调用active step idx,也只有主通道有。所以这里只对主通道的相应metavar set
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_WORD)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit

                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber = SMB_Write_Msg;
                memcpy(&SMBMessage.Send_buf[1], &GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Data,  \
                       2);
                SMBMessage.m_SlaveAddr = Smart_Battery::m_OptWordAddress;
                SMBMessage.m_bufLen = 3;       //字节数
                SMBMessage.Send_buf[0] = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Register; //发送字节
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_WRITEREG_STRING)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit

                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber = SMB_Write_Msg;
                memcpy(&SMBMessage.Send_buf[0], &GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsString.m_Data[0],  \
                       GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsString.m_Len);
                SMBMessage.m_SlaveAddr = Smart_Battery::m_OptWordAddress; //GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SlaveAddr;
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019
            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_TYPE_SMB_OPTWORDADDRESS)
            {
                p_Info->StepLimitProcess = STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE; //准备运行
                bNeedCheck[uc_ChannelNo] = 1;                     //不检查任何Limit
                Smart_Battery::m_OptWordAddress = (unsigned char)GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SMB_SendMsShort.m_Data;

            }
            else if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_WRITE_SMB_BLOCK)
            {
                p_Info->StepLimitProcess=STEPLIMITCHECK;
                StepRunningStatus[uc_ChannelNo]=STEPRUNNING_DOSPECIALCASE;  //准备运行
                bNeedCheck[uc_ChannelNo] = 1;
                St_SMBMessage SMBMessage;
                SMBMessage.MsgNumber=SMB_Write_Msg;
                unsigned char uc_Length = GetStepRunInfo[uc_ChannelNo].GetSMBBlockInfo.m_SMBBlockData.m_nElementNum;
                memcpy(&SMBMessage.Send_buf[1],&GetStepRunInfo[uc_ChannelNo].GetSMBBlockInfo.m_SMBBlockData.m_arrElement[0],uc_Length);
                SMBMessage.m_SlaveAddr=Smart_Battery::m_OptWordAddress;//GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_SMBValue.m_SlaveAddr;
                SMBMessage.m_bufLen = (uc_Length+1);         //字节数
                SMBMessage.Send_buf[0]= GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];  //发送字节
                Smart_Battery::SendMsgToSMB(SMBMessage);
                Smart_Battery::ResetSMBusFirstLog(SMBMessage.Send_buf[0]); //add by zc 04/01/2019
            }

        }

        if(CommonFunc::uc_Support_AUX)
            AuxLimitAckMgr[uc_ChannelNo].bNeedWattingAck = 0;

        //check if the last control type is pulse type. dirui 2014/0928
        unsigned char InActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        if(ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo])
            InActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

        switch(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType)
        {
        case CT_IVCH_CPULSE:
            switch (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[InActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
            {
            case CT_IVCH_CPULSE:
                PULSE_safety_check::Init();//初始化安全检查
                break;
            default:
                Setup_PulseTask();  //切换脉冲任务
                break;
            }
            break;
        default:
            switch (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[InActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
            {
            case CT_IVCH_CPULSE:
                //这里是最后一次把关，停止脉冲
                Pulse_End(uc_ChannelNo);
                Remove_PulseTask();    //恢复普通任务
                break;
            default:
                break;
            }
            break;

        }
        return;    //  20130927 何
    }

    if (p_Info->EngineStatus == 0) //引擎没有调用
    {
//         StepDisposableMSG[uc_ChannelNo].Clamp_Value   = ChannelInfo_Manager::CalibrateClampDAC(uc_ChannelNo,
//                 p_Info->m_ucVoltageRange,
//                 p_Info->m_fVclampHigh);
//         StepDisposableMSG[uc_ChannelNo].Clamp_Value_2 = ChannelInfo_Manager::CalibrateLowClampDAC(uc_ChannelNo,
//                 p_Info->m_ucVoltageRange,
//                 p_Info->m_fVclampLow);

        StepDisposableMSG[uc_ChannelNo].AutoCaliFlag = 0;    //comment out chen 20130423


        if(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
            uIntraLatstChanIdx = uc_ChannelNo;
        else if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
            uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
        else if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)) || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)))
            uIntraLatstChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[p_Info->m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1;


        for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
        {
			StepDisposableMSG[uc_idx].Clamp_Value   = ChannelInfo_Manager::CalibrateClampDAC(uc_idx,
                p_Info->m_ucVoltageRange,
                p_Info->m_fVclampHigh);
			StepDisposableMSG[uc_idx].Clamp_Value_2 = ChannelInfo_Manager::CalibrateLowClampDAC(uc_idx,
                p_Info->m_ucVoltageRange,
                p_Info->m_fVclampLow);
			
            EngineHandle[uc_idx] = OS_LogicProcess::requestLogicPorcess(uc_idx, &StepDisposableMSG[uc_idx]);
            //comment out chen    20130710    暂时屏蔽
            ChannelInfo_Manager::ChannelInfo[uc_idx].EngineStatus = 1;
            if(EngineHandle[uc_idx] == 0)
            {
                ChannelInfo_Manager::ChannelInfo[uc_idx].EngineStatus = 0;
                //Error 若出现溢出现象
                return;    //
            }


        }

    }

    MetaVariable_Task::update_dVdq(uc_ChannelNo); //1901042 zyx
    MetaVariable_Task::update_dQdV(uc_ChannelNo);

    if(CommonFunc::uc_Support_AUX)
        ProcessLimitAckState(uc_ChannelNo);

    if(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        uIntraLatstChanIdx = uc_ChannelNo;
    else if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
    else if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo)) || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)))
        uIntraLatstChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[p_Info->m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1;

    for(unsigned char uc_idx = uc_ChannelNo; uc_idx <= uIntraLatstChanIdx; uc_idx++)
    {
        ProcessEngineState(uc_idx);   // 2015/05/07 引擎状态处理  这里要处理多通道内部并联
    }
}

/*****************************************************
// Function Name: Step_RunningRoutine
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                     RUNNING状态的稳态响应
// Others:
//                进行控制输出；
//                进行Step_Limit的检测，如有不可跳步Step_Limit到达，则上发Sch_LimitStatusChanged信息；
//                如可跳步Step_Limit到达，则上传信息Sch_LimitArrived信息；同时转入WAIT状态
//
// History：(目前 仅测试用)
//
// ******************************************************/
void Step_Manager_task::Step_RunningRoutine(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;

    switch(StepRunningStatus[uc_ChannelNo])
    {
    case STEPRUNNING_READY:
        ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        m_Range[uc_ChannelNo] = p_Ctrl->m_ucCurrentRange;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange = m_Range[uc_ChannelNo];//p_Ctrl->m_ucCurrentRange;

        StepRunningStatus[uc_ChannelNo] = STEPRUNNING_JUDGESPECIALCASE;
        break;

    case STEPRUNNING_JUDGESPECIALCASE:
        Step_JudgeSpecialCase(uc_ChannelNo, p_Ctrl->m_u16ControlType);

        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].SpecialProcess)
            StepRunningStatus[uc_ChannelNo] = STEPRUNNING_DOSPECIALCASE;
        else
            StepRunningStatus[uc_ChannelNo] = STEPRUNNING_CACULATEOUTPUT;
        break;

    case STEPRUNNING_CACULATEOUTPUT:
        Step_Control(uc_ChannelNo);  //  20130927 何
        ProcessStepCheckLimit(uc_ChannelNo); //40us
        break;


    case STEPRUNNING_DOSPECIALCASE:
        //特殊处理...
        switch(p_Ctrl->m_u16ControlType)
        {
        case CT_IVCH_SETVARIABLE:
        case CT_IVCH_SETVALUE: //171109 zyx
        case CT_TYPE_SMB_WRITEREG_WORD:
        case CT_TYPE_SMB_WRITEREG_STRING:
        case CT_TYPE_SMB_OPTWORDADDRESS:
        case CT_IVCH_WRITE_SMB_BLOCK:
            ProcessStepCheckLimit(uc_ChannelNo);   //  20130927 何
            break;
        case CT_IVCH_CPULSE:
            //脉冲处理接口，这里应该是唯一的启动脉冲的地方。
            if (!Pulse::Pulse_is_Running)       //Add by DKQ 08.20.2014
            {
                Pulse::Start(uc_ChannelNo);
                Pulse::Pulse_is_Running = true;
            }
            ProcessStepCheckLimit(uc_ChannelNo);   //Add by DKQ 08.20.2014,需要狄瑞及培青确认是不是该在这里检查
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }


}

/*****************************************************
// Function Name: Step_JudgeSpecialCase
// Version:     V1.0
// Date:                 2012/11/12
// Description:
//                  判断是否是特殊过程
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::Step_JudgeSpecialCase(unsigned char uc_ChannelNo, unsigned char CtrlType)
{
    switch(CtrlType)
    {
    //...
    case CT_IVCH_CPULSE:
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].SpecialProcess = 1; //特殊过程
        break;
    case CT_IVCH_IR:
    case CT_IVCH_ACIR:
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].SpecialProcess = 0;
        IR::InitIR(uc_ChannelNo);
        break;
    case CT_SPECIAL_IRTEST:
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].SpecialProcess = 0;
        Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo] = 1;
        IR::InitIR(uc_ChannelNo);
        break;
    //...
    default:
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].SpecialProcess = 0;
        break;
    }
}

void Step_Manager_task::CCCV_StepControl(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx;    //  20130927 何
    unsigned char Updata_Flag = 0;
    float ReturnFValue = 0;
    float f_set = 0;
    unsigned char VoltRange = ucGetChnVoltRange(uc_ChannelNo);
    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    float					V_Limit			=	ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_High;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    unsigned char		* 	p_CtrlState		=	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState;
    St_ChannelInfo		*	p_Info			=	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    St_IV_Buffer1		*	p_Buffer		=	&ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo];
    St_CtrlValueCheck	*	p_CheckValue	=	&safety_check::CtrlValueCheck[uc_ChannelNo];

    float TargetI = CCCV_TargetCurrent[uc_ChannelNo] = CalculateCurrent(uc_ChannelNo);
    float TargetV = CCCV_TargetVoltage[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                    p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                    p_Ctrl->m_CtrlValues.m_fControlValue[1]);
    Ctrl_Pid_Coef * p_Pid = &ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo];
    switch(ChannelCCCVStatus[uc_ChannelNo])
    {
    case STEP_CCCV_INIT:
        bCCCV_Initiated[uc_ChannelNo] = true;//courseTune contorl
        CCCV_Init(uc_ChannelNo);   // 初始化
        ClearChnSampleFilter(uc_ChannelNo);
        Step_OutputCurrDac(uc_ChannelNo,0,true);//false
        NewCodeRefreshed[uc_ChannelNo] = 0;
        CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
        CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);

        if(ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
        {
            if(CommonFunc::GetTimeTicks(&Parallel_WaitSlaveChnDisposableFinish_Time[uc_ChannelNo]) < MAX_PARALLEL_WAITDISPOSABLEFINISH_TIME)
            {
                unsigned char uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
                for(unsigned char uc_Idx = uc_ChannelNo + 1; uc_Idx <= uIntraLatstChanIdx; uc_Idx++)
                {
                    if(ChannelInfo_Manager::ChannelInfo[uc_Idx].chn_State ==  LOGIC_PROCESS_TRAN)
                    {
                        return;
                    }

                }
            }
        }

        ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_RAMP;
        CCCV_RAMP_Status[uc_ChannelNo] = RAMP_INIT;
        break;

    case STEP_CCCV_RAMP:
        switch(CCCV_RAMP_Status[uc_ChannelNo])
        {
        case RAMP_INIT:
            if(IsGetNewSample(uc_ChannelNo,RAMP_INIT))
            {
                if(IsCCCV_VoltageArrived(uc_ChannelNo,TargetV,TargetI))
                {
                    SetChnSampleFilter(uc_ChannelNo);
                    if(p_Ctrl->m_u16ControlType == CT_IVCH_CCCV)
                    {
                        CCCV_CV_Init(uc_ChannelNo, TargetV, p_Buffer->m_fVoltage);
                        ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_CV;
                    }
                    else
                    {
                        ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_RELAX;
                        ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage = p_Buffer->m_fVoltage;
                        ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);//p_Buffer->m_fCurrent
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime = CommonFunc::GetSystemTime();
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_before_Relax = STEP_CCCV_INIT;
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = true;   //Stop logging the relxation dat
                    }
                }
                else
                {
                    CCCV_SampleAndCalculateRampStage(uc_ChannelNo,TargetV,TargetI);
                    CCCV_RAMP_Status[uc_ChannelNo] = FAST_RAMP;
                }
            }
            break;
        case FAST_RAMP:
            float IR = Get_IR_Value(uc_ChannelNo);
            if(IR > 0)
            {
                float I_safe = 0.8f*((TargetV-p_Buffer->m_fVoltage)/IR + f_CCCV_Ramp_I0[uc_ChannelNo]);

                if( (TargetI>=0 && TargetI<=I_safe) || (TargetI<0 && TargetI>=I_safe) )
                    ChannelCCCVCounter[uc_ChannelNo] = DEF_CCCV_RESUME_STAGE_COUNT;
                else
                {
                    float start_counter = DEF_CCCV_RESUME_STAGE_COUNT * I_safe / TargetI;
                    if(start_counter<0)
                        start_counter = 0;
                    if(start_counter > DEF_CCCV_RESUME_STAGE_COUNT)
                        start_counter = DEF_CCCV_RESUME_STAGE_COUNT;

                    ChannelCCCVCounter[uc_ChannelNo] = start_counter;//(unsigned int)

                }
                CV2CCCVRefreshValue[uc_ChannelNo] = CV2CCCV_K_gain[uc_ChannelNo] * ChannelCCCVCounter[uc_ChannelNo] + f_CCCV_Ramp_I0[uc_ChannelNo];
                Step_OutputCurrDac(uc_ChannelNo,CV2CCCVRefreshValue[uc_ChannelNo],true);//false

                if(ChannelCCCVCounter[uc_ChannelNo] >= DEF_CCCV_RESUME_STAGE_COUNT)
                {
                    CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
                    CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
                    CCCV_RAMP_Status[uc_ChannelNo] = RAMP_IR_CALCULATE;
                    break;
                }
            }
            CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
            CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
            CCCV_RAMP_Status[uc_ChannelNo] = SLOW_RAMP;
            break;
        case SLOW_RAMP:
            if(IsGetNewSample(uc_ChannelNo))
            {
                if(IsCCCV_VoltageArrived(uc_ChannelNo,TargetV,TargetI))
                {
                    CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
                    CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
                    CCCV_RAMP_Status[uc_ChannelNo] = RAMP_IR_CALCULATE;//TEST;
                    break;
                }
                if(ChannelCCCVCounter[uc_ChannelNo] < DEF_CCCV_RESUME_STAGE_COUNT)
                    ChannelCCCVCounter[uc_ChannelNo]++;

                CV2CCCVRefreshValue[uc_ChannelNo] = CV2CCCV_K_gain[uc_ChannelNo] * ChannelCCCVCounter[uc_ChannelNo] + f_CCCV_Ramp_I0[uc_ChannelNo];
                Step_OutputCurrDac(uc_ChannelNo,CV2CCCVRefreshValue[uc_ChannelNo],true);//false

                if(ChannelCCCVCounter[uc_ChannelNo]>=DEF_CCCV_RESUME_STAGE_COUNT)
                    CCCV_RAMP_Status[uc_ChannelNo] = RAMP_IR_CALCULATE;

                CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
                CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
            }
            break;
        case RAMP_IR_CALCULATE:
            if(IsGetNewSample(uc_ChannelNo, RAMP_IR_CALCULATE))
            {
                CCCV_RampCalculateIR(uc_ChannelNo);
                SetChnSampleFilter(uc_ChannelNo);
                if(IsCCCV_VoltageArrived(uc_ChannelNo,TargetV,TargetI))
                {
                    if(p_Ctrl->m_u16ControlType == CT_IVCH_CCCV)
                    {
                        CCCV_CV_Init(uc_ChannelNo, TargetV, p_Buffer->m_fVoltage);
                        ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_CV;
                    }
                    else
                    {
                        ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_RELAX;
                        ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage = p_Buffer->m_fVoltage;
                        ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);//p_Buffer->m_fCurrent
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime = CommonFunc::GetSystemTime();
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_before_Relax = STEP_CCCV_RAMP;
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = true;   //Stop logging the relxation dat
                    }
                }
                else
                {
                    ChannelInfo_Manager::Init_HistoryData(uc_ChannelNo);
                    b_CurCoarseTune[uc_ChannelNo]=true;
                    b_CurCoarseTuneInit[uc_ChannelNo]=false;
                    CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
                    CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
                    ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_CC;
//                    Pid_Ctrl::LastCal[uc_ChannelNo]=CommonFunc::GetSystemTime();
                    CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;
                }
            }
            break;
        }
        break;

    case STEP_CCCV_RELAX:
//         ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 0;//撤销电流滤波
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        Step_OutputCurrDac(uc_ChannelNo,0,true);   //Output current 0 //false

        if(CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime) <100) // delay 10ms for let the current die down
            break;
        else
        {
            if(CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime) <200)    //sample voltage for 20ms
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Voltage += p_Buffer->m_fVoltage;
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Counter ++;
                break;

            }
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Voltage /= ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Counter;
            float Voltage_Accurace = V_Limit*5E-5f;  //might be needed to adjust based on the system requirment
            float Voltage_Difference = (ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage -  \
                                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Voltage);
            if (abs(Voltage_Difference) < Voltage_Accurace)
                Voltage_Difference = 0;
            float temp_IR = Voltage_Difference/(ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current) ;
            //need to calculate minimum IR

            if(temp_IR >0)    // only after get a valid value we update the IR, otherwise, we keep whatever it has.
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Relax = temp_IR;
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Relax = true;
            }
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed = true;
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Voltage = 0;
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relax_Counter = 0;
            ChannelCCCVStatus[uc_ChannelNo] =  STEP_CCCV_AFTER_RELAX;
        }

        break;

    case STEP_CCCV_AFTER_RELAX:
//         ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 0;//恢复电流滤波
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        float AfterRelaxCurrent = 0;
        if ( ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_before_Relax == STEP_CCCV_CC)
            AfterRelaxCurrent = CalculateCurrent(uc_ChannelNo);   // resume CC current ASAP
        else
            AfterRelaxCurrent = ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current;   // resume current before relax ASAP

        Step_OutputCurrDac(uc_ChannelNo,AfterRelaxCurrent,false);   // resume CC current ASAP

        if(CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime)>500) // delay 10ms for let the current die down
        {
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;    //resume log data
//             ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 1;//恢复电流滤波
            ChannelInfo_Manager::RecoverFilter(uc_ChannelNo);
            CCCV_CV_Init(uc_ChannelNo, TargetV, p_Buffer->m_fVoltage);
            ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_CV;

        }
        break;
    case STEP_CCCV_CC://Constant Current
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;
        CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  //20150916,add by hepeiqing,for huawei's debug
        TargetI = CalculateCurrent(uc_ChannelNo);
        ReturnFValue =TargetI;
        CCCV_TargetCurrent[uc_ChannelNo] =TargetI;//qjm 200122
        TargetV = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                  p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                  p_Ctrl->m_CtrlValues.m_fControlValue[1]);

        if(IsCCCV_VoltageArrived(uc_ChannelNo,TargetV,TargetI))
        {
            if(!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed && p_Ctrl->m_u16ControlType==CT_IVCH_CCCV_WRM)
            {
                ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage = p_Buffer->m_fVoltage;
                ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);////p_Buffer->m_fCurrent;

                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RelaxStartTime = CommonFunc::GetSystemTime();
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_before_Relax = STEP_CCCV_CC;
                ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_RELAX;
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = true;
                break;
            }
            else
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Relaxed = false;   //clear fhte flag
//                Pid_Ctrl::m_StartRange[uc_ChannelNo]=ChannelInfo_Manager::Get_PID_MaxCurrentRange(uc_ChannelNo);
                CCCV_CV_Init(uc_ChannelNo, TargetV, p_Buffer->m_fVoltage);
                ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_CV;
            }
            break;
        }
        if(b_CurCoarseTune[uc_ChannelNo])
        {
            if(!b_CurCoarseTuneInit[uc_ChannelNo])
            {
                uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                b_CurCoarseTuneSampleFinish[uc_ChannelNo]=false;
                b_CurCoarseTuneInit[uc_ChannelNo]=true;
                Updata_Flag = 1;
                m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime=m_CurFineTuneFactor.m_fStartTime+ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime);
                if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)||ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
                {
                    m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime +=m_CurFineTuneFactor.m_fParaWaitTime;
                }
                f_set = Step_Manager_task::CalculateCurrent(uc_ChannelNo);
                ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_set);
            }
            else
            {
                bCCCV_Initiated[uc_ChannelNo] = false;//courseTune
                if(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime)>m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime)
                {
                    if(NewCodeRefreshed[uc_ChannelNo])
                    {
                        NewCodeRefreshed[uc_ChannelNo]=0;
                        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))//dir 20190821
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
                        else
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fCurrent;

                        uc_CurCoarseTuneSampleCnt[uc_ChannelNo]++;
                        if(uc_CurCoarseTuneSampleCnt[uc_ChannelNo]>=SAMPLE_POINT)
                        {
                            uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                            b_CurCoarseTuneSampleFinish[uc_ChannelNo]=true;
                            Updata_Flag = 1;
                        }
                    }
                }
            }
        }
        else
        {
            if(!b_CC_RampStampFlag[uc_ChannelNo])
            {

                MsTime TimeDiff = CommonFunc::TimeDiff(uc_CC_RampStampTime[uc_ChannelNo]);
                float time2 = TimeDiff.Second+ (float)(TimeDiff.Us100/10000);
                if(time2>m_CurFineTuneFactor.m_fStartTime)
                {
                    b_CC_RampStampFlag[uc_ChannelNo]=true;
                    for(unsigned char uc_Idx = uc_ChannelNo; uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize; uc_Idx++)
                        ChannelInfo_Manager::RecoverFilter(uc_Idx);
                }
            }
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
            {
                if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 6.4 ms modified by Ron 080715
                    Updata_Flag = 1;
            }
            else
            {
                if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))
                    Updata_Flag = 1;
            }
        }
        if(Updata_Flag||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bDynamicControl)
        {
            if(b_CurCoarseTuneSampleFinish[uc_ChannelNo])
            {
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
            }
        }
        p_CheckValue->fCtrlValue = TargetI; //fCurrent;
        break;
    case STEP_CCCV_CV://Constant Voltage
//         if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HS21044)//防止采样过快导致的PID抖动
//         {
//             if(!((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 4) & 0x0F) == uc_ChannelNo)))
//                 break;
//         }
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;
        bCV[uc_ChannelNo] = 1;
//         ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 1;//恢复电流滤波
        ChannelInfo_Manager::RecoverFilter(uc_ChannelNo);

        ChannelInfo_Manager::GetPV_CV_StageCurrent(uc_ChannelNo);
        CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  //20150916,add by hepeiqing,for huawei's debug
        p_Info->m_bCVstage = 1;
        ReturnFValue = TargetV;
        if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)
        {
            ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput = 0;
            Step_OutputVoltDac(uc_ChannelNo,ReturnFValue,false);
            p_CheckValue->fCtrlValue = TargetV;
            Updata_Flag = 1;
        }
#if (IV_AUTORANGE == 1)
        if(ChannelInfo_Manager::BoardCFG.m_Global.bAutoRangeEnable)
        {
            AutoRangeSwtichControl(uc_ChannelNo);
        }
#endif
        break;
    default:
        CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  //20150916,add by hepeiqing,for huawei's debug
        ReturnFValue = 0;
        break;
    }
    *p_CtrlState=CCS_I;
    CCCV_TargetCurrent[uc_ChannelNo] = TargetI;
    CCCV_TargetVoltage[uc_ChannelNo] = TargetV;
}


void Step_Manager_task::Step_Control(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx;    //  20130927 何
    unsigned char Updata_Flag = 0;
    float ReturnFValue = 0;
    float fValue1;
    float time;
    float f_set = 0;
    unsigned char uc_range = 0;
    unsigned char uc_TotalChannels =ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    float					V_Limit			=	ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[uc_range].m_Bound.m_High;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    unsigned char		* 	p_CtrlState		=	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState;
    St_ChannelInfo		*	p_Info			=	&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    St_IV_Buffer1		*	p_Buffer		=	&ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo];
    St_CtrlValueCheck	*	p_CheckValue	=	&safety_check::CtrlValueCheck[uc_ChannelNo];

    switch(p_Ctrl->m_u16ControlType)
    {
    case CT_TYPE_DO_SETTING:   //DO 输出时暂时不对通道操作 保持上次的状态，仅仅输出数字电平 dirui 2015/03/17
        unsigned char DigitalValue;
        unsigned char DigitalChn;
        DigitalChn = p_Ctrl->m_CtrlValues.m_fControlValue[0];
        DigitalValue = p_Ctrl->m_CtrlValues.m_fControlValue[1];
        board_config::OUTPUT_UnitDO(uc_ChannelNo, DigitalChn, DigitalValue);
        break;
    case CT_IVCH_REST:

        if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))		 //update dac every 100ms
            Updata_Flag = 1;

        if(Updata_Flag)
        {
            ReturnFValue=0;
            p_CheckValue->fCtrlValue = ReturnFValue;
        }
        *p_CtrlState=CCS_REST;
        break;
    case CT_IVCH_SETVARIABLE: //180503 zyx  //mits8
    case CT_IVCH_SETVALUE:
    case CT_IVCH_EXTCHARGE:
        *p_CtrlState = CCS_REST;
        break;
    case CT_IVCH_PAUSE:
        *p_CtrlState = CCS_REST;
        // For ACIM
        if(Step_Manager_task::ACIM_Status[uc_ChannelNo] == ACIM_WAIT_RELAY_ON ||
                Step_Manager_task::ACIM_Status[uc_ChannelNo] == ACIM_WAIT_RELAY_OFF)
        {
            //Open ACIM relay disposable function
            if(!AcimRequest_Send[uc_ChannelNo])	// Only send request once
            {
                St_DisposableMSG    DisposableMSG;
                DisposableMSG.AutoCaliFlag = 0;
                if(Step_Manager_task::ACIM_Status[uc_ChannelNo] == ACIM_WAIT_RELAY_ON)
                {
                    DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr;
                    DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlAcimOn.stepLen;
                }
                else
                {
                    DisposableMSG.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr;
                    DisposableMSG.Disposable_task.stepLen = OS_LogicProcess::SetCtrlAcimOff.stepLen;
                }
                DisposableMSG.Disposable_Flag = DISPOSABLE_to_CLOSECHN;

                EngineHandle[uc_ChannelNo] = OS_LogicProcess::requestLogicPorcess(uc_ChannelNo, &DisposableMSG);

                if(EngineHandle[uc_ChannelNo] != false)
                    AcimRequest_Send[uc_ChannelNo] = true;
            }

            switch(OS_LogicProcess::getHandleStatus(EngineHandle[uc_ChannelNo]))
            {
            case DISPOSABLE_ASSIGNED:                        //已经交付，待运行
                EngineOverTimeTriger[uc_ChannelNo] = 1;
                MsTimeEngineTask[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
                break;
            case DISPOSABLE_RUNNING:          //运行中
                unsigned long long TimeTicks;
                MsTime MyMsTime;
                MyMsTime.Second = SysTimeEngineTask[uc_ChannelNo].Second;
                MyMsTime.Us100 = SysTimeEngineTask[uc_ChannelNo].Us100;
                TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
                if(EngineOverTimeTriger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)       //超时？
                {
                    EngineOverTimeTriger[uc_ChannelNo] = 0;
                    //引擎调用失败处理

                }
                break;
            case DISPOSABLE_FINISHED:           //结束
                EngineOverTimeTriger[uc_ChannelNo] = 0;
                if(Step_Manager_task::ACIM_Status[uc_ChannelNo] == ACIM_WAIT_RELAY_ON)
                {
                    EthernetParse::m_ReportStatus.m_btCMD = PC_AcimRelayOn_Msg;
                    Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_TEST;
                }
                else
                {
                    EthernetParse::m_ReportStatus.m_btCMD = PC_AcimRelayOff_Msg;
                    Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_NONE;
                }
                EthernetParse::MPReportData(uc_ChannelNo, SEND_EVENT_CONTENT);
                AcimRequest_Send[uc_ChannelNo] = false;
                break;
            case DISPOSABLE_ERROR:            //出错
                //引擎调用失败处理
                break;
            case DISPOSABLE_DEAD:             //历史记录已经消亡
                break;
            default:
                break;
            }
        }
        break;
    case CT_IVCH_CURRENT:
#if (ENABLE_FINE_TUNE == 1)
        if(b_CurCoarseTune[uc_ChannelNo] ==true)
        {
            if(!b_CurCoarseTuneInit[uc_ChannelNo])
            {
                uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                b_CurCoarseTuneSampleFinish[uc_ChannelNo]=false;
                for(unsigned char uc_Idx = uc_ChannelNo;
                        uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                        uc_Idx++)
                    ChannelInfo_Manager::SetFilterStatus(uc_Idx, false);
                b_CurCoarseTuneInit[uc_ChannelNo]=true;
                m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime=m_CurFineTuneFactor.m_fStartTime + ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime);
                if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
                {
                    m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime +=m_CurFineTuneFactor.m_fParaWaitTime;
                }
                f_set = Step_Manager_task::CalculateCurrent(uc_ChannelNo);
                ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_set);
            }
            else
            {
                if(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime) > m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime)
                {
                    if(NewCodeRefreshed[uc_ChannelNo])
                    {
                        NewCodeRefreshed[uc_ChannelNo]=0;

                        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))  //dir 20190821
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
                        else
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fCurrent;
                        uc_CurCoarseTuneSampleCnt[uc_ChannelNo]++;
                        if(uc_CurCoarseTuneSampleCnt[uc_ChannelNo]>=SAMPLE_POINT)
                        {
                            uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                            b_CurCoarseTuneSampleFinish[uc_ChannelNo]=true;
                            Updata_Flag = 1;
                        }
                    }
                }
            }
        }
        else
        {
            if(!b_CC_RampStampFlag[uc_ChannelNo])
            {
                MsTime TimeDiff = CommonFunc::TimeDiff(uc_CC_RampStampTime[uc_ChannelNo]);
                time = TimeDiff.Second +(float)(TimeDiff.Us100)/10000;
                if(time>m_CurFineTuneFactor.m_fStartTime)
                {
                    for(unsigned char uc_Idx = uc_ChannelNo;
                            uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                            uc_Idx++)
                        ChannelInfo_Manager::RecoverFilter(uc_Idx);
                    b_CC_RampStampFlag[uc_ChannelNo]=true;
                }
            }
        }
#endif
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
        {
            if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 6.4 ms modified by Ron 080715
                Updata_Flag = 1;
        }
        else
        {
            if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))
                Updata_Flag = 1;
        }

        if(Updata_Flag||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bDynamicControl)
        {
            if(b_CurCoarseTuneSampleFinish[uc_ChannelNo])
            {
                ReturnFValue = Step_Manager_task::CalculateCurrent(uc_ChannelNo); //1907161 zyx
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
                p_CheckValue->fCtrlValue = ReturnFValue;
            }
        }

        *p_CtrlState=CCS_I;
        break;
    case CT_IVCH_VOLTAGE:		//04.23.2013
    case CT_IVCH_DIGIV:
    {
        float StartV = CV_StartVoltage[uc_ChannelNo];
        float TargetV = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                        p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
                        p_Ctrl->m_CtrlValues.m_fControlValue[0]);   //目标值

        uc_range = p_Info->m_ucVoltageRange;
        V_Limit			=	ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[uc_range].m_Bound.m_High;

        switch(ChannelCVStatus[uc_ChannelNo])
        {
        case STEP_CV_Init:
            StartV = p_Buffer->m_fVoltage; // 当前电压值；
//             TargetV = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
//                       p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
//                       p_Ctrl->m_CtrlValues.m_fControlValue[0]);   //目标值

            if(StartV < 0.7f * TargetV) //  如果时电阻特别是串联时，从通道有电压建立但是负载没有电压值，因为没有开通道引起。
                StartV = TargetV;

            if(TargetV >= StartV)
                CV2CCCV_K_gain[uc_ChannelNo] = V_Limit / 200;      //K 值
            else
                CV2CCCV_K_gain[uc_ChannelNo] = -V_Limit / 200;
            uw16_CVResumeStageCounter[uc_ChannelNo] = (TargetV - StartV) / CV2CCCV_K_gain[uc_ChannelNo]; //根据差值，计算RAMP 步数
            ChannelCVCounter[uc_ChannelNo] = 0;
            CV_RampStageKeepTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
            CCCVNoLimitCheckFlag[uc_ChannelNo] = 1;  //Add by DKQ 10_17_2014
            ChannelCVStatus[uc_ChannelNo] = STEP_CV_RAMP;
            ReturnFValue = StartV;
            ChannelCCCV_LimitDlyCounter[uc_ChannelNo] = 0;
            break;
        case STEP_CV_RAMP:
            if(NewCodeRefreshed[uc_ChannelNo])
            {
                if ( ChannelCVCounter[uc_ChannelNo] >= uw16_CVResumeStageCounter[uc_ChannelNo])
                {
                    ChannelCVStatus[uc_ChannelNo]= STEP_CV_KEEP;
                    CommonFunc::GetTimeTicksUpdate(&VoltOutputInterval[uc_ChannelNo]);

#if DEF_CV_RESUME_DELAY == 0
                    CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;
#endif
                    ReturnFValue = TargetV;
                    uw16_CVResumeStageCounter[uc_ChannelNo] = 0;
                }
                else
                {
                    fValue1 = p_Buffer->m_fVoltage; // 当前电压值；
                    if (abs (fValue1 - TargetV) <=  0.001f*abs(V_Limit))
                    {
                        ChannelCVStatus[uc_ChannelNo]= STEP_CV_KEEP;
                        CommonFunc::GetTimeTicksUpdate(&VoltOutputInterval[uc_ChannelNo]);

                        ChannelInfo_Manager::SetPV_CV_stagetime(uc_ChannelNo);//161019 zyx //161123
#if DEF_CV_RESUME_DELAY == 0
                        CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  // Add by DKQ 10_17_2014
#endif
                        uw16_CVResumeStageCounter[uc_ChannelNo] = 0;
                        ChannelCVCounter[uc_ChannelNo] = 0 ;
                        ReturnFValue = TargetV;
                    }
                    else
                    {
                        unsigned char uc_UpdateCounter = 0;
                        float  ramp_target_voltage =   StartV + ChannelCVCounter[uc_ChannelNo]*CV2CCCV_K_gain[uc_ChannelNo];
                        unsigned int div=0;
                        if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_PWM)&&board_config::DacInfo.m_bHaveVoltageDac)
                            div=600;
                        else
                            div=1000;
                        if(::abs(ramp_target_voltage - fValue1) > V_Limit/div )
                        {
                            MsTime timediff = CommonFunc::TimeDiff(CV_RampStageKeepTime[uc_ChannelNo]);
                            if(timediff.Us100 >= CV_STAGE_KEEP_TIME_OUT)//CV台阶保持超过10ms跳转一下台阶
                            {
                                uc_UpdateCounter = 1;
                            }
                            else
                            {
                                ReturnFValue = ramp_target_voltage;
                            }
                        }
                        else
                        {
                            uc_UpdateCounter = 1;
                        }
                        if(uc_UpdateCounter)
                        {
                            CV_RampStageKeepTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
                            ChannelCVCounter[uc_ChannelNo]++;
                            ReturnFValue = StartV + ChannelCVCounter[uc_ChannelNo]*CV2CCCV_K_gain[uc_ChannelNo];
                        }
                    }
                }
                NewCodeRefreshed[uc_ChannelNo] =0;
            }
            break;
        case STEP_CV_KEEP:
            ReturnFValue = TargetV;
            bCV[uc_ChannelNo] = 1;
#if (IV_AUTORANGE == 1)
            if(ChannelInfo_Manager::BoardCFG.m_Global.bAutoRangeEnable)
            {
                AutoRangeSwtichControl(uc_ChannelNo);
            }
#endif
#if DEF_CV_RESUME_DELAY == 1
            if(CCCVNoLimitCheckFlag[uc_ChannelNo])
            {
                if(::abs(ReturnFValue - p_Buffer->m_fVoltage) <= V_Limit / 2500)
                    CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;      //if Constant Voltage is stable,check the PV_Current limit;
                ChannelCCCV_LimitDlyCounter[uc_ChannelNo] ++;
                if(ChannelCCCV_LimitDlyCounter[uc_ChannelNo] >= 2400)  // overtime protect 2S
                    CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;      //if Voltage is not stable, but we got a 2 second protection here;
            }
#endif
            break;
        default:
            CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  //20150916,add by hepeiqing,for huawei's debug
            ReturnFValue = p_Buffer->m_fVoltage;//ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage);  // 当前电压值；
            break;
        }

        CV_StartVoltage[uc_ChannelNo] = StartV;
        CV_TargetVoltage[uc_ChannelNo] = TargetV;
//161226 zyx
//         if((ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue >= 0 && (TargetV <= p_Buffer->m_fVoltage))
//                 ||(ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue < 0 && (TargetV >= p_Buffer->m_fVoltage)))
        if(abs (p_Buffer->m_fVoltage - TargetV) <=  0.0004f*abs(V_Limit))
        {
            ChannelInfo_Manager::GetPV_CV_StageCurrent(uc_ChannelNo);
            if(ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageTime].m_bNeedRecal == 0)//190917 zyx
                ChannelInfo_Manager::SetPV_CV_stagetime(uc_ChannelNo);
        }
//161226 zyx
//         if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HS21044) && (ChannelCVStatus[uc_ChannelNo] == STEP_CV_KEEP))//防止采样过快导致的PID抖动
//         {
//             if(!((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 4) & 0x0F) == uc_ChannelNo)))
//                 break;
//         }
        if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)
        {
            ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput = 0;
            Step_OutputVoltDac(uc_ChannelNo,ReturnFValue,false);
            p_CheckValue->fCtrlValue = TargetV;  //ReturnFValue;
            Updata_Flag = 1;
        }
        if(p_Info->bDigitalV)
            *p_CtrlState = CCS_I;
        else
            *p_CtrlState = CCS_V;
        break;
    }
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
    {
        CCCV_StepControl(uc_ChannelNo);
        break;
    }

//=========================================================================================
//===============CCCV_TB BY ZC 07.07.2017==================================================
//=========================================================================================

    case CT_IVCH_CCCV_TB:
    {
        //time
        unsigned long long TimeTicks;
        MsTime *tb_TimeBuffer = &ChannelInfo_Manager::CCCV_TB_TimeBuffer[uc_ChannelNo];
        TimeTicks = CommonFunc::GetTimeTicks(tb_TimeBuffer); //求系统时间和tb_TimeBuffer的时间差，即计算距离上一个状态过去的时间
        Ctrl_Pid_Coef * p_Pid = &ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo];  //取PID参数

        CCCV_TB_ChargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                              p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
                                              p_Ctrl->m_CtrlValues.m_fControlValue[0]);

        CCCV_TB_DischargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                p_Ctrl->m_CtrlValues.m_fControlValue[1]);

        CCCV_TB_TargetVoltage[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                              p_Ctrl->m_ucCtrlValue_DataType[4],p_Ctrl->m_CtrlValueOperator[4],p_Ctrl->m_CtrlValueCoefficients[4],
                                              p_Ctrl->m_CtrlValues.m_fControlValue[4]);

        switch(ChannelTBStatus[uc_ChannelNo])
        {
        case STEP_CCCV_TB_START:
        {
//=============从上位机取得所需控制参数：CC，CC WIDTH，DC，DC WIDTH，CV======================================

            CCCV_TB_ChargeTime[uc_ChannelNo] = 10.0f * ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                               p_Ctrl->m_ucCtrlValue_DataType[2],p_Ctrl->m_CtrlValueOperator[2],p_Ctrl->m_CtrlValueCoefficients[2],
                                               p_Ctrl->m_CtrlValues.m_fControlValue[2]);

            CCCV_TB_DischargeTime[uc_ChannelNo] = 10.0f * ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                  p_Ctrl->m_ucCtrlValue_DataType[3],p_Ctrl->m_CtrlValueOperator[3],p_Ctrl->m_CtrlValueCoefficients[3],
                                                  p_Ctrl->m_CtrlValues.m_fControlValue[3]);

            for(unsigned char uc_i = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo;
                    uc_i < ChannelInfo_Manager::CCCV_TB_LimitCount[uc_ChannelNo]; uc_i++)
            {
                St_Limit * p_Limit = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[uc_i];
                if( (p_Limit->m_fLeftValue == MetaCode_DV_CCCV_TB_PulseInterval) && (p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_METAVALUE)
                        && ( (p_Limit->m_ucCompareSign == MP_MCS_GREATER_THAN) || (p_Limit->m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO) )
                        &&(p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE) )
                {
                    CCCV_TB_DV_PulseCount[uc_ChannelNo] = p_Limit->m_fRightValue;
                }

            }
            if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
            {
                CCCV_TB_PULSECOUNTER[uc_ChannelNo] = CCCV_TB_DV_PulseCount[uc_ChannelNo] - 1;
            }
            ChannelTBStatus[uc_ChannelNo] = STEP_CCCV_TB_CC;
            //记录时间
            ChannelInfo_Manager::CCCV_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
            //求系统时间和tb_TimeBuffer的时间差，即计算距离上一个状态过去的时间
            TimeTicks = CommonFunc::GetTimeTicks(tb_TimeBuffer);
        }
//  进入CC阶段=================================================================
        case STEP_CCCV_TB_CC://Constant Current
        {
            if(TimeTicks >= CCCV_TB_ChargeTime[uc_ChannelNo])	//已经充电1s，进入放电模式
            {
                //记录时间
                ChannelInfo_Manager::CCCV_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
                //记录进入放电脉冲前的电流电压
                ChannelInfo_Manager::CCCV_TB_IR_Volt[uc_ChannelNo] = p_Buffer->m_fVoltage;
                ChannelInfo_Manager::CCCV_TB_IR_Curr[uc_ChannelNo] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);//p_Buffer->m_fCurrent;
                //放电模式下，由于时间很短，因此不需要滤波
//                 ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 0;
//                 ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
                //输出放电电流值
                Step_OutputCurrDac(uc_ChannelNo,CCCV_TB_DischargeCurrent[uc_ChannelNo],true);
                //SNDLOG
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                    SendLog(uc_ChannelNo);  //dirui20170907
                }
                //进入放电脉冲阶段
                ChannelTBStatus[uc_ChannelNo] = STEP_CCCV_TB_CC_DISCHARGE;
                ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] = 0;
                //隔N个脉冲打一个清晰的脉冲
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    CCCV_TB_PULSECOUNTER[uc_ChannelNo] ++;
                    if( CCCV_TB_PULSECOUNTER[uc_ChannelNo] > CCCV_TB_DV_PulseCount[uc_ChannelNo])
                        CCCV_TB_PULSECOUNTER[uc_ChannelNo] = 1;
                }
            }
            else
            {
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //12.25.2018 qzy
                {
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                }
//                 ChannelInfo_Manager::RecoverFilter(uc_ChannelNo);

//  进入CV阶段=================================================================
                if( ((CCCV_TB_ChargeCurrent[uc_ChannelNo] >= 0.0f)&&(p_Buffer->m_fVoltage >= CCCV_TB_TargetVoltage[uc_ChannelNo]))       //Charge process
                        ||((CCCV_TB_ChargeCurrent[uc_ChannelNo] < 0.0f)&&(p_Buffer->m_fVoltage <= CCCV_TB_TargetVoltage[uc_ChannelNo])) )		   //Discharge process
                {
                    p_Info->DigitCtrl=1;
                    p_Pid->m_PidTypeFlag = PID_V;
                    p_Pid->m_SetValue_1B = CCCV_TB_TargetVoltage[uc_ChannelNo];
                    p_Pid->m_fLastErr_1B = p_Pid->m_SetValue_1B -  p_Buffer->m_fVoltage;//记录进入PID初始值
                    p_Pid->m_LastOutput_1B = ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,  ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current));//p_Buffer->m_fCurrent); //记录此时电流值(归一值)作为PID初始output //ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,

                    ChannelInfo_Manager::CCCV_TB_CurBuffer[uc_ChannelNo] = CCCV_TB_ChargeCurrent[uc_ChannelNo];  //Record CC value before entering CV mode
//                     Pid_Ctrl::m_Counter[uc_ChannelNo]=0;
//                    Pid_Ctrl::m_LastVoltage[uc_ChannelNo]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
//                    Pid_Ctrl::LastCal[uc_ChannelNo] =CommonFunc::GetSystemTime();
                    ChannelTBStatus[uc_ChannelNo] = STEP_CCCV_TB_CV;
                    ChannelInfo_Manager::SetPV_CV_stagetime(uc_ChannelNo);
                    p_CheckValue->CheckType = CtrlValueCheck_Voltage;
                    p_CheckValue->Counter   = 0;
                }
//  未进入CV阶段=========================================================================================
                else
                {
//===================解决CC时电流不稳问题=================================================================
                    MsTime TimeDiff = CommonFunc::TimeDiff(uc_CC_RampStampTime[uc_ChannelNo]);
                    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
                    {
                        if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 6.4 ms modified by Ron 080715
                            Updata_Flag = 1;
                    }
                    else
                    {
                        if((((DacOutputCounter >> 0) & 0x0F)) == uc_ChannelNo)  //update dac every 10ms
                            Updata_Flag = 1;
                    }
                    if(Updata_Flag||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bDynamicControl)
                        Step_OutputCurrDac(uc_ChannelNo,CCCV_TB_ChargeCurrent[uc_ChannelNo],false);
                    p_CheckValue->fCtrlValue = CCCV_TB_ChargeCurrent[uc_ChannelNo]; //fCurrent;
//================================END====================================================================
                }
//====================================额外打点使得脉冲边缘清晰可见=========================================
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    if( CCCV_TB_PULSECOUNTER[uc_ChannelNo] >= CCCV_TB_DV_PulseCount[uc_ChannelNo] )
                    {
                        if(TimeTicks <= 89) //modify by zc 12052017
                        {
                            if(TimeTicks - 10 * ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] >= 10)
                            {
                                //SNDLOG
                                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                                SendLog(uc_ChannelNo);  //dirui 20170907
                                ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo]++;
                            }
                        }
                        else
                        {
                            //达到延时时间后，打点间隔正常
                            ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                        }
                    }
                    else
                        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                }
            }
//=======================================================================================================
        }
        break;

//  CV阶段使用PID调节，将CC充电电流值作为PID输出的上下限，以此保证恒流阶段输出为恒定电流
        case STEP_CCCV_TB_CV:
        {
//========================充电时间到，需要进入放电脉冲=====================================================
            if(TimeTicks >= CCCV_TB_ChargeTime[uc_ChannelNo])	//已经充电1s，进入放电模式
            {
                //记录时间
                ChannelInfo_Manager::CCCV_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
                //放电脉冲模式下，由于时间很短，因此不需要滤波
//                 ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 0;
//                 ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
                //输出放电电流值
                Step_OutputCurrDac(uc_ChannelNo,CCCV_TB_DischargeCurrent[uc_ChannelNo],true);
                //SNDLOG
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                    SendLog(uc_ChannelNo);
                }
                //进入放电脉冲阶段
//                 Pid_Ctrl::m_Counter[uc_ChannelNo]=0;
//                Pid_Ctrl::m_LastVoltage[uc_ChannelNo]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
//                Pid_Ctrl::LastCal[uc_ChannelNo] =CommonFunc::GetSystemTime();
                ChannelTBStatus[uc_ChannelNo] = STEP_CCCV_TB_CC_DISCHARGE;
                ChannelInfo_Manager::CCCV_TB_LastState[uc_ChannelNo] = STEP_CCCV_TB_CV;
                ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] = 0;
                //隔N个脉冲打一个清晰的脉冲
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    CCCV_TB_PULSECOUNTER[uc_ChannelNo] ++;
                    if( CCCV_TB_PULSECOUNTER[uc_ChannelNo] > CCCV_TB_DV_PulseCount[uc_ChannelNo])
                        CCCV_TB_PULSECOUNTER[uc_ChannelNo] = 1;
                }
            }
            else //充电时间还未达到1s
            {
//===========在恒压阶段，进入充电模式之前，因为经历过放电脉冲，因此需要先将电流恢复回之前的充电电流值，延时等待电压回升后再进行PID控制

                if(TimeTicks <= 99)
                {
                    Step_OutputCurrDac(uc_ChannelNo,ChannelInfo_Manager::CCCV_TB_CurBuffer[uc_ChannelNo],true);			//回到进入放电脉冲时的电流值
                    if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                    {
                        if( CCCV_TB_PULSECOUNTER[uc_ChannelNo] >= CCCV_TB_DV_PulseCount[uc_ChannelNo] )
                        {
                            if(TimeTicks <= 89)
                            {
                                if(TimeTicks - 10 * ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] >= 10)
                                {
                                    //SNDLOG
                                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                                    SendLog(uc_ChannelNo);  //dirui 20170907
                                    ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo]++;
                                }
                            }
                        }
                        else
                            ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                    }
                }
//==========================================================================================================================
                else
                {
                    if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //12.25.2018 qzy
                    {
                        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                    }
                    //恢复电流滤波
//                     ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNeedFilter = 1;
//                     ChannelInfo_Manager::RecoverFilter(uc_ChannelNo);
                    //取恒压CV电压值
                    Step_OutputVoltDac(uc_ChannelNo,CCCV_TB_TargetVoltage[uc_ChannelNo],true);
                    p_CheckValue->fCtrlValue = CCCV_TB_TargetVoltage[uc_ChannelNo];
                }
            }
        }
        break;
//===================================================================
//=========放电脉冲模式==============================================
        case STEP_CCCV_TB_CC_DISCHARGE:
        {
//====================放电脉冲时间已到，需要回到恒压阶段========================
            if(TimeTicks >= CCCV_TB_DischargeTime[uc_ChannelNo])
            {
                if(ChannelInfo_Manager::CCCV_TB_LastState[uc_ChannelNo] == STEP_CCCV_TB_CV)
                {
                    ReturnFValue = ChannelInfo_Manager::CCCV_TB_CurBuffer[uc_ChannelNo]; //恢复到放电脉冲前输出电流
                    float Normalized_CCCV_TB_CurBuffer = ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,  ChannelInfo_Manager::CCCV_TB_CurBuffer[uc_ChannelNo]); //防止过冲//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,
                    if( (Normalized_CCCV_TB_CurBuffer >= 0.9f * Pid_Ctrl::fUpLimit[uc_ChannelNo]) && (Normalized_CCCV_TB_CurBuffer > 0) )
                    {
                        Pid_Ctrl::fUpLimit[uc_ChannelNo] = Normalized_CCCV_TB_CurBuffer;
                    }
                }
                else
                    ReturnFValue = CCCV_TB_ChargeCurrent[uc_ChannelNo];				//恢复到输出电流TargetI

                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,true);
                ChannelInfo_Manager::CCCV_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
//=========================IR Mearsurement 08232017 by ZC==============================================
                if(ChannelInfo_Manager::CCCV_TB_LastState[uc_ChannelNo] == STEP_CCCV_TB_CC)
                {
                    float temp_IR = ChannelInfo_Manager::CCCV_TB_IR_Volt[uc_ChannelNo] - p_Buffer->m_fVoltage;
                    temp_IR = temp_IR / (ChannelInfo_Manager::CCCV_TB_IR_Curr[uc_ChannelNo] - ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current));//p_Buffer->m_fCurrent);
                    if(temp_IR > 0)
                    {
                        CCCV_TB_IR[uc_ChannelNo][CCCV_TB_IR_Pointer[uc_ChannelNo]] = temp_IR;
                        CCCV_TB_IR_Pointer[uc_ChannelNo]++;
                        ChannelInfo_Manager::CCCV_TB_IR_Counter[uc_ChannelNo]++;
                        if(CCCV_TB_IR_Pointer[uc_ChannelNo] >= 32)
                            CCCV_TB_IR_Pointer[uc_ChannelNo] = 0;
                        if(ChannelInfo_Manager::CCCV_TB_IR_Counter[uc_ChannelNo] >= 32)
                            ChannelInfo_Manager::CCCV_TB_IR_Counter[uc_ChannelNo] =32;

                        temp_IR = 0;
                        for(int i = 0; i < ChannelInfo_Manager::CCCV_TB_IR_Counter[uc_ChannelNo]; i++ )
                        {
                            temp_IR += CCCV_TB_IR[uc_ChannelNo][i];
                        }
                        temp_IR = temp_IR / ChannelInfo_Manager::CCCV_TB_IR_Counter[uc_ChannelNo];

                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV_TB = temp_IR;
                        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV_TB = true;
                    }
                }
//=========================END=====================================================================
                ChannelTBStatus[uc_ChannelNo] = ChannelInfo_Manager::CCCV_TB_LastState[uc_ChannelNo]; //回到之前阶段
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                    SendLog(uc_ChannelNo);		//SNDLOG
                }
                ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] = 0;
            }
//==================放电脉冲阶段，输出放电脉冲电流值====================================
            else//discharge
            {
                ////判断DAC更新标志位，第一次进入才更新，之后不再更新，输出放电脉冲电流值
                CCCV_TB_DischargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                        p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                        p_Ctrl->m_CtrlValues.m_fControlValue[1]);   //add by zc 171205
                Step_OutputCurrDac(uc_ChannelNo,CCCV_TB_DischargeCurrent[uc_ChannelNo],false);
                p_CheckValue->fCtrlValue = CCCV_TB_DischargeCurrent[uc_ChannelNo];
                if(CCCV_TB_DV_PulseCount[uc_ChannelNo] >= 1) //用户不设置，则CCCV_TB不特殊打点  by zc 12/18/2018
                {
                    //前3ms每1ms打一个点，显示下降沿
                    if( CCCV_TB_PULSECOUNTER[uc_ChannelNo] >= CCCV_TB_DV_PulseCount[uc_ChannelNo] )
                    {
                        if(TimeTicks <= 39)
                        {
                            if(TimeTicks - 10 * ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] >= 10)
                            {
                                //SNDLOG
                                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = false;
                                SendLog(uc_ChannelNo);  //dirui 20170907
                                ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo]++;
                            }
                        }
                        else
                        {
                            ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] = 0;
                            ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                        }
                    }
                    else
                    {
                        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bIsDvInterval = true;
                    }
                }
            }
            break;
        }
        }
        *p_CtrlState=CCS_I;
    }
    break;
//================================================================================================
//       New Control Type for Samsung Turbo Boost Test 1A/1B    Add by zc 06.04.2018
//================================================================================================
    case CT_IVCH_CURRENT_TB:
    {
        //time
        unsigned long long TimeTicks;
        MsTime *tb_TimeBuffer = &ChannelInfo_Manager::CC_TB_TimeBuffer[uc_ChannelNo];
        TimeTicks = CommonFunc::GetTimeTicks(tb_TimeBuffer); //求系统时间和tb_TimeBuffer的时间差，即计算距离上一个状态过去的时间

        switch(ChannelCC_TBStatus[uc_ChannelNo])  //状态机
        {
        case STEP_CC_TB_START:  //起始步
        {
//=============从上位机取得所需控制参数：CC，DC，CC WIDTH，DC WIDTH======================================
            CC_TB_ChargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
                                                p_Ctrl->m_CtrlValues.m_fControlValue[0]);	//Charge Current Value

            CC_TB_DischargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                   p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                                                   p_Ctrl->m_CtrlValues.m_fControlValue[1]); //Discharge Current Value

            CC_TB_ChargeTime[uc_ChannelNo] = 10.0f * ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                             p_Ctrl->m_ucCtrlValue_DataType[2],p_Ctrl->m_CtrlValueOperator[2],p_Ctrl->m_CtrlValueCoefficients[2],
                                             p_Ctrl->m_CtrlValues.m_fControlValue[2]); //Charge Time Period

            CC_TB_DischargeTime[uc_ChannelNo] = 10.0f * ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                p_Ctrl->m_ucCtrlValue_DataType[3],p_Ctrl->m_CtrlValueOperator[3],p_Ctrl->m_CtrlValueCoefficients[3],
                                                p_Ctrl->m_CtrlValues.m_fControlValue[3]);	//Discharge Time Period

            ChannelCC_TBStatus[uc_ChannelNo] = STEP_CC_TB_DISCHARGE;  //先执行放电步
            //记录时间
            ChannelInfo_Manager::CC_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;
            //求系统时间和tb_TimeBuffer的时间差，即计算距离上一个状态过去的时间
            TimeTicks = CommonFunc::GetTimeTicks(tb_TimeBuffer);
        }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++  放电步  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        case STEP_CC_TB_DISCHARGE://Discharge Stage
        {
//===========================================放电时间已到，需要回到充电阶段=======================================
            if(TimeTicks >= CC_TB_DischargeTime[uc_ChannelNo])
            {
                ReturnFValue = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                               p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
                               p_Ctrl->m_CtrlValues.m_fControlValue[0]);  //从上位机取得充电电流值

                CC_TB_ChargeCurrent[uc_ChannelNo] = ReturnFValue;
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,true);

                p_CheckValue->fCtrlValue = ReturnFValue; //fCurrent;

                ChannelInfo_Manager::CC_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();//OS_Time;  //记录时间

                ChannelCC_TBStatus[uc_ChannelNo] = STEP_CC_TB_CHARGE; //回到充电阶段																						//comment out by zc 06/05/2018
                ChannelInfo_Manager::CC_TB_CVCounter[uc_ChannelNo] = 0;
            }
//==========================================放电脉冲阶段，输出放电脉冲电流值====================================
            else//discharge
            {
                ////判断DAC更新标志位，第一次进入才更新，之后不再更新，输出放电脉冲电流值
                ReturnFValue = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                               p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                               p_Ctrl->m_CtrlValues.m_fControlValue[1]);  //从上位机取得放电电流值

                CC_TB_DischargeCurrent[uc_ChannelNo] = ReturnFValue;
                Step_OutputCurrDac(uc_ChannelNo,CC_TB_DischargeCurrent[uc_ChannelNo],false); //输出放电电流
                p_CheckValue->fCtrlValue = CC_TB_DischargeCurrent[uc_ChannelNo];
            }
            break;
        }

//  进入CC阶段=================================================================
        case STEP_CC_TB_CHARGE://Charge stage
        {
//=====================CHARGE阶段时间已足够，进入放电模式===========================
            if(TimeTicks >= CC_TB_ChargeTime[uc_ChannelNo])
            {
                ChannelInfo_Manager::CC_TB_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();												//记录时间
//                 ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
                CC_TB_DischargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                       p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                                                       p_Ctrl->m_CtrlValues.m_fControlValue[1]); 																														//使用最新的放电电流
                Step_OutputCurrDac(uc_ChannelNo,CC_TB_DischargeCurrent[uc_ChannelNo],true);
                ChannelCC_TBStatus[uc_ChannelNo] = STEP_CC_TB_DISCHARGE;																									//进入放电脉冲阶段
                ChannelInfo_Manager::CC_TB_CVCounter[uc_ChannelNo] = 0;
            }
//====================CHARGE阶段未结束，继续输出给定电流==========================
            else //
            {
//                 ChannelInfo_Manager::RecoverFilter(uc_ChannelNo);

                CC_TB_ChargeCurrent[uc_ChannelNo] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                                                    p_Ctrl->m_ucCtrlValue_DataType[0],p_Ctrl->m_CtrlValueOperator[0],p_Ctrl->m_CtrlValueCoefficients[0],
                                                    p_Ctrl->m_CtrlValues.m_fControlValue[0]);																															//使用最新的放电电流
                Step_OutputCurrDac(uc_ChannelNo,CC_TB_ChargeCurrent[uc_ChannelNo],false);
                p_CheckValue->fCtrlValue = CC_TB_ChargeCurrent[uc_ChannelNo]; //fCurrent;
//================================END=============================================
            }
        }
        break;
        }
        *p_CtrlState=CCS_I;
        break;
    }

    case CT_IVCH_CRATE:
        if(b_CurCoarseTune[uc_ChannelNo] ==true)
        {
            if(!b_CurCoarseTuneInit[uc_ChannelNo])
            {
                for(unsigned char uc_Idx = uc_ChannelNo;
                        uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                        uc_Idx++)
                    ChannelInfo_Manager::SetFilterStatus(uc_Idx, false);
                b_CurCoarseTuneInit[uc_ChannelNo]=true;
                uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                b_CurCoarseTuneSampleFinish[uc_ChannelNo]=false;
                m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime=m_CurFineTuneFactor.m_fStartTime + ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime);
                if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
                {
                    m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime +=m_CurFineTuneFactor.m_fParaWaitTime;
                }
                f_set = Step_Manager_task::CalculateCRate(uc_ChannelNo);
                ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_set);
            }
            else
            {

                if(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime) > m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime)
                {
                    if(NewCodeRefreshed[uc_ChannelNo])
                    {
                        NewCodeRefreshed[uc_ChannelNo]=0;

                        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))  //dir 20190821
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
                        else
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fCurrent;

                        uc_CurCoarseTuneSampleCnt[uc_ChannelNo]++;
                        if(uc_CurCoarseTuneSampleCnt[uc_ChannelNo]>=SAMPLE_POINT)
                        {
                            uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                            b_CurCoarseTuneSampleFinish[uc_ChannelNo]=true;
                            Updata_Flag = 1;
                        }
                    }
                }
            }
        }
        else
        {
            if(!b_CC_RampStampFlag[uc_ChannelNo])
            {
                MsTime TimeDiff = CommonFunc::TimeDiff(uc_CC_RampStampTime[uc_ChannelNo]);
                if(TimeDiff.Us100>500)
                {
                    for(unsigned char uc_Idx = uc_ChannelNo;
                            uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                            uc_Idx++)
                        ChannelInfo_Manager::RecoverFilter(uc_Idx);
                    b_CC_RampStampFlag[uc_ChannelNo]=true;
                }
            }
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
            {
                if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 3.2 ms modified by Ron 080715
                    Updata_Flag = 1;
            }
            else
            {
                if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))		 //update dac every 100ms
                    Updata_Flag = 1;
            }
        }
        if(Updata_Flag)
        {
            if(b_CurCoarseTuneSampleFinish[uc_ChannelNo])
            {
                ReturnFValue=CalculateCRate(uc_ChannelNo);
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
                p_CheckValue->fCtrlValue = ReturnFValue;
            }
        }
        *p_CtrlState=CCS_I;
        break;
    case CT_IVCH_CRAMP:
        if(((DacOutputCounter >> 0) & 0x0F) == uc_ChannelNo)  //update dac every 1ms
        {
            ReturnFValue=CalculateCRamp(uc_ChannelNo);
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
            p_CheckValue->fCtrlValue = ReturnFValue;
            Updata_Flag = 1;
        }
        *p_CtrlState=CCS_I;
        break;
    case CT_IVCH_CSTAIRCASE:
        if(b_CurCoarseTune[uc_ChannelNo] ==true)
        {
            if(!b_CurCoarseTuneInit[uc_ChannelNo])
            {
                for(unsigned char uc_Idx = uc_ChannelNo;
                        uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                        uc_Idx++)
                    ChannelInfo_Manager::SetFilterStatus(uc_Idx, false);
                b_CurCoarseTuneInit[uc_ChannelNo]=true;
                uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                b_CurCoarseTuneSampleFinish[uc_ChannelNo]=false;
                m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime=m_CurFineTuneFactor.m_fStartTime + ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime);
                if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
                {
                    m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime +=m_CurFineTuneFactor.m_fParaWaitTime;
                }
                f_set = Step_Manager_task::CalculateCStaircase(uc_ChannelNo);
                ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_set);
            }
            else
            {
                if(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime) >m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime)
                {
                    if(NewCodeRefreshed[uc_ChannelNo])
                    {
                        NewCodeRefreshed[uc_ChannelNo]=0;

                        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))  //dir 20190821
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
                        else
                            f_CurCoarseTuneADValue[uc_ChannelNo][uc_CurCoarseTuneSampleCnt[uc_ChannelNo]]=ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fCurrent;

                        uc_CurCoarseTuneSampleCnt[uc_ChannelNo]++;
                        if(uc_CurCoarseTuneSampleCnt[uc_ChannelNo]>=SAMPLE_POINT)
                        {
                            uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
                            Updata_Flag = 1;
                            b_CurCoarseTuneSampleFinish[uc_ChannelNo]=true;
                        }
                    }
                }
            }
        }
        else
        {
            if(!b_CC_RampStampFlag[uc_ChannelNo])
            {
                MsTime TimeDiff = CommonFunc::TimeDiff(uc_CC_RampStampTime[uc_ChannelNo]);
                if(TimeDiff.Us100>500)
                {
                    for(unsigned char uc_Idx = uc_ChannelNo;
                            uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                            uc_Idx++)
                        ChannelInfo_Manager::RecoverFilter(uc_Idx);
                    b_CC_RampStampFlag[uc_ChannelNo]=true;
                }
            }
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
            {
                if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 3.2 ms modified by Ron 080715
                    Updata_Flag = 1;
            }
            else
            {
                if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))		 //update dac every 110ms
                    Updata_Flag = 1;
            }
        }
        ReturnFValue = CalculateCStaircase(uc_ChannelNo);
        if(!Updata_Flag)
        {
            if(p_CheckValue->fCtrlValue != ReturnFValue)
                Updata_Flag = 1;  // 20190829 yy
        }
        if(Updata_Flag)
        {
            if(b_CurCoarseTuneSampleFinish[uc_ChannelNo])
            {
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
                p_CheckValue->fCtrlValue = ReturnFValue;
            }
        }
        *p_CtrlState=CCS_I;
        break;
    case CT_IVCH_POWER:
    case CT_IVCH_LOAD:
    case CT_IVCH_PRAMP:
        if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)
        {
            ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput = 0;
            switch(p_Ctrl->m_u16ControlType)
            {
            case CT_IVCH_POWER:
                ReturnFValue=CalculatePower(uc_ChannelNo);
                break;
            case CT_IVCH_LOAD:
                ReturnFValue=CalculateLoad(uc_ChannelNo);
                break;
            case CT_IVCH_PRAMP:
                ReturnFValue=CalculatePowerRamp(uc_ChannelNo);
                break;
            default:
                return;
                //break;

            }
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
            Updata_Flag = 1;
        }
        *p_CtrlState=CCS_I;
        break;
    case CT_IVCH_SIMU_I:   //12.22.2013ADD
    {
        unsigned long long TimeTicks;
        St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
        MsTime *Simu_I_TimeBuffer = &SIMU_I_TimeBuffer[uc_ChannelNo];

        ReturnFValue= CalculateISimulation(uc_ChannelNo);                    //12.22.2013ADD

        if( ((SimInfo->m_fValue >= 0.0f) && (ReturnFValue <= 0.0f)) || ((SimInfo->m_fValue <= 0.0f) && (ReturnFValue >= 0.0f)) )
            ReturnFValue = 0.0f;


        TimeTicks = CommonFunc::GetTimeTicks(Simu_I_TimeBuffer);

        if( (TimeTicks >= 30) &&(bTimePassed[uc_ChannelNo] == 0) )
        {
            bAfterSimuLoaded[uc_ChannelNo] = 1;
            bTimePassed[uc_ChannelNo] = 1;
        }
        if(bAfterSimuLoaded[uc_ChannelNo])
            Calculate_Simu_IR(uc_ChannelNo);

        if(bNewSimuLoaded[uc_ChannelNo]) //simulation point changed 20160906 zyx changed
        {
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,true);                       //12.22.2013ADD
            bNewSimuLoaded[uc_ChannelNo] = 0;
//======================================================
            SIMU_I_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();
            bTimePassed[uc_ChannelNo] = 0;
//=======================================================
        }
        else
        {
            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
            {
                if((DacOutputCounter & 0x03) == uc_ChannelNo)  //update dac every 3.2 ms modified by Ron 080715
                    Updata_Flag = 1;
            }
            else
            {
                if((DacOutputCounter << 29 == 0)&&(((DacOutputCounter >> 3) & 0x0F) == uc_ChannelNo))		 //update dac every 100ms
                    Updata_Flag = 1;
            }
        }
        if(Updata_Flag)
        {
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);                       //12.22.2013ADD
            bNewSimuLoaded[uc_ChannelNo] = 0;
        }
        Updata_Flag = 1;
        SimuSamp_SendExtraLog(uc_ChannelNo);
        *p_CtrlState=CCS_I;			//12.22.2013ADD
        break;
    }
    case CT_IVCH_SIMU_P:  //12.22.2013ADD
    case CT_IVCH_SIMU_L:  //12.22.2013ADD
        if(p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_P)
        {
            unsigned long long TimeTicks;
            St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
            MsTime *Simu_I_TimeBuffer = &SIMU_I_TimeBuffer[uc_ChannelNo];
            ReturnFValue= CalculatePSimulation(uc_ChannelNo);                    //12.22.2013ADD

            if( ((SimInfo->m_fValue >= 0.0f) && (ReturnFValue <= 0.0f)) || ((SimInfo->m_fValue <= 0.0f) && (ReturnFValue >= 0.0f)) )
                ReturnFValue = 0.0f;
            TimeTicks = CommonFunc::GetTimeTicks(Simu_I_TimeBuffer);

            if( (TimeTicks >= 30) &&(bTimePassed[uc_ChannelNo] == 0) )
            {
                bAfterSimuLoaded[uc_ChannelNo] = 1;
                bTimePassed[uc_ChannelNo] = 1;
            }
            if(bAfterSimuLoaded[uc_ChannelNo])
                Calculate_Simu_IR(uc_ChannelNo);
        }
        else
            ReturnFValue= CalculateLSimulation(uc_ChannelNo);                    //12.22.2013ADD
        Updata_Flag = 1;
        if(bNewSimuLoaded[uc_ChannelNo])    //simulation point changed
        {
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,true);                       //12.22.2013ADD
            bNewSimuLoaded[uc_ChannelNo] = 0;

            SIMU_I_TimeBuffer[uc_ChannelNo] = CommonFunc::GetSystemTime();  //开放simulation IR测试  by ZC 01.04.2018
            bTimePassed[uc_ChannelNo] = 0;
        }
        else if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)    //or voltage sampled
        {
            Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);                       //12.22.2013ADD
        }
        SimuSamp_SendExtraLog(uc_ChannelNo);
        *p_CtrlState=CCS_I;			//12.22.2013ADD
        break;
    case CT_IVCH_C_CV:
    case CT_IVCH_V_CV:
        if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)
        {   ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput =0;

            ReturnFValue=CalculateCV(uc_ChannelNo);
            if(p_Ctrl->m_u16ControlType == CT_IVCH_C_CV)
                Step_OutputCurrDac(uc_ChannelNo,ReturnFValue,false);
            else
                Step_OutputVoltDac(uc_ChannelNo,ReturnFValue,false);
            p_CheckValue->fCtrlValue = ReturnFValue;
            Updata_Flag = 1;
        }

        *p_CtrlState=CCS_I;
        if(p_Ctrl->m_u16ControlType == CT_IVCH_V_CV)
        {
            if(!p_Info->bDigitalV)
                *p_CtrlState = CCS_V;
        }
        break;
    case CT_IVCH_VRAMP:
    case CT_IVCH_VSTAIRCASE:

        if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput)
        {
            ChannelInfo_Manager::Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.DacOutput = 0;
            if(p_Ctrl->m_u16ControlType == CT_IVCH_VRAMP)
                ReturnFValue = CalculateVRamp(uc_ChannelNo);
            else
                ReturnFValue = CalculateVStaircase(uc_ChannelNo);
            Step_OutputVoltDac(uc_ChannelNo, ReturnFValue, false);
            Updata_Flag = 1;
        }
        if(p_Info->bDigitalV)
            *p_CtrlState = CCS_I;
        else
            *p_CtrlState = CCS_V;
        break;
    case CT_IVCH_IR:
    case CT_IVCH_ACIR:
        IR::ProcessIR(uc_ChannelNo);
        break;
    case CT_SPECIAL_IRTEST:
        Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo] = 1;
        IR::ProcessIR(uc_ChannelNo);
        break;
    //我们把没有支持，或者说准备以后再支持的控制类型，都放到本注释以下，以示区分：
    //we put control types,witch not supported or fullfill in the future,in the following for distinguish:
    case CT_IVCH_SIMU_V:  //12.22.2013ADD	  //We Do Not Offer Voltage Simulation
        if((((DacOutputCounter >> 0) & 0x0F)) == uc_ChannelNo)  //update dac every 10ms    //12.22.2013ADD
        {
            p_Info->bNeedDacOutput = 1;  //12.22.2013ADD
        }
        *p_CtrlState = CCS_I; //12.22.2013ADD
        break;
    case CT_IVCH_DIGIPOWER:
    case CT_IVCH_DIGILOAD:
    case CT_IVCH_ACS:            //put control type together which r not supported yet.
    case CT_TYPE_CAN_SEND_MSG:
    case CT_IVCH_WRITECANOUTBOUND: //190617,hpq
    default:
        break;
    }
#if (SCH_STATUS_REPORT_ENABLE  == 1)
    if(Updata_Flag)
        ScheduleStatusReport::m_StepCtrlValue[uc_ChannelNo] = ReturnFValue;
#endif
}

float Step_Manager_task::Current_FineTune(unsigned char uc_ChannelNo, float fValue,unsigned char range)
{
    float   m_fMaxError = 0;
    float   f_adjust  = 0;
    float 	New_Output = 0;
    St_CurFineTune * p_FT_Info = &ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo];

    p_FT_Info->m_OutputInterval = CommonFunc::GetSystemTime();
    p_FT_Info->m_SampleValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
    p_FT_Info->m_Err = p_FT_Info->m_LastSetValue - p_FT_Info->m_SampleValue ;
    p_FT_Info->m_LastSetValue = fValue;
    m_fMaxError=ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(uc_ChannelNo,range)
                *(ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)-ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter)*0.015f;  //????3%??????1.5%
    if ((abs(ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_Err)> p_FT_Info->m_fMinError)  \
            && (abs(ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_Err)<m_fMaxError))//precision of the channel
    {
        if (ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_Err > 0)
            f_adjust = p_FT_Info->m_fStep;  // less than 1/16 of the range
        else
            f_adjust = -p_FT_Info->m_fStep;

        ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust += f_adjust;

        if(abs(ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust) >= p_FT_Info->m_fMaxAdjust)
            ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust =
                (ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust>0? p_FT_Info->m_fMaxAdjust:-p_FT_Info->m_fMaxAdjust);

        New_Output = fValue+ ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust;
        ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_LastOutput = New_Output;
    }
    else
    {
        New_Output = fValue+ ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust;
        ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_LastOutput = New_Output;
    }

    return New_Output;
}
void Step_Manager_task::Step_OutputCurrDac(unsigned char uc_ChannelNo, float fValue, bool IsKeyPiont)
{
    unsigned long Code = 0;
    unsigned char uc_MChannelCount_Flag = 0;
    St_CurFineTune * p_FT_Info = &ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo];
    fValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, fValue);	//170719 zyx

    float   f_ParaMod_temp = fValue;
//     float   fValue_tmp = fValue;
    float   New_Output = fValue;
    float   f_adjust  = fValue;
    float   m_fMaxError = 0;
# if(ENABLE_FINE_TUNE == 1)
    if((ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime) >m_CurFineTuneFactor.m_f_CurCoarseTuneStartTime) &&  (!bCCCV_Initiated[uc_ChannelNo]))//Add by DKQ on 06.04.2014 do not do fine tune within 0.1 second
    {

        p_FT_Info->m_SampleValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
        p_FT_Info->m_Err = p_FT_Info->m_LastSetValue - p_FT_Info->m_SampleValue ;
        p_FT_Info->m_LastSetValue = fValue;

        unsigned char range=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
        if(range >= ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_ucIRangeCount)
            range = ChannelInfo_Manager::CurrRngPhysicalRange1;//170320,add by hepeiqing

        if((b_CurCoarseTune[uc_ChannelNo]) &&( ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus==STEPRUNNING))
        {
            IsKeyPiont = true;
            uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
            b_CurCoarseTune[uc_ChannelNo]=false;
            b_CurCoarseTuneInit[uc_ChannelNo]=false;
            float MaxValue=-10000;
            float MinValue=10000;
            float   sum = 0;
            for (unsigned char i=0; i<SAMPLE_POINT; i++)
            {
                if(MaxValue<f_CurCoarseTuneADValue[uc_ChannelNo][i])
                    MaxValue=f_CurCoarseTuneADValue[uc_ChannelNo][i];
                if(MinValue>f_CurCoarseTuneADValue[uc_ChannelNo][i])
                    MinValue=f_CurCoarseTuneADValue[uc_ChannelNo][i];
                sum += f_CurCoarseTuneADValue[uc_ChannelNo][i];
            }
            sum =sum -MaxValue-MinValue;
            p_FT_Info->m_SampleValue = sum/(SAMPLE_POINT-2);
            f_CurCoarseTuneADValue[uc_ChannelNo][4] =p_FT_Info->m_SampleValue;//qjm 200122
            m_fMaxError = ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(uc_ChannelNo,range)
                          *(ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)-ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter)*0.01f;  //
            p_FT_Info->m_Err = ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue -p_FT_Info->m_SampleValue ;
            if(p_FT_Info->m_Err>m_fMaxError)   // 20200604
                p_FT_Info->m_Err=m_fMaxError;
            else if(p_FT_Info->m_Err<-m_fMaxError)
                p_FT_Info->m_Err=-m_fMaxError;
            p_FT_Info->m_TotalAdjust =p_FT_Info->m_Err;
            p_FT_Info->m_LastSetValue = ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue;//fValue;
            p_FT_Info->m_OutputInterval = CommonFunc::GetSystemTime();
            New_Output =  ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fSetValue + p_FT_Info->m_Err;  //fValue
//             fValue_tmp = New_Output;
            ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_LastOutput = New_Output;
            Step_Manager_task::DacOutputCounter = 0;
            b_CC_RampStampFlag[uc_ChannelNo]=false;
            b_CurCoarseTuneFinished[uc_ChannelNo]=true;
            uc_CC_RampStampTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
        }
        else
        {

            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag == false)	//需要微调
            {
                b_FineTuneFinished[uc_ChannelNo]=true;
                if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bDynamicControl)
                {
                    if(CommonFunc::GetTimeTicks(&p_FT_Info->m_OutputInterval) > FINE_TUNE_INTERVAL)	//100ms
                    {
                        p_FT_Info->m_OutputInterval = CommonFunc::GetSystemTime();
                        New_Output = Current_FineTune(uc_ChannelNo,fValue,range);	//微调
                    }
                    else
                    {
                        New_Output = fValue+ ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust;
                    }

                }
                else
                {
                    New_Output = Current_FineTune(uc_ChannelNo,fValue,range);//微调

                }
            }
            else
            {
                New_Output = fValue+ ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_TotalAdjust;

            }
            ChannelInfo_Manager::m_CurFineTune_Info[ uc_ChannelNo ].m_LastOutput = New_Output;

        }


        ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_LastOutput_1B =ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,  New_Output) ;	// 将微调值更新为PID初始值， 09.09.2015//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,

        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)) //TODO: need to do anything for intra unit parallel here??
        {
//             if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag)
//                 New_Output = fValue_tmp;
            New_Output /= ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo);
            f_ParaMod_temp = New_Output;
            uc_MChannelCount_Flag = 1;
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)&& ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
        {
//             if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag)
//                 New_Output = fValue_tmp;
            float IntraChannelValue,InterChannelValue;
            IntraChannelValue=(New_Output-fValue)/ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize    \
                              +fValue/(ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)-ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter);   //20190228 dirui
            InterChannelValue=fValue;
            CANBusParse::Parallel_MasterSend_UpdateOutput(InterChannelValue,IsKeyPiont);
            f_ParaMod_temp = IntraChannelValue;
            uc_MChannelCount_Flag = 1;
        }
        else
        {
//             if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag)
//             {
//                 New_Output = fValue_tmp;//20190809 dyy
//             }
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,m_Range[uc_ChannelNo],New_Output);
            StepDAC_Out(uc_ChannelNo,Code,ACTIVE_DAC_CUR);
        }
    }
    else
# endif
    {
        ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_LastOutput_1B =ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo, fValue) ;	// 将微调值更新位PID初始值， 09.09.2015//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,

        m_FSet[uc_ChannelNo] = fValue;    //20131010 chen add
        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)) //TODO: need to do anything for intra unit parallel here??
        {
            fValue /= ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo);
            f_ParaMod_temp = fValue;
            uc_MChannelCount_Flag = 1;
#if( USEDSETPARALVALUE==1)
            ChannelInfo_Manager::RecordRefreshValueForParal[uc_ChannelNo]=fValue;
#endif
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
        {
            if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
            {
                float IntraChannelValue,InterChannelValue;
                IntraChannelValue=fValue /(ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)-ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter);
                InterChannelValue=fValue;

                CANBusParse::Parallel_MasterSend_UpdateOutput(InterChannelValue,IsKeyPiont);
                f_ParaMod_temp = IntraChannelValue;
                uc_MChannelCount_Flag = 1;
            }
            else
            {
                ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_LastOutput_1B =ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo, fValue) ;	// 将微调值更新位PID初始值， 09.09.2015//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,

                Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,m_Range[uc_ChannelNo],fValue);
                StepDAC_Out(uc_ChannelNo,Code,ACTIVE_DAC_CUR);
            }
        }
        else
        {
//             if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag && ChannelInfo_Manager::IsSingleChanMode(uc_ChannelNo))//20190806 dyy
//             {
//                 fValue = fValue_tmp;//20190809 dyy
//             }
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,m_Range[uc_ChannelNo],fValue);
            StepDAC_Out(uc_ChannelNo,Code,ACTIVE_DAC_CUR);
        }
        p_FT_Info->m_LastOutput = fValue;
        p_FT_Info->m_LastSetValue = fValue;
        p_FT_Info->m_TotalAdjust = 0;
        p_FT_Info->m_OutputInterval = CommonFunc::GetSystemTime();
    }

    if(uc_MChannelCount_Flag)
    {
        unsigned char uc_ChannelCount;
        uc_ChannelCount=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
        for(unsigned char uc_i=uc_ChannelNo; uc_i<(uc_ChannelNo+uc_ChannelCount); uc_i++)
        {
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_i,m_Range[uc_ChannelNo],f_ParaMod_temp);	//master channel
            StepDAC_Out(uc_i,Code,ACTIVE_DAC_CUR);
        }
#if( USEDSETPARALVALUE==1)
        ChannelInfo_Manager::RecordRefreshValueForParal[uc_ChannelNo]=fValue;
#endif
    }
}

void Step_Manager_task::Step_OutputCurrDac_Submaster(unsigned char uc_ChannelNo,float fValue)
{
    unsigned long Code;
    unsigned char uc_sum = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;   // 20161021    yangy

    //no DigitalI ctrl.
    for(unsigned char uc_i=uc_ChannelNo; uc_i<uc_sum; uc_i++)
    {
        if(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_i,MetaCode_PV_Steptime) >m_CurFineTuneFactor.m_fStartTime) //Add by DKQ on 06.04.2014 do not do fine tune within 0.1 second
        {
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_SampleValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_i,MetaCode_PV_Current);
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_Err = ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_LastSetValue - ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_SampleValue ;
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_LastSetValue = fValue;

            float   New_Output,f_adjust;

            if (abs(ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_Err) > ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fMinError)  //precision of the channel
            {
                if (ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_Err > 0)
                    f_adjust = ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fStep;  // less thant 1/16 of the range
                else
                    f_adjust = -ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fStep;

                ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_TotalAdjust += f_adjust;

                if(abs(ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_TotalAdjust) >= ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fMaxAdjust)
                    ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_TotalAdjust =
                        (ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_TotalAdjust > 0 ? ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fMaxAdjust : -ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_fMaxAdjust);

                New_Output = fValue + ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_TotalAdjust;
                ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_LastOutput = New_Output;
            }
            else
                New_Output = ChannelInfo_Manager::m_CurFineTune_Info[ uc_i ].m_LastOutput;
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_i, m_Range[uc_ChannelNo], New_Output);
            StepDAC_Out(uc_i, Code, ACTIVE_DAC_CUR);   // 20161021    yangy
        }
        else
        {
            m_FSet[uc_i] = fValue;    //20131010 chen add

            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_i, m_Range[uc_ChannelNo], fValue);	 //SumMaster channel Range be valid
            StepDAC_Out(uc_i, Code, ACTIVE_DAC_CUR);   // 20161021    yangy
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_LastOutput = fValue;
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_LastSetValue = fValue;
            ChannelInfo_Manager::m_CurFineTune_Info[uc_i].m_TotalAdjust = 0;

        }
    }
}

void Step_Manager_task::Step_OutputVoltDac(unsigned char uc_ChannelNo, float fValue, bool IsKeyPiont)
{
    unsigned long Code  = 0;  //  20130927 何
    unsigned char uc_MChannelCount_Flag = 0;
    float      f_ParaMod_temp = fValue;
    float Current_for_Slaves = 0; // add for Battery simulatior DKQ   // 03.28.2017 for PWM并联恒压改为硬件PID
//=====================================================BY ZC 08.11.2017================================
    unsigned char ActiveIdx;
    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
//=======================================================END======================================================
    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
    {
        if(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
        {   //Parallel mode
            unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&VoltOutputInterval[uc_ChannelNo]);
            if(TimeDiff < PARALLEL_PID_INTERVAL)
                return;
            else
            {
                CommonFunc::GetTimeTicksUpdate(&VoltOutputInterval[uc_ChannelNo]);
            }

        }

        ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_SetValue_1B = fValue;
        ChannelInfo_Manager::PrepareIBasedPID(uc_ChannelNo, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange);

        float      fKp, fKi, fKd, Output_1B;
        // float  I_Max = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange].m_Bound.m_High;

        ST_IR_Info* p_IR_Info = &ChannelInfo_Manager::m_IR_Info[uc_ChannelNo];
        p_IR_Info->m_IR_Confirmed = true;

        float  IR_Value = Get_IR_Value(uc_ChannelNo);

        if(IR_Value <= 0)
            p_IR_Info->m_IR_Confirmed = false;

        if (p_IR_Info->m_IR_Confirmed)
        {
            fKp = 0;
            fKi = 0.5 * board_config::DacInfo.m_fPID_Coefficient /(IR_Value);//0.01 *
            fKd = 0;
        }
        else
        {
            St_PID * p_PID = &ChannelInfo_Manager::ChannelPID[uc_ChannelNo].m_VoltagePID[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange];
            fKp = p_PID->m_fKp;
            fKi = p_PID->m_fKi;
            fKd = p_PID->m_fKd; // 测试用，暂时屏蔽， 09.09.2015
        }
        ChannelInfo_Manager::m_RC_HistoryCurrentFilterValue[uc_ChannelNo]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
        ChannelInfo_Manager::m_RC_HistoryVoltageFilterValue[uc_ChannelNo]=ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;

        Output_1B = Pid_Ctrl::IBased_PID_Output_Calculation( uc_ChannelNo, fKp, fKi, fKd, & (ChannelInfo_Manager::m_Pid_Coef[ uc_ChannelNo])  );
        fValue  = ChannelInfo_Manager::DeUnifyCurrent( uc_ChannelNo, Output_1B);//ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange,
        fValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,fValue);	//170719 zyx

        if(p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_TB)
        {
            float fCCCVmaxCurrent = ::abs(CCCV_TB_ChargeCurrent[uc_ChannelNo]);
            if(fValue > fCCCVmaxCurrent)
                fValue = fCCCVmaxCurrent;
            else if(fValue < -fCCCVmaxCurrent)
                fValue = -fCCCVmaxCurrent;
        }
        else if(p_Ctrl->m_u16ControlType == CT_IVCH_CCCV || p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_WRM)
        {
            float fCCCVmaxCurrent = ::abs(CCCV_TargetCurrent[uc_ChannelNo]);
            if(fValue > fCCCVmaxCurrent)
                fValue = fCCCVmaxCurrent;
            else if(fValue < -fCCCVmaxCurrent)
                fValue = -fCCCVmaxCurrent;
        }
        m_FSet[uc_ChannelNo] = fValue;    //20131010 chen add

        ChannelInfo_Manager::m_DataFreshForPID[uc_ChannelNo]    =  false;

        ChannelInfo_Manager::CCCV_TB_CurBuffer[uc_ChannelNo] = fValue;	//09.07.2018 for Turbo Boost, store for next CV by ZC
        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)) //TODO: need to do anything for intra unit parallel here??
        {
            fValue /= ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo);
            f_ParaMod_temp = fValue;
            uc_MChannelCount_Flag = 1;
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)&& ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
        {
            float IntraChannelValue,InterChannelValue;
            IntraChannelValue=fValue /(ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)-ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter);
            InterChannelValue=fValue;
            CANBusParse::Parallel_MasterSend_UpdateOutput(InterChannelValue,IsKeyPiont);
            f_ParaMod_temp = IntraChannelValue;
            uc_MChannelCount_Flag = 1;
        }
        else
        {
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,m_Range[uc_ChannelNo],fValue);	//master channel
            StepDAC_Out(uc_ChannelNo,Code,ACTIVE_DAC_CUR);
        }
    }
    else    //have hardware voltage dac
    {
        Code = ChannelInfo_Manager::CalibrateVoltageDAC(uc_ChannelNo,ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange,fValue);

        StepDAC_Out(uc_ChannelNo,Code,ACTIVE_DAC_VLT);
        {
            Current_for_Slaves = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;

            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo)&& ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
            {

                CANBusParse::Parallel_MasterSend_UpdateOutput(Current_for_Slaves,IsKeyPiont);

            }
        }
    }

    if(uc_MChannelCount_Flag)
    {
        unsigned char uc_ChannelCount;
        uc_ChannelCount=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
        for(unsigned char uc_i=uc_ChannelNo; uc_i<(uc_ChannelNo+uc_ChannelCount); uc_i++)
        {
            Code = ChannelInfo_Manager::CalibrateCurrentDAC(uc_i,m_Range[uc_ChannelNo],f_ParaMod_temp);	//master channel
            StepDAC_Out(uc_i,Code,ACTIVE_DAC_CUR);
        }
#if( USEDSETPARALVALUE==1)
        ChannelInfo_Manager::RecordRefreshValueForParal[uc_ChannelNo]=fValue;
#endif
    }
}

/*****************************************************
// Function Name: Step_WaitRoutine
// Version:     V1.0
// Date:                 2012/11/12
// Description:
//                  WAIT状态的瞬态响应
// Others:
//                  进入此状态时，启动超时机制，如超时则设定引擎任务为停止，
//                 并设定TRANSITION状态后的目的状态为IDLE，转入TRANSITION状态
// History：
//
// ******************************************************/
void Step_Manager_task::Step_WaitRoutine(unsigned char uc_ChannelNo)
{
    if (StepTimeTrigger[uc_ChannelNo]==0)
    {
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].SysTimeStepTask = CommonFunc::GetSystemTime();

        StepTimeTrigger[uc_ChannelNo]=1;                      //Schedule计时标记
    }

    unsigned long long TimeTicks;
    MsTime MyMsTime;
    MyMsTime.Second = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].SysTimeStepTask.Second;
    MyMsTime.Us100 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].SysTimeStepTask.Us100;
    TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);


    if(StepTimeTrigger[uc_ChannelNo] == 1 && TimeTicks >= uw32_OverTime)       //超时？
    {
        StepTimeTrigger[uc_ChannelNo] = 0;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPTRANSITION;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepTransationToStatus = STEPIDLE; //目标状态
        PrepareForStop(uc_ChannelNo);
    }
}

/*****************************************************
// Function Name: Get_StepRunInfo
// Version:      V1.0
// Date:                  2012/11/12
// Description:
//                  提取Step中的相关信息
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::Get_StepRunInfo(unsigned char uc_ChannelNo, St_StepInfo ChannelStep)
{
    GetStepRunInfo[uc_ChannelNo].GetStepInfo = ChannelStep;
}

/*****************************************************
// Function Name: ClearStepMessage
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   清除Step响应完毕的信息
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task:: ClearStepMessage(St_DoStepMessage *Message)
{
    Message->Used = 0;
    Message->StepMessage.MsgNumber = 0;
}

unsigned char Step_Manager_task::Check_PresentCtrlType(unsigned char uc_ChannelNo)
{
    unsigned char uc_CtrlState = CCS_REST;
    unsigned char uc_CtrlType = CT_REST;
    uc_CtrlType = OS_LogicProcess::m_RunningTask[uc_ChannelNo].logicProcessMSg.TargetCtrlType;
    switch(uc_CtrlType)
    {
    case CT_CURRENT:
        uc_CtrlState = CCS_I;
        break;
    case CT_VOLTAGE:
        uc_CtrlState = CCS_V;
        break;
    case CT_REST:
        uc_CtrlState = CCS_REST;
        break;
    default:
        uc_CtrlState = CCS_OFF;
        break;
    }

    return uc_CtrlState;

}
/*****************************************************
// Function Name: PrepareForStop
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   准备调用引擎进行停止操作
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task:: PrepareForStop(unsigned char uc_ChannelNo)
{
    safety_check::InitCtrlValue(uc_ChannelNo, CtrlValueCheck_Null, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::CurrRngPhysicalRange1);
    unsigned char uc_CtrlState = Check_PresentCtrlType(uc_ChannelNo);   // 20200810 yy
    //switch(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState)   //调用相关的关通道时序
    switch(uc_CtrlState)   //调用相关的关通道时序
    {
    case CCS_I:
        StepDisposableMSG[uc_ChannelNo].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr;    //临时调用IBT测试以后要统一到OS_LogicProcess::SetCtrlVltToOff_ptr;
        StepDisposableMSG[uc_ChannelNo].Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
        StepDisposableMSG[uc_ChannelNo].Disposable_Flag = DISPOSABLE_to_CLOSECHN;
        break;
    case CCS_V:
        StepDisposableMSG[uc_ChannelNo].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
        StepDisposableMSG[uc_ChannelNo].Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
        StepDisposableMSG[uc_ChannelNo].Disposable_Flag = DISPOSABLE_to_CLOSECHN;
        break;
    case CCS_EXTERNAL:
        break;
    case CCS_REST:
    case CCS_OFF:
    case CCS_OTHER:  //20150907,fix: huawei code 0 unsafe bug
    default:
        StepDisposableMSG[uc_ChannelNo].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
        StepDisposableMSG[uc_ChannelNo].Disposable_task.stepLen = OS_LogicProcess::SetCtrlRstToOff.stepLen;
        StepDisposableMSG[uc_ChannelNo].Disposable_Flag = DISPOSABLE_to_CLOSECHN;
        break;
    }
    StepDisposableMSG[uc_ChannelNo].TargetCtrlType = CT_OFF;  // 20200810 yy
    if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        //set up slave channel's ctrl type, current range, control value:
        if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) ) //interunit master channel set ctrl type and ctrl value
        {
            OS_LogicProcess::LogicStatus[uc_ChannelNo] = LOGIC_IDLE; //dirui 2017/07/04
            OS_LogicProcess::LogicCtrlType = DO_STOP;
            CANBusParse::Parallel_MasterSend_Stop();
            SetChnSampleFilter(uc_ChannelNo);
            OS_LogicProcess::m_bSlaveActive[uc_ChannelNo] = 0;
        }
        Step_Manager_task::SetDisposabletask(uc_ChannelNo, CCS_OFF);
    }
    //#endif
}

/*****************************************************
// Function Name: PrepareForRun
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   准备调用引擎进行运行操作
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::PrepareForRun(unsigned char uc_ChannelNo)
{
    safety_check::InitCtrlValue(uc_ChannelNo, CtrlValueCheck_Null, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::CurrRngPhysicalRange1);
    unsigned char uc_CT_Type = CT_REST;
    unsigned char uc_CtrlState = CCS_REST;
    unsigned char uc_Check_Type = CtrlValueCheck_Null;
    b_CurCoarseTune[uc_ChannelNo] =false; //zyx 210419
    switch(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType)       //根据控制类型确认调用的开通道时序
    {
    case CT_IVCH_EXTCHARGE:   //External Charge输出时暂时不对通道操作 保持Rest
        bExtChargeFlag[uc_ChannelNo] = 1;
        for(unsigned char i = 0; i < 4; i++)
        {
            bEC_CalEnergyFlag[uc_ChannelNo][i] = 1;
        }
        CANBusParse::IV_SendExtChargeOut(uc_ChannelNo, 1);
        break;
    case CT_TYPE_DO_SETTING:   //DO 输出时暂时不对通道操作 保持Rest
    case CT_IVCH_REST:
    case CT_IVCH_SETVARIABLE: //180503 zyx  //mits8
    case CT_IVCH_SETVALUE:
        break;
    case CT_IVCH_PAUSE:
        break;
    case CT_IVCH_CRATE:
    case CT_IVCH_CURRENT:
        b_CurCoarseTune[uc_ChannelNo] =true;
        b_CurCoarseTuneInit[uc_ChannelNo]=false;
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
    case CT_IVCH_CCCV_TB: //TURBO BOOST BY ZC 07.07.2017
    case CT_IVCH_CURRENT_TB:  //CC TB BY ZC 06.04.2018
        uc_CT_Type = CT_CURRENT;
        uc_Check_Type = CtrlValueCheck_Current;
        uc_CtrlState = CCS_I;
        break;
    case CT_IVCH_CSTAIRCASE:
        float fCurCoarseTuneStarttime = m_CurFineTuneFactor.m_fStartTime;
        if(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
            fCurCoarseTuneStarttime += m_CurFineTuneFactor.m_fParaWaitTime;

        if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[2] \
                > (fCurCoarseTuneStarttime + (0.7*ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount*SAMPLE_POINT)/1000))
        {
            b_CurCoarseTune[uc_ChannelNo] =true;
            b_CurCoarseTuneInit[uc_ChannelNo]=false;

        }
        else
        {
            b_CurCoarseTune[uc_ChannelNo] =false;
            b_CurCoarseTuneSampleFinish[uc_ChannelNo] = true;
        }
    case CT_IVCH_CRAMP:
    case CT_IVCH_C_CV:
    case CT_IVCH_LOAD:
    case CT_IVCH_PRAMP:
    case CT_IVCH_SIMU_I:  //12.22.2013ADD
    case CT_IVCH_SIMU_V: //12.22.2013ADD  //We Do Not Offer Voltage Simulation
    case CT_IVCH_SIMU_P:  //12.22.2013ADD
    case CT_IVCH_SIMU_L: //12.22.2013ADD
    case CT_IVCH_DIGII:
    case CT_IVCH_ACS:
    case CT_IVCH_IR:
    case CT_IVCH_ACIR:
    case CT_SPECIAL_IRTEST:                        //IR异常特殊处理
    case CT_IVCH_CPULSE:						//电流脉冲处理
        uc_CT_Type = CT_CURRENT;
        uc_CtrlState = CCS_I;
        break;
    case CT_IVCH_POWER:
        uc_CT_Type = CT_CURRENT;
        uc_Check_Type = CtrlValueCheck_Power;
        uc_CtrlState = CCS_I;
        break;
    case CT_IVCH_VOLTAGE:     //04.23.2013
    case CT_IVCH_V_CV:     //20150908
    case CT_IVCH_SIMU_Battery:  //190218 zyx
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
//                 || ((ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[uc_ChannelNo].m_UnitGroupInfo.m_WholeGroupSize>1)&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_PWM)))//并联时只允许PWM机器主通道运行模拟电压
        {
            uc_CT_Type = CT_CURRENT;
            uc_CtrlState = CCS_I;
        }
        else
        {
            uc_CT_Type = CT_VOLTAGE;
            uc_CtrlState = CCS_V;
        }
        uc_Check_Type = CtrlValueCheck_Voltage;
        break;
    case CT_IVCH_VRAMP:
    case CT_IVCH_VSTAIRCASE:
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
        {
            uc_CT_Type = CT_CURRENT;
            uc_CtrlState = CCS_I;
        }
        else
        {
            uc_CT_Type = CT_VOLTAGE;
            uc_CtrlState = CCS_V;
        }
        break;
    case CT_IVCH_DIGIV:      //04.23.2013
        uc_CT_Type = CT_CURRENT;
        uc_Check_Type = CtrlValueCheck_Voltage;
        uc_CtrlState = CCS_I;
        break;
    case CT_TYPE_CAN_SEND_MSG:
        ChannelInfo_Manager::SendCANMessage(uc_ChannelNo);
        break;
    case CT_IVCH_WRITECANOUTBOUND://190617,hpq
        ChannelInfo_Manager::SendCANMessage_Outbound(uc_ChannelNo);
        break;
    default:
        uc_CT_Type = CT_OFF;
        uc_CtrlState = CCS_OFF;
        break;
    }

    SetDisposabletask(uc_ChannelNo, uc_CT_Type);
    if(uc_Check_Type != CtrlValueCheck_Null)
        safety_check::InitCtrlValue(uc_ChannelNo, uc_Check_Type,
                                    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange,
                                    GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange);
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = uc_CtrlState;
    if ( !ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() && ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
    {
        SetIntraUnitParallelSlaveChannelCtrlState(uc_ChannelNo, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState);
    }
    StepDisposableMSG[uc_ChannelNo].msgID = STEP_TASK;
    StepDisposableMSG[uc_ChannelNo].CH_Number = uc_ChannelNo;

    PrepareforCtrlRoutine(uc_ChannelNo);    //进入Step瞬间的控制过程
}
float Step_Manager_task::CalculateGroupChanCtrlValue(unsigned short uChanIdx, float fSet, unsigned short uCtrlType)
{
    float fValue = fSet;
    if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        return fSet;
    }
    else   //for parallel mode
    {
        if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uChanIdx))
        {
            unsigned short uChanCount = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupID ].m_UnitGroupInfo.m_WholeGroupSize
                                        - ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter; //ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupingInfo.m_uGroupChanCount;

            switch (uCtrlType)
            {
            case CT_IVCH_CURRENT:
            case CT_IVCH_CURRENT_TB:  //by zc 09.04.2018
            case CT_IVCH_CCCV_TB:			//by zc 09.04.2018
            case CT_IVCH_CRAMP:
            case CT_IVCH_CRATE:
            case CT_IVCH_CSTAIRCASE:
            case CT_IVCH_POWER:
            case CT_IVCH_LOAD:
            case CT_IVCH_PRAMP:
            case CT_IVCH_SIMU_I:
            case CT_IVCH_SIMU_P:
            case CT_IVCH_SIMU_L:
            case CT_IVCH_SIMU_Battery:  //190218 zyx
            case CT_IVCH_DIGII:
            case CT_IVCH_VOLTAGE:
            case CT_IVCH_DIGIV:
            case CT_IVCH_V_CV:
            case CT_IVCH_C_CV:
            case CT_IVCH_VRAMP:
            case CT_IVCH_VSTAIRCASE:
                fValue = fSet / uChanCount;
                break;
            default:
                fValue = 0;
                break;
            }

            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitParaMode(uChanIdx)) //inter unit parallel mode
            {
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = fValue;
                return fValue;
            }
            else //intra unit parallel mode
            {
                //TO CHECK:
                unsigned short udx;
                for (udx = uChanIdx; udx <= ChannelInfo_Manager::FindGroupLastChanIdx(uChanIdx); udx ++ )
                {
                    ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupCtrl.m_fGroupChanCtrlValue[uChanIdx] = fValue;
                    ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupCtrl.m_uGroupChanCtrlType[uChanIdx] = CT_IVCH_CURRENT;
                }
                return fValue;
            }
        }
        else
        {
            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitParaMode(uChanIdx))  //inter unit parallel mode
            {
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = 0.0f;
                ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_REST;
                fValue = fSet;
            }
            else //intra unit parallel mode
            {
                //TO CHECK:
                unsigned short udx;
                for (udx = uChanIdx; udx <= ChannelInfo_Manager::FindGroupLastChanIdx(uChanIdx); udx ++ )
                {
                    ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupCtrl.m_fGroupChanCtrlValue[udx] = 0.0f;
                    ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupCtrl.m_uGroupChanCtrlType[udx] = CT_IVCH_REST;
                }
            }
        }
    }
    return fValue;
    //#endif
    //end 09.18
}

/*****************************************************
// Function Name: PrepareforCtrlRoutine
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                  进入Step瞬间的控制过程
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::PrepareforCtrlRoutine(unsigned char uc_ChannelNo)
{
    float f_Set=0;  //dir format f_Set. 20191023
    unsigned long DacCode;
    float I_UpperLimit;  //The upper limit of the Range
    unsigned char  uc_updata = 0;
    float fValue2;
    unsigned char SlaveKeepCurrent = 0;//复杂串并联从通道保持电流输出。  //COMMENT OUT 03.21.2018 CAN传输电压
    //reset digital ctrl flag
    //
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl = 0; //04.23.2013
    ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, true);
    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = false;
    switch(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType)
    {
    case CT_TYPE_DO_SETTING:   //DO 输出时暂时不对通道操作 保持上次的状态dirui 2015/03/17
    case CT_IVCH_EXTCHARGE:
    case  CT_IVCH_REST:
    case CT_IVCH_SETVARIABLE: //180503 zyx //mits8
    case CT_IVCH_SETVALUE:
    case  CT_IVCH_PAUSE:
    case  CT_IVCH_IR:	  //Add by DKQ 07.31.2015
    case  CT_IVCH_ACIR:
        DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range, 0);
        StepDisposableMSG[uc_ChannelNo].Dac_Value = DacCode;
        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = 0.0f;
        break;

    case  CT_IVCH_CURRENT:
        f_Set = CalculateCurrent(uc_ChannelNo);		//SUPPORT_PARALLEL <----- bug, here is for group. Should calc for each chan for diff ctrl type!
        if(ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo])
        {
            ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo] = false;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag = false;
        }
        uc_updata = 1;
        break;
    case  CT_IVCH_CURRENT_TB:   //add by zc 09.04.2018
    {
// Fix CURRENT_TB first incorrect CC current value issue by zc 04/15/2020
        unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;  //Fix bug. The preset current value is incorrect.

        St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
        f_Set = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                p_Ctrl->m_ucCtrlValue_DataType[1],p_Ctrl->m_CtrlValueOperator[1],p_Ctrl->m_CtrlValueCoefficients[1],
                p_Ctrl->m_CtrlValues.m_fControlValue[1]);  //从上位机取得放电电流值

        uc_updata = 1;
    }
    break;
    case CT_IVCH_CRAMP:
        f_Set = CalculateCRamp(uc_ChannelNo);
        if(ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo])
        {
            ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo] = false;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bNoNeedFineTune_flag = false;
        }
        uc_updata = 1;
        break;

    case CT_IVCH_CRATE:
        f_Set = CalculateCRate(uc_ChannelNo);
        uc_updata = 1;
        break;
    case CT_IVCH_CSTAIRCASE:
        f_Set = CalculateCStaircase(uc_ChannelNo);
        uc_updata = 1;
        break;
    case CT_IVCH_POWER:
        f_Set = CalculatePower(uc_ChannelNo);
        uc_updata = 1;
        break;

    case CT_IVCH_LOAD:
        f_Set = CalculateLoad(uc_ChannelNo);
        uc_updata = 1;
        break;

    case CT_IVCH_PRAMP:
        f_Set = CalculatePowerRamp(uc_ChannelNo);
        uc_updata = 1;
        break;
    case CT_IVCH_V_CV:
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
        {
            f_Set = Init_DigiV(uc_ChannelNo, f_Set);
        }
        else
            f_Set = 0.0;
        uc_updata = 1;
        break;
    case CT_IVCH_C_CV:
    case CT_IVCH_SIMU_I:  																	//12.22.2013ADD
    case CT_IVCH_SIMU_P:
    case CT_IVCH_SIMU_L:
        f_Set = 0.0;  																		//12.22.2013ADD
        uc_updata = 1;
        break;
    case CT_IVCH_SIMU_V:  //12.22.2013ADD		//We Do Not Offer Voltage Simulation
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl == 1)
        {

            DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange, f_Set);
        }
        else
        {
            f_Set = 0.0;  //12.22.2013ADD
            DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange, f_Set); //12.22.2013ADD
        }
        StepDisposableMSG[uc_ChannelNo].Dac_Value = DacCode; //12.22.2013ADD
        break;

    case CT_IVCH_ACS:
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl == 1)
        {
            float fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_Set);	//170719 zyx
            DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range, fValue2);
        }
        else
        {
            f_Set = 0;
            DacCode = ChannelInfo_Manager::CalibrateVoltageDAC(uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_VRange, f_Set);
        }
        StepDisposableMSG[uc_ChannelNo].Dac_Value = DacCode;
        break;

    //以下为电压，因为电压暂时没有多range  暂无修改    comment out chen
    case CT_IVCH_VOLTAGE:   //04.23.2013
    {
        St_IV_Buffer1		*	p_Buffer		=	&ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo];
        f_Set=TranslateOutputForVoltage(uc_ChannelNo);     //05.09.2013
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
        {
            ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
            ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo]=true;
            f_Set = Init_DigiV(uc_ChannelNo,f_Set);
            f_Set = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,f_Set);	//170719 zyx
            f_Set = CalculateGroupChanCtrlValue(uc_ChannelNo, f_Set, GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType);
            StepDisposableMSG[uc_ChannelNo].Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range,f_Set );
        }
        else
        {
            StepDisposableMSG[uc_ChannelNo].Volt_DacValue = ChannelInfo_Manager::CalibrateVoltageDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_VRange,p_Buffer->m_fVoltage );
            SlaveKeepCurrent = 1;
        }
    }
    break;
    case CT_IVCH_DIGIV:     //04.23.2013
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = true;
        f_Set = TranslateOutputForVoltage(uc_ChannelNo);   //05.09.2013
        f_Set = Init_DigiV(uc_ChannelNo, f_Set);
        f_Set = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_Set);	//170719 zyx
        f_Set = CalculateGroupChanCtrlValue(uc_ChannelNo, f_Set, GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType);
        DacCode = ChannelInfo_Manager::CalibrateCurrentDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range, f_Set );
        StepDisposableMSG[uc_ChannelNo].Dac_Value = DacCode;
        break;

    case CT_IVCH_VRAMP:
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = true;
        f_Set = CalculateVRamp(uc_ChannelNo);
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
        {
            f_Set = Init_DigiV(uc_ChannelNo,f_Set);
            fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,f_Set);//ChannelInfo_Manager::ClampPower(uc_ChannelNo,f_Set);                 //160518 //170918 zyx
            StepDisposableMSG[uc_ChannelNo].Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range,fValue2 );
        }
        else
        {
            StepDisposableMSG[uc_ChannelNo].Volt_DacValue = ChannelInfo_Manager::CalibrateVoltageDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_VRange,f_Set );
            SlaveKeepCurrent = 1;
        }
        break;
    case CT_IVCH_VSTAIRCASE:
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo]=true;
        f_Set=CalculateVStaircase(uc_ChannelNo);
        if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].bDigitalV)
        {
            f_Set = Init_DigiV(uc_ChannelNo,f_Set);
            float fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,f_Set);	//170719 zyx
            StepDisposableMSG[uc_ChannelNo].Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range,fValue2 );
        }
        else
        {
            StepDisposableMSG[uc_ChannelNo].Volt_DacValue = ChannelInfo_Manager::CalibrateVoltageDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_VRange,f_Set );
            SlaveKeepCurrent = 1;
        }
        break;
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
    {
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo] = true;
        //add for prepare DAC
        unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        if(ChannelCCCVStatus[uc_ChannelNo] != STEP_CCCV_INIT)
        {
            f_Set = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0],
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0],
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0],
                    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]);
        }
        else
            f_Set = 0;
        uc_updata = 1;
        break;
    }
    case CT_IVCH_CCCV_TB:
    {
        ChannelInfo_Manager::SetFilterStatus(uc_ChannelNo, false);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo]=true;
        //add for prepare DAC
        unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        f_Set=ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0],
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0],
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0],
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]);
        uc_updata = 1;
        break;
    }
    case CT_IVCH_CPULSE:
        //modified by hepeiqing 20140924, set Current DAC to 0V at the end of Transition,before PulseTimer working
        StepDisposableMSG[uc_ChannelNo].Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC( uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range,0.0f );
        break;
    default:
        break;
    }

    if(uc_updata)
    {
        f_Set = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, f_Set);	//170719 zyx
        f_Set = CalculateGroupChanCtrlValue(uc_ChannelNo, f_Set, GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType);
        DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, StepDisposableMSG[uc_ChannelNo].next_Range, f_Set);
        StepDisposableMSG[uc_ChannelNo].Dac_Value = DacCode;
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_TotalAdjust = 0; //Add by DKQ 06.03.2014
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastOutput = f_Set; //Add by DKQ 06.03.2014
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastSetValue = f_Set;
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_OutputInterval = CommonFunc::GetSystemTime();

        I_UpperLimit = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,StepDisposableMSG[uc_ChannelNo].next_Range);
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fMinError  = I_UpperLimit * m_CurFineTuneFactor.m_fMinError;
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fStep      = I_UpperLimit * m_CurFineTuneFactor.m_fStep;
        ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fMaxAdjust = ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)*I_UpperLimit * m_CurFineTuneFactor.m_fMaxAdjust;
    }

    if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        //set up slave channel's ctrl type, current range, control value:
        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))  //unit内部并联
        {
            //Possible will not set up slave disposable.  Here is good to set up slave's disposable if slave does not have neither sdu mgr and step mgr.
            //Instead, set up slave in step mgr level in step mgr::ProcessState-->ChannelInfo_Manager::LoadNextStep-->ChannelInfo_Manager::LoadStepData
            SetIntraUnitParallelSlaveDisposable(uc_ChannelNo, GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType, f_Set);
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) ) //interunit master channel set ctrl type and ctrl value
        {
            for(unsigned char uc_Idx = uc_ChannelNo;
                    uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;
                    uc_Idx++)
            {
                DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_Idx, StepDisposableMSG[uc_ChannelNo].next_Range, f_Set);
                StepDisposableMSG[uc_Idx].Dac_Value = DacCode;
                ChannelInfo_Manager::m_CurFineTune_Info[uc_Idx].m_TotalAdjust = 0; //Add by DKQ 06.03.2014
                ChannelInfo_Manager::m_CurFineTune_Info[uc_Idx].m_LastOutput = f_Set; //Add by DKQ 06.03.2014
                ChannelInfo_Manager::m_CurFineTune_Info[uc_Idx].m_LastSetValue = f_Set;
            }

            if (ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType == CAN_CMD_PARA_START)
            {
                unsigned short u16ControlType = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;
                bool b_Needfilter = CANBusParse::SubMasterIfFilter(uc_ChannelNo, u16ControlType); //根据控制类型决定是否使用滤波
                CANBusParse::Parallel_MasterSend_Start(b_Needfilter, ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType, f_Set,
                                                       ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange);
                OS_LogicProcess::LogicStatus[uc_ChannelNo] = LOGIC_IDLE;
                OS_LogicProcess::LogicCtrlType = DO_RUN; //dirui 2017/07/04
                OS_LogicProcess::m_bSlaveActive[uc_ChannelNo] = 0;
                OS_LogicProcess::be_StartThirdParty = false;
            }
            else if (ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType == CAN_CMD_PARA_MASTER_STEPCHANGE)
            {
                unsigned short u16ControlType = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;
                bool b_Needfilter = CANBusParse::SubMasterIfFilter(uc_ChannelNo, u16ControlType); ////根据控制类型决定是否使用滤波 20160822
                if(SlaveKeepCurrent)    //COMMENT OUT 03.21.2018 CAN传输电压
                {
                    float  fCtrlValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
                    f_Set=fCtrlValue/(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount   \
                                      -ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter);
                }
                CANBusParse::Parallel_MasterSend_StepChange(b_Needfilter, ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType, f_Set,
                        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange);
                OS_LogicProcess::LogicStatus[uc_ChannelNo] = LOGIC_NOTREADY;
            }

        }

        //再针对并联修正一下，主要是第一点的控制值
        if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitParaMode(uc_ChannelNo))  //unit内部并联
        {
            ChannelInfo_Manager::CopyChannelInfoFlagToSlaveChannel(uc_ChannelNo);   //copy ChannelInfo[master].bDigitalI , bDigitalV, m_bCVStage to all slaves
        }
    }
}

//----------------------------- for submaster only ---------------------------------
void Step_Manager_task::PrepareForParallelSubGroupRun()    //only one chan per unit, inter unit parallel
{
    unsigned short uc_ChannelNo = 0;
    unsigned char activeIdx =  ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    if(	ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank < 3)
    {
        safety_check::InitCtrlValue(uc_ChannelNo, CtrlValueCheck_Null, VOLTAGE_RANGE_HIGH, Step_Manager_task::m_Range[uc_ChannelNo]);
        Get_StepRunInfo(uc_ChannelNo, ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[activeIdx].m_StepInfo);
        GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange = Step_Manager_task::m_Range[uc_ChannelNo];
    }
    else
    {
        safety_check::InitCtrlValue(uc_ChannelNo, CtrlValueCheck_Null, VOLTAGE_RANGE_HIGH, ChannelInfo_Manager::CurrRngPhysicalRange1);
        Get_StepRunInfo(uc_ChannelNo, ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[activeIdx].m_StepInfo);
        GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
    }
    //no sdu for submaster, wait for CAN_CMA_PARA_START/STOP/MASTER_STEPCHANGE to get here.
    IVSDL::SubmasterStartRoutine(uc_ChannelNo);

    float f_Set;
    unsigned long DacCode;
    float I_UpperLimit;  //The upper limit of the Range

    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl = 0; //04.23.2013
    // set up switch:
    switch(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType)
    {
    case CT_IVCH_REST:
    case CT_IVCH_SETVARIABLE: //180503 zyx //mits8
    case CT_IVCH_SETVALUE:
    case CT_IVCH_PAUSE:
        SetDisposabletask(uc_ChannelNo, CT_REST);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = CCS_REST;
        break;

    case CT_IVCH_CURRENT:
        SetDisposabletask(uc_ChannelNo, CT_CURRENT);

        safety_check::InitCtrlValue(uc_ChannelNo, CtrlValueCheck_Current,
                                    VOLTAGE_RANGE_HIGH,
                                    ChannelInfo_Manager::CurrRngPhysicalRange1);//170320,add by hepeiqing

        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = CCS_I;
        break;
    case CT_IVCH_VOLTAGE:
        SetDisposabletask(uc_ChannelNo, CT_VOLTAGE);
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CtrlState = CCS_V;
        break;

    default:
        break;
    }

    StepDisposableMSG[uc_ChannelNo].msgID = STEP_TASK;
    StepDisposableMSG[uc_ChannelNo].CH_Number = uc_ChannelNo;
    //set up DAC code:
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl = 0; //????

    switch(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType)
    {
    case CT_TYPE_DO_SETTING:   //DO 输出时暂时不对通道操作 保持上次的状态dirui 2015/03/17
    case CT_IVCH_EXTCHARGE:
        break;
    case  CT_IVCH_REST:
    case CT_IVCH_SETVARIABLE: //180503 zyx //mits8
    case CT_IVCH_SETVALUE:
    case  CT_IVCH_PAUSE:
        for(unsigned char uc_Idx = uc_ChannelNo;
                uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize
                ; uc_Idx++)
        {
            DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_Idx, ChannelInfo_Manager::CurrRngPhysicalRange1, 0);
            StepDisposableMSG[uc_Idx].Dac_Value = DacCode;
        }

        break;
    case CT_IVCH_VOLTAGE:
        for(unsigned char uc_Idx = uc_ChannelNo;
                uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize
                ; uc_Idx++)
        {
            DacCode = ChannelInfo_Manager::CalibrateVoltageDAC(uc_Idx,
                      VOLTAGE_RANGE_HIGH,
                      ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue);

            StepDisposableMSG[uc_Idx].Volt_DacValue = DacCode;
        }
        break;
    case  CT_IVCH_CURRENT:

        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))  //submaster
        {
            for(unsigned char uc_Idx = uc_ChannelNo;
                    uc_Idx < uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize
                    ; uc_Idx++)
            {
                DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uc_Idx,
                          GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange,
                          ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue);

                StepDisposableMSG[uc_Idx].Dac_Value = DacCode;
            }
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_TotalAdjust = 0; //Add by DKQ 06.03.2014
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastOutput = f_Set; //Add by DKQ 06.03.2014
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastSetValue = f_Set;

            I_UpperLimit = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,StepDisposableMSG[uc_ChannelNo].next_Range);
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fMinError  = I_UpperLimit * m_CurFineTuneFactor.m_fMinError;
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fStep      = I_UpperLimit * m_CurFineTuneFactor.m_fStep;
            ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_fMaxAdjust = ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo)*I_UpperLimit * m_CurFineTuneFactor.m_fMaxAdjust;
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        {
            CalibrateAndSetGroupCurrentDAC(uc_ChannelNo, f_Set);
        }

        break;
    }

}
void Step_Manager_task::SetIntraUnitParallelSlaveDisposable(unsigned char uc_ChannelNo,  unsigned short u_CtrlType, float f_Set)
{
    unsigned long DacCode;
    for (unsigned char u_idx = uc_ChannelNo + 1; u_idx <= ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo); u_idx ++ )
    {
        DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(u_idx, StepDisposableMSG[u_idx].next_Range, f_Set);
        StepDisposableMSG[u_idx].Dac_Value = DacCode;
        ChannelInfo_Manager::m_CurFineTune_Info[u_idx].m_TotalAdjust = 0; //Add by DKQ 06.03.2014
        ChannelInfo_Manager::m_CurFineTune_Info[u_idx].m_LastOutput = f_Set; //Add by DKQ 06.03.2014
        ChannelInfo_Manager::m_CurFineTune_Info[u_idx].m_LastSetValue = f_Set;
    }
}



//--------------------------for multiple channel in intra unit group or interunit subgroup ----------------------------------

void  Step_Manager_task::CalibrateAndSetGroupCurrentDAC(unsigned short uc_ChannelNo, float fCtrlValue)
{
    unsigned long DacCode;
    unsigned short uChanIdx, uLastChanIdx, uChanNum;

    if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        uLastChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
    else
        uLastChanIdx = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;

    uChanNum = ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo);

    fCtrlValue = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, fCtrlValue);	//170719 zyx
    if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
    {
        fCtrlValue /= uChanNum;

        ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue = fCtrlValue;  //prepare for CAN sending from master to submaster

        for (uChanIdx = uc_ChannelNo; uChanIdx <= uLastChanIdx; uChanIdx ++)
        {
            DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(uChanIdx, StepDisposableMSG[uChanIdx].next_Range, fCtrlValue);
            StepDisposableMSG[uChanIdx].Dac_Value = DacCode;
        }
    }
}

float Step_Manager_task::Init_DigiV(unsigned char uc_ChannelNo, float fVoltage)
{
    float fSetCurrent;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].DigitCtrl = 1;
    ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_PidTypeFlag = PID_V;
    ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_SetValue_1B = fVoltage;
    ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_fLastErr_1B = ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_SetValue_1B - ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage;//记录进入PID初始值

    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_LastOutput_1B = ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,
            ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current);//记录进入PID初始值,量程应该选择下一个量程，否则可能会有大的过冲  dirui 20140715//ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange ,

    fSetCurrent = ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current;
    return fSetCurrent;
}

/*****************************************************
// Function Name: SendMsgToStep
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                   向Step传递信息
// Others:
//
// History：
//
// ******************************************************/
int Step_Manager_task::SendMsgToStep(unsigned char uc_ChannelNo, St_StepMessage StepMessage)
{
    unsigned char MsgRecordCount;

    if ((MsgBufManage[uc_ChannelNo].FullCount > 7) || (MsgBufManage[uc_ChannelNo].EmptyCount < 0))
    {
        return -1;
    }
    MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount] = MsgBufManage[uc_ChannelNo].RecordMsgEmpty[MsgBufManage[uc_ChannelNo].EmptyCount];

    MsgRecordCount = MsgBufManage[uc_ChannelNo].RecordMsgFull[MsgBufManage[uc_ChannelNo].FullCount]; //记录在Message队列中哪个地方；
    Step_Manager_task::StepMessageBuf[MsgRecordCount][uc_ChannelNo] = StepMessage;

    MsgBufManage[uc_ChannelNo].FullCount++;
    MsgBufManage[uc_ChannelNo].EmptyCount--;
    return 1;
}

/*****************************************************
// Function Name: CalculateStepCurrent
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算输出电流
// Others:
//
// History：
//
// ******************************************************/
float Step_Manager_task::CalculateCurrent(unsigned char uc_ChannelNo)
{
    float fCurrentOut = 0;
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl ThisStep = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    unsigned char Ctrltype = ThisStep.m_ucCtrlValue_DataType[0];
    unsigned char Operator = ThisStep.m_CtrlValueOperator[0];
    float fCoefficient = ThisStep.m_CtrlValueCoefficients[0];
    float fValue = ThisStep.m_CtrlValues.m_fControlValue[0];
    fCurrentOut = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype,Operator,fCoefficient,fValue);   // 20190917 yy
    return fCurrentOut;
}

/*****************************************************
// Function Name: TranslateOutputForVoltage
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算输出电压
// Others:
//
// History:    (未测试)
//
// ******************************************************/
float  Step_Manager_task::TranslateOutputForVoltage(unsigned char uc_ChannelNo) //04.23.2013
{
    float fRetValue;
    float fValue;
    unsigned char ActiveIdx;
    unsigned char Ctrltype;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Ctrltype = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    unsigned char Operator = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
    fRetValue = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype,Operator,fCoefficient,fValue);   // 20190917 yy
    return fRetValue;
}

/*****************************************************
// Function Name: CalculateStepCRate
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算CRate输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculateCRate(unsigned char uc_ChannelNo)
{
    float fRetValue;
    float fValue1, fValue2;
    unsigned char Ctrltype1;//,Ctrltype2;
    unsigned char ActiveIdx;


    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fValue1);   // 20190917 yy
    fValue2 = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity);
    fValue2 = MYABS(fValue2);

    fRetValue = fValue1 * fValue2; //计算输出电流

    return fRetValue;
}

/*****************************************************
// Function Name: CalculateStepCRamp
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                计算CRamp输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculateCRamp(unsigned char uc_ChannelNo)
{
    float StepTime;
    float fRetValue;
    float fValue1, fValue2;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1, Ctrltype2;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1,Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[1];
    fValue2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1];
    unsigned char Operator2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[1];
    float fCoefficient2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2,Operator2,fCoefficient2,fValue2);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fResumeStepTime; //预处理条件下，计算为指定初始时间  20131010 dirui
    fRetValue = (fValue1 + fValue2 * StepTime); //计算输出电流

    return fRetValue;
}
float  Step_Manager_task::CalculateCV(unsigned char uc_ChannelNo)
{
    float f_StepTime;

    St_CvRunInfo *CvRunInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CvInfo;
    f_StepTime = ChannelInfo_Manager::Read_CvCycleTime(uc_ChannelNo);

    if(CvRunInfo->m_bEnded)
        return CvRunInfo->m_fBase;

    if(f_StepTime < CvRunInfo->m_fStgTime)
        return (CvRunInfo->m_fGain * f_StepTime + CvRunInfo->m_fOffset);
    else
    {
        St_CvInfo *CvInfo = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_CvInfo;
        CvRunInfo->m_StageIdx++;
        if(CvRunInfo->m_StageIdx >= CvInfo->m_CvData.m_Ctrl.m_ucStageCount)
        {
            CvRunInfo->m_StageIdx = 0;
            CvRunInfo->m_CycleIdx++;
            if(CvRunInfo->m_CycleIdx >= CvInfo->m_CvData.m_Ctrl.m_ucRepeatNumber)
            {
                CvRunInfo->m_bEnded = 1;
                return CvRunInfo->m_fBase;
            }
        }
        ChannelInfo_Manager::Write_CvCycleTime(uc_ChannelNo, 0.0f);
        float fStart    = CvInfo->m_CvData.m_Stages[CvRunInfo->m_StageIdx].m_fStart;
        float fEnd      = CvInfo->m_CvData.m_Stages[CvRunInfo->m_StageIdx].m_fEnd;
        float fScanRate = CvInfo->m_CvData.m_Stages[CvRunInfo->m_StageIdx].m_fScanRate;
        CvRunInfo->m_fOffset = fStart;
        if(fEnd >= fStart)
            CvRunInfo->m_fGain = abs(fScanRate);
        else
            CvRunInfo->m_fGain = -abs(fScanRate);
        CvRunInfo->m_fStgTime = abs((fEnd - fStart) / fScanRate);

        return CvRunInfo->m_fOffset;
    }
}

/*****************************************************
// Function Name: CalculateCStaircase
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算CStaircase输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculateCStaircase(unsigned char uc_ChannelNo)
{
    float StepTime;
    float fRetValue;
    float fValue1, fValue2, fValue3;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1, Ctrltype2, Ctrltype3;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;

    Ctrltype1 = p_Ctrl->m_ucCtrlValue_DataType[0];
    fValue1 = p_Ctrl->m_CtrlValues.m_fControlValue[0];
    unsigned char Operator1 =  p_Ctrl->m_CtrlValueOperator[0];
    float fCoefficient1 =  p_Ctrl->m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = p_Ctrl->m_ucCtrlValue_DataType[1];
    fValue2 = p_Ctrl->m_CtrlValues.m_fControlValue[1];
    unsigned char Operator2 =  p_Ctrl->m_CtrlValueOperator[1];
    float fCoefficient2 = p_Ctrl->m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2, Operator2,fCoefficient2,fValue2);   // 20190917 yy

    Ctrltype3 = p_Ctrl->m_ucCtrlValue_DataType[2];
    fValue3 = p_Ctrl->m_CtrlValues.m_fControlValue[2];  //fValue3=GetParameter(uc_ChannelNo,fValue3,Ctrltype3);
    unsigned char Operator3 = p_Ctrl->m_CtrlValueOperator[2];
    float fCoefficient3 = p_Ctrl->m_CtrlValueCoefficients[2];// 20190917 yy
    fValue3 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype3,Operator3,fCoefficient3,fValue3);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fResumeStepTime; //预处理条件下，计算为指定初始时间  20131010 dirui

    fRetValue = (fValue1 + fValue2 * (unsigned short)(StepTime / fValue3)); //计算输出电流

    return fRetValue;
}

/*****************************************************
// Function Name: CalculatePower
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算Power输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculatePower(unsigned char uc_ChannelNo)
{
    float fRetValue;
    float fValue1;
    float SampleV;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1;
    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]; //fValue1=GetParameter(uc_ChannelNo,fValue1,Ctrltype1);
    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1,Operator1,fCoefficient1,fValue1);   // 20190917 yy

    safety_check::CtrlValueCheck[uc_ChannelNo].fCtrlValue = fValue1;

    SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;

    if (SampleV == 0)
        return 0;

    fRetValue = fValue1 / SampleV; //计算输出电流

    return fRetValue;
}

/*****************************************************
// Function Name: CalculateLoad
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算Load输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculateLoad(unsigned char uc_ChannelNo)
{
    float fRetValue;
    float fValue1;
    float SampleV;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];

    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fValue1);   // 20190917 yy

    safety_check::CtrlValueCheck[uc_ChannelNo].fCtrlValue = fValue1;

    SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;

    if(fValue1 != 0)
        fRetValue = - ::fabsf(SampleV / fValue1);       //计算输出电流值
    return fRetValue;
}

/*****************************************************
// Function Name: CalculatePowerRamp
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算PowerRamp输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculatePowerRamp(unsigned char uc_ChannelNo)
{
    float fRetValue;
    float fValue1, fValue2;
    float StepTime;
    float SampleV;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1, Ctrltype2;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];

    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1,Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[1];
    unsigned char Operator2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[1];
    float fCoefficient2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1];

    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2,Operator2,fCoefficient2,fValue2);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fResumeStepTime; //预处理条件下，计算为指定初始时间  20131010 dirui

    fRetValue = (fValue1 + fValue2 * StepTime);

    SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;

    if (SampleV != 0)
        fRetValue = fRetValue / SampleV;

    return fRetValue;
}

/*****************************************************
// Function Name: CalculatePowerStaircase
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算PowerStaircase输出
// Others:
//
// History:
//
// ******************************************************/
float  Step_Manager_task::CalculatePowerStaircase(unsigned char uc_ChannelNo)
{
    float StepTime;
    float fRetValue;
    float fValue1, fValue2, fValue3;
    float SampleV;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1, Ctrltype2, Ctrltype3;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;

    Ctrltype1 = p_Ctrl->m_ucCtrlValue_DataType[0];
    unsigned char Operator1 =  p_Ctrl->m_CtrlValueOperator[0];
    float fCoefficient1 =  p_Ctrl->m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = p_Ctrl->m_CtrlValues.m_fControlValue[0];

    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1,Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = p_Ctrl->m_ucCtrlValue_DataType[1];
    unsigned char Operator2 =  p_Ctrl->m_CtrlValueOperator[1];
    float fCoefficient2 =  p_Ctrl->m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = p_Ctrl->m_CtrlValues.m_fControlValue[1];

    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2,Operator2,fCoefficient2,fValue2);   // 20190917 yy

    Ctrltype3 = p_Ctrl->m_ucCtrlValue_DataType[2];
    unsigned char Operator3 =  p_Ctrl->m_CtrlValueOperator[2];
    float fCoefficient3 =  p_Ctrl->m_CtrlValueCoefficients[2];// 20190917 yy
    fValue3 = p_Ctrl->m_CtrlValues.m_fControlValue[2];

    fValue3 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype3,Operator3,fCoefficient3,fValue3);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = 0; //预处理条件下，不能计算StepTime

    fRetValue = (fValue1 + fValue2 * (unsigned short)(StepTime / fValue3));

    SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;

    if (SampleV != 0)
        fRetValue = fRetValue / SampleV;

    return fRetValue;
}


float  Step_Manager_task::CalculateISimulation(unsigned char uc_ChannelNo) //12.22.2013ADD
{
    float f_StepTime;
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
//====================== GET IR ===================================================
    float ReturnValue;
    float NextValue;
//==================== SELECT IR SOURCE ============================================
    if (ACCURATE_IR_Size[uc_ChannelNo] < 5 && INACCURATE_IR_Size[uc_ChannelNo] < 8 && !ACCURATE_IR_SizeFlag[uc_ChannelNo] && !INACCURATE_IR_SizeFlag[uc_ChannelNo])
        //修复resume之后没有IR值的BUG  by zc 01.04.2018
    {
        if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep))    //from IR step
            SIMU_I_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep;
        else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge)
            SIMU_I_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_RisingEdge;
        else if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Relax))
            SIMU_I_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Relax;   //from CCCV
        else if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV_TB))
            SIMU_I_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV_TB;   //from CCCV_TB
        else if((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule))  //From Schedule
            SIMU_I_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Schedule;
        else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV)
            SIMU_I_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV;
    }
    else if (ACCURATE_IR_Size[uc_ChannelNo] >= 5 || ACCURATE_IR_SizeFlag[uc_ChannelNo])
        SIMU_I_IR[uc_ChannelNo] = ACCURATE_IR[uc_ChannelNo];
    else
        SIMU_I_IR[uc_ChannelNo] = INACCURATE_IR[uc_ChannelNo];
//===================IF THERE IS NO IR. DO NOT CLAMP VOLTAGE =======================
//=======如果IR=0，（或者有硬件钳压）则跑普通的SIMU I程序==============================================
    if (SIMU_I_IR[uc_ChannelNo] == 0)
    {
        if(SimInfo->m_bStarted == 0)
        {
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, SimInfo->m_fStartTime);
            SimInfo->m_bStarted = 1;
            bNewSimuLoaded[uc_ChannelNo] = 1;
        }
        if(SimInfo->m_bEnded)
            return 0.0f;

        if(!SimInfo->m_bRepeat)
        {   //simulation无需做循环
            f_StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

            if(f_StepTime <= SimInfo->m_fEndTime)
                return SimInfo->m_fValue;
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_bStageLogged      = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                    return SimInfo->m_fValue;
                else
                {
                    SimInfo->m_bEnded = 1;
                    return 0.0f;
                }
            }
        }
        else
        {   //simulation需要循环做
            f_StepTime = ChannelInfo_Manager::Read_SimuCycleTime(uc_ChannelNo);

            if(f_StepTime <= SimInfo->m_fEndTime)
                return SimInfo->m_fValue;
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    SimInfo->m_bStageLogged      = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    return SimInfo->m_fValue;
                }
                else
                {
                    SimInfo->m_PointIdx = 0;
                    SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                    SimInfo->m_bStageLogged = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    SimInfo->m_CycleIdx++;

                    if((SimInfo->m_CycleIdx % SimInfo->m_LogInterval) == 0)
                    {
                        if(SimInfo->m_bExtraLog)
                            SimInfo->m_bLogCycle = 1;
                        if(SimInfo->m_LogInterval != 1)   //如果==1，表示每个CYCLE都要LOG，就无所谓DV_TIME清0了
                            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
                        SimInfo->m_bLogLimitValid = 1;
                    }
                    else
                    {
                        SimInfo->m_bLogCycle = 0;
                        SimInfo->m_bLogLimitValid = 0;
                    }
                    ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, 0.0f);
                    return SimInfo->m_fValue;
                }
            }
        }
    }
//================= IF THERE IS A VALID IR. CLAMP VOLTAGE ========================
//============如果有IR值，（且没有硬件钳压）则使用软件钳压===========================
    else
    {
        if(SimInfo->m_bStarted == 0)
        {
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, SimInfo->m_fStartTime);
            SimInfo->m_bStarted = 1;
            bNewSimuLoaded[uc_ChannelNo] = 1;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            NextValue = Step_Manager_task::GetNextIValue(uc_ChannelNo);
            return NextValue;
        }
        if(SimInfo->m_bEnded)
            return 0.0f;

        if(!SimInfo->m_bRepeat)
        {   //simulate once
            f_StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

            if(f_StepTime <= SimInfo->m_fEndTime)
            {
                ReturnValue = Step_Manager_task::CalculateReturnIValue(uc_ChannelNo);
                return ReturnValue;
            }
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                bSimuOutOfDeadBand[uc_ChannelNo] = 0;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_bStageLogged      = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    NextValue = Step_Manager_task::GetNextIValue(uc_ChannelNo);
                    return NextValue;
                }
                else
                {
                    SimInfo->m_bEnded = 1;
                    return 0.0f;
                }
            }
        }
        else
        {   //simulation 需要循环
            f_StepTime = ChannelInfo_Manager::Read_SimuCycleTime(uc_ChannelNo);

            if(f_StepTime <= SimInfo->m_fEndTime)
            {
                ReturnValue = Step_Manager_task::CalculateReturnIValue(uc_ChannelNo);
                return ReturnValue;
            }
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    SimInfo->m_bStageLogged      = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    NextValue = Step_Manager_task::GetNextIValue(uc_ChannelNo); //???
                    return NextValue;
                }
                else
                {
                    SimInfo->m_PointIdx = 0;
                    SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                    SimInfo->m_bStageLogged = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    SimInfo->m_CycleIdx++;

                    if((SimInfo->m_CycleIdx % SimInfo->m_LogInterval) == 0)
                    {
                        if(SimInfo->m_bExtraLog)
                            SimInfo->m_bLogCycle = 1;
                        if(SimInfo->m_LogInterval != 1)
                            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
                        SimInfo->m_bLogLimitValid = 1;
                    }
                    else
                    {
                        SimInfo->m_bLogCycle = 0;
                        SimInfo->m_bLogLimitValid = 0;
                    }
                    ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, 0.0f);
                    NextValue = Step_Manager_task::GetNextIValue(uc_ChannelNo);
                    return NextValue;
                }
            }
        }
    }
}
//============根据IR判断，下一个新simulation输出的电流值是否会造成过压，如果是，则需要提前减小输出的电流值，用于抑制每个simulation value第一个点的过压====================
float  Step_Manager_task::GetNextIValue(unsigned char uc_ChannelNo)			// Calculate the next step first current output value with voltage clamp      by GL
{
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
    ST_RANGE DigiVoltRange = ChannelInfo_Manager::ClampDigiVolt(uc_ChannelNo, CT_IVCH_SIMU_I);  //171016 hpq
    float PV_Voltage = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;  //Present Voltage  BY ZC 20171110
    float PV_Current = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current    GL  //171030 hpq
    float Return_Value;
    if (SimInfo->m_fValue >= 0.0f)   //下个脉冲为正值
    {
        if (((SimInfo->m_fValue - PV_Current) * SIMU_I_IR[uc_ChannelNo] + PV_Voltage) >= DigiVoltRange.High)     //超出上钳压值  //171016 hpq
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            Return_Value = (DigiVoltRange.High - PV_Voltage) / SIMU_I_IR[uc_ChannelNo] + PV_Current;				//输出恰好达到钳压值的电流//171016 hpq
            if (Return_Value > 0.0f)
            {
                if(Return_Value > PV_Current)
                    return Return_Value;
                else
                    return PV_Current;
            }
            else
                return 0.0f;
        }
        else
        {   //否则输出正常电流
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            return SimInfo->m_fValue;
        }
    }
    else //下个脉冲为负值
    {
        if (((SimInfo->m_fValue - PV_Current) * SIMU_I_IR[uc_ChannelNo] + PV_Voltage) < DigiVoltRange.Low)    //When Future voltage < ClampLow  //171016 hpq
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            Return_Value = (DigiVoltRange.Low - PV_Voltage) / SIMU_I_IR[uc_ChannelNo] + PV_Current; //171016 hpq
            if (Return_Value < 0.0f)
            {
                if(Return_Value < PV_Current)
                    return Return_Value;
                else
                    return PV_Current;
            }
            else
                return 0.0f;
        }
        else
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            return SimInfo->m_fValue;
        }
    }
}
//============根据IR判断，继续输出当前simulation的电流值是否会造成过压，如果是，则需要减小输出的电流值，用于抑制每个simulation value输出过程中的过压====================
float  Step_Manager_task::CalculateReturnIValue(unsigned char uc_ChannelNo)                          // Calculate the next current output value with voltage clamp      by GL
{
    float delta_Voltage;                //GL
    float delta_Current;                //GL
    float ReturnValue;									//GL
    unsigned char V_range = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange;
    float VRangeValue_high = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[V_range].m_Bound.m_High;
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
    ST_RANGE DigiVoltRange = ChannelInfo_Manager::ClampDigiVolt(uc_ChannelNo, CT_IVCH_SIMU_I);  //171016 hpq
    float PV_Voltage = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;  //Present Voltage  BY ZC 20171110
    float PV_Current = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current    GL  //171030 hpq
//************************************ Uplimit Clamp
    if (SimInfo->m_fValue >= 0.0f)              //Next Point > 0
    {
        //下一个电流值为正，且当前未超出钳压值时
        if (PV_Voltage < DigiVoltRange.High)     //When Voltage < ClampHigh  //171016 hpq
        {
            //若很接近钳压值
            if (DigiVoltRange.High - PV_Voltage < (VRangeValue_high*SIMU_CLOSE_TO_CLAMP_PERCENTAGE))     //Accuracy 0.005  //171016 hpq
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                //则继续输出当前电流
                bSimuOutOfDeadBand[uc_ChannelNo] = 1;
                if (PV_Current > 0.0f)
                    return PV_Current;
                else
                    return 0.0f;
            }
            else if (bSimuOutOfDeadBand[uc_ChannelNo] == 1)
            {   //若已经接近或超出精度范围时
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                delta_Voltage = DigiVoltRange.High - PV_Voltage;  //171016 hpq
                delta_Current = delta_Voltage / SIMU_I_IR[uc_ChannelNo];
                float TempValue = PV_Current + 0.5f * delta_Current;
                if(TempValue >= SimInfo->m_fValue)
                {
                    ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                    return SimInfo->m_fValue;
                }
                else if (TempValue > 0.0f)
                {
                    return TempValue;
                }
                else
                {
                    return 0.0f; //按照一定速度（10%？）回调电流值
                }
            }
            else //否则输出正常值
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                return SimInfo->m_fValue;
            }
        }
        else                       // When Voltage > ClampHigh
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            delta_Voltage = PV_Voltage - DigiVoltRange.High;  //171016 hpq
            delta_Current = delta_Voltage / SIMU_I_IR[uc_ChannelNo];
            ReturnValue = PV_Current - 2.0f * delta_Current;
            if (ReturnValue > 0.0f)
                return ReturnValue;
            else
                return 0.0f; //超出精度迅速减小电流值
        }
    }
//************************************Downlimit Clamp
    else                                        //Next Point < 0
    {
        if (PV_Voltage > DigiVoltRange.Low)     //When Voltage > ClampLow     //171016 hpq
        {
            if (PV_Voltage - DigiVoltRange.Low < (VRangeValue_high*SIMU_CLOSE_TO_CLAMP_PERCENTAGE))     //171016 hpq
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                bSimuOutOfDeadBand[uc_ChannelNo] = 1;
                if(PV_Current < 0.0f)
                    return PV_Current;
                else
                    return 0.0f;
            }
            else if (bSimuOutOfDeadBand[uc_ChannelNo] == 1)
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                delta_Voltage = DigiVoltRange.Low - PV_Voltage;     //171016 hpq
                delta_Current = delta_Voltage / SIMU_I_IR[uc_ChannelNo];
                float TempValue = PV_Current + 0.5f * delta_Current;
                if(TempValue <= SimInfo->m_fValue)
                {
                    ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                    return SimInfo->m_fValue;
                }
                else if (TempValue < 0.0f)
                {
                    return TempValue;
                }
                else
                {
                    return 0.0f;
                }
            }
            else
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                return SimInfo->m_fValue;
            }
        }
        else                       // When Voltage < ClampLow
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            delta_Voltage = DigiVoltRange.Low - PV_Voltage;     //171016 hpq
            delta_Current = delta_Voltage / SIMU_I_IR[uc_ChannelNo];
            ReturnValue = PV_Current + 2.0f * delta_Current;
            if (ReturnValue < 0.0f)
                return ReturnValue;
            else
                return 0.0f;
        }
    }
//*********************************************************************************************************************************************************************Downlimit Clamp
}

float  Step_Manager_task::CalculatePSimulation(unsigned char uc_ChannelNo) //12.22.2013ADD
{
    float f_StepTime, f_SampledV;
    f_SampledV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;

//====================== GET IR ===================================================
    float ReturnValue;
    float NextValue;
//==================== SELECT IR SOURCE ============================================
    if (ACCURATE_IR_Size[uc_ChannelNo] < 5 && INACCURATE_IR_Size[uc_ChannelNo] < 8 && !ACCURATE_IR_SizeFlag[uc_ChannelNo] && !INACCURATE_IR_SizeFlag[uc_ChannelNo])
        //修复resume之后没有IR值的BUG  by zc 01.04.2018
    {
        if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep))    //from IR step
            SIMU_P_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep;  //BY ZC 08.05.2019 fix power simulation voltage clamp issue
        else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge)
            SIMU_P_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_RisingEdge;
        else if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Relax))
            SIMU_P_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Relax;   //from CCCV
        else if ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV_TB))
            SIMU_P_IR[uc_ChannelNo] =  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV_TB;   //from CCCV_TB
        else if((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule))  //From Schedule
            SIMU_P_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Schedule;
        else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV)
            SIMU_I_IR[uc_ChannelNo] = ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV;
    }
    else if (ACCURATE_IR_Size[uc_ChannelNo] >= 5 || ACCURATE_IR_SizeFlag[uc_ChannelNo])
        SIMU_P_IR[uc_ChannelNo] = ACCURATE_IR[uc_ChannelNo];
    else
        SIMU_P_IR[uc_ChannelNo] = INACCURATE_IR[uc_ChannelNo];
//===================IF THERE IS NO IR. DO NOT CLAMP VOLTAGE =======================
//=======如果IR=0，则跑普通的SIMU P程序==============================================
    if (SIMU_P_IR[uc_ChannelNo] == 0)
    {
        if(SimInfo->m_bStarted == 0)
        {
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, SimInfo->m_fStartTime);
            SimInfo->m_bStarted = 1;
        }
        if(SimInfo->m_bEnded)
            return 0.0f;

        if(!SimInfo->m_bRepeat)
        {   //simulation无需做循环
            f_StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

            if(f_StepTime <= SimInfo->m_fEndTime)
                return SimInfo->m_fValue / f_SampledV;
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_bStageLogged      = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                    return SimInfo->m_fValue / f_SampledV;
                else
                {
                    SimInfo->m_bEnded = 1;
                    return 0.0f;
                }
            }
        }
        else
        {   //simulation需要循环做
            f_StepTime = ChannelInfo_Manager::Read_SimuCycleTime(uc_ChannelNo);

            if(f_StepTime <= SimInfo->m_fEndTime)
                return SimInfo->m_fValue / f_SampledV;
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    SimInfo->m_bStageLogged      = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    return SimInfo->m_fValue / f_SampledV;
                }
                else
                {
                    SimInfo->m_PointIdx = 0;
                    SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                    SimInfo->m_bStageLogged = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    SimInfo->m_CycleIdx++;
                    if((SimInfo->m_CycleIdx % SimInfo->m_LogInterval) == 0)
                    {
                        if(SimInfo->m_bExtraLog)
                            SimInfo->m_bLogCycle = 1;
                        if(SimInfo->m_LogInterval != 1)   //如果==1，表示每个CYCLE都要LOG，就无所谓DV_TIME清0了
                            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
                        SimInfo->m_bLogLimitValid = 1;
                    }
                    else
                    {
                        SimInfo->m_bLogCycle = 0;
                        SimInfo->m_bLogLimitValid = 0;
                    }
                    ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, 0.0f);
                    return SimInfo->m_fValue / f_SampledV;
                }
            }
        }
    }
//==============IR!=0,进行钳压=========================================
    else
    {
        if(SimInfo->m_bStarted == 0)
        {
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, SimInfo->m_fStartTime);
            SimInfo->m_bStarted = 1;

            bNewSimuLoaded[uc_ChannelNo] = 1;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            NextValue = Step_Manager_task::GetNextPValue(uc_ChannelNo);  //取下一个点的电流值
            return NextValue;

        }
        if(SimInfo->m_bEnded)
            return 0.0f;

        if(!SimInfo->m_bRepeat)
        {   //simulation无需做循环
            f_StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

            if(f_StepTime <= SimInfo->m_fEndTime)
            {
                ReturnValue = Step_Manager_task::CalculateReturnPValue(uc_ChannelNo);  //进行钳压计算后得到应该输出的电流值
                return ReturnValue;
            }
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                bSimuOutOfDeadBand[uc_ChannelNo] = 0;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_bStageLogged      = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    NextValue = Step_Manager_task::GetNextPValue(uc_ChannelNo);   //取下一个点的电流值
                    return NextValue;
                }
                else
                {
                    SimInfo->m_bEnded = 1;
                    return 0.0f;
                }
            }
        }
        else
        {   //simulation需要循环做
            f_StepTime = ChannelInfo_Manager::Read_SimuCycleTime(uc_ChannelNo);

            if(f_StepTime <= SimInfo->m_fEndTime)
            {
                ReturnValue = Step_Manager_task::CalculateReturnPValue(uc_ChannelNo);
                return ReturnValue;
            }
            else
            {
                bNewSimuLoaded[uc_ChannelNo] = 1;
                SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
                SimInfo->m_PointIdx++;
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    SimInfo->m_bStageLogged      = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    NextValue = Step_Manager_task::GetNextPValue(uc_ChannelNo);    //取下一个点的电流值
                    return NextValue;
                }
                else
                {
                    SimInfo->m_PointIdx = 0;
                    SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                    SimInfo->m_bStageLogged = 0;
                    if(SimInfo->m_bLogCycle)
                        Step_Manager_task::SendExtraLog(uc_ChannelNo);
                    SimInfo->m_CycleIdx++;
                    if((SimInfo->m_CycleIdx % SimInfo->m_LogInterval) == 0)
                    {
                        if(SimInfo->m_bExtraLog)
                            SimInfo->m_bLogCycle = 1;
                        if(SimInfo->m_LogInterval != 1)   //如果==1，表示每个CYCLE都要LOG，就无所谓DV_TIME清0了
                            ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
                        SimInfo->m_bLogLimitValid = 1;
                    }
                    else
                    {
                        SimInfo->m_bLogCycle = 0;
                        SimInfo->m_bLogLimitValid = 0;
                    }
                    ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, 0.0f);
                    NextValue = Step_Manager_task::GetNextPValue(uc_ChannelNo);   //取下一个点的电流值
                    return NextValue;
                }
            }
        }
    }
//=============================================================================
}

//============= 10042017 add by zc ==========================================
//============根据IR判断，下一个新simulation输出的电流值是否会造成过压，如果是，则需要提前减小输出的电流值，用于抑制每个simulation value第一个点的过压====================
float  Step_Manager_task::GetNextPValue(unsigned char uc_ChannelNo)			// Calculate the next step first current output value with voltage clamp      by zc
{
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
    ST_RANGE DigiVoltRange = ChannelInfo_Manager::ClampDigiVolt(uc_ChannelNo, CT_IVCH_SIMU_P);  //BY ZC 20171110
    float PV_Voltage = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;  //Present Voltage  BY ZC 20171110
    float PV_Current = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current  BY ZC 20171110
    float Next_Current = SimInfo->m_fValue / PV_Voltage;
    float Return_Value;
    if( Next_Current >= 0.0f)   //下个脉冲为正值
    {
        if (((Next_Current - PV_Current) * SIMU_P_IR[uc_ChannelNo] + PV_Voltage) >= DigiVoltRange.High)     //超出上钳压值  //by zc 20171110
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            Return_Value = (DigiVoltRange.High - PV_Voltage) / SIMU_P_IR[uc_ChannelNo] + PV_Current;				//输出恰好达到钳压值的电流   //BY ZC 20171110
            if (Return_Value > 0.0f)
            {
                if(Return_Value > PV_Current)
                    return Return_Value;
                else
                    return PV_Current;
            }
            else
                return 0.0f;
        }
        else
        {   //否则输出正常电流
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            return Next_Current;
        }
    }
    else //下个脉冲为负值
    {
        if (((Next_Current - PV_Current) * SIMU_P_IR[uc_ChannelNo] + PV_Voltage) < DigiVoltRange.Low)    //When Future voltage < ClampLow  //BY ZC 20171110
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            Return_Value = (DigiVoltRange.Low - PV_Voltage) / SIMU_P_IR[uc_ChannelNo] + PV_Current; //BY ZC 20171110
            if (Return_Value < 0.0f)
            {
                if(Return_Value < PV_Current)
                    return Return_Value;
                else
                    return PV_Current;
            }
            else
                return 0.0f;
        }
        else
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
            bSimuOutOfDeadBand[uc_ChannelNo] = 0;
            return Next_Current;
        }
    }
}
//============根据IR判断，继续输出当前simulation的电流值是否会造成过压，如果是，则需要减小输出的电流值，用于抑制每个simulation value输出过程中的过压====================
float  Step_Manager_task::CalculateReturnPValue(unsigned char uc_ChannelNo)                          // Calculate the next current output value with voltage clamp      by GL
{
    float delta_Voltage;                //GL
    float delta_Current;                //GL
    float ReturnValue;									//GL
    unsigned char V_range = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange;
    float VRangeValue_high = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[V_range].m_Bound.m_High;
    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
    ST_RANGE DigiVoltRange = ChannelInfo_Manager::ClampDigiVolt(uc_ChannelNo, CT_IVCH_SIMU_P);  //BY ZC 20171110
    float PV_Voltage = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;  //Present Voltage    GL  //BY ZC 20171110
    float PV_Current = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current    GL  //BY ZC 20171110
    float Next_Current = SimInfo->m_fValue / PV_Voltage;


//************************************ Uplimit Clamp
    if (Next_Current >= 0.0f)              //Next Point > 0
    {
        //下一个电流值为正，且当前未超出钳压值时
        if (PV_Voltage < DigiVoltRange.High)     //When Voltage < ClampHigh  //BY ZC 20171110
        {
            //若很接近钳压值
            //	if (ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fPowerSimulationVclampHigh - PV_Voltage < 0.005f)     //Accuracy 0.005
            if (DigiVoltRange.High - PV_Voltage < (VRangeValue_high*SIMU_CLOSE_TO_CLAMP_PERCENTAGE))     //Accuracy 0.005   //BY ZC 20171110
            {
                //则继续输出当前电流
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                bSimuOutOfDeadBand[uc_ChannelNo] = 1;
                if (PV_Current > 0.0f)
                    return PV_Current;
                else
                    return 0.0f;
            }
            else if (bSimuOutOfDeadBand[uc_ChannelNo] == 1)
            {   //若已经接近或超出精度范围时
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                delta_Voltage = DigiVoltRange.High - PV_Voltage; //BY ZC 20171110
                delta_Current = delta_Voltage / SIMU_P_IR[uc_ChannelNo];
                float TempValue = PV_Current + 0.5f * delta_Current;
                if(TempValue >= Next_Current)
                {
                    ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                    return Next_Current;
                }
                else if (TempValue > 0.0f)
                {
                    return TempValue;
                }
                else
                {
                    return 0.0f; //按照一定速度（50%？）回调电流值
                }
            }
            else //否则输出正常值
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                return Next_Current;
            }
        }
        else                       // When Voltage > ClampHigh
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            delta_Voltage = PV_Voltage - DigiVoltRange.High;    //BY ZC 20171110
            delta_Current = delta_Voltage / SIMU_P_IR[uc_ChannelNo];
            ReturnValue = PV_Current - 1.5f * delta_Current;
            if (ReturnValue > 0.0f)
                return ReturnValue;
            else
                return 0.0f; //超出精度迅速减小电流值
        }
    }
//************************************Downlimit Clamp
    else                                        //Next Point < 0
    {
        if (PV_Voltage > DigiVoltRange.Low)   //When Voltage > ClampLow   //BY ZC 20171110
        {
            if (PV_Voltage - DigiVoltRange.Low < (VRangeValue_high*SIMU_CLOSE_TO_CLAMP_PERCENTAGE))  //BY ZC 20171110
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                bSimuOutOfDeadBand[uc_ChannelNo] = 1;
                if(PV_Current < 0.0f)
                    return PV_Current;
                else
                    return 0.0f;
            }
            else if (bSimuOutOfDeadBand[uc_ChannelNo] == 1)
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
                delta_Voltage = DigiVoltRange.Low - PV_Voltage;   //BY ZC 20171110
                delta_Current = delta_Voltage / SIMU_P_IR[uc_ChannelNo];
                float TempValue = PV_Current + 0.5f * delta_Current;
                if(TempValue <= Next_Current)
                {
                    ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                    return Next_Current;
                }
                else if (TempValue < 0.0f)
                {
                    return TempValue;
                }
                else
                {
                    return 0.0f;
                }
            }
            else
            {
                ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
                return Next_Current;
            }
        }
        else                       // When Voltage < ClampLow
        {
            ChannelInfo_Manager::Is_ClampDigiVolt_Work[uc_ChannelNo] = true;
            bSimuOutOfDeadBand[uc_ChannelNo] = 1;
            delta_Voltage = DigiVoltRange.Low - PV_Voltage;   //BY ZC 20171110
            delta_Current = delta_Voltage / SIMU_P_IR[uc_ChannelNo];
            ReturnValue = PV_Current + 2.0f * delta_Current;
            if (ReturnValue < 0.0f)
                return ReturnValue;
            else
                return 0.0f;
        }
    }
//*********************************************************************************************************************************************************************Downlimit Clamp

}
void Step_Manager_task::Calculate_Simu_IR(unsigned char uc_ChannelNo)
{
    St_IV_Buffer1		*	p_Buffer		=	&ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo];
    if(LV_Voltage[uc_ChannelNo] == 0)   //赋初值    GL
        LV_Voltage[uc_ChannelNo] = p_Buffer->m_fVoltage;
    float PV_Current = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current    GL  //171030 hpq
//===================  IR   =========================================
    delta_voltage[uc_ChannelNo] = p_Buffer->m_fVoltage - LV_Voltage[uc_ChannelNo];
    delta_current[uc_ChannelNo] = PV_Current - LV_Current[uc_ChannelNo];  //171030 hpq
    ACCURATE_IR_Sum[uc_ChannelNo] = 0;     //内阻准确值累加值清零
    INACCURATE_IR_Sum[uc_ChannelNo] = 0;		//内阻不准确值累加值清零
    if (abs(delta_voltage[uc_ChannelNo]) >= 0.005f && abs(delta_current[uc_ChannelNo] >= 0.01f))       //Accurate IR     > 0.005   且   > 0.1
    {
        ACCURATE_IR_Update[uc_ChannelNo] = delta_voltage[uc_ChannelNo] / delta_current[uc_ChannelNo];		//DELTA_V/DELTA_I = IR
        if (ACCURATE_IR_Update[uc_ChannelNo] > 0)                          //IR值必须是正值，否则弃点
            AccurateIR[uc_ChannelNo][ACCURATE_IR_Size[uc_ChannelNo]++] = ACCURATE_IR_Update[uc_ChannelNo];			//记录该值至寄存器中
        if (ACCURATE_IR_Size[uc_ChannelNo] >= 10)
        {
            ACCURATE_IR_Size[uc_ChannelNo] = 0;
            ACCURATE_IR_SizeFlag[uc_ChannelNo] = 1;  //判断是否已经记满10个点
        }
    }
    else if (abs(delta_current[uc_ChannelNo]) >= 0.01f)		 //Inaccurate IR < 0.005    且   > 0.1
    {
        INACCURATE_IR_Update[uc_ChannelNo] = delta_voltage[uc_ChannelNo] / delta_current[uc_ChannelNo];
        if (INACCURATE_IR_Update[uc_ChannelNo] > 0)
            InaccurateIR[uc_ChannelNo][INACCURATE_IR_Size[uc_ChannelNo]++] = INACCURATE_IR_Update[uc_ChannelNo];
        if (INACCURATE_IR_Size[uc_ChannelNo] >= 10)
        {
            INACCURATE_IR_Size[uc_ChannelNo] = 0;
            INACCURATE_IR_SizeFlag[uc_ChannelNo] = 1;
        }
    }
//========求Inaccurate IR平均值=====================
    if(INACCURATE_IR_SizeFlag[uc_ChannelNo])   //如果已经记满10个点
    {
        for(int i = 0; i < 10; i++)
            INACCURATE_IR_Sum[uc_ChannelNo] += InaccurateIR[uc_ChannelNo][i];
        INACCURATE_IR[uc_ChannelNo] = INACCURATE_IR_Sum[uc_ChannelNo] / 10.0f; //10个点求平均
    }
    else
    {
        for(int i = 0; i < INACCURATE_IR_Size[uc_ChannelNo]; i++)
            INACCURATE_IR_Sum[uc_ChannelNo] += InaccurateIR[uc_ChannelNo][i];
        INACCURATE_IR[uc_ChannelNo] = INACCURATE_IR_Sum[uc_ChannelNo] / INACCURATE_IR_Size[uc_ChannelNo]; //未达到10个点则有几个点就求几个平均值
    }
//=======求Accurate IR平均值=========================
    if (ACCURATE_IR_SizeFlag[uc_ChannelNo])
    {
        for(int i = 0; i < 10; i++)
            ACCURATE_IR_Sum[uc_ChannelNo] += AccurateIR[uc_ChannelNo][i];
        ACCURATE_IR[uc_ChannelNo] = ACCURATE_IR_Sum[uc_ChannelNo] / 10.0f;
    }
    else
    {
        for(int i = 0; i < ACCURATE_IR_Size[uc_ChannelNo]; i++)
            ACCURATE_IR_Sum[uc_ChannelNo] += AccurateIR[uc_ChannelNo][i];
        ACCURATE_IR[uc_ChannelNo] = ACCURATE_IR_Sum[uc_ChannelNo] / ACCURATE_IR_Size[uc_ChannelNo];
    }
    LV_Voltage[uc_ChannelNo] = p_Buffer->m_fVoltage;  //记录此时电压电流
    LV_Current[uc_ChannelNo] = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;  //Present Current    GL  //171030 hpq
    bAfterSimuLoaded[uc_ChannelNo] = 0;
}
//=========================10042017 add end by zc=======================================================


float  Step_Manager_task::CalculateLSimulation(unsigned char uc_ChannelNo) //12.22.2013ADD
{
    float f_StepTime, f_SampledV;

    f_SampledV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;

    St_SimuRunInfo *SimInfo = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo;
    if(SimInfo->m_bStarted == 0)
    {
        SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
        ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, SimInfo->m_fStartTime);
        SimInfo->m_bStarted = 1;
    }
    if(SimInfo->m_bEnded)
        return 0.0f;

    if(!SimInfo->m_bRepeat)
    {   //simulation无需做循环
        f_StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

        if(f_StepTime <= SimInfo->m_fEndTime)
            return (-::fabsf((f_SampledV / SimInfo->m_fValue)));
        else
        {
            bNewSimuLoaded[uc_ChannelNo] = 1;
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            SimInfo->m_bStageLogged      = 0;
            if(SimInfo->m_bLogCycle)
                Step_Manager_task::SendExtraLog(uc_ChannelNo);
            SimInfo->m_PointIdx++;
            if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                return (-::fabsf((f_SampledV / SimInfo->m_fValue)));
            else
            {
                SimInfo->m_bEnded = 1;
                return 0.0f;
            }
        }
    }
    else
    {   //simulation需要循环做
        f_StepTime = ChannelInfo_Manager::Read_SimuCycleTime(uc_ChannelNo);

        if(f_StepTime <= SimInfo->m_fEndTime)
            return (-::fabsf((f_SampledV / SimInfo->m_fValue)));
        else
        {
            bNewSimuLoaded[uc_ChannelNo] = 1;
            SimInfo->m_StageDacTime      = CommonFunc::GetSystemTime();
            SimInfo->m_PointIdx++;
            if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
            {
                SimInfo->m_bStageLogged      = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                return (-::fabsf((f_SampledV / SimInfo->m_fValue)));
            }
            else
            {
                SimInfo->m_PointIdx = 0;
                SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                SimInfo->m_bStageLogged = 0;
                if(SimInfo->m_bLogCycle)
                    Step_Manager_task::SendExtraLog(uc_ChannelNo);
                SimInfo->m_CycleIdx++;
                if((SimInfo->m_CycleIdx % SimInfo->m_LogInterval) == 0)
                {
                    if(SimInfo->m_bExtraLog)
                        SimInfo->m_bLogCycle = 1;
                    if(SimInfo->m_LogInterval != 1)   //如果==1，表示每个CYCLE都要LOG，就无所谓DV_TIME清0了
                        ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
                    SimInfo->m_bLogLimitValid = 1;
                }
                else
                {
                    SimInfo->m_bLogCycle = 0;
                    SimInfo->m_bLogLimitValid = 0;
                }
                ChannelInfo_Manager::Write_SimuCycleTime(uc_ChannelNo, 0.0f);
                return (-::fabsf((f_SampledV / SimInfo->m_fValue)));
            }
        }
    }
}

/*****************************************************
// Function Name: CalculateVRamp
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算VRamp输出
// Others:
//
// History:    (未测试)
//
// ******************************************************/
float  Step_Manager_task::CalculateVRamp(unsigned char uc_ChannelNo)
{
    float StepTime;
    float fRetValue;
    float fValue1, fValue2;
    unsigned char ActiveIdx;
    unsigned char Ctrltype1, Ctrltype2;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    Ctrltype1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[0];
    unsigned char Operator1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[0];
    float fCoefficient1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];

    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCtrlValue_DataType[1];
    unsigned char Operator2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueOperator[1];
    float fCoefficient2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1];

    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2, Operator2,fCoefficient2,fValue2);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = 0; //预处理条件下，不能计算StepTime

    fRetValue = (fValue1 + fValue2 * StepTime);

    return fRetValue;
}

/*****************************************************
// Function Name: CalculateVStaircase
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 计算VStaircase输出
// Others:
//
// History:    (未测试)
//
// ******************************************************/
float  Step_Manager_task::CalculateVStaircase(unsigned char uc_ChannelNo)
{
    float StepTime;
    float fRetValue = 0.0f;
    float fValue1, fValue2, fValue3;
    unsigned char Ctrltype1, Ctrltype2, Ctrltype3;

    unsigned char ActiveIdx;

    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;

    Ctrltype1 = p_Ctrl->m_ucCtrlValue_DataType[0];
    unsigned char Operator1 =  p_Ctrl->m_CtrlValueOperator[0];
    float fCoefficient1 =  p_Ctrl->m_CtrlValueCoefficients[0];// 20190917 yy
    fValue1 = p_Ctrl->m_CtrlValues.m_fControlValue[0];

    fValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1,Operator1,fCoefficient1,fValue1);   // 20190917 yy

    Ctrltype2 = p_Ctrl->m_ucCtrlValue_DataType[1];
    unsigned char Operator2 =  p_Ctrl->m_CtrlValueOperator[1];
    float fCoefficient2 =  p_Ctrl->m_CtrlValueCoefficients[1];// 20190917 yy
    fValue2 = p_Ctrl->m_CtrlValues.m_fControlValue[1];

    fValue2 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype2,Operator2,fCoefficient2,fValue2);   // 20190917 yy

    Ctrltype3 = p_Ctrl->m_ucCtrlValue_DataType[2];
    unsigned char Operator3 =  p_Ctrl->m_CtrlValueOperator[2];
    float fCoefficient3 =  p_Ctrl->m_CtrlValueCoefficients[2];// 20190917 yy
    fValue3 = p_Ctrl->m_CtrlValues.m_fControlValue[2];

    fValue3 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype3,Operator3,fCoefficient3,fValue3);   // 20190917 yy

    StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime);

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus == STEPLOADSTEP)
        StepTime = 0; //预处理条件下，不能计算StepTime
    if(fValue3 != 0)
        fRetValue = (fValue1 + fValue2 * (unsigned short)(StepTime / fValue3));

    return fRetValue;
}


/*****************************************************
// Function Name: SetDisposabletask
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                         设置引擎任务
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::SetDisposabletask(unsigned char uc_ChannelNo, unsigned char CtrlType)
{
    unsigned char activeIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_ChannelInfo		*		p_Info	=		&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    //(1) set slaves disposable msg: auto calib flag, next range, last range,:
    if(!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        //for intra unit parallel group master channel:
        unsigned short uChanIdx, uLastChanIdx;//uFromChanIdx,

        uLastChanIdx = uc_ChannelNo + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[p_Info->m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize; //ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);
        for (uChanIdx = uc_ChannelNo + 1; uChanIdx < uLastChanIdx; uChanIdx ++)
        {
            //calib flag:
            StepDisposableMSG[uChanIdx].AutoCaliFlag = 0;    //comment out chen 20130426

            //current range:
            //slave channel's current range is always set to high. Ctrl type can only be rest of current
            StepDisposableMSG[uChanIdx].next_Range = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[activeIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
            StepDisposableMSG[uChanIdx].last_Range = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[activeIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
            StepDisposableMSG[uChanIdx].RangeSet = 0;

            //voltage range:
            // set all slaves to what??? TODO
            StepDisposableMSG[uChanIdx].next_VRange = VOLTAGE_RANGE_HIGH; //p_Info->m_ucVoltageRange;
            StepDisposableMSG[uChanIdx].last_VRange = VOLTAGE_RANGE_HIGH; //p_Info->m_ucVoltageRange;
        }

    }

    unsigned char Type = CT_REST;
    St_DisposableMSG	*		p_Msg		=		&StepDisposableMSG[uc_ChannelNo];
    p_Msg->AutoCaliFlag = 0;    //comment out chen 20130426
    p_Msg->next_Range = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    p_Msg->last_Range = p_Info->m_ucCurrentRange;
    p_Msg->next_VRange = p_Info->m_ucVoltageRange;
    p_Msg->last_VRange = p_Info->m_ucVoltageRange;
    p_Msg->RangeSet = 0;

    switch(p_Info->m_CtrlState)
    {
    case CCS_I:
        switch(CtrlType)
        {
        case CT_REST:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToRst.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            break;

        case CT_CURRENT:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToCur.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CCHN;
            if(p_Info->m_ucCurrentRange != GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange)
                p_Msg->RangeSet = 1;
            Type = CT_CURRENT;
            break;

        case CT_VOLTAGE:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToVlt.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CCHN;
            if(p_Info->m_ucCurrentRange != GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange)
                p_Msg->RangeSet = 1;
            Type = CT_CURRENT;
            break;
        case CT_OFF:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            Type = CT_OFF;
            break;
        default:
            return;
        }
        break;
    case CCS_V:
        switch(CtrlType)
        {
        case CT_REST:
            p_Msg->Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen=OS_LogicProcess::SetCtrlVltToRst.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            break;

        case CT_CURRENT:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToCur.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CCHN;
            if(p_Info->m_ucCurrentRange != GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange)
                p_Msg->RangeSet = 1; //量程切换全部需要相同处理 20140827 dirui
            Type = CT_CURRENT;
            break;
        case CT_VOLTAGE:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToVlt.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CCHN;
            if(p_Info->m_ucCurrentRange != GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange)
                p_Msg->RangeSet = 1;
            Type = CT_CURRENT;
            break;
        case CT_OFF:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            Type = CT_OFF;
            break;
        default:
            return;
        }
        break;
    case CCS_REST:
        switch(CtrlType)
        {

        case CT_CURRENT:
            p_Msg->Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen=OS_LogicProcess::SetCtrlRstToCur.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_OPENCHN;

            p_Msg->next_Range = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange;  //chen add
            Type = CT_CURRENT;
            break;
        case CT_VOLTAGE:
            p_Msg->Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen=OS_LogicProcess::SetCtrlRstToVlt.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_OPENCHN;
            Type = CT_CURRENT;
            break;

        case CT_REST:
            p_Msg->Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen=OS_LogicProcess::SetCtrlOffToRst.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_NONE;

            break;
        case CT_OFF:
            p_Msg->Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen=OS_LogicProcess::SetCtrlRstToOff.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_NONE;
            break;
        default:
            return;
        }
        break;
    case CCS_OFF:
        switch(CtrlType)
        {
        case CT_REST:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToRst.stepLen;  //g modify 20131108
            p_Msg->Disposable_Flag = DISPOSABLE_to_NONE;
            break;

        case CT_CURRENT:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToCur.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_OPENCHN;
            Type = CT_CURRENT;
            break;

        case CT_VOLTAGE:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToVlt.stepLen;
            p_Msg->Disposable_Flag = DISPOSABLE_to_OPENCHN;
            Type = CT_CURRENT;
            break;
        case CT_OFF:
            p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
            p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetCtrlRstToOff.stepLen;  //g modify 20131108
            p_Msg->Disposable_Flag = DISPOSABLE_to_NONE;
            break;
        default:
            return;
        }
        break;
    default:
        return;
    }
    p_Msg->TargetCtrlType = CtrlType;  // 20200810 yy

    CallForSetSlaveChanDisposble(uc_ChannelNo, Type);  //add by dirui,20150908
}

void Step_Manager_task::CallForSetSlaveChanDisposble(unsigned short uc_ChannelNo, unsigned short uc_SlaveToCtrlType)
{
    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_WholeGroupSize == 1)
        return;   //只有一个通道就不必再调用了

    if ((uc_SlaveToCtrlType == CT_REST) || (uc_SlaveToCtrlType == CT_OFF))
    {
        if(!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        {
            if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
            {
                SetSlaveChannelDisposableTask(uc_ChannelNo + 1,               //dirui 20150618
                                              ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo),
                                              uc_SlaveToCtrlType);
            }
            else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
            {
                if (ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1) //only need to set up slave's disposable when havig multiple channels in unit
                {
                    SetSlaveChannelDisposableTask(uc_ChannelNo + 1, uc_ChannelNo + \
                                                  ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1, \
                                                  uc_SlaveToCtrlType);
                }
            }
            else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))
            {
                SetSlaveChannelDisposableTask(uc_ChannelNo + 1, uc_ChannelNo +  \
                                              ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1, \
                                              uc_SlaveToCtrlType);
            }
        }
    }

    /////////////////////////////////////////////////////////////////
    else if (uc_SlaveToCtrlType == CT_CURRENT)
    {
        if(!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        {
            if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
            {
                if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))

                    SetSlaveChannelDisposableTask(uc_ChannelNo + 1,												//dirui 0618
                                                  ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo),
                                                  CT_CURRENT);
                else
                    SetSlaveChannelDisposableTask(uc_ChannelNo + 1,
                                                  ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo),
                                                  CT_REST);//uc_ChannelNo + 181220 zyx

            }
            else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))  //inter unit, master or submaster
            {
                if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
                {
                    if (ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1) //only need to set up slave's disposable when havig multiple channels in unit
                    {
                        SetSlaveChannelDisposableTask(uc_ChannelNo + 1, uc_ChannelNo +
                                                      ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize - 1,
                                                      CT_CURRENT);
                    }
                }
                else if (ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > 1) //only need to set up slave's disposable when havig multiple channels in unit
                {
                    SetSlaveChannelDisposableTask(uc_ChannelNo+1, uc_ChannelNo+
                                                  ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize-1,
                                                  CT_REST);
                }
            }
            else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))
            {
                SetSlaveChannelDisposableTask(uc_ChannelNo+1,uc_ChannelNo+
                                              ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize-1,
                                              CT_CURRENT);
            }
        }
    }

    return;
}



void Step_Manager_task::SetIntraUnitParallelSlaveChannelCtrlState(unsigned char uc_ChannelNo, unsigned char uc_ChannelCtrlState)
{
    if (!ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        return;

    unsigned short uChanIdx, uLastChanIdx;

    uLastChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(uc_ChannelNo);

    for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uLastChanIdx; uChanIdx ++) //dirui 20150629
    {
        if (ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
            ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = uc_ChannelCtrlState;
        else
            ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_OFF;
    }


}

void Step_Manager_task::SetSlaveChannelDisposableTask(unsigned char uc_FromChan, unsigned char uc_ToChan, unsigned char uc_NewStepCtrlType)
{
    //
    //submasters, slaves never change current range and voltage range, they stay at high range for both and the only two ctrl types:rest or current.
    //
    unsigned short uChanIdx;//,m_SLaveCTType;
    unsigned char activeIdx;
    if(uc_FromChan > 1)
        activeIdx = ChannelInfo_Manager::ChannelStep[uc_FromChan - 1].m_ucActiveIdx;
    else
        activeIdx = ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx;
    for (uChanIdx = uc_FromChan; uChanIdx <= uc_ToChan; uChanIdx ++)
    {
        switch(ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState)  //dirui 20150629
        {
        case CCS_I:

            switch(uc_NewStepCtrlType)
            {
            case CT_REST:
            case CT_OFF:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_CLOSECHN;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_OFF;
                break;

            case CT_CURRENT:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToCur.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_CCHN;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_I;
                break;

            case CT_VOLTAGE:
                break;

            default:
                break;
            }
            break;

        case CCS_REST:  //off, rest, voltage, external.Slave can only be off/rest, no voltage ctrl state
            switch(uc_NewStepCtrlType)
            {
            case CT_REST:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen=OS_LogicProcess::SetCtrlOffToRst.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_NONE;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState=CCS_REST;
                break;
            case CT_OFF:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen=OS_LogicProcess::SetCtrlRstToOff.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_NONE;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState=CCS_OFF;
                break;

            case CT_CURRENT:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen=OS_LogicProcess::SetCtrlRstToCur.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_OPENCHN;
                StepDisposableMSG[uChanIdx].next_Range= ChannelInfo_Manager::ChannelStep[uc_FromChan - 1].m_Info[activeIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange; //if slave is on, must be current ctrl, high range.
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState=CCS_I;
                break;

            case CT_VOLTAGE:
            default:
                break;
            }
            break;
        case CCS_OFF:

            switch(uc_NewStepCtrlType)
            {
            case CT_REST:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr=OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen=OS_LogicProcess::SetCtrlOffToRst.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_NONE;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState=CCS_REST;
                break;
            case CT_OFF:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen = OS_LogicProcess::SetCtrlRstToOff.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_NONE;
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_OFF;
                break;

            case CT_CURRENT:
                StepDisposableMSG[uChanIdx].Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr;
                StepDisposableMSG[uChanIdx].Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToCur.stepLen;
                StepDisposableMSG[uChanIdx].Disposable_Flag = DISPOSABLE_to_OPENCHN;
                StepDisposableMSG[uChanIdx].next_Range = ChannelInfo_Manager::ChannelStep[uc_FromChan - 1].m_Info[activeIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange; //if slave is on, must be current ctrl, high range.
                ChannelInfo_Manager::ChannelInfo[uChanIdx].m_CtrlState = CCS_I;
                break;

            case CT_VOLTAGE:
            default:
                break;
            }
            break;

        default:
            break;
        }
    }
    StepDisposableMSG[uChanIdx].TargetCtrlType = uc_NewStepCtrlType;  // 20200810 yy
    return;
}


/*****************************************************
// Function Name:ProcessStepCheckLimit
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                StepLimit处理状态机
// Others:
//
// History：
//
// ******************************************************/
void Step_Manager_task::ProcessStepCheckLimit(unsigned char uc_ChannelNo)
{
    if((bStepLimitChecked[uc_ChannelNo] == 1) || (PULSE_bStepLimitChecked[uc_ChannelNo] == 1))
    {
        bStepLimitChecked[uc_ChannelNo] = 0;
        PULSE_bStepLimitChecked[uc_ChannelNo] = 0;

    }
    else
        return;
    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == IDLE
            ||ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus == GOSTOP)    // 04.26.2018 yy
        return;

    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    unsigned char bReached = 1;   //  20130927 何
    unsigned char bSetVariable = (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE);
    unsigned char bSetValue = (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVALUE); //171109 zyx
    unsigned char bAllLimitChecked = 1;
    St_ScheduleMessage ScheduleMessage;

    St_UnsafeLimit		*		p_Limit		=		&EthernetParse::UnsafeLimit[uc_ChannelNo];
    St_StepData			*		p_Data		=		&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data;

    for(unsigned char LimitIdx = 0; LimitIdx < p_Data->m_Ctrl.m_ucStepLimitNo; LimitIdx++)
    {
        bAllLimitChecked &= ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[LimitIdx].m_bChecked;
        if(p_Data->m_Limits[LimitIdx].m_bAnyValid)
            bReached  = bReached && (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[LimitIdx].m_AnyLimitReachedCount > 0);
        else
            bReached  = bReached && (ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[LimitIdx].m_bReached);
        if(bReached)
        {
            ChannelInfo_Manager::m_Online_UpdataEnable[uc_ChannelNo] = false;
            if(CommonFunc::uc_Support_AUX)
            {
                if(Step_Manager_task:: bExtChargeFlag[uc_ChannelNo])
                {
                    Step_Manager_task:: bExtChargeFlag[uc_ChannelNo] = 0;
                    CANBusParse::IV_SendExtChargeOut(uc_ChannelNo, 0);
                }
            }
            if(GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVARIABLE
                    && GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType != CT_IVCH_SETVALUE)  //mits8
                ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, LAST_POINT); //180309 zyx
            switch(p_Data->m_Limits[LimitIdx].m_u16GotoStepID)
            {
            case MCU_GOTOSTOPSTEP:
            case MCU_GOTONEXTSTEP:
            case MCU_GOTOSAMESTEP:
            case MCU_GOTOUNSAFE:
                if(p_Data->m_Limits[LimitIdx].m_u16GotoStepID == MCU_GOTOUNSAFE)
                {
                    memset((void *)&EthernetParse::UnsafeLimit[uc_ChannelNo], 0, sizeof(EthernetParse::UnsafeLimit[uc_ChannelNo]));
                    if(LimitIdx == 0)
                    {
                        p_Limit->m_fLeft_Value1 = p_Data->m_Limits[0].m_fLeft_Value;   //for  Limit Unsafe  20140211
                        p_Limit->m_fRight_Value1 = p_Data->m_Limits[0].m_fRight_Value;
                    }
                    else if(LimitIdx == 1)
                    {
                        if(p_Data->m_Limits[0].m_u16GotoStepID == MCU_GOTOANDCONDICTION)
                        {
                            p_Limit->m_fLeft_Value1 = p_Data->m_Limits[0].m_fLeft_Value;
                            p_Limit->m_fRight_Value1 = p_Data->m_Limits[0].m_fRight_Value;
                            p_Limit->m_fLeft_Value2 = p_Data->m_Limits[1].m_fLeft_Value;
                            p_Limit->m_fRight_Value2 = p_Data->m_Limits[1].m_fRight_Value;
                        }
                        else
                        {
                            p_Limit->m_fLeft_Value1 = p_Data->m_Limits[1].m_fLeft_Value;
                            p_Limit->m_fRight_Value1 = p_Data->m_Limits[1].m_fRight_Value;
                        }
                    }
                    else
                    {
                        if((p_Data->m_Limits[(LimitIdx - 1)].m_u16GotoStepID == MCU_GOTOANDCONDICTION)
                                && (p_Data->m_Limits[(LimitIdx - 2)].m_u16GotoStepID == MCU_GOTOANDCONDICTION))
                        {
                            p_Limit->m_fLeft_Value1 = p_Data->m_Limits[(LimitIdx - 2)].m_fLeft_Value;
                            p_Limit->m_fRight_Value1 = p_Data->m_Limits[(LimitIdx - 2)].m_fRight_Value;
                            p_Limit->m_fLeft_Value2 = p_Data->m_Limits[LimitIdx - 1].m_fLeft_Value;
                            p_Limit->m_fRight_Value2 = p_Data->m_Limits[LimitIdx - 1].m_fRight_Value;
                            p_Limit->m_fLeft_Value3 = p_Data->m_Limits[LimitIdx].m_fLeft_Value;
                            p_Limit->m_fRight_Value3 = p_Data->m_Limits[LimitIdx].m_fRight_Value;
                        }
                        else if(p_Data->m_Limits[(LimitIdx - 1)].m_u16GotoStepID == MCU_GOTOANDCONDICTION)
                        {
                            p_Limit->m_fLeft_Value1 = p_Data->m_Limits[(LimitIdx - 1)].m_fLeft_Value;
                            p_Limit->m_fRight_Value1 = p_Data->m_Limits[(LimitIdx - 1)].m_fRight_Value;
                            p_Limit->m_fLeft_Value2 = p_Data->m_Limits[LimitIdx].m_fLeft_Value;
                            p_Limit->m_fRight_Value2 = p_Data->m_Limits[LimitIdx].m_fRight_Value;
                        }
                        else
                        {
                            p_Limit->m_fLeft_Value1 = p_Data->m_Limits[LimitIdx].m_fLeft_Value;
                            p_Limit->m_fRight_Value1 = p_Data->m_Limits[LimitIdx].m_fRight_Value;
                        }
                    }
                }
                ScheduleMessage.GotoStepID = p_Data->m_Limits[LimitIdx].m_u16GotoStepID;
                IVSDL::LimitArrivedRoutine(uc_ChannelNo);
                break;
            case MCU_GOTOANDCONDICTION:
                continue;   //continue the for(;;) loop ... ...
            //break;
            case MCU_GOTOWARNING:
            case MCU_GOTOGROUPWAITFORNEXT:
            case MCU_GOTOGROUPFOLLOWNEXT:
            case MCU_GOTOLOG:
            case MCU_GOTOGLOBALUNSAFE:
            case MCU_GOTOGROUPUNSAFE:
                ScheduleMessage.GotoStepID = MCU_GOTOUNSAFE;
                IVSDL::LimitArrivedRoutine(uc_ChannelNo);
                break;
            default:
                ScheduleMessage.GotoStepID = p_Data->m_Limits[LimitIdx].m_u16GotoStepID;
                IVSDL::LimitArrivedRoutine(uc_ChannelNo);
                break;
            }

            if(p_Data->m_Ctrl.m_u16ControlType != CT_SPECIAL_IRTEST) //if active step is SPECIAL_IRTEST ,don't log.  dirui add 2014/08/04
            {
                ChannelInfo_Manager::Copy_PVmeta2LSmeta(uc_ChannelNo);//zyx 2204241
                MetaVariable_Task::calculate_capacity(1, uc_ChannelNo);
                Schedule_Manager_task::SendMetaSync(uc_ChannelNo);
                if(p_Data->m_Ctrl.m_u16ControlType == CT_IVCH_CPULSE)
                    Pulse_LogLimit_task::DoWork(); //151026,add by hepeqing,in case of missing log data doing single pulse
                EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; // 2013/10/24 yang  修改关键点记录数据
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bCriticalLog = 1;//160425

                Smart_Battery::SetSMBusLastLog(uc_ChannelNo, NOTLASTPOINT);//add by zc 04/01/2019
                if(bSetVariable)
                    IVSDL::IVLogTCCountForAccess(uc_ChannelNo);
                Step_Manager_task::SendLog(uc_ChannelNo);

                ChannelInfo_Manager::PulseLimitArriveData[uc_ChannelNo].bPulseStepEnded = 1;
            }
            bNeedCheck[uc_ChannelNo] = 0;
            PULSE_bNeedCheck[uc_ChannelNo] = 0;
            ScheduleMessage.MsgNumber = Sch_LimitArrive_Msg;
            //161019 zyx
            IVSDL::TriggerLog(uc_ChannelNo, SDL_DATA_FOR_DISPLAY, LOG_NORMAL_DATA_POINT);
            ChannelInfo_Manager::ClearPV_CV_StageTime(uc_ChannelNo);
            ChannelInfo_Manager::ClearPV_CV_StageCurrent(uc_ChannelNo);
            //161019 zyx
            ScheduleMessage.m_Message.m_RunMsg.LimitID = p_Data->m_Limits[LimitIdx].m_u16LimitID;
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPWAIT;
            if(p_Data->m_bIRTestStep) //若是电阻测试步，那么重新制定下一步
                ScheduleMessage.GotoStepID = IR::IRSpecialGotoNext[uc_ChannelNo];

            Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, ScheduleMessage);

            ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage);
            ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);

            if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_ZeroTest_Enable)
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_TransStartTime = CommonFunc::GetSystemTime();
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_Trans_Started = true;
            }
            return;
        }
        else
        {
            if(p_Data->m_Limits[LimitIdx].m_u16GotoStepID != MCU_GOTOANDCONDICTION)
                bReached = 1;
        }
    }

    if(bAllLimitChecked && (bSetVariable || bSetValue))//171109 zyx
    {
        ScheduleMessage.GotoStepID = MCU_GOTONEXTSTEP;
        IVSDL::LimitArrivedRoutine(uc_ChannelNo);     //解决SetVariable无LOG点问题
        EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT; //   修改关键点记录数据

        ChannelInfo_Manager::Copy_PVmeta2LSmeta(uc_ChannelNo);//zyx 2204241
        MetaVariable_Task::calculate_capacity(1, uc_ChannelNo);
        Schedule_Manager_task::SendMetaSync(uc_ChannelNo);

        if(bSetVariable)
            IVSDL::IVLogTCCountForAccess(uc_ChannelNo);

        Step_Manager_task::SendLog(uc_ChannelNo);
        if(Smart_Battery::BeSupportSmbus)//add by zc 03.04.2019
        {
            Smart_Battery::SetSMBusLastLog(uc_ChannelNo, NOTLASTPOINT);//zyx 2110091
        }

        bNeedCheck[uc_ChannelNo] = 0;
        PULSE_bNeedCheck[uc_ChannelNo] = 0;
        ScheduleMessage.MsgNumber = Sch_LimitArrive_Msg;
        ScheduleMessage.m_Message.m_RunMsg.LimitID = SPECIALLIMITID;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus = STEPWAIT;

        if(bSetValue)//171109 zyx
        {
            ScheduleMessage.GotoStepID = p_Data->m_Limits[0].m_u16GotoStepID; //171017 zyx
        }

        if(p_Data->m_bIRTestStep) //若是电阻测试步，那么重新制定下一步
            ScheduleMessage.GotoStepID = IR::IRSpecialGotoNext[uc_ChannelNo];

        Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, ScheduleMessage);
    }
}

/*****************************************************
// Function Name:SendLog
// Version:       V1.0
// Date:                2012/11/12
// Description:
//                 发送log数据
// Others:
//
// History：
//
// ******************************************************/
int Step_Manager_task::SendLog(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::DVmetaInit(uc_ChannelNo);  //每次Log数据后要清DV（与广州商议暂定）

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bValid
            && (!ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bLogLimitValid)
            && (!ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bCriticalLog) ) //160425
        return 1;		//如果在做SIMULATION，而且SIMULAITON控制不响应LOG LIMIT，则不做LOG。

    if(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_PAUSE)
        return 1;
    EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_DATA);

    if(CommonFunc::uc_Support_AUX)
    {
//         if(!ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bSdlEnabled)
        CANBusParse::IV_TriggerLogToAUX(uc_ChannelNo);
    }
//     if(!ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bSdlEnabled)
//     {
    if(CANBusParse::CANBMS_bWork)
        EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_BMS_DATA);
//     }

    return 1;
}
int Step_Manager_task::SendExtraLog(unsigned char uc_ChannelNo)
{
    EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_DATA);

    if(CommonFunc::uc_Support_AUX)
    {
//         if(!ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bSdlEnabled)
        CANBusParse::IV_TriggerLogToAUX(uc_ChannelNo);
    }
    return 1;
}

int Step_Manager_task::SendLastLogforCSV(unsigned char uc_ChannelNo)
{
    CANBusParse::lastCANBMSData[uc_ChannelNo] = true;
    EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_BMS_DATA);
    Smart_Battery::SetSMBusLastLog(uc_ChannelNo, LASTPOINT);

    ChannelInfo_Manager::SetDataFlag(uc_ChannelNo, STOP_POINT);
    EthernetParse::uc_LogCriticalPoint = LOG_CRITICAL_DATA_POINT;
    EthernetParse::MPReportData(uc_ChannelNo, SEND_LOG_DATA);
    CANBusParse::IV_TriggerLastLogToAux(uc_ChannelNo);

}
/*****************************************************
// Function Name:Log_SMBusData
// Version:       V1.0
// Date:                2019/03/08
// Description:
//                 log smbus数据
// Others:
//
// History：
//
// ******************************************************/
int Step_Manager_task::Log_SMBusData(unsigned char uc_ChannelNo) //add by zc 03.04.2019
{
    ST_SMBDATABUF mSmbBufDataLog;
    for(unsigned char uc_i=0; uc_i<Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount; uc_i++)
    {
        if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Logging_Interval == SYNC_WITH_IV)
        {
            MsTime *SMBus_TimeBuffer = &Smart_Battery::m_PreviousLogUploadTime[uc_i];
            if( Smart_Battery::FirstLog_Flag[uc_i] || (CommonFunc::GetTimeTicks(SMBus_TimeBuffer) >= 9800) || Smart_Battery::LastLog_Flag[uc_i])//zyx 2110091
            {
                Smart_Battery::UploadSMBusData(uc_i, mSmbBufDataLog, SEND_LOG_SMB_DATA);//add by zc 04/01/2019
            }
        }

        if( (Smart_Battery::FirstLog_Flag[uc_i]) && (Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh == ONE_TIME) ) //add by zc 03.04.2019
        {
            Smart_Battery::UploadSMBusData(uc_i, mSmbBufDataLog, SEND_LOG_SMB_DATA);//add by zc 04/01/2019
        }
    }
    return 1;
}
/*****************************************************
// Function Name:Log_IdleSMBusData
// Version:       V1.0
// Date:                2019/03/08
// Description:
//                 log idle smbus数据
// Others:
//
// History：
//
// ******************************************************/
int Step_Manager_task::Log_IdleSMBusData(unsigned char uc_ChannelNo,unsigned char uc_i) //add by zc 03.04.2019
{
//     if(!ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_bSdlEnabled)//add by zc 03.04.2019
//     {
    ST_SMBDATABUF mSmbBufDataLog;
    if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh != ONE_TIME)//add by zc 03.04.2019
    {
        MsTime *SMBus_TimeBuffer = &Smart_Battery::m_PreviousIdleUploadTime[uc_i];
        if(CommonFunc::GetTimeTicks(SMBus_TimeBuffer) >= 10000)
        {
            Smart_Battery::UploadSMBusData(uc_i, mSmbBufDataLog, SEND_IDLE_SMB_DATA);//add by zc 04/01/2019
        }
    }
//     }
    return 1;
}

void Step_Manager_task::Pulse_End(unsigned char uc_ChannelNo)
{
    if ( Pulse::Pulse_is_Running)  //如果有脉冲输出的话，尽快停止输出脉冲。
    {   Pulse::End(uc_ChannelNo);
        Pulse::Pulse_is_Running = false ;
    }
}

void Step_Manager_task::SimuSamp_SendExtraLog(unsigned char uc_ChannelNo)
{
    if((!ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bStageLogged)
            && ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bLogCycle)
    {
        unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_StageDacTime);
        if(TimeDiff < board_config::DacInfo.m_uSettingTime)
            ChannelInfo_Manager::Isamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.SimuSamp = 0;
        else
        {
            if(ChannelInfo_Manager::Isamp_NeedRecalculateSign[uc_ChannelNo].m_Bits.SimuSamp == 1)
            {
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SimuInfo.m_bStageLogged = 1;
                Step_Manager_task::SendExtraLog(uc_ChannelNo);
            }
        }
    }
}

void Step_Manager_task::ClearStepMsg(unsigned char uc_ChannelNo, unsigned char MsgIndex)
{
    St_StepMessage GetMessage;
    GetMessage = StepMessageBuf[MsgIndex][uc_ChannelNo];  // 取得Message响应信息
    DoStepMessageBuf[uc_ChannelNo].StepMessage = GetMessage;
    MsgBufManage[uc_ChannelNo].FullCount = 0; //收到Stop信息取出，其余信息一律不要；
    MsgBufManage[uc_ChannelNo].DeletingCount = 0;
    MsgBufManage[uc_ChannelNo].EmptyCount = 7;
}

void Step_Manager_task::StepDAC_Out(unsigned char uc_ChannelNo, unsigned long Code, unsigned char Type)
{
    IR::DAC_Flag = 1; //  20130927 何
    ( *board_config::Func_Dac_out)(uc_ChannelNo, Code, Type);
    IR::DAC_Flag = 0;
}

void Step_Manager_task::StepClear_Adjust(unsigned char uc_ChannelNo)
{
    ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_TotalAdjust = 0;   // 清一次微调参数重新开始计数
    ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastOutput = 0;
    ChannelInfo_Manager::m_CurFineTune_Info[uc_ChannelNo].m_LastSetValue = 0;
}

void Step_Manager_task::AUXAckFailRoutine(unsigned char uc_ChannelNo)
{
    if(AuxLimitAckMgr[uc_ChannelNo].bAuxAckFailed)
        return;
    AuxLimitAckMgr[uc_ChannelNo].bAuxAckFailed = 1;

    unsigned char uc_AuxIdx = 0;
    for(uc_AuxIdx = 0; uc_AuxIdx < MAXAUXUNIT; uc_AuxIdx++)
    {
        if(CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][uc_AuxIdx].m_auxStatus != AUX_STATUS_NORMAL)
            return;

    }
    St_ScheduleMessage SchMessage;
    memset((void *)&SchMessage, 0, sizeof(St_ScheduleMessage));
    SchMessage.MsgNumber = AUX_ACKFAIL_Msg;
    SchMessage.st_status_sch_running.m_btCMD_Source =  ST_CANPARSE;
    SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
    SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_ACK_FAILED;
    safety_check::SetUnsafe(uc_ChannelNo,AUX_ACK_FAILED,0, AUX_UNSAFE_TYPE);
    for(unsigned char uc_i = 0; uc_i < CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNCount; uc_i++)
    {
        if(!CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNInfo[uc_i].m_Checked)
        {
            if(CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNInfo[uc_i].m_IfAnyLimit)
            {
                for(unsigned char uc_k = 1; uc_k < 64; uc_k++) // unsigned long long m_AuxUintBit;
                {
                    if((CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNInfo[uc_i].m_AuxUintBit >> uc_k) & 0x01)
                        SchMessage.st_status_sch_running.m_btLost_Unit = uc_k;
                }
            }
            else
                SchMessage.st_status_sch_running.m_btLost_Unit = CANBusParse::st_SNCheckTab[uc_ChannelNo][LIMITSN].m_SNInfo[uc_i].m_AuxUnitID;
            break;
        }
    }

    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = SchMessage.st_status_sch_running.m_uw32Safe_Code;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_UnsafeType = SchMessage.st_status_sch_running.m_bUnsafeType;
    Schedule_Manager_task::UnsafeContent[uc_ChannelNo] = SchMessage;
    Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo, SchMessage); //??aux Unit?????IV??

}

unsigned char Step_Manager_task::IfParaModStepRunning(unsigned char uc_ChannelNo)
{
    switch(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].StepStatus)
    {
    case STEPTRANSITION:
    case STEPRUNNING:
        return 1;
    default:
        return 0;
    }

}
void Step_Manager_task::GetNextIRangeValue(unsigned char uc_ChannelNo)
{
    float Imax_NextRang;
    unsigned char uc_Irange=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
    unsigned char uc_LowRange =uc_Irange+1;     //切换到小一级的电流量程
    m_CurRangePara[uc_ChannelNo].m_CurrengRange_Value = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,uc_Irange) *AUTO_RANGE_PERCENT_HIGH;
    if(uc_LowRange < ChannelInfo_Manager::m_st_BoadType.m_ucIRangeCount)
    {
        m_CurRangePara[uc_ChannelNo].m_NextLowRange_Value = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,uc_LowRange) * AUTO_RANGE_PERCENT_LOW;
    }
    else
        m_CurRangePara[uc_ChannelNo].m_NextLowRange_Value = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,ChannelInfo_Manager::m_st_BoadType.m_ucIRangeCount-1)* AUTO_RANGE_PERCENT_LOW;

    if(ChannelInfo_Manager::BoardCFG.m_Global.bOne2TwoWay)  //双向的，
    {
        m_CurRangePara[uc_ChannelNo].m_bCheckRangeSwitchNeededFlag = true;
    }
    else     //单向的
    {

        if((uc_Irange+1) <ChannelInfo_Manager::m_st_BoadType.m_ucIRangeCount)       //ChannelInfo_Manager::m_st_BoadType.m_ucIRangeCount
        {
            m_CurRangePara[uc_ChannelNo].m_bCheckRangeSwitchNeededFlag = true;
        }
        else
        {
            m_CurRangePara[uc_ChannelNo].m_bCheckRangeSwitchNeededFlag =false;
        }
    }
}

float Step_Manager_task::EstimateIRValue(unsigned char uc_ChannelNo)
{   //zyx 2106241
    float IR = 0.0f;
    float MaxVolt = 0.0f;
    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_TestObject.m_fMaxAllowVoltage == 0)
    {
        unsigned char VoltRange = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange;
        MaxVolt = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_High;
    }
    else
        MaxVolt = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_TestObject.m_fMaxAllowVoltage;

    long long int SeriesCount = (long long int )( abs(MaxVolt) / SINGLE_BATTARY_VOLT );
    if(SeriesCount <= 0)
        SeriesCount = 1;
    float TargetI = abs(CalculateCurrent(uc_ChannelNo));
    if(TargetI > 0)
        IR = abs(0.079f * powf(TargetI, -1.131f)) * SeriesCount;
    return IR;
}

float Step_Manager_task::Get_IR_Value(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx;
    ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    ST_IR_Info* p_IR_Info = &ChannelInfo_Manager::m_IR_Info[uc_ChannelNo];
    float IR_Value = 0;
    ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_UNKNOW;
    if(p_IR_Info->IR_From.m_bIR_from_Schedule)  //From Schedule
    {
        IR_Value = p_IR_Info->m_fIR_by_Schedule;
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_SCH;
    }
    else if(p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_WRM && p_IR_Info->IR_From.m_bIR_from_Relax)
    {
        IR_Value =  p_IR_Info->m_fIR_by_Relax;   //from CCCV Relax
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_CCCV_WRM;
    }
    else if(p_IR_Info->IR_From.m_bIR_from_IRStep)  //from IR step
    {
        IR_Value =  p_IR_Info->m_fIR_by_IRStep;
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_STEP;
    }
    //======================BY ZC 08232017==============================
    else if(p_IR_Info->IR_From.m_bIR_from_CCCV_TB)
    {
        IR_Value =  p_IR_Info->m_fIR_by_CCCV_TB;   //from CCCV_TB
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_CCCV_TB;
    }
    //===========================END======================================
    else if(p_IR_Info->IR_From.m_bIR_from_RisingEdge)
    {
        IR_Value = p_IR_Info->m_fIR_RisingEdge;
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_CC_Rising ;
    }
    else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV)
    {
        IR_Value = p_IR_Info->m_fIR_by_CCCV;
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_CCCV_RAMP ;
    }
    else if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Estimate)
    {
        IR_Value = p_IR_Info->m_fIR_by_Estimate;
        ChannelInfo_Manager::f_IR_Sourceback[uc_ChannelNo] = SET_ESTIMATE ;
    }

    ChannelInfo_Manager::f_IR_Valueback[uc_ChannelNo] = IR_Value;
    return IR_Value;
}
void Step_Manager_task::CCCV_SampleAndCalculateRampStage(unsigned char uc_ChannelNo, float TargetV,float TargetI)
{
    f_CCCV_Ramp_V0[uc_ChannelNo]= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage);//p_Buffer->m_fVoltage; // 首次保存, 注意首点电压不能清0。
    f_CCCV_Ramp_I0[uc_ChannelNo]= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);//p_Buffer->m_fCurrent; // 首次保存
    CV2CCCV_K_gain[uc_ChannelNo] = (TargetI-f_CCCV_Ramp_I0[uc_ChannelNo])/DEF_CCCV_RESUME_STAGE_COUNT;
}

void Step_Manager_task::CCCV_Init(unsigned char uc_ChannelNo)
{
    ChannelCCCVCounter[uc_ChannelNo] = 0;
    CCCVNoLimitCheckFlag[uc_ChannelNo] = 1;//zyx 200526 nolimit before output 0A
    NewCodeRefreshed[uc_ChannelNo] = 0;
    ChannelInfo_Manager::m_NeedRCFilterFlg[uc_ChannelNo]=false;   //add by qjm  20171113
    CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);

    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Estimate = false;
    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Estimate = 0;

    if(ChannelInfo_Manager::BoardCFG.m_Global.bAutoRangeEnable)
    {
        uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Init;
    }
}

bool Step_Manager_task::IsGetNewSample(unsigned char uc_ChannelNo, unsigned char CCCV_RAMP_Status)
{
    if ((ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) || ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))\
            && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)\
            && ((CCCV_RAMP_Status == RAMP_IR_CALCULATE) || (CCCV_RAMP_Status == RAMP_INIT)))
    {
        //parallel communication always laster than single channel sample. So it need delay
        NewCodeRefreshed[uc_ChannelNo] = 0;
        if( CommonFunc::GetTimeTicks( &m_CCCV_Ramp_ParaModTime[uc_ChannelNo]) >= MAX_CCCV_PARAMOD_TIME)  // 10ms
        {
            CommonFunc::GetTimeTicksUpdate(&m_CCCV_Ramp_ParaModTime[uc_ChannelNo]);
            return true;
        }

    }
    else
    {   //PWM rising time is 10ms, and sample may 400us, the sample must delay.
        if(NewCodeRefreshed[uc_ChannelNo] \
                && CommonFunc::GetTimeTicks(&CCCV_RisingTime[uc_ChannelNo]) > board_config::DacInfo.m_uRisingTime)
        {
            NewCodeRefreshed[uc_ChannelNo] = 0;
            CommonFunc::GetTimeTicksUpdate(&CCCV_RisingTime[uc_ChannelNo]);
            return true;
        }
    }
    return false;

}


bool Step_Manager_task::IsCCCV_VoltageArrived(unsigned char uc_ChannelNo,float TargetV,float TargetI)
{
    float SampleV = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage);
    if(((TargetI >= 0.0f)&&(SampleV >= TargetV))       //Charge process
            ||((TargetI < 0.0f)&&(SampleV <= TargetV)))    //Discharge process
    {
        return true;
    }
    return false;
}

void Step_Manager_task::CCCV_RampCalculateIR(unsigned char uc_ChannelNo)
{
    float Voltage_Difference = 0;
    float Current_Difference =  0;
    float temp_IR = 0;
    St_IV_Buffer1 *	p_Buffer =&ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo];
    float MinVoltDiff = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange].m_Bound.m_High * CCCV_Ramp_VoltDifference;
    MinVoltDiff = abs(MinVoltDiff);
    Voltage_Difference = abs(p_Buffer->m_fVoltage-f_CCCV_Ramp_V0[uc_ChannelNo]);
    if(Voltage_Difference >= MinVoltDiff)
    {
        Current_Difference = abs(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current)-f_CCCV_Ramp_I0[uc_ChannelNo]);

        temp_IR = Voltage_Difference/Current_Difference;
        if(temp_IR >0)
        {
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_CCCV = temp_IR;
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_CCCV = true;
        }
    }
}

void Step_Manager_task::RapidlyCommunicateInit(void)
{
    memset(b_IsRapidlyCommun, 0, sizeof(b_IsRapidlyCommun));
    CommonFunc::GetTimeTicksUpdate(&m_RepidlyCommunOverTimeCheck);
}

void Step_Manager_task::CheckRapidlyCommunicate(void)
{
    if((IsNoChnNeedRapidlyCommunicate() || CommonFunc::GetTimeTicks(&m_RepidlyCommunOverTimeCheck) >  RAPIDLY_COMMUN_OVERTIME )
            && MultiMcuComm::uw32_MaxTime_Confirm == MCU_MAX_SEND_CONFIRM1_RAPIDLY_COMMUNICATE)
    {

        MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_NORMAL;
        CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK;
        for(int chnIdx = 0; chnIdx < MAXCHANNELNO; chnIdx++)
        {
            ClearRapidlyCommun(chnIdx);
            IVSDL::SetFactor_Time(chnIdx, CANBusParse::f_DeltaValueTime_back[chnIdx]);
        }
        CommonFunc::GetTimeTicksUpdate(&m_RepidlyCommunOverTimeCheck);
    }
}

bool Step_Manager_task::IsNoChnNeedRapidlyCommunicate(void)
{
    for(int chnIdx = 0; chnIdx < MAXCHANNELNO; chnIdx++)
    {
        if(b_IsRapidlyCommun[chnIdx])
            return false;
    }
    return true;
}


void Step_Manager_task::ContorlChnSampleFilter(unsigned char chn, bool IsChnNeedFilter,bool IsRapidlyCommunicate)
{
    if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(chn) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(chn)) //TODO: need to do anything for intra unit parallel here??
    {   //intra
        int uIntraLatstChanIdx = ChannelInfo_Manager::FindGroupLastChanIdx(chn);
        for(int chnIdx = chn; chnIdx < uIntraLatstChanIdx; chnIdx++)
//             ChannelInfo_Manager::ChannelInfo[chnIdx].m_bNeedFilter = IsChnNeedFilter;
            ChannelInfo_Manager::SetFilterStatus(chnIdx, IsChnNeedFilter);
    }
    else if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(chn)&& ChannelInfo_Manager::IsCurrentRangeParallelHigh(chn))
    {   //inter
        ST_UNIT_GROUP_INFO *ParaChnInfo = &ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo;
        CANBusParse::Parallel_MasterSend_SetFilter(IsChnNeedFilter,IsRapidlyCommunicate);
        for(int chnIdx = ParaChnInfo->m_HeadChn; chnIdx < ParaChnInfo->m_InUnitGroupSize+ParaChnInfo->m_HeadChn; chnIdx++)
        {
//             ChannelInfo_Manager::ChannelInfo[chnIdx].m_bNeedFilter = IsChnNeedFilter;
            ChannelInfo_Manager::SetFilterStatus(chnIdx, IsChnNeedFilter);
            if(IsRapidlyCommunicate)
                SetRapidlyCommun(chnIdx);
            else
                ClearRapidlyCommun(chnIdx);

        }
        if(IsRapidlyCommunicate)
        {
            MultiMcuComm::uw32_MaxTime_Confirm = MCU_MAX_SEND_CONFIRM1_RAPIDLY_COMMUNICATE;
            CANBusParse::uw32_MaxTime_Para_UpdateOut = PARA_UPDATEOUT_TICK_RAPIDLY_COMMUNICATE;
            CommonFunc::GetTimeTicksUpdate(&Step_Manager_task::m_RepidlyCommunOverTimeCheck);
        }
    }
    else
    {   //single
//         ChannelInfo_Manager::ChannelInfo[chn].m_bNeedFilter = IsChnNeedFilter;
        ChannelInfo_Manager::SetFilterStatus(chn, IsChnNeedFilter);
    }
}

void Step_Manager_task::CCCV_CV_Init(unsigned char uc_ChannelNo, float TargetV, float SampleV)
{
    Ctrl_Pid_Coef * p_Pid = &ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo];
    St_ChannelInfo*	p_Info = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    St_CtrlValueCheck* p_CheckValue = &safety_check::CtrlValueCheck[uc_ChannelNo];

    p_Info->DigitCtrl=1;
    p_Pid->m_PidTypeFlag = PID_V;
    p_Pid->m_SetValue_1B = TargetV;//ChannelInfo_Manager::UnifyVoltage(uc_ChannelNo, p_Info->m_ucVoltageRange, TargetV);
    p_Pid->m_fLastErr_1B = p_Pid->m_SetValue_1B - SampleV;//ChannelInfo_Manager::UnifyVoltage(uc_ChannelNo, p_Info->m_ucVoltageRange, SampleV);//记录进入PID初始值， 09.09.2015

    CommonFunc::GetTimeTicksUpdate(&VoltOutputInterval[uc_ChannelNo]);

    float  IR_Value = Get_IR_Value(uc_ChannelNo);
    if(IR_Value <= 0)
    {
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Estimate = EstimateIRValue(uc_ChannelNo);
        if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_Estimate > 0)
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Estimate = true;

    }

    ChannelInfo_Manager::SetPV_CV_stagetime(uc_ChannelNo);//161019 zyx //161123
    p_CheckValue->CheckType = CtrlValueCheck_Voltage;
    p_CheckValue->Counter   = 0;
    CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;
    bCCCV_Initiated[uc_ChannelNo] = false;//0;

}
#if (IV_AUTORANGE == 1)
void Step_Manager_task::AutoRangeSwtichControl(unsigned char uc_ChannelNo)
{
    float Current;
    unsigned char uc_ChangeToRange;
    unsigned long time;
    if(!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())   //暂时不支持并联通道的量程切换
        return;
    switch(uc_AutoRangeStatus[uc_ChannelNo])
    {
    case AutoRange_Init:
        GetNextIRangeValue(uc_ChannelNo);
        if(m_CurRangePara[uc_ChannelNo].m_bCheckRangeSwitchNeededFlag)
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Check;
        else
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_NoNeedSwitch;
        break;
    case AutoRange_Check:
        Current = abs(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current));
        if(Current<m_CurRangePara[uc_ChannelNo].m_NextLowRange_Value)
        {
            AutoRangeSwitchStartTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
            uc_AutoRangeStatus[uc_ChannelNo]= AutoRange_WaitforChangeLow;
        }
        if(ChannelInfo_Manager::BoardCFG.m_Global.bOne2TwoWay)
        {
            if(Current>m_CurRangePara[uc_ChannelNo].m_CurrengRange_Value)
            {
                AutoRangeSwitchStartTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
                uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_WaitforChangeHigh;
            }
        }
        break;
    case AutoRange_WaitforChangeLow:
        Current = abs(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current));
        if(Current>=m_CurRangePara[uc_ChannelNo].m_NextLowRange_Value)
        {
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Check;
            AutoRangeSwitchStartTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
            break;
        }
        time = CommonFunc::GetTimeTicks(&AutoRangeSwitchStartTime[uc_ChannelNo]);
        if(time>AUTO_RANGE_STABILIZATION_TIME)	//稳定时间
        {
            unsigned char uc_CurrentRange=ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
            for(unsigned char  uc_range=ChannelInfo_Manager::m_st_BoadType.m_ucIRangeCount-1; uc_range> uc_CurrentRange; uc_range--)
            {
                if(Current < ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,uc_range) *AUTO_RANGE_PERCENT_LOW)
                {
                    uc_ChangeToRange = 	uc_range;
                    break;
                }
            }
            SetAutoRange_Disposabletask(uc_ChannelNo,uc_ChangeToRange);
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Switch;
        }
        break;
    case AutoRange_WaitforChangeHigh:
        Current = abs(ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current));
        if(Current<m_CurRangePara[uc_ChannelNo].m_CurrengRange_Value)
        {
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Check;
            AutoRangeSwitchStartTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
            break;
        }
        time = CommonFunc::GetTimeTicks(&AutoRangeSwitchStartTime[uc_ChannelNo]);
        if(time>AUTO_RANGE_STABILIZATION_TIME)	//稳定时间
        {
            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange>=1)
                uc_ChangeToRange =ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange-1;
            else
            {
                uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Check;
                AutoRangeSwitchStartTime[uc_ChannelNo] = CommonFunc::GetSystemTime();
                break;
            }
            SetAutoRange_Disposabletask(uc_ChannelNo,uc_ChangeToRange);
            uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Switch;
        }
        break;
    case AutoRange_Switch:
        AutoRange_TransitionRoutine(uc_ChannelNo);
        break;
    case AutoRange_NoNeedSwitch:
        break;

    }
}

void Step_Manager_task::SetAutoRange_Disposabletask(unsigned char uc_ChannelNo, unsigned char uc_IRange)
{
    unsigned long DacCode;
    float f_Set;
    unsigned char activeIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_ChannelInfo		*p_Info	=&ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];
    St_DisposableMSG	*p_Msg	=&StepDisposableMSG[uc_ChannelNo];
    unsigned char Type = CT_REST;
    if(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        p_Msg->AutoCaliFlag = 0;
        GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange = uc_IRange;
        p_Msg->next_Range = GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
        p_Msg->last_Range = p_Info->m_ucCurrentRange;
        p_Msg->next_VRange = p_Info->m_ucVoltageRange;
        p_Msg->last_VRange = p_Info->m_ucVoltageRange;
        p_Msg->RangeSet = 1;

        p_Msg->Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetAutoRangeSwitch.st_Disposabletask_ptr;
        p_Msg->Disposable_task.stepLen = OS_LogicProcess::SetAutoRangeSwitch.stepLen;
        p_Msg->Disposable_Flag = DISPOSABLE_to_CCHN;
        p_Msg->TargetCtrlType = CT_CURRENT;  // 20200810 yy
        p_Msg->Dac_Value    = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,
                              uc_IRange,
                              ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue);    //采到什么值输出什么值

        if(p_Info->m_ucCurrentRange != GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_ucCurrentRange)
            p_Msg->RangeSet = 1; //
        p_Msg->msgID = STEP_TASK;
        p_Msg->CH_Number = uc_ChannelNo;
    }
}
void Step_Manager_task::AutoRange_TransitionRoutine(unsigned char uc_ChannelNo)
{
    St_ChannelInfo * p_Info = &ChannelInfo_Manager::ChannelInfo[uc_ChannelNo];

    unsigned char  ActiveStep = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_ThisControl    =    &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveStep].m_StepInfo.m_Data.m_Ctrl;
    unsigned char LogicProcessStatus = 0;
    if (p_Info->EngineStatus == 0) //引擎没有调用
    {
        EngineHandle[uc_ChannelNo] = OS_LogicProcess::requestLogicPorcess(uc_ChannelNo, &StepDisposableMSG[uc_ChannelNo]);
        b_AutoRangeRequestFlag[uc_ChannelNo]=true;
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 1;
        if(EngineHandle[uc_ChannelNo] == 0)
        {
            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].EngineStatus = 0;
            //Error 若出现溢出现象
            return;    //
        }
    }
    if(b_AutoRangeRequestFlag[uc_ChannelNo])
    {
        LogicProcessStatus = OS_LogicProcess::getHandleStatus(EngineHandle[uc_ChannelNo]);
    }
    if(LogicProcessStatus ==DISPOSABLE_FINISHED) //运行完毕
    {
        float fTargetCurrent = CalculateCurrent(uc_ChannelNo);
        p_Info->EngineStatus = 0;
        p_Info->m_fCurrentRangeOfPid = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,p_Info->m_ucCurrentRange);  //p_Info->m_ucCurrentRange
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,  fTargetCurrent);//uCurrRng,
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = abs(Pid_Ctrl::fUpLimit[uc_ChannelNo]);

        ChannelInfo_Manager::m_Pid_Coef[uc_ChannelNo].m_LastOutput_1B = ChannelInfo_Manager::UnifyCurrent(uc_ChannelNo,
                (ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current)));//

        m_Range[uc_ChannelNo] =p_Info->m_ucCurrentRange;
        b_AutoRangeRequestFlag[uc_ChannelNo]=false;
        uc_AutoRangeStatus[uc_ChannelNo]=AutoRange_Init;
    }
}
#endif