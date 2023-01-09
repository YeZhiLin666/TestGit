// *****************************************************
// File Name: CheckLimit_Non_V.cpp
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         检查脉冲与电压无关的Limit
//							loglimit暂时不在此处处理 20140815
// Others:
// History：
//
// ******************************************************/
#include "includes.h"
UInt16 CheckLimit_Non_V_task::Priority=PRIORITY4;
UInt16 CheckLimit_Non_V_task::TaskID=CHECKLIMIT_NON_V_TASK;

// unsigned char CheckLimit_Non_V_task::m_CheckedChannelCount_LL;
unsigned char CheckLimit_Non_V_task::m_StartCheckLimitID;


void CheckLimit_Non_V_task::MyInit(void)
{

    m_StartCheckLimitID = 0;  //开始检查的LimitID  默认值为2

}
void CheckLimit_Non_V_task::DoWork(void)
{

    S_Non_V_LimitCheck(CHECK_NON_V_LIMITCOUNT);     //loglimit暂时不在这里面处理  20140815

}


void CheckLimit_Non_V_task::S_Non_V_LimitCheck(unsigned char MaxLimitCount)
{
    unsigned char ChannelIdx,ActiveIdx;
    unsigned char LimitCount = 0;

    if(Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx==0)
        Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx = m_StartCheckLimitID; //不能指定从0开始，即使没有V limit // if == 0, += equals = here and saves 6 bytes

    while(LimitCount < MaxLimitCount)
    {
        if(Check_V_Limit_task::FindNextStepLimit())
        {
            LimitCount++;
            ChannelIdx = Step_Manager_task::PULSE_SL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;

            unsigned char bReached = 0,ucLeftType,ucRightType;
            float fLeftValue,fRightValue;
            float f_LeftMaxCompValue,f_LeftMinCompValue;  //用于比较的最大最小值  dirui 20140820
            float f_RightMaxCompValue,f_RightMinCompValue;  //用于比较的最大最小值  dirui 20140820

            St_ChannelStep * thisChnStep = &ChannelInfo_Manager::ChannelStep[ChannelIdx];		// using this as root for another 2 address locating saves a little bit.
            St_Limit * thisLimit = &thisChnStep->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx];

            ucLeftType = thisLimit->m_ucLeftValue_DataType;
            fLeftValue = thisLimit->m_fLeftValue;
			unsigned char ucLeftValue_Operator = thisLimit->m_ucLeftValue_Operator;// 20190917 yy
            float fLeftValueCoefficient = thisLimit->m_fLeftValueCoefficient;
            ucRightType = thisLimit->m_ucRightValue_DataType;
            fRightValue = thisLimit->m_fRightValue;
			unsigned char ucRightValue_Operator = thisLimit->m_ucRightValue_Operator;// 20190917 yy
            float fRightValueCoefficient = thisLimit->m_fRightValueCoefficient;

            if((ucLeftType==MP_DATA_TYPE_METAVALUE && fLeftValue == MetaCode_PV_Voltage)  //Checklimit_Non_V 不支持voltage类型的Limit
                    ||(ucRightType==MP_DATA_TYPE_METAVALUE && fRightValue == MetaCode_PV_Voltage))
            {   //modified by hepeiqing,20140924.   //return if this is a Fast_Limit,
                Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx++;
                return;
            }

// 			switch(ucLeftType)  //calculate left value;
// 			{
// 			case MP_DATA_TYPE_METAVALUE:
//                 unsigned char uc_safetype = SAFETY_I;
//                 unsigned char uc_PV = true;
// 				switch(int(fLeftValue))
// 				{
// 				case MetaCode_PV_Current:
// 					f_LeftMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_I);
// 					f_LeftMinCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_I);
//  					break;
// 				case MetaCode_PV_Voltage:
// 					f_LeftMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_V);
// 					f_LeftMinCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_V);
// 					break;
// 				default:
// 					f_LeftMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,fLeftValue);
// 					f_LeftMinCompValue = f_LeftMaxCompValue;
// 					break;
// 				}
// 				break;
// 			case MP_DATA_TYPE_FORMULA:
// 				f_LeftMaxCompValue= ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,ucLeftType,fLeftValue);
// 				f_LeftMinCompValue=f_LeftMaxCompValue;
// 				break;
// 			default:
// 				f_LeftMaxCompValue=fLeftValue;
// 				f_LeftMinCompValue=fLeftValue;
// 				break;
// 			}
// 			switch(ucRightType)   //calculate right value;
// 			{
// 			case MP_DATA_TYPE_METAVALUE:
//                 unsigned char uc_safetype = SAFETY_I;
//                 unsigned char uc_PV = true;
// 				switch(int(fRightValue))
// 				{
// 				case MetaCode_PV_Current:
// 					f_RightMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_I);
// 					f_RightMinCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_I);
// 					break;
// 				case MetaCode_PV_Voltage:
// 					f_RightMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MAXCOMPVALUE,SAFETY_V);
// 					f_RightMinCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable4(ChannelIdx,MINCOMPVALUE,SAFETY_V);
// 					break;
// 				default:
// 					f_RightMaxCompValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,fRightValue);
// 					f_RightMinCompValue=f_RightMaxCompValue;
// 					break;
// 				}
// 				break;
// 			case MP_DATA_TYPE_FORMULA:
// 				f_RightMaxCompValue = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,ucRightType,fRightValue);
// 				f_RightMinCompValue=f_RightMaxCompValue;
// 				break;
// 			default:
// 				f_RightMaxCompValue = fRightValue;
// 				f_RightMinCompValue = fRightValue;
// 				break;
// 			}
            float * CompValue[2];
            CompValue[0] = &f_LeftMaxCompValue;
            CompValue[1] = &f_LeftMinCompValue;
            S_Non_V_CalculateValue(ChannelIdx,ucLeftType,ucLeftValue_Operator,
			                        fLeftValueCoefficient,fLeftValue,CompValue);  //calculate left value;

            CompValue[0] = &f_RightMaxCompValue;
            CompValue[1] = &f_RightMinCompValue;
            S_Non_V_CalculateValue(ChannelIdx,ucRightType,ucRightValue_Operator,
			                       fRightValueCoefficient,fRightValue,CompValue);//calculate right value;

            switch(thisLimit->m_ucCompareSign)			// didn't modify this to bReached = 1 : 2 coz it doesn't save for simple case process.
            {
            case MP_MCS_GREATER_THAN:                // >
                if(f_LeftMaxCompValue > f_RightMaxCompValue)
                {
                    thisLimit->m_fLeft_Value = f_LeftMaxCompValue;
                    bReached = 1;   // 如果提到外边的话，脉冲就跑不了。
                }
                break;
            case MP_MCS_LESS_THAN:                   // <
                if(f_LeftMinCompValue < f_RightMinCompValue)
                {
                    thisLimit->m_fLeft_Value = f_LeftMinCompValue;
                    bReached = 1;
                }
                break;
            case MP_MCS_EQUAL_TO:                    // ==
                if(f_LeftMaxCompValue == f_RightMaxCompValue)
                {
                    thisLimit->m_fLeft_Value = f_LeftMaxCompValue;
                    bReached = 1;
                }
                break;
            case MP_MCS_GREATER_THAN_OR_EQUAL_TO:    // >=
                if(f_LeftMaxCompValue >= f_RightMaxCompValue)
                {
                    thisLimit->m_fLeft_Value = f_LeftMaxCompValue;
                    bReached = 1;
                }
                break;
            case MP_MCS_LESS_THAN_OR_EQUAL_TO:       // <=
                if(f_LeftMinCompValue <= f_RightMinCompValue)
                {
                    thisLimit->m_fLeft_Value = f_LeftMinCompValue;
                    bReached = 1;
                }
                break;
            default:
// 				bReached = 0;
                break;
            }

            St_LimitInfo *thisLimitInfo = &thisChnStep->m_limitInfo[Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx];
            thisLimitInfo->m_bReached = bReached;
            thisLimitInfo->m_bChecked = 1;
            Step_Manager_task::PULSE_SL_CurrentPtr.m_LimitIdx++;
        }
        else
            return;
    }
}

