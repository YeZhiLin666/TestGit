///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_P_TC_509138_Base.cpp
//Version:		V1.0
//Date:				2015/05/06
//Description:
//						辅助配置相关操作
//Others:      DKQ
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"




SSPI_Para Aux_P_TC_509138_Base::Aux_P_TC_509138_SSP0ADC;
unsigned long Aux_P_TC_509138_Base::m_Inchip_temperature;   //片内温度传感器的码值

unsigned long Aux_P_TC_509138_Base::error_counter;

void Aux_P_TC_509138_Base::IO_Init(void)
{
    //There is no speacial port for this board
    // ControlPin::SetIObyCP(Pin_ADCBUSY_T);
}
void Aux_P_TC_509138_Base::SPI_SET(void)
{
    Aux_P_TC_509138_SSP0ADC.uc_Module_Name = SSP0_DEVICE1;
    Aux_P_TC_509138_SSP0ADC.uc_Speed = 2;
    Aux_P_TC_509138_SSP0ADC.uc_CPOL = 0;
    Aux_P_TC_509138_SSP0ADC.uc_CPHA = 0;
    Aux_P_TC_509138_SSP0ADC.uc_MSTR = 1;    //master
    Aux_P_TC_509138_SSP0ADC.uc_BITS = 8;
    Aux_P_TC_509138_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp

    SSPIFunc::setPara(&Aux_P_TC_509138_Base::Aux_P_TC_509138_SSP0ADC,0);
    SSPIFunc::my_Init(0);
}

void Aux_P_TC_509138_Base::ADC_Init( unsigned char BID)
{
    // No speacial ADC init
    // Adc32bit_LTC2498::Pin_CS = AUX_Unit_Base::Pin_ECODE2;
    // Adc32bit_LTC2498::Pin_Busy = Aux_P_TC_509138_Base::Pin_ADCBUSY_T;


}
void Aux_P_TC_509138_Base::Func_Init(unsigned char BoardID)
{
    // this should be moved to hardware_Init so all the Aux_ADC_CS_Info inital togegher
    //Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_P_TC_509138_Base::Aux_T_Trigger;
    //Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_P_TC_509138_Base::Aux_T_Read;
}
void Aux_P_TC_509138_Base::Hardware_Init(unsigned char BoardID)
{
    //IO_Init();

    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;   //该PN号下的ADC芯片共有两个，由于PN600826没有波码，故ADC的波码是固定的
    //Aux_P_TC_509138_Base::ADC_Init();

    Aux_P_TC_509138_Base::SPI_SET();

    //设置片选信息
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_P_TC_509130;


    Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID);  //1101 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = 3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 17;   // 17 用于采样温度
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[16]=1; //保证采样温度
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_P_TC_509138_Base::Aux_T_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_P_TC_509138_Base::Aux_T_Read;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = BoardID;

    Aux_DataManager::m_DGIdx2BIdx[BoardID] = BoardID; //data_group index 与 board ID 等值


    //获取通道信息并建立面板通道与电气通道的联系

    //分配 data_group 开始号
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
    //标记data_group 有效
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //该板型最多16个电气通道
    {   //拷贝板上通道信息到AuxDataBank中去。好像是有点重复，但先这样吧
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
        //根据该通道populated 来确定ＡＤＣ通道是否有效　
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //对热电偶板，若带副板的话，要实行转换.其他板无副板的可能。
        {
            if (i<8)  //面板上前8个通道是对应于 9-15 电气通道
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 8+i;
            }
            else //面板上前8个通道是对应于 9-15 电气通道
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i-8;
            }

        }
        else
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号
        }

    }

}
void Aux_P_TC_509138_Base::Software_Init(unsigned char Board_ID)
{
    // There is no speacial software init
}


//T温度，在这里温度可能是T型，也可能是K型
unsigned long Aux_P_TC_509138_Base::Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2498::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_P_TC_509138_Base::Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn)
{
    unsigned long temp;
    SPI_SET();
    if (ADCchn>=16)
    {
        unsigned long ul_Inchip_temperature = Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
        if(ul_Inchip_temperature!=0)
        {
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].InChipTemp_instant_code = ul_Inchip_temperature ;//将数据放到一个统一管理的片内温度QUE里以便做平均处理
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].updated= true ;//将数据放到一个统一管理的片内温度QUE里以便做平均处理
        }
        else
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].updated= false;
    }
    else
    {
        temp=Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
        if(temp!=0)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = temp;//将数据放到Aux_DataBank 中, 2498的数放到16-31
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true;
        }
        else
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = false;
    }
    return 1;
}
