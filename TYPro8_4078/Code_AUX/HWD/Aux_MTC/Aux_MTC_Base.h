
#pragma once
#include "../HWD/Aux_Chamber_TEC/Aux_Chamber_TEC_Base.h"

#define MTC_MAX_CHANNELNO 8

typedef struct
{
	bool    m_IsOpenChamber;
    bool    m_UseGroupTChamber;
    bool    m_UseDefaultValue;
    unsigned short m_UnitIP;
    unsigned short m_IVChnIndexInUnit;
    float   m_DefaultTempSetting;
}MTC_GROUP_CTRL;
enum
{
    Pin_TEC_PWM_1,
    Pin_TEC_PWM_2,
    Pin_TEC_PWM_3,
    Pin_TEC_PWM_4,
    Pin_TEC_PWM_5,
    Pin_TEC_PWM_6,
    Pin_TEC_PWM_7,
    Pin_TEC_PWM_8,
    Pin_TEC_ENB_1,
    Pin_TEC_ENB_2,
    Pin_TEC_ENB_3,
    Pin_TEC_ENB_4,
    Pin_TEC_ENB_5,
    Pin_TEC_ENB_6,
    Pin_TEC_ENB_7,
    Pin_TEC_ENB_8,
    Pin_TEC_DIR_1,
    Pin_TEC_DIR_2,
    Pin_TEC_DIR_3,
    Pin_TEC_DIR_4,
    Pin_TEC_DIR_5,
    Pin_TEC_DIR_6,
    Pin_TEC_DIR_7,
    Pin_TEC_DIR_8,
    Pin_MTC_TEMP_ALERT,
    Pin_FAN_LOCK_1,
    Pin_FAN_LOCK_2,
    Pin_FAN_LOCK_3,
    Pin_FAN_LOCK_4,
    Pin_FAN_LOCK_5,
    Pin_FAN_LOCK_6,
    Pin_FAN_LOCK_7,
    Pin_FAN_LOCK_8,
	Pin_FAN_CTRL_1,
	Pin_FAN_CTRL_2,
	Pin_FAN_CTRL_3,
	Pin_FAN_CTRL_4,
	Pin_FAN_CTRL_5,
	Pin_FAN_CTRL_6,
	Pin_FAN_CTRL_7,
	Pin_FAN_CTRL_8,
//	Pin_FCTL_A1,
//	Pin_FCTL_B1,
//	Pin_FCTL_A2,
//	Pin_FCTL_B2,
//	Pin_FCTL_A3,
//	Pin_FCTL_B3,
//	Pin_FCTL_A4,
//	Pin_FCTL_B4,
//	Pin_FCTL_A5,
//	Pin_FCTL_B5,
//	Pin_FCTL_A6,
//	Pin_FCTL_B6,
//	Pin_FCTL_A7,
//	Pin_FCTL_B7,
//	Pin_FCTL_A8,
//	Pin_FCTL_B8,
//	Pin_FCTL_A9,
//	Pin_FCTL_B9,
//	Pin_FCTL_A10,
//	Pin_FCTL_B10,
//	Pin_FCTL_A11,
//	Pin_FCTL_B11,
//	Pin_FCTL_A12,
//	Pin_FCTL_B12,
//	Pin_FCTL_A13,
//	Pin_FCTL_B13,
//	Pin_FCTL_A14,
//	Pin_FCTL_B14,
//	Pin_FCTL_A15,
//	Pin_FCTL_B15,
//	Pin_FCTL_A16,
//	Pin_FCTL_B16,
    Pin_MTC_Total_Pins
};  //by zc 04.04.2018

enum {
    MTCStatusOK = 0x000000,
    MTCOverlimit = 0x010000,
    MTCOverTime  = 0x020000,
    MTCFanFault  = 0x030000
};

