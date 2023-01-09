
#ifndef    _SYS_2468_
#define    _SYS_2468_   

////target PLL////
#define KEIL_BOARD_LPC23XX				1
#define EA_BOARD_LPC24XX				0

/* If USB device is used, the CCLK setting needs to be 57.6Mhz, CCO will be 288Mhz
to get precise USB clock 48Mhz. If USB is not used, you set any clock you want
based on the table below. If you want to use USB, change "define USE_USB" from 0 to 1 */
 
#define	USE_USB				  0

//This segment should not be modified
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* PLL Setting Table Matrix */
/* 	
	Main Osc.	CCLKCFG		Fcco		Fcclk 		M 	N
	12Mhz		29		300Mhz		10Mhz		24	1
	12Mhz		35		360Mhz		10Mhz		14	0					
	12Mhz		27		336Mhz		12Mhz		13	0
	12Mhz		14		300Mhz		20Mhz		24	1		
	12Mhz		17		360Mhz		20Mhz		14	0
	12Mhz		13		336Mhz		24Mhz		13	0
	12Mhz		11		300Mhz		25Mhz		24	1   
	12Mhz		9		300Mhz		30Mhz		24	1
	12Mhz		11		360Mhz		30Mhz		14	0
	12Mhz		9		320Mhz		32Mhz		39	2
	12Mhz		9		350Mhz		35Mhz		174	11
	12Mhz		7		312Mhz		39Mhz		12	0
 	12Mhz		8		360Mhz		40Mhz		14	0 
	12Mhz		7		360Mhz		45Mhz		14	0
	12Mhz		6		336Mhz		48Mhz		13	0  
//	12Mhz		5		300Mhz		50Mhz		24	1      //
	12Mhz		5		312Mhz		52Mhz		12	0
	12Mhz		5		336Mhz		56Mhz		13	0		
// 	12Mhz		4		300Mhz		60Mhz		24	1	//	
  	12Mhz		4		320Mhz		64Mhz		39	2
	12Mhz		4		350Mhz		70Mhz		174	11
	12Mhz		4		360Mhz		72Mhz		14	0		
	12Mhz		3		300Mhz		75Mhz		24	1
	12Mhz		3		312Mhz		78Mhz		12	0  
	12Mhz		3		320Mhz		80Mhz		39	2
	12Mhz		3		336Mhz		84Mhz		13	0 
//	12Mhz		3		288Mhz		72Mhz		11	1      //
*/

/*	NOTE: 
	1.The Fcco of the LPC2378 can not extend 290,if extend this value ,it can not be download 
	the code by ULINK2 Jtag.Only use the uart (ISP)TO download the code once to active it.
    2. fcco=288M,and Fclk=288M/4=72Mhz   */

#if USE_USB		/* 1 is USB, 0 is non-USB related */  
	//-----------------------------------------------------
	// Fcclk = 57.6Mhz, Fcco = 288Mhz, and USB 48Mhz //
	#define PLL_MValue			11
	#define PLL_NValue			0
	#define CCLKDivValue		4
	#define USBCLKDivValue		5
	//pllclk = 288M
	//cclk = 288/CCLKDivValue   =72M
	//pclk = cclk/    =	 18M
	// System configuration: Fosc, Fcclk, Fcco, Fpclk must be defined //
	// PLL input Crystal frequence range 4KHz~20MHz. //
	#define Fosc	12000000
	// System frequence,should be less than 80MHz. //
	#define Fcclk	57600000
	#define Fcco	288000000
	//-----------------------------------------------------

	#else

	/*
	//-----------------------------------------------------
	// Fcclk = 50Mhz, Fcco = 300Mhz,USB  None//
	#define PLL_MValue			24
	#define PLL_NValue			1
	#define CCLKDivValue		5
	#define USBCLKDivValue		6
	// System configuration: Fosc, Fcclk, Fcco, Fpclk must be defined //
	// PLL input Crystal frequence range 4KHz~20MHz. //
	#define Fosc	12000000
	// System frequence,should be less than 80MHz. //
	#define Fcclk	50000000
	#define Fcco	300000000
	//-----------------------------------------------------
	*/
	/*
	//-----------------------------------------------------
	// Fcclk = 60Mhz, Fcco = 300Mhz,USB  None //
	#define PLL_MValue		24
	#define PLL_NValue		1
	#define CCLKDivValue		4
	#define USBCLKDivValue		5
	#define Fosc	12000000
	#define Fcclk	60000000
	#define Fcco	300000000
	*/

	//-----------------------------------------------------
	//Fcclk = 72Mhz, Fcco = 288Mhz, and USB 48Mhz //
	#define PLL_MValue			11
	#define PLL_NValue			0
	#define CCLKDivValue		3
	#define USBCLKDivValue		5
	#define Fosc	12000000
	#define Fcclk	72000000
	#define Fcco	288000000
	//-----------------------------------------------------

	#endif

	/* APB clock frequence , must be 1/2/4 multiples of ( Fcclk/4 ). */
	/* If USB is enabled, the minimum APB must be greater than 16Mhz */ 
	#if USE_USB
	#define Fpclk	(Fcclk / 2)
	#else
	#define Fpclk	(Fcclk / 4)
	//#define Fpclk	(Fcclk / 2)       //为提CAN提速到1M使用
	#endif
