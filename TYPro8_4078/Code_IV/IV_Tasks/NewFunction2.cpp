#include "../Entry/includes.h"

///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////
unsigned char               thirdParty::m_status[MAXCHANNELNO];    //״̬��    8bit
st_Third_Party_Msg          thirdParty::st_third_party_msg[MAXCHANNELNO];
unsigned char               thirdParty::m_MsgValid[MAXCHANNELNO];    //1������msg    0����û��
st_Third_Handle             thirdParty::st_third_handle[MAXCHANNELNO];    //��¼
unsigned char               thirdParty::manu_auto_Flag[MAXCHANNELNO];    //�ֶ����Զ�У׼ flag   if( == 0) not care

UInt16                      thirdParty::Priority = PRIORITY4;
UInt16                      thirdParty::TaskID = THIRDPARTY_TASK;      // should be a definition which positioning this task in OS_Task[]
// unsigned short              thirdParty::Dac_set_value;    //�����Զ�У׼DAC��ֵ����msg�ڻ�ã�������Auto_Base
// float                       thirdParty::f_desired_value;    //20131010  by chen
unsigned char               thirdParty::UnsafeLockSwitch[MAXCHANNELNO];        //ΪPWM�������ERROR�źŲ���ͨ��               //�ǰ�ȫ״̬����ģʽ����  add by qjm 20140822

unsigned char               thirdParty::AutoCaliFlag;


void thirdParty::Init(void)
{
    unsigned char i;
    for( i = 0 ; i < MAXCHANNELNO; i++)
    {
        setStatus(i,THIRD_PARTY_STATUS_INACTIVE);
        m_MsgValid[i] = 0;
        st_third_handle[i].handleID = 0;
        st_third_handle[i].type = 0;
        UnsafeLockSwitch[i] = 0;  //PWM����
        manu_auto_Flag[i] = 0;
    }

// 	Dac_set_value = 0;
    AutoCaliFlag = 0;
}


void thirdParty::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_THIRDPARTY] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_THIRDPARTY);
#endif

    for(unsigned char i = 0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; i++)
    {
        processMSG(i);    //���յ�ǰ״̬��������msg
        processFSM(i);    //���յ�ǰ״̬����һЩˢ�ºʹ�����
    }
#if SOFTWARE_DOG_ENABLE == 1
    SoftWare_WatchDog();   //��λ����Ӧ����   dirui 0828   ���Դ������β�Ӱ������
#endif

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_THIRDPARTY] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_THIRDPARTY);
#endif
}

// void thirdParty::EndTask(void)
// {
// 	return;
// }

//0 fail
//1 succeed
unsigned char thirdParty::sendMsgToMe(const st_Third_Party_Msg * msg)    //�βλ���    ����    comment out chen 20130104
{   //��msg ���ڴ�λ��
    unsigned char chn = msg->chn;

    if(chn >ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount )
        chn %= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ;

    st_third_party_msg[chn].chn = msg->chn;
    st_third_party_msg[chn].st_third_party_cmd = msg->st_third_party_cmd;
    st_third_party_msg[chn].TaskID = msg->TaskID;
    if(msg->st_third_party_cmd.cmd == THIRD_PARTY_STOP)  //20160407,add by hepeiqing, ��ֹsafety check���͹�����δ��ʼ���������µ����̴�������
    {
        st_third_party_msg[chn].st_third_party_cmd.ctl_cmd_ex.I_Range = ChannelInfo_Manager::CurrRngPhysicalRange1;
        st_third_party_msg[chn].st_third_party_cmd.ctl_cmd_ex.V_Range = VOLTAGE_RANGE_HIGH;
        if(board_config::Func_Set_Volt_Range != 0)
        {
            board_config::Func_Set_Volt_Range(chn,VOLTAGE_RANGE_HIGH);
        }
    }

    m_MsgValid[chn] = 1;
    return 1;
}

