/*****************************************************************************
// 程序文件      :TC_LAUDA_PRO_P2E.C    定义温控箱中各种控制命令文件
// 文件描述      :为LPC2468 AUX-Temperature Controller Board服务
// 编写日期      :2019.04.02
*****************************************************************************/



#include "../../Entry/includes.h"

extern struct FUNC{
	My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);   
}st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

// 注意设置
// #define  COM_INTERVAL_WRITE_AND_READ                 0         //是否使用串口间隔写和读功能 为0表示不使用间隔写和读功能
// const unsigned char StartCmd[TC_LAUDA_PRO_P2E_START_CMD_LEN] ={0x73, 0x74, 0x61, 0x72, 0x74, 0x0D};// "start\r";
// const unsigned char SetTempCmd[TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN] ={0x6F, 0x75, 0x74, 0x5F, 0x73, 0x70, 0x5F, 0x30, 0x30, 0x5F, 0x35, 0x35, 0x2E, 0x35, 0x0D };// "out_sp_00_55.5\r";
// const unsigned char ReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN] ={0x69, 0x6E, 0x5F, 0x70, 0x76, 0x5F, 0x30, 0x30, 0x0D};//"in_pv_00\r";
// 												  //{0x69, 0x6E, 0x5F, 0x73, 0x70, 0x5F, 0x30, 0x30, 0x0D};//"in_sp_00\r";
// const unsigned char StopCmd[TC_LAUDA_PRO_P2E_STOP_CMD_LEN] ={0x73, 0x74, 0x6F, 0x70, 0x0D};// "stop\r";

////////////////////////////////////////////////////
const unsigned char ControlChamber[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN] = {"OUT_MODE_01_0\r\n"}; //internal temperature control
const unsigned char StartCmd[TC_LAUDA_PRO_P2E_START_CMD_LEN] ="START\r\n";// "start\r";
const unsigned char StopCmd[TC_LAUDA_PRO_P2E_STOP_CMD_LEN] ="STOP\r\n";// "stop\r";
                                                                       
const unsigned char SetTempCmd[TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN] ="OUT_SP_00_55.5\r\n";// "out_sp_00_55.5\r";
const unsigned char ReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN] ="IN_PV_00\r\n";//"in_pv_00\r";
                                                                     
const unsigned char SetHumiCmd[TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN] ="OUT_SP_01_1\r\n";// "out_sp_00_55.5\r";
const unsigned char ReadHumiCmd[TC_LAUDA_PRO_P2E_READ_HUMI_CMD_LEN] = "IN_SP_01\r\n";//"in_pv_00\r";

unsigned char TC_LAUDA_PRO_P2E::TE_AuxChannalNo;

unsigned int TC_LAUDA_PRO_P2E::CountHumi = 0;
unsigned int TC_LAUDA_PRO_P2E::CountTempInternal = 10;
unsigned int TC_LAUDA_PRO_P2E::CountTempExternal = 20;

bool isRunning = 0;
int writtenModeCommands = 0;
int writtenHumiCommands = 0;
int writtenTempCommands = 0;
int currentHumiLevel = 0;
unsigned char lastReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN] ="IN_PV_00\r\n";
unsigned char lastSetTempCmd[TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN] = "OUT_SP_00_XXXX\r\n";

unsigned char recv_state = 0xff;

////////////////////////////////////////////////////

void TC_LAUDA_PRO_P2E::Init(unsigned char ChanNo)
{
	// 初始化函数指针。
	unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
	for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
	{
		st_SendCMD[ChanNo][uc_i].Function = (&TC_LAUDA_PRO_P2E::SendRunCmd);   //  2-M)读取温度主机到从机命令  (以字节操作) 2-M) Read Temperature Master to Slave Command (in byte operation)

	}

	st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_LAUDA_PRO_P2E::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_LAUDA_PRO_P2E::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_LAUDA_PRO_P2E::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
	st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_LAUDA_PRO_P2E::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)  
	st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_LAUDA_PRO_P2E::Parse);  //  5-S)读取温度从机回复主机命令  (以字节操作)
	
    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;	
}

