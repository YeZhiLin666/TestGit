/*****************************************************************************
// 程序文件      :CAN.h     主要用于CAN硬件驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#ifndef __CAN_H_ 
#define __CAN_H_


//----CAN----------------------------------------------------------------------------
//CAN部分，保留，一般不用修改
#define CAN_BUF_NO				5
#define CAN_BUF_LEN				200
#define CAN_BUF_INDEX_LEN		5
//----CAN----------------------------------------------------------------------------

// CAN接收超时长度，单位毫秒
#define CAN_TIMEOUT				5000 //	500

#define ACCEPTANCE_FILTER_ENABLED	1

#define CAN_MEM_BASE		0xE0038000

#define MAX_PORTS	2		/* Number of CAN port on the chip */		

/* BRP+1 = Fpclk/(CANBitRate * QUANTAValue)
   QUANTAValue = 1 + (Tseg1+1) + (Tseg2+1)
   QUANTA value varies based on the Fpclk and sample point
   e.g. (1) sample point is 87.5%, Fpclk is 48Mhz
   the QUANTA should be 16
        (2) sample point is 90%, Fpclk is 12.5Mhz
   the QUANTA should be 10 
   Fpclk = Fclk /APBDIV
   or
   BitRate = Fcclk/(APBDIV * (BRP+1) * ((Tseg1+1)+(Tseg2+1)+1))
*/ 	
/* Here are some popular bit timing settings for LPC2468, google on "SJA1000"
CAN bit timing, the same IP used inside LPC2000 CAN controller. There are several 
bit timing calculators on the internet. 
http://www.port.de/engl/canprod/sv_req_form.html
*/

/* When Fcclk is 50Mhz and 60Mhz and APBDIV is 4,
so Fpclk is 12.5Mhz and 15Mhz respectively. 
Common CAN bit rates for 12.5Mhz(50Mhz CCLK) clock frequency */
//when Fpclk is 12.5Mhz, QUANTA is 10 and sample point is 90% 
#define BITRATE125K12_5MHZ		0x00070009     //*12.5Mhz  www.port.de
#define BITRATE250K12_5MHZ		0x00070004      //*12.5Mhz  www.port.de
//when Fpclk is 12.5Mhz, QUANTA is 25 and sample point is 68% 
#define BITRATE500K12_5MHZ		0x007f0000       //*12.5Mhz  www.port.de



/* When Fcclk is 50Mhz and APBDIV is 2,
so Fpclk is 25Mhz  respectively. 
Common CAN bit rates for 25Mhz(50Mhz CCLK) clock frequency */
//when Fpclk is 25Mhz, sample point is 80% 
#define BITRATE125K25MHZ		0x002F0009      //  *25M 周立功提供Samples 1     BTL  20   SJW  1    Sampling   85.0%
#define BITRATE250K25MHZ		0x002F0004      //  *25M 周立功提供Samples 1     BTL  20   SJW  1    Sampling   85.0%
#define BITRATE500K25MHZ		0x00160004      //  *25M 周立功提供Samples 1     BTL  20   SJW  1    Sampling   85.0%
#define BITRATE1M25MHZ		    0x00020004      //  *25M 周立功提供Samples      BTL    SJW      Sampling  

/* When Fcclk is 60Mhz and APBDIV is 2,
so Fpclk is 30Mhz  respectively. 
Common CAN bit rates for 30Mhz(60Mhz CCLK) clock frequency */
//when Fpclk is 30Mhz, sample point is 83.3% 
#define BITRATE125K30MHZ		0x00830027  
#define BITRATE250K30MHZ		0x00830013
#define BITRATE500K30MHZ		0x00890009
#define BITRATE1M30MHZ		    0x00830004


/* When Fcclk is 72Mhz and APBDIV is 2,
so Fpclk is 36Mhz  respectively. 
Common CAN bit rates for 36Mhz(72Mhz CCLK) clock frequency */
//when Fpclk is 36Mhz, sample point is 83.3% 
#define BITRATE125K36MHZ		0x0083002F     
#define BITRATE250K36MHZ		0x00830017
#define BITRATE500K36MHZ		0x0083000B
#define BITRATE1M36MHZ		    0x00830005


