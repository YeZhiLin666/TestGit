///////////////////////////////////////////////////////////////////////////////////
//File Name: Step.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						Step管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../IV_Tasks/OS_LogicProcess.h"

#define CCCV_TB_IR_COUNTLIMIT 32
#define SAMPLE_POINT          5

#define AUTO_RANGE_WAIT_STABLE 10000//AutoRange_WaitOutputStably 
#define AUTO_RANGE_WAIT_CHANGE_OVERTIME 100000 // 10 second wait change range
#define AUTO_RANGE_PERCENT_LOW 	0.6f
#define AUTO_RANGE_PERCENT_HIGH 0.9f
#define AUTO_RANGE_STABILIZATION_TIME  10000.0f

#define CCCV_Ramp_VoltDifference 0.0002f //the min voltage change when CCCV Ramp need calculate IR

#define SINGLE_BATTARY_VOLT  4.2f  //for estimate IR\. if IR too small, it can't be constant the voltage

#define RAPIDLY_COMMUN_CHECK  (10000)
#define RAPIDLY_COMMUN_OVERTIME  (50000)

#define ClearChnSampleFilter(chn)   (ContorlChnSampleFilter(chn, false, true))
#define SetChnSampleFilter(chn)  (ContorlChnSampleFilter(chn, true, false))

#define SetRapidlyCommun(chn)  (Step_Manager_task::b_IsRapidlyCommun[chn] = true)
#define ClearRapidlyCommun(chn)  (Step_Manager_task::b_IsRapidlyCommun[chn] = false)

#define CV_STAGE_KEEP_TIME_OUT 300  //100us计数  pwm上升沿为10ms，需要预留多点时间
#define PARALLEL_PID_INTERVAL  200  //  if parallel mode, PID Contorl Interval is 20ms    unit: 100us
#define SIMU_CLOSE_TO_CLAMP_PERCENTAGE 0.001f   //钳压值与当前电压值相差 0.1%
#define  FINE_TUNE_INTERVAL 1000 //fine tune interval : unit 100us //200616 zyx
#define MAX_PARALLEL_WAITDISPOSABLEFINISH_TIME 10000	//2022.09.13 dyy 并联主通道等待从通道完成时序 最大等待1秒

enum
{
    STEPRUNNING_READY,
    STEPRUNNING_JUDGESPECIALCASE,
    STEPRUNNING_DOSPECIALCASE,
    STEPRUNNING_CACULATEOUTPUT,
    STEPRUNNING_UPDATE,
    STEPRUNNING_OUTPUT,
    STEPRUNNING_LIMITCHECK,
};
enum
{
    AUXACK_IDLE,
    AUXACK_WAITCLEARLIMITACK,
    AUXACK_SENDDOWNLOADLIMIT,
    AUXACK_WAITDOWNLOADLIMITACK,
    AUXACK_RETRANCLEARLIMIT,
    AUXACK_RETRANDOWNLOADLIMIT,
    AUXACK_AUXACKFAIL,
};
enum
{
    NORMAL=0,
    SLOWPULSE,
    FASTPULSE,
};

// 2015.09.22
typedef enum
{
    ACIM_NONE					= 0,
    ACIM_START					= 1,
    ACIM_WAIT					= 2,
    ACIM_WAIT_RELAY_ON 			= 3,
    ACIM_TEST 					= 4,
    ACIM_WAIT_RELAY_OFF 		= 5,
    ACIM_END					= 6,
} ACIM_STATUS;

enum
{
    NORMAL_PAUSE = 0,
    ACIM_PAUSE,
    T_CHAMBER_PAUSE,
};
enum    //AutoRnageStatus
{
    AutoRange_Init,
    AutoRange_Check,
    AutoRange_WaitforChangeLow,
    AutoRange_WaitforChangeHigh,
    AutoRange_Switch,
    AutoRange_NoNeedSwitch,
};

