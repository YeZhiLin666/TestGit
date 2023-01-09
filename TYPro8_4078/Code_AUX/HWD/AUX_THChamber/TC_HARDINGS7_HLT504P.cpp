/*****************************************************************************
// 程序文件      :TC_HARDINGS7_HLT504P.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2014.04.08
// 针对特定设备  :ARBIN控制重庆哈丁科技TPC7062K说明
*****************************************************************************/


#include "../../Entry/includes.h"



extern struct FUNC{
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);   
}st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];
 



//------------------------------------------------------------------------------------------------


//S7 SMART
// 高低温箱
// HD_modbus RTU_HUT_V1.3.doc.pdf

// 1-M)要求设置运行方式命令主机(MCT板)到从机（温控器） (以字节操作)
// 运行方式设定 0551 1word Unsigned 2=定值方式            (0x0227)
// 请求帧：01 06 02 27 00 02 B9 B8
// 返回帧：01 06 02 27 00 02 B9 B8
//

static const unsigned char uc_COM_ModeCmd_HLT504P[TC_HLT504P_MAX_COM_MODE_CMD] = {
0x01, 0x06, 0x02, 0x27, 0x00, 0x02, 0xB9, 0xB8};    // Mode Master to  Slave cmd 


// 2-M)要求设置运行命令主机(MCT板)到从机（温控器） (以字节操作)
// 写 1 号机数据设备启动    (0x0226)
// 请求帧：01 06 02 26 00 01 A8 79
// 返回帧：01 06 02 26 00 01 A8 79
//
static const unsigned char uc_COM_RunCmd_HLT504P[TC_HLT504P_MAX_COM_RUN_CMD] = {
0x01, 0x06, 0x02, 0x26, 0x00, 0x01, 0xA8, 0x79};    // Run Master to  Slave cmd 


// 3-M)要求设置停止命令主机主机(MCT板)到从机（温控器）   (以字节操作)
// 写 1 号机数据设备停止    (0x0226)
// 请求帧：01 06 02 26 00 02 E8 78
// 返回帧：01 06 02 26 00 02 E8 78
//
static const unsigned char uc_COM_StopCmd_HLT504P[TC_HLT504P_MAX_COM_STOP_CMD] = {
0x01, 0x06, 0x02, 0x26, 0x00, 0x02, 0xE8, 0x78};    // Stop Master to  Slave cmd  

//-------------------------------------------------------
// 4-M)要求设置温度/湿度命令主机(MCT板)到从机（温控器） (以字节操作)
// 写 1 号机定值方式下的数据 (22度，55%）
// 22度，55%请求帧：01 10 03 52 00 02 04 00 DC 02 26 22 FA
// 返回帧：01 10 03 52 00 02 E0 5D
//
// const unsigned char uc_COM_WriteCmd_HLT504P[TC_HLT504P_MAX_COM_WRITE_CMD] = {
// 0x01, 0x10, 0x03, 0x52, 0x00, 0x02, 0x04, 0x00, 0xDC, 0x02, 
// 0x26, 0x22, 0XFA};     // write Master to  Slave cmd  (上箱22度，下箱22度）


static const unsigned char uc_COM_WriteCmd_HLT504P[TC_HLT504P_MAX_COM_WRITE_CMD] = {
0x01, 0x10, 0x03, 0x52, 0x00, 0x01, 0x02, 0x00, 0xDC, 0x22, 0XFA};     // write Master to  Slave cmd  (上箱22度，下箱22度）


//  从机回复时，Reply设置值：
static const unsigned char uc_COM_WriteReply_HLT504P[TC_HLT504P_MAX_COM_WRITE_REPLY] = {
0x01, 0x10, 0x03, 0x52, 0x00, 0x02, 0xE0, 0x5D};     // write Slave to  Master reply (回复设置成功) 

// 5-M)要求读取温度/湿度命令主机(MCT板)到从机（温控器）  (以字节操作) 
// 读取 1 号机数据 (30度，30度，60%）
// 请求帧：01 03 00 02 00 06 C5 C8
// 响应帧：01 03 0C 01 2C 01 2C 02 58 00 00 00 00 00 00 3F 3B
//
static const unsigned char uc_COM_ReadCmd_HLT504P[TC_HLT504P_MAX_COM_READ_CMD] = {
0x01, 0x03, 0x00, 0x00, 0x00, 0x06, 0xC5, 0xC8};    // read Master to  Slave cmd (读取) 

