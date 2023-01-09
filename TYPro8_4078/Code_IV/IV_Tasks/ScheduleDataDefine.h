/*****************************************************
File Name: ScheduleDataDefine.h
Version:        V1.0
Date:                2012/11/12
Description:
�������������ؽṹ������궨��
Others:
History��

******************************************************/
#pragma once

#include "../Entry/includes.h"

#include "../../COMMON_OS/LPC4078CORE/TIMERS.h"
#include "../../COMMON_OS/LPC4078CORE/RTC.h"
#include "../../COMMON_OS/protocols/SF_Hwf.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue.h"

#include "../../common_OS/protocols/SF_Hwf.h"
#include "../IV_Tasks/SimuBuffer.h"

#define IV_BUFFER1_BETA                      20
#define IV_BUFFER1_ALPHA                     20

#define FRAM_BASE                            0x80        // Fram�洢Config����ʼ��ַ...  //IVconfig���ռ��6K���ҵĿռ䣬 PID��16K����ַ��ʼ��
#define FRAM_NONE_BASE                       0X4010      // ��������2.3K�ֽ�  (Ԥ���µĿռ䣩["2.3K�ֽ�"]
#define FRAM_CANBMS_MAP_BASE                 0X4940      // CANBMS_MAPռ��68�ֽ� 
#define FRAM_PARALLEL_BASE                   0X4990      // PARALLELռ��636�ֽ�  
#define FRAM_IV_BOARDTYPE                    0X4D00      //  
#define FRAM_PID_BASE                        0X4D64      // PIDռ��6.1K�ֽ� // sizeof(St_IV_ChannelPID)*16=384*16=6144  
#define FRAM_AUXMAP_BASE                     0X6570      // AUX MAPռ��4.1K�ֽ� 40+128*8*4=4136  (�뱣��AUXMAP��������Է����պ�Դ���������֧�֣�

#define FRAM_REMAIN_LENGTH                   (0X8000 - FRAM_AUXMAP_BASE -  FRAM_AUXMAP_LENGTH)   // 256K/8=32K    ʣ���С�ռ�1.1K


//����5.7K��MAP��,512*11=5632   �൱��һ��IVMCU����֧��4��AUXMCU������512��ͨ���� ƽ��512/16=32 ,ÿ��IV�ɴ�32��Auxͨ����
#define MAXAUXMCUCHANNE       (512) // (4*8*16)    //IVMCU����֧��4��AUXMCU������512��ͨ��
#define FRAM_AUXMAP_LENGTH    (0X1628) // 5672    //IVMCU����֧��4��AUXMCU������512��ͨ��,ÿͨ��11���ֽ�  // 40+512*11=5672
#define MAX_ADC_CODE_QUE             32
// MAP������ʵ��һά�ֽ�����   ��IV�����11�ֽڼ���Ԥ���ռ䣬����AUXʵ��6�ֽڼ���Ԥ����

#define   Ec0_Ttype	0.0f
#define   Ec1_Ttype	3.8748106364e1f
#define   Ec2_Ttype	3.329222788e-2f
#define   Ec3_Ttype	2.0618243404e-4f
#define   Ec4_Ttype	-2.1882256846e-6f
#define   Ec5_Ttype	1.0996880928e-8f
#define   Ec6_Ttype	-3.0815758772e-11f
#define   Ec7_Ttype	4.547913529e-14f
#define   Ec8_Ttype	-2.7512901673e-17f
const float   Ec0_Ktype	= 1.7600413686e1f;
const float   Ec1_Ktype	= 3.8921204975e1f;
const float   Ec2_Ktype	= 1.8558770032e-2f;
const float   Ec3_Ktype	= -9.9457592874e-5f;
const float   Ec4_Ktype	= 3.1840945719e-7f;
const float   Ec5_Ktype	= -5.6072844889e-10f;
const float   Ec6_Ktype	= 5.6075059059e-13f;
const float   Ec7_Ktype	= -3.2020720003e-16f;
const float   Ec8_Ktype	= 9.7151147152e-20f;
const float   Ec9_Ktype	= -1.2104721275e-23f;
const float   Ea0_Ktype	= 1.185976e2f;
const float   Ea1_Ktype	= -1.183432e-4f;

const float   Ec0_Jtype	= 0.0f;
const float   Ec1_Jtype	= 5.038118715e1f;
const float   Ec2_Jtype	= 3.0475836930e-2f;
const float   Ec3_Jtype	= -8.5681065720e-5f;
const float   Ec4_Jtype	= 1.3228195295e-7f;
const float   Ec5_Jtype	= -1.705295837e-10f;
const float   Ec6_Jtype	= 2.0948090697e-13f;
const float   Ec7_Jtype	= -1.2538395336e-16f;
const float   Ec8_Jtype	= 1.5631725697e-20f;

const float   Ec0_Etype	= 0.0f;
const float   Ec1_Etype	= 5.86655087e1f;
const float   Ec2_Etype	= 4.5032275582e-2f;
const float   Ec3_Etype	= 2.8908407212e-5f;
const float   Ec4_Etype	= -3.3056896652e-7f;
const float   Ec5_Etype	= 6.502440327e-10f;
const float   Ec6_Etype	= -1.9197495504e-13f;
const float   Ec7_Etype	= -1.2536600497e-15f;
const float   Ec8_Etype	= 2.1489217569e-18f;
const float   Ec9_Etype	= -1.4388041782e-21f;
const float   Ea0_Etype	= 3.5960899481e-25f;

enum IR_SOURCE
{
	SET_NONE = 0,
    SET_SCH = 1,
    SET_CCCV_WRM = 2,
    SET_STEP = 3,
    SET_CCCV = 4,
    SET_CCCV_TB = 5,
    SET_CC_Rising = 6,
    SET_CCCV_RAMP = 7,
	SET_ESTIMATE = 8,
    SET_UNKNOW = 9,
};

enum FRAM_ADDRESS
{
    CHANNEL_CFG,
    NONE_EMPTY,
    CANBMS_MAP,
    PARALLEL_BATCH,
    BOARDTYPE_CFG,
    PID_CFG,
    REMAIN_EMPTY,
    MAX_ADDRESS
};

//#define MAXCHANNELNO                         16        //���16ͨ��
//#define MAXMSGNO                             16        //��Ϣ���������16��

//#if SUPPORT_PARALLEL == 1
//#define MAXCHANNELNO                         1        //���16ͨ��
#define MAXUNITNO			40		//���40 unit/system
//#endif
#define IV_SAMP_CURRENT_FILTER_DEPTH         256       //����IVͨ���������ݵ����˲�������ֵ��������˲���Ƚ���Ϊ��λ����̬����
#define IV_SAMP_VOLTAGE_FILTER_DEPTH         256       //����IVͨ���������ݵ�ѹ�˲����
#define IV_FRAM_MESSAGE_DEPTH                20        //����FRAM TASK����Ϣ�������
#define IV_SCHEDULE_MESSAGE_DEPTH            4         //����SCHEDULE TASK����Ϣ�������
#define IV_STEP_MESSAGE_DEPTH                4         //����STEP TASK����Ϣ�������
#define IV_PARSE_MESSAGE_DEPTH               4         //����PARSE TASK����Ϣ�������

//safetycheck
#define  DEF_ACCURACY                     0.0005f      //Ĭ�Ͼ��ȣ�Ĭ��Ϊ ���֮5
#define  DEF_CTRL_VALUE_ERROR_FACTOR      100          //Ĭ�ϳ�������100��������Ϊ�ǿ���ֵ�쳣������Ĭ��Ϊ5%
//#define  CTRL_VALUE_CHECK_PERIOD          1 // 60           //����ֵ������ڣ�ʱ�䵥λΪ�� //1805222 zyx
#define  IRREGULAR_CHECK_PERIOD           1 // 60           //�����Լ�����ڣ�ʱ�䵥λΪ��
#define  CTRL_VALUE_CHECK_PERIOD           1 // 10           //����ֵ������� ʱ�䵥λΪ�� //180808 zyx
// #define  IRREGULAR_VOLT_CHECK_COUNT       5 // 10           //�������ѹ��鴥���������
// #define  IRREGULAR_CHECK_MAX_STEPTIME     4*3600       //������ʱ�����ʱ�����ֵ: 4Сʱ
// #define  IRREGULAR_CHECK_ERRORCAPA        1.2f         //�������������Ĵ��󴥷�ϵ��
// #define  IRREGULAR_CHECK_WARNINGCAPA      1.05f        //�������������ı�������ϵ��
// #define  IRREGULAR_CHECK_ERRORVOLT        0.2f         //�������ѹ���Ĵ��󴥷�ϵ��

enum eGroupState
{
    eGroupState_Normal,
    eGroupState_Master,
    eGroupState_Submaster,
    eGroupState_Slave,
    eGroupState_Serial,
    eGroupState_Booster,
};
typedef struct
{
    unsigned char CanID;
    unsigned char ChanIdx;
} St_ChanAddr;
typedef struct
{
    unsigned char State;                            //ͨ������
    St_ChanAddr   Master;                           //��ͨ��Ѱַ
    unsigned char SlaveCount;                       //��ͨ������
    unsigned char SubmastCount;                     //��Unit����
    St_ChanAddr   Submaster[MAX_UNIT_PER_GROUP];    //���д�UnitѰַ
    //����Ϊ���ü�������Ϊ���м���
    bool          bMultiHigh;                       //�Ƿ��������в���HIGH���̣���ͨ����״̬
} St_Grp;

//ͨ����̬��
enum
{
    CHN_LOCK_IDLE = 0x01,
    CHN_LOCK_SCH,
    CHN_LOCK_THIRD_PARTY,
};

enum MESSAGE_SOURCE
{
    Message_Parse = 0,
    Message_Schedule,
    Message_Step,
    Message_MetaVariable,
    Message_3rdPart,
    Message_Fram,
    Message_Sample,
};


//����Step��� ö�ٶ��� //
enum
{
    CCS_OFF,
    CCS_REST,
    CCS_I,
    CCS_V,
    CCS_EXTERNAL,
    CCS_OTHER=50,
}; //����״̬

enum
{
    CT_REST,
    CT_VOLTAGE,
    CT_CURRENT,
    CT_OFF,
}; //��������

enum //STEP״̬
{
    STEPIDLE,
    STEPLOADSTEP,
    STEPTRANSITION,
    STEPRUNNING,
    STEPWAIT,
    STEP_RELAX,
    STEP_AFTER_RELAX,
// 	STEP_WAIT_ParallelSlave_PowerReady,   //�ȴ�������ͨ����Դ�ȶ�
};

enum    //StepLimit״̬
{
    STEPLIMITSTART,
    STEPLIMITNOTREACH,
    STEPLIMITCHECK,
    STEPLIMITGOTONEXT,
    STEPLIMITREACH,
    STEPLIMITLOOP,
    STEPLIMITEND,
};

enum    //CV ״̬
{
    STEP_CV_Init,
    STEP_CV_RAMP,
    STEP_CV_KEEP,
    STEP_CV_IR,
    STEP_CV_AFTER_IR,
    STEP_CV_TEST,
};

enum    //CCCV ״̬
{
    STEP_CCCV_INIT,
    STEP_CCCV_RAMP,
    STEP_CCCV_RELAX,
    STEP_CCCV_AFTER_RELAX,
    //STEP_CCCV_WAIT2CV,
    STEP_CCCV_CC,//STEP_CCCV_START,
    STEP_CCCV_CV,
};
enum //CCCV RAMP
{
    NO_RAMP,
    RAMP_INIT,
    FAST_RAMP,
    SLOW_RAMP,
    RAMP_IR_CALCULATE,//RAMP_IR_TEST,
};

enum    // 06.28.2017 TURBO BOOST ״̬ by ZC
{
    STEP_CCCV_TB_START,
    STEP_CCCV_TB_CC,
    STEP_CCCV_TB_CV,
    STEP_CCCV_TB_CC_DISCHARGE,
};

enum	//LogLimit״̬
{
    STEP_CC_TB_START,       //��ʼ��
    STEP_CC_TB_CHARGE,			//��粽
    STEP_CC_TB_DISCHARGE,		//�ŵ粽
};
enum	//LogLimit״̬
{
    LOGLIMITSTART,
    LOGLIMITIFRUNNING,
    LOGLIMITCHECK,
    LOGLIMITSEND,
    LOGLIMITEND,
};

/*Schedule ���ö�ٶ���*/

enum
{
    PCASSIGNOK,
    PCASSIGNFAIL,

    PCSTARTOK,
    PCSTARTFAIL,

    PCSTOPOK,
    PCSTOPFAIL,
};     //������������� ����ȷ������������Ϊ�Ļظ�

enum
{
    NEXTSTEP,
    ENDTEST,
    UNSAFE,
    SETVARIABLE=0x0011,
    LOGLIMIT,
};	//Limit _Arrive����ʱ��һ����Ӧ����



////safety ����/////////////

enum
{
    CtrlValueCheck_Null,
    CtrlValueCheck_Current,
    CtrlValueCheck_Voltage,
    CtrlValueCheck_Power,
    //CtrlValueCheck_Load,
};
enum
{
    SAFETY_CHECK_SYS = 0x22,
    SAFETY_CHECK_SCH,
};

#define SAFETY_CHECK_PERIOD     1000 // 100ms                 //����ʱ��    ���ｫ�����Ը���Ӳ����ʹ�ò�ͬ��ʱ��
#define SAFETY_CHECK_RESUME     10000 //1000ms  �ָ�sch���ʱ��

//ErrorFlag    ÿλ����һ������
/*enum
{
SAFETY_CHECK_ERROR_I = 0,
SAFETY_CHECK_ERROR_V,
SAFETY_CHECK_ERROR_P,
SAFETY_CHECK_ERROR_SCH,
SAFETY_CHECK_ERROR_SYS,
SAFETY_CHECK_ERROR_THIRD,
SAFETY_CHECK_ERROR_MAXVALUE,
};*/

enum
{
    SAFETY_V = 0,
    SAFETY_I,
    SAFETY_P,
    SAFETY_SCH,
    SAFETY_SYS,
    SAFETY_THIRD,
    SAFETY_MAXVALUE,
};

////////////////////////////////////////////////////
// following is the definitions that will be used in this section :checkitem.
#define NO_CHECK		0
#define PI_CHECK		1
#define V_CHECK			2
#define Hw_CHECK		4
#define PULSE_CHECK 8

// following defines the STATUS for StatusProcess() Status Machines.
#define SAFE		0
#define SAFE2UNSAFE	1
#define UNSAFE		2
#define UNSAFE2SAFE	4


//#if SUPPORT_PARALLEL == 1
#ifndef MXUNITNO
#define MAXUNITNO			40		//���40 unit/system
#endif
//#endif




//////////////////////////

// //Schedule ״̬
// enum
// {
// 	NOTREADY,
// 	IDLE,
// 	START,
// 	RUNNING,
// 	GOPAUSE,
// 	RESUME,
// 	GOSTOP,
// 	FIXEDUNSAFE,
// 	DYNAMICUNSAFE,
// 	WAITFORAUX,
// };



//��Ӧ�б�
enum
{
    //***********************************************
    //				Sch Mgr ��Ϣ����
    //***********************************************
    Sch_Assign_Msg=1,

    Sch_Stop_Msg,
    Sch_Stopped_Msg,

    Sch_Continue_Msg,

    Sch_Run_Msg,
    Sch_Started_Msg,

    Sch_Pause_Msg,
    Sch_JumpA_Msg,
    Sch_JumpM_Msg,
    Sch_Resume_Msg,

    Sch_LimitArrive_Msg,
//	Sch_LimitPropertyChange_Msg,
    Sch_LimitStatusChange_Msg,

    Sch_OnSafe_Msg,
    Sch_OnUnsafe_Msg,
    //Sch_OnUnsafe_Msg,
    //Sch_OnSafe_Msg,

    //***********************************************
    //				Step Mgr ��Ϣ����
    //***********************************************
    Step_Stop_Msg,
    Step_Run_Msg,
    Step_JumpA_Msg,
    Step_JumpM_Msg,

    //***********************************************
    //				Sch Mgr ��Ϣ��PC���
    //***********************************************
    PC_AssignRpt_Msg,
    PC_StopRpt_Msg,
    PC_Started_Msg,
    //PC_PauseRpt_Msg,
    //PC_JumpARpt_Msg,
    //PC_JumpMRpt_Msg,
    //PC_ResumeRpt_Msg,
    PC_LimitArrive_Msg,
    PC_LimitChange_Msg,
    //PC_UnsafeRpt_Msg,
    //PC_LimitStatusChanged_Msg,
    PC_Abnormal_Msg,
    PC_BackToNormal_Msg,
    PC_Unsafe_Msg,
    PC_Error_Msg,
    PC_Jump_Msg,
    PC_GoStop_Msg,
    PC_AcimRelayOn_Msg,
    PC_AcimRelayOff_Msg,

