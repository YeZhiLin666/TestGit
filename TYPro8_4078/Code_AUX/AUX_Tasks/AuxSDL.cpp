#include "../Entry/includes.h"

//public:
unsigned short               AuxSDL::Priority = PRIORITY4;
unsigned short               AuxSDL::TaskID = SDL_TASK;

//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

unsigned char                AuxSDL::m_ScanStartIVUnitBufIdx;
unsigned char                AuxSDL::m_ScanStartIVIdx[MAX_IVMCU_PER_SYS];    //SDL任务记录首次开始扫描IV号。
unsigned char                AuxSDL::m_ScanStartTypeIdx[MAX_IVMCU_PER_SYS];    //SDL任务记录首次开始扫描辅助类型号。
unsigned char                AuxSDL::m_CurrentIVUnitBufIdx;
unsigned char                AuxSDL::m_CurrentIVIdx[MAX_IVMCU_PER_SYS];
unsigned char                AuxSDL::m_CurrentTypeIdx[MAX_IVMCU_PER_SYS];
unsigned char                AuxSDL::m_CurrentAuxChIdx[MAX_IVMCU_PER_SYS];
//St_SDL_DeltaValue            AuxSDL::m_DeltaValue[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE][MAX_AUXCH_PER_BOARD];
St_SDL_LastValue             AuxSDL::m_LastValue[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_CH_IN_AUX_UNIT];
St_SDL_Info                  AuxSDL::m_Info[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU]; //将MAX_BOARD_PER_AUXMCU改为MAX_AUX_TYPE 20160714 zyx

#pragma arm section //结束外存变量定义 



////////
//    Init(void)
//    : SDL模块初始化处理函数
// Input:  N/A
// Return: N/A
///////
void AuxSDL::Init(void)
{
    int IvIdx,TypeIdx,AuxchIdx;
    float RangeH,RangeL;
    unsigned short int uw16_AuxChennal = 0;

    unsigned char  uc_ScanStartFlag[MAX_IVMCU_PER_SYS];
    m_CurrentIVUnitBufIdx = 0;
    do {
        m_CurrentIVIdx[m_CurrentIVUnitBufIdx] = 0;
        m_CurrentTypeIdx[m_CurrentIVUnitBufIdx] = 0;
        m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx] = 0;
        uc_ScanStartFlag[m_CurrentIVUnitBufIdx] = 0;

        for(IvIdx = 0; IvIdx < MAX_IVCH_PER_IVMCU; IvIdx++)
        {
            for(TypeIdx = 0; TypeIdx < MAX_AUX_TYPE; TypeIdx++)
            {
                // 			if(Aux_DataManager::m_Iv2Aux_Map[IvIdx][TypeIdx].m_chn_count == 0)

                unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(TypeIdx);
                if(TempAuxTypeIdex == 0xFF)//辅助限制
                    continue ;

                if(Aux_DataManager::m_MapFromPC[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_ActualCounts == 0)
                    continue;  // break;   // 因为上位机下时类型板已经按填定情况，2Vd，T,...如果2VD没有也会空着。

                // 大数据体中保存的
                Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_sdl_alive = 0;
                Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_aux_type = TypeIdx;

                unsigned char uc_i=0;
                /* for( uc_i=0; uc_i < MAX_BOARD_PER_AUXMCU; uc_i ++)
                {
                if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_BType.m_AuxType == TypeIdx)
                {
                for(unsigned char uc_j = 0; uc_j < uc_i ;uc_j ++)
                {
                uw16_AuxChennal += Aux_DataManager::m_AllAuxBoards[uc_j].m_abi_globe.m_Total_Channels;  // 扫出一字列表中的通道参数
                }
                break;
                }
                }*/  //暂时屏蔽 qjm 20150603
                switch(TypeIdx)  // 用于判定联合体
                {
                case AUX_VOLTAGE:
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate = SDL_LOGDATA_2VD_DELTA; // SDL默认：变化量值1记录一点给PC
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_log_interval = SDL_LOGDATA_2VD_INTERVAL;  // SDL默认：T 5秒，2VD 100毫秒记录一点给PC
                    //RangeH = Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uw16_AuxChennal].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;  // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。
                    //RangeL = Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uw16_AuxChennal].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
                    //RangeH = 5; // 暂时锁着量程 ？？？20131217
                    //RangeL = -5; // 暂时锁着量程 ？？？20131217
                    //                         m_Info[m_CurrentIVUnitBufIdx][IvIdx][TypeIdx].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[0][IvIdx][TypeIdx].m_fLogDeltaRate * AuxSDL::GetFullValue( RangeH, RangeL); // 因为大数据体为两维管理SDL数，仅取第一通道量程范围?
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].fLogDeltaValue =Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate;
                    break;
                case AUX_PRESSURE:
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate = SDL_LOGDATA_PRESSURE_DELTA; // SDL默认：变化量值1记录一点给PC
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_log_interval = SDL_LOGDATA_PRESSURE_INTERVAL;  // SDL默认：
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].fLogDeltaValue =Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate;
                    break;
                case AUX_TEMPERATURE:
                case AUX_EXTERNALCHARGE:
                default:
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate = SDL_LOGDATA_T_DELTA; // SDL默认：变化量值1记录一点给PC
                    Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_log_interval = SDL_LOGDATA_T_INTERVAL;  // SDL默认：T 5秒，2VD 100毫秒记录一点给PC
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_fLogDeltaRate; // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。
                    break;

                    //default: break;

                }

                m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].bLogPcData = LOGSWITCH_OFF;
                for (AuxchIdx = 0; AuxchIdx < MAX_CH_IN_AUX_UNIT; AuxchIdx++)
                {
                    CommonFunc::GetTimeTicksUpdate( &m_LastValue[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex][AuxchIdx].LogTime);
                    CommonFunc::GetTimeTicksUpdate( &m_LastValue[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex][AuxchIdx].DisplayLogTime);
                    CommonFunc::GetTimeTicksUpdate( &m_LastValue[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex][AuxchIdx].IdlLogTime);
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].NeedValue[AuxchIdx].bReportIvData = LOGSWITCH_OFF;
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].NeedValue[AuxchIdx].ReportDataTypebit = 0;
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].NeedValue[AuxchIdx].ReportTime_DXDT = CommonFunc::GetSystemTime();
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].NeedValue[AuxchIdx].MCUId = 0;
                    m_Info[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].NeedValue[AuxchIdx].ReportInterval = Aux_DataManager::m_AuxSDLBank[m_CurrentIVUnitBufIdx][IvIdx][TempAuxTypeIdex].m_log_interval;  // SDL默认：T 5秒，2VD 100毫秒记录一点给IV
                }

                if(uc_ScanStartFlag == 0)
                {
                    uc_ScanStartFlag[m_CurrentIVUnitBufIdx] = 1;
                    m_ScanStartIVUnitBufIdx = m_CurrentIVUnitBufIdx;
                    m_ScanStartIVIdx[m_CurrentIVUnitBufIdx] = IvIdx;
                    m_ScanStartTypeIdx[m_CurrentIVUnitBufIdx] = TypeIdx;
                }
            }
        }
    } while(MYIVUNITINDEX(m_CurrentIVUnitBufIdx));
}

