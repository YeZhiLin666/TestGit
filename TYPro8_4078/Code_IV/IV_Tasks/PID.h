////////////////////////////////////////////////////////////////////////////////////////////
//File Name:      PID.H
//Version:	      V1.0
//Date:	     04.23.2013
//Description:   PID Calculation
//
//Others:
//History£º
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PID_H_
#define _PID_H_

#define PID_I    1
#define PID_V   2
#define PID_P   3
#define PID_L   4
#define PID_TypeTotal    4
//#define SAMPLEDELAYCOUNTER  3

class Ctrl_Pid_Coef
{
public:
    uint8  m_PidTypeFlag;

// 	static float  f_Last_Current;
// 	static float  f_Last_Voltage;

    float  m_LastOutput_1B;

// 	uint8 m_CurrentIRange;

    float  m_SetValue_1B;
    float  m_SampleValue_1B;

    float m_fLastErr_1B;
    float m_fLastlastErr_1B; // by CZ TURBO BOOST 07.10.2017
//	bool m_bLoadIsResistor;
    static unsigned char m_SampleDelayCounter;

    void Init(void);
};

//#define ERRLOGDEPTH 300              dirui del 2017/12/18

class Pid_Ctrl
{
public:
    // for debugging  // ÓÃ DEBUG_TRACE_RECORD ´úÌæ
    static float  fUpLimit[MAXCHANNELNO];	// ????????PID??, 09.09.2015
    static float  fDnLimit[MAXCHANNELNO];
//    static unsigned long m_Counter[MAXCHANNELNO];

public:
    static float IBased_PID_Output_Calculation(unsigned char uc_Ch_No, float fKp,  float fKi, float fKd, Ctrl_Pid_Coef * pPidCoef);
};

#endif
