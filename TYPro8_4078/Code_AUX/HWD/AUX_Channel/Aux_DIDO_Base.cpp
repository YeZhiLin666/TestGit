///////////////////////////////////////////////////////////////////////////////////
//File Name: Aux_DIDO_Base.cpp
//Version:		V1.0
//Date:			2015/01/20
//Description:
//					Aux DIDO Function  this  is a Multi-Auxiliary board (1 CAN_BMS Port 4DO 4DI 1 uart)
//Others:      Novice
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../../Entry/includes.h"
UInt16 Aux_DIDO_Base::Priority = PRIORITY4;
UInt16    Aux_DIDO_Base::TaskID = AUX_DIDO_TASK;
//DI DO Module 
//DI
const St_Port Aux_DIDO_Base::Pin_DI_1      = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_DIDO_Base::Pin_DI_2      = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_DIDO_Base::Pin_DI_3      = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const St_Port Aux_DIDO_Base::Pin_DI_4      = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_I,             0};

//DO
const St_Port Aux_DIDO_Base::Pin_DO_1     = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_DIDO_Base::Pin_DO_2     = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_DIDO_Base::Pin_DO_3     = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_DIDO_Base::Pin_DO_4     = {1,        29,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

//temp one board 
St_DIDOMapMgr  Aux_DIDO_Base::m_DIDO_MapMgr[MAX_DO_COUNT] = {
	0,0,&Pin_DI_1,&Pin_DO_1,
	1,1,&Pin_DI_2,&Pin_DO_2,
	2,2,&Pin_DI_3,&Pin_DO_3,
	3,3,&Pin_DI_4,&Pin_DO_4,

};

/******************************************************************************
** Function name:		Hardware_Init
**
** Descriptions:	        //4 DO Output
**			     
** parameters:		  None
** Returned value:		None
** 
******************************************************************************/
void Aux_DIDO_Base::Hardware_Init(void)
{  
	//内部IO初始化
	ControlPin::SetIObyCP(Pin_DI_1);
	ControlPin::SetIObyCP(Pin_DI_2);
	ControlPin::SetIObyCP(Pin_DI_3);
	ControlPin::SetIObyCP(Pin_DI_4);


	ControlPin::SetIObyCP(Pin_DO_1);
	ControlPin::SetIObyCP(Pin_DO_2);
	ControlPin::SetIObyCP(Pin_DO_3);
	ControlPin::SetIObyCP(Pin_DO_4);	

}

void Aux_DIDO_Base::Software_Init(void)
{

	for(unsigned char boardIdx = 0;boardIdx<MAX_BOARD_PER_AUXMCU;boardIdx++)
	{
		if(!Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_AliveFlg
			||Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType != AUX_DO)//DIDO
			continue;
		if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount<=0)
			continue;
		for(unsigned char phyChnIdx = 0;phyChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount;phyChnIdx++)
		{						 
			;// Aux_DataManager::m_AllAuxBoards[boardIdx].m_AuxSensor_ChInfo[phyChnIdx].m_AuxDI = 0;
			//m_DIDO_MapMgr[phyChnIdx].m_pSt_DIPort = 

		}
	}
	//	
	//		m_DIDO_MapMgr[uc_AuxCh].m_MapDIIdx
	//		for(unsigned char unitIdx = 0;unitIdx<MAX_IVMCU_PER_SYS;unitIdx++)
	//	 {
	//			if(!Aux_DataManager::m_IVMCU_ID_Info[unitIdx].m_Valid)
	//				continue;
	//			for(unsigned char uc_IVIdx = 0;uc_IVIdx<MAX_IVCH_PER_IVMCU;uc_IVIdx++)
	//			{	
	//				if(Aux_DataManager::m_MapFromPC[unitIdx][unitIdx][AUX_DO].m_ActualCounts<=0)
	//					continue;
	//			  for(unsigned char auxChnIdx = 0;auxChnIdx<Aux_DataManager::m_MapFromPC[unitIdx][unitIdx][AUX_DO].m_ActualCounts;auxChnIdx++)
	//				{
	//					unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[unitIdx][unitIdx][AUX_DO].m_MapInfo[auxChnIdx].m_Index_InAuxUnit;
	//					m_DIDO_MapMgr[uc_AuxCh].m_MapDIIdx = uc_AuxCh;
	//				} 			
	//				
	//			}
	//	}
}

//4 DO Output & 4DI 4DO Sample Task 

void Aux_DIDO_Base::DoWork(void)
{
	//create a struct in aux_datamanager.cpp about DIDOCtrl [struct: m_AuxDOCtrlMgr]
	//first step: according to can cmd to set step ctrl logic

	bool SetValue= false;
	unsigned char uc_AuxCh = 0;
	for(unsigned char unitIdx = 0;unitIdx<MAX_IVMCU_PER_SYS;unitIdx++)   //此部分的转换应该在接收时做一次转换即可
	{
		if(!Aux_DataManager::m_IVMCU_ID_Info[unitIdx].m_Valid)
			continue;
		for(unsigned char uc_IVIdx = 0;uc_IVIdx<MAX_IVCH_PER_IVMCU;uc_IVIdx++)
		{								

			if(Aux_DataManager::m_MapFromPC[unitIdx][uc_IVIdx][AUX_DO].m_ActualCounts<=0)
				continue;
			const ST_Aux_oneDOCtrl* pOneChnDOCtrl =  &Aux_DataManager::m_AuxDOCtrlMgr[unitIdx][uc_IVIdx].m_DOValue[0];
			if(Aux_DataManager::m_AuxDOCtrlMgr[unitIdx][uc_IVIdx].m_DoCount>MAX_DO_COUNT)
				continue; //protect 
			for(unsigned char DOIdx = 0;DOIdx<Aux_DataManager::m_AuxDOCtrlMgr[unitIdx][uc_IVIdx].m_DoCount;DOIdx++)
			{
				if(Aux_DataManager::m_MapFromPC[unitIdx][uc_IVIdx][AUX_DO].m_MapInfo[DOIdx].m_used == 0)
					continue;
				if(!pOneChnDOCtrl[DOIdx].m_bValid)
					continue;
				uc_AuxCh = pOneChnDOCtrl[DOIdx].m_CID; //get map chnIdx		 
				SetValue = pOneChnDOCtrl[DOIdx].m_bOut;	    				
				if(ControlPin::ReadMe(*m_DIDO_MapMgr[uc_AuxCh].m_pSt_DOPort)!=SetValue)
				{
					if(SetValue)
						ControlPin::SetMeHigh(*m_DIDO_MapMgr[uc_AuxCh].m_pSt_DOPort,1);				  						      
					else					
						ControlPin::SetMeLow(*m_DIDO_MapMgr[uc_AuxCh].m_pSt_DOPort,1);					
				}
			}
			// Aux_DataManager::m_AuxDOCtrlMgr[unitIdx][uc_IVIdx].m_DOValue[uc_DOCnt].m_DoValue  = 1;			
			//unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[m_PresentLimit.m_IVunitBufIndex][m_PresentLimit.m_IVch][MyLimit->m_aux_type].m_MapInfo[MyLimit->m_chn_index].m_Index_InAuxUnit;
			//	Aux_DataManager::Aux_GetData(Aux_DataManager::m_IVMCU_ID_Info[m_PresentLimit.m_IVunitBufIndex].m_ID, m_PresentLimit.m_IVch,MyLimit->m_aux_type,uc_AuxCh,&MyData);
		}
	}
	AuxDIDO_SampleManager::Do_Sample();   //  muti-auxiliary board sample 
}






