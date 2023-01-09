#include "../Entry/includes.h"

UInt16 SampleProcess_Pulse::Priority = PRIORITY4;
UInt16 SampleProcess_Pulse::TaskID = SAMPLE_PULSE_TASK;			// should be a definition which positioning this task in OS_Task[]

bool   SampleProcess_Pulse::NewCodeSampled;

void SampleProcess_Pulse::Init(void)
{

}

void SampleProcess_Pulse::DoWork(void)
{
    //如果当前处于慢脉冲状态，而且普通采样也有新的码值。   则进行一次滤波转换过程
    //因为PULSE功能只在单通道里运行，所以这里姑且就直接写Vector[0]这种代码了。
    if(!Pulse::m_Info.m_bPulseSample)
    {   //正在运行慢脉冲，可以处理换码
        if(SampleProcess_Pulse::NewCodeSampled)   //完成了一次普通ADC过程
        {
            if( ChannelInfo_Manager::Insert_Current2HistoryData(0,
                    ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc,
                    ChannelInfo_Manager::ChannelInfo[0].m_ucCurrentRange))
            {
// 				ChannelInfo_Manager::MarkSampleDataAsFresh( 0 );  //04.23.2013
                ChannelInfo_Manager::m_DataFreshForPID[0] = true;
                safety_check::I_Sample_Flag[0] = 1;    //chen 20130812
                ChannelInfo_Manager::ChannelInfo[0].Current_Sampleflg=1;  //dirui 20130917
                ChannelInfo_Manager::Set_CurrentRecalculateSign(0);
            }
            if( ChannelInfo_Manager::Insert_Voltage2HistoryData(0,
                    ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc,
                    ChannelInfo_Manager::ChannelInfo[0].m_ucVoltageRange))
            {
// 				ChannelInfo_Manager::MarkSampleDataAsFresh( 0 );  //04.23.2013
                ChannelInfo_Manager::m_DataFreshForPID[0] = true;
                safety_check::V_Sample_Flag[0] = 1;    //chen 20130812
                ChannelInfo_Manager::ChannelInfo[0].Voltage_Sampleflg=1;  //dirui 20130917
                ChannelInfo_Manager::Set_VoltageRecalculateSign(0);
            }

            SampleProcess_Pulse::NewCodeSampled = 0;
        }
    }
    ConvertPulseStageData();    //需要将脉冲中断采样的ADC码转换成浮点数，供limitcheck 和safety检查用，无论当前是出于快stage，还是慢stage

    if(board_config::Func_BuildInVolt_ReadAll != 0)
        (* board_config::Func_BuildInVolt_ReadAll)();

    return;
}

void SampleProcess_Pulse::ConvertPulseStageData(void)
{
    float fCurrent,fVoltage,fPower;
    unsigned char StgIdx;
    bool  bConverted = false;

    for(StgIdx = 0; StgIdx < Pulse::m_Info.m_StageCount; StgIdx++)
    {
        PULSE_RAW_DATA * p_RawData[4] = {
            &Pulse::raw_data[0][SAVE_2_HEAD][StgIdx],
            &Pulse::raw_data[0][SAVE_2_TAIL][StgIdx],
            &Pulse::raw_data[1][SAVE_2_HEAD][StgIdx],
            &Pulse::raw_data[1][SAVE_2_TAIL][StgIdx]
        };
        for(int i=0; i<4; i++)
        {
            PULSE_RAW_DATA * p_Data = p_RawData[i];
            if((p_RawData[i]->m_bSampleUpdated) && (!p_RawData[i]->m_bDataConverted))
            {
                //code被更新了，同时 数据没有被转换
                fCurrent = ChannelInfo_Manager::CalibrateCurrentADC(0,
                           ChannelInfo_Manager::ChannelInfo[0].m_ucCurrentRange,
                           p_Data->m_uI_ADC_Code);
                fVoltage = ChannelInfo_Manager::CalibrateVoltageADC(0,
                           ChannelInfo_Manager::ChannelInfo[0].m_ucVoltageRange,
                           p_Data->m_uV_ADC_Code);
                p_Data->m_fCurrent = fCurrent;
                p_Data->m_fVoltage = fVoltage;
                p_Data->m_bSampleUpdated = 0;
                p_Data->m_bDataConverted = 1;
                p_Data->m_bSumCapacity   = 1;

                unsigned char Pos = (i%2) ? SAVE_2_TAIL : SAVE_2_HEAD;
                ST_PULSE_STAGE_DATA * p_StageData = &ChannelInfo_Manager::Pulse_Stage_Data[Pos][StgIdx];

                p_StageData->m_Current = fCurrent;
                p_StageData->m_Voltage = fVoltage;
                p_StageData->m_Valid = 1;           //标记这个数据是合法的
                p_StageData->m_Checked = 0;         //标记这个数据还没有被用于LIMIT CHECK
                p_StageData->m_StageID = StgIdx;    //冗余记录

                if(fVoltage > Pulse::fMaxVoltage)
                {
                    Pulse::fMaxVoltage = fVoltage;
                    Pulse::fMaxVoltage_Curr = fCurrent;
                }

                if(fVoltage < Pulse::fMinVoltage)
                {
                    Pulse::fMinVoltage = fVoltage;
                    Pulse::fMinVoltage_Curr = fCurrent;
                }

                if(fCurrent > Pulse::fMaxCurrent)
                    Pulse::fMaxCurrent = fCurrent;

                if(fCurrent < Pulse::fMinCurrent)
                    Pulse::fMinCurrent = fCurrent;

                fPower = fVoltage * fCurrent;

                if(fPower > Pulse::fMaxPower)
                    Pulse::fMaxPower = fPower;

                if(fPower < Pulse::fMinPower)
                    Pulse::fMinPower = fPower;

                bConverted = true;
            }
        }
    }

    if(bConverted)
    {
        ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Voltage].m_fValue = fVoltage;
        ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Current].m_fValue = fCurrent;
    }
}

// void SampleProcess_Pulse::EndTask(void)
// {

// }