/* When Fcclk is 72Mhz and APBDIV is 4,
so Fpclk is 18Mhz  respectively. 
Common CAN bit rates for 18Mhz(72Mhz CCLK) clock frequency */
//when Fpclk is 18Mhz, sample point is 83.3% 
#define BITRATE125K18MHZ		0x00850011         //18M 波特率计算器TSEG1/2   3,0   采样点83.3%  三次采样 J  0 符合TS2>=SJ
#define BITRATE250K18MHZ		0x0083000B        //18M 波特率计算器TSEG1/2   3,0   采样点83.3%  三次采样  J  0 符合TS2>=SJ
#define BITRATE500K18MHZ	    0x00830005       //18M 波特率计算器TSEG1/2   3,0   采样点83.3%  三次采样J  0   符合TS2>=SJ
#define BITRATE1M18MHZ		    0x00830002       //18M 波特率计算器TSEG1/2   3,0   采样点83.3%  三次采样  J  0 符合TS2>=SJ

/* When Fpclk is 60Mhz  respectively. 
Common CAN bit rates for 60Mhz clock frequency */
//when Fpclk is 60Mhz, sample point is 83.3% 
#define BITRATE125K60MHZ		0x0085003B          //60M 波特率计算器TSEG1/2   5,0    
#define BITRATE250K60MHZ		0x00830027          //60M 波特率计算器TSEG1/2   3,0   
#define BITRATE500K60MHZ	    0x0083000D        //60M 波特率计算器TSEG1/2   3,0  
#define BITRATE1M60MHZ		    0x00830006        //60M 波特率计算器TSEG1/2   3,0  
//注意使用周立功CAN盒时，对应设置表如下：
//   CAN波特率			 R0          R1     
//     125K               0X03      0X1C      
//     250K               0X01      0X1C
//     500K               0X00      0X1C
//      1M                0X00      0X14


/* Acceptance filter mode in AFMR register */
#define ACCF_OFF				0x01
#define ACCF_BYPASS			    0x02          //旁路模式
#define ACCF_ON				    0x00
#define ACCF_FULLCAN			0x04         //全局滤波模式

/* This number applies to all FULLCAN IDs, explicit STD IDs, group STD IDs, 
explicit EXT IDs, and group EXT IDs. */ 
#define ACCF_IDEN_NUM			4

/* Identifiers for FULLCAN, EXP STD, GRP STD, EXP EXT, GRP EXT */
#define FULLCAN_ID				0x100
#define EXP_STD_ID				0x100
#define GRP_STD_ID				0x200
#define EXP_EXT_ID				0x100000
#define GRP_EXT_ID				0x200000

// Error status words: 
//    -bits 0-7 are current errors
//    -bits 8-15 are corresponding latched errors
#define CAN_XMT_ERR             0x0101
#define CLR_CAN_XMT_ERR         0xFFFE
#define CAN_XMT_TOUT            0x0202
#define CLR_CAN_XMT_TOUT        0xFFFD
#define CAN_RCV_ERR             0x0404
#define CLR_CAN_RCV_ERR         0xFFFB
#define DSP_SYS_ERR             0x0808


