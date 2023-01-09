#include "../Entry/includes.h"

//public:
unsigned short               IVSDL::Priority = PRIORITY8;
unsigned short               IVSDL::TaskID = SDL_TASK;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_SDL_QUEUE                 IVSDL::Buffer;                                //SDL buffer
unsigned char                IVSDL::Valid;
//unsigned long                IVSDL::CurrentCounter[MAXCHANNELNO];
///unsigned long                IVSDL::VoltageCounter[MAXCHANNELNO];
St_Grp                       IVSDL::m_GrpInfo4SDL[MAXCHANNELNO];

St_FrequentlyLog	 		 IVSDL::SubMasterFrequentlyLog;                //记录并联时SubMaster的采样汇报时间


//private:
//unsigned char                IVSDL::m_bPairIV;
//unsigned char                IVSDL::ucLowSpeedFactor;
//unsigned char                IVSDL::ucLowSpeedFactor_New;
//unsigned char                IVSDL::bHaveLoggedOnePoint;                   //是否Log了一个通道；
//unsigned short               IVSDL::CurrentChannelIdx;                     //SDL采用分时机制，每次SDL周期只处理一个通道的一个POINT，所以存在SDL目前正在处理的通道号标记。
MsTime                       IVSDL::LastLogTime[MAXCHANNELNO];             //上一次采样的时间点；
//float                        IVSDL::LastLoggedCCapacity[MAXCHANNELNO];     //上次记录的 充电容量；
//float                        IVSDL::LastLoggedDCapacity[MAXCHANNELNO];     //上次记录的 放电容量；
//float                        IVSDL::LastLoggedCEnergy[MAXCHANNELNO];       //上次记录的 充电能量；
//float                        IVSDL::LastLoggedDEnergy[MAXCHANNELNO];       //上次记录的 放电能量；
//St_SDL_History_Point         IVSDL::LastLoggedCurrent[MAXCHANNELNO];       //上次记录的 电流；
//St_SDL_History_Point         IVSDL::LastLoggedVoltage[MAXCHANNELNO];       //上次记录的 电压；
//St_SDL_History_Point         IVSDL::PreviousCurrent[MAXCHANNELNO];         //上次参与计算的 电流；
//St_SDL_History_Point         IVSDL::PreviousVoltage[MAXCHANNELNO];         //上次参与计算的 电压；
//St_SDL_History_Point         IVSDL::FirstLoggedCurrent[MAXCHANNELNO];      //第一个因为偏移导致被LOG的记录点，用于修正dIdt；
//St_SDL_History_Point         IVSDL::FirstLoggedVoltage[MAXCHANNELNO];      //第一个因为偏移导致被LOG的记录点，用于修正dVdt；
St_DeltaValue                IVSDL::DeltaValue[MAXCHANNELNO];              //每个数据项的 Delta值，DCR没有Delta值
//St_SDL_HISTORY_QUEUE         IVSDL::HistoryCurrentData[MAXCHANNELNO];      //为每个通道建立一个电流历史数据队列
//St_SDL_HISTORY_QUEUE         IVSDL::HistoryVoltageData[MAXCHANNELNO];      //为每个通道建立一个电压历史数据队列
//St_SDL_History_Point         IVSDL::LastCurrentAdcPoint[MAXCHANNELNO];
//St_SDL_History_Point         IVSDL::LastVoltageAdcPoint[MAXCHANNELNO];
St_ChannelStatus             IVSDL::ChannelStatus[MAXCHANNELNO];           //记录每个通道的当前控制状态，是计算Delta值的源头

//unsigned char                IVSDL::uc_DischargeCFlag[MAXCHANNELNO];       // 放电/充电电容量间隔记录切换标志
//unsigned char                IVSDL::uc_DischargeEFlag[MAXCHANNELNO];       // 放电/充电电容量间隔记录切换标志
//unsigned char                IVSDL::uc_IR_SendCount[MAXCHANNELNO];         // 内阻IR数据补救性上传计数器
unsigned char                IVSDL::uc_ACR_SendCount[MAXCHANNELNO];         // 内阻ACR数据补救性上传计数器
//UWord32                      IVSDL::uw32_IntervalTCapacity[MAXCHANNELNO];   // 电容量间隔记录时间
//UWord32                      IVSDL::uw32_IntervalTEnergy[MAXCHANNELNO];     // 电容量间隔记录时间
UWord32                      IVSDL::uw32_Interval_4_ETable[MAXCHANNELNO];   // Internval for Extended table 间隔记录时间
ST_LOG_MVUD                  IVSDL::bLogMVUD[MAXCHANNELNO];
// the following is not used
//#if SUPPORT_AUX == 1
//St_AUXSDL_Info               IVSDL::m_AUXInfo[MAXCHANNELNO][MAXAUXTYPE];
//ST_AUX_SDL				     IVSDL::m_AuxSDLBank[MAXCHANNELNO][MAXAUXTYPE]; //SDL参数按辅助类型分，但IV通道之间可以有所不同static ST_AUX_DATA_REQUEST 		  IVSDL::m_AuxDataRequestBank[MAXCHANNELNO][3*MAX_AUXCH_PER_AUXMCU]; //用于登记那些辅助通道数据被请求了,*3 是因为有3种数据
//St_AUXSDL_LastValue		     IVSDL::m_LastAUXValue[MAXCHANNELNO][MAXAUXTYPE][MAX_AUXCH_PER_AUXMCU];
//#endif
ST_BuiltIn_DATA    			 IVSDL::st_BuiltIn_Aux_Data;
MsTime                  	 IVSDL::m_IntervalTime_SDL;
//float										 IVSDL::m_LastBuiltInValue[MAXCHANNELNO][MAX_AUXCH_PER_DATA_GROUP];
// unsigned long                IVSDL::m_dwStepPoint[MAXCHANNELNO];//mits8
MsTime 							IVSDL::ReportLog_time;
bool                        IVSDL::IsChnNeedFilter[MAXCHANNELNO];
#pragma arm section

void IVSDL::Init_WithoutBuffer(void)
{
    Valid = ChannelInfo_Manager::BoardCFG.m_Global.m_bUseSDL;
//    ucLowSpeedFactor = 1;
//    ucLowSpeedFactor_New = 1;
//    m_bPairIV = 0; //not use PairIV function in default mode
    Init_SubMasterFrequentlyLog();
    for (unsigned short ChannelIdx = 0; ChannelIdx < MAXCHANNELNO; ChannelIdx++)
    {
        St_VRangeCFG * pV_Param = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH];
        St_IRangeCFG * pI_Param = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[ChannelInfo_Manager::CurrRngPhysicalRange1];
        St_ChannelStatus * p_Status = &ChannelStatus[ChannelIdx];
        St_DeltaValue * pDelta = &DeltaValue[ChannelIdx];

        IsChnNeedFilter[ChannelIdx] = true;
//        uc_DischargeCFlag[ChannelIdx] = 0;       // 放电/充电电容量间隔记录切换标志
//        uc_DischargeEFlag[ChannelIdx] = 0;       // 放电/充电电容量间隔记录切换标志
//        uc_IR_SendCount[ChannelIdx] = 0;             // 内阻IR数据补救性上传计数器
        uc_ACR_SendCount[ChannelIdx] = 0;            // 内阻ACR数据补救性上传计数器
//        uw32_IntervalTCapacity[ChannelIdx] = GetSystemTimeInSecond();   // 电容量间隔记录时间
//        uw32_IntervalTEnergy[ChannelIdx] = GetSystemTimeInSecond();     // 电容量间隔记录时间
        uw32_Interval_4_ETable[ChannelIdx] = GetSystemTimeInSecond();     // E_Table delay time

//        INIT_QUEUE(HistoryCurrentData[ChannelIdx]);
//        INIT_QUEUE(HistoryVoltageData[ChannelIdx]);

//        PreviousCurrent[ChannelIdx].bLoged = 1;
//        PreviousVoltage[ChannelIdx].bLoged = 1;
//        p_Status->bNeedSDL = 0;
        p_Status->u16ControlType = CT_IVCH_STOP;

        p_Status->ExpCondition = eExpCondition_Battery;
        SetFactor_Time(ChannelIdx, ChannelInfo_Manager::BoardCFG.m_Global.m_fSdlTime * 100);
        SetFactor(ChannelIdx, 0,FACTOR_I);
        SetFactor(ChannelIdx, ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_V * 10,FACTOR_V);
        SetFactor(ChannelIdx, 0,FACTOR_Cap);
        SetFactor(ChannelIdx, 0,FACTOR_Energy);
        p_Status->ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
        p_Status->ucVoltageRange = VOLTAGE_RANGE_HIGH;
        p_Status->Vmax = GetFullValue(pV_Param->m_Bound.m_High, pV_Param->m_Bound.m_Low);
        p_Status->Imax = GetFullValue(pI_Param->m_Bound.m_High, pI_Param->m_Bound.m_Low);

//        float fVoltage, fCurrent;
//        fVoltage = ::abs(p_Status->fVoltageAccuracy * p_Status->Vmax);
//        fCurrent = ::abs(p_Status->fCurrentAccuracy * p_Status->Imax);
//        pDelta->u32Voltage = fVoltage / pV_Param->m_Adc.m_Gain;
//        pDelta->u32Current = fCurrent / pI_Param->m_Adc.m_Gain;
//        pDelta->fCapacity = p_Status->fBatteryCapacity * p_Status->fCapacityAccuracy;
//        pDelta->fEnergy = p_Status->fBatteryCapacity * p_Status->fBatteryVoltage * p_Status->fEnergyAccuracy;
//        pDelta->f_dIdt = 0;
//        pDelta->f_dVdt = 0;
//        pDelta->f_dIdt2 = 0;
//        pDelta->f_dVdt2 = 0;
        ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32CurrentRampDelta = MULTI_FACTOR * ACCURACY * p_Status->Imax / (::abs(pI_Param->m_Adc.m_Gain));
//         ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bNeedFilter = 1;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx, true);

        p_Status->fBatteryVoltage = p_Status->Vmax / 2;
        p_Status->fBatteryCapacity = p_Status->fBatteryVoltage * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::CurrRngPhysicalRange1);

//        if(pDelta->u32Voltage <= 10)      //Pretect if Config is not right,that will cause a continuous logging of voltage.
//            pDelta->u32Voltage = 4194;
//        m_dwStepPoint[ChannelIdx] = 1; //mits8
        bLogMVUD[ChannelIdx].LogMVUD_MetaCode = 0;
        bLogMVUD[ChannelIdx].bLogAllMVUD = false;
    }
}
void IVSDL::Init(void)
{
    INIT_QUEUE(Buffer);

    Init_WithoutBuffer();
}
// void IVSDL::DoWork(void)
// {
// #if DEBUG_TASK_STATISTIC == 1
// 	OS_ServeProcess::Task_Count[COUNT_SDL] ++;
// #endif

//     //if(Buffer.m_u32Length > 0)
// 	if((Buffer.m_u32Length >= MAX_PACK_LOGPOINT)||(CommonFunc::GetTimeTicks(&m_IntervalTime_SDL)>5000))
//     {
//         CommonFunc::GetTimeTicksUpdate( &m_IntervalTime_SDL);
// 		EthernetParse::MPReportData(0,SEND_LOG_SDL);    //调用第二信道发送数据到上位机
//     }