    SIMU_ASSIGN_MSG,   //12.22.2013ADD
    //************************************************
    //                  AUX ��Schedule��Ϣ����
    //************************************************
    AUX_Unsafe_Msg,
    AUX_ACKFAIL_Msg,
    Para_SubUnsafe_Msg,
    CAN_BMS_Unsafe_Msg,
    SMB_Lost_Unsafe_Msg,  //add by zc 12012017
};
//aux ��������
// enum
// {
// 	ORIG_DATA,
// 	DX_DT_DATA,
// 	D2X_DT2_DATA,
// 	DATA_TYPE_ERROR,
// };
enum {
    LOG_DELTA_NONE,
    LOG_DELTA_TIME,
    LOG_DELTA_META,
    LOG_DELTA_ALL,
};
enum
{
    LOG_TIME_1MS,
    LOG_TIME_10MS,
    LOG_TIME_20MS,
    LOG_TIME_50MS,
    LOG_TIME_100MS,
    LOG_TIME_1000MS,
    LOG_TIME_2000MS,
    LOG_TIME_5000MS,
};
enum
{
    SAFETYSN,
    LIMITSN,
};


typedef struct
{
    float         m_fEndTime;        //input
    float         m_fValue;          //input

    signed short  m_BufIdx;          //output
    unsigned char m_bEnded;
    unsigned long m_PointIdx;        //output

    bool          m_bValid;          //simulation is working?
    bool          m_bLogLimitValid;  //LogLimit�Ƿ������ã�
    bool          m_bCriticalLog;    //160425,add by hepeiqing,for huawei's request,log last point of simulation cycle.
    //ʹ��simulationģ������pulse���ܣ����±�����˶����ӣ�
    bool          m_bRepeat;          //���simulationҪ��cycle?
    bool          m_bExtraLog;        //���simulaiton�Ƿ���Ҫ���ж���LOG?
    bool          m_bLogCycle;       //���cycle��Ҫ����LOG��
    bool          m_bStageLogged;    //��stage���Ƿ�LOG�˵�һ���㣿
    bool          m_bStarted;        //Simulation������ô?
    float         m_fStartTime;      //Simulation��������ʼʱ��
    unsigned long m_LogInterval;     //ÿN��cycle����һ��LOG
    unsigned long m_CycleIdx;        //Cycle�����
    MsTime        m_CycleStartTime;  //simulation cycle����ʼʱ��
    MsTime        m_StageDacTime;    //Stage�ĵ�һ��DAC��ʼʱ��
    float         m_fCycleTime;      //ÿ��ѭ��������
} St_SimuRunInfo;

typedef struct
{
    float         m_fStgTime;        //input

    bool          m_bEnded;
    unsigned long m_CycleIdx;        //Cycle�����
    unsigned long m_StageIdx;        //Stage�����

    MsTime        m_CycleStartTime;  //simulation cycle����ʼʱ��

    float         m_fGain;           //�������ֵ����
    float         m_fOffset;         //�������ֵ����

    float         m_fBase;           //ѭ������ѭ��������DAC���ֵ�����ͻ��趨, һ��Ϊ0
} St_CvRunInfo;

typedef struct
{
    unsigned char AUX_CAN_ID;	   	//����ͨ��CANID��
    unsigned char AUX_Type; 	   	//����ͨ������
    unsigned short AUX_VirtualID;	//����ͨ���߼����
    unsigned short AUX_PhysicalID;	//����ͨ��������
    unsigned char Report;           //�ظ�ok or fail
} ST_IVMAP;
typedef struct
{
    unsigned char    m_LimitCheck:1;
    unsigned char    m_ReportValue:1;
    unsigned char    m_Log:1;
    //you can add other here!
    unsigned char    m_Updata:1;
    unsigned char    RVD:4;
} St_AUX_VALUE_BITS;

typedef union
{
    unsigned char      m_Char;    //�������񣬿��������︳ֵ0xFF;
    St_AUX_VALUE_BITS  m_Bits;
} St_AUX_VALUE;

typedef struct
{
    float aux_data;			//ԭʼ����
    float aux_dx_dt;		//��ʱ���һ�׵���
    float aux_d2x_dt2;  //��ʱ��Ķ��׵���
    //bool  updated;			//�����Ƿ����

    St_AUX_VALUE   updated_aux_data;
    St_AUX_VALUE   updated_aux_dxdt;
    St_AUX_VALUE   updated_aux_d2xdt2;
} ST_AUX_DATA;					//���ڴ�Ÿ���������

//typedef struct
//{
//	unsigned char IV_ChnIdx;
//	unsigned char Status;
//	unsigned short SN;
//
//}ST_LIMIT_CHANGE;
//�˽ṹ�����ݴ�CAN��������
typedef struct
{
    unsigned char AUX_CANID;
    unsigned char IV_ChnIdx;
    unsigned char Aux_Type;
    unsigned short Aux_ChnIdx;
    unsigned char Data_type;
    float         Aux_Data;
} ST_AUX_UPLOADATA;
typedef struct
{
    unsigned char m_IV_ChnNo;  	//IVͨ����
    unsigned char m_aux_type;		//��������
    unsigned short m_aux_virtual_chn; //��������ͨ����
    unsigned char m_data_type; //�������� orign or dxdt ..
    unsigned char m_single_or_repeat; //���λ����ظ�����
    unsigned  char    m_TimeDelta;      //ʱ��delta
    unsigned  char    m_MetaDelta;
} ST_REQUEST_AUXDATA;


typedef struct
{
    St_SchSafeData  m_AUXSafety;
} ST_AUXSAFETYCFG;

typedef struct
{
    unsigned char m_UnitId;
    unsigned short m_PhysicalChannelIdx;
} St_AUXchannelInfo;
typedef struct    //����PC��IV ����Ϣ�ṹ(��ȷ��)
{
    unsigned short m_Aux_UnitID[MAXAUXUNIT];
    unsigned short m_Start_InUnit[MAXAUXUNIT];   //ÿ��Unit����ʼͨ��
    unsigned short m_End_InUnit[MAXAUXUNIT];    //ÿ��Unit����ֹͨ��
    unsigned short m_ActualTotalAuxUnit;					//ʵ��unit ����
} ST_PC2IV_MAP;


// typedef struct    //����PC��IV ����Ϣ�ṹ   <<����ͨ����Ӳ�����ú�ͨ������>> ��4 �߼����
// {
// 	unsigned char m_IVCh_Index;     // �ڸ�unit�ڵ�IVͨ�����

// //	St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAXAUXTYPE][MAX_CH_IN_AUX_UNIT];  // һ��IVͨ���ֵ��ĸ���ͨ��  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
//     St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAXAUXTYPE][MAX_MAP_COUNT_PER_IV_CHAN];  // һ��IVͨ���ֵ��ĸ���ͨ��  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
// 	unsigned short m_Total_In_OneIVCh[MAXAUXTYPE];  // ͬ�ָ���������ͨ��������  	//--new add---������

// } St_PC2IVUnit4AuxMap;  //  From PC to IVUnit for (about) AuxMap
// ����ĳ��IVMCU�� ������ôһ���ṹ������ St_PC2IVUNIT4AuxMap    st_AllAuxMap [MAX_IVCH_PER_IVMCU]


//typedef struct    //����PC��IV ����Ϣ�ṹ(��ȷ��)
//{
////	unsigned char m_Aux_UnitID[MAXAUXUNIT];
////  	unsigned char m_Start_InUnit[MAXAUXUNIT];   //ÿ��Unit����ʼͨ��
////  	unsigned char m_End_InUnit[MAXAUXUNIT];    //ÿ��Unit����ֹͨ��
////	unsigned char m_PhysicalID[MAXAUXUNIT][MAX_AUXCH_INDEX];  //�����Ű���˳���������64��
//	St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
//	unsigned char m_ActualTotalAuxIndex;					//ʵ��Incex����
//	unsigned char m_AcutalIndexID[MAX_AUXCH_INDEX];         //�洢���ڼ�����������(VirtualID)
//} ST_INTRACANNET;
typedef struct
{
    St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
    unsigned short m_ActualTotalAuxIndex;					//ʵ��Incex����
    unsigned short m_AcutalIndexID[MAX_AUXCH_INDEX]; 		//�洢���ڼ�����������(VirtualID)

} ST_CANNET;


typedef struct
{
    St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
    unsigned short m_ActualTotalAuxIndex;					//ʵ��Incex����
    unsigned short m_AcutalIndexID[MAX_AUXCH_INDEX];         //�洢���ڼ�����������(VirtualID)
} ST_IV4AUX;
typedef struct
{
    unsigned char m_Aux_UnitID[MAXAUXUNIT];
    unsigned char m_Aux_UnitCANPort[MAXAUXUNIT];
    unsigned char m_Total_Aux_Unit;
} ST_AUXUNITFG;   //�洢�������Ͷ�Ӧ��Unit��Ϣ
typedef struct
{
    ST_CANNET  m_IntraCanNet;            //ÿ��ͨ����������
    ST_CANNET  m_OuterCanNet;			  //ÿ��ͨ����������
    ST_IV4AUX 		m_iv4aux;               //IV�ڲ���������
    ST_AUXUNITFG   m_AuxUnitCfg;				//Unit�����������Ϣ
    unsigned char   m_MapConfig;                //������Ϣ������λ����
} ST_IV_MAPCONFIG;


