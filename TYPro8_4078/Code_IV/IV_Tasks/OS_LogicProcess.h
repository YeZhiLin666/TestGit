#pragma once

#include "../Entry/G_DF.h"
#include "../../COMMON_OS/LPC4078CORE/RTC.h"

#define LOGICPROCESS_HANDLE_MAX         100    //最大保留32个
#define LOGICPROCESS_HANDLE_ID_MAX      65535

//时序处理状态
#define LOGICPROCESS_STATUS_HANDING    0
#define LOGICPROCESS_STATUS_FINISH         1
#define LOGICPROCESS_STATUS_ERROR        -1

#define DAC_OUT_PUT_ZERO 0x800000    //偏移码  0V
#define DAC_OUT_PUT_MAX  0xFFFFFF     //最大10V
#define DAC_OUT_PUT_MIN  0x000000    //最小-10V


//for DisposableFlag    when use openchn DisposableFlag = DISPOSABLE_OPENCHN
//comment out chen 20130708
enum
{
    DISPOSABLE_to_NONE = 0x01,    //没有时序
    DISPOSABLE_to_OPENCHN,
    DISPOSABLE_to_CLOSECHN,
    DISPOSABLE_to_CCHN,
    DISPOSABLE_to_RESE,    //预留
};

//for St_RealyOperation RelaySet
enum  //因为BT2208驱动还没有改造，所以，这里暂时还留着，以后会删除掉，何培青，20140522
{
    ACTIVE_CUR_RELAY_NONE = 0x0000,                                      //不操作relay
    ACTIVE_CUR_RELAY_SUPER_HIGH = 0x0001,                            //电流超高量程
    ACTIVE_CUR_RELAY_HIGH = 0x0002,                                       //电流高量程
    ACTIVE_CUR_RELAY_MIDDLE = 0x0004,                                   //电流中量程
    ACTIVE_CUR_RELAY_LOW = 0x0008,                                       //电流低量程
    ACTIVE_CUR_GAIN = 0x0010,                                                //电流数字量程
    ACTIVE_CUR_CUT_PWM = 0x0020,                                         //拉栅压
    ACTIVE_CUR_RST_CUTOFF = 0x0040,                                     //恢复栅压
    ACTIVE_CHAGE = 0x0080,                                                     //充放电极性控制
    ACITVE_RESERV1 = 0x0100,                                                  //预留1
    ACTIVE_U_P_CHARGE = 0x0200,                                            //unipolar充电
    ACTIVE_U_P_DISCH = 0x0400,                                               //unipolar放电
    ACTIVE_VOL_RANGE = 0x0800,                                             //电压量程切换--------------自动校准继电器
    ACTIVE_V_I_R = 0x1000,                                                       //电流端电压测量
    ACTIVE_RESERV2 = 0x2000,                                                  //预留2
    ACTIVE_P_R = 0x4000,                                                          //保护PR继电器
    ACTIVE_OUT_RELAY = 0x8000,                                              //输出继电器
};

//for rangeset
enum
{
    ACTIVE_CLOSE_CURRENT_RELAY = 0x0001,    //关闭现有量程
    ACTIVE_OUT_NEXT_RELAY = 0x0002,          //打开新量程
    //RESERV
};

//for dac
enum
{
    ACTIVE_DAC_NONE =0x0000,                                              //没有操作
    ACTIVE_DAC_CUR = 0x0001,                                               //根据msg输出电流
    ACTIVE_DAC_VLT = 0x0002,                                                //根据msg输出电压
    ACTIVE_DAC_VH  = 0x0004,                                                //根据msg高钳压
    ACTIVE_DAC_VL  = 0x0008,                                                 //根据msg低钳压
    ACTIVE_DAC_CUR_ZERO = 0x0010,                                     //电流输出0
    ACTIVE_DAC_VLT_ZERO = 0x0020,                                      //电压输出0
    ACTIVE_DAC_VH_DEFAULT = 0x0040,                                  //高钳压拉到最大
    ACTIVE_DAC_VL_DEFAULT = 0x0080,                                   //低钳压拉倒最大
// 	ACTIVE_DAC_CUR_NEAR_ZERO =  0x0100,                                  //电流输出接近0，但偏大一些
//     ACTIVE_DAC_OFFSET_5PERCENT_ZERO = 0x200,
    //增加一个，输入DAC当前的ADC值  20131101  chen add
    ACTIVE_DAC_CUR_ADC = 0x2000,
};

//稳态瞬态判断，当瞬态的时候不进行safecheck
//comment out chen 20130724
enum
{
    LOGIC_PROCESS_STEADY = 0xB1,
    LOGIC_PROCESS_TRAN,
    LOGIC_PROCESS_CHN_CLOSE,
    LOGIC_PROCESS_CHN_OPEN,
};

