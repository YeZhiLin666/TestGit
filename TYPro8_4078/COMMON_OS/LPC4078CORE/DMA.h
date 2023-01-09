#pragma once

#define MAX_DMA_CHN        8
#define UART0RCVFIFO       LPC_UART0_BASE// 0x4000C000	                                    /* UART FIFO Adress         */
#define UART1RCVFIFO       LPC_UART1_BASE// 0x4000C000	                                    /* UART FIFO Adress         */
#define DMA_BUFSIZE        64
#define DMA_TRANSFER_SIZE  32

#define MAX_BUF_LENGTH     20
#define MAX_DATA_LENGTH    256
#define  USE_COMPLETE_DMA_CONFIG   0  // complete config 

typedef struct
{
    unsigned char m_Length;
    unsigned char m_RxBuf[MAX_DATA_LENGTH];
} St_OneDMABuf;
typedef struct
{
    unsigned char m_front;
    unsigned char m_rear;
    unsigned char m_length;
    St_OneDMABuf m_Buf[MAX_BUF_LENGTH];
} St_QueueBuf;
class  DMAFunc
{
public: //data
    static unsigned char                 u8_RxBuffer[DMA_BUFSIZE];
    static unsigned int                  DMA_Err_Count[MAX_DMA_CHN];
private:
    static LPC_GPDMACH_TypeDef * const   st_LPC_GPDMA_Ch[MAX_DMA_CHN];
#if(USE_COMPLETE_DMA_CONFIG==1)
    static const uint32_t   GPDMA_LUTPerAddr[];
    static const uint8_t    GPDMA_LUTPerBurst[];
    static const uint8_t	   GPDMA_LUTPerWid[];
#endif
public:
    static void DMAISR(unsigned char DMA_Chn);
    static void UART0DMA_Rcv(uint8 uiNum);
    static void SetupUart0();

    static St_QueueBuf  m_UartRxQueue;
#if(USE_COMPLETE_DMA_CONFIG==1)
    static void Init(void);
    static Status GPDMA_Setup(GPDMA_Channel_CFG_Type *GPDMAChannelConfig);
#endif


private:

};

