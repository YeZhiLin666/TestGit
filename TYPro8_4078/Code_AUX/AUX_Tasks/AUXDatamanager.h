#pragma once

#include "../Aux_Tasks/AuxDataDefine.h"



#if( _USER_DEFINE_BOARD_ENABLE_ == 1&& _USER_BOARD_TYPE_ ==  AUX_TEMPLM75_FOR_ZHONGDIAN)
#define I2C_TEMP_MAXCHN    4
typedef enum
{
    en_Safe = 0,
    en_Unsafe = 1,
} en_SafetyStaus;


#endif


class Aux_DataManager
{
public:

    static st_Aux_Chn_Info st_Aux_info[MAX_BOARD_PER_AUXMCU][17];	 //类似于channle info  //临时测试  //  电池包还需要用这个

    static st_Aux_Sample st_aux_sample_setting[MAX_BOARD_PER_AUXMCU][17]; //电池包需要用

    static ST_AUX_ADC_CS     Aux_ADC_CS_Info[16];  //Add by DKQ 05.04.2015   //记录每个片选信号相关的信息
    static unsigned short    Aux_ADC_TimeOut_Flag; //Add by QJM	 05.05.2015  //记录每个CS对应的ADC timeout
    static unsigned char     Aux_ADC_TotalCnt;
    //		static ST_AUXSAMPLE 				m_AuxSample;
    static ST_AUX_BOARDCFG				m_Aux_BoardCFG[MAX_BOARD_PER_AUXMCU];   //存储用于分层处理的板信息

    static ST_MAP_FROM_PC						m_MapFromPC[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];  //辅助unit用的MAP表
    static MsTime                   m_TriggerLogTime[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];  //160429
    //static St_DOWNLOAD_MAP_FROM_PC  m_DownLoadMapFromPC[MAX_AUXCH_PER_AUXMCU]; //下载时用的辅助MAP结构，最后应转换成大表用
    static St_DOWNLOAD_MAP_FROM_PC  m_DownLoadMapFromPC[MAX_MAP_COUNT_PER_UNIT]; //下载时用的辅助MAP结构，最后应转换成大表用

    static ST_IVMCUID_CFG       		m_IVMCU_ID_Info[MAX_IVMCU_PER_SYS];    //IV MCU ID 有效信息
    static ST_AUX_BOARD_INFO  	        m_AllAuxBoards[MAX_BOARD_PER_AUXMCU];  //用于存储扫描EEPROM获取的信息
    static ST_AUX_BOARD_INFO_IN_EEPROM  m_AuxBoardInfo4EEPROM;

    static ST_AUX_IN_UNIT_LOOKUP_TABLE  m_AuxChnLookupTable [MAX_AUX_TYPE]; //记录每种辅助在长蛇阵的位置以便快速查找
    static ST_AUX_CHN_LOCATION          m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU];    //“长蛇阵”，用于记录该unit内每个辅助通道的数据在数据bank中的位置

    static ST_AUX_DATA  		      m_AuxDataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//用于按Data_Group存放辅助通道数据.
    //	static ST_AUX_DIDO_DATA  		  m_Aux_DIDO_DataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//用于按Data_Group存放辅助通道数据.
    static unsigned char         m_DGIdx2BIdx[MAX_DATA_GROUP]; //
    static ST_AUX_P2E_IN_UNIT         m_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];// (PannelChannel number to Electic Channel Number)
    //	static unsigned char         m_DIDO_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];// (PannelChannel number to Electic Channel Number)

    static bool                  m_DataGroupMgr[MAX_DATA_GROUP]; //用于标记每个data group 是否已生效
    //	static bool                  m_DIDO_DataGroupMgr[MAX_DATA_GROUP]; //用于标记每个data group 是否已生效

    static ST_INCHIP_TEMP         m_InChipTemperature[MAX_DATA_GROUP];  //用于存放ADC片内温度供热电偶冷端补偿

    static ST_AUX_LIMIT 				m_AuxLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_LIMIT_PER_IVCH];
    static unsigned char 				m_AuxLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

    static ST_AUX_IFANY_LIMIT 	m_AuxIfAnyLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_IFANY_LIMIT_PER_IVCH];
    static unsigned char 				m_AuxIfAnyLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

    static ST_AUX_SAFETY				m_AuxSafetyBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];     //add by dirui 20131206   每个类型应该可以存储至少High/Low 两种数据
    static unsigned char 				m_AuxSafetyCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //每个IV通道下的辅助通道safety设置个数可能不一样


    static ST_AUX_SDL						 m_AuxSDLBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU]; //SDL参数按辅助类型分，但IV通道之间可以有所不同
    static ST_AUX_DATA_REQUEST 	 m_AuxDataRequestBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][3*MAX_AUXCH_PER_AUXMCU]; //用于登记那些辅助通道数据被请求了,*3 是因为有3种数据

    static unsigned char 				 m_AuxDataRequestCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //每个IV通道下的辅助通道数据请求个数
    static unsigned char         ADC_SampleProcessType[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];      //采样处理类型，中断或者任务，电池包要用到

    static signed int                   m_Count_Limit_AllChannel[MAX_IVMCU_PER_SYS] ;
    static signed int                   m_Count_AnyLimit_AllChannel[MAX_IVMCU_PER_SYS] ;

    static const unsigned char          FramConfigDictionary[32*1024/8];                              // Fram存储的Config字典
