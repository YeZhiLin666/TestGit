#pragma once
#define SDL_TASK_CHANNEL_COUNT              4 // 2       //ÿ��SDL���񣬴���4��ͨ��

#define DISPLAY_LOGDATA_INTERVAL            20000      //��ʾ���ݼ����¼ʱ��  10��
#define IDL_LOGDATA_INTERVAL                40000      //��ʱ���ݼ�¼ʱ��   12��

#define SDL_LOGDATA_T_INTERVAL              (IV_DEFAULT_SDL_TIME * 10000) // 50000;  // SDLĬ�ϣ�T 5�룬2VD 100�����¼һ���PC
#define SDL_LOGDATA_H_INTERVAL              (IV_DEFAULT_SDL_TIME * 10000) // 50000;  // SDLĬ�ϣ�H 5�룬2VD 100�����¼һ���PC, ʪ��
#define SDL_LOGDATA_2VD_INTERVAL            (IV_DEFAULT_SDL_TIME * 10000) // 1000;   // SDLĬ�ϣ�T 5�룬2VD 100�����¼һ���PC
#define SDL_LOGDATA_EC_INTERVAL             (IV_DEFAULT_SDL_TIME * 10000)
#define SDL_LOGDATA_PRESSURE_INTERVAL       (IV_DEFAULT_SDL_TIME * 10000)
#define SDL_LOGDATA_T_DELTA                 AUX_DEFAULT_SDL_FACTOR_T       //�¶�SDL����Ĭ��ֵΪ1���϶ȱ仯��     // SDLĬ�ϣ��仯��ֵT 1�ȣ�2VD 0.5V ��¼һ���PC
#define SDL_LOGDATA_H_DELTA                 AUX_DEFAULT_SDL_FACTOR_H       //ʪ��SDL����Ĭ��ֵΪ1%Ũ�ȱ仯��     // SDLĬ�ϣ��仯��ֵT 1�ȣ�2VD 0.5V ��¼һ���PC
#define SDL_LOGDATA_2VD_DELTA               AUX_DEFAULT_SDL_FACTOR_2ndV   //��ѹSDL����Ĭ��ֵΪǧ��֮һ;  // SDLĬ�ϣ��仯��ֵT 1�ȣ�2VD 0.5V ��¼һ���PC
#define SDL_LOGDATA_EC_DELTA                AUX_DEFAULT_SDL_FACTOR_EC_Current   //�ⲿ������ SDL����Ĭ��ֵΪǧ��֮һ
#define SDL_LOGDATA_PRESSURE_DELTA          AUX_DEFAULT_SDL_FACTOR_PRESSURE    //
//#define MAX_AUX_TYPE                        16
//#define MAX_AUX_TYPE_CHANNEL                16
#define MAX_IV_MCU_COUNT                    64

// ������¼��ʽ
#define STEP_LOG_IV                         0      //Step Logģʽ������¼����
#define SDL_LOG_DELTA                       1      //SDL Logģʽ������¼����


// �������ݷ�ʽ��λ�����ж�
#define LOG_PC_DATA                        (0X01 << 0)        //ΪPC��¼����  
#define REPORT_IV_DATA                     (0X01 << 1)        //ΪIV��������
#define DATA_PC_AND_IV                     (LOG_PC_DATA | REPORT_IV_DATA)     //ΪPC��IV��������


enum TE_AUX_LOGSWITCH
{
    LOGSWITCH_OFF = 0,
    LOGSWITCH_ON,
};

enum SDL_LOG_DELTATYPE
{
    LOG_DELTA_NONE,
    LOG_DELTA_TIME,
    LOG_DELTA_META,
    LOG_DELTA_TIME_DEFAULT,
    LOG_DELTA_META_DEFAULT,
    LOG_DELTA_ALL,
};

enum
{
    LOG_TIME_1MS,
    LOG_TIME_10MS,
    LOG_TIME_20MS,
    LOG_TIME_50MS,
    LOG_TIME_100MS,
    LOG_TIME_1000MS,
    LOG_TIME_2000MS,
    LOG_TIME_5000MS,
};

typedef struct
{
    unsigned char     uc_PCFlag;       //ΪPC��¼����ʱ������������ֵ��־
    unsigned char     uc_IvFlag;       //ΪIv��¼����ʱ������־
    float             fLogDeltaRate;  //ΪPC��¼��������ֵ����
    float             LogInterval;     //ΪPC��¼����ʱ����
    float             ReportInterval;  //ΪIV��������ʱ����
} St_SDL_DeltaValue;

