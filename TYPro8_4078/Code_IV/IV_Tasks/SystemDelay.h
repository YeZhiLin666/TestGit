///////////////////////////////////////////////////////////////////////////////////
//File Name: SystemDelay.h
//Version:		V1.0
//Date:				2015/9/8
//Description:
//						Step�������
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once

enum
{
    SYSTEMDELAY_IDLE=0,
    SYSTEMDELAY_WAIT,
    SYSTEMDELAY_FINISH,
    SYSTEMDELAY_REST
};
class SystemDelayTask
{
public:
    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;                                                                  //���ȼ�
    static unsigned char beSystemDelayFlg;
    static unsigned long m_DelayTime;                                                       //delay time
    static unsigned char SystemDelayStatus;
    static unsigned char TriggerDelay;
    static MsTime   m_RecordDelayTime;
    static unsigned char RestDelay;
public:
    static void        DoWork(void);                                                                 //����ִ�к���
    static void        MyInit(void);                                                                 //�����ʼ��
    static void        EndTask(void);                                                                //�������
private:
    static void			GotoDelayWork(void);

};
