/*****************************************************************************
// 程序文件      :TC_POLYSCIENCE.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Cotroller board服务
// 编写日期      :2011.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

unsigned char TC_POLYSCIENCE_CA02A1P1::uc_Read_T_P_F_H_Cmd = 0;
unsigned char TC_POLYSCIENCE_CA02A1P1::uc_Read_OverTime = 0;
unsigned char TC_POLYSCIENCE_CA02A1P1::uc_COM_BoardID = 0;

const		char CmdBuf_PV[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RT\r";
const		char CmdBuf_Probe1[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="R1\r";
const		char CmdBuf_Probe2[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="R2\r";
const		char CmdBuf_PSI_P[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RP\r";
const		char CmdBuf_KPA_P[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RK\r";
const		char CmdBuf_GPM_F[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RG\r";
const		char CmdBuf_LPM_F[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RL\r";
const		char CmdBuf_LINE_V[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RV\r";
const		char CmdBuf_Remote_T[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RR\r";
const		char CmdBuf_Ambient_T[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RA\r";
const		char CmdBuf_Fluid_Level[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RFL\r";;
const		char CmdBuf_Compressor_Amp[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RCA\r";
const		char CmdBuf_Pump_Amp[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RPA\r";
const		char CmdBuf_Humid[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="RRH\r";



extern struct FUNC{
	My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);   
}st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


void TC_POLYSCIENCE_CA02A1P1::Init(unsigned char ChanNo)
{
	// 初始化函数指针。
	unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
	for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
	{
		st_SendCMD[ChanNo][uc_i].Function = (&TC_POLYSCIENCE_CA02A1P1::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
	}

	st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_POLYSCIENCE_CA02A1P1::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_POLYSCIENCE_CA02A1P1::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_POLYSCIENCE_CA02A1P1::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_POLYSCIENCE_CA02A1P1::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)  
	st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_POLYSCIENCE_CA02A1P1::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
	
	uc_Read_T_P_F_H_Cmd = 0;
	uc_Read_OverTime = 0;
	MTCI_CTRL::uw32_CMD_INTERVALTIME = 60*10; //CMD_INTERVALTIME;    60ms
	
	
	for(unsigned char uc_i = 0; uc_i < MAX_BOARD_PER_AUXMCU; uc_i++)
	{
			if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_valid == BEUSED)
			{
					if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_Total_UART !=0)
					{
						    uc_COM_BoardID = uc_i;
					}
			}
	}
	
	//标记data_group 有效
	Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1] = true;
//	Aux_DataManager::m_AllAuxBoards[(uc_COM_BoardID+1)%MAX_BOARD_PER_AUXMCU].m_BoardInfo.m_valid = true;
	for(int i = 0; i< 14; i++) // 14个虚拟电压通道  
	{  
			Aux_DataManager::m_AllAuxBoards[(uc_COM_BoardID+1)%MAX_BOARD_PER_AUXMCU].m_BoardInfo.m_abi_chn[i].m_Populated = true;
			Aux_DataManager::m_AllAuxBoards[(uc_COM_BoardID+1)%MAX_BOARD_PER_AUXMCU].m_BoardInfo.m_abi_chn[i].m_AuxType = AUX_VOLTAGE;
			Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[(uc_COM_BoardID+1)%MAX_BOARD_PER_AUXMCU].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
			Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][i].e_chan_id = i; //面板通道与电气通道同号
			Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1;
		
			Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][i]. m_Chn_Info.m_Populated = true;
			Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][i]. m_Chn_Info.m_AuxType = AUX_VOLTAGE;
	}
	
	 Aux_DataManager::Aux_BuildLongSnake();
	 Aux_DataManager::Aux_ConfirmContainAuxTypes();//辅助限制
	 Aux_DataManager::Aux_TranslateMapFromPC();
}


/*************************************************************** 
*    WRITE A COMMAND TO TEMPERATURE CHAMBER                   *
*                                                             *
*    IN:     COMMAND                                          *
*    RETURN: VOID                                             *  
***************************************************************/

//bool TC_POLYSCIENCE_CA02A1P1::ReadChamber (unsigned char uc_UartCh,unsigned char *pCmd, unsigned char len)
//{
//	//unsigned char next_pCmd;
//	unsigned int ByteNum = 0;    

//	while( UARTFunc::gl_Rx_first[uc_UartCh] != UARTFunc::gl_Rx_last[uc_UartCh] )   //buffer not empty
//	{
//		*pCmd = UARTFunc::gl_Rx_buf[uc_UartCh][UARTFunc::gl_Rx_first[uc_UartCh]]; //first available byte

//		UARTFunc::gl_Rx_first[uc_UartCh] = ( UARTFunc::gl_Rx_first[uc_UartCh] + 1 ) % MAX_UART_BUF_SIZE;  //shift to next byte on the ring buff

//		//if( ((*pCmd)== '\n' )|| (*pCmd == '\r') )
//		if(*pCmd == '\r')
//		{
//			if( ByteNum > 0 )
//			{
//				*pCmd=0;
//				return true;
//			}
//			return false;
//		}
//		pCmd++;

