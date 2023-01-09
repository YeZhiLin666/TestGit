///////////////////////////////////////////////////////////////////////////////////
//File Name: IndependentVoltage.h
//Version:		V1.0
//Date:				2017/06/10
//Description:
//						独立恒压控制任务，用于SK从通道电压处理
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////

#pragma once

#define     BOOSTER_MIN_VOLTAGE            20
#define     SERIES_MIN_VOLTAGE             BOOSTER_MIN_VOLTAGE/2
#define     MASTER_MIN_VOLTAGE             BOOSTER_MIN_VOLTAGE/2


class IndependentVoltageCtrl_task
{
public:

    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;

public:
    static void DoWork(void);                                                                 //任务执行函数
    static void MyInit(void);                                                                 //任务初始化
    static void EndTask(void);                                                                //任务结束
private:
    static UInt16 MyStatus;	                                                                 //任务状态
public:
    static unsigned char be_ChannelRunFlg[MAXCHANNELNO];
//     static float m_CtrlValue[MAXCHANNELNO];
    static bool Series2BoosterSendVoltageEnable;

};

