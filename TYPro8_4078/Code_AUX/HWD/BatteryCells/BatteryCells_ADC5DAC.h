///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_ADC5DAC.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						����������������ͷ�ļ�
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////
#pragma once
class BatteryCells_ADC5DAC
{
public:
    static void          Adc_Init(void);
    static char  ADC_rd(unsigned long BoardID,unsigned char ADCID,unsigned long ChannelID);

};
