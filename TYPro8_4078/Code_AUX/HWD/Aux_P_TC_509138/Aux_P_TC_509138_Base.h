///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_P_TC_509138_Base.h
//Version:		V2.0
//Date:				2015/04/30
//Description:
//
//Others:      qjm
//History:

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"

class Aux_P_TC_509138_Base
{
public:
    static SSPI_Para Aux_P_TC_509138_SSP0ADC;
    //	static SSPI_Para Aux_P&TC_509138_SSP1ADC;


    static  unsigned long m_Inchip_temperature;   //片内温度传感器的码值


private:
    static unsigned long error_counter;

    static void ADC_Init(unsigned char BoardID);
    static void Func_Init(unsigned char BoardID);
    static char ADC_rd(unsigned char BoardID,unsigned char ADCID,unsigned char ChannelID);
    static char ADC_Cs(unsigned char BoardID,unsigned char ADCID,unsigned char ChannelID);


public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(void); // To set the SPI for this board

    static unsigned long Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn);


};
