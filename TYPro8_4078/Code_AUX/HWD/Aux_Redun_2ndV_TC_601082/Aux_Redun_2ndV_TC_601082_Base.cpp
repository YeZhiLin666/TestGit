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

// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_SCK0;      //= {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_MISO0;    // = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_MOSI0;    // = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};

// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_SCK1;     // = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_MISO1;     //= {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_MOSI1;     //= {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_ECODE1;		//={1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_ECODE2;		//={1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_ECODE3;		//={1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_EOE;		  //={1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//  St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_ADCBUSY_V;  //	={2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port Aux_Redun_2ndV_TC_601082_Base::Pin_ADCBUSY_T= {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0};;

SSPI_Para Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP1ADC;
SSPI_Para Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP0ADC;
unsigned long Aux_Redun_2ndV_TC_601082_Base::m_Inchip_temperature;   //片内温度传感器的码值

unsigned long Aux_Redun_2ndV_TC_601082_Base::error_counter;

void Aux_Redun_2ndV_TC_601082_Base::IO_Init(void)
{
    ControlPin::SetIObyCP(Pin_ADCBUSY_T);
}

void Aux_Redun_2ndV_TC_601082_Base::SPI_SET(unsigned char spiChn)
{
    switch (spiChn)
    {

    case  0:
    {

        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_Module_Name = SSP0_DEVICE1;
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_Speed = 2;
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_CPOL = 0;
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_CPHA = 0;
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_MSTR = 1;    //master
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_BITS = 8;
        Aux_Redun_2ndV_TC_601082_SSP0ADC.uc_LSBF = 0;    //LTC2498  for Temp
        SSPIFunc::setPara(&Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP0ADC,0);
        SSPIFunc::my_Init(0);


    }
    break;

    case 1:
    {
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_Speed = 2;
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_CPOL = 0;
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_CPHA = 0;
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_MSTR = 1;    //master
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_BITS = 8;
        Aux_Redun_2ndV_TC_601082_SSP1ADC.uc_LSBF = 0;     //LTC2449 for 2ndV

        SSPIFunc::setPara(&Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP1ADC,1);
        SSPIFunc::my_Init(1);

    }
    break;

    default:
        break;


    }

}




