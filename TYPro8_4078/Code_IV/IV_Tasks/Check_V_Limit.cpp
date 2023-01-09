// *****************************************************
// File Name: Check_V_Limit.cpp
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         检查脉冲与电压相关的Limit
// Others:
// History：
//
// ******************************************************/
#include "includes.h"
UInt16 Check_V_Limit_task::Priority=PRIORITY4;
UInt16 Check_V_Limit_task::TaskID=CHECK_V_LIMIT_TASK;

float Check_V_Limit_task::Max_PulseVoltage;
float Check_V_Limit_task::Min_PulseVoltage;

float Check_V_Limit_task::Max_PulseCurrent;
float Check_V_Limit_task::Min_PulseCurrent;

float Check_V_Limit_task::Max_PulsePower;
float Check_V_Limit_task::Min_PulsePower;

unsigned char Check_V_Limit_task::m_CheckedChannelCount_SL;

void Check_V_Limit_task::DoWork(void)
{

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CHECK_V_LIMIT] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CHECK_V_LIMIT);
#endif
    S_V_LimitCheck(PULSE_V_LIMITCHECK_COUNT);

    if((IVSDL::Buffer.m_u32Length >= MAX_PACK_LOGPOINT) || (CommonFunc::GetTimeTicks(&IVSDL::m_IntervalTime_SDL) > 5000))//zyx 2202251
    {
        CommonFunc::GetTimeTicksUpdate( &IVSDL::m_IntervalTime_SDL);
        EthernetParse::MPReportData(0, SEND_LOG_SDL);   //调用第二信道发送数据到上位机
    }
    
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CHECK_V_LIMIT] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CHECK_V_LIMIT);
#endif
}
// void Check_V_Limit_task::EndTask(void)
// {


// }

void Check_V_Limit_task::MyInit(void)
{
    m_CheckedChannelCount_SL=0;

}

