#include "../Entry/includes.h"

//#if _BOARD_TYPE_  == BOARDTYPE_IBT
//#if _IBT_VERSION_ == IBT_VER_D
//const unsigned char g_chn_order[MAXCHANNELNO] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
//#else
//const unsigned char g_chn_order[MAXCHANNELNO] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
// unsigned char g_chn_order[MAXCHANNELNO] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//#endif
//#else
//const unsigned char g_chn_order[MAXCHANNELNO] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//#endif


//comment out chen
St_Disposable OS_LogicProcess::SetCtrlRstToCur;
St_Disposable OS_LogicProcess::SetCtrlRstToVlt;
St_Disposable OS_LogicProcess::SetCtrlRstToOff;

St_Disposable OS_LogicProcess::SetCtrlOffToRst;
St_Disposable OS_LogicProcess::SetCtrlOffToCur;
St_Disposable OS_LogicProcess::SetCtrlOffToVlt;

St_Disposable OS_LogicProcess::SetCtrlCurToCur;
St_Disposable OS_LogicProcess::SetCtrlCurToVlt;
St_Disposable OS_LogicProcess::SetCtrlCurToOff;
St_Disposable OS_LogicProcess::SetCtrlCurToRst;

St_Disposable OS_LogicProcess::SetCtrlVltToVlt;
St_Disposable OS_LogicProcess::SetCtrlVltToCur;
St_Disposable OS_LogicProcess::SetCtrlVltToOff;
St_Disposable OS_LogicProcess::SetCtrlVltToRst;

St_Disposable OS_LogicProcess::SetCtrlVltToExt;
St_Disposable OS_LogicProcess::SetCtrlExtToVlt;
St_Disposable OS_LogicProcess::SetAutoCaliOpen;    //自动校准开
St_Disposable OS_LogicProcess::SetAutoCaliClose;   //自动校准关
St_Disposable OS_LogicProcess::SetCtrlAcimOn;    //ACIM开
St_Disposable OS_LogicProcess::SetCtrlAcimOff;   //ACIM关

St_Disposable OS_LogicProcess::SetCaliBoardRstToVlt;
St_Disposable OS_LogicProcess::SetCaliBoardVltToOff;
St_Disposable OS_LogicProcess::SetCaliBoardVltToVlt;

St_Disposable OS_LogicProcess::SetVoltRelay;   //支持单电压动作 用于冗余电压处理20140827 dirui
#if (IV_AUTORANGE == 1)
#endif
St_Disposable  OS_LogicProcess::SetAutoRangeSwitch;
UInt16                 OS_LogicProcess::Priority = PRIORITY4;//commnet out chen 1121
UInt16                 OS_LogicProcess::TaskID = DISPOSABLE_TASK;
volatile unsigned char OS_LogicProcess::Logic_For_Safe[MAXCHANNELNO];    //转换状态    瞬态  稳态

St_Disposable_G_Que    OS_LogicProcess::m_RunningTask[MAXCHANNELNO];
unsigned short         OS_LogicProcess::handle_index;    //索引  1-65535
qDisposTaskBuf         OS_LogicProcess::m_qHandle;

unsigned char          OS_LogicProcess::m_bActive[MAXCHANNELNO];
bool				   OS_LogicProcess::m_bSlaveActive[MAXCHANNELNO];
unsigned char          OS_LogicProcess::m_bQueueBusy[MAXCHANNELNO];
// unsigned long int      OS_LogicProcess::Set_DAC_Output_MIN = DAC_OUT_PUT_ZERO; // 设置一个微值ISET提升过死区
unsigned char 		   OS_LogicProcess::LogicStatus[MAXCHANNELNO];
MsTime 				   OS_LogicProcess::Logic_last_time[MAXCHANNELNO];
unsigned char 			OS_LogicProcess::LogicCtrlType;
bool 					OS_LogicProcess::be_NeedWaitflg;
bool 					OS_LogicProcess::be_StartThirdParty;
unsigned char 			m_UnitId=0;
unsigned char  			FindNextChannelID=0;

