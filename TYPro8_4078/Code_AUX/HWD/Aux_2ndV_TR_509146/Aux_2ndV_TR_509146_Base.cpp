///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						����������ز���
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

SSPI_Para Aux_2ndV_TR_509146_Base::Aux_2ndV_TR_509146_SSP1ADC;

void Aux_2ndV_TR_509146_Base::IO_Init(void)
{
    //There is no speacial port for this board


}
void Aux_2ndV_TR_509146_Base::SPI_SET(void)
{
    Aux_2ndV_TR_509146_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_2ndV_TR_509146_SSP1ADC.uc_Speed = 2;
    Aux_2ndV_TR_509146_SSP1ADC.uc_CPOL = 0;
    Aux_2ndV_TR_509146_SSP1ADC.uc_CPHA = 0;
    Aux_2ndV_TR_509146_SSP1ADC.uc_MSTR = 1;    //master
    Aux_2ndV_TR_509146_SSP1ADC.uc_BITS = 8;
    Aux_2ndV_TR_509146_SSP1ADC.uc_LSBF = 0;   //LTC2449

    SSPIFunc::setPara(&Aux_2ndV_TR_509146_Base::Aux_2ndV_TR_509146_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_2ndV_TR_509146_Base::ADC_Init(unsigned char BoardID,unsigned long ADCID)
{


}
void Aux_2ndV_TR_509146_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{

}
void Aux_2ndV_TR_509146_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();
    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //ÿ����ֻ��һ��ADC LTC2449

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_2ndV_TR_509146;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;


    Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID); //1110 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = 10;  //10*50=500us
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 16;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_2ndV_TR_509146_Base::Aux_V_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_2ndV_TR_509146_Base::Aux_V_Read;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = BoardID;
    Aux_DataManager::m_DGIdx2BIdx[BoardID] = BoardID; //data_group index �� board ID ��ֵ

    //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

    //���� data_group ��ʼ��
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
    //���data_group ��Ч
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //�ð������16������ͨ�����Ҹð��޸���
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ���������е��ظ�������������
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;

        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;



    }


}
void Aux_2ndV_TR_509146_Base::Software_Init(unsigned char Board_ID)
{
    // There is no speacial software init
}

unsigned long Aux_2ndV_TR_509146_Base::Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);

    return 1;
}

unsigned long Aux_2ndV_TR_509146_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET();
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //�����ݷŵ�Aux_DataBank ��
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //����Ѿ�����
    return 1;

}
