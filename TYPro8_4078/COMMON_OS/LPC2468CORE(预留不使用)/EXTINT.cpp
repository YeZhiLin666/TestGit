/*****************************************************************************
// 程序文件      :extint.c     LPC2378  EINT模块功能程序
// 文件描述      :为LPC2468 IV-MPU board服务
// 编写日期      :2009.12.01
*****************************************************************************/
#include "../../COMMON/includes.h"


//-------------------------------------------------
    unsigned char EXTINTFunc::system_error; //no error	
//-------------------------------------------------


//class EXTINTFunc
unsigned char EXTINTFunc::MyIOCount =2;

//--------------------------------------------------------------------------------------
	 // Pin configuration:
   	 //!<  External interrupt 0, P2.10 >//
   	 //!<  External interrupt 1, P2.11 >//
//--------------------------------------------------------------------------------------

unsigned char EXTINTFunc::MyIO[2][4] =     //[Port],[pin],[mod],
{
	{2,10,PM_MOD,0},
	{2,11,PM_MOD,0}
};

/******************************************************************************
** Function name:        EINT0_Handler
**
** Descriptions:         外部中断处理程序，用来做同步信号中断
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                             

******************************************************************************/
/*void EINT2_IRQHandler(void)
{
    // external INT handler    外部中断0中断处理
        EXTINT = EINT2;		           //clear interrupt //
    IO2_INT_CLR   |=(1<<12);
}*/

/******************************************************************************
** Function name:        EINT1_Handler
**
** Descriptions:         外部中断1处理程序，用来做同步信号中断
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                             

******************************************************************************/
void EINT3_Handler (void)
{
//   external INT handler    外部中断3中断处理

	UWord32 ENback; //keep the current interrupt enalbe information	
	
	EXTINT = EINT3;		           //clear interrupt //
	IO2_INT_CLR   |= (1<<13);

    ENback = VICIntEnable; //keep the current interrupt enalbe information	
    
	VICIntEnClr = 0xFFFFFFFF; // disable all interrupt, 1 << EINT3_INT;	/* Disable EINT3 Interrupt */
	
	//Hardware_Extint::system_error = 1; 

//   	IVCH_B_SetLock(1);
//	//flags: to trun off channel
//	uc_CTRL[0] = IVCH_NO_CTRL;
	

	VICVectAddr = 0;		    // Acknowledge Interrupt // i.e. VICAddress 
	VICIntEnable = ENback; // Enable the interrupt which was enabled before. 1 <<	EINT3_INT; // Enable EINT3
}


/******************************************************************************
** Function name:        EINT_Init
**
** Descriptions:         初始化外中断
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void EXTINTFunc::EINT_Init(void)
{
    /*  每个项目的外中断各不相同, 有的有，有的没有。
       根据是否使用同步SYNCSTART 功能来确定是否需要初始化
       请在初始中标明用途及对应的注释*/
    //#ifdef _SYNCSTART_
		EINT_2and3();/* 不支持同步不用外中断，屏蔽处理*/
	//#endif
}

/******************************************************************************
** Function name:        EINTInit
**
** Descriptions:         初始化外中断内部函数
**                               
** parameters:           None
**                                      
** Returned value:       成功初始化返回TRUE，未成功初始化返回FALSE
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:       2010/03/02                                                                                                           

******************************************************************************/
void EXTINTFunc::EINT_2and3(void)
{
   // Initialize external interrupt pin and  install interrupt handler  
   // Returned value: true or false, return false if the interrupt  handler can't be installed to the VIC table.
   //  外部中断初始化

    PINSEL4 &=  ~0x01000000;	/* clear P2.12 as EINT2 */
    PINSEL4 |=     0x01000000;	/* set P2.12 as EINT2 */

	PINSEL4 &=  ~0x04000000;	/* clear P2.13 as EINT3 */
    PINSEL4 |=     0x04000000;	/* set P2.13 as EINT3 */

    IO2_INT_EN_R = 0x00000000;	/* Port2.12  is rising edge. */
    IO2_INT_EN_F = 0x00001000;	/* Port2.12  is falling edge. */

	IO2_INT_EN_R = 0x00000000;	/* Port2.13  is rising edge. */
    IO2_INT_EN_F = 0x00002000;	/* Port2.13  is falling edge. */
	
    
    EXTMODE = EINT2_EDGE;	/* INT2 edge trigger *///模式1为边沿触发；0为水平沿触发
    EXTPOLAR = 0x00000000;			/* INT is falling edge by default */  //极性1为上升沿；0为下降沿
	
	EXTMODE &= EINT3_LEVEL;	 //Level sensitive for EINT3
    EXTPOLAR = 0x00000000;	/* INT3 low active *///模式1: high active；0: low active
   
    EXTINT = EINT2;		           //clear interrupt //
    IO2_INT_CLR |= (1 << 12);

    EXTINT = EINT3;		           //clear interrupt eint3 //
    IO2_INT_CLR |= (1 << 13);

    FIO3CLR |=(1<<CTRL_EINT2);                 //control EINT2
    FIO3CLR |=(1<<CTRL_EINT3);                 //control EINT3
	
    IRQFunc::install_irq( EINT2_INT, (void *)EINT3_Handler, HIGHEST_PRIORITY);  //0: normal interrupt, 1: Fast interrupt

    TIMERSFunc::DELAY_SYS( 3000 ); //delay 0.5ms	  

}

/******************************************************************************
**                            End Of File
******************************************************************************/

