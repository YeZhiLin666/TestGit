///////////////////////////////////////////////////////////////////////////////////
//File Name: CheckLimit_4.cpp
//Version:        V1.0
//Date:                2013/9/17
//Description:
//                        Step/Log Limit Check
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
UInt16        Check_Limit4_task::Priority = PRIORITY4;
UInt16        Check_Limit4_task::TaskID = CHECKLIMIT_4_TASK;

unsigned char Check_Limit4_task::m_CheckedChannelCount_SL;
unsigned char Check_Limit4_task::m_CheckedChannelCount_LL;
unsigned char Check_Limit4_task::m_BuiltInV_AnyLimitChn = 0;
unsigned char Check_Limit4_task::m_BuiltInT_AnyLimitChn = 0;
// unsigned char uc_AuxType = 0XFF;
// unsigned char uc_AuxDxDt = false;
void Check_Limit4_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CHECKLIMIT4] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_CHECKLIMIT4);
#endif

    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
//                 case BOARDTYPE_BT2204:
    case BOARDTYPE_LBT22043:
    case BOARDTYPE_LBT22013:
    case BOARDTYPE_LBT22023:
//                 case BOARDTYPE_LBT22024:
//                 case BOARDTYPE_LBT22013FR:
//                 case BOARDTYPE_LBT22044:
        for(unsigned char ChannelIdx = 0; ChannelIdx < BURSTMODESAMPLING_MAX_CHANNEL; ChannelIdx++)
        {
            BurstModeSampling::ReportSimpleData();  //  2017.6.8  yy
            if(BurstModeSampling::uc_A_BurstMode[ChannelIdx] || BurstModeSampling::bl_NeedRunning[ChannelIdx])
            {
#if THREAD_TASK_TIME_STATISTIC == 1
                Time_Count::thread_time(COUNT_CHECKLIMIT4);
#endif
                return;
            }
        }
        break;
    default:
        break;
    }


    SL_Check(STEPLIMITCHECK_COUNT);
    LL_Check(LOGLIMITCHECK_COUNT);

    if(m_CheckedChannelCount_LL == 0)
    {
#if THREAD_TASK_TIME_STATISTIC == 1
        Time_Count::thread_time(COUNT_CHECKLIMIT4);
#endif
        return;
    }

    for(unsigned char ChannelIdx = 0; ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        // 		if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_SimuInfo.m_bValid
        // 			&& (!ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_SimuInfo.m_bLogLimitValid))
        // 			continue;

        // 		//如果在做SIMULATION，而且SIMULAITON控制不响应LOG LIMIT，则不做LOG。

        if(Step_Manager_task::bNeedCheck[ChannelIdx] && Step_Manager_task::bLogLimitChecked[ChannelIdx] && (!ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_IR_Stop_Logging)) //during CCCV Relaxed, do not log data
        {
            LL_DoLog(ChannelIdx);
            Step_Manager_task::bLogLimitChecked[ChannelIdx] = 0;
        }
    }

    if((IVSDL::Buffer.m_u32Length >= MAX_PACK_LOGPOINT) || (CommonFunc::GetTimeTicks(&IVSDL::m_IntervalTime_SDL) > 5000))
    {
        CommonFunc::GetTimeTicksUpdate( &IVSDL::m_IntervalTime_SDL);
        EthernetParse::MPReportData(0, SEND_LOG_SDL);   //调用第二信道发送数据到上位机
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CHECKLIMIT4] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_CHECKLIMIT4);
#endif
}

void Check_Limit4_task::MyInit(void)
{
    Reset();
    BurstModeSampling::Init();   //  2017.6.8  yy
    IVSDL::Init_WithoutBuffer();
}

// void Check_Limit4_task::EndTask(void)
// {

// }

void Check_Limit4_task::Reset(void)
{
    m_CheckedChannelCount_SL = 0;
    m_CheckedChannelCount_LL = 0;
}

