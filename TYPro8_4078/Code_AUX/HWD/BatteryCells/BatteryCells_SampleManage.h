///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_SampleManage.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样相关处理头文件
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once
// #include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
// #include "../../common_os/protocols/SF_Hwf.h"
#include "../Aux_Tasks/AuxDataDefine.h"
// #include "../../COMMON_os/LPC4078CORE/SSPI.h"

#define MAXSAMPLETIME     2000

class BatteryCells_SampleManage
{
private:
    static st_Aux_Sample 	st_aux_sample_setting[MAX_BOARD_PER_AUXMCU];
    static unsigned long Sample_conter[MAX_BOARD_PER_AUXMCU];
    static MsTime RecordTime;
    static MsTime LastTime;
    static MsTime TimeT(MsTime a,MsTime b);

public:
    static unsigned char Use_ADCNUM;  //每个板子上ADC几个是在运行的 目前不支持Slave模式
    static unsigned long Aux_V_Trigger(unsigned char Bid,unsigned char Bchn);
    static unsigned long Aux_V_Read(unsigned char Bid);
    static void sampleSettingSet_CS(unsigned char Bid);
    static void Init(unsigned char BoardID);
    static char SampleISR(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID);
};




