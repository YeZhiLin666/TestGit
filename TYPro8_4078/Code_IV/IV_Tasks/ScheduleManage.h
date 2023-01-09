// *****************************************************
// File Name: Schedule.h
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         Schedule������������ඨ��
// Others:
// History��
//
// ******************************************************/
#include "../IV_Tasks/PowerAdj.h"

#pragma once

#define SPECIALLIMITID 0xffff
#define EMPTYCOUNT    7
#define MAXMESSAGE    8
//#define MAXLIMITNO    50
//#define MAXSAFETYNO   50
#define uw32_OverTime   2*50000 //50000//15000    //�����ʱ��̫�̣�Teslaͨ����ʱ��ϳ���������������ж�  comment out chen 20130426
// UDP_AUX_LIMIT_RATE * 50000 = 10s
// #define Aux_OverTime  50000 // 1000  //�ж�Aux�Ƿ���ڣ�

#define SENDDELAY    200*10		//����7��ʱ
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
} St_DoMessage;         //��Ҫ�����Message�ṹ

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
//	bool usingFlag; //�豸���ڱ�ʹ��״̬��ռ����Ϊtrue
//	bool OpenFlag;  //�豸����״̬��trueΪopen
    unsigned char usingFlag;  //�豸���ڱ�ʹ��״̬��ռ����Ϊtrue
    unsigned char OpenFlag;   //�豸����״̬��trueΪopen
    unsigned char uc_channel[MAXMTCCHAMBERNO]; //�����豸��ͨ��
    unsigned char m_ucType[MAXMTCCHAMBERNO];   //�������豸����������
    unsigned short m_uwAuxVirtIndx[MAXMTCCHAMBERNO]; //AUX����ID
} GlobalDeviceSwitch; //���ڼ���豸����״̬ zyx20160707

typedef struct
{
    unsigned char StopStatus;
    unsigned char SubStopCode;
} StopMsg;

enum            //Assign+Start ���
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
// 	Wait_StartACK_M01,		//����������Ϣ��M0
// 	Wait_StartACK_M02,
// 	Wait_StartACK_M03,
// 	Wait_StartACK_M04,		//�ȴ�M0Ӧ��
// };
/*****************************************************
//Class Name: Schedule_Manager_task
//Version:        V1.0
//Date:                2012/11/12
//Description:
//                        Schedule������
//Others:
//History��
******************************************************/
class Schedule_Manager_task
{
public:                //MAXCHANNELNO�����ڲ��ԣ�ʵ��������ʹ��BoardCFG.m_Global.m_ucIVChannelCount
    static UInt16 MyStatus;                                                  //����״̬
    static UInt16 TaskID;                                                    //����ID
    static UInt16 Priority;                                                  //���ȼ�

		static bool IsNeedSetStopPoint[MAXCHANNELNO];
	
    static unsigned short IV_AUXTYPE[MAXCHANNELNO];
    static St_ResumeManage ResumeManage[MAXCHANNELNO];

    static unsigned char LockModeSwitch[MAXCHANNELNO];                       //״̬����ģʽ����
    static unsigned char  SchTimeTrigger[MAXCHANNELNO];
    static StopMsg SendFinishFlag[MAXCHANNELNO];			//Stopԭ����

    static unsigned char bSendedClrLimit[MAXCHANNELNO];
    static unsigned short STEP_AUXLIMITBIT[MAXCHANNELNO];
    static unsigned char STEP_NEXT[MAXCHANNELNO];
    static unsigned short SendAUX_SafetyFlg[MAXCHANNELNO];
    static unsigned char SendClearLimitStatus[MAXCHANNELNO];
    static unsigned char      bMetaSyncSend[MAXCHANNELNO];
    static St_ScheduleMessage UnsafeContent[MAXCHANNELNO];     //ͨ���쳣��Ϣ
    static unsigned char IRSpecialTestFlg[MAXCHANNELNO];		//IR�쳣���Ա��
    static unsigned char IRHasBeenRun[MAXCHANNELNO];
    static GlobalDeviceSwitch TemperatureChamberSwitch[MAXMCUUNIT]; //�¿����־λ  changed by zc 04.25.2018
    //static bool    SCH_RESUME_Flag[MAXCHANNELNO];   //RESUME ������־  add by qjm 20141016
    static unsigned char MetaCode_List[MAX_SEND_METAVARIABLE-2];

