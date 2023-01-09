/*****************************************************************************
// 程序文件      :TH_SIEMENS_S7_226CN.C    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


#if(DEBUG_TRACE_RECORD== 1)
extern UWord32 uw32_TimeStamp_Test[100];
extern UWord16 uw16_Put;
extern UWord16 uw16_Get;
extern UWord32 uw32_Count[10];
#endif


//------------------------------------------------------------------------------------------------
// TH_SIEMENS_S7-226CN  串口通信方式  ACSII  传输方式：无检验 //
//

// 默认常温时温度值为22℃度
// 默认常态时湿度值为55%RH

//  1-M)设置温度主机到从机 (以字Word操作)
//  CMD: >MMWDUUUUDDDDDDDDr
//           MM：机号(在编程时因该转换为16进制数据)  WD：写命令  UUUU：S7200 内部V 存储器地址（在编程时因该转换为16 进制数据）
//           DDDDDDDD：从指定的V单元开始的4字节数据，每2个字符代表一个字节。r：回车字符(0X0D)
//  MODEL: >00WD03C400DC0226r
const unsigned char uc_COM_WriteTempCmd_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x34, 0x30,
    0x30, 0x44, 0x43, 0x30, 0x32, 0x32, 0x36, 0x0D
};     // write Master to  Slave cmd  (设置温度22度，就是220)  // 采用整型数方式

//  MODEL:>00WD035441B00000r
//0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x35, 0x34, 0x34,
//0x31, 0x42, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0D};     // write Master to  Slave cmd  (设置温度22度，就是220)  // 采用浮点数方式


//设置值=湿度值*10;    如：55(55*10),85(85*10) %湿度Humi.
//  MODEL: >00WD03C602260000r
const unsigned char uc_COM_WriteHumiCmd_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x36, 0x30,
    0x32, 0x32, 0x36, 0x30, 0x30, 0x30, 0x30, 0x0D
};     // write Master to  Slave cmd  (设置湿度55%，就是550)   // 采用整型数方式

//  MODEL: >00WD035842480000r
//0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x35, 0x38, 0x34,
//0x32, 0x34, 0x38, 0x30, 0x30, 0x30, 0x30, 0x0D};     // write Master to  Slave cmd  (设置湿度55%，就是550)  // 采用浮点数方式

//  从机回复时，Reply一模一样：
//  Reply CMD: <MMVDUUUUaabbccddee…r      <MMVDUUUU：响应命令头(0X3E)   aabbccddee：从指定的V单元开始的128字节，每2个字符代表一个字节。  r：回车字符(0X0D)
//  MODEL: <00WD03C4r
const unsigned char uc_COM_WriteReply_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x34, 0x0D
};     // write Slave to  Master reply

//  2-M)读取温度主机到从机  (以字Word操作)
//  CMD: >MMVDUUUUr        >：起始字符(0X3E)    MM：机号(在编程时因该转换为16进制数据)  VD：读命令  UUUU：S7200 内部V 存储器地址（在编程时因该转换为16 进制数据） r：回车字符(0X0D)
//  MODEL: >00VD0324r
const unsigned char uc_COM_ReadTempCmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x32, 0x34, 0x0D
};    // read Master to  Slave cmd (读取温度)

//  MODEL: >00VD035Cr
const unsigned char uc_COM_ReadHumiCmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x35, 0x43, 0x0D
};    // read Master to  Slave cmd (读取湿度)


//  Reply CMD:  <MMVDUUUUaabbccddee…r      <MMVDUUUU：响应命令头(0X3C)    aabbccddee：从指定的V单元开始的128字节，每2个字符代表一个字节。  r：回车字符(0X0D)
//  Reply MODEL: <00VD032441A00000000000000000000040E0000040E00000250014600400000040E000000000000000000000000000000000000000000000000100000000000000030000r
const unsigned char uc_COM_ReadReply_S7_226CN[TH_S7_226CN_MAX_COM_READ_REPLY] = {
    0x3C, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x32, 0x34, 0x34,
    0x31, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x34, 0x30, 0x45, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x34, 0x30, 0x45, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32,
    0x35, 0x30, 0x30, 0x31, 0x34, 0x36, 0x30, 0x30, 0x34, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x45, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x33, 0x30, 0x30, 0x30, 0x30, 0x0D
};   // read Slave to  Master reply  //(回复温度22度，就是220)

//  3-M)要求运行命令主机到从机
//  MODEL: >00WD038800010000r
const unsigned char uc_COM_RunCmd_S7_226CN_T[TH_S7_226CN_MAX_COM_RUN_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x0D
};    // Run Master to  Slave cmd

//  MODEL: >00WD038800010002r
const unsigned char uc_COM_RunCmd_S7_226CN_TH[TH_S7_226CN_MAX_COM_RUN_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x32, 0x0D
};    // Run Master to  Slave cmd

//  4-M)要求停止命令主机到从机命令
//  MODEL: >00WD038800000000r
const unsigned char uc_COM_StopCmd_S7_226CN_T[TH_S7_226CN_MAX_COM_STOP_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0D
};     // Stop Master to  Slave cmd

//  MODEL: >00WD038800000002r
const unsigned char uc_COM_StopCmd_S7_226CN_TH[TH_S7_226CN_MAX_COM_STOP_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x0D
};     // Stop Master to  Slave cmd

//  5-M)读取DI通道命令主机到从机命令
//  MODEL: >00VD036Cr
const unsigned char uc_COM_ReadDICmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x36, 0x43, 0x0D
};    // read Master to  Slave cmd (读取DI)


//
// S7_226CN  串口通信方式  ASCII  传输方式：无校验 //
//----------------------------------------------------------------------------------------------------


void TH_S7_226CN::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_S7_226CN::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_S7_226CN::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_S7_226CN::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_S7_226CN::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_S7_226CN::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_S7_226CN::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
}

/*----*/
unsigned char TH_S7_226CN::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_S7_226CN::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    unsigned char  Modle = 1;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;
    union  F_data f_Value;
    short int temp = 0;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_S7_226CN_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == (ChanNo+S7_226CN_ASCII_FIRST)
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X32 || MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X35)
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X34 || MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43)   // VD 0324 035C读取温度、读取湿度
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D))  // \r
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_S7_226CN[2]
                && (gR_CmdBuf[uw16_i+7] == 0X32 || gR_CmdBuf[uw16_i+7] == 0X35)
                && (gR_CmdBuf[uw16_i+8] == 0X34 || gR_CmdBuf[uw16_i+8] == 0X43)    // VD 0324 035C读取温度、读取湿度
                && (gR_CmdBuf[uw16_i+137] == 0X0D))  // \r
