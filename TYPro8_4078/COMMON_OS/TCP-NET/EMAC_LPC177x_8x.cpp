/*-----------------------------------------------------------------------------
*      RL-ARM - TCPnet
*-----------------------------------------------------------------------------
*      Name:    EMAC_LPC177x_8x.c
*      Purpose: Driver for NXP LPC177x_8x EMAC Ethernet Controller
*      Rev.:    V4.60
*-----------------------------------------------------------------------------
*      This code is part of the RealView Run-Time Library.
*      Copyright (c) 2004-2012 KEIL - An ARM Company. All rights reserved.
*----------------------------------------------------------------------------*/


#include "../../COMMON_os/Tcp-net/Net_Config.h"
#include "../../COMMON_os/Tcp-net/EMAC_LPC177x_8x.h"
#include "../../COMMON_os/LPC4078CORE/LPC407x_8x_177x_8x.h"
#include "../../COMMON_os/Data_Manipulation/Datatypes.h"
#include "../../COMMON_os/LPC4078CORE/Arbin_LPC4078.h"
#include "../../COMMON_os/LPC4078CORE/IRQ.h"
#include "../Entry/G_DF.h"
#include "includes.h"

#define     _10MBIT_                     0
#define     NET_SPEED_LIMIT              0
#define     _100MBIT_                    1

//#define HIGHEST_PRIORITY  0



//extern unsigned long install_irq( IRQn_Type IntNumber,unsigned long  Priority);

//U8  socket_udp = 0;
//U8  socket_udp_2 = 0;
/* The following macro definitions may be used to select the speed
of the physical link:

_10MBIT_   - connect at 10 MBit only
_100MBIT_  - connect at 100 MBit only

By default an autonegotiation of the link speed is used. This may take
longer to connect, but it works for 10MBit and 100MBit physical links.      */

#define _DP83848_

#ifdef _DP83848_
#define PHY_DEF_ADR    DP83848C_DEF_ADR
#define PHY_ID         DP83848C_ID
#else
#define PHY_DEF_ADR    KSZ8001_DEF_ADR
#define PHY_ID         KSZ8001_ID
#endif

/* Net_Config.c */
extern U8 own_hw_adr[];



/* EMAC local DMA Descriptors. */
static            RX_Desc Rx_Desc[NUM_RX_FRAG];
static __align(8) RX_Stat Rx_Stat[NUM_RX_FRAG]; /* Must be 8-Byte alligned    */
static            TX_Desc Tx_Desc[NUM_TX_FRAG];
static            TX_Stat Tx_Stat[NUM_TX_FRAG];

/* EMAC local DMA buffers. */
static U32 rx_buf[NUM_RX_FRAG][ETH_FRAG_SIZE>>2];
static U32 tx_buf[NUM_TX_FRAG][ETH_FRAG_SIZE>>2];

/*-----------------------------------------------------------------------------
*      EMAC Ethernet Driver Functions
*-----------------------------------------------------------------------------
*  Required functions for Ethernet driver module:
*  a. Polling mode: - void init_ethernet ()
*                   - void send_frame (OS_FRAME *frame)
*                   - void poll_ethernet (void)
*  b. Interrupt mode: - void init_ethernet ()
*                     - void send_frame (OS_FRAME *frame)
*                     - void int_enable_eth ()
*                     - void int_disable_eth ()
*                     - interrupt function
*----------------------------------------------------------------------------*/
//ICMP
U32 MyReceiveTime;
REMOTEM rm;
//unsigned short TimeCaliFlag;
/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);
static void write_PHY (U32 PhyReg, U16 Value);
static U16  read_PHY  (U32 PhyReg);
U32 CalCRC(unsigned char * p, unsigned char len);
static U32 ArchSwap32(U32 D);
static unsigned short checksum_ICMP(unsigned short *buffer,int size);

U32 uw32_TestSumRevice;
U32 uw32_TestSumTime;

extern LOCALM nlocalm[3];
extern U8  uc_remotemIpAdr[4];

