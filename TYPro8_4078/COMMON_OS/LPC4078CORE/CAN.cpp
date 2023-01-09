/*****************************************************************************
// 程序文件      :CAN.cpp     CAN处理模块（收发缓冲区，循环队列）
// 文件描述      :LPC4078 MCU board
// 编写日期(MM.DD.YYYY)      :2013.11.18
*****************************************************************************/

#include "includes.h"


unsigned char CANFunc::CAN1Init  = 0;
unsigned char CANFunc::CAN2Init  = 0;
unsigned int CANFunc::CAN1Status;
unsigned int CANFunc::CAN2Status;
//*************************内存缓冲区变量定义*************************//

unsigned long   CANFunc::CANSendFailedCnt = 0;
unsigned  short CANFunc::SendingCnt;
//Rx and Tx Buffer Interface
unsigned int    CANFunc::RxBufIndex;                       //Enqueued by CAN_Rx_Msg()& Dequeued by CANBusParse()
unsigned short  CANFunc::RxBuffStatus;
//************************end 缓冲区变量定义*************************//
unsigned char   CANFunc::LastMsgTxRes;                    //Last sended CAN msg result
unsigned short  CANFunc::CAN_MsgNo;                        //Confim Ack Number (SN)
unsigned char   CANFunc::TrigCANTxFlg;                     //Trigger ISR Flag
unsigned char   CANFunc::CANSendFlg;                       //Tx Buffer ready flag ** Setting in CAN_TxMsgReady() and used in CAN_Tx_Msg()**
unsigned int CANFunc::ACCFRamIdx = 0;                  //RAM index
//Built_in CANBMS  Start//
unsigned char CANFunc::IsCANPort1_AsBuiltInCANBMS = 0;
unsigned char CANFunc::IsCANPort2_AsBuiltInCANBMS = 0;
//Built_in CANBMS  End//
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

// dynamic memory signals: EMC_DYCS0; EMC_CAS; EMC_RAS; EMC_CKE0; EMC_DQM0; EMC_CLKOUT1; EMC_CLKOUT0;   0XA0000000  0x01E84800 //
//*************************缓冲区变量定义*************************//
St_BMS_Filter         CANFunc::BMS_Filter; //1712211 hpq
CQ_CANBMSRXBUF2       CANFunc::CQ_CanBmsRxBuf2;
CQ_CANTXBUF           CANFunc::CQ_CanTxBuffer[2];
CQ_CANRXBUF           CANFunc::CQ_CanRxBuffer[3];
//#if(SUPPORT_CAN_BMS == 1)
ST_Multi_Pack         CANFunc::m_MutiPackets[MAXCHANNELNO][CAN_MULTI_PACK_BUF_LEN];
ST_AuxMulti_Pack         CANFunc::m_AuxMutiPackets[CAN_MULTI_PACK_BUF_LEN];
//#endif
//************************end 缓冲区变量定义*************************//

CAN_TXREG    CANFunc::TempCANTxData;                         //Dequeue TxBuffer's data to this member for transfer to CAN register format
ST_CAN_RxMSG CANFunc::TempCANRxData;
ST_CAN_RxMSG CANFunc::TempNETtoCANRxData;
CAN_RXREG    CANFunc::ReadRxReg;
unsigned char CANFunc::PushToBufRes;
//acceptance filter Ram lookup index
unsigned short CANFunc::SourceBoardID;
unsigned int   CANFunc::FilterBrdCstID;
CANFilterExtGrp     CANFunc::PrioGrpFilter[4];
MsTime	CANFunc::CAN_SendAbnTimer;                              //Sending OverTime Timer
unsigned int CANFunc::DOICnt;
unsigned int CANFunc::FullCnt;
unsigned char CANFunc::CAN1RecordBaudRate = CANBAUD500k;
unsigned char CANFunc::CAN2RecordBaudRate = CANBAUD500k;


#pragma arm section //结束外存定义
/*******************************************************************************************

********************************************************************************************/

