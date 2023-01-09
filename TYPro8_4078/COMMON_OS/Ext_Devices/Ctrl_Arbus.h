/*
Ctrl_Arbus.h

Author:			sam
Description:	Define address and constants for the M5272 Application Board
*/

#ifndef Ctrl_Arbus_H
#define Ctrl_Arbus_H

#include <stdio.h>
#include <string.h>
#include "../../COMMON/Datatypes.h"
#include "../../HWD/HWDLIB/ADC16bit_AD7656.h"

//#include "../../COMMON/includes.h"


#define EXT_IO_Base_ADDRESS		    (0x90000000)   //for arbin bus write  2468  0x81000000
//////////////////////////////////////////////////

/*  Read Address   */
#define ARBIN_BUS_READ_ADDRESS		(EXT_IO_Base_ADDRESS+0x0004)  //0x8100 0004
////////////////////////////////////////////////////////////////////

#define ADC_READ_ADDRESS		      (EXT_IO_Base_ADDRESS+0x0001)
#define ADC_STATUS_ADDRESS		    (EXT_IO_Base_ADDRESS+0x000a)
#define ADC_TO_BUFFER_ADDRESS	    (EXT_IO_Base_ADDRESS+0x000e)


/*  Write Address   */
#define ARBINBUS_CTRB_ADDRESS	    (EXT_IO_Base_ADDRESS+0x0000)
#define ARBINBUS_DIR_ADDRESS	    (EXT_IO_Base_ADDRESS+0x0002)
#define UART_ENABLE_ADDRESS		    (EXT_IO_Base_ADDRESS+0x0004)
#define DAC_CHANNEL_ADDRESS	        (EXT_IO_Base_ADDRESS+0x0006)
#define ADC_READ_CONVERT_ADDRESS	(EXT_IO_Base_ADDRESS+0x0008)
#define DAC_WRITE_ADDRESS	        (EXT_IO_Base_ADDRESS+0x000a)
#define DACBUFFER_WR_ADDRESS	    (EXT_IO_Base_ADDRESS+0x000c)
#define ARBIN_BUS_WRITE_ADDRESS   	(EXT_IO_Base_ADDRESS + 0x0004)
////////////////////////////////////////////////////////////////
#define ADC_SELECT_ADDRESS	        (EXT_IO_Base_ADDRESS+0x0010)
#define ADC_TRIGGER_ADDRESS	        (EXT_IO_Base_ADDRESS+0x0012)



/* Bit level definitions  */

#define ARBINBUS_CTRB_ARBIN	    (0x0001)
#define ARBINBUS_CTRB_READ	    (0x0002)
#define ARBINBUS_CTRB_ADDRW	    (0x0004)
#define ARBINBUS_CTRB_DATAW	    (0x0008)
#define ARBINBUS_ARBINDIR_OUT	(0x0000)
#define ARBINBUS_ARBINDIR_IN	(0x0001)
#define UART_CTR_ENABLE     	(0x0001)
#define UART_CTR_DISABLE     	(0x0000)


/* Constant Definitions  */
#define ARBIN_BUS_DELAY_S     	(1)
#define ARBIN_BUS_DELAY_M     	(2)
#define ARBIN_BUS_DELAY_L     	(4)
//////////////////////////////////////////

#define ARBIN_BUS_DELAY_SX  	(5)
#define ARBIN_BUS_DELAY_MX  	(25)
#define ARBIN_BUS_DELAY_LX  	(50)

#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );

//zl
//Macro to handle arbin bus, 32 bits bus, 8, 16, and 32 bits bus
//Arbin Bus Write
#define cpu_iowr_8(ADDR,DATA)   *((volatile uint8 *)(ADDR)) = (DATA)
#define cpu_iowr_16(ADDR,DATA)  *((volatile uint16 *)(ADDR)) = (DATA)
#define cpu_iowr_32(ADDR,DATA)  *((volatile uint32 *)(ADDR)) = (DATA)

//#define ADC_ADD     0X81000000     //adc address
//#define WriteCPLD        (*(volatile unsigned long *)(ADC_ADD))

//Arbin bus read
#define cpu_iord_8(ADDR)        *((volatile uint8 *)(ADDR))
#define cpu_iord_16(ADDR)       *((volatile uint16 *)(ADDR))
#define cpu_iord_32(ADDR)       *((volatile uint32 *)(ADDR))
//////////////////////////////////////////////////////////////////////

//=============================


class Ctrl_Arbus
{
    //Attribute
public:

    //Operation
public:
    static void Init(void);

    static void WriteBus(uint16 addr,uint16 val);
    static int16 ReadBus(uint16 addr);
    static void WriteBusX(uint16 addr,uint16 val);
    static int16 ReadBusX(uint16 addr);

    static void ReadAdc(unsigned char Device_Number,unsigned long AdcNo, int16& Current, int16& Voltage);
    static void WriteMcDac(uint8 DacNo, uint16 Value);
    static void UnSafeCmd(void);
};


#endif	// Ctrl_Arbus_H

