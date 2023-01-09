
#ifndef _HPT200_SampleManage_H_
#define _HPT200_SampleManage_H_

// #define HPT200_ADC_NUM 2
// #define HPT200_ADC_I_CHN_MAX  1

// #define HPT200_ADC_DG_BUILT_UP_TIME_TICKS    6    //模拟门建立时间  600us
#define MAXSAMPLETIMECOUNTER    4500
#define MINSAMPLETIMECOUNTER    15

class HPT200_SampleManage
{
public:

    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);
    // 	static int ProcessADC(char adc_device_chn);
    //
    // 	static char getCHN(char adc_device_chn);
    // 	static unsigned long int  getValue(char adc_device_chn);
    // 	static char getType(char adc_device_chn);

    //IR触发特殊的ADC控制
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);

    static void  IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);

    static unsigned long WaitForSampleCounter;

    //end /IR触发特殊的ADC控制

protected:

private:
    //for 接口函数
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;

    //内部运转
    //static char m_I_chn;
    //static char m_V_chn;
    static char m_ADC_chn;

    //时间
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

// 	static MsTime TimeT(MsTime a,MsTime b);
};


#endif
