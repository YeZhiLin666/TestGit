///////////////////////////////////////////////////////////////////////////////////
//File Name: AUXDatamanager.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						�������ݹ������
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////

#include "../Entry/includes.h"
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
// 32K�б���һ��0XFF����8���ֽڴ洢λ��
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


//----// K���¶Ȱ�-----------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��0(��C)��1372(��C)
//��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
const float  f_CoEA_Ktype[9 ]= {0.0f,  2.5173462e-2,    -1.1662878e-6,  -1.0833638e-9,   -8.977354e-13,
                                -3.7342377e-16,  -8.6632643e-20, -1.0450598e-23,  -5.1920577e-28
                                };
//�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
const float  f_CoEB_Ktype[10] = {    0.0f,     2.508355e-2,    7.860106e-8,   -2.503131e-10,   8.31527e-14,
                                     -1.228034e-17,     9.804036e-22,   -4.413030e-26,    1.057734e-30,   -1.052755e-35
                                     };
//�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
const float  f_CoEC_Ktype[7] = {  -1.318058e2, 4.830222e-2,  -1.646031e-6, 5.464731e-11, -9.650715e-16,  8.802193e-21,  -3.110810e-26  };

//---------// K���¶Ȱ�-----------------------------------------------------------------------------------------------------

//--------// T���¶Ȱ�-----------------------------------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��0(��C)��400(��C)        T type thermocouple
//T_type�ȵ�ż��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
const float    f_CoEA_Ttype[8] = {   0.0,  2.5949192e-2, -2.1316967e-7,  7.9018692e-10, 4.2527777e-13,
                                     1.3304473e-16,  2.0241446e-20, 1.2668171e-24
                                 };
//�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
const float    f_CoEB_Ttype[7] = {   0.0,   2.5928e-2, -7.602961e-7,  4.637791e-11, -2.165394e-15,
                                     6.048144e-20,  -7.293422e-25
                                 };

//-------// T���¶Ȱ�-----------------------------------------------------------------------------------------------------------

//----// J���¶Ȱ�-----------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��-210(��C)��1200 (��C)
//��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-210(��C)��0(��C) ��΢����ѹ��Χ��-8095(��V)��0(��V)
const float    f_CoEA_Jtype[9] = {   0.0f,  1.9528268e-2, -1.2286185e-6,  -1.0752178e-9, -5.9086933e-13,
                                     -1.7256713e-16,  -2.8131513e-20, -2.3963370e-24,  -8.3823321e-29
                                     };
//�¶ȷ�Χ��0(��C)��760(��C) ��΢����ѹ��Χ��0(��V)��42929(��V)
const float    f_CoEB_Jtype[8] = {    0.0f,     1.978425e-2,    -2.001204e-7,   1.036969e-11,   -2.549687e-16,
                                      3.585153e-21,     -5.344285e-26,   5.099890e-31
                                 };
//�¶ȷ�Χ��760(��C)��1200(��C) ��΢����ѹ��Χ��42929(��V)��69553(��V)
const float    f_CoEC_Jtype[6] = {  -3.11358187e3, 3.00543684e-1,  -9.94773230e-6, 1.70276630e-10, -1.43033468e-15,4.73886084e-21 };

//---------// J���¶Ȱ�-----------------------------------------------------------------------------------------------------

//--------// E���¶Ȱ�-----------------------------------------------------------------------------------------------------------
//����ʱ�¶�ת��Ϊ΢����ѹ���峣��ϵ��
//�¶ȷ�Χ��-200(��C)��1000(��C)        E type thermocouple
//T_type�ȵ�ż��΢����ѹת��Ϊ��ʱ�¶ȶ��峣��ϵ��
//�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-8825(��V)��0(��V)
const float    f_CoEA_Etype[9] = {   0.0,  1.6977288e-2, -4.3514970e-7,  -1.5859697e-10, -9.2502871e-14,
                                     -2.6084314e-17,  -4.1360199e-21, -3.4034030e-25, -1.1564890e-29
                                     };
//�¶ȷ�Χ��0(��C)��1000(��C) ��΢����ѹ��Χ��0(��V)��76373(��V)
const float    f_CoEB_Etype[10] = {   0.0,   1.7057035e-2, -2.3301759e-7,  6.5435585e-12, -7.3562749e-17,
                                      -1.78960001e-21,  8.4036165e-26, -1.3735879e-30, 1.0629823e-35, -3.2447087e-41
                                      };

//-------// E���¶Ȱ�-----------------------------------------------------------------------------------------------------------
st_Aux_Chn_Info 	Aux_DataManager::st_Aux_info[MAX_BOARD_PER_AUXMCU][17];	 //������channle info  //��ʱ����

st_Aux_Sample 		Aux_DataManager::st_aux_sample_setting[MAX_BOARD_PER_AUXMCU][17];

ST_AUX_ADC_CS     Aux_DataManager::Aux_ADC_CS_Info[16];//Add by DKQ 05.04.2015Ŀǰ���֧��16��ADCƬѡ��ַ
unsigned short    Aux_DataManager::Aux_ADC_TimeOut_Flag;
unsigned char     Aux_DataManager::Aux_ADC_TotalCnt;  // add byqjm 05.05.2015