void Check_Limit4_task::SL_Check(unsigned char MaxLimitCount)
{
    unsigned char ChannelIdx = 0;
    unsigned char ActiveIdx = 0;
    unsigned char LimitCount = 0;
    unsigned char bJumpCheck = 0;     //有些LIMIT，不需要经过符号判断CHECK，直接被置为0或1
    while(LimitCount < MaxLimitCount)
    {
        if(FindNextStepLimit())
        {
            LimitCount++;
            bJumpCheck = 0;
            ChannelIdx = Step_Manager_task::SL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;

            unsigned char bReached = 0, ucLeftType = 0, ucRightType = 0;
            float fLeftValue = 0.0f, fRightValue = 0.0f;

            St_Limit * thisLimit = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[Step_Manager_task::SL_CurrentPtr.m_LimitIdx];
            St_LimitInfo * thisLimitInfo = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[Step_Manager_task::SL_CurrentPtr.m_LimitIdx];
            St_ChannelInfo * thisChannelInfo = &ChannelInfo_Manager::ChannelInfo[ChannelIdx];
            ucLeftType = thisLimit->m_ucLeftValue_DataType;
            fLeftValue = thisLimit->m_fLeftValue;
//            unsigned char ucLeftValue_Operator = thisLimit->m_ucLeftValue_Operator;// 20190917 yy
//            float fLeftValueCoefficient = thisLimit->m_fLeftValueCoefficient;
            ucRightType = thisLimit->m_ucRightValue_DataType;
            fRightValue = thisLimit->m_fRightValue;
//            unsigned char ucRightValue_Operator = thisLimit->m_ucRightValue_Operator;// 20190917 yy
//            float fRightValueCoefficient = thisLimit->m_fRightValueCoefficient;

            if(ucLeftType == MP_DATA_TYPE_METAVALUE && fLeftValue == MetaCode_PV_Current)
            {
                if(Step_Manager_task::CCCVNoLimitCheckFlag[ChannelIdx])
                {
                    bJumpCheck = 1;
                }
            }

            if((ucLeftType == MP_DATA_TYPE_METAVALUE && fLeftValue == MetaCode_PV_Current)
                    || (ucRightType == MP_DATA_TYPE_METAVALUE && fRightValue == MetaCode_PV_Current))
            {   //利用了狄瑞的一个标志，判断是否电流被采样，如果没有被重新采样则LIMIT自动为FALSE；
                if(!thisChannelInfo->Current_Sampleflg)
                {
                    bJumpCheck = 1;
                }
            }
            if((ucLeftType == MP_DATA_TYPE_METAVALUE && fLeftValue == MetaCode_PV_Voltage)
                    || (ucRightType == MP_DATA_TYPE_METAVALUE && fRightValue == MetaCode_PV_Voltage))
            {   //利用了狄瑞的一个标志，判断是否电压被采样，如果没有被重新采样则LIMIT自动为FALSE；
                if(!thisChannelInfo->Voltage_Sampleflg)
                {
                    bJumpCheck = 1;
                }
            }
            if(thisLimit->m_uCheckPlace == ValueCheckPlace_AUXI)
            {
                if(!BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag)   //同时没有BUILTINAUX 存在的情况下
                {
                    bReached = thisLimitInfo->m_bReached;
                    bJumpCheck = 1;   //这个LIMIT在AUX通道检查，IV只关心结果
                }
            }


            if(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE)
            {
                switch (int(fLeftValue))//set variable 时只检查下面这些项。//niuyasong2017.05.05
                {
                case MetaCode_PV_Steptime:
                case MetaCode_PV_ChargeCapacity:
                case MetaCode_PV_DischargeCapacity:
                case MetaCode_PV_ChargeEnergy:
                case MetaCode_PV_DischargeEnergy:
                case MetaCode_PV_CycleIndex:
                case MetaCode_TC_Timer1:
                case MetaCode_TC_Timer2:
                case MetaCode_TC_Timer3:
                case MetaCode_TC_Timer4:
                case MetaCode_TC_ChargeCapacity1:
                case MetaCode_TC_ChargeCapacity2:
                case MetaCode_TC_DischargeCapacity1:
                case MetaCode_TC_DischargeCapacity2:
                case MetaCode_TC_ChargeEnergy1:
                case MetaCode_TC_ChargeEnergy2:
                case MetaCode_TC_DischargeEnergy1:
                case MetaCode_TC_DischargeEnergy2:
                case MetaCode_TC_Counter1:
                case MetaCode_TC_Counter2:
                case MetaCode_TC_Counter3:
                case MetaCode_TC_Counter4:
                    break;
                default:
                    bJumpCheck = 1;
                    break;
                }
            }
            //TEST_H_IRQ;
            if(!bJumpCheck)
            {
                if(CommonFunc::uc_Support_AUX)
                {
                    ChannelInfo_Manager::AuxLimitCheckOk = 1;
                }
                thisLimit->m_fLeft_Value = ChannelInfo_Manager::GetLimitValue(thisLimit, ChannelIdx, LIMIT_LEFT_VALUE);
                thisLimit->m_fRight_Value  = ChannelInfo_Manager::GetLimitValue(thisLimit, ChannelIdx, LIMIT_RIGHT_VALUE);

                bReached=CompareSign(thisLimit->m_ucCompareSign,thisLimit->m_fLeft_Value,thisLimit->m_fRight_Value);



                if(CommonFunc::uc_Support_AUX)
                {
                    if((thisLimit->m_bLimitFormulaHasAux) && (ChannelInfo_Manager::AuxLimitCheckOk == 0))
                    {
                        bReached = 0;
                    }
                }
                if((thisLimit->m_bAnyValid) && ( BuiltIn_DataManager::b_BuiltInAux) )
                {
                    if(bReached)
                        thisLimitInfo->m_AnyLimitReachedCount = 1;
                }
            }
            //TEST_L_IRQ;

            thisLimitInfo->m_bReached = bReached;//buildIn May clear Aux bReached flag, cause to limit never arrived. zyx 2102251
            thisLimitInfo->m_bChecked = 1;
            Step_Manager_task::SL_CurrentPtr.m_LimitIdx++;
        }
        else   //Find no more limit,return;
            return;
    }
}
signed char Check_Limit4_task::FindNextStepLimit(void)
{
    unsigned char ChannelIdx, ActiveIdx;

    for(;;)//while(1)
    {
        if(Step_Manager_task::bNeedCheck[Step_Manager_task::SL_CurrentPtr.m_ChannelIdx])
        {
            ChannelIdx = Step_Manager_task::SL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
            if(Step_Manager_task::SL_CurrentPtr.m_LimitIdx < ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo)
            {
                Step_Manager_task::bStepLimitChecked[ChannelIdx] = 1;
                return 1;
            }
            else
            {
                Step_Manager_task::SL_CurrentPtr.m_ChannelIdx++;
                if(Step_Manager_task::SL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    Step_Manager_task::SL_CurrentPtr.m_ChannelIdx = 0;
                Step_Manager_task::SL_CurrentPtr.m_LimitIdx = 0;
                m_CheckedChannelCount_SL++;
                //if((m_CheckedChannelCount_SL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                //  &&(Step_Manager_task::SL_CurrentPtr.m_LimitIdx >= Step_Manager_task::SL_StartPtr.m_LimitIdx))
                if(m_CheckedChannelCount_SL > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    return 0;
            }
        }
        else
        {
            Step_Manager_task::SL_CurrentPtr.m_ChannelIdx++;
            if(Step_Manager_task::SL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                Step_Manager_task::SL_CurrentPtr.m_ChannelIdx = 0;
            Step_Manager_task::SL_CurrentPtr.m_LimitIdx = 0;
            m_CheckedChannelCount_SL++;
            if(m_CheckedChannelCount_SL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                return 0;
        }
    }
}

void Check_Limit4_task::LL_Check(unsigned char MaxLimitCount)
{
    unsigned char ChannelIdx = 0, ActiveIdx = 0;
    unsigned char LimitCount = 0;
    unsigned char bJumpCheck = 0;

    while(LimitCount < MaxLimitCount)
    {
        if(FindNextLogLimit())
        {
//             memset(ChannelInfo_Manager::m_FormulaValue, 0, sizeof(ST_FORMULA_VALUE)*MAXCHANNELNO * IV_MAX_SCHEDULE_FORMULA_NUMBER); //170515 zyx
            LimitCount++;
            bJumpCheck = 0;//2107121 zyx
            ChannelIdx = Step_Manager_task::LL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
            unsigned char bReached = 0;

            St_Limit * thisLimit = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[Step_Manager_task::LL_CurrentPtr.m_LimitIdx];
            St_LimitInfo * thisLimitInfo = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[Step_Manager_task::LL_CurrentPtr.m_LimitIdx];

//#if SUPPORT_AUX == 1
            if(CommonFunc::uc_Support_AUX)
            {
                if(thisLimit->m_uCheckPlace == ValueCheckPlace_AUXI)
                {
                    bReached = thisLimitInfo->m_bReached;
                    bJumpCheck = 1;   //这个LIMIT在AUX通道检查，IV只关心结果
                }
            }
//#endif

            Step_Manager_task::bLogLimitChecked[ChannelIdx] = 1;


//            unsigned char ucLeftType = 0, ucRightType = 0;
            float fLeftValue = 0.0f, fRightValue = 0.0f;

            if(!bJumpCheck)
            {
//                ucLeftType = thisLimit->m_ucLeftValue_DataType;
                fLeftValue = thisLimit->m_fLeftValue;
//                unsigned char ucLeftValue_Operator = thisLimit->m_ucLeftValue_Operator;// 20190917 yy
//                float fLeftValueCoefficient = thisLimit->m_fLeftValueCoefficient;
//                ucRightType = thisLimit->m_ucRightValue_DataType;
                fRightValue = thisLimit->m_fRightValue;
//                unsigned char ucRightValue_Operator = thisLimit->m_ucRightValue_Operator;// 20190917 yy
//                float fRightValueCoefficient = thisLimit->m_fRightValueCoefficient;

                if(CommonFunc::uc_Support_AUX)
                {
                    ChannelInfo_Manager::AuxLimitCheckOk = 1;
                }
                fLeftValue = ChannelInfo_Manager::GetLimitValue(thisLimit, ChannelIdx, LIMIT_LEFT_VALUE);
                fRightValue = ChannelInfo_Manager::GetLimitValue(thisLimit, ChannelIdx, LIMIT_RIGHT_VALUE);

                bReached=CompareSign(thisLimit->m_ucCompareSign,fLeftValue,fRightValue);

                if(CommonFunc::uc_Support_AUX)
                {
                    if((thisLimit->m_bLimitFormulaHasAux) && (ChannelInfo_Manager::AuxLimitCheckOk == 0))
                    {
                        bReached = 0;
                    }
                }
								if((thisLimit->m_bAnyValid) && ( BuiltIn_DataManager::b_BuiltInAux) )
                {
                    if(bReached)
                        thisLimitInfo->m_AnyLimitReachedCount = 1;
                }
            }

            thisLimitInfo->m_bReached = bReached;
            thisLimitInfo->m_bChecked = 1;
            Step_Manager_task::LL_CurrentPtr.m_LimitIdx++;

        }
        else   //Find no more limit,return;
            return;
    }
}
signed char Check_Limit4_task::FindNextLogLimit(void)
{
    unsigned char ChannelIdx, ActiveIdx;

    for(;;)//while(1)
    {
        if(Step_Manager_task::bNeedCheck[Step_Manager_task::LL_CurrentPtr.m_ChannelIdx])
        {
            ChannelIdx = Step_Manager_task::LL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
            St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
            unsigned char LimitCount = p_Ctrl->m_ucStepLimitNo + p_Ctrl->m_ucLogLimitNo;
            if(Step_Manager_task::LL_CurrentPtr.m_LimitIdx < p_Ctrl->m_ucStepLimitNo)
                Step_Manager_task::LL_CurrentPtr.m_LimitIdx = p_Ctrl->m_ucStepLimitNo;
            if(Step_Manager_task::LL_CurrentPtr.m_LimitIdx < LimitCount)
            {
                Step_Manager_task::bLogLimitChecked[ChannelIdx] = 1;
                return 1;
            }
            else
            {
                Step_Manager_task::LL_CurrentPtr.m_ChannelIdx++;
                if(Step_Manager_task::LL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    Step_Manager_task::LL_CurrentPtr.m_ChannelIdx = 0;
                Step_Manager_task::LL_CurrentPtr.m_LimitIdx = 0;
                m_CheckedChannelCount_LL++;
                // 				if((m_CheckedChannelCount_LL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                // 					&&(Step_Manager_task::LL_CurrentPtr.m_LimitIdx >= Step_Manager_task::LL_StartPtr.m_LimitIdx))
                if(m_CheckedChannelCount_LL > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    return 0;
            }
        }
        else
        {
            Step_Manager_task::LL_CurrentPtr.m_ChannelIdx++;
            if(Step_Manager_task::LL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                Step_Manager_task::LL_CurrentPtr.m_ChannelIdx = 0;
            Step_Manager_task::LL_CurrentPtr.m_LimitIdx = 0;
            m_CheckedChannelCount_LL++;
            if(m_CheckedChannelCount_LL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                return 0;
        }
    }
}
void Check_Limit4_task::LL_DoLog(unsigned char ChannelIdx)
{
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    unsigned char bReached = 1;
    signed char   AndLimitCount = 0;
    //	St_ScheduleMessage ScheduleMessage;

    //for(unsigned char LimitIdx=ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo;
    //	LimitIdx<(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo
    //	+ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo);
    for(unsigned char LimitIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_StepLimitCount;
            LimitIdx < ChannelInfo_Manager::ChannelStep[ChannelIdx].m_LimitCount;
            LimitIdx++)
    {
        if(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_bAnyValid)
            bReached  = bReached && (ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[LimitIdx].m_AnyLimitReachedCount > 0);
        else
            bReached  = bReached && (ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[LimitIdx].m_bReached);
        if(bReached)
        {
            switch(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_u16GotoStepID)
            {
            case MCU_GOTOLOG:
                Step_Manager_task::SendLog(ChannelIdx);
                if(Smart_Battery::BeSupportSmbus)//add by zc 03.04.2019
                {
                    Step_Manager_task::Log_SMBusData(ChannelIdx);//add by zc 03.04.2019
                }
                for(; AndLimitCount >= 0; AndLimitCount--)
                    if(!ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[LimitIdx - AndLimitCount].m_bAuxCheck)
                        ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[LimitIdx - AndLimitCount].m_bReached = 0;
                return;
            case MCU_GOTOANDCONDICTION:
                AndLimitCount++;
                continue;   //continue the for(;;) loop ... ...
// 			case MCU_GOTOSTOPSTEP:              // 这些东西先不用可屏蔽掉了   dirui 2017/12/18
// 			case MCU_GOTONEXTSTEP:
// 			case MCU_GOTOSAMESTEP:
// 			case MCU_GOTOUNSAFE:
// 			case MCU_GOTOWARNING:
// 			case MCU_GOTOGROUPWAITFORNEXT:
// 			case MCU_GOTOGROUPFOLLOWNEXT:
// 			case MCU_GOTOGLOBALUNSAFE:
// 			case MCU_GOTOGROUPUNSAFE:
            default:
                break;
            }
        }
        else
        {
            AndLimitCount = 0;
            if(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_u16GotoStepID != MCU_GOTOANDCONDICTION)
                bReached = 1;
        }
    }
}

//#if SUPPORT_AUX == 1
void Check_Limit4_task::LimitStatusChanged(unsigned char ChannelIdx, unsigned char bTrue, int SN)
{
    //	unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;

    for(unsigned char LimitIdx = 0;
            //LimitIdx<(ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo
            //+ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo);
            LimitIdx < ChannelInfo_Manager::ChannelStep[ChannelIdx].m_LimitCount;
            LimitIdx++)
    {
        St_LimitInfo * p_Limit = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_limitInfo[LimitIdx];
        if(p_Limit->m_SN == SN)
        {
            if(!p_Limit->m_bAuxCheck)
                continue;

            p_Limit->m_bChecked = 1;
            if(p_Limit->m_bAnyLimit)
            {
                if(bTrue)
                    p_Limit->m_AnyLimitReachedCount++;
                else
                    p_Limit->m_AnyLimitReachedCount--;

                if(p_Limit->m_AnyLimitReachedCount > 0)
                    p_Limit->m_bReached = 1;
                else
                    p_Limit->m_bReached = 0;
            }
            else
                p_Limit->m_bReached = bTrue;

            return;
        }
    }
}
unsigned char Check_Limit4_task::CompareSign(unsigned char m_ucCompareSign,const float fLeftValue, const float fRightValue)
{
    bool IsOK = false;
    switch(m_ucCompareSign)
    {
    case MP_MCS_GREATER_THAN:                // >
        if(fLeftValue > fRightValue)
            IsOK = true;
        break;
    case MP_MCS_LESS_THAN:                   // <
        if(fLeftValue < fRightValue)
            IsOK = true;
        break;
    case MP_MCS_EQUAL_TO:                    // ==
        if(fLeftValue == fRightValue)
            IsOK = true;
        break;
    case MP_MCS_GREATER_THAN_OR_EQUAL_TO:    // >=
        if(fLeftValue >= fRightValue)
            IsOK = true;
        break;
    case MP_MCS_LESS_THAN_OR_EQUAL_TO:       // <=
        if(fLeftValue <= fRightValue)
            IsOK = true;
        break;
    default:
        break;
    }
    return IsOK;
}
void   Check_Limit4_task::GetAuxValue(St_Limit * thisLimit,unsigned char uc_AuxType,unsigned char uc_AuxDxDt,float *fGetValue1,unsigned char ChannelIdx,unsigned char ucValue_Operator, float fValueCoefficient)
{
    if(uc_AuxType != 0XFF)
    {
        unsigned char m_datagroup_idx = 0, chn = 0;
        float AuxValue = 0, AuxDxdtValue = 0;
        bool bGetNormalAuxData = false;// Non-BuildIn Aux data.
        if( BuiltIn_DataManager::b_BuiltInAux)
        {
            switch(uc_AuxType)
            {
            case AUX_TEMPERATURE:
                if(thisLimit->m_bAnyValid)
                {
                    m_datagroup_idx = BuiltIn_DataManager::m_BuiltInAuxInfo.TempDataBank;
                    chn =  ChannelIdx * BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV + m_BuiltInT_AnyLimitChn;
                    AuxValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, (m_BuiltInT_AnyLimitChn + MetaCode_PV_BuiltIn_Data1));
                    AuxDxdtValue = BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_data_group_id][BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_chan_id].aux_dx_dt;
                    m_BuiltInT_AnyLimitChn++;
                    if(m_BuiltInT_AnyLimitChn >= BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV)
                        m_BuiltInT_AnyLimitChn = 0;
                }
                else
                {
                    if(*fGetValue1 < BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV)
                    {
                        m_datagroup_idx = BuiltIn_DataManager::m_BuiltInAuxInfo.TempDataBank;
                        chn = ChannelIdx * BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV + *fGetValue1;
                        AuxValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, *fGetValue1 + MetaCode_PV_BuiltIn_Data1);
                        AuxDxdtValue = BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_data_group_id][BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_chan_id].aux_dx_dt;
                    }
                    else
                    {
                        bGetNormalAuxData = true;
                    }
                }
                break;
            case AUX_VOLTAGE:
                if(thisLimit->m_bAnyValid)
                {
                    m_datagroup_idx = BuiltIn_DataManager::m_BuiltInAuxInfo.VolDataBank;
                    chn =  ChannelIdx * BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV + m_BuiltInV_AnyLimitChn;
                    AuxValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, (m_BuiltInV_AnyLimitChn + MetaCode_PV_BuiltIn_Data1));
                    AuxDxdtValue = BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_data_group_id][BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_chan_id].aux_dx_dt;
                    m_BuiltInV_AnyLimitChn++;
                    if(m_BuiltInV_AnyLimitChn >= BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV)
                        m_BuiltInV_AnyLimitChn = 0;
                }
                else
                {
                    if(*fGetValue1 < BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV)
                    {
                        m_datagroup_idx = BuiltIn_DataManager::m_BuiltInAuxInfo.VolDataBank;
                        chn = ChannelIdx * BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV + *fGetValue1;
                        AuxValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, *fGetValue1 + MetaCode_PV_BuiltIn_Data1);
                        AuxDxdtValue = BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_data_group_id][BuiltIn_DataManager::m_PChn2EChn[m_datagroup_idx][chn].e_chan_id].aux_dx_dt;
                    }
                    else
                    {
                        bGetNormalAuxData = true;
                    }
                }
                break;
            default:
                bGetNormalAuxData = true;
                break;
            }
        }
        else
        {
            bGetNormalAuxData = true;
        }

        if(bGetNormalAuxData)
        {
            if(*fGetValue1 < MAXVIRTUALCHANNEL)
            {
                ST_AUX_DATA   * aux_Value = &Channel_MapAuxInfo::m_AuxData[ChannelIdx][uc_AuxType][(unsigned short )*fGetValue1];
                if(aux_Value->updated_aux_data.m_Char == 0)
                    ChannelInfo_Manager::AuxLimitCheckOk = 0;
                AuxValue = aux_Value->aux_data;
                AuxDxdtValue = aux_Value->aux_dx_dt;
            }
            else
                return;
        }

        if(uc_AuxDxDt)
        {
            *fGetValue1 = ChannelInfo_Manager::CoefficientCalculate(ucValue_Operator,fValueCoefficient, AuxDxdtValue);
        }
        else
        {
            *fGetValue1 = ChannelInfo_Manager::CoefficientCalculate(ucValue_Operator,fValueCoefficient, AuxValue);
        }
    }
}

