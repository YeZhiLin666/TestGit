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

SSPI_Para Aux_WMADC_PT100_602532_Base::Aux_WMADC_PT100_602532_SSP1ADC;
unsigned char Aux_WMADC_PT100_602532_Base::SampleErrorCount = 0;
void Aux_WMADC_PT100_602532_Base::IO_Init(void)
{
    //There is no speacial port for this board


}
void Aux_WMADC_PT100_602532_Base::SPI_SET(void)
{
    Aux_WMADC_PT100_602532_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_WMADC_PT100_602532_SSP1ADC.uc_Speed = 2;
    Aux_WMADC_PT100_602532_SSP1ADC.uc_CPOL = 0;
    Aux_WMADC_PT100_602532_SSP1ADC.uc_CPHA = 0;
    Aux_WMADC_PT100_602532_SSP1ADC.uc_MSTR = 1;    //master
    Aux_WMADC_PT100_602532_SSP1ADC.uc_BITS = 8;
    Aux_WMADC_PT100_602532_SSP1ADC.uc_LSBF = 0;   //LTC2449

    SSPIFunc::setPara(&Aux_WMADC_PT100_602532_Base::Aux_WMADC_PT100_602532_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_WMADC_PT100_602532_Base::ADC_Init(unsigned char BoardID,unsigned long ADCID)
{


}
void Aux_WMADC_PT100_602532_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{

}
void Aux_WMADC_PT100_602532_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();
    //���� data_group ��ʼ��
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //����ADC ��Ϣ
    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_PT100_602532;

        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;


        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx = i;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = 10;  //10*50=500us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 8;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = &Aux_WMADC_PT100_602532_Base::Aux_V_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = &Aux_WMADC_PT100_602532_Base::Aux_V_Read;
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
    //AuxDataBank[0]��Ӧ��һ��ADC(1-8ͨ��--AuxDataBank[0][0]-AuxDataBank[0][8])
    //AuxDataBank[1]��Ӧ�ڶ���ADC(17-24),AuxDataBank[2]��Ӧ������ADC(33-40),AuxDataBank[3]��Ӧ���ĸ�ADC(49-56)

    for(int i = 0; i< 64; i++)
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ��AuxDataBank[0]---AuxDataBank[3]
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[i%16]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info.m_Populated;
    }
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
void Aux_WMADC_PT100_602532_Base::Software_Init(unsigned char Board_ID)
{
    if(Aux_DataManager::m_AllAuxBoards[Board_ID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932)  //Add by DKQ
    {
        Aux_Chamber_TEC_Base::Aux_Chamber_TEC_Board = true;

    }
}

unsigned long Aux_WMADC_PT100_602532_Base::Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_WMADC_PT100_602532_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET();
    unsigned long code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //�����ݷŵ�Aux_DataBank ��
    if(code != 0)
		{
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = code;
		}
		else
		{
			SampleErrorCount++;
			if(SampleErrorCount>=100)
			{
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = code;
				SampleErrorCount = 0;
      }
    }
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //����Ѿ�����
    return 1;
}
