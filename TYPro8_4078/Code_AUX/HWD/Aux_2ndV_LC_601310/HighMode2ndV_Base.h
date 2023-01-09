///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						~{8(VzEdVCO`9X2YWwM7ND<~~}
//Others:      dirui
//History~{#:~}

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
class HighMode2ndV_Base
{
public:
    static SSPI_Para HighMode2ndV_SSP1ADC;
    static const St_Port Pin_SCK1;
    static const St_Port Pin_MISO1;
    static const St_Port Pin_MOSI1;
    static const St_Port Pin_ECODE1;
    static const St_Port Pin_ECODE2;
    static const St_Port Pin_ECODE3;
    static const St_Port Pin_EOE;
    static const St_Port Pin_ADCBUSY;
    static const St_Port Pin_Cover;
    static const St_Port Pin_CS;

private:
    static void ADC_Init(unsigned char BoardID);
    static void Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static float CaliChannel(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID,unsigned long Code);
    static float cali2ndV(unsigned char BoardID,unsigned char ChannelID,unsigned long Code);
    static unsigned char  caliIndex(unsigned char BoardID,unsigned char ChannelID);
public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(unsigned char BoardID);


};
