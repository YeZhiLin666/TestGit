#include "includes.h"

U8 TcpRxTx::socket_tcp = 0;
U8 TcpRxTx::ENET_BUF[ENET_BUFSIZE];
U16 TcpRxTx::ENET_BufStart = 0;
U16 TcpRxTx::ENET_BufEnd = 0;
U16 TcpRxTx::ENET_BufLen = 0;

U8 TcpRxTx::abort_tcp2 = 0;
U8 TcpRxTx::socket_tcp2 = 0;
U8 TcpRxTx::ENET_BUF2[ENET_BUFSIZE];
U16 TcpRxTx::ENET_BufStart2 = 0;
U16 TcpRxTx::ENET_BufEnd2 = 0;
U16 TcpRxTx::ENET_BufLen2 = 0;

// U8  UdpRxTx::socket_udp = 0;
U8  UdpRxTx::socket_MM_M = 0;
// U8  UdpRxTx::socket_MM_S = 0;

U8  uc_remotemIpAdr[4];

extern          uint8 	   uc_UDP_SendLock;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

uint8 UdpRxTx::uc_udpENET_BUF[ENET_BUFSIZE];
uint16 UdpRxTx::uc_udpENET_BufStart;
uint16 UdpRxTx::uc_udpENET_BufEnd;
uint16 UdpRxTx::uc_udpENET_BufLen;
uint8  UdpRxTx::uc_udpSourceIp;
uint8  UdpRxTx::uc_udpSourcePort;
//ReceiveQueue1   UdpRxTx::ReceiveQueue;            // 接收队列个数管理,相当于指针 ;
#pragma arm section //结束外存变量定义


void UdpRxTx::Init_McuUdp()
{


// 	  init_TcpNet();

// 		socket_udp = udp_get_socket(0, UDP_OPT_SEND_CS|UDP_OPT_CHK_CS, udp_callback);
// 	   if (socket_udp != 0)
// 		{
// 		    udp_open (socket_udp, PORT_NUM2);
// 		}

    socket_MM_M = udp_get_socket(0, UDP_OPT_SEND_CS|UDP_OPT_CHK_CS, udp_callback);
    if (socket_MM_M != 0)
    {
        udp_open (socket_MM_M, MCU_PORT_M);
    }

//        socket_MM_S = udp_get_socket(0, UDP_OPT_SEND_CS|UDP_OPT_CHK_CS, udp_callback);
// 	     if (socket_MM_M != 0)
// 		{
// 		    udp_open (socket_MM_M, MCU_PORT_S);
// 		}
}
//void TcpRxTx::GetBFromTCP(uint32 * uw32_Data)
//{
//	for(int8 c8_i = 3; c8_i >= 0;c8_i --)
//	{
//		if(ENET_BufLen > 0)
//		{
//			CommonFunc::uc_F2UC[c8_i] = ENET_BUF[ENET_BufStart];
//			//ENET_BUF[ENET_BufStart] =0;
//			ENET_BufLen --;
//			ENET_BufStart ++;
//			if(ENET_BufStart >= ENET_BUFSIZE)
//			{
//				ENET_BufStart = 0;
//			}
//		}
//	}
//
// 	*uw32_Data = CommonFunc::UC_2_DWord();
// 	//return uw32_Data;
// }

void TcpRxTx::GetBFromTCP(uint8 * uc_Data)
{
    unsigned char uc_temp = 0;
    if(ENET_BufLen > 0)
    {
        uc_temp = ENET_BUF[ENET_BufStart];
        //ENET_BUF[ENET_BufStart] =0;
        ENET_BufLen --;
        ENET_BufStart ++;
        if(ENET_BufStart >= ENET_BUFSIZE)
        {
            ENET_BufStart = 0;
        }
    }
    *uc_Data = uc_temp;
    //return uc_temp;
}

