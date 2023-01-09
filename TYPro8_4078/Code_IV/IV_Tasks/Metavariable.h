//??????METAVARIABE
//MV???$)A!h

#ifndef _METAVARIABLE_H_
#define _METAVARIABLE_H_
#include "../../COMMON_OS/LPC4078CORE/RTC.h"


#define M0_REFRESH_TIME 10000	//1s

//#define ABS(x) ( (x>0)?(x):(-x) )    //??????
#define MAX_AUXCH_PER_DATA_GROUP        16
#define MAX_DXDT_T_TIME            (3.4f)       //4*1000*10    // DXDTŒ¬∂»Õ®µ¿º∆À„º‰∏Ù ±º‰4√Î   Tmax   17*200ms
#define MIN_DXDT_T_TIME            (1.0f)     	// DXDTŒ¬∂»Õ®µ¿º∆À„º‰∏Ù◊Ó–° ±º‰1√Î   Tmin
#define DXDT_T_PRECISION           (0.1f)       // DXDTŒ¬∂»Õ®µ¿0.1∂»
#define MIN_DXDT_V_TIME            (0.1f)     	// DXDTµÁ—πÕ®µ¿º∆À„º‰∏Ù◊Ó–° ±º‰100∫¡√Î   Tmin
#define MAX_DXDT_V_TIME            (1.0f)       // DXDTµÁ—πÕ®µ¿º∆À„º‰∏Ù◊Ó¥Û ±º‰1√Î   Tmax
#define DXDT_V_PRECISION           (0.0002f)    // DXDTµÁ—πÕ®µ¿¡ø≥Ãæ´∂»µƒÕÚ∑÷÷Æ∂˛  

enum
{
    SMBLOG_IDLE=0,
    SMBLOG_WAIT,
    SMBLOG_DOLOG,
    SMBLOG_NEXT
};

typedef struct
{
    unsigned char     My_CurrentHnd;
    unsigned char     MySMBMv_ID;
    MsTime 						MyRecorderTime;
    ST_SMB_MetaValue	MySMBRecordData;
    unsigned char     MySMBRecordType;
} ST_SMBDATABUF;

typedef struct
{
    bool    bRefresh;   
    //float   m_PowerIndex;      
    float   V_accuracy;
    float   m_deltaV;
    //int     m_countMax;
    float   m_TimeMin;
    float   m_TimeMax;
    //float   m_sum;
    //float   m_DeltaVolt;
    float   m_VoltLast;
    float   m_Dxdt;     
    //UINT    m_count;
    MsTime  LastSampleTime;
} DXDT_INFO;

// typedef struct//180921 zyx
// {
// 	bool  IsRefresh;
//     bool  firstPoint;
// 	unsigned char reserved[2];
// 	float LastVoltage;
// 	float LastCapacity;
//     float DeltaV_min;
//     float DeltaQ_min;
//     float TimeMin;
//     float TimeMax;
// 	MsTime LastSampleTime;
// }LastValue;

typedef struct
{
    bool  IsRefresh;
    bool  firstPoint;
    unsigned char reserved[2];
    float LastVoltage;
    float LastCapacity;
    float DeltaV_min;
    float DeltaQ_min;
    float TimeMin;
    float TimeMax;
    float MinDeltaValue;//Min Delta Value when over TimeMax.
    MsTime LastSampleTime;
} LastValue;

typedef struct
{
    float Value;
		float aux_dx_dt;
    MsTime Time;
} ST_DXDT_VALUE;

typedef struct
{
	  float Max_Time;
	  float Min_Time;
	  float Precision;
}St_DXDT_Parameter;

class MetaVariable_Task
{
private:
    static int Cap_Calcu_Ptr;
    static int MYSTATUS;
    static ST_DXDT_VALUE     st_History[2][MAX_AUXCH_PER_DATA_GROUP];
    static ST_SMBDATABUF	SMBData_Buf[200];


public:
	static bool b_RedundantVotlage_NeedUpdate[MAXCHANNELNO];
    static MsTime 					M0_RefreshInterval;
    static int 						VSMB_Handle[MAXCHANNELNO];
    static int My_SMBLog_Status;
    static MsTime m_capacity_Time[MAXCHANNELNO];    //?????$)A!@??
    static MsTime m_SMBRef_Time[MAX_SMB_BMS_SIGNAL];
    static char capacityStatus[MAXCHANNELNO];    //
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]

    static bool bSendMVUD[MAXCHANNELNO];
	
    static ST_SMBDATABUF  m_SMBLogData;
    static DXDT_INFO dVdtInfo[MAXCHANNELNO];
    /***************** dQdV **************************/
    static LastValue dQdV[MAXCHANNELNO];

    /***************** dVdQ **************************/
    static LastValue dVdQ[MAXCHANNELNO];
	
	static void UpdateValues_WithMV_UD();
public:
    static void Init(void);
    static void DoWork(void);
// 	static void EndTask(void);
    static void calculate_capacity(char requestType, char chn);
//	static void Init_dVdt(unsigned char ChannelIdx);
    static int16 CheckAndSetCapCandidate(char requestType, char chn);
    static void Calculate_dVdt(unsigned char uc_ChannelNo);
    static void InitDvdt(unsigned char ChannelIdx);
    static void update_dVdq(unsigned char ChannelIdx);
    static void Init_dVdq(unsigned char ChannelIdx);
    static void Init_dQdv(unsigned char ChannelIdx);
    static void update_dQdV(unsigned char ChannelIdx);
		static void Calculate_BuiltInAuxDXDT(unsigned char uc_Group,unsigned char uc_ch);
    static void BuiltInAuxDXDT_Result(unsigned char uc_Group,unsigned char uc_ch,double f_Deta,double f_Time);
		static void BuiltInAuxDXDT_GetParameter(unsigned char uc_Group,unsigned char uc_ch,St_DXDT_Parameter * st_dxdt_Parameter);
private:
//	static void calculate_dVdt(void);
    static void calculate_SMBRefreshValue(void);
    static void calculate_dQdV(void);
    static void calculate_dVdQ(void);

};

#endif