//����msg
//�ı��ChannelInfo_Manager::ChannelInfo.m_ucStatus
//st_third_handle  ���
void thirdParty::processMSG(unsigned char chn)
{
    if(!m_MsgValid[chn])
        return;             //Have no message,do nothing and return;

    if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 == CHN_LOCK_SCH)
    {
        m_MsgValid[chn] = 0;    //clear
        return;    //�����κδ���
    }

    st_Third_Handle * p_Handle = &st_third_handle[chn];
    St_ChannelInfo * p_ChnInfo = &ChannelInfo_Manager::ChannelInfo[chn];
    unsigned char Cmd = st_third_party_msg[chn].st_third_party_cmd.cmd;

    switch(getStatus(chn))    //��ͬ״̬���ܲ�ͬ����
    {
    case THIRD_PARTY_STATUS_IDLE:    //idle ����CC������ת����Ϊtran״̬
        switch(Cmd)
        {
        case THIRD_PARTY_CC:    //����
            if(UnsafeLockSwitch[chn])
                return;
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CLOSE)
            {
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_OFF_TO_CC);
                p_Handle->type = THIRD_PARTY_CC;    //DISPOSABLE_MSG_CMD_OFF_TO_CC;
                setStatus(chn,THIRD_PARTY_STATUS_TRAN);    //���뵽Tran״̬
                p_ChnInfo->m_ucStatus = CHANNEL_INFO_CC;
            }
            break;
        case THIRD_PARTY_CV:
            if(UnsafeLockSwitch[chn])
                return;//������Error�źţ�Pwm��ֹ��ͨ��
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CLOSE)
            {
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_OFF_TO_CV);
                p_Handle->type = THIRD_PARTY_CV;    //DISPOSABLE_MSG_CMD_OFF_TO_CV;
                setStatus(chn,THIRD_PARTY_STATUS_TRAN);//���뵽Tran״̬
                if((board_config::DacInfo.m_bHaveVoltageDac)                                                        //board type which have VoltDAC
                        //||(( _BOARD_TYPE_ == BOARDTYPE_HPT200) && p_ChnInfo->m_bCaliBoard ))   //HPT200,and is used as CaliBoard
                        ||(( ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200) && p_ChnInfo->m_bCaliBoard ))
                    p_ChnInfo->m_ucStatus = CHANNEL_INFO_CV;
                else
                {
                    p_ChnInfo->m_ucStatus = CHANNEL_INFO_CV;
                    p_ChnInfo->m_CtrlState = CCS_I;  // DKQ  //�Ա�������ֵ�ѹģʽ  //20150518
                    ChannelInfo_Manager::m_Pid_Coef[chn].Init(); // DKQ  //����ϴ�PID�ڲ�����
                    ChannelInfo_Manager::m_Pid_Coef[chn].m_PidTypeFlag = PID_V; // DKQ  //ָ���ǵ�ѹPID��ʽ
                }
            }
            break;
// 		case THIRD_PARTY_CP:
// 		case THIRD_PARTY_CL:
// 		case THIRD_PARTY_SETCLAMPV:
// 			break;
        case THIRD_PARTY_CLR_3_RD_MODE:    //clear ����ģʽ
            setStatus(chn,THIRD_PARTY_STATUS_INACTIVE);    //ת��δ����
            break;
