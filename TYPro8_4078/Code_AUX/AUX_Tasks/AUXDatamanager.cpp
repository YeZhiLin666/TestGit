///////////////////////////////////////////////////////////////////////////////////
//File Name: AUXDatamanager.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助数据管理操作
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////

#include "../Entry/includes.h"
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
// 32K列表，表一个0XFF代表8个字节存储位。
const unsigned char     Aux_DataManager::FramConfigDictionary[32*1024/8] =
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
};

#if DEBUG_TRACE_RECORD == 1
St_DebugTraceRecord     Aux_DataManager::DebugTraceRecord;
#endif


//----// K型温度板-----------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在0(°C)到1372(°C)
//从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
const float  f_CoEA_Ktype[9 ]= {0.0f,  2.5173462e-2,    -1.1662878e-6,  -1.0833638e-9,   -8.977354e-13,
                                -3.7342377e-16,  -8.6632643e-20, -1.0450598e-23,  -5.1920577e-28
                                };
//温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
const float  f_CoEB_Ktype[10] = {    0.0f,     2.508355e-2,    7.860106e-8,   -2.503131e-10,   8.31527e-14,
                                     -1.228034e-17,     9.804036e-22,   -4.413030e-26,    1.057734e-30,   -1.052755e-35
                                     };
//温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
const float  f_CoEC_Ktype[7] = {  -1.318058e2, 4.830222e-2,  -1.646031e-6, 5.464731e-11, -9.650715e-16,  8.802193e-21,  -3.110810e-26  };

//---------// K型温度板-----------------------------------------------------------------------------------------------------

//--------// T型温度板-----------------------------------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在0(°C)到400(°C)        T type thermocouple
//T_type热电偶从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
const float    f_CoEA_Ttype[8] = {   0.0,  2.5949192e-2, -2.1316967e-7,  7.9018692e-10, 4.2527777e-13,
                                     1.3304473e-16,  2.0241446e-20, 1.2668171e-24
                                 };
//温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
const float    f_CoEB_Ttype[7] = {   0.0,   2.5928e-2, -7.602961e-7,  4.637791e-11, -2.165394e-15,
                                     6.048144e-20,  -7.293422e-25
                                 };

//-------// T型温度板-----------------------------------------------------------------------------------------------------------

//----// J型温度板-----------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在-210(°C)到1200 (°C)
//从微伏电压转化为摄时温度定义常数系数
//温度范围在-210(°C)到0(°C) ，微伏电压范围在-8095(μV)到0(μV)
const float    f_CoEA_Jtype[9] = {   0.0f,  1.9528268e-2, -1.2286185e-6,  -1.0752178e-9, -5.9086933e-13,
                                     -1.7256713e-16,  -2.8131513e-20, -2.3963370e-24,  -8.3823321e-29
                                     };
//温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
const float    f_CoEB_Jtype[8] = {    0.0f,     1.978425e-2,    -2.001204e-7,   1.036969e-11,   -2.549687e-16,
                                      3.585153e-21,     -5.344285e-26,   5.099890e-31
                                 };
//温度范围在760(°C)到1200(°C) ，微伏电压范围在42929(μV)到69553(μV)
const float    f_CoEC_Jtype[6] = {  -3.11358187e3, 3.00543684e-1,  -9.94773230e-6, 1.70276630e-10, -1.43033468e-15,4.73886084e-21 };

//---------// J型温度板-----------------------------------------------------------------------------------------------------

//--------// E型温度板-----------------------------------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在-200(°C)到1000(°C)        E type thermocouple
//T_type热电偶从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-8825(μV)到0(μV)
const float    f_CoEA_Etype[9] = {   0.0,  1.6977288e-2, -4.3514970e-7,  -1.5859697e-10, -9.2502871e-14,
                                     -2.6084314e-17,  -4.1360199e-21, -3.4034030e-25, -1.1564890e-29
                                     };
//温度范围在0(°C)到1000(°C) ，微伏电压范围在0(μV)到76373(μV)
const float    f_CoEB_Etype[10] = {   0.0,   1.7057035e-2, -2.3301759e-7,  6.5435585e-12, -7.3562749e-17,
                                      -1.78960001e-21,  8.4036165e-26, -1.3735879e-30, 1.0629823e-35, -3.2447087e-41
                                      };

//-------// E型温度板-----------------------------------------------------------------------------------------------------------
st_Aux_Chn_Info 	Aux_DataManager::st_Aux_info[MAX_BOARD_PER_AUXMCU][17];	 //类似于channle info  //临时测试

st_Aux_Sample 		Aux_DataManager::st_aux_sample_setting[MAX_BOARD_PER_AUXMCU][17];

ST_AUX_ADC_CS     Aux_DataManager::Aux_ADC_CS_Info[16];//Add by DKQ 05.04.2015目前最多支持16个ADC片选地址
unsigned short    Aux_DataManager::Aux_ADC_TimeOut_Flag;
unsigned char     Aux_DataManager::Aux_ADC_TotalCnt;  // add byqjm 05.05.2015

//ST_AUXSAMPLE 	  		Aux_DataManager::m_AuxSample;
ST_AUX_BOARDCFG	   	Aux_DataManager::m_Aux_BoardCFG[MAX_BOARD_PER_AUXMCU];   //存储用于分层处理的板信息
signed int          Aux_DataManager::m_Count_Limit_AllChannel[MAX_IVMCU_PER_SYS];  //
signed int          Aux_DataManager::m_Count_AnyLimit_AllChannel[MAX_IVMCU_PER_SYS];
ST_MAP_FROM_PC		   Aux_DataManager::m_MapFromPC[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
MsTime              Aux_DataManager::m_TriggerLogTime[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];//160429
//St_DOWNLOAD_MAP_FROM_PC		Aux_DataManager::m_DownLoadMapFromPC[MAX_AUXCH_PER_AUXMCU]; //供存放来自PC的MAP信息
St_DOWNLOAD_MAP_FROM_PC		Aux_DataManager::m_DownLoadMapFromPC[MAX_MAP_COUNT_PER_UNIT]; //供存放来自PC的MAP信息

ST_IVMCUID_CFG      Aux_DataManager::m_IVMCU_ID_Info[MAX_IVMCU_PER_SYS];


ST_AUX_BOARD_INFO  	    Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU];  //用于存储扫描EEPROM获取的信息
ST_AUX_BOARD_INFO_IN_EEPROM  Aux_DataManager::m_AuxBoardInfo4EEPROM;  //用于存取和读取E2PROM里的板信息。
// ST_AUX_BOARD_INFO_IN_EEPROM  Aux_DataManager::m_AuxBoardInfoRead[MAX_BOARD_PER_AUXMCU];  //用于存取和读取E2PROM里的板信息。
ST_AUX_IN_UNIT_LOOKUP_TABLE    Aux_DataManager::m_AuxChnLookupTable [MAX_AUX_TYPE]; //
ST_AUX_CHN_LOCATION            Aux_DataManager::m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU];    //用于记录该unit内每个辅助通道的物理地址，即板地址和电气通道地址



ST_AUX_DATA  		      Aux_DataManager::m_AuxDataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//用于按类型存放辅助通道数据.
//ST_AUX_DIDO_DATA  		Aux_DataManager::m_Aux_DIDO_DataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//用于按类型存放非ADC型辅助通道数据.

unsigned char         Aux_DataManager::m_DGIdx2BIdx[MAX_DATA_GROUP]; //记录每个Data_Group_Index 对应的板地址Board_Index
ST_AUX_P2E_IN_UNIT         Aux_DataManager::m_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//  (PannelChannel number to Electic Channel Number)
//unsigned char         Aux_DataManager::m_DIDO_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//  (PannelChannel number to Electic Channel Number)

bool                  Aux_DataManager::m_DataGroupMgr[MAX_DATA_GROUP]; //用于标记每个data group 是否已生效
//bool                  Aux_DataManager::m_DIDO_DataGroupMgr[MAX_DATA_GROUP]; //用于标记每个data group 是否已生效