//		if( (ByteNum++) > len)
//		{
//			*pCmd=0;
//			return false;
//		}

//		if( UARTFunc::gl_Rx_first[uc_UartCh] == UARTFunc::gl_Rx_last[uc_UartCh] )//buffer empty? wait
//		{
//			ARBIN_DELAY_US(14); 
//		}

//	}
//	*pCmd=0;
//	return false;
//}

unsigned int uw32_Polyscience_ByteNum = 0;   



/***************************************************************
*   Parse the data come back from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_POLYSCIENCE_CA02A1P1::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	//unsigned char *echo;
	unsigned char *pCmd;
	bool NoZero = 0;
	bool b_is_temp = NoZero;	        

//	if( UARTFunc::uart_bytenum(uc_ChNo) < TC_POLYSCIENCE_CA02A1P1_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
//		return;
		if( UARTFunc::uart_bytenum(uc_ChNo) < 1) //wait until MIN_LEN bytes have been read into the buffer
		return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);             

	MTCI_CTRL::gR_CmdBuf[TC_POLYSCIENCE_CA02A1P1_MAX_RESPONSE_CMD_LEN+1]=0;
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

		if(uc_Read_T_P_F_H_Cmd == READ_POLYSCIENCE_PV_TEMPERATURE)
		{
		    MTCI_CTRL::SetPresentValue(uc_ChNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL); 
		}
		
		Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][uc_Read_T_P_F_H_Cmd].aux_data = PresentValue/10.0;
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);
		uc_Read_OverTime = 0;
		if(++uc_Read_T_P_F_H_Cmd > READ_POLYSCIENCE_RELATIVE_HUMIDITY)
		{
			 uc_Read_T_P_F_H_Cmd = READ_POLYSCIENCE_PV_TEMPERATURE;
		}
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

void TC_POLYSCIENCE_CA02A1P1::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char  *p;
	//const char CmdBuf[SEND_MAX_BUF]="SS123.4\r\n";
	const char CmdBuf[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="SS123.4\r";
	//float SetPointValue=temperature;
	short SetPointValue=MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
	unsigned short Count = 0; 


	//strcpy(CmdBuf,"TEMP,S22.0\r\n");  //ZL, for SCP_220, accordting to Dr. Xie
	//p=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,CmdBuf+6); 
	p = (unsigned char*) &CmdBuf[2];
	Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p);
	p = (unsigned char*)&CmdBuf[2 + Count];

    *p='\r';
    p++;
//    *p = '\n';
//    p++;  //ZL, for SCP_220, accordting to Dr. Xie
	*p=0;   
	p = (unsigned char*)&CmdBuf[0];
  
	uw32_Polyscience_ByteNum = 0;
	memset(&MTCI_CTRL::gR_CmdBuf[0],0,MAX_UART_BUF_SIZE);
	UARTFunc::uart_putcmd(uc_ChNo, p, strlen(CmdBuf));		  


    MTCI_CTRL::Change_T_H(uc_ChNo,SEND_COM_WRITE_CMD);   // 温控器只能单命令接收时仅发一条指令
    MTCI_CTRL::ExchangeCheckCounter(uc_ChNo, &MTCI_CTRL::g_Cmd_Flag[uc_ChNo], SEND_COM_READ_CMD, &MTCI_CTRL::g_WriteCmd_Counter[uc_ChNo]);

	//MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作 
	return;          
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_POLYSCIENCE_CA02A1P1::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char *pCmd;
	//const char CmdBuf[SEND_MAX_BUF]="SO1\r\n" ;
	const char CmdBuf[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="SO1\r" ;
	pCmd=(unsigned char*) &CmdBuf[0];
 
	uw32_Polyscience_ByteNum = 0;
	memset(&MTCI_CTRL::gR_CmdBuf[0],0,MAX_UART_BUF_SIZE);  
	UARTFunc::uart_putcmd(uc_ChNo, pCmd, strlen(CmdBuf));
	//MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作 
	return;
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_POLYSCIENCE_CA02A1P1::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char *pCmd;
	//const char CmdBuf[SEND_MAX_BUF]="RT\r\n" ;
	unsigned char uc_length = 3;
	
		if(uc_Read_OverTime)   // 读数超时，取不到数据，换下一个
		{
			if(++uc_Read_T_P_F_H_Cmd > READ_POLYSCIENCE_RELATIVE_HUMIDITY)
			{
				 uc_Read_T_P_F_H_Cmd = READ_POLYSCIENCE_PV_TEMPERATURE;
			}
	  }
	
	switch(uc_Read_T_P_F_H_Cmd)
	{
		case READ_POLYSCIENCE_PV_TEMPERATURE:
		default:
		  uc_length =strlen(CmdBuf_PV);
		  pCmd = (unsigned char*) &CmdBuf_PV[0];
			break;
		case READ_POLYSCIENCE_PROBE1_TEMPERATURE:
		  uc_length =strlen(CmdBuf_Probe1);
		  pCmd = (unsigned char*) &CmdBuf_Probe1[0];
			break;
	 case READ_POLYSCIENCE_PROBE2_TEMPERATURE:
		  uc_length =strlen(CmdBuf_Probe2);
		  pCmd = (unsigned char*) &CmdBuf_Probe2[0];
			break;
	 case READ_POLYSCIENCE_PSI_PRESSURE:
		  uc_length =strlen(CmdBuf_PSI_P);
		  pCmd = (unsigned char*) &CmdBuf_PSI_P[0];
			break;
	 case READ_POLYSCIENCE_KPA_PRESSURE:
		  uc_length =strlen(CmdBuf_KPA_P);
		  pCmd = (unsigned char*) &CmdBuf_KPA_P[0];
			break;
	 	 case READ_POLYSCIENCE_GPM_FLOW:
		  uc_length =strlen(CmdBuf_GPM_F);
		  pCmd = (unsigned char*) &CmdBuf_GPM_F[0];
			break;
	 case READ_POLYSCIENCE_LPM_FLOW:
		  uc_length =strlen(CmdBuf_LPM_F);
		  pCmd = (unsigned char*) &CmdBuf_LPM_F[0];
			break;
	 case READ_POLYSCIENCE_LINE_VOLTAGE:
		  uc_length =strlen(CmdBuf_LINE_V);
		  pCmd = (unsigned char*) &CmdBuf_LINE_V[0];
			break;
	 case READ_POLYSCIENCE_REMOTE_PROBE_TEMPERATURE:
		  uc_length =strlen(CmdBuf_Remote_T);
		  pCmd = (unsigned char*) &CmdBuf_Remote_T[0];
			break;
	 case READ_POLYSCIENCE_AMBIENT_TEMPERATURE:
		  uc_length =strlen(CmdBuf_Ambient_T);
		  pCmd = (unsigned char*) &CmdBuf_Ambient_T[0];
			break;
	 case READ_POLYSCIENCE_FLUID_LEVEL:
		  uc_length =strlen(CmdBuf_Fluid_Level);
		  pCmd = (unsigned char*) &CmdBuf_Fluid_Level[0];
			break;
	case READ_POLYSCIENCE_COMPRESSOR_AMPERAGE:
		  uc_length =strlen(CmdBuf_Compressor_Amp);
		  pCmd = (unsigned char*) &CmdBuf_Compressor_Amp[0];
			break;
	 case READ_POLYSCIENCE_PUMP_AMPERAGE:
		  uc_length =strlen(CmdBuf_Pump_Amp);
		  pCmd = (unsigned char*) &CmdBuf_Pump_Amp[0];
			break;
	case READ_POLYSCIENCE_RELATIVE_HUMIDITY:
		  uc_length =strlen(CmdBuf_Humid);
		  pCmd = (unsigned char*) &CmdBuf_Humid[0];
			break;
	//	default:break;
	}
	uc_Read_OverTime = 1;
	uw32_Polyscience_ByteNum = 0; 
	memset(&MTCI_CTRL::gR_CmdBuf[0],0,MAX_UART_BUF_SIZE);  
	UARTFunc::uart_putcmd(uc_ChNo, pCmd, uc_length);	
	
	MTCI_CTRL::Change_T_H(uc_ChNo,SEND_COM_READ_CMD);   // 温控器只能单命令接收时仅发一条指令
  MTCI_CTRL::ExchangeCheckCounter(uc_ChNo, &MTCI_CTRL::g_Cmd_Flag[uc_ChNo], SEND_COM_WRITE_CMD, &MTCI_CTRL::g_ReadCmd_Counter[uc_ChNo]);
//	pCmd = (unsigned char*) &CmdBuf[0];

//	memcpy(MTCI_CTRL::gChamberSendBuf,pCmd,strlen(CmdBuf));
//	UARTFunc::uart_putcmd(uc_ChNo, MTCI_CTRL::gChamberSendBuf, strlen(CmdBuf));
	//MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作 
	
	if(MTCI_CTRL::uc_OffLine[uc_ChNo][FIRST_CHAMBER_CHANNEL])
	{ // 掉线时，14个虚拟电压通道，第一个通道 与 温箱温度通道同步更新。
		 Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[uc_COM_BoardID].m_data_group_start+1][READ_POLYSCIENCE_PV_TEMPERATURE].aux_data \
		   = MTCI_CTRL::str_Uart[uc_ChNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]/10.0;
	}
		
	return;                                 	          
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_POLYSCIENCE_CA02A1P1::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char *pCmd;
	//const char CmdBuf[SEND_MAX_BUF]="SO0\r\n";
	const char CmdBuf[TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE]="SO0\r";
	pCmd=(unsigned char*) &CmdBuf[0];

	uw32_Polyscience_ByteNum = 0;
	memset(&MTCI_CTRL::gR_CmdBuf[0],0,MAX_UART_BUF_SIZE);  
	UARTFunc::uart_putcmd(uc_ChNo, pCmd, strlen(CmdBuf));
	//MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作 
	return;    

}

