/*****************************************************************************
// 程序文件      :TC_EUROTHERM_3216.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


const unsigned char uc_COM_WriteCmd_3216[TC_3216_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x80, 0x04, 0x00, 0x02, 0x04, 0x41, 0xA0, 0x00, 0x00, 0x86, 0x63
};     // write Master to  Slave cmd

const unsigned char uc_COM_WriteReply_3216[TC_3216_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x80, 0x04, 0x00, 0x02, 0x04, 0x41, 0xA0, 0x00, 0x00, 0x86, 0x63
};     // write Slave to  Master reply

const unsigned char uc_COM_ReadCmd_3216[TC_3216_MAX_COM_READ_CMD] = {
    0x01, 0x04, 0x80, 0x02, 0x00, 0x02, 0xF9, 0xCB
};    // read Master to  Slave cmd

const unsigned char uc_COM_ReadReply_3216[TC_3216_MAX_COM_READ_REPLY] = {
    0x01, 0x04, 0x02, 0x00, 0xDC, 0xB8, 0xA9
};     // read Slave to  Master reply


void TC_3216::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_3216::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_3216::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_3216::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_3216::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_3216::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_3216::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}






void TC_3216::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(ChanNo) < TC_3216_MIN_RESPONSE_CMD_LEN) return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_3216[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_3216[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_3216[2])   //&& gR_CmdBuf[7] == 0x00)
        {
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );

            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
            return;
        }
//         else
//         {
//            return;
//         }

        uw16_i++;
    } while(uw16_i < TC_3216_MIN_RESPONSE_CMD_LEN);
}


void TC_3216::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写命令
    //unsigned char slave_addr = 0x01;
    //unsigned char hbspr300 = 0x01;
    //unsigned char lbspr300 = 0x2C;
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    union UW_F
    {
        float f_member;
        unsigned long ul_member;
    } F_data;


    for(MbChkSum = 0; MbChkSum < TC_3216_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_3216[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    F_data.f_member =  (float) register_data;
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-6] = (unsigned char) (F_data.ul_member >> 24);
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-5] = (unsigned char) (F_data.ul_member >> 16);
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-4] = (unsigned char) (F_data.ul_member >> 8);
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-3] = (unsigned char) (F_data.ul_member);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_3216_MAX_COM_WRITE_CMD-2);
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_3216_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_3216_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}



void TC_3216::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令，实际上跟发送读命令一样是读取温度命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_3216_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_3216[MbChkSum];

    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_3216_MAX_COM_READ_CMD-2);
        MbBuf[TC_3216_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_3216_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_3216_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_3216::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];



    for(MbChkSum = 0; MbChkSum < TC_3216_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_3216[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_3216_MAX_COM_READ_CMD-2);
        MbBuf[TC_3216_MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
        MbBuf[TC_3216_MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_3216_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_3216::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{

    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];



    //for(MbChkSum = 0;MbChkSum < (TC_3216_MAX_COM_WRITE_CMD-2);MbChkSum++)
    for(MbChkSum = 0; MbChkSum < TC_3216_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_3216[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_3216_MAX_COM_READ_CMD-2);
        MbBuf[TC_3216_MAX_COM_WRITE_CMD-2] = (unsigned char) (MbChkSum >>8);
        MbBuf[TC_3216_MAX_COM_WRITE_CMD-1] = (unsigned char) (MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_3216_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}



