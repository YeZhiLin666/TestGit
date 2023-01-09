
#ifndef _HS21044_SampleManage_H_
#define _HS21044_SampleManage_H_

// #define HS21044_ADC_NUM 2
// #define HS21044_ADC_I_CHN_MAX  1

// #define HS21044_ADC_DG_BUILT_UP_TIME_TICKS    6    //ģ���Ž���ʱ��  600us

class HS21044_SampleManage
{
public:
    static UInt16 TaskID;          //Add by DKQ 08.20.2014                                                          //����ID
    static UInt16 Priority;                                                                //���ȼ�

    static void Init(void);
    static char SampleISR(unsigned long CH_Number,unsigned long type);
    static void DoWork(void);
    // 	static int ProcessADC(char adc_device_chn);
    //
    // 	static char getCHN(char adc_device_chn);
    // 	static unsigned long int  getValue(char adc_device_chn);
    // 	static char getType(char adc_device_chn);

    //IR���������ADC����
    static char IR_ADC(char chn);
    static char renew_IR_ADC(void);

    static void  IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);
    static void  IR_Read_ADC_BV(char chn,unsigned long *SampleV,unsigned long *SampleI,unsigned long *SampleBV);
    static void  Normal_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI);

    //end /IR���������ADC����

protected:

private:
    //for �ӿں���
    static char m_chn;
    static unsigned long int m_value;
    static char m_type;

    //�ڲ���ת
    static char m_ADC_chn;

    //ʱ��
    static SysTime sample_time;

    //IR
    static char chn_DoIR;
    static char chn_history;
    static char ADC_Status;
// 	static unsigned short TimeTicks;

    static unsigned char IR_CTRL_FLAG;
// 	static unsigned long CurrSum1 ;//ADC�����ۼ����ֵ���õ�ַ
// 	static unsigned long Count1;
// 	static unsigned long CurrMax1;//ADC�������ֵ
// 	static unsigned long CurrMin1;//ADC������Сֵ
// 	static unsigned long CurrSum2 ;//ADC�����ۼ����ֵ���õ�ַ
// 	static unsigned long Count2;

};


#endif
