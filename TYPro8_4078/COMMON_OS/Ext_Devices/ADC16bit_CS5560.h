/*****************************************************************************
// �����ļ�      :ADC16bit_CS5560.h     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ16λ��������Ӳ��оƬCS5560������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#ifndef _ADC16bit_CS5560_H_
#define _ADC16bit_CS5560_H_




#define ADC_CS5560_RST 1<<26    //P1.26
#define ADC_CS5560_TIME_OUT 50000


//ʹ��SPI���߽�������
class Adc16bit_CS5560
{
public:

    static void Adc_Init(unsigned char Device_Number);
    static void CS5560_CONV(void);
    static unsigned long int  CS5560_Read(char chn,char IVtype);
    static void Adc_reset(unsigned char Device_Number);
    static unsigned long int BPToUP(unsigned long int temp);
};
#endif

