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

SSPI_Para Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC;
const St_Port Aux_2ndV_HIR_511898_WMADC602540_Base::Pin_Conver		= {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
unsigned short  Aux_2ndV_HIR_511898_WMADC602540_Base:: WaitForSampleCnt;

void Aux_2ndV_HIR_511898_WMADC602540_Base::IO_Init(void)
{
    //There is no speacial port for this board
    ControlPin::SetIObyCP(Pin_Conver);

    // 	Adc24bit_ADS1259::Pin_Conver = Aux_2ndV_HIR_511898_WMADC602540_Base::Pin_Conver;

}
void Aux_2ndV_HIR_511898_WMADC602540_Base::SPI_SET(void)
{
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_Module_Name = SSP1_DEVICE0;
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_Speed = 2;
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_CPOL = 0;
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_CPHA = 1;
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_MSTR = 1;    //master
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_BITS = 8;
    Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC.uc_LSBF = 0;   //ADS1259


    SSPIFunc::setPara(&Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_2ndV_HIR_511898_WMADC602540_Base::ADC_Init(void)
{
    unsigned short AdcSSP;
    Adc24bit_ADS1259::Pin_Conver = Aux_2ndV_HIR_511898_WMADC602540_Base::Pin_Conver;
    Adc24bit_ADS1259::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,5);
    AdcSSP=4;  //
    switch(AdcSSP)
    {
    case 0:
        WaitForSampleCnt=4500;  //10HZ
        break;
    case 1:
        WaitForSampleCnt=2500;  //16.6HZ
        break;
    case 2:
        WaitForSampleCnt=810;  //50HZ
        break;
    case 3:
        WaitForSampleCnt=700;  //60HZ
        break;
    case 4:
        WaitForSampleCnt=110;  //400HZ
        break;
    case 5:
        WaitForSampleCnt=45;  //1200HZ
        break;
    case 6:
        WaitForSampleCnt=20;  //3600HZ
        break;
    case 7:
        WaitForSampleCnt=15;  //14400HZ
        break;
    default:
        WaitForSampleCnt=4500;
        break;
    }

    // 	SSPIFunc::setPara(&Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_2ndV_HIR_511898_WMADC602540_SSP1ADC,1);
    // 	SSPIFunc::my_Init(1);             //��ʼ��
    SPI_SET();

    for(unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::SettingCS_Low(i);
        //	 Adc24bit_ADS1259::WriteReg(1,i);
        Adc24bit_ADS1259::WriteReg(1);

    }

}
void Aux_2ndV_HIR_511898_WMADC602540_Base::Func_Init(void)
{
    // 	for(unsigned char i=0;i<14;i++)
    // 	  Aux_2ndV_HIR_511898_WMADC602540_Base::data[i]=0;
}
void Aux_2ndV_HIR_511898_WMADC602540_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();
    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=16;  //14 //��ʱ����16���Ժ��Ǹ�����λ���µ���Ϣ����
    ADC_Init();
    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid = Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i].m_Populated;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_2ndV_HIR_602540;
        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
        Aux_DataManager::Aux_ADC_CS_Info[i].b_ADCCH_Valid[0]=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i].m_Populated;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;

        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = WaitForSampleCnt;  //10*45=450us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = &Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = &Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Read;

        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx = 0; //һ������ϵͳ��ֻ��һ��߹�ģ�����16��ͨ�������data group index ��Ϊ0
    }

    Aux_DataManager::m_DGIdx2BIdx[0] = BoardID; //data_group index =0  �� board ID


    //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

    //���� data_group ��ʼ��  //1��AUXMCU��ֻ��һ��߹�ģ�Ķ��ε�ѹֱ�ӷŵ� data_group_idx =0
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //���data_group ��Ч
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //�ð������16������ͨ��
    {   //��������ͨ����Ϣ��AuxDataBank��ȥ���������е��ظ�������������
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //���ȵ�ż�壬��������Ļ���Ҫʵ��ת��.�������޸���Ŀ��ܡ�
        {
            if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_SlaveGoesFirst)
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
        else
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
        }

    }

}
void Aux_2ndV_HIR_511898_WMADC602540_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
}

unsigned long Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    //   Adc24bit_ADS1259::SetChannel(u_CSid);
    Adc24bit_ADS1259::Convert();
    return 1;
}

unsigned long Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    //�߹�ģÿ��ADCֻ��һ��ͨ�������������ǰ�Ƭѡ��ַ��ͨ����ַ��16��Ƭѡת����16��ͨ��
    SPI_SET();

    // Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].aux_instant_code = Adc24bit_ADS1259::Read(1,u_CSid); //�����ݷŵ�Aux_DataBank ��
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].aux_instant_code = Adc24bit_ADS1259::Read(1); //�����ݷŵ�Aux_DataBank ��

    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].updated = true; //����Ѿ�����
    return 1;

}
