///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2017/02/07
//Description:
//						辅助配置相关操作
//Others:      Yasong Niu
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

SSPI_Para Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC;
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_Conver		= {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_MUXWR		= {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD0	  	= {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD1	  	= {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD2	  	= {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD3	  	= {1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


unsigned short  Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base:: WaitForSampleCnt;

void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::IO_Init(void)
{

    ControlPin::SetIObyCP(Pin_Conver);
    ControlPin::SetIObyCP(Pin_MUXWR);
    ControlPin::SetIObyCP(Pin_VAD0);
    ControlPin::SetIObyCP(Pin_VAD1);
    ControlPin::SetIObyCP(Pin_VAD2);
    ControlPin::SetIObyCP(Pin_VAD3);


    // 	Adc24bit_ADS1259::Pin_Conver = Aux_WMADC_2ndV_LC_602528_Base::Pin_Conver;

}
void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::SPI_SET(void)
{
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_Module_Name = SSP1_DEVICE0;
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_Speed = 2;
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_CPOL = 0;
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_CPHA = 1;
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_MSTR = 1;    //master
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_BITS = 8;
    Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC.uc_LSBF = 0;   //ADS1259


    SSPIFunc::setPara(&Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::ADC_Init(unsigned char BoardID)
{
    unsigned short AdcSSP;
    Adc24bit_ADS1259::Pin_Conver = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_Conver;
    Adc24bit_ADS1259::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;
		if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType==Aux_WMADC_2ndV_VL_430834)
		{			
			Adc24bit_ADS1259::InitExtBase(eCONVCODETYPE_BIPOLAR,7);
		}
		else
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

    for(unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        SPI_SET();

        Aux_DataManager::SettingCS_High();
        ARBUS_DELAY(200);
        Aux_DataManager::SettingCS_Low(i);//(Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value);
        Adc24bit_ADS1259::WriteReg(1);
        Aux_DataManager::SettingCS_High();
    }
}
void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Func_Init(void)
{
    // 	for(unsigned char i=0;i<14;i++)
    // 	  Aux_WMADC_2ndV_LC_602528_Base::data[i]=0;
}
void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Hardware_Init(unsigned char BoardID)
{

    if (!Aux_DataManager::m_Inital_GPIO_4_LC_Boards)
    {
        IO_Init();
        Aux_DataManager::m_Inital_GPIO_4_LC_Boards = true;
    }

    ADC_Init(BoardID);

    for(unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_2ndV_LC_602528;
        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;

        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;

        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i; //1110 EOE~ECODE1; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = WaitForSampleCnt;  //10*45=450us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 16;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = NULL; // we are not going to use ISR sample  &Aux_WMADC_2ndV_LC_602528_Base::Aux_V_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = NULL;  // We are not going to use ISR sample &Aux_WMADC_2ndV_LC_602528_Base::Aux_V_Read;

        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx =  i;

        Aux_DataManager::m_DGIdx2BIdx[ i] = BoardID; //remember the board ID for the data_group index


        //获取通道信息并建立面板通道与电气通道的联系

        //分配 data_group 开始号
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //标记data_group 有效
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i] = true;
    }


    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }

    for(int i = 0; i< 64; i++) //该板型最多64个电气通道,实际最多只有32个通道有效，每个data group只有8个通道，最多占4个datagroup。
    {   //拷贝板上通道信息到AuxDataBank中去。
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
        //根据该通道populated 来确定ＡＤＣ通道是否有效　
        Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[i%16]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][i%16].m_Chn_Info.m_Populated;

        switch(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC)
        {
        case 1://只有一个主板

            if(i<16)
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_chan_id = i%16; //面板通道与电气通道同号
            }
            break;
        case 2:	//	一个主板带一个副板	 副板在后
            if(i<16)
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_chan_id = i%16;
            }
            else if((i<32)&&(i>=16))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_chan_id = i%16;
            }
            break;
        case 3: //	一个主板带两个副板	 副板在后
            if(i<16)
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_chan_id = i%16;
            }
            else if((i<32)&&(i>=16))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_chan_id = i%16;
            }
            else if((i<48)&&(i>=32))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i%16].e_chan_id = i%16;
            }
            break;
        case 4://	一个主板带三个副板	 副板在后
            if(i<16)
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+0][i%16].e_chan_id = i%16;
            }
            else if((i<32)&&(i>=16))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i%16].e_chan_id = i%16;
            }
            else if((i<48)&&(i>=32))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i%16].e_chan_id = i%16;
            }
            else if((i<64)&&(i>=48))
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i%16].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i%16].e_chan_id = i%16;
            }
            break;
        default:
            break;
        }
    }

}
void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
    Aux_Sample_2ndV_LC::Init();	//20151216
    Aux_Sample_2ndV_LC::Board_Adc_Trigger = &Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_V_Trigger;//AD1259 trigger
    Aux_Sample_2ndV_LC::Board_Adc_Read = &Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_V_Read;//AD1259 read
    Aux_Sample_2ndV_LC::Pin_MUXWR = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_MUXWR;
    Aux_Sample_2ndV_LC::Pin_VAD0 = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD0;
    Aux_Sample_2ndV_LC::Pin_VAD1 = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD1;
    Aux_Sample_2ndV_LC::Pin_VAD2 = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD2;
    Aux_Sample_2ndV_LC::Pin_VAD3 = Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Pin_VAD3;
}

void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_V_Trigger(void)
{
    SPI_SET();
    Adc24bit_ADS1259::Convert();
}

void Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
//	unsigned long temp;
    //高共模每个ADC只有一个通道，所以数据是按片选地址当通道地址，16个片选转换成16个通道
    SPI_SET();

    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc24bit_ADS1259::Read(1); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //标记已经更新

}
