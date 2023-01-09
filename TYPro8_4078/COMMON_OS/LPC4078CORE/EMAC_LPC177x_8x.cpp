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

#include <Net_Config.h>
#include "EMAC_LPC177x_8x.h"
#include "../LPC4078CORE/LPC407x_8x_177x_8x.h"
//#include "../LPC4078CORE/system_LPC407x_8x_177x_8x.h"
//#include <LPC407x_8x_177x_8x.h>             /* LPC177x/8x definitions             */
#include "../../COMMON/Datatypes.h"
#include "../../HWD/LPC4078CORE/IRQ.h"
#define NET_SPEED_LIMIT   1
//#define HIGHEST_PRIORITY  0


U32 CalCRC(unsigned char * p, unsigned char len);
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
REMOTEM rm;
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

/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);
static void write_PHY (U32 PhyReg, U16 Value);
static U16  read_PHY  (U32 PhyReg);
static U32 ArchSwap32(U32 D);
static unsigned short checksum_ICMP(unsigned short *buffer,int size);



void send_ICMP_TimeRequest(void)
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

    *dp++ = rm.HwAdr[0] | rm.HwAdr[1] << 8 | rm.HwAdr[2] << 16 | rm.HwAdr[3] << 24;
    *dp++ = 0x03000000 | rm.HwAdr[4] | rm.HwAdr[5] << 8;
    *dp++ = 0x0004D547 | (ipv << 24);	// last 8 bits are Emac
    *dp++ = 0x00450008;
    *dp++ = 0x07F62800;
    *dp++ = 0x01800000;
    *dp++ = 0xA8C40000 | ((0x0000B918 - ipv) & 0x00FF) << 8 | (0x0000B918 - ipv) >> 8;	// create checksum for this specific packet.
    *dp++ = 0xA8C40001 | (ipv << 8);				// adding source IP.
    *dp++ = 0x000D6401;
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