// 		case THIRD_PARTY_AUX_VOLTAGE:
// 			break;
// 		case THIRD_PARTY_AUX_TEMPERATURE:
// 			break;
        default:
            setStatus(chn,THIRD_PARTY_STATUS_IDLE);    //donothing
            break;
        }
        m_MsgValid[chn] = 0;    //clear
        return;    //break THIRD_PARTY_STATUS_IDLE
    case THIRD_PARTY_STATUS_TRAN:
        ChannelInfo_Manager::m_NeedRCFilterFlg[chn]=false; //true
        if(Cmd == THIRD_PARTY_STOP)    //stop����
        {
            unsigned char uc_StopStatus = p_ChnInfo->m_ucStatus;
            if((uc_StopStatus == CHANNEL_INFO_CC) || (uc_StopStatus == CHANNEL_INFO_CV))
                p_Handle->type = THIRD_PARTY_STOP;
            if(uc_StopStatus == CHANNEL_INFO_CC)    //��ͨ�����������ʱ
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CC_TO_OFF);
            else if(uc_StopStatus == CHANNEL_INFO_CV)    //��ѹ���
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CV_TO_OFF);
            setStatus(chn,THIRD_PARTY_STATUS_TRAN);    //תΪidle״̬
            p_ChnInfo->m_ucStatus = CHANNEL_INFO_CLOSE;//תΪ�ر�״̬
        }
        else if(Cmd == THIRD_PARTY_CLR_3_RD_MODE)    //�������������
        {
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CC)    //��ͨ�����������ʱ
            {
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CC_TO_OFF);
                p_Handle->type = THIRD_PARTY_STOP;
            }
            else if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CV)    //��ѹ���
            {
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CV_TO_OFF);
                p_Handle->type = THIRD_PARTY_STOP;
            }
            setStatus(chn,THIRD_PARTY_STATUS_INACTIVE);
            p_ChnInfo->m_ucStatus = CHANNEL_INFO_CLOSE;//תΪ�ر�״̬
        }
        m_MsgValid[chn] = 0;    //clear
        return;//break  THIRD_PARTY_STATUS_TRAN
    case THIRD_PARTY_STATUS_OUTPUT:     //���״̬
        switch(Cmd)
        {
        case THIRD_PARTY_CC:
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CC)
            {
                p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CC_TO_CC);
                p_Handle->type = THIRD_PARTY_CC;
                setStatus(chn,THIRD_PARTY_STATUS_TRAN);
            }
            break;//����Tran
        case THIRD_PARTY_CV:
            char uc_update = -1;
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CLOSE)
            {
// 				p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_OFF_TO_CV);
// 				p_Handle->type = THIRD_PARTY_CV;
// 				setStatus(chn,THIRD_PARTY_STATUS_TRAN);//���뵽Tran״̬
                p_ChnInfo->m_ucStatus = CHANNEL_INFO_CV;
                uc_update = DISPOSABLE_MSG_CMD_OFF_TO_CV;
            }
            else if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CV)
            {
// 				p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CV_TO_CV);
// 				p_Handle->type = THIRD_PARTY_CV;
// 				setStatus(chn,THIRD_PARTY_STATUS_TRAN);
                uc_update = DISPOSABLE_MSG_CMD_CV_TO_CV;
            }

            if(uc_update != -1)
            {
                p_Handle->handleID = stuffDisposableMsg(chn,uc_update);
                p_Handle->type = THIRD_PARTY_CV;
                setStatus(chn,THIRD_PARTY_STATUS_TRAN);//���뵽Tran״̬
            }
            break;//����Tran
// 		case THIRD_PARTY_CP:
// 		case THIRD_PARTY_CL:
// 		case THIRD_PARTY_SETCLAMPV:
// 			break;//��ʱδ��
        case THIRD_PARTY_STOP:
            unsigned char uc_MSG_CMD = 0;
            if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CC)    //��ͨ�����������ʱ
            {
// 				setStatus(chn,THIRD_PARTY_STATUS_TRAN);
// 				p_Handle->handleID = stuffDisposableMsg(chn,DISPOSABLE_MSG_CMD_CC_TO_OFF);
// 				p_Handle->type = THIRD_PARTY_STOP;
                uc_MSG_CMD = DISPOSABLE_MSG_CMD_CC_TO_OFF;
            }
            else if(p_ChnInfo->m_ucStatus == CHANNEL_INFO_CV)    //��ѹ���
            {
                uc_MSG_CMD = DISPOSABLE_MSG_CMD_CV_TO_OFF;
            }
            if(uc_MSG_CMD != 0)
            {
                setStatus(chn,THIRD_PARTY_STATUS_TRAN);
                p_Handle->handleID = stuffDisposableMsg(chn,uc_MSG_CMD);
                p_Handle->type = THIRD_PARTY_STOP;
            }
            p_ChnInfo->m_ucStatus = CHANNEL_INFO_CLOSE;    //תΪ�ر�״̬
            p_ChnInfo->m_CtrlState=CCS_OFF;   //add by qjm 2014/10/22
            break;//����stop Tran
