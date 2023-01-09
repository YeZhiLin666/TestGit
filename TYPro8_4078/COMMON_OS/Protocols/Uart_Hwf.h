#pragma once

#define  REDUNDANT_TYPE   2
#define M0_MAX_CHANNELNO  16

#define M4_MAX_CHANNELNO 16

#define UART_CMD_LENGTH_POSITION  2
#define UART_CMD_HEAD_POSITION    1

#define MAX_UART_RX_LENGTH    24 // 64 


//// 命令格式：开始与结束符
//Start    '$'                     ASCII code (0X24)
//End     <CR> Carriage Return     ASCII code (0X0D)
//// 命令格式：开始与结束符
#define UART_CMD_START   0X24    //  ASCII '$'
#define UART_CMD_END     0X0D    //  ASCII <CR>




enum  en_UART_CMD
{
    //M4-> M0
    CMD_UART_SET_SAFETY = 0x50,
    CMD_UART_SET_GAIN_OFFSET = 0x51,
    CMD_UART_SET_MAX_CHN_NO = 0x52,
    CMD_UART_INIT_M0 = 0x53,//repetition
    CMD_UART_SetData_Switch=0x54,
    CMD_ACK = 0x55,
    //M0 -> M4
    CMD_UART_REPORT_UNSAFE = 0xA0,
    CMD_UART_REPORT_DATA  = 0xA1,
    CMD_UART_REPORT_CalibPara = 0xA2,
};


enum en_REDUNDANT_TYPE
{
    eType_Voltage,
    eType_Temp,
    eType_MAX,
};
typedef union
{
    float 		  m_fData;
    int   		  m_IntData;
    unsigned char m_ucData[4];
} Un_4Byte;

typedef struct
{
    unsigned char chn0:1;
    unsigned char chn1:1;
    unsigned char chn2:1;
    unsigned char chn3:1;
    unsigned char chn4:1;
    unsigned char chn5:1;
    unsigned char chn6:1;
    unsigned char chn7:1;
    unsigned char chn8:1;
    unsigned char chn9:1;
    unsigned char chn10:1;
    unsigned char chn11:1;
    unsigned char chn12:1;
    unsigned char chn13:1;
    unsigned char chn14:1;
    unsigned char chn15:1;
    unsigned char chn16:1;
    unsigned char chn17:1;
    unsigned char chn18:1;
    unsigned char chn19:1;
    unsigned char chn20:1;
    unsigned char chn21:1;
    unsigned char chn22:1;
    unsigned char chn23:1;
    unsigned char chn24:1;
    unsigned char chn25:1;
    unsigned char chn26:1;
    unsigned char chn27:1;
    unsigned char chn28:1;
    unsigned char chn29:1;
    unsigned char chn30:1;
    unsigned char chn31:1;
} St_Redundant_Meas_Bit_Mark;



typedef struct
{
    unsigned char  m_CmdStart;
    unsigned char  m_CmdHead;
    unsigned char  m_Length;
    unsigned char  m_IVChnNo;
    unsigned char  m_M4ADCChnIdx;
    unsigned char   m_btNone_11[3];   // 预留reserved
    float 		   m_SafetyH;
    float		   m_SafetyL;
    unsigned char  m_redundantType;
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;      // 结构：20字节
} St_Uart_SetSafety;

typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;
    unsigned char   m_MaxChnNo;
    unsigned char   m_ucNone[3];
    float   		m_fGain;
    float 			m_fOffset;
    unsigned short  m_CheckSum;
    unsigned char   m_CaliType;   //
    unsigned char   m_CmdEnd;      // 结构：16字节
} St_Uart_SetGainOffset;
typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_MaxChnNo;
    unsigned short  m_CheckSum;
    unsigned char   m_btNone_11;    // 预留reserved
    unsigned char   m_CmdEnd;       // 结构：8字节
} St_Uart_SetChannelMap;

typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;
    unsigned char   m_ONOROFF;    //0x01: turn on,  0x00: turn  off
    unsigned short  m_btNone_11;    // 预留reserved
    unsigned char   m_CmdEnd;       // 结构：8字节
} St_Uart_SetDataSwitch;
typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;
    float           m_UnsafeValue;
    unsigned char   m_UnsafeType;
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;       // 结构：12字节
} St_Uart_ReportUnsafe;

typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_MaxChnNo;          // 最大通道数
    unsigned char   m_ChnOrderFlag;     //00: 与IV通道号顺序相同  01:与IV通道号顺序相反
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;       //结构:8字节
} St_Uart_SetBoardInfo;
typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;       // 获取的通道号
    float           m_Voltage;     //AD 电压数据
    unsigned char   m_None;      //预留
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;       //结构:12字节
} St_Uart_GetADCVoltage;
typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;          // 最大通道数
    St_RangeCali    m_st_ADCPara;
    unsigned char   m_None;      //预留
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;       //结构:16字节
} St_Uart_ReportCalibPara;
typedef struct
{
    unsigned char   m_CmdStart;
    unsigned char   m_CmdHead;
    unsigned char   m_Length;
    unsigned char   m_ChnNo;          // 最大通道数
    unsigned char   m_FeedBackCmd;    // 回复的CMD
    unsigned short  m_CheckSum;
    unsigned char   m_CmdEnd;       //结构:16字节
} St_Uart_ReportAck;
typedef union
{
    St_Uart_SetSafety      m_Uart_SetSafety;
    St_Uart_SetGainOffset  m_Uart_SetGainOffset;
    St_Uart_SetChannelMap  m_Uart_SetChannelMap;
    St_Uart_ReportUnsafe   m_Uart_ReportUnsafe;
    St_Uart_SetBoardInfo   m_Uart_SetBoardInfo;
    St_Uart_GetADCVoltage  m_Uart_GetVoltage;
    St_Uart_SetDataSwitch  m_Uart_SetDataSwitch;
    St_Uart_ReportCalibPara  m_Uart_ReportCalibPara;
    St_Uart_ReportAck      m_Uart_ReportAck;
} Un_Uart_CMDGroup;

//#define FILL_SIZE (MAX_UART_RX_LENGTH-sizeof(Un_Uart_CMDGroup))   // 联合体计算长度时,以8的倍数处理
typedef struct
{
    int                UnAckCounter; //不回复次数  180906
    Un_Uart_CMDGroup   m_UartCmdGroup;
    //unsigned char      m_FillBuf[FILL_SIZE];
} St_UartCmdBuf;


