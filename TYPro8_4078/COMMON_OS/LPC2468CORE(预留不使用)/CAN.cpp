/*****************************************************************************
// 程序文件      :CAN.cpp     主要用于CAN硬件驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#include "../../COMMON/includes.h"



/*
//Receive Queue: one queue for each CAN port
//extern CAN_MSG MsgBuf_RX1, MsgBuf_RX2;
//extern UWord32 CAN1RxDone, CAN2RxDone;
CAN_MSG MsgBuf_TX1, MsgBuf_TX2; // TX and RX Buffers for CAN message
//CAN_MSG MsgBuf_RX1, MsgBuf_RX2; // TX and RX Buffers for CAN message
//UWord32 CAN1RxDone, CAN2RxDone;

UWord32 CANStatus;
UWord32 CAN1RxCount  = 0, CAN2RxCount  = 0;
UWord32 CAN1ErrCount = 0, CAN2ErrCount = 0;
unsigned char  c_pointer;
UWord32 dummy;
UWord32 * pDest;*/

//-------------------------------------------------
	CAN_MSG CANFunc::MsgBuf_TX1; 
	CAN_MSG CANFunc::MsgBuf_TX2; // TX and RX Buffers for CAN message
	UWord32 CANFunc::CAN1RxCount;
	UWord32 CANFunc::CAN2RxCount;
	UWord32 CANFunc::CANStatus;
	UWord32 CANFunc::CAN1ErrCount;
	UWord32 CANFunc::CAN2ErrCount;
	unsigned char  CANFunc::c_pointer;
	//UWord32 dummy;
	UWord32 * CANFunc::pDest;
// 设置变量定义在外存

#pragma arm section rwdata = "SRAM",zidata = "SRAM"		  // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01E84800
// dynamic memory signals: EMC_DYCS0; EMC_CAS; EMC_RAS; EMC_CKE0; EMC_DQM0; EMC_CLKOUT1; EMC_CLKOUT0;   0XA0000000  0x01E84800 //

	unsigned short CANFunc::uw16_RevStat;			                // receving status, receiving buffer pointer
    unsigned char  CANFunc::uc_s_Buf[CAN_BUF_NO][CAN_BUF_LEN];		// short buffer array   5包缓存帧数据
    unsigned char  CANFunc::uc_s_Complete_Flg[CAN_BUF_NO];			// short buffer's flags of : filling/filled/been processed(empty). 1/2/0    现在5包状态
    unsigned char  CANFunc::uc_s_M_R[CAN_BUF_NO][CAN_BUF_INDEX_LEN];	// short buffer's index of received messages.    包索引
    unsigned char  CANFunc::uc_s_SourceID[CAN_BUF_NO];				// short buffer's message group's source ID            包源ID
    unsigned char  CANFunc::uc_s_GrpID[CAN_BUF_NO];					// short buffer's message group's group ID      包组号
    unsigned char  CANFunc::uc_s_BytesNoInBuf[CAN_BUF_NO];			// the no. of message-bytes in short buffers.            包中总共字节数含包头和校验
    unsigned char  CANFunc::uc_s_M_No[CAN_BUF_NO];					// the no. of messages of the group being received in short buffers.
    //UWord16        CANFunc::uw16_CAN_s_Buf_TimeStampNo[CAN_BUF_NO];
    UWord16        CANFunc::uw16_CAN_s_Buf_TimeStamp[CAN_BUF_NO];
    RxBuffers CANFunc::str_RxBuf[64];						// 16 str_TxBuf can send about 94 bytes.      

	UW32DATA CANFunc::uw32_CANtemp;
    //unsigned short *uc_can_xdata_ptr;		// tx buffer pointer
    unsigned char CANFunc::uc_TxBuf[100];   //[300];       //帧数据发送缓冲
    unsigned char CANFunc::uc_RxBuf[100];   //[300];       //帧数据接收缓冲
	UWord16       CANFunc::uw16_S_gID;                  //帧索引号
	TxBuffers CANFunc::str_TxBuf[64];
	signed short  CANFunc::w16_TxB_SP;	// TxBuffers StartPoint, TxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_TxBuf[uc_TxB_SP]
    signed short  CANFunc::w16_TxB_DL;	// TxBuffers StartPoint, TxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_TxBuf[uc_TxB_SP]
	unsigned char CANFunc::uc_flg_TxIdle;		    // if Tx buffer is Idle.//  0表示发送中断处于开始状态

    signed short  CANFunc::w16_RxB_SP;	// RxBuffers StartPoint, RxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_RxBuf[uc_RxB_SP]
	signed short  CANFunc::w16_RxB_DL;	// RxBuffers StartPoint, RxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_RxBuf[uc_RxB_SP]
    unsigned char CANFunc::uc_flg_RxIdle; // if Rx buffer is Idle.//  0表示接收中断处于开始状态
    
    unsigned char CANFunc::uc_Arbitration;
	unsigned char CANFunc::uc_BEIEERR;
	unsigned char CANFunc::uc_EIEERR;
	unsigned char CANFunc::uc_ArBitrationFlag;
	UWord32       CANFunc::uw32_TBindex;
	UWord16       CANFunc::uw16_ArbitrationCounter;
	unsigned char CANFunc::uc_IVCH_CmdBuf_SID;						// where the command comes from, source dspid
#pragma arm section //结束外存变量定义
//-------------------------------------------------

extern U8 own_hw_adr[ETH_ADRLEN];

/******************************************************************************
** Function name:		CAN_Handler
**
** Descriptions:		CAN中断处理程序
**                               
** parameters:		None
**                                      
** Returned value:	None
**                    
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CAN_Handler(void) __irq 
{
   //CAN1和CAN2共用接收中断入口服务程序	
	CANFunc::CANStatus = CAN1ICR;
	//uc_flg_TxIdle =1;
    if( CANFunc::CANStatus & (1 << 0) )    //CAN1 receive
	{
        CANFunc::c_pointer=CAN1RID & 0x0000007F;		
		if((CANFunc::c_pointer ==  own_hw_adr[5] )||(CANFunc::c_pointer==  0x7F) )      //旁路模式进行软滤波0x7F = 127
        {
            CANFunc::CAN1RxCount++;
            CANFunc::Can_Recv_Data();
        }
		CAN1CMR = 0x04; // release receive buffer
	}
	if( CANFunc::CANStatus & (1 << 6) ) 
	{

		CANFunc::uc_Arbitration=1;
	}
	/*if( CANStatus & (1 << 7) ) //BEIE
	{

		uc_BEIEERR = 1;
	}
	if( CANStatus & (1 << 2) ) //EIE
	{

		uc_EIEERR = 1;
	}	*/

   // uc_flg_TxIdle =0;
   VICVectAddr = 0;		// Acknowledge Interrupt //
}