void OS_LogicProcess::Init(void)
{
//     if (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_IBT && ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_D)
//     {
//        for( int i=0; i<MAXCHANNELNO; i++)
//        {
// 	        g_chn_order[i] = 15-i;
//        }
//     }
    handle_index = 1;

    memset(&m_qHandle,0,sizeof(m_qHandle));
    INIT_QUEUE(m_qHandle);
    LogicCtrlType=DO_NA;
	  be_StartThirdParty = false;
    for(unsigned char ChannelIdx = 0; ChannelIdx<MAXCHANNELNO; ChannelIdx++)
    {
        Logic_For_Safe[ChannelIdx] = LOGIC_PROCESS_STEADY;

        memset(&m_RunningTask[ChannelIdx], 0, sizeof(St_Disposable_G_Que));

        m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag = DISPOSABLE_to_NONE;    //none
        m_RunningTask[ChannelIdx].logicProcessMSg.TargetCtrlType = CT_REST;  // 20200810 yy
        m_bActive[ChannelIdx] = 0;
        m_bQueueBusy[ChannelIdx] = 0;
        m_bSlaveActive[ChannelIdx] = 0;
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_State = LOGIC_PROCESS_CHN_CLOSE;
		memset(&CAN_WatchdogTask::st_chan_state[ChannelIdx],0,sizeof(CAN_WatchdogTask::st_chan_state[ChannelIdx]));
    }
}

//那么没执行完的时候接受msg不响应
void OS_LogicProcess::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_LOGIC] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_LOGIC);
#endif
    for(unsigned char ChannelIdx=0;
            ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
            ChannelIdx++)
    {
        if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode!=eMixMode)
        {
            if(m_bActive[ChannelIdx])
                ProcessActive(ChannelIdx);
        }
        else
            ProcessActive(ChannelIdx);
    }
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_LOGIC] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_LOGIC);
#endif
}

