#pragma once

#include "../Entry/G_DF.h"
#include "../../COMMON_OS/LPC4078CORE/RTC.h"

#define LOGICPROCESS_HANDLE_MAX         100    //�����32��
#define LOGICPROCESS_HANDLE_ID_MAX      65535

//ʱ����״̬
#define LOGICPROCESS_STATUS_HANDING    0
#define LOGICPROCESS_STATUS_FINISH         1
#define LOGICPROCESS_STATUS_ERROR        -1

#define DAC_OUT_PUT_ZERO 0x800000    //ƫ����  0V
#define DAC_OUT_PUT_MAX  0xFFFFFF     //���10V
#define DAC_OUT_PUT_MIN  0x000000    //��С-10V


//for DisposableFlag    when use openchn DisposableFlag = DISPOSABLE_OPENCHN
//comment out chen 20130708
enum
{
    DISPOSABLE_to_NONE = 0x01,    //û��ʱ��
    DISPOSABLE_to_OPENCHN,
    DISPOSABLE_to_CLOSECHN,
    DISPOSABLE_to_CCHN,
    DISPOSABLE_to_RESE,    //Ԥ��
};

//for St_RealyOperation RelaySet
enum  //��ΪBT2208������û�и��죬���ԣ�������ʱ�����ţ��Ժ��ɾ�����������࣬20140522
{
    ACTIVE_CUR_RELAY_NONE = 0x0000,                                      //������relay
    ACTIVE_CUR_RELAY_SUPER_HIGH = 0x0001,                            //������������
    ACTIVE_CUR_RELAY_HIGH = 0x0002,                                       //����������
    ACTIVE_CUR_RELAY_MIDDLE = 0x0004,                                   //����������
    ACTIVE_CUR_RELAY_LOW = 0x0008,                                       //����������
    ACTIVE_CUR_GAIN = 0x0010,                                                //������������
    ACTIVE_CUR_CUT_PWM = 0x0020,                                         //��դѹ
    ACTIVE_CUR_RST_CUTOFF = 0x0040,                                     //�ָ�դѹ
    ACTIVE_CHAGE = 0x0080,                                                     //��ŵ缫�Կ���
    ACITVE_RESERV1 = 0x0100,                                                  //Ԥ��1
    ACTIVE_U_P_CHARGE = 0x0200,                                            //unipolar���
    ACTIVE_U_P_DISCH = 0x0400,                                               //unipolar�ŵ�
    ACTIVE_VOL_RANGE = 0x0800,                                             //��ѹ�����л�--------------�Զ�У׼�̵���
    ACTIVE_V_I_R = 0x1000,                                                       //�����˵�ѹ����
    ACTIVE_RESERV2 = 0x2000,                                                  //Ԥ��2
    ACTIVE_P_R = 0x4000,                                                          //����PR�̵���
    ACTIVE_OUT_RELAY = 0x8000,                                              //����̵���
};

//for rangeset
enum
{
    ACTIVE_CLOSE_CURRENT_RELAY = 0x0001,    //�ر���������
    ACTIVE_OUT_NEXT_RELAY = 0x0002,          //��������
    //RESERV
};

//for dac
enum
{
    ACTIVE_DAC_NONE =0x0000,                                              //û�в���
    ACTIVE_DAC_CUR = 0x0001,                                               //����msg�������
    ACTIVE_DAC_VLT = 0x0002,                                                //����msg�����ѹ
    ACTIVE_DAC_VH  = 0x0004,                                                //����msg��ǯѹ
    ACTIVE_DAC_VL  = 0x0008,                                                 //����msg��ǯѹ
    ACTIVE_DAC_CUR_ZERO = 0x0010,                                     //�������0
    ACTIVE_DAC_VLT_ZERO = 0x0020,                                      //��ѹ���0
    ACTIVE_DAC_VH_DEFAULT = 0x0040,                                  //��ǯѹ�������
    ACTIVE_DAC_VL_DEFAULT = 0x0080,                                   //��ǯѹ�������
// 	ACTIVE_DAC_CUR_NEAR_ZERO =  0x0100,                                  //��������ӽ�0����ƫ��һЩ
//     ACTIVE_DAC_OFFSET_5PERCENT_ZERO = 0x200,
    //����һ��������DAC��ǰ��ADCֵ  20131101  chen add
    ACTIVE_DAC_CUR_ADC = 0x2000,
};

//��̬˲̬�жϣ���˲̬��ʱ�򲻽���safecheck
//comment out chen 20130724
enum
{
    LOGIC_PROCESS_STEADY = 0xB1,
    LOGIC_PROCESS_TRAN,
    LOGIC_PROCESS_CHN_CLOSE,
    LOGIC_PROCESS_CHN_OPEN,
};

