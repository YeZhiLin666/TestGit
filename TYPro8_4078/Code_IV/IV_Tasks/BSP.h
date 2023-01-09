#pragma once

#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../../COMMON_OS/Ext_Devices/ControlPin.h"

// typedef struct
// {
// 	unsigned short   ACR_CH8:3;
// 	unsigned short   Reserved1:1;
// 	unsigned short   ACREN:1;
// 	unsigned short   Reserved2:11;
// }St_ACR_Bit;

// typedef union
// {
// 	unsigned short   Reg;
// 	St_ACR_Bit       Bits;
// }St_ACR;

// typedef struct
// {
// 	unsigned short   DCS3:1;
// 	unsigned short   DCS4:1;
// 	unsigned short   BUS1:1;
// 	unsigned short   BUS2:1;
// 	unsigned short   DCS5:1;
// 	unsigned short   Reserved2:4;
// 	unsigned short   E_ACIM:1;       // bit9:  ACIM
// 	unsigned short   Reserved4:3;
// 	unsigned short   Reserved5:3;
// }St_DCS_Bit;

// typedef union
// {
// 	unsigned short   Reg;
// 	St_DCS_Bit       Bits;
// }St_DCS;

typedef struct //dac infomation
{
    unsigned long long           m_uRisingTime; // Current output delay
    unsigned long long           m_uSettingTime;
    unsigned char                m_bHaveVoltageDac;
    unsigned char                m_bHaveClampVdac;
// 	unsigned char                m_bChannelBasedClampVdac;

    unsigned char                m_bCurrRngCount;
    unsigned char                m_bVoltRngCount;

// 	unsigned long long           m_uSettingTime;
    unsigned char                m_bHaveBuildInVolt;
    unsigned char                m_bUnipolar;
    float                        m_fPulseRiseTimeInMS;
    float                        m_fPID_Coefficient;
} St_DacInfo;

enum
{
    ADC_TYPE_I = 0xA0,
    ADC_TYPE_V,
};
enum RedundantMode
{
    REDUNDANT_PIN,
    REDUNDANT_M0
};
typedef struct //enable features
{
    unsigned char                Power_Off_Enable;
    unsigned char                Fan_Safety_Mode;
    unsigned char                Temperature_Safety_Mode;
//   unsigned char                Maxwell_Machine;
// 	unsigned char                Mcu_PWMFan_Enable;
//   unsigned char                High_Rng_Discharge_Mode;
// 	unsigned char                K2;
    unsigned char              Redundant_Safety_Mode;
    unsigned short			   OS_Tick_Interval;
    unsigned char			   PWM_Version_Control;
    RedundantMode              RedundantVoltMode;
	unsigned char               ADC_CS5560_Enable;

} St_Hardware_Feature_Enable;

class board_config
{
public:
    static    My_Func_Ptr_Void     FeedWatchDog;
    static    St_DacInfo           DacInfo;
    static    float                Accuracy;
    static    bool                 bHaveRedundantCheck;
    static    bool				         bFanAdj;
		static    unsigned short        Voltage_Range_Change_Time;
// 	static    unsigned char        ClampH_Outputflag;
// 	static    unsigned char        ClampL_Outputflag;
// 	static    volatile St_ACR      st_ACR;    //记录ACR的channel状态
// 	static    volatile St_DCS      st_DCS;    //记录DCS状态
    static    const St_Port        Pin_ACIM;

    static    My_Func_Ptr_Ext      Func_Dac_out;
    static    My_Func_Ptr          Func_Sample;
    static    My_Func_Ptr_Ext      Func_SetRange;
    static    My_Func_Ptr_Ext2     Func_SetRelay;
	static	  My_Func_Ptr_Ext7	   Func_Set_Volt_Range;
	static    My_Func_Ptr_Ext1     Func_SetAutoRange;   //add by qjm 20190826 增加额外配置自动切换量程的函数。
    //  static    My_Func_ptr_Ext3     Func_TempSample;
    static    My_Func_Ptr          Func_CheckError;    //add Wzh
    static    My_Func_Ptr          Func_RedundantSafety;
    static    My_Func_Ptr          Func_ReleaseRedundantSafety;
    static    My_Func_Ptr          Func_FanSafety;  //add by qjm 20150203
    static    My_Func_Ptr          Func_TemperatureSafety;//温度保护
    static    My_Func_Ptr_Ext      Func_UnitDO;       //模块内部DIDO支持
    static    My_Func_Ptr_Ext      Func_UnitDI;
    static    My_Func_Ptr          Func_UnitDO_Reset;
    static    My_Func_Ptr_Void     Func_UnitDI_ReadAll;
    static    My_Func_Ptr_Void     Func_BuildInVolt_ReadAll;
    static    My_Func_Ptr          Func_RTCProcedureForIV;
    static 	  My_Func_Ptr		       Func_FanAdj;    //fan adjust
    static 	  My_Func_Ptr          Func_ACR;
    // static 	  My_Func_Ptr_Void     Func_Avoid_Hot;

    static unsigned char getBID(void);
//    static unsigned short IVBoardType;
//    static unsigned short IVBoardTypeversion;
    static St_Hardware_Feature_Enable  Feature_Enable_init;
    static char DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type);
    static char FanSafetyCheck(unsigned long CH_Number,unsigned long type);
    static char TemperatureSafetyCheck(unsigned long CH_Number,unsigned long type);
    static char OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value);
// 	static char INPUT_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value);
    static char RTCProcedureForIV(unsigned long CH_Number,unsigned long Value);
// 	static char FanAdj(unsigned long Level,unsigned long AdjMod);
    static char RedundantSafety(unsigned long CH_Number,unsigned long type);
    static char ReleaseRedundantSafety(unsigned long CH_Number,unsigned long type);
// 	static char ACR_En(unsigned long CH_Number,unsigned long type);
// 	static void ACR_V(unsigned long type);

};

//应对于Tesla的掉电检测
class PowerLostCheck
{
private:
    static UInt16 skip_time;
    static MsTime last_Time;

    static unsigned char first_check;

    //
    static unsigned char powerLost;
    static unsigned char powerOn;
    static unsigned short powerLost_counter;
    static unsigned short powerOn_counter;

#if(USE_AS_AUXMCU == 0)
    static unsigned char sendPowerOff();
#endif

public:
    static unsigned char PowerIsOn;
    static unsigned char ParallelPowerCheckFlag;

    //task
    static UInt16  Priority;    //PRIORITY8
    static UInt16  TaskID;    //POWERLOSTCHECK_TASK
    static void Init(void);
    static void DoWork(void);
    static   My_Func_Ptr_void1     Func_PowerOffSafetCheck;

#if(USE_AS_AUXMCU == 0)
    static unsigned char checkPower();
    static unsigned char stateProcess();
#endif
    static unsigned char PowerOffSafety(void);
};

