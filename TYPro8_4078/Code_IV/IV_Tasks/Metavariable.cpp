//改名为METAVARIABE
//MV维护
//updata 2012 11 14 by chen
//2013 02 07 更新  chen    暂时并没有工作
//2013 02 18 更新  chen    加入了积分部分

#include "../Entry/includes.h"
#include "Metavariable.h"

//#include "app/manage/DataManage.h"   //大数据体

#define MVST_WAIT 				11
#define MVST_CALCULATE 		12

// float MetaVariable_Task::Voltage_Buf[MAXCHANNELNO][11];
// int MetaVariable_Task::VB_Handle[MAXCHANNELNO];


int MetaVariable_Task::VSMB_Handle[MAXCHANNELNO];
DXDT_INFO MetaVariable_Task::dVdtInfo[MAXCHANNELNO] = {0};
int MetaVariable_Task::Cap_Calcu_Ptr;
int MetaVariable_Task::MYSTATUS;
int MetaVariable_Task::My_SMBLog_Status;
MsTime MetaVariable_Task::m_capacity_Time[MAXCHANNELNO];    //电量时间
MsTime MetaVariable_Task::m_SMBRef_Time[MAX_SMB_BMS_SIGNAL];
char MetaVariable_Task::capacityStatus[MAXCHANNELNO];

/***************** dQdV **************************/
LastValue MetaVariable_Task::dQdV[MAXCHANNELNO];

/***************** dVdQ **************************/
LastValue MetaVariable_Task::dVdQ[MAXCHANNELNO];
ST_DXDT_VALUE     MetaVariable_Task::st_History[2][MAX_AUXCH_PER_DATA_GROUP];
bool MetaVariable_Task::bSendMVUD[MAXCHANNELNO] = {0};
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
ST_SMBDATABUF	MetaVariable_Task::SMBData_Buf[200];
#pragma arm section
ST_SMBDATABUF  MetaVariable_Task::m_SMBLogData;
ST_SMBDATABUF mSmbBufData;
UInt16 MetaVariable_Task::Priority = PRIORITY8;
UInt16 MetaVariable_Task::TaskID = MV_TASK;			//  should be a definition which positioning this task in OS_Task[]

MsTime MetaVariable_Task::M0_RefreshInterval;
bool MetaVariable_Task::b_RedundantVotlage_NeedUpdate[MAXCHANNELNO] = {0};

void MetaVariable_Task::Init(void)
{
    //全部置0
    for(unsigned char i = 0; i < MAXCHANNELNO ; i++)    //遍历通道
    {
        capacityStatus[i] = 0;
// // 		/***************** dQdV **************************/
        Init_dQdv(i);
// /***************** dVdQ **************************/
        Init_dVdq(i);
        MetaVariable_Task::InitDvdt(i);
    }
    memset(&st_History,0,sizeof(st_History));

    VSMB_Handle[0] = CQ::Register(&MetaVariable_Task::SMBData_Buf[0].My_CurrentHnd, sizeof(ST_SMBDATABUF), 200);
    CQ::InitQueue(VSMB_Handle[0]);
    CQ::ResetQueueSize(VSMB_Handle[0], 200);



    MYSTATUS = MVST_WAIT;
    Cap_Calcu_Ptr = 0;


    CommonFunc::GetTimeTicksUpdate(&M0_RefreshInterval);
}

void MetaVariable_Task::UpdateValues_WithMV_UD()
{
    for(unsigned char i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
    {
        if(Schedule_Manager_task::IfSchRunning(i) == 0)
            continue;

        if(ChannelInfo_Manager::uc_MV_Udx_Update[i] == 1)
        {
            if((ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[i]&Glb_Safety_CURRENT) != 0)
                ChannelInfo_Manager::GetScheduleSafty(i, MP_SCH_SAFTY_TYPE_CURRENT, &safety_check::Check_S[i].checkSCH_I);
            if((ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[i]&Glb_Safety_VOLTAGE) != 0)
            {
                ChannelInfo_Manager::GetScheduleSafty(i, MP_SCH_SAFTY_TYPE_VOLTAGE, &safety_check::Check_S[i].checkSCH_V);
                b_RedundantVotlage_NeedUpdate[i] = 1;
            }
            if((ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[i]&Glb_Safety_POWER) != 0)
                ChannelInfo_Manager::GetScheduleSafty(i, MP_SCH_SAFTY_TYPE_POWER, &safety_check::Check_S[i].checkSCH_P);
            if((ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[i]&Glb_Safety_CAPACITY) != 0)
                ChannelInfo_Manager::GetScheduleSafty(i, MP_SCH_SAFTY_TYPE_CAPACITY, &safety_check::Check_S[i].checkSCH_C);
            if((ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[i]&Glb_Safety_STEPTIME) != 0)
                ChannelInfo_Manager::GetScheduleSafty(i, MP_SCH_SAFTY_TYPE_STEPTIME, &safety_check::Check_S[i].checkSCH_t);

            ChannelInfo_Manager::uc_MV_Udx_Update[i] = 0;
        }

        if(b_RedundantVotlage_NeedUpdate[i] == 1)
        {
            if(ChannelInfo_Manager::uc_MV_Udx_Glb_RedundantVotlage[i] == 1)
            {
                if(CommonFunc::GetTimeTicks(&M0_RefreshInterval) > M0_REFRESH_TIME)
                {
                    St_SaftyCheckItem compactSafetyInfo = {0};
                    short TypeIdx = M0RedundantFunc::getThisTypeIdx(eType_Voltage);
                    if(TypeIdx != -1)
                    {
                        M0RedundantFunc::GetM0SafetyInfo(i,eType_Voltage,&compactSafetyInfo);
                        if(compactSafetyInfo.m_fHigh != M0RedundantFunc::m_RedundantChnMap[TypeIdx].m_fSafetyH[i])
                            M0RedundantFunc::SetRedundantSafety(i,eType_Voltage);
                    }
                    CommonFunc::GetTimeTicksUpdate(&M0_RefreshInterval);
                    b_RedundantVotlage_NeedUpdate[i] = 0;
                }

            }
        }
    }
}

void MetaVariable_Task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_META] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_META);
#endif

