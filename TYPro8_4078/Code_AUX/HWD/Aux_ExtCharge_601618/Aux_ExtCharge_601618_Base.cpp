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

SSPI_Para Aux_ExtCharge_601618_Base::Aux_ExtCharge_601618_SSP1ADC;
SSPI_Para Aux_ExtCharge_601618_Base::Aux_ExtCharge_601618_SSP0ADC;
//DI DO Module
//DI
const St_Port Aux_ExtCharge_601618_Base::Pin_DI_1      = {1,        2,        eCPTYPE_MCUGPIO,   GPIO_I,             0};


//DO
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_1     = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_2     = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_3     = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_4     = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_5     = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_6     = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_7     = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_DO_8     = {1,        29,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_KP       = {2,        2,         eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_ExtCharge_601618_Base::Pin_ADCBUSY_T = {2,        13,         eCPTYPE_MCUGPIO,   GPIO_I,             0};
//temp one board
St_DOMapMgr  Aux_ExtCharge_601618_Base::m_ExChargeDO_MapMgr[8] = {
    0,&Pin_DO_1,
    1,&Pin_DO_2,
    2,&Pin_DO_3,
    3,&Pin_DO_4,
    4,&Pin_DO_5,
    5,&Pin_DO_6,
    6,&Pin_DO_7,
    7,&Pin_DO_8,

};
MsTime       Aux_ExtCharge_601618_Base::StartTime;
MsTime       Aux_ExtCharge_601618_Base::TimePassed;
unsigned char    Aux_ExtCharge_601618_Base::SafetyStatus;

unsigned char Aux_ExtCharge_601618_Base::My_BoardID =0;

long Aux_ExtCharge_601618_Base::data[14];

void Aux_ExtCharge_601618_Base::IO_Init(void)
{
    //There is no speacial port for this board
    // 	ControlPin::SetIObyCP(Pin_Conver);
    ControlPin::SetIObyCP(Pin_DI_1);

    ControlPin::SetIObyCP(Pin_DO_1);
    ControlPin::SetIObyCP(Pin_DO_2);
    ControlPin::SetIObyCP(Pin_DO_3);
    ControlPin::SetIObyCP(Pin_DO_4);
    ControlPin::SetIObyCP(Pin_DO_5);
    ControlPin::SetIObyCP(Pin_DO_6);
    ControlPin::SetIObyCP(Pin_DO_7);
    ControlPin::SetIObyCP(Pin_DO_8);
    ControlPin::SetIObyCP(Pin_KP);

    ControlPin::SetMeLow(Pin_DO_1);
    ControlPin::SetMeLow(Pin_DO_2);
    ControlPin::SetMeLow(Pin_DO_3);
    ControlPin::SetMeLow(Pin_DO_4);
    ControlPin::SetMeLow(Pin_DO_5);
    ControlPin::SetMeLow(Pin_DO_6);
    ControlPin::SetMeLow(Pin_DO_7);
    ControlPin::SetMeLow(Pin_DO_8);
    ControlPin::SetMeHigh(Pin_KP);

    ControlPin::SetIObyCP(Pin_ADCBUSY_T);

}
void Aux_ExtCharge_601618_Base::SPI_SET(unsigned char spiChn)
{
    switch (spiChn)
    {

    case  0:
    {

        Aux_ExtCharge_601618_SSP0ADC.uc_Module_Name = SSP0_DEVICE1;
        Aux_ExtCharge_601618_SSP0ADC.uc_Speed = 2;
        Aux_ExtCharge_601618_SSP0ADC.uc_CPOL = 0;
        Aux_ExtCharge_601618_SSP0ADC.uc_CPHA = 0;
        Aux_ExtCharge_601618_SSP0ADC.uc_MSTR = 1;    //master
        Aux_ExtCharge_601618_SSP0ADC.uc_BITS = 8;
        Aux_ExtCharge_601618_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp
        SSPIFunc::setPara(&Aux_ExtCharge_601618_Base::Aux_ExtCharge_601618_SSP0ADC,0);
        SSPIFunc::my_Init(0);

    }
    break;

    case 1:
    {
        Aux_ExtCharge_601618_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
        Aux_ExtCharge_601618_SSP1ADC.uc_Speed = 2;
        Aux_ExtCharge_601618_SSP1ADC.uc_CPOL = 0;
        Aux_ExtCharge_601618_SSP1ADC.uc_CPHA = 0;
        Aux_ExtCharge_601618_SSP1ADC.uc_MSTR = 1;    //master
        Aux_ExtCharge_601618_SSP1ADC.uc_BITS = 8;
        Aux_ExtCharge_601618_SSP1ADC.uc_LSBF = 0;   //LTC2449

        SSPIFunc::setPara(&Aux_ExtCharge_601618_Base::Aux_ExtCharge_601618_SSP1ADC,1);
        SSPIFunc::my_Init(1);
    }
    break;

    default:
        break;
    }


}
void Aux_ExtCharge_601618_Base::ADC_Init(void)
{
    Adc32bit_LTC2449::Pin_CS  = AUX_Unit_Base::Pin_ECODE1;
    Adc32bit_LTC2449::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;

    Adc32bit_LTC2498::Pin_Busy = Aux_ExtCharge_601618_Base::Pin_ADCBUSY_T;

}
void Aux_ExtCharge_601618_Base::Func_Init(void)
{
    for(unsigned char i=0; i<14; i++)
        Aux_ExtCharge_601618_Base::data[i]=0;
}
void Aux_ExtCharge_601618_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;
    SafetyStatus=0;
    Aux_Safety_Check::EmergencyStopCheckEnable =1;
    IO_Init();
    //      Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //�˴�����������Ϣ���£����һ��ADC  LTC 2449
    ADC_Init();

    My_BoardID = BoardID;   //��¼�Լ���BoardID��
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    if( ((AuxFunc&0x01)==0x01)&& (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC !=0) )	// ������ADC
    {
        for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
        {
            Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
            Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_ExtCharge_601618;

            Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
            Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
            Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
            Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;
            Aux_DataManager::m_DGIdx2BIdx[i] = BoardID; //data_group index 0,�� 1 ��Ӧ��  board ID
        }

        Aux_DataManager::Aux_ADC_CS_Info[0].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_CS_Address.m_Value = 0x0E; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[0].m_Wait_Count = 10;  //10*50=500us
        Aux_DataManager::Aux_ADC_CS_Info[0].m_ADCCH_MaxCnt = 16;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger =  &Aux_ExtCharge_601618_Base::Aux_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_ExtCharge_601618_Base::Aux_Read;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx = 0;
// 		Aux_DataManager::m_DGIdx2BIdx[0] = 0; //1807131

        Aux_DataManager::Aux_ADC_CS_Info[1].sspChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[1].m_CS_Address.m_Value = 0x0D;  //1101 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[1].m_Wait_Count = 4000;  //4000*50=200ms
        Aux_DataManager::Aux_ADC_CS_Info[1].m_ADCCH_MaxCnt = 17;
        Aux_DataManager::Aux_ADC_CS_Info[1].b_ADCCH_Valid[16]=1; //��֤�����¶�
        Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Trigger = &Aux_ExtCharge_601618_Base::Aux_T_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Rd = &Aux_ExtCharge_601618_Base::Aux_T_Read;
        Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx = 1;
// 		Aux_DataManager::m_DGIdx2BIdx[1] = 1; //data_group index �� board ID ��ֵ//1807131


        //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

        //���� data_group ��ʼ��
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //���data_group ��Ч
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1] = true;

        for(unsigned char i = 0; i< 32; i++) //�ð������32������ͨ��,ǰ16��ͨ���ǵ�ѹ����16�����¶�
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
            //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
            Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[(i%16)]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info.m_Populated;
        }

        for(int i = 0; i< 16; i++) //�ð������16������ͨ�����Ҹð��޸���, ��ʵ�ʿ�������Ϊ16ͨ����12ͨ��
        {   //��������ͨ����Ϣ��AuxDataBank��ȥ.
            // 			     Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
            // 				   //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
            // 				   Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;
            //
            // 					 Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            // 					 Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = i;
        }
        //               for(int j =0;j<8;j++)
        //               {
        //                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j*2].e_chan_id = j;
        //                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j*2+1].e_chan_id = j;
        //                }
    }

}
void Aux_ExtCharge_601618_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
    // 	Aux_ReadDI::m_InitDI = & Aux_ExtCharge_601618_Base::Init_DI;
    // 	Aux_ReadDI::m_ReadDI = & Aux_ExtCharge_601618_Base::Read_DI;
}

