/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TH_PLC_700_ASCII::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_PLC_700_ASCII::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_PLC_700_ASCII::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_PLC_700_ASCII::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_PLC_700_ASCII::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_PLC_700_ASCII::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_PLC_700_ASCII::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TH_PLC_700_ASCII::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    char *TempPtr = NULL;
    char *HumPtr = NULL;
    short PresentValue = 0;
//    unsigned short int uw16_i = 0;
		unsigned int uw16_Total;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    TempPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"TR=");
    HumPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"HR=");

    if(TempPtr != NULL)//读温度
    {
        PresentValue = (short)(atof((const char*)(TempPtr + 3))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
    else if(HumPtr != NULL)//读湿度
    {
        PresentValue = (short)(atof((const char*)(HumPtr + 3))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
		
		return;
}


void TH_PLC_700_ASCII::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned short uw16_len = 0;

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//温度
    {
        unsigned char TempSetBuf[20] = {"T"};//设定温度通道值

        short temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        unsigned char SetBuf[10] = {0};

        sprintf((char *)SetBuf,"%04d\r\n",temp);
        strncat((char *)TempSetBuf+1,(char *)SetBuf,strlen((char *)SetBuf));

        uw16_len = strlen((char*)TempSetBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,TempSetBuf,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//湿度
    {
        unsigned char HumSetBuf[20] = {"H"};//设定温度通道值

        short temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        unsigned char SetBuf[10] = {0};

        sprintf((char *)SetBuf,"%02d\r\n",temp);
        strncat((char *)HumSetBuf+1,(char *)SetBuf,strlen((char *)SetBuf));

        uw16_len = strlen((char*)HumSetBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,HumSetBuf,uw16_len);
    }
		
		return;
}

void TH_PLC_700_ASCII::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {"T?\r\n"};//停止
    unsigned short uw16_len = strlen((char*)SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
		
		return;
}

void TH_PLC_700_ASCII::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//
    {
        unsigned char SendBuf[] = {"T?\r\n"};//读温度
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//
    {
        unsigned char SendBuf[] = {"H?\r\n"};//读湿度
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
		
		return;
}

void TH_PLC_700_ASCII::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {"T?\r\n"};//停止
    unsigned short uw16_len = strlen((char*)SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
		
		return;
}

