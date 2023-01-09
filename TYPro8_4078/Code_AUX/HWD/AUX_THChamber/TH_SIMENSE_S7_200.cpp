/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

union{
	float f_val;
	unsigned char uc_buf[4];
}u_FloatToHex;

void TH_SIMENSE_S7_200::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SIMENSE_S7_200::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SIMENSE_S7_200::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SIMENSE_S7_200::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SIMENSE_S7_200::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SIMENSE_S7_200::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SIMENSE_S7_200::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}

void TH_SIMENSE_S7_200::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;
		unsigned int uw16_Total;
	
		uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_SIMENSE_S7_200_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护
		
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01) && (MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03) && (MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x04))
        {
						u_FloatToHex.uc_buf[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+6];
						u_FloatToHex.uc_buf[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
						u_FloatToHex.uc_buf[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
						u_FloatToHex.uc_buf[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
					
            PresentValue = u_FloatToHex.f_val*10;

            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,uc_Temp4Humi_Flag,FIRST_CHAMBER_CHANNEL);
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i < uw16_Total) && (uw16_i < MAX_UART_BUF_SIZE - 3));
		
		return;
}

void TH_SIMENSE_S7_200::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SetTempCmd[] = {0x01,0x06,0x00,0x64,0x41,0xB0,0x00,0x00,0x83,0xD4};//设定温度通道值
    unsigned char SetHumiCmd[] = {0x01,0x06,0x00,0x6A,0x41,0xB0,0x00,0x00,0xEA,0x15};//设定湿度通道值
    unsigned short crc16;
    unsigned short uw16_len = sizeof(SetTempCmd);
    short temp;

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
				u_FloatToHex.f_val = temp/10;
        SetTempCmd[4] = u_FloatToHex.uc_buf[3];
        SetTempCmd[5] = u_FloatToHex.uc_buf[2];
				SetTempCmd[6] = u_FloatToHex.uc_buf[1];
				SetTempCmd[7] = u_FloatToHex.uc_buf[0];

        crc16 = MTCI_datatypes::CRC16(SetTempCmd,8);
        SetTempCmd[8] = (unsigned char) (crc16 >>8);
        SetTempCmd[9] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
				u_FloatToHex.f_val = temp/10;
        SetHumiCmd[4] = u_FloatToHex.uc_buf[3];
        SetHumiCmd[5] = u_FloatToHex.uc_buf[2];
				SetHumiCmd[6] = u_FloatToHex.uc_buf[1];
				SetHumiCmd[7] = u_FloatToHex.uc_buf[0];

        crc16 = MTCI_datatypes::CRC16(SetHumiCmd,8);
        SetHumiCmd[8] = (unsigned char) (crc16 >>8);
        SetHumiCmd[9] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,SetHumiCmd,uw16_len);
    }
		
		return;
}

void TH_SIMENSE_S7_200::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ReadTempCmd[] = {0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};//读取1个通道温度值
    unsigned char ReadHumiCmd[] = {0x01,0x03,0x00,0x0C,0x00,0x02,0x04,0x08};//读取1个通道流量值
    unsigned short crc16;
    unsigned short uw16_len = sizeof(ReadTempCmd);

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        crc16 = MTCI_datatypes::CRC16(ReadTempCmd,6);
        ReadTempCmd[6] = (unsigned char) (crc16 >>8);
        ReadTempCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        crc16 = MTCI_datatypes::CRC16(ReadHumiCmd,6);
        ReadHumiCmd[6] = (unsigned char) (crc16 >>8);
        ReadHumiCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_putcmd(ChanNo,ReadHumiCmd,uw16_len);
    }
		
		return;
}

void TH_SIMENSE_S7_200::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x06,0x00,0xC8,0x00,0x01,0xC9,0xF4};//启动定值
    unsigned short crc16;
    unsigned short uw16_len = sizeof(MbBuf);

    crc16 = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (crc16 >>8);
    MbBuf[7] = (unsigned char) (crc16 & 0xff);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
		
		return;
}

void TH_SIMENSE_S7_200::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x06,0x00,0xC8,0x00,0x02,0x89,0xF5};//停止定值
    unsigned short crc16;
    unsigned short uw16_len = sizeof(MbBuf);

    crc16 = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (crc16 >>8);
    MbBuf[7] = (unsigned char) (crc16 & 0xff);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
		
		return;
}



