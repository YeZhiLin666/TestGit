///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_CPS.h
//Version:		V1.0
//Date:				2018/08/06
//Description:
//
//Others:      dirui
//History:

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../COMMON_os/Ext_Devices/ControlPin.h"

enum
{
    SYSCTRL_DI_FANFEEDBACK			=(1<<0),
    SYSCTRL_DI_24VSTSTUS				=(1<<1),
    SYSCTRL_DI_POFF							=(1<<4),
    SYSCTRL_DI_RESERVE1					=(1<<5),
    SYSCTRL_DI_RESERVE2					=(1<<10),
    SYSCTRL_DI_BUTTON						=(1<<14),
    SYSCTRL_DI_EMERGENCYSTOP		=(1<<15),


};
enum
{
    SYSCTRL_DO_FANPOWER				  =(1<<0),
    SYSCTRL_DO_24VPOWER					=(1<<1),
    SYSCTRL_DO_PWM380           =(1<<2),
    SYSCTRL_DO_PON							=(1<<4),
    SYSCTRL_DO_RESERVE					=(1<<10),
    SYSCTRL_DO_POFF							=(1<<11),
    SYSCTRL_DO_ALARMRED					=(1<<12),
    SYSCTRL_DO_ALARMYELLOW			=(1<<13),
    SYSCTRL_DO_ALARMGREEN				=(1<<14),
    SYSCTRL_DO_ALARMBEEP				=(1<<15),

};
class Aux_SystemCtrlBoard_415016_Base
{

public:
    static unsigned short m_GetDICode;
    static unsigned short m_SetDOCode;
    static unsigned short m_StartDelayTime;
    static unsigned short m_StopDelayTime;
public:
    static void Func_Init(void);
public:
    static void Hardware_Init(unsigned char BoardID);
    static void Software_Init(unsigned char BoardID);
    static void IO_Init(void);

    static void Init_DI(void);
    static void Init_DO(void);
    static void Read_DI(void);
    static void Write_DO(void);



};