typedef struct
{
    unsigned char  Used;
    St_StepMessage StepMessage;
} St_DoStepMessage;         //需要处理的Message结构

typedef struct
{
    St_StepInfo 		GetStepInfo;
    St_CvInfo			GetCVInfo;
    St_FormulaInfo		GetFormulaInfo;
    St_SMBBlockInfo  GetSMBBlockInfo;

} St_GetStepRunInfo;            //Step运行相关信息

typedef struct
{
    UInt16   MyAuxLimitIdx;
    UInt16   MyAuxStepID;
} St_AuxLimit;

typedef struct
{
    unsigned char bWattingAckOver;
    unsigned char AckStatus;
    unsigned char ReClrLimitCnt;
    unsigned char ReDwonLimitCnt;
    unsigned short WaitAckCnt;
    unsigned char  AuxLimitCnt;
    unsigned char bNeedWattingAck;
    unsigned char bAuxAckFailed;
} St_AuxLimitAckMgr;

typedef struct
{
    float points[10];
    int   count;
} avg;
typedef struct
{
    bool     m_bCheckRangeSwitchNeededFlag;   //0： 需要自动量程切换，  1：已经切换到最低量程，不再切换
    float    m_CurrengRange_Value;               //当前量程
    float    m_NextLowRange_Value;            //比当前量程低一个量程
} St_AutoRangeSwitchCurPara;