int OS_LogicProcess::ProcessActive(unsigned char ChannelIdx)
{
    int result = 0;

    MsTime TimeDiff;

    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode==eMixMode)
    {
        switch(LogicStatus[ChannelIdx])
        {
        case LOGIC_NOTREADY:
            CommonFunc::GetTimeTicksUpdate(&Logic_last_time[ChannelIdx]);
            FindNextChannelID=0;
            break;
        case LOGIC_IDLE:
						FindNextChannelID=0;
				    if(be_StartThirdParty == false)
						{
            for(unsigned char i=0; i<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount-1; i++)
            {
                unsigned char m_Id=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[i].m_Unit_Idx+1;
                if(CANBusParse::m_SubACKUnitFlg[m_Id]!=true)
                    return 0;
							}
            }

            m_UnitId=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[0].m_Unit_Idx;


            if(OS_LogicProcess::LogicCtrlType==DO_RUN && OS_LogicProcess::be_NeedWaitflg)// 开通道时最后开主通道，关闭则相反
                LogicStatus[ChannelIdx]=LOGIC_WAIT;
            else
            {
                TimeDiff = CommonFunc::TimeDiff(Logic_last_time[ChannelIdx]);

                if((ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus==IDLE)
									||(ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus==NOTREADY)
				   ||(ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus==FIXEDUNSAFE)
				   ||(ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus==DYNAMICUNSAFE))// 需判断主通道彻底完成后才能处理从通道
                {
                    CommonFunc::GetTimeTicksUpdate(&Logic_last_time[ChannelIdx]);
                    LogicStatus[ChannelIdx]=LOGIC_WAIT;
                    return 0;
                }
                else
                {

                    break;
                }
            }


            return 0;
        case LOGIC_WAIT:
            TimeDiff = CommonFunc::TimeDiff(Logic_last_time[ChannelIdx]);
            if(TimeDiff.Second<1)
            {
                return 0;
            }
            else
            {
// 						if(m_UnitId<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount)
// 						{
// 								m_UnitId++;
// 								LogicStatus[ChannelIdx]=LOGIC_TRIGGER;
// 						}
                if(FindNextChannelID<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount-1)
                {
                    m_UnitId=FindNextSubChannel(0);
                    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount-FindNextChannelID-1].m_beSlaveUnit>=eSeriesBoosterRank_Serial && OS_LogicProcess::LogicCtrlType==DO_RUN)  //Slave unit  执行延时启动  暂时按照Unit顺序排列处理
                        LogicStatus[ChannelIdx]=LOGIC_TRIGGER;
                    else if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[FindNextChannelID-1].m_beSlaveUnit>=eSeriesBoosterRank_Serial && OS_LogicProcess::LogicCtrlType==DO_STOP)  //Slave unit  执行延时启动  暂时按照Unit顺序排列处理
                        LogicStatus[ChannelIdx]=LOGIC_TRIGGER;
                    else
                    {
                        //	FindNextChannelID++;
                        return 0;
                    }
                }
                else
                {
// 							if(FindNextChannelID>=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount)
// 							{
                    OS_LogicProcess::be_NeedWaitflg=false;
                    LogicStatus[ChannelIdx]=LOGIC_END;

                    FindNextChannelID=0;
// 							}

                }
            }

            return 0;
        case LOGIC_TRIGGER:
            float m_SlaveCtrlValue;

            unsigned char u16ControlType =(unsigned char)CT_IVCH_VOLTAGE; //ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;


            bool b_Needfilter = CANBusParse::SubMasterIfFilter(ChannelIdx,u16ControlType); //根据控制类型决定是否使用滤波

            if(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_VOLTAGE)//需加入其他电压类型
                m_SlaveCtrlValue=ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
            else
                m_SlaveCtrlValue=ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
            if(OS_LogicProcess::LogicCtrlType==DO_RUN)// && ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_CtrlState<CCS_I) //切换状态时不能动从通道的恒压动作
            {   //	CANBusParse::Parallel_MasterSend_RUN(m_UnitId+1,    \  /*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount+2-m_UnitId,    */
                CANBusParse::Parallel_MasterSend_RUN(m_UnitId+1,b_Needfilter, \
//				u16ControlType,m_SlaveCtrlValue,m_MixModCtrlPara[ChannelIdx].m_ucVoltageRange);     //comment out 03.21.2018 CAN传输电压
                                                     u16ControlType,m_SlaveCtrlValue,ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucVoltageRange);
                for(unsigned char uc_i  = 0; uc_i<ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter; uc_i++) //记录series和booster控制电压
                {
                    if(ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_Unit_Idx == m_UnitId)
                    {
                        ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].ControlVoltage = m_SlaveCtrlValue;
                    }
                }
            }
            else if(OS_LogicProcess::LogicCtrlType==DO_STOP)
            {
                CANBusParse::Parallel_MasterSend_DoStop(m_UnitId+1);
            }
            else
            {
                LogicStatus[ChannelIdx]=LOGIC_NOTREADY;
                break;
            }
            CommonFunc::GetTimeTicksUpdate(&Logic_last_time[ChannelIdx]);
            LogicStatus[ChannelIdx]=LOGIC_WAIT;

            return 0;

        case LOGIC_END:
            LogicStatus[ChannelIdx]=LOGIC_NOTREADY;

            break;
        default:
            break;

        }

        if((OS_LogicProcess::LogicCtrlType==DO_RUN)&&(!m_bSlaveActive[0]))
        {
            if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank==0x00)
            {
                for(unsigned char uc_i  = 0; uc_i<ChannelInfo_Manager::gUnit_stUnitInfo.m_Series2BoosterUnitCounter; uc_i++) //记录series和booster控制电压
                {
                    float Series2BoosterControlValue = ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].ControlVoltage;
                    float ActualControlVoltage = ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].OutputVoltage;
                    if(ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_beSlaveUnit==eSeriesBoosterRank_Serial)
                    {
                        if(Series2BoosterControlValue<BOOSTER_MIN_VOLTAGE)
                        {
                            Series2BoosterControlValue = BOOSTER_MIN_VOLTAGE;
                        }
                        Series2BoosterControlValue/=2;

                    }
                    else if(ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[uc_i].m_SubMasterChannels.m_beSlaveUnit==eSeriesBoosterRank_Booster)
                    {
                        if(Series2BoosterControlValue>0)
                        {
                            Series2BoosterControlValue= 0;
                        }
                        Series2BoosterControlValue = abs(Series2BoosterControlValue)+BOOSTER_MIN_VOLTAGE;

                    }

                    if(abs(Series2BoosterControlValue - ActualControlVoltage)>50)
                    {
                        //没有按照控制输出，报错并返回,停止通道.

                        m_RunningTask[ChannelIdx].bActionFinished = 1;
                        memset(&PWM_Base::TransitionInfo[ChannelIdx],0,sizeof(PWM_Base::TransitionInfo[ChannelIdx]));
                        PWM_Base::TransitionInfo[ChannelIdx].bStepInited = 1;
                        Step_Manager_task::PrepareForStop(ChannelIdx);
                        ChannelInfo_Manager::ChannelInfo[ChannelIdx].EngineStatus=0;
                        ChannelInfo_Manager::ChannelInfo[ChannelIdx].StepTransationToStatus=STEPIDLE;
                        ChannelInfo_Manager::ChannelInfo[ChannelIdx].StepStatus=STEPTRANSITION;
                        Step_Manager_task::ClearStepMessage(&(Step_Manager_task::DoStepMessageBuf[ChannelIdx]));
                        ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus=GOSTOP;

                        return 0;
                    }
                }
            }
        }

        if(!m_bActive[ChannelIdx])
            return 1;

        for(unsigned char uc_i=0; uc_i<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount-1; uc_i++) //同步所有的SubMasterUnit
        {

            if(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_beSlaveUnit==eSeriesBoosterRank_SubMaster
                    && ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank==0x00)
            {
                unsigned short u16ControlType = ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[0].m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType;
                float  				 fCtrlValue=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[0].m_GroupID].m_stSubGroupCtrl.m_fSubGroupCtrlValue;
                bool b_Needfilter = CANBusParse::SubMasterIfFilter(ChannelIdx,u16ControlType); //根据控制类型决定是否使用滤波
                if(OS_LogicProcess::LogicCtrlType==DO_RUN && u16ControlType!=CT_IVCH_REST && !m_bSlaveActive[0] && u16ControlType!=CT_IVCH_SETVARIABLE )
                {
//                     u16ControlType=CT_IVCH_CURRENT;
//                     fCtrlValue=10;
                    CANBusParse::Parallel_MasterSend_RUN(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx+1,b_Needfilter,u16ControlType,				\
                                                         fCtrlValue,ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange);
//                     m_bSlaveActive[0]=1;
                }
                else if((OS_LogicProcess::LogicCtrlType==DO_STOP || u16ControlType==CT_IVCH_REST) && !m_bSlaveActive[0])
                {
                    CANBusParse::Parallel_MasterSend_DoStop(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx+1);
//                     m_bSlaveActive[0]=1;
                }
            }
        }
        m_bSlaveActive[0]=1;
        result = processDisposable(ChannelIdx);
    }
    else
        result = processDisposable(ChannelIdx);

    if(result == LOGICPROCESS_STATUS_FINISH)    //finish
    {
        m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag = DISPOSABLE_to_NONE;
        if(m_bQueueBusy[ChannelIdx])
        {
            m_RunningTask[ChannelIdx].logicProcessMSg.AutoCaliFlag = 0;
            m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
            m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr;
            m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;

            m_RunningTask[ChannelIdx].ID              = m_RunningTask[ChannelIdx].NextID;
            m_RunningTask[ChannelIdx].bActionFinished = 0;

            processDisposable(ChannelIdx);
            m_bQueueBusy[ChannelIdx] = 0;
        }
        else
        {
            m_bActive[ChannelIdx] = 0;
            setStatus(m_RunningTask[ChannelIdx].ID,DISPOSABLE_FINISHED);
        }
    }
    else if(result == LOGICPROCESS_STATUS_HANDING)
        setStatus(m_RunningTask[ChannelIdx].ID,DISPOSABLE_RUNNING);
    else if (result == LOGICPROCESS_STATUS_ERROR)
    {
        setStatus(m_RunningTask[ChannelIdx].ID,DISPOSABLE_ERROR);
        m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag = DISPOSABLE_to_NONE;    //???′

        m_bActive[ChannelIdx] = 0;
    }
    return 1;
}
unsigned char OS_LogicProcess::FindNextSubChannel(unsigned char GorupID)
{
//	if(FindNextChannelID<ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount)

    FindNextChannelID++;
    if(OS_LogicProcess::LogicCtrlType==DO_RUN)
        return ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_uGroupUnitCount-FindNextChannelID-1].m_Unit_Idx;
    else
        return ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_SubMasterChannels[FindNextChannelID-1].m_Unit_Idx;
}
signed int OS_LogicProcess::processDisposable(unsigned char ChannelIdx)
{
    DumpUnnessaryMove(ChannelIdx);	//end?¤?è′|àí

    St_Disposable_G_Que * p_RunningTask = &m_RunningTask[ChannelIdx];
    St_ChannelInfo			*	p_ChnInfo			=	&ChannelInfo_Manager::ChannelInfo[ChannelIdx];

    if(!p_RunningTask->bActionFinished)
    {
        if(p_RunningTask->logicProcessMSg.Disposable_task.st_Disposabletask_ptr->bPWM)
        {
            p_RunningTask->bActionFinished = PWM_Base::DoTransition(ChannelIdx,&p_RunningTask->logicProcessMSg);
        }
        else
        {
            DoDAC(ChannelIdx);
            DoMove(ChannelIdx);
        }
        p_RunningTask->DelayTime = p_RunningTask->logicProcessMSg.Disposable_task.st_Disposabletask_ptr->uw32_DelayTime;
        p_RunningTask->StartTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        Logic_For_Safe[ChannelIdx] = LOGIC_PROCESS_TRAN;    //comment out chen 20130627 modify
        p_ChnInfo->chn_State = LOGIC_PROCESS_TRAN;    //comment out chen 20130724
    }
    else
    {
        MsTime TimePassed;
        TimePassed = CommonFunc::TimeDiff(p_RunningTask->StartTime);//p_RunningTask->logicProcessMSg.SysTimeDisposableTask);
        if(TimePassed.Us100 >= p_RunningTask->DelayTime)
        {
            p_RunningTask->logicProcessMSg.counter++;
            p_RunningTask->logicProcessMSg.Disposable_task.st_Disposabletask_ptr++;
            p_RunningTask->bActionFinished = 0;

            if(p_RunningTask->logicProcessMSg.counter >= p_RunningTask->logicProcessMSg.Disposable_task.stepLen )//è?2?2??èê±??μ?íê3é
            {
                p_RunningTask->logicProcessMSg.counter = 0;
                Logic_For_Safe[ChannelIdx] = LOGIC_PROCESS_STEADY;    //comment out chen 20130627 modify

                if(p_RunningTask->logicProcessMSg.Disposable_Flag == DISPOSABLE_to_OPENCHN)    //′ò?aí¨μà
				{
                    p_ChnInfo->chn_State = LOGIC_PROCESS_CHN_OPEN;
					CAN_WatchdogTask::st_chan_state[ChannelIdx].m_bIsChanOpen = true; // 20191230 yy
				}
                else if(p_RunningTask->logicProcessMSg.Disposable_Flag == DISPOSABLE_to_CLOSECHN)    //1?±?í¨μà
				{
                    p_ChnInfo->chn_State = LOGIC_PROCESS_CHN_CLOSE;
					CAN_WatchdogTask::st_chan_state[ChannelIdx].m_bIsChanOpen = false; // 20191230 yy
				}
                else if(p_RunningTask->logicProcessMSg.Disposable_Flag == DISPOSABLE_to_NONE)    //1?±?í¨μà
                    p_ChnInfo->chn_State = LOGIC_PROCESS_CHN_CLOSE;
                else if(p_RunningTask->logicProcessMSg.Disposable_Flag == DISPOSABLE_to_CCHN)    //′ò?aí¨μà
                    p_ChnInfo->chn_State = LOGIC_PROCESS_CHN_OPEN;
                else
                    p_ChnInfo->chn_State = LOGIC_PROCESS_CHN_CLOSE;//LOGIC_PROCESS_CHN_OPEN; //1812281 zyx

                if(p_ChnInfo->chn_State == LOGIC_PROCESS_CHN_OPEN)
                {
                    Sample_Pro::NewCodeSampled[ChannelIdx] = 0;
					if (ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_RisingEdge_Started )//DK 210714
						ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_RisingEdge_TransFinished = true;
                    if(p_RunningTask->logicProcessMSg.RangeSet == 1)   //comment out chen 20130906  á?3ì?D??
                        p_ChnInfo->m_ucCurrentRange = p_RunningTask->logicProcessMSg.next_Range;
                }
                return LOGICPROCESS_STATUS_FINISH;//1;    //3é1|íê
            }
        }
    }

    return LOGICPROCESS_STATUS_HANDING;
}
void OS_LogicProcess::DumpUnnessaryMove(unsigned char ChannelIdx)
{
    while( m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr->RangeChangedFlag
            &&(m_RunningTask[ChannelIdx].logicProcessMSg.RangeSet == 0) )
    {
        m_RunningTask[ChannelIdx].logicProcessMSg.counter++;
        m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr++;
    }

    return;
}

