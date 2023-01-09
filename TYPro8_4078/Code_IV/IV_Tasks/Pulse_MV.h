///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Energy.h
//Version:        V1.0
//Date:                2014/08/22
//Description:
//       脉冲电量计算 根据满Buffer的台阶数据计算出一个cycle里的电量，然后乘以DV_Pulse_Cycle_Index,
//	     来近似表示自上次计算以来的电量变化值。
//		 能量计算 累计（每个台阶的电量 * 每个台阶的电压）得到一个Cycle的能量，最后乘以DV_Pulse_Cycle_Index
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once

typedef struct
{
    float          TimeInHour;
    unsigned long  CalculatedCycle;
    float          LastCapacity;
    float          LastEnergy;
    unsigned long  TimeInUs;
} St_PulseStgMV;

class PULSE_MV_Task
{
public:
    static UInt16               Priority;
    static UInt16               TaskID;

public:
    static St_PulseStgMV        StgInfo[MAX_PULSE_STAGE_NUM];
    static void Init(void);
    static void DoWork(void);
// 	static void EndTask(void);
private:
    static void Pulse_CalCapacity(unsigned char uc_ChannelNo);
    //static void Pulse_IncCCapacity(unsigned char uc_ChannelNo,float f_Capacity);
    //static void Pulse_IncDEnergy(unsigned char uc_ChannelNo,float f_Capacity);

};
