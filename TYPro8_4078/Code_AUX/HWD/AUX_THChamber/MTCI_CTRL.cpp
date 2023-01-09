/*****************************************************************************
// 程序文件      :Mtci_ctrl.c    定义温控箱中各种处理文件
// 文件描述      :
// 编写日期      :
*****************************************************************************/

#include "../../Entry/includes.h"

UInt16  MTCI_CTRL::Priority = PRIORITY4;
UInt16  MTCI_CTRL::TaskID = UART_PARSE_TASK;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char uc_Aux_UartTaskPointer;
/*
struct  Uart_Operation
{
unsigned int short TC_BrandName;
unsigned int short BaudRate;
unsigned int short DataBit;
unsigned int short Parity;
unsigned int short StopBit;

}MTCI_CTRL::str_Uart[MAX_UART_TOTALNUM];
*/
MTCI_GROUP_CTRL  MTCI_CTRL::st_GroupControler[MAX_UART_NUM]; //温控箱主控通道 //1910101 zyx
//   TemperatureChamber
Uart_Operation MTCI_CTRL::MTCI_CTRL::str_Uart[MAX_UART_TOTALNUM];

MsTime MTCI_CTRL::LastMtciSendTime[MAX_UART_NUM];
unsigned char MTCI_CTRL::PresentChamberWriteTempCount[MAX_UART_NUM];
unsigned char MTCI_CTRL::PresentChamberReadTempCount[MAX_UART_NUM];
unsigned char MTCI_CTRL::PresentChamberWriteHumiCount[MAX_UART_NUM];
unsigned char MTCI_CTRL::PresentChamberReadHumiCount[MAX_UART_NUM];

unsigned char MTCI_CTRL::uc_Uart_TChamber_Num;   // 扫描串口通道号
unsigned short int MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM;// 命令最大公共重复次数
unsigned long int MTCI_CTRL::uw32_CMD_INTERVALTIME;// 每次发送命令间隔时间

unsigned char MTCI_CTRL::gW_CmdBuf[MAX_UART_BUF_SIZE];    // 暂时用于发送命令组包
short  MTCI_CTRL::gW_TemperatureValue[MAX_UART_NUM][MAX_CHAMBER_NUM];  // 写控制温度值SV
short  MTCI_CTRL::gW_TemperaLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];  // 最后一次写控制温度值SV
bool MTCI_CTRL::gW_IsPreWrCmdOk[MAX_UART_NUM];     // 准备处理写命令成功标志
bool MTCI_CTRL::gW_IsWriteCmdOk[MAX_UART_NUM];   // 写控制值命令成功标志
bool MTCI_CTRL::gW_IsRunCmdOk[MAX_UART_NUM];       // 运行命令成功标志
bool MTCI_CTRL::gW_IfNeedToCheckStatus[MAX_UART_NUM];   // 需要发送检查状态命令标志
bool MTCI_CTRL::gW_IfNeedToCheckSetpoint[MAX_UART_NUM]; // 需要发送调整点数命令标志
bool MTCI_CTRL::gC_IsRunning[MAX_UART_NUM];   // 正式控制设置控制值标志

unsigned int MTCI_CTRL::gW_PreWrRepeatNum[MAX_UART_NUM];    // 准备写当前值命令重复计数器
unsigned int MTCI_CTRL::gW_WriteRepeatNum[MAX_UART_NUM];    // 写命令重复计数器
unsigned int MTCI_CTRL::gW_RunRepeatNum[MAX_UART_NUM];      // 写运行命令重复计数器
unsigned int MTCI_CTRL::gW_ReadRepeatNum[MAX_UART_NUM];     // 写读命令重复计数器
unsigned int MTCI_CTRL::gW_StopRepeatNum[MAX_UART_NUM];     // 写停止命令剩余计数器

unsigned char MTCI_CTRL::gR_CmdBuf[MAX_UART_BUF_SIZE]; // 真实接收缓冲区的转移数据缓冲区  // 暂时用于接收命令组包解释
//	short int MTCI_CTRL::gR_PresentTempValue[MAX_UART_NUM];    // 当前读取出的温度值PV
//	short int MTCI_CTRL::gR_TempLastValue[MAX_UART_NUM];       // 最后一次读取出的温度值
MsTime MTCI_CTRL::gR_ReadStamp[MAX_UART_NUM];  //  已发出读命令则更新读时间
MsTime MTCI_CTRL::gR_ReadStampForAll[MAX_UART_NUM];  // 全部通道已发出读命令则更新读时间
unsigned char MTCI_CTRL::gR_ReadDelayCnt;
unsigned char MTCI_CTRL::gR_ChanCur;  // 通道扫描计数器

MsTime MTCI_CTRL::gW_WriteStamp[MAX_UART_NUM];   // 对串口发出过命令则更新时间
MsTime MTCI_CTRL::gC_TimeoutStamp[MAX_UART_NUM];    //  数据更新接收超时时间记录
unsigned long MTCI_CTRL::g_InitFlag;
//unsigned long g_SystemTime;

// 	unsigned char gl_Tx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实发送缓冲区
// 	unsigned int gl_Tx_first[MAX_UART_NUM];//  取数指针
// 	unsigned int gl_Tx_last[MAX_UART_NUM]; //  存数指针
//     unsigned char gl_Tx_EmptyFlag[MAX_UART_NUM]; //
//
// 	unsigned char gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实接收缓冲区
// 	unsigned int gl_Rx_first[MAX_UART_NUM];//  取数指针
// 	unsigned int gl_Rx_last[MAX_UART_NUM]; //  存数指针

short  MTCI_CTRL::gW_HumiValue[MAX_UART_NUM][MAX_CHAMBER_NUM];
short  MTCI_CTRL::gW_HumiLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];
//	short int MTCI_CTRL::gR_PresentHumiValue[MAX_UART_NUM];
//	short int MTCI_CTRL::gR_HumiLastValue[MAX_UART_NUM];

unsigned long int MTCI_CTRL::gR_PresentValue_DO32[MAX_UART_NUM];   // 保存DO值  Unused digital output 0，1，2，Other digital outputs
unsigned long int MTCI_CTRL::gR_PresentValue_DI32[MAX_UART_NUM];   // 保存DI值  Unused digital input 0，1，2，Other digital inputs

short int  MTCI_CTRL::gR_Zero_Counter[MAX_UART_NUM];     // 接收数据为零值计算器
//short int  gR_Updata_Counter[MAX_UART_NUM];    // 接收数据更新计算器
//short int gR_PresentValue_Set1[MAX_UART_NUM];   // 保存设置值1  Set value 1

unsigned char  MTCI_CTRL::g_Cmd_Flag[MAX_UART_NUM];         // 当前执行代码命令
unsigned char  MTCI_CTRL::gW_WriteCmd_Flag[MAX_UART_NUM];   // 写温度/湿度控制值命令标志
unsigned char  MTCI_CTRL::gW_ReadCmd_Flag[MAX_UART_NUM];    // 读温度/湿度控制值命令标志
unsigned char  MTCI_CTRL::gW_StopCmd_Flag[MAX_UART_NUM];    // 停止温度/湿度控制值命令标志
unsigned char  MTCI_CTRL::gW_RunCmd_Flag[MAX_UART_NUM];     // 运行温度/湿度控制值命令标志
unsigned char  MTCI_CTRL::g_WriteCmd_Counter[MAX_UART_NUM]; // 写命令计数器
unsigned char  MTCI_CTRL::g_ReadCmd_Counter[MAX_UART_NUM];  // 读命令计数器
unsigned char  MTCI_CTRL::g_StopCmd_Counter[MAX_UART_NUM];  // 停止命令计数器
unsigned char  MTCI_CTRL::g_RunCmd_Counter[MAX_UART_NUM];   // 运行命令计数器

unsigned char  MTCI_CTRL::gR_ReadCmd_Flag[MAX_UART_NUM];    // 读温度/湿度命令标志  备份用于读取数据处理温度/湿度状态
MsTime         MTCI_CTRL::gR_ReadCmd_Stamp[MAX_UART_NUM];   // 读温度/湿度命令更新时间

unsigned char  MTCI_CTRL::gW_HumiFlag[MAX_UART_NUM];         //  用于标志是否已经设置湿度
MsTime  MTCI_CTRL::gW_HumiInterval[MAX_UART_NUM];     //  用于设置湿度间隔时间
MsTime   MTCI_CTRL::Wait_delay[MAX_UART_NUM];   // 写完命令延时
unsigned char   MTCI_CTRL::uc_If_Wait_delay[MAX_UART_NUM];  // 写完命令延时标志
unsigned char   MTCI_CTRL::uc_OffLine[MAX_UART_NUM][MAX_CHAMBER_NUM];
//------------------------------
//  << Voetsch温控器 >>  Begin
unsigned char MTCI_CTRL::gw_WarnLimit_Flag[MAX_UART_NUM];    // 写警告限制标置
unsigned char MTCI_CTRL::gR_Updata_Flag[MAX_UART_NUM];    // 接收数据更新标置
short int  MTCI_CTRL::gR_Updata_Counter[MAX_UART_NUM];    // 接收数据更新计算器
//short int gR_PresentValue_Nominal2[MAX_UART_NUM];  // 保存名义值2避免更改  Nominal value of parameter 2
//short int gR_PresentValue_Set1[MAX_UART_NUM][12];   // 保存设置值1  Set value 1
short int MTCI_CTRL::gR_PresentValue_Set1[MAX_UART_NUM];   // 保存设置值1  Set value 1
unsigned long int MTCI_CTRL::gR_PV_DO32[MAX_UART_NUM];   // 保存DO值  Unused digital output 0，1，2，Other digital outputs
//  << Voetsch温控器 >>  End
//----------------------------------

