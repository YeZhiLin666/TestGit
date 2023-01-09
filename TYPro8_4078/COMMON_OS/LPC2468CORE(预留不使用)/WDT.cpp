/*****************************************************************************
// �����ļ�      :wdt.c   LPC2468  WDTģ�鹦�ܳ���
// �ļ�����      :ΪLPC2468 IV-MPU board����
// ��д����      :2009.12.01
*****************************************************************************/
#include "../../COMMON/includes.h"


//-------------------------------------------------
    UWord32       WDTFunc::uw32_wdt_counter;             //���Ź�������
    UWord64       WDTFunc::uw64_Watchdog;
    unsigned char WDTFunc::uc_Watchdog_Timer;
	unsigned char WDTFunc::uc_Watchdog_Valid;
    unsigned char WDTFunc::uc_WatchdogNo;
 //-------------------------------------------------
 

/******************************************************************************
** Function name:        WDTHandler
**
** Descriptions:         �ں˿��Ź��жϴ���
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
//   �ں˿��Ź��жϴ���
     TIMERSFunc::DELAY_SYS(24000*5); // 5ms  //  ����LPC1778��WWDT�ж��бز�����.
     //WDMOD  &= ~WDTOF;		// clear the time-out terrupt flag //
     //WDMOD |= WDEN | WDRESET;
    
     WDMOD &= ~WDTOF;  //  ����LPC1778��WWDT�ж��бز�����.

     WDTFunc::Watchdog_Feed();  //  ����LPC1778��WWDT�ж��бز�����.
    

	 //NVIC_SystemReset();
     WDMOD &= ~WDTOF;		// clear the time-out terrupt flag //
     
     VICVectAddr = 0;		// Acknowledge Interrupt //
}

/******************************************************************************
** Function name:        Watchdog_Init
**
** Descriptions:         �ں˿��Ź���ʼ��
**                               
** parameters:           None   
**                                      
** Returned value:       �ɹ���ʼ������1
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
unsigned char WDTFunc::Watchdog_Init( void )
{
//   �ں˿��Ź���ʼ��
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
** Descriptions:         ι�ں˿��Ź�
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
    //  ι�ں˿��Ź�
    //   Feed watchdog timer to prevent it from timeout
    WDFEED = 0xAA;		//Feeding sequence //
    WDFEED = 0x55; 
}


/******************************************************************************
**                            End Of File
******************************************************************************/
