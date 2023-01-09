#pragma once

#define LOGGEST_FAST_STAGE                      (1)//1 ms����Ϊ������
#define MIN_PULSE_STAGE_MILLI_SECONDS			(0.1f)
#define MIN_PULSE_STAGE_INC_MILLI_SECONDS		(0.01f)
#define MAX_PULSE_STAGE_MILLI_SECONDS			(60.0f*60.0f*1000.0f)
#define MIN_PULSE_LOG_DATA_SECONDS				(1.0f)//��ʱ����
#define MAX_SINGLE_PULSE_TOTAL_MILLI_SECONDS	(10000)   // 500ms --> 10000ms single pulse   8.10.2007 //05.21.2012 06.28.2012 modify from 10s to 500ms  )//��ʱ����

#define SAVE_2_HEAD                             0         //�浽ͷ�㻺��
#define SAVE_2_TAIL                             1         //�浽β�㻺��

typedef struct
{
    float             m_fUS;
    float             m_fMS;
    unsigned long     m_u32TimerSet;
} PULSE_RISE_TIME;
typedef struct
{
    float             m_fElapsedTime;
    bool              m_bResume;
    unsigned char     m_StgIdx;
} PULSE_RESUME_INFO;
typedef  struct
{
    bool              m_bFull;           //�Ƿ�������
    bool              m_bDataLogged;     //�Ƿ����ϴ�����
    MsTime            m_CycleStartTime;  //��cycle��ʼʱ��ϵͳʱ��
    unsigned long     m_CycleIdx;
} PULSE_BUF_INFO;     //���ڹ���������
typedef struct
{
    unsigned char     m_uStageNum;                //̨�׺�
    unsigned long     m_uI_ADC_Code;              //����DAC��ֵ
    unsigned long     m_uV_ADC_Code;              //��ѹDAC��ֵ
    float             m_fCurrent;                 //����������
    float             m_fVoltage;                 //��ѹ������
    unsigned short    m_uElapsedTimeWithinStage;  //�õ��ڸ�stage�ڵ����۲���ʱ�� us
    bool              m_bSampleUpdated;           //�������±�־
    bool              m_bDataConverted;           //�����Ƿ�ת���ɸ�����
    //bool              m_bDataLogged;              //�Ƿ��Ѿ��ϱ�
    bool			  m_bSumCapacity;             //�����ۼƵ���
    bool              m_bSafetyChecked;            //���ڰ�ȫ���
    unsigned long     m_dataPoint;
} PULSE_RAW_DATA;     //���ڱ���ÿ�����ԭʼ����
typedef struct
{
    //unsigned int      m_uHead_Index;              //��stage�ĵ�һ��������������ݻ������λ��
    //unsigned int      m_uTail_Index;              //��stage������/���һ��������������ݻ������λ��

    bool              m_bFastStage;               //���࣬Ҳ���Դ�������Ϣ��ȡ�� �ǿ�stage������stage
    MsTime            m_StageStartTime;           // ��stage��ʼʱ��ȡ��ϵͳʱ��

    unsigned long     m_TimeInUS;                 //������������ʼ���� ΢�� Ϊ��λ��ʱ��
    //������Ҫ��ӵ���Ϣ
} PULSE_STAGE_INFO;  //���ڹ���ÿ������stage��������

typedef struct
{
    bool              m_bFast;         //�Ƿ��ǿ���̨��
    unsigned char     m_Non_V_Limit_Priority;
    bool              m_bVerySlow;     //���stage��ȸ���60�룬��Ϊ��Ҫ���������趨Timer��Ԥ��Ƶ�Ĵ���
    unsigned long     m_PredictAdcCode;  //Ԥ�Ƶ�ADC��ֵ������ADC�������Ʋ�
    unsigned long     m_PredictAdcCode_Max_Head;
    unsigned long     m_PredictAdcCode_Min_Head;
    unsigned long     m_PredictAdcCode_Max_Tail;
    unsigned long     m_PredictAdcCode_Min_Tail;
    unsigned long     m_DacCode;
    unsigned long     m_DacCode_NextStg; //��һ��Stage��DAC��ֵ
    unsigned long     m_TimerSet;
// 	unsigned long     m_TimerSet2;
// 	unsigned long     m_TimerSet3;
} St_PulseStageInfo_Running;   //����ʱ������̨�׽ṹ����
typedef struct
{
    bool              m_bPulseSample;  //���ڱ�ʶ�Ƿ�������ͨ��������:   0-->PULSEû�в�������ͨ��������    1-->PULSE���ڲ�������ͨ�����رգ�
    unsigned char     m_StageCount;    //̨������
    unsigned char     m_StageIdx;      //�������е�̨�����
    //MsTime            m_StartTime;     //pulse�ĵ�һ��stage����ʱ��ϵͳʱ�䣬��100usΪ�ֱ���
    unsigned long     m_CycleCounter;  //pulse�ܹ�����˶��ٴ����ڣ�

    unsigned char     m_SampType;      //0:��ͨ��¼��ÿ��pulse stage��¼�����㣻 1:�����¼��ÿ��pulse stage��¼β��һ���㣻

    St_PulseStageInfo_Running    m_Stages[MAX_PULSE_STAGE_NUM];
} St_PulseInfo_Running;

