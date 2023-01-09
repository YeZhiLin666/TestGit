///////////////////////////////////////////////////////////////////////////////////
//File Name: IndependentVoltage.h
//Version:		V1.0
//Date:				2017/06/10
//Description:
//						������ѹ������������SK��ͨ����ѹ����
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////

#pragma once

#define     BOOSTER_MIN_VOLTAGE            20
#define     SERIES_MIN_VOLTAGE             BOOSTER_MIN_VOLTAGE/2
#define     MASTER_MIN_VOLTAGE             BOOSTER_MIN_VOLTAGE/2


class IndependentVoltageCtrl_task
{
public:

    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;

public:
    static void DoWork(void);                                                                 //����ִ�к���
    static void MyInit(void);                                                                 //�����ʼ��
    static void EndTask(void);                                                                //�������
private:
    static UInt16 MyStatus;	                                                                 //����״̬
public:
    static unsigned char be_ChannelRunFlg[MAXCHANNELNO];
//     static float m_CtrlValue[MAXCHANNELNO];
    static bool Series2BoosterSendVoltageEnable;

};