//<<Begin   CAN  开放式接口界面函数>>
/******************************************************************************
** Function name:		Can_Init
**
** Descriptions:		总体初始化设置
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::Can_Init(void)
{
    //unsigned char i = 0;
    //CAN1_Init(100); // 1Mhz   
    //CAN1_Init(50) ; //500Khz 
    CAN1_Init(25);  //250Khz       //  上位机和下位机要同时改CAN波特率
    //CAN1_Init(12);  //125Khz

    CAN_SetACCF( ACCF_BYPASS );     //监视状态利用软件过滤ID	  ACCF_BYPASS旁路模式 	 
    //CAN_SetACCF( ACCF_FULLCAN );    //硬件跟据ID索引表过滤ID	  ACCF_FULLCAN全局过滤模式
}

/******************************************************************************
** Function name:		CAN2_Init
**
** Descriptions:		总体初始化设置2
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::CAN2_Init(unsigned char uc_brate)    // uc_brate,速率: 100, 1M/bps; 50, 500kbps; 25, 250kbps; 12, 125kbps; 其它, 500kbps
{
    //  CAN1硬件初始化程序
    //uc_brate      速率: 100, 1M/bps; 50, 500kbps; 25, 250kbps; 12, 125kbps; 其它, 500kbps
    UWord32 can_btr = 0 ;
    switch(uc_brate)
    {
        case 100:	can_btr = BITRATE1M18MHZ;    break; // 1Mbps的设定值   主频72MHZ， 4分频时
        case 50:	can_btr = BITRATE500K18MHZ;  break;  // 500Kbps的设定值
        case 25:	can_btr= BITRATE250K18MHZ;   break; // 250kbps的设定
        case 12:	can_btr= BITRATE125K18MHZ;   break;// 125kbps的设定
        default:	can_btr = BITRATE500K18MHZ;  break;// 默认500Kbps设定

        /*case 100:	can_btr = BITRATE500K36MHZ;    break; // 1Mbps的设定值   主频72MHZ， 2分频时
        case 50:	can_btr = BITRATE500K36MHZ;  break;  // 500Kbps的设定值
        case 25:	can_btr= BITRATE250K36MHZ;   break; // 250kbps的设定
        case 12:	can_btr= BITRATE125K36MHZ;   break;// 125kbps的设定
        default:	can_btr = BITRATE500K36MHZ;  break;// 默认500Kbps设定*/
    }
//	CAN2RxDone =  FALSE;
	for(uw16_S_gID = 0; uw16_S_gID < 64; uw16_S_gID ++)			// 清除数据标志。
	{
		str_RxBuf[uw16_S_gID].uc_valid = 0;
	}

	for(uw16_S_gID = 0; uw16_S_gID < CAN_BUF_NO; uw16_S_gID ++)
	{
		uc_s_Complete_Flg[uw16_S_gID] = 0; 
	}
	uw16_S_gID = 1;
	uc_flg_TxIdle = 0;

	clear_msg_objects();	// Clear CAN RAM

	PCONP |= (1 << 14);	// Enable clock to the peripheral


    PINSEL0	&= ~0x00000F00;
	PINSEL0 |= 0x0000A00;	//  port0.4~5, function 0x10
	
	CAN2MOD = 1;	// Reset CAN  禁止模式
	CAN2IER = 0;	// Disable Receive Interrupt   中断使用
	CAN2GSR = 0;	// Reset error counter when CANxMOD is in reset  全局状态

    CAN2BTR = can_btr;   //总线时序
        
	CAN2MOD = 0x0;	// CAN in normal operation mode  正常模式
     
	// Install CAN interrupt handler
	IRQFunc::install_irq( CAN_INT,(void *) CAN_Handler, HIGHEST_PRIORITY +11 );
	CAN2IER =  0x01;//0x00000603;		// Enable receive interrupts  Enable transmit TIE1 TIE2 TIE3 interrupts	
	//return  ;
	
}

/******************************************************************************
** Function name:		CAN1_Init
**
** Descriptions:		总体初始化设置1
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::CAN1_Init(unsigned char uc_brate)    // uc_brate,速率: 100, 1M/bps; 50, 500kbps; 25, 250kbps; 12, 125kbps; 其它, 500kbps
{
    //CAN2硬件初始化程序
    //uc_brate  速率: 100, 1M/bps; 50, 500kbps; 25, 250kbps; 12, 125kbps; 其它, 500kbps
    UWord32 can_btr = 0 ;
    switch(uc_brate)
    {
        case 100:	can_btr = BITRATE1M18MHZ;    break; // 1Mbps的设定值   主频72MHZ， 4分频时
        case 50:	can_btr = BITRATE500K18MHZ;  break;  // 500Kbps的设定值
        case 25:	can_btr= BITRATE250K18MHZ;   break; // 250kbps的设定
        case 12:	can_btr= BITRATE125K18MHZ;   break;// 125kbps的设定
        default:	can_btr = BITRATE500K18MHZ;  break;// 默认500Kbps设定
        		
        /*case 100:	can_btr = BITRATE500K36MHZ;    break; // 1Mbps的设定值   主频72MHZ， 2分频时
        case 50:	can_btr = BITRATE500K36MHZ;  break;  // 500Kbps的设定值
        case 25:	can_btr= BITRATE250K36MHZ;   break; // 250kbps的设定
        case 12:	can_btr= BITRATE125K36MHZ;   break;// 125kbps的设定
        default:	can_btr = BITRATE500K36MHZ;  break;// 默认500Kbps设定*/
    }
