/*****************************************************************************
// 程序文件      :ADC16bit_AD7606.cpp     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片AD7606驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#include "includes.h"

unsigned char Adc16bit_AD7606::CodeType;
St_Port	Adc16bit_AD7606::OS0;
St_Port	Adc16bit_AD7606::OS1;
St_Port	Adc16bit_AD7606::OS2;
St_Port	Adc16bit_AD7606::CONVA;
St_Port	Adc16bit_AD7606::CONVB;
St_Port	Adc16bit_AD7606::Rst;
St_Port Adc16bit_AD7606::BUSY;
St_Port	Adc16bit_AD7606::FDATA;

unsigned long  Adc16bit_AD7606::Address;

void Adc16bit_AD7606::Init(unsigned char ConvCodeType)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
        //case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }
    ControlPin::SetMeLow(OS0);
    ControlPin::SetMeLow(OS1);
    ControlPin::SetMeLow(OS2);
    ControlPin::SetMeHigh(CONVA);
    ControlPin::SetMeHigh(CONVB);
    Reset();
}
void Adc16bit_AD7606::Convert(void)
{
    ControlPin::SetMeHigh(CONVA);  // ch1~2
    ControlPin::SetMeHigh(CONVB);	//ch3~4
    TIMERSFunc::DELAY_SYS(100, 0);
    ControlPin::SetMeLow(CONVA);
    ControlPin::SetMeLow(CONVB);
    TIMERSFunc::DELAY_SYS(100, 0);
    ControlPin::SetMeHigh(CONVA);
    ControlPin::SetMeHigh(CONVB);
}

// 同为读取数据，本程序不等待和处理芯片忙时信号。
void Adc16bit_AD7606::Read(unsigned long *Data1,unsigned long *Data2)
{
    volatile unsigned long Value;

    *Data1 = 0;
    *Data2 = 0;
    *Data1 = * (volatile unsigned short  *)(Address);   //Hardware designer is so clever that we do not need to Setting CS or Readdata Pin.by HPQ
    *Data2 = * (volatile unsigned short  *)(Address);
    Value  = * (volatile unsigned short  *)(Address);
    Value  = * (volatile unsigned short  *)(Address);
    *Data1 <<= 8;
    *Data2 <<= 8;
    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        *Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
        *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
    }
}

// 同为读取数据，本程序需要等待和处理芯片忙时信号。
void Adc16bit_AD7606::ReadAfterBusy(unsigned long *Data1,unsigned long *Data2)
{
    unsigned long uw32_timeout = 0;
    volatile unsigned long Value;

    *Data1 = 0;
    *Data2 = 0;
    while (ControlPin::ReadMe(BUSY))
    {
        uw32_timeout++;
        //if(uw32_timeout > 0xFFFFFF)
        if(uw32_timeout > 0X1D4C0) //0xFFFFFF)    // 10ms  12000*10
            return;
    }
    *Data1 = * (volatile unsigned short  *)(Address);   //Hardware designer is so clever that we do not need to Setting CS or Readdata Pin.by HPQ
    *Data2 = * (volatile unsigned short  *)(Address);
    Value  = * (volatile unsigned short  *)(Address);
    Value  = * (volatile unsigned short  *)(Address);
    *Data1 <<= 8;
    *Data2 <<= 8;
    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        *Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
        *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
    }
}
void           Adc16bit_AD7606::ReadAfterBusy4Ch (unsigned long *Data1,unsigned long *Data2,unsigned long *Data3,unsigned long *Data4)
{
    unsigned long uw32_timeout = 0;
    //unsigned long Value;

    *Data1 = 0;
    *Data2 = 0;
    *Data3 = 0;
    *Data4 = 0;
    while (ControlPin::ReadMe(BUSY))
    {
        uw32_timeout++;
        if(uw32_timeout > 0xFFFFFF)
            return;
    }
    *Data1 = * (volatile unsigned short  *)(Address);   //Hardware designer is so clever that we do not need to Setting CS or Readdata Pin.by HPQ
    *Data2 = * (volatile unsigned short  *)(Address);
    *Data3 = * (volatile unsigned short  *)(Address);
    *Data4 = * (volatile unsigned short  *)(Address);
    *Data1 <<= 8;
    *Data2 <<= 8;
    *Data3 <<= 8;
    *Data4 <<= 8;
    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        *Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
        *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
        *Data3  = CommonFunc::Bipolar2Unipolar24BitCode(*Data3);
        *Data4  = CommonFunc::Bipolar2Unipolar24BitCode(*Data4);
    }
}
void Adc16bit_AD7606::Reset(void)
{
    ControlPin::SetMeLow(Rst);
    TIMERSFunc::DELAY_SYS(100, 0);
    ControlPin::SetMeHigh(Rst);
    TIMERSFunc::DELAY_SYS(100, 0);
    ControlPin::SetMeLow(Rst);
}




/*********************************************************************************
**                            End Of File
*********************************************************************************/
