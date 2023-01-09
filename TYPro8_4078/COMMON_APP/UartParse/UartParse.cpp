#include "includes.h"

UInt16 UartParse::Priority = PRIORITY4;
UInt16 UartParse::TaskID = UART_PARSE_TASK;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
//temp
St_QueueBuf  	  UartParse::m_UartRxQueue;
St_RESENDBuf      UartParse::ReSendUartMsg; //重发
MsTime            AckTimeOut;

#pragma arm section //

void UartParse::MyInit(void)
{
    m_UartRxQueue.m_front = 0;
    m_UartRxQueue.m_rear = 0;
    m_UartRxQueue.m_length = 0;
    memset(&ReSendUartMsg, 0, sizeof(St_RESENDBuf));
    CommonFunc::GetTimeTicksUpdate(&AckTimeOut);
    for(int i = 0; i < MAXADCHANNELNO; i++)
    {
        M0RedundantFunc::m_M0_CalibPara[i].m_Gain = 0.0f;
        M0RedundantFunc::m_M0_CalibPara[i].m_Offset = 0.0f;
    }
}

void UartParse::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_UART_M0] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_UART_M0);
#endif
    if( UARTFunc::uart_bytenum(UART_M4TOM0_CH) >= MAX_UART_RX_LENGTH)
    {
        if( m_UartRxQueue.m_length < MAX_BUF_LENGTH) //CirQueue full
        {
            while(UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][UARTFunc::gl_Rx_first[UART_M4TOM0_CH]] != UART_CMD_START)//180821 zyx
            {
                UARTFunc::gl_Rx_first[UART_M4TOM0_CH] = ( UARTFunc::gl_Rx_first[UART_M4TOM0_CH] + 1 )%MAX_UART_BUF_SIZE;
                if(UARTFunc::gl_Rx_first[UART_M4TOM0_CH] == UARTFunc::gl_Rx_last[UART_M4TOM0_CH])
								{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_UART_M0] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_UART_M0);
#endif
									
                    return;
								}
            }
            memcpy((void*)&m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf,(void*)&UARTFunc::gl_Rx_buf[UART_M4TOM0_CH][UARTFunc::gl_Rx_first[UART_M4TOM0_CH]],MAX_UART_RX_LENGTH);   //  转存到其它缓冲区
            UARTFunc::gl_Rx_first[UART_M4TOM0_CH] = ( UARTFunc::gl_Rx_first[UART_M4TOM0_CH] + MAX_UART_RX_LENGTH )%MAX_UART_BUF_SIZE;    //  取数指针更改
//             if(m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf[UART_CMD_LENGTH_POSITION] > MAX_DATA_LENGTH)
//                 m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf[UART_CMD_LENGTH_POSITION] = MAX_DATA_LENGTH;  // 保护最大长度避免发送方长度出错。
// 			m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_Length = m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf[UART_CMD_LENGTH_POSITION];
            m_UartRxQueue.m_rear = (m_UartRxQueue.m_rear+1)%MAX_BUF_LENGTH;
            m_UartRxQueue.m_length++;
        }
    }

    UartParse::Parse();
    if(CommonFunc::GetTimeTicks(&AckTimeOut) > 5000) //>500ms //180821 zyx
    {
        ResendMsg();
        CommonFunc::GetTimeTicksUpdate(&AckTimeOut);
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_UART_M0] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_UART_M0);
#endif
}
void UartParse::Parse(void)
{
    Un_Uart_CMDGroup DeQueueData;
    unsigned char CmdHead,Length = 0;
    //do checksum
    //start mask uart interrupt

    if(m_UartRxQueue.m_length>0 && m_UartRxQueue.m_length<=MAX_BUF_LENGTH) //180821 zyx
    {
        //NVIC_DisableIRQ(DMA_IRQn);
        if(m_UartRxQueue.m_front ==  m_UartRxQueue.m_rear)
            return;
        if(m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[0] != UART_CMD_START)
        {
            m_UartRxQueue.m_length--;
            m_UartRxQueue.m_front = (m_UartRxQueue.m_front+1)%MAX_BUF_LENGTH;
            return;
        }
        Length = m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[UART_CMD_LENGTH_POSITION];
        if(Length > sizeof(Un_Uart_CMDGroup))
        {
            m_UartRxQueue.m_length--;
            m_UartRxQueue.m_front = (m_UartRxQueue.m_front+1)%MAX_BUF_LENGTH;
            return;
        }
        CmdHead = m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[UART_CMD_HEAD_POSITION];
        memcpy((void*)&DeQueueData.m_Uart_ReportUnsafe.m_CmdStart,&m_UartRxQueue.m_Buf[m_UartRxQueue.m_front].m_RxBuf[0],Length);
        m_UartRxQueue.m_length--;
        m_UartRxQueue.m_front = (m_UartRxQueue.m_front+1)%MAX_BUF_LENGTH;
        //NVIC_EnableIRQ(DMA_IRQn);

        //start  uart interrupt

        switch(CmdHead)
        {
            //M0 -> M4
        case CMD_UART_REPORT_UNSAFE:
            Parse_UnsafeInfo(&DeQueueData.m_Uart_ReportUnsafe);
            break;
        case CMD_UART_REPORT_DATA:
            Parse_ReportData(&DeQueueData.m_Uart_GetVoltage);
            break;
        case CMD_UART_REPORT_CalibPara:
            Parse_ReportCalibPara(&DeQueueData.m_Uart_ReportCalibPara);
            break;
        case CMD_ACK:
            AckCheck(&DeQueueData.m_Uart_ReportAck);
        default:
            break;
        }
    }
}