typedef struct
{
    float             fValue;
    MsTime            LogTime;        //��һ��LOG��PC��ʱ���

    MsTime            ReportTime;     //��һ��REPORT��IV��ʱ���

    MsTime            DisplayLogTime;       //��һ����ʾ���ݼ�¼��PC��ʱ���
    MsTime            IdlLogTime;          //��һ����ʱ���ݼ�¼��PC��ʱ���
} St_SDL_LastValue;

typedef struct
{
    unsigned char     MCUId;     //ΪIV��������MCU��
    unsigned char     bReportIvData;      //ΪIV��������
    unsigned long int ReportDataTypebit;  //ΪIV������������ ԭʼ���ݣ�һ��ʱ�䵼��������ʱ�䵼��
    UWord64           ReportInterval;     //ΪIV��������ʱ����
    MsTime            ReportTime_DXDT;    //ΪIV����һ��ʱ�䵼��ʱ����
} St_NeedValue;

typedef struct
{
    unsigned char     bLogPcData;        //ΪPC��¼����
    float             fLogDeltaValue;    //ΪPC��¼�����������������ȡ�����ֵ����ǧ��֮��
    St_NeedValue      NeedValue[MAX_CH_IN_AUX_UNIT];         //ΪIV��������
} St_SDL_Info;

class AuxSDL
{
public:
    static unsigned short               Priority;
    static unsigned short               TaskID;

private:
    static unsigned char                m_ScanStartIVUnitBufIdx;    //SDL�����¼�״ο�ʼɨ��IVUnit�š�
    static unsigned char                m_ScanStartIVIdx[MAX_IVMCU_PER_SYS];    //SDL�����¼�״ο�ʼɨ��IV�š�
    static unsigned char                m_ScanStartTypeIdx[MAX_IVMCU_PER_SYS];    //SDL�����¼�״ο�ʼɨ�踨�����ͺš�
    static unsigned char                m_CurrentIVUnitBufIdx;
    static unsigned char                m_CurrentIVIdx[MAX_IVMCU_PER_SYS];    //SDL����ÿ�δ������е�ͨ��̫��ʱ������һ��ͨ��������̫���ˣ�
    static unsigned char                m_CurrentTypeIdx[MAX_IVMCU_PER_SYS];    //SDL����ÿ�δ������е�ͨ��̫��ʱ������һ��ͨ��������̫���ˣ�
    static unsigned char                m_CurrentAuxChIdx[MAX_IVMCU_PER_SYS];   //����ʹ��������current������Ŀ���ǣ�ʹ��ÿ�δ���N��ͨ����N������Ϊ4,8,16���Ժ���Ծ���
    //static St_SDL_DeltaValue            m_DeltaValue[MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE][MAX_AUXCH_PER_BOARD]];
    static St_SDL_LastValue             m_LastValue[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_CH_IN_AUX_UNIT];
    static St_SDL_Info                  m_Info[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU]; //MAX_BOARD_PER_AUXMCU��ΪMAX_AUX_TYPE 20160714 zyx

public:
    static void Init(void);
    static void DoWork(void);
    static void TriggerLog_Net(unsigned char uc_IVUnitBufIdx,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char uc_LogType);
    static void SDLSetParameterRoutine(unsigned char uc_IVUnitID,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,St_SDL_DeltaValue * fSdlPara);
    static void SDLEndedRoutine(unsigned char uc_IVUnitBufIdx,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
    static void SwitchLog(unsigned char uc_IVUnitID,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned char bLogOn);
    //static void RequireData(unsigned char uc_IVUnitBufIdx, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char IvMCUIdx,float Factor);
    static void RequireData(ST_AUX_DATA_REQUEST st_data_request);

private:
    static void SetFactor_Time (unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn, unsigned char Send_Logtype,UWord64 Factor);
    static void SetFactor_Value(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,float Factor);
    static void Check_LogValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
    static void Check_ReportValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);

    inline static unsigned long long GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100);
    inline static float GetFullValue(float HighValue,float LowValue);
    static unsigned char MaxValueCheck(unsigned char uc_IVUnit, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
// 		static void  SendDisplayAuxData();
};

