/*****************************************************************************
// 程序文件      :SDRAM.cpp     主要用于存放没有放入特定文件的类程序
// 文件描述      :为LPC4078 MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/
#include "includes.h"

//class SDRAMFunc
unsigned char SDRAMFunc::MyIOCount = 57;

const unsigned char SDRAMFunc::MyIO[57][4] =     //[Port],[pin],[mod],[initvalue]
{
    {2,14,SDRAM_ENABLE,0},
    {2,15,SDRAM_ENABLE,0},
    {2,16,SDRAM_ENABLE,0},
    {2,17,SDRAM_ENABLE,0},
    {2,18,SDRAM_ENABLE,0},
    {2,19,SDRAM_ENABLE,0},
    {2,20,SDRAM_ENABLE,0},
    {2,21,SDRAM_ENABLE,0},
    {2,24,SDRAM_ENABLE,0},
    {2,25,SDRAM_ENABLE,0},
    {2,28,SDRAM_ENABLE,0},
    {2,29,SDRAM_ENABLE,0},
    {3,0,SDRAM_ENABLE,0},
    {3,1,SDRAM_ENABLE,0},
    {3,2,SDRAM_ENABLE,0},
    {3,3,SDRAM_ENABLE,0},
    {3,4,SDRAM_ENABLE,0},
    {3,5,SDRAM_ENABLE,0},
    {3,6,SDRAM_ENABLE,0},
    {3,7,SDRAM_ENABLE,0},
    {3,8,SDRAM_ENABLE,0},
    {3,9,SDRAM_ENABLE,0},
    {3,10,SDRAM_ENABLE,0},
    {3,11,SDRAM_ENABLE,0},
    {3,12,SDRAM_ENABLE,0},
    {3,13,SDRAM_ENABLE,0},
    {3,14,SDRAM_ENABLE,0},
    {3,15,SDRAM_ENABLE,0},
    {4,0,SDRAM_ENABLE,0},
    {4,1,SDRAM_ENABLE,0},
    {4,2,SDRAM_ENABLE,0},
    {4,3,SDRAM_ENABLE,0},
    {4,4,SDRAM_ENABLE,0},
    {4,5,SDRAM_ENABLE,0},
    {4,6,SDRAM_ENABLE,0},
    {4,7,SDRAM_ENABLE,0},
    {4,8,SDRAM_ENABLE,0},
    {4,9,SDRAM_ENABLE,0},
    {4,10,SDRAM_ENABLE,0},
    {4,11,SDRAM_ENABLE,0},
    {4,12,SDRAM_ENABLE,0},
    {4,13,SDRAM_ENABLE,0},
    {4,14,SDRAM_ENABLE,0},
    {4,15,SDRAM_ENABLE,0},
    {4,16,SDRAM_ENABLE,0},
    {4,17,SDRAM_ENABLE,0},
    {4,18,SDRAM_ENABLE,0},
    {4,19,SDRAM_ENABLE,0},
    {4,20,SDRAM_ENABLE,0},
    {4,21,SDRAM_ENABLE,0},
    {4,22,SDRAM_ENABLE,0},
    {4,23,SDRAM_ENABLE,0},
    {4,24,SDRAM_ENABLE,0},
    {4,25,SDRAM_ENABLE,0},
    {4,26,SDRAM_ENABLE,0},
    {4,27,SDRAM_ENABLE,0},
    {4,30,SDRAM_ENABLE,0}

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
    // EMC_CS0 0x8000 0000 - 0x83FF FFFF static memory devices 64 MB  // 用于 32Mbit(4MByte) NOR FLASH “AM29LV320D ”
    // EMC_CS1 0x9000 0000 - 0x93FF FFFF static memory devices 64 MB  // 用于 CPLD “XC9572XL-10TQG100C ”
    // EMC_CS2 0x9800 0000 - 0x9BFF FFFF static memory devices 64 MB  // 未使用
    // EMC_CS3 0x9C00 0000 - 0x9FFF FFFF static memory devices 64 MB  // 未使用
    // EMC_DYCS0 0xA000 0000 - 0xAFFF FFFF dynamic memory devices 256 MB  // 256Mbit(32MByte) SDRAM “K4S511632B-UC75 ”
    // EMC_DYCS1 0xB000 0000 - 0xBFFF FFFF dynamic memory devices 256 MB  // 未使用
    // EMC_DYCS2 0xC000 0000 - 0xCFFF FFFF dynamic memory devices 256 MB  // 未使用
    // EMC_DYCS3 0xD000 0000 - 0xDFFF FFFF dynamic memory devices 256 MB  // 未使用

    //  K4S511632B-UC75   256Mbit(32MByte) SDRAM  (DYCS0=0xA000 0000 - 0xAFFF FFFF)
    // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01E84800

    UWord32 uw32_parameter_32M[4]= {0x00000680,12,0xA1008000,0x01F40000}; // 32M  //DynamicConfig0;Set mode register;Check address;Clear SDRAM length;
    UWord32 uw32_parameter_64M[4]= {0x00000880,13,0xA2008000,0x03E80000}; // 64M  //DynamicConfig0;Set mode register;Check address;Clear SDRAM length;

    //  MT48LC32M16A2   512Mbit(64MByte) SDRAM  0x03E80000  64M
    UWord32 *uw32_P = &uw32_parameter_64M[0];
#if HARDWARE_SDRAM_32M == 1
    //  K4S511632B-UC75   256Mbit(32MByte) SDRAM  0x01F40000  32M
    uw32_P = &uw32_parameter_32M[0];
#endif

    UWord32 uw32_i = 0;
    UWord32 * This_P = NULL;

    // Power Up the EMC controller. //
    LPC_SC->PCONP      |= PCEMC;   // Turn On EMC Power //
    LPC_SC->EMCDLYCTL   = 0x00001010;  // [4:0] EMCCLKDELAY delay 4ns;[12:8]FBCLKIN delay 4ns;EMC_CLKOUT[0]/EMC_CLKOUT[1] delay 0ns
    LPC_EMC->Control 	= 0x00000001;  // Enabled (POR and warm reset value); Normal memory map; Normal mode (warm reset value).
    LPC_EMC->Config  	= 0x00000000;  // Endian mode Little-endian mode (POR reset value); CLKOUT ratio(POR reset value).

    LPC_IOCON->P2_14 |= 1;  // EMC_CS2 — LOW active Chip Select 2 signal.  // P2.14 CS2 //
    LPC_IOCON->P2_16 |= 1;  // EMC_CAS — LOW active SDRAM Column Address Strobe.  // P2.16 CAS //
    LPC_IOCON->P2_17 |= 1;  // EMC_RAS — LOW active SDRAM Row Address Strobe.  // P2.17 RAS //
    LPC_IOCON->P2_18 |= 1;  // EMC_CLK0 — SDRAM clock 0.  // P2.18 CLK0 //
    LPC_IOCON->P2_19 |= 1;  // EMC_CLK1 — SDRAM clock 1.  // P2.19 CLK1 //
    LPC_IOCON->P2_20 |= 1;  // EMC_DYCS0 — SDRAM chip select 0.  // P2.20 DYCS0 //
    LPC_IOCON->P2_21 |= 1;  // EMC_DYCS1 — SDRAM chip select 1.  // P2.21 DYCS1 //
    LPC_IOCON->P2_24 |= 1;  // EMC_CKE0 — SDRAM clock enable 0.  // P2.24 CKE0 //
    LPC_IOCON->P2_25 |= 1;  // EMC_CKE1 — SDRAM clock enable 1.  // P2.25 CKE1 //
    LPC_IOCON->P2_28 |= 1;  // EMC_DQM0 — Data mask 0 used with SDRAM and static devices.  // P2.28 DQM0 //
    LPC_IOCON->P2_29 |= 1;  // EMC_DQM1 — Data mask 1 used with SDRAM and static devices.  // P2.29 DQM1 //

    This_P = (UWord32 *) (&LPC_IOCON->P3_0);
    for(uw32_i =0; uw32_i < 16; uw32_i++)  //P3.00 --P3.15 Data //
    {
        *This_P |= 1;     // EMC_D[X] — External memory data line X.
        This_P++;
    }

    This_P = (UWord32 *) (&LPC_IOCON->P4_0);
    //This_P = (UWord32 *)(LPC_IOCON_BASE + ((4 * 32)*sizeof(UWord32)));
    for(uw32_i =0; uw32_i < 24; uw32_i++) // P4.00 --P4.23Address //
    {
        *This_P |= 1;   // EMC_A[X] — External memory address line X.
        This_P++;
    }

    LPC_IOCON->P4_24 |= 1;  // EMC_OE — LOW active Output Enable signal.  // P4.24 OE //
    LPC_IOCON->P4_25 |= 1;  // EMC_WE — LOW active Write Enable signal.  // P4.25 WE //
    LPC_IOCON->P4_30 |= 1;  // EMC_CS0 — LOW active Chip Select 0 signal.  // P4.30 CS0 //
    LPC_IOCON->P4_31 |= 1;  // EMC_CS1 — LOW active Chip Select 1 signal.  // P4.31 CS1 //

    LPC_EMC->StaticConfig0 = 0x00000081;    // Memory width (MW) 16 bit (POR reset value);For reads/writes the respective active bits in BLS3:0 are LOW.
    TIMERSFunc::DELAY_SYS( 10, 0 );

    LPC_EMC->StaticConfig1 = 0x00000081;///如果是试用版需要改0x00000081    // Memory width (MW) 16 bit;For reads/writes the respective active bits in BLS3:0 are LOW.
    TIMERSFunc::DELAY_SYS( 10, 0);
    TIMERSFunc::DELAY_SYS( 3000, 0 ); //delay 0.5ms
    LPC_EMC->DynamicRP     = 2;		// command period: 3(n+1) clock cycles //
    LPC_EMC->DynamicRAS    = 3;		// RAS command period: 4(n+1) clock cycles //
    LPC_EMC->DynamicSREX   = 7;		// Self-refresh period: 8(n+1) clock cycles //
    LPC_EMC->DynamicAPR    = 2;		// Data out to active: 3(n+1) clock cycles //
    LPC_EMC->DynamicDAL    = 5;		// Data in to active: 5(n) clock cycles //
    LPC_EMC->DynamicWR     = 1;		// Write recovery: 2(n+1) clock cycles //
    LPC_EMC->DynamicRC     = 5;		// Active to Active cmd: 6(n+1) clock cycles //
    LPC_EMC->DynamicRFC    = 5;		// Auto-refresh: 6(n+1) clock cycles //
    LPC_EMC->DynamicXSR    = 7;		// Exit self-refresh: 8(n+1) clock cycles //
    LPC_EMC->DynamicRRD    = 1;		// Active bank A->B: 2(n+1) clock cycles //
    LPC_EMC->DynamicMRD    = 2;		// Load Mode to Active cmd: 3(n+1) clock cycles //
    Arbin_EMC->DynamicMRD.Reg = 3;  // Load Mode to Active cmd: 4(n+1) clock cycles //
    LPC_EMC->DynamicReadConfig = 1;		// Command delayed strategy ;using EMCCLKDELAY (command delayed, clock out not delayed).//

    // Default setting, RAS latency 3 CCLKs, CAS latenty 3 CCLKs. //
    LPC_EMC->DynamicRasCas0 = 0x00000303;
    //#if ENG_BOARD_LPC24XX		// NXP engineering board //
    // 256MB, 16Mx16, 12 row, 4 banks, column 9 //    32M
    // 512Mbits, 32Mx16， 13 row, 4 banks, colomum 10   high-performance  // 64M
    LPC_EMC->DynamicConfig0 = uw32_P[0];


    // 16MB, 1Mx16, 4 banks, row=11, column=8 //
    LPC_EMC->DynamicConfig1 = 0x00000080;
    TIMERSFunc::DELAY_SYS(600000, 0);			// use timer 1 //

    // Mem clock enable, CLKOUT runs, send command: NOP //
    LPC_EMC->DynamicControl = 0x00000183;
    TIMERSFunc::DELAY_SYS(120000, 0);			// use timer 1 //

    // Mem clock enable, CLKOUT runs，Send command: PRECHARGE-ALL, shortest possible refresh period //
    LPC_EMC->DynamicControl = 0x00000103;

    // set 2*16=32 CCLKs between SDRAM refresh cycles //
    LPC_EMC->DynamicRefresh = 0x00000002;
    for(uw32_i = 0; uw32_i < 0x40; uw32_i++);	// wait 128 AHB clock cycles //

    // set 28 x 16CCLKs=448CCLK=7us between SDRAM refresh cycles //  60M X*60000000/16=28  X*60000000=448  X=7.4666us
    LPC_EMC->DynamicRefresh = 28;

    // To set mode register in SDRAM, enter mode by issue
    // MODE command, after finishing, bailout and back to NORMAL mode. //
    // Mem clock enable, CLKOUT runs, send command: MODE //
    LPC_EMC->DynamicControl = 0x00000083;
    // Set mode register in SDRAM //
    // Mode regitster table for Micron's MT48LCxx //
    // bit 9:   Programmed burst length(0)  //
    // bit 8~7: Normal mode(0)
    // bit 6~4: CAS latency 3
    // bit 3:   Sequential(0)
    // bit 2~0: Burst length is 8
    // Mode register value calculation is:Base address + (mode register value << (bank bits + column bits + bus width/16)
    uw32_i = *((volatile UWord32 *)(0XA0000000 | (0x33 << uw32_P[1])));

    LPC_EMC->DynamicControl = 0x00000000;	  // Send command: NORMAL //

    LPC_EMC->DynamicConfig0 |= 0x00080000;	  // Enable buffer //
    TIMERSFunc::DELAY_SYS(12000, 0);				  // Use timer 1 //

    SDRAM_Check(uw32_P[2]);

    //清零SDRAM
    uint32 * SdramStartAdr   = ( uint32 *) 0XA0000000;
    uint32 longNumber = uw32_P[3];  // 0x01F40000;   // 0x03E80000
    memset(SdramStartAdr, 0x00000000, longNumber);
    LPC_EMC->StaticWaitRd1 = 0x08;   //提高外部总线频率

}

/*****************************************************************************
** Function name:		SDRAM_Check
**
** Descriptions:		Check external SDRAM memory
**
** parameters:		    Address,
**
** Returned value:		1:Success
**
*****************************************************************************/
unsigned char SDRAMFunc::SDRAM_Check(unsigned long int Address)
{
    unsigned short int uw16_i = 0;
    unsigned short int uw16_error = 0;

    unsigned char * uc_A = (unsigned char *) 0XA0008000;
    for( uw16_i = 0; uw16_i < 200; uw16_i++)
    {
        *uc_A = uw16_i%255;
        uc_A ++;
    }


    UWord32 * uw32_B = (UWord32 *) Address;
    for( uw16_i = 0; uw16_i < 200; uw16_i++)
    {
        *uw32_B = (uw16_i+10) & 0xFF;   //(uw16_i+10)%255
        uw32_B ++;
    }

    uc_A = (unsigned char *) 0XA0008000;
    for( uw16_i = 0; uw16_i < 200; uw16_i++)
    {
        if(*uc_A != (uw16_i%255))
            uw16_error++;
        uc_A ++;
    }

    uw32_B = (UWord32 *) Address;
    for( uw16_i = 0; uw16_i < 200; uw16_i++)
    {
        if(*uw32_B != ((uw16_i+10)%255))
            uw16_error++;
        uw32_B ++;
    }

    if(uw16_error > 1)
    {
        uw16_i = 0;
#if  USE_AS_AUXMCU == 0
        //CConfig::uwOutOf_FRAM_error  |= 0X8000;
#else
        AuxCConfig::uwOutOf_FRAM_error |= 0X8000;
#endif
        return 0;
    }

    return 1;
}

