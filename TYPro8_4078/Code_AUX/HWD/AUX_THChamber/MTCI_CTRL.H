/*****************************************************************************
// 程序文件      :Mtci_ctrl.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

/******************************************************************************/
//温控箱每次更新的值都存于以下两组变量中：
//static  short int gR_PresentTempValue[MAX_UART_NUM];
//static  short int gR_PresentHumiValue[MAX_UART_NUM];
//新辅助板初始化时，可将以上各个变量地址指向对应的DataBank位置（注意变量类型: short int -> float,转换时还要除以10）
// By Yifei, 2015.7.20
/******************************************************************************/

#ifndef MTCI_CTRL_H
#define MTCI_CTRL_H 1


#include "../../COMMON_os/LPC4078CORE/UART.h"


//#define PUBLIC_REPEAT_NUM	            (1) //(5) // (20)   // 命令最大公共重复次数
//#define CMD_INTERVALTIME	            (1000) //(50) //(25) //(1000) //(200)//(1000) // 发送命令间断时间 1秒一次


//   <>PUBLIC REPEAT NUM   <1=>1 <2=>2 <5=>5 <10=>10
//   <> REPEAT  1/2/5/10
//   <> Default:    1    // 每次发送命令重复次数
#define PUBLIC_REPEAT_NUM	      (1)    // 命令最大公共重复次数

//   <o>CMD INTERVALTIME   <500=>50 <1000=>100 <2000=>200 <10000=>1000 <50000=>5000
//   <i> INTERVALTIME  50/100/200/1000/5000
//   <i> Default:    1000    // 每次发送命令间隔时间1秒
#define CMD_INTERVALTIME	     (200*10)   // 每次发送命令间隔时间200毫秒 

#define FIRST_UART_NUM            1  // 1   串口板第一个开始的串口编号

#define MAX_CHAMBER_NUM         8 //单个串口控制的温控箱数量

#define FIRST_CHAMBER_CHANNEL		0
#define SECOND_CHAMBER_CHANNEL		1
#define THIRD_CHAMBER_CHANNEL 2

// 用于次数计算判断的限定值
//#define PUBLIC_REPEAT_NUM	            (1) // (20)   // 命令最大公共重复次数
/*#define MAX_PREWR_CMD_REPEAT_NUM	    (PUBLIC_REPEAT_NUM)
#define MAX_WRITE_CMD_REPEAT_NUM	    (PUBLIC_REPEAT_NUM)
#define MAX_RUN_CMD_REPEAT_NUM		    (PUBLIC_REPEAT_NUM)
#define MAX_STOP_CMD_REPEAT_NUM		    (PUBLIC_REPEAT_NUM)
#define MAX_READ_CMD_REPEAT_NUM	        (PUBLIC_REPEAT_NUM)*/

//  回应包长度限制
#define MIN_RESPONSE_CMD_NUM        3 // 3帧   // 回应包最小数量限制
#define MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define MIN_RESPONSE_CMD_LEN		(MIN_RESPONSE_CMD_NUM*MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define MAX_RESPONSE_CMD_LEN		(MAX_RESPONSE_CMD_NUM*MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制


//#define SEND_MAX_BUF      256    // 1 byte slave address + 253 byte of PDU + 2 CRC
//#define SEND_MAX_BUF      (2*MAX_COM_WRITE_CMD)    // 最大暂存缓冲区
#define SEND_MAX_BUF        300


#define ASCII_FIRST_UART_NUM         (FIRST_UART_NUM + 0X30)  // 1   串口板第一个开始的串口编号


// 单一使用命令收发宏定义
enum TE_COM_CMD
{
    COM_NONE_CMD = 0,	         // NONE CMD
    SEND_COM_MODE_CMD,	         // SEND MODE CMD
    SEND_COM_WRITE_CMD,	         // SEND WRITE CMD
    SEND_COM_READ_CMD,	         // SEND READ CMD
    SEND_COM_RUN_CMD,	         // SEND RUN CMD
    SEND_COM_STOP_CMD,	         // SEND STOP CMD
    RECEIVE_COM_MODE_CMD,	     // RECEIVE MODE CMD
    RECEIVE_COM_WRITE_CMD,	     // RECEIVE WRITE CMD
    RECEIVE_COM_READ_CMD,	     // RECEIVE READ CMD
    RECEIVE_COM_RUN_CMD,         // RECEIVE RUN CMD
    RECEIVE_COM_STOP_CMD,	     // RECEIVE STOP CMD
    MAX_COM_CMD_TOTAL,           // MAX TOTAL
};


