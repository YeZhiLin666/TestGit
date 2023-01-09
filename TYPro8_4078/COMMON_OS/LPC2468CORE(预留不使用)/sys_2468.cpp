
#include "../../COMMON/includes.h"

UWord32 test;
//private
void sys_2468::ConfigPLL(void)
{
	UWord32 uw32_MValue=0;
	UWord32 uw32_NValue = 0,uw32_i = 0,uw32_j = 0;

	if ( PLLSTAT & (1 << 25) )       // Check connect bit status //
	{
		PLLCON = 1;			// Enable PLL, disconnected //
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
	}

	PLLCON = 0;				//Disable PLL, disconnected //
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	SCS |= 0x20;			// Enable main OSC //
	while( !(SCS & 0x40) );	// Wait until main OSC is usable //

	CLKSRCSEL = 0x1;		// select main OSC, 12MHz, as the PLL clock source //

	PLLCFG = PLL_MValue | (PLL_NValue << 16);
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	PLLCON = 1;				// Enable PLL, disconnected //
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	CCLKCFG = CCLKDivValue;	// Set clock divider //
		#if USE_USB
			USBCLKCFG = USBCLKDivValue;		//usbclk = 288 MHz/6 = 48 MHz //
		#endif

	while ( ((PLLSTAT & (1 << 26)) == 0) );	// Check lock bit status //

	uw32_MValue = PLLSTAT & 0x00007FFF;
	uw32_NValue = (PLLSTAT & 0x00FF0000) >> 16;
	while ((uw32_MValue != PLL_MValue) && ( uw32_NValue != PLL_NValue) );

	PLLCON = 3;				// enable and connect //
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	while ( ((PLLSTAT & (1 << 25)) == 0) )	// Check connect bit status //
	{
		for(uw32_i=0;uw32_i<1;uw32_i++)
		{
			uw32_j++;
		}
		if(uw32_j>1000)
			break;
	}

	for(uw32_i=0;uw32_i<30000;uw32_i++)    //提高频率到72M后要加2000延时就能正常使用系统,不会出现跑飞。
	{
		uw32_j++;
	}
	uw32_i=uw32_j;  
}


void sys_2468::GPIOResetInit( void )
{
	// Reset all GPIO pins to default: primary function //
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	PINSEL3 = 0x00000000;
	PINSEL4 = 0x00000000;
	PINSEL5 = 0x00000000;
	PINSEL6 = 0x00000000;
	PINSEL7 = 0x00000000;
	PINSEL8 = 0x00000000;
	PINSEL9 = 0x00000000;
	PINSEL10 = 0x00000000;

	IODIR0 = 0x00000000;
	IODIR1 = 0x00000000;
	IOSET0 = 0x00000000;
	IOSET1 = 0x00000000;

	FIO0DIR = 0x00000000;
	FIO1DIR = 0x00000000;
	FIO2DIR = 0x00000000;
	FIO3DIR = 0x00000000;
	FIO4DIR = 0x00000000;

	FIO0SET = 0x00000000;
	FIO1SET = 0x00000000;
	FIO2SET = 0x00000000;
	FIO3SET = 0x00000000;
	FIO4SET = 0x00000000;
	//return;        
}

void sys_2468::init_VIC(void)
{
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
	return;
}

////end private


////public////
//Target初始化
void sys_2468::TargetResetInit(void)
{
#ifdef __DEBUG_RAM    
	MEMMAP = 0x2;			// remap to internal RAM //    重映射
#endif

#ifdef __DEBUG_FLASH    
	MEMMAP = 0x1;			// remap to internal flash //
#endif

#if USE_USB
	PCONP |= 0x80000000;		// Turn On USB PCLK //
#endif

	sys_2468::ConfigPLL();         // Configure PLL, switch from IRC to Main OSC //

	//Set system timers for each component //
	//设置外设时钟
#if (Fpclk / (Fcclk / 4)) == 1
	PCLKSEL0 = 0x00000000;	// PCLK is 1/4 CCLK //
	PCLKSEL1 = 0x00000000;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
	PCLKSEL0 = 0xAAAAAAAA;	// PCLK is 1/2 CCLK //
	PCLKSEL1 = 0xAAAAAAAA;	 
#endif
#if (Fpclk / (Fcclk / 4)) == 4
	PCLKSEL0 = 0x55555555;	//PCLK is the same as CCLK //	    //基本被除以4了
	PCLKSEL1 = 0x55555555;	
#endif

	PCLKSEL1 |= 0x00000004;	// GPIO  souce CCLK//	
	PCLKSEL1 |= 0x01000000;	// Media Control Interface  MCI  souce CCLK//	
	PCLKSEL1 |= 0x10000000;	// SYSCON souce CCLK//

	//Set memory accelerater module//
	//存储器加速模块
	MAMCR = 0;    
#if Fcclk < 20000000
	MAMTIM = 1;
#else
#if Fcclk < 40000000
	MAMTIM = 2;
#else
	MAMTIM = 3;
#endif
#endif
	MAMCR = 2;

	sys_2468::GPIOResetInit();    //GPIO reset

	sys_2468::init_VIC();    //initVIC
}

