/*****************************************************************************
// 程序文件      :TH_BINDER_MB2.C    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
// MB2  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以字Word操作)
//  CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)][Number of Words(2 bytes)] [Number of bytes(1 byte)][Word value (x byte)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 15 81 00 02 04 XX XX XX XX YY YY    (XX设置值先传高位再传低位; YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteTempCmd_MB2[TC_MB2_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x11, 0x4C, 0x00, 0x02, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xF9, 0x77
};     // write Master to  Slave cmd  (设置温度22度 float型)

//  从机回复时：
//  Reply CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)] [Number of Word(2 bytes)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 10 15 81 00 02 YY YY   (YY校验值先传低位再传高位)
const unsigned char uc_COM_WriteReply_MB2[TC_MB2_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x11, 0x4C, 0x00, 0x02, 0x15, 0xEC
};     // write Slave to  Master reply ((YY校验值先传低位再传高位))


//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [Address first word (2 byte)] [Number of words (2 byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 03 11 A9 00 02 YY YY   (YY校验值先传低位再传高位)(读取温度)
const unsigned char uc_COM_ReadTempCmd_MB2[TC_MB2_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x10, 0x04, 0x00, 0x02, 0x81, 0x0A
};    // read Master to  Slave cmd (读取1个float温度值)


//  从机回复时，Reply使用float方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [Word values (x byte)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 04 XX XX XX XX YY YY   (XX温度值先传低位再传高位; YY校验值先传低位再传高位)
const unsigned char uc_COM_ReadReply_MB2[TC_MB2_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xCB, 0xD7
};     // read Slave to  Master reply  //(回复温度22.0度，float型)


//  3-M)要求运行命令主机到从机
//  CMD: {[controller address (1 byte)] [read command (0x03)] [Address first word (2 byte)] [Number of words (2 byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 03 11 A9 00 02 YY YY   (YY校验值先传低位再传高位)(读取温度)
//  从机回复时，Reply使用float方式。
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [Word values (x byte)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 04 XX XX XX XX YY YY   (XX温度值先传低位WORD再传高位WORD; YY校验值先传低位再传高位)
//  使用float型
const unsigned char uc_COM_RunCmd_MB2[TC_MB2_MAX_COM_RUN_CMD] =  {
    0x01, 0x03, 0x10, 0x04, 0x00, 0x02, 0x81, 0x0A
};    // read Master to  Slave cmd (读取1个float型值)


//  4-M)要求停止命令主机到从机命令
//  CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)] [Number of Word(2 byte)] [Number of bytes(1 byte)][Word value (x byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 10 15 81 00 02 04 XX XX XX XX YY YY    (XX设置值先传低位再传高位; YY校验值先传低位再传高位)
//  从机回复时，Reply一模一样：
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 10 11 4c 00 02 YY YY   (XX设置值先传高位再传低位; YY校验值先传低位再传高位)

const unsigned char uc_COM_StopCmd_MB2[TC_MB2_MAX_COM_STOP_CMD] =  {
    0x01, 0x10, 0x11, 0x4c, 0x00, 0x02, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xF9, 0x77
};     // write Master to  Slave cmd  (设置温度22.0度)

//
// MB2  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------
void TC_MB2::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_MB2::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_MB2::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_MB2::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_MB2::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_MB2::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_MB2::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 500*10; //CMD_INTERVALTIME;		
}

/*----*/
unsigned char TC_MB2::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TC_MB2::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    //unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    //short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;
		unsigned int  MbChkSum;

//     uw16_Total =  uart_bytenum(ChanNo);
//    uw16_Total =  TC_MB2::ParseResult_FirstCheck(ChanNo); //保证数据完整性
	
		uw16_Total =  UARTFunc::uart_bytenum(ChanNo); //只获取数据长度，不取值

    if( uw16_Total < TC_MB2_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_MB2[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_MB2[2])
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_MB2[0] && gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_MB2[1] && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_MB2[2])
#endif
        {   // 利用包头或包尾判断数据值。
						MbChkSum = MTCI_datatypes::CRC16(MTCI_CTRL::gR_CmdBuf+uw16_i,7);//接收数据crc校验检查
						if((MTCI_CTRL::gR_CmdBuf[uw16_i+7] != (unsigned char)(MbChkSum >> 8)) || (MTCI_CTRL::gR_CmdBuf[uw16_i+8] != (unsigned char)(MbChkSum & 255)))
							return;
						
            unsigned char Tmp[4];
            memset(Tmp, 0, 4);
            Tmp[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 5];
            Tmp[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 6];
            Tmp[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 3];
            Tmp[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 4];
            PresentValue = MTCI_datatypes::Convert_FloatToInt(Tmp);

            if( TC_MB2::SetPresentValue_Check(ChanNo, PresentValue) ) //避免交替出现0值
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL); //uc_Temp4Humi_Flag
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TC_MB2::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB2_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
	
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     	{
// 	    	for(MbChkSum = 0;MbChkSum < (uw16_len-6);MbChkSum++)
// 				MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_MB2[MbChkSum]; //获取写Hum的头几个固定字节
// 	        register_data = MTCI_CTRL::gW_HumiValue[ChanNo];       //获取写Hum的值
//     	}
// 		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
// 		{
    for(MbChkSum = 0; MbChkSum < (uw16_len-6); MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_MB2[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
// 		}
// 		else
// 			return;


#ifdef COM_CMD_CHANNEL_DIFFER //选择发送给哪个温控箱
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    unsigned char tmp[4];
    memset(tmp,0,4);
    MTCI_datatypes::Convert_IntToFloat(tmp,register_data);
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-3] = tmp[2];
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-4] = tmp[3];
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-5] = tmp[0];
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-6] = tmp[1];

    //MbBuf[TC_MB2_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);  //data high bytes
    //MbBuf[TC_MB2_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);  //data low bytes
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);  //CRC运算函数，已经自动将其高低位转换了
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_MB2_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}


void TC_MB2::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB2_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//         {
//             for(MbChkSum = 0;MbChkSum < TC_MB2_MAX_COM_READ_CMD;MbChkSum++)
//                 MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_MB2[MbChkSum];
//         }
//         else
//         {
    for(MbChkSum = 0; MbChkSum < TC_MB2_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_MB2[MbChkSum];
//         }
    uw16_len = TC_MB2_MAX_COM_READ_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB2_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB2_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}
void TC_MB2::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB2_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
	
    for(MbChkSum = 0; MbChkSum < TC_MB2_MAX_COM_READ_CMD; MbChkSum++)
    {

        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_MB2[MbChkSum];
    }
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB2_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB2_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}

void TC_MB2::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB2_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
	
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


//     if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     {
//         for(MbChkSum = 0;MbChkSum < TC_MB2_MAX_COM_WRITE_CMD;MbChkSum++)
//             MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_MB2[MbChkSum];
//     }
//     else
//     {
    for(MbChkSum = 0; MbChkSum < TC_MB2_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_MB2[MbChkSum];
//     }
    uw16_len = TC_MB2_MAX_COM_WRITE_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB2_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB2_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}



