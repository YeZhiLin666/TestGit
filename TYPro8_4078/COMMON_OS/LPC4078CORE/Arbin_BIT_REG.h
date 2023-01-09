/************************************************************************/
//   ARBIN 功能寄存器位域定义 ,仅定义使用到的功能寄存器，其他部分待添加

/************************************************************************/
#ifndef   __ARBIN_BIT_REG_H
#define   __ARBIN_BIT_REG_H

#define     __PLL_BIT_FIELD
#define     __POWER_BIT_FIELD
#define     __EXTINT_BIT_FIELD
#define     __POWER_BIT_FIELD
#define     __EMC1_BIT_FIELD
#define     __EMC2_BIT_FIELD
#define     __Misc_BIT_FIELD
#define     __WDT_BIT_FIELD
#define     __TIMER_BIT_FIELD
//#define     __I2C_BIT_FIELD
#define     __RTC_BIT_FIELD
#define     __SSP_BIT_FIELD
//#define     __CAN_BIT_FIELD
#define     __IOCON_BIT_FIELD

/************************************************************************/
//  PLL clock control
/************************************************************************/
#ifdef __PLL_BIT_FIELD
//PLL Control registers
typedef union
{
    struct
    {
        uint32    PLLE:1;
        uint32    RSVB1:31;

    } BitReg;
    uint32    Reg;

} Arbin_PLLCON;

//PLL configuration registers
typedef union
{
    struct
    {
        uint32    MSEL:5;
        uint32    PSEL:2;
        uint32    RSVB7:25;
    } BitReg;
    uint32     Reg;

} Arbin_PLLCFG;

//PLL status registers
typedef union
{
    struct
    {
        uint32    MSEL:5;
        uint32    PSEL:2;
        uint32    RSVB7:1;
        uint32    PLL_STAT:1;
        uint32    RSVB9:1;
        uint32    PLOCK:1;
        uint32    RSVB11:21;
    } BitReg;

    uint32     Reg;

} Arbin_PLLSTAT;

