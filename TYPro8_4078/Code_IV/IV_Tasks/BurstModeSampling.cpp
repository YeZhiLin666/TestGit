

#include "../Entry/includes.h"

//UInt16 BurstModeSampling::Priority = PRIORITY4;
//UInt16 BurstModeSampling::TaskID = SAMPLE_TASK;			// should be a definition which positioning this task in OS_Task[]


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
St_SIMPLELIST_QUEUE  BurstModeSampling::st_BurstSampleList[BURSTMODESAMPLING_MAX_CHANNEL];

unsigned long int BurstModeSampling::uw32_P_A_Log[BURSTMODESAMPLING_MAX_CHANNEL];    //  ��¼��һ������λ��
unsigned long int BurstModeSampling::uw32_P_B_Send[BURSTMODESAMPLING_MAX_CHANNEL];   //  �ϱ���һ������λ��
bool BurstModeSampling::uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];          //  ����BurstModeģʽtrue
bool BurstModeSampling::uc_B_DataFinish[BURSTMODESAMPLING_MAX_CHANNEL];         //  �ϱ��������true
MsTime BurstModeSampling::StartTime[BURSTMODESAMPLING_MAX_CHANNEL];             //  BurstMode��ʼʱ��
//     float BurstModeSampling::fStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
//     float BurstModeSampling::fStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
double BurstModeSampling::dStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
double BurstModeSampling::dStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];

bool BurstModeSampling::b_Logging[BURSTMODESAMPLING_MAX_CHANNEL];
bool BurstModeSampling::b_StepTimeReset[BURSTMODESAMPLING_MAX_CHANNEL];

unsigned short int BurstModeSampling::uw16_BurstModeDelayTime[BURSTMODESAMPLING_MAX_CHANNEL];      //  PC����BurstMode�ӳ�ʱ�� 1:100us ��λ��100us [��������0-10000֮��]
unsigned long int  BurstModeSampling::uw32_BurstModePointNum[BURSTMODESAMPLING_MAX_CHANNEL];       //  PC����BurstMode�������  [��������100-1000֮��]
unsigned long int  BurstModeSampling::uw32_BurstModeSampleSpeed[BURSTMODESAMPLING_MAX_CHANNEL];    //  PC����BurstMode����ٶ� 1:100us��¼һ�� n:n*100us��¼һ�� [��������1-10֮��]

//unsigned long int  BurstModeSampling::uw32_SendTotal[BURSTMODESAMPLING_MAX_CHANNEL];   //  ���ϱ���������
unsigned char BurstModeSampling::uc_Voltage_range[BURSTMODESAMPLING_MAX_CHANNEL];  // ��¼���ݵĵ�ѹ����
unsigned char BurstModeSampling::uc_Current_range[BURSTMODESAMPLING_MAX_CHANNEL];  // ��¼���ݵĵ�������
unsigned short int BurstModeSampling::uw16_IntervalCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // ������ʱ��
unsigned short int  BurstModeSampling::uw16_SendTimeCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // ���ͼ��ʱ��
bool  BurstModeSampling::bl_NeedRunning[BURSTMODESAMPLING_MAX_CHANNEL];    // ����������Ҫ����
bool  BurstModeSampling::bl_StepResume[BURSTMODESAMPLING_MAX_CHANNEL];     // ���ָ�����
bool  BurstModeSampling::bl_ChangedTime[BURSTMODESAMPLING_MAX_CHANNEL];    // ǿ�任ʱ���־
bool  BurstModeSampling::bl_Time;           // ʱ�䱸�ݱ�־
MsTime BurstModeSampling::OSTime_back;      // ϵͳʱ�䱸�ݱ�־

#pragma arm section //��������������



//��ʼ��
void BurstModeSampling::Init(void)
{
    for(unsigned char uc_i = 0; uc_i < BURSTMODESAMPLING_MAX_CHANNEL; uc_i++)
    {
        uw16_BurstModeDelayTime[uc_i] = 0;
        uw32_BurstModePointNum[uc_i] = 1000;
        uw32_BurstModeSampleSpeed[uc_i] = 1;  // PC����Ĭ��ֵ

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
//������
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
            if((QUEUE_DEPTH(st_BurstSampleList[ChannelIdx]) >= BURSTMODESAMPLING_MIN_SENDDATA)           // ���������
                    ||(uw16_SendTimeCounter[ChannelIdx] >= BURSTMODESAMPLING_MIN_SENDDATA)                   // ��ʱ�����
                    ||(!uc_A_BurstMode[ChannelIdx]&&((uw32_BurstModePointNum[ChannelIdx] - uw32_P_B_Send[ChannelIdx]) <= BURSTMODESAMPLING_MIN_SENDDATA)))   // ��β�����
            {
                uw16_SendTimeCounter[ChannelIdx] = 0;
                if(!IS_QUEUE_EMPTY(IVSDL::Buffer))
                    EthernetParse::MPReportData(0,SEND_LOG_SDL);    //���Ȱ��Ѿ���¼��ͨ���ݵ��ϴ����ᵼ�����ݴ�λ������BurstModePointû���ϴ��꣬���������µ���ͨ���ݵ�
                else if(BurstReport_LogSDL(ChannelIdx, &EthernetParse::m_pReport->m_Report_LogData_SDL))
                    EthernetParse::PushToSendingDataBuf(&EthernetParse::m_CmdTxBuffer_Async_Temp[0], EthernetParse::DataTx_Length); // ���Ͳ�������
            }
        }
    }
}