ST_INCHIP_TEMP        Aux_DataManager::m_InChipTemperature[MAX_DATA_GROUP];  //用于存放ADC片内温度

ST_AUX_LIMIT 		    Aux_DataManager::m_AuxLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_LIMIT_PER_IVCH];
unsigned char 			Aux_DataManager::m_AuxLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

ST_AUX_IFANY_LIMIT 	    Aux_DataManager::m_AuxIfAnyLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_IFANY_LIMIT_PER_IVCH];
unsigned char 			Aux_DataManager::m_AuxIfAnyLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
ST_AUX_SAFETY				    Aux_DataManager::m_AuxSafetyBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
unsigned char 				 	Aux_DataManager::m_AuxSafetyCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //每个IV通道下的辅助通道safety设置个数可能不一样

ST_AUX_SDL					    Aux_DataManager::m_AuxSDLBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];//SDL参数按辅助类型分，但IV通道之间可以有所不同

ST_AUX_DATA_REQUEST 		    Aux_DataManager::m_AuxDataRequestBank [MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][3*MAX_AUXCH_PER_AUXMCU]; //用于登记那些辅助通道数据被请求了
unsigned char 				 	Aux_DataManager::m_AuxDataRequestCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //每个IV通道下的辅助通道数据请求个数
unsigned char        		Aux_DataManager:: ADC_SampleProcessType[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];

bool                    Aux_DataManager::CANBMSEnableFlag=0;
bool           Aux_DataManager::m_Enable_2ndV_LC_Sample = false;  // Enable Low Cost 2nd Votlage boards sample task;
bool           Aux_DataManager::m_Enable_2ndV_BC_Sample = false;  // Enable Battery Cell 2nd Votlage boards sample task;

bool                    Aux_DataManager::m_Inital_GPIO_4_LC_Boards = false ; // Not initial the GPIO for 2ndV_LC boards yet
//DIDO manager
ST_AUX_DOCtrl                 Aux_DataManager::m_AuxDOCtrlMgr[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

ST_AUX_DATA    Aux_DataManager::st_Aux_Data;
bool                    Aux_DataManager::m_bLogAuxDxdt;    //160429

signed char  Aux_DataManager::AuxTypeIndexTable[MAX_AUX_TYPE_PER_MCU];//辅助限制
#pragma arm section //结束外存变量定义


#if DEBUG_TRACE_RECORD == 1
void Aux_DataManager::InitDebugTraceRecord(void)
{
    INIT_QUEUE(DebugTraceRecord);
}
void Aux_DataManager::PushDebugTraceRecord(St_DebugPoint myDebugPoint)
{
    ENQUEUE(DebugTraceRecord,DEBUG_TRACE_RECORD_DEPTH,myDebugPoint);
}
#endif


//暂时手动填充m_AllAuxBoards，作为AUX板信息
//采样1级2级均用此做判断    因为m_AllAuxBoards和我使用的还有较大差距，内部没有状态机，需要考虑如何整合一起
void Aux_DataManager::Init(void)
{
#if DEBUG_TRACE_RECORD == 1
    InitDebugTraceRecord();
#endif

    unsigned char uc_IVunitBufIndex = 0;
    do {
        m_Count_Limit_AllChannel[uc_IVunitBufIndex] = 0;
        m_Count_AnyLimit_AllChannel[uc_IVunitBufIndex] = 0;
    } while(MYIVUNITINDEX(uc_IVunitBufIndex));
    memset(&m_AuxDataBank,0,sizeof(m_AuxDataBank));   //Add by DKQ 08.14.2015
    memset(&m_AuxChnLookupTable,0,sizeof(m_AuxChnLookupTable));
    memset(&m_AuxChanLongSnake,0,sizeof(m_AuxChanLongSnake));
    memset(&Aux_ADC_CS_Info,0,sizeof(Aux_ADC_CS_Info));
    Aux_ADC_TimeOut_Flag = 0;
    Aux_ADC_TotalCnt = 0;
    memset(&m_Aux_BoardCFG,0,sizeof(m_Aux_BoardCFG));
    memset(&m_DownLoadMapFromPC,0,sizeof(m_DownLoadMapFromPC));
    memset(&m_IVMCU_ID_Info,0,sizeof(m_IVMCU_ID_Info));
    memset(&m_DGIdx2BIdx,0,sizeof(m_DGIdx2BIdx));
    memset(&m_PChn2EChn,0,sizeof(m_PChn2EChn));
    memset(&m_DataGroupMgr,0,sizeof(m_DataGroupMgr));
    memset(&m_InChipTemperature,0,sizeof(m_InChipTemperature));
    memset(&m_AuxDOCtrlMgr,0,sizeof(&m_AuxDOCtrlMgr));
    memset(&m_AuxLimitBank,0,sizeof(m_AuxLimitBank));
    memset(&m_AuxIfAnyLimitBank,0,sizeof(m_AuxIfAnyLimitBank));

    for (unsigned char i = 0; i<MAX_BOARD_PER_AUXMCU; i++)
    {
        memset(&m_AllAuxBoards[i],0,sizeof(ST_AUX_BOARD_INFO));

    }
    memset(&m_AuxBoardInfo4EEPROM,0,sizeof(m_AuxBoardInfo4EEPROM));


    uc_IVunitBufIndex = 0;
    do {
        for (unsigned char i = 0; i<MAX_IVCH_PER_IVMCU; i++)
        {
            m_AuxLimitCount[uc_IVunitBufIndex][i] = 0;  //初始化清除所有limit
            m_AuxIfAnyLimitCount[uc_IVunitBufIndex][i] = 0; //初始化清除所有IfAny limit

            for(unsigned char j = 0; j< MAX_AUX_TYPE_PER_MCU; j++)//辅助限制
            {
                memset(&m_MapFromPC[uc_IVunitBufIndex][i][j],0,sizeof(ST_MAP_FROM_PC));
            }
        }
    } while(MYIVUNITINDEX(uc_IVunitBufIndex));

    Aux_DataManager::CANBMSEnableFlag=0;
    Init_BoardCFG();
    for(unsigned char i = 0; i<MAX_AUX_TYPE_PER_MCU; i++) //辅助限制
    {
        AuxTypeIndexTable[i] = -1;
    }
// 	INIT_QUEUE(CANFunc::CQ_CanBmsRxBuf2);

}

//下面的函数用于将从PC下载来的map信息转换成分配给各个IVMCU的各个IV通道的各类型的辅助通道在本unit内的各个辅助通道放到
// m_MapFromPC中去。
// 新的辅助不再要求连续MAP
// 要注意的时，对每一个IVMCU，将其unit ID　要存放在m_IVMCU_ID中，因为IVMCU的ID在不辅助中不再是连续的。而是断续的。　
// 还有，每有一个IVMCU map 到本辅助unit，要将 m_Valid_IV[ ]置1，以便应用程序快速判断跳过空的MAP　
//待和PC机一起调试


unsigned long Aux_DataManager::Aux_GetMapfromPC(unsigned char * uc_ErrorFlag)
{
    MP_AllIVMap    * Lst_AllIVMap;  // Local Mete
    Lst_AllIVMap = (MP_AllIVMap   *) &EthernetParse::m_btPackDataBuffer[0];

    unsigned short int uw16_ChSum = 0;
    for (unsigned char i=0; i<MAX_IVMCU_PER_SYS; i++)
    {   // 统计MAP表中辅助通道总数量
        for (unsigned char j=0; j<MAX_IVCH_PER_IVMCU; j++)
        {
            for (unsigned char k=0; k<MAX_AUX_TYPE; k++)
                uw16_ChSum += Lst_AllIVMap->m_Total_In_ThisIVCh[i][j][k];  // 统计MAP表中辅助通道总数量
        }
    }

    for(unsigned short i=0; i<MAX_MAP_COUNT_PER_UNIT; i++)
        memset(&m_DownLoadMapFromPC[i],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));

    if(uw16_ChSum <= 0)
        return sizeof(MP_AllIVMap);

    unsigned char uc_valid = 0;
    unsigned short int uw16_count = 0;
    for(unsigned short i=0; i<MAX_MAP_COUNT_PER_UNIT; i++)
    {
        //memset(&m_DownLoadMapFromPC[i],0,sizeof(ST_DOWNLOAD_MAP_FROM_PC));
        uc_valid= Lst_AllIVMap->m_AllIVMap[i].m_valid;
        if(uc_valid==1)
        {
            m_DownLoadMapFromPC[i]= Lst_AllIVMap->m_AllIVMap[i];
            if(++uw16_count >= uw16_ChSum)   // 统计MAP表中辅助通道总数量
                break;
        }
    }

    //unsigned char uc_IVunitBufIndex = 0;
    unsigned long uw32_Get = sizeof(St_DOWNLOAD_MAP_FROM_PC);
    /*   unsigned long uw32_length = sizeof(MP_AllIVMap);
      uw32_length -=  uw32_Get *  MAX_MAP_COUNT_PER_UNIT; // 计算一次包中前面管理信息长度
      */
    unsigned long uw32_length = (sizeof(MP_AllIVMap) - uw32_Get * MAX_MAP_COUNT_PER_UNIT); // 计算一次包中前面管理信息长度

//   for(unsigned short i=0;i<(MAX_MAP_COUNT_PER_UNIT-uw16_ChSum);i++)
//        memset(&EthernetParse::m_btPackDataBuffer[uw32_length + (uw16_ChSum+i)*uw32_Get],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));  // 清理没下载满512条的空间，避免读出时出现乱数。
    for(unsigned short i=uw16_ChSum; i<(MAX_MAP_COUNT_PER_UNIT-uw16_ChSum); i++)
        memset(&Lst_AllIVMap->m_AllIVMap[i],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));  // 清理没下载满512条的空间，避免读出时出现乱数。

    uw32_length += (uw32_Get * uw16_ChSum);  // 计算出实际一维字节数量
    return (uw32_length);  // 上传一维存储字节长度
}