//	calculate_dVdt();//1901042 zyx
    calculate_capacity(0, 99);
    calculate_dQdV();
    calculate_dVdQ();

    UpdateValues_WithMV_UD();

    calculate_SMBRefreshValue();

    if(CQ::GetItemNumber(VSMB_Handle[0]) > 0)

    {
//
        unsigned char uc_SMB = SEND_NONE;
        CQ::Dequeue(VSMB_Handle[0], &m_SMBLogData);
// // 		Smart_Battery::m_SMBLogIdx=mSmbBufData.MySMBMv_ID;
// // 		m_SMBLog_Time[mSmbBufData.MySMBMv_ID]=mSmbBufData.MyRecorderTime;
        if(((ChannelInfo_Manager::ChannelInfo[0].ScheduleStatus==RUNNING) || (Smart_Battery::LastLog_Flag[m_SMBLogData.MySMBMv_ID])) && (m_SMBLogData.MySMBRecordType == SEND_LOG_SMB_DATA)) //modify by zc 03.04.2019
        {
            if(	Smart_Battery::LastLog_Flag[m_SMBLogData.MySMBMv_ID]) //add by zc 03.04.2019
                Smart_Battery::LastLog_Flag[m_SMBLogData.MySMBMv_ID] = false;
            if(m_SMBLogData.MySMBRecordData.m_type == TYPE_WORD )
// 				EthernetParse::MPReportData(0,SEND_LOG_SMB_DATA_FLOAT);
                uc_SMB = SEND_LOG_SMB_DATA_FLOAT;
            if(m_SMBLogData.MySMBRecordData.m_type == TYPE_STRING )
// 				EthernetParse::MPReportData(0,SEND_LOG_SMB_DATA_STRING);
                uc_SMB = SEND_LOG_SMB_DATA_STRING;

        }
        else
        {
            if(m_SMBLogData.MySMBRecordData.m_type == TYPE_WORD )
// 				EthernetParse::MPReportData(0,SEND_IDLE_SMB_DATA_FLOAT);
                uc_SMB = SEND_IDLE_SMB_DATA_FLOAT;
            if(m_SMBLogData.MySMBRecordData.m_type == TYPE_STRING )
// 				EthernetParse::MPReportData(0,SEND_IDLE_SMB_DATA_STRING);
                uc_SMB = SEND_IDLE_SMB_DATA_STRING;

        }
        if(uc_SMB != SEND_NONE)
            EthernetParse::MPReportData(0, uc_SMB);
    }

    for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
    {
        if((ChannelInfo_Manager::ChannelSchedule[i].m_scBufIdx < 0) || (ChannelInfo_Manager::ChannelSchedule[i].m_scBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
            continue;

        if(ChannelInfo_Manager::ScheduleBuf[ChannelInfo_Manager::ChannelSchedule[i].m_scBufIdx].m_ItemCount.m_u16FormulaCount > 0)
        {
            memset(ChannelInfo_Manager::m_FormulaValue[i], 0, sizeof(ST_FORMULA_VALUE) * IV_MAX_SCHEDULE_FORMULA_NUMBER); //180905 zyx
        }
        if(bSendMVUD[i])
        {
            EthernetParse::MPReportData(i,SEND_MVUDSYNCH);
            bSendMVUD[i] = false;
        }
    }
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_META] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_META);
#endif
}

// void MetaVariable_Task::EndTask(void)
// {
//
// }

void MetaVariable_Task::calculate_SMBRefreshValue(void)
{

    switch (My_SMBLog_Status)
    {
    case SMBLOG_IDLE:
        for(unsigned char uc_i = 0; uc_i < MAX_SMB_BMS_SIGNAL; uc_i++)
        {

            MetaVariable_Task::m_SMBRef_Time[uc_i] = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;

        }
        if(Smart_Battery::BeSupportSmbus)// && ChannelInfo_Manager::ChannelInfo[0].ScheduleStatus==RUNNING)
            My_SMBLog_Status = SMBLOG_WAIT;
        break;
    case SMBLOG_WAIT:
        if(!Smart_Battery::BeSupportSmbus)// && ChannelInfo_Manager::ChannelInfo[0].ScheduleStatus!=RUNNING)
        {
            My_SMBLog_Status = SMBLOG_IDLE;
            break;
        }
        //MsTime timediff;
        //Int16 temp;
        float TimeInSecond;
//       MsTime OStime;
        for(unsigned char uc_i = 0; uc_i < Smart_Battery::m_SMBCfgBuf.m_buf[0].m_MetaCount; uc_i++)
        {

            MsTime TimeDiff = CommonFunc::TimeDiff(MetaVariable_Task::m_SMBRef_Time[uc_i]); //~{!@~}????~{!B~}???????????????????~{!@~}?????? zyx 171225

            TimeInSecond = TimeDiff.Second + TimeDiff.Us100 / 10000;

            float RefreshInterval;   //判断LOG速度   //add by zc 03.04.2019
            float LogInterval;
            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh == FAST)
                RefreshInterval = Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Fast;//add by zc 03.04.2019
//					RefreshInterval = Smart_Battery::m_Refresh_Fast;//add by zc 03.04.2019 for test
            else if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Refresh == SLOW)
                RefreshInterval = Smart_Battery::m_SMBCfgBuf.m_buf[0].m_Refresh_Slow;//add by zc 03.04.2019
//					RefreshInterval = Smart_Battery::m_Refresh_Slow;//add by zc 03.04.2019 for test
            else
                continue;

            if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Logging_Interval == SYNC_WITH_REFRESH)  //与Refresh速度同步//add by zc 03.04.2019