#define READ_TIMEOVER_RATE           (120*10000) // (CMD_INTERVALTIME/1000) // 读取数据超时系数，基准以1秒间隔一次;2分钟。 zyx 181019


// uc_Temp4Humi_Flag 设值控制值标志宏定义  位域操作
#define SETPOINTVALUE_NONE           (0X00)
#define SETPOINTVALUE_TEMPERATURE    (0X01)
#define SETPOINTVALUE_HUMI           (0X02)
#define SETPOINTVALUE_MAX            (0X03)

//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.   
#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.  
#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.  



//-------------------------------------------------------------
// MCU SPI  FM25L256  SSP1总线用于FM25L256 芯片存储数据EEPROM    256K bit=32K Byte   地址: 0X0000~0X7D00
// ID	        Name	             Bytes	      Comment            Address
// 1	Global Data Area	          64	      0 ~ 63           0X0000~0X003F
// 2	IV Channel Setting Area	      5056	      64 ~ 5119        0X0040~0X13FF
// 3	Aux Channel Setting Area	  3072	      5120 ~ 8191      0X1400~0X1FFF
// 4	Aux Channel Special Area	  64	      8192 ~ 8256	   0X2000~0X203F
// 5	TempChamber Channel Area      32	      8257 ~ 8289	   0X2040~0X205F
// 6	TempChamber PID Channel Area  320	      8290 ~ 8610	   0X2060~0X219F
// 6	TempChamber UART Channel Area  40	      8704 ~ 8744	   0X2200~0X2228

#define EEP_GLOBALBASE		        0x00           //  全局存储区开始地址
#define EEP_GBLARBINIDBASE	    0x00
#define EEP_GBLPCANOBASE	        0x04
#define EEP_PCIPBASE               0x10           //写远程PC网络地址的开始地址
#define EEP_MCUPORTBASE           0x14           //写本地MCU端口号地址的开始地址   

#define EEP_IVCHBASE		        0x40           //  IV通道信息存储区开始地址
#define EEP_IVCHCOUNTBASE	        0x0E
#define EEP_IVCHIRNGCNTBASE	    0x1A
#define EEP_IVCHVRNGCNTBASE	    0x1B
#define EEP_IVCHIRNGBASE	        0x1C
#define EEP_IVCHVRNGBASE	        0x80
#define EEP_IVCHCLAMPBASE	        0xCB
#define EEP_IVCHCALI_R_BASE	    0x08
#define EEP_IVCHCALI_S_BASE	    0x10
#define EEP_IVCH_DTFBASE	        0x18

#define EEP_AUXCHBASE_FMSPI		0x1400         //  辅助通道信息存储区开始地址
#define EEP_AUXCHCOUNTBASE_FMSPI	0x0F
#define EEP_AUXRNGBASE_FMSPI	    0x0C
#define EEP_AUXCALI_R_BASE_FMSPI	0x0
#define EEP_AUXCALI_S_BASE_FMSPI	0x0C

#define EEP_AUXSPECIALBASE_FMSPI	0x2000        //辅助通道特殊信息保存开始地址 
#define EEP_ELE_TCHAMBER_CHO_FMSPI	    0x2040        //电气板温控箱选择设置信息保存开始地址 
#define EEP_ELE_TCHAMBER_PID_FMSPI		0x2060        //电气板温控箱PID通道设置信息保存开始地址 
#define EEP_CHAMBER_UART_FMSPI		    0x2200        //温控箱UART通道设置信息保存开始地址 

