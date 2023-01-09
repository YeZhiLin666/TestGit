///////////////////////////////////////////////////////////////////////////////////
//File Name: HighMode2ndV_SampleManage.cpp
//Version:		V1.0
//Date:				2014/11/28
//Description:
//						HighMod2ndV辅助采样相关处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
#define AUX_ADS1259_SAMPLE_TIME    10        //50us 单位     50*10=500us
st_Aux_Sample 	HighMode2ndV_SampleManage::st_aux_sample_setting[MAX_BOARD_PER_AUXMCU];
unsigned long 	HighMode2ndV_SampleManage::Sample_conter[MAX_BOARD_PER_AUXMCU];


void HighMode2ndV_SampleManage::Init(unsigned char BoardID)
{
    for(unsigned char uc_i=0; uc_i<MAX_AUXCH_PER_BOARD; uc_i++)
    {

        Sample_conter[BoardID]=0;
        HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].stateMachine=AUX_SAMPLE_IDLE_ENUM;
        HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].sspChn=1;
        HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].ADC_chn=0;
    }

}
char HighMode2ndV_SampleManage::SampleISR(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
    if(Aux_DataManager::m_Aux_BoardCFG[BoardID].BeUsed==0)
        return 0;
    SSPIFunc::setPara(&HighMode2ndV_Base::HighMode2ndV_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化
    static signed char next_chn[8] = {0,0,0,0,0,0,0,0};   //下一个trigger的chn

    switch(st_aux_sample_setting[BoardID].stateMachine)    //状态机
    {
    case AUX_SAMPLE_IDLE_ENUM:
        //第一次只是简单的进行采样存在的板子的第一个物理通道即可
        next_chn[BoardID]= 0;
        st_aux_sample_setting[BoardID].ADC_chn = next_chn[BoardID];
        Aux_V_Trigger(BoardID,next_chn[BoardID]);    //first just trigger and no return
        st_aux_sample_setting[BoardID].stateMachine=AUX_SAMPLE_WAITTING_TRIGGER;
        break;
    case AUX_SAMPLE_WAITTING_TRIGGER:
        Aux_V_Read(BoardID);    //此时read信息 无用
        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;
        break;
    case AUX_SAMPLE_WAITTING_SAMPLE:
        Sample_conter[BoardID]++;

        if(Sample_conter[BoardID] >= AUX_ADS1259_SAMPLE_TIME)    //500us
        {
            Sample_conter[BoardID]= 0;
            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_TRIGGER_NEXT;
        }
        break;
    case AUX_SAMPLE_TRIGGER_NEXT:

        if(next_chn[BoardID]>=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_ChannelCount)
            next_chn[BoardID]=0;
        else
            next_chn[BoardID]++;

        sampleSettingSet_CS(BoardID);

        Aux_V_Trigger(BoardID,next_chn[BoardID]);
        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_TRIGGER_NEXT;
        break;
    case AUX_SAMPLE_WAITTING_TRIGGER_NEXT:
        unsigned long adc_code_temp;
        unsigned long temp[16];
        // Adc24bit_ADS1259::SetChannel(st_aux_sample_setting[BoardID].ADC_chn);
        //( *board_config::Func_AUX_ADC_CS[0][0][st_aux_sample_setting[BoardID].ADC_chn])(0,0,st_aux_sample_setting[BoardID].ADC_chn);
        //		        Adc24bit_ADS1259::Convert();
        temp[st_aux_sample_setting[BoardID].ADC_chn]=Adc24bit_ADS1259::Read(1,st_aux_sample_setting[BoardID].ADC_chn);//采样数据源码

        adc_code_temp =temp[st_aux_sample_setting[BoardID].ADC_chn];
        if (adc_code_temp!=0)
        {
            Aux_DataManager::st_Aux_info[BoardID][HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].ADC_chn].AdcCode = adc_code_temp;
            Aux_DataManager::st_Aux_info[BoardID][HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].ADC_chn].NewAdcCode = 1;
        }
        else
        {
            Aux_DataManager::st_Aux_info[BoardID][HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].ADC_chn].NewAdcCode = 0;
        }

        HighMode2ndV_SampleManage::st_aux_sample_setting[BoardID].ADC_chn = next_chn[BoardID];    //为下一次数据准备

        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;    //继续等待采样

        break;
    default:
        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_IDLE_ENUM;
        break;
    }




    return 1;
}
unsigned long HighMode2ndV_SampleManage::Aux_V_Trigger(unsigned char Bid,unsigned char Bchn)
{
    Adc24bit_ADS1259::SetChannel(Bchn);
    Adc24bit_ADS1259::Convert();
    return 1;

}

unsigned long HighMode2ndV_SampleManage::Aux_V_Read(unsigned char Bid)
{

    return 0;

}
void HighMode2ndV_SampleManage::sampleSettingSet_CS(unsigned char Bid)
{
    //   片选信号输出进行过滤处理

    //     switch(Bid)
    //     {
    //         case 7:
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE1,0); // 7号板
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE2,0);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE3,0);
    //             break;
    //         case 6:
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE1,1);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE2,0);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE3,0);
    //             break;
    //         case 5:
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE1,0);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE2,1);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE3,0);
    //             break;
    //         case 4:
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE1,1);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE2,1);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE3,0);
    //             break;
    //         case 3:
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE1,0);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE2,0);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE3,1);
    //             break;
    //         case 2:
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE1,1);
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE2,0);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE3,1);
    //             break;
    //         case 1:
    //             ControlPin::SetMeLow(LowMode2ndV_Base::Pin_ECODE1,0);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE2,1);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE3,1);
    //             break;
    //         case 0:
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE1,1);   // 0号板
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE2,1);
    //             ControlPin::SetMeHigh(LowMode2ndV_Base::Pin_ECODE3,1);
    //             break;
    //         default:
    //             break;
    //     }
}