void Aux_DataManager::Aux_ScanIVMCU_UnitID(void)  //扫描都有MAP到该辅助unit的IVunit的unit　ＩＤ并放到 m_IVMCU_ID_Info中去。
{
    unsigned short ptr=0;
    unsigned short count=0;
    memset(&m_IVMCU_ID_Info,0,sizeof(ST_IVMCUID_CFG)*MAX_IVMCU_PER_SYS);
    m_IVMCU_ID_Info[0].m_ID = m_DownLoadMapFromPC[0].m_IVUnit ;  //第一个IVunit
    m_IVMCU_ID_Info[0].m_Valid= 1;
    ptr++;
    count++;
    for (unsigned short i = 1; i< MAX_MAP_COUNT_PER_UNIT; i++)   //    MAX_AUXCH_PER_AUXMCU
    {
        if(!m_DownLoadMapFromPC[i].m_valid)
            continue;
        for (unsigned char j=0; j< count; j++)  //查询是否已经有该IVｕｎｉｔ了。
        {
            if (  m_IVMCU_ID_Info[j].m_ID == m_DownLoadMapFromPC[i].m_IVUnit )
                break; //如有，不再找了，
            else
            {
                if (j == (count-1) ) //查找完了都没有找到相同的
                {
                    m_IVMCU_ID_Info[ptr].m_ID = m_DownLoadMapFromPC[i].m_IVUnit ;
                    m_IVMCU_ID_Info[ptr].m_Valid= 1;
                    ptr++;
                    count++;
                }
            }
        }
    }
}



void Aux_DataManager::Aux_TranslateMapFromPC(void )   //将下载的MAP信息按 IVUNIT->IVCH->辅助类型展开,并把在DataBank中的位置找出来;
{

    unsigned char ptr=0;
    //     unsigned char counter=0;

    Aux_ScanIVMCU_UnitID();
//     MultiMcuComm::Scan_IV_AUXMAP();

    unsigned char m_SeekUnitID = 0;
    MultiMcuComm::Scan_Clear(UDP_MAP_AUX);

    for (unsigned char i=0; i<MAX_IVMCU_PER_SYS; i++)
    {
        for (unsigned char j=0; j<MAX_IVCH_PER_IVMCU; j++)
        {
            for (unsigned char k=0; k<MAX_AUX_TYPE; k++)
            {
                unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(k);
                if(TempAuxTypeIdex == 0xFF)
                    continue;//辅助限制  查找该辅助类型是否存在，
                m_MapFromPC[i][j][TempAuxTypeIdex]. m_ActualCounts = 0;  //为每一个辅助类型查找之前先清0
                ptr=0;
                for(unsigned short m=0; m<MAX_MAP_COUNT_PER_UNIT; m++)
                {
                    if (!m_DownLoadMapFromPC[m].m_valid)
                        continue;

                    if ((m_DownLoadMapFromPC[m].m_IVUnit == m_IVMCU_ID_Info[i].m_ID)
                            &&(m_DownLoadMapFromPC[m].m_IVCh == j)&&(m_DownLoadMapFromPC[m].m_AuxType == k))
                    {
                        m_SeekUnitID = Aux_DataManager::m_DownLoadMapFromPC[m].m_IVUnit ;
                        MultiMcuComm::Scan_EmptySeat(m_SeekUnitID,j,UDP_MAP_AUX);

                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_Index_InIVCh = (m_DownLoadMapFromPC[m].m_Index_InIVCh_HighBit << 8) | (m_DownLoadMapFromPC[m].m_Index_InIVCh & 0xff);//m_Index_InIVCh_HighBit为m_Index_InIVCh的高位
                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_Index_InAuxUnit = m_DownLoadMapFromPC[m].m_Index_InAuxUnit;
                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_data_group_idx = m_AuxChanLongSnake[m_AuxChnLookupTable [k].m_StartInSnake + m_DownLoadMapFromPC[m].m_Index_InAuxUnit].m_data_group_idx ;
                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_chn = m_AuxChanLongSnake[m_AuxChnLookupTable [k].m_StartInSnake + m_DownLoadMapFromPC[m].m_Index_InAuxUnit].m_chn ;
                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_valid = true;
                        m_MapFromPC[i][j][TempAuxTypeIdex].m_ActualCounts ++;
                        if(++ptr >= MAX_AUXCH_PER_AUXMCU)
                            break;
                        CommonFunc::uc_Support_AUX = 1;

                    }
                }



            }

        }
    }
    MultiMcuComm::Build_NewTable();
}





unsigned char Aux_DataManager::Aux_LookupIVunitID (unsigned char uc_ID)
{
    for (unsigned char uc_i = 0; uc_i < MAX_IVMCU_PER_SYS; uc_i++)
    {
        if (m_IVMCU_ID_Info[uc_i].m_ID == uc_ID)
            return uc_i;
    }
    return 255;
}






//下面的函数用I2C去读取板信息并保持到 m_AllAuxBoards; 待调试


//根据PC下来的配置信息m_MapFromPC 及从板上获取的各个辅助通道的实际物理信息 m_AllAuxBoards建立各个辅助的物理地址表，
//按照IV通道的顺序及各种辅助通道的顺序排列成一个“长蛇阵” m_AllChnsInAuxUnit,及其索引表 m_Iv2AuxMap



//在读完所有的板的EEPROM 信息后， m_AllAuxBoards, m_AuxDataBank, m_PChn2EChn, 应该就能拥有足够的信息来建立
// m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU] 和 m_AuxChnLookupTable [MAX_AUX_TYPE]; //