unsigned char m_NetLinkFlag;   // 网络连接成功标志

typedef void (*My_Func_Ptr_Void)(void);
static My_Func_Ptr_Void   Func_SendNet;
uint8 	       uc_UDP_SendLock;


void Send_ICMP_TimeRequest(void)
{
    /* Send frame to EMAC ethernet controller */
    U32 idx, ipv;
    U32 *dp;

    //comment out chen
    U32 ICMP_Send_Time = 0;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    unsigned short icmp[9];
    unsigned short icmp_check = 0;

    Hour = LPC_RTC->HOUR;
    Minute = LPC_RTC->MIN;
    Second = LPC_RTC->SEC;
    //ICMP_Send_Time = OS_ServeProcess::OS_Time.Hour*60*60*1000 + OS_ServeProcess::OS_Time.Minute*60*1000 + OS_ServeProcess::OS_Time.Second*1000 + OS_ServeProcess::OS_Time.Us100/10;    //不会>86400000
    ICMP_Send_Time = Hour*60*60*1000 + Minute*60*1000 + Second*1000;    //不会>86400000
    ICMP_Send_Time = ArchSwap32(ICMP_Send_Time);

    ipv = own_hw_adr[5];
    idx = LPC_EMAC->TxProduceIndex;
    dp  = (U32 *)Tx_Desc[idx].Packet;


    icmp[0] = 0x000D;
    icmp[1] = 0x0003;
    icmp[2] = 0x0004;
    icmp[3] = (unsigned short)(ICMP_Send_Time);
    icmp[4] = (unsigned short)(ICMP_Send_Time>>16);
    icmp[5] = 0x0000;
    icmp[6] = 0x0000;
    icmp[7] = 0x0000;
    icmp[8] = 0x0000;
    icmp_check = checksum_ICMP(&icmp[0],18);    //增加了发送下位机时间戳，这样就能从抓包软件内很好的判断


    *dp++ = rm.HwAdr[0] | rm.HwAdr[1] << 8 | rm.HwAdr[2] << 16 | rm.HwAdr[3] << 24;  // Dst EMAC
    *dp++ = rm.HwAdr[4] | rm.HwAdr[5] << 8 | own_hw_adr[0] << 16| own_hw_adr[1] << 24;
    *dp++ = own_hw_adr[2] | own_hw_adr[3] << 8| own_hw_adr[4] << 16| ipv << 24; // Src EMAC
    *dp++ = 0x00450008;   //  [08 00]Type IP 0x0800; [45]Header length:20 bytes; [00]Services Codepoint:Default(0x00);
    *dp++ = 0x07F62800;   //  [00 28]Total Length:40; [F6 07]Identification:0xf607;
    *dp++ = 0x01800000;   //  [00 00]Fragment offset: 0; [80]Time to live 128; [01] ICMP(1);
    *dp++ = ((0x0000B918 - ipv) & 0x00FF) << 8 | (0x0000B918 - ipv) >> 8|nlocalm[0].IpAdr[0]<< 16| nlocalm[0].IpAdr[1] << 24;	// create checksum for this specific packet.
    *dp++ = nlocalm[0].IpAdr[2] | nlocalm[0].IpAdr[3] << 8| uc_remotemIpAdr[0] << 16 | uc_remotemIpAdr[1] << 24;				// adding source IP.
    *dp++ = 0x000D0000| uc_remotemIpAdr[2] | uc_remotemIpAdr[3] << 8;  // Destination IP; [0D]Control Message Type:13(Timestamp request); [00]Control Message code:0;
    //从0x000D开始就是ICMP  13代表时间戳请求  标识符+序号
    *dp++ = 0x00030000|(icmp_check&0x0000FFFF);  //FFEB;    //FFEB  check sum 错误    icmp_check&0x0000FFFF
    *dp++ = 0x00000004| ( (ICMP_Send_Time<<16)&0xFFFF0000);
    *dp++ = 0x00000000| ( (ICMP_Send_Time>>16)&0x0000FFFF);    //ArchSwap32(ICMP_Send_Time);//0x00000000;    //发送的原始时间戳
    *dp++ = 0x00000000;
    *dp = 0x00000000;

    Tx_Desc[idx].Ctrl = 54 | (TCTRL_INT | TCTRL_LAST);

    /* Start frame transmission. */
    if (++idx == NUM_TX_FRAG) idx = 0;
    LPC_EMAC->TxProduceIndex = idx;
}

