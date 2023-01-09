
#ifndef _IBT31162_SampleManage_H_
#define _IBT31162_SampleManage_H_

#define IBT31162_ADC_NUM 2
#define IBT31162_ADC_I_CHN_MAX  15  //(MAXCHANNELNO)  // 16

#define IBT31162_ADC_DG_BUILT_UP_TIME_TICKS    6    //ģ���Ž���ʱ��  600us

#define IBT31162_ADC_READTIMEOUT   20      // 50us * 20 = 1ms


class IBT31162_SampleManage
{
public:

    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);

    //IR���������ADC����
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);

    static void IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    //end /IR���������ADC����

    static unsigned char I_ADG_Address[MAXCHANNELNO];

private:
    //for �ӿں���
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;

    //�ڲ���ת
    static char m_ADC_chn;
    static unsigned char m_ch_Index;

    //ʱ��
    static SysTime sample_time;

    //IR
    static char chn_history;
    static char chn_DoIR;
    static char ADC_Status;
    static unsigned short TimeTicks;
    static unsigned short TimeOut_Count;
    static const unsigned short DelayTickCount_ADG;  //600us��ģ����
    static const unsigned short DelayTickCount_AdcConv;  
    static const unsigned short DelayTickCount_AdcReadData; 

    static unsigned char IR_CTRL_FLAG;
};


#endif
