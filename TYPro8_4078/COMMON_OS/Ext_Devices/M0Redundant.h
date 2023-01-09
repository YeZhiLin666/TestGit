#pragma once

#define MAX_REDUNDANT_TYPE 2

//// 命令格式：开始与结束符
//Start    '$'                     ASCII code (0X24)
//End     <CR> Carriage Return     ASCII code (0X0D)
//// 命令格式：开始与结束符

#define COMMAND_START_CHARACTER     0X24
#define COMMAND_END_CHARACTER       0X0D


#define M0_ADC_Gain 1.03356f
#define M0_ADC_Offset -0.05717f


typedef struct
{
    unsigned char m_Type;
    float			  m_fAdcGain;
    float         m_fAdcOffset;
    unsigned char m_bValid[MAXCHANNELNO];
    unsigned char m_ChanNo[MAXCHANNELNO];
    float         m_fSafetyH[MAXCHANNELNO];
    float         m_fSafetyL[MAXCHANNELNO];
} St_RedundantChnMap;

class M0RedundantFunc
{
public:
    static const unsigned char m_M0MapTable[MAXCHANNELNO];
    static const unsigned char m_M0MapChnValid[MAXCHANNELNO];
    static float  m_SampleData[MAXCHANNELNO];
    static St_RangeCali m_M0_CalibPara[MAXCHANNELNO];
    static St_RedundantChnMap  M0_RedundantMap;

// 	static void DoSafetyCheck(void);
// 	static void SetAdcGainOffset(unsigned char Type);
    static void SetRedundantSafety(unsigned char uc_IVChnNo,unsigned char uc_Type);
    static void SetM0ConfigInfo(void);
    static void SetM0CalibPara(unsigned char uc_ChnNo,unsigned char uc_MaxChnNo,St_RangeCali m_para);
    static void SetM0DataRefresh(unsigned char uc_ChnNo,unsigned char uc_SwitchOn);
    static void ReleaseSafetyCheck(unsigned char uc_IVChnNo,unsigned char uc_Type);
    static short getThisTypeIdx(unsigned char Type);
    static void GetM0SafetyInfo(unsigned char uc_IVChnNo,unsigned char uc_Type,St_SaftyCheckItem * compactSafetyInfo);
    //attribute
    static bool                 m_bClampValid[MAXCHANNELNO];
    static bool                 m_bIsM0ReleaseCheck[MAXCHANNELNO];
    static St_RedundantChnMap   m_RedundantChnMap[MAX_REDUNDANT_TYPE];
    static St_UartCmdBuf        SafetyInfo;
    static float m_fM0Value;
private:

};