#endif
        {   // 利用包头或包尾判断数据值。
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_S7_226CN_MIN_RESPONSE_CMD_NUM)
// 			   return;	// 避免温度或湿度交替接收数据时,更新错误值。

            // 将十六进制8位变换成4位。  如'E' 0X45-0X30变换成0X0E
            f_Value.uc_d[0] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+15]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+16]);
            f_Value.uc_d[1] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+13]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+14]);
            f_Value.uc_d[2] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+11]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+12]);
            f_Value.uc_d[3] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+9])  << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+10]);

            PresentValue = (short int) (f_Value.f_d * 10);

            if( TH_S7_226CN::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;

                if(MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43)   //  VD  035C读取湿度   // 在处理数据又多又慢的情况下，再作补救切换。
                    uc_Temp4Humi_Flag = SETPOINTVALUE_HUMI;
                else
                    uc_Temp4Humi_Flag = SETPOINTVALUE_TEMPERATURE;
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            if((MTCI_CTRL::gC_IsRunning[ChanNo])&&(MTCI_CTRL::uc_LockModleT[ChanNo]==0))   // 正式控制设置控制值，未锁定温度模式
            {
                temp = abs(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]-MTCI_CTRL::gR_PV_Back[ChanNo]);  //  死区范围-+0.5度 10.5~9.5
                if(temp < 1)   // 不变化则认为不工作。
                {
                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
                    return;
                }

                if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] <= (100+5))     // 温度值为10.5℃度保护
                    Modle = 0; //  下一层用于判断发送温、湿命令
                else if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] >= (90+5))
                    Modle = 1;

                if(++MTCI_CTRL::uc_ReadCount[ChanNo] >= 10)
                {
                    MTCI_CTRL::gR_PV_Back[ChanNo] = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                    if(MTCI_CTRL::uc_RunTH[ChanNo] != Modle)
                    {
                        MTCI_CTRL::uc_RunTH[ChanNo] = Modle;
                        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;   // 进行温、湿模式切换
                        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
                    }
                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
                }
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            return;
        }

