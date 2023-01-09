#include "../../COMMON/includes.h"

//class SDRAMFunc
unsigned char SDRAMFunc::MyIOCount = 57;

//--------------------------------------------------------------------------------------
	// Pin configuration:
	// P2.14 - /EMC_CS2
	        // P2.15 - /EMC_CS3
	// P2.16 - /EMC_CAS
	// P2.17 - /EMC_RAS
	// P2.18 - EMC_CLK[0]
	// P2.19 - EMC_CLK[1]
	// P2.20 - EMC_DYCS0
	// P2.21 - EMC_DYCS1
		    // P2.22 - EMC_DYCS2
		    // P2.23 - EMC_DYCS3
	// P2.24 - EMC_CKE0
	// P2.25 - EMC_CKE1
		    // P2.26 - EMC_CKE2
		    // P2.27 - EMC_CKE3
	// P2.28 - EMC_DQM0
	// P2.29 - EMC_DQM1
			// P2.30 - EMC_DQM2
			// P2.31 - EMC_DQM3
	// P3.0-P3.31 - EMC_D[0-31]
	// P4.0-P4.23 - EMC_A[0-23]
	// P4.24 - /EMC_OE
	// P4.25 - /EMC_WE
	// P4.30 - /EMC_CS0
	// P4.31 - /EMC_CS1   

	//P3.00 --P3.15 Data 
	// P4.00 --P4.23Address 
	// P4.24 OE   
	// P4.25 WE    
	// P4.30 CS0   
	// P4.31 CS1  
//--------------------------------------------------------------------------------------

unsigned char SDRAMFunc::MyIO[57][4] =     //[Port],[pin],[mod],[initvalue]
{
	{2,14,PM_MOD,0},
	{2,15,PM_MOD,0},
	{2,16,PM_MOD,0},
	{2,17,PM_MOD,0},
	{2,18,PM_MOD,0},
	{2,19,PM_MOD,0},
	{2,20,PM_MOD,0},
	{2,21,PM_MOD,0},
	{2,24,PM_MOD,0},
	{2,25,PM_MOD,0},
	{2,28,PM_MOD,0},
	{2,29,PM_MOD,0},
	{3,0,PM_MOD,0},
	{3,1,PM_MOD,0},
	{3,2,PM_MOD,0},
	{3,3,PM_MOD,0},
	{3,4,PM_MOD,0},
	{3,5,PM_MOD,0},
	{3,6,PM_MOD,0},
	{3,7,PM_MOD,0},
	{3,8,PM_MOD,0},
	{3,9,PM_MOD,0},
	{3,10,PM_MOD,0},
	{3,11,PM_MOD,0},
	{3,12,PM_MOD,0},
	{3,13,PM_MOD,0},
	{3,14,PM_MOD,0},
	{3,15,PM_MOD,0},
	{4,0,PM_MOD,0},
	{4,1,PM_MOD,0},
	{4,2,PM_MOD,0},
	{4,3,PM_MOD,0},
	{4,4,PM_MOD,0},
	{4,5,PM_MOD,0},
	{4,6,PM_MOD,0},
	{4,7,PM_MOD,0},
	{4,8,PM_MOD,0},
	{4,9,PM_MOD,0},
	{4,10,PM_MOD,0},
	{4,11,PM_MOD,0},
	{4,12,PM_MOD,0},
	{4,13,PM_MOD,0},
	{4,14,PM_MOD,0},
	{4,15,PM_MOD,0},
	{4,16,PM_MOD,0},
	{4,17,PM_MOD,0},
	{4,18,PM_MOD,0},
	{4,19,PM_MOD,0},
	{4,20,PM_MOD,0},
	{4,21,PM_MOD,0},
	{4,22,PM_MOD,0},
	{4,23,PM_MOD,0},
	{4,24,PM_MOD,0},
	{4,25,PM_MOD,0},
	{4,26,PM_MOD,0},
	{4,27,PM_MOD,0},
	{4,30,PM_MOD,0}
	
};


/*****************************************************************************
** Function name:		SDRAMInit
**
** Descriptions:		Initialize external SDRAM memory IDT71V124
**
** parameters:		None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void SDRAMFunc::SDRAM_Init( void )
{
  UWord32 i, dummy = dummy; 
  EMC_CTRL = 0x00000001;
  PCONP  |= 0x00000800;		// Turn On EMC PCLK //

	
	
  PINSEL4 |= 0x50000000;
  PINSEL5 |= 0x05050555;
  PINSEL6  = 0x55555555;               //P3.00 --P3.07 Data//
  PINSEL8  = 0x55555555;               // P4.00 --P4.15Address //
  PINSEL9  |= 0x50555555;              // P4.24 OE   P4.25 WE    P4.30 CS0   P4.31 CS1//

//  PINSEL6  = 0x55555555;               //P3.00 --P3.15 Data//
 // PINSEL8  = 0x55555555;               // P4.00 --P4.23Address //
//  PINSEL9  |= 0x50555555;              // P4.24 OE   P4.25 WE    P4.30 CS0   P4.31 CS1//
// PINSEL8  = 0x55555555;               // P4.00 --P4.23Address //

  EMC_STA_CFG0  = 0x00000080;
  TIMERSFunc::DELAY_SYS( 10 );
   
  EMC_STA_CFG1 = 0x00000081;	   // zl,  bit7: Byte lane state: low/low, high/high, bit1: 16bits
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
 EMC_DYN_CFG1 = 0x00000080;

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
