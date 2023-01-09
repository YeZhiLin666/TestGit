///////////////////////////////////////////////////////////////////////////////////
//File Name: Pulse_LogLimit.cpp
//Version:        V1.0
//Date:                2014/8/21
//Description:
//                     Pulse Log Limit Check
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
UInt16        Pulse_LogLimit_task::Priority=PRIORITY4;
UInt16        Pulse_LogLimit_task::TaskID=PULSE_LOGLIMIT_TASK;

unsigned char Pulse_LogLimit_task::m_CheckedChannelCount_LL;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
PULSE_RAW_DATA  Pulse_LogLimit_task::Backup_raw_data[2][2][MAX_PULSE_STAGE_NUM];
MsTime Pulse_LogLimit_task::LastReportTime[MAXADCHANNELNO]; //放在pulse拟合时间和实际OStime不匹配

#pragma arm section //结束外存变量定义  

void Pulse_LogLimit_task::DoWork(void)
{
    Pulse_LL_Check(PULSE_LOGLIMITCHECK_COUNT);

    if(m_CheckedChannelCount_LL == 0)
        return;

    for(unsigned char ChannelIdx = 0; ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        if(Step_Manager_task::bNeedCheck[ChannelIdx] && Step_Manager_task::bLogLimitChecked[ChannelIdx])
        {
            Pulse_LL_DoLog(ChannelIdx);
            Step_Manager_task::bLogLimitChecked[ChannelIdx] = 0;
        }
    }
}
// void Pulse_LogLimit_task::EndTask(void)
// {

