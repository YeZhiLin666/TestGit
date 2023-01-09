#include "includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

float M0RedundantFunc::m_fM0Value = 0;

//temp
St_UartCmdBuf        M0RedundantFunc::SafetyInfo;
St_RedundantChnMap   M0RedundantFunc::m_RedundantChnMap[MAX_REDUNDANT_TYPE];
bool                 M0RedundantFunc::m_bIsM0ReleaseCheck[MAXCHANNELNO];
bool                 M0RedundantFunc::m_bClampValid[MAXCHANNELNO];
float                M0RedundantFunc::m_SampleData[MAXCHANNELNO];
St_RangeCali         M0RedundantFunc::m_M0_CalibPara[MAXCHANNELNO];
St_RedundantChnMap   M0RedundantFunc::M0_RedundantMap;
#pragma arm section //

//every board type add a m0 redundant?


const unsigned char M0RedundantFunc::m_M0MapTable[MAXCHANNELNO] =
{
    0,1,2,3,
    4,5,6,7,
    8,9,10,11,
    12,13,14,15
};
const unsigned char M0RedundantFunc::m_M0MapChnValid[MAXCHANNELNO] =
{
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1
};


// void M0RedundantFunc::DoSafetyCheck(void)
// {

// }

short M0RedundantFunc::getThisTypeIdx(unsigned char Type)
{
    for(unsigned char idx =0; idx<MAX_REDUNDANT_TYPE; idx++)
    {
        if(m_RedundantChnMap[idx].m_Type == Type)
            return idx;
    }
    return -1;
}

// void M0RedundantFunc::SetAdcGainOffset(unsigned char Type)
// {
// 	short TypeIdx = getThisTypeIdx(Type);
// 	if(TypeIdx==-1)
// 		return;

// 	memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);

// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdHead = CMD_UART_SET_GAIN_OFFSET;
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CaliType = m_RedundantChnMap[TypeIdx].m_Type; //voltage
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_fGain   = m_RedundantChnMap[TypeIdx].m_fAdcGain;
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_fOffset = m_RedundantChnMap[TypeIdx].m_fAdcOffset;

// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CheckSum = 0;
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdStart = UART_CMD_START;  // 包头
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdEnd = UART_CMD_END;   // 包尾
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset);
// 	UartParse::SendCmd(&SafetyInfo);
// }

void M0RedundantFunc::SetRedundantSafety(unsigned char uc_IVChnNo,unsigned char uc_Type)
{
    short TypeIdx = getThisTypeIdx(uc_Type);
    if(TypeIdx== -1)
        return;

    memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);

    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdHead =CMD_UART_SET_SAFETY;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CheckSum = 0;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdStart = UART_CMD_START;  // 包头
    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdEnd = UART_CMD_END;   // 包尾
    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_redundantType = uc_Type;//voltage
    SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety);
    St_SaftyCheckItem compactSafetyInfo;
    M0RedundantFunc::GetM0SafetyInfo(uc_IVChnNo,uc_Type,&compactSafetyInfo);
    m_RedundantChnMap[TypeIdx].m_fSafetyH[uc_IVChnNo] = compactSafetyInfo.m_fHigh;
    m_RedundantChnMap[TypeIdx].m_fSafetyL[uc_IVChnNo] =  compactSafetyInfo.m_fLow;
    //use IV  redundant map to set this safety range
    if(m_RedundantChnMap[TypeIdx].m_bValid[uc_IVChnNo])
    {
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_IVChnNo = uc_IVChnNo;
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_M4ADCChnIdx = m_RedundantChnMap[TypeIdx].m_ChanNo[uc_IVChnNo];
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_SafetyH = m_RedundantChnMap[TypeIdx].m_fSafetyH[uc_IVChnNo];
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_SafetyL = m_RedundantChnMap[TypeIdx].m_fSafetyL[uc_IVChnNo];
    }
    else
        return;

    UartParse::SendCmd(&SafetyInfo);
}
void M0RedundantFunc::SetM0CalibPara(unsigned char uc_ChnNo,unsigned char uc_MaxChnNo,St_RangeCali m_para)
{
    memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdHead = CMD_UART_SET_GAIN_OFFSET;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CheckSum = 0;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdStart = UART_CMD_START;  // 包头
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CmdEnd = UART_CMD_END;   // 包尾
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_fGain = m_para.m_Gain;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_fOffset = m_para.m_Offset;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_ChnNo = uc_ChnNo;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_MaxChnNo = uc_MaxChnNo;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_CaliType = eType_Voltage;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetGainOffset);
    UartParse::SendCmd(&SafetyInfo);
}
void M0RedundantFunc::SetM0ConfigInfo(void)
{
    memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);

    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_CmdHead = CMD_UART_INIT_M0;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_CheckSum = 0;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_CmdStart = UART_CMD_START;  // 包头
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_CmdEnd = UART_CMD_END;   // 包尾
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_redundantType = 0;//voltage
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo);
    if((ChannelInfo_Manager::m_st_BoadType.m_PowerLevel == POWER_LEVEL_HC) && (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084))
    {
        SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_ChnOrderFlag =0x01;
    }
    else
    {
        SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_ChnOrderFlag =0x00;
    }
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_MaxChnNo =ChannelInfo_Manager::m_st_BoadType.m_u16BoardChCount;

    UartParse::SendCmd(&SafetyInfo);
}
void M0RedundantFunc::SetM0DataRefresh(unsigned char uc_ChnNo,unsigned char uc_SwitchOn)
{
    memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);
    SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_CmdHead = CMD_UART_SetData_Switch;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_ChnNo =uc_ChnNo;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_ONOROFF =uc_SwitchOn;
