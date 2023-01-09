/*****************************************************************************
// 程序文件      :TH_SIEMENS_S7_SAIA.Cpp    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2017.10.10
*****************************************************************************/


#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// TH_SIEMENS_S7_SAIA  串口通信方式  MODBUS TCP/IP  传输方式：RTU模式 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以浮点数操作)
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 F0 00 02 FF 02 XX XX XX XX     (XX设置温度浮点数先传高位再传低位)
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 F8 00 02 FF 02 XX XX XX XX     (XX设置湿度浮点数先传高位再传低位)
const unsigned char uc_COM_WriteTemp_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xF0,
    0x00, 0x02, 0xFF, 0x02, 0x41, 0xB0, 0x00, 0x00
};     // write Master to  Slave cmd  (设置温度22度)

const unsigned char uc_COM_WriteHumi_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xF8,
    0x00, 0x02, 0xFF, 0x02, 0x42, 0x5C, 0x00, 0x00
};     // write Master to  Slave cmd  (设置湿度55%)

//  从机回复时，Reply一模一样：
//  Reply CMD: {[MBAP (8 byte)] [Error Field] [Empty Field] [Length Empty field] }
//  Reply MODEL: 53 35 10 01 03 04 0F 03 00 FF 07
const unsigned char uc_COM_WriteReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_REPLY] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x04, 0x0F, 0x03, 0x00, 0xFF, 0x07
};     // write Slave to  Master reply (回复设置温度)

//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] }
//  MODEL: 53 35 10 01 03 05 03 08 01 3E 00 40 00 02 FF 02 温度
//  MODEL: 53 35 10 01 03 05 03 08 01 3E 00 44 00 02 FF 02 湿度
const unsigned char uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x05, 0x03, 0x08, 0x01, 0x3E, 0x00, 0x40,
    0x00, 0x02, 0xFF, 0x02
};    // read Master to  Slave cmd

const unsigned char uc_COM_ReadHumiCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x05, 0x03, 0x08, 0x01, 0x3E, 0x00, 0x44,
    0x00, 0x02, 0xFF, 0x02
};    // read Master to  Slave cmd

//  从机回复时，
//  Reply CMD:  {[MBAP (8 byte)] [Error Field] [Empty Field] [Length Empty field]
//         [Registers Value high byte] [Registers Value low byte]}
//  Reply MODEL: 53 35 10 01 03 06 0F 03 00 FF 07 XX XX XX XX   (XX 温度/湿度 浮点数 先传高位再传低位)
const unsigned char uc_COM_ReadReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x06, 0x0F, 0x03, 0x00, 0xFF, 0x07,
    0x41,0xB0, 0x00, 0x00
};     // read Slave to  Master reply  //(回复温度22度)

//  3-M)要求运行命令主机到从机
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 E8 00 01 FF 02 03 01

const unsigned char uc_COM_RunCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD] =  {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xE8,
    0x00, 0x01, 0xFF, 0x02, 0x03, 0x01
};    // read Master to  Slave cmd

//  4-M)要求停止命令主机到从机命令
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 E8 00 01 FF 02 00 00

const unsigned char uc_COM_StopCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD] =  {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xE8,
    0x00, 0x01, 0xFF, 0x02, 0x00, 0x00
};     // write Master to  Slave cmd

//  从机回复时，Reply一模一样：
//  Reply CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] }
//  Run  Reply MODEL: 00 00 00 00 00 06 01 10 00 C8 00 01
//  Stop Reply MODEL: 00 00 00 00 00 06 01 10 00 C8 00 01
// const unsigned char uc_COM_RUNorStopReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_RUNorSTOP_REPLY] = {
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0xC8,
//     0x00, 0x01
// };     // write Slave to  Master reply (回复设置温度)

//
// TH_SIEMENS_S7_SAIA  串口通信方式  MODBUS TCP/IP  传输方式：RTU模式 //
//----------------------------------------------------------------------------------------------------

// 注意：调整以下参数  #define  COM_INTERVAL_WRITE_AND_READ不使用间隔写和读功能
//   <q> COM INTERVAL WRITE AND READ Enable:  Off
// #define  COM_INTERVAL_WRITE_AND_READ                 0         //是否使用串口间隔写和读功能 为1表示使用间隔写和读功能，为0表示不使用间隔写和读功能
// #define PUBLIC_REPEAT_NUM	      (3)    // 命令最大公共重复次数
// #define CMD_INTERVALTIME	     (200*10)   // 每次发送命令间隔时间 200毫秒

