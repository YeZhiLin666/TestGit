/*****************************************************************************
// 程序文件      :TC_WATLOW_F4T_MODBUS_FLOAT.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Controller Board服务
// 编写日期      :2011.12.01
*****************************************************************************/

#include "includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

union
{
	unsigned char uc_Buf[4];
	float fval;
}FloatToU8;

void TC_WATLOW_F4T_MODBUS_FLOAT::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    // 防止死机情况，先全部初始化一次函数指针。
    for(unsigned char uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_WATLOW_F4T_MODBUS_FLOAT::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TC_WATLOW_F4T_MODBUS_FLOAT::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;
	
		if( UARTFunc::uart_bytenum(uc_ChNo) < TC_WATLOW_F4T_MODBUS_FLOAT_MIN_RESPONSE_CMD_LEN) 
			return;

    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);
	
    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01 && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03 && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x04) // && gR_CmdBuf[uw16_i+7] == 0x00)  2009.07.25     //only parse the read cmd with 2 bytes data
        {
						FloatToU8.uc_Buf[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
						FloatToU8.uc_Buf[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
						FloatToU8.uc_Buf[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
						FloatToU8.uc_Buf[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+6];
					
            PresentValue = FloatToU8.fval*10;

            MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);    //KL update present value of the data read from chamber

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
            return;
        }

        uw16_i++;
    } while(uw16_i < TC_WATLOW_F4T_MODBUS_FLOAT_MIN_RESPONSE_CMD_LEN);
		
		return;
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_FLOAT::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned int  XChkSum = 0;
    unsigned char MbBuf[SEND_MAX_BUF];
    memset(MbBuf,0,sizeof(MbBuf));
    unsigned char XcrcHi,XcrcLo = 0;

    MbBuf[0] = 0x01;
    MbBuf[1] = 0x10;
    MbBuf[2] = 0x0A;
    MbBuf[3] = 0xDE;
		MbBuf[4] = 0x00;
		MbBuf[5] = 0x02;
		MbBuf[6] = 0x04;

		FloatToU8.fval = (float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10;
		
    MbBuf[7] 	= FloatToU8.uc_Buf[3];
    MbBuf[8] 	= FloatToU8.uc_Buf[2];
		MbBuf[9] 	= FloatToU8.uc_Buf[1];
		MbBuf[10] = FloatToU8.uc_Buf[0];
	
    XChkSum = MTCI_datatypes::CRC16(MbBuf,11);
    XcrcHi = (unsigned char)(XChkSum >> 8);
    XcrcLo = (unsigned char)(XChkSum & 255);
    MbBuf[11] = XcrcHi;
    MbBuf[12] = XcrcLo;

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,13);
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_FLOAT::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbr100 = 0x0B;
    unsigned char lbr100 = 0x04;
    unsigned char hbnrr = 0x00;
    unsigned char lbnrr = 0x02;
    unsigned int  MbChkSum = 0;
    unsigned char MbBuf[SEND_MAX_BUF];
    memset(MbBuf,0,sizeof(MbBuf));

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x03;
    MbBuf[2] = hbr100;
    MbBuf[3] = lbr100;
    MbBuf[4] = hbnrr;
    MbBuf[5] = lbnrr;
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (MbChkSum >>8);
    MbBuf[7] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_FLOAT::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbr100 = 0x0B;
    unsigned char lbr100 = 0x04;
    unsigned char hbnrr = 0x00;
    unsigned char lbnrr = 0x02;
    unsigned int  MbChkSum = 0;
    unsigned char MbBuf[SEND_MAX_BUF];
    memset(MbBuf,0,sizeof(MbBuf));

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x03;
    MbBuf[2] = hbr100;
    MbBuf[3] = lbr100;
    MbBuf[4] = hbnrr;
    MbBuf[5] = lbnrr;
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (MbChkSum >>8);
    MbBuf[7] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_WATLOW_F4T_MODBUS_FLOAT::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char slave_addr = 0x01;
    unsigned char hbr100 = 0x0B;
    unsigned char lbr100 = 0x04;
    unsigned char hbnrr = 0x00;
    unsigned char lbnrr = 0x02;
    unsigned int  MbChkSum = 0;
    unsigned char MbBuf[SEND_MAX_BUF];
    memset(MbBuf,0,sizeof(MbBuf));

    MbBuf[0] = slave_addr;
    MbBuf[1] = 0x03;
    MbBuf[2] = hbr100;
    MbBuf[3] = lbr100;
    MbBuf[4] = hbnrr;
    MbBuf[5] = lbnrr;
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (MbChkSum >>8);
    MbBuf[7] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);
}

