#pragma  once
#include "../AUX_Tasks/Aux_SafeCheck.h"

class AuxRedundantSafetyCheck
{
public:
    static  UInt16        Priority ;
    static  UInt16        TaskID ;
    static void Init(void);
    static void DoWork(void);
public:
    static St_M4SafetyCeck st_RedundantSafetyCheck[M4_MAX_CHANNELNO];
private:
    static signed char get_M4ChnNo(unsigned char IV_ChnIdx);
    static void ReportUnsafe(unsigned char IV_ChnNo,float f_unsafeVal);
};
