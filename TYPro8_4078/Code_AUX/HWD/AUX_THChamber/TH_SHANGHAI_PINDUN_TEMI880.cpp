/*****************************************************************************
// 程序文件      :TH_SHANGHAI_PINDUN_TEMI880.C    定义温控箱中各种处理文件
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

// 1-M)设置温度主机到从机 (以字节操作)
// {
// 修改定值温度设定值
// 格式：E01#600#478#修改值#A
// 修改定值湿度设定值
// 格式：E01#600#479#修改值#A
// }
// Transmission : E01#600#478#22.00#A
// 45 30 31 23 36 30 30 23 34 37 38 23 32 32 2E 30 30 23 41

// 45 30 31 23 36 30 30 23 34 37 38 23 XX XX XX XX XX 23 41
// (XXXXXXXX温度值先传高位再传低位)22.00

const unsigned char uc_COM_WriteTempCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
    0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
    0x38, 0x23, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41
};     // write Master to  Slave cmd  (设置温度22度，就是22.00)  // 采用浮点数方式


// Transmission : E01#600#479#55.00#A
// 45 30 31 23 36 30 30 23 34 37 39 23 35 35 2E 30 30 23 41

// 45 30 31 23 36 30 30 23 34 37 39 23 YY YY YY YY YY 23 41
// (YYYYYYYYY湿度值先传高位再传低位)55.00

const unsigned char uc_COM_WriteHumiCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
    0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
    0x39, 0x23, 0x35, 0x35, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41
};     // write Master to  Slave cmd  (设置湿度55%，就是55.00)  // 采用浮点数方式


// const unsigned char uc_COM_WriteReply_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
// 0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
// 0x38, 0x23, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41};     // write Slave to  Master reply

//  2-M)读取温度主机到从机  (以字节操作)
// {
// 查询仪表的运行状态：
// S0199A 或者S01#99#A
// 注：[01]是仪表的地址 [99]是查询指令
// }
// Transmission : S0199A
// 53 30 31 39 39 41

// Transmission : S01#99#A
// 53 30 31 23 39 39 23 41

const unsigned char uc_COM_ReadTempCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD] = {
    0x53, 0x30, 0x31, 0x39, 0x39, 0x41
};    // read Master to  Slave cmd (读取温度)


// 从机回复时，
// Reply CMD:  {
// 仪表返回	[地址(1B)][ID(1B)][运行状态(1B)][当前温度(4B)][当前温度设定值(4B)] [当前湿度(4B)][当前湿度设定值(4B)][校验码1 (1B)] [校验码2 (1B)]
// 报文举例	以下是仪表返回的数据，以16进制显示
// 01 63 02 1B 18 00 00 7C 15 00 00 E8 1E 00 00 88 13 00 00 CB CB
// 报文解析
// 01 是仪表的地址
// 63 是ID，标识这段数据包是何种格式
// 02 运行状态 0:程式停止1:程式运行2:定值停止3:定值运行

// 1B 18 00 00 是当前温度值,

// 还原算法如下，以下的数是16进制
// 温度值=(00 * 1000000 + 00*10000 +18 * 100 + 1B)/64
// =(1800+1B)/64
//  (181B/64=3D  =61) (181B%64=47  =71)
// 结果是:61.71
// (181B=6171)
// 例如：当温度为负数时,根据数制原理求补数的方法
// 最高位为1表示为负数 （60 F0 FF FF）
// <1>原数：60 F0 FF FF    (实为：-4000)
// <2>取反: 9F 0F 00 00
// <3>求和: (00 * 1000000 + 00*10000 +0F * 100 + 9F)=F9F
// <4>加1：(F9F=3999)  3999 + 1 = 4000
// <5>取反-4000
// <6>转化成单精度:-4000/100.0  = -40.0

// 7C 15 00 00 是当前温度设定值,

// 还原算法如下，以下的数是16进制
// 温度设定值=(00 * 1000000 + 00*10000 +15 * 100 + 7C)/64
// 结果是:55.00

// E8 1E 00 00 是当前湿度,

// 还原算法如下，以下的数是16进制
// 湿度=(00 * 1000000 + 00*10000 +1E * 100 + E8)/64
// 1EE8=7912   7912/100=79.12
// 结果是:79.12
// 结果是:78.22

// 88 13 00 00 是当前湿度设定值,

// 还原算法如下，以下的数是16进制
// 湿度设定值=(00 * 1000000 + 00*10000 +13 * 100 + 88)/64
// 结果是:50.00

// 【如果是单温仪表，湿度的设定和显示值也会发过来，不用就行了】
// CB CB     是本段数据的校验码，两个值相同，建议使用倒数第二个。
// }

// Response : 01 63 02 98 08 00 00 98 08 00 00 7C 15 00 00 7C 15 00 00 C8 C8
//
// 01 63 02 XX XX XX XX 98 08 00 00 YY YY YY YY 7C 15 00 00 C8 C8
// (XXXXXXXX温度值先传低位再传高位; YYYYYYYYY湿度值先传低位再传高位)
// 使用full完整方式时，显示值=温度值/100;    如：22(2200/100=898/A)度,55(5500/100=157C/A)%湿度Humi.
//  [正常码字方式: (45度*100)  4500 = 0X1194; (-60度*100) -6000 = 0XFFFFE890 (0x100000000-0xFFFFE890=0X1770 = 6000)]

const unsigned char uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY] = {
    0x01, 0x63, 0x02, 0x98, 0x08, 0x30, 0x30, 0x98, 0x08, 0x30,
    0x30, 0x7C, 0x15, 0x30, 0x30, 0x7C, 0x15, 0x30, 0x30, 0xC8,
    0xC8
};   // read Slave to  Master reply  //(回复温度22度，就是2200;湿度55%,就是5500)

//  3-M)要求运行命令主机到从机 (以字节操作)
// {
// 启动运行：
// 格式：E01#99#1#A
// 说明：[01]是仪表的地址 [99]是查询指令  [1]表示启动运行
// }
// Transmission : E01#99#1#A
// 45 30 31 23 39 39 23 31 23 41

const unsigned char uc_COM_RunCmd_SHANGHAI_PINDUN_TEMI880_T[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD] =  {
    0x45, 0x30, 0x31, 0x23, 0x39, 0x39, 0x23, 0x31, 0x23, 0x41
};    // Run Master to  Slave cmd


//  4-M)要求停止命令主机到从机   (以字节操作)
// {
// 停止运行：
// 格式：E01#99#2#A
// 说明：[01]是仪表的地址 [99]是查询指令  [2]表示停止运行
// }
// Transmission : E01#99#2#A
// 45 30 31 23 39 39 23 32 23 41

const unsigned char uc_COM_StopCmd_SHANGHAI_PINDUN_TEMI880_T[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD] =  {
    0x45, 0x30, 0x31, 0x23, 0x39, 0x39, 0x23, 0x32, 0x23, 0x41
};     // Stop Master to  Slave cmd


//
// TH_SHANGHAI_PINDUN_TEMI880  串口通信方式  ASCII  传输方式：校验 //
//----------------------------------------------------------------------------------------------------


void TH_SHANGHAI_PINDUN_TEMI880::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
}

/*----*/
unsigned char TH_SHANGHAI_PINDUN_TEMI880::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_SHANGHAI_PINDUN_TEMI880::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    union  Int_data I_Value;
    short int temp = 0;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+SHANGHAI_PINDUN_TEMI880_ASCII_FIRST)
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+19] == MTCI_CTRL::gR_CmdBuf[uw16_i+20])
#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[0]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+19] == MTCI_CTRL::gR_CmdBuf[uw16_i+20])  // 读取温度、读取湿度
#endif
        {   // 利用包头或包尾判断数据值。
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM)
// 			   return;	// 避免温度或湿度交替接收数据时,更新错误值。

            I_Value.uc_d[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+6];
            I_Value.uc_d[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
            I_Value.uc_d[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
            I_Value.uc_d[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
            if(I_Value.ul_member > 0X80000000)  // 负数特别处理
                PresentValue = (short int) (-1*(0X100000000 - I_Value.ul_member)/10);
            else
                PresentValue = (short int) (I_Value.ul_member/10);

            if( TH_SHANGHAI_PINDUN_TEMI880::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

                I_Value.uc_d[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+14];
                I_Value.uc_d[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+13];
                I_Value.uc_d[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+12];
                I_Value.uc_d[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+11];
                if(I_Value.ul_member > 0X80000000)  // 负数特别处理
                    PresentValue = (short int) (-1*(0X100000000 - I_Value.ul_member)/10);
                else
                    PresentValue = (short int) (I_Value.ul_member/10);
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

//            if((MTCI_CTRL::gC_IsRunning[ChanNo])&&(MTCI_CTRL::uc_LockModleT[ChanNo]==0))   // 正式控制设置控制值，未锁定温度模式
//            {
//                temp = abs(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue-MTCI_CTRL::gR_PV_Back[ChanNo]);  //  死区范围-+0.5度 10.5~9.5
//                if(temp < 1)   // 不变化则认为不工作。
//                {
//                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
//                    return;
//                }
//
//                if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue <= (100+5))     // 温度值为10.5℃度保护
//                    Modle = 0; //  下一层用于判断发送温、湿命令
//                else if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue >= (90+5))
//                    Modle = 1;
//
//                if(++MTCI_CTRL::uc_ReadCount[ChanNo] >= 10)
//                {
//                    MTCI_CTRL::gR_PV_Back[ChanNo] = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue;
//                    if(MTCI_CTRL::uc_RunTH[ChanNo] != Modle)
//                    {
//                        MTCI_CTRL::uc_RunTH[ChanNo] = Modle;
//                        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;   // 进行温、湿模式切换
//                        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
//                    }
//                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
//                }
//            }

            return;
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_SHANGHAI_PINDUN_TEMI880::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制命令,主要入口，里面带写值，写运行分支
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TH_SHANGHAI_PINDUN_TEMI880::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // 缩小10倍处理
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 12);
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // 缩小10倍处理
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 12);
    }


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // 发送定长处理

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}


void TH_SHANGHAI_PINDUN_TEMI880::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_SHANGHAI_PINDUN_TEMI880_T[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}

void TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
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

void TH_SHANGHAI_PINDUN_TEMI880::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_SHANGHAI_PINDUN_TEMI880_T[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// 温、湿控制为不同指令时，交替标志以进入温、湿分别发送的指令系统运作
}