    static MsTime SDLIdleMsTime[MAXCHANNELNO];	//0819
	static MsTime SDLShowMsTime[MAXCHANNELNO];	//0819	
// 	static unsigned char  M4WaitACKOvertimeFlag[MAXCHANNELNO]; //add by qjm 20180710
// 	static unsigned char  uc_StartOrRestartFlag;      //0: start;  1: restart
// 	static unsigned char  M4ReceiveACKCnt;        //add by qjm 20180803
// 	static unsigned char  M4RestartSentCnt;   //���͵�Restart ��������
// // 	static MsTime  M0FeedBackCommon[MAXCHANNELNO];            // add by qjm 20180710
//     static unsigned short  M4WaitTimeCnt[MAXCHANNELNO];  //��ʱ20ms����һ��
// 	static unsigned short M4WaitPacketCnt[MAXCHANNELNO];    //��ʱ500ms ����һ��
// 	static unsigned char  M4SendComCounter[MAXCHANNELNO];   //��෢����Σ� 100ms ���ٷ��͡�
// 	static unsigned char  M4SendPacketCounter[MAXCHANNELNO]; //ÿ��500ms ����һ�Σ���෢���ĴΣ� ����û�лظ�������
// 	static unsigned char  M4SendStartStatus[MAXCHANNELNO];
// 	static bool           Receive_M0_StartedACKFlag;
    static unsigned short M_SysALarmEmergencyFlg;

    static unsigned long m_dwTestID[MAXCHANNELNO];  //mits8
		static St_ScheduleMessage  StartScheduleUnsafe[MAXCHANNELNO];

private:

    static St_ScheduleMessage ScheduleMessageBuf[MAXMESSAGE][MAXCHANNELNO];  //����ͨ��Schedule��Ϣ����
    static St_DoMessage DoMessageBuf[MAXCHANNELNO];                          //������ǰ�������Ϣ����
    static MsTime SysTimeEngineTask[MAXCHANNELNO];	//0819
    static MsTime  MsTimeEngineTask[MAXCHANNELNO];
    static unsigned short EngineHandle[MAXCHANNELNO];
    static unsigned char EngineOverTimeTriger[MAXCHANNELNO];
    static St_MsgBufManage MsgBufManage[MAXCHANNELNO];
//	static unsigned char SendTirgger[MAXCHANNELNO];                           //�������Ϳ���
//	static MsTime SysTimeSendTask[MAXCHANNELNO];	//0819

    static unsigned char SendGoStopFlag[MAXCHANNELNO];			//Gostop���ͱ��
// 	static MsTime SDLTimeTicksForIdle[MAXCHANNELNO];				//��ʾ/��ʱ ���ݷ��Ͷ�ʱ����
//	static MsTime SDLTimeTicksForShow[MAXCHANNELNO];
// 	static MsTime SDLIdleMsTime[MAXCHANNELNO];	//0819
//	static MsTime SDLShowMsTime[MAXCHANNELNO];	//0819
//	static MsTime UnsafeTimeTicksForFixed[MAXCHANNELNO];
//	static MsTime UnsafeTimeTicksForDynamic[MAXCHANNELNO];
//	static MsTime FixedUnsafeMsTime[MAXCHANNELNO];	//0819
    static MsTime DynamicUnsafeMsTime[MAXCHANNELNO];	//0819