// }
void Pulse_LogLimit_task::MyInit(void)
{
    m_CheckedChannelCount_LL=0;
    memset(LastReportTime, 0, sizeof(MsTime)*MAXADCHANNELNO);
}
void Pulse_LogLimit_task::Pulse_LL_Check(unsigned char MaxLimitCount)
{
    unsigned char ChannelIdx = 0,ActiveIdx;
    unsigned char LimitCount = 0;
    bool IsJumpCheck = false;
    bool IsCheck = true;

    while(LimitCount < MaxLimitCount)
    {

        if(Pulse_FindNextLogLimit())
        {
            LimitCount++;
            ChannelIdx = Step_Manager_task::LL_CurrentPtr.m_ChannelIdx;
            St_ChannelStep * p_Step = &ChannelInfo_Manager::ChannelStep[ChannelIdx];
            ActiveIdx = p_Step->m_ucActiveIdx;
            unsigned char bReached = 0;
            St_Limit * p_Limit = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[Step_Manager_task::LL_CurrentPtr.m_LimitIdx];
            Step_Manager_task::bLogLimitChecked[ChannelIdx] = 1;

            if(!IsJumpCheck)
            {

                unsigned char ucLeftType,ucRightType;
                float fLeftValue,fRightValue;

                ucLeftType = p_Limit->m_ucLeftValue_DataType;
                fLeftValue = p_Limit->m_fLeftValue;
                unsigned char ucLeftValue_Operator = p_Limit->m_ucLeftValue_Operator;// 20190917 yy
                float fLeftValueCoefficient = p_Limit->m_fLeftValueCoefficient;
                ucRightType = p_Limit->m_ucRightValue_DataType;
                fRightValue = p_Limit->m_fRightValue;
                unsigned char ucRightValue_Operator = p_Limit->m_ucRightValue_Operator;// 20190917 yy
                float fRightValueCoefficient = p_Limit->m_fRightValueCoefficient;

                if(ucLeftType==MP_DATA_TYPE_METAVALUE)   //判断MV种类
                {
                    switch (int(fLeftValue))
                    {
                    case MetaCode_PV_Steptime:
                    case MetaCode_DV_Time:
                    case MetaCode_PV_PulseCycle:
                    case MetaCode_DV_PulseCycle:
                        break;
                    default:
                        bReached = 0;
                        IsCheck = false;
                        break;
                    }
                }
                if(ucRightType==MP_DATA_TYPE_METAVALUE )
                {
                    switch (int(fRightValue))
                    {
                    case MetaCode_PV_Steptime:
                    case MetaCode_DV_Time:
                    case MetaCode_PV_PulseCycle:
                    case MetaCode_DV_PulseCycle:
                        break;
                    default:
                        bReached = 0;
                        IsCheck = false;
                        break;
                    }
                }

                if(IsCheck)
                {
                    switch(ucLeftType)  //calculate left value;
                    {
                    case MP_DATA_TYPE_METAVALUE:
                        fLeftValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,fLeftValue);
                        fLeftValue = ChannelInfo_Manager::CoefficientCalculate(ucLeftValue_Operator,fLeftValueCoefficient, fLeftValue);  // 20190917 yy
                        break;
                    case MP_DATA_TYPE_FORMULA:
                        fLeftValue = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,ucLeftType,ucLeftValue_Operator,fLeftValueCoefficient,fLeftValue);
                        break;
                    case MP_DATA_TYPE_FIGURE:
                    default:
                        break;
                    }
                    switch(ucRightType)   //calculate right value;
                    {
                    case MP_DATA_TYPE_FIGURE:
                        break;
                    case MP_DATA_TYPE_METAVALUE:
                        fRightValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,fRightValue);
                        fRightValue = ChannelInfo_Manager::CoefficientCalculate(ucRightValue_Operator,fRightValueCoefficient, fRightValue);  // 20190917 yy
                        break;
                    case MP_DATA_TYPE_FORMULA:
                        fRightValue = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,ucRightType,ucRightValue_Operator,fRightValueCoefficient,fRightValue);
                        break;
                    default:
                        break;
                    }
                    switch(p_Limit->m_ucCompareSign)
                    {
                    case MP_MCS_GREATER_THAN:                // >
                        if(fLeftValue > fRightValue)
                            bReached = 1;
                        break;
                    case MP_MCS_LESS_THAN:                   // <
                        if(fLeftValue < fRightValue)
                            bReached = 1;
                        break;
                    case MP_MCS_EQUAL_TO:                    // ==
                        if(fLeftValue == fRightValue)
                            bReached= 1;
                        break;
                    case MP_MCS_GREATER_THAN_OR_EQUAL_TO:    // >=
                        if(fLeftValue >= fRightValue)
                            bReached = 1;
                        break;
                    case MP_MCS_LESS_THAN_OR_EQUAL_TO:       // <=
                        if(fLeftValue<= fRightValue)
                            bReached = 1;
                        break;
                    default:
                        bReached = 0;
                        break;
                    }
                }
                else
                    IsJumpCheck = true;
                p_Step->m_limitInfo[Step_Manager_task::LL_CurrentPtr.m_LimitIdx].m_bReached = bReached;
                p_Step->m_limitInfo[Step_Manager_task::LL_CurrentPtr.m_LimitIdx].m_bChecked = 1;
                Step_Manager_task::LL_CurrentPtr.m_LimitIdx++;
            }
        }
        else
            return;
    }
}
signed char Pulse_LogLimit_task::Pulse_FindNextLogLimit(void)
{
    unsigned char ChannelIdx,ActiveIdx;

    for(;;)//while(1)
    {
        if(Step_Manager_task::bNeedCheck[Step_Manager_task::LL_CurrentPtr.m_ChannelIdx])
        {
            ChannelIdx = Step_Manager_task::LL_CurrentPtr.m_ChannelIdx;
            ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
            unsigned char LimitCount = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo
                                       + ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo;
            if(Step_Manager_task::LL_CurrentPtr.m_LimitIdx < ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo)
                Step_Manager_task::LL_CurrentPtr.m_LimitIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo;
            if(Step_Manager_task::LL_CurrentPtr.m_LimitIdx < LimitCount)
            {
                Step_Manager_task::bLogLimitChecked[ChannelIdx] = 1;
                return 1;
            }
            else
            {
// 				Step_Manager_task::LL_CurrentPtr.m_ChannelIdx++;
// 				if(Step_Manager_task::LL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
// 					Step_Manager_task::LL_CurrentPtr.m_ChannelIdx = 0;
// 				Step_Manager_task::LL_CurrentPtr.m_LimitIdx = 0;
// 				m_CheckedChannelCount_LL++;
                Pulse_LL_FindNextLimit();
                if(m_CheckedChannelCount_LL > ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    return 0;
            }
        }
        else
        {
// 			Step_Manager_task::LL_CurrentPtr.m_ChannelIdx++;
// 			if(Step_Manager_task::LL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
// 				Step_Manager_task::LL_CurrentPtr.m_ChannelIdx = 0;
// 			Step_Manager_task::LL_CurrentPtr.m_LimitIdx = 0;
// 			m_CheckedChannelCount_LL++;
            Pulse_LL_FindNextLimit();
            if(m_CheckedChannelCount_LL >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                return 0;
        }
    }
}

void        Pulse_LogLimit_task::Pulse_LL_DoLog(unsigned char ChannelIdx)  //大部分延续非脉冲的现有处理
{
    unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    unsigned char bReached = 1;
    signed char   AndLimitCount = 0;
    //	St_ScheduleMessage ScheduleMessage;
    St_ChannelStep * p_Step = &ChannelInfo_Manager::ChannelStep[ChannelIdx];

    for(unsigned char LimitIdx = p_Step->m_StepLimitCount;
            LimitIdx < p_Step->m_LimitCount;
            LimitIdx++)
    {
        if(p_Step->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_bAnyValid)
            bReached  = bReached && (p_Step->m_limitInfo[LimitIdx].m_AnyLimitReachedCount > 0);
        else
            bReached  = bReached && (p_Step->m_limitInfo[LimitIdx].m_bReached);
        if(bReached)
        {
            switch(p_Step->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_u16GotoStepID)
            {
            case MCU_GOTOLOG:
                if(ChannelInfo_Manager::PulseLimitArriveData[ChannelIdx].bCriticalPoint==0)   //若Limit触发了关键点的话，那么不必再Log了，防止出现顺序不一致   dirui 2014/12/26
                    Pulse_LogLimit_task::SendLog(ChannelIdx);
                for(; AndLimitCount >= 0; AndLimitCount--)
                    if(!p_Step->m_limitInfo[LimitIdx - AndLimitCount].m_bAuxCheck)
                        p_Step->m_limitInfo[LimitIdx - AndLimitCount].m_bReached = 0;
                return;
            case MCU_GOTOANDCONDICTION:
                AndLimitCount++;
                continue;   //continue the for(;;) loop ... ...

            default:
                break;
            }
        }
        else
        {
            AndLimitCount = 0;
            if(p_Step->m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[LimitIdx].m_u16GotoStepID != MCU_GOTOANDCONDICTION)
                bReached = 1;
        }
    }
}
void Pulse_LogLimit_task::SendLog(unsigned char uc_ChannelNo)
{
    //根据Log模式是一个点还是两个点，将电压电流数据上报，若原始数据还没有转化成电压电流值，则转换
    //一旦要log数据了要将已满的buffer里的数据立即copy过来然后处理。
    unsigned long BufSize = sizeof(PULSE_RAW_DATA) * Pulse::m_Info.m_StageCount;
    unsigned BufIdx = !Pulse::active_index;   //取出非active缓冲区
    if(!Pulse::buf_info[BufIdx].m_bFull)
        return;  //缓冲区没有满，退出
    if(Pulse::buf_info[BufIdx].m_bDataLogged)
        return;   //如果这个缓冲区已经被LOG了，则不需要重复LOG

    St_Pulse_PackInfo * st_PackInfo = (St_Pulse_PackInfo *) &EthernetParse::m_Point_Temp[0];
    st_PackInfo->m_wChannelNumber = uc_ChannelNo;
    st_PackInfo->m_btIfPartialPusle = 0;
    st_PackInfo->m_dwPulseCycleID = Pulse::buf_info[BufIdx].m_CycleIdx;
    st_PackInfo->m_PulseStartTime = Pulse::buf_info[BufIdx].m_CycleStartTime;
    st_PackInfo->m_btFirstPointID = 0;
    st_PackInfo->m_btIfHaveCapEng = 1;
    st_PackInfo->m_btbuf_Idex = 0 / 1;
    if(BufIdx != !Pulse::active_index)
        return;
    memcpy(&Backup_raw_data[0][SAVE_2_HEAD][0],&Pulse::raw_data[BufIdx][SAVE_2_HEAD][0],BufSize);
    memcpy(&Backup_raw_data[0][SAVE_2_TAIL][0],&Pulse::raw_data[BufIdx][SAVE_2_TAIL][0],BufSize);
    Pulse::buf_info[BufIdx].m_bDataLogged = 1;
    //memcpy(&Backup_raw_data[0][save][0],&Pulse::raw_data[0][0][0],sizeof(PULSE_RAW_DATA)*2*2*MAX_PULSE_STAGE_NUM); //[缓冲区1/2][头/尾][台阶数] [2][2][];
    //待完善
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_fValue         = 0.0f;

    for(unsigned char StgIdx = 0; StgIdx < Pulse::m_Info.m_StageCount; StgIdx++)
    {
        PULSE_RAW_DATA * p_raw_data_head = &Backup_raw_data[0][SAVE_2_HEAD][StgIdx];
        PULSE_RAW_DATA * p_raw_data_tail = &Backup_raw_data[0][SAVE_2_TAIL][StgIdx];
        if((!p_raw_data_head->m_bSampleUpdated) && (!p_raw_data_head->m_bDataConverted))
            return;
        if((!p_raw_data_tail->m_bSampleUpdated) && (!p_raw_data_tail->m_bDataConverted))
            return;
        if(!p_raw_data_head->m_bDataConverted)
        {
            p_raw_data_head->m_fCurrent = ChannelInfo_Manager::CalibrateCurrentADC(0,
                                          ChannelInfo_Manager::ChannelInfo[0].m_ucCurrentRange,
                                          p_raw_data_head->m_uI_ADC_Code);
            p_raw_data_head->m_fVoltage = ChannelInfo_Manager::CalibrateVoltageADC(0,
                                          ChannelInfo_Manager::ChannelInfo[0].m_ucVoltageRange,
                                          p_raw_data_head->m_uV_ADC_Code);
        }
        if(!p_raw_data_tail->m_bDataConverted)
        {
            p_raw_data_tail->m_fCurrent = ChannelInfo_Manager::CalibrateCurrentADC(0,
                                          ChannelInfo_Manager::ChannelInfo[0].m_ucCurrentRange,
                                          p_raw_data_tail->m_uI_ADC_Code);
            p_raw_data_tail->m_fVoltage = ChannelInfo_Manager::CalibrateVoltageADC(0,
                                          ChannelInfo_Manager::ChannelInfo[0].m_ucVoltageRange,
                                          p_raw_data_tail->m_uV_ADC_Code);
        }
    }

    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_uBuildInVoltCount; i++)
        {
            if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bBuildInVoltNewCode_Pulse[i])
            {
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_bBuildInVoltNewCode_Pulse[i] = 0;
                ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fBuildInVolt_Pulse[i] = ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_u32BuildInVolt_Pulse[i] * ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_BuildInV[i].m_Gain + ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_BuildInV[i].m_Offset;
            }
        }
    }

