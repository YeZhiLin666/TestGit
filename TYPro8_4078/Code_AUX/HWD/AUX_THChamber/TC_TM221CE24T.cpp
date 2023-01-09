/*****************************************************************************
// 程序文件      :TC_TM221CE24T.C    定义温控箱中各种处理文件
// 文件描述      :为LPC4078 AUX-MPU board服务
// 编写日期      :2019.09.17
*****************************************************************************/


#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
// TC_TM221CE24T  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以字Word操作) 910
//  CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number (byte)] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 03 8E 00 02 04 XX XX XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x03, 0x8E, 0x00, 0x02, 0x04, 0x00, 0x00, 0x41, 0xB0, 0x5F, 0x57,
};     // write Master to  Slave cmd  (设置温度22度，就是22.0)


//设置湿度主机到从机 (以字Word操作) 914
//  CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number (byte)] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 03 92 00 02 04 XX XX XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteHumiCmd_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x03, 0x92, 0x00, 0x02, 0x04, 0x00, 0x00, 0x42, 0x5C, 0x5F, 0x73,
}; //(设置湿度55%，就是55.0)


//  从机回复时，Reply一模一样：
//  Reply CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 10 03 8E XX XX YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteReply_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_WRITE_REPLY] = {
    0x01, 0x10, 0x03, 0x8E, 0x00, 0x02, 0x21, 0xA7
};     // write Slave to  Master reply   01 10 03 8E 00 02 21 A7



//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[device address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 01 91 00 02 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadTempCmd_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x01, 0x91, 0x00, 0x02, 0x94, 0x1A
};    // read Master to  Slave cmd 01 03 01 91 00 02 94 1A  若读出为22.0，则实际值为22.0度

//读取湿度主机到从机(以字Word操作) 
//  MODEL: 01 03 03 66 00 02 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadHumiCmd_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x03, 0x66, 0x00, 0x02, 0x24, 0x50
};    // read Master to  Slave cmd 

//  从机回复 温度湿度 格式一样
//  Reply CMD: {[device address (1 byte)] [read command (0x03)] [number of 打他data bytes] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 00 03 04 XX XX XX XX YY YY   (XX温度值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadReply_TC_TM221CE24T[TC_TM221CE24T_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xCB, 0xD7
};     // read Slave to  Master reply  //(回复温度22.0度，就是22.0,若是湿度22.0，相同) 01 03 04 00 00 41 B0 CB D7


//切换为远程模式：只有在远程模式下，上位机才能对温控器进行控制 写地址260  X6:1  X3:0
const unsigned char uc_COM_WriteModeCmd_TC_TM221CE24T[TC_TM221CE24T_MIN_COM_WRITE_CMD] = {
    0x01, 0x10, 0x01, 0x04, 0x00, 0x01, 0x02, 0x00, 0x40, 0xB6, 0xE4,
};     // write Master to  Slave cmd  (设置为远程模式)01 10 01 04 00 01 02 00 40 B6 E4


//主机到从机 (以字Word操作) 260   启动温度控制 
//  CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number (byte)] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 01 04 00 01 02 XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_ON_TC_TM221CE24T[TC_TM221CE24T_MIN_COM_WRITE_CMD] = {
    0x01, 0x10, 0x01, 0x04, 0x00, 0x01, 0x02, 0x00, 0x4C, 0xB6, 0xE1
};     // write Master to  Slave cmd  (启动温度控制)01 10 01 04 00 01 02 00 4C B6 E1


//主机到从机 (以字Word操作) 260   启动温度、湿度控制 
//  CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number (byte)] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 01 04 00 01 02 XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTHCmd_ON_TC_TM221CE24T[TC_TM221CE24T_MIN_COM_WRITE_CMD] = {
    0x01, 0x10, 0x01, 0x04, 0x00, 0x01, 0x02, 0x00, 0x6C, 0xB7, 0x39
};		// write Master to  Slave cmd


//主机到从机 (以字Word操作) 260 停止温湿度控制  与远程模式命令相同
//  CMD: {[device address (1 byte)] [write command (0x10)] [register address high byte] [register address low byte] [register number high byte] [register number low byte] [data number (byte)] [data low byte(s)] [data high byte(s)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 01 04 00 01 02 XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_OFF_TC_TM221CE24T[TC_TM221CE24T_MIN_COM_WRITE_CMD] = {
    0x01, 0x10, 0x01, 0x04, 0x00, 0x01, 0x02, 0x00, 0x40, 0xB6, 0xE4
};     // write Master to  Slave cmd  (启动温度控制)01 10 01 04 00 01 02 00 40 B6 E4


//
// TC_TM221CE24T  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------


void TC_TM221CE24T::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
		unsigned char MbBuf[TC_TM221CE24T_MIN_COM_WRITE_CMD];
	
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_TM221CE24T::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_TM221CE24T::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_TM221CE24T::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_TM221CE24T::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_TM221CE24T::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_TM221CE24T::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
		
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	
		memcpy(MbBuf, uc_COM_WriteModeCmd_TC_TM221CE24T, TC_TM221CE24T_MIN_COM_WRITE_CMD);
		UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_TM221CE24T_MIN_COM_WRITE_CMD);//发送远程模式命令
		
}

