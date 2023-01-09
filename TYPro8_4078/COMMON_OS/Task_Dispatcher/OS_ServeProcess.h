/*****************************************************************************
// �����ļ�      :OS_ServeProcess.cpp     ��Ҫ���ڲ���ϵͳ���񡢵��Դ���
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.11
*****************************************************************************/

#pragma once

#include "../Tcp-net/net_config.h"
#include "../Data_Manipulation/Datatypes.h"

#define DEV_WAITTIME        6000000


//����ָ��
typedef void (*My_Func_Ptr_Void)(void);
typedef float (*My_Func_ptr_Ext3)(unsigned char Value_A);
typedef float (*My_Func_Ptr_Ext4)(unsigned long Value_A,unsigned long Value_B,unsigned long Value_C,unsigned long Value_D);
typedef unsigned long  (*My_Func_Ptr_Ext5)(unsigned long  Value_A,unsigned long Value_B);
typedef char (*My_Func_Ptr)(unsigned long  Value_A,unsigned long Value_B);
typedef char (*My_Func_Ptr_Ext)(unsigned long  Value_A,unsigned long  Value_B,unsigned long Value_C);
typedef void (*My_Func_Ptr_Ext1)(unsigned char Value_A,unsigned char Value_B);
typedef void (*My_Func_Ptr_Ext2)(unsigned char Value_A,unsigned short Value_B,unsigned short Value_C,unsigned char Value_D);
typedef void (*My_Func_Ptr_Ext3)(unsigned char Value_A,unsigned long Value_B);
typedef void (*My_Func_Ptr_Ext6)(unsigned char Value_A,float Value_B);
typedef unsigned char (*My_Func_Ptr_Ext7)(unsigned char Value_A,unsigned char Value_B);
typedef unsigned char (*My_Func_Ptr_void1)(void);
//���õĻ�
//pf();    ����ɲο���C��ָ�롷


enum TASK_NUMBER
{
    ETHERNETPARS_ETASK = 0,     //  Parse manage  // ���������
    DISPOSABLE_TASK,            //  Disposable manage (0ne time)  // һ����Ӳ��ʱ��������
    THIRDPARTY_TASK,            //  Thirdparty manage   // �������������������
    LED_TASK,                   //  LED manage (Flash light)  // ��Ӱ��˸������
    SAMPLE_TASK,                //  Sample manage  // ������������
    SCHEDULE_TASK,              //  Schedule manage  // �ƻ���������
    STEP_TASK,                  //  Step manage  // ����������
    CHECKLIMIT_4_TASK,          //  Check limit task in priority of 4
    CHECKLIMIT_8_TASK,          //  Check limit task in priority of 8
    EEPROM_TASK,                //  FRAM manage  // ��д���紦������
    //TEST_TASK,                //  Test manage  // ���Դ�������(Ϊ����ʱʹ��)
    SDL_TASK,                   //  SDL manage  // SDL��������

    SAFETYCHECK_TASK,
    LOG_TASK,
    MV_TASK,
    EVENTSBUFFERING_TASK,       //  �¼�������ظ�
    POWERLOSTCHECK_TASK,    //�����Device_MoreCase.h����
    CANBUSPARSE_TASK ,
    CANMSGBUFFERING_TASK,
    AUXSAMPLE_TASK,
    AUX_SAMPLE_4_TASK,
    AUXTHIRDPARTY_TASK,
    DS18B20_TASK,           //18B20Ҫ���ϸ�ĵȴ�ʱ����ʱ����������
    I2C_TASK,
    AUXBOARD_I2CRSN_TASK,   // �������ù���RSN����

    BT11_SAMPLE_TASK,        //BT11 ADC��������ȡ���ж���������  //DKQ 08.20.2014
    HS21044_SAMPLE_TASK,
    CHECKLIMIT_NON_V_TASK,
    CHECK_V_LIMIT_TASK,
    SAMPLE_PULSE_TASK,
    PULSE_LOGLIMIT_TASK,
    PULSE_MV_TASK,
    PULSE_SAFETY_TASK,
    //	AUX_DIDO_TASK,
    TEC_CTRL_TASK,
    UART_PARSE_TASK,
    REDUNDANTSAMPLE_TASK,
    FANADJ_TASK,
    AUX_DI_TASK,
    AUX_DO_TASK,
    AUX_UARTPARSE_TASK,
    AUXREDUNDANTSAFETYCHECK_TASK,
    AUX_SAMPLE_2ndV_LC_TASK,
    AUX_PID_TASK,
    AUX_SYSCTRL_TASK,
    CANWATCHDOG_TASK,
    SYSTEMDELAY_TASK,
    AUX_CHAMBER_TEC_KEY_TASK, //Added by Chang Zhong
    AUX_CHAMBER_TEC_CONTROL_TASK,//Added by Chang Zhong
    INDEPENDENTVOLTAGECTRL_TASK, //Added by Di Rui
    AUX_ACIM_Test_TASK,     // Added by qjm 20170717
    SMBUS_TASK,
    MULTIMCUCOMMUNICATION_TASK,
    POWERADJ_TASK,
    AUX_MTC_TASK,        //Added for MTC by Chang Zhong 04.05.2018
    DWIN_UART_PARSE_TASK,	   //Added for DWIN DGUS LCD SCREEN by Chang Zhong 04.05.2018
    TASK_COUNT_MAX,             //  Totel task number  // �ܹ�����������ֵ
};

