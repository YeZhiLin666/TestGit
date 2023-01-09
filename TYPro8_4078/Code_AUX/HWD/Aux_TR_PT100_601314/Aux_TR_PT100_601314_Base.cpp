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

SSPI_Para Aux_TR_PT100_601314_Base::Aux_TR_PT100_601314_SSP1ADC;

void Aux_TR_PT100_601314_Base::IO_Init(void)
{
    //There is no speacial port for this board


}
void Aux_TR_PT100_601314_Base::SPI_SET(void)
{
    Aux_TR_PT100_601314_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_TR_PT100_601314_SSP1ADC.uc_Speed = 2;
    Aux_TR_PT100_601314_SSP1ADC.uc_CPOL = 0;
    Aux_TR_PT100_601314_SSP1ADC.uc_CPHA = 0;
    Aux_TR_PT100_601314_SSP1ADC.uc_MSTR = 1;    //master
    Aux_TR_PT100_601314_SSP1ADC.uc_BITS = 8;
    Aux_TR_PT100_601314_SSP1ADC.uc_LSBF = 0;   //LTC2449

    SSPIFunc::setPara(&Aux_TR_PT100_601314_Base::Aux_TR_PT100_601314_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_TR_PT100_601314_Base::ADC_Init(unsigned char BoardID,unsigned long ADCID)
{


}
void Aux_TR_PT100_601314_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{

}
void Aux_TR_PT100_601314_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();
    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=1;  //每个板只有一个ADC LTC2449

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Valid=1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_TR_PT100_601314;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].counter =0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_Finished = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_WorkingChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_StateMachine = AUX_Sample_IDLE;


    Aux_DataManager::Aux_ADC_CS_Info[BoardID].sspChn = 1;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_CS_Address.m_Value = (7-BoardID); //1110 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_Wait_Count = 10;  //10*50=500us
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_ADCCH_MaxCnt = 16;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Trigger = &Aux_TR_PT100_601314_Base::Aux_V_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardADC_Rd = &Aux_TR_PT100_601314_Base::Aux_V_Read;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = BoardID;
    Aux_DataManager::m_DGIdx2BIdx[BoardID] = BoardID; //data_group index 与 board ID 等值

    //获取通道信息并建立面板通道与电气通道的联系

    //分配 data_group 开始号
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
    //标记data_group 有效
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

    for(int i = 0; i< 16; i++) //该板型最多16个电气通道，且该板无副板
    {
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
        //根据该通道populated 来确定ＡＤＣ通道是否有效　
        Aux_DataManager::Aux_ADC_CS_Info[BoardID].b_ADCCH_Valid[i]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info.m_Populated;

        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //对热电偶板，若带副板的话，要实行转换.其他板无副板的可能。
        {
//=============Added by ZC 09/29/2016===========================
//=============Modified by ZC 06/06/2017========================
            if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932)
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 3-i; // 0 ->3 ; 1 -> 2; 2 ->1; 3 ->0
                }
                else if(i<8)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 11-i; // 4 ->7 ; 5 -> 6; 6 ->5; 7 ->4
                }
                else if(i<12)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 19-i; // 8 ->11 ; 9 -> 10; 10 ->9; 11 ->8
                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 27-i; // 12 ->15 ; 13 -> 14; 14 ->13; 15 ->12
                }
                Aux_DataManager::m_PChn2EChn[1][0].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[1][0].e_chan_id = 16;

            }
            else
            {
//=============End===============================================
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
            } //Added by ZC 09/29/2016

        }
        else
        {
//=============Modified by ZC 06/06/2017========================
            if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932)
            {
                if(i<4)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 3-i; // 0 ->3 ; 1 -> 2; 2 ->1; 3 ->0
                }
                else if(i<8)
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 11-i; // 4 ->7 ; 5 -> 6; 6 ->5; 7 ->4
                }
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][8].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][8].e_chan_id = 8;

            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号
            }
//=======================END=================================================
        }
    }
}
void Aux_TR_PT100_601314_Base::Software_Init(unsigned char Board_ID)
{
    if(Aux_DataManager::m_AllAuxBoards[Board_ID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932)  //Add by DKQ
    {
        Aux_Chamber_TEC_Base::Aux_Chamber_TEC_Board = true;

    }
}

unsigned long Aux_TR_PT100_601314_Base::Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET();
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_TR_PT100_601314_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET();
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //标记已经更新
    return 1;
}