////end target PLL////



////IRQ////
#define I_Bit			0x80
#define F_Bit			0x40

#define SYS32Mode		0x1F
#define IRQ32Mode		0x12
#define FIQ32Mode		0x11

#define HIGHEST_PRIORITY	0x01
#define LOWEST_PRIORITY		0x0F

#define	WDT_INT			0
#define SWI_INT			1
#define ARM_CORE0_INT		2
#define	ARM_CORE1_INT		3
#define	TIMER0_INT		4
#define TIMER1_INT		5
#define UART0_INT		6
#define	UART1_INT		7
#define	PWM0_1_INT		8
#define I2C0_INT		9
#define SPI0_INT		10			/* SPI and SSP0 share VIC slot */
#define SSP0_INT		10
#define	SSP1_INT		11
#define	PLL_INT			12
#define RTC_INT			13
#define EINT0_INT		14
#define EINT1_INT		15
#define EINT2_INT		16
#define EINT3_INT		17
#define	ADC0_INT		18
#define I2C1_INT		19
#define BOD_INT			20
#define EMAC_INT		21
#define USB_INT			22
#define CAN_INT			23
#define MCI_INT			24
#define GPDMA_INT		25
#define TIMER2_INT		26
#define TIMER3_INT		27
#define UART2_INT		28
#define UART3_INT		29
#define I2C2_INT		30
#define I2S_INT			31

#define VIC_SIZE		32

#define VECT_ADDR_INDEX	0x100
#define VECT_CNTL_INDEX 0x200

/* Be aware that, from compiler to compiler, nested interrupt will have to
be handled differently. More details can be found in Philips LPC2000
family app-note AN10381 */

/* unlike Keil CARM Compiler, in ARM's RealView compiler, don't save and 
restore registers into the stack in RVD as the compiler does that for you. 
See RVD ARM compiler Inline and embedded assemblers, "Rules for 
using __asm and asm keywords. */
static UWord32 sysreg;		/* used as LR register */
#define IENABLE __asm { MRS sysreg, SPSR; MSR CPSR_c, #SYS32Mode }
#define IDISABLE __asm { MSR CPSR_c, #(IRQ32Mode|I_Bit); MSR SPSR_cxsf, sysreg }
////end IRQ////


class sys_2468
{
    private:
	
	static void ConfigPLL(void);    //配置锁相环
	static void GPIOResetInit(void);    //reset GPIO
	static void init_VIC(void);    //中断向量

    
    public:
    //RTC
	//timer
	//install_irq
	static void TargetResetInit(void);    //研究下存储器映射和外部存储器

	//static UWord32 install_irq( UWord32 IntNumber, void *HandlerAddr, UWord32 Priority );

	////////////////////////////////////////////////////////////////////////////////////////////////
	static void SDRAMInit(void);

};
//PLL  主频
//IRQ
//



#endif