//------------------------------
//  << PLCS7-200温控器 >>  Begin
unsigned char MTCI_CTRL::uc_RunTH[MAX_UART_NUM];      // 运行温或湿模式
unsigned char MTCI_CTRL::uc_RunFrist[MAX_UART_NUM];   // 第一次运行温或湿模式
unsigned char MTCI_CTRL::uc_RunCount[MAX_UART_NUM];   // 第一次运行温或湿模式计算次数
unsigned char MTCI_CTRL::uc_ReadCount[MAX_UART_NUM];  // 温或湿模式读取时计算次数
unsigned char MTCI_CTRL::uc_LockModleT[MAX_UART_NUM];  // 锁定温度模式
short  MTCI_CTRL::gW_T_Back[MAX_UART_NUM];  // 写控制温度值SV  备份
short  MTCI_CTRL::gR_PV_Back[MAX_UART_NUM];  // 读温度值PV  备份
//  << PLCS7-200温控器 >>  End
//----------------------------------


//     struct FUNC{
//     void (* Function) (void);		//function 函数指针   //格式  void function(void);
//     }st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//  End  TemperatureChamber




/*
//-------------------------------------------------------
//  1-M)设置温度主机到从机 (以字节操作)
unsigned char uc_COM_WriteCmd[SEND_MAX_BUF];            // write Master to  Slave cmd  (设置温度22度)

//  从机回复时，Reply设置值：
unsigned char uc_COM_WriteReply[SEND_MAX_BUF];     // write Slave to  Master reply (回复设置成功)

//  2-M)读取温度主机到从机  (以字节操作)
unsigned char uc_COM_ReadCmd[SEND_MAX_BUF];    // read Master to  Slave cmd (读取)

//  从机回复时，Reply
unsigned char uc_COM_ReadReply[SEND_MAX_BUF];     // read Slave to  Master reply

//  3-M)要求运行命令主机到从机
unsigned char uc_COM_RunCmd[SEND_MAX_BUF];    // read Master to  Slave cmd

//  4-M)要求停止命令主机到从机命令
unsigned char uc_COM_StopCmd[SEND_MAX_BUF];    // read Master to  Slave cmd
//----------------------------------------------------------------------------------------------------
*/


struct FUNC {
    My_Func_Ptr_Ext1  Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


#pragma arm section //结束外存变量定义

bool 		MTCI_CTRL::m_btRefreshMap;

#if(DEBUG_TRACE_RECORD== 1)
UWord32 uw32_TimeStamp_Test[100];
UWord16 uw16_Put;
UWord16 uw16_Get;
UWord32 uw32_Count[10];
#endif


void MTCI_CTRL::MyInit(void)
{
    unsigned char ChanNo;

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = CMD_INTERVALTIME;
    m_btRefreshMap = false;

    //    struct Uart_Operation * S_Poin = (struct Uart_Operation * ) &uc_EEPROM_Read_Buf[0];

    //unsigned short int uw16_i;


    for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
    {
        MTCI_CTRL::DtReset(ChanNo);

        for(unsigned char uc_Chamber = 0; uc_Chamber < MAX_CHAMBER_NUM; uc_Chamber++)
        {
            gW_TemperatureValue[ChanNo][uc_Chamber] = 220;  // 默认常温时温度值为22℃度
            gW_HumiValue[ChanNo][uc_Chamber] = 550;  // 默认常态时湿度值为55%RH
            gW_TemperaLastValue[ChanNo][uc_Chamber] = gW_TemperatureValue[ChanNo][uc_Chamber];
            gW_HumiLastValue[ChanNo][uc_Chamber] = gW_HumiValue[ChanNo][uc_Chamber];
        }
        gW_StopRepeatNum[ChanNo] = 0;
        //gW_WriteStamp[ChanNo] = 0;
        //gR_ReadStamp[ChanNo] = 0;
        //gC_TimeoutStamp[ChanNo] = 0;

        CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);
        CommonFunc::GetTimeTicksUpdate( &gR_ReadStamp[ChanNo]);
        CommonFunc::GetTimeTicksUpdate( &gC_TimeoutStamp[ChanNo]);

        uc_OffLine[ChanNo][FIRST_CHAMBER_CHANNEL]=false;
        uc_OffLine[ChanNo][SECOND_CHAMBER_CHANNEL]=false;
        gC_IsRunning[ChanNo]=false;              //  不控制设置值
        MTCI_CTRL::Clear_Counter(ChanNo);

        PresentChamberWriteTempCount[ChanNo] = 0;
        PresentChamberReadTempCount[ChanNo] = 0;
        PresentChamberWriteHumiCount[ChanNo] = 0;
        PresentChamberReadHumiCount[ChanNo] = 0;

        memset(&st_GroupControler[ChanNo],0, sizeof(MTCI_GROUP_CTRL));//1910101 zyx
        //         MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_570i;     // TC_4
        //         MTCI_CTRL::str_Uart[ChanNo].BaudRate = TC_BR_9600;
        //         MTCI_CTRL::str_Uart[ChanNo].DataBit = TC_DATA_BIT_8;
        //         MTCI_CTRL::str_Uart[ChanNo].Parity = TC_PARITY_NONE;
        //         MTCI_CTRL::str_Uart[ChanNo].StopBit = TC_STOP_BIT_ONE;
        //         UARTInit(ChanNo,MTCI_CTRL::str_Uart[ChanNo].BaudRate,MTCI_CTRL::str_Uart[ChanNo].DataBit, MTCI_CTRL::str_Uart[ChanNo].Parity,MTCI_CTRL::str_Uart[ChanNo].StopBit);
    }

    if(g_InitFlag != 0xA5A55A5A)
    {
        for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
        {
            str_Uart[ChanNo].TC_BrandName = 0;
            str_Uart[ChanNo].BaudRate = 0;
            str_Uart[ChanNo].DataBit = 0;
            str_Uart[ChanNo].Parity = 0;
            str_Uart[ChanNo].StopBit = 0;
            str_Uart[ChanNo].uc_Chamber_TempCount = 0;
            str_Uart[ChanNo].uc_Chamber_HumiCount = 0;

            for(unsigned char uc_Chamber = 0; uc_Chamber < MAX_CHAMBER_NUM; uc_Chamber++)
            {
                str_Uart[ChanNo].gR_PresentTempValue[uc_Chamber] = 220;  // 默认常温时温度值为22℃度
                str_Uart[ChanNo].gR_PresentHumiValue[uc_Chamber] = 550;  // 默认常态时湿度值为55%RH
                str_Uart[ChanNo].gR_TempLastValue[uc_Chamber] = 220;
                str_Uart[ChanNo].gR_HumiLastValue[uc_Chamber] = 550;

                str_Uart[ChanNo].H_in_chan_id[uc_Chamber] = 255;
                str_Uart[ChanNo].H_in_group_id[uc_Chamber] = 255;
                str_Uart[ChanNo].T_in_chan_id[uc_Chamber] = 255;
                str_Uart[ChanNo].T_in_group_id[uc_Chamber] = 255;
            }

            g_InitFlag = 0xA5A55A5A;
        }
    }

    /*    #if AUX_MULTI_FUNCTION_BOARD == 1
    // 多功能板：  4号为温控箱UART 温度，5号为温控箱UART 湿度; 6号温控箱UART 16DI;7号温控箱UART 16D0;
    for(unsigned char boardIdx = 4;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)
    {
    if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE)
    {
    unsigned char auxChnIdx = 0;
    while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
    {
    Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 22;
    auxChnIdx++;
    }
    }

    if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_FLOW_RATE)
    {
    unsigned char auxChnIdx = 0;
    while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
    {
    Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 55;
    auxChnIdx++;
    }
    }
    }
    #endif
    */
    // 一次性读出，分别处理。
    //EEPROM_Read(SEL_MODULE_SPI0, 4*EEP_TCHAMBER_UART_LENGTH, EEP_CHAMBER_UART_FMSPI );   // Address 0x2200 to 0x2250

    //初始化 str_Uart
    for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
    {
        MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = 0;
        MTCI_CTRL::str_Uart[ChanNo].BaudRate = 0;
        MTCI_CTRL::str_Uart[ChanNo].DataBit = 0;
        MTCI_CTRL::str_Uart[ChanNo].Parity = 0;
        MTCI_CTRL::str_Uart[ChanNo].StopBit = 0;

        str_Uart[ChanNo].uc_Chamber_TempCount = 0;
        str_Uart[ChanNo].uc_Chamber_HumiCount = 0;
    }

