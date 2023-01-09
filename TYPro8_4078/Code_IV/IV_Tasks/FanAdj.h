///////////////////////////////////////////////////////////////////////////////////
//File Name: FanAdj.h
//Version:		V1.0
//Date:				2015/05/13
//Description:
//						Fan�������
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../IV_Tasks/OS_LogicProcess.h"
// #define FANADJ_INTERVAL				3000
// #define VF                           53.0f
// #define VR                           0.0f//-2.5f
enum FANADJMOD
{
    FANADJ_NA=0,
    I2CMOD=1,
    PWMMOD,
};
typedef struct
{
    bool Fan_Level_Valid;

    MsTime RecordProduceTime;

} FanLeveInfo;
class FanAdj_task
{
public:
    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;



// 	static unsigned short HeatSinkCounter;
// 	static float PerAdjPower;
    static unsigned char AdjMod;


    static unsigned char MAX_Ratio;
    static unsigned char FanCtrlValue[CURRENT_RANGE_COUNT][8];
    //���ȼ�
private:
    static MsTime LastSampleTime;
    //static float PerAdjPower200; //Ron added 101715
    static unsigned char Output_Level;

    static FanLeveInfo AllFanLevelInfo[8];

public:
    static void        DoWork(void);                                                                 //����ִ�к���
    static void        MyInit(void);                                                                 //�����ʼ��
// 	static void        EndTask(void);
};