enum
{
    LOGIC_NOTREADY=0x00,
    LOGIC_IDLE,
    LOGIC_WAIT,
    LOGIC_TRIGGER,
    LOGIC_END,
};
enum
{
    DO_NA=0x00,
    DO_STOP,
    DO_RUN,
};
typedef struct
{
    unsigned short ID;    //0-65535个
    unsigned char status;         //状态  8 bit  代表含义
} LogicProcessHandle;

typedef CIRCULAR_QUEUE(LogicProcessHandle,LOGICPROCESS_HANDLE_MAX)    qDisposTaskBuf;

enum
{
    DISPOSABLE_ASSIGNED = 0x01,    //已经交付，待运行
    DISPOSABLE_RUNNING,           //运行中
    DISPOSABLE_FINISHED,                //结束
    DISPOSABLE_ERROR,               //出错
    DISPOSABLE_DEAD,                 //历史记录已经消亡
};

typedef struct
{
    unsigned short RelaySet; 			 //操作数

    unsigned short ActiveFlag;     //16位操作描述符    每位代表一个动作    最多支持16种操作
    unsigned short InactiveFlag;
} St_RealyOperation;

typedef struct
{
    unsigned short DacSet;  //DAC操作数
    unsigned short DacFlag; //16位操作描述符    每位代表一个DAC 最多支持16种操作
} St_DacOperation;

enum
{
    eTransitionType_NULL = 0,
    eTransitionType_RstToCur,
    eTransitionType_RstToVlt,
    eTransitionType_RstToOff,
    eTransitionType_OffToRst,
    eTransitionType_OffToCur,
    eTransitionType_OffToVlt,
    eTransitionType_CurToCur,
    eTransitionType_CurToVlt,
    eTransitionType_CurToOff,
    eTransitionType_CurToRst,
    eTransitionType_VltToVlt,
    eTransitionType_VltToCur,
    eTransitionType_VltToOff,
    eTransitionType_VltToRst,
};

typedef struct
{
    unsigned long uw32_DelayTime;    //时间点 100us为单位
    St_RealyOperation RelayMove;     //继电器操作
    St_DacOperation   DacMove;       //DAC操作
    unsigned char RangeChangedFlag;    //是否考虑切换量程    0不需要    1需要

    unsigned char bPWM;
    unsigned char PWM_TransitionType;
} St_DisposableTask;
///////////////////for 一次性时序数组///////////////////

typedef struct
{
    St_DisposableTask * st_Disposabletask_ptr;    //指向步骤结构指针
    unsigned short   stepLen;    //步数
    //unsigned char prio;    //优先级处理    comment out chen 20130702  只有2个优先级，最高和最低，只有关通道是最高，其他时序都为最高
} St_Disposable;
///////////////////for 一次性时序共用部分///////////////////

//一次性任务MSG
typedef struct
{
    unsigned char  CH_Number;       //通道号
    unsigned char  msgID;//判断是从哪发来的msg
    unsigned short RangeSet;//区别不同Range
    unsigned char  last_Range;    //做保留
    unsigned char  next_Range;
    unsigned char  last_VRange;
    unsigned char  next_VRange;

    St_Disposable   Disposable_task;    //msg内的副本
    unsigned char   Disposable_Flag;    //具体时序的标志    comment out chen 20130708
	unsigned char   TargetCtrlType;    // 目标控制类型    20200810 yy
	
    //DAC部分预留
    unsigned long Dac_Value;       //DAC值
    unsigned long Volt_DacValue;
    unsigned long Clamp_Direction; //钳位方向
    //上位机只发上钳和下钳，下位机程序要根据不同的硬件写不同的设置。如仅有上的只用一个就行。
    //如果是双钳位，那么规定Clamp_Value为CLAMPVH，Clamp_Value_2为CLAMPVL
    unsigned long Clamp_Value;     //钳位值
    unsigned long Clamp_Value_2;    //

    unsigned char AutoCaliFlag;    //自动校准预留    1自动校准    0普通操作
    //comment out chen

    //ADC部分预留
    unsigned char ADC;

    //执行所需    这部分本想考虑放到chn info里，但是每个结构增加如此的数据没有必要，决定放到全局msg里就可以，但是从效果上不如放到
    //chn info有层次感
    //直接赋0
    //comment out chen
    volatile unsigned char state;     //状态
    unsigned char counter;  //计数

    MsTime MsTimeDisposableTask;
    MsTime SysTimeDisposableTask;
} St_DisposableMSG;
///////////////////for 一次性时序msg///////////////////