unsigned char BurstModeSampling::BurstReport_LogSDL(unsigned short ChannelIdx, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport) //mits8
{
    UWord16 uw16_Max = QUEUE_DEPTH(st_BurstSampleList[ChannelIdx])<<1;	  //  ���������еĵ���
    if(uw16_Max <= 0)
    {
        return 0;
    }
    UWord16 uw16_Count = sizeof(St_Data_Pairs_4BytesCycleIdx);//(St_SDL_Point_Pack);  //  ���㳤��:13�ֽ�;
    UWord16 uw16_Min = (MAX_CMD_LOGDATA_BUFFER/uw16_Count) & MAX_SDL_PAIRS_NUMBER;//108; // һ���Կ�ȡ�ĵ���
    Word16 w16_Remain = uw16_Max - uw16_Min;  //  ���㳤��
    if(w16_Remain > 0)
    {
        uw16_Max = uw16_Min;
    }

    pReport->m_dwCmd = CMD_REPORT_BASIC_TABLE_4BYTES_CYCLE_IDX;
    pReport->ItemCount = (unsigned char) uw16_Max;
#if SOFTWARE_DOG_ENABLE == 1
    pReport->m_u16Token = EthernetParse::m_u16Token + 1;
#endif
//	UWord16 uw16_Length = sizeof(MP_REPORT_LOGDATA_SDL) - (uw16_Count * MAX_AUX_SDL_POINT);   //  ���ֽ��� = ��ͷ����Ϣ;
    UWord16 uw16_Length = sizeof(MP_REPORT_SDL_4BYTESCYCLEIdx_PACK) - (uw16_Count * MAX_SDL_PAIRS_NUMBER);   //  ���ֽ��� = ��ͷ����Ϣ; MP_REPORT_LOGDATA_SDL
    uw16_Length += uw16_Count * uw16_Max;  //  ȡ�����ֽ���=���㳤��*��ȡ�ĵ���;

    unsigned char  bEmpty;
    st_SimpleData  St_ObjBuff; //st_SimpleData
    for(uint16 uw16_k = 0; uw16_k < uw16_Max; uw16_k++)//  /2
    {
        DEQUEUE(st_BurstSampleList[ChannelIdx],BURSTMODESAMPLING_MAX_BUFFER,St_ObjBuff,bEmpty);
        BurstPointChange(ChannelIdx,&St_ObjBuff, &pReport->Point[uw16_k]);//2*
    }

    EthernetParse::DataTx_Length = uw16_Length;// ����׼���������ݰ������ݳ���;
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

    //unsigned long long int uw64_Us100 = 0;	//uw16_BurstModeDelayTime[ChannelIdx];	// ��ʱ�޸ģ�delay timeΪ����������StartTime[ChannelIdx]
    unsigned long long int uw64_Us100 = uw16_BurstModeDelayTime[ChannelIdx];
    uw64_Us100 += uw32_P_B_Send[ChannelIdx]*uw32_BurstModeSampleSpeed[ChannelIdx];
    uw64_Us100 += StartTime[ChannelIdx].Us100;
    D->u16Us100 = uw64_Us100%10000;  // ����ʱ��
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

    D->un_data.fValue[0] = ChannelInfo_Manager::CalibrateCurrentADC(ChannelIdx,uc_Current_range[ChannelIdx],S->uw32_Current); // ����ת��
    D->un_data.fValue[1] = ChannelInfo_Manager::CalibrateVoltageADC(ChannelIdx,uc_Voltage_range[ChannelIdx],S->uw32_Voltage); // ����ת��
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


    if(++uw32_P_B_Send[ChannelIdx] >= uw32_BurstModePointNum[ChannelIdx]) // �涨��������
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
    if(!uc_A_BurstMode[ChannelIdx]) // δ����BurstModeSampling����
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
            if(++uw16_IntervalCounter[ChannelIdx] < uw32_BurstModeSampleSpeed[ChannelIdx])  //  �ٶ�δ��
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
    if(++uw32_P_A_Log[ChannelIdx] >= uw32_BurstModePointNum[ChannelIdx]) // �涨��������
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

    uc_Voltage_range[ChannelIdx] =  ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange;   //��õ�ǰrange
    uc_Current_range[ChannelIdx] =  ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucCurrentRange;   //��õ�ǰrange

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
    // �ǳ��Ͻ�һ��Ҫ��
    if((p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CRATE)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CRAMP)
            ||(p_ThisControl->m_u16ControlType == CT_IVCH_CSTAIRCASE))
    {
        if(!p_ThisControl->m_BurstModeSample)
            return;

        if(p_ThisControl->m_ucStepLimitNo == 1)  // �жϵ�ǰ������
        {
            for(unsigned char LimitIdx = 0;
                    LimitIdx < (p_ThisControl->m_ucStepLimitNo + p_ThisControl->m_ucLogLimitNo);
                    LimitIdx++)
            {
                MyLimit = &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[LimitIdx];
                if ((MyLimit->m_fLeftValue == MetaCode_PV_Steptime)
                        &&((MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN)||(MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO)||(MyLimit->m_ucCompareSign == MP_MCS_EQUAL_TO))
                        &&(MyLimit->m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE)
                        &&(MyLimit->m_fRightValue >= 5))  // Step����ֻ��һ��StepLimit����ͬʱ���㣺��ֵΪPV_step_time��Operator Ϊ>��>=����ֵΪ����5�ĳ�����
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

        if(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Ctrl.m_u16ControlType == CT_IVCH_REST) // �л�����һ��
        {
            if(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_StepID+1].m_Ctrl.m_ucStepLimitNo == 1)  // �ж���һ��
                if ((ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_fLeftValue == MetaCode_PV_Steptime)
                        &&((ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucCompareSign == MP_MCS_GREATER_THAN)||(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO)||(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_StepID+1].m_Limits[0].m_ucCompareSign == MP_MCS_EQUAL_TO))
                        &&(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE)
                        &&(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[u16_NextStepID].m_Limits[0].m_fRightValue >= 5))   // Step����ֻ��һ��StepLimit����ͬʱ���㣺��ֵΪPV_step_time��Operator Ϊ>��>=����ֵΪ����5�ĳ�����
                {
                    if(MyLimit->m_fRightValue < (float)2.2)
                        MyLimit->m_fRightValue = 2.2;  // ����ʱ��2.2s�������⴦����Ƴ���
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
//     double d_Delay = uw16_BurstModeDelayTime[ChannelIdx]*0.0001; // ��΢��ת����λΪ��
//     double d_Consume = d_Delay;
//     d_Consume += uw32_BurstModePointNum[ChannelIdx]*uw32_BurstModeSampleSpeed[ChannelIdx]*0.0001;// ����ʱ��
//
//     if(d_Consume > dSetValue)
//     {
//         // ����ָ���ʼ��
//         if(d_Delay < dSetValue)  // �ѹ���ʱʱ��
//         {
//             d_Consume = dSetValue - d_Delay;
//             UWord32 uw32_log = d_Consume/(uw32_BurstModeSampleSpeed[ChannelIdx]*0.0001); // ʱ��任������
//             uw32_P_A_Log[ChannelIdx] = uw32_log+1;   // ��λ���·�ʱ�����ȷ��1����,����һ�㿪ʼ
//             uw32_P_B_Send[ChannelIdx] = uw32_log+1;
//         }
//         else
//         {  // δ����ʱʱ��
//             uw32_P_A_Log[ChannelIdx] = 0;
//             uw32_P_B_Send[ChannelIdx] = 0;
//         }
//         BurstModeSampling::bl_StepResume[ChannelIdx] = true;
//     }
//     else
//         BurstModeSampling::bl_NeedRunning[ChannelIdx] = false;   // ��� BurstModeSampling ����
// }

void BurstModeSampling::BurstParameterCheck(unsigned short ChannelIdx)
{
    if(uw16_BurstModeDelayTime[ChannelIdx] > 10000)   // PC����Ĭ��ֵ����
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
