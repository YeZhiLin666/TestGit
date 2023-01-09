/*****************************************************************************
// 程序文件      :wdt.c   LPC4078  WDT模块功能程序
// 文件描述      :LPC4078 IV-MPU board服务
// 编写日期      :2012.12.21
*****************************************************************************/
#include "includes.h"

//-------------------------------------------------
// UWord32       WDTFunc::uw32_wdt_counter;             //看门狗计数器
// UWord64       WDTFunc::uw64_Watchdog;
// unsigned char WDTFunc::uc_Watchdog_Timer;
// unsigned char WDTFunc::uc_Watchdog_Valid;
// unsigned char WDTFunc::uc_WatchdogNo;
//-------------------------------------------------


/******************************************************************************
** Function name:        WDTHandler
**
** Descriptions:         内核看门狗中断处理
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
void WDT_IRQHandler(void)
{
    //   内核看门狗中断处理
    TIMERSFunc::DELAY_SYS(5,1); // 5ms  //  对于LPC1778的WWDT中断中必不可少.
    Arbin_WDT->MOD.BitReg.bWDEN = 1;
    WDTFunc::Watchdog_Feed();  //  对于LPC1778的WWDT中断中必不可少.

    Arbin_WDT->MOD.BitReg.bWDTOF = 0;
}

/******************************************************************************
** Function name:        Watchdog_Init
**
** Descriptions:         内核看门狗初始化
**
** parameters:           None
**
** Returned value:       成功初始化返回1
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
unsigned char WDTFunc::Watchdog_Init( void )
{
    //   内核看门狗初始化
    //  Initialize watchdog timer, install the  watchdog timer interrupt handler
    //  Returned value:   true or false, return false if the VIC table is full
    //  and WDT interrupt handler can be  installed.

    Arbin_WDT->TC.BitReg.Count = WDT_FEED_VALUE;// once WDEN is set, the WDT will start after feeding //
    Arbin_WDT->MOD.BitReg.bWDEN = 1;
    Arbin_WDT->MOD.BitReg.bWDRESET = 1;  //Add by DKQ 06_17_2014
    Arbin_WDT->MOD.BitReg.bWDTOF = 0;// clear the time-out terrupt flag //
    WDTFunc::Watchdog_Feed();
    // Install interrupt handler //
// 	if( IRQFunc::install_irq( WDT_IRQn, HIGHEST_PRIORITY + 4) == FALSE )
// 	{
// 		return( FALSE );
// 	}

    Arbin_WDT->MOD.BitReg.bWDEN = 1;

    return( TRUE );
}

/******************************************************************************
** Function name:        Watchdog_Feed
**
** Descriptions:         喂内核看门狗
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
void WDTFunc::Watchdog_Feed( void )
{
    Arbin_WDT->FEED.Reg = 0xAA;
    Arbin_WDT->FEED.Reg = 0x55;
}
/******************************************************************************
** Function name:        ResetMcu
**
** Descriptions:         复位 MCU
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2013/06/07
**
** Revision Dates:

******************************************************************************/
void WDTFunc::ResetMcu(void)
{
    if(Arbin_WDT->MOD.BitReg.bWDEN)
    {
        Arbin_WDT->FEED.Reg = 0xAA;
        Arbin_WDT->TV.BitReg.Count = 0;
        Arbin_WDT->TC.BitReg.Count = 0;
        Arbin_WDT->FEED.Reg = 0x55;
    }
    else
    {
        Arbin_WDT->TC.BitReg.Count = 1;
        Arbin_WDT->MOD.BitReg.bWDEN = 1;
        Arbin_WDT->MOD.BitReg.bWDRESET = 1;
        Arbin_WDT->FEED.Reg = 0xAA;
        Arbin_WDT->FEED.Reg = 0x55;
    }
}


/******************************************************************************
** Function name:        Watchdog_SetLongTime
**
** Descriptions:         内核看门狗设置长时间
**
** parameters:           uw32_Time
**
** Returned value:       None
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
// void WDTFunc::Watchdog_SetLongTime( unsigned long int uw32_Time )  //for ethernet init,60s
// {
// 	Arbin_WDT->TC.BitReg.Count = uw32_Time;
// 	Arbin_WDT->FEED.Reg = 0xAA;
// 	Arbin_WDT->FEED.Reg = 0x55;
// }

/******************************************************************************
**                            End Of File
******************************************************************************/
