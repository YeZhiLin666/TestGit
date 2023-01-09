#include "includes.h"

unsigned char  Adc16bit_AD7656::CodeType;

St_Port        Adc16bit_AD7656::Pin_ConverA;
St_Port        Adc16bit_AD7656::Pin_ConverB;
St_Port        Adc16bit_AD7656::Pin_ConverC;
St_Port        Adc16bit_AD7656::Pin_Busy;
St_Port        Adc16bit_AD7656::Pin_Rst;
St_Port        Adc16bit_AD7656::Pin_CS;

unsigned long  Adc16bit_AD7656::Address;

void Adc16bit_AD7656::Init(unsigned char ConvCodeType)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
    case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }

    ControlPin::SetMeHigh(Pin_CS);
    ControlPin::SetMeLow(Pin_ConverA);
    ControlPin::SetMeLow(Pin_ConverB);
    ControlPin::SetMeLow(Pin_ConverC);

    Reset();
}

// 同为读取数据，本程序需要等待和处理芯片忙时信号。
// A,B,C 应整合为一个函数，入口多一参数
void Adc16bit_AD7656::ReadA(unsigned long *Data1,unsigned long *Data2)
{
    unsigned long uw32_timeout = 0;
    ControlPin::SetMeHigh(Pin_ConverA);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeHigh(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_ConverA);
    TIMERSFunc::DELAY_SYS(5, 0);
    // 	Reset();
    ControlPin::SetMeHigh(Pin_ConverA);
    while(ControlPin::ReadMe(Pin_Busy))
    {
        if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
        {
            *Data1 = 0;
            *Data2 =  * (volatile unsigned short  *)(Address);
            *Data2 =  * (volatile unsigned short  *)(Address);
            *Data2 <<= 8;
            if(CodeType == eCONVCODETYPE_BIPOLAR)
            {

                *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
            }

            ControlPin::SetMeLow(Pin_ConverA);
            return;
        }
    }
    *Data1 = * (volatile unsigned short  *)(Address);
    *Data2 = * (volatile unsigned short  *)(Address);

    *Data1 <<= 8;
    *Data2 <<= 8;
    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        *Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
        *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
    }

    ControlPin::SetMeLow(Pin_ConverA);
}
void Adc16bit_AD7656::ReadB(unsigned long *Data1,unsigned long *Data2)
{
		unsigned long uw32_timeout = 0;

	  ControlPin::SetMeHigh(Pin_ConverB);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeHigh(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_ConverB);
    TIMERSFunc::DELAY_SYS(5, 0);
	
	ControlPin::SetMeHigh(Pin_ConverB);
	while(ControlPin::ReadMe(Pin_Busy))
	{
		if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
		{
			*Data1 = 0;
			*Data2 = 0;
			return;
		}
	}
	*Data1 = * (volatile unsigned short  *)(Address);
	*Data2 = * (volatile unsigned short  *)(Address);

	*Data1 <<= 8;
	*Data2 <<= 8;
	if(CodeType == eCONVCODETYPE_BIPOLAR)
	{
		*Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
		*Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
	}

	ControlPin::SetMeLow(Pin_ConverB);
}

void Adc16bit_AD7656::ReadC(unsigned long *Data1,unsigned long *Data2)
{
    unsigned long uw32_timeout = 0;
    ControlPin::SetMeHigh(Pin_ConverC);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeHigh(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_Rst);
    TIMERSFunc::DELAY_SYS(5, 0);
    ControlPin::SetMeLow(Pin_ConverC);
    TIMERSFunc::DELAY_SYS(5, 0);

    ControlPin::SetMeHigh(Pin_ConverC);
    while(ControlPin::ReadMe(Pin_Busy))
    {
        if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
        {
            *Data1 = 0;
            *Data2 = 0;
            return;
        }
    }
    *Data1 = * (volatile unsigned short  *)(Address);
    *Data2 = * (volatile unsigned short  *)(Address);

    *Data1 <<= 8;
    *Data2 <<= 8;
    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        *Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
        *Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
    }

    ControlPin::SetMeLow(Pin_ConverC);
}

// void Adc16bit_AD7656::SetAllConvert(void)
// {
// 	ControlPin::SetMeHigh(Pin_ConverA);
// 	ControlPin::SetMeHigh(Pin_ConverB);
// 	ControlPin::SetMeHigh(Pin_ConverC);
// }
// void Adc16bit_AD7656::ClrAllConvert(void)
// {
// 	ControlPin::SetMeLow(Pin_ConverA);
// 	ControlPin::SetMeLow(Pin_ConverB);
// 	ControlPin::SetMeLow(Pin_ConverC);
// }

// A,B,C 应整合为一个函数，入口多一参数
// void Adc16bit_AD7656::ConverA(void)
// {
// 	ControlPin::SetMeHigh(Pin_ConverA);
// 	TIMERSFunc::DELAY_SYS(200, 0);
// 	ControlPin::SetMeLow(Pin_ConverA);
// }
// void Adc16bit_AD7656::ConverB(void)
// {
// 	ControlPin::SetMeHigh(Pin_ConverB);
// 	TIMERSFunc::DELAY_SYS(200, 0);
// 	ControlPin::SetMeLow(Pin_ConverB);
// }
// void Adc16bit_AD7656::ConverC(void)
// {
// 	ControlPin::SetMeHigh(Pin_ConverC);
// 	TIMERSFunc::DELAY_SYS(200, 0);
// 	ControlPin::SetMeLow(Pin_ConverC);
// }

// 同为读取数据，本程序不等待和处理芯片忙时信号。
// void Adc16bit_AD7656::ReadData(unsigned long *Data1,unsigned long *Data2)
// {
// 	*Data1 = * (volatile unsigned short  *)(Address);
// 	*Data2 = * (volatile unsigned short  *)(Address);

// 	*Data1 <<= 8;
// 	*Data2 <<= 8;
// 	if(CodeType == eCONVCODETYPE_BIPOLAR)
// 	{
// 		*Data1  = CommonFunc::Bipolar2Unipolar24BitCode(*Data1);
// 		*Data2  = CommonFunc::Bipolar2Unipolar24BitCode(*Data2);
// 	}
// }

void Adc16bit_AD7656::Reset(void)
{
    ControlPin::SetMeHigh(Pin_Rst);
    TIMERSFunc::DELAY_SYS(200, 0);
    ControlPin::SetMeLow(Pin_Rst);
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