/*--------------------------- init_ethernet ----------------------------------*/

void init_ethernet (void)
{
    /* Initialize the EMAC ethernet controller. */
    unsigned char hashvalue = 0;
    unsigned char dest[6];
    U32 regv,tout,id1,id2,rept;
    U32  CRCValue = 0;

    Func_SendNet=&MultiMcuComm::PackToNetLinkRoad;

    /* Power Up the EMAC controller. */
    Arbin_SC->PCONP.BitReg.bPCENET = 1;

    Arbin_IOCON->P1_0.BitReg.FUNC = 1;   /* P1.0  == ENET_TXD0    */
    Arbin_IOCON->P1_1.BitReg.FUNC = 1;   /* P1.1  == ENET_TXD1    */
    Arbin_IOCON->P1_4.BitReg.FUNC = 1;   /* P1.4  == nENET_TX_EN  */
    Arbin_IOCON->P1_8.BitReg.FUNC = 1;   /* P1.8  == ENET_CRS_DV  */
    Arbin_IOCON->P1_9.BitReg.FUNC = 1;   /* P1.9  == ENET_RXD0    */
    Arbin_IOCON->P1_10.BitReg.FUNC = 1;  /* P1.10 == ENET_RXD1    */
    Arbin_IOCON->P1_14.BitReg.FUNC = 1;  /* P1.14 == ENET_RX_ER   */
    Arbin_IOCON->P1_15.BitReg.FUNC = 1;  /* P1.15 == ENET_REF_CLK */
    Arbin_IOCON->P1_16.BitReg.FUNC = 1;  /* P1.16 == ENET_MDC     */
    Arbin_IOCON->P1_17.BitReg.FUNC = 1;  /* P1.17 == ENET_MDIO    */

    regv = LPC_EMAC->Module_ID;
    if(regv == OLD_EMAC_MODULE_ID)
        Arbin_IOCON->P1_6.BitReg.FUNC = 1;   // ENET_TX_CLK — Ethernet Transmit Clock (MII interface).
    else
        Arbin_IOCON->P1_6.BitReg.FUNC = 0;   //GPIO

    /* Reset all EMAC internal modules. */
    LPC_EMAC->MCFG = MCFG_RES_MII;
    LPC_EMAC->MAC1    = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX |
                        MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;   //CR_PASS_RUNT_FRM

    /* A short delay after reset. */
    for (tout = 1000; tout; tout--);

    /* Initialize MAC control registers. */
    LPC_EMAC->MAC1 = MAC1_PASS_ALL;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = ETH_MAX_FLEN;
    LPC_EMAC->CLRT = CLRT_DEF;
    LPC_EMAC->IPGR = IPGR_DEF_PART2|IPGR_DEF_PART1;
    for (tout = 100; tout; tout--);
    /* Enable Reduced MII interface. */
    LPC_EMAC->MCFG = MCFG_CLK_DIV36 | MCFG_RES_MII;   //1778 64

    LPC_EMAC->MCFG &= ~(MCFG_RES_MII);
    /* Enable Reduced MII interface. */
    LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;

    /* Reset Reduced MII Logic. */
    LPC_EMAC->SUPP = SUPP_RES_RMII | SUPP_SPEED;
    for (tout = 100; tout; tout--);
    LPC_EMAC->SUPP = SUPP_SPEED;


    rept = 10;
rpt:
    /* Put the PHY in reset mode */
    write_PHY (PHY_REG_BCR, PHY_BMCR_RESET);
    for (tout = 0; tout < 1000; tout++);

    /* Wait for hardware reset to end. */
    for (tout = 0; tout < 0x100000; tout++)
    {
        regv = read_PHY (PHY_REG_BCR);
        if (!(regv & 0x8800))
            break;   /* Reset complete, device not Power Down. */
    }

#if (NET_SPEED_LIMIT   == 1)			//限制了网速为10MHz
    // 在DP83848自动协商网速之前将DP83848网速能力限制为10M，这样协商的结果就只能是10M，而不会达到100M//
    regv  = read_PHY (PHY_REG_ANAR);
    regv &= ~((0x01<<7)|(0x01<<8)|(0x01<<9));   //将7,8，9三个BIT置为0,100M禁能。
    write_PHY (PHY_REG_ANAR, regv);

    for (tout = 0; tout < 0x100000; tout++)     // Wait for register reset ok. //
    {
        regv = read_PHY (PHY_REG_ANAR);
        if (!(regv & ((0x01<<7)|(0x01<<8)|(0x01<<9))))
            break;				  // OK //
    }
#endif


    /* Check if this is a DP83848 PHY. */
    id1 = read_PHY (PHY_REG_IDR1);
    id2 = read_PHY (PHY_REG_IDR2);
    if ((id1 == 0xFFFF) || (id2 == 0xFFFF))
    {   /* Chip Reset problem, retry several times. */
        for (tout = 0; tout < 0x100000; tout++);
        if (--rept) goto rpt;
    }
    if (((id1 << 16) | (id2 & 0xFFF0)) == PHY_ID)
    {   /* Configure the PHY device */
//#if defined (_10MBIT_)
#if (_10MBIT_ == 1)
        /* Connect at 10MBit */
        write_PHY (PHY_REG_BCR, PHY_FULLD_10M);
//#elif defined (_100MBIT_)
#elif (_100MBIT_ == 1)
        /* Connect at 100MBit */
        write_PHY (PHY_REG_BCR, PHY_FULLD_100M);
#else
        /* Use autonegotiation about the link speed. */
        write_PHY (PHY_REG_BCR, PHY_AUTO_NEG);
        /* Wait to complete Auto_Negotiation. */

        for (tout = 0; tout < 0x100000; tout++)
        {
            regv = read_PHY (PHY_REG_BSR);
            if (regv == 0xFFFF)
                continue;
            if (regv & 0x0020)
                break;/* Autonegotiation Complete. */
        }
#endif
    }

    // Check the link status. //
    for (tout = 0; tout < 0x20000; tout++)   //0x10000
    {
        regv = read_PHY (PHY_REG_STS);
        if (regv & LINK_VALID_STS)
        {
            m_NetLinkFlag  =  1;
            break;	         // Link is on. 网络已连上//
        }
    }

    // Configure Full/Half Duplex mode. 双工/半双工//
    if (regv & FULL_DUP_STS)
    {
        /* Full duplex is enabled. */
        LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
        LPC_EMAC->Command |= CR_FULL_DUP;
        LPC_EMAC->IPGT     = IPGT_FULL_DUP;
    }
    else
    {
        /* Half duplex mode. */
        LPC_EMAC->IPGT = IPGT_HALF_DUP;
    }

    /* Configure 100MBit/10MBit mode. */
    if (regv & SPEED_10M_STS)
    {
        // 10MBit mode. //
        LPC_EMAC->SUPP = 0;
        tout=read_PHY (0x18);
        write_PHY (0x18, tout|0x0020 |0x0004);
    }
    else
    {
        /* 100MBit mode. */
        LPC_EMAC->SUPP = SUPP_SPEED;
        tout=read_PHY (0x18);
        write_PHY (0x18, tout|0x0020 );
    }

    /* Set the Ethernet MAC Address registers */
    LPC_EMAC->SA0 = ((U32)own_hw_adr[5] << 8) | (U32)own_hw_adr[4];
    LPC_EMAC->SA1 = ((U32)own_hw_adr[3] << 8) | (U32)own_hw_adr[2];
    LPC_EMAC->SA2 = ((U32)own_hw_adr[1] << 8) | (U32)own_hw_adr[0];
    dest[0] = own_hw_adr[5];
    dest[1] = own_hw_adr[4];
    dest[2] = own_hw_adr[3];
    dest[3] = own_hw_adr[2];
    dest[4] = own_hw_adr[1];
    dest[5] =own_hw_adr[0];
    CRCValue = CalCRC(dest, 6);
    hashvalue = ((CRCValue & 0X1FFFFFFF) >> 23);
    /* Initialize Tx and Rx DMA Descriptors */
    rx_descr_init ();
    tx_descr_init ();

    /* Receive Broadcast, Multicast and Perfect Match Packets */
    //LPC_EMAC->RxFilterCtrl = RFC_MCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;
    LPC_EMAC->RxFilterCtrl = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;

    if(hashvalue <= 31)
    {
        LPC_EMAC->HashFilterL = 1 << hashvalue;
        LPC_EMAC->HashFilterH = 0;
    }
    else
    {
        hashvalue -= 32;
        LPC_EMAC->HashFilterH = 1 << hashvalue;
        LPC_EMAC->HashFilterL = 0;
    }
    /* Enable EMAC interrupts. */
    LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;
    //	LPC_EMAC->IntEnable = INT_RX_DONE;

    /* Reset all interrupts */
    LPC_EMAC->IntClear  = 0xFFFF;


    m_NetLinkFlag  =  1;
    /* Enable receive and transmit mode of MAC Ethernet core */
    LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1     |= MAC1_REC_EN;
    /* Configure VIC for EMAC interrupt. */
    if ( IRQFunc::install_irq( ENET_IRQn, HIGHEST_PRIORITY + 10) == FALSE )   //3
    {
        return ;
    }
}