//	CAN1RxDone = FALSE;
	for(uw16_S_gID = 0; uw16_S_gID < 64; uw16_S_gID ++)			// 清除数据标志。
	{
		str_RxBuf[uw16_S_gID].uc_valid = 0;
	}

	for(uw16_S_gID = 0; uw16_S_gID < CAN_BUF_NO; uw16_S_gID ++)
	{
		uc_s_Complete_Flg[uw16_S_gID] = 0; 
	}
	uw16_S_gID = 1;
	uc_flg_TxIdle = 0;
	uc_ArBitrationFlag=1;
	uw16_ArbitrationCounter=0;

    w16_TxB_SP=0;
    w16_TxB_DL=0;	

	clear_msg_objects();	// Clear CAN RAM

    PCONP |= (1 << 13) ;	// Enable clock to the peripheral

    PINSEL0	&= ~0x0000000F;
	PINSEL0 |= 0x0000005;	// port0.0~1, function 0x01, 

	CAN1MOD  = 1;	// Reset CAN  禁止模式
	CAN1IER    = 0;	// Disable Receive Interrupt   中断使用
	CAN1GSR  = 0;	// Reset error counter when CANxMOD is in reset  全局状态

    CAN1BTR = can_btr;   //总线时序
        
	CAN1MOD = 0x8;	// CAN in normal operation mode  正常模式

	//Install CAN interrupt handler
	IRQFunc::install_irq( CAN_INT,(void *) CAN_Handler, HIGHEST_PRIORITY +11 );
	//CAN1IER =  0x01;		// Enable receive interrupts
	CAN1IER =  0x41;
	//CAN1IER =  0xcd;
	TIMERSFunc::DELAY_SYS( 3000 ); //delay 0.5ms	
}

