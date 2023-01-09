// *****************************************************
// File Name: CanbusWatchdog.h
// Version:        V1.0
// Date:                2015/08/19
// Description:
//              Can通讯看门狗
// Others:
// History：
//
// ******************************************************/
#ifndef __CANBUSWATCHDOG_H
#define __CANBUSWATCHDOG_H
#pragma once

#define MAXSUBUNIT 255
#define MAXMASTERUNIT 255
#define MAXAUXUNIT	255
#define CANWATCHACKOVERTIME  200;
#define MAXIVMAPAUXUNIT  16
#define MAXDELAYCOUNTER  100;
#define MAXDRESENDCOUNTER  20 //200

#define PARALLEL_WATCHDOG_MAX_SN    230  // 255
#define PARALLEL_WATCHDOG_ACKFAILD_SN   245

enum
{
    CAN_WATCHDOG_MASTERUNIT_IDLE,
    CAN_WATCHDOG_MASTERUNIT_WATIFORSENDACK,
    CAN_WATCHDOG_MASTERUNIT_SENDTOSUBUNIT,
    CAN_WATCHDOG_MASTERUNIT_WAITSUBUNITACK,
    CAN_WATCHDOG_MASTERUNIT_ACKFAILD,
};

enum
{
    CAN_WATCHDOG_SUBUNIT_IDLE,
    CAN_WATCHDOG_SUBUNIT_WAITMASTERUNITSN,
    CAN_WATCHDOG_SUBUNIT_SENDACKTOMASTERUNIT,
    CAN_WATCHDOG_SUBUNIT_ACKFAILD,
};

enum
{
    CAN_WATCHDOG_IVUNIT_IDLE,
    CAN_WATCHDOG_IVUNIT_SENDTOAUXUNIT,
    CAN_WATCHDOG_IVUNIT_WAITAUXUNITACK,
    CAN_WATCHDOG_IVUNIT_ACKFAILD,
};

enum
{
    CAN_WATCHDOG_AUXUNIT_IDLE,
    CAN_WATCHDOG_AUXUNIT_WAITIVUNITSN,
    CAN_WATCHDOG_AUXUNIT_SENDACKTOIVUNIT,
    CAN_WATCHDOG_AUXUNIT_ACKFAILD,
};
enum
{
    WATCHDOG_NA=0,
    WATCHDOG_MASTERSEND,
    WATCHDOG_MASTERRCV,
    WATCHDOG_SUBSEND,
    WATCHDOG_IVSENDTOAUX,
    WATCHDOG_IVRCVAUX,
    WATCHDOG_AUXSEND,
};

typedef struct
{
    unsigned char m_MasterUnitID;           //作为从时主编号
    unsigned char m_SubUnitID[MAXSUBUNIT];	//作为主时从编号
    unsigned char m_MasterSendSN[MAXSUBUNIT];//主向从发确号
    unsigned long m_MasterReSendCount[MAXSUBUNIT];
    unsigned char m_MasterReceiveACK[MAXSUBUNIT];//主接受从的应答号
    unsigned char m_SubACKSN;                     //从向主发应答号
    unsigned char m_SubUnitCount;             //从的数量
    unsigned char m_IVSendSN[MAXIVMAPAUXUNIT];
    unsigned char m_IVReSendSNCount[MAXIVMAPAUXUNIT];
    unsigned char m_AuxAck[MAXIVMAPAUXUNIT];
    unsigned char m_AuxUnitID[MAXIVMAPAUXUNIT];
    unsigned char m_AuxUnitCanType[MAXIVMAPAUXUNIT];
    unsigned char m_AuxUnitCount;
    unsigned short m_IVChannelMapFlg[MAXIVMAPAUXUNIT];             //IV 通道map辅助记录 16bit
    unsigned short m_IVChannelMapMask;                             //没有运行辅助的就屏蔽掉了
    //unsigned char be_SLaveUnit;               //是否具备主功能
    //unsigned char be_MasterUnit;              //是否具备从功能
    bool		   m_bIsSubStatusSync;
	bool       m_bIsNeedSubStatusSync[MAXIVMAPAUXUNIT];//是否需要状态同步，复杂串并联的booster和series不需要
    unsigned long  m_SubUnitLastSyncTime[MAXSUBUNIT];
} ST_CANWATCHDOGACK;