void CheckLimit_Non_V_task::S_Non_V_CalculateValue(unsigned char uc_ChannelNo,unsigned char ucValueType,unsigned char uc_Operator,float fCoefficient,float fValue,float * CompValue[2])
{
    switch(ucValueType)  //calculate value;
    {
    case MP_DATA_TYPE_METAVALUE:
        unsigned char uc_safetype = SAFETY_I;
        unsigned char uc_PV = true;
        switch(int(fValue))
        {
        case MetaCode_PV_Current:
            break;
        case MetaCode_PV_Voltage:
            uc_safetype = SAFETY_V;
            break;
        default:
            uc_PV = false;
            break;
        }

        if(uc_PV)
        {
            * CompValue[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable4(uc_ChannelNo,MAXCOMPVALUE,uc_safetype);
            * CompValue[0]  = ChannelInfo_Manager::CoefficientCalculate(uc_Operator,fCoefficient, * CompValue[0] );  // 20190917 yy
            * CompValue[1] = ChannelInfo_Manager::Read_SpecifiedMetavariable4(uc_ChannelNo,MINCOMPVALUE,uc_safetype);
			* CompValue[1]  = ChannelInfo_Manager::CoefficientCalculate(uc_Operator,fCoefficient, * CompValue[1] );  // 20190917 yy
        }
        else
        {
            * CompValue[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,fValue);
			* CompValue[0]  = ChannelInfo_Manager::CoefficientCalculate(uc_Operator,fCoefficient, * CompValue[0] );  // 20190917 yy
            * CompValue[1] = * CompValue[0];
        }
        break;
    case MP_DATA_TYPE_FORMULA:
	case MP_DATA_TYPE_ADVFORMULA:	
        * CompValue[0] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo,ucValueType,uc_Operator,fCoefficient,fValue);
        * CompValue[1] = * CompValue[0];
        break;
    default:
        * CompValue[0] = fValue;
        * CompValue[1] = fValue;
        break;
    }
}

