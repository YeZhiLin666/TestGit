///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						~{8(VzEdVCO`9X2YWwM7ND<~~}
//Others:      dirui
//History~{#:~}

///////////////////////////////////////////////////////////////////////////////////
#ifndef _AUX_BASE_
#define _AUX_BASE_

#include "../../COMMON_os/Ext_Devices/ControlPin.h"


//////K~{O5J}~}
// #define   Ec0_Ktype	 1.7600413686e1f      
// #define   Ec1_Ktype	 3.8921204975e1f
// #define   Ec2_Ktype	 1.8558770032e-2f
// #define   Ec3_Ktype	 -9.9457592874e-5f
// #define   Ec4_Ktype	 3.1840945719e-7f
// #define   Ec5_Ktype	 -5.6072844889e-10f
// #define   Ec6_Ktype	 5.6075059059e-13f
// #define   Ec7_Ktype	 -3.2020720003e-16f
// #define   Ec8_Ktype	 9.7151147152e-20f
// #define   Ec9_Ktype	 -1.2104721275e-23f
// #define   Ea0_Ktype	 1.185976e2f
// #define   Ea1_Ktype	 -1.183432e-4f

const float   Ec0_Ktype	= 1.7600413686e1f;
const float   Ec1_Ktype	= 3.8921204975e1f;
const float   Ec2_Ktype	= 1.8558770032e-2f;
const float   Ec3_Ktype	= -9.9457592874e-5f;
const float   Ec4_Ktype	= 3.1840945719e-7f;
const float   Ec5_Ktype	= -5.6072844889e-10f;
const float   Ec6_Ktype	= 5.6075059059e-13f;
const float   Ec7_Ktype	= -3.2020720003e-16f;
const float   Ec8_Ktype	= 9.7151147152e-20f;
const float   Ec9_Ktype	= -1.2104721275e-23f;
const float   Ea0_Ktype	= 1.185976e2f;
const float   Ea1_Ktype	= -1.183432e-4f;
//////T~{O5J}~}
#define   Ec0_Ttype	0.0f    
#define   Ec1_Ttype	3.8748106364e1f
#define   Ec2_Ttype	3.329222788e-2f
#define   Ec3_Ttype	2.0618243404e-4f
#define   Ec4_Ttype	-2.1882256846e-6f
#define   Ec5_Ttype	1.0996880928e-8f
#define   Ec6_Ttype	-3.0815758772e-11f
#define   Ec7_Ttype	4.547913529e-14f
#define   Ec8_Ttype	-2.7512901673e-17f


enum TE_CHECK_T_VOLTAGERANG
{                        
	COUNT_TEMPERATURE = 0, 
	CHECK_T_VOLTAGERANG ,      
};


class AUX_Base
{
private:

	static const float f_TCoefficientA_Ktype[9];
	static const float f_tcA_Ktype[9];
	static const float f_TCoefficientB_Ktype[10];
	static const float f_tcB_Ktype[10];
	static const float f_TCoefficientC_Ktype[7];
	static const float f_tcC_Ktype[7];

	static const float f_TCoefficientA_Ttype[8];
	static const float f_tcA_Ttype[8];
	static const float f_TCoefficientB_Ttype[7];
	static const float f_tcB_Ttype[7];


	//每个Bid预留一个
	static float t90_SampleEx_KType[8];
	static float t90_SampleEx_TType[8];

public:
	static const St_Port Pin_SCK0;
	static const St_Port Pin_MISO0;
	static const St_Port Pin_MOSI0;

	static void Hardware_Init(void);
	static float T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag);
	static float T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag);
	static void T_t90_KType_SampleEx(unsigned char Bid,float f_Value);
	static void T_t90_TType_SampleEx(unsigned char Bid,float f_Value);
	static float get_SampleEx(unsigned char Bid,unsigned char type);

	static unsigned char getBID(void);
};	
#endif
