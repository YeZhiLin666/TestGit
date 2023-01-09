/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

unsigned char TC_HARDING_THREE_LAYERS_MODBUS_TCP::m_RecvDataType;

void TC_HARDING_THREE_LAYERS_MODBUS_TCP::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_HARDING_THREE_LAYERS_MODBUS_TCP::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_HARDING_THREE_LAYERS_MODBUS_TCP::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i%MAX_UART_BUF_SIZE] == 0x15) && \
                (MTCI_CTRL::gR_CmdBuf[(uw16_i+1)%MAX_UART_BUF_SIZE] == 0x00) && \
                (MTCI_CTRL::gR_CmdBuf[(uw16_i+2)%MAX_UART_BUF_SIZE] == 0x03) && \
                (MTCI_CTRL::gR_CmdBuf[(uw16_i+3)%MAX_UART_BUF_SIZE] == 0x12))
        {
            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+6] << 8) & 0xff00;//高字节
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+7] & 0xff;//低字节
            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,m_RecvDataType,FIRST_CHAMBER_CHANNEL);

            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+12] << 8) & 0xff00;//高字节
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+13] & 0xff;//低字节
            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,m_RecvDataType,SECOND_CHAMBER_CHANNEL);

            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+18] << 8) & 0xff00;//高字节
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+19] & 0xff;//低字节
            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,m_RecvDataType,THIRD_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i + 5 < uw16_Total) && (uw16_i + 5 < MAX_UART_BUF_SIZE));

    return;
}



void TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    short temp;

    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        unsigned char SetTempCmd1[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x03,0x52,0x00,0x64};//写上箱温度设定值
        unsigned char SetTempCmd2[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x03,0x56,0x00,0x64};//写中箱温度设定值
        unsigned char SetTempCmd3[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x03,0x5A,0x00,0x64};//写下箱温度设定值

        unsigned short uw16_len = sizeof(SetTempCmd1);

        switch(ChamberChn)
        {
        case 0:
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
            SetTempCmd1[uw16_len - 2] = (temp >> 8) & 0xff;
            SetTempCmd1[uw16_len - 1] = temp & 0xff;
            UARTFunc::uart_putcmd(ChanNo,SetTempCmd1,uw16_len);
        }
        break;

        case 1:
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][SECOND_CHAMBER_CHANNEL];
            SetTempCmd2[uw16_len - 2] = (temp >> 8) & 0xff;
            SetTempCmd2[uw16_len - 1] = temp & 0xff;
            UARTFunc::uart_putcmd(ChanNo,SetTempCmd2,uw16_len);
        }
        break;

        case 2:
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][THIRD_CHAMBER_CHANNEL];
            SetTempCmd3[uw16_len - 2] = (temp >> 8) & 0xff;
            SetTempCmd3[uw16_len - 1] = temp & 0xff;
            UARTFunc::uart_putcmd(ChanNo,SetTempCmd3,uw16_len);
        }
        break;

        default:
            break;
        }
    }

    return;
}

void TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        m_RecvDataType = SETPOINTVALUE_TEMPERATURE;
        unsigned char ReadTempCmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x03,0x00,0x00,0x00,0x09};//读取3个通道温度值
        unsigned short uw16_len = sizeof(ReadTempCmd);

        UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
    }

    return;
}

void TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    unsigned char ChamberRunCmd1[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x26,0x00,0x01};//上箱运行
    unsigned char ChamberRunCmd2[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x30,0x00,0x01};//中箱运行
    unsigned char ChamberRunCmd3[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x2C,0x00,0x01};//下箱运行

    unsigned short uw16_len = sizeof(ChamberRunCmd1);

    switch(ChamberChn)
    {
    case 0:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberRunCmd1,uw16_len);
    }
    break;

    case 1:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberRunCmd2,uw16_len);
    }
    break;

    case 2:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberRunCmd3,uw16_len);
    }
    break;

    default:
        break;
    }
}

void TC_HARDING_THREE_LAYERS_MODBUS_TCP::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    unsigned char ChamberStopCmd1[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x26,0x00,0x02};//上箱停止
    unsigned char ChamberStopCmd2[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x30,0x00,0x02};//中箱停止
    unsigned char ChamberStopCmd3[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x02,0x2C,0x00,0x02};//下箱停止

    unsigned short uw16_len = sizeof(ChamberStopCmd1);

    switch(ChamberChn)
    {
    case 0:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberStopCmd1,uw16_len);
    }
    break;

    case 1:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberStopCmd2,uw16_len);
    }
    break;

    case 2:
    {
        UARTFunc::uart_putcmd(ChanNo,ChamberStopCmd3,uw16_len);
    }
    break;

    default:
        break;
    }

    return;
}



