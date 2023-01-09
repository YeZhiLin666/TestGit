///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助配置相关操作
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

SSPI_Para Aux_2ndV_LC_601310_Base::Aux_2ndV_LC_601310_SSP1ADC;
const St_Port Aux_2ndV_LC_601310_Base::Pin_Conver		= {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_2ndV_LC_601310_Base::Pin_MUXWR		= {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_2ndV_LC_601310_Base::Pin_VAD0	  	= {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_2ndV_LC_601310_Base::Pin_VAD1	  	= {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_2ndV_LC_601310_Base::Pin_VAD2	  	= {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_2ndV_LC_601310_Base::Pin_VAD3	  	= {1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


unsigned short  Aux_2ndV_LC_601310_Base:: WaitForSampleCnt;

void Aux_2ndV_LC_601310_Base::IO_Init(void)
{

    ControlPin::SetIObyCP(Pin_Conver);
    ControlPin::SetIObyCP(Pin_MUXWR);
    ControlPin::SetIObyCP(Pin_VAD0);
    ControlPin::SetIObyCP(Pin_VAD1);
    ControlPin::SetIObyCP(Pin_VAD2);
    ControlPin::SetIObyCP(Pin_VAD3);


    // 	Adc24bit_ADS1259::Pin_Conver = Aux_2ndV_LC_601310_Base::Pin_Conver;

}
void Aux_2ndV_LC_601310_Base::SPI_SET(void)
{
    Aux_2ndV_LC_601310_SSP1ADC.uc_Module_Name = SSP1_DEVICE0;
    Aux_2ndV_LC_601310_SSP1ADC.uc_Speed = 2;
    Aux_2ndV_LC_601310_SSP1ADC.uc_CPOL = 0;
    Aux_2ndV_LC_601310_SSP1ADC.uc_CPHA = 1;
    Aux_2ndV_LC_601310_SSP1ADC.uc_MSTR = 1;    //master
    Aux_2ndV_LC_601310_SSP1ADC.uc_BITS = 8;
    Aux_2ndV_LC_601310_SSP1ADC.uc_LSBF = 0;   //ADS1259


    SSPIFunc::setPara(&Aux_2ndV_LC_601310_Base::Aux_2ndV_LC_601310_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_2ndV_LC_601310_Base::ADC_Init(unsigned char BoardID)
{
    unsigned short AdcSSP;
    Adc24bit_ADS1259::Pin_Conver = Aux_2ndV_LC_601310_Base::Pin_Conver;
    Adc24bit_ADS1259::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,7);	// SPS=14400
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

    SPI_SET();

    Aux_DataManager::SettingCS_High();
    ARBUS_DELAY(20);
    Aux_DataManager::SettingCS_Low(7-BoardID);//(Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value);
    Adc24bit_ADS1259::WriteReg(1);
    Aux_DataManager::SettingCS_High();

}
void Aux_2ndV_LC_601310_Base::Func_Init(void)
{
    // 	for(unsigned char i=0;i<14;i++)
    // 	  Aux_2ndV_LC_601310_Base::data[i]=0;
}
void Aux_2ndV_LC_601310_Base::Hardware_Init(unsigned char BoardID)
{

    if (!Aux_DataManager::m_Inital_GPIO_4_LC_Boards)
    {
        IO_Init();
        Aux_DataManager::m_Inital_GPIO_4_LC_Boards = true;
    }

    ADC_Init(BoardID);

    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //For LC boards, only one ADC per board

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_2ndV_LC_601310;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID); //1110 EOE~ECODE1; //1110 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = WaitForSampleCnt;  //10*45=450us
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 16;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = NULL; // we are not going to use ISR sample  &Aux_2ndV_LC_601310_Base::Aux_V_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = NULL;  // We are not going to use ISR sample &Aux_2ndV_LC_601310_Base::Aux_V_Read;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx =  BoardID;

    Aux_DataManager::m_DGIdx2BIdx[ BoardID] = BoardID; //remember the board ID for the data_group index


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

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //对热电偶板,高共模板，若带副板的话，要实行转换.其他板无副板的可能。
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
void Aux_2ndV_LC_601310_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
    Aux_Sample_2ndV_LC::Init();	//20151216
    Aux_Sample_2ndV_LC::Board_Adc_Trigger = &Aux_2ndV_LC_601310_Base::Aux_V_Trigger;//AD1259 trigger
    Aux_Sample_2ndV_LC::Board_Adc_Read = &Aux_2ndV_LC_601310_Base::Aux_V_Read;//AD1259 read
    Aux_Sample_2ndV_LC::Pin_MUXWR = Aux_2ndV_LC_601310_Base::Pin_MUXWR;
    Aux_Sample_2ndV_LC::Pin_VAD0 = Aux_2ndV_LC_601310_Base::Pin_VAD0;
    Aux_Sample_2ndV_LC::Pin_VAD1 = Aux_2ndV_LC_601310_Base::Pin_VAD1;
    Aux_Sample_2ndV_LC::Pin_VAD2 = Aux_2ndV_LC_601310_Base::Pin_VAD2;
    Aux_Sample_2ndV_LC::Pin_VAD3 = Aux_2ndV_LC_601310_Base::Pin_VAD3;
}

void Aux_2ndV_LC_601310_Base::Aux_V_Trigger(void)
{
    SPI_SET();
    Adc24bit_ADS1259::Convert();
}

void Aux_2ndV_LC_601310_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
//	unsigned long temp;
    //高共模每个ADC只有一个通道，所以数据是按片选地址当通道地址，16个片选转换成16个通道
    SPI_SET();

    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc24bit_ADS1259::Read(1); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //标记已经更新

}
