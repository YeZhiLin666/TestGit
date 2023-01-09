///////////////////////////////////////////////////////////////////////////////////
//File Name: BatteryCells_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助配置相关操作
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
const St_Port BatteryCells_Base::Pin_ECODE1		= {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BatteryCells_Base::Pin_ECODE2		= {1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BatteryCells_Base::Pin_ECODE3		= {1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BatteryCells_Base::Pin_EOE		= {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


const St_Port BatteryCells_Base::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port BatteryCells_Base::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port BatteryCells_Base::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
SSPI_Para BatteryCells_Base::BatteryCells_SSP1ADC;
unsigned long BatteryCells_Base::BC_ReadData[AUX_BC_MAXCHIP][AUX_BC_MAXCHANNELPERCHIP];
unsigned char BatteryCells_Base::m_ADC_ChipAddr[8];
unsigned char BatteryCells_Base::m_ADC_MaxCnt[8];
unsigned char BatteryCells_Base::config_data[AUX_BC_ModuleCnt][6];
unsigned char BatteryCells_Base::beUsedCHannel=0;
unsigned int  BatteryCells_Base::Sum_BatteryCellChannel=0;
ST_MAPBATTODAT BatteryCells_Base::m_MapBattToDat[128];

#pragma arm section //结束外存变量定义

void BatteryCells_Base::IO_Init(unsigned char BoardID)
{
    ControlPin::SetIObyCP(Pin_MISO1);
    ControlPin::SetIObyCP(Pin_MOSI1);
    ControlPin::SetIObyCP(Pin_SCK1);
    ControlPin::SetIObyCP(Pin_EOE);
    ControlPin::SetIObyCP(Pin_ECODE1);
    ControlPin::SetIObyCP(Pin_ECODE2);
    ControlPin::SetIObyCP(Pin_ECODE3);



}
void BatteryCells_Base::ADC_Init(unsigned char BoardID,unsigned long ADCID)
{

    BatteryCells_ADC5DAC::Adc_Init();
    for(unsigned char j=0; j<AUX_BC_ModuleCnt; j++)
    {

        config_data[j][0] = 0xFC ;      //CFGR0
        config_data[j][1] = 0X00 ;      //CFGR1  下限0V
        config_data[j][2] = 0x50 ;      //CFGR2
        config_data[j][3] = 0xC3 ;      //CFGR3   上限5V
        config_data[j][4] = 0x00 ;      //CFGR4
        //   config_data[j][5] = 0xF0;     //CFRG5
        config_data[j][5] = 0x00;      //CFRG5

    }
    //	ADC12bit_LTC68042::wakeup_idle();
    ADC16bit_LTC68042::LTC6804_wrcfg(AUX_BC_ModuleCnt,config_data,1);   //写配置寄存器


}
void BatteryCells_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{

    //	board_config::Func_AUX_ADC_rd[BoardID][ADCID][ChannelID]=&BatteryCells_Base::ADC_rd;
    //	board_config::Func_AUX_ADC_CS[BoardID][ADCID][ChannelID]=&BatteryCells_Base::ADC_Cs;
    board_config::Func_AUX_Sample[BoardID][ADCID][ChannelID]=&BatteryCells_SampleManage::SampleISR;
    // 	board_config::Func_AUX_CaliChannel[BoardID][ADCID][ChannelID]=&LTC6804::CaliChannel;
}
void BatteryCells_Base::Hardware_Init(unsigned char BoardID)
{
    BatteryCells_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    BatteryCells_SSP1ADC.uc_Speed = 1;
    BatteryCells_SSP1ADC.uc_CPOL = 1;
    BatteryCells_SSP1ADC.uc_CPHA = 1;
    BatteryCells_SSP1ADC.uc_MSTR = 1;    //master
    BatteryCells_SSP1ADC.uc_BITS = 8;
    BatteryCells_SSP1ADC.uc_LSBF = 0;
    SSPIFunc::setPara(&BatteryCells_Base::BatteryCells_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化

    IO_Init(BoardID);
    ADC16bit_LTC68042::setCS(&Pin_EOE);     //传递片选信号LTC6804_CS
    ADC_Init(0,0);

    Aux_DataManager::m_Aux_BoardCFG[BoardID].ChannelADC[BoardID]=7;   //支持8个ADC 编号0~7


    Aux_DataManager::m_Aux_BoardCFG[BoardID].BeUsed = 1;
    Aux_DataManager::m_Aux_BoardCFG[BoardID].m_ChannelCount = 60;

    Aux_DataManager::m_Aux_BoardCFG[BoardID].BoardTYPE = AUX_BATTERYCELLS;
    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_BoardType = Aux_2ndV_BS_601440;

    Aux_DataManager::Aux_ADC_CS_Info[BoardID].m_data_group_idx = 0;


    //电池包的ADC在任务中轮询，不在中断中，因此初始化内涉及中断部分可省略， Yifei
    //?????????????????????

    //?? data_group ???
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;	//??????0
    for(int g_id =0; g_id<6; g_id++)
    {
        Aux_DataManager::m_DGIdx2BIdx[g_id] = BoardID; //前6个data_group均为该板
    }

    for(int k=0; k<6; k++)
    {
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+k] = true;
        //Aux_DataManager::m_DataGroupMgr[k] = true;	//?????,?????????????,????????
    }

    int i = 0;	//???????????
    //??????m_AuxDataBank??
    for(int data_group_id=0; data_group_id<6; data_group_id++)
        for(int chan_id=0; chan_id<16; chan_id++)	//?????96?????,??data_group???6?group,??group?16?
        {   //?????????AuxDataBank??
            Aux_DataManager::m_AuxDataBank[data_group_id][chan_id].m_Chn_Info = Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i++];   //copy????? AuxDataBank
            //?????populated ???ADC??????
            Aux_DataManager::Aux_ADC_CS_Info[data_group_id].b_ADCCH_Valid[chan_id] = Aux_DataManager::m_AuxDataBank[data_group_id][chan_id].m_Chn_Info.m_Populated;
        }


    for(int data_group_id = 0; data_group_id<6; data_group_id++)	//???????????????
        for(int chan_id = 0; chan_id<16; chan_id++)
        {
            //?????,???????,??????
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) 	//?????
            {
                Aux_DataManager::m_PChn2EChn[data_group_id][chan_id].e_data_group_id = 5 - data_group_id;
                Aux_DataManager::m_PChn2EChn[data_group_id][chan_id].e_chan_id = 15 - chan_id;
            }
            else	//??????
            {
                Aux_DataManager::m_PChn2EChn[data_group_id][chan_id].e_data_group_id = 2 - data_group_id;
                Aux_DataManager::m_PChn2EChn[data_group_id][chan_id].e_chan_id = 15 - chan_id;
            }
        }


}
void BatteryCells_Base::Software_Init(unsigned char Board_ID)
{
    if(Aux_DataManager::m_AllAuxBoards[Board_ID].m_BoardInfo.m_valid==VIRTUAL)
    {
        for(unsigned char uc_i=0; uc_i<MAX_CH_IN_AUX_UNIT; uc_i++)
        {
            // 			board_config::Func_AUX_CaliChannel[Board_ID][0][uc_i]=&LTC6804::CaliChannel;
        }
    }
    else
    {
        for(unsigned char uc_i=0; uc_i<MAX_CH_IN_AUX_UNIT; uc_i++)
        {
            Func_Init(Board_ID,0,uc_i);
            Aux_DataManager::ADC_SampleProcessType[Board_ID][0][uc_i]=SAMPLETASK;   //注册为任务调度类型采样处理
        }

        BatteryCells_SampleManage::Init(Board_ID);
        //注册电池包通道到虚拟通道上
        for(unsigned char uc_i=0; uc_i<128; uc_i++)
        {
            m_MapBattToDat[uc_i].m_BatteryCellsChipID=uc_i/12;
            m_MapBattToDat[uc_i].m_BatteryCellsChannelNum=uc_i%12;
            m_MapBattToDat[uc_i].m_BoardID=uc_i/16;
            m_MapBattToDat[uc_i].m_AuxChannelNum=uc_i%16;
        }
    }
    BatteryCells_Base::ReleaselConfig(Board_ID);
}
char BatteryCells_Base::ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
    return 0;
}

char BatteryCells_Base::ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{


    return 0;
}



unsigned char	BatteryCells_Base::ReleaselConfig(unsigned char BoardID)
{

    //for(unsigned char uc_i=0;uc_i<MAX_BOARD_PER_AUXMCU;uc_i++)
    //{
    //if(Sum_BatteryCellChannel>0)  //若有残留参数，先清掉。
    //	Sum_BatteryCellChannel=0;

    if((Aux_DataManager::m_Aux_BoardCFG[BoardID].BoardTYPE!=AUX_BATTERYCELLS) || (Aux_DataManager::m_Aux_BoardCFG[BoardID].BeUsed==0))
    {
        // 			//直到没有通道被注册则统计被使用的通道数
        // 			beUsedCHannel=Sum_BatteryCellChannel/12;
        // 			if((Sum_BatteryCellChannel%12)!=0)
        // 				beUsedCHannel++;
        return Sum_BatteryCellChannel;
    }
    Sum_BatteryCellChannel+=Aux_DataManager::m_Aux_BoardCFG[BoardID].m_ChannelCount;   //统计一共多少个通道
    // 		beUsedCHannel=Sum_BatteryCellChannel/12;
    // 	if((Sum_BatteryCellChannel%12)!=0)
    // 		beUsedCHannel++;
    //}


    return beUsedCHannel;
}

