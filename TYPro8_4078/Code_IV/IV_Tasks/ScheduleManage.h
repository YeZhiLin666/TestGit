// *****************************************************
// File Name: Schedule.h
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         Schedule操作各种相关类定义
// Others:
// History：
//
// ******************************************************/
#include "../IV_Tasks/PowerAdj.h"

#pragma once

#define SPECIALLIMITID 0xffff
#define EMPTYCOUNT    7
#define MAXMESSAGE    8
//#define MAXLIMITNO    50
//#define MAXSAFETYNO   50
#define uw32_OverTime   2*50000 //50000//15000    //上面的时间太短，Tesla通道打开时间较长，不能用上面的判断  comment out chen 20130426
// UDP_AUX_LIMIT_RATE * 50000 = 10s
// #define Aux_OverTime  50000 // 1000  //判断Aux是否存在？

#define SENDDELAY    200*10		//发送7延时
#define UnsafeTimeInterval 2

typedef struct
{
    unsigned char BufCount;
    unsigned char Priority;
} St_HighPriority;

typedef struct
{
    unsigned char      Used;
    St_ScheduleMessage ScheduleMessage;
} St_DoMessage;         //需要处理的Message结构

typedef struct
{
    unsigned char RecordMsgFull[MAXMESSAGE];
    int           FullCount;
    unsigned char RecordMsgEmpty[MAXMESSAGE];
    int           EmptyCount;
    unsigned char RecordMsgDeleting[MAXMESSAGE];
    int           DeletingCount;
} St_MsgBufManage;

typedef struct
{
    unsigned short m_ResumeStepID;
    unsigned short m_ResumeCycleID;
    double m_ResumeStepTime;
    double m_ResumeTestTime;
    unsigned char m_ResumeStatus;
} St_ResumeManage;

typedef struct
{
//	bool usingFlag; //设备处于被使用状态，占用中为true
//	bool OpenFlag;  //设备开关状态，true为open
    unsigned char usingFlag;  //设备处于被使用状态，占用中为true
    unsigned char OpenFlag;   //设备开关状态，true为open
    unsigned char uc_channel[MAXMTCCHAMBERNO]; //开启设备的通道
    unsigned char m_ucType[MAXMTCCHAMBERNO];   //开启了设备的哪种类型
    unsigned short m_uwAuxVirtIndx[MAXMTCCHAMBERNO]; //AUX虚拟ID
} GlobalDeviceSwitch; //用于监控设备开关状态 zyx20160707

typedef struct
{
    unsigned char StopStatus;
    unsigned char SubStopCode;
} StopMsg;

enum            //Assign+Start 结果
{
    ERROR,
    SCHEDULERUNNING=1,
    SCHEDULEUNSAGE,
    ASSIGNOK,
    ASSIGNFAIL,
};

enum
{
    NOAUXCOMMAND,
    SENDAUXCOMMAND,
    WAITFORAUXACK,
    GOTONEXT,
    AUXACKOK,
    AUXACKFAIL,

    SENDCLEARLIMIT,
    WAITFORCLEARLIMITACK,
    CLEARLIMITACKFAIL,
    CLEARLIMITACKOK,
};
enum
{
    CONTINUE,
    JUMP,
};
// enum
// {
// 	Wait_IDLE=0,
// 	Wait_StartACK_M01,		//发送启动信息到M0
// 	Wait_StartACK_M02,
// 	Wait_StartACK_M03,
// 	Wait_StartACK_M04,		//等待M0应答
// };
/*****************************************************
//Class Name: Schedule_Manager_task
//Version:        V1.0
//Date:                2012/11/12
//Description:
//                        Schedule管理类
//Others:
//History：
******************************************************/
class Schedule_Manager_task
{
public:                //MAXCHANNELNO仅用于测试，实际条件下使用BoardCFG.m_Global.m_ucIVChannelCount
    static UInt16 MyStatus;                                                  //任务状态
    static UInt16 TaskID;                                                    //任务ID
    static UInt16 Priority;                                                  //优先级

		static bool IsNeedSetStopPoint[MAXCHANNELNO];
	
    static unsigned short IV_AUXTYPE[MAXCHANNELNO];
    static St_ResumeManage ResumeManage[MAXCHANNELNO];

