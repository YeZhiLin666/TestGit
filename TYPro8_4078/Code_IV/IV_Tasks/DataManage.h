#pragma once

// #include "../IV_Tasks/ScheduleDataDefine.h"
#include "../IV_Tasks/PID.h"   //04.23.2013
// #include "../../COMMON_OS/Protocols/CAN_Hwf.h"
//#include "../IV_Tasks/IVSDL.h"

typedef struct
{
    unsigned char         bPulseStepEnded;
    unsigned char         bCriticalPoint;    //�Ƿ�Ϊ�ؼ��㣬���ڿ���ѹ����ʱ����ɾ�������ݵ㣩

    float                 fValue;            //�������ֵ
    float                 fValue2;           //����ĸ���ֵ�����fValue�ǵ�����fValue2���ǵ�ѹ����֮����֮��
} St_Pulse_Point;

typedef  struct
{
    unsigned int  m_StageID; //����
    float         m_Current;
    float         m_Voltage;
    bool          m_Valid;    //��ǵ�����ѹֵ�Ƿ��δ������ԭʼ����ת������������������Ŀ�ֵ������limit�жϡ�
    bool          m_Checked;  //��ǵ�����ѹֵ�Ƿ�LIMIT�����������ֹ�ظ������
} ST_PULSE_STAGE_DATA;


typedef struct
{
    bool     m_bIR_from_Schedule;
    bool     m_bIR_from_Relax;
    bool     m_bIR_from_IRStep;
    bool	 m_bIR_from_CCCV_TB;
	bool     m_bIR_from_RisingEdge;
    bool     m_bIR_from_CCCV;
    bool     m_bIR_from_Estimate;
 //   bool     m_bIR_from_Trans; //STEP test IR every 1 hour

}ST_IR_FROM;

typedef struct
{
    bool     m_IR_Confirmed;
	bool     m_IR_ZeroTest_Enable; 		
    bool     m_IR_Stop_Logging;
    
    ST_IR_FROM  IR_From; //Can use to judge whether have an IR value 
    float	 m_fIR_by_IRStep;
    float    m_fIR_by_Schedule;
    float    m_fIR_by_Trans;

//==========08232017 bu ZC==============
    float    m_fIR_by_CCCV_TB;
//======================================
    float    m_fIR_by_CCCV;
    float    m_fIR_by_Estimate;

    MsTime   m_TransStartTime;
    float    m_Trans_Voltage;
    int      m_Trans_Counter;
    bool     m_Trans_Started;
	
	float    m_RisingEdge_Current[10];
//     float    m_RisingEdge_ITransTime[100];
// 	float    m_RisingEdge_IStepIndex[100];
//     float    m_RisingEdge_IStepTime[100];
    float    m_RisingEdge_Voltage[10];
// 	float    m_RisingEdge_VTransTime[100];
// 	float    m_RisingEdge_VStepIndex[100];
//     float    m_RisingEdge_VStepTime[100];
	bool     m_RisingEdge_TransFinished;
    int      m_RisingEdge_Counter;
	bool     m_RisingEdge_VolSample_Started;
    bool     m_RisingEdge_Started;
	float    m_RisingEdge_StepCtrlCurValue;
	float    m_fIR_RisingEdge;	

    MsTime   m_RelaxStartTime;
    float    m_fIR_by_Relax;
    float    m_Relax_Voltage;

    int      m_Relax_Counter;
    bool     m_Relaxed;
    unsigned char m_before_Relax;
}	ST_IR_Info;

typedef struct
{
    float    m_current;
    float    m_voltage;
		bool     m_VolUpdateFlag;
}
ST_IV_Pair;

typedef struct
{
    bool Reflash;
    float Value;
} ST_FORMULA_VALUE;

typedef struct
{
    float High;
    float Low;
} ST_RANGE;

//#define MAX_ARBIN_VALUE 200000
#define POWER_OF(n)     (0x01<<n)

//  1--0  2--1  4--2  8--3  16--4  32--5  64--6  128--7  [2^n--n]   x=log10(y)/log10(2)  ���ö��������㷨 x=log2(y)  x=(log10f(y)/log10f(2));
#define POWER_OF_0_1      (1)
#define POWER_OF_1_2      (2)
#define POWER_OF_2_4      (4)
#define POWER_OF_3_8      (8)
#define POWER_OF_4_16     (16)
#define POWER_OF_5_32     (32)
#define POWER_OF_6_64     (64)
#define POWER_OF_7_128    (128)

#define MAX_BUILDIN_AUXCH                         (8)   // ����ڲ�����ͨ������	;�ݶ�Ϊ8����Ŀǰ��LPC4078�ڲ�ADCͨ�����Ϊ8��
#define MAX_BUILDIN_AUXTYPE	 MAX_BUILDIN_AUXCH  //������ø������಻�ܳ���������ø���ͨ����
#define MAX_AUXCH_PER_AUXMCU (64)
// #define MAX_BUILDIN_TYPE (12)

// #define INTERACANNET 0x01
// #define OUTERCANNET  0x02
// #define IV4AUX       0x04

#define  MAXUNITID   255
// #define SPECIAL_IRTEST  83   //���ڱ�����������IR���Բ�

#define MAXAUXLIMIT   16    //ÿ��Step���16������Limit

#define MAX_ADC_CODE_QUE             32
#define AUX_BUILTIN_MAXCHANNELNO     32
// #define MAX_AUXCH_PER_DATA_GROUP        16

// #define PARAMAX_CTRL_OFFSET    0.001f
// #define PARA_MASK_CTRL_TIME    1000   //100ms
#define USEDSETPARALVALUE      0       //ʹ���趨ֱֵ�����ڼ����ͨ���������

#define MAX_SMBUS_SIGNAL				128

#define DEFAULT_OPERATOR         1 // * 
#define DEFAULT_COEFFICIENT      1 //  1*n

#define ucGetChnVoltRange(ChannelIdx)  (ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange)
typedef struct
{
    unsigned int        m_IVChnIdx;
    CANBMS_METACOUNT    m_Items[MAXCHANNELNO]; //170505 zyx
    //ST_CANBMS_ITEMS    m_Items[MAXCHANNELNO]; //170505 zyx
} ST_Config_Item; // for CAN bms

enum
{
    LIMIT_RIGHT_VALUE,
    LIMIT_LEFT_VALUE = 1,
};//Check Limit Left value or Right value; //zyx 2102251


enum Glb_Safety
{
	Glb_Safety_CURRENT = 1<<0,
	Glb_Safety_VOLTAGE = 1<<1,
	Glb_Safety_POWER = 1<<2,
	Glb_Safety_CAPACITY = 1<<3,
	Glb_Safety_STEPTIME = 1<< 4,
};