////////
//    DoWork(void)
//    : SDL模块运行工作处理函数
// Input:  N/A
// Return: N/A
///////
void AuxSDL::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SDL] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_SDL);
#endif

    unsigned char uc_breakFlag = 0;
    unsigned char uc_breakCount = 0;
    do {
        if(m_CurrentIVUnitBufIdx == 0)
            m_CurrentIVUnitBufIdx = m_ScanStartIVUnitBufIdx;

        for( ; m_CurrentIVIdx[m_CurrentIVUnitBufIdx] < MAX_IVCH_PER_IVMCU; m_CurrentIVIdx[m_CurrentIVUnitBufIdx]++ ) // 扫IV带的AUX
        {
            if (Aux_DataManager::m_IVMCU_ID_Info[m_CurrentIVUnitBufIdx].m_Valid == 0)   //对没有关联的IV unit 跳过，适合于专属辅助unit的情况。只有一个IV MCU
                continue;
            for( ; m_CurrentTypeIdx[m_CurrentIVUnitBufIdx] < MAX_AUX_TYPE; m_CurrentTypeIdx[m_CurrentIVUnitBufIdx]++ ) // 扫AUX的类型
            {
                unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(m_CurrentTypeIdx[m_CurrentIVUnitBufIdx]);
                if(TempAuxTypeIdex == 0xFF)//辅助限制
                    continue ;

                if(Aux_DataManager::m_MapFromPC[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex].m_ActualCounts == 0)
                {
                    m_CurrentTypeIdx[m_CurrentIVUnitBufIdx]++;
#if DEBUG_TASK_STATISTIC == 1
                    goto GotoTestTime;
#else
                    return;
#endif
                }

                for( ; m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx] < Aux_DataManager::m_MapFromPC[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex].m_ActualCounts; m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]++) // 扫整个PC下发的Map列表
                {
                    UWord32 uw32_temp = CommonFunc::GetTimeTicks(&m_LastValue[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex][m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]].IdlLogTime);
                    if(uw32_temp >= IDL_LOGDATA_INTERVAL)
                    {
                        CommonFunc::GetTimeTicksUpdate(&m_LastValue[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex][m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]].IdlLogTime);
                        TriggerLog_Net(m_CurrentIVUnitBufIdx, m_CurrentIVIdx[m_CurrentIVUnitBufIdx], m_CurrentTypeIdx[m_CurrentIVUnitBufIdx], m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx], SDL_DATA_FOR_IDLE);
                    }

                    // IV发送方向
                    if(m_Info[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex].NeedValue[m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]].bReportIvData)
                        Check_ReportValue(m_CurrentIVUnitBufIdx, m_CurrentIVIdx[m_CurrentIVUnitBufIdx],m_CurrentTypeIdx[m_CurrentIVUnitBufIdx],m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]);

                    if(++uc_breakCount >= SDL_TASK_CHANNEL_COUNT)
                    {
                        uc_breakFlag = 1; // 分时时间已经用完，下次再做
                        m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]++;
                        break;
                    }
                }

                if(uc_breakFlag)
                {
#if DEBUG_TASK_STATISTIC == 1
                    goto GotoTestTime;
#else
                    return;
#endif
                }
                else
                    m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx] = 0;  // 退出超过AUX类型下的总通道数，则要重新开始。
            }
            m_CurrentTypeIdx[m_CurrentIVUnitBufIdx] = 0;  // 退出超过AUX类型的总数，则要重新开始。
        }
        m_CurrentIVIdx[m_CurrentIVUnitBufIdx] = m_ScanStartIVIdx[m_CurrentIVUnitBufIdx];  // 退出超过IV的总数，则要重新开始。
    } while(MYIVUNITINDEX(m_CurrentIVUnitBufIdx));
    //AuxSDL::SendDisplayAuxData();