/******************************************************************************
** Function name:		CAN_Handler
**
** Descriptions:		CAN1/2中断处理函数（收发）
**
** parameters:		None
**
** Returned value:	None
**
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
void CAN_IRQHandler()
{
    // CAN1/2中断处理函数（收发）
    //Process   CAN1 controller
    //    int ReadTemp;
    if((!CANFunc::CAN1Init)&&(!CANFunc::CAN2Init))
        return;

    if(CANFunc::CAN1Init)
    {
        CANFunc::CAN1Status = LPC_CAN1->ICR;
        for(unsigned char uc_Int = 0; uc_Int<AllowIntNum; uc_Int++)
        {
            if(!CANFunc::CAN1Status)
                break;
            UInt32 CAN1IntInfo = CANFunc::CAN1Status&(1<<uc_Int);
            switch(CAN1IntInfo)
            {
            case CANTI1:
            case CANTI2:
                CANFunc::CAN_ISRSend(CAN1);
                break;
            case CANRI:
                CANFunc::CAN_RxProcess(CAN1);
                break;
            case CANEI:
            case CANEPI:
            case CANBEI:
                //  			CANFunc::CAN_Reset(CAN1);
                CANFunc::CAN_Reset_BaudRate(CAN1,CANFunc::CAN1RecordBaudRate);
                break;
            case CANALI:
                //                  ReadTemp = LPC_CAN1->RID;
                //                  ReadTemp = LPC_CAN1->ICR;
                break;
            case CANDOI:
                LPC_CAN1->CMR |= 0x08; //清除CMR中CDO
                CANFunc::DOICnt++;
                break;
            default:
                break;  // continue;
            }
        }
    }

    //Process   CAN2 controller
    if(CANFunc::CAN2Init)
    {
        CANFunc::CAN2Status = LPC_CAN2->ICR;
        for(unsigned char uc_Int = 0; uc_Int<AllowIntNum; uc_Int++)
        {
            if(!CANFunc::CAN2Status)
                break;
            UInt32 CAN2IntInfo = CANFunc::CAN2Status&(1<<uc_Int);
            switch(CAN2IntInfo)
            {
            case  CANTI1:
            case  CANTI2:
                CANFunc::CAN_ISRSend(CAN2);
                break;
            case  CANRI:
                CANFunc::CAN_RxProcess(CAN2);
                break;
            case CANEI:
            case CANEPI:
            case CANBEI:
#if(USE_AS_AUXMCU==1)
                if(Aux_DataManager::CANBMSEnableFlag)
                {
                    if(CANBusParse::m_CANBMS_AuxInfo.m_bAuxForward)
                        CANFunc::CAN_Reset_BaudRate(CAN2,CANBusParse::m_CANBMS_AuxInfo.m_BaudRate);
                }
                else
                    CANFunc::CAN_Reset_BaudRate(CAN2,CANFunc::CAN2RecordBaudRate);
#else
                CANFunc::CAN_Reset_BaudRate(CAN2,CANFunc::CAN2RecordBaudRate);
#endif
                break;
            case CANALI:
                //                ReadTemp = LPC_CAN2->RID ;
                //                ReadTemp = LPC_CAN2->ICR;
                break;
            case CANDOI:
                LPC_CAN2->CMR |= 0x08; //清除CMR中CDO
                break;
            default:
                break;  // continue;
            }
        }
    }
}

//<<Begin   CANx（1、2）  端口初始化>>
/******************************************************************************
** Function name:		Can_Init
**
** Descriptions:		初始化CAN1/2驱动（寄存器、波特率、滤波等）
**
** parameters:		CANx
**
** Returned value:	None
**
** Created Date:      2009/12/01
**
** Revision Dates:

******************************************************************************/
void CANFunc::Can_Init(unsigned char CANx,unsigned char BaudRate)
{
    //  初始化CAN1/2驱动（寄存器、波特率、滤波等）
    if(CANx!=CAN1 &&CANx!=CAN2)
        return;
    switch(CANx)
    {
    case  CAN1:
        LPC_SC->PCONP |= (1 << 13) ;	// Enable clock to can1
        LPC_IOCON->P0_0 |= 1;
        LPC_IOCON->P0_1 |= 1;
        clear_msg_objects(CAN1);
        CAN_Init_BaudRate(CAN1,BaudRate);
        CAN1Init = 1;
        break;

    case  CAN2:
        LPC_SC->PCONP |= (1 << 14) ;	// Enable clock to can2
        LPC_IOCON->P0_4 |= 2;
        LPC_IOCON->P0_5 |= 2;
        clear_msg_objects(CAN2);
        CAN_Init_BaudRate(CAN2,BaudRate);
        CAN2Init = 1;
        break;
    default:
        break;
    }

}
void CANFunc::InitMyCAN(void)
{
    Can_Init(CAN1,CANBAUD500k);
    Can_Init(CAN2,CANBAUD500k);
    CAN_SetACCF( ACCF_BYPASS );     //ID
    IRQFunc::install_irq( CAN_IRQn, HIGHEST_PRIORITY +11 );

    LPC_CAN1->IER = 0X0CF;
    LPC_CAN2->IER = 0X0CF;
    TIMERSFunc::DELAY_SYS( 1000, 0); //delay 0.5ms
    CAN_BufferInit();
}
// void CANFunc::CAN_Reset(unsigned char CANx)
// {
// 	if(CANx!=CAN1 &&CANx!=CAN2)
// 		return ;
// 	LPC_CAN_TypeDef *pCAN = (CANx == CAN1) ? LPC_CAN1 : LPC_CAN2;
// 	clear_msg_objects(CANx);
// 	CAN_Init_BaudRate(CANx,CANBAUD500k);
// 	IRQFunc::install_irq( CAN_IRQn, HIGHEST_PRIORITY +11 );
// 	pCAN->IER =  0x0CF;//0x00000603;		// Enable receive interrupts  Enable transmit TIE1 (~TIE2 ~TIE3) DOI EI interrupts
// }
void CANFunc::CAN_Init_BaudRate(unsigned char CANx,unsigned char BaudRate)
{
    unsigned int can_btr = 0 ;
    switch(BaudRate)
    {
    case  CANBAUD125k:
        can_btr = CANBAUD125k60M;
        break;
    case  CANBAUD250k:
        can_btr = CANBAUD250k60M;
        break;
    case  CANBAUD500k:
        can_btr = CANBAUD500k60M;
        break;
    case  CANBAUD1M:
        can_btr = CANBAUD1M60M;
        break;
    default:
        can_btr = CANBAUD500k60M;
        break;
    }
    if(CANx == CAN1)
    {
        LPC_CAN1->MOD = 1;	// Reset CAN
        LPC_CAN1->IER = 0;	// Disable Receive Interrupt
        LPC_CAN1->GSR = 0;	// Reset error counter when CANxMOD is in reset
        LPC_CAN1->BTR = can_btr;  // 波特率
        LPC_CAN1->MOD = 0x0;	// CAN in normal operation mode
        CAN1RecordBaudRate = BaudRate;
    }
    else if(CANx == CAN2)
    {
        LPC_CAN2->MOD = 1;	// Reset CAN
        LPC_CAN2->IER = 0;	// Disable Receive Interrupt
        LPC_CAN2->GSR = 0;	// Reset error counter when CANxMOD is in reset
        LPC_CAN2->BTR = can_btr;  //波特率
        LPC_CAN2->MOD = 0x0;	// CAN in normal operation mode
        CAN2RecordBaudRate = BaudRate;
    }
}

