/*****************************************************************************
// 程序文件      :TC_WATLOW_F4.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Controller Board服务
// 编写日期      :2011.12.01
*****************************************************************************/



#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TC_WATLOW_F4::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_WATLOW_F4::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_WATLOW_F4::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_WATLOW_F4::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_WATLOW_F4::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_WATLOW_F4::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_WATLOW_F4::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TC_WATLOW_F4::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_WATLOW_F4_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01 && MTCI_CTRL::gR_CmdBuf[1] == 0x03 && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x02) // && gR_CmdBuf[uw16_i+7] == 0x00)  2009.07.25     //only parse the read cmd with 2 bytes data
        {
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );     //KL combined

            MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);    //KL update present value of the data read from chamber

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
            return;
        }
        // 	else
        //     {
        // 		return;
        // 	}

        uw16_i++;
    } while(uw16_i < TC_WATLOW_F4_MIN_RESPONSE_CMD_LEN);
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbspr300 = 0x01;
    unsigned char lbspr300 = 0x2C;
    short int  register_data;
    unsigned int  XChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned char XcrcHi,XcrcLo;

    //         #if(TC_WATLOW_F4_INPUT_CH == TC_WATLOW_F4_INPUT_THIRD)
    //         hbspr300 = 0x02;                   // 20150324  YY
    //         lbspr300 = 0x72;
    //         #else
    hbspr300 = 0x01;
    lbspr300 = 0x2C;
    //         #endif

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x06;
    MbBuf[2] = hbspr300;
    MbBuf[3] = lbspr300;

    register_data = MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[4] = (unsigned char) (register_data >> 8);
    MbBuf[5] = (unsigned char) (register_data & 255);
    XChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    XcrcHi = (unsigned char)(XChkSum >> 8);
    XcrcLo = (unsigned char)(XChkSum & 255);
    MbBuf[6] = XcrcHi;
    MbBuf[7] = XcrcLo;

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);

    //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
#if(TC_WATLOW_F4_INPUT_CH == TC_WATLOW_F4_INPUT_THIRD)
    unsigned char CmdBuf[SEND_MAX_BUF] = "\x01\x03\x00\x6C\x00\x01\x44\x17";
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,8);      // 20150324  YY
#else
    unsigned char CmdBuf[SEND_MAX_BUF] = "\x01\x03\x00\x64\x00\x01\xC5\xD5";
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,8);
#endif

    //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbr100 = 0x00;
    unsigned char lbr100 = 0x64;
    unsigned char hbnrr = 0x00;
    unsigned char lbnrr = 0x01;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

#if(TC_WATLOW_F4_INPUT_CH == TC_WATLOW_F4_INPUT_THIRD)
    hbr100 = 0x00;                   // 20150324  YY
    lbr100 = 0x6C;
#else
    hbr100 = 0x00;
    lbr100 = 0x64;
#endif

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x03;
    MbBuf[2] = hbr100;
    MbBuf[3] = lbr100;
    MbBuf[4] = hbnrr;
    MbBuf[5] = lbnrr;
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (MbChkSum >>8);
    MbBuf[7] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8 );

    //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_WATLOW_F4::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbspr300 = 0x01;
    unsigned char lbspr300 = 0x2C;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    //     #if(TC_WATLOW_F4_INPUT_CH == TC_WATLOW_F4_INPUT_THIRD)
    //     hbspr300 = 0x02;                   // 20150324  YY
    //     lbspr300 = 0x72;
    //     #else
    hbspr300 = 0x01;
    lbspr300 = 0x2C;
    //     #endif

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x06;
    MbBuf[2] = hbspr300;
    MbBuf[3] = lbspr300;
    MbBuf[4] = 0x00;
    MbBuf[5] = 0xDC;

#if(TC_WATLOW_F4_INPUT_CH == TC_WATLOW_F4_INPUT_THIRD)
    MbBuf[4] = 0x00;                // 20150324  YY
    MbBuf[5] = 0x03;
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (MbChkSum >> 8);
    MbBuf[7] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);

    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

