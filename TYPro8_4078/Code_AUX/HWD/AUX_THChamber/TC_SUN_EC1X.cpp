/*****************************************************************************
// 程序文件      :TC_SUN_EC1X.C    定义温控箱中各种控制命令文件
// 编写日期      :2019.05.14
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

int count = 0; //Read Command Control Var

void TC_SUN_EC1X::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_SUN_EC1X::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_SUN_EC1X::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_SUN_EC1X::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_SUN_EC1X::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_SUN_EC1X::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_SUN_EC1X::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/***************************************************************
*   Parse the data come back from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_SUN_EC1X::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    //unsigned char *echo;
    unsigned char *pCmd;
    bool NoZero = 0;
    bool b_is_temp = NoZero;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_SUN_EC1X_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    MTCI_CTRL::gR_CmdBuf[TC_SUN_EC1X_MAX_RESPONSE_CMD_LEN+1]=0;

    pCmd = MTCI_CTRL::gR_CmdBuf;

    b_is_temp = (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ); //zl

    NoZero = (*pCmd) != 0; //zl

    while( (!(b_is_temp ))&& (*pCmd!=0))
        pCmd++; //zl


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

        MTCI_CTRL::SetPresentValue(uc_ChNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);


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

void TC_SUN_EC1X::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    char CmdBuf[SEND_MAX_BUF] = {0};
		char temp[20] = {0};
		short int uw16_point = 0;

		strcpy(CmdBuf,"SET=");
		sprintf(temp,"%.1f",(float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10);
		strcat(CmdBuf,temp);
// 		strcat(CmdBuf,"C\r\n\0\0");
		strcat(CmdBuf,"C\r\n");

        uw16_point = strlen(CmdBuf);
        if(uw16_point<(SEND_MAX_BUF-4))
		{
			  CmdBuf[uw16_point-3] = 0xAB;  //  171 0xAB 左角引号
			  CmdBuf[uw16_point-2] = '\r';  //  
			  CmdBuf[uw16_point-1] = '\n';  // 
			  CmdBuf[uw16_point] = '\0';  // 
		}

    UARTFunc::uart_putcmd(uc_ChNo,(unsigned char *)CmdBuf,strlen(CmdBuf));     // Let UART send whatever in the buffer out
    return;
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_SUN_EC1X::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    /*unsigned char *pCmd;
    const char CmdBuf[SEND_MAX_BUF]="MODE,CONSTANT\r\n\0\0" ;
    pCmd=(unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));  */

    //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
    return;
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_SUN_EC1X::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd = NULL;
    const char CmdBuf[SEND_MAX_BUF]="TEMP?\r\n\0\0" ;
    pCmd = (unsigned char*) &CmdBuf[0];
    count++;
    //if (count >10) {
		if (count >5) {
        UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));
        count = 0;
     }
    return;
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_SUN_EC1X::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    const char CmdBuf[SEND_MAX_BUF]="STOP\r\n\0\0";
    //pCmd="MODE,STANDBY\r\n\0\0" ;
    pCmd=(unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));

    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
    return;

}