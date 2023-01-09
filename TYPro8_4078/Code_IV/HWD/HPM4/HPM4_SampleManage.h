
#ifndef _HPM4_SampleManage_H_
#define _HPM4_SampleManage_H_

// #define HPM4_ADC_NUM 2
// #define HPM4_ADC_I_CHN_MAX  1

// #define HPM4_ADC_DG_BUILT_UP_TIME_TICKS    6    //ģ���Ž���ʱ��  600us
#define MAXSAMPLETIMECOUNTER    4500
#define MINSAMPLETIMECOUNTER    15

class HPM4_SampleManage
{
public:
    static char m_ADC_chn;
    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);
    //IR���������ADC����
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);

    static void  IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);

    static unsigned long WaitForSampleCounter;

    //end /IR���������ADC����

protected:

private:
    //for �ӿں���
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;
    static const unsigned short DelayTickCount_ADG;

    //�ڲ���ת
    //static char m_I_chn;
    //static char m_V_chn;
// 		static char m_ADC_chn;

    //ʱ��
    static SysTime sample_time;

    //IR
    static char chn_DoIR;
    static char chn_history;
    static char ADC_Status;
    static unsigned short TimeTicks;
// 		static MsTime RecordTime;
// 		static MsTime LastTime;
    static unsigned long SampleDelayCounter;

    static unsigned char IR_CTRL_FLAG;

// 		static MsTime TimeT(MsTime a,MsTime b);   //delete by qjm 20171222
};


#endif
