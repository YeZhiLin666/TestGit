/*****************************************************************************
// 程序文件      :TC_KSI_THS2005.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Cotroller board服务
// 编写日期      :2012.01.06
*****************************************************************************/


#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];






void TC_THS2005::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_THS2005::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_THS2005::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_THS2005::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_THS2005::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_THS2005::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_THS2005::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/***************************************************************
*   Parse the data come back from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TC_THS2005::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{

    unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_THS2005_MIN_RESPONSE_CMD_LEN) return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < 5*TC_THS2005_MAX_COM_READ_REPLY; uw16_i ++)
    {   //  ????400??200??  // Temperature range -200癈 to +400癈 (-328癋 to +725癋) //Resolution 0.1癈 or 癋, 0.1% RH, or 0.01 for other linear applications
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 'S')&&(MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 'T')&&(MTCI_CTRL::gR_CmdBuf[uw16_i + 2] == 'X'))  // ??STX,????????
        {
            //  STX,1,0,A,TYPE,STATUS,TEMP_PV,HUMI_PV,TEMP_SV,HUMI_SV,PGM_NAME,CYCLE,STEP,HOUR,MIN,ERROR,END
            //  STX,1,0,A,6,1,22.55,0,22.00,0,ARB.PGM,1,1,0,2,0,END
            //  ?14??? 22.55 TEMP_PV value
            uc_MoveFlag = 1;
            uw16_j = 0;
        }

        if(uc_MoveFlag == 1)
        {
            if(MTCI_CTRL::gR_CmdBuf[uw16_i] == ',')   //  ??????????
            {
                uw16_j ++;
            }

            if(uw16_j == 3)
            {
                if(MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 'A')  //  Is this the response from "STX,0,1,A,END"
                {

                    uw16_k = uw16_i+7;;
                    pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_k]; //DATA FROM CHAMBER
                    p = pCmd; //DATA FROM CHAMBER

                    if((*pCmd) == 0 )    //  if Null, then give up this reading
                        return;
                    if( (*pCmd)==' ')	 //	 for temperature less than 100, the first byte will be a space, just skip this space
                        pCmd++;

                    if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
                    {
                        p=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);

                        if( p == NULL )
                            return;

                        MTCI_CTRL::SetPresentValue(uc_ChNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1
                        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);
                        return;

                    }
                }
                else
                {
                    return;
                }

            }

        }
    }

}


/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_THS2005::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{   unsigned int j;
    unsigned char *pCmd;
    char CmdBuf[SEND_MAX_BUF];
//float SetPointValue=temperature;
    short SetPointValue;
    unsigned char p2[10];
    char  *p1;
    unsigned short Count = 0;
    SetPointValue = MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];

    pCmd = (unsigned char*) &CmdBuf[0];
    strcpy(CmdBuf,"STX,0,1,W,one-poin.pgm,1,2,150,-70,");  /* STX,0,1,W. PGM_NAME,Cycle, Step, High Limit, Low Limit */

//p1=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p2);
    Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p2);
    p1 = (char*) &p2[Count];
    /*p=ctrl_Value2String(SetPointValue,CmdBuf+6);  */
    strcat(CmdBuf,(char*)p2);     /* temperature set value  */
    strcat(CmdBuf,",0,0,0,0,0,0,0,");  /* humid, hour, min, p1, p2, p3, th */
    strcat(CmdBuf,(char*)p2);     /* Repeat temperature set value */
    strcat(CmdBuf,",0,999,59,1,1,1,0,END\n\r\0\0");  /* humid, hour, min, p1, p2, p3, th */
    pCmd = (unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));
    for ( j=0; j<100; j++ )					 // delay for THS2005 to response to W command  j cannot be less than 80
    {
        DELAY2(5000);
    }

//MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_THS2005::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    char CmdBuf[SEND_MAX_BUF]="STX,0,1,S,one-poin.pgm,END\n\r\0\0";		/* Start Program, STX,0,1,S,PGM_NAME,END  */
    unsigned int j;

    pCmd = (unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));

    for ( j=0; j<400; j++) {							// delay for THS2005 to response to S command. j must be greater than 380
        DELAY2(5000);
    }

    //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作


}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_THS2005::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    //char i;
    unsigned char *pCmd;
    char CmdBuf[SEND_MAX_BUF] = "STX,0,1,A,END\n\r\0\0" ;
    DELAY2(4000);
    pCmd = (unsigned char*) &CmdBuf[0];
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));
    DELAY2(4000);

    //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_THS2005::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char *pCmd;
    //pCmd="MODE,STANDBY\r\n\0\0" ;
    char CmdBuf[SEND_MAX_BUF] = "STX,0,1,W,one-poin.pgm,1,2,150,-70,22,0,0,0,0,0,0,0,22,0,1000,59,0,0,0,0,END";
    pCmd=(unsigned char*) &CmdBuf[0];
    //pCmd="STX,0,1,E,END\r\n\0\0";
    UARTFunc::uart_putcmd(uc_ChNo,pCmd,strlen(CmdBuf));
    //DELAY2(400);

    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作

}


