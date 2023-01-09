/*****************************************************************************
// 程序文件      :TH_CLCD_9700.c    定义温控箱中各种处理文件
// 文件描述      :为LPC4078 AUX-MPU board服务
// 编写日期      :2015.08.13
// 编写人        ：张翼飞
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------
const unsigned char uc_COM_WriteCmd_CLCD_9700[TH_CLCD_9700_MAX_COM_WRITE_CMD] = {
    0x40, 0x30, 0x31, 0x31, 0x35, 0x30, 0x30, 0x44, 0x43, 0x30,
    0x32, 0x32, 0x36, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30,
    0x30, 0x30, 0x37, 0x35, 0x2A, 0x0D
};     // write Master to  Slave cmd  (设置温度22度)

//  从机回复时，Reply设置值：
const unsigned char uc_COM_WriteReply_CLCD_9700[TH_CLCD_9700_MAX_COM_WRITE_REPLY] = {
    0x40, 0x30, 0x31, 0x31, 0x35, 0x30, 0x30, 0x34, 0x35, 0x2A,
    0x0D, 0x0A
};     // write Slave to  Master reply (回复设置成功)


const unsigned char uc_COM_ReadCmd_CLCD_9700[TH_CLCD_9700_MAX_COM_READ_CMD] = {
    0x40, 0x30, 0x31, 0x30, 0x31, 0x34, 0x30, 0x2A, 0x0D
};    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_CLCD_9700[TH_CLCD_9700_MAX_COM_READ_REPLY] = {
    0x40, 0x30, 0x31, 0x30, 0x31, 0x30, 0x39, 0x43, 0x44, 0x32,
    0x34, 0x30, 0x33, 0x30, 0x39, 0x43, 0x34, 0x31, 0x35, 0x37,
    0x43, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x42, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x31, 0x30, 0x35,
    0x33, 0x30, 0x30, 0x36, 0x34, 0x33, 0x32, 0x2A, 0x0D, 0x0A
};     // read Slave to  Master reply


const unsigned char uc_COM_RunCmd_CLCD_9700[TH_CLCD_9700_MAX_COM_RUN_CMD] = {
    0x40, 0x30, 0x31, 0x35, 0x33, 0x30, 0x31, 0x31, 0x37, 0x37,
    0x2A, 0x0D
};    // run Master to  Slave cmd //


const unsigned char uc_COM_StopCmd_1_CLCD_9700[TH_CLCD_9700_MAX_COM_STOP_CMD_1] = {
    0x40, 0x30, 0x31, 0x35, 0x33, 0x30, 0x35, 0x31, 0x37, 0x33,
    0x2A, 0x0D
};    // stop1 Master to  Slave cmd
const unsigned char uc_COM_StopCmd_2_CLCD_9700[TH_CLCD_9700_MAX_COM_STOP_CMD_2] = {
    0x40, 0x30, 0x31, 0x35, 0x33, 0x30, 0x32, 0x31, 0x37, 0x34,
    0x2A, 0x0D
};    // stop2 Master to  Slave cmd



//----------------------------------------------------------------------------------------------------


void TH_CLCD_9700::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_CLCD_9700::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_CLCD_9700::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_CLCD_9700::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_CLCD_9700::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_CLCD_9700::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_CLCD_9700::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TH_CLCD_9700::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned char uc_Buffer[4][6];
// 	unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_CLCD_9700_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do { // 去丢[STX (1 byte)][Addr (2 byte)]
// 		if( (MTCI_CTRL::gR_CmdBuf[uw16_i]==uc_COM_ReadReply_CLCD_9700[0]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+1]==uc_COM_ReadReply_CLCD_9700[1]) &&\
// 			(MTCI_CTRL::gR_CmdBuf[uw16_i+2]==uc_COM_ReadReply_CLCD_9700[2]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+3]==uc_COM_ReadReply_CLCD_9700[3]) &&\
// 			(MTCI_CTRL::gR_CmdBuf[uw16_i+4]==uc_COM_ReadReply_CLCD_9700[4]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+77]==uc_COM_ReadReply_CLCD_9700[77]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+78]==uc_COM_ReadReply_CLCD_9700[78]) )

        if( (MTCI_CTRL::gR_CmdBuf[uw16_i]==uc_COM_ReadReply_CLCD_9700[0])
                //&& (MTCI_CTRL::gR_CmdBuf[uw16_i+1]==uc_COM_ReadReply_CLCD_9700[1])&&(MTCI_CTRL::gR_CmdBuf[uw16_i+2]==uc_COM_ReadReply_CLCD_9700[2])   //  机器地址01
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+3]==uc_COM_ReadReply_CLCD_9700[3])&&(MTCI_CTRL::gR_CmdBuf[uw16_i+4]==uc_COM_ReadReply_CLCD_9700[4]))     //  读回复命令01
            //&& (MTCI_CTRL::gR_CmdBuf[uw16_i+77]==uc_COM_ReadReply_CLCD_9700[77]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+78]==uc_COM_ReadReply_CLCD_9700[78]) )  // 包结束*+CR
        {   // 利用包头或包尾判断数据值。    //  机器地址01   //  读回复命令01

            if( (MTCI_CTRL::gR_CmdBuf[uw16_i+77]==uc_COM_ReadReply_CLCD_9700[77]) && (MTCI_CTRL::gR_CmdBuf[uw16_i+78]==uc_COM_ReadReply_CLCD_9700[78]) )  // 包尾判断。 // 包结束*+CR
            {   //  包已完整,进行处理

                //PresentValue = ((short int)(gR_CmdBuf[uw16_i+7]) << 12 |gR_CmdBuf[uw16_i+8]) << 8 |gR_CmdBuf[uw16_i+9]) << 4 | (gR_CmdBuf[uw16_i+10]) );
// 			   gR_PresentTempValue[ChanNo] = TH_CLCD_9700_asc2int(gR_CmdBuf[uw16_i+5], gR_CmdBuf[uw16_i+6], gR_CmdBuf[uw16_i+7], gR_CmdBuf[uw16_i+8]);   //  温度
// 			   ctrl_SetPresentValue(ChanNo, gR_PresentTempValue[ChanNo],SETPOINTVALUE_TEMPERATURE);
// 			   gR_PresentHumiValue[ChanNo] = TH_CLCD_9700_asc2int(gR_CmdBuf[uw16_i+9], gR_CmdBuf[uw16_i+10], gR_CmdBuf[uw16_i+11], gR_CmdBuf[uw16_i+12]);   //  湿度
// 			   ctrl_SetPresentValue(ChanNo, gR_PresentHumiValue[ChanNo],SETPOINTVALUE_HUMI);

                MTCI_datatypes::ctrl_String2Value_Hex(&MTCI_CTRL::gR_CmdBuf[uw16_i+5], &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]);      //  温度
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]/10,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
                MTCI_datatypes::ctrl_String2Value_Hex(&MTCI_CTRL::gR_CmdBuf[uw16_i+9], &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]);      //  湿度
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]/10,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);

                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

                return;
            }
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);
}



void TH_CLCD_9700::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_CLCD_9700_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //    return;

    for(MbChkSum = 0; MbChkSum < TH_CLCD_9700_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_CLCD_9700[MbChkSum];

    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+5);
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+9);

    //TH_CLCD_9700_AddTemp(&MTCI_CTRL::gW_TemperatureValue[ChanNo],MbBuf);

//		#ifdef COM_CMD_CHANNEL_DIFFER
//    if( ChanNo != 0 )
//			MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
//    #endif

    TH_CLCD_9700::AddCheck(32,MbBuf);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TH_CLCD_9700::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_CLCD_9700_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_CLCD_9700_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_CLCD_9700[MbChkSum];
    uw16_len = TH_CLCD_9700_MAX_COM_RUN_CMD;


//        #ifdef COM_CMD_CHANNEL_DIFFER

//	    if( ChanNo != 0 )
//	    {
//		    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
//        }
//        #endif

    //TH_CLCD_9700_AddCheck(8,MbBuf);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TH_CLCD_9700::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_CLCD_9700_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_CLCD_9700_MAX_COM_READ_CMD; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_CLCD_9700[MbChkSum];
    }

//		#ifdef COM_CMD_CHANNEL_DIFFER
//	    if( ChanNo != 0 )
//	    {
//		    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
//      }
//		#endif
//
//		TH_CLCD_9700_AddCheck(5,MbBuf);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TH_CLCD_9700::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//------------------------------------First Cmd----------------------------------------------------
//  发送停止命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_CLCD_9700_MAX_COM_STOP_CMD_1;
    unsigned char MbBuf[SEND_MAX_BUF];
// 	unsigned long int  uw32_DO;
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_CLCD_9700_MAX_COM_STOP_CMD_1; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_1_CLCD_9700[MbChkSum];
    uw16_len = TH_CLCD_9700_MAX_COM_STOP_CMD_1;

//     #ifdef COM_CMD_CHANNEL_DIFFER
//	 if( ChanNo != 0 )
//	 {
//	      MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
//   }
//	 #endif
//
//	 TH_CLCD_9700_AddCheck(8,MbBuf);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

//-------------------------------------Second Cmd--------------------------------------------------

    uw16_len = TH_CLCD_9700_MAX_COM_STOP_CMD_2;

    for(MbChkSum = 0; MbChkSum < TH_CLCD_9700_MAX_COM_STOP_CMD_2; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_2_CLCD_9700[MbChkSum];
    uw16_len = TH_CLCD_9700_MAX_COM_STOP_CMD_2;

//     #ifdef COM_CMD_CHANNEL_DIFFER
//	 if( ChanNo != 0 )
//	 {
//	      MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
//   }
//	 #endif
//
//	 TH_CLCD_9700_AddCheck(8,MbBuf);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

//------------------------------------Cmd end------------------------------------------------------

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}

short int TH_CLCD_9700::asc2int(unsigned char asc1,unsigned char asc2,unsigned char asc3,unsigned char asc4)
{
    unsigned char asc[4] = { asc1, asc2, asc3, asc4 };
    short int num[4] = {0,0,0,0};
    int sum = 0;
    for(int i=0; i<4; i++)
    {
        if( (asc[i]>= 'A') && (asc[i]<= 'F') )
        {
            num[i] = asc[i] - 55;
        }
        else if( (asc[i]>= 'a') && (asc[i]<= 'f') )
        {
            num[i] = asc[i] - 87;
        }
        else if( (asc[i]>= '0') && (asc[i]<= '9') )
        {
            num[i] = asc[i] - 48;
        }
    }



// 	sum = num[0] + num[1]*16 + num[2]*16*16 + num[3]*16*16*16;

    sum = num[0]*16*16*16 + num[1]*16*16 + num[2]*16 + num[3];

    if(sum > 32767)
        sum = 32767;
    return (short int)sum;
}

void TH_CLCD_9700::AddTemp(short int * pValue, unsigned char* str)
{
    short int value = *pValue;
    short int num[4] = {0,0,0,0};
    unsigned char asc[4] = {0x00,0x00,0x00,0x00};
    for(int i=0; i<4; i++)
    {
        num[i] = value%16;
        value /= 16;

        if( (num[i]>= 0) && (num[i]<= 9) )
        {
            asc[i] = num[i] + 48;
        }
        else if( (num[i]>= 10) && (num[i]<= 15) )
        {
            asc[i] = num[i] + 55;
        }
    }
// 	str[5] = asc[0];
// 	str[6] = asc[1];
// 	str[7] = asc[2];
// 	str[8] = asc[3];

    str[5] = asc[3];
    str[6] = asc[2];
    str[7] = asc[1];
    str[8] = asc[0];
}

void TH_CLCD_9700::AddCheck(unsigned int bits, unsigned char* str)
{
    char checkH = 0x00;
    char checkL = 0x00;
    char XorSum = 0x00;
    unsigned short sum = 0;
    unsigned short sumH = 0;
    unsigned short sumL = 0;
    for(int i=0; i<bits; i++)
    {
        XorSum ^= str[i];
    }
    sum = (unsigned int)XorSum;
    sumL = sum%16;
    sumH = sum/16;

    if( (sumL >= 0) && (sumL <= 9) )
    {
        checkL = sumL + 48;
    }
    else if( (sumL >= 10) && (sumL <= 15) )
    {
        checkL = sumL + 55;
    }

    if( (sumH >= 0) && (sumH <= 9) )
    {
        checkH = sumH + 48;
    }
    else if( (sumH >= 10) && (sumH <= 15) )
    {
        checkH = sumH + 55;
    }

// 	str[bits] = checkL;
// 	str[bits+1] = checkH;

    str[bits] = checkH;
    str[bits+1] = checkL;
}