// 		case THIRD_PARTY_AUX_VOLTAGE:
// 		case THIRD_PARTY_AUX_TEMPERATURE:
        default:
            break;
        }
        m_MsgValid[chn] = 0;    //clear
        return;    //break    THIRD_PARTY_STATUS_OUTPUT
    case THIRD_PARTY_STATUS_INACTIVE:
        if(Cmd == THIRD_PARTY_SET_3_RD_MODE)    //����
        {
            thirdParty::m_status[chn] = THIRD_PARTY_STATUS_IDLE;    //ת����idle״̬
            manu_auto_Flag[chn] = p_ChnInfo->m_IndependantCtrl.m_ucBoardOperateType;   //st_third_party_msg[chn].st_third_party_cmd.ctl_cmd_ex.Battery_Module;
        }
        m_MsgValid[chn] = 0;    //clear
        return;
    default:    //��ֹ�������
        thirdParty::m_status[chn] = THIRD_PARTY_STATUS_INACTIVE;    //תΪδ������
        m_MsgValid[chn] = 0;//clear
        return;
    }
}

//������tran��output���룬ά��
void thirdParty::processFSM(unsigned char  chn)
{
    unsigned char state = 0;
    state = getStatus(chn);

    if(state == THIRD_PARTY_STATUS_TRAN)
	{
		CommonFunc::GetTimeTicksUpdate(&ChannelInfo_Manager::ChannelInfo[chn].m_tLastFineTuningTime);//190917 hpq
        processDisposableHandle(chn);
	}
    else if(state == THIRD_PARTY_STATUS_OUTPUT)
    {
        unsigned long dacValueTemp = 0;
        if(ChannelInfo_Manager::ChannelInfo[chn].m_ucStatus == CHANNEL_INFO_CC)
		{//190917 hpq
				//dacValueTemp = ChannelInfo_Manager::ChannelInfo[chn].m_u32DacValue; 
			  float fNewValue = GetNewCurrentValue_FineTuning(chn);
			  dacValueTemp = ChannelInfo_Manager::CalibrateCurrentDAC(chn, 
			           ChannelInfo_Manager::ChannelInfo[chn].m_ucCurrentRange, 
			           fNewValue);
            Step_Manager_task::StepDAC_Out(chn,dacValueTemp,ACTIVE_DAC_CUR);
        }

        if(manu_auto_Flag[chn] == THIRD_PARTY_MANUAL)
        {
            if(ChannelInfo_Manager::ChannelInfo[chn].m_ucStatus == CHANNEL_INFO_CV)  //DKQ 06.18.2013  ���������PID ��ѹ����ģʽ
            {
                ChannelInfo_Manager::ChannelInfo[chn].bDigitalV = !board_config::DacInfo.m_bHaveVoltageDac;
                Step_Manager_task::m_Range[chn] = ChannelInfo_Manager::ChannelInfo[chn].m_ucCurrentRange;

// 				if(Step_Manager_task::Unipolar.BoardFlag)
// 					  ChannelInfo_Manager::Vsamp_NeedRecalculateSign[chn].m_Bits.DacOutput = 0;  // 20160630 yy  close PID

                if(ChannelInfo_Manager::Vsamp_NeedRecalculateSign[chn].m_Bits.DacOutput)
                {   //add by hepeiqing,20160407,��ֹ�ӹ��ʵ���У׼��ѹ��ʱ���ѹ��������
                    ChannelInfo_Manager::Vsamp_NeedRecalculateSign[chn].m_Bits.DacOutput = 0;
                    Step_Manager_task::Step_OutputVoltDac(chn,st_third_party_msg[chn].st_third_party_cmd.ctl_cmd_ex.VDAC_Set,false);
                }
            }
        }
    }
}
float thirdParty::GetNewCurrentValue_FineTuning(unsigned char chn)
{//190917 hpq
	float fNewValue = ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_SetCurrent;
	if(ChannelInfo_Manager::ChannelInfo[chn].m_bThirdParty_FineTuning == 0)
		return fNewValue;
	MsTime timediff = CommonFunc::TimeDiff(ChannelInfo_Manager::ChannelInfo[chn].m_tLastFineTuningTime);
	if((timediff.Second > 1 || timediff.Us100 > 5000)
    && 	(ChannelInfo_Manager::ChannelInfo[chn].m_bThirdParty_FineTuningDone == 0)	)
		//fine tuning very 500ms is enough for calibration check
	{
		ChannelInfo_Manager::ChannelInfo[chn].m_bThirdParty_FineTuningDone = 1;  //fine tuning working for once
		CommonFunc::GetTimeTicksUpdate(&ChannelInfo_Manager::ChannelInfo[chn].m_tLastFineTuningTime);
		unsigned char Range =  ChannelInfo_Manager::ChannelInfo[chn].m_ucCurrentRange;
		float fMaxError=ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(chn,Range)*0.005f;
		float fMinError=ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(chn,Range)*0.0002f;  //fine tuning 0.02% minimum
		float fError = ChannelInfo_Manager::Read_SpecifiedMetavariable2(chn,MetaCode_PV_Current) 
	                       - ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_SetCurrent;
		if (abs(fError)> fMinError)
			ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset -= fError;//*0.2;  //fine tuning smoothing factor
		if(ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset > fMaxError)
			ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset = fMaxError;
		else if(ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset < -fMaxError)
			ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset = -fMaxError;
	}
	fNewValue += ChannelInfo_Manager::ChannelInfo[chn].m_fThirdParty_FineTuningLastOffset;
	return fNewValue;
}
//���Ĵ�ͨ����״̬
unsigned char thirdParty::setStatus(unsigned char chn,unsigned char status)
{
    thirdParty::m_status[chn] = status;

    if(status == THIRD_PARTY_STATUS_INACTIVE)    //�������δ�����manu_auto_Flag����
        manu_auto_Flag[chn] = 0;

    return 1;
}

