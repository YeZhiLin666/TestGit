#include "../Entry/includes.h"
UInt16            AuxCheckLimit4_task::Priority=PRIORITY4;
UInt16            AuxCheckLimit4_task::TaskID=CHECKLIMIT_4_TASK;

int               AuxCheckLimit4_task::m_CheckedLimitCount;
int               AuxCheckLimit4_task::m_CheckedAnyLimitCount;
ST_LIMIT_POINTER  AuxCheckLimit4_task::m_PresentLimit;
ST_LIMIT_POINTER  AuxCheckLimit4_task::m_PresentAnyLimit;

void AuxCheckLimit4_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_CHECKLIMIT4] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_CHECKLIMIT4);
#endif

    Limit_Check(LIMITCHECK_COUNT);
    IfAny_Limit_Check(IFANY_LIMITCHECK_COUNT);

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_CHECKLIMIT4] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_CHECKLIMIT4);
#endif
}

void AuxCheckLimit4_task::MyInit(void)
{
    Reset();
}
void AuxCheckLimit4_task::EndTask(void)
{

}
void AuxCheckLimit4_task::Reset(void)
{
    m_CheckedLimitCount = 0;
    m_PresentLimit.m_IVunitBufIndex = 0;
    m_PresentLimit.m_IVch = 0;
    m_PresentLimit.m_limitIdx = 0;


    m_CheckedAnyLimitCount = 0;
    m_PresentAnyLimit.m_IVunitBufIndex = 0;
    m_PresentAnyLimit.m_IVch = 0;
    m_PresentAnyLimit.m_limitIdx = 0;

}

void AuxCheckLimit4_task::Limit_Check(unsigned char MaxLimitCount)
{
    ST_AUX_LIMIT *MyLimit;
    ST_AUX_DATA MyData;
    m_CheckedLimitCount = 0;
    float fReachedValue;

    while(m_CheckedLimitCount < MaxLimitCount)
    {
        if(FindNextLimit())
        {
            m_CheckedLimitCount++;
            MyLimit = &Aux_DataManager::m_AuxLimitBank[m_PresentLimit.m_IVunitBufIndex][m_PresentLimit.m_IVch][m_PresentLimit.m_limitIdx];

            unsigned char bReached = 0;
            float fLeftValue,fRightValue;

            fRightValue = MyLimit->m_right_value;
            //unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[m_PresentLimit.m_IVunitBufIndex][m_PresentLimit.m_IVch][MyLimit->m_aux_type].m_MapInfo[MyLimit->m_chn_index].m_Index_InAuxUnit;
            unsigned char uc_AuxCh = MyLimit->m_chn_index;  //hepeiqing,20150320
            Aux_DataManager::Aux_GetData(Aux_DataManager::m_IVMCU_ID_Info[m_PresentLimit.m_IVunitBufIndex].m_ID, m_PresentLimit.m_IVch,MyLimit->m_aux_type,uc_AuxCh,&MyData);
            switch(MyLimit->m_left_type)
            {
            case 0:
                fLeftValue = MyData.aux_data;
                break;
            case 1:
                fLeftValue = MyData.aux_dx_dt;
                break;
            case 2:
                fLeftValue = MyData.aux_d2x_dt2;
                break;
            default:
                fLeftValue = MyData.aux_data;
                break;
            }

            switch(MyLimit->m_ComparsionSign)
            {
            case CAN_COMPARE_SIGN_GREATER_THAN:
                bReached = (fLeftValue > fRightValue);
                break;
            case CAN_COMPARE_SIGN_LESS_THAN:
                bReached = (fLeftValue < fRightValue);
                break;
            case CAN_COMPARE_SIGN_GREATER_THAN_OR_EQUAL_TO:
                bReached = (fLeftValue >= fRightValue);
                break;
            case CAN_COMPARE_SIGN_LESS_THAN_OR_EQUAL_TO:
                bReached = (fLeftValue <= fRightValue);
                break;
            case CAN_COMPARE_SIGN_EQUAL:
                bReached = (fLeftValue == fRightValue);
                break;
            default:
                bReached = (fLeftValue >= fRightValue);
                break;
            }

            if(bReached)
                fReachedValue = fLeftValue;

            if(bReached != MyLimit->m_limit_Reached)
            {
                MyLimit->m_limit_Reached = bReached;
                CANBusParse::Aux_RptLimitStatus(MyLimit,fReachedValue);
            }

            m_PresentLimit.m_limitIdx++;
        }
        else   //Find no more limit,return;
            return;
    }
}
signed char AuxCheckLimit4_task::FindNextLimit(void)
{
    if(m_CheckedLimitCount >= Aux_DataManager::m_Count_Limit_AllChannel[m_PresentLimit.m_IVunitBufIndex])
    {
        m_PresentLimit.m_IVunitBufIndex++;
        if(m_PresentLimit.m_IVunitBufIndex >= MAX_IVMCU_PER_SYS)
        {
            m_PresentLimit.m_IVunitBufIndex = 0;
        }
        return 0;   // 这样做恐怕不行，总的limit count数是一个动态变化的。如果中间的IV通道的limit被清除后，可能导致后面IV通道的limit检查被跳过
    }

    while(1)
    {
        if(m_PresentLimit.m_limitIdx < Aux_DataManager::m_AuxLimitCount[m_PresentLimit.m_IVunitBufIndex][m_PresentLimit.m_IVch])
            return 1;
        else
        {
            m_PresentLimit.m_limitIdx = 0;
            m_PresentLimit.m_IVch ++;
            if(m_PresentLimit.m_IVch >= MAXCHANNELNO)
            {
                m_PresentLimit.m_IVch = 0;
                m_PresentLimit.m_IVunitBufIndex++;
                if(m_PresentLimit.m_IVunitBufIndex >= MAX_IVMCU_PER_SYS)
                {
                    m_PresentLimit.m_IVunitBufIndex = 0;
                    return 0;
                }
            }
        }
    }
}


