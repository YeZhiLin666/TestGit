/*****************************************************
File Name: Protocol.h
Version:		V1.0
Date:				2012/11/12
Description:
�������������ؽṹ������궨��
�Ȼ�����ʵ�ʱ����Ҫ�ϲ���SF_Hwf.h���棬���ϡ���λ������
Others:
History��

******************************************************/
/*#pragma  once

#define IV_MAX_LIMIT_NUMBER                  16        //����IVͨ��STEP LIMIT ���֧����Ŀ
#define IV_MAX_PULSE_STAGE_NUMBER            10        //����IVͨ��PULSE STAGE���֧����Ŀ
#define IV_MAX_CV_STAGE_NUMBER               10        //����IVͨ��CV STAGE���֧����Ŀ
#define IV_MAX_SCHEDULE_STEP_NUMBER          100       //����ÿ��Schedule�����������step��Ŀ
#define IV_MAX_SCHEDULE_CV_NUMBER            10        //����ÿ��Schedule�����������CV��Ŀ
#define IV_MAX_SCHEDULE_FORMULA_NUMBER       10        //����ÿ��Schedule�����������Formula��Ŀ
#define IV_MAX_SCHEDULE_BUFFER_NUMBER        20        //����IVͨ��Schedule�������������ݶ�Ϊ20��
#define IV_MAX_SIMULATION_BUFFER_NUMBER      20        //����IVͨ��Simulation�������������ݶ�Ϊ20��
#define IV_MAX_PULSE_BUFFER_NUMBER           20        //����IVͨ��Pulse�������������ݶ�Ϊ20��
#define IV_MAX_ASSIGN_MD5_NUMBER             16        //����ÿ��ASSIGN��������ܳ��ص�MD5�����

#define IV_NORMAL_SIMULATION_POINT_NUMBER    2000*3    //6K���㣬��ͨsimulation:I,V,P,L simulation��
#define IV_BATTERY_SIMULATION_POINT_NUMBER   2000*2    //4K���㣬���simulation,   ע����Ƕ���ĵ���Ҫʹ���������������ϳߴ�һ����

enum MP_GOTO_STEP
{
MCU_GOTOSTOPSTEP            = 60000,
MCU_GOTONEXTSTEP            = 60001,
MCU_GOTOSAMESTEP            = 60002,
MCU_GOTOUNSAFE              = 60010,
MCU_GOTOWARNING             = 60020,
MCU_GOTOANDCONDICTION       = 60029,
MCU_GOTOGROUPWAITFORNEXT    = 60052,
MCU_GOTOGROUPFOLLOWNEXT     = 60053
};

enum MP_IV_CTRL_TYPE
{                                  //CtrlValue,            ExtCtrlValue1,                ExtCtrlValue2
CT_IVCH_STOP=0,                //09.15.2011
CT_IVCH_CURRENT=11,            //current               N/A                           N/A
CT_IVCH_VOLTAGE,               //voltage               N/A                           N/A
CT_IVCH_CRATE,                 //C                     N/A
CT_IVCH_REST,                  //N/A                   N/A                           N/A
CT_IVCH_POWER,                 //power                 N/A                           N/A
CT_IVCH_LOAD,                  //load                  N/A                           N/A
CT_IVCH_SETVARIABLE,
CT_IVCH_CRAMP,                 //start current(A)      ramp rate(A/s)                N/A
CT_IVCH_VRAMP,                 //start voltage(V)      ramp rate(V/s)                N/A
CT_IVCH_CSTAIRCASE,            //start current(A)      staircase rate(A/stair)       stair (s)
CT_IVCH_VSTAIRCASE,            //start voltage(V)      staircase rate(V/stair)       stair (s)
CT_IVCH_CPULSE,
CT_IVCH_VPULSE,
CT_IVCH_IR,                    //amplitude(A)          sample time(s)                offset
CT_IVCH_ACI,                   //Peak-Peak current(A)  Frequency(Hz)                 offset
CT_IVCH_ACIM,
CT_IVCH_CCCP=26,               //current               voltage
CT_IVCH_CCCV=26,               //current               voltage
CT_IVCH_CELLV,
CT_IVCH_ANALOGOUTPUT,
CT_IVCH_DIGITALOUTPUT,
CT_IVCH_EXTCHARGE,             //N/A                   N/A                            N/A
CT_IVCH_EXTLOAD,
CT_IVCH_ACS,
CT_IVCH_DIGII,
CT_ICVH_DIGIV,
CT_IVCH_DIGIPOWER,
CT_ICVH_DIGILOAD,

CT_IVCH_PRAMP=39,
CT_IVCH_PSTAIRCASE=40,

CT_IVCH_CSIMULATION=51,
CT_IVCH_VSIMULATION,
CT_IVCH_PSIMULATION,
CT_IVCH_LSIMULATION,

CT_IVCH_C_CV=60,
CT_IVCH_V_CV,

CTRL_TYPE_MCELL_ACI=80,       //Peak-Peak current(A)  Frequency(Hz)                  offset
CTRL_TYPE_VOLT_METER,         //N/A                   N/A                            N/A

NUM_OF_MP_IV_CTRL_TYPE
};

enum MP_METAVARIABLE_CODE
{
MetaCode_PV_Voltage,
MetaCode_PV_Current,
MetaCode_PV_Steptime,
MetaCode_PV_TestTime,
MetaCode_PV_StepIndex,
MetaCode_PV_CycleIndex,
MetaCode_PV_ChargeCapacity,
MetaCode_PV_DischargeCapacity,
MetaCode_PV_ChargeEnergy,
MetaCode_PV_DischargeEnergy,
MetaCode_PV_InternalResistance,
MetaCode_PV_dVdt,
MetaCode_PV_dIdt,
MetaCode_PV_d2Vd2t,
MetaCode_PV_PulseStageAveVol,
MetaCode_PV_PulseAveVol,

MetaCode_LS_Voltage,
MetaCode_LS_Current,
MetaCode_LS_StepTime,
MetaCode_LS_TestTime,
MetaCode_LS_StepIndex,
MetaCode_LS_CycleIndex,
MetaCode_LS_ChargeCapacity,
MetaCode_LS_DischargeCapacity,
MetaCode_LS_ChargeEnergy,
MetaCode_LS_DischargeEnergy,
MetaCode_LS_InternalResistance,

MetaCode_LC_Voltage,
MetaCode_LC_Current,
MetaCode_LC_StepTime,
MetaCode_LC_TestTime,
MetaCode_LC_StepIndex,
MetaCode_LC_CycleIndex,
MetaCode_LC_ChargeCapacity,
MetaCode_LC_DischargeCapacity,
MetaCode_LC_ChargeEnergy,
MetaCode_LC_DischargeEnergy,
MetaCode_LC_InternalResistance,

MetaCode_LL_Voltage,
MetaCode_LL_Current,
MetaCode_LL_StepTime,
MetaCode_LL_TestTime,
MetaCode_LL_StepIndex,
MetaCode_LL_CycleIndex,
MetaCode_LL_ChargeCapacity,
MetaCode_LL_DischargeCapacity,
MetaCode_LL_ChargeEnergy,
MetaCode_LL_DischargeEnergy,
MetaCode_LL_InternalResistance,
MetaCode_LL_PulseStageAveVol,
MetaCode_LL_PulseAveVol,

MetaCode_DV_Time,
MetaCode_DV_Current,
MetaCode_DV_Voltage,

MetaCode_TC_Counter1,
MetaCode_TC_Counter2,
MetaCode_TC_Counter3,
MetaCode_TC_Counter4,
MetaCode_TC_Timer1,
MetaCode_TC_Timer2,
MetaCode_TC_Timer3,
MetaCode_TC_Timer4,
MetaCode_TC_ChargeCapacity1,
MetaCode_TC_ChargeCapacity2,
MetaCode_TC_DischargeCapacity1,
MetaCode_TC_DischargeCapacity2,
MetaCode_TC_ChargeEnergy1,
MetaCode_TC_ChargeEnergy2,
MetaCode_TC_DischargeEnergy1,
MetaCode_TC_DischargeEnergy2,

MetaCode_MV_Mass,
MetaCode_MV_SpecificCapacity,
MetaCode_MV_NorminalCapacity,
MetaCode_MV_IHmin,
MetaCode_MV_IHmax,
MetaCode_MV_IMmin,
MetaCode_MV_IMmax,
MetaCode_MV_ILmin,
MetaCode_MV_ILmax,
MetaCode_MV_VHmin,
MetaCode_MV_VHmax,
MetaCode_MV_VLmin,
MetaCode_MV_VLmax,
};

enum MP_CURRENT_RANGE
{
CURRENT_RANGE_LOW,
CURRENT_RANGE_MED,
CURRENT_RANGE_HIGH,
CURRENT_RANGE_VERGYHIGH,
};

enum MP_VOLTAGE_RANGE
{
VOLTAGE_RANGE_LOW,
VOLTAGE_RANGE_MED,
VOLTAGE_RANGE_HIGH,
VOLTAGE_RANGE_VERYHIGH,
};

enum MP_MD5CODE_TYPE
{
MD5CODE_TYPE_SCHEDULE,
MD5CODE_TYPE_SIMULATION,
MD5CODE_TYPE_PULSE,
};

enum MCU_ERRCODE
{
MCU_ERRCODE_STARTCMD_UNKNOW            =   1,          //����ȷ���߲��Ե�START����
MCU_ERRCODE_STARTNPAUSE_UNKNOWCMD      =   2,          //�Ƿ���STARTNPAUSE ����
MCU_ERRCODE_STARTCMD_LOADCODEERROR     =   6,          //ϵͳװ�����ݳ���
MCU_ERRCODE_STARTCMD_SYSUNSAFE         =   8,          //ϵͳ����ȫ�޷�����

MCU_ERRCODE_PAUSECMD_UNKNOWCMD         =  21,
MCU_ERRCODE_PAUSECMD_HAVEPAUSECMD,

MCU_ERRCODE_JUMPCMD_NOTRUNNING		   = 31,

MCU_ERRCODE_RESUMECMD_UNKNOWCMD        = 41,
MCU_ERRCODE_RESUMECMD_STEPRUNNING,

MCU_ERRCODE_SCHEDULETOOLONG			   = 51,           // SCH ����
MCU_ERRCODE_SCHEDULELENGTHNOTMATCH,		 	           // 52 SCH ���Ȳ�ƥ��(������ؽ����ղ���)
MCU_ERRCODE_SCHEDULESTEPIDERROR,          	           // 53 SCH ��STEPIDԽ��
MCU_ERRCODE_SCHEDULELOADERROR,	         	           // 54 SCH ��STEP���ش���

MCU_ERRCODE_SIMULATIONDATAERROR			=61,		   // Simulation Data Error

MCU_ERRCODE_EMERGENCYSTOP			    =81,

MCU_ERRCODE_WATCHDOG_TIMEOUT	        =91		       // WatchDog Timeout
};

typedef struct //St_MD5
{
unsigned char m_ucData[16];
}St_MD5;

typedef struct
{
St_MD5         m_MD5;
unsigned char  m_ucType;
unsigned char  m_bOk;
}St_MD5Assign;

typedef struct
{
unsigned short m_u16ChannelIdx;
unsigned char  m_ucMD5Count;
unsigned char  m_ucReserved;

St_MD5Assign   m_MD5Assign[IV_MAX_ASSIGN_MD5_NUMBER];
}St_Assign;*/

