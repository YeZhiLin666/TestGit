

#include "../Entry/includes.h"

//UInt16 BurstModeSampling::Priority = PRIORITY4;
//UInt16 BurstModeSampling::TaskID = SAMPLE_TASK;			// should be a definition which positioning this task in OS_Task[]


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_SIMPLELIST_QUEUE  BurstModeSampling::st_BurstSampleList[BURSTMODESAMPLING_MAX_CHANNEL];

unsigned long int BurstModeSampling::uw32_P_A_Log[BURSTMODESAMPLING_MAX_CHANNEL];    //  记录下一个数据位置
unsigned long int BurstModeSampling::uw32_P_B_Send[BURSTMODESAMPLING_MAX_CHANNEL];   //  上报下一个数据位置
bool BurstModeSampling::uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];          //  进入BurstMode模式true
bool BurstModeSampling::uc_B_DataFinish[BURSTMODESAMPLING_MAX_CHANNEL];         //  上报数据完毕true
MsTime BurstModeSampling::StartTime[BURSTMODESAMPLING_MAX_CHANNEL];             //  BurstMode开始时间
//     float BurstModeSampling::fStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
//     float BurstModeSampling::fStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
double BurstModeSampling::dStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
double BurstModeSampling::dStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];

bool BurstModeSampling::b_Logging[BURSTMODESAMPLING_MAX_CHANNEL];
bool BurstModeSampling::b_StepTimeReset[BURSTMODESAMPLING_MAX_CHANNEL];

unsigned short int BurstModeSampling::uw16_BurstModeDelayTime[BURSTMODESAMPLING_MAX_CHANNEL];      //  PC下载BurstMode延迟时间 1:100us 单位：100us [允许输入0-10000之间]
unsigned long int  BurstModeSampling::uw32_BurstModePointNum[BURSTMODESAMPLING_MAX_CHANNEL];       //  PC下载BurstMode打点总数  [允许输入100-1000之间]
unsigned long int  BurstModeSampling::uw32_BurstModeSampleSpeed[BURSTMODESAMPLING_MAX_CHANNEL];    //  PC下载BurstMode打点速度 1:100us记录一点 n:n*100us记录一点 [允许输入1-10之间]

//unsigned long int  BurstModeSampling::uw32_SendTotal[BURSTMODESAMPLING_MAX_CHANNEL];   //  已上报数据总量
unsigned char BurstModeSampling::uc_Voltage_range[BURSTMODESAMPLING_MAX_CHANNEL];  // 记录数据的电压量程
unsigned char BurstModeSampling::uc_Current_range[BURSTMODESAMPLING_MAX_CHANNEL];  // 记录数据的电流量程
unsigned short int BurstModeSampling::uw16_IntervalCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // 计算间隔时间
unsigned short int  BurstModeSampling::uw16_SendTimeCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // 发送间隔时间
bool  BurstModeSampling::bl_NeedRunning[BURSTMODESAMPLING_MAX_CHANNEL];    // 条件合适需要运行
bool  BurstModeSampling::bl_StepResume[BURSTMODESAMPLING_MAX_CHANNEL];     // 步恢复运行
bool  BurstModeSampling::bl_ChangedTime[BURSTMODESAMPLING_MAX_CHANNEL];    // 强变换时间标志
bool  BurstModeSampling::bl_Time;           // 时间备份标志
MsTime BurstModeSampling::OSTime_back;      // 系统时间备份标志

#pragma arm section //结束外存变量定义