class ChannelInfo_Manager
{
private:
    static bool                     b_NeedFilter[MAXCHANNELNO]; //�˲�����
public:
	static unsigned char 			uc_MV_Udx_Glb_Safety[MAXCHANNELNO];
	static unsigned char 			uc_MV_Udx_Glb_RedundantVotlage[MAXCHANNELNO];
	static unsigned char 			uc_MV_Udx_Update[MAXCHANNELNO];
		static void 										SpecialPoint_Init(unsigned char uc_ChannelNo);
    static void                     SetFilterStatus(unsigned char uc_ChannelNo, bool Status);
    static void                     RecoverFilter(unsigned char uc_ChannelNo);
    static bool                     GetFilterStatus(unsigned char uc_ChannelNo);
    static void                     SetChanConfig(unsigned char uc_ChannelNo, St_ChannelCFG *mychancfg);
    static void                     GetChanConfig(unsigned char uc_ChannelNo, St_ChannelCFG *mychancfg);
    static void                     SetChanPID(unsigned char uc_ChannelNo, St_IV_ChannelPID *mychancfg);
    static void                     GetChanPID(unsigned char uc_ChannelNo, St_IV_ChannelPID *mychancfg);
    static int                      LoadTestObject(unsigned char uc_ChannelNo);
    static unsigned char            GetMatchedVoltageRange(unsigned char uc_ChannelNo, float fMaxVoltage);
    static unsigned char            GetMatchedCurrentRange(unsigned char uc_ChannelNo, float fMaxCurrCharge, St_StepCtrl *myctrl);
    static float                    Get_IRangeMaxValue(unsigned char uc_ChannelNo, unsigned char PhysicalCurrentRange);
    static unsigned char            GetPhysicalCurrentRange(unsigned char VirtualCurrentRange);
// 	static unsigned char            GetVirtualCurrentRange(unsigned char PhysicalCurrentRange);
    static void                     InitVirtualCurrentRange(void);
    static void                     Add_Global_Loglimit(unsigned char uc_ChannelNo, St_StepData *mystep, St_ChannelSchedule *mysch);
    static void                     LoadScheduleClamp(unsigned char uc_ChannelNo);
		static unsigned char  AuxSaftyTypeChangeAuxChannelType(unsigned short ChannelIdx,unsigned char uc_AuxSaftyType,St_SaftyCheckItem * MySafty);
public:
    static float		           f_IR_Sourceback[MAXCHANNELNO];
    static float				   f_IR_Valueback[MAXCHANNELNO];
//#if SUPPORT_AUX == 1
    static signed char              CurrRngMapVirtual2Physical[NUM_OF_IV_RANGE_I];
    static signed char              CurrRngMapPhysical2Virtual[NUM_OF_IV_RANGE_I];
    static unsigned char            CurrRngPhysicalRange1;
//     static unsigned char            uc_Support_AUX;
    static unsigned char            AuxLimitCheckOk;
//#endif
    static ST_PULSE_STAGE_DATA	    Pulse_Stage_Data[2][MAX_PULSE_STAGE_NUM];                   //����STAGEͷβ���ݣ�����LIMIT CHECK
    //#if(SUPPORT_CAN_BMS==1)
    static ST_CANBMS_MAP           m_CANMapInfo;
    static ST_Config_Item          m_CANCofigItem;
    static ST_BMS_MetaValue        m_CANMetaVariable[MAXCHANNELNO][MAX_CAN_BMS_SIGNAL];
    //#endif
//========================06.28.2017 TURBO BOOST BY ZC==================================
    static MsTime										CCCV_TB_TimeBuffer[MAXCHANNELNO];
    static int											CCCV_TB_CVCounter[MAXCHANNELNO];
    static bool											CCCV_TB_EnableCounter[MAXCHANNELNO];
    static float										CCCV_TB_CurBuffer[MAXCHANNELNO];
    static int                    	CCCV_TB_LastState[MAXCHANNELNO];
    static float										CCCV_TB_IR_Volt[MAXCHANNELNO];
    static float										CCCV_TB_IR_Curr[MAXCHANNELNO];
    static int											CCCV_TB_IR_Counter[MAXCHANNELNO];
    static unsigned char 						CCCV_TB_LimitCount[MAXCHANNELNO];
//========================06.04.2018 TURBO BOOST BY ZC==================================
    static MsTime										CC_TB_TimeBuffer[MAXCHANNELNO];
    static int											CC_TB_CVCounter[MAXCHANNELNO];
//=======================================================================================
    static bool                     uc_useMetaCode_MV_UD[MAXCHANNELNO];
		static bool 					Volt_Range_Change_Flag[MAXCHANNELNO];
		static MsTime 					Voltage_Range_Change_Time[MAXCHANNELNO];
    static bool                     Is_Current_SoftWareClamp_Work [MAXCHANNELNO];
    static bool                     Is_Power_SoftWareClamp_Work[MAXCHANNELNO];
    static bool                     Is_ClampDigiVolt_Work[MAXCHANNELNO];
    static st_GetMetaValue          MetaVariable2[MAXCHANNELNO][MetaCode_MV_COUNT];
    static St_IV_Buffer1            IV_Buffer1[MAXCHANNELNO];
    static unsigned long            IV_Buffer1_Alpha;
    static unsigned long            IV_Buffer1_Beta;
    static St_ChannelSchedule       ChannelSchedule[MAXCHANNELNO];                                // ͨ��Schedule��Ϣ //
    static St_ChannelStep           ChannelStep[MAXCHANNELNO];                                    // ͨ��Step��Ϣ //
    static St_ChannelInfo           ChannelInfo[MAXCHANNELNO];                                    // ͨ��״̬��Ϣ
    static St_CapaEnerUpdated       CapaEnerUpdated[MAXCHANNELNO];                                // ��־�����������Ƿ�ˢ���ˡ�Ϊ��SDL���ټ�����
    static St_BoardCFG              BoardCFG;                                                     // ͨ��������
    static St_IV_ChannelPID         ChannelPID[MAXCHANNELNO];									 // ͨ��PID����
    static IV_BOARDTYPE_CFG         m_st_BoadType;
    static St_CurFineTune  			m_CurFineTune_Info [MAXCHANNELNO];							//Add by DKQ 06.03.2014
    static unsigned char            m_ChannelIRJudge[MAXCHANNELNO];								 //�洢�����쳣�жϽ��
    //static const unsigned char      FramConfigDictionary[32*1024/8];                              // Fram�洢��Config�ֵ�
// 	static float                    m_TempProtectData[MAXADCHANNELNO];                            //add by qjm20140612
    static St_ScheduleBuffer        ScheduleBuf[IV_MAX_SCHEDULE_BUFFER_NUMBER];                   // Schedule������ //
    static Un_IsampStatus           Isamp_NeedRecalculateSign[MAXCHANNELNO];                      // ����������ر��� ��Ҫ���¼����־
    static Un_VsampStatus           Vsamp_NeedRecalculateSign[MAXCHANNELNO];                      // ��ѹ������ر��� ��Ҫ���¼����־
//	static unsigned char            BuildIn_DI[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];               //161125 zyx
    static unsigned char            BuildIn_DO[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];
    static float					m_RecordMasterChannelPVCurrent[MAXCHANNELNO];

    static ST_FORMULA_VALUE         m_FormulaValue[MAXCHANNELNO][IV_MAX_SCHEDULE_FORMULA_NUMBER];//170515 zyx


    static St_Para_FeedBackCtrl     m_ParaFeedbackCtrl[MAXCHANNELNO];
    static St_UnitInfo 	gUnit_stUnitInfo;
    static St_Serial2BoosterOutInfo    gUnit_stSeries2BoosterOutInfo[MAX_UNIT_PER_GROUP];
    static void InitUnitInfo(unsigned char m_IP);
    //   #endif