/*
// Type definition to hold a CAN message
typedef struct
{
	UWord32 Frame; 	// Bits 16..19: DLC - Data Length Counter      0..9ID索引
					// Bit 30: Set if this is a RTR message
					// Bit 31: Set if this is a 29-bit ID message
	UWord32 MsgID;	// CAN Message ID (11-bit or 29-bit)
	UWord32 DataA;	// CAN Message Data Bytes 0-3
	UWord32 DataB;	// CAN Message Data Bytes 4-7
} CAN_MSG;


extern CAN_MSG MsgBuf_TX1, MsgBuf_TX2; // TX and RX Buffers for CAN message
//extern CAN_MSG MsgBuf_RX1, MsgBuf_RX2; // TX and RX Buffers for CAN message
//extern UWord32 CAN1RxDone, CAN2RxDone;
extern UWord32 CAN1RxCount , CAN2RxCount ;
*/
/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
/*
//<<Begin   CAN  开放式接口界面函数>>
void Can_Init(void);
void CAN1_Init(unsigned char uc_brate);
void CAN2_Init(unsigned char uc_brate);
void CAN_SetACCF( UWord32 ACCFMode );
unsigned char Can_Tx_Msgs(unsigned char uc_no, unsigned char uc_DesAddr);
                 // sending the msgs, unpack them into several msgs. 
		// uc_no is the number of bytes to be sent. uc_DesAddr is the destination address.
		// if uc_no <= 8, send one message, if uc_no > 8, send messages.
void Can_Recv_Data(void);
void Can_Packup(void);
//<<End      CAN  开放式接口界面函数>>


//<<Begin   CAN 封闭式函数>>
//void CAN_ISR_Rx1( void );
//void CAN_ISR_Rx2( void );
void CAN_SetACCF_Lookup( void );
//UWord32 CAN1_SendMessage( CAN_MSG *pTxBuf );
//UWord32 CAN2_SendMessage( CAN_MSG *pTxBuf );
void clear_msg_objects(void);
//unsigned char  Can_Tx_Msg(UWord32 uw32_ID, unsigned char uc_msglen, unsigned char *uc_datbuf, unsigned char c_MsgNum);
unsigned char  Can_Tx_Msg(void);
unsigned char Recv_Routine(UWord32 uw32_ID, unsigned char uc_bytes_no, unsigned char *datbuf);
void CAN_Handler(void) __irq;
//<<End  CAN 封闭式函数>>  */

	typedef struct
	{
		UWord32 Frame; 	// Bits 16..19: DLC - Data Length Counter      0..9ID索引
						// Bit 30: Set if this is a RTR message
						// Bit 31: Set if this is a 29-bit ID message
		UWord32 MsgID;	// CAN Message ID (11-bit or 29-bit)
		UWord32 DataA;	// CAN Message Data Bytes 0-3
		UWord32 DataB;	// CAN Message Data Bytes 4-7
	} CAN_MSG;

    struct RxBuffers 						// 这里51和DSP全然不同。  帧数据
    {
        UWord32       uw32_ID;
        unsigned char uc_len;
        unsigned char uc_datbuf[8];
        unsigned char uc_valid;				// 初始化时全部置0. 1表示有数据
    };

	struct UW32
	{
		unsigned char low,midl,midh,hig; 
	}; 
	union  UW32DATA
	{
	        struct UW32 chardata;
	        UWord32      value ;
    };

	struct TxBuffers 
	{
		UWord32       uw32_ID;
		unsigned char uc_len;
		unsigned char datbuf[8];
		unsigned char uc_FirstFrame;     // 最先一帧标志
	};

class  CANFunc
{
//Attribute
public:
	static CAN_MSG MsgBuf_TX1; 
	static CAN_MSG MsgBuf_TX2; // TX and RX Buffers for CAN message
	//CAN_MSG MsgBuf_RX1, MsgBuf_RX2; // TX and RX Buffers for CAN message
	//UWord32 CAN1RxDone, CAN2RxDone;
	static UWord32 CAN1RxCount;
	static UWord32 CAN2RxCount;
	static UWord32 CANStatus;
	static UWord32 CAN1ErrCount;
	static UWord32 CAN2ErrCount;
	static unsigned char  c_pointer;
	//UWord32 dummy;
	static UWord32 * pDest;
// 设置变量定义在外存

#pragma arm section rwdata = "SRAM",zidata = "SRAM"		  // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01E84800
// dynamic memory signals: EMC_DYCS0; EMC_CAS; EMC_RAS; EMC_CKE0; EMC_DQM0; EMC_CLKOUT1; EMC_CLKOUT0;   0XA0000000  0x01E84800 //

	static unsigned short uw16_RevStat;			                // receving status, receiving buffer pointer
    static unsigned char  uc_s_Buf[CAN_BUF_NO][CAN_BUF_LEN];		// short buffer array   5包缓存帧数据
    static unsigned char  uc_s_Complete_Flg[CAN_BUF_NO];			// short buffer's flags of : filling/filled/been processed(empty). 1/2/0    现在5包状态
    static unsigned char  uc_s_M_R[CAN_BUF_NO][CAN_BUF_INDEX_LEN];	// short buffer's index of received messages.    包索引
    static unsigned char  uc_s_SourceID[CAN_BUF_NO];				// short buffer's message group's source ID            包源ID
    static unsigned char  uc_s_GrpID[CAN_BUF_NO];					// short buffer's message group's group ID      包组号
    static unsigned char  uc_s_BytesNoInBuf[CAN_BUF_NO];			// the no. of message-bytes in short buffers.            包中总共字节数含包头和校验
    static unsigned char  uc_s_M_No[CAN_BUF_NO];					// the no. of messages of the group being received in short buffers.
    //static UWord16        uw16_CAN_s_Buf_TimeStampNo[CAN_BUF_NO];
    static UWord16        uw16_CAN_s_Buf_TimeStamp[CAN_BUF_NO];

