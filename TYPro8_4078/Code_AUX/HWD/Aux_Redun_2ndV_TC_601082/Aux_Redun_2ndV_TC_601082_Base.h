///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_Redun_2ndV_TC_601082_BASE.h
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

class Aux_Redun_2ndV_TC_601082_Base
{
public:
    static SSPI_Para Aux_Redun_2ndV_TC_601082_SSP0ADC;
    static SSPI_Para Aux_Redun_2ndV_TC_601082_SSP1ADC;

    // 	  static  St_Port Pin_SCK0;
    // 		static  St_Port Pin_MISO0;
    // 		static  St_Port Pin_MOSI0;
    //
    // 		static  St_Port Pin_SCK1;
    // 		static  St_Port Pin_MISO1;
    // 		static  St_Port Pin_MOSI1;
    //
    // 		static  St_Port Pin_ECODE1;
    // 		static  St_Port Pin_ECODE2;
    // 		static  St_Port Pin_ECODE3;
    // 		static  St_Port Pin_EOE;
    //     static  St_Port Pin_ADCBUSY_V;
    static  const St_Port Pin_ADCBUSY_T;
    static  unsigned long m_Inchip_temperature;   //片内温度传感器的码值


private:
    static unsigned long error_counter;

    static void ADC_Init();
    static void Func_Init(unsigned long BoardID);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);


public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(unsigned char spiChn);
    static unsigned long Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_V_Read(unsigned char u_CSid,unsigned char ADCchn);

    static unsigned long Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn);


};