//     St_Pulse_PackInfo * st_PackInfo = (St_Pulse_PackInfo *) &EthernetParse::m_Point_Temp[0];
//     st_PackInfo->m_wChannelNumber = uc_ChannelNo;
//     st_PackInfo->m_btIfPartialPusle = 0;
//     st_PackInfo->m_dwPulseCycleID = Pulse::buf_info[BufIdx].m_CycleIdx;
//     st_PackInfo->m_PulseStartTime = Pulse::buf_info[BufIdx].m_CycleStartTime;
//     st_PackInfo->m_btFirstPointID = 0;
//     st_PackInfo->m_btIfHaveCapEng = 1;
//     st_PackInfo->m_btbuf_Idex = 0 / 1;

    ChannelInfo_Manager::DVmetaInit(uc_ChannelNo);
    EthernetParse::MPReportData(uc_ChannelNo,SEND_PULSE_LOG_DATA);
}
void Pulse_LogLimit_task::Pulse_LL_FindNextLimit(void)
{
    Step_Manager_task::LL_CurrentPtr.m_ChannelIdx++;
    if(Step_Manager_task::LL_CurrentPtr.m_ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
        Step_Manager_task::LL_CurrentPtr.m_ChannelIdx = 0;
    Step_Manager_task::LL_CurrentPtr.m_LimitIdx = 0;
    m_CheckedChannelCount_LL++;
}
