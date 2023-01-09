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

SSPI_Para Aux_MultiFunc_514194_Base::Aux_MultiFunc_514194_SSP1ADC;
//DI DO Module
//DI
const St_Port Aux_MultiFunc_514194_Base::Pin_DI_1      = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DI_2      = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DI_3      = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DI_4      = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_I,             0};

//DO
const St_Port Aux_MultiFunc_514194_Base::Pin_DO_1     = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DO_2     = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DO_3     = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_MultiFunc_514194_Base::Pin_DO_4     = {1,        29,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
//temp one board
St_DIDOMapMgr  Aux_MultiFunc_514194_Base::m_DIDO_MapMgr[MAX_DO_COUNT] = {
    0,0,&Pin_DI_1,&Pin_DO_1,
    1,1,&Pin_DI_2,&Pin_DO_2,
    2,2,&Pin_DI_3,&Pin_DO_3,
    3,3,&Pin_DI_4,&Pin_DO_4,

};

unsigned char Aux_MultiFunc_514194_Base::My_BoardID =0;

long Aux_MultiFunc_514194_Base::data[14];

void Aux_MultiFunc_514194_Base::IO_Init(void)
{
    //There is no speacial port for this board
    // 	ControlPin::SetIObyCP(Pin_Conver);

    // 	Adc24bit_ADS1259::Pin_Conver = Aux_MultiFunc_514194_Base::Pin_Conver;

}
void Aux_MultiFunc_514194_Base::SPI_SET(void)
{
    Aux_MultiFunc_514194_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_MultiFunc_514194_SSP1ADC.uc_Speed = 2;
    Aux_MultiFunc_514194_SSP1ADC.uc_CPOL = 0;
    Aux_MultiFunc_514194_SSP1ADC.uc_CPHA = 0;
    Aux_MultiFunc_514194_SSP1ADC.uc_MSTR = 1;    //master
    Aux_MultiFunc_514194_SSP1ADC.uc_BITS = 8;
    Aux_MultiFunc_514194_SSP1ADC.uc_LSBF = 0;   //LTC2449

    SSPIFunc::setPara(&Aux_MultiFunc_514194_Base::Aux_MultiFunc_514194_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_MultiFunc_514194_Base::ADC_Init(void)
{
    //    Aux_DataManager::m_AllAuxBoards[0].m_TotalADC=1;  //�˴�����������Ϣ���£����һ��ADC  LTC 2449


}
void Aux_MultiFunc_514194_Base::Func_Init(void)
{
    for(unsigned char i=0; i<14; i++)
        Aux_MultiFunc_514194_Base::data[i]=0;
}
void Aux_MultiFunc_514194_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;
    IO_Init();
    CANBusParse::DIDO_Ctrl=1;
    //    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //�˴�����������Ϣ���£����һ��ADC  LTC 2449
    // 	 ADC_Init();

    My_BoardID = BoardID;   //��¼�Լ���BoardID��
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    //if( (((AuxFunc&0x01)==0x01)&& (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC !=0)) || ((AuxFunc&0x10) == 0x10) ) // ������ADC���������˴����¿��䶼�����ʼ��
    if( ((AuxFunc&0x01)==0x01) || ((AuxFunc&0x02)==0x02) || ((AuxFunc&0x10) == 0x10) )	// ������ADC����DIDO�����˴����¿��䶼�����ʼ��
    {
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[0]=1;

        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_MultiFunc_514194;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[0]=1;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[1]=1;
        for(unsigned i =2; i<8; i++)
        {
            Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]=0;
        }
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[8]=1;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[9]=1;
        for(unsigned i =10; i<16; i++)
        {
            Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]=0;
        }
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID); //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = 50;  //10*50=500us
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 16;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_MultiFunc_514194_Base::Aux_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_MultiFunc_514194_Base::Aux_Read;

        Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = BoardID; //
        Aux_DataManager::m_DGIdx2BIdx[BoardID] = BoardID; //data_group index �� board ID ��ֵ

        //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

        //���� data_group ��ʼ��
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
        //���data_group ��Ч
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

        for(int i = 0; i< 16; i++) //�ð������16������ͨ�����Ҹð��޸���, ��ʵ��ֻ��4��ͨ����Ч
        {   //��������ͨ����Ϣ��AuxDataBank��ȥ���������е��ظ�������������
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
            //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
            Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;

            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
        }
    }

    if( ((AuxFunc&0x02)==0x02) && (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI !=0))  //��DI�����ݷ� 2-5
    {
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i+2]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i+2]. m_Chn_Info.m_AuxType = AUX_DI;

        }
        //        OS_ServeProcess::MountTask(Aux_ReadDI::TaskID,                  0,            Aux_ReadDI::Priority,                  1);
    }

    if(((AuxFunc&0x04)==0x04) &&( Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DO !=0) ) //��DO�����ݷ� 10-13
    {
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DO; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i+10]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i+10]. m_Chn_Info.m_AuxType = AUX_DO;

        }
        //        OS_ServeProcess::MountTask(Aux_WriteDO::TaskID,                  0,            Aux_WriteDO::Priority,                  1);
    }
    if(((AuxFunc&0x08)==0x08) &&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_CANBMS !=0))
    {
        Aux_DataManager::CANBMSEnableFlag=1;
        AuxCConfig::ReadContent_ForFRAM(&EthernetParse::m_btPackDataBuffer[0], FRAM_AUXCANBMS_MAP_BASE, FRAM_AUXCANBMS_MAP_LENGTH);
        MultiMcuComm::Scan_IV_CANBMSMAP();
    }

    if( ((AuxFunc&0x10)==0x10) && (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_UART !=0))  //��UART�����ݷŵ� 7-8
    {
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][6].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[0];   //copy  ͨ����Ϣ�� AuxDataBank
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][7].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[1];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][8].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[2];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][9].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[3];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][10].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[4];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][11].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[5];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][12].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[6];   //copy  ͨ����Ϣ�� AuxDataBank
				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][13].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[7];   //copy  ͨ����Ϣ�� AuxDataBank
			
			
        //				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][14].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[2];   //copy  ͨ����Ϣ�� AuxDataBank
        //				Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][15].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[3];   //copy  ͨ����Ϣ�� AuxDataBank
        //������Ӧ�ý��¿�������װ���������
        //���� OSCreateTaskForInit() ��װ

        UARTFunc::UARTInit( 0, TC_BR_9600, TC_DATA_BIT_8, TC_PARITY_NONE, TC_STOP_BIT_ONE );
        UARTFunc::UARTInit( 1, TC_BR_9600, TC_DATA_BIT_8, TC_PARITY_NONE, TC_STOP_BIT_ONE );
        UARTFunc::UARTInit( 2, TC_BR_9600, TC_DATA_BIT_8, TC_PARITY_NONE, TC_STOP_BIT_ONE );  //SMB I2C2�Ƿ�ռ��
        UARTFunc::UARTInit( 3, TC_BR_9600, TC_DATA_BIT_8, TC_PARITY_NONE, TC_STOP_BIT_ONE );
    }


    //Data_group arrangement for �๦�ܰ�

    //	   0  1 2  3  4  5   6 7   8 9 10 11 12 13 14 15
    //	   T  T DI DI DI DI  UART  V V DO DO DO DO  UART

}
void Aux_MultiFunc_514194_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
    Aux_ReadDI::m_InitDI = & Aux_MultiFunc_514194_Base::Init_DI;
    Aux_ReadDI::m_ReadDI = & Aux_MultiFunc_514194_Base::Read_DI;
    Aux_WriteDO::m_InitDO = & Aux_MultiFunc_514194_Base::Init_DO;
    Aux_WriteDO::m_WriteDO = & Aux_MultiFunc_514194_Base::Write_DO;
}