// 	for(int ChanIdx = 0;ChanIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChanIdx++)
// 	{
// 		CurrentChannelIdx = ChanIdx;
// 		if(m_GrpInfo4SDL[ChanIdx].State == eGroupState_Normal)
// 		{
// 			if(!Valid)   //普通通道如果是ACCESS数据格式，不需要进行SDL检查
// 				continue;
// 			Check_Normal(ChanIdx);
// 		}
// 		else if(m_GrpInfo4SDL[ChanIdx].State == eGroupState_Master)
// 			Check_Normal(ChanIdx);
// 		else if(m_GrpInfo4SDL[ChanIdx].State == eGroupState_Submaster)
// 			Check_Submaster(ChanIdx);
// 		//slave不用SDL
// 	}
// }
/*void IVSDL::Check_Normal(unsigned short ChannelIdx)
{
    bool bHaveLoggedOnePoint = false;
    bool bOK = false;
    St_SDL_History_Point HistoryPoint;
    if(ChannelStatus[ChannelIdx].bNeedSDL == false)
        return;

//     Check_Voltage(bOK, HistoryPoint);//180827 zyx
//     Check_Current(bOK, HistoryPoint);

//     CheckCapEner(ChannelIdx);


    if(!bHaveLoggedOnePoint)
    {
        unsigned long long TimeDiff = GetTimeTicks(LastLogTime[ChannelIdx].Second, LastLogTime[ChannelIdx].Us100, OS_ServeProcess::OS_Time.Second, OS_ServeProcess::OS_Time.Us100);

        if(TimeDiff >= DeltaValue[ChannelIdx].u64Time)
        {
            St_SDL_Point MyPoint;

            MyPoint.Time = OS_ServeProcess::OS_Time;
            MyPoint.fValue[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Current);
            MyPoint.fValue[1] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Voltage);
            MyPoint.fValue[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
            MyPoint.fValue[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
            MyPoint.fValue[4] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy);
            MyPoint.fValue[5] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy);
            MyPoint.ucDataCount = 6;
            MyPoint.u16ChannelIdx = ChannelIdx;
            MyPoint.bCriticalPoint = 1;
            MyPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;

            MyPoint.ucType = SDL_DATA_FOR_LOG;

            MyPoint.u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
            MyPoint.dTestTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx, MetaCode_PV_TestTime);//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_TestTime);
            MyPoint.dStepTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx, MetaCode_PV_Steptime);//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Steptime);
            MyPoint.u16Cycle_Idx = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_CycleIndex);
            MyPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;
            MyPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[ChannelIdx];//0; //1804201 zyx
            ChannelInfo_Manager::ClearDataFlag(ChannelIdx, 0xFFFFFFFF );
            ENQUEUE(Buffer, SDL_QUEUE_DEPTH, MyPoint);

//			if(m_bPairIV)
//			{
//				MyPoint.fValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_Current);
//				ENQUEUE(Buffer,SDL_QUEUE_DEPTH,MyPoint);
//			}
            if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
                IVSDL::Check_BuiltInAuxForLog(ChannelIdx, SDL_DATA_FOR_LOG);

            LastLogTime[ChannelIdx] = MyPoint.Time;
        }
    }
    else
    {
        if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
        {
            unsigned long long TimeDiff = GetTimeTicks(Schedule_Manager_task::SDLIdleMsTime[ChannelIdx].Second, Schedule_Manager_task::SDLIdleMsTime[ChannelIdx].Us100, OS_ServeProcess::OS_Time.Second, OS_ServeProcess::OS_Time.Us100);
            if(TimeDiff >= DeltaValue[ChannelIdx].u64Time)
            {
                IVSDL::Check_BuiltInAuxForLog(ChannelIdx, SDL_DATA_FOR_LOG);
                Schedule_Manager_task::SDLIdleMsTime[ChannelIdx] = OS_ServeProcess::OS_Time;
            }
        }
    }
    //AdjustSdlSpeed();

    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        if(!Schedule_Manager_task::IfSchRunning(ChannelIdx))
            return ;

        unsigned char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_scBufIdx;
        unsigned char uc_ChanCnt = (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount);
        float f_GetValue;
        for(int i = 0; i < uc_ChanCnt; i++)
        {
            f_GetValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_BuiltIn_Data1 + i);
            if(::fabsf(IVSDL::m_LastBuiltInValue[ChannelIdx][i] - f_GetValue) >=::fabsf(ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_SchAux_Glb_Temperature.m_fDelta))
            {
                IVSDL::Check_BuiltInAuxForLog(ChannelIdx, SDL_DATA_FOR_LOG);
                IVSDL::m_LastBuiltInValue[ChannelIdx][i] = f_GetValue;
            }
        }
    }
}
*/

void IVSDL::Check_Submaster_Report(unsigned short ChannelIdx)
{
    unsigned long long TimeDiff = CommonFunc::GetTimeTicks(&LastLogTime[ChannelIdx]);//GetTimeTicks(LastLogTime[ChannelIdx].Second, LastLogTime[ChannelIdx].Us100, OS_ServeProcess::OS_Time.Second, OS_ServeProcess::OS_Time.Us100);
    if(TimeDiff >= DeltaValue[ChannelIdx].u64Time)
    {
        SubMasterSdlLogTimesFrequently2Normal(ChannelIdx);
        CANBusParse::Parallel_SubmasterSend_ReportData(ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Current),
                ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Voltage));
        LastLogTime[ChannelIdx] = CommonFunc::GetSystemTime();//OS_Time; //10.07 debugging add, update time
    }
}
// unsigned char IVSDL::NeedSDL(unsigned short ChannelIdx)
// {
// 	return ChannelStatus[ChannelIdx].bNeedSDL;
// }

void IVSDL::ScheduleStartedRoutine(unsigned short ChannelIdx)
{
//     signed char SchBufIdx;
    float fCapacity, fVoltage;

    fCapacity = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_MV_NorminalCapacity);
    fVoltage  = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_MV_NorminalVoltage);

//     SchBufIdx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_scBufIdx;
//     if(SchBufIdx != -1)
//     {
// // 		SetFactor_I(ChannelIdx, ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlFactor_I);
// // 		SetFactor_V(ChannelIdx, ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlFactor_V);
// // 		SetFactor_Capacity(ChannelIdx, ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlFactor_Capacity);
// // 		SetFactor_Energy(ChannelIdx, ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlFactor_Energy);
// // 		SetFactor_Time(ChannelIdx,ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_Setting.m_fSdlTime);
//     }
    SetCapacity(ChannelIdx, fCapacity);
    SetNVoltage(ChannelIdx, fVoltage);

//    INIT_QUEUE(HistoryCurrentData[ChannelIdx]);
//    INIT_QUEUE(HistoryVoltageData[ChannelIdx]);

    ChannelInfo_Manager::Init_HistoryData(ChannelIdx);
    ChannelStatus[ChannelIdx].bIdle = 0;
}
void IVSDL::ScheduleEndedRoutine(unsigned short ChannelIdx)
{
    ChannelStatus[ChannelIdx].Vmax = GetFullValue(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High,
                                     ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_Low);
    ChannelStatus[ChannelIdx].Imax = GetFullValue(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[ChannelInfo_Manager::CurrRngPhysicalRange1].m_Bound.m_High,
                                     ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[ChannelInfo_Manager::CurrRngPhysicalRange1].m_Bound.m_Low);

    //闲时数据间隔
    SetFactor(ChannelIdx, ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_V * 10,FACTOR_V);
    SetFactor_Time(ChannelIdx, ChannelInfo_Manager::BoardCFG.m_Global.m_fSdlTime * 100);

//    DeltaValue[ChannelIdx].f_dVdt2 = 0;
    float fVoltage;
    fVoltage = ::abs(ChannelStatus[ChannelIdx].Vmax * ChannelStatus[ChannelIdx].fVoltageAccuracy);
//    DeltaValue[ChannelIdx].u32Voltage = fVoltage / ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Adc.m_Gain;

//    ChannelStatus[ChannelIdx].bNeedSDL = 0;
    ChannelStatus[ChannelIdx].bIdle    = 1;
}

void IVSDL::SubmasterStartRoutine(unsigned short ChannelIdx)
{
    unsigned char	ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    if(p_Ctrl->m_u16ControlType == CT_IVCH_CURRENT_TB || p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_TB) //2018.09.06 DBPT测试并联时按照100us更新
    {
        SetFactor_Time(ChannelIdx,IV_SUB_SDL_TIME_FREQUENTLY_FOR_DBPT);
    }
    else
    {
        SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_FREQUENTLY);
    }
    SubMasterFrequentlyLog.bNeedFrequentlyLog = true;
//    INIT_QUEUE(HistoryCurrentData[ChannelIdx]);
//    INIT_QUEUE(HistoryVoltageData[ChannelIdx]);
    ChannelInfo_Manager::Init_HistoryData(ChannelIdx);
}
void IVSDL::SubmasterStopRoutine(unsigned short ChannelIdx)
{
    SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_IDLE);

//    INIT_QUEUE(HistoryCurrentData[ChannelIdx]);
//    INIT_QUEUE(HistoryVoltageData[ChannelIdx]);
    ChannelInfo_Manager::Init_HistoryData(ChannelIdx);
}

void IVSDL::StepStartedRoutine(unsigned short ChannelIdx, bool IsClearHistoryData)
{
    unsigned short ControlType;
    unsigned char IRange, VRange; //,ActiveIdx;

//    CurrentCounter[ChannelIdx] = 0;
//    VoltageCounter[ChannelIdx] = 0;
//    INIT_QUEUE(HistoryCurrentData[ChannelIdx]);
//    INIT_QUEUE(HistoryVoltageData[ChannelIdx]);

    if(IsClearHistoryData)
        ChannelInfo_Manager::Init_HistoryData(ChannelIdx);

    Sample_Pro::NewCodeSampled[ChannelIdx] = 0;    //step 开始了，ADC开始采样，因为IBT的通道号错乱，可能导致问题
    Sample_Pro::SampleOverTime[ChannelIdx] = CommonFunc::GetSystemTime();

    St_StepData * StepData = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data;
    St_ChannelStatus * p_ChnStatus = &ChannelStatus[ChannelIdx];

    ControlType = StepData->m_Ctrl.m_u16ControlType;
    IRange      = StepData->m_Ctrl.m_ucCurrentRange;
    VRange      = StepData->m_Ctrl.m_ucVoltageRange;

//     if((StepData->m_Ctrl.m_ucLogLimitNo == 0) && Valid)  //20130831 hepeiqing add,when tested in Access mode //180818 zyx
//         p_ChnStatus->bNeedSDL = 1;
//     else
//         p_ChnStatus->bNeedSDL = 0;

    p_ChnStatus->u16ControlType   = ControlType;
    p_ChnStatus->ucCurrentRange   = IRange;
    p_ChnStatus->ucVoltageRange   = VRange;
    p_ChnStatus->dXdt             = StepData->m_Ctrl.m_CtrlValues.m_fControlValue[1];
//    p_ChnStatus->bFirstAdcLogged_I = 0;
//    p_ChnStatus->bFirstAdcLogged_V = 0;


    St_ChannelCFG * p_Chn_CFG = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx];

    p_ChnStatus->Vmax = GetFullValue(p_Chn_CFG->m_VRange[VRange].m_Bound.m_High,
                                     p_Chn_CFG->m_VRange[VRange].m_Bound.m_Low);
    p_ChnStatus->Imax = GetFullValue(p_Chn_CFG->m_IRange[IRange].m_Bound.m_High,
                                     p_Chn_CFG->m_IRange[IRange].m_Bound.m_Low);
    p_ChnStatus->Vmax = ::abs(p_ChnStatus->Vmax);
    p_ChnStatus->Imax = ::abs(p_ChnStatus->Imax);
    if(m_GrpInfo4SDL[ChannelIdx].bMultiHigh)
        p_ChnStatus->Imax *= (m_GrpInfo4SDL[ChannelIdx].SlaveCount + 1);
    SDL_SetParameters(ChannelIdx);

    LastLogTime[ChannelIdx] = CommonFunc::GetSystemTime();//OS_Time;
    Schedule_Manager_task::SDLIdleMsTime[ChannelIdx] = CommonFunc::GetSystemTime();//OS_Time;