void TC_LAUDA_PRO_P2E::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);
		
		if(recv_state > 3)
			return;
		
		short int PresentValue = 0;
		unsigned char *pCmd = MTCI_CTRL::gR_CmdBuf;
		unsigned char* p;
		
		while((*pCmd != 0x0d) && (*pCmd != 0))
		{
			if((*pCmd == '-') || ((*pCmd >= '0') && (*pCmd <= '9')))
				break;
			pCmd++;
		}
		
		p=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);
		PresentValue *= 10;
		
		if( p == NULL )
				return;
        
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_HUMI,MTCI_CTRL::PresentChamberReadHumiCount[uc_ChNo]);    //KL update present value of the data read from chamber
    else        
        MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue/10,SETPOINTVALUE_TEMPERATURE,MTCI_CTRL::PresentChamberReadTempCount[uc_ChNo]);    //KL update present value of the data read from chamber

//     if(MTCI_datatypes::ctrl_String2Value_Signed(MTCI_CTRL::gR_CmdBuf, &PresentValue) != NULL)
//         MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE);    //KL update present value of the data read from chamber

	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
	return;

}     

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

// void TC_LAUDA_PRO_P2E::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
// {
//     //  发送写控制值命令

//         short int  register_data;       
//         unsigned int  MbChkSum; 
// 		unsigned int  uw16_len = 0;
//         unsigned char HumiLevel = 1;
//         unsigned char MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN] = {0};
//         unsigned char MbSetBuf[20] = {0};
// 		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // 对串口发出过命令则更新时间

//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     	{
//             //发送Mode命令，切换所控制的温控箱
//             /**********************可能不需要***************************/
//              unsigned char HumiCount = MTCI_CTRL::PresentChamberHumiCount[uc_ChNo];
//             memcpy(MbModeBuf, ControlChamber, TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);           
//             if(HumiCount > 0)
//             {
//                 MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-2] = HumiCount+'1';
//             }
//             UARTFunc::uart_putcmd(uc_ChNo,MbModeBuf,TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);  // 发送定长处理
//             /******************************************************/
//             //发送实际控制温控箱湿度等级的命令
//             if(MTCI_CTRL::gW_HumiValue[uc_ChNo][HumiCount] > 8)
//                 HumiLevel = 8;
//             else if(MTCI_CTRL::gW_HumiValue[uc_ChNo][HumiCount] < 1)
//                 HumiLevel = 1;
//             else
//                 HumiLevel = MTCI_CTRL::gW_HumiValue[uc_ChNo][HumiCount];
//             
//             memcpy(MbSetBuf,SetHumiCmd,TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN);
//             MbSetBuf[TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN-2] = HumiLevel + '0';
//             UARTFunc::uart_putcmd(uc_ChNo,MbSetBuf,TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN);  // 发送定长处理
//          
//             MTCI_CTRL::PresentChamberHumiCount[uc_ChNo]++;
//             if(MTCI_CTRL::PresentChamberHumiCount[uc_ChNo] >= MTCI_CTRL::str_Uart[uc_ChNo].uc_Chamber_HumiCount)
//                 MTCI_CTRL::PresentChamberHumiCount[uc_ChNo] = 0;
//     	}
// 		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
// 		{
//             //发送Mode命令，切换所控制的温控箱
//              unsigned char TempCount = MTCI_CTRL::PresentChamberTempCount[uc_ChNo];
//             memcpy(MbModeBuf, ControlChamber, TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);           
//             if(TempCount > 0)
//             {
//                 MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-2] = TempCount+'0';
//             }
//             UARTFunc::uart_putcmd(uc_ChNo,MbModeBuf,TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);  // 发送定长处理
//             
//             //发送实际控制温控箱温度值的命令
//             memcpy(MbSetBuf,SetTempCmd,TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN);
//             unsigned char  *p;
//             short SetPointValue= MTCI_CTRL::gW_TemperatureValue[uc_ChNo][TempCount];//555
//             unsigned short Count = 0; 
//             p = (unsigned char*) &MbSetBuf[10];
//             Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p);
//             p = (unsigned char*)&MbSetBuf[10 + Count];
//             *p=0x0D; //'\r'; 
//             UARTFunc::uart_putcmd(uc_ChNo,MbSetBuf,(TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN));
//             
//             MTCI_CTRL::PresentChamberTempCount[uc_ChNo]++;
//             if(MTCI_CTRL::PresentChamberTempCount[uc_ChNo] >= MTCI_CTRL::str_Uart[uc_ChNo].uc_Chamber_TempCount)
//                 MTCI_CTRL::PresentChamberTempCount[uc_ChNo] = 0;
//             
// 		}
// 		else
// 			return;
// }
void TC_LAUDA_PRO_P2E::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	/*
	-Avoid sending the same humidity command too many times
	-The same humidity level command will be resent after 10 tries in case
	 there is an error in transmission.
	-A different setting humidity level command will be sent immediately
	
	-Similar to humidity level commands, the program will resend the same set-mode command after 10 tries
	-A different set-mode command will be sent immediately

	-The same temperature setting command will be resent after 10 counts of the 
	 variable writtenTempCommands.
	-A different setting temperature setting command will be sent immediately
	*/
    short int  register_data;       
    unsigned int  MbChkSum; 
	unsigned int  uw16_len = 0;
