/*****************************************************************************
// 程序文件      :ADC32bit_LTC2498.cpp     主要用于模数转换,将为模拟量输入转化为32位数字量的硬件芯片LTC2498驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#ifndef _ADC32bit_LTC2498_H_
#define _ADC32bit_LTC2498_H_

#include "../LPC4078CORE/SSPI.h"
#include "includes.h"

// User Definition
// #define ADC_LTC2498_ENABLE 1
// define how many ADC is possible in this application(in this PCA)
// #define LTC2498_MAXADC_COUNT		16+1    //内部温度通道

//<<<End>>> User Definition

class Adc32bit_LTC2498
{
public:

    static St_Port  Pin_CS;
    static St_Port  Pin_Busy;
    static unsigned char IsExCrystalsFlag;    //是否使用外部晶振

    //static unsigned char CodeType;    //Bipolar Code Type or Unipolar Code Type;

    static unsigned long Trigger(unsigned char sspId,unsigned char ADCchn);    //first time no return value
    static unsigned long Read(unsigned char sspId);
    //		static void setCS(const St_Port * cs);
    //		static St_Port getCS(void);

private:

};

#endif

