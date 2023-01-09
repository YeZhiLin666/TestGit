///////////////////////////////////////////////////////////////////////////////////
//File Name: IndependentVoltage.cpp
//Version:		V1.0
//Date:				2017/06/10
//Description:
//						独立恒压控制任务，用于SK从通道电压处理
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
UInt16 IndependentVoltageCtrl_task::Priority = PRIORITY4;
UInt16 IndependentVoltageCtrl_task::TaskID = INDEPENDENTVOLTAGECTRL_TASK;			// should be a definition which positioning this task in OS_Task[]
unsigned char IndependentVoltageCtrl_task::be_ChannelRunFlg[MAXCHANNELNO];
// float 				IndependentVoltageCtrl_task::m_CtrlValue[MAXCHANNELNO];
bool IndependentVoltageCtrl_task::Series2BoosterSendVoltageEnable;
void IndependentVoltageCtrl_task::DoWork(void)                                                                 //任务执行函数
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_INDEPENDENT_VOL_CTRL] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_INDEPENDENT_VOL_CTRL);
#endif
	
    unsigned long u32_Voltage;
    static unsigned char Series2BoosterSendVoltageTime = 0;
    for(unsigned char uc_channel=0; uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++)
    {
        if(ChannelInfo_Manager::Serial2ParaMod_IsInSubmasterUnit() && be_ChannelRunFlg[uc_channel]==true)
        {

            if(ChannelInfo_Manager::Serial2ParaMod_IsBoosterUnit())
            {
                //PWM_SampleManage::m_InsideCtrlVoltageValue=ChannelInfo_Manager::IV_Buffer1[uc_channel].m_fVoltage;   //03.21.2018 CAN????

                if(PWM_SampleManage::m_InsideCtrlVoltageValue>0)
                {
                    PWM_SampleManage::m_InsideCtrlVoltageValue= 0;
                }

                u32_Voltage=ChannelInfo_Manager::CalibrateVoltageDAC(uc_channel,VOLTAGE_RANGE_HIGH,
                            (abs(PWM_SampleManage::m_InsideCtrlVoltageValue)+BOOSTER_MIN_VOLTAGE));

                if((Series2BoosterSendVoltageEnable==true)&&(Series2BoosterSendVoltageTime++>=10))
                {
                    CANBusParse::Parallel_Series2BoosterSend_OutVoltage(ChannelInfo_Manager::IV_Buffer1[uc_channel].m_fVoltage);
                    Series2BoosterSendVoltageTime = 0;
                }
            }
            else if(ChannelInfo_Manager::Serial2ParaMod_IsSerialUnit())
            {
                if(PWM_SampleManage::m_InsideCtrlVoltageValue<BOOSTER_MIN_VOLTAGE)
                {
                    PWM_SampleManage::m_InsideCtrlVoltageValue= BOOSTER_MIN_VOLTAGE;
                }

                u32_Voltage=ChannelInfo_Manager::CalibrateVoltageDAC(uc_channel,VOLTAGE_RANGE_HIGH,
                            (PWM_SampleManage::m_InsideCtrlVoltageValue)/2);

                if((Series2BoosterSendVoltageEnable==true)&&(Series2BoosterSendVoltageTime++>=10))
                {
                    CANBusParse::Parallel_Series2BoosterSend_OutVoltage(ChannelInfo_Manager::IV_Buffer1[uc_channel].m_fVoltage);
                    Series2BoosterSendVoltageTime = 0;
                }
            }

            (*board_config::Func_Dac_out)(uc_channel,u32_Voltage,ACTIVE_DAC_VLT);
        }
    }
		
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_INDEPENDENT_VOL_CTRL] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_INDEPENDENT_VOL_CTRL);
#endif
}
void IndependentVoltageCtrl_task::MyInit(void)                                                                 //任务初始化
{
    for(unsigned char uc_i=0; uc_i<MAXCHANNELNO; uc_i++)
    {
        be_ChannelRunFlg[uc_i]=false;
//         m_CtrlValue[uc_i]=0.0f;
        Series2BoosterSendVoltageEnable = false;
    }
}
void IndependentVoltageCtrl_task::EndTask(void)                                                                //任务结束
{

}
