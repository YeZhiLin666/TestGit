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
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
SSPI_Para Aux_AIO_DIO_602178_WMADC602548_Base::Aux_AIO_DIO_602178_WMADC602548_SSP0ADC;
SSPI_Para Aux_AIO_DIO_602178_WMADC602548_Base::Aux_AIO_DIO_602178_WMADC602548_SSP1ADC;
#pragma arm section //��������������
//DI DO Module
//DI
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_1      = {3,        16,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_2      = {3,        17,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_3      = {3,        18,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_4      = {3,        19,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_5      = {3,        20,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_6      = {3,        21,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_7      = {3,        22,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DI_8      = {3,        23,        eCPTYPE_MCUGPIO,   GPIO_I,             0};

//DO
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_1     = {2,        14,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_2     = {2,        15,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_3     = {2,        22,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_4     = {2,        23,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_5     = {2,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_6     = {2,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_7     = {2,        30,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DO_8     = {2,        31,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
//DAC AD5754
const 	St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacBIN2COMP = {2,        4,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacLoad = {2,        7,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCLR = {2,        9,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS1 = {2,        2,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS2 = {2,        3,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
//temp one board
St_DIDOMapMgr  Aux_AIO_DIO_602178_WMADC602548_Base::m_DIDO_MapMgr[8] = {
    0,0,&Pin_DI_1,&Pin_DO_1,
    1,1,&Pin_DI_2,&Pin_DO_2,
    2,2,&Pin_DI_3,&Pin_DO_3,
    3,3,&Pin_DI_4,&Pin_DO_4,
    4,4,&Pin_DI_5,&Pin_DO_5,
    5,5,&Pin_DI_6,&Pin_DO_6,
    6,6,&Pin_DI_7,&Pin_DO_7,
    7,7,&Pin_DI_8,&Pin_DO_8,
};

unsigned char Aux_AIO_DIO_602178_WMADC602548_Base::My_BoardID =0;

long Aux_AIO_DIO_602178_WMADC602548_Base::data[14];

void Aux_AIO_DIO_602178_WMADC602548_Base::IO_Init(void)
{
    ControlPin::SetIObyCP(Pin_DacBIN2COMP);
    ControlPin::SetIObyCP(Pin_DacLoad);
    ControlPin::SetIObyCP(Pin_DacCLR);
    ControlPin::SetIObyCP(Pin_DacCS1);
    ControlPin::SetIObyCP(Pin_DacCS2);
// 	ControlPin::SetIObyCP(Pin_DI_4);
// 	ControlPin::SetIObyCP(Pin_DI_5);
// 	ControlPin::SetIObyCP(Pin_DI_6);
// 	ControlPin::SetIObyCP(Pin_DI_7);
// 	ControlPin::SetIObyCP(Pin_DI_8);
//
// 	ControlPin::SetIObyCP(Pin_DO_1);
// 	ControlPin::SetIObyCP(Pin_DO_2);
// 	ControlPin::SetIObyCP(Pin_DO_3);
// 	ControlPin::SetIObyCP(Pin_DO_4);
// 	ControlPin::SetIObyCP(Pin_DO_5);
// 	ControlPin::SetIObyCP(Pin_DO_6);
// 	ControlPin::SetIObyCP(Pin_DO_7);
//  	ControlPin::SetIObyCP(Pin_DO_8);

}
void Aux_AIO_DIO_602178_WMADC602548_Base::SPI_SET(unsigned char spiChn)
{
    switch (spiChn)
    {

    case  0:
    {
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_Module_Name = SSP0_DEVICE3;
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_Speed = 7;
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_CPOL = 0;
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_CPHA = 1;
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_MSTR = 1;    //master
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_BITS = 8;
        Aux_AIO_DIO_602178_WMADC602548_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp
        SSPIFunc::setPara(&Aux_AIO_DIO_602178_WMADC602548_Base::Aux_AIO_DIO_602178_WMADC602548_SSP0ADC,0);
        SSPIFunc::my_Init(0);
    }
    break;
    case 1:
    {
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_Speed = 2;
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_CPOL = 0;
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_CPHA = 0;
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_MSTR = 1;    //master
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_BITS = 8;
        Aux_AIO_DIO_602178_WMADC602548_SSP1ADC.uc_LSBF = 0;   //LTC2449
        SSPIFunc::setPara(&Aux_AIO_DIO_602178_WMADC602548_Base::Aux_AIO_DIO_602178_WMADC602548_SSP1ADC,1);
        SSPIFunc::my_Init(1);
    }
    break;
    default:
        break;
    }
}
void Aux_AIO_DIO_602178_WMADC602548_Base::ADC_Init(void)
{
    Adc32bit_LTC2449::Pin_CS  = AUX_Unit_Base::Pin_ECODE1;
    Adc32bit_LTC2449::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;

}

void Aux_AIO_DIO_602178_WMADC602548_Base::DAC_Init(void)
{
    //AD5754
    DAC16bit_AD5754::Pin_DacBin = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacBIN2COMP;
// 	DAC16bit_AD5754::Pin_DacCS = AUX_Unit_Base::Pin_EOE;
    DAC16bit_AD5754::Pin_DacLoad =Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacLoad;
    DAC16bit_AD5754::Pin_DacClr = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCLR;
    DAC16bit_AD5754::SPI_Idx = 0;

    //��ʼ����power up AD5754
    SPI_SET(0);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP
}
void Aux_AIO_DIO_602178_WMADC602548_Base::Func_Init(void)
{
// 	for(unsigned char i=0;i<14;i++)
// 		Aux_AIO_DIO_602178_WMADC602548_Base::data[i]=0;
}
void Aux_AIO_DIO_602178_WMADC602548_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;
    IO_Init();
    CANBusParse::DIDO_Ctrl=2;

    ADC_Init();
    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }
//   Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //�˴�����������Ϣ���£����һ��ADC  LTC 2449
    My_BoardID = BoardID;   //��¼�Լ���BoardID��
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    //if( (((AuxFunc&0x01)==0x01)&& (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC !=0)) || ((AuxFunc&0x10) == 0x10) ) // ������ADC���������˴����¿��䶼�����ʼ��
    if( ((AuxFunc&0x01)==0x01)&&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC!=0 )	)// ������ADC
    {

        Aux_DataManager::Aux_ADC_CS_Info[0].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardType = Aux_WMADC_AIO_DIO_602548;

        Aux_DataManager::Aux_ADC_CS_Info[0].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[0].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_StateMachine = AUX_Sample_IDLE;

        Aux_DataManager::Aux_ADC_CS_Info[0].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_CS_Address.m_Value = 0x00;//    0010 EOE~ECODE1   0x0E; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[0].m_Wait_Count = 10;  //10*50=500us
        Aux_DataManager::Aux_ADC_CS_Info[0].m_ADCCH_MaxCnt = 8;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger =  &Aux_AIO_DIO_602178_WMADC602548_Base::Aux_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_AIO_DIO_602178_WMADC602548_Base::Aux_Read;
        Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx = 0;
        Aux_DataManager::m_DGIdx2BIdx[0] = BoardID; //1807131
        Aux_DataManager::m_DGIdx2BIdx[1] = BoardID;
        Aux_DataManager::m_DGIdx2BIdx[2] = BoardID;
        Aux_DataManager::m_DGIdx2BIdx[3] = BoardID;
        //��ȡͨ����Ϣ���������ͨ�������ͨ������ϵ

        //���� data_group ��ʼ��
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //���data_group ��Ч
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

        for(int i = 0; i< 8; i++) //�ð������8������ͨ�����Ҹð�߱������ṹ
        {   //��������ͨ����Ϣ��AuxDataBank��ȥ��
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
            //���ݸ�ͨ��populated ��ȷ�����ģ�ͨ���Ƿ���Ч��
            Aux_DataManager::Aux_ADC_CS_Info[0].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //���ȵ�ż�壬��������Ļ���Ҫʵ��ת��.�������޸���Ŀ���?
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 4+i; //���ͨ�������ͨ��ͬ��

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i-4; //���ͨ�������ͨ��ͬ��
                }
            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            }
        }
    }

    if( ((AuxFunc&0x02)==0x02) && (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI !=0))  //��DI�����ݷ� 2-5
    {
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i]. m_Chn_Info.m_AuxType = AUX_DI;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //����������Ļ���Ҫʵ��ת��.?
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = 4+i; //���ͨ�������ͨ��ͬ��

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = i-4; //���ͨ�������ͨ��ͬ��
                }

            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            }
        }
    }

    if(((AuxFunc&0x04)==0x04) &&( Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DO !=0) ) //��DO�����ݷ� 10-13
    {
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DO; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i]. m_Chn_Info.m_AuxType = AUX_DO;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //����������Ļ���Ҫʵ��ת��.?
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_chan_id = 4+i; //���ͨ�������ͨ��ͬ��

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_chan_id = i-4; //���ͨ�������ͨ��ͬ��
                }

            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            }
        }
        //        OS_ServeProcess::MountTask(Aux_ReadDI::TaskID,                  0,            Aux_ReadDI::Priority,                  1);
    }
    if(((AuxFunc&0x80)==0x80) &&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC !=0))  //������DACͨ��
    {
        if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC<=4)
        {
            DAC16bit_AD5754::Pin_DacCS = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS1;
            DAC_Init();
        }
        else
        {
            DAC16bit_AD5754::Pin_DacCS = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS1;
            DAC_Init();
            DAC16bit_AD5754::Pin_DacCS = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS2;
            DAC_Init();
        }
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_DAC_chn[i];   //copy  ͨ����Ϣ�� AuxDataBank
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i]. m_Chn_Info.m_AuxType = AUX_AO;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //����������Ļ���Ҫʵ��ת��.?
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_chan_id = 4+i; //���ͨ�������ͨ��ͬ��

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_chan_id = i-4; //���ͨ�������ͨ��ͬ��
                }

            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].e_chan_id = i; //���ͨ�������ͨ��ͬ��
            }
        }
    }


}
void Aux_AIO_DIO_602178_WMADC602548_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
    Aux_ReadDI::m_InitDI = & Aux_AIO_DIO_602178_WMADC602548_Base::Init_DI;
    Aux_ReadDI::m_ReadDI = & Aux_AIO_DIO_602178_WMADC602548_Base::Read_DI;
    Aux_WriteDO::m_InitDO = & Aux_AIO_DIO_602178_WMADC602548_Base::Init_DO;
    Aux_WriteDO::m_WriteDO = & Aux_AIO_DIO_602178_WMADC602548_Base::Write_DO;
    board_config::Func_AUX_Dac_Out = & Aux_AIO_DIO_602178_WMADC602548_Base::Dac_out;
}

