///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Energy.h
//Version:        V1.0
//Date:                2014/08/22
//Description:
//       ����������� ������Buffer��̨�����ݼ����һ��cycle��ĵ�����Ȼ�����DV_Pulse_Cycle_Index,
//	     �����Ʊ�ʾ���ϴμ��������ĵ����仯ֵ��
//		 �������� �ۼƣ�ÿ��̨�׵ĵ��� * ÿ��̨�׵ĵ�ѹ���õ�һ��Cycle��������������DV_Pulse_Cycle_Index
//Others:
//History��

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
