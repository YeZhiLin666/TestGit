
#include "../../COMMON/includes.h"

unsigned long IRQFunc::IRprotect;
UWord32  IRQFunc::uw32_Irq[3];

void IRQFunc::init_VIC(void) 
{
    // 复位中断各控制器
   	UWord32 i = 0;
	UWord32 *vect_addr, *vect_cntl;

	/* initialize VIC*/
	VICIntEnClr = 0xffffffff;	  //disable
	VICVectAddr = 0;		      //向量地址寄存器，当发生中断时，保存时ISR地址
	VICIntSelect = 0;			  //中断选择寄存器，当发生中断时，该位对应为1

	/* set all the vector and vector control register to 0 */
	for ( i = 0; i < VIC_SIZE; i++ )
	{
		vect_addr = (UWord32 *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + i*4);
		vect_cntl = (UWord32 *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + i*4);
		*vect_addr = 0x0;	  //复位  F100 -17C
		*vect_cntl = 0xF;	  //复位  F200 -27C
	}
}

UWord32 IRQFunc::install_irq( UWord32 IntNumber, void *HandlerAddr, UWord32 Priority )
{
    // 设置中断号，中断入口地址，中断优先级,快速中断最快响应
	UWord32 *vect_addr;
	UWord32 *vect_cntl;
    //test=IntNumber;
	VICIntEnClr = 1 << IntNumber;	/* Disable Interrupt */
/*	if (test>32)//VIC_SIZE )
	{
		return (FALSE);
	}
	else
	{*/
		/* find first un-assigned VIC address for the handler */
		vect_addr = (UWord32 *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber*4);		//中断向量地址
		vect_cntl = (UWord32 *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber*4);		//优先级
		*vect_addr = (UWord32)HandlerAddr;	/* set interrupt vector */
		*vect_cntl = Priority;
		VICIntEnable = 1 << IntNumber;	/* Enable Interrupt */	     //中断使能
		return( TRUE );
	//}
}
void IRQFunc::All_EnableIRQ(void)
{
       VICIntEnable=IRQFunc::IRprotect;        //resume IR
}


void IRQFunc::All_DisableIRQ(void)
{
       IRQFunc::IRprotect=VICIntEnable;      //protect IR      
       VICIntEnClr=0xFFFFFFFF;     //disable IR       we must disable interupt here,or it will fail IAP
}




/*********************************************************************************
**                            End Of File
*********************************************************************************/