#if DEBUG_TASK_STATISTIC == 1
GotoTestTime:
    OS_ServeProcess::Task_Count_Time[COUNT_SDL] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_SDL);
#endif
}
/*
MsTime LastDisplayAuxTime = {0};
void AuxSDL::SendDisplayAuxData()
{
    // PC发送方向
    //UWord32 uw32_temp = CommonFunc::GetTimeTicks(&m_LastValue[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex][m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]].IdlLogTime);
    UWord32 uw32_temp = CommonFunc::GetTimeTicks(&LastDisplayAuxTime);
    if(uw32_temp >= IDL_LOGDATA_INTERVAL)
    {
        //CommonFunc::GetTimeTicksUpdate(&m_LastValue[m_CurrentIVUnitBufIdx][m_CurrentIVIdx[m_CurrentIVUnitBufIdx]][TempAuxTypeIdex][m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx]].IdlLogTime);
        //TriggerLog_Net(m_CurrentIVUnitBufIdx, m_CurrentIVIdx[m_CurrentIVUnitBufIdx], m_CurrentTypeIdx[m_CurrentIVUnitBufIdx], m_CurrentAuxChIdx[m_CurrentIVUnitBufIdx], SDL_DATA_FOR_IDLE);
        CommonFunc::GetTimeTicksUpdate(&LastDisplayAuxTime);
        MsTime SysTime = CommonFunc::GetSystemTime();
        St_SDL_Point_Pack MyPoint = {0};
        MyPoint.ucType = SDL_DATA_FOR_IDLE;
        MyPoint.u32Second = SysTime.Second;
        MyPoint.u16Us100 =  SysTime.Us100;
        MyPoint.bIV = 0;

        for(unsigned char AuxType = 0; AuxType < MAX_AUX_TYPE; AuxType++ )
        {
            if(Aux_DataManager::m_AuxChnLookupTable[AuxType].m_TotalChnNum <= 0)
                continue;
            int TotalAuxChn = Aux_DataManager::m_AuxChnLookupTable[AuxType].m_TotalChnNum;
            int StartInSnake = Aux_DataManager::m_AuxChnLookupTable[AuxType].m_StartInSnake;
            for(int AuxChnIdx = 0; AuxChnIdx < TotalAuxChn; AuxChnIdx++)
            {
                ST_AUX_CHN_LOCATION *AuxChnLocation = &Aux_DataManager::m_AuxChanLongSnake[StartInSnake+AuxChnIdx];
                if(AuxChnLocation->m_valid)
                {
                    int data_group_idx = AuxChnLocation->m_data_group_idx;
                    int PHY_auxChnIdx = AuxChnLocation->m_chn;
                    if(Aux_DataManager::m_AuxDataBank[data_group_idx][PHY_auxChnIdx].m_Chn_Info.m_Populated == 1)
                    {
                        MyPoint.fValue = Aux_DataManager::m_AuxDataBank[data_group_idx][PHY_auxChnIdx].aux_data;
                        MyPoint.ucChannelIdx = AuxChnIdx;
                        MyPoint.ucValueType = Aux_DataManager::m_AuxDataBank[data_group_idx][PHY_auxChnIdx].m_Chn_Info.m_AuxType;

                        Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType = Aux_DataManager::m_AuxDataBank[data_group_idx][PHY_auxChnIdx].m_Chn_Info.m_AuxType;
                        Aux_DataManager::st_Aux_Data.aux_dx_dt = Aux_DataManager::m_AuxDataBank[data_group_idx][PHY_auxChnIdx].aux_dx_dt;

                        St_SDL_Point_Pack * st_Point;
                        st_Point =(St_SDL_Point_Pack *) &EthernetParse::m_Point_Temp[0];
                        *st_Point = MyPoint;

                        EthernetParse::MPReportData(0,SEND_AUX_LOGDATA);
                    }
                }
            }

        }

    }

}
*/
////////
//    SwitchLog(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned char bLogOn)
//    : SDL模块执行检查方式与发送方式开关处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
//         Send_Logtype :  发送网络方式
//         bLogOn : 开关标志
// Return: N/A
///////
void AuxSDL::SwitchLog(unsigned char uc_IVUnitID, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned char bLogOn)
{
    unsigned char uc_IVUnitBufIdx =  Aux_DataManager::Aux_LocateIVUnitBufferIndex(uc_IVUnitID);
    if( MaxValueCheck(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn) )
        return;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    if((Send_Logtype & LOG_PC_DATA) == LOG_PC_DATA)
    {
        if(bLogOn == LOGSWITCH_OFF)
            m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].bLogPcData = LOGSWITCH_OFF;   // 关闭
        else
        {
            m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].bLogPcData = bLogOn;  // 针对PC 打开或 其他使用

            ST_AUX_DATA st_Aux_Data;

            for(unsigned char AuxchIdx = 0; AuxchIdx < Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_Counts_ByScheduleMap; AuxchIdx++)
            {
                CommonFunc::GetTimeTicksUpdate(&m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][AuxchIdx].LogTime); // 更新记录时间
                unsigned char AuxchHard  = Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[AuxchIdx].m_Index_InAuxUnit; // 逻辑号下标转成物理号
                if( Aux_DataManager::Aux_GetData(uc_IVUnitID,uc_IVchn,uc_AuxType,AuxchHard,&st_Aux_Data) )
                    m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][AuxchIdx].fValue = st_Aux_Data.aux_data;  // 保存更新的采样数据值
            }
        }
    }

    if((Send_Logtype & REPORT_IV_DATA) == REPORT_IV_DATA)
    {
        if(bLogOn == LOGSWITCH_OFF)
        {
            m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].bReportIvData = LOGSWITCH_OFF; // 关闭
            m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportDataTypebit = 0;
        }
        else
        {
            m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].bReportIvData = bLogOn; // 针对IV 打开或 其他使用
            CommonFunc::GetTimeTicksUpdate( &m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].ReportTime);  // 更新IV记录时间
        }
    }
}

