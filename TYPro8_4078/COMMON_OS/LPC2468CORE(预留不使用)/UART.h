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



//----UART----------------------------------------------------------------------------
#define MAX_UART_NUM           4
#define UART_BUFSIZE          (768) //(512)
//#define MAX_UART_TOTALNUM	   (4)       // 使用串口UART时，最大的串口总数量
//#define MAX_UART_BUF_SIZE	   (512)     // 使用串口UART时，各个串口的最大缓冲区
#define MAX_UART_TOTALNUM	   (MAX_UART_NUM)       // 使用串口UART时，最大的串口总数量
#define MAX_UART_BUF_SIZE	   (UART_BUFSIZE)
//----UART----------------------------------------------------------------------------




#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

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

// 发送长度和接收长度定义：
#define MAX_COM_WRITE_CMD	     (2+6)    // CMD(6 byte)+CRC(2 byte)         //  09/14/2011  yangying
#define MAX_COM_WRITE_REPLY	     (2+6)    // REPLY(116byte)+CRC(2 byte)     //  09/14/2011  yangying
#define MAX_COM_READ_CMD	     (2+6)    // CMD(3 byte)+CRC(2 byte)          //  09/14/2011  yangying
#define MAX_COM_READ_REPLY	     (2+5)    // REPLY(116byte)+CRC(2 byte)     //  09/14/2011  yangying

#define MAX_COM_STOP_CMD	     (2+6)    // CMD(6 byte)+CRC(2 byte)      //  09/14/2011  yangying  
#define MAX_COM_RUN_CMD	         (2+6)    // CMD(6 byte)+CRC(2 byte)       //  09/14/2011  yangying 

#define WRITE_CMD_POINT_VAlUE	 (4)    // Write Value point (4)        //  09/14/2011  yangying
#define READ_REPLY_POINT_VAlUE	 (3)    // Read REPLY Value point (3)    //  09/14/2011  yangying

#define DELAY2(k)      	{unsigned int i; for(i=0; i<k; i++);}


typedef struct 
{
	unsigned long UXRBR_THR_DLL;
	//unsigned long UXTHR;
    //unsigned long UXDLL;
    unsigned long UXDLM_IER;
    //unsigned long UXIER;
    unsigned long UXIIR_FCR;
	//unsigned long UXFCR;
	unsigned long UXLCR;
	unsigned long UXMCR;
	unsigned long UXLSR;
	unsigned long UXMSR;
	unsigned long UXSCR;
	unsigned long UXACR;
    unsigned long UXICR;
    unsigned long UXFDR;
	unsigned long UXEmpty;
    unsigned long UXTER;
}str_Reg_Uart;


class UARTFunc
{
public:
	static unsigned char gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实接收缓冲区
	static unsigned int  gl_Rx_first[MAX_UART_NUM];//  取数指针
	static unsigned int  gl_Rx_last[MAX_UART_NUM]; //  存数指针	

public:	
 static void uart_putcmd (unsigned char ChanNo, unsigned char *pCmd,unsigned short uw16_Length);
 static bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
 static UWord32 UARTInit( UWord32 uw32_PortNum, UWord32 uw32_BaudRate );
 static void UARTXHandler_Processing (unsigned char uc_UartCh);

private:
 static void UART0_Init (UWord32 uw32_BaudRate);
 static int  UART0_SendByte (int ucData);
 static void UART0_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length);
 static void UART1_Init (UWord32 uw32_BaudRate);
 static int  UART1_SendByte (int ucData);
 static void UART1_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length);
 static void UART2_Init (UWord32 uw32_BaudRate);
 static int  UART2_SendByte (int ucData);
 static void UART2_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length);
 static void UART3_Init (UWord32 uw32_BaudRate);
 static int  UART3_SendByte (int ucData);
 static void UART3_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length);
};
#endif


