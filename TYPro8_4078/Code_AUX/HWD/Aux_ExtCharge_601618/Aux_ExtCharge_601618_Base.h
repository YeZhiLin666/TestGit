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

class Aux_ExtCharge_601618_Base
{
public:
    static SSPI_Para Aux_ExtCharge_601618_SSP1ADC;
    static SSPI_Para Aux_ExtCharge_601618_SSP0ADC;

    static St_DOMapMgr  m_ExChargeDO_MapMgr[8];
    static MsTime       StartTime;
    static MsTime       TimePassed;
    static unsigned char  SafetyStatus;   //0:上电默认状态，安全， 1：引脚出现低电平，开始进入不安全状态  2：状态稳定，确定为不安全状态。

    static  const St_Port Pin_ADCBUSY_T;

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
    static void SPI_SET(unsigned char spiChn);
    // 	  static void Init_DI(void);
    // 	  static void Read_DI(void);
    static unsigned long Aux_Trigger(unsigned char u_ADCid,unsigned char ADCchn);
    static unsigned long Aux_Read(unsigned char u_ADCid,unsigned char ADCchn);
    static void Set_ExtCharge_Relay(unsigned int value, unsigned char uc_chn);
    static char EmergencyStop_Check(void);
    static void EmergencyStop_Set(unsigned char value);
    static unsigned long Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn);


private:
    static const St_Port Pin_DI_1;
    // 		static const St_Port Pin_DI_2;
    // 		static const St_Port Pin_DI_3;
    // 		static const St_Port Pin_DI_4;
    static const St_Port Pin_DO_1;
    static const St_Port Pin_DO_2;
    static const St_Port Pin_DO_3;
    static const St_Port Pin_DO_4;
    static const St_Port Pin_DO_5;
    static const St_Port Pin_DO_6;
    static const St_Port Pin_DO_7;
    static const St_Port Pin_DO_8;
    static const St_Port Pin_KP;
private:
    static unsigned char My_BoardID;
};