#if ((THREAD_TASK_TIME_STATISTIC == 1) || (DEBUG_TASK_STATISTIC == 1))
#if USE_AS_AUXMCU==0
enum TASK_COUNT
{
    COUNT_STEP = 0,
    COUNT_CHECKLIMIT4,
    COUNT_CHECKLIMIT8,
    COUNT_SCHEDULE,
    COUNT_SAMPLE,          
    COUNT_ETHERNET,
    COUNT_CAN,
    COUNT_LOGIC,
    COUNT_LED,            
    COUNT_META,
    COUNT_SDL,
    COUNT_FRAM,
    COUNT_THIRDPARTY,
    COUNT_SAFTY,
    COUNT_I2C,
		COUNT_FANADJ,
		COUNT_POWERADJ,
		COUNT_POWER_LOST,
		COUNT_INDEPENDENT_VOL_CTRL,
    COUNT_UART_M0,         
    COUNT_CAN_WATCHDOG,
    COUNT_SAFTY_PULSE,
    COUNT_EVENTS_BUFFER,  
    COUNT_CHECK_V_LIMIT,
    COUNT_MCU_RUDP,
    COUNT_SMB,
    MAX_TASK_COUNT,
};
#else
enum TASK_COUNT
{
    COUNT_CHECKLIMIT4,
    COUNT_CHECKLIMIT8,
    COUNT_SAMPLE,          
    COUNT_ETHERNET,
    COUNT_CAN,
    COUNT_LED,            
    COUNT_META,
    COUNT_SDL,
    COUNT_FRAM,
    COUNT_ACIM_TEST,
    COUNT_SAFTY,         
    COUNT_I2C,
		COUNT_UART_PARSE,
    COUNT_UART_DWIN,         
		COUNT_CAN_WATCHDOG,
    COUNT_DI,
    COUNT_DO,
    COUNT_REDUNDANTSAFETY,
    COUNT_SAMPLE_2NDV_LC,   
    COUNT_MTCI,
		COUNT_MTC,
		COUNT_TEC,
    COUNT_REDUNDANTSAMPLE,
    COUNT_CONFIGRSN,
    COUNT_MCU_RUDP,
		COUNT_POWERADJ,
		COUNT_SYSCTRL,
    MAX_TASK_COUNT,
};
#endif
#endif

////////////////////////////////////////////////////////////////////////
////end user define////
//0930�޸�
#define  PRIORITY1      1
#define  PRIORITY2      2
#define  PRIORITY4      4
#define  PRIORITY8      8
#define  PRIORITYBG	    64

#define BGTICKS			64
#define MESSAGEMASK		31			// Used to roll back Messages[] index.	 ??

#define MAX_ONLINE_TASK_COUNT   64  // �������������������������Ŀ
typedef CIRCULAR_QUEUE(unsigned short,MAX_ONLINE_TASK_COUNT)    TASKID_QUEUE;

typedef struct
{
    unsigned char TaskID;                   //����ID
    void (* TaskAddress)     (void);		//TaskAddress ����ָ��   //�����ʽ  void function(void);
    void (* TaskInitAddress) (void);        //�����ʼ������ָ��
    unsigned char Priority;	                //���ȼ�
    unsigned char IDFlag;                   //�����Ƿ��Ѿ�������
} OS_TaskCtrBlock;    //������ƿ�

typedef struct
{
    OS_TaskCtrBlock      Task;
    bool                 m_bSuspended;
    short                m_OnlineID;
} OS_Task_Online;

typedef struct
{
    unsigned char SemID;
    unsigned char IDFlag;
} SemCtrBlock;

class OS_ServeProcess
{
private:
    static Int16             Ptr_TaskP1;		// current pointer of Task with priority 1, may be invalid when being used.
    static Int16             Ptr_TaskP2;		// these pointers are used for convenience, but need to be checked each time before using
    static Int16             Ptr_TaskP4;		// and also, these pointers will avoid a task to be run for multiple time when others with the same priorities
    static Int16             Ptr_TaskP8;		// are not executed at all or being executed with less times.
    static Int16             Ptr_TaskPBG;

    static UInt16            BGCount;			// control the counts for BG work
    static UInt16            P8BGSwitch;