void CANFunc::CAN_Reset_BaudRate(unsigned char CANx,unsigned char BaudRate)
{
    unsigned int can_btr = 0 ;
    switch(BaudRate)
    {
    case  CANBAUD125k:
        can_btr = CANBAUD125k60M;
        break;
    case  CANBAUD250k:
        can_btr = CANBAUD250k60M;
        break;
    case  CANBAUD500k:
        can_btr = CANBAUD500k60M;
        break;
    case  CANBAUD1M:
        can_btr = CANBAUD1M60M;
        break;
    default:
        can_btr = CANBAUD500k60M;
        break;
    }
    if(CANx == CAN1)
    {
        LPC_CAN1->IER = 0;	// Disable Receive Interrupt
        LPC_CAN1->GSR = 0;	// Reset error counter when CANxMOD is in reset
        LPC_CAN1->MOD = 1;	// Reset CAN
        LPC_CAN1->BTR = can_btr;  // 波特率
        LPC_CAN1->MOD = 0x0;	// CAN in normal operation mode
        LPC_CAN1->IER = 0x0CF;	// Enable Receive Interrupt
        CAN1RecordBaudRate = BaudRate;
    }
    else if(CANx == CAN2)
    {
        LPC_CAN2->IER = 0;	// Disable Receive Interrupt
        LPC_CAN2->GSR = 0;	// Reset error counter when CANxMOD is in reset
        LPC_CAN2->MOD = 0x01;
        LPC_CAN2->BTR = can_btr;  //波特率
        LPC_CAN2->MOD = 0x0;	// CAN in normal operation mode
        LPC_CAN2->IER = 0x0CF;	// Enable Receive Interrupt
        CAN2RecordBaudRate = BaudRate;
    }
}


/******************************************************************************
** Function name:		CAN_SetACCF
**
** Descriptions:		CAN Acceptance Mode
**
** parameters:		ACCFMode
**
** Returned value:	None
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
void CANFunc::CAN_SetACCF(unsigned int ACCFMode )
{
    //CAN Acceptance Mode
    //CAN1and  CAN2 are suitable
    switch( ACCFMode )
    {
    case ACCF_OFF:                     //
        LPC_CANAF->AFMR = ACCFMode;
        LPC_CAN1->MOD  = 1;	// Reset CAN
        LPC_CAN1->IER    = 0;	// Disable Receive Interrupt
        LPC_CAN1->GSR   = 0;	// Reset error counter when CANxMOD is in reset
        LPC_CAN2->MOD = 1;	// Reset CAN
        LPC_CAN2->IER = 0;	// Disable Receive Interrupt
        LPC_CAN2->GSR = 0;	// Reset error counter when CANxMOD is in reset
        break;

    case ACCF_BYPASS:            //旁路
        LPC_CANAF->AFMR = ACCFMode;
        break;

    case ACCF_ON:                 //打开
    case ACCF_FULLCAN:          //暂未使用FULLCAN （只能标准帧）
        LPC_CANAF->AFMR = ACCF_OFF;
        CAN_SetACCF_Lookup();     //填入对应帧类型的滤波ID、IDGroup
        LPC_CANAF->AFMR = ACCFMode;
        break;

    default:
        break;
    }
}




/***************************************************
** Function name:		CAN_BufferInit
**
** Descriptions:		CAN缓冲区初始化
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:
**
** Revision Dates:
************************************************************/

void CANFunc::CAN_BufferInit(void)
{
    // CAN缓冲区初始化
    //tx buffer
// 	INIT_QUEUE(CQ_CanTxBuffer[0]);
// 	INIT_QUEUE(CQ_CanTxBuffer[1]);
    for(unsigned char idx = 0; idx<2; idx++)
        INIT_QUEUE(CQ_CanTxBuffer[idx]);
    CAN_MsgNo = 0;
    SendingCnt = 0;
    CANSendFailedCnt = 0;

    //Recv Buffer Initial
    RxBufIndex = 0;
    ReadRxReg.RxID=0;
    memset(&ReadRxReg.RxData.uc_Data[0],0, sizeof(unsigned char)* 72);
// 	ReadRxReg.RxData.uw32_Data[0] = 0;
// 	ReadRxReg.RxData.uw32_Data[1] = 0;
// 	INIT_QUEUE(CQ_CanRxBuffer[0]);
// 	INIT_QUEUE(CQ_CanRxBuffer[1]);
    for(unsigned char idx = 0; idx<3; idx++)
        INIT_QUEUE(CQ_CanRxBuffer[idx]);
// 	for(unsigned char chnidx = 0;chnidx<MAXCHANNELNO;chnidx++)
// 		memset((void*)&m_MutiPackets[chnidx][0].Status,0,sizeof(m_MutiPackets)*CAN_MULTI_PACK_BUF_LEN);

    for(unsigned char chnidx = 0; chnidx<MAXCHANNELNO; chnidx++)
        memset((void*)&m_MutiPackets[chnidx][0].Status,0,sizeof(ST_Multi_Pack)*CAN_MULTI_PACK_BUF_LEN);
}


