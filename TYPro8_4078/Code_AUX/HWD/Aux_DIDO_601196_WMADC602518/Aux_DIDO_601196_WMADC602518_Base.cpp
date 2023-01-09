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

//DI DO Module
//DI
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DI_1      = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DI_2      = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DI_3      = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DI_4      = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_I,             0};

// //DO
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DO_1     = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DO_2     = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DO_3     = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
// const St_Port Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_DO_4     = {1,        29,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
//temp one board
// St_DIDOMapMgr  Aux_DIDO_601196_WMADC602518_WMADC602518_Base::m_DIDO_MapMgr[MAX_DO_COUNT] = {
// 	0,0,&Pin_DI_1,&Pin_DO_1,
// 	1,1,&Pin_DI_2,&Pin_DO_2,
// 	2,2,&Pin_DI_3,&Pin_DO_3,
// 	3,3,&Pin_DI_4,&Pin_DO_4,

// };

unsigned char  Aux_DIDO_601196_WMADC602518_Base::My_BoardID =0;
unsigned short Aux_DIDO_601196_WMADC602518_Base::DIDO_Value =0;


void Aux_DIDO_601196_WMADC602518_Base::IO_Init(void)
{
    //There is no speacial port for this board
// 	ControlPin::SetIObyCP(Pin_Conver);

// 	Adc24bit_ADS1259::Pin_Conver = Aux_DIDO_601196_WMADC602518_WMADC602518_Base::Pin_Conver;

}


void Aux_DIDO_601196_WMADC602518_Base::Func_Init(void)
{
// 	for(unsigned char i=0;i<14;i++)
// 	  Aux_DIDO_601196_WMADC602518_WMADC602518_Base::data[i]=0;
}
void Aux_DIDO_601196_WMADC602518_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;

    My_BoardID = BoardID;   //记录自己的BoardID；
    CANBusParse::DIDO_Ctrl=2;
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;


    if( ((AuxFunc&0x02)==0x02) && (Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI !=0))  //将DI的数据放 2-5
    {
        //分配 data_group 开始号
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;  //用于存放DI的16通道数据
        //标记data_group 有效	，一块DIDO板占两个data_group, ，DI和 DO各一个
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DI; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i]. m_Chn_Info.m_AuxType = AUX_DI;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //，若带副板的话，要实行转换.。
            {
                if (i<8)  //面板电压1-8 对应于电气通道通道9-16
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = 8+i;

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                    Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i- 8;
                }
            }
            else
            {
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i;
            }

        }
//        OS_ServeProcess::MountTask(Aux_ReadDI::TaskID,                  0,            Aux_ReadDI::Priority,                  1);
    }

    if(((AuxFunc&0x04)==0x04) &&( Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DO !=0) ) //将DO的数据放 10-13
    {

        Aux_DataManager::m_DataGroupMgr[8] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DO; i++)
        {
            Aux_DataManager::m_AuxDataBank[8][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[8][i]. m_Chn_Info.m_AuxType = AUX_DO;
            if((Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_assemble.m_WithSlave)) //，若带副板的话，要实行转换.。
            {
                if (i<8)  //面板电压1-8 对应于电气通道通道9-16
                {
                    Aux_DataManager::m_PChn2EChn[8][i].e_data_group_id = 8;
                    Aux_DataManager::m_PChn2EChn[8][i].e_chan_id = 8+i;

                }
                else
                {
                    Aux_DataManager::m_PChn2EChn[8][i].e_data_group_id = 8;
                    Aux_DataManager::m_PChn2EChn[8][i].e_chan_id = i- 8;
                }
            }
            else
            {
                Aux_DataManager::m_PChn2EChn[8][i].e_data_group_id = 8;
                Aux_DataManager::m_PChn2EChn[8][i].e_chan_id = i;
            }
        }
//         OS_ServeProcess::MountTask(Aux_WriteDO::TaskID,                  0,            Aux_WriteDO::Priority,                  1);
    }

    //Data_group arrangement for DIDO_601196

//	   0  1 2  3  4  5   6 7   8 9 10 11 12 13 14 15
//     DI_1~DI_16

}
void Aux_DIDO_601196_WMADC602518_Base::Software_Init(unsigned char Board_ID)
{
// 	Func_Init();
    Aux_ReadDI::m_InitDI = & Aux_DIDO_601196_WMADC602518_Base::Init_DI;
    Aux_ReadDI::m_ReadDI = & Aux_DIDO_601196_WMADC602518_Base::Read_DI;
    Aux_WriteDO::m_InitDO = & Aux_DIDO_601196_WMADC602518_Base::Init_DO;
    Aux_WriteDO::m_WriteDO = & Aux_DIDO_601196_WMADC602518_Base::Write_DO;
}


void Aux_DIDO_601196_WMADC602518_Base::Init_DI(void)
{
//内部IO初始化
//    ControlPin::SetIObyCP(Pin_DI_1);
// 	 ControlPin::SetIObyCP(Pin_DI_2);
// 	 ControlPin::SetIObyCP(Pin_DI_3);
// 	 ControlPin::SetIObyCP(Pin_DI_4);


}
void Aux_DIDO_601196_WMADC602518_Base::Init_DO(void)
{
//   ControlPin::SetIObyCP(Pin_DO_1);
// 	ControlPin::SetIObyCP(Pin_DO_2);
// 	ControlPin::SetIObyCP(Pin_DO_3);
// 	ControlPin::SetIObyCP(Pin_DO_4);

}

void Aux_DIDO_601196_WMADC602518_Base::Read_DI(void)
{
    unsigned short value=0;
    unsigned short result=0;
    value = CPLD_FUNC::Read(NBUS_WRITE_ARBINDIR);
    for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[My_BoardID].m_BoardInfo.m_abi_globe.m_Total_DI; i++)
    {
        result = (value & (1<<i));
        result=result>>i;
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i].aux_data = result;
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[My_BoardID].m_data_group_start][i].updated = true;

    }

}

void Aux_DIDO_601196_WMADC602518_Base::Write_DO(void)
{

// 	  unsigned short result=0;
    unsigned char data;

//      CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,value);
    //写DO端口

    for (unsigned char i=0; i< Aux_DataManager::m_AllAuxBoards[My_BoardID].m_BoardInfo.m_abi_globe.m_Total_DO; i++)
    {

        if(Aux_DataManager::m_AuxDataBank[8][i].updated)
        {
            data =  Aux_DataManager::m_AuxDataBank[8][i].aux_data ;
            if(data==0x01)
                DIDO_Value |=(1<<i);
            else
                DIDO_Value &= (~(1<<i));
// 					  CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,DIDO_Value);
            Aux_DataManager::m_AuxDataBank[8][i].updated=0;
        }
        else
            continue;
    }
// 		if(Aux_DataManager::m_AuxDataBank[8][i].updated)
    CPLD_FUNC::Write(NBUS_WIRTE_ARBIN,DIDO_Value);
}