    static St_Pulse_Point           PulseLimitArriveData[MAXCHANNELNO];                            //Pulse Limit �����¼���ݣ���ʱ��SDL�ṹ��
    static float RecordRefreshValueForParal[MAXCHANNELNO];										   //��¼������ͨ��ˢ��ֵ�� ֱ�����ڼ�����
    static ST_IR_Info            	 m_IR_Info[MAXCHANNELNO];                   //To record internal resistence of a cell for auto PID calculating	  add by DKQ on 08.03.2016
    static ST_IV_Pair           	  m_IV_Pair[MAXCHANNELNO];                   //To remember current and voltage before tuansistion for IR calcuting  add by DKQ on 08.03.2016
    static ST_Config_Item 				 m_SMBCofigItem;
    static ST_SMB_MetaValue 			 m_SMBusMetaVariable[MAXCHANNELNO][MAX_SMBUS_SIGNAL];
    static float                   m_RC_VoltageCoefficient[MAXCHANNELNO];             //To set RC Filter coefficient
    static float                   m_RC_CurrentCoefficient[MAXCHANNELNO];
    static float 									 m_RC_HistoryVoltageFilterValue[MAXCHANNELNO];  //Rc record history voltage value
    static float									 m_RC_HistoryCurrentFilterValue[MAXCHANNELNO];	//RC record history current value
    static float                   m_RC_OutputValue[MAXCHANNELNO];					//RC output voltage value
    //static float 									 m_RC_OutputCurrentValue[MAXCHANNELNO];					//RC output current value
    static float 									 m_RC_CurrentFilterDeepth[MAXCHANNELNO];
    static float 									 m_RC_VoltageFilterDeepth[MAXCHANNELNO];
    static bool  				 					 m_NeedRCFilterFlg[MAXCHANNELNO];
#if  USE_AS_AUXMCU == 0  //mits8
    static unsigned long m_DataFlag[MAXCHANNELNO]; //zyx 180309
    static void SetDataFlag(UWord16 uw16_ChannelNumber, unsigned long setBit);
    static void ClearDataFlag(UWord16 uw16_ChannelNumber);
    static bool IsSpecialPoint(UWord16 uw16_ChannelNumber, unsigned long PointType);
    static void GetSpecialPointStatus(UWord16 uw16_ChannelNumber);
#endif
//	static unsigned short          m_StepIndexUp[MAXCHANNELNO];
//     static unsigned char		   m_SlaveUnitCounter[MAX_GROUP_PER_UNIT];
    static St_SeriesBoosterMode    m_SeriesBoosterMode;
    static unsigned char           m_Online_UpdataEnable[MAXCHANNELNO];
private:
    static St_PulseBuffer           PulseBuf[IV_MAX_PULSE_BUFFER_NUMBER];                         // Pulse������ //
    static St_HistoryData 		    HistoryData[MAXCHANNELNO];                                    // ͨ����ʷ���� //
// 	static St_HistoryData_ACR     HistoryDataACR;
    static St_Metavariable 		    MetaVariables[MAXCHANNELNO];                                  // ͨ��Metavariable���� //
//	static St_TaskMessages          TaskMessages;                                                 // Messageѭ������
#if DEBUG_TRACE_RECORD == 1
    static St_DebugTraceRecord      DebugTraceRecord;                                             // Debug׷��ģʽ�µ�׷�ٶ�ջ
#endif
//	static unsigned short int      uw16_Setvariable_Counter[MAXCHANNELNO];
//	static unsigned short int      uw16_Setvariable[MAXCHANNELNO][IV_MAX_SCHEDULE_STEP_NUMBER/5];

public:
// 	static unsigned long Read_AcrAfterFilter(void);
    static void Init_HistoryBufQueue(unsigned short uc_ChannelNo);
    static void Init(void);                                                                        //�����������г�Ա��ʼ��
    static void InitChannelInfo(unsigned char uc_ChannelNo);
    //#if SUPPORT_PARALLEL ==1
    //	static void SaveSubgroupData(unsigned char uChanIdx, float fValue);

    //	static void SaveGroupData(unsigned char uChanIdx, float fValue);
// 	static bool  IfGroupStartSafetyCheck(unsigned char groupID);
    static void  ProcessParaSubgroupDataReport(unsigned char uUnidID, float fValue,  unsigned short uType);

    //	static bool IsAllGroupChanEngineHandlerSame(unsigned char groupID,unsigned short uChanIdx, unsigned short uEngineHandlerCase);

    //
    //����ͨ���Լ���ģʽ�ͽ�ɫ��
    // Unit������ͨ����һ���ģ�
    //

	static bool  IsSingleChanMode(unsigned char uChanIdx);
    static bool  Serial2ParaMod_IsSingleChanMode() ;

    static bool  Serial2ParaMod_IsIntraUnitParaMode(unsigned char uChanIdx) ;
    static bool  Serial2ParaMod_IsInterUnitParaMode(unsigned char uChanIdx) ;

    static bool  Serial2ParaMod_IsInterUnitPara_MasterUnit(unsigned char uChanIdx);

    //static bool  IsInterUnitPara_SubmasterUnit(unsigned char uChanIdx);

    static bool  Serial2ParaMod_IsInterUnitMasterChannel(unsigned char uChanIdx)  ;

    static bool  Serial2ParaMod_IsIntraUnitMasterChannel(unsigned char uChanIdx) ;

    static bool Serial2ParaMod_IsMasterChannel(unsigned char uChanIdx);

    static bool  Serial2ParaMod_IsIntraUnitSlaveChannel(unsigned char uChanIdx) ;

    static bool  Serial2ParaMod_IsInterUnitSubmasterChannel(unsigned char uChanIdx) ;
    static bool  Serial2ParaMod_IsInterUnitMasterSubmasterChannel(unsigned char uChanIdx) ;

    static bool  Serial2ParaMod_IsInterUnitMsUnitSlaveChannel(unsigned char uChanIdx) ;

    static bool  Serial2ParaMod_IsInterUnitSubUnitSlaveChannel(unsigned char uChanIdx) ;

    //static bool  IsSlaveChannel(unsigned char uChanIdx) ;

    static bool  Serial2ParaMod_IsInMasterUnit(void);

    static bool  Serial2ParaMod_IsInSubmasterUnit(void) ;
    static bool  Serial2ParaMod_IsBoosterUnit(void);
    static bool  Serial2ParaMod_IsSerialUnit(void);
    static unsigned char  FindMasterChanIdx(unsigned char uChanIdx);

    //static unsigned short  FindMasterUnitIdx(unsigned char uChanIdx);

    static unsigned char  FindGroupLastChanIdx(unsigned char uChanIdx);

// 	static unsigned char  GetUnitNumForInterUnitPara(unsigned char uChanIdx);

// 	static unsigned char  GetMasterUnitIDForInterUnitPara(unsigned char uChanIdx);

// 	static unsigned char  GetLastUnitIDForInterUnitPara(unsigned char uChanIdx);

    static unsigned char  GetTotalChanNumOfParaGroup(unsigned char uChanIdx);

    static void SetCurrentRangeParallelHigh(unsigned char uChanIdx, bool bFlag);

    static bool IsCurrentRangeParallelHigh(unsigned char uChanIdx);

    static void ClearPV_CV_StageTime(unsigned char uChanIdx);

    static void ClearPV_CV_StageCurrent(unsigned char uChanIdx); //20161227 zyx

    //static void ResetGroupData(void);

    //static void ResetSubGroupData(unsigned char groupID);

    //static bool AllChannelSampledOnceForIntraUnitParallel(unsigned short uStartChanIdx, unsigned short uEndChanIdx);

    //static bool AllChannelSampledOnceForInterUnitParallel(void);

// 	static void ClearGroupSampleFlag(void);

// 	static void ClearGroupSampleFlag(unsigned short uFlagType);

// 	static void ClearGroupSampleFlag(unsigned short uFlagType, short uChanIdx);

// 	static void SetGroupSampleFlag(void);

// 	static void SetGroupSampleFlag(unsigned short uFlagType);

// 	static void SetGroupSampleFlag(unsigned short uFlagType, short uChanIdx);

// 	static void SetGroupSampleFlag_Current(unsigned short uFlagType, unsigned short uChanIdx);

// 	static void SetGroupSampleFlag_Voltage(unsigned short uFlagType, unsigned short uChanIdx);

    //static void SetGroupNewSampleFlag(unsigned short uChanIdx, bool bSet);

    //static void ClearAllChanInGroupNewSampleFlag(unsigned short uChanIdx);

    //static bool IfIntraUnitParaGroupHasAnyNewSample(unsigned short uChanIdx);

