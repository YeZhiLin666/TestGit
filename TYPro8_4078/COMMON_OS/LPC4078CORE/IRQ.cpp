/*****************************************************************************
// �����ļ�      :IRQ.cpp     ��Ҫ����Ӳ���ж�������
// �ļ�����      :ΪLPC4078 MCU board����
// ��д����      :2012.07.11
*****************************************************************************/

#include "includes.h"

UWord32  IRQFunc::uw32_Irq[3];
UWord32  u32_Disable;
UWord32  u32_Enable;

/******************************************************************************
** Function name:        init_VIC
**
** Descriptions:         �ж�������ʼ��
**
** parameters:           None
**
** Returned value:       None
**
** Created Date:         2012/04/06
**
** Revision Dates:

******************************************************************************/
void IRQFunc::init_VIC(void)
{
    // ��λ�жϸ�������
    UWord32 uw32_i = 0;

    // set all the vector control priority register to Min //
    for( uw32_i = 0; uw32_i <= 40; uw32_i++ )
    {
        // preemption = 1, sub-priority = 1 //
        NVIC_SetPriority((IRQn_Type) uw32_i, LOWEST_PRIORITY);
    }

    for( uw32_i = 0; uw32_i < 3; uw32_i++ )
        uw32_Irq[uw32_i] = 0;
    NVIC_SetPriorityGrouping(0x07);     //�������ȼ���

    u32_Disable = 0;
    u32_Enable = 0;
}

/******************************************************************************
** Function name:        install_irq
**
** Descriptions:         �ж�������װ
**
** parameters:           IntNumber    Priority
**
** Returned value:       TRUE
**
** Created Date:         2012/04/06
**
** Revision Dates:

******************************************************************************/
UWord32 IRQFunc::install_irq( IRQn_Type IntNumber,UWord32  Priority)
{
    // �����жϺţ��ж���ڵ�ַ���ж����ȼ�,�����ж������Ӧ
    // preemption = 1, sub-priority = 1 //
    NVIC_SetPriority(IntNumber, Priority);
    // Enable interrupt for timer 0 //
    NVIC_EnableIRQ(IntNumber);
    return( TRUE );
}

void IRQFunc::DisableIRQ(unsigned long * IRQ1,unsigned long * IRQ2)
{
    *IRQ1 = NVIC->ISER[0] ;
    *IRQ2 = (NVIC->ISER[1] & 0X01FFFFFF);
    NVIC->ICER[0] = (unsigned long)(~(1 << 17)); //0XFFFFFFFF;
    NVIC->ICER[1] = 0X01FFFFFF;
    u32_Disable++;
}

void IRQFunc::EnableIRQ(unsigned long IRQ1,unsigned long IRQ2)
{
    NVIC->ICER[0] = (unsigned long)(~(1 << 17)); //0XFFFFFFFF;
    NVIC->ICER[1] = 0X01FFFFFF;
    NVIC->ISER[0] = IRQ1;
    NVIC->ISER[1] = (IRQ2 & 0X01FFFFFF);
    u32_Enable++;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/




