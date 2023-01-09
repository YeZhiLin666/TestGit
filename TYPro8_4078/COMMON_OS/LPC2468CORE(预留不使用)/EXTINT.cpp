/*****************************************************************************
// �����ļ�      :extint.c     LPC2378  EINTģ�鹦�ܳ���
// �ļ�����      :ΪLPC2468 IV-MPU board����
// ��д����      :2009.12.01
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
** Descriptions:         �ⲿ�жϴ������������ͬ���ź��ж�
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
    // external INT handler    �ⲿ�ж�0�жϴ���
        EXTINT = EINT2;		           //clear interrupt //
    IO2_INT_CLR   |=(1<<12);
}*/

/******************************************************************************
** Function name:        EINT1_Handler
**
** Descriptions:         �ⲿ�ж�1�������������ͬ���ź��ж�
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
//   external INT handler    �ⲿ�ж�3�жϴ���

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
** Descriptions:         ��ʼ�����ж�
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
    /*  ÿ����Ŀ�����жϸ�����ͬ, �е��У��е�û�С�
       �����Ƿ�ʹ��ͬ��SYNCSTART ������ȷ���Ƿ���Ҫ��ʼ��
       ���ڳ�ʼ�б�����;����Ӧ��ע��*/
    //#ifdef _SYNCSTART_
		EINT_2and3();/* ��֧��ͬ���������жϣ����δ���*/
	//#endif
}

/******************************************************************************
** Function name:        EINTInit
**
** Descriptions:         ��ʼ�����ж��ڲ�����
**                               
** parameters:           None
**                                      
** Returned value:       �ɹ���ʼ������TRUE��δ�ɹ���ʼ������FALSE
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:       2010/03/02                                                                                                           

******************************************************************************/
void EXTINTFunc::EINT_2and3(void)
{
   // Initialize external interrupt pin and  install interrupt handler  
   // Returned value: true or false, return false if the interrupt  handler can't be installed to the VIC table.
   //  �ⲿ�жϳ�ʼ��

    PINSEL4 &=  ~0x01000000;	/* clear P2.12 as EINT2 */
    PINSEL4 |=     0x01000000;	/* set P2.12 as EINT2 */

	PINSEL4 &=  ~0x04000000;	/* clear P2.13 as EINT3 */
    PINSEL4 |=     0x04000000;	/* set P2.13 as EINT3 */

    IO2_INT_EN_R = 0x00000000;	/* Port2.12  is rising edge. */
    IO2_INT_EN_F = 0x00001000;	/* Port2.12  is falling edge. */

	IO2_INT_EN_R = 0x00000000;	/* Port2.13  is rising edge. */
    IO2_INT_EN_F = 0x00002000;	/* Port2.13  is falling edge. */
	
    
    EXTMODE = EINT2_EDGE;	/* INT2 edge trigger *///ģʽ1Ϊ���ش�����0Ϊˮƽ�ش���
    EXTPOLAR = 0x00000000;			/* INT is falling edge by default */  //����1Ϊ�����أ�0Ϊ�½���
	
	EXTMODE &= EINT3_LEVEL;	 //Level sensitive for EINT3
    EXTPOLAR = 0x00000000;	/* INT3 low active *///ģʽ1: high active��0: low active
   
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

