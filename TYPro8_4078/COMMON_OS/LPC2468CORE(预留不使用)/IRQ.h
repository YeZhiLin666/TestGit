#ifndef _IRQ_
#define _IRQ_

#include "../../COMMON/Datatypes.h"

//#define HIGHEST_PRIORITY	0x00    //VIC  0X00： 中断//最高优先级0级
//#define LOWEST_PRIORITY		0x1F    //VIC  0X1F： 中断//最低优先级31级

class IRQFunc
{
	private:
		static unsigned long IRprotect;
	public:
	static UWord32  uw32_Irq[3];
    static void init_VIC(void); 
	static UWord32 install_irq( UWord32 IntNumber, void *HandlerAddr, UWord32 Priority );
	static void All_EnableIRQ(void);
	static void All_DisableIRQ(void);
};
#endif