// typedef struct //dac infomation
// {
// 	unsigned char                m_bHaveVoltageDac;
// 	unsigned char                m_bHaveClampVdac;
// 	unsigned char                m_bChannelBasedClampVdac;

//   unsigned char                m_bCurrRngCount;
//   unsigned char                m_bVoltRngCount;

// 	unsigned long long           m_uSettingTime;
// }St_DacInfo;

/*typedef struct
{
signed long                  m_FragIdx;
signed long                  m_FragCount;
signed long                  m_ByteCount;
signed long                  m_PointCount;

unsigned char                m_bDownloading;  //�������أ�

St_MD5                       m_MD5;
}St_SimuDownMgr;*/
#pragma pack(1)
typedef struct
{
    unsigned long     Second;
    unsigned long     Us;
} TIME2US;
#pragma pack()
/*������ͨ��Metavariable������ؽṹ����*/
typedef union  //Un_ValueConversion
{
    double                       m_double;
    float                        m_float;
    MsTime						 m_Time;

    TIME2US                      m_Time2us;

    unsigned long long int       m_uInt64;
    signed long long int         m_sInt64;
    unsigned long                m_uInt32;
    signed long                  m_sInt32;
    unsigned short               m_uInt16;
    signed short                 m_sInt16;
    unsigned char                m_uInt08;
    signed char                  m_sInt08;

    unsigned char                m_ucData[16];
    unsigned short               m_u16Data[8];
    unsigned long                m_u32Data[4];
    unsigned long long           m_u64Data[2];
    float                        m_fData[4];
} Un_ValueConversion;
typedef struct //st_GetMetaValue
{
    float                        m_fValue;
    //	unsigned char                m_bNeedCalculate;   //���������Ҫ�����������ʹ��
    unsigned char                m_bNeedRecal;       //���������Ҫ���¼�����
    //float                        m_fReserved;  //���õ�����ֽ�
    Un_ValueConversion           m_Value;
} st_GetMetaValue;

typedef struct
{
    float                        m_fCurrent;
    float                        m_fVoltage;
    unsigned long                m_CurrA;
    unsigned long                m_CurrB;
    unsigned long                m_VoltA;
    unsigned long                m_VoltB;
    unsigned char                m_uCurrCount;
    unsigned char                m_uVoltCount;

    unsigned long                m_uCurrAfterFilter;
    unsigned long                m_uVoltAfterFilter;

    float                        m_fCurrentBeforeRC;
} St_IV_Buffer1;

//#if(SUPPORT_CAN_BMS==1)
typedef struct
{
    float                        m_fValue;
    unsigned char                m_bNeedCalculate;   //���������Ҫ�����������ʹ��
    unsigned char                m_bNeedRecal;       //���������Ҫ���¼�����
    unsigned char                m_bNeedLog;
    //float                        m_fReserved;  //���õ�����ֽ�
    Un_ValueConversion           m_Value;
} ST_BMS_MetaValue;

//#endif
typedef struct //PV_Info
{
    //   double                       m_dChargeCapacity;
    //   double                       m_dDischargeCapacity;
    //   double                       m_dChargeEnergy;
    //   double                       m_dDischargeEnergy;
    unsigned                     m_CCapacity:1;
    unsigned                     m_DCapacity:1;
    unsigned                     m_CEnergy:1;
    unsigned                     m_DEnergy:1;
} St_CapaEnerUpdated;

typedef struct //DV_Info
{
    MsTime                       m_LastLogTime;
    float                        m_fLastLogVoltage;
    float                        m_fLastLogCurrent;
} St_DV_Info;

typedef struct //St_TimerRegister
{
    double                         m_dResult;
    MsTime                         m_StartTime;
} St_TimerRegister;

typedef struct //St_CapacityRegister
{
    double                         m_dResult;
    double                         m_StartCapacity;
} St_CapacityRegister;

typedef struct //St_TimeBitStatus
{
    unsigned                       TestTime:1;
    unsigned                       StepTime:1;
    unsigned                       Timer1:1;
    unsigned                       Timer2:1;
    unsigned                       Timer3:1;
    unsigned                       Timer4:1;
} St_TimeBitStatus;
typedef union  //Un_TimeStatus
{
    St_TimeBitStatus               m_Bits;
    unsigned char                  m_Char;
} Un_TimeStatus;

typedef struct //St_CapaEnerBitStatus
{
    unsigned                       Capa1:1;
    unsigned                       Capa2:1;
} St_CapaEnerBitStatus;
typedef union  //Un_CapaEnerStatus
{
    St_CapaEnerBitStatus           m_Bits;
    unsigned char                  m_Char;
} Un_CapaEnerStatus;

typedef struct //St_IsampBitStatus
{
    unsigned                       Iset:1;
    unsigned                       NormalizedI:1;
    unsigned                       CalCapacity:1;
    unsigned                       DacOutput:1;
    unsigned                       SimuSamp:1;
} St_IsampBitStatus;
typedef union  //Un_IsampStatus
{
    St_IsampBitStatus              m_Bits;
    unsigned char                  m_Char;
} Un_IsampStatus;

typedef struct //St_VsampBitStatus
{
    unsigned                       Iset:1;
    unsigned                       NormalizedV:1;
    unsigned                       CalCapacity:1;
    unsigned                       DacOutput:1;
    unsigned                       SimuSamp:1;
} St_VsampBitStatus;
typedef union  //Un_VsampStatus
{
    St_VsampBitStatus              m_Bits;
    unsigned char                  m_Char;
} Un_VsampStatus;

typedef struct //TC_Info
{
    //unsigned  long                 m_u32Counter1;
    //unsigned  long                 m_u32Counter2;
    //unsigned  long                 m_u32Counter3;
    //unsigned  long                 m_u32Counter4;

    St_TimerRegister               m_Timer1;
    St_TimerRegister               m_Timer2;
    St_TimerRegister               m_Timer3;
    St_TimerRegister               m_Timer4;

    St_CapacityRegister            m_ChargeCapacity1;
    St_CapacityRegister            m_ChargeCapacity2;
    St_CapacityRegister            m_DischargeCapacity1;
    St_CapacityRegister            m_DischargeCapacity2;
    St_CapacityRegister            m_ChargeEnergy1;
    St_CapacityRegister            m_ChargeEnergy2;
    St_CapacityRegister            m_DischargeEnergy1;
    St_CapacityRegister            m_DischargeEnergy2;

    //Un_TcStatus                    m_Status;      //�����ȥ����Ϊ�������ﲻ��ͳһ����memset��λ.
} St_TC_Info;
typedef struct //St_Metavariable
{
    //St_PV_Info                         m_PV_Info;      //Present Value
    //St_PV_Info                         m_LS_Info;      //Last Step value
    //St_PV_Info                         m_LC_Info;      //Last Cycle value
    //St_PV_Info                         m_LL_Info;      //Last Log value//����ʱ����
    St_TC_Info                         m_TC_Info;      //Test Counter value
    St_DV_Info                         m_DV_Info;      //DV time/voltage/current//����ʱ����
} St_Metavariable;

/*������ͨ��Step������ؽṹ����*/
typedef struct //St_StepRunningInfo
{
    /*unsigned char m_bIfMyLimit;
    unsigned char m_bIfMyLimit_Reached;*/
    unsigned char           bSpecialStepLimit;               //this is a special step?,such as IR
} St_StepRunningInfo;
typedef struct //St_StepInfo
{
    St_StepData             m_Data;          //�����ڴ�schedule���ݿ�ֱ�� �ڴ濽�� ������ֵ
    St_StepRunningInfo      m_RunningInfo;   //��λ��ά��������ʱ����
} St_StepInfo;



/*������ͨ��Simulation������ؽṹ����*/
// typedef struct //St_NormalSimuPoint
// {
//     float                   m_fTime;
//     float                   m_fValue;
// }St_NormalSimuPoint;
// typedef struct //St_BatterySimuPoint
// {
//     float                   m_fCapacity;
//     float                   m_fVoltage;
//     float                   m_fDCR;
// }St_BatterySimuPoint;
// typedef union  //Un_SimulationPoint
// {
//     St_NormalSimuPoint   m_Normal[IV_NORMAL_SIMULATION_POINT_NUMBER];
//    // St_BatterySimuPoint  m_Battery[IV_BATTERY_SIMULATION_POINT_NUMBER];
// }Un_SimuPoints; //12.22.2013ADD

//typedef struct //St_SimulationInfo     //12.22.2013ADD
//{
//    unsigned long           m_u32PointIdx;       //��ǰ����ʹ�õ�"��"���
//    signed char             m_scSimuBufIdx;      //ָ��ǰʹ�õ�Simulation Buffer�����ָ��
//    unsigned char           m_ucReserved[2];
//}St_SimulationInfo;