unsigned long Aux_MultiFunc_514194_Base::Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_MultiFunc_514194_Base::Aux_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET();
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //�����ݷŵ�Aux_DataBank ��
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //����Ѿ�����
    return 1;
}
void Aux_MultiFunc_514194_Base::Init_DI(void)
{
    //�ڲ�IO��ʼ��
    ControlPin::SetIObyCP(Pin_DI_1);
    ControlPin::SetIObyCP(Pin_DI_2);
    ControlPin::SetIObyCP(Pin_DI_3);
    ControlPin::SetIObyCP(Pin_DI_4);


}
void Aux_MultiFunc_514194_Base::Init_DO(void)
{
    ControlPin::SetIObyCP(Pin_DO_1);
    ControlPin::SetIObyCP(Pin_DO_2);
    ControlPin::SetIObyCP(Pin_DO_3);
    ControlPin::SetIObyCP(Pin_DO_4);

}

void Aux_MultiFunc_514194_Base::Read_DI(void)
{
    unsigned char result=0;
    for (unsigned char i=0; i<4; i++)
    {
        result = ControlPin::ReadMe(*Aux_MultiFunc_514194_Base::m_DIDO_MapMgr[i].m_pSt_DIPort);
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+2].aux_data = result;
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+2].updated = true;

        // Aux_DataManager::m_Aux_DIDO_DataBank[0][i].aux_data = result;  ////����ŵ�DIDO DataGroup 0ȥ
        // Aux_DataManager::m_Aux_DIDO_DataBank[0][i].updated = true;

    }

}

void Aux_MultiFunc_514194_Base::Write_DO(void)
{

    unsigned char result=0;

    //дDO�˿�
    for (unsigned char i=0; i<4; i++)
    {

        //if(Aux_DataManager::m_Aux_DIDO_DataBank[1][i].updated)
        if(Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+10].updated)

        {   //result = Aux_DataManager::m_Aux_DIDO_DataBank[1][i].aux_data;  //��DIDO DataGroup 1ȡ���ֵ
            result =  Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+10].aux_data ;

            if(result)
                ControlPin::SetMeHigh(*m_DIDO_MapMgr[i].m_pSt_DOPort);
            else
                ControlPin::SetMeLow(*m_DIDO_MapMgr[i].m_pSt_DOPort);

        }
    }

}