//    PreviousVoltage[ChannelIdx].bLoged = 1;
//    PreviousCurrent[ChannelIdx].bLoged = 1;
//    LastLoggedCCapacity[ChannelIdx] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
//    LastLoggedDCapacity[ChannelIdx] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
//    LastLoggedCEnergy[ChannelIdx] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy);
//    LastLoggedDEnergy[ChannelIdx] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy);

    //uw32_IntervalTCapacity[ChannelIdx] = OS_ServeProcess::OS_Time.Second - 10;  //151202,add by hpq,for capacity log bug
    IVSDL::uw32_Interval_4_ETable[ChannelIdx] = GetSystemTimeInSecond() - 10;  //  2013/10/23  Yangy 解决无记录点问题。
}
void IVSDL::LimitArrivedRoutine(unsigned short ChannelIdx)
{
//     unsigned char bOK = 0, bNotEmpty;
//     St_SDL_History_Point HistoryPoint;
//    CurrentChannelIdx = ChannelIdx;

//#if SUPPORT_AUX == 1
    //if(CommonFunc::uc_Support_AUX)
    //CANBusParse::Send_ClearLimitsToAUX(ChannelIdx,0);    // 20140110 何培青 解决手工跳步的一些BUG
//#endif

//     if(ChannelStatus[ChannelIdx].bNeedSDL) //180818 zyx
//     {
//         for(int i = 0 ; i < 200; i++)
//         {
//             bNotEmpty = Check_Voltage(bOK, HistoryPoint);
//             if(!bNotEmpty)
//                 break;
//         }
//         for(int i = 0 ; i < 200; i++)
//         {
//             bNotEmpty = Check_Current(bOK, HistoryPoint);
//             if(!bNotEmpty)
//                 break;
//         }
//     }
    //TriggerLog(ChannelIdx,SDL_DATA_TYPE_LOG,1);

    //ChannelInfo_Manager::Init_HistoryData(ChannelIdx);

    ChannelStatus[ChannelIdx].u16ControlType = CT_IVCH_STOP;
//     ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bNeedFilter = 1;
//    ChannelStatus[ChannelIdx].bNeedSDL = 0;
}

void IVSDL::SetLogMVUDFlag(unsigned short ChannelIdx, unsigned char MVUD_MetaCode)
{
    if((MVUD_MetaCode >= MetaCode_MV_UD1 && MVUD_MetaCode <= MetaCode_MV_UD4)\
            || (MVUD_MetaCode >= MetaCode_MV_UD5 && MVUD_MetaCode <= MetaCode_MV_UD16))
        bLogMVUD[ChannelIdx].LogMVUD_MetaCode = MVUD_MetaCode;
    else if(MVUD_MetaCode == LOG_ALL_MVUD)
    {
        bLogMVUD[ChannelIdx].bLogAllMVUD = true;
    }
}
void IVSDL::ClearLogMVUDFlag(unsigned short ChannelIdx)
{
    bLogMVUD[ChannelIdx].LogMVUD_MetaCode = 0;
    bLogMVUD[ChannelIdx].bLogAllMVUD = false;
}
unsigned char IVSDL::IsLogAllMVUD(unsigned short ChannelIdx)
{
    return bLogMVUD[ChannelIdx].bLogAllMVUD;
}
unsigned char IVSDL::IsLogMVUD(unsigned short ChannelIdx)
{
    return bLogMVUD[ChannelIdx].LogMVUD_MetaCode;
}
void IVSDL::LogDataHead(unsigned short ChannelIdx, unsigned char uc_LogType,unsigned char uc_bCriticalPoint, St_SDL_Point_4BytesCycleIdx * PointData)
{
    memset(PointData, 0, sizeof(St_SDL_Point_4BytesCycleIdx));
    PointData->Time = CommonFunc::GetSystemTime();
    PointData->u16ChannelIdx = ChannelIdx;
    PointData->ucType = uc_LogType;
    PointData->bCriticalPoint = uc_bCriticalPoint;   // 是否为关键点，（在考虑压缩的时候不能删除的数据点）
    PointData->u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
    PointData->dTestTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx,MetaCode_PV_TestTime,PointData->Time);
    PointData->dStepTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx,MetaCode_PV_Steptime,PointData->Time);
    if(ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue > 0)
        PointData->u32Cycle_Idx = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
    PointData->u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;
    PointData->u32DataPoint =  ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32;
}
void IVSDL::IVLogAllMVUD(unsigned short ChannelIdx, unsigned char uc_LogType,unsigned char uc_bCriticalPoint, St_SDL_Point_4BytesCycleIdx *MyPoint)
{
    St_SDL_Point_4BytesCycleIdx *MVUD_Data = MyPoint;
    IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, uc_bCriticalPoint, MetaCode_MV_UD1, MetaCode_MV_UD2, MetaCode_MV_UD3, MVUD_Data);
    IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, uc_bCriticalPoint, MetaCode_MV_UD4, 0, 0, MVUD_Data);
    for(unsigned char MV_UD = MetaCode_MV_UD5; MV_UD < MetaCode_MV_UD16; MV_UD+=3)
        IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, uc_bCriticalPoint, MV_UD, MV_UD + 1, MV_UD + 2, MVUD_Data);
}

void IVSDL::IVLogBaseInfo(unsigned short ChannelIdx, UWord16 u16ControlType, St_SDL_Point_4BytesCycleIdx *MyPoint)
{
    MyPoint->ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;
    MyPoint->un_data.fValue[0] = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Current].m_fValue;
    MyPoint->un_data.fValue[1] = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
    MyPoint->un_data.fValue[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
    MyPoint->un_data.fValue[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
    MyPoint->un_data.fValue[4] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy); // uw32_ChargeEnergy++;//
    MyPoint->un_data.fValue[5] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy); // uw32_ChargeEnergy;//
    MyPoint->ucDataCount = 6;
    PushPoint(*MyPoint);

}

void IVSDL::IVLogTCCountForAccess(unsigned short ChannelIdx)
{
    IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_TCCOUNT_ACCESS, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, MetaCode_TC_Counter1, MetaCode_TC_Counter2, MetaCode_TC_Counter3, NULL);
    IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_TCCOUNT_ACCESS, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, MetaCode_TC_Counter4, 0, 0, NULL);
//     IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_TCCOUNT_ACCESS, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, MetaCode_TC_Counter4, MetaCode_TC_Counter5, MetaCode_TC_Counter6, NULL);
//     IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_TCCOUNT_ACCESS, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, MetaCode_TC_Counter7, MetaCode_TC_Counter8, 0, NULL);

}

void IVSDL::IVLogMetaVar(unsigned short ChannelIdx, unsigned char DataLogFormat, unsigned char uc_LogType, unsigned char uc_bCriticalPoint, \
                         BYTE MetaCode1, BYTE MetaCode2, BYTE MetaCode3, St_SDL_Point_4BytesCycleIdx *MyPoint)
{
    St_SDL_Point_4BytesCycleIdx * PointData = NULL;
    if(!MetaCode1 && !MetaCode2 && !MetaCode3)
        return;
    if(MyPoint == NULL)
    {
        PointData = (St_SDL_Point_4BytesCycleIdx *)malloc(sizeof(St_SDL_Point_4BytesCycleIdx));
        if(PointData == NULL)
            return;
        LogDataHead(ChannelIdx, uc_LogType, uc_bCriticalPoint, PointData);
    }
    else
    {
        PointData = MyPoint;
        memset(PointData->un_data.fValue, 0, sizeof(PointData->un_data.fValue));
    }
    int point = 0;
    PointData->ucDataLogFormat = DataLogFormat;//SDL_DATA_TYPE_LOG_3_META_VAR;
    if(MetaCode1 > 0)
    {
        PointData->un_data.dat[point].ucType = MetaCode1;
        PointData->un_data.dat[point++].fValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode1);
    }
    if(MetaCode2 > 0)
    {
        PointData->un_data.dat[point].ucType = MetaCode2;
        PointData->un_data.dat[point++].fValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode2);
    }
    if(MetaCode3 > 0)
    {
        PointData->un_data.dat[point].ucType = MetaCode3;
        PointData->un_data.dat[point++].fValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode3);
    }
    PointData->ucDataCount = point*2;
    PushPoint(*PointData);
    if(MyPoint == NULL)
    {
        free(PointData);
    }
}
void IVSDL::TriggerLog(unsigned short ChannelIdx, unsigned char uc_LogType, unsigned char uc_bCriticalPoint)
{
    St_SDL_Point_4BytesCycleIdx MyPoint;
    memset(&MyPoint, 0, sizeof(St_SDL_Point_4BytesCycleIdx));
    LogDataHead(ChannelIdx, uc_LogType, uc_bCriticalPoint, &MyPoint);

    UWord16 uw16_IntervalsTimeC = INTERVALTIME_T1S;//INTERVALTIME_T1S;   //Per Customer's request  DKQ 04.11.2014
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    UWord16 u16ControlType = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;

    //161019
    if(uc_LogType == SDL_DATA_FOR_DISPLAY)
    {
        MyPoint.ucType = SDL_DATA_FOR_DISPLAY;
        MyPoint.u32DataPoint = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32;

        //Ext B
        IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_NORMAL_DATA_POINT, MetaCode_PV_dVdt, MetaCode_PV_dVdQ, MetaCode_PV_dQdV, &MyPoint);

        if(u16ControlType != CT_IVCH_IR)
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_NORMAL_DATA_POINT, MetaCode_PV_InternalResistance, MetaCode_PV_ACR, 0, &MyPoint);
        else
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_NORMAL_DATA_POINT, MetaCode_PV_ACR,0,0, &MyPoint);
    
        //IV_Basic        
        IVSDL::IVLogBaseInfo(ChannelIdx, u16ControlType, &MyPoint);

        
        
        return ;  // Not to increase log data point
    }
    //161019


    if(MyPoint.ucType == SDL_DATA_FOR_LOG)
    {
        if(ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount == 1)
        {
            if(BurstModeSampling::b_Logging[ChannelIdx])
                return;
        }

        if((ChannelInfo_Manager::m_DataFlag[ChannelIdx] & STOP_POINT) == 0)
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32 ++;  // increae log data point

        ChannelInfo_Manager::GetSpecialPointStatus(ChannelIdx);
        MyPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[ChannelIdx];//1804251 zyx //ul_SpecialPoint;
        ChannelInfo_Manager::ClearDataFlag(ChannelIdx);

        MyPoint.ucType = SDL_DATA_FOR_LOG;
        IVSDL::ReportLog_time = MyPoint.Time;

        MyPoint.u32DataPoint =  ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32; //180516 zyx//IVSDL::m_dwDataPoint[ChannelIdx];

        if(uc_bCriticalPoint == LOG_CRITICAL_DATA_POINT)  // 2013/10/24 yang  修改关键点记录数据
        {   // 关键点记录处理，必须全部传送。
            // IV_Extended

#if DEBUG_TEST_MODE == 1
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug1].m_fValue = ChannelInfo_Manager::f_IR_Sourceback[ChannelIdx];
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug2].m_fValue = ChannelInfo_Manager::f_IR_Valueback[ChannelIdx];
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug3].m_fValue = ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_fIR_RisingEdge;
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode_Debug1, MetaCode_Debug2, MetaCode_Debug3, &MyPoint);

            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug4].m_fValue = 0;//CurrError;
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug5].m_fValue = 0;//VoltError;
            ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug6].m_fValue = ChannelInfo_Manager::m_CurFineTune_Info[ ChannelIdx ].m_TotalAdjust;
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode_Debug4, MetaCode_Debug5, MetaCode_Debug6, &MyPoint);
#endif
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode_PV_dVdt, MetaCode_PV_dVdQ, MetaCode_PV_dQdV, &MyPoint);

            unsigned char MetaCode1 = 0, MetaCode2 = 0;
//             if((u16ControlType == CT_IVCH_IR) || (uc_IR_SendCount[ChannelIdx] == 1))
//             {
            MetaCode1 = MetaCode_PV_InternalResistance;
