//����ΪMETAVARIABE
//MVά��

#ifndef _METAVARIABLE_H_
#define _METAVARIABLE_H_
#include "../../COMMON_OS/LPC4078CORE/RTC.h"

#define MAX_DXDT_T_TIME            (8.0f)       //4*1000*10    // DXDT�¶�ͨ��������ʱ��4��   Tmax   17*200ms
#define MIN_DXDT_T_TIME            (1.0f)     	// DXDT�¶�ͨ����������Сʱ��1��   Tmin
#define DXDT_T_PRECISION           (0.5f)       // DXDT�¶�ͨ��0.5��
#define MIN_DXDT_V_TIME            (0.1f)     	// DXDT��ѹͨ����������Сʱ��100����   Tmin
#define MAX_DXDT_V_TIME            (10.0f)       // DXDT��ѹͨ�����������ʱ��10��   Tmax
#define DXDT_V_PRECISION           (0.0002f)    // DXDT��ѹͨ�����̾��ȵ����֮��  

#define MIN_DXDT_AuxCurr_TIME      (0.1f)
#define MAX_DXDT_AuxCurr_TIME      (10.0f)
#define DXDT_AuxCurr_PRECISION     (0.0002f)

#define MIN_DXDT_P_TIME            (0.1f)
#define MAX_DXDT_P_TIME            (10.0f)
#define DXDT_P_PRECISION           (0.0002f)

#define MIN_DXDT_EXT_TIME          (0.1f)
#define MAX_DXDT_EXT_TIME          (10.0f)
#define DXDT_EXT_PRECISION         (0.0002f)

#define MIN_DXDT_ACIM_TIME         (0.1f)
#define MAX_DXDT_ACIM_TIME         (10.0f)
#define DXDT_ACIM_PRECISION        (0.0002f)

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
} St_DXDT_Parameter;

class MetaVariable_Task
{
private:
    static MsTime LastSentEnergyTime;  //�ϴη��͵���������ʱ��
    static char  TimeRecordFlag;
    static int Cap_Calcu_Ptr;
    static int MystatusDXDT[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];
    static char Inc_Dec_Flag[8];
    static ST_DXDT_VALUE     st_History[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];
public:
    static MsTime m_capacity_Time[8];    //����ʱ��
    static char  capacityStatus[8];    //
    static char  MetaTaskRunCounter;  //��ʼ��Ϊ0������һͨ���̵�����ʱ����1���ر�ʱ��1.����־��Ϊ0�������õ����������㡣
    static float ChargeEnergy[8];
    static float ChargeCapacity[8];
    static float DischargeEnergy[8];
    static float DisChargeCapacity[8];
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]
    static unsigned char  C_E_Flag;

public:
    static void Init(void);
    static void DoWork(void);
    static void EndTask(void);
    static void calculate_capacity(char requestType, char chn);
    // 		static void Init_dVdt(unsigned char ChannelIdx);
    static int16 CheckAndSetCapCandidate(char requestType, char chn);
    static void IncCCapacity(unsigned short uc_ChannelNo, double dSetValue);
    static void IncDCapacity(unsigned short uc_ChannelNo, double dSetValue);
    static void IncCEnergy(unsigned short uc_ChannelNo, double dSetValue);
    static void IncDEnergy(unsigned short uc_ChannelNo, double dSetValue);
    static void Calculate_AuxDXDT(unsigned char uc_Group,unsigned char uc_ch);
    static void AuxDXDT_Result(unsigned char uc_Group,unsigned char uc_ch,float f_Deta,float f_Time);
    static void AuxDXDT_GetParameter(unsigned char uc_Group,unsigned char uc_ch,St_DXDT_Parameter * st_dxdt_Parameter);
    // 		 static void

private:


    // 		static void calculate_dVdt(void);
};

#endif