//			if(Smart_Battery::m_Logging_Interval[uc_i] == SYNC_WITH_REFRESH)  //与Refresh速度同步	//add by zc 03.04.2019 for test
                LogInterval = RefreshInterval;
            else if(Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Logging_Interval > SYNC_WITH_REFRESH)//add by zc 03.04.2019
//			else if(Smart_Battery::m_Logging_Interval[uc_i] > SYNC_WITH_REFRESH) //add by zc 03.04.2019 for test
            {
                LogInterval = RefreshInterval* Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_Logging_Interval;//add by zc 03.04.2019
//				LogInterval = RefreshInterval* Smart_Battery::m_Logging_Interval[uc_i];//add by zc 03.04.2019 for test
                if(TimeInSecond>=RefreshInterval)
                    Step_Manager_task::Log_IdleSMBusData(0,uc_i);
            }
            else
            {
                if(TimeInSecond>=RefreshInterval)
                    Step_Manager_task::Log_IdleSMBusData(0,uc_i);
                continue;
            }
            if(TimeInSecond>=LogInterval)
            {
                mSmbBufData.MyRecorderTime = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
                Smart_Battery::m_PreviousLogUploadTime[uc_i] = mSmbBufData.MyRecorderTime;
                Smart_Battery::m_PreviousIdleUploadTime[uc_i] = mSmbBufData.MyRecorderTime;
                mSmbBufData.MySMBMv_ID= Smart_Battery::m_SMBCfgBuf.m_buf[0].m_ValidSignal[uc_i].m_SMB_RX_Meta;//uc_i;
                mSmbBufData.MySMBRecordData=ChannelInfo_Manager::m_SMBusMetaVariable[0][mSmbBufData.MySMBMv_ID];//uc_i
                mSmbBufData.MySMBRecordType = SEND_LOG_SMB_DATA;
                CQ::Enqueue(VSMB_Handle[0], &mSmbBufData);



                CommonFunc::GetTimeTicksUpdate(&MetaVariable_Task::m_SMBRef_Time[uc_i]);
            }
        }

        break;


    default:
        if(!Smart_Battery::BeSupportSmbus)// && ChannelInfo_Manager::ChannelInfo[0].ScheduleStatus!=RUNNING)
            My_SMBLog_Status = SMBLOG_IDLE;

        break;
    }

}

// void MetaVariable_Task::calculate_dVdt(void)    // all channels will be processed here in perticular order.
// {
//     float fVoltage;
//     MsTime tempT;
//     int i;

//     switch(MYSTATUS)
//     {
//     case MVST_WAIT:
//         tempT = OS_ServeProcess::TimeDiff(LastSampleTime);
//         if(tempT.Us100 >= 1000)
//         {
//             //	for(i = 0; i < MAXCHANNELNO; i ++)
//             for(i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; i ++)   //Add by DKQ 06_19_2014
//                 CQ::Enqueue(VB_Handle[i], &ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue);
//             OS_ServeProcess::recordCurTime(&LastSampleTime);
//             V_Calcu_Ptr = 0;
//             MYSTATUS = MVST_CALCULATE;
//         }
//         return;

//     case MVST_CALCULATE:
//         //		if(V_Calcu_Ptr >= MAXCHANNELNO)			// finish all channels.
//         if(V_Calcu_Ptr >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount  )			// finish all channels.
//         {
//             MYSTATUS = MVST_WAIT;
//             return;
//         }

//         if(CQ::GetItemNumber(VB_Handle[V_Calcu_Ptr]) < 11)			// not 1 second yet, then assume this is not ready, and proceed to the next channel data.
//         {
//             V_Calcu_Ptr ++;
//             return;
//         }

//         CQ::Dequeue(VB_Handle[V_Calcu_Ptr], &fVoltage);		// Get the voltage from 1 second ago
//         fVoltage = ChannelInfo_Manager::Get_MetaVariable(V_Calcu_Ptr, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage) - fVoltage;	// because 1 second based, so fVoltage / 1 = fVoltage
//         ChannelInfo_Manager::MetaVariable2[V_Calcu_Ptr][MetaCode_PV_dVdt].m_fValue = fVoltage;    //把dvdt的值存放到大数据体内
//         V_Calcu_Ptr ++;
//         return;