/******************************************************************************
** Function name:		CAN_SetACCF
**
** Descriptions:		CAN设置 过滤
**                               
** parameters:		ACCFMode
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::CAN_SetACCF(UWord32 ACCFMode )
{
    //全局检收过滤器    CAN1和CAN2一样，仅有关闭时不一样
	switch( ACCFMode )      //全局检收过滤器     //验收过滤
	{
		case ACCF_OFF:                     //检收过滤器不工作            
    		CAN_AFMR = ACCFMode;      
    		CAN1MOD  = 1;	// Reset CAN
    		CAN1IER    = 0;	// Disable Receive Interrupt
    		CAN1GSR   = 0;	// Reset error counter when CANxMOD is in reset
    		CAN2MOD = 1;	// Reset CAN
    		CAN2IER = 0;	// Disable Receive Interrupt
    		CAN2GSR = 0;	// Reset error counter when CANxMOD is in reset
            break;

		case ACCF_BYPASS:            //信息被使用的CAN控制器接收
			CAN_AFMR = ACCFMode;
			break;

		case ACCF_ON:                   //将接收到的CAN标识符ID屏蔽
		case ACCF_FULLCAN:          //过滤器处理指定ID值的信息接收和保存
			CAN_AFMR = ACCF_OFF;
			CAN_SetACCF_Lookup();     //验收过滤器表格
			CAN_AFMR = ACCFMode;
			break;

		default:break;
	}
}

/******************************************************************************
** Function name:		Can_Tx_Msgs
**
** Descriptions:		CAN发送消息
**                               
** parameters:		uc_no    uc_DesAddr
**                                      
** Returned value:	返回0表示失败    返回1表示单帧成功  返回2表示多帧成功  
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
unsigned char CANFunc::Can_Tx_Msgs(unsigned char uc_no, unsigned char uc_DesAddr)
{
    //CAN多帧发送程序
    //uc_no 发送个数      uc_DesAddr  目标地址
    //Returned value:	        没有足够缓冲区返回0 ；(已作处理等到有空间为止)
    //发送单帧成功返回1； 发送多帧成功返回2
	UWord32 uw32_ID = 0;
	unsigned char uc_i, uc_msg_no, uc_lastmsglen, uc_temp;
	unsigned char uc_j;
	signed int  uc_t_SP, uc_t_DL;
	
	unsigned char uc_dsp_id = 1;
	
    uc_flg_TxIdle = 1;
	// get the SP and DL, otherwise they may be changed during this function.
	uc_t_SP = w16_TxB_SP;
	uc_t_DL = w16_TxB_DL;
	
    // 生成Checksum校验位
	uw32_ID = 0;
	for(uc_i = 0; uc_i < uc_no; uc_i ++)
	{
		uw32_ID += uc_TxBuf[uc_i];
	}
	uc_TxBuf[uc_no] = (uw32_ID >> 8);
	uc_TxBuf[uc_no + 1] = uw32_ID;
	uc_no += 2;
    // <<<End>>>生成Checksum校验位


    // 生成29位ID
	uw32_ID = 0;
	uw32_ID = (((uw32_ID | uw16_S_gID) << 13) | uc_dsp_id) << 1;	// set gID and Source ID and then make single/multi-frame(ID07) to digit 0
	if(uc_no > 8)
	{
		uw32_ID |= 0x01;	// Set multi-messages, if single, no need to set ID7, becomes 0 automatically
	}
	uw32_ID = (uw32_ID << 7) | uc_DesAddr;	// Adding Destination Address
    // <<<End>>> 生成29位ID
	
	// 校验和生成ID虽然都放在前面，有可能执行后发现缓冲区不够而退出，导致白执行一次，浪费效率，
	// 但是也可以达到以下效果：在发送前执行这些工作，这个时间内也有可能缓冲被中断发送后增加到足够，
	// 避免缓冲不够退出，同时也可以减少取出DL和重置DL之间的间隔。

    // 检查是否有足够的发送缓冲
	if(uc_no <= 8)			// single message need to be sent.
	{
        if(uc_t_DL >= 64)	// no buffer? return
		{
            uc_flg_TxIdle = 0;
			return 0;
		}
		uc_msg_no = 1;			// 只需要一条Msg
	}
	else
	{
		uc_msg_no = (uc_no - 4) / 6 + 1;				// 共有多少msg? 这里粗略计算
		uc_lastmsglen = uc_no + 2 - uc_msg_no * 6;		// 最后一个msg的字节数。等效于 % 6
		if(uc_lastmsglen != 0)							// 如果最后一个msg字节数不为0，则+1
		{
			uc_msg_no ++;
			uc_lastmsglen += 2;							// 加上最后一帧的前两个序号长度
		}
		else			                                                      // 最后一个msg字节为0，那么就不存在这个虚拟的最后的msg, 而真正的最后一个msg长度为8。
		{
			uc_lastmsglen = 8;
		}
		if(uc_msg_no > (40 - uc_t_DL))		// not enough buffers for the message? return
		{
			uc_flg_TxIdle = 0;
			return 0;	// return a false msg.
		}
	}
	
    // 发送单帧信息
	if(uc_msg_no == 1)	
	{
        uc_temp = (uc_t_SP + uc_t_DL) % 64;     //查找空下标；发送时SP是递增，DL是递减
		
		str_TxBuf[uc_temp].uw32_ID = uw32_ID;
		str_TxBuf[uc_temp].uc_len = uc_no;
		str_TxBuf[uc_temp].uc_FirstFrame=1;
		for(uc_i = 0; uc_i < 8; uc_i ++)
		{
			str_TxBuf[uc_temp].datbuf[uc_i] = uc_TxBuf[uc_i];	
		}
		uw16_S_gID = (uw16_S_gID + 1) % 16;                // 递增 Group ID,  "& 0x0F" 相当于 "% 16"
        uc_flg_TxIdle = 0;
		w16_TxB_DL ++;	// set DL to trigger timer when DL = 0; this should be at the end of a data setting process this DL maybe changed by interrupt.				
		return 1;											// 如单帧，则整个函数至此返回。
	}
    // <<<End>>> 发送单帧信息

    //-------	
    // 发送多帧信息
	for(uc_i = 0; uc_i < uc_msg_no; uc_i ++)
	{
		uc_j = (uc_t_SP + uc_t_DL + uc_i) % 64;  //查找空下标；发送时SP是递增，DL是递减
		
		str_TxBuf[uc_j].uw32_ID = uw32_ID;
		
		str_TxBuf[uc_j].uc_len = 8;
		if((uc_i == (uc_msg_no - 1)) && (uc_lastmsglen != 0))	// if it's the last msg and not 8 bytes long.
		{
			str_TxBuf[uc_j].uc_len = uc_lastmsglen;
		}
		
		str_TxBuf[uc_j].datbuf[0] = 0;
		str_TxBuf[uc_j].datbuf[1] = uc_i;
		
		if(uc_i == 0)
		{
			str_TxBuf[uc_j].datbuf[2] = 0;
			str_TxBuf[uc_j].datbuf[3] = uc_msg_no;
			str_TxBuf[uc_j].uc_FirstFrame=1;
			 //多帧中第一帧前两字节是帧序号，跟着两字节是总共要发帧数，后四字节是数据
		}
		else
		{
			str_TxBuf[uc_j].datbuf[2] = uc_TxBuf[uc_i * 6 - 2];      //每帧前两字节是帧序号，后六字节是数据
			str_TxBuf[uc_j].datbuf[3] = uc_TxBuf[uc_i * 6 - 1];
			str_TxBuf[uc_j].uc_FirstFrame=0;
		}
		
		str_TxBuf[uc_j].datbuf[4] = uc_TxBuf[uc_i * 6];
		str_TxBuf[uc_j].datbuf[5] = uc_TxBuf[uc_i * 6 + 1];
		str_TxBuf[uc_j].datbuf[6] = uc_TxBuf[uc_i * 6 + 2];
		str_TxBuf[uc_j].datbuf[7] = uc_TxBuf[uc_i * 6 + 3];
	}
	uw16_S_gID = (uw16_S_gID + 1) % 16;// 递增Group ID, "& 0x0F" 相当于 "% 16"
    w16_TxB_DL = w16_TxB_DL+ uc_msg_no;
    uc_flg_TxIdle = 0;
	return 2;
    // <<<End>>> 发送多帧信息
}

/******************************************************************************
** Function name:		Can_Recv_Data
**
** Descriptions:		CAN接收数据
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::Can_Recv_Data(void)		// Receive Data from the IF2 buffer
{
    //  读取 CAN1收到的单帧数据存入缓存
    // 如果当前位为1，则代表这个Msg Obj有接收数据
	// 取得ID07和DATA的Byte1,Byte2判断是否是多帧位第一帧（需要优先处理来建立接受进程）
	//unsigned char uc_dsp_id = 1;
    uw32_CANtemp.value=0;
    uw32_CANtemp.value=CAN1RID; // ID		// RX标识符
					
    c_pointer=uw32_CANtemp.chardata.low & 0x7F;		
    //if((c_pointer ==  uc_dsp_id )||(c_pointer==  0x7F) )      //旁路模式进行软滤波0x7F = 127
    //  {
    // 接收msg obj的数据到逻辑Buf中去	// 设置指针到第一个接收缓冲区的标号上（初始化时设置17 ~ 32为接受缓冲区）
	uw32_CANtemp.value=0;
	uw32_CANtemp.value= CAN1RDA; // Data A
	if(((CAN1RID & 0x00000080) != 0) && (uw32_CANtemp.chardata.midl== 1) && (uw32_CANtemp.chardata.low== 0))	// 如果 是多帧，并且为第一帧，则在逻辑缓冲区内从上向下放置，否则反向
	{
		for(c_pointer = 0; c_pointer < 32; c_pointer ++)
		{
			if(str_RxBuf[c_pointer].uc_valid == 0)           //  查空缓冲区0表示没有数据
			{
				break;
			}
		}
		if(c_pointer == 32)
		{
			//break;
			//todo: 报告丢数据了,并退出这个循环。
		}
	}
	else
	{
		for(c_pointer = 32; c_pointer < 64; c_pointer ++)
		{
			if(str_RxBuf[c_pointer].uc_valid == 0)        //  查空缓冲区0表示没有数据
			{
				break;
			}
		}
		if(c_pointer == 64)
		{
			//break;
			//todo: 报告丢数据了,并退出这个循环。
		}
	}
						// 放置数据到相应逻辑缓冲区内
    uw32_CANtemp.value=0;
    uw32_CANtemp.value=CAN1RDA; // Data A
	str_RxBuf[c_pointer].uc_datbuf[0] = uw32_CANtemp.chardata.low ;
	str_RxBuf[c_pointer].uc_datbuf[1] = uw32_CANtemp.chardata.midl ;
	str_RxBuf[c_pointer].uc_datbuf[2] = uw32_CANtemp.chardata.midh ;
	str_RxBuf[c_pointer].uc_datbuf[3] = uw32_CANtemp.chardata.hig;
    uw32_CANtemp.value=0;
    uw32_CANtemp.value=CAN1RDB; // Data B
	str_RxBuf[c_pointer].uc_datbuf[4] = uw32_CANtemp.chardata.low ;
	str_RxBuf[c_pointer].uc_datbuf[5] = uw32_CANtemp.chardata.midl;
	str_RxBuf[c_pointer].uc_datbuf[6] = uw32_CANtemp.chardata.midh ;
	str_RxBuf[c_pointer].uc_datbuf[7] = uw32_CANtemp.chardata.hig;

    // 放置ID到相应逻辑缓冲区内			
	str_RxBuf[c_pointer].uw32_ID = CAN1RID; // ID		// RX标识符
		// 放置字节数目到相应逻辑缓冲区内
	str_RxBuf[c_pointer].uc_len = (UChar8) (CAN1RFS>>16);
	str_RxBuf[c_pointer].uc_len &= 0x0F;		// 获取信息内字节数
		// 放置有数据标志位
	str_RxBuf[c_pointer].uc_valid = 1;
			  // }				   
}

/******************************************************************************
** Function name:		Can_Packup
**
** Descriptions:		CAN组包接收的数据
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::Can_Packup(void)		// 组包应该在主函数cmd Parse前调用，需要在短循环中。此函数目的为解决51组包速度过低
{
//   从帧到包 组成函数
	char c_i = 0;
	for(c_i = 0; c_i < 64; c_i ++)					// 第一帧都在0-31中,所以可以确保被先解析
	{
		if(str_RxBuf[c_i].uc_valid == 1)
		{
			Recv_Routine(str_RxBuf[c_i].uw32_ID, str_RxBuf[c_i].uc_len, str_RxBuf[c_i].uc_datbuf);    //str_RxBuf_uc_datbuf[c_i]
			str_RxBuf[c_i].uc_valid = 0;			// 处理完毕清标志
		}
	}
}
//<<End  CAN  开放式接口界面函数>>


//<<Begin  CAN 封闭式函数>>
/******************************************************************************
** Function name:		CAN_SetACCF_Lookup
**
** Descriptions:		Initialize CAN, install CAN interrupt handler    检收过滤器AF表格   
**                     外发的ID跟表中存的ID相同时，才会收数据。
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void CANFunc::CAN_SetACCF_Lookup( void )
{
//   检收过滤器AF表格,  外发的ID跟表中存的ID相同时，才会收数据。
	UWord32 address = 0;
	// Set explicit standard Frame       //检收过滤器AF表格     
	CAN_SFF_SA = address;                //标准帧单个起始地址
	
    *((volatile UWord32 *)(CAN_MEM_BASE + address)) = 0x00000001;    //存入ID标识符  接收0单标准帧  1单标准帧
    address += 4; 
    *((volatile UWord32 *)(CAN_MEM_BASE + address)) =0x00020003;    //存入ID标识符   接收  2单标准帧  3单标准帧
    address += 4; 

	// Set group standard Frame    //标准帧组起始地址
    CAN_SFF_GRP_SA = address;
  
    *((volatile UWord32 *)(CAN_MEM_BASE + address)) =0x00090025; //存入ID标识符接收范围为9到37的标准帧
    address += 4; 
		
	// Set explicit extended Frame    //扩展帧单个起始地址
	CAN_EFF_SA = address;

    *((volatile UWord32*)(CAN_MEM_BASE + address)) = 0x00000000;    //存入ID标识符接收0扩展帧  
    address += 4; 
    *((volatile UWord32 *)(CAN_MEM_BASE + address)) = 0x00000002;    //存入ID标识符接收2扩展帧  
    address += 4; 


	// Set group extended Frame    //扩展帧组起始地址
	CAN_EFF_GRP_SA = address;

	*((volatile UWord32 *)(CAN_MEM_BASE + address)) =0x00000050; //存入ID标识符
	address += 4; 
	
	*((volatile UWord32 *)(CAN_MEM_BASE + address)) =0x00000100; //存入ID标识符接收范围为80到256的标准帧
	address += 4; 
		  
	// Set End of Table 
 	CAN_EOT = address;         //AF表格中止地址
}

/******************************************************************************
** Function name:		clear_msg_objects
**
** Descriptions:		清除发送接收缓冲区对象
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::clear_msg_objects(void)	//Clear Message Objects
{
//  清空 CAN1三个硬件发送缓冲区和一个硬件接收缓冲区
	CAN1MOD = CAN2MOD = 1;	// Reset CAN  禁止模式
	
	CAN1TFI1 =  0x00000000;    //缓冲区TBS1
	CAN1TID1 =  0x00000000;
	CAN1TDA1 = 0x00000000;
	CAN1TDB1 = 0x00000000;
	
	CAN1TFI2 =  0x00000000;  //缓冲区TBS2
	CAN1TID2 =  0x00000000;
	CAN1TDA2 = 0x00000000;
	CAN1TDB2 = 0x00000000;
	CAN1CMR =  0x00000000;

	CAN1TFI3 = 0x00000000;  //缓冲区TBS3
	CAN1TID3 = 0x00000000;
	CAN1TDA3 = 0x00000000;
	CAN1TDB3 = 0x00000000;

	CAN1RFS =  0x00000000; 
	CAN1RID =  0x00000000;
	CAN1RDA=  0x00000000;
    CAN1RDB=  0x00000000;

	CAN2TFI1 =  0x00000000;    //缓冲区TBS1
	CAN2TID1 =  0x00000000;
	CAN2TDA1 = 0x00000000;
	CAN2TDB1 = 0x00000000;
	
	CAN2TFI2 =  0x00000000;  //缓冲区TBS2
	CAN2TID2 =  0x00000000;
	CAN2TDA2 = 0x00000000;
	CAN2TDB2 = 0x00000000;
	CAN2CMR =  0x00000000;

	CAN2TFI3 = 0x00000000;  //缓冲区TBS3
	CAN2TID3 = 0x00000000;
	CAN2TDA3 = 0x00000000;
	CAN2TDB3 = 0x00000000;

	CAN2RFS =  0x00000000; 
	CAN2RID =  0x00000000;
	CAN2RDA=  0x00000000;
    CAN2RDB=  0x00000000;
		
    CAN1MOD = CAN2MOD = 0;	// CAN in normal operation mode  正常模式
	
			
}

/******************************************************************************
** Function name:		Can_Tx_Msg
**
** Descriptions:		CAN1单帧发送程序
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
//unsigned char  Can_Tx_Msg(UWord32 uw32_ID, unsigned char uc_msglen, unsigned char *uc_datbuf, unsigned char c_MsgNum)
unsigned char  CANFunc::Can_Tx_Msg(void)
{
//  CAN1单帧发送程序, This function tried to use all 3 ,Transmit Buffer to send datas.  
//  uw32_ID对方ID；uc_msglen数据个数；uc_datbuf  数据地址；c_MsgNum帧号；
// Returned value:		0-Successful;  1- Xmit Error;    2- Time Out;	
	UWord32 dummy = 0, counter;
	// Wait for xmit buffer empty
	counter = 0;

	/*if((w16_TxB_DL==0)||(uc_flg_TxIdle = =1))
		     return  3;*/

	while(1)
	{
		counter++;
		dummy = CAN1SR;

		// selecting an empty Transmit Buffer to Send the messages.
		if(dummy & 0x00000004)		// TB0 is free
		{
			uw32_TBindex = 0x01;
			uw16_RevStat &= CLR_CAN_XMT_TOUT;
			break;
		}
		if((dummy & 0x00000400)&&(str_TxBuf[w16_TxB_SP].uc_FirstFrame==0))			// TB0 is not empty, but TB1 is free
		{
			uw32_TBindex = 0x02;
			uw16_RevStat &= CLR_CAN_XMT_TOUT;
			break;
		}
		/*if((dummy & 0x00040000)&&(str_TxBuf[w16_TxB_SP].uc_FirstFrame==0))		// TB0&TB1 are not free, but TB2 is free
		{
			uw32_TBindex = 0x04;
			uw16_RevStat &= CLR_CAN_XMT_TOUT;
			break;
		}*/
		
		//	to avoid dead cycles
		if(dummy != 0)			// Xmit Error
		{
			uw16_RevStat |= CAN_XMT_ERR;
			return 1;
		}
		else if(counter > 255)	//	Time Out
		{
			uw16_RevStat |= CAN_XMT_TOUT;
			return 2;
		}
	}

	dummy=(UWord32) str_TxBuf[w16_TxB_SP].uc_len;
    MsgBuf_TX1.Frame = 0x80000000|(dummy<<16)|w16_TxB_SP; // 29-bit, no RTR, DLC is 8 bytes,ID索引
    MsgBuf_TX1.MsgID =str_TxBuf[w16_TxB_SP].uw32_ID ; // CAN ID
    uw32_CANtemp.value=0;
	uw32_CANtemp.chardata.low   = str_TxBuf[CANFunc::w16_TxB_SP].datbuf[0];
	uw32_CANtemp.chardata.midl  = str_TxBuf[w16_TxB_SP].datbuf[1];
	uw32_CANtemp.chardata.midh = str_TxBuf[w16_TxB_SP].datbuf[2];
	uw32_CANtemp.chardata.hig    = str_TxBuf[w16_TxB_SP].datbuf[3];
	MsgBuf_TX1.DataA = uw32_CANtemp.value;
	uw32_CANtemp.value=0;
	uw32_CANtemp.chardata.low   = str_TxBuf[w16_TxB_SP].datbuf[4];
	uw32_CANtemp.chardata.midl  = str_TxBuf[w16_TxB_SP].datbuf[5];
	uw32_CANtemp.chardata.midh = str_TxBuf[w16_TxB_SP].datbuf[6];
	uw32_CANtemp.chardata.hig    = str_TxBuf[w16_TxB_SP].datbuf[7];
	MsgBuf_TX1.DataB = uw32_CANtemp.value;
	w16_TxB_SP = (w16_TxB_SP + 1) % 64; //取帧号最大缓存20个帧
	w16_TxB_DL --;
	if(w16_TxB_DL<=0)
		w16_TxB_DL=0;
	uc_flg_TxIdle = 0;
        
	// Set up identifier registers
	if(uw32_TBindex == 0x01)		// setting TB0
	{
		CAN1TFI1   = MsgBuf_TX1.Frame & 0xC00F0000;    //缓冲区TBS1
		CAN1TID1  = MsgBuf_TX1.MsgID;
		CAN1TDA1 = MsgBuf_TX1.DataA;
		CAN1TDB1 = MsgBuf_TX1.DataB;
		CAN1CMR  = 0x21; //0x21;                                     //发送请求，选择TBS1 发送   
	}
	if(uw32_TBindex == 0x02)		// setting TB1
	{
		CAN1TFI2   = MsgBuf_TX1.Frame & 0xC00F0000;    //缓冲区TBS2
		CAN1TID2  = MsgBuf_TX1.MsgID;
		CAN1TDA2 = MsgBuf_TX1.DataA;
		CAN1TDB2 = MsgBuf_TX1.DataB;
		CAN1CMR = 0x41; //0x41;                                     //发送请求，选择TBS2 发送 
	}
	/*if(uw32_TBindex == 0x04)		// setting TB2
	{
		CAN1TFI3   = MsgBuf_TX1.Frame & 0xC00F0000;    //缓冲区TBS3
		CAN1TID3  = MsgBuf_TX1.MsgID;
		CAN1TDA3 = MsgBuf_TX1.DataA;
		CAN1TDB3 = MsgBuf_TX1.DataB;
		CAN1CMR = 0x8B;//单次发送0X9A;//单次自发自收//0x81;                                   //发送请求，选择TBS3 发送 
	}*/

 	uw16_RevStat &= CLR_CAN_XMT_ERR;
	
	uc_ArBitrationFlag=1;
	if(uc_Arbitration !=1)
	    uw16_ArbitrationCounter = 0;
      
 	return 0;
}	//End of Can_Tx_Msg