    //S_Poin = (struct Uart_Operation * ) &uc_EEPROM_Read_Buf[ChanNo*EEP_TCHAMBER_UART_LENGTH];

    //从databank中找出串口的配置存入str_Uart, 以便后者初始化
    for(int g_id = 0; g_id < 8; g_id++)
        for(int c_id = 0; c_id < 16; c_id++)
        {
            if( (Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_AuxSubType == UART) && (Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_Populated == 1) )
            {
                ChanNo = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_UART_Port;

                if(str_Uart[ChanNo].uc_Chamber_TempCount == 0)
                {
                    MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Device_Type;
                    MTCI_CTRL::str_Uart[ChanNo].BaudRate = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Baud_Rate;
                    MTCI_CTRL::str_Uart[ChanNo].DataBit = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_COM_DataBit;
                    MTCI_CTRL::str_Uart[ChanNo].Parity = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_COM_Parity;
                    MTCI_CTRL::str_Uart[ChanNo].StopBit = Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_COM_StopBit;
                }

                if(Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_AuxType == AUX_TEMPERATURE)
                {
                    MTCI_CTRL::str_Uart[ChanNo].T_in_group_id[str_Uart[ChanNo].uc_Chamber_TempCount] = g_id;
                    MTCI_CTRL::str_Uart[ChanNo].T_in_chan_id[str_Uart[ChanNo].uc_Chamber_TempCount] = c_id;
                    str_Uart[ChanNo].uc_Chamber_TempCount++;
                }
                else if(Aux_DataManager::m_AuxDataBank[g_id][c_id].m_Chn_Info.m_AuxType == AUX_HUMIDITY)
                {
                    MTCI_CTRL::str_Uart[ChanNo].H_in_group_id[str_Uart[ChanNo].uc_Chamber_HumiCount] = g_id;
                    MTCI_CTRL::str_Uart[ChanNo].H_in_chan_id[str_Uart[ChanNo].uc_Chamber_HumiCount] = c_id;
                    str_Uart[ChanNo].uc_Chamber_HumiCount++;
                }
            }
        }
    for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
    {


#if(_TEST_TO_CHECK_== 1)
        //----------------测试暂时锁定---------------
        MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_SAMWONTECH_TEMP2500;
        //MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_4;
        //MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_570i;   // TC_4
        //MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_Voetsch;   // TC_4
        //MTCI_CTRL::str_Uart[ChanNo].TC_BrandName = TC_WATLOW_F4;
        MTCI_CTRL::str_Uart[ChanNo].BaudRate = TC_BR_9600;
        MTCI_CTRL::str_Uart[ChanNo].DataBit = TC_DATA_BIT_8;
        MTCI_CTRL::str_Uart[ChanNo].Parity = TC_PARITY_NONE;
        MTCI_CTRL::str_Uart[ChanNo].StopBit = TC_STOP_BIT_ONE;
        //----------------测试暂时锁定---------------
#endif

        switch (MTCI_CTRL::str_Uart[ChanNo].TC_BrandName)
        {
        //================================================
        case TC_WATLOW_F4:                 //  单独温度控制
            TC_WATLOW_F4::Init(ChanNo);
            break;
        case TC_SCP220:
            //TC_SCP220::Init(ChanNo);
            TH_SCP220::Init(ChanNo);
            break;
        case TC_SAMWONTECH_TEMP880:
            TC_TEMP880::Init(ChanNo);
            break;
        case TC_SAMWONTECH_TEMP2500:
            TC_TEMP2500::Init(ChanNo);
            break;
        case TC_2204E:
            TC_2204E::Init(ChanNo);
            break;
        case TC_P91:
            TC_FDCP91::Init(ChanNo);
            break;
        case TC_8200:
            TC_8200::Init(ChanNo);
            break;
        case TC_PC200:
            TC_PC200::Init(ChanNo);
            break;
        //-----------------------------------------
        case TC_THS2005:                        //  未特别定义
            TC_THS2005::Init(ChanNo);
            break;
        case TC_3216:
            TC_3216::Init(ChanNo);
            break;
        case TC_MB1:                    //  温度控制
            TC_MB1::Init(ChanNo);
            break;
        case TC_HLT403P:
            TC_HLT403P::Init(ChanNo);
            break;
        case TC_JULABO_FP50MA:
            TC_JULABO_FP50MA_F4::Init(ChanNo);
            break;
        case TC_JUFU_9700S:
            TH_JUFU_9700S::Init(ChanNo);
            break;
        case TC_SUN_EC1X:
            TC_SUN_EC1X::Init(ChanNo);
            break;
        //================================================
        case TC_570i:                    //  温度、湿度同时控制
            TH_EZT570I::Init(ChanNo);
            break;
        case TC_430i:
            TH_EZT430I::Init(ChanNo);
            break;
        case TC_Voetsch:
            TH_VOETSCH::Init(ChanNo);
            break;
        case TC_Voetsch_VC_L4006:       //  温度、湿度同时控制
            TH_VOETSCH_L4006::Init(ChanNo);
            break;
        //-----------------------------------------
        case TC_TEMI2500:                       //  未特别定义
            TH_TEMI2500::Init(ChanNo);
            break;
        case TC_TPC7062K:
            TH_TPC7062K::Init(ChanNo);
            break;
        case TC_S7_226CN:
            //MTCI_CTRL::str_Uart[ChanNo].StopBit = TC_STOP_BIT_TWO;
            TH_S7_226CN::Init(ChanNo);
            break;
        case TC_CLCD_9700:
            TH_CLCD_9700::Init(ChanNo);
            break;
        case TC_SHANGHAI_PINDUN_TEMI880:
            TH_SHANGHAI_PINDUN_TEMI880::Init(ChanNo);
            break;
        case TC_TT_5166:
            TH_TT_5166::Init(ChanNo);
            break;
        case TC_GUANGZHOU_WUSUO:
            TH_GUANGZHOU_WUSUO::Init(ChanNo);
            break;
        case TC_DONGGUAN_HUANCE_UMC1200:
            TH_HUANCE_UMC1200::Init(ChanNo);
            break;
        case TC_CHONGQING_YINHE_MODBUSTCP:
            TH_YINHE_MODBUSTCP::Init(ChanNo);
            break;
        case TC_HLT504P:
            TH_HLT504P::Init(ChanNo);
            break;
        case TC_CHONGQING_ATAIHE:
            TH_ATAIHE::Init(ChanNo);
            break;
        case TC_ESPEC_GPL_4:
            TH_GPL_4::Init(ChanNo);
            break;
        case TC_JULABO_A45T:
            TC_JULABO_A45T::Init(ChanNo);
            break;
        case TC_TM221CE24T:
            TC_TM221CE24T::Init(ChanNo);
            break;
        case TC_US_9256P_ACCU3:
            TH_US_9256P_ACCU3::Init(ChanNo);
            break;
        case TC_SIEMENS_S7_SAIA:
            TH_SIEMENS_S7_SAIA::Init(ChanNo);
            break;
        case TC_9700S_2C:
            TH_9700S_2C::Init(ChanNo);
            break;
        case TC_SIEMENS_S7_XC1001A_SAIA:
            TH_SIEMENS_S7_XC1001A_SAIA::Init(ChanNo);
            break;
        case TC_LAUDA_PRO_P2E:
            TC_LAUDA_PRO_P2E::Init(ChanNo);
            break;
        case TC_HITACHI_EC15MHP:
            TH_HITACHI_EC15MHP::Init(ChanNo);
            break;
        case TC_Voetsch_Network:
            TH_VOETSCH_NETWORK::Init(ChanNo);
            break;
        case TC_TEMI2500_MODBUS2:
            TC_TEMI2500_MODBUS2::Init(ChanNo);
            break;
        case TC_HLT504P_ONLY_TEMP:
            TC_HLT504P::Init(ChanNo);
            break;

        case TH_MEMMERTGMBH_RS232:
            TH_MEMMERTGMBH_RS232::Init(ChanNo);
            break;
        case TC_MT8071:
            TC_MT8071_MODBUS::Init(ChanNo);
            break;
        case TC_SCP220_ONLY_TEMP:
            TC_SCP220::Init(ChanNo);
            break;
        case TC_ESPEC_GPL_4_ONLY_TEMP:
            TC_GPL_4::Init(ChanNo);
            break;
        case TC_MB2:
            TC_MB2::Init(ChanNo);
            break;
        case TC_U740:
            TC_U740::Init(ChanNo);
            break;
        case TC_SIMPAC_TCP:
            TC_SIMPAC::Init(ChanNo);
            break;
        case TC_TT_5166_Sunwoda:
            TH_TT_5166_Sunwoda::Init(ChanNo);
            break;
        case TC_SONGHUA_2PT_BD:
            TH_SONGHUA_2PT_BD::Init(ChanNo);
            break;
        case TC_9700S_2C_ONLY_TEMP:
            TC_9700S_2C::Init(ChanNo);
            break;
        case TC_DONGDA_TCY1023:
            TC_DONGDA::Init(ChanNo);
            break;
        case TC_SIEMENS_S7_XC1001A_SAIA_ONLY_TEMP:
            TC_SIEMENS_S7_XC1001A_SAIA::Init(ChanNo);
            break;
        case TC_MT8071_WITH_HUMIDITY:
            TH_MT8071_MODBUS::Init(ChanNo);
            break;
        case TC_BIA_CLIMATIC_MODBUS_TCP:
            TC_BIA_CLIMATIC::Init(ChanNo);
            break;
        case TC_ZUNDAR_AZ1000:
            TH_ZUNDAR_AZ1000::Init(ChanNo);
            break;
        case TC_HONGRUIDA_MODBUS_TCP:
            TH_HONGRUIDA_MODBUS_TCP::Init(ChanNo);
            break;
        case TC_SPI2_P1:
            TH_SPI2_P1::Init(ChanNo);
            break;
        case TC_SIGMASYSTEMS_C4:
            TC_SigmaSystemsC4::Init(ChanNo);
            break;
        case TC_WATLOW_F4_MODBUS_TCP:
            TC_WATLOW_F4T_MODBUS_TCP::Init(ChanNo);
            break;
        case TC_POLYSCIENCE_CA02A1P1:
            TC_POLYSCIENCE_CA02A1P1::Init(ChanNo);
            break;
        case TC_HONGRUIDA_MODBUS_RS485:
            TH_HONGRUIDA_MODBUS_RS485::Init(ChanNo);
            break;
        case TC_SIEMENS_S7_200:
            TH_SIMENSE_S7_200::Init(ChanNo);
            break;
        case TC_HARDING_THREE_LAYERS_MODBUS_TCP:
            TC_HARDING_THREE_LAYERS_MODBUS_TCP::Init(ChanNo);
            break;
        case TC_PLC_700_ASCII:
            TH_PLC_700_ASCII::Init(ChanNo);
            break;
        case TC_V0_45A1:
            TC_V0_45A1::Init(ChanNo);
            break;
        case TC_MT8071_DOUBLE_LAYER:
            TC_MT8071_MODBUS_DOUBLE_LAYER::Init(ChanNo);
            break;
        case TC_GT9700S:
            TH_GT9700S::Init(ChanNo);
            break;				
				case TC_WATLOW_F4T_MODBUS_FLOAT:
						TC_WATLOW_F4T_MODBUS_FLOAT::Init(ChanNo);
						break;							
        case TC_H_OYO1000:
            TC_H_OYO1000::Init(ChanNo);
            break;
        default:
            MTCI_CTRL::str_Uart[ChanNo].BaudRate = TC_BR_9600;
            MTCI_CTRL::str_Uart[ChanNo].DataBit = TC_DATA_BIT_8;
            MTCI_CTRL::str_Uart[ChanNo].Parity = TC_PARITY_NONE;
            MTCI_CTRL::str_Uart[ChanNo].StopBit = TC_STOP_BIT_ONE;
            TC_TEMP880::Init(ChanNo);    //  默认保护处理   // 安全起建默认初始化一次函数指针，避免频繁复位。
            break;
            //continue;
        }

        UARTFunc::UARTInit(ChanNo,MTCI_CTRL::str_Uart[ChanNo].BaudRate,MTCI_CTRL::str_Uart[ChanNo].DataBit, MTCI_CTRL::str_Uart[ChanNo].Parity,MTCI_CTRL::str_Uart[ChanNo].StopBit);
    }

