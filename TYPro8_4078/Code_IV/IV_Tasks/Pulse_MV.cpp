///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Energy.cpp
//Version:        V1.0
//Date:                2014/08/22
//Description:
//
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

UInt16            PULSE_MV_Task::Priority=PRIORITY4;
UInt16            PULSE_MV_Task::TaskID=PULSE_MV_TASK;
St_PulseStgMV     PULSE_MV_Task::StgInfo[MAX_PULSE_STAGE_NUM];

void PULSE_MV_Task::Init(void)
{
    for(unsigned char i=0; i<MAX_PULSE_STAGE_NUM; i++) //复位标记位
    {
        Pulse::raw_data[1][1][i].m_bSumCapacity=0;
        Pulse::raw_data[0][1][i].m_bSumCapacity=0;
    }
}
void PULSE_MV_Task::DoWork(void)
{
    for(unsigned char i = 0; i < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount ; i ++)
        Pulse_CalCapacity(i);//根据实际需要决定能量计算任务间隔
}
//计算原则：
//原则上两个原始数据缓冲里。凡是没有被用来累加过的，都要进行用于累计。
//对于有完整记录数据的buffer，如果所有的数据都没用于计算过电量的话。 要看已经经历过了多少个cycle，
//然后用这些数据来代表说有前面经历过的cycle。以此来累计电量。（这种情况发生在脉冲cycle的时间短于metavarable 任务的时间周期，比如200us）
//计算的时候要按charge和discharge分开累计，如果脉冲cycle时间长于metavariable周期的话，在做一次metavariable任务之后，两个buffer的数据都可能只是部分更新。
//对部分填满的buffer，或者虽然已经填满，但只有部分数据被用于计算过电量的，也要对没有累计过的部分进行累计电量。也是按charge及discharge来分开计算
//最终的电量是由整cycle部分和部分cycle部分的总和。
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

            pRaw->m_bSumCapacity = 0;   //已经计算过了，就标记一下，防止重复计算
            pStgInfo->CalculatedCycle++;
            pStgInfo->LastCapacity = fCapa;
            pStgInfo->LastEnergy   = fEner;

            if((Pulse::m_Info.m_CycleCounter > StgInfo[StgIdx].CalculatedCycle)
                    &&(Pulse::m_Info.m_CycleCounter - StgInfo[StgIdx].CalculatedCycle >= 20))    //20这个数字是个估算，可能会导致误差
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