void TcpRxTx::procrec (U8 *buf, U16 len)
{
    U16 i = 0;
    uc_UDP_SendLock = 1;
    if(len > 0)
    {
        if(len > (ENET_BUFSIZE - ENET_BufLen))
        {
            int x = 0;
            x++;
            uc_UDP_SendLock = 0;
            return;          //我的缓冲区已经满啦，再送进来的数据将会只能存储一部分，可能会导致出错，还不如直接抛弃这个包
        }
        for(i = 0; i < len; i ++)
        {
            if(ENET_BufLen >= ENET_BUFSIZE)
            {
                break;
            }
            // pattern: When Start != End, data is valid. when Start == End, no data.
            ENET_BUF[ENET_BufEnd] = buf[i];			// get the data
            ENET_BufEnd ++;							// increase End
            ENET_BufLen ++;
            if(ENET_BufEnd >= ENET_BUFSIZE)			// End rollover
            {
                ENET_BufEnd = 0;
            }
            // following protects buf overflow by to much data, if data is too much, it will be cut off.
        }
    }
    // test

    uc_UDP_SendLock = 0;
    //<<<End>>> test
}

U16 TcpRxTx::tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
    /* This function is called by the TCP module on TCP event */
    /* Check the 'Net_Config.h' for possible events.          */
    //par = par;

    if (soc != socket_tcp)
    {
        return (0);
    }

    switch (evt)
    {
    case TCP_EVT_DATA:
        /* TCP data frame has arrived, data is located at *par1, */
        /* data length is par2. Allocate buffer to send reply.   */
        procrec(ptr, par);
        break;

    case TCP_EVT_CONREQ:
        /* Remote peer requested connect, accept it */
        /* Return 1 to accept connection, or 0 to reject connection */
		return (1);

    case TCP_EVT_CONNECT:
        /* The TCP socket is connected */
    	#if  USE_AS_AUXMCU == 0
	    for(unsigned char uc_channel = 0; uc_channel < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++)//191217 zyx
	    {
			if(!Schedule_Manager_task::IfSchRunning(uc_channel))
	            memset(&ChannelInfo_Manager::MetaVariable2[uc_channel][MetaCode_PV_Error_IV].m_fValue,0,sizeof(st_GetMetaValue)*6);  // MetaCode_PV_Error_IV---MetaCode_PV_Error_INVERTER
		}
		#endif
        return (1);

// 	case TCP_EVT_ABORT:
        /* Connection was aborted */
// 		break;

// 	case TCP_EVT_CLOSE:
        /* Connection has been closed */
// 		break;

// 	case TCP_EVT_ACK:
        /* Our sent data has been acknowledged by remote peer */
// 		break;

    default:
        break;
    }
    return (0);
}
uint16 TcpRxTx::SendTo(uint8* pDataBuf, uint16 DataLen)
{
    U16 maxlen = 0;
//	U16 i;
    U8 *sendbuf;

    uc_UDP_SendLock = 1;
    if(tcp_check_send(socket_tcp))
    {
        maxlen = tcp_max_dsize(socket_tcp);
        if(DataLen <= maxlen)
        {
            sendbuf = tcp_get_buf(DataLen);
            if(sendbuf == 0)  // null
            {
                uc_UDP_SendLock = 0;
                return 0;
            }
//			for(i = 0; i < DataLen; i ++)
//			{
//				sendbuf[i] = pDataBuf[i];
//			}
            memcpy(sendbuf, pDataBuf, DataLen);
            tcp_send(socket_tcp, sendbuf, DataLen);
            uc_UDP_SendLock = 0;
            return DataLen;
        }
    }
    uc_UDP_SendLock = 0;
    return 0;

}

// uint8 TcpRxTx::GetBFromTCP2(void)
// {
// 	unsigned char uc_temp = 0;
// 	if(ENET_BufLen2 > 0)
// 	{
// 		uc_temp = ENET_BUF2[ENET_BufStart2];
// 		ENET_BUF2[ENET_BufStart2] =0;
// 		ENET_BufLen2 --;
// 		ENET_BufStart2 ++;
// 		if(ENET_BufStart2 >= ENET_BUFSIZE)
// 		{
// 			ENET_BufStart2 = 0;
// 		}
// 	}
// 	return uc_temp;
// }