/******************************************************************************
** Function name:		clear_msg_objects
**
** Descriptions:		清除CANx的寄存器buffer 必须CAN1&CAN2都打开PCONP
**
** parameters:		None
**
** Returned value:	None
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
void CANFunc::clear_msg_objects(unsigned char CANx)	//Clear Message Objects
{
    // 清除CANx的寄存器buffer 必须CAN1&CAN2都打开PCONP
    LPC_CAN_TypeDef *pCAN = (CANx == CAN1) ? LPC_CAN1 : LPC_CAN2;

    pCAN->MOD = 1;           // Reset CAN 1 or CAN 2
    pCAN->TFI1 =  0x00000000;    //TBS1
    pCAN->TID1 =  0x00000000;
    pCAN->TDA1 = 0x00000000;
    pCAN->TDB1 = 0x00000000;

    pCAN->TFI2 =  0x00000000;  //TBS2
    pCAN->TID2 =  0x00000000;
    pCAN->TDA2 = 0x00000000;
    pCAN->TDB2 = 0x00000000;
    pCAN->CMR =  0x00000000;

    pCAN->TFI3 = 0x00000000;  //TBS3
    pCAN->TID3 = 0x00000000;
    pCAN->TDA3 = 0x00000000;
    pCAN->TDB3 = 0x00000000;

    pCAN->RFS =  0x00000000;
    pCAN->RID =  0x00000000;
    pCAN->RDA=  0x00000000;
    pCAN->RDB=  0x00000000;
    pCAN->MOD = 0;	// CAN in normal operation mode

}



/******************************************************************************
** Module name:		CAN Message Receive and Send Process
**
** Descriptions:		CAN1/CAN2
**
** parameters:		CANx
**
** Returned value:	None
**
** Created Date:      2013/11/01
**
** Revision Dates:    2014/07/21

******************************************************************************/
unsigned char  CANFunc::Can_Rx_Msg(unsigned char CANx)		// Receive Data from the IF2 buffer
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return CAN_RX_ERR;
    LPC_CAN_TypeDef *pCAN = (CANx == CAN1) ? LPC_CAN1 : LPC_CAN2;
    unsigned int  RxStatus = pCAN->GSR ;
    if(RxStatus & GSR_BUFVALUE)
    {
        ReadRxReg.RxID=pCAN->RID; //RXID
        ReadRxReg.RxFrame.Frame = pCAN->RFS;
        ReadRxReg.RxData.uw32_Data[0] = pCAN->RDA; // Data A
        ReadRxReg.RxData.uw32_Data[1]=pCAN->RDB; // Data B

        TempCANRxData.FrameType = ReadRxReg.RxFrame.BitFrame.FF;
        TempCANRxData.len = ReadRxReg.RxFrame.BitFrame.DLC;
        TempCANRxData.RxData.uw32_Data[0] = ReadRxReg.RxData.uw32_Data[0];	//union WORD[2] to uc_data[8]
        TempCANRxData.RxData.uw32_Data[1] = ReadRxReg.RxData.uw32_Data[1];

        TempCANRxData.RxID = ReadRxReg.RxID;
        TempCANRxData.RxRES = 1;
        pCAN->CMR = 0x04; // release receive buffer
        return  CAN_RXOK;
    }
    else if((RxStatus |GSR_ES )||(RxStatus |GSR_BS ))
    {
        pCAN->CMR = 0x04; //
        return CAN_RX_ERR;              //待添加
    }
    pCAN->CMR = 0x04; //
    return CAN_RX_ERR;
}



//发送寄存器准备状态 ，正确则返回空闲Buffer
unsigned char CANFunc::CAN_TxReadyInfo(unsigned char CANx)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return CAN_TXERR1;
    unsigned int TxStatus = 0, counter=0;
    unsigned char uc_TBindex;
    // Wait for xmit buffer empty
    LPC_CAN_TypeDef *pCAN = (CANx == CAN1) ? LPC_CAN1 : LPC_CAN2;
    TxStatus = pCAN->SR;             //要加入防护处理 正在发送 已完成。。。TBS TCS
    if(TxStatus &0x0000000C )       //TCS1(~TCS2 ~TCS3), any TCS send ok,  TS not sending a message
    {
        SendingCnt = 0;
        CANSendFlg = 1;          //Tx Buffer Register OK
        while(CANSendFlg)
        {
            counter++;
            // selecting an empty Transmit Buffer to Send the messages.
            if(TxStatus & (TBS1 | TCS1))		// TB0 is free    // TB0&TB1 are not free, but TB2 is free
            {
                uc_TBindex = 0x01;
                break;
            }
            else
            {
                if(counter > 255)	//	Time Out
                    return CAN_TXERR1;
            }
        }
        return uc_TBindex;
    }
    else                            //在这里加一个累计 如果一直sending表明总线错误了 调试获得SendingCnt
    {
        SendingCnt++;
        if(SendingCnt<500)
            return CAN_SENDING;
        else
        {
            SendingCnt = 0;
            return CAN_TXERR1;
        }
    }
}