//             }

            if((u16ControlType == CT_IVCH_ACIR) || (uc_ACR_SendCount[ChannelIdx] == 1))
            {
                MetaCode2 = MetaCode_PV_ACR;
            }
            IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode1, MetaCode2, 0, &MyPoint);
            uw32_Interval_4_ETable[ChannelIdx] = GetSystemTimeInSecond();//MainTime_Process::OS_Time.Second; // delay for E_Table
            if(IsLogAllMVUD(ChannelIdx))//190711 zyx
                IVSDL::IVLogAllMVUD(ChannelIdx, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, &MyPoint);
            if(IsLogMVUD(ChannelIdx) != 0)
                IVSDL::IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, IsLogMVUD(ChannelIdx), 0, 0, &MyPoint);
            if(IsLogAllMVUD(ChannelIdx) || (IsLogMVUD(ChannelIdx) != 0))
                ClearLogMVUDFlag(ChannelIdx);
        }
        else
        {   // 非关键点记录处理，可进行后继压缩传送。

            if(Buffer.m_u32Length >= (SDL_QUEUE_DEPTH * 0.8)) //  当数据缓冲区比较紧张已达到80%~100%时，30秒的判断时间变为50秒作为间隔判断时间。
            {
                uw16_IntervalsTimeC = INTERVALTIME_T30S;
                //            uw16_IntervalsTimeE = INTERVALTIME_T50S;
            }
            else if (Buffer.m_u32Length >= (SDL_QUEUE_DEPTH * 0.5)) //1805141
            {
                uw16_IntervalsTimeC = INTERVALTIME_T5S;
            }

            //   Word32  W32_tempTimeE = OS_ServeProcess::OS_Time.Second - uw32_IntervalTEnergy[ChannelIdx];
            Word32  W32_tempTimeE = CommonFunc::TimeDiffInSecond( uw32_Interval_4_ETable[ChannelIdx]);

            if(W32_tempTimeE < 0)      //  20131015 yy  解决数据通信量过多问题
            {
                uw32_Interval_4_ETable[ChannelIdx] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;
                //uw32_IntervalTCapacity[ChannelIdx] = OS_ServeProcess::OS_Time.Second;
                // uw32_IntervalTEnergy[ChannelIdx] = OS_ServeProcess::OS_Time.Second;
                W32_tempTimeE = 0; // 保护反转时间问题
            }

//        uc_CEValueType[uc_LoopIndex++] = MetaCode_PV_ChargeCapacity;
//        uc_CEValueType[uc_LoopIndex++] = MetaCode_PV_DischargeCapacity;
//        uc_LoopCount = 2;

//        Word32  W32_tempTimeC = OS_ServeProcess::OS_Time.Second - uw32_IntervalTCapacity[ChannelIdx];
//             Word32  W32_tempTimeC = OS_ServeProcess::OS_Time.Second - uw32_Interval_4_ETable[ChannelIdx];
            Word32  W32_tempTimeC = CommonFunc::TimeDiffInSecond( uw32_Interval_4_ETable[ChannelIdx]);

            if(W32_tempTimeC >= uw16_IntervalsTimeC)   // 如果大于等于5秒，record E_Table。
            {
                unsigned char MetaCode1 = 0, MetaCode2 = 0, MetaCode3 = 0;
                if(MetaVariable_Task::dVdtInfo[ChannelIdx].bRefresh == true)
                {
                    MetaCode1 = MetaCode_PV_dVdt;
                    MetaVariable_Task::dVdtInfo[ChannelIdx].bRefresh = false;
                }
                if(MetaVariable_Task::dVdQ[ChannelIdx].IsRefresh == true)
                {
                    MetaCode2 = MetaCode_PV_dVdQ;
                    MetaVariable_Task::dVdQ[ChannelIdx].IsRefresh = false;
                }
                if(MetaVariable_Task::dQdV[ChannelIdx].IsRefresh == true)
                {
                    MetaCode3 = MetaCode_PV_dQdV;
                    MetaVariable_Task::dQdV[ChannelIdx].IsRefresh = false;
                }
                IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_NORMAL_DATA_POINT, MetaCode1, MetaCode2, MetaCode3, &MyPoint);

                uw32_Interval_4_ETable[ChannelIdx] = GetSystemTimeInSecond();//OS_ServeProcess::OS_Time.Second;

                if((u16ControlType == CT_IVCH_IR) || (u16ControlType == CT_IVCH_ACR))
                {
                    IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_NORMAL_DATA_POINT, MetaCode_PV_InternalResistance, MetaCode_PV_ACR, 0, &MyPoint);
                    // 仅当为IR功能时，才向上位机传送IR内阻值。

//                    uc_IR_SendCount[ChannelIdx] = 1;

                    // 仅当为ACR功能时，才向上位机传送ACR内阻值。

                    uc_ACR_SendCount[ChannelIdx] = 1;
                }
                if(IsLogAllMVUD(ChannelIdx))//190711 zyx
                    IVSDL::IVLogAllMVUD(ChannelIdx, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, &MyPoint);
                if(IsLogMVUD(ChannelIdx) != 0)
                    IVSDL::IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, SDL_DATA_FOR_LOG, LOG_CRITICAL_DATA_POINT, IsLogMVUD(ChannelIdx), 0, 0, &MyPoint);
                if(IsLogAllMVUD(ChannelIdx) || (IsLogMVUD(ChannelIdx) != 0))
                    ClearLogMVUDFlag(ChannelIdx);

#if DEBUG_TEST_MODE == 1
                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug1].m_fValue = ChannelInfo_Manager::f_IR_Sourceback[ChannelIdx];
                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug2].m_fValue = ChannelInfo_Manager::f_IR_Valueback[ChannelIdx];
                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug3].m_fValue = ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_fIR_RisingEdge;
                IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode_Debug1, MetaCode_Debug2, MetaCode_Debug3, &MyPoint);

                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug4].m_fValue = 0;//CurrError;
                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug5].m_fValue = 0;//VoltError;
                ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_Debug6].m_fValue = ChannelInfo_Manager::m_CurFineTune_Info[ ChannelIdx ].m_TotalAdjust;
                IVLogMetaVar(ChannelIdx, SDL_DATA_TYPE_LOG_3_META_VAR, uc_LogType, LOG_CRITICAL_DATA_POINT, MetaCode_Debug4, MetaCode_Debug5, MetaCode_Debug6, &MyPoint);
#endif
            }
        }
        IVSDL::IVLogBaseInfo(ChannelIdx, u16ControlType, &MyPoint);

    }
}

void IVSDL::GetPoint(unsigned short ChannelIdx, unsigned char DataFor, unsigned char ValueType, unsigned long DataPoint, unsigned long DataFlag, St_Data_Pairs * MyPoint)
{
#if USE_AS_AUXMCU == 0
    MsTime OStime;
    CommonFunc::GetTimeTicksUpdate(&OStime);
    MyPoint->u32Second = OStime.Second;
    MyPoint->u16Us100 = OStime.Us100;

    MyPoint->bIV = 1;
    MyPoint->ucSensor = 0;
    MyPoint->u32DataPoint = DataPoint;
    MyPoint->u32DataBitFlag = DataFlag;
    MyPoint->ucType = DataFor;//SDL_DATA_FOR_LOG;
    MyPoint->u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
    MyPoint->dTestTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx, MetaCode_PV_TestTime,OStime); //Read_SpecifiedMetavariable2
    MyPoint->dStepTime = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx, MetaCode_PV_Steptime,OStime); //Read_SpecifiedMetavariable2
    MyPoint->u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
    MyPoint->u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;
    MyPoint->ucChannelIdx = ChannelIdx;


    switch(ValueType)
    {
    case SDL_DATA_TYPE_LOG_C_V_CAP_ENG:
        MyPoint->un_data.fValue[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Current);
        MyPoint->un_data.fValue[1] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Voltage);
        MyPoint->un_data.fValue[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
        MyPoint->un_data.fValue[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
        MyPoint->un_data.fValue[4] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy);
        MyPoint->un_data.fValue[5] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy);

#if DEBUG_TEST_IR_MODE == 1
        MyPoint->un_data.fValue[2] = ChannelInfo_Manager::f_IR_Sourceback[ChannelIdx];
        MyPoint->un_data.fValue[3] = ChannelInfo_Manager::f_IR_Valueback[ChannelIdx];
        MyPoint->un_data.fValue[4] = ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_fIR_RisingEdge;
        MyPoint->un_data.fValue[5] = ChannelInfo_Manager::m_IR_Info[ChannelIdx].m_fIR_by_CCCV;
#endif

        MyPoint->ucDataCount = 6;
        MyPoint->ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;
        break;
    default :
        break;
    }

#endif

}

//Delta 参数算法
void IVSDL::SDL_SetParameters(unsigned short ChannelIdx)
{   //这里还有一个事情没做，就是ChannelStatus里面的dVdt单位是V(A)/S,为了后面计算省时间，这里还需要将其转换为 Bit/100ns；
    unsigned char IRange, VRange;
//    float fVoltage, fCurrent;

    IRange = ChannelStatus[ChannelIdx].ucCurrentRange;
    VRange = ChannelStatus[ChannelIdx].ucVoltageRange;

    St_VRangeCFG * pV_Param = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VRange];
    St_IRangeCFG * pI_Param = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[IRange];
    St_ChannelStatus * p_Status = &ChannelStatus[ChannelIdx];
    St_DeltaValue * pDelta = &DeltaValue[ChannelIdx];
    St_ChannelInfo * p_Info = &ChannelInfo_Manager::ChannelInfo[ChannelIdx];

//    pDelta->f_dIdt    = 0.0f;
//    pDelta->f_dVdt    = 0.0f;
//    fVoltage					= ::abs(p_Status->fVoltageAccuracy * p_Status->Vmax);
//    fCurrent					= ::abs(p_Status->fCurrentAccuracy * p_Status->Imax);
//    pDelta->fCapacity = ::abs(p_Status->fBatteryCapacity * p_Status->fCapacityAccuracy);
//    pDelta->fEnergy   = ::abs(p_Status->fBatteryCapacity * p_Status->fBatteryVoltage * p_Status->fEnergyAccuracy);
//     p_Info->m_bNeedFilter = 1;
    ChannelInfo_Manager::SetFilterStatus(ChannelIdx,true);

//     if(p_Status->ExpCondition == eExpCondition_Resistance)
//     {
//         switch(p_Status->u16ControlType)
//         {
//         case CT_IVCH_CRAMP:
//             pDelta->f_dIdt = p_Status->dXdt;
//             pDelta->f_dVdt = p_Status->fR * pDelta->f_dIdt;
// //             p_Info->m_bNeedFilter = 0;
//             ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
//             break;
//         case CT_IVCH_VRAMP:
//             pDelta->f_dVdt = ChannelStatus[ChannelIdx].dXdt;
//             pDelta->f_dIdt = pDelta->f_dVdt / p_Status->fR;
// //             p_Info->m_bNeedFilter = 0;
//             ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
//             break;
//         default:
//             break;
//         }
//     }
//     else
//     {
    switch(p_Status->u16ControlType)
    {
    case CT_IVCH_SIMU_I:   //12.22.2013ADD     //simulation是否应该启用SDL，这是个问题
    case CT_IVCH_SIMU_P:   //12.22.2013ADD
    case CT_IVCH_SIMU_L:   //12.22.2013ADD
//        if(p_Info->m_SimuInfo.m_bExtraLog)
//            p_Status->bNeedSDL = 0;
//             p_Info->m_bNeedFilter = 0;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_C_CV:
    case CT_IVCH_V_CV:
    case CT_IVCH_SIMU_Battery: //181008 zyx
    case CT_IVCH_POWER:
    case CT_IVCH_LOAD:
    case CT_IVCH_PRAMP:
//             p_Info->m_bNeedFilter = 0;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_CRAMP:
    case CT_IVCH_CSTAIRCASE: //zyx 180831
//        pDelta->f_dIdt = p_Status->dXdt;
//        pDelta->f_dVdt = 0;
//             p_Info->m_bNeedFilter = 0;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_VRAMP:
//        pDelta->f_dIdt = 0;
//        pDelta->f_dVdt = ChannelStatus[ChannelIdx].dXdt;
//             p_Info->m_bNeedFilter = 0;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_CPULSE:
    case CT_IVCH_IR:
    case  CT_IVCH_ACIR:
    case CT_SPECIAL_IRTEST://zyx 2105291
//        p_Status->bNeedSDL = 0;
//             p_Info->m_bNeedFilter = 0;
        ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_SETVARIABLE:
    case CT_IVCH_SETVALUE: //200911 zyx  need Filter, Or it will lead to a unreasonable value in log data.
        break;
    case CT_IVCH_REST:
        ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Current].m_fValue = 0.0f;
        break;
    case CT_IVCH_CURRENT:
    case CT_IVCH_VOLTAGE:
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
        if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bDynamicControl)
//                 p_Info->m_bNeedFilter = 0;
            ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_CCCV_TB:  //TURBO BOOSTBY ZC 07.07.2017
    case CT_IVCH_CURRENT_TB: //CC TURBO BOOST BY ZC 06.04.2018
        if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_bDynamicControl)