#define EEP_PCIPLENGTH             0x04          //写远程PC网络地址长度
#define EEP_MCUPORTLENGTH         0x02          //写本地MCU端口号地址长度 
#define EEP_GLOBALLENGTH	        0x10
#define EEP_IVCHLENGTH		        0xFF
#define EEP_IVCHBASICLENGTH	    0x1A
#define EEP_IVCHRNGLENGTH	        0x19
#define EEP_IVCHCALILENGTH	    0x08
#define EEP_IVCHOTHERBASE	        0xE4
#define EEP_IVCHOTHERLENGTH	    0x0A
#define EEP_AUXCHLENGTH_FMSPI		0x2C
#define EEP_AUXRNGLENTH_FMSPI		0x18
#define EEP_AUXCALILENGTH_FMSPI	0x0C
#define EEP_AUXSPECIALLENGTH_FMSPI	0x04        //辅助通道特殊信息保存地址长度 
#define EEP_ELE_TCHAMBER_CHO_LENGTH	0X05       //电气板温控箱选择设置信息保存地址长度 
#define EEP_ELE_TCHAMBER_PID_LENGTH	0x21        //电气板温控箱PID通道设置信息保存地址长度 
#define EEP_TCHAMBER_UART_LENGTH	0x0A        //温控箱UART通道设置信息保存地址长度 

// MCU SPI  FM25L256  SSP1总线用于FM25L256 芯片存储数据EEPROM   256K bit=32K Byte   地址: 0X0000~0X7D00
//----------------------------------



// 以下定义为uc_Aux_ChooseTaskPointer的状态。
/*#define AUX_CHOOSETASK_IDLEDATA                   0
#define AUX_CHOOSETASK_SPECIALFUN_COM_WRITE       1
#define AUX_CHOOSETASK_SPECIALFUN_COM_READ        2
#define AUX_CHOOSETASK_SPECIALFUN_COM_RESULT      3*/

enum TE_AUX_CHOOSETASK
{
    AUX_CHOOSETASK_IDLEDATA =0,
    AUX_CHOOSETASK_SPECIALFUN_COM_WRITE,
    AUX_CHOOSETASK_SPECIALFUN_COM_READ,
    AUX_CHOOSETASK_SPECIALFUN_COM_WRITE_RESULT,
		AUX_CHOOSETASK_SPECIALFUN_COM_READ_RESULT,
};


typedef struct	//Yifei
{
    unsigned char TC_BrandName;	// 温控箱类型
    unsigned char BaudRate;
    unsigned char DataBit;
    unsigned char Parity;
    unsigned char StopBit;
	
    unsigned char uc_Chamber_TempCount;
    unsigned char uc_Chamber_HumiCount;	
    unsigned char T_in_group_id[MAX_CHAMBER_NUM];	// 该串口的温度对应 DataBank 的 Group Id
    unsigned char T_in_chan_id[MAX_CHAMBER_NUM];		// 该串口的温度对应 DataBank 的 Channel Id
    unsigned char H_in_group_id[MAX_CHAMBER_NUM];	// 该串口的湿度对应 DataBank 的 Group Id
    unsigned char H_in_chan_id[MAX_CHAMBER_NUM];		// 该串口的温度对应 DataBank 的 Channel Id
    unsigned char btNone[3];

    short int gR_PresentTempValue[MAX_CHAMBER_NUM];
    short int gR_TempLastValue[MAX_CHAMBER_NUM];
    short int gR_PresentHumiValue[MAX_CHAMBER_NUM];
    short int gR_HumiLastValue[MAX_CHAMBER_NUM];
} Uart_Operation;


typedef struct
{
    bool  IsSetTempValue;
    bool  IsSetHumiValue;
    float TempValue;
    float HumiValue;
}ChamberSetValue;

typedef struct
{
    bool    m_IsOpenChamber;
    bool    m_UseGroupTChamber;
    bool    m_UseDefaultValue;//when Master Chn stop， use new master chn control value. All chn stop, use default value.
    bool    m_IgnoreDefaultValue;//Ignore Default Value after all chn stop. Keep TChamber tempreature.    
    unsigned short m_UnitIP;
    unsigned short m_IVChnIndexInUnit;
    float   m_DefaultTempSetting;
    float   m_DefaultHumiSetting;
    ChamberSetValue m_ChamberSetValue[MAX_IVMCU_PER_SYS][MAXCHANNELNO];   
}MTCI_GROUP_CTRL;//1910101 zyx
class MTCI_CTRL
{
public:

    static Uart_Operation str_Uart[MAX_UART_TOTALNUM];
    static bool m_btRefreshMap;
    static   UInt16  Priority;
    static   UInt16  TaskID;

		static unsigned char PresentChamberWriteTempCount[MAX_UART_NUM];
		static unsigned char PresentChamberReadTempCount[MAX_UART_NUM];
		static unsigned char PresentChamberWriteHumiCount[MAX_UART_NUM];
		static unsigned char PresentChamberReadHumiCount[MAX_UART_NUM];
		static MsTime LastMtciSendTime[MAX_UART_NUM];


    static MTCI_GROUP_CTRL  st_GroupControler[MAX_UART_NUM]; //温控箱主控通道//1910101 zyx
    //  TemperatureChamber
    static  unsigned char uc_Uart_TChamber_Num;   // 扫描串口通道号
    static  unsigned short int uw16_PUBLIC_REPEAT_NUM; // 命令最大公共重复次数
    static  unsigned long int uw32_CMD_INTERVALTIME; // 每次发送命令间隔时间

    static  unsigned char gW_CmdBuf[MAX_UART_BUF_SIZE];  // 暂时用于发送命令组包
    static  short  gW_TemperatureValue[MAX_UART_NUM][MAX_CHAMBER_NUM];   // 写控制温度值SV
    static  short  gW_TemperaLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];  // 最后一次写控制温度值SV
    static  bool gW_IsPreWrCmdOk[MAX_UART_NUM];    // 准备写当前值命令成功标志
    static  bool gW_IsWriteCmdOk[MAX_UART_NUM];   // 写控制值命令成功标志
    static  bool gW_IsRunCmdOk[MAX_UART_NUM];     // 运行命令成功标志
    static  bool gW_IfNeedToCheckStatus[MAX_UART_NUM];      // 需要发送检查状态命令标志
    static  bool gW_IfNeedToCheckSetpoint[MAX_UART_NUM];    // 需要发送调整点数命令标志
    static  bool gC_IsRunning[MAX_UART_NUM];    // 正式控制设置控制值标志

    static  unsigned int gW_PreWrRepeatNum[MAX_UART_NUM];   // 准备写当前值命令重复计数器
    static  unsigned int gW_WriteRepeatNum[MAX_UART_NUM];   // 写命令重复计数器
    static  unsigned int gW_RunRepeatNum[MAX_UART_NUM];     // 写运行命令重复计数器
    static  unsigned int gW_ReadRepeatNum[MAX_UART_NUM];    // 写读命令重复计数器
    static  unsigned int gW_StopRepeatNum[MAX_UART_NUM];    // 写停止命令重复计数器


    static  unsigned char gR_CmdBuf[MAX_UART_BUF_SIZE];    // 真实接收缓冲区的转移数据缓冲区   // 暂时用于接收命令组包解释

    static  MsTime gR_ReadStamp[MAX_UART_NUM];    //  已发出读命令则更新读时间
    static  MsTime gR_ReadStampForAll[MAX_UART_NUM];   // 全部通道已发出读命令则更新读时间
    static  unsigned char gR_ReadDelayCnt;
    static  unsigned char gR_ChanCur;    // 通道扫描计数器

    static  MsTime gW_WriteStamp[MAX_UART_NUM];      // 对串口发出过命令则更新时间
    static  MsTime gC_TimeoutStamp[MAX_UART_NUM];    //  数据更新接收超时时间记录
    static  unsigned long g_InitFlag;

    // 	extern  unsigned char gl_Tx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // 真实发送缓冲区
    // 	extern  unsigned int gl_Tx_first[MAX_UART_NUM];//  取数指针
    // 	extern  unsigned int gl_Tx_last[MAX_UART_NUM]; //  存数指针
    //     extern  unsigned char gl_Tx_EmptyFlag[MAX_UART_NUM]; //

    // 	extern  unsigned char gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE];   // 真实接收缓冲区
    // 	extern  unsigned int gl_Rx_first[MAX_UART_NUM];//  取数指针
    // 	extern  unsigned int gl_Rx_last[MAX_UART_NUM]; //  存数指针


    static  short  gW_HumiValue[MAX_UART_NUM][MAX_CHAMBER_NUM];
    static  short  gW_HumiLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];


    static unsigned long int gR_PresentValue_DO32[MAX_UART_NUM];   // 保存DO值  Unused digital output 0，1，2，Other digital outputs
    static unsigned long int gR_PresentValue_DI32[MAX_UART_NUM];   // 保存DI值  Unused digital input 0，1，2，Other digital inputs

    static short int  gR_Zero_Counter[MAX_UART_NUM];     // 接收数据为零值计算器
    //extern short int  gR_Updata_Counter[MAX_UART_NUM];    // 接收数据更新计算器
    //extern short int gR_PresentValue_Set1[MAX_UART_NUM];   // 保存设置值1  Set value 1

    static  unsigned char  g_Cmd_Flag[MAX_UART_NUM];         // 当前执行代码命令
    static  unsigned char  gW_WriteCmd_Flag[MAX_UART_NUM];   // 写温度/湿度控制值命令标志
    static  unsigned char  gW_ReadCmd_Flag[MAX_UART_NUM];    // 读温度/湿度控制值命令标志
    static  unsigned char  gW_StopCmd_Flag[MAX_UART_NUM];    // 停止温度/湿度控制值命令标志
    static  unsigned char  gW_RunCmd_Flag[MAX_UART_NUM];     // 运行温度/湿度控制值命令标志
    static  unsigned char  g_WriteCmd_Counter[MAX_UART_NUM]; // 写命令计数器
    static  unsigned char  g_ReadCmd_Counter[MAX_UART_NUM];  // 读命令计数器
    static  unsigned char  g_StopCmd_Counter[MAX_UART_NUM];  // 停止命令计数器
    static  unsigned char  g_RunCmd_Counter[MAX_UART_NUM];   // 运行命令计数器

    static  unsigned char  gR_ReadCmd_Flag[MAX_UART_NUM];    // 读温度/湿度命令标志  备份用于读取数据处理温度/湿度状态
    static  MsTime         gR_ReadCmd_Stamp[MAX_UART_NUM];   // 读温度/湿度命令更新时间

    static  unsigned char  gW_HumiFlag[MAX_UART_NUM];         //  用于标志是否已经设置湿度
    static  MsTime  gW_HumiInterval[MAX_UART_NUM];     //  用于设置湿度间隔时间

    static  MsTime   Wait_delay[MAX_UART_NUM];        // 写完命令延时
    static  unsigned char   uc_If_Wait_delay[MAX_UART_NUM];  // 写完命令延时标志
    static  unsigned char   uc_OffLine[MAX_UART_NUM][MAX_CHAMBER_NUM];
    //------------------------------
    //  << PLCS7-200温控器 >>  Begin
    static unsigned char uc_RunTH[MAX_UART_NUM];      // 运行温或湿模式
    static unsigned char uc_RunFrist[MAX_UART_NUM];   // 第一次运行温或湿模式
    static unsigned char uc_RunCount[MAX_UART_NUM];   // 第一次运行温或湿模式计算次数
    static unsigned char uc_ReadCount[MAX_UART_NUM];  // 温或湿模式读取时计算次数
    static unsigned char uc_LockModleT[MAX_UART_NUM];  // 锁定温度模式
    static short  gW_T_Back[MAX_UART_NUM];  // 写控制温度值SV  备份
    static short  gR_PV_Back[MAX_UART_NUM];  // 读温度值PV  备份
    //  << PLCS7-200温控器 >>  End
    //----------------------------------

    //     extern struct FUNC{
    //     void (* Function) (void);		//function 函数指针   //格式  void function(void);
    //     }st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

    //  End  TemperatureChamber


    //------------------------------
    //  << Voetsch温控器 >>  Begin
    static unsigned char gw_WarnLimit_Flag[MAX_UART_NUM];    // 写警告限制标置
    static unsigned char gR_Updata_Flag[MAX_UART_NUM];    // 接收数据更新标置
    static short int  gR_Updata_Counter[MAX_UART_NUM];    // 接收数据更新计算器
    //extern short int gR_PresentValue_Nominal2[MAX_UART_NUM];  // 保存名义值2避免更改  Nominal value of parameter 2
    //extern short int gR_PresentValue_Set1[MAX_UART_NUM][12];   // 保存设置值1  Set value 1
    static short int gR_PresentValue_Set1[MAX_UART_NUM];   // 保存设置值1  Set value 1
    static unsigned long int gR_PV_DO32[MAX_UART_NUM];   // 保存DO值  Unused digital output 0，1，2，Other digital outputs
    //  << Voetsch温控器 >>  End
    //----------------------------------