//CAN 发送函数，原始报文,首先要调用CAN_TxReadyInfo判断状态是否OK,OK则返回TBSIdx
unsigned char  CANFunc::Can_Tx_Msg(unsigned char CANx, unsigned char uc_TBindex,const ST_CAN_TxMsg *TxMsgBuf)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return CAN_TXFAILED;
    unsigned int TxStatus = 0;
    LPC_CAN_TypeDef *pCAN = (CANx == CAN1) ? LPC_CAN1 : LPC_CAN2;

    if(!CANSendFlg)
        return CAN_TXFAILED;
    CANSendFlg = 0;
    memcpy(TempCANTxData.Data.uc_Data, TxMsgBuf->databuf,8);
    unsigned char TxDataLen = TxMsgBuf->len;
    unsigned char FrameType = TxMsgBuf->FrameType;
    TempCANTxData.TxFrame.Frame = (TxDataLen<<16)|(FrameType<<31);
    if(TxMsgBuf->FrameType == 1)        //扩展帧
        TempCANTxData.MsgID = TxMsgBuf->TxID &0x1FFFFFFF;
    else
        TempCANTxData.MsgID = TxMsgBuf->TxID &0x7FF;
    // Set up identifier registers
    if(uc_TBindex == 0x01)		// setting TB0
    {
        pCAN->TFI1   = TempCANTxData.TxFrame.Frame & 0xC00F0000;    //TBS0
        pCAN->TID1  = TempCANTxData.MsgID;
        pCAN->TDA1 = TempCANTxData.Data.uw32_Data[0];
        pCAN->TDB1 = TempCANTxData.Data.uw32_Data[1];
        pCAN->CMR |= 0x21; //0x21;
    }
    else if(uc_TBindex == 0x02)		// setting TB1
    {
        pCAN->TFI2   = TempCANTxData.TxFrame.Frame & 0xC00F0000;    //TBS1
        pCAN->TID2  = TempCANTxData.MsgID;
        pCAN->TDA2 = TempCANTxData.Data.uw32_Data[0];
        pCAN->TDB2 = TempCANTxData.Data.uw32_Data[1];
        pCAN->CMR |= 0x41; //0x41;
    }
    else if(uc_TBindex == 0x04)		// setting TB2
    {
        pCAN->TFI3   = TempCANTxData.TxFrame.Frame & 0xC00F0000;
        pCAN->TID3  = TempCANTxData.MsgID;
        pCAN->TDA3 = TempCANTxData.Data.uw32_Data[0];
        pCAN->TDB3 = TempCANTxData.Data.uw32_Data[1];
        pCAN->CMR = 0x81;////0x8B;
    }
    else
        return CAN_TXFAILED;
    TxStatus = pCAN->SR;
    if(TxStatus&0x020202)         //添加数据超载处理
    {
        pCAN->CMR |= 0x08; //清除CMR中CDO
        return CAN_TXFAILED;//或复位
    }
    return CAN_TXOK;
}



//接收报文处理，存入接收缓冲区
unsigned char CANFunc::CAN_RxProcess(unsigned char CANx)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return RX_ERROR;
    unsigned char RxResult = 0;
    MP_CAN_MSG PushMsgToQueue;
    RxResult =  CANFunc::Can_Rx_Msg(CANx);

    // ReadNext:
    if(RxResult == CAN_RXOK)            //数据考入接收缓冲区
    {
        PushMsgToQueue.Un_CANID.m_U32ID =  CANFunc::TempCANRxData.RxID;
        memcpy(PushMsgToQueue.Un_Content.Data,CANFunc::TempCANRxData.RxData.uc_Data,8);
        if(CQ_CanRxBuffer[CANx].m_u32Length >(CAN_RX_BUF_LENGTH-1))  //should do...
        {
            RxBuffStatus = RX_FULL;    //报告异常，add
            return RX_FULL;
        }
        else
        {
            if(CANBusParse::CANBMS_ProcessOrigMsg(CANx,TempCANRxData)) // 处理BMS两包组合
                return RX_OK;
            PushMsgToQueue.bBMSCmd = 0;
            ENQUEUE(CQ_CanRxBuffer[CANx],CAN_RX_BUF_LENGTH,PushMsgToQueue);
            RxBufIndex++;         //Rxbuffer累加
            RxBuffStatus = RX_OK;
            if(CQ_CanRxBuffer[CANx].m_u32Length >(CAN_RX_BUF_LENGTH/2))
            {
                RxBuffStatus = RX_HALFFULL;
            }
            else if(CQ_CanRxBuffer[CANx].m_u32Length >(CAN_RX_BUF_LENGTH-1))  //should do...
            {
                RxBuffStatus = RX_FULL;    //报告异常，add
                return RX_FULL;
            }
        }
        return RxBuffStatus;
    }
    return  RX_ERROR;
}


//网络接收报文处理，存入接收缓冲区
unsigned char CANFunc::NETtoCAN_RxProcess(void)
{
// 	  unsigned char RxResult = 0;
    MP_CAN_MSG PushMsgToQueue;
    PushMsgToQueue.Un_CANID.m_U32ID =  CANFunc::TempNETtoCANRxData.RxID;
    memcpy(PushMsgToQueue.Un_Content.Data,CANFunc::TempNETtoCANRxData.RxData.uc_Data,sizeof(UN_CANWORD));
    if(CQ_CanRxBuffer[RUDPNET].m_u32Length >(CAN_RX_BUF_LENGTH-1))
    {
        return RX_FULL;
    }
    else
    {
        //if(CANBusParse::CANBMS_ProcessOrigMsg(RUDPNET,TempNETtoCANRxData))
        //	return RX_OK;

        UN_CANID CANID;
        CANID.m_U32ID = TempNETtoCANRxData.RxID;

        if(CANID.m_CANID.CmdType == BMS_CMD_TYPE)
        {
// 					 ST_CAN_RxMSG * OrgMsg = &TempNETtoCANRxData; // 处理BMS两包组合
#if(USE_AS_AUXMCU ==0)  //IV
            CANBusParse::CANBMS_IVAssembleMsgs(RUDPNET,&TempNETtoCANRxData);	 // IV for tow CAN pack Data
#else  //aux
            CANBusParse::CANBMS_AuxAssembleMsgs(RUDPNET,&TempNETtoCANRxData);	 // AUX for tow CAN pack Command
#endif
            return RX_OK;
        }

        PushMsgToQueue.bBMSCmd = 0;
        ENQUEUE(CQ_CanRxBuffer[RUDPNET],CAN_RX_BUF_LENGTH,PushMsgToQueue);
        if(CQ_CanRxBuffer[RUDPNET].m_u32Length >(CAN_RX_BUF_LENGTH/2))
            return RX_HALFFULL;
        else if(CQ_CanRxBuffer[RUDPNET].m_u32Length >(CAN_RX_BUF_LENGTH-1))
            return RX_FULL;
    }
    return RX_OK;
}


