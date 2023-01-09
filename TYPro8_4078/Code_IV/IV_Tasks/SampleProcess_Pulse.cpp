#include "../Entry/includes.h"

UInt16 SampleProcess_Pulse::Priority = PRIORITY4;
UInt16 SampleProcess_Pulse::TaskID = SAMPLE_PULSE_TASK;			// should be a definition which positioning this task in OS_Task[]

bool   SampleProcess_Pulse::NewCodeSampled;

void SampleProcess_Pulse::Init(void)
{

}

void SampleProcess_Pulse::DoWork(void)
{
    //�����ǰ����������״̬��������ͨ����Ҳ���µ���ֵ��   �����һ���˲�ת������
    //��ΪPULSE����ֻ�ڵ�ͨ�������У�����������Ҿ�ֱ��дVector[0]���ִ����ˡ�
    if(!Pulse::m_Info.m_bPulseSample)
    {   //�������������壬���Դ�����
        if(SampleProcess_Pulse::NewCodeSampled)   //�����һ����ͨADC����
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
    ConvertPulseStageData();    //��Ҫ�������жϲ�����ADC��ת���ɸ���������limitcheck ��safety����ã����۵�ǰ�ǳ��ڿ�stage��������stage

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
                //code�������ˣ�ͬʱ ����û�б�ת��
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
                p_StageData->m_Valid = 1;           //�����������ǺϷ���
                p_StageData->m_Checked = 0;         //���������ݻ�û�б�����LIMIT CHECK
                p_StageData->m_StageID = StgIdx;    //�����¼

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

