#include "includes.h"

/*****************系统运行闪烁指示灯******************************************
** Start:         系统运行闪烁指示灯
******************************************************************************/
#pragma arm section rwdata = "SRAMSPECIAL",zidata = "SRAMSPECIAL"      // 设置变量存储在外部扩展存储器中     DRAM   0XA0000000  0x00000040
UWord32          LEDFunc::uw32_GotoBootMode;   //必须放在第一位
unsigned char    LEDFunc::uc_NOP[60];  //   空出60字节空间外存
#pragma arm section //结束外存变量定义  
UWord32          LEDFunc::uw16_LedTimer;
UInt16           LEDFunc::Priority = PRIORITYBG;
UInt16           LEDFunc::TaskID = LED_TASK;    //DISPOSABLE_TASK;    comment out chen 20130817
unsigned char    LEDFunc::uc_TimeLED;
//                                         Port      Bits       PortType           Function               Address
const St_Port    LEDFunc::Pin_HeartBeat = {0,        31,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
unsigned short 	 LEDFunc::m_SysStatus=0;
void LEDFunc::Init(void)
{
    LEDFunc::uw32_GotoBootMode = 0;
    LEDFunc::uc_NOP[0] = 0;
    ControlPin::SetIObyCP(Pin_HeartBeat);
}
void LEDFunc::DoWork(void)
{   //系统运行闪烁指示灯



#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_LED]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_LED);
#endif
    unsigned long w16_Second = CommonFunc::TimeDiffInSecond(uw16_LedTimer);//OS_ServeProcess::OS_Time.Second - uw16_LedTimer;
    if(w16_Second > 0)
    {
        if(w16_Second > 1)//  1s
        {
            if(board_config::FeedWatchDog != 0)
                board_config::FeedWatchDog();   //1s喂一次硬件狗 add by qjm 20151026
            if(uc_TimeLED != 0)
            {
                ControlPin::SetMeHigh(Pin_HeartBeat);
                uc_TimeLED = 0;
#if (EXTERNAL_HIGHT_LONG_LED == 1)
#if (USE_AS_AUXMCU == 0 )
                //if(Schedule_Manager_task::M_SysALarmEmergencyFlg==0)
                //{
                for(unsigned char uc_channel=0; uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++)
                {
                    if(ChannelInfo_Manager::ChannelInfo[uc_channel].ScheduleStatus==RUNNING)
                    {
                        m_SysStatus|=(1<<uc_channel);
                    }
                    else
                        m_SysStatus&=~(1<<uc_channel);

                }

                if(	m_SysStatus>0)
                {
                    CANBusParse::IV_SendTOSysCtrl_RUN();
                }
                else
                    CANBusParse::IV_SendTOSysCtrl_STOP();
                //}

#endif
#endif
            }
            else
            {
                ControlPin::SetMeLow(Pin_HeartBeat);
                uc_TimeLED = 1;
            }
            uw16_LedTimer = GetSystemTimeInSecond();
        }
    }
    else
        uw16_LedTimer = GetSystemTimeInSecond();

// #if  (USE_AS_AUXMCU == 0)
// if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM )
// {
// 	switch(PWM_Base::PWM_LEDRunStatus)
// 	{
// 		case SYSTEM_IDLE:
// 			PWM_Base::LEDShow(0,1,0,0);
// 			break;
// 		case SYSTEM_RUN:
// 			PWM_Base::LEDShow(0,0,1,0);
// 			break;
// 		case SYSTEM_ALARM:
// 			PWM_Base::LEDShow(1,0,0,1);
// 			break;
// 			default:
// 				break;
// 	}

// }
// #endif

#if MCU_WATCHDOG_ENABLE == 1
    WDTFunc::Watchdog_Feed();  //Add by DKQ 06_17_2014
#endif

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_LED] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_LED);
#endif
}


/*****************系统运行闪烁指示灯******************************************
** End:         系统运行闪烁指示灯
******************************************************************************/