    static unsigned char LockModeSwitch[MAXCHANNELNO];                       //状态锁定模式开关
    static unsigned char  SchTimeTrigger[MAXCHANNELNO];
    static StopMsg SendFinishFlag[MAXCHANNELNO];			//Stop原因标记

    static unsigned char bSendedClrLimit[MAXCHANNELNO];
    static unsigned short STEP_AUXLIMITBIT[MAXCHANNELNO];
    static unsigned char STEP_NEXT[MAXCHANNELNO];
    static unsigned short SendAUX_SafetyFlg[MAXCHANNELNO];
    static unsigned char SendClearLimitStatus[MAXCHANNELNO];
    static unsigned char      bMetaSyncSend[MAXCHANNELNO];
    static St_ScheduleMessage UnsafeContent[MAXCHANNELNO];     //通道异常信息
    static unsigned char IRSpecialTestFlg[MAXCHANNELNO];		//IR异常测试标记
    static unsigned char IRHasBeenRun[MAXCHANNELNO];
    static GlobalDeviceSwitch TemperatureChamberSwitch[MAXMCUUNIT]; //温控箱标志位  changed by zc 04.25.2018
    //static bool    SCH_RESUME_Flag[MAXCHANNELNO];   //RESUME 启动标志  add by qjm 20141016
    static unsigned char MetaCode_List[MAX_SEND_METAVARIABLE-2];

    static MsTime SDLIdleMsTime[MAXCHANNELNO];	//0819
	static MsTime SDLShowMsTime[MAXCHANNELNO];	//0819	
// 	static unsigned char  M4WaitACKOvertimeFlag[MAXCHANNELNO]; //add by qjm 20180710
// 	static unsigned char  uc_StartOrRestartFlag;      //0: start;  1: restart
// 	static unsigned char  M4ReceiveACKCnt;        //add by qjm 20180803
// 	static unsigned char  M4RestartSentCnt;   //发送的Restart 命令条数
// // 	static MsTime  M0FeedBackCommon[MAXCHANNELNO];            // add by qjm 20180710
//     static unsigned short  M4WaitTimeCnt[MAXCHANNELNO];  //定时20ms发送一次
// 	static unsigned short M4WaitPacketCnt[MAXCHANNELNO];    //定时500ms 发送一次
// 	static unsigned char  M4SendComCounter[MAXCHANNELNO];   //最多发送五次， 100ms 后不再发送。
// 	static unsigned char  M4SendPacketCounter[MAXCHANNELNO]; //每个500ms 发送一次，最多发送四次， 两秒没有回复报警。
// 	static unsigned char  M4SendStartStatus[MAXCHANNELNO];
// 	static bool           Receive_M0_StartedACKFlag;
    static unsigned short M_SysALarmEmergencyFlg;

    static unsigned long m_dwTestID[MAXCHANNELNO];  //mits8
		static St_ScheduleMessage  StartScheduleUnsafe[MAXCHANNELNO];

private:

    static St_ScheduleMessage ScheduleMessageBuf[MAXMESSAGE][MAXCHANNELNO];  //建立通道Schedule信息队列
    static St_DoMessage DoMessageBuf[MAXCHANNELNO];                          //建立当前处理的信息缓冲
    static MsTime SysTimeEngineTask[MAXCHANNELNO];	//0819
    static MsTime  MsTimeEngineTask[MAXCHANNELNO];
    static unsigned short EngineHandle[MAXCHANNELNO];
    static unsigned char EngineOverTimeTriger[MAXCHANNELNO];
    static St_MsgBufManage MsgBufManage[MAXCHANNELNO];
//	static unsigned char SendTirgger[MAXCHANNELNO];                           //触发发送开关
//	static MsTime SysTimeSendTask[MAXCHANNELNO];	//0819

    static unsigned char SendGoStopFlag[MAXCHANNELNO];			//Gostop发送标记
// 	static MsTime SDLTimeTicksForIdle[MAXCHANNELNO];				//显示/闲时 数据发送定时参数
//	static MsTime SDLTimeTicksForShow[MAXCHANNELNO];
// 	static MsTime SDLIdleMsTime[MAXCHANNELNO];	//0819
//	static MsTime SDLShowMsTime[MAXCHANNELNO];	//0819
//	static MsTime UnsafeTimeTicksForFixed[MAXCHANNELNO];
//	static MsTime UnsafeTimeTicksForDynamic[MAXCHANNELNO];
//	static MsTime FixedUnsafeMsTime[MAXCHANNELNO];	//0819
    static MsTime DynamicUnsafeMsTime[MAXCHANNELNO];	//0819