//                 p_Info->m_bNeedFilter = 0;
            ChannelInfo_Manager::SetFilterStatus(ChannelIdx,false);
        break;
    case CT_IVCH_DIGIV:   //04.23.2013
    case CT_TYPE_DO_SETTING:
    case CT_IVCH_EXTCHARGE:
    case CT_IVCH_VSTAIRCASE:
    default:
        break;
    }
//     }
    IsChnNeedFilter[ChannelIdx] = ChannelInfo_Manager::GetFilterStatus(ChannelIdx);
//    pDelta->u32Voltage = fVoltage / ::abs(pV_Param->m_Adc.m_Gain);
//    pDelta->u32Current = fCurrent / ::abs(pI_Param->m_Adc.m_Gain);
//    //p_Info->m_u32CurrentRampDelta = MULTI_FACTOR * ACCURACY * p_Status->Imax / (::abs(pI_Param->m_Adc.m_Gain));
//    //    modified by hepeiqing,20160415,修复GROUP情况下，不能正常滤波bug
//    pDelta->f_dIdt2 = pDelta->f_dIdt  / pI_Param->m_Adc.m_Gain / 10000;
//    pDelta->f_dVdt2 = pDelta->f_dVdt  / pV_Param->m_Adc.m_Gain / 10000;

//    if(m_GrpInfo4SDL[ChannelIdx].State == eGroupState_Master)
//    {
//        pDelta->fCurrent = fCurrent;
//        //p_Info->m_u32CurrentRampDelta = MULTI_FACTOR * ACCURACY * p_Status->Imax;
//        //    modified by hepeiqing,20160415,修复GROUP情况下，不能正常滤波bug
//        pDelta->f_dIdt2 = pDelta->f_dIdt  / 10000;
//    }
}

//inline  unsigned char IVSDL::Check_Current(unsigned char bOK,St_SDL_History_Point HistoryPoint)
// inline  unsigned char IVSDL::Check_Current(unsigned char bOK, St_SDL_History_Point HistoryPoint) //mits8

// {
//     DEQUEUE(HistoryCurrentData[CurrentChannelIdx], SDL_HISTORY_DEPTH, HistoryPoint, bOK);     //处理一个 电流点
//     if(!bOK)
//         return 0;

//     if(HistoryPoint.bCriticalPoint)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = HistoryPoint.Time;
//         MySdlPoint.fValue[0] = HistoryPoint.fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
// // 		MySdlPoint.ucValueType = MetaCode_PV_Current;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MySdlPoint);

//         MySdlPoint.u32TestID = Schedule_Manager_task::m_dwTestID[CurrentChannelIdx];
//         MySdlPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_TestTime);
//         MySdlPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_Steptime);
//         MySdlPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
//         MySdlPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[CurrentChannelIdx].m_RunningStepIdx;
//         MySdlPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[CurrentChannelIdx];
//         ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );
//         LastLoggedCurrent[CurrentChannelIdx].u32Value = HistoryPoint.u32Value;
//         LastLoggedCurrent[CurrentChannelIdx].fValue = HistoryPoint.fValue;
//         LastLoggedCurrent[CurrentChannelIdx].Time = HistoryPoint.Time;
//         LastLoggedCurrent[CurrentChannelIdx].bLoged = 1;

// //		if(m_bPairIV)
// //		{
//         MySdlPoint.fValue[0] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_DV_Voltage].m_fValue;
//         MySdlPoint.fValue[1] = HistoryPoint.fValue2;
// //			MySdlPoint.ucValueType = MetaCode_PV_Voltage;
//         MySdlPoint.fValue[2] =  ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue;
//         MySdlPoint.fValue[3] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//         MySdlPoint.fValue[4] =  ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeEnergy].m_fValue;
//         MySdlPoint.fValue[5] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeEnergy].m_fValue;
//         MySdlPoint.ucDataCount = 6;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;  //ACITY;
//         PushPoint(MySdlPoint);
//         LastLoggedCurrent[CurrentChannelIdx].fValue2 = HistoryPoint.fValue2;
// //		}

//         LastLogTime[CurrentChannelIdx] = HistoryPoint.Time;
//         bHaveLoggedOnePoint = 1;
//     }
//     else
//     {
// //         if(m_GrpInfo4SDL[CurrentChannelIdx].State == eGroupState_Normal) 180511 zyx
// //             SDL_IV(CurrentChannelIdx, HistoryPoint, SDL_CURRENT);
// //         else
// //             fSDL_I(CurrentChannelIdx, HistoryPoint);
//     }
//     return 1;
// }
//inline  unsigned char IVSDL::Check_Voltage(unsigned char bOK,St_SDL_History_Point HistoryPoint)
// inline  unsigned char IVSDL::Check_Voltage(unsigned char bOK, St_SDL_History_Point HistoryPoint) //mits8

// {
//     DEQUEUE(HistoryVoltageData[CurrentChannelIdx], SDL_HISTORY_DEPTH, HistoryPoint, bOK);     //处理一个 电压点
//     if(!bOK)
//         return 0;

//     if(HistoryPoint.bCriticalPoint)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = HistoryPoint.Time;
//         MySdlPoint.fValue[1] = HistoryPoint.fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
// // 		MySdlPoint.ucValueType = MetaCode_PV_Voltage;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MySdlPoint);

//         MySdlPoint.u32TestID = Schedule_Manager_task::m_dwTestID[CurrentChannelIdx];
//         MySdlPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_TestTime);
//         MySdlPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_Steptime);
//         MySdlPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
//         MySdlPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[CurrentChannelIdx].m_RunningStepIdx;
//         MySdlPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[CurrentChannelIdx];
//         ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );
//         LastLoggedVoltage[CurrentChannelIdx].u32Value = HistoryPoint.u32Value;
//         LastLoggedVoltage[CurrentChannelIdx].fValue   = HistoryPoint.fValue;
//         LastLoggedVoltage[CurrentChannelIdx].Time     = HistoryPoint.Time;
//         LastLoggedVoltage[CurrentChannelIdx].bLoged   = 1;

// //		if(m_bPairIV)
// //		{
//         MySdlPoint.fValue[0] = HistoryPoint.fValue2;
// //			MySdlPoint.ucValueType = MetaCode_PV_Current;
//         MySdlPoint.fValue[1] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_Current].m_fValue; //MetaCode_DV_Current
//         MySdlPoint.fValue[2] =  ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue;
//         MySdlPoint.fValue[3] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//         MySdlPoint.fValue[4] =  ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeEnergy].m_fValue;
//         MySdlPoint.fValue[5] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeEnergy].m_fValue;
//         MySdlPoint.ucDataCount = 6;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;   //ACITY;
//         PushPoint(MySdlPoint);

//         LastLoggedVoltage[CurrentChannelIdx].fValue2 = HistoryPoint.fValue2;
// //		}

//         LastLogTime[CurrentChannelIdx] = HistoryPoint.Time;
//         bHaveLoggedOnePoint = 1;
//     }
//     else
//         SDL_IV(CurrentChannelIdx, HistoryPoint, SDL_VOLTAGE);
//     return 1;
// }

// inline  void IVSDL::Check_CCapacity()  //mits8
// {
//     float fMyValue = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue;
//     if((fMyValue - LastLoggedCCapacity[CurrentChannelIdx]) > DeltaValue[CurrentChannelIdx].fCapacity)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = OS_ServeProcess::OS_Time;
//         MySdlPoint.fValue[2] = fMyValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_ChargeCapacity;
//         LastLoggedCCapacity[CurrentChannelIdx] = fMyValue;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MySdlPoint);

//         MySdlPoint.u32TestID = Schedule_Manager_task::m_dwTestID[CurrentChannelIdx];
// //         MySdlPoint.u32DataPoint = IVSDL::m_dwDataPoint[CurrentChannelIdx]++;
//         MySdlPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_TestTime);
//         MySdlPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_Steptime);
//         MySdlPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_CycleIndex].m_fValue;
//         MySdlPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[CurrentChannelIdx].m_RunningStepIdx;
//         MySdlPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[CurrentChannelIdx];
//         ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );


//         //upload Change Cap time
//         //MySdlPoint.fValue =  ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx,MetaCode_PV_ChargeCapacityTime); ////zyx 171010
//         ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_ChargeCapacityTime);
//         MySdlPoint.fValue[3] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_ChargeCapacityTime;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAPACITY;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
//         PushPoint(MySdlPoint);

//         LastLogTime[CurrentChannelIdx] = OS_ServeProcess::OS_Time;
//         bHaveLoggedOnePoint = 1;
//     }

//     ChannelInfo_Manager::CapaEnerUpdated[CurrentChannelIdx].m_CCapacity = 0;   // 20140612 he  修改容量能量遗漏
// }
// inline  void IVSDL::Check_DCapacity()  //mits8
// {
//     float fMyValue = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//     if((fMyValue - LastLoggedDCapacity[CurrentChannelIdx]) > DeltaValue[CurrentChannelIdx].fCapacity)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = OS_ServeProcess::OS_Time;
//         MySdlPoint.fValue[3] = fMyValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_DischargeCapacity;
//         LastLoggedDCapacity[CurrentChannelIdx] = fMyValue;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MySdlPoint);

//         MySdlPoint.u32TestID = Schedule_Manager_task::m_dwTestID[CurrentChannelIdx];
// //         MySdlPoint.u32DataPoint = IVSDL::m_dwDataPoint[CurrentChannelIdx]++;
//         MySdlPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_TestTime);
//         MySdlPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_Steptime);
//         MySdlPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_CycleIndex].m_fValue;
//         MySdlPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[CurrentChannelIdx].m_RunningStepIdx;
//         MySdlPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[CurrentChannelIdx];
//         ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );


//         //upload Dischange Cap time
// // 		MySdlPoint.fValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx,MetaCode_PV_DischargeCapacityTime);//zyx 171010
//         ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_DischargeCapacityTime);
//         MySdlPoint.fValue[2] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_DischargeCapacityTime;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAPACITY;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
//         PushPoint(MySdlPoint);

//         LastLogTime[CurrentChannelIdx] = OS_ServeProcess::OS_Time;
//         bHaveLoggedOnePoint = 1;
//     }

//     ChannelInfo_Manager::CapaEnerUpdated[CurrentChannelIdx].m_DCapacity = 0;  // 20140612 he  修改容量能量遗漏
// }
// inline  void IVSDL::Check_CEnergy()  //mits8
// {
//     float fMyValue = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeEnergy].m_fValue;
//     if((fMyValue - LastLoggedCEnergy[CurrentChannelIdx]) > DeltaValue[CurrentChannelIdx].fEnergy)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = OS_ServeProcess::OS_Time;
//         MySdlPoint.fValue[2] = fMyValue;
//         MySdlPoint.fValue[3] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeEnergy].m_fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_ChargeEnergy;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_ENERGY;

//         LastLoggedCEnergy[CurrentChannelIdx] = fMyValue;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
//         PushPoint(MySdlPoint);

//         LastLogTime[CurrentChannelIdx] = OS_ServeProcess::OS_Time;
//         bHaveLoggedOnePoint = 1;
//     }

//     ChannelInfo_Manager::CapaEnerUpdated[CurrentChannelIdx].m_CEnergy = 0;
// }
// inline  void IVSDL::Check_DEnergy() //mits8
// {
//     float fMyValue = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_DischargeEnergy].m_fValue;

//     if((fMyValue - LastLoggedDEnergy[CurrentChannelIdx]) > DeltaValue[CurrentChannelIdx].fEnergy)
//     {
//         St_SDL_Point MySdlPoint;

//         MySdlPoint.Time = OS_ServeProcess::OS_Time;
//         MySdlPoint.fValue[3] = fMyValue;
//         MySdlPoint.fValue[2] = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_ChargeEnergy].m_fValue;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
// // 		MySdlPoint.ucValueType = MetaCode_PV_DischargeEnergy;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_ENERGY;

//         LastLoggedDEnergy[CurrentChannelIdx] = fMyValue;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
//         PushPoint(MySdlPoint);

//         LastLogTime[CurrentChannelIdx] = OS_ServeProcess::OS_Time;
//         bHaveLoggedOnePoint = 1;
//     }

