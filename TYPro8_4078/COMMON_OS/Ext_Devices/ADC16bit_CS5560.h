/*****************************************************************************
// 程序文件      :ADC16bit_CS5560.h     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片CS5560驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#ifndef _ADC16bit_CS5560_H_
#define _ADC16bit_CS5560_H_




#define ADC_CS5560_RST 1<<26    //P1.26
#define ADC_CS5560_TIME_OUT 50000


//使用SPI总线进行驱动
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

