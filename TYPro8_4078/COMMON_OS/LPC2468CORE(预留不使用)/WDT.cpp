/*****************************************************************************
// 程序文件      :wdt.c   LPC2468  WDT模块功能程序
// 文件描述      :为LPC2468 IV-MPU board服务
// 编写日期      :2009.12.01
*****************************************************************************/
#include "../../COMMON/includes.h"


//-------------------------------------------------
    UWord32       WDTFunc::uw32_wdt_counter;             //看门狗计数器
    UWord64       WDTFunc::uw64_Watchdog;
    unsigned char WDTFunc::uc_Watchdog_Timer;
	unsigned char WDTFunc::uc_Watchdog_Valid;
    unsigned char WDTFunc::uc_WatchdogNo;
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
void WDTHandler(void) __irq 
{
//   内核看门狗中断处理
     TIMERSFunc::DELAY_SYS(24000*5); // 5ms  //  对于LPC1778的WWDT中断中必不可少.
     //WDMOD  &= ~WDTOF;		// clear the time-out terrupt flag //
     //WDMOD |= WDEN | WDRESET;
    
     WDMOD &= ~WDTOF;  //  对于LPC1778的WWDT中断中必不可少.

     WDTFunc::Watchdog_Feed();  //  对于LPC1778的WWDT中断中必不可少.
    

	 //NVIC_SystemReset();
     WDMOD &= ~WDTOF;		// clear the time-out terrupt flag //
     
     VICVectAddr = 0;		// Acknowledge Interrupt //
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

    WDTC = WDT_FEED_VALUE;	// once WDEN is set, the WDT will start after feeding //
    
    //LPC_WDT->MOD |= WDEN | WDRESET;
    WDMOD |= WDEN;

	WDMOD &= ~WDTOF;		// clear the time-out terrupt flag //

    WDTFunc::Watchdog_Feed();

	// Install interrupt handler //	
	IRQFunc::install_irq( WDT_INT,(void *) WDTHandler, HIGHEST_PRIORITY + 4);
    

    //WDMOD |= WDEN | WDRESET;
	WDMOD |= WDEN;
	
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
    //  喂内核看门狗
    //   Feed watchdog timer to prevent it from timeout
    WDFEED = 0xAA;		//Feeding sequence //
    WDFEED = 0x55; 
}


/******************************************************************************
**                            End Of File
******************************************************************************/