void init_ethernet (void) {
    /* Initialize the EMAC ethernet controller. */
    U32 regv,tout,id1,id2;
    U32  CRCValue = 0;
    unsigned char hashvalue = 0;
    unsigned char dest[6];
    /* Power Up the EMAC controller. */
    // LPC_SC->PCONP |= (1U << 30);
    // Power Up the EMAC controller. //Set up power for Ethernet module //
    LPC_SC->PCONP |= PCENET;
    /* Configure Ethernet Pins. */
    LPC_IOCON->P1_0  =  1;  /* P1.0  == ENET_TXD0    */
    LPC_IOCON->P1_1  =  1;  /* P1.1  == ENET_TXD1    */
    LPC_IOCON->P1_4  =  1;  /* P1.4  == nENET_TX_EN  */
    LPC_IOCON->P1_8  =  1;  /* P1.8  == ENET_CRS_DV  */
    LPC_IOCON->P1_9  =  1;  /* P1.9  == ENET_RXD0    */
    LPC_IOCON->P1_10 =  1;  /* P1.10 == ENET_RXD1    */
    LPC_IOCON->P1_14 =  1;  /* P1.14 == ENET_RX_ER   */
    LPC_IOCON->P1_15 =  1;  /* P1.15 == ENET_REF_CLK */
    LPC_IOCON->P1_16 =  1;  /* P1.16 == ENET_MDC     */
    LPC_IOCON->P1_17 =  1;  /* P1.17 == ENET_MDIO    */

    regv = LPC_EMAC->Module_ID;
    if(regv == OLD_EMAC_MODULE_ID)
    {
        // On Rev. '-', MAC_MODULEID should be equal to OLD_EMAC_MODULE_ID, P1.6 should be set. //
        // selects P1[0,1,4,6,8,9,10,14,15] //
        //LPC_PINCON->PINSEL2 = 0X50151105;
        LPC_IOCON->P1_6 = 1;  // ENET_TX_CLK — Ethernet Transmit Clock (MII interface).
    }
    else
    {
        // on rev. 'A', MAC_MODULEID should not equal to OLD_EMAC_MODULE_ID, P1.6 should not be set. //
        // selects P1[0,1,4,8,9,10,14,15] //
        //LPC_PINCON->PINSEL2 = 0X50150105;
        LPC_IOCON->P1_6 = 0;  // ENET_TX_CLK — Ethernet Transmit Clock (MII interface).
    }
    /* Reset all EMAC internal modules. */
    LPC_EMAC->MCFG = MCFG_RES_MII;
    LPC_EMAC->MAC1    = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX |
                        MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;   //CR_PASS_RUNT_FRM

    /* A short delay after reset. */
    for (tout = 100; tout; tout--);

    /* Initialize MAC control registers. */
    LPC_EMAC->MAC1 = MAC1_PASS_ALL;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = ETH_MAX_FLEN;
    LPC_EMAC->CLRT = CLRT_DEF;
    // LPC_EMAC->IPGR = IPGR_DEF;             //comment in 2468
    LPC_EMAC->IPGR = IPGR_DEF_PART2|IPGR_DEF_PART1;
    for (tout = 100; tout; tout--);
    /* Enable Reduced MII interface. */
    LPC_EMAC->MCFG = MCFG_CLK_DIV36 | MCFG_RES_MII;   //1778 64

    LPC_EMAC->MCFG &= ~(MCFG_RES_MII);
    /* Enable Reduced MII interface. */
    LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;

    /* Reset Reduced MII Logic. */
    for (tout = 100; tout; tout--);
    LPC_EMAC->SUPP = 0;

    /* Put the LAN8700 in reset mode */
    write_PHY (PHY_REG_BCR, 0x8000);
    for (tout = 0; tout < 1000; tout++);

    /* Wait for hardware reset to end. */
    for (tout = 0; tout < 0x100000; tout++)
    {
        regv = read_PHY (PHY_REG_BCR);
        if (!(regv & 0x8000))
        {
            /* Reset complete */
            break;
        }
    }



    /* Check if this is a KSZ8001 PHY. */
    id1 = read_PHY (PHY_REG_PID1);
    id2 = read_PHY (PHY_REG_PID2);
    if (((id1 << 16) | (id2 & 0xFFF0)) == PHY_ID)
    {
        /* Configure the PHY device */

        /* Use autonegotiation about the link speed. */
        write_PHY (PHY_REG_BCR, PHY_AUTO_NEG);
        /* Wait to complete Auto_Negotiation. */
        for (tout = 0; tout < 0x100000; tout++)
        {
            regv = read_PHY (PHY_REG_BSR);
            if (regv & 0x0020)
            {
                /* Autonegotiation Complete. */
                break;
            }
        }

    }

    /* Check the link status. */
    for (tout = 0; tout < 0x10000; tout++) {
        regv = read_PHY (0x10);
        if (regv & 0x0001)
        {
            /* Link is on. */
            break;
        }
    }

    /* Configure Full/Half Duplex mode. */
    if (regv & 0x0004) {
        /* Full duplex is enabled. */
        LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
        LPC_EMAC->Command |= CR_FULL_DUP;
        LPC_EMAC->IPGT     = IPGT_FULL_DUP;
    }
    else {
        /* Half duplex mode. */
        LPC_EMAC->IPGT = IPGT_HALF_DUP;
    }

    /* Configure 100MBit/10MBit mode. */
    if (regv & 0x0002) {
        /* 10MBit mode. */
        LPC_EMAC->SUPP = 0;
        tout=read_PHY (0x18);
        write_PHY (0x18, tout|0x0020 |0x0004);
    }
    else {
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
    LPC_EMAC->RxFilterCtrl = RFC_MCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;

    /* Enable EMAC interrupts. */
    LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;


    /* Reset all interrupts */
    LPC_EMAC->IntClear  = 0xFFFF;

    if ( IRQFunc::install_irq( ENET_IRQn, HIGHEST_PRIORITY + 14) == FALSE )   //3
    {
        return ;
    }

    /* Enable receive and transmit mode of MAC Ethernet core */
    LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1     |= MAC1_REC_EN;
}


/*--------------------------- int_enable_eth ---------------------------------*/

void int_enable_eth (void) {
    /* Ethernet Interrupt Enable function. */
    NVIC_EnableIRQ(ENET_IRQn);
}


/*--------------------------- int_disable_eth --------------------------------*/

void int_disable_eth (void) {
    /* Ethernet Interrupt Disable function. */
    NVIC_DisableIRQ(ENET_IRQn);
}


/*--------------------------- send_frame -------------------------------------*/

void send_frame (OS_FRAME *frame) {
    /* Send frame to EMAC ethernet controller */
    U32 idx,len;
    U32 *sp,*dp;

    idx = LPC_EMAC->TxProduceIndex;
    sp  = (U32 *)&frame->data[0];
    dp  = (U32 *)Tx_Desc[idx].Packet;

    /* Copy frame data to EMAC packet buffers. */
    for (len = (frame->length + 3) >> 2; len; len--) {
        *dp++ = *sp++;
    }
    Tx_Desc[idx].Ctrl = (frame->length-1) | (TCTRL_INT | TCTRL_LAST);

    /* Start frame transmission. */
    if (++idx == NUM_TX_FRAG) idx = 0;
    LPC_EMAC->TxProduceIndex = idx;
}


/*--------------------------- interrupt_ethernet -----------------------------*/

void ENET_IRQHandler (void) {
    /* EMAC Ethernet Controller Interrupt function. */
    OS_FRAME *frame;
    U32 idx,int_stat,RxLen,info;
    U32 *sp,*dp;
    UWord32 ENback[2];

    while ((int_stat = (LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)) != 0) {
        LPC_EMAC->IntClear = int_stat;
        if (int_stat & INT_RX_DONE) {
            /* Packet received, check if packet is valid. */
            idx = LPC_EMAC->RxConsumeIndex;
            while (idx != LPC_EMAC->RxProduceIndex) {
                info = Rx_Stat[idx].Info;
                if (!(info & RINFO_LAST_FLAG)) {
                    goto rel;
                }

                RxLen = (info & RINFO_SIZE) - 3;
                if (RxLen > ETH_MTU || (info & RINFO_ERR_MASK)) {
                    /* Invalid frame, ignore it and free buffer. */
                    goto rel;
                }
                /* Flag 0x80000000 to skip sys_error() call when out of memory. */
                frame = alloc_mem (RxLen | 0x80000000);
                /* if 'alloc_mem()' has failed, ignore this packet. */
                if (frame != NULL) {
                    dp = (U32 *)&frame->data[0];
                    sp = (U32 *)Rx_Desc[idx].Packet;
                    for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
                        *dp++ = *sp++;
                    }
                    put_in_queue (frame);
                }
rel:
                if (++idx == NUM_RX_FRAG) idx = 0;
                /* Release frame from EMAC buffer. */
                LPC_EMAC->RxConsumeIndex = idx;
            }
        }
        if (int_stat & INT_TX_DONE) {
            /* Frame transmit completed. */
        }
    }

}