void Aux_DataManager::Aux_BuildLongSnake(void)
{

    unsigned char uc_SnakePointer =0;
    for (unsigned char j=0; j< MAX_AUX_TYPE; j++)   //扫描所有的辅助类型
    {

        m_AuxChnLookupTable[j].m_TotalChnNum =0;  //该类型的通道数初始化为0
        m_AuxChnLookupTable[j].m_StartInSnake  = uc_SnakePointer;  //记下该类型的在长蛇阵中起点
        // if((j!=AUX_DO)&&(j!=AUX_DI))   //如果是非DIDO的类型， 从 m_AuxDataBank里找
        {   for (unsigned char i=0; i<MAX_DATA_GROUP; i++) //扫描所有的DATA_GROUP
            {
                if(!m_DataGroupMgr[i])
                    continue;

                for(unsigned char k=0; k<MAX_AUXCH_PER_DATA_GROUP; k++)  //扫描每个DATAgroup的每个面板通道
                {
                    if(!m_AuxDataBank[m_PChn2EChn[i][k].e_data_group_id][m_PChn2EChn[i][k].e_chan_id].m_Chn_Info.m_Populated)
                        continue;
                    if(m_AuxDataBank[m_PChn2EChn[i][k].e_data_group_id][m_PChn2EChn[i][k].e_chan_id].m_Chn_Info.m_AuxType == j) //查看AuxDataBank里该面板通道对应的电气通道的
                        //辅助类型乎合正在扫描的类型
                    {
                        m_AuxChnLookupTable[j].m_TotalChnNum ++;      //该类型的通道数加1
                        m_AuxChanLongSnake [uc_SnakePointer].m_valid = 1;
                        m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[i][k].e_data_group_id;  //data_group_idx 放长蛇阵中
                        m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[i][k].e_chan_id;  //电气通道地址放长蛇中
                        uc_SnakePointer++;
                    }
                }
            }
        }

    }
//============add by zc 01192018  for TEC I2C
    if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932) // 04.25.2018 ZYF
    {
        if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave)
        {
            m_AuxDataBank[m_PChn2EChn[1][0].e_data_group_id][m_PChn2EChn[1][0].e_chan_id].m_Chn_Info.m_Populated = 1;
            m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[1][0].e_data_group_id;  //data_group_idx 放长蛇阵中
            m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[1][0].e_chan_id;  //电气通道地址放长蛇中
        }
        else
        {
            m_AuxDataBank[m_PChn2EChn[0][8].e_data_group_id][m_PChn2EChn[0][8].e_chan_id].m_Chn_Info.m_Populated = 1;
            m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[0][8].e_data_group_id;  //data_group_idx 放长蛇阵中
            m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[0][8].e_chan_id;  //电气通道地址放长蛇中
        }
        m_AuxChnLookupTable[1].m_TotalChnNum ++;      //该类型的通道数加1
        m_AuxChanLongSnake [uc_SnakePointer].m_valid = 1;
        uc_SnakePointer++;
    }
}



//根据PC下来的配置信息m_MapFromPC 及从板上获取的各个辅助通道的实际物理信息 m_AllAuxBoards建立各个辅助的物理地址表，
//按照IV通道的顺序及各种辅助通道的顺序排列成一个“长蛇阵” m_AllChnsInAuxUnit,及其索引表 m_Iv2AuxMap

//这个函数用于转换实际UNITID号 和 存放IVMUC的信息的BUFFER 序号。因为buffer的序号并不等于IVMCU UnitID号
unsigned char Aux_DataManager::Aux_LocateIVUnitBufferIndex(unsigned char uc_IV_unitID)
{
    unsigned char uc_i;
    for (uc_i = 0; uc_i < MAX_IVMCU_PER_SYS ; uc_i++)
    {
        if(m_IVMCU_ID_Info[uc_i].m_Valid)  //如果没有ＩＶＭＣＵ被ＭＡＰ跳过
        {
            if(m_IVMCU_ID_Info[uc_i].m_ID == uc_IV_unitID)
                return uc_i;
        }
    }
    return 0xFF; //如果没找到的话，返回一个0xFF
}
unsigned char Aux_DataManager::Aux_LocateAuxBufferIndex(unsigned char IvUnitBufIdx,unsigned char IvChanIdx,unsigned char AuxType,unsigned char AuxIdxIn_AuxUnit)
{
    unsigned char uc_i;

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
        return 0xFF;

    for (uc_i = 0; uc_i < Aux_DataManager::m_MapFromPC[IvUnitBufIdx][IvChanIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap; uc_i++)
    {
        if(Aux_DataManager::m_MapFromPC[IvUnitBufIdx][IvChanIdx][TempAuxTypeIdex].m_MapInfo[uc_i].m_Index_InAuxUnit == AuxIdxIn_AuxUnit)
            return uc_i;
    }

    return 0xFF; //如果没找到的话，返回一个0xFF
}

bool Aux_DataManager::Aux_GetData(unsigned char uc_IV_unitID, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxCh, ST_AUX_DATA * result)
{

    unsigned char uc_data_group_idx;
    unsigned char uc_chn;
    unsigned char uc_i;
    // 	  unsigned char uc_j;

    unsigned char uc_IVunitBufIndex;
    ST_AUX_DATA  result_1;
    ST_AUX_DIDO_DATA *result_2;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(uc_IV_unitID);

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
        return 0;

    for (uc_i = 0; uc_i < m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_ActualCounts; uc_i++)
    {
        if (m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_Index_InAuxUnit == uc_AuxCh)
        {
            uc_data_group_idx = m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_data_group_idx;
            uc_chn = m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_chn;

            *result = m_AuxDataBank[uc_data_group_idx][uc_chn];
            return 1;
        }
    }

    return 0;
}


bool Aux_DataManager::TransAuxChnFromSequence2Physical(unsigned char uc_IV_PhysicalID, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxSequenceChan, unsigned char *uc_BoradIdx, unsigned char *uc_AuxPhysicalchn) // 20170228 zyx
{
    unsigned char uc_i;
    unsigned char uc_IVunitBufIndex;

    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(uc_IV_PhysicalID);

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
        return 0;

    for (uc_i = 0; uc_i < m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; uc_i++)
    {
        if (m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_Index_InAuxUnit == uc_AuxSequenceChan)
        {
            *uc_BoradIdx = m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_data_group_idx;
            *uc_AuxPhysicalchn = m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[uc_i].m_chn;

            return 1;
        }
    }

    return 0;
}


// unsigned char Aux_DataManager::Aux_GetLimitCount(unsigned char uc_IVch)
// {
//     //这里有疑惑，每个IV都有两种LIMIT：SingleLimit/AnyLimit，这里是要返回哪个类型的数目呢？
//     // 原来考虑分成两个函数，一个是 limit 一个是Ifany limit
//     //return m_AuxLimitCount[uc_IVch];
// }

// unsigned char Aux_DataManager::Aux_GetIfAnyLimitCount(unsigned char uc_IVch)
// {
//     //这里有疑惑，每个IV都有两种LIMIT：SingleLimit/AnyLimit，这里是要返回哪个类型的数目呢？
//     // 原来考虑分成两个函数，一个是 limit 一个是Ifany limit
//     //return m_AuxIfAnyLimitCount[uc_IVch];
// }

unsigned char  Aux_DataManager::Aux_AddLimit(ST_AUX_LIMIT st_limit)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex =Aux_LocateIVUnitBufferIndex(st_limit.m_Source_CANID);

    unsigned char uc_EOB = m_AuxLimitCount[uc_IVunitBufIndex][st_limit.m_IV_index];  //获取该limit的IV通道的当前limit数
    if(uc_EOB > MAX_LIMIT_PER_IVCH)
        return LIMIT_BANK_FULL;
// 	else
// 	{
    for(unsigned char Limit_Num = 0; Limit_Num<uc_EOB; Limit_Num++)
    {
        if(st_limit.m_limit_SN == m_AuxLimitBank [uc_IVunitBufIndex][st_limit.m_IV_index][Limit_Num].m_limit_SN)
            return NO_ERROR;   //为重发limit
    }
    m_AuxLimitBank [uc_IVunitBufIndex][st_limit.m_IV_index][uc_EOB] = st_limit;
    m_AuxLimitCount[uc_IVunitBufIndex][st_limit.m_IV_index] ++;
    m_Count_Limit_AllChannel[uc_IVunitBufIndex] ++;
    return NO_ERROR;
// 	}
}




unsigned char Aux_DataManager::Aux_AddIfAnyLimit(ST_AUX_IFANY_LIMIT st_ifanylimit)