//发送中断调用该函数
unsigned char  CANFunc::CAN_ISRSend(unsigned char CANx)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return 0;
    MP_CAN_MSG DeQueue_Msg ;
    unsigned char DEQUEUE_OK = 0,uc_TxRdy,SendResult;
    ST_CAN_TxMsg CAN_TxMsg;
    ////////////ID and Data///////////////////
    if(CQ_CanTxBuffer[CANx].m_u32Length >0)      //先不dequeue报文，待上次报文发送成功才弹出，要不会丢数据。如为CAN_TXFAILED缓存 然后不dequeue，然后在去尝试
    {   //利用中断发送
        uc_TxRdy = CANFunc::CAN_TxReadyInfo(CANx);
        switch(uc_TxRdy)
        {
        case CAN_SENDING:
            LastMsgTxRes =  CAN_SENDING;
            break;
        case CAN_TXERR1:                                 //这里要添加处理,可能是总线错误 发送can异常
            CANSendFailedCnt++;
            LastMsgTxRes =  CAN_TXERR1;
            break;
        default:				                          //如果没有进入以上错误，则uc_TxRdy返回的是一个可以TxBuffer的index
            DEQUEUE(CQ_CanTxBuffer[CANx],CAN_TX_BUF_LENGTH,DeQueue_Msg,DEQUEUE_OK);
            if(DEQUEUE_OK)
            {
                CAN_TxMsg.TxID = DeQueue_Msg.Un_CANID.m_U32ID;
                memcpy(CAN_TxMsg.databuf, DeQueue_Msg.Un_Content.Data,8);
                CAN_TxMsg.FrameType = DeQueue_Msg.FrameType; //1;
// 				CAN_TxMsg.len = 8;
                CAN_TxMsg.len = DeQueue_Msg.DLC; //1903191 zyx
                unsigned char TempCANTxRes = CANFunc::Can_Tx_Msg(CANx,uc_TxRdy,&CAN_TxMsg);
                if(TempCANTxRes == CAN_TXOK)
                {
                    LastMsgTxRes = CAN_TXOK;
                }
                else
                {
                    LastMsgTxRes = CAN_TXFAILED;
                    CANSendFailedCnt++;
                }
            }
            else
            {
                LastMsgTxRes = CAN_TXBUF_ERR;                    //发送buffer弹出错误
                CANSendFailedCnt++;
            }
            break;
        }
        //if(CANSendFailedCnt > 20)
        //	;//TrigCANTxFlg = 0;                                //暂时屏蔽   需添加报错
    }
    else
    {
        SendResult = 0;                //No Msg Need To Sended
    }
    return SendResult;
}
// 调用该函数来发送CAN报文
unsigned char CANFunc::CAN_Push_to_Send(unsigned char CANx,const MP_CAN_MSG * can_msg)
{
    //将命令做正确解析 然后存入事件缓存 等待确认
    if(CANx!=CAN1 &&CANx!=CAN2)
        return CAN_PUSH_FAIL;
    MP_CAN_MSG DeQueue_Msg;
    unsigned char DEQUEUE_OK = 0,uc_TxRdy,SendResult;
    ST_CAN_TxMsg CAN_TxMsg;
    NVIC_DisableIRQ(CAN_IRQn);
    ENQUEUE(CQ_CanTxBuffer[CANx],CAN_TX_BUF_LENGTH,*can_msg);
    ////////////ID and Data///////////////////
    if(CQ_CanTxBuffer[CANx].m_u32Length >0)      //
    {   //利用中断发送
        uc_TxRdy = CANFunc::CAN_TxReadyInfo(CANx);
        switch(uc_TxRdy)
        {
        case CAN_SENDING:
            LastMsgTxRes =  CAN_SENDING;
            break;
        case CAN_TXERR1:                                 //这里要添加处理,可能是总线错误 发送can异常
            CANSendFailedCnt++;
            LastMsgTxRes =  CAN_TXERR1;
            break;
        default:			                          //如果没有进入以上错误，则uc_TxRdy返回的是一个可以TxBuffer的index
            DEQUEUE(CQ_CanTxBuffer[CANx],CAN_TX_BUF_LENGTH,DeQueue_Msg,DEQUEUE_OK);
            if(DEQUEUE_OK)
            {
                memcpy(CAN_TxMsg.databuf, DeQueue_Msg.Un_Content.Data,8);  //暂时先写最大8DeQueue_Msg.DLC
                CAN_TxMsg.TxID = DeQueue_Msg.Un_CANID.m_U32ID;
                CAN_TxMsg.FrameType = DeQueue_Msg.FrameType;
                CAN_TxMsg.len =  DeQueue_Msg.DLC;
                unsigned char TempCANTxRes = CANFunc::Can_Tx_Msg(CANx,uc_TxRdy,&CAN_TxMsg);
                if(TempCANTxRes == CAN_TXOK)
                {
                    LastMsgTxRes = CAN_TXOK;
                }
                else
                {
                    LastMsgTxRes = CAN_TXFAILED;
                    CANSendFailedCnt++;
                }
            }
            else
            {
                LastMsgTxRes = CAN_TXBUF_ERR;                    //发送buffer弹出错误
                CANSendFailedCnt++;
            }
            break;
        }
        if(CANSendFailedCnt > 20)
            ;//TrigCANTxFlg = 0;                                //暂时屏蔽   需添加报错
    }
    else
    {
        //暂时屏蔽
        SendResult = 0;                //No Msg Need To Sended
    }
    NVIC_EnableIRQ(CAN_IRQn);		 //这里放开
    return SendResult;
}

