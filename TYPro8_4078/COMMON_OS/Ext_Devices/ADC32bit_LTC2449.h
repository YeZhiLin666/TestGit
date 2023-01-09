/*****************************************************************************
// �����ļ�      :ADC32bit_LTC2449.cpp     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ32λ��������Ӳ��оƬLTC2449������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#ifndef _ADC32bit_LTC2449_H_
#define _ADC32bit_LTC2449_H_
/*
#include "../../COMMON/Datatypes.h"
#include "../../COMMON/LPC177x_8x.h"
#include "../../COMMON/GPIO.h"
#include "../../COMMON/SSPI.h"
*/
#include "../LPC4078CORE/SSPI.h"
#include "includes.h"

// User Definition
#define ADC_LTC2449_ENABLE 1	 // only IOs are defined, no need to define a different one.
#define LTC2449_MAXADC_COUNT 16
#define LTC2449_SAMPLE_WAITTING_TIME    5//100us��λ

//<<<End>>> User Definition


class Adc32bit_LTC2449
{
public:
    static St_Port  Pin_CS;
    static St_Port  Pin_Busy;

    static unsigned char CodeType;    //Bipolar Code Type or Unipolar Code Type;

    static unsigned long Trigger(unsigned char sspId,unsigned char ADCchn);    //first time no return value
    static unsigned long Read(unsigned char sspId);
    static void setCS(const St_Port * cs);
    static St_Port getCS(void);

private:

};

#endif

