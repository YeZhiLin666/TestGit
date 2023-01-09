#pragma once

//#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"
#include "../../COMMON_OS/Ext_Devices/ControlPin.h"

//  Exterior Check Safe Class

class excheck
{
public:

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    static char M0ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type);
//#endif

//#if(FAN_SAFETY_MODE==1)
    static char ExFanCheckSafety(St_Port MyPort);
//#endif

};


