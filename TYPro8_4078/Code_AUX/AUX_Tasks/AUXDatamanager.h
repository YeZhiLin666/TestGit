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

    static st_Aux_Chn_Info st_Aux_info[MAX_BOARD_PER_AUXMCU][17];	 //������channle info  //��ʱ����  //  ��ذ�����Ҫ�����

    static st_Aux_Sample st_aux_sample_setting[MAX_BOARD_PER_AUXMCU][17]; //��ذ���Ҫ��

    static ST_AUX_ADC_CS     Aux_ADC_CS_Info[16];  //Add by DKQ 05.04.2015   //��¼ÿ��Ƭѡ�ź���ص���Ϣ
    static unsigned short    Aux_ADC_TimeOut_Flag; //Add by QJM	 05.05.2015  //��¼ÿ��CS��Ӧ��ADC timeout
    static unsigned char     Aux_ADC_TotalCnt;
    //		static ST_AUXSAMPLE 				m_AuxSample;
    static ST_AUX_BOARDCFG				m_Aux_BoardCFG[MAX_BOARD_PER_AUXMCU];   //�洢���ڷֲ㴦��İ���Ϣ

    static ST_MAP_FROM_PC						m_MapFromPC[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];  //����unit�õ�MAP��
    static MsTime                   m_TriggerLogTime[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];  //160429
    //static St_DOWNLOAD_MAP_FROM_PC  m_DownLoadMapFromPC[MAX_AUXCH_PER_AUXMCU]; //����ʱ�õĸ���MAP�ṹ�����Ӧת���ɴ����
    static St_DOWNLOAD_MAP_FROM_PC  m_DownLoadMapFromPC[MAX_MAP_COUNT_PER_UNIT]; //����ʱ�õĸ���MAP�ṹ�����Ӧת���ɴ����

    static ST_IVMCUID_CFG       		m_IVMCU_ID_Info[MAX_IVMCU_PER_SYS];    //IV MCU ID ��Ч��Ϣ
    static ST_AUX_BOARD_INFO  	        m_AllAuxBoards[MAX_BOARD_PER_AUXMCU];  //���ڴ洢ɨ��EEPROM��ȡ����Ϣ
    static ST_AUX_BOARD_INFO_IN_EEPROM  m_AuxBoardInfo4EEPROM;

    static ST_AUX_IN_UNIT_LOOKUP_TABLE  m_AuxChnLookupTable [MAX_AUX_TYPE]; //��¼ÿ�ָ����ڳ������λ���Ա���ٲ���
    static ST_AUX_CHN_LOCATION          m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU];    //�������󡱣����ڼ�¼��unit��ÿ������ͨ��������������bank�е�λ��

    static ST_AUX_DATA  		      m_AuxDataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//���ڰ�Data_Group��Ÿ���ͨ������.
    //	static ST_AUX_DIDO_DATA  		  m_Aux_DIDO_DataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//���ڰ�Data_Group��Ÿ���ͨ������.
    static unsigned char         m_DGIdx2BIdx[MAX_DATA_GROUP]; //
    static ST_AUX_P2E_IN_UNIT         m_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];// (PannelChannel number to Electic Channel Number)
    //	static unsigned char         m_DIDO_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];// (PannelChannel number to Electic Channel Number)

    static bool                  m_DataGroupMgr[MAX_DATA_GROUP]; //���ڱ��ÿ��data group �Ƿ�����Ч
    //	static bool                  m_DIDO_DataGroupMgr[MAX_DATA_GROUP]; //���ڱ��ÿ��data group �Ƿ�����Ч

    static ST_INCHIP_TEMP         m_InChipTemperature[MAX_DATA_GROUP];  //���ڴ��ADCƬ���¶ȹ��ȵ�ż��˲���

    static ST_AUX_LIMIT 				m_AuxLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_LIMIT_PER_IVCH];
    static unsigned char 				m_AuxLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

    static ST_AUX_IFANY_LIMIT 	m_AuxIfAnyLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_IFANY_LIMIT_PER_IVCH];
    static unsigned char 				m_AuxIfAnyLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

    static ST_AUX_SAFETY				m_AuxSafetyBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];     //add by dirui 20131206   ÿ������Ӧ�ÿ��Դ洢����High/Low ��������
    static unsigned char 				m_AuxSafetyCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //ÿ��IVͨ���µĸ���ͨ��safety���ø������ܲ�һ��


    static ST_AUX_SDL						 m_AuxSDLBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU]; //SDL�������������ͷ֣���IVͨ��֮�����������ͬ
    static ST_AUX_DATA_REQUEST 	 m_AuxDataRequestBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][3*MAX_AUXCH_PER_AUXMCU]; //���ڵǼ���Щ����ͨ�����ݱ�������,*3 ����Ϊ��3������

    static unsigned char 				 m_AuxDataRequestCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //ÿ��IVͨ���µĸ���ͨ�������������
    static unsigned char         ADC_SampleProcessType[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];      //�����������ͣ��жϻ������񣬵�ذ�Ҫ�õ�

    static signed int                   m_Count_Limit_AllChannel[MAX_IVMCU_PER_SYS] ;
    static signed int                   m_Count_AnyLimit_AllChannel[MAX_IVMCU_PER_SYS] ;

    static const unsigned char          FramConfigDictionary[32*1024/8];                              // Fram�洢��Config�ֵ�
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
    static void Init(void);    //�����ϵ�Ĭ�ϳ�ʼ��
    //֮����Զ�FRAM���ٴθ���
    static void Aux_GetSystemConfig(void); 		//��ȡ����̫���������йر�����MCU��ϵͳ������Ϣ,�������m_SystemConfig
    static void Aux_GetActualConfig(void);  	//ɨ�����п��ܵĸ����棬��BID���� 0��7�����������m_ActucalConfig
    static void Aux_BuildActiveConfig(void);  //����ʵ�ʵ�ϵͳ���ã� �����m_ActualConfig,���������е�����ʹ��
    static void Aux_ReportMatchFailed(void); 	// ����MAP����������Ӧ��ͨ����Ϊ������
    static unsigned long Aux_GetMapfromPC(unsigned char * uc_ErrorFlag);  	//����̫��������batch��ȡMAP��Ϣ,����m_PatAuxToIV

    static void Aux_BuildLongSnake(void);

    static void Aux_TranslateMapFromPC(void );
    static void Aux_ScanIVMCU_UnitID(void);
    static unsigned char Aux_LookupIVunitID (unsigned char uc_ID);
    static void Aux_ObtainGainOffset(void);
    static bool Aux_GetData(unsigned char uc_IV_unitID,unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxCh, ST_AUX_DATA * result);
    static bool TransAuxChnFromSequence2Physical(unsigned char uc_IV_PhysicalID, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char uc_AuxSequenceChan, unsigned char *uc_BoradIdx, unsigned char *uc_AuxPhysicalchn); // 20170228 zyx


    static unsigned char Aux_LocateIVUnitBufferIndex(unsigned char uc_IV_unitID); //���ڸ���һ��Unit ID������5��������Ϣ��MAP����Ǹ�buffer�����
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
    // �洢�Ͷ�ȡ������Ϣ
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

    static signed char  AuxTypeIndexTable[MAX_AUX_TYPE_PER_MCU];//��������
    static unsigned char Aux_LocateAuxTypeArrayIndex(unsigned char uc_AuxType);
    static signed char Aux_LocateAuxTypeInArray(unsigned char uc_i);
    static void Aux_ConfirmContainAuxTypes(void);//��������
};



//////////////////////////////////////////////
// �ο�

// ĳ��������ͨ������ϵͳ������Ķ���
// Cluster��	ָ��ϵͳ��ĳ�ض��������͵�ͨ�����ܺ͡������ȫ��ͨ���š�(Index_InGloble)
// Unit��	���ض���������ͨ�����ڸ�UNIT�ڵ�ȫ����ֹ���(unit ID, Index_InGloble)
// IVChannel��/Schedule��	�������IV Channelʹ�õĸ���ͨ���ģ�IV �ڲ�����ͨ����� ����1 ��ʼ������ȫ�������һһ��Ӧ��ϵ. (Index_InIVCh, Index_InGloble)
// IVMCU��	����IVͨ���ڵĸ���ͨ���߼���ź͸���UNIT�ţ����ڸø���UNIT�ڵĸ�����ͨ������� (Index_InIVCh, UnitID, Index_InUnit)
// AUX�ͣãռ�	�����͸���ͨ�����BID�ţ�����Ӧ��unit��ͨ����ź�����ͨ���ŵĶ�Ӧ��ϵ (BID, Index_InUnit, Index_InBoard)






