/*****************************************************************************
// �����ļ�      :ADC32bit_LTC2498.cpp     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ32λ��������Ӳ��оƬLTC2498������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#ifndef _ADC32bit_LTC2498_H_
#define _ADC32bit_LTC2498_H_

#include "../LPC4078CORE/SSPI.h"
#include "includes.h"

// User Definition
// #define ADC_LTC2498_ENABLE 1
// define how many ADC is possible in this application(in this PCA)
// #define LTC2498_MAXADC_COUNT		16+1    //�ڲ��¶�ͨ��

//<<<End>>> User Definition

class Adc32bit_LTC2498
{
public:

    static St_Port  Pin_CS;
    static St_Port  Pin_Busy;
    static unsigned char IsExCrystalsFlag;    //�Ƿ�ʹ���ⲿ����

    //static unsigned char CodeType;    //Bipolar Code Type or Unipolar Code Type;

    static unsigned long Trigger(unsigned char sspId,unsigned char ADCchn);    //first time no return value
    static unsigned long Read(unsigned char sspId);
    //		static void setCS(const St_Port * cs);
    //		static St_Port getCS(void);

private:

};

#endif