//     default:
//         MYSTATUS = MVST_WAIT;
//     }
// }
void MetaVariable_Task::InitDvdt(unsigned char ChannelIdx)
{
    dVdtInfo[ChannelIdx].m_deltaV = abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange].m_Bound.m_High \
                                      * ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdt.m_fDeltaXMin * 2);
    dVdtInfo[ChannelIdx].V_accuracy = abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange].m_Bound.m_High \
                                      * 0.0004f);

    dVdtInfo[ChannelIdx].m_TimeMin = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdt.m_fDeltaTimeMin;
    dVdtInfo[ChannelIdx].m_TimeMax = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdt.m_fDeltaTimeMax;

    dVdtInfo[ChannelIdx].bRefresh = false;
    dVdtInfo[ChannelIdx].m_VoltLast = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
    dVdtInfo[ChannelIdx].m_Dxdt = 0;
    CommonFunc::GetTimeTicksUpdate(&dVdtInfo[ChannelIdx].LastSampleTime);
}
void MetaVariable_Task::Calculate_dVdt(unsigned char uc_ChannelNo)
{
    float Voltage = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
    float time = ChannelInfo_Manager::fHowLongToNow(dVdtInfo[uc_ChannelNo].LastSampleTime);
    float nowDeltaVoltage = Voltage - dVdtInfo[uc_ChannelNo].m_VoltLast; // current Voltage difference
    if(abs(nowDeltaVoltage) > abs(dVdtInfo[uc_ChannelNo].m_deltaV) && time > dVdtInfo[uc_ChannelNo].m_TimeMin)
    {
        dVdtInfo[uc_ChannelNo].m_Dxdt = nowDeltaVoltage / time;
        if(dVdtInfo[uc_ChannelNo].m_Dxdt <= FLT_MAX && dVdtInfo[uc_ChannelNo].m_Dxdt >= -FLT_MAX)
        {
            dVdtInfo[uc_ChannelNo].m_VoltLast = Voltage;
            CommonFunc::GetTimeTicksUpdate(&dVdtInfo[uc_ChannelNo].LastSampleTime);
            if(ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_dVdt].m_fValue != dVdtInfo[uc_ChannelNo].m_Dxdt) \
            {
                dVdtInfo[uc_ChannelNo].bRefresh = true;
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_dVdt].m_fValue = dVdtInfo[uc_ChannelNo].m_Dxdt;
            }
        }
    }
    else if(time > dVdtInfo[uc_ChannelNo].m_TimeMax)//abs(nowDeltaVoltage) > abs(dVdtInfo[uc_ChannelNo].V_accuracy) && 
    {
        dVdtInfo[uc_ChannelNo].m_Dxdt = nowDeltaVoltage / time;
        if(dVdtInfo[uc_ChannelNo].m_Dxdt <= FLT_MAX && dVdtInfo[uc_ChannelNo].m_Dxdt >= -FLT_MAX)
        {
            dVdtInfo[uc_ChannelNo].m_VoltLast = Voltage;
            CommonFunc::GetTimeTicksUpdate(&dVdtInfo[uc_ChannelNo].LastSampleTime);
            if(ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_dVdt].m_fValue != dVdtInfo[uc_ChannelNo].m_Dxdt) \
            {
                dVdtInfo[uc_ChannelNo].bRefresh = true;
                ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_dVdt].m_fValue = dVdtInfo[uc_ChannelNo].m_Dxdt;
            }
        }   
    }
    return;
}

void MetaVariable_Task::calculate_dQdV(void)    // all channels will be processed here in perticular order.
{
    for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)   //Add by DKQ 06_19_2014
    {
        if(!Schedule_Manager_task::IfSchRunning(i))
            continue;
        
        float dQ = (ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_ChargeCapacity].m_fValue - ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_DischargeCapacity].m_fValue) - dQdV[i].LastCapacity; //
        float dV = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue - dQdV[i].LastVoltage;
        if( abs(dV) >= dQdV[i].DeltaV_min
                && CommonFunc::GetTimeTicks(&dQdV[i].LastSampleTime) > dQdV[i].TimeMin  ) 
        {
            float fdQdv = dQ / dV;
            if(IsFiniteNumber(fdQdv))
            {
                ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_dQdV].m_fValue = fdQdv;
                dQdV[i].LastCapacity = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_ChargeCapacity].m_fValue - ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_DischargeCapacity].m_fValue;  //
                dQdV[i].LastVoltage = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue; //更新电压值
                CommonFunc::GetTimeTicksUpdate(&dQdV[i].LastSampleTime);//?~{(9~}???~{!@~}??
                dQdV[i].IsRefresh = true;
            }
        }
        else if(CommonFunc::TimeDiffInSecond( dQdV[i].LastSampleTime.Second) > dQdV[i].TimeMax && abs(dV) >= dQdV[i].MinDeltaValue)
        {
            float fdQdv = dQ / dV;
            if(IsFiniteNumber(fdQdv))
            {
                ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_dQdV].m_fValue = fdQdv;
                dQdV[i].LastCapacity = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_ChargeCapacity].m_fValue - ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_DischargeCapacity].m_fValue;  //
                dQdV[i].LastVoltage = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue; //更新电压值
                CommonFunc::GetTimeTicksUpdate(&dQdV[i].LastSampleTime);//?~{(9~}???~{!@~}??
                dQdV[i].IsRefresh = true;
            }
        }
    }
    return;
}

