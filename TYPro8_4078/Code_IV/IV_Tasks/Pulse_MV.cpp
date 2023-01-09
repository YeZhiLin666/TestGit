///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Energy.cpp
//Version:        V1.0
//Date:                2014/08/22
//Description:
//
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

UInt16            PULSE_MV_Task::Priority=PRIORITY4;
UInt16            PULSE_MV_Task::TaskID=PULSE_MV_TASK;
St_PulseStgMV     PULSE_MV_Task::StgInfo[MAX_PULSE_STAGE_NUM];

void PULSE_MV_Task::Init(void)
{
    for(unsigned char i=0; i<MAX_PULSE_STAGE_NUM; i++) //��λ���λ
    {
        Pulse::raw_data[1][1][i].m_bSumCapacity=0;
        Pulse::raw_data[0][1][i].m_bSumCapacity=0;
    }
}
void PULSE_MV_Task::DoWork(void)
{
    for(unsigned char i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; i ++)
        Pulse_CalCapacity(i);//����ʵ����Ҫ������������������
}
//����ԭ��
//ԭ��������ԭʼ���ݻ��������û�б������ۼӹ��ģ���Ҫ���������ۼơ�
//������������¼���ݵ�buffer��������е����ݶ�û���ڼ���������Ļ��� Ҫ���Ѿ��������˶��ٸ�cycle��
//Ȼ������Щ����������˵��ǰ�澭������cycle���Դ����ۼƵ������������������������cycle��ʱ�����metavarable �����ʱ�����ڣ�����200us��
//�����ʱ��Ҫ��charge��discharge�ֿ��ۼƣ��������cycleʱ�䳤��metavariable���ڵĻ�������һ��metavariable����֮������buffer�����ݶ�����ֻ�ǲ��ָ��¡�
//�Բ���������buffer��������Ȼ�Ѿ���������ֻ�в������ݱ����ڼ���������ģ�ҲҪ��û���ۼƹ��Ĳ��ֽ����ۼƵ�����Ҳ�ǰ�charge��discharge���ֿ�����
//���յĵ���������cycle���ֺͲ���cycle���ֵ��ܺ͡�
// unsigned long CapaCounter = 0;
void PULSE_MV_Task::Pulse_CalCapacity(unsigned char uc_ChannelNo)
{
    unsigned char StgIdx,StgCount = Pulse::m_Info.m_StageCount;
    float fCurr2,fVolt2;
    double fCapa,fEner;
// 	bool bNeedCal = 0;
    bool active = !Pulse::active_index;
// 	CapaCounter++;
    for(StgIdx=0; StgIdx<StgCount; StgIdx++)
    {
        PULSE_RAW_DATA* pRaw = &Pulse::raw_data[active][SAVE_2_TAIL][StgIdx];
        St_PulseStgMV* pStgInfo = &StgInfo[StgIdx];
        if(pRaw->m_bSumCapacity)
        {
            fCurr2 = pRaw->m_fCurrent;
            fVolt2 = pRaw->m_fVoltage;
            fCapa  = fCurr2 * pStgInfo->TimeInHour;
            unsigned long ulPulseTime = pStgInfo->TimeInUs;
            fEner  = fCapa * fVolt2;

            pRaw->m_bSumCapacity = 0;   //�Ѿ�������ˣ��ͱ��һ�£���ֹ�ظ�����
            pStgInfo->CalculatedCycle++;
            pStgInfo->LastCapacity = fCapa;
            pStgInfo->LastEnergy   = fEner;

            if((Pulse::m_Info.m_CycleCounter > StgInfo[StgIdx].CalculatedCycle)
                    &&(Pulse::m_Info.m_CycleCounter - StgInfo[StgIdx].CalculatedCycle >= 20))    //20��������Ǹ����㣬���ܻᵼ�����
            {
                fCapa *= (Pulse::m_Info.m_CycleCounter - StgInfo[StgIdx].CalculatedCycle - 1);
                fEner *= (Pulse::m_Info.m_CycleCounter - StgInfo[StgIdx].CalculatedCycle - 1);
                ulPulseTime *= (Pulse::m_Info.m_CycleCounter - StgInfo[StgIdx].CalculatedCycle - 1);
                StgInfo[StgIdx].CalculatedCycle = Pulse::m_Info.m_CycleCounter;
            }
            if(fCapa > 0)
            {
// 			ChannelInfo_Manager::IncCCapacity(uc_ChannelNo,fCapa);
// 			ChannelInfo_Manager::IncCEnergy(uc_ChannelNo,fEner);
                ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeCapacity,uc_ChannelNo,fCapa);
                ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_ChargeEnergy,uc_ChannelNo,fEner);
                ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_ChargeCapacityTime, uc_ChannelNo, ulPulseTime);

            }
            else
            {
// 			ChannelInfo_Manager::IncDCapacity(uc_ChannelNo,-fCapa);
// 			ChannelInfo_Manager::IncDEnergy(uc_ChannelNo,-fEner);
                ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeCapacity,uc_ChannelNo,-fCapa);
                ChannelInfo_Manager::IncCapacityOrEnergy(MetaCode_PV_DischargeEnergy,uc_ChannelNo,-fEner);
                ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(MetaCode_PV_DischargeCapacityTime, uc_ChannelNo, ulPulseTime);

            }

// 			bNeedCal = 1;
        }

// 		if(bNeedCal == 0)
// 			continue;
//
// 		bNeedCal = 0;
    }
}
// void PULSE_MV_Task::EndTask(void)
// {

// }
