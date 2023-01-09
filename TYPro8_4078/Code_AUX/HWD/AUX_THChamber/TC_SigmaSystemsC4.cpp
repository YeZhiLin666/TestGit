/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

bool TC_SigmaSystemsC4::SetModeFlag;
bool TC_SigmaSystemsC4::StopCompressorFlag;

void TC_SigmaSystemsC4::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_SigmaSystemsC4::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_SigmaSystemsC4::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_SigmaSystemsC4::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_SigmaSystemsC4::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_SigmaSystemsC4::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_SigmaSystemsC4::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_SigmaSystemsC4::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    char *Temp0Ptr = NULL;
    char *Temp0FPtr = NULL;
		char *Temp1Ptr = NULL;
    char *Temp1FPtr = NULL;
    short PresentValue = 0;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    Temp0Ptr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"T0 ");
    Temp1Ptr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"T1 ");
		Temp0FPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"T0F ");
    Temp1FPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"T1F ");		

    if(Temp0Ptr != NULL)//读温度
    {
        PresentValue = (short)(atof((const char*)(Temp0Ptr + 3))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
    else if(Temp1Ptr != NULL)//读温度
    {
        PresentValue = (short)(atof((const char*)(Temp1Ptr + 3))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, SECOND_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
    else if(Temp0FPtr != NULL)//读温度
    {
        PresentValue = (short)(atof((const char*)(Temp0FPtr + 4))*10);
				PresentValue = (((PresentValue/10) - 32)/1.8)*10;
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
    else if(Temp1FPtr != NULL)//读温度
    {
        PresentValue = (short)(atof((const char*)(Temp1FPtr + 4))*10);
				PresentValue = (((PresentValue/10) - 32)/1.8)*10;
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, SECOND_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }		
}


void TC_SigmaSystemsC4::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned short uw16_len = 0;

//    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//温度
    {
        unsigned char TempSetBuf[20] = {"GT "};//设定温度通道值

        short temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        float fSetPoint = (float)temp/10;
        unsigned char SetBuf[10] = {0};

        sprintf((char *)SetBuf,"%.1f\r\n",fSetPoint);
        strncat((char *)TempSetBuf+3,(char *)SetBuf,strlen((char *)SetBuf));

        uw16_len = strlen((char*)TempSetBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,TempSetBuf,uw16_len);
    }
}


void TC_SigmaSystemsC4::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {"SP\r\n"};//远程模式
    unsigned short uw16_len = strlen((char*)SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);

    SetModeFlag = true;
}

void TC_SigmaSystemsC4::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    if(SetModeFlag)
    {
        SetModeFlag = false;
        unsigned char SendBuf[] = {"CO\r\n"};//启动
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);

        return;
    }
		else if(StopCompressorFlag)
		{
        StopCompressorFlag = false;
        unsigned char SendBuf[] = {"QU\r\n"};//关闭
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);

        return;			
		}

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//
    {
        unsigned char SendBuf[] = {"PT 0\r\n"};//读温度0
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//
    {
        unsigned char SendBuf[] = {"PT 1\r\n"};//读温度1
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
}

void TC_SigmaSystemsC4::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {"CF\r\n"};//停止
    unsigned short uw16_len = strlen((char*)SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
		
		StopCompressorFlag = true;
}