    static int SchAssignRoutine(St_Assign * AssignMsg);
    static void  DeleteScheduleMsg(unsigned char uc_ChannelNo,unsigned char MsgCount);           //ɾ��������ָ��һ����Ϣ
    static int SimulationAssignRoutine(unsigned char uc_ChannelNo,St_MD5Assign * AssignMsg); //12.22.2013ADD

    static unsigned char IR_UnsafeFlg[MAXCHANNELNO];              //�ж�IR�쳣���
    static unsigned long TirggerIDLECounter[MAXCHANNELNO];
// 	static unsigned long BuiltInSDL_TriggerCounter[MAXCHANNELNO];
//   static  bool b_start2resumeflag[MAXCHANNELNO];     //0: Start ���      1�� resume ����
public:
    static void DoWork(void);                                                                   //����ִ�к���
    static void MyInit(void);                                                                   //�����ʼ��
    static void EndTask(void);                                                                  //�������
    static int  SendMsgToSchedule(unsigned char uc_ChannelNo,St_ScheduleMessage ScheduleMessage);
    static void ClearScheduleMessage(St_DoMessage *Message);                                    //���Schedule��Ӧ��ϵ���Ϣ
    static unsigned char IfSchRunning(unsigned char uc_ChannelNo);
    static unsigned char SchAssignAddStarRoutine(St_Assign * AssignMsg);
    static unsigned char SimulationAssignAddStarRoutine(unsigned char uc_ChannelNo,St_MD5Assign * AssignMsg);  //12.22.2013ADD
    static void SetSetVariable(unsigned char uc_ChannelNo);																								//ȡ��Sch״̬
    static void SetSetValue(unsigned char uc_ChannelNo);
    static void StopChannel(unsigned char uc_ChannelNo);//ȡ��Sch״̬
    static unsigned char  GetSetVariable(unsigned long uw32_Var,unsigned char uc_Pos);          //ȡ��SetVariable����
		static void SchReportStartScheduleUnsafeMsgToPC(unsigned char uc_ChannelNo, St_ScheduleMessage * UnsafeInfo);
	static void SDLIdeShowMsTime_Updata(void);
    static void SendMetaSync(unsigned char uc_ChannelNo);

private:
    static void PreprocessSlaveUnsafeMsg(unsigned char uc_ChannelNo);
    static int  PreprocessMsg(unsigned char uc_ChannelNo);                                      //��ϢԤ����
    static void ProcessState(unsigned char uc_ChannelNo);                                       //״̬������

    //static void SchAssignMsgRoutineWithNOTREADY(unsigned char uc_ChannelNo);                    //Sch_Assign_Msg��Ϣ��Ӧ���̴���(NOTREADY)
    //static void SchAssignMsgRoutineWithIDLE(unsigned char uc_ChannelNo);                        //Sch_Assign_Msg��Ϣ��Ӧ���̴���(IDLE)
    static void SchOnUnsafeMsgRoutine(unsigned char uc_ChannelNo);                              //Sch_OnUnsafe_Msg��Ϣ��Ӧ���̴���
    //static void SchOnSafeMsgRoutine(unsigned char uc_ChannelNo);                          //Sch_OnSafe_Msg��Ϣ��Ӧ���̴���
    static void SchRunMsgRoutine(unsigned char uc_ChannelNO);      //Sch_Run_Msg��Ϣ��Ӧ���̴���
    static void SchResumeMsgRoutine(unsigned char uc_ChannelNo);   //Sch_Resume_Msg��Ϣ��Ӧ���̴���
    static void SchStopMsgRoutine(unsigned char uc_ChannelNo);                                  //Sch_Stop_Msg��Ϣ��Ӧ���̴���
    static void SchJumpAMsgRoutine(unsigned char uc_ChannelNo);                                 //Sch_JumpA_Msg��Ϣ��Ӧ���̴���
    static void SchJumpMMsgRoutine(unsigned char uc_ChannelNo);                                 //Sch_JumpM_Msg��Ϣ��Ӧ���̴���
    static void SchContinueMsgRoutine(unsigned char uc_ChannelNo);
    static void SchLimitArriveMsgRoutine(unsigned char uc_ChannelNo);                           //Sch_LimitArrive_Msg��Ϣ��Ӧ���̴���
    //static void SchLimitPropertyChangeMsgRoutine(unsigned char un_ChannelNo);                   //Sch_LimitPropertyChange_Msg��Ϣ��Ӧ���̴���
    static void SchLimitStatusChangeMsgRoutine(unsigned char uc_ChannelNo);                     //Sch_LimitStatusChanged_Msg��Ϣ��Ӧ���̴���
    static void SchStartedMsgRoutine(unsigned char uc_ChannelNo);                               //Sch_Started_Msg��Ϣ��Ӧ���̴���
    static void SchStoppedMsgRoutine(unsigned char uc_ChannelNo);
    static void SchStartRoutine(unsigned char uc_ChannelNo);                                    //START״̬������Ϣ���봦�����
    static void SchRunningRoutine(unsigned char uc_ChannelNo);                                  //RUNNING״̬������Ϣ���봦�����
    static void SchGopauseRoutine(unsigned char uc_ChannelNo);                                  //GOPAUSE״̬������Ϣ���봦�����
    static void SchResumeRoutine(unsigned char uc_ChannelNo);                                   //RESUME״̬������Ϣ���봦�����
    static void SchGoStopRoutine(unsigned char uc_ChannelNo);                                   //GOSTOP״̬������Ϣ���봦�����
    static void Sch_FixedUnsafeRoutine(unsigned char uc_ChannelNo);
//	static void Sch_DynamicUnsafeRoutine(unsigned char uc_ChannelNo);
//	static unsigned char  GetSetVariable(unsigned long uw32_Var,unsigned char uc_Pos);          //ȡ��SetVariable����
// 	static unsigned char RecordLastStepData(unsigned char uc_ChannelNo);                        //��¼��һ�����LS��ر���
	//static void SendSDLIdleData(unsigned char uc_ChannelNo,unsigned long long TimeInterval);      //������ʱ����
	static void SendSDLIdleData(unsigned char uc_ChannelNo);      //������ʱ����
	//static void SendSDLShowData(unsigned char uc_ChannelNo,unsigned long long TimeInterval);      //������ʾ����
	static void SendSDLShowData(unsigned char uc_ChannelNo);      //������ʾ����
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
    //AUX��Ӳ���20131120
//#if SUPPORT_AUX == 1
    static void SendScheduleMapToAUX(unsigned char uc_ChannelNo);
    static void SendSafetyInfoToAUX(unsigned char uc_ChannelNo);
    static void SendSDLParaToAUX(unsigned char uc_ChannelNo);
    static void AuxOnUnsafeMsgRoutine(unsigned char uc_ChannelNo);
    static void AuxACKFAILMsgRoutine(unsigned char uc_ChannelNo);
    static void ClearMapAUXSafetyInfo(unsigned char uc_ChannelNo,unsigned char Auxtype,unsigned char VirtualID,unsigned char ClearType);
    static int ClearAUXLimits(unsigned char uc_ChannelNo,unsigned char Clear4New);
    static void SchWaitForAuxRoutine(unsigned char uc_ChannelNo);
    static void UseTemperatureChamber( unsigned char uc_channel, unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //ռ���¿���  changed by zc 04.25.2018
    static void RelsaseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //�ͷ��¿����ռ��  changed by zc 04.25.2018
    static void OpenTemperatureChamber(unsigned short m_uwAuxVirtIndx,unsigned char m_ucType, unsigned char AuxUnitIdx, unsigned char AuxChnNo); //���¿���  changed by zc 04.25.2018
    static void CloseTemperatureChamber( unsigned char AuxUnitIdx, unsigned char AuxChnNo ); //�ر��¿���   changed by zc 04.25.2018

//#endif
};