{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(st_ifanylimit.m_Source_CANID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;

    unsigned char uc_EOB = m_AuxIfAnyLimitCount[uc_IVunitBufIndex ][st_ifanylimit.m_IV_index];  //获取该limit的IV通道的当前Ifany limit数
    if(uc_EOB > MAX_IFANY_LIMIT_PER_IVCH)
    {
        return IFANY_LIMIT_BANK_FULL;
    }

    for(unsigned char Limit_Num = 0; Limit_Num<uc_EOB; Limit_Num++)
    {
        if(st_ifanylimit.m_limit_SN == m_AuxIfAnyLimitBank[uc_IVunitBufIndex][st_ifanylimit.m_IV_index][Limit_Num].m_limit_SN)
            return NO_ERROR;
    }
    m_AuxIfAnyLimitBank [uc_IVunitBufIndex ] [st_ifanylimit.m_IV_index][uc_EOB] = st_ifanylimit;
    m_AuxIfAnyLimitCount [uc_IVunitBufIndex ][st_ifanylimit.m_IV_index] ++;
    m_Count_AnyLimit_AllChannel [uc_IVunitBufIndex ] ++;
    return NO_ERROR;

}

//下面的函数用于清除某个IV通道的所有limit，包括一般的limit和IFany型limit
//目前看不出有必要做一个清所有通道的limit的需要，因为IVMCU 对每个IV通道是单独管理的。
unsigned char  Aux_DataManager::Aux_ClearLimits( unsigned char uc_IV_unitID,unsigned char uc_IVch)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(uc_IV_unitID);
    if(uc_IVunitBufIndex==0xff)
        return 0;

    if(uc_IVch >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;
    else
    {
        m_Count_Limit_AllChannel[uc_IVunitBufIndex]    -= m_AuxLimitCount[uc_IVunitBufIndex][uc_IVch];
        m_Count_AnyLimit_AllChannel[uc_IVunitBufIndex] -= m_AuxIfAnyLimitCount[uc_IVunitBufIndex][uc_IVch];

        if(m_Count_Limit_AllChannel[uc_IVunitBufIndex] < 0)
            m_Count_Limit_AllChannel[uc_IVunitBufIndex] = 0;
        if(m_Count_AnyLimit_AllChannel [uc_IVunitBufIndex]< 0)
            m_Count_AnyLimit_AllChannel[uc_IVunitBufIndex] = 0;

        m_AuxLimitCount[uc_IVunitBufIndex][uc_IVch] = 0;
        m_AuxIfAnyLimitCount[uc_IVunitBufIndex][uc_IVch] = 0;
        // SDL 部分需要针对多IVMCU的情况处理    DKQ
        // 清除SDL模块标志，停止运行SDL任务
        for(unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)
        {
            unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(uc_AuxType);
            if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
                continue;
            // 			if(Aux_DataManager::m_Iv2Aux_Map[uc_IVch][uc_AuxType].m_chn_count == 0)
            if(Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap == 0)
                continue;  // break;   // 因为上位机下时类型板已经按填定情况，2Vd，T,...如果2VD没有也会空着。

            //AuxSDL::SDLEndedRoutine(uc_IVch, uc_AuxType, 0);	// 清除SDL工作标志，停止运行
            AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_AuxType, 0, LOG_PC_DATA, LOGSWITCH_OFF);

            // 清除 Aux向IV报数标志
            //	for(unsigned char AuxchIdx = 0;AuxchIdx < MAX_AUXCH_PER_BOARD;AuxchIdx++)
            for(unsigned char AuxchIdx = 0; AuxchIdx < MAX_CH_IN_AUX_UNIT; AuxchIdx++)
            {
                AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_AuxType, AuxchIdx, REPORT_IV_DATA, LOGSWITCH_OFF);
                AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_AuxType, AuxchIdx, LOG_PC_DATA, LOGSWITCH_OFF);
            }
        }
        return NO_ERROR;
    }
}



unsigned char Aux_DataManager:: Aux_AddSafety (ST_AUX_HALF_SAFETY st_half_safety)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(st_half_safety.m_Source_CANID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;
    if(st_half_safety.m_aux_type >=MAX_AUX_TYPE)
        return AUX_TYPE_OVERFLOW;
    if(st_half_safety.m_IV_index >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(st_half_safety.m_aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
        return AUX_TYPE_OVERFLOW;

    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_Source_CANID = st_half_safety.m_Source_CANID;
    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_IV_index = st_half_safety.m_IV_index;
    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_aux_type = st_half_safety.m_aux_type;
    //  下面代码被注释掉，因为以前是连续MAP的，知道了起始和结束通道号就可以便于检查所有的辅助通道。
    //   现在得逐个扫描了。
    if((m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] == 0 )&&
            (m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] == 0) ) //alive
    {
        // 				m_AuxSafetyBank[st_half_safety.m_IV_index][st_half_safety.m_aux_type].m_Start_Chn_InUnit =
        // 				m_MapFromPC[st_half_safety.m_IV_index][st_half_safety.m_aux_type].m_Start_InUnit;
        //
        // 				m_AuxSafetyBank[st_half_safety.m_IV_index][st_half_safety.m_aux_type].m_End_Chn_InUnit =
        // 				m_MapFromPC[st_half_safety.m_IV_index][st_half_safety.m_aux_type].m_End_InUnit;

    }
    else
    {
        Aux_Safety_Check::CleanSafetyCheckResults(uc_IVunitBufIndex,st_half_safety.m_IV_index); //已经设置了safety，清掉以前的检查结果  DKQ 12.17.2013
    }

    switch(st_half_safety.m_High_or_Low)
    {
    case AUX_SAFETY_LOW_END:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_LOW_END] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] = 1;  //alive

        //m_AuxSafetyBank[st_safety.m_IV_index][st_safety.m_aux_type][AUX_SAFETY_LOW_END] = st_safety;
        m_AuxSafetyCount[uc_IVunitBufIndex][st_half_safety.m_IV_index] ++;
        break;
    case AUX_SAFETY_HIGH_END:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_HIGH_END] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] = 1;  //alive
        break;
    case AUX_SAFETY_Voltage_LOW:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_LOW] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_LOW] = 1;  //alive
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] = 1;  //alive
        break;
    case AUX_SAFETY_Voltage_HIGH:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_HIGH] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_HIGH] = 1;  //alive
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] = 1;  //alive
        break;
    case AUX_SAFETY_Current_LOW:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_LOW] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_LOW] = 1;  //alive
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] = 1;  //alive
        break;
    case AUX_SAFETY_Current_HIGH:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_HIGH] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_HIGH] = 1;  //alive
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] = 1;  //alive
        break;
    case AUX_SAFETY_Temperature_LOW:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_LOW] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_LOW] = 1;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] = 1;  //alive		//alive
        break;
    case AUX_SAFETY_Temperature_HIGH:
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_HIGH] = st_half_safety.m_safe_value;
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_HIGH] = 1;  //alive
        m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] = 1;  //alive
        break;
    default:
        break;
    }
    return NO_ERROR;

}

