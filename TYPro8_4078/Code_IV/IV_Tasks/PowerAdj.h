///////////////////////////////////////////////////////////////////////////////////
//File Name: PowerAdj.h
//Version:		V1.0
//Date:				2018/02/09
//Description:
//						PowerAdj功能定义
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"


#define     AD_OVERTIME                 20*10000
#define     Unsafe_Overtime             16*10000



enum
{
    POWERADJ_IDLE=1,      			//空闲
	POWERADJ_WAIT,
// 	POWERADJ_START,		//发送启动信息到M0
// 	POWERADJ_RUNNING,
    POWERADJ_REQUIRE,
    POWERADJ_WAIT_REQUIREACK,
    POWERADJ_RUNNING,
// 	POWERADJ_WAIT_RefreshACK1,
// 	POWERADJ_WAIT_RefreshACK2,
// 	POWERADJ_WAIT_RefreshACK3,
// 	POWERADJ_WAIT_RefreshACK4,
// 	POWERADJ_SENDSTOPTOM0_1,		//发送停止信息到M0
// 	POWERADJ_WAIT_STOPACK1,
// 	POWERADJ_WAIT_STOPACK2,
// 	POWERADJ_WAIT_STOPACK3,		 	//等待M0应答
// 	POWERADJ_WAIT_STOPACK4,
// 	POWERADJ_WAIT_STOP,
// 	POWERADJ_ERROR,					//异常处理
    POWERADJ_Feedback_ERROR,
    POWERADJ_COMMUNATION_ERROR,
    POWERADJ_FIXUNSAFE,
    POWERADJ_GOTOThirdParty,
    POWERADJ_WaitForDefault,
    POWERADJ_OnThirdParty,
};
enum
{
    CAN_WATCHDOG_IDLE,
    CAN_WATCHDOG_WATIFORSENDACK,
    CAN_WATCHDOG_SENDTOM0,
    CAN_WATCHDOG_WAITM0ACK,
    CAN_WATCHDOG_ACKFAILD,
};
typedef struct
{
    float                 m_Vpp;            //正向电压
    float                 m_Vmm;           //负向电压
} St_PowerData;

typedef struct
{
    bool IsResistorFlag;	//是否使用电阻负载
    float m_ResistorValue;	//电阻值
} St_LoadTypePowerAdj;

typedef struct
{

    float                 m_Vm;      //正向电压的档位电压
    float                 m_Vn;      //负向电压的档位电压
} St_PowerLevelData;
typedef struct
{
    bool     m_unsafeFlag;      // 0：反馈是安全的    1：反馈是不安全的
    MsTime   m_UpdataTime;
    MsTime   m_UnsafeTime;
// 	unsigned char m_unsafeFlag;
} St_FeedBackCheckData;

class PowerAdj
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级
// 	  static bool  PowerAdjWorkFlag;      //功率电源处于调节状态。   0：功率电源没有在调节，   1： 功率电源需要调节。
    static bool Calib_PowerParaFlag[MAXCHANNELNO];
// 	  static bool SchRunningFlag[MAXCHANNELNO]; //Schedule 运行标志
    static unsigned char m_PowerAdjStatus;
// 	  static bool b_M0ADCValueUnsafeFlag;   //false: 安全电压范围     true: 不安全的电压范围
    static St_PowerData m_PowerValue;
	  static St_PowerData m_M0Feedback_Value[4];
// 	  static UInt16  Time20msCnt;   //用于记录20ms 刷新一次电源电压控制值。
// 	  static uint32  Time50usCnt;    //用于定时计数
// 	  static bool    TimeStartFlag; //用于标识启动50us计时
// 	  static bool    Time20msStartFlag;    //用于启动20ms重发机制的定时标志
// 	  static unsigned char Receive_ACK_Status;    //00: 没有收到 ACK，  01：正确收到刷新反馈ACK,  02:反馈电压不符合要求，下位机要停机报警
// 	  static unsigned char PackLostCnt[4];     //目前最多支持4个M0
// 	  static bool           Receive_M0_StartedACKFlag;
// 	  static bool  Receive_M0_IDACKFlag[4];
    static St_LoadTypePowerAdj SchLoadPara[MAXCHANNELNO]; //Schedule 电阻负载参数
	  static St_PowerLevelData   m_PowerLevelValue;
	  static  unsigned char uc_M0ADData_UpdataFlag;      //收到数据置1 ， 使用后清零
	  static  unsigned char uc_DoCheckFlag;
	  static St_FeedBackCheckData   m_FeedBackData;
	  static  unsigned char uc_DefaultVoltageFlag;    //该标志置位时，表示需要电源设置的默认最大。
	  static unsigned long  WaitCnt;
// 	  static unsigned char  ReceiveSN;
	private:
// 		static unsigned long WaitACKCnt; 
// 	  static unsigned char uc_SendComCnt;
// 	  static unsigned char m_Watchdog_SN;
// 	  static unsigned char m_CanWatchdogStatus;
// 	  static MsTime   m_AckWaitTime;
// 	  static unsigned char ReSendCount;
	public:
		static void        DoWork(void);                                                                 //任务执行函数
		static void        MyInit(void);                                                                 //任务初始化
		static void        DoVoltageProcess(void);
		static void        CheckFeedBack(void);
	  static void CalPowerSetValue(void);
	private:
// 		static void 			 						SendMsgToM0(MP_CAN_MSG  m_Msg);
// 		static unsigned short 			 	CrcCheck(unsigned char *address,unsigned char len);
	  
	  static  void  CalPowerUpdataValue(void);
	  static void ProcessWatchDog(void);
	  static void PowerSetDefaultValue(void);
// 	  static int	CheckACKSN(void);
};

