/**********************************************************************
* $Id$		system_LPC407x_8x_177x_8x.h			2011-06-02
*//**
* @file		system_LPC407x_8x_177x_8x.h
* @brief	CMSIS Cortex-M3 Device Peripheral Access Layer Source File
*			for the NXP LPC Device Series
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#ifndef __SYSTEM_LPC407x_8x_177x_8x_H
#define __SYSTEM_LPC407x_8x_177x_8x_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    extern uint32_t SystemCoreClock;      /*!< System Clock Frequency (Core Clock)  	*/
    extern uint32_t PeripheralClock;	    /*!< Peripheral Clock Frequency (Pclk) 	    */
    extern uint32_t EMCClock;			        /*!< EMC Clock                              */
    extern uint32_t USBClock;			        /*!< USB Frequency 						              */
    // External Interrupts
    extern void ENET_IRQHandler (void);
    extern void RTC_IRQHandler(void);
    extern void TIMER0_IRQHandler (void);
    extern void TIMER1_IRQHandler(void);
    extern void TIMER2_IRQHandler(void);
    extern void TIMER3_IRQHandler(void);
    //extern void EINT0_IRQHandler(void);
    extern void EINT1_IRQHandler (void);
    extern void I2C0_IRQHandler (void);
    extern void I2C1_IRQHandler (void);
    extern void I2C2_IRQHandler (void);
    extern void WDT_IRQHandler(void);
    extern void UART0_IRQHandler(void);
    extern void UART1_IRQHandler(void);
    extern void UART2_IRQHandler(void);
    extern void UART3_IRQHandler(void);
    extern void CAN_IRQHandler(void);
    extern void PWM1_IRQHandler(void);
    //extern void DMA_IRQHandler(void);
    /**
    * Initialize the system
    *
    * @param  none
    * @return none
    *
    * @brief  Setup the microcontroller system.
    *         Initialize the System and update the SystemCoreClock variable.
    */
    extern void SystemInit (void);

    /**
    * Update SystemCoreClock variable
    *
    * @param  none
    * @return none
    *
    * @brief  Updates the SystemCoreClock with current core Clock
    *         retrieved from cpu registers.
    */
    extern void SystemCoreClockUpdate (void);

    /*----------------------------------------------------------------------------
    Define clocks
    *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32768UL)        /* RTC oscillator frequency           */
#define IRC_OSC     (12000000UL)        /* Internal RC oscillator frequency   */
#define WDT_OSC		  (  500000UL)		/* Internal WDT oscillator frequency  */


#define PCLCD       (1<<0)  // (0 bit): LCD controller power/clock enable
#define PCTIM0      (1<<1)  // (1 bit): Timer/Counter 0 power/clock enable
#define PCTIM1      (1<<2)  // (2 bit): Timer/Counter 1 power/clock enable
#define PCUART0     (1<<3)  // (3 bit): UART 0 power/clock enable
#define PCUART1     (1<<4)  // (4 bit): UART 1 power/clock enable
#define PCPWM0      (1<<5)  // (5 bit): PWM0 power/clock enable
#define PCPWM1      (1<<6)  // (6 bit): PWM1 power/clock enable
#define PCI2C0      (1<<7)  // (7 bit): I2C 0 interface power/clock enable
#define PCUART4     (1<<8)  // (8 bit): UART 4 power/clock enable
#define PCRTC       (1<<9)  // (9 bit): RTC and Event Recorder power/clock enable
#define PCSSP1      (1<<10)  // (10 bit): SSP 1 interface power/clock enable
#define PCEMC       (1<<11)  // (11 bit): External Memory Controller power/clock enable
#define PCADC       (1<<12)  // (12 bit): A/D converter power/clock enable
#define PCCAN1      (1<<13)  // (13 bit): CAN controller 1 power/clock enable
#define PCCAN2      (1<<14)  // (14 bit): CAN controller 2 power/clock enable
#define PCGPIO      (1<<15)  // (15 bit): IOCON, GPIO, and GPIO interrupts power/clock enable
#define PCReserved      (1<<16)  // (16 bit): - Reserved. Read value is undefined, only zero should be written.
#define PCMCPWM     (1<<17)  // (17 bit): Motor Control PWM power/clock enable
#define PCQEI       (1<<18)  // (18 bit): Quadrature encoder interface power/clock enable
#define PCI2C1      (1<<19)  // (19 bit): I2C 1 interface power/clock enable
#define PCSSP2      (1<<20)  // (20 bit): SSP 2 interface power/clock enable
#define PCSSP0      (1<<21)  // (21 bit): SSP 0 interface power/clock enable
#define PCTIM2      (1<<22)  // (22 bit): Timer 2 power/clock enable
#define PCTIM3      (1<<23)  // (23 bit): Timer 3 power/clock enable
#define PCUART2     (1<<24)  // (24 bit): UART 2 power/clock enable
#define PCUART3     (1<<25)  // (25 bit): UART 3 power/clock enable
#define PCI2C2      (1<<26)  // (26 bit): I2C 2 interface power/clock enable
#define PCI2S       (1<<27)  // (27 bit): I2S interface power/clock enable
#define PCSDC       (1<<28)  // (28 bit): SD Card interface power/clock enable
#define PCGPDMA     (1<<29)  // (29 bit): GPDMA function power/clock enable
#define PCENET      (1<<30)  // (30 bit): Ethernet block power/clock enable
#define PCUSB       (1<<31)  // (31 bit): USB interface power/clock enable

    /*
    //-------- <<< end of configuration section >>> ------------------------------
    */

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LPC407x_8x_177x_8x_H */