enum
{
    LOGIC_NOTREADY=0x00,
    LOGIC_IDLE,
    LOGIC_WAIT,
    LOGIC_TRIGGER,
    LOGIC_END,
};
enum
{
    DO_NA=0x00,
    DO_STOP,
    DO_RUN,
};
typedef struct
{
    unsigned short ID;    //0-65535��
    unsigned char status;         //״̬  8 bit  ������
} LogicProcessHandle;

typedef CIRCULAR_QUEUE(LogicProcessHandle,LOGICPROCESS_HANDLE_MAX)    qDisposTaskBuf;

enum
{
    DISPOSABLE_ASSIGNED = 0x01,    //�Ѿ�������������
    DISPOSABLE_RUNNING,           //������
    DISPOSABLE_FINISHED,                //����
    DISPOSABLE_ERROR,               //����
    DISPOSABLE_DEAD,                 //��ʷ��¼�Ѿ�����
};

typedef struct
{
    unsigned short RelaySet; 			 //������

    unsigned short ActiveFlag;     //16λ����������    ÿλ����һ������    ���֧��16�ֲ���
    unsigned short InactiveFlag;
} St_RealyOperation;

typedef struct
{
    unsigned short DacSet;  //DAC������
    unsigned short DacFlag; //16λ����������    ÿλ����һ��DAC ���֧��16�ֲ���
} St_DacOperation;

enum
{
    eTransitionType_NULL = 0,
    eTransitionType_RstToCur,
    eTransitionType_RstToVlt,
    eTransitionType_RstToOff,
    eTransitionType_OffToRst,
    eTransitionType_OffToCur,
    eTransitionType_OffToVlt,
    eTransitionType_CurToCur,
    eTransitionType_CurToVlt,
    eTransitionType_CurToOff,
    eTransitionType_CurToRst,
    eTransitionType_VltToVlt,
    eTransitionType_VltToCur,
    eTransitionType_VltToOff,
    eTransitionType_VltToRst,
};

typedef struct
{
    unsigned long uw32_DelayTime;    //ʱ��� 100usΪ��λ
    St_RealyOperation RelayMove;     //�̵�������
    St_DacOperation   DacMove;       //DAC����
    unsigned char RangeChangedFlag;    //�Ƿ����л�����    0����Ҫ    1��Ҫ

    unsigned char bPWM;
    unsigned char PWM_TransitionType;
} St_DisposableTask;
///////////////////for һ����ʱ������///////////////////

typedef struct
{
    St_DisposableTask * st_Disposabletask_ptr;    //ָ����ṹָ��
    unsigned short   stepLen;    //����
    //unsigned char prio;    //���ȼ�����    comment out chen 20130702  ֻ��2�����ȼ�����ߺ���ͣ�ֻ�й�ͨ������ߣ�����ʱ��Ϊ���
} St_Disposable;
///////////////////for һ����ʱ���ò���///////////////////

//һ��������MSG
typedef struct
{
    unsigned char  CH_Number;       //ͨ����
    unsigned char  msgID;//�ж��Ǵ��ķ�����msg
    unsigned short RangeSet;//����ͬRange
    unsigned char  last_Range;    //������
    unsigned char  next_Range;
    unsigned char  last_VRange;
    unsigned char  next_VRange;

    St_Disposable   Disposable_task;    //msg�ڵĸ���
    unsigned char   Disposable_Flag;    //����ʱ��ı�־    comment out chen 20130708
	unsigned char   TargetCtrlType;    // Ŀ���������    20200810 yy
	
    //DAC����Ԥ��
    unsigned long Dac_Value;       //DACֵ
    unsigned long Volt_DacValue;
    unsigned long Clamp_Direction; //ǯλ����
    //��λ��ֻ����ǯ����ǯ����λ������Ҫ���ݲ�ͬ��Ӳ��д��ͬ�����á�������ϵ�ֻ��һ�����С�
    //�����˫ǯλ����ô�涨Clamp_ValueΪCLAMPVH��Clamp_Value_2ΪCLAMPVL
    unsigned long Clamp_Value;     //ǯλֵ
    unsigned long Clamp_Value_2;    //

    unsigned char AutoCaliFlag;    //�Զ�У׼Ԥ��    1�Զ�У׼    0��ͨ����
    //comment out chen

    //ADC����Ԥ��
    unsigned char ADC;

    //ִ������    �ⲿ�ֱ��뿼�Ƿŵ�chn info�����ÿ���ṹ������˵�����û�б�Ҫ�������ŵ�ȫ��msg��Ϳ��ԣ����Ǵ�Ч���ϲ���ŵ�
    //chn info�в�θ�
    //ֱ�Ӹ�0
    //comment out chen
    volatile unsigned char state;     //״̬
    unsigned char counter;  //����

    MsTime MsTimeDisposableTask;
    MsTime SysTimeDisposableTask;
} St_DisposableMSG;
///////////////////for һ����ʱ��msg///////////////////

