///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//
//Others:      dirui
//History:

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"

class Aux_2ndV_LC_601310_Base
{
public:
    static SSPI_Para Aux_2ndV_LC_601310_SSP1ADC;
    static const St_Port Pin_Conver;
    static const St_Port Pin_MUXWR;
    static const St_Port Pin_VAD0;
    static const St_Port Pin_VAD1;
    static const St_Port Pin_VAD2;
    static const St_Port Pin_VAD3;


private:
    static void ADC_Init(unsigned char BoardID);
    static void Func_Init(void);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    // 		static float CaliChannel(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID,unsigned long Code);
    static float cali2ndV(unsigned char BoardID,unsigned char ChannelID,unsigned long Code);
    static unsigned char  caliIndex(unsigned char BoardID,unsigned char ChannelID);
    static unsigned short  WaitForSampleCnt;
public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(void);

    static void Aux_V_Trigger(void);
    static void Aux_V_Read(unsigned char u_CSid,unsigned char ADCchn);
    static void Triger_Protection(void);
    static void Switch_Channel(unsigned char channel);


};