unsigned char  Aux_DataManager:: Aux_ClearSafeties (unsigned char uc_IV_unitID, unsigned char uc_IVch)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(uc_IV_unitID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;
    if (uc_IVch >=MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;
    else
    {
        m_AuxSafetyCount[uc_IVunitBufIndex ][uc_IVch] = 0;			 //将该IV通道下的safety数清0 即可
        for(unsigned char i =0; i< MAX_AUX_TYPE; i++)
        {
            unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(i);
            if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
                continue;

            m_AuxSafetyBank[uc_IVunitBufIndex ][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END] = 0;
            m_AuxSafetyBank[uc_IVunitBufIndex ][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END] = 0;
        }


        return NO_ERROR;
    }
}

unsigned char Aux_DataManager:: Aux_AddSDL (ST_AUX_SDL st_sdl)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(st_sdl.m_Source_CANID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;

    if(st_sdl.m_aux_type >=MAX_AUX_TYPE)
        return AUX_TYPE_OVERFLOW;
    if(st_sdl.m_IV_index >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(st_sdl.m_aux_type);
    if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
        return AUX_TYPE_OVERFLOW;

    m_AuxSDLBank[uc_IVunitBufIndex][st_sdl.m_IV_index][TempAuxTypeIdex] = st_sdl;
    return NO_ERROR;
}

//下面的函数用来建立IV数据请求记录。AUXMCU的数据上报任务根据请求记录通过CAN向IV上报数据
//为避免对同一个辅助通道的同一类型的数据重复请求，IVMCU在请求时应做个标志。若请求过，则不应再请求

unsigned char Aux_DataManager:: Aux_AddDataRequest (ST_AUX_DATA_REQUEST st_data_request)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(st_data_request.m_Source_CANID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;

    if(st_data_request.m_aux_type >=MAX_AUX_TYPE)
        return AUX_TYPE_OVERFLOW;
    if(st_data_request.m_IV_index >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;
    if(st_data_request.m_aux_chn_index >=MAX_AUXCH_PER_AUXMCU)
        return AUX_CHNO_OVERFLOW;
    if(st_data_request.m_data_type >2)
        return AUX_DATA_TYPE_OVERFLOW;

    unsigned char uc_EOB = m_AuxDataRequestCount[uc_IVunitBufIndex][st_data_request.m_IV_index];  //获取该IV通道的当前数据请求数数
    if(uc_EOB>=(3*MAX_AUXCH_PER_AUXMCU))
        return AUX_DATA_REQUEST_BANK_FULL;
    {
        m_AuxDataRequestBank [uc_IVunitBufIndex][st_data_request.m_IV_index][uc_EOB] = st_data_request;
        m_AuxDataRequestCount[uc_IVunitBufIndex][st_data_request.m_IV_index] ++;
        AuxSDL::RequireData(st_data_request);  //这个函数在多IVMCU的情况下是否需要修改？
        return NO_ERROR;
    }

}

unsigned char  Aux_DataManager::Aux_ClearDataRequests(unsigned char uc_IV_unitID, unsigned char uc_IVch)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex = Aux_LocateIVUnitBufferIndex(uc_IV_unitID);
    if(uc_IVunitBufIndex==0xff)
        return UNIT_ID_NOT_FIND;

    if(uc_IVch >= MAX_IVCH_PER_IVMCU)
        return IV_CHNO_OVERFLOW;
    else
    {
        m_AuxDataRequestCount[uc_IVunitBufIndex][uc_IVch] = 0;
        for(unsigned char uc_i=0; uc_i < MAX_AUX_TYPE; uc_i ++)
        {
            unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(uc_i);
            if(TempAuxTypeIdex == 0xFF)//辅助限制  如果没有该辅助类型则不用找了。
                continue;
            if(m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap!=0)
            {
                for(unsigned char uc_j =0  ; uc_j <= m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap ; uc_j ++)
                {
                    AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_i, uc_j, REPORT_IV_DATA, LOGSWITCH_OFF);
                }
            }
        }
    }  //暂时屏蔽  qjm 20150603

    return NO_ERROR;
}



//需要根据IV 下发的信息查找到对应的辅助通道数据，然后上发PC
void Aux_DataManager::Aux_TrigLogPoint( ST_AUX_DATA_TRIG * TrigInfo, St_SDL_Point_Pack * MyPoint)
{
    //  ST_AUX_DATA st_Aux_Data;
    Aux_GetData( TrigInfo->m_IV_unitID,TrigInfo->m_IVChnIdx,TrigInfo->m_AuxType,TrigInfo->m_AuxChnIdx,&st_Aux_Data);

// 	if(st_Aux_Data.value_update_time.Second < MyPoint->u32Second)     //160429   //180813 hpq
// 		return;
// 	if(  (st_Aux_Data.value_update_time.Second == MyPoint->u32Second)
// 		&& (st_Aux_Data.value_update_time.Us100 <= MyPoint->u16Us100) ) //160429
// 	  return;

    MyPoint->u32Second = TrigInfo->TrigTime.Second;
    MyPoint->u16Us100 =TrigInfo->TrigTime.Us100;
    //MyPoint->ucValueType = MetaCode_PV_Aux_Value;
    MyPoint->fValue = st_Aux_Data.aux_data;
    MyPoint->bIV = 0;
    MyPoint->ucValueType =  TrigInfo->m_AuxType;
    //MyPoint->ucIVChIdxTriggerer = TrigInfo->m_IVChnIdx; //WL add 12.02.2019 for CSV project
    //   if( TrigInfo->m_AuxType == AUX_TEMPERATURE)
    //    ;// MyPoint->ucSensor =  Aux_DataManager::m_AllAuxBoards[TrigInfo->m_AuxType].m_AuxSensor_ChInfo[TrigInfo->m_AuxChnIdx].m_AuxTemp.m_AuxThermoCouple.SensorType;//暂时屏蔽 齐静明20150603
    //   else
    //     MyPoint->ucSensor = TrigInfo->m_AuxType;
    MyPoint->ucChannelIdx = TrigInfo->m_AuxChnIdx;

    st_Aux_Data.m_Chn_Info.m_AuxType = TrigInfo->m_AuxType;

    EthernetParse::MPReportData(TrigInfo->m_IVChnIdx,SEND_AUX_LOGDATA);  // PC 以太网络发送
}

void Aux_DataManager::Aux_TrigLogGroupPoint(ST_AUX_DATA_TRIG* TrigInfo, St_SDL_OnePoint *MyPoint)
{
    Aux_GetData( TrigInfo->m_IV_unitID,TrigInfo->m_IVChnIdx,TrigInfo->m_AuxType,TrigInfo->m_AuxChnIdx,&st_Aux_Data);
    /*MyPoint->u32Second = TrigInfo->TrigTime.Second;
    MyPoint->u16Us100 = TrigInfo->TrigTime.Us100;
    MyPoint->bIV = 0;
    MyPoint->*/
    st_Aux_Data.m_Chn_Info.m_AuxType = TrigInfo->m_AuxType;
    MyPoint->fValue = st_Aux_Data.aux_data;
    MyPoint->ucValueType = TrigInfo->m_AuxType;
    MyPoint->ucChanneldx = TrigInfo->m_AuxChnIdx;
}

void Aux_DataManager::Aux_TrigLogGroupPoint_dxdt(ST_AUX_DATA_TRIG* TrigInfo, St_SDL_OnePoint *MyPoint)
{
    MyPoint->fValue = st_Aux_Data.aux_dx_dt;
    switch(TrigInfo->m_AuxType) //(Aux_DataManager::st_Aux_Data.m_Chn_Info.m_AuxType)
    {
    case AUX_VOLTAGE:
        MyPoint->ucValueType = AUX_VOLTAGE_DVDT_DATA;
        break;
    case AUX_TEMPERATURE:
        MyPoint->ucValueType = AUX_TEMPERATURE_DTDT_DATA;
        break;
    case AUX_PRESSURE:
        MyPoint->ucValueType = AUX_PRESSURE_DPDT_DATA;
        break;
    default:
        break;
    }
    MyPoint->ucChanneldx = TrigInfo->m_AuxChnIdx;
}


void Aux_DataManager::Init_BoardCFG(void)
{
    AuxCConfig::LoadSysConfig();
    // 	Aux_ScanBoardInfo();  // 通道序号重新排序
    // 	Aux_GetMapfromPC( &uc_Result);    // 转换PCMap表以便管理
    //     Aux_BuildIv2AuxMap();  // 转换IVMap表以便管理
    // 	Aux_ObtainGainOffset(); //获取每个通道的Gain， offset，及 extra信息。 Add by DKQ 03.26.2014

}
// 下面的函数用于获取各个板上的各个通道的Gain，Offset， 以及 extra信息。放在读板信息后调用。
void Aux_DataManager::Aux_ObtainGainOffset(void)
{
    unsigned char uc_i, uc_j;
    for (uc_i = 0; uc_i < MAX_BOARD_PER_AUXMCU; uc_i ++)
    {
        if( Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_valid==BEUSED)
        {
            unsigned char data_group_idx=Aux_DataManager::m_AllAuxBoards[uc_i].m_data_group_start;
            for(uc_j = 0; uc_j <Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_Max_Channels; uc_j++)
            {
                Aux_DataManager::m_AuxDataBank[data_group_idx+uc_j/16][uc_j].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain=Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uc_j].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
                Aux_DataManager::m_AuxDataBank[data_group_idx+uc_j/16][uc_j].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset=Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_chn[uc_j].m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
            }
        }
    }
}