    //#endif //SUPPORT_PARALLEL
    static void Init_BoardCFG(void);
    static void Init_Metavariable(unsigned char uc_ChannelNo);
    static void InitGroup4SDL(MP_BATCH_GROUP *pCmd);
    static float Read_SimuCycleTime(unsigned char uc_ChannelNo);
    static void  Write_SimuCycleTime(unsigned char uc_ChannelNo, float fTime);
    static float Read_CvCycleTime(unsigned char uc_ChannelNo);
    static void  Write_CvCycleTime(unsigned char uc_ChannelNo, float fTime);

// 	static unsigned short Get_BoardChannelCount(void);
    static void Init_HistoryData(unsigned char uc_ChannelNo);                                      //��ա���ʼ����ʷ����
    static unsigned long Read_CurrentAfterFilter(unsigned char uc_ChannelNo);                     //���˲��������ֵ
    static unsigned long Read_CurrentLatest(unsigned char uc_ChannelNo);                          //���������ĵ�����ֵ
    static unsigned long Read_VoltageAfterFilter(unsigned char uc_ChannelNo);                     //���˲����ѹ��ֵ
    static unsigned long Read_BuildInVoltAfterFilter(unsigned char uc_ChannelNo, unsigned char BuildInIdx);
    static unsigned long Read_VoltageLatest(unsigned char uc_ChannelNo);                          //���������ĵ�ѹ��ֵ
    //    static float Get_NormalizedVoltage(unsigned char uc_ChannelNo);                               //��ȡ ��һ����ѹ
    //    static float Get_NormalizedCurrent(unsigned char uc_ChannelNo);                               //��ȡ ��һ������
    static bool Insert_Current2HistoryData(unsigned char uc_ChannelNo, unsigned long Current, unsigned char Range);    //���������ֵ
    static bool Insert_Voltage2HistoryData(unsigned char uc_ChannelNo, unsigned long Voltage, unsigned char Range);    //�����ѹ��ֵ
    static bool Insert_BuildInVolt2HistoryData(unsigned char uc_ChannelNo, unsigned char BuildInIdx, unsigned long Voltage);
    static float Read_SpecifiedMetavariable2(unsigned char uc_ChannelNo, unsigned char uc_MetaCode);
    // static void Read_SpecifiedMetavariable3(unsigned char uc_ChannelNo,unsigned char uc_MetaCode);
    static void RefreshMetaVariables(unsigned char uc_ChannelNo, unsigned char uc_MetaCode);
    static double Read_DoubleTime(unsigned char uc_ChannelNo,unsigned char uc_MetaCode,MsTime SystemTime);


	static float Calculate_ADVFormula(unsigned char uc_ChannelNo, St_AdvFormula *Formula);
    static float Read_SpecifiedMetavariable4(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, unsigned char type);
    static float GetLimitValue(St_Limit * thisLimit, unsigned char ChannelIdx, bool LeftOrRight);
    static float Calculate_Formula(unsigned char uc_ChannelNo, St_FormulaData *Formula);          //����Formula
    static float Calculate_CANConfig_Formula(unsigned char uc_ChannelNo, unsigned char CANConfigIdex,unsigned char Formula_Idex);
    static float Get_MetaVariable(unsigned char uc_ChannelNo, unsigned char uc_DataType, unsigned char uc_Operator,float fCoefficient,float fMetaCode);
    static void Set_TimeRecalculateSign(void);
    static void Set_CurrentRecalculateSign(unsigned char uc_ChannelNo);
    static void Set_VoltageRecalculateSign(unsigned char uc_ChannelNo);
    static void Set_CCapacityRecalculateSign(unsigned char uc_ChannelNo);
    static void Set_DCapacityRecalculateSign(unsigned char uc_ChannelNo);
    static void Set_CEnergyRecalculateSign(unsigned char uc_ChannelNo);
    static void Set_DEnergyRecalculateSign(unsigned char uc_ChannelNo);

    static void Write_SpecifiedMetavariable2(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, float fValue);
    static void Write_SpecifiedMetavariable3(unsigned char uc_ChannelNo, unsigned char uc_MetaCode, float fValue);

    //	#if SUPPORT_PARALLEL == 1
    static void CopyMasterSpecifiedMetavariable(unsigned short uSourceChan, unsigned short uFromChan, unsigned short uToChan, unsigned char uc_MetaCode);
    //#endif

// 	static void SetTestTime(unsigned short uc_ChannelNo, double dSetValue);
// 	static void SetStepTime(unsigned short uc_ChannelNo, double dSetValue);
    static void SetPV_CV_stagetime(unsigned char uc_ChannelNo);//161019 zyx
    static void GetPV_CV_StageCurrent(unsigned char uc_ChannelNo);//161226 zyx
    static void SetPV_CV_StageCurrent(unsigned char uc_ChannelNo, float TargetI); //161226 zyx
    static void SetPV_CV_StageCurrent(unsigned char uc_ChannelNo);//161226 zyx


// 	static void IncCCapacity(unsigned short uc_ChannelNo, double dSetValue);
// 	static void IncDCapacity(unsigned short uc_ChannelNo, double dSetValue);
//     static void IncCCapacityTime(unsigned short uc_ChannelNo, unsigned short usTimeUs100);
//     static void IncDCapacityTime(unsigned short uc_ChannelNo, unsigned short usTimeUs100);
// 	static void IncCEnergy(unsigned short uc_ChannelNo, double dSetValue);
// 	static void IncDEnergy(unsigned short uc_ChannelNo, double dSetValue);
    static void IncCapacityOrEnergy(unsigned char uc_MetaCode, unsigned short uc_ChannelNo, double dSetValue);
    static void IncCCapacityTimeOrDCapacityTime(unsigned char uc_MetaCode, unsigned short uc_ChannelNo, unsigned long ulTimeUs);

    static void DVmetaInit(unsigned char uc_ChannelNo);      //���ߴ������壬���ͨ���ո� log ��һ����,DV������Ҫ���³�ʼ��
    static void DVmetaClear(unsigned char uc_ChannelNo);                                          //���ߴ������壬���ͨ���ո� �����ˣ�DV������0��ʼ����

    static void Reset(unsigned short ChannelIdx, unsigned char uc_MV_ResetType);
    static void Increment(unsigned short ChannelIdx, unsigned char uc_MV_IncrementType);
    static void Decrement(unsigned short ChannelIdx, unsigned char uc_MV_DecrementType);
    static void ResetNormalMV(unsigned short ChannelIdx, unsigned short us_MV_ResetNormalType);


    static int LoadNextStep(unsigned char uc_ChannelNo, unsigned short u16_GotoStepID);           //������һ����nonactive step��������Ϊ������׼��
    //    static int LoadFormulaData(unsigned char uc_ChannelNo,unsigned short u16_FormulaID);          //����formula��step ������

    //	#if SUPPORT_PARALLEL == 1
    static void SetIntraUnitParallelSlaveStepCtrlType(unsigned char uc_ChannelNo, unsigned char NonActiveStep, UWord16 SlaveCtrlType);
    //	static void SetIntraUnitParallelSlaveChannelCtrlType(unsigned char uc_ChannelNo, unsigned short u_StepID, unsigned short u_CtrlType);
    static void CopyChannelInfoFlagToSlaveChannel(unsigned char uc_ChannelNo);

    //  #endif

    static int InsertSchedule2Buf(St_MD5 ScheduleID, void *pData);
    static bool bCanDeleteSchBuf(unsigned long SchBufs);
    static void RegisterSimu2Schedule(St_MD5 MD5);
    static int InsertPulse2Buf(St_MD5 PulseID, void *pData);

    static unsigned char RegisterSchedule(St_Assign * AssignMsg);
    //    static unsigned char RegisterPulse(unsigned char uc_ChannelNo,St_MD5 MD5);

