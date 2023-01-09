#include "../Entry/includes.h"
unsigned short            AuxSysCtrl_task::Priority=PRIORITY4;
unsigned short            AuxSysCtrl_task::TaskID=AUX_SYSCTRL_TASK;

//                                         Port      Bits       PortType           Function               Address
const St_Port    AuxSysCtrl_task::PIN_WATCHDOG = {3,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char 						AuxSysCtrl_task::SysCtrlStatus;
unsigned char 						AuxSysCtrl_task::SysRunStatus;
unsigned char 						AuxSysCtrl_task::SysPowerOnStatus;
unsigned char 						AuxSysCtrl_task::SysPowerOffStatus;
unsigned char 						AuxSysCtrl_task::SysEstopStatus;


unsigned long 						AuxSysCtrl_task::ADCSample_Counter;
float 										AuxSysCtrl_task::m_GetTempValue;
unsigned short            AuxSysCtrl_task::SysCtrl_BoardErrorCode;
bool											AuxSysCtrl_task::SysCtrl_AlarmFlg;
bool											AuxSysCtrl_task::SysCtrl_MOVFlg;

MsTime 										AuxSysCtrl_task::SysSafety_last_time;
MsTime 										AuxSysCtrl_task::SysSafety_error_time;
MsTime 										AuxSysCtrl_task::SysSafety_resume_time;


MsTime										AuxSysCtrl_task::PowerOnStartTime;
MsTime										AuxSysCtrl_task::PowerOnRecordTime;

MsTime										AuxSysCtrl_task::PowerOffStartTime;
MsTime										AuxSysCtrl_task::PowerOffRecordTime;

MsTime										AuxSysCtrl_task::EStopStartTime;
MsTime										AuxSysCtrl_task::EStopRecordTime;

MsTime										AuxSysCtrl_task::KeyStartTime;
MsTime										AuxSysCtrl_task::KeyRecordTime;

unsigned char 						AuxSysCtrl_task::SysKeyStatus;

unsigned short 						AuxSysCtrl_task::SysErrorFlg[MAXUNIT];
unsigned short						AuxSysCtrl_task::SysCtrl_RunFlg[MAXUNIT];
unsigned short            AuxSysCtrl_task::m_Counter=0;
unsigned short 						AuxSysCtrl_task::KeyCode;
unsigned short 						m_GetDICode=0;

#pragma arm section //结束外存变量定义

void AuxSysCtrl_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SYSCTRL]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SYSCTRL);
#endif
	
    GetKey();
    ProcessStatus();

    m_Counter++;
    if(m_Counter>100)
        ControlPin::SetMeHigh(PIN_WATCHDOG);
    if(m_Counter>200)
    {
        ControlPin::SetMeLow(PIN_WATCHDOG);
        m_Counter=0;
    }
    AuxSysCtrl_task::Do_Move();

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SYSCTRL] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_SYSCTRL);
#endif
}
void	AuxSysCtrl_task::GetKey(void)
{

    MsTime TimeDiff;
    unsigned short temp;
    switch(SysKeyStatus)
    {
    case KEY_CODE:
        KeyCode=CPLD_FUNC::Read(NBUS_WRITE_ARBINDIR);
        if(KeyCode>0)
        {
            KeyStartTime=CommonFunc::GetSystemTime();
            SysKeyStatus=KEY_WAIT;
        }
        break;
    case KEY_WAIT:
        temp=CPLD_FUNC::Read(NBUS_WRITE_ARBINDIR);
        TimeDiff = CommonFunc::TimeDiff(KeyStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-KeyStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-KeyStartTime.Us100;
        if(TimeDiff.Us100>200)
        {
            if(temp==KeyCode)
                SysKeyStatus=KEY_CONFIRM;
            else
                SysKeyStatus=KEY_CODE;

        }
        break;
    case KEY_CONFIRM:
        SysKeyStatus=KEY_CODE;
        m_GetDICode=KeyCode;
        break;
    default:
        break;
    }

}
void	AuxSysCtrl_task::ProcessStatus(void)
{
    switch(SysRunStatus)
    {
    case SYS_IDLE:
        //	if(Aux_SystemCtrlBoard_415016_Base::m_GetDICode&SYSCTRL_DI_BUTTON)
        SysRunStatus=SYS_START;
        SysPowerOnStatus=POWERON_IDLE;
        // CPLD_FUNC::Write(NBUS_WRITE_ARBINDIR,0x0000);
        PowerOnStartTime=CommonFunc::GetSystemTime();
        break;
    case SYS_START:
        if(sys_StartRoutine())
            SysRunStatus=SYS_RUN;
        break;
    case SYS_RUN:
        sys_RunRoutine();
        break;
    case SYS_STOP:
        sys_StopRoutine();
        break;
    case SYS_ERROR:
        sys_ErrorRoutine();
        break;
    case SYS_RESUME:
        sys_ResumeRoutine();
        break;
    default:
        break;
    }
}
void AuxSysCtrl_task::MyInit(void)
{
    SysCtrlStatus=SYSSAFETYCHECK_SAFE;
    SysRunStatus=SYS_IDLE;
    SysPowerOnStatus=POWERON_IDLE;
    SysPowerOffStatus=POWEROFF_IDLE;
    SysKeyStatus=KEY_CONFIRM;
    Aux_SystemCtrlBoard_415016_Base::m_SetDOCode=0x0004;
    Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_24VPOWER;
    Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=(~SYSCTRL_DO_PWM380);
    CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
    m_GetDICode=0x0000;
    SysCtrl_AlarmFlg=0;
    SysCtrl_MOVFlg=0;
    ADCSample_Counter=0;
    m_GetTempValue=0;
    KeyCode=0;
    for(unsigned short i=0; i<MAXUNIT; i++)
    {
        SysCtrl_RunFlg[i]=0;
        SysErrorFlg[i]=0;
    }
    ControlPin::SetIObyCP(AuxSysCtrl_task::PIN_WATCHDOG);
    ControlPin::SetMeLow(PIN_WATCHDOG);

}
void AuxSysCtrl_task::EndTask(void)
{

}

void 	AuxSysCtrl_task::Do_CTRLSafetyCheck(void)
{
    if(AuxSysCtrl_task::Check_Temper(STANDARD_TEMPERATURE))
        SysCtrl_BoardErrorCode|=SYSTEMPER;
    if(AuxSysCtrl_task::Check_EStop())
        SysCtrl_BoardErrorCode|=SYSESTOP;
    if(AuxSysCtrl_task::Check_ErrorMsg())
        SysCtrl_BoardErrorCode|=SYSALARM;
}
void 	AuxSysCtrl_task::Do_SafetyProcess(void)
{
    MsTime Time;    //用于时间比较

    switch(SysCtrlStatus)
    {
    case SYSSAFETYCHECK_SAFE:
        if(SysCtrl_BoardErrorCode>0)
        {
            CommonFunc::GetTimeTicksUpdate(&SysSafety_last_time);
            SysCtrlStatus=SYSSAFETYCHECK_SAFETOUNSAFE;
        }
        break;
    case SYSSAFETYCHECK_SAFETOUNSAFE:
        if (SysCtrl_BoardErrorCode>0)
        {
            //比较时间
            CommonFunc::GetTimeTicksUpdate(&SysSafety_last_time);
            Time = TimeT(SysSafety_last_time,SysSafety_error_time);    //比较这2个时间  safety_error_time>safety_last_time
            if(Time.Us100 >= SAFETY_CHECK_PERIOD)
            {
                SysCtrlStatus=SYSSAFETYCHECK_UNSAFE;
                //Do_Move();

                break;
            }
        }
        else
        {
            SysCtrlStatus=SYSSAFETYCHECK_SAFE;
        }
        break;
    case SYSSAFETYCHECK_UNSAFE:
        if(SysCtrl_BoardErrorCode>0)
        {
            //do nothing
        }
        else
        {
            //记录恢复时间
            CommonFunc::GetTimeTicksUpdate(&SysSafety_resume_time);
            SysCtrlStatus=SYSSAFETYCHECK_UNSAFETOSAFE;
        }

        break;
    case SYSSAFETYCHECK_UNSAFETOSAFE:
        if (SysCtrl_BoardErrorCode>0)
        {
            SysCtrlStatus=SYSSAFETYCHECK_UNSAFE;
        }
        else
        {
            //比较恢复时间，准备恢复正常
            CommonFunc::GetTimeTicksUpdate(&SysSafety_last_time);
            Time = TimeT(SysSafety_last_time,SysSafety_resume_time);    //resume time 比较
            //if(Time.Us100 >= SAFETY_CHECK_RESUME)
            if(Time.Us100+Time.Second*10000 >= SAFETY_CHECK_RESUME)
            {
                SysCtrl_BoardErrorCode=0;
                SysCtrlStatus=SYSSAFETYCHECK_SAFE;

                break;
            }
        }
        break;
    default:
        break;
    }
}
void 	AuxSysCtrl_task::Do_Move(void)
{
    if(SysCtrl_BoardErrorCode&SYSTEMPER)
    {
    }
    if(SysCtrl_BoardErrorCode&SYSESTOP)
    {}
    if(SysCtrl_BoardErrorCode&SYSALARM)
    {}
    SysCtrl_AlarmFlg=false;
    SysCtrl_MOVFlg=false;
    for(unsigned char uc_i=0; uc_i<MAXUNIT; uc_i++)
    {
        if(AuxSysCtrl_task::SysErrorFlg[uc_i]>0)
        {
            SysCtrl_AlarmFlg=1;
            break;
        }

    }
    for(unsigned char uc_i=0; uc_i<MAXUNIT; uc_i++)
    {
        if(AuxSysCtrl_task::SysCtrl_RunFlg[uc_i]>0)
        {
            SysCtrl_MOVFlg=true;
            break;
        }

    }

    if(SysCtrl_AlarmFlg)
    {
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMYELLOW;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMGREEN;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMRED;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMBEEP;
    }
    else
    {
        if(SysCtrl_MOVFlg)
        {
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMYELLOW;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMGREEN;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMRED;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMBEEP;
        }
        else
        {
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMYELLOW;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMGREEN;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMRED;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMBEEP;
        }
    }
    CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
}

bool 	AuxSysCtrl_task::Check_Temper(float StandardValue)
{
    ADCSample_Counter++;
    if(ADCSample_Counter>999)
    {
        m_GetTempValue=(ADCFunc::Read(1)/10)*0.67568-0.23;
        ADCSample_Counter=0;
    }

    return (m_GetTempValue>StandardValue);
}
bool 	AuxSysCtrl_task::Check_EStop(void)
{
    //硬件暂无该电路
    return 0;
}
bool	AuxSysCtrl_task::Check_ErrorMsg(void)
{
    //硬件暂无该电路
    if(AuxSysCtrl_task::SysCtrl_AlarmFlg)//AuxSysCtrl_task::SysErrorFlg
        return true;
    else
        return false;
}

unsigned char	AuxSysCtrl_task::sys_StartRoutine(void)
{
    PowerOnRoutine();

}
unsigned char	AuxSysCtrl_task::sys_RunRoutine(void)
{
    Do_CTRLSafetyCheck();
    Do_Move();

    if(!(m_GetDICode>>15))//|| Aux_SystemCtrlBoard_415016_Base::m_GetDICode&SYSCTRL_DI_POFF)
    {
        SysRunStatus=SYS_STOP;
        SysPowerOffStatus	=	POWEROFF_IDLE;
    }

    if((m_GetDICode&0x4000)==0)
    {
        // Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_POFF;
        //CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
        SysRunStatus=SYS_STOP;
        SysPowerOffStatus	=	POWEROFF_IDLE;
    }
}
unsigned char 	AuxSysCtrl_task::sys_StopRoutine(void)
{
    MsTime TimeDiff;
    switch(SysPowerOffStatus)
    {
    case POWEROFF_IDLE:
        PowerOffStartTime=CommonFunc::GetSystemTime();
        //Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_24VPOWER;
        SysPowerOffStatus=POWEROFF_POWERSWITCH;
        break;
    case POWEROFF_POWERSWITCH:
        // = OS_ServeProcess::TimeDiff(m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);
        TimeDiff = CommonFunc::TimeDiff(PowerOffStartTime);
        //         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOffStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOffStartTime.Us100;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_PON;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_PWM380;
        if(TimeDiff.Second>Aux_SystemCtrlBoard_415016_Base::m_StopDelayTime)
        {
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_24VPOWER;

            SysPowerOffStatus=POWEROFF_MCUOFF;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWEROFF_UPSON:
        TimeDiff = CommonFunc::TimeDiff(PowerOffStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOffStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOffStartTime.Us100;
        if(TimeDiff.Second>5)
        {
            SysPowerOnStatus=POWEROFF_MCUOFF;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWEROFF_MCUOFF:
        TimeDiff = CommonFunc::TimeDiff(PowerOffStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOffStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOffStartTime.Us100;
        if(TimeDiff.Second>1)
        {
            //SysPowerOnStatus=POWEROFF_UPSOFF;
            if((m_GetDICode>>15))//|| Aux_SystemCtrlBoard_415016_Base::m_GetDICode&SYSCTRL_DI_POFF)
                SysRunStatus=SYS_IDLE;
            PowerOnStartTime=CommonFunc::GetSystemTime();
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_FANPOWER;

            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_POFF;
            SysRunStatus=SYS_IDLE;
            //			CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
        }
        break;
    case POWEROFF_UPSOFF:
        TimeDiff = CommonFunc::TimeDiff(PowerOffStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOffStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOffStartTime.Us100;
        if(TimeDiff.Second>5)
        {
            SysPowerOnStatus=POWEROFF_IDLE;
            PowerOnStartTime=CommonFunc::GetSystemTime();
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_POFF;
        }
        break;
    case POWEROFF_ABNORMAL:
        break;
    default:
        break;
    }
    CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
}
unsigned char 	AuxSysCtrl_task::sys_ResumeRoutine(void)
{
}
unsigned char		AuxSysCtrl_task::sys_ErrorRoutine(void)
{
}

unsigned char AuxSysCtrl_task::PowerOnRoutine(void)
{
    MsTime TimeDiff;
    switch(SysPowerOnStatus)
    {
    case POWERON_IDLE:
        //PowerOnStartTime=OS_ServeProcess::OS_Time;
        // Aux_SystemCtrlBoard_415016_Base::m_SetDOCode=0x0000;
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;
        if(TimeDiff.Second>Aux_SystemCtrlBoard_415016_Base::m_StartDelayTime)
        {
            SysPowerOnStatus=POWERON_POWERSWITCH;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWERON_POWERSWITCH:
        // = OS_ServeProcess::TimeDiff(m_RefreshCounter[0][m_CurrentSMBSignal.m_SMB_RX_Meta]);
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
        //         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_PON;
        // CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
        if(TimeDiff.Second>1)
        {
            SysPowerOnStatus=POWERON_AUXPOWER;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWERON_AUXPOWER:
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;
        if(TimeDiff.Second%2>0)
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode&=~SYSCTRL_DO_ALARMYELLOW;
        else
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMYELLOW;

        if(TimeDiff.Second>2)
        {
            SysPowerOnStatus=POWERON_CTRL24VON;
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_ALARMYELLOW;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }

        break;
    case POWERON_CTRL24VON:
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_24VPOWER;
        Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_PWM380;
        // CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
        if(TimeDiff.Second>1)
        {
            SysPowerOnStatus=POWERON_SYSTEMCHECK;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWERON_SYSTEMCHECK:
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;

        if(TimeDiff.Second>1)
        {
            SysPowerOnStatus=POWERON_FAN24VON;
            PowerOnStartTime=CommonFunc::GetSystemTime();
        }
        break;
    case POWERON_FAN24VON:
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;

        if(TimeDiff.Second>1)
        {
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_FANPOWER;
            //	CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
            PowerOnStartTime=CommonFunc::GetSystemTime();
            SysPowerOnStatus=POWERON_ACPOWERON;
        }
        break;
    case POWERON_ACPOWERON:
        TimeDiff = CommonFunc::TimeDiff(PowerOnStartTime);
//         TimeDiff.Second=OS_ServeProcess::OS_Time.Second-PowerOnStartTime.Second;
//         TimeDiff.Us100=OS_ServeProcess::OS_Time.Us100-PowerOnStartTime.Us100;

        if(TimeDiff.Second>1)
        {
            Aux_SystemCtrlBoard_415016_Base::m_SetDOCode|=SYSCTRL_DO_PON;
            PowerOnStartTime=CommonFunc::GetSystemTime();
            SysPowerOnStatus=POWERON_STANDBY;
            //CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
        }
        break;
    case POWERON_STANDBY:
        PowerOnStartTime=CommonFunc::GetSystemTime();
        return 1;
        //break;
    default:
        break;
    }
    CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,Aux_SystemCtrlBoard_415016_Base::m_SetDOCode);
    return 0;
}

MsTime AuxSysCtrl_task::TimeT(MsTime a,MsTime b)
{
    MsTime tempDiff;
    signed short temp;

    tempDiff.Second = b.Second - a.Second;
    temp = b.Us100 - a.Us100;
    if(temp < 0)
    {
        temp = temp + 10000;
        tempDiff.Second --;
    }
    tempDiff.Us100 = temp;
    return tempDiff;
}
