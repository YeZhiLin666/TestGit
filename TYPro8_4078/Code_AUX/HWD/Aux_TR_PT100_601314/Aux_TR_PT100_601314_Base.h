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

class Aux_TR_PT100_601314_Base
{
public:
    static SSPI_Para Aux_TR_PT100_601314_SSP1ADC;

private:
    static void ADC_Init(unsigned char BoardID,unsigned long ADCID);
    static void Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    // 		static float CaliChannel(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID,unsigned long Code);
    static float cali2ndV(unsigned char BoardID,unsigned char ChannelID,unsigned long Code);
    static unsigned char  caliIndex(unsigned char BoardID,unsigned char ChannelID);
public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(void);

    static unsigned long Aux_V_Trigger(unsigned char u_ADCid,unsigned char ADCchn);
    static unsigned long Aux_V_Read(unsigned char u_ADCid,unsigned char ADCchn);


};
