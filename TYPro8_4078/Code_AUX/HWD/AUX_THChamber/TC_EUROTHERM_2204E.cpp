/*****************************************************************************
// 程序文件      :TC_EUROTHERM_2204E.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];




const unsigned char uc_COM_WriteCmd_2204E[TC_2204E_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x02, 0x00, 0xDC, 0x29, 0x93
};     // write Master to  Slave cmd

const unsigned char uc_COM_WriteReply_2204E[TC_2204E_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x02, 0x00, 0xDC, 0x29, 0x93
};     // write Slave to  Master reply

const unsigned char uc_COM_ReadCmd_2204E[TC_2204E_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA
};    // read Master to  Slave cmd

const unsigned char uc_COM_ReadReply_2204E[TC_2204E_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x00, 0xDC, 0xB9, 0xDD
};     // read Slave to  Master reply


void TC_2204E::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_2204E::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_2204E::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_2204E::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_2204E::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_2204E::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_2204E::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}






void TC_2204E::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(ChanNo) < TC_2204E_MIN_RESPONSE_CMD_LEN)
			return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_2204E[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_2204E[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_2204E[2]) // && gR_CmdBuf[7] == 0x00)
        {
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );

            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
            return;
        }
        //     else
        // 	{
        //        return;
        //     }

        uw16_i++;
    } while(uw16_i < TC_2204E_MIN_RESPONSE_CMD_LEN);
}


void TC_2204E::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写命令
    //unsigned char slave_addr = 0x01;
    //unsigned char hbspr300 = 0x01;
    //unsigned char lbspr300 = 0x2C;
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned char XcrcHi,XcrcLo;

    //MbBuf[0] = slave_addr;
    //MbBuf[1] = 0x06;
    //MbBuf[2] = hbspr300;
    //MbBuf[3] = lbspr300;

    //MbBuf[0] = 0x01;
    //MbBuf[1] = 0x06;
    //MbBuf[2] = 0x01;
    //MbBuf[3] = 0x2C;

    //register_data = gW_TemperatureValue[ChanNo];
    //MbBuf[4] = (unsigned char) (register_data >> 8);
    //MbBuf[5] = (unsigned char) (register_data & 255);
    //XChkSum = CRC16(MbBuf,6);
    //XcrcHi = (unsigned char)(XChkSum >> 8);
    //XcrcLo = (unsigned char)(XChkSum & 255);
    //MbBuf[6] = XcrcHi;
    //MbBuf[7] = XcrcLo;

    //uart_putcmd(ChanNo,MbBuf);

    for(MbChkSum = 0; MbChkSum < TC_2204E_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_2204E[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[TC_2204E_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TC_2204E_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_2204E_MAX_COM_WRITE_CMD-2);
    MbBuf[TC_2204E_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_2204E_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_2204E_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}



void TC_2204E::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令，实际上跟发送读命令一样是读取温度命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_2204E_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_2204E[MbChkSum];

    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_2204E_MAX_COM_READ_CMD-2);
        MbBuf[TC_2204E_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_2204E_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_2204E_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_2204E::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    //unsigned char slave_addr = 0x01;
    //unsigned char hbr100 = 0x00;
    //unsigned char lbr100 = 0x64;
    //unsigned char hbnrr = 0x00;
    //unsigned char lbnrr = 0x01;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    //MbBuf[0] = slave_addr;
    //MbBuf[1] = 0x03;
    //MbBuf[2] = hbr100;
    //MbBuf[3] = lbr100;
    //MbBuf[4] = hbnrr;
    //MbBuf[5] = lbnrr;

    //MbBuf[0] = 0x01;
    //MbBuf[1] = 0x03;
    //MbBuf[2] = 0x00;
    //MbBuf[3] = 0x64;
    //MbBuf[4] = 0x00;
    //MbBuf[5] = 0x01;
    //MbChkSum = CRC16(MbBuf,6);
    //MbBuf[6] = (unsigned char) (MbChkSum >>8);
    //MbBuf[7] = (unsigned char) (MbChkSum & 255);
    //uart_putcmd(ChanNo,MbBuf);

    for(MbChkSum = 0; MbChkSum < TC_2204E_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_2204E[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_2204E_MAX_COM_READ_CMD-2);
        MbBuf[TC_2204E_MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
        MbBuf[TC_2204E_MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);
    }
#endif


    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_2204E_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_2204E::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{

    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];


    //for(MbChkSum = 0;MbChkSum < (TC_2204E_MAX_COM_WRITE_CMD-2);MbChkSum++)
    for(MbChkSum = 0; MbChkSum < TC_2204E_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_2204E[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_2204E_MAX_COM_READ_CMD-2);
        MbBuf[TC_2204E_MAX_COM_WRITE_CMD-2] = (unsigned char) (MbChkSum >>8);
        MbBuf[TC_2204E_MAX_COM_WRITE_CMD-1] = (unsigned char) (MbChkSum & 255);
    }
#endif


    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_2204E_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


