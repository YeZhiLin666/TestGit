/*****************************************************************************
// �����ļ�      :ADC24bit_CS5560.h     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ16λ��������Ӳ��оƬCS5560������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/
#pragma once

#define ADC_CS5560_TIME_OUT 50000

class Adc24bit_CS5560
{
public:
    static unsigned char      CodeType;
    static       St_Port      Pin_Conver;
    static       St_Port      Pin_Busy;
    static       St_Port      Pin_CS;
    static       St_Port      Pin_Rst;
    static unsigned char      SPI_Idx;

public:
    static void            Init        (unsigned char ConvCodeType);
    static void            Convert     (void);
    static unsigned long   Read        (void);   //��оƬ��ֻ��һ��CHANNEL�����贫��ADC CHN �β���
    static void            Trigger     (void);
    static unsigned long   ReadData    (void);
    static void            Reset       (void);
};