/*������ͨ��Step������ؽṹ����*/
/*typedef union  //Un_CtrlValues
{
St_MD5         m_MD5;                   //���ڿ���ֵ��������Ʒ�ʽ��simulation,pulse�������ʹ��MD5
float          m_fControlValue[5];      //���������������Ŀ���ֵ����5��������,�����Ԥ��Ϊbattery simulation�ĳ�ʼ����ʹ��
}Un_CtrlValues;
typedef struct //St_StepCtrl
{
unsigned short m_u16StepID;
unsigned char  m_ucStepLimitNo;
unsigned char  m_ucLogLimitNo;          //���ܲ���

unsigned short m_u16ControlType;
unsigned char  m_ucCurrentRange;
unsigned char  m_ucVoltageRange;

Un_CtrlValues  m_CtrlValues;

unsigned char  m_ucCtrlValue_DataType[4];     //4������ֵ�� ��ֵ���ͣ� ������MetaVariable��Formula...
}St_StepCtrl;
typedef struct //St_Limit
{
float          m_fLeftValue;
float          m_fRightValue;

unsigned char  m_ucLeftValue_DataType;
unsigned char  m_ucRightValue_DataType;
unsigned short m_u16GotoStepID;

unsigned char  m_ucCompareSign;
unsigned char  m_ucReserved[3];

unsigned char  m_ucIfCanSelfJump;  //��limit�Ƿ���Զ�������״̬
unsigned char  m_ucEquationID;     //��limit����λ����equation���
unsigned short m_u16LimitID;       //��limit����λ����limit���

float          m_fReserved;
}St_Limit;
typedef struct //St_StepData
{
St_StepCtrl             m_Ctrl;
St_Limit                m_Limits[IV_MAX_LIMIT_NUMBER];
}St_StepData;*/

