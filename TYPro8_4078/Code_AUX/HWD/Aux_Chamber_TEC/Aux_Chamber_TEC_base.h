
#ifndef _CHAMBER_TEC_BASE_H_
#define _CHAMBER_TEC_BASE_H_


enum
{
    GetTemp,
    CalPWM,
    OutputPWM
};

typedef struct
{
    float  SetTemp;
    float  SampleTemp;
    float  NewErr;
    float  Kp;
    float  Ki;
    float  Kd;
    float  Up;
    float  Up1;
    float  Ui;
    float  Ud;
    float  OutPreSat;
    float  OutMax;
    float  OutMin;
    float  Output;
    float  SatErr;
    float  LastTemp;
    float  LastOutput;
    float  LastErr;
} CHAMBER_TEC_PID_INFO;


class Aux_Chamber_TEC_Base
{
public:
    static void KEY_DoWork(void);
    static void KEY_Init(void);
    static void Chamber_TEC_DoWork(void);
    static void Set_TempPoint(void);
//		static void DoWork(void);
    static void Init(void);
    static UInt16 KEY_Priority;
    static UInt16 KEY_TaskID;
    static UInt16 Chamber_TEC_Priority;
    static UInt16 Chamber_TEC_TaskID;
    static bool Aux_Chamber_TEC_Board;
//		static CHAMBER_TEC_PID_INFO * Pid_Info;
    static CHAMBER_TEC_PID_INFO Chamber_Pid_Info;
    static float Temperature;
    static float Temperature1;
    static unsigned char TempRec[3];

private:
    static unsigned char T_Alert_Flag1;
    static unsigned char T_Alert_Flag2;
    static unsigned char KEY_FLAG;
    static float PW_INC_LMT;
    static float PW_DEC_LMT;
    static unsigned char TempUp;
    static unsigned short TEC_Phase;
    static unsigned char TC_count;
//		static float Temperature1;
    static unsigned char COOLRANGE;
    static unsigned char HEATRANGE;
//		static unsigned char TempRec[3];

//		static void Set_TempPoint(CHAMBER_TEC_PID_INFO* Pid_Info);
//		static void Init_Chamber_TEC(CHAMBER_TEC_PID_INFO* Pid_Info);
//		static void Chamber_TEC_DoWork(CHAMBER_TEC_PID_INFO *Pid_Info);
//		static void Init_PIDPara(CHAMBER_TEC_PID_INFO* Pid_Info);
//		static float PID_Regulator(CHAMBER_TEC_PID_INFO *Pid_Info);
//		static void Set_TempPoint(void);
    static void Init_Chamber_TEC(void);
//		static void Chamber_TEC_DoWork(void);
    static void Init_PIDPara(void);
    static float PID_Regulator(void);
    static void Init_PWM1(void);
    static void PWM_Calculation(float pwm_output);
    static void Temp_Alert(void);
    static void CLR_Temp_Alert(void);

    static void SET_ERR1(void);
    static void SET_ERR2(void);
    static void CLR_ERR1(void);
    static void CLR_ERR2(void);
    static void Check_T_Alert(void);
    static void Update_Temp(void);

};

//extern void Err_Disp(void);

#endif
