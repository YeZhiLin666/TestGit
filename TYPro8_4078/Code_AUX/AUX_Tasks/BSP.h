#pragma once

#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../../COMMON_OS/Ext_Devices/ControlPin.h"
#include "AUXDataDefine.h"

class board_config
{
public:
    static    My_Func_Ptr_Void    FeedWatchDog;
    static    My_Func_Ptr_Ext     Func_AUX_Sample[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];
    static    My_Func_Ptr_Ext3    Func_AUX_Dac_Out;
    static    My_Func_Ptr         Func_AuxSample;
    static char AUX_SampleProcessManage(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
};