    MTCI_CTRL::Init_SetRun();
}
void MTCI_CTRL::Clear_Counter(unsigned char ChanNo)
{
    gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    gW_RunRepeatNum[ChanNo] = 0;
    gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    gW_WriteRepeatNum[ChanNo] = 0;
    gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    gW_ReadRepeatNum[ChanNo] = 0;
    gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    gW_StopRepeatNum[ChanNo] = 0;

    g_WriteCmd_Counter[ChanNo] = 0; // 写命令计数器
    g_ReadCmd_Counter[ChanNo] = 0;  // 读命令计数器
    g_StopCmd_Counter[ChanNo] = 0;  // 停止命令计数器
    g_RunCmd_Counter[ChanNo] = 0;   // 运行命令计数器

    gR_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
    CommonFunc::GetTimeTicksUpdate( &Wait_delay[ChanNo]);
    uc_If_Wait_delay[ChanNo] = 0;
}

void MTCI_CTRL::reset(unsigned char ChanNo,unsigned char t_controller_type)	  //DK
{
    //unsigned char ChanNo;

    //for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
    //{
    if ( gC_IsRunning[ChanNo])   // 正式控制设置控制值
    {
        gW_StopRepeatNum[ChanNo] = 15;
        //ctrl_SendStopCmd(ChanNo,SETPOINTVALUE_TEMPERATURE);
        gC_IsRunning[ChanNo]=false;              //  不控制设置值
    }
    //}

    MTCI_CTRL::DtReset(ChanNo);
}

void MTCI_CTRL::DtReset(unsigned char ChanNo)
{
    //char *pCmd;
    //unsigned char ChanNo;

    //for( ChanNo = 0; ChanNo < MAX_UART_NUM; ChanNo++ )
    //{
    gW_IsPreWrCmdOk[ChanNo] = true;
    gW_IsWriteCmdOk[ChanNo] = true;
    gW_IsRunCmdOk[ChanNo] = true;

    gW_IfNeedToCheckStatus[ChanNo] = false;
    gW_IfNeedToCheckSetpoint[ChanNo] = false;

    gW_PreWrRepeatNum[ChanNo] = 100;
    gW_WriteRepeatNum[ChanNo] = 100;
    gW_RunRepeatNum[ChanNo] = 100;
    gW_ReadRepeatNum[ChanNo] = 100;

    gR_Updata_Counter[ChanNo] = 0;
    gR_Zero_Counter[ChanNo] = 0;
    gR_Updata_Flag[ChanNo] = FALSE;
    gw_WarnLimit_Flag[ChanNo] = FALSE;
    //gR_PresentValue_Nominal2[ChanNo] = 500;
    gR_PresentValue_Set1[ChanNo] = 800;
    gR_PV_DO32[ChanNo] = 0X7AAAAAAA;
    gR_PresentValue_DI32[ChanNo] = 0;
    CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);
    MTCI_CTRL::Clear_Counter(ChanNo);

    //}

    gR_ReadDelayCnt = 0;
    //gR_ReadStampForAll = g_SystemTime;
    gR_ChanCur = 0;
}


