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
** Descriptions:        ��дʾ������
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "../../COMMON/includes.h"

//#define FPCLK         72000000

	unsigned char UARTFunc::gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // ��ʵ���ջ�����
	unsigned int  UARTFunc::gl_Rx_first[MAX_UART_NUM];//  ȡ��ָ��
	unsigned int  UARTFunc::gl_Rx_last[MAX_UART_NUM]; //  ����ָ��

//void uart_putcmd (unsigned char ChanNo, char *pCmd)
void UARTFunc::uart_putcmd (unsigned char ChanNo, unsigned char *pCmd,unsigned short uw16_Length)
{
	 switch(ChanNo)
	 {
		 case 0:
			 UART0_SendBuffer(pCmd,uw16_Length);
		   break;
		 case 1:
			 UART1_SendBuffer(pCmd,uw16_Length);
		   break;
		 case 2:
			 UART2_SendBuffer(pCmd,uw16_Length);
		   break;
		 case 3:
			 UART3_SendBuffer(pCmd,uw16_Length);
		   break;
		 default:
			 break;
   }
}

/****************************************************************************
 ** GET UART COMMAND WHICH IS TRANSMITTED FROM TEMPERATURTE-CHAMBER       ***
 ** BASED ON MODBUS RTU PROTOCOL, THE RECEIVED COMMAND LOOKS LIKE         ***
 **     SEND: 01 03 00 64 00 01 C5 D5                                     ***
 ** RECEIVED: 01 03 02 01 2C B8 09 (TEMPERATURE = 30)                     ***
 ** MESSAGE : 0X012C = 30                                                 ***
 ****************************************************************************/
bool UARTFunc::uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len)
{
//  �Դ��ڻ��������ݽ��н�����֤���Ⱥ�����
	unsigned int ByteNum = 0;          

	while( gl_Rx_first[ChanNo] != gl_Rx_last[ChanNo] )   // ȡ����ȡ����ͬ�˳���
	{  
		*pCmd = gl_Rx_buf[ChanNo][gl_Rx_first[ChanNo]];  //  ת�浽���������� gR_CmdBuf[MAX_UART_BUF_SIZE]
		gl_Rx_first[ChanNo] = ( gl_Rx_first[ChanNo] + 1 )%MAX_UART_BUF_SIZE;    //  ȡ��ָ�����
		if( ByteNum >= MAX_COM_READ_REPLY)  // 05/23/2011 yangying 
   	 {
			if( ByteNum == MAX_COM_READ_REPLY || ByteNum == MAX_COM_WRITE_REPLY)  // 05/23/2011 yangying 
			   { *pCmd=0;           return true;   }
      else 
		   	 { *pCmd=0;           return false;  }
   	 }
   	pCmd++;    //  ת�浽���������� gR_CmdBuf[MAX_UART_BUF_SIZE]����
	 	if( (ByteNum++) > len )
		{  *pCmd=0;               return false;  }                             
    if( gl_Rx_first[ChanNo] == gl_Rx_last[ChanNo] )        DELAY2(500);
 	} 
	*pCmd=0;
	return false;
}

UWord32 UARTFunc::UARTInit( UWord32 uw32_PortNum, UWord32 uw32_BaudRate )
{
	switch(uw32_PortNum)
	{
		case 0: 
			UART0_Init( uw32_BaudRate );
			break;
		case 1: 
			UART1_Init( uw32_BaudRate );
			break;
		case 2:
			UART2_Init( uw32_BaudRate );
			break;
		case 3: 
			UART3_Init( uw32_BaudRate );
			break;
		default: 
			return 0;
	}
	return 1;
}


void UART0Handler (void) __irq 
{
    UARTFunc::UARTXHandler_Processing(0);
	VICVectAddr = 0;		// Acknowledge Interrupt //
}


