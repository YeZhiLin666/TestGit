/*****************************************************************************
// 程序文件      :TH_CHONGQING_ATAIHE.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  CHONGQING_ATAIHE(重庆阿泰可温箱UC8-60125-ES) Temperature Chamber //
//  步入式温湿度试验箱    西门子 PLC S7-200 SMART
//  Model:UC8-60125-ES
//  串口通信方式：ASCII模式 传输方式：ASCII模式 无校验
//  特别注意：运行和停止命令，跟读和写命令不一致。
// 有特定配置要求
// 1)#define  COM_INTERVAL_WRITE_AND_READ                 0         //是否使用串口间隔写和读功能 为0表示不使用间隔写和读功能
// 2)#define CMD_INTERVALTIME	     (500*10)   // 每次发送命令间隔时间 500毫秒

//  Start    '$'                     ASCII code 36  (0X24)
//  End     <CR> Carriage Return     ASCII code 13  (0X0D)

//  1-M)设置温度主机到从机 (以字符操作)
// 写入字信息格式：
// 命令格式：设备号+电脑号+命令字符+地址+a（分段符）+数据1+…+数据N +写入数量+A（结束符）

// 设备号：4位数字字符（0~9999），例：0001
// 电脑号：4位数字字符（0~9999），固定为0001
// 命令字符：
// 两位字母符号:
// 前字符为类别字符：固定为w
// 后字符为操作字符：
// 操作对象为字则a
// 操作对象为位则打开位为b，
// 关闭位为c


//  Send string:
//  00010001wa02106a000100550000A00010001wa02108a000100990000A
//  Response string:

const unsigned char uc_COM_WriteCmd_ATAIHE_T[TH_ATAIHE_MAX_COM_WRITE_CMD] =
    "00010001wa02106a000100220000A";     // write Master to  Slave cmd  (设置温度22度)
const unsigned char uc_COM_WriteCmd_ATAIHE_H[TH_ATAIHE_MAX_COM_WRITE_CMD] =
    "00010001wa02108a000100550000A";     // write Master to  Slave cmd  (设置湿度55%)

//  从机回复时，Reply一模一样设置值：
const unsigned char uc_COM_WriteReply_ATAIHE[TH_ATAIHE_MAX_COM_WRITE_REPLY] =
    "00010001wa02106a000100220000A00010001wa02108a000100550000A";
// write Slave to  Master reply (回复设置温度22.0度)

//  2-M)读取温度主机到从机  (以字符操作)
//  Send string (PC → controller)
// 读取命令：
// 发送命令格式：
// 设备号+电脑号+命令字符+地址+a（分段符）+读取数量+A（结束符）
// 设备号：4位数字字符（0~9999），例：0001
// 电脑号：4位数字字符（0~9999）备用，目前固定为0001

//  Send string:
//  00010001ra02100a0002A
//  Response string:
//  00010001ra02100a0002     299     600A
//  30 30 30 31 30 30 30 31 72 61 30 32 31 30 30 61 30 30 30 32 20 20 20 20 20 32 39 39 20 20 20 20 20 36 30 30 41

const unsigned char uc_COM_ReadCmd_ATAIHE[TH_ATAIHE_MAX_COM_READ_CMD] =
    "00010001ra02100a0002A";    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_ATAIHE[TH_ATAIHE_MAX_COM_READ_REPLY] =
    "00010001ra02100a0002     220     550A";
// read Slave to  Master reply  //(回复温度22度)

//  3-M)要求运行命令主机到从机
//  Send string:
//  00010001wb09540000A
//  Response string:
//  00010001wb09540000A

const unsigned char uc_COM_RunCmd_ATAIHE[TH_ATAIHE_MAX_COM_RUN_CMD] =
    "00010001wb09540000A";

//  4-M)要求停止命令主机到从机命令
//  Send string:
//  00010001wc09540000A
//  Response string:

const unsigned char uc_COM_StopCmd_ATAIHE[TH_ATAIHE_MAX_COM_STOP_CMD] =
    "00010001wc09540000A";

//
//  CHONGQING_ATAIHE(重庆阿泰可温箱UC8-60125-ES) Temperature Chamber //
//  步入式温湿度试验箱
//  Model:UC8-60125-ES
//----------------------------------------------------------------------------------------------------

void TH_ATAIHE::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_ATAIHE::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_ATAIHE::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_ATAIHE::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_ATAIHE::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_ATAIHE::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_ATAIHE::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TH_ATAIHE::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    unsigned short int uw16_k,uw16_n,uw16_m;
    unsigned char uc_Buffer[2][8];
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_ATAIHE_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < uw16_Total; uw16_i ++)   //   解决快速更新数据问题
    {   //  控制在正190度负55度。  // Temperature range -55°C to +190°C //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0X41)  // 寻找End ‘A’，过滤多余的位数。
        {
            if((uw16_Total - uw16_i) < 37)
                return;  // 不足一包，不用再检查。

            if(MTCI_CTRL::gR_CmdBuf[uw16_i + 37] != 0X41)  // 寻找End ‘A’，过滤多余的位数。
                continue; // 两包保护处理,避免中间有其他的回复信息加入。加了就丢去   解决写DIDO值错误问题

            if((MTCI_CTRL::gR_CmdBuf[uw16_i + 9] != 0X72)&&(MTCI_CTRL::gR_CmdBuf[uw16_i + 10] != 0X61))  // 寻找'ra'
                continue;

            uc_MoveFlag = 2;
            uw16_j = uw16_i + 8;  //gR_CmdBuf[uw16_i+8];
            uw16_k = uw16_i;
            break;
            //}
        }
    }

    if((uc_MoveFlag < 2)||(uw16_i >= uw16_Total))
        return;   // 找不到间隔，自动退出,或为发送命令。	 //

    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 37] != 0X41)  // 寻找End ‘A’，过滤多余的位数。
        return; // 两包保护处理,避免中间有其他的回复信息加入。

    for(uw16_i = 0; uw16_i < 8; uw16_i ++)
    {
        uc_Buffer[0][uw16_i] = 0x20;
        uc_Buffer[1][uw16_i] = 0x20;
    }
    for(uw16_i = 0; uw16_i < 2; uw16_i ++)
    {
        switch(uw16_i)
        {
        case 0:
            uw16_j = uw16_k + TH_ATAIHE_READ_REPLY_POINT_VAlUE_T;     // Actual value of parameter 1 温度实际值
            break;                   // 1+7=8     Actual value of parameter 1  Get
        case 1:
            uw16_j = uw16_k + TH_ATAIHE_READ_REPLY_POINT_VAlUE_H;    // Nominal value of parameter 2  湿度实际值
            break;                   // 8+7=15    Nominal value of parameter 2 Get
        default:
            uw16_j =uw16_j;
            break;
        }
        pCmd = MTCI_CTRL::String_MoveBuff(&MTCI_CTRL::gR_CmdBuf[uw16_j],&uc_Buffer[uw16_i][0],4); //DATA FROM CHAMBER
        p = pCmd; //DATA FROM CHAMBER

        if( (*pCmd) == 0 )    //  为空退出。
            continue;

        if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
        {
            //p=ctrl_String2Value(pCmd, &PresentValue);
            //PresentValue = (short int) (atof(pCmd)*10);  // 放大10倍上传

            //if( p == NULL )
            //	continue;

            switch(uw16_i)
            {
            case 0:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]);
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] /=10;
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1 温度实际值
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                break;

            case 1:
                for(uw16_n = 0; uw16_n < 8; uw16_n ++)
                {
                    if(uc_Buffer[1][uw16_n] == 0x41) // "A"
                    {
                        uc_Buffer[1][uw16_n] = 0x20;
                        for(uw16_m = uw16_n; uw16_m < 8; uw16_m ++)
                            uc_Buffer[1][uw16_m] = 0x20;
                    }
                }
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]); // Actual value of parameter 2  湿度实际值
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL] /=10;
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
                break;

            default:
                break;
            }
        }
    }

}


void TH_ATAIHE::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_WRITE_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

// 	if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
// 		return;

    MbBuf[uw16_len-1] = 'A';  // 命令结束标志
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteCmd_ATAIHE_H[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_ATAIHE_WRITE_CMD_POINT_VAlUE_H);  // Nominal value of parameter 2
        //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // 缩小10倍处理
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteCmd_ATAIHE_T[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_ATAIHE_WRITE_CMD_POINT_VAlUE_T);  // Nominal value of parameter 1
        //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE);    // 缩小10倍处理
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        if(MbBuf[MbChkSum] == 0x2E)  // '.'
        {
            //MbBuf[MbChkSum] = 0x30;  // '0'
            MbBuf[MbChkSum] = MbBuf[MbChkSum+1]; // 小数后值平移一位
            MbBuf[MbChkSum+1] = 0x30;  // '0'
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}


void TH_ATAIHE::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_RUN_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // 命令结束标志

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}

void TH_ATAIHE::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_READ_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // 命令结束标志

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_ATAIHE::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_STOP_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // 命令结束标志

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}