class Step_Manager_task
{
public:
		static MsTime Parallel_WaitSlaveChnDisposableFinish_Time[MAXCHANNELNO];
    static bool IsSetCCCV_StageTime;
    static float CC_Current[MAXCHANNELNO];
    static UInt16 MyStatus;	                                                                 //任务状态
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级
    static St_CurFineTuneFactor m_CurFineTuneFactor;
    static unsigned long        DacOutputCounter;
    static unsigned char        bDoneSLThisCycle;
    static unsigned char        bNewSimuLoaded[MAXCHANNELNO];
    static unsigned char        bNeedCheck[MAXCHANNELNO];
    static unsigned char        bStepLimitChecked[MAXCHANNELNO];
    static unsigned char        bLogLimitChecked[MAXCHANNELNO];
    static St_LimitPointer      SL_StartPtr;
    static St_LimitPointer      SL_CurrentPtr;
    static St_LimitPointer      LL_StartPtr;
    static St_LimitPointer      LL_CurrentPtr;
    static unsigned char 		ScanStepLimitNum[MAXCHANNELNO];
    static unsigned char 		StepLimitReach[MAXCHANNELNO];
    static unsigned char 		TimeRefreshFlag[MAXCHANNELNO];
    static float                m_FSet[MAXCHANNELNO];
    static St_AuxLimit			AuxLimitBuf[MAXCHANNELNO][MAXAUXLIMIT];
    static UInt16				MyAuxLimitCount[MAXCHANNELNO];
    static St_AuxLimitAckMgr    AuxLimitAckMgr[MAXCHANNELNO];
    static MsTime 				SysTimeEngineTask[MAXCHANNELNO];	//0819
    static MsTime  				MsTimeEngineTask[MAXCHANNELNO];
    static unsigned short 		EngineHandle[MAXCHANNELNO];
    static unsigned char 		EngineOverTimeTriger[MAXCHANNELNO];
    static unsigned char    	ChannelCVStatus[MAXCHANNELNO];  //CV状态机 add by qjm 20141016
    static unsigned int     	ChannelCVCounter[MAXCHANNELNO];
    static float            	CV2CCCV_K_gain[MAXCHANNELNO];
    static float            	CV_StartVoltage[MAXCHANNELNO];
    static float            	CV_TargetVoltage[MAXCHANNELNO];
    static float            	CV2CCCVRefreshValue[MAXCHANNELNO];
    static unsigned int     	uw16_CVResumeStageCounter[MAXCHANNELNO];   //add by qjm 2014/10/22
    static unsigned char    	NewCodeRefreshed[16];
    static unsigned char    	ChannelCCCVStatus[MAXCHANNELNO];
//====================CCCV_TB 07.07.2017 BY ZC=================================================================
    static unsigned char    	ChannelTBStatus[MAXCHANNELNO];//06.28.2017 TURBO BOOST ZC
    static float              CCCV_TB_TargetVoltage[MAXCHANNELNO]; //恒压电压值
    static float              CCCV_TB_ChargeCurrent[MAXCHANNELNO];//充电电流
    static float              CCCV_TB_ChargeTime[MAXCHANNELNO]; //充电电流每个周期持续时间
    static float              CCCV_TB_DischargeCurrent[MAXCHANNELNO]; //放电电流
    static float              CCCV_TB_DischargeTime[MAXCHANNELNO]; //放电电流每个周期持续时间
    static float              CCCV_TB_DV_PulseCount[MAXCHANNELNO];
    static float              CCCV_TB_PULSECOUNTER[MAXCHANNELNO];
    static int								CCCV_TB_IR_Pointer[MAXCHANNELNO];
    static float							CCCV_TB_IR[MAXCHANNELNO][CCCV_TB_IR_COUNTLIMIT];
    static int								CCCV_TB_UPDATE_FLAG[MAXCHANNELNO];
    static bool      					CCCV_TB_NeedExtLogFlg[MAXCHANNELNO];
//====================CC_TB 06.04.2018 BY ZC===========================================================
    static unsigned char    	ChannelCC_TBStatus[MAXCHANNELNO];//06.04.2018 TURBO BOOST BY ZC
    static float              CC_TB_ChargeCurrent[MAXCHANNELNO];//06.04.2018  充电电流
    static float              CC_TB_ChargeTime[MAXCHANNELNO]; //06.04.2018  充电电流每个周期持续时间
    static float              CC_TB_DischargeCurrent[MAXCHANNELNO]; //06.04.2018  放电电流
    static float              CC_TB_DischargeTime[MAXCHANNELNO]; //06.04.2018  放电电流每个周期持续时间
//=============================================================================================
    static unsigned int    		ChannelCCCVCounter[MAXCHANNELNO]; // 09.09.2015
    static unsigned short   	ChannelCCCV_LimitDlyCounter[MAXCHANNELNO];
    static bool             	bCCCV_Initiated[MAXCHANNELNO];
    static bool             	CCCVNoLimitCheckFlag[MAXCHANNELNO];
    static float            	CCCV_TargetVoltage[MAXCHANNELNO];
    static float            	CCCV_TargetCurrent[MAXCHANNELNO];
    static unsigned char        uc_AutoRangeStatus[MAXCHANNELNO];
    static unsigned long        AutoRangeLastOutput[MAXCHANNELNO];
    static float					AutoRangeLastVoltage[MAXCHANNELNO];
    static bool             	bExtChargeFlag[MAXCHANNELNO];  //add by qjm 20150901 Extcharge 开始标志
    static bool             	bEC_CalEnergyFlag[MAXCHANNELNO][4]; //add by qjm20151013  开始进入外部充电能力累加。
    static St_GetStepRunInfo 	GetStepRunInfo[MAXCHANNELNO];							     //Step提取信息
    //PULSE
    static unsigned char        PULSE_bNeedCheck[MAXCHANNELNO];
    static St_LimitPointer      PULSE_SL_CurrentPtr;
    static St_LimitPointer		PULSE_LL_CurrentPtr;
    static unsigned char        PULSE_bStepLimitChecked[MAXCHANNELNO];
    static unsigned char        PULSE_bLogLimitChecked[MAXCHANNELNO];
    static unsigned char        bCV[MAXCHANNELNO];