/*----*/
void MTCI_CTRL::SetPresentValue(unsigned char ChanNo, short int Value, unsigned char uc_Temp4Humi_Flag, unsigned char uc_Chamber)
{
    // 更新现时取回的值

    /*if(	Value == 0)
    {   // 避过交替出现的零值显示出现。
    if( gR_Zero_Counter[ChanNo]++ <= 5)
    return;
    else
    gR_Zero_Counter[ChanNo] = 0;
    }
    else
    {
    gR_Zero_Counter[ChanNo] = 0;
    }*/

    // if( (Value/10) != (gR_TempLastValue[ChanNo]/10) )  // COMMENT OUT 3.2.2009

    if((uc_Temp4Humi_Flag & SETPOINTVALUE_TEMPERATURE) == SETPOINTVALUE_TEMPERATURE)
    {
        if(uc_Chamber > str_Uart[ChanNo].uc_Chamber_TempCount)
            return;
        uc_OffLine[ChanNo][uc_Chamber]=false;
        unsigned char tempInGroupId = str_Uart[ChanNo].T_in_group_id[uc_Chamber];
        unsigned char tempInChnId = str_Uart[ChanNo].T_in_chan_id[uc_Chamber];

        if( Value != str_Uart[ChanNo].gR_TempLastValue[uc_Chamber] || m_btRefreshMap == true)
        {
            str_Uart[ChanNo].gR_PresentTempValue[uc_Chamber] = Value;

            if( (tempInGroupId < 8) && (tempInChnId < 16) )
            {
                Aux_DataManager::m_AuxDataBank[tempInGroupId][tempInChnId].aux_data = ((float)Value) / 10.0f;
                Aux_DataManager::m_AuxDataBank[tempInGroupId][tempInChnId].value_update_time = CommonFunc::GetSystemTime();  //160429
                MetaVariable_Task::Calculate_AuxDXDT(tempInGroupId,tempInChnId);    // all channels will be processed here in perticular order.
                m_btRefreshMap = false;
            }

            /*            #if AUX_MULTI_FUNCTION_BOARD == 1
            // 多功能板：  4号为温控箱UART 温度，5号为温控箱UART 湿度; 6号温控箱UART 16DI;7号温控箱UART 16D0;
            for(unsigned char boardIdx = 4;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)
            {
            if((Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE)
            &&(Aux_DataManager::m_AllAuxBoards[boardIdx].m_AuxSensor_ChInfo[ChanNo].m_AuxTemp.m_AuxThermoCouple.ControlModel == 1))
            {
            unsigned char auxChnIdx = 0;
            while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
            {
            if((ChanNo>=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit)
            &&(ChanNo<=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_End_InUnit))
            {
            Aux_DataManager::m_AuxDataBank[boardIdx][ChanNo].aux_data = Value/10.0;  // 缩小10倍
            }
            auxChnIdx++;
            }
            }
            }
            #endif*/
        }

        str_Uart[ChanNo].gR_TempLastValue[uc_Chamber] = Value;

#if(DEBUG_TRACE_RECORD== 1)
        uw32_Count[3]++;
#endif
    }
    else if((uc_Temp4Humi_Flag & SETPOINTVALUE_HUMI) == SETPOINTVALUE_HUMI)
    {
        if(uc_Chamber > str_Uart[ChanNo].uc_Chamber_HumiCount)
            return;
        uc_OffLine[ChanNo][uc_Chamber]=false;
        unsigned char humiInGroupId = str_Uart[ChanNo].H_in_group_id[uc_Chamber];
        unsigned char humiInChnId = str_Uart[ChanNo].H_in_chan_id[uc_Chamber];

        if( Value != str_Uart[ChanNo].gR_HumiLastValue[uc_Chamber] || m_btRefreshMap == true)
        {
            str_Uart[ChanNo].gR_PresentHumiValue[uc_Chamber] = Value;
            if( (humiInGroupId < 8) && (humiInChnId < 16) )
            {
                Aux_DataManager::m_AuxDataBank[humiInGroupId][humiInChnId].aux_data = ((float)Value) / 10.0f;
                Aux_DataManager::m_AuxDataBank[humiInGroupId][humiInChnId].value_update_time = CommonFunc::GetSystemTime();  //160429
                MetaVariable_Task::Calculate_AuxDXDT(humiInGroupId,humiInChnId);    // all channels will be processed here in perticular order.
                m_btRefreshMap = false;
            }
            /*            #if AUX_MULTI_FUNCTION_BOARD == 1
            // 多功能板：  4号为温控箱UART 温度，5号为温控箱UART 湿度; 6号温控箱UART 16DI;7号温控箱UART 16D0;
            for(unsigned char boardIdx = 4;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)
            {
            if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_FLOW_RATE)
            //   &&(Aux_DataManager::m_AllAuxBoards[boardIdx].m_AuxSensor_ChInfo[ChanNo].m_AuxTemp.m_AuxThermoCouple.ControlModel == 1))
            {
            unsigned char auxChnIdx = 0;
            while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
            {
            if((ChanNo>=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_Start_InUnit)
            &&(ChanNo<=Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_End_InUnit))
            {
            Aux_DataManager::m_AuxDataBank[boardIdx][ChanNo].aux_data = Value/10.0;    // 缩小10倍
            }
            auxChnIdx++;
            }
            }
            }
            #endif*/
        }

        str_Uart[ChanNo].gR_HumiLastValue[uc_Chamber] = Value;

#if(DEBUG_TRACE_RECORD== 1)
        uw32_Count[4]++;
#endif
    }

}

void MTCI_CTRL::DoAllWriteCmdCtrl(unsigned char ChanNo)
{
    //  补充和重复执行多次控制命令处理
    //-------对于RS485最合适的程序，在同一串口线上要等待发送读命令,多个串口不能用一次性循环操作-----------

    //-------对于RS232最合适的程序，不用同一串口线，因而可直接发送读命令，多个串口可以用一次性循环操作-----------
    //  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_UartChnCoreInfo.m_Set_Value
    long g_Time;
    unsigned char uc_Success = 0;
    unsigned  char  uc_T4H = SETPOINTVALUE_NONE;

    //for( gR_ChanCur=0; gR_ChanCur < MAX_UART_NUM; gR_ChanCur++ )
    {
        g_Time = CommonFunc::GetTimeTicks( &gW_WriteStamp[ChanNo]);  // g_SystemTime - gW_WriteStamp[gR_ChanCur];
        if(g_Time <= 0)
        {
            CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
            //continue;
            return;
        }
        else
        {
            //g_Time += 5;  //  提前5毫秒,解决处理和延时占用的时间。
        }

        if(!gC_IsRunning[ChanNo])   // 还未正式控制设置控制值。（开机后一般都处于这一条件里面）
        {
            if(g_Cmd_Flag[ChanNo] == SEND_COM_STOP_CMD)
            {   // 次数计算判断
                if(g_Time > MTCI_CTRL::uw32_CMD_INTERVALTIME)
                {   // 时间计算判断
                    uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_STOP_CMD);
                    MTCI_CTRL::SendStopCmd(ChanNo,uc_T4H);
                    uc_Success = MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_STOP_CMD);
                    g_Cmd_Flag[ChanNo] = SEND_COM_STOP_CMD;    //  20191031  yy
                    CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间

                    if(uc_Success && gW_StopCmd_Flag[ChanNo] == SETPOINTVALUE_MAX)
                    {
                        MTCI_CTRL::Clear_Counter(ChanNo);
                        g_Cmd_Flag[ChanNo] = SEND_COM_READ_CMD; // 发完多次停止命令后可接着发读取数据命令
                        CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间
                    }
                }
            }

        }
        else if(!gW_IsRunCmdOk[ChanNo])  // 正式控制设置控制值后gW_IsRunCmdOk[ChanNo]=0，执行运行命令。
        {
            if(g_Time > MTCI_CTRL::uw32_CMD_INTERVALTIME)
            {   // 时间计算判断
                uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_RUN_CMD);
                MTCI_CTRL::SendRunCmd(ChanNo,uc_T4H);
                uc_Success = MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_RUN_CMD);
                g_Cmd_Flag[ChanNo] = SEND_COM_RUN_CMD;    //  20191031 yy
                CommonFunc::GetTimeTicksUpdate( &gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

                if(uc_Success && gW_RunCmd_Flag[ChanNo] == SETPOINTVALUE_MAX)   // 写运行命令重复计数器
                {   // 次数计算判断
                    gW_IsRunCmdOk[ChanNo] = true;
                    MTCI_CTRL::Clear_Counter(ChanNo);
                    CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间
                }
            }
        }
        else if(!gW_IsWriteCmdOk[ChanNo])  // 正式控制设置控制值后gW_IsWriteCmdOk[ChanNo]=0，执行写控制值命令。
        {
            if(g_Time > MTCI_CTRL::uw32_CMD_INTERVALTIME)  //  已发出读命令时间
            {   // 时间计算判断
                uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_WRITE_CMD);
                MTCI_CTRL::SendWriteCmd(ChanNo,uc_T4H);
                uc_Success = MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_WRITE_CMD);
                g_Cmd_Flag[ChanNo] = SEND_COM_WRITE_CMD;    //  20191031 yy
                CommonFunc::GetTimeTicksUpdate( &gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

                if(uc_Success && gW_WriteCmd_Flag[ChanNo] == SETPOINTVALUE_MAX)
                {
                    gW_IsWriteCmdOk[ChanNo] = true;  //  发送写控制值命令成功标志
                    g_Cmd_Flag[ChanNo] = SEND_COM_READ_CMD; // 发完多次停止命令后可接着发读取数据命令
                    MTCI_CTRL::Clear_Counter(ChanNo);
                    CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间
                }
            }
        }
    }
}