// 	  SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_CheckSum = 0;
    SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_CmdStart = UART_CMD_START;  // 包头
    SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch.m_CmdEnd = UART_CMD_END;   // 包尾
// 	SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_redundantType = 0;//voltage
    SafetyInfo.m_UartCmdGroup.m_Uart_SetBoardInfo.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetDataSwitch);
    UartParse::SendCmd(&SafetyInfo);

}
void M0RedundantFunc::ReleaseSafetyCheck(unsigned char uc_IVChnNo,unsigned char uc_Type)
{
    short TypeIdx = getThisTypeIdx(uc_Type);
    if(TypeIdx== -1)
        return;
    if(m_RedundantChnMap[TypeIdx].m_bValid[uc_IVChnNo])
    {
        memset((unsigned char*)&SafetyInfo,0,MAX_UART_RX_LENGTH);
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdHead =CMD_UART_SET_SAFETY;
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CheckSum = 0;
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdStart = UART_CMD_START;  // 包头
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_CmdEnd = UART_CMD_END;   // 包尾
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_redundantType = uc_Type;//voltage
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_Length = sizeof(SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety);
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_IVChnNo = uc_IVChnNo;
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_M4ADCChnIdx = m_RedundantChnMap[TypeIdx].m_ChanNo[uc_IVChnNo];
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_SafetyH = 10000.0f;
        SafetyInfo.m_UartCmdGroup.m_Uart_SetSafety.m_SafetyL = -1000.0f;
        UartParse::SendCmd(&SafetyInfo);
    }
    else
        return;
}

void M0RedundantFunc::GetM0SafetyInfo(unsigned char uc_IVChnNo,unsigned char uc_Type,St_SaftyCheckItem * compactSafetyInfo)
{
    unsigned char SaftyType = MP_SCH_SAFTY_TYPE_VOLTAGE;//same sch type
    St_SaftyCheckItem  systemSafety,schSafety,clampSafety;
    compactSafetyInfo->m_fHigh = 10000.0f;
    compactSafetyInfo->m_fLow = -10000.0f;
    ChannelInfo_Manager::GetSystemSafty(uc_IVChnNo,SaftyType,VOLTAGE_RANGE_HIGH, &systemSafety);
    if(systemSafety.m_bIfCheck)
    {
        compactSafetyInfo->m_fHigh = systemSafety.m_fHigh;
        compactSafetyInfo->m_fLow = systemSafety.m_fLow;
    }
    ChannelInfo_Manager::GetScheduleSafty(uc_IVChnNo,SaftyType, &schSafety);
// 	if(schSafety.m_bIfCheck)
// 	{
        if(systemSafety.m_fHigh > schSafety.m_fHigh)
            compactSafetyInfo->m_fHigh = schSafety.m_fHigh;

        if(systemSafety.m_fLow < schSafety.m_fLow)
            compactSafetyInfo->m_fLow = schSafety.m_fLow;
// 	}
	if(!ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_RedundantVotlage.bNeedCheck)
		   return;
// 	if(m_bClampValid[uc_IVChnNo])		// 20190723 yy  close code
// 	{
// 		clampSafety.m_fHigh = ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_fVclampHigh;
// 		clampSafety.m_fLow = ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_fVclampLow;
// 		if(compactSafetyInfo->m_fHigh>=clampSafety.m_fHigh)
// 			compactSafetyInfo->m_fHigh = clampSafety.m_fHigh;

// 		if(compactSafetyInfo->m_fLow <= clampSafety.m_fLow)
// 			compactSafetyInfo->m_fLow = clampSafety.m_fLow;
// 	}

    //compactSafetyInfo->m_fHigh  += abs(compactSafetyInfo->m_fHigh)*0.015f;   //Add by DKQ 09.15.2015 放宽检查上下限
    //compactSafetyInfo->m_fLow   -= abs(compactSafetyInfo->m_fLow)*0.030f;

    unsigned char Range = ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_ucVoltageRange;  // 在下发网络命令运行或恢复时已赋值。
    if(Range > VOLTAGE_RANGE_HIGH)
        Range = VOLTAGE_RANGE_HIGH;
// 	compactSafetyInfo->m_fHigh  += abs(ChannelInfo_Manager::BoardCFG.m_Channel[uc_IVChnNo].m_VRange[Range].m_Bound.m_High)*0.015f; //0.015f;   // 以量程为基础放宽检查上下限   yy 20160216
// 	compactSafetyInfo->m_fLow   -= abs(ChannelInfo_Manager::BoardCFG.m_Channel[uc_IVChnNo].m_VRange[Range].m_Bound.m_High)*0.015f;	
	compactSafetyInfo->m_fHigh  += abs(ChannelInfo_Manager::BoardCFG.m_Channel[uc_IVChnNo].m_VRange[Range].m_Bound.m_High)*ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_RedundantVotlage.m_Safety_RedundantVoltage*0.01f;   // 放宽检查上下限   yy 20190723
	compactSafetyInfo->m_fLow   -= abs(ChannelInfo_Manager::BoardCFG.m_Channel[uc_IVChnNo].m_VRange[Range].m_Bound.m_High)*ChannelInfo_Manager::ChannelInfo[uc_IVChnNo].m_RedundantVotlage.m_Safety_RedundantVoltage*0.01f;	
}
