#pragma once

#define STANDARD_TEMPERATURE         60           //设定安全温度上限60度

enum
{
    SYS_IDLE,
    SYS_START,
    SYS_RUN,
    SYS_STOP,
    SYS_ERROR,
    SYS_RESUME
};

enum SYSCTRL_UnsafeCode
{
    ALLSAFE  		=0,
    SYSALARM		= (1<<0),
    SYSESTOP		= (1<<1),
    SYSTEMPER		= (1<<2),
};
enum
{
    SYSSAFETYCHECK_SAFE,
    SYSSAFETYCHECK_SAFETOUNSAFE,
    SYSSAFETYCHECK_UNSAFE,
    SYSSAFETYCHECK_UNSAFETOSAFE
};

enum
{
    POWERON_IDLE,
    POWERON_POWERSWITCH,
    POWERON_AUXPOWER,
    POWERON_CTRL24VON,
    POWERON_SYSTEMCHECK,
    POWERON_FAN24VON,
    POWERON_ACPOWERON,
    POWERON_STANDBY
};

enum
{
    POWEROFF_IDLE,
    POWEROFF_POWERSWITCH,
    POWEROFF_UPSON,
    POWEROFF_MCUOFF,
    POWEROFF_UPSOFF,
    POWEROFF_ABNORMAL
};

enum
{
    ESTOP_IDLE,
    ESTOP_MCUOFF,
    ESTOP_POWEROFF,
    ESTOP_ALARM
};

enum
{
    KEY_CODE,
    KEY_WAIT,
    KEY_CONFIRM
};

//enum
//{
//	SYS_CTRL_NORMAL,
//	SYS_CTRL_ALARM,
//	SYS_CTRL_DOSWITCH
//};
class AuxSysCtrl_task
{
public:

    static unsigned short            	Priority;
    static unsigned short            	TaskID;

    static unsigned short           					SysCtrl_BoardErrorCode;
    static bool												SysCtrl_AlarmFlg;
    static bool												SysCtrl_MOVFlg;

    static float 											TempSampleValue;
    static bool 											Estop_Flg;
    static unsigned long 							ADCSample_Counter;
    static float 											m_GetTempValue;


    static unsigned short 											SysErrorFlg[MAXUNIT];
    static unsigned short											SysCtrl_RunFlg[MAXUNIT];
public:
    static void              					DoWork(void);
    static void              					MyInit(void);
    static void              					EndTask(void);
private:
    static void												ProcessStatus(void);
    static void												GetKey(void);
private:
    static unsigned char 							SysRunStatus;
    static unsigned char 							SysCtrlStatus;
    static unsigned char 							SysPowerOnStatus;
    static unsigned char 							SysPowerOffStatus;
    static unsigned char 							SysEstopStatus;
    static unsigned char 							SysKeyStatus;
    static const St_Port    					PIN_WATCHDOG;
    static unsigned short            	m_Counter;
    static unsigned short 						KeyCode;

//	static unsigned char 							SysCtrlMoveStatus;

    static MsTime 										SysSafety_last_time;
    static MsTime 										SysSafety_error_time;
    static MsTime 										SysSafety_resume_time;

    static MsTime											PowerOnStartTime;
    static MsTime											PowerOnRecordTime;

    static MsTime											PowerOffStartTime;
    static MsTime											PowerOffRecordTime;

    static MsTime											EStopStartTime;
    static MsTime											EStopRecordTime;

    static MsTime											KeyStartTime;
    static MsTime											KeyRecordTime;

    static unsigned char							sys_StartRoutine(void);
    static unsigned char							sys_RunRoutine(void);
    static unsigned char 							sys_StopRoutine(void);
    static unsigned char  						sys_ResumeRoutine(void);
    static unsigned char							sys_ErrorRoutine(void);

    static unsigned char  						PowerOnRoutine(void);


    static void 											Do_CTRLSafetyCheck(void);
    static void 											Do_SafetyProcess(void);
    static void 											Do_Move(void);

    static bool 											Check_Temper(float StandardValue);
    static bool 											Check_EStop(void);
    static bool 											Check_ErrorMsg(void);

    static	MsTime 										TimeT(MsTime a,MsTime b);

};
