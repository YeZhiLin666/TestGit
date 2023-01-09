
#include "../../Entry/includes.h"

//temp: called by Aux_DIDO_Base DoWork fuction
void AuxDIDO_SampleManager::Do_Sample(void)
{
	unsigned char bIOValue = 0;
	for(unsigned char boardIdx = 0;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)  //DI DO
	{ 
		if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount<=0 )
			continue;
		if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType== AUX_DI)
		{
			for(unsigned char chnIdx = 0;chnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount;chnIdx++)			
			{			
				if(chnIdx>=MAX_DO_COUNT)
					continue;
				bIOValue = ControlPin::ReadMe(*Aux_DIDO_Base::m_DIDO_MapMgr[chnIdx].m_pSt_DIPort);
				AUX_SampleManage::st_Aux_info[boardIdx][chnIdx].PV = bIOValue;
				AUX_SampleManage::st_Aux_info[boardIdx][chnIdx].NewAdcCode = 1;
			}
		}
		else if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType== AUX_DO)
		{
			for(unsigned char chnIdx = 0;chnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount;chnIdx++)			
			{			
				bIOValue = ControlPin::ReadMe(*Aux_DIDO_Base::m_DIDO_MapMgr[chnIdx].m_pSt_DOPort);
				AUX_SampleManage::st_Aux_info[boardIdx][chnIdx].PV = bIOValue;
				AUX_SampleManage::st_Aux_info[boardIdx][chnIdx].NewAdcCode = 1;
			}
		}
	}
}