/******************************************************************************
** Function name:		Recv_Routine
**
** Descriptions:		CAN接收程序
**                               
** parameters:		uw32_ID    uc_bytes_no    datbuf
**                                      
** Returned value:	finish receiving current message: return 0; unnecessary resend: return 1; 
**                    not first Message when receiving new groups: return 2; no vacant buffer: return 3;
** 
** Created Date:      2009/12/01     
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
unsigned char CANFunc::Recv_Routine(UWord32 uw32_ID, unsigned char uc_bytes_no, unsigned char *datbuf)
{
// 对不同一包中的数据进行缓存，以便以后对同一包中数据排队去帧号组成长链
// uw32_ID is the Identifier of received messages, uc_bytes_no is the number of data in this message, and *datbuf is the buffer where data in message stores.
// finish receiving current message: return 0; unnecessary resend: return 1; not first Message when receiving new groups: return 2; no vacant buffer: return 3;
	unsigned char uc_temp = 0;
	unsigned char uc_i, uc_j;
	unsigned char uc_Multi;
	unsigned char uc_G;
	unsigned char uc_s;
	unsigned char uc_SID;
	UWord16 uw16_M_Index;
	UWord16 uw16_temp;
	
	for(uc_i = 0; uc_i < CAN_BUF_NO; uc_i ++)
	{
		if(uc_s_Complete_Flg[uc_i] == 1)	// checking 'Time Out' Buffers, and now, Time Out doesn't trigger any feedback
		{
			//if(((Ctrl_Timer1ms::m_uw16TimeStampNo - uw16_CAN_s_Buf_TimeStampNo[uc_i]) * 0xFFFFFFFF + Ctrl_Timer1ms::m_LastStamp - uw32_CAN_s_Buf_TimeStamp[uc_i]) > CAN_TIMEOUT)
			if((OS_ServeProcess::OS_Time.Us100 - uw16_CAN_s_Buf_TimeStamp[uc_i]) > CAN_TIMEOUT)
			{
				uc_s_Complete_Flg[uc_i] = 0;       //超时500毫秒，存起超时时间
				//uw16_CAN_s_Buf_TimeStampNo[uc_i] = Ctrl_Timer1ms::m_uw16TimeStampNo;
				uw16_CAN_s_Buf_TimeStamp[uc_i] = OS_ServeProcess::OS_Time.Us100;
			}
		}
	}
	
	uc_Multi = (uw32_ID << 24) >> 31;	// get multi-message bit      多帧位
	uc_G = (uw32_ID << 7) >> 28; 		// get GrpID;        组标识符
	uc_SID = (uw32_ID << 17) >> 25;		// get init source ID    源标识符

	// if it's a resend message(same GrpID and same SourceID) that has not been processed, discard it. no matter single / multi
	uc_temp = 0;									// set a temp flag
	for(uc_i = 0; uc_i < CAN_BUF_NO; uc_i ++)
	{
		if((uc_s_SourceID[uc_i] == uc_SID) && (uc_s_GrpID[uc_i] == uc_G) && (uc_s_Complete_Flg[uc_i] == 2))
		{
			uc_temp = 1;      //判断是否是上位机重发的帧
		}
	}
	if(uc_temp == 1)								// if it's a resend message that has not been processed.
	{
		//todo: give a report to sender that it's a unnecessary resend.
		return 1;
	} // end dealing resent message.

	// if it is a single frame message. find a vacant buffer and put it into it. single message doesn't need a TIMEOUT
	if(uc_Multi == 0)	// if single			单帧处理		
	{
	        //timer_counter0=0;      //测试组包时间
		// get a vacant buffer to save. if not available, quit.
		uc_s = 99;
		for(uc_i = 0; uc_i < CAN_BUF_NO; uc_i ++)	// find a vacant buffer
		{
			if(uc_s_Complete_Flg[uc_i] == 0)        //未填数据为空标志
			{
				uc_s = uc_i;
				break;
			}
		}
		if(uc_s == 99)	// if no vacant buffer available
		{
			return 3;	// return...discard??
		}
		else			// if vacant buffer available, set data in to buffer [uc_s]
		{
			for(uc_i = 0; uc_i < uc_bytes_no; uc_i ++)
			{
				uc_s_Buf[uc_s][uc_i] = datbuf[uc_i];
			}
			uc_s_GrpID[uc_s] = uc_G;
			uc_s_SourceID[uc_s] = uc_SID;
			uc_s_Complete_Flg[uc_s] = 2;        //填完成数据标志
			uc_s_BytesNoInBuf[uc_s] = uc_bytes_no;  // 帧中实际数据个数

			return 0;
		}
	} // end receiving single frame message
	
	// if it is a multi-frame message, 1. see if it's a one in receiving. 2. start a new receiving process  多帧处理
	else				
	{
		// see if it's the one under receiving. if uc_s = 0 - 4, then under receiving. if uc_s = 99. not in receiving.
		//?? the situation that ucxxxCompeletexxx = 2 has been coped in former code and lead to a 'return', so, only 0 and 1 remains.
		uc_s = 99;
		for(uc_i = 0; uc_i < CAN_BUF_NO; uc_i ++)
		{
			if((uc_s_SourceID[uc_i] == uc_SID) && (uc_s_GrpID[uc_i] == uc_G) && (uc_s_Complete_Flg[uc_i] == 1))
			{
				uc_s = uc_i;      //判断是否是5包中同一包中的数据
			}
		}
		// get the index of message.第几帧号码从0开始算
		uw16_M_Index = datbuf[0];
		uw16_M_Index = (uw16_M_Index << 8) | datbuf[1];
		
		//if(uw16_M_Index==0)
			//timer_counter0=0;      //测试组包时间
		
		// process under different situation.
		// in receiving process, in buffer[uc_s]
		if(uc_s < CAN_BUF_NO)             //对旧包的处理
		{
			uw16_temp = uw16_M_Index * 6 - 2;       
			//一帧传6个字节数据，第一帧少2个节字发总帧数
			
			uc_j = 6;							// if not last message, real data length always be 6=(8-2)
			if(uw16_M_Index + 1 == uc_s_M_No[uc_s])	// if it's the last message, then might be different real data length
			{
				uc_j = uc_bytes_no;   // 最后一帧可能不满6个字节
			        // based on:  uc_s_BytesNoInBuf[uc_s] = uc_s_M_No[uc_s] * 6 - 2 - (8 - uc_bytes_no);
				uc_s_BytesNoInBuf[uc_s] = uc_s_M_No[uc_s] * 6 - 10 + uc_bytes_no;	
			}
			// set data
			for(uc_i = 0; uc_i < uc_j; uc_i ++)
			{
				uc_s_Buf[uc_s][uw16_temp + uc_i] = datbuf[2 + uc_i];
			}
			
			// set the corresponding 'receving complete index'      // 1个字节8个位组成，1帧对应1位标志
			uc_i = uw16_M_Index >> 3; 	//   uc_i = uw16_M_Index / 8;	
			uc_j = uw16_M_Index & 0x0007;	// mod 8      计算帧索引1帧1个标志
			uc_s_M_R[uc_s][uc_i] = uc_s_M_R[uc_s][uc_i] | (0x01 << uc_j);   // 置接收标志
			
			// Check if Complete Receiving.        检查包是否完整接收
			uc_j = 1;
			for(uc_i = 0; uc_i < CAN_BUF_INDEX_LEN; uc_i ++)
			{
				if(uc_s_M_R[uc_s][uc_i] != 0xFF)
				{
					uc_j = 0;
				}
			}
			if(uc_j)	// if complete Receiving.
			{
				uc_s_Complete_Flg[uc_s] = 2; //填完成数据标志
			}
			return 0;
		} // ending in receiving process, in short buffer
		//not in receiving process, should start a new process, if not the first message in the group, discard it.  没有包第一  帧处理
		if(uc_s == 99)            //对新收到的包处理
		{
			if(uw16_M_Index != 0)				// if not the first message after complete the last one, return.
			{
				return 2;
			}
			// find a vacant short buffers, 
			uc_s = 99;
			for(uc_i = 0; uc_i < CAN_BUF_NO; uc_i ++)
			{
				if(uc_s_Complete_Flg[uc_i] == 0)
				{
					uc_s = uc_i;
					break;
				}
			}
			if(uc_s == 99)					// uc_s = 99 means no vacant
			{
			
				return 3;	
			}

			uc_s_Complete_Flg[uc_s] = 1;        //正在填数据中标志
			uc_s_GrpID[uc_s] = uc_G;			// set new Group ID
			uc_s_SourceID[uc_s] = uc_SID;		// set init source ID
			
			// set the TIMEOUT init Value      超时存时间
			//uw16_CAN_s_Buf_TimeStampNo[uc_s] = Ctrl_Timer1ms::m_uw16TimeStampNo;
			uw16_CAN_s_Buf_TimeStamp[uc_s] = OS_ServeProcess::OS_Time.Us100;

			// Get Number of messages in the group.  取出总共要传的帧数
			uc_s_M_No[uc_s] = datbuf[2];		
			uc_s_M_No[uc_s] = (uc_s_M_No[uc_s] << 8) | datbuf[3];

			// init Message Register    计算帧索引1帧1个标置
			uc_i = uc_s_M_No[uc_s] >> 3;	//uc_i = uc_s_M_No[uc_s] / 8; get uc_i: number of Bytes and uc_j: number of bits
			uc_j = uc_s_M_No[uc_s] & 0x07;		// mod 8
				// 1. set index to all 1.
			for(uc_temp = 0; uc_temp < CAN_BUF_INDEX_LEN; uc_temp ++)	
			{
				uc_s_M_R[uc_s][uc_temp] = 0xFF;
			}
				// 2. set corresponding byte to 0
			for(uc_temp = 0; uc_temp < uc_i; uc_temp ++)	
			{
				uc_s_M_R[uc_s][uc_temp] = 0;
			}
				// 3. set the rest bits in the last corresponding byte to 0.
			uc_s_M_R[uc_s][uc_i] = (uc_s_M_R[uc_s][uc_i] >> uc_j) << uc_j; 

			// get the first message's content
			for(uc_temp = 0; uc_temp < 4; uc_temp ++)	
			{
				uc_s_Buf[uc_s][uc_temp] = datbuf[uc_temp + 4];    
			        //包中的第一帧帧号占2字节，总帧数占2字节
			}
			// set the corresponding index
			uc_s_M_R[uc_s][0] = uc_s_M_R[uc_s][0] | 0x01;
			return 0;
		}// ending start a new process
		return 0;
	}// ending multi-receiving part.
}

/******************************************************************************
** Function name:        IVCH_CmdParse
**
** Descriptions:         IV通道的命令解析
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::IVCH_CmdParse(void)				// 命令解析模块主体 
{
    //命令解析模块主函数
	UWord16 uw16_i = 0;
    unsigned char uc_i;
	unsigned char uc_IVCH_CmdBufFlg;

    uc_IVCH_CmdBufFlg = 0;
    for(uc_i = 0; uc_i < 5; uc_i ++)	// 检查通信缓存，如有接受完整进程，转存到命令缓存
    {
        if(uc_s_Complete_Flg[uc_i] == 2)
        {
            for(uw16_i = 0; uw16_i < uc_s_BytesNoInBuf[uc_i]; uw16_i ++)
            {
                uc_RxBuf[uw16_i] = uc_s_Buf[uc_i][uw16_i];
            }

            w16_RxB_DL = (UWord16)uc_s_BytesNoInBuf[uc_i] - 1;
            uc_IVCH_CmdBuf_SID = uc_s_SourceID[uc_i];	// 存信息来源的Can节点id
            uc_s_Complete_Flg[uc_i] = 0;
            uc_IVCH_CmdBufFlg = 1;
            break;	// 找到第一个接受完毕的通信，则跳出处理，余下的下一循环处理
        }
    }

    if(uc_IVCH_CmdBufFlg == 0)			// 如通信缓存空，退出。
    {
        return;
    }
    //IVCH_CmdChecksum();					// 否则校验此通信内容
    //Communicate_Processing::IVCH_CmdClassify();					// 如通过校验，则进行识别 
    //EthernetParse::CmdParse();
}

/******************************************************************************
** Function name:        IVCH_CmdChecksum
**
** Descriptions:         校验通信内容
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void CANFunc::IVCH_CmdChecksum(void)// 校验通信内容，如不通过，则清零uc_IVCHBufFlg标志
{
    //校验通信内容
	unsigned char uc_Idx = 0;
	//unsigned char uc_IVCH_CmdBufFlg;
    UWord16 uw16_CAN_CheckSum = 0;
	
    for(uc_Idx = 0; uc_Idx < w16_RxB_DL - 1; uc_Idx ++)// 加和除2校验位的所有字节
    {
        uw16_CAN_CheckSum += uc_RxBuf[uc_Idx];
    }
    if((uw16_CAN_CheckSum & 0x00FF) != uc_RxBuf[w16_RxB_DL])	// 低8位
    {
        //uc_IVCH_CmdBufFlg = 0;
        return;
    }
    if((uw16_CAN_CheckSum >> 8) != uc_RxBuf[w16_RxB_DL - 1])	// 高8位
    {
        //uc_IVCH_CmdBufFlg = 0;
        return;
    }
}

//<<End  CAN 封闭式函数>>


/******************************************************************************
**                            End Of File
******************************************************************************/
