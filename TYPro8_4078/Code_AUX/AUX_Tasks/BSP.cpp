#include "includes.h"

/*****************BoardConfig Class******************************************
** Start:         BoardConfig Class
******************************************************************************/
My_Func_Ptr_Void    board_config::FeedWatchDog;
My_Func_Ptr_Ext			board_config::Func_AUX_Sample[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];
My_Func_Ptr          board_config::Func_AuxSample;
My_Func_Ptr_Ext3         board_config::Func_AUX_Dac_Out;
char board_config::AUX_SampleProcessManage(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
    char temp = 0;
    temp=( *board_config::Func_AUX_Sample[BoardID][ADCID][ChannelID])(BoardID,ADCID,ChannelID);
    return 1;
}

/*****************BoardConfig Class******************************************
** End:           BoardConfig Class
******************************************************************************/

