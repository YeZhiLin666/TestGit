///////////////////////////////////////////////////////////////////////////////////
//File Name: BATTERYCELL_BASE.h
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
#define AUX_BC_ModuleCnt	4
#define AUX_BC_MAXCHIP        8
#define AUX_BC_MAXCHANNELPERCHIP   12
enum
{
    SLEEP,
    CONVERT,
    POLL_0,
    POLL_1,
    POLL_2,
    POLL_3,
    READ,
};

typedef struct
{
    unsigned char m_BatteryCellsChipID;
    unsigned char m_BatteryCellsChannelNum;
    unsigned char m_BoardID;
    unsigned char m_AuxChannelNum;
} ST_MAPBATTODAT;

class BatteryCells_Base
{
public:
    static SSPI_Para BatteryCells_SSP1ADC;
    static unsigned long BC_ReadData[AUX_BC_MAXCHIP][AUX_BC_MAXCHANNELPERCHIP];
    static unsigned char m_ADC_ChipAddr[8];
    static unsigned char m_ADC_MaxCnt[8];
    static unsigned char config_data[AUX_BC_ModuleCnt][6];
    static ST_MAPBATTODAT m_MapBattToDat[128];
    static unsigned char beUsedCHannel;
    static unsigned int  Sum_BatteryCellChannel;
    static const St_Port Pin_SCK1;
    static const St_Port Pin_MISO1;
    static const St_Port Pin_MOSI1;

    static const St_Port Pin_ECODE1;
    static const St_Port Pin_ECODE2;
    static const St_Port Pin_ECODE3;
    static const St_Port Pin_EOE;

private:
    static void ADC_Init(unsigned char BoardID,unsigned long ADCID);
    static void Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
    static char ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);

public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(unsigned char BoardID);

    static unsigned char	ReleaselConfig(unsigned char BoardID);


};
