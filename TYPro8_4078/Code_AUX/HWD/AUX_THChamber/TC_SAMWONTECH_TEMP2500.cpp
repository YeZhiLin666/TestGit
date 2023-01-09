/*****************************************************************************
// 程序文件      :TC_SAMWONTECH_TEMP2500.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  SAMWONTECH(韩国三元)TEMP2500 controller //
//  串口通信方式  SumCheck  传输方式：ASCII模式 有SUM校验
//  特别注意：当使用停止和运行命令，跟读和写命令不一致时，机制必须修改。


// 阅读操作说明书
// I)Page    33/123
// A)Operation Mode:  Select Fix mode.
// B)Time Mode:  Select OFF mode.
// II)Page   29/123
// 2.6.3 Fix Stop Screen
// III)Page  31/123
// 2.6.4 Fix Run Screen


// 默认常温时温度值为22℃度

// communication command structure:
// //-------------------------------------------------------
// (1)  (2)     (3)      (4)  (5)   (6)   (7) (8)
// STX  Address Command   ,   Data  SUM   CR   LF
// //-------------------------------------------------------
// (1) STX(Start of Text)
// Beginning letter of Communication Command
// Indicate the beginning of Communication Command having a code value (0x02) with a STX (Start of Text), which is a letter of Ascii.
// (2) Address
// Indicate the ADDRESS which is a number of TEMP880 that you intend to communicate with.
// (3) Command
// Specify the command from an upper device (See 4.3.2~4.3.9).
// (4) ‘,’Divider
// Indicate the Divider which separates the Command and Data by comma (',').
// (5) Data part
// Indicate the letter line which has a regular form of a letter line followed the Communication Command Rule
// (6) Check Sum
// Indicate the inferior rank of 2-byte to D by adding all letters from the letter of after STX to the letter
// of before SUM using the Ascii code and Use only when the protocol of TEMP880 is “the basic
// protocol of “PCL!” + Check Sum”
// check sum = HEX[(2)+(3)+(4)+(5)]
// (7,8) CR,LF Vertical Section letter
// Indicate the end of Communication Command as CR(0x0D), LF(0x0A) by using the Ascii code.
// ([stx] = 0x02, [cr] = 0x0d, [lf] = 0x0a)
// - Para. number : 1~32.
// - dddd : Indicates a character string in hexadecimal format.


//-------------------------------------------------------
//  1-M)设置温度主机到从机 (以字节操作)
// {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][No. of contents (2字节)][,(1字节)][D-Reg.No1 (4字节)][,(1字节)][dddd-1 (4字节)][,(1字节)][dddd-? (4字节)][,(1字节)][dddd-? (4字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Transmission : [stx]01WRD,01,0104,00DCDF[cr][lf]
// 02 30 31 57 52 44 2C 30 31 2C 30 31 30 34 2C 30 30 44 43 44 46 0D 0A

// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 34 2C XX XX XX XX [SUM] 0D 0A
// (XXXXXXXX温度值先传高位再传低位)
// 使用full完整方式时，显示值=温度值/10;    如：22(220/10=DC/A)度
// [正常码字方式: (45度*10)  450 = 0X01C2; (-60度*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]
//
// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][OK (2字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//
const unsigned char uc_COM_WriteCmd_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X34, 0x2C, 0x30, 0x30, 0x44, 0x43, 0X44,
    0X46, 0x0D, 0x0A
};     // write Master to  Slave cmd  (设置温度22度)

//  从机回复时，Reply设置值：
const unsigned char uc_COM_WriteReply_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_REPLY] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0X31,
    0X34, 0x0D, 0x0A
};     // write Slave to  Master reply (回复设置成功)

//  2-M)读取温度主机到从机  (以字节操作)
// {{[STX (1 byte)][Addr (2 byte)][RRD (3字节)][,(1字节)][No. of contents (2字节)][,(1字节)][D-Reg.No1 (4字节)][,(1字节)][D-Reg.No? (4字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Transmission : [stx]01RRD,01,0001C3[cr][lf]
// 02 30 31 52 52 44 2C 30 31 2C 30 30 30 31 43 33 0D 0A
//
// 02 30 31 52 52 44 2C 4F 4B 2C XX XX XX XX [SUM] 0D 0A
// (XXXXXXXX温度值先传高位再传低位)
// 使用full完整方式时，显示值=温度值/10;    如：22(220/10=DC/A)度
// [正常码字方式: (45度*10)  450 = 0X01C2; (-60度*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]
//
//从机回复时，Reply
//Response : [stx]01RRD,OK,00DC22[cr][lf]
//02 30 31 52 52 44 2C 4F 4B 2C 30 30 44 43 32 32 0D 0A
const unsigned char uc_COM_ReadCmd_TEMP2500[TC_TEMP2500_MAX_COM_READ_CMD] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x30, 0x31, 0x2C,
    0x30, 0x30, 0X30, 0X31, 0x43, 0X33, 0x0D, 0x0A
};    // read Master to  Slave cmd (读取)
//从机回复时，Reply
const unsigned char uc_COM_ReadReply_TEMP2500[TC_TEMP2500_MAX_COM_READ_REPLY] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x2C,
    0x30, 0x30, 0X44, 0X43, 0X32, 0X32, 0x0D, 0x0A
};     // read Slave to  Master reply  //(回复温度22度，就是220)
//
//  3-M)要求运行命令主机到从机
// {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][No. of contents (2字节)][,(1字节)][D-Reg.No1 (4字节)][,(1字节)][dddd-1 (4字节)][,(1字节)][dddd-? (4字节)][,(1字节)][dddd-? (4字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Transmission : [stx]01WRD,02,0106,0001,0102,000198[cr][lf]
// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 36 2C 30 30 30 31 2C 30 31 30 32 2C 30 30 30 31 39 38 0D 0A
//
// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][OK (2字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//

const unsigned char uc_COM_RunCmd_TEMP2500[TC_TEMP2500_MAX_COM_RUN_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X36, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X32, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x39,
    0x38, 0x0D, 0x0A
};    // read Master to  Slave cmd

//  4-M)要求停止命令主机到从机命令
// {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][Para.number (2字节)][,(1字节)][D-Reg.No1 (4字节)][,(1字节)][dddd-1 (4字节)][,(1字节)][dddd-? (4字节)][,(1字节)][dddd-? (4字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Transmission : [stx]01WRD,02,0106,0001,0102,00049B[cr][lf]
// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 36 2C 30 30 30 31 2C 30 31 30 32 2C 30 30 30 34 39 42 0D 0A

// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3字节)][,(1字节)][OK (2字节)][SUM (2字节)][CR (1字节)][LR (1字节)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A

const unsigned char uc_COM_StopCmd_TEMP2500[TC_TEMP2500_MAX_COM_STOP_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X36, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X32, 0x2C, 0x30, 0x30, 0x30, 0x34, 0x39,
    0x42, 0x0D, 0x0A
};    // read Master to  Slave cmd


//
//  SAMWONTECH(韩国三元)TEMP2500   串口通信方式  SumCheck  传输方式：ASCII模式 有SUM校验   //
//----------------------------------------------------------------------------------------------------



void TC_TEMP2500::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_TEMP2500::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_TEMP2500::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_TEMP2500::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_TEMP2500::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_TEMP2500::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_TEMP2500::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_TEMP2500::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据

    //static  unsigned long  uw32_Test[100];
    //static  unsigned char  uc_Test_s;

    unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    //unsigned char uc_Buffer[4][6];
// 	unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;

    uw16_Total = UARTFunc::uart_bytenum(ChanNo);
    if( uw16_Total < TC_TEMP2500_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

    if(uw16_Total > MAX_UART_BUF_SIZE )   // 取出数据最大缓冲区保护，因为它不能用循环使用的办法,保持最后有一些空间不溢出。
    {
        uw16_Total = MAX_UART_BUF_SIZE;   // 取出数据最大缓冲区保护，因为它不能用循环使用的办法,保持最后有一些空间不溢出。

    }


    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do { // 去丢[STX (1 byte)][Addr (2 byte)]
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TEMP2500[3] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TEMP2500[4] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TEMP2500[5]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+4] == uc_COM_ReadReply_TEMP2500[7] && MTCI_CTRL::gR_CmdBuf[uw16_i+5] == uc_COM_ReadReply_TEMP2500[8] )
        {   // 利用包头或包尾判断数据值。

            //if(gR_CmdBuf[uw16_i+13] == uc_COM_ReadReply[16] && gR_CmdBuf[uw16_i+14] == uc_COM_ReadReply[17])  // 包尾判断。
            if(MTCI_CTRL::gR_CmdBuf[uw16_i+13] == uc_COM_ReadReply_TEMP2500[16] && MTCI_CTRL::gR_CmdBuf[uw16_i+14] == uc_COM_ReadReply_TEMP2500[17])  // 包尾判断。
            {   //  包已完整,进行处理

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+7]; // [READ_REPLY_POINT_TEMPVAlUE - 3]=7   // 去丢[STX (1 byte)][Addr (2 byte)]
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]);   //  温度
                //PresentValue = (short int) (atof(pCmd)*10);  // 放大10倍上传
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL],SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

                return;
            }
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);
}

void TC_TEMP2500::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令
    unsigned int  uw16_len = TC_TEMP2500_MAX_COM_WRITE_CMD;
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //    return;

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_TEMP2500[MbChkSum];

    //register_data = gW_TemperatureValue[ChanNo];
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TC_TEMP2500_WRITE_CMD_POINT_TEMPVAlUE);

    //ctrl_Value2String(register_data,MbBuf+3+6);
    //gcvt((double) (register_data/10), 6, MbBuf+3+6);    // 缩小10倍处理 */

    //register_data = gW_HumiValue[ChanNo];
    //ctrl_Value2String(&gW_HumiValue[ChanNo],MbBuf + WRITE_CMD_POINT_HUMIVAlUE);
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP2500_MAX_COM_WRITE_CMD - 5) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP2500_MAX_COM_WRITE_CMD - 4);
    MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD - 4] = MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD-2];
    MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD - 3] = MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD-1];  // 交换数据
    MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD - 2] = uc_COM_WriteCmd_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_CMD-2];
    MbBuf[TC_TEMP2500_MAX_COM_WRITE_CMD - 1] = uc_COM_WriteCmd_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_CMD-1]; // 填写0X0D,0X0A 结尾

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_TEMP2500::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  uw16_len = TC_TEMP2500_MAX_COM_RUN_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TEMP2500[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP2500_MAX_COM_WRITE_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP2500_MAX_COM_RUN_CMD - 4);
        MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD - 4] = MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD - 3] = MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD-1];  // 交换数据
        MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD - 2] = uc_COM_RunCmd_TEMP2500[TC_TEMP2500_MAX_COM_RUN_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_RUN_CMD - 1] = uc_COM_RunCmd_TEMP2500[TC_TEMP2500_MAX_COM_RUN_CMD-1]; // 填写0X0D,0X0A 结尾
    }