unsigned short thirdParty::stuffDisposableMsg(unsigned char chn,unsigned char type)
{
    //  float temp_set = 0;
    //  unsigned char range = 0;
    //  float temp_VH_set = ChannelInfo_Manager::ChannelInfo[chn].m_IndependantCtrl.m_fClampVH;
    //  float temp_VL_set = ChannelInfo_Manager::ChannelInfo[chn].m_IndependantCtrl.m_fClampVL;

    St_DisposableMSG st_Disposable_msg;
    st_Disposable_msg.CH_Number = chn;
    st_Disposable_msg.msgID = THIRDPARTY_TASK;

    st_Third_Party_ctl_Ex * p_Cmd = &st_third_party_msg[chn].st_third_party_cmd.ctl_cmd_ex;
    St_ChannelInfo * p_Info = &ChannelInfo_Manager::ChannelInfo[chn];

    manu_auto_Flag[chn] = ChannelInfo_Manager::ChannelInfo[chn].m_IndependantCtrl.m_ucBoardOperateType; // yy 20161216

    if(manu_auto_Flag[chn] == THIRD_PARTY_AUTO)
    {
        st_Disposable_msg.AutoCaliFlag = 1;    //�Զ�У׼����
        AutoCaliFlag = 1;
    }
    else    // THIRD_PARTY_MANUAL or others
    {
        st_Disposable_msg.AutoCaliFlag = 0;
        AutoCaliFlag = 0;
    }

    // All Ranges
    st_Disposable_msg.RangeSet = 0;
    st_Disposable_msg.next_Range = p_Cmd->I_Range;
    st_Disposable_msg.last_Range = p_Info->m_ucCurrentRange;
    st_Disposable_msg.next_VRange = p_Cmd->V_Range;
    st_Disposable_msg.last_VRange = p_Info->m_ucVoltageRange;

    // Clamps
    st_Disposable_msg.Clamp_Value   = ChannelInfo_Manager::CalibrateClampDAC(chn,
                                      p_Cmd->V_Range,
                                      p_Info->m_IndependantCtrl.m_fClampVH);
    st_Disposable_msg.Clamp_Value_2 = ChannelInfo_Manager::CalibrateLowClampDAC(chn,
                                      p_Cmd->V_Range,
                                      p_Info->m_IndependantCtrl.m_fClampVL);

    // Save range to ChannelInfo
    p_Info->m_ucCurrentRange  = p_Cmd->I_Range;
    p_Info->m_ucVoltageRange  = p_Cmd->V_Range;
		
		ChannelInfo_Manager::Set_PID_MaxCurrentRange(chn);  //add by qjm 20201217 �������У׼��ѹû�е���ֵ��Bug

    // Specific initial
    switch(type)
    {
    case DISPOSABLE_MSG_CMD_OFF_TO_CC:
        st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr;
        st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToCur.stepLen;
        st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_OPENCHN;
	    st_Disposable_msg.TargetCtrlType = CT_CURRENT;  // 20200810 yy
        st_Disposable_msg.Dac_Value		=		ChannelInfo_Manager::CalibrateCurrentDAC(chn,
                                                p_Cmd->I_Range,
                                                p_Cmd->IDAC_Set);    //��������    comment out chen 20130105
        break;
    case DISPOSABLE_MSG_CMD_CC_TO_CC:
        st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr;
        st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToCur.stepLen;
        st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CCHN;
	    st_Disposable_msg.TargetCtrlType = CT_CURRENT;  // 20200810 yy
        st_Disposable_msg.Dac_Value     = ChannelInfo_Manager::CalibrateCurrentDAC(chn,
                                          p_Cmd->I_Range,
                                          p_Cmd->IDAC_Set);    //��������    comment out chen 20130105
        break;
    case DISPOSABLE_MSG_CMD_CC_TO_OFF:    //����ʱ��ʱ����msg��dac�Ѿ���������
        st_Disposable_msg.Disposable_task.st_Disposabletask_ptr =OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr;
        st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlCurToOff.stepLen;
        st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
	    st_Disposable_msg.TargetCtrlType = CT_OFF;  // 20200810 yy
        st_Disposable_msg.AutoCaliFlag = 0;
        break;
    default:
        break;
    }


//#if (_BOARD_TYPE_ != BOARDTYPE_TESLA_PWM)
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_TESLA_PWM)
    {
        switch(type)
        {
        case DISPOSABLE_MSG_CMD_OFF_TO_CV:
            if(!p_Info->m_bCaliBoard)
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToVlt.stepLen;
            }
            else
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCaliBoardRstToVlt.st_Disposabletask_ptr;//20150518
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCaliBoardRstToVlt.stepLen;
            }
            st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_OPENCHN;
			st_Disposable_msg.TargetCtrlType = CT_VOLTAGE;  // 20200810 yy
            //20130821
            st_Disposable_msg.Dac_Value = CONVERTER_MID;  //ChannelInfo_Manager::CalibrateCurrentDAC(chn,range,0);    //��������    comment out chen 20130105