    static int SchAssignRoutine(St_Assign * AssignMsg);
    static void  DeleteScheduleMsg(unsigned char uc_ChannelNo,unsigned char MsgCount);           //删除队列中指定一条信息
    static int SimulationAssignRoutine(unsigned char uc_ChannelNo,St_MD5Assign * AssignMsg); //12.22.2013ADD

    static unsigned char IR_UnsafeFlg[MAXCHANNELNO];              //判断IR异常标记
    static unsigned long TirggerIDLECounter[MAXCHANNELNO];
// 	static unsigned long BuiltInSDL_TriggerCounter[MAXCHANNELNO];
//   static  bool b_start2resumeflag[MAXCHANNELNO];     //0: Start 命令，      1： resume 命令
public:
    static void DoWork(void);                                                                   //任务执行函数
    static void MyInit(void);                                                                   //任务初始化
    static void EndTask(void);                                                                  //任务结束
    static int  SendMsgToSchedule(unsigned char uc_ChannelNo,St_ScheduleMessage ScheduleMessage);
    static void ClearScheduleMessage(St_DoMessage *Message);                                    //清除Schedule响应完毕的信息
    static unsigned char IfSchRunning(unsigned char uc_ChannelNo);
    static unsigned char SchAssignAddStarRoutine(St_Assign * AssignMsg);
    static unsigned char SimulationAssignAddStarRoutine(unsigned char uc_ChannelNo,St_MD5Assign * AssignMsg);  //12.22.2013ADD
    static void SetSetVariable(unsigned char uc_ChannelNo);																								//取得Sch状态
    static void SetSetValue(unsigned char uc_ChannelNo);
    static void StopChannel(unsigned char uc_ChannelNo);//取得Sch状态
    static unsigned char  GetSetVariable(unsigned long uw32_Var,unsigned char uc_Pos);          //取得SetVariable条件
		static void SchReportStartScheduleUnsafeMsgToPC(unsigned char uc_ChannelNo, St_ScheduleMessage * UnsafeInfo);
	static void SDLIdeShowMsTime_Updata(void);
    static void SendMetaSync(unsigned char uc_ChannelNo);

private:
    static void PreprocessSlaveUnsafeMsg(unsigned char uc_ChannelNo);
    static int  PreprocessMsg(unsigned char uc_ChannelNo);                                      //信息预处理
    static void ProcessState(unsigned char uc_ChannelNo);                                       //状态机处理