//CAN立即发送，如果硬件状态未完成，则存入发送缓冲区
// unsigned char  CANFunc::CAN_Immediate_Send(unsigned char CANx,const MP_CAN_MSG * can_msg)
// {
// 	if(CANx!=CAN1 &&CANx!=CAN2)
// 		return CAN_TXERR1;
// 	unsigned char uc_TxRdy;
// 	ST_CAN_TxMsg CAN_TxMsg;
// 	////////////ID and Data///////////////////
// 	uc_TxRdy = CANFunc::CAN_TxReadyInfo(CANx);
// 	switch(uc_TxRdy)
// 	{
// 	case CAN_SENDING:
// 		LastMsgTxRes =  CAN_SENDING;
// 		if( CQ_CanTxBuffer[CANx].m_u32Length <= CAN_TX_BUF_LENGTH)               //发送缓冲区不满
// 		{
// 			ENQUEUE(CQ_CanTxBuffer[CANx],CAN_TX_BUF_LENGTH,*can_msg);
// 		}
// 		break;
// 	case CAN_TXERR1:                                 //这里要添加处理,可能是总线错误 发送can异常
// 		CANSendFailedCnt++;
// 		LastMsgTxRes =  CAN_TXERR1;
// 		break;
// 	default:				                          //如果没有进入以上错误，则uc_TxRdy返回的是一个可以TxBuffer的index
// 		CAN_TxMsg.TxID = can_msg->Un_CANID.m_U32ID;
// 		memcpy(CAN_TxMsg.databuf, can_msg->Un_Content.Data,8);
// 		CAN_TxMsg.FrameType = 1;
// 		CAN_TxMsg.len = 8;
// 		unsigned char TempCANTxRes = CANFunc::Can_Tx_Msg(CANx,uc_TxRdy,&CAN_TxMsg);
// 		if(TempCANTxRes == CAN_TXOK)
// 		{
// 			LastMsgTxRes = CAN_TXOK;
// 		}
// 		else
// 		{
// 			LastMsgTxRes = CAN_TXFAILED;
// 			CANSendFailedCnt++;
// 		}
// 		if(CANSendFailedCnt > 20)
// 			;                                //暂时屏蔽   需添加报错
// 	}
// 	return  LastMsgTxRes;
// }


/************************************************************
**
**             CAN硬件滤波模块
**
*************************************************************/
//预留过滤单个广播包&命令帧组
void CANFunc::InitFilterID()
{
    //最高位过滤该板ID group过滤
    unsigned char BoardCANID = CANFunc::getBID()+1;

    //DOWNLimit  trig log 高优先级组
    PrioGrpFilter[0].ExtLow =  0X00000000 |BoardCANID<<20;
    PrioGrpFilter[0].ExtHigh = 0X0007FFFF |BoardCANID<<20;

    PrioGrpFilter[1].ExtLow =  0X10000000 |BoardCANID<<20;
    PrioGrpFilter[1].ExtHigh = 0X1007FFFF |BoardCANID<<20;
    PrioGrpFilter[2].ExtLow = 0x0FF00000;
    PrioGrpFilter[2].ExtHigh = 0x0FFfffff;
    PrioGrpFilter[3].ExtLow = 0x1FF00000;
    PrioGrpFilter[3].ExtHigh = 0x1FFfffff;
    //其他低优先级组 A-B
}


