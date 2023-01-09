
#ifndef _BT11_SampleManage_H_
#define _BT11_SampleManage_H_

#define BT11_ADC_NUM 2
#define BT11_ADC_I_CHN_MAX  1

#define BT11_ADC_DG_BUILT_UP_TIME_TICKS    6    //模拟门建立时间  600us

class BT11_SampleManage
{
public:
	static UInt16 TaskID;          //Add by DKQ 08.20.2014                                                          //任务ID
	static UInt16 Priority;                                                                //优先级

	static void Init(void);
	static char SampleISR(unsigned long CH_Number,unsigned long type);
	static void DoWork(void);	  
	static int ProcessADC(char adc_device_chn);

	static char getCHN(char adc_device_chn);
	static unsigned long int  getValue(char adc_device_chn);
	static char getType(char adc_device_chn);

	//IR触发特殊的ADC控制
	static char IR_ADC(char chn);
	static char renew_IR_ADC(void);

	static void  IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
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
	static unsigned short TimeTicks;

	static unsigned char IR_CTRL_FLAG;
};


#endif
