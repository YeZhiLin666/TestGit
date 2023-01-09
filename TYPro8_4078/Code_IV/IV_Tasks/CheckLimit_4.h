///////////////////////////////////////////////////////////////////////////////////
//File Name: Step.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						Step�������
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../IV_Tasks/OS_LogicProcess.h"


#define STEPLIMITCHECK_COUNT   4   //ÿ������CHECK��STEP LIMIT����
#define LOGLIMITCHECK_COUNT    4   //ÿ������CHECK��LOG LIMIT���� //2

class Check_Limit4_task
{
public:
    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;                                                                  //���ȼ�

    static unsigned char m_CheckedChannelCount_SL;
    static unsigned char m_CheckedChannelCount_LL;
    static unsigned char m_BuiltInV_AnyLimitChn;
    static unsigned char m_BuiltInT_AnyLimitChn;

public:
    static void        DoWork(void);                                                                 //����ִ�к���
    static void        MyInit(void);                                                                 //�����ʼ��
// 	static void        EndTask(void);                                                                //�������

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