    //static void SchAssignMsgRoutineWithNOTREADY(unsigned char uc_ChannelNo);                    //Sch_Assign_Msg信息响应过程处理(NOTREADY)
    //static void SchAssignMsgRoutineWithIDLE(unsigned char uc_ChannelNo);                        //Sch_Assign_Msg信息响应过程处理(IDLE)
    static void SchOnUnsafeMsgRoutine(unsigned char uc_ChannelNo);                              //Sch_OnUnsafe_Msg信息响应过程处理
    //static void SchOnSafeMsgRoutine(unsigned char uc_ChannelNo);                          //Sch_OnSafe_Msg信息响应过程处理
    static void SchRunMsgRoutine(unsigned char uc_ChannelNO);      //Sch_Run_Msg信息响应过程处理
    static void SchResumeMsgRoutine(unsigned char uc_ChannelNo);   //Sch_Resume_Msg信息响应过程处理
    static void SchStopMsgRoutine(unsigned char uc_ChannelNo);                                  //Sch_Stop_Msg信息响应过程处理
    static void SchJumpAMsgRoutine(unsigned char uc_ChannelNo);                                 //Sch_JumpA_Msg信息响应过程处理
    static void SchJumpMMsgRoutine(unsigned char uc_ChannelNo);                                 //Sch_JumpM_Msg信息响应过程处理
    static void SchContinueMsgRoutine(unsigned char uc_ChannelNo);
    static void SchLimitArriveMsgRoutine(unsigned char uc_ChannelNo);                           //Sch_LimitArrive_Msg信息响应过程处理
    //static void SchLimitPropertyChangeMsgRoutine(unsigned char un_ChannelNo);                   //Sch_LimitPropertyChange_Msg信息响应过程处理
    static void SchLimitStatusChangeMsgRoutine(unsigned char uc_ChannelNo);                     //Sch_LimitStatusChanged_Msg信息响应过程处理
    static void SchStartedMsgRoutine(unsigned char uc_ChannelNo);                               //Sch_Started_Msg信息响应过程处理
    static void SchStoppedMsgRoutine(unsigned char uc_ChannelNo);
    static void SchStartRoutine(unsigned char uc_ChannelNo);                                    //START状态的无信息输入处理过程
    static void SchRunningRoutine(unsigned char uc_ChannelNo);                                  //RUNNING状态的无信息输入处理过程
    static void SchGopauseRoutine(unsigned char uc_ChannelNo);                                  //GOPAUSE状态的无信息输入处理过程
    static void SchResumeRoutine(unsigned char uc_ChannelNo);                                   //RESUME状态的无信息输入处理过程
    static void SchGoStopRoutine(unsigned char uc_ChannelNo);                                   //GOSTOP状态的无信息输入处理过程
    static void Sch_FixedUnsafeRoutine(unsigned char uc_ChannelNo);
//	static void Sch_DynamicUnsafeRoutine(unsigned char uc_ChannelNo);
//	static unsigned char  GetSetVariable(unsigned long uw32_Var,unsigned char uc_Pos);          //取得SetVariable条件
// 	static unsigned char RecordLastStepData(unsigned char uc_ChannelNo);                        //记录上一步骤的LS相关变量
	//static void SendSDLIdleData(unsigned char uc_ChannelNo,unsigned long long TimeInterval);      //发送闲时数据
	static void SendSDLIdleData(unsigned char uc_ChannelNo);      //发送闲时数据
	//static void SendSDLShowData(unsigned char uc_ChannelNo,unsigned long long TimeInterval);      //发送显示数据
	static void SendSDLShowData(unsigned char uc_ChannelNo);      //发送显示数据
    static void SoftWare_WatchDog(void);
    static unsigned char IRspecial_Judge(unsigned char uc_ChannelNo);
    static void SchReportMessageToPC(unsigned char uc_ChannelNo,unsigned char CMD,St_ScheduleMessage * UnsafeContent);
    static void IRspecial_Judge_Process(unsigned char uc_ChannelNo);
    static void Sch_BuiltinAuxSafety_Load(unsigned char uc_ChannelNo);
//#if SUPPORT_AUX == 1
    static unsigned char SCH_SendTOAUXStatus[MAXCHANNELNO];
    static void WaitForAUXAckRoutine(unsigned char uc_ChannelNo);

    static bool Aux_WaitForClearLimitAckRoutine(unsigned char uc_ChannelNo);
    static void Aux_ClearLimitAckFailRoutine(unsigned char uc_ChannelNo);
    static void ScheduleToStopIdle(unsigned char uc_ChannelNo);
// 	static void PowerAdjNeedCheck(unsigned char uc_ChannelNo,bool b_Start2ResumeFlag);
// 	static void SchWaitRoutine(unsigned char uc_ChannelNo);
//#endif
public:
    //AUX添加部分20131120
//#if SUPPORT_AUX == 1
    static void SendScheduleMapToAUX(unsigned char uc_ChannelNo);
    static void SendSafetyInfoToAUX(unsigned char uc_ChannelNo);
    static void SendSDLParaToAUX(unsigned char uc_ChannelNo);
    static void AuxOnUnsafeMsgRoutine(unsigned char uc_ChannelNo);
    static void AuxACKFAILMsgRoutine(unsigned char uc_ChannelNo);
    static void ClearMapAUXSafetyInfo(unsigned char uc_ChannelNo,unsigned char Auxtype,unsigned char VirtualID,unsigned char ClearType);
    static int ClearAUXLimits(unsigned char uc_ChannelNo,unsigned char Clear4New);
    static void SchWaitForAuxRoutine(unsigned char uc_ChannelNo);
    static void UseTemperatureChamber( unsigned char uc_channel, unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //占用温控箱  changed by zc 04.25.2018
    static void RelsaseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //释放温控箱的占用  changed by zc 04.25.2018
    static void OpenTemperatureChamber(unsigned short m_uwAuxVirtIndx,unsigned char m_ucType, unsigned char AuxUnitIdx, unsigned char AuxChnNo); //打开温控箱  changed by zc 04.25.2018
    static void CloseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //关闭温控箱   changed by zc 04.25.2018

//#endif
};