//装载IRQ的主要原因是要把ISR函数地址给到对象的寄存器上，另外赋给优先级
//直接忽略了FIRQ
//当中断发生时，按照IRQ控制器操作，寻找中断向量表，从中取出ISR程序，调用，然后返回
/*UWord32 sys_2468::install_irq( UWord32 IntNumber, void *HandlerAddr, UWord32 Priority )
{
	UWord32 *vect_addr;
	UWord32 *vect_cntl;
  test=IntNumber;
	VICIntEnClr = 1 << IntNumber;	*//* Disable Interrupt */
/*	if (test>32)//VIC_SIZE )
	{
		return (FALSE);
	}
	else
	{*/
		/* find first un-assigned VIC address for the handler */
	/*	vect_addr = (UWord32 *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber*4);		//中断向量地址
		vect_cntl = (UWord32 *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber*4);		//优先级
		*vect_addr = (UWord32)HandlerAddr;	*//* set interrupt vector */
		/**vect_cntl = Priority;
		VICIntEnable = 1 << IntNumber;	*//* Enable Interrupt */	     //中断使能
	/*	return( TRUE );
	//}
}*/

//Sdram初始化
void sys_2468::SDRAMInit(void)
{
	{
		UWord32 i, dummy = dummy;
		EMC_CTRL = 0x00000001;
		PCONP  |= 0x00000800;		// Turn On EMC PCLK //

	    EMC_STA_CFG0      = 0x00000080;
		TIMERSFunc::DELAY_SYS( 10 );

		EMC_STA_CFG1 = 0x00000081;///如果是试用版需要改0x00000081
		TIMERSFunc::DELAY_SYS( 10 );
		TIMERSFunc::DELAY_SYS( 3000 ); //delay 0.5ms	

		EMC_DYN_RP     = 2;		/* command period: 3(n+1) clock cycles */
		EMC_DYN_RAS    = 3;		/* RAS command period: 4(n+1) clock cycles */
		EMC_DYN_SREX   = 7;		/* Self-refresh period: 8(n+1) clock cycles */
		EMC_DYN_APR    = 2;		/* Data out to active: 3(n+1) clock cycles */
		EMC_DYN_DAL    = 5;		/* Data in to active: 5(n+1) clock cycles */
		EMC_DYN_WR     = 1;		/* Write recovery: 2(n+1) clock cycles */
		EMC_DYN_RC     = 5;		/* Active to Active cmd: 6(n+1) clock cycles */
		EMC_DYN_RFC    = 5;		/* Auto-refresh: 6(n+1) clock cycles */
		EMC_DYN_XSR    = 7;		/* Exit self-refresh: 8(n+1) clock cycles */
		EMC_DYN_RRD    = 1;		/* Active bank A->B: 2(n+1) clock cycles */
		EMC_DYN_MRD    = 2;		/* Load Mode to Active cmd: 3(n+1) clock cycles */

		EMC_DYN_RD_CFG = 1;		/* Command delayed strategy */

		/* Default setting, RAS latency 3 CCLKs, CAS latenty 3 CCLKs. */
		EMC_DYN_RASCAS0 = 0x00000303;
		//#if ENG_BOARD_LPC24XX		/* NXP engineering board */
		/* 256MB, 16Mx16, 4 banks, row=12, column=9 */
		//  EMC_DYN_CFG0 = 0x00000480;
		//#else						/* Embedded Artists board */
		/* 256MB, 16Mx16, 4 banks, row=13, column=9 */
		EMC_DYN_CFG0 = 0x00000680;
		//#endif
		/* 16MB, 1Mx16, 4 banks, row=11, column=8 */  //zl
		EMC_DYN_CFG1 = 0x00000080;//***************************/
		TIMERSFunc::DELAY_SYS(600000);			/* use timer 1 */

		/* Mem clock enable, CLKOUT runs, send command: NOP */
		EMC_DYN_CTRL = 0x00000183;
		TIMERSFunc::DELAY_SYS(120000);			/* use timer 1 */

		/* Send command: PRECHARGE-ALL, shortest possible refresh period */
		EMC_DYN_CTRL = 0x00000103;

		/* set 32 CCLKs between SDRAM refresh cycles */
		EMC_DYN_RFSH = 0x00000002;
		for(i = 0; i < 0x40; i++);	/* wait 128 AHB clock cycles */

		/* set 28 x 16CCLKs=448CCLK=7us between SDRAM refresh cycles */
		EMC_DYN_RFSH = 28;
	    
		/* To set mode register in SDRAM, enter mode by issue
		MODE command, after finishing, bailout and back to NORMAL mode. */    
		/* Mem clock enable, CLKOUT runs, send command: MODE */
		EMC_DYN_CTRL = 0x00000083;
		/* Set mode register in SDRAM */
		/* Mode regitster table for Micron's MT48LCxx */
		/* bit 9:   Programmed burst length(0)
		bit 8~7: Normal mode(0)
		bit 6~4: CAS latency 3
		bit 3:   Sequential(0)
		bit 2~0: Burst length is 8
		row position is 12 */
		dummy = *((volatile UWord32 *)(0XA0000000 | (0x33 << 12)));

		EMC_DYN_CTRL = 0x00000000;	  /* Send command: NORMAL */

		EMC_DYN_CFG0 |= 0x00080000;	  /* Enable buffer */
		TIMERSFunc::DELAY_SYS(6000);				  /* Use timer 1 */
	}
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/