/*������ͨ��Pulse������ؽṹ����*/
/*typedef struct //St_PulseStage
{
float                   m_fAmplitude;
float                   m_fStageTime;
float                   m_fLogInterval;

unsigned char           m_ucControlType;
unsigned char           m_ucReserved[3];
}St_PulseStage;
typedef struct //St_PulseData
{
unsigned char           m_ucStageCount;
unsigned char           m_bIsSinglePulse;
unsigned char           m_ucReserved[2];

float                   m_fBaseAmplitude;
float                   m_fLeavingAmplitude;

St_PulseStage           m_Stages[IV_MAX_PULSE_STAGE_NUMBER];
}St_PulseData;*/

/*������ͨ��Formula������ؽṹ����*/
/*typedef struct //St_FormulaData
{
unsigned char           m_ucFUNCTION1;
unsigned char           m_ucFUNCTION2;
unsigned char           m_ucFUNCTIONX;
unsigned char           m_ucFUNCTIONY;

unsigned char           m_ucOPERATOR;
unsigned char           m_ucReserved[3];

unsigned char           m_ucDataType[8];

float                   m_fX1;
float                   m_fX2;
float                   m_fX3;
float                   m_fX4;
float                   m_fY1;
float                   m_fY2;
float                   m_fY3;
float                   m_fY4;
}St_FormulaData;*/