/*--------------------------- rx_descr_init ----------------------------------*/

static void rx_descr_init (void) {
    /* Initialize Receive Descriptor and Status array. */
    U32 i;

    for (i = 0; i < NUM_RX_FRAG; i++) {
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

static void tx_descr_init (void) {
    /* Initialize Transmit Descriptor and Status array. */
    U32 i;

    for (i = 0; i < NUM_TX_FRAG; i++) {
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

static void write_PHY (U32 PhyReg, U16 Value) {
    /* Write a data 'Value' to PHY register 'PhyReg'. */
    U32 tout;

    LPC_EMAC->MADR = PHY_DEF_ADR | PhyReg;
    LPC_EMAC->MWTD = Value;

    /* Wait utill operation completed */
    for (tout = 0; tout < MII_WR_TOUT; tout++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
            break;
        }
    }
}


/*--------------------------- read_PHY ---------------------------------------*/

static U16 read_PHY (U32 PhyReg) {
    /* Read a PHY register 'PhyReg'. */
    U32 tout, val;

    LPC_EMAC->MADR = PHY_DEF_ADR | PhyReg;
    LPC_EMAC->MCMD = MCMD_READ;

    /* Wait until operation completed */
    for (tout = 0; tout < MII_RD_TOUT; tout++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
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