void OS_LogicProcess::DoDAC(unsigned char ChannelIdx)
{
    unsigned short FlagTemp = 0;
    FlagTemp = m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr->DacMove.DacFlag;

    IR::DAC_Flag=1;

    if( (FlagTemp & ACTIVE_DAC_CUR) == ACTIVE_DAC_CUR )
    {
        switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
        {
//             case BOARDTYPE_BT2204:
        case BOARDTYPE_LBT22043:
        case BOARDTYPE_LBT22013:
        case BOARDTYPE_LBT22023:
//             case BOARDTYPE_LBT22024:
//             case BOARDTYPE_LBT22013FR:
//             case BOARDTYPE_LBT22044:
            if(BurstModeSampling::bl_NeedRunning[ChannelIdx])
            {
                BurstModeSampling::OpenBurstModeSampling(ChannelIdx);//  2017.6.8  yy
                //IVSDL::ChannelStatus[uc_ChannelNo].bNeedSDL = 0;   // 清除 BurstModeSampling 步开始的首点记录
            }
            break;
        default:
            break;
        }

        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,m_RunningTask[ChannelIdx].logicProcessMSg.Dac_Value,ACTIVE_DAC_CUR);
        //IR::DAC_Flag=0;
    }

    if( (FlagTemp & ACTIVE_DAC_VLT) == ACTIVE_DAC_VLT )  //Be set in BoardType that have Hardware Voltage Loop.
    {
        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,m_RunningTask[ChannelIdx].logicProcessMSg.Volt_DacValue,ACTIVE_DAC_VLT);
        //IR::DAC_Flag=0;
    }

    if( (FlagTemp & ACTIVE_DAC_CUR_ZERO) == ACTIVE_DAC_CUR_ZERO )
    {
        //IR::DAC_Flag=1;
//         unsigned long int uw32_value = DAC_OUT_PUT_ZERO;
//         if(Step_Manager_task::Unipolar.BoardFlag)
//             uw32_value = DAC_OUT_PUT_MIN;    // 单向功率板处理  05252016 yy
        board_config::DAC_out(ChannelIdx,DAC_OUT_PUT_ZERO,ACTIVE_DAC_CUR);    //output 0
        //IR::DAC_Flag=0;
    }