//  unsigned char HumiLevel = 1;
    unsigned char MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN] = {0};
    unsigned char MbSetBuf[20] = {0};
	char temp[20]={0};
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI){
         for(unsigned char HumiCount = 0; HumiCount < MTCI_CTRL::str_Uart[uc_ChNo].uc_Chamber_HumiCount; HumiCount++){
			//发送实际控制温控箱湿度等级的命令
			unsigned char HumiLevel = MTCI_CTRL::gW_HumiValue[uc_ChNo][HumiCount] / 10;
			if( HumiLevel > 8)
				HumiLevel = 8;
			else if(HumiLevel < 1)
				HumiLevel = 1;
			if(currentHumiLevel != HumiLevel || writtenHumiCommands > 10){
				writtenHumiCommands = 0;
				memcpy(MbSetBuf,SetHumiCmd,TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN);
				MbSetBuf[TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN-4] = HumiLevel + '0';
				currentHumiLevel = HumiLevel;
				recv_state = 0xff;
				UARTFunc::uart_putcmd(uc_ChNo,MbSetBuf,TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN-2);  // 发送定长处理
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
			}
			else{
				/*Counter to delay retransmission of the same humidity level command*/
				writtenHumiCommands++;
				return;
			}
        }
    }
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
	{  
		//External TC
		if (TE_AuxChannalNo == 0) {
			CountTempInternal++;
			if(writtenModeCommands == 0){
				memcpy(MbModeBuf, ControlChamber, TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);
				MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-4] = '1';
				recv_state = 0xff;
				UARTFunc::uart_putcmd(uc_ChNo,MbModeBuf,TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-2);
				writtenModeCommands++;
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
				return;
			}
			else if(writtenModeCommands < 10){
				writtenModeCommands++;
			}
			else{
				writtenModeCommands = 0;
			}
			memset(MbSetBuf,0,sizeof(MbSetBuf));
			memset(temp,0,sizeof(temp));
			strcpy((char*)MbSetBuf,"OUT_SP_00_");
			sprintf(temp,"%.1f",(float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10);
			strcat((char*)MbSetBuf,temp);
			strcat((char*)MbSetBuf,"\r\n");

			int compare = 0;
			for(int i = 10; i < TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN; i++){
				if(MbSetBuf[i] != lastSetTempCmd[i])
					compare++;
			}
			if(compare > 0 || writtenTempCommands == 0){
				writtenTempCommands = 0; //reset the counter when a different temperature control command is sent
				recv_state = 0xff;
				UARTFunc::uart_putcmd(uc_ChNo,MbSetBuf,strlen((char*)MbSetBuf));
				memcpy(lastSetTempCmd, MbSetBuf, TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN);
				writtenTempCommands++;
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
			}
			else if(compare == 0 && writtenTempCommands < 10){
				writtenTempCommands++;
			}
			else{
				writtenTempCommands = 0;
			}
		}
		//Internal TC
		else if (TE_AuxChannalNo == 1) {
			if(writtenModeCommands == 0){
				memcpy(MbModeBuf, ControlChamber, TC_LAUDA_PRO_P2E_SELECT_CMD_LEN);
				//MbModeBuf[TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-4] = '1';
				recv_state = 0xff;
				UARTFunc::uart_putcmd(uc_ChNo,MbModeBuf,TC_LAUDA_PRO_P2E_SELECT_CMD_LEN-2);
				writtenModeCommands++;
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
				return;
			}
			else if(writtenModeCommands < 10){
				writtenModeCommands++;
			}
			else{
				writtenModeCommands = 0;
			}
			memset(MbSetBuf,0,sizeof(MbSetBuf));
			memset(temp,0,sizeof(temp));
			strcpy((char*)MbSetBuf,"OUT_SP_00_");
			sprintf(temp,"%.1f",(float)MTCI_CTRL::gW_TemperatureValue[uc_ChNo][SECOND_CHAMBER_CHANNEL]/10);
			strcat((char*)MbSetBuf,temp);
			strcat((char*)MbSetBuf,"\r\n");	

			int compare = 0;
			for(int i = 10; i < TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN; i++){
				if(MbSetBuf[i] != lastSetTempCmd[i])
					compare++;
			}
			if(compare > 0 || writtenTempCommands == 0){
				writtenTempCommands = 0;
				recv_state = 0xff;
				UARTFunc::uart_putcmd(uc_ChNo,MbSetBuf,strlen((char*)MbSetBuf));
				memcpy(lastSetTempCmd, MbSetBuf, TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN);
				writtenTempCommands++;
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
			}
			else if(compare == 0 && writtenTempCommands < 10){
				writtenTempCommands++;
			}
			else{
				writtenTempCommands = 0;
			}
		}
	}
	else 
		return;
}	
	
