/****************************************************************************************************//**	Arbin_LPC4078.h			2013-06-05
																									  Arbin custom 4078 headfile
																									  Use Bit Fields struct to operate register.

																									  **********************************************************************/

#ifndef Arbin_LPC4078_H__
#define Arbin_LPC4078_H__
#ifndef Arbin_CORE_M4
#define Arbin_CORE_M4
#endif
#ifdef __cplusplus
extern "C" {
#endif

#include "../LPC4078CORE/Arbin_BIT_REG.h"
#include "../LPC4078CORE/system_LPC407x_8x_177x_8x.h"

    /* ================================================================================ */
    /* ================       Device Specific Peripheral Section       ================ */
    /* ================================================================================ */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

    /*----------- System Control (SC) ----ALL Regs is 32bit-----------------------*/
    typedef struct
    {
        __IO uint32               FLASHCFG;                   /*!< Offset: 0x000 (R/W)  Flash Accelerator Configuration Register */
        uint32               RESERVED0[31];
        __IO Arbin_PLLCON         PLL0CON;                    /*!< Offset: 0x080 (R/W)  PLL0 Control Register */
        __IO Arbin_PLLCFG         PLL0CFG;                    /*!< Offset: 0x084 (R/W)  PLL0 Configuration Register */
        __I  Arbin_PLLSTAT        PLL0STAT;                   /*!< Offset: 0x088 (R/ )  PLL0 Status Register */
        __O  Arbin_PLLFEED        PLL0FEED;                   /*!< Offset: 0x08C ( /W)  PLL0 Feed Register */
        uint32               RESERVED1[4];
        __IO Arbin_PLLCON         PLL1CON;                    /*!< Offset: 0x0A0 (R/W)  PLL1 Control Register */
        __IO Arbin_PLLCFG         PLL1CFG;                    /*!< Offset: 0x0A4 (R/W)  PLL1 Configuration Register */
        __I  Arbin_PLLSTAT        PLL1STAT;                   /*!< Offset: 0x0A8 (R/ )  PLL1 Status Register */
        __O  Arbin_PLLFEED        PLL1FEED;                   /*!< Offset: 0x0AC ( /W)  PLL1 Feed Register */
        uint32               RESERVED2[4];
        __IO Arbin_PCON           PCON;                       /*!< Offset: 0x0C0 (R/W)  Power Control Register */
        __IO Arbin_PCONP          PCONP;                      /*!< Offset: 0x0C4 (R/W)  Power Control for Peripherals Register */
        __IO Arbin_PCONP1         PCONP1;                      /*!< Offset: 0x0C8 (R/W)  Power Control for Peripherals Register */
        uint32               RESERVED3[13];
        __IO Arbin_EMCCLKSEL      EMCCLKSEL;                  /*!< Offset: 0x100 (R/W)  External Memory Controller Clock Selection Register */
        __IO  Arbin_CCLKSEL       CCLKSEL;                    /*!< Offset: 0x104 (R/W)  CPU Clock Selection Register */
        __IO uint32               USBCLKSEL;                  /*!< Offset: 0x108 (R/W)  USB Clock Selection Register */
        __IO Arbin_CLKSRCSEL      CLKSRCSEL;                  /*!< Offset: 0x10C (R/W)  Clock Source Select Register */
        __IO Arbin_CANSLEEPCLR	CANSLEEPCLR;                /*!< Offset: 0x110 (R/W)  CAN Sleep Clear Register */
        __IO Arbin_CANWAKEFLAGS	CANWAKEFLAGS;               /*!< Offset: 0x114 (R/W)  CAN Wake-up Flags Register */
        uint32               RESERVED4[10];
        __IO Arbin_EXTINT         EXTINT;                     /*!< Offset: 0x140 (R/W)  External Interrupt Flag Register */
        uint32               RESERVED5[1];
        __IO Arbin_EXTMODE        EXTMODE;                    /*!< Offset: 0x148 (R/W)  External Interrupt Mode Register */
        __IO Arbin_EXTPOLAR       EXTPOLAR;                   /*!< Offset: 0x14C (R/W)  External Interrupt Polarity Register */
        uint32               RESERVED6[12];
        __IO uint32               RSID;                       /*!< Offset: 0x180 (R/W)  Reset Source Identification Register */
        uint32               RESERVED7[7];
        __IO Arbin_SCS            SCS;                        /*!< Offset: 0x1A0 (R/W)  System Controls and Status Register */
        __IO uint32               IRCTRIM;                    /*!< Offset: 0x1A4 (R/W) Clock Dividers                     */
        __IO Arbin_PCLKSEL        PCLKSEL;                    /*!< Offset: 0x1A8 (R/W)  Peripheral Clock Selection Register */
        uint32               RESERVED8;
        __IO uint32               PBOOST;						/*!< Offset: 0x1B0 (R/W)  Power Boost control register */
        __IO uint32               SPIFICLKSEL;
        __IO uint32               LCD_CFG;                    /*!< Offset: 0x1B8 (R/W)  LCD Configuration and clocking control Register */
        uint32               RESERVED10[1];
        __IO uint32               USBIntSt;                   /*!< Offset: 0x1C0 (R/W)  USB Interrupt Status Register */
        __IO uint32               DMAREQSEL;                  /*!< Offset: 0x1C4 (R/W)  DMA Request Select Register */
        __IO Arbin_CLKOUTCFG      CLKOUTCFG;                  /*!< Offset: 0x1C8 (R/W)  Clock Output Configuration Register */
        __IO Arbin_RSTCON0        RSTCON0;                    /*!< Offset: 0x1CC (R/W)  RESET Control0 Register */
        __IO Arbin_RSTCON1        RSTCON1;                    /*!< Offset: 0x1D0 (R/W)  RESET Control1 Register */
        uint32               RESERVED11[2];
        __IO Arbin_EMCDLYCTL      EMCDLYCTL;                  /*!< Offset: 0x1DC (R/W) SDRAM programmable delays          */
        __IO Arbin_EMCCAL         EMCCAL;                     /*!< Offset: 0x1E0 (R/W) Calibration of programmable delays */
    } Arbin_LPC_SC_TypeDef;



    /*-------- External Memory Controller (EMC) ----ALL Regs is 32bit--------------*/
    typedef struct
    {
        __IO Arbin_CONTROL            Control;
        __I  Arbin_STATUS             Status;
        __IO Arbin_CONFIG             Config;
        uint32                   RESERVED0[5];
        __IO Arbin_DYNAMICCONTROL     DynamicControl;
        __IO Arbin_DYNAMICREFRESH     DynamicRefresh;
        __IO Arbin_DYNAMICREADCONFIG  DynamicReadConfig;
        uint32                   RESERVED1[1];
        __IO Arbin_DYNAMICRP          DynamicRP;
        __IO Arbin_DYNAMICRAS         DynamicRAS;
        __IO Arbin_DYNAMICSREX        DynamicSREX;
        __IO Arbin_DYNAMICAPR         DynamicAPR;
        __IO Arbin_DYNAMICDAL         DynamicDAL;
        __IO Arbin_DYNAMICWR          DynamicWR;
        __IO Arbin_DYNAMICRC          DynamicRC;
        __IO Arbin_DYNAMICRFC         DynamicRFC;
        __IO Arbin_DYNAMICXSR         DynamicXSR;
        __IO Arbin_DYNAMICRRD         DynamicRRD;
        __IO Arbin_DYNAMICMRD         DynamicMRD;
        uint32                   RESERVED2[9];
        __IO Arbin_STATICEXTENDEDWAIT StaticExtendedWait;
        uint32                   RESERVED3[31];
        __IO Arbin_DYNAMICCONFIG      DynamicConfig0;
        __IO Arbin_DYNAMICRASCAS      DynamicRasCas0;
        uint32                   RESERVED4[6];
        __IO Arbin_DYNAMICCONFIG      DynamicConfig1;
        __IO Arbin_DYNAMICRASCAS      DynamicRasCas1;
        uint32                   RESERVED5[6];
        __IO Arbin_DYNAMICCONFIG      DynamicConfig2;
        __IO Arbin_DYNAMICRASCAS      DynamicRasCas2;
        uint32                   RESERVED6[6];
        __IO Arbin_DYNAMICCONFIG      DynamicConfig3;
        __IO Arbin_DYNAMICRASCAS      DynamicRasCas3;
        uint32                   RESERVED7[38];
        __IO Arbin_STATICCONFIG       StaticConfig0;
        __IO Arbin_STATICWAITWEN      StaticWaitWen0;
        __IO Arbin_STATICWAITOEN      StaticWaitOen0;
        __IO Arbin_STATICWAITRD       StaticWaitRd0;
        __IO Arbin_STATICWAITPAGE     StaticWaitPage0;
        __IO Arbin_STATICWAITWR       StaticWaitWr0;
        __IO Arbin_STATICWATTURN      StaticWaitTurn0;
        uint32                   RESERVED8[1];
        __IO Arbin_STATICCONFIG       StaticConfig1;
        __IO Arbin_STATICWAITWEN      StaticWaitWen1;
        __IO Arbin_STATICWAITOEN      StaticWaitOen1;
        __IO Arbin_STATICWAITRD       StaticWaitRd1;
        __IO Arbin_STATICWAITPAGE     StaticWaitPage1;
        __IO Arbin_STATICWAITWR       StaticWaitWr1;
        __IO Arbin_STATICWATTURN      StaticWaitTurn1;
        uint32                   RESERVED9[1];
        __IO Arbin_STATICCONFIG       StaticConfig2;
        __IO Arbin_STATICWAITWEN      StaticWaitWen2;
        __IO Arbin_STATICWAITOEN      StaticWaitOen2;
        __IO Arbin_STATICWAITRD       StaticWaitRd2;
        __IO Arbin_STATICWAITPAGE     StaticWaitPage2;
        __IO Arbin_STATICWAITWR       StaticWaitWr2;
        __IO Arbin_STATICWATTURN      StaticWaitTurn2;
        uint32                   RESERVED10[1];
        __IO Arbin_STATICCONFIG       StaticConfig3;
        __IO Arbin_STATICWAITWEN      StaticWaitWen3;
        __IO Arbin_STATICWAITOEN      StaticWaitOen3;
        __IO Arbin_STATICWAITRD       StaticWaitRd3;
        __IO Arbin_STATICWAITPAGE     StaticWaitPage3;
        __IO Arbin_STATICWAITWR       StaticWaitWr3;
        __IO Arbin_STATICWATTURN      StaticWaitTurn3;
    } Arbin_LPC_EMC_TypeDef;

    /*------------- Watchdog Timer (WDT) ------------------------------------------*/
    typedef struct
    {
        __IO Arbin_MOD            MOD;              //8bit
        uint8                RESERVED0[3];
        __IO Arbin_WDTTC          TC;               //32bit
        __O  Arbin_FEED           FEED;             //8bit
        uint8                RESERVED1[3];
        __I  Arbin_TV             TV;               //32bit
        uint32               RESERVED2;
        __IO Arbin_WARNINT        WARNINT;           //32bit
        __IO Arbin_WINDOW         WINDOW;            //32bit
    } Arbin_LPC_WDT_TypeDef;

    /*------------- Timer (TIM) -------------ALL Regs is 32bit---------------------*/
    typedef struct
    {
        __IO Arbin_IR            IR;                     /*!< Offset: 0x000 Interrupt Register (R/W) */
        __IO Arbin_TCR           TCR;                    /*!< Offset: 0x004 Timer Control Register (R/W) */
        __IO Arbin_TC            TC;                     /*!< Offset: 0x008 Timer Counter Register (R/W) */
        __IO Arbin_PR            PR;                     /*!< Offset: 0x00C Prescale Register (R/W) */
        __IO Arbin_PC            PC;                     /*!< Offset: 0x010 Prescale Counter Register (R/W) */
        __IO Arbin_MCR           MCR;                    /*!< Offset: 0x014 Match Control Register (R/W) */
        __IO Arbin_MR            MR0;                    /*!< Offset: 0x018 Match Register 0 (R/W) */
        __IO Arbin_MR            MR1;                    /*!< Offset: 0x01C Match Register 1 (R/W) */
        __IO Arbin_MR            MR2;                    /*!< Offset: 0x020 Match Register 2 (R/W) */
        __IO Arbin_MR            MR3;                    /*!< Offset: 0x024 Match Register 3 (R/W) */
        __IO Arbin_CCR           CCR;                    /*!< Offset: 0x028 Capture Control Register (R/W) */
        __I  Arbin_CR            CR0;                    /*!< Offset: 0x02C Capture Register 0 (R/ ) */
        __I  Arbin_CR            CR1;					/*!< Offset: 0x030 Capture Register 1 (R/ ) */
        uint32              RESERVED0[2];
        __IO Arbin_EMR           EMR;                    /*!< Offset: 0x03C External Match Register (R/W) */
        uint32              RESERVED1[12];
        __IO Arbin_CTCR          CTCR;                   /*!< Offset: 0x070 Count Control Register (R/W) */
    } Arbin_LPC_TIM_TypeDef;



    /*-------- --- Inter-Integrated Circuit (I2C) ----ALL Regs is 32bit------------*/
#ifdef  __I2C_BIT_FIELD
    typedef struct
    {
        __IO Arbin_CONSET        CONSET;                 /*!< Offset: 0x000 I2C Control Set Register (R/W) */
        __I  Arbin_STAT          STAT;                   /*!< Offset: 0x004 I2C Status Register (R/ ) */
        __IO Arbin_DAT           DAT;                    /*!< Offset: 0x008 I2C Data Register (R/W) */
        __IO Arbin_ADR0          ADR0;                   /*!< Offset: 0x00C I2C Slave Address Register 0 (R/W) */
        __IO Arbin_SCLH          SCLH;                   /*!< Offset: 0x010 SCH Duty Cycle Register High Half Word (R/W) */
        __IO Arbin_SCLL          SCLL;                   /*!< Offset: 0x014 SCL Duty Cycle Register Low Half Word (R/W) */
        __O  Arbin_CONCLR        CONCLR;                 /*!< Offset: 0x018 I2C Control Clear Register ( /W) */
        __IO Arbin_MMCTRL        MMCTRL;                 /*!< Offset: 0x01C Monitor mode control register (R/W) */
        __IO Arbin_ADR1          ADR1;                   /*!< Offset: 0x020 I2C Slave Address Register 1 (R/W) */
        __IO Arbin_ADR1          ADR2;                   /*!< Offset: 0x024 I2C Slave Address Register 2 (R/W) */
        __IO Arbin_ADR1          ADR3;                   /*!< Offset: 0x028 I2C Slave Address Register 3 (R/W) */
        __I  Arbin_DATA_BUFFER   DATA_BUFFER;            /*!< Offset: 0x02C Data buffer register ( /W) */
        __IO Arbin_MASK          MASK0;                  /*!< Offset: 0x030 I2C Slave address mask register 0 (R/W) */
        __IO Arbin_MASK          MASK1;                  /*!< Offset: 0x034 I2C Slave address mask register 1 (R/W) */
        __IO Arbin_MASK          MASK2;                  /*!< Offset: 0x038 I2C Slave address mask register 2 (R/W) */
        __IO Arbin_MASK          MASK3;                  /*!< Offset: 0x03C I2C Slave address mask register 3 (R/W) */
    } Arbin_LPC_I2C_TypeDef;

#endif



    /*------------- Real-Time Clock (RTC) -----------------------------------------*/
    typedef struct
    {
        __IO Arbin_ILR          ILR;              //8bit
        uint8              RESERVED0[7];
        __IO Arbin_RTC_CCR      CCR;              //8bit
        uint8              RESERVED1[3];
        __IO Arbin_CIIR         CIIR;            //8bit
        uint8              RESERVED2[3];
        __IO Arbin_AMR          AMR;             //8bit
        uint8              RESERVED3[3];
        __I  Arbin_CTIME0       CTIME0;
        __I  Arbin_CTIME1       CTIME1;
        __I  Arbin_CTIME2       CTIME2;
        __IO Arbin_SEC          SEC;              //8bit
        uint8              RESERVED4[3];
        __IO Arbin_MIN          MIN;              //8bit
        uint8              RESERVED5[3];
        __IO Arbin_HRS          HOUR;              //8bit
        uint8              RESERVED6[3];
        __IO Arbin_DOM          DOM;              //8bit
        uint8              RESERVED7[3];
        __IO Arbin_DOW          DOW;             //8bit
        uint8              RESERVED8[3];
        __IO Arbin_DOY          DOY;             //16bit
        uint16             RESERVED9;
        __IO Arbin_MONTH        MONTH;          //8bit
        uint8              RESERVED10[3];
        __IO Arbin_YEAR         YEAR;           //16bit
        uint16             RESERVED11;
        __IO Arbin_CALIBRATION  CALIBRATION;
        __IO Arbin_GPREG        GPREG0;
        __IO Arbin_GPREG        GPREG1;
        __IO Arbin_GPREG        GPREG2;
        __IO Arbin_GPREG        GPREG3;
        __IO Arbin_GPREG        GPREG4;
        __IO Arbin_RTC_AUXEN    RTC_AUXEN;         //8bit
        uint8              RESERVED12[3];
        __IO Arbin_RTC_AUX      RTC_AUX;           //8bit
        uint8              RESERVED13[3];
        __IO Arbin_ASEC         ALSEC;              //8bit
        uint8              RESERVED14[3];
        __IO Arbin_AMIN         ALMIN;             //8bit
        uint8              RESERVED15[3];
        __IO Arbin_AHRS         ALHOUR;           //8bit
        uint8              RESERVED16[3];
        __IO Arbin_ADOM         ALDOM;             //8bit
        uint8              RESERVED17[3];
        __IO Arbin_ADOW         ALDOW;               //8bit
        uint8              RESERVED18[3];
        __IO Arbin_ADOY         ALDOY;              //16bit
        uint16             RESERVED19;
        __IO Arbin_AMON         ALMON;               //8bit
        uint8              RESERVED20[3];
        __IO Arbin_AYRS         ALYEAR;              //16bit
        uint16             RESERVED21;
        __IO uint32             ERSTATUS;
        __IO uint32             ERCONTROL;
        __IO uint32             ERCOUNTERS;
        uint32             RESERVED22;
        __IO uint32             ERFIRSTSTAMP0;
        __IO uint32             ERFIRSTSTAMP1;
        __IO uint32             ERFIRSTSTAMP2;
        uint32             RESERVED23;
        __IO uint32             ERLASTSTAMP0;
        __IO uint32             ERLASTSTAMP1;
        __IO uint32             ERLASTSTAMP2;
    } Arbin_LPC_RTC_TypeDef;


    /*----------- Pin Connect Block (PINCON) ----ALL Regs is 32bit-----------------*/
    typedef struct
    {
        __IO Arbin_IOCON_D P0_0;				/* 0x000 */
        __IO Arbin_IOCON_D P0_1;
        __IO Arbin_IOCON_D P0_2;
        __IO Arbin_IOCON_D P0_3;
        __IO Arbin_IOCON_D P0_4;
        __IO Arbin_IOCON_D P0_5;
        __IO Arbin_IOCON_D P0_6;
        __IO Arbin_IOCON_W P0_7;

        __IO Arbin_IOCON_W P0_8;				/* 0x020 */
        __IO Arbin_IOCON_W P0_9;
        __IO Arbin_IOCON_D P0_10;
        __IO Arbin_IOCON_D P0_11;
        __IO Arbin_IOCON_A P0_12;
        __IO Arbin_IOCON_A P0_13;
        __IO Arbin_IOCON_D P0_14;
        __IO Arbin_IOCON_D P0_15;

        __IO Arbin_IOCON_D P0_16;				/* 0x040 */
        __IO Arbin_IOCON_D P0_17;
        __IO Arbin_IOCON_D P0_18;
        __IO Arbin_IOCON_D P0_19;
        __IO Arbin_IOCON_D P0_20;
        __IO Arbin_IOCON_D P0_21;
        __IO Arbin_IOCON_D P0_22;
        __IO Arbin_IOCON_A P0_23;

        __IO Arbin_IOCON_A P0_24;				/* 0x060 */
        __IO Arbin_IOCON_A P0_25;
        __IO Arbin_IOCON_A P0_26;
        __IO Arbin_IOCON_I P0_27;
        __IO Arbin_IOCON_I P0_28;
        __IO Arbin_IOCON_U P0_29;
        __IO Arbin_IOCON_U P0_30;
        __IO Arbin_IOCON_U P0_31;

        __IO Arbin_IOCON_D P1_0;				/* 0x080 */
        __IO Arbin_IOCON_D P1_1;
        __IO Arbin_IOCON_D P1_2;
        __IO Arbin_IOCON_D P1_3;
        __IO Arbin_IOCON_D P1_4;
        __IO Arbin_IOCON_W P1_5;
        __IO Arbin_IOCON_W P1_6;
        __IO Arbin_IOCON_W P1_7;

        __IO Arbin_IOCON_D P1_8;				/* 0x0A0 */
        __IO Arbin_IOCON_D P1_9;
        __IO Arbin_IOCON_D P1_10;
        __IO Arbin_IOCON_D P1_11;
        __IO Arbin_IOCON_D P1_12;
        __IO Arbin_IOCON_D P1_13;
        __IO Arbin_IOCON_W P1_14;
        __IO Arbin_IOCON_D P1_15;

        __IO Arbin_IOCON_W P1_16;				/* 0x0C0 */
        __IO Arbin_IOCON_W P1_17;
        __IO Arbin_IOCON_D P1_18;
        __IO Arbin_IOCON_D P1_19;
        __IO Arbin_IOCON_D P1_20;
        __IO Arbin_IOCON_D P1_21;
        __IO Arbin_IOCON_D P1_22;
        __IO Arbin_IOCON_D P1_23;

        __IO Arbin_IOCON_D P1_24;				/* 0x0E0 */
        __IO Arbin_IOCON_D P1_25;
        __IO Arbin_IOCON_D P1_26;
        __IO Arbin_IOCON_D P1_27;
        __IO Arbin_IOCON_D P1_28;
        __IO Arbin_IOCON_D P1_29;
        __IO Arbin_IOCON_A P1_30;
        __IO Arbin_IOCON_A P1_31;

        __IO Arbin_IOCON_D P2_0;				/* 0x100 */
        __IO Arbin_IOCON_D P2_1;
        __IO Arbin_IOCON_D P2_2;
        __IO Arbin_IOCON_D P2_3;
        __IO Arbin_IOCON_D P2_4;
        __IO Arbin_IOCON_D P2_5;
        __IO Arbin_IOCON_D P2_6;
        __IO Arbin_IOCON_D P2_7;

        __IO Arbin_IOCON_D P2_8;				/* 0x120 */
        __IO Arbin_IOCON_D P2_9;
        __IO Arbin_IOCON_D P2_10;
        __IO Arbin_IOCON_D P2_11;
        __IO Arbin_IOCON_D P2_12;
        __IO Arbin_IOCON_D P2_13;
        __IO Arbin_IOCON_D P2_14;
        __IO Arbin_IOCON_D P2_15;

        __IO Arbin_IOCON_D P2_16;				/* 0x140 */
        __IO Arbin_IOCON_D P2_17;
        __IO Arbin_IOCON_D P2_18;
        __IO Arbin_IOCON_D P2_19;
        __IO Arbin_IOCON_D P2_20;
        __IO Arbin_IOCON_D P2_21;
        __IO Arbin_IOCON_D P2_22;
        __IO Arbin_IOCON_D P2_23;

        __IO Arbin_IOCON_D P2_24;				/* 0x160 */
        __IO Arbin_IOCON_D P2_25;
        __IO Arbin_IOCON_D P2_26;
        __IO Arbin_IOCON_D P2_27;
        __IO Arbin_IOCON_D P2_28;
        __IO Arbin_IOCON_D P2_29;
        __IO Arbin_IOCON_D P2_30;
        __IO Arbin_IOCON_D P2_31;

        __IO Arbin_IOCON_D P3_0;				/* 0x180 */
        __IO Arbin_IOCON_D P3_1;
        __IO Arbin_IOCON_D P3_2;
        __IO Arbin_IOCON_D P3_3;
        __IO Arbin_IOCON_D P3_4;
        __IO Arbin_IOCON_D P3_5;
        __IO Arbin_IOCON_D P3_6;
        __IO Arbin_IOCON_D P3_7;

        __IO Arbin_IOCON_D P3_8;				/* 0x1A0 */
        __IO Arbin_IOCON_D P3_9;
        __IO Arbin_IOCON_D P3_10;
        __IO Arbin_IOCON_D P3_11;
        __IO Arbin_IOCON_D P3_12;
        __IO Arbin_IOCON_D P3_13;
        __IO Arbin_IOCON_D P3_14;
        __IO Arbin_IOCON_D P3_15;

        __IO Arbin_IOCON_D P3_16;				/* 0x1C0 */
        __IO Arbin_IOCON_D P3_17;
        __IO Arbin_IOCON_D P3_18;
        __IO Arbin_IOCON_D P3_19;
        __IO Arbin_IOCON_D P3_20;
        __IO Arbin_IOCON_D P3_21;
        __IO Arbin_IOCON_D P3_22;
        __IO Arbin_IOCON_D P3_23;

        __IO Arbin_IOCON_D P3_24;				/* 0x1E0 */
        __IO Arbin_IOCON_D P3_25;
        __IO Arbin_IOCON_D P3_26;
        __IO Arbin_IOCON_D P3_27;
        __IO Arbin_IOCON_D P3_28;
        __IO Arbin_IOCON_D P3_29;
        __IO Arbin_IOCON_D P3_30;
        __IO Arbin_IOCON_D P3_31;

        __IO Arbin_IOCON_D P4_0;				/* 0x200 */
        __IO Arbin_IOCON_D P4_1;
        __IO Arbin_IOCON_D P4_2;
        __IO Arbin_IOCON_D P4_3;
        __IO Arbin_IOCON_D P4_4;
        __IO Arbin_IOCON_D P4_5;
        __IO Arbin_IOCON_D P4_6;
        __IO Arbin_IOCON_D P4_7;

        __IO Arbin_IOCON_D P4_8;				/* 0x220 */
        __IO Arbin_IOCON_D P4_9;
        __IO Arbin_IOCON_D P4_10;
        __IO Arbin_IOCON_D P4_11;
        __IO Arbin_IOCON_D P4_12;
        __IO Arbin_IOCON_D P4_13;
        __IO Arbin_IOCON_D P4_14;
        __IO Arbin_IOCON_D P4_15;

        __IO Arbin_IOCON_D P4_16;				/* 0x240 */
        __IO Arbin_IOCON_D P4_17;
        __IO Arbin_IOCON_D P4_18;
        __IO Arbin_IOCON_D P4_19;
        __IO Arbin_IOCON_D P4_20;
        __IO Arbin_IOCON_D P4_21;
        __IO Arbin_IOCON_D P4_22;
        __IO Arbin_IOCON_D P4_23;

        __IO Arbin_IOCON_D P4_24;				/* 0x260 */
        __IO Arbin_IOCON_D P4_25;
        __IO Arbin_IOCON_D P4_26;
        __IO Arbin_IOCON_D P4_27;
        __IO Arbin_IOCON_D P4_28;
        __IO Arbin_IOCON_D P4_29;
        __IO Arbin_IOCON_D P4_30;
        __IO Arbin_IOCON_D P4_31;

        __IO Arbin_IOCON_D P5_0;				/* 0x280 */
        __IO Arbin_IOCON_D P5_1;
        __IO Arbin_IOCON_I P5_2;
        __IO Arbin_IOCON_I P5_3;
        __IO Arbin_IOCON_D P5_4;				/* 0x290 */
    } Arbin_LPC_IOCON_TypeDef;







    /*------ Synchronous Serial Communication (SSP) ----ALL Regs is 32bit----------*/
    typedef struct
    {
        __IO Arbin_CR0       CR0;                    /*!< Offset: 0x000 Control Register 0 (R/W) */
        __IO Arbin_CR1       CR1;                    /*!< Offset: 0x004 Control Register 1 (R/W) */
        __IO Arbin_DR        DR;                     /*!< Offset: 0x008 Data Register (R/W) */
        __I  Arbin_SR        SR;                     /*!< Offset: 0x00C Status Registe (R/ ) */
        __IO Arbin_CPSR      CPSR;                   /*!< Offset: 0x010 Clock Prescale Register (R/W) */
        __IO Arbin_IMSC      IMSC;                   /*!< Offset: 0x014 Interrupt Mask Set and Clear Register (R/W) */
        __IO Arbin_RIS       RIS;                    /*!< Offset: 0x018 Raw Interrupt Status Register (R/W) */
        __IO Arbin_MIS       MIS;                    /*!< Offset: 0x01C Masked Interrupt Status Register (R/W) */
        __IO Arbin_ICR       ICR;                    /*!< Offset: 0x020 SSPICR Interrupt Clear Register (R/W) */
        __IO Arbin_DMACR     DMACR;
    } Arbin_LPC_SSP_TypeDef;


    /*------------- Controller Area Network (CAN) ---------------------------------*/
#ifdef  __CAN_BIT_FIELD
    typedef struct
    {
        __IO uint32         mask[512];              /* ID Masks                           */
    } Arbin_CANAF_RAM_TypeDef;

    typedef struct                          /* Acceptance Filter Registers        */
    {
        ///Offset: 0x00000000 - Acceptance Filter Register
        __IO Arbin_AFMR         AFMR;

        ///Offset: 0x00000004 - Standard Frame Individual Start Address Register
        __IO Arbin_SFF_SA       SFF_sa;

        ///Offset: 0x00000008 - Standard Frame Group Start Address Register
        __IO Arbin_SFF_GRP_SA   SFF_GRP_sa;

        ///Offset: 0x0000000C - Extended Frame Start Address Register
        __IO Arbin_EFF_SA       EFF_sa;

        ///Offset: 0x00000010 - Extended Frame Group Start Address Register
        __IO Arbin_EFF_GRP_SA   EFF_GRP_sa;

        ///Offset: 0x00000014 - End of AF Tables register
        __IO Arbin_ENDOFTABLE   ENDofTable;

        ///Offset: 0x00000018 - LUT Error Address register
        __I  Arbin_LUTERRAD     LUTerrAd;

        ///Offset: 0x0000001C - LUT Error Register
        __I  Arbin_LUTERR       LUTerr;

        ///Offset: 0x00000020 - CAN Central Transmit Status Register
        __IO Arbin_FCANIE       FCANIE;

        ///Offset: 0x00000024 - FullCAN Interrupt and Capture registers 0
        __IO Arbin_FCANIC0      FCANIC0;

        ///Offset: 0x00000028 - FullCAN Interrupt and Capture registers 1
        __IO Arbin_FCANIC1      FCANIC1;
    } Arbin_LPC_CANAF_TypeDef;

    typedef struct                          /* Central Registers                  */
    {
        __I  Arbin_TXSR     TxSR;
        __I  Arbin_RXSR     RxSR;
        __I  Arbin_MSR      MSR;
    } Arbin_LPC_CANCR_TypeDef;

    typedef struct                          /* Controller Registers               */
    {
        ///Offset: 0x00000000 - Controls the operating mode of the CAN Controller
        __IO Arbin_CAN1MOD MOD;

        ///Offset: 0x00000004 - Command bits that affect the state
        __O  Arbin_CAN1CMR CMR;

        ///Offset: 0x00000008 - Global Controller Status and Error Counters
        __IO Arbin_CAN1GSR GSR;

        ///Offset: 0x0000000C - Interrupt status, Arbitration Lost Capture, Error Code Capture
        __I  Arbin_CAN1ICR ICR;

        ///Offset: 0x00000010 - Interrupt Enable Register
        __IO Arbin_CAN1IER IER;

        ///Offset: 0x00000014 - Bus Timing Register
        __IO Arbin_CAN1BTR BTR;

        ///Offset: 0x00000018 - Error Warning Limit
        __IO Arbin_CAN1EWL EWL;

        ///Offset: 0x0000001C - Status Register
        __I  Arbin_CAN1SR SR;

        ///Offset: 0x00000020 - Receive frame status
        __IO Arbin_CAN1RFS RFS;

        ///Offset: 0x00000024 - Received Identifier
        __IO Arbin_CAN1RID RID;

        ///Offset: 0x00000028 - Received data bytes 1-4
        __IO Arbin_CAN1RDA RDA;

        ///Offset: 0x0000002C - Received data bytes 5-8
        __IO Arbin_CAN1RDB RDB;

        ///Offset: 0x00000030 - Transmit frame info (Tx Buffer 1)
        __IO Arbin_CAN1TFI TFI1;

        ///Offset: 0x00000034 - Transmit Identifier (Tx Buffer 1)
        __IO Arbin_CAN1TID TID1;

        ///Offset: 0x00000038 - Transmit data bytes 1-4 (Tx Buffer 1)
        __IO Arbin_CAN1TDA TDA1;

        ///Offset: 0x0000003C - Transmit data bytes 5-8 (Tx Buffer 1)
        __IO Arbin_CAN1TDB TDB1;

        ///Offset: 0x00000040 - Transmit frame info (Tx Buffer 2)
        __IO Arbin_CAN1TFI TFI2;

        ///Offset: 0x00000044 - Transmit Identifier (Tx Buffer 2)
        __IO Arbin_CAN1TID TID2;

        ///Offset: 0x00000048 - Transmit data bytes 1-4 (Tx Buffer 2)
        __IO Arbin_CAN1TDA TDA2;

        ///Offset: 0x0000004C - Transmit data bytes 5-8 (Tx Buffer 2)
        __IO Arbin_CAN1TDB TDB2;

        ///Offset: 0x00000050 - Transmit frame info (Tx Buffer 3)
        __IO Arbin_CAN1TFI TFI3;

        ///Offset: 0x00000054 - Transmit Identifier (Tx Buffer 3)
        __IO Arbin_CAN1TDA TID3;

        ///Offset: 0x00000058 - Transmit data bytes 1-4 (Tx Buffer 3)
        __IO Arbin_CAN1TDA TDA3;

        ///Offset: 0x0000005C - Transmit data bytes 5-8 (Tx Buffer 3)
        __IO Arbin_CAN1TDB TDB3;
    } Arbin_LPC_CAN_TypeDef;

#endif



#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

    /******************************************************************************/
    /*                         Peripheral memory map                              */
    /******************************************************************************/
    /* Base addresses                                                             */
#define Arbin_FLASH_BASE        (0x00000000UL)
#define Arbin_RAM_BASE          (0x10000000UL)
#define Arbin_PERI_RAM_BASE     (0x20000000UL)
#define Arbin_APB0_BASE         (0x40000000UL)
#define Arbin_APB1_BASE         (0x40080000UL)
#define Arbin_AHBRAM1_BASE      (0x20004000UL)
#define Arbin_AHB_BASE          (0x20080000UL)
#define Arbin_CM3_BASE          (0xE0000000UL)

    /* APB0 peripherals                                                           */
#define Arbin_WDT_BASE          (Arbin_APB0_BASE + 0x00000)
#define Arbin_TIM0_BASE         (Arbin_APB0_BASE + 0x04000)
#define Arbin_TIM1_BASE         (Arbin_APB0_BASE + 0x08000)
#define Arbin_UART0_BASE        (Arbin_APB0_BASE + 0x0C000)
#define Arbin_UART1_BASE        (Arbin_APB0_BASE + 0x10000)
#define Arbin_PWM0_BASE         (Arbin_APB0_BASE + 0x14000)
#define Arbin_PWM1_BASE         (Arbin_APB0_BASE + 0x18000)
#define Arbin_I2C0_BASE         (Arbin_APB0_BASE + 0x1C000)

#define Arbin_RTC_BASE          (Arbin_APB0_BASE + 0x24000)
#define Arbin_GPIOINT_BASE      (Arbin_APB0_BASE + 0x28080)
#define Arbin_IOCON_BASE        (Arbin_APB0_BASE + 0x2C000)
#define Arbin_SSP1_BASE         (Arbin_APB0_BASE + 0x30000)
#define Arbin_ADC_BASE          (Arbin_APB0_BASE + 0x34000)
#define Arbin_CANAF_RAM_BASE    (Arbin_APB0_BASE + 0x38000)
#define Arbin_CANAF_BASE        (Arbin_APB0_BASE + 0x3C000)
#define Arbin_CANCR_BASE        (Arbin_APB0_BASE + 0x40000)
#define Arbin_CAN1_BASE         (Arbin_APB0_BASE + 0x44000)
#define Arbin_CAN2_BASE         (Arbin_APB0_BASE + 0x48000)
#define Arbin_I2C1_BASE         (Arbin_APB0_BASE + 0x5C000)

    /* APB1 peripherals                                                           */
#define Arbin_SSP0_BASE         (Arbin_APB1_BASE + 0x08000)
#define Arbin_DAC_BASE          (Arbin_APB1_BASE + 0x0C000)
#define Arbin_TIM2_BASE         (Arbin_APB1_BASE + 0x10000)
#define Arbin_TIM3_BASE         (Arbin_APB1_BASE + 0x14000)
#define Arbin_UART2_BASE        (Arbin_APB1_BASE + 0x18000)
#define Arbin_UART3_BASE        (Arbin_APB1_BASE + 0x1C000)
#define Arbin_I2C2_BASE         (Arbin_APB1_BASE + 0x20000)
#define Arbin_UART4_BASE        (Arbin_APB1_BASE + 0x24000)
#define Arbin_I2S_BASE          (Arbin_APB1_BASE + 0x28000)
#define Arbin_SSP2_BASE         (Arbin_APB1_BASE + 0x2C000)

#define Arbin_MCI_BASE          (Arbin_APB1_BASE + 0x40000)
#define Arbin_SC_BASE           (Arbin_APB1_BASE + 0x7C000)

    /* AHB peripherals                                                            */
#define Arbin_GPDMA_BASE        (Arbin_AHB_BASE  + 0x00000)

#define Arbin_EMAC_BASE         (Arbin_AHB_BASE  + 0x04000)

#define Arbin_CRC_BASE          (Arbin_AHB_BASE  + 0x10000)

#define Arbin_EMC_BASE          (Arbin_AHB_BASE  + 0x1C000)


    /******************************************************************************/
    /*                         Peripheral declaration                             */
    /******************************************************************************/
#define Arbin_SC                ((Arbin_LPC_SC_TypeDef        *) Arbin_SC_BASE       )
#define Arbin_WDT               ((Arbin_LPC_WDT_TypeDef       *) Arbin_WDT_BASE      )
#define Arbin_TIM0              ((Arbin_LPC_TIM_TypeDef       *) Arbin_TIM0_BASE     )
#define Arbin_TIM1              ((Arbin_LPC_TIM_TypeDef       *) Arbin_TIM1_BASE     )
#define Arbin_TIM2              ((Arbin_LPC_TIM_TypeDef       *) Arbin_TIM2_BASE     )
#define Arbin_TIM3              ((Arbin_LPC_TIM_TypeDef       *) Arbin_TIM3_BASE     )

    //#define Arbin_I2C0              ((Arbin_LPC_I2C_TypeDef       *) Arbin_I2C0_BASE     )
    //#define Arbin_I2C1              ((Arbin_LPC_I2C_TypeDef       *) Arbin_I2C1_BASE     )
    //#define Arbin_I2C2              ((Arbin_LPC_I2C_TypeDef       *) Arbin_I2C2_BASE     )
#define Arbin_RTC               ((Arbin_LPC_RTC_TypeDef       *) Arbin_RTC_BASE      )
#define Arbin_IOCON             ((Arbin_LPC_IOCON_TypeDef     *) Arbin_IOCON_BASE    )
#define Arbin_SSP0              ((Arbin_LPC_SSP_TypeDef       *) Arbin_SSP0_BASE     )
#define Arbin_SSP1              ((Arbin_LPC_SSP_TypeDef       *) Arbin_SSP1_BASE     )
#define Arbin_SSP2              ((Arbin_LPC_SSP_TypeDef       *) Arbin_SSP2_BASE     )

    //#define Arbin_CANAF_RAM         ((Arbin_LPC_CANAF_RAM_TypeDef *) Arbin_CANAF_RAM_BASE)
    //#define Arbin_CANAF             ((Arbin_LPC_CANAF_TypeDef     *) Arbin_CANAF_BASE    )
    //#define Arbin_CANCR             ((Arbin_LPC_CANCR_TypeDef     *) Arbin_CANCR_BASE    )
    //#define Arbin_CAN1              ((Arbin_LPC_CAN_TypeDef       *) Arbin_CAN1_BASE     )
    //#define Arbin_CAN2              ((Arbin_LPC_CAN_TypeDef       *) Arbin_CAN2_BASE     )
    //#define Arbin_GPDMA             ((Arbin_LPC_GPDMA_TypeDef     *) Arbin_GPDMA_BASE    )

#define Arbin_EMC               ((Arbin_LPC_EMC_TypeDef       *) Arbin_EMC_BASE      )


#ifdef __cplusplus
}
#endif
#endif  // Arbin_LPC4078_H__