/*--------------------------- int_enable_eth ---------------------------------*/

void int_enable_eth (void)
{
    /* Ethernet Interrupt Enable function. */
    NVIC_EnableIRQ(ENET_IRQn);
}


/*--------------------------- int_disable_eth --------------------------------*/

void int_disable_eth (void)
{
    /* Ethernet Interrupt Disable function. */
    NVIC_DisableIRQ(ENET_IRQn);
}


/*--------------------------- send_frame -------------------------------------*/

void send_frame (OS_FRAME *frame)
{
    /* Send frame to EMAC ethernet controller */
    U32 idx = 0;
    U32 len;
    U32 *sp,*dp;

    idx = LPC_EMAC->TxProduceIndex;
    sp  = (U32 *)&frame->data[0];
    dp  = (U32 *)Tx_Desc[idx].Packet;

    /* Copy frame data to EMAC packet buffers. */
    for (len = (frame->length + 3) >> 2; len; len--)
    {
        *dp++ = *sp++;
    }
    Tx_Desc[idx].Ctrl = (frame->length-1) | (TCTRL_INT | TCTRL_LAST);

    /* Start frame transmission. */
    if (++idx == NUM_TX_FRAG) idx = 0;
    LPC_EMAC->TxProduceIndex = idx;
}


/*--------------------------- interrupt_ethernet -----------------------------*/

