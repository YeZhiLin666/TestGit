/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TC_SIMPAC::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_SIMPAC::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_SIMPAC::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_SIMPAC::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_SIMPAC::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_SIMPAC::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_SIMPAC::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}


void TC_SIMPAC::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    short PresentValue = 0;
    unsigned short int uw16_i = 0;
	
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    do 
		{
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x31) && (MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 0xb6))//读数据
        {
						PresentValue = (short)(atof((const char*)(MTCI_CTRL::gR_CmdBuf + uw16_i + 2))*10);
						MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
            
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
            return;
				}

        uw16_i++;
    } while((MTCI_CTRL::gR_CmdBuf[uw16_i] != 0) && (uw16_i + 1 < sizeof(MTCI_CTRL::gR_CmdBuf))) ;
}


void TC_SIMPAC::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char SendBuf[20] = {0x31,0x31,0x30,0x30,0x31,0xb6,0x31,0xb6,0x31,0xb6};//设定温度通道值
	unsigned short uw16_len = 0;

	short temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
	float fSetPoint = (float)temp/10;
	unsigned char SetBuf[10] = {0};

	sprintf((char *)SetBuf,"%.1f\r",fSetPoint);
	strncat((char *)SendBuf+10,(char *)SetBuf,strlen((char *)SetBuf));

	uw16_len = 10 + strlen((char *)SetBuf);
	
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
}


void TC_SIMPAC::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char SendBuf[] = {0x31,0x34,0x30,0x30,0x31,0xb6,0x31,0xb6,0x31,0xb6,0x31,0x0d};//启动
	unsigned short uw16_len = sizeof(SendBuf);
	
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
}

void TC_SIMPAC::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char SendBuf[] = {0x31,0x31,0x30,0x30,0x34,0xb6,0x31,0xb6,0x31,0x0d};//读取
	unsigned short uw16_len = sizeof(SendBuf);
	
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
}

void TC_SIMPAC::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char SendBuf[] = {0x31,0x34,0x30,0x30,0x31,0xb6,0x31,0xb6,0x31,0xb6,0x30,0x0d};//停止
	unsigned short uw16_len = sizeof(SendBuf);
	
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
}