//     ChannelInfo_Manager::CapaEnerUpdated[CurrentChannelIdx].m_DEnergy = 0;   // 20140612 he  修改容量能量遗漏
// }
// inline  void IVSDL::CheckCapEner(unsigned short ChannelIdx)
// {
//     bool IsUpdate = false;
//     if(ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_CCapacity)
//     {
//         float fMyValue = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue;
//         if((fMyValue - LastLoggedCCapacity[ChannelIdx]) > DeltaValue[ChannelIdx].fCapacity)
//         {
//             LastLoggedCCapacity[ChannelIdx] = fMyValue;
//             LastLogTime[ChannelIdx] = OS_ServeProcess::OS_Time;
//             bHaveLoggedOnePoint = 1;
//             IsUpdate = true;
//         }
//         ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_CCapacity = 0;
//     }
//     if(ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_DCapacity)
//     {
//         float fMyValue = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
//         if((fMyValue - LastLoggedDCapacity[ChannelIdx]) > DeltaValue[ChannelIdx].fCapacity)
//         {
//             LastLoggedDCapacity[ChannelIdx] = fMyValue;
//             LastLogTime[ChannelIdx] = OS_ServeProcess::OS_Time;
//             bHaveLoggedOnePoint = 1;
//             IsUpdate = true;
//         }
//         ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_DCapacity = 0;
//     }
//     if(ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_CEnergy)
//     {
//         float fMyValue = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_ChargeEnergy].m_fValue;
//         if((fMyValue - LastLoggedCEnergy[ChannelIdx]) > DeltaValue[ChannelIdx].fEnergy)
//         {
//             LastLoggedCEnergy[ChannelIdx] = fMyValue;
//             LastLogTime[ChannelIdx] = OS_ServeProcess::OS_Time;
//             bHaveLoggedOnePoint = 1;
//             IsUpdate = true;
//         }
//         ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_CEnergy = 0;
//     }
//     if(ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_DEnergy)
//     {
//         float fMyValue = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DischargeEnergy].m_fValue;
//         if((fMyValue - LastLoggedDEnergy[ChannelIdx]) > DeltaValue[ChannelIdx].fEnergy)
//         {
//             LastLoggedDEnergy[ChannelIdx] = fMyValue;
//             LastLogTime[ChannelIdx] = OS_ServeProcess::OS_Time;
//             bHaveLoggedOnePoint = 1;
//             IsUpdate = true;
//         }
//         ChannelInfo_Manager::CapaEnerUpdated[ChannelIdx].m_DEnergy = 0;   // 20140612 he  修改容量能量遗漏
//     }

//     if(IsUpdate)
//     {
//         St_SDL_Point MySdlPoint;
//         MySdlPoint.Time = OS_ServeProcess::OS_Time;
//         MySdlPoint.u16ChannelIdx = CurrentChannelIdx;
//         MySdlPoint.bCriticalPoint = 0;
//         MySdlPoint.ucType = SDL_DATA_FOR_LOG;
//         MySdlPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;
//         MySdlPoint.u16ChannelIdx = ChannelIdx;
//         MySdlPoint.bCriticalPoint = LOG_NORMAL_DATA_POINT;
//         MySdlPoint.u32TestID = Schedule_Manager_task::m_dwTestID[CurrentChannelIdx];
//         MySdlPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_TestTime);
//         MySdlPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(CurrentChannelIdx, MetaCode_PV_Steptime);
//         MySdlPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[CurrentChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
//         MySdlPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[CurrentChannelIdx].m_RunningStepIdx;
//         MySdlPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[CurrentChannelIdx];
//         ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );

//         MySdlPoint.fValue[0] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Current);
//         MySdlPoint.fValue[1] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Voltage);
//         MySdlPoint.fValue[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
//         MySdlPoint.fValue[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
//         MySdlPoint.fValue[4] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy);
//         MySdlPoint.fValue[5] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy);
//         MySdlPoint.ucDataCount = 6;
//         MySdlPoint.u32DataPoint = ++ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32;//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DataPoints); //180516 zyx//IVSDL::m_dwDataPoint[ChannelIdx];
//         //ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DataPoints, (MySdlPoint.u32DataPoint + 1)); //mits8 180516 zyx//IVSDL::m_dwDataPoint[ChannelIdx]++;
//         PushPoint(MySdlPoint);
//     }
// }
/*
void IVSDL::SDL_IV(unsigned short ChannelIdx, St_SDL_History_Point Point, unsigned char type)
{
//     unsigned long u32Delta;
//     St_SDL_Point MyPoint;
//     unsigned long long u64TimeDiff;
//     unsigned long LastValue;

//     // the folling variables are initialized by "type"
//     unsigned char Ctrl;
//     unsigned char Type;
//     unsigned char PairType;
//     unsigned long			*		p_Counter;
//     St_SDL_History_Point	*		p_FirstLogged;
//     St_SDL_History_Point	*		p_LastLogged;
//     St_SDL_History_Point	*		p_Previous;
//     float							Delta_dt2;
//     //float						      Delta_dt;
//     //float                           f_Gain;
//     unsigned long					Delta_Value;
//     //------------------------------------------------

//     if(type == SDL_CURRENT)
//     {
//         p_Counter			=		&CurrentCounter[ChannelIdx];
//         p_FirstLogged		=		&FirstLoggedCurrent[ChannelIdx];
//         p_LastLogged		=		&LastLoggedCurrent[ChannelIdx];
//         p_Previous			=		&PreviousCurrent[ChannelIdx];
//         //f_Gain              =       ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[ChannelStatus[ChannelIdx].ucCurrentRange].m_Adc.m_Gain;
//         //Delta_dt            =		DeltaValue[ChannelIdx].f_dIdt;
//         Delta_dt2			=		DeltaValue[ChannelIdx].f_dIdt2;
//         Delta_Value			=		DeltaValue[ChannelIdx].u32Current;
//         Type				=		MetaCode_PV_Current;
//         PairType			=		MetaCode_PV_Voltage;
//         Ctrl                =       CT_IVCH_CRAMP;
//     }
//     else if(type == SDL_VOLTAGE)
//     {
//         p_Counter			=		&VoltageCounter[ChannelIdx];
//         p_FirstLogged		=		&FirstLoggedVoltage[ChannelIdx];
//         p_LastLogged		=		&LastLoggedVoltage[ChannelIdx];
//         p_Previous			=		&PreviousVoltage[ChannelIdx];
//         //f_Gain              =       ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[ChannelStatus[ChannelIdx].ucVoltageRange].m_Adc.m_Gain;
//         //Delta_dt            =		DeltaValue[ChannelIdx].f_dVdt;
//         Delta_dt2			=		DeltaValue[ChannelIdx].f_dVdt2;
//         Delta_Value			=		DeltaValue[ChannelIdx].u32Voltage;
//         Type				=		MetaCode_PV_Voltage;
//         PairType			=		MetaCode_PV_Current;
//         Ctrl                =       CT_IVCH_VRAMP;
//     }

//     if(ChannelStatus[ChannelIdx].u16ControlType == Ctrl)
//     {
//         if(*p_Counter > 5)     //Cramp里如果连续LOG了5个点，则表示不正常，需要重新计算dIdt和dIdt2
//         {
//             *p_Counter = 0;
//             u64TimeDiff = GetTimeTicks(p_FirstLogged->Time.Second, p_FirstLogged->Time.Us100, Point.Time.Second, Point.Time.Us100);
//             Delta_dt2  = (signed long)Point.u32Value - (signed long)p_FirstLogged->u32Value;
//             Delta_dt2 /= u64TimeDiff;
//             //Delta_dt  *= f_Gain * 10000; //no need
//         }

//         u64TimeDiff = GetTimeTicks(p_LastLogged->Time.Second, p_LastLogged->Time.Us100, Point.Time.Second, Point.Time.Us100); //计算两个RecordTime结构之间的时间差函数调用
//         LastValue = p_LastLogged->u32Value + Delta_dt2 * u64TimeDiff;
//     }
//     else
//         LastValue = p_LastLogged->u32Value;

//     MyPoint.bCriticalPoint = 0;
//     MyPoint.u16ChannelIdx = ChannelIdx;
// // 	MyPoint.ucValueType = Type;
//     MyPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAPACITY;

//     MyPoint.u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
// //     MyPoint.u32DataPoint = IVSDL::m_dwDataPoint[CurrentChannelIdx]++;
//     MyPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_TestTime);
//     MyPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Steptime);
//     MyPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue;
//     MyPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;
//     MyPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[ChannelIdx];
//     ChannelInfo_Manager::ClearDataFlag(CurrentChannelIdx, 0xFFFFFFFF );


//     u32Delta = (Point.u32Value > LastValue) ? (Point.u32Value - LastValue) : (LastValue - Point.u32Value);

//     if(u32Delta >= Delta_Value)
//     {   //需要push something
//         *p_LastLogged = Point;
//         if(*p_Counter == 0)
//             *p_FirstLogged = Point;

//         //unsigned long counter = *p_Counter;
//         //counter++;
//         //*p_Counter = counter;
//         *p_Counter = (*p_Counter + 1);

//         if(!p_Previous->bLoged)
//         {   //上一个点没有LOG，进来检查看看，这次是否需要顺便将上一个点也LOG了？
//             if(ChannelStatus[ChannelIdx].u16ControlType == Ctrl)
//             {
//                 u64TimeDiff = GetTimeTicks(p_Previous->Time.Second, p_Previous->Time.Us100, Point.Time.Second, Point.Time.Us100);
//                 LastValue = p_Previous->u32Value + Delta_dt2 * u64TimeDiff;
//             }
//             else
//             {
//                 LastValue = p_Previous->u32Value;
//             }
//             u32Delta = (Point.u32Value > LastValue) ? (Point.u32Value - LastValue) : (LastValue - Point.u32Value);

//             if(u32Delta >= Delta_Value)
//             {
//                 MyPoint.bCriticalPoint = 0;
//                 if(type == SDL_CURRENT)
//                     MyPoint.fValue[0] = p_Previous->fValue;
//                 else
//                     MyPoint.fValue[1] = p_Previous->fValue;
//                 MyPoint.Time = p_Previous->Time;
//                 MyPoint.ucType = SDL_DATA_FOR_LOG;
// // 				PushPoint(MyPoint);       //push previous point

// // 				if(m_bPairIV)
// // 				{
// // 					MyPoint.fValue = p_Previous->fValue2;
//                 if(type == SDL_CURRENT)
//                     MyPoint.fValue[1] = p_Previous->fValue2;
//                 else
//                     MyPoint.fValue[0] = p_Previous->fValue2;
// // 					MyPoint.ucValueType = PairType;
//                 PushPoint(MyPoint);
// // 				}
//             }
//         }
// // 		MyPoint.ucValueType = Type;
// // 		MyPoint.fValue = Point.fValue;
//         if(type == SDL_CURRENT)
//             MyPoint.fValue[0] = p_Previous->fValue;
//         else
//             MyPoint.fValue[1] = p_Previous->fValue;
//         MyPoint.Time = Point.Time;
//         MyPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MyPoint);

// // 		if(m_bPairIV)
// // 		{
// // 			MyPoint.fValue = Point.fValue2;
//         if(type == SDL_CURRENT)
//             MyPoint.fValue[1] = p_Previous->fValue2;
//         else
//             MyPoint.fValue[0] = p_Previous->fValue2;
// // 			MyPoint.ucValueType = PairType;
//         PushPoint(MyPoint);
// // 		}

//         p_Previous->bLoged = 1;

//         LastLogTime[ChannelIdx].Second = Point.Time.Second;
//         LastLogTime[ChannelIdx].Us100 = Point.Time.Us100;
//         bHaveLoggedOnePoint = 1;
//     }
//     else
//     {   //不需要push something,just record present value to previous;
//         *p_Previous = Point;
//         p_Previous->bLoged = 0;
//         *p_Counter = 0;
//     }
}*/
// void IVSDL::fSDL_I(unsigned short ChannelIdx, St_SDL_History_Point CurrentPoint) //mits8 180511 zyx
// {
//     float fDelta;
//     St_SDL_Point MyPoint;
//     unsigned long long u64TimeDiff;
//     float LastValue;