void MetaVariable_Task::calculate_dVdQ(void)    // all channels will be processed here in perticular order.
{
    for(int i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)   //Add by DKQ 06_19_2014
    {
        if(!Schedule_Manager_task::IfSchRunning(i))
            continue;
        if(ChannelInfo_Manager::m_IR_Info[i].m_IR_Stop_Logging == true)
            continue;
        if(CommonFunc::GetTimeTicks(&dVdQ[i].LastSampleTime) > dVdQ[i].TimeMin) //100
        {
            float CurrentCap = (ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_ChargeCapacity].m_fValue - ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_DischargeCapacity].m_fValue);

            float dQ = CurrentCap - dVdQ[i].LastCapacity;
            if(abs(dQ) >= dVdQ[i].DeltaQ_min)
            {
                float dV = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue - dVdQ[i].LastVoltage;
                float dVdq = dV / dQ;
                if(IsFiniteNumber(dVdq))
                {
                    ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_dVdQ].m_fValue = dVdq;
                    MetaVariable_Task::dVdQ[i].IsRefresh = true; //zyx 180905
                    dVdQ[i].LastCapacity = CurrentCap; 
                    dVdQ[i].LastVoltage = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue; //更新电压值
                    CommonFunc::GetTimeTicksUpdate(&dVdQ[i].LastSampleTime);//?~{(9~}???~{!@~}??
                }

            }
        }
        else if(CommonFunc::TimeDiffInSecond( dVdQ[i].LastSampleTime.Second) > dVdQ[i].TimeMax)
        {
            float CurrentCap = (ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_ChargeCapacity].m_fValue - ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_DischargeCapacity].m_fValue);

            float dQ = CurrentCap - dVdQ[i].LastCapacity;
            if(abs(dQ) >= dVdQ[i].MinDeltaValue)
            {
                float dV = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue - dVdQ[i].LastVoltage;
                float dVdq = dV / dQ;
                if(IsFiniteNumber(dVdq))
                {
                    ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_dVdQ].m_fValue = dVdq;
                    MetaVariable_Task::dVdQ[i].IsRefresh = true; //zyx 180905
                    dVdQ[i].LastCapacity = CurrentCap; 
                    dVdQ[i].LastVoltage = ChannelInfo_Manager::MetaVariable2[i][MetaCode_PV_Voltage].m_fValue; //更新电压值
                    CommonFunc::GetTimeTicksUpdate(&dVdQ[i].LastSampleTime);//?~{(9~}???~{!@~}??
                }

            }
        }
            
    }
    return;
}
// void MetaVariable_Task::Init_dVdt(unsigned char ChannelIdx)
// {
//     CQ::InitQueue(VB_Handle[ChannelIdx]);
// }

void MetaVariable_Task::Init_dVdq(unsigned char ChannelIdx)
{
    dVdQ[ChannelIdx].LastVoltage = 0.0f;
    dVdQ[ChannelIdx].LastCapacity = 0.0f;
    dVdQ[ChannelIdx].IsRefresh = false;
    dVdQ[ChannelIdx].firstPoint = false;
//    dVdQ[ChannelIdx].DeltaQ_min = ( 0.002f * 2.777777e-8f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange));
//    dVdQ[ChannelIdx].DeltaV_min = (ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * 0.001f);
    dVdQ[ChannelIdx].DeltaQ_min = (  0.001f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange));
    dVdQ[ChannelIdx].MinDeltaValue = 0.0001f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange);
    dVdQ[ChannelIdx].TimeMin = 0.1f;
    dVdQ[ChannelIdx].TimeMax = 120;
    CommonFunc::GetTimeTicksUpdate(&dVdQ[ChannelIdx].LastSampleTime);
    ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_dVdQ].m_fValue = 0;
}

void MetaVariable_Task::Init_dQdv(unsigned char ChannelIdx)
{
    dQdV[ChannelIdx].LastVoltage = 0.0f;
    dQdV[ChannelIdx].LastCapacity = 0.0f;
    dQdV[ChannelIdx].IsRefresh = false;
    dQdV[ChannelIdx].firstPoint = false;
//    dQdV[ChannelIdx].DeltaQ_min = (0.002f * 2.777777e-8f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange));
//    dQdV[ChannelIdx].DeltaV_min = (ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * 0.001f);
    dQdV[ChannelIdx].DeltaV_min =    abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * 0.0004f);
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPS21024)
        dQdV[ChannelIdx].MinDeltaValue = abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * 0.00004f);
    else
        dQdV[ChannelIdx].MinDeltaValue = abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * 0.0001f);
    dQdV[ChannelIdx].TimeMin = 0.1f;
    dQdV[ChannelIdx].TimeMax = 120;
    CommonFunc::GetTimeTicksUpdate(&dQdV[ChannelIdx].LastSampleTime);
    ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_dQdV].m_fValue = 0; //MetaCode_PV_dQdV

}

void MetaVariable_Task::update_dVdq(unsigned char ChannelIdx)
{
    // MetaVariable_Task::calculate_capacity(1, ChannelIdx);  //171205 zyx
    dVdQ[ChannelIdx].LastVoltage = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
    dVdQ[ChannelIdx].LastCapacity = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue \
                                    - ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
    dVdQ[ChannelIdx].IsRefresh = false;
    dVdQ[ChannelIdx].firstPoint = true;
//    dVdQ[ChannelIdx].DeltaQ_min = 0.002f * 2.777777e-8f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange);// 0.001 * ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_MV_NorminalCapacity].m_fValue;//0.002 * 2.777777e-8f * ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_IRange[ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange].m_Bound.m_High;
//    dVdQ[ChannelIdx].DeltaV_min = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdQ.m_fDeltaXMin;
    dVdQ[ChannelIdx].DeltaQ_min = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdQ.m_fDeltaXMin * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange);
    dVdQ[ChannelIdx].TimeMin = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdQ.m_fDeltaTimeMin * 10000;
    dVdQ[ChannelIdx].TimeMax = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdQ.m_fDeltaTimeMax;

    CommonFunc::GetTimeTicksUpdate(&dVdQ[ChannelIdx].LastSampleTime);
    ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_dVdQ].m_fValue = 0;
}

