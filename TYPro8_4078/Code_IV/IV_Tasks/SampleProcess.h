//改名为SampleProcess
//data acquisition
//updata 2012 11 14 by chen

/*
任务一：采样任务

adc SampleProcess_period 进行周期采样，参考.net的原先设计：比如1ms 1次采样，每次采样1个adc通道

总体完成80%
*/

//SampleProcess
#ifndef _SAMPLE_PROCESS_H_
#define _SAMPLE_PROCESS_H_

// #define  DATA_ACQ_PERIOD    400000 //XXms    //DAQ任务周期

#include "../../COMMON_OS/Protocols/SF_Hwf.h"

#define TSample_ErrorCount		3
#define MAX_AUXCH_PER_DATA_GROUP        16

//此对应Data_acq_getType
enum
{
    SAMPLE_PROCESS_ADC_V = 0xB1,
    SAMPLE_PROCESS_ADC_I,
// 	SAMPLE_PROCESS_ADC_T
};

//ADC类型
// enum
// {
// 	SAMPLE_PROCESS_SIGMA_DELTA_ADC = 0x02,
// 	SAMPLE_PROCESS_S_A_ADC,    //逐次比较
// 	SAMPLE_PROCESS_MUL_CHN
// };

//sample ctrl type    采样控制类型
enum
{
    SAMPLE_CTRL_IMMEDIATE = 0xB0,    //立即数
    SAMPLE_CTRL_AVERAGE_FILTER,        //平均滤波
    SAMPLE_CTRL_MEDIAN_AVERAGE_FILTER,    //中位值平均滤波
    SAMPLE_CTRL_MOVING_AVERAGE,     //滑动平均滤波
    SAMPLE_CTRL_CODE_WORD               //for pid 立即数的码字
};
// enum
// {
// 	DI_IDLE=0,
// 	DI_RD,
// 	DI_WAIT,
// 	DI_SAVE,
// };
enum TE_CHECK_T_VOLTAGERANG
{
    COUNT_TEMPERATURE = 0,
    CHECK_T_VOLTAGERANG ,
};
#define MAX_IV_SAMPLE_OVERTIME (10000)
/*
1 立即数型
sample_task任务以固有周期T进行
每T个周期，从ADC直接采出一个立即数，送入大数据体内作为PV值

2 中位值平均滤波
连续采N个数据，去掉最大和最小
然后除以N-2
N选取3～14
缺点是 此周期较长，我们定为T2 为 N*T1（1 立即数周期）

3 滑动平均
把连续取N个采样值看成一个队列
队列的长度固定为N
每次采样到一个新数据放入队尾,并扔掉原来队首的一次数据.(先进先出原则)
把队列中的N个数据进行算术平均运算,就可获得新的滤波结果
*/

class Sample_Pro
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]
    static unsigned char NewCodeSampled[MAXCHANNELNO];
		static MsTime BulitInSampleErrorTime[2][MAX_AUXCH_PER_DATA_GROUP];
    static MsTime SampleOverTime[MAXCHANNELNO];
    static unsigned char NewIRSampled[MAXCHANNELNO];
    static unsigned char Digital_SampleCounter[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];

    static void Init(void);
    static void DoWork(void);
// 	static void EndTask(void);
    static void sample_Pro_Data(char chn,unsigned long int value,char type);

    static unsigned long avgQueue(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth);    //求平均
    static float Convert_Chn(unsigned char data_group_idx,unsigned char chn);
    static void enQueue(unsigned char data_group_idx,unsigned char chn);

protected:

private:
// 	static char Sample_Pro_Adc_device_num;    //adc器件通道数
    //函数指针  指向LinearX中的驱动
// 	static int (* pSample_Pro_ProcessADC) (char adc_device_chn);
// 	static char (* pSample_Pro_getCHN)(char adc_device_chn);
// 	static unsigned long int  (* pSample_Pro_getValue)(char adc_device_chn);   //获得码字
// 	static char (* pSample_Pro_getType)(char adc_device_chn);
    static void ProcessBuiltInData(void);
    static void ProcessInChipTemperature (unsigned char data_group_idx);
    static void enQueue_InChipTemp( unsigned char data_group_idx);
    static unsigned long avgQueue_InChipTemp(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth);
    static float Convert_Regular_2ndV(unsigned char data_group_idx, unsigned char chn,unsigned long code);
    static float Convert_ThermoCouple(unsigned char data_group_idx, unsigned char TC_type,unsigned char chn, unsigned long code);
    static void Convert_InChipTemp(unsigned char data_group_idx);
    static float Calculate_RefVoltage(unsigned char TC_type,float Ref_Temp); //根据参考温度计算参考电压
    static float T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_JType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_EType(float f_Value,unsigned char uc_CheckFlag);
};

#endif