#if DEBUG_TRACE_RECORD == 1
    static St_DebugTraceRecord          DebugTraceRecord;
#endif
    static 	ST_AUX_DOCtrl    				m_AuxDOCtrlMgr[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
    static bool            CANBMSEnableFlag;
    static bool           m_Enable_2ndV_LC_Sample;  // Enable Low Cost 2nd Votlage boards sample task;
    static bool           m_Enable_2ndV_BC_Sample;  // Enable Battery Cell 2nd Votlage boards sample task;

    static bool                  m_Inital_GPIO_4_LC_Boards; // To remeber if the GPIO for Low Cost Voltage boards has been initialized.
    static ST_AUX_DATA    st_Aux_Data;
    static bool                  m_bLogAuxDxdt;    //160429

public:
    static void Init(void);    //开机上电默认初始化
    //之后可以读FRAM等再次更新
    static void Aux_GetSystemConfig(void); 		//获取从以太网下来的有关本辅助MCU的系统配置信息,结果存于m_SystemConfig
    static void Aux_GetActualConfig(void);  	//扫描所有可能的辅助版，从BID拨码 0到7，将结果存于m_ActucalConfig
    static void Aux_BuildActiveConfig(void);  //建立实际的系统配置， 存放于m_ActualConfig,供后续所有的任务使用
    static void Aux_ReportMatchFailed(void); 	// 报告MAP出错，并将相应的通道置为不可用
    static unsigned long Aux_GetMapfromPC(unsigned char * uc_ErrorFlag);  	//从以太网下来的batch获取MAP信息,填入m_PatAuxToIV

    static void Aux_BuildLongSnake(void);

    static void Aux_TranslateMapFromPC(void );
    static void Aux_ScanIVMCU_UnitID(void);
    static unsigned char Aux_LookupIVunitID (unsigned char uc_ID);
    static void Aux_ObtainGainOffset(void);
    static bool Aux_GetData(unsigned char uc_IV_unitID,unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxCh, ST_AUX_DATA * result);
    static bool TransAuxChnFromSequence2Physical(unsigned char uc_IV_PhysicalID, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxSequenceChan, unsigned char *uc_BoradIdx, unsigned char *uc_AuxPhysicalchn); // 20170228 zyx


    static unsigned char Aux_LocateIVUnitBufferIndex(unsigned char uc_IV_unitID); //对于给定一个Unit ID，比如5，它的信息在MAP里的那个buffer里放做
    static unsigned char Aux_LocateAuxBufferIndex(unsigned char IvUnitBufIdx,unsigned char IvChanIdx,unsigned char AuxType,unsigned char AuxIdxIn_AuxUnit);

    static unsigned char Aux_GetLimitCount(unsigned char uc_IVch);
    static unsigned char Aux_GetIfAnyLimitCount(unsigned char uc_IVch);
    static unsigned char Aux_AddLimit(ST_AUX_LIMIT st_limit);
    static unsigned char Aux_AddIfAnyLimit(ST_AUX_IFANY_LIMIT st_ifanylimit);
    static unsigned char Aux_ClearLimits( unsigned char uc_IV_unitID,unsigned char uc_IVch);
    static unsigned char Aux_AddSafety (ST_AUX_HALF_SAFETY st_half_safety);
    static unsigned char Aux_ClearSafeties (unsigned char uc_IV_unitID, unsigned char uc_IVch);
    static unsigned char Aux_AddSDL (ST_AUX_SDL st_sdl);
    static unsigned char Aux_AddDataRequest (ST_AUX_DATA_REQUEST st_data_request);
    static unsigned char Aux_ClearDataRequests(unsigned char uc_IV_unitID, unsigned char uc_IVch);
    static void Aux_TrigLogPoint( ST_AUX_DATA_TRIG * TrigInfo, St_SDL_Point_Pack * MyPoint);
	static void Aux_TrigLogGroupPoint(ST_AUX_DATA_TRIG* TrigInfo, St_SDL_OnePoint* MyPoint);
	static void Aux_TrigLogGroupPoint_dxdt(ST_AUX_DATA_TRIG* TrigInfo, St_SDL_OnePoint* MyPoint);
    // 存储和读取配置信息
    static void Init_BoardCFG(void);
    static void SaveChannelConfigPara(unsigned short ChannelCount,unsigned char * pChannelIdx);
    static unsigned char FindNextAuxADCid(void);
    static unsigned short CheckAllAuxADCStatus(void);  //add by qjm 20150505
    static char  SettingCS_Low(unsigned char m_CS_Address);
    static char  SettingCS_High(void);

    static unsigned char findNextChn(unsigned char u_ADCid); //add by qjm 20150512
    static unsigned char findNextADC_CS(unsigned char ADCidx);
    static unsigned char  findLastChn(unsigned char ADCID);
    static unsigned long CalibrateVoltageDAC(unsigned char uc_datagroupidx,unsigned char uc_ChannelNo,float f_Voltage);
    static unsigned long CalibrateDAC_for_DDS(unsigned char uc_datagroupidx, unsigned char uc_ChannelNo,float f_RSM);
    static float UnifyADCVoltage(unsigned char uc_ChannelNo, float fCurrentValue);
    static float UnifyVoltage( unsigned char uc_ChannelNo,  float fVoltageValue );
    static float DeUnifyVoltage( unsigned char uc_ChannelNo, float fUniVoltageValue);
    static float DeUnifyADCVoltage(unsigned char uc_ChannelNo, float fUniCurrentValue);
    static float GetUnifyCurrent( unsigned char  uc_ChannelNo, unsigned char IRange );
    static float GetUnifyVoltage( unsigned char  uc_ChannelNo,  unsigned char VRange  );
    static float GetFloatCurrent( unsigned char  uc_ChannelNo);
#if DEBUG_TRACE_RECORD == 1
    static void  InitDebugTraceRecord(void);
    static void  PushDebugTraceRecord(St_DebugPoint myDebugPoint);
#endif

public:
    static float T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_JType(float f_Value,unsigned char uc_CheckFlag);
    static float T_t90_Convert_EType(float f_Value,unsigned char uc_CheckFlag);

    static signed char  AuxTypeIndexTable[MAX_AUX_TYPE_PER_MCU];//辅助限制
    static unsigned char Aux_LocateAuxTypeArrayIndex(unsigned char uc_AuxType);
    static signed char Aux_LocateAuxTypeInArray(unsigned char uc_i);
    static void Aux_ConfirmContainAuxTypes(void);//辅助限制
};



//////////////////////////////////////////////
// 参考

// 某辅助类型通道号在系统配置里的定义
// Cluster级	指明系统内某特定辅助类型的通道的总和。即最大全局通道号。(Index_InGloble)
// Unit级	该特定辅助类型通道的在该UNIT内的全局起止序号(unit ID, Index_InGloble)
// IVChannel级/Schedule级	分配给该IV Channel使用的辅助通道的，IV 内部辅助通道序号 （从1 开始），和全局序号有一一对应关系. (Index_InIVCh, Index_InGloble)
// IVMCU级	各个IV通道内的辅助通道逻辑序号和辅助UNIT号，及在该辅助UNIT内的该类型通道的序号 (Index_InIVCh, UnitID, Index_InUnit)
// AUXＭＣＵ级	该类型辅助通道板的BID号，及相应的unit内通道序号和物理通道号的对应关系 (BID, Index_InUnit, Index_InBoard)