//static unsigned short uw16_V1=0,uw16_V2=0,uw16_V3=0,uw16_V4=0;
static unsigned char uc_T_Update_Limit_Counter=0;
static unsigned char uc_H_Update_Limit_Counter=0;

void TH_SIEMENS_S7_SAIA::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SIEMENS_S7_SAIA::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SIEMENS_S7_SAIA::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    //MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 3; // PUBLIC_REPEAT_NUM;
    //MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}

/*----*/
unsigned char TH_SIEMENS_S7_SAIA::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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


    // 避过交替出现的读错数值显示。
    unsigned short int uw16_V = 0;
    if(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] == SETPOINTVALUE_TEMPERATURE)
    {
        uw16_V = abs(MTCI_CTRL::str_Uart[ChanNo].gR_TempLastValue[0] - Value);
        if(uw16_V > 200)  // 2分钟才升10度
        {
            if(++uc_T_Update_Limit_Counter < 5)
                return 0;
        }
        else
            uc_T_Update_Limit_Counter = 0;
    }
    else if(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] == SETPOINTVALUE_HUMI)
    {
        uw16_V = abs(MTCI_CTRL::str_Uart[ChanNo].gR_HumiLastValue[0] - Value);
        if(uw16_V > 100)
        {
            if(++uc_H_Update_Limit_Counter < 5)
                return 0;
        }
        uc_H_Update_Limit_Counter = 0;
    }
    return  1;
}

void TH_SIEMENS_S7_SAIA::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    unsigned char *pCmd;
    short int PresentValue = 0;
    short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    unsigned char uc_n = 0;
    unsigned int uw16_Total;
    union UW_F
    {
        float f_member;
        unsigned char uc_member[4];  // 低位在前
    } F_data;
		
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_SIEMENS_S7_SAIA[5]    //  0x06
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_SIEMENS_S7_SAIA[7] // 0x03
                && MTCI_CTRL::gR_CmdBuf[uw16_i+3] == uc_COM_ReadReply_SIEMENS_S7_SAIA[8]) // 0x00
        {   // 利用包头或包尾判断数据值。

            unsigned int uw16_k = uw16_Total - uw16_i + 5 ;  // 从uw16_i开始到命令头部的字节数
            unsigned int uw16_m = uw16_Total - (uw16_i + 6);    // 从uw16_i+3开始到命令尾巴部的4字节数4

            long g_Time = CommonFunc::GetTimeTicks( &MTCI_CTRL::gR_ReadCmd_Stamp[ChanNo]);  // 接收超时时间
            if((g_Time < 240)||(g_Time > MTCI_CTRL::uw32_CMD_INTERVALTIME*1.8))   //  小于[READ_REPLY*2*10] or 大于一读一写间隔时间(N*2)
            {
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间    600ms-(344)经验值   344/13=26.46
                return;
            }

            if((uw16_k < TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)
                    ||(uw16_m < 4)
                    ||(MTCI_CTRL::g_Cmd_Flag[ChanNo] == SEND_COM_RUN_CMD)
                    ||(MTCI_CTRL::g_Cmd_Flag[ChanNo] == SEND_COM_STOP_CMD)
                    ||(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] != uc_Temp4Humi_Flag)) // 避免出现温度与湿度交错填放错误。
            {
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间
                return;
            }

            for(uc_n=0; uc_n<4; uc_n++)
                F_data.uc_member[uc_n]	=	MTCI_CTRL::gR_CmdBuf[uw16_i+9-uc_n];
            PresentValue = (short int)(F_data.f_member*10);//  温度/湿度

            if( TH_SIEMENS_S7_SAIA::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间
            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_SIEMENS_S7_SAIA::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned char uc_n = 0;
    union UW_F
    {
        float f_member;
        unsigned char uc_member[4]; // 低位在前
    } F_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumi_SIEMENS_S7_SAIA[MbChkSum];
        F_data.f_member = (float) (MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]/10);
        for(uc_n=0; uc_n<4; uc_n++)
            MbBuf[uw16_len-1-uc_n] = F_data.uc_member[uc_n];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTemp_SIEMENS_S7_SAIA[MbChkSum];
        F_data.f_member = (float) (MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL]/10);
        for(uc_n=0; uc_n<4; uc_n++)
            MbBuf[uw16_len-1-uc_n] = F_data.uc_member[uc_n];
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理
}


void TH_SIEMENS_S7_SAIA::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_SIEMENS_S7_SAIA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_SIEMENS_S7_SAIA::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_SIEMENS_S7_SAIA[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[MbChkSum];
#endif
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_SIEMENS_S7_SAIA::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_SIEMENS_S7_SAIA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