void Check_V_Limit_task::S_V_LimitCheck(unsigned char MaxLimitCount)
{
    unsigned char ChannelIdx,ActiveIdx;

    CheckLimit_Non_V_task::m_StartCheckLimitID = 0;  //重新找下开始点，防止一直累加
    for(unsigned char i=0; i<2; i++) //V limit可能是0～2
    {
        ChannelIdx = Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx;
        if(!Step_Manager_task::bNeedCheck[ChannelIdx])
            continue;    //add by hepeiqing ,2015/01/15
        St_ChannelStep *thisStep = &ChannelInfo_Manager::ChannelStep[ChannelIdx];
        ActiveIdx = thisStep->m_ucActiveIdx;
        St_Limit *thisLimit = &thisStep->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[i];

        unsigned char bReached = 0,ucLeftType,ucRightType;
        float fLeftValue,fRightValue;
        float f_MaxLeftCompValue,f_MinLeftCompValue,f_MaxRightValue,f_MinRightValue;
        ucLeftType = thisLimit->m_ucLeftValue_DataType;
        fLeftValue = thisLimit->m_fLeftValue;
        ucRightType = thisLimit->m_ucRightValue_DataType;
        fRightValue = thisLimit->m_fRightValue;

        if((ucLeftType==MP_DATA_TYPE_METAVALUE && fLeftValue != MetaCode_PV_Voltage)
                ||(ucRightType==MP_DATA_TYPE_METAVALUE && fRightValue != MetaCode_PV_Voltage))   //Check_V_Limit 只支持电压类型 （暂时只判断PV_Voltage,逐步要细化）dirui 20140813
        {   //modified by hepeiqing,20140924.   //return if not a Fast_Limit,
            //	Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx++;
            return;
        }

//#if SUPPORT_AUX == 1
        if(CommonFunc::uc_Support_AUX)
            ChannelInfo_Manager::AuxLimitCheckOk = 1;
//#endif

        St_Pulse_Point * pulse_Point = &ChannelInfo_Manager::PulseLimitArriveData[ChannelIdx];

// 		switch(ucLeftType)  //calculate left value;
// 		{
// 		case MP_DATA_TYPE_METAVALUE:
// 			f_MaxLeftCompValue = Pulse::fMaxVoltage;//ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_V);
// 			f_MinLeftCompValue = Pulse::fMinVoltage;//ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_V);
// 			break;
// 		default:
// 			f_MaxLeftCompValue = fLeftValue;
// 			f_MinLeftCompValue = fLeftValue;
// 			break;
// 		}

// 		switch(ucRightType)   //calculate right value;
// 		{
// 		case MP_DATA_TYPE_METAVALUE:
// 			f_MaxRightValue = Pulse::fMaxVoltage;//ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_V);
// 			f_MinRightValue = Pulse::fMinVoltage;//ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_V);
// 			break;
// 		default:
// 			f_MaxRightValue = fRightValue;
// 			f_MinRightValue = fRightValue;
// 			break;
// 		}

        float * CompValue[2];
        CompValue[0] = &f_MaxLeftCompValue;
        CompValue[1] = &f_MinLeftCompValue;
        Check_V_Limit_task::S_V_CalculateValue(ucLeftType,fLeftValue,CompValue);  //calculate left value;

        CompValue[0] = &f_MaxRightValue;
        CompValue[1] = &f_MinRightValue;
        Check_V_Limit_task::S_V_CalculateValue(ucRightType,fRightValue,CompValue);  //calculate right value;

        switch(thisLimit->m_ucCompareSign) // Temporarily set bReach to either 1 or 2, and reset it to 1 after switch section. 1: Min process 2: Max process
        {
        case MP_MCS_GREATER_THAN:                // >
            if(f_MaxLeftCompValue > f_MaxRightValue)
                bReached = 1;//2;//zyx 190321 pluse step limit can't reach
            break;
        case MP_MCS_LESS_THAN:                   // <
            if(f_MinLeftCompValue < f_MinRightValue)
                bReached = 1;
            break;
        case MP_MCS_EQUAL_TO:                    // ==
            if(f_MinLeftCompValue == f_MaxRightValue)
                bReached= 1;//2;
            break;
        case MP_MCS_GREATER_THAN_OR_EQUAL_TO:    // >=
            if(f_MaxLeftCompValue >= f_MaxRightValue)
                bReached = 1;//2;
            break;
        case MP_MCS_LESS_THAN_OR_EQUAL_TO:       // <=
            if(f_MinLeftCompValue<= f_MinRightValue)
                bReached = 1;
            break;
        default:
            bReached = 0;
            break;
        }

        switch(bReached)
        {
        case 1:
            thisLimit->m_fLeft_Value = f_MinLeftCompValue;
            if(!pulse_Point->bPulseStepEnded)
            {
                pulse_Point->fValue=Pulse::fMinVoltage_Curr;
                pulse_Point->fValue2=Pulse::fMinVoltage;
                pulse_Point->bCriticalPoint=1;    //作为关键记录点
            }
            break;
        case 2:
            thisLimit->m_fLeft_Value = f_MaxLeftCompValue;
            if(!pulse_Point->bPulseStepEnded)
            {
                pulse_Point->fValue=Pulse::fMaxVoltage_Curr;
                pulse_Point->fValue2=Pulse::fMaxVoltage;
                pulse_Point->bCriticalPoint=1;    //作为关键记录点
            }
            break;
        default:
            break;
        }

//#if SUPPORT_AUX == 1    //???????
        if(CommonFunc::uc_Support_AUX)
        {
            if((thisLimit->m_bLimitFormulaHasAux) && (ChannelInfo_Manager::AuxLimitCheckOk == 0))
            {
                bReached = 0;
            }
        }
//#else
//#endif

        St_LimitInfo *thisLimitInfo = &thisStep->m_limitInfo[i];
        thisLimitInfo->m_bReached = bReached;
        thisLimitInfo->m_bChecked = 1;
        //Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx++;
        CheckLimit_Non_V_task::m_StartCheckLimitID++;   //记录有几个V limit
    }
}

signed char Check_V_Limit_task::FindNextStepLimit(void)
{
    unsigned char ChannelIdx,ActiveIdx;

    for(;;)//while(1)
    {
        if(Step_Manager_task::PULSE_bNeedCheck[Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx])
        {
            ChannelIdx = Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
            if(Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx < ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo)
            {
                Step_Manager_task::PULSE_bStepLimitChecked[ChannelIdx] = 1;
                return 1;
            }
            else
            {
// 				Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx++;
// 				if(Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
// 					Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx = 0;
// 				Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx = 0;
// 				m_CheckedChannelCount_SL++;

                S_V_ChannelIdx();

                if(m_CheckedChannelCount_SL > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    return 0;
            }
        }
        else
        {
// 			Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx++;
// 			if(Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
// 				Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx = 0;
// 			Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx = 0;
// 			m_CheckedChannelCount_SL++;

            S_V_ChannelIdx();

            if(m_CheckedChannelCount_SL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                return 0;
        }
    }

}

void Check_V_Limit_task::S_V_CalculateValue(unsigned char ucValueType,float fValue,float * CompValue[2])
{
    switch(ucValueType)  //calculate value;
    {
    case MP_DATA_TYPE_METAVALUE:
        * CompValue[0] = Pulse::fMaxVoltage;;
        * CompValue[1] = Pulse::fMinVoltage;
        break;
    default:
        * CompValue[0] = fValue;
        * CompValue[1] = fValue;
        break;
    }
}

void Check_V_Limit_task::S_V_ChannelIdx(void)
{
    Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx++;
    if(Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx = 0;
    Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx = 0;
    m_CheckedChannelCount_SL++;
}

