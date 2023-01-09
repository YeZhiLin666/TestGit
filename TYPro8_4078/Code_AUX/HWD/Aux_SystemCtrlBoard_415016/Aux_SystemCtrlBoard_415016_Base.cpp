///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_CPS.cpp
//Version:		V1.0
//Date:				2018/08/06
//Description:
//						辅助系统控制单元配置相关操作
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

unsigned short Aux_SystemCtrlBoard_415016_Base::m_GetDICode;
unsigned short Aux_SystemCtrlBoard_415016_Base::m_SetDOCode;
unsigned short Aux_SystemCtrlBoard_415016_Base::m_StartDelayTime=1;
unsigned short Aux_SystemCtrlBoard_415016_Base::m_StopDelayTime=30;

void Aux_SystemCtrlBoard_415016_Base::IO_Init(void)
{

}


void Aux_SystemCtrlBoard_415016_Base::Func_Init(void)
{
// 	for(unsigned char i=0;i<14;i++)
// 	  Aux_DIDO_601196_WMADC602518_WMADC602518_Base::data[i]=0;
}
void Aux_SystemCtrlBoard_415016_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;

    // My_BoardID = 200;   //记录自己的BoardID；
    if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType==Aux_Sys_Control_PWM_416326)
    {
        Aux_SystemCtrlBoard_415016_Base::m_StartDelayTime=1;
        Aux_SystemCtrlBoard_415016_Base::m_StopDelayTime=30;
    }
    if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType==Aux_Sys_Control_Liner_416326)
    {
        Aux_SystemCtrlBoard_415016_Base::m_StartDelayTime=1;
        Aux_SystemCtrlBoard_415016_Base::m_StopDelayTime=1;
    }

}
void Aux_SystemCtrlBoard_415016_Base::Software_Init(unsigned char Board_ID)
{
// 	Func_Init();
    Aux_ReadDI::m_InitDI = & Aux_SystemCtrlBoard_415016_Base::Init_DI;
    Aux_ReadDI::m_ReadDI = & Aux_SystemCtrlBoard_415016_Base::Read_DI;
    Aux_WriteDO::m_InitDO = & Aux_SystemCtrlBoard_415016_Base::Init_DO;
    Aux_WriteDO::m_WriteDO = & Aux_SystemCtrlBoard_415016_Base::Write_DO;
}


void Aux_SystemCtrlBoard_415016_Base::Init_DI(void)
{


}
void Aux_SystemCtrlBoard_415016_Base::Init_DO(void)
{


}

void Aux_SystemCtrlBoard_415016_Base::Read_DI(void)
{

}

void Aux_SystemCtrlBoard_415016_Base::Write_DO(void)
{


}
