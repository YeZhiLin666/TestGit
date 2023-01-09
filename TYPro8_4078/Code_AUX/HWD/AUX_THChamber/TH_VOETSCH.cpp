/*****************************************************************************
// 程序文件      :TH_VOETSCH.c    定义温控箱中各种处理文件
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
//  0080.0 Set value 1
//  0000.0 Not used
//  0000.0 Not used
//  0000.0 Not used
//  0000.0 Not used
//  011
//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
//  digital output 2
//  01010101010101010101010101010 Other digital outputs
//  <CR>

//  Send string:
//  $01E 0023.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_WriteCmd_VOETSCH[TH_VOETSCH_MAX_COM_WRITE_CMD] =
    "$01E 0022.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010";     // write Master to  Slave cmd  (设置温度22度)

//  从机回复时，Reply一模一样设置值：
const unsigned char uc_COM_WriteReply_VOETSCH[TH_VOETSCH_MAX_COM_WRITE_REPLY] =
    "-020.0 0022.0 0050.0 0020.0 0100.0 1200.0 0000.0 00000000000000000000000000000000";
// write Slave to  Master reply (回复设置温度22.0度)

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

const unsigned char uc_COM_ReadCmd_VOETSCH[TH_VOETSCH_MAX_COM_READ_CMD] = "$01I";    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
const unsigned char uc_COM_ReadReply_VOETSCH[TH_VOETSCH_MAX_COM_READ_REPLY] =
    "0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0 0020.3 0000.0 0020.4 01101010101010101010101010101010";
// read Slave to  Master reply  //(回复温度22度)

//  3-M)要求运行命令主机到从机
//  (读取温度)
//  Send string:
//  $01I<CR>
//  Response string:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

const unsigned char uc_COM_RunCmd_VOETSCH[TH_VOETSCH_MAX_COM_RUN_CMD] = "$01I";    // read Master to  Slave cmd (读取)

//  4-M)要求停止命令主机到从机命令
//  (设置温度22度，就是220)
//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
//  Send string:
//  $01E 0022.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 00101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_StopCmd_VOETSCH[TH_VOETSCH_MAX_COM_STOP_CMD] =
    "$01E 0022.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 00101010101010101010101010101010";    // write Master to  Slave cmd  (设置温度22度)


//  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
//  1 :digital output 1   // 它的值1表示运行;0表示停止。
// 停与写不一致。

//
//  Voetsch(德国WEISS-VOETSCH环境仪器设备有限公司) Temperature Chamber //
//----------------------------------------------------------------------------------------------------

void TH_VOETSCH::Init(unsigned char ChanNo)
{
    // 初始化函数指针。
    unsigned char uc_i; // 防止死机情况，先全部初始化一次函数指针。
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_VOETSCH::SendReadCmd);   //  2-M)读取温度主机到从机命令  (以字节操作)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_VOETSCH::SendWriteCmd);  //  1-M)设置温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_VOETSCH::SendReadCmd);    //  2-M)读取温度主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_VOETSCH::SendRunCmd);      //  3-M)要求运行命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_VOETSCH::SendStopCmd);    //  4-M)要求停止命令主机到从机命令  (以字节操作)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_VOETSCH::ParseResult);  //  5-S)读取温度从机回复主机命令  (以字节操作)
}

void TH_VOETSCH::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_VOETSCH_MIN_RESPONSE_CMD_LEN)
        return;  // 收满两条以上处理一次信息，下限保护

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < uw16_Total; uw16_i ++)   //   解决快速更新数据问题
    {   //  控制在正190度负55度。  // Temperature range -55°C to +190°C //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0X0D)  // 寻找End <CR>，过滤多余的位数。
        {
            //  "0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0 0020.3 0000.0 0020.4 01101010101010101010101010101010"
            //  14(Value)*6(个数)+32(DIDO)+14(空格0X20)+1(0X0D)=131字节
            //  第8位开始 0022.0 Actual value of parameter 1
            // 1         Nominal value of parameter 1 Set  (Value 1)
            // 1+7=8     Actual value of parameter 1  Get  (Value 2)
            // 8+7=15    Nominal value of parameter 2 Set  (Value 3)
            // 15+7=22   Actual value of parameter 2  Get  (Value 4)
            // 22+7=29   Set value 1                       (Value 5)
            // 29+7=36   Set value 1                       (Value 6)

            if((uw16_Total - uw16_i) < 131)
                return;  // 不足一包，不用再检查。

            if(MTCI_CTRL::gR_CmdBuf[uw16_i + 131] != 0X0D)  // 寻找End <CR>，过滤多余的位数。
                continue; // 两包保护处理,避免中间有其他的回复信息加入。加了就丢去   解决写DIDO值错误问题

            //if(uw16_i < 14*READ_REPLY_POINT_VAlUE)  //  没数据再找   解决快速更新数据问题
            //{
            //	continue;
            //}
            //else
            //{
            uc_MoveFlag = 2;
            uw16_j = uw16_i + 8;  //gR_CmdBuf[uw16_i+8];
            uw16_k = uw16_i;
            break;
            //}
        }
    }

    if((uc_MoveFlag < 2)||(uw16_i >= uw16_Total)
            ||MTCI_CTRL::gR_CmdBuf[uw16_k+1] == '$')
        return;   // 找不到间隔，自动退出,或为发送命令。	 //

    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 131] != 0X0D)  // 寻找End <CR>，过滤多余的位数。
        return; // 两包保护处理,避免中间有其他的回复信息加入。加了就丢去    解决写DIDO值错误问题

    for(uw16_i = 0; uw16_i < 4; uw16_i ++)
    {
        switch(uw16_i)
        {
        case 0:
            uw16_j = uw16_k + 8;     // Actual value of parameter 1 温度实际值
            break;                   // 1+7=8     Actual value of parameter 1  Get
        case 1:
            uw16_j = uw16_k + 15;    // Nominal value of parameter 2  湿度设定值
            break;                   // 8+7=15    Nominal value of parameter 2 Set
        case 2:
            uw16_j = uw16_k + 22;    // Actual value of parameter 2  湿度实际值
            break;                   // 15+7=22   Actual value of parameter 2  Get
        case 3:
            uw16_j = uw16_k + 29;    // Set value 1
            break;                   // 22+7=29   Set value 1
        default:
            uw16_j =uw16_j;
            break;
        }
        pCmd = MTCI_CTRL::String_MoveBuff(&MTCI_CTRL::gR_CmdBuf[uw16_j],&uc_Buffer[uw16_i][0],TH_VOETSCH_READ_REPLY_POINT_VAlUE); //DATA FROM CHAMBER
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
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1 温度实际值
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                break;

            case 1:
                if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
                {
                    MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]);      //  从温控器读到
                    PresentValue = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
                }
                break;

            case 2:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]); // Actual value of parameter 2  湿度实际值
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
                break;

            case 3:  //if(gR_Updata_Counter[ChanNo] > 3)
                // {
                // gR_Updata_Counter[ChanNo] = 0;
                MTCI_CTRL::gR_Updata_Flag[ChanNo] = TRUE;

                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::gR_PresentValue_Set1[ChanNo]); //    Set value 1
                PresentValue = MTCI_CTRL::gR_PresentValue_Set1[ChanNo];

                //gR_PresentValue_Set1[ChanNo] = PresentValue;    //    Set value 1

                //ctrl_SendWriteCmd(ChanNo);   // USE TO TEST
                //ctrl_SendStopCmd(ChanNo);  // USE TO TEST


                uw32_DO = 0;
                for(uw16_j = 0; uw16_j < 30; uw16_j ++)   //  两帧取前一帧的DIDO
                {
                    //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
                    //  1 :digital output 1   // 它的值1表示运行;0表示停止。
                    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 130 - uw16_j] == '1') //Unused digital output 0，1，2，Other digital outputs
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

            //if(gR_Updata_Flag[ChanNo] == TRUE)
            //	return;  //break;   // 仅存一次取到的湿度,DIDO.
        }
    }

}


void TH_VOETSCH::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送写控制值命令

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间

    if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
        return;



    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_VOETSCH[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    //MbBuf[WRITE_CMD_POINT_VAlUE] = (unsigned char) (register_data >> 8);
    //MbBuf[WRITE_CMD_POINT_VAlUE+1] = (unsigned char) (register_data & 255);

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_WRITE_CMD_POINT_VAlUE + 1);  // Nominal value of parameter 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE);    // 缩小10倍处理

    register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_WRITE_CMD_POINT_VAlUE + 8);  // Nominal value of parameter 2
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // 缩小10倍处理

    register_data = MTCI_CTRL::gR_PresentValue_Set1[ChanNo];
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_WRITE_CMD_POINT_VAlUE + 15);  // Set value 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+12);    // 缩小10倍处理

    for(MbChkSum = 0; MbChkSum < 30; MbChkSum++)
    {
        //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
        //  1 :digital output 1   // 它的值1表示运行;0表示停止。
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>MbChkSum) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0，1，2，Other digital outputs
        {
            MbBuf[TH_VOETSCH_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // 置1
        }
        else
        {
            MbBuf[TH_VOETSCH_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // 置0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}


void TH_VOETSCH::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送运行命令
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_VOETSCH[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // 实际上跟发送读命令一样是读取温度命令
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TH_VOETSCH::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送读命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_VOETSCH[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}

void TH_VOETSCH::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送停止命令

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_VOETSCH[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // 命令结束标志<CR>
    //MbChkSum = CRC16(MbBuf,MAX_COM_WRITE_CMD-2);
    //MbBuf[MAX_COM_WRITE_CMD-2] = (unsigned char) (MbChkSum >> 8);
    //MbBuf[MAX_COM_WRITE_CMD-1] = (unsigned char) (MbChkSum & 255);

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+TH_VOETSCH_WRITE_CMD_POINT_VAlUE+8);  // Nominal value of parameter 2
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // 缩小10倍处理

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gR_PresentValue_Set1[ChanNo],MbBuf+TH_VOETSCH_WRITE_CMD_POINT_VAlUE+15);  // Set value 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+12);    // 缩小10倍处理

    for(MbChkSum = 0; MbChkSum < 30; MbChkSum++)
    {
        //  0 :Unused digital output 0   // 必须要保护否将无法完全接收这一命令，就不会有回复。
        //  1 :digital output 1   // 它的值1表示运行;0表示停止。
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>MbChkSum) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0，1，2，Other digital outputs
        {
            MbBuf[TH_VOETSCH_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // 置1
        }
        else
        {
            MbBuf[TH_VOETSCH_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // 置0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// 温、湿控制同为一指令时必须同时置标志以进入温、湿分别发送的指令系统运作
}




