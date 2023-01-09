/*----------------------------------------------------------------------------
*      RL-ARM - A P I
*----------------------------------------------------------------------------
*      Name:    TcpTxRx.h
*      Purpose: Application Programming Interface
*      Rev.:    V4.53
*----------------------------------------------------------------------------
*      This code is part of the RealView Run-Time Library.
*      Copyright (c) 2004-2012 KEIL - An ARM Company. All rights reserved.
*---------------------------------------------------------------------------*/


#ifndef _TCPTXRX_H_
#define _TCPTXRX_H_

#include "../Data_Manipulation/datatypes.h"
#include "../Data_Manipulation/CircularQueue.h"

#define ENET_BUFSIZE 1400  //1024

//#include <net_config.h>
#include "../Tcp-net/net_config.h"

extern U8  uc_remotemIpAdr[4];
#define  TcpMaxConNum 18
#define  UdpMaxSocNum 20
#define  UdpMaxIVMCU    4
#define  UdpMaxReceive    (UdpMaxSocNum*2*10)


typedef enum
{
    TCP_DATA_TYPE_NONE      = 0,
    TCP_DATA_TYPE_CHAR      = 0X01,
    TCP_DATA_TYPE_UWORD     = 0X02,
    TCP_DATA_TYPE_WORD      = 0X03,
    TCP_DATA_TYPE_UDWORD    = 0X04,
    TCP_DATA_TYPE_DWORD     = 0X05,
    TCP_DATA_TYPE_FLOAD     = 0X06,

} TCP_DATA_TYPE;

typedef enum
{
    UDP_NORMAL      = 0,
    UDP_BROADCAST   = 1,
} UDP_SOCKET;

typedef struct
{
    uint8  socket;
    uint16 ListenPort;
    uint8  IP[4];
} ST_TcpServer;
typedef struct
{
    uint8  socket;
    uint16 LocalPort;
    uint16 RemotePort;
    uint8  RemoteIP[4];
} ST_TcpClient;
typedef struct
{
    uint8  socket;
    uint16 LocalPort;
    uint8  LocalIP[4];
    uint16 RemotePort;
    uint8  RemoteIP[4];
}
ST_UdpSocket;
typedef struct
{
    U8 socket_tcp ;
    U8 BUF[ENET_BUFSIZE];
    U16 BufStart;
    U16 BufEnd;
    U16 BufLen;
} St_TcpBuf;

typedef struct
{

    U8 Source_Ip;
    U8 Seat;
    //U8 BUF[ENET_BUFSIZE];
    //U16 BufStart;
    //U16 BufEnd;
    U16 Source_Port;
    U16 BufLen;
} St_UdpBuf;


class TcpRxTx
{
public:
    //static U8     tcp2buf[16][ENET_BUFSIZE];
    //static U16    tcp2buf_Length[16];
    //static U8     tcp2buffer_Consume;
    //static U8     tcp2buffer_Produce;

    static uint8  socket_tcp;
    static uint8  ENET_BUF[ENET_BUFSIZE];
    static uint16 ENET_BufStart;
    static uint16 ENET_BufEnd;
    static uint16 ENET_BufLen;

    static U8 abort_tcp2;
    static uint8  socket_tcp2;
    static uint8  ENET_BUF2[ENET_BUFSIZE];
    static uint16 ENET_BufStart2;
    static uint16 ENET_BufEnd2;
    static uint16 ENET_BufLen2;

    static int8   GetBufferFromTcp2(void);
    //static void  SendTCP2data(void);
    //static uint8 GetBFromTCP(void);
// 	static void   GetBFromTCP(uint32 * uw32_Data);
    static void   GetBFromTCP(uint8 * uc_Data);
    static void   procrec (uint8 *buf, uint16 len);
    static uint16 tcp_callback (uint8 soc, uint8 evt, uint8  *ptr, uint16 par);
    static uint16 SendTo(uint8* pDataBuf, uint16 DataLen);
// 	static uint8  GetBFromTCP2(void);
    static void   procrec2 (uint8 *buf, uint16 len);
    static uint16 tcp_callback2 (uint8 soc, uint8 evt, uint8  *ptr, uint16 par);
    static uint16 SendToReport(uint8* pDataBuf, uint16 DataLen);
};

typedef   CIRCULAR_QUEUE(St_UdpBuf, UdpMaxReceive)             ReceiveQueue1;       // |━：：??y?：?：♂D??：oy1：1：∴：a,?：∴|━?┐：?：2????

class UdpRxTx
{
public:
// 	static uint8  socket_udp;
    static uint8  socket_MM_M;
//     static uint8  socket_MM_S;

    static uint8  uc_udpENET_BUF[ENET_BUFSIZE];
    static uint16 uc_udpENET_BufStart;
    static uint16 uc_udpENET_BufEnd;
    static uint16 uc_udpENET_BufLen;
    static uint8  uc_udpSourceIp;
    static uint8  uc_udpSourcePort;
    static ReceiveQueue1         ReceiveQueue;            // ?：?：o??：?：♂D??：oy1：1：∴：a,?：∴|━?┐：?：2???? ;
    static U16    udp_callback(U8 socket, U8 *remip, U16 port, U8 *buf, U16 len);
    //static U16    udp_callback2(U8 socket, U8 *remip, U16 port, U8 *buf, U16 len);
    static void   udp_procrec (U8 *buf, U16 len,U8 *remip,U16 port);
    static unsigned char udp_SendTo(uint8* pDataBuf, uint16 DataLen);
    static unsigned char udp_SendToLink(uint8* pDataBuf, uint16 DataLen, uint8 uc_IpAdr, uint16 uw16_Port);
    static unsigned char udp_SendToBroadCast(uint8* pDataBuf, uint16 DataLen);

//udp

    static void Init_McuUdp();

};


#endif