typedef struct
{
    St_DisposableMSG logicProcessMSg;
    MsTime           StartTime;
    unsigned long    DelayTime;
    unsigned char    bActionFinished;
    unsigned short   ID;
    unsigned short   NextID;
} St_Disposable_G_Que;   //任务请求队列


//for 排队机制
//comment out chen 20130708
typedef struct
{
    unsigned char cur_Disposable;    //当前时序
    unsigned char prio_isUsed;         //排队使用
} St_Disposable_Prio;   //排队优先级

//extern const unsigned char g_chn_order[MAXCHANNELNO];
// extern unsigned char g_chn_order[MAXCHANNELNO];

class OS_LogicProcess
{
public:
    static St_Disposable          SetCtrlRstToCur;
    static St_Disposable          SetCtrlRstToVlt;
    static St_Disposable          SetCtrlRstToOff;
    static St_Disposable          SetCtrlOffToRst;
    static St_Disposable          SetCtrlOffToCur;
    static St_Disposable          SetCtrlOffToVlt;
    static St_Disposable          SetCtrlCurToCur;
    static St_Disposable          SetCtrlCurToVlt;
    static St_Disposable          SetCtrlCurToOff;
    static St_Disposable          SetCtrlCurToRst;
    static St_Disposable          SetCtrlVltToVlt;
    static St_Disposable          SetCtrlVltToCur;
    static St_Disposable          SetCtrlVltToOff;
    static St_Disposable          SetCtrlVltToRst;
    static St_Disposable          SetCtrlVltToExt;
    static St_Disposable          SetCtrlExtToVlt;
    static St_Disposable          SetAutoCaliOpen;    //自动校准开
    static St_Disposable          SetAutoCaliClose;   //自动校准关     //本通道作为IV通道，处于自动校准流程中
    static St_Disposable 					SetCtrlAcimOn;		    //ACIM开
    static St_Disposable 					SetCtrlAcimOff;   		//ACIM关

    static St_Disposable          SetCaliBoardRstToVlt;
    static St_Disposable          SetCaliBoardVltToOff;
    static St_Disposable          SetCaliBoardVltToVlt;                //本通道作为CALI通道，不处于自动校准流程中
	static St_Disposable          SetAutoRangeSwitch;

    static St_Disposable		      SetVoltRelay;
    static UInt16                 Priority;//commnet out chen 1121
    static UInt16                 TaskID;
//     static unsigned long int      Set_DAC_Output_MIN; // 设置一个微值ISET提升过死区
    static volatile unsigned char Logic_For_Safe[MAXCHANNELNO];    //转换状态    瞬态  稳态
    static St_Disposable_G_Que    m_RunningTask[MAXCHANNELNO]; // Ron Modified 081415
    static unsigned char 			LogicStatus[MAXCHANNELNO];
    static MsTime 					Logic_last_time[MAXCHANNELNO];
    static unsigned char 			LogicCtrlType;
    static bool 					be_NeedWaitflg;
    static bool						m_bSlaveActive[MAXCHANNELNO];
		static bool 					be_StartThirdParty;
private:
    //static St_Disposable_G_Que    m_RunningTask[MAXCHANNELNO]; //Ron modified 081415
    static unsigned short         handle_index;    //索引  1-65535
    static qDisposTaskBuf         m_qHandle;
    static unsigned char          m_bActive[MAXCHANNELNO];
    static unsigned char          m_bQueueBusy[MAXCHANNELNO];

public:
    static void                   Init(void);
    static void                   DoWork(void);
    static unsigned short         requestLogicPorcess(unsigned char ChannelIdx,St_DisposableMSG * st_oneTime_msg);
    static unsigned char          getHandleStatus(unsigned short HandleID);
// 	static unsigned long          CalibrateDac(float fdac);
    static void										InitBoardEngine(St_DisposableTask* open,
            unsigned char open_len,
            St_DisposableTask* close,
            unsigned char close_len,
            St_DisposableTask* cc2cc,
            unsigned char cc2cc_len,
            St_DisposableTask* rst2off,
            unsigned char rst2off_len);

private:
    static int                    ProcessActive(unsigned char ChannelIdx);
    static signed int             processDisposable(unsigned char ChannelIdx);   //OpenChn or Close Chn or others
    static void                   DumpUnnessaryMove(unsigned char ChannelIdx);//判断执行的动作，是一个跳跃过程
    static void                   DoMove(unsigned char ChannelIdx);//做Relay,DAC动作
    static void                   DoDAC(unsigned char ChannelIdx);
    static void                   setStatus(unsigned short HandleID,unsigned char state);
    static unsigned char 		  FindNextSubChannel(unsigned char GorupID);
};