void ENET_IRQHandler (void)
{
    /* EMAC Ethernet Controller Interrupt function. */
    U32 idx = 0;
    U32 int_stat,RxLen,info;
    U32 *sp,*dp;
    OS_FRAME *frame;

    //	U8 myi;


    while ((int_stat = (LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)) != 0)
    {
        LPC_EMAC->IntClear = int_stat;

        if (int_stat & INT_TX_DONE)
        {
            /* Frame transmit completed. */
            if((Func_SendNet != 0)&&(uc_UDP_SendLock == 0))
                (* Func_SendNet) ();
        }

        if (int_stat & INT_RX_DONE)
        {
            /* Packet received, check if packet is valid. */
            idx = LPC_EMAC->RxConsumeIndex;
            while (idx != LPC_EMAC->RxProduceIndex)
            {
                info = Rx_Stat[idx].Info;
                if (!(info & RINFO_LAST_FLAG))
                {
                    goto rel;
                }

                RxLen = (info & RINFO_SIZE) - 3;
                if (RxLen > ETH_MTU || (info & RINFO_ERR_MASK))
                {
                    /* Invalid frame, ignore it and free buffer. */
                    goto rel;
                }
                /* Flag 0x80000000 to skip sys_error() call when out of memory. */
                frame = alloc_mem (RxLen | 0x80000000);
                /* if 'alloc_mem()' has failed, ignore this packet. */
                //			myi = 0;
                if (frame != NULL)
                {
                    dp = (U32 *)&frame->data[0];
                    sp = (U32 *)Rx_Desc[idx].Packet;
                    for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--)
                    {
                        *dp++ = *sp++;
                    }
                    //ICMP 中断内处理
                    // Is the Packet a ICMP Timestamp reply packet? 0E: Time Reply Packet; 01: ICMP Packet; 0008: IP Packet.
//                     dp = (U32 *)&frame->data[0];
//                     if(((*(dp+8) & 0x00FF0000) == 0x000E0000) && ((*(dp+5) & 0xFF000000) == 0x01000000) && ((*(dp+3) & 0x0000FFFF) == 0x00000008))
//                     {   // if yes, retrieve "Receving Time" to MyReceiveTime.
//                         MyReceiveTime = ((*(dp+11) >> 16) << 24) | ((*(dp+11) >> 24) << 16) | ((*(dp+12) << 24) >> 16) | ((*(dp+12) << 16) >> 24);
                        //修改大小端问题 20130321
//                         MyReceiveTime = ArchSwap32(MyReceiveTime);


//                         //TimeCaliFlag = 1;    //获取对时信息
//                     }

#if DEBUG_TEST_COMMDATA_MODE == 1
                    // TCP:0x06 // Dst Port: 5679
                    if(((*(dp+5) & 0xFF000000) == 0x06000000) && ((*(dp+9) & 0x0000FFFF) == 0x00002F16))
                    {
                        ++ uw32_TestSumRevice; //  仅为测试用，成型后要清理丢。
                        if(uw32_TestSumTime >= 5)  //  50ms
                            uw32_TestSumRevice = 0;
                        uw32_TestSumTime = 0;
                        //myi = 0;
                    }
#endif
                    put_in_queue (frame);    //压入Fram到queue
                }
rel:
                if (++idx == NUM_RX_FRAG) idx = 0;
                /* Release frame from EMAC buffer. */
                LPC_EMAC->RxConsumeIndex = idx;
            }
            /* Frame transmit completed. */
        }
    }

}