signed char AuxCheckLimit4_task::FindNextAnyLimit(void)
{
    if(m_CheckedAnyLimitCount >= Aux_DataManager::m_Count_AnyLimit_AllChannel[m_PresentAnyLimit.m_IVunitBufIndex])
    {
        m_PresentAnyLimit.m_IVunitBufIndex++;
        if(m_PresentAnyLimit.m_IVunitBufIndex >= MAX_IVMCU_PER_SYS)
        {
            m_PresentAnyLimit.m_IVunitBufIndex = 0;
        }
        return 0;
    }
    while(1)
    {
        if(m_PresentAnyLimit.m_limitIdx < Aux_DataManager::m_AuxIfAnyLimitCount[m_PresentAnyLimit.m_IVunitBufIndex][m_PresentAnyLimit.m_IVch])
            return 1;
        else
        {
            m_PresentAnyLimit.m_limitIdx = 0;
            m_PresentAnyLimit.m_IVch ++;

            if(m_PresentAnyLimit.m_IVch >= MAXCHANNELNO)
            {
                m_PresentAnyLimit.m_IVch = 0;
                m_PresentAnyLimit.m_IVunitBufIndex++;
                if(m_PresentAnyLimit.m_IVunitBufIndex >= MAX_IVMCU_PER_SYS)
                {
                    m_PresentAnyLimit.m_IVunitBufIndex = 0;
                    return 0;
                }
            }

        }
    }
}

