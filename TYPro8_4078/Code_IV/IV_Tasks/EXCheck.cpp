#include "includes.h"


/*****************Exterior Check Safe Class******************************************
** Start:         Exterior Check Safe Class
******************************************************************************/

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
char excheck::M0ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
    M0RedundantFunc::ReleaseSafetyCheck(uc_ChannelNo,(unsigned char)type);
    safety_check::ReleaseRedundantSafetyFlgCheck(uc_ChannelNo);
    safety_check::RedundantSafetyFlg[uc_ChannelNo]=0;
    M0RedundantFunc::m_bClampValid[uc_ChannelNo] = false;
    return 0;
}
//#endif

//#if(FAN_SAFETY_MODE==1)
char excheck::ExFanCheckSafety(St_Port MyPort)
{
    return ((ControlPin::ReadMe(MyPort))== 0? 1: 0);
}
//#endif

/*****************Exterior Check Safe Class******************************************
** End:           Exterior Check Safe Class
******************************************************************************/

