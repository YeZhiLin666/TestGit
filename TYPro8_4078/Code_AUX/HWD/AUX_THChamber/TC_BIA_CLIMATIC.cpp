/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

bool TC_BIA_CLIMATIC::IsNeedSendChamber1RunCmd2;
bool TC_BIA_CLIMATIC::IsNeedSendChamber2RunCmd2;
bool TC_BIA_CLIMATIC::IsNeedSendChamber1StopCmd2;
bool TC_BIA_CLIMATIC::IsNeedSendChamber2StopCmd2;

void TC_BIA_CLIMATIC::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_BIA_CLIMATIC::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_BIA_CLIMATIC::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_BIA_CLIMATIC::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_BIA_CLIMATIC::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_BIA_CLIMATIC::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_BIA_CLIMATIC::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_BIA_CLIMATIC::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
				if((MTCI_CTRL::gR_CmdBuf[uw16_i%MAX_UART_BUF_SIZE] == 0x05) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+1)%MAX_UART_BUF_SIZE] == 0x01) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+2)%MAX_UART_BUF_SIZE] == 0x03) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+3)%MAX_UART_BUF_SIZE] == 0x02))
				{
						PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+4] << 8) & 0xff00;//高字节
						PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+5] & 0xff;//低字节

            if(ChamberChn == 0)
                MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);
            else if(ChamberChn == 1)
                MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE,SECOND_CHAMBER_CHANNEL);
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i + 5 < uw16_Total) && (uw16_i + 5 < MAX_UART_BUF_SIZE));
}

void TC_BIA_CLIMATIC::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SetTemp1Cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x3B,0x00,0x00};//设定温度通道值1
    unsigned char SetTemp2Cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x9F,0x00,0x00};//设定温度通道值2
    unsigned short uw16_len = sizeof(SetTemp1Cmd);
    short temp;
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(ChamberChn == 1)
    {
				//运行命令和停止命需要连续两个报文交替发送，因为握手信号 50.10 位需要 0/1 之间切换
        if(IsNeedSendChamber2RunCmd2 || IsNeedSendChamber2StopCmd2 || IsNeedSendChamber1RunCmd2 || IsNeedSendChamber1StopCmd2)
        {
            return;
        }
        else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][SECOND_CHAMBER_CHANNEL];
            SetTemp2Cmd[uw16_len-2] = (temp >> 8) & 0xff;
            SetTemp2Cmd[uw16_len-1] = temp & 0xff;

            UARTFunc::uart_putcmd(ChanNo,SetTemp2Cmd,uw16_len);
        }
    }
    else if(ChamberChn == 0)
    {
				//运行命令和停止命需要连续两个报文交替发送，因为握手信号 50.10 位需要 0/1 之间切换
        if(IsNeedSendChamber2RunCmd2 || IsNeedSendChamber2StopCmd2 || IsNeedSendChamber1RunCmd2 || IsNeedSendChamber1StopCmd2)
        {
            return;
        }
        else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
        {
            temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
            SetTemp1Cmd[uw16_len-2] = (temp >> 8) & 0xff;
            SetTemp1Cmd[uw16_len-1] = temp & 0xff;

            UARTFunc::uart_putcmd(ChanNo,SetTemp1Cmd,uw16_len);
        }
    }
}

void TC_BIA_CLIMATIC::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ReadTemp1Cmd[] 	= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x03,0x00,0x0D,0x00,0x01};//读取温度通道1
    unsigned char ReadTemp2Cmd[] 	= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x03,0x00,0x71,0x00,0x01};//读取温度通道2
    unsigned char Run1cmd[] 			= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x32,0x04,0x03};//启动定值1
    unsigned char Run2cmd[] 			= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x96,0x04,0x03};//启动定值2
    unsigned char Stop1cmd[] 			= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x32,0x00,0x01};//停止定值1
    unsigned char Stop2cmd[] 			= {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x96,0x00,0x01};//停止定值2
    unsigned short uw16_len = sizeof(ReadTemp1Cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberReadTempCount[ChanNo];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(ChamberChn == 1)
    {
				//运行命令和停止命需要连续两个报文交替发送，因为握手信号 50.10 位需要 0/1 之间切换
        if(IsNeedSendChamber2RunCmd2 || IsNeedSendChamber2StopCmd2)
        {
            if(IsNeedSendChamber2RunCmd2)
            {
                IsNeedSendChamber2RunCmd2 = false;
                UARTFunc::uart_putcmd(ChanNo,Run2cmd,uw16_len);
            }
            else
            {
                IsNeedSendChamber2StopCmd2 = false;
                UARTFunc::uart_putcmd(ChanNo,Stop2cmd,uw16_len);
            }

            return;
        }
        else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
        {
            UARTFunc::uart_putcmd(ChanNo,ReadTemp2Cmd,uw16_len);
        }
    }
    else if(ChamberChn == 0)
    {
				//运行命令和停止命需要连续两个报文交替发送，因为握手信号 50.10 位需要 0/1 之间切换
        if(IsNeedSendChamber1RunCmd2 || IsNeedSendChamber1StopCmd2)
        {
            if(IsNeedSendChamber1RunCmd2)
            {
                IsNeedSendChamber1RunCmd2 = false;
                UARTFunc::uart_putcmd(ChanNo,Run1cmd,uw16_len);
            }
            else
            {
                IsNeedSendChamber1StopCmd2 = false;
                UARTFunc::uart_putcmd(ChanNo,Stop1cmd,uw16_len);
            }

            return;
        }
        else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
        {
            UARTFunc::uart_putcmd(ChanNo,ReadTemp1Cmd,uw16_len);
        }
    }
}

void TC_BIA_CLIMATIC::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Run1cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x32,0x00,0x03};//启动定值1
    unsigned char Run2cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x96,0x00,0x03};//启动定值2
    unsigned short uw16_len = sizeof(Run1cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberWriteTempCount[ChanNo];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(ChamberChn == 1)
    {
        UARTFunc::uart_putcmd(ChanNo,Run2cmd,uw16_len);
        IsNeedSendChamber2RunCmd2 = true;
    }
    else if(ChamberChn == 0)
    {
        UARTFunc::uart_putcmd(ChanNo,Run1cmd,uw16_len);
        IsNeedSendChamber1RunCmd2 = true;
    }
}

void TC_BIA_CLIMATIC::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Stop1cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x32,0x04,0x01};//停止定值1
    unsigned char Stop2cmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x00,0x96,0x04,0x01};//停止定值2
    unsigned short uw16_len = sizeof(Stop1cmd);
    unsigned char ChamberChn = MTCI_CTRL::PresentChamberWriteTempCount[ChanNo];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(ChamberChn == 1)
    {
        UARTFunc::uart_putcmd(ChanNo,Stop2cmd,uw16_len);
        IsNeedSendChamber2StopCmd2 = true;
    }
    else if(ChamberChn == 0)
    {
        UARTFunc::uart_putcmd(ChanNo,Stop1cmd,uw16_len);
        IsNeedSendChamber1StopCmd2 = true;
    }
}



