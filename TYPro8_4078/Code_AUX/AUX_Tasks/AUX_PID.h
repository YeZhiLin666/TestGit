#pragma once
#define MAXPIDCHANNELNUM    8


typedef struct
{
    float 					Kp;
    float           Ki;
    float           Kd;
    unsigned char   m_PidTypeFlag;
    float  					f_Last_Current;
    float  					f_Last_Voltage;
    float  					m_LastOutput_1B;
    unsigned char  	m_DataBank; //m_CurrentIRange;
    float  					m_SetValue_1B;
    float  					m_SampleValue_1B;
    float 					m_fLastErr_1B;
    float           m_SetValue_Open;
    bool 						m_bLoadIsResistor;
    unsigned char   m_AI_Chn;
    unsigned char   m_AI_DataBank;
} ST_AUX_PID;

class AuxPID_task
{
public:

    static unsigned short            	Priority;
    static unsigned short            	TaskID;
    static ST_AUX_PID                 mAuxPID_par[MAXPIDCHANNELNUM];
    static unsigned char							beNeedPID[MAXPIDCHANNELNUM];
    static unsigned char              Counter;
public:
    static void              					DoWork(void);
    static void              					MyInit(void);
    static void              					EndTask(void);

private:
    static float AUX_PID_Output_Calculation(unsigned char uc_Ch_No, float fKp,  float fKi, float fKd, ST_AUX_PID pPid);
};
