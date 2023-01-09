#pragma once

class PWM_SampleManage
{
public:
    static float m_InsideCtrlVoltageValue;
    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);

    //IR触发特殊的ADC控制
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);
    static void IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    static unsigned long CaculateFilterAverage(unsigned long *Array,unsigned char Lenth);
    static unsigned long WaitForSampleCounter;
    static unsigned long m_u32ChannelVoltageAdc;
private:
// 	static char m_chn;
// 	static char m_type;
    static char ADC_Status;
    static char ADC_Tick;
    static unsigned long SampleDelayCounter;
    static char ADC1_Status;
    static unsigned char CVdelayCounter;
};