void TcpRxTx::procrec2(U8 *buf, U16 len)
{
    U16 i;
    uc_UDP_SendLock = 1;
    if(len > 0)
    {
        if(len > (ENET_BUFSIZE - ENET_BufLen2))
        {
            int x = 0;
            x++;
            uc_UDP_SendLock = 0;
            return;          //我的缓冲区已经满啦，再送进来的数据将会只能存储一部分，可能会导致出错，还不如直接抛弃这个包
        }
        for(i = 0; i < len; i ++)
        {
            if(ENET_BufLen2 >= ENET_BUFSIZE)
            {
                break;
            }
            // pattern: When Start != End, data is valid. when Start == End, no data.
            ENET_BUF2[ENET_BufEnd2] = buf[i];			// get the data
            ENET_BufEnd2 ++;							// increase End
            ENET_BufLen2 ++;
            if(ENET_BufEnd2 >= ENET_BUFSIZE)			// End rollover
            {
                ENET_BufEnd2 = 0;
            }
            // following protects buf overflow by to much data, if data is too much, it will be cut off.
        }
    }
    // test

    uc_UDP_SendLock = 0;
    //<<<End>>> test
}

U16 TcpRxTx::tcp_callback2 (U8 soc, U8 evt, U8 *ptr, U16 par)
{
    /* This function is called by the TCP module on TCP event */
    /* Check the 'Net_Config.h' for possible events.          */
    //par = par;

    if (soc != socket_tcp2)
    {
        return (0);
    }

    switch (evt)
    {
    case TCP_EVT_DATA:
        /* TCP data frame has arrived, data is located at *par1, */
        /* data length is par2. Allocate buffer to send reply.   */
        procrec2(ptr, par);
        break;

    case TCP_EVT_CONREQ:
        /* Remote peer requested connect, accept it */
        return (1);

    case TCP_EVT_CONNECT:
        /* The TCP socket is connected */
        abort_tcp2 = 0;
        return (1);

    case TCP_EVT_ABORT:
        /* Connection was aborted */
        abort_tcp2 = 1;
        break;

// 	case TCP_EVT_CLOSE:
        /* Connection has been closed */
// 		break;

// 	case TCP_EVT_ACK:
        /* Our sent data has been acknowledged by remote peer */
// 		break;

    default:
        break;
    }
    return (0);
}

uint16 TcpRxTx::SendToReport(uint8* pDataBuf, uint16 DataLen)
{
    U16 maxlen = 0;
//	U16 i;
    U8 *sendbuf;
    U8 result = 0;
    uc_UDP_SendLock = 1;
    if(tcp_check_send(socket_tcp2))
    {
        maxlen = tcp_max_dsize(socket_tcp2);
        if(DataLen <= maxlen)
        {
            sendbuf = tcp_get_buf(DataLen);
            if(sendbuf == 0)  // null
            {
                uc_UDP_SendLock = 0;
                return 1;
            }
//			for(i = 0; i < DataLen; i ++)
//			{
//				sendbuf[i] = pDataBuf[i];
//			}
            memcpy(sendbuf, pDataBuf, DataLen);
            //tcp_send(socket_tcp2, sendbuf, DataLen);
            //return DataLen;  //  发送成功。

            if(tcp_send(socket_tcp2, sendbuf, DataLen))  //DKQ  20130626
                result = 3;  //  发送成功。  //DKQ
            else   //DKQ
            {
#if DEBUG_TEST_COMMDATA_MODE == 1
                EthernetParse::uw32_TestBlock1++; //  仅为测试用，成型后要清理丢。
#endif

                result = 2; //DKQ
            }
        }
        else
        {
            result = 1;
        }
    }

#if DEBUG_TEST_COMMDATA_MODE == 1
    EthernetParse::uw32_TestBlock1++; //  仅为测试用，成型后要清理丢。
#endif
    uc_UDP_SendLock = 0;
    return result;
}

U16 UdpRxTx::udp_callback (U8 socket, U8 *remip, U16 port, U8 *buf, U16 len)
{
// This function is called when UDP data is received //

// Process received data from 'buf' //
// 	if ((len == 2) && (bindex == (buf[0]<<8 | buf[1])))
// 	{
// 	wait_ack == __FALSE;
// 	}
// 	return (0);


    //if ((socket != socket_udp)&&(socket != socket_MM_S))
    if ((socket != socket_MM_M)&&(socket != 0))
    {
        return (0);
    }

    udp_procrec(buf, len, remip, port);

    return (0);

}