//     if(ChannelStatus[ChannelIdx].u16ControlType == CT_IVCH_CRAMP)
//     {
//         if(CurrentCounter[ChannelIdx] > 5)     //Cramp里如果连续LOG了5个点，则表示不正常，需要重新计算dIdt和dIdt2
//         {
//             u64TimeDiff = GetTimeTicks(FirstLoggedCurrent[ChannelIdx].Time.Second, FirstLoggedCurrent[ChannelIdx].Time.Us100, CurrentPoint.Time.Second, CurrentPoint.Time.Us100);
//             DeltaValue[ChannelIdx].f_dIdt2  = (signed long)CurrentPoint.fValue - (signed long)FirstLoggedCurrent[ChannelIdx].fValue;
//             DeltaValue[ChannelIdx].f_dIdt2 /= u64TimeDiff;
//         }

//         u64TimeDiff = GetTimeTicks(LastLoggedCurrent[ChannelIdx].Time.Second, LastLoggedCurrent[ChannelIdx].Time.Us100, CurrentPoint.Time.Second, CurrentPoint.Time.Us100); //计算两个RecordTime结构之间的时间差函数调用
//         LastValue = LastLoggedCurrent[ChannelIdx].fValue + DeltaValue[ChannelIdx].f_dIdt2 * u64TimeDiff;
//     }
//     else
//         LastValue = LastLoggedCurrent[ChannelIdx].fValue;

//     MyPoint.bCriticalPoint = 0;
//     MyPoint.u16ChannelIdx = ChannelIdx;
// // 	MyPoint.ucValueType = MetaCode_PV_Current;
//     MyPoint.ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAPACITY;

//     MyPoint.u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
// //     MyPoint.u32DataPoint = IVSDL::m_dwDataPoint[ChannelIdx]++;
//     MyPoint.fTestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_TestTime);
//     MyPoint.fStepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Steptime);
//     MyPoint.u16Cycle_Idx = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue;
//     MyPoint.u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;
//     MyPoint.u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[ChannelIdx];
//     ChannelInfo_Manager::ClearDataFlag(ChannelIdx, 0xFFFFFFFF );


//     fDelta = ::abs(CurrentPoint.fValue - LastValue);

//     if(fDelta >= DeltaValue[ChannelIdx].fCurrent)
//     {   //需要push something
//         LastLoggedCurrent[ChannelIdx] = CurrentPoint;
//         if(CurrentCounter[ChannelIdx] == 0)
//             FirstLoggedCurrent[ChannelIdx] = CurrentPoint;
//         CurrentCounter[ChannelIdx]++;
//         if(!PreviousCurrent[ChannelIdx].bLoged)
//         {   //上一个点没有LOG，进来检查看看，这次是否需要顺便将上一个点也LOG了？
//             if(ChannelStatus[ChannelIdx].u16ControlType == CT_IVCH_CRAMP)
//             {
//                 u64TimeDiff = GetTimeTicks(PreviousCurrent[ChannelIdx].Time.Second, PreviousCurrent[ChannelIdx].Time.Us100, CurrentPoint.Time.Second, CurrentPoint.Time.Us100);
//                 LastValue = PreviousCurrent[ChannelIdx].fValue + DeltaValue[ChannelIdx].f_dIdt2 * u64TimeDiff;
//             }
//             else
//                 LastValue = PreviousCurrent[ChannelIdx].fValue;
//             fDelta = ::abs(CurrentPoint.fValue > LastValue);

//             if(fDelta >= DeltaValue[ChannelIdx].fCurrent)
//             {
//                 MyPoint.bCriticalPoint = 0;
//                 MyPoint.fValue[0] = PreviousCurrent[ChannelIdx].fValue;
//                 MyPoint.Time = PreviousCurrent[ChannelIdx].Time;
//                 MyPoint.ucType = SDL_DATA_FOR_LOG;
// // 				PushPoint(MyPoint);       //push previous point

// // 				if(m_bPairIV)
// // 				{
//                 MyPoint.fValue[1] = PreviousCurrent[ChannelIdx].fValue2;
// // 					MyPoint.ucValueType = MetaCode_PV_Voltage;
//                 PushPoint(MyPoint);
// // 				}
//             }
//         }
// // 		MyPoint.ucValueType = MetaCode_PV_Current;
//         MyPoint.fValue[0] = CurrentPoint.fValue;
//         MyPoint.Time = CurrentPoint.Time;
//         MyPoint.ucType = SDL_DATA_FOR_LOG;
// // 		PushPoint(MyPoint);

// // 		if(m_bPairIV)
// // 		{
//         MyPoint.fValue[1] = CurrentPoint.fValue2;
// // 			MyPoint.ucValueType = MetaCode_PV_Voltage;
//         PushPoint(MyPoint);
// // 		}

//         PreviousCurrent[ChannelIdx].bLoged = 1;

//         LastLogTime[ChannelIdx].Second = CurrentPoint.Time.Second;
//         LastLogTime[ChannelIdx].Us100 = CurrentPoint.Time.Us100;
//         bHaveLoggedOnePoint = 1;
//     }
//     else
//     {   //不需要push something,just record present value to previous;
//         PreviousCurrent[ChannelIdx] = CurrentPoint;
//         PreviousCurrent[ChannelIdx].bLoged = 0;
//         CurrentCounter[ChannelIdx] = 0;
//     }
// }
void IVSDL::SetFactor_Time(unsigned short ChannelIdx, float Factor)
{
    if(Step_Manager_task::b_IsRapidlyCommun[ChannelIdx])
        return;
    if(Factor < 0)
        Factor = -Factor;

    DeltaValue[ChannelIdx].f_Time = Factor;
    if(DeltaValue[ChannelIdx].f_Time <= 0)
        DeltaValue[ChannelIdx].f_Time = ChannelInfo_Manager::BoardCFG.m_Global.m_fSdlTime;
    if(DeltaValue[ChannelIdx].f_Time <= 0)
        DeltaValue[ChannelIdx].f_Time = IV_DEFAULT_SDL_TIME;

    DeltaValue[ChannelIdx].u64Time = DeltaValue[ChannelIdx].f_Time * 10000;   //将秒转换为 百微秒为单位；
}
void IVSDL::SetFactor(unsigned short ChannelIdx, float Factor, int FactorType)
{
    float* Accuracy =  &ChannelStatus[ChannelIdx].fCurrentAccuracy;
    float* fSdlFactor = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_I;
    if(abs(Factor) > 0.0f)
        Accuracy[FactorType] = Factor;
    else if( fSdlFactor[FactorType] > 0.0f)
        Accuracy[FactorType] = fSdlFactor[FactorType];
    else
    {
        float  DefaultFactor[4] = {IV_DEFAULT_SDL_FACTOR_I,IV_DEFAULT_SDL_FACTOR_V,IV_DEFAULT_SDL_FACTOR_CAPACITY,IV_DEFAULT_SDL_FACTOR_ENERGY};
        Accuracy[FactorType] = DefaultFactor[FactorType];
    }
}
// void IVSDL::SetFactor_V(unsigned short ChannelIdx, float Factor)
// {
//     if(Factor < 0)
//         Factor = -Factor;

//     ChannelStatus[ChannelIdx].fVoltageAccuracy = Factor;
//     if(ChannelStatus[ChannelIdx].fVoltageAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fVoltageAccuracy = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_V;
//     if(ChannelStatus[ChannelIdx].fVoltageAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fVoltageAccuracy = IV_DEFAULT_SDL_FACTOR_V;
// }
// void IVSDL::SetFactor_Capacity(unsigned short ChannelIdx, float Factor)
// {
//     if(Factor < 0)
//         Factor = -Factor;

//     ChannelStatus[ChannelIdx].fCapacityAccuracy = Factor;
//     if(ChannelStatus[ChannelIdx].fCapacityAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fCapacityAccuracy = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_Capacity;
//     if(ChannelStatus[ChannelIdx].fCapacityAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fCapacityAccuracy = IV_DEFAULT_SDL_FACTOR_CAPACITY;
// }
// void IVSDL::SetFactor_Energy(unsigned short ChannelIdx, float Factor)
// {
//     if(Factor < 0)
//         Factor = -Factor;

//     ChannelStatus[ChannelIdx].fEnergyAccuracy = Factor;
//     if(ChannelStatus[ChannelIdx].fEnergyAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fEnergyAccuracy = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_fSdlFactor_Energy;
//     if(ChannelStatus[ChannelIdx].fEnergyAccuracy <= 0)
//         ChannelStatus[ChannelIdx].fEnergyAccuracy = IV_DEFAULT_SDL_FACTOR_ENERGY;
// }
void IVSDL::SetCapacity(unsigned short ChannelIdx, float Value)
{
    if(Value < 0)
        Value = -Value;

    ChannelStatus[ChannelIdx].fBatteryCapacity = Value;
    if(ChannelStatus[ChannelIdx].fBatteryCapacity <= 0)
    {
        signed char SchBufIdx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_scBufIdx;
        if((SchBufIdx >= 0) && (SchBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER))
        {
            if(ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_bValid)
                ChannelStatus[ChannelIdx].fBatteryCapacity = ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ScheduleBuf[SchBufIdx].m_ucMacCurrentRange);
        }
    }
    if(ChannelStatus[ChannelIdx].fBatteryCapacity <= 0)
        ChannelStatus[ChannelIdx].fBatteryCapacity = ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::CurrRngPhysicalRange1);
    if(ChannelStatus[ChannelIdx].fBatteryCapacity <= 0)
        ChannelStatus[ChannelIdx].fBatteryCapacity = IV_DEFAULT_SDL_NORMINAL_CAPACITY;
}
void IVSDL::SetNVoltage(unsigned short ChannelIdx, float Value)
{
    if(Value < 0)
        Value = -Value;

    ChannelStatus[ChannelIdx].fBatteryVoltage = Value;
    if(ChannelStatus[ChannelIdx].fBatteryVoltage <= 0)
        ChannelStatus[ChannelIdx].fBatteryVoltage = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
    if(ChannelStatus[ChannelIdx].fBatteryVoltage <= 0)
        ChannelStatus[ChannelIdx].fBatteryVoltage = IV_DEFAULT_SDL_NORMINAL_VOLTAGE;
}

//inline unsigned long long IVSDL::GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100)
/*inline unsigned long long IVSDL::GetTimeTicks(unsigned long FromInsecond, unsigned short FromUs100, unsigned long ToInsecond, unsigned short ToUs100)

{
    if(ToInsecond < FromInsecond)
        return 0;
    if((ToInsecond == FromInsecond) && (ToUs100 <= FromUs100))
        return 0;

    //	MsTime timediff;
    Int16 temp;
    unsigned long long TimeTicks;  //以100us为单位的时间单位数

    temp = ToUs100 - FromUs100;
    if(temp < 0)
    {
        temp = temp + 10000;
        ToInsecond --;
    }

    TimeTicks  = ToInsecond - FromInsecond;
    TimeTicks *= 10000;
    TimeTicks += temp;

    return TimeTicks;
}*/

inline void IVSDL::PushPoint(St_SDL_Point_4BytesCycleIdx MyPoint)
{
    ENQUEUE(Buffer, SDL_QUEUE_DEPTH, MyPoint);
}

//inline void IVSDL::AdjustSdlSpeed(void)
//{
//    switch(ucLowSpeedFactor)
//    {
//    case 4:
//        ucLowSpeedFactor_New = 4;
//        return;
//    case 2:
//        if(Buffer.m_u32Length > SDL_QUEUE_DEPTH_7IN8)
//        {
//            ucLowSpeedFactor_New = 4;
//            AdjustSdlFactor();
//        }
//        else if(Buffer.m_u32Length < SDL_QUEUE_DEPTH_3IN4)
//        {
//            ucLowSpeedFactor_New = 1;
//            AdjustSdlFactor();
//        }
//        return;
//    case 1:
//    default:
//        if(Buffer.m_u32Length > SDL_QUEUE_DEPTH_7IN8)
//        {
//            ucLowSpeedFactor_New = 4;
//            AdjustSdlFactor();
//        }
//        else if(Buffer.m_u32Length > SDL_QUEUE_DEPTH_3IN4)
//        {
//            ucLowSpeedFactor_New = 2;
//            AdjustSdlFactor();
//        }
//        return;
//    }
//}
//inline void IVSDL::AdjustSdlFactor(void)
//{
//    if(ucLowSpeedFactor_New == ucLowSpeedFactor)
//        return;