    static void UnregisterSchedule(unsigned char uc_ChannelNo);
    static void TryDeleteSchedule(unsigned char uc_ChannelNo, St_MD5 MD5);

    //    static int KillSchedules();		//12.12.2013ADD   NOT USED PROGRAM

//     static int CheckScheduleMD5(unsigned char uc_ChannelNo, St_MD5 MD5);
    static float BMS2ClampPower(unsigned char uc_ChannelNo, float f_Current);	//170609 zyx
    static ST_RANGE ClampPower(unsigned char uc_ChannelNo);	//160518
    static ST_RANGE ClampCurrent(unsigned char uc_ChannelNo);	//170719 zyx
    static ST_RANGE ClampDigiVolt(unsigned char uc_ChannelNo, unsigned short u16_ControlType);	//171016 hpq
    static float CurrentCheck(unsigned char uc_ChannelNo, float f_SetCurrent); //170906 zyx
    static float ClampIRIL(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Current);
    static float ClampIRIH(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Current);
    static unsigned char Clamp2AMP(unsigned char uc_ChannelNo, unsigned char uc_Range, float f_Amp);
    //��ADC DACУ׼��ص� public��������:
    static float CalibrateCurrentADC(unsigned char uc_ChannelNo, unsigned char range, unsigned long code);
    static unsigned long CalibrateCurrentDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Current);
    static float CalibrateVoltageADC(unsigned char uc_ChannelNo, unsigned char range, unsigned long code);
    static unsigned long CalibrateVoltageDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage);
	static float ChannelDirection(unsigned char uc_ChannelNo,float Sys_Voltage, float Channel_Voltage);
    static unsigned long CalibrateClampDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage);
    static unsigned long CalibrateLowClampDAC(unsigned char uc_ChannelNo, unsigned char range, float f_Voltage);
// 	static void SaveCalibrateCurrentPara(unsigned short ChannelIdx,unsigned char range);
// 	static void SaveCalibrateVoltagePara(unsigned short ChannelIdx,unsigned char range);
// 	static void SaveCalibrateBuildInVoltagePara(unsigned short ChannelIdx,unsigned char AuxIdx);
// 	static void SaveCalibrateClampVoltagePara(unsigned short ChannelIdx,unsigned char range);
// 	static void SaveCalibrateLowClampVoltagePara(unsigned short ChannelIdx,unsigned char range);
    static void SaveCalibratePara(unsigned char WriteType, unsigned short ChannelIdx, unsigned char range);
    static void SavePidPara(unsigned short ChannelCount, unsigned char * pChannelIdx);
    static void SaveConfigPara(unsigned short ChannelCount, unsigned char * pChannelIdx);
// 	static void SaveCalibrateACRPara(unsigned short ChannelIdx);

    static void GetSystemSafty(unsigned short ChannelIdx, unsigned char SaftyType, unsigned char Range, St_SaftyCheckItem * MySafty);
    static void GetScheduleSafty(unsigned short ChannelIdx, unsigned char SaftyType, St_SaftyCheckItem * MySafty);
    //    static unsigned long long GetTimeTicks(MsTime myTime);
    //    static void GetTimeTicksUpdate(MsTime * myTime);
    //static bool  IfFormulaContain(St_FormulaData *Formula, unsigned short DataType);
	static bool  IfFormulaContain(signed char schBufIdx, signed char formulaIdx,unsigned short DataType,unsigned char ControlValue_DataType);
// 	static bool  IfAdvFormulaContain(signed char schBufIdx, signed char formulaIdx,unsigned short DataType);		
    // #if(SUPPORT_CAN_BMS==1)
// 	static float Read_CANBMSMetaVariable(unsigned char CAN_Chn,CANBMS_METACODE BMS_MetaCode);
// 	static float Read_SMBBMSMetaVariable(unsigned char IV_Chn,unsigned char BMS_MetaCode);
    static  void Set_CANBMSMetaVariable(unsigned char IV_Chn, CANBMS_METACODE MetaCode, float fValue);
    static void Convert_CAN_MetaSignal(unsigned char ChnIdx, ST_CANBMS_CfgUnit *OrigCnfig);
    static void CANBMS_SetMetaLog(unsigned char IV_ChnIdx);
    static void SendCANMessage(unsigned char uc_ChannelNo);
    static void SendCANMessage_Outbound(unsigned char uc_ChannelNo);//190617,hpq
    //    static bool Assign_CANConfig(unsigned char uc_ChannelNo);
    //	   static bool Clear_CANConfig(unsigned char uc_ChannelNo) ;  //Add by DKQ 12.30.2015
    // #endif
/////////////////Smartbattery
    static void Convert_SMB_MetaSignal(unsigned char ChnIdx, ST_SMBBMS_CfgUnit *OrigCfg);
    static void SMB_SetMetaLog(unsigned char IV_ChnIdx);

#if DEBUG_TRACE_RECORD == 1
    static void InitDebugTraceRecord();
    static void PushDebugTraceRecord(St_DebugPoint myDebugPoint);
#endif
    //START 04.23.2013   Functions and Variables Defined for PID Calculation
    //    static void SetIDACValue( unsigned char uc_ChannelNo, unsigned char IRange, unsigned long Value );       //Reserved  for future
    //    static void SetVDACValue( unsigned char uc_ChannelNo, unsigned char VRange, unsigned long Value );   //Reserved  for future
    //    static void SetVDesireValue( unsigned char uc_ChannelNo, unsigned char VRange, float f_Value );             //Reserved for future
    //    static float Calibrate_uwDACToFloat_I( unsigned char uc_ChannelNo,unsigned char IRange, unsigned long uw24_Iset );   //Reserved for future
    //    static float Calibrate_uwDACToFloat_V( unsigned char uc_ChannelNo,unsigned char VRange, unsigned long uw24_Vset );  //Reserved for future
    //    static unsigned long Calibrate_Float_ITouwDAC( unsigned char uc_ChannelNo,unsigned char IRange, float f_Current );    //Reserved for future
    //    static unsigned long Calibrate_Float_VTouwDAC( unsigned char uc_ChannelNo,unsigned char VRange, float f_Voltage ) ;  //Reserved for future
    static float UnifyCurrent( unsigned char uc_ChannelNo,  float fCurrentValue );//unsigned char IRange,
    //static float UnifyVoltage( unsigned char uc_ChannelNo, unsigned char VRange,  float fVoltageValue );
    //    static float UnifyPower( unsigned char uc_ChannelNo, unsigned char IRange,  unsigned char VRange,  float fPowerValue );     //Reserved for future
    //    static float UnifyLoad( unsigned char uc_ChannelNo, unsigned char IRange,  unsigned char VRange,  float fLoadValue );         //Reserved for future
    static float DeUnifyCurrent( unsigned char uc_ChannelNo, float fUniCurrentValue );//unsigned char IRange,
    //    static float DeUnifyVoltage( unsigned char uc_ChannelNo, unsigned char VRange,  float fUniVoltageValue );                       //Reserved for future
    static float GetUnifyCurrent( unsigned char  uc_ChannelNo );//, unsigned char IRange
//     static float GetUnifyVoltage( unsigned char  uc_ChannelNo,  unsigned char VRange  );
// 	static float GetFloatCurrent( unsigned char  uc_ChannelNo);
// 	static float GetFloatVoltage( unsigned char  uc_ChannelNo);
    //    static float Calibrate_uwADCToFloat_I( unsigned char  uc_ChannelNo, unsigned char IRange, unsigned long  uw24_Isample );  //Reserved for future
    //		static float Calibrate_uwADCToFloat_V( unsigned char uc_ChannelNo, unsigned char VRange, unsigned long  uw24_Vsample );    //Reserved for future
    static void PrepareIBasedPID( unsigned char uc_ChannelNo, unsigned char IRange, unsigned char VRange ) ;