//初始化
void BurstModeSampling::Init(void)
{
    for(unsigned char uc_i = 0; uc_i < BURSTMODESAMPLING_MAX_CHANNEL; uc_i++)
    {
        uw16_BurstModeDelayTime[uc_i] = 0;
        uw32_BurstModePointNum[uc_i] = 1000;
        uw32_BurstModeSampleSpeed[uc_i] = 1;  // PC下载默认值

        INIT_QUEUE(st_BurstSampleList[uc_i]);
        uc_A_BurstMode[uc_i] = false;
        uc_B_DataFinish[uc_i] = false;
        bl_NeedRunning[uc_i] = false;
        bl_StepResume[uc_i] = false;
        // StartTime[uc_i] = OS_ServeProcess::OS_Time;
        CommonFunc::GetTimeTicksUpdate(&StartTime[uc_i]);
        dStartTestTime[uc_i] = 0;
        dStartStepTime[uc_i] = 0;

        uw16_IntervalCounter[uc_i] = 0;
        uw32_P_A_Log[uc_i] = 0;
        uw32_P_B_Send[uc_i] = 0;
        uw16_SendTimeCounter[uc_i] = 0;
        bl_ChangedTime[uc_i] = false;
        b_Logging[uc_i] = false;
        b_StepTimeReset[uc_i] = false;
    }
    bl_Time = false;
    OSTime_back = CommonFunc::GetSystemTime();//OS_Time;

}
/*
//主任务
void BurstModeSampling::DoWork(void)
{


}


void BurstModeSampling::EndTask(void)
{

}
*/

void BurstModeSampling::ReportSimpleData(void)
{
    for(unsigned char ChannelIdx = 0; ChannelIdx < BURSTMODESAMPLING_MAX_CHANNEL; ChannelIdx++)
    {
        if(uc_A_BurstMode[ChannelIdx])
            return;

//         if(!b_StepTimeReset[ChannelIdx])//1805281 zyx
//             return;
        // report data
        if(!uc_B_DataFinish[ChannelIdx])
        {
            uw16_SendTimeCounter[ChannelIdx] ++;
            if((QUEUE_DEPTH(st_BurstSampleList[ChannelIdx]) >= BURSTMODESAMPLING_MIN_SENDDATA)           // 以数量组包
                    ||(uw16_SendTimeCounter[ChannelIdx] >= BURSTMODESAMPLING_MIN_SENDDATA)                   // 以时间组包
                    ||(!uc_A_BurstMode[ChannelIdx]&&((uw32_BurstModePointNum[ChannelIdx] - uw32_P_B_Send[ChannelIdx]) <= BURSTMODESAMPLING_MIN_SENDDATA)))   // 以尾包组包
            {
                uw16_SendTimeCounter[ChannelIdx] = 0;
                if(!IS_QUEUE_EMPTY(IVSDL::Buffer))
                    EthernetParse::MPReportData(0,SEND_LOG_SDL);    //不先把已经记录普通数据点上传，会导致数据错位。由于BurstModePoint没有上传完，不会引入新的普通数据点
                else if(BurstReport_LogSDL(ChannelIdx, &EthernetParse::m_pReport->m_Report_LogData_SDL))
                    EthernetParse::PushToSendingDataBuf(&EthernetParse::m_CmdTxBuffer_Async_Temp[0], EthernetParse::DataTx_Length); // 发送测试数据
            }
        }
    }
}

unsigned char BurstModeSampling::BurstReport_LogSDL(unsigned short ChannelIdx, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport) //mits8
{
    UWord16 uw16_Max = QUEUE_DEPTH(st_BurstSampleList[ChannelIdx])<<1;	  //  队列中已有的点数
    if(uw16_Max <= 0)
    {
        return 0;
    }
    UWord16 uw16_Count = sizeof(St_Data_Pairs_4BytesCycleIdx);//(St_SDL_Point_Pack);  //  单点长度:13字节;
    UWord16 uw16_Min = (MAX_CMD_LOGDATA_BUFFER/uw16_Count) & MAX_SDL_PAIRS_NUMBER;//108; // 一次性可取的点数
    Word16 w16_Remain = uw16_Max - uw16_Min;  //  单点长度
    if(w16_Remain > 0)
    {
        uw16_Max = uw16_Min;
    }

    pReport->m_dwCmd = CMD_REPORT_BASIC_TABLE_4BYTES_CYCLE_IDX;
    pReport->ItemCount = (unsigned char) uw16_Max;
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = EthernetParse::m_u16Token + 1;
#endif
//	UWord16 uw16_Length = sizeof(MP_REPORT_LOGDATA_SDL) - (uw16_Count * MAX_AUX_SDL_POINT);   //  总字节数 = 包头包信息;
    UWord16 uw16_Length = sizeof(MP_REPORT_SDL_4BYTESCYCLEIdx_PACK) - (uw16_Count * MAX_SDL_PAIRS_NUMBER);   //  总字节数 = 包头包信息; MP_REPORT_LOGDATA_SDL
    uw16_Length += uw16_Count * uw16_Max;  //  取点总字节数=单点长度*已取的点数;

    unsigned char  bEmpty;
    st_SimpleData  St_ObjBuff; //st_SimpleData
    for(uint16 uw16_k = 0; uw16_k < uw16_Max; uw16_k++)//  /2
    {
        DEQUEUE(st_BurstSampleList[ChannelIdx],BURSTMODESAMPLING_MAX_BUFFER,St_ObjBuff,bEmpty);
        BurstPointChange(ChannelIdx,&St_ObjBuff, &pReport->Point[uw16_k]);//2*
    }

    EthernetParse::DataTx_Length = uw16_Length;// 保存准备发送数据包的数据长度;
    EthernetParse::HeadFieldSaveBuff(EthernetParse::m_CmdTxBuffer_Async_Temp, &EthernetParse::DataTx_Length, SEND_LOG_SDL);
    return 1;
}

