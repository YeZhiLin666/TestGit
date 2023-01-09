///////////////////////////////////////////////////////////////////////////////////
//File Name: Step.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						Step管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../IV_Tasks/OS_LogicProcess.h"


#define STEPLIMITCHECK_COUNT   4   //每个周期CHECK的STEP LIMIT个数
#define LOGLIMITCHECK_COUNT    4   //每个周期CHECK的LOG LIMIT个数 //2

class Check_Limit4_task
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级

    static unsigned char m_CheckedChannelCount_SL;
    static unsigned char m_CheckedChannelCount_LL;
    static unsigned char m_BuiltInV_AnyLimitChn;
    static unsigned char m_BuiltInT_AnyLimitChn;

public:
    static void        DoWork(void);                                                                 //任务执行函数
    static void        MyInit(void);                                                                 //任务初始化
// 	static void        EndTask(void);                                                                //任务结束

    static void        Reset(void);
    static void        SL_Check(unsigned char MaxLimitCount);
    static signed char FindNextStepLimit(void);

    static void        LL_Check(unsigned char MaxLimitCount);
    static signed char FindNextLogLimit(void);
    static void        LL_DoLog(unsigned char ChannelIdx);
    static unsigned char  CompareSign(unsigned char m_ucCompareSign,const float fLeftValue, const float fRightValue);
//#if SUPPORT_AUX == 1
    static void        LimitStatusChanged(unsigned char ChannelIdx,unsigned char bTrue,int SN);
    static void       GetAuxValue(St_Limit * thisLimit,unsigned char uc_AuxType,unsigned char uc_AuxDxDt,float *fGetValue1,unsigned char ChannelIdx,unsigned char ucValue_Operator, float fValueCoefficient);
//#endif

};


