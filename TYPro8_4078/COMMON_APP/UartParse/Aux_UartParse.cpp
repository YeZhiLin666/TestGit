#include "../Entry/includes.h"



UInt16 AuxUartParse::Priority = PRIORITY4;
UInt16 AuxUartParse::TaskID = AUX_UARTPARSE_TASK;


St_QueueBuf  	  AuxUartParse::m_UartRxQueue;


void AuxUartParse::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_UART_PARSE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_UART_PARSE);
#endif

    if( UARTFunc::uart_bytenum(UART_M4TOM0_CH) >= MAX_UART_RX_LENGTH)
    {
        if( m_UartRxQueue.m_length < MAX_BUF_LENGTH) //CirQueue full
        {
            memcpy((void*)&m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf,(void*)&UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][UARTFunc::gl_Rx_first[UART_M4TOM0_CH]],MAX_UART_RX_LENGTH);   //  转存到其它缓冲区
            UARTFunc::gl_Rx_first[UART_M4TOM0_CH] = ( UARTFunc::gl_Rx_first[UART_M4TOM0_CH] + MAX_UART_RX_LENGTH )%MAX_UART_BUF_SIZE;    //  取数指针更改
            m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_Length = m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf[UART_CMD_LENGTH_POSITION];
            m_UartRxQueue.m_rear = (m_UartRxQueue.m_rear+1)%MAX_BUF_LENGTH;
            m_UartRxQueue.m_length++;
        }
    }
    DoUartParse();

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_UART_PARSE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_UART_PARSE);
#endif
}
void AuxUartParse::Init(void)
{
    // add code
    m_UartRxQueue.m_front = 0;
    m_UartRxQueue.m_rear = 0;
    m_UartRxQueue.m_length = 0;
}

void AuxUartParse::DoUartParse(void)
{
    Un_Uart_CMDGroup DeQueueData;
    unsigned char CmdHead,Length = 0;
    //do checksum
    //start mask uart interrupt

    if(m_UartRxQueue.m_length>0 && m_UartRxQueue.m_length<MAX_BUF_LENGTH)
    {
        //NVIC_DisableIRQ(DMA_IRQn);
        if(m_UartRxQueue.m_front ==  m_UartRxQueue.m_rear)
            return;

        Length = m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[UART_CMD_LENGTH_POSITION];
        CmdHead = m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[UART_CMD_HEAD_POSITION];
        memcpy((void*)&DeQueueData.m_Uart_ReportUnsafe.m_CmdStart,&m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[0],Length);
        m_UartRxQueue.m_length--;
        m_UartRxQueue.m_front = (m_UartRxQueue.m_front+1)%MAX_BUF_LENGTH;
        switch(CmdHead)
        {
        case CMD_UART_SET_SAFETY:
            Aux_Safety_Check::SetSafetyInfo(& DeQueueData.m_Uart_SetSafety);
            break;

        case CMD_UART_SET_GAIN_OFFSET:
            RedundantSample_Task::SetGainOffset(& DeQueueData.m_Uart_SetGainOffset);
            break;
        case CMD_UART_SET_MAX_CHN_NO:
            RedundantSample_Task::SetChannelMap(& DeQueueData.m_Uart_SetChannelMap);
            break;

        case CMD_UART_REPORT_UNSAFE:
            break;
        default:
            break;
        }
    }
}

/*
void AuxUartParse::ParseOneCmd(unsigned char idx)
{


}
*/

void AuxUartParse::Send_UartCmd(St_UartCmdBuf * Uart_Cmd)
{
    UARTFunc::uart_putcmd (1, (unsigned char*)&Uart_Cmd->m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdStart,MAX_UART_RX_LENGTH);
}



unsigned char * AuxUartParse::UartChecksumPro( unsigned short* DataLen ,unsigned char* pDataBuf )
{
    unsigned char* sendbuf;
    unsigned short TotalLen = *DataLen;
    int a;
    unsigned short ResChecksum ;
    unsigned short Checksum = AuxUartParse::MyCheckSum((unsigned short*)pDataBuf,TotalLen);
    TotalLen +=2;    //add 2 bytes for checksum
    // sendbuf = udp_get_buf (TotalLen);
    sendbuf[0] = Checksum&0xff;//checksum ???
    sendbuf[1] = (Checksum>>8)&0xff;
    memcpy(&sendbuf[2],pDataBuf,TotalLen-2);
    ResChecksum = AuxUartParse::MyCheckSum((unsigned short*)sendbuf,TotalLen);	//0x0000 0xffff
    if(ResChecksum!=0x0000)
        a= 0;
    *DataLen = TotalLen;  //????
    return sendbuf;
}


unsigned short AuxUartParse::MyCheckSum(unsigned short *buffer, unsigned short size)
{
    unsigned long cksum=0;
    unsigned short LastNum;
    //??
    while(size>1)
    {
        cksum+=*buffer++;
        size-=sizeof(unsigned short);

    }
    //????
    if(size)
    {
        LastNum = (*buffer);
        //cksum+=(LastNum);
        cksum+=*(unsigned char *)buffer;

    }
    //????
    //cksum=(cksum>>16)+(cksum&0xffff);
    //cksum+=(cksum>>16);
    while (cksum>>16)
        cksum=(cksum>>16)+(cksum & 0xffff);
    return (unsigned short)(~cksum);
}