void BurstModeSampling::BurstPointChange(unsigned short ChannelIdx,st_SimpleData * S, St_Data_Pairs_4BytesCycleIdx * D) //mits8  //1805141
{
//     St_Data_Pairs * back = D;

    D->u32TestID = Schedule_Manager_task::m_dwTestID[ChannelIdx];
    if(ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue > 0)
        D->u32Cycle_Idx = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_CycleIndex].m_fValue - 1;
    D->u16Step_Idx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_RunningStepIdx;

    //unsigned long long int uw64_Us100 = 0;	//uw16_BurstModeDelayTime[ChannelIdx];	// 临时修改，delay time为负，已算入StartTime[ChannelIdx]
    unsigned long long int uw64_Us100 = uw16_BurstModeDelayTime[ChannelIdx];
    uw64_Us100 += uw32_P_B_Send[ChannelIdx]*uw32_BurstModeSampleSpeed[ChannelIdx];
    uw64_Us100 += StartTime[ChannelIdx].Us100;
    D->u16Us100 = uw64_Us100%10000;  // 计算时间
    D->u32Second = StartTime[ChannelIdx].Second + uw64_Us100/10000;

    MsTime EndTime;
    EndTime.Second = D->u32Second;
    EndTime.Us100 = D->u16Us100;
    D->dTestTime = dStartTestTime[ChannelIdx] + CommonFunc::TimeCompare(StartTime[ChannelIdx], EndTime);//+ ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_TestTime);
    D->dStepTime = CommonFunc::TimeCompare(StartTime[ChannelIdx], EndTime);//fStartStepTime[ChannelIdx] + //D->fTestTime;//uw32_P_B_Send[ChannelIdx]*uw32_BurstModeSampleSpeed[ChannelIdx] + uw16_BurstModeDelayTime[ChannelIdx];//S->StepTime;//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Steptime);

//     if(D->u32Second == 0)
//         int i = 0;
    D->ucType = SDL_DATA_FOR_LOG;//display = 1;idle = 2;log = 0;
    D->bIV = 1;
    D->ucSensor = 0;
    D->ucDataLogFormat = SDL_DATA_TYPE_LOG_C_V_CAP_ENG;
    D->ucChannelIdx =  ChannelIdx;

    ChannelInfo_Manager::SetDataFlag(ChannelIdx, BURSTMODE_POINT );
    D->u32DataBitFlag = ChannelInfo_Manager::m_DataFlag[ChannelIdx];//1804251 zyx //ul_SpecialPoint;
    ChannelInfo_Manager::ClearDataFlag(ChannelIdx);

    D->un_data.fValue[0] = ChannelInfo_Manager::CalibrateCurrentADC(ChannelIdx,uc_Current_range[ChannelIdx],S->uw32_Current); // 码字转换
    D->un_data.fValue[1] = ChannelInfo_Manager::CalibrateVoltageADC(ChannelIdx,uc_Voltage_range[ChannelIdx],S->uw32_Voltage); // 码字转换
    D->un_data.fValue[2] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeCapacity);
    D->un_data.fValue[3] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeCapacity);
    D->un_data.fValue[4] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_ChargeEnergy); // uw32_ChargeEnergy++;//
    D->un_data.fValue[5] = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_DischargeEnergy); // uw32_ChargeEnergy;//
    D->ucDataCount = 6;
    D->u32DataPoint = ++ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_DataPoints].m_Value.m_uInt32;//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_DataPoints);//180516 zyx//IVSDL::m_dwDataPoint[ChannelIdx];
