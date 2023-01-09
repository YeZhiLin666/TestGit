/*****************************************************************************
// 程序文件      :IRQ.h     主要用于硬件中断驱动。
// 文件描述      :为LPC4078 MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

#ifndef _IRQ_
#define _IRQ_

#include "../Data_Manipulation/Datatypes.h"

#define HIGHEST_PRIORITY	0x00    //VIC  0X00： 中断//最高优先级0级
#define LOWEST_PRIORITY		0x1F    //VIC  0X1F： 中断//最低优先级31级

class IRQFunc
{

private:
    static unsigned long IRprotect;
public:
    static UWord32  uw32_Irq[3];
    //private:

    //protected:


    //Operation
public:
    static void init_VIC( void );
    static UWord32  install_irq( IRQn_Type IntNumber, UWord32 Priority);
    //static void All_EnableIRQ(void);
    //static void All_DisableIRQ(void);

    static void DisableIRQ(unsigned long * IRQ1,unsigned long * IRQ2);
    static void EnableIRQ(unsigned long IRQ1,unsigned long IRQ2);

    //private:

    //protected:

};


#endif

