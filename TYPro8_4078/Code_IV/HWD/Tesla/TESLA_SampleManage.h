
#ifndef _TESLA_SampleManage_H_
#define _TESLA_SampleManage_H_

// #define TESLA_ADC_NUM 2
#define TESLA_ADC_I_CHN_MAX  (MAXCHANNELNO)  // 16

#define TESLA_ADC_ADG_TICKS 10    //600us Open ADG--->200us Open ADG seens more good
#define TESLA_ADC_CONV_TICKS 2
#define TESLA_ADC_READ_TICKS 2

class TESLA_SampleManage
{
public:

	static void Init(void);
	static char SampleISR(unsigned long CH_Number,unsigned long type);

	//IR触发特殊的ADC控制
	static char IR_ADC(char chn);
	static char renew_IR_ADC(void);

	static void IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *Sample);
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

	//IR
	static char chn_history;
	static char chn_DoIR;
	static char ADC_Status;

	static unsigned char IR_CTRL_FLAG;
	static unsigned short TimeTicks;
	static const unsigned short DelayTickCount_ADG;  //600us????
	static const unsigned short DelayTickCount_AdcConv;  
	static const unsigned short DelayTickCount_AdcReadData; 
};


#endif