// 	if( (FlagTemp & ACTIVE_DAC_CUR_NEAR_ZERO) == ACTIVE_DAC_CUR_NEAR_ZERO)
// 	{
// 		IR::DAC_Flag=1;
//         unsigned long int uw32_value = DAC_OUT_PUT_ZERO;
//         if(Step_Manager_task::Unipolar.BoardFlag)
//             uw32_value = Set_DAC_Output_MIN;    // 单向功率板处理  05252016 yy
// 		board_config::DAC_out(ChannelIdx,uw32_value,ACTIVE_DAC_CUR);    //output 0
// 		IR::DAC_Flag=0;
// 	}

    if( (FlagTemp & ACTIVE_DAC_VLT_ZERO) == ACTIVE_DAC_VLT_ZERO )
    {
        //IR::DAC_Flag=1;
//         unsigned long int uw32_value = DAC_OUT_PUT_ZERO;
//         if(Step_Manager_task::Unipolar.BoardFlag)
//             uw32_value = Set_DAC_Output_MIN;   // 单向功率板处理  05252016 yy
//
        board_config::DAC_out(ChannelIdx,DAC_OUT_PUT_ZERO,ACTIVE_DAC_VLT);    //output 0
        //IR::DAC_Flag=0;
    }

    if(((FlagTemp & ACTIVE_DAC_VH) == ACTIVE_DAC_VH )&&(board_config::DacInfo.m_bHaveClampVdac))
    {
        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,m_RunningTask[ChannelIdx].logicProcessMSg.Clamp_Value,ACTIVE_DAC_VH);
        //IR::DAC_Flag=0;
    }

    if(((FlagTemp & ACTIVE_DAC_VL) == ACTIVE_DAC_VL )&&(board_config::DacInfo.m_bHaveClampVdac))
    {
        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,m_RunningTask[ChannelIdx].logicProcessMSg.Clamp_Value_2,ACTIVE_DAC_VL);
        //IR::DAC_Flag=0;
    }

    if( ((FlagTemp & ACTIVE_DAC_VH_DEFAULT) == ACTIVE_DAC_VH_DEFAULT )&&(board_config::DacInfo.m_bHaveClampVdac))
    {
        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);     //高钳压最大值
        //IR::DAC_Flag=0;
    }

    if( ((FlagTemp & ACTIVE_DAC_VL_DEFAULT) == ACTIVE_DAC_VL_DEFAULT )&&(board_config::DacInfo.m_bHaveClampVdac))
    {
        //IR::DAC_Flag=1;
        board_config::DAC_out(ChannelIdx,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);      //低钳压最小值
        //IR::DAC_Flag=0;
    }   