void Aux_DataManager::SaveChannelConfigPara(unsigned short ChannelCount,unsigned char * pChannelIdx)
{
//     UChar8 uc_DeviceAddr = 0XFF;
//     unsigned char uc_BChID = 0;
//     UWord16       uw16_PhysicalCh = 0;
//     MP_AuxUnitConfigDownload * st_Config;
//     st_Config = (MP_AuxUnitConfigDownload *) &EthernetParse::m_btPackDataBuffer[0];

    /*for(unsigned short ChannelIndex = 0; ChannelIndex < MAX_AUXCH_PER_AUXMCU;ChannelIndex++)
    {
    if(MYBITTRUE(pChannelIdx,ChannelIndex))
    {
    unsigned char uc_i = 0;
    unsigned char AuxCh_Count = 0;
    unsigned char uc_Board_FirstCH = 0;
    unsigned char uc_BChID_back = 0;
    for( uc_i=0; uc_i < MAX_BOARD_PER_AUXMCU; uc_i ++)
    {
    //if(m_AllAuxBoards[uc_i].m_BoardInfo.m_BType.m_AuxType == st_Config->m_Channel[ChannelIndex].m_ChannelType)
    //{
    if((st_Config->m_Channel[ChannelIndex].m_ChannelNumber >= m_AllAuxBoards[uc_i].m_Start_InUnit)
    && (st_Config->m_Channel[ChannelIndex].m_ChannelNumber <= m_AllAuxBoards[uc_i].m_End_InUnit))
    {
    if(uc_DeviceAddr != uc_i)
    {
    uc_Board_FirstCH = 1;
    uc_BChID = 0;
    }
    else
    {
    uc_BChID ++;
    }
    uc_DeviceAddr = uc_i;    //  板号处理
    break;
    }
    AuxCh_Count += m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_Total_Channels;  // 计算总通道数
    }

    if(uc_i >= MAX_BOARD_PER_AUXMCU)  // 搜索不到相同的通道类型放弃存储。
    continue;

    Aux_DataManager::m_AllAuxBoards[uc_DeviceAddr].m_BoardInfo.m_abi_chn[uc_BChID].m_AuxSensorCoreInfo = st_Config->m_Channel[ChannelIndex].m_AuxSensorCoreInfo; // 更新通道信息
    uw16_PhysicalCh = Aux_DataManager::Convert_PhysicalChID(uc_DeviceAddr, uc_BChID); // 同一块板中面板对应通道号处理
    //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
    //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
    AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_DeviceAddr), uw16_PhysicalCh, &st_Config->m_Channel[ChannelIndex]);   // 通道信息存入I2C的24LC128

    if(uc_Board_FirstCH)    // 写同一块辅助板的第一个通道信息时写入RSN号
    {
    uc_Board_FirstCH = 0;
    AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // 注册序号RSN
    AuxCConfig::WriteAUXBoardRSN(MAX_BOARD_PER_AUXMCU - 1 - uc_DeviceAddr ,&AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_I2C);
    AuxConfigRSN::Check_RSN_Init( uc_DeviceAddr );

    AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_FRAM = 0;
    AuxCConfig::WriteRSN(uc_DeviceAddr, &AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_FRAM); // 特殊写板时才会全清一次FRAM中的RSN

    AuxConfigRSN::Check_RSN_Init( uc_DeviceAddr );
    }
    }
    }	*/
}




