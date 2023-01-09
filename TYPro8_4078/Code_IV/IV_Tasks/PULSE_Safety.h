///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Safety.h
//Version:        V1.0
//Date:                2013/9/17
//Description:
//           ���尲ȫ������񣬿���ֵ�쳣ֻ������������Mv�ĵ�����ѹ֮�⻹Ӧ��̨��
//			 ����m_Pulse_Stage_Data�Ƚϣ��κ�һ��Unsafe�˾���Ӧ
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////

//#ifndef _PULSE_SAFETY_CHECK_H_
//#define _PULSE_SAFETY_CHECK_H_
#pragma once
#include "../../common_OS/protocols/SF_Hwf.h"
#include "../IV_Tasks/ScheduleDataDefine.h"


#define MAXUNSAFECHECKTYPE   3           //Unsafe �������
#define PULSESAFETY_CHECK_PERIOD     100 // 10ms                 //����ʱ��    ���ｫ�����Ը���Ӳ����ʹ�ò�ͬ��ʱ��

enum
{
    NONE,
    UNSAFE_COUNTER, 	//��������
    UNSAFE_CYCLE,		//ѭ��������
    UNSAFE_STAGE,		//����������
};

enum
{
    CURRENTSAFETY,
    VOLTAGESAFETY,
    POWERSAFETY,

};

typedef struct
{
    unsigned char BufCheckNum;             //����Bufֵ
    unsigned long UnsafeCount;              //�쳣ֵͳ������
    unsigned long CheckCount;               //�������
    unsigned char UnsafeStageCount;          //�쳣Stageͳ������
    unsigned char UnsafeCycleCount;        //�쳣Cycleͳ������
    unsigned long UnsafeCycleID;             //�쳣ѭ��ID
    unsigned long UnsafeStageRecordCycleID;   //Stage�쳣ʱ��ѭ��ID
    unsigned char UnsafeCycleRefrush;       //ѭ��ˢ�±������Stage�쳣�ж�
    unsigned char CheckStageID;            //��鵽�˵ڼ���Stage
    unsigned char BufOffset;				//���ĸ��㿪ʼ��
    unsigned long RecordCount;      //��ͨ���ͳ������������ͬ���建�������֣�
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
    static unsigned long ErrorFlag[MAXCHANNELNO];    //safety��־    X X ������ SYS SCH P V I    1����unsafe  0����safe

// 	static unsigned char safety_chn;    //��ǰcheck��chn
// 	static unsigned char b_ClearError[MAXCHANNELNO];
    static MsTime safety_last_time[MAXCHANNELNO];
// 	static MsTime safety_error_time[MAXCHANNELNO];
    static MsTime safety_resume_time[MAXCHANNELNO];

    // �¶����Ա����
    static unsigned char Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.

    static float I_Record[MAXCHANNELNO];		// used for recording error values. each channel should has it's own buffer because
    static float V_Record[MAXCHANNELNO];		// - sometimes there's no new value for this channel, but there's a timeout  triggered
    static float P_Record[MAXCHANNELNO];		// - unsafeand if only one variable, it could be updated by other channels value already.
    static float C_Record[MAXCHANNELNO];
    static float t_Record[MAXCHANNELNO];
    // �� safety_check::Init(void)�з������ϱ����ĳ�ʼ�� = 0 ����


// 	static uint16 CheckAndSetCandidate(unsigned char chn);
    static void SetUnsafe(unsigned char chn, uint32 unsafeType, float value);
    static void inline ClrUnsafe(unsigned char chn, uint16 unsafeType);
// 	static void ClearSchUnsafeIfNotRunning(unsigned char chn);
    static void StatusProcess(unsigned char chn, uint16 checkItem);
    //add philip

    static char PULSE_safety_check_sch_is_ready(unsigned char chn);
    static void check(unsigned char chn, uint16 checkItem);
    static void sendError(unsigned char chn);    //��sch���������;�����Ϣ����ֹͣͨ��
    //	static char resumeError(unsigned char chn);    //�ָ�sch

    //static char sendToEthernet(unsigned char chn,unsigned char safetyStatus);    //����λ������״̬

//	static void clearPV(void);

    static void MakeIrregularCheck(void);
// 	static unsigned char CheckIrrationality(unsigned char ChannelIdx);
    static void MakeCtrlValueCheck(void);
    static unsigned char CheckCtrlValue(unsigned char ChannelIdx);
    static void NormalSafetyValueCheck(unsigned char chn,DWORD type,float value);
    static void BeNormalSafetyPIValueCheck(unsigned char chn,float current,float power);
};

//#endif