////////
//    RequireData(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char IvMCUIdx,unsigned long Factor)
//    : SDL模块登记需要定时数据MCU处理函数
// Input:  st_data_request  大数体请求数据结构
// Return: N/A
///////
void AuxSDL::RequireData(ST_AUX_DATA_REQUEST st_data_request)
{
    unsigned char uc_IVUnit = st_data_request.m_Source_CANID;
    unsigned char uc_IVchn = st_data_request.m_IV_index;
    unsigned char uc_AuxType =  st_data_request.m_aux_type;
    unsigned char uc_AuxChn = st_data_request.m_aux_chn_index;
    unsigned char IvMCUIdx = 0;
    //float         Factor;
    UWord64       uw64_Interval;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    switch(st_data_request.m_TimeDelta)
    {
    case LOG_TIME_1MS:
        uw64_Interval = 10;
        break;
    case LOG_TIME_10MS:
        uw64_Interval = 100;
        break;
    case LOG_TIME_20MS:
        uw64_Interval = 200;
        break;
    case LOG_TIME_50MS:
        uw64_Interval = 500;
        break;
    case LOG_TIME_100MS:
        uw64_Interval = 1000;
        break;
    case LOG_TIME_1000MS:
        uw64_Interval = 10000;
        break;
    case LOG_TIME_2000MS:
        uw64_Interval = 20000;
        break;
    case LOG_TIME_5000MS:
        uw64_Interval = 50000;
        break;
    default:
        uw64_Interval = 10000;
        break;
    }

    unsigned char uc_IVUnitBufIdx =  Aux_DataManager::Aux_LocateIVUnitBufferIndex(uc_IVUnit);
    unsigned char uc_AuxChnBufIdx = Aux_DataManager::Aux_LocateAuxBufferIndex(uc_IVUnitBufIdx,uc_IVchn,uc_AuxType,uc_AuxChn);
    m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChnBufIdx].MCUId = uc_IVUnit;
    //UWord64 uw64_Interval = (UWord64)(Factor * 10000);  // 秒变100微秒

    switch(st_data_request.m_data_type)
    {
    case ORIG_DATA:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChnBufIdx].ReportDataTypebit |= 1;
        break;
    case DX_DT_DATA:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChnBufIdx].ReportDataTypebit |= 2;
        break;
    case D2X_DT2_DATA:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChnBufIdx].ReportDataTypebit |= 4;
        break;
    default:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChnBufIdx].ReportDataTypebit |= 1;
        break;
    }

    SetFactor_Time(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChnBufIdx, REPORT_IV_DATA, uw64_Interval);

    if(st_data_request.m_MetaDelta != 0)
    {
        if(st_data_request.m_aux_type == AUX_TEMPERATURE)
            SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChnBufIdx, AUX_DEFAULT_SDL_FACTOR_T*st_data_request.m_MetaDelta);
        else if(st_data_request.m_aux_type == AUX_HUMIDITY)  //zyx 20160811
            SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChnBufIdx, AUX_DEFAULT_SDL_FACTOR_H*st_data_request.m_MetaDelta);
        else
            SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChnBufIdx, AUX_DEFAULT_SDL_FACTOR_2ndV*st_data_request.m_MetaDelta);
    }

    SwitchLog(uc_IVUnit, uc_IVchn, uc_AuxType, uc_AuxChnBufIdx, REPORT_IV_DATA, LOGSWITCH_ON);
}