float Aux_DataManager::T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    //温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
    //温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[K型温度范围:-270(°C)到1372(°C)]
    if( f_Value < -6458 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >54886)
        return ( 1372 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9

    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    {
        CoE = &f_CoEA_Ktype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20644)        //温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
    {
        CoE = &f_CoEB_Ktype[0];
        for(uc_i=1; uc_i<10; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=54886)       //温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
    {
        CoE = &f_CoEC_Ktype[0];
        f_TempE =(*CoE);
        for(uc_i=1; uc_i<7; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float  Aux_DataManager::T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[T型温度范围:-270(°C)到400(°C)]
    if( f_Value < -6258 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >20872)
        return ( 400 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    {
        CoE = &f_CoEA_Ttype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    {
        CoE = &f_CoEB_Ttype[0];
        for(uc_i=1; uc_i<7; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float Aux_DataManager::T_t90_Convert_JType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-210(°C)到0(°C) ，微伏电压范围在-8095(μV)到0(μV)
    //温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
    //温度范围在760(°C)到1200(°C) ，微伏电压范围在42929(μV)到69553(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[J型温度范围:-270(°C)到1200(°C)]
    if( f_Value < -8095 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >69553)
        return ( 1200 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9
    if(f_Value<0)       //温度范围在-210(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    {
        CoE = &f_CoEA_Jtype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=42929)        //温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
    {
        CoE = &f_CoEB_Jtype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=69553)       //温度范围在500(°C)到1372(°C) ，微伏电压范围在42929(μV)到69553(μV)
    {

        CoE = &f_CoEC_Jtype[0];
        f_TempE = (*CoE);
        for(uc_i=1; uc_i<6; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float Aux_DataManager::T_t90_Convert_EType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-8825(μV)到0(μV)
    //温度范围在0(°C)到1000(°C) ，微伏电压范围在0(μV)到76373(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[E型温度范围:-270(°C)到1000(°C)]
    if( f_Value < -8825 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >76373 )
        return ( 1000 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    {
        CoE = &f_CoEA_Etype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    {
        CoE = &f_CoEB_Etype[0];
        for(uc_i=1; uc_i<10; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }

    return ( f_TempE );
}

unsigned char Aux_DataManager::FindNextAuxADCid(void)   //返回的是一个片选地址，应该是一个无符号数，这应养成习惯。对于一个不应该出现负数的量，我们就应该定义成无符号数。

{
    unsigned char ADC_CS_idx;
    unsigned short CheckResult;
    unsigned char temp;
    ADC_CS_idx = OS_ServeProcess::AUX_ADC_Idx;
    CheckResult = CheckAllAuxADCStatus();
    if(Aux_ADC_CS_Info[ADC_CS_idx].m_StateMachine == AUX_Sample_READ)  //防止还没有对上次的数据读完就切换到别的片选地址去了
        return ADC_CS_idx;
    if(Aux_ADC_TotalCnt==1)
        OS_ServeProcess::AUX_ADC_GOTO_Idx=ADC_CS_idx;
    else
    {
        OS_ServeProcess::AUX_ADC_GOTO_Idx= findNextADC_CS(ADC_CS_idx);

    }
    //     OS_ServeProcess::AUX_ADC_GOTO_Idx=ADC_CS_idx+1;   //如果选择在这个函数里进行片选地址递增的话，则针对任何一种情况，都应该是这样的


    if (CheckResult!=0)  //如果至少有一个ADC 转换完毕
    {
        if (!(CheckResult&(1<<ADC_CS_idx))) //（ 不是当前CS_Idx的那个ADC转换完毕）
        {
            for(unsigned char i=1; i<16; i++)
            {
                if(i<=ADC_CS_idx)
                    temp = ADC_CS_idx-i;  //找最远的那个ADC
                else
                    temp = 16+ADC_CS_idx-i;  //翻转
                if (CheckResult&(1<<(temp)))
                {
                    ADC_CS_idx = temp;   //找到了最远的那个ADC，实现跳变
                    break;
                }

            }
        }

    }
    else
    {
        if(Aux_ADC_CS_Info[ADC_CS_idx].m_StateMachine ==AUX_Sample_WAIT)
        {
            if(Aux_ADC_TotalCnt==1)
            {
                return ADC_CS_idx;
            }
            ADC_CS_idx=findNextADC_CS(ADC_CS_idx);
        }
    }
    return ADC_CS_idx;
}

unsigned short Aux_DataManager::CheckAllAuxADCStatus(void)   //这里必须用unsigned short，如果用short的话，只能正确标记15个ADC
{
    //for(unsigned char idx=0;idx<Aux_DataManager::Aux_ADC_TotalCnt;idx++)
    for(unsigned char idx=0; idx<16; idx++)
    {
        if(Aux_ADC_CS_Info[idx].b_Triggered)
            Aux_ADC_CS_Info[idx].counter++;  //每个ADC Trigger后的通道，刷新计时器
        if(Aux_ADC_CS_Info[idx].counter>=Aux_ADC_CS_Info[idx]. m_Wait_Count)
        {
            if(Aux_ADC_CS_Info[idx].m_StateMachine ==AUX_Sample_WAIT)    //状态机的更新在这里统一处理比较好
            {
                SettingCS_High();
                Aux_ADC_CS_Info[idx].m_StateMachine= AUX_Sample_TRIGGER;
                Aux_ADC_CS_Info[idx].b_Finished=1;
                Aux_DataManager::Aux_ADC_CS_Info[idx].b_Triggered=0;
                Aux_ADC_TimeOut_Flag |= (1<<idx);
                Aux_ADC_CS_Info[idx].m_WorkingChn=findNextChn(idx);
            }
        }
    }
    return Aux_ADC_TimeOut_Flag;
}
char  Aux_DataManager::SettingCS_Low(unsigned char m_CS_Address) //这部分代码要重新考虑，不同的板片选实现的方式不完全一样
{
    switch(m_CS_Address)
    {
    case 0:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 1:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 2:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 3:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 4:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 5:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 6:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 7:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_EOE);
        break;
    case 8:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 9:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 10:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 11:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 12:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 13:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 14:
        ControlPin::SetMeLow(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    case 15:
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_ECODE3);
        ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
        break;
    default:
        break;
    }
    return 1;

}

char  Aux_DataManager::SettingCS_High(void) //这部分代码要重新考虑，不同的板片选实现的方式不完全一样

{
    ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
}

unsigned char  Aux_DataManager::findNextChn(unsigned char ADCID)
{
    unsigned char chn;
    chn = Aux_ADC_CS_Info[ADCID].m_WorkingChn;
    for(unsigned char i=chn+1; i<Aux_ADC_CS_Info[ADCID].m_ADCCH_MaxCnt; i++)
    {
        if(!Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i])                     //通道不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    // 翻转了
    for (unsigned char i=0; i<chn; i++)
    {
        if(!Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i])                     //通道不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
}

unsigned char  Aux_DataManager::findNextADC_CS(unsigned char ADCidx)
{
    unsigned char temp=ADCidx;
    for(unsigned i = temp+1; i<16; i++)
    {
        if(!Aux_ADC_CS_Info[i].m_Valid)                     //ADC不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    for(unsigned i = 0; i<temp; i++)
    {
        if(!Aux_ADC_CS_Info[i].m_Valid)                     //ADC不存在
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    // 		 return 0;
}
unsigned char  Aux_DataManager::findLastChn(unsigned char ADCID)
{
    unsigned char temp=Aux_DataManager::Aux_ADC_CS_Info[ADCID].m_WorkingChn;
    for(unsigned char i=temp; i>0; i--)
    {
        if(!Aux_DataManager::Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i-1])                     //通道不存在
        {
            continue;
        }
        else
        {
            return (i-1);
        }

    }
    for(unsigned char i=Aux_ADC_CS_Info[ADCID].m_ADCCH_MaxCnt; i>temp; i--)
    {
        if(!Aux_DataManager::Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i-1])                     //通道不存在
        {
            continue;
        }
        else
        {
            return (i-1);
        }
    }
    //    return 0;


}

unsigned long Aux_DataManager::CalibrateVoltageDAC(unsigned char uc_datagroupidx, unsigned char uc_ChannelNo,float f_Voltage)
{
    float f_gain,f_offset,phy_gain,phy_offset;
    unsigned long u32_Voltage;
    f_gain = m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    f_offset = m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    phy_gain =  Aux_DataManager::m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = Aux_DataManager::m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    f_Voltage = f_Voltage*phy_gain+phy_offset;
    u32_Voltage = f_Voltage*f_gain + f_offset;

    if(u32_Voltage > CONVERTER_MAX)
        u32_Voltage = CONVERTER_MAX;

    return u32_Voltage;
}

unsigned long Aux_DataManager::CalibrateDAC_for_DDS(unsigned char uc_datagroupidx, unsigned char uc_ChannelNo,float f_RSM)  //Add by DKQ to seperate from Voltage DAC
{
    float f_gain,f_offset,phy_gain,phy_offset, f_Voltage;
    unsigned long u32_Voltage;
    f_gain = m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    f_offset = m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    phy_gain =  Aux_DataManager::m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = Aux_DataManager::m_AuxDataBank[uc_datagroupidx][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    f_Voltage = f_RSM*phy_gain+phy_offset;
	if (f_Voltage >10)   //DDS control signal is designed to between 0 to 10V
		f_Voltage =10;
	if (f_Voltage <0)
		f_Voltage =0;
    u32_Voltage = f_Voltage*f_gain + f_offset;

    if(u32_Voltage > CONVERTER_MAX)
        u32_Voltage = CONVERTER_MAX;

    return u32_Voltage;
}
float Aux_DataManager::UnifyADCVoltage(unsigned char uc_ChannelNo, float fCurrentValue)    //04.23.2013
{
    float f_Temp = 0.0f;

    float f_Bound;

    f_Bound = m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_ChannelNo].m_AI_DataBank][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    if(f_Bound!=0)
        f_Temp = fCurrentValue / f_Bound;

    return f_Temp;
}
float Aux_DataManager::UnifyVoltage( unsigned char uc_ChannelNo,  float fVoltageValue)  //04.23.2013
{
    float   f_Temp = 0.0f;
    float f_Bound;

    f_Bound = m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_ChannelNo].m_DataBank][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    if(f_Bound!=0)
        f_Temp = fVoltageValue / f_Bound;

    return f_Temp;
}
float Aux_DataManager::DeUnifyADCVoltage(unsigned char uc_ChannelNo, float fUniCurrentValue)  //04.23.2013
{

    float f_Temp = 0.0f;

    float f_Bound;

    f_Bound =  m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_ChannelNo].m_AI_DataBank][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;

    f_Temp = fUniCurrentValue * f_Bound;

    return f_Temp;
}
float Aux_DataManager::DeUnifyVoltage( unsigned char uc_ChannelNo, float fUniVoltageValue)  //04.23.2013
{
    float  f_Temp = 0.0f;
    float f_Bound;

    f_Bound = m_AuxDataBank[AuxPID_task::mAuxPID_par[uc_ChannelNo].m_DataBank][uc_ChannelNo].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    f_Temp = fUniVoltageValue * f_Bound;

    return f_Temp;
}

unsigned char Aux_DataManager::Aux_LocateAuxTypeArrayIndex(unsigned char uc_AuxType)//辅助限制
{
    unsigned char uc_i;
    for (uc_i = 0; uc_i < MAX_AUX_TYPE_PER_MCU ; uc_i++)
    {
        if( AuxTypeIndexTable[uc_i] != -1)  //
        {
            if(AuxTypeIndexTable[uc_i] == uc_AuxType)
                return uc_i;
        }
    }
    return 0xFF; //
}
signed char Aux_DataManager::Aux_LocateAuxTypeInArray(unsigned char uc_i)//辅助限制
{
    if(uc_i < MAX_AUX_TYPE_PER_MCU)
    {
        return AuxTypeIndexTable[uc_i];
    }
    return -1;
}
void Aux_DataManager::Aux_ConfirmContainAuxTypes(void)//辅助限制
{
    unsigned char uc_i = 0;
    for (unsigned char j=0; j< MAX_AUX_TYPE; j++)   //扫描所有的辅助类型
    {
        if(m_AuxChnLookupTable[j].m_TotalChnNum >0)
        {
            AuxTypeIndexTable[uc_i] = j;
            uc_i++;
            if(uc_i >= MAX_AUX_TYPE_PER_MCU)
                return;
        }
    }
}