/*������ͨ��Formula������ؽṹ����*/
typedef struct //St_FormulaInfo
{
    St_FormulaData          m_Data;

    //����ΪFormula����ʱ����
    signed char             m_scID;
    float                   m_fLastCalcValue;
} St_FormulaInfo;
typedef union
{
    struct
    {
        float x1;
        float x2;
        float x3;
        float x4;
        float y1;
        float y2;
        float y3;
        float y4;
    } fX;
    float f_Arry[8];
} UN_Formula_fx;

//typedef struct
//{
//	UN_Formula_fx fX;
//	unsigned char Refreshed[8];
//}ST_Formula_fX;

/*������ͨ��CV������ؽṹ����*/
typedef struct //St_CvInfo
{
    St_CvData               m_CvData;

    //��������CV����ʱ����
} St_CvInfo;

typedef struct //St_PulseInfo  Add by DKQ 08.20.2014
{
    St_PulseData          m_Data;

    //����ΪPulse����ʱ����
    signed char           m_PulseID;

} St_PulseInfo;

/*������Schedule Buffer������ؽṹ����*/
typedef struct //St_StepBuf
{
    St_SMBBlockData  m_SMBBlockData;
} St_SMBBlockInfo;
typedef struct //St_StepBuf
{
    St_StepInfo             m_StepInfo;
    //St_SimulationInfo       m_SimulationInfo;      //12.22.2013ADD
    St_FormulaInfo          m_FormulaInfo;        //99%����;һ��֮��ֻ��1��formula,����ж��formula��Ҫ����ʱload
    St_PulseInfo            m_PulseInfo;
    St_CvInfo               m_CvInfo;
    St_SMBBlockInfo         m_SMBBlockInfo;
    MsTime                  m_StartTime;          //step start time

    //����ı�����ʱ��ʵ�֡�

} St_StepBuf;  //�ṩStep taskʹ��
typedef struct //St_LimitInfo
{
    int                     m_SN;

    unsigned char      m_bAuxCheck:1;
    unsigned char      m_bAnyLimit:1;
    unsigned char      m_bReached:1;
    unsigned char      m_bChecked:1;
    int                m_AnyLimitReachedCount;    //��Щ������Ȼ����λ������ʱ����������Ϊ�˷��������д��Э���ļ����ˡ�

} St_LimitInfo;
typedef struct //St_ChannelStep
{
    St_StepBuf              m_Info[2];            //����Info������һ��ΪBackground Info����һ��ΪFront Info�������л�ʹ��

    unsigned char           m_ucActiveIdx;        //��ǰ��Info��Index
    unsigned char           m_bIsDvInterval;      //������ʶ��step�Ƿ�ֻ��һ��Dv_time>x��loglimit���ã��������������Ҫ���⴦��
    unsigned char           m_bIsFirstLogPoint;   //��ʶ�Ƿ��һ��LogPoint;
    //unsigned char           m_bSdlEnabled;
    float                   m_fLogInterval;       //��¼log interval
    unsigned long long      m_u64LogInterval;

    MsTime                  SysTimeStepTask;	//0819
    MsTime                  MsTimeStepTask;

    unsigned char           m_LimitCount;
    unsigned char           m_StepLimitCount;
    unsigned char           m_LogLimitCount;
    St_LimitInfo            m_limitInfo[IV_MAX_LIMIT_NUMBER];
    //	SysTime                 SysTimeLimitCheckTask;  //dirui add for Limit delay 0819
    MsTime                  MsTimeLimitCheckTask;    //dirui add for Limit delay
} St_ChannelStep;
typedef struct //St_ChannelSchedule
{
    St_MD5                  m_ID;

    MsTime                  m_StartTime;    //schedule start time;

    signed char             m_scBufIdx;
    unsigned char           m_bResetTestTime;

    St_ScheduleSetting      m_Settings;
    St_SchTestObject        m_TestObject;

    St_ScheduleItemCount    m_ItemCount;

    unsigned short          m_NextStepIdx;
    unsigned short          m_RunningStepIdx;

    //    SysTime    SysTimeScheduleTask;	0819
    MsTime    MsTimeScheduleTask;
} St_ChannelSchedule;
typedef struct
{
    unsigned char m_SafetySNIndex;
    unsigned char m_LimitSNIndex;
} ST_SNINDEX;
//#if SUPPORT_PARALLEL == 1
//
//Grouping information and settings:
//
typedef enum //ͨ��ģʽ
{
    //	eChannelMode_Normal = 0, // default. single channel����ͨ������
    eChannelMode_Master = 0, // master in parallel, defualt, even for single channel running status (group size is 1)����������Ϊ��ͨ����group�ĵ�һ��ͨ������
    eChannelMode_Submaster, //interunit parallel, first slave channel in other-than-master unit����unit����ʱ����unit�ĵ�һ��ͨ��
    eChannelMode_Slave //slave in paralle, other than master and submaster����ͨ��
} En_ChannelMode;




typedef struct
{
    unsigned char m_Chn_Rank;    // =0 (Master); 1 (Sub Master) ; 2(Slave)
    unsigned char m_Group_Size;  //�����master�Ļ����Լ���group size,�Ա����ڷֵ���
    ST_UNIT_N_CHN m_Supervisor;  //��ͨ�����ϼ�ͨ��; �����Master���ϼ�ͨ�������Լ��������SubMaster���ϼ�ͨ������master������� slave���ϼ�ͨ������ Sub master��
    ST_UNIT_N_CHN m_SubMasterChannels[MAX_UNIT_PER_GROUP];  //�����master�Ļ����Լ���sub masterͨ��λ��
    unsigned char m_SlaveChnnels [MAX_CHN_PER_UNIT];  //�����submaster�Ļ����Լ�������slaveͨ��
} ST_EXTENDED_CHN_INFO;






typedef struct
{
    //	En_GroupMode	m_eGroupMode; //intra unit para/inter unit para/serial/booster����ʲôģʽ��group
    unsigned char 	m_eGroupMode; //intra unit para/inter unit para/serial/booster����ʲôģʽ��group
    En_ChannelMode m_eChannelMode; // channel is normal/master/submaster/slaveͨ������ģʽ
    unsigned char m_uMasterUnitIdx;//��ͨ������unit�š�Submaster report data��Ҫ�� submaster����CAN�ϵĲ������������Ҫ��
    unsigned char m_uMasterChanIdx;//��ͨ����unit�ڵ�ͨ���š��Ա�ȵ��������һ��ͨ�������㣨С����ͣ���Ҫ֪�����ĸ���ģ���Ҫ���unit�ڲ������������unit��������ֵΪ0����һ��ͨ����.
    unsigned char m_uGroupChanCount;  //default 1. number of channels in the group��Group�м���ͨ����
} St_GroupingInfo;

//
// group data
//
typedef struct {

    bool		m_bGroupNewCodeSampled;

    bool		m_bGroupCurrentSampleFlag_SafetyCheck;
    bool		m_bGroupVoltageSampleFlag_SafetyCheck;

    bool		m_bGroupCurrentSampleFlag_LimitCheck;
    bool		m_bGroupVoltageSampleFlag_LimitCheck;

    //�����������
    float 		m_fGroupCurrent;
    float 		m_fGroupVoltage;
    float 		m_fGroupPower;
    float 		m_fGroupCCapacity;
    float 		m_fGroupDCapacity;
    float 		m_fGroupCEnergy;
    float 		m_fGroupDEnergy;

    //	float 		m_fGroupCapapcityTime;

} St_GroupData;

enum
{
    FLAG_TYPE_PARALLEL_NONE = 0x0,
    FLAG_TYPE_PARALLEL_FRESHSAMPLECODE = 0x1,
    FLAG_TYPE_PARALLEL_SAFETYCHECK = 0x2,
    FLAG_TYPE_PARALLEL_LIMITCHECK = 0x4,
    FLAG_TYPE_PARALLEL_8 = 0x8,
    FLAG_TYPE_PARALLEL_WHATEVER = 0xFF
};  //use bit


//
//parallel group ctrl
//
typedef struct
{
    float m_fGroupChandIdt;
    float m_fGroupChandVdt;

    float m_fGroupChanCurrent;
    float m_fGroupChanVoltage;

    float m_fGroupChanCapacity;
    float m_fGroupChanEnergy;

} St_GroupDetalValue;


typedef struct {

    unsigned char 		m_bCurrentRangeParallelHigh;  //paralllel high or not? Tell slave to set to high or off, when master is current high.

    unsigned long			m_ulGroupChanCtrlCode[MAXCHANNELNO];   //DACֵ

    unsigned char m_uGroupChanCtrlType[MAXCHANNELNO];
    float m_fGroupChanCtrlValue[MAXCHANNELNO];

    float m_fGroupChanCurrRampDelta[MAXCHANNELNO];
    float m_fGroupChanVoltRampDelta[MAXCHANNELNO];
    St_GroupDetalValue m_GroupDeltaValue[MAXCHANNELNO];

    unsigned char		m_ucEngineHandler[MAXCHANNELNO];

} St_GroupCtrl;


