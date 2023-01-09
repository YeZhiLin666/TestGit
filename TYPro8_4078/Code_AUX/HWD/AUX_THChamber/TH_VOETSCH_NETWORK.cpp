/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.c    定义温控箱中各种处理文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  Voetsch(德国WEISS-VOETSCH环境仪器设备有限公司) Temperature Chamber //
//  MCU2468AUX  4RS232板 PN：502314		ASCII模式
//  串口通信方式：ASCII模式 传输方式：ASCII-2模式 无校验
//  特别注意：运行和停止命令，跟读和写命令一致。


//  Start    '$'                     ASCII code 36  (0X24)
//  End     <CR> Carriage Return     ASCII code 13  (0X0D)

//  1-M)设置温度主机到从机 (以字符操作)
//  Send string (PC → controller)
//  Set nominal values for parameters
//  The following string sets the nominal value for parameter 1 to 25 and the nominal value for
//  parameter 2 to 50 and starts operation.
//  The string reads as follows:
//  $01E 0023.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010 <CR>

//  $01E
//  0023.0 Nominal value of parameter 1
//  0050.0 Nominal value of parameter 2
//  011
//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
//  digital output 2
//  01010101010101010101010101010 Other digital outputs
//  <CR>

//  Send string:
//  $01E 0023.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_WriteCmd_VOETSCH_NETWORK[TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD] =
    "$01E 0022.0 0055.0 01111010101010101";     // write Master to  Slave cmd  (设置温度22度)


//  2-M)读取温度主机到从机  (以字符操作)
//  Send string (PC → controller)
//  The string reads as follows: $01I<CR>
//  $
//  01 Bus address of the unit
//  I Request to send the actual status
//  <CR>

//  Response string (Controller → PC) for a unit with 2 parameters
//  The following string contains nominal and actual values for temperature and humidity2)
//  together with various other parameters.
//  The values are instantaneous values:
//  The string reads as follows:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

//  0023.0 Nominal value of parameter 1
//  0020.5 Actual value of parameter 1
//  0050.0 Nominal value of parameter 2
//  0041.0 Actual value of parameter 2
//  0080.0 Set value 1
//  0080.0 Set value 1
//  0000.0 Not used
//  0020.0 Actual value Pt100-1 (°C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.2 Actual value Pt100-2 (°C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.3 Actual value Pt100-3 (°C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.4 Actual value Pt100-4 (°C, analogue I/O card) 1)
//  011
//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
//  digital output 2
//  01010101010101010101010101010 Other digital outputs
//  <CR>

//  Send string:
//  $01I<CR>
//  Response string:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

const unsigned char uc_COM_ReadCmd_VOETSCH_NETWORK[TH_VOETSCH_NETWORK_MAX_COM_READ_CMD] = "$01I";    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_VOETSCH_NETWORK[TH_VOETSCH_NETWORK_MAX_COM_READ_REPLY] =
    "-005.0 -004.2 0030.0 0000.0 0000.0 01000000000000000000000000000000";
// read Slave to  Master reply  //(回复温度22度)

//  3-M)要求运行命令主机到从机
//  (读取温度)
//  Send string:
//  $01I<CR>
//  Response string:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

const unsigned char uc_COM_RunCmd_VOETSCH_NETWORK[TH_VOETSCH_NETWORK_MAX_COM_RUN_CMD] = 
		"$01E 0022.0 0055.0 01111010101010101";

//  4-M)要求停止命令主机到从机命令
//  (设置温度22度，就是220)
//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
//  Send string:
//  $01E 0022.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 00101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_StopCmd_VOETSCH_NETWORK[TH_VOETSCH_NETWORK_MAX_COM_STOP_CMD] =
    "$01E 0022.0 0055.0 00111010101010101";    // write Master to  Slave cmd  (设置温度22度)


//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
// 停与写不一致。

//
//  Voetsch(德国WEISS-VOETSCH环境仪器设备有限公司) Temperature Chamber //
//----------------------------------------------------------------------------------------------------

