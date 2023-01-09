
#ifndef _TEC_H_
#define _TEC_H_


#define   LOG_TEMP_INTERVAL_TIME    (300000)  // 记录温度时间间隔器,30秒

struct  TEC_Pid_Info
{
    float  SetTemp;
    float  SampleTemp0;
    float  SampleTemp1; //Ron 4-24-2015
    float  SampleTemp2; //Ron 4-24-2015
    float  Last_Temp;
    float  Last_Output;
    float Last_Error;
    float New_Error;


};

enum {
    Read_Temp,
    Cal_PWM,
    Output_PWM
};


class TEC_Ctrl
{
public:
    static UInt16     TaskID;
    static UInt16     Priority;
    static UInt16     TEC_status;
    static TEC_Pid_Info    Pid_Info;
    static float  temperature;
    static MsTime     m_Interval_LogTemp;    // 记录温度时间间隔器


public:
    static void Init(void);
    static void DoWork(void);
    static float Pid_Output_Calculation(float fKp,  float fKi, float fKd, TEC_Pid_Info * pPidInfo);
    static void Ctrl_Pid_Coef_Init(void);
    static void Init_PWM(void);
    static void BuildInAux_TriggerLog_Net(unsigned char uc_AuxType, unsigned short uw16_AuxChnbit);


private:
// 	static float Temp0;  //Ron 4-24-2015
// 	static float Temp1;  //Ron 4-24-2015
// 	static float Temp2;  //Ron 4-24-2015
// 	static float Error;


};

#endif