//��unit�����Ż��õ���С�����ݴ洢��
typedef struct {
    //��ʾ����������//sub unit�������±�־
    //set T when received corr. unit CAN pack's value
    //TODO: when to reset??
    bool m_bSubGroupChanVoltageUpdated;
    bool m_bSubGroupChanCurrentUpdated;
    bool m_bSubGroupChanCCapacityUpdated;
    bool m_bSubGroupChanDCapacityUpdated;
    bool m_bSubGroupChanCEnergyUpdated;
    bool m_bSubGroupChanDEnergyUpdated;

    float m_fSubGroupChanVoltage;  //�ڲ����쳣������£���Ϊ��ͨ����ѹ�������ѹ --> �����쳣���
    float m_fSubGroupChanCurrent;
    float m_fSubGroupChanCCapacity;  //����һ��limit�ж���ʹ�õ�capacity��ʱ��Ҫ�󲻸ߣ�ʹ��current���Ƽ��㡣
    float m_fSubGroupChanDCapacity;
    float m_fSubGroupChanCEnergy;
    float m_fSubGroupChanDEnergy;

} St_SubGroupData; //��Ҫ����unit����ʹ�ã����ÿ��subgroup��С������


typedef struct
{
    unsigned short m_uSubGroupUnitID;

    unsigned short m_uSubGroupCtrlType;

    float	m_fSubGroupCtrlValue;

    unsigned char m_ucCANCMDType;

    bool m_bSubGroupCtrlUpdated;

} St_SubGroupCtrl;

//unit��ͨ������״̬��
typedef enum
{
    //eUnitMode_Normal,
    //eUnitMode_Para_IntraUnit,
    eUnitMode_Para_Master,
    eUnitMode_Para_Submaster,
    eUnitMode_Para_Slave
} En_UnitChannelMode;

typedef struct
{
    bool m_bGroupCheckSafety;

} St_GroupStatusInfo;

typedef struct
{

    unsigned short m_uUnitID;

    //	En_GroupMode		m_eUnitGroupingMode; // normal, intra-parallel, inter-parallel
    //	unsigned char 	m_eUnitGroupingMode; // normal, intra-parallel, inter-parallel
    //	En_UnitMode 	m_eUnitMode;  //single channel running, intra-unit paralle, inter-unit parallel master unit, inter-unit paralle submaster unit.
    //	bool 					m_bMasterUnit; //bool
    //	unsigned char	m_uMasterUnitID;
    //	unsigned char 	m_uGroupUnitCount;
    ST_UNIT_GROUP_INFO  m_UnitGroupInfo;
    St_GroupStatusInfo	m_stGroupStatusInfo;
    bool				m_bSubGroupDataValid[MAXUNITNO]; // �Զ�unit����������С���Ƿ�����С��͸��¡���CAN����report data��T,����������ȫ����0.
    //���ڱ�ʶsubgroup�ڻ�������������Ч����
    //�͵�ͨ��ģʽ��ChannelInfo_Manager::ChannelInfo[uChanIdx].Current_Sampleflg �ȼ�
    St_SubGroupData		m_stSubGroupData[MAXUNITNO];
    St_SubGroupCtrl		m_stSubGroupCtrl;

} St_GroupInfo;

typedef struct
{
    St_GroupInfo  m_GroupInfo[MAX_GROUP_PER_UNIT];
    unsigned char m_GroupCount;
    unsigned char m_MyUnitID;
    unsigned char m_InterMasterGroupID; //��Ϊ��unitʱ�������ĸ�������
    unsigned char m_SubMasterGroupID;
    unsigned char m_ParaMode;
    unsigned char m_Series2BoosterUnitCounter;
} St_UnitInfo;

typedef struct
{
    ST_UNIT_N_CHN		m_SubMasterChannels;
    float           ControlVoltage;
    float           OutputVoltage;
    bool            IsNeedCheckRunState;//0:����Ҫ���,1:��Ҫ��顣
// 	bool            RunState;//0:ֹͣ,1:���С�
// 	MsTime          CheckPeriod;
} St_Serial2BoosterOutInfo;

typedef struct
{
    bool     m_bStartCtrl;
    float    m_MasterCurrent;
    MsTime   m_MaskCtrlTick;
} St_Para_FeedBackCtrl;
//#endif //SUPPORT_PARALLLEL
typedef struct //St_ChannelInfo
{
    bool    IsOpenClampRange;
    BYTE    clampHighControlType;
    BYTE    clampLowControlType;
    float   clampHigh;
    float   clampLow;
} St_ClampRange; //1712211 zyx
typedef struct //St_ChannelInfo
{
    //SDL:
	bool                    m_bNoNeedFineTune_flag;		//20190808 dyy
    bool                    m_bDynamicControl;
//     unsigned char           m_bNeedFilter;
    float                   m_fResumeTestTime;
    float                   m_fResumeStepTime;
    float                   m_fResumeTC_Timer1;//zyx 211101
    float                   m_fResumeTC_Timer2;
    float                   m_fResumeTC_Timer3;
    float                   m_fResumeTC_Timer4;	
    unsigned long           m_u32VoltageAdc;
    unsigned long           m_u32CurrentAdc;
    unsigned long           m_u32BuildInVolt[MAX_BUILDIN_2V_COUNT];
    unsigned char           m_bBuildInVoltNewCode[MAX_BUILDIN_2V_COUNT];
    unsigned long           m_u32BuildInVolt_Pulse[MAX_BUILDIN_2V_COUNT];
    unsigned char           m_bBuildInVoltNewCode_Pulse[MAX_BUILDIN_2V_COUNT];
    float                   m_fBuildInVolt_Pulse[MAX_BUILDIN_2V_COUNT];
    float                   m_fBulitInData[MAX_BUILTIN_COUNT];  //����BUILTIN ��������
    unsigned long           m_u32CurrentRampDelta;
    unsigned long           m_u32VotlageRampDelta;
    float                   m_fCurrentRampDelta;  //����ר�� 20150717
    //SDL^

    float                   m_fNormalizedVoltage;
    float                   m_fNormalizedCurrent;

    unsigned long           m_u32DacValue;
    float                   Clamp_Value;
    unsigned char           Clamp_Direction;
    float                   m_fCurrent;
    float                   m_fVoltage;
    unsigned char           m_ucCurrentRange;
    unsigned char           m_ucVoltageRange;
    unsigned char 			m_ucSyncStop:1;			//�¿�����SCHͬ��ֹͣ��־
    unsigned char			m_ucNone:7;					//Ԥ�� reserved
    unsigned char           m_ucStatus;
    unsigned char           m_bScheduleRunning;
	unsigned char           m_bThirdParty_FineTuningDone;  //190917,hpq
	unsigned char           m_bThirdParty_FineTuning;  //190917,hpq
	float                   m_fThirdParty_SetCurrent;  //190917,hpq
	float                   m_fThirdParty_FineTuningLastOffset;  //190917,hpq
	MsTime                  m_tLastFineTuningTime;     //190917,hpq

    unsigned long           m_u32RelayStatus;

    St_IndependantCtrl      m_IndependantCtrl;

    float                   m_fCurrentRangeOfPid; //1904031 zyx
    float                   m_fVoltageRangeOfPid;
    ///////////////////////add for schedule manager

    unsigned char   m_CtrlState;
    unsigned char   ScheduleStatus;
    unsigned char   StepStatus;
    unsigned char   m_UnsafeType;
    unsigned long int   m_SafetyStatus;
    unsigned long int   m_AUXSafetyStatus;  //dirui add 20131120
    unsigned char 	StepLimitProcess;  //dirui add 20130122
    unsigned char   LogLimitProcess;  //dirui add 20130122
    unsigned char   m_Event_Source;		//dirui add 0917
    unsigned char   Current_Sampleflg;  //dirui add 0917
    unsigned char   Voltage_Sampleflg;	//dirui add 0917
    unsigned char   m_bCVstage;         //hepeiqing add 20140327,in CV stage? for CCCV step


    WORD   Report_LimitID;     //dirui add 2013 0924
    WORD   Report_EquationID;  //dirui add 20130924

    unsigned char   EngineStatus;       //����״̬  XXXXXX Error running    1���� ��Ч    0��Ч    comment out chen 20130123
    unsigned char   StartedReportNumber;
    unsigned char   StepTransationToStatus;
    unsigned char   DigitCtrl;
    unsigned char   Adjustment;
    unsigned char   DacReverse;
    unsigned char   SpecialProcess;
    unsigned long   ResumeTime;

    unsigned char   bNeedDacOutput;    //20130925,add by hepeiqing,Control Dac Output frequency
// 	unsigned char   bDigitalI;    //20131118 add    ���ֵ���
    unsigned char   bDigitalV;

    unsigned char ScheduleUnsafeFlg;
    unsigned char chn_Running_Flag;    //idle sch or ������    comment out chen 20130513

    //float           m_fCapacity;   //  ��ض����
    //float           m_fMass;
    //float           m_fSpecificCapacity;
    float           m_fVclampHigh;
    float           m_fVclampLow;
    float           m_fCurrentSimulationVclampHigh;      //09.25.2017 add by zc
    float           m_fCurrentSimulationVclampLow;				//09.25.2017 add by zc
    float           m_fPowerSimulationVclampHigh;      //10.04.2017 add by zc
    float           m_fPowerSimulationVclampLow;				//10.04.2017 add by zc
    //float           m_fNominalVoltage;  // ��(��)����ѹ
    BYTE               m_DataType_DigiVclampHigh;               //171016 hpq
    BYTE               m_DataType_DigiVclampLow;                //171016 hpq
    St_ClampRange   m_CurrentClampRange;    //1712211 zyx
    St_ClampRange   m_PowerClampRange;  //1712211 zyx
	St_SafetyRedundantVotlage   m_RedundantVotlage;  // 20190723 yy

    unsigned char chn_State;    //����ת��״̬    add by chen
    unsigned char   m_bCaliBoard;
    MsTime          m_LastCaliTime;    //��ֹУ׼ʱ���������Ϊһ���У׼ֻ����м�����

    St_SimuRunInfo  m_SimuInfo;
    St_CvRunInfo    m_CvInfo;

    //AUX ���20131120
    // 	unsigned char  IVCANID;
    // 	unsigned short AUXTYPE;     //��ͨ�����ж����ָ���ͨ��������bit��ʾ
    // 	unsigned char AuxTypeChannelCount[MAXAUXTYPE];  //���ָ���ͨ��������
    ST_SNINDEX m_AUX_SN_Index;
    //#if( SUPPORT_PARALLEL == 1)
    //	St_GroupingInfo		m_GroupingInfo;				//������Ϣ
    unsigned char       m_GroupID;
    St_GroupCtrl 		m_GroupCtrl;			//��������
    St_GroupData		m_GroupData;			//��������
    ST_EXTENDED_CHN_INFO  Extended_Chn_Info;
    //#endif

    //St_Grp    m_GrpInfo4SDL;
} St_ChannelInfo;



