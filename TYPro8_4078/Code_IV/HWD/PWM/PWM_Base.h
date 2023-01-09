#pragma once

// #include "../../IV_Tasks/OS_LogicProcess.h"
// #include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
// #include "../IV_Tasks/BSP.h"
// #include "../../common_OS/Data_Manipulation/Datatypes.h"

#define PWMSETLOWPULSEDACTIME  50
#define PWMSETLOWPULSEADCTIME  (100-PWMSETLOWPULSEDACTIME)
#define PWMSETHIGHPULSEDACTIME 50
#define PWMSETHIGHPULSEADCTIME (100-PWMSETHIGHPULSEDACTIME)

#define PWMMINIMUMPLSE         50

#define TURN_ON_RAMP_TIME		  100     //100ms
#define MIN_RAMP_STEP_TIME        1       //10ms
#define TURN_ON_RAMP_STEP_NUM	  (TURN_ON_RAMP_TIME / MIN_RAMP_STEP_TIME)

#define _LOCK_ERROR_					0						//
#define MAX_DSP_NUMBER					125
enum
{
    PWM_Pin_AdcConverA,
    PWM_Pin_AdcConverB,
    PWM_Pin_AdcConverC,
    PWM_Pin_AdcBusy,
    PWM_Pin_AdcCS1,
    PWM_Pin_AdcRst,
    PWM_Pin_DacBinComp,
    PWM_Pin_DacCS,
    PWM_Pin_DacClr,
    PWM_Pin_DacLoad,
    PWM_Pin_WD,
//	PWM_Pin_SCK0,
//	PWM_Pin_MISO0,
//	PWM_Pin_MOSI0,
    PWM_Pin_VM1,
    PWM_Pin_IRH1,
    PWM_Pin_IRM1,
    PWM_Pin_IRL1,
    PWM_Pin_PROT1,
    PWM_Pin_UNLOCK1,
    PWM_Pin_EXT1,
    PWM_Pin_IVS1,
    PWM_Pin_VRL1,
//#if ( _PWM_VERSION_ ==0)
    PWM_Pin_ERR,
//#else
    PWM_Pin_DRIVEERR,
    PWM_Pin_OUDCERR,
    PWM_Pin_POWERERR,
    PWM_Pin_OTHERERR,
    PWM_Pin_VVS1,
    PWM_Pin_FanCtrl,
//#endif
    PWM_Pin_Syncro,
    PWM_Pin_SCK1,
    PWM_Pin_MISO1,
    PWM_Pin_MOSI1,

    PWM_Pin_ADCBUSY_C,
    PWM_Pin_ADCBUSY_V,
    PWM_Pin_Cover,
    PWM_Pin_CS_C,
    PWM_Pin_CS_V,
// 	PWM_Pin_Led1,
// 	PWM_Pin_Led2,
// 	PWM_Pin_Led3,
    PWM_Pin_VR,
    PWM_Pin_ACI,
    PWM_Pin_DO1,
    PWM_Pin_InductanceFanCheckSignal,
    PWM_Pin_BoardFanCheckSignal,
    PWM_Pin_AlarmLEDRed,
    PWM_Pin_AlarmLEDOrange,
    PWM_Pin_AlarmLEDGreen,
    PWM_Pin_Buzzer,
    PWM_Total_Pins,
};