#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == (ChanNo+S7_226CN_ASCII_FIRST)
                && MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X36
                && MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43   //  VD 036C 读取DI
                && MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D)  // \r
#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_S7_226CN[2]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X36
                && MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43    //  VD 036C 读取DI
                && MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D)  // \r
#endif
        {
            // DI为上传报警信息
//             0001 1010 0000 0000 0000 0000 0000 0000
//             31位                15位               0位
//             如果某位的二进制为1，则表示对应报警发生，具体含义如下表：
//             报警位报警说明HMI显示
//             0X01000000  为CM1超压报警     IO  0.0接入24V电压
//             0X02000000  为CM1油压报警     IO  0.1接入24V电压
//             0X04000000  为CM1热保护报警   IO  0.2接入24V电压
//             0X08000000  为相序/缺相报警   IO  0.3接入24V电压

//             0X10000000  为CM2超压报警     IO  0.4接入24V电压
//             0X20000000  为CM2油压报警     IO  0.5接入24V电压
//             0X40000000  为CM2热保护报警   IO  0.6接入24V电压
//             0X80000000  为超温报警        IO  0.7接入24V电压

//             0X00010000  为鼓风热保护报警  IO  1.0接入24V电压
//             0X00020000  为CM3超压报警     IO  1.1接入24V电压
//             0X00040000  为CM3油压报警     IO  1.2接入24V电压
//             0X00080000  为CM3热保护报警   IO  1.3接入24V电压

//             0X00100000  为加热器过流报警  IO  1.4接入24V电压
//             0X00200000  为加湿器过流报警  IO  1.5接入24V电压
//             0X00400000  为制冷机缺水报警  IO  1.6接入24V电压
//             0X00800000  为加湿缺水报警    IO  1.7接入24V电压

            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+9] - 0X30) << 12 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+10]- 0X30) << 8
                            |(short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+11]- 0X30) << 4 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+12]- 0X30));//

            MTCI_CTRL::gR_PresentValue_DI32[ChanNo] = MTCI_CTRL::gR_PresentValue_DI32[0];  // 任何串口都默认0号报警
            MTCI_CTRL::gR_PresentValue_DI32[0] = (MTCI_CTRL::gR_CmdBuf[uw16_i+9] - 0X30) << 28 | (MTCI_CTRL::gR_CmdBuf[uw16_i+10]- 0X30) << 24
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+11] - 0X30) << 20 | (MTCI_CTRL::gR_CmdBuf[uw16_i+12]- 0X30) << 16
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+13] - 0X30) << 12 | (MTCI_CTRL::gR_CmdBuf[uw16_i+14]- 0X30) << 8
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+15] - 0X30) << 4  | (MTCI_CTRL::gR_CmdBuf[uw16_i+16]- 0X30);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

#if AUX_MULTI_FUNCTION_BOARD == 1
            // 多功能板：  4号为温控箱UART 温度，5号为温控箱UART 湿度; 6号温控箱UART 16DI;7号温控箱UART 16D0;
            for(unsigned char boardIdx = 6; boardIdx<MAX_BOARD_PER_AUXMCU; boardIdx++)
            {
                if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_DI)
                {
                    unsigned char auxChnIdx = 0;
                    unsigned char auxDI = 0;
                    unsigned char DI_D[4];
                    unsigned long int DI_32Value = gR_PresentValue_DI32[0];
                    DI_32Value = (DI_32Value>>16);
                    DI_D[0] = (DI_32Value>>8)&0X0F;
                    DI_D[1] = (DI_32Value>>12)&0X0F;
                    DI_D[2] = DI_32Value&0X0F;
                    DI_D[3] = (DI_32Value>>4)&0X0F;
                    DI_32Value = DI_D[0]|(DI_D[1]<<4)|(DI_D[2]<<8)|(DI_D[3]<<12);
                    //while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
                    while(auxChnIdx<16)
                    {
                        auxDI = (unsigned char) ((DI_32Value>>auxChnIdx)&0X00000001);  // 仅用高16位值
                        if(auxDI)
                            Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 1;
                        else
                            Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 0;
                        auxChnIdx++;
                    }
                }
            }
