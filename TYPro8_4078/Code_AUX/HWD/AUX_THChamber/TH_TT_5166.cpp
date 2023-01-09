/*****************************************************************************
// 程序文件      :TH_TT_5166.C    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2017.10.10
*****************************************************************************/


#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
// TT_5166  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以字Word操作)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 06 00 26 XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_TT_5166[TH_TT_5166_MAX_COM_WRITE_CMD] = {
    0x00, 0x06, 0x00, 0x26, 0x00, 0xDC, 0x68, 0x49
};     // write Master to  Slave cmd  (设置温度22度，就是220)

//  MODEL: 00 06 00 27 XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
//01 06 00 48 XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
//设置值=湿度值*10;    如：55(55*10),85(85*10) %湿度Humi.
const unsigned char uc_COM_WriteHumiCmd_TT_5166[TH_TT_5166_MAX_COM_WRITE_CMD] = {
    0x00, 0x06, 0x00, 0x27, 0x02, 0x26, 0xB8, 0xAA
};     // write Master to  Slave cmd  (设置湿度55%，就是550)

//  从机回复时，Reply一模一样：
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 00 06 00 26 XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteReply_TT_5166[TH_TT_5166_MAX_COM_WRITE_CMD] = {
    0x00, 0x06, 0x00, 0x26, 0x00, 0xDC, 0x68, 0x49
};     // write Slave to  Master reply (回复设置温度22度，就是220)

//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 03 00 00 00 01 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadTempCmd_TT_5166[TH_TT_5166_MAX_COM_READ_CMD] = {
    0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0xDB
};    // read Master to  Slave cmd (读取1个字)

//  MODEL: 00 03 00 03 00 01 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadHumiCmd_TT_5166[TH_TT_5166_MAX_COM_READ_CMD] = {
    0x00, 0x03, 0x00, 0x03, 0x00, 0x01, 0x75, 0xDB
};    // read Master to  Slave cmd (读取1个字)

//  从机回复时，Reply使用full完整方式，不使用integer整数方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 00 03 02 XX XX YY YY   (XX温度值先传高位再传低位; YY校验值先传低位再传高位)
//  使用full完整方式时，显示值=温度值/100;    如：60(6000/100)度,22(2200/100)度
const unsigned char uc_COM_ReadReply_TT_5166[TH_TT_5166_MAX_COM_READ_REPLY] = {
    0x00, 0x03, 0x02, 0x08, 0x98, 0x83, 0xEE
};     // read Slave to  Master reply  //(回复温度22度，就是2200)

//  3-M)要求运行命令主机到从机
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 05 00 00 FF 00 YY YY   (YY校验值先传低位再传高位)


const unsigned char uc_COM_RunCmd_TT_5166[TH_TT_5166_MAX_COM_RUN_CMD] =  {
    0x00, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x8D, 0xEB
};    // read Master to  Slave cmd (读取1个字)

//  4-M)要求停止命令主机到从机命令
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 05 00 01 FF 00 YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)

const unsigned char uc_COM_StopCmd_TT_5166[TH_TT_5166_MAX_COM_STOP_CMD] =  {
    0x00, 0x05, 0x00, 0x01, 0xFF, 0x00, 0xDC, 0x2B
};     // write Master to  Slave cmd  (设置温度22度，就是220)

//
// TT_5166  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------


void TH_TT_5166::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_TT_5166::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_TT_5166::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_TT_5166::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_TT_5166::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_TT_5166::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_TT_5166::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

\
/*----*/
unsigned char TH_TT_5166::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_TT_5166::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
	
		uw16_Total =  UARTFunc::uart_bytenum(ChanNo);
		
    if( uw16_Total < TH_TT_5166_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);
		
    uw16_i = 0;
    do {
//         #ifdef COM_CMD_CHANNEL_DIFFER
// 		if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TT_5166[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TT_5166[2])
// 		#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TT_5166[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TT_5166[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TT_5166[2])
//         #endif
        {   // 利用包头或包尾判断数据值。
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );//  温度
            if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
                PresentValue /=10;  // 显示值=传送值/100;
            if( TH_TT_5166::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_TT_5166::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_TT_5166[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_TT_5166[MbChkSum];
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

    MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
    MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理
}


void TH_TT_5166::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TT_5166[MbChkSum];

    uw16_len = TH_TT_5166_MAX_COM_RUN_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_TT_5166::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_TT_5166[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_TT_5166[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_TT_5166[MbChkSum];
#endif
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_TT_5166::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TT_5166[MbChkSum];

    uw16_len = TH_TT_5166_MAX_COM_STOP_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



