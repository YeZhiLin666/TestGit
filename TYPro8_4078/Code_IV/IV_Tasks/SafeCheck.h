//改名为SAFE_CHECK
//安全检查

#ifndef _SAFETY_CHECK_H_
#define _SAFETY_CHECK_H_

#include "../IV_Tasks/ScheduleDataDefine.h"

enum
{
    CHECK_SYS_V = 0,
    CHECK_SYS_I,
    CHECK_SYS_P,
    CHECK_SCH_V,
    CHECK_SCH_I,
    CHECK_SCH_P,
    CHECK_SCH_C,
    CHECK_SCH_t,
};
///////////////////////////////////////////////////

#define          FanSafe       0
#define          FanUnsafe     1
class safety_check
{
public:
    static UInt16               Priority;
    static UInt16               TaskID;			//  should be a definition which positioning this task in OS_Task[]
    static St_Safty             Check_S[MAXCHANNELNO];
    static unsigned char        RedundantSafetyFlg[MAXCHANNELNO];
    static unsigned char        Fan_FaultCheckFlag;    //add by qjm 2015 1.26 风扇运转故障检测
    static unsigned char        Temperature_FaultCheckFlag;
    static unsigned char        I_Sample_Flag[MAXCHANNELNO];
    static unsigned char        V_Sample_Flag[MAXCHANNELNO];
    static St_CtrlValueCheck    CtrlValueCheck[MAXCHANNELNO];
    static St_IrregularCheck    IrregularCheck[MAXCHANNELNO];
    static unsigned char        MTCTempSafetyGoto[MAXCHANNELNO];
    static St_BuiltinSafety    BuiltinAux_Check[MAXCHANNELNO]; //add by qjm 20180307
    static St_CtrlValueSetting  m_CtrlValueSetting[MAXCHANNELNO];
    static St_AbnormalCheck     AbnormalInfo[MAXCHANNELNO]; //1805222 zyx
    static St_IrregularSetting  m_IrregularSetting;
    static unsigned char 		Hw_Err_Flag[MAXCHANNELNO];		//add wzh
    static unsigned char Safty_HwErrFlag;									//add wzh
    static double g_dChargeCapacity[MAXCHANNELNO];
    static double g_dDischargeCapacity[MAXCHANNELNO];
    static void Init(void);
    static void DoWork(void);
    static void          InitCtrlValue(unsigned char ChannelIdx,unsigned char CheckType,unsigned char VoltRange,unsigned char CurrRange);
    static void          InitAbnormalCheck(unsigned char ChannelIdx);
    static void SetUnsafe(unsigned char chn, uint32 unsafeCode, float value,unsigned char UnsafeType);

    static bool IsUnsafe(unsigned char chn,uint32 unsafeCode,unsigned char UnsafeType);
    static void sendError(unsigned char chn);    //向sch或其他发送警告信息，并停止通道
    static unsigned char VClampRangeCheck(unsigned char ChannelIdx,float Value);
    static unsigned char TemperatureChamberCheck(unsigned char ChannelIdx);//检查该通道是否想控制温控箱
    // 新定义成员变量
    static unsigned char Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.
protected:

private:
    //	#if SUPPORT_PARALLEL == 1
    static float Submaster_Record;
    static void check_Submaster(unsigned char chn, uint32 checkItem);
    static void check_SubSlave(unsigned char chn, uint32 checkItem);
    static void  BuiltinCheck(unsigned char chn);
    //	#endif
    static char Safety_Check_status[MAXCHANNELNO];

    static unsigned char safety_chn;    //当前check的chn

    static MsTime safety_last_time[MAXCHANNELNO];
    static MsTime safety_error_time[MAXCHANNELNO];
    static MsTime safety_resume_time[MAXCHANNELNO];
    static float UD_Record[MAXCHANNELNO];                           //170122
    static int UD_UnsafeIndex[MAXCHANNELNO];                      //170122

    static uint32 CheckAndSetCandidate(unsigned char chn);
    static void BuiltinAuxReportUnsafe(unsigned char chn,unsigned char AuxType,float value,unsigned char aux_chn);


    static void ClearSchUnsafeIfNotRunning(unsigned char chn);
    static void StatusProcess(unsigned char chn, uint32 checkItem);
    //add philip

    static char safety_check_sch_is_ready(unsigned char chn);
    static void check(unsigned char chn, uint32 checkItem);
    static char resumeError(unsigned char chn);    //恢复sch

    static void MakeIrregularCheck(void);
    static unsigned char CheckIrrationality(unsigned char ChannelIdx);
    static void MakeCtrlValueCheck(unsigned char ChannelIdx); //1805222 zyx
    static unsigned char CheckCtrlValue(unsigned char ChannelIdx);
    static void MakeHwErrCheck(unsigned char chn);   //add Wzh
    static void SafetyValueCheck(DWORD type, unsigned char chn, float value);
    static void Fan_Safety_Check(unsigned char chn);
    static void CheckCableConnection(unsigned char chn);
public:
    static void ClearUnsafeIfNotRunning(unsigned char chn);
    static void SetRedundantSafetyFlgCheck(unsigned char chn,float f_Value); //Add by DKQ
    static void ReleaseRedundantSafetyFlgCheck(unsigned char chn);
    static void SetPV_Error(unsigned char chn, uint32 unsafeCode, unsigned char UnsafeType );
    static void ClrUnsafe(unsigned char chn, uint32 unsafeCode, unsigned char UnsafeType );
    static uint32 GetPV_Error(unsigned char chn,  unsigned char UnsafeType );
    static void ClearAll_PV_Error(unsigned char chn);
    static uint8 GetPV_ErrorType(unsigned char chn);
    static void SetBuiltInSafetyPara(unsigned char uc_ChannelNo);


};

#endif
