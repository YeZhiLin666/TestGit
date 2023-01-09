
#ifndef _LBT21XXX_SampleManage_H_
#define _LBT21XXX_SampleManage_H_

#define LBT21XXX_ADC_NUM 2
#define LBT21XXX_ADC_I_CHN_MAX  15  //(MAXCHANNELNO)  // 16

#define LBT21XXX_ADC_DG_BUILT_UP_TIME_TICKS    6    //模拟门建立时间  600us

enum eADG_DelayTickCount
{
		eADG_DelayTickCountFor0us 	= 0,
    eADG_DelayTickCountFor100us = 2,
    eADG_DelayTickCountFor400us = 8,
};

class LBT21XXX_SampleManage
{
public:

    static void Init(void);
		static void SetADG_DelayTickCount(eADG_DelayTickCount count);
    static char SampleISR(unsigned long CH_Number,unsigned long type);
		static void ResetSampleChanNumber(void);

    //IR触发特殊的ADC控制
    static char IR_ADC(char chn);
    static void IR_Trigger_ADC(void);
    static char renew_IR_ADC(void);

    static void IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    //end /IR触发特殊的ADC控制

    static unsigned char I_ADG_Address[MAXCHANNELNO];
    static void Adc24bit_AD7175_Work(void);
    static void Adc24bit_CS5560_Work(void);
private:
    //for 接口函数


    //内部运转
    static char m_ADC_chn;


    //时间
    static SysTime sample_time;

    //IR
    static char chn_history;
    static char chn_DoIR;
    static char ADC_Status;
    static unsigned short TimeTicks;
    static unsigned short DelayTickCount_ADG;  //600us开模拟门
// 	static const unsigned short DelayTickCount_AdcConv;
// 	static const unsigned short DelayTickCount_AdcReadData;

    static unsigned char IR_CTRL_FLAG;
    static unsigned char IR_SWITCH_FLAG;   //170207,hpq


    static unsigned long RestADC;
    static unsigned char SampleChanNumber;

};


#endif