//ST_AUXSAMPLE 	  		Aux_DataManager::m_AuxSample;
ST_AUX_BOARDCFG	   	Aux_DataManager::m_Aux_BoardCFG[MAX_BOARD_PER_AUXMCU];   //�洢���ڷֲ㴦��İ���Ϣ
signed int          Aux_DataManager::m_Count_Limit_AllChannel[MAX_IVMCU_PER_SYS];  //
signed int          Aux_DataManager::m_Count_AnyLimit_AllChannel[MAX_IVMCU_PER_SYS];
ST_MAP_FROM_PC		   Aux_DataManager::m_MapFromPC[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
MsTime              Aux_DataManager::m_TriggerLogTime[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];//160429
//St_DOWNLOAD_MAP_FROM_PC		Aux_DataManager::m_DownLoadMapFromPC[MAX_AUXCH_PER_AUXMCU]; //���������PC��MAP��Ϣ
St_DOWNLOAD_MAP_FROM_PC		Aux_DataManager::m_DownLoadMapFromPC[MAX_MAP_COUNT_PER_UNIT]; //���������PC��MAP��Ϣ

ST_IVMCUID_CFG      Aux_DataManager::m_IVMCU_ID_Info[MAX_IVMCU_PER_SYS];


ST_AUX_BOARD_INFO  	    Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU];  //���ڴ洢ɨ��EEPROM��ȡ����Ϣ
ST_AUX_BOARD_INFO_IN_EEPROM  Aux_DataManager::m_AuxBoardInfo4EEPROM;  //���ڴ�ȡ�Ͷ�ȡE2PROM��İ���Ϣ��
// ST_AUX_BOARD_INFO_IN_EEPROM  Aux_DataManager::m_AuxBoardInfoRead[MAX_BOARD_PER_AUXMCU];  //���ڴ�ȡ�Ͷ�ȡE2PROM��İ���Ϣ��
ST_AUX_IN_UNIT_LOOKUP_TABLE    Aux_DataManager::m_AuxChnLookupTable [MAX_AUX_TYPE]; //
ST_AUX_CHN_LOCATION            Aux_DataManager::m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU];    //���ڼ�¼��unit��ÿ������ͨ���������ַ�������ַ�͵���ͨ����ַ



ST_AUX_DATA  		      Aux_DataManager::m_AuxDataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//���ڰ����ʹ�Ÿ���ͨ������.
//ST_AUX_DIDO_DATA  		Aux_DataManager::m_Aux_DIDO_DataBank[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//���ڰ����ʹ�ŷ�ADC�͸���ͨ������.

unsigned char         Aux_DataManager::m_DGIdx2BIdx[MAX_DATA_GROUP]; //��¼ÿ��Data_Group_Index ��Ӧ�İ��ַBoard_Index
ST_AUX_P2E_IN_UNIT         Aux_DataManager::m_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//  (PannelChannel number to Electic Channel Number)
//unsigned char         Aux_DataManager::m_DIDO_PChn2EChn [MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];//  (PannelChannel number to Electic Channel Number)

bool                  Aux_DataManager::m_DataGroupMgr[MAX_DATA_GROUP]; //���ڱ��ÿ��data group �Ƿ�����Ч
//bool                  Aux_DataManager::m_DIDO_DataGroupMgr[MAX_DATA_GROUP]; //���ڱ��ÿ��data group �Ƿ�����Ч

ST_INCHIP_TEMP        Aux_DataManager::m_InChipTemperature[MAX_DATA_GROUP];  //���ڴ��ADCƬ���¶�

ST_AUX_LIMIT 		    Aux_DataManager::m_AuxLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_LIMIT_PER_IVCH];
unsigned char 			Aux_DataManager::m_AuxLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

ST_AUX_IFANY_LIMIT 	    Aux_DataManager::m_AuxIfAnyLimitBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_IFANY_LIMIT_PER_IVCH];
unsigned char 			Aux_DataManager::m_AuxIfAnyLimitCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
ST_AUX_SAFETY				    Aux_DataManager::m_AuxSafetyBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
unsigned char 				 	Aux_DataManager::m_AuxSafetyCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //ÿ��IVͨ���µĸ���ͨ��safety���ø������ܲ�һ��

ST_AUX_SDL					    Aux_DataManager::m_AuxSDLBank[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];//SDL�������������ͷ֣���IVͨ��֮�����������ͬ

ST_AUX_DATA_REQUEST 		    Aux_DataManager::m_AuxDataRequestBank [MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][3*MAX_AUXCH_PER_AUXMCU]; //���ڵǼ���Щ����ͨ�����ݱ�������
unsigned char 				 	Aux_DataManager::m_AuxDataRequestCount[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU]; //ÿ��IVͨ���µĸ���ͨ�������������
unsigned char        		Aux_DataManager:: ADC_SampleProcessType[MAX_BOARD_PER_AUXMCU][MAX_ADCNUM_PER_BOARD][MAX_CH_IN_AUX_UNIT];

bool                    Aux_DataManager::CANBMSEnableFlag=0;
bool           Aux_DataManager::m_Enable_2ndV_LC_Sample = false;  // Enable Low Cost 2nd Votlage boards sample task;
bool           Aux_DataManager::m_Enable_2ndV_BC_Sample = false;  // Enable Battery Cell 2nd Votlage boards sample task;