#endif


    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_TEMP2500::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_TEMP2500_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_TEMP2500[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP2500_MAX_COM_READ_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP2500_MAX_COM_READ_CMD - 4);
        MbBuf[TC_TEMP2500_MAX_COM_READ_CMD - 4] = MbBuf[TC_TEMP2500_MAX_COM_READ_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_READ_CMD - 3] = MbBuf[TC_TEMP2500_MAX_COM_READ_CMD-1];  // 交换数据
        MbBuf[TC_TEMP2500_MAX_COM_READ_CMD - 2] = uc_COM_ReadCmd_TEMP2500[TC_TEMP2500_MAX_COM_READ_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_READ_CMD - 1] = uc_COM_ReadCmd_TEMP2500[TC_TEMP2500_MAX_COM_READ_CMD-1]; // 填写0X0D,0X0A 结尾
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TC_TEMP2500::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_TEMP2500_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TEMP2500[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP2500_MAX_COM_STOP_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP2500_MAX_COM_WRITE_CMD - 4);
        MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD - 4] = MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD - 3] = MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD-1];  // 交换数据
        MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD - 2] = uc_COM_StopCmd_TEMP2500[TC_TEMP2500_MAX_COM_STOP_CMD-2];
        MbBuf[TC_TEMP2500_MAX_COM_STOP_CMD - 1] = uc_COM_StopCmd_TEMP2500[TC_TEMP2500_MAX_COM_STOP_CMD-1]; // 填写0X0D,0X0A 结尾
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}

unsigned char TC_TEMP2500::LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen)
{
//  加载校验值到发送包里面。
    int uw32_Count = usDataLen - 5;
    short int  register_data;
    if(uw32_Count <= 0)
        return (0);

    //register_data = MTCI_datatypes::CheckXorSum(&puchMsg[1],usDataLen - 5) & 0X00FF;
    register_data = MTCI_datatypes::CheckSum(&puchMsg[1],usDataLen - 5) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,(puchMsg + usDataLen - 4));
    puchMsg[usDataLen - 4] = puchMsg[usDataLen - 2];
    puchMsg[usDataLen - 3] = puchMsg[usDataLen - 1];  // 交换数据
    puchMsg[usDataLen - 2] = uc_COM_WriteCmd_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_CMD - 2];
    puchMsg[usDataLen - 1] = uc_COM_WriteCmd_TEMP2500[TC_TEMP2500_MAX_COM_WRITE_CMD - 1]; // 填写0X2A,0X0D 结尾

    return (1);
}



