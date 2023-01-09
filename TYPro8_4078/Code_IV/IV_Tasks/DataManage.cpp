#include "../Entry/includes.h"
//public 属性:
// 设置变量定义在外存
//#if SUPPORT_AUX == 1
signed char             ChannelInfo_Manager::CurrRngMapVirtual2Physical[NUM_OF_IV_RANGE_I];
signed char             ChannelInfo_Manager::CurrRngMapPhysical2Virtual[NUM_OF_IV_RANGE_I];
unsigned char           ChannelInfo_Manager::CurrRngPhysicalRange1;
unsigned char           ChannelInfo_Manager::AuxLimitCheckOk;
//#endif
/*const unsigned char     ChannelInfo_Manager::FramConfigDictionary[32*1024/8] =
{
//hepeiqing:this dictionary need to be resetted when config getting stable.
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};*/

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
ST_PULSE_STAGE_DATA	    ChannelInfo_Manager::Pulse_Stage_Data[2][MAX_PULSE_STAGE_NUM];      //脉冲STAGE头尾数据，用于LIMIT CHECK
st_GetMetaValue         ChannelInfo_Manager::MetaVariable2[MAXCHANNELNO][MetaCode_MV_COUNT];
St_IV_Buffer1           ChannelInfo_Manager::IV_Buffer1[MAXCHANNELNO];
unsigned long           ChannelInfo_Manager::IV_Buffer1_Alpha;
unsigned long           ChannelInfo_Manager::IV_Buffer1_Beta;
//===============06.28.2017 TURBO BOOST BY ZC======================================================
MsTime									ChannelInfo_Manager::CCCV_TB_TimeBuffer[MAXCHANNELNO];
int											ChannelInfo_Manager::CCCV_TB_CVCounter[MAXCHANNELNO];
bool										ChannelInfo_Manager::CCCV_TB_EnableCounter[MAXCHANNELNO];
float										ChannelInfo_Manager::CCCV_TB_CurBuffer[MAXCHANNELNO];
int											ChannelInfo_Manager::CCCV_TB_LastState[MAXCHANNELNO];
float										ChannelInfo_Manager::CCCV_TB_IR_Volt[MAXCHANNELNO];
float										ChannelInfo_Manager::CCCV_TB_IR_Curr[MAXCHANNELNO];
int											ChannelInfo_Manager::CCCV_TB_IR_Counter[MAXCHANNELNO];
unsigned char 					ChannelInfo_Manager::CCCV_TB_LimitCount[MAXCHANNELNO];
//===============06.04.2018 CC TURBO BOOST BY ZC======================================================
MsTime									ChannelInfo_Manager::CC_TB_TimeBuffer[MAXCHANNELNO];
int											ChannelInfo_Manager::CC_TB_CVCounter[MAXCHANNELNO];
//==================================================================================================
//#if( SUPPORT_CAN_BMS==1)
ST_CANBMS_MAP          ChannelInfo_Manager::m_CANMapInfo;
ST_Config_Item         ChannelInfo_Manager::m_CANCofigItem;
ST_BMS_MetaValue        ChannelInfo_Manager::m_CANMetaVariable[MAXCHANNELNO][MAX_CAN_BMS_SIGNAL];
//#endif
ST_Config_Item 				 ChannelInfo_Manager::m_SMBCofigItem;
ST_SMB_MetaValue 				ChannelInfo_Manager::m_SMBusMetaVariable[MAXCHANNELNO][MAX_SMBUS_SIGNAL];

St_ChannelSchedule      ChannelInfo_Manager::ChannelSchedule[MAXCHANNELNO];                   // 通道Schedule信息 //
St_ChannelStep          ChannelInfo_Manager::ChannelStep[MAXCHANNELNO];
St_ChannelInfo          ChannelInfo_Manager::ChannelInfo[MAXCHANNELNO];                       // 通道状态信息
St_CapaEnerUpdated      ChannelInfo_Manager::CapaEnerUpdated[MAXCHANNELNO];                   // 标志容量、能量是否被刷新了。为了SDL减少计算量
St_BoardCFG             ChannelInfo_Manager::BoardCFG;
St_IV_ChannelPID        ChannelInfo_Manager::ChannelPID[MAXCHANNELNO];
St_CurFineTune  		ChannelInfo_Manager::m_CurFineTune_Info [MAXCHANNELNO];   //Add by DKQ 06.03.2014

Ctrl_Pid_Coef           ChannelInfo_Manager::m_Pid_Coef[MAXCHANNELNO];             				//04.23.2013
IV_BOARDTYPE_CFG        ChannelInfo_Manager::m_st_BoadType;
bool					ChannelInfo_Manager::m_DataFreshForPower[MAXCHANNELNO];   				//04.23.2013
bool					ChannelInfo_Manager::m_DataFreshForLoad[MAXCHANNELNO];   					//04.23.2013
bool  					ChannelInfo_Manager::m_DataFreshForVoltagePID[MAXCHANNELNO];      //04.23.2013
bool					ChannelInfo_Manager::m_DataFreshForVoltageClamp[MAXCHANNELNO];    //04.23.2013
bool					ChannelInfo_Manager::m_DataFreshForPID[MAXCHANNELNO];             //04.23.2013
// float                   ChannelInfo_Manager::m_TempProtectData[MAXADCHANNELNO];     //add by qjm20140612  用于温度补偿
unsigned char           ChannelInfo_Manager::m_ChannelIRJudge[MAXCHANNELNO];

//unsigned char           ChannelInfo_Manager::BuildIn_DI[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];  //161125 zyx
unsigned char           ChannelInfo_Manager::BuildIn_DO[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];
float					ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[MAXCHANNELNO];
float                   ChannelInfo_Manager::m_RC_VoltageCoefficient[MAXCHANNELNO];             //To set RC Filter coefficient
float                   ChannelInfo_Manager::m_RC_CurrentCoefficient[MAXCHANNELNO];
float 									ChannelInfo_Manager::m_RC_HistoryVoltageFilterValue[MAXCHANNELNO];  //Rc record history voltage value
float									 	ChannelInfo_Manager::m_RC_HistoryCurrentFilterValue[MAXCHANNELNO];	//RC record history current value
float                   ChannelInfo_Manager::m_RC_OutputValue[MAXCHANNELNO];					//RC output voltage value
//float 									ChannelInfo_Manager::m_RC_OutputCurrentValue[MAXCHANNELNO];					//RC output current value
float 									ChannelInfo_Manager::m_RC_CurrentFilterDeepth[MAXCHANNELNO];
float 									ChannelInfo_Manager::m_RC_VoltageFilterDeepth[MAXCHANNELNO];
bool  				 					ChannelInfo_Manager::m_NeedRCFilterFlg[MAXCHANNELNO];

// unsigned char						ChannelInfo_Manager::m_SlaveUnitCounter[MAX_GROUP_PER_UNIT];

//#if SUPPORT_PARALLEL == 1
St_Para_FeedBackCtrl    ChannelInfo_Manager::m_ParaFeedbackCtrl[MAXCHANNELNO];
St_UnitInfo				ChannelInfo_Manager::gUnit_stUnitInfo;
St_Serial2BoosterOutInfo    ChannelInfo_Manager::gUnit_stSeries2BoosterOutInfo[MAX_UNIT_PER_GROUP];
//#endif

Un_IsampStatus          ChannelInfo_Manager::Isamp_NeedRecalculateSign[MAXCHANNELNO];         // 电流采样相关变量 需要重新计算标志
Un_VsampStatus          ChannelInfo_Manager::Vsamp_NeedRecalculateSign[MAXCHANNELNO];         // 电压采样相关变量 需要重新计算标志

//private 属性:
St_ScheduleBuffer       ChannelInfo_Manager::ScheduleBuf[IV_MAX_SCHEDULE_BUFFER_NUMBER];      // Schedule缓冲区 //
St_PulseBuffer          ChannelInfo_Manager::PulseBuf[IV_MAX_PULSE_BUFFER_NUMBER];            // Pulse缓冲区 //
St_Pulse_Point		    ChannelInfo_Manager::PulseLimitArriveData[MAXCHANNELNO];
St_HistoryData          ChannelInfo_Manager::HistoryData[MAXCHANNELNO];                       // 通道历史数据 //
// St_HistoryData_ACR      ChannelInfo_Manager::HistoryDataACR;
St_Metavariable         ChannelInfo_Manager::MetaVariables[MAXCHANNELNO];                     // 通道Metavariable数据 //
float					ChannelInfo_Manager::RecordRefreshValueForParal[MAXCHANNELNO];
ST_IR_Info    ChannelInfo_Manager::m_IR_Info[MAXCHANNELNO];                   //To record internal resistence of a cell for auto PID calculating	  add by DKQ on 08.03.2016
ST_IV_Pair    ChannelInfo_Manager::m_IV_Pair[MAXCHANNELNO];                   //To remember current and voltage before tuansistion for IR calcuting  add by DKQ on 08.03.2016
St_SeriesBoosterMode    ChannelInfo_Manager::m_SeriesBoosterMode;
ST_FORMULA_VALUE         ChannelInfo_Manager::m_FormulaValue[MAXCHANNELNO][IV_MAX_SCHEDULE_FORMULA_NUMBER]; //170515 zyx

ST_BUILTIN_ADC_INFO      BuiltIn_DataManager::BuiltIn_ADC_Info[16];
ST_BuiltIn_DATA          BuiltIn_DataManager::m_BuiltInDataBank[2][MAX_AUXCH_PER_DATA_GROUP];
ST_INCHIP_TEMP           BuiltIn_DataManager::m_InChipTemperature[2];  //用于存放ADC片内温度
ST_BUILTIN_BOARD_INFO_IN_EEPROM       BuiltIn_DataManager::Aux_BuiltIn_BoardInfo;  //用于存取和读取E2PROM里的BUILTIN 板信息
ST_BUILTIN_BOARD_INFO_IN_EEPROM    BuiltIn_DataManager::Write_Aux_BuiltIn_BoardInfo;
ST_AUX_P2E_IN_UNIT       BuiltIn_DataManager::m_PChn2EChn [2][MAX_AUXCH_PER_DATA_GROUP];
bool                     BuiltIn_DataManager::m_DataGroupMgr[2];
bool                     BuiltIn_DataManager::m_Sample_Flag = false;
bool                     BuiltIn_DataManager::m_WriteFRAM_Flag = false;  //add by qjm 写FRAM 标志
unsigned short           BuiltIn_DataManager::m_uwSamplePauseFlag = 0;
bool                     BuiltIn_DataManager::b_BuiltInAux;           //是否有BuiltIn AUX 存在
bool                     BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = false; //add by qjm 20180112
ST_BUILTINAUX_Info       BuiltIn_DataManager::m_BuiltInAuxInfo;
unsigned short    BuiltIn_DataManager::BuiltIn_ADC_TimeOut_Flag;
unsigned char     BuiltIn_DataManager::BuiltIn_ADC_TotalCnt;
unsigned char     BuiltIn_DataManager::BuiltIn_ADC_Idx;
unsigned char     BuiltIn_DataManager::BuiltIn_ADC_GOTO_Idx;
const St_Port BuiltIn_DataManager::Pin_ECODE1		= {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BuiltIn_DataManager::Pin_ECODE2		= {2,        7,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BuiltIn_DataManager::Pin_ECODE3		= {2,        9,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port BuiltIn_DataManager::Pin_EOE		  = {1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
unsigned char BuiltIn_DataManager::m_SuspendFlg = 0;  // add by qjm 2018/01/31
unsigned char BuiltIn_DataManager::m_BuiltInCSFlag = 0; // Builtin ADC器件的片选信号，  0， 不操作片选，为高   1： 片选有效， 为低

//unsigned short          ChannelInfo_Manager::m_StepIndexUp[MAXCHANNELNO];
//unsigned short int      ChannelInfo_Manager::uw16_Setvariable_Counter[MAXCHANNELNO];
//unsigned short int      ChannelInfo_Manager::uw16_Setvariable[MAXCHANNELNO][IV_MAX_SCHEDULE_STEP_NUMBER/5];
unsigned long            ChannelInfo_Manager::m_DataFlag[MAXCHANNELNO]; //zyx 180309

bool 					ChannelInfo_Manager::Volt_Range_Change_Flag[MAXCHANNELNO];		//2电压量程切换标志
MsTime 				ChannelInfo_Manager::Voltage_Range_Change_Time[MAXCHANNELNO];	//2电压量程切等待时间
bool                    ChannelInfo_Manager::Is_Current_SoftWareClamp_Work [MAXCHANNELNO];
bool                    ChannelInfo_Manager::Is_Power_SoftWareClamp_Work[MAXCHANNELNO];
bool                    ChannelInfo_Manager::Is_ClampDigiVolt_Work[MAXCHANNELNO];
bool                    ChannelInfo_Manager::b_NeedFilter[MAXCHANNELNO];



#if DEBUG_TRACE_RECORD == 1
St_DebugTraceRecord     ChannelInfo_Manager::DebugTraceRecord;
#endif

//#if SUPPORT_AUX == 1
//AUX 增加相关参数20131119
ST_AUX_DATA   	        Channel_MapAuxInfo::m_AuxData[MAXCHANNELNO][MAXAUXTYPE][MAXVIRTUALCHANNEL];
unsigned char           Channel_MapAuxInfo::m_AuxDataRequested[MAXCHANNELNO][MAXAUXTYPE][MAXVIRTUALCHANNEL];
//ST_PC2IV_MAP          ChannelInfo_Manager::m_AUXmap[MAXCHANNELNO][MAXAUXTYPE];
ST_IV_MAPCONFIG         Channel_MapAuxInfo::m_AUXmap[MAXCHANNELNO][MAXAUXTYPE];

unsigned char           Channel_MapAuxInfo::CurrentStepAuxSum[MAXCHANNELNO][MAXUNITID];
unsigned char           Channel_MapAuxInfo::CurrentSafetyCount[MAXCHANNELNO][MAXUNITID];

//unsigned char           Channel_MapAuxInfo::m_BackUpAuxUnit[MAXCHANNELNO][MAXAUXTYPE][MAX_CH_IN_AUX_UNIT];
St_PC2IVUnit4AuxMap     Channel_MapAuxInfo::PC2IVUnit4AuxMap[MAXCHANNELNO];   // IVMCU的辅助配置和分配信息
St_BuildInAuxInfo       Channel_MapAuxInfo::BuildInAuxInfo; // 内部辅助通道物理映射信息
ST_AUX_SAFETY			Channel_MapAuxInfo::m_IV4AuxSafetyBank[MAXCHANNELNO][MAXAUXTYPE];     //IV专属条件下辅助安全参数存储
ST_AUX_SDL		    	Channel_MapAuxInfo::m_IV4AuxSDLBank[MAXCHANNELNO][MAXAUXTYPE]; //IV专属条件下辅助SDl参数存储
ST_AUX_DOCtrl           Channel_MapAuxInfo::m_AuxDoCtrlMgr[MAXCHANNELNO];
ST_AUX_AOCtrl           Channel_MapAuxInfo::m_AuxAOCtrlMgr[MAXCHANNELNO];   //add by qjm 2016.0816
float                   Channel_MapAuxInfo::m_AuxLastStepMetaValue[MAXCHANNELNO][4]; //0:ChargeEnergy 1:Discharge Energy 2:ChargeCapacity 3:DisChargeCapacity
unsigned short           Channel_MapAuxInfo::m_Total_In_OneIVCh[MAXCHANNELNO];    // 一个IV通道分到的辅助通道总数量
unsigned short          Channel_MapAuxInfo::IVChanMapAuxCount[MAXCHANNELNO][MAXAUXTYPE];   //Schedule used Aux chan counter. //180821 zyx

unsigned char           ChannelInfo_Manager::m_Online_UpdataEnable[MAXCHANNELNO];    // 通道在线更新使能
//#endif
float		           ChannelInfo_Manager::f_IR_Sourceback[MAXCHANNELNO];
float				   ChannelInfo_Manager::f_IR_Valueback[MAXCHANNELNO];
bool                   ChannelInfo_Manager::uc_useMetaCode_MV_UD[MAXCHANNELNO];
unsigned char 		   ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[MAXCHANNELNO] = {0};
unsigned char 		   ChannelInfo_Manager::uc_MV_Udx_Glb_RedundantVotlage[MAXCHANNELNO] = {0};
unsigned char 		   ChannelInfo_Manager::uc_MV_Udx_Update[MAXCHANNELNO] = {0};
#pragma arm section //结束外存变量定义  

MP_CAN_MSG  AUX_Msg;
void ChannelInfo_Manager::SetChanConfig(unsigned char uc_ChannelNo, St_ChannelCFG *mychancfg)
{
    BoardCFG.m_Channel[uc_ChannelNo] = *mychancfg;
    for(unsigned char rng = 0; rng < m_st_BoadType.m_ucIRangeCount; rng++) //rng is 虚拟量程
        BoardCFG.m_Channel[uc_ChannelNo].m_IRange[GetPhysicalCurrentRange(rng)] = mychancfg->m_IRange[rng];
}
void ChannelInfo_Manager::GetChanConfig(unsigned char uc_ChannelNo, St_ChannelCFG *mychancfg)
{
    *mychancfg = BoardCFG.m_Channel[uc_ChannelNo];
    for(unsigned char rng = 0; rng < m_st_BoadType.m_ucIRangeCount; rng++) //rng is 虚拟量程
        mychancfg->m_IRange[rng] = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[GetPhysicalCurrentRange(rng)];
}
void ChannelInfo_Manager::SetChanPID(unsigned char uc_ChannelNo, St_IV_ChannelPID *mychanpid)
{
    ChannelPID[uc_ChannelNo] = *mychanpid;
    for(unsigned char rng = 0; rng < m_st_BoadType.m_ucIRangeCount; rng++) //rng is 虚拟量程
        ChannelPID[uc_ChannelNo].m_VoltagePID[GetPhysicalCurrentRange(rng)] = mychanpid->m_VoltagePID[rng];
}
void ChannelInfo_Manager::GetChanPID(unsigned char uc_ChannelNo, St_IV_ChannelPID *mychanpid)
{
    *mychanpid = ChannelPID[uc_ChannelNo];
    for(unsigned char rng = 0; rng < m_st_BoadType.m_ucIRangeCount; rng++) //rng is 虚拟量程
        mychanpid->m_VoltagePID[rng] = ChannelPID[uc_ChannelNo].m_VoltagePID[GetPhysicalCurrentRange(rng)];
}


void ChannelInfo_Manager::LoadScheduleClamp(unsigned char uc_ChannelNo) //180825 zyx
{
    St_ScheduleSetting const *Setting = &ChannelSchedule[uc_ChannelNo].m_Settings;

    ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampLow = Setting->m_ClampVoltage_Simulation.m_fLow;
    ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampHigh = Setting->m_ClampVoltage_Simulation.m_fHigh;
    ChannelInfo[uc_ChannelNo].m_fPowerSimulationVclampHigh = Setting->m_ClampVoltage_Simulation.m_fHigh;
    ChannelInfo[uc_ChannelNo].m_fPowerSimulationVclampLow = Setting->m_ClampVoltage_Simulation.m_fLow; //180905 yy
    ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampLow = Setting->m_ClampVoltage_Simulation.m_ucDataTypeLow;
    ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampHigh = Setting->m_ClampVoltage_Simulation.m_ucDataTypeHigh; //zyx 180711


    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampHigh = Setting->m_ClampVoltage.m_fHigh;
    ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_fVclampLow = Setting->m_ClampVoltage.m_fLow;


    ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHighControlType = Setting->m_ClampPower.m_ucDataTypeHigh;
    ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHigh = Setting->m_ClampPower.m_fHigh;
    ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLowControlType = Setting->m_ClampPower.m_ucDataTypeLow;
    ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLow  = Setting->m_ClampPower.m_fLow;
    if(ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHighControlType == 0 && ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHigh == MAX_FLOAT \
            && ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLowControlType == 0 && ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLow == MIN_FLOAT) //180711 zyx
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_PowerClampRange.IsOpenClampRange = false;
    }
    else
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_PowerClampRange.IsOpenClampRange = true;
    }

    ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHighControlType = Setting->m_ClampCurrent.m_ucDataTypeHigh;
    ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHigh = Setting->m_ClampCurrent.m_fHigh;
    ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLowControlType = Setting->m_ClampCurrent.m_ucDataTypeLow;
    ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLow  = Setting->m_ClampCurrent.m_fLow;
    if(ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHighControlType == 0 && ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHigh == MAX_FLOAT \
            && ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLowControlType == 0 && ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLow == MIN_FLOAT) //180711 zyx
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CurrentClampRange.IsOpenClampRange = false;
    }
    else
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CurrentClampRange.IsOpenClampRange = true;
    }

    ChannelInfo[uc_ChannelNo].m_RedundantVotlage = Setting->m_Safety_RedundantVoltage;  // 20190723 yy
}

int ChannelInfo_Manager::LoadTestObject(unsigned char uc_ChannelNo)
{   //171113,hpq,add for pro8
    signed char    sc_SchBufIdx  = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return -1;     //schedule buffer index 指针越界，返回-1;
    if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
        return -1;     //schedule buffer 不合法，返回-1;

    const St_SchTestObject TestObject = ChannelSchedule[uc_ChannelNo].m_TestObject;
//
// 	ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange= GetMatchedVoltageRange(uc_ChannelNo,TestObject.m_fMaxVoltageCharge);
//
// 	ChannelInfo[uc_ChannelNo].m_fVclampHigh = TestObject.m_fVoltClampHigh;
// 	ChannelInfo[uc_ChannelNo].m_fVclampLow = TestObject.m_fVoltClampLow;
//   float voltage = Get_MetaVariable(uc_ChannelNo, MP_DATA_TYPE_METAVALUE, MetaCode_PV_Voltage);	// Get Voltage
//   unsigned char uc_CheckVClamp = safety_check::VClampRangeCheck(uc_ChannelNo,voltage);
//     if(!uc_CheckVClamp)   // 增加提前检查所设置的钳压是否正常 yy  20160216
//     {
//         EthernetParse::SendTasksMsgProcess(uc_ChannelNo, ST_ETHERNETPARSE, VClampUnsafe ,voltage); //钳压异常返回
//         return -1;
//     }
// 	ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampHigh = TestObject.m_ClampVoltage_Simulation.m_ucDataTypeHigh; //zyx 180711
// 	ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampHigh = TestObject.m_ClampVoltage_Simulation.m_fHigh;
// 	ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampLow = TestObject.m_ClampVoltage_Simulation.m_ucDataTypeLow;
// 	ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampLow = TestObject.m_ClampVoltage_Simulation.m_fLow;
//
// 	ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHighControlType = TestObject.m_ClampPower.m_ucDataTypeHigh;
// 	ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHigh = TestObject.m_ClampPower.m_fHigh;
// 	ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLowControlType = TestObject.m_ClampPower.m_ucDataTypeLow;
// 	ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLow  = TestObject.m_ClampPower.m_fLow;
// 	if(ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHighControlType == 0 && ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHigh == MAX_FLOAT \
// 		&& ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLowControlType == 0 && ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLow == MIN_FLOAT) //180711 zyx
// 	{
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_PowerClampRange.IsOpenClampRange = false;
// 	}
// 	else
// 	{
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_PowerClampRange.IsOpenClampRange = true;
// 	}
//
// 	ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHighControlType = TestObject.m_ClampCurrent.m_ucDataTypeHigh;
// 	ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHigh = TestObject.m_ClampCurrent.m_fHigh;
// 	ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLowControlType = TestObject.m_ClampCurrent.m_ucDataTypeLow;
// 	ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLow  = TestObject.m_ClampCurrent.m_fLow;
// 	if(ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHighControlType == 0 && ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHigh == MAX_FLOAT \
// 		&& ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLowControlType == 0 && ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLow == MIN_FLOAT) //180711 zyx
// 	{
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CurrentClampRange.IsOpenClampRange = false;
// 	}
// 	else
// 	{
// 		ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_CurrentClampRange.IsOpenClampRange = true;
// 	}
//
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_Mass, TestObject.m_fMass);
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_SpecificCapacity, TestObject.m_fSpecificCapacity);
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity, TestObject.m_fNorminalCapacity);
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalIR, TestObject.m_fNorminalIR);
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalVoltage, TestObject.m_fNorminalVoltage);
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacitor, TestObject.m_fNorminalCapacitor);
    if(MetaVariable2[uc_ChannelNo][MetaCode_MV_NorminalCapacity].m_fValue == 0)
        MetaVariable2[uc_ChannelNo][MetaCode_MV_NorminalCapacity].m_fValue = MetaVariable2[uc_ChannelNo][MetaCode_MV_Mass].m_fValue * MetaVariable2[uc_ChannelNo][MetaCode_MV_SpecificCapacity].m_fValue;
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_C_Rate, MetaVariable2[uc_ChannelNo][MetaCode_MV_NorminalCapacity].m_fValue);

    return 0;
}
unsigned char ChannelInfo_Manager::GetMatchedVoltageRange(unsigned char uc_ChannelNo, float fMaxVoltage)
{   //171113,hpq,add for pro8
    fMaxVoltage = ::abs(fMaxVoltage);
    unsigned char VoltRng = VOLTAGE_RANGE_HIGH;
    float f_V_LowRange = abs(BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VOLTAGE_RANGE_LOW].m_Bound.m_High);
//     float f_V_MedRange = abs(BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VOLTAGE_RANGE_MED].m_Bound.m_High);
//     float f_V_HighRange = abs(BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High);


    if(BoardCFG.m_Global.m_bHaveVoltLowRng && board_config::Func_Set_Volt_Range != 0)
    {
        if((fMaxVoltage <= f_V_LowRange))
        {   //High --> Low
// 				VoltRng = VOLTAGE_RANGE_LOW;
            VoltRng = board_config::Func_Set_Volt_Range(uc_ChannelNo,VOLTAGE_RANGE_LOW);

        }
//             else if((fMaxVoltage <= V_MedRange))
// 			{//High-->Medium
// //                 VoltRng = VOLTAGE_RANGE_MED;
//                VoltRng = board_config::Func_Set_Volt_Range(uc_ChannelNo,VOLTAGE_RANGE_MED);
//
//             }
        else
        {   //High-->High
// 				VoltRng = VOLTAGE_RANGE_HIGH;
            VoltRng = board_config::Func_Set_Volt_Range(uc_ChannelNo,VOLTAGE_RANGE_HIGH);
        }

        Volt_Range_Change_Flag[uc_ChannelNo] = 1;
        CommonFunc::GetTimeTicksUpdate(&Voltage_Range_Change_Time[uc_ChannelNo]);
    }
    else
        VoltRng = VOLTAGE_RANGE_HIGH;


    return VoltRng;
}
unsigned char ChannelInfo_Manager::GetMatchedCurrentRange(unsigned char uc_ChannelNo, float fMaxCurrCharge, St_StepCtrl *myctrl)
{   //171113,hpq,add for pro8
    float fMaxCurrent = myctrl->m_fMaxCurrent;
    unsigned char CurrRng = 0;

    if(fMaxCurrent == 0)
    {
        switch(myctrl->m_u16ControlType)
        {
        case CT_IVCH_CPULSE:
            unsigned short PulseID = myctrl->m_CtrlValues.m_fControlValue[0];// if it is PulseID  dirui 2014/0806
            fMaxCurrent = GetPulseMaxAmplitude(uc_ChannelNo, PulseID);
            break;
        case CT_IVCH_CCCV:
        case CT_IVCH_CCCV_WRM:
        case CT_IVCH_CURRENT:
            if(myctrl->m_ucCtrlValue_DataType[0] != MP_DATA_TYPE_FIGURE)
                fMaxCurrent = fMaxCurrCharge;
            else
                fMaxCurrent = myctrl->m_CtrlValues.m_fControlValue[0];
            break;
        case CT_IVCH_CRATE:
            if(myctrl->m_ucCtrlValue_DataType[0] != MP_DATA_TYPE_FIGURE)
                fMaxCurrent = ::abs(fMaxCurrCharge);
            else
            {
                fMaxCurrent = myctrl->m_CtrlValues.m_fControlValue[0];
                fMaxCurrent *= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity);
//                 fMaxCurrent = MYABS(fMaxCurrent);
            }
            break;
        case CT_IVCH_IR:
            if(abs(myctrl->m_CtrlValues.m_fControlValue[0] + myctrl->m_CtrlValues.m_fControlValue[2]) > abs(myctrl->m_CtrlValues.m_fControlValue[0] - myctrl->m_CtrlValues.m_fControlValue[2]))
                fMaxCurrent = myctrl->m_CtrlValues.m_fControlValue[0] + myctrl->m_CtrlValues.m_fControlValue[2];// IR = AMP + OFFSET //zyx 200430
            else
                fMaxCurrent = myctrl->m_CtrlValues.m_fControlValue[0] - myctrl->m_CtrlValues.m_fControlValue[2];
            break;
        case CT_SPECIAL_IRTEST://zyx 2105291
            fMaxCurrent = myctrl->m_CtrlValues.m_fControlValue[0] * ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity); //?????0.1C
            break;
        default:
            fMaxCurrent = fMaxCurrCharge;
            break;
        }
    }

// 	if(m_st_BoadType.m_ucIRangeCount >= 1)
// 		CurrRng = m_st_BoadType.m_ucIRangeCount - 1;
// 	else
    CurrRng = CURRENT_RANGE_1;
//     if(fMaxCurrent > 0)
//     {
    for(int VirtualRng = m_st_BoadType.m_ucIRangeCount - 1; VirtualRng >= 0; VirtualRng--)
    {
        if((fMaxCurrent <= BoardCFG.m_Channel[uc_ChannelNo].m_IRange[GetPhysicalCurrentRange(VirtualRng)].m_Bound.m_High)\
                &&(fMaxCurrent >= BoardCFG.m_Channel[uc_ChannelNo].m_IRange[GetPhysicalCurrentRange(VirtualRng)].m_Bound.m_Low))
        {
            CurrRng = VirtualRng;
            break;
        }
    }
    if((CurrRng == CURRENT_RANGE_1) && (::abs(fMaxCurrent) > Get_IRangeMaxValue(uc_ChannelNo, GetPhysicalCurrentRange(CURRENT_RANGE_1))))
        CurrRng = CURRENT_RANGE_PARALLELHIGH;
// 		for(int VirtualRng = 0;VirtualRng < m_st_BoadType.m_ucIRangeCount;VirtualRng++)
// 		{
// 			if(fMaxCurrent > Get_IRangeMaxValue(uc_ChannelNo,GetPhysicalCurrentRange(VirtualRng)))
// 			{
// 				if(VirtualRng == 0)
// 					CurrRng = CURRENT_RANGE_PARALLELHIGH;
// 				else
// 					CurrRng = VirtualRng;
// 				break;
// 			}
// 		}
//     }

    return CurrRng;
}
float ChannelInfo_Manager::Get_IRangeMaxValue(unsigned char uc_ChannelNo, unsigned char PhysicalCurrentRange)
{   //171113,hpq,add for pro8
    float fValue1 = ::abs(BoardCFG.m_Channel[uc_ChannelNo].m_IRange[PhysicalCurrentRange].m_Bound.m_High);
    float fValue2 = ::abs(BoardCFG.m_Channel[uc_ChannelNo].m_IRange[PhysicalCurrentRange].m_Bound.m_Low);
    return (fValue1 > fValue2) ? fValue1 : fValue2;
}
unsigned char ChannelInfo_Manager::GetPhysicalCurrentRange(unsigned char VirtualCurrentRange)//170320,add by hepeiqing
{
    if(VirtualCurrentRange == CURRENT_RANGE_PARALLELHIGH)
        VirtualCurrentRange = CURRENT_RANGE_1;
    if(VirtualCurrentRange >= m_st_BoadType.m_ucIRangeCount)
    {
        VirtualCurrentRange = m_st_BoadType.m_ucIRangeCount - 1;       //虚拟量程个数
        if(VirtualCurrentRange > CURRENT_RANGE_8)
            VirtualCurrentRange = CURRENT_RANGE_1;
    }

    signed char PhysicalRng = CurrRngMapVirtual2Physical[VirtualCurrentRange];
    if((PhysicalRng < 0) || (PhysicalRng >= board_config::DacInfo.m_bCurrRngCount))   //物理量程个数
    {
        PhysicalRng = board_config::DacInfo.m_bCurrRngCount - 1;
        if(PhysicalRng > CURRENT_RANGE_8)
            PhysicalRng = CURRENT_RANGE_1;
    }

    return PhysicalRng;
}
// unsigned char ChannelInfo_Manager::GetVirtualCurrentRange(unsigned char PhysicalCurrentRange)//170320,add by hepeiqing
// {
// 	if(PhysicalCurrentRange == CURRENT_RANGE_PARALLELHIGH)
// 		PhysicalCurrentRange = CURRENT_RANGE_1;
//
// 	signed char VirtualRng = CurrRngMapPhysical2Virtual[PhysicalCurrentRange];
// 	if( (VirtualRng < 0) || (VirtualRng > CURRENT_RANGE_8) )
// 		VirtualRng = CURRENT_RANGE_1;
//
// 	return VirtualRng;
// }

void ChannelInfo_Manager::InitVirtualCurrentRange(void)//170320,add by hepeiqing
{
    unsigned long CheckSum = m_st_BoadType.m_u16BoardType + m_st_BoadType.m_CheckSum;

    for(int i = 0; i < NUM_OF_IV_RANGE_I; i++)
    {
        CurrRngMapVirtual2Physical[i] = -1;
        CurrRngMapPhysical2Virtual[i] = -1;
        CheckSum += m_st_BoadType.m_IRangeVirtual2PhysicalMap[i];
    }

    if((CheckSum & 0xFF) != 0xFF)
    {
        for(int i = 0; i < NUM_OF_IV_RANGE_I; i++)
            m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] = i;
    }

    //unsigned char VirtualRng = CURRENT_RANGE_1; //0
//     if(m_st_BoadType.m_u16BoardType == BOARDTYPE_IBT31162) // 单向功率板处理  20161209
//     {
//         CurrRngMapVirtual2Physical[CURRENT_RANGE_1] = CURRENT_RANGE_1;
//         CurrRngMapVirtual2Physical[CURRENT_RANGE_2] = CURRENT_RANGE_2;
//
//         CurrRngMapPhysical2Virtual[CURRENT_RANGE_1] = CURRENT_RANGE_1;
//         CurrRngMapPhysical2Virtual[CURRENT_RANGE_2] = CURRENT_RANGE_2;

// 			  m_st_BoadType.m_ucIRangeCount = 2;
//
// 				memset(m_st_BoadType.m_IRange,0, NUM_OF_IV_RANGE_I*sizeof(St_IRange_BoardType));
// 				m_st_BoadType.m_IRange[CURRENT_RANGE_1].m_High	= 20;
// 				m_st_BoadType.m_IRange[CURRENT_RANGE_2].m_High	= 10;
//
//     }
//     else
//     {
    for(int i = 0; i < m_st_BoadType.m_ucIRangeCount; i++) //NUM_OF_IV_RANGE_I 170504 这样可以解决CurrRngMapPhysical2Virtual表 LFN1234，P2V为1237的问题
    {
//             if((m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] < 0) || (m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] >= board_config::DacInfo.m_bCurrRngCount))   //物理量程个数
        if(m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] >= board_config::DacInfo.m_bCurrRngCount)   //物理量程个数
        {
            m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] = board_config::DacInfo.m_bCurrRngCount - 1;
            if(m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] > CURRENT_RANGE_8)
                m_st_BoadType.m_IRangeVirtual2PhysicalMap[i] = CURRENT_RANGE_1;
        }
        CurrRngMapVirtual2Physical[i] = m_st_BoadType.m_IRangeVirtual2PhysicalMap[i];
        if((CurrRngMapVirtual2Physical[i] >= CURRENT_RANGE_1) && (CurrRngMapVirtual2Physical[i] <= CURRENT_RANGE_8))
            CurrRngMapPhysical2Virtual[CurrRngMapVirtual2Physical[i]] = i;
    }

//     }


    CurrRngPhysicalRange1 = GetPhysicalCurrentRange(CURRENT_RANGE_1);
}
void ChannelInfo_Manager::AddGlobalLoglimit(St_StepData *mystep, int equationIdx, float fLeftValue, float fRightValue, unsigned short GotoStepID, unsigned short LimitID,unsigned char ucDataType)
{
    mystep->m_Limits[equationIdx].m_bAnyValid             = false;
    mystep->m_Limits[equationIdx].m_bChecked              = false;
    mystep->m_Limits[equationIdx].m_bLimitFormulaHasAux   = false;
    mystep->m_Limits[equationIdx].m_bReached              = false;
    mystep->m_Limits[equationIdx].m_bStepLimit            = false;
    mystep->m_Limits[equationIdx].m_fLeftValue            = fLeftValue;
    mystep->m_Limits[equationIdx].m_fRightValue           = fRightValue;
    mystep->m_Limits[equationIdx].m_u16GotoStepID         = GotoStepID;
    mystep->m_Limits[equationIdx].m_u16LimitID            = LimitID;
    mystep->m_Limits[equationIdx].m_ucCompareSign         = MP_MCS_GREATER_THAN_OR_EQUAL_TO;
    mystep->m_Limits[equationIdx].m_ucEquationID          = equationIdx;
    mystep->m_Limits[equationIdx].m_uCheckPlace           = 0;
    mystep->m_Limits[equationIdx].m_ucLeftValue_DataType  = MP_DATA_TYPE_METAVALUE;
    mystep->m_Limits[equationIdx].m_ucRightValue_DataType = ucDataType;
    mystep->m_Limits[equationIdx].m_fLeftValueCoefficient = 1;
    mystep->m_Limits[equationIdx].m_fRightValueCoefficient = 1;
    mystep->m_Limits[equationIdx].m_ucLeftValue_Operator = 0;
    mystep->m_Limits[equationIdx].m_ucRightValue_Operator = 0;
    mystep->m_Limits[equationIdx].m_fLeft_Value = 0;
    mystep->m_Limits[equationIdx].m_fRight_Value = 0; //2011211 zyx fixed bug(forget initialization)
}
void ChannelInfo_Manager::Add_Global_Loglimit(unsigned char uc_ChannelNo,St_StepData *mystep,St_ChannelSchedule *mysch)
{
    bool bShouldHaveLogLimit = true;
    switch(mystep->m_Ctrl.m_u16ControlType)
    {
    case CT_IVCH_PAUSE:
    case CT_IVCH_SETVARIABLE:
    case CT_IVCH_IR:
    case CT_IVCH_ACI:
    case CT_IVCH_ACIM:
    case CT_IVCH_ACR:
    case CT_IVCH_ACIR:
    case CT_IVCH_SETVALUE:
    case CT_TYPE_CAN_SEND_MSG:
    case CT_IVCH_WRITECANOUTBOUND:   //190617,hpq
    case CT_TYPE_DO_SETTING:
        bShouldHaveLogLimit = false;
        break;
    default:
        bShouldHaveLogLimit = true;
        break;
    }

    if(bShouldHaveLogLimit && (mystep->m_Ctrl.m_ucLogLimitNo == 0))
    {
        int limitIdx = 0, equationIdx = mystep->m_Ctrl.m_ucStepLimitNo;
        float fValue = 0;
        if(mysch->m_Settings.m_DV_Voltage.m_bEnabled)
        {
            if(mysch->m_Settings.m_DV_Voltage.m_bByPercentage == 0)	//0: 固定数值
                fValue = mysch->m_Settings.m_DV_Voltage.m_fDelta;
            else if(mysch->m_Settings.m_DV_Voltage.m_bByPercentage == 1)//1: 百分比
                fValue = mysch->m_Settings.m_DV_Voltage.m_fDelta * mysch->m_TestObject.m_fMaxAllowVoltage / 100;
            else	//2: MV变量
                fValue = mysch->m_Settings.m_DV_Voltage.m_fDelta;
            fValue = ::abs(fValue);
            AddGlobalLoglimit(mystep, equationIdx, MetaCode_DV_Voltage, fValue, MCU_GOTOANDCONDICTION, limitIdx,mysch->m_Settings.m_DV_Voltage.m_ucDataType);
            equationIdx++;

            fValue = mysch->m_Settings.m_DV_Time_Min;
            AddGlobalLoglimit(mystep, equationIdx, MetaCode_DV_Time, fValue, MCU_GOTOLOG, limitIdx,MP_DATA_TYPE_FIGURE);
            equationIdx++;
            limitIdx++;
        }
        if(mysch->m_Settings.m_DV_Current.m_bEnabled)
        {
            if(mysch->m_Settings.m_DV_Current.m_bByPercentage == 0)
                fValue = mysch->m_Settings.m_DV_Current.m_fDelta;
            else if(mysch->m_Settings.m_DV_Current.m_bByPercentage == 1)
                fValue = mysch->m_Settings.m_DV_Current.m_fDelta * mysch->m_TestObject.m_fMaxCurrentCharge / 100;
            else
                fValue = mysch->m_Settings.m_DV_Current.m_fDelta;
            fValue = ::abs(fValue);
            AddGlobalLoglimit(mystep, equationIdx, MetaCode_DV_Current, fValue, MCU_GOTOANDCONDICTION, limitIdx,mysch->m_Settings.m_DV_Current.m_ucDataType);
            equationIdx++;

            fValue = mysch->m_Settings.m_DV_Time_Min;
            AddGlobalLoglimit(mystep, equationIdx, MetaCode_DV_Time, fValue, MCU_GOTOLOG, limitIdx,MP_DATA_TYPE_FIGURE);
            equationIdx++;
            limitIdx++;
        }
//     if(mysch->m_Settings.m_DV_Time_Max.m_bEnabled)
// 		{
        fValue = ::abs(mysch->m_Settings.m_DV_Time_Max);
        AddGlobalLoglimit(mystep, equationIdx, MetaCode_DV_Time, fValue, MCU_GOTOLOG, limitIdx,MP_DATA_TYPE_FIGURE);
        equationIdx++;
// 		}
        mystep->m_Ctrl.m_ucLogLimitNo = equationIdx - mystep->m_Ctrl.m_ucStepLimitNo;
    }
}
//public 方法：
/*unsigned long ChannelInfo_Manager::Read_AcrAfterFilter(void)
{
	unsigned long CodeAfterFilter = 0;
	unsigned short ItemCount = CQ::GetItemNumber(HistoryDataACR.m_uHnd);

	if(ItemCount == 0)
		return 0;                       //use 0 to sign get value failed;

	switch(ItemCount)
	{
	case POWER_OF(7):   //电流滤波缓冲区一般都处于满的状态，这个最容易遇到，放在第一个CASE，节省时间。
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 7;
		break;
	case POWER_OF(0):
		CodeAfterFilter = HistoryDataACR.m_u32Sum;
		break;
	case POWER_OF(1):
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 1;
		break;
	case POWER_OF(2):
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 2;
		break;
	case POWER_OF(3):
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 3;
		break;
	case POWER_OF(4):
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 4;
		break;
	case POWER_OF(5):
		CodeAfterFilter = HistoryDataACR.m_u32Sum >> 5;
		break;
	default:
		CodeAfterFilter = HistoryDataACR.m_u32Sum / ItemCount;
		break;
	}

	return CodeAfterFilter;
}
*/
void ChannelInfo_Manager::Init_HistoryBufQueue(unsigned short uc_ChannelNo)
{
    St_HistoryData * p_Data = &HistoryData[uc_ChannelNo];
    p_Data->m_uCurrFilterDepth = BoardCFG.m_Global.m_uFilterDepth_I;
    p_Data->m_uVoltFilterDepth = BoardCFG.m_Global.m_uFilterDepth_V;
    p_Data->m_uCurrFilterDepth++;
    p_Data->m_uVoltFilterDepth++;
    if(p_Data->m_uCurrFilterDepth > 256)
        p_Data->m_uCurrFilterDepth = 256;
    if(p_Data->m_uVoltFilterDepth > 256)
        p_Data->m_uVoltFilterDepth = 256;

    if(p_Data->m_uCurrHnd == 0)
        p_Data->m_uCurrHnd = CQ::Register(p_Data->m_uCurrBuf2, sizeof(float), p_Data->m_uCurrFilterDepth);
    if(p_Data->m_uVoltHnd == 0)
        p_Data->m_uVoltHnd = CQ::Register(p_Data->m_uVoltBuf2, sizeof(float), p_Data->m_uVoltFilterDepth);

    p_Data->m_u32CurrentSum = 0;
    p_Data->m_u32VoltageSum = 0;
    CQ::ResetQueueSize(p_Data->m_uCurrHnd, p_Data->m_uCurrFilterDepth);
    CQ::ResetQueueSize(p_Data->m_uVoltHnd, p_Data->m_uVoltFilterDepth);

// 	HistoryDataACR.m_uFilterDepth = 256; //128;
// 	if(HistoryDataACR.m_uHnd == 0)
// 		HistoryDataACR.m_uHnd = CQ::Register(HistoryDataACR.m_uBuf2,sizeof(float),HistoryDataACR.m_uFilterDepth);
// 	HistoryDataACR.m_u32Sum = 0;
// 	CQ::ResetQueueSize(HistoryDataACR.m_uHnd,HistoryDataACR.m_uFilterDepth);

    for(int i = 0; i < MAX_BUILDIN_2V_COUNT; i++)
    {
        p_Data->m_BuildInVolt[i].m_uFilterDepth = p_Data->m_uVoltFilterDepth;
        if(p_Data->m_BuildInVolt[i].m_uHandler == 0)
            p_Data->m_BuildInVolt[i].m_uHandler = CQ::Register(p_Data->m_BuildInVolt[i].m_uBuf, sizeof(unsigned long), p_Data->m_BuildInVolt[i].m_uFilterDepth);
        p_Data->m_BuildInVolt[i].m_u32Sum = 0;
        CQ::ResetQueueSize(p_Data->m_BuildInVolt[i].m_uHandler, p_Data->m_BuildInVolt[i].m_uFilterDepth);
    }
}
void ChannelInfo_Manager::Init()
{
    ChannelInfo_Manager::InitVirtualCurrentRange();//170428 zyx
    IV_Buffer1_Alpha = IV_BUFFER1_ALPHA;
    IV_Buffer1_Beta  = IV_BUFFER1_BETA * (2 ^ 24) * board_config::Accuracy;

    for(unsigned char i = 0; i < MAXCHANNELNO; i++)
        Schedule_Manager_task::IsNeedSetStopPoint[i] = false;

// 	HistoryDataACR.m_uHnd = 0;
    for(int i = 0; i < MAXCHANNELNO; i++)
    {
        Pid_Ctrl::fUpLimit[i] = 1.0f;
        Pid_Ctrl::fDnLimit[i] = -1.0f;
    }
    //#if (SUPPORT_PARALLEL == 1)
    //	InitUnitInfo();
    //	#endif
    for(unsigned char uc_i = 0; uc_i < IV_MAX_SCHEDULE_BUFFER_NUMBER; uc_i++)
        memset((void *)&ScheduleBuf[uc_i], 0, sizeof(ScheduleBuf[uc_i]));
    for (unsigned char uc_ChannelNo = 0; uc_ChannelNo < MAXCHANNELNO; uc_ChannelNo++)
    {
        HistoryData[uc_ChannelNo].m_uCurrHnd = 0;
        HistoryData[uc_ChannelNo].m_uVoltHnd = 0;
        for (int i = 0; i < MAX_BUILDIN_2V_COUNT; i++)
            HistoryData[uc_ChannelNo].m_BuildInVolt[i].m_uHandler = 0;
        Init_HistoryBufQueue(uc_ChannelNo);

        Init_Metavariable(uc_ChannelNo);
// 		Init_ChannelSchedule(uc_ChannelNo);
        ChannelSchedule[uc_ChannelNo].m_scBufIdx = -1;
        Init_ChannelStep(uc_ChannelNo);
        Init_HistoryData(uc_ChannelNo);
        InitChannelInfo(uc_ChannelNo);

        Set_VoltageRecalculateSign(uc_ChannelNo);
        Set_CurrentRecalculateSign(uc_ChannelNo);
        Set_CCapacityRecalculateSign(uc_ChannelNo);
        Set_DCapacityRecalculateSign(uc_ChannelNo);
        Set_CEnergyRecalculateSign(uc_ChannelNo);
        Set_DEnergyRecalculateSign(uc_ChannelNo);

// 		Init_PidCoef(uc_ChannelNo);            //04.23.2013  Initialize PID buffer
        m_Pid_Coef[uc_ChannelNo].Init();

        ChannelInfo[uc_ChannelNo].chn_Running_Flag = CHN_LOCK_IDLE;

        PulseLimitArriveData[uc_ChannelNo].bCriticalPoint = 0; //初始化 脉冲关键记录点
        PulseLimitArriveData[uc_ChannelNo].fValue = 0.0f;
        PulseLimitArriveData[uc_ChannelNo].fValue2 = 0.0f;

        m_RecordMasterChannelPVCurrent[uc_ChannelNo] = 0.0f;

        //#if( SUPPORT_CAN_BMS==1)
        memset((void*)&m_CANMetaVariable[uc_ChannelNo][0], 0, sizeof(ST_BMS_MetaValue)*MAX_CAN_BMS_SIGNAL);
        //#endif

// 		m_IR_Info[uc_ChannelNo].m_IR_Confirmed = false;   //Add by DKQ
// 		m_IR_Info[uc_ChannelNo].m_IR_from_Schedule = false;   //Add by DKQ
// 		m_IR_Info[uc_ChannelNo].m_IR_from_IRStep = false;   //Add by DKQ
// 		m_IR_Info[uc_ChannelNo].m_IR_from_Trans = false;   //Add by DKQ
// 		m_IR_Info[uc_ChannelNo].m_IR_from_Relax = false;   //Add by DKQ
//
// 		m_IR_Info[uc_ChannelNo].m_IR_by_Trans = 0;
// 		m_IR_Info[uc_ChannelNo].m_Trans_Voltage = 0;
// 		m_IR_Info[uc_ChannelNo].m_Trans_Counter = 0;
// 		m_IR_Info[uc_ChannelNo].m_Trans_Started = false;
//
// 		m_IR_Info[uc_ChannelNo].m_IR_by_Relax = 0;
// 		m_IR_Info[uc_ChannelNo].m_Relax_Voltage = 0;
// 		m_IR_Info[uc_ChannelNo].m_Relax_Counter = 0;
// 		m_IR_Info[uc_ChannelNo].m_Relaxed = false;
// 		m_IR_Info[uc_ChannelNo].m_IR_Stop_Logging = false;
//
// //============08232017 by ZC=============================
// 		m_IR_Info[uc_ChannelNo].m_IR_by_CCCV_TB = 0;
// 		m_IR_Info[uc_ChannelNo].m_IR_from_CCCV_TB = false;
// //========================================================

        memset(&m_IR_Info[uc_ChannelNo], 0, sizeof(m_IR_Info[uc_ChannelNo]));

        memset(&Volt_Range_Change_Flag[uc_ChannelNo],0,sizeof(Volt_Range_Change_Flag[uc_ChannelNo]));
        memset(&Voltage_Range_Change_Time[uc_ChannelNo],0,sizeof(Voltage_Range_Change_Time[uc_ChannelNo]));
        Is_Current_SoftWareClamp_Work[uc_ChannelNo] = false;
        Is_Power_SoftWareClamp_Work[uc_ChannelNo] = false;
        Is_ClampDigiVolt_Work[uc_ChannelNo] = false;

        m_RC_VoltageCoefficient[uc_ChannelNo] = 1;           //To set RC Filter coefficient
        m_RC_CurrentCoefficient[uc_ChannelNo] = 1;
        m_RC_HistoryVoltageFilterValue[uc_ChannelNo] = 0; //Rc record history voltage value
        m_RC_HistoryCurrentFilterValue[uc_ChannelNo] = 0;	//RC record history current value
        m_RC_OutputValue[uc_ChannelNo] = 0;					//RC output voltage value
        //	m_RC_OutputCurrentValue[uc_ChannelNo]=0;					//RC output current value
        m_RC_VoltageFilterDeepth[uc_ChannelNo] = 1;
        m_RC_CurrentFilterDeepth[uc_ChannelNo] = 1;
        m_NeedRCFilterFlg[uc_ChannelNo] = false;
//        m_StepIndexUp[uc_ChannelNo]=0XFFFF;
        m_Online_UpdataEnable[uc_ChannelNo] = false;
    }
// 	for(unsigned char i = 0; i < MAXADCHANNELNO; i++)
// 	{
// 		ChannelInfo_Manager::m_TempProtectData[i]=25.0;       //add by qjm 20140612
// 	}
    ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 = CHN_LOCK_IDLE;

    Set_TimeRecalculateSign();

    Init_ScheduleBuffer();
    Init_PulseBuffer();

//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
        Channel_MapAuxInfo::Init_AUXInfo();    //20131120  dirui
//#endif

#if DEBUG_TRACE_RECORD == 1
    InitDebugTraceRecord();
#endif

    memset(m_FormulaValue, 0, sizeof(ST_FORMULA_VALUE)*MAXCHANNELNO * IV_MAX_SCHEDULE_FORMULA_NUMBER); //170515 zyx

// 	return 1;
}

void ChannelInfo_Manager::InitChannelInfo(unsigned char uc_ChannelNo)
{
    unsigned char       m_GroupID_back = ChannelInfo[uc_ChannelNo].m_GroupID; // 并联已更新  06.14.2017 yy  备份
    memset (&ChannelInfo[uc_ChannelNo], 0, sizeof(St_ChannelInfo)); //Add by DKQ 03.11.2016
    ChannelInfo[uc_ChannelNo].m_GroupID = m_GroupID_back; // 并联已更新  06.14.2017 yy  恢复
    ChannelInfo[uc_ChannelNo].m_ucStatus = CHANNEL_INFO_CLOSE;
    ChannelInfo[uc_ChannelNo].m_u32DacValue = 0x800000;    //默认为0
    ChannelInfo[uc_ChannelNo].m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo[uc_ChannelNo].m_ucVoltageRange = VOLTAGE_RANGE_HIGH;
    ChannelInfo[uc_ChannelNo].m_fVclampHigh = 10000.0;
    ChannelInfo[uc_ChannelNo].m_fVclampLow = -10000.0;

    /*
    ChannelInfo[uc_ChannelNo].m_fNormalizedCurrent = 0;
    ChannelInfo[uc_ChannelNo].m_fNormalizedVoltage = 0;

    ChannelInfo[uc_ChannelNo].m_CtrlState=0;
    ChannelInfo[uc_ChannelNo].ScheduleStatus=0;
    ChannelInfo[uc_ChannelNo].StepStatus=0;
    ChannelInfo[uc_ChannelNo].m_SafetyStatus=0;
    ChannelInfo[uc_ChannelNo].StepLimitProcess=0;  //dirui add 20130122
    ChannelInfo[uc_ChannelNo].LogLimitProcess=0;  //dirui add 20130122

    ChannelInfo[uc_ChannelNo].EngineStatus=0;       //引擎状态  XXXXXX Error running    1代表 有效    0无效    comment out chen 20130123
    ChannelInfo[uc_ChannelNo].StartedReportNumber=0;
    ChannelInfo[uc_ChannelNo].StepTransationToStatus=0;
    ChannelInfo[uc_ChannelNo].DigitCtrl=0;
    ChannelInfo[uc_ChannelNo].Adjustment=0;
    ChannelInfo[uc_ChannelNo].DacReverse=0;
    ChannelInfo[uc_ChannelNo].SpecialProcess=0;
    ChannelInfo[uc_ChannelNo].ResumeTime=0;

    ChannelInfo[uc_ChannelNo].ScheduleUnsafeFlg=0;
    */
    //	#if SUPPORT_PARALLEL == 1
// 	ClearGroupSampleFlag();
    //#endif

// 	return 1;
}
void ChannelInfo_Manager::Init_BoardCFG(void)
{
    //load config from FRAM and validate the Config file
    //CConfig::LoadSysConfig();

    //IVSDL::Init();
//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
        Channel_MapAuxInfo::Init_AUXInfo();
//#endif
    if(BoardCFG.m_Global.m_u16IVChannelCount > MAXCHANNELNO)
        BoardCFG.m_Global.m_u16IVChannelCount = MAXCHANNELNO;

//     memset((void *)&Step_Manager_task::Unipolar,0,sizeof(St_Unipolar));

// 	return 1;
}
void ChannelInfo_Manager::Init_Metavariable(unsigned char uc_ChannelNo)
{
    float fVoltage = MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
    unsigned long keep_DataPoints =  MetaVariable2[uc_ChannelNo][MetaCode_PV_DataPoints].m_Value.m_uInt32; // add by DKQ
    memset(&MetaVariable2[uc_ChannelNo][0], 0, sizeof(MetaVariable2[uc_ChannelNo]));
    MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue = fVoltage;
    MetaVariable2[uc_ChannelNo][MetaCode_PV_DataPoints].m_Value.m_uInt32 = keep_DataPoints; //Add by DKQ
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    St_ChannelCFG * p_ChnCFG = &BoardCFG.m_Channel[uc_ChannelNo];
    p_MV2[MetaCode_PV_VmaxOnCycle].m_fValue = p_MV2[MetaCode_PV_Voltage].m_fValue;
// 	p_MV2[MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;
// 	p_MV2[MetaCode_DV_PulseCycle].m_fValue         = 0.0f;
// 	p_MV2[MetaCode_PV_PulseCycle].m_Value.m_uInt32 = 0; //若Resume到其他脉冲步怎么办？  dirui  2014/10/29
// 	p_MV2[MetaCode_PV_PulseCycle].m_fValue         = 0.0f;
    p_MV2[MetaCode_MV_IHmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_1)].m_Bound.m_Low;
    p_MV2[MetaCode_MV_IHmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_1)].m_Bound.m_High;
    p_MV2[MetaCode_MV_IMmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_2)].m_Bound.m_Low;
    p_MV2[MetaCode_MV_IMmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_2)].m_Bound.m_High;
    p_MV2[MetaCode_MV_ILmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_3)].m_Bound.m_Low;
    p_MV2[MetaCode_MV_ILmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_3)].m_Bound.m_High;
    p_MV2[MetaCode_MV_VHmin].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_Low;
    p_MV2[MetaCode_MV_VHmax].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
    p_MV2[MetaCode_MV_VLmin].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_LOW].m_Bound.m_Low;
    p_MV2[MetaCode_MV_VLmax].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_LOW].m_Bound.m_High;

// 	p_MV2[MetaCode_MV_UD1].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_MV_UD2].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_MV_UD3].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_MV_UD4].m_bNeedRecal = 0;

    //p_MV2[MetaCode_PV_Steptime].m_bNeedCalculate = 1;
// 	p_MV2[MetaCode_PV_Steptime].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Steptime].m_bNeedRecal = 1;

    //p_MV2[MetaCode_PV_TestTime].m_bNeedCalculate = 1;
// 	p_MV2[MetaCode_PV_TestTime].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_TestTime].m_bNeedRecal = 1;

//    p_MV2[MetaCode_PV_DataPoints].m_fValue = 0.0f; //mits8
//    p_MV2[MetaCode_PV_DataPoints].m_Value.m_uInt32 = 0;
    p_MV2[MetaCode_PV_TestTime].m_bNeedRecal = 1;
    //p_MV2[MetaCode_TC_Timer1].m_bNeedCalculate = 1;
    //161019 zyx
// 	p_MV2[MetaCode_PV_CV_StageTime].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_CV_StageTime].m_bNeedRecal = 0;
    p_MV2[MetaCode_PV_CV_StageTime].m_Value.m_Time = CommonFunc::GetSystemTime();//OS_Time;
    //161019 zyx
    //161226 zyx
// 	p_MV2[MetaCode_PV_CV_StageCurrent].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_CV_StageCurrent].m_bNeedRecal = 0;
    //161226 zyx
    p_MV2[MetaCode_TC_Timer1].m_bNeedRecal = 1;

    //p_MV2[MetaCode_TC_Timer2].m_bNeedCalculate = 1;
    p_MV2[MetaCode_TC_Timer2].m_bNeedRecal = 1;

    //p_MV2[MetaCode_TC_Timer3].m_bNeedCalculate = 1;
    p_MV2[MetaCode_TC_Timer3].m_bNeedRecal = 1;

    //p_MV2[MetaCode_TC_Timer4].m_bNeedCalculate = 1;
    p_MV2[MetaCode_TC_Timer4].m_bNeedRecal = 1;

    //p_MV2[MetaCode_TC_ChargeCapacity1].m_bNeedCalculate = 1;
// 	p_MV2[MetaCode_TC_ChargeCapacity1].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_ChargeCapacity1].m_bNeedRecal = 1; //Add by DKQ 03.15.2016

// 	p_MV2[MetaCode_TC_DischargeCapacity1].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_DischargeCapacity1].m_bNeedRecal = 1;

// 	p_MV2[MetaCode_TC_ChargeEnergy1].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_ChargeEnergy1].m_bNeedRecal = 1;  //Add by DKQ 03.15.2016

// 	p_MV2[MetaCode_TC_DischargeEnergy1].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_DischargeEnergy1].m_bNeedRecal = 1;

// 	p_MV2[MetaCode_TC_ChargeCapacity2].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_ChargeCapacity2].m_bNeedRecal = 1;  //Add by DKQ 03.15.2016

// 	p_MV2[MetaCode_TC_DischargeCapacity2].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_DischargeCapacity2].m_bNeedRecal = 1;

// 	p_MV2[MetaCode_TC_ChargeEnergy2].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_ChargeEnergy2].m_bNeedRecal = 1; //Add by DKQ 03.15.2016

// 	p_MV2[MetaCode_TC_DischargeEnergy2].m_fValue = 0.0f;
    p_MV2[MetaCode_TC_DischargeEnergy2].m_bNeedRecal = 1;

    p_MV2[MetaCode_DV_Time].m_bNeedRecal = 1;
    p_MV2[MetaCode_DV_Current].m_bNeedRecal = 1;
    p_MV2[MetaCode_DV_Voltage].m_bNeedRecal = 1;

// 	p_MV2[MetaCode_TC_Counter1].m_fValue = 0.0f;
// 	p_MV2[MetaCode_TC_Counter2].m_fValue = 0.0f;
// 	p_MV2[MetaCode_TC_Counter3].m_fValue = 0.0f;
// 	p_MV2[MetaCode_TC_Counter4].m_fValue = 0.0f;

    /*初始化当前步的MV变量*/
// 	p_MV2[MetaCode_PV_Current].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_StepIndex].m_fValue = 0.0f;
    // p_MV2[MetaCode_PV_CycleIndex].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_CycleIndex].m_fValue = 1;  //在mits pro4里，Cycle index从1开始，tc_counter从0开始，这里延续这个传统
// 	p_MV2[MetaCode_PV_ChargeCapacity].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_DischargeCapacity].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_ChargeCapacityTime].m_fValue = 0.0f; //zyx 171010
    p_MV2[MetaCode_PV_ChargeCapacityTime].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_PV_DischargeCapacityTime].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_DischargeCapacityTime].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_PV_ChargeEnergy].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_DischargeEnergy].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_InternalResistance].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_ACR].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_dVdt].m_fValue = 0.0f;

// 	p_MV2[MetaCode_PV_dIdt].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_PulseStageAveVol].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_PulseAveVol].m_fValue = 0.0f;

// 	p_MV2[MetaCode_PV_ChargeCapacity].m_Value.m_double = 0.0f;
// 	p_MV2[MetaCode_PV_DischargeCapacity].m_Value.m_double = 0.0f;
//     p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Second = 0;//zyx 171010
//     p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 = 0;
// 	p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Second = 0;//zyx 171010
//     p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 = 0;
// 	p_MV2[MetaCode_PV_ChargeEnergy].m_Value.m_double = 0.0f;
// 	p_MV2[MetaCode_PV_DischargeEnergy].m_Value.m_double = 0.0f;

// 	p_MV2[MetaCode_PV_UNIT_DI1].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_UNIT_DI2].m_fValue = 0.0f;

//	p_MV2[MetaCode_PV_UNIT_DI1].m_bNeedRecal = 1; //add by qjm for DI //161125 zyx
//	p_MV2[MetaCode_PV_UNIT_DI2].m_bNeedRecal = 1; //add by qjm for DI //161125 zyx
// 	p_MV2[MetaCode_PV_BuildIn_V1].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_BuildIn_V1].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_PV_BuildIn_V2].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_BuildIn_V2].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_PV_BuildIn_V3].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_BuildIn_V3].m_bNeedRecal = 0;
// 	p_MV2[MetaCode_PV_BuildIn_V4].m_fValue = 0.0f;
// 	p_MV2[MetaCode_PV_BuildIn_V4].m_bNeedRecal = 0;


// 	p_MV2[MetaCode_MV_Mass].m_fValue             = 0.0f;
// 	p_MV2[MetaCode_MV_SpecificCapacity].m_fValue = 0.0f;
// 	p_MV2[MetaCode_MV_NorminalCapacity].m_fValue = 0.0f;
// 	p_MV2[MetaCode_LS_ACR].m_fValue = 0.0f;
// 	p_MV2[MetaCode_LC_ACR].m_fValue = 0.0f;
// 	p_MV2[MetaCode_LS_InternalResistance].m_fValue = 0.0f;
// 	p_MV2[MetaCode_LC_InternalResistance].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_IV].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_AUX].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_DCDC].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_CANBMS].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_SMB].m_fValue = 0.0f;
    p_MV2[MetaCode_PV_Error_INVERTER].m_fValue = 0.0f;
//	MetaVariable_Task::Init_dVdt(uc_ChannelNo);
//	memset((void *)&BuildIn_DI[uc_ChannelNo][0],0,MAX_BUILDIN_DI_COUNT); //161125 zyx
    memset((void *)&BuildIn_DO[uc_ChannelNo][0], 0, MAX_BUILDIN_DI_COUNT);
    /*初始化上一步的MV变量*/
    //  memset((void *)&MetaVariables[uc_ChannelNo].m_LS_Info,0,sizeof(MetaVariables[uc_ChannelNo].m_LS_Info));
    /*初始化上一循环的MV变量*/
    //  memset((void *)&MetaVariables[uc_ChannelNo].m_LC_Info,0,sizeof(MetaVariables[uc_ChannelNo].m_LC_Info));
    /*初始化上一Log的MV变量*/
    //  memset((void *)&MetaVariables[uc_ChannelNo].m_LL_Info,0,sizeof(MetaVariables[uc_ChannelNo].m_LL_Info));
    /*初始化计数器、计时器的MV变量*/
    memset((void *)&MetaVariables[uc_ChannelNo].m_TC_Info, 0, sizeof(MetaVariables[uc_ChannelNo].m_TC_Info));
    /*
    MetaVariables[uc_ChannelNo].m_TC_Info.m_ChargeCapacity1.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_DischargeCapacity1.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_ChargeEnergy1.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_DischargeEnergy1.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_ChargeCapacity2.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_DischargeCapacity2.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_ChargeEnergy2.m_StartCapacity = 0.0f;
    MetaVariables[uc_ChannelNo].m_TC_Info.m_DischargeEnergy2.m_StartCapacity = 0.0f;
    */

    St_TC_Info * p_Info = &MetaVariables[uc_ChannelNo].m_TC_Info;
// 	p_Info->m_Timer1.m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	p_Info->m_Timer1.m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;
// 	p_Info->m_Timer2.m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	p_Info->m_Timer2.m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;
// 	p_Info->m_Timer3.m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	p_Info->m_Timer3.m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;
// 	p_Info->m_Timer4.m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	p_Info->m_Timer4.m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;

// 	ChannelSchedule[uc_ChannelNo].m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	ChannelSchedule[uc_ChannelNo].m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;
// 	ChannelStep[uc_ChannelNo].m_Info[0].m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	ChannelStep[uc_ChannelNo].m_Info[0].m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;
// 	ChannelStep[uc_ChannelNo].m_Info[1].m_StartTime.Second = OS_ServeProcess::OS_Time.Second;
// 	ChannelStep[uc_ChannelNo].m_Info[1].m_StartTime.Us100 = OS_ServeProcess::OS_Time.Us100;

    p_Info->m_Timer1.m_StartTime= CommonFunc::GetSystemTime();//OS_Time;
    p_Info->m_Timer2.m_StartTime = CommonFunc::GetSystemTime();
    p_Info->m_Timer3.m_StartTime = CommonFunc::GetSystemTime();
    p_Info->m_Timer4.m_StartTime = CommonFunc::GetSystemTime();
    ChannelSchedule[uc_ChannelNo].m_StartTime = CommonFunc::GetSystemTime();
    ChannelStep[uc_ChannelNo].m_Info[0].m_StartTime = CommonFunc::GetSystemTime();
    ChannelStep[uc_ChannelNo].m_Info[1].m_StartTime = CommonFunc::GetSystemTime();


    ChannelInfo[uc_ChannelNo].m_fResumeStepTime = 0;
    ChannelInfo[uc_ChannelNo].m_fResumeTestTime = 0;
    ChannelInfo[uc_ChannelNo].m_fResumeTC_Timer1 = 0;//2111011 zyx
    ChannelInfo[uc_ChannelNo].m_fResumeTC_Timer2 = 0;
    ChannelInfo[uc_ChannelNo].m_fResumeTC_Timer3 = 0;
    ChannelInfo[uc_ChannelNo].m_fResumeTC_Timer4 = 0;

    Pulse::MVinit();



// 	return 1;
}
void ChannelInfo_Manager::InitGroup4SDL(MP_BATCH_GROUP *pCmd)
{
    memset(IVSDL::m_GrpInfo4SDL,0,sizeof(IVSDL::m_GrpInfo4SDL));

    for(int i = 0; i < MAXCHANNELNO; i++)
        IVSDL::m_GrpInfo4SDL[i].State = eGroupState_Normal;

    for(int GrpIdx = 0; GrpIdx < pCmd->m_GroupCounter; GrpIdx++)
    {
        int ChanIdx = pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChn;

        int uc_ToUpdate = 0;
        if(pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 0)
        {   //master通道
            if(pCmd->m_UnitGroupInfo[GrpIdx].m_WholeGroupSize == 1)
            {   //GRP只有一个通道，即为单通道
                IVSDL::m_GrpInfo4SDL[ChanIdx].State          = eGroupState_Normal;
                continue;
            }
            else
            {   //GRP有多个通道，复杂一点
                IVSDL::m_GrpInfo4SDL[ChanIdx].State          = eGroupState_Master;
                IVSDL::m_GrpInfo4SDL[ChanIdx].SlaveCount     = pCmd->m_UnitGroupInfo[GrpIdx].m_WholeGroupSize - 1;
                IVSDL::m_GrpInfo4SDL[ChanIdx].SubmastCount   = pCmd->m_UnitGroupInfo[GrpIdx].m_uGroupUnitCount;
                for(int i = 0; i < IVSDL::m_GrpInfo4SDL[ChanIdx].SubmastCount; i++)
                {
                    IVSDL::m_GrpInfo4SDL[ChanIdx].Submaster[i].CanID     = pCmd->m_UnitGroupInfo[GrpIdx].m_SubMasterChannels[i].m_Unit_Idx + 1;
                    IVSDL::m_GrpInfo4SDL[ChanIdx].Submaster[i].ChanIdx   = pCmd->m_UnitGroupInfo[GrpIdx].m_SubMasterChannels[i].m_beSlaveUnit;
                }
                //以上代码搞定主通道初始化，下面搞定附带的从通道初始化
// 				for(int i = ChanIdx+1,slaveCount = 0;
// 					(i < BoardCFG.m_Global.m_u16IVChannelCount)&&(slaveCount < IVSDL::m_GrpInfo4SDL[ChanIdx].SlaveCount);
// 					i++,slaveCount++)
// 				{
// 					IVSDL::m_GrpInfo4SDL[i].State          = eGroupState_Slave;
// 					IVSDL::m_GrpInfo4SDL[i].Master.CanID   = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Unit_Idx;
// 					IVSDL::m_GrpInfo4SDL[i].Master.ChanIdx = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Chn_Idx;
// 				}
                uc_ToUpdate = 1;
            }
        }
        else if((pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 1) || (pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 3) || (pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 4))
        {   //submaster
            if(pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 1)
                IVSDL::m_GrpInfo4SDL[ChanIdx].State          = eGroupState_Submaster;
            else if(pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 3)
                IVSDL::m_GrpInfo4SDL[ChanIdx].State          = eGroupState_Serial;
            else if(pCmd->m_UnitGroupInfo[GrpIdx].m_HeadChnRank == 4)
                IVSDL::m_GrpInfo4SDL[ChanIdx].State          = eGroupState_Booster;

            IVSDL::m_GrpInfo4SDL[ChanIdx].Master.CanID   = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Unit_Idx;
            IVSDL::m_GrpInfo4SDL[ChanIdx].Master.ChanIdx = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Chn_Idx;
            IVSDL::m_GrpInfo4SDL[ChanIdx].SlaveCount     = pCmd->m_UnitGroupInfo[GrpIdx].m_InUnitGroupSize - 1;
            IVSDL::m_GrpInfo4SDL[ChanIdx].SubmastCount   = 0;
// 			//以上代码搞定主通道初始化，下面搞定附带的从通道初始化
// 			for(int i = ChanIdx+1,slaveCount = 0;
// 				(i < BoardCFG.m_Global.m_u16IVChannelCount)&&(slaveCount < IVSDL::m_GrpInfo4SDL[ChanIdx].SlaveCount);
// 				i++,slaveCount++)
// 			{
// 				IVSDL::m_GrpInfo4SDL[i].State          = eGroupState_Slave;
// 				IVSDL::m_GrpInfo4SDL[i].Master.CanID   = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Unit_Idx;
// 				IVSDL::m_GrpInfo4SDL[i].Master.ChanIdx = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Chn_Idx;
// 			}

            uc_ToUpdate = 1;
        }

        if(uc_ToUpdate)
        {
            // 下面搞定附带的从通道初始化
            for(int i = ChanIdx + 1, slaveCount = 0;
                    (i < BoardCFG.m_Global.m_u16IVChannelCount) && (slaveCount < IVSDL::m_GrpInfo4SDL[ChanIdx].SlaveCount);
                    i++, slaveCount++)
            {
                IVSDL::m_GrpInfo4SDL[i].State          = eGroupState_Slave;
                IVSDL::m_GrpInfo4SDL[i].Master.CanID   = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Unit_Idx;
                IVSDL::m_GrpInfo4SDL[i].Master.ChanIdx = pCmd->m_UnitGroupInfo[GrpIdx].m_Supervisor.m_Chn_Idx;
            }
        }
    }
}
//#if SUPPORT_PARALLEL == 1
void ChannelInfo_Manager::InitUnitInfo(unsigned char m_IP)
{
    gUnit_stUnitInfo.m_GroupCount = 99;
    //	gUnit_stUnitInfo.m_InterMasterUnitID=m_IP;
    gUnit_stUnitInfo.m_MyUnitID = m_IP;
    gUnit_stUnitInfo.m_InterMasterGroupID = 1;
    gUnit_stUnitInfo.m_SubMasterGroupID = 1;
    gUnit_stUnitInfo.m_ParaMode = eSingleMode;
    for(unsigned char uc_i = 0; uc_i < MAX_GROUP_PER_UNIT; uc_i++)
    {
        St_GroupInfo * p_Info = &gUnit_stUnitInfo.m_GroupInfo[uc_i];
        p_Info->m_UnitGroupInfo.m_HeadChn = uc_i;
        p_Info->m_UnitGroupInfo.m_HeadChnRank = eUnitMode_Para_Master;
        p_Info->m_UnitGroupInfo.m_InUnitGroupSize = 1;
        p_Info->m_UnitGroupInfo.m_uGroupUnitCount = 0;
        p_Info->m_UnitGroupInfo.m_WholeGroupSize = 1;
        p_Info->m_UnitGroupInfo.m_Supervisor.m_Unit_Idx = m_IP;
        p_Info->m_UnitGroupInfo.m_Supervisor.m_Chn_Idx = 0;
        p_Info->m_stGroupStatusInfo.m_bGroupCheckSafety = false;
        ChannelInfo_Manager::ChannelInfo[uc_i].m_GroupID = uc_i;
        for(unsigned char uc_j = 0; uc_j < MAX_UNIT_PER_GROUP; uc_j++)
        {
            //p_Info->m_UnitGroupInfo.m_SubMasterChannels[uc_j].m_Chn_Idx=0;
            p_Info->m_UnitGroupInfo.m_SubMasterChannels[uc_j].m_Unit_Idx = 0;
        }

        p_Info->m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_REST;
        p_Info->m_stSubGroupCtrl.m_fSubGroupCtrlValue = 0.0f;
        p_Info->m_stSubGroupCtrl.m_ucCANCMDType = CAN_CMD_PARA_MASTER_STOP;
        p_Info->m_stSubGroupCtrl.m_bSubGroupCtrlUpdated = FALSE;


        for(unsigned char uc_k = 0; uc_k < MAXUNITNO; uc_k++)
        {
            p_Info->m_bSubGroupDataValid[uc_k] = false;
            memset(&p_Info->m_stSubGroupData[uc_k], 0, sizeof(St_SubGroupData));
        }



    }

    //gUnit_stUnitInfo.m_eUnitGroupingMode = eGroupMode_Single; //eGroupMode_ParaInterUnit;// normal, intra-parallel, inter-parallel
    //gUnit_stUnitInfo.m_eUnitMode = eUnitMode_Normal;  //eUnitMode_Para_Master; //single channel running, intra-unit paralle, inter-unit parallel master unit, inter-unit paralle submaster unit.
    //gUnit_stUnitInfo.m_bMasterUnit = false; //bool
    //gUnit_stUnitInfo.m_uMasterUnitID = gUnit_stUnitInfo.m_uUnitID;
    //gUnit_stUnitInfo.m_uGroupUnitCount = 99;

    //gUnit_stUnitInfo.m_stGroupStatusInfo.m_bGroupCheckSafety = false;




}
//use enum TE_METAVARIABLE_CODE as the value type here received from sub-master
void  ChannelInfo_Manager::ProcessParaSubgroupDataReport(unsigned char uUnitID, float fValue, unsigned short uType)
{

    //	if (!ChannelInfo_Manager::IsInterUnitPara_MasterUnit()) //只有主unit才处理
    //	return;

    //只有属于本unit的master组内的unit,才会被处理
    if((uUnitID < gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_uUnitID ) &&  \
            (uUnitID > gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_uUnitID + 		\
             gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount))
        return;

    St_SubGroupData * p_Data = &gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_stSubGroupData[uUnitID - 1];

// 	switch (uType)
// 	{
// 	case MetaCode_PV_Voltage:
// 		p_Data->m_fSubGroupChanVoltage = fValue;
// 		p_Data->m_bSubGroupChanVoltageUpdated= true;
// 		break;

// 	case MetaCode_PV_Current:
// 		p_Data->m_fSubGroupChanCurrent = fValue;
// 		p_Data->m_bSubGroupChanCurrentUpdated= true;  //sub unit采样更新标志
// 		gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_bSubGroupDataValid[uUnitID-1] = true;  //表明换步后来了数据了from sub master
// 		break;

// 	case MetaCode_PV_ChargeCapacity:   //如果从unit报的话，如果不报，则在metavar task中，主通道计算事同时计算subunit的
// 		p_Data->m_fSubGroupChanCCapacity= fValue;
// 		p_Data->m_bSubGroupChanCCapacityUpdated= true;
// 		break;

// 	case MetaCode_PV_DischargeCapacity:
// 		p_Data->m_fSubGroupChanDCapacity= fValue;
// 		p_Data->m_bSubGroupChanDCapacityUpdated= true;
// 		break;

// 	case MetaCode_PV_ChargeEnergy:
// 		p_Data->m_fSubGroupChanCEnergy= fValue;
// 		p_Data->m_bSubGroupChanCEnergyUpdated= true;
// 		break;

// 	case MetaCode_PV_DischargeEnergy:
// 		p_Data->m_fSubGroupChanDEnergy= fValue;
// 		p_Data->m_bSubGroupChanDEnergyUpdated= true;
// 		break;

// 	default: break;
// 	}

    if(uType == MetaCode_PV_Current)
    {
        p_Data->m_fSubGroupChanCurrent = fValue;
        p_Data->m_bSubGroupChanCurrentUpdated = true; //sub unit采样更新标志
        gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::gUnit_stUnitInfo.m_InterMasterGroupID].m_bSubGroupDataValid[uUnitID - 1] = true; //表明换步后来了数据了from sub master
    }
    else
    {
        p_Data->m_fSubGroupChanVoltage = fValue;
        p_Data->m_bSubGroupChanVoltageUpdated = true;
    }

//     unsigned short uw16_Offset = uType%MetaCode_PV_Voltage;
//     if(uw16_Offset < 6)
//     {
//         bool  * Update = &p_Data->m_bSubGroupChanVoltageUpdated;
//         Update += uw16_Offset;
//         float * Value = &p_Data->m_fSubGroupChanVoltage;
//         Value += uw16_Offset;
//         * Update = true;
//         * Value = fValue;
//     }     //delete by qjm20171229

}



//
//查找通道自己的模式和角色：
// Unit内所有通道都一样的：
//
//判断当前通道是否为单通道模式--先获取通道所在组ID，再判断组内成员总数是否为1
bool  ChannelInfo_Manager::IsSingleChanMode(unsigned char uChanIdx)
{
    unsigned char index = ChannelInfo[uChanIdx].m_GroupID;
    return (gUnit_stUnitInfo.m_GroupInfo[index].m_UnitGroupInfo.m_WholeGroupSize == 1);
}
//判断当前unit所有通道是否都是单通道
bool  ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode()
{
    return (gUnit_stUnitInfo.m_GroupCount == BoardCFG.m_Global.m_u16IVChannelCount)
           && (gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_WholeGroupSize == 0x01) \
           && (gUnit_stUnitInfo.m_GroupInfo[BoardCFG.m_Global.m_u16IVChannelCount - 1].m_UnitGroupInfo.m_WholeGroupSize == 0x01);


}

//属于unit内部并联？
bool  ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitParaMode(unsigned char uChanIdx)
{
    return (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx == gUnit_stUnitInfo.m_MyUnitID)\
           && (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize == gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_WholeGroupSize);

}

//属于多unit并联？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitParaMode(unsigned char uChanIdx)
{
    return (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_uGroupUnitCount > 1);

}

//属于多unit并联的master unit？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitPara_MasterUnit(unsigned char uChanIdx)
{
    //return ((gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx==UnitID) && \
    //  (gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount>1))
    return (Serial2ParaMod_IsInterUnitParaMode(uChanIdx)  && (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx == gUnit_stUnitInfo.m_MyUnitID));
}

//属于多unit并联的sub-unit？

//多unit并联时的master通道？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(unsigned char uChanIdx)
{
    return (Serial2ParaMod_IsInterUnitPara_MasterUnit(uChanIdx)) && ( uChanIdx == gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn);
}

// unit内并联时的master通道？
bool  ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(unsigned char uChanIdx)
{
    return (Serial2ParaMod_IsIntraUnitParaMode(uChanIdx)) && (uChanIdx == gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn) ;
}

//master?
bool ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(unsigned char uChanIdx)
{
    return (Serial2ParaMod_IsInterUnitMasterChannel(uChanIdx) || Serial2ParaMod_IsIntraUnitMasterChannel(uChanIdx));
}

//unit内部并联的slave？
bool  ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitSlaveChannel(unsigned char uChanIdx)
{
    ST_UNIT_GROUP_INFO * p_Info = &gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo;
    return	(Serial2ParaMod_IsIntraUnitParaMode(uChanIdx)
             && (uChanIdx != p_Info->m_HeadChn))
            && (uChanIdx > p_Info->m_HeadChn)
            && (uChanIdx < p_Info->m_HeadChn + p_Info->m_InUnitGroupSize);
}



// 多unit并联，submaster通道？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(unsigned char uChanIdx)
{
    return (Serial2ParaMod_IsInterUnitParaMode(uChanIdx)) && \
           (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx != gUnit_stUnitInfo.m_MyUnitID) && \
           (uChanIdx == gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn);
}


bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterSubmasterChannel(unsigned char uChanIdx)
{
    return (Serial2ParaMod_IsInterUnitParaMode(uChanIdx)) &&
           (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn == uChanIdx)
           && (gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChnRank == 0x01);
    //|| ( ChannelInfo[uChanIdx].m_GroupingInfo.m_eChannelMode == eChannelMode_Master)); //多unit并联的master也要对自己unit的先做小组工作
}


//多unit并联的主unit 的slave？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMsUnitSlaveChannel(unsigned char uChanIdx)
{
    ST_UNIT_GROUP_INFO * p_Info = &gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo;
    return 	(Serial2ParaMod_IsInterUnitParaMode(uChanIdx))
            && (p_Info->m_uGroupUnitCount > 0)
            && (gUnit_stUnitInfo.m_MyUnitID == p_Info->m_Supervisor.m_Unit_Idx)
            && (uChanIdx > p_Info->m_HeadChn)
            && ( uChanIdx < (p_Info->m_HeadChn + p_Info->m_InUnitGroupSize) );
}

//多unit并联的sub-unit 的slave？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubUnitSlaveChannel(unsigned char uChanIdx)
{
    ST_UNIT_GROUP_INFO * p_Info = &gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo;
    return	(Serial2ParaMod_IsInterUnitParaMode(uChanIdx))
            && (p_Info->m_uGroupUnitCount > 0)
            && (gUnit_stUnitInfo.m_MyUnitID != p_Info->m_Supervisor.m_Unit_Idx)
            && ( uChanIdx > p_Info->m_HeadChn)
            && (uChanIdx < (p_Info->m_HeadChn + p_Info->m_InUnitGroupSize));
}

//slave 通道?

//所在unit是多unit并联的主unit？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInMasterUnit(void)
{
    return gUnit_stUnitInfo.m_MyUnitID == gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx && \
           (gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount > 0);
}
bool ChannelInfo_Manager::Serial2ParaMod_IsBoosterUnit(void)  //be used for SK
{
    return ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank == eSeriesBoosterRank_Booster;
}
bool ChannelInfo_Manager::Serial2ParaMod_IsSerialUnit(void)  //be used for SK
{
    return ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank == eSeriesBoosterRank_Serial;
}
//所在unit多unit并联的sub unit？
bool  ChannelInfo_Manager::Serial2ParaMod_IsInSubmasterUnit(void)
{
    return (gUnit_stUnitInfo.m_MyUnitID != gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx) && \
           (gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_SubMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount > 0);
}


unsigned char  ChannelInfo_Manager::FindMasterChanIdx(unsigned char uChanIdx)
{
    //single channel mode:
    if (Serial2ParaMod_IsSingleChanMode())
        return 0;

    //intra unit parallel, master unit:
    if (Serial2ParaMod_IsIntraUnitParaMode(uChanIdx))
        return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn;

    //inter unit parallel, sub master unit:
    if (Serial2ParaMod_IsInterUnitParaMode(uChanIdx) && gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChnRank==1)
        return 0;
    //inter unit parallel, master unit:
    if (Serial2ParaMod_IsInterUnitParaMode(uChanIdx) && gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChnRank==0)
        return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_HeadChn;

    return 0;
}



/****************************************************************************************************************************************************/
//
//找Master通道/Unit:
//
//unit内部并联，取主通道chanidx
//多unit并联，取主unit ID
/****************************************************************************************************************************************************/
/****************************************************************************************************************************************************/
//
// 取组内最后一个通道的通道号：如果是unit内并联，是整个group最后一个通道；
// 如果是多unit并联，是subgroup的最后一个通道号，其实就是unit的最后一个通道号
//
/****************************************************************************************************************************************************/
unsigned char  ChannelInfo_Manager::FindGroupLastChanIdx(unsigned char uChanIdx)
{
    unsigned char masteridx = FindMasterChanIdx(uChanIdx);
    unsigned char totalchannum = GetTotalChanNumOfParaGroup(uChanIdx);

    return masteridx + totalchannum - 1;
}

/****************************************************************************************************************************************************
//
//多unit并联，取unit个数：
//
****************************************************************************************************************************************************/
// unsigned char  ChannelInfo_Manager::GetUnitNumForInterUnitPara(unsigned char uc_ChannelNo)
// {
// 	//if (gUnit_stUnitInfo.m_eUnitGroupingMode < eGroupMode_ParaInterUnit)  //normal, or intra-unit parallel
// 	//	return 0;
// 	//else
// 	//	return ChannelInfo[0].m_GroupingInfo.m_uGroupChanCount / MAXCHANNELNO;
// 	return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_GroupID].m_UnitGroupInfo.m_uGroupUnitCount;
// }  //delete by qjm 20171222

// unsigned char  ChannelInfo_Manager::GetMasterUnitIDForInterUnitPara(unsigned char uChanIdx)
// {
// 	return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx;
// }

// unsigned char  ChannelInfo_Manager::GetLastUnitIDForInterUnitPara(unsigned char uChanIdx)
// {
// 	if(gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_uGroupUnitCount==1)
// 		return gUnit_stUnitInfo.m_MyUnitID;
// 	return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx + GetUnitNumForInterUnitPara(uChanIdx) - 1; //0-based
// }



void ChannelInfo_Manager::SetCurrentRangeParallelHigh(unsigned char uChanIdx, bool bFlag)
{
    ChannelInfo[uChanIdx].m_GroupCtrl.m_bCurrentRangeParallelHigh = bFlag;
    IVSDL::m_GrpInfo4SDL[uChanIdx].bMultiHigh = bFlag;
    return;
}

bool ChannelInfo_Manager::IsCurrentRangeParallelHigh(unsigned char uChanIdx)
{
    if(ChannelInfo_Manager::gUnit_stUnitInfo.m_ParaMode == eMixMode)
        return 1;
    else
        return ChannelInfo[uChanIdx].m_GroupCtrl.m_bCurrentRangeParallelHigh;
}

//161019 zyx
void ChannelInfo_Manager::ClearPV_CV_StageTime(unsigned char uChanIdx)
{
    MetaVariable2[uChanIdx][MetaCode_PV_CV_StageTime].m_bNeedRecal = 0;
    MetaVariable2[uChanIdx][MetaCode_PV_CV_StageTime].m_fValue = 0;
    MetaVariable2[uChanIdx][MetaCode_PV_CV_StageTime].m_Value.m_Time = CommonFunc::GetSystemTime();

    Step_Manager_task::IsSetCCCV_StageTime = false;
    memset(&Step_Manager_task::Voltavg, 0, sizeof(avg)*MAXCHANNELNO);
    memset(&Step_Manager_task::Curravg, 0, sizeof(avg)*MAXCHANNELNO);
    memset(&Step_Manager_task::CC_Current, 0, sizeof(float)*MAXCHANNELNO);


}
//161019 zyx

//161227 zyx
void ChannelInfo_Manager::ClearPV_CV_StageCurrent(unsigned char uChanIdx)
{
    MetaVariable2[uChanIdx][MetaCode_PV_CV_StageCurrent].m_fValue = 0;
}
//161227 zyx


/****************************************************************************************************************************************************
//
//取整个并联通道数：
//
****************************************************************************************************************************************************/
unsigned char  ChannelInfo_Manager::GetTotalChanNumOfParaGroup(unsigned char uChanIdx)
{
    if(Serial2ParaMod_IsIntraUnitMasterChannel(uChanIdx))
        return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_WholeGroupSize;
    else if(Serial2ParaMod_IsInterUnitMasterChannel(uChanIdx))
        return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_WholeGroupSize;
    else if(Serial2ParaMod_IsInterUnitSubmasterChannel(uChanIdx))
        return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_UnitGroupInfo.m_InUnitGroupSize;

    return 1;   //0  changed by DKQ 01.30.2017
}

/*
void ChannelInfo_Manager::ResetGroupData(void)
{
unsigned short chanidx ;

for (chanidx =0; chanidx < MAXCHANNELNO; chanidx ++)
{
memset(&ChannelInfo[chanidx].m_GroupData, 0, sizeof (St_GroupData));
}
}

void ChannelInfo_Manager::ResetSubGroupData(unsigned char groupID)
{
unsigned short unitidx ;

for (unitidx =0; unitidx < MAXUNITNO; unitidx ++)
{
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_bSubGroupDataValid[unitidx] = false;		// 对多unit并联，保存小组是否送来小组和更新。由CAN命令report data置T,整组求和完毕全部清0.

ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_fSubGroupChanCurrent = 0.0f; //多unit并联存放小组数据
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_fSubGroupChanVoltage = 0.0f; //多unit并联存放小组数据

ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanCurrentUpdated = false;
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanVoltageUpdated = false;
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanCCapacityUpdated = false;
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanDCapacityUpdated = false;
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanCEnergyUpdated = false;
ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[groupID].m_stSubGroupData[unitidx].m_bSubGroupChanDEnergyUpdated = false;
}

}
*/

/**************************************************************************************/
// 标识
//第一类:  start之后或者transition过后来了初值，表明现在有有效值了
//第二类:   新来了采样
/**************************************************************************************/

//only for inter-parallel submaster unit:
// bool  ChannelInfo_Manager::IfGroupStartSafetyCheck(unsigned char uChanIdx)
// {
// 	//if (gUnit_stUnitInfo.m_eUnitMode == eGroupMode_ParaIntraUnit)
// 	//TODO
// 	//	return true;	//TODO
// 	//	else if (gUnit_stUnitInfo.m_eUnitMode == eGroupMode_ParaInterUnit)
// 	//return gUnit_stUnitInfo.m_stGroupStatusInfo.m_bGroupCheckSafety;

// 	return gUnit_stUnitInfo.m_GroupInfo[ChannelInfo[uChanIdx].m_GroupID].m_stGroupStatusInfo.m_bGroupCheckSafety;
// }


//判断是否所有小组数据都更新完毕

/*void ChannelInfo_Manager::ClearGroupSampleFlag(void)
{
	unsigned short utypeidx;
	unsigned short chanidx;

	for (utypeidx = 1; utypeidx <= FLAG_TYPE_PARALLEL_8; utypeidx <<=1)
	{
		for (chanidx =0; chanidx < MAXCHANNELNO; chanidx ++)
		{
			ClearGroupSampleFlag(utypeidx, chanidx);
		}
	}

	return;
}*/
// void ChannelInfo_Manager::ClearGroupSampleFlag(unsigned short uFlagType)
// {
// 	unsigned short chanidx;

// 	for (chanidx =0; chanidx < MAXCHANNELNO; chanidx ++)
// 	{
// 		ClearGroupSampleFlag(uFlagType, chanidx);
// 	}

// 	return;
// }

/*void ChannelInfo_Manager::ClearGroupSampleFlag(unsigned short uFlagType, short uChanIdx)
{
	St_GroupData * p_GroupData = &ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData;
	if (uFlagType & FLAG_TYPE_PARALLEL_FRESHSAMPLECODE)
	{
		p_GroupData->m_bGroupNewCodeSampled = false;
	}

	if (uFlagType & FLAG_TYPE_PARALLEL_SAFETYCHECK)
	{
		p_GroupData->m_bGroupCurrentSampleFlag_SafetyCheck = false;
		p_GroupData->m_bGroupVoltageSampleFlag_SafetyCheck = false;
	}

	if (uFlagType & FLAG_TYPE_PARALLEL_LIMITCHECK)
	{
		p_GroupData->m_bGroupCurrentSampleFlag_LimitCheck = false;
		p_GroupData->m_bGroupVoltageSampleFlag_LimitCheck = false;
	}

	// 	m_bValidMetaVar[uChanIdx] = false;
	// 	m_bValidIVSDL[uChanIdx] = false;


	return;
}*/

/*
void ChannelInfo_Manager::SetGroupSampleFlag(void)
{
unsigned short utypeidx;
unsigned short chanidx;

for (utypeidx = 1; utypeidx <= FLAG_TYPE_PARALLEL_8; utypeidx >>2)
{
for (chanidx =0; chanidx < MAXCHANNELNO; chanidx ++)
{
SetGroupSampleFlag(utypeidx, chanidx);
}
}

return;
}
*/

// void ChannelInfo_Manager::SetGroupSampleFlag(unsigned short uFlagType)
// {
// 	unsigned short chanidx;

// 	for (chanidx =0; chanidx < MAXCHANNELNO; chanidx ++)
// 	{
// 		SetGroupSampleFlag(uFlagType, chanidx);
// 	}

// 	return;
// }

/*void ChannelInfo_Manager::SetGroupSampleFlag(unsigned short uFlagType, short uChanIdx)
{
	if (uFlagType & FLAG_TYPE_PARALLEL_FRESHSAMPLECODE)
	{
		ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData.m_bGroupNewCodeSampled = true;
	}

	if ((uFlagType & FLAG_TYPE_PARALLEL_SAFETYCHECK) || (uFlagType && FLAG_TYPE_PARALLEL_LIMITCHECK))
	{
		SetGroupSampleFlag_Current(uFlagType, uChanIdx);
		SetGroupSampleFlag_Voltage(uFlagType, uChanIdx);
	}

	return;
}

void ChannelInfo_Manager::SetGroupSampleFlag_Current(unsigned short uFlagType, unsigned short uChanIdx)
{
	if (uFlagType & FLAG_TYPE_PARALLEL_SAFETYCHECK)
	{
		ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData.m_bGroupCurrentSampleFlag_SafetyCheck = true;
	}

	if (uFlagType & FLAG_TYPE_PARALLEL_LIMITCHECK)
	{
		ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData.m_bGroupCurrentSampleFlag_LimitCheck = true;
	}

	return;
}

void ChannelInfo_Manager::SetGroupSampleFlag_Voltage(unsigned short uFlagType, unsigned short uChanIdx)
{
	if (uFlagType & FLAG_TYPE_PARALLEL_SAFETYCHECK)
	{
		ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData.m_bGroupVoltageSampleFlag_SafetyCheck = true;
	}

	if (uFlagType & FLAG_TYPE_PARALLEL_LIMITCHECK)
	{
		ChannelInfo_Manager::ChannelInfo[uChanIdx].m_GroupData.m_bGroupVoltageSampleFlag_LimitCheck = true;
	}

	return;
}
*/

/****************************************************************************************************************************************************
//
//  借用每个通道的safety_check::I_Sample_Flag[chan idx]来判断是不是所包含的通道都初采样完毕。这样后面计算采样才有效。
//
******************************************************************************************************************************************************/
/*
bool ChannelInfo_Manager::AllChannelSampledOnceForIntraUnitParallel(unsigned short uStartChanIdx, unsigned short uEndChanIdx)
{
bool btemp;
btemp = true;

unsigned char uchanid;

for (uchanid = uStartChanIdx; uchanid < uEndChanIdx;  uchanid++)
{
btemp = btemp && ChannelInfo_Manager::ChannelInfo[uchanid].Current_Sampleflg;
}

return btemp;
}
*/
/*
bool ChannelInfo_Manager::AllChannelSampledOnceForInterUnitParallel(void)
{
if (!IsCurrentRangeParallelHigh(0))  //if only H/M/L,only master is on, parallel only needs to check master's sample
{
return ChannelInfo[0].Current_Sampleflg;
}
else
{
bool btemp;
btemp = true;

//parallel high current, all channel on, need to get all submaster's current first sample after transition:
unsigned char uunitid, ustartuint, ulastunit;

ustartuint = gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_Supervisor.m_Unit_Idx;
ulastunit = gUnit_stUnitInfo.m_uGroupUnitCount + ustartuint -1;

for (uunitid = ustartuint; uunitid < ulastunit;  uunitid++)
{
btemp = btemp && gUnit_stUnitInfo.m_bSubGroupDataValid[uunitid];
}

return btemp;

}
}*/





//#endif  //SUPPORT_PARALLEL
//#if SUPPORT_AUX == 1
void Channel_MapAuxInfo::Init_AUXInfo()
{   //add by hepeiqing ,20131128
    // unsigned char  IvChannelIdx;
    // unsigned short AuxChCount_EachIV[MAXAUXTYPE];
    //    unsigned short AuxChCount_Already_Have = 0;
    //    ST_IV_MAPCONFIG *MyMap;
    for (unsigned char i = 0; i < MAXCHANNELNO; i++)
    {
        m_AuxLastStepMetaValue[i][0] = 0;
        m_AuxLastStepMetaValue[i][1] = 0;
        m_AuxLastStepMetaValue[i][2] = 0;
        m_AuxLastStepMetaValue[i][3] = 0;
    }
    //memset(&m_AUXmap[0][0],0,sizeof(m_AUXmap));
    //memset(&Channel_MapAuxInfo::m_AUXmap[0][0],0,sizeof(Channel_MapAuxInfo::m_AUXmap));
    return;
}
//#endif

float ChannelInfo_Manager::Read_SimuCycleTime(unsigned char uc_ChannelNo)
{
    return(fHowLongToNow(ChannelInfo[uc_ChannelNo].m_SimuInfo.m_CycleStartTime));
}
void ChannelInfo_Manager::Write_SimuCycleTime(unsigned char uc_ChannelNo, float fTime)
{
    ChannelInfo[uc_ChannelNo].m_SimuInfo.m_CycleStartTime = CalculatePreviousMsTime(fTime);
}
float ChannelInfo_Manager::Read_CvCycleTime(unsigned char uc_ChannelNo)
{
    return(fHowLongToNow(ChannelInfo[uc_ChannelNo].m_CvInfo.m_CycleStartTime));
}
void ChannelInfo_Manager::Write_CvCycleTime(unsigned char uc_ChannelNo, float fTime)
{
    ChannelInfo[uc_ChannelNo].m_CvInfo.m_CycleStartTime = CalculatePreviousMsTime(fTime);
}
// unsigned short ChannelInfo_Manager::Get_BoardChannelCount(void)
// {
// 	return BoardCFG.m_Global.m_u16IVChannelCount;    //本板配置的最大通道数;
// }
void ChannelInfo_Manager::Init_HistoryData(unsigned char uc_ChannelNo)
{   //将缓冲区的 指针 清0即可(表示缓冲区为空)，在初始化、换步的时候可以用到
    CQ::InitQueue(HistoryData[uc_ChannelNo].m_uCurrHnd);
    CQ::InitQueue(HistoryData[uc_ChannelNo].m_uVoltHnd);

    IV_Buffer1[uc_ChannelNo].m_uCurrCount = 0;
    IV_Buffer1[uc_ChannelNo].m_uVoltCount = 0;

//    MetaVariable_Task::ZaInitDvdt(uc_ChannelNo);//180921 zyx
    HistoryData[uc_ChannelNo].m_u32CurrentSum = 0;
    HistoryData[uc_ChannelNo].m_u32VoltageSum = 0;

    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        for(int i = 0; i < MAX_BUILDIN_2V_COUNT; i++)
        {
            CQ::InitQueue(HistoryData[uc_ChannelNo].m_BuildInVolt[i].m_uHandler);
            HistoryData[uc_ChannelNo].m_BuildInVolt[i].m_u32Sum = 0;
        }
    }

// 	return 1;
}

double ChannelInfo_Manager::Read_DoubleTime(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, MsTime SystemTime)
{
    double fValue = 0.0f;
    switch(uc_MetaCode)
    {
    case MetaCode_PV_Steptime:
        if(Schedule_Manager_task::IfSchRunning(uc_ChannelNo))
            fValue = dHowLongToNow(ChannelStep[uc_ChannelNo].m_Info[ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StartTime,SystemTime);
        else if(m_DataFlag[uc_ChannelNo] == STOP_POINT)
            fValue = dHowLongToNow(ChannelStep[uc_ChannelNo].m_Info[ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StartTime,SystemTime);
        else
            fValue = 0;
        break;
    case MetaCode_PV_TestTime:
        if(Schedule_Manager_task::IfSchRunning(uc_ChannelNo))
            fValue = dHowLongToNow(ChannelSchedule[uc_ChannelNo].m_StartTime,SystemTime);
        else if(m_DataFlag[uc_ChannelNo] == STOP_POINT)
            fValue = dHowLongToNow(ChannelSchedule[uc_ChannelNo].m_StartTime,SystemTime);
        else
            fValue = 0;
        break;
    default:
        fValue = 0;
        break;
    }
    return fValue;
}

float ChannelInfo_Manager::Read_SpecifiedMetavariable2(unsigned char uc_ChannelNo, unsigned char uc_MetaCode)
{
    if(uc_MetaCode >= MetaCode_MV_COUNT)
        return 0.0f;

    st_GetMetaValue *mV2 = &MetaVariable2[uc_ChannelNo][uc_MetaCode];
    if(mV2->m_bNeedRecal)
        RefreshMetaVariables(uc_ChannelNo, uc_MetaCode);  //这句代码，大部分情况下，不会被执行。	//每隔1.6ms更新时间

    return mV2->m_fValue;
}

void ChannelInfo_Manager::RefreshMetaVariables(unsigned char uc_ChannelNo, unsigned char uc_MetaCode)
{
    St_Metavariable * mVCH = &MetaVariables[uc_ChannelNo];
    float * mv2_fValue = &MetaVariable2[uc_ChannelNo][uc_MetaCode].m_fValue;
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    unsigned char uc_MetaOffset = 0;

    MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 0;
    //这里处理一些特殊的变量，需要 定时、周期性计算得到结果，如果用不到，就不需要计算的。
    switch(uc_MetaCode)
    {
//161125 zyx
// 	case MetaCode_PV_UNIT_DI1:   //对于DI的特殊处理
// 		*mv2_fValue = BuildIn_DI[uc_ChannelNo][0];
// 	  MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 1;//add by XP for DI
// 		return;
// 	case MetaCode_PV_UNIT_DI2:   //对于DI的特殊处理
// 		*mv2_fValue =BuildIn_DI[uc_ChannelNo][1];
// 	  MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 1;//add by XP for DI
//		return;
//161125 zyx

    case MetaCode_DV_Time:
        *mv2_fValue = fHowLongToNow(mVCH->m_DV_Info.m_LastLogTime);
        return;
    case MetaCode_PV_Steptime:
        if(Schedule_Manager_task::IfSchRunning(uc_ChannelNo))
            *mv2_fValue = fHowLongToNow(ChannelStep[uc_ChannelNo].m_Info[ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StartTime);
        else
            *mv2_fValue = 0;
        return;
    case MetaCode_PV_TestTime:
        if(Schedule_Manager_task::IfSchRunning(uc_ChannelNo))
            *mv2_fValue = fHowLongToNow(ChannelSchedule[uc_ChannelNo].m_StartTime);
        else
            *mv2_fValue = 0;
        return;
    case MetaCode_DV_Current:
        *mv2_fValue = ::fabs(p_MV2[MetaCode_PV_Current].m_fValue - mVCH->m_DV_Info.m_fLastLogCurrent);
        return;
    case MetaCode_DV_Voltage:
        *mv2_fValue = ::fabs(p_MV2[MetaCode_PV_Voltage].m_fValue - mVCH->m_DV_Info.m_fLastLogVoltage);
        return;
    case MetaCode_TC_Timer1:
// 		*mv2_fValue = fHowLongToNow(mVCH->m_TC_Info.m_Timer1.m_StartTime);
// 		return;
    case MetaCode_TC_Timer2:
// 		*mv2_fValue = fHowLongToNow(mVCH->m_TC_Info.m_Timer2.m_StartTime);
// 		return;
    case MetaCode_TC_Timer3:
// 		*mv2_fValue = fHowLongToNow(mVCH->m_TC_Info.m_Timer3.m_StartTime);
// 		return;
    case MetaCode_TC_Timer4:
// 		*mv2_fValue = fHowLongToNow(mVCH->m_TC_Info.m_Timer4.m_StartTime);
// 		return;

        St_TimerRegister * TimerRegister = &mVCH->m_TC_Info.m_Timer1;
        uc_MetaOffset = uc_MetaCode % MetaCode_TC_Timer1;
        TimerRegister += uc_MetaOffset;
        *mv2_fValue = fHowLongToNow(TimerRegister->m_StartTime);
        return;
    case MetaCode_TC_ChargeCapacity1:
// 		*mv2_fValue = mVCH->m_TC_Info.m_ChargeCapacity1.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		return;
    case MetaCode_TC_ChargeCapacity2:
// 		*mv2_fValue = mVCH->m_TC_Info.m_ChargeCapacity2.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		return;
    case MetaCode_TC_DischargeCapacity1:
// 		*mv2_fValue = mVCH->m_TC_Info.m_DischargeCapacity1.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		return;
    case MetaCode_TC_DischargeCapacity2:
// 		*mv2_fValue = mVCH->m_TC_Info.m_DischargeCapacity2.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		return;
    case MetaCode_TC_ChargeEnergy1:
// 		*mv2_fValue = mVCH->m_TC_Info.m_ChargeEnergy1.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		return;
    case MetaCode_TC_ChargeEnergy2:
// 		*mv2_fValue = mVCH->m_TC_Info.m_ChargeEnergy2.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		return;
    case MetaCode_TC_DischargeEnergy1:
// 		*mv2_fValue = mVCH->m_TC_Info.m_DischargeEnergy1.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		return;
    case MetaCode_TC_DischargeEnergy2:
// 		*mv2_fValue = mVCH->m_TC_Info.m_DischargeEnergy2.m_StartCapacity
// 			+ p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		return;

        St_CapacityRegister * CapacityRegister = &mVCH->m_TC_Info.m_ChargeCapacity1;
        uc_MetaOffset = uc_MetaCode % MetaCode_TC_ChargeCapacity1;
        CapacityRegister += uc_MetaOffset;
        *mv2_fValue = CapacityRegister->m_StartCapacity
                      + p_MV2[MetaCode_PV_ChargeCapacity + uc_MetaOffset / 2].m_fValue;
        return;
    //161019 zyx
    case MetaCode_PV_CV_StageTime:
        st_GetMetaValue *mv2 = &MetaVariable2[uc_ChannelNo][uc_MetaCode];
        *mv2_fValue =  fHowLongToNow( mv2->m_Value.m_Time );
        mv2->m_bNeedRecal = 1;
        return;
    //161019 zyx

    case MetaCode_PV_ChargeCapacityTime: //zyx 171010
//       *mv2_fValue = p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Second + p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 * 1.0e-4f;
//       //mv2->m_bNeedRecal = 1;
// 			MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 1;
//         return;
    case MetaCode_PV_DischargeCapacityTime:
//          *mv2_fValue = p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Second + p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 * 1.0e-4f;
//          // mv2->m_bNeedRecal = 1;
// 		 MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 1;

        uc_MetaOffset = uc_MetaCode % MetaCode_PV_ChargeCapacityTime;
        *mv2_fValue = p_MV2[MetaCode_PV_ChargeCapacityTime + uc_MetaOffset].m_Value.m_Time2us.Second + p_MV2[MetaCode_PV_ChargeCapacityTime  + uc_MetaOffset].m_Value.m_Time2us.Us * 1.0e-6f;
        MetaVariable2[uc_ChannelNo][uc_MetaCode].m_bNeedRecal = 1;
        return;
    default:
        return;
    }
}
float ChannelInfo_Manager::Read_SpecifiedMetavariable4(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, unsigned char Type)
{
// 	float f_Value1,f_Value2;
    float f_Value1 = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage + Type % 2);
    float f_Value2 = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);
    switch(uc_MetaCode)
    {
    case MAXCOMPVALUE:
        switch(Type)
        {
        case SAFETY_V:
            //专门为脉冲读取参数服务
            //快脉冲测试应该采用直接比较最大/最小值的方式，而不是每次都去检查一遍
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);  //还要加上同PV值比较的处理，这样对于慢脉冲的话不会有遗漏
            if(Check_V_Limit_task::Max_PulseVoltage >= f_Value1)
                return Check_V_Limit_task::Max_PulseVoltage;
            else
                return f_Value1;
        case SAFETY_I:
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
            if(Check_V_Limit_task::Max_PulseCurrent >= f_Value1)
                return Check_V_Limit_task::Max_PulseCurrent;
            else
                return f_Value1;
        case SAFETY_P:
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
// 			f_Value2= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);
            if(Check_V_Limit_task::Max_PulsePower >= f_Value1 * f_Value2)
                return Check_V_Limit_task::Max_PulsePower;
            else
                return f_Value1 * f_Value2;
        default:
            return 0;
        }

    //		break;
    case MINCOMPVALUE:
        switch(Type)
        {
        case SAFETY_V:
            //专门为脉冲读取参数服务
            //快脉冲测试应该采用直接比较最大/最小值的方式，而不是每次都去检查一遍
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);  //还要加上同PV值比较的处理，这样对于慢脉冲的话不会有遗漏
            if(Check_V_Limit_task::Min_PulseVoltage < f_Value1)
                return Check_V_Limit_task::Min_PulseVoltage;
            else
                return f_Value1;
        case SAFETY_I:
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
            if(Check_V_Limit_task::Min_PulseCurrent<f_Value1)//Max/Min接口由何培青提供 20140825
                return Check_V_Limit_task::Min_PulseCurrent;
            else
                return f_Value1;
        case SAFETY_P:
// 			f_Value1= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Current);
// 			f_Value2= ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Voltage);
            if(Check_V_Limit_task::Min_PulsePower < f_Value1 * f_Value2)
                return Check_V_Limit_task::Min_PulsePower;
            else
                return f_Value1 * f_Value2;
        default:
            return 0;
        }

        //		break;

    }
    return 0;
}


float ChannelInfo_Manager::GetLimitValue(St_Limit * thisLimit, unsigned char ChannelIdx, bool LeftOrRight)////zyx 2102251
{
    unsigned char Type = 0;
    float fInputValue = 0;
    float fOutputValue = 0;
    unsigned char ucValue_Operator = 0;
    float fValueCoefficient = 0;
    unsigned char uc_AuxType = 0xFF;
    unsigned char uc_AuxDxDt = false;
    if(LeftOrRight == LIMIT_LEFT_VALUE)
    {
        Type = thisLimit->m_ucLeftValue_DataType;
        fInputValue = thisLimit->m_fLeftValue;
        ucValue_Operator = thisLimit->m_ucLeftValue_Operator;
        fValueCoefficient = thisLimit->m_fLeftValueCoefficient;
    }
    else
    {
        Type = thisLimit->m_ucRightValue_DataType;
        fInputValue = thisLimit->m_fRightValue;
        ucValue_Operator = thisLimit->m_ucRightValue_Operator;
        fValueCoefficient = thisLimit->m_fRightValueCoefficient;

    }


    switch(Type)  //calculate left value;
    {
    case MP_DATA_TYPE_METAVALUE:
        fOutputValue = ChannelInfo_Manager::Read_SpecifiedMetavariable2(ChannelIdx, fInputValue);
        fOutputValue = ChannelInfo_Manager::CoefficientCalculate(ucValue_Operator,fValueCoefficient, fOutputValue);  // 20190917 yy
        break;
    case MP_DATA_TYPE_FORMULA:
    case MP_DATA_TYPE_CANBMS:
    case MP_DATA_TYPE_SMBBMS:
    case MP_DATA_TYPE_ADVFORMULA:
        fOutputValue = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx, Type, ucValue_Operator,fValueCoefficient,fInputValue);   // 20190917 yy
        break;

    case MP_DATA_TYPE_AUX_T:
        uc_AuxType = AUX_TEMPERATURE;
        break;
    case MP_DATA_TYPE_AUX_dTdt:
        uc_AuxType = AUX_TEMPERATURE;
        uc_AuxDxDt = true;
        break;
    case MP_DATA_TYPE_AUX_V:
        uc_AuxType = AUX_VOLTAGE;
        break;
    case MP_DATA_TYPE_AUX_dVdt:
        uc_AuxType = AUX_VOLTAGE;
        uc_AuxDxDt = true;
        break;
    case MP_DATA_TYPE_AUX_DI:
        uc_AuxType = AUX_DI;
        break;
    case MP_DATA_TYPE_AUX_DO:
        uc_AuxType = AUX_DO;
        break;
    case MP_DATA_TYPE_AUX_FlowRate:
        uc_AuxType = AUX_FLOW_RATE;
        break;

    case MP_DATA_TYPE_AUX_Humidity:
        uc_AuxType = AUX_HUMIDITY;
        break;
    case MP_DATA_TYPE_EC:
        uc_AuxType = AUX_EXTERNALCHARGE;
        break;

    case MP_DATA_TYPE_FIGURE:
    default:
        fOutputValue = fInputValue;
        break;
    }
    if(uc_AuxType != 0xFF)
    {
        Check_Limit4_task::GetAuxValue(thisLimit,uc_AuxType,uc_AuxDxDt,&fInputValue,ChannelIdx,ucValue_Operator,fValueCoefficient);
        fOutputValue = fInputValue;
    }
    return fOutputValue;
}

float ChannelInfo_Manager::Calculate_ADVFormula(unsigned char uc_ChannelNo, St_AdvFormula *Formula)
{
    float RPN_Data[64] = {0};
    memset(RPN_Data,0,sizeof(RPN_Data));
    unsigned char temp= 0;

    float fa = 0;
    float fb = 0;
    float fvalue = 0;
    for(unsigned char i = 0; i < Formula->m_u16DataCount; i++)
    {
        switch(Formula->m_AdvFormulaData[i].m_ucOperator)
        {
        case FormulaOperator_Number:
        {
            RPN_Data[temp++] = Formula->m_AdvFormulaData[i].m_fvaule;
            break;
        }

        case FormulaOperator_Add:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];
            RPN_Data[temp++] = fb + fa;
            break;
        }

        case FormulaOperator_Sub:	//-
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];
            RPN_Data[temp++] = fb - fa;
            break;
        }

        case FormulaOperator_Neg:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            RPN_Data[temp++] = -fa;

            break;
        }

        case FormulaOperator_Mul:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];
            RPN_Data[temp++] = fb * fa;
            break;
        }

        case FormulaOperator_Div:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];

            if(fa != 0)
                RPN_Data[temp++] = fb / fa;
            else
                return 0;

            break;
        }

        case FormulaOperator_Pow:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];
            RPN_Data[temp++] = pow(fb,fa);
            break;
        }

        case FormulaOperator_Mod:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];
            fb = RPN_Data[--temp];
            RPN_Data[temp++] = fmodf(fb,fa);
            break;
        }

        case FormulaOperator_Max:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fvalue = -FLT_MAX; //
            for(unsigned char j=0; j<Formula->m_AdvFormulaData[i].m_ucParamCount; j++)
            {
// 				temp_value[i] = RPN_Data[--temp];
                if(fvalue < RPN_Data[--temp])
                    fvalue = RPN_Data[temp];

            }
            RPN_Data[temp++] = fvalue;
            break;
        }

        case FormulaOperator_Min:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fvalue = FLT_MAX;
            for(unsigned char k=0; k<Formula->m_AdvFormulaData[i].m_ucParamCount; k++)
            {
// 				temp_value[i] = RPN_Data[--temp];
                if(fvalue > RPN_Data[--temp])
                    fvalue = RPN_Data[temp];
            }
            RPN_Data[temp++] = fvalue;
            break;
        }


        case FormulaOperator_Func:
        {
            if(temp < Formula->m_AdvFormulaData[i].m_ucParamCount)
                return 0;

            fa = RPN_Data[--temp];

            RPN_Data[temp++] = ChannelInfo_Manager::Function(Formula->m_AdvFormulaData[i].m_ucFuncID, fa);
            break;
        }

        case FormulaOperator_Variable:
        {
            RPN_Data[temp++] = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Formula->m_AdvFormulaData[i].m_ucDataType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,Formula->m_AdvFormulaData[i].m_fvaule);
            break;
        }

        default:
            return 0;
        }

    }

    if(temp == 1)
    {
        return RPN_Data[--temp];
    }
    return 0;

}



float ChannelInfo_Manager::Calculate_Formula(unsigned char uc_ChannelNo, St_FormulaData *Formula)
{
    //    unsigned char Aux_VirtualChn = 0,AuxType = 0;
    UN_Formula_fx  MyValue;
    float* pfX = &Formula->m_fX1;
    for(unsigned char uc_i = 0; uc_i < 8; uc_i++)
    {
        MyValue.f_Arry[uc_i] = Get_MetaVariable(uc_ChannelNo, Formula->m_ucDataType[uc_i],DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, pfX[uc_i]);
    }

    if(MyValue.fX.x4 == 0)
        MyValue.fX.x2 = 0;
    else
        MyValue.fX.x2 = MyValue.fX.x2 * MyValue.fX.x3 / MyValue.fX.x4;

    if(MyValue.fX.y4 == 0)
        MyValue.fX.y2 = 0;
    else
        MyValue.fX.y2 = MyValue.fX.y2 * MyValue.fX.y3 / MyValue.fX.y4;

    MyValue.fX.x2 = Function(Formula->m_ucFUNCTIONX, MyValue.fX.x2);
    MyValue.fX.x1 = MyValue.fX.x1 * MyValue.fX.x2;
    //MyValue.fX.x1 = Function(Formula->m_ucFUNCTION2,MyValue.fX.x1);

    MyValue.fX.y2 = Function(Formula->m_ucFUNCTIONY, MyValue.fX.y2);
    MyValue.fX.y1 = MyValue.fX.y1 * MyValue.fX.y2;

    MyValue.fX.x1 = Operation(Formula->m_ucOPERATOR, MyValue.fX.x1, MyValue.fX.y1);
    MyValue.fX.x1 = Function(Formula->m_ucFUNCTION2, MyValue.fX.x1);

    return   Function(Formula->m_ucFUNCTION1, MyValue.fX.x1);
}
float ChannelInfo_Manager::Calculate_CANConfig_Formula(unsigned char uc_ChannelNo, unsigned char CANConfigIdex,unsigned char Formula_Idex)
{
    if(CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaReflash[Formula_Idex] == true)
        return CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaValue[Formula_Idex];
    St_FormulaData *Formula = &CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaData[Formula_Idex];
    UN_Formula_fx  MyValue;
    float* pfX = &Formula->m_fX1;
    for(unsigned char uc_i = 0; uc_i < 8; uc_i++)
    {
        if(Formula->m_ucDataType[uc_i] == MP_DATA_TYPE_FORMULA)//公式嵌套
        {
            MyValue.f_Arry[uc_i] = Calculate_CANConfig_Formula(uc_ChannelNo, CANConfigIdex,(unsigned char)pfX[uc_i]);
        }
        else
        {
            MyValue.f_Arry[uc_i] = Get_MetaVariable(uc_ChannelNo, Formula->m_ucDataType[uc_i],DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, pfX[uc_i]);
        }

    }

    if(MyValue.fX.x4 == 0)
        MyValue.fX.x2 = 0;
    else
        MyValue.fX.x2 = MyValue.fX.x2 * MyValue.fX.x3 / MyValue.fX.x4;

    if(MyValue.fX.y4 == 0)
        MyValue.fX.y2 = 0;
    else
        MyValue.fX.y2 = MyValue.fX.y2 * MyValue.fX.y3 / MyValue.fX.y4;

    MyValue.fX.x2 = Function(Formula->m_ucFUNCTIONX, MyValue.fX.x2);
    MyValue.fX.x1 = MyValue.fX.x1 * MyValue.fX.x2;
    //MyValue.fX.x1 = Function(Formula->m_ucFUNCTION2,MyValue.fX.x1);

    MyValue.fX.y2 = Function(Formula->m_ucFUNCTIONY, MyValue.fX.y2);
    MyValue.fX.y1 = MyValue.fX.y1 * MyValue.fX.y2;

    MyValue.fX.x1 = Operation(Formula->m_ucOPERATOR, MyValue.fX.x1, MyValue.fX.y1);
    MyValue.fX.x1 = Function(Formula->m_ucFUNCTION2, MyValue.fX.x1);

    CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaValue[Formula_Idex] = Function(Formula->m_ucFUNCTION1, MyValue.fX.x1);
    CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaReflash[Formula_Idex] = true;
    return   CANBusParse::m_CANConfig.m_buf[CANConfigIdex].m_FormulaValue[Formula_Idex];
}
// bool  ChannelInfo_Manager::IfFormulaContain(St_FormulaData *Formula, unsigned short DataType)
// {
//     for(unsigned char uc_i = 0; uc_i < 8; uc_i++)
//     {
//         if(Formula->m_ucDataType[uc_i] == DataType)
//             return true;
//     }

//     return false;
// }

bool  ChannelInfo_Manager::IfFormulaContain(signed char schBufIdx, signed char formulaIdx,unsigned short DataType,unsigned char ControlValue_DataType)
{
    if((schBufIdx >= 0) && (formulaIdx >= 0))
    {
        if(ControlValue_DataType == MP_DATA_TYPE_FORMULA)
        {

            float* pf_X = &ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_fX1;
            for(unsigned char i = 0; i < 8; i++)
            {
                if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i] == MP_DATA_TYPE_FORMULA)
                {
                    if(DataType == MP_DATA_TYPE_FORMULA)
                        return true;
                    if(IfFormulaContain(schBufIdx,*pf_X,DataType,ControlValue_DataType))
                        return true;
                }
                else if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i] == DataType)
                {
                    return true;
                }
                pf_X++;
            }
            return false;
        }
        else
        {
            for(unsigned short i = 0; i < ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_u16DataCount; i++)
            {
                if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_AdvFormulaData[i].m_ucDataType == DataType)
                    return true;
            }
            return false;
        }
    }
    else
        return false;


}

//#if(SUPPORT_CAN_BMS ==1)


//can bms message read/set
/*float ChannelInfo_Manager::Read_CANBMSMetaVariable(unsigned char CAN_Chn,CANBMS_METACODE BMS_MetaCode) //char 170505 zyx
{
	if(BMS_MetaCode >= MAX_CAN_BMS_SIGNAL) //MetaCode_MV_COUNT 170512 zyx
		return 0.0f;
	return m_CANMetaVariable[CAN_Chn][BMS_MetaCode].m_fValue;
}*/
/*float ChannelInfo_Manager::Read_SMBBMSMetaVariable(unsigned char IV_Chn,unsigned char BMS_MetaCode)
{
	if(BMS_MetaCode >= MetaCode_MV_COUNT)
		return 0.0f;
	return ChannelInfo_Manager::m_SMBusMetaVariable[IV_Chn][BMS_MetaCode].m_Data.m_fValue[0];
}*/
void ChannelInfo_Manager::Set_CANBMSMetaVariable(unsigned char IV_Chn, CANBMS_METACODE MetaCode, float fValue)
{
    //unsigned char CAN_BMS_Chn  = m_CANMapInfo.m_CANmap[IV_Chn].m_CANBMSChnIdx;
    if(IV_Chn > MAXCHANNELNO) //目前将与MAXCHANNELNO等长
        return;
    m_CANMetaVariable[IV_Chn][MetaCode].m_fValue = fValue;
}

/*
//原则，先删除解除绑定的config（ m_CANConfig.m_buf要加入绑定Idx = -1表示无绑定 还要加入一个timestamp表示解绑时间
//然后没有的话，则add进config buffer
//通过m_CANConfig.m_buf[bufidx]的m_RegisterBit 知道该bufIdx被哪个IV通道注册过
bool ChannelInfo_Manager::Assign_CANConfig(unsigned char uc_ChannelNo)
{
//最先解绑对应的IV通道的buffer
int oldidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;  //全部初始化为-1
for(unsigned char idx = 0;idx<CANBusParse::m_CANConfig.m_bufCnt;idx++)//首先判断是否有此MD5的config
{
if(0 == memcmp((void*)&CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_MD5, (void*)&CANBusParse::m_CANConfig.m_buf[idx].m_MD5, 16))//find this config
{
//			  if(oldidx<0) //有，但无任何绑定
//				  break;
if(oldidx!=idx)  //需要更新绑定
{
if(oldidx>0)   //如果以前没有assign，就不存在跟旧的解绑
{
CANBusParse::m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);  //跟旧config解绑
CANBusParse::m_CANConfig.m_buf[oldidx].m_AbondonTime = OS_ServeProcess::OS_Time;
}
CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = idx;  //重新绑定
CANBusParse::m_CANConfig.m_buf[idx].m_RegisterBit |= 1<<uc_ChannelNo;   //绑定
}
else  //表明与之前绑定一致
{
CANBusParse::m_CANConfig.m_buf[oldidx].m_AbondonTime = OS_ServeProcess::OS_Time; //可以不要
}

int cfgidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
CANBusParse::m_CANConfig.m_buf[cfgidx].m_bBrdCstUpdate=1;
if(cfgidx >=0&&cfgidx<=MAX_LEN_CAN_CONFIG)
{
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bUpdated =1;   //updateflag == this msg is needed send 3 can msg
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_CANMsg.FrameType =CANBusParse::m_CANConfig.m_buf[cfgidx].m_FrameType;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_CANMsg.DLC = 8;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_CANMsg.Un_CANID.m_U32ID= CANBusParse::m_CANConfig.m_buf[cfgidx].m_BrdCstID;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bBroadcast = 1;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_Period = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BrdCst_interval;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_SignalStop = 0;
if(!CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_Period )  //if period = 0
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bValid = 0;
else
{
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bValid =1;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bUpdated = 1;//update send
}
CommonFunc::GetTimeTicksUpdate(&CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_CurrentTick);
}
//memset((void*)&m_CANMetaVariable[uc_ChannelNo][0],0,sizeof(ST_BMS_MetaValue)*MAX_CAN_BMS_SIGNAL);
CANBusParse::CANBMS_setBroadcast(uc_ChannelNo);
CANBusParse::CANBMS_SetBaudRate(uc_ChannelNo);
//CANBusParse::CANBMS_StopAllSend(uc_ChannelNo);
return true;
}
}
// 该MD5码的CANCONFIG 不存在
if(oldidx>=0)
{
CANBusParse::m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);  //跟旧config解绑
CANBusParse::m_CANConfig.m_buf[oldidx].m_AbondonTime = OS_ServeProcess::OS_Time;
}
if(CANBusParse::m_CANConfig.m_bufCnt<MAX_LEN_CAN_CONFIG) //无绑定
{
unsigned char myIdx= CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_insertPos = CANBusParse::m_CANConfig.m_bufCnt++;   // 表示下一次存入的位置是此位置
CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = myIdx;  //
memcpy(&CANBusParse:: m_CANConfig.m_buf[myIdx].m_MD5,&CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_MD5,sizeof(St_MD5)); //给对应config存入MD5
CANBusParse::m_CANConfig.m_buf[myIdx].m_RegisterBit |= 1<<uc_ChannelNo;  //绑定
return false;  //表示要求下载
}
else
{//不存在，且缓冲满了，则尝试删除buffer中的config
unsigned char oldestIdx = 0;
for(unsigned char idx = 1;idx<MAX_LEN_CAN_CONFIG;idx++)
{
if(CANBusParse:: m_CANConfig.m_buf[idx].m_RegisterBit ==0)  //先查找无绑定的(或已全部解绑的）因为 20>16,总有一个缓冲是空的
if(memcmp((void*)&CANBusParse::m_CANConfig.m_buf[idx].m_AbondonTime.Second,(void*)&CANBusParse::m_CANConfig.m_buf[oldestIdx].m_AbondonTime.Second,8)<0)
oldestIdx = idx;   //最小的os time
}
CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_insertPos = oldestIdx;
CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = oldestIdx;  //
memcpy(&CANBusParse:: m_CANConfig.m_buf[oldestIdx].m_MD5,&CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_MD5,sizeof(St_MD5)); //给对应config存入MD5
CANBusParse::m_CANConfig.m_buf[oldestIdx].m_RegisterBit |= 1<<uc_ChannelNo;  //绑定

return false;
}


}

*/
/*
bool ChannelInfo_Manager::Clear_CANConfig(unsigned char uc_ChannelNo)  //Add by DKQ 01.04.2015
{
int oldidx = CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx;
CANBusParse::m_CANConfig.m_buf[oldidx].m_RegisterBit &= ~(1<<uc_ChannelNo);  //跟旧config解绑
CANBusParse::m_CANConfig.m_buf[oldidx].m_AbondonTime = OS_ServeProcess::OS_Time;
CANBusParse::m_CANAssignMD5[uc_ChannelNo].m_cfgBufIdx = -1;

CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bValid = 0;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_Period = 0;
CANBusParse::m_BMS_BroadCstBuf[uc_ChannelNo].m_bUpdated = 0;

CANBusParse::CANBMS_ClearBMS(uc_ChannelNo);
CANBusParse::m_BMS_SendBuf[uc_ChannelNo].m_glbStopAll = 1;
return true;
}
*/
// typedef struct
// {   //190617,hpq
//     unsigned short     m_u32CANID;
//     unsigned short     m_u16OutboundSignalIndex;
//     float              m_fGain;
//     float              m_fOffset;
//     float              m_fMinValue;
//     float              m_fMaxValue;
//     unsigned char      m_u8DLC;
//     unsigned char      m_u8ByteOrder;
//     unsigned char      m_u8DataType;
//     unsigned char      m_u8RVD;
//     unsigned char      m_u8StartByte;
//     unsigned char      m_u8StartBit;
//     unsigned char      m_u8EndByte;
//     unsigned char      m_u8EndBit;
//     float              m_fRvd[2];          // ?????,??4????  // ????:16??
// } St_BMS_CFG_OutboundMessage;



//Convert original CAN BMS config(OrigCfg) from Ethernet packets to a new data struct( m_CANConfig.m_buf)
void ChannelInfo_Manager::Convert_CAN_MetaSignal(unsigned char ChnIdx, ST_CANBMS_CfgUnit *OrigCfg)
{
    unsigned char IVChnNo = m_CANMapInfo.m_CANmap[ChnIdx].m_IVChnNo;
    int insertIdx = CANBusParse::m_CANAssignMD5[IVChnNo].m_insertPos;
    if(insertIdx < 0)
        return;

    float fMinPeriod = BMS_BroadCst_Min_Period;
    unsigned char ValidCount = 0;

    if(ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount == 1)
        fMinPeriod = BMS_BroadCst_Min_Period / 10;
    for(unsigned char i = 0; i < BMS_BROAD_CST_COUNT; i++)
    {

        if(OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCst_interval != 0)
            ValidCount += 1;
    }
    if(ValidCount == 0)
    {
        fMinPeriod = fMinPeriod;
    }
    else
    {
        fMinPeriod *= ValidCount;
    }


    St_CANBMS_Config * p_CAN_Cfg = &CANBusParse::m_CANConfig.m_buf[insertIdx];
    for(unsigned char i = 0; i < BMS_BROAD_CST_COUNT; i++)
    {
        p_CAN_Cfg->m_BroadCastInfo[i].m_Info = OrigCfg->m_GlobalCfg.m_BrocastInfo[i];
        p_CAN_Cfg->m_BroadCastInfo[i].m_bBrdCstUpdate = 1;
// 		p_CAN_Cfg->m_BroadCastInfo[i].m_Endian = OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_Endian;
// 		p_CAN_Cfg->m_BroadCastInfo[i].m_FrameType = OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_FrameType;
// 		p_CAN_Cfg->m_BroadCastInfo[i].m_BrdCst_ID =OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCst_ID;

        if(OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCst_interval > 0 && OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCst_interval < fMinPeriod)
            p_CAN_Cfg->m_BroadCastInfo[i].m_Info.m_BrdCst_interval = fMinPeriod;
        else
            p_CAN_Cfg->m_BroadCastInfo[i].m_Info.m_BrdCst_interval = OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCst_interval;
// 		for(unsigned char j = 0;j<2;j++)
// 		{
// 		  p_CAN_Cfg->m_BroadCastInfo[i].m_Data_Mode[j] = OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_DataMode[j];
// 		  p_CAN_Cfg->m_BroadCastInfo[i].m_BrdCstData[j] = OrigCfg->m_GlobalCfg.m_BrocastInfo[i].m_BrdCstData[j];
// 		}

        int cfgidx = CANBusParse::m_CANAssignMD5[IVChnNo].m_cfgBufIdx;
        if(cfgidx >= 0 && cfgidx <= MAX_LEN_CAN_CONFIG)
        {
            St_BMS_SendUnit * p_Buff = &CANBusParse::m_BMS_BroadCstBuf[IVChnNo][i];
            p_Buff->m_bUpdated = 1;
            p_Buff->m_CANMsg.FrameType = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_FrameType;
            p_Buff->m_CANMsg.DLC = 8;
            p_Buff->m_CANMsg.Un_CANID.m_U32ID = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCst_ID;
            p_Buff->m_bBroadcast = 1;
            p_Buff->m_Period = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCst_interval;
//         for(unsigned char j = 0;j<2;j++)
//         {
//             p_Buff->m_Data_Mode[j] = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_DataMode[j];
//             p_Buff->m_BrdCstData[j] = CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCstData[j];
//         }
            memcpy((void *)&p_Buff->m_Data_Mode[0], (void *)&CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_DataMode[0], 2);
            memcpy((void *)&p_Buff->m_BrdCstData[0], (void *)&CANBusParse::m_CANConfig.m_buf[cfgidx].m_BroadCastInfo[i].m_Info.m_BrdCstData[0], 2 * sizeof(float));

            CommonFunc::GetTimeTicksUpdate(&p_Buff->m_CurrentTick);
            if(!p_Buff->m_Period)  //Period ==0
                p_Buff->m_bValid = 0;
            else
            {
                p_Buff->m_bValid = 1;// broadcast valid
                p_Buff->m_bUpdated = 1;//update send
            }
            p_Buff->m_SignalStop = 0;
        }
    }
    for(unsigned char i = 0; i < 16; i++)	//每个CANconfig包含16条公式
    {
        p_CAN_Cfg->m_FormulaData[i] = OrigCfg->m_GlobalCfg.m_FormulaData[i];
        p_CAN_Cfg->m_FormulaReflash[i] = false;//初始化公式状态
        p_CAN_Cfg->m_FormulaValue[i] = 0;//初始化公式值
    }
    p_CAN_Cfg->m_bEnable = 1;  //enable this metatab
    p_CAN_Cfg->m_IsFDCAN = OrigCfg->m_GlobalCfg.m_IsCANFD[0];
    p_CAN_Cfg->m_BaudRate = OrigCfg->m_GlobalCfg.m_BaudRate[0]; //181225 zyx
    p_CAN_Cfg->m_BitRate = OrigCfg->m_GlobalCfg.m_BitRate[0]; //181225 zyx
    p_CAN_Cfg->m_UnsafeTimeout = OrigCfg->m_GlobalCfg.m_UnsafeTimeout[0] * 10; //181225 zyx
    p_CAN_Cfg->m_AuxCANID = m_CANMapInfo.m_CANmap[ChnIdx].m_AuxUnitID;
    CANBusParse::m_BroadCstIdex[IVChnNo] = 0;

    memset(p_CAN_Cfg->m_CAN_OutboundSignals, 0, sizeof(p_CAN_Cfg->m_CAN_OutboundSignals));
    memset(p_CAN_Cfg->m_CAN_OutboundMsgInfos, 0, sizeof(p_CAN_Cfg->m_CAN_OutboundMsgInfos));
    p_CAN_Cfg->m_CAN_OutboundSignalCount = OrigCfg->m_GlobalCfg.m_BMS_OutboundSignal_Count;
    for(int sigidx = 0; sigidx < OrigCfg->m_GlobalCfg.m_BMS_OutboundSignal_Count; sigidx++)
        p_CAN_Cfg->m_CAN_OutboundSignals[sigidx] = OrigCfg->m_CAN_OutboundSignals[sigidx];
    p_CAN_Cfg->m_CAN_OutboundMsgCount = 0;
    for(int msgidx = 0; msgidx < OrigCfg->m_GlobalCfg.m_BMS_OutboundMessage_Count; msgidx++)
    {
        int msginfoidx = 0;
        for(msginfoidx = 0; msginfoidx < p_CAN_Cfg->m_CAN_OutboundMsgCount; msginfoidx++)
        {
            if(p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_CANID == OrigCfg->m_CAN_OutboundMessages[msgidx].m_u32CANID)
            {   //add to existing msginfo
                p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MsgMember[p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MemberCount]
                    = OrigCfg->m_CAN_OutboundMessages[msgidx];
                CANBusParse::CANOutboundSignalLinearTransformation(p_CAN_Cfg,msginfoidx);
                p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MemberCount++;
                break;
            }
        }
        if(msginfoidx >= p_CAN_Cfg->m_CAN_OutboundMsgCount)
        {   //new added
            msginfoidx = p_CAN_Cfg->m_CAN_OutboundMsgCount;
            p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_CANID = OrigCfg->m_CAN_OutboundMessages[msgidx].m_u32CANID;
            p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MsgMember[p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MemberCount]
                = OrigCfg->m_CAN_OutboundMessages[msgidx];
            CANBusParse::CANOutboundSignalLinearTransformation(p_CAN_Cfg,msginfoidx);
            p_CAN_Cfg->m_CAN_OutboundMsgInfos[msginfoidx].m_MemberCount++;
            p_CAN_Cfg->m_CAN_OutboundMsgCount++;
        }
    }
    //sort ascending!!!//
    for(CANBMS_METAIdx i = 0 ; i < OrigCfg->m_GlobalCfg.m_ItemCount.TotalItems; i++) //170505 zyx
    {
        St_CANBMS_ParsedMeta * p_Signal = &p_CAN_Cfg->m_ValidSignal[i];
        ST_CAN_BMS_SIGNAL * p_BMS_Signal = &OrigCfg->m_CAN_Signals[i];

// 		p_Signal->m_ByteOrder = p_BMS_Signal->m_ByteOrder;
// 		p_Signal->m_StartDataIndex = p_BMS_Signal->m_StartDataIndex;
// 		p_Signal->m_EndDataIndex = p_BMS_Signal->m_EndDataIndex;

        unsigned int * uc_Byte_S = &p_BMS_Signal->m_StartDataIndex;
        unsigned int * uc_Byte_D = &p_Signal->m_StartDataIndex;
        for(unsigned char uc_i = 0; uc_i < 3; uc_i++)
            * uc_Byte_D++ = * uc_Byte_S++;

//        p_Signal->m_lowValidBit = CAN_ORGCODE >> (63 - (CANBusParse:: m_CANConfig.m_buf[insertIdx].m_ValidSignal[i].m_EndDataIndex % 64) );

        p_Signal->m_CANMessageID = p_BMS_Signal->m_CANMessageID;
        CANBMS_METACODE MetaIdx = p_BMS_Signal->m_CAN_RX_Meta; //170505 zyx
        p_Signal->m_CAN_RX_Meta = (CANBMS_METACODE)MetaIdx;
        p_Signal->m_DataLog = p_BMS_Signal->m_DataLog;
        p_Signal->m_DataType = p_BMS_Signal->m_DataType;
        p_Signal->m_DLC = p_BMS_Signal->m_DLC;
// 		p_Signal->m_ScaleFactor = p_BMS_Signal->m_ScaleFactor;
// 		p_Signal->m_Offset = p_BMS_Signal->m_Offset;
// 		p_Signal->m_MaxValue = p_BMS_Signal->m_MaxValue;
// 		p_Signal->m_MinValue = p_BMS_Signal->m_MinValue;

        float * f_S = &p_BMS_Signal->m_MinValue;
        float * f_D = &p_Signal->m_MinValue;
        for(unsigned char uc_i = 0; uc_i < 4; uc_i++)
            * f_D++ = * f_S++;
    }
    p_CAN_Cfg->m_MetaCount = OrigCfg->m_GlobalCfg.m_ItemCount;
    ChannelInfo_Manager::m_CANMapInfo.m_CANmap[IVChnNo].m_bIVActive = true; //180614 zyx
    CANBusParse::CheckCanBmsMap();
    // memset((void*)&m_CANMetaVariable[IVChnNo][0],0,sizeof(ST_BMS_MetaValue)*MAX_CAN_BMS_SIGNAL);
    CANBMS_SetMetaLog(IVChnNo);
    //CANBusParse::CANBMS_QuickSortCfg(OrigCfg->m_GlobalCfg.m_ItemCount, p_CAN_Cfg->m_ValidSignal);//移交上位机排序
    CANBusParse::CANBMS_SetBaudRate(IVChnNo);
    //CANBusParse::CANBMS_StopAllSend(IVChnNo);
}

void ChannelInfo_Manager::CANBMS_SetMetaLog(unsigned char IV_ChnIdx)
{
    int cfgidx = CANBusParse::m_CANAssignMD5[IV_ChnIdx].m_cfgBufIdx;
    CANBMS_METACOUNT MetaCount = CANBusParse::m_CANConfig.m_buf[cfgidx].m_MetaCount.TotalItems;
    St_CANBMS_ParsedMeta *pMetaSignals = &CANBusParse::m_CANConfig.m_buf[cfgidx].m_ValidSignal[0];

    for(CANBMS_METAIdx idx = 0; idx < MetaCount; idx++) //char 170505 zyx
    {
        m_CANMetaVariable[IV_ChnIdx][pMetaSignals[idx].m_CAN_RX_Meta].m_bNeedLog = pMetaSignals[idx].m_DataLog;
    }
}

//Make use of Sch's CtrlValue to send CAN BMS Messages
void ChannelInfo_Manager::SendCANMessage(unsigned char uc_ChannelNo)
{
    MP_SET_CANMSG myMsg;

    unsigned char ActiveIdx = ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_CAN_SendMsg * p_CANValue = &ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_CANValue;

    myMsg.m_IVChnNo = uc_ChannelNo; //Add by DKQ 12.28.2015
// 	myMsg.m_CANMsg.m_AllStop	= p_CANValue->m_AllStop;
// 	myMsg.m_CANMsg.m_SignalStop	= p_CANValue->m_SignalStop;
// 	myMsg.m_CANMsg.m_FrameType	= p_CANValue->m_FrameType;
    myMsg.m_CANMsg = *p_CANValue;
    myMsg.m_CANMsg.m_bOutboundMsg = 0;   //190617,hpq
// 	myMsg.m_CANMsg.m_Length		= p_CANValue->m_Length;
    CANBusParse::SetCanBmsData(&myMsg);
}
void ChannelInfo_Manager::SendCANMessage_Outbound(unsigned char uc_ChannelNo)
{   //190617,hpq
    MP_SET_CANMSG myMsg;
// 	myMsg.m_CANMsg.m_Period		= p_CANValue->m_Period;
    unsigned char ActiveIdx = ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_CAN_SendMsg * p_CANValue = &ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_CANValue;
// 	memcpy(myMsg.m_CANMsg.m_Data,p_CANValue->m_Data,8);
    myMsg.m_IVChnNo = uc_ChannelNo; //Add by DKQ 12.28.2015
    myMsg.m_CANMsg = *p_CANValue;
    myMsg.m_CANMsg.m_bOutboundMsg = 1;   //190617,hpq

    CANBusParse::SetCanBmsData(&myMsg);

}
//#endif

/////////////////SmartBattery
void ChannelInfo_Manager::Convert_SMB_MetaSignal(unsigned char ChnIdx, ST_SMBBMS_CfgUnit *OrigCfg)
{
    unsigned char IVChnNo = 0;	 //暂时不支持map
    int insertIdx = Smart_Battery::m_SMBAssignMD5[IVChnNo].m_insertPos;
    if(insertIdx < 0)
        return;


    St_SMBBMS_Config *p_SMB_Cfg = &Smart_Battery::m_SMBCfgBuf.m_buf[insertIdx];

    int cfgidx = Smart_Battery::m_SMBAssignMD5[IVChnNo].m_cfgBufIdx;
    if(cfgidx >= 0 && cfgidx <= MAX_LEN_SMB_CONFIG)
    {

        p_SMB_Cfg->m_bEnable = 1;  //enable this metatab
        p_SMB_Cfg->m_BaudRate = OrigCfg->m_GlobalCfg.m_BaudRate;

        //sort ascending!!!//
        for(unsigned char i = 0 ; i < OrigCfg->m_GlobalCfg.m_ItemCount; i++) //SMB 信号直接使用应该就可以了
        {
// 		St_SMBBMS_ParsedMeta * p_SMB_Signal = &p_SMB_Cfg->m_ValidSignal[i];
            p_SMB_Cfg->m_MetaCount = OrigCfg->m_GlobalCfg.m_ItemCount;
            p_SMB_Cfg->m_UnsafeTimeout = OrigCfg->m_GlobalCfg.m_UnsafeTimeout * 10;	 //ADDED BY ZC 12012017
        }
        SMB_SetMetaLog(IVChnNo);


    }
}
void ChannelInfo_Manager::SMB_SetMetaLog(unsigned char IV_ChnIdx)
{
    int cfgidx = Smart_Battery::m_SMBAssignMD5[IV_ChnIdx].m_cfgBufIdx;
    unsigned short MetaCount = Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_MetaCount;
    St_SMBBMS_ParsedMeta	*pMetaSignals = &Smart_Battery::m_SMBCfgBuf.m_buf[cfgidx].m_ValidSignal[0];
    for(unsigned char idx = 0; idx < MetaCount; idx++)
    {
        m_SMBusMetaVariable[IV_ChnIdx][pMetaSignals[idx].m_SMB_RX_Meta].m_bNeedLog = pMetaSignals[idx].m_Datalog;
    }
}

float ChannelInfo_Manager::CoefficientCalculate(unsigned char Operator,float fCoefficient,float fValue)
{
// 	if(Operator == 1)  // *
    return (fCoefficient*fValue);
}
float ChannelInfo_Manager::Get_MetaVariable(unsigned char uc_ChannelNo, unsigned char uc_DataType, unsigned char uc_Operator,float fCoefficient,float fMetaCode)
{
    unsigned char uc_Operate = 1;  // 20190926 yy
    float Value = 0;
//#if SUPPORT_AUX == 1
// 	ST_AUX_DATA * p_Temp_Data	=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_TEMPERATURE][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_Vol_Data	=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_VOLTAGE][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_DI_Data		=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_DI][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_DO_Data		=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_DO][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_FR_Data		=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_FLOW_RATE][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_Hum_Data	=	&Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_HUMIDITY][(unsigned short)fMetaCode];
// 	ST_AUX_DATA * p_EC_Data   = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_EXTERNALCHARGE][(unsigned short)fMetaCode];

    ST_AUX_DATA * p_Data;
//#endif
    switch(uc_DataType)
    {
    case MP_DATA_TYPE_FIGURE:
        Value = fMetaCode;
        uc_Operate = 0;   // 20190926 yy
        break;
    case MP_DATA_TYPE_METAVALUE:
        Value = Read_SpecifiedMetavariable2(uc_ChannelNo, fMetaCode);
        if((fMetaCode >= MetaCode_MV_UD1 && fMetaCode <= MetaCode_MV_UD4)
                ||(fMetaCode >= MetaCode_MV_UD5 && fMetaCode <= MetaCode_MV_UD16))
        {
            ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo] = true;
        }
        break;
    case MP_DATA_TYPE_CANBMS:
// 		Value = Read_CANBMSMetaVariable(uc_ChannelNo,fMetaCode);
        if(fMetaCode >= MAX_CAN_BMS_SIGNAL) //MetaCode_MV_COUNT 170512 zyx
            return 0.0f;
        Value = m_CANMetaVariable[uc_ChannelNo][(unsigned short)fMetaCode].m_fValue;
        break;
    case MP_DATA_TYPE_SMBBMS:
// 		Value= Read_SMBBMSMetaVariable(uc_ChannelNo,fMetaCode);
        if(ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][(unsigned char)fMetaCode].m_type == TYPE_STRING)
            Value = ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][(unsigned char)fMetaCode].m_Data.m_SValue[0];
        else
            Value = ChannelInfo_Manager::m_SMBusMetaVariable[uc_ChannelNo][(unsigned char)fMetaCode].m_Data.m_fValue[0];
        break;
//#if SUPPORT_AUX == 1
    case MP_DATA_TYPE_AUX_T:
        if((BuiltIn_DataManager::b_BuiltInAux)  && (BuiltIn_DataManager::m_BuiltInAuxInfo.TempExistFlag))
        {
            unsigned short chn = (unsigned short)fMetaCode;
            if(chn < BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV)
                Value = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_BuiltIn_Data1 + chn].m_fValue;
            else
            {
//              chn-=BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV;
// 						unsigned char Aux_VirturalIdx = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[AUX_TEMPERATURE][chn].m_Map.m_Index_InAuxUnit;
                p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_TEMPERATURE][chn];
                if(p_Data->updated_aux_data.m_Char == 0)
                    AuxLimitCheckOk = 0;
                Value = p_Data ->aux_data;
            }
        }
        else
        {
            p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_TEMPERATURE][(unsigned short)fMetaCode];
            if(p_Data->updated_aux_data.m_Char == 0)
                AuxLimitCheckOk = 0;
            Value = p_Data ->aux_data;
        }
        break;
    case MP_DATA_TYPE_AUX_dTdt:
// 		if(p_Temp_Data->updated_aux_dxdt.m_Char == 0)
// 			AuxLimitCheckOk = 0;
// 		Value = p_Temp_Data->aux_dx_dt;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_TEMPERATURE][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_dxdt.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_dx_dt;
        break;
    case MP_DATA_TYPE_AUX_V:
        if((BuiltIn_DataManager::b_BuiltInAux)  && (BuiltIn_DataManager::m_BuiltInAuxInfo.VolExistFlag))
        {
            unsigned short chn = (unsigned short)fMetaCode;
            if(chn < BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV)
                Value = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_BuiltIn_Data1 + chn].m_fValue;
            else
            {
//              chn-=BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV;
// 						unsigned char VirturalIdx = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[AUX_VOLTAGE][chn].m_Map.m_Index_InAuxUnit;
                p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_VOLTAGE][chn];
                if(p_Data->updated_aux_data.m_Char == 0)
                    AuxLimitCheckOk = 0;
                Value = p_Data ->aux_data;
            }
        }
        else
        {
            p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_VOLTAGE][(unsigned short)fMetaCode];
            if(p_Data->updated_aux_data.m_Char == 0)
                AuxLimitCheckOk = 0;
            Value = p_Data ->aux_data;
        }
        break;
    case MP_DATA_TYPE_AUX_dVdt:
// 		if(p_Vol_Data->updated_aux_dxdt.m_Char == 0)
// 			AuxLimitCheckOk = 0;
// 		Value = p_Vol_Data->aux_dx_dt;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_VOLTAGE][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_dxdt.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_dx_dt;
        break;
    case MP_DATA_TYPE_AUX_DI:
// 		if(p_DI_Data->updated_aux_data.m_Char == 0)
// 			ChannelInfo_Manager::AuxLimitCheckOk = 0;
// 		Value = p_DI_Data->aux_data;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_DI][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_data.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_data;
        break;
    case MP_DATA_TYPE_AUX_DO:
// 		if(p_DO_Data->updated_aux_data.m_Char == 0)
// 			ChannelInfo_Manager::AuxLimitCheckOk = 0;
// 		Value = p_DO_Data->aux_data;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_DO][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_data.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_data;
        break;
    case MP_DATA_TYPE_AUX_FlowRate:
// 		if(p_FR_Data->updated_aux_data.m_Char == 0)    // 用于温控器湿度   20150609 yang
// 			AuxLimitCheckOk = 0;
// 		Value = p_FR_Data->aux_data;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_FLOW_RATE][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_data.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_data;
        break;
    case MP_DATA_TYPE_AUX_Humidity:
// 		if(p_Hum_Data->updated_aux_data.m_Char == 0)    //温控箱湿度数据 zyx
// 			AuxLimitCheckOk = 0;
// 		Value = p_Hum_Data->aux_data;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_HUMIDITY][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_data.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_data;
        break;
    case  MP_DATA_TYPE_EC:
// 		if(p_EC_Data->updated_aux_data.m_Char == 0)    //温控箱湿度数据 zyx
// 			AuxLimitCheckOk = 0;
// 		Value = p_EC_Data->aux_data;
        p_Data = &Channel_MapAuxInfo::m_AuxData[uc_ChannelNo][AUX_EXTERNALCHARGE][(unsigned short)fMetaCode];
        if(p_Data->updated_aux_data.m_Char == 0)
            AuxLimitCheckOk = 0;
        Value = p_Data ->aux_data;
        break;
//#endif

    case MP_DATA_TYPE_FORMULA:
        if((ChannelSchedule[uc_ChannelNo].m_scBufIdx >= 0) && (ChannelSchedule[uc_ChannelNo].m_scBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER))
        {
            if(m_FormulaValue[uc_ChannelNo][(unsigned char)fMetaCode].Reflash == true)
                Value = m_FormulaValue[uc_ChannelNo][(unsigned char)fMetaCode].Value;
            else
            {
                Value = Calculate_Formula(uc_ChannelNo, \
                                          &ScheduleBuf[ChannelSchedule[uc_ChannelNo].m_scBufIdx].m_FormulaData[(unsigned char)fMetaCode]);
                m_FormulaValue[uc_ChannelNo][(unsigned char)fMetaCode].Value = Value;
                m_FormulaValue[uc_ChannelNo][(unsigned char)fMetaCode].Reflash = true;
            }
        }
        else
            Value = 0;
        uc_Operate = 0;    // 20190926 yy
        break;

    case MP_DATA_TYPE_ADVFORMULA:
        if((ChannelSchedule[uc_ChannelNo].m_scBufIdx >= 0) && (ChannelSchedule[uc_ChannelNo].m_scBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER))
        {
            Value = Calculate_ADVFormula(uc_ChannelNo, \
                                         &ScheduleBuf[ChannelSchedule[uc_ChannelNo].m_scBufIdx].m_AdvFormula[(unsigned char)fMetaCode]);
        }
        else
            Value = 0;
        uc_Operate = 0;    // 20190926 yy

        break;

    default:
        break;
    }
    if(uc_Operate)
        Value = CoefficientCalculate(uc_Operator,fCoefficient,Value);  // 20190926 yy
    return Value;
}
void ChannelInfo_Manager::Set_TimeRecalculateSign(void)
{
    for(int uc_ChannelNo = 0; uc_ChannelNo < BoardCFG.m_Global.m_u16IVChannelCount; uc_ChannelNo++)
    {
        st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
        p_MV2[MetaCode_PV_Steptime].m_bNeedRecal = 1;
        p_MV2[MetaCode_PV_TestTime].m_bNeedRecal = 1;
        p_MV2[MetaCode_TC_Timer1].m_bNeedRecal = 1;
        p_MV2[MetaCode_TC_Timer2].m_bNeedRecal = 1;
        p_MV2[MetaCode_TC_Timer3].m_bNeedRecal = 1;
        p_MV2[MetaCode_TC_Timer4].m_bNeedRecal = 1;
//         memset((void*)&p_MV2[MetaCode_PV_Steptime].m_bNeedRecal,1,2*sizeof(unsigned char));
//         memset((void*)&p_MV2[MetaCode_TC_Timer1].m_bNeedRecal,1,4*sizeof(unsigned char));
        p_MV2[MetaCode_DV_Time].m_bNeedRecal = 1;
    }
}
void ChannelInfo_Manager::Set_CurrentRecalculateSign(unsigned char uc_ChannelNo)
{
    Isamp_NeedRecalculateSign[uc_ChannelNo].m_Char = 0xFF;
    MetaVariable2[uc_ChannelNo][MetaCode_DV_Current].m_bNeedRecal = 1;
}
void ChannelInfo_Manager::Set_VoltageRecalculateSign(unsigned char uc_ChannelNo)
{
    Vsamp_NeedRecalculateSign[uc_ChannelNo].m_Char = 0xFF;
    MetaVariable2[uc_ChannelNo][MetaCode_DV_Voltage].m_bNeedRecal = 1;
}
void ChannelInfo_Manager::Set_CCapacityRecalculateSign(unsigned char uc_ChannelNo)
{
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeCapacity1].m_bNeedRecal = 1;
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeCapacity2].m_bNeedRecal = 1;
    memset((void*)&MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeCapacity1].m_bNeedRecal, 1, 2 * sizeof(unsigned char));
}
void ChannelInfo_Manager::Set_DCapacityRecalculateSign(unsigned char uc_ChannelNo)
{
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeCapacity1].m_bNeedRecal = 1;
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeCapacity2].m_bNeedRecal = 1;
    memset((void*)&MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeCapacity1].m_bNeedRecal, 1, 2 * sizeof(unsigned char));
}
void ChannelInfo_Manager::Set_CEnergyRecalculateSign(unsigned char uc_ChannelNo)
{
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeEnergy1].m_bNeedRecal = 1;
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeEnergy2].m_bNeedRecal = 1;
    memset((void*)&MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeEnergy1].m_bNeedRecal, 1, 2 * sizeof(unsigned char));
}
void ChannelInfo_Manager::Set_DEnergyRecalculateSign(unsigned char uc_ChannelNo)
{
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeEnergy1].m_bNeedRecal = 1;
// 	MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeEnergy2].m_bNeedRecal = 1;
    memset((void*)&MetaVariable2[uc_ChannelNo][MetaCode_TC_DischargeEnergy1].m_bNeedRecal, 1, 2 * sizeof(unsigned char));
}
unsigned long ChannelInfo_Manager::Read_CurrentAfterFilter(unsigned char uc_ChannelNo)
{
    unsigned long CodeAfterFilter = 0;
    int ItemCount = CQ::GetItemNumber(HistoryData[uc_ChannelNo].m_uCurrHnd);

    if(ItemCount <= 0)
        return 0;                       //use 0 to sign get value failed;

    if(ItemCount == POWER_OF_7_128)
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_u32CurrentSum >> 7;//电流滤波缓冲区一般都处于满的状态，这个最容易遇到，放在第一个CASE，节省时间。
    else
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_u32CurrentSum / ItemCount;
    return CodeAfterFilter;
}

unsigned long ChannelInfo_Manager::Read_VoltageAfterFilter(unsigned char uc_ChannelNo)
{
    unsigned long CodeAfterFilter = 0;
    int ItemCount = CQ::GetItemNumber(HistoryData[uc_ChannelNo].m_uVoltHnd);

    if(ItemCount <= 0)   //缓冲区为空，无法取得数据
        return 0;                       //use 0 to sign get value failed;

    if(ItemCount == POWER_OF_4_16)
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_u32VoltageSum >> 4;
    else
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_u32VoltageSum / ItemCount;

    return CodeAfterFilter;
}
unsigned long ChannelInfo_Manager::Read_BuildInVoltAfterFilter(unsigned char uc_ChannelNo, unsigned char BuildInIdx)
{
    unsigned long CodeAfterFilter = 0;
    int ItemCount = CQ::GetItemNumber(HistoryData[uc_ChannelNo].m_BuildInVolt[BuildInIdx].m_uHandler);

    if(ItemCount <= 0)   //缓冲区为空，无法取得数据
        return 0;                       //use 0 to sign get value failed;

    if(ItemCount == POWER_OF_4_16)
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_BuildInVolt[BuildInIdx].m_u32Sum >> 4;
    else
        CodeAfterFilter = HistoryData[uc_ChannelNo].m_BuildInVolt[BuildInIdx].m_u32Sum / ItemCount;

    return CodeAfterFilter;
}

void ChannelInfo_Manager::Write_SpecifiedMetavariable2(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, float fValue)
{
    if(uc_MetaCode >= MetaCode_MV_COUNT)
        return;

    Write_SpecifiedMetavariable3(uc_ChannelNo, uc_MetaCode, fValue); //这句代码，大部分情况下，不会被执行。

    MetaVariable2[uc_ChannelNo][uc_MetaCode].m_fValue = fValue;
}
void ChannelInfo_Manager::Write_SpecifiedMetavariable3(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, float fValue)
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    St_TC_Info * p_TC_Info = &MetaVariables[uc_ChannelNo].m_TC_Info;
    switch(uc_MetaCode)
    {
    case MetaCode_PV_Steptime:
        ChannelStep[uc_ChannelNo].m_Info[ChannelStep[uc_ChannelNo].m_ucActiveIdx].m_StartTime = CalculatePreviousMsTime(fValue);
        return;
    case MetaCode_PV_TestTime:
        ChannelSchedule[uc_ChannelNo].m_StartTime = CalculatePreviousMsTime(fValue);
        return;
    case MetaCode_PV_ChargeCapacity:
    // p_MV2[MetaCode_PV_ChargeCapacity].m_Value.m_double = fValue;
    // break;
    case MetaCode_PV_DischargeCapacity:
    // p_MV2[MetaCode_PV_DischargeCapacity].m_Value.m_double = fValue;
    // break;
    case MetaCode_PV_ChargeEnergy:
    //p_MV2[MetaCode_PV_ChargeEnergy].m_Value.m_double = fValue;
    //break;
    case MetaCode_PV_DischargeEnergy:
        p_MV2[uc_MetaCode].m_Value.m_double = fValue;
        break;
    case MetaCode_TC_Timer1:
// 		p_TC_Info->m_Timer1.m_StartTime = CalculatePreviousMsTime(fValue);
// 		return;
    case MetaCode_TC_Timer2:
// 		p_TC_Info->m_Timer2.m_StartTime = CalculatePreviousMsTime(fValue);
// 		return;
    case MetaCode_TC_Timer3:
// 		p_TC_Info->m_Timer3.m_StartTime = CalculatePreviousMsTime(fValue);
// 		return;
    case MetaCode_TC_Timer4:
// 		p_TC_Info->m_Timer4.m_StartTime = CalculatePreviousMsTime(fValue);
// 		return;
        St_TimerRegister * TimeR = &p_TC_Info->m_Timer1;
        TimeR += (uc_MetaCode % MetaCode_TC_Timer1);
        TimeR->m_StartTime = CalculatePreviousMsTime(fValue);
        return;
    case MetaCode_TC_ChargeCapacity1:
// 		p_TC_Info->m_ChargeCapacity1.m_StartCapacity = fValue - p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		p_MV2[MetaCode_TC_ChargeCapacity1].m_fValue = fValue;
// 		return;
    case MetaCode_TC_ChargeCapacity2:
// 		p_TC_Info->m_ChargeCapacity2.m_StartCapacity = fValue - p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		p_MV2[MetaCode_TC_ChargeCapacity2].m_fValue = fValue;
// 		return;
    case MetaCode_TC_DischargeCapacity1:
// 		p_TC_Info->m_DischargeCapacity1.m_StartCapacity = fValue - p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		p_MV2[MetaCode_TC_DischargeCapacity1].m_fValue = fValue;
// 		return;
    case MetaCode_TC_DischargeCapacity2:
// 		p_TC_Info->m_DischargeCapacity2.m_StartCapacity = fValue - p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		p_MV2[MetaCode_TC_DischargeCapacity2].m_fValue = fValue;
// 		return;
    case MetaCode_TC_ChargeEnergy1:
// 		p_TC_Info->m_ChargeEnergy1.m_StartCapacity = fValue - p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		p_MV2[MetaCode_TC_ChargeEnergy1].m_fValue = fValue;
// 		return;
    case MetaCode_TC_ChargeEnergy2:
// 		p_TC_Info->m_ChargeEnergy2.m_StartCapacity = fValue - p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		p_MV2[MetaCode_TC_ChargeEnergy2].m_fValue = fValue;
// 		return;
    case MetaCode_TC_DischargeEnergy1:
// 		p_TC_Info->m_DischargeEnergy1.m_StartCapacity = fValue - p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		p_MV2[MetaCode_TC_DischargeEnergy1].m_fValue = fValue;
// 		return;
    case MetaCode_TC_DischargeEnergy2:
// 		p_TC_Info->m_DischargeEnergy2.m_StartCapacity = fValue - p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		p_MV2[MetaCode_TC_DischargeEnergy2].m_fValue = fValue;

        St_CapacityRegister * CapReg = &p_TC_Info->m_ChargeCapacity1;
        unsigned char uc_MetaOffset = uc_MetaCode % MetaCode_TC_ChargeCapacity1;
        CapReg += uc_MetaOffset;
        CapReg->m_StartCapacity = fValue - p_MV2[MetaCode_PV_ChargeCapacity + uc_MetaOffset / 2].m_fValue;
        p_MV2[uc_MetaCode].m_fValue = fValue;
        return;
    //161019 zyx
    case MetaCode_PV_CV_StageTime:
        p_MV2[MetaCode_PV_CV_StageTime].m_Value.m_Time = CalculatePreviousMsTime(fValue);
        return;
    //161019 zyx

    case MetaCode_PV_ChargeCapacityTime: //zyx 171010
    {
        p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time2us.Second = fValue;
        p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time2us.Us =  (fValue - p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Second) * 1.0e6f ;
        return;
    }

    case MetaCode_PV_DischargeCapacityTime:
    {
        p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time2us.Second = fValue;
        p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time2us.Us = (fValue - p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Second) * 1.0e6f ;
        return;
    }

    default:
        return;
    }

    return;
}

//#if SUPPORT_PARALLEL == 1
void ChannelInfo_Manager::CopyMasterSpecifiedMetavariable(unsigned short uSourceChan, unsigned short uFromChan, unsigned short uToChan, unsigned char uc_MetaCode)
{
// 	if(uc_MetaCode >= MetaCode_MV_COUNT)
// 		return;

    //	if (!IsIntraUnitMasterChannel(uc_ChannelNo))
    //		return;

    unsigned short uChanIdx;

    for ( uChanIdx = uFromChan; uChanIdx <= uToChan; uChanIdx ++)
    {
        MetaVariable2[uChanIdx][uc_MetaCode].m_fValue = MetaVariable2[uSourceChan][uc_MetaCode].m_fValue;
    }
}
//#endif


/*void ChannelInfo_Manager::SetTestTime(unsigned short uc_ChannelNo, double dSetValue)
{
	Write_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_TestTime,dSetValue);
}
void ChannelInfo_Manager::SetStepTime(unsigned short uc_ChannelNo, double dSetValue)
{
	Write_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_PV_Steptime,dSetValue);
}
*/
//161019 zyx
void ChannelInfo_Manager::SetPV_CV_stagetime(unsigned char uc_ChannelNo)
{
    Write_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_CV_StageTime, MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageTime].m_fValue);
    MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageTime].m_bNeedRecal = 1;
}
//161019 zyx
//161226 zyx
void ChannelInfo_Manager::GetPV_CV_StageCurrent(unsigned char uc_ChannelNo)
{
    MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue = MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
}

void ChannelInfo_Manager::SetPV_CV_StageCurrent(unsigned char uc_ChannelNo, float TargetI)
{
    MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue = TargetI;
}

void ChannelInfo_Manager::SetPV_CV_StageCurrent(unsigned char uc_ChannelNo)
{
    unsigned char NonActiveIdx;
    NonActiveIdx = !ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    if(m_Online_UpdataEnable[uc_ChannelNo])
        NonActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    float PresentV = Get_MetaVariable(uc_ChannelNo, MP_DATA_TYPE_METAVALUE, DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,MetaCode_PV_Voltage);  // 当前电压值；
    float TargetV = ChannelStep[uc_ChannelNo].m_Info[NonActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0];
    unsigned char CurrentRange = ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
    bool bPositive = false;
    float fDeltaV = ::fabs(TargetV - PresentV);
    float fVoltLimit = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[ChannelInfo[uc_ChannelNo].m_ucVoltageRange].m_Bound.m_High;
    if(fDeltaV > fVoltLimit * 0.005f)
        bPositive = (TargetV > PresentV);
    else
        bPositive = (Get_MetaVariable(uc_ChannelNo, MP_DATA_TYPE_METAVALUE, DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,MetaCode_PV_Current) >= 0);

    if(bPositive)
        MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[CurrentRange].m_Bound.m_High;
    else
        MetaVariable2[uc_ChannelNo][MetaCode_PV_CV_StageCurrent].m_fValue = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[CurrentRange].m_Bound.m_Low;
}

//161226 zyx
// void ChannelInfo_Manager::IncCCapacity(unsigned short uc_ChannelNo, double dSetValue)
// {
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
// 	p_MV2[MetaCode_PV_ChargeCapacity].m_Value.m_double += dSetValue;
// 	p_MV2[MetaCode_PV_ChargeCapacity].m_fValue = p_MV2[MetaCode_PV_ChargeCapacity].m_Value.m_double;
// 	p_MV2[MetaCode_TC_ChargeCapacity1].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_TC_ChargeCapacity2].m_bNeedRecal = 1;

// 	CapaEnerUpdated[uc_ChannelNo].m_CCapacity = 1;   // 20140612 he  修改容量能量遗漏
// }
// void ChannelInfo_Manager::IncDCapacity(unsigned short uc_ChannelNo, double dSetValue)
// {
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
// 	p_MV2[MetaCode_PV_DischargeCapacity].m_Value.m_double += dSetValue;
// 	p_MV2[MetaCode_PV_DischargeCapacity].m_fValue = p_MV2[MetaCode_PV_DischargeCapacity].m_Value.m_double;
// 	p_MV2[MetaCode_TC_DischargeCapacity1].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_TC_DischargeCapacity2].m_bNeedRecal = 1;

// 	CapaEnerUpdated[uc_ChannelNo].m_DCapacity = 1;  // 20140612 he  修改容量能量遗漏
// }

void ChannelInfo_Manager::IncCapacityOrEnergy(unsigned char uc_MetaCode, unsigned short uc_ChannelNo, double dSetValue)
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][MetaCode_PV_ChargeCapacity];
    unsigned char uc_MetaOffset = uc_MetaCode % MetaCode_PV_ChargeCapacity;
    p_MV2 += uc_MetaOffset;
    p_MV2->m_Value.m_double += dSetValue;
    p_MV2->m_fValue = p_MV2->m_Value.m_double;

    p_MV2 = &MetaVariable2[uc_ChannelNo][MetaCode_TC_ChargeCapacity1 + 2 * uc_MetaOffset];
    p_MV2->m_bNeedRecal = 1;
    p_MV2 ++;
    p_MV2->m_bNeedRecal = 1;

// 		if(uc_MetaCode == MetaCode_PV_ChargeCapacity)//20190809 新增全局capacity
// 		{
// 			g_fChargeCapacity[uc_ChannelNo] += (float)dSetValue;
// 		}
// 		else if(uc_MetaCode == MetaCode_PV_DischargeCapacity)
// 		{
// 			g_fDischargeCapacity[uc_ChannelNo] += (float)dSetValue;
// 		}

    switch(uc_MetaCode)
    {
    case MetaCode_PV_ChargeCapacity:
        safety_check::g_dChargeCapacity[uc_ChannelNo] += dSetValue;  //20190809 新增全局capacity
        CapaEnerUpdated[uc_ChannelNo].m_CCapacity = 1;
        break;
    case MetaCode_PV_DischargeCapacity:
        safety_check::g_dDischargeCapacity[uc_ChannelNo] += dSetValue; //20190809 新增全局capacity
        CapaEnerUpdated[uc_ChannelNo].m_DCapacity = 1;
        break;
    case MetaCode_PV_ChargeEnergy:
        CapaEnerUpdated[uc_ChannelNo].m_CEnergy = 1;
        break;
    case MetaCode_PV_DischargeEnergy:
        CapaEnerUpdated[uc_ChannelNo].m_DEnergy = 1;
        break;
    default:
        break;
    }
}


// void ChannelInfo_Manager::IncCCapacityTime(unsigned short uc_ChannelNo, unsigned short usTimeUs100) //zyx 171010
// {
// //     float fCCTime = usTimeUs100 * 1.0e-4f;
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
// 	p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 += usTimeUs100;
//     unsigned long  m_sec = p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 / 10000;
//     if(m_sec > 0)
//     {
//         p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Second += m_sec;
//         p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 = p_MV2[MetaCode_PV_ChargeCapacityTime].m_Value.m_Time.Us100 % 10000;
//     }
// }
// void ChannelInfo_Manager::IncDCapacityTime(unsigned short uc_ChannelNo, unsigned short usTimeUs100) //zyx 171010
// {
// //     float fDCTime = usTimeUs100 * 1.0e-4f;
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
//     p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 += usTimeUs100;
//     unsigned long  m_sec = p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 / 10000;
//     if(m_sec > 0)
//     {
//         p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Second += m_sec;
//         p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 = p_MV2[MetaCode_PV_DischargeCapacityTime].m_Value.m_Time.Us100 % 10000;
//     }
// }

void ChannelInfo_Manager::IncCCapacityTimeOrDCapacityTime(unsigned char uc_MetaCode, unsigned short uc_ChannelNo, unsigned long TimeInUs)
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][MetaCode_PV_ChargeCapacityTime];
    unsigned char uc_MetaOffset = uc_MetaCode % MetaCode_PV_ChargeCapacityTime;
    p_MV2 += uc_MetaOffset;

    unsigned long TempUs = (p_MV2->m_Value.m_Time2us.Us + TimeInUs) % 1000000;
    unsigned long  TempSecond = (p_MV2->m_Value.m_Time2us.Us + TimeInUs) / 1000000;
    p_MV2->m_Value.m_Time2us.Us = TempUs;
    if(TempSecond > 0)
    {
        p_MV2->m_Value.m_Time2us.Second += TempSecond;
    }

}

// void ChannelInfo_Manager::IncCEnergy(unsigned short uc_ChannelNo, double dSetValue)
// {
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
// 	p_MV2[MetaCode_PV_ChargeEnergy].m_Value.m_double += dSetValue;
// 	p_MV2[MetaCode_PV_ChargeEnergy].m_fValue = p_MV2[MetaCode_PV_ChargeEnergy].m_Value.m_double;
// 	p_MV2[MetaCode_TC_ChargeEnergy1].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_TC_ChargeEnergy2].m_bNeedRecal = 1;

// 	CapaEnerUpdated[uc_ChannelNo].m_CEnergy = 1;
// }
// void ChannelInfo_Manager::IncDEnergy(unsigned short uc_ChannelNo, double dSetValue)
// {
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
// 	p_MV2[MetaCode_PV_DischargeEnergy].m_Value.m_double += dSetValue;
// 	p_MV2[MetaCode_PV_DischargeEnergy].m_fValue = p_MV2[MetaCode_PV_DischargeEnergy].m_Value.m_double;
// 	p_MV2[MetaCode_TC_DischargeEnergy1].m_bNeedRecal = 1;
// 	p_MV2[MetaCode_TC_DischargeEnergy2].m_bNeedRecal = 1;

// 	CapaEnerUpdated[uc_ChannelNo].m_DEnergy = 1;
// }

void ChannelInfo_Manager::DVmetaInit(unsigned char uc_ChannelNo)
{
    if((ChannelStep[uc_ChannelNo].m_bIsFirstLogPoint)||(CommonFunc::TimeDiffInSecond(MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Second) > (ChannelStep[uc_ChannelNo].m_fLogInterval + 5)))
    {
        MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime = CommonFunc::GetSystemTime();
//         MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Second = OS_ServeProcess::OS_Time.Second;
//         MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Us100 = OS_ServeProcess::OS_Time.Us100;
        ChannelStep[uc_ChannelNo].m_bIsFirstLogPoint = 0;
    }

    if (ChannelStep[uc_ChannelNo].m_bIsDvInterval)
    {
        unsigned long long TimeIncrement = 0;
        TimeIncrement = ChannelStep[uc_ChannelNo].m_fLogInterval * 10000;
        TimeIncrement += MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Us100;
        MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Second += TimeIncrement / 10000;
        MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Us100 = TimeIncrement % 10000;
    }
    else
    {
        MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime = CommonFunc::GetSystemTime();
//         MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Second = OS_ServeProcess::OS_Time.Second;
//         MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Us100 = OS_ServeProcess::OS_Time.Us100;
    }

    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    p_MV2[MetaCode_DV_Time].m_fValue  = 0;
    p_MV2[MetaCode_DV_Current].m_fValue  = 0;
    p_MV2[MetaCode_DV_Voltage].m_fValue  = 0;
    MetaVariables[uc_ChannelNo].m_DV_Info.m_fLastLogVoltage = p_MV2[MetaCode_PV_Voltage].m_fValue;
    MetaVariables[uc_ChannelNo].m_DV_Info.m_fLastLogCurrent = p_MV2[MetaCode_PV_Current].m_fValue;
}

void ChannelInfo_Manager::DVmetaClear(unsigned char uc_ChannelNo)
{
    ChannelStep[uc_ChannelNo].m_bIsFirstLogPoint = 0;
    MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime = CommonFunc::GetSystemTime();
//     MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Second = OS_ServeProcess::OS_Time.Second;
//     MetaVariables[uc_ChannelNo].m_DV_Info.m_LastLogTime.Us100 = OS_ServeProcess::OS_Time.Us100;

    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    p_MV2[MetaCode_DV_Time].m_fValue  = 0;
    p_MV2[MetaCode_DV_Time].m_bNeedRecal = 1;
    p_MV2[MetaCode_DV_Current].m_fValue  = 0;
    p_MV2[MetaCode_DV_Voltage].m_fValue  = 0;
    MetaVariables[uc_ChannelNo].m_DV_Info.m_fLastLogVoltage = p_MV2[MetaCode_PV_Voltage].m_fValue;
    MetaVariables[uc_ChannelNo].m_DV_Info.m_fLastLogCurrent = p_MV2[MetaCode_PV_Current].m_fValue;
}

void ChannelInfo_Manager::ResetNormalMV(unsigned short ChannelIdx, unsigned short us_MV_ResetType)   //zyx 171010
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[ChannelIdx][0];
    p_MV2[us_MV_ResetType].m_fValue = 0.0f;
    memset(&p_MV2[us_MV_ResetType].m_Value, 0, sizeof(Un_ValueConversion));
}


//以下三个函数用于 step control. set variable
void ChannelInfo_Manager::Reset(unsigned short ChannelIdx, unsigned char uc_MV_ResetType)
{
// 	st_GetMetaValue * p_MV2 = &MetaVariable2[ChannelIdx][0];
// 	St_Metavariable * p_MV  = &MetaVariables[ChannelIdx];
    st_GetMetaValue * p_Meta;
    unsigned char uc_MetaOffset = uc_MV_ResetType;
    switch(uc_MV_ResetType)
    {
    case MP_MV_RESET_CHAN_CHARGE_CAP:
// 		p_MV->m_TC_Info.m_ChargeCapacity1.m_StartCapacity += p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		p_MV->m_TC_Info.m_ChargeCapacity2.m_StartCapacity += p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
// 		p_MV2[MetaCode_PV_ChargeCapacity].m_fValue = 0.0f;
// 		p_MV2[MetaCode_PV_ChargeCapacity].m_Value.m_double = 0.0f;
// 		break;
    case MP_MV_RESET_CHAN_DISCHARGE_CAP:
// 		p_MV->m_TC_Info.m_DischargeCapacity1.m_StartCapacity += p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		p_MV->m_TC_Info.m_DischargeCapacity2.m_StartCapacity += p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
// 		p_MV2[MetaCode_PV_DischargeCapacity].m_fValue = 0.0f;
// 		p_MV2[MetaCode_PV_DischargeCapacity].m_Value.m_double = 0.0f;
// 		break;
    case MP_MV_RESET_CHAN_CHARGE_ENERGY:
// 		p_MV->m_TC_Info.m_ChargeEnergy1.m_StartCapacity += p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		p_MV->m_TC_Info.m_ChargeEnergy2.m_StartCapacity += p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
// 		p_MV2[MetaCode_PV_ChargeEnergy].m_fValue = 0.0f;
// 		p_MV2[MetaCode_PV_ChargeEnergy].m_Value.m_double = 0.0f;
// 		break;
    case MP_MV_RESET_CHAN_DISCHARGE_ENERGY:
// 		p_MV->m_TC_Info.m_DischargeEnergy1.m_StartCapacity += p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		p_MV->m_TC_Info.m_DischargeEnergy2.m_StartCapacity += p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
// 		p_MV2[MetaCode_PV_DischargeEnergy].m_fValue = 0.0f;
// 		p_MV2[MetaCode_PV_DischargeEnergy].m_Value.m_double = 0.0f;

        p_Meta = &MetaVariable2[ChannelIdx][MetaCode_PV_ChargeCapacity];
        uc_MetaOffset -= MP_MV_RESET_CHAN_CHARGE_CAP;
        p_Meta += uc_MetaOffset;
        St_CapacityRegister * Register = &MetaVariables[ChannelIdx].m_TC_Info.m_ChargeCapacity1;
        Register += 2 * uc_MetaOffset;

        Register->m_StartCapacity += p_Meta->m_fValue;
        Register ++;
        Register->m_StartCapacity += p_Meta->m_fValue;
        p_Meta->m_fValue = 0.0f;
        p_Meta->m_Value.m_double = 0.0f;
        break;
    case MP_MV_RESET_COUNTER_TIMER1:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_Timer1,0);
// 		break;
    case MP_MV_RESET_COUNTER_TIMER2:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_Timer2,0);
// 		break;
    case MP_MV_RESET_COUNTER_TIMER3:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_Timer3,0);
// 		break;
    case MP_MV_RESET_COUNTER_TIMER4:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_Timer4,0);
// 		break;
    case MP_MV_RESET_COUNTER_CHARGE_CAP1:
//  	Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_ChargeCapacity1,0);
// 		break;
    case MP_MV_RESET_COUNTER_CHARGE_CAP2:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_ChargeCapacity2,0);
// 		break;
    case MP_MV_RESET_COUNTER_DISCHARGE_CAP1:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_DischargeCapacity1,0);
// 		break;
    case MP_MV_RESET_COUNTER_DISCHARGE_CAP2:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_DischargeCapacity2,0);
// 		break;
    case MP_MV_RESET_COUNTER_CHARGE_ENERGY1:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_ChargeEnergy1,0);
// 		break;
    case MP_MV_RESET_COUNTER_CHARGE_ENERGY2:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_ChargeEnergy2,0);
// 		break;
    case MP_MV_RESET_COUNTER_DISCHARGE_ENERGY1:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_DischargeEnergy1,0);
// 		break;
    case MP_MV_RESET_COUNTER_DISCHARGE_ENERGY2:
// 		Write_SpecifiedMetavariable2(ChannelIdx,MetaCode_TC_DischargeEnergy2,0);
// 		break;

        uc_MetaOffset = MetaCode_TC_Timer1 + uc_MetaOffset - MP_MV_RESET_COUNTER_TIMER1;
        Write_SpecifiedMetavariable2(ChannelIdx, uc_MetaOffset, 0);
        break;

    case MP_MV_RESET_COUNTER_COUNTER1:
// 		p_MV2[MetaCode_TC_Counter1].m_fValue = 0.0f;
// 		break;
    case MP_MV_RESET_COUNTER_COUNTER2:
// 		p_MV2[MetaCode_TC_Counter2].m_fValue = 0.0f;
// 		break;
    case MP_MV_RESET_COUNTER_COUNTER3:
// 		p_MV2[MetaCode_TC_Counter3].m_fValue = 0.0f;
// 		break;
    case MP_MV_RESET_COUNTER_COUNTER4:
// 		p_MV2[MetaCode_TC_Counter4].m_fValue = 0.0f;
// 		break;

        p_Meta = &MetaVariable2[ChannelIdx][MetaCode_TC_Counter1];
        uc_MetaOffset -= MP_MV_RESET_COUNTER_COUNTER1;
        p_Meta += uc_MetaOffset;
        p_Meta->m_fValue = 0.0f;
        break;
    default:
        break;
    }
}
void ChannelInfo_Manager::Increment(unsigned short ChannelIdx, unsigned char uc_MV_IncrementType)
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[ChannelIdx][0];
    switch(uc_MV_IncrementType)
    {
    case MP_MV_INC_CHAN_CYCLE_INDEX:
        p_MV2[MetaCode_PV_CycleIndex].m_fValue += 1;
        safety_check::g_dChargeCapacity[ChannelIdx] = 0;//20200316修改为全局g_fChargeCapacity只对每个cycle有效
        safety_check::g_dDischargeCapacity[ChannelIdx] = 0;//20200316修改为全局g_fDischargeCapacity只对每个cycle有效
        //Copy_PVmeta2LCmeta(ChannelIdx);
        break;
    case MP_MV_INC_COUNTER_COUNTER1:
// 		p_MV2[MetaCode_TC_Counter1].m_fValue += 1;
// 		break;
    case MP_MV_INC_COUNTER_COUNTER2:
// 		p_MV2[MetaCode_TC_Counter2].m_fValue += 1;
// 		break;
    case MP_MV_INC_COUNTER_COUNTER3:
// 		p_MV2[MetaCode_TC_Counter3].m_fValue += 1;
// 		break;
    case MP_MV_INC_COUNTER_COUNTER4:
// 		p_MV2[MetaCode_TC_Counter4].m_fValue += 1;
// 		break;
        unsigned char uc_MetaOffset = MetaCode_TC_Counter1 + uc_MV_IncrementType - MP_MV_INC_COUNTER_COUNTER1;
        p_MV2[uc_MetaOffset].m_fValue += 1;
        break;
    default:
        break;
    }
}
void ChannelInfo_Manager::Decrement(unsigned short ChannelIdx, unsigned char uc_MV_DecrementType)
{
    st_GetMetaValue * p_MV2 = &MetaVariable2[ChannelIdx][0];
    switch(uc_MV_DecrementType)
    {
    case MP_MV_DEC_COUNTER_COUNTER1:
// 		p_MV2[MetaCode_TC_Counter1].m_fValue--;
// 		if(p_MV2[MetaCode_TC_Counter1].m_fValue < 0)
// 			p_MV2[MetaCode_TC_Counter1].m_fValue = 0;
// 		break;
    case MP_MV_DEC_COUNTER_COUNTER2:
// 		p_MV2[MetaCode_TC_Counter2].m_fValue--;
// 		if(p_MV2[MetaCode_TC_Counter2].m_fValue < 0)
// 			p_MV2[MetaCode_TC_Counter2].m_fValue = 0;
// 		break;
    case MP_MV_DEC_COUNTER_COUNTER3:
// 		p_MV2[MetaCode_TC_Counter3].m_fValue--;
// 		if(p_MV2[MetaCode_TC_Counter3].m_fValue < 0)
// 			p_MV2[MetaCode_TC_Counter3].m_fValue = 0;
// 		break;
    case MP_MV_DEC_COUNTER_COUNTER4:
// 		p_MV2[MetaCode_TC_Counter4].m_fValue--;
// 		if(p_MV2[MetaCode_TC_Counter4].m_fValue < 0)
// 			p_MV2[MetaCode_TC_Counter4].m_fValue = 0;
// 		break;

        unsigned char uc_MetaOffset = MetaCode_TC_Counter1 + uc_MV_DecrementType - MP_MV_DEC_COUNTER_COUNTER1;
        p_MV2[uc_MetaOffset].m_fValue--;         //ZZJ 190722
        if(p_MV2[uc_MetaOffset].m_fValue < 0)    //ZZJ 190722
            p_MV2[uc_MetaOffset].m_fValue = 0;   //ZZJ 190722
        break;
    default:
        break;
    }
}

bool ChannelInfo_Manager::Insert_Current2HistoryData(unsigned char uc_ChannelNo, unsigned long Current, unsigned char Range)
{
    unsigned long CurrentOut;
    //    unsigned char bFull;
    st_GetMetaValue * p_MV2 = &MetaVariable2[uc_ChannelNo][0];
    St_HistoryData * p_History = &HistoryData[uc_ChannelNo];
    St_ChannelInfo * p_ChnInfo = &ChannelInfo[uc_ChannelNo];
//#if _BOARD_TYPE_ == BOARDTYPE_BT2208
// 	if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_BT2208)
// 	{
// #if BOARDTYPE_BT2208SINGLE==1
// 	    ChannelInfo[0].chn_State = ChannelInfo[BT2208_1_CHN_NUM].chn_State;  //low
//   	    ChannelInfo[8].chn_State = ChannelInfo[BT2208_1_CHN_NUM].chn_State;  //mid
// #endif
//     }
//#endif
    switch(p_ChnInfo->chn_State)
    {
    case LOGIC_PROCESS_CHN_CLOSE:
        m_RecordMasterChannelPVCurrent[uc_ChannelNo] = p_MV2[MetaCode_PV_Current].m_fValue = 0;
        p_ChnInfo->m_GroupData.m_fGroupCurrent = 0;
        IV_Buffer1[uc_ChannelNo].m_fCurrent = 0.0f;
        CQ::InitQueue(p_History->m_uCurrHnd);
        p_History->m_u32CurrentSum = 0;
        IV_Buffer1[uc_ChannelNo].m_uCurrCount = 0;
        IV_Buffer1[uc_ChannelNo].m_fCurrentBeforeRC = 0;
        return true;
    case LOGIC_PROCESS_STEADY:
    case LOGIC_PROCESS_CHN_OPEN:
        break;
    case LOGIC_PROCESS_TRAN:
    // if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started)//DK 210714
    //     break;
    default:
        CQ::InitQueue(p_History->m_uCurrHnd);
        p_History->m_u32CurrentSum = 0;
        IV_Buffer1[uc_ChannelNo].m_uCurrCount = 0;
        return false;
    }

    //20150331 add start:电流采样滤波 一级缓冲区
    St_IV_Buffer1 * p_Buf = &IV_Buffer1[uc_ChannelNo];
//     if(!p_ChnInfo->m_bNeedFilter)
    if(GetFilterStatus(uc_ChannelNo) == false)
        p_Buf->m_uCurrCount = 0;

    switch(p_Buf->m_uCurrCount)
    {
    case 2:
        unsigned long a, b, c;
        a = p_Buf->m_CurrA;
        b = p_Buf->m_CurrB;
        c = Current;
        unsigned long ba, ca;
        if(b > a)       ba = b - a;
        else            ba = a - b;
        if(c > a)       ca = c - a;
        else            ca = a - c;
        if ((ba / ca > IV_Buffer1_Alpha) && (ba > IV_Buffer1_Beta)) //b为野点
            p_Buf->m_CurrB = c;                  //抛弃b点，
        else
        {
            p_Buf->m_CurrA = b;                 //b不是野点
            p_Buf->m_CurrB = c;
            p_Buf->m_uCurrAfterFilter = b;
        }
        break;
    case 0://buffer里没有采样值，则使用最新采样进行1级BUFFER
        p_Buf->m_CurrA = Current;
        p_Buf->m_uCurrCount = 1;
        p_Buf->m_uCurrAfterFilter = Current;
        break;
    case 1://buffer里有一个采样值，因为新的采样值无法判断是否为野值，保持不变
        p_Buf->m_CurrB = Current;
        p_Buf->m_uCurrCount = 2;
        break;
    default://buffer复位
        p_Buf->m_uCurrCount = 1;
        p_Buf->m_CurrA = Current;
        p_Buf->m_uCurrAfterFilter = Current;
        break;
    }
    Current = p_Buf->m_uCurrAfterFilter;
    p_Buf->m_fCurrent = CalibrateCurrentADC(uc_ChannelNo, Range, Current);
    //20150331 add end:电流采样滤波 一级缓冲区

//     if(p_ChnInfo->m_bNeedFilter)
    if(GetFilterStatus(uc_ChannelNo) == true)
    {
        unsigned long u32Delta;

        u32Delta = (Current > p_History->m_u32LastCurrentValue) ?
                   (Current - p_History->m_u32LastCurrentValue) : (p_History->m_u32LastCurrentValue - Current);

        if(u32Delta >= p_ChnInfo->m_u32CurrentRampDelta)
        {
            CQ::InitQueue(p_History->m_uCurrHnd);
            HistoryData[uc_ChannelNo].m_u32CurrentSum = 0;
        }
        if(CQ::IsQueueFull(p_History->m_uCurrHnd))
        {
            CQ::Dequeue(p_History->m_uCurrHnd, &CurrentOut);
            p_History->m_u32CurrentSum -= CurrentOut;   //如果挤出了一个数，那么 总和 需要减掉这个数
        }
        CQ::Enqueue(p_History->m_uCurrHnd, &Current);
        p_History->m_u32LastCurrentValue = Current;

        p_History->m_u32CurrentSum += Current;          //新加的数需要 累加加入 总和
    }
    else
    {
        CQ::InitQueue(p_History->m_uCurrHnd);
        CQ::Enqueue(p_History->m_uCurrHnd, &Current);
        p_History->m_u32LastCurrentValue = Current;
        p_History->m_u32CurrentSum = Current;
    }

    CurrentOut = Read_CurrentAfterFilter(uc_ChannelNo);
    /*
    #if _BOARD_TYPE_ == BOARDTYPE_BT2208
    #if BOARDTYPE_BT2208SINGLE==1
    if(ChannelInfo[14].m_ucCurrentRange == CURRENT_RANGE_1)
    {
    if(uc_ChannelNo == 14)
    {
    Range = CURRENT_RANGE_1;
    uc_ChannelNo = 14;
    }
    else
    {
    return false;
    }

    }
    else if(ChannelInfo[14].m_ucCurrentRange == CURRENT_RANGE_2)
    {
    if(uc_ChannelNo == 8)
    {
    Range = CURRENT_RANGE_2;
    uc_ChannelNo = 14;
    }
    else
    {
    return false;
    }

    }
    else if(ChannelInfo[14].m_ucCurrentRange == CURRENT_RANGE_3)
    {
    if(uc_ChannelNo == 0)
    {
    Range = CURRENT_RANGE_3;
    uc_ChannelNo = 14;
    }
    else
    {
    return false;
    }
    }
    #endif
    #endif
    */
    p_MV2[MetaCode_PV_Current].m_fValue = CalibrateCurrentADC(uc_ChannelNo, Range, CurrentOut);
    m_RecordMasterChannelPVCurrent[uc_ChannelNo] = p_MV2[MetaCode_PV_Current].m_fValue;

    if(IVSDL::m_GrpInfo4SDL[uc_ChannelNo].State == eGroupState_Normal)
    {
//         IVSDL::PushCurrent2HistoryData(uc_ChannelNo, p_MV2[MetaCode_PV_Current].m_fValue, CurrentOut);
        ;
    }
    else if (IVSDL::m_GrpInfo4SDL[uc_ChannelNo].State == eGroupState_Master)
    {
        float fCurrSum = m_ParaFeedbackCtrl[uc_ChannelNo].m_MasterCurrent = p_MV2[MetaCode_PV_Current].m_fValue;  //get  Master Chn Current value

        for (unsigned char i = uc_ChannelNo + 1 ; i < BoardCFG.m_Global.m_u16IVChannelCount; i ++)
        {
            if(IVSDL::m_GrpInfo4SDL[i].State != eGroupState_Slave)
                break;

#if (USEDSETPARALVALUE==1)

            fCurrSum += RecordRefreshValueForParal[i];

#else
            fCurrSum += MetaVariable2[i][MetaCode_PV_Current].m_fValue;
#endif
        }
        if(IVSDL::m_GrpInfo4SDL[uc_ChannelNo].SubmastCount > 0)
        {
            short int w16_ID = -1;
            for(unsigned char uc_i = 0; uc_i < gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_uGroupUnitCount; uc_i++)
            {
                unsigned char m_groupSubUnitID = gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_UnitGroupInfo.m_SubMasterChannels[uc_i].m_Unit_Idx;
#if (USEDSETPARALVALUE==1)
                fCurrSum += RecordRefreshValueForParal[uc_i];
#else
                if(w16_ID != m_groupSubUnitID)
                    fCurrSum += gUnit_stUnitInfo.m_GroupInfo[gUnit_stUnitInfo.m_InterMasterGroupID].m_stSubGroupData[m_groupSubUnitID].m_fSubGroupChanCurrent;
                w16_ID = m_groupSubUnitID;
#endif
            }
        }
        p_ChnInfo->m_GroupData.m_fGroupCurrent = fCurrSum;
        p_MV2[MetaCode_PV_Current].m_fValue = fCurrSum;
//         IVSDL::PushCurrent2HistoryData_FloatOnly(uc_ChannelNo, fCurrSum, CurrentOut); //leave the CurrentOut alone. It does not count.
    }
    else if (IVSDL::m_GrpInfo4SDL[uc_ChannelNo].State == eGroupState_Submaster)
    {
        m_ParaFeedbackCtrl[uc_ChannelNo].m_MasterCurrent = p_MV2[MetaCode_PV_Current].m_fValue;  //get  Master Chn Current value
        float fCurrSum = p_MV2[MetaCode_PV_Current].m_fValue;

        for (unsigned char i = uc_ChannelNo + 1 ; i < BoardCFG.m_Global.m_u16IVChannelCount; i ++)
        {
            if(IVSDL::m_GrpInfo4SDL[i].State != eGroupState_Slave)
                break;
#if (USEDSETPARALVALUE==1)
            fCurrSum += RecordRefreshValueForParal[i];
#else
            fCurrSum += MetaVariable2[i][MetaCode_PV_Current].m_fValue;
#endif
        }
        p_ChnInfo->m_GroupData.m_fGroupCurrent = fCurrSum;
        p_MV2[MetaCode_PV_Current].m_fValue = fCurrSum;
//     IVSDL::Check_Submaster(uc_ChannelNo); zyx 181015
    }


    p_Buf->m_fCurrentBeforeRC = MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
    if(m_NeedRCFilterFlg[uc_ChannelNo])
    {
        MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = RC_FilterOutput(uc_ChannelNo,  \
                MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue,  \
                &m_RC_HistoryCurrentFilterValue[uc_ChannelNo], 							\
                m_RC_CurrentCoefficient[uc_ChannelNo],											\
                m_RC_CurrentFilterDeepth[uc_ChannelNo]);
    }
    if((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started) && (ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_TransFinished))
    {
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_VolSample_Started =true;
//         float Current = abs(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue) *0.05f;
//         if ((abs(p_MV2[MetaCode_PV_Current].m_fValue) <Current) && (!ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_VolUpdateFlag))
//         {
//             ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_VolUpdateFlag = true;
//             ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage);
//             ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = p_MV2[MetaCode_PV_Current].m_fValue;
//         }
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_Current[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] =p_MV2[MetaCode_PV_Current].m_fValue;
//         ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_IStepTime[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] =p_MV2[MetaCode_PV_Steptime].m_fValue;
// 		ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_IStepIndex[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] =p_MV2[MetaCode_PV_StepIndex].m_fValue;
//         ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_ITransTime[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] = \
// 		CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_TransStartTime)*0.1;
    }
    return true;
}
bool ChannelInfo_Manager::Insert_BuildInVolt2HistoryData(unsigned char uc_ChannelNo, unsigned char BuildInIdx, unsigned long Voltage)
{
    unsigned long VoltageOut;
    //    unsigned char bFull;

    St_WindowFilter * p_V = &HistoryData[uc_ChannelNo].m_BuildInVolt[BuildInIdx];

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].chn_State == LOGIC_PROCESS_TRAN)
    {
        CQ::InitQueue(p_V->m_uHandler);
        p_V->m_u32Sum = 0;
        return false;
    }

//     if(ChannelInfo[uc_ChannelNo].m_bNeedFilter)
    if(GetFilterStatus(uc_ChannelNo) == true)
    {
        if(CQ::IsQueueFull(p_V->m_uHandler))
        {
            CQ::Dequeue(p_V->m_uHandler, &VoltageOut);
            p_V->m_u32Sum -= VoltageOut;   //如果挤出了一个数，那么 总和 需要减掉这个数
        }
        CQ::Enqueue(p_V->m_uHandler, &Voltage);
        p_V->m_u32Sum += Voltage;          //新加的数需要 累加加入 总和
    }
    else
    {
        CQ::InitQueue(p_V->m_uHandler);
        CQ::Enqueue(p_V->m_uHandler, &Voltage);
        p_V->m_u32Sum = Voltage;
    }

    VoltageOut = Read_BuildInVoltAfterFilter(uc_ChannelNo, BuildInIdx);
    MetaVariable2[uc_ChannelNo][MetaCode_PV_BuildIn_V1 + BuildInIdx].m_fValue =
        VoltageOut * BoardCFG.m_Channel[uc_ChannelNo].m_BuildInV[BuildInIdx].m_Gain + BoardCFG.m_Channel[uc_ChannelNo].m_BuildInV[BuildInIdx].m_Offset;

    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044)
    {   //190524,hpq
        ChannelInfo[uc_ChannelNo].m_fBulitInData[0]
            = MetaVariable2[uc_ChannelNo][MetaCode_PV_BuildIn_V1 + BuildInIdx].m_fValue;
        MetaVariable2[uc_ChannelNo][MetaCode_PV_BuiltIn_Data1 + 0].m_fValue
            = MetaVariable2[uc_ChannelNo][MetaCode_PV_BuildIn_V1 + BuildInIdx].m_fValue;
    }
    return true;
}
bool ChannelInfo_Manager::Insert_Voltage2HistoryData(unsigned char uc_ChannelNo, unsigned long Voltage, unsigned char Range)
{
    unsigned long VoltageOut;
    //    unsigned char bFull;

    if(Volt_Range_Change_Flag[uc_ChannelNo])
    {
        unsigned long Voltage_Change_Tick = CommonFunc::GetTimeTicks(&Voltage_Range_Change_Time[uc_ChannelNo]);
        if(Voltage_Change_Tick < board_config::Voltage_Range_Change_Time)
        {
            //预留3ms给通道做电压量程切换
            Sample_Pro::NewCodeSampled[uc_ChannelNo] = 0;
            return false;
        }
        else
        {
            Volt_Range_Change_Flag[uc_ChannelNo] = 0;
            Init_HistoryData(uc_ChannelNo);
        }
    }

    if(ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].chn_State == LOGIC_PROCESS_TRAN)
    {
        if(!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_VolSample_Started )
        {
            CQ::InitQueue(HistoryData[uc_ChannelNo].m_uVoltHnd);
            HistoryData[uc_ChannelNo].m_u32VoltageSum = 0;
//        MetaVariable_Task::ZaInitDvdt(uc_ChannelNo);//180921 zyx
            return false;
        }
    }

    //20150331 add start:电流采样滤波 一级缓冲区
//     if(!ChannelInfo[uc_ChannelNo].m_bNeedFilter)
    if(GetFilterStatus(uc_ChannelNo) == false)
        IV_Buffer1[uc_ChannelNo].m_uVoltCount = 0;
    switch(IV_Buffer1[uc_ChannelNo].m_uVoltCount)
    {
    case 2:
        unsigned long a, b, c;
        a = IV_Buffer1[uc_ChannelNo].m_VoltA;
        b = IV_Buffer1[uc_ChannelNo].m_VoltB;
        c = Voltage;
        unsigned long ba, ca;
        if(b > a)       ba = b - a;
        else            ba = a - b;
        if(c > a)       ca = c - a;
        else            ca = a - c;
        if ((ba / ca > IV_Buffer1_Alpha) && (ba > IV_Buffer1_Beta)) //b为野点
            IV_Buffer1[uc_ChannelNo].m_VoltB = c;                  //抛弃b点，
        else
        {
            IV_Buffer1[uc_ChannelNo].m_VoltA = b;                 //b不是野点
            IV_Buffer1[uc_ChannelNo].m_VoltB = c;
            IV_Buffer1[uc_ChannelNo].m_uVoltAfterFilter = b;
        }
        break;
    case 0://buffer里没有采样值，则使用最新采样进行1级BUFFER
        IV_Buffer1[uc_ChannelNo].m_VoltA = Voltage;
        IV_Buffer1[uc_ChannelNo].m_uVoltCount = 1;
        IV_Buffer1[uc_ChannelNo].m_uVoltAfterFilter = Voltage;
        break;
    case 1://buffer里有一个采样值，因为新的采样值无法判断是否为野值，保持不变
        IV_Buffer1[uc_ChannelNo].m_VoltB = Voltage;
        IV_Buffer1[uc_ChannelNo].m_uVoltCount = 2;
        break;
    default://buffer复位
        IV_Buffer1[uc_ChannelNo].m_uVoltCount = 1;
        IV_Buffer1[uc_ChannelNo].m_VoltA = Voltage;
        IV_Buffer1[uc_ChannelNo].m_uVoltAfterFilter = Voltage;
        break;
    }
    Voltage = IV_Buffer1[uc_ChannelNo].m_uVoltAfterFilter;
    IV_Buffer1[uc_ChannelNo].m_fVoltage = CalibrateVoltageADC(uc_ChannelNo, Range, Voltage);
    //20150331 add end:电流采样滤波 一级缓冲区



//     if(ChannelInfo[uc_ChannelNo].m_bNeedFilter)
    if(GetFilterStatus(uc_ChannelNo) == true)
    {

        if(CQ::IsQueueFull(HistoryData[uc_ChannelNo].m_uVoltHnd))
        {
            CQ::Dequeue(HistoryData[uc_ChannelNo].m_uVoltHnd, &VoltageOut);
            HistoryData[uc_ChannelNo].m_u32VoltageSum -= VoltageOut;   //如果挤出了一个数，那么 总和 需要减掉这个数
        }
        CQ::Enqueue(HistoryData[uc_ChannelNo].m_uVoltHnd, &Voltage);

        HistoryData[uc_ChannelNo].m_u32VoltageSum += Voltage;          //新加的数需要 累加加入 总和
    }
    else
    {
        CQ::InitQueue(HistoryData[uc_ChannelNo].m_uVoltHnd);
        CQ::Enqueue(HistoryData[uc_ChannelNo].m_uVoltHnd, &Voltage);
        HistoryData[uc_ChannelNo].m_u32VoltageSum = Voltage;
    }

    //CommonFunc::GetTimeTicksUpdate(&LastFilterPointStamp);
    HistoryData[uc_ChannelNo].m_u32LastVoltageValue = Voltage;

    VoltageOut = Read_VoltageAfterFilter(uc_ChannelNo);

    float fVolt = CalibrateVoltageADC(uc_ChannelNo, Range, VoltageOut);
    if(fVolt > ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_VmaxOnCycle].m_fValue)
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_VmaxOnCycle].m_fValue = fVolt;
    MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue = fVolt;

//     IVSDL::PushVoltage2HistoryData(uc_ChannelNo, MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue, VoltageOut);

    if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_VolSample_Started)
    {
        unsigned char uc_point=0;
        double temp_IR=0;
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Voltage[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter]=fVolt;
// 		ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_VStepTime[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] =ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Steptime].m_fValue;
//         ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_VStepIndex[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] =ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue;
// 		ChannelInfo_Manager::m_IR_Info[uc_ChannelNo]. m_RisingEdge_VTransTime[ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter] = \
// 		CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_TransStartTime)*0.1;
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter++;
        if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter>= 10)//DK 210714
        {
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_VolSample_Started= false;
            float V_Limit = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[0].m_Bound.m_High;  //temporally only one voltage is assumed
            float Voltage_Accurace = V_Limit * 1E-4f; //might be needed to adjust based on the system requirment
            float Current = abs(ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current-ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue) *0.9f;
            float sum_voltage = 0;
            float sum_current = 0;
            float avg_current = 0;
            float avg_voltage = 0;
            for(unsigned short i=2; i<=6; i++)
            {
                sum_voltage +=  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Voltage[i];
                sum_current +=  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Current[i];

            }
            avg_current = (float)sum_current /5.0;
            avg_voltage = (float)sum_voltage /5.0;


            double Voltage_Difference = ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage - avg_voltage; //ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Voltage[i]);
            double Current_Difference = ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current - avg_current; //ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Current[i]) ;
            if((abs(Voltage_Difference)>=Voltage_Accurace)&& (Current_Difference !=0))
                temp_IR = Voltage_Difference /Current_Difference;//(ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current-ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Current[i]) ;

            else
                temp_IR =0;

            if (temp_IR > 0)	// only after get a valid value we update the IR, otherwise, we keep whatever it has. // qjm 210513
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_RisingEdge = temp_IR;
                //ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue =temp_IR;  //dirui del 20200407
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge = true;   //????
            }
            else
            {
// 				if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IsCCCVWAM_Flag)
// 				{
// 					ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IsCCCVWAM_Flag=false;
// 					ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_CCCVWAM_Enable =true;
// 				}
// 				else
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_ZeroTest_Enable =true;
            }
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started =false;
// 			unsigned char ActiveIdx = ChannelStep[uc_ChannelNo].m_ucActiveIdx;
// 			St_StepCtrl			* 	p_Ctrl	=	&ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;
// 			if(p_Ctrl->m_u16ControlType==CT_IVCH_CURRENT)
// 				ChannelInfo[uc_ChannelNo].m_bNeedFilter =1;
            RecoverFilter(uc_ChannelNo);
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_TransFinished = false;   // Reset the flag//DK 210714
        }
    }

    if(m_NeedRCFilterFlg[uc_ChannelNo])
    {
        MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue = RC_FilterOutput(uc_ChannelNo,  \
                MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue,  \
                &m_RC_HistoryVoltageFilterValue[uc_ChannelNo], 							\
                m_RC_VoltageCoefficient[uc_ChannelNo],											\
                m_RC_VoltageFilterDeepth[uc_ChannelNo]);
    }
    // Set_VoltageRecalculateSign(uc_ChannelNo);
    MetaVariable_Task::Calculate_dVdt(uc_ChannelNo);//180921 zyx
    return true;
}
int ChannelInfo_Manager::LoadNextStep(unsigned char uc_ChannelNo, unsigned short u16_GotoStepID) //u16_GotoStepID为从0开始的自然数，0指代第一步
{
    signed char    sc_SchBufIdx     = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    unsigned short u16_RunningStepID;

//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
        Schedule_Manager_task::STEP_AUXLIMITBIT[uc_ChannelNo] = 0; //清除辅助limit处理标记，重新判断  dirui 20131226
//#endif

    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return -1;     //schedule buffer index 指针越界，返回-1;

    if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
        return -1;     //schedule buffer 不合法，返回-1;

    u16_RunningStepID = ChannelSchedule[uc_ChannelNo].m_RunningStepIdx;    //获取当前步骤的步骤号
    switch (u16_GotoStepID)
    {
    case MCU_GOTONEXTSTEP:
        if(((u16_RunningStepID + 1) < ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount)
                && (ScheduleBuf[sc_SchBufIdx].m_StepData[u16_RunningStepID + 1].m_bIRTestStep != 1)) //若下一步是IR测试步的话不能处理  Di 20140617
        {
            LoadStepData(uc_ChannelNo, u16_RunningStepID + 1);
            ChannelSchedule[uc_ChannelNo].m_NextStepIdx = u16_RunningStepID + 1;
            //InitStep(u16_RunningStepID+1);     //step运行时参数初始化，有待完成
        }
        else
        {
            IVSDL::ScheduleEndedRoutine(uc_ChannelNo);
            return -1;  //Stop Channel;
        }
        break;
// 	case MCU_GOTOSTOPSTEP:
// 		//Stop Channel;
// 		  break;
    case MCU_GOTOSAMESTEP:
        LoadStepData(uc_ChannelNo, u16_RunningStepID);
        break;
// 	case MCU_GOTOUNSAFE:
// 		//Stop Channel And Make Unsafe;
// 		break;
// 	case MCU_GOTOWARNING:
// 		 //Make Warning;
// 		break;
    default:
        if ((u16_GotoStepID < ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount) ||
                (ScheduleBuf[sc_SchBufIdx].m_StepData[u16_GotoStepID].m_bIRTestStep == 1))
        {
            LoadStepData(uc_ChannelNo, u16_GotoStepID);
            ChannelSchedule[uc_ChannelNo].m_NextStepIdx = u16_GotoStepID;
            //InitStep(u16_GotoStepID);
        }
        else
        {
            IVSDL::ScheduleEndedRoutine(uc_ChannelNo);
            //Stop Channel;
        }
        break;
    }

    return 1;
}
int ChannelInfo_Manager::InsertSchedule2Buf(St_MD5 ScheduleID, void *pData)
{
    signed char sc_BufIdx;
    unsigned long CopiedSize = 0;
    St_MD5 OldMd5;

    sc_BufIdx = FindScheduleBuf(ScheduleID);
    if(sc_BufIdx != -1)
        return 0;      //标示 这个Schedule MD5已经存在了，无需再加入到BUFFER里了；

    sc_BufIdx = GetEmptyScheduleBuf();
    if(sc_BufIdx == -1)
        return -1;    //标示 schedule buffer满了，无法再插入新的Schedule，当然，这种情况不会出现；

		memset(&ScheduleBuf[sc_BufIdx],0,sizeof(ScheduleBuf[sc_BufIdx]));	//dyy 20220803 解决特殊IR步有机率出现不跳步的问题
    St_ScheduleBuffer			*		p_Buff	=		&ScheduleBuf[sc_BufIdx];
    St_ScheduleItemCount	*		p_Count = 	&ScheduleBuf[sc_BufIdx].m_ItemCount;

    if(p_Buff->m_bValid)  //we are going to abandon this schedule,so abandon corresponding simulation file firstly
    {
        for (unsigned short StepIdx = 0; StepIdx < p_Count->m_u16StepCount; StepIdx++)
        {
            St_StepCtrl * p_Ctrl = &p_Buff->m_StepData[StepIdx].m_Ctrl;
            if((p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_I)
                    || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_V)
                    || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_L)
                    || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_P))
            {
                SIMU_BUFFER::UnRegisterIV(sc_BufIdx, p_Ctrl->m_CtrlValues.m_MD5);
            }
        }
    }

    p_Buff->m_ucMacCurrentRange = CURRENT_RANGE_3;

    memcpy(&OldMd5,                                                                   //Copy Setting And ItemCount Segment
           (void *)((unsigned long)pData),
           sizeof(St_MD5) );
    CopiedSize += sizeof(St_MD5);

    p_Buff->m_ID = ScheduleID;
    memcpy(&p_Buff->m_Setting,                                                                   //Copy Setting And ItemCount Segment
           (void *)((unsigned long)pData + sizeof(St_MD5)),
           sizeof(St_ScheduleSetting) + sizeof(St_ScheduleItemCount) ); //sizeof(St_SchTestObject) +
    CopiedSize += sizeof(St_ScheduleSetting) + sizeof(St_ScheduleItemCount);  //171113,hpq,add for pro8  sizeof(St_SchTestObject) +

    if(p_Count->m_u16StepCount > IV_MAX_SCHEDULE_STEP_NUMBER \
            || p_Count->m_u16FormulaCount >  IV_MAX_SCHEDULE_FORMULA_NUMBER \
            || p_Count->m_u16CvCount >  IV_MAX_SCHEDULE_CV_NUMBER \
            || p_Count->m_u16PulseCount > IV_MAX_PULSE_BUFFER_NUMBER \
            || p_Count->m_u16TestSettingCount >  MAX_TEST_SETTING_COUNT \
            || p_Count->m_u16SMBBlockCount >  MAX_SMB_BLOCK_CONTENT_COUNT \
      )
    {
        return -1;
    }
    for (unsigned short StepIdx = 0; StepIdx < p_Count->m_u16StepCount; StepIdx++)              //Copy Step Data Segment
    {
        St_StepCtrl * p_Ctrl = &p_Buff->m_StepData[StepIdx].m_Ctrl;
        memcpy(&p_Buff->m_StepData[StepIdx],
               (void *)((unsigned long)pData + CopiedSize),
               sizeof(St_StepData));
        CopiedSize += sizeof(St_StepCtrl) + (p_Ctrl->m_ucStepLimitNo + p_Ctrl->m_ucLogLimitNo) * sizeof(St_Limit);
        if(p_Ctrl->m_ucCurrentRange > p_Buff->m_ucMacCurrentRange)
            p_Buff->m_ucMacCurrentRange = p_Ctrl->m_ucCurrentRange;

        if((p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_I)
                || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_V)
                || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_L)
                || (p_Ctrl->m_u16ControlType == CT_IVCH_SIMU_P))
        {
            SIMU_BUFFER::RegisterIV(sc_BufIdx, p_Ctrl->m_CtrlValues.m_MD5);
        }
    }

    //     for (int CvIdx = 0;CvIdx < p_Count->m_u16CvCount; CvIdx++)                       //Copy CV Data Segment
    //     {
    //         memcpy(&p_Buff->m_CvData[CvIdx],
    //             (void *)((unsigned long)pData + CopiedSize),
    //             sizeof(St_CvData));
    //         CopiedSize += sizeof(St_CvCtrl) + p_Buff->m_CvData[CvIdx].m_Ctrl.m_ucStageCount*sizeof(St_CvStage);
    //     }

    for (int FormulaIdx = 0; FormulaIdx < p_Count->m_u16FormulaCount; FormulaIdx++) //Copy Formula Data Segment
    {
        memcpy(&p_Buff->m_FormulaData[FormulaIdx],
               (void *)((unsigned long)pData + CopiedSize),
               sizeof(St_FormulaData));
        CopiedSize += sizeof(St_FormulaData);
    }
    for (int PulseIdx = 0; PulseIdx < p_Count->m_u16PulseCount; PulseIdx++) //Copy Pulse Data Segment
    {
        memcpy(&p_Buff->m_PulseData[PulseIdx],
               (void *)((unsigned long)pData + CopiedSize),
               sizeof(St_PulseData));
        CopiedSize += sizeof(St_PulseData);
    }

    unsigned char *uc_ElementNum = 0;
    unsigned int uint32_SizeLen = 0, ElementUnitSize = sizeof(St_TestSettingElement);
    for (int TestSettingIdx = 0; TestSettingIdx < p_Count->m_u16TestSettingCount; TestSettingIdx++) //Copy Pulse Data Segment
    {
        uc_ElementNum = (unsigned char *)((unsigned long)pData + CopiedSize);
        if(*uc_ElementNum > MAX_TEST_SETTING_ELEMENT_COUNT)
            return -1;
        uint32_SizeLen = (4 + ElementUnitSize * (*uc_ElementNum));
        memcpy(&p_Buff->m_TestSetting[TestSettingIdx],
               (void *)((unsigned long)pData + CopiedSize),
               uint32_SizeLen );
        CopiedSize += uint32_SizeLen;
    }

    for (int CvIdx = 0; CvIdx < p_Count->m_u16CvCount; CvIdx++)                      //Copy CV Data Segment
    {
        memcpy(&p_Buff->m_CvData[CvIdx],
               (void *)((unsigned long)pData + CopiedSize),
               sizeof(St_CvData));
        CopiedSize += sizeof(St_CvCtrl) + p_Buff->m_CvData[CvIdx].m_Ctrl.m_ucStageCount * sizeof(St_CvStage);
    }
    for(int SMBIdx = 0; SMBIdx < p_Count->m_u16SMBBlockCount; SMBIdx++)
    {
        memcpy(&p_Buff->m_SMBBlockData[SMBIdx],
               (void *)((unsigned long)pData + CopiedSize),
               sizeof(St_SMBBlockData));
        CopiedSize += sizeof(St_SMBBlockData) ;

    }

    if((p_Count->m_u16AdvFormulaCount & 0xff) == ((p_Count->m_u16AdvFormulaCount>>8) &0xff))	//高字节 == 低字节
    {
        for (int FormulaIdx = 0; FormulaIdx < (p_Count->m_u16AdvFormulaCount& 0xff); FormulaIdx++) //Copy Adv Formula Data Segment
        {
            memcpy(&p_Buff->m_AdvFormula[FormulaIdx],
                   (void *)((unsigned long)pData + CopiedSize),
                   sizeof(St_AdvFormula));
            CopiedSize += sizeof(St_AdvFormula);
        }
    }



    //set unassigend testsetting Idx  (last step Testsetting Idx)
    short int16_lastTestSettingIdx = -1;
    for(unsigned short uw16_i = 0; uw16_i < p_Count->m_u16StepCount; uw16_i++)
    {
        if(p_Buff->m_StepData[uw16_i].m_Ctrl.m_ucTestSettingIdx >= 0)
        {
            int16_lastTestSettingIdx = p_Buff->m_StepData[uw16_i].m_Ctrl.m_ucTestSettingIdx;
            continue;
        }
        else
            p_Buff->m_StepData[uw16_i].m_Ctrl.m_ucTestSettingIdx = int16_lastTestSettingIdx;
        int16_lastTestSettingIdx = p_Buff->m_StepData[uw16_i].m_Ctrl.m_ucTestSettingIdx;
    }
    p_Buff->m_bValid = true;
    p_Buff->m_u32IvRegister = 0;    //没有IV通道被Assign到此Schedule Buffer上

    for (unsigned short ChannelIdx = 0; ChannelIdx < BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        if(!memcmp((void *)&ChannelSchedule[ChannelIdx].m_ID, (void *)&ScheduleID, 16))
        {
            SetBit_32Bit(p_Buff->m_u32IvRegister, ChannelIdx);
        }
    }
    //插入特殊的Step步用于IR异常判断

    unsigned short m_LastStepID;
    for(unsigned short uw16_i = 0; uw16_i < p_Count->m_u16StepCount; uw16_i++)
        p_Buff->m_StepData[uw16_i].m_bIRTestStep = 0; //将以前步的标记强行清0

    m_LastStepID = p_Count->m_u16StepCount - 1;
    if(!p_Buff->m_StepData[m_LastStepID].m_bIRTestStep)   //最后一步不是IR测试步的话   Di 20140617
    {
        m_LastStepID = p_Count->m_u16StepCount;
        //插入特殊步Step处理
        p_Buff->m_StepData[m_LastStepID].m_Ctrl.m_u16ControlType = CT_SPECIAL_IRTEST;//SPECIAL_IRTEST; //zyx 2105291
        p_Buff->m_StepData[m_LastStepID].m_Ctrl.m_u16StepID = m_LastStepID + 1;
        p_Buff->m_StepData[m_LastStepID].m_bIRTestStep = 1;
        p_Buff->m_StepData[m_LastStepID].m_Ctrl.m_ucStepLimitNo = 1;
        p_Buff->m_StepData[m_LastStepID].m_Ctrl.m_fMaxCurrent = 0;
        p_Buff->m_StepData[m_LastStepID].m_Ctrl.m_CtrlValues.m_fControlValue[0]=  PERCRATE*2;//0.1C
    }
    return 1;     //schedule 成功搬到BUFFERS里
}
bool ChannelInfo_Manager::bCanDeleteSchBuf(unsigned long SchBufs)
{
    signed char SchBufIdx;   //  20140611 he  修改Simulation assign的BUG。

    for(SchBufIdx = 0; SchBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER; SchBufIdx++)
    {
        if(CheckBit_32Bit(SchBufs, SchBufIdx))
        {
            if(ScheduleBuf[SchBufIdx].m_bValid && (ScheduleBuf[SchBufIdx].m_u32IvRegister != 0))  //合法，而且 有IV通道在使用，不能删除
                return 0;
        }
    }

    return 1;
}
void ChannelInfo_Manager::RegisterSimu2Schedule(St_MD5 MD5)
{
    signed char SchBufIdx;   //  20140611 he  修改Simulation assign的BUG。

    for(SchBufIdx = 0; SchBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER; SchBufIdx++)
    {
        if(ScheduleBuf[SchBufIdx].m_bValid)
        {
            for (int StepIdx = 0; StepIdx < ScheduleBuf[SchBufIdx].m_ItemCount.m_u16StepCount; StepIdx++)              //Copy Step Data Segment
            {
                if(((ScheduleBuf[SchBufIdx].m_StepData[StepIdx].m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_I)
                        || (ScheduleBuf[SchBufIdx].m_StepData[StepIdx].m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_V)
                        || (ScheduleBuf[SchBufIdx].m_StepData[StepIdx].m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_L)
                        || (ScheduleBuf[SchBufIdx].m_StepData[StepIdx].m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_P))
                        && (memcmp(&ScheduleBuf[SchBufIdx].m_StepData[StepIdx].m_Ctrl.m_CtrlValues.m_MD5, &MD5, 16) == 0))
                {
                    SIMU_BUFFER::RegisterIV(SchBufIdx, MD5);
                }
            }
        }
    }
}
int ChannelInfo_Manager::InsertPulse2Buf(St_MD5 PulseID, void *pData)
{
    signed char sc_BufIdx;

    sc_BufIdx = FindPulseBuf(PulseID);
    if(sc_BufIdx != -1)
        return 0;      //标示 这个Schedule MD5已经存在了，无需再加入到BUFFER里了；

    sc_BufIdx = GetEmptyPulseBuf();
    if(sc_BufIdx == -1)
        return -1;    //标示 schedule buffer满了，无法再插入新的Schedule，当然，这种情况不会出现；

    PulseBuf[sc_BufIdx].m_ID = PulseID;
    memcpy(&PulseBuf[sc_BufIdx].m_Data,
           pData,
           sizeof(St_PulseData));

    PulseBuf[sc_BufIdx].m_bValid = true;
    PulseBuf[sc_BufIdx].m_u32IvRegister = 0;

    return 1;     //pulse 成功搬到BUFFERS里
}
unsigned char ChannelInfo_Manager::RegisterSchedule(St_Assign * AssignMsg)//St_MD5 MD5)
{
    signed char sc_BufIdx;
    unsigned char uc_ChannelNo = AssignMsg->m_u16ChannelIdx;
    if (uc_ChannelNo >= BoardCFG.m_Global.m_u16IVChannelCount)
        return 0;

    St_MD5 OldMd5 = ChannelSchedule[uc_ChannelNo].m_ID;   //旧的MD5保存下来

    if(memcmp((void *)&ChannelSchedule[uc_ChannelNo].m_ID, (void *)&AssignMsg->m_MD5Assign[0].m_MD5, 16) != 0)
    {   //if a new schedule assigned to channel, Try delete old schedule firstly
        UnregisterSchedule(uc_ChannelNo);
        memset((void *)&ChannelSchedule[uc_ChannelNo].m_ID, 0, 16);
        ChannelSchedule[uc_ChannelNo].m_scBufIdx = -1;
        TryDeleteSchedule(uc_ChannelNo, OldMd5);
    }

    sc_BufIdx = FindScheduleBuf(AssignMsg->m_MD5Assign[0].m_MD5);
    if (sc_BufIdx == -1)
    {
        //ChannelSchedule[uc_ChannelNo].m_scBufIdx = -1;  //表示
        ChannelSchedule[uc_ChannelNo].m_ID = AssignMsg->m_MD5Assign[0].m_MD5;
        return 0;
    }
    else
    {
        ChannelSchedule[uc_ChannelNo].m_ID = AssignMsg->m_MD5Assign[0].m_MD5;
        ChannelSchedule[uc_ChannelNo].m_scBufIdx = sc_BufIdx;
        SetBit_32Bit(ScheduleBuf[sc_BufIdx].m_u32IvRegister, uc_ChannelNo);
        memcpy(&ChannelSchedule[uc_ChannelNo].m_Settings,
               &ScheduleBuf[sc_BufIdx].m_Setting,
               sizeof(St_ScheduleSetting));
// 		memcpy(&ChannelSchedule[uc_ChannelNo].m_TestObject, //1804201 zyx
// 			&ScheduleBuf[sc_BufIdx].m_TestObject,
// 			sizeof(St_SchTestObject));
        memcpy(&ChannelSchedule[uc_ChannelNo].m_ItemCount,
               &ScheduleBuf[sc_BufIdx].m_ItemCount,
               sizeof(St_ScheduleItemCount));
        if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
        {
            if( ChannelSchedule[uc_ChannelNo].m_Settings.m_PowerAdjPara.IsCheckPowerAdj)
            {
                PowerAdj::SchLoadPara[uc_ChannelNo].IsResistorFlag =true;
                PowerAdj::SchLoadPara[uc_ChannelNo]. m_ResistorValue=ChannelSchedule[uc_ChannelNo].m_Settings.m_PowerAdjPara.m_ResistorValue;
// 						PowerAdj::m_PowerAdjStatus = POWERADJ_REQUIRE;
            }
            else
            {
                PowerAdj::SchLoadPara[uc_ChannelNo].IsResistorFlag =false;
            }
            if(PowerAdj::m_PowerAdjStatus>POWERADJ_WAIT)
                PowerAdj::m_PowerAdjStatus = POWERADJ_REQUIRE;
        }
        return 1;
    }
}

void ChannelInfo_Manager::UnregisterSchedule(unsigned char uc_ChannelNo)
{
    //    signed char sc_SchBufIdx,sc_SimuBufIdx,sc_PulseBufIdx;
    signed char sc_SchBufIdx, sc_PulseBufIdx;

    if (ChannelSchedule[uc_ChannelNo].m_scBufIdx == -1)
        return ;   //本身就没有schedule，无需进一步处理。

    sc_SchBufIdx = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return ;   //本身就没有schedule，无需进一步处理。

    for (int i = 0; i < ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount; i++)
    {
        switch(ScheduleBuf[sc_SchBufIdx].m_StepData[i].m_Ctrl.m_u16ControlType)
        {
        case CT_IVCH_CPULSE:
        case CT_IVCH_VPULSE:
            sc_PulseBufIdx = FindPulseBuf(ScheduleBuf[sc_SchBufIdx].m_StepData[i].m_Ctrl.m_CtrlValues.m_MD5);
            if (sc_PulseBufIdx != -1)
            {
                ClrBit_32Bit(PulseBuf[sc_PulseBufIdx].m_u32IvRegister, uc_ChannelNo);
                if (PulseBuf[sc_PulseBufIdx].m_u32IvRegister == 0)
                {
                    PulseBuf[sc_PulseBufIdx].m_AbandonTime = CommonFunc::GetSystemTime();
//                     PulseBuf[sc_PulseBufIdx].m_AbandonTime.Second = OS_ServeProcess::OS_Time.Second;
//                     PulseBuf[sc_PulseBufIdx].m_AbandonTime.Us100 = OS_ServeProcess::OS_Time.Us100;
                }
            }
            break;
        default:
            break;
        }
    }

    ClrBit_32Bit(ScheduleBuf[sc_SchBufIdx].m_u32IvRegister, uc_ChannelNo);
    if (ScheduleBuf[sc_SchBufIdx].m_u32IvRegister == 0)
    {
        ScheduleBuf[sc_SchBufIdx].m_AbandonTime = CommonFunc::GetSystemTime();
//         ScheduleBuf[sc_SchBufIdx].m_AbandonTime.Second = OS_ServeProcess::OS_Time.Second;
//         ScheduleBuf[sc_SchBufIdx].m_AbandonTime.Us100 = OS_ServeProcess::OS_Time.Us100;
    }

    memset((void *)&ChannelSchedule[uc_ChannelNo].m_ID, 0, 16);
    ChannelSchedule[uc_ChannelNo].m_scBufIdx = -1;

// 	return 1;
}

void ChannelInfo_Manager::TryDeleteSchedule(unsigned char uc_ChannelNo, St_MD5 MD5)
{
    signed char sc_SchBufIdx;

    sc_SchBufIdx = FindScheduleBuf(MD5);
    if(sc_SchBufIdx == -1)
        return ;

    ClrBit_32Bit(ScheduleBuf[sc_SchBufIdx].m_u32IvRegister, uc_ChannelNo);

    if(ScheduleBuf[sc_SchBufIdx].m_u32IvRegister == 0)
    {
        ScheduleBuf[sc_SchBufIdx].m_AbandonTime = CommonFunc::GetSystemTime();
//         ScheduleBuf[sc_SchBufIdx].m_AbandonTime.Second = OS_ServeProcess::OS_Time.Second;
//         ScheduleBuf[sc_SchBufIdx].m_AbandonTime.Us100 = OS_ServeProcess::OS_Time.Us100;
    }

// 	return 1;
}
// int ChannelInfo_Manager::CheckScheduleMD5(unsigned char uc_ChannelNo,St_MD5 MD5)
// {
// 	return 1;
// }

ST_RANGE ChannelInfo_Manager::ClampPower(unsigned char uc_ChannelNo)	//170609 zyx
{
    ST_RANGE fPclamp;
    fPclamp.High = 0.0f;
    fPclamp.Low = 0.0f;

    float fVoltage = MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;

    fPclamp.High = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHighControlType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampHigh) / fVoltage;
    fPclamp.Low = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLowControlType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_PowerClampRange.clampLow) / fVoltage;
    if(fPclamp.High < fPclamp.Low)
    {
        float tmp = fPclamp.High;
        fPclamp.High = fPclamp.Low;
        fPclamp.Low = tmp;
    }

    return fPclamp;
}

ST_RANGE ChannelInfo_Manager::ClampCurrent(unsigned char uc_ChannelNo)	//170719 zyx
{
    ST_RANGE fIchamp;
    fIchamp.High = 0.0f;
    fIchamp.Low = 0.0f;

    fIchamp.High = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHighControlType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampHigh);
    fIchamp.Low = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLowControlType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,ChannelInfo[uc_ChannelNo].m_CurrentClampRange.clampLow);
    if(fIchamp.High < fIchamp.Low)
    {
        float tmp = fIchamp.High;
        fIchamp.High = fIchamp.Low;
        fIchamp.Low = tmp;
    }

    return fIchamp;
}

ST_RANGE ChannelInfo_Manager::ClampDigiVolt(unsigned char uc_ChannelNo, unsigned short u16_ControlType)	//171016 hpq
{
    ST_RANGE fVchamp;
// 	fVchamp.High = 0.0f;
// 	fVchamp.Low = 0.0f;

    if( u16_ControlType == CT_IVCH_SIMU_I)
    {
        fVchamp.High = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampHigh);
        fVchamp.Low = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_fCurrentSimulationVclampLow);
    }
    else if(u16_ControlType == CT_IVCH_SIMU_P)
    {
        fVchamp.High = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_fPowerSimulationVclampHigh);
        fVchamp.Low = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, ChannelInfo[uc_ChannelNo].m_DataType_DigiVclampLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, ChannelInfo[uc_ChannelNo].m_fPowerSimulationVclampLow);
    }
//     if(fVchamp.High < fVchamp.Low)
//     {
//         float tmp = fVchamp.High;
//         fVchamp.High = fVchamp.Low;
//         fVchamp.Low = tmp;
//     }

    return fVchamp;
}

float ChannelInfo_Manager::CurrentCheck(unsigned char uc_ChannelNo, float f_SetCurrent)
{
    char IsClamp = 0;
    ST_RANGE PowerRange;
    ST_RANGE CurrentRange;
    ST_RANGE Range;
    memset(&PowerRange,0,sizeof(ST_RANGE));
    memset(&CurrentRange,0,sizeof(ST_RANGE));
    memset(&Range,0,sizeof(ST_RANGE));
    bool IsSetPowerClamp = ChannelInfo[uc_ChannelNo].m_PowerClampRange.IsOpenClampRange;
    bool IsSetCurrentClamp = ChannelInfo[uc_ChannelNo].m_CurrentClampRange.IsOpenClampRange;
    if(!(IsSetCurrentClamp || IsSetPowerClamp))
        return f_SetCurrent;

    if(IsSetPowerClamp && (!IsSetCurrentClamp))
    {
        Range = ClampPower(uc_ChannelNo);
        IsClamp = 1;
    }
    else if((!IsSetPowerClamp) && IsSetCurrentClamp)
    {
        Range = ClampCurrent(uc_ChannelNo);
        IsClamp = 2;
    }
    else
    {
        PowerRange = ClampPower(uc_ChannelNo);
        CurrentRange = ClampCurrent(uc_ChannelNo);
        if(PowerRange.High >= CurrentRange.High)
        {
            Range.High = CurrentRange.High;
            IsClamp = 2;
        }
        else
        {
            Range.High = PowerRange.High;
            IsClamp = 1;
        }

        if(PowerRange.Low <= CurrentRange.Low)
        {
            Range.Low = CurrentRange.Low;
            IsClamp = 2;
        }
        else
        {
            Range.Low = PowerRange.Low;
            IsClamp = 1;
        }

        if(Range.Low > Range.High)
        {
            Range = PowerRange;
            IsClamp = 1;
        }
    }
    if(f_SetCurrent > Range.High)
    {
        f_SetCurrent = Range.High;
        switch(IsClamp)
        {
        case 1:
            Is_Power_SoftWareClamp_Work[uc_ChannelNo] = true;
            break;
        case 2:
            Is_Current_SoftWareClamp_Work[uc_ChannelNo] = true;
            break;
        default:
            break;
        }
    }
    else if(f_SetCurrent < Range.Low)
    {
        f_SetCurrent = Range.Low;
        switch(IsClamp)
        {
        case 1:
            Is_Power_SoftWareClamp_Work[uc_ChannelNo] = true;
            break;
        case 2:
            Is_Current_SoftWareClamp_Work[uc_ChannelNo] = true;
            break;
        default:
            break;
        }
    }
    else
    {
        Is_Current_SoftWareClamp_Work[uc_ChannelNo] = false;
        Is_Power_SoftWareClamp_Work[uc_ChannelNo] = false;
    }

    return f_SetCurrent;
}
unsigned char ChannelInfo_Manager::Clamp2AMP(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Amp)
{
    unsigned char unsafe = 0;
    float m_fHigh, m_fLow;
    St_ScheduleSetting	*		p_Setting		=		&ChannelSchedule[uc_ChannelNo].m_Settings;
    St_SchTestObject    *   p_TestObj   =   &ChannelSchedule[uc_ChannelNo].m_TestObject;
    St_ChannelCFG				*		p_ChnCFG		=		&BoardCFG.m_Channel[uc_ChannelNo];
// 	if(p_Setting->m_SafetyCurrent.m_bEnabled)
// 	{

    if(p_Setting->m_SafetyCurrent.m_nMaxType  == 1)
    {
        m_fHigh = p_Setting->m_SafetyCurrent.m_fMax * p_TestObj->m_fMaxCurrentCharge / 100;
        for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
        {
            if(uc_ChannelNo == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
            {
                m_fHigh = m_fHigh * ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
                m_fLow = m_fLow * ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
                break;
            }
        }
    }
    else
        m_fHigh = p_Setting->m_SafetyCurrent.m_fMax;

    if(p_Setting->m_SafetyCurrent.m_nMinType  == 1) //180726 zyx
    {
        m_fLow = p_Setting->m_SafetyCurrent.m_fMin * p_TestObj->m_fMaxCurrentCharge / 100;
        for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
        {
            if(uc_ChannelNo == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
            {
                m_fLow = m_fLow * ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
                break;
            }
        }
    }
    else
        m_fLow = p_Setting->m_SafetyCurrent.m_fMin;

    m_fHigh = (m_fHigh <= p_ChnCFG->m_IRange[uc_Range].m_Bound.m_High) ? m_fHigh : p_ChnCFG->m_IRange[uc_Range].m_Bound.m_High;
    m_fLow = (m_fLow >= p_ChnCFG->m_IRange[uc_Range].m_Bound.m_Low) ? m_fLow : p_ChnCFG->m_IRange[uc_Range].m_Bound.m_Low;


    if(2 * f_Amp > (m_fHigh - m_fLow) * 9 / 10)
    {
        safety_check::SetUnsafe(uc_ChannelNo, AMP_IR_UNSAFE, f_Amp, IV_UNSAFE_TYPE); //Unsafe报警
        safety_check::sendError(uc_ChannelNo);		// trigger a sending
// 			safety_check::ClrUnsafe(uc_ChannelNo, AMP_IR_UNSAFE, IV_UNSAFE_TYPE );
        unsafe = 1;
    }
// 	}
    return unsafe;
}
float ChannelInfo_Manager::ClampIRIH(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Current)
{
    if(f_Current > ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_IRange[uc_Range].m_Bound.m_High)
        f_Current = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_IRange[uc_Range].m_Bound.m_High;
    if(ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_fVpp == 0)
        if(f_Current > 0)
            f_Current = 0;
    return f_Current;
}

float ChannelInfo_Manager::ClampIRIL(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Current)
{
    if(f_Current < ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_IRange[uc_Range].m_Bound.m_Low)
        f_Current = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_IRange[uc_Range].m_Bound.m_Low;
    if(ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_fVmm == 0)
        if(f_Current < 0)
            f_Current = 0;
    return f_Current;
}
#define CalibreateADC( code, Gain, Offset) ((code) < (CONVERTER_MAX)?((code) * (Gain) + (Offset)):((CONVERTER_MAX)*(Gain)+(Offset)))
//和ADC DAC校准相关的 public方法集合:
float ChannelInfo_Manager::CalibrateCurrentADC(unsigned char uc_ChannelNo, unsigned char range, unsigned long code)
{
// 		 if(Step_Manager_task::Unipolar.BoardFlag)
// 		 {
// 	        if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucIRangeCount)
// 		         range = CURRENT_RANGE_3;
// 		 }
// 		 else
// 		 {
    if(range >= board_config::DacInfo.m_bCurrRngCount)
        range = board_config::DacInfo.m_bCurrRngCount - 1;  //170320,modified by hepeiqing

    St_RangeCali* ADC = &BoardCFG.m_Channel[uc_ChannelNo].m_IRange[range].m_Adc;
    float f_Current = CalibreateADC( code, ADC->m_Gain, ADC->m_Offset);
    return f_Current;
}
unsigned long ChannelInfo_Manager::CalibrateCurrentDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Current)
{
// 	 if(Step_Manager_task::Unipolar.BoardFlag)
// 	 {
// 			if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucIRangeCount)    //chen add
// 				range = CURRENT_RANGE_1;
// 	 }
// 	 else
// 	 {
    if(range >= board_config::DacInfo.m_bCurrRngCount)
        range = board_config::DacInfo.m_bCurrRngCount - 1;  //170320,modified by hepeiqing
//    }

    if(f_Current > GetCurrRangeHighOffsetValue(uc_ChannelNo,range, 1.05)) //170321,add by hepeiqing,for U.S.A.'s request
        f_Current = GetCurrRangeHighOffsetValue(uc_ChannelNo,range, 1.05);
    else if(f_Current < GetCurrRangeLowOffsetValue(uc_ChannelNo,range, 1.05))
        f_Current = GetCurrRangeLowOffsetValue(uc_ChannelNo,range, 1.05);

#if HIGH_RNG_DISCHARGE_MODE == 1
    if((range == ChannnelInfo_Manager::CurrRngPhysicalRange1) && (f_Current > 0.0f))
        f_Current = 0.0f;
#endif
    float f_gain, f_offset;
    unsigned long u32_Current;

//     if(Step_Manager_task::Unipolar.BoardFlag)    //  20161025
// 	{
//         u32_Current = ChannelInfo_Manager::Unipolar_CalibrateCurrentDAC(uc_ChannelNo,range,f_Current);
//         return u32_Current;
// 	}

    f_gain = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[range].m_Dac.m_Gain;
    f_offset = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[range].m_Dac.m_Offset;

    f_Current = f_Current * f_gain + f_offset;
    u32_Current = f_Current;

    if(u32_Current > CONVERTER_MAX)
        u32_Current = CONVERTER_MAX;

    return u32_Current;
}
float ChannelInfo_Manager::CalibrateVoltageADC(unsigned char uc_ChannelNo, unsigned char range, unsigned long code)
{
    if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucVRangeCount)
        range = VOLTAGE_RANGE_HIGH;     //如果超出了量程，修改量程为最低量程，这样有利于更灵敏的保护
    //强制保护  chen20130906
// #if defined _TESLA_PWM_BOARD_TYPE_
// 	range = VOLTAGE_RANGE_HIGH;
// #endif


    St_RangeCali* ADC = &BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_Adc;
    double f_Voltage = CalibreateADC( code, ADC->m_Gain, ADC->m_Offset);

    return f_Voltage;
}
unsigned long ChannelInfo_Manager::CalibrateVoltageDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage)
{
    if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucVRangeCount)
        range = VOLTAGE_RANGE_HIGH;       //超出量程数，

    float f_gain, f_offset;
    unsigned long u32_Voltage;
    f_gain = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_Dac.m_Gain;
    f_offset = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_Dac.m_Offset;

    f_Voltage = f_Voltage * f_gain + f_offset;

// 	if(Step_Manager_task::Unipolar.BoardFlag)   //  20161025
// 	{
//         Unipolar_Convert(&f_Voltage,1);
// 	}

    u32_Voltage = f_Voltage;

    if(u32_Voltage > CONVERTER_MAX)
        u32_Voltage = CONVERTER_MAX;

    return u32_Voltage;
}

float ChannelInfo_Manager::ChannelDirection(unsigned char uc_ChannelNo,float Sys_Voltage, float Channel_Voltage)
{
    float V1,V2;
    float u32_DifferVoltage;
    V1=Sys_Voltage;
    V2=CalibrateVoltageADC(uc_ChannelNo, ChannelInfo_Manager::ChannelInfo[0].m_ucVoltageRange,Channel_Voltage);
//	u32_DifferVoltage=(V1-V2);
    u32_DifferVoltage=::fabs(V1-V2);
    return u32_DifferVoltage;

}

unsigned long ChannelInfo_Manager::CalibrateClampDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage)
{
    if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucVRangeCount)
        range = VOLTAGE_RANGE_HIGH;

    float f_gain, f_offset;
    unsigned long u32_Voltage;
    f_gain = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_ClampDac.m_Gain;
    f_offset = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_ClampDac.m_Offset;

    f_Voltage = f_Voltage * f_gain + f_offset;

//     if(Step_Manager_task::Unipolar.BoardFlag)   //  20161025
// 	{
// 	    Unipolar_Convert(&f_Voltage,1);
// 	}
//
    u32_Voltage = f_Voltage;

    if(u32_Voltage > CONVERTER_MAX)
        u32_Voltage = CONVERTER_MAX;
    return u32_Voltage;
}
unsigned long ChannelInfo_Manager::CalibrateLowClampDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage)
{
    if(range >= BoardCFG.m_Channel[uc_ChannelNo].m_ucVRangeCount)
        range = VOLTAGE_RANGE_HIGH;

    float f_gain, f_offset;
    unsigned long u32_Voltage;
    f_gain = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_LowClampDac.m_Gain;
    f_offset = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[range].m_LowClampDac.m_Offset;

//     if(Step_Manager_task::Unipolar.BoardFlag)   //  20161025
// 	{
// 	    if(f_Voltage < 0.0)
// 		    f_Voltage = 0.0;
    f_Voltage = f_Voltage * f_gain + f_offset;
//         Unipolar_Convert(&f_Voltage,1);
// 	}
//     else
// 	    f_Voltage = f_Voltage*f_gain + f_offset;


    u32_Voltage = f_Voltage;

    if(u32_Voltage > CONVERTER_MAX)
        u32_Voltage = CONVERTER_MAX;
    return u32_Voltage;
}
void ChannelInfo_Manager::SaveCalibratePara(unsigned char WriteType, unsigned short ChannelIdx, unsigned char range)
{
    St_FramMessage myMessage;

    myMessage.m_wChannelBitIdx = 0;
    myMessage.m_wChannelBitIdx |= 0x01 << ChannelIdx;
    myMessage.m_bIsWrite = 1;
    myMessage.m_bPID = 0;
    switch(WriteType)
    {
    case WRITE_CURRENT:
        if(range >= 8)
            return;
        myMessage.m_u16Length = 16;
        myMessage.m_wFramType = WRITE_CURRENT;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_IRange[range].m_Adc;
        break;
    case WRITE_VOLTAGE:
        if (range >= 4)
            return;
        myMessage.m_u16Length = 16;
        myMessage.m_wFramType = WRITE_VOLTAGE;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_Adc;
        break;
    case WRITE_BUILDINVOLT:
        myMessage.m_u16Length = sizeof(St_RangeCali);
        myMessage.m_wFramType = WRITE_BUILDINVOLT;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_BuildInV[range];
        break;
    case WRITE_ACR:
        myMessage.m_u16Length = 8;
        myMessage.m_wFramType = WRITE_ACR;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_ACR_R;
        break;
    case WRITE_CLAMPVOLH:
        if(range >= BoardCFG.m_Channel[ChannelIdx].m_ucVRangeCount)
            range = VOLTAGE_RANGE_HIGH;
        myMessage.m_u16Length = 8;
        myMessage.m_wFramType = WRITE_CLAMPVOLH;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_ClampDac;
        break;
    case WRITE_CLAMPVOLL:
        if(range >= BoardCFG.m_Channel[ChannelIdx].m_ucVRangeCount)
            range = VOLTAGE_RANGE_HIGH;
        myMessage.m_u16Length = 8;
        myMessage.m_wFramType = WRITE_CLAMPVOLL;
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_LowClampDac;
        break;
    default:
        break;
    }

    CConfig::PushFramMessage(myMessage);
}
/*
void ChannelInfo_Manager::SaveCalibrateCurrentPara(unsigned short ChannelIdx,unsigned char range)
{
	if(range >= 8)
		return;

	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = 16;
	myMessage.m_wFramType = WRITE_CURRENT;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_IRange[range].m_Adc;
	CConfig::PushFramMessage(myMessage);
}
void ChannelInfo_Manager::SaveCalibrateVoltagePara(unsigned short ChannelIdx,unsigned char range)
{
	if (range >= 4)
		return;

	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = 16;
	myMessage.m_wFramType = WRITE_VOLTAGE;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_Adc;
	CConfig::PushFramMessage(myMessage);
}
void ChannelInfo_Manager::SaveCalibrateBuildInVoltagePara(unsigned short ChannelIdx,unsigned char AuxIdx)
{
	if (AuxIdx >= MAX_BUILDIN_2V_COUNT)
		return;

	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = sizeof(St_RangeCali);
	myMessage.m_wFramType = WRITE_BUILDINVOLT;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_BuildInV[AuxIdx];
	CConfig::PushFramMessage(myMessage);
}
void ChannelInfo_Manager::SaveCalibrateACRPara(unsigned short ChannelIdx)
{
	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = 8;
	myMessage.m_wFramType = WRITE_ACR;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_ACR_R;
	CConfig::PushFramMessage(myMessage);
}

void ChannelInfo_Manager::SaveCalibrateClampVoltagePara(unsigned short ChannelIdx,unsigned char range)
{
	if(range >= BoardCFG.m_Channel[ChannelIdx].m_ucVRangeCount)
		range = VOLTAGE_RANGE_HIGH;

	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = 8;
	myMessage.m_wFramType = WRITE_CLAMPVOLH;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_ClampDac;

	CConfig::PushFramMessage(myMessage);
}
void ChannelInfo_Manager::SaveCalibrateLowClampVoltagePara(unsigned short ChannelIdx,unsigned char range)
{
	if(range >= BoardCFG.m_Channel[ChannelIdx].m_ucVRangeCount)
		range = VOLTAGE_RANGE_HIGH;

	St_FramMessage myMessage;
	myMessage.m_wChannelBitIdx = 0;
	myMessage.m_wChannelBitIdx |= 0x01<<ChannelIdx;
	myMessage.m_bIsWrite = 1;
	myMessage.m_bPID = 0;
	myMessage.m_u16Length = 8;
	myMessage.m_wFramType = WRITE_CLAMPVOLL;
	myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIdx].m_VRange[range].m_LowClampDac;

	CConfig::PushFramMessage(myMessage);
}*/
void ChannelInfo_Manager::SavePidPara(unsigned short ChannelCount, unsigned char * pChannelIdx)
{
    St_FramMessage myMessage;
    myMessage.m_wChannelCount = ChannelCount;
    myMessage.m_wChannelBitIdx = pChannelIdx[0];
    myMessage.m_wChannelBitIdx |= pChannelIdx[1] << 8;
    for(unsigned short ChannelIndex = 0; ChannelIndex < MAXCHANNELNO; ChannelIndex++)
    {
        if(MYBITTRUE(pChannelIdx, ChannelIndex))
        {
            myMessage.m_bIsWrite = 1;
            myMessage.m_bPID = 1;
            myMessage.m_u16ChannelIdx = ChannelIndex;
            myMessage.m_u16Length = sizeof(St_IV_ChannelPID);
            myMessage.m_StartAddress = (unsigned long)&ChannelPID[ChannelIndex];
            myMessage.m_wFramType = WRITE_PID;
            CConfig::PushFramMessage(myMessage);
        }
    }

}
void ChannelInfo_Manager::SaveConfigPara(unsigned short ChannelCount, unsigned char * pChannelIdx)
{
//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
        Channel_MapAuxInfo::Init_AUXInfo();
//#endif
    St_FramMessage myMessage;
    myMessage.m_wChannelCount = ChannelCount;
    myMessage.m_wChannelBitIdx = pChannelIdx[0];
    myMessage.m_wChannelBitIdx |= pChannelIdx[1] << 8;
    if(BoardCFG.m_Global.m_uBuildInVoltCount > MAX_BUILDIN_2V_COUNT)
        BoardCFG.m_Global.m_uBuildInVoltCount = MAX_BUILDIN_2V_COUNT;
    if (ChannelCount == BoardCFG.m_Global.m_u16IVChannelCount)
    {
        myMessage.m_bIsWrite = 1;
        myMessage.m_bPID = 0;
        myMessage.m_u16Length = sizeof(St_BoardGlobalCFG);
        myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Global;
        myMessage.m_wFramType = WRITE_CONFIG;
        CConfig::PushFramMessage(myMessage);
    }

    for(unsigned short ChannelIndex = 0; ChannelIndex < MAXCHANNELNO; ChannelIndex++)
    {
        St_ChannelCFG * p_ChnCFG = &BoardCFG.m_Channel[ChannelIndex];
        st_GetMetaValue * p_MV2 = &MetaVariable2[ChannelIndex][0];
        p_MV2[MetaCode_MV_IHmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_1)].m_Bound.m_Low;
        p_MV2[MetaCode_MV_IHmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_1)].m_Bound.m_High;
        p_MV2[MetaCode_MV_IMmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_2)].m_Bound.m_Low;
        p_MV2[MetaCode_MV_IMmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_2)].m_Bound.m_High;
        p_MV2[MetaCode_MV_ILmin].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_3)].m_Bound.m_Low;
        p_MV2[MetaCode_MV_ILmax].m_fValue = p_ChnCFG->m_IRange[GetPhysicalCurrentRange(CURRENT_RANGE_3)].m_Bound.m_High;
        p_MV2[MetaCode_MV_VHmin].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_Low;
        p_MV2[MetaCode_MV_VHmax].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
        p_MV2[MetaCode_MV_VLmin].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_LOW].m_Bound.m_Low;
        p_MV2[MetaCode_MV_VLmax].m_fValue = p_ChnCFG->m_VRange[VOLTAGE_RANGE_LOW].m_Bound.m_High;

        if(MYBITTRUE(pChannelIdx, ChannelIndex))
        {
            myMessage.m_bIsWrite = 1;
            myMessage.m_bPID = 0;
            myMessage.m_u16Length = sizeof(St_ChannelCFG);
            myMessage.m_wFramType = WRITE_CONFIG;
            myMessage.m_StartAddress = (unsigned long)&BoardCFG.m_Channel[ChannelIndex];
            CConfig::PushFramMessage(myMessage);
        }
    }
}
void ChannelInfo_Manager::GetSystemSafty(unsigned short ChannelIdx, unsigned char SaftyType, unsigned char Range, St_SaftyCheckItem * MySafty)
{
    St_ChannelCFG * p_ChnCFG = &BoardCFG.m_Channel[ChannelIdx];
    switch(SaftyType)
    {
    case MP_SYS_SAFTY_TYPE_CURRENT:
        if(p_ChnCFG->m_fCurrentSaftyLimitHigh > p_ChnCFG->m_fCurrentSaftyLimitLow) //safety有效
        {
            MySafty->m_fHigh = p_ChnCFG->m_fCurrentSaftyLimitHigh;
            MySafty->m_fLow = p_ChnCFG->m_fCurrentSaftyLimitLow;
//             for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
//             {
//                 if(ChannelIdx == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
//                 {
//                    MySafty->m_fHigh = p_ChnCFG->m_fCurrentSaftyLimitHigh*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                    MySafty->m_fLow = p_ChnCFG->m_fCurrentSaftyLimitLow*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                    break;
//                 }
//             }
        }
        else
        {
            if(Range >= CURRENT_RANGE_COUNT)
                Range = ChannelInfo_Manager::CurrRngPhysicalRange1;
            MySafty->m_fHigh = GetCurrRangeHighOffsetValue(ChannelIdx,Range, SAFTY_LOOSEN_FACTOR);
            MySafty->m_fLow = GetCurrRangeLowOffsetValue(ChannelIdx,Range, SAFTY_LOOSEN_FACTOR);
//                         for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
//             {
//                 if(ChannelIdx == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
//                 {
//                     MySafty->m_fHigh=MySafty->m_fHigh*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                    MySafty->m_fLow = MySafty->m_fLow*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                     break;
//                 }
//             }
        }
        break;
    case MP_SYS_SAFTY_TYPE_VOLTAGE:
        if(p_ChnCFG->m_fVpp > p_ChnCFG->m_fVmm) //safety有效
        {
            MySafty->m_fHigh = p_ChnCFG->m_fVpp;
            MySafty->m_fLow = p_ChnCFG->m_fVmm;
        }
        else
        {
            if(Range > VOLTAGE_RANGE_HIGH)
                Range = VOLTAGE_RANGE_HIGH;
            MySafty->m_fHigh = p_ChnCFG->m_VRange[Range].m_Bound.m_High * SAFTY_LOOSEN_FACTOR;
            MySafty->m_fLow = p_ChnCFG->m_VRange[Range].m_Bound.m_Low * SAFTY_LOOSEN_FACTOR;
        }
        break;
    case MP_SYS_SAFTY_TYPE_POWER:
        if(p_ChnCFG->m_fPowerSaftyLimitHigh > p_ChnCFG->m_fPowerSaftyLimitLow) //safety有效
        {
            MySafty->m_fHigh = p_ChnCFG->m_fPowerSaftyLimitHigh;
            MySafty->m_fLow = p_ChnCFG->m_fPowerSaftyLimitLow;
//              for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
//             {
//                 if(ChannelIdx == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
//                 {
//                    MySafty->m_fHigh = p_ChnCFG->m_fPowerSaftyLimitHigh*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                    MySafty->m_fLow = p_ChnCFG->m_fPowerSaftyLimitLow*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                    break;
//                 }
//             }
        }
        else
        {
            MySafty->m_fHigh = MAX_ARBIN_VALUE;
            MySafty->m_fLow = -MAX_ARBIN_VALUE;
        }
        break;
    case MP_SYS_SAFTY_TYPE_INIT_SCHEDULE:
    default:
        MySafty->m_fHigh = MAX_ARBIN_VALUE;
        MySafty->m_fLow = -MAX_ARBIN_VALUE;
        break;
    }

    MySafty->m_bIfCheck = 1;
}


void ChannelInfo_Manager::GetScheduleSafty(unsigned short ChannelIdx, unsigned char SaftyType, St_SaftyCheckItem * MySafty)
{
    St_ChannelCFG				*		p_ChnCFG		=		&BoardCFG.m_Channel[ChannelIdx];
    St_ScheduleSetting	*		p_Setting		=		&ChannelSchedule[ChannelIdx].m_Settings;
    St_SchTestObject * p_TestObj    =   &ChannelSchedule[ChannelIdx].m_TestObject;
//	St_SchTestObject    *   p_TestObject=   &ChannelSchedule[ChannelIdx].m_TestObject;
    int AuxIndex = 0;

    switch(SaftyType)
    {
    case MP_SCH_SAFTY_TYPE_CURRENT:
// 		MySafty->m_bIfCheck = p_Setting->m_SafetyCurrent.m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        if(p_Setting->m_SafetyCurrent.m_nMaxType  == 1)
        {
            MySafty->m_fHigh = p_Setting->m_SafetyCurrent.m_fMax * p_TestObj->m_fMaxCurrentCharge / 100;
//             for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
//             {
//                 if(ChannelIdx == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
//                 {
//                     MySafty->m_fHigh=MySafty->m_fHigh*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                     break;
//                 }
//             }
        }
        else if(p_Setting->m_SafetyCurrent.m_nMaxType == 2)
        {
            MySafty->m_fHigh = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyCurrent.m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyCurrent.m_fMax);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_CURRENT;
        }
        else
            MySafty->m_fHigh = p_Setting->m_SafetyCurrent.m_fMax;

        if(p_Setting->m_SafetyCurrent.m_nMinType  == 1)//180726 zyx
        {
            MySafty->m_fLow = p_Setting->m_SafetyCurrent.m_fMin * p_TestObj->m_fMaxCurrentCharge / 100;
//             for(int GroupIdx = 0; GroupIdx < ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupCount; GroupIdx++) //zyx 170727
//             {
//                 if(ChannelIdx == ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_HeadChn)
//                 {
//                     MySafty->m_fLow=MySafty->m_fLow*ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[GroupIdx].m_UnitGroupInfo.m_WholeGroupSize;
//                     break;
//                 }
//             }
        }
        else if(p_Setting->m_SafetyCurrent.m_nMinType == 2)
        {
            MySafty->m_fLow = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyCurrent.m_ucDataTypeLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyCurrent.m_fMin);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx]|= Glb_Safety_CURRENT;
        }
        else
            MySafty->m_fLow = p_Setting->m_SafetyCurrent.m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_VOLTAGE:
// 		MySafty->m_bIfCheck = p_Setting->m_SafetyVoltage.m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        if(p_Setting->m_SafetyVoltage.m_nMaxType  == 1)
            MySafty->m_fHigh = p_Setting->m_SafetyVoltage.m_fMax * p_TestObj->m_fMaxAllowVoltage / 100;
        else if(p_Setting->m_SafetyVoltage.m_nMaxType  == 2)
        {
            MySafty->m_fHigh = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyVoltage.m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyVoltage.m_fMax);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_VOLTAGE;
            ChannelInfo_Manager::uc_MV_Udx_Glb_RedundantVotlage[ChannelIdx] = 1;
        }
        else
            MySafty->m_fHigh = p_Setting->m_SafetyVoltage.m_fMax;

        if(p_Setting->m_SafetyVoltage.m_nMinType  == 1)//180726 zyx
            MySafty->m_fLow = p_Setting->m_SafetyVoltage.m_fMin * p_TestObj->m_fMinAllowVoltage / 100; //180803 zyx
        else if(p_Setting->m_SafetyVoltage.m_nMinType  == 2)
        {
            MySafty->m_fLow = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyVoltage.m_ucDataTypeLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyVoltage.m_fMin);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_VOLTAGE;
        }
        else
            MySafty->m_fLow = p_Setting->m_SafetyVoltage.m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_POWER:
// 		MySafty->m_bIfCheck = p_Setting->m_SafetyPower.m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        if(p_Setting->m_SafetyPower.m_nMaxType  == 1)
            MySafty->m_fHigh = p_Setting->m_SafetyPower.m_fMax * p_TestObj->m_fMaxCurrentCharge * p_TestObj->m_fMaxAllowVoltage / 100;
        else if(p_Setting->m_SafetyPower.m_nMaxType  == 2)
        {
            MySafty->m_fHigh = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyPower.m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyPower.m_fMax);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_POWER;
        }
        else
            MySafty->m_fHigh = p_Setting->m_SafetyPower.m_fMax;
        if(p_Setting->m_SafetyPower.m_nMinType  == 1)
            MySafty->m_fLow = p_Setting->m_SafetyPower.m_fMin * p_TestObj->m_fMaxCurrentCharge * p_TestObj->m_fMaxAllowVoltage / 100;
        else if(p_Setting->m_SafetyPower.m_nMinType  == 2)
        {
            MySafty->m_fLow = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyPower.m_ucDataTypeLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyPower.m_fMin);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_POWER;
        }
        else
            MySafty->m_fLow = p_Setting->m_SafetyPower.m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_CAPACITY:
//         MySafty->m_bIfCheck = p_Setting->m_SafetyCapacity.m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        if(p_Setting->m_SafetyCapacity.m_nMaxType  == 1)
            MySafty->m_fHigh = p_Setting->m_SafetyCapacity.m_fMax * p_TestObj->m_fNorminalCapacity / 100;
        else if(p_Setting->m_SafetyCapacity.m_nMaxType  == 2)
        {
            MySafty->m_fHigh = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyCapacity.m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyCapacity.m_fMax);
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_CAPACITY;
        }
        else
            MySafty->m_fHigh = p_Setting->m_SafetyCapacity.m_fMax;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_STEPTIME:
//         MySafty->m_bIfCheck = p_Setting->m_SafetyStepTime.m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        if(p_Setting->m_SafetyStepTime.m_nMaxType == 2)
        {
            MySafty->m_fHigh = ChannelInfo_Manager::Get_MetaVariable(ChannelIdx,p_Setting->m_SafetyStepTime.m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,p_Setting->m_SafetyStepTime.m_fMax) * 3600; //hour to seconds
            ChannelInfo_Manager::uc_MV_Udx_Glb_Safety[ChannelIdx] |= Glb_Safety_STEPTIME;
        }
        else
            MySafty->m_fHigh = p_Setting->m_SafetyStepTime.m_fMax;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiVoltage:
        AuxIndex = p_Setting->m_SchAux_Glb_Voltage.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiTemprature:
        AuxIndex = p_Setting->m_SchAux_Glb_Temperature.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiPressure:
        AuxIndex = p_Setting->m_SchAux_Glb_Pressure.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiExternalCharge:
        AuxIndex = p_Setting->m_SchAux_Glb_ExtCurrent.m_SafetyStartIndex;
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiPhValue:
        AuxIndex = p_Setting->m_SchAux_Glb_PH.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiFlowRate:
        AuxIndex = p_Setting->m_SchAux_Glb_FlowRate.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiHumidity: 								  // 用于新版温控器湿度SCH报警   zyx
        AuxIndex = p_Setting->m_SchAux_Glb_Humidity.m_SafetyStartIndex;
// 		MySafty->m_bIfCheck = p_Setting->m_AuxSafty[AuxIndex].m_bEnabled;
// 		if(MySafty->m_bIfCheck)
// 		{
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
// 		}
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyCurrent:
        AuxIndex = p_Setting->m_SchAux_Glb_SaftyCurrent.m_SafetyStartIndex;
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage:
        AuxIndex = p_Setting->m_SchAux_Glb_SafetyVoltage.m_SafetyStartIndex;
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyTemperature:
        AuxIndex = p_Setting->m_SchAux_Glb_SafetyTemperature.m_SafetyStartIndex;
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiDI:
        AuxIndex = p_Setting->m_SchAux_Glb_DI.m_SafetyStartIndex;
        MySafty->m_fHigh = p_Setting->m_AuxSafty[AuxIndex].m_fMax;
        MySafty->m_fLow = p_Setting->m_AuxSafty[AuxIndex].m_fMin;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiConcentration  :
        break;
    default:
        MySafty->m_bIfCheck = 0;
        break;
    }
}

unsigned char  ChannelInfo_Manager::AuxSaftyTypeChangeAuxChannelType(unsigned short ChannelIdx,unsigned char uc_AuxSaftyType,St_SaftyCheckItem * MySafty)
{
    // enum TE_SCH_SAFTY_TYPE    change to   enum TE_AUX_CHAN_TYPE

    St_ScheduleSetting	*		p_Setting		=		&ChannelSchedule[ChannelIdx].m_Settings;
//    St_SchTestObject * p_TestObj    =   &ChannelSchedule[ChannelIdx].m_TestObject;
//    int AuxIndex = 0;
    MySafty->m_bIfCheck = 0;
    switch(uc_AuxSaftyType)
    {
    case MP_SCH_SAFTY_TYPE_AuxiVoltage:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_Voltage.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_VOLTAGE;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiTemprature:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_Temperature.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_TEMPERATURE;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiPressure:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_Pressure.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_PRESSURE;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiExternalCharge:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_ExtCurrent.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_EXTERNALCHARGE;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiFlowRate:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_FlowRate.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_FLOW_RATE;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiHumidity: 								  // 用于新版温控器湿度SCH报警   zyx
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_Humidity.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_HUMIDITY;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyCurrent:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_SaftyCurrent.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_SAFETY;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_SafetyVoltage.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_SAFETY;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiSafetyTemperature:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_SafetyTemperature.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_SAFETY;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiDI:
        MySafty->m_bIfCheck = p_Setting->m_SchAux_Glb_DI.m_bEnabled;
        if(MySafty->m_bIfCheck)
            return AUX_SAFETY;
        break;
    case MP_SCH_SAFTY_TYPE_AuxiConcentration:
    case MP_SCH_SAFTY_TYPE_AuxiPhValue:
    default:
        break;
    }
    return ALLTYPE;
}

double ChannelInfo_Manager::dHowLongToNow(MsTime myTime,MsTime SystemTime)
{
    MsTime timediff;
    Int16 temp;
    double TimeInSecond;
    //MsTime OStime = CommonFunc::GetSystemTime();

    if(BurstModeSampling::bl_Time)
    {
        BurstModeSampling::OSTime_back = SystemTime;//  2017.6.8  yy
        BurstModeSampling::bl_Time = false;
    }

    if(myTime.Second > SystemTime.Second)
        return 0;
    else if((myTime.Second == SystemTime.Second) && (myTime.Us100 >= SystemTime.Us100))
        return 0;

    temp = SystemTime.Us100 - myTime.Us100;
    timediff.Second = SystemTime.Second - myTime.Second;
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;

    TimeInSecond  = timediff.Us100;
    TimeInSecond /= 10000;
    TimeInSecond += timediff.Second;

    return TimeInSecond;
}

//private 方法：
float ChannelInfo_Manager::fHowLongToNow(MsTime myTime)
{
    MsTime timediff;
    Int16 temp;
    float TimeInSecond;
    MsTime OStime = CommonFunc::GetSystemTime();

    if(BurstModeSampling::bl_Time)
    {
        BurstModeSampling::OSTime_back = OStime;//  2017.6.8  yy
        BurstModeSampling::bl_Time = false;
    }

    if(myTime.Second > OStime.Second)
        return 0;
    else if((myTime.Second == OStime.Second) && (myTime.Us100 >= OStime.Us100))
        return 0;

    temp = OStime.Us100 - myTime.Us100;
    timediff.Second = OStime.Second - myTime.Second;
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;

    TimeInSecond  = timediff.Us100;
    TimeInSecond /= 10000;
    TimeInSecond += timediff.Second;

    return TimeInSecond;
}

#if DEBUG_TRACE_RECORD == 1
void ChannelInfo_Manager::InitDebugTraceRecord()
{
    INIT_QUEUE(DebugTraceRecord);
}
void ChannelInfo_Manager::PushDebugTraceRecord(St_DebugPoint myDebugPoint)
{
    ENQUEUE(DebugTraceRecord, DEBUG_TRACE_RECORD_DEPTH, myDebugPoint);
}
#endif

//private 方法：
MsTime ChannelInfo_Manager::CalculatePreviousMsTime(double time)
{
    MsTime PreviousTime;
    MsTime CurrentTime = CommonFunc::GetSystemTime();
    Int16 Us100;
    unsigned long InSecond;


    InSecond = time;
    Us100 = (time - InSecond) * 10000; //hepeiqing add 20130828,this is a bug that lead to a tiny time error

    if(CurrentTime.Us100 < Us100)
    {
        CurrentTime.Second--;
        CurrentTime.Us100 += 10000;
    }

    PreviousTime.Second = CurrentTime.Second - InSecond;
    PreviousTime.Us100 = CurrentTime.Us100 - Us100;

    return PreviousTime;
}
void ChannelInfo_Manager::Init_ScheduleBuffer()
{
    for (int i = 0; i < IV_MAX_SCHEDULE_BUFFER_NUMBER; i++)
    {
        ScheduleBuf[i].m_u32IvRegister = 0;
        ScheduleBuf[i].m_bValid = 0;
    }

    SIMU_BUFFER::Init();
// 	return 1;
}
void ChannelInfo_Manager::Init_PulseBuffer()
{
    for (int i = 0; i < IV_MAX_PULSE_BUFFER_NUMBER; i++)
    {
        PulseBuf[i].m_u32IvRegister = 0;
        PulseBuf[i].m_bValid = 0;
    }
// 	return 1;
}
// void ChannelInfo_Manager::Init_ChannelSchedule(unsigned char uc_ChannelNo)
// {
// 	ChannelSchedule[uc_ChannelNo].m_scBufIdx = -1;

// 	return 1;
// }
void ChannelInfo_Manager::Init_ChannelStep(unsigned char uc_ChannelNo)
{
    MsTime time0, time1;
    unsigned char activeindex = ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    time0 = ChannelStep[uc_ChannelNo].m_Info[0].m_StartTime;
    time1 = ChannelStep[uc_ChannelNo].m_Info[1].m_StartTime;
    memset(&ChannelStep[uc_ChannelNo], 0, sizeof(ChannelStep[uc_ChannelNo]));
    ChannelStep[uc_ChannelNo].m_Info[0].m_StartTime = time0;
    ChannelStep[uc_ChannelNo].m_Info[1].m_StartTime = time1;

    ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = CT_IVCH_STOP;
    ChannelStep[uc_ChannelNo].m_Info[1].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = CT_IVCH_STOP;
    ChannelStep[uc_ChannelNo].m_ucActiveIdx = activeindex;

    Step_Manager_task::ACIM_Status[uc_ChannelNo] = ACIM_NONE;
// 	return 1;
}

void ChannelInfo_Manager::Copy_PVmeta2LSmeta(unsigned char uc_ChannelNo)
{
    st_GetMetaValue	* p_MV2				=		&MetaVariable2[uc_ChannelNo][0];

    p_MV2[MetaCode_LS_Voltage].m_fValue              = p_MV2[MetaCode_PV_Voltage].m_fValue;
    p_MV2[MetaCode_LS_Current].m_fValue              = p_MV2[MetaCode_PV_Current].m_fValue;
    p_MV2[MetaCode_LS_StepTime].m_fValue             = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Steptime); //p_MV2[MetaCode_PV_Steptime].m_fValue;
    p_MV2[MetaCode_LS_TestTime].m_fValue             = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_TestTime); //p_MV2[MetaCode_PV_TestTime].m_fValue;
    p_MV2[MetaCode_LS_StepIndex].m_fValue            = p_MV2[MetaCode_PV_StepIndex].m_fValue;
    p_MV2[MetaCode_LS_CycleIndex].m_fValue           = p_MV2[MetaCode_PV_CycleIndex].m_fValue;
    p_MV2[MetaCode_LS_ChargeCapacity].m_fValue       = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_ChargeCapacity); //p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
    p_MV2[MetaCode_LS_DischargeCapacity].m_fValue    = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_DischargeCapacity); //p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
    p_MV2[MetaCode_LS_ChargeEnergy].m_fValue         = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_ChargeEnergy); //p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
    p_MV2[MetaCode_LS_DischargeEnergy].m_fValue      = Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_DischargeEnergy); //p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
    p_MV2[MetaCode_LS_InternalResistance].m_fValue   = p_MV2[MetaCode_PV_InternalResistance].m_fValue;
    p_MV2[MetaCode_LS_ACR].m_fValue                  = p_MV2[MetaCode_PV_ACR].m_fValue;

    p_MV2[MetaCode_LS_dVdt].m_fValue                 = p_MV2[MetaCode_PV_dVdt].m_fValue;
    p_MV2[MetaCode_LS_dIdt].m_fValue                 = p_MV2[MetaCode_PV_dIdt].m_fValue;
    p_MV2[MetaCode_LS_AC_Impedance].m_fValue         = p_MV2[MetaCode_PV_AC_Impedance].m_fValue;
    p_MV2[MetaCode_LS_VmaxOnCycle].m_fValue          = p_MV2[MetaCode_PV_VmaxOnCycle].m_fValue;

//   return 1;
}
void ChannelInfo_Manager::Copy_PVmeta2LCmeta(unsigned char uc_ChannelNo)
{
    st_GetMetaValue	* p_MV2				=		&MetaVariable2[uc_ChannelNo][0];
    //St_GroupData		* p_ChnData		=		&ChannelInfo[uc_ChannelNo].m_GroupData;

    p_MV2[MetaCode_LC_Voltage].m_fValue              = p_MV2[MetaCode_LS_Voltage].m_fValue;
    p_MV2[MetaCode_LC_Current].m_fValue              = p_MV2[MetaCode_LS_Current].m_fValue;
    p_MV2[MetaCode_LC_StepTime].m_fValue             = p_MV2[MetaCode_LS_StepTime].m_fValue;
    p_MV2[MetaCode_LC_TestTime].m_fValue             = p_MV2[MetaCode_LS_TestTime].m_fValue;
    p_MV2[MetaCode_LC_StepIndex].m_fValue            = p_MV2[MetaCode_LS_StepIndex].m_fValue;
    p_MV2[MetaCode_LC_CycleIndex].m_fValue           = p_MV2[MetaCode_LS_CycleIndex].m_fValue;
    p_MV2[MetaCode_LC_ChargeCapacity].m_fValue       = p_MV2[MetaCode_LS_ChargeCapacity].m_fValue;
    p_MV2[MetaCode_LC_DischargeCapacity].m_fValue    = p_MV2[MetaCode_LS_DischargeCapacity].m_fValue;
    p_MV2[MetaCode_LC_ChargeEnergy].m_fValue         = p_MV2[MetaCode_LS_ChargeEnergy].m_fValue;
    p_MV2[MetaCode_LC_DischargeEnergy].m_fValue      = p_MV2[MetaCode_LS_DischargeEnergy].m_fValue;
    p_MV2[MetaCode_LC_InternalResistance].m_fValue   = p_MV2[MetaCode_LS_InternalResistance].m_fValue;
    p_MV2[MetaCode_LC_ACR].m_fValue                  = p_MV2[MetaCode_LS_ACR].m_fValue;

    p_MV2[MetaCode_LC_dVdt].m_fValue                 = p_MV2[MetaCode_LS_dVdt].m_fValue;
    p_MV2[MetaCode_LC_dIdt].m_fValue                 = p_MV2[MetaCode_LS_dIdt].m_fValue;
    p_MV2[MetaCode_LC_AC_Impedance].m_fValue         = p_MV2[MetaCode_LS_AC_Impedance].m_fValue;
    p_MV2[MetaCode_LC_VmaxOnCycle].m_fValue          = p_MV2[MetaCode_LS_VmaxOnCycle].m_fValue;

// 	return 1;
}

void ChannelInfo_Manager::Set_PID_MaxCurrentRange(unsigned char uc_ChannelNo)
{
    unsigned char  NonActiveStep = !ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    St_StepCtrl * p_ThisControl = &ChannelStep[uc_ChannelNo].m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl;
    unsigned char CurrentRange = ChannelInfo_Manager::GetPhysicalCurrentRange(p_ThisControl->m_ucCurrentRange);
    if((ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)) //master, but not PH current range
            || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) && ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo)))
    {
        ChannelInfo[uc_ChannelNo].m_fCurrentRangeOfPid = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,CurrentRange) * ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo);
    }
    else
        ChannelInfo[uc_ChannelNo].m_fCurrentRangeOfPid = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,CurrentRange);
}

float ChannelInfo_Manager::Get_PID_MaxCurrentRange(unsigned char uc_ChannelNo)
{
    return ChannelInfo[uc_ChannelNo].m_fCurrentRangeOfPid;
}

void ChannelInfo_Manager::SpecialPoint_Init(unsigned char uc_ChannelNo)
{
    Step_Manager_task::b_CurCoarseTuneFinished[uc_ChannelNo] = false;
    Step_Manager_task::b_FineTuneFinished[uc_ChannelNo] = false;
    Is_Current_SoftWareClamp_Work[uc_ChannelNo] = false;
    Is_Power_SoftWareClamp_Work[uc_ChannelNo] = false;
    Is_ClampDigiVolt_Work[uc_ChannelNo] = false;
}

bool ChannelInfo_Manager::bNeedChangeRange(unsigned short usControlType)
{
    bool bNeedChangeRange = true;
    switch(usControlType)
    {
    case CT_TYPE_CAN_SEND_MSG:	//zyx 2107121
    case CT_IVCH_WRITECANOUTBOUND:
    case CT_IVCH_WRITE_SMB_BLOCK:
    case CT_TYPE_SMB_WRITEREG_WORD:
    case CT_TYPE_SMB_WRITEREG_STRING:
    case CT_TYPE_SMB_OPTWORDADDRESS:
    case CT_IVCH_SETVARIABLE:
    case CT_IVCH_SETVALUE:
        bNeedChangeRange = false;
        break;
    default:
        bNeedChangeRange = true;
        break;
    }
    return bNeedChangeRange;
}

int ChannelInfo_Manager::LoadStepData(unsigned char uc_ChannelNo, unsigned short u16_StepID)
{
    signed char    sc_SchBufIdx  = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    unsigned char  NonActiveStep = !ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    SpecialPoint_Init(uc_ChannelNo);
    if(m_Online_UpdataEnable[uc_ChannelNo])
        NonActiveStep = ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return -1;     //schedule buffer index 指针越界，返回-1;
    if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
        return -1;     //schedule buffer 不合法，返回-1;

    if(!m_Online_UpdataEnable[uc_ChannelNo])
        ChannelStep[uc_ChannelNo].m_Info[NonActiveStep].m_StartTime = CommonFunc::GetSystemTime();
    St_StepCtrl * p_ThisControl    =    &ChannelStep[uc_ChannelNo].m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl;
    St_ChannelStep * p_ThisStep    =    &ChannelStep[uc_ChannelNo];
    St_ChannelInfo * p_ThisInfo    =    &ChannelInfo[uc_ChannelNo];

    p_ThisInfo->m_bNoNeedFineTune_flag = false;		//20190807 dyy
    p_ThisInfo->m_bDynamicControl = false;
    Step_Manager_task::bCV[uc_ChannelNo] = 0;
    //第一步：load主控制步骤；
    memcpy( (void *)&p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data,
            (void *)&ScheduleBuf[sc_SchBufIdx].m_StepData[u16_StepID],
            sizeof(St_StepData) );
    p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = GetMatchedCurrentRange(uc_ChannelNo,
            ChannelSchedule[uc_ChannelNo].m_TestObject.m_fMaxCurrentCharge,
            &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl);//171113,hpq,add for pro8
//     if(p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl.m_ucLogLimitNo == 0)//180727 zyx
//         p_ThisStep->m_bSdlEnabled = 1;
//     else
//         p_ThisStep->m_bSdlEnabled = 0;
    ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_StepIndex].m_fValue = u16_StepID;  // 20190927 yy
    Add_Global_Loglimit(uc_ChannelNo,&p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data,&ChannelSchedule[uc_ChannelNo]);
    //now loaded in the ctrl type, ctrl value and current range:
    //single channel mode, if PH is set in sdu, convert to general current high range:
    if (Serial2ParaMod_IsSingleChanMode())
    {
        if (p_ThisControl->m_ucCurrentRange == CURRENT_RANGE_PARALLELHIGH)
        {
            p_ThisControl->m_ucCurrentRange = CURRENT_RANGE_1;
            SetCurrentRangeParallelHigh(uc_ChannelNo, false);
        }
    }
    else  //parallel mode
    {
        if(Serial2ParaMod_IsMasterChannel(uc_ChannelNo) || Serial2ParaMod_IsInterUnitSubmasterChannel(uc_ChannelNo))
        {
//			if( (p_ThisControl->m_ucCurrentRange == CURRENT_RANGE_PARALLELHIGH)  && (p_ThisControl->m_u16ControlType == CT_IVCH_VOLTAGE) )                // 03.28.2017 PWM并联改为硬件PID
//				p_ThisControl->m_u16ControlType = CT_IVCH_DIGIV;    //20150120,add by hepeiqing
            //for intra unit parallel: copy step from master to slave:
            if (Serial2ParaMod_IsIntraUnitMasterChannel(p_ThisInfo->m_GroupID) || Serial2ParaMod_IsInterUnitSubmasterChannel(p_ThisInfo->m_GroupID))
            {
                unsigned short uChanIdx, uLastChanIdx;
                uLastChanIdx = FindGroupLastChanIdx(uc_ChannelNo);

                //有的没的先拷贝一份一样的再修改:
                for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uLastChanIdx; uChanIdx++  )
                {
                    ::memcpy( (void *)&ChannelStep[uChanIdx],
                              (void *)&ChannelStep[uc_ChannelNo],
                              sizeof(ChannelStep[uc_ChannelNo]) );
                }
            }

            //decide whether all chan on (PH) or only master on
            unsigned short ControlType;
            ControlType = p_ThisControl->m_u16ControlType;

            //for parallel mode, set up PH flag, and convert master's PH to general current high:

//             if( ControlType != CT_IVCH_SETVARIABLE && ControlType != CT_IVCH_SETVALUE) //dirui 20170413  setvariable 不处理量程问题 //171109 zyx
            if(bNeedChangeRange(ControlType))
            {
                if (p_ThisControl->m_ucCurrentRange == CURRENT_RANGE_PARALLELHIGH)
                {
                    //for IR, ACI, ACIM, setVar, only master works
                    switch(ControlType)
                    {
                    //	case CT_IVCH_REST:
                    //	case CT_IVCH_SETVARIABLE:
                    //case CT_IVCH_IR:     //dir delete for IR to CC change . 20191023
                    case  CT_IVCH_ACIR:
                    case CT_IVCH_ACR:
                    case CT_IVCH_ACI:
                    case CT_SPECIAL_IRTEST://zyx 2105291
                    case CT_IVCH_ACIM:
                    case CTRL_TYPE_MCELL_ACI:
                    case CT_IVCH_CELLV:
                    case CT_IVCH_ACS:
                    case CTRL_TYPE_VOLT_METER:
                    case CT_TYPE_DO_SETTING:
                    case CT_IVCH_EXTCHARGE:
                    case CT_IVCH_PAUSE:

                        p_ThisControl->m_ucCurrentRange = CURRENT_RANGE_1;
                        SetCurrentRangeParallelHigh(uc_ChannelNo, false);
                        break;
                    default:
                        SetCurrentRangeParallelHigh(uc_ChannelNo, true);
                        break;
                    }
                    //master chan must be high range from PH
                    p_ThisControl->m_ucCurrentRange = CURRENT_RANGE_1;
                }
                else if(ControlType == CT_IVCH_REST && (Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo) || Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo)) )
                {
                    SetCurrentRangeParallelHigh(uc_ChannelNo, true);
                    p_ThisControl->m_ucCurrentRange = CURRENT_RANGE_1;
                }
                else
                {
                    SetCurrentRangeParallelHigh(uc_ChannelNo, false);
                }
            }
            //set group ctrl type:
            //if all chan on (PH), then slaves & submater are C-ctrl, else rest
            //      if (IsInterUnitParaMode(uc_ChannelNo))//inter unit parallel
            if (Serial2ParaMod_IsInterUnitMasterChannel(uc_ChannelNo))
            {
                if (IsCurrentRangeParallelHigh(uc_ChannelNo) && ControlType != CT_IVCH_REST)
                    gUnit_stUnitInfo.m_GroupInfo[p_ThisInfo->m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_CURRENT;
                else
                    gUnit_stUnitInfo.m_GroupInfo[p_ThisInfo->m_GroupID].m_stSubGroupCtrl.m_uSubGroupCtrlType = CT_IVCH_REST;
            }
            else //IsIntraUnitParaMode
            {
                if (IsCurrentRangeParallelHigh(uc_ChannelNo) && ControlType != CT_IVCH_REST)
                    SetIntraUnitParallelSlaveStepCtrlType(uc_ChannelNo, NonActiveStep, CT_IVCH_CURRENT);
                else
                    SetIntraUnitParallelSlaveStepCtrlType(uc_ChannelNo, NonActiveStep, CT_IVCH_REST);
            }
        }

    }

    Set_PID_MaxCurrentRange(uc_ChannelNo);//190403 zyx
//     Step_Manager_task::ClearAdjust[uc_ChannelNo] = 0;
// 	 if(Step_Manager_task::Unipolar.BoardFlag) // 单向功率板处理  20161209
// 	 {
// 		Step_Manager_task::Unipolar.EngineState[uc_ChannelNo] = UNIPOLAR_INTERNCHANGE_START;
// 		Step_Manager_task::Unipolar.EngineEnable[uc_ChannelNo] = 0;
// 		Step_Manager_task::Unipolar.EngineInterval[uc_ChannelNo] = OS_ServeProcess::OS_Time;
// 		Step_Manager_task::Unipolar.EngineWait[uc_ChannelNo] = OS_ServeProcess::OS_Time;
//     }

    if(p_ThisControl->m_u16ControlType == CT_IVCH_IR)
    {
        p_ThisControl->m_ucLogLimitNo  = 0;
    }
    p_ThisControl->m_ucCurrentRange = GetPhysicalCurrentRange(p_ThisControl->m_ucCurrentRange);//170320,add by hepeiqing
//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
    {
        CANBusParse::st_SNResTable[uc_ChannelNo][0].m_SNCount = 0;
        Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].AuxLimitCnt = 0;
        CANBusParse::IV_ClearAuxSNList(uc_ChannelNo, LIMITSN);
        for(unsigned char UnitIDIdx = 0; UnitIDIdx < MAXAUXUNIT; UnitIDIdx++)
        {
            if( CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bExist)
                CANBusParse::st_IV2AuxInfoMgr[uc_ChannelNo][UnitIDIdx].m_bShiedCheck = 1;
        }
        memcpy((void*)&Channel_MapAuxInfo::CurrentStepAuxSum[uc_ChannelNo][0], &Channel_MapAuxInfo::CurrentSafetyCount[uc_ChannelNo][0], MAXUNITID); //要加上safety的
        memset((void*)&Channel_MapAuxInfo::Channel_MapAuxInfo::m_AuxDataRequested[uc_ChannelNo][0][0], 0, sizeof(unsigned char)*MAXAUXTYPE * MAXVIRTUALCHANNEL);
    }
//#endif

    for(unsigned char LimitIdx = 0;
            LimitIdx < (p_ThisControl->m_ucStepLimitNo + p_ThisControl->m_ucLogLimitNo);
            LimitIdx++)
    {
        St_LimitInfo *  p_ThisLimit  =   &p_ThisStep->m_limitInfo[LimitIdx];
        St_Limit *      p_Limit      =   &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[LimitIdx];

        p_ThisLimit->m_SN = -1;
        p_ThisLimit->m_bChecked = 0;
        p_ThisLimit->m_bReached = 0;
        if(p_Limit->m_uCheckPlace == ValueCheckPlace_AUXI)
        {
            BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = false;
            if( (CommonFunc::uc_Support_AUX == 1) && (BuiltIn_DataManager::b_BuiltInAux))
            {
                if(p_Limit->m_bAnyValid)       //需要AUX 和 BUILTIN 辅助同时查
                {
                    p_ThisLimit->m_bAuxCheck = 1;

                    bool bNeedBuildInCheck = false; //zyx 2102251
                    if( BuiltIn_DataManager::m_BuiltInAuxInfo.VolExistFlag == true
                            && (p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_V || p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_V
                                || p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_dVdt || p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_dVdt))
                        bNeedBuildInCheck = true;
                    else  if( BuiltIn_DataManager::m_BuiltInAuxInfo.TempExistFlag == true
                              && (p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_T || p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_T
                                  || p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_dTdt || p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_dTdt))
                        bNeedBuildInCheck = true;

                    if(bNeedBuildInCheck)
                        BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = true;

                }
                else
                {
                    unsigned char ChannelIdx = 0;//AuxType,
                    ChannelIdx = p_Limit->m_fLeftValue;

                    St_AUXchannelInfo  m_AuxInfo;
                    m_AuxInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_ChannelNo, 1, ChannelIdx);
                    unsigned char myIP = board_config::getBID() + 1;
                    if(myIP == m_AuxInfo.m_UnitId)
                    {
                        p_ThisLimit->m_bAuxCheck = 0;
                        BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = true;
// 					           p_Limit->m_uCheckPlace = ValueCheckPlace_BuiltInOnly;
                    }
                    else
                        p_ThisLimit->m_bAuxCheck = 1;
                }
            }
            else if( CommonFunc::uc_Support_AUX == 1)
            {
                p_ThisLimit->m_bAuxCheck = 1;
            }
            else if(BuiltIn_DataManager::b_BuiltInAux)
            {
                p_ThisLimit->m_bAuxCheck = 0;
                BuiltIn_DataManager::b_BuiltInAuxLimitCheckFlag = true;
//              if(p_Limit->m_bAnyValid)
// 							 p_Limit->m_uCheckPlace = ValueCheckPlace_BuiltInAny;
// 						 else
// 							 p_Limit->m_uCheckPlace = ValueCheckPlace_BuiltInOnly;
            }
        }
// 		else if(p_Limit->m_uCheckPlace == ValueCheckPlace_IV)
// 		{

//     }
        else
            p_ThisLimit->m_bAuxCheck = 0;
        p_ThisLimit->m_bAnyLimit = p_Limit->m_bAnyValid;
        p_ThisLimit->m_AnyLimitReachedCount = 0;

        if(u16_StepID == (ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount - 1))  //last step,特殊处理，省去后续STEP,SCHEDULE部分处理的麻烦
        {
            if(p_Limit->m_u16GotoStepID == MCU_GOTONEXTSTEP)
                p_Limit->m_u16GotoStepID = MCU_GOTOSTOPSTEP;
        }

//#if SUPPORT_AUX == 1
        if(CommonFunc::uc_Support_AUX)
        {
            //向辅助发送limit操作  //comment out by DK   //open by hpq,20131218
            if(p_ThisLimit->m_bAuxCheck)
            {
                bool bDIDO = (  (p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_DI) || (p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_DI)
                                || (p_Limit->m_ucLeftValue_DataType == MP_DATA_TYPE_AUX_DO) || (p_Limit->m_ucRightValue_DataType == MP_DATA_TYPE_AUX_DO) );
                if( (p_Limit->m_ucCompareSign != MP_MCS_EQUAL_TO) || bDIDO )
                {
                    Step_Manager_task::AuxLimitBuf[uc_ChannelNo][Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].AuxLimitCnt].MyAuxLimitIdx = LimitIdx;
                    Step_Manager_task::AuxLimitBuf[uc_ChannelNo][Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].AuxLimitCnt].MyAuxStepID = p_ThisControl->m_u16StepID;
                    Step_Manager_task::AuxLimitAckMgr[uc_ChannelNo].AuxLimitCnt++;
                }
                else
                {
                    p_ThisLimit->m_bReached = 1;
                }
            }
            //向辅助发送RequsetData命令 comment out by junjie 2014/1/6
            Channel_MapAuxInfo::Load_AuxStepData(uc_ChannelNo, p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[LimitIdx]);
        }
//#endif
    }
    p_ThisStep->m_LimitCount = p_ThisControl->m_ucLogLimitNo + p_ThisControl->m_ucStepLimitNo;
    p_ThisStep->m_StepLimitCount = p_ThisControl->m_ucStepLimitNo;
    p_ThisStep->m_LogLimitCount = p_ThisControl->m_ucLogLimitNo;

//==========================08312017 by ZC=====================================
//===============扣除CCCV_TV_PulseInterval的LogLimit=====================================
    CCCV_TB_LimitCount[uc_ChannelNo] = p_ThisStep->m_LimitCount;
    for(unsigned char uc_i = p_ThisControl->m_ucStepLimitNo; uc_i < p_ThisStep->m_LimitCount; uc_i++)
    {
        St_Limit * p_Limit = &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[uc_i];
        if(p_Limit->m_fLeftValue == MetaCode_DV_CCCV_TB_PulseInterval )
        {
            p_ThisStep->m_LogLimitCount--;
            p_ThisControl->m_ucLogLimitNo--;
            p_ThisStep->m_LimitCount--;
        }
    }
//===========================END===============================================
//     if(p_ThisControl->m_ucLogLimitNo == 0) //180727 zyx
//         p_ThisStep->m_bSdlEnabled = 1;
//     else
//         p_ThisStep->m_bSdlEnabled = 0;

//#if SUPPORT_AUX == 1
    if(CommonFunc::uc_Support_AUX)
    {
        //利用EnableSDL判断limit是否下载完成
//         CANBusParse::IV_EnableSDLToAUX(uc_ChannelNo, u16_StepID, p_ThisStep->m_bSdlEnabled);
        short TestSettingIdx = p_ThisControl->m_ucTestSettingIdx;
        if( TestSettingIdx >= 0)
            Channel_MapAuxInfo::Set_AuxOutputCtrl(uc_ChannelNo, TestSettingIdx);
    }
//#endif

    //判断是否本步为只有一个DV_time>x模式，如果是，则需要特殊处理：
    if (p_ThisControl->m_ucLogLimitNo == 1)
    {
        St_Limit * MyLimit = &p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Limits[p_ThisControl->m_ucStepLimitNo];
        if ((MyLimit->m_fLeftValue == MetaCode_DV_Time) && (MyLimit->m_ucLeftValue_DataType == MP_DATA_TYPE_METAVALUE)
                && ((MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN) || (MyLimit->m_ucCompareSign == MP_MCS_GREATER_THAN_OR_EQUAL_TO))
                && (MyLimit->m_ucRightValue_DataType == MP_DATA_TYPE_FIGURE))
        {
            p_ThisStep->m_bIsDvInterval = 1;
            p_ThisStep->m_fLogInterval = MyLimit->m_fRightValue;
            p_ThisStep->m_u64LogInterval = p_ThisStep->m_fLogInterval * 10000;
        }
        else
            p_ThisStep->m_bIsDvInterval = false;
    }
    else
        p_ThisStep->m_bIsDvInterval = false;

    //第二部：load额外的特殊控制结构；
    p_ThisStep->m_bIsFirstLogPoint = 1;
    Pid_Ctrl::fUpLimit[uc_ChannelNo] = 1.0f;
    Pid_Ctrl::fDnLimit[uc_ChannelNo] = -1.0f;   //20150813,add by hpq,for huawei bug
    p_ThisInfo->bDigitalV = 1;
//     p_ThisInfo->bDigitalI = 0;
    p_ThisInfo->m_SimuInfo.m_bValid = 0;
    p_ThisInfo->m_SimuInfo.m_bCriticalLog = 0;  //160425
    Step_Manager_task::CCCVNoLimitCheckFlag[uc_ChannelNo] = 0;  //20150916,add by hepeiqing,for huawei's debug
    switch (p_ThisControl->m_u16ControlType)
    {
    case CT_IVCH_VOLTAGE:
        if(m_Online_UpdataEnable[uc_ChannelNo])
        {
            St_Sche_Onlineupdata  * st_Onlineupdata = (St_Sche_Onlineupdata  *) &EthernetParse::m_btPackDataBuffer;
            if(!st_Onlineupdata->st_updata.m_Step_UpData[0].m_ucValid_StepCtrl)
                break;
        }

        Step_Manager_task::ChannelCVStatus[uc_ChannelNo] = STEP_CV_Init;
        Step_Manager_task::CCCVNoLimitCheckFlag[uc_ChannelNo] = 1;
        ChannelInfo_Manager::SetPV_CV_StageCurrent(uc_ChannelNo);
    case CT_IVCH_VRAMP:
    case CT_IVCH_VSTAIRCASE:
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM)
            p_ThisInfo->bDigitalV = 0;
        else
        {
            if(board_config::DacInfo.m_bHaveVoltageDac )
            {
                if( ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_ChannelNo))
                    p_ThisInfo->bDigitalV = 1;
                else
                    p_ThisInfo->bDigitalV = 0;
            }
        }
        break;
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCV_WRM:
    {
        if(m_Online_UpdataEnable[uc_ChannelNo])
        {
            St_Sche_Onlineupdata  * st_Onlineupdata = (St_Sche_Onlineupdata  *) &EthernetParse::m_btPackDataBuffer;
            if(!st_Onlineupdata->st_updata.m_Step_UpData[0].m_ucValid_StepCtrl)
                break;
        }

        Step_Manager_task::ChannelCCCVStatus[uc_ChannelNo] = STEP_CCCV_INIT;
        Step_Manager_task::CCCVNoLimitCheckFlag[uc_ChannelNo] = 1;
        Step_Manager_task::bCCCV_Initiated[uc_ChannelNo] = false;//0;
        p_ThisInfo->m_bCVstage = 0;
        unsigned char uCurrRng = p_ThisControl->m_ucCurrentRange;    // 将电流设定值作为PID计算时的钳位值， 09.09.2015
        float fTargetCurrent = Get_MetaVariable(uc_ChannelNo,
                                                p_ThisControl->m_ucCtrlValue_DataType[0],p_ThisControl->m_CtrlValueOperator[0],p_ThisControl->m_CtrlValueCoefficients[0],
                                                p_ThisControl->m_CtrlValues.m_fControlValue[0]);
        SetPV_CV_StageCurrent(uc_ChannelNo, fTargetCurrent);
        //151120,modified by hepeiqing, for formula or metavariable as CC value bug
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = UnifyCurrent(uc_ChannelNo,  fTargetCurrent);//uCurrRng,
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = abs(Pid_Ctrl::fUpLimit[uc_ChannelNo]);
        Pid_Ctrl::fDnLimit[uc_ChannelNo] = - Pid_Ctrl::fUpLimit[uc_ChannelNo];    //20150813,add by hpq,for huawei bug
        break;
    }
    case CT_IVCH_CCCV_TB:  //TURBO BOOST BY ZC 07.07.2017
    {
        Step_Manager_task::ChannelTBStatus[uc_ChannelNo] = STEP_CCCV_TB_START;
        unsigned char uCurrRng = p_ThisControl->m_ucCurrentRange;    // 将电流设定值作为PID计算时的钳位值
        float fTargetCurrent = Get_MetaVariable(uc_ChannelNo,
                                                p_ThisControl->m_ucCtrlValue_DataType[0], p_ThisControl->m_CtrlValueOperator[0],p_ThisControl->m_CtrlValueCoefficients[0],
                                                p_ThisControl->m_CtrlValues.m_fControlValue[0]);
        SetPV_CV_StageCurrent(uc_ChannelNo, fTargetCurrent);
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = UnifyCurrent(uc_ChannelNo, fTargetCurrent);//uCurrRng,
        Pid_Ctrl::fUpLimit[uc_ChannelNo] = abs(Pid_Ctrl::fUpLimit[uc_ChannelNo]);
        Pid_Ctrl::fDnLimit[uc_ChannelNo] = - Pid_Ctrl::fUpLimit[uc_ChannelNo];
        ChannelInfo_Manager::CCCV_TB_CVCounter[uc_ChannelNo] = 0;
        Step_Manager_task::CCCV_TB_DV_PulseCount[uc_ChannelNo] = 0;
        ChannelInfo_Manager::CCCV_TB_EnableCounter[uc_ChannelNo] = false;
        ChannelInfo_Manager::CCCV_TB_LastState[uc_ChannelNo] = STEP_CCCV_TB_CC;
        break;
    }
    case CT_IVCH_CURRENT_TB:  //CC TURBO BOOST BY ZC 06.04.2018
    {
        Step_Manager_task::ChannelCC_TBStatus[uc_ChannelNo] = STEP_CC_TB_START;
        break;
    }
    case CT_IVCH_DIGIV:
        if(m_Online_UpdataEnable[uc_ChannelNo])
        {
            St_Sche_Onlineupdata  * st_Onlineupdata = (St_Sche_Onlineupdata  *) &EthernetParse::m_btPackDataBuffer;
            if(!st_Onlineupdata->st_updata.m_Step_UpData[0].m_ucValid_StepCtrl)
                break;
        }

        Step_Manager_task::ChannelCVStatus[uc_ChannelNo] = STEP_CV_Init;
        Step_Manager_task::CCCVNoLimitCheckFlag[uc_ChannelNo] = 1;
        unsigned char uCurrRng = p_ThisControl->m_ucCurrentRange;    // for digital V, use current safety 设定值作为PID计算时的钳位值， 01.18.2017 DKQ

        Pid_Ctrl::fUpLimit[uc_ChannelNo] = UnifyCurrent(uc_ChannelNo,  0.953 * safety_check::Check_S[uc_ChannelNo].checkSCH_I.m_fHigh );//uCurrRng,

        Pid_Ctrl::fDnLimit[uc_ChannelNo] = UnifyCurrent(uc_ChannelNo,  0.953 * safety_check::Check_S[uc_ChannelNo].checkSCH_I.m_fLow);//uCurrRng,
        p_ThisInfo->bDigitalV = 1;
        ChannelInfo_Manager::SetPV_CV_StageCurrent(uc_ChannelNo);
        break;
// 	case CT_IVCH_DIGII:
// 		p_ThisInfo->bDigitalI = 1;
// 		break;
    case CT_IVCH_CPULSE:
    case CT_IVCH_VPULSE:
    {
        p_ThisInfo->bDigitalV = 0;
        unsigned short PulseID = p_ThisControl->m_CtrlValues.m_fControlValue[0];// if it is PulseID  dirui 2014/0806
        LoadPulseData(uc_ChannelNo, PulseID);
// 			if ( Pulse::Pulse_is_Running)  //如果有脉冲输出的话，尽快停止输出脉冲。
// 			{
// 				p_ThisInfo->ScheduleStatus = GOSTOP;
// 				Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_ACR_OVERTIME;
// 			}
        break;
    }
    case CT_IVCH_SIMU_V: //12.22.2013ADD //We Do Not Offer Voltage Simulation
    {
        p_ThisInfo->m_SimuInfo.m_BufIdx = SIMU_BUFFER::GetSimuIdx(p_ThisControl->m_CtrlValues.m_MD5);
        p_ThisInfo->m_SimuInfo.m_PointIdx = 0;

        if(p_ThisInfo->m_fResumeStepTime <= 0)
        {
            if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                p_ThisInfo->m_SimuInfo.m_bEnded = 0;
            else
                p_ThisInfo->m_SimuInfo.m_bEnded = 1;
        }
        else  //not start from 0s
        {
            unsigned long PointCount = SIMU_BUFFER::GetSimuPointCount(p_ThisInfo->m_SimuInfo.m_BufIdx);
            for(; p_ThisInfo->m_SimuInfo.m_PointIdx < PointCount; p_ThisInfo->m_SimuInfo.m_PointIdx++)
            {
                if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                {
                    if(p_ThisInfo->m_fResumeStepTime < p_ThisInfo->m_SimuInfo.m_fEndTime)
                        break;
                }
            }
            if(p_ThisInfo->m_SimuInfo.m_PointIdx >= PointCount)
                p_ThisInfo->m_SimuInfo.m_bEnded = 1;
            else
                p_ThisInfo->m_SimuInfo.m_bEnded = 0;
        }
        if(board_config::DacInfo.m_bHaveVoltageDac || ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[0].m_UnitGroupInfo.m_HeadChnRank >= 3)
            p_ThisInfo->bDigitalV = 0;    //12.22.2013ADD
        Step_Manager_task::bNewSimuLoaded[uc_ChannelNo] = 1;
        break;
    }
    case CT_IVCH_SIMU_I:     //12.22.2013ADD
    case CT_IVCH_SIMU_P:     //12.22.2013ADD
    case CT_IVCH_SIMU_L:     //12.22.2013ADD
    {
        p_ThisInfo->m_SimuInfo.m_bValid = 1;
        p_ThisInfo->m_SimuInfo.m_BufIdx = SIMU_BUFFER::GetSimuIdx(p_ThisControl->m_CtrlValues.m_MD5);
        unsigned long PointCount = SIMU_BUFFER::GetSimuPointCount(p_ThisInfo->m_SimuInfo.m_BufIdx);
        p_ThisInfo->m_SimuInfo.m_bEnded       = 0;
        p_ThisInfo->m_SimuInfo.m_bStageLogged = 0;
        p_ThisInfo->m_SimuInfo.m_bStarted     = 0;
        p_ThisInfo->m_SimuInfo.m_StageDacTime = CommonFunc::GetSystemTime();
        if(PointCount == 0)
        {   //没有Point的simulation是一个无效simulation文件，可以直接认为结束
            p_ThisInfo->m_SimuInfo.m_fValue = 0.0f;
            p_ThisInfo->m_SimuInfo.m_bEnded = 1;
            break;
        }
        unsigned char MySetting = p_ThisControl->m_ucCtrlValue_DataType[0];  //来源于上位机的STEP后 DropBox
        p_ThisInfo->m_SimuInfo.m_bRepeat      = ((MySetting == eRepeat)   || (MySetting == eRepeat_ExtraLog));
        p_ThisInfo->m_SimuInfo.m_bExtraLog    = ((MySetting == eRepeat_ExtraLog) || (MySetting == eNoRepeat_ExtraLog));
        p_ThisInfo->m_SimuInfo.m_LogInterval  = p_ThisControl->m_CtrlValues.m_fControlValue[4];
        p_ThisInfo->m_SimuInfo.m_bLogCycle    = 0;
        if(p_ThisInfo->m_SimuInfo.m_bRepeat)
        {   //这是一个simulation循环
            p_ThisInfo->m_SimuInfo.m_PointIdx   = PointCount - 1;
            SIMU_BUFFER::GetNextValue(uc_ChannelNo);
            p_ThisInfo->m_SimuInfo.m_fCycleTime = p_ThisInfo->m_SimuInfo.m_fEndTime;   //找到循环周期时间

            p_ThisInfo->m_SimuInfo.m_PointIdx   = 0;
            if(p_ThisInfo->m_fResumeStepTime <= 0)  //start命令,时间从0开始，simulation点数就从0开始
            {
                //Write_SimuCycleTime(uc_ChannelNo,0.0f);
                p_ThisInfo->m_SimuInfo.m_fStartTime = 0.0f;
                SIMU_BUFFER::GetNextValue(uc_ChannelNo);
                if(p_ThisInfo->m_SimuInfo.m_bExtraLog)
                    p_ThisInfo->m_SimuInfo.m_bLogCycle = 1;
                p_ThisInfo->m_SimuInfo.m_CycleIdx = 0;
                p_ThisInfo->m_SimuInfo.m_bLogLimitValid = 1;  //第一个cycle一定是LOG周期，无论是否真的LOG，log limit都会起作用
            }
            else
            {
                unsigned long long uTime1, uTime2;
                uTime1 = p_ThisInfo->m_fResumeStepTime * 10000;
                uTime2 = p_ThisInfo->m_SimuInfo.m_fCycleTime * 10000;
                p_ThisInfo->m_SimuInfo.m_CycleIdx = uTime1 / uTime2;
                uTime1 %= uTime2;
                if((p_ThisInfo->m_SimuInfo.m_CycleIdx % p_ThisInfo->m_SimuInfo.m_LogInterval) == 0)
                {
                    p_ThisInfo->m_SimuInfo.m_bLogLimitValid = 1;
                    if(p_ThisInfo->m_SimuInfo.m_bExtraLog)
                        p_ThisInfo->m_SimuInfo.m_bLogCycle = 1;
                }
                else
                    p_ThisInfo->m_SimuInfo.m_bLogLimitValid = 0;

                float fResumeTime = uTime1;// / 10000;
                fResumeTime /= 10000;
                p_ThisInfo->m_SimuInfo.m_fStartTime = fResumeTime;
                for(; p_ThisInfo->m_SimuInfo.m_PointIdx < PointCount; p_ThisInfo->m_SimuInfo.m_PointIdx++)
                {
                    if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                    {
                        if(fResumeTime < p_ThisInfo->m_SimuInfo.m_fEndTime)
                            break;
                    }
                }
            }
        }
        else
        {   //这是一个普通simulation，不必要做循环
            p_ThisInfo->m_SimuInfo.m_PointIdx = 0;
            if(p_ThisInfo->m_fResumeStepTime <= 0)
                SIMU_BUFFER::GetNextValue(uc_ChannelNo);
            else  //not start from 0s
            {
                for(; p_ThisInfo->m_SimuInfo.m_PointIdx < PointCount; p_ThisInfo->m_SimuInfo.m_PointIdx++)
                {
                    if(SIMU_BUFFER::GetNextValue(uc_ChannelNo))
                    {
                        if(p_ThisInfo->m_fResumeStepTime < p_ThisInfo->m_SimuInfo.m_fEndTime)
                            break;
                    }
                }
                if(p_ThisInfo->m_SimuInfo.m_PointIdx >= PointCount)
                {
                    p_ThisInfo->m_SimuInfo.m_bEnded = 1;
                    p_ThisInfo->m_SimuInfo.m_fValue = 0.0f;
                }
                else
                    p_ThisInfo->m_SimuInfo.m_bEnded = 0;
            }
            p_ThisInfo->m_SimuInfo.m_bLogCycle = p_ThisInfo->m_SimuInfo.m_bExtraLog;
            p_ThisInfo->m_SimuInfo.m_bLogLimitValid = 1;
        }
        Step_Manager_task::bNewSimuLoaded[uc_ChannelNo] = 1;
        Step_Manager_task::ACCURATE_IR_Size[uc_ChannelNo] = 0; //清除之前测试的IR值  add by ZC 01.04.2018
        Step_Manager_task::INACCURATE_IR_Size[uc_ChannelNo] = 0;
        Step_Manager_task::ACCURATE_IR_SizeFlag[uc_ChannelNo] = 0;
        Step_Manager_task::INACCURATE_IR_SizeFlag[uc_ChannelNo] = 0;
        break;
    }
    case CT_IVCH_C_CV:
    case CT_IVCH_V_CV:
    {
        int CvIdx = p_ThisControl->m_CtrlValues.m_fControlValue[0];
        if((CvIdx < 0) || (CvIdx >= IV_MAX_SCHEDULE_CV_NUMBER))
            break;
        signed char    sc_SchBufIdx = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
        if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
            break;
        if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
            break;
        memcpy(&p_ThisStep->m_Info[NonActiveStep].m_CvInfo.m_CvData,
               &ScheduleBuf[sc_SchBufIdx].m_CvData[CvIdx],
               sizeof(St_CvData));
        if(p_ThisControl->m_u16ControlType == CT_IVCH_V_CV)
        {
            if(board_config::DacInfo.m_bHaveVoltageDac)
                p_ThisInfo->bDigitalV = 0;
        }

        St_CvData * CvData = &ChannelStep[uc_ChannelNo].m_Info[NonActiveStep].m_CvInfo.m_CvData;
        p_ThisInfo->m_CvInfo.m_bEnded = false;
        p_ThisInfo->m_CvInfo.m_CycleIdx = 0;
        p_ThisInfo->m_CvInfo.m_StageIdx = 0;
        p_ThisInfo->m_CvInfo.m_fBase = CvData->m_Ctrl.m_fBase;
        if(p_ThisInfo->m_fResumeStepTime <= 0)
        {
            Write_CvCycleTime(uc_ChannelNo, 0.0f);
            p_ThisInfo->m_CvInfo.m_fOffset  = CvData->m_Stages[0].m_fStart;
            if(CvData->m_Stages[0].m_fEnd >= CvData->m_Stages[0].m_fStart)
                p_ThisInfo->m_CvInfo.m_fGain    = abs(CvData->m_Stages[0].m_fScanRate);
            else
                p_ThisInfo->m_CvInfo.m_fGain    = -abs(CvData->m_Stages[0].m_fScanRate);
            p_ThisInfo->m_CvInfo.m_fStgTime = abs((CvData->m_Stages[0].m_fEnd - CvData->m_Stages[0].m_fStart) / CvData->m_Stages[0].m_fScanRate);
        }
        else
        {
            float fCycleTime = 0;
            for(int StgIdx = 0; StgIdx < CvData->m_Ctrl.m_ucStageCount; StgIdx++)
                fCycleTime += abs((CvData->m_Stages[StgIdx].m_fStart - CvData->m_Stages[StgIdx].m_fEnd) / CvData->m_Stages[StgIdx].m_fScanRate);

            unsigned long long uTime1, uTime2;
            uTime1 = p_ThisInfo->m_fResumeStepTime * 10000;
            uTime2 = fCycleTime * 10000;
            p_ThisInfo->m_CvInfo.m_CycleIdx = uTime1 / uTime2;
            uTime1 %= uTime2;

            float fResumeTime = uTime1;// / 10000;
            fResumeTime /= 10000;
            float fStageTime = 0.0f;
            for(int StgIdx = 0; StgIdx < CvData->m_Ctrl.m_ucStageCount; StgIdx++)
            {
                p_ThisInfo->m_CvInfo.m_StageIdx = StgIdx;
                fStageTime = (CvData->m_Stages[StgIdx].m_fStart - CvData->m_Stages[StgIdx].m_fEnd) / CvData->m_Stages[StgIdx].m_fScanRate;
                fStageTime = abs(fStageTime);
                p_ThisInfo->m_CvInfo.m_fOffset  = CvData->m_Stages[StgIdx].m_fStart;
                if(CvData->m_Stages[StgIdx].m_fEnd >= CvData->m_Stages[StgIdx].m_fStart)
                    p_ThisInfo->m_CvInfo.m_fGain    = CvData->m_Stages[StgIdx].m_fScanRate;
                else
                    p_ThisInfo->m_CvInfo.m_fGain    = -CvData->m_Stages[StgIdx].m_fScanRate;
                p_ThisInfo->m_CvInfo.m_fStgTime = fStageTime;

                if(fResumeTime < fStageTime)
                    break;
                else
                    fResumeTime -= fStageTime;
            }
            Write_CvCycleTime(uc_ChannelNo, fResumeTime);

            if(p_ThisInfo->m_CvInfo.m_CycleIdx >= CvData->m_Ctrl.m_ucRepeatNumber)
                p_ThisInfo->m_CvInfo.m_bEnded = true;
        }
        break;
    }
    case CT_IVCH_WRITE_SMB_BLOCK:
    {
        unsigned short SMBBlockIndex = p_ThisControl->m_CtrlValues.m_fControlValue[1];
        memcpy(&ChannelStep[uc_ChannelNo].m_Info[NonActiveStep].m_SMBBlockInfo, &ScheduleBuf[sc_SchBufIdx].m_SMBBlockData[SMBBlockIndex], sizeof(St_SMBBlockData));
        memcpy(&Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetSMBBlockInfo, &ScheduleBuf[sc_SchBufIdx].m_SMBBlockData[SMBBlockIndex], sizeof(St_SMBBlockData));
    }
    break;

    case  CT_IVCH_CURRENT:     //add by qjm20191226  ??????????????
    case  CT_IVCH_CRAMP:
// 	case  CT_IVCH_CSTAIRCASE:
// 	case  CT_IVCH_CRATE:
    case  CT_IVCH_POWER:
    case  CT_IVCH_LOAD:
        if((!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_RisingEdge)&&
                (!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_Schedule) &&
                (!ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep) ||
                (CommonFunc::GetTimeTicks(&ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_TransStartTime) > 3.6E7 ))  // only every one hour need to detect the IR
        {
            float fSetValue,SampleV;
            fSetValue =Get_MetaVariable(uc_ChannelNo,
                                        p_ThisControl->m_ucCtrlValue_DataType[0],p_ThisControl->m_CtrlValueOperator[0],p_ThisControl->m_CtrlValueCoefficients[0],
                                        p_ThisControl->m_CtrlValues.m_fControlValue[0]);
            float fSampleCur=ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);
            float fIRangValue = 0.01f*ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,p_ThisControl->m_ucCurrentRange);
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started=true;

//			if(Serial2ParaMod_IsSingleChanMode())   // 20200701
//			     ChannelInfo[uc_ChannelNo].m_ucCurrentRange=p_ThisStep->m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
//			else
//			     ChannelInfo[uc_ChannelNo].m_ucCurrentRange=ChannelInfo_Manager::CurrRngPhysicalRange1;
            if(p_ThisControl->m_u16ControlType ==CT_IVCH_CURRENT )
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue =fSetValue;
            }
            if((p_ThisControl->m_u16ControlType)==CT_IVCH_POWER)
            {
                SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
                if (SampleV != 0)
                {
                    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue = fSetValue / SampleV; //??????
                }
                else
                {
                    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started =false;
                }
            }
            if((p_ThisControl->m_u16ControlType)==CT_IVCH_CRATE)
            {
                float fValue2 = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_MV_NorminalCapacity);
                fValue2 = MYABS(fValue2);
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue= fSetValue * fValue2; //??????
            }
            if((p_ThisControl->m_u16ControlType)==CT_IVCH_LOAD)
            {
                SampleV = ChannelInfo_Manager::IV_Buffer1[uc_ChannelNo].m_fVoltage;
                if(fSetValue != 0)
                    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue = - ::fabsf(SampleV / fSetValue);       //???????
                else
                {
                    ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started =false;
                }
            }
// 			  if(((p_ThisControl->m_u16ControlType)==CT_IVCH_CRAMP) ||((p_ThisControl->m_u16ControlType)==CT_IVCH_CSTAIRCASE))
// 				  ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started =false;
            if(abs(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_StepCtrlCurValue-fSampleCur)<fIRangValue)
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started=false;
            if(ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started )
            {
// 					ChannelInfo[uc_ChannelNo].m_bNeedFilter =0;
                SetFilterStatus(uc_ChannelNo, false);
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_TransStartTime = CommonFunc::GetSystemTime();
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_TransFinished = false;//DK 210714
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Counter=0;
                ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_VolUpdateFlag =false;
                ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_voltage =ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Voltage);
                ChannelInfo_Manager::m_IV_Pair[uc_ChannelNo].m_current = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo, MetaCode_PV_Current);
            }
            else
            {
                ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_ZeroTest_Enable = true;
            }
        }
        else
        {
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_RisingEdge_Started =false;
            ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_ZeroTest_Enable = false;
        }
        break;

    default:
        if(board_config::DacInfo.m_bHaveVoltageDac)
        {
            p_ThisInfo->bDigitalV = 0;
// 			p_ThisInfo->bDigitalI = 0;
        }
        break;
    }


//    if(   (p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT)
//            || (p_ThisControl->m_u16ControlType == CT_IVCH_VOLTAGE)
    if(	  (p_ThisControl->m_u16ControlType == CT_IVCH_VOLTAGE)
            || (p_ThisControl->m_u16ControlType == CT_IVCH_DIGIV)
            || (p_ThisControl->m_u16ControlType == CT_IVCH_POWER)
            || (p_ThisControl->m_u16ControlType == CT_IVCH_CCCV)
            || (p_ThisControl->m_u16ControlType == CT_IVCH_CCCV_WRM))
//             || (p_ThisControl->m_u16ControlType == CT_IVCH_CCCV_TB)
//             || (p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT_TB)	)
    {   //for current dynamic control
        if(p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_CANBMS)
            p_ThisInfo->m_bDynamicControl = true;
        else if(p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_FORMULA || p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_ADVFORMULA)
        {
            int FormulaIdx = p_ThisControl->m_CtrlValues.m_fControlValue[0];
//             if( IfFormulaContain(&ScheduleBuf[sc_SchBufIdx].m_FormulaData[FormulaIdx], MP_DATA_TYPE_CANBMS) )
            if( IfFormulaContain(sc_SchBufIdx, FormulaIdx, MP_DATA_TYPE_CANBMS,p_ThisControl->m_ucCtrlValue_DataType[0]) )
                p_ThisInfo->m_bDynamicControl = true;
        }
        else if(p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_SMBBMS)
        {
            if((p_ThisControl->m_u16ControlType == CT_IVCH_CCCV) || (p_ThisControl->m_u16ControlType == CT_IVCH_CCCV_WRM) || (p_ThisControl->m_u16ControlType == CT_IVCH_CCCV_TB))
                p_ThisInfo->m_bDynamicControl = true;  //change true to false,by hepeiqing 181215,for sumsung's request
            else
                p_ThisInfo->m_bDynamicControl = true;
        }
    }

    if((p_ThisControl->m_u16ControlType == CT_IVCH_CCCV_TB) || (p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT_TB))
    {
        p_ThisInfo->m_bDynamicControl = true;
        p_ThisInfo->m_bNoNeedFineTune_flag = true;
    }

    if(p_ThisControl->m_u16ControlType == CT_IVCH_CURRENT)
    {
        if((p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_CANBMS) || (p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_METAVALUE) || (p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_SMBBMS))
        {
            p_ThisInfo->m_bDynamicControl = true;
            p_ThisInfo->m_bNoNeedFineTune_flag = true;
        }
        else if(p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_FORMULA || p_ThisControl->m_ucCtrlValue_DataType[0] == MP_DATA_TYPE_ADVFORMULA)
        {
            int FormulaIdx = p_ThisControl->m_CtrlValues.m_fControlValue[0];
            if( IfFormulaContain(sc_SchBufIdx, FormulaIdx, MP_DATA_TYPE_CANBMS,p_ThisControl->m_ucCtrlValue_DataType[0]) || IfFormulaContain(sc_SchBufIdx, FormulaIdx, MP_DATA_TYPE_METAVALUE,p_ThisControl->m_ucCtrlValue_DataType[0]))
            {
                p_ThisInfo->m_bDynamicControl = true;
                p_ThisInfo->m_bNoNeedFineTune_flag = true;
            }
        }
        ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo] = false;
    }

    if((p_ThisControl->m_u16ControlType == CT_IVCH_CRAMP) || (p_ThisControl->m_u16ControlType == CT_IVCH_SIMU_I))//20190808 dyy
    {
        p_ThisInfo->m_bDynamicControl = true;			//20190808 dyy
        p_ThisInfo->m_bNoNeedFineTune_flag = true;
        ChannelInfo_Manager::uc_useMetaCode_MV_UD[uc_ChannelNo] = false;
    }


    BurstModeSampling::BurstModeSamplingCheck(uc_ChannelNo, u16_StepID); //  2017.6.8  yy
    //CheckJumpStep_ToProtectCycle(uc_ChannelNo,u16_StepID);
//	m_StepIndexUp[uc_ChannelNo] = u16_StepID;
    Step_Manager_task::uc_CurCoarseTuneSampleCnt[uc_ChannelNo]=0;
    return 1;
}
//#if SUPPORT_PARALLEL == 1
void ChannelInfo_Manager::SetIntraUnitParallelSlaveStepCtrlType(unsigned char uc_ChannelNo,  unsigned char NonActiveStep, UWord16 SlaveCtrlType)
{
    if (!Serial2ParaMod_IsIntraUnitMasterChannel(uc_ChannelNo))
        return;

    unsigned short uChanIdx, uLastChanIdx;
    uLastChanIdx = FindGroupLastChanIdx(uc_ChannelNo);

    for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uLastChanIdx; uChanIdx++  )
    {
        ChannelStep[uChanIdx].m_Info[NonActiveStep].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType = SlaveCtrlType;
    }

    //TODO: if we have to set up slave from StepMgr level, due to possible different ctrl type in master and slaves, have to set up m_Ctrl members here more than ControlType
}

void ChannelInfo_Manager::CopyChannelInfoFlagToSlaveChannel(unsigned char uc_ChannelNo)
{
    if (!Serial2ParaMod_IsMasterChannel(uc_ChannelNo))
        return;

    unsigned short uChanIdx, uLastChanIdx;
    uLastChanIdx = FindGroupLastChanIdx(uc_ChannelNo);

    for (uChanIdx = uc_ChannelNo + 1; uChanIdx <= uLastChanIdx; uChanIdx++  )
    {
// 		ChannelInfo[uChanIdx].bDigitalI = ChannelInfo[uc_ChannelNo].bDigitalI;
        ChannelInfo[uChanIdx].bDigitalV = ChannelInfo[uc_ChannelNo].bDigitalV;
        ChannelInfo[uChanIdx].m_bCVstage = ChannelInfo[uc_ChannelNo].m_bCVstage;
    }
    return;
}
//#endif
/*拆分出需要aux的limit和formula
//add by junjie 2014/1/6
//Left DataType  Comparison Sign  Right DataType
//aux Meta          __             IV Meta
//IV Meta           __             aux Meta
//IV Meta           __             Inc Aux Formul
//Inc Aux Formula   __             IV Meta
//Aux Meta          __             IV Formula
//IV Formula        __             Aux Meta
*/

//#if SUPPORT_AUX == 1
unsigned char Channel_MapAuxInfo::Send_RequestData(unsigned char uc_ChannelNo, unsigned char DataType, unsigned short Aux_VirtualIdx)
{
    unsigned char AuxType = MAXAUXTYPE;//zyx 2107121
    unsigned char DataFlag = ORIG_DATA;

    switch(DataType)
    {
    case MP_DATA_TYPE_AUX_T:
    case MP_DATA_TYPE_AUX_dTdt:
        AuxType = AUX_TEMPERATURE;
        if(DataType == MP_DATA_TYPE_AUX_dTdt)
            DataFlag = DX_DT_DATA;
        //else if(DataType == MP_DATA_TYPE_AUX_d2Tdt2)
        //    DataFlag = D2X_DT2_DATA;
        break;
    case MP_DATA_TYPE_AUX_V:
    case MP_DATA_TYPE_AUX_dVdt:
        AuxType = AUX_VOLTAGE;
        if(DataType == MP_DATA_TYPE_AUX_dVdt)
            DataFlag = DX_DT_DATA;
        //else if(DataType == MP_DATA_TYPE_AUX_d2Vdt2)
        //    DataFlag = D2X_DT2_DATA;
        break;
    case MP_DATA_TYPE_AUX_P:
    case MP_DATA_TYPE_AUX_dPdt:
        AuxType = AUX_PRESSURE;
        if(DataType == MP_DATA_TYPE_AUX_dPdt)
            DataFlag = DX_DT_DATA;
        //else if(DataType == MP_DATA_TYPE_AUX_d2Pdt2)
        //    DataFlag = D2X_DT2_DATA;
        break;
    case MP_DATA_TYPE_AUX_DI:
        AuxType = AUX_DI;
        break;
    case MP_DATA_TYPE_AUX_DO:
        AuxType = AUX_DO;
        break;
    case MP_DATA_TYPE_AUX_FlowRate:          // 用于温控器湿度   20150609 yang
    case MP_DATA_TYPE_AUX_dFRdt:
        AuxType = AUX_FLOW_RATE;
        if(DataType == MP_DATA_TYPE_AUX_dFRdt)
            DataFlag = DX_DT_DATA;
        //else if(DataType == MP_DATA_TYPE_AUX_d2FRdt2)
        //    DataFlag = D2X_DT2_DATA;
        break;

    case MP_DATA_TYPE_AUX_Humidity:          // 用于新温控器湿度   zyx
    case MP_DATA_TYPE_AUX_dHumdt:
        AuxType = AUX_HUMIDITY;
        if(DataType == MP_DATA_TYPE_AUX_dHumdt)
            DataFlag = DX_DT_DATA;
        //else if(DataType == MP_DATA_TYPE_AUX_d2FRdt2)
        //    DataFlag = D2X_DT2_DATA;
        break;
    case MP_DATA_TYPE_EC:
        AuxType = AUX_EXTERNALCHARGE;
        break;
    default:
        AuxType = MAXAUXTYPE;//NUM_OF_AUX_CHAN_TYPE_NULL;
        break;
    }
    if(AuxType < MAXAUXTYPE)//NUM_OF_AUX_CHAN_TYPE_NULL)
    {
        if(!m_AuxDataRequested[uc_ChannelNo][AuxType][Aux_VirtualIdx ])  //该auxchn没有被请求过
        {
            ST_REQUEST_AUXDATA RptDataFormat;
            RptDataFormat.m_aux_type = AuxType;
            RptDataFormat.m_IV_ChnNo = uc_ChannelNo;
            RptDataFormat.m_aux_virtual_chn = Aux_VirtualIdx;
            RptDataFormat.m_single_or_repeat = 0;
            RptDataFormat.m_data_type = DataFlag;
            RptDataFormat.m_TimeDelta = LOG_TIME_100MS;
            RptDataFormat.m_MetaDelta = 0;
            CANBusParse::IV_RequestDataToAUX(RptDataFormat);
            m_AuxDataRequested[uc_ChannelNo][AuxType][Aux_VirtualIdx] = 1;
            m_AuxData[uc_ChannelNo][AuxType][Aux_VirtualIdx].updated_aux_data.m_Char = 0x00;
        }
    }
    return 1;
}

void Channel_MapAuxInfo::checkAuxTypeInFormula(unsigned char uc_ChannelNo, signed char schBufIdx, signed char formulaIdx,unsigned char ControlValue_DataType)
{

    if((schBufIdx >= 0) && (formulaIdx >= 0))
    {
        if(ControlValue_DataType == MP_DATA_TYPE_FORMULA)
        {
            float* pf_X = &ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_fX1;
            for(unsigned char i = 0; i < 8; i++)
            {
                if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i] != MP_DATA_TYPE_FORMULA)
                {
                    if(BuiltIn_DataManager::b_BuiltInAux)
                    {
                        if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i] == MP_DATA_TYPE_AUX_T)
                        {
                            if(*pf_X < BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV)
                            {
                                pf_X++;
                                continue;
                            }
                        }
                        else if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i] == MP_DATA_TYPE_AUX_V)
                        {
                            if(*pf_X < BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV)
                            {
                                pf_X++;
                                continue;
                            }
                        }
                    }
                    Send_RequestData(uc_ChannelNo,	ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_FormulaData[formulaIdx].m_ucDataType[i],	*pf_X);
                }
                else
                {
                    checkAuxTypeInFormula(uc_ChannelNo, schBufIdx, *pf_X,ControlValue_DataType);
                }
                pf_X++;
            }

        }
        else
        {
            St_AdvFormulaData* pf_Node = &ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_AdvFormulaData[0];

            for(unsigned short i = 0; i < ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_u16DataCount; i++)
            {
                if(BuiltIn_DataManager::b_BuiltInAux)
                {
                    if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_AdvFormulaData[i].m_ucDataType == MP_DATA_TYPE_AUX_T)
                    {
                        if(pf_Node->m_fvaule < BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV)
                        {
                            pf_Node++;
                            continue;
                        }
                    }
                    else if(ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_AdvFormulaData[i].m_ucDataType == MP_DATA_TYPE_AUX_V)
                    {
                        if(pf_Node->m_fvaule < BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV)
                        {
                            pf_Node++;
                            continue;
                        }
                    }
                }
                Send_RequestData(uc_ChannelNo,	ChannelInfo_Manager::ScheduleBuf[schBufIdx].m_AdvFormula[formulaIdx].m_AdvFormulaData[i].m_ucDataType,	(unsigned short)pf_Node->m_fvaule);
                pf_Node++;
            }
        }
    }
}

unsigned char Channel_MapAuxInfo::Load_AuxStepData(unsigned char uc_ChannelNo, St_Limit AuxLimit)
{
    if(AuxLimit.m_ucLeftValue_DataType == MP_DATA_TYPE_FORMULA || AuxLimit.m_ucLeftValue_DataType == MP_DATA_TYPE_ADVFORMULA)
    {   //公式的巡检也可以考虑放到Send_RequestData函数里，只不过这样需要实现函数嵌套调用，不爽
        signed char formulaIdx = AuxLimit.m_fLeftValue;
        signed char schBufIdx  = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
        if((schBufIdx >= 0) && (schBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER) && (formulaIdx >= 0))
        {
            checkAuxTypeInFormula(uc_ChannelNo, schBufIdx, formulaIdx,AuxLimit.m_ucLeftValue_DataType);

        }
    }
    else
    {
        Send_RequestData(uc_ChannelNo, AuxLimit.m_ucLeftValue_DataType, AuxLimit.m_fLeftValue);
    }

    if(AuxLimit.m_ucRightValue_DataType == MP_DATA_TYPE_FORMULA || AuxLimit.m_ucRightValue_DataType == MP_DATA_TYPE_ADVFORMULA)
    {
        signed char formulaIdx = AuxLimit.m_fRightValue;
        signed char schBufIdx  = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
        if((schBufIdx >= 0) && (schBufIdx < IV_MAX_SCHEDULE_BUFFER_NUMBER) && (formulaIdx >= 0))
        {
            checkAuxTypeInFormula(uc_ChannelNo, schBufIdx, formulaIdx,AuxLimit.m_ucRightValue_DataType);

        }
    }
    else
    {
        Send_RequestData(uc_ChannelNo, AuxLimit.m_ucRightValue_DataType, AuxLimit.m_fRightValue);
    }

    return 1;
}
//#endif

int ChannelInfo_Manager::LoadPulseData(unsigned char uc_ChannelNo, unsigned short u16_PulseID)
{
    signed char    sc_SchBufIdx = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    unsigned char  ActiveStep = !ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return -1;     //schedule buffer index 指针越界，返回-1;

    if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
        return -1;     //schedule buffer 不合法，返回-1;

    memcpy(&ChannelStep[uc_ChannelNo].m_Info[ActiveStep].m_PulseInfo.m_Data,
           &ScheduleBuf[sc_SchBufIdx].m_PulseData[u16_PulseID],
           sizeof(St_PulseData));
    ChannelStep[uc_ChannelNo].m_Info[ActiveStep].m_PulseInfo.m_PulseID = u16_PulseID;  //Add by DKQ 08.20.2014

    //Pulse::PulseInit(uc_ChannelNo);  //Add DKQ 08.20.2014
    return 1;
}
float ChannelInfo_Manager::GetPulseMaxAmplitude(unsigned char uc_ChannelNo, unsigned short u16_PulseID)
{
    signed char    sc_SchBufIdx = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    unsigned char  ActiveStep = !ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return 0;     //schedule buffer index 指针越界，返回-1;

    if(!ScheduleBuf[sc_SchBufIdx].m_bValid)
        return 0;     //schedule buffer 不合法，返回-1;

    float fMaxValue = -100000000000;
    float fValue = 0;
    for(int StgIdx = 0; StgIdx < ScheduleBuf[sc_SchBufIdx].m_PulseData[u16_PulseID].m_uStageNum; StgIdx++)
    {
        fValue = ScheduleBuf[sc_SchBufIdx].m_PulseData[u16_PulseID].m_StageAmplitude[StgIdx];
        fValue = ::abs(fValue);
        if(fValue > fMaxValue)
            fMaxValue = fValue;
    }
    return fMaxValue;
}
//和schedule、simulation相关的 private方法集合：
signed char ChannelInfo_Manager::GetEmptyScheduleBuf()
{
    signed char        i, sc_BufIdx = -1;
    unsigned long      u32_OldestAbandonTime = 0xFFFFFFFF;

    for (i = 0; i < IV_MAX_SCHEDULE_BUFFER_NUMBER; i++)
    {
        if (!ScheduleBuf[i].m_bValid)      //这是一个空buffer可以直接用了
        {
            return i;
        }
    }

    for (i = 0; i < IV_MAX_SCHEDULE_BUFFER_NUMBER; i++)
    {
        if (ScheduleBuf[i].m_bValid)                      //buffer不为空，
        {
            if (!ScheduleBuf[i].m_u32IvRegister)            //buffer没有被IV通道使用，这个可以考虑动一动
            {
                if (ScheduleBuf[i].m_AbandonTime.Second < u32_OldestAbandonTime)
                {
                    u32_OldestAbandonTime = ScheduleBuf[i].m_AbandonTime.Second;
                    sc_BufIdx = i;
                }
            }
            else                                   //buffer已经被IV通道使用了，这个不能动
                continue;
        }
    }

    return sc_BufIdx;
}
signed char ChannelInfo_Manager::FindScheduleBuf(St_MD5 ScheduleID)
{
    for (signed char i = 0; i < IV_MAX_SCHEDULE_BUFFER_NUMBER; i++)
    {
        if (ScheduleBuf[i].m_bValid)      //合法的buffer，比较是否为相同的MD5
        {
            if(!memcmp((void *)&ScheduleBuf[i].m_ID, (void *)&ScheduleID, 16))
                return i;
        }
    }

    return -1;   //表示没有找到相同的Schedule ID
}

signed char ChannelInfo_Manager::GetEmptyPulseBuf()
{
    signed char        i, sc_BufIdx = -1;
    unsigned long      u32_OldestAbandonTime = 0xFFFFFFFF;

    for (i = 0; i < IV_MAX_PULSE_BUFFER_NUMBER; i++)
    {
        if (!PulseBuf[i].m_bValid)      //这是一个空buffer可以直接用了
        {
            return i;
        }
        else                               //buffer不为空，
        {
            if (!PulseBuf[i].m_u32IvRegister)            //buffer没有被IV通道使用，这个可以考虑动一动
            {
                if (PulseBuf[i].m_AbandonTime.Second < u32_OldestAbandonTime)
                {
                    u32_OldestAbandonTime = PulseBuf[i].m_AbandonTime.Second;
                    sc_BufIdx = i;
                }
            }
            else                                   //buffer已经被IV通道使用了，这个不能动
                continue;
        }
    }

    return sc_BufIdx;
}
signed char ChannelInfo_Manager::FindPulseBuf(St_MD5 PulseID)
{
    for (signed char i = 0; i < IV_MAX_PULSE_BUFFER_NUMBER; i++)
    {
        if (PulseBuf[i].m_bValid)      //合法的buffer，比较是否为相同的MD5
        {
            if(!memcmp((void *)&PulseBuf[i].m_ID, (void *)&PulseID, 16))
                return i;
        }
    }

    return -1;   //表示没有找到相同的Schedule ID
}

float ChannelInfo_Manager::CalculateEVEN(float x)	//将正(负)数向上(下)舍入到最近的偶数
{
    int iResult = 0;
    if(x >= 0)
    {
        iResult = (int)ceilf(x);
        x = (float)iResult;
        return ((iResult & 0x01) ? x+1 : x);
    }
    else
    {
        iResult = (int)floorf(x);
        x = (float)iResult;
        return ((abs(iResult) & 0x01) ? x-1 :x);	//负数是以补码形式存在内存，这里取绝对值，避免出错

    }

}

float ChannelInfo_Manager::CalculateODD(float x)	//将正(负)数向上(下)舍入到最近的奇数
{
    int iResult = 0;
    if(x >= 0)
    {
        iResult = (int)ceilf(x);
        x = (float)iResult;
        return ((iResult & 0x01) ? x : x+1);
    }
    else
    {
        iResult = (int)floorf(x);
        x = (float)iResult;
        return ((abs(iResult) & 0x01) ? x :x-1);	//负数是以补码形式存在内存，这里取绝对值，避免出错

    }

}


float ChannelInfo_Manager::Function(unsigned char FUNtype, float dX)
{
    static bool bRandomSeeded = false;

    double dResult;
    int i, iResult;

    switch( FUNtype )
    {
    case MP_MF_NONE:

        dResult = dX;
        break;

    case MP_MF_ABS:

        dResult = ::fabsf( dX );
        break;

    case MP_MF_ACOS:

        dResult = ::acosf( dX );
        break;

    case MP_MF_ASIN:

        dResult = ::asinf( dX );
        break;

    case MP_MF_ATAN:

        dResult = ::atanf( dX );
        break;

    case MP_MF_CEIL:

        dResult = ::ceilf( dX );
        break;

    case MP_MF_COS:

        dResult = ::cosf( dX );
        break;

    case MP_MF_CUBIC:

        dResult = dX * dX * dX;
        break;

    case MP_MF_EVEN:

        dResult = CalculateEVEN(dX);	//将正(负)数向上(下)舍入到最近的偶数
        break;

    case MP_MF_EXP:

        dResult = ::expf( dX );
        break;

    case MP_MF_FACT:

        iResult = 1;

        for( i = 2; i <= int(dX); i++) {
            iResult *= i;
        }

        dResult = float( iResult );

        break;

    case MP_MF_FLOOR:

        dResult = ::floorf( dX );
        break;

    case MP_MF_INT:

        if( dX > 0.0f ) {
            dResult = float( int( dX + 0.5f ) );
        }
        else {
            dResult = float( int( dX - 0.5f ) );
        }
        break;

    case MP_MF_LN:

        dResult = ::logf( dX );
        break;

    case MP_MF_LOG10:

        dResult = ::log10f( dX );
        break;

    case MP_MF_ODD:

        dResult = CalculateODD(dX);		//将正(负)数向上(下)舍入到最近的奇数
        break;

    case MP_MF_RANDOM:

        if( ! bRandomSeeded ) {
            bRandomSeeded = true;
            srand(  (unsigned int)GetSystemTimeInSecond() );
        }

        dResult = (double( rand() ) / 100.0) * (double( rand() ) / 100.0f);
        dResult = ( dX != 0.0f ) ? fmodf(dResult, fabsf(dX)) : 0.0f;

        //
        // This check is just for safety,
        // it should never evaluate true.
        //
        if( dResult > fabsf(dX) ) {
            dResult = fabsf(dX);
        }

        //
        // Negate the value, if needed.
        //
        if( dX < 0.0f ) {
            dResult = -dResult;
        }

        break;

    case MP_MF_SIGN:

        if( dX > 0.0f ) {
            dResult =  1.0f;
        }
        else if( dX < 0.0f ) {
            dResult = -1.0f;
        }
        else {
            dResult =  0.0f;
        }

        break;

    case MP_MF_SIN:

        dResult = ::sinf( dX );
        break;

    case MP_MF_SQR:

        dResult = dX * dX;
        break;

    case MP_MF_SQRT:

        dResult = ::sqrtf(dX);
        break;

    case MP_MF_TAN:

        dResult = ::tanf( dX );
        break;

    case MP_MF_TRUNC:

        dResult = float( int( dX ) );
        break;

    default:
        dResult = 0;
        break;
    }

    return( dResult );
}
float ChannelInfo_Manager::Operation(unsigned char Operator, float x, float y)
{
    switch(Operator)
    {
    case MP_MO_ADD:
        return(x + y);
    case MP_MO_SUBTRACT:
        return(x - y);
    case MP_MO_MULTIPLY:
        return(x * y);
    case MP_MO_DIVIDE:
        if(y == 0)
            return 0;
        else
            return(x / y);
    case MP_MO_MODULUS:
//        if(y == 0)
//            y = 1;
//        return((unsigned long)x % (unsigned long)y);
        return fmodf(x,y);
    default:
        return(x + y);
    }
}

///////////////////////////////////////////////////////////////////////////DIGITAL CONTROL & PID/////////////////////////////////////////////////////////////////////////////////
// void ChannelInfo_Manager::Init_PidCoef( unsigned char  uc_ChannelNo )       //04.23.2013
// {
// 	m_Pid_Coef[uc_ChannelNo].Init();

// 	return 1;
// }
float ChannelInfo_Manager::UnifyCurrent(unsigned char uc_ChannelNo,  float fCurrentValue)    //04.23.2013 //unsigned char IRange,
{
    float f_Temp = 0.0f;

    float f_Bound = ChannelInfo_Manager::Get_PID_MaxCurrentRange(uc_ChannelNo);

    //f_Bound = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[IRange].m_Bound.m_High;     // I-RangeMax
    if(f_Bound != 0)
        f_Temp = fCurrentValue / f_Bound;

    return f_Temp;
}
// float ChannelInfo_Manager::UnifyVoltage(unsigned char uc_ChannelNo, unsigned char VRange,  float fVoltageValue)  //04.23.2013
// {
//     float   f_Temp = 0.0f;
//     float f_Bound;

//     f_Bound = BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VRange].m_Bound.m_High;     // V-RangeMax
//     if(f_Bound != 0)
//         f_Temp = fVoltageValue / f_Bound;

//     return f_Temp;
// }
float ChannelInfo_Manager::DeUnifyCurrent(unsigned char uc_ChannelNo, float fUniCurrentValue)  //04.23.2013 // unsigned char IRange,
{

    float f_Temp = 0.0f;

    float f_Bound = ChannelInfo_Manager::Get_PID_MaxCurrentRange(uc_ChannelNo);;

    //f_Bound = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[IRange].m_Bound.m_High;     // I-RangeMax

    f_Temp = fUniCurrentValue * f_Bound;//* ChannelInfo_Manager::GetTotalChanNumOfParaGroup(uc_ChannelNo); //zyx 190402

    return f_Temp;
}
float ChannelInfo_Manager::GetUnifyCurrent( unsigned char  uc_ChannelNo )       //04.23.2013 //, unsigned char IRange
{
    return UnifyCurrent(uc_ChannelNo, IV_Buffer1[uc_ChannelNo].m_fCurrent );// IRange,
}
// float ChannelInfo_Manager::GetUnifyVoltage( unsigned char  uc_ChannelNo, unsigned char VRange  )     //04.23.2013
// {
//     return UnifyVoltage( uc_ChannelNo, VRange, IV_Buffer1[uc_ChannelNo].m_fVoltage );
// }
/*float ChannelInfo_Manager::GetFloatCurrent( unsigned char  uc_ChannelNo)                 //04.23.2013
{
	return IV_Buffer1[uc_ChannelNo].m_fCurrent;
	// return 	MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
}
float ChannelInfo_Manager::GetFloatVoltage( unsigned char  uc_ChannelNo)                  //04.23.2013
{
	return IV_Buffer1[uc_ChannelNo].m_fVoltage;
	//return MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
}*/
void ChannelInfo_Manager::PrepareIBasedPID(unsigned char uc_ChannelNo, unsigned char IRange, unsigned char VRange)            //04.23.2013
{
    switch(  m_Pid_Coef[ uc_ChannelNo ].m_PidTypeFlag  )
    {

    case PID_V:
        m_Pid_Coef[ uc_ChannelNo ].m_SampleValue_1B = IV_Buffer1[uc_ChannelNo].m_fVoltage;//GetUnifyVoltage( uc_ChannelNo, VRange );
        break;
//     case PID_I:
//         m_Pid_Coef[ uc_ChannelNo ].m_SampleValue_1B = GetUnifyCurrent( uc_ChannelNo );//, IRange
//         break;
    default:
        break;
    }
}

/*void ChannelInfo_Manager::MarkSampleDataAsFresh( unsigned char uc_ChannelNo )    //04.23.2013
{
	//m_DataFreshForPower[ uc_ChannelNo] = true;
	//m_DataFreshForLoad[ uc_ChannelNo] = true;
	//m_DataFreshForVoltagePID[ uc_ChannelNo] = true;
	//m_DataFreshForVoltageClamp[ uc_ChannelNo] = true;
	m_DataFreshForPID[ uc_ChannelNo] = true;
}*/

/*void ChannelInfo_Manager::Unipolar_Convert(float * f_Set,unsigned char uc_abs)
{
    float f_Value = *f_Set;
    f_Value -= DAC_OUT_PUT_ZERO;   //  双边变成单边 0V---2.5V
    if(uc_abs)
	    f_Value = abs(f_Value);
    *f_Set = f_Value;
}

unsigned char ChannelInfo_Manager::Unipolar_Rang(unsigned char uc_ChannelNo,unsigned char range)
{
    if((Step_Manager_task::Unipolar.Charge[uc_ChannelNo] == 0)   // 1：充电；0：放电
        && (range < CURRENT_RANGE_5) )
    {
        range += CURRENT_RANGE_5;
    }
    return range;
}

unsigned long ChannelInfo_Manager::Unipolar_CalibrateCurrentDAC(unsigned char uc_ChannelNo,unsigned char range,float f_Current)
{
	float f_gain,f_offset;
	unsigned long u32_Current;

    range = Unipolar_Rang(uc_ChannelNo,range);

    if((range < CURRENT_RANGE_5)) // 解决负向5、6、7、8量程时设小电流输出大电流，反向问题。
        f_Current = abs(f_Current);
    f_gain = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[range].m_Dac.m_Gain;
    f_offset = BoardCFG.m_Channel[uc_ChannelNo].m_IRange[range].m_Dac.m_Offset;
    f_Current = f_Current*f_gain + f_offset;

    if(range < CURRENT_RANGE_5)
     {
           if(f_Current >= DAC_OUT_PUT_ZERO)
             {
                     Unipolar_Convert(&f_Current,0);   //  双边变成单边 0V---2.5V
             }
             else
             {
                    f_Current = CONVERTER_MIN;    //  解决接近0时，DAC值回头问题。
             }
     }
     else
     {
        //if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        //{
            Unipolar_Convert(&f_Current,0);   //  双边变成单边 0V---2.5V
        //}
     }
     f_Current = abs(f_Current);
     	u32_Current = f_Current;

	if(u32_Current > CONVERTER_MAX)
		u32_Current = CONVERTER_MAX;
     return u32_Current;
}
*/
float ChannelInfo_Manager::RC_FilterOutput(unsigned char uc_ChannelNo, float CurrentValue, float *HistoryValue, float FilterCoefficient, float FilterDeepth)
{
    m_RC_OutputValue[uc_ChannelNo] = CurrentValue * FilterCoefficient / FilterDeepth + *HistoryValue * (FilterDeepth - FilterCoefficient) / FilterDeepth;
    *HistoryValue = m_RC_OutputValue[uc_ChannelNo];
    return m_RC_OutputValue[uc_ChannelNo];
}
#if  USE_AS_AUXMCU == 0  //mits8
void  ChannelInfo_Manager::SetDataFlag(UWord16 uw16_ChannelNumber, unsigned long setBit) //180309 zyx  //mits8
{
    m_DataFlag[uw16_ChannelNumber] |= setBit;
}
void  ChannelInfo_Manager::ClearDataFlag(UWord16 uw16_ChannelNumber) //mits8
{
    m_DataFlag[uw16_ChannelNumber] = 0;//&= ~setBit;
}
bool ChannelInfo_Manager::IsSpecialPoint(UWord16 uw16_ChannelNumber, unsigned long PointType) //mits8
{
    if((m_DataFlag[uw16_ChannelNumber] & PointType) == PointType)
        return true;
    else
        return false;
}
void ChannelInfo_Manager::SetFilterStatus(unsigned char uc_ChannelNo, bool Status)
{
    b_NeedFilter[uc_ChannelNo] = Status;
}
void ChannelInfo_Manager::RecoverFilter(unsigned char uc_ChannelNo)
{
    b_NeedFilter[uc_ChannelNo] = IVSDL::IsChnNeedFilter[uc_ChannelNo];
}
bool ChannelInfo_Manager::GetFilterStatus(unsigned char uc_ChannelNo)
{
    return b_NeedFilter[uc_ChannelNo];
}
void ChannelInfo_Manager::GetSpecialPointStatus(UWord16 uw16_ChannelNumber)
{
    if(ChannelInfo[uw16_ChannelNumber].m_bDynamicControl == true)
        SetDataFlag(uw16_ChannelNumber, DYNAMIC_CONTROL_ON);
    if(GetFilterStatus(uw16_ChannelNumber) == false)
        SetDataFlag(uw16_ChannelNumber, CURRENT_FILTER_OFF);
    if(Step_Manager_task::b_CurCoarseTuneFinished[uw16_ChannelNumber] == false)
        SetDataFlag(uw16_ChannelNumber, CURRENT_COARSETUNE_OFF);
    if(Step_Manager_task::b_FineTuneFinished[uw16_ChannelNumber] == false)
        SetDataFlag(uw16_ChannelNumber, CURRENT_FINETUNE_OFF);
    if((m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_Schedule == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_Relax == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_IRStep == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_CCCV_TB == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_RisingEdge == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_CCCV == 0)\
            &&(m_IR_Info[uw16_ChannelNumber].IR_From.m_bIR_from_Estimate == 0))
    {
        if((ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_I)\
                || (ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_Info[ChannelInfo_Manager::ChannelStep[uw16_ChannelNumber].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType == CT_IVCH_SIMU_P))
        {
            if((Step_Manager_task::ACCURATE_IR[uw16_ChannelNumber] == 0)\
                    &&(Step_Manager_task::INACCURATE_IR[uw16_ChannelNumber] == 0))
                SetDataFlag(uw16_ChannelNumber, IR_VALUE_NULL);
        }
        else
            SetDataFlag(uw16_ChannelNumber, IR_VALUE_NULL);
    }
    if(Is_Current_SoftWareClamp_Work[uw16_ChannelNumber] == true)
        SetDataFlag(uw16_ChannelNumber, CURRENT_SOFTWARECLAMP_ON);
    if(Is_Power_SoftWareClamp_Work[uw16_ChannelNumber] == true)
        SetDataFlag(uw16_ChannelNumber, POWER_SOFTWARECLAMP_ON);
    if(Is_ClampDigiVolt_Work[uw16_ChannelNumber] == true)
        SetDataFlag(uw16_ChannelNumber, SIMU_CLAMPDIGITVOLT_ON);
}
#endif

unsigned int  Channel_MapAuxInfo::Locate_AUXChannel(unsigned char uc_channelNo, unsigned char Auxtype, unsigned short VirtualID)
{
    unsigned int AuxUnitID;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[Auxtype];
    AuxUnitID = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+VirtualID].m_Aux_UnitID;
    return AuxUnitID;
}


unsigned char  Channel_MapAuxInfo::RecvAuxData(ST_AUX_UPLOADATA UploadData)
{
    unsigned char DataType;//,ErrCode;
    signed short VirtualIdx;

    //VirtualIdx = GetAuxVirtualChannelIdx(UploadData.IV_ChnIdx,UploadData.AUX_CANID,UploadData.Aux_Type,UploadData.Aux_ChnIdx);
    VirtualIdx = UploadData.Aux_ChnIdx;
    if(VirtualIdx == -1)
        return 0;

    DataType = UploadData.Data_type;
    ST_AUX_DATA * p_AuxData = &m_AuxData[UploadData.IV_ChnIdx][UploadData.Aux_Type][VirtualIdx];
    switch(DataType)
    {
    case  ORIG_DATA:
        p_AuxData->aux_data = UploadData.Aux_Data;
        p_AuxData->updated_aux_data.m_Char = 0xFF;
        break;
    case DX_DT_DATA:
        p_AuxData->aux_dx_dt =  UploadData.Aux_Data;
        p_AuxData->updated_aux_dxdt.m_Char = 0xFF;
        break;
    case D2X_DT2_DATA:
        p_AuxData->aux_d2x_dt2 =  UploadData.Aux_Data;
        p_AuxData->updated_aux_d2xdt2.m_Char = 0xFF;
        break;
    default:
        return DATA_TYPE_ERROR;//ErrCode = DATA_TYPE_ERROR;
    }
    return DATA_TYPE_ERROR;//ErrCode = DataType;
}

St_AUXchannelInfo Channel_MapAuxInfo::GetAuxChannelInfo(unsigned char uc_channelNo, unsigned char AuxType, unsigned short VirtualIdx)
{   //add by hepeiqing ,20131128
    St_AUXchannelInfo R_MyInfo;
    //    unsigned char ChannelCount = 0;
    unsigned short uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_aux_type_ptr[AuxType];
    R_MyInfo.m_PhysicalChannelIdx = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+VirtualIdx].m_Map.m_Index_InAuxUnit;  //VirtualIdx;

    //MyInfo.m_ChannelIdx 表示在指定AUX UNIT上对应VirtualIdx的物理通道号。
    R_MyInfo.m_UnitId = Locate_AUXChannel(uc_channelNo, AuxType, VirtualIdx);
    //R_MyInfo.m_PhysicalChannelIdx=Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_AuxInfo_In_OneIVCh[AuxType][VirtualIdx].m_Map.m_Index_InAuxUnit; //comment out JIE

    return R_MyInfo;
}
unsigned char Channel_MapAuxInfo::AuxLimit_GetComPareSign(unsigned char m_ucCompareSign, unsigned char uc_channelNo, unsigned char LimitIdx)
{
    unsigned char Aux_DownLimitSign;
    switch(m_ucCompareSign)
    {
    case MP_MCS_GREATER_THAN:
        Aux_DownLimitSign = CAN_COMPARE_SIGN_GREATER_THAN;
        break;
    case MP_MCS_GREATER_THAN_OR_EQUAL_TO:
        Aux_DownLimitSign = CAN_COMPARE_SIGN_GREATER_THAN_OR_EQUAL_TO;
        break;
    case MP_MCS_LESS_THAN:
        Aux_DownLimitSign = CAN_COMPARE_SIGN_LESS_THAN;
        break;
    case MP_MCS_LESS_THAN_OR_EQUAL_TO:
        Aux_DownLimitSign = CAN_COMPARE_SIGN_LESS_THAN_OR_EQUAL_TO;
        break;
    case MP_MCS_EQUAL_TO:
        Aux_DownLimitSign = CAN_COMPARE_SIGN_EQUAL;
        break;
// 		ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_AnyLimitReachedCount =1;
// 		ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_bChecked = 1;
// 		ChannelInfo_Manager::ChannelStep[uc_channelNo].m_limitInfo[LimitIdx].m_bReached = 1;
// 		return CAN_COMPARE_SIGN_USELESS;  //add...
    //MyMsg.Content.m_DownLimit.Sign = CAN_COMPARE_SIGN_GREATER_THAN_OR_EQUAL_TO;
    //break;
    case MP_MCS_NONE:
    default:
        return CAN_COMPARE_SIGN_USELESS;	 //ERROR
    }
    return Aux_DownLimitSign;
}
unsigned int Channel_MapAuxInfo::AuxMapBySchedule( unsigned char Channel ) //180821 zyx
{
    bool IsSameAuxUnit = false;
    unsigned char m_AuxIndex = 0;
    unsigned short uw_aux_type_ptr = 0;
    memset(Channel_MapAuxInfo::m_AUXmap[Channel], 0, sizeof(ST_IV_MAPCONFIG) * MAXAUXTYPE);
    for(unsigned char uc_j = 0; uc_j < MAXAUXTYPE; uc_j++)
    {
        unsigned char m_SeekUnitID = 0;
        int TotalInOneIvChn = 0;
        TotalInOneIvChn = Channel_MapAuxInfo::IVChanMapAuxCount[Channel][uc_j];
        for(unsigned short uc_k = 0; uc_k < TotalInOneIvChn; uc_k++)   //????????????Unit, ????????Can
        {
            ST_IV_MAPCONFIG * thisAuxMap = &Channel_MapAuxInfo::m_AUXmap[Channel][uc_j];
            ST_CANNET * thisCanNet;
            if(BuiltIn_DataManager::b_BuiltInAux)
            {
                unsigned char uc_AuxType = MAXAUXTYPE;
                switch (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BoardType)
                {
                case Aux_BulitIn_16V_409386:
                    uc_AuxType = AUX_VOLTAGE;
                    break;
                case Aux_BulitIn_16T_409920:
                    uc_AuxType = AUX_TEMPERATURE;
                    break;
                case Aux_BuiltIn_8V8T_409924:
                    uc_AuxType = uc_j;
                    break;
                default:
                    break;
                }
                if(uc_AuxType == uc_j)
                {
                    unsigned char BuiltIn_chnCnt = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
                    if(uc_k < BuiltIn_chnCnt)
                        continue;
                }
            }
            uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[Channel].m_aux_type_ptr[uc_j];
            unsigned char * backUpUnit = &Channel_MapAuxInfo::PC2IVUnit4AuxMap[Channel].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+uc_k].m_Aux_UnitID;//某通道下某一类型的AUX的其中一个ID //&Channel_MapAuxInfo::m_BackUpAuxUnit[Channel][uc_j][uc_k];
            St_OneAuxChInfo * thisAuxInfoInOneIVCh = &Channel_MapAuxInfo::PC2IVUnit4AuxMap[Channel].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+uc_k];

            if((*backUpUnit != m_SeekUnitID) && (*backUpUnit != 0xff))
            {
                if(thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit > 0)
                {
                    for(unsigned int cnt = 0; cnt < thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit; cnt++)
                    {
                        if(*backUpUnit == thisAuxMap->m_AuxUnitCfg.m_Aux_UnitID[cnt])
                        {
                            IsSameAuxUnit = true;
                            break;
                        }
                        else
                            IsSameAuxUnit = false;
                    }
                }
                else
                    IsSameAuxUnit = false;

                m_SeekUnitID = *backUpUnit;

                if(!IsSameAuxUnit)
                {
                    unsigned char UnitCount = thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit;
                    thisAuxMap->m_AuxUnitCfg.m_Aux_UnitID[UnitCount] = m_SeekUnitID;
                    thisAuxMap->m_AuxUnitCfg.m_Aux_UnitCANPort[UnitCount] = thisAuxInfoInOneIVCh->m_CAN_Port;
                    thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit++;
                }
            }
            if(thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_EXCLUSIVE
                    || thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_PUBLIC)
            {
                if(thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_EXCLUSIVE)//CAN????
                    thisCanNet = &thisAuxMap->m_IntraCanNet;
                else
                    thisCanNet = &thisAuxMap->m_OuterCanNet;
                thisCanNet->m_AcutalIndexID[m_AuxIndex] = thisAuxInfoInOneIVCh->m_Map.m_Index_InIVCh;
                thisCanNet->m_AuxChannelInfo[thisAuxInfoInOneIVCh->m_Map.m_Index_InIVCh].m_PhysicalChannelIdx
                    = thisAuxInfoInOneIVCh->m_Map.m_Index_InAuxUnit;
                thisCanNet->m_AuxChannelInfo[uc_k].m_UnitId = thisAuxInfoInOneIVCh->m_Aux_UnitID;
                thisCanNet->m_ActualTotalAuxIndex++;
                m_AuxIndex = thisCanNet->m_ActualTotalAuxIndex;
            }
        }
    }
    //CANBusParse::IV_InitStatusMgr();  // 20200728 clear Aux connect status after start
    MultiMcuComm::Scan_IV_AUXMAP();
    return 1;
}
unsigned int Channel_MapAuxInfo::AuxMapParse(void)
{
    bool IsSameAuxUnit = false;
    unsigned char m_AuxIndex = 0;
    memset(Channel_MapAuxInfo::m_AUXmap, 0, sizeof(ST_IV_MAPCONFIG) * MAXCHANNELNO * MAXAUXTYPE);

    for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
    {
        unsigned short uw_aux_type_ptr = 0;
        for(unsigned char uc_j = 0; uc_j < MAXAUXTYPE; uc_j++)
        {
            unsigned char m_SeekUnitID = 0;
            for(unsigned short uc_k = 0; uc_k < Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_i].m_Total_In_OneIVCh[uc_j]; uc_k++)   //????????????Unit, ????????Can
            {
                // <<<Begin>>> Pointers Definition
                ST_IV_MAPCONFIG * thisAuxMap = &Channel_MapAuxInfo::m_AUXmap[uc_i][uc_j];
                ST_CANNET * thisCanNet;
                if(BuiltIn_DataManager::b_BuiltInAux)
                {
                    unsigned char uc_AuxType = MAXAUXTYPE;
                    switch (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BoardType)
                    {
                    case Aux_BulitIn_16V_409386:
                        uc_AuxType = AUX_VOLTAGE;
                        break;
                    case Aux_BulitIn_16T_409920:
                        uc_AuxType = AUX_TEMPERATURE;
                        break;
                    case Aux_BuiltIn_8V8T_409924:
                        uc_AuxType = uc_j;
                        break;
                    default:
                        break;
                    }
                    if(uc_AuxType == uc_j)
                    {
                        unsigned char BuiltIn_chnCnt = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels / ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
                        if(uc_k < BuiltIn_chnCnt)
                            continue;
                    }
                }
                uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_i].m_aux_type_ptr[uc_j];

                unsigned char * backUpUnit = &Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_i].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+uc_k].m_Aux_UnitID;//某通道下某一类型的AUX的其中一个ID //&Channel_MapAuxInfo::m_BackUpAuxUnit[uc_i][uc_j][uc_k];
                St_OneAuxChInfo * thisAuxInfoInOneIVCh = &Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_i].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+uc_k];
                // <<<End>>> Pointers Definition

                if((*backUpUnit != m_SeekUnitID) && (*backUpUnit != 0xff))
                {
                    if(thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit > 0)
                    {
                        for(unsigned int cnt = 0; cnt < thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit; cnt++)
                        {
                            if(*backUpUnit == thisAuxMap->m_AuxUnitCfg.m_Aux_UnitID[cnt])
                            {
                                IsSameAuxUnit = true;
                                break;
                            }
                            else
                                IsSameAuxUnit = false;
                        }
                    }
                    else
                        IsSameAuxUnit = false;

                    m_SeekUnitID = *backUpUnit;

                    if(!IsSameAuxUnit)
                    {
                        unsigned char UnitCount = thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit;
                        thisAuxMap->m_AuxUnitCfg.m_Aux_UnitID[UnitCount] = m_SeekUnitID;
                        thisAuxMap->m_AuxUnitCfg.m_Aux_UnitCANPort[UnitCount] = thisAuxInfoInOneIVCh->m_CAN_Port;
                        thisAuxMap->m_AuxUnitCfg.m_Total_Aux_Unit++;
                    }
                }

                if(thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_EXCLUSIVE
                        || thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_PUBLIC)
                {
                    if(thisAuxInfoInOneIVCh->m_CAN_Port == CAN_PORT_USE_EXCLUSIVE)//CAN????
                        thisCanNet = &thisAuxMap->m_IntraCanNet;
                    else
                        thisCanNet = &thisAuxMap->m_OuterCanNet;

                    thisCanNet->m_AcutalIndexID[m_AuxIndex] = thisAuxInfoInOneIVCh->m_Map.m_Index_InIVCh;
                    thisCanNet->m_AuxChannelInfo[thisAuxInfoInOneIVCh->m_Map.m_Index_InIVCh].m_PhysicalChannelIdx
                        = thisAuxInfoInOneIVCh->m_Map.m_Index_InAuxUnit;
                    thisCanNet->m_AuxChannelInfo[uc_k].m_UnitId = thisAuxInfoInOneIVCh->m_Aux_UnitID;
                    thisCanNet->m_ActualTotalAuxIndex++;
                    m_AuxIndex = thisCanNet->m_ActualTotalAuxIndex;
                }
            }
        }
    }

    unsigned long int *  u32_IvRegister;
    for(unsigned short int uw16_n = 0; uw16_n <  MCU_MAX_NUMBER; uw16_n++)
    {   // 整个表查一次  // 20190323 yy
        for(unsigned char uc_r = 0; uc_r < 3; uc_r++)
        {
            u32_IvRegister = &MultiMcuComm::St_Unit[uw16_n].u32_Aux_IvRegister;  // 20190829 yy
            u32_IvRegister += uc_r;
            *u32_IvRegister = 0;
        }
    }
    CANBusParse::IV_InitStatusMgr();
    MultiMcuComm::Scan_IV_AUXMAP();
    return 1;
}

void Channel_MapAuxInfo::SaveAuxMap_ForFRAM(unsigned short uw16_GetIndex)
{
    //AuxMapParse();  //dirui add 0313

    unsigned long uw32_length = (sizeof(MP_IVUnit4AuxMap) - sizeof(St_OneAuxChInfo) * MAX_MAP_COUNT_PER_UNIT); // 计算一次包中前面管理信息长度40字节
    //uw16_GetIndex --;  //  循环中多加了一次就变为数量值从1开始计，如果减少一次就变为实际数组位置从0开始计。
    if(uw16_GetIndex > MAX_MAP_COUNT_PER_UNIT) // 512     // 扩大到5.7K存MAP表
        uw16_GetIndex = MAX_MAP_COUNT_PER_UNIT; // 512;   // 用于最长存储FRAM的保护，暂定最大为512组一维数据。    // 扩大到5.7K存MAP表

    uw32_length += (uw16_GetIndex * sizeof(St_OneAuxChInfo));  // MAP表计算出实际一维字节数量  以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。  40+512*11=5672

    CConfig::WriteContent_ForFRAM(&EthernetParse::m_btPackDataBuffer[0], FRAM_AUXMAP_BASE, uw32_length);
}

void Channel_MapAuxInfo::ReadAuxMap_ForFRAM(void)
{
    /*
        unsigned long  uw32_length = (sizeof(MP_IVUnit4AuxMap) - sizeof(St_OneAuxChInfo) * MAX_AUX_TYPE_PER_MCU); // 计算一次包中前面管理信息长度
    	unsigned short uw16_GetIndex = MAX_AUX_TYPE_PER_MCU; //512;   // 用于最长存储FRAM的保护，暂定最大为512组一维数据。  扩大到5.7K存MAP表
    	uw32_length += (uw16_GetIndex * sizeof(St_OneAuxChInfo));  // MAP表计算出实际一维字节数量  以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。  40+512*11=5672
    */

    unsigned long  uw32_length = sizeof(MP_IVUnit4AuxMap); // MAP表计算出实际一维字节数量  以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。  40+512*11=5672
    CConfig::ReadContent_ForFRAM(&EthernetParse::m_btPackDataBuffer[0], FRAM_AUXMAP_BASE, uw32_length);

    ConvertAuxMap();
    AuxMapParse();  //dirui add 0313
}

unsigned short Channel_MapAuxInfo::ConvertAuxMap(void)
{
    MP_IVUnit4AuxMap * st_IVUnit4AuxMap;
    st_IVUnit4AuxMap = (MP_IVUnit4AuxMap *) (&EthernetParse::m_btPackDataBuffer[0]);
    unsigned short uw_aux_type_ptr = 0;
    unsigned short count = 0;
    unsigned short total_aux_chn_num = 0;
    unsigned short ptr = 0;
    switch (st_IVUnit4AuxMap->m_uAuxMapState)
    {
    case AUX_MAP_NORMAL:
        BuiltIn_DataManager::b_BuiltInAux = false;
        CommonFunc::uc_Support_AUX = 1;
        break;
    case AUX_MAP_BUILT_IN:
        BuiltIn_DataManager::b_BuiltInAux = true;
        CommonFunc::uc_Support_AUX = 0;
        break;
    case AUX_MAP_NORMAL_AND_BUILT_IN:
        CommonFunc::uc_Support_AUX = 1;
        BuiltIn_DataManager::b_BuiltInAux = true;
        break;
    case AUX_MAP_NA:
    default:
        BuiltIn_DataManager::b_BuiltInAux = false;
        CommonFunc::uc_Support_AUX = 0;
        break;

    }

    for(unsigned short uw_i = 0; uw_i < MAXCHANNELNO; uw_i++)
    {
        total_aux_chn_num += st_IVUnit4AuxMap->m_Total_In_OneIVCh[uw_i];
    }

    if(total_aux_chn_num > MAX_MAP_COUNT_PER_UNIT)
        total_aux_chn_num = MAX_MAP_COUNT_PER_UNIT;

    for(unsigned short uc_idx = 0, uc_IVdx = 0; uc_idx < MAXCHANNELNO; uc_idx++)
    {
        Channel_MapAuxInfo::m_Total_In_OneIVCh[uc_idx] = st_IVUnit4AuxMap->m_Total_In_OneIVCh[uc_idx];  // IVCH,Not AuxType 20151204 ying

        for(UWord16 uc_kdx = 0; uc_kdx < MAXAUXTYPE; uc_kdx++)
            Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_idx].m_Total_In_OneIVCh[uc_kdx] = 0;

        if(MYBITTRUE(st_IVUnit4AuxMap->m_ucChannelIdx, uc_idx))
        {
            if(st_IVUnit4AuxMap->m_Total_In_OneIVCh[uc_idx] > 0)
            {
                uw_aux_type_ptr = 0;
                Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_idx].m_IVCh_Index = uc_idx;

                for(unsigned char aux_type_num = 0; aux_type_num < MAXAUXTYPE; aux_type_num++)
                {
                    count = 0;
                    ptr = 0;
                    Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_idx].m_aux_type_ptr[aux_type_num] = uw_aux_type_ptr;

                    while(ptr < total_aux_chn_num)
                    {
                        if((st_IVUnit4AuxMap->m_AuxInfo_In_OneIVCh[ptr].m_AuxType == aux_type_num) && (st_IVUnit4AuxMap->m_AuxInfo_In_OneIVCh[ptr].m_IVCh_Index == uc_idx))
                        {
                            count++;
                            Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_idx].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr++] = st_IVUnit4AuxMap->m_AuxInfo_In_OneIVCh[ptr];
                        }

                        ptr++;
                    }

                    Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_idx].m_Total_In_OneIVCh[aux_type_num] = count;
                }
            }
            if(uc_IVdx++ >= st_IVUnit4AuxMap->m_u16ChannelCount) // 保护功能，用完数据退出。
                break;
        }
    }

    return total_aux_chn_num;
}


void Channel_MapAuxInfo::Set_AuxOutputCtrl(unsigned char uc_ChannelNo, signed short u16_TestSettingIdx)
{
    //	unsigned char Aux_ChnIdx = 0;
    unsigned short Aux_DOCount = 0;
    unsigned short Aux_AOCount = 0;
    signed char  sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return;

    const St_TestSetting *pMyTestSetting = &ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[u16_TestSettingIdx];

    for(unsigned short elementIdx = 0; elementIdx < pMyTestSetting->m_ucElementNum; elementIdx++)
    {
        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_DO)
            Aux_DOCount++;
        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_AO)
            Aux_AOCount++;

        if(((pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_TEMPERATURE)
                || (pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_HUMIDITY)) //AUX_FLOW_RATE改为AUX_HUMIDITY
                && pMyTestSetting->m_TestSettingElement[elementIdx].m_ucTurnOn)   // 20150611 yang
            //控制温控箱，顺便控制湿度
        {
            unsigned char Ctrltype1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_ucCtrlValue_DataType[0];
            unsigned char Operator1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_CtrlValueOperator[0];
            float fCoefficient1 =  pMyTestSetting->m_TestSettingElement[elementIdx].m_CtrlValueCoefficients[0];
            float fCtrlValue1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_fCtrlValue1;
            fCtrlValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fCtrlValue1);   // 20190926 yy
            /*设置温控箱的值*/
            CANBusParse::IV_SendTempChamberOut( uc_ChannelNo,
                                                pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx,
                                                pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType,
                                                fCtrlValue1);

            if(ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_Setting.m_MTCSafetyCheck.m_bMTCTempNeedCheck == 1)
            {
                CANBusParse::IV_SendMTCSafetyTimeOut(uc_ChannelNo,
                                                     pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx,
                                                     pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType,
                                                     ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_Setting.m_MTCSafetyCheck.m_uMTCSafetyTimeValue,
                                                     ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_Setting.m_MTCSafetyCheck.m_MTCTempSafetyGoto);
            }
        }
    }
    if(!ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_bUseGroupTChamber)//zyx 2106241
    {
        for(unsigned short elementIdx = 0; elementIdx < pMyTestSetting->m_ucElementNum; elementIdx++)
        {
            unsigned char AuxVirtIndx = ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[u16_TestSettingIdx].m_TestSettingElement[elementIdx].m_uwAuxVirtIndx; //add by zc 04.25.2018
            unsigned char AuxType = ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[u16_TestSettingIdx].m_TestSettingElement[elementIdx].m_ucType; //add by zc 04.25.2018
            St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(uc_ChannelNo, AuxType, AuxVirtIndx); //获取AUX物理通道号和所属ID（ip）//add by zc 04.25.2018

            if(((pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_TEMPERATURE)
                    || (pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_HUMIDITY)) //AUX_FLOW_RATE改为AUX_HUMIDITY
                    && pMyTestSetting->m_TestSettingElement[elementIdx].m_ucTurnOn)
                if( (((Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].OpenFlag >> MyInfo.m_PhysicalChannelIdx) & 0x01) == 0)
                        && Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].uc_channel[MyInfo.m_PhysicalChannelIdx] == uc_ChannelNo)
                {
                    /*启动温控箱 zyx*/
                    unsigned short runON = 1; //run0N = 1 即开启温控箱
                    CANBusParse::IV_SendTempChamberOutRunON(uc_ChannelNo,
                                                            pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx,
                                                            pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType,
                                                            runON);
                    //将温控箱设置为开状态
                    Schedule_Manager_task::OpenTemperatureChamber( pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx,
                            pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType, MyInfo.m_UnitId, MyInfo.m_PhysicalChannelIdx );  //changed by zc 04.25.2018
                    //break;
                }
        }
    }
    if(Aux_DOCount != 0)
        Set_AuxOutput_DOCtrl(uc_ChannelNo, u16_TestSettingIdx);
    if(Aux_AOCount != 0)
        Set_AuxOutput_AOCtrl(uc_ChannelNo, u16_TestSettingIdx);
}

void Channel_MapAuxInfo::Set_AuxOutput_DOCtrl(unsigned char uc_ChannelNo, signed short u16_TestSettingIdx)
{
    unsigned short Aux_ChnIdx = 0;
    signed char  sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return;

    const St_TestSetting *pMyTestSetting = &ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[u16_TestSettingIdx];

    for(unsigned short elementIdx = 0; elementIdx < pMyTestSetting->m_ucElementNum; elementIdx++)
    {
        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType != AUX_DO)
            continue;
        uint32 u32_shiftBitIO = 0;
        Aux_ChnIdx = pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx;
        u32_shiftBitIO = 1 << Aux_ChnIdx;
        m_AuxDoCtrlMgr[uc_ChannelNo].m_bitMask |= u32_shiftBitIO;

        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucTurnOn)   //1   // 预演按指定的辅助通道号设置一次,控制值。
        {
            m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus |= u32_shiftBitIO;
        }
        else
        {
            if((m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus & u32_shiftBitIO))
                m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus ^= u32_shiftBitIO;
        }
    }
    memset((void*)&m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[0].m_BitOut, 0, sizeof(St_perAuxUnitDOInfo)*MAXAUXUNIT);
    //get aux unit Bit out
    unsigned short bFind = 0;
    signed char unitIdx = -1;
    unsigned int thisIVBitValue = 0;
    int Total = Channel_MapAuxInfo::HowManyAuxTypeMap(uc_ChannelNo, AUX_DO); //180821 zyx
    unsigned short uw_aux_type_ptr = 0;
    for(unsigned short countIdx = 0 ; countIdx < Total; countIdx++) //PC2IVUnit4AuxMap[uc_ChannelNo].m_Total_In_OneIVCh[AUX_DO]
    {
        bFind = 0;
        uw_aux_type_ptr = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_ChannelNo].m_aux_type_ptr[AUX_DO];
        for(unsigned short validIdx = 0; validIdx < MAXAUXUNIT; validIdx++)
        {
            if( m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[validIdx].m_bValid && m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[validIdx].m_ID != 0)
                if(m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[validIdx].m_ID == PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Aux_UnitID)
                {
                    bFind = 1;
                    thisIVBitValue = (m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus & (1 << PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Map.m_Index_InIVCh)); // IV中辅助通道索引号是否相同。
                    if(thisIVBitValue)  //transfer IV's bit Out to Aux Unit Bit out
                        m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[unitIdx].m_BitOut |=  1 << PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Map.m_Index_InAuxUnit; // 如果指定的辅助通道号与IV中辅助索引号相同设置控制值。
                    break;
                }
        }

        //new unitIdx
        if(!bFind)
        {
            unitIdx++;
            m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[unitIdx].m_bValid = 1;
            m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[unitIdx].m_ID = PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Aux_UnitID;
            thisIVBitValue = (m_AuxDoCtrlMgr[uc_ChannelNo].m_bitDOStatus & (1 << PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Map.m_Index_InIVCh));
            if(thisIVBitValue)  //transfer IV's bit Out to Aux Unit Bit out
                m_AuxDoCtrlMgr[uc_ChannelNo].m_AuxUnitInfo[unitIdx].m_BitOut |=  1 << PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr+countIdx].m_Map.m_Index_InAuxUnit; // 如果指定的辅助通道号与IV中辅助索引号相同设置控制值。
            bFind = 1;
        }
    }
    //send DO Ctrl by CAN Bus
    CANBusParse::IV_SendDigitOut(uc_ChannelNo, 1);
}
void Channel_MapAuxInfo::Set_AuxOutput_AOCtrl(unsigned char uc_ChannelNo, signed short u16_TestSettingIdx)
{
    unsigned short Aux_ChnIdx = 0;
    unsigned short Aux_AI_ChnIdx = 0;
    unsigned short Aux_AI_Type = 0;
    signed char  sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return;

    const St_TestSetting *pMyTestSetting = &ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[u16_TestSettingIdx];

    m_AuxAOCtrlMgr[uc_ChannelNo].m_ChnCount = pMyTestSetting->m_ucElementNum;
    unsigned short uw_aux_type_ptr1 = 0;
    unsigned short uw_aux_type_ptr2 = 0;

    for(unsigned short elementIdx = 0; elementIdx < pMyTestSetting->m_ucElementNum; elementIdx++)
    {
        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType != AUX_AO)
            continue;
        Aux_ChnIdx = pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx;
        uw_aux_type_ptr1 = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_ChannelNo].m_aux_type_ptr[AUX_AO];
        m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AO_ChnNO = PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr1+Aux_ChnIdx].m_Map.m_Index_InAuxUnit; // 20170228 zyx
        m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].m_ID = PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr1+Aux_ChnIdx].m_Aux_UnitID;
        m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].m_CANType = PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr1+Aux_ChnIdx].m_CAN_Port;

        if(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucTurnOn)   //1   // 预演按指定的辅助通道号设置一次,控制值。
        {
            float set_value;
            float m_ki;
            unsigned char Ctrltype1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_ucCtrlValue_DataType[0];
            unsigned char Operator1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_CtrlValueOperator[0];
            float fCoefficient1 =  pMyTestSetting->m_TestSettingElement[elementIdx].m_CtrlValueCoefficients[0];
            float fCtrlValue1 = pMyTestSetting->m_TestSettingElement[elementIdx].m_fCtrlValue1;
            fCtrlValue1 = ChannelInfo_Manager::Get_MetaVariable(uc_ChannelNo, Ctrltype1, Operator1,fCoefficient1,fCtrlValue1);   // 20190926 yy
            set_value = fCtrlValue1 * 100;
            if(set_value > 16384)
            {
                set_value = 16384;
            }
            else if (set_value < -16384)
            {
                set_value = -16384;
            }

            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Set_Value = 16384 + set_value;

            if(pMyTestSetting->m_TestSettingElement[elementIdx].m_fCtrlValue2 == 0)
            {
                m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AI_ChnNO = 0xFF;
            }
            else
            {
                Aux_AI_ChnIdx = pMyTestSetting->m_TestSettingElement[elementIdx].m_fCtrlValue3;
                m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AI_ChnType = pMyTestSetting->m_TestSettingElement[elementIdx].m_nCtrlValue3Type;
                Aux_AI_Type = pMyTestSetting->m_TestSettingElement[elementIdx].m_nCtrlValue3Type;
                if(Aux_AI_Type == 01)
                {
                    Aux_AI_Type = AUX_VOLTAGE;
                }
                else if(Aux_AI_Type == 02)
                {
                    Aux_AI_Type = AUX_TEMPERATURE;
                }
                else
                {
                    Aux_AI_Type = AUX_PRESSURE;
                }
                uw_aux_type_ptr2 = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_ChannelNo].m_aux_type_ptr[Aux_AI_Type];
                m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AI_ChnNO = PC2IVUnit4AuxMap[uc_ChannelNo].m_AuxInfo_In_OneIVCh[uw_aux_type_ptr2+Aux_AI_ChnIdx].m_Map.m_Index_InAuxUnit; //Aux_AI_ChnIdx;//180825 zyx
                m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AI_ChnType = Aux_AI_Type;
            }
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].kP = pMyTestSetting->m_TestSettingElement[elementIdx].m_fP;
            m_ki = pMyTestSetting->m_TestSettingElement[elementIdx].m_fI;
            if(m_ki < 0.01)
            {
                m_ki = 0.01;
            }
            else if(m_ki > 10.23)
            {
                m_ki = 10.23;
            }
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].kI = (m_ki * 100);
        }
        else
        {
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Set_Value = 16384;
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].kI = 1;
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].kP = 0;
            m_AuxAOCtrlMgr[uc_ChannelNo].m_Value[elementIdx].Aux_AI_ChnNO = 0xFF;
        }
    }
    CANBusParse::IV_SendAnalogSingnalOut(uc_ChannelNo, 1);
}
unsigned char  Channel_MapAuxInfo::RecvAuxMetaData(unsigned char uc_channelNo, unsigned char MVType, float value)
{
    unsigned char uc_type = MVType;
    float MV_Value = value;
    unsigned char chn = uc_channelNo;
    st_GetMetaValue * p_MV2 = &ChannelInfo_Manager::MetaVariable2[chn][0];
    switch(uc_type)
    {
    case 0:          //ChargeEnergy
        if(Step_Manager_task::bEC_CalEnergyFlag[chn][0])
        {
            m_AuxLastStepMetaValue[chn][0] = p_MV2[MetaCode_PV_ChargeEnergy].m_fValue;
            p_MV2[MetaCode_PV_ChargeEnergy].m_fValue = m_AuxLastStepMetaValue[chn][0] + MV_Value;
            Step_Manager_task::bEC_CalEnergyFlag[chn][0] = 0;
        }
        p_MV2[MetaCode_PV_ChargeEnergy].m_fValue = m_AuxLastStepMetaValue[chn][0] + MV_Value;
        break;
    case 1:         //Discharge Energy
        if(Step_Manager_task::bEC_CalEnergyFlag[chn][1])
        {
            m_AuxLastStepMetaValue[chn][1] = p_MV2[MetaCode_PV_DischargeEnergy].m_fValue;
            p_MV2[MetaCode_PV_DischargeEnergy].m_fValue = m_AuxLastStepMetaValue[chn][1] + MV_Value;
            Step_Manager_task::bEC_CalEnergyFlag[chn][1] = 0;
        }
        p_MV2[MetaCode_PV_DischargeEnergy].m_fValue = m_AuxLastStepMetaValue[chn][1] + MV_Value;
        break;
    case 2:         //ChargeCapacity
        if(Step_Manager_task::bEC_CalEnergyFlag[chn][2])
        {
            m_AuxLastStepMetaValue[chn][2] = p_MV2[MetaCode_PV_ChargeCapacity].m_fValue;
            p_MV2[MetaCode_PV_ChargeCapacity].m_fValue = m_AuxLastStepMetaValue[chn][2] + MV_Value;
            Step_Manager_task::bEC_CalEnergyFlag[chn][2] = 0;
        }
        p_MV2[MetaCode_PV_ChargeCapacity].m_fValue = m_AuxLastStepMetaValue[chn][2] + MV_Value;
        break;
    case 3:         //DisChargeCapacity
        if(Step_Manager_task::bEC_CalEnergyFlag[chn][3])
        {
            m_AuxLastStepMetaValue[chn][3] = p_MV2[MetaCode_PV_DischargeCapacity].m_fValue;
            p_MV2[MetaCode_PV_DischargeCapacity].m_fValue = m_AuxLastStepMetaValue[chn][3] + MV_Value;
            Step_Manager_task::bEC_CalEnergyFlag[chn][3] = 0;
        }
        p_MV2[MetaCode_PV_DischargeCapacity].m_fValue =  m_AuxLastStepMetaValue[chn][3] + MV_Value;
        break;
    }
    return 0;
}

//#endif      // Started at Channel_MapAuxInfo::Write_MapInfo( )
void  BuiltIn_DataManager::BuiltIn_IO_Init(void)
{
    ControlPin::SetIObyCP(BuiltIn_DataManager::Pin_EOE);
    ControlPin::SetIObyCP(BuiltIn_DataManager::Pin_ECODE1);
    ControlPin::SetIObyCP(BuiltIn_DataManager::Pin_ECODE2);
    ControlPin::SetIObyCP(BuiltIn_DataManager::Pin_ECODE3);
    ControlPin::SetMeHigh(BuiltIn_DataManager::Pin_EOE);

}
void  BuiltIn_DataManager::BuiltIn_HardwareInit(void)
{
    unsigned char Board_id = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BID;
    switch (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BoardType)
    {
    case Aux_BulitIn_16V_409386:
        // 							   unsigned char Board_id = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BID;
        BuiltIn_16V_409386_Base::Hardware_Init(Board_id);
        break;
    case Aux_BulitIn_16T_409920:
        BuiltIn_16T_409920_Base::Hardware_Init(Board_id);
        break;
    case Aux_BuiltIn_8V8T_409924:

        break;
    default:
        return; //break;
    }
    for(unsigned char i = 0; i < 16; i++) //最多16个片选地址
    {
        if(BuiltIn_DataManager::BuiltIn_ADC_Info[i].m_Valid == 1)
        {
            BuiltIn_DataManager::BuiltIn_ADC_Idx = i; //初始化辅助ADC片选首地址  防止只有一块板，波码不是0
            break;
        }
        else
        {
            continue;
        }
    }
    BuiltIn_DataManager::BuiltIn_ADC_TotalCnt = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_ADC;
    if(BuiltIn_DataManager::BuiltIn_ADC_TotalCnt != 0)
    {
        OS_ServeProcess::BuiltIn_Flag = true;
        m_Sample_Flag = true;
    }
}
unsigned char BuiltIn_DataManager::FindNextBuiltInADCid(void)   //返回的是一个片选地址，应该是一个无符号数，这应养成习惯。对于一个不应该出现负数的量，我们就应该定义成无符号数。

{
    unsigned char ADC_CS_idx;
    unsigned short CheckResult;
    unsigned char temp;
    ADC_CS_idx = BuiltIn_ADC_Idx;
    CheckResult = CheckAllBuiltInADCStatus();
    if(BuiltIn_ADC_Info[ADC_CS_idx].m_StateMachine == BUILTIN_Sample_READ)  //防止还没有对上次的数据读完就切换到别的片选地址去了
        return ADC_CS_idx;
    if(BuiltIn_ADC_TotalCnt == 1)
        OS_ServeProcess::AUX_ADC_GOTO_Idx = ADC_CS_idx;
    else
    {
        OS_ServeProcess::AUX_ADC_GOTO_Idx = findNextADC_CS(ADC_CS_idx);

    }
    //     OS_ServeProcess::AUX_ADC_GOTO_Idx=ADC_CS_idx+1;   //如果选择在这个函数里进行片选地址递增的话，则针对任何一种情况，都应该是这样的


    if (CheckResult != 0) //如果至少有一个ADC 转换完毕
    {
        if (!(CheckResult & (1 << ADC_CS_idx))) //（ 不是当前CS_Idx的那个ADC转换完毕）
        {
            for(unsigned char i = 1; i < 16; i++)
            {
                if(i <= ADC_CS_idx)
                    temp = ADC_CS_idx - i; //找最远的那个ADC
                else
                    temp = 16 + ADC_CS_idx - i; //翻转
                if (CheckResult & (1 << (temp)))
                {
                    ADC_CS_idx = temp;   //找到了最远的那个ADC，实现跳变
                    break;
                }

            }
        }

    }
    else
    {
        if(BuiltIn_ADC_Info[ADC_CS_idx].m_StateMachine == BUILTIN_Sample_IDLE)
        {
            if(BuiltIn_ADC_TotalCnt == 1)
            {
                return ADC_CS_idx;
            }
            ADC_CS_idx = findNextADC_CS(ADC_CS_idx);
        }
    }
    return ADC_CS_idx;
}

unsigned short BuiltIn_DataManager::CheckAllBuiltInADCStatus(void)   //这里必须用unsigned short，如果用short的话，只能正确标记15个ADC
{
    //for(unsigned char idx=0;idx<Aux_DataManager::Aux_ADC_TotalCnt;idx++)
// 	ControlPin::SetMeHigh(LEDFunc::Pin_HeartBeat);
    for(unsigned char idx = 0; idx < 16; idx++)
    {
        if(BuiltIn_ADC_Info[idx].b_Triggered)
        {
            BuiltIn_ADC_Info[idx].counter++;  //每个ADC Trigger后的通道，刷新计时器
            if(BuiltIn_ADC_Info[idx].counter >= BuiltIn_ADC_Info[idx]. m_Wait_Count)
            {
                if((BuiltIn_ADC_Info[idx].m_StateMachine == BUILTIN_Sample_WAIT) && (m_SuspendFlg == 0)) //状态机的更新在这里统一处理比较好
                {
                    SettingCS_High();
                    BuiltIn_ADC_Info[idx].m_StateMachine = BUILTIN_Sample_TRIGGER;
                    BuiltIn_ADC_Info[idx].b_Finished = 1;
                    BuiltIn_ADC_Info[idx].b_Triggered = 0;
                    BuiltIn_ADC_TimeOut_Flag |= (1 << idx);
                    BuiltIn_ADC_Info[idx].m_WorkingChn = findNextChn(idx);
                }
            }
        }
        if (BuiltIn_ADC_Info[idx].b_ADG_Opened)
        {
            BuiltIn_ADC_Info[idx].counter++;
            if(BuiltIn_ADC_Info[idx].counter >= BuiltIn_ADC_Info[idx].m_TickCount_ADG)
            {
                if((BuiltIn_ADC_Info[idx].m_StateMachine == BUILTIN_Sample_WAIT) && (m_SuspendFlg == 0)) //状态机的更新在这里统一处理比较好
                {
                    SettingCS_High();
                    BuiltIn_ADC_Info[idx].m_StateMachine = BUILTIN_Sample_TRIGGER;
                    BuiltIn_ADC_Info[idx].b_Finished = 1;
                    BuiltIn_ADC_Info[idx].b_ADG_Opened = 0;
                    BuiltIn_ADC_TimeOut_Flag |= (1 << idx);
                    BuiltIn_ADC_Info[idx].m_WorkingChn = findNextChn(idx);
                }
            }
        }
        if(m_WriteFRAM_Flag)
        {
            if(m_uwSamplePauseFlag == 0)
            {
                SettingCS_High();
                m_Sample_Flag = false;
            }
        }
    }
    return BuiltIn_ADC_TimeOut_Flag;
}
char  BuiltIn_DataManager::SettingCS_Low(unsigned char m_CS_Address) //这部分代码要重新考虑，不同的板片选实现的方式不完全一样
{
    switch(m_CS_Address)
    {
    case 0:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 1:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 2:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 3:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 4:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 5:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 6:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 7:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeLow(Pin_EOE);
        break;
    case 8:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 9:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 10:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 11:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeLow(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 12:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 13:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeLow(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 14:
        ControlPin::SetMeLow(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    case 15:
        ControlPin::SetMeHigh(Pin_ECODE1);
        ControlPin::SetMeHigh(Pin_ECODE2);
        ControlPin::SetMeHigh(Pin_ECODE3);
        ControlPin::SetMeHigh(Pin_EOE);
        break;
    default:
        break;
    }
    BuiltIn_DataManager::m_BuiltInCSFlag = 1;
    return 1;

}

void  BuiltIn_DataManager::SettingCS_High(void) //这部分代码要重新考虑，不同的板片选实现的方式不完全一样

{
    ControlPin::SetMeHigh(Pin_EOE);
    BuiltIn_DataManager::m_BuiltInCSFlag = 0;
}

unsigned char  BuiltIn_DataManager::findNextChn(unsigned char ADCID)
{
    unsigned char chn;
    chn = BuiltIn_ADC_Info[ADCID].m_WorkingChn;
    for(unsigned char i = chn + 1; i < BuiltIn_ADC_Info[ADCID].m_ADCCH_MaxCnt; i++)
    {
        if(!BuiltIn_ADC_Info[ADCID].b_ADCCH_Valid[i])                     //通道不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    // 翻转了
    for (unsigned char i = 0; i < chn; i++)
    {
        if(!BuiltIn_ADC_Info[ADCID].b_ADCCH_Valid[i])                     //通道不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    return 0;
}

unsigned char  BuiltIn_DataManager::findNextADC_CS(unsigned char ADCidx)
{
    unsigned char temp = ADCidx;
    for(unsigned i = temp + 1; i < 16; i++)
    {
        if(!BuiltIn_ADC_Info[i].m_Valid)                     //ADC不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    for(unsigned i = 0; i < temp; i++)
    {
        if(!BuiltIn_ADC_Info[i].m_Valid)                     //ADC不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    return 0;
}
unsigned char  BuiltIn_DataManager::findLastChn(unsigned char ADCID)
{
    unsigned char temp = BuiltIn_ADC_Info[ADCID].m_WorkingChn;
    for(unsigned char i = temp; i > 0; i--)
    {
        if(!BuiltIn_ADC_Info[ADCID].b_ADCCH_Valid[i - 1])                   //通道不存在
        {
            continue;
        }
        else
        {
            return (i - 1);
        }

    }
    for(unsigned char i = BuiltIn_ADC_Info[ADCID].m_ADCCH_MaxCnt; i > temp; i--)
    {
        if(!BuiltIn_ADC_Info[ADCID].b_ADCCH_Valid[i - 1])                   //通道不存在
        {
            continue;
        }
        else
        {
            return (i - 1);
        }
    }
    return 0;
}
char  BuiltIn_DataManager::SampleISR(void)
{
    unsigned char Last_chn;
    unsigned long data;
    if((!m_Sample_Flag) || (IR::DAC_Flag == 1))
        return 0;

    BuiltIn_ADC_Idx = FindNextBuiltInADCid();
    switch(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine)
    {
    case BUILTIN_Sample_IDLE:
        if(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_ADG_Exist_Flag == true)
        {
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_OPENADG;
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_ADG_Opened = false;
        }
        else
        {
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_TRIGGER;
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_Finished = 0;
        }
        break;
    case BUILTIN_Sample_OPENADG:
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].counter = 0;
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_BoardADC_OpenADG(BuiltIn_ADC_Idx, BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_WorkingChn);
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_ADG_Opened = true;
// 				      if(BuiltIn_DataManager::m_WriteFRAM_Flag)
        m_uwSamplePauseFlag &= ~(1 << BuiltIn_ADC_Idx);
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_WAIT;
        break;
    case BUILTIN_Sample_WAIT:

        break;
    case BUILTIN_Sample_TRIGGER:
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].counter = 0;
// 				      if(BuiltIn_DataManager::m_WriteFRAM_Flag)
        m_uwSamplePauseFlag |= 1 << BuiltIn_ADC_Idx;
        SettingCS_Low(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_CS_Address.m_Value);//拉低片选
        if(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_ADG_Exist_Flag == true)
        {
            Last_chn = findLastChn(BuiltIn_ADC_Idx);
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_BoardADC_Trigger(BuiltIn_ADC_Idx, Last_chn);
        }
        else
        {
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_BoardADC_Trigger(BuiltIn_ADC_Idx, BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_WorkingChn);
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_Triggered = 1;
        }
        if(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_WAIT2_FLAG)
        {
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_WAIT2;
        }
        else
        {
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_READ;
        }
        break;

    case BUILTIN_Sample_READ:
        Last_chn = findLastChn(BuiltIn_ADC_Idx);
        data = BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_BoardADC_Rd(BuiltIn_ADC_Idx, Last_chn);
        if(data != 0)
        {
            SettingCS_High();
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].counter = 0;
            BuiltIn_ADC_TimeOut_Flag &= ~(1 << BuiltIn_ADC_Idx);				//读取数据后清标志
            BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_Finished = 0;
            if(BuiltIn_ADC_Info[BuiltIn_ADC_Idx].b_ADG_Exist_Flag == true)
            {
                BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_OPENADG;
            }
            else
            {
// 										  if(BuiltIn_DataManager::m_WriteFRAM_Flag)
                m_uwSamplePauseFlag &= ~(1 << BuiltIn_ADC_Idx);
                BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_WAIT;
            }

            BuiltIn_ADC_Idx = BuiltIn_ADC_GOTO_Idx;
        }
        break;
    case BUILTIN_Sample_WAIT2:
        BuiltIn_ADC_Info[BuiltIn_ADC_Idx].m_StateMachine = BUILTIN_Sample_READ;
        break;
    default:

        break;

    }
    return 0;
}


unsigned char ChannelInfo_Manager::CheckJumpStep_ToProtectCycle(unsigned char uc_ChannelNo, unsigned short u16_FormStepID, unsigned short u16_TargetStepID)
{
    if(u16_FormStepID != ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_RunningStepIdx)
        return 0;
    if(u16_FormStepID != u16_TargetStepID)
    {
        signed char    sc_SchBufIdx  = ChannelSchedule[uc_ChannelNo].m_scBufIdx;
        if((sc_SchBufIdx < 0)
                || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER)
                || (u16_TargetStepID >= ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount)
                || (u16_FormStepID >= ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount))
            return 0;

        if(u16_TargetStepID < u16_FormStepID)   // 互换 9--->1 change to 1--->9
        {
            unsigned short u16_StepID = u16_TargetStepID;
            u16_TargetStepID = u16_FormStepID;
            u16_FormStepID = u16_StepID;
        }


        Un_CtrlValues * p_Ctrl_Values	=	&ScheduleBuf[sc_SchBufIdx].m_StepData[u16_FormStepID].m_Ctrl.m_CtrlValues;
        unsigned long uw32_Increment    =	p_Ctrl_Values->m_fControlValue[1];
        if(ScheduleBuf[sc_SchBufIdx].m_StepData[u16_FormStepID].m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE)   // SETVARIABLE步
        {
            if(Schedule_Manager_task::GetSetVariable(uw32_Increment,MP_MV_INC_CHAN_CYCLE_INDEX) != 0)
                return 0;
        }
        if(ScheduleBuf[sc_SchBufIdx].m_StepData[u16_TargetStepID].m_Ctrl.m_u16ControlType == CT_IVCH_SETVARIABLE)  // SETVARIABLE步
        {
            p_Ctrl_Values	=	&ScheduleBuf[sc_SchBufIdx].m_StepData[u16_TargetStepID].m_Ctrl.m_CtrlValues;
            uw32_Increment    =	p_Ctrl_Values->m_fControlValue[1];
            if(Schedule_Manager_task::GetSetVariable(uw32_Increment,MP_MV_INC_CHAN_CYCLE_INDEX) != 0)
                return 0;
        }

        for(unsigned short int uw16_i = u16_FormStepID; uw16_i <= u16_TargetStepID; uw16_i++) // 扫描SETVARIABLE步
        {
            p_Ctrl_Values	=	&ScheduleBuf[sc_SchBufIdx].m_StepData[uw16_i].m_Ctrl.m_CtrlValues;
            uw32_Increment    =	p_Ctrl_Values->m_fControlValue[1];
            if(ScheduleBuf[sc_SchBufIdx].m_StepData[uw16_i].m_Ctrl.m_u16ControlType != CT_IVCH_SETVARIABLE)  // 普通步
                continue;
            else
            {
                if(Schedule_Manager_task::GetSetVariable(uw32_Increment, MP_MV_INC_CHAN_CYCLE_INDEX) == 0)       // 无CycleIndex
                    continue;
            }
            return 1;
        }
    }
    return 0;
}


int ChannelInfo_Manager::OnlineUpdata_ScanScheduleBuf(void *pData)
{
    signed char sc_BufIdx = -1;
    //unsigned char uc_rangebit[MAX_UPDATA_STEP_NUMBER];  // 1-Curren;2-Voltage
    unsigned char uc_AuxChange = 0;
    unsigned char uc_Support_AUX_back = CommonFunc::uc_Support_AUX;
    unsigned short uw16_point_Index = 0;
    unsigned long CopiedSize = 0;
    unsigned long u32IvRegister = 0;
    St_Sche_Onlineupdata  * st_Onlineupdata = (St_Sche_Onlineupdata  *) pData;
    St_ScheduleBuffer * p_Buff = &ScheduleBuf[0];

    for (signed char i = 0; i < IV_MAX_SCHEDULE_BUFFER_NUMBER; i++)
    {
        if(ScheduleBuf[i].m_bValid)      //合法的buffer，比较是否为相同的MD5
        {
            if(!memcmp((void *)&ScheduleBuf[i].m_ID, (void *)&st_Onlineupdata->st_updata.m_OldID, 16))
            {
                if(ScheduleBuf[i].m_u32IvRegister != 0)    // 已有IV通道被Assign到此Schedule Buffer上
                {
                    sc_BufIdx = i;
                    break;
                }
            }
        }
    }
    if(sc_BufIdx <= -1)
        return 0;      // 查原SCH位置失败；

    for(unsigned short uw16_i = 0; uw16_i < st_Onlineupdata->st_updata.m_ucStepCount; uw16_i++)  // check Step Number
    {
        uw16_point_Index = st_Onlineupdata->st_updata.m_ucStepNum[uw16_i];
        if(uw16_point_Index >= ScheduleBuf[sc_BufIdx].m_ItemCount.m_u16StepCount)
            return 0;       // 更改步号不正确
    }

    // 检查更新的控制类型，避免不允许的类型被更新
//     for(unsigned short uw16_ch = 0; uw16_ch < BoardCFG.m_Global.m_u16IVChannelCount; uw16_ch++)
//     {
//         if(((st_Onlineupdata->uw32_Channelbit >> uw16_ch) & 0X01) == ((u32IvRegister >> uw16_ch) & 0X01))
//         {
    for(unsigned short uw16_i = 0; uw16_i < st_Onlineupdata->st_updata.m_ucStepCount; uw16_i++)
    {
        uw16_point_Index = st_Onlineupdata->st_updata.m_ucStepNum[uw16_i]; //  Check Step Ctrl Type
        if(st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_ucValid_StepCtrl)
        {
            signed char sc_sch = 1, sc_updata = 1;
            unsigned short u16ControlType = st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Ctrl.m_u16ControlType;
            float f_value = st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Ctrl.m_CtrlValues.m_fControlValue[0];
            sc_updata = OnlineUpdata_CheckStepControlType(u16ControlType, f_value);
            u16ControlType = ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_u16ControlType;
            f_value = ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_CtrlValues.m_fControlValue[0];
            sc_sch = OnlineUpdata_CheckStepControlType(u16ControlType, f_value);
            if((sc_sch <= -1) || (sc_updata <= -1))
                return 0;
        }
    }

//         }
//     }

    p_Buff = &ScheduleBuf[sc_BufIdx];
    if(st_Onlineupdata->st_updata.m_ucValid_Stetting)
    {
        memcpy(&p_Buff->m_Setting,                                                                    // replace Global
               &st_Onlineupdata->st_updata.m_Setting,
               sizeof(St_ScheduleSetting));
        CopiedSize = sizeof(St_ScheduleSetting);   // St_ScheduleSetting 728 Byte
    }

    CopiedSize = sizeof(St_Sche_Onlineupdata);  //  St_Sche_Onlineupdata 844 Byte
    uw16_point_Index = 0;
    CommonFunc::uc_Support_AUX = 0;
//     for(unsigned short uw16_i = 0; uw16_i < MAX_UPDATA_STEP_NUMBER; uw16_i++)
//         uc_rangebit[uw16_i] = 0;
    for(unsigned short uw16_i = 0; uw16_i < st_Onlineupdata->st_updata.m_ucStepCount; uw16_i++)               // replace Step
    {
        uw16_point_Index = st_Onlineupdata->st_updata.m_ucStepNum[uw16_i];
        if(st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_ucValid_StepCtrl)  // replace Step control value
        {
//             if(memcmp((void *)&ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_ucCurrentRange,
//                       (void *)&st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Ctrl.m_ucCurrentRange, sizeof(unsigned char)))
//                 uc_rangebit[uw16_i] |= 0x01;   //  1-Curren;2-Voltage
//             if(memcmp((void *)&ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_ucVoltageRange,
//                       (void *)&st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Ctrl.m_ucVoltageRange, sizeof(unsigned char)))
//                 uc_rangebit[uw16_i] |= 0x02;   //  1-Curren;2-Voltage
            ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Ctrl = st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Ctrl;
            CopiedSize = sizeof(St_StepCtrl);  // St_StepCtrl 36 Byte
        }

        if(st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_ucValid_Limit >= 1)  // replace Step Limit
        {
            unsigned short uw16_Limit_Indexst = 0;
            for(unsigned short uw16_j = 0; uw16_j < st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_ucValid_Limit; uw16_j++)
            {
                uw16_Limit_Indexst = st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_LimitsNum[uw16_j];
                if(st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Limits[uw16_j].m_uCheckPlace == ValueCheckPlace_AUXI)
                {
//                     uc_AuxChange = 1;
//                     CommonFunc::uc_Support_AUX = 1;   //  封锁涉及辅助的在线更新
                }
                ScheduleBuf[sc_BufIdx].m_StepData[uw16_point_Index].m_Limits[uw16_Limit_Indexst] = st_Onlineupdata->st_updata.m_Step_UpData[uw16_i].m_Limits[uw16_j];
            }
            CopiedSize = sizeof(St_Limit);  // St_Limit 36 Byte
        }
    }

    p_Buff->m_ID = st_Onlineupdata->st_updata.m_NewID;   // replace SCH MD5

    u32IvRegister  = p_Buff->m_u32IvRegister;    // 已有IV通道被Assign到此Schedule Buffer上
    for(unsigned short uw16_ch = 0; uw16_ch < BoardCFG.m_Global.m_u16IVChannelCount; uw16_ch++)
    {
        if(((st_Onlineupdata->uw32_Channelbit >> uw16_ch) & 0X01) == 0X01)
        {
            if(((st_Onlineupdata->uw32_Channelbit >> uw16_ch) & 0X01) == ((u32IvRegister >> uw16_ch) & 0X01))
            {
                if(ChannelInfo_Manager::ChannelInfo[uw16_ch].ScheduleStatus == RUNNING)
                {
                    ChannelSchedule[uw16_ch].m_ID = st_Onlineupdata->st_updata.m_NewID;  //  Assign SCH to Channel 通道切换SCH
                    ChannelSchedule[uw16_ch].m_scBufIdx = sc_BufIdx;
                    SetBit_32Bit(ScheduleBuf[sc_BufIdx].m_u32IvRegister, uw16_ch);

                    if(st_Onlineupdata->st_updata.m_ucValid_Stetting)
                    {
                        ChannelInfo_Manager::OnlineUpdata_Load(st_Onlineupdata, uw16_ch, MAX_UPDATA_STEP_NUMBER);//, uc_rangebit//zyx 2105291
//                        uc_AuxChange = 1;
//                        CommonFunc::uc_Support_AUX = 1;  //  封锁涉及辅助的在线更新
                    }

                    if(uc_AuxChange)
                    {
                        ChannelInfo_Manager::OnlineUpdata_AUXLoad(uw16_ch);
                    }

                    for(unsigned short uw16_i = 0; uw16_i < st_Onlineupdata->st_updata.m_ucStepCount; uw16_i++)
                    {
                        uw16_point_Index = st_Onlineupdata->st_updata.m_ucStepNum[uw16_i]; //  扫描当前运行Step
                        if(uw16_point_Index == ChannelSchedule[uw16_ch].m_RunningStepIdx)
                        {
                            // 当前步有更新
                            ChannelInfo_Manager::OnlineUpdata_Load(st_Onlineupdata, uw16_ch, uw16_i);//, uc_rangebit//zyx 2105291
                            break;
                        }
                    }
                }
            }
        }
    }

    CommonFunc::uc_Support_AUX = uc_Support_AUX_back;

    return 1;     //schedule 成功搬到BUFFERS里
}

signed char ChannelInfo_Manager::OnlineUpdata_CheckStepControlType(UWord16 uw16_ControlType, float f_Value)
{
    switch(uw16_ControlType)       //根据控制类型确认是否允许SCH在线更新
    {
    case CT_IVCH_EXTCHARGE:   //External Charge输出时暂时不对通道操作 保持Rest
    case CT_IVCH_EXTLOAD:
    case CT_TYPE_CAN_SEND_MSG:
    case CT_IVCH_WRITECANOUTBOUND://190617,hpq
    case CT_IVCH_V_CV:
    case CT_IVCH_C_CV:
    case CT_IVCH_ACS:
    case CT_IVCH_IR:
    case CT_IVCH_ACIR:
    case CT_IVCH_ACR:
    case CT_SPECIAL_IRTEST:                        //IR异常特殊处理
    case CT_IVCH_CPULSE:						//电流脉冲处理
    case CT_IVCH_VPULSE:
    case CT_IVCH_SIMU_I:
    case CT_IVCH_SIMU_V:
    case CT_IVCH_SIMU_P:
    case CT_IVCH_SIMU_L:
    case CT_IVCH_SETVARIABLE:
        return -1;  // 暂不支持SCH在线更新的控制类型

    case CT_IVCH_PAUSE:
        UWord16  uw16_value = (int) f_Value + 0.1f;
        switch(uw16_value)
        {
        case ACIM_PAUSE:
        case T_CHAMBER_PAUSE:
            return -1;  // 暂不支持SCH在线更新的控制类型
        default:
            return 1;
        }
        return 1;

    case CT_TYPE_DO_SETTING:   //DO 输出时暂时不对通道操作 保持Rest
    case CT_IVCH_REST:
    case CT_IVCH_CURRENT:
    case CT_IVCH_CRAMP:
    case CT_IVCH_CSTAIRCASE:
    case CT_IVCH_CCCV:
    case CT_IVCH_CCCP:
    case CT_IVCH_DIGII:
    case CT_IVCH_DIGIV:
    case CT_IVCH_DIGIPOWER:
    case CT_IVCH_DIGILOAD:
    case CT_IVCH_CRATE:
    case CT_IVCH_POWER:
    case CT_IVCH_PRAMP:
//         case CT_IVCH_PSTAIRCASE:
    case CT_IVCH_LOAD:
    case CT_IVCH_VOLTAGE:
    case CT_IVCH_VRAMP:
    case CT_IVCH_VSTAIRCASE:
    case CT_IVCH_STOP:
        return 1;
    default:
        return 1;
    }
}

void ChannelInfo_Manager::OnlineUpdata_Clear(UWord16 uc_ChannelNo)
{
    m_Online_UpdataEnable[uc_ChannelNo] = false;
    Schedule_Manager_task::SchTimeTrigger[uc_ChannelNo] = 0;                    //Schedule计时标记
    ChannelSchedule[uc_ChannelNo].MsTimeScheduleTask = CommonFunc::GetSystemTime();
    ChannelInfo_Manager::DVmetaClear(uc_ChannelNo);
}

void ChannelInfo_Manager::OnlineUpdata_Load(St_Sche_Onlineupdata  * st_Onlineupdata, unsigned short uw16_ch, unsigned char uc_Index)//, unsigned char * uc_rangebit
{
    if(st_Onlineupdata->st_updata.m_ucValid_Stetting)
    {
        unsigned char sc_BufIdx = ChannelInfo_Manager::ChannelSchedule[uw16_ch].m_scBufIdx;
        if((sc_BufIdx < 0) || (sc_BufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
            return;

        bool IsNeedReloadStep = false;//zyx 2105291
        if(memcmp(&ChannelSchedule[uw16_ch].m_Settings.m_DV_Current, &ScheduleBuf[sc_BufIdx].m_Setting.m_DV_Current, sizeof(ChannelSchedule[uw16_ch].m_Settings.m_DV_Current)) 
         ||memcmp(&ChannelSchedule[uw16_ch].m_Settings.m_DV_Voltage, &ScheduleBuf[sc_BufIdx].m_Setting.m_DV_Voltage, sizeof(ChannelSchedule[uw16_ch].m_Settings.m_DV_Voltage)) 
         || (ChannelSchedule[uw16_ch].m_Settings.m_DV_Time_Max != ScheduleBuf[sc_BufIdx].m_Setting.m_DV_Time_Max)
         || (ChannelSchedule[uw16_ch].m_Settings.m_DV_Time_Min != ScheduleBuf[sc_BufIdx].m_Setting.m_DV_Time_Min))
            IsNeedReloadStep = true;				
				
        memcpy(&ChannelSchedule[uw16_ch].m_Settings,
               &ScheduleBuf[sc_BufIdx].m_Setting,
               sizeof(St_ScheduleSetting));   //  替换通道全局信息   replace Channel Global

//         IVSDL::ScheduleStartedRoutine(uw16_ch);
//         IVSDL::SetFactor_I(uw16_ch, ChannelInfo_Manager::ScheduleBuf[sc_BufIdx].m_Setting.m_fSdlFactor_I);
//         IVSDL::SetFactor_V(uw16_ch, ChannelInfo_Manager::ScheduleBuf[sc_BufIdx].m_Setting.m_fSdlFactor_V);
//         IVSDL::SetFactor_Capacity(uw16_ch, ChannelInfo_Manager::ScheduleBuf[sc_BufIdx].m_Setting.m_fSdlFactor_Capacity);
//         IVSDL::SetFactor_Energy(uw16_ch, ChannelInfo_Manager::ScheduleBuf[sc_BufIdx].m_Setting.m_fSdlFactor_Energy);
//         IVSDL::SetFactor_Time(uw16_ch,ChannelInfo_Manager::ScheduleBuf[sc_BufIdx].m_Setting.m_fSdlTime);

        safety_check::InitAbnormalCheck(uw16_ch);
        GetScheduleSafty(uw16_ch, MP_SCH_SAFTY_TYPE_CURRENT, &safety_check::Check_S[uw16_ch].checkSCH_I);
        GetScheduleSafty(uw16_ch, MP_SCH_SAFTY_TYPE_VOLTAGE, &safety_check::Check_S[uw16_ch].checkSCH_V);
        GetScheduleSafty(uw16_ch, MP_SCH_SAFTY_TYPE_POWER, &safety_check::Check_S[uw16_ch].checkSCH_P);
        GetScheduleSafty(uw16_ch, MP_SCH_SAFTY_TYPE_CAPACITY, &safety_check::Check_S[uw16_ch].checkSCH_C);
        GetScheduleSafty(uw16_ch, MP_SCH_SAFTY_TYPE_STEPTIME, &safety_check::Check_S[uw16_ch].checkSCH_t);
        ChannelInfo_Manager::LoadScheduleClamp(uw16_ch); //zyx 2105291
        MetaVariable_Task::update_dVdq(uw16_ch);
        MetaVariable_Task::update_dQdV(uw16_ch);
        MetaVariable_Task::InitDvdt(uw16_ch);
        if(uc_Index >= MAX_UPDATA_STEP_NUMBER && !IsNeedReloadStep)//zyx 2105291
            return;
    }
    m_Online_UpdataEnable[uw16_ch] = true;  // 方便下层处理
    st_Onlineupdata->st_updata.m_Step_UpData[0].m_ucValid_StepCtrl = st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_ucValid_StepCtrl; // 更新仅会运行一步，可统一标志

    LoadNextStep(uw16_ch, ChannelSchedule[uw16_ch].m_RunningStepIdx);
    if(st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_ucValid_StepCtrl)
    {
//         unsigned short uw16_point_Index = st_Onlineupdata->st_updata.m_ucStepNum[uc_Index];
//         unsigned char uc_rangebit = 0;  // 1-Curren;2-Voltage
//         if(memcmp((void *)&ScheduleBuf[ChannelSchedule[uw16_ch].m_scBufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_ucCurrentRange,
//                 (void *)&st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_Ctrl.m_ucCurrentRange,sizeof(unsigned char)))
//             uc_rangebit |= 0x01;   //  1-Curren;2-Voltage
//         if(memcmp((void *)&ScheduleBuf[ChannelSchedule[uw16_ch].m_scBufIdx].m_StepData[uw16_point_Index].m_Ctrl.m_ucVoltageRange,
//             (void *)&st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_Ctrl.m_ucVoltageRange,sizeof(unsigned char)))
//             uc_rangebit |= 0x02;   //  1-Curren;2-Voltage


        Step_Manager_task::DacOutputCounter = 0;
        Step_Manager_task::StepClear_Adjust(uw16_ch); // 清一次微调参数重新开始计数
        MetaVariable_Task::calculate_capacity(1, uw16_ch); // 避免容量不准确问题

        CQ::InitQueue(HistoryData[uw16_ch].m_uCurrHnd);  // 避免容量不准确问题
        HistoryData[uw16_ch].m_u32CurrentSum = 0;
        IV_Buffer1[uw16_ch].m_uCurrCount = 0;

        CQ::InitQueue(HistoryData[uw16_ch].m_uVoltHnd);  // 避免容量不准确问题
        HistoryData[uw16_ch].m_u32VoltageSum = 0;
        IV_Buffer1[uw16_ch].m_uVoltCount = 0;

        if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uw16_ch))
        {
            ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[uw16_ch].m_GroupID].m_stSubGroupCtrl.m_ucCANCMDType = CAN_CMD_PARA_START;
        }

        unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uw16_ch].m_ucActiveIdx;
        Step_Manager_task::Get_StepRunInfo(uw16_ch, ChannelInfo_Manager::ChannelStep[uw16_ch].m_Info[ActiveIdx].m_StepInfo);
        //zyx 2105291
        Step_Manager_task::m_Range[uw16_ch] = ChannelInfo_Manager::GetMatchedCurrentRange(uw16_ch, \
                                              ChannelInfo_Manager::ChannelSchedule[uw16_ch].m_TestObject.m_fMaxCurrentCharge,\
                                              &st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_Ctrl);//zyx 2105181
        //st_Onlineupdata->st_updata.m_Step_UpData[uc_Index].m_Ctrl.m_ucCurrentRange; // 1更新变换
        Step_Manager_task::PrepareForRun(uw16_ch);  // 并联时需要向从MCU发送更新值。 2计算加载
        Step_Manager_task::m_Range[uw16_ch] = GetPhysicalCurrentRange(Step_Manager_task::m_Range[uw16_ch]);

        // DigitalV 针对无硬件电压时，使用电流控制电压。避免图型出现锯齿情况，执行时序解决。
//         switch(ChannelInfo_Manager::ChannelStep[uw16_ch].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
//         {
//         case CT_IVCH_VOLTAGE:
//         case CT_IVCH_DIGIV:
//         case CT_IVCH_CCCV:
//             uc_rangebit[uc_Index] = 0XFF;
//             break;
// //              case CT_IVCH_DIGII:
// //              case CT_IVCH_VRAMP:
// // 	         case CT_IVCH_VSTAIRCASE:
//         default:
//             uc_rangebit[uc_Index] = 0XFF;  // 解决电流输出不稳定和记录少点问题。
//             break;
//         }
//         if(uc_rangebit[uc_Index])
//         {
        ChannelInfo[uw16_ch].StepTransationToStatus = STEPRUNNING;
        ChannelInfo[uw16_ch].StepStatus = STEPTRANSITION;
//         }
//         else
//         {
//             ChannelInfo_Manager::OnlineUpdata_Clear(uw16_ch);
//         }
    }
    else
    {
        ChannelInfo_Manager::OnlineUpdata_Clear(uw16_ch);
    }
}

void ChannelInfo_Manager::OnlineUpdata_AUXLoad(unsigned short uw16_ch)
{
    return;  //  封锁涉及辅助的在线更新
    /*
        Step_Manager_task::AuxLimitAckMgr[uw16_ch].AckStatus=AUXACK_IDLE;  //重新开始
        Step_Manager_task::AuxLimitAckMgr[uw16_ch].bNeedWattingAck = 1; // 重新发送Limit
        Step_Manager_task::AuxLimitAckMgr[uw16_ch].ReDwonLimitCnt=0;   //清重发标记
        Step_Manager_task::AuxLimitAckMgr[uw16_ch].WaitAckCnt = 0;

        for(unsigned char uc_AuxIdx = 0;uc_AuxIdx<MAXAUXUNIT;uc_AuxIdx++)
        {
            if(CANBusParse::st_IV2AuxInfoMgr[uw16_ch][uc_AuxIdx].m_bExist != 1)
                continue;

            CommonFunc::GetTimeTicksUpdate(&CANBusParse::st_IV2AuxInfoMgr[uw16_ch][uc_AuxIdx].m_checkAuxInfoTick);	 // reset counter
            CANBusParse::st_IV2AuxInfoMgr[uw16_ch][uc_AuxIdx].m_askedCount = 0;	  // 先清除IV检查AUX超时和次数
            CANBusParse::st_IV2AuxInfoMgr[uw16_ch][uc_AuxIdx].m_errorCount = 0;
            CANBusParse::st_IV2AuxInfoMgr[uw16_ch][uc_AuxIdx].m_startAskFlag = 0;//启动询问标志（代表恢复normal状态)
        }

        unsigned char sc_BufIdx = ChannelInfo_Manager::ChannelSchedule[uw16_ch].m_scBufIdx;
        Schedule_Manager_task::ClearMapAUXSafetyInfo(uw16_ch,ALLTYPE,0,ALLMAPCHANNEL);//20131121 dirui 在Schedule结束之后清除AUX的安全信息
        Schedule_Manager_task::ClearAUXLimits(uw16_ch,0);
        Step_Manager_task::AuxLimitAckMgr[uw16_ch].bAuxAckFailed = 0; //reset  temp
        CANBusParse::st_SNCheckTab[uw16_ch][LIMITSN].m_SNCount=0;
        CANBusParse::st_SNCheckTab[uw16_ch][SAFETYSN].m_SNCount=0;
        Schedule_Manager_task::SendSafetyInfoToAUX(uw16_ch);
        Schedule_Manager_task::SendSDLParaToAUX(uw16_ch);

        ChannelInfo[uw16_ch].StepTransationToStatus=STEPRUNNING;
        ChannelInfo[uw16_ch].StepStatus=STEPTRANSITION;  */
}
void Channel_MapAuxInfo::InitAuxMap(unsigned uc_channelNo) //180821 zyx
{
    for(unsigned char AuxType = 0; AuxType < MAXAUXTYPE; AuxType++) //190108 zyx
        IVChanMapAuxCount[uc_channelNo][AuxType] = 0;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Voltage.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_VOLTAGE] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Voltage.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Temperature.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_TEMPERATURE] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Temperature.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Pressure.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_PRESSURE] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Pressure.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_ExtCurrent.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_EXTERNALCHARGE] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_ExtCurrent.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_FlowRate.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_FLOW_RATE] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_FlowRate.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Humidity.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_HUMIDITY] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_Humidity.m_SafetyChanCount;

    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_AO.m_SafetyChanCount > 0)//180911 zyx
        IVChanMapAuxCount[uc_channelNo][AUX_AO] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_AO.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_DI.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_DI] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_DI.m_SafetyChanCount;
    if(ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_DO.m_SafetyChanCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_DO] = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_DO.m_SafetyChanCount;

    IVChanMapAuxCount[uc_channelNo][AUX_CURRENT] = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[AUX_CURRENT];
    IVChanMapAuxCount[uc_channelNo][AUX_ACIM] = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[AUX_ACIM];
//    IVChanMapAuxCount[uc_channelNo][AUX_BATTERYCELLS] = Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[AUX_BATTERYCELLS]; //zyx 190418

    int AuxSafeCount = ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_SafetyVoltage.m_SafetyChanCount;
//                        + ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_SaftyCurrent.m_SafetyChanCount \
//                        + ChannelInfo_Manager::ChannelSchedule[uc_channelNo].m_Settings.m_SchAux_Glb_SafetyTemperature.m_SafetyChanCount;
    if(AuxSafeCount > 0)
        IVChanMapAuxCount[uc_channelNo][AUX_SAFETY] = AuxSafeCount;
    int totalAuxChan = 0;
    for(unsigned char i = 0; i < MAXAUXTYPE; i++)
        totalAuxChan += IVChanMapAuxCount[uc_channelNo][i];
    Channel_MapAuxInfo::m_Total_In_OneIVCh[uc_channelNo] = totalAuxChan; //st_IVUnit4AuxMap->m_Total_In_OneIVCh[uc_idx];  // IVCH,Not AuxType 20151204 ying
    AuxMapBySchedule(uc_channelNo);
}
unsigned int    Channel_MapAuxInfo::HowManyAuxTypeMap(unsigned char uc_channelNo, unsigned char AuxType) //return Aux type map count //180821 zyx
{
    if(Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[AuxType] <= 0)
        return 0;
    else
        return Channel_MapAuxInfo::PC2IVUnit4AuxMap[uc_channelNo].m_Total_In_OneIVCh[AuxType];//IVChanMapAuxCount[uc_channelNo][AuxType]; //180825 zyx
}
float  ChannelInfo_Manager::GetMaxCurrRangeValue(unsigned char  Chn,unsigned char IRange)
{
    St_RangeBound RangeBound = ChannelInfo_Manager::BoardCFG.m_Channel[Chn].m_IRange[IRange].m_Bound;
    if(abs(RangeBound.m_High) >  abs(RangeBound.m_Low))
        return  abs(RangeBound.m_High);
    else
        return  abs(RangeBound.m_Low);
}

float  ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(unsigned char  Chn,unsigned char IRange)
{
    float MaxRangeValue = GetMaxCurrRangeValue(Chn,  IRange);
    MaxRangeValue = MaxRangeValue * 2;
    return MaxRangeValue;
}
float  ChannelInfo_Manager::GetCurrRangeHighOffsetValue(unsigned char  Chn,unsigned char IRange, float OffsetPrecent)
{
    St_RangeBound RangeBound = ChannelInfo_Manager::BoardCFG.m_Channel[Chn].m_IRange[IRange].m_Bound;
    return RangeBound.m_High + GetMaxCurrRangeValue(Chn,IRange)*(OffsetPrecent-1.0f);
}
float  ChannelInfo_Manager::GetCurrRangeLowOffsetValue(unsigned char  Chn,unsigned char IRange, float OffsetPrecent)
{
    St_RangeBound RangeBound = ChannelInfo_Manager::BoardCFG.m_Channel[Chn].m_IRange[IRange].m_Bound;
    return RangeBound.m_Low - GetMaxCurrRangeValue(Chn,IRange)*(OffsetPrecent-1.0f);
}