void Aux_Redun_2ndV_TC_601082_Base::ADC_Init()
{
    Adc32bit_LTC2449::Pin_CS  = AUX_Unit_Base::Pin_ECODE1;
    Adc32bit_LTC2449::Pin_Busy =AUX_Unit_Base::Pin_ADCBUSY;
    //
    //		SSPIFunc::setPara(&Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP1ADC,1);
    //	  SSPIFunc::my_Init(1);

    // 	  Adc32bit_LTC2498::Pin_CS = AUX_Unit_Base::Pin_ECODE2;
    //   	Adc32bit_LTC2498::setCS(&AUX_Unit_Base::::Pin_ECODE2);
    Adc32bit_LTC2498::Pin_Busy = Aux_Redun_2ndV_TC_601082_Base::Pin_ADCBUSY_T;
    //	  SSPIFunc::setPara(&Aux_Redun_2ndV_TC_601082_Base::Aux_Redun_2ndV_TC_601082_SSP0ADC,0);
    //	  SSPIFunc::my_Init(0);
}
void Aux_Redun_2ndV_TC_601082_Base::Func_Init(unsigned long BoardID)
{
    // should move to Hardware_Init
    //	  Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger = &Aux_Redun_2ndV_TC_601082_Base::Aux_V_Trigger;
    //	  Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_Redun_2ndV_TC_601082_Base::Aux_V_Read;

    //	  Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Trigger = &Aux_Redun_2ndV_TC_601082_Base::Aux_T_Trigger;
    //	  Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Rd = &Aux_Redun_2ndV_TC_601082_Base::Aux_T_Read;
}
void Aux_Redun_2ndV_TC_601082_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();

    //Aux_DataManager::m_AllAuxBoards[0].m_abi_globe.m_Total_ADC=2;   //该PN号下的ADC芯片共有两个，由于PN600826没有波码，故ADC的波码是固定的
    Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC=2;   //万一没有拨成0，也应该能工作

    Aux_Redun_2ndV_TC_601082_Base::ADC_Init();



    for(unsigned char i =0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC; i++)
    {
        Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_Redun_2ndV_TC_601082;

        Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
        Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
        Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;
        Aux_DataManager::m_DGIdx2BIdx[i] = BoardID; //data_group index 0,及 1 对应的  board ID

    }



    Aux_DataManager::Aux_ADC_CS_Info[0].sspChn = 1;
    Aux_DataManager::Aux_ADC_CS_Info[0].m_CS_Address.m_Value = 0x0E; //1110 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[0].m_Wait_Count = 10;  //10*50=500us
    Aux_DataManager::Aux_ADC_CS_Info[0].m_ADCCH_MaxCnt = 16;  //8
    Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger = &Aux_Redun_2ndV_TC_601082_Base::Aux_V_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_Redun_2ndV_TC_601082_Base::Aux_V_Read;
    Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx = 0;
    // 		Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_valid = BEUSED;


    Aux_DataManager::Aux_ADC_CS_Info[1].sspChn = 0;
    Aux_DataManager::Aux_ADC_CS_Info[1].m_CS_Address.m_Value = 0x0D;  //1101 EOE~ECODE1
    Aux_DataManager::Aux_ADC_CS_Info[1].m_Wait_Count = 4000;  //4000*50=200ms
    Aux_DataManager::Aux_ADC_CS_Info[1].m_ADCCH_MaxCnt = 17;
    Aux_DataManager::Aux_ADC_CS_Info[1].b_ADCCH_Valid[16]=1; //保证采样温度
    Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Trigger = &Aux_Redun_2ndV_TC_601082_Base::Aux_T_Trigger;
    Aux_DataManager::Aux_ADC_CS_Info[1].m_BoardADC_Rd = &Aux_Redun_2ndV_TC_601082_Base::Aux_T_Read;
    Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx = 1;
    // 		Aux_DataManager::m_AllAuxBoards[1].m_BoardInfo.m_valid = BEUSED;



    //获取通道信息并建立面板通道与电气通道的联系

    //分配 data_group 开始号, 一个AUXMCU只带一块ADDON板
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
    //标记data_group 有效	，一块ＡＤＤＯＮ板占两个data_group, ，电压和 温度各一个
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1] = true;

    for(unsigned char i = 0; i< 32; i++) //该板型最多32个电气通道,前16个通道是电压，后16个是温度
    {   //拷贝板上通道信息到AuxDataBank中去。好像是有点重复，但先这样吧
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
        //根据该通道populated 来确定ＡＤＣ通道是否有效　
        Aux_DataManager::Aux_ADC_CS_Info[i/16].b_ADCCH_Valid[(i%16)]= Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+i/16][(i%16)].m_Chn_Info.m_Populated;
    }
    for(unsigned char j =0; j<16; j++)
    {
        if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //，若带副板的话，要实行转换.。
        {
            if (j<8)  //面板电压1-8 对应于电气通道通道9-16
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_chan_id = 8+j;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_chan_id = 8+j;

            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_chan_id = j- 8;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_chan_id =j-8;

            }

        }
        else
        {
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][j].e_chan_id = j;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][j].e_chan_id = j;


        }

    }

}
void Aux_Redun_2ndV_TC_601082_Base::Software_Init(unsigned char Board_ID)
{
    //	Func_Init(Board_ID);
    // 		Aux_DataManager::ADC_SampleProcessType[Board_ID][0][uc_i]=SAMPLEISR;   //注册为中断类型采样处理
}

// char Aux_Redun_2ndV_TC_601082_Base::ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
// {
// 	return 0;
// }

// char Aux_Redun_2ndV_TC_601082_Base::ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
// {
//
//
// 	return 0;
// }
unsigned long Aux_Redun_2ndV_TC_601082_Base::Aux_V_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);

    return 1;
}

unsigned long Aux_Redun_2ndV_TC_601082_Base::Aux_V_Read(unsigned char u_CSid, unsigned char ADCchn)
{
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx ][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //将数据放到Aux_DataBank 中
    Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx ][ADCchn].updated = true; //标记已经更新
    return 1;

}
//T温度，在这里温度可能是T型，也可能是K型
unsigned long Aux_Redun_2ndV_TC_601082_Base::Aux_T_Trigger(unsigned char u_CSid,unsigned char ADCchn)
{
    //直接调用LTC2498
    SPI_SET(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);
    // 	  Adc32bit_LTC2498::Pin_CS = AUX_Unit_Base::Pin_ECODE2;
    Adc32bit_LTC2498::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
    return 1;
}

unsigned long Aux_Redun_2ndV_TC_601082_Base::Aux_T_Read(unsigned char u_CSid,unsigned char ADCchn)
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
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx].InChipTemp_instant_code= temp ;//将数据放到一个统一管理的片内温度QUE里以便做平均处理
            Aux_DataManager::m_InChipTemperature[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx].updated= true ;//将数据放到一个统一管理的片内温度QUE里以便做平均处理

        }
        else
        {   Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx ][ADCchn].aux_instant_code =temp; // Adc32bit_LTC2498::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn);//将数据放到Aux_DataBank 中, 2498的数放到16-31
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[1].m_data_group_idx ][ADCchn].updated = true; // 标记已经采样
        }
        return 1;
    }
}