// 	static void MarkSampleDataAsFresh( unsigned char uc_ChannelNo );
// 	static void  Init_PidCoef( unsigned char uc_ChannelNo ) ;
    static Ctrl_Pid_Coef         m_Pid_Coef[MAXCHANNELNO];
    static bool	m_DataFreshForPower[MAXCHANNELNO];
    static bool	m_DataFreshForLoad[MAXCHANNELNO];
    static bool  m_DataFreshForVoltagePID[MAXCHANNELNO] ;
    static bool	m_DataFreshForVoltageClamp[MAXCHANNELNO];
    static bool	m_DataFreshForPID[MAXCHANNELNO];


    //END 04.23.2013 Functions and Variables Defined for PID Calculation
    //    static MsTime GetTimeDiff_MsTime(MsTime myTime);
    //    static unsigned long long GetTimeDiff_us100Tick(MsTime myTime);
    static signed char FindScheduleBuf(St_MD5 ScheduleID);

    static int LoadPulseData(unsigned char uc_ChannelNo, unsigned short u16_PulseID);
    static void Init_ChannelStep(unsigned char uc_ChannelNo);
    static void Copy_PVmeta2LSmeta(unsigned char uc_ChannelNo);
    static void Copy_PVmeta2LCmeta(unsigned char uc_ChannelNo);
//     static void Unipolar_Convert(float * f_Set,unsigned char uc_abs);
//     static unsigned long Unipolar_CalibrateCurrentDAC(unsigned char uc_ChannelNo,unsigned char range,float f_Current);
//     static unsigned char Unipolar_Rang(unsigned char uc_ChannelNo,unsigned char range);

    static float RC_FilterOutput(unsigned char uc_ChannelNo, float CurrentValue, float *HistoryValue, float FilterCoefficient, float FilterDeepth);
    static int OnlineUpdata_ScanScheduleBuf(void *pData);
    static void OnlineUpdata_Clear(UWord16 uc_ChannelNo);
    static float fHowLongToNow(MsTime myTime);
    static unsigned char CheckJumpStep_ToProtectCycle(unsigned char uc_ChannelNo, unsigned short u16_FormStepID, unsigned short u16_TargetStepID);

    static float Get_PID_MaxCurrentRange(unsigned char uc_ChannelNo);
	static float CoefficientCalculate(unsigned char uc_Operator,float fCoefficient,float fValue);
	static void Set_PID_MaxCurrentRange(unsigned char uc_ChannelNo);
	
		static float  GetMaxCurrRangeValue(unsigned char  Chn,unsigned char IRange);
		static float  GetCurrRangeForCalculateAccuracy(unsigned char  Chn,unsigned char IRange);
		static float  GetCurrRangeHighOffsetValue(unsigned char  Chn,unsigned char IRange, float OffsetPrecent);
		static float  GetCurrRangeLowOffsetValue(unsigned char  Chn,unsigned char IRange, float OffsetPrecent);
private:
    static double dHowLongToNow(MsTime myTime,MsTime SystemTime);
    //    static double dHowLongToNow(MsTime myTime);
//	static float fHowLongToNow(MsTime myTime);
    static MsTime CalculatePreviousMsTime(double time);
    static void Init_ScheduleBuffer();
    //static int Init_SimulationBuffer();
    static void Init_PulseBuffer();
// 	static int Init_ChannelSchedule(unsigned char uc_ChannelNo);

    //    static void Init_BoardCFG2Default();
    // д�����Ϣ //
    //    static int Copy_PVmeta2LSmeta(unsigned char uc_ChannelNo);
    //static int Copy_PVmeta2LCmeta(unsigned char uc_ChannelNo);
    static int LoadStepData(unsigned char uc_ChannelNo, unsigned short u16_StepID);
    static bool bNeedChangeRange(unsigned short usControlType);

    //static int LoadCvData(unsigned char uc_ChannelNo,unsigned short u16_CvID);
    static float GetPulseMaxAmplitude(unsigned char uc_ChannelNo, unsigned short u16_PulseID);
    static signed char GetEmptyScheduleBuf();
    //    static signed char GetEmptyScheduleBuf2();
    static signed char GetEmptyPulseBuf();
    static signed char FindPulseBuf(St_MD5 ScheduleID);

    static float Function(unsigned char FUNtype, float x);
    static float Operation(unsigned char Operator, float x, float y);
    static signed char OnlineUpdata_CheckStepControlType(UWord16 uw16_ControlType, float f_Value);
    static void OnlineUpdata_Load(St_Sche_Onlineupdata  * st_Onlineupdata, unsigned short uw16_ch, unsigned char uc_Index);//, unsigned char * uc_rangebit//zyx 2105291
    static void OnlineUpdata_AUXLoad(unsigned short uw16_ch);
    static void AddGlobalLoglimit(St_StepData *mystep, int equationIdx, float fLeftValue, float fRightValue, unsigned short GotoStepID, unsigned short LimitID,unsigned char ucDataType);
    //static void Set_PID_MaxCurrentRange(unsigned char uc_ChannelNo);
	static float CalculateEVEN(float x);	//����(��)������(��)���뵽�����ż��
	static float CalculateODD(float x);	//����(��)������(��)���뵽���������
};

typedef enum
{
    BUILTIN_Sample_IDLE,
    BUILTIN_Sample_OPENADG,
    BUILTIN_Sample_WAIT,
    BUILTIN_Sample_TRIGGER,
    BUILTIN_Sample_READ,
    BUILTIN_Sample_WAIT2,
} BUILTIN_Sample_status;

typedef union
{
    unsigned char m_ECODE1: 1;
    unsigned char m_ECODE2: 1;
    unsigned char m_ECODE3: 1;
    unsigned char m_EOE: 1;
    unsigned char m_Reserved: 4;
    unsigned char m_Value;
} UN_AUX_ADC_CS_ADDRESS;

typedef struct
{
    bool m_Valid;                            // ���ڱ�־��Ч��Ƭѡ��ַ
    bool b_ADCCH_Valid[17] ;                 //ÿ��ADC���16��ͨ��������LTC2498 �ڲ���һ���¶�ͨ�����ܹ�17��ͨ��
    bool b_Finished;                         //�ø�ADCоƬ��ͨ���Ѿ�ת�����
    bool b_Triggered;
    bool b_ADG_Exist_Flag;                   //��ģ���ŵ�ADC ��־
    bool b_ADG_Opened;                       //��ģ���ŵı�־
    bool b_WAIT2_FLAG;                        //Trigger ����READ ǰ��Ҫ�ȴ�һ��ʱ��ı�־
    unsigned short m_TickCount_ADG;          //�ȴ�ģ�����л��ȶ�ʱ��
// 	unsigned char  m_BoardType;              //��ס���ͣ��������ݴ�ſ�����Ч
    UN_AUX_ADC_CS_ADDRESS  m_CS_Address;     //Ƭѡ��ַ�ı���
    unsigned short m_Wait_Count;             //��cs��Ӧ�İ���ADC��ת��ʱ��
    unsigned char  m_WorkingChn;             //��CS��Ӧ�İ���ADC�����ڹ�����ͨ��
    unsigned char  sspChn;                   //ssp chn
    unsigned char  m_StateMachine;           //�ڲ�״̬��
    unsigned short  counter;                 //ÿ��ADCоƬ��ǰ�ļ�����
    unsigned char  m_ADCCH_MaxCnt;              //ÿ��ADCоƬʵ��֧�ֵ�ͨ��������
    unsigned long (* m_BoardADC_Trigger)     (unsigned char u_ADCid, unsigned char ADCchn);  //��CS��Ӧ�İ���ADC��trigger����ָ��
    unsigned long (* m_BoardADC_Rd)         (unsigned char u_ADCid, unsigned char ADCchn);  //��CS��Ӧ�İ���ADC��Rd����ָ��
    void (* m_BoardADC_OpenADG)    (unsigned char u_ADCid, unsigned char ADCchn);
    unsigned char m_data_group_idx;        //���ڱ�����Ƭѡ��ӦADC��ADC�Ƕ�Ӧ��data group ����ʼindex
} ST_BUILTIN_ADC_INFO;


