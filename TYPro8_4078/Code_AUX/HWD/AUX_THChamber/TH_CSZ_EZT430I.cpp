/*****************************************************************************
// 程序文件      :TH_CSZ_EZT430I.C    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// EZT430I  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以字Word操作)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 24 XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_EZT430I[TH_EZT430I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x24, 0x00, 0xDC, 0xC8, 0x58
};     // write Master to  Slave cmd  (设置温度22度，就是220)

//  MODEL: 01 06 00 29 XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
//01 06 00 29 XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
//设置值=湿度值*10;    如：55(55*10),85(85*10) %湿度Humi.
const unsigned char uc_COM_WriteHumiCmd_EZT430I[TH_EZT430I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x29, 0x02, 0x26, 0xD8, 0xB8
};     // write Master to  Slave cmd  (设置湿度55%，就是550)

//  从机回复时，Reply一模一样：
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 24 XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteReply_EZT430I[TH_EZT430I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x24, 0x00, 0xDC, 0xC8, 0x58
};     // write Slave to  Master reply (回复设置温度22度，就是220)

//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 23 00 01 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadTempCmd_EZT430I[TH_EZT430I_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x23, 0x00, 0x01, 0x75, 0xC0
};    // read Master to  Slave cmd (读取1个字)

//  MODEL: 01 03 00 28 00 01 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadHumiCmd_EZT430I[TH_EZT430I_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x28, 0x00, 0x01, 0x04, 0x02
};    // read Master to  Slave cmd (读取1个字)

//  从机回复时，Reply使用full完整方式，不使用integer整数方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX温度值先传高位再传低位; YY校验值先传低位再传高位)
//  使用full完整方式时，显示值=温度值/10;    如：60(600/10)度,22(220/10)度
const unsigned char uc_COM_ReadReply_EZT430I[TH_EZT430I_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x00, 0xDC, 0xB9, 0xDD
};     // read Slave to  Master reply  //(回复温度22度，就是220)

//  3-M)要求运行命令主机到从机
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 23 00 01 YY YY   (YY校验值先传低位再传高位)
//  从机回复时，Reply使用full完整方式，不使用integer整数方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX温度值先传高位再传低位; YY校验值先传低位再传高位)
//  使用full完整方式时，显示值=温度值/10;    如：60(600/10)度,22(220/10)度


const unsigned char uc_COM_RunCmd_EZT430I[TH_EZT430I_MAX_COM_RUN_CMD] =  {
    0x01, 0x03, 0x00, 0x23, 0x00, 0x01, 0x75, 0xC0
};    // read Master to  Slave cmd (读取1个字)

//  4-M)要求停止命令主机到从机命令
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 24 XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
//  从机回复时，Reply一模一样：
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 24 XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)

const unsigned char uc_COM_StopCmd_EZT430I[TH_EZT430I_MAX_COM_STOP_CMD] =  {
    0x01, 0x06, 0x00, 0x24, 0x00, 0xDC, 0xC8, 0x58
};     // write Master to  Slave cmd  (设置温度22度，就是220)

//
// EZT430I  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------
void TH_EZT430I::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_EZT430I::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_EZT430I::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_EZT430I::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_EZT430I::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_EZT430I::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_EZT430I::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

}

/*----*/
unsigned char TH_EZT430I::SetPresentValue_Check(unsigned char ChanNo, short int Value)
{
// 更新现时取回的值

    if(	Value == 0)
    {   // 避过交替出现的零值显示出现。
        if( MTCI_CTRL::gR_Zero_Counter[ChanNo]++ <= 5)
            return  0;
        else
            MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }
    else
    {
        MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }
    return  1;
}

void TH_EZT430I::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_EZT430I_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_EZT430I[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_EZT430I[2])
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_EZT430I[0] && gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_EZT430I[1] && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_EZT430I[2])
#endif
        {   // 利用包头或包尾判断数据值。
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_EZT430I_MIN_RESPONSE_CMD_NUM)
// 			   return;	// 避免温度或湿度交替接收数据时,更新错误值。

            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );//  温度
            if( TH_EZT430I::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_EZT430I::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT430I_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_EZT430I[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_EZT430I[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    }
    else
        return;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
    MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}


void TH_EZT430I::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT430I_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT430I_MAX_COM_READ_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_EZT430I[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT430I_MAX_COM_READ_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT430I[MbChkSum];
    }
    uw16_len = TH_EZT430I_MAX_COM_READ_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT430I_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT430I_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}

void TH_EZT430I::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT430I_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    //for(MbChkSum = 0;MbChkSum < (MAX_COM_READ_CMD-2);MbChkSum++)
    for(MbChkSum = 0; MbChkSum < TH_EZT430I_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_EZT430I[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT430I[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT430I[MbChkSum];
#endif
    }
    //MbChkSum = CRC16(MbBuf,MAX_COM_READ_CMD-2);
    //MbBuf[MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
    //MbBuf[MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT430I_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT430I_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}

void TH_EZT430I::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT430I_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT430I_MAX_COM_WRITE_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_EZT430I[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT430I_MAX_COM_WRITE_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_EZT430I[MbChkSum];
    }
    uw16_len = TH_EZT430I_MAX_COM_WRITE_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT430I_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}



