/****************************************Copyright (c)****************************************************
**                                 http://www.PowerAVR.com
**								   http://www.PowerMCU.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.c
** Last modified Date:  2010-05-12
** Last Version:        V1.00
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          PowerAVR
** Created date:        2010-05-10
** Version:             V1.00
** Descriptions:        编写示例代码
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "includes.h"

//#define FPCLK         60000000
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

unsigned char UARTFunc::gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实接收缓冲区
unsigned int  UARTFunc::gl_Rx_first[MAX_UART_NUM];//  取数指针
unsigned int  UARTFunc::gl_Rx_last[MAX_UART_NUM]; //  存数指针
unsigned long int  UARTFunc::gl_Rx_Error[MAX_UART_NUM];//  存数错误计数器

unsigned char UARTFunc::gl_Tx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE];// 真实发送缓冲区
unsigned int  UARTFunc::gl_Tx_first[MAX_UART_NUM];//  取数指针
unsigned int  UARTFunc::gl_Tx_last[MAX_UART_NUM];//  存数指针
#pragma arm section




void UARTFunc::uart_putcmd (unsigned char ChanNo, unsigned char *pCmd,unsigned short uw16_Length)
{
    //  驱动串口发送输出命令，发送时只发送不接收。

    //	 switch(ChanNo)
    //	 {
    //		 case 0:
    //			 UART0_SendBuffer(pCmd,uw16_Length);
    //		   break;
    //		 case 1:
    //			 UART1_SendBuffer(pCmd,uw16_Length);
    //		   break;
    //		 case 2:
    //			 UART2_SendBuffer(pCmd,uw16_Length);
    //		   break;
    //		 case 3:
    //			 UART3_SendBuffer(pCmd,uw16_Length);
    //		   break;
    //		 default:
    //			 break;
    //   }

    if((uw16_Length == 0) || (uw16_Length > MAX_UART_BUF_SIZE))//无数据和溢出保护
    {
        return;
    }

    unsigned int i;
    unsigned short uw16_count;
		unsigned int TxLastTemp = gl_Tx_last[ChanNo];//避免中断取值造成溢出
		unsigned int TxFirstTemp = gl_Tx_first[ChanNo];

    if(TxLastTemp >= TxFirstTemp)   //存数大于取数
    {
        uw16_count = MAX_UART_BUF_SIZE - (TxLastTemp - TxFirstTemp);   //  存指针 - 取指针 = 长度
    }
    else
    {
        uw16_count = TxFirstTemp - TxLastTemp;
    }

    if((uw16_Length > uw16_count) || (uw16_count > MAX_UART_BUF_SIZE))  // 无空存储区和溢出保护
    {
        return;
    }
	
#if (USE_AS_AUXMCU == 1)
	if(ChanNo == 0)//MTCI发送命令前先清除上一次接收缓存
		UARTFunc::uart_empty(0);
#endif

    for (i=0; i < uw16_Length; i++)
    {
        gl_Tx_buf [ChanNo][gl_Tx_last[ChanNo]] = *pCmd;
        pCmd++;
        gl_Tx_last[ChanNo] = (gl_Tx_last[ChanNo]+1) % MAX_UART_BUF_SIZE;
    }

    UARTSend( ChanNo, 1);

}

#if  USE_AS_AUXMCU == 1
unsigned int UARTFunc::uart_getcmd (unsigned char ChanNo,unsigned char *pCmd)
{
    //  对串口缓冲区数据进行接收验证长度和命令
    unsigned int ByteNum = 0;
		unsigned int UartRecvLen = uart_bytenum(ChanNo);
	
    while(UartRecvLen--)   // 取数，取到相同退出。
    {
        *pCmd = gl_Rx_buf[ChanNo][gl_Rx_first[ChanNo]];  //  转存到其它缓冲区 gR_CmdBuf[MAX_UART_BUF_SIZE]
        gl_Rx_first[ChanNo] = ( gl_Rx_first[ChanNo] + 1 )%MAX_UART_BUF_SIZE;    //  取数指针更改
        pCmd++;    //  转存到其它缓冲区 gR_CmdBuf[MAX_UART_BUF_SIZE]更改
			
				ByteNum++;

//        if( gl_Rx_first[ChanNo] == gl_Rx_last[ChanNo] )
//        {
//            ARBIN_DELAY_US(17);  // 延时17微秒
//        }
    }

    return ByteNum;
}
#endif

unsigned int UARTFunc::uart_bytenum(unsigned char ChanNo)
{
		unsigned int RxLastTemp = gl_Rx_last[ChanNo];//避免中断取值造成溢出
		unsigned int RxFirstTemp = gl_Rx_first[ChanNo];
		unsigned int RetValue = 0;
	
    // 计算接收缓冲区中接收数据长度
    if(RxLastTemp >= RxFirstTemp)   //  (存数大于取数)
    {
				RetValue = RxLastTemp - RxFirstTemp;
    }
    else
    {
				RetValue = (RxLastTemp+(MAX_UART_BUF_SIZE - RxFirstTemp));
    }
		
		if(RetValue > MAX_UART_BUF_SIZE)
		{
				gl_Rx_first[ChanNo] = 0;
				gl_Rx_last[ChanNo] = 0;
				RetValue = 0;
		}

		return RetValue;
}

void UART0_IRQHandler (void)
{
    UARTFunc::UARTXHandler_Processing(0);
}

void UART1_IRQHandler ()
{
    UARTFunc::UARTXHandler_Processing(1);
}

void UART2_IRQHandler ()
{
    UARTFunc::UARTXHandler_Processing(2);
}

void UART3_IRQHandler ()
{
    UARTFunc::UARTXHandler_Processing(3);
}

void UARTFunc::UARTXHandler_Processing (unsigned char uc_UartCh)
{
    UChar8 uc_IIRValue = 0, uc_LSRValue = 0;
    UChar8 uc_Dummy = uc_Dummy;
    LPC_UART_TypeDef *LPC_UARTall;

    switch(uc_UartCh)
    {
    case 0:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART0_BASE;
        break;
    case 1:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART1_BASE;
        break;
    case 2:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART2_BASE;
        break;
    case 3:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART3_BASE;
        break;
    default:
        return;
    }
    uc_IIRValue = LPC_UARTall->IIR;
    uc_IIRValue >>= 1;
    uc_IIRValue &= 0x07;			// check bit 1~3, interrupt identification //

    if ( uc_IIRValue == IIR_RLS )		// Receive Line Status //
    {
        uc_LSRValue = LPC_UARTall->LSR;
        // Receive Line Status //
        if ( uc_LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
        {
            uc_Dummy = LPC_UARTall->RBR;		//Dummy read on RX to clear
            return;
        }
        if ( uc_LSRValue & LSR_RDR )	// Receive Data Ready //
        {
            uc_Dummy = LPC_UARTall->RBR;
            if (((gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE) != gl_Rx_first[uc_UartCh])
            {
                gl_Rx_Error[uc_UartCh] = 0;
                gl_Rx_buf[uc_UartCh][gl_Rx_last[uc_UartCh]] = uc_Dummy;
                gl_Rx_last[uc_UartCh]=(gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE;
                //LPC_UARTall->IER = IER_RBR | IER_THRE | IER_RLS;     //Enable UART0 Interrupt
            }
            else
            {
                if(++gl_Rx_Error[uc_UartCh] >= MAX_UART_BUF_SIZE) // 20181126 yy
                {
                    gl_Rx_last[uc_UartCh] = 0;
                    gl_Rx_first[uc_UartCh] = 0;
                    gl_Rx_Error[uc_UartCh] = 0;
                }
            }
        }
    }
    else if ( uc_IIRValue == IIR_RDA )	// Receive Data Available //
    {
        // Receive Data Available //
        uc_Dummy = LPC_UARTall->RBR;

        if (((gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE) != gl_Rx_first[uc_UartCh])
        {
            gl_Rx_Error[uc_UartCh] = 0;
            gl_Rx_buf[uc_UartCh][gl_Rx_last[uc_UartCh]] = uc_Dummy;
            gl_Rx_last[uc_UartCh]=(gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE;
            //LPC_UARTall->IER = IER_RBR | IER_THRE | IER_RLS;     //Enable UART0 Interrupt
        }
        else
        {
            if(++gl_Rx_Error[uc_UartCh] >= MAX_UART_BUF_SIZE) // 20181126 yy
            {
                gl_Rx_last[uc_UartCh] = 0;
                gl_Rx_first[uc_UartCh] = 0;
                gl_Rx_Error[uc_UartCh] = 0;
            }
        }
    }
    else if ( uc_IIRValue == IIR_CTI )	// Character timeout indicator //
    {
        uc_Dummy = IIR_CTI;
    }
    else if ( uc_IIRValue == IIR_THRE )	// THRE, transmit holding register empty //
    {
        uc_LSRValue = LPC_UARTall->LSR;		// Check status in the LSR to see if
        // valid data in U0THR or not //
        if ( uc_LSRValue & LSR_THRE )
        {
            UARTSend( uc_UartCh, 1);
        }
    }
}



/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**						on the data length
**
** parameters:			portNum, buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UARTFunc::UARTSend(unsigned char uc_UartCh, unsigned int Length )
{
    LPC_UART_TypeDef *LPC_UARTall;

    if(gl_Tx_first[uc_UartCh] == gl_Tx_last[uc_UartCh])
    {
        return;
    }

    switch(uc_UartCh)
    {
    case 0:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART0_BASE;
        break;
    case 1:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART1_BASE;
        break;
    case 2:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART2_BASE;
        break;
    case 3:
        LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART3_BASE;
        break;
    default:
        return;
    }

    while ( Length != 0 )
    {
        // THRE status, contain valid data //
        while ( !(LPC_UARTall->LSR & LSR_THRE) );

        LPC_UARTall->THR = gl_Tx_buf [uc_UartCh][gl_Tx_first[uc_UartCh]];
        gl_Tx_first[uc_UartCh] = (gl_Tx_first[uc_UartCh] + 1) % MAX_UART_BUF_SIZE;
        Length --;
    }
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART0 port, setup pin select,
**						clock, baudrate, data bits,parity, stop bits, FIFO, etc.
**
** parameters:			portNum(0, 1, 2, 3, 4), UART baudrate, data bits, parity, stop bits
** Returned value:		true or false, return false only if the
**						interrupt handler can't be installed to the
**						VIC table
**
*****************************************************************************/
UWord32 UARTFunc::UARTInit( UWord32 uw32_PortNum, unsigned char uc_BaudRate, unsigned char uc_DataBits, unsigned char uc_Parity, unsigned char uc_StopBits )
{
    //	UWord32 uw32_BaudRate;
    UWord32 IRQ_IntNumber = UART0_IRQn;
    uint16_t usFdiv;
    unsigned char uc_RetrunFlag = FALSE;
    unsigned char LCR_WLS;  // World length select
    unsigned char LCR_SBS;	// Stop Bit Select
    unsigned char LCR_PE;   // Parity Enable
    unsigned char LCR_PS; // Parity Select


    volatile uint8_t   *U_LCR;
    volatile uint8_t   *U_DLM;
    volatile uint8_t   *U_DLL;
    volatile uint8_t   *U_TER;
    //    volatile uint8_t   *U_FCR;
    volatile uint32_t  *U_IER;

    switch(uw32_PortNum)
    {
    case 0:
        // 4串口板UART0  TXD1(P0.2)  RXD1(P0.3)
        LPC_IOCON->P0_2 |= 0x31;         // Pin P0.2 used as TXD0 (Com0) Pull-up resistor enabled and Hysteresis enabled.
        LPC_IOCON->P0_3 |= 0x31;         // Pin P0.3 used as RXD0 (Com0) Pull-up resistor enabled and Hysteresis enabled.
        LPC_SC->PCONP  |= (1<<3);        //开启UART0电源

        U_LCR = &(LPC_UART0->LCR);
        U_DLM = &(LPC_UART0->DLM);
        U_DLL = &(LPC_UART0->DLL);
        U_IER = &(LPC_UART0->IER);
        U_TER = &(LPC_UART0->TER);
        //            U_FCR = &(LPC_UART0->FCR);
        break;
    case 1:
        // 4串口板UART1  TXD2(P2.0)  RXD2(P2.1)
        LPC_IOCON->P2_0 |= 0x32;         // 解决串口频频进中断造成死机问题。 增加上拉电阻和滞后设置。
        LPC_IOCON->P2_1 |= 0x32;
        LPC_SC->PCONP  |= (1<<4);        //开启UART1电源

        U_LCR = &(LPC_UART1->LCR);
        U_DLM = &(LPC_UART1->DLM);
        U_DLL = &(LPC_UART1->DLL);
        U_IER = &(LPC_UART1->IER);
        U_TER = &(LPC_UART1->TER);
        //            U_FCR = &(LPC_UART1->FCR);
        break;
    case 2:
        // 4串口板UART2  TXD3(P0.10) RXD3(P0.11)
        LPC_IOCON->P0_10 |= 0x31;          // TXD3  Pull-up resistor enabled and Hysteresis enabled.   注意串口I2C2是否占用
        LPC_IOCON->P0_11 |= 0x31;          // RXD3  Pull-up resistor enabled and Hysteresis enabled.
        LPC_SC->PCONP  |= (1<<24);        //开启UART0电源

        //LPC_UART2->FCR  = 0x06;

        U_LCR = &(LPC_UART2->LCR);
        U_DLM = &(LPC_UART2->DLM);
        U_DLL = &(LPC_UART2->DLL);
        U_IER = &(LPC_UART2->IER);
        U_TER = &(LPC_UART2->TER);
        //            U_FCR = &(LPC_UART2->FCR);
        break;
    case 3:
        // 4串口板UART3  TXD4(P4.28) RXD4(P4.29)
        LPC_IOCON->P4_28 |= 0x32;            //  TXD4 Pull-up resistor enabled and Hysteresis enabled.
        LPC_IOCON->P4_29 |= 0x32;            //  RXD4 Pull-up resistor enabled and Hysteresis enabled.
        LPC_SC->PCONP  |= (1<<25);        //开启UART3电源

        //LPC_UART3->FCR  = 0x06;

        U_LCR = &(LPC_UART3->LCR);
        U_DLM = &(LPC_UART3->DLM);
        U_DLL = &(LPC_UART3->DLL);
        U_IER = &(LPC_UART3->IER);
        U_TER = &(LPC_UART3->TER);
        //            U_FCR = &(LPC_UART3->FCR);
        break;
    default:
        return 0;
        //break;
    }

    //	switch (uc_BaudRate)
    //	{
    //
    //	 case TC_BR_1200:
    //	       uw32_BaudRate=1200;
    //		   break;
    //	 case TC_BR_1800:
    //	       uw32_BaudRate=1800;
    //		   break;
    //	 case TC_BR_2400:
    //	       uw32_BaudRate=2400;
    //		   break;
    //	 case TC_BR_4800:
    //	       uw32_BaudRate=4800;
    //		   break;
    //	 case TC_BR_9600:
    //	       uw32_BaudRate=9600;
    //		   break;
    //	 case TC_BR_14400:
    //	       uw32_BaudRate=14400;
    //		   break;
    //	 case TC_BR_19200:
    //	       uw32_BaudRate=19200;
    //		   break;
    //	 case TC_BR_38400:
    //	       uw32_BaudRate=38400;
    //		   break;
    //	 case TC_BR_57600:
    //	       uw32_BaudRate=57600;
    //		   break;
    //	case TC_BR_115200:
    //	       uw32_BaudRate=115200;
    //		   break;
    //	default:
    //	       uw32_BaudRate=9600;
    //		   break;
    //	}
    UWord32 uw32_BaudRate[10] = {
        1200,
        1800,
        2400,
        4800,
        9600,
        14400,
        19200,
        38400,
        57600,
        115200
    };
    if(uc_BaudRate>9)
        uc_BaudRate = 4;	// Default BaudRate is 9600

    //LPC_UART0->LCR  = 0x83;                          //允许设置波特率
    *U_LCR |= 0x80;
    usFdiv = (PeripheralClock/ 16) / uw32_BaudRate[uc_BaudRate];  //(FPCLK/ 16)/uw32_BaudRate  // 设置波特率
    *U_DLM  = usFdiv / 256;
    *U_DLL  = usFdiv % 256;
    *U_LCR  = 0x03;                      // 锁定波特率

    switch (uc_DataBits)
    {
    case TC_DATA_BIT_5:
        LCR_WLS=0;
        break;
    case TC_DATA_BIT_6:
        LCR_WLS=1;
        break;
    case TC_DATA_BIT_7:
        LCR_WLS=2;
        break;
    case TC_DATA_BIT_8:
        LCR_WLS=3;
        break;
    default:
        LCR_WLS=3;
        break;
    }

    switch (uc_Parity)
    {
    case TC_PARITY_EVEN:
        LCR_PE=8;
        LCR_PS=0x10;   //32 zyx 181019
        break;

    case TC_PARITY_ODD:
        LCR_PE=8;
        LCR_PS=0;
        break;
    case TC_PARITY_NONE:
        LCR_PE=0;
        LCR_PS=0;
        break;
    default:
        LCR_PE=0;
        LCR_PS=0;
        break;
    }

    switch (uc_StopBits)
    {
    case TC_STOP_BIT_ONE:
        LCR_SBS=0;
        break;
        //	case TC_STOP_BIT_OTHER:	  // reserved for stop bit as 1.5
        //	    LCR_WLS=0;
        //		LCR_SBS=4;
        //		break;
    case TC_STOP_BIT_TWO:
        LCR_SBS=4;
        break;
    default:
        LCR_SBS=0;
        break;
    }
    *U_LCR = LCR_WLS|LCR_SBS|LCR_PE|LCR_PS;   // Set the LCR by the combination of data bits, stop bits, parity

    //str_pUart->UXIIR_FCR = 0x00;		// DisEnable and reset TX and RX FIFO. //
    //*U_FCR  = 0x89; 				 //enable fifo and DMA & 8 byte trigger


    IRQFunc::install_irq( (IRQn_Type)(IRQ_IntNumber+uw32_PortNum), HIGHEST_PRIORITY +12 );

    //*U_IER = IER_THRE | IER_RLS;	// Enable UART interrupt //disable rx int (use dma rx)
    *U_IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART interrupt //

    *U_TER = TER_TXEN_ON;

    //str_Uart[uw32_PortNum].gl_Rx_last = str_Uart[uw32_PortNum].gl_Rx_first = 0;
    gl_Rx_last[uw32_PortNum] = 0;
    gl_Rx_first[uw32_PortNum] = 0;
    gl_Tx_last[uw32_PortNum] = 0;
    gl_Tx_first[uw32_PortNum] = 0;
    gl_Rx_Error[uw32_PortNum] = 0;

    memset(gl_Rx_buf, 0, sizeof(unsigned char)*MAX_UART_NUM*MAX_UART_BUF_SIZE); // 真实接收缓冲区
    memset(gl_Tx_buf, 0, sizeof(unsigned char)*MAX_UART_NUM*MAX_UART_BUF_SIZE);// 真实发送缓冲区

    return (uc_RetrunFlag);
}


