/*****************************************************************************
// �����ļ�      :extint.c     LPC2378  EINTģ�鹦�ܳ���
// �ļ�����      :ΪLPC2468 IV-MPU board����
// ��д����      :2009.12.01
*****************************************************************************/
#include "includes.h"


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
/*void EINT0_IRQHandler(void)
{
// external INT handler    �ⲿ�ж�0�жϴ���
LPC_SC->EXTINT   =  EXTI_EINT0_BIT_MARK;   // External Interrupt Flag enabled.//
LPC_GPIOINT->IO2IntClr |= (1 << 10);

//LPC_GPIO0->SET    |= (1 << 31);
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
void EINT3_IRQHandler (void)
{
    // external INT handler    �ⲿ�ж�1�жϴ���
    LPC_SC->EXTINT   =  EXTI_EINT1_BIT_MARK;   // External Interrupt Flag enabled.//
    LPC_GPIOINT->IO2IntClr |= (1 << 11);

    //LPC_GPIO0->SET    |= (1 << 31);
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

    /*//!<  External interrupt 0, P2.10 >//
    LPC_IOCON->P2_10 = 0X11;  // P2.10 EINT0 �� External interrupt 0 input.  // Pull-up resistor enabled.

    LPC_IOCON->P0_21 = 0X10;  // EINT1 Control.    // Pull-up resistor enabled.
    LPC_GPIO0->DIR |= (1 << 21);
    LPC_GPIO0->SET |= (1 << 21);

    LPC_SC->EXTMODE  |=  (EXTI_EINT0_BIT_MARK);   //EINTX is edge sensitive.//
    LPC_SC->EXTPOLAR &=  ~(EXTI_EINT0_BIT_MARK);   //EINTX is low-active or falling-edge sensitive.//
    LPC_SC->EXTINT   |=  (EXTI_EINT0_BIT_MARK);   // External Interrupt Flag enabled.//
    LPC_GPIOINT->IO2IntClr |= (1 << 10);

    // External Interrupt 0 Interrupt related register setting //
    if ( IRQFunc::install_irq( EINT0_IRQn, HIGHEST_PRIORITY + 1) == FALSE )
    {
    //return ;
    }*/


    //!<  External interrupt 1, P2.11 >//
    LPC_IOCON->P2_11 = 0X11;  // P2.11 EINT1 �� External interrupt 1 input.  // Pull-up resistor enabled.

    LPC_IOCON->P0_22 = 0X10;  // EINT0 Control.    // Pull-up resistor enabled.
    LPC_GPIO0->DIR |= (1 << 22);
    LPC_GPIO0->SET = (1 << 22);

    LPC_SC->EXTMODE  |=  (EXTI_EINT1_BIT_MARK);   //EINTX is edge sensitive.//
    LPC_SC->EXTPOLAR &=  ~(EXTI_EINT1_BIT_MARK);   //EINTX is low-active or falling-edge sensitive.//
    LPC_SC->EXTINT   |=  (EXTI_EINT1_BIT_MARK);   // External Interrupt Flag enabled.//
    LPC_GPIOINT->IO2IntClr |= (1 << 11);

    // External Interrupt 1 Interrupt related register setting //
// 	if ( IRQFunc::install_irq( EINT1_IRQn, HIGHEST_PRIORITY + 2) == FALSE )
// 	{
    //return ;
// 	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/