unsigned long Aux_AIO_DIO_602178_WMADC602548_Base::Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET(1);
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_AIO_DIO_602178_WMADC602548_Base::Aux_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET(1);
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //�����ݷŵ�Aux_DataBank ��
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //����Ѿ�����
    return 1;
}
void Aux_AIO_DIO_602178_WMADC602548_Base::Init_DI(void)
{
    //�ڲ�IO��ʼ��
    ControlPin::SetIObyCP(Pin_DI_1);
    ControlPin::SetIObyCP(Pin_DI_2);
    ControlPin::SetIObyCP(Pin_DI_3);
    ControlPin::SetIObyCP(Pin_DI_4);
    ControlPin::SetIObyCP(Pin_DI_5);
    ControlPin::SetIObyCP(Pin_DI_6);
    ControlPin::SetIObyCP(Pin_DI_7);
    ControlPin::SetIObyCP(Pin_DI_8);

    ControlPin::SetMeLow(Pin_DI_1);
    ControlPin::SetMeLow(Pin_DI_2);
    ControlPin::SetMeLow(Pin_DI_3);
    ControlPin::SetMeLow(Pin_DI_4);
    ControlPin::SetMeLow(Pin_DI_5);
    ControlPin::SetMeLow(Pin_DI_6);
    ControlPin::SetMeLow(Pin_DI_7);
    ControlPin::SetMeLow(Pin_DI_8); //Ĭ��DI �ϵ�ΪClose ״̬

}
void Aux_AIO_DIO_602178_WMADC602548_Base::Init_DO(void)
{
    ControlPin::SetIObyCP(Pin_DO_1);
    ControlPin::SetIObyCP(Pin_DO_2);
    ControlPin::SetIObyCP(Pin_DO_3);
    ControlPin::SetIObyCP(Pin_DO_4);
    ControlPin::SetIObyCP(Pin_DO_5);
    ControlPin::SetIObyCP(Pin_DO_6);
    ControlPin::SetIObyCP(Pin_DO_7);
    ControlPin::SetIObyCP(Pin_DO_8);

    ControlPin::SetMeHigh(Pin_DO_1);
    ControlPin::SetMeHigh(Pin_DO_2);
    ControlPin::SetMeHigh(Pin_DO_3);
    ControlPin::SetMeHigh(Pin_DO_4);
    ControlPin::SetMeHigh(Pin_DO_5);
    ControlPin::SetMeHigh(Pin_DO_6);
    ControlPin::SetMeHigh(Pin_DO_7);
    ControlPin::SetMeHigh(Pin_DO_8);  //Ĭ��DO �ϵ�ΪClose ״̬
}

