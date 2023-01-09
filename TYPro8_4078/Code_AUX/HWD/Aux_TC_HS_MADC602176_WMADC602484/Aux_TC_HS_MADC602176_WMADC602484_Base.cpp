///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_TC_HS_MADC602176_WMADC602484_Base.cpp
//Version:		V1.0
//Date:				2016/07/13
//Description:
//						����������ز���
//Others:      WP
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"




SSPI_Para Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC;
unsigned long Aux_TC_HS_MADC602176_WMADC602484_Base::m_Inchip_temperature;   //Ƭ���¶ȴ���������ֵ

unsigned long Aux_TC_HS_MADC602176_WMADC602484_Base::error_counter;

void Aux_TC_HS_MADC602176_WMADC602484_Base::IO_Init(void)
{
    //There is no speacial port for this board
    // ControlPin::SetIObyCP(Pin_ADCBUSY_T);
}
void Aux_TC_HS_MADC602176_WMADC602484_Base::SPI_SET(void)
{
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_Module_Name = SSP0_DEVICE1;
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_Speed = 2;
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_CPOL = 0;
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_CPHA = 0;
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_MSTR = 1;    //master
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_BITS = 8;
    Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp

    SSPIFunc::setPara(&Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_TC_HS_MADC602176_WMADC602484_SSP0ADC,0);
    SSPIFunc::my_Init(0);
}

void Aux_TC_HS_MADC602176_WMADC602484_Base::ADC_Init( unsigned char BID)
{
    // No speacial ADC init



}
void Aux_TC_HS_MADC602176_WMADC602484_Base::Func_Init(unsigned char BoardID)
{
    // this should be moved to hardware_Init so all the Aux_ADC_CS_Info inital togegher

}
void Aux_TC_HS_MADC602176_WMADC602484_Base::Hardware_Init(unsigned char BoardID)
{
//	IO_Init();


    Aux_TC_HS_MADC602176_WMADC602484_Base::SPI_SET();

    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //����ADC ��Ϣ
    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_TC_HS_602484;

        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;
        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = 3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 17;   // 17 ���ڲ����¶�
        Aux_DataManager::Aux_ADC_CS_Info[i].b_ADCCH_Valid[16]=1; //��֤�����¶�
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i;  //1101 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx = i;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = &Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_T_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = &Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_T_Read;

        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //���data_group ��Ч
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i] = true;
        Aux_DataManager::m_DGIdx2BIdx[i] = BoardID;

    }
    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }

    unsigned char bt_ADC_Cnt =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC;
    unsigned char bt_CHN_Cnt =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_Channels;



    //��������ͨ����Ϣ��AuxDataBank��ȥ��AuxDataBank[0]---AuxDataBank[3]
    //AuxDataBank[0]��Ӧ��һ��ADC(1-16ͨ��--AuxDataBank[0][0]-AuxDataBank[0][15])
    //AuxDataBank[1]��Ӧ�ڶ���ADC(17-32),AuxDataBank[2]��Ӧ������ADC(33-48),AuxDataBank[3]��Ӧ���ĸ�ADC(49-64)

    for(int i = 0; i< 64; i++)
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ��AuxDataBank[0]---AuxDataBank[3]
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[i%16]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info.m_Populated;
    }

    unsigned char ChnCountPerADC = bt_CHN_Cnt/bt_ADC_Cnt;

    if((bt_CHN_Cnt%bt_ADC_Cnt)!=0)
        ChnCountPerADC = bt_CHN_Cnt/bt_ADC_Cnt+1;

    for(int j=0; j<bt_CHN_Cnt; j++)
    {
        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+j/16][j%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+j/ChnCountPerADC;
        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+j/16][j%16].e_chan_id = j%ChnCountPerADC;
    }



}
void Aux_TC_HS_MADC602176_WMADC602484_Base::Software_Init(unsigned char Board_ID)
{
    // There is no speacial software init
}


//K�¶�
unsigned long Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2498::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_TC_HS_MADC602176_WMADC602484_Base::Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn)
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




