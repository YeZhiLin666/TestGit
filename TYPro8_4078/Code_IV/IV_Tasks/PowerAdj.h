///////////////////////////////////////////////////////////////////////////////////
//File Name: PowerAdj.h
//Version:		V1.0
//Date:				2018/02/09
//Description:
//						PowerAdj���ܶ���
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"


#define     AD_OVERTIME                 20*10000
#define     Unsafe_Overtime             16*10000



enum
{
    POWERADJ_IDLE=1,      			//����
	POWERADJ_WAIT,
// 	POWERADJ_START,		//����������Ϣ��M0
// 	POWERADJ_RUNNING,
    POWERADJ_REQUIRE,
    POWERADJ_WAIT_REQUIREACK,
    POWERADJ_RUNNING,
// 	POWERADJ_WAIT_RefreshACK1,
// 	POWERADJ_WAIT_RefreshACK2,
// 	POWERADJ_WAIT_RefreshACK3,
// 	POWERADJ_WAIT_RefreshACK4,
// 	POWERADJ_SENDSTOPTOM0_1,		//����ֹͣ��Ϣ��M0
// 	POWERADJ_WAIT_STOPACK1,
// 	POWERADJ_WAIT_STOPACK2,
// 	POWERADJ_WAIT_STOPACK3,		 	//�ȴ�M0Ӧ��
// 	POWERADJ_WAIT_STOPACK4,
// 	POWERADJ_WAIT_STOP,
// 	POWERADJ_ERROR,					//�쳣����
    POWERADJ_Feedback_ERROR,
    POWERADJ_COMMUNATION_ERROR,
    POWERADJ_FIXUNSAFE,
    POWERADJ_GOTOThirdParty,
    POWERADJ_WaitForDefault,
    POWERADJ_OnThirdParty,
};
enum
{
    CAN_WATCHDOG_IDLE,
    CAN_WATCHDOG_WATIFORSENDACK,
    CAN_WATCHDOG_SENDTOM0,
    CAN_WATCHDOG_WAITM0ACK,
    CAN_WATCHDOG_ACKFAILD,
};
typedef struct
{
    float                 m_Vpp;            //�����ѹ
    float                 m_Vmm;           //�����ѹ
} St_PowerData;

typedef struct
{
    bool IsResistorFlag;	//�Ƿ�ʹ�õ��踺��
    float m_ResistorValue;	//����ֵ
} St_LoadTypePowerAdj;

typedef struct
{

    float                 m_Vm;      //�����ѹ�ĵ�λ��ѹ
    float                 m_Vn;      //�����ѹ�ĵ�λ��ѹ
} St_PowerLevelData;
typedef struct
{
    bool     m_unsafeFlag;      // 0�������ǰ�ȫ��    1�������ǲ���ȫ��
    MsTime   m_UpdataTime;
    MsTime   m_UnsafeTime;
// 	unsigned char m_unsafeFlag;
} St_FeedBackCheckData;

class PowerAdj
{
public:
    static UInt16 TaskID;                                                                    //����ID
    static UInt16 Priority;                                                                  //���ȼ�
// 	  static bool  PowerAdjWorkFlag;      //���ʵ�Դ���ڵ���״̬��   0�����ʵ�Դû���ڵ��ڣ�   1�� ���ʵ�Դ��Ҫ���ڡ�
    static bool Calib_PowerParaFlag[MAXCHANNELNO];
// 	  static bool SchRunningFlag[MAXCHANNELNO]; //Schedule ���б�־
    static unsigned char m_PowerAdjStatus;
// 	  static bool b_M0ADCValueUnsafeFlag;   //false: ��ȫ��ѹ��Χ     true: ����ȫ�ĵ�ѹ��Χ
    static St_PowerData m_PowerValue;
	  static St_PowerData m_M0Feedback_Value[4];
// 	  static UInt16  Time20msCnt;   //���ڼ�¼20ms ˢ��һ�ε�Դ��ѹ����ֵ��
// 	  static uint32  Time50usCnt;    //���ڶ�ʱ����
// 	  static bool    TimeStartFlag; //���ڱ�ʶ����50us��ʱ
// 	  static bool    Time20msStartFlag;    //��������20ms�ط����ƵĶ�ʱ��־
// 	  static unsigned char Receive_ACK_Status;    //00: û���յ� ACK��  01����ȷ�յ�ˢ�·���ACK,  02:������ѹ������Ҫ����λ��Ҫͣ������
// 	  static unsigned char PackLostCnt[4];     //Ŀǰ���֧��4��M0
// 	  static bool           Receive_M0_StartedACKFlag;
// 	  static bool  Receive_M0_IDACKFlag[4];
    static St_LoadTypePowerAdj SchLoadPara[MAXCHANNELNO]; //Schedule ���踺�ز���
	  static St_PowerLevelData   m_PowerLevelValue;
	  static  unsigned char uc_M0ADData_UpdataFlag;      //�յ�������1 �� ʹ�ú�����
	  static  unsigned char uc_DoCheckFlag;
	  static St_FeedBackCheckData   m_FeedBackData;
	  static  unsigned char uc_DefaultVoltageFlag;    //�ñ�־��λʱ����ʾ��Ҫ��Դ���õ�Ĭ�����
	  static unsigned long  WaitCnt;
// 	  static unsigned char  ReceiveSN;
	private:
// 		static unsigned long WaitACKCnt; 
// 	  static unsigned char uc_SendComCnt;
// 	  static unsigned char m_Watchdog_SN;
// 	  static unsigned char m_CanWatchdogStatus;
// 	  static MsTime   m_AckWaitTime;
// 	  static unsigned char ReSendCount;
	public:
		static void        DoWork(void);                                                                 //����ִ�к���
		static void        MyInit(void);                                                                 //�����ʼ��
		static void        DoVoltageProcess(void);
		static void        CheckFeedBack(void);
	  static void CalPowerSetValue(void);
	private:
// 		static void 			 						SendMsgToM0(MP_CAN_MSG  m_Msg);
// 		static unsigned short 			 	CrcCheck(unsigned char *address,unsigned char len);
	  
	  static  void  CalPowerUpdataValue(void);
	  static void ProcessWatchDog(void);
	  static void PowerSetDefaultValue(void);
// 	  static int	CheckACKSN(void);
};