//PLL Feed registers
typedef union
{
    struct
    {
        uint32    PLLFEED:8;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_PLLFEED;

#endif

/************************************************************************/
//        Peripherals Power&Clock Control
/************************************************************************/
#ifdef __POWER_BIT_FIELD
//Power Mode Control registers
typedef union
{
    struct
    {
        uint32    PM0:1;
        uint32    PM1:1;
        uint32    BODRPM:1;
        uint32    BOGD:1;
        uint32    BORD:1;
        uint32    RSVB5:3;
        uint32    SMFLAG:1;
        uint32    DSFLAG:1;
        uint32    PDFLAG:1;
        uint32    DPDFLAG:1;
        uint32    RSVB12:20;
    } BitReg;

    uint32     Reg;

} Arbin_PCON;

//Power  Control for Peripherals registers
//bPC... add "b"to avoid conflicts with Macro definition
typedef union
{
    struct
    {
        uint32    bPCLCD:1;         //bit0
        uint32    bPCTIM0:1;
        uint32    bPCTIM1:1;
        uint32    bPCUART0:1;
        uint32    bPCUART1:1;
        uint32    bPCPWM0:1;
        uint32    bPCPWM1:1;
        uint32    bPCI2C0:1;
        uint32    bPCUART4:1;
        uint32    bPCRTC:1;

        uint32    bPCSSP1:1;     //BIT10
        uint32    bPCEMC:1;
        uint32    bPCADC:1;
        uint32    bPCCAN1:1;
        uint32    bPCCAN2:1;
        uint32    bPCGPIO:1;
        uint32    bPCSPIFI:1;
        uint32    bPCMCPWM:1;
        uint32    bPCQEI:1;
        uint32    bPCI2C1:1;

        uint32    bPCSSP2:1;//BIT20
        uint32    bPCSSP0:1;
        uint32    bPCTIM2:1;
        uint32    bPCTIM3:1;
        uint32    bPCUART2:1;
        uint32    bPCUART3:1;
        uint32    bPCI2C2:1;
        uint32    bPCI2S:1;
        uint32    bPCSDC:1;
        uint32    bPCGPDMA:1;
        uint32    bPCENET:1;
        uint32    bPCUSB:1;
    } BitReg;

    uint32     Reg;

} Arbin_PCONP;

//Power Control for Peripherals registers
typedef union
{
    struct
    {
        uint32    RSVB0:3;
        uint32    PCCMP:1;
        uint32    RSVB4:28;
    } BitReg;

    uint32     Reg;

} Arbin_PCONP1;


//EMC Clock Selection register
typedef union
{
    struct
    {
        uint32    EMCDIV:1;
        uint32    RSVB1:31;
    } BitReg;

    uint32     Reg;

} Arbin_EMCCLKSEL;
//CPU Clock Selection register
typedef union
{
    struct
    {
        uint32    CCLKDIV:5;
        uint32    RSVB5:3;
        uint32    CCLKSETL:1;
        uint32    RSVB9:23;
    } BitReg;

    uint32     Reg;

} Arbin_CCLKSEL;

//Reserved USB
//USB Clock Selection register
typedef union
{
    struct
    {
        uint32    USBDIV:5;
        uint32    RSVB5:3;
        uint32    USBSEL:2;
        uint32    RSVB10:22;
    } BitReg;

    uint32     Reg;

} Arbin_USBCLKSEL;

//Clock Source Selection register
typedef union
{
    struct
    {
        uint32    CLKSRC:1;
        uint32    RSVB1:31;
    } BitReg;

    uint32     Reg;

} Arbin_CLKSRCSEL;

//Peripheral Clock Selection register
typedef union
{
    struct
    {
        uint32    PCLKDIV:5;
        uint32    RSVB5:27;
    } BitReg;

    uint32     Reg;

} Arbin_PCLKSEL;

#endif
//Reserved SPIFI

/************************************************************************/
//                         External Interrupt Reg
/************************************************************************/
#ifdef __EXTINT_BIT_FIELD
//External Interrupt Flag register, add "b"to avoid conflicts with Macro definition
typedef union
{
    struct
    {
        uint32    bEINT0:1;
        uint32    bEINT1:1;
        uint32    bEINT2:1;
        uint32    bEINT3:1;
        uint32    RSVB4:28;
    } BitReg;

    uint32     Reg;

} Arbin_EXTINT;

//External Interrupt Mode register

typedef union
{
    struct
    {
        uint32    EXTMODE0:1;
        uint32    EXTMODE1:1;
        uint32    EXTMODE2:1;
        uint32    EXTMODE3:1;
        uint32    RSVB4:28;
    } BitReg;

    uint32     Reg;

} Arbin_EXTMODE;
///External Interrupt Polarity register

typedef union
{
    struct
    {
        uint32    EXTPOLAR0:1;
        uint32    EXTPOLAR1:1;
        uint32    EXTPOLAR2:1;
        uint32    EXTPOLAR3:1;
        uint32    RSVB4:28;
    } BitReg;

    uint32     Reg;

} Arbin_EXTPOLAR;

//END OF EXT
#endif
/************************************************************************/
//Reserved Device and Peripheral Reset  registers
//RSID REG | RSTCON0 REG | RSTCON1 REG
/************************************************************************/

/************************************************************************/
//             Miscellaneous system control register
/************************************************************************/
#ifdef __Misc_BIT_FIELD
//System Controls and Status register
typedef union
{
    struct
    {
        uint32    EMCSC:1;
        uint32    EMCRD:1;
        uint32    EMCBC:1;
        uint32    MCIPWRAL:1;
        uint32    OSCRS:1;
        uint32    OSCEN:1;
        uint32    OSCSTAT:1;
        uint32    RSVB7:25;
    } BitReg;

    uint32     Reg;

} Arbin_SCS;

// CAN Sleep Clear register
typedef union
{
    struct
    {
        uint32    RSVB0:1;
        uint32    CAN1SLEEP:1;
        uint32    CAN2SLEEP:1;
        uint32    RSVB3:29;
    } BitReg;

    uint32     Reg;

} Arbin_CANSLEEPCLR;

// CAN Wake-up Flags register
typedef union
{
    struct
    {
        uint32    RSVB0:1;
        uint32    CAN1WAKE:1;
        uint32    CAN2WAKE:1;
        uint32    RSVB3:29;
    } BitReg;

    uint32     Reg;

} Arbin_CANWAKEFLAGS;

//Reserved USB Interrupt Status Register
//Reserved DMA Request Select Register

//Clock Output Configuration register
typedef union
{
    struct
    {
        uint32    CLKOUTSEL:4;
        uint32    CLKOUTDIV:4;
        uint32    CLKOUT_EN:1;
        uint32    CLKOUT_ACT:1;
        uint32    RSVB10:22;
    } BitReg;

    uint32     Reg;

} Arbin_CLKOUTCFG;
//end of Miscellaneous
#endif

/************************************************************************/
//                        Watchdog Timer  PART3                                                                   */
/************************************************************************/
#ifdef __WDT_BIT_FIELD
//Watchdog Mode register
typedef union
{
    struct
    {
        uint8    bWDEN:1;
        uint8    bWDRESET:1;
        uint8    bWDTOF:1;
        uint8    bWDTINT:1;
        uint8    WDPROTECT:1;
        uint8    RSVB5:3;
    } BitReg;

    uint8     Reg;

} Arbin_MOD;/////8bit///////


//Watchdog Timer Constant register
typedef union
{
    struct
    {
        uint32    Count:24;
        uint32    RSVB24:8;
    } BitReg;

    uint32     Reg;

} Arbin_WDTTC;

//Watchdog Feed register   8bit
typedef union
{
    typedef struct
    {
        uint8    Feed:8;

    } BitReg;

    uint8     Reg;

} Arbin_FEED;

//Watchdog Timer Value register
typedef union
{
    struct
    {
        uint32    Count:24;
        uint32    RSVB24:8;
    } BitReg;

    uint32     Reg;

} Arbin_TV;

//Watchdog Timer Warning Interrupt register
typedef union
{
    struct
    {
        uint32    WARNINT:10;
        uint32    RSVB10:22;
    } BitReg;

    uint32     Reg;

} Arbin_WARNINT;

//Watchdog Timer Window register
typedef union
{
    struct
    {
        uint32    WINDOW:24;
        uint32    RSVB24:8;
    } BitReg;

    uint32     Reg;

} Arbin_WINDOW;

//End of Watchdog module
#endif

/************************************************************************/
//                           TIMER      Register
/************************************************************************/
#ifdef __TIMER_BIT_FIELD
//Interrupt Register
typedef union
{
    struct
    {
        uint32    MR0INT:1;
        uint32    MR1INT:1;
        uint32    MR2INT:1;
        uint32    MR3INT:1;
        uint32    CR0INT:1;
        uint32    CR1INT:1;
        uint32    RSVB6:26;
    } BitReg;

    uint32     Reg;
} Arbin_IR;
//Timer Control Register
typedef union
{
    struct
    {
        uint32    CEN:1;
        uint32    CRST:1;
        uint32    RSVB2:30;
    } BitReg;

    uint32     Reg;
} Arbin_TCR;

//Timer Counter Registers
typedef union
{
    struct
    {
        uint32    TC:32;
    } BitReg;

    uint32     Reg;
} Arbin_TC;
// Prescale register
typedef union
{
    struct
    {
        uint32    PM:32;
    } BitReg;

    uint32     Reg;
} Arbin_PR;

// Prescale Counter register
typedef union
{
    struct
    {
        uint32    PC:32;
    } BitReg;

    uint32     Reg;
} Arbin_PC;

// Match Control register
typedef union
{
    struct
    {
        uint32    MR0I:1;
        uint32    MR0R:1;
        uint32    MR0S:1;
        uint32    MR1I:1;
        uint32    MR1R:1;
        uint32    MR1S:1;
        uint32    MR2I:1;
        uint32    MR2R:1;
        uint32    MR2S:1;
        uint32    MR3I:1;
        uint32    MR3R:1;
        uint32    MR3S:1;
        uint32    RSVB12:20;

    } BitReg;

    uint32     Reg;
} Arbin_MCR;

// Match register[0:3]
typedef union
{
    struct
    {
        uint32    MATCH:32;
    } BitReg;

    uint32     Reg;
} Arbin_MR;

// Capture Control register
typedef union
{
    struct
    {
        uint32    CAP0RE:1;
        uint32    CAP0FE:1;
        uint32    CAP0I:1;
        uint32    CAP1RE:1;
        uint32    CAP1FE:1;
        uint32    CAP1I:1;
        uint32    RSVB6:26;

    } BitReg;

    uint32     Reg;
} Arbin_CCR;

// Capture register[0:1]
typedef union
{
    struct
    {
        uint32    CAP:32;
    } BitReg;

    uint32     Reg;

} Arbin_CR;

// External Match register
typedef union
{
    struct
    {
        uint32    EM0:1;
        uint32    EM1:1;
        uint32    EM2:1;
        uint32    EM3:1;
        uint32    EMC0:2;
        uint32    EMC1:2;
        uint32    EMC2:2;
        uint32    EMC3:2;
        uint32    RSVB12:20;
    } BitReg;

    uint32     Reg;

} Arbin_EMR;

// Count Control register
typedef union
{
    struct
    {
        uint32    CTMODE:2;
        uint32    CINSEL:2;
        uint32    RSVB4:28;
    } BitReg;

    uint32     Reg;

} Arbin_CTCR;

#endif

/************************************************************************/
//                         I2C registers
/************************************************************************/
#ifdef __I2C_BIT_FIELD
//I2C Control Set register
typedef union
{
    struct
    {
        uint32    RSVB0:2;
        uint32    AA:1;
        uint32    SI:1;
        uint32    STO:1;
        uint32    STA:1;
        uint32    I2EN:1;
        uint32    RSVB7:25;
    } BitReg;

    uint32     Reg;

} Arbin_CONSET;
//I2C Control Clear register
typedef union
{
    struct
    {
        uint32    RSVB0:2;
        uint32    AAC:1;
        uint32    SIC:1;
        uint32    RSVB4:1;
        uint32    STAC:1;
        uint32    I2ENC:1;
        uint32    RSVB7:25;
    } BitReg;

    uint32     Reg;

} Arbin_CONCLR;

//I2C  Status register
typedef union
{
    struct
    {
        uint32    RSVB0:3;
        uint32    Status:5;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_STAT;

//I2C  Data register
typedef union
{
    struct
    {
        uint32    Data:8;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;
} Arbin_DAT;

//I2C Monitor mode control register
typedef union
{
    struct
    {
        uint32    MM_ENA:1;
        uint32    ENA_SCL:1;
        uint32    MATCH_ALL:1;
        uint32    RSVB3:29;
    } BitReg;

    uint32     Reg;

} Arbin_MMCTRL;

//I2C Data buffer register
typedef union
{
    struct
    {
        uint32    Data:8;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_DATA_BUFFER;

//I2C Slave Address registers
typedef union
{
    struct
    {
        uint32    GC:1;
        uint32    Address:7;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_ADR0;

//I2C Slave Address registers
typedef union
{
    struct
    {
        uint32    GC:1;
        uint32    Address:7;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_ADR1;

//I2C Mask registers MASK[0:3]
typedef union
{
    struct
    {
        uint32    RSVB0:1;
        uint32    MASK:7;
        uint32    RSVB8:24;
    } BitReg;

    uint32     Reg;

} Arbin_MASK;

//I2C SCL HIGH duty cycle registers
typedef union
{
    struct
    {
        uint32    SCLH:16;
        uint32    RSVB16:16;
    } BitReg;

    uint32     Reg;
} Arbin_SCLH;

//I2C SCL LOW duty cycle registers
typedef union
{
    struct
    {
        uint32    SCLL:16;
        uint32    RSVB16:16;
    } BitReg;

    uint32     Reg;
} Arbin_SCLL;
//End of I2C module
#endif
/************************************************************************/
//                   RTC register Miscellaneous register group
/************************************************************************/
#ifdef __RTC_BIT_FIELD
//Interrupt Location Register
typedef union
{
    struct
    {
        uint8    RTCCIF:1;
        uint8    RTCALF:1;
        uint8    RSVB2:6;
    } BitReg;

    uint8     Reg;

} Arbin_ILR;
//Clock Control Register
typedef union
{
    struct
    {
        uint8    CLKEN:1;
        uint8    CTCRST:1;
        uint8    RSVB2:2;
        uint8    CCALEN:1;
        uint8    RSVB5:3;
    } BitReg;

    uint8     Reg;

} Arbin_RTC_CCR;
//Counter Increment Interrupt Register
typedef union
{
    struct
    {
        uint8    bIMSEC:1;
        uint8    bIMIN:1;
        uint8    bIMHOUR:1;
        uint8    bIMDOM:1;

        uint8    bIMDOW:1;
        uint8    bIMDOY:1;
        uint8    bIMMON:1;
        uint8    bIMYEAR:1;

    } BitReg;

    uint8     Reg;

} Arbin_CIIR;
//Alarm Mask Register
typedef union
{
    struct
    {
        uint8    bAMRSEC:1;
        uint8    bAMRMIN:1;
        uint8    bAMRHOUR:1;
        uint8    bAMRDOM:1;

        uint8    bAMRDOW:1;
        uint8    bAMRDOY:1;
        uint8    bAMRMON:1;
        uint8    bAMRYEAR:1;

    } BitReg;

    uint8     Reg;

} Arbin_AMR;

//RTC Auxiliary control Register
typedef union
{
    struct
    {
        uint8    RSVB0:4;
        uint8    RTC_OSCF:1;
        uint8    RSVB5:1;
        uint8    RTC_PDOUT:1;
        uint8    RSVB7:1;
    } BitReg;

    uint8     Reg;

} Arbin_RTC_AUX;
//RTC Auxiliary Enable Register
typedef union
{
    struct
    {
        uint8    RSVB0:4;
        uint8    RTC_OSCFEN:1;
        uint8    RSVB5:3;
    } BitReg;

    uint8     Reg;

} Arbin_RTC_AUXEN;

//Consolidated Time Register0
typedef union
{
    struct
    {
        uint32    SECONDS:6;
        uint32    RSVB6:2;
        uint32    MINUTES:6;
        uint32    RSVB14:2;
        uint32    HOURS:5;
        uint32    RSVB21:3;
        uint32    DOW:3;
        uint32    RSVB27:5;
    } BitReg;

    uint32     Reg;

} Arbin_CTIME0;

//Consolidated Time Register1
typedef union
{
    struct
    {
        uint32    DOM:5;
        uint32    RSVB5:3;
        uint32    MONTH:4;
        uint32    RSVB12:4;
        uint32    YEAR:12;
        uint32    RSVB28:4;
    } BitReg;

    uint32     Reg;

} Arbin_CTIME1;
//Consolidated Time Register2
typedef union
{
    struct
    {
        uint32    DOY:12;
        uint32    RSVB12:20;

    } BitReg;

    uint32     Reg;

} Arbin_CTIME2;

//Seconds Register
typedef union
{
    struct
    {
        uint8    SECONDS:6;
        uint8    RSVB6:2;

    } BitReg;

    uint8     Reg;

} Arbin_SEC;

//Minutes Register
typedef union
{
    struct
    {
        uint8    MINUTES:6;
        uint8    RSVB6:2;

    } BitReg;

    uint8     Reg;

} Arbin_MIN;
//Hours Register
typedef union
{
    struct
    {
        uint8    HOURS:5;
        uint8    RSVB5:3;

    } BitReg;

    uint8     Reg;
} Arbin_HRS;
//Day of month Register
typedef union
{
    struct
    {
        uint8    DOM:5;
        uint8    RSVB5:3;

    } BitReg;

    uint8     Reg;

} Arbin_DOM;
//day of week Register
typedef union
{
    struct
    {
        uint8    DOW:3;
        uint8    RSVB3:5;

    } BitReg;

    uint8     Reg;

} Arbin_DOW;

//day of year Register
typedef union
{
    struct
    {
        uint16    DOY:9;
        uint16    RSVB9:7;

    } BitReg;

    uint16     Reg;

} Arbin_DOY;

//Month Register
typedef union
{
    struct
    {
        uint8    MONTH:4;
        uint8    RSVB4:4;

    } BitReg;

    uint8     Reg;

} Arbin_MONTH;
//Year Register
typedef union
{
    struct
    {
        uint16    YEAR:12;
        uint16    RSVB12:4;

    } BitReg ;

    uint16     Reg;

} Arbin_YEAR;
//Calibration Register??
typedef union
{
    struct
    {
        uint32    CALVAL:17;
        uint32    CALDIR:1;
        uint32    RSVB18:14;

    } BitReg;

    uint32     Reg;

} Arbin_CALIBRATION;
//General purpose Register GPREG[0:4]
typedef union
{
    struct
    {
        uint32    GP:32;
    } BitReg;

    uint32     Reg;

} Arbin_GPREG;
//Alarm Register group
//Alarm seconds register
typedef union
{
    struct
    {
        uint8    SECONDS:6;
        uint8    RSVB6:2;

    } BitReg;

    uint8     Reg;

} Arbin_ASEC;
//Alarm minutes register
typedef union
{
    struct
    {
        uint8    MINUTES:6;
        uint8    RSVB6:2;

    } BitReg;

    uint8     Reg;

} Arbin_AMIN;
//Alarm Hours Register
typedef union
{
    struct
    {
        uint8    HOURS:5;
        uint8    RSVB5:3;

    } BitReg;

    uint8     Reg;

} Arbin_AHRS;
//Alarm Day of month Register
typedef union
{
    struct
    {
        uint8    DOM:5;
        uint8    RSVB5:3;

    } BitReg;

    uint8     Reg;

} Arbin_ADOM;
//Alarm day of week Register
typedef union
{
    struct
    {
        uint8    DOW:3;
        uint8    RSVB3:5;

    } BitReg;

    uint8     Reg;

} Arbin_ADOW;

//Alarm day of year Register
typedef union
{
    struct
    {
        uint16    DOY:9;
        uint16    RSVB9:7;

    } BitReg;

    uint16     Reg;

} Arbin_ADOY;

//Alarm Month Register
typedef union
{
    struct
    {
        uint8    MONTH:4;
        uint8    RSVB4:4;

    } BitReg;

    uint8     Reg;

} Arbin_AMON;
//Alarm Year Register
typedef union
{
    struct
    {
        uint16    YEAR:12;
        uint16    RSVB12:4;

    } BitReg;

    uint16     Reg;

} Arbin_AYRS;
//End of RTC module
#endif
/************************************************************************/
// Synchronous Serial Communication (SSP)
/************************************************************************/
#ifdef __SSP_BIT_FIELD
//SSPn Control Register 0
typedef union
{
    struct
    {
        uint32    DDS:4;
        uint32    FRF:2;
        uint32    CPOL:1;
        uint32    CPHA:1;
        uint32    SCR:8;
        uint32    RSVB16:16;

    } BitReg;

    uint32     Reg;

} Arbin_CR0;
//SSPn Control Register 1
typedef union
{
    struct
    {
        uint32    LBM:1;
        uint32    SSE:1;
        uint32    MS:1;
        uint32    SOD:1;
        uint32    RSVB4:28;

    } BitReg;

    uint32     Reg;
} Arbin_CR1;
//SSPn Data Register [0:2]
typedef union
{
    struct
    {
        uint32    DATA:16;
        uint32    RSVB16:16;

    } BitReg;

    uint32     Reg;

} Arbin_DR;
//SSPn Status Register
typedef union
{
    struct
    {
        uint32    TFE:1;
        uint32    TNF:1;
        uint32    RNE:1;
        uint32    RFF:1;
        uint32    BSY:1;
        uint32    RSVB5:27;

    } BitReg;

    uint32     Reg;

} Arbin_SR;
//SSPn Clock Prescale Register
typedef union
{
    struct
    {
        uint32   CPSDVSR:8;
        uint32   RSVB8:24;
    } BitReg;
    uint32     Reg;

} Arbin_CPSR;
//SSPn Interrupt Set/Clear Register
typedef union
{
    struct
    {
        uint32   RORIM:1;
        uint32   RTIM:1;
        uint32   RXIM:1;
        uint32   TXIM:1;
        uint32   RSVB4:28;
    } BitReg;
    uint32     Reg;

} Arbin_IMSC;
//SSPn Raw Interrupt Status Register
typedef union
{
    struct
    {
        uint32   RORRIS:1;
        uint32   RTRIS:1;
        uint32   RXRIS:1;
        uint32   TXRIS:1;
        uint32   RSVB4:28;
    } BitReg;
    uint32     Reg;

} Arbin_RIS;
//SSPn Masked Interrupt Status Register
typedef union
{
    struct
    {
        uint32   RORMIS:1;
        uint32   RTMIS:1;
        uint32   RXMIS:1;
        uint32   TXMIS:1;
        uint32   RSVB4:28;
    } BitReg;
    uint32     Reg;

} Arbin_MIS;

//SSPn Interrupt Clear Register
typedef union
{
    struct
    {
        uint32   RORIC:1;
        uint32   RTIC:1;
        uint32   RSVB2:30;
    } BitReg;
    uint32     Reg;

} Arbin_ICR;
//SSPn Interrupt DMA control Register
typedef union
{
    struct
    {
        uint32   RXDMAE:1;
        uint32   TXDMAE:1;
        uint32   RSVB2:30;
    } BitReg;
    uint32     Reg;

} Arbin_DMACR;
//End of SSP module
#endif

/************************************************************************/
//               Controller Area Network (CAN)
/************************************************************************/
#ifdef   __CAN_BIT_FIELD
//Acceptance Filter Mode Register
typedef union
{
    struct
    {
        uint32   ACCOFF:1;
        uint32   ACCBP:1;
        uint32   EFCAN:1;
        uint32   RSVB3:29;
    } BitReg;
    uint32     Reg;

} Arbin_AFMR;
//Standard Frame Individual Start Address  Register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   SFF_SA:9;
        uint32   RSVB11:21;
    } BitReg;
    uint32     Reg;

} Arbin_SFF_SA;
//Standard Frame Group Start Address  Register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   SFF_GRP_SA:10;
        uint32   RSVB12:20;
    } BitReg;
    uint32     Reg;

} Arbin_SFF_GRP_SA;
//Extended Frame  Start Address  Register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   EFF_GRP_SA:9;
        uint32   RSVB11:21;
    } BitReg;
    uint32     Reg;

} Arbin_EFF_SA;
//Extended Frame Group Start Address  Register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   EFF_GRP_SA:10;
        uint32   RSVB12:20;
    } BitReg;
    uint32     Reg;

} Arbin_EFF_GRP_SA;
//End of FA Tables register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   ENDOFTABLE:10;
        uint32   RSVB12:20;
    } BitReg;
    uint32     Reg;

} Arbin_ENDOFTABLE;
//LUT Error Address  Register
typedef union
{
    struct
    {
        uint32   RSVB0:2;
        uint32   LUTERRAD:9;
        uint32   RSVB11:21;
    } BitReg;
    uint32     Reg;

} Arbin_LUTERRAD;
//LUT Error  Register
typedef union
{
    struct
    {
        uint32   LUTERR:1;
        uint32   RSVB1:31;
    } BitReg;
    uint32     Reg;

} Arbin_LUTERR;
//Global FullCAN Enable  Register
typedef union
{
    struct
    {
        uint32   FCANIE:1;
        uint32   RSVB1:31;
    } BitReg;
    uint32     Reg;

} Arbin_FCANIE;
// FullCAN Interrupt and Capture  Register
typedef union
{
    struct
    {
        uint32   INTPND:32;

    } BitReg;
    uint32     Reg;

} Arbin_FCANIC0;
// FullCAN Interrupt and Capture  Register
typedef union
{
    struct
    {
        uint32   INTpND32:32;

    } BitReg;
    uint32     Reg;

} Arbin_FCANIC1;
//Central Transmit Status Register
typedef union
{
    struct
    {
        uint32   TS1:1;
        uint32   TS2:1;
        uint32   RSVB2:6;
        uint32   TBS1:1;
        uint32   TBS2:1;
        uint32   RSVB10:6;
        uint32   TCS1:1;
        uint32   TCS2:1;
        uint32   RSVB18:14;

    } BitReg;
    uint32     Reg;

} Arbin_TXSR;
//Central Receive Status Register
typedef union
{
    struct
    {
        uint32   RS1:1;
        uint32   RS2:1;
        uint32   RSVB2:6;
        uint32   RBS1:1;
        uint32   RBS2:1;
        uint32   RSVB10:6;
        uint32   DOS1:1;
        uint32   DOS2:1;
        uint32   RSVB18:14;

    } BitReg;
    uint32     Reg;

} Arbin_RXSR;
//Central Miscellaneous Status Register
typedef union
{
    struct
    {
        uint32   E1:1;
        uint32   E2:1;
        uint32   RSVB2:6;
        uint32   BS1:1;
        uint32   BS2:1;
        uint32   RSVB10:22;

    } BitReg;
    uint32     Reg;

} Arbin_MSR;

//CAN Mode Register
typedef union
{
    struct
    {
        uint32   RM:1;
        uint32   LOM:1;
        uint32   STM:1;
        uint32   TPM:1;
        uint32   SM:1;
        uint32   RPM:1;
        uint32   RSVB6:1;
        uint32   TM:1;
        uint32   RSVB8:24;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1MOD;
//CAN Cammand Register
typedef union
{
    struct
    {
        uint32   TR:1;
        uint32   AT:1;
        uint32   RRB:1;
        uint32   CDO:1;
        uint32   SRR:1;
        uint32   STB1:1;
        uint32   STB2:1;
        uint32   STB3:1;
        uint32   RSVB8:24;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1CMR;
//CAN Global Status Register
typedef union
{
    struct
    {
        uint32   RBS:1;
        uint32   DOS:1;
        uint32   TBS:1;
        uint32   TCS:1;
        uint32   RS:1;
        uint32   TS:1;
        uint32   ES:1;
        uint32   BS:1;
        uint32   RSVB8:8;
        uint32   RXERR:8;
        uint32   TXERR:8;


    } BitReg;
    uint32     Reg;

} Arbin_CAN1GSR;
//CAN Interrupt and  Capture Register
typedef union
{
    struct
    {
        uint32   RI:1;
        uint32   TI1:1;
        uint32   EI:1;
        uint32   DOI:1;
        uint32   WUI1;
        uint32   EPI:1;
        uint32   ALI:1;
        uint32   BEI:1;
        uint32   IDI:1;
        uint32   TI2:1;
        uint32   TI3E:1;
        uint32   RSVB11:5;
        uint32   ERRBIT4_0:5;
        uint32   ERRDIR:1;
        uint32   ERRC1_0:2;
        uint32   ALCBIT:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1ICR;
//CAN Interrupt Enable Register
typedef union
{
    struct
    {
        uint32   RIE:1;
        uint32   TIE1:1;
        uint32   EIE:1;
        uint32   DOIE:1;
        uint32   WUIE:1;
        uint32   EPIE:1;
        uint32   ALIE:1;
        uint32   BEIE:1;
        uint32   IDIE:1;
        uint32   TI2E:1;
        uint32   TI3E:1;
        uint32   RSVB11:21;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1IER;
//CAN Bus Timing Register
typedef union
{
    struct
    {
        uint32   BRP:10;
        uint32   RSVB10:4;
        uint32   SJW:2;
        uint32   TESG1:4;
        uint32   TESG2:3;
        uint32   SAM:1;
        uint32   RSVB24:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1BTR;
//CAN Error Warning Limit Register
typedef union
{
    struct
    {
        uint32   EWL:8;
        uint32   RSVB8:24;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1EWL;

//CAN Status Register
typedef union
{
    struct
    {
        uint32   RBS_1:1;
        uint32   DOS_1:1;
        uint32   TBS1_1:1;
        uint32   TCS1_1:1;
        uint32   RS_1:1;
        uint32   TS1_1:1;
        uint32   ES_1:1;
        uint32   BS_1:1;

        uint32   RBS_2:1;
        uint32   DOS_2:1;
        uint32   TBS2_2:1;
        uint32   TCS2_2:1;
        uint32   RS_2:1;
        uint32   TS2_2:1;
        uint32   ES_2:1;
        uint32   BS_2:1;

        uint32   RBS_3:1;
        uint32   DOS_3:1;
        uint32   TBS3_3:1;
        uint32   TCS3_3:1;
        uint32   RS_3:1;
        uint32   TS3_3:1;
        uint32   ES_3:1;
        uint32   BS_3:1;

        uint32   RSVB24:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1SR;
//CAN Receive Frame Status Register
typedef union
{
    struct
    {
        uint32   IDINDEX:10;
        uint32   BP:1;
        uint32   RSVB11:5;
        uint32   DLC:4;
        uint32   RSVB20:10;
        uint32   RTR:1;
        uint32   bFF:1;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1RFS;
//CAN Receive Identifier Register
typedef union
{
    typedef struct
    {
        uint32   ID:11;
        uint32   RSVB11:21;

    } st_mBits0;
    typedef struct
    {
        uint32   ID:29;
        uint32   RSVB29:3;

    } st_mBits1;
    uint32     Reg;
    st_mBits0   BitReg0;
    st_mBits1   BitReg1;
} Arbin_CAN1RID;
//CAN Receive Data Register
typedef union
{
    struct
    {
        uint32   DATA1:8;
        uint32   DATA2:8;
        uint32   DATA3:8;
        uint32   DATA4:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1RDA;
//CAN Receive Data Register
typedef union
{
    struct
    {
        uint32   DATA5:8;
        uint32   DATA6:8;
        uint32   DATA7:8;
        uint32   DATA8:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1RDB;
//CAN Transmit Frame Information Register
typedef union
{
    struct
    {
        uint32   PRIO:8;
        uint32   RSVB8:8;
        uint32   DLC:4;
        uint32   RSVB20:10;
        uint32   RTR:1;
        uint32   bFF:1;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1TFI;

//CAN Transfer Identifier Register
typedef union
{
    typedef struct
    {
        uint32   ID:11;
        uint32   RSVB11:21;

    } st_mBits0;
    typedef struct
    {
        uint32   ID:29;
        uint32   RSVB29:3;

    } st_mBits1;
    uint32     Reg;
    st_mBits0   BitReg0;
    st_mBits1  BitReg1;
} Arbin_CAN1TID;
//CAN Transfer Data Register
typedef union
{
    struct
    {
        uint32   DATA1:8;
        uint32   DATA2:8;
        uint32   DATA3:8;
        uint32   DATA4:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1TDA;
//CAN Transfer Data Register
typedef union
{
    struct
    {
        uint32   DATA5:8;
        uint32   DATA6:8;
        uint32   DATA7:8;
        uint32   DATA8:8;

    } BitReg;
    uint32     Reg;

} Arbin_CAN1TDB;

////End of CAN module
#endif

typedef union
{
    struct
    {
        uint32    RSTLCD:1;
        uint32    RSTTIM0:1;
        uint32    RSTTIM1:1;
        uint32    RSTUART0:1;
        uint32    RSTUART1:1;
        uint32    RSTPWM0:1;
        uint32    RSTPWM1:1;
        uint32    RSTI2C0:1;
        uint32    RSTUART4:1;
        uint32    RSTRTC:1;
        uint32    RSTSSP1:1;
        uint32    RSTEMC:1;
        uint32    RSTADC:1;
        uint32    RSTCAN1:1;
        uint32    RSTCAN2:1;
        uint32    RSTGPIO:1;
        uint32    RSTSPIFI:1;
        uint32    RSTMCPWM:1;
        uint32    RSTQEI:1;
        uint32    RSTI2C1:1;
        uint32    RSTSSP2:1;
        uint32    RSTSSP0:1;
        uint32    RSTTIM2:1;
        uint32    RSTTIM3:1;
        uint32    RSTUART2:1;
        uint32    RSTUART3:1;
        uint32    RSTI2C2:1;
        uint32    RSTI2S:1;
        uint32    RSTSDC:1;
        uint32    RSTGPDMA:1;
        uint32    RSTENET:1;
        uint32    RSTUSB:1;
    } BitReg;

    uint32     Reg;
} Arbin_RSTCON0;

typedef union
{
    struct
    {
        uint32    RSTIOCON:1;
        uint32    RSTDAC:1;
        uint32    RSTCANACC:1;
        uint32    RSVB3:29;
    } BitReg;

    uint32     Reg;
} Arbin_RSTCON1;

/************************************************************************/
//             External Memory Controller (EMC)
/************************************************************************/
#ifdef   __EMC1_BIT_FIELD


/************************************************************************/
/*                   EMC delay control and calibration                  */
/************************************************************************/

//EMC Delay Control registe

typedef union
{
    struct
    {
        uint32    CMDDLY:5;
        uint32    RSVB5:3;
        uint32    FBCLKDLY:5;
        uint32    RSVB13:3;
        uint32    CLKOUT0DLY:5;
        uint32    RSVB21:3;
        uint32    CLKOUT1DLY:5;
        uint32    RSVB29:3;
    } BitReg;

    uint32     Reg;

} Arbin_EMCDLYCTL;

//EMC Calibration register
typedef union
{
    struct
    {
        uint32    CALVALUE:8;
        uint32    RSVB5:6;
        uint32    bSTART:1;
        uint32    bDONE:1;
        uint32    RSVB16:16;
    } BitReg;

    uint32     Reg;

} Arbin_EMCCAL;

//EMC Control register
typedef union
{
    struct
    {
        uint32   E:1;
        uint32   M:1;
        uint32   L:1;
        uint32   RSVB3:29;

    } BitReg;
    uint32     Reg;

} Arbin_CONTROL;
//EMC Status register
typedef union
{
    struct
    {
        uint32   B:1;
        uint32   S:1;
        uint32   SA:1;
        uint32   RSVB3:29;

    } BitReg;
    uint32     Reg;

} Arbin_STATUS;
//EMC Configuration register
typedef union
{
    struct
    {
        uint32   EM:1;
        uint32   RSVB:7;
        uint32   CLKR:1;
        uint32   RSVB9:23;

    } BitReg;
    uint32     Reg;

} Arbin_CONFIG;
//Dynamic Memory Control register
typedef union
{
    struct
    {
        uint32   CE:1;
        uint32   CS:1;
        uint32   SR:1;
        uint32   RSVB3:2;
        uint32   MMC:1;
        uint32   RSVB6:1;
        uint32   wI:2;          //2bits
        uint32   RSVB9:23;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICCONTROL;
//EMC Memory Refresh Timer register
typedef union
{
    struct
    {
        uint32   REFRESH:11;
        uint32   RSVB11:21;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICREFRESH;
//Dynamic Memory Read Configuration register
typedef union
{
    struct
    {
        uint32   RD:2;          //2bits
        uint32   RSVB2:30;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICREADCONFIG;
//Dynamic Memory Memory Precharge Command Period register
typedef union
{
    struct
    {
        uint32   TRP:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRP;

//Dynamic Memory Active to Precharge Command Period register
typedef union
{
    struct
    {
        uint32   TRAS:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRAS;
//Dynamic Memory Self_refresh Exit Time register
typedef union
{
    struct
    {
        uint32   TSREX:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICSREX;
//Dynamic Memory Last Data Out to Active Time register
typedef union
{
    struct
    {
        uint32   TAPR:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICAPR;

#endif
#ifdef   __EMC2_BIT_FIELD
//Dynamic Memory Data In to Active Command Time register
typedef union
{
    struct
    {
        uint32   TDAL:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICDAL;
//Dynamic Memory Write Recovery Time register
typedef union
{
    struct
    {
        uint32   TWR:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICWR;
//Dynamic Memory Active to Active Command Period register
typedef union
{
    typedef struct
    {
        uint32   TRC:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRC;
//Dynamic Memory Auto-refresh Period register
typedef union
{
    struct
    {
        uint32   TRFC:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRFC;
//Dynamic Memory Exit Self-refresh  register
typedef union
{
    struct
    {
        uint32   TXSR:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICXSR;
//Dynamic Memory Active Bank A to Active Bank B Time  register
typedef union
{
    struct
    {
        uint32   TRRD:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRRD;
//Dynamic Memory Load Mode register to Active Command Timer
typedef union
{
    typedef struct
    {
        uint32   TMRD:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICMRD;
//Static Memory Extended Wait register
typedef union
{
    struct
    {
        uint32   EXTENDEDWAIT:10;
        uint32   RSVB10:22;

    } BitReg;
    uint32     Reg;

} Arbin_STATICEXTENDEDWAIT;
//Dynamic Memory Extended Configuration register[0:3]
typedef union
{
    struct
    {
        uint32   RSVB0:3;
        uint32   MD:2;
        uint32   RSVB5:2;
        uint32   AM0:6;
        uint32   RSVB13:1;
        uint32   AM1:1;
        uint32   RSVB15:4;
        uint32   B:1;
        uint32   P:1;
        uint32   RSVB21:11;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICCONFIG;
//Dynamic Memory RASCAS delay register[0:3]
typedef union
{
    struct
    {
        uint32   RAS:2;
        uint32   RSVB2:6;
        uint32   CAS:2;
        uint32   RSVB10:22;

    } BitReg;
    uint32     Reg;

} Arbin_DYNAMICRASCAS;
//Static Memory Configuration register[0:3]
typedef union
{
    struct
    {
        uint32   MW:2;
        uint32   RSVB2:1;
        uint32   PM:1;
        uint32   RSVB4:2;
        uint32   PC:1;
        uint32   PB:1;
        uint32   EW:1;
        uint32   RSVB9:10;
        uint32   B:1;
        uint32   P:1;
        uint32   RSVB21:11;

    } BitReg;
    uint32     Reg;

} Arbin_STATICCONFIG;
//Static Memory Write Enable delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITWEN:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWAITWEN;
//Static Memory  Output Enable delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITOEN:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWAITOEN;
//Static Memory Read delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITRD:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWAITRD;
//Static Memory Page Read delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITPAGE:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWAITPAGE;
//Static Memory  Write delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITWR:5;
        uint32   RSVB5:27;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWAITWR;
//Static Memory  Turn Round delay register[0:3]
typedef union
{
    struct
    {

        uint32   WAITTURN:4;
        uint32   RSVB4:28;

    } BitReg;
    uint32     Reg;

} Arbin_STATICWATTURN;
//End of EMC module

#endif

/************************************************************************/
//         IOCON模块
/************************************************************************/
#ifdef __IOCON_BIT_FIELD
//Type D IOCON registers bit description
typedef union
{
    struct
    {

        uint32   FUNC:3;
        uint32   MODE:2;
        uint32   HYS:1;
        uint32   INV:1;
        uint32   RSVB7:2;
        uint32   SLEW:1;
        uint32   OD:1;
        uint32   RSVB11:21;

    } BitReg;
    uint32     Reg;

} Arbin_IOCON_D;

//Type W IOCON registers bit description
typedef union
{
    struct
    {

        uint32   FUNC:3;
        uint32   MODE:2;
        uint32   HYS:1;
        uint32   INV:1;
        uint32   ADMODE:1;     //Select Analog/Digital
        uint32   FILTER:1;
        uint32   SLEW:1;
        uint32   OD:1;
        uint32   RSVB11:21;

    } BitReg;
    uint32     Reg;

} Arbin_IOCON_W;
//Type A IOCON registers bit description
typedef union
{
    struct
    {

        uint32   FUNC:3;
        uint32   MODE:2;
        uint32   RSVB5:1;
        uint32   INVERT:1;
        uint32   ADMODE:1;     //Select Analog/Digital
        uint32   FILTER:1;
        uint32   RSVB9:1;
        uint32   OD:1;
        uint32   RSVB11:4;
        uint32   DACEN:1;
        uint32   RSVB17:15;

    } BitReg;
    uint32     Reg;

} Arbin_IOCON_A;
//Type I IOCON registers bit description
typedef union
{
    struct
    {

        uint32   FUNC:3;
        uint32   RSVB3:3;
        uint32   INVERT:1;
        uint32   RSVB7:1;
        uint32   HS:1;
        uint32   HIDRIVE:1;
        uint32   RSVB10:22;

    } BitReg;
    uint32     Reg;

} Arbin_IOCON_I;
//Type U IOCON registers bit description
typedef union
{
    struct
    {

        uint32   FUNC:3;
        uint32   RSVB3:29;

    } BitReg;
    uint32     Reg;

} Arbin_IOCON_U;
//End of IOCON module
#endif


//end of Arbin_BIT_REG.H
#endif

