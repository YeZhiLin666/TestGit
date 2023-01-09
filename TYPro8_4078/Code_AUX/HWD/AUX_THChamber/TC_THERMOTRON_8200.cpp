/*****************************************************************************
// 程序文件      :TC_THERMOTRON_8200.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
//  Thermotron&Industries(美国热测设备工业公司)8200 controller //
//  串口通信方式  MODBUS  传输方式：ASCII模式 无校验
//  特别注意：当使用停止和运行命令，跟读和写命令不一致时，机制必须修改。

//  Multidrop address communication protocol (RS-485)
//  The host computer sends:
//  [FF]#aCMND<t> or [FF]#aCMND<data><t>
//  The 8200 responds with:
//  [FF]#a<data><t>
//  Where:
//  [FF] = prefix character (FF hexadecimal)
//  # = ASCII pound sign (35 decimal  0X23)
//  a = One-byte address of the 8200 (1...127)
//  CMND = command mnemonic
//  <data> = data or character string sent with command mnemonic
//  <t> = computer terminator, for example a carriage return <cr>

//  1-M)设置温度主机到从机 (以字节操作)
//  Syntax: SETPn? or SETPn,data
//  Operation example: SETP1,22.000
//  8200 response: 22.000      (22设置值)

//  ASCII转16进制命令  ：53 45 54 50 31 2C 32 32 2E 30 30 30 0D
//  ASCII转16进制回复  ：32 32 2E 30 30 30 0D

//  Query example: SETP1?
//  8200 response: 22.000      (22设置值)

//   ASCII转16进制命令 ：53 45 54 50 31 3F 0D
//   ASCII转16进制回复 ：32 32 2E 30 30 30 0D
const unsigned char uc_COM_WriteCmd_8200[TC_8200_MAX_COM_WRITE_CMD] = {
    0x53, 0x45, 0x54, 0x50, 0x31, 0x2C, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // write Master to  Slave cmd  (设置温度22度)
//0x53, 0x45, 0x54, 0x50, 0x31, 0x2C, 0x32, 0x32, 0x2E, 0x30};     // write Master to  Slave cmd  (设置温度22.0度)
//  从机回复时，Reply一模一样设置值：
const unsigned char uc_COM_WriteReply_8200[TC_8200_MAX_COM_WRITE_REPLY] = {
    0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // write Slave to  Master reply (回复设置温度22.0度)

//  2-M)读取温度主机到从机  (以字Word操作)
//  Syntax: PVARn?
//  Where n is the channel number (1 to 48). See Description.
//  1 through 4 External process variable channels 1 through 4
//  5 through 8 Internal process variable channels 5 through 8
//  9 through 12 Undefined
//  13 through 28 Monitor channels 1 through 16
//  29 through 32 Undefined
//  33 through 36 System Monitor temperature channels for refrigeration system 1
//  (high-stage suction, high-stage discharge, low-stage suction,
//  and low-stage discharge)
//  37 through 48 System Monitor channels for refrigeration systems 2, 3, and 4

//  Query example: PVAR1?
//  8200 response: 22.000

//  ASCII转16进制命令 ：50 56 41 52 31 3F 0D
//  ASCII转16进制回复 ：32 32 2E 30 30 30 0D

const unsigned char uc_COM_ReadCmd_8200[TC_8200_MAX_COM_READ_CMD] = {
    0x50, 0x56, 0x41, 0x52, 0x31, 0x3F, 0X0D
};    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_8200[TC_8200_MAX_COM_READ_REPLY] = {
    0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // read Slave to  Master reply  //(回复温度22度，就是220)

//  3-M)要求运行命令主机到从机
//  Syntax: RUNM
//  8200 response: Places a stopped 8200 in run manual mode.

//  ASCII转16进制命令 ：52 55 4E 4D 0D
//  ASCII转16进制回复 ：

const unsigned char uc_COM_RunCmd_8200[TC_8200_MAX_COM_RUN_CMD] = {
    0x52, 0x55, 0x4E, 0x4D, 0X0D
};    // read Master to  Slave cmd

//  4-M)要求停止命令主机到从机命令
//  Syntax: STOP
//  8200 response: The 8200 goes into stop mode.

//  ASCII转16进制命令 ：53 54 4F 50 0D
//  ASCII转16进制回复 ：

const unsigned char uc_COM_StopCmd_8200[TC_8200_MAX_COM_STOP_CMD] = {
    0x53, 0x54, 0x4F, 0x50, 0X0D
};    // read Master to  Slave cmd


//
//  EZT570I  串口通信方式  MODBUS  传输方式：RTU模式 CRC校验 //
//----------------------------------------------------------------------------------------------------

void TC_8200::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_8200::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_8200::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_8200::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_8200::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_8200::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_8200::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_8200::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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


    if( UARTFunc::uart_bytenum(ChanNo) < TC_8200_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    pCmd = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < 2*TC_8200_MAX_COM_READ_REPLY; uw16_i ++)
    {   //  控制在正400度负200度。  // Temperature range -200°C to +400°C (-328°F to +725°F) //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        if((MTCI_CTRL::gR_CmdBuf[uw16_i]) == '.')  // 寻找小数点，过滤多余的位数。['CR'为0X0D]
        {
            if(uw16_i < 1) // 保护一开始就找到为小数点的情况,即数据不完整 如[.7'CR'5.7'CR'5.7'CR']
                continue;

            if(uw16_j == 0)    // 如： [-22.2'CR'-22.2'CR'-22.2'CR']   (j=3,k=9;整数-22: k-j-3=3，减3作用是为了去丢第一的有效小数和第一的换行标志'CR'，以及第二的小数点)
            {
                uw16_j = uw16_i;  //  第一次查到小数点。   利用两值的小数点判断。
                uc_MoveFlag = 1;
            }
            else
            {
                uw16_k = uw16_i;  //  第二次查到小数点。
                uc_MoveFlag = 2;
                break;
            }
        }
    }

    if((uc_MoveFlag < 2)||(uw16_k == (2*TC_8200_MAX_COM_READ_REPLY-1))||(uw16_i == 2*TC_8200_MAX_COM_READ_REPLY))
        return;   // 找不到间隔或无小数位，自动退出。

    uw16_k = uw16_k - uw16_j - 3;   // 保存第二次值算出的整数长度  // 如： [-22.2'CR'-22.2'CR'-22.2'CR']   (j=3,k=9;整数-22: 9-3-3=3，减3作用是为了去丢第一的有效小数和第一的换行标志'CR'，以及第二的小数点)

    for(uw16_i = 0; uw16_i < TC_8200_MAX_COM_READ_REPLY; uw16_i ++)
    {   //  控制在正400度负200度。  // Temperature range -200°C to +400°C (-328°F to +725°F) //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        MTCI_CTRL::gR_CmdBuf[uw16_i] =  MTCI_CTRL::gR_CmdBuf[uw16_i + uw16_j + 3];  // 利用第二次找到的数作为处理对象。跳过第一次值的小数位和换行标志'CR'(0x0D)平移数据。指向间隔了两数的地方。
        if(uw16_i >= uw16_k + 2) // 加2的作用为了保留整数长度后小数点和一位有效数
            MTCI_CTRL::gR_CmdBuf[uw16_i] = '0'; // 小数点后一位有效，其它填写零补充。
    }

    pCmd = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER
    p = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER

    if( (*pCmd) == 0 )    //  为空退出。
        return;

    if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
    {
        p=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);
        //PresentValue = (short int) (atof(pCmd)*10);  // 放大10倍上传

        if( p == NULL )
            return;

        MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
    }
    else
    {
        return;
    }

}

void TC_8200::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写命令
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_8200[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];

    MbChkSum = MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + TC_8200_WRITE_CMD_POINT_VAlUE);
    if(MbChkSum > (TC_8200_MAX_COM_WRITE_CMD - TC_8200_WRITE_CMD_POINT_VAlUE - 1))
        MbChkSum = (TC_8200_MAX_COM_WRITE_CMD - TC_8200_WRITE_CMD_POINT_VAlUE - 1);  // 数据长度最多6字节

    //ctrl_Value2String(register_data,MbBuf+3+6);
    //gcvt((double) (register_data/10), 6, MbBuf+3+6);    // 缩小10倍处理

    MbBuf[TC_8200_WRITE_CMD_POINT_VAlUE + MbChkSum] = uc_COM_WriteCmd_8200[TC_8200_MAX_COM_WRITE_CMD - 1]; // 填写0X0D结尾
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_WRITE_CMD_POINT_VAlUE + MbChkSum + 1);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_8200::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_8200[MbChkSum];
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // 实际上跟发送读命令一样是读取温度命令
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_RUN_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TC_8200::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_8200[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作

}

void TC_8200::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_8200[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_STOP_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 单温控制必须同时置标志以进入温、湿分别发送的指令系统运作
}