/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

void TC_LAUDA_PRO_P2E::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TC_LAUDA_PRO_P2E_START_CMD_LEN];
    memcpy(Buf, StartCmd, TC_LAUDA_PRO_P2E_START_CMD_LEN);
	recv_state = 0xff;
	UARTFunc::uart_putcmd(uc_ChNo,Buf,TC_LAUDA_PRO_P2E_START_CMD_LEN-2);
	isRunning = 1;
	//MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;
}
/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

	
void TC_LAUDA_PRO_P2E::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
//     unsigned char Buf[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN];
//     memcpy(Buf, ReadTempCmd, TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN);
// 	UARTFunc::uart_putcmd(uc_ChNo, Buf, TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN ); 
// 	//MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;               
    //  发送读命令

	short int  register_data;       
	unsigned int  MbChkSum; 
	unsigned int  uw16_len = 0;
	unsigned char HumiLevel = 1;
	unsigned char MbReadBuf[20] = {0};
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // 对串口发出过命令则更新时间

	if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
	{
		//读取温控箱湿度设定值
		recv_state = 3;
		memcpy(MbReadBuf,ReadHumiCmd,TC_LAUDA_PRO_P2E_READ_HUMI_CMD_LEN);
		UARTFunc::uart_putcmd(uc_ChNo,MbReadBuf,(TC_LAUDA_PRO_P2E_READ_HUMI_CMD_LEN-2));
		//TIMERSFunc::DELAY_SYS(500, 1);
		//DELAY2(5000);
	}
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
	{
		unsigned char TempCount = MTCI_CTRL::PresentChamberReadTempCount[uc_ChNo];
		//发送实际控制温控箱温度值的命令 
		memcpy(MbReadBuf,ReadTempCmd,TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN);
		if(TempCount > 0){
			recv_state = 2;
			MbReadBuf[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-4] = '3';  //IN_PV_03\r\n
			lastReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-4] = '3';
			UARTFunc::uart_putcmd(uc_ChNo,MbReadBuf,(TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-2));
			//TIMERSFunc::DELAY_SYS(500, 1);
			//DELAY2(5000);
		}
		else{
			if(lastReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-4] == '0')
				/*If the last read command was the same, do not resend*/
				return;
			else{
				recv_state = 1;
				UARTFunc::uart_putcmd(uc_ChNo,MbReadBuf,(TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-2));
				lastReadTempCmd[TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN-4] = '0';
				//TIMERSFunc::DELAY_SYS(500, 1);
				//DELAY2(5000);
			}
		}
		
	}
	else
		return;    
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                  *
*   Out: void                                                 *
***************************************************************/
void TC_LAUDA_PRO_P2E::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TC_LAUDA_PRO_P2E_STOP_CMD_LEN];
    memcpy(Buf, StopCmd, TC_LAUDA_PRO_P2E_STOP_CMD_LEN);
	recv_state = 0xff;
    UARTFunc::uart_putcmd(uc_ChNo,Buf,TC_LAUDA_PRO_P2E_STOP_CMD_LEN-2);
	isRunning = 0;
	writtenModeCommands = 0;
	writtenHumiCommands = 0;
	writtenTempCommands = 0;
	currentHumiLevel = 0;
	for(int i = 10; i < TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN; i++){
		lastSetTempCmd[i] = 'X';
	}
	//MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;
}

