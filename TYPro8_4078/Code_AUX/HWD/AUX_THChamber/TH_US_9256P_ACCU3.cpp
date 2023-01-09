/*****************************************************************************
// 程序文件      :TC_WATLOW_F4.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Controller Board服务
// 编写日期      :2011.12.01
*****************************************************************************/
#include "../../Entry/includes.h"

#define SEND_CMD_BUF_SIZE 8
#define STOP 	0
#define RUN 	1

enum field_name
{
	SLAVE_ADDR,
	FUNCTION_CODE,
	ADDR_HIGH,
	ADDR_LOW,
	DATA_HIGH,
	DATA_LOW,
	CRC_HIGH,
	CRC_LOW
};

unsigned char TH_US_9256P_ACCU3::chamber_state;

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


unsigned char TH_US_9256P_ACCU3::get_state(void)
{
	return chamber_state;
}

void TH_US_9256P_ACCU3::set_state(unsigned char state)
{
	chamber_state = state;
}

void TH_US_9256P_ACCU3::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_US_9256P_ACCU3::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_US_9256P_ACCU3::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_US_9256P_ACCU3::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_US_9256P_ACCU3::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_US_9256P_ACCU3::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_US_9256P_ACCU3::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TH_US_9256P_ACCU3::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		int temp;
    short PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TH_US_9256P_ACCU3_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    do 
		{
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01 && MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 0x03 && MTCI_CTRL::gR_CmdBuf[uw16_i + 2] == 0x02)//读数据
        {
            temp = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );     //KL combined
						
						if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
						{
							if(temp == 0xAA10)//没接温度干球传感器
							{
								;
							}
							else
							{
									PresentValue = temp;
									PresentValue /= 10;
								MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
							}
						}
						else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
						{
							if(temp == 0)//没接湿度干球传感器
							{
								;
							}
							else
							{
								PresentValue = temp;
								PresentValue /= 10;
								if(PresentValue > 1000)
									PresentValue = 1000;
								else if(PresentValue < 0)
									PresentValue = 0;
								MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
							}
						}
            
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
            return;
        }

        uw16_i++;
    } while(uw16_i < TC_WATLOW_F4_MIN_RESPONSE_CMD_LEN);
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_US_9256P_ACCU3::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char CmdBuf[SEND_CMD_BUF_SIZE];
		unsigned short crc16;
		short temp;
		short register_data;
	
		CmdBuf[SLAVE_ADDR] = 0x01;
		CmdBuf[FUNCTION_CODE] = 0x06;
		
		if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//温度
		{
			CmdBuf[ADDR_HIGH] = 0x08;
			CmdBuf[ADDR_LOW] = 0xFC;
			
			temp = MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
			
			if(temp > 2500)//温度设定范围-200~250
			{
				temp = 2500;
			}
			else if(temp < -2000)
			{
				temp = -2000;
			}
			
			register_data = temp;
			
			CmdBuf[DATA_HIGH] = (unsigned char) (register_data >> 8);
			CmdBuf[DATA_LOW] = (unsigned char) (register_data & 255);
		}
		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//湿度
		{
			CmdBuf[ADDR_HIGH] = 0x08;
			CmdBuf[ADDR_LOW] = 0xFD;
			
			temp = MTCI_CTRL::gW_HumiValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
			
			if(temp > 1000)//湿度设定0~100
			{
				temp = 1000;
			}
			else if(temp < 0)
			{
				temp = 0;
			}
			
			register_data = temp;
			
			CmdBuf[DATA_HIGH] = (unsigned char) (register_data >> 8);
			CmdBuf[DATA_LOW] = (unsigned char) (register_data & 255);
		}
		
		crc16 = MTCI_datatypes::CRC16(CmdBuf,6);
		CmdBuf[CRC_HIGH] = (unsigned char) (crc16 >>8);
		CmdBuf[CRC_LOW] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,SEND_CMD_BUF_SIZE);
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_US_9256P_ACCU3::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char CmdBuf[SEND_CMD_BUF_SIZE];
		unsigned short crc16;
		unsigned short register_data;
		
		CmdBuf[SLAVE_ADDR] = 0x01;
		CmdBuf[FUNCTION_CODE] = 0x03;
	
		if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//温度
		{
			CmdBuf[ADDR_HIGH] = 0x14;
			CmdBuf[ADDR_LOW] = 0xC9;
			
			register_data = 1;//读取一个暂存区
			CmdBuf[DATA_HIGH] = (unsigned char) (register_data >> 8);
			CmdBuf[DATA_LOW] = (unsigned char) (register_data & 255);
		}
		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//湿度
		{
			CmdBuf[ADDR_HIGH] = 0x14;
			CmdBuf[ADDR_LOW] = 0xCB;
			
			register_data = 1;//读取一个暂存区
			CmdBuf[DATA_HIGH] = (unsigned char) (register_data >> 8);
			CmdBuf[DATA_LOW] = (unsigned char) (register_data & 255);
		}
		
		crc16 = MTCI_datatypes::CRC16(CmdBuf,6);
		CmdBuf[CRC_HIGH] = (unsigned char) (crc16 >>8);
		CmdBuf[CRC_LOW] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,SEND_CMD_BUF_SIZE);
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_US_9256P_ACCU3::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		if(get_state() == RUN)//运行指令和停止指令一样,故只发送一次
			return;
	
    unsigned char CmdBuf[SEND_CMD_BUF_SIZE];
		unsigned short crc16;
		
		CmdBuf[SLAVE_ADDR] 		= 0x01;
		CmdBuf[FUNCTION_CODE] = 0x06;
		CmdBuf[ADDR_HIGH] 		= 0x01;
		CmdBuf[ADDR_LOW] 			= 0xF4;
		CmdBuf[DATA_HIGH] 		= 0x00;
		CmdBuf[DATA_LOW] 			= 0x01;

    crc16 = MTCI_datatypes::CRC16(CmdBuf,6);
    CmdBuf[CRC_HIGH] = (unsigned char) (crc16 >>8);
    CmdBuf[CRC_LOW] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,SEND_CMD_BUF_SIZE);
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
		
		set_state(RUN);
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TH_US_9256P_ACCU3::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		if(get_state() == STOP)//运行指令和停止指令一样,故只发送一次
			return;
	
    unsigned char CmdBuf[SEND_CMD_BUF_SIZE];
		unsigned short crc16;
		
		CmdBuf[SLAVE_ADDR] 		= 0x01;
		CmdBuf[FUNCTION_CODE] = 0x06;
		CmdBuf[ADDR_HIGH] 		= 0x01;
		CmdBuf[ADDR_LOW] 			= 0xF4;
		CmdBuf[DATA_HIGH] 		= 0x00;
		CmdBuf[DATA_LOW] 			= 0x01;

    crc16 = MTCI_datatypes::CRC16(CmdBuf,6);
    CmdBuf[CRC_HIGH] = (unsigned char) (crc16 >>8);
    CmdBuf[CRC_LOW] = (unsigned char) (crc16 & 255);
	
    UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,SEND_CMD_BUF_SIZE);
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
		
		set_state(STOP);
}