void MetaVariable_Task::update_dQdV(unsigned char ChannelIdx)
{
    dQdV[ChannelIdx].LastVoltage = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
    dQdV[ChannelIdx].LastCapacity = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_ChargeCapacity].m_fValue \
                                    - ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DischargeCapacity].m_fValue;
    dQdV[ChannelIdx].IsRefresh = false;
//    dQdV[ChannelIdx].DeltaQ_min = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dQdV.m_fDeltaXMin * 2.777777e-8f * ChannelInfo_Manager::GetMaxCurrRangeValue(ChannelIdx,ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange);////0.002 *
//    dQdV[ChannelIdx].DeltaV_min = ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High*0.0002f;//0.001f;//ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High * ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dVdQ.m_fDeltaXMin;
    dQdV[ChannelIdx].DeltaV_min = abs(ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dQdV.m_fDeltaXMin * ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High*2);
    dQdV[ChannelIdx].TimeMin = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dQdV.m_fDeltaTimeMin * 10000;
    dQdV[ChannelIdx].TimeMax = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_dQdV.m_fDeltaTimeMax;

    CommonFunc::GetTimeTicksUpdate(&dQdV[ChannelIdx].LastSampleTime);
    ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_dQdV].m_fValue = 0; //MetaCode_PV_dQdV

}
/* requestType, 0 : not mandatory, only do capacity calculation when new sampling is available.
// requestType, 1 : mandatory, do capacity calcuation no matter new sampling is available or not.
// this usually happens at the end of of a step. The correct order of operation is:
// 1. cmd or conditions require a step to stop.
// 2. do mandatory cap calcu to updating the final capacity value.
// 3. record all data that needed by this event.
// 4. start stopping this step(transition process)*/
void MetaVariable_Task::calculate_capacity(char requestType, char chn)
{
    unsigned short uw16_Capacity = MetaCode_MV_COUNT;
    unsigned short uw16_Time = MetaCode_MV_COUNT;
    int i;
    double fCapacity = 0;    //??
    double fEnergy = 0;
    double time = 0;
    MsTime temp_time;


    // if not mandatory cap, Cycling channel # being processed.
    if(requestType == 0)
    {
        chn = 99;
        // since we also need to update the status and timerecord of some channels,
        // we don't start from Cap_Calcu_Ptr and we have to go through all channels.
        for(i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; i ++)
        {
            if(CheckAndSetCapCandidate(requestType, i) == 1)
            {
                if(chn == 99)
                    chn = i;
                else if(chn <= Cap_Calcu_Ptr && i > Cap_Calcu_Ptr)
                    chn = i;
            } // endif
        }// end for

        // if we want to do time interval control in future, we can do it here by recognizing the change pattern of Cap_Calcu_Ptr <<<LATER>>>-philip 8/6/2013-
        // e.g. if(chn!= 99){ if(chn <= Cap_Calcu_Ptr) { //means another round here,
        // a 5ms time interval need to be waited by using State machine. and Cap_Calcu_Ptr is set to 0. etc.} }
        if(chn != 99)		// if found, reset Cap_Calcu_Ptr
            Cap_Calcu_Ptr = chn;
    }
    else	// if mandatory cap process request. check if valid.
    {
        if(chn >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            return;   //  20161008  yangy

        if(CheckAndSetCapCandidate(requestType, chn) == -1)
            chn = 99;
    }

    if(chn != 99)	// if next valid channel is found.
    {
        if(Step_Manager_task:: bExtChargeFlag[chn])
            return;

        ChannelInfo_Manager::Isamp_NeedRecalculateSign[chn].m_Bits.CalCapacity = 0; // zyx 210513
        ChannelInfo_Manager::Vsamp_NeedRecalculateSign[chn].m_Bits.CalCapacity = 0;

//         unsigned long irq1, irq2;  //modified by hpq,161212,fix 0.2% capacity error bug
//         IRQFunc::DisableIRQ(&irq1, &irq2);
        temp_time = CommonFunc::TimeDiffAndUpdateMyTime(&m_capacity_Time[chn]);//2111011 zyx
//         CommonFunc::GetTimeTicksUpdate(&m_capacity_Time[chn]);    //??????
        time = temp_time.Us100 * 2.777777e-8f; 		// = * 1.0e-5 / 3600
        if(temp_time.Second > 0) //1811231 hpq
            time += temp_time.Second * 2.777777e-4f;
//         IRQFunc::EnableIRQ(irq1, irq2); //modified by hpq,161212

        //fCapacity = ChannelInfo_Manager::Get_MetaVariable(chn,MP_DATA_TYPE_METAVALUE,MetaCode_PV_Current) * time;
        fCapacity = ChannelInfo_Manager::IV_Buffer1[chn].m_fCurrentBeforeRC * time;
        fEnergy = fCapacity * ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,MetaCode_PV_Voltage);
        // for energy We may also need to get the first voltage too, however, for battery, there's no large vibration in V,
        // and we can leave it now and deal it <<<LATER>>>-philip 8/6/2013- if necessory

        //abnormal status   for USA's request,20150930
        if(((ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CCCV)  //cccv step
                ||(ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CCCV_WRM))
                && (Step_Manager_task::ChannelCCCVStatus[chn] == STEP_CCCV_CV)                                                        //cccv step,cv stage
                && ((Step_Manager_task::CCCV_TargetCurrent[chn] >= 0.0f) && (fCapacity < 0.0f)))
        {   //charge CCCV,CV stage,but current is discharge status
// 			ChannelInfo_Manager::IncCCapacity(chn,fCapacity);      //?????????
// 			ChannelInfo_Manager::IncCEnergy(chn,fEnergy);
//             ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeCapacity,chn,fCapacity);
//             ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeEnergy,chn,fEnergy);
// 			if(CT_IVCH_IR != ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
//                 ChannelInfo_Manager::IncDCapacityTime(chn, temp_time.Us100); //zyx 171010
//                 ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_DischargeCapacityTime,chn,temp_time.Us100);
            uw16_Capacity = MetaCode_PV_ChargeCapacity;
            uw16_Time = MetaCode_PV_DischargeCapacityTime;
        }
        else if(((ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CCCV)  //cccv step
                 ||(ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_CCCV_WRM))
                && (Step_Manager_task::ChannelCCCVStatus[chn] == STEP_CCCV_CV)                                                        //cccv step,cv stage
                && ((Step_Manager_task::CCCV_TargetCurrent[chn] < 0.0f) && (fCapacity > 0.0f)))
        {   //discharge CCCV,CV stage,but current is charge status
// 			ChannelInfo_Manager::IncDCapacity(chn,-fCapacity);      //?????????
// 			ChannelInfo_Manager::IncDEnergy(chn,-fEnergy);
//             ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeCapacity,chn,-fCapacity);
//             ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeEnergy,chn,-fEnergy);
// 			if(CT_IVCH_IR != ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
            //ChannelInfo_Manager::IncCCapacityTime(chn, temp_time.Us100); //zyx 171010
//                  ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_ChargeCapacityTime,chn,temp_time.Us100);
            uw16_Capacity = MetaCode_PV_DischargeCapacity;
            uw16_Time = MetaCode_PV_ChargeCapacityTime;
            fCapacity = -fCapacity;
            fEnergy   = -fEnergy;
        }
        else
        {   //normal status    for USA's request,20150930
            if(fCapacity > 0)    //??
            {
// 				ChannelInfo_Manager::IncCCapacity(chn,fCapacity);     //?????????
// 				ChannelInfo_Manager::IncCEnergy(chn,fEnergy);
//                 ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeCapacity,chn,fCapacity);
//                 ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeEnergy,chn,fEnergy);
// 				if(CT_IVCH_IR != ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
// 					ChannelInfo_Manager::IncCCapacityTime(chn, temp_time.Us100); //zyx 171010
//                     ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_ChargeCapacityTime,chn,temp_time.Us100);
                uw16_Capacity = MetaCode_PV_ChargeCapacity;
                uw16_Time = MetaCode_PV_ChargeCapacityTime;
            }
            else if(fCapacity < 0)     //??
            {
// 				ChannelInfo_Manager::IncDCapacity(chn,-fCapacity);      //?????????
// 				ChannelInfo_Manager::IncDEnergy(chn,-fEnergy);
//                 ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeCapacity,chn,-fCapacity);
//                 ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeEnergy,chn,-fEnergy);
// 				if(CT_IVCH_IR != ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
// 					ChannelInfo_Manager::IncDCapacityTime(chn, temp_time.Us100); //zyx 171010
//                     ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_DischargeCapacityTime,chn,temp_time.Us100);
                uw16_Capacity = MetaCode_PV_DischargeCapacity;
                uw16_Time = MetaCode_PV_DischargeCapacityTime;
                fCapacity = -fCapacity;
                fEnergy = -fEnergy;
            }
        }


        if(uw16_Capacity != MetaCode_MV_COUNT)
        {
            ChannelInfo_Manager::IncCapacityOrEnergy(uw16_Capacity, chn, fCapacity);
            ChannelInfo_Manager::IncCapacityOrEnergy(uw16_Capacity + 2, chn, fEnergy);
            if(CT_IVCH_IR != ChannelInfo_Manager::ChannelStep[chn].m_Info[ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
            {
                unsigned long ulTime = temp_time.Us100 * 100 + temp_time.Second* 1000000;
                ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(uw16_Time, chn, ulTime);
            }
        }
    }
}

// return -1: not valid channel for cap process, return 1: valid.
int16 MetaVariable_Task::CheckAndSetCapCandidate(char requestType, char chn)
{
    // if the channel's off or rest, or
    // if the channel's not running, e.g. it's in transition stage
    // we don't do cap caculation.
    if(ChannelInfo_Manager::ChannelInfo[chn].m_CtrlState == CCS_REST	\
            || ChannelInfo_Manager::ChannelInfo[chn].m_CtrlState == CCS_OFF	\
            || ChannelInfo_Manager::ChannelInfo[chn].StepTransationToStatus != STEPRUNNING)
    {
        capacityStatus[chn] = 0;    //reset
        return -1;
    }

    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx;
    switch(ChannelInfo_Manager::ChannelStep[chn].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
    {
    case CT_IVCH_CPULSE:	// more can be added here if required.
    case CT_IVCH_IR:
        capacityStatus[chn] = 0;    //reset  //zyx 171010
    case  CT_IVCH_ACIR:
        return -1;

    default:
        break;
    }

    if(capacityStatus[chn] == 0)    //????????
    {
        capacityStatus[chn] = 1;
        CommonFunc::GetTimeTicksUpdate(&m_capacity_Time[chn]);    //????
        return -1;
    }
    // No new data for regular request. Mandatory request is ignored here
    if((ChannelInfo_Manager::Isamp_NeedRecalculateSign[chn].m_Bits.CalCapacity != 1)
            && (ChannelInfo_Manager::Vsamp_NeedRecalculateSign[chn].m_Bits.CalCapacity != 1) //modified by hepeiqing,20150908
            && (requestType != 1) )
    {
        return -1;
    }

    return 1;
}

/* Additional // Important // Need Other People's Cooperation //

// 1. We need capacityStatus[chn] to be reset to 0 each time there's a Step ends.
// in case we can not reset it to 0 by updating due to task's running speed
// and then a false time will be used for next cap calculating
//
// and if possible, please also set capacityStatus[chn] to 1 when a step(other than setvariables, IR, Pulse)
// finishes transistion stage and start the running stage, also record the start time by
// MainTime_Process::recordCurTime(&m_capacity_Time[chn]);  then we don't lose any time need to be integration

// 2. We need each time a step ends, before ending,
// MetaVariable_Task::calculate_capacity(char requestType, char chn) needs to be
// called before ending the step with the parameter requestType value = 1. This
// will update capacity and engrgy, then a up to xx ms error can be avoided.*/



void MetaVariable_Task::Calculate_BuiltInAuxDXDT(unsigned char uc_Group,unsigned char uc_ch)    // all channels will be processed here in perticular order.
{
    St_DXDT_Parameter  st_dxdt_Parameter;
    BuiltInAuxDXDT_GetParameter(uc_Group,uc_ch,&st_dxdt_Parameter);
    double f_Deta = BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].aux_data - st_History[uc_Group][uc_ch].Value;
    double f_Time = CommonFunc::TimeCompare(st_History[uc_Group][uc_ch].Time,BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].value_update_time);
    if(((f_Time>st_dxdt_Parameter.Min_Time)&&(abs(f_Deta)>st_dxdt_Parameter.Precision))||(f_Time>st_dxdt_Parameter.Max_Time))
    {
        BuiltInAuxDXDT_Result(uc_Group,uc_ch,f_Deta,f_Time);
    }
}

void MetaVariable_Task::BuiltInAuxDXDT_GetParameter(unsigned char uc_Group,unsigned char uc_ch,St_DXDT_Parameter * st_dxdt_Parameter)    // all channels will be processed here in perticular order.
{
    if(BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].m_Chn_Info.m_AuxType == AUX_TEMPERATURE)
    {
        st_dxdt_Parameter->Max_Time = MAX_DXDT_T_TIME;
        st_dxdt_Parameter->Min_Time = MIN_DXDT_T_TIME;
        st_dxdt_Parameter->Precision = DXDT_T_PRECISION;
    }
    else
    {
        st_dxdt_Parameter->Max_Time = MAX_DXDT_V_TIME;
        st_dxdt_Parameter->Min_Time = MIN_DXDT_V_TIME;
        st_dxdt_Parameter->Precision = DXDT_V_PRECISION * 2 * \
                                       abs(BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
    }
}

void MetaVariable_Task::BuiltInAuxDXDT_Result(unsigned char uc_Group,unsigned char uc_ch,double f_Deta,double f_Time)
{
    unsigned char FloatType = FP_ZERO;
    double TempVal = 0;
    TempVal = f_Deta/f_Time;
    FloatType = fpclassify(TempVal);

    if((FloatType == FP_ZERO) || (FloatType == FP_NORMAL))
    {
        BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].aux_dx_dt = (float)TempVal;    //把dvdt的值存放到大数据体内  Y/1=X/t
        BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].aux_d2x_dt2 = (float)((TempVal - st_History[uc_Group][uc_ch].aux_dx_dt)/f_Time);
        st_History[uc_Group][uc_ch].aux_dx_dt = BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].aux_dx_dt;
    }
    st_History[uc_Group][uc_ch].Value = BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].aux_data;
    st_History[uc_Group][uc_ch].Time = BuiltIn_DataManager::m_BuiltInDataBank[uc_Group][uc_ch].value_update_time;
}
/* Additional // Important // Need Other People's Cooperation //

// 1. We need capacityStatus[chn] to be reset to 0 each time there's a Step ends.
// in case we can not reset it to 0 by updating due to task's running speed
// and then a false time will be used for next cap calculating
//
// and if possible, please also set capacityStatus[chn] to 1 when a step(other than setvariables, IR, Pulse)
// finishes transistion stage and start the running stage, also record the start time by
// OS_ServeProcess::recordCurTime(&m_capacity_Time[chn]);  then we don't lose any time need to be integration

// 2. We need each time a step ends, before ending,
// MetaVariable_Task::calculate_capacity(char requestType, char chn) needs to be
// called before ending the step with the parameter requestType value = 1. This
// will update capacity and engrgy, then a up to xx ms error can be avoided.*/