void AuxCheckLimit4_task::IfAny_Limit_Check(unsigned char MaxLimitCount)
{
    ST_AUX_IFANY_LIMIT *MyLimit;
    ST_AUX_DATA MyData;
    m_CheckedAnyLimitCount = 0;
    unsigned char bReachedNew;
    float fReachedValue = 0.0;

    while(m_CheckedAnyLimitCount < MaxLimitCount)
    {
        if(FindNextAnyLimit())
        {
            m_CheckedAnyLimitCount++;
            MyLimit = &Aux_DataManager::m_AuxIfAnyLimitBank[m_PresentAnyLimit.m_IVunitBufIndex][m_PresentAnyLimit.m_IVch][m_PresentAnyLimit.m_limitIdx];

            unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(MyLimit->m_aux_type);
            if(TempAuxTypeIdex == 0xFF)//辅助限制
                continue ;

            unsigned char bReached = 0;
            float fLeftValue,fRightValue,fReachValue = 0;
            fRightValue = MyLimit->m_right_value;



            for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[m_PresentAnyLimit.m_IVunitBufIndex][m_PresentAnyLimit.m_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //检查该类型的所通道
                //找出该IV该类型辅助的所有通道进行轮询	,
            {
                unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[m_PresentAnyLimit.m_IVunitBufIndex][m_PresentAnyLimit.m_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
                Aux_DataManager::Aux_GetData( Aux_DataManager::m_IVMCU_ID_Info[m_PresentAnyLimit.m_IVunitBufIndex].m_ID, m_PresentAnyLimit.m_IVch, MyLimit->m_aux_type, uc_AuxCh, &MyData);

                switch(MyLimit->m_left_type)
                {
                case 0:
                    fLeftValue = MyData.aux_data;
                    break;
                case 1:
                    fLeftValue = MyData.aux_dx_dt;
                    break;
                case 2:
                    fLeftValue = MyData.aux_d2x_dt2;
                    break;
                default:
                    fLeftValue = MyData.aux_data;
                    break;
                }

                switch(MyLimit->m_ComparsionSign)
                {
                case CAN_COMPARE_SIGN_GREATER_THAN:
                    bReached = (fLeftValue > fRightValue);
                    break;
                case CAN_COMPARE_SIGN_LESS_THAN:
                    bReached = (fLeftValue < fRightValue);
                    break;
                case CAN_COMPARE_SIGN_GREATER_THAN_OR_EQUAL_TO:
                    bReached = (fLeftValue >= fRightValue);
                    break;
                case CAN_COMPARE_SIGN_LESS_THAN_OR_EQUAL_TO:
                    bReached = (fLeftValue <= fRightValue);
                    break;
                case CAN_COMPARE_SIGN_EQUAL:
                    bReached = (fLeftValue == fRightValue);
                    break;
                default:
                    bReached = (fLeftValue >= fRightValue);
                    break;
                }


                if (uc_AuxCh <= 63)
                    SetBit((unsigned char *)&MyLimit->m_limit_Reached[0],uc_AuxCh,bReached);
                else
                    SetBit((unsigned char *)&MyLimit->m_limit_Reached[1],uc_AuxCh,bReached);


                if(bReached)
                    fReachedValue = fLeftValue;
            }
            bReachedNew = MyLimit->m_limit_Reached[0] || MyLimit->m_limit_Reached[1];
            if(Aux_DataManager::m_MapFromPC[m_PresentAnyLimit.m_IVunitBufIndex][m_PresentAnyLimit.m_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap > 0)
            {
                if(bReachedNew != MyLimit->m_bReached)
                {
                    MyLimit->m_bReached = bReachedNew;
                    //获取unitID
                    if(Aux_DataManager::m_IVMCU_ID_Info[m_PresentAnyLimit.m_IVunitBufIndex].m_Valid)
                    {
                        MyLimit->m_Source_CANID = Aux_DataManager::m_IVMCU_ID_Info[m_PresentAnyLimit.m_IVunitBufIndex].m_ID;
                        CANBusParse::Aux_RptAnyLimitStatus(MyLimit,fReachedValue);
                    }
                }
            }

            m_PresentAnyLimit.m_limitIdx++;
        }
        else   //Find no more limit,return;
            return;
    }
}