//    ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_DataPoints, D->u32DataPoint + 1); //mits8 180516 zyx  //IVSDL::m_dwDataPoint[ChannelIdx]++;


//     D->fValue =  uw32_P_B_Send[ChannelIdx]+1;// Test11



//     D++;
//     *D = *back;
//     D->ucValueType = MetaCode_PV_Current;


//     D->fValue =  uw32_P_B_Send[ChannelIdx]+1;// Test12

//     if(uw32_P_B_Send[ChannelIdx] == 0)
//     {
//         uw32_FinishA[uc_put] = uw64_Us100; // Test13
//         uw32_FinishB[uc_put] = D->u32Second;
//         if(++uc_put >= 200)
//             uc_put = 0;
//     }


    if(++uw32_P_B_Send[ChannelIdx] >= uw32_BurstModePointNum[ChannelIdx]) // 规定数量到达
    {
        uc_B_DataFinish[ChannelIdx] = true;
        b_Logging[ChannelIdx] = false;
        float fTestTime = D->dTestTime;//fStartTestTime[ChannelIdx] +
        float fStepTime = D->dStepTime;//fStartStepTime[ChannelIdx] +
        ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_TestTime,fTestTime);
        ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_Steptime,fStepTime);
    }
}

void BurstModeSampling::BurstSavePoint(unsigned short ChannelIdx)
{
    if(!uc_A_BurstMode[ChannelIdx]) // 未启动BurstModeSampling功能
        return;

    if((uw32_P_A_Log[ChannelIdx] == 0)||(bl_StepResume[ChannelIdx]))
    {
        if(CommonFunc::GetTimeTicks(&StartTime[ChannelIdx]) < uw16_BurstModeDelayTime[ChannelIdx])
            return;
//         else
//         {
//             b_Logging[ChannelIdx] = true;
// //             if(	ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_bResetTestTime)
// //             {
// //                 ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_TestTime,0);
// //                 ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_bResetTestTime = false;
// //             }
// //             ChannelInfo_Manager::Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_Steptime,0);

//         }
    }

    unsigned char uc_updata = false;
    if(uw32_P_A_Log[ChannelIdx] == 0)
        uc_updata = true;
    else
    {
        if(bl_StepResume[ChannelIdx])
        {
            uc_updata = true;
        }
        else
        {
            if(++uw16_IntervalCounter[ChannelIdx] < uw32_BurstModeSampleSpeed[ChannelIdx])  //  速度未到
                return;
        }
    }
    uw16_IntervalCounter[ChannelIdx] = 0;


    if(uc_updata && bl_StepResume[ChannelIdx])
    {
        bl_StepResume[ChannelIdx] = false;
    }
//     }

    st_SimpleData  St_ObjBuff;
    St_ObjBuff.uw32_Voltage = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32VoltageAdc;
    St_ObjBuff.uw32_Current = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32CurrentAdc;
//     St_ObjBuff.TestTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_TestTime);
//     St_ObjBuff.StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, MetaCode_PV_Steptime);
    ENQUEUE(st_BurstSampleList[ChannelIdx],BURSTMODESAMPLING_MAX_BUFFER,St_ObjBuff);
    if(++uw32_P_A_Log[ChannelIdx] >= uw32_BurstModePointNum[ChannelIdx]) // 规定数量到达
    {
        CloseBurstModeSampling(ChannelIdx);
    }
}

