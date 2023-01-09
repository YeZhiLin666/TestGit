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

class Aux_DIDO_601196_WMADC602518_Base
{
public:
    static St_DIDOMapMgr  m_DIDO_MapMgr[MAX_DO_COUNT];


private:

    static void Func_Init(void);

public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);

    static void Init_DI(void);
    static void Init_DO(void);
    static void Read_DI(void);
    static void Write_DO(void);


private:
// 		static const St_Port Pin_DI_1;
// 		static const St_Port Pin_DI_2;
// 		static const St_Port Pin_DI_3;
// 		static const St_Port Pin_DI_4;
// 		static const St_Port Pin_DO_1;
// 		static const St_Port Pin_DO_2;
// 		static const St_Port Pin_DO_3;
// 		static const St_Port Pin_DO_4;
private:
    static unsigned char My_BoardID;
    static unsigned short DIDO_Value;
};