typedef struct //St_ScheduleBuffer
{
    unsigned char LimitCount;
    unsigned char IfAnyLimitCount;

    unsigned short StepID;
} St_AuxLimitStatus;


typedef struct //St_ScheduleBuffer
{
    St_MD5               m_ID;

    MsTime               m_AbandonTime;

    unsigned char        m_bValid;
    unsigned char        m_ucReserved1[3];

    unsigned long        m_u32IvRegister;      //���֧��32��ͨ��ע��
    //unsigned char        m_ucIvCount;        //��ʾ��N��IVͨ����ʹ�����Schedule;  //ʹ��bit��ʾһ��ͨ���Ƿ�register
    unsigned char        m_ucMacCurrentRange;  //��scheduel�����ĵ�������

    //����Ϊ����ʱ�������������ڴ濽��

    St_ScheduleSetting   m_Setting;
//	St_SchTestObject     m_TestObject;//171113 hpq add for pro8

    St_ScheduleItemCount m_ItemCount;

    St_StepData          m_StepData[IV_MAX_SCHEDULE_STEP_NUMBER];
    St_StepData        m_StepData_ExtraIR;  //170721,PeiqingHe,��Ҫ��important!���д��벻�ܱ�ɾ����
    St_FormulaData       m_FormulaData[IV_MAX_SCHEDULE_FORMULA_NUMBER];
	St_AdvFormula       m_AdvFormula[MAX_ADVFORMULA_COUNT];	
    St_PulseData         m_PulseData[IV_MAX_PULSE_BUFFER_NUMBER];
    St_TestSetting       m_TestSetting[MAX_TEST_SETTING_COUNT];
    St_CvData            m_CvData[IV_MAX_SCHEDULE_CV_NUMBER];
    St_SMBBlockData      m_SMBBlockData[MAX_SMB_BLOCK_CONTENT_COUNT];
} St_ScheduleBuffer;

/*****Simualtion Buffer******/
// typedef struct //St_SimulationBuffer
// {
// 	St_MD5               m_ID;
// 	MsTime               m_AbandonTime;
// 	unsigned char        m_bValid;
// 	unsigned char        m_bIsBatterySimu;       //�Ƿ�ΪBattery Simulation
// 	signed char          m_scSimuBufferIdx;
// 	unsigned long        m_u32IvRegister;        //���֧��32��ͨ��ע��
// 	unsigned long        m_u32PointNumber;       //��Ϊ��buffer�ĵ���Ŀ
// 	Un_SimuPoints      	 m_Data;
// }St_SimulationBuffer; //12.22.2013ADD


/*������Pulse Buffer������ؽṹ����*/
typedef struct
{
    St_MD5               m_ID;

    MsTime               m_AbandonTime;

    unsigned char        m_bValid;
    //unsigned char        m_ucIvCount;            //��ʾ��N��IVͨ����ʹ�����Schedule;
    unsigned char        m_ucReserved1[3];

    unsigned long        m_u32IvRegister;        //���֧��32��ͨ��ע��
    //����Ϊ����ʱ�������������ڴ濽��

    St_PulseData         m_Data;
} St_PulseBuffer;

/*������config������ؽṹ����*/
typedef struct //St_ConverterCharacter
{
    unsigned long       m_u32Mid;
    unsigned long       m_u32Max;
} St_ConverterCharacter;
typedef struct //St_BoardCFG
{
    St_BoardGlobalCFG   m_Global;

    St_ChannelCFG       m_Channel[MAXCHANNELNO];

    //����Ϊ���س�ʼ�����ݣ�

    unsigned char chn_Running_Flag2;
} St_BoardCFG;

typedef struct
{   unsigned char  		m_valid;   //�ð��Ƿ���Ч���
    ST_ABI_GLOBLE 			m_abi_globe;   //ȫ����Ϣ
    ST_ABI_CHN 				m_abi_chn[32];  //���128��,���ǵ�Addon��һ������32������ذ���96��
    ST_ABI_ASSEMBLE  	m_assemble;  //���ڻ����е������Ϣ
    unsigned short     m_RSN;     //����AUXMC�е�ע����Ϣ
} ST_BUILTIN_BOARD_INFO_IN_EEPROM;

typedef struct
{
    unsigned char  		m_valid;   //�ð��Ƿ���Ч���
    ST_ABI_ASSEMBLE    m_assemble;  //���ڻ����е������Ϣ
    ST_ABI_GLOBLE 			m_abi_globe;   //ȫ����Ϣ
    unsigned short     m_RSN;     //����AUXMC�е�ע����Ϣ
    unsigned char m_Start_InUnit; // Add Nov 25, 2013  ���ڴ洢�����ð���ͨ���ڸ���unit�ڵ�ͨ�����
    unsigned char m_End_InUnit;		//Add Nov. 25,2013

} ST_AUX_BOARD_INFO_SAVE;    //18�ֽ�

typedef struct //St_WindowFilter
{
    unsigned long                                                m_uBuf[IV_SAMP_VOLTAGE_FILTER_DEPTH];
    unsigned short                                               m_uHandler;

    unsigned short                                               m_uFilterDepth;

    unsigned long int                                            m_u32Sum;

    unsigned long int                                            m_u32LastValue;
} St_WindowFilter;
/*History Data Struct*/
typedef struct //St_HistoryData
{
    //�˲���ȿɶ�̬���ã����ӽӿ�
    //CIRCULAR_QUEUE(unsigned long,IV_SAMP_CURRENT_FILTER_DEPTH)   m_Current;    //������ʷ�������� ѭ������
    // CIRCULAR_QUEUE(unsigned long,IV_SAMP_VOLTAGE_FILTER_DEPTH)   m_Voltage;    //��ѹ��ʷ�������� ѭ������

    unsigned long                                                m_uCurrBuf2[IV_SAMP_CURRENT_FILTER_DEPTH];
    unsigned long                                                m_uVoltBuf2[IV_SAMP_VOLTAGE_FILTER_DEPTH];
    unsigned short                                               m_uCurrHnd;
    unsigned short                                               m_uVoltHnd;

    unsigned short                                               m_uCurrFilterDepth;
    unsigned short                                               m_uVoltFilterDepth;

    unsigned long int                                            m_u32CurrentSum;
    unsigned long int                                            m_u32VoltageSum;

    unsigned long int                                            m_u32LastCurrentValue;
    unsigned long int                                            m_u32LastVoltageValue;

    St_WindowFilter                                              m_BuildInVolt[MAX_BUILDIN_2V_COUNT];
} St_HistoryData;