/*--------------------------- rx_descr_init ----------------------------------*/

static void rx_descr_init (void)
{
    /* Initialize Receive Descriptor and Status array. */
    U32 i = 0;

    for (i = 0; i < NUM_RX_FRAG; i++)
    {
        Rx_Desc[i].Packet  = (U32)&rx_buf[i];
        Rx_Desc[i].Ctrl    = RCTRL_INT | (ETH_FRAG_SIZE-1);
        Rx_Stat[i].Info    = 0;
        Rx_Stat[i].HashCRC = 0;
    }

    /* Set EMAC Receive Descriptor Registers. */
    LPC_EMAC->RxDescriptor       = (U32)&Rx_Desc[0];
    LPC_EMAC->RxStatus           = (U32)&Rx_Stat[0];
    LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

    /* Rx Descriptors Point to 0 */
    LPC_EMAC->RxConsumeIndex  = 0;
}


/*--------------------------- tx_descr_init ---- -----------------------------*/

static void tx_descr_init (void)
{
    /* Initialize Transmit Descriptor and Status array. */
    U32 i = 0;

    for (i = 0; i < NUM_TX_FRAG; i++)
    {
        Tx_Desc[i].Packet = (U32)&tx_buf[i];
        Tx_Desc[i].Ctrl   = 0;
        Tx_Stat[i].Info   = 0;
    }

    /* Set EMAC Transmit Descriptor Registers. */
    LPC_EMAC->TxDescriptor       = (U32)&Tx_Desc[0];
    LPC_EMAC->TxStatus           = (U32)&Tx_Stat[0];
    LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

    /* Tx Descriptors Point to 0 */
    LPC_EMAC->TxProduceIndex  = 0;
}


