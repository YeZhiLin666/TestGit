
#ifndef _HS21044_SampleManage_H_
#define _HS21044_SampleManage_H_

// #define HS21044_ADC_NUM 2
// #define HS21044_ADC_I_CHN_MAX  1

// #define HS21044_ADC_DG_BUILT_UP_TIME_TICKS    6    //模拟门建立时间  600us

class HS21044_SampleManage
{
public:
    static UInt16 TaskID;          //Add by DKQ 08.20.2014                                                          //任务ID
    static UInt16 Priority;                                                                //优先级

    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);
    static void DoWork(void);
    // 	static int ProcessADC(char adc_device_chn);
    //
    // 	static char getCHN(char adc_device_chn);
    // 	static unsigned long int  getValue(char adc_device_chn);
    // 	static char getType(char adc_device_chn);

    //IR触发特殊的ADC控制
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);

    static void  IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    static void  IR_Read_ADC_BV(char chn,unsigned long *SampleV,unsigned long *SampleI,unsigned long *SampleBV);
    static void  Normal_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);

    //end /IR触发特殊的ADC控制

protected:

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
    static char chn_DoIR;
    static char chn_history;
    static char ADC_Status;
// 	static unsigned short TimeTicks;

    static unsigned char IR_CTRL_FLAG;
// 	static unsigned long CurrSum1 ;//ADC采样累加求和值放置地址
// 	static unsigned long Count1;
// 	static unsigned long CurrMax1;//ADC采样最大值
// 	static unsigned long CurrMin1;//ADC采样最小值
// 	static unsigned long CurrSum2 ;//ADC采样累加求和值放置地址
// 	static unsigned long Count2;

};


#endif
