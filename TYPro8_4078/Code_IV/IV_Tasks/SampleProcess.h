//����ΪSampleProcess
//data acquisition
//updata 2012 11 14 by chen

/*
����һ����������

adc SampleProcess_period �������ڲ������ο�.net��ԭ����ƣ�����1ms 1�β�����ÿ�β���1��adcͨ��

�������80%
*/

//SampleProcess
#ifndef _SAMPLE_PROCESS_H_
#define _SAMPLE_PROCESS_H_

// #define  DATA_ACQ_PERIOD    400000 //XXms    //DAQ��������

#include "../../COMMON_OS/Protocols/SF_Hwf.h"

#define TSample_ErrorCount		3
#define MAX_AUXCH_PER_DATA_GROUP        16

//�˶�ӦData_acq_getType
enum
{
    SAMPLE_PROCESS_ADC_V = 0xB1,
    SAMPLE_PROCESS_ADC_I,
// 	SAMPLE_PROCESS_ADC_T
};

//ADC����
// enum
// {
// 	SAMPLE_PROCESS_SIGMA_DELTA_ADC = 0x02,
// 	SAMPLE_PROCESS_S_A_ADC,    //��αȽ�
// 	SAMPLE_PROCESS_MUL_CHN
// };

//sample ctrl type    ������������
enum
{
    SAMPLE_CTRL_IMMEDIATE = 0xB0,    //������
    SAMPLE_CTRL_AVERAGE_FILTER,        //ƽ���˲�
    SAMPLE_CTRL_MEDIAN_AVERAGE_FILTER,    //��λֵƽ���˲�
    SAMPLE_CTRL_MOVING_AVERAGE,     //����ƽ���˲�
    SAMPLE_CTRL_CODE_WORD               //for pid ������������
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
1 ��������
sample_task�����Թ�������T����
ÿT�����ڣ���ADCֱ�Ӳɳ�һ�������������������������ΪPVֵ

2 ��λֵƽ���˲�
������N�����ݣ�ȥ��������С
Ȼ�����N-2
Nѡȡ3��14
ȱ���� �����ڽϳ������Ƕ�ΪT2 Ϊ N*T1��1 ���������ڣ�

3 ����ƽ��
������ȡN������ֵ����һ������
���еĳ��ȹ̶�ΪN
ÿ�β�����һ�������ݷ����β,���ӵ�ԭ�����׵�һ������.(�Ƚ��ȳ�ԭ��)
�Ѷ����е�N�����ݽ�������ƽ������,�Ϳɻ���µ��˲����
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

    static unsigned long avgQueue(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth);    //��ƽ��
    static float Convert_Chn(unsigned char data_group_idx,unsigned char chn);
    static void enQueue(unsigned char data_group_idx,unsigned char chn);

protected:

private:
// 	static char Sample_Pro_Adc_device_num;    //adc����ͨ����
    //����ָ��  ָ��LinearX�е�����
// 	static int (* pSample_Pro_ProcessADC) (char adc_device_chn);
// 	static char (* pSample_Pro_getCHN)(char adc_device_chn);
// 	static unsigned long int  (* pSample_Pro_getValue)(char adc_device_chn);   //�������
// 	static char (* pSample_Pro_getType)(char adc_device_chn);
    static void ProcessBuiltInData(void);
    static void ProcessInChipTemperature (unsigned char data_group_idx);
    static void enQueue_InChipTemp( unsigned char data_group_idx);
    static unsigned long avgQueue_InChipTemp(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth);
    static float Convert_Regular_2ndV(unsigned char data_group_idx, unsigned char chn,unsigned long code);
    static float Convert_ThermoCouple(unsigned char data_group_idx, unsigned char TC_type,unsigned char chn, unsigned long code);
    static void Convert_InChipTemp(unsigned char data_group_idx);
    static float Calculate_RefVoltage(unsigned char TC_type,float Ref_Temp); //���ݲο��¶ȼ���ο���ѹ
    static float T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_JType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_EType(float f_Value,unsigned char uc_CheckFlag);
};

#endif
