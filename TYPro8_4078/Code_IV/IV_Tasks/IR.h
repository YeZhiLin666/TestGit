///////////////////////////////////////////////////////////////////////////////////
//File Name: IR.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						IR����
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#ifndef _IR_H_
#define _IR_H_

#define MAXPULSE     10 // 200    //  ԭΪ10������    // ������֤���ƼӴ�200���� 
#define AMPULSE     2
#define PERCRATE     0.05                    //IR������������ķ����������Ķ���
#define MAXSPECIALIRSTAGETIME  300           //IR��������ʱ����ʱ�趨Ϊ30ms
#define  Fpclk       60000000                   //���õ�����ʱ��Ϊ60MHz
#define IR_INTERVAL100us (Fpclk/10000 - 1)     // 100΢��

enum
{

    TIMEDIVISION,
    TIMERINT,
};
enum
{
    IRSTAGEHIGH=0,
    IRSTAGELOW,
    SAMPLEHIGH,
    SAMPLELOW,
    CHECKIFIO,
    IRCALCULATE,
    //Timer1
// 	IRTIMERIDLE=0x10,
// 	INTERVALT1,
// 	INTERVALT2,
// 	INTERVALT3,
// 	INTERVALT4,
// 	INTERVALT5,
// 	INTERVALT6,


    OUTPUT_DAC_1st,
    TRIGGER_ADC_1st,
    SAMPLE_DATA_1st,
    OUTPUT_DAC_2nd,
    TRIGGER_ADC_2nd,
    SAMPLE_DATA_2nd,
    OUTPUT_CURRENT_0,
// 	IRSPECIALSTAGEHIGH=0x20,
// 	IRSPECIALSTAGELOW,
// 	IRSPECIALSAMPLEHIGH,
// 	IRSPECIALSAMPLELOW,
// 	IRSPECIALCHECKIFIO,
// 	IRSPECIALCALCULATE,
};

typedef struct
{
    unsigned long    HiStageV[MAXPULSE];
    unsigned long    LoStageV[MAXPULSE];
    unsigned long    HiStageI[MAXPULSE];
    unsigned long    LoStageI[MAXPULSE];
} St_IR_SampCode;

class IR
{
private:
    static unsigned char IR_Status[MAXCHANNELNO];
    static unsigned char IR_StageIO[MAXCHANNELNO];
    static MsTime IR_StartTime[MAXCHANNELNO];	//0819
// 	static MsTime IR_TimeInterval[MAXCHANNELNO];
    static unsigned long long IR_TimeInterval[MAXCHANNELNO];
    static float IR_Stage[MAXCHANNELNO];
// 	static unsigned char CurrentIRChannel;
    static float IR_SampleInterval[MAXCHANNELNO];
    static unsigned char IR_SampleFlagH[MAXCHANNELNO];
    static unsigned char IR_SampleFlagL[MAXCHANNELNO];


public:
//	static float SaveIRVoltage[MAXCHANNELNO];//��ʱ���ڴ���IR��ѹ���⣬������������������ɾ��
    static unsigned char DAC_Flag;
    static unsigned char ADG_Ready; // add by DKQ 01.27.2017
    static UWord32 WaitTime;
//	static unsigned char WaitRecord;

    static St_IR_SampCode SampCode[MAXCHANNELNO];

    static float IR_IHigh[MAXCHANNELNO][MAXPULSE];
    static float IR_ILow[MAXCHANNELNO][MAXPULSE];
    static float IR_VHigh[MAXCHANNELNO][MAXPULSE];
    static float IR_VLow[MAXCHANNELNO][MAXPULSE];

    static unsigned char IR_PulseRecord[MAXCHANNELNO];
    static unsigned long IR_IHighDacCode[MAXCHANNELNO];
    static unsigned long IR_ILowDacCode[MAXCHANNELNO];
    static unsigned char IR_CtrlType[MAXCHANNELNO];
    static float IR_Value[MAXCHANNELNO];

// IR_T1,, T5 is replaced by the IR_Output_DAC_1st ,,,, IR_Output_Current_0
//	static UWord32 IR_T1[MAXCHANNELNO];
//	static UWord32 IR_T2[MAXCHANNELNO];
//	static UWord32 IR_T3[MAXCHANNELNO];
//	static UWord32 IR_T4[MAXCHANNELNO];
//	static UWord32 IR_T5[MAXCHANNELNO];
    static UWord32 IR_Output_DAC_1st[MAXCHANNELNO];
    static UWord32 IR_Trigger_ADC_1st[MAXCHANNELNO];
    //static UWord32 IR_Sample_Data_1st[MAXCHANNELNO];
    static UWord32 IR_Output_DAC_2nd[MAXCHANNELNO];
    static UWord32 IR_Trigger_ADC_2nd[MAXCHANNELNO];
    //static UWord32 IR_Sample_Data_2nd[MAXCHANNELNO];
    static UWord32 IR_Output_Current_0[MAXCHANNELNO];

    static unsigned char IRTimerStatus;
    static unsigned char IRChannelNo;
    static unsigned short IRChannelFlg;

    //static unsigned short IRChannelFlg_CopyInit[2];   // [X]   0:ԭֵ 1������ֵ
    //static unsigned char IR_CopyFlg;

    static unsigned char IR_TimerEn;
    static unsigned char CheckIRChannel;

    static unsigned short IRSpecialGotoNext[MAXCHANNELNO];
    static unsigned char IRSpecialRange[MAXCHANNELNO];

    static unsigned char IR_TriggerFlag;    //��ʱʹ��
    static unsigned int Set_LowPulseDacTime;
    static unsigned int Set_LowPulseAdcTime;
    static unsigned int Set_HighPulseDacTime;
    static unsigned int Set_HighPulseAdcTime;
    static unsigned int Set_MinimumPulseTime;   //�ⲿ�ֳ�ʼ���ó��ŵ��������Base�
private:
    static float PulseH[MAXCHANNELNO];
    static float PulseL[MAXCHANNELNO];
    static float DAmp[MAXCHANNELNO];			//Amp����ϵ��
    static float MinAmp[MAXCHANNELNO];						//��СAmp
    static float Offset[MAXCHANNELNO];
    static unsigned char Range[MAXCHANNELNO];


//	static unsigned char bFastMeasure;
private:

    static void IR_Calculate(unsigned char uc_ChannelNo);
    static unsigned char IR_SpecialCalculate(unsigned char uc_ChannelNo);
    static unsigned char IR_CheckFail(void);
public:
    static void InitIR(unsigned char uc_ChannelNo);  //IR��ʼ��
    static void ProcessIR(unsigned char uc_ChannelNo);  //IR����
    static void ProcessIRTimer1(void); //IR��ʱ���жϴ���

    static void IR_AmpAdjust(unsigned char uc_ChannelNo);
    static unsigned char IR_AmpCheck(unsigned char uc_ChannelNo);

    static char (* pIR_ADC)(char uc_ChannelNo);    //IR�ж��֧�ֺ�������
    static char (*pRenew_IR_ADC)(void);
    static void (*pIR_Trigger_ADC)(void);
    static void (*pIR_Read_ADC)(char uc_ChannelNo,unsigned long *SampleV,unsigned long *SampleI);
};
#endif