void MTCI_CTRL::DoAllReadCmdCtrl(unsigned char ChanNo)
{
    //-------对于RS485最合适的程序，在同一串口线上要等待发送读命令,多个串口不能用一次性循环操作-----------

    //-------对于RS232最合适的程序，不用同一串口线，因而可直接发送读命令，多个串口可以用一次性循环操作-----------
    long g_Time;
    unsigned  char  uc_T4H = SETPOINTVALUE_NONE;
    unsigned  char  uc_CMD = SEND_COM_READ_CMD;

    //for(gR_ChanCur = 0; gR_ChanCur < MAX_UART_NUM;gR_ChanCur ++)
    {
        // 		if(!ctrl_IfCanSendReadCmd(gR_ChanCur))
        // 			return; // 必不会超过接收的最大保护长度

        g_Time = CommonFunc::GetTimeTicks( &gW_WriteStamp[ChanNo]); // 对串口发出过命令
        if(g_Time < MTCI_CTRL::uw32_CMD_INTERVALTIME)
        {
            //continue;  // 对串口发出过命令则保护
            return;
            /*if(++gR_ChanCur >= MAX_UART_NUM)
            gR_ChanCur = 0;
            return;*/
        }

        g_Time = CommonFunc::GetTimeTicks( &gR_ReadStampForAll[ChanNo]);  // 全部通道已发出读命令
        if(g_Time <= 0)
        {
            CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间
            //continue;
            return;
            /*if(++gR_ChanCur >= MAX_UART_NUM)
            gR_ChanCur = 0;
            return;*/
        }
        else
        {
            //g_Time += 5;  //  提前5毫秒,解决处理和延时占用的时间。
        }

        if(g_Time < MTCI_CTRL::uw32_CMD_INTERVALTIME)    // 延时200毫秒发送读命令
        {
            //continue;
            return;
            /*if(++gR_ChanCur >= MAX_UART_NUM)
            gR_ChanCur = 0;
            return;*/
        }
        else
        {
            CommonFunc::GetTimeTicksUpdate( &gR_ReadStampForAll[ChanNo]);    // 全部通道已发出读命令则更新读时间
        }

        if(gC_IsRunning[ChanNo])   // 正式控制设置控制值
        {
            if(gW_IsWriteCmdOk[ChanNo] && gW_IsRunCmdOk[ChanNo])
            {
#if COM_INTERVAL_WRITE_AND_READ==1
                if(g_Cmd_Flag[ChanNo] == SEND_COM_WRITE_CMD)
                {   // 交替发送写更新值命令和读命令
                    uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_WRITE_CMD);
                    MTCI_CTRL::SendWriteCmd(ChanNo,uc_T4H);
                    MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_WRITE_CMD);   // 温控器只能单命令接收时仅发一条指令
                    MTCI_CTRL::ExchangeCheckCounter(ChanNo, &g_Cmd_Flag[ChanNo], SEND_COM_READ_CMD, &g_WriteCmd_Counter[ChanNo]);
                }
                else
                {
                    uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_READ_CMD);
                    MTCI_CTRL::SendReadCmd(ChanNo,uc_T4H);
                    MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_READ_CMD);   // 温控器只能单命令接收时仅发一条指令
                    MTCI_CTRL::ExchangeCheckCounter(ChanNo, &g_Cmd_Flag[ChanNo], SEND_COM_WRITE_CMD, &g_ReadCmd_Counter[ChanNo]);
                }
#else     // 仅发送读命令
                uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_READ_CMD);
                MTCI_CTRL::SendReadCmd(ChanNo,uc_T4H);
                MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_READ_CMD);   // 温控器只能单命令接收时仅发一条指令
                MTCI_CTRL::ExchangeCheckCounter(ChanNo, &g_Cmd_Flag[ChanNo], SEND_COM_READ_CMD, &g_ReadCmd_Counter[ChanNo]);
#endif
            }
            CommonFunc::GetTimeTicksUpdate( &gR_ReadStamp[ChanNo]);   //  已发出读命令则更新读时间
        }
        else //if((gR_ReadDelayCnt%MAX_UART_NUM) == gR_ChanCur)
        {   // 还未正式控制设置控制值或者处于停止状态时。
            if(g_Cmd_Flag[ChanNo] != SEND_COM_STOP_CMD)  // 写停止命令外面处理
            {
                uc_T4H = MTCI_CTRL::Get_SetPoinState(ChanNo,SEND_COM_READ_CMD);
                MTCI_CTRL::SendReadCmd(ChanNo,uc_T4H);
                MTCI_CTRL::Change_T_H(ChanNo,SEND_COM_READ_CMD);   // 温控器只能单命令接收时仅发一条指令
                MTCI_CTRL::ExchangeCheckCounter(ChanNo, &g_Cmd_Flag[ChanNo], SEND_COM_READ_CMD, &g_ReadCmd_Counter[ChanNo]);
            }
            CommonFunc::GetTimeTicksUpdate( &gR_ReadStamp[ChanNo]);  //  已发出读命令则更新读时间
        }
    }

    /*if(++gR_ChanCur >= MAX_UART_NUM)
    gR_ChanCur = 0;
    gR_ReadDelayCnt++;*/
}

void MTCI_CTRL::DoAllParseResultCtrl(unsigned char ChanNo)
{
    //-------对于RS485最合适的程序，在同一串口线上要等待发送读命令,多个串口不能用一次性循环操作-----------

    //-------对于RS232最合适的程序，不用同一串口线，因而可直接发送读命令，多个串口可以用一次性循环操作-----------
    long g_Time;
    unsigned char  uc_T4H;

    if(!uc_If_Wait_delay[ChanNo])
    {
        g_Time = CommonFunc::GetTimeTicks( &Wait_delay[ChanNo]);
        if(g_Time <= (MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM*10000)) // 避开写数据的回馈信息n秒
        {
            UARTFunc::uart_empty(ChanNo);  // 直接放弃数据
            return;
        }
        else
            uc_If_Wait_delay[ChanNo] = 1;
    }

    //for( MTCI_CTRL::gR_ChanCur = 0; MTCI_CTRL::gR_ChanCur < MAX_UART_NUM; MTCI_CTRL::gR_ChanCur++ )
    {
        //uc_T4H = MTCI_CTRL::Get_SetPoinState(MTCI_CTRL::gR_ChanCur,SEND_COM_READ_CMD);
        uc_T4H = MTCI_CTRL::gR_ReadCmd_Flag[ChanNo];
        MTCI_CTRL::ParseResult(ChanNo,uc_T4H);
        g_Time = CommonFunc::GetTimeTicks( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 接收超时时间
        if(g_Time <= 0)
        {
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间
            //continue;
            return;
        }
        else
        {
            //g_Time += 5;  //  提前5毫秒,解决处理和延时占用的时间。
        }
        if(g_Time > READ_TIMEOVER_RATE)  // 判断接收超时时间2分钟 // 09/17/2013 针对251板一定要增大这个设置时间否则永远显示22度。
        {
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // 更新接收超时时间

            for(unsigned char uc_Chamber = 0; uc_Chamber < MAX_CHAMBER_NUM; uc_Chamber++)
            {
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[uc_Chamber] = 220;  // 默认常温时温度值为22℃度
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[uc_Chamber] = 550;  // 默认常态时湿度值为55%RH

                if( (str_Uart[ChanNo].T_in_group_id[uc_Chamber] < 8) && (str_Uart[ChanNo].T_in_chan_id[uc_Chamber] < 16) )
                {
                    m_btRefreshMap = true;
                    SetPresentValue(ChanNo, str_Uart[ChanNo].gR_PresentTempValue[uc_Chamber], SETPOINTVALUE_TEMPERATURE, uc_Chamber);
                    m_btRefreshMap = true;
                    SetPresentValue(ChanNo, str_Uart[ChanNo].gR_PresentHumiValue[uc_Chamber], SETPOINTVALUE_HUMI, uc_Chamber);
                }
                uc_OffLine[ChanNo][uc_Chamber] = true;// yy 2112281
            }
        }
    }
}


/*
bool ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len)
{
unsigned int i;
for(i = 0; i < Len; i++)
if( Src[i] != Dest[i] )
return false;
return true;
}
*/


void MTCI_CTRL::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    // 分析接收的结果数据
    (* st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
}

void MTCI_CTRL::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送写控制值命令
    (* st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
    CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    CommonFunc::GetTimeTicksUpdate( &gR_ReadCmd_Stamp[ChanNo]);
}


void MTCI_CTRL::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送运行命令
    (* st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
    CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    CommonFunc::GetTimeTicksUpdate( &gR_ReadCmd_Stamp[ChanNo]);
}

void MTCI_CTRL::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送读命令
    (* st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
    CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_HUMI;
    else
        MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
    CommonFunc::GetTimeTicksUpdate( &gR_ReadCmd_Stamp[ChanNo]);
}

void MTCI_CTRL::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送停止命令
    (* st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function

    gW_IsPreWrCmdOk[ChanNo]=true;
    gW_IsWriteCmdOk[ChanNo]=true;
    gW_IsRunCmdOk[ChanNo]=true;
    gW_IfNeedToCheckStatus[ChanNo]=false;
    gW_IfNeedToCheckSetpoint[ChanNo]=false;
    gC_IsRunning[ChanNo]=false;              //  不控制设置值
    CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    CommonFunc::GetTimeTicksUpdate( &gR_ReadCmd_Stamp[ChanNo]);
}


void MTCI_CTRL::SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  发送报警限制命令
    //(* st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
}

void MTCI_CTRL::SendCheckStatusCmd(unsigned char ChanNo)
{
    // 需要发送检查状态命令
    //(* st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
}


void MTCI_CTRL::SendCheckSetpointCmd(unsigned char ChanNo)
{
    // 需要发送调整点数命令
    //(* st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function) (ChanNo,uc_Temp4Humi_Flag);				// call the function
}

bool MTCI_CTRL::IfCanSendReadCmd(unsigned char ChanNo)
{
    //if(uart_bytenum(ChanNo) < MAX_RESPONSE_CMD_LEN*5)
    //if(uart_bytenum(ChanNo)<MAX_RESPONSE_CMD_LEN*3)
    //if(uart_bytenum(ChanNo) < MAX_RESPONSE_CMD_LEN)
    if(UARTFunc::uart_bytenum(ChanNo) < MAX_UART_BUF_SIZE)
        return true;
    else
        return false;
}

