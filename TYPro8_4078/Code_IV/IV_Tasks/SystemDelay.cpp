#include "../Entry/includes.h"

UInt16 SystemDelayTask::TaskID=SYSTEMDELAY_TASK;                                                                    //任务ID
UInt16 SystemDelayTask::Priority=PRIORITY8;
unsigned char SystemDelayTask::beSystemDelayFlg=false;
unsigned long SystemDelayTask::m_DelayTime;                                                       //delay time
unsigned char SystemDelayTask::SystemDelayStatus;
unsigned char SystemDelayTask::TriggerDelay;
MsTime   SystemDelayTask::m_RecordDelayTime;
unsigned char SystemDelayTask::RestDelay;

void SystemDelayTask::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SYSTEMDELAY] ++;
#endif
    MsTime mTimeDiff;
    if(beSystemDelayFlg)
    {
        switch(SystemDelayStatus)
        {
        case SYSTEMDELAY_IDLE:
            if(TriggerDelay)
            {
                SystemDelayStatus=SYSTEMDELAY_WAIT;
                //OS_ServeProcess::recordCurTime(&m_RecordDelayTime);
                m_RecordDelayTime=OS_ServeProcess::OS_Time;
            }
            break;
        case SYSTEMDELAY_WAIT:
            //MsTime TimeDiff = OS_ServeProcess::TimeDiff(m_RecordDelayTime);
            if (mTimeDiff.Second>m_DelayTime)
            {
// 					#if(FAN_SAFETY_MODE==1)
// 					if(board_config::FanSafetyCheck(0,0)==1)
// 					{
// 						break;
// 					}
// 					#endif
                m_RecordDelayTime=OS_ServeProcess::OS_Time;
                SystemDelayStatus=SYSTEMDELAY_FINISH;
            }
            break;
        case SYSTEMDELAY_FINISH:
            mTimeDiff = OS_ServeProcess::TimeDiff(m_RecordDelayTime);
            if (mTimeDiff.Second>3)
            {
                GotoDelayWork();
                SystemDelayStatus=SYSTEMDELAY_REST;
            }
            break;
        case SYSTEMDELAY_REST:
            if (RestDelay)
            {
                SystemDelayStatus=SYSTEMDELAY_IDLE;
            }
            break;
        default:
            SystemDelayStatus=SYSTEMDELAY_IDLE;
            break;
        }

    }
    else
        return;

}
void SystemDelayTask::MyInit(void)
{
    if(beSystemDelayFlg==false)  //因为硬件初始化要早于任务初始化
    {
        TriggerDelay=0;
        RestDelay=0;
        m_DelayTime=0;
        SystemDelayStatus=SYSTEMDELAY_IDLE;
    }
}
void SystemDelayTask::GotoDelayWork(void)
{
    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
    case BOARDTYPE_LBT21084:
// 	case BOARDTYPE_LBT21084VB:
    case BOARDTYPE_LBT21044:
    case BOARDTYPE_LBT20084:
        for(unsigned char ChannelIdx = 0; ChannelIdx<MAXCHANNELNO; ChannelIdx++)
        {
            LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Bits.R_VM = 1;
            LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Bits.R_CH = 0;
            LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Bits.R_CM = 0;
            LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Bits.R_CL = 0;
            LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Bits.P_R = 0;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084_Base::LBT21084_relaystatus[ChannelIdx].Reg);
        }

        break;
    default:
        break;

    }

};