////////
//    SDLSetParameterRoutine(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,St_SDL_DeltaValue * fSdlPara)
//    : SDL模块Schedule开始运行程序处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
//         fSdlPara :  配置各参数结构指针
// Return: N/A
///////
void AuxSDL::SDLSetParameterRoutine(unsigned char uc_IVUnitID, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,St_SDL_DeltaValue * fSdlPara)
{
    //if((fSdlPara->uc_IvFlag == LOG_DELTA_TIME)||(fSdlPara->uc_IvFlag == LOG_DELTA_ALL))
    //{
    //	UWord64 uw64_Interval = (UWord64)(fSdlPara->ReportInterval * 10000);  // 秒变100微秒
    //	SetFactor_Time( uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, uw64_Interval);  // 发送向IV
    //	//SwitchLog( uc_IVchn, uc_AuxType, uc_AuxChn, DATA_PC_AND_IV, LOGSWITCH_ON);
    //}
    float             LogInterval;
    float             fLogDeltaRate;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return;

    unsigned char uc_IVUnitBufIdx =  Aux_DataManager::Aux_LocateIVUnitBufferIndex(uc_IVUnitID);
    switch(uc_AuxType)  // 用于判定联合体
    {
    case AUX_VOLTAGE:
        LogInterval = SDL_LOGDATA_2VD_INTERVAL;  // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_2VD_DELTA;
        break;

    case AUX_TEMPERATURE:
        LogInterval = SDL_LOGDATA_T_INTERVAL;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_T_DELTA;
        break;
    case AUX_EXTERNALCHARGE:
        LogInterval = SDL_LOGDATA_EC_INTERVAL ;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_EC_DELTA;
        break;
    case AUX_PRESSURE:
        LogInterval = SDL_LOGDATA_PRESSURE_INTERVAL ;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_PRESSURE_DELTA;
        break;
    case AUX_HUMIDITY:                        // 20160714 zyx
        LogInterval = SDL_LOGDATA_H_INTERVAL;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_H_DELTA;
        break;
    default:
        LogInterval = SDL_LOGDATA_T_INTERVAL;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_T_DELTA;
        break;
    }

    // 使用默认方式SDL触发记录数据
    if(fSdlPara->uc_PCFlag == LOG_DELTA_NONE)
    {
        SetFactor_Time(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, LogInterval);   // 发送向PC
        SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, fLogDeltaRate);            // 发送向PC
        Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_sdl_alive = 0;
        return;
    }
    else if(fSdlPara->uc_PCFlag == LOG_DELTA_TIME_DEFAULT)
    {
        SetFactor_Time(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, LogInterval);   // 发送向PC
        Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_sdl_alive = 0;
        return;
    }
    else if(fSdlPara->uc_PCFlag == LOG_DELTA_META_DEFAULT)
    {
        SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, fLogDeltaRate);            // 发送向PC
        Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_sdl_alive = 0;
        return;
    }

    if((fSdlPara->uc_PCFlag == LOG_DELTA_TIME)||(fSdlPara->uc_PCFlag == LOG_DELTA_ALL))
    {
        if(fSdlPara->LogInterval == 0 )
        {
            fSdlPara->LogInterval = LogInterval;
        }
        UWord64 uw64_Interval = (UWord64)(fSdlPara->LogInterval * 10000);  // 秒变100微秒
        SetFactor_Time(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, uw64_Interval);  // 发送向PC
    }
    if((fSdlPara->uc_PCFlag == LOG_DELTA_META)||(fSdlPara->uc_PCFlag == LOG_DELTA_ALL))
    {
        if(fSdlPara->fLogDeltaRate == 0 )
        {
            fSdlPara->fLogDeltaRate = fLogDeltaRate;
        }
        SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, fSdlPara->fLogDeltaRate);  // 发送向PC
    }
    //SwitchLog( uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, LOGSWITCH_ON);
    Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_sdl_alive = 1;
}