bool                    Aux_DataManager::m_Inital_GPIO_4_LC_Boards = false ; // Not initial the GPIO for 2ndV_LC boards yet
//DIDO manager
ST_AUX_DOCtrl                 Aux_DataManager::m_AuxDOCtrlMgr[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];

ST_AUX_DATA    Aux_DataManager::st_Aux_Data;
bool                    Aux_DataManager::m_bLogAuxDxdt;    //160429

signed char  Aux_DataManager::AuxTypeIndexTable[MAX_AUX_TYPE_PER_MCU];//��������
#pragma arm section //��������������


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


//��ʱ�ֶ����m_AllAuxBoards����ΪAUX����Ϣ
//����1��2�����ô����ж�    ��Ϊm_AllAuxBoards����ʹ�õĻ��нϴ��࣬�ڲ�û��״̬������Ҫ�����������һ��
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
            m_AuxLimitCount[uc_IVunitBufIndex][i] = 0;  //��ʼ���������limit
            m_AuxIfAnyLimitCount[uc_IVunitBufIndex][i] = 0; //��ʼ���������IfAny limit

            for(unsigned char j = 0; j< MAX_AUX_TYPE_PER_MCU; j++)//��������
            {
                memset(&m_MapFromPC[uc_IVunitBufIndex][i][j],0,sizeof(ST_MAP_FROM_PC));
            }
        }
    } while(MYIVUNITINDEX(uc_IVunitBufIndex));

    Aux_DataManager::CANBMSEnableFlag=0;
    Init_BoardCFG();
    for(unsigned char i = 0; i<MAX_AUX_TYPE_PER_MCU; i++) //��������
    {
        AuxTypeIndexTable[i] = -1;
    }
// 	INIT_QUEUE(CANFunc::CQ_CanBmsRxBuf2);

}

//����ĺ������ڽ���PC��������map��Ϣת���ɷ��������IVMCU�ĸ���IVͨ���ĸ����͵ĸ���ͨ���ڱ�unit�ڵĸ�������ͨ���ŵ�
// m_MapFromPC��ȥ��
// �µĸ�������Ҫ������MAP
// Ҫע���ʱ����ÿһ��IVMCU������unit ID��Ҫ�����m_IVMCU_ID�У���ΪIVMCU��ID�ڲ������в����������ġ����Ƕ����ġ���
// ���У�ÿ��һ��IVMCU map ��������unit��Ҫ�� m_Valid_IV[ ]��1���Ա�Ӧ�ó�������ж������յ�MAP��
//����PC��һ�����


unsigned long Aux_DataManager::Aux_GetMapfromPC(unsigned char * uc_ErrorFlag)
{
    MP_AllIVMap    * Lst_AllIVMap;  // Local Mete
    Lst_AllIVMap = (MP_AllIVMap   *) &EthernetParse::m_btPackDataBuffer[0];

    unsigned short int uw16_ChSum = 0;
    for (unsigned char i=0; i<MAX_IVMCU_PER_SYS; i++)
    {   // ͳ��MAP���и���ͨ��������
        for (unsigned char j=0; j<MAX_IVCH_PER_IVMCU; j++)
        {
            for (unsigned char k=0; k<MAX_AUX_TYPE; k++)
                uw16_ChSum += Lst_AllIVMap->m_Total_In_ThisIVCh[i][j][k];  // ͳ��MAP���и���ͨ��������
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
            if(++uw16_count >= uw16_ChSum)   // ͳ��MAP���и���ͨ��������
                break;
        }
    }

    //unsigned char uc_IVunitBufIndex = 0;
    unsigned long uw32_Get = sizeof(St_DOWNLOAD_MAP_FROM_PC);
    /*   unsigned long uw32_length = sizeof(MP_AllIVMap);
      uw32_length -=  uw32_Get *  MAX_MAP_COUNT_PER_UNIT; // ����һ�ΰ���ǰ�������Ϣ����
      */
    unsigned long uw32_length = (sizeof(MP_AllIVMap) - uw32_Get * MAX_MAP_COUNT_PER_UNIT); // ����һ�ΰ���ǰ�������Ϣ����

//   for(unsigned short i=0;i<(MAX_MAP_COUNT_PER_UNIT-uw16_ChSum);i++)
//        memset(&EthernetParse::m_btPackDataBuffer[uw32_length + (uw16_ChSum+i)*uw32_Get],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));  // ����û������512���Ŀռ䣬�������ʱ����������
    for(unsigned short i=uw16_ChSum; i<(MAX_MAP_COUNT_PER_UNIT-uw16_ChSum); i++)
        memset(&Lst_AllIVMap->m_AllIVMap[i],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));  // ����û������512���Ŀռ䣬�������ʱ����������

    uw32_length += (uw32_Get * uw16_ChSum);  // �����ʵ��һά�ֽ�����
    return (uw32_length);  // �ϴ�һά�洢�ֽڳ���
}