typedef struct
{
    WORD               m_wChannelNumber;
    BYTE               m_btIfPartialPusle;                  // �Ƿ�������塣	// 0��ʾ��������  // 1��ʾ����������
    DWORD              m_dwPulseCycleID;                    // ����Cycle�š�Ԥ�����������ܽ��м�¼
    BYTE               m_btPointCount;                      // �����ڵ�����ע���������Stage������n��Stage������ʵ������2n���㡣
    BYTE               m_btFirstPointID;                    // �����ڵ�һ��ĵ�š���k��Stage�ĵ�m���㣬����Ϊ2(k-1)+m-1��������˵����һ��Stage�ĵ�һ��������Ϊ0����ʮ��Stage�����һ������Ϊ19��������Ҫע���Ժ��Ƿ�����ÿ��Stage�����㣬�����Ļ���Ҫ������ơ�
    BYTE               m_btIfHaveCapEng;                    // �����Ƿ���������������ݡ�����������ۣ��Ƿ��������ݾͲ���������������������������������ͨ���ݵ��ϴ�
    BYTE               m_btbuf_Idex;                        // ���ڹ���������ǰ���л����룬��0�Ż�1��
    MsTime             m_PulseStartTime;
} St_Pulse_PackInfo;

typedef struct
{
    unsigned long     m_uI_ADC_Code_Head;              //����DAC��ֵ
    unsigned long     m_uV_ADC_Code_Head;              //��ѹDAC��ֵ
    unsigned long     m_uI_ADC_Code_Tail;              //����DAC��ֵ
    unsigned long     m_uV_ADC_Code_Tail;              //��ѹDAC��ֵ
} HistorySampleStg;
typedef struct
{
    HistorySampleStg  m_Stags[MAX_PULSE_STAGE_NUM];
    bool              bValid;
} PulseHistorySample;
enum
{
    PulseSM2_Dac,
    PulseSM2_Sample1,
    PulseSM2_Sample2Pre,    //�ڽ��еڶ��β�����ʱ����ǰ�ر� ��ͨ�������񣬷�ֹ����
    PulseSM2_Sample2,
    PulseSM2_End,
};
enum
{
    PulseSM_Dac,
    PulseSM_Sample1_Conv,
    PulseSM_Sample1_Read,
    PulseSM_Sample2_Conv,
    PulseSM_Sample2_Read,
    PulseSM_End,
};

enum
{
    PulseSM3_Dac,
    PulseSM3_Sample1_Conv1,
    PulseSM3_Sample1_Read1Conv2,
    PulseSM3_Sample1_Read2Conv3,
    PulseSM3_Sample1_Read3,
    PulseSM3_Sample2_Conv1,
    PulseSM3_Sample2_Read1Conv2,
    PulseSM3_Sample2_Read2Conv3,
    PulseSM3_Sample2_Read3,
    PulseSM3_End,
};

class Pulse
{
private:
    //static unsigned char                   m_StateMachine;       //��λ��̨����ת״̬��
    static St_PulseStageInfo_Running       *m_StagePtr;          //̨������ָ�룬ָ���������е�̨�׶���
    static PULSE_RESUME_INFO               m_ResumeInfo;
    static PULSE_RISE_TIME                 m_RiseTime;
    static PulseHistorySample              m_HistorySample[3];
    static PulseHistorySample              *m_pSampleLast1;
    static PulseHistorySample              *m_pSampleLast2;
    static PulseHistorySample              *m_pSampleCurrent;
    static unsigned long                   m_SamleErrorCounter;

public:
    static unsigned char                   m_StateMachine;       //��λ��̨����ת״̬��
    static St_PulseInfo_Running            m_Info;               //����̬��PULSE���壬   ����λ�����ص�ԭʼPULSE����PULSE_DEF�������
    static PULSE_RAW_DATA                  raw_data[2][2][ MAX_PULSE_STAGE_NUM] ; //[������1/2][ͷ/β][̨����] [2][2][];
    //ͷβ�ֿ����Ա��ڼ�¼һ����(ֻ��¼β��)�ͼ�¼�����㣨�ף�β���㣩

    static PULSE_STAGE_INFO                stage_info[2][ MAX_PULSE_STAGE_NUM]; //��Ӧ�������������ģ��й�����stage����������
    static PULSE_BUF_INFO	                 buf_info[2];
    static unsigned char                   active_index;            //pulse����������������������ݵ� ���
    //static unsigned char                   stage_count;
    static bool			                       Pulse_is_Running;  //����Ƿ��ѿ�ʼ����������
    //static bool                            DoingAdc;
    static float                           fMaxVoltage;
    static float                           fMinVoltage;
    static float                           fMaxVoltage_Curr;      //2014/12/26,per ATL's request
    static float                           fMinVoltage_Curr;      //2014/12/26,per ATL's request
    static float                           fMaxCurrent;
    static float                           fMinCurrent;
    static float                           fMaxPower;
    static float                           fMinPower;

public:
    static void        SetRiseTime(float fRiseTimeInMs);
    static void        MVinit(void);
    static void        PulseInit(unsigned char ChannelIdx);
    static void        Start(unsigned char ChannelIdx);
    static void        End(unsigned char ChannelIdx);
    static void        PulseISR3(void);
    static inline void SaveRawData(unsigned char StgIdx,unsigned long Current,unsigned long Voltage,unsigned char head_or_tail);
    static inline void ReadAdcAfterBusy (unsigned long *Data1,unsigned long *Data2);
    static inline void Dac_SendData(unsigned long value);
};
