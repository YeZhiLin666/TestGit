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

class Check_Limit8_task
{
public:
    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;                                                                  //���ȼ�

    static unsigned char m_CheckedChannelCount_SL;
    static unsigned char m_CheckedChannelCount_LL;

public:
    static void        DoWork(void);                                                                 //����ִ�к���
    static void        MyInit(void);                                                                 //�����ʼ��
// 	static void        EndTask(void);                                                                //�������
};