/******************************************************************************
** Function name:		CAN_SetACCF_Lookup
**
** Descriptions:		Initialize CAN, install CAN interrupt handler   RAM滤波
**                     标准 标准组 扩展 扩展组
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void CANFunc::CAN_SetACCF_Lookup( void )
{
    // Set explicit standard Frame
    //unsigned int Mask[20];

    CANFunc::InitFilterID();
    ACCFRamIdx = 0;
    //标准帧明晰滤波
    LPC_CANAF->SFF_sa = ACCFRamIdx;                //起始地址
    //标准帧组滤波
    LPC_CANAF->SFF_GRP_sa = LPC_CANAF->SFF_sa;

    // CAN BMS STD FRAME GROUP
#if(USE_AS_AUXMCU ==1)
    CANFunc::CAN_StdGrpFilter(CAN2,0,0x7FF);
    LPC_CANAF->EFF_sa = LPC_CANAF->SFF_GRP_sa+ACCFRamIdx<<2;
    LPC_CANAF->EFF_GRP_sa = LPC_CANAF->EFF_sa;

    CAN_ExtGrpFilter(CAN1,PrioGrpFilter[0].ExtLow,PrioGrpFilter[0].ExtHigh);
    // CAN PARALLEL BroadCast Filter  temp CAN1
    CAN_ExtGrpFilter(CAN1,PrioGrpFilter[2].ExtLow,PrioGrpFilter[2].ExtHigh);  //
    CAN_ExtGrpFilter(CAN1,PrioGrpFilter[1].ExtLow,PrioGrpFilter[1].ExtHigh);
    CAN_ExtGrpFilter(CAN1,PrioGrpFilter[3].ExtLow,PrioGrpFilter[3].ExtHigh);  //

    CAN_ExtGrpFilter(CAN2,0,0x1FFFFFFF);
#else
    if(IsCANPort1_AsBuiltInCANBMS)
    {
        CANFunc::CAN_StdGrpFilter(CAN1,0,0x7FF);
    }
    if(IsCANPort2_AsBuiltInCANBMS)
    {
        CANFunc::CAN_StdGrpFilter(CAN2,0,0x7FF);
    }
    LPC_CANAF->EFF_sa = LPC_CANAF->SFF_GRP_sa+ACCFRamIdx<<2;
    LPC_CANAF->EFF_GRP_sa = LPC_CANAF->EFF_sa;

    if(IsCANPort1_AsBuiltInCANBMS)
    {
        CAN_ExtGrpFilter(CAN1,0,0x1FFFFFFF);
    }
    else
    {
        CAN_ExtGrpFilter(CAN1,PrioGrpFilter[0].ExtLow,PrioGrpFilter[0].ExtHigh);
        // CAN PARALLEL BroadCast Filter  temp CAN1
        CAN_ExtGrpFilter(CAN1,PrioGrpFilter[2].ExtLow,PrioGrpFilter[2].ExtHigh);  //
        CAN_ExtGrpFilter(CAN1,PrioGrpFilter[1].ExtLow,PrioGrpFilter[1].ExtHigh);
        CAN_ExtGrpFilter(CAN1,PrioGrpFilter[3].ExtLow,PrioGrpFilter[3].ExtHigh);  //


    }
    if(IsCANPort2_AsBuiltInCANBMS)
    {
        CAN_ExtGrpFilter(CAN2,0,0x1FFFFFFF);
    }
    else
    {
        CAN_ExtGrpFilter(CAN2,PrioGrpFilter[0].ExtLow,PrioGrpFilter[0].ExtHigh);
        CAN_ExtGrpFilter(CAN2,PrioGrpFilter[1].ExtLow,PrioGrpFilter[1].ExtHigh);
    }
#endif
    LPC_CANAF->ENDofTable +=(ACCFRamIdx<<2);
}
// Extended Explicit Frame  filter
//void CANFunc::CAN_ExtExpFilter(unsigned char CANx,uint32 ExpID)
//{
//	if(CANx!=CAN1 &&CANx!=CAN2)
//		return;
//	uint32 tempRAMIdx,SCC;
//	SCC = CANx<<29;
//	tempRAMIdx = ACCFRamIdx;
//	LPC_CANAF_RAM->mask[tempRAMIdx] = SCC|ExpID;
//	ACCFRamIdx++;

//}
// Extended Frame Group filter
void CANFunc::CAN_ExtGrpFilter(unsigned char CANx,uint32 ExtIDFrom,uint32 ExtIDTo)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return;
    unsigned int Mask[2],SCC;
    uint32 tempRAMIdx = ACCFRamIdx;                 // 临时扩展id地址+ID*4)
    Mask[0] = ExtIDFrom &0x1FFFFFFF;
    Mask[1] =  ExtIDTo &0x1FFFFFFF;
    SCC = CANx<<29;
    LPC_CANAF_RAM->mask[tempRAMIdx] = SCC | Mask[0];
    LPC_CANAF_RAM->mask[tempRAMIdx+1] = SCC| Mask[1];
    ACCFRamIdx+= 2;

}

// Extended Frame Group filter
void CANFunc::CAN_StdGrpFilter(unsigned char CANx,uint32 StdIDFrom,uint32 StdIDTo)
{
    if(CANx!=CAN1 &&CANx!=CAN2)
        return;
    unsigned int Mask[2];
    uint32 tempRAMIdx = ACCFRamIdx;                 // 临时扩展id地址+ID*4)
    Mask[0] = (CANx<<29)| ((StdIDFrom &0x7FF)<<16);
    Mask[1] = (CANx<<13)| (StdIDTo &0x7FF);
    LPC_CANAF_RAM->mask[tempRAMIdx] = Mask[0]| Mask[1];
    ACCFRamIdx+= 1;
}
//Built_in CANBMS  Start//
#if(USE_AS_AUXMCU ==0)
void CANFunc::CAN_ChangeACCF(void)
{
    if((IsCANPort1_AsBuiltInCANBMS != ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN1PortUsedAsCANBMS)\
            ||(IsCANPort2_AsBuiltInCANBMS != ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN2PortUsedAsCANBMS))
    {
        IsCANPort1_AsBuiltInCANBMS = ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN1PortUsedAsCANBMS;
        IsCANPort2_AsBuiltInCANBMS = ChannelInfo_Manager::BoardCFG.m_Global.m_bCAN2PortUsedAsCANBMS;
        //CAN_SetACCF(ACCF_BYPASS);
    }
}
#endif
//Built_in CANBMS  End//
//暂时为了测试引用
unsigned char CANFunc::getBID(void)
{
    //BS0-BS7    P3.24-P3.31
    unsigned char BoardID = 0;

    St_Port MyPort;
    MyPort.PortType = eCPTYPE_MCUGPIO;
    MyPort.Function = GPIO_I;
    MyPort.Port = 3;
    MyPort.Bits = 24;

    for(unsigned char i = 0; i<8; i++)
    {
        ControlPin::SetIObyCP(MyPort);
        BoardID |= ControlPin::ReadMe(MyPort)<<i;

        MyPort.Bits++;
    }

    return BoardID;
}

bool CANFunc::IsCANopen(void)
{
    return (CAN1Init||CAN2Init);

}
/******************************************************************************
**                            End Of File
******************************************************************************/