//	the following code never beign used
//	if( (FlagTemp & ACTIVE_DAC_CUR_ADC) == ACTIVE_DAC_CUR_ADC )
//	{//从ADC获取电压，然后输出此电压值的码字，都是high量程  20131101
//		float voltage = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage);	// Get Voltage
//		voltage = -voltage;  //negtive do not know why

//		unsigned long Dac_Value = CalibrateDac(voltage);    //直接计算出相同的电压    20131018  maybe有差异
//		board_config::DAC_out(ChannelIdx,Dac_Value,ACTIVE_DAC_CUR);
//	}

    IR::DAC_Flag=0;

}
void OS_LogicProcess::DoMove(unsigned char ChannelIdx)
{
    if( (m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr->RelayMove.RelaySet & ACTIVE_OUT_NEXT_RELAY) \
            == ACTIVE_OUT_NEXT_RELAY)
    {
        board_config::Func_SetRange(ChannelIdx,
                                    m_RunningTask[ChannelIdx].logicProcessMSg.next_Range,
                                    m_RunningTask[ChannelIdx].logicProcessMSg.next_VRange);
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange=m_RunningTask[ChannelIdx].logicProcessMSg.next_Range;  //dir 20200729
    }
//    ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange=m_RunningTask[ChannelIdx].logicProcessMSg.next_Range;//20140512 dirui add
    board_config::Func_SetRelay(ChannelIdx,
                                m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr->RelayMove.ActiveFlag,
                                m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_task.st_Disposabletask_ptr->RelayMove.InactiveFlag,
                                m_RunningTask[ChannelIdx].logicProcessMSg.AutoCaliFlag);

    m_RunningTask[ChannelIdx].bActionFinished = 1;
}