typedef struct
{
    bool		   m_bIsChanOpen;
	unsigned char  m_ucChanCtrl;
} ST_CANWATCHDOG_CHANNEL_STATE;

class CAN_WatchdogTask
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级
    static MsTime LastSyncTime;//161028
    static bool   IsSubMasterSetOpen;//161028
public:
    static ST_CANWATCHDOGACK m_CanWatchdogACK;
    static unsigned char m_CanWatchdogMasterUnitACKStatus;
    static unsigned char m_CanWatchdogSubMasterUnitACKStatus;
    static unsigned char m_CanWatchdogIVUnitACKStatus;
    static unsigned char m_CanWatchdogAuxUnitACKstatus;
    static unsigned char m_CanWatchdogSafetyStatus;
    static bool		MasterUnit_beNeedWatchdogflg;
    //static unsigned long DelayCounter;
    static MsTime        DelayWaitTime;
// 	static bool		IV_beNeedWatchdogflg;
    static bool		Aux_beNeedWatchdogflg;
    static unsigned char m_NextMasterSendSNidx;
// 	static unsigned char m_NextSubmasterAckidx;
// 	static unsigned char m_NextIVSendSNidx;
// 	static unsigned char m_NextAuxAckidx;
    static MsTime   m_MasterUnitAckWaitTime;
    static unsigned char SubMasterACKRefreshFlg;
    static bool MasterReSendRefreshFlg;
    static ST_CANWATCHDOG_CHANNEL_STATE   st_chan_state[MAXCHANNELNO];
//     static MsTime       SubUnitOutWatchDogTime[MAXCHANNELNO];

// 	static MsTime   m_IVReceiveAuxAckWaitTime;
#if (USE_AS_AUXMCU == 1 )
    static unsigned char AuxACKRefershFlg;
#endif
public:

    static void        DoWork(void);                                                                 //任务执行函数
    static void        MyInit(void);                                                                 //任务初始化
    static void        EndTask(void);                                                                //任务结束
    static  int 		AuxAckIVSN(unsigned char m_IVUnitID,unsigned char m_ACK,unsigned char m_cantype);
//   static void 			 MasterReceiveACK(MP_CAN_MSG* MyMsg);
//   static int 			   SubMasterReceiveSN(MP_CAN_MSG* MyMsg);
    static bool        IsSubMasterOpen(void);
    static void 	MasterOrSuMasterReceiveACK(MP_CAN_MSG* MyMsg);
private:
#if (USE_AS_AUXMCU == 0 )
// 	static void  		MasterSendSN(unsigned char m_SubmasterUnitID,unsigned char m_SN);

// 	static int 			SubMasterSendACK(unsigned char m_MasterUnitID,unsigned char m_ACK);

    static int			MasterCheckACK(unsigned char m_SubUnitID);
// 	static int			IVSendSNToAux(unsigned char m_AuxUnitID,unsigned char m_Sn,unsigned m_cantype);

// 	static int			IVCheckAuxACK(unsigned char m_Sn,unsigned char m_SubUnitID);
    static void			ProcessMasterACK(void);
    static void			ProcessSubMasterACK(void);
// 	static void			ProcessIVAck(void);
    static bool     IsMasterAndSubMasterSync(void);
    static int 	MasterOrSubMasterSendACK(unsigned char m_MasterUnitID,unsigned char m_ACK,unsigned char AckType);
    static void         SubUnitStopChannel(unsigned char uc_ChannelNo);
//     static void SuMasterOutCheck(void);
#else


    // static  int 		AuxAckIVSN(unsigned char m_IVUnitID,unsigned char m_ACK,unsigned char m_cantype);
    static void			ProcessAUXAck(void);
public:
    static unsigned char          m_IVUnitID;
    static unsigned char          m_Ack;
#endif
};

#endif