void BurstModeSampling::OpenBurstModeSampling(unsigned short ChannelIdx)
{
    CommonFunc::GetTimeTicksUpdate(&StartTime[ChannelIdx]);
    // StartTime[ChannelIdx] = OS_ServeProcess::OS_Time;
    dStartTestTime[ChannelIdx] = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx,MetaCode_PV_TestTime,StartTime[ChannelIdx]);//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_TestTime);
    dStartStepTime[ChannelIdx] = ChannelInfo_Manager::Read_DoubleTime(ChannelIdx,MetaCode_PV_Steptime,StartTime[ChannelIdx]);//ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx,MetaCode_PV_Steptime);
    uc_A_BurstMode[ChannelIdx] = true;
    uw16_IntervalCounter[ChannelIdx] = 0;
    uw16_SendTimeCounter[ChannelIdx] = 0;
    INIT_QUEUE(st_BurstSampleList[ChannelIdx]);
    uc_B_DataFinish[ChannelIdx] = false;
    b_StepTimeReset[ChannelIdx] = false;//1805281 zyx
    if(!bl_StepResume[ChannelIdx])
    {
        uw32_P_A_Log[ChannelIdx] = 0;
        uw32_P_B_Send[ChannelIdx] = 0;
    }
    b_Logging[ChannelIdx] = true;
    BurstModeSampling::BurstParameterCheck(ChannelIdx);
}

void BurstModeSampling::CloseBurstModeSampling(unsigned short ChannelIdx)
{
    uc_A_BurstMode[ChannelIdx] = false;
    bl_NeedRunning[ChannelIdx] = false;
    bl_StepResume[ChannelIdx] = false;

    uc_Voltage_range[ChannelIdx] =  ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange;   //获得当前range
    uc_Current_range[ChannelIdx] =  ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange;   //获得当前range

//    b_Logging[ChannelIdx] = false;
//     ChannelIdx[ChannelIdx] = false;
    b_StepTimeReset[ChannelIdx] = false;
}

void BurstModeSampling::BurstModeSamplingCheck(unsigned short ChannelIdx,unsigned short u16_StepID)
{
    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
//         case BOARDTYPE_BT2204:
    case BOARDTYPE_LBT22043:
    case BOARDTYPE_LBT22013:
    case BOARDTYPE_LBT22023:
//         case BOARDTYPE_LBT22013FR:
//         case BOARDTYPE_LBT22043:
        break;

    default:
        return; //break;
    }

    signed char    sc_SchBufIdx  = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return;		

    unsigned char  NonActiveStep = !ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx;

    St_StepCtrl * p_ThisControl    =    &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl;
    St_ChannelStep * p_ThisStep    =    &ChannelInfo_Manager::ChannelStep[ChannelIdx];
