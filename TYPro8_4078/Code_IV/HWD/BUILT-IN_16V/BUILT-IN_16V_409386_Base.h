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

class BuiltIn_16V_409386_Base
{
public:
    static SSPI_Para BuiltIn_16V_409386_SSP0ADC;
//   static SSPI_Para BuiltIn_16T_409932_SSP0ADC;
private:
    static const St_Port Pin_START;
    static const St_Port Pin_DRDY_M;
//   static const St_Port Pin_DRDY_S;
    static unsigned long RestADC;


private:
    static void ADC_Init(unsigned char u_CSid);
// 	static void Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);

    // 		static float CaliChannel(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID,unsigned long Code);
// 	static float cali2ndV(unsigned char BoardID,unsigned char ChannelID,unsigned long Code);
    static unsigned char  caliIndex(unsigned char BoardID,unsigned char ChannelID);
public:
    static void Hardware_Init(unsigned char BoardID);
// 	static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(void);

    static void BuiltIn_2ndV_ADG_Open(unsigned char u_CSid,unsigned char ChannelIdx);
    static unsigned long BuiltIn_2ndV_Trigger(unsigned char u_ADCid,unsigned char ADCchn);
    static unsigned long BuiltIn_2ndV_Read(unsigned char u_ADCid,unsigned char ADCchn);

//   static unsigned long BuiltIn_TC_Trigger(unsigned char u_ADCid,unsigned char ADCchn);
// 	static unsigned long BuiltIn_TC_Read(unsigned char u_ADCid,unsigned char ADCchn);


};
