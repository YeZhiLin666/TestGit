/*****************************************************************************
// �����ļ�      :wdt.c   LPC4078  WDTģ�鹦�ܳ���
// �ļ�����      :LPC4078 IV-MPU board����
// ��д����      :2012.12.21
*****************************************************************************/
#include "includes.h"

//-------------------------------------------------
// UWord32       WDTFunc::uw32_wdt_counter;             //���Ź�������
// UWord64       WDTFunc::uw64_Watchdog;
// unsigned char WDTFunc::uc_Watchdog_Timer;
// unsigned char WDTFunc::uc_Watchdog_Valid;
// unsigned char WDTFunc::uc_WatchdogNo;
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
void WDT_IRQHandler(void)
{
    //   �ں˿��Ź��жϴ���
    TIMERSFunc::DELAY_SYS(5,1); // 5ms  //  ����LPC1778��WWDT�ж��бز�����.
    Arbin_WDT->MOD.BitReg.bWDEN = 1;
    WDTFunc::Watchdog_Feed();  //  ����LPC1778��WWDT�ж��бز�����.

    Arbin_WDT->MOD.BitReg.bWDTOF = 0;
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
    Arbin_WDT->FEED.Reg = 0xAA;
    Arbin_WDT->FEED.Reg = 0x55;
}
/******************************************************************************
** Function name:        ResetMcu
**
** Descriptions:         ��λ MCU
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
** Descriptions:         �ں˿��Ź����ó�ʱ��
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