void Aux_AIO_DIO_602178_WMADC602548_Base::Read_DI(void)
{
    unsigned char result=0;
    for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[My_BoardID].m_BoardInfo.m_abi_globe.m_Total_DI; i++)
    {
        result = ControlPin::ReadMe(*Aux_AIO_DIO_602178_WMADC602548_Base::m_DIDO_MapMgr[i].m_pSt_DIPort);
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start+1][i].aux_data = result;
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start+1][i].updated = true;

    }

}

void Aux_AIO_DIO_602178_WMADC602548_Base::Write_DO(void)
{
    unsigned char result=0;

    //дDO�˿�
    for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[My_BoardID].m_BoardInfo.m_abi_globe.m_Total_DO; i++)
    {
        if(Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start+2][i].updated)
        {
            result =  Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start+2][i].aux_data ;

            if(result)
                ControlPin::SetMeHigh(*m_DIDO_MapMgr[i].m_pSt_DOPort);
            else
                ControlPin::SetMeLow(*m_DIDO_MapMgr[i].m_pSt_DOPort);
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start+2][i].updated=0;
        }
        else
            continue;
    }

}
void Aux_AIO_DIO_602178_WMADC602548_Base::Dac_out(unsigned char chn,unsigned long value)
{
    SPI_SET(0);
    if(chn<4)
    {
// 		   Aux_DataManager::SettingCS_Low(0x01);//��ʼ��DAC1
        DAC16bit_AD5754::Pin_DacCS = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS1;
    }
    else
    {
// //        Aux_DataManager::SettingCS_Low(0x02);//��ʼ��DAC1
        DAC16bit_AD5754::Pin_DacCS = Aux_AIO_DIO_602178_WMADC602548_Base::Pin_DacCS2;
        chn-=4;
    }
    DAC16bit_AD5754::Dac_Out(chn,value);
}