void UartParse::SendCmd(St_UartCmdBuf * Uart_Cmd)
{
    //memset((void*)&Uart_Cmd->m_FillBuf,0,FILL_SIZE);

    //memset(Uart_Cmd->m_FillBuf,0,FILL_SIZE);
    //UARTFunc::UART1_SendBuffer((unsigned char*)&Uart_Cmd->m_UartCmdGroup.m_Uart_ReportUnsafe.m_CheckSum,MAX_UART_RX_LENGTH);

    //UARTFunc::uart_putcmd (1, (unsigned char*)&Uart_Cmd->m_UartCmdGroup.m_Uart_ReportUnsafe.m_CheckSum,MAX_UART_RX_LENGTH);
    UARTFunc::uart_putcmd (1, (unsigned char*)&Uart_Cmd->m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdStart,MAX_UART_RX_LENGTH);
    PushToResendMsg(Uart_Cmd);
}


void UartParse::Parse_UnsafeInfo(const St_Uart_ReportUnsafe * UnsafeInfo)
{
    unsigned char IV_ChnNo = UnsafeInfo->m_ChnNo;
    if(IV_ChnNo>=MAXCHANNELNO)
        return;

    //safety_check::RedundantSafetyFlg[IV_ChnNo] = 1;

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
    {
        M0RedundantFunc::m_fM0Value = UnsafeInfo->m_UnsafeValue;
        safety_check::SetRedundantSafetyFlgCheck(IV_ChnNo,UnsafeInfo->m_UnsafeValue);
    }
//#endif

}
void UartParse::Parse_ReportData(const St_Uart_GetADCVoltage * Data)
{
    unsigned char IV_ChnNo = Data->m_ChnNo;
    M0RedundantFunc::m_SampleData[IV_ChnNo] =Data->m_Voltage;
}
void UartParse::Parse_ReportCalibPara( St_Uart_ReportCalibPara *Para)
{
    unsigned char uc_chn = Para->m_ChnNo;
    M0RedundantFunc::m_M0_CalibPara[uc_chn]= Para->m_st_ADCPara;
}
void UartParse::PushToResendMsg(St_UartCmdBuf * Uart_Cmd) //180821 zyx
{
    if(ReSendUartMsg.m_length < MAX_RESEND_BUF_LENGTH)
    {
        for(int i = 0; i < MAX_RESEND_BUF_LENGTH; i++)
        {
            if(ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_SetSafety.m_CmdStart == 0)
            {
                memcpy(&ReSendUartMsg.m_Buf[i], Uart_Cmd, sizeof(St_UartCmdBuf));
                ReSendUartMsg.m_length++;
                return;
            }
        }
    }
    return;
}
void UartParse::DelResendMsg( int index )
{
    memset(&ReSendUartMsg.m_Buf[index], 0, sizeof(St_UartCmdBuf));
    ReSendUartMsg.m_length--;
}
void UartParse::ResendMsg()
{
    if(ReSendUartMsg.m_length <= 0)
        return;
    for(int i = 0; i < MAX_RESEND_BUF_LENGTH; i++)
    {
        if(ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_SetSafety.m_CmdStart == UART_CMD_START)
        {
            UARTFunc::uart_putcmd (1, (unsigned char*)&ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_ReportUnsafe.m_CmdStart,MAX_UART_RX_LENGTH);
            ReSendUartMsg.m_Buf[i].UnAckCounter++;
            if(ReSendUartMsg.m_Buf[i].UnAckCounter > 10)
                DelResendMsg( i );
        }
    }
}
void UartParse::AckCheck(St_Uart_ReportAck *Data)
{
    for(int i = 0; i < MAX_RESEND_BUF_LENGTH; i++)
    {
        if(ReSendUartMsg.m_length <= 0)
            return;
        if(ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_SetSafety.m_CmdStart == UART_CMD_START)
        {
            if(Data->m_FeedBackCmd == ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_SetSafety.m_CmdHead
                    && Data->m_ChnNo == ReSendUartMsg.m_Buf[i].m_UartCmdGroup.m_Uart_SetSafety.m_IVChnNo)
            {
                DelResendMsg( i );
            }
        }
    }
}
