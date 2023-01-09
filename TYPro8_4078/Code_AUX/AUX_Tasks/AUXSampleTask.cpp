#include "../Entry/includes.h"
UInt16            AUX_Sample_4_task::Priority=PRIORITY4;
UInt16            AUX_Sample_4_task::TaskID=AUX_SAMPLE_4_TASK;
void AUX_Sample_4_task::Init(void)
{
    return;
}
void AUX_Sample_4_task::DoWork(void)
{
    for(unsigned char uchar_i=0; uchar_i<8; uchar_i++) //´ýÓÅ»¯
    {
        for(unsigned char uchar_j=0; uchar_j<16; uchar_j++)
        {
            if(Aux_DataManager::ADC_SampleProcessType[uchar_i][0][uchar_j]==SAMPLETASK)
                board_config::AUX_SampleProcessManage(uchar_i,0,uchar_j);
            else
                continue;
        }
    }

}
void AUX_Sample_4_task::EndTask(void)
{
    return;
}