class Aux_MTC_Base
{
public:
		static unsigned char Fan_Err_Screen_Iter[MTC_MAX_CHANNELNO];

    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static bool Aux_MTC_Board;
		static int Aux_MTC_Version;  //Add for New 2nd version MZTC by ZC 12/013/2019
    static int  index;
    static UInt16 Priority;
    static UInt16 TaskID;
		static int PinLevel;
    static CHAMBER_TEC_PID_INFO MTC_Pid_Info[MTC_MAX_CHANNELNO];
    static float Temperature[MTC_MAX_CHANNELNO];
    static long long Temperature_set[MTC_MAX_CHANNELNO];
    static unsigned char T_Alert_Flag[MTC_MAX_CHANNELNO];
    static unsigned short MTCSafetyCheckTimeValue[MTC_MAX_CHANNELNO];
    static unsigned short Aux_MTC_TemperatureSafetyCheckFlag[MTC_MAX_CHANNELNO];
    static unsigned short Aux_MTC_TemperatureSafetyReportFlag[MTC_MAX_CHANNELNO];
    static unsigned short Aux_MTC_TemperatureHighLimitCheckFlag[MTC_MAX_CHANNELNO];
    static unsigned short Aux_MTC_TemperatureLowLimitCheckFlag[MTC_MAX_CHANNELNO];
    static long long Aux_MTC_TemperatureHighLimit[MTC_MAX_CHANNELNO];
    static long long Aux_MTC_TemperatureLowLimit[MTC_MAX_CHANNELNO];
    static MsTime MTCSafetyCheckTimeBuffer[MTC_MAX_CHANNELNO];
    static unsigned short MTCTempSafetyGoto[MTC_MAX_CHANNELNO];
    static int MTC_CH_Status[MTC_MAX_CHANNELNO];
    static unsigned short ALLTemperatureSafetyCheckFlag;
    static unsigned short ALLTemperatureHighLimitCheckFlag;
    static unsigned short ALLTemperatureLowLimitCheckFlag;
    static int MTCMode[MTC_MAX_CHANNELNO]; //MTC当前模式
	static MTC_GROUP_CTRL st_MTC_Group_Ctrl[MTC_MAX_CHANNELNO];
	static unsigned char groupCount;
	static int MTC_Fan_Fault_Counter[MTC_MAX_CHANNELNO];  //Add for New 2nd version MZTC by ZC 12/013/2019
//		static unsigned short TestInput[MTC_MAX_CHANNELNO];
//		static unsigned short Fan_Fault_All_Flag;
//		static bool testflag;

    static void Init(void);
    static void DoWork(void);
    static void lltostr(long long TempSetPoit);
    static void Set_TempPoint(unsigned int TECChannelNo);
    static void TemperatureCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType, unsigned char MTC_ChnIdx);
    static void FanLockCheckLocal(unsigned char MTC_ChnIdx);// Fan Fault Check Local. Mu
		static void FanLockCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType, unsigned char MTC_ChnIdx);
	  static void TurnOnMTCChn( unsigned char TECChannelNo );
    static void TurnOffMTCChn( unsigned char TECChannelNo );

private:
    static unsigned char T_Alert_Flag2;
    static unsigned char KEY_FLAG;
    static float PW_INC_LMT;
    static float PW_DEC_LMT;
    static unsigned char TempUp;
    static unsigned short TEC_Phase;
    static unsigned char TC_count;
    static float Temperature1[MTC_MAX_CHANNELNO];
    static unsigned char COOLRANGE[MTC_MAX_CHANNELNO];
    static unsigned char HEATRANGE[MTC_MAX_CHANNELNO];
    static unsigned char temp[2];
    static unsigned char ErrorReportCount[MTC_MAX_CHANNELNO];
//		static unsigned char TouchScreenRefreshCount;

    static void Init_PIDPara(void);
    static void Init_MTC(void);
    static float PID_Regulator(unsigned int TECChannelNo);
    static void Init_PWM(void); //初始化PWM模块
    static void PWM_Calculation(float pwm_output, unsigned int TECChannelNo);
    static void Temp_Alert(void);
    static void CLR_Temp_Alert(void);
    static void TemperatureLimitCheck( unsigned char TECChannelNo );

    static void SET_ERR1(void);
    static void SET_ERR2(void);
    static void CLR_ERR1(void);
    static void CLR_ERR2(void);
    static void Check_T_Alert(void);
    static void UpdateAllSetTemp(void);
    static unsigned char ReportMTCTempUnsafe(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType, unsigned char MTC_ChnIdx);
    static unsigned char ReportMTCFanUnsafe(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType, unsigned char MTC_ChnIdx);

};

