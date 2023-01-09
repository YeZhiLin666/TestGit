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
// #define MAX_DO_COUNT   8

class Aux_DDS_ACIM_ACR_408888_Base
{
public:
    static SSPI_Para Aux_DDS_ACIM_ACR_408888_SSP0ADC;
    static SSPI_Para Aux_DDS_ACIM_ACR_408888_SSP1ADC;

//ADC
    static const 	St_Port Pin_ADC_CONV;
    static const 	St_Port Pin_ADC_DRDY1;
    static const 	St_Port Pin_ADC_DRDY2;
    static const 	St_Port Pin_ADC_CS1;
    static const 	St_Port Pin_ADC_CS2;

    static const  St_Port Pin_I_VS_RMS;
    static const  St_Port Pin_SETS;
    static const  St_Port Pin_ACIM_I;
    static const  St_Port Pin_ACIM_V;
//AD8253
    static const  St_Port Pin_VA0;
    static const  St_Port Pin_VA1;
    static const  St_Port Pin_ARW;
    static unsigned char Amplitude_Flag;    //正弦波幅值调节方式，  01： AD5160    02：DAC 调节方式
    static const St_Port Pin_None;
		static const St_Port Pin_DAC_BIAS_SET; // Only for 15A ACIM board
		static const St_Port Pin_PROT;	// Only for 15A ACIM board

    static int AD8253_VRead_Gain;
    static bool isACIM;	// ACR,ACIR board: false;	ACIM board: true
		static bool isCurrentSoruce;	// when 15A ACIM board used as sin current source, set this to 1
    //DDS   //01/27/2018
    static const 	St_Port Pin_DDS_CS;  //01/27/2018

private:
// 	static long data[2][8];
// 	static void ADC_Init(void);
//     static void DAC_Init(void);
    static void Func_Init(void);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    // 		static float CaliChannel(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID,unsigned long Code);
    static float cali2ndV(unsigned char BoardID,unsigned char ChannelID,unsigned long Code);
    static unsigned char  caliIndex(unsigned char BoardID,unsigned char ChannelID);
public:
    static void DAC_Init(void);
    static void ADC_Init(void);
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);
    static void SPI_SET(unsigned char spiChn);
    static void DDS_SPI_Init(void);
    static void AD5160_SPI_Init(void);
    static void Write_AD8253(unsigned short Gain);
    static void Write_AD5160(float data);
// 	static void Init_DI(void);
// 	static void Init_DO(void);
// 	static void Read_DI(void);
// 	static void Write_DO(void);
    static void Dac_out(unsigned char chn,unsigned long value);
    static unsigned long Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn);
    static unsigned long Aux_Read(unsigned char u_CSid,unsigned char ADCchn);

private:
    static const 	St_Port Pin_DacBIN2COMP;
    static const 	St_Port Pin_DacCS;
    static const 	St_Port Pin_DacLoad;
    static const 	St_Port Pin_DacCLR;  //DAC

    //static const 	St_Port Pin_DDS_CS;
    static const 	St_Port Pin_DDS_CS6;  //
    static const 	St_Port Pin_DDS_SLEEP;


private:
    static unsigned char My_BoardID;
};