#if(USE_AS_AUXMCU ==1)
void UARTFunc::UARTSendFIFO(unsigned char uc_UartCh)
{
    // 16 byte Receive and Transmit FIFOs.
    //  LPC_UART_TypeDef *LPC_UARTall;
    UWord16 uw16_int = 0;
    UWord16 uw16_remain = 0;
    unsigned short uw16_count = 0;

    if(gl_Tx_last[uc_UartCh] >= gl_Tx_first[uc_UartCh])   //  (存数大于取数)
        uw16_count = (gl_Tx_last[uc_UartCh] - gl_Tx_first[uc_UartCh]);   //  存数指针 - 取数指针 = 长度
    else
        uw16_count = (gl_Tx_last[uc_UartCh]+(MAX_UART_BUF_SIZE - gl_Tx_first[uc_UartCh]));

    if( uw16_count == 0)  // 空存储区保护
        return;

    uw16_int = uw16_count/16;
    uw16_remain = uw16_count& 0x0F;  //uw16_count%16;

    /*	switch(uc_UartCh)
    	{
    	case 0:
    		//			LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART0_BASE;
    	case 1:
    		//			LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART1_BASE;
    	case 2:
    		//		  LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART2_BASE;
    	case 3:
    		//		  LPC_UARTall = (LPC_UART_TypeDef *)LPC_UART3_BASE;
    		break;
    	default:
    		return;
    	}   */

    if ( uw16_int != 0 )  // FIFO Full Send
    {
        // THRE status, contain valid data //
        UARTSend(uc_UartCh, 16 );
        return;
    }

    if( uw16_remain != 0)  // FIFO Remain Send
        UARTSend(uc_UartCh, uw16_remain );
}


