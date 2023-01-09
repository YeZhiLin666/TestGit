/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TC_TEMI2500_MODBUS2::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_TEMI2500_MODBUS2::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_TEMI2500_MODBUS2::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_TEMI2500_MODBUS2::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_TEMI2500_MODBUS2::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_TEMI2500_MODBUS2::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_TEMI2500_MODBUS2::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_TEMI2500_MODBUS2::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TC_TEMI2500_MODBUS2_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护
		
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do 
		{
				if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01) && (MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03) && (MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x02))
				{
					PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+3] << 8) & 0xff00;//高字节
					PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+4] & 0xff;//低字节
					PresentValue /= 10;
					
					MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);
					CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

					return;
				}

        uw16_i++;
    } while((uw16_i < uw16_Total) && (uw16_i < MAX_UART_BUF_SIZE - 3));
}



void TC_TEMI2500_MODBUS2::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char SetTempCmd[] = {0x01,0x06,0x00,0x03,0x00,0x00,0xff,0xff};//设定温度通道值
	unsigned short crc16;
	unsigned short uw16_len = sizeof(SetTempCmd);
	short temp;
	
	if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
	{
		temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL]*10;
		SetTempCmd[4] = (temp >> 8) & 0xff;
		SetTempCmd[5] = temp & 0xff;
	
		crc16 = MTCI_datatypes::CRC16(SetTempCmd,6);
		SetTempCmd[6] = (unsigned char) (crc16 >>8);
		SetTempCmd[7] = (unsigned char) (crc16 & 0xff);

		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
		UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
	}
}

void TC_TEMI2500_MODBUS2::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char ReadTempCmd[] = {0x01,0x03,0x00,0x02,0x00,0x01,0xff,0xff};//读取1个通道温度值
	unsigned short crc16;
	unsigned short uw16_len = sizeof(ReadTempCmd);
	
	crc16 = MTCI_datatypes::CRC16(ReadTempCmd,6);
	ReadTempCmd[6] = (unsigned char) (crc16 >>8);
	ReadTempCmd[7] = (unsigned char) (crc16 & 0xff);

  CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
  UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
}

void TC_TEMI2500_MODBUS2::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char MbBuf[] = {0x01,0x06,0x00,0x01,0x00,0x03,0xff,0xff};//启动定值
	unsigned short crc16;
	unsigned short uw16_len = sizeof(MbBuf);
	
	crc16 = MTCI_datatypes::CRC16(MbBuf,6);
	MbBuf[6] = (unsigned char) (crc16 >>8);
	MbBuf[7] = (unsigned char) (crc16 & 0xff);

  CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
  UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TC_TEMI2500_MODBUS2::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char MbBuf[] = {0x01,0x06,0x00,0x01,0x00,0x02,0xff,0xff};//停止定值
	unsigned short crc16;
	unsigned short uw16_len = sizeof(MbBuf);
	
	crc16 = MTCI_datatypes::CRC16(MbBuf,6);
	MbBuf[6] = (unsigned char) (crc16 >>8);
	MbBuf[7] = (unsigned char) (crc16 & 0xff);

  CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
  UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



