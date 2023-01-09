/*****************************************************************************
// �����ļ�      :ADC.h     LPC4078�ڲ�12bitADC����
// �ļ�����      :ΪMCU board����
// ��д����      :2014.03.11
*****************************************************************************/

#ifndef _ADCFUNC_H_
#define _ADCFUNC_H_

//LPC4078_12bitADC

#define MAXINTERNALADC 8    //�Ժ�Ҫ�ŵ���������
#define ADC_VALUE_MAX 0xFFF

class ADCFunc
{
private:
    static unsigned char      CodeType;
    static float LPC4078ADC_Data[MAXINTERNALADC];  //�Ժ�Ҫ�ŵ���������

public:
    static void            Init        (void);
    static void            StartConvert (void);
    static void 			StopConvert(void);
    static float   		Read        (unsigned char ADC_Channel);
    static unsigned long ReadADC_Code(unsigned char ADC_Channel);
    static void           Trigger     (unsigned char ADC_Channel);
    static unsigned long    ReadData    (void);
    static void           Close      (void);

};

#endif
