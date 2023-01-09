/*****************************************************************************
// 程序文件      :TH_ESPEC_SCP220.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Cotroller board服务
// 编写日期      :2011.12.01
//Add Humidity by yuxiong lu 04/18/2019
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];




void TH_SCP220::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SCP220::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SCP220::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SCP220::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SCP220::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SCP220::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SCP220::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/***************************************************************
*   Parse the data come back from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SCP220::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    //unsigned char *echo;
    unsigned char *pCmd;
    unsigned char *pCmdtemp;
    bool NoZero = 0;
    bool b_is_temp = NoZero;

//     if( UARTFunc::uart_bytenum(uc_ChNo) < TH_SCP220_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer

//         return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    MTCI_CTRL::gR_CmdBuf[TH_SCP220_MAX_RESPONSE_CMD_LEN+1]=0;

    pCmd = MTCI_CTRL::gR_CmdBuf;
    pCmdtemp = MTCI_CTRL::gR_CmdBuf;

    b_is_temp = (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ); //zl

    NoZero = (*pCmd) != 0; //zl

    bool tempflag = false;
    while( (!(b_is_temp ))&& (*pCmd!=0)) {
        pCmd++; //zl
        pCmdtemp++;
    }

    for (int i = 0; i < 6; i++) {
        if ((*pCmdtemp) == '.')
        {
            tempflag = true;
        }
        pCmdtemp++;
    }


    // while( (*pCmd) <= 0x20 && (*pCmd) != 0 ) pCmd++; /*ignore preblank*/

    if( (*pCmd) == 0 )
        return;

    if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
    {
        unsigned char* p;
        short int PresentValue = 0;
        p=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);
        if( p == NULL )
            return;
        if ( uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE && tempflag) {
            MTCI_CTRL::SetPresentValue(uc_ChNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
            //my_humflag = false;
        }
        else if ( uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI && (!(tempflag)) ) {
            MTCI_CTRL::SetPresentValue(uc_ChNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
            //my_temflag = false;
        }


        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);
    }

    else
    {
        return;
    }
}


/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SCP220::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char  *p;
    const char CmdBufTem[SEND_MAX_BUF]="TEMP,S22.0\r\n";

    unsigned char  *q;
    const char CmdBufHum[SEND_MAX_BUF]="HUMI,S100.0 H100.0 L0.0\r\n";


    if (uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE) {
        short SetPointValue=MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
        unsigned short Count = 0;


        //strcpy(CmdBuf,"TEMP,S22.0\r\n");  //ZL, for SCP_220, accordting to Dr. Xie
        //p=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,CmdBuf+6);
        p = (unsigned char*) &CmdBufTem[6];
        Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p);
        p = (unsigned char*)&CmdBufTem[6 + Count];

        *p='\r';
        p++;
        *p = '\n';
        p++;  //ZL, for SCP_220, accordting to Dr. Xie
        *p=0;
        p = (unsigned char*)&CmdBufTem[0];
        UARTFunc::uart_putcmd(uc_ChNo,p,strlen(CmdBufTem));     // Let UART send whatever in the buffer out
        //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo]=SETPOINTVALUE_HUMI;
    }
    else if (uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI) {
        short SetPointValue=MTCI_CTRL::gW_HumiValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
        unsigned short Count = 0;


        //strcpy(CmdBuf,"TEMP,S22.0\r\n");  //ZL, for SCP_220, accordting to Dr. Xie
        //p=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,CmdBuf+6);
        q = (unsigned char*) &CmdBufHum[6];
        Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,q);
        q = (unsigned char*)&CmdBufHum[6 + Count];

        *q='\r';
        q++;
        *q = '\n';
        q++;  //ZL, for SCP_220, accordting to Dr. Xie
        *q=0;
        q = (unsigned char*)&CmdBufHum[0];
        UARTFunc::uart_putcmd(uc_ChNo,q,strlen(CmdBufHum));     // Let UART send whatever in the buffer out
        //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo]= SETPOINTVALUE_TEMPERATURE;
    }
    //float SetPointValue=temperature;


    //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
    return;
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SCP220::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    const char CmdBuf[SEND_MAX_BUF]="MODE,CONSTANT\r\n\0\0" ;
    pCmd=(unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));

    //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作

    /*if (MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] == SETPOINTVALUE_TEMPERATURE) {
    	MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_HUMI;
    }
    else if(MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] == SETPOINTVALUE_HUMI) {
    	MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_TEMPERATURE;
    }*/
    return;
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SCP220::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    const char CmdBufTem[SEND_MAX_BUF]="TEMP?\r\n\0\0" ;
    const char CmdBufHum[SEND_MAX_BUF]="HUMI?\r\n\0\0" ;
    if (uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE) {
        pCmd = (unsigned char*) &CmdBufTem[0];
        UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBufTem));
        //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_HUMI;
    }
    else if (uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI) {
        pCmd = (unsigned char*) &CmdBufHum[0];
        UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBufHum));
        //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_TEMPERATURE;
    }
    //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
    return;
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TH_SCP220::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    const char CmdBuf[SEND_MAX_BUF]="MODE,STANDBY\r\n\0\0";
    //pCmd="MODE,STANDBY\r\n\0\0" ;
    pCmd=(unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));

    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作

    /*if (MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] == SETPOINTVALUE_TEMPERATURE) {
    	MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_HUMI;
    }
    else if(MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] == SETPOINTVALUE_HUMI) {
    	MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_TEMPERATURE;
    }*/
    return;

}

