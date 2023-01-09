#pragma once

#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../common_os/protocols/SF_Hwf.h"


// #define VSET_VOL_MIN						-100.0
// #define VSET_VOL_MAX						100.0
// #define DAC_REF_VOL							3.3
#define I2C_Power               I2C1

#define DAC_ADDR                            0X98

class Aux_PowerADJ_200A_412206_Base
{
public:

// 	static UWord16       uw16_LedTimer;
//   static float Vpp_Set;
//   static float Vmm_Set;
//     static unsigned char HV_OK;

//    static const St_Port HV_OK1;
//    static const St_Port HV_OK2;
    static const St_Port SW_TR;
    static unsigned short  TimeTicks;
    static MsTime EngineDelayTime;
    static unsigned char PowerStatus;
//  private:
// 	static MsTime LastdelayTime1;

public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
// 	static void IO_Init(void);
//  static void DataSampleProcess200A (unsigned char chan);
    static unsigned char OpenProcess200A(void);
//   static void Aux_Power_adc_Read(char ADCchn);
    static void DAC6571_SET(unsigned char chan,unsigned long data);
    static void Aux_Power_dac_out(unsigned char chan,float Vol_Vset);
    static unsigned long Aux_Read(unsigned char u_CSid, unsigned char ADCchn);
    static unsigned long Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn);

};