    static UInt16            Messages[64];		// Messages system: buffer, pointer and count.
    static UInt16            MessageCount;
    static UInt16            Ptr_Message;
    static UInt16            SmallTickCount;
    static OS_TaskCtrBlock   OS_Task[TASK_COUNT_MAX];    //������ƿ�
    static unsigned char     TaskCount;    //�����ڲ�����
    static UInt16            TickFlag;
    static OS_Task_Online    Online_Task[MAX_ONLINE_TASK_COUNT];
    static TASKID_QUEUE      m_qOnlineTaskID;
    static unsigned short    m_OnlineTaskIDmap[MAX_ONLINE_TASK_COUNT];
    static short             OnlineTaskCount;
    static MsTime            OS_Time; //Can't be public
    static bool              OS_Time_Carried;
public:
    static bool 			 bUpdateSystemTime_Enable;
    static BYTE              Version[16]; //zyx 180305
    static St_DETAIL         Detail; //zyx 1804201
    static unsigned char     Eth_TimeCalied;
    static unsigned char     CAN_TimeCalied;
//    static unsigned char     TimeCali;
    static unsigned long     TimeCaliSecond;
    static unsigned short    TimeCaliTicks;
    static unsigned short    TimeCaliTickCount;
    static unsigned char     AUX_ADC_Idx;
    static unsigned char     AUX_ADC_GOTO_Idx;
    static bool              BuiltIn_Flag;        // �Ƿ����BUILTIN ����
#if DEBUG_TASK_STATISTIC == 1
    static unsigned long     PeriodCount;
    static unsigned long     PeriodSet;
    static unsigned long     Task_Count[MAX_TASK_COUNT+2];
    static MsTime            Task_Count_Time[MAX_TASK_COUNT+2];
#endif
    static unsigned short   uw16_Counter_RTC_Cali;
    static unsigned short   uw16_Limit_RTC_Cali;
    static unsigned long    uw32_Cycle1s_Count;   // ���ѭ��
    static unsigned char    uc_IfAdjust_RTC_Start;
	static unsigned char  m_SmoothRTCSM;
    static MsTime         m_SmoothRTCtime;
    static signed long long m_SmoothRTCtimeErrorInms[20];
    static unsigned char  m_SmoothRTCtimeCounter;
//    static unsigned char  m_SmoothRTCSetCalibrateUpdate;
    static short          m_SmoothRTCSetCalibrateValue;

private:
    static void   doOSWork(void);    //ִ������
    static void   OS_Start(void);    //ʹ��Timer0
    static void   ProcessPriority1(void);
    static void   ProcessPriority2(void);
    static void   ProcessPriority4(void);
    static void   ProcessPriority8GB(void);
    //static void   ProcessMessage(void);
    //	static int    SendMessages(UInt16 myMessage);
    static UInt16 FetchNextTask(UInt16 myPriority,UInt16 ifForEach);
    static void   Adjust_RTC_Time(void);
    static void   RTC_SetNewSystemTime(void);
		static void   OverTimeStartRTC(unsigned short uw16_OverTime);

public:
    static void   OSAuthorize(void);
    static void   Init_Device_Firmware(void);    //��ʼ��Ƭ������
    static void   Init_NetWork(void);
    static void   OS_Init(void);    //��ʼ��
    static void   OSCreateTask( void (*fDoWork)(void),void (*fInit)(void),unsigned char priority,unsigned char TaskID );    //creat task : �����ַ�����ȼ���tick
    static void   Timer0HandlerISR(void); // __irq ;
    static void   RTCHandlerISR(void); // __irq;


    static short  MountTask(unsigned char TaskID, unsigned char Priority,bool bInit);
    static void   DisMountTask(unsigned short OnlineTaskID);
    static void   SuspendTask(unsigned short OnlineTaskID);
    static void   ResumeTask(unsigned short OnlineTaskID,bool bInit);
#if  USE_AS_AUXMCU == 1
    static bool   IsMounted(unsigned short OnlineTaskID);
#endif
    //	static unsigned char Get_TaskPriority(unsigned short OnlineTaskID);
    //	static void   Set_TaskPriority(unsigned short OnlineTaskID,unsigned char Priority);
    static void   Set_TaskPriority2(unsigned short OnlineTaskID,unsigned char Priority);
    static void   RestartSystemTime(MsTime SetSystemTime);
    static void   SetSystemTimeSmooth(MsTime SetSystemTime);

public:
    static unsigned short  OTID_4_Reg_SamplePro;   //OnlineTaskID for Regular Sample Process //Add by DKQ 08.20.2014
    static unsigned short  OTID_4_Reg_CheckLimit4;
    static unsigned short  OTID_4_Reg_CheckLimit8;
    static unsigned short  OTID_4_Reg_SafetyCheck;
    static unsigned short  OTID_4_Reg_MetaVariable;

    static signed short    OTID_4_Pulse_SamplePro;   //OnlineTaskID for Pulse Sample Process
    static signed short    OTID_4_Pulse_Check_V_Limit;
    static signed short    OTID_4_Pulse_Check_non_V_Limit;
    static signed short    OTID_4_Pulse_SafetyCheck;
    static signed short    OTID_4_Pulse_MetaVariable;
    static signed short    OTID_4_Pulse_LogLimit;

public:
    friend  void 	CommonFunc::SetSystemTimeCarried(void);
    friend void CommonFunc::SetSystemTime(MsTime SetSystemTime);
    friend MsTime CommonFunc::GetSystemTime(void);


};