////////
//    ScheduleEndedRoutine(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
//    : SDL模块Schedule结束程序处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
// Return: N/A
///////
void AuxSDL::SDLEndedRoutine(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
{
    float LogInterval,fLogDeltaRate;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return;
    // 恢复默认值设置参数
    switch(uc_AuxType)  // 用于判定联合体
    {
    case AUX_VOLTAGE:
        LogInterval = SDL_LOGDATA_2VD_INTERVAL;  // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_2VD_DELTA;
        break;

    case AUX_TEMPERATURE:
        LogInterval = SDL_LOGDATA_T_INTERVAL;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_T_DELTA;
        break;
    case AUX_EXTERNALCHARGE:
        LogInterval = SDL_LOGDATA_EC_INTERVAL ;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_EC_DELTA;
        break;
    case AUX_PRESSURE:
        LogInterval = SDL_LOGDATA_PRESSURE_INTERVAL ;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_PRESSURE_DELTA;
        break;
    case AUX_HUMIDITY:                        // 20160714 zyx
        LogInterval = SDL_LOGDATA_H_INTERVAL;   // 使用默认值
        fLogDeltaRate = SDL_LOGDATA_H_DELTA;
        break;
    default:
        LogInterval = SDL_LOGDATA_2VD_INTERVAL;  // 使用默认值
        fLogDeltaRate = AUX_DEFAULT_SDL_FACTOR_T;  // 1
        break;

    }
    SetFactor_Time(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, LogInterval);   // 发送向PC
    SetFactor_Value(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, fLogDeltaRate);            // 发送向PC
    SwitchLog(Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID, uc_IVchn, uc_AuxType, uc_AuxChn, LOG_PC_DATA, LOGSWITCH_OFF);
    //SwitchLog( uc_IVchn, uc_AuxType, uc_AuxChn, DATA_PC_AND_IV, LOGSWITCH_OFF);
    Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_sdl_alive = 0;
}

////////
//    TriggerLog_Net(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char uc_LogType, unsigned char uc_bCriticalPoint)
//    : SDL模块记录数据处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
//         uc_LogType :   记录类型是否显示数据、闲时数据
//         uc_bCriticalPoint :   关键点标志
// Return: N/A
///////
void AuxSDL::TriggerLog_Net(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char uc_LogType)
{
    ST_AUX_DATA_TRIG DataTrigInfo;
    St_SDL_Point_Pack * st_Point;
    st_Point =(St_SDL_Point_Pack *) &EthernetParse::m_Point_Temp[0];
    st_Point->u32Second = 0;
    st_Point->u16Us100 = 0;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    if((uc_LogType == SDL_DATA_FOR_DISPLAY)||(uc_LogType == SDL_DATA_FOR_IDLE))
    {   //显示数据或者闭时数据
        st_Point->ucType = uc_LogType;

        DataTrigInfo.m_IVChnIdx = uc_IVchn;
        DataTrigInfo.m_IV_unitID = Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
        DataTrigInfo.m_AuxType = uc_AuxType;
        DataTrigInfo.TrigTime = CommonFunc::GetSystemTime();


        DataTrigInfo.m_AuxChnIdx  = Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
        Aux_DataManager::Aux_TrigLogPoint(&DataTrigInfo, st_Point);

        return;
    }

//     if(uc_LogType == SDL_DATA_FOR_LOG)
//     {   //记录数据
//         if(m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].bLogPcData) // 按发送方向PC/IV_MCU更新记录时间
//         {
//             st_Point->ucType = SDL_DATA_FOR_LOG;
//             DataTrigInfo.m_IVChnIdx = uc_IVchn;
//             DataTrigInfo.m_IV_unitID = Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
//             DataTrigInfo.m_AuxType = uc_AuxType;
//             DataTrigInfo.TrigTime = CommonFunc::GetSystemTime();
//             DataTrigInfo.m_AuxChnIdx  = Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
//             Aux_DataManager::Aux_TrigLogPoint(&DataTrigInfo, st_Point);
//             m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].fValue = st_Point->fValue;  // 保存更新的采样数据值
//         }
//     }
}

//private:
////////
//    Check_LogValue(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
//    : SDL模块检查记录值准备发送PC处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
// Return: N/A
///////
//void AuxSDL::Check_LogValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
//{
////    unsigned char uc_LogDataFlag = 0;
//    UWord64  uw64_temp = 0;
////    float f_temp = 0.0f;
////    float fValue = 0.0f;
//    ST_AUX_DATA st_Aux_Data;
//		memset(&st_Aux_Data,0,sizeof(ST_AUX_DATA));
//
//    // 1)记录数据处理
//    // A: Delta变量获取数据
//    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
//    if(TempAuxTypeIdex == 0xFF)//辅助限制
//        return ;

////     if(Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate != 0)
////     {
////         unsigned char uc_ChIndex_InAuxUnit = Aux_DataManager::m_MapFromPC[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
////         unsigned char IV_unitID =  Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
////         Aux_DataManager::Aux_GetData(IV_unitID, uc_IVchn,uc_AuxType,uc_ChIndex_InAuxUnit,&st_Aux_Data);   // 取得采样值
////         fValue  = st_Aux_Data.aux_data;  // 取得采样值
////         switch(uc_AuxType)
////         {
////         case AUX_DO:
////             if(fValue != m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].fValue)
////             {
////                 TriggerLog_Net(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, SDL_DATA_FOR_LOG);
////                 uc_LogDataFlag = 1;
////             }
////             break;

////         default:
////             f_temp = fValue - m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].fValue;
////             if(::fabsf(f_temp) >= ::fabsf(m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue))  // 要用计算后的值，不用千分之几的比率。
////             {
////                 //if(((f_temp < 0)&&(Aux_DataManager::m_AuxSDLBank[uc_IVUnit][uc_IVchn][uc_AuxType].m_fLogDeltaValue < 0))
////                 //	|| ((f_temp > 0)&&(Aux_DataManager::m_AuxSDLBank[uc_IVUnit][uc_IVchn][uc_AuxType].m_fLogDeltaValue > 0)))
////                 //{
////                 TriggerLog_Net(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, SDL_DATA_FOR_LOG);
////                 uc_LogDataFlag = 1;
////                 //}
////             }
////             break;
////         }
////     }

////     // B: 间隔时间获取数据
////     if(uc_LogDataFlag == 0)
////     {
////         uw64_temp = CommonFunc::GetTimeTicks(&m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].LogTime);
////         if(uw64_temp >= Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_log_interval)
////         {
////             TriggerLog_Net(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, SDL_DATA_FOR_LOG);
////             uc_LogDataFlag = 1;
////         }
////     }

////     if(uc_LogDataFlag == 1)
////     {
////         CommonFunc::GetTimeTicksUpdate(&m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].LogTime); // 更新记录时间
////         CommonFunc::GetTimeTicksUpdate( &m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].DisplayLogTime);  // 更新显示时间
////         return;
////     }

//    // 2)显示数据处理
//    uw64_temp = CommonFunc::GetTimeTicks(&m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].DisplayLogTime);
//    if(uw64_temp >= DISPLAY_LOGDATA_INTERVAL)   // 默认的间隔显示时间
//    {
//        CommonFunc::GetTimeTicksUpdate( &m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].DisplayLogTime);  // 更新显示时间
//        TriggerLog_Net(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn, SDL_DATA_FOR_DISPLAY);
////        uc_LogDataFlag = 2;
//    }

//    // 3)闲时数据处理  (在上一层高处判断，避免过多的语句判断）
//}

////////
//    Check_ReportValue(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
//    : SDL模块检查IV要求记录值准备发送IV处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
// Return: N/A
///////
void AuxSDL::Check_ReportValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
{
    unsigned char uc_SendFlag = 0;
    UWord64  uw64_temp;
    ST_AUX_DATA st_Aux_Data;
    MP_CAN_MSG can_msg;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    uw64_temp = CommonFunc::GetTimeTicks(&m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].ReportTime);
    if(uw64_temp >= m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportInterval)
    {
        unsigned char uc_ChIndex_InAuxUnit = Aux_DataManager::m_MapFromPC[m_CurrentIVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InAuxUnit; // 逻辑号下标转成物理号
        Aux_DataManager::Aux_GetData(Aux_DataManager::m_IVMCU_ID_Info[m_CurrentIVUnitBufIdx].m_ID, uc_IVchn, uc_AuxType, uc_ChIndex_InAuxUnit, &st_Aux_Data);   // 取得采样值
        CommonFunc::GetTimeTicksUpdate( &m_LastValue[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex][uc_AuxChn].ReportTime);  // 更新IV记录时间
        ST_AUX_DATAPOINT Aux_DataPoint;
        Aux_DataPoint.DestID = Aux_DataManager::m_IVMCU_ID_Info[m_CurrentIVUnitBufIdx].m_ID;
        Aux_DataPoint.AckStatus = 0;  //ok
        Aux_DataPoint.Auxtype = uc_AuxType;
        //Aux_DataPoint.AuxChn = uc_AuxChn;  //hepeiqing   20150312
        Aux_DataPoint.AuxChn = Aux_DataManager::m_MapFromPC[m_CurrentIVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_MapInfo[uc_AuxChn].m_Index_InIVCh;
        Aux_DataPoint.f_Value = st_Aux_Data.aux_data;     // 取得采样值
        Aux_DataPoint.ChnNo = uc_IVchn;
        Aux_DataPoint.DataType = 0;  //ORIG_DATA
        //CANBusParse::Aux_RptData(Aux_DataPoint);

        unsigned long int uw32_DType = m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportDataTypebit;
        for(unsigned char uc_i=0; uc_i<3; uc_i++) // for(unsigned char uc_i=0;uc_i<32;uc_i++)
        {
            if((uw32_DType>>uc_i) == 0X00000001)   // 按位标识使用 //数据类型，原始数据0bit， 一阶时间导数 1bit， 二阶时间导数2bit
            {
                switch(uc_i)
                {
                case 0:
                    Aux_DataPoint.DataType = ORIG_DATA;
                    Aux_DataPoint.f_Value = st_Aux_Data.aux_data;     // 取得采样值
                    CANBusParse::Aux_RptData(Aux_DataPoint);
                    break;
                case 1:
// 					MsTime  tempT = OS_ServeProcess::TimeDiff(m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportTime_DXDT);
                    unsigned long long  tempT = CommonFunc::GetTimeTicks(&m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportTime_DXDT);//zyx 180615
                    if(uc_AuxType == AUX_TEMPERATURE)
                    {
                        if(tempT >= MAX_DXDT_T_TIME)////zyx 180615						if(tempT.Second >= MAX_DXDT_T_TIME)
                        {
                            CommonFunc::GetTimeTicksUpdate(&m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportTime_DXDT);
                            uc_SendFlag = 1;
                        }
                    }
                    else
                    {
                        if(tempT >= MAX_DXDT_V_TIME)////zyx 180615						if(tempT.Second >= MAX_DXDT_V_TIME)
                        {
                            CommonFunc::GetTimeTicksUpdate(&m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportTime_DXDT);
                            uc_SendFlag = 1;
                        }
                    }
                    if(uc_SendFlag)
                    {
                        Aux_DataPoint.DataType = DX_DT_DATA;
                        Aux_DataPoint.f_Value = st_Aux_Data.aux_dx_dt;
                        CANBusParse::Aux_RptData(Aux_DataPoint);
                    }
                    break;
                case 2:
                    if(uc_SendFlag)
                    {
                        Aux_DataPoint.DataType = D2X_DT2_DATA;
                        Aux_DataPoint.f_Value = st_Aux_Data.aux_d2x_dt2;
                        CANBusParse::Aux_RptData(Aux_DataPoint);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

////////
//    SetFactor_Time (unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned long Factor)
//    : SDL模块设置时间参数处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
//         Send_Logtype :   发送数据方向，IV，PC
//         Factor :   时间参数
// Return: N/A
///////
void AuxSDL::SetFactor_Time (unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,UWord64 Factor)
{
    if( MaxValueCheck(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn) )
        return;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    if((Send_Logtype & LOG_PC_DATA) == LOG_PC_DATA)
    {
        ST_AUX_SDL st_sdl = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex];
        st_sdl.m_Source_CANID = Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
        st_sdl.m_IV_index = uc_IVchn;
        st_sdl.m_aux_type = uc_AuxType;
        st_sdl.m_log_interval = Factor;
        st_sdl.m_sdl_alive = 1;
        Aux_DataManager:: Aux_AddSDL(st_sdl);
    }

    if((Send_Logtype & REPORT_IV_DATA) == REPORT_IV_DATA)
    {
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].bReportIvData = LOGSWITCH_ON;
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].NeedValue[uc_AuxChn].ReportInterval = Factor;
    }
}

////////
//    SetFactor_Value(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,float Factor)
//    : SDL模块设置变量参数处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
//         Factor :   时间参数
// Return: N/A
///////
void AuxSDL::SetFactor_Value(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,float Factor)
{
    float RangeH,RangeL;
    unsigned short int uw16_AuxChennal = 0;
    unsigned char uc_i=0;

    if( MaxValueCheck(uc_IVUnitBufIdx, uc_IVchn, uc_AuxType, uc_AuxChn) )
        return;

    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return ;

    ST_AUX_SDL st_sdl = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex];
    st_sdl.m_Source_CANID = Aux_DataManager::m_IVMCU_ID_Info[uc_IVUnitBufIdx].m_ID;
    st_sdl.m_IV_index = uc_IVchn;
    st_sdl.m_aux_type = uc_AuxType;
    st_sdl.m_fLogDeltaRate = Factor;
    st_sdl.m_sdl_alive = 1;
    Aux_DataManager:: Aux_AddSDL(st_sdl);

    // 存好后再用最大量程进行计算，因为下位机下发的是千分率，得转成数值进行运算。
// 	for( uc_i=0; uc_i < MAX_BOARD_PER_AUXMCU; uc_i ++) //由于该for循环内部已无代码，故屏蔽该for循环 20160811
// 	{
// 		//if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_BType.m_AuxType == uc_AuxType)
// 		/*	if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_BType.m_AuxType == uc_AuxType)  //
// 		{
// 		for(unsigned char uc_j = 0; uc_j < uc_i ;uc_j ++)
// 		{
// 		uw16_AuxChennal += Aux_DataManager::m_AllAuxBoards[uc_j].m_abi_globe.m_Total_Channels;  // 扫出一字列表中的通道参数
// 		}
// 		break;
// 		}*/  //暂时屏蔽  qjm 20150603
// 	}

    switch(uc_AuxType)  // 用于判定联合体
    {
    case AUX_VOLTAGE:
        // 			RangeH = Aux_DataManager::m_AllAuxBoards[uc_i].m_AuxSensor_ChInfo[uw16_AuxChennal].m_Aux2ndV.RangeH;  // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。
        // 			RangeL = Aux_DataManager::m_AllAuxBoards[uc_i].m_AuxSensor_ChInfo[uw16_AuxChennal].m_Aux2ndV.RangeL;

        //RangeH = Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uw16_AuxChennal].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;  // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。
        //RangeL = Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uw16_AuxChennal].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
        //RangeH = 5; // 暂时锁着量程 ？？？20131217
        //RangeL = -5; // 暂时锁着量程 ？？？20131217
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate;
        break;

    case AUX_TEMPERATURE:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate; // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。

        break;

    case AUX_HUMIDITY:  //20160714 zyx
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate; // 因为大数据体为两维管理SDL数，仅取第一通道量程范围。
        break;

    case AUX_EXTERNALCHARGE:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate;
        break;
    case AUX_PRESSURE:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate;
        break;
    default:
        m_Info[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].fLogDeltaValue = Aux_DataManager::m_AuxSDLBank[uc_IVUnitBufIdx][uc_IVchn][TempAuxTypeIdex].m_fLogDeltaRate;
        break;
    }
}

////////
//    GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100)
//    : SDL模块计算两时间之间长度处理函数
// Input:  From ：  开始时间
//         To :   结束时间
// Return:  TimeTicks：时间长度
///////
inline unsigned long long AuxSDL::GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100)
{
    if(ToInsecond < FromInsecond)
        return 0;
    if((ToInsecond == FromInsecond)&&(ToUs100 <= FromUs100))
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
}

////////
//    GetFullValue(float HighValue,float LowValue)
//    : SDL模块计算范围内精度长度处理函数
// Input:  HighValue ：  高值范围
//         LowValue :    低值范围
// Return: Value：范围
///////
inline float AuxSDL::GetFullValue(float HighValue,float LowValue)
{
    float Value1,Value2;

    Value1 = HighValue;
    if(Value1 < 0)
        Value1 = -Value1;

    Value2 = LowValue;
    if(Value2 < 0)
        Value2 = -Value2;

    if(Value1 < Value2)
        return Value2*2;
    else
        return Value1*2;
}

////////
//    MaxValueCheck(float HighValue,float LowValue)
//    : SDL模块保护范围内精度长度处理函数
// Input:  uc_IVchn  ：  IV通道
//         uc_AuxType ：  辅助通道类型
//         uc_AuxChn :   辅助通道号
// Return: Value：0 ：没有错误
///////
unsigned char AuxSDL::MaxValueCheck(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn)
{
    if(uc_AuxType >=MAX_AUX_TYPE)
        return AUX_TYPE_OVERFLOW;
    if(uc_IVchn >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;
    if(uc_AuxChn >= MAX_CH_IN_AUX_UNIT)
        return AUX_CHNO_OVERFLOW;

    return NO_ERROR;
}