void TH_VOETSCH_NETWORK::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_VOETSCH_NETWORK::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_VOETSCH_NETWORK::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_VOETSCH_NETWORK::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_VOETSCH_NETWORK::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_VOETSCH_NETWORK::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_VOETSCH_NETWORK::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TH_VOETSCH_NETWORK::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    // 分析接收的结果数据
    unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned char uc_Buffer[4][6];
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;

    for(uw16_i = 0; uw16_i < 5; uw16_i ++)
    {
        switch(uw16_i)
        {
        case 0:
            uw16_j = uw16_k + 7;     // Actual value of parameter 1 温度实际值
            break;                   // 1+7=8     Actual value of parameter 1  Get
        case 1:
            uw16_j = uw16_k + 14;    // Nominal value of parameter 2  湿度设定值
            break;                   // 8+7=15    Nominal value of parameter 2 Set
        case 2:
            uw16_j = uw16_k + 21;    // Actual value of parameter 2  湿度实际值
            break;                   // 22+7=29   Set value 1
        case 4:
            uw16_j = uw16_k + 35;
            break;
				
        default:
            uw16_j =uw16_j;
            break;
        }
        pCmd = MTCI_CTRL::String_MoveBuff(&MTCI_CTRL::gR_CmdBuf[uw16_j],&uc_Buffer[uw16_i][0],TH_VOETSCH_NETWORK_READ_REPLY_POINT_VAlUE); //DATA FROM CHAMBER
        p = pCmd; //DATA FROM CHAMBER

        if( (*pCmd) == 0 )    //  为空退出。
            continue;

        if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
        {
            switch(uw16_i)
            {
            case 0:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]);
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1 温度实际值
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                break;

            case 2:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]); // Actual value of parameter 2  湿度实际值
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI,FIRST_CHAMBER_CHANNEL);
                break;

            case 4:
                MTCI_CTRL::gR_Updata_Flag[ChanNo] = TRUE;
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::gR_PresentValue_Set1[ChanNo]); //    Set value 1
                PresentValue = MTCI_CTRL::gR_PresentValue_Set1[ChanNo];


                uw32_DO = 0;
                for(uw16_j = 0; uw16_j < 30; uw16_j ++)   //  两帧取前一帧的DIDO
                {
                    //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
                    //  1 :digital output 1   // 它的值1表示运行;0表示停止。
                    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 66 - uw16_j] == '1') //Unused digital output 0，1，2，Other digital outputs
                    {   //Unused digital output 0，1，2，Other digital outputs
                        uw32_DO |= (1<<uw16_j);  // 置1     // 取后一包
                    }
                    else
                    {
                        uw32_DO &= ~(1<<uw16_j);  // 置0
                    }
                }
                //gR_PV_DO32[ChanNo] = uw32_DO; // 取出DIDO通道信息值。

                if(uw32_DO != MTCI_CTRL::gR_PV_DO32[ChanNo])
                    MTCI_CTRL::gR_PV_DO32[ChanNo] = uw32_DO; // 取出DIDO通道信息值。

                MTCI_CTRL::gR_PresentValue_DI32[0] = MTCI_CTRL::gR_PV_DO32[ChanNo];


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
                        DI_D[0] = (DI_32Value>>12)&0X0F;
                        DI_D[1] = (DI_32Value>>8)&0X0F;
                        DI_D[2] = (DI_32Value>>4)&0X0F;
                        DI_D[3] = (DI_32Value>>0)&0X0F;
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

                //}
                //else
                //  gR_Updata_Counter[ChanNo]++ ;
                break;

            default:
                break;
            }
        }
    }
}


void TH_VOETSCH_NETWORK::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送写控制值命令
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_VOETSCH_NETWORK[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_NETWORK_WRITE_CMD_POINT_VAlUE + 1);  // Nominal value of parameter 1

    register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_NETWORK_WRITE_CMD_POINT_VAlUE + 8);  // Nominal value of parameter 2

    for(MbChkSum = 0; MbChkSum < 15; MbChkSum++)
    {
        //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
        //  1 :digital output 1   // 它的值1表示运行;0表示停止。
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>(MbChkSum+15)) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0，1，2，Other digital outputs
        {
            MbBuf[TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // 置1
        }
        else
        {
            MbBuf[TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // 置0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}


void TH_VOETSCH_NETWORK::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_NETWORK_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_VOETSCH_NETWORK[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_VOETSCH_NETWORK::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_NETWORK_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_VOETSCH_NETWORK[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_VOETSCH_NETWORK::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_NETWORK_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_VOETSCH_NETWORK[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+TH_VOETSCH_NETWORK_WRITE_CMD_POINT_VAlUE+8);  // Nominal value of parameter 2
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gR_PresentValue_Set1[ChanNo],MbBuf+TH_VOETSCH_NETWORK_WRITE_CMD_POINT_VAlUE+15);  // Set value 1

    for(MbChkSum = 0; MbChkSum < 15; MbChkSum++)
    {
        //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
        //  1 :digital output 1   // 它的值1表示运行;0表示停止。
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>(MbChkSum+15)) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0，1，2，Other digital outputs
        {
            MbBuf[TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // 置1
        }
        else
        {
            MbBuf[TH_VOETSCH_NETWORK_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // 置0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}




