///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Safety.h
//Version:        V1.0
//Date:                2013/9/17
//Description:
//           脉冲安全检查任务，控制值异常只检查电流，除了Mv的电流电压之外还应和台阶
//			 数据m_Pulse_Stage_Data比较，任何一个Unsafe了就响应
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////

//#ifndef _PULSE_SAFETY_CHECK_H_
//#define _PULSE_SAFETY_CHECK_H_
#pragma once
#include "../../common_OS/protocols/SF_Hwf.h"
#include "../IV_Tasks/ScheduleDataDefine.h"


#define MAXUNSAFECHECKTYPE   3           //Unsafe 检查类型
#define PULSESAFETY_CHECK_PERIOD     100 // 10ms                 //超限时间    这里将来可以根据硬件来使用不同的时间

enum
{
    NONE,
    UNSAFE_COUNTER, 	//总数超标
    UNSAFE_CYCLE,		//循环数超标
    UNSAFE_STAGE,		//阶梯数超标
};

enum
{
    CURRENTSAFETY,
    VOLTAGESAFETY,
    POWERSAFETY,

};

typedef struct
{
    unsigned char BufCheckNum;             //检查的Buf值
    unsigned long UnsafeCount;              //异常值统计数量
    unsigned long CheckCount;               //检查数量
    unsigned char UnsafeStageCount;          //异常Stage统计数量
    unsigned char UnsafeCycleCount;        //异常Cycle统计数量
    unsigned long UnsafeCycleID;             //异常循环ID
    unsigned long UnsafeStageRecordCycleID;   //Stage异常时的循环ID
    unsigned char UnsafeCycleRefrush;       //循环刷新标记用于Stage异常判断
    unsigned char CheckStageID;            //检查到了第几个Stage
    unsigned char BufOffset;				//从哪个点开始查
    unsigned long RecordCount;      //普通检查统计数量，用于同脉冲缓冲检查区分；
} ST_PULSE_SAFETYCHECK;


class PULSE_safety_check
{
public:
    static UInt16               Priority;
    static UInt16               TaskID;			//  should be a definition which positioning this task in OS_Task[]


// 	static unsigned char        RedundantSafetyFlg[MAXCHANNELNO];

// 	static unsigned char        I_Sample_Flag[MAXCHANNELNO];
// 	static unsigned char        V_Sample_Flag[MAXCHANNELNO];
// 	static St_CtrlValueCheck    CtrlValueCheck[MAXCHANNELNO];
// 	static St_IrregularCheck    IrregularCheck[MAXCHANNELNO];


// 	static St_CtrlValueSetting  m_CtrlValueSetting;
// 	static St_IrregularSetting  m_IrregularSetting;


    //add for new pulse safe design;
    static unsigned char		Pulse_UnsafeType[MAXCHANNELNO];
    //static PULSE_RAW_DATA		*CheckPulseSafetyBuf1_ptr;
    //static PULSE_RAW_DATA		*CheckPulseSafetyBuf2_ptr;
    //static PULSE_RAW_DATA		*BeCheckedPulseSafetyBuf_ptr;

    static ST_PULSE_SAFETYCHECK 	Pulse_SafetyCheck[MAXCHANNELNO];

    static float UD_Record[MAXCHANNELNO];                           //170122
    static int UD_UnsafeIndex[MAXCHANNELNO];                      //170122

    static void Init(void);
    static void DoWork(void);
// 	static void EndTask(void);
// 	static MsTime TimeT(MsTime a,MsTime b);    // a<b

// 	static unsigned char getSafetyFlag(unsigned char chn);
    static void          InitCtrlValue(unsigned char ChannelIdx,unsigned char CheckType,unsigned char VoltRange,unsigned char CurrRange);
    static void          InitAbnormalCheck(unsigned char ChannelIdx);
protected:

private:
// 	static char PULSE_Safety_Check_status[MAXCHANNELNO];
    static unsigned long ErrorFlag[MAXCHANNELNO];    //safety标志    X X 第三方 SYS SCH P V I    1代表unsafe  0代表safe

// 	static unsigned char safety_chn;    //当前check的chn
// 	static unsigned char b_ClearError[MAXCHANNELNO];
    static MsTime safety_last_time[MAXCHANNELNO];
// 	static MsTime safety_error_time[MAXCHANNELNO];
    static MsTime safety_resume_time[MAXCHANNELNO];

    // 新定义成员变量
    static unsigned char Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.

    static float I_Record[MAXCHANNELNO];		// used for recording error values. each channel should has it's own buffer because
    static float V_Record[MAXCHANNELNO];		// - sometimes there's no new value for this channel, but there's a timeout  triggered
    static float P_Record[MAXCHANNELNO];		// - unsafeand if only one variable, it could be updated by other channels value already.
    static float C_Record[MAXCHANNELNO];
    static float t_Record[MAXCHANNELNO];
    // 在 safety_check::Init(void)中放入以上变量的初始化 = 0 即可


// 	static uint16 CheckAndSetCandidate(unsigned char chn);
    static void SetUnsafe(unsigned char chn, uint32 unsafeType, float value);
    static void inline ClrUnsafe(unsigned char chn, uint16 unsafeType);
// 	static void ClearSchUnsafeIfNotRunning(unsigned char chn);
    static void StatusProcess(unsigned char chn, uint16 checkItem);
    //add philip

    static char PULSE_safety_check_sch_is_ready(unsigned char chn);
    static void check(unsigned char chn, uint16 checkItem);
    static void sendError(unsigned char chn);    //向sch或其他发送警告信息，并停止通道
    //	static char resumeError(unsigned char chn);    //恢复sch

    //static char sendToEthernet(unsigned char chn,unsigned char safetyStatus);    //向上位机报告状态

//	static void clearPV(void);

    static void MakeIrregularCheck(void);
// 	static unsigned char CheckIrrationality(unsigned char ChannelIdx);
    static void MakeCtrlValueCheck(void);
    static unsigned char CheckCtrlValue(unsigned char ChannelIdx);
    static void NormalSafetyValueCheck(unsigned char chn,DWORD type,float value);
    static void BeNormalSafetyPIValueCheck(unsigned char chn,float current,float power);
};

//#endif