unsigned short OS_LogicProcess::requestLogicPorcess(unsigned char ChannelIdx,St_DisposableMSG * st_Disposable_msg_ptr)
{
    LogicProcessHandle MyHandle;

    if(m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag != DISPOSABLE_to_NONE)    //是当前是否有时序运行    DISPOSABLE_NONE
    {   //有时序的情况
        if(m_RunningTask[ChannelIdx].logicProcessMSg.Disposable_Flag == DISPOSABLE_to_CLOSECHN)    //判断当前时序是否为关闭通道时序
            return 0;     //失败
        else
        {   //判断是否需要排队
            if(st_Disposable_msg_ptr->Disposable_Flag == DISPOSABLE_to_CLOSECHN)    //关闭通道可以排队
            {
                if(m_bQueueBusy[ChannelIdx])    //已经在排队了，无法再排了，最多只能允许一个在排。
                    return 0;
                m_bQueueBusy[ChannelIdx] = 1;

                MyHandle.ID = handle_index;
                MyHandle.status = DISPOSABLE_ASSIGNED;
                ENQUEUE(m_qHandle,LOGICPROCESS_HANDLE_MAX,MyHandle);

                m_RunningTask[ChannelIdx].NextID = handle_index;    //内部维护状态
                handle_index++;
                if(handle_index >= LOGICPROCESS_HANDLE_ID_MAX)    //65535
                    handle_index = 1;     //从1开始    0代表分配失败//如果是1的话是有问题的

                return MyHandle.ID;
            }
            else
                return 0;
        }
    }
    else    //没有时序
    {
        m_RunningTask[ChannelIdx].logicProcessMSg = *st_Disposable_msg_ptr;   //赋值

        m_RunningTask[ChannelIdx].logicProcessMSg.counter = 0;
        m_RunningTask[ChannelIdx].ID = handle_index;
        m_RunningTask[ChannelIdx].bActionFinished = 0;

		if(st_Disposable_msg_ptr->Disposable_Flag == DISPOSABLE_to_OPENCHN
			||st_Disposable_msg_ptr->Disposable_Flag == DISPOSABLE_to_CCHN)  // 20191230 yy
		    CAN_WatchdogTask::st_chan_state[ChannelIdx].m_ucChanCtrl = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_CtrlState;
        MyHandle.ID = handle_index;
        MyHandle.status = DISPOSABLE_ASSIGNED;
        ENQUEUE(m_qHandle,LOGICPROCESS_HANDLE_MAX,MyHandle);                  //新加入Handle入队
        m_bActive[ChannelIdx] = 1;
        processDisposable(ChannelIdx);

        handle_index++;

        if(handle_index >= LOGICPROCESS_HANDLE_ID_MAX)    //65535，
            handle_index = 1;

        return MyHandle.ID;    //成功压入
    }
}
unsigned char OS_LogicProcess::getHandleStatus(unsigned short HandleID)
{
    for(unsigned char i = 0; i<LOGICPROCESS_HANDLE_MAX; i++)
    {
        if(m_qHandle.Object[i].ID == HandleID)
            return m_qHandle.Object[i].status;
    }
    return DISPOSABLE_DEAD;
}
void OS_LogicProcess::setStatus(unsigned short HandleID,unsigned char state)
{
    for(unsigned char i = 0; i<LOGICPROCESS_HANDLE_MAX; i++)
    {
        if(m_qHandle.Object[i].ID == HandleID)
            m_qHandle.Object[i].status = state;
    }
}