/*----*/
void MTCI_CTRL::SetPointValueCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag,unsigned short RunON)
{
    // 置标志和设置控制温控箱值

    //if(gC_IsRunning[ChanNo] == false)
    if(RunON == 1)
    {   //  运行开始
        gW_IsPreWrCmdOk[ChanNo] = false;
        gW_IsWriteCmdOk[ChanNo] = false;
        gW_IsRunCmdOk[ChanNo] = false;
        gW_PreWrRepeatNum[ChanNo] = 0;
        gW_WriteRepeatNum[ChanNo] = 0;
        gW_RunRepeatNum[ChanNo] = 0;
        gW_StopRepeatNum[ChanNo] = 0;
        gW_IfNeedToCheckStatus[ChanNo] = false;
        gW_IfNeedToCheckSetpoint[ChanNo] = false;

        gC_IsRunning[ChanNo] = true;  // 正式控制设置控制值
        CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        UARTFunc::uart_empty(ChanNo);   // 清空接收缓冲区

        //gW_TemperatureValue[ChanNo]= uValue;
        //gW_HumiValue[ChanNo]= uValue;

        //ctrl_SendWriteCmd(ChanNo);
        g_Cmd_Flag[ChanNo] = SEND_COM_RUN_CMD;
        gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
        gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    }
    else
    {   // 停止工作
        //gC_IsRunning[ChanNo] = false;              //  不控制设置值
        gW_IsWriteCmdOk[ChanNo] = false; // 对跳步时进行控制输出
        gW_WriteRepeatNum[ChanNo] = 0;
        CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
        g_Cmd_Flag[ChanNo] = SEND_COM_READ_CMD;
        gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    }
}


void MTCI_CTRL::ExecStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  执行停止命令温控箱操作

    // 停止工作
    gC_IsRunning[ChanNo] = false;              //  不控制设置值
    gW_IsRunCmdOk[ChanNo] = false;
    gW_IsWriteCmdOk[ChanNo] = false; // 对跳步时进行控制输出
    gW_WriteRepeatNum[ChanNo] = 0;
    gW_StopRepeatNum[ChanNo] = 0;
    CommonFunc::GetTimeTicksUpdate( &gW_WriteStamp[ChanNo]);  // 对串口发出过命令则更新时间
    g_Cmd_Flag[ChanNo] = SEND_COM_STOP_CMD;
    gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;
    gW_HumiFlag[ChanNo] = 0;
    CommonFunc::GetTimeTicksUpdate( &gW_HumiInterval[ChanNo]);  // 对串口发出过命令则更新时间
}


/*
short int ctrl_GetValueCmd(unsigned char ChanNo)
{
//  获取当前温控箱温度值
return gR_PresentTempValue[ChanNo];
}*/


void MTCI_CTRL::Init_SetRun(void)
{
    //-------对于RS485最合适的程序，在同一串口线上要等待发送读命令,多个串口不能用一次性循环操作-----------

    //-------对于RS232最合适的程序，不用同一串口线，因而可直接发送读命令，多个串口可以用一次性循环操作-----------
    unsigned char ChanCur;
    unsigned long CoutTime;
    //unsigned char *pCmd;

    for(ChanCur=0; ChanCur < MAX_UART_NUM; ChanCur++)
    {
        for(unsigned char uc_Chamber = 0; uc_Chamber < str_Uart[ChanCur].uc_Chamber_TempCount; uc_Chamber++)
        {
            if( (str_Uart[ChanCur].T_in_group_id[uc_Chamber] < 8) && (str_Uart[ChanCur].T_in_chan_id[uc_Chamber] < 16) )
            {
                m_btRefreshMap = true;
                SetPresentValue(ChanCur, str_Uart[ChanCur].gR_PresentTempValue[uc_Chamber], SETPOINTVALUE_TEMPERATURE, uc_Chamber);
            }
        }
        for(unsigned char uc_Chamber = 0; uc_Chamber < str_Uart[ChanCur].uc_Chamber_HumiCount; uc_Chamber++)
        {
            if( (str_Uart[ChanCur].H_in_group_id[uc_Chamber] < 8) && (str_Uart[ChanCur].H_in_chan_id[uc_Chamber] < 16) )
            {
                m_btRefreshMap = true;
                SetPresentValue(ChanCur, str_Uart[ChanCur].gR_PresentHumiValue[uc_Chamber], SETPOINTVALUE_HUMI, uc_Chamber);
            }
        }

        if (MTCI_CTRL::str_Uart[ChanCur].TC_BrandName == TC_THS2005)
        {
            unsigned char pCmdA[]= "STX,0,1,E,END";
            UARTFunc::uart_putcmd(ChanCur,pCmdA,14); //strlen(&pCmdA));

            unsigned char pCmdB[]="STX,0,1,W,ARB.PGM,9999,2,150,-70,22,0,0,0,1,1,1,0,22,0,1000,59,1,1,1,0,END";
            UARTFunc::uart_putcmd(ChanCur,pCmdB,75); //strlen(&pCmdB));

            //TC_THS2005_SendWriteCmd(RxTCPBuffer[15],550);

            //pCmd= "STX,0,1,S,ARB.PGM,END";
            //uart_putcmd(RxTCPBuffer[15],pCmd,strlen(pCmd));

        }
        else
        {
            if (MTCI_CTRL::str_Uart[ChanCur].TC_BrandName == TC_HLT403P)
                TC_HLT403P::SendModeCmd(0,0);
            else if (MTCI_CTRL::str_Uart[ChanCur].TC_BrandName == TC_HLT504P)
                TH_HLT504P::SendModeCmd(0,0);
            else
                MTCI_CTRL::SendReadCmd(ChanCur,SETPOINTVALUE_NONE);
        }

        UARTFunc::UARTSendFIFO(ChanCur);
        for(CoutTime=0; CoutTime < (1000*MTCI_CTRL::uw32_CMD_INTERVALTIME); CoutTime++) ;
    }

}


unsigned char MTCI_CTRL::Change_T_H(unsigned char ChanNo,unsigned char uc_CmdType)
{
    //  发送命令前判断温度操作,还是湿度操作
    unsigned  char  uc_CmdFlag = SETPOINTVALUE_TEMPERATURE;
    unsigned  char  uc_T4H = SETPOINTVALUE_TEMPERATURE;

    unsigned int *RepeatNum;
    unsigned char *Cmd_Flag;
    unsigned int MAXNum = MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM;

    switch(uc_CmdType)
    {
    case SEND_COM_WRITE_CMD:
        RepeatNum = &gW_WriteRepeatNum[ChanNo]; // 命令重复计数器
        Cmd_Flag = &gW_WriteCmd_Flag[ChanNo];
        //MAXNum = MAX_WRITE_CMD_REPEAT_NUM;
        break;

    case SEND_COM_READ_CMD:
        RepeatNum = &gW_ReadRepeatNum[ChanNo]; // 命令重复计数器
        Cmd_Flag = &gW_ReadCmd_Flag[ChanNo];
        //MAXNum = MAX_READ_CMD_REPEAT_NUM;
        break;

    case SEND_COM_RUN_CMD:
        RepeatNum = &gW_RunRepeatNum[ChanNo]; // 命令重复计数器
        Cmd_Flag = &gW_RunCmd_Flag[ChanNo];
        //MAXNum = MAX_RUN_CMD_REPEAT_NUM;
        break;

    case SEND_COM_STOP_CMD:
        RepeatNum = &gW_StopRepeatNum[ChanNo]; // 命令重复计数器
        Cmd_Flag = &gW_StopCmd_Flag[ChanNo];
        //MAXNum = MAX_STOP_CMD_REPEAT_NUM;
        break;

    default:
        return SETPOINTVALUE_TEMPERATURE;
        //break;
    }

    *RepeatNum = *RepeatNum + 1;
    if(*RepeatNum >= MAXNum) // 命令重复计数器
    {   // 次数计算判断
        if((*Cmd_Flag & SETPOINTVALUE_TEMPERATURE) == SETPOINTVALUE_TEMPERATURE)
        {
            if(uc_CmdType == SEND_COM_WRITE_CMD)
            {
                PresentChamberWriteTempCount[ChanNo]++;
            }
            else if(uc_CmdType == SEND_COM_READ_CMD)
            {
                PresentChamberReadTempCount[ChanNo]++;
            }

            if(PresentChamberWriteTempCount[ChanNo] >= str_Uart[ChanNo].uc_Chamber_TempCount)
            {
                *Cmd_Flag &= (~SETPOINTVALUE_TEMPERATURE);
                uc_T4H = SETPOINTVALUE_HUMI;
                PresentChamberWriteTempCount[ChanNo]=0;
            }
            else if(PresentChamberReadTempCount[ChanNo] >= str_Uart[ChanNo].uc_Chamber_TempCount)
            {
                *Cmd_Flag &= (~SETPOINTVALUE_TEMPERATURE);
                uc_T4H = SETPOINTVALUE_HUMI;
                PresentChamberReadTempCount[ChanNo]=0;
            }
        }
        else if((*Cmd_Flag & SETPOINTVALUE_HUMI) == SETPOINTVALUE_HUMI)
        {
            if(uc_CmdType == SEND_COM_WRITE_CMD)
            {
                PresentChamberWriteHumiCount[ChanNo]++;
            }
            else if(uc_CmdType == SEND_COM_READ_CMD)
            {
                PresentChamberReadHumiCount[ChanNo]++;
            }

            if(PresentChamberWriteHumiCount[ChanNo] >= str_Uart[ChanNo].uc_Chamber_HumiCount)
            {
                *Cmd_Flag &= (~SETPOINTVALUE_HUMI);
                uc_T4H = SETPOINTVALUE_TEMPERATURE;
                PresentChamberWriteHumiCount[ChanNo]=0;
            }
            else if(PresentChamberReadHumiCount[ChanNo] >= str_Uart[ChanNo].uc_Chamber_HumiCount)
            {
                *Cmd_Flag &= (~SETPOINTVALUE_HUMI);
                uc_T4H = SETPOINTVALUE_TEMPERATURE;
                PresentChamberReadHumiCount[ChanNo]=0;
            }
        }

        if(*Cmd_Flag == SETPOINTVALUE_NONE )
        {
            *Cmd_Flag = SETPOINTVALUE_MAX;
        }

        *RepeatNum = 0;
        return 1;
    }
    /*
    	// 保护作用
    	if((uc_T4H != SETPOINTVALUE_TEMPERATURE) && (uc_T4H != SETPOINTVALUE_HUMI))
    	{
    		uc_T4H = SETPOINTVALUE_TEMPERATURE;
    	}
    */
    return 0;
}