/*--------------------------- write_PHY --------------------------------------*/

static void write_PHY (U32 PhyReg, U16 Value)
{
    /* Write a data 'Value' to PHY register 'PhyReg'. */
    U32 tout = 0;

    LPC_EMAC->MADR = PHY_DEF_ADR | PhyReg;
    LPC_EMAC->MWTD = Value;

    /* Wait utill operation completed */
    for (tout = 0; tout < MII_WR_TOUT; tout++)
    {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
        {
            break;
        }
    }
}


/*--------------------------- read_PHY ---------------------------------------*/

static U16 read_PHY (U32 PhyReg)
{
    /* Read a PHY register 'PhyReg'. */
    U32 tout = 0;
    U32 val;

    LPC_EMAC->MADR = PHY_DEF_ADR | PhyReg;
    LPC_EMAC->MCMD = MCMD_READ;

    /* Wait until operation completed */
    for (tout = 0; tout < MII_RD_TOUT; tout++)
    {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
        {
            break;
        }
    }
    LPC_EMAC->MCMD = 0;
    val = LPC_EMAC->MRDD;
    return (val);
}

/******************************************************************************
** Function name:        CalCRC
**
** Descriptions:         CRC校验
**
** parameters:           * p    len
**
** Returned value:       crc返回校验结果
**
** Created Date:         2009/12/01
**
** Revision Dates:

******************************************************************************/
U32 CalCRC(unsigned char * p, unsigned char len)
{
    //  CRC校验
    unsigned char i = 0; // 迭代子
    unsigned char j; // 另一个迭代子
    unsigned char byte; // 当前字节
    U32 q0, q1, q2, q3; // 临时变量
    U32 crc = 0; // CRC 结果

    crc = 0xFFFFFFFF;
    for(i = 0; i < len; i++)
    {
        byte = *p++;
        for(j = 0; j < 2; j++)
        {
            if(((crc >> 28) ^ (byte >> 3)) & 0x00000001)
            {
                q3 = 0x04C11DB7;
            }
            else
            {
                q3 = 0x00000000;
            }

            if(((crc >> 29) ^ (byte >> 2)) & 0x00000001)
            {
                q2 = 0x09823B6E;
            }
            else
            {
                q2 = 0x00000000;
            }
            if(((crc >> 30) ^ (byte >> 1)) & 0x00000001)
            {
                q1 = 0x130476DC;
            }
            else
            {
                q1 = 0x00000000;
            }
            if(((crc >> 31) ^ (byte >> 0)) & 0x00000001)
            {
                q0 = 0x2608EDB8;
            }
            else
            {
                q0 = 0x00000000;
            }

            crc = (crc << 4) ^ q3 ^ q2 ^ q1 ^ q0;
            byte >>= 4;
        }
    }
    return crc;
}

//大小端转换
static U32 ArchSwap32(U32 D)
{
    return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}

//
unsigned short checksum_ICMP(unsigned short *buffer,int size)
{
    unsigned long cksum=0;
    //将数据以字为单位累加到CKSUM中
    while(size>1)
    {
        cksum+=*buffer++;
        size-=sizeof(unsigned short);
    }
    //如果为奇数，将最后一个字节扩展到双字，再累加到cksum中
    if(size)
    {
        cksum+=*(unsigned char *)buffer;
    }
    //将cksum的高16位和低16位相加，取反后得到校验和
    cksum=(cksum>>16)+(cksum&0xffff);
    cksum+=(cksum>>16);
    return (unsigned short)(~cksum);
}

/*-----------------------------------------------------------------------------
* end of file
*----------------------------------------------------------------------------*/