void UdpRxTx::udp_procrec (U8 *buf, U16 len, U8 *remip, U16 port)
{
    if(len > 0)
    {
        MultiMcuComm::ReceiveData_ToBuffer(buf, len, remip, port);
    }
}


/*************************UDP  SEND**********************/
//void UdpRxTx::send_data(uint8* pDataBuf, uint16 DataLen,unsigned char Soc_Idx)
unsigned char UdpRxTx::udp_SendTo(uint8* pDataBuf, uint16 DataLen)
{
    U8*sendbuf;
    U8 Success = 0;

    if(DataLen > MAX_PACKLIMIT_SIZE_UDP)
        return 0;

    sendbuf = udp_get_buf (DataLen);//uc_udpENET_BufLen
    if(sendbuf == 0)  // null
        return 0;
//  str_copy (sendbuf,pDataBuf);
    memcpy(sendbuf,pDataBuf,DataLen);
    //udp_send (UdpSoc_A[Soc_Idx].socket, UdpSoc_A[Soc_Idx].RemoteIP, UdpSoc_A[Soc_Idx].RemotePort, sendbuf, DataLen);
    Success = udp_send (0, uc_remotemIpAdr, PORT_NUM2, sendbuf, DataLen);  //socket_udp
    return   Success;
}

unsigned char UdpRxTx::udp_SendToLink(uint8* pDataBuf, uint16 DataLen, uint8 uc_IpAdr, uint16 uw16_Port)
{
    U8*sendbuf;
    U8 Success = 0;
    UWord32 uw32_IP;

    if(DataLen > MAX_PACKLIMIT_SIZE_UDP)
        return 0;

    sendbuf = udp_get_buf (DataLen);//uc_udpENET_BufLen
    if(sendbuf == 0)  // null
        return 0;
//  str_copy (sendbuf,pDataBuf);
    memcpy(sendbuf,pDataBuf,DataLen);
    //udp_send (UdpSoc_A[Soc_Idx].socket, UdpSoc_A[Soc_Idx].RemoteIP, UdpSoc_A[Soc_Idx].RemotePort, sendbuf, DataLen);

    //uw32_IP = 196+(168<<8)+(1<<16)+(uc_IpAdr<<24);
    uw32_IP = uc_remotemIpAdr[0]+(uc_remotemIpAdr[1]<<8)+(uc_remotemIpAdr[2]<<16)+(uc_IpAdr<<24);
//              uw32_IP = 196+(168<<8)+(1<<16)+(255<<24);  // 暂时测试用PC广播包 MCU_UDP_BROADCAST

    Success = udp_send (socket_MM_M, (uint8 *)&uw32_IP, uw16_Port, sendbuf, DataLen);
    return   Success;
}

unsigned char UdpRxTx::udp_SendToBroadCast(uint8* pDataBuf, uint16 DataLen)
{
    U8*sendbuf;
    U8 Success = 0;
    UWord32 uw32_IP;
    uint8 uc_IpAdr = 255;
    uint16 uw16_Port = MCU_PORT_M;

    if(DataLen > MAX_PACKLIMIT_SIZE_UDP)
        return 0;

    sendbuf = udp_get_buf (DataLen);//uc_udpENET_BufLen
    if(sendbuf == 0)  // null
        return 0;

    memcpy(sendbuf,pDataBuf,DataLen);

    uw32_IP = uc_remotemIpAdr[0]+(uc_remotemIpAdr[1]<<8)+(uc_remotemIpAdr[2]<<16)+(uc_IpAdr<<24);
//              uw32_IP = 196+(168<<8)+(1<<16)+(255<<24);  // 暂时测试用PC广播包 MCU_UDP_BROADCAST

    Success = udp_send (socket_MM_M, (uint8 *)&uw32_IP, uw16_Port, sendbuf, DataLen);
    return   Success;
}