// 				if(Step_Manager_task::Unipolar.BoardFlag)
// 		            st_Disposable_msg.Dac_Value = OS_LogicProcess::Set_DAC_Output_MIN;   // ˫�߱�ɵ��� 0V---2.5V

            st_Disposable_msg.Volt_DacValue = ChannelInfo_Manager::CalibrateVoltageDAC(chn,
                                              p_Cmd->V_Range,
                                              p_Cmd->VDAC_Set);
            break;
        case DISPOSABLE_MSG_CMD_CV_TO_CV:
            if(!p_Info->m_bCaliBoard)
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToVlt.stepLen;
            }
            else
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCaliBoardVltToVlt.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCaliBoardVltToVlt.stepLen;
            }
            st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CCHN;
            st_Disposable_msg.TargetCtrlType = CT_VOLTAGE;  // 20200810 yy
            st_Disposable_msg.Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC(chn,p_Cmd->I_Range,0);    //��������    comment out chen 20130105

// 				if(Step_Manager_task::Unipolar.BoardFlag)
// 			        st_Disposable_msg.Dac_Value = OS_LogicProcess::Set_DAC_Output_MIN;   // ˫�߱�ɵ��� 0V---2.5V

            st_Disposable_msg.Volt_DacValue = ChannelInfo_Manager::CalibrateVoltageDAC(chn,
                                              p_Cmd->V_Range,
                                              p_Cmd->VDAC_Set);
            break;

        case DISPOSABLE_MSG_CMD_CV_TO_OFF:    //����ʱ��ʱ����msg��dac�Ѿ���������
            //st_Disposable_msg.RangeSet = 0;
            if(!p_Info->m_bCaliBoard)
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
            }
            else
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCaliBoardVltToOff.st_Disposabletask_ptr;//20150518
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCaliBoardVltToOff.stepLen;
            }
            st_Disposable_msg.AutoCaliFlag = 0;
            st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
			st_Disposable_msg.TargetCtrlType = CT_OFF;  // 20200810 yy
            break;
        default:
            //return 0;
            break;
        }
    }
