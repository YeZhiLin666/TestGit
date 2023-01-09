
#pragma once
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../common_os/protocols/SF_Hwf.h"
#include "../../COMMON_os/Ext_Devices/ADC101C027.h"
typedef struct
{
	ADC101C027_TypeDef Vpp_Vol;
	ADC101C027_TypeDef Vpp_Cur;
	ADC101C027_TypeDef Vmm_Vol;
	ADC101C027_TypeDef Vmm_Cur;
}POWER_2US6K_IV_TypeDef;

enum
{
	FanSafe = 0,
  FanUnsafe = 1,
};
enum
{
	VppSingleMode = 0,
  VmmSingleMode,
	VppVmmMultiMode,  
};
class Aux_PowerADJ_2US6K_422108_Base
{

public:
	static const St_Port Pin_ERR1; 				//STOP
	static const St_Port Pin_ERR2; 
	static const St_Port Pin_RST; 
	static const St_Port Pin_OFF; 
	static const St_Port Pin_FANALM;
	static const St_Port Pin_FANPWM; 
	static const St_Port Pin_PFCOK; 
	static const St_Port Pin_ERRP;
	static const St_Port Pin_RunLed;
  static const St_Port Pin_Mode0;
  static const St_Port Pin_Mode1;
  static const St_Port Pin_Mode2;
  static const St_Port Pin_Mode3;
	
  static MsTime EngineDelayTime;
	static unsigned char PowerStatus;
  static unsigned char PowerRunMode_Vpp;
  static unsigned char PowerRunMode_Vmm;
	static POWER_2US6K_IV_TypeDef Power_2US6K_IV;
	static unsigned char Power_ErrCnt;
	static unsigned char Power_DacRampStatus;
  static unsigned char ErrPinRiseNum;
	static unsigned char ErrPinFallNum;
	static unsigned char PowerConfigMode;

public:	
	  static void Hardware_Init(unsigned char BoardID);
	  static void Software_Init(unsigned char BoardID);
	  static void IO_Init(void);
		static unsigned char OpenProcess600A(void);		
		static void ADC_Init(void);
		static void SetDacOut2Zero(void);
		static bool Get_ErrPinStatus(void);
		static void Aux_Power_dac_out(unsigned char chan,float Vol_Vset);
    static void DAC_SET(unsigned char chan,unsigned long data);
    static void Aux_Read(unsigned char ADCchn);
    static void Aux_I2C_Sample(void);
    static bool CurrentSafetyCheck(void);
	  static void Init_DI(void);
	  static void Init_DO(void);
	  static void Read_DI(void);
	  static void Write_DO(void);
    static unsigned char FanSafetyPinCheck(void);
    static void Fan_Safety_Check(void);
    static void Fan_PWM_Adjust(void);
};