unsigned long Aux_ExtCharge_601618_Base::Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_ExtCharge_601618_Base::Aux_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //�����ݷŵ�Aux_DataBank ��
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx][ADCchn].updated = true; //����Ѿ�����
    return 1;
}

//T�¶ȣ��������¶ȿ�����T�ͣ�Ҳ������K��
unsigned long Aux_ExtCharge_601618_Base::Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    //ֱ�ӵ���LTC2498
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    // 	  Adc32bit_LTC2498::Pin_CS = AUX_Unit_Base::Pin_ECODE2;
    Adc32bit_LTC2498::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_ExtCharge_601618_Base::Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn)
{
    unsigned long temp;
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    temp=Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    if(temp==0)
        return 0;
    else
    {
        if (ADCchn>=16)
        {
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx].InChipTemp_instant_code= temp ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx].updated= true ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������

        }
        else
        {   Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx ][ADCchn].aux_instant_code =temp; // Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);//�����ݷŵ�Aux_DataBank ��, 2498�����ŵ�16-31
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx ][ADCchn].updated = true; // ����Ѿ�����
        }
        return 1;
    }
}
// void Aux_ExtCharge_601618_Base::Init_DI(void)
// {
//�ڲ�IO��ʼ��
//    ControlPin::SetIObyCP(Pin_DI_1);
// 	 ControlPin::SetIObyCP(Pin_DI_2);
// 	 ControlPin::SetIObyCP(Pin_DI_3);
// 	 ControlPin::SetIObyCP(Pin_DI_4);