/*unsigned long OS_LogicProcess::CalibrateDac(float fdac)
{
	float f_gain,f_offset,f_Bound;
	float f_mid;
	unsigned long u32_Current;
	f_gain = 1;
	f_offset = 0;
	f_Bound = 0xFFFFFF;
	f_mid = 0x800000;
	fdac = fdac*f_gain + f_offset;

	u32_Current = f_mid + (fdac/Auto_DAC_out_Range)*f_mid;
	if(u32_Current>f_Bound)
	{
		return f_Bound;
	}
	else
	{
		return u32_Current;
	}
}*/   //delete by qjm 20171222

void OS_LogicProcess::InitBoardEngine(
    St_DisposableTask* open,
    unsigned char open_len,
    St_DisposableTask* close,
    unsigned char close_len,
    St_DisposableTask* cc2cc,
    unsigned char cc2cc_len,
    St_DisposableTask* rst2off,
    unsigned char rst2off_len
)
{
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = open;
    OS_LogicProcess::SetCtrlRstToCur.stepLen = open_len;

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = open;
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = open_len;

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = open;
    OS_LogicProcess::SetCtrlOffToCur.stepLen = open_len;

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = open;
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = open_len;


    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = rst2off;
    OS_LogicProcess::SetCtrlRstToOff.stepLen = rst2off_len;

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = rst2off;
    OS_LogicProcess::SetCtrlOffToRst.stepLen = rst2off_len;


    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = cc2cc;
    OS_LogicProcess::SetCtrlCurToCur.stepLen = cc2cc_len;

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = cc2cc;
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = cc2cc_len;

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = cc2cc;
    OS_LogicProcess::SetCtrlVltToCur.stepLen = cc2cc_len;

    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = cc2cc;
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = cc2cc_len;


    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = close;
    OS_LogicProcess::SetCtrlCurToOff.stepLen = close_len;

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = close;
    OS_LogicProcess::SetCtrlCurToRst.stepLen = close_len;

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = close;
    OS_LogicProcess::SetCtrlVltToOff.stepLen = close_len;

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = close;
    OS_LogicProcess::SetCtrlVltToRst.stepLen = close_len;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
