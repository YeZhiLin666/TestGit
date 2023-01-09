/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TC_V0_45A1::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_V0_45A1::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_V0_45A1::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_V0_45A1::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_V0_45A1::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_V0_45A1::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_V0_45A1::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/****************************************************************************
 ** GET UART COMMAND WHICH IS TRANSMITTED FROM TEMPERATURTE-CHAMBER       ***
 ** BASED ON MODBUS RTU PROTOCOL, THE RECEIVED COMMAND LOOKS LIKE         ***
 **     SEND:                                                             ***
 ** RECEIVED:                                                             ***
 ** MESSAGE :                                                             ***
 ****************************************************************************/

void TC_V0_45A1::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TC_V0_45A1_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护
//
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01) && (MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03) && (MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x02))
        {
            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+3] << 8) & 0xff00;//高字节
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+4] & 0xff;//低字节

            if(ChamberChn == 0)
                MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);
            else if(ChamberChn == 1)
                MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE,SECOND_CHAMBER_CHANNEL);
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i < uw16_Total) && (uw16_i < MAX_UART_BUF_SIZE - 3));

    return;
}

void TC_V0_45A1::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SetTemp1Cmd[] = {0x01,0x06,0x03,0x61,0x00,0x00,0xff,0xff};//设定温度通道值1
    unsigned char SetTemp2Cmd[] = {0x01,0x06,0x03,0x66,0x00,0x00,0xff,0xff};//设定温度通道值2
    unsigned short crc16;
    unsigned short uw16_len = sizeof(SetTemp1Cmd);
    short temp;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

        if(ChamberChn == 1)
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][SECOND_CHAMBER_CHANNEL];
            SetTemp2Cmd[4] = (temp >> 8) & 0xff;
            SetTemp2Cmd[5] = temp & 0xff;

            crc16 = MTCI_datatypes::CRC16(SetTemp2Cmd,6);
            SetTemp2Cmd[6] = (unsigned char) (crc16 >>8);
            SetTemp2Cmd[7] = (unsigned char) (crc16 & 0xff);

            UARTFunc::uart_putcmd(ChanNo,SetTemp2Cmd,uw16_len);
        }
        else if(ChamberChn == 0)
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
            SetTemp1Cmd[4] = (temp >> 8) & 0xff;
            SetTemp1Cmd[5] = temp & 0xff;

            crc16 = MTCI_datatypes::CRC16(SetTemp1Cmd,6);
            SetTemp1Cmd[6] = (unsigned char) (crc16 >>8);
            SetTemp1Cmd[7] = (unsigned char) (crc16 & 0xff);

            UARTFunc::uart_putcmd(ChanNo,SetTemp1Cmd,uw16_len);
        }
    }
    return;
}

void TC_V0_45A1::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ReadTemp1Cmd[] = {0x01,0x03,0x00,0x02,0x00,0x01,0xff,0xff};//读取上箱温度通道
    unsigned char ReadTemp2Cmd[] = {0x01,0x03,0x00,0x06,0x00,0x01,0xff,0xff};//读取下箱温度通道
    unsigned short crc16;
    unsigned short uw16_len = sizeof(ReadTemp1Cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        if(ChamberChn == 1)
        {
            crc16 = MTCI_datatypes::CRC16(ReadTemp2Cmd,6);
            ReadTemp2Cmd[6] = (unsigned char) (crc16 >>8);
            ReadTemp2Cmd[7] = (unsigned char) (crc16 & 0xff);

            UARTFunc::uart_putcmd(ChanNo,ReadTemp2Cmd,uw16_len);
        }
        else if(ChamberChn == 0)
        {
            crc16 = MTCI_datatypes::CRC16(ReadTemp1Cmd,6);
            ReadTemp1Cmd[6] = (unsigned char) (crc16 >>8);
            ReadTemp1Cmd[7] = (unsigned char) (crc16 & 0xff);

            UARTFunc::uart_putcmd(ChanNo,ReadTemp1Cmd,uw16_len);
        }
    }
    return;
}

void TC_V0_45A1::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Run1cmd[] = {0x01,0x06,0x02,0x31,0x00,0x01,0xff,0xff};//上箱
    unsigned char Run2cmd[] = {0x01,0x06,0x02,0x32,0x00,0x01,0xff,0xff};//下箱
    unsigned short crc16;
    unsigned short uw16_len = sizeof(Run1cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

    if(ChamberChn == 1)
    {
        crc16 = MTCI_datatypes::CRC16(Run2cmd,6);
        Run2cmd[6] = (unsigned char) (crc16 >>8);
        Run2cmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,Run2cmd,uw16_len);
    }
    else if(ChamberChn == 0)
    {
        crc16 = MTCI_datatypes::CRC16(Run1cmd,6);
        Run1cmd[6] = (unsigned char) (crc16 >>8);
        Run1cmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,Run1cmd,uw16_len);
    }

    return;
}

void TC_V0_45A1::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Stop1cmd[] = {0x01,0x06,0x02,0x31,0x00,0x02,0xff,0xff};//上箱
    unsigned char Stop2cmd[] = {0x01,0x06,0x02,0x32,0x00,0x02,0xff,0xff};//下箱
    unsigned short crc16;
    unsigned short uw16_len = sizeof(Stop1cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

    if(ChamberChn == 1)
    {
        crc16 = MTCI_datatypes::CRC16(Stop2cmd,6);
        Stop2cmd[6] = (unsigned char) (crc16 >>8);
        Stop2cmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,Stop2cmd,uw16_len);
    }
    else if(ChamberChn == 0)
    {
        crc16 = MTCI_datatypes::CRC16(Stop1cmd,6);
        Stop1cmd[6] = (unsigned char) (crc16 >>8);
        Stop1cmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,Stop1cmd,uw16_len);
    }

    return;
}



