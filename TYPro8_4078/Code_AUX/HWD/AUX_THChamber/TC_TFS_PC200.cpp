/*****************************************************************************
// 程序文件      :TC_TFS_PC200.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];



//------------------------------------------------------------------------------------------------
//  Thermo Fisher Scientific	PC200 controller //
//  MCU2468AUX  4RS232板 PN：502314		Hex （应用1）
//  串口通信方式  in binary form, do not use ASCII. 传输方式：All byte values are shown in hex
//  特别注意：运行和停止命令，跟读和写命令一致。


//  Lead char Addr-MSB Addr-LSB Command n d-bytes d-byte 1 ... d-byte n CheckSum
//  CA or CC 0 1
//  Lead char 0xCA (RS-232) 0xCC (RS-485)
//  Device address is 1 (RS-232)
//  Addr-msb Most significant byte of device address (RS-232: 0).
//  Addr-lsb Least significant byte of device address is 1 (RS-232).
//  Command Command byte (see Table 1).
//  n d-bytes Number of data bytes to follow (00 to 03 hex).
//  d-byte 1 1st data byte (the qualifier byte is considered a data byte).
//  ... ...
//  d-byte n nth data byte.
//  CheckSum Bitwise inversion of the 1 byte sum of bytes beginning with the most
//  significant address byte and ending with the byte preceding the CheckSum.
//  (To perform a bitwise inversion, "exclusive OR" the one byte sum with FF hex.)

//  NOTE 16 bit used with °C, 32 bit with °F and °K using 0.01 precision

//  1-M)设置温度主机到从机 (以字节操作)
//  {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [data high byte] [data low byte] [CheckSum (FF-Sum)]}
//  CA 00 01 F0 02(d1)(d2)(cs)
//  CA 00 01 F0 02 XX XX YY    (XX设置值先传高位再传低位; YY校验值)
//  设置值=温度值*10;    如：60(60*10)度,22(22*10)度

//  Reply CMD: {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [qualifier byte] [data high byte] [data low byte] [CheckSum]}
//  从机回复时：
//  CA 00 01 F0 03(qb)(d1)(d2)(cs)
//  CA 00 01 F0 03(qb) XX XX YY    (XX设置值先传高位再传低位; YY校验值)

const unsigned char uc_COM_WriteCmd_PC200[TC_PC200_MAX_COM_WRITE_CMD] = {
    0xCA, 0x00, 0x01, 0xF0, 0x02, 0x00, 0xDC, 0x30
};     // write Master to  Slave cmd  (设置温度22度)

//  从机回复时，Reply一模一样设置值：
const unsigned char uc_COM_WriteReply_PC200[TC_PC200_MAX_COM_WRITE_REPLY] = {
    0xCA, 0x00, 0x01, 0xF0, 0x03, 0x11, 0x00, 0xDC, 0xEE
};     // write Slave to  Master reply (回复设置温度22.0度)

//  2-M)读取温度主机到从机  (以字Word操作)
//  {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [CheckSum]}
//  Read Internal Temperature (RTD1)
//  CA 00 01 20 00 YY    (YY校验值)
//  CA 00 01 20 00 DE

//  从机回复时。
//  Reply CMD:  {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [qualifier byte] [data high byte] [data low byte] [CheckSum]}
//  CA 00 01 20 03(qb)(d1)(d2)(cs)
//  Reply：CA 00 01 20 03 (qb)XX XX  YY   (XX温度值先传高位再传低位; YY校验值)
//  显示值=温度值/10;    如：60(600/10)度,22(220/10)度

//  Read External Temperature (RTD2)
//  MASTER SENDS
//  CA 00 01 21 00 DD
//  UNIT RESPONDS
//  CA 00 01 21 03(qb)(d1)(d2)(cs)

const unsigned char uc_COM_ReadCmd_PC200[TC_PC200_MAX_COM_READ_CMD] = {
    0xCA, 0x00, 0x01, 0x20, 0x00, 0xDE
};    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_PC200[TC_PC200_MAX_COM_READ_REPLY] = {
    0xCA, 0x00, 0x01, 0x20, 0x03, 0x11, 0x00, 0xDC, 0xEE
};     // read Slave to  Master reply  //(回复温度22度，就是220)

//  3-M)要求运行命令主机到从机
//  {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [CheckSum]}

//  CA 00 01 20 00 YY   (YY校验值先传低位再传高位)
//  (读取温度)CA 00 01 20 00 DE

const unsigned char uc_COM_RunCmd_PC200[TC_PC200_MAX_COM_RUN_CMD] = {
    0xCA, 0x00, 0x01, 0x20, 0x00, 0xDE
};    // read Master to  Slave cmd (读取)

//  4-M)要求停止命令主机到从机命令
//  {[Lead char(1 byte)] [Addr-msb(1 byte)] [Addr-lsb(1 byte)] [command(1 byte)] [register byte] [data high byte] [data low byte] [CheckSum]}

//  CA 00 01 F0 02 00 DC YY   (XX设置值先传高位再传低位; YY校验值)
//  (设置温度22度，就是220) CA 00 01 F0 02 00 DC 30

const unsigned char uc_COM_StopCmd_PC200[TC_PC200_MAX_COM_STOP_CMD] = {
    0xCA, 0x00, 0x01, 0xF0, 0x02, 0x00, 0xDC, 0x30
};    // write Master to  Slave cmd  (设置温度22度)


//
//  Thermo Fisher Scientific	PC200 controller //
//----------------------------------------------------------------------------------------------------

void TC_PC200::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_PC200::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_PC200::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_PC200::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_PC200::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_PC200::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_PC200::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_PC200::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(ChanNo) < TC_PC200_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_PC200[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_PC200[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_PC200[2] && MTCI_CTRL::gR_CmdBuf[uw16_i+3] == uc_COM_ReadReply_PC200[3]) // && gR_CmdBuf[TC_PC200_MAX_COM_READ_REPLY] == 0x00)
        {
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+TC_PC200_READ_REPLY_POINT_VAlUE]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+TC_PC200_READ_REPLY_POINT_VAlUE+1]) );

            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
            return;
        }
        //     else
        // 	{
        //        return;
        //     }

        uw16_i++;
    } while(uw16_i < TC_PC200_MIN_RESPONSE_CMD_LEN);

}


void TC_PC200::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写命令
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];


    for(MbChkSum = 0; MbChkSum < TC_PC200_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_PC200[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[TC_PC200_WRITE_CMD_POINT_VAlUE] = (unsigned char) (register_data >> 8);
    MbBuf[TC_PC200_WRITE_CMD_POINT_VAlUE+1] = (unsigned char) (register_data & 255);
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + FIRST_UART_NUM);
#endif

    MbBuf[TC_PC200_MAX_COM_WRITE_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],TC_PC200_MAX_COM_WRITE_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_PC200_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_PC200::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_PC200_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_PC200[MbChkSum];
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // 实际上跟发送读命令一样是读取温度命令
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + FIRST_UART_NUM);
        MbBuf[TC_PC200_MAX_COM_RUN_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],TC_PC200_MAX_COM_RUN_CMD-2);
    }
#endif
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_PC200_MAX_COM_RUN_CMD);
    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_PC200::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_PC200_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_PC200[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + FIRST_UART_NUM);
    }
#endif
    MbBuf[TC_PC200_MAX_COM_READ_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],TC_PC200_MAX_COM_READ_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_PC200_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_PC200::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_PC200_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_PC200[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + FIRST_UART_NUM);
        MbBuf[TC_PC200_MAX_COM_STOP_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],TC_PC200_MAX_COM_STOP_CMD-2);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_PC200_MAX_COM_STOP_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}




