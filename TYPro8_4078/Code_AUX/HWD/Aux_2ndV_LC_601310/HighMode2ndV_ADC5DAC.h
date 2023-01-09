///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_ADC5DAC.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样驱动处理头文件
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once
class HighMode2ndV_ADC5DAC
{
public:
    static void          Adc_Init(void);
    static char  ADC_rd(unsigned long CH_Number,unsigned long type);

};
