#include "../Entry/includes.h"
unsigned short            AuxPID_task::Priority=PRIORITY4;
unsigned short            AuxPID_task::TaskID=AUX_PID_TASK;
unsigned char             AuxPID_task::Counter=0;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

ST_AUX_PID                AuxPID_task::mAuxPID_par[MAXPIDCHANNELNUM];  //
unsigned char             AuxPID_task::beNeedPID[MAXPIDCHANNELNUM];    // 00:无DAC输出任务， 01：开环输出DAC ， 02:闭环输出DAC
#pragma arm section //结束外存变量定义

void AuxPID_task::DoWork(void)
{
    Counter++;
    if(Counter<10)
        return;
    else
        Counter=0;
    for(unsigned char uc_AuxChannel=0; uc_AuxChannel<MAXPIDCHANNELNUM; uc_AuxChannel++)
    {
        unsigned long data;
        if(AuxPID_task::beNeedPID[uc_AuxChannel]==0x02)
        {
            float sample_value = Aux_DataManager::m_AuxDataBank[mAuxPID_par[uc_AuxChannel].m_AI_DataBank][mAuxPID_par[uc_AuxChannel].m_AI_Chn].aux_data;
            AuxPID_task::mAuxPID_par[uc_AuxChannel].m_SampleValue_1B= Aux_DataManager::UnifyADCVoltage(mAuxPID_par[uc_AuxChannel].m_AI_Chn,sample_value);
            float beOutput=AuxPID_task::AUX_PID_Output_Calculation(uc_AuxChannel,AuxPID_task::mAuxPID_par[uc_AuxChannel].Kp,AuxPID_task::mAuxPID_par[uc_AuxChannel].Ki,  \
                           AuxPID_task::mAuxPID_par[uc_AuxChannel].Kd,AuxPID_task::mAuxPID_par[uc_AuxChannel]);

            float value =Aux_DataManager::DeUnifyVoltage(uc_AuxChannel,beOutput);  //如果是电压控制，其他的自行修改

            Aux_DataManager::m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_AuxChannel].m_DataBank][uc_AuxChannel].aux_data = value;
            Aux_DataManager::m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_AuxChannel].m_DataBank][uc_AuxChannel].updated = 1;
            Aux_DataManager::m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_AuxChannel].m_DataBank][uc_AuxChannel].value_update_time = CommonFunc::GetSystemTime();//OS_Time;
            data=Aux_DataManager::CalibrateVoltageDAC(AuxPID_task::mAuxPID_par[uc_AuxChannel].m_DataBank,uc_AuxChannel,value);
            Aux_AIO_DIO_602178_WMADC602548_Base::Dac_out(uc_AuxChannel, data);
        }
        else if(AuxPID_task::beNeedPID[uc_AuxChannel]==0x01)
        {
            data=AuxPID_task::mAuxPID_par[uc_AuxChannel].m_SetValue_Open;
            Aux_AIO_DIO_602178_WMADC602548_Base::Dac_out(uc_AuxChannel, data);// 0x6696836
        }
        else
            continue;

    }
}
void AuxPID_task::MyInit(void)
{
    for(unsigned char uc_i=0; uc_i<MAXPIDCHANNELNUM; uc_i++)
        AuxPID_task::beNeedPID[uc_i]=0x00;
}
void AuxPID_task::EndTask(void)
{
}
float AuxPID_task::AUX_PID_Output_Calculation(unsigned char uc_Ch_No, float fKp,  float fKi, float fKd, ST_AUX_PID pPid)
{
    float DeltaOutput = 0;
    float Err_1B = AuxPID_task::mAuxPID_par[uc_Ch_No].m_SetValue_1B - AuxPID_task::mAuxPID_par[uc_Ch_No].m_SampleValue_1B;

    if(abs(Err_1B) < 0.0001f)	// 如果差值小于要求的精度，则不做PID控制， 09.09.2015
        DeltaOutput = 0;
    else
        DeltaOutput = fKp * (Err_1B - AuxPID_task::mAuxPID_par[uc_Ch_No].m_fLastErr_1B)   +   fKi * Err_1B;

    float Output = AuxPID_task::mAuxPID_par[uc_Ch_No].m_LastOutput_1B + DeltaOutput;

    AuxPID_task::mAuxPID_par[uc_Ch_No].m_LastOutput_1B=Output;
    AuxPID_task::mAuxPID_par[uc_Ch_No].m_fLastErr_1B=Err_1B;
    return Output;
}