	static RxBuffers str_RxBuf[64];						// 16 str_TxBuf can send about 94 bytes.      

	static UW32DATA uw32_CANtemp;
    //unsigned short *uc_can_xdata_ptr;		// tx buffer pointer
    static unsigned char uc_TxBuf[100];   //[300];       //帧数据发送缓冲
	static unsigned char uc_RxBuf[100];   //[300];       //帧数据接收缓冲
	static UWord16       uw16_S_gID;                  //帧索引号

	static TxBuffers str_TxBuf[64];
	static signed short    w16_TxB_SP;	// TxBuffers StartPoint, TxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_TxBuf[uc_TxB_SP]
	static signed short    w16_TxB_DL;	// TxBuffers StartPoint, TxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_TxBuf[uc_TxB_SP]
    static unsigned char uc_flg_TxIdle;		    // if Tx buffer is Idle.//  0表示发送中断处于开始状态

    static signed short    w16_RxB_SP;	// RxBuffers StartPoint, RxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_RxBuf[uc_RxB_SP]
	static signed short    w16_RxB_DL;	// RxBuffers StartPoint, RxBuffers DataLength, DL=0, no data; DL = 1, 1 data in str_RxBuf[uc_RxB_SP]
    static unsigned char uc_flg_RxIdle; // if Rx buffer is Idle.//  0表示接收中断处于开始状态

	static unsigned char uc_Arbitration;
	static unsigned char uc_BEIEERR;
	static unsigned char uc_EIEERR;
	static unsigned char uc_ArBitrationFlag;
	static UWord32       uw32_TBindex;
	static UWord16       uw16_ArbitrationCounter; 
	static unsigned char uc_IVCH_CmdBuf_SID;						// where the command comes from, source dspid
#pragma arm section //结束外存变量定义

//private:

//protected:


//Operation
public:
	static void Can_Init(void);
	static void CAN1_Init(unsigned char uc_brate);
	static void CAN2_Init(unsigned char uc_brate);
	static void CAN_SetACCF( UWord32 ACCFMode );
	static unsigned char Can_Tx_Msgs(unsigned char uc_no, unsigned char uc_DesAddr);
	// sending the msgs, unpack them into several msgs. 
	// uc_no is the number of bytes to be sent. uc_DesAddr is the destination address.
	// if uc_no <= 8, send one message, if uc_no > 8, send messages.
	static void Can_Recv_Data(void);
	static void Can_Packup(void);
    static unsigned char  Can_Tx_Msg(void);

private:
	//void CAN_ISR_Rx1( void );
	//void CAN_ISR_Rx2( void );
	static void CAN_SetACCF_Lookup( void );
	//UWord32 CAN1_SendMessage( CAN_MSG *pTxBuf );
	//UWord32 CAN2_SendMessage( CAN_MSG *pTxBuf );
	static void clear_msg_objects(void);
	//unsigned char  Can_Tx_Msg(UWord32 uw32_ID, unsigned char uc_msglen, unsigned char *uc_datbuf, unsigned char c_MsgNum);
	//static unsigned char  Can_Tx_Msg(void);
	static unsigned char Recv_Routine(UWord32 uw32_ID, unsigned char uc_bytes_no, unsigned char *datbuf);

	static void IVCH_CmdParse(void);				// 命令解析模块主体 
	static void IVCH_CmdChecksum(void);			// 校验通信内容，如不通过，则清零uc_IVCHBufFlg标志

protected:
//	void CAN_Handler(void) __irq;
	
};

#endif	// __CAN_H

/******************************************************************************
**                            End Of File
******************************************************************************/