public:

    static void DoWork(void);
    static void MyInit(void);
    static void reset(unsigned char uc_ChNo,unsigned char t_controller_type);//DK
    static void DtReset(unsigned char uc_ChNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendtwentyoneCmd(unsigned char ChanNo);
    static void SendeightCmd(unsigned char ChanNo);
    static void SendtwentythreeCmd(unsigned char ChanNo);
    static void Clear_Counter(unsigned char ChanNo);



    static void SendCheckStatusCmd(unsigned char ChanNo);
    static void SendCheckSetpointCmd(unsigned char ChanNo);
    static bool IfCanSendReadCmd(unsigned char ChanNo);
    //static void SetPresentValue(unsigned char ChanNo, short int Value, unsigned char uc_Temp4Humi_Flag);
		static void SetPresentValue(unsigned char ChanNo, short int Value, unsigned char uc_Temp4Humi_Flag, unsigned char uc_Chamber);
    static void DoAllWriteCmdCtrl(unsigned char ChanNo);
    static void DoAllParseResultCtrl(unsigned char ChanNo);
    static void DoAllReadCmdCtrl(unsigned char ChanNo);
    static void SetPointValueCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag, unsigned short RunON);
    static void ExecStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    //short int ctrl_GetValueCmd(unsigned char ChanNo);
    //unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned int CheckSum(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned char CheckXorSum(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);

    //void ctrl_Value2String(short int * pvalue, unsigned char* Destn);
    //void ctrl_String2Value(unsigned char* string, short int* pValue);
    //bool ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len);
    static void Init_SetRun(void);
    static unsigned char Change_T_H(unsigned char ChanNo,unsigned char uc_CmdType);
    static unsigned char ExchangeCheckCounter(unsigned char Channel,unsigned char *CmdSource, unsigned char CmdDest,unsigned char * Counter);
    static unsigned char Get_SetPoinState(unsigned char ChanNo,unsigned char uc_CmdType);

    //unsigned char  CalcCheckSum_Byte(unsigned char *puchMsg, unsigned int usDataLen);
    static unsigned char*  String_MoveBuff(unsigned char* Source, unsigned char* Destn,unsigned int length);

    //short int Convert_FloatToInt(unsigned char* Source);
    //void Convert_IntToFloat(unsigned char* Source,short int w16_Value);
};


//函数指针
typedef void (*My_Func_Ptr_Void)(void);
typedef char (*My_Func_Ptr)(unsigned long  Value_A,unsigned long Value_B);
typedef char (*My_Func_Ptr_Ext)(unsigned long  Value_A,unsigned long  Value_B,unsigned long Value_C);
typedef void (*My_Func_Ptr_Ext1)(unsigned char Value_A,unsigned char Value_B);
typedef void (*My_Func_Ptr_Ext2)(unsigned char Value_A,unsigned short Value_B,unsigned short Value_C,unsigned char Value_D);
typedef float (*My_Func_ptr_Ext3)(unsigned char Value_A);
//调用的话
//pf();    具体可参考《C和指针》


#endif