//#else	// TESLA_PWM Board
    else
    {
        switch(type)
        {
        case DISPOSABLE_MSG_CMD_OFF_TO_CV:
            if(AutoCaliFlag == 0)
            {
                st_Disposable_msg.next_Range = p_Cmd->V_Range;
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlOffToVlt.stepLen;
                st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_OPENCHN;

                //temp_set = p_Cmd->VDAC_Set;    //����DAC
                st_Disposable_msg.Dac_Value = CONVERTER_MID;
                st_Disposable_msg.Clamp_Value = ChannelInfo_Manager::CalibrateClampDAC(chn,VOLTAGE_RANGE_HIGH,p_Info->m_IndependantCtrl.m_fClampVH);
                st_Disposable_msg.Clamp_Value_2 = ChannelInfo_Manager::CalibrateLowClampDAC(chn,VOLTAGE_RANGE_HIGH,p_Info->m_IndependantCtrl.m_fClampVL);
            }
            else    //�Զ�У׼
            {
                //range = p_Cmd->V_Range;
                st_Disposable_msg.next_Range = p_Cmd->V_Range;
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetAutoCaliOpen.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetAutoCaliOpen.stepLen;
                st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_OPENCHN;
                st_Disposable_msg.Dac_Value = CONVERTER_MID;
            }
			st_Disposable_msg.TargetCtrlType = CT_VOLTAGE;  // 20200810 yy
            break;
        case DISPOSABLE_MSG_CMD_CV_TO_CV:
            if(AutoCaliFlag == 0)
            {
                st_Disposable_msg.next_Range = p_Cmd->V_Range;
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToVlt.stepLen;
                st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CCHN;

                st_Disposable_msg.Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC(chn,p_Cmd->V_Range,0);
                st_Disposable_msg.Clamp_Value = ChannelInfo_Manager::CalibrateClampDAC(chn,VOLTAGE_RANGE_HIGH,p_Info->m_IndependantCtrl.m_fClampVH);
                st_Disposable_msg.Clamp_Value_2 = ChannelInfo_Manager::CalibrateLowClampDAC(chn,VOLTAGE_RANGE_HIGH,p_Info->m_IndependantCtrl.m_fClampVL);
            }
            else    //�Զ�У׼
            {
                st_Disposable_msg.next_Range = p_Cmd->V_Range;
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetAutoCaliOpen.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetAutoCaliOpen.stepLen;
                st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CCHN;
            }
			st_Disposable_msg.TargetCtrlType = CT_VOLTAGE;  // 20200810 yy
            break;
        case DISPOSABLE_MSG_CMD_CV_TO_OFF:    //����ʱ��ʱ����msg��dac�Ѿ���������
            if(AutoCaliFlag == 0)
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetCtrlVltToOff.stepLen;
            }
            else    //�Զ�У׼
            {
                st_Disposable_msg.Disposable_task.st_Disposabletask_ptr = OS_LogicProcess::SetAutoCaliClose.st_Disposabletask_ptr;
                st_Disposable_msg.Disposable_task.stepLen = OS_LogicProcess::SetAutoCaliClose.stepLen;
            }
            st_Disposable_msg.Disposable_Flag = DISPOSABLE_to_CLOSECHN;
			st_Disposable_msg.TargetCtrlType = CT_OFF;  // 20200810 yy
            break;
        default:
            //return 0;
            break;
        }
    }
