/*********************************************************************************************************
** Function name:     	DMA Module
** Descriptions:			LPC4078 ARM  DMA.cpp
**  Mainly used for     UART0 DMA Receive
*********************************************************************************************************/

#include "includes.h"

unsigned char    DMAFunc::u8_RxBuffer[DMA_BUFSIZE];
#pragma arm section rwdata = "SRAM" zidata = "SRAM"

unsigned int     DMAFunc::DMA_Err_Count[MAX_DMA_CHN];
St_QueueBuf  	  DMAFunc::m_UartRxQueue;

#pragma arm section

//can not modify DMA_Ptr
LPC_GPDMACH_TypeDef * const DMAFunc::st_LPC_GPDMA_Ch[MAX_DMA_CHN] =
{
    LPC_GPDMACH0,LPC_GPDMACH1,
    LPC_GPDMACH2,LPC_GPDMACH3,
    LPC_GPDMACH4,LPC_GPDMACH5,
    LPC_GPDMACH6,LPC_GPDMACH7
};

//NVIC Interrupt Service funciton
void DMA_IRQHandler(void)
{
    if( LPC_GPDMA->IntStat == 0)
        return;
    unsigned char IntTCStatReg = LPC_GPDMA->IntTCStat;
    unsigned char IntErrReg = LPC_GPDMA->IntErrStat;
    unsigned char activeChn = 0;
    for(unsigned char Chn = 0; Chn<MAX_DMA_CHN; Chn++) //find valid active DMA Chn
    {
        activeChn = (IntTCStatReg &(Chn+1))>>Chn;
        if(activeChn)
        {
            DMAFunc::DMAISR(Chn);	  //TEMP just receive UnsafeInfo
        }

        if((IntErrReg &(Chn+1))>>Chn)
        {
            DMAFunc::DMA_Err_Count[Chn]++;
            LPC_GPDMA->IntErrClr |= (Chn+1);
        }
    }
}


/*********************************************************************************************************
** Function name:     	Init
** Descriptions:			LPC4078 ARM UART DMA Init
** input parameters:    void chn 0
** output parameters:   void
** Returned value:      void
*********************************************************************************************************/
void DMAFunc::SetupUart0()
{
    LPC_SC->PCONP |= (1<<29);				 //DMA Power On
    LPC_GPDMA->IntTCClear |= 0xff;
    LPC_GPDMA->IntErrClr |= 0xff;

    //LPC_SC->DMAREQSEL &= ~(1 << 10); //< Assure that DMA request is connected to UART0 RX
    LPC_SC->DMAREQSEL &= ~(1 << 13); //< Assure that DMA request is connected to UART0 RX
    LPC_GPDMACH0->CLLI = 0;
    LPC_GPDMACH0->CSrcAddr  = (uint32)UART1RCVFIFO;	       //UART 1 Base Adr don't care?
    LPC_GPDMACH0->CDestAddr = (uint32)&u8_RxBuffer[0];	          //Rx Buffer Adr

    LPC_GPDMACH0->CControl |= MAX_UART_RX_LENGTH
                              |  (0x0 << 12)		                 		 // Src Burst Size = 8
                              |  (0x5 << 15)                            // Dest Burst Size = 32
                              |  (0x00 << 18)                            // Src Width = 8
                              |  (0x00 << 21)                   			 // Dest Width = 8
                              |  (0x00 << 26)		         				 //Scr Adr Increase
                              |  (0x01 << 27)		                      //Dest Adr Increase
                              |  (0x80000000);									 //Enable DMA0 Interrupt Count


    LPC_GPDMACH0->CConfig |= (0x02 << 11)	           	 //TransferType  Peripheral to Mem
                             |  (13 << 1)                  //Src Peripheral
                             | (0x00 << 6)                 //Dest Peripheral  don't care
                             | (0x01<<15);
    IRQFunc::install_irq(DMA_IRQn,HIGHEST_PRIORITY +12 );
    //NVIC_EnableIRQ (DMA_IRQn);
    //NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));
    LPC_GPDMACH0->CConfig |= 0x01;   						 //Enale Chn 0
    LPC_GPDMA->Config = 0x01;	                        //Enable DMA Little Endian(Default)

    //app initial
    memset(DMAFunc::DMA_Err_Count,0,8);
}

/*********************************************************************************
** Function name:	    DMAISR
** Descriptions:	    UART DMA Interrupt service funciton
** input parameters:    DMA_Chn
** output parameters:   none
** Returned value:      void
**********************************************************************************/
void DMAFunc::DMAISR(unsigned char DMA_Chn)
{
    LPC_GPDMA->IntTCClear |= (DMA_Chn+1);

    //clear Interrupt flag
    st_LPC_GPDMA_Ch[DMA_Chn]->CDestAddr = (uint32)&u8_RxBuffer[0];
    st_LPC_GPDMA_Ch[DMA_Chn]->CControl = st_LPC_GPDMA_Ch[DMA_Chn]->CControl | MAX_UART_RX_LENGTH;  //next time transfer size
    //	st_LPC_GPDMA_Ch[DMA_Chn]->CConfig |= 0x01;   						                                //Enale Chn 0 		//nex time receive address
    if( m_UartRxQueue.m_length<=MAX_BUF_LENGTH) //CirQueue full
    {
        memcpy((void*)&m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_RxBuf,u8_RxBuffer,MAX_UART_RX_LENGTH);
        m_UartRxQueue.m_Buf[m_UartRxQueue.m_rear].m_Length = u8_RxBuffer[UART_CMD_LENGTH_POSITION];
        m_UartRxQueue.m_rear = (m_UartRxQueue.m_rear+1)%MAX_BUF_LENGTH;
        m_UartRxQueue.m_length++;
    }
    st_LPC_GPDMA_Ch[DMA_Chn]->CConfig |= 0x01;   						                                //Enale Chn 0 		//nex time receive address
    return;

}



bool UartEnQueue(unsigned char uc_Data)
{


}
/*********************************************************************************************************
** Function name:	    UART0DMA_Rcv
** Descriptions:	    UART DMA next time transfer size
** input parameters:    pRcvbuf:
**                      uiNum:
** output parameters:   none
** Returned value:      void
*********************************************************************************************************/
void DMAFunc::UART0DMA_Rcv (uint8 uiNum)
{
    while ((LPC_GPDMA->RawIntTCStat & 0x01) == 0);
    LPC_GPDMACH0->CDestAddr  = (uint32)u8_RxBuffer;
    LPC_GPDMA->IntTCClear |= 1;
    LPC_GPDMACH0->CControl =  LPC_GPDMACH0->CControl|uiNum;
    LPC_GPDMACH0->CConfig |= 0x01;   						 //Enale Chn 0
}


/*********************************************************************************************************
End Of File
*********************************************************************************************************/
