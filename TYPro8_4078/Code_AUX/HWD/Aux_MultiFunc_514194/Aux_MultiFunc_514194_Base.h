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
#define MAX_DO_COUNT   4

class Aux_MultiFunc_514194_Base
{
public:
    static SSPI_Para Aux_MultiFunc_514194_SSP1ADC;
    static St_DIDOMapMgr  m_DIDO_MapMgr[MAX_DO_COUNT];


private:
    static long data[14];
    static void ADC_Init(void);
    static void Func_Init(void);
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
    static void Init_DI(void);
    static void Init_DO(void);
    static void Read_DI(void);
    static void Write_DO(void);
    static unsigned long Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_Read(unsigned char u_CSid,unsigned char ADCchn);

private:
    static const St_Port Pin_DI_1;
    static const St_Port Pin_DI_2;
    static const St_Port Pin_DI_3;
    static const St_Port Pin_DI_4;
    static const St_Port Pin_DO_1;
    static const St_Port Pin_DO_2;
    static const St_Port Pin_DO_3;
    static const St_Port Pin_DO_4;
private:
    static unsigned char My_BoardID;
};
