/*****************************************************************************
// 程序文件      :ADC16bit_AD7606.h     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片AD7606驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/
#pragma once
/*********************************************************************************
**                            Adc pin  Define
*********************************************************************************/


#define  ADC_DELAYTIME		100




class Adc16bit_AD7606
{
    //Attribute
public:
    static St_Port	OS0;
    static St_Port	OS1;
    static St_Port	OS2;
    static St_Port	CONVA;
    static St_Port	CONVB;
    static St_Port	Rst;
    static St_Port 	BUSY;
    static St_Port	FDATA;
    static unsigned char CodeType;
    static unsigned long Address;

public:
    static void            Init          (unsigned char ConvCodeType);
    static void            Convert       (void);
    static void            Read          (unsigned long *Data1,unsigned long *Data2);
    static void            ReadAfterBusy (unsigned long *Data1,unsigned long *Data2);
    static void            ReadAfterBusy4Ch (unsigned long *Data1,unsigned long *Data2,unsigned long *Data3,unsigned long *Data4);
    static void            Reset         (void);
};

