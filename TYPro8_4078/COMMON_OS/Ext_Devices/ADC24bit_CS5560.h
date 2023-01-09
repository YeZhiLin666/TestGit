/*****************************************************************************
// 程序文件      :ADC24bit_CS5560.h     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片CS5560驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
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
    static unsigned long   Read        (void);   //此芯片就只有一个CHANNEL，无需传递ADC CHN 形参了
    static void            Trigger     (void);
    static unsigned long   ReadData    (void);
    static void            Reset       (void);
};


