/*****************************************************************************
// �����ļ�      :IRQ.h     ��Ҫ����Ӳ���ж�������
// �ļ�����      :ΪLPC4078 MCU board����
// ��д����      :2012.07.11
*****************************************************************************/

#ifndef _IRQ_
#define _IRQ_

#include "../Data_Manipulation/Datatypes.h"

#define HIGHEST_PRIORITY	0x00    //VIC  0X00�� �ж�//������ȼ�0��
#define LOWEST_PRIORITY		0x1F    //VIC  0X1F�� �ж�//������ȼ�31��

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