// 	St_ChannelInfo * p_ThisInfo    =    &ChannelInfo_Manager::ChannelInfo[ChannelIdx];
    St_Limit * MyLimit;
    unsigned short u16_NextStepID = u16_StepID+1;

    BurstModeSampling::bl_NeedRunning[ChannelIdx] = false;
    // 非常严紧一步要求
    if((p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CRATE)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CRAMP)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CSTAIRCASE))
    {
        if(!p_ThisControl->m_BurstModeSample)
            return;

        if(p_ThisControl->m_ucStepLimitNo == 1)  // 判断当前步条件
        {
            for(unsigned char LimitIdx = 0;
                    LimitIdx < (p_ThisControl->m_ucStepLimitNo + p_ThisControl->m_ucLogLimitNo);
                    LimitIdx++)
            {
                MyLimit = &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[LimitIdx];
                if ((MyLimit->m_fLeftValue == MetaCode_PV_Steptime)
                        &&((MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN)||(MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO)||(MyLimit->m_ucCompareSign == MP_MCS_EQUAL_TO))
                        &&(MyLimit->m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE)
                        &&(MyLimit->m_fRightValue >= 5))  // Step有且只有一个StepLimit，且同时满足：左值为PV_step_time，Operator 为>或>=，右值为大于5的常数。
                {
                    if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fResumeStepTime <= (float)0.0)
                    {
                        BurstModeSampling::bl_NeedRunning[ChannelIdx] = true;
                        BurstModeSampling::bl_ChangedTime[ChannelIdx] = true;
                    }
                    return;
                }
            }
        }

        MyLimit = &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[0];
        if(MyLimit->m_u16GotoStepID == MCU_GOTOSTOPSTEP )
            return;

        if(MyLimit->m_u16GotoStepID < MCU_GOTOSTOPSTEP)
        {
            u16_NextStepID = MyLimit->m_u16GotoStepID;
        }

        if(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Ctrl.m_u16ControlType == CT_IVCH_REST) // 切换成下一步
        {
            if(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_StepID+1].m_Ctrl.m_ucStepLimitNo == 1)  // 判断下一步
                if ((ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_fLeftValue == MetaCode_PV_Steptime)
                        &&((ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucCompareSign == MP_MCS_GREATER_THAN)||(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO)||(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_StepID+1].m_Limits[0].m_ucCompareSign == MP_MCS_EQUAL_TO))
                        &&(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE)
                        &&(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_fRightValue >= 5))   // Step有且只有一个StepLimit，且同时满足：左值为PV_step_time，Operator 为>或>=，右值为大于5的常数。
                {
                    if(MyLimit->m_fRightValue < (float)2.2)
                        MyLimit->m_fRightValue = 2.2;  // 设置时间2.2s保护避免处理机制出错。
                    if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fResumeStepTime <= (float)0.0)
                    {
                        BurstModeSampling::bl_NeedRunning[ChannelIdx] = true;
                        BurstModeSampling::bl_ChangedTime[ChannelIdx] = true;
                    }
                    return;
                }
        }

    }

}

// void BurstModeSampling::BurstModeSamplingResume(unsigned short ChannelIdx, double dSetValue)
// {
//     BurstModeSampling::BurstParameterCheck(ChannelIdx);
//
//     double d_Delay = uw16_BurstModeDelayTime[ChannelIdx]*0.0001; // 百微秒转化单位为秒
//     double d_Consume = d_Delay;
//     d_Consume += uw32_BurstModePointNum[ChannelIdx]*uw32_BurstModeSampleSpeed[ChannelIdx]*0.0001;// 计算时间
//
//     if(d_Consume > dSetValue)
//     {
//         // 计算恢复开始点
//         if(d_Delay < dSetValue)  // 已过延时时间
//         {
//             d_Consume = dSetValue - d_Delay;
//             UWord32 uw32_log = d_Consume/(uw32_BurstModeSampleSpeed[ChannelIdx]*0.0001); // 时间变换成整数
//             uw32_P_A_Log[ChannelIdx] = uw32_log+1;   // 上位机下发时间仅精确到1毫秒,以下一点开始
//             uw32_P_B_Send[ChannelIdx] = uw32_log+1;
//         }
//         else
//         {  // 未过延时时间
//             uw32_P_A_Log[ChannelIdx] = 0;
//             uw32_P_B_Send[ChannelIdx] = 0;
//         }
//         BurstModeSampling::bl_StepResume[ChannelIdx] = true;
//     }
//     else
//         BurstModeSampling::bl_NeedRunning[ChannelIdx] = false;   // 清除 BurstModeSampling 功能
// }

void BurstModeSampling::BurstParameterCheck(unsigned short ChannelIdx)
{
    if(uw16_BurstModeDelayTime[ChannelIdx] > 10000)   // PC下载默认值保护
        uw16_BurstModeDelayTime[ChannelIdx] = 10000;

    if(uw32_BurstModePointNum[ChannelIdx] < 100)
        uw32_BurstModePointNum[ChannelIdx] = 100;
    else if(uw32_BurstModePointNum[ChannelIdx] > 1000)
        uw32_BurstModePointNum[ChannelIdx] = 1000;

    if(uw32_BurstModeSampleSpeed[ChannelIdx] < 1)
        uw32_BurstModeSampleSpeed[ChannelIdx] = 1;
    else if(uw32_BurstModeSampleSpeed[ChannelIdx] > 10)
        uw32_BurstModeSampleSpeed[ChannelIdx] = 10;

}
