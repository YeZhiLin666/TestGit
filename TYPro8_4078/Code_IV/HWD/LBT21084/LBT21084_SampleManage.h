
#ifndef _LBT2108_SampleManage_H_
#define _LBT2108_SampleManage_H_

#define LBT2108_ADC_NUM 2
#define LBT2108_ADC_I_CHN_MAX  15  //(MAXCHANNELNO)  // 16

#define LBT2108_ADC_DG_BUILT_UP_TIME_TICKS    6    //模拟门建立时间  600us

class LBT21084_SampleManage
{
public:

    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);

    //IR触发特殊的ADC控制
    static char IR_ADC(char chn);
    static void IR_Trigger_ADC(void);
    static char renew_IR_ADC(void);

    static void IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    //end /IR触发特殊的ADC控制

    static unsigned char I_ADG_Address[MAXCHANNELNO];

private:
    //for 接口函数
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;

    //内部运转
    static char m_ADC_chn;


    //时间
    static SysTime sample_time;

    //IR
    static char chn_history;
    static char chn_DoIR;
    static char ADC_Status;
    static unsigned short TimeTicks;
    static const unsigned short DelayTickCount_ADG;  //600us开模拟门
    static const unsigned short DelayTickCount_AdcConv;
    static const unsigned short DelayTickCount_AdcReadData;

    static unsigned char IR_CTRL_FLAG;
    static unsigned char IR_SWITCH_FLAG;   //170207,hpq
    static unsigned int Trigger_Counter;

    static unsigned long RestADC;
};


#endif