typedef struct
{
    ST_ABI_CHN     		m_Chn_Info;  //��ͨ�������Բ���
    st_ADC_Code_Que  	aux_adc_history; //ADC ��ʷ��ֵ
    unsigned long 		aux_instant_code;  //˲ʱADC��ֵ
    unsigned long 		aux_avg_code;  //ƽ��ADC��ֵ

    float 						aux_avg_voltage;	//ƽ����Ĳ�����ѹ����ֵ
    float 						aux_pv_Valtage;  //keep the old one, for the time being
// 	float 						aux_instant_data;
    float 						aux_data;			//ת���������ֵ����
    float 						aux_dx_dt;		//��ʱ���һ�׵���
    float 						aux_d2x_dt2;  //��ʱ��Ķ��׵���
    bool  						updated;			//�����Ƿ����
    MsTime            value_update_time;  //160429

    St_AUX_VALUE   		updated_aux_data;
//    St_AUX_VALUE   		updated_aux_dxdt;
//    St_AUX_VALUE   		updated_aux_d2xdt2;
    unsigned long                m_dataA;
    unsigned long                m_dataB;
    unsigned long                m_udataAfterFilter;
    unsigned char                m_udataCount;
		unsigned char     	aux_data_error_count;
} ST_BuiltIn_DATA;					//���ڴ����ADC��صĸ���������

typedef struct
{
    st_ADC_Code_Que  InChipTemp_adc_history; //ADC ��ʷ�� ֵ
    unsigned long InChipTemp_instant_code;  //˲ʱADC��ֵ
    unsigned long InChipTemp_avg_code;      //ƽ��ADC��ֵ
    float InChipTemp_avg_voltage;	//ƽ����Ĳ�����ѹ����ֵ
    float InChipTemp_data;			//ת���������ֵ����
    bool  updated;			//�����Ƿ����
} ST_INCHIP_TEMP;

typedef struct
{
    unsigned char e_data_group_id;
    unsigned char e_chan_id;
} ST_AUX_P2E_IN_UNIT;

typedef struct
{
    bool   TempExistFlag;
    unsigned char TempCntInOneIV;      //ÿ��IV ͨ���ֵ���BUILTIN �¶� ͨ����
    unsigned char TempDataBank;
    bool   VolExistFlag;
    unsigned char VolCntInOneIV;       //ÿ��IVͨ���ֵ���BuiltIn ��ѹͨ����
    unsigned char VolDataBank;
} ST_BUILTINAUX_Info;

class BuiltIn_DataManager
{
public:
    static ST_BUILTIN_BOARD_INFO_IN_EEPROM    Aux_BuiltIn_BoardInfo;
		static ST_BUILTIN_BOARD_INFO_IN_EEPROM    Write_Aux_BuiltIn_BoardInfo;
    static ST_BUILTIN_ADC_INFO    BuiltIn_ADC_Info[16];
    static ST_BuiltIn_DATA        m_BuiltInDataBank[2][MAX_AUXCH_PER_DATA_GROUP];    //ĿǰBUILTIN ���֧��32ͨ��
    static ST_INCHIP_TEMP         m_InChipTemperature[2];
    static ST_AUX_P2E_IN_UNIT         m_PChn2EChn [2][MAX_AUXCH_PER_DATA_GROUP];
    static bool                   m_DataGroupMgr[2];
    static bool                   m_Sample_Flag;
    static bool                  m_WriteFRAM_Flag;
    static unsigned short         m_uwSamplePauseFlag;       // ���ڼ�¼ADC �Ƿ������ͣ�� һ��Bit ��ʾһ��ADC �� Bit  1����ʾ������ͣ�� 0��������ͣ��
    static bool          b_BuiltInAux;           //�Ƿ���BuiltIn AUX ����
    static bool          b_BuiltInAuxLimitCheckFlag;     // �Ƿ����BUILTIN ������Limit��顣
    static ST_BUILTINAUX_Info    m_BuiltInAuxInfo;

    static unsigned short    BuiltIn_ADC_TimeOut_Flag;
    static unsigned char     BuiltIn_ADC_TotalCnt;
    static unsigned char     BuiltIn_ADC_Idx;
    static unsigned char     BuiltIn_ADC_GOTO_Idx;
    static const St_Port Pin_ECODE1;
    static const St_Port Pin_ECODE2;
    static const St_Port Pin_ECODE3;
    static const St_Port Pin_EOE;
    static unsigned char m_SuspendFlg;
    static unsigned char m_BuiltInCSFlag;

public:
    static unsigned char FindNextBuiltInADCid(void);
    static unsigned short CheckAllBuiltInADCStatus(void);  //add by qjm 20150505
    static char  SettingCS_Low(unsigned char m_CS_Address);
    static void  SettingCS_High(void);
    static char  SampleISR(void);
    static void  BuiltIn_IO_Init(void);
    static void  BuiltIn_HardwareInit(void);
    static unsigned char findNextChn(unsigned char u_ADCid); //add by qjm 20150512
    static unsigned char findNextADC_CS(unsigned char ADCidx);
    static unsigned char  findLastChn(unsigned char ADCID);
};
//-------------PC--------------------------------

// ע�⣬������Ϣ��׼����ӵ�system��configure ���õ�"unit" ҳ�����������ɡ�unit��
// ҳ����ܶ�ն��� ��Ϊ��Щ�ɣ֣�������Ҫ�Ĳ�������unit����Ҫ��ͬ��������������Ҫ�Ĳ���IVunit����Ҫ

//-----------------IV-----------------------------

//////////////////////////////////////////////
// �ο�

// ĳ��������ͨ������ϵͳ������Ķ���
// Cluster��	ָ��ϵͳ��ĳ�ض��������͵�ͨ�����ܺ͡������ȫ��ͨ���š�(Index_InGloble)
// Unit��	���ض���������ͨ�����ڸ�UNIT�ڵ�ȫ����ֹ���(unit ID, Index_InGloble)
// IVChannel��/Schedule��	�������IV Channelʹ�õĸ���ͨ���ģ�IV �ڲ�����ͨ����� ����1 ��ʼ������ȫ�������һһ��Ӧ��ϵ. (Index_InIVCh, Index_InGloble)
// IVMCU��	����IVͨ���ڵĸ���ͨ���߼���ź͸���UNIT�ţ����ڸø���UNIT�ڵĸ�����ͨ������� (Index_InIVCh, UnitID, Index_InUnit)
// AUX�ͣãռ�	�����͸���ͨ�����BID�ţ�����Ӧ��unit��ͨ����ź�����ͨ���ŵĶ�Ӧ��ϵ (BID, Index_InUnit, Index_InBoard)
// ����ͨ���弶	����ͨ���ţ������� (Index_InBoard)
// �����ø���ͨ�� ����ͨ���� 1-8 ��Index_InADC)


//////////////////////////////////////////���������������Ӧ����///////////////////////////////
//��ʱ������ڱ�����봦��

//#if SUPPORT_AUX == 1
typedef struct
{
    unsigned char     m_Source_CANID;
    unsigned char     m_IV_index;
    unsigned char     m_aux_type;
    float		      m_safe_value[2];
    unsigned char     m_safety_alive[2];
} ST_AUX_SAFETY;
typedef struct
{
    unsigned char 	  m_Source_CANID;
    unsigned char     m_IV_index;
    unsigned char     m_aux_type;
    float             m_fLogDeltaRate;
    UWord64           m_log_interval;
    unsigned char     m_sdl_alive;
} ST_AUX_SDL;
typedef struct
{
    unsigned char     m_Source_CANID;
    unsigned char     m_IV_index;
    unsigned char     m_aux_type;
    unsigned char     m_aux_chn_index;
    unsigned char     m_data_type;
    unsigned char     m_single_or_repeat;
    unsigned  char    m_TimeDelta;
    unsigned  char    m_MetaDelta;
} ST_AUX_DATA_REQUEST;

typedef struct
{
    unsigned int     m_BitOut;
    unsigned char    m_ID;
    unsigned char   m_bValid;
} St_perAuxUnitDOInfo;
typedef struct
{
    unsigned int     m_bitDOStatus;
    St_perAuxUnitDOInfo    m_AuxUnitInfo[MAXAUXUNIT];
    //unsigned int     m_bitLastDOStatus;
    unsigned int     m_bitMask;    //this TestSetting's element Num
} ST_AUX_DOCtrl;
/////////////////////////////////////////END
typedef struct
{
    signed long  Set_Value: 15;
    unsigned long kP: 7;
    unsigned long kI: 10;
    unsigned char Aux_AI_ChnNO;  //AI��ͨ����
    unsigned char Aux_AI_ChnType;
    unsigned char Aux_AO_ChnNO;
    unsigned char    m_ID;
    unsigned char m_CANType;
} ST_AO_Data;

typedef struct
{
    unsigned   char        m_ChnCount;
    ST_AO_Data             m_Value[32];
} ST_AUX_AOCtrl;

//#endif

//#if SUPPORT_AUX == 1

class Channel_MapAuxInfo
{
    //  metavariable
public:

    static St_PC2IVUnit4AuxMap      	PC2IVUnit4AuxMap[MAXCHANNELNO];   // IVMCU�ĸ������úͷ�����Ϣ
    static St_BuildInAuxInfo        	BuildInAuxInfo; // �ڲ�����ͨ������ӳ����Ϣ
    static unsigned short             m_Total_In_OneIVCh[MAXCHANNELNO];    // һ��IVͨ���ֵ��ĸ���ͨ��������
    static unsigned short               IVChanMapAuxCount[MAXCHANNELNO][MAXAUXTYPE];   //Schedule used Aux chan counter.

    static ST_AUX_DATA   				m_AuxData[MAXCHANNELNO][MAXAUXTYPE][MAXVIRTUALCHANNEL];   //����ͨ���ش����ݻ�����
    static unsigned char        		m_AuxDataRequested[MAXCHANNELNO][MAXAUXTYPE][MAXVIRTUALCHANNEL];
    static ST_IV_MAPCONFIG         	    m_AUXmap[MAXCHANNELNO][MAXAUXTYPE];                   //���ݲ�ͬ��ʵ����Ҫ���´���Map�������˶���Unit�����Ͳ�����֧��
    static unsigned char    		    CurrentStepAuxSum[MAXCHANNELNO][MAXUNITID];
    static unsigned char                CurrentSafetyCount[MAXCHANNELNO][MAXUNITID];
//	static unsigned char 				m_BackUpAuxUnit[MAXCHANNELNO][MAXAUXTYPE][MAX_CH_IN_AUX_UNIT];  //����Map��ı��ݣ����ڴ��������޸�
    //����ΪIVר���ĸ������������ݻ�����
    static ST_AUX_SAFETY				m_IV4AuxSafetyBank[MAXCHANNELNO][MAXAUXTYPE];     //IVר�������¸�����ȫ�����洢
    static ST_AUX_SDL					m_IV4AuxSDLBank[MAXCHANNELNO][MAXAUXTYPE]; //IVר�������¸���SDl�����洢
    static ST_AUX_DATA_REQUEST 			m_IV4AuxDataRequestBank[MAXCHANNELNO][3 * MAX_AUXCH_PER_AUXMCU]; //���ڵǼ���Щ����ͨ���������ˣ�*3����Ϊ��3�����ݣ�
    static unsigned char 				m_IV4AuxDataRequestCount[MAXCHANNELNO];
    static ST_AUX_DOCtrl         m_AuxDoCtrlMgr[MAXCHANNELNO];
    static ST_AUX_AOCtrl         m_AuxAOCtrlMgr[MAXCHANNELNO];   //add by qjm 2016.0816
    static float                 m_AuxLastStepMetaValue[MAXCHANNELNO][4]; //0:ChargeEnergy 1:Discharge Energy 2:ChargeCapacity 3:DisChargeCapacity
    //������Ӹ���Limit����
    //  Limit

    //  function
public:
    static void 				Init_AUXInfo();
    static void 				SaveAuxMap_ForFRAM(unsigned short uw16_GetIndex);
    static void                 ReadAuxMap_ForFRAM(void);
    static unsigned short       ConvertAuxMap(void);
    static unsigned int 		AuxMapParse(void);   //����PC ���ص� Map��
    static unsigned int         AuxMapBySchedule( unsigned char Channel );
    //    static int 				    Write_MapInfo(unsigned char uc_channelNo,unsigned char Auxtype,unsigned char VirtualID,ST_IVMAP MapInfo);
    static unsigned int  		Locate_AUXChannel(unsigned char uc_channelNo, unsigned char Auxtype, unsigned short VirtualID);
    static  unsigned char 		RecvAuxData(ST_AUX_UPLOADATA UploadData);
    static unsigned char 		AuxLimit_GetComPareSign(unsigned char m_ucCompareSign, unsigned char uc_channelNo, unsigned char LimitIdx);
    static unsigned char     RecvAuxMetaData(unsigned char uc_channelNo, unsigned char MVType, float value);

    static St_AUXchannelInfo    GetAuxChannelInfo(unsigned char uc_channelNo, unsigned char AuxType, unsigned short VirtualIdx);
    //    static signed short 		GetAuxVirtualChannelIdx(unsigned char uc_channelNo,unsigned char AuxCanId,unsigned char AuxType,unsigned short PhysicalIdx);
    static unsigned char        Send_RequestData(unsigned char uc_ChannelNo, unsigned char DataType, unsigned short Aux_VirtualIdx);
    static void                  checkAuxTypeInFormula(unsigned char uc_ChannelNo, signed char schBufIdx, signed char formulaIdx,unsigned char ControlValue_DataType);

    static unsigned char 		Load_AuxStepData(unsigned char uc_ChannelNo, St_Limit AuxLimit);
    //IVר�и���SDL���ֲ���
    //    static void                 RequireAuxData(ST_AUX_DATA_REQUEST st_data_request);
    //    static void                 SetAUXFactor_Time (unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,UWord64 Factor);
    //    static void                 SwitchAUXLog(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned char bLogOn);
    //    static unsigned char        MaxValueCheck(unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
    //    static bool                 Aux_GetData(unsigned char uc_IVUNIT_ID, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn, ST_AUX_DATA* result);
    //    static unsigned char        Aux_AddSDL(ST_AUX_SDL st_sdl);
    static void					Set_AuxOutputCtrl(unsigned char uc_ChannelNo, signed short  u16_TestSettingIdx);
    static void                 Set_AuxOutput_DOCtrl(unsigned char uc_ChannelNo, signed short u16_TestSettingIdx);
    static void                 Set_AuxOutput_AOCtrl(unsigned char uc_ChannelNo, signed short u16_TestSettingIdx);
    static void                 InitAuxMap(unsigned uc_channelNo);
    static unsigned int         WhereAuxTypeMapStart(unsigned char uc_channelNo, unsigned char AuxType);  //return Aux type map start index
    static unsigned int         HowManyAuxTypeMap(unsigned char uc_channelNo, unsigned char AuxType); //return Aux type map count

};

//#endif