/*������ͨ��CV������ؽṹ����*/
/*typedef struct //St_CvStage
{
float                   m_fStart;
float                   m_fEnd;
float                   m_fScanRate;
}St_CvStage;
typedef struct //St_CvCtrl
{
unsigned char           m_ucStageCount;
unsigned char           m_ucReserved[3];

unsigned long           m_ucRepeatNumber;

float                   m_fBase;
}St_CvCtrl;
typedef struct //St_CvData
{
St_CvCtrl               m_Ctrl;

St_CvStage              m_Stages[IV_MAX_CV_STAGE_NUMBER];
}St_CvData;*/

/*������Schedule Buffer������ؽṹ����*/
/*typedef struct //St_SchSafeData
{
float           m_fMin;
float           m_fMax;
}St_SchSafeData;
typedef struct //St_ScheduleSetting
{
unsigned short m_u16SafetyEnabed;
unsigned short m_u16Reserved1;

St_SchSafeData m_Isafty;    // ��Ϊ�ṹ
St_SchSafeData m_Vsafty;
St_SchSafeData m_Psafty;
St_SchSafeData m_Reserved2[2];

unsigned short m_u16UnsafeCheckCount;
unsigned short m_u16Reserved3;

float          m_fDvDtInterval;    //second;

float          m_fReserved4[16];

}St_ScheduleSetting;
typedef struct //St_ScheduleItemCount
{
unsigned short     m_u16StepCount;
unsigned short     m_u16Reserved0;

unsigned short     m_u16FormulaCount;
unsigned short     m_u16CvCount;

unsigned short     m_u16Reserved[4];   //Ҫ����ż��������4�ֽڶ���
}St_ScheduleItemCount;*/

/*��������*/
/*typedef struct //St_IndependantCtrl
{
unsigned short          m_u16ControlType;
unsigned char           m_ucCurrentRange;
unsigned char           m_ucVoltageRange;

float                   m_fControlValue;

float                   m_fClampVH;
float                   m_fClampVL;
}St_IndependantCtrl;*/

/*Config Struct*/
/*typedef struct //St_RangeBound
{
float               m_High;
float               m_Low;
}St_RangeBound;
typedef struct //St_RangeCali
{
float               m_Gain;
float               m_Offset;
}St_RangeCali;
typedef struct //St_IRangeCFG
{
St_RangeBound       m_Bound;

St_RangeCali        m_Adc;
St_RangeCali        m_Dac;

unsigned char       m_bHasDAC;
unsigned char       m_Reserved[3];

unsigned long       m_u32DacRampZeroDelayInUs;  //maxwell .net����ʹ�ã��Ժ��ò���
float               m_fDacRampZeroOutput;
float               m_fDacRampRate;
}St_IRangeCFG;
typedef struct //St_VRangeCFG
{
St_RangeBound       m_Bound;

St_RangeCali        m_Adc;
St_RangeCali        m_Dac;

unsigned char       m_bHasDAC;
unsigned char       m_Reserved[3];
}St_VRangeCFG;
typedef struct //St_VClampCFG
{
St_RangeCali        m_HrangeDac;
St_RangeCali        m_LrangeDac;
}St_VClampCFG;
typedef struct //St_ChannelCFG
{
unsigned char       m_ucIRangeCount;
unsigned char       m_ucVRangeCount;
unsigned char       m_ucReserved[2];

St_IRangeCFG        m_IRange[8];
St_VRangeCFG        m_VRange[4];
St_VClampCFG        m_VClamp;
}St_ChannelCFG;
typedef struct //St_BoardGlobalCFG
{
unsigned long       m_u32ArbinID;

unsigned char       m_ucPCA[10];
unsigned short      m_u16Reserved;

unsigned short      m_u16BoardType;
unsigned short      m_u16ChannelType;

unsigned short      m_u16IVChannelCount;   //�����ֽ�
unsigned short      m_u16GatewayChannelCout;

unsigned short      m_u16AuxChannelCount;  //�����ֽ�
unsigned short      m_u16Reserved2;
}St_BoardGlobalCFG;*/