/*********************************************************************************************************
** Function name:       UART0_Init
** Descriptions:        ��Ĭ��ֵ��ʼ������0�����ź�ͨѶ����������Ϊ8λ����λ��1λֹͣλ������żУ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UARTFunc::UART0_Init (UWord32 uw32_BaudRate)
{
	 UWord16 usFdiv;
     UWord32 PeripheralClock;
	 PeripheralClock = Fpclk;
	 
	str_Reg_Uart * str_pUart;

	str_pUart = (str_Reg_Uart*)UART0_BASE_ADDR; 
    PINSEL0 |= 0x00000050;       // TXD1(P0.2)  RXD1(P0.3) // 4���ڰ�UART0  TXD1(P0.2) RXD1(P0.3)
			

  
  	str_pUart->UXLCR  = 0x83;                          //�������ò�����              
    usFdiv = (PeripheralClock/ 16) / uw32_BaudRate;  //(FPCLK/ 16)/uw32_BaudRate  // ���ò�����                  
    str_pUart->UXDLM_IER  = usFdiv / 256;
    str_pUart->UXRBR_THR_DLL  = usFdiv % 256; 
    str_pUart->UXLCR  = 0x03;                      // ����������                
    str_pUart->UXIIR_FCR  = 0x06; 				
	
	IRQFunc::install_irq( UART0_INT, (void *) UART0Handler,HIGHEST_PRIORITY +11 );
	str_pUart->UXDLM_IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART0 interrupt //
}

/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        �Ӵ���0��������
** input parameters:    data: ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UARTFunc::UART0_SendByte (int ucData)
{
	while (!(U0LSR & 0x20));
    return (U0THR = ucData);
}

/*********************************************************************************************************
Write character to Serial Port
** Function name:       UART0_SendString
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    s:   Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UARTFunc::UART0_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length) 
{
	unsigned char i = 0;
	
	for (i=0; i<uw16_Length; i++)
	{
	  UART0_SendByte(*pCmd++);
	}
}

void UART1Handler (void) __irq 
{
    UARTFunc::UARTXHandler_Processing(1);
	VICVectAddr = 0;		// Acknowledge Interrupt //
}

void UARTFunc::UART1_Init (UWord32 uw32_BaudRate)
{
	UWord16 usFdiv;
	str_Reg_Uart * str_pUart;
    UWord32 PeripheralClock;
	PeripheralClock = Fpclk;
	 
	str_pUart = (str_Reg_Uart*)UART1_BASE_ADDR; 
	PINSEL4 |= 0x0000000A;	      // TXD2(P2.0) RXD2(P2.1)  // 4���ڰ�UART1  TXD2(P2.0) RXD2(P2.1)
			

  
  	str_pUart->UXLCR  = 0x83;                          //�������ò�����              
    usFdiv = (PeripheralClock/ 16) / uw32_BaudRate;  //(FPCLK/ 16)/uw32_BaudRate  // ���ò�����                  
    str_pUart->UXDLM_IER  = usFdiv / 256;
    str_pUart->UXRBR_THR_DLL  = usFdiv % 256; 
    str_pUart->UXLCR  = 0x03;                      // ����������                
    str_pUart->UXIIR_FCR  = 0x06; 				
	
	IRQFunc::install_irq( UART1_INT, (void *) UART1Handler,HIGHEST_PRIORITY +12 );
	str_pUart->UXDLM_IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART1 interrupt //
}

/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        �Ӵ���0��������
** input parameters:    data: ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UARTFunc::UART1_SendByte (int ucData)
{
	while (!(U1LSR & 0x20));
    return (U1THR = ucData);
}
/*********************************************************************************************************
Write character to Serial Port
** Function name:       UART0_SendString
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    s:   Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UARTFunc::UART1_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length) 
{
	unsigned char i = 0;
	
	for (i=0; i<uw16_Length; i++)
	{
	  UART1_SendByte(*pCmd++);
	}
	return;
}


void UART2Handler (void) __irq 
{	
   UARTFunc::UARTXHandler_Processing(2);
   VICVectAddr = 0;		// Acknowledge Interrupt //
}

void UARTFunc::UART2_Init (UWord32 uw32_BaudRate)
{
    UWord16 usFdiv;
	
	str_Reg_Uart * str_pUart;
     UWord32 PeripheralClock;
	 PeripheralClock = Fpclk;
	 
	PCONP |= (1<<24);
	str_pUart = (str_Reg_Uart*)UART2_BASE_ADDR;
	PINSEL0 |= 0x00500000;	      // TXD3(P0.10) RXD3(P0.11) // 4���ڰ�UART2  TXD3(P0.10) RXD3(P0.11)
			

  
  	str_pUart->UXLCR  = 0x83;                          //�������ò�����              
    usFdiv = (PeripheralClock/ 16) / uw32_BaudRate;  //(FPCLK/ 16)/uw32_BaudRate  // ���ò�����                  
    str_pUart->UXDLM_IER  = usFdiv / 256;
    str_pUart->UXRBR_THR_DLL  = usFdiv % 256; 
    str_pUart->UXLCR  = 0x03;                      // ����������                
    str_pUart->UXIIR_FCR  = 0x06; 				
	
	IRQFunc::install_irq( UART2_INT, (void *) UART2Handler,HIGHEST_PRIORITY +13 );
	str_pUart->UXDLM_IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART1 interrupt //
}


/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        �Ӵ���0��������
** input parameters:    data: ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UARTFunc::UART2_SendByte (int ucData)
{
	while (!(U2LSR & 0x20));
    return (U2THR = ucData);
}

/*********************************************************************************************************
Write character to Serial Port
** Function name:       UART0_SendString
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    s:   Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UARTFunc::UART2_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length) 
{
	unsigned char i = 0;
	
	for (i=0; i<uw16_Length; i++)
	{
	  UART2_SendByte(*pCmd++);
	}
	return;
}


void UART3Handler (void) __irq 
{
    UARTFunc::UARTXHandler_Processing(3);
	VICVectAddr = 0;		// Acknowledge Interrupt //
}


void UARTFunc::UART3_Init (UWord32 uw32_BaudRate)
{
    UWord16 usFdiv;
	str_Reg_Uart * str_pUart;
     UWord32 PeripheralClock;
	 PeripheralClock = Fpclk;
	 
	PCONP |= (1<<25);
	str_pUart = (str_Reg_Uart*)UART3_BASE_ADDR;
	PINSEL9 |= 0x0F000000;	      // TXD4(P4.28) RXD4(P4.29) // 4���ڰ�UART3  TXD4(P4.28) RXD4(P4.29)
			 
  	str_pUart->UXLCR  = 0x83;                          //�������ò�����              
    usFdiv = (PeripheralClock/ 16) / uw32_BaudRate;  //(FPCLK/ 16)/uw32_BaudRate  // ���ò�����                  
    str_pUart->UXDLM_IER  = usFdiv / 256;
    str_pUart->UXRBR_THR_DLL  = usFdiv % 256; 
    str_pUart->UXLCR  = 0x03;                      // ����������                
    str_pUart->UXIIR_FCR  = 0x06; 				
	
	IRQFunc::install_irq( UART3_INT, (void *) UART3Handler,HIGHEST_PRIORITY +14);
	str_pUart->UXDLM_IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART1 interrupt //
}


/*********************************************************************************************************
** Function name:       UART0_SendByte
** Descriptions:        �Ӵ���0��������
** input parameters:    data: ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
int UARTFunc::UART3_SendByte (int ucData)
{
	while (!(U3LSR & 0x20));
    return (U3THR = ucData);
}
/*********************************************************************************************************
Write character to Serial Port
** Function name:       UART0_SendString
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    s:   Ҫ���͵��ַ���ָ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UARTFunc::UART3_SendBuffer(unsigned char *pCmd,unsigned short uw16_Length) 
{
	unsigned char i = 0;
	
	for (i=0; i<uw16_Length; i++)
	{
	  UART3_SendByte(*pCmd++);
	}
	return;
}


void UARTFunc::UARTXHandler_Processing (unsigned char uc_UartCh)
{
    UChar8 uc_IIRValue = 0, uc_LSRValue = 0;
    UChar8 uc_Dummy = uc_Dummy;
    str_Reg_Uart * str_pUart;
	
	  switch(uc_UartCh)
	  {
       case 0:
			str_pUart = (str_Reg_Uart*)UART0_BASE_ADDR; 
			break;
		case 1: 
			str_pUart = (str_Reg_Uart*)UART1_BASE_ADDR; 
			break;
		case 2: 
			str_pUart = (str_Reg_Uart*)UART2_BASE_ADDR; 
			break;
		case 3: 
			str_pUart = (str_Reg_Uart*)UART3_BASE_ADDR; 
			break;
		default: 
			return;
			//break;
	}

    uc_IIRValue = str_pUart->UXIIR_FCR;
    
    uc_IIRValue >>= 1;			// skip pending bit in IIR //
    uc_IIRValue &= 0x07;			// check bit 1~3, interrupt identification //
	
    if ( uc_IIRValue == IIR_RLS )		// Receive Line Status //
    {
		uc_LSRValue = str_pUart->UXLSR;
		// Receive Line Status //
		if ( uc_LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
		    	// There are errors or break interrupt //
		    	// Read LSR will clear the interrupt //
		    	
		    	//uw32_UART0Status = uc_LSRValue;
		    	
		    	uc_Dummy = str_pUart->UXRBR_THR_DLL;		//Dummy read on RX to clear 

				return;
		}
		if ( uc_LSRValue & LSR_RDR )	// Receive Data Ready //			
		{
		    	// If no error on RLS, normal ready, save into the data buffer. //
		    	// Note: read RBR will clear the interrupt //
					
				uc_Dummy = str_pUart->UXRBR_THR_DLL;

                if (((gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE) != gl_Rx_first[uc_UartCh])
	            {
 	 	            gl_Rx_buf[uc_UartCh][gl_Rx_last[uc_UartCh]] = uc_Dummy; 
		            gl_Rx_last[uc_UartCh]=(gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE;
                    str_pUart->UXDLM_IER = IER_RBR|IER_THRE|IER_RLS;  //Enable UART0 Interrupt
		        }
		   }
     }
    else if ( uc_IIRValue == IIR_RDA )	// Receive Data Available //
    {
		// Receive Data Available //

		uc_Dummy = str_pUart->UXRBR_THR_DLL;

        if (((gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE) != gl_Rx_first[uc_UartCh])
	    {
 	 	            gl_Rx_buf[uc_UartCh][gl_Rx_last[uc_UartCh]] = uc_Dummy; 
		            gl_Rx_last[uc_UartCh]=(gl_Rx_last[uc_UartCh]+1)%MAX_UART_BUF_SIZE;
                    str_pUart->UXDLM_IER = IER_RBR|IER_THRE|IER_RLS;  //Enable UART0 Interrupt
	    }
		
        /*if((str_Uart[uc_UartCh].gl_Rx_last + 1)%MAX_UART_BUF_SIZE != str_Uart[uc_UartCh].gl_Rx_first)
        {
            str_Uart[uc_UartCh].gl_Rx_buf[str_Uart[uc_UartCh].gl_Rx_last] = uc_Dummy;
            str_Uart[uc_UartCh].gl_Rx_last = (str_Uart[uc_UartCh].gl_Rx_last + 1)%MAX_UART_BUF_SIZE; // ÿ��һ������βָ�������һλ����ָ������ݴ���
            str_pUart->UXDLM_IER = IER_RBR|IER_THRE|IER_RLS;  //Enable UART0 Interrupt
        }*/		
    }
    else if ( uc_IIRValue == IIR_CTI )	// Character timeout indicator //
    {
		/// Character Time-out indicator //
		//uw32_UART0Status |= 0x100;		// Bit 9 as the CTI error //
		
		uc_Dummy = IIR_CTI;
    }
    else if ( uc_IIRValue == IIR_THRE )	// THRE, transmit holding register empty //
    {
		// THRE interrupt //
		uc_LSRValue = str_pUart->UXLSR;		// Check status in the LSR to see if
								// valid data in U0THR or not //
		/*if ( uc_LSRValue & LSR_THRE )
		{
    	       uc_UART0TxEmpty = 1;
		}
		else
		{
    	       uc_UART0TxEmpty = 0;
		}*/
    }
	
}