// }

// void Aux_ExtCharge_601618_Base::Read_DI(void)
// {
//   unsigned char result=0;
// 	for (unsigned char i=0;i<4;i++)
// 	 {
//      result = ControlPin::ReadMe(*Aux_ExtCharge_601618_Base::m_DIDO_MapMgr[i].m_pSt_DIPort);
//      Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+2].aux_data = result;
// 		 Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i+2].updated = true;
//
// 		// Aux_DataManager::m_Aux_DIDO_DataBank[0][i].aux_data = result;  ////����ŵ�DIDO DataGroup 0ȥ
// 		// Aux_DataManager::m_Aux_DIDO_DataBank[0][i].updated = true;
//
//    }

// }


void Aux_ExtCharge_601618_Base::Set_ExtCharge_Relay(unsigned int value, unsigned char uc_chn)
{
    // 	  bool bIO = value &(1<<uc_chn);
    if (value==1)
        ControlPin::SetMeHigh(*m_ExChargeDO_MapMgr[uc_chn].m_pSt_DOPort);
    else
        ControlPin::SetMeLow(*m_ExChargeDO_MapMgr[uc_chn].m_pSt_DOPort);
}

char Aux_ExtCharge_601618_Base::EmergencyStop_Check(void)
{
    unsigned char value,temp;

    temp = ControlPin::ReadMe(Pin_DI_1);
    switch(SafetyStatus)
    {
    case 0:
        if(!temp)
        {
            SafetyStatus = 1;
            StartTime = CommonFunc::GetSystemTime();//OS_Time;
        }
        TimePassed.Us100 =0;
        value=1;
        break;
    case 1:
        if(temp)
        {
            SafetyStatus = 0;
        }
        else
        {
            TimePassed = CommonFunc::TimeDiff(StartTime);
            if(TimePassed.Us100 > 3000) //300ms
            {
                SafetyStatus =2;
                // 								ControlPin::SetMeLow(Pin_KP);
            }
        }
        value =1;
        break;
    case 2:
        if(temp)
        {
            SafetyStatus = 3;
            StartTime = CommonFunc::GetSystemTime();//OS_Time;
        }
        TimePassed.Us100=0;
        value = 0;
        break;
    case 3:
        if(temp)
        {
            TimePassed = CommonFunc::TimeDiff(StartTime);
            if(TimePassed.Us100 > 3000) //300ms
            {
                SafetyStatus =0;
                // 								ControlPin::SetMeHigh(Pin_KP,1);
            }
        }
        value=0;
    default:
        value = 1;
    }
    return value;
}

void Aux_ExtCharge_601618_Base::EmergencyStop_Set(unsigned char value)
{
    if(value==1)
    {
        ControlPin::SetMeHigh(Pin_KP);
    }
    else
    {
        ControlPin::SetMeLow(Pin_KP);
    }
}
