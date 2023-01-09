/*****************************************************************************
// 程序文件      :IVCH_Main.C     LPC4078  外部通道IVCH处理主函数程序
// 文件描述      :为LPC4078 IV-MPU board服务
// 编写日期      :2009.12.01
*****************************************************************************/
#include "includes.h"


extern void send_ICMP_TimeRequest(void);    //ICMP
//-------------------------------------------------
UWord32 RTCFunc::uw32_MSTimeCount;                       //  用于秒的毫秒计数器
unsigned char RTCFunc::uc_Time48Lock;                    //  48位计数器锁定键，1为锁
unsigned char RTCFunc::uc_SSFailNum;                     //   秒更新失败次数
RTCTime  RTCFunc::Str_LocalTime;   // 初如化 时钟
RTCTime  RTCFunc::Str_RctTemp;     // 暂存计算时间
//-------------------------------------------------


/******************************************************************************
** Function name:        RTCHandler
**
** Descriptions:         RTC中断对时程序
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/

void RTC_IRQHandler(void)
{
    //  RTC interrupt handler, it executes based on  the alarm setting
    LPC_RTC->ILR |= RTC_IRL_RTCCIF;		// clear interrupt flag //
    OS_ServeProcess::RTCHandlerISR();
}

/******************************************************************************
** Function name:		RTC_InitAndStart
**
** Descriptions:		RTC 初始化并启用
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void RTCFunc::RTC_InitAndStart()
// {
// #ifdef _SYSTEM_RTC_
// 	RTCInit();
// 	RTCSetTime();		/* Set local time */
// 	RTCSetAlarm();		/* set alarm time  */
// 	RTCStart();
// #endif

// }

/******************************************************************************
** Function name:        RTCInit
**
** Descriptions:         RTC 初始化
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
void RTCFunc::RTCInit( void )
{
    Arbin_SC->PCONP.BitReg.bPCRTC = 1;
    Arbin_RTC->AMR.Reg = 0;
    Arbin_RTC->CIIR.Reg = 0;
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.Reg = 0;
    Arbin_RTC->ILR.Reg = 0;
    Arbin_RTC->SEC.BitReg.SECONDS = 0;  //OS_ServeProcess::OS_Time.Second;
    Arbin_RTC->MIN.BitReg.MINUTES = 0;  //OS_ServeProcess::OS_Time.Minute;
    Arbin_RTC->HOUR.BitReg.HOURS = 0;  //OS_ServeProcess::OS_Time.Hour;
    Arbin_RTC->DOY.BitReg.DOY = 0;     //OS_ServeProcess::OS_Time.Day;
    Arbin_RTC->YEAR.BitReg.YEAR = 2013;//OS_ServeProcess::OS_Time.Year;
    Arbin_RTC->ILR.BitReg.RTCCIF = 1;
    Arbin_RTC->CIIR.BitReg.bIMSEC = 1;
    Arbin_RTC->CCR.BitReg.CCALEN = 1;
    Arbin_RTC->CALIBRATION.BitReg.CALVAL = 0;
    Arbin_RTC->CALIBRATION.BitReg.CALDIR = 0;
    IRQFunc::install_irq( RTC_IRQn, HIGHEST_PRIORITY);// + 5

    uw32_MSTimeCount = 0;
    RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStart
**
** Descriptions:		RTC 启用
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
void RTCFunc::RTCStart( void )
{
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.Reg = 0;
    Arbin_RTC->CCR.BitReg.CLKEN = 1;
    Arbin_RTC->ILR.BitReg.RTCCIF = 1;
    Arbin_RTC->CCR.BitReg.CCALEN = 1;
    Arbin_RTC->CALIBRATION.BitReg.CALVAL = 0;
    Arbin_RTC->CALIBRATION.BitReg.CALDIR = 0;
    uw32_MSTimeCount = 0;
    RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStop
**
** Descriptions:		RTC 停止
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void RTCFunc::RTCStop( void )
// {
// 	Arbin_RTC->CCR.BitReg.CLKEN = 0;
// }

/******************************************************************************
** Function name:		RTC_CTCReset
**
** Descriptions:		RTC 复位
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
void RTCFunc::RTC_CTCReset( void )
{
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.BitReg.CTCRST ^= 1;       ////////////
    LPC_RTC->ILR |= RTC_IRL_RTCCIF;		// clear interrupt flag //
}

/******************************************************************************
** Function name:		RTCSetTime
**
** Descriptions:		RTC 设置时间
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void RTCFunc::RTCSetTime( void)
// {
// 	Arbin_RTC->SEC.BitReg.SECONDS = Str_LocalTime.RTC_Sec;
// 	Arbin_RTC->MIN.BitReg.MINUTES = Str_LocalTime.RTC_Min;
// 	Arbin_RTC->HOUR.BitReg.HOURS = Str_LocalTime.RTC_Hour;
// 	Arbin_RTC->DOY.BitReg.DOY = Str_LocalTime.RTC_Yday;
// 	Arbin_RTC->YEAR.BitReg.YEAR = Str_LocalTime.RTC_Year;
// }

/******************************************************************************
** Function name:		RTCSetAlarm
**
** Descriptions:		RTC 设置报警
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void RTCFunc::RTCSetAlarm( void)
// {
// 	Arbin_RTC->ALSEC.BitReg.SECONDS = 59;
// 	Arbin_RTC->ALMIN.BitReg.MINUTES = 59;
// 	Arbin_RTC->ALHOUR.BitReg.HOURS = 23;
// 	Arbin_RTC->ALDOM.BitReg.DOM = 31;
// 	Arbin_RTC->ALDOW.BitReg.DOW = 6;
// 	Arbin_RTC->ALDOY.BitReg.DOY = 365;
// 	Arbin_RTC->ALMON.BitReg.MONTH = 12;
// 	Arbin_RTC->ALYEAR.BitReg.YEAR = 2089;
// }

/******************************************************************************
** Function name:		RTCGetTime
**
** Descriptions:		RTC 取得时间
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void  RTCFunc::RTCGetTime( void )
// {
//     Str_LocalTime.RTC_Sec = Arbin_RTC->SEC.BitReg.SECONDS;
//     Str_LocalTime.RTC_Min = Arbin_RTC->MIN.BitReg.MINUTES;
//     Str_LocalTime.RTC_Hour = Arbin_RTC->HOUR.BitReg.HOURS;
//     Str_LocalTime.RTC_Yday = Arbin_RTC->DOY.BitReg.DOY;
//     Str_LocalTime.RTC_Year = Arbin_RTC->YEAR.BitReg.YEAR;

// }

/******************************************************************************
** Function name:		RTCSetAlarmMask
**
** Descriptions:		RTC SetAlarmMask
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
// void RTCFunc::RTCSetAlarmMask( UWord32 AlarmMask )
// {
// 	Arbin_RTC->AMR.Reg = AlarmMask;
// }


void  RTCFunc::StopRTC( void )
{
    NVIC_DisableIRQ(RTC_IRQn);
    Arbin_RTC->CCR.BitReg.CLKEN = 0;
    RTC_CTCReset();
}

void  RTCFunc::ReStartRTC( void )
{
    RTC_CTCReset();
    Arbin_RTC->CCR.BitReg.CLKEN = 1;
    NVIC_EnableIRQ(RTC_IRQn);
}


/*****************************************************************************
**                            End Of File
******************************************************************************/

