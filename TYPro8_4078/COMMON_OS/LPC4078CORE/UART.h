/****************************************Copyright (c)****************************************************
**                                 http://www.PowerAVR.com
**								   http://www.PowerMCU.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.h
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
#ifndef __UART_H
#define __UART_H


/*
//----UART----------------------------------------------------------------------------
#define MAX_UART_NUM           4
#define UART_BUFSIZE          (768) //(512)
#define MAX_UART_TOTALNUM	   (MAX_UART_NUM)       // 使用串口UART时，最大的串口总数量
#define MAX_UART_BUF_SIZE	   (UART_BUFSIZE)       // 使用串口UART时，各个串口的最大缓冲区
//----UART----------------------------------------------------------------------------
*/


//   <>UART MAX COM NUMBER <1=> 1 <2=> 2 <3=> 3 <4=> 4
//   <> UART COM 1/2/3/4
//   <> Default:    1
#define MAX_UART_NUM              4  // 1   串口板最大支持个数  // 使用串口UART时，最大的串口总数量

#define UART_BUFSIZE              (768) //(512)  // 使用串口UART时，各个串口的最大缓冲区
#define MAX_UART_TOTALNUM	      (MAX_UART_NUM)       // 使用串口UART时，最大的串口总数量
#define MAX_UART_BUF_SIZE	      (UART_BUFSIZE)




// Interrupt Enable Register
#define IER_RBR		0x01  // enable the Receive Data Aviable Interrupt 
#define IER_THRE	0x02  // Enable THRE interrupt for URATn
#define IER_RLS		0x04  // Enable the RX line status interrupts
// Interrupt Identification Register
#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define TER_TXEN_OFF	    0x00
#define TER_TXEN_ON	        0x80

// UART FIFO
#define FCR_FIFO_ENABLE		    0x01
#define FCR_RXFIFO_RESET	    0x02
#define IER_TXFIFO_RESET		0x04
#define FCR_RXFIFO_1BYTE	    0x00
#define FCR_RXFIFO_4BYTE	    0x40
#define FCR_RXFIFO_8BYTE	    0x80
#define FCR_RXFIFO_14BYTE	    0xC0



#define DELAY2(k)      	{unsigned int i; for(i=0; i<k; i++);}


#define UART_M4TOM0_CH	    (0X01)   //  在M4中使用UART1作为与M0通道接口



typedef struct
{
    unsigned long UXRBR_THR_DLL;  // 三合一
    //unsigned long UXTHR;
    //unsigned long UXDLL;
    unsigned long UXDLM_IER;  // 二合一
    //unsigned long UXIER;
    unsigned long UXIIR_FCR;  // 二合一
    //unsigned long UXFCR;
    unsigned long UXLCR;
    unsigned long UXMCR;     // U1串口调制解调用
    unsigned long UXLSR;
    unsigned long UXMSR;     // U1串口调制解调用
    unsigned long UXSCR;
    unsigned long UXACR;
    unsigned long UXICR;    // U0/2/3/4串口用
    unsigned long UXFDR;
    unsigned long UXEmpty;  // 预留
    unsigned long UXTER;
} str_Reg_Uart;


class UARTFunc
{
public:
    static unsigned char gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实接收缓冲区
    static unsigned int  gl_Rx_first[MAX_UART_NUM];//  取数指针
    static unsigned int  gl_Rx_last[MAX_UART_NUM]; //  存数指针
    static unsigned long int  gl_Rx_Error[MAX_UART_NUM];//  存数错误计数器

    static  unsigned char gl_Tx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE];// 真实发送缓冲区
    static  unsigned int  gl_Tx_first[MAX_UART_NUM];//  取数指针
    static  unsigned int  gl_Tx_last[MAX_UART_NUM];//  存数指针

public:
    static void uart_putcmd (unsigned char ChanNo, unsigned char *pCmd,unsigned short uw16_Length);
    static void UARTXHandler_Processing (unsigned char uc_UartCh);
    static UWord32 UARTInit( UWord32 uw32_PortNum, unsigned char uc_BaudRate, unsigned char uc_DataBits, unsigned char uc_Parity, unsigned char uc_StopBits );
#if  USE_AS_AUXMCU == 1
    static unsigned int uart_getcmd (unsigned char ChanNo,unsigned char *pCmd);
#endif


public:
    static void UARTSend(unsigned char uc_UartCh, unsigned int Length );
    static unsigned int uart_bytenum(unsigned char ChanNo);

// 	static void UARTSendFIFO(unsigned char uc_UartCh);
// 	static unsigned short int  UARTCheck_SendByte(unsigned char uc_UartCh);
#if(USE_AS_AUXMCU ==1)
    static void UARTSendFIFO(unsigned char uc_UartCh);
    static void uart_empty(unsigned char ChanNo);
    static bool uart_getcmd_AppointLong (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
#endif
};
#endif