typedef struct
{
    unsigned char    StageIdx;
    unsigned char    StateMachine;
    MsTime           StartTime;
    unsigned char    bStarted;
} St_TransRamp;
enum
{
    DisposableRamp_DacOut,
    DisposableRamp_Delay,
    DisposableRamp_Finish,
};
typedef struct
{
    unsigned short   Rly_Set;
    unsigned short   Rly_Clr;

    unsigned short   bSetVoltRangeActive:1;
    unsigned short   bSetCurrRangeActive:1;
    unsigned short   bSetCurrDac2Zero:1;
    unsigned short   bSetVoltDac2Zero:1;
    unsigned short   bSetCurrDac2NegtiveMax:1;
    unsigned short   bSetCurrDac2PositiveMax:1;
    unsigned short   bSetCurrDacActive:1;
    unsigned short   bSetVoltDacActive:1;
    unsigned short   bSetCVHdacActive:1;
    unsigned short   bSetCVLdacActive:1;
	unsigned short   bSetCVHdacInactive:1;
	unsigned short   bSetCVLdacInactive:1;	
    unsigned short   bSetDacRamp:1;
    unsigned short   bSetDacValue:1;
    unsigned short   bSetDischargeResidual:1;  //Add DKQ 02.03.2015
	unsigned short   Reserved:1;       // Add DKQ 02.03.2015

    unsigned short   DelayTime;

    unsigned long    DacValue;
} St_TranTask;
typedef struct
{
    unsigned char    StepIdx;
    unsigned char    StepCount;
    unsigned char    bStepInited;   //Steps initialized?
    unsigned char    bActionDone;   //if true,Relay,dac done,and wait for DelayTime
    MsTime           StartTime;

    St_TransRamp     m_Ramp;

    St_TranTask      StepInfo[50];
} St_TransitionInfo;


typedef struct
{
    unsigned short   VRL:1;
    unsigned short   PROT:1;
    unsigned short   UNLOCK:1;
    unsigned short   EXT:1;
    unsigned short   IRM:1;
    unsigned short   IRL:1;
    unsigned short   IVS:1;
    unsigned short   IRH:1;
    unsigned short   Reserved:8;
} St_PWM_Relay_Bit;
enum
{
    PWM_RLY_VL       = (0x01 << 0),
    PWM_RLY_Protect  = (0x01 << 1),
    PWM_RLY_Unlock   = (0x01 << 2),
    PWM_RLY_Ext      = (0x01 << 3),
    PWM_RLY_IM       = (0x01 << 4),
    PWM_RLY_IL       = (0x01 << 5),
    PWM_RLY_Vctrl    = (0x01 << 6),
    PWM_RLY_IH       = (0x01 << 7),
};
typedef union
{
    unsigned short   Reg;
    St_PWM_Relay_Bit Bits;
} St_PWM_Relay;

typedef struct
{
    unsigned char State;
//#if ( _PWM_VERSION_ ==0)
    signed long Counter;
    signed long CheckCounter;
//#else
    unsigned short IGBTErrCounter;         //
    unsigned short POWERErrCounter;    //
    unsigned short DlinkErrCounter;
    unsigned short Inverter2FuseCounter;
    unsigned char  ErrorTypeFlag;     //0:没有故障发生 ，1:可恢复故障，  1：不可恢复故障。
//#endif
    MsTime        StartTime;
    char          Code;
} St_PWM_Error;
////////////////
enum
{
    PWM_SSP0_DAC_NAME = 0xA0,
    PWM_SSP0_DAC_CLAMP,
    PWM_SSP0_FRAM_NAME,
    PWM_SSP1_ADI_NAME,
    PWM_SSP1_ADV_NAME
};

enum
{
    PWM_ADC_IDLE,
    PWM_ADC_CONV,
    PWM_ADC_Read,
    PWM_ADC_ReadC,
	PWM_ADC_ReadB,
};

enum
{
    PWM_ADC1_IDLE,
    PWM_ADC1_WAIT,
    PWM_ADC1_Trigger,
    PWM_ADC1_ReadI,
    PWM_ADC1_ReadV,
    PWM_ADC1_CVdelay,
};

enum
{
    ErrorSM_Idle,
    ErrorSM_Normal,
    ErrorSM_Error,
};
enum
{
    SYSTEM_IDLE=1,
    SYSTEM_RUN,
    SYSTEM_ALARM,
};

class PWM_Base
{
private:
    static volatile St_PWM_Relay PWM_relaystatus[MAXCHANNELNO];    //记录自身的relay状态