unsigned char* MTCI_CTRL::String_MoveBuff(unsigned char* Source, unsigned char* Destn,unsigned int uw16_length)
{
    //  接收的结果数据转移到其它缓冲区以便转换。
    //unsigned char * p_Source = Source;
    unsigned char * p_Destn = Destn;
    unsigned short int uw16_i = 0;
    unsigned short int uw16_k = 0;

    while( (*Source) == 0x20 && (*Source)!=0 )
    {
        Source++; // ignore preblank, original   // 越过空格: 0x20=Space; 0x00=NUL
    }

    for(uw16_i = 0; uw16_i < uw16_length; uw16_i++)  // 仅取字符  处理
    {   //  控制在正400度负200度。  // Temperature range -200°C to +400°C (-328°F to +725°F) //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        * Destn =  *Source;  // 跳过上一帧，平移数据
        if(* Destn == '-' ||* Destn == '+')
        {
            uw16_k = uw16_i;    // 有正负号时还需要平移。
        }
        Source++;
        Destn++;
    }

    if(uw16_k <= 0)
        return p_Destn;

    uw16_i = 0;
    Source = p_Destn + uw16_k;
    Destn =  p_Destn;

    for(uw16_i = 0; uw16_i < uw16_length; uw16_i++)  // 仅取字符  处理
    {   //  控制在正400度负200度。  // Temperature range -200°C to +400°C (-328°F to +725°F) //Resolution 0.1°C or °F, 0.1% RH, or 0.01 for other linear applications
        * Destn =  *Source;   // 有正负号时还需要平移。
        if(uw16_i >= (uw16_length - uw16_k))
        {
            * Destn = '0'; // 小数点后一位有效，其它填写零补充。
        }
        Source++;
        Destn++;
    }

    return p_Destn;

}

unsigned char MTCI_CTRL::ExchangeCheckCounter(unsigned char Channel,unsigned char *CmdSource, unsigned char CmdDest,unsigned char * Counter)
{
    // 交替发送运行命令和读命令时间检查
    unsigned char uc_ChangeFlag = 0;
    unsigned char uc_CMD = *CmdSource;
    unsigned int MAXNum = MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM;

    switch(uc_CMD)
    {
    case SEND_COM_RUN_CMD:
        if(*Counter >= MAXNum)
        {
            uc_ChangeFlag = 1;   // 交替发送运行命令和读命令
        }
        break;

    case SEND_COM_WRITE_CMD:
        if(*Counter >= MAXNum)
        {
            // 20180919  暂不交换命令  完成温度、湿度后再处理。
            if((gW_WriteCmd_Flag[Channel] == SETPOINTVALUE_NONE)||(gW_WriteCmd_Flag[Channel] == SETPOINTVALUE_MAX))
                uc_ChangeFlag = 1;  // 交替发送写命令和读命令
            else
                * Counter = 0;
        }
        break;

    case SEND_COM_STOP_CMD:
        if(*Counter >= MAXNum)
        {
            uc_ChangeFlag = 1;  // 交替发送运行命令和读命令
        }
        break;

    case SEND_COM_READ_CMD:
        if(*Counter >= MAXNum)
        {
            if((gW_ReadCmd_Flag[Channel] == SETPOINTVALUE_NONE)||(gW_ReadCmd_Flag[Channel] == SETPOINTVALUE_MAX))
                uc_ChangeFlag = 1;  // 交替发送写命令和读命令
            else
                * Counter = 0;
        }
        break;

    default:
        if(*Counter >= MAXNum)
        {
            uc_ChangeFlag = 1;  // 交替发送运行命令和读命令
        }
        else
        {
            *CmdSource = SEND_COM_READ_CMD;
        }
        break;
    }

    if(!uc_ChangeFlag)
    {
        *Counter +=1;
    }
    else
    {
        *CmdSource = CmdDest;   // 交替发送运行命令和读命令
        *Counter = 1;
    }
    return (*CmdSource);
}


unsigned char MTCI_CTRL::Get_SetPoinState(unsigned char ChanNo,unsigned char uc_CmdType)
{
    //  发送命令前判断温度操作,还是湿度操作
    unsigned  char  uc_T4H = SETPOINTVALUE_TEMPERATURE;
    switch(uc_CmdType)
    {
    case SEND_COM_WRITE_CMD:
        uc_T4H = gW_WriteCmd_Flag[ChanNo];
        break;

    case SEND_COM_READ_CMD:
        uc_T4H = gW_ReadCmd_Flag[ChanNo];
        break;

    case SEND_COM_RUN_CMD:
        uc_T4H = gW_RunCmd_Flag[ChanNo];
        break;

    case SEND_COM_STOP_CMD:
        uc_T4H = gW_StopCmd_Flag[ChanNo];
        break;

    default:
        return SETPOINTVALUE_TEMPERATURE;
        //break;
    }


    if(uc_T4H == SETPOINTVALUE_HUMI)
    {
        uc_T4H = SETPOINTVALUE_HUMI;
    }
    else
    {
        uc_T4H = SETPOINTVALUE_TEMPERATURE;
    }

    return uc_T4H;
}


void MTCI_CTRL::DoWork(void)
{
    //  选择可运行任务，用于慢速对需求时间不太快任务适用。
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_MTCI] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_MTCI);
#endif
		long g_Time = 0;

    for( gR_ChanCur=0; gR_ChanCur < MAX_UART_NUM; gR_ChanCur++ )
    {
        if(gR_ChanCur >= 1)  // 串口仅用到一个串口UART0  TXD1(P0.2)  RXD1(P0.3)
				{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_MTCI] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_MTCI);
#endif
            return;
				}

        switch(uc_Aux_UartTaskPointer)
        {
        case AUX_CHOOSETASK_IDLEDATA:
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_SPECIALFUN_COM_WRITE;
            break;

        case AUX_CHOOSETASK_SPECIALFUN_COM_WRITE:
            MTCI_CTRL::DoAllWriteCmdCtrl(gR_ChanCur);
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_SPECIALFUN_COM_WRITE_RESULT;
						CommonFunc::GetTimeTicksUpdate( &LastMtciSendTime[gR_ChanCur]);
            break;
				
        case AUX_CHOOSETASK_SPECIALFUN_COM_WRITE_RESULT:
						g_Time = CommonFunc::GetTimeTicks( &LastMtciSendTime[gR_ChanCur]);
						if(g_Time < uw32_CMD_INTERVALTIME)
							break;
            MTCI_CTRL::DoAllParseResultCtrl(gR_ChanCur);
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_SPECIALFUN_COM_READ;
            break;

        case AUX_CHOOSETASK_SPECIALFUN_COM_READ:
            MTCI_CTRL::DoAllReadCmdCtrl(gR_ChanCur);
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_SPECIALFUN_COM_READ_RESULT;
						CommonFunc::GetTimeTicksUpdate( &LastMtciSendTime[gR_ChanCur]);
            break;

        case AUX_CHOOSETASK_SPECIALFUN_COM_READ_RESULT:
						g_Time = CommonFunc::GetTimeTicks( &LastMtciSendTime[gR_ChanCur]);
						if(g_Time < uw32_CMD_INTERVALTIME)
							break;
            MTCI_CTRL::DoAllParseResultCtrl(gR_ChanCur);
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_SPECIALFUN_COM_WRITE;
            break;

        default:
            uc_Aux_UartTaskPointer = AUX_CHOOSETASK_IDLEDATA;
            break;
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_MTCI] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_MTCI);
#endif
}