    //***********************************************************************   add by GL 内阻实时更新和钳压
    static unsigned char        bSimuOutOfDeadBand[MAXCHANNELNO];                  //GL
    static unsigned char        bAfterSimuLoaded[MAXCHANNELNO];							//GL
    static unsigned char        bTimePassed[MAXCHANNELNO];
    static float        AccurateIR[MAXCHANNELNO][10];																			//GL 6ceshi
    static float        InaccurateIR[MAXCHANNELNO][10];																		//GL
    static float        SIMU_I_IR[MAXCHANNELNO];																									//GL
    static float        SIMU_P_IR[MAXCHANNELNO];							// 10042017 add by ZC
    static float				ACCURATE_IR[MAXCHANNELNO];																//GL
    static float        INACCURATE_IR[MAXCHANNELNO];																								//GL
    static int   				ACCURATE_IR_Size[MAXCHANNELNO];  		 //Accurate IR size
    static int          INACCURATE_IR_Size[MAXCHANNELNO];
    static bool         ACCURATE_IR_SizeFlag[MAXCHANNELNO];
    static bool         INACCURATE_IR_SizeFlag[MAXCHANNELNO];
    static float 				LV_Voltage[MAXCHANNELNO];    //Last voltage value
    static float 				LV_Current[MAXCHANNELNO];		 //Last current value
    static float 				ACCURATE_IR_Update[MAXCHANNELNO];     //Update Accurate IR value
    static float 				INACCURATE_IR_Update[MAXCHANNELNO];   //
    static float 				delta_voltage[MAXCHANNELNO];
    static float 				delta_current[MAXCHANNELNO];
    static float 				ACCURATE_IR_Sum[MAXCHANNELNO];
    static float 				INACCURATE_IR_Sum[MAXCHANNELNO];
    static MsTime				SIMU_I_TimeBuffer[MAXCHANNELNO];
    static unsigned char  M4WaitACKOvertimeFlag[MAXCHANNELNO]; //add by qjm 20181205
    static unsigned char  uc_StartOrRestartFlag;      //0: start;  1: restart
    static unsigned char  M4ReceiveACKCnt;        //add by qjm 20180803
    static unsigned char  M4RestartSentCnt;   //发送的Restart 命令条数
    static unsigned short  M4WaitTimeCnt[MAXCHANNELNO];  //定时20ms发送一次
    static unsigned short M4WaitPacketCnt[MAXCHANNELNO];    //定时500ms 发送一次
    static unsigned char  M4SendComCounter[MAXCHANNELNO];   //最多发送五次， 100ms 后不再发送。
    static unsigned char  M4SendPacketCounter[MAXCHANNELNO]; //每个500ms 发送一次，最多发送四次， 两秒没有回复报警。
    static unsigned char  M4SendStartStatus[MAXCHANNELNO];
    static bool          b_CurCoarseTune[MAXCHANNELNO];  //add by qjm 20181224
    static bool          b_CurCoarseTuneInit[MAXCHANNELNO];  //粗调开始前的参数初始化标志，保证进入粗调环节首次进入，后期不再进入
    static bool          b_CurCoarseTuneSampleFinish[MAXCHANNELNO];  //粗调采样点完成标志   add by qjm2019.12.28
    static unsigned char  uc_CurCoarseTuneSampleCnt[MAXCHANNELNO];  //????????;
    static float          f_CurCoarseTuneADValue[MAXCHANNELNO][SAMPLE_POINT];
    static unsigned char  uc_CurCoarseTuneCnt[MAXCHANNELNO];
    static bool         b_CC_RampStampFlag[MAXCHANNELNO];
    static MsTime 		uc_CC_RampStampTime[MAXCHANNELNO];
    static bool          b_CurCoarseTuneFinished[MAXCHANNELNO];
    static bool          b_FineTuneFinished[MAXCHANNELNO];
    static bool           b_LEDFlag;
    static avg          Voltavg[MAXCHANNELNO];
    static avg          Curravg[MAXCHANNELNO];
    static bool        b_AutoRangeRequestFlag[MAXCHANNELNO];  //时序请求发送标志
    static float               f_CCCV_Ramp_V0[MAXCHANNELNO];
    static float               f_CCCV_Ramp_I0[MAXCHANNELNO];
    static MsTime              m_CCCV_Ramp_ParaModTime[MAXCHANNELNO];
    static unsigned char       CCCV_RAMP_Status[MAXCHANNELNO];
    static MsTime              CCCV_RisingTime[MAXCHANNELNO]; //judge channel is it over rising time
    static bool                b_IsRapidlyCommun[MAXCHANNELNO];
    static MsTime              m_RepidlyCommunOverTimeCheck;
    //**************************************************************************
public : // for SUPPORT_PARALLEL call
    static St_DoStepMessage 	DoStepMessageBuf[MAXCHANNELNO];	                                 //建立当前处理的信息缓冲
    static ACIM_STATUS 			ACIM_Status[MAXCHANNELNO];																				// ACIM暂停步状态指示	2015.09.22
    static bool 				AcimRequest_Send[MAXCHANNELNO];																				// 定义ACIM一次性时序请求是否发出
    static unsigned short		ACIM_CH_ON;													// 记录已接通ACIM回路的通道号，专为LBT2108HC添加
private:
    static St_StepMessage 		StepMessageBuf[MAXMESSAGE][MAXCHANNELNO];                          //建立通道Step信息队列