#endif

#if(DEBUG_TRACE_RECORD== 1)
            uw32_Count[2]++;
#endif

            return;
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_S7_226CN::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制命令,主要入口，里面带写值，写运行分支
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    if(MTCI_CTRL::gW_T_Back[ChanNo] != MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL])
    {
        if(MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL] <= 105)     // 温度值为10.5℃度保护
        {
            MTCI_CTRL::uc_LockModleT[ChanNo] = 1;
            MTCI_CTRL::uc_RunTH[ChanNo] = 0; //  下一层用于判断发送温、湿命令
        }
        else
        {
            if(MTCI_CTRL::gW_HumiFlag[ChanNo] == 1)    //  判断是否控制湿度，控制则温湿模式
            {
                MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
                MTCI_CTRL::uc_RunTH[ChanNo] = 1;
            }
            else
            {
                Time = CommonFunc::GetTimeTicks( &MTCI_CTRL::gW_HumiInterval[ChanNo]); //g_SystemTime - gW_HumiInterval[ChanNo]; // 温度设置后是否在6秒内设置过湿度
                if(Time < 0)
                {
                    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_HumiInterval[ChanNo]);//MTCI_CTRL::gW_HumiInterval[ChanNo] = g_SystemTime;
                    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
                    return;
                }
                else if(Time < HUMI_INTERVALTIME)
                {
                    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
                    return;
                }
                MTCI_CTRL::uc_LockModleT[ChanNo] = 1;
                MTCI_CTRL::uc_RunTH[ChanNo] = 0; //  下一层用于判断发送温、湿命令
            }
        }
        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
        MTCI_CTRL::gW_T_Back[ChanNo] = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];   // 进行温、湿模式切换
    }

    if(MTCI_CTRL::uc_RunFrist[ChanNo] == 0)
    {
        if(++MTCI_CTRL::uc_RunCount[ChanNo] >= 6)
        {
            MTCI_CTRL::uc_RunFrist[ChanNo] = 1;
        }
        else
        {
            SendRunCmd(ChanNo,uc_Temp4Humi_Flag);  // 进行温、湿模式切换; //  下一层用于调用发送温、湿命令
        }
        return;
    }

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TH_S7_226CN::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_S7_226CN[MbChkSum];

    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+9);
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+13);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}


void TH_S7_226CN::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(MTCI_CTRL::uc_RunTH[ChanNo] == 0)    // 进行温、湿模式切换
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_RUN_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_RunCmd_S7_226CN_T[MbChkSum];
        MTCI_CTRL::uc_RunTH[ChanNo] = 0;
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_RUN_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_RunCmd_S7_226CN_TH[MbChkSum];
        MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}

void TH_S7_226CN::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_S7_226CN[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_S7_226CN[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_S7_226CN[MbChkSum];
#endif
    }


    if(++uc_ReadDI_Count >= 5)
    {
        uc_ReadDI_Count = 0;
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_READ_CMD; MbChkSum++) // 间隔两次后读一次DI，DI为上传报警信息
            MbBuf[MbChkSum] = uc_COM_ReadDICmd_S7_226CN[MbChkSum];
    }


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作


#if(DEBUG_TRACE_RECORD== 1)
    //uw32_TimeStamp_Test[uw16_Put] = uw32_TimeStamp;
    //uw16_Put =(uw16_Put+1)%100;
    uw32_Count[0]++;
#endif
}

void TH_S7_226CN::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    if(MTCI_CTRL::uc_RunTH[ChanNo])     // 温度值为10℃度保护
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_STOP_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_StopCmd_S7_226CN_TH[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_STOP_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_StopCmd_S7_226CN_T[MbChkSum];
    }

    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}