typedef struct
{
    St_DisposableMSG logicProcessMSg;
    MsTime           StartTime;
    unsigned long    DelayTime;
    unsigned char    bActionFinished;
    unsigned short   ID;
    unsigned short   NextID;
} St_Disposable_G_Que;   //�����������


//for �Ŷӻ���
//comment out chen 20130708
typedef struct
{
    unsigned char cur_Disposable;    //��ǰʱ��
    unsigned char prio_isUsed;         //�Ŷ�ʹ��
} St_Disposable_Prio;   //�Ŷ����ȼ�

//extern const unsigned char g_chn_order[MAXCHANNELNO];
// extern unsigned char g_chn_order[MAXCHANNELNO];

class OS_LogicProcess
{
public:
    static St_Disposable          SetCtrlRstToCur;
    static St_Disposable          SetCtrlRstToVlt;
    static St_Disposable          SetCtrlRstToOff;
    static St_Disposable          SetCtrlOffToRst;
    static St_Disposable          SetCtrlOffToCur;
    static St_Disposable          SetCtrlOffToVlt;
    static St_Disposable          SetCtrlCurToCur;
    static St_Disposable          SetCtrlCurToVlt;
    static St_Disposable          SetCtrlCurToOff;
    static St_Disposable          SetCtrlCurToRst;
    static St_Disposable          SetCtrlVltToVlt;
    static St_Disposable          SetCtrlVltToCur;
    static St_Disposable          SetCtrlVltToOff;
    static St_Disposable          SetCtrlVltToRst;
    static St_Disposable          SetCtrlVltToExt;
    static St_Disposable          SetCtrlExtToVlt;
    static St_Disposable          SetAutoCaliOpen;    //�Զ�У׼��
    static St_Disposable          SetAutoCaliClose;   //�Զ�У׼��     //��ͨ����ΪIVͨ���������Զ�У׼������
    static St_Disposable 					SetCtrlAcimOn;		    //ACIM��
    static St_Disposable 					SetCtrlAcimOff;   		//ACIM��

    static St_Disposable          SetCaliBoardRstToVlt;
    static St_Disposable          SetCaliBoardVltToOff;
    static St_Disposable          SetCaliBoardVltToVlt;                //��ͨ����ΪCALIͨ�����������Զ�У׼������
	static St_Disposable          SetAutoRangeSwitch;

    static St_Disposable		      SetVoltRelay;
    static UInt16                 Priority;//commnet out chen 1121
    static UInt16                 TaskID;
//     static unsigned long int      Set_DAC_Output_MIN; // ����һ��΢ֵISET����������
    static volatile unsigned char Logic_For_Safe[MAXCHANNELNO];    //ת��״̬    ˲̬  ��̬
    static St_Disposable_G_Que    m_RunningTask[MAXCHANNELNO]; // Ron Modified 081415
    static unsigned char 			LogicStatus[MAXCHANNELNO];
    static MsTime 					Logic_last_time[MAXCHANNELNO];
    static unsigned char 			LogicCtrlType;
    static bool 					be_NeedWaitflg;
    static bool						m_bSlaveActive[MAXCHANNELNO];
		static bool 					be_StartThirdParty;
private:
    //static St_Disposable_G_Que    m_RunningTask[MAXCHANNELNO]; //Ron modified 081415
    static unsigned short         handle_index;    //����  1-65535
    static qDisposTaskBuf         m_qHandle;
    static unsigned char          m_bActive[MAXCHANNELNO];
    static unsigned char          m_bQueueBusy[MAXCHANNELNO];

public:
    static void                   Init(void);
    static void                   DoWork(void);
    static unsigned short         requestLogicPorcess(unsigned char ChannelIdx,St_DisposableMSG * st_oneTime_msg);
    static unsigned char          getHandleStatus(unsigned short HandleID);
// 	static unsigned long          CalibrateDac(float fdac);
    static void										InitBoardEngine(St_DisposableTask* open,
            unsigned char open_len,
            St_DisposableTask* close,
            unsigned char close_len,
            St_DisposableTask* cc2cc,
            unsigned char cc2cc_len,
            St_DisposableTask* rst2off,
            unsigned char rst2off_len);

private:
    static int                    ProcessActive(unsigned char ChannelIdx);
    static signed int             processDisposable(unsigned char ChannelIdx);   //OpenChn or Close Chn or others
    static void                   DumpUnnessaryMove(unsigned char ChannelIdx);//�ж�ִ�еĶ�������һ����Ծ����
    static void                   DoMove(unsigned char ChannelIdx);//��Relay,DAC����
    static void                   DoDAC(unsigned char ChannelIdx);
    static void                   setStatus(unsigned short HandleID,unsigned char state);
    static unsigned char 		  FindNextSubChannel(unsigned char GorupID);
};