// unsigned short int  UARTFunc::UARTCheck_SendByte(unsigned char uc_UartCh)
// {
// 	unsigned short int uw16_SendCount = 0;

// 	if(gl_Tx_last[uc_UartCh] >= gl_Tx_first[uc_UartCh])   //  (存数大于取数)
// 	{
// 		uw16_SendCount = (gl_Tx_last[uc_UartCh] - gl_Tx_first[uc_UartCh]);   //  存数指针 - 取数指针 = 长度
// 	}
// 	else
// 	{
// 		uw16_SendCount = (gl_Tx_last[uc_UartCh]+(MAX_UART_BUF_SIZE - gl_Tx_first[uc_UartCh]));
// 	}

// 	return (uw16_SendCount);
// }


void UARTFunc::uart_empty(unsigned char ChanNo)
{
    //  清空串口接收缓冲区，打开中断
    gl_Rx_last[ChanNo] = 0;
    gl_Rx_first[ChanNo] = 0;
    gl_Rx_Error[ChanNo] = 0;

    volatile uint32_t  *U_IER;

    switch(ChanNo)
    {
    case 0:
        U_IER = &(LPC_UART0->IER);
        break;
    case 1:
        U_IER = &(LPC_UART1->IER);
        break;
    case 2:
        U_IER = &(LPC_UART2->IER);
        break;
    case 3:
        U_IER = &(LPC_UART3->IER);
        break;
    default:
        return ;
        //break;
    }

    //*U_IER = IER_RBR|IER_THRE|IER_RLS; //Enable UART0 Interruput
    *U_IER|= (IER_RBR|IER_RLS); //Enable UART0 Interruput
}

bool UARTFunc::uart_getcmd_AppointLong (unsigned char ChanNo,unsigned char *pCmd, unsigned int len)
{
    //  对串口缓冲区数据进行接收验证长度和命令
    // 	unsigned int ByteNum = len;
    unsigned int uw16_i = len;

    while( uw16_i != 0 )   // 取数，取到相同退出。
    {
        // EX0_DISABLE;

        *pCmd = gl_Rx_buf[ChanNo][gl_Rx_first[ChanNo]];  //  转存到其它缓冲区 gR_CmdBuf[MAX_UART_BUF_SIZE]

        gl_Rx_first[ChanNo] = ( gl_Rx_first[ChanNo] + 1 )%MAX_UART_BUF_SIZE;    //  取数指针更改

        pCmd++;    //  转存到其它缓冲区 gR_CmdBuf[MAX_UART_BUF_SIZE]更改

        uw16_i --;
    }
    return true;  // 如果在MAX_RESPONSE_CMD_LEN 与MIN_RESPONSE_CMD_LEN之间也属正常取数。
}
#endif