    static St_DisposableMSG		StepDisposableMSG[MAXCHANNELNO]; 				                 //引擎任务信息
    static unsigned char  		StepTimeTrigger[MAXCHANNELNO];                                     //计时标记用于超时
    static unsigned long 		m_DacCode[MAXCHANNELNO];
    static unsigned char 		StepRunningStatus[MAXCHANNELNO];


    static St_MsgBufManage 		MsgBufManage[MAXCHANNELNO];

    static unsigned char 		LimitTimeDelayTirgger[MAXCHANNELNO];                                 //计时标记用于超时
    static 	St_AutoRangeSwitchCurPara 	m_CurRangePara[MAXCHANNELNO];
    static MsTime 				AutoRangeSwitchStartTime[MAXCHANNELNO];
	static 	MsTime              CV_RampStageKeepTime[MAXCHANNELNO];

public:
    static MsTime VoltOutputInterval[MAXCHANNELNO]; //20200901 zyx

public:
    static void DoWork(void);                                                                 //任务执行函数
    static void MyInit(void);                                                                 //任务初始化
// 	static void EndTask(void);                                                                //任务结束

    static int  SendMsgToStep(unsigned char uc_ChannelNo,St_StepMessage StepMessage);         //(St_StepMsg StepMsg);					 //向Step传递信息  大数据体中实现
    static void ClearStepMessage(St_DoStepMessage *Message);								  //清除Step响应完毕的信息；
    static void PrepareForStop(unsigned char uc_ChannelNo);						              //准备调用引擎进行停止操作
    //理论直接输出值计算
    static float  CalculateCurrent(unsigned char uc_ChannelNo);
    static float  TranslateOutputForVoltage(unsigned char uc_ChannelNo); //04.23.2013
    static float  CalculateCRate(unsigned char uc_ChannelNo);
    static float  CalculateCRamp(unsigned char uc_ChannelNo);
    static float  CalculateCV(unsigned char uc_ChannelNo);
    static float  CalculateCStaircase(unsigned char uc_ChannelNo);
    static float  CalculatePower(unsigned char uc_ChannelNo);
    static float  CalculateLoad(unsigned char uc_ChannelNo);
    static float  CalculatePowerRamp(unsigned char uc_ChannelNo);
    static float  CalculatePowerStaircase(unsigned char uc_ChannelNo);
    static float  CalculateISimulation(unsigned char uc_ChannelNo);		//12.22.2013ADD
    static float  CalculatePSimulation(unsigned char uc_ChannelNo);   //12.22.2013ADD
    static float  CalculateLSimulation(unsigned char uc_ChannelNo);   //12.22.2013ADD
    static float  CalculateVRate(unsigned char uc_ChannelNo);
    static float  CalculateVRamp(unsigned char uc_ChannelNo);
    static float  CalculateVStaircase(unsigned char uc_ChannelNo);
    static int    SendLog(unsigned char uc_ChannelNo);	    //发送log数据
    static int    SendExtraLog(unsigned char uc_ChannelNo);
	static int	SendLastLogforCSV(unsigned char uc_ChannelNo);
    static int    Log_SMBusData(unsigned char uc_ChannelNo);   //add by zc 03.04.2019
    static int 		Log_IdleSMBusData(unsigned char uc_ChannelNo,unsigned char uc_i); //add by zc 03.04.2019
//===============09062017 TESLA SIMULATION I VOLTAGE CLAMP================
    static void  Calculate_Simu_IR(unsigned char uc_ChannelNo);						//10.04.2017 by zc
    static float  CalculateReturnIValue(unsigned char uc_ChannelNo);								//  Voltage Clamp GL
    static float  GetNextIValue(unsigned char uc_ChannelNo);
    static float  CalculateReturnPValue(unsigned char uc_ChannelNo);								//  Voltage Clamp GL
    static float  GetNextPValue(unsigned char uc_ChannelNo);
//========================================END=============================												//  Voltage Clamp GL
    static void PrepareForRun(unsigned char uc_ChannelNo);						              //准备调用引擎进行运行操作
    static void Get_StepRunInfo(unsigned char uc_ChannelNo,St_StepInfo ChannelStep);          //提取Step中的相关信息
    static unsigned char Check_PresentCtrlType(unsigned char uc_ChannelNo);

private:
    static int	PreprocessMsg(unsigned char uc_ChannelNo);  	                              //信息预处理
    static void ProcessState(unsigned char uc_ChannelNo); 						 	          //状态机处理
    static void ProcessEngineState(unsigned char uc_ChannelNo);                   //引擎状态处理
    static void ProcessLimitAckState(unsigned char uc_ChannelNo);                 //Limit应答判断状态处理
    static void StepRunMsgRoutine(unsigned char uc_ChannelNo);     				              //Step_Run_Msg信息响应过程处理
    static void StepStopMsgRoutine(unsigned char uc_ChannelNo);					              //Step_Stop_Msg信息响应过程处理
    static void StepJumpMsgRoutine(unsigned char uc_ChannelNo);				              //Step_Jump_Msg信息响应过程处理