    static St_DisposableTask SetCtrl_RstToCur[1];
    static St_DisposableTask SetCtrl_RstToVlt[1];
    static St_DisposableTask SetCtrl_RstToOff[1];
    static St_DisposableTask SetCtrl_OffToRst[1];
    static St_DisposableTask SetCtrl_OffToCur[1];
    static St_DisposableTask SetCtrl_OffToVlt[1];
    static St_DisposableTask SetCtrl_CurToCur[1];
    static St_DisposableTask SetCtrl_CurToVlt[1];
    static St_DisposableTask SetCtrl_CurToOff[1];
    static St_DisposableTask SetCtrl_CurToRst[1];
    static St_DisposableTask SetCtrl_VltToVlt[1];
    static St_DisposableTask SetCtrl_VltToCur[1];
    static St_DisposableTask SetCtrl_VltToOff[1];
    static St_DisposableTask SetCtrl_VltToRst[1];
		static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay

#if (IV_AUTORANGE == 1)
	static St_DisposableTask SetAutoRangeSwitch[1];
#endif
// 	static unsigned char Length_RstToCur;
// 	static unsigned char Length_RstToVlt;
// 	static unsigned char Length_RstToOff;
// 	static unsigned char Length_OffToRst;
// 	static unsigned char Length_OffToCur;
// 	static unsigned char Length_OffToVlt;
// 	static unsigned char Length_CurToCur;
// 	static unsigned char Length_CurToVlt;
// 	static unsigned char Length_CurToOff;
// 	static unsigned char Length_CurToRst;
// 	static unsigned char Length_VltToVlt;
// 	static unsigned char Length_VltToCur;
// 	static unsigned char Length_VltToOff;
// 	static unsigned char Length_VltToRst;

    static const unsigned char m_bHaveVoltageDac;
    static const unsigned char m_bHaveClampVdac;
// 	static const unsigned char m_bChannelBasedClampVdac;

    //static St_PWM_Error m_Error;  //add Wzh
    static unsigned long long TimePassed;		//add Wzh

    //hardware  把这部分扔到别的文件夹去
    static void IO_Init(void);
    static void ADC_Init(void);
    static void DAC_Init(void);

    //software
    static void Func_Init(void);

    //       static void regCHNInfo(unsigned char CH_Number,unsigned long type);
    //       static void regEngine(unsigned char CH_Number,unsigned long type);
    static void Engine(void);// 有电压环
    static void EngineStuff(void);
    static void SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali);
    static char SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange);
    static void SetCurrRange(unsigned char ChannelIdx,unsigned char range);


    static void DoStep(unsigned char ChannelIdx,St_DisposableMSG *Task);
    static void DoChangeCurrentRange(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl);
    static void DoTurnChannelOn(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl);
    static void DoTurnChannelOff(unsigned char ChannelIdx,unsigned char CurrRange,unsigned char VoltRange,unsigned char bCurrCtrl);
    static void DoRest2Off(unsigned char ChannelIdx);
    static void DoOff2Rest(unsigned char ChannelIdx);
    static void DoClose2Close(unsigned char ChannelIdx);

    static void Feed_CpldWD(void);    //PWM喂狗信号  大概6s一次
    static void PWM_FeatureEnable_Init(void);
    static char FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod);
public:
    static SSPI_Para     PWM_SSP0DAC;
    static SSPI_Para     PWM_SSP1ADC;
    static DSP_Basic_Parameter   Record_DSP_Basic_Parameter[MAX_DSP_NUMBER];
    static DSP_Adjust_Parameter  Record_DSP_Adjust_Parameter[MAX_DSP_NUMBER];
		static DSP_Indentity_Parameter  Record_DSP_Indentity_Parameter[MAX_DSP_NUMBER];
    static St_TransitionInfo TransitionInfo[2];
    static const St_Port PinGroup[PWM_Total_Pins];
    static St_PWM_Error m_Error;   //add by qjm 20150724
    static unsigned char  PWM_LEDRunStatus;
    static void SetVoltRange(unsigned char ChannelIdx,unsigned char range);
    //初始化
    static void Hardware_Init(void);
    static void Software_Init(void);
		static void AdditionalEngine(void);

    //四个基本操作
    static char DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type);

    static unsigned char DoTransition(unsigned char ChannelIdx,St_DisposableMSG *Task);
    static char FanCheckSafety(unsigned long CH_Number,unsigned long type);
    static char ErrorCheck(unsigned long ChannelIdx,unsigned long Para);   //add Wzh
// 	static void LEDShow(unsigned char Red_IO,unsigned char Orange_IO,unsigned char Green_IO,unsigned char BuzzSwitch);
};



