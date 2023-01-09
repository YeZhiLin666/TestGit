///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_TC_MADC602280_WMADC602524_Base.cpp
//Version:		V1.0
//Date:				2016/07/13
//Description:
//						����������ز���
//Others:      DKQ
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"




SSPI_Para Aux_TC_MADC602280_WMADC602524_Base::Aux_TC_MADC602280_WMADC602524_SSP0ADC;
unsigned long Aux_TC_MADC602280_WMADC602524_Base::m_Inchip_temperature;   //Ƭ���¶ȴ���������ֵ

unsigned long Aux_TC_MADC602280_WMADC602524_Base::error_counter;

void Aux_TC_MADC602280_WMADC602524_Base::IO_Init(void)
{
    //There is no speacial port for this board
    // ControlPin::SetIObyCP(Pin_ADCBUSY_T);
}
void Aux_TC_MADC602280_WMADC602524_Base::SPI_SET(void)
{
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_Module_Name = SSP0_DEVICE1;
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_Speed = 2;
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_CPOL = 0;
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_CPHA = 0;
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_MSTR = 1;    //master
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_BITS = 8;
    Aux_TC_MADC602280_WMADC602524_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp

    SSPIFunc::setPara(&Aux_TC_MADC602280_WMADC602524_Base::Aux_TC_MADC602280_WMADC602524_SSP0ADC,0);
    SSPIFunc::my_Init(0);
}

void Aux_TC_MADC602280_WMADC602524_Base::ADC_Init( unsigned char BID)
{
    // No speacial ADC init
    // Adc32bit_LTC2498::Pin_CS = AUX_Unit_Base::Pin_ECODE2;
    // Adc32bit_LTC2498::Pin_Busy = Aux_TC_MADC602280_WMADC602524_Base::Pin_ADCBUSY_T;


}
void Aux_TC_MADC602280_WMADC602524_Base::Func_Init(unsigned char BoardID)
{
    // this should be moved to hardware_Init so all the Aux_ADC_CS_Info inital togegher
    //Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Trigger;
    //Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Read;
}
void Aux_TC_MADC602280_WMADC602524_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();

// 	Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;   //��PN���µ�ADCоƬ��������������PN600826û�в��룬��ADC�Ĳ����ǹ̶���
// 	Aux_TC_MADC602280_WMADC602524_Base::ADC_Init();

    Aux_TC_MADC602280_WMADC602524_Base::SPI_SET();
    //���� data_group ��ʼ��
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //����ADC ��Ϣ
    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_TC_602524;

        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;
        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 0;
        if(Adc32bit_LTC2498::IsExCrystalsFlag==1)
            Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = 840;
        else
            Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = 3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 17;   // 17 ���ڲ����¶�
        Aux_DataManager::Aux_ADC_CS_Info[i].b_ADCCH_Valid[16]=1; //��֤�����¶�
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i;  //1101 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx = i;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = &Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = &Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Read;
        //���data_group ��Ч
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i] = true;
        Aux_DataManager::m_DGIdx2BIdx[i] = BoardID; //data_group index 0,�� 1 ��Ӧ��  board ID

    }



    for(int i = 0; i< 64; i++) //�ð������16������ͨ��
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ���������е��ظ�������������
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[(i%16)]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info.m_Populated;
    }
    unsigned char bt_ADC_Cnt =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC;
    switch(bt_ADC_Cnt)
    {
    default:
    case 1:
        for(unsigned char i=0; i<16; i++)
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
        }
        break;
    case 2:
        for(unsigned char i=0; i<32; i++)
        {
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_SlaveGoesFirst))
            {
                if (i<16)  //�����ǰ16��ͨ���Ƕ�Ӧ�� 17-32 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = i;
                }
                else //
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-16].e_chan_id = i-16;
                }

            }
            else
            {
                if(i<16)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_chan_id = i-16;
                }

            }
        }
        break;
    case 3:
        for(unsigned char i=0; i<48; i++)
        {
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_SlaveGoesFirst))
            {
                if (i<16)  							//�����ǰ16��ͨ���Ƕ�Ӧ�� 32-48 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_chan_id =i;
                }
                else if(i>=16&&i<32)    	//������м�16��ͨ���Ƕ�Ӧ�� 16-32 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_chan_id = i-16;
                }
                else										//����Ϻ�16��ͨ���Ƕ�Ӧ�� 1-16����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-32].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-32].e_chan_id = i-32;
                }
            }
            else
            {
                if (i<16)  							//�����ǰ16��ͨ���Ƕ�Ӧ�� 1-16 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i;
                }
                else if(i>=16&&i<32)    	//������м�16��ͨ���Ƕ�Ӧ�� 16-32 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_chan_id = i-16;
                }
                else										//����Ϻ�16��ͨ���Ƕ�Ӧ�� 32-48����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-32].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-32].e_chan_id = i-32;
                }

            }
        }
        break;
    case 4:
        for(unsigned char i=0; i<64; i++)
        {
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_SlaveGoesFirst))
            {
                if (i<16)  							//�����ǰ16��ͨ���Ƕ�Ӧ�� 32-48 ����ͨ��
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_chan_id = i;
                }
                else if(i>=16&&i<32)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-16].e_chan_id = i-16;
                }
                else if(i>=32&&i<48)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-32].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-32].e_chan_id = i-32;
                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-48].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i-48].e_chan_id = i-48;
                }
            }
            else
            {
                if (i<16)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i;
                }
                else if(i>=16&&i<32)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-16].e_chan_id = i-16;
                }
                else	if(i>=32&&i<48)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-32].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i-32].e_chan_id = i-32;
                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i-48].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i-48].e_chan_id = i-48;
                }
            }
        }
        break;

    }

}
void Aux_TC_MADC602280_WMADC602524_Base::Software_Init(unsigned char Board_ID)
{
    // There is no speacial software init
}


//T�¶ȣ��������¶ȿ�����T�ͣ�Ҳ������K��
unsigned long Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2498::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_TC_MADC602280_WMADC602524_Base::Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn)
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