/*----*/
unsigned char TC_TM221CE24T::SetPresentValue_Check(unsigned char ChanNo, short int Value)
{
// 更新现时取回的值

    if(	Value == 0)
    {   // 避过交替出现的零值显示出现。
        if( MTCI_CTRL::gR_Zero_Counter[ChanNo]++ <= 5)
            return  0;
        else
            MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }
    else
    {
        MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }
    return  1;
}

void TC_TM221CE24T::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    unsigned char *pCmd;
    short int PresentValue = 0;
//     short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
		union UW_F
    {
        float f_member;
        unsigned char uc_member[4];
    } F_data;
		
		uw16_Total =  UARTFunc::uart_bytenum(ChanNo);
		
//     uw16_Total =  TC_TM221CE24T::ParseResult_FirstCheck(ChanNo);
// 	
		if( uw16_Total < TC_TM221CE24T_MIN_RESPONSE_CMD_LEN)
			return;

//     if( !TC_TM221CE24T::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf,TC_TM221CE24T_MAX_RESPONSE_CMD_LEN) )
//         return;   // 过多信息不处理，上限保护 // 转移数据
		
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TC_TM221CE24T[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TC_TM221CE24T[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TC_TM221CE24T[2] && MTCI_CTRL::gR_CmdBuf[uw16_i+7] != 0)
        {   // 利用包头或包尾判断数据值。
					
						F_data.uc_member[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
						F_data.uc_member[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
						F_data.uc_member[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+6];
						F_data.uc_member[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
						PresentValue = (int)(F_data.f_member * 10);

            if( TC_TM221CE24T::SetPresentValue_Check(ChanNo, PresentValue) )
            {
// #ifdef COM_TEMP4HUMI_CMD
// 							MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag);
// #else	
							MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
// #endif  		
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TC_TM221CE24T::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令
		unsigned int  MbChkSum;
    short int  register_data;
		unsigned char MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD];
		union UW_F
    {
        float f_member;
        unsigned char uc_member[4];
    } F_data;
		
			CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
		
		if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
		{
			for(MbChkSum = 0; MbChkSum < (TC_TM221CE24T_MAX_COM_WRITE_CMD-6); MbChkSum++)
					MbBuf[MbChkSum] = uc_COM_WriteTempCmd_TC_TM221CE24T[MbChkSum];
					
			register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
		}
// 		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
// 		{
// 			for(MbChkSum = 0; MbChkSum < (TC_TM221CE24T_MAX_COM_WRITE_CMD-6); MbChkSum++)
// 					MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_TC_TM221CE24T[MbChkSum];
// 			
// 			register_data = MTCI_CTRL::gW_HumiValue[ChanNo];
// 		}
		else
			return;
		
			
			F_data.f_member = ((float)register_data)/10.0f;

			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-6] = F_data.uc_member[1];
			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-5] = F_data.uc_member[0];
			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-4] = F_data.uc_member[3];
			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-3] = F_data.uc_member[2];
			
			MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_TM221CE24T_MAX_COM_WRITE_CMD-2);
			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
			MbBuf[TC_TM221CE24T_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
		
			UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_TM221CE24T_MAX_COM_WRITE_CMD);  // 发送定长处理

}


void TC_TM221CE24T::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
		unsigned char MbBuf[TC_TM221CE24T_MIN_COM_WRITE_CMD];
	
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	
// #ifdef COM_TEMP4HUMI_CMD	
// //启动 温湿度控制
// 		memcpy(MbBuf,uc_COM_WriteTHCmd_ON_TC_TM221CE24T,TC_TM221CE24T_MIN_COM_WRITE_CMD);
// #else		
//仅启动温度控制
		memcpy(MbBuf,uc_COM_WriteTempCmd_ON_TC_TM221CE24T,TC_TM221CE24T_MIN_COM_WRITE_CMD);
// #endif	
			
			UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_TM221CE24T_MIN_COM_WRITE_CMD);  // 发送定长处理	
}

void TC_TM221CE24T::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    unsigned char MbBuf[TC_TM221CE24T_MAX_COM_READ_CMD];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
		
// #ifdef COM_TEMP4HUMI_CMD
// 		if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
// 				memcpy(MbBuf, uc_COM_ReadHumiCmd_TC_TM221CE24T, TC_TM221CE24T_MAX_COM_READ_CMD);
// 		
// 		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
// 				memcpy(MbBuf, uc_COM_ReadTempCmd_TC_TM221CE24T, TC_TM221CE24T_MAX_COM_READ_CMD);
// #else	
		memcpy(MbBuf, uc_COM_ReadTempCmd_TC_TM221CE24T, TC_TM221CE24T_MAX_COM_READ_CMD);
// #endif
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_TM221CE24T_MAX_COM_READ_CMD);
}

void TC_TM221CE24T::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令
		unsigned char MbBuf[TC_TM221CE24T_MIN_COM_WRITE_CMD];
	
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	
		memcpy(MbBuf, uc_COM_WriteTempCmd_OFF_TC_TM221CE24T, TC_TM221CE24T_MIN_COM_WRITE_CMD);
		UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_TM221CE24T_MIN_COM_WRITE_CMD);			//停止温湿度控制
}



