///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_P_TC_509138_Base.cpp
//Version:		V1.0
//Date:				2015/05/06
//Description:
//						����������ز���
//Others:      DKQ
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"




SSPI_Para Aux_P_TC_509138_Base::Aux_P_TC_509138_SSP0ADC;
unsigned long Aux_P_TC_509138_Base::m_Inchip_temperature;   //Ƭ���¶ȴ���������ֵ

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

    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;   //��PN���µ�ADCоƬ��������������PN600826û�в��룬��ADC�Ĳ����ǹ̶���
    //Aux_P_TC_509138_Base::ADC_Init();

    Aux_P_TC_509138_Base::SPI_SET();

    //����Ƭѡ��Ϣ
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_P_TC_509130;


    Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID);  //1101 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = 3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 17;   // 17 ���ڲ����¶�
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[16]=1; //��֤�����¶�
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_P_TC_509138_Base::Aux_T_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_P_TC_509138_Base::Aux_T_Read;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = BoardID;

    Aux_DataManager::m_DGIdx2BIdx[BoardID] = BoardID; //data_group index �� board ID ��ֵ


    //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

    //���� data_group ��ʼ��
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
    //���data_group ��Ч
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //�ð������16������ͨ��
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ���������е��ظ�������������
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //���ȵ�ż�壬��������Ļ���Ҫʵ��ת��.�������޸���Ŀ��ܡ�
        {
            if (i<8)  //�����ǰ8��ͨ���Ƕ�Ӧ�� 9-15 ����ͨ��
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 8+i;
            }
            else //�����ǰ8��ͨ���Ƕ�Ӧ�� 9-15 ����ͨ��
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i-8;
            }

        }
        else
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
        }

    }

}
void Aux_P_TC_509138_Base::Software_Init(unsigned char Board_ID)
{
    // There is no speacial software init
}


//T�¶ȣ��������¶ȿ�����T�ͣ�Ҳ������K��
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
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].InChipTemp_instant_code = ul_Inchip_temperature ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].updated= true ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
        }
        else
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx].updated= false;
    }
    else
    {
        temp=Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
        if(temp!=0)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = temp;//�����ݷŵ�Aux_DataBank ��, 2498�����ŵ�16-31
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true;
        }
        else
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = false;
    }
    return 1;
}