typedef struct //St_HistoryData
{
    unsigned long                                                m_uBuf2[IV_SAMP_CURRENT_FILTER_DEPTH];
    unsigned short                                               m_uHnd;

    unsigned short                                               m_uFilterDepth;

    unsigned long int                                            m_u32Sum;

    unsigned long int                                            m_u32LastValue;
} St_HistoryData_ACR;

/*Message Define For Tasks*/
typedef struct //St_FramMessage
{
    unsigned char   m_bIsWrite;
    unsigned char   m_bPID;       //�Ƿ���PID����
    unsigned short  m_u16ChannelIdx;
#ifdef REPORT_FRAM_EVENT
    WORD               m_wChannelBitIdx;
    WORD               m_wChannelCount;
    WORD               m_wFramType;
    MsTime             m_MsTime;
#endif
    unsigned short  m_u16Length;
    unsigned long   m_StartAddress;
} St_FramMessage;

typedef struct
{
    unsigned char LimitPropertyChange;
    unsigned short LimitID;
    unsigned short EquationID;
} St_ScheduleRunMsg;
typedef union
{
    St_Assign      m_Assign;
    St_ScheduleRunMsg m_RunMsg;

    //Di rui: add other schedule message here;

} Un_ScheduleMessage;
typedef struct
{
    unsigned char MsgPriority;
    unsigned char MsgNumber;
    //unsigned char LimitArriveNumber;
    unsigned short StepID;
    unsigned short FromStepID;
    unsigned long  CycleID;
    unsigned long  FromCycleID;  // ����
    unsigned char  ForceJumpFlag;   // ����
    //unsigned short LimitID;
    //unsigned short EquationID;
    unsigned short GotoStepID;
    //unsigned char LimitPropertyChange;
    //...
    Un_ScheduleMessage  m_Message;
    unsigned char m_btCMD;
    ST_EVENT_SCHEDULE_RUNNING st_status_sch_running;
} St_ScheduleMessage;

typedef struct
{
    unsigned char MsgNumber;

    unsigned char StepID;
    unsigned short ToStepID;
    unsigned short FromStepID;
//	unsigned short CircleID;    dirui del 2018/12/18

    unsigned short LimtID;
    unsigned short EquationID;
    //...
} St_StepMessage;  //����Ҫͳһ����������

// typedef struct //St_TaskMessages           dirui del 2018/12/18
// {
// 	CIRCULAR_QUEUE(St_FramMessage,IV_FRAM_MESSAGE_DEPTH)            m_Fram;
// 	CIRCULAR_QUEUE(St_ScheduleMessage,IV_SCHEDULE_MESSAGE_DEPTH)    m_Schedule;

// 	//add other task message here;
// }St_TaskMessages;

typedef struct
{
    float         m_fHigh;
    float         m_fLow;

    unsigned char m_bIfCheck;
} St_SaftyCheckItem;

typedef struct
{
    St_SaftyCheckItem       checkSYS_V;
    St_SaftyCheckItem       checkSYS_I;
    St_SaftyCheckItem       checkSYS_P;
    St_SaftyCheckItem       checkSCH_V;
    St_SaftyCheckItem       checkSCH_I;
    St_SaftyCheckItem       checkSCH_P;
    St_SaftyCheckItem       checkSCH_C; //step Capacity check zyx 180521
    St_SaftyCheckItem       checkSCH_t; //step time check zyx 180521
} St_Safty;
typedef struct
{
    unsigned char   m_bIfCheckFlag;     //0:����Ҫ��飬  1����ѹ  2���¶�
    St_SaftyCheckItem       checkSCH_Aux_V;
    St_SaftyCheckItem       checkSCH_Aux_T;
} St_BuiltinSafety;

typedef struct
{
    unsigned char   m_ChannelIdx;
    unsigned char   m_LimitIdx;
    unsigned short  m_StepIdx;
    St_MD5          m_SchMd5;
} St_LimitPointer;

typedef struct
{
    float           m_fStartTime;
    float           m_fMinError;
    float           m_fStep;
    float           m_fMaxAdjust;
	float           m_fParaWaitTime;  // add by qjm  ���Ӳ���ʱ�ȴ���ͨ�����ݸ��µ�ʱ�䣬 ��128�˲���ȼ���
	float           m_f_CurCoarseTuneStartTime;
} St_CurFineTuneFactor;

typedef struct
{
    MsTime          m_OutputInterval;// fine tune output interval, current output would be shock if fine tune control too quick.
    float           m_LastOutput;
    float           m_LastSetValue;
    float           m_SampleValue;
    float           m_TotalAdjust;
    float           m_Err;

    float           m_fMinError;       //΢����������С���
    float           m_fStep;           //ÿ��΢���ķ���
    float           m_fMaxAdjust;      //���΢������
    float           m_fSetValue;
} St_CurFineTune;   //Add by DKQ on 06.03.2014 for current fine tunning

// typedef struct                   dirui del 2017/12/18
// {
// 	float           m_LastOutput;
// 	float           m_LastCurrentSet;
// 	float           m_LastLastCurrentSet;
// 	float           m_LastActuCurrent;
// 	float           m_TotalAdjust;
// 	float           m_Err;
// 	float           m_LastErr;
// 	float           m_LastVoltage;
// } St_PowerFineTune;   //Add by DKQ on 06.04.2014 for Power fine tunning


typedef struct
{
//	bool           bNeedCheck;
//	bool           bGoStop;
    unsigned char  CheckType;
    float          fCtrlValue;
    float          fError;
    bool           bCtrlCurrClampV;
    float          fClampVerror;

    signed long    Counter;
    unsigned char  ErrorCode;
    bool           bWarning;
} St_CtrlValueCheck;

typedef struct
{
    MsTime         NextTime;    //�ϴμ�����ֵ��ʱ��
    bool           bChecking;
    unsigned char  ChannelIdx;

    float          fErrorFactor;
} St_CtrlValueSetting;

typedef struct
{
    bool           bNeedCheck;
// 	bool           bGoStop;     //���CtrlValue�쳣�ˣ���ֹͣ
// 	float          fWarningCapa;
// 	float          fErrorCapa;
// 	float          fWarningEnergy;
// 	float          fErrorEnergy;

    float          fErrorVoltage;
    signed long    Counter;
    signed long    MaxCount; //Max times of Battery Voltage lower than threshold.
    unsigned char  ErrorCode;
    //bool           bWarning;
} St_IrregularCheck; //zyx 180705
typedef struct
{
    MsTime         NextTime;    //�ϴμ������Ե�ʱ��
    bool           bChecking;
    unsigned char  ChannelIdx;
} St_IrregularSetting;

typedef struct
{
    unsigned char uc_start;
    unsigned char uc_length;
    //unsigned char uc_N;    //ϵ������ʱ�������ɣ���Ϊ���ܹ涨��ÿ��������Ӧ������  ��ʼ��ʼ����Nϵ����Ϊ0
    unsigned long ul_data[MAX_ADC_CODE_QUE];
} st_ADC_Code_Que;

typedef struct
{
    unsigned char m_IV_unitID;
    unsigned char m_AuxChnIdx;
    unsigned char m_AuxType;
    unsigned char m_AuxSensor;
    unsigned char m_IVChnIdx;
    unsigned char m_RptDest;  //Ԥ��
    unsigned char m_Reserve[2];
    float         m_value;
    MsTime    TrigTime;
} ST_BUILTIN_AUX_DATA_TRIG;

typedef struct
{
    unsigned char m_IVCh_Index;     // �ڸ�unit�ڵ�IVͨ�����
    St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAX_MAP_COUNT_PER_UNIT];  // һ��IVͨ���ֵ��ĸ���ͨ��  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
    unsigned short m_Total_In_OneIVCh[MAXAUXTYPE];  // ͬ�ָ���������ͨ��������  	//--new add---������
		unsigned short m_aux_type_ptr[MAXAUXTYPE];
} St_PC2IVUnit4AuxMap;  //  From PC to IVUnit for (about) AuxMap

#if DEBUG_TRACE_RECORD == 1
typedef struct
{
    unsigned short   m_u16RecordType;
    MsTime           m_RecordTime;
    //	SysTime          m_sysTime;	0819

    unsigned long    m_u32Parameter1;
    unsigned long    m_u32Parameter2;
    unsigned long    m_u32Parameter3;
    unsigned long    m_u32Parameter4;

    float            m_fParameter1;
    float            m_fParameter2;
} St_DebugPoint;

typedef CIRCULAR_QUEUE(St_DebugPoint,DEBUG_TRACE_RECORD_DEPTH) St_DebugTraceRecord;
#endif

