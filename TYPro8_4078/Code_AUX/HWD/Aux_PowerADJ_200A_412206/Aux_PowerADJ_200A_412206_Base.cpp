//#include "includes.h"
#include "../Entry/includes.h"


/**********************************************************
** Start:
******************************************************************************/
const St_Port   Aux_PowerADJ_200A_412206_Base::SW_TR = {1,       18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

unsigned short Aux_PowerADJ_200A_412206_Base::TimeTicks;
MsTime Aux_PowerADJ_200A_412206_Base::EngineDelayTime;
unsigned char Aux_PowerADJ_200A_412206_Base::PowerStatus = POWER_IDLE;
void Aux_PowerADJ_200A_412206_Base::Hardware_Init(unsigned char BoardID)
{
// 	 IO_Init();
    unsigned char AuxFunc;
    ADCFunc::Init ();
    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    if( ((AuxFunc&0x01)==0x01)&&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC!=0 )	)// 配置了ADC
    {

        Aux_DataManager::Aux_ADC_CS_Info[0].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardType = Aux_PowerADJ_200A_412206;

        Aux_DataManager::Aux_ADC_CS_Info[0].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[0].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_StateMachine = AUX_Sample_IDLE;

        Aux_DataManager::Aux_ADC_CS_Info[0].sspChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_CS_Address.m_Value = 0x00;//    0010 EOE~ECODE1   0x0E; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[0].m_Wait_Count = 100;  //10*50=500us
        Aux_DataManager::Aux_ADC_CS_Info[0].m_ADCCH_MaxCnt = 8;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger =  &Aux_PowerADJ_200A_412206_Base::Aux_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_PowerADJ_200A_412206_Base::Aux_Read;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx = 0;
        Aux_DataManager::m_DGIdx2BIdx[BoardID] = 0;

        //获取通道信息并建立面板通道与电气通道的联系

        //分配 data_group 开始号
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //标记data_group 有效
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

        for(int i = 0; i< 8; i++) //该板型最多8个电气通道，且该板具备主副结构
        {   //拷贝板上通道信息到AuxDataBank中去。
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
            //根据该通道populated 来确定ＡＤＣ通道是否有效　
            Aux_DataManager::Aux_ADC_CS_Info[0].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;

            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号

        }

    }
    if(((AuxFunc&0x80)==0x80) &&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC !=0))  //配置了DAC通道
    {

        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_DAC_chn[i];   //copy  通道信息到 AuxDataBank
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i]. m_Chn_Info.m_Populated = Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_DAC_chn[i].m_Populated;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i]. m_Chn_Info.m_AuxType = AUX_AO;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3;

            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_chan_id = i; //面板通道与电气通道同号

        }
    }

    ControlPin::SetMeLow(SW_TR);

}
void Aux_PowerADJ_200A_412206_Base::Software_Init(unsigned char BoardID)
{

    board_config::Func_AUX_Dac_Out = &Aux_PowerADJ_200A_412206_Base::DAC6571_SET;
    POWERADJ::Func_Voltage_Dac_Out = &Aux_PowerADJ_200A_412206_Base::Aux_Power_dac_out;
    POWERADJ::PowerRunningState = &Aux_PowerADJ_200A_412206_Base::OpenProcess200A;
    POWERADJ::VppSET_VOL_MAX = Aux_DataManager::m_AuxDataBank[3][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    POWERADJ::VppSET_VOL_MIN = Aux_DataManager::m_AuxDataBank[3][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    POWERADJ::VmmSET_VOL_MAX = Aux_DataManager::m_AuxDataBank[3][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    POWERADJ::VmmSET_VOL_MIN = Aux_DataManager::m_AuxDataBank[3][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
}
// void Aux_PowerADJ_200A_412206_Base::DataSampleProcess200A (unsigned char chan)
// {
//
// 	     POWERADJ::m_M4_Value.m_Vmm_Adc = 4.0f;
// //  			 data1 = ADCFunc::Read(0);
//
// 	     POWERADJ::m_M4_Value.m_Vpp_Adc = Aux_DataManager::m_AuxDataBank[0][chan].aux_data;
//
// }


unsigned char Aux_PowerADJ_200A_412206_Base::OpenProcess200A(void)
{
    MsTime tempT;

    switch(PowerStatus)
    {
    case POWER_IDLE:
        Aux_Power_dac_out(0,POWERADJ::VppSET_VOL_MAX);
        PowerStatus = POWER_WAIT;
        CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
        break;
    case POWER_WAIT:
        MsTime tempT = CommonFunc::TimeDiff(EngineDelayTime);
        if(GPIOFunc::RD_G_Port(2,12) == 0x00 )
        {
            if(tempT.Us100  >1000)//100ms
            {
                PowerStatus =POWER_RUN;
                CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
            }
        }
        else
        {
            if(tempT.Us100  >1000)//100ms
            {
                PowerStatus = POWER_ERROR;
                CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
            }
        }
        break;
    case POWER_RUN:
        MsTime tempT1 = CommonFunc::TimeDiff(EngineDelayTime);
        if(GPIOFunc::RD_G_Port(2,12) == 0x01)
        {
            if(tempT1.Us100  >1000)//100ms
            {
                PowerStatus = POWER_ERROR;
            }
        }
        else
        {
            CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
        }
        break;
    case POWER_ERROR:

        break;
    }

    return PowerStatus;
}

void Aux_PowerADJ_200A_412206_Base::DAC6571_SET(unsigned char chan,unsigned long data)
{
    uint16_t DAC_InCoding = 0;
    st_I2CModule  I2Cmodule_a;

    DAC_InCoding = (data & 0x3FF) << 2;
    I2Cmodule_a.I2Cx = I2C_Power;
    I2Cmodule_a.CtrlByteNum = 2;//POINTER BYTE
    I2Cmodule_a.BufLen = 0;
    memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
    if(chan==0)
        I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = 0x98;
    else if(chan==1)
        I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = 0x9A;
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = DAC_InCoding & 0x0FFC;
    I2Cmodule_a.OPMode = I2C_MASTWRITE;
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        //unlock
        I2CBusNew::m_I2CBusMgr[I2Cmodule_a.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return ;

    }
    else
        return ;//失败
}

void Aux_PowerADJ_200A_412206_Base::Aux_Power_dac_out(unsigned char chan,float Vol_Vset)
{
    if(chan!=0)//这个板就一个通道
        return;
    if(Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_Populated!=1)
        return;
// 	Vol_Vset = 15;
    uint16 DAC_CODE = 0;
    if(Vol_Vset > Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
        Vol_Vset = Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    else if(Vol_Vset < Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
        Vol_Vset = Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    Aux_DataManager::m_AuxDataBank[3][chan].aux_data = Vol_Vset;
    Aux_DataManager::m_AuxDataBank[3][chan].updated = 1;
    Aux_DataManager::m_AuxDataBank[3][chan].value_update_time = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;  //160429
    DAC_CODE=Aux_DataManager::CalibrateVoltageDAC(3,chan,Vol_Vset);
    if(DAC_CODE>0x3FF)
        DAC_CODE = 0x3FF;
    DAC6571_SET(chan,DAC_CODE);

}
unsigned long Aux_PowerADJ_200A_412206_Base::Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    return 1;
}
unsigned long Aux_PowerADJ_200A_412206_Base::Aux_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = ADCFunc::ReadADC_Code(ADCchn); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //标记已经更新
    return 1;
}