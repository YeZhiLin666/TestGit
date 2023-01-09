#pragma once

#define    ACIM_TEST_REPETITION   5
#define    MAX_ACIM_TEST_NUM	  MAX_ACIM_DATA_POINT_NUM
enum
{
    ACR_IDLE,
    ACR_WAIT_V_Relay,
    ACR_SetResValue,
    ACR_Output_SineWave,
    ACR_DCMeasure_Start,
    ACR_CheckLoad_Open,
    ACR_WAIT_10MS,
    ACR_Read_Irsm_Open,
    ACR_WAIT,
    ACR_CalDC_Value,
    ACR_DAC_Output,
    ACR_WAIT_IRMS_1S,
    ACR_Set_Gain,
    ACR_WAIT_VRMS_1S,
    ACR_Read_Vrsm,
    ACR_Read_Irsm,
    ACR_Cal_ACR,
    ACR_WAIT_NEXT_TEST,
    ACR_STOP_TEST
};

enum
{
    ACIM_IDLE,
    ACIM_SET_DDS_FREQ,
    ACIM_WAIT_Relay_On,
    ACIM_SetResValue,
    ACIM_Output_SineWave,
    ACIM_DCMeasure_Start_H,  //05/18/2018
    ACIM_DCMeasure_Start_L,
    ACIM_WAIT,
    ACIM_CalDC_Value,
    ACIM_DAC_Output,
    ACIM_WAIT_IRMS_1S,
    ACIM_ACMeasure_Start_H,  //05/18/2018
    ACIM_ACMeasure_Start_L,
    ACIM_Set_Gain,
    ACIM_WAIT_VRMS_1S,
    ACIM_Read_Vrms,
    ACIM_GOTO_FAST_MODE,
    ACIM_WAIT_FAST_MODE_1S,
    ACIM_SAMPLE_SINWAVE,
    ACIM_SAMPLE_SINWAVE_H,  //04/18/2018
    ACIM_SAMPLE_SINWAVE_M,  //05/18/2018
    ACIM_SAMPLE_SINWAVE_L,  //04/18/2018
    ACIM_SAMPLE_VBK,  //05/18/2018
    ACIM_GOTO_NORMAL_MODE,
    ACIM_WAIT_NORMAL_MODE_1S,
    ACIM_CONV_DATA,
    ACIM_CAL_FFT,
    ACIM_CONFIRM_VALUE,
    ACIM_WAIT_NEXT_TEST,
    ACIM_STOP_TEST,
    ACIM_ADC_SAMPLE_RATE,  //04/18/2018
    ACIM_DELAY,
		ACIM_HANGUP
};

enum
{
    ACIM_FROM_10_TO_100_mOhm,
    ACIM_FROM_100_TO_1000_mOhm
};

enum
{
    ACIM_CURRENT,
    ACIM_VOLTAGE,
    ACIM_RESISTANCE,
    ACIM_PHASE,
    ACIM_IMPEDANCE
};

class Aux_ACIMTest
{
public:
    static unsigned short                 Priority;
    static unsigned short                 TaskID;
    static float   ACIM_I_Sum;
    static unsigned  char  ACIM_I_Cnt;
    static float  ACIM_DC_Value;
    static  float   ACIM_Mac[MAX_ACIM_TEST_NUM];	// Mod
    static  float   ACIM_Pac[MAX_ACIM_TEST_NUM];	// Phase
    static  float   ACIM_Rac[ACIM_TEST_REPETITION];
    static  float   ACIM_5160_Value;   // 用于设定正弦波幅值     3~255  : 0.05~1A
    static  float   ACIM_SineAmplitude_value;
    static  float   ACIM_Rac_Avg;
    static  float   ACIM_Pac_Avg;
    static bool    b_ACIM_StartTest_Flag;
    static bool    b_ACIM_TestFinished_Flag;
    static bool    b_ACIM_DC_StartMea_Flag;
    static  bool   b_ACIM_OpenCircuit_CheckFlag;
    static bool    b_ACIM_FREQUENCY_High_Flag;  //04/18/2018
    static bool    b_ACIM_FREQUENCY_Low_Flag;  //05/17/2018
    static unsigned char   uc_test_counter;
    static float    sum_Rac;
    static unsigned char    uc_ACIM_TEST_Source;    //要求启动ACIM 测试来源， 0xAA: Listenfornet  校准，   0xBB： 上位机Schedule控制
    static  unsigned char   uc_ACIM_CALI_OhmRange;
    static unsigned int Idata[256];
    static unsigned int Vdata[256];
    static unsigned int VBKdata[256];  //04/12/2018
    static unsigned int Vtemp[20];  //03/22/2018
    static float f_Frequency;  //04/10/2018
    static unsigned int ACIMTestNum;
    static float Frequency_List[MAX_ACIM_TEST_NUM];
    static float f_FrequencyH;
    static float f_FrequencyL;
    static unsigned int PointPerDecade;
	static bool				b_lastPoint;
// 	static void (*m_WriteDO)(void);
// 	static void (*m_InitDO)(void);

private:
    static  unsigned char  ACIM_Status;
    static  unsigned short TimeTicks;
    static  float   ACIM_Irms;
    static  float   ACIM_Irms_Openload;
    static  float   ACIM_Vrms;
    static unsigned int TestId;

    static  unsigned short Gain_coefficient;

public:
    static void Init(void);
    static void DoWork(void);
    static void Set_ACIM_Status( unsigned char status);
    static void ReportData(float value, MsTime time, int type);
    static void FFT_RealSamp( float* data, unsigned short nn );
    static void FFT_TwoReal(float* data1, float* data2, float * fft1, float* fft2, unsigned short n);
    static void ProcessACR(void);
    static void ProcessACIM(void);
		static void ProcessCurrentSource(void);

    static float pC[16 * 16]; //10.16
    static float pV[16 * 16]; //10.16
    static float pVBK[16 * 16];  //04/12/2018
    static float IFFT[256*2];
    static float VFFT[256*2];
    static float pVtemp[20];  //03/22/2018

    static float DCV;  //03/15/2018
    static float DCV1;  //03/16/2018
    static float pVmax;  //03/20/2018
    static float pVmin;  //03/20/2018

    static float* pCurrent;
    static float* pVoltage;
    static float* p_IFFT;
    static float* p_VFFT;

};