    static void Step_LoadRoutine(unsigned char uc_ChannelNo);					              //LOADSTEP状态的瞬态响应
    static void Step_TransitionRoutine(unsigned char uc_ChannelNo);				              //TRANSITION状态的瞬态响应
    static void Step_RunningRoutine(unsigned char uc_ChannelNo);				              //RUNNING状态的稳态响应
    static void Step_WaitRoutine(unsigned char uc_ChannelNo);						          //WAIT状态的瞬态响应

    static float Init_DigiV(unsigned char uc_ChannelNo,float fVoltage);
    static void PrepareforCtrlRoutine(unsigned char uc_ChannelNo);			                  //进入Step瞬间的控制过程
    static void SetDisposabletask(unsigned char uc_ChannelNo,unsigned char CtrlType);		  //设置引擎任务
#if (IV_AUTORANGE == 1)
    static void SetAutoRange_Disposabletask(unsigned char uc_ChannelNo, unsigned char uc_IRange); //设置AutoRange量程切换引擎任务
#endif
    static void AutoRange_TransitionRoutine(unsigned char uc_ChannelNo);  //转换完成时序
    //Step运行过程相关处理函数
    static void Step_JudgeSpecialCase(unsigned char uc_ChannelNo,unsigned char CtrlType);
    static void Step_Control(unsigned char uc_ChannelNo);
    static float Step_CaculateOutput(unsigned char uc_ChannelNo);