//#endif

    unsigned short handle = OS_LogicProcess::requestLogicPorcess(chn,&st_Disposable_msg);
    return handle;
}

signed char thirdParty::getStatus(unsigned char chn)
{
    return (signed char)(thirdParty::m_status[chn]);
}

int thirdParty::processDisposableHandle(unsigned char chn)
{
    unsigned char LogicProcessStatus = OS_LogicProcess::getHandleStatus(st_third_handle[chn].handleID);

    if(LogicProcessStatus == DISPOSABLE_FINISHED)    //�������
    {
        unsigned char uc_Status = 0;
        switch(st_third_handle[chn].type)
        {
        case THIRD_PARTY_CC:
        case THIRD_PARTY_CV:
// 			setStatus(chn,THIRD_PARTY_STATUS_OUTPUT);
            uc_Status = THIRD_PARTY_STATUS_OUTPUT;
            break;
        case THIRD_PARTY_STOP:
        default:
// 			setStatus(chn,THIRD_PARTY_STATUS_IDLE);
            uc_Status = THIRD_PARTY_STATUS_IDLE;
            break;
        }

        if(uc_Status != 0)
            setStatus(chn,uc_Status);
        return 1;
    }
    else if(LogicProcessStatus == DISPOSABLE_ERROR)    //����
        return -1;
    else if(LogicProcessStatus == DISPOSABLE_RUNNING)    //������
    {
        setStatus(chn,THIRD_PARTY_STATUS_TRAN);    //ת����
        return 0;
    }
    else if(LogicProcessStatus == DISPOSABLE_ASSIGNED)    //�ȴ�����
        return 0;

    if(LogicProcessStatus == DISPOSABLE_DEAD)
        return -1;    //����

    return -1;
}

// void thirdParty::clearMSG(char chn)
// {
// 	memset(&thirdParty::st_third_party_msg[chn], 0, sizeof(st_Third_Party_Msg));
// 	memset(&thirdParty::st_third_handle[chn],0,sizeof(st_Third_Handle));    //���
// }

//���е�����ʱ��סsch
//void thirdParty::lockSCH(unsigned char chn)
//{
////    St_ScheduleMessage sch_msg;
//}

//void unlockSCH(unsigned char chn)
//{
// //   St_ScheduleMessage sch_msg;
//}



void thirdParty::SoftWare_WatchDog(void)   //��λû����Ӧʱ������״̬���� dirui 0828
{   //20130904,modified by hepeiqing
    if(EthernetParse::m_bResetThirdParty)
    {
        for(unsigned char uc_i=0; uc_i<MAXCHANNELNO; uc_i++)
        {
            if(ChannelInfo_Manager::ChannelInfo[uc_i].m_bCaliBoard)
                continue;
            if(getStatus(uc_i)== THIRD_PARTY_STATUS_OUTPUT)
            {
                setStatus(uc_i,THIRD_PARTY_STATUS_TRAN);

                if(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucStatus == CHANNEL_INFO_CC)    //��ͨ�����������ʱ
                    st_third_handle[uc_i].handleID = stuffDisposableMsg(uc_i,DISPOSABLE_MSG_CMD_CC_TO_OFF);
                else if(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucStatus == CHANNEL_INFO_CV)    //��ѹ���
                    st_third_handle[uc_i].handleID = stuffDisposableMsg(uc_i,DISPOSABLE_MSG_CMD_CV_TO_OFF);

                st_third_handle[uc_i].type = THIRD_PARTY_STOP;
                ChannelInfo_Manager::ChannelInfo[uc_i].m_ucStatus = CHANNEL_INFO_CLOSE;    //תΪ�ر�״̬
            }
        }
        EthernetParse::m_bResetThirdParty = 0;
    }
}







//#endif
///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////