//    for (unsigned char ChannelIdx = 0; ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
//    {
//        //DeltaValue[ChannelIdx].fCurrent /= ucLowSpeedFactor;  // no need
//        //DeltaValue[ChannelIdx].fVoltage /= ucLowSpeedFactor;
//        DeltaValue[ChannelIdx].u32Voltage /= ucLowSpeedFactor;
//        DeltaValue[ChannelIdx].u32Current /= ucLowSpeedFactor;
//        DeltaValue[ChannelIdx].fEnergy /= ucLowSpeedFactor;
//        DeltaValue[ChannelIdx].fCapacity /= ucLowSpeedFactor;

//        //DeltaValue[ChannelIdx].fCurrent *= ucLowSpeedFactor_New;    // no need
//        //DeltaValue[ChannelIdx].fVoltage *= ucLowSpeedFactor_New;
//        DeltaValue[ChannelIdx].u32Voltage *= ucLowSpeedFactor_New;
//        DeltaValue[ChannelIdx].u32Current *= ucLowSpeedFactor_New;
//        DeltaValue[ChannelIdx].fEnergy *= ucLowSpeedFactor_New;
//        DeltaValue[ChannelIdx].fCapacity *= ucLowSpeedFactor_New;
//    }

//    ucLowSpeedFactor_New = ucLowSpeedFactor;
//}

inline float IVSDL::GetFullValue(float HighValue, float LowValue)
{
    float Value1, Value2;

//	Value1 = HighValue;
//	if(Value1 < 0)
//		Value1 = -Value1;

//	Value2 = LowValue;
//	if(Value2 < 0)
//		Value2 = -Value2;

    Value1 = abs(HighValue);
    Value2 = abs(LowValue);

    if(Value1 < Value2)
        return Value2 * 2;
    else
        return Value1 * 2;




}

void IVSDL::Init_SubMasterFrequentlyLog( void )
{
    SubMasterFrequentlyLog.bNeedFrequentlyLog = false;
    SubMasterFrequentlyLog.LogCount = 0;
}


/*进行IV_SUB_SDL_FREQUENTLY_COUNT次快速汇报后，将SubMaster从快速汇报（10ms）转换为正常汇报（50ms）*/
inline	void IVSDL::SubMasterSdlLogTimesFrequently2Normal(unsigned short ChannelIdx)
{
    unsigned char	ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    if(p_Ctrl->m_u16ControlType == CT_IVCH_CURRENT_TB || p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_TB) //2018.09.06 DBPT测试
    {
        if( (SubMasterFrequentlyLog.bNeedFrequentlyLog == true) && (SubMasterFrequentlyLog.LogCount < IV_SUB_SDL_FREQUENTLY_COUNT_FOR_DBPT) )
        {
            SubMasterFrequentlyLog.LogCount++;
            if(SubMasterFrequentlyLog.LogCount >= IV_SUB_SDL_FREQUENTLY_COUNT_FOR_DBPT)
            {
                SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_NORMAL);
                SubMasterFrequentlyLog.LogCount = 0;
                SubMasterFrequentlyLog.bNeedFrequentlyLog = false;
            }
        }
    }
    else
    {
        if( (SubMasterFrequentlyLog.bNeedFrequentlyLog == true) && (SubMasterFrequentlyLog.LogCount < IV_SUB_SDL_FREQUENTLY_COUNT) )
        {
            if(Step_Manager_task::b_IsRapidlyCommun[ChannelIdx])
                return;
            SubMasterFrequentlyLog.LogCount++;
            if(SubMasterFrequentlyLog.LogCount >= IV_SUB_SDL_FREQUENTLY_COUNT)
            {
                SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_NORMAL);
                SubMasterFrequentlyLog.LogCount = 0;
                SubMasterFrequentlyLog.bNeedFrequentlyLog = false;
            }
        }
    }
}

/*将SubMaster从正常汇报（50ms）转换为快速汇报（10ms）*/
void IVSDL::SubMasterSdlLogTimesNormal2Frequently(unsigned short ChannelIdx)
{
    unsigned char	ActiveIdx = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;
    St_StepCtrl			* 	p_Ctrl			=	&ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
    if(SubMasterFrequentlyLog.bNeedFrequentlyLog == false)
    {
        if(p_Ctrl->m_u16ControlType == CT_IVCH_CURRENT_TB || p_Ctrl->m_u16ControlType == CT_IVCH_CCCV_TB)
        {
            SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_FREQUENTLY_FOR_DBPT); //09.06.2018		DBPT测试并联时按照100us更新
        }
        else
        {
            SetFactor_Time(ChannelIdx, IV_SUB_SDL_TIME_FREQUENTLY);
        }
        SubMasterFrequentlyLog.bNeedFrequentlyLog = true;
        LastLogTime[ChannelIdx] = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
    }
    SubMasterFrequentlyLog.LogCount = 0;
}
/////////////////////////////////////add for built_in Aux dirui 2017/12/07
/////////////////////////////add for built_in Aux   dirui 2017/12/07
void IVSDL::Check_BuiltInAuxForLog(unsigned short ChannelIdx, unsigned char uc_LogType ) //循环发送
{
// 	unsigned char uc_IVUnitBufIdx= board_config::getBID();  //需要确认
    unsigned char uc_IVUnitBufIdx = Smart_Battery::m_SMBUnitID;
    if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
            || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))
    {   //190524,hpq
        if(board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044)
            TriggerLog_BuiltInAux(uc_IVUnitBufIdx, ChannelIdx, 0, uc_LogType);
        unsigned char uc_ChanCnt = (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount);
        for(int i = 0; i < uc_ChanCnt; i++)
        {
            // pReport->fBuiltInData[i]=ChannelInfo_Manager::Read_SpecifiedMetavariable2(uw16_ChannelNumber,MetaCode_PV_BuiltIn_Data1 + i);
            TriggerLog_BuiltInAux(uc_IVUnitBufIdx, ChannelIdx, i, uc_LogType);
        }
    }
}
void IVSDL::TriggerLog_BuiltInAux(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxChn, unsigned char uc_LogType)
{
    ST_BUILTIN_AUX_DATA_TRIG DataTrigInfo;
    St_SDL_Point_Pack * st_Point;
    st_Point = (St_SDL_Point_Pack *) &EthernetParse::m_Point_Temp[0];
    MsTime SysTime = CommonFunc::GetSystemTime();
    st_Point->u32Second =  SysTime.Second;
    st_Point->u16Us100 = SysTime.Us100;

    unsigned char uc_ChanCnt = (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount);
    unsigned char uc_BuiltInBoardchn = uc_IVchn * uc_ChanCnt + uc_AuxChn;
    if((uc_LogType == SDL_DATA_FOR_DISPLAY) || (uc_LogType == SDL_DATA_FOR_IDLE))
    {   //显示数据或者闭时数据
        st_Point->ucType = uc_LogType;

        DataTrigInfo.m_IVChnIdx = uc_IVchn;
        DataTrigInfo.m_IV_unitID = uc_IVUnitBufIdx; // Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
        DataTrigInfo.m_AuxType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_BuiltInBoardchn].m_AuxType;
        DataTrigInfo.TrigTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        DataTrigInfo.m_value = 	ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_IVchn, MetaCode_PV_BuiltIn_Data1 + uc_AuxChn);
        DataTrigInfo.m_AuxChnIdx  = uc_BuiltInBoardchn; //ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount * uc_AuxChn +uc_IVchn ;//Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
        TrigLogBuiltInPoint(&DataTrigInfo, st_Point);

        return;
    }

    if(uc_LogType == SDL_DATA_FOR_LOG)
    {   //记录数据
// 		if(m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].bLogPcData) // 按发送方向PC/IV_MCU更新记录时间
// 		{
        st_Point->ucType = SDL_DATA_FOR_LOG;
        DataTrigInfo.m_IVChnIdx = uc_IVchn;
        DataTrigInfo.m_IV_unitID = uc_IVUnitBufIdx;//Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
        DataTrigInfo.m_AuxType = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_BuiltInBoardchn].m_AuxType;
        DataTrigInfo.m_value = 	ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_IVchn, MetaCode_PV_BuiltIn_Data1 + uc_AuxChn);
        DataTrigInfo.TrigTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        DataTrigInfo.m_AuxChnIdx  = uc_BuiltInBoardchn;// ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount * uc_AuxChn +uc_IVchn ;//uc_IVchn;//Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
        TrigLogBuiltInPoint(&DataTrigInfo, st_Point);
        //m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].fValue = st_Point->fValue;  // 保存更新的采样数据值

//        IVSDL::m_LastBuiltInValue[uc_IVchn][uc_BuiltInBoardchn] = DataTrigInfo.m_value;
// 		}
    }
}
void IVSDL::TrigLogBuiltInPoint( ST_BUILTIN_AUX_DATA_TRIG * TrigInfo, St_SDL_Point_Pack * MyPoint)
{
// 	Aux_GetData( TrigInfo->m_IV_unitID,TrigInfo->m_IVChnIdx,TrigInfo->m_AuxType,TrigInfo->m_AuxChnIdx,&st_Aux_Data);
    //此处填写读出的builtIn数据
// 	if(st_Aux_Data.value_update_time.Second < MyPoint->u32Second)     //160429
// 		return;
// 	if(  (st_Aux_Data.value_update_time.Second == MyPoint->u32Second)
// 		&& (st_Aux_Data.value_update_time.Us100 <= MyPoint->u16Us100) ) //160429
// 	  return;
    unsigned char uc_bankgroup;
    MyPoint->u32Second = TrigInfo->TrigTime.Second;
    MyPoint->u16Us100 = TrigInfo->TrigTime.Us100;
    //MyPoint->ucValueType = MetaCode_PV_Aux_Value;
    MyPoint->fValue =  TrigInfo->m_value;
    MyPoint->bIV = 0;
    MyPoint->ucValueType =  TrigInfo->m_AuxType;
    //   if( TrigInfo->m_AuxType == AUX_TEMPERATURE)
    //    ;// MyPoint->ucSensor =  Aux_DataManager::m_AllAuxBoards[TrigInfo->m_AuxType].m_AuxSensor_ChInfo[TrigInfo->m_AuxChnIdx].m_AuxTemp.m_AuxThermoCouple.SensorType;//暂时屏蔽 齐静明20150603
    //   else
    //     MyPoint->ucSensor = TrigInfo->m_AuxType;
    //MyPoint->ucChannelIdx = TrigInfo->m_AuxChnIdx;  //190524,hpq
    MyPoint->ucChannelIdx = TrigInfo->m_IVChnIdx;     //190524,hpq
    unsigned char uc_AuxType =TrigInfo->m_AuxType;
    if(uc_AuxType ==AUX_VOLTAGE)
        uc_bankgroup = BuiltIn_DataManager::m_BuiltInAuxInfo.VolDataBank;
    else
        uc_bankgroup = 	BuiltIn_DataManager::m_BuiltInAuxInfo.TempDataBank;
    st_BuiltIn_Aux_Data.m_Chn_Info.m_AuxType = TrigInfo->m_AuxType;
    st_BuiltIn_Aux_Data.aux_dx_dt= BuiltIn_DataManager::m_BuiltInDataBank[uc_bankgroup][TrigInfo->m_AuxChnIdx].aux_dx_dt;

    EthernetParse::MPReportData(TrigInfo->m_IVChnIdx, SEND_BUILTIN_DATA); // PC 以太网络发送
}

// void IVSDL::IVSdlPushPoint(unsigned short ChannelIdx, unsigned char ucDataType, unsigned char ucValueType, unsigned char bCriticalPoint)
// {
// 		St_SDL_Point MySdlPoint;
// 		MySdlPoint.Time = OS_ServeProcess::OS_Time;
// 		MySdlPoint.fValue = ChannelInfo_Manager::MetaVariable2[ChannelIdx][ucValueType].m_fValue;
// 		MySdlPoint.u16ChannelIdx = ChannelIdx;
// 		MySdlPoint.bCriticalPoint = bCriticalPoint;
// 		MySdlPoint.ucValueType = ucValueType;
// 		MySdlPoint.ucType = ucDataType;
// 		IVSDL::PushPoint(MySdlPoint);
//     return;
// }


//////////////////////////////////////
