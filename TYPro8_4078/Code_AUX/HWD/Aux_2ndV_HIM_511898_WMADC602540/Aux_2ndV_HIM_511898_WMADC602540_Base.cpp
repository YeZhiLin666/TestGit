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
    // 	SSPIFunc::my_Init(1);             //初始化
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
    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=16;  //14 //暂时定义16，以后考虑根据上位机下的信息更新
    ADC_Init();
    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid = Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i].m_Populated;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_2ndV_HIR_602540;
        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        //根据该通道populated 来确定ＡＤＣ通道是否有效　
        Aux_DataManager::Aux_ADC_CS_Info[i].b_ADCCH_Valid[0]=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i].m_Populated;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;

        Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = i; //1110 EOE~ECODE1
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = WaitForSampleCnt;  //10*45=450us
        Aux_DataManager::Aux_ADC_CS_Info[i].m_ADCCH_MaxCnt = 1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Trigger = &Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Trigger;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardADC_Rd = &Aux_2ndV_HIR_511898_WMADC602540_Base::Aux_V_Read;

        Aux_DataManager::Aux_ADC_CS_Info[i].m_data_group_idx = 0; //一个辅助系统里只有一块高共模，最多16个通道，因此data group index 均为0
    }

    Aux_DataManager::m_DGIdx2BIdx[0] = BoardID; //data_group index =0  的 board ID


    //获取通道信息并建立面板通道与电气通道的联系

    //分配 data_group 开始号  //1个AUXMCU下只有一块高工模的二次电压直接放到 data_group_idx =0
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //标记data_group 有效
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //该板型最多16个电气通道
    {   //拷贝板上通道信息到AuxDataBank中去。好像是有点重复，但先这样吧
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //对热电偶板，若带副板的话，要实行转换.其他板无副板的可能。
        {
            if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_SlaveGoesFirst)
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
        else
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号
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
    //高共模每个ADC只有一个通道，所以数据是按片选地址当通道地址，16个片选转换成16个通道
    SPI_SET();

    // Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].aux_instant_code = Adc24bit_ADS1259::Read(1,u_CSid); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].aux_instant_code = Adc24bit_ADS1259::Read(1); //将数据放到Aux_DataBank 中

    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][u_CSid].updated = true; //标记已经更新
    return 1;

}