//  从机回复时，Reply
static const unsigned char uc_COM_ReadReply_HLT504P[TC_HLT504P_MAX_COM_READ_REPLY] = {
0x01, 0x03, 0x0C, 0x01, 0x2C, 0x01, 0x2C, 0x02, 0x58, 0x00, 
0x00, 0x00, 0X00, 0X00, 0x00, 0x3F, 0x3B};     // read Slave to  Master reply  //(回复温度-10度)


//
//  ARBIN控制重庆哈丁科技S7 HLT504P说明  串口通信方式   //
//----------------------------------------------------------------------------------------------------





void TC_HLT504P::Init(unsigned char ChanNo)
{
// 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_HLT504P::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_HLT504P::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_HLT504P::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_HLT504P::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_HLT504P::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_HLT504P::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TC_HLT504P::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// 分析接收的结果数据
    unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    unsigned char uc_n = 0;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;


//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TC_HLT504P_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
//         #ifdef COM_CMD_CHANNEL_DIFFER
// 		if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_HUANCE_UMC1200[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_HUANCE_UMC1200[2])
// 		#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_HLT504P[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_HLT504P[2])
//         #endif
        {   // 利用包头或包尾判断数据值。

            unsigned int uw16_k = uw16_Total - uw16_i + 5 ;  // 从uw16_i开始到命令头部的字节数
            unsigned int uw16_m = uw16_Total - (uw16_i + 3);    // 从uw16_i+3开始到命令尾巴部的4字节数4

            pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+TC_HLT504P_READ_REPLY_POINT_TEMPVAlUE]; // [11 - 3]=8   // 去丢[STX (1 byte)][Addr (2 byte)]
            PresentValue = ((*pCmd)<<8) + (*(pCmd+1));
            //PresentValue = (short int) (atof(pCmd)*10);  // 放大10倍上传
            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

            pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+TC_HLT504P_READ_REPLY_POINT_HUMIVAlUE];   // [19 - 3]=16 // 去丢[STX (1 byte)][Addr (2 byte)]
            PresentValue = ((*pCmd)<<8) + (*(pCmd+1));
            //PresentValue = (short int) (atof(pCmd)*10);  // 放大10倍上传
            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间
            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TC_HLT504P::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送写控制值命令
    unsigned int  uw16_len = TC_HLT504P_MAX_COM_WRITE_CMD;
    unsigned int  MbChkSum;
    //unsigned short int  register_data;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_HLT504P[MbChkSum];

    //register_data = gW_TemperatureValue[ChanNo];
    MbBuf[TC_HLT504P_WRITE_CMD_POINT_TEMPVAlUE] =	(MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL]>>8)& 0xFF;
    MbBuf[TC_HLT504P_WRITE_CMD_POINT_TEMPVAlUE + 1] =	MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL] & 0xFF;

    //MbBuf[TC_HLT504P_WRITE_CMD_POINT_HUMIVAlUE] =	(MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]>>8) & 0xFF;
    //MbBuf[TC_HLT504P_WRITE_CMD_POINT_HUMIVAlUE + 1] =	MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL] & 0xFF;

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[0] = ChanNo + 1;
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_HLT504P_MAX_COM_WRITE_CMD-2);
    MbBuf[TC_HLT504P_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_HLT504P_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 0XFF);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TC_HLT504P::SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送模式设置命令
    unsigned int  uw16_len = TC_HLT504P_MAX_COM_MODE_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ModeCmd_HLT504P[MbChkSum];
    uw16_len = TC_HLT504P_MAX_COM_MODE_CMD;

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[0] = ChanNo + 1;
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TC_HLT504P::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送运行命令
    unsigned int  uw16_len = TC_HLT504P_MAX_COM_RUN_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_HLT504P[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[0] = ChanNo + 1;
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TC_HLT504P::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_HLT504P_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_HLT504P[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[0] = ChanNo + 1;
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TC_HLT504P::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_HLT504P_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_HLT504P[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[0] = ChanNo + 1;
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

