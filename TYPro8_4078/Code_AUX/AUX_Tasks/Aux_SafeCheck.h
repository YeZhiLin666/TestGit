//改名为SAFE_CHECK
//安全检查

#ifndef _AUX_SAFETY_CHECK_H_
#define _AUX_SAFETY_CHECK_H_

#include "../../common_os/protocols/SF_Hwf.h"
#include "../../common_OS/protocols/Uart_Hwf.h"


#define SAFETY_CHECK_PERIOD     1000 // 100ms                 //超限时间    这里将来可以根据硬件来使用不同的时间
#define SAFETY_CHECK_RESUME     10000 //1000ms  恢复safe检测时间


#define AUX_NO_CHECK		0
#define AUX_V_CHECK		1
#define AUX_T_CHECK			2


// following defines the STATUS for StatusProcess() Status Machines.
#define SAFE		0
#define SAFE2UNSAFE	1
#define UNSAFE		2
#define UNSAFE2SAFE	4


///////
#define AUX_T_SAFE    1
#define AUX_V_SAFE    2

#define M4_MAX_CHANNELNO 16
#define M4_MAX_ADCCHANNELNO  16

typedef enum
{
    en_Safe = 0,
    en_Unsafe = 1,
} en_SafetyStaus;

enum AUX_ONLINE_RSM_STATE
{
    NoUse,
    Monitoring,
    Protect,
};
typedef struct
{
    unsigned long ErrorFlag;
    unsigned char m_state;

    float f_record;     //记录的错误
} ST_AUX_SAFETY_CHECK;

typedef struct
{
    unsigned long long NotSafeFlag[2];  //用于标志每种辅助最多128个通道的超限情况
    unsigned char m_state[MAX_AUXCH_PER_AUXMCU];    //128通道的状态

    float f_record[MAX_AUXCH_PER_AUXMCU];     //unsafe时候的值
} ST_AUX_SAFETY_RECORD;
//我在想，预留128个是否过大
typedef struct
{
    float f_SafetyVoltage[8];
    float f_SafetyCurrent[8];
    float f_SafetyTemperature[8]; //新增三组辅助安全电压，电流，温度值
} ST_OnlineRSM_Safety_RECORD;

//RedundantVoltage:
typedef struct
{
    unsigned char m_bValid[M4_MAX_ADCCHANNELNO];
    unsigned char m_IVChnNo[M4_MAX_ADCCHANNELNO];

} St_IV_M4ADCChnMap;
typedef struct
{
    unsigned char  m_MaxChnCount;
    unsigned char  m_M4PhyChn;
    unsigned char  m_M4Type;
    unsigned char  m_UnsafeCount;
    unsigned char  m_ReportCnt;
    bool           m_SafetyValid;
    bool           m_bReported;
    bool           m_bReleaseCheck;
    float  			m_SafetyH;
    float  			m_SafetyL;
    MsTime  m_waitReleaseTick;
    en_SafetyStaus m_safeStatus;
} St_M4SafetyCeck;

typedef struct
{
    unsigned char  	m_MaxChnCount;
    unsigned char  	m_M4PhyChn;
    unsigned char  	m_M4Type;
    unsigned char  	m_UnsafeCount;
    unsigned char  	m_ReportCnt;
    bool           	m_SafetyValid;
    bool           	m_bReported;
    bool           	m_bReleaseCheck;
    float  					m_SafetyH;
    float  					m_SafetyL;
    MsTime  				m_waitReleaseTick;
    en_SafetyStaus 	m_safeStatus;
} St_SafetyCeck;


class Aux_Safety_Check
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
    static unsigned char  EmergencyStopCheckEnable;

    static unsigned char sampleFlag[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];                                 //采样标志
    static ST_AUX_SAFETY_CHECK st_aux_safety[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];    //[MAX_AUX_TYPE];
    static ST_AUX_SAFETY_RECORD st_check_result[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];

    static MsTime safety_last_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];//[16];
    static MsTime safety_error_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];//[16];
    static MsTime safety_resume_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];//[16];

    //function
    static void Init(void);
    static void DoWork(void);
    static void EndTask(void);

    static MsTime TimeT(MsTime a,MsTime b);

    //RedundantVoltage:
    static St_IV_M4ADCChnMap st_IV2M4ADCChnMap;
    static St_SafetyCeck st_SafetyCheck[M4_MAX_CHANNELNO];
public:

    static void CleanSafetyCheckResults( unsigned char uc_IVunitBufIndex, unsigned char uc_IVch);
    //RedundantVoltage:
    static bool SetSafetyInfo(const St_Uart_SetSafety *safetyInfo);
    static unsigned char MonitorSafetyLedState[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
protected:
private:
    static unsigned char safety_id  [MAX_IVMCU_PER_SYS];
    static unsigned char safety_chn [MAX_IVMCU_PER_SYS];
    static MsTime       LED_ProtectTime;
    static unsigned char Led_State[8];
    static ST_OnlineRSM_Safety_RECORD  st_OnlineRSM_UnsafeData[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];


    static unsigned char CheckAndSetCandidate(unsigned char uc_IVch);
    static unsigned char check(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,uint16 checkItem);

    static unsigned char statusProcess(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch);
    static void OnlineRSMSafetyCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch);
    static void OnlineRSMStateScan(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch);

    //报错和恢复
    static unsigned char sendError( unsigned char uc_IVunitBufIndex,unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_Auxch);
    static unsigned char resumeError(unsigned char uc_IVunitBufIndex,unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_Auxch);    //可以说是预留
};

#endif
