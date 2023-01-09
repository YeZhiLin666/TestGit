/*****************************************************************************
// 程序文件      :TC_FUTURE_FDCP91.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];



//------------------------------------------------------------------------------------------------
//Future Design Controls P91 T-CONTROLLER温控器测试( CSZ M1.2 Chamber)
//串口通信方式  MODBUS  传输方式：RTU模式 CRC校验
//

//  1-M)设置温度主机到从机 (以字Word操作)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 3C XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteCmd_FDCP91[TC_FDCP91_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};     // write Master to  Slave cmd  (设置温度22度，就是220)

//  从机回复时，Reply一模一样：
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 3C XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteReply_FDCP91[TC_FDCP91_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};     // write Slave to  Master reply (回复设置温度22度，就是220)

//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 3D 00 01 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadCmd_FDCP91[TC_FDCP91_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x80, 0x00, 0x01, 0x85, 0xE2
};    // read Master to  Slave cmd (读取1个字)

//  从机回复时，Reply使用full完整方式，不使用integer整数方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX温度值先传高位再传低位; YY校验值先传低位再传高位)
//  使用full完整方式时，显示值=温度值/10;    如：60(600/10)度,22(220/10)度
const unsigned char uc_COM_ReadReply_FDCP91[TC_FDCP91_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x00, 0xDC, 0xB9, 0xDD
};     // read Slave to  Master reply  //(回复温度22度，就是220)


//  3-M)要求运行命令主机到从机
//  (读取温度)
//01 03 00 80 00 01 YY YY   (YY校验值先传低位再传高位)
//(读取温度)01 03 00 80 00 01 85 E2


const unsigned char uc_COM_RunCmd_FDCP91[TC_FDCP91_MAX_COM_RUN_CMD] = {
    0x01, 0x03, 0x00, 0x80, 0x00, 0x01, 0x85, 0xE2
};    // read Master to  Slave cmd (读取)

//  4-M)要求停止命令主机到从机命令
//  (设置温度22度，就是220)
//{[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//01 06 00 00 00 DC YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)

const unsigned char uc_COM_StopCmd_FDCP91[TC_FDCP91_MAX_COM_STOP_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};    // write Master to  Slave cmd  (设置温度22度)

//
// EZT570I  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------

void TC_FDCP91::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_FDCP91::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_FDCP91::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_FDCP91::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_FDCP91::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_FDCP91::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_FDCP91::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_FDCP91::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    /*unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;

    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned char uc_Buffer[4][6];
    unsigned long int uw32_DO = 0;*/
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(ChanNo) < TC_FDCP91_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_FDCP91[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_FDCP91[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_FDCP91[2])  // && gR_CmdBuf[TC_FDCP91_MAX_COM_READ_REPLY] == 0x00)
        {
            PresentValue = ((short int)(uw16_i+MTCI_CTRL::gR_CmdBuf[TC_FDCP91_READ_REPLY_POINT_VAlUE]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+TC_FDCP91_READ_REPLY_POINT_VAlUE+1]) );

            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
            return;
        }
        //     else
        // 	{
        //        return;
        //     }

        uw16_i++;
    } while(uw16_i < TC_FDCP91_MIN_RESPONSE_CMD_LEN);
}


void TC_FDCP91::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写命令
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_FDCP91[MbChkSum];
    //MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = 0X0D;  // 命令结束标志<CR>
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[TC_FDCP91_WRITE_CMD_POINT_VAlUE] = (unsigned char) (register_data >> 8);
    MbBuf[TC_FDCP91_WRITE_CMD_POINT_VAlUE+1] = (unsigned char) (register_data & 255);
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);

    //MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],MAX_COM_WRITE_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_FDCP91::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_FDCP91[MbChkSum];
    //MbBuf[MAX_COM_RUN_CMD-1] = 0X0D;  // 命令结束标志<CR>
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // 实际上跟发送读命令一样是读取温度命令

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);

        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD-2);
        MbBuf[TC_FDCP91_MAX_COM_RUN_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_FDCP91_MAX_COM_RUN_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_RUN_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_FDCP91::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_FDCP91[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_READ_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
    MbBuf[TC_FDCP91_MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);
    //MbBuf[MAX_COM_READ_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],MAX_COM_READ_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_FDCP91::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_FDCP91[MbChkSum];
    //MbBuf[MAX_COM_STOP_CMD-1] = 0X0D;  // 命令结束标志<CR>

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_STOP_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_STOP_CMD-2] = (unsigned char) (MbChkSum >> 8);
    MbBuf[TC_FDCP91_MAX_COM_STOP_CMD-1] = (unsigned char) (MbChkSum & 255);

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //	return;

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_STOP_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}