    //特殊控制接口函数
    static void Setup_PulseTask(void);
    static void Remove_PulseTask(void);
    static void ClearStepMsg(unsigned char uc_ChannelNo,unsigned char MsgIndex);
    static void GetNextIRangeValue(unsigned char uc_ChannelNo);
    static void AutoRangeSwtichControl(unsigned char uc_ChannelNo);

public :  //DKQ 06.18.2013    //将这几个与PID有关的弄成Public 以便在第三方也能用
    static inline void Step_OutputCurrDac(unsigned char uc_ChannelNo,float fValue,bool IsKeyPiont);
    static inline void Step_OutputCurrDac_For_Power(unsigned char uc_ChannelNo,float fValue); //Add by DKQ 06.04.2014
    static void StepDAC_Out(unsigned char uc_ChannelNo,unsigned long Code,unsigned char Type);
    static void Step_OutputCurrDac_Submaster(unsigned char uc_ChannelNo,float fValue);
    static  void Step_OutputVoltDac(unsigned char uc_ChannelNo,float fValue,bool IsKeyPiont);
	static float Current_FineTune(unsigned char uc_ChannelNo, float fValue,unsigned char range);
    static unsigned char m_Range[MAXCHANNELNO];  //DKQ
    static void ProcessStepCheckLimit(unsigned char uc_ChannelNo);	//处理Limit到达事件
    static void Pulse_End(unsigned char uc_ChannelNo);
    static void SimuSamp_SendExtraLog(unsigned char uc_ChannelNo);
    static void StepClear_Adjust(unsigned char uc_ChannelNo);
public:
    static void PrepareForParallelSubGroupRun();
    static void CallForSetSlaveChanDisposble(unsigned short uc_ChannelNo, unsigned short uc_SlaveToCtrlType);
    static void  CalibrateAndSetGroupCurrentDAC(unsigned short uc_ChannelNo, float fCtrlValue);
    static void SetIntraUnitParallelSlaveChannelCtrlState(unsigned char uc_ChannelNo, unsigned char uc_ChannelCtrlState);
    static void SetSlaveChannelDisposableTask(unsigned char uc_FromChan, unsigned char uc_ToChan, unsigned char uc_NewStepCtrlType);
    static void SetIntraUnitParallelSlaveDisposable(unsigned char uc_ChannelNo,  unsigned short u_CtrlType, float f_Set);
    static  float CalculateGroupChanCtrlValue(unsigned short uChanIdx, float fSet, unsigned short uCtrlType);
    static void AUXAckFailRoutine(unsigned char uc_ChannelNo);
    static float GetIRValue(unsigned char uc_ChannelNo);
    static unsigned char IfParaModStepRunning(unsigned char uc_ChannelNo);

public:
    static float Get_IR_Value(unsigned char uc_ChannelNo);
    static void CCCV_StepControl(unsigned char uc_ChannelNo);
    static void CCCV_Init(unsigned char uc_ChannelNo);
    static void CCCV_SampleAndCalculateRampStage(unsigned char uc_ChannelNo, float TargetV,float TargetI);

    static bool IsGetNewSample(unsigned char uc_ChannelNo,unsigned char CCCV_RAMP_Status = NO_RAMP);
    static bool IsCCCV_VoltageArrived(unsigned char uc_ChannelNo,float TargetV,float TargetI);
    static void CCCV_RampCalculateIR(unsigned char uc_ChannelNo);
    static void ContorlChnSampleFilter(unsigned char chn, bool IsChnNeedFilter,bool IsCCCV_Ramp);
    static float EstimateIRValue(unsigned char uc_ChannelNo);
    static void CCCV_CV_Init(unsigned char uc_ChannelNo, float TargetV, float SampleV);
    static bool IsNoChnNeedRapidlyCommunicate(void);
    static void CheckRapidlyCommunicate(void);
    static void RapidlyCommunicateInit(void);

};


