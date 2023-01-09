#include "../../Entry/includes.h"

#define STOP 	0
#define RUN 	1

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

float TH_SIEMENS_S7_XC1001A_SAIA::UcharToFloat(unsigned char *buf)
{
	char tmp[4] = {0};

	tmp[0] = * (buf + 3);
	tmp[1] = * (buf + 2);
	tmp[2] = * (buf + 1);
	tmp[3] = * (buf + 0);

	return (* (float *) tmp);
}

void TH_SIEMENS_S7_XC1001A_SAIA::FloatToUchar(unsigned char *buf,float value)
{
	buf[0] = * ((char *) &value + 3);
	buf[1] = * ((char *) &value + 2);
	buf[2] = * ((char *) &value + 1);
	buf[3] = * ((char *) &value + 0);
}

void TH_SIEMENS_S7_XC1001A_SAIA::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SIEMENS_S7_XC1001A_SAIA::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}


/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TH_SIEMENS_S7_XC1001A_SAIA::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	short temp;
    short PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TH_SIEMENS_S7_XC1001A_SAIA_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    do 
		{
				if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x11 && MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 0x03 && MTCI_CTRL::gR_CmdBuf[uw16_i + 2] == 0x08)//读数据
				{
						temp = (short)(UcharToFloat(&MTCI_CTRL::gR_CmdBuf[uw16_i+3])*10);

						PresentValue = temp;
						MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

						temp = (short)(UcharToFloat(&MTCI_CTRL::gR_CmdBuf[uw16_i+7])*10);

						PresentValue = temp;
						MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
						
						CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
						return;
				}

				uw16_i++;
			} while(uw16_i < TH_SIEMENS_S7_XC1001A_SAIA_MAX_RESPONSE_CMD_LEN);
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SIEMENS_S7_XC1001A_SAIA::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char CmdBuf[13];
	unsigned short crc16;
	float temp; 
	
	CmdBuf[0] 		= 0x11;
	CmdBuf[1] 		= 0x10;

	CmdBuf[4] 		= 0x00;
	CmdBuf[5] 		= 0x02;
	CmdBuf[6] 		= 0x04;

	if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//温度
	{
		CmdBuf[2] 		= 0x01;
		CmdBuf[3] 		= 0xF8;
		
		temp = (float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10;
		FloatToUchar(CmdBuf+7,temp);
	}
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//湿度
	{
		CmdBuf[2] 		= 0x01;
		CmdBuf[3] 		= 0xFC;
		
		temp = (float)MTCI_CTRL::gW_HumiValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10;
		FloatToUchar(CmdBuf+7,temp);
	}
		
    crc16 = MTCI_datatypes::CRC16(CmdBuf,11);
    CmdBuf[11] = (unsigned char) (crc16 >>8);
    CmdBuf[12] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,13);
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SIEMENS_S7_XC1001A_SAIA::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char CmdBuf[8];
	unsigned short crc16;
	
	CmdBuf[0] 		= 0x11;
	CmdBuf[1] 		= 0x03;
	CmdBuf[2] 		= 0x00;
	CmdBuf[3] 		= 0x20;
	CmdBuf[4] 		= 0x00;
	CmdBuf[5] 		= 0x04;

    crc16 = MTCI_datatypes::CRC16(CmdBuf,6);
    CmdBuf[6] = (unsigned char) (crc16 >>8);
    CmdBuf[7] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,8);
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_SIEMENS_S7_XC1001A_SAIA::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char CmdBuf[11];
	unsigned short crc16;
	
	CmdBuf[0] 		= 0x11;
	CmdBuf[1] 		= 0x10;
	CmdBuf[2] 		= 0x01;
	CmdBuf[3] 		= 0xF4;
	CmdBuf[4] 		= 0x00;
	CmdBuf[5] 		= 0x01;
	CmdBuf[6] 		= 0x02;
	CmdBuf[7] 		= 0x03;//使能温湿度通道
	CmdBuf[8] 		= 0x01;//开启温箱

    crc16 = MTCI_datatypes::CRC16(CmdBuf,9);
    CmdBuf[9] = (unsigned char) (crc16 >>8);
    CmdBuf[10] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,11);
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TH_SIEMENS_S7_XC1001A_SAIA::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char CmdBuf[11];
	unsigned short crc16;
	
	CmdBuf[0] 		= 0x11;
	CmdBuf[1] 		= 0x10;
	CmdBuf[2] 		= 0x01;
	CmdBuf[3] 		= 0xF4;
	CmdBuf[4] 		= 0x00;
	CmdBuf[5] 		= 0x01;
	CmdBuf[6] 		= 0x02;
	CmdBuf[7] 		= 0x03;//使能温湿度通道
	CmdBuf[8] 		= 0x00;//关闭温箱

    crc16 = MTCI_datatypes::CRC16(CmdBuf,9);
    CmdBuf[9] = (unsigned char) (crc16 >>8);
    CmdBuf[10] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,11);
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}