void Aux_DataManager::Aux_ScanIVMCU_UnitID(void)  //ɨ�趼��MAP���ø���unit��IVunit��unit���ɣĲ��ŵ� m_IVMCU_ID_Info��ȥ��
{
    unsigned short ptr=0;
    unsigned short count=0;
    memset(&m_IVMCU_ID_Info,0,sizeof(ST_IVMCUID_CFG)*MAX_IVMCU_PER_SYS);
    m_IVMCU_ID_Info[0].m_ID = m_DownLoadMapFromPC[0].m_IVUnit ;  //��һ��IVunit
    m_IVMCU_ID_Info[0].m_Valid= 1;
    ptr++;
    count++;
    for (unsigned short i = 1; i< MAX_MAP_COUNT_PER_UNIT; i++)   //    MAX_AUXCH_PER_AUXMCU
    {
        if(!m_DownLoadMapFromPC[i].m_valid)
            continue;
        for (unsigned char j=0; j< count; j++)  //��ѯ�Ƿ��Ѿ��и�IV�������ˡ�
        {
            if (  m_IVMCU_ID_Info[j].m_ID == m_DownLoadMapFromPC[i].m_IVUnit )
                break; //���У��������ˣ�
            else
            {
                if (j == (count-1) ) //�������˶�û���ҵ���ͬ��
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



void Aux_DataManager::Aux_TranslateMapFromPC(void )   //�����ص�MAP��Ϣ�� IVUNIT->IVCH->��������չ��,������DataBank�е�λ���ҳ���;
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
                    continue;//��������  ���Ҹø��������Ƿ���ڣ�
                m_MapFromPC[i][j][TempAuxTypeIdex]. m_ActualCounts = 0;  //Ϊÿһ���������Ͳ���֮ǰ����0
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

                        m_MapFromPC[i][j][TempAuxTypeIdex].m_MapInfo[ptr].m_Index_InIVCh = (m_DownLoadMapFromPC[m].m_Index_InIVCh_HighBit << 8) | (m_DownLoadMapFromPC[m].m_Index_InIVCh & 0xff);//m_Index_InIVCh_HighBitΪm_Index_InIVCh�ĸ�λ
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






//����ĺ�����I2Cȥ��ȡ����Ϣ�����ֵ� m_AllAuxBoards; ������


//����PC������������Ϣm_MapFromPC ���Ӱ��ϻ�ȡ�ĸ�������ͨ����ʵ��������Ϣ m_AllAuxBoards�������������������ַ��
//����IVͨ����˳�򼰸��ָ���ͨ����˳�����г�һ���������� m_AllChnsInAuxUnit,���������� m_Iv2AuxMap



//�ڶ������еİ��EEPROM ��Ϣ�� m_AllAuxBoards, m_AuxDataBank, m_PChn2EChn, Ӧ�þ���ӵ���㹻����Ϣ������
// m_AuxChanLongSnake [MAX_AUXCH_PER_AUXMCU] �� m_AuxChnLookupTable [MAX_AUX_TYPE]; //

void Aux_DataManager::Aux_BuildLongSnake(void)
{

    unsigned char uc_SnakePointer =0;
    for (unsigned char j=0; j< MAX_AUX_TYPE; j++)   //ɨ�����еĸ�������
    {

        m_AuxChnLookupTable[j].m_TotalChnNum =0;  //�����͵�ͨ������ʼ��Ϊ0
        m_AuxChnLookupTable[j].m_StartInSnake  = uc_SnakePointer;  //���¸����͵��ڳ����������
        // if((j!=AUX_DO)&&(j!=AUX_DI))   //����Ƿ�DIDO�����ͣ� �� m_AuxDataBank����
        {   for (unsigned char i=0; i<MAX_DATA_GROUP; i++) //ɨ�����е�DATA_GROUP
            {
                if(!m_DataGroupMgr[i])
                    continue;

                for(unsigned char k=0; k<MAX_AUXCH_PER_DATA_GROUP; k++)  //ɨ��ÿ��DATAgroup��ÿ�����ͨ��
                {
                    if(!m_AuxDataBank[m_PChn2EChn[i][k].e_data_group_id][m_PChn2EChn[i][k].e_chan_id].m_Chn_Info.m_Populated)
                        continue;
                    if(m_AuxDataBank[m_PChn2EChn[i][k].e_data_group_id][m_PChn2EChn[i][k].e_chan_id].m_Chn_Info.m_AuxType == j) //�鿴AuxDataBank������ͨ����Ӧ�ĵ���ͨ����
                        //�������ͺ�������ɨ�������
                    {
                        m_AuxChnLookupTable[j].m_TotalChnNum ++;      //�����͵�ͨ������1
                        m_AuxChanLongSnake [uc_SnakePointer].m_valid = 1;
                        m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[i][k].e_data_group_id;  //data_group_idx �ų�������
                        m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[i][k].e_chan_id;  //����ͨ����ַ�ų�����
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
            m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[1][0].e_data_group_id;  //data_group_idx �ų�������
            m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[1][0].e_chan_id;  //����ͨ����ַ�ų�����
        }
        else
        {
            m_AuxDataBank[m_PChn2EChn[0][8].e_data_group_id][m_PChn2EChn[0][8].e_chan_id].m_Chn_Info.m_Populated = 1;
            m_AuxChanLongSnake [uc_SnakePointer].m_data_group_idx = m_PChn2EChn[0][8].e_data_group_id;  //data_group_idx �ų�������
            m_AuxChanLongSnake [uc_SnakePointer].m_chn = m_PChn2EChn[0][8].e_chan_id;  //����ͨ����ַ�ų�����
        }
        m_AuxChnLookupTable[1].m_TotalChnNum ++;      //�����͵�ͨ������1
        m_AuxChanLongSnake [uc_SnakePointer].m_valid = 1;
        uc_SnakePointer++;
    }
}



//����PC������������Ϣm_MapFromPC ���Ӱ��ϻ�ȡ�ĸ�������ͨ����ʵ��������Ϣ m_AllAuxBoards�������������������ַ��
//����IVͨ����˳�򼰸��ָ���ͨ����˳�����г�һ���������� m_AllChnsInAuxUnit,���������� m_Iv2AuxMap

//�����������ת��ʵ��UNITID�� �� ���IVMUC����Ϣ��BUFFER ��š���Ϊbuffer����Ų�������IVMCU UnitID��
unsigned char Aux_DataManager::Aux_LocateIVUnitBufferIndex(unsigned char uc_IV_unitID)
{
    unsigned char uc_i;
    for (uc_i = 0; uc_i < MAX_IVMCU_PER_SYS ; uc_i++)
    {
        if(m_IVMCU_ID_Info[uc_i].m_Valid)  //���û�Уɣ֣ͣãձ��ͣ�������
        {
            if(m_IVMCU_ID_Info[uc_i].m_ID == uc_IV_unitID)
                return uc_i;
        }
    }
    return 0xFF; //���û�ҵ��Ļ�������һ��0xFF
}
unsigned char Aux_DataManager::Aux_LocateAuxBufferIndex(unsigned char IvUnitBufIdx,unsigned char IvChanIdx,unsigned char AuxType,unsigned char AuxIdxIn_AuxUnit)
{
    unsigned char uc_i;

    unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(AuxType);
    if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
        return 0xFF;

    for (uc_i = 0; uc_i < Aux_DataManager::m_MapFromPC[IvUnitBufIdx][IvChanIdx][TempAuxTypeIdex].m_Counts_ByScheduleMap; uc_i++)
    {
        if(Aux_DataManager::m_MapFromPC[IvUnitBufIdx][IvChanIdx][TempAuxTypeIdex].m_MapInfo[uc_i].m_Index_InAuxUnit == AuxIdxIn_AuxUnit)
            return uc_i;
    }

    return 0xFF; //���û�ҵ��Ļ�������һ��0xFF
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
    if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
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
    if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
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
//     //�������ɻ�ÿ��IV��������LIMIT��SingleLimit/AnyLimit��������Ҫ�����ĸ����͵���Ŀ�أ�
//     // ԭ�����Ƿֳ�����������һ���� limit һ����Ifany limit
//     //return m_AuxLimitCount[uc_IVch];
// }

// unsigned char Aux_DataManager::Aux_GetIfAnyLimitCount(unsigned char uc_IVch)
// {
//     //�������ɻ�ÿ��IV��������LIMIT��SingleLimit/AnyLimit��������Ҫ�����ĸ����͵���Ŀ�أ�
//     // ԭ�����Ƿֳ�����������һ���� limit һ����Ifany limit
//     //return m_AuxIfAnyLimitCount[uc_IVch];
// }

unsigned char  Aux_DataManager::Aux_AddLimit(ST_AUX_LIMIT st_limit)
{
    unsigned char uc_IVunitBufIndex;
    uc_IVunitBufIndex =Aux_LocateIVUnitBufferIndex(st_limit.m_Source_CANID);

    unsigned char uc_EOB = m_AuxLimitCount[uc_IVunitBufIndex][st_limit.m_IV_index];  //��ȡ��limit��IVͨ���ĵ�ǰlimit��
    if(uc_EOB > MAX_LIMIT_PER_IVCH)
        return LIMIT_BANK_FULL;
// 	else
// 	{
    for(unsigned char Limit_Num = 0; Limit_Num<uc_EOB; Limit_Num++)
    {
        if(st_limit.m_limit_SN == m_AuxLimitBank [uc_IVunitBufIndex][st_limit.m_IV_index][Limit_Num].m_limit_SN)
            return NO_ERROR;   //Ϊ�ط�limit
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

    unsigned char uc_EOB = m_AuxIfAnyLimitCount[uc_IVunitBufIndex ][st_ifanylimit.m_IV_index];  //��ȡ��limit��IVͨ���ĵ�ǰIfany limit��
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

//����ĺ����������ĳ��IVͨ��������limit������һ���limit��IFany��limit
//Ŀǰ�������б�Ҫ��һ��������ͨ����limit����Ҫ����ΪIVMCU ��ÿ��IVͨ���ǵ�������ġ�
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
        // SDL ������Ҫ��Զ�IVMCU���������    DKQ
        // ���SDLģ���־��ֹͣ����SDL����
        for(unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)
        {
            unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(uc_AuxType);
            if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
                continue;
            // 			if(Aux_DataManager::m_Iv2Aux_Map[uc_IVch][uc_AuxType].m_chn_count == 0)
            if(Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap == 0)
                continue;  // break;   // ��Ϊ��λ����ʱ���Ͱ��Ѿ���������2Vd��T,...���2VDû��Ҳ����š�

            //AuxSDL::SDLEndedRoutine(uc_IVch, uc_AuxType, 0);	// ���SDL������־��ֹͣ����
            AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_AuxType, 0, LOG_PC_DATA, LOGSWITCH_OFF);

            // ��� Aux��IV������־
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
    if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
        return AUX_TYPE_OVERFLOW;

    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_Source_CANID = st_half_safety.m_Source_CANID;
    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_IV_index = st_half_safety.m_IV_index;
    m_AuxSafetyBank[uc_IVunitBufIndex][st_half_safety.m_IV_index][TempAuxTypeIdex].m_aux_type = st_half_safety.m_aux_type;
    //  ������뱻ע�͵�����Ϊ��ǰ������MAP�ģ�֪������ʼ�ͽ���ͨ���žͿ��Ա��ڼ�����еĸ���ͨ����
    //   ���ڵ����ɨ���ˡ�
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
        Aux_Safety_Check::CleanSafetyCheckResults(uc_IVunitBufIndex,st_half_safety.m_IV_index); //�Ѿ�������safety�������ǰ�ļ����  DKQ 12.17.2013
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
        m_AuxSafetyCount[uc_IVunitBufIndex ][uc_IVch] = 0;			 //����IVͨ���µ�safety����0 ����
        for(unsigned char i =0; i< MAX_AUX_TYPE; i++)
        {
            unsigned char TempAuxTypeIdex = Aux_LocateAuxTypeArrayIndex(i);
            if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
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
    if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
        return AUX_TYPE_OVERFLOW;

    m_AuxSDLBank[uc_IVunitBufIndex][st_sdl.m_IV_index][TempAuxTypeIdex] = st_sdl;
    return NO_ERROR;
}

//����ĺ�����������IV���������¼��AUXMCU�������ϱ�������������¼ͨ��CAN��IV�ϱ�����
//Ϊ�����ͬһ������ͨ����ͬһ���͵������ظ�����IVMCU������ʱӦ������־�������������Ӧ������

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

    unsigned char uc_EOB = m_AuxDataRequestCount[uc_IVunitBufIndex][st_data_request.m_IV_index];  //��ȡ��IVͨ���ĵ�ǰ������������
    if(uc_EOB>=(3*MAX_AUXCH_PER_AUXMCU))
        return AUX_DATA_REQUEST_BANK_FULL;
    {
        m_AuxDataRequestBank [uc_IVunitBufIndex][st_data_request.m_IV_index][uc_EOB] = st_data_request;
        m_AuxDataRequestCount[uc_IVunitBufIndex][st_data_request.m_IV_index] ++;
        AuxSDL::RequireData(st_data_request);  //��������ڶ�IVMCU��������Ƿ���Ҫ�޸ģ�
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
            if(TempAuxTypeIdex == 0xFF)//��������  ���û�иø��������������ˡ�
                continue;
            if(m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap!=0)
            {
                for(unsigned char uc_j =0  ; uc_j <= m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap ; uc_j ++)
                {
                    AuxSDL::SwitchLog(uc_IV_unitID, uc_IVch, uc_i, uc_j, REPORT_IV_DATA, LOGSWITCH_OFF);
                }
            }
        }
    }  //��ʱ����  qjm 20150603

    return NO_ERROR;
}



//��Ҫ����IV �·�����Ϣ���ҵ���Ӧ�ĸ���ͨ�����ݣ�Ȼ���Ϸ�PC
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
    //    ;// MyPoint->ucSensor =  Aux_DataManager::m_AllAuxBoards[TrigInfo->m_AuxType].m_AuxSensor_ChInfo[TrigInfo->m_AuxChnIdx].m_AuxTemp.m_AuxThermoCouple.SensorType;//��ʱ���� �뾲��20150603
    //   else
    //     MyPoint->ucSensor = TrigInfo->m_AuxType;
    MyPoint->ucChannelIdx = TrigInfo->m_AuxChnIdx;

    st_Aux_Data.m_Chn_Info.m_AuxType = TrigInfo->m_AuxType;

    EthernetParse::MPReportData(TrigInfo->m_IVChnIdx,SEND_AUX_LOGDATA);  // PC ��̫���緢��
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
    // 	Aux_ScanBoardInfo();  // ͨ�������������
    // 	Aux_GetMapfromPC( &uc_Result);    // ת��PCMap���Ա����
    //     Aux_BuildIv2AuxMap();  // ת��IVMap���Ա����
    // 	Aux_ObtainGainOffset(); //��ȡÿ��ͨ����Gain�� offset���� extra��Ϣ�� Add by DKQ 03.26.2014

}
// ����ĺ������ڻ�ȡ�������ϵĸ���ͨ����Gain��Offset�� �Լ� extra��Ϣ�����ڶ�����Ϣ����á�
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
    uc_DeviceAddr = uc_i;    //  ��Ŵ���
    break;
    }
    AuxCh_Count += m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_Total_Channels;  // ������ͨ����
    }

    if(uc_i >= MAX_BOARD_PER_AUXMCU)  // ����������ͬ��ͨ�����ͷ����洢��
    continue;

    Aux_DataManager::m_AllAuxBoards[uc_DeviceAddr].m_BoardInfo.m_abi_chn[uc_BChID].m_AuxSensorCoreInfo = st_Config->m_Channel[ChannelIndex].m_AuxSensorCoreInfo; // ����ͨ����Ϣ
    uw16_PhysicalCh = Aux_DataManager::Convert_PhysicalChID(uc_DeviceAddr, uc_BChID); // ͬһ���������Ӧͨ���Ŵ���
    //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
    //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
    AuxCConfig::WriteAUXChannelCfg( (MAX_BOARD_PER_AUXMCU - 1 - uc_DeviceAddr), uw16_PhysicalCh, &st_Config->m_Channel[ChannelIndex]);   // ͨ����Ϣ����I2C��24LC128

    if(uc_Board_FirstCH)    // дͬһ�鸨����ĵ�һ��ͨ����Ϣʱд��RSN��
    {
    uc_Board_FirstCH = 0;
    AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_I2C =  st_Config->m_Global.m_u16Board_RSN;   // ע�����RSN
    AuxCConfig::WriteAUXBoardRSN(MAX_BOARD_PER_AUXMCU - 1 - uc_DeviceAddr ,&AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_I2C);
    AuxConfigRSN::Check_RSN_Init( uc_DeviceAddr );

    AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_FRAM = 0;
    AuxCConfig::WriteRSN(uc_DeviceAddr, &AuxConfigRSN::st_BoardRSN[uc_DeviceAddr].uw16_BoardRSN_FRAM); // ����д��ʱ�Ż�ȫ��һ��FRAM�е�RSN

    AuxConfigRSN::Check_RSN_Init( uc_DeviceAddr );
    }
    }
    }	*/
}




float Aux_DataManager::T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag)
{
    //��΢����ѹת��Ϊ��ʱ�¶�
    //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
    //�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
    //�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // �����ȵ���ʱĬ��Ϊ22��ʱ��
    //[K���¶ȷ�Χ:-270(��C)��1372(��C)]
    if( f_Value < -6458 )        //��������  ΢����ѹΪ��С��λ����
        return ( -273.0 );
    if( f_Value >54886)
        return ( 1372 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
    }

    //�����
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9

    if(f_Value<0)       //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
    {
        CoE = &f_CoEA_Ktype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20644)        //�¶ȷ�Χ��0(��C)��500(��C) ��΢����ѹ��Χ��0(��V)��20644(��V)
    {
        CoE = &f_CoEB_Ktype[0];
        for(uc_i=1; uc_i<10; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=54886)       //�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��20644(��V)��54886(��V)
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
    //��΢����ѹת��Ϊ��ʱ�¶�
    //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
    //�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // �����ȵ���ʱĬ��Ϊ22��ʱ��
    //[T���¶ȷ�Χ:-270(��C)��400(��C)]
    if( f_Value < -6258 )        //��������  ΢����ѹΪ��С��λ����
        return ( -273.0 );
    if( f_Value >20872)
        return ( 400 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
    }

    //�����
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
    {
        CoE = &f_CoEA_Ttype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
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
    //��΢����ѹת��Ϊ��ʱ�¶�
    //�¶ȷ�Χ��-210(��C)��0(��C) ��΢����ѹ��Χ��-8095(��V)��0(��V)
    //�¶ȷ�Χ��0(��C)��760(��C) ��΢����ѹ��Χ��0(��V)��42929(��V)
    //�¶ȷ�Χ��760(��C)��1200(��C) ��΢����ѹ��Χ��42929(��V)��69553(��V)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // �����ȵ���ʱĬ��Ϊ22��ʱ��
    //[J���¶ȷ�Χ:-270(��C)��1200(��C)]
    if( f_Value < -8095 )        //��������  ΢����ѹΪ��С��λ����
        return ( -273.0 );
    if( f_Value >69553)
        return ( 1200 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
    }

    //�����
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9
    if(f_Value<0)       //�¶ȷ�Χ��-210(��C)��0(��C) ��΢����ѹ��Χ��-5891(��V)��0(��V)
    {
        CoE = &f_CoEA_Jtype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=42929)        //�¶ȷ�Χ��0(��C)��760(��C) ��΢����ѹ��Χ��0(��V)��42929(��V)
    {
        CoE = &f_CoEB_Jtype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=69553)       //�¶ȷ�Χ��500(��C)��1372(��C) ��΢����ѹ��Χ��42929(��V)��69553(��V)
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
    //��΢����ѹת��Ϊ��ʱ�¶�
    //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-8825(��V)��0(��V)
    //�¶ȷ�Χ��0(��C)��1000(��C) ��΢����ѹ��Χ��0(��V)��76373(��V)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // �����ȵ���ʱĬ��Ϊ22��ʱ��
    //[E���¶ȷ�Χ:-270(��C)��1000(��C)]
    if( f_Value < -8825 )        //��������  ΢����ѹΪ��С��λ����
        return ( -273.0 );
    if( f_Value >76373 )
        return ( 1000 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // ������ѹֵ�Ƿ����ڹ涨��Χ��
    }

    //�����
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //�¶ȷ�Χ��-200(��C)��0(��C) ��΢����ѹ��Χ��-5603(��V)��0(��V)
    {
        CoE = &f_CoEA_Etype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //�¶ȷ�Χ��0(��C)��400(��C) ��΢����ѹ��Χ��0(��V)��20872(��V)
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

unsigned char Aux_DataManager::FindNextAuxADCid(void)   //���ص���һ��Ƭѡ��ַ��Ӧ����һ���޷���������Ӧ����ϰ�ߡ�����һ����Ӧ�ó��ָ������������Ǿ�Ӧ�ö�����޷�������

{
    unsigned char ADC_CS_idx;
    unsigned short CheckResult;
    unsigned char temp;
    ADC_CS_idx = OS_ServeProcess::AUX_ADC_Idx;
    CheckResult = CheckAllAuxADCStatus();
    if(Aux_ADC_CS_Info[ADC_CS_idx].m_StateMachine == AUX_Sample_READ)  //��ֹ��û�ж��ϴε����ݶ�����л������Ƭѡ��ַȥ��
        return ADC_CS_idx;
    if(Aux_ADC_TotalCnt==1)
        OS_ServeProcess::AUX_ADC_GOTO_Idx=ADC_CS_idx;
    else
    {
        OS_ServeProcess::AUX_ADC_GOTO_Idx= findNextADC_CS(ADC_CS_idx);

    }
    //     OS_ServeProcess::AUX_ADC_GOTO_Idx=ADC_CS_idx+1;   //���ѡ����������������Ƭѡ��ַ�����Ļ���������κ�һ���������Ӧ����������


    if (CheckResult!=0)  //���������һ��ADC ת�����
    {
        if (!(CheckResult&(1<<ADC_CS_idx))) //�� ���ǵ�ǰCS_Idx���Ǹ�ADCת����ϣ�
        {
            for(unsigned char i=1; i<16; i++)
            {
                if(i<=ADC_CS_idx)
                    temp = ADC_CS_idx-i;  //����Զ���Ǹ�ADC
                else
                    temp = 16+ADC_CS_idx-i;  //��ת
                if (CheckResult&(1<<(temp)))
                {
                    ADC_CS_idx = temp;   //�ҵ�����Զ���Ǹ�ADC��ʵ������
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

unsigned short Aux_DataManager::CheckAllAuxADCStatus(void)   //���������unsigned short�������short�Ļ���ֻ����ȷ���15��ADC
{
    //for(unsigned char idx=0;idx<Aux_DataManager::Aux_ADC_TotalCnt;idx++)
    for(unsigned char idx=0; idx<16; idx++)
    {
        if(Aux_ADC_CS_Info[idx].b_Triggered)
            Aux_ADC_CS_Info[idx].counter++;  //ÿ��ADC Trigger���ͨ����ˢ�¼�ʱ��
        if(Aux_ADC_CS_Info[idx].counter>=Aux_ADC_CS_Info[idx]. m_Wait_Count)
        {
            if(Aux_ADC_CS_Info[idx].m_StateMachine ==AUX_Sample_WAIT)    //״̬���ĸ���������ͳһ����ȽϺ�
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
char  Aux_DataManager::SettingCS_Low(unsigned char m_CS_Address) //�ⲿ�ִ���Ҫ���¿��ǣ���ͬ�İ�Ƭѡʵ�ֵķ�ʽ����ȫһ��
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

char  Aux_DataManager::SettingCS_High(void) //�ⲿ�ִ���Ҫ���¿��ǣ���ͬ�İ�Ƭѡʵ�ֵķ�ʽ����ȫһ��

{
    ControlPin::SetMeHigh(AUX_Unit_Base::Pin_EOE);
}

unsigned char  Aux_DataManager::findNextChn(unsigned char ADCID)
{
    unsigned char chn;
    chn = Aux_ADC_CS_Info[ADCID].m_WorkingChn;
    for(unsigned char i=chn+1; i<Aux_ADC_CS_Info[ADCID].m_ADCCH_MaxCnt; i++)
    {
        if(!Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i])                     //ͨ��������
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    // ��ת��
    for (unsigned char i=0; i<chn; i++)
    {
        if(!Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i])                     //ͨ��������
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
        if(!Aux_ADC_CS_Info[i].m_Valid)                     //ADC������
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
        if(!Aux_ADC_CS_Info[i].m_Valid)                     //ADC������
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
        if(!Aux_DataManager::Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i-1])                     //ͨ��������
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
        if(!Aux_DataManager::Aux_ADC_CS_Info[ADCID].b_ADCCH_Valid[i-1])                     //ͨ��������
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

unsigned char Aux_DataManager::Aux_LocateAuxTypeArrayIndex(unsigned char uc_AuxType)//��������
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
signed char Aux_DataManager::Aux_LocateAuxTypeInArray(unsigned char uc_i)//��������
{
    if(uc_i < MAX_AUX_TYPE_PER_MCU)
    {
        return AuxTypeIndexTable[uc_i];
    }
    return -1;
}
void Aux_DataManager::Aux_ConfirmContainAuxTypes(void)//��������
{
    unsigned char uc_i = 0;
    for (unsigned char j=0; j< MAX_AUX_TYPE; j++)   //ɨ�����еĸ�������
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
