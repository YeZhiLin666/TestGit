#pragma once
#include "../../COMMON_os/Task_Dispatcher/OS_ServeProcess.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../common_OS/protocols/CAN_Hwf.h"
///// IVMCU һ���Ķ���
#define MAX_IVMCU_PER_SYS   MAX_IVUNIT   //�޶�һ��ϵͳ���16��IVMCU
//#define MAX_IVCH_PER_IVMCU  16  //�޶�һ��IVMCU���ֻ��8��IVͨ�����򻯰渨��������ARPRE ����Ϊ16��ʵ��ʹ�õ�15��ͨ��
#define MAX_IVCH_PER_IVMCU  MAXCHANNELNO  //�޶�һ��IVMCU���ֻ��8��IVͨ�����򻯰渨��������ARPRE ����Ϊ16��ʵ��ʹ�õ�15��ͨ��
#define MAX_LIMIT_PER_IVCH   16 //�涨ÿ��IVͨ�������16��limit
#define MAX_IFANY_LIMIT_PER_IVCH	16  //�涨ÿ��IVͨ�����ֻ��16��IFANY limit
#define MAX_DATA_GROUP             9  // �涨���8�������顣һ�����һ���������Ӧ��һ��֧��16ͨ����ADC
#define MAX_AUXCH_PER_DATA_GROUP        16 // 
#define MAX_AUXMCU_PER_IVMCU 6   //�򻯰渨��
//#define AUX_ADC_MAX_CHN       16
////  ����MCUһ���Ķ���
#define MAX_AUX_TYPE     MAXAUXTYPE //�ݶ������16�ָ���ͨ������
//#define MAX_AUX_TYPE_BOARD                      16
//#define MAXAUXTYPE							(8)//(NUM_OF_AUX_CHAN_TYPE)
#define MAX_SAFETY_RANGE					2

//AO���λ�÷ŵ�96���⣬��databank��ŵ�group3��λ�ã�û�г�96.ֻ�Ĵ洢���ֵΪ128.group0��AI,group1��DI��group2��DO��group3�ŵ�AO.
#define MAX_AUXCH_PER_BOARD  128    //17 		//һ�������������16��ͨ�� ����֧���¶�17ͨ��
#define MAX_HUMIDITY_PER_BOARD	4		//һ���������������4��ʪ��ͨ�������ĸ����ڣ�
#define MAX_ADCNUM_PER_BOARD  2   	//һ�����������2��ADC
//#define MAX_AUXCH_PER_AUXMCU  (MAX_AUXCH_PER_BOARD * MAX_BOARD_PER_AUXMCU) //һ������MCU�����128������ͨ�������ǿ�����ͬһ���ͣ�Ҳ�����ǲ�ͬ����
#define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT

#define MAX_FACE_PER_BOARD	 2;   // ��ʱ���и���ʱ���������Ϊ����塣

#define MAX_AUX_CHANNEL_NUM_PER_ADC 16
#define MAX_ADC_CHIPS   16

#define MAX_DIDO_PIN_PER_BOARD       32
#define MAX_ADC_CODE_QUE             32
#define   Ec0_Ttype	0.0f
#define   Ec1_Ttype	3.8748106364e1f
#define   Ec2_Ttype	3.329222788e-2f
#define   Ec3_Ttype	2.0618243404e-4f
#define   Ec4_Ttype	-2.1882256846e-6f
#define   Ec5_Ttype	1.0996880928e-8f
#define   Ec6_Ttype	-3.0815758772e-11f
#define   Ec7_Ttype	4.547913529e-14f
#define   Ec8_Ttype	-2.7512901673e-17f
const float   Ec0_Ktype	= 1.7600413686e1f;
const float   Ec1_Ktype	= 3.8921204975e1f;
const float   Ec2_Ktype	= 1.8558770032e-2f;
const float   Ec3_Ktype	= -9.9457592874e-5f;
const float   Ec4_Ktype	= 3.1840945719e-7f;
const float   Ec5_Ktype	= -5.6072844889e-10f;
const float   Ec6_Ktype	= 5.6075059059e-13f;
const float   Ec7_Ktype	= -3.2020720003e-16f;
const float   Ec8_Ktype	= 9.7151147152e-20f;
const float   Ec9_Ktype	= -1.2104721275e-23f;
const float   Ea0_Ktype	= 1.185976e2f;
const float   Ea1_Ktype	= -1.183432e-4f;

const float   Ec0_Jtype	= 0.0f;
const float   Ec1_Jtype	= 5.038118715e1f;
const float   Ec2_Jtype	= 3.0475836930e-2f;
const float   Ec3_Jtype	= -8.5681065720e-5f;
const float   Ec4_Jtype	= 1.3228195295e-7f;
const float   Ec5_Jtype	= -1.705295837e-10f;
const float   Ec6_Jtype	= 2.0948090697e-13f;
const float   Ec7_Jtype	= -1.2538395336e-16f;
const float   Ec8_Jtype	= 1.5631725697e-20f;

const float   Ec0_Etype	= 0.0f;
const float   Ec1_Etype	= 5.86655087e1f;
const float   Ec2_Etype	= 4.5032275582e-2f;
const float   Ec3_Etype	= 2.8908407212e-5f;
const float   Ec4_Etype	= -3.3056896652e-7f;
const float   Ec5_Etype	= 6.502440327e-10f;
const float   Ec6_Etype	= -1.9197495504e-13f;
const float   Ec7_Etype	= -1.2536600497e-15f;
const float   Ec8_Etype	= 2.1489217569e-18f;
const float   Ec9_Etype	= -1.4388041782e-21f;
const float   Ea0_Etype	= 3.5960899481e-25f;


#define    MAX_AUX_TRIGGROUPLOG_POINT  32  // St_SDL_OnePoint points[32]

enum
{
    NOUSE=0,    //û����
    BEUSED=1,    //ʵ��ʹ��
    VIRTUAL=2,   //����
};
enum
{
    HIGH_COMMON_MODE=1,
    LOW_COMMON_MODE,
    TEMPERATURE_MODE,
    BATTERYCELLS,
    MULTIAUX,
};
enum
{
    SAMPLEIDLE=0,
    SAMPLEISR,
    SAMPLETASK,
};
enum
{
    AUXADC_IDLE,
    AUXADC_NEW_CONVERT=1,
    AUXADC_READ_LAST_CONVERT,
    AUXADC_WAIT,
};



typedef enum
{
    AUX_Sample_IDLE,
    AUX_Sample_TRIGGER,
    AUX_Sample_READ,
    AUX_Sample_WAIT,

} AUX_Sample_status;

typedef enum
{
    AUX_2ndV_LC_Sample_IDLE,
    AUX_2ndV_LC_Sample_SWITCH,
    AUX_2ndV_LC_Sample_WAIT1,
    AUX_2ndV_LC_Sample_TRIGGER,
    AUX_2ndV_LC_Sample_WAIT2,
    AUX_2ndV_LC_Sample_READ,

} AUX_2ndV_LC_Sample_status;

typedef union
{
    unsigned char m_ECODE1:1;
    unsigned char m_ECODE2:1;
    unsigned char m_ECODE3:1;
    unsigned char m_EOE:1;
    unsigned char m_Reserved:4;
    unsigned char m_Value;
} UN_AUX_ADC_CS_ADDRESS;


typedef struct
{
    unsigned char uc_start;
    unsigned char uc_length;
    //unsigned char uc_N;    //ϵ������ʱ�������ɣ���Ϊ���ܹ涨��ÿ��������Ӧ������  ��ʼ��ʼ����Nϵ����Ϊ0
    unsigned long ul_data[MAX_ADC_CODE_QUE];
} st_ADC_Code_Que;

typedef struct
{
    bool m_Valid;                               // ���ڱ�־��Ч��Ƭѡ��ַ
    bool b_ADCCH_Valid[17] ;                 //ÿ��ADC���16��ͨ��������LTC2498 �ڲ���һ���¶�ͨ�����ܹ�17��ͨ��
    bool b_Finished;                         //�ø�ADCоƬ��ͨ���Ѿ�ת�����
    bool b_Triggered;
    unsigned char  m_BoardType;                 //��ס���ͣ��������ݴ�ſ�����Ч
    UN_AUX_ADC_CS_ADDRESS  m_CS_Address;                //Ƭѡ��ַ�ı���
    unsigned short m_Wait_Count;                //��cs��Ӧ�İ���ADC��ת��ʱ��
    unsigned char  m_WorkingChn;                //��CS��Ӧ�İ���ADC�����ڹ�����ͨ��
    unsigned char  sspChn;                       //ssp chn
    unsigned char  m_StateMachine;                 //�ڲ�״̬��
    unsigned short  counter;                 //ÿ��ADCоƬ��ǰ�ļ�����
    unsigned char  m_ADCCH_MaxCnt;              //ÿ��ADCоƬʵ��֧�ֵ�ͨ��������
    unsigned long (* m_BoardADC_Trigger)     (unsigned char u_ADCid,unsigned char ADCchn);   //��CS��Ӧ�İ���ADC��trigger����ָ��
    unsigned long (* m_BoardADC_Rd)        (unsigned char u_ADCid,unsigned char ADCchn);   //��CS��Ӧ�İ���ADC��Rd����ָ��
    unsigned char m_data_group_idx;        //���ڱ�����Ƭѡ��ӦADC��ADC�Ƕ�Ӧ��data group ����ʼindex
    //�൱�ڹ�ȥ�������ĸ��ۣ�������������ˣ��Ա���Ͱ����ֱ�ӹ���
    //������ͨ����ͨ���� ��ID���� group_idx
    // ���ڸ߹�ģ�� group_idx =0, ֻ��һ��group
    // ���� Addon, group_idx=0 ��Ӧ��2449�� group_idx =1 ��Ӧ�� 2498
    //���� ��ذ� group_idx 0 -5 ��Ӧ��1-96��ͨ�� ��ADC1-8����group_idx 6-7 ��Ӧ��97-128 ��ADC 9-11����

} ST_AUX_ADC_CS;


enum TE_CHECK_T_VOLTAGERANG
{
    COUNT_TEMPERATURE = 0,
    CHECK_T_VOLTAGERANG ,
};
//�ڲ�ά��״̬��״̬������μ�����ͼ
typedef enum
{
    // 	AUX_SAMPLE_IDLE_ENUM = 0xC0,
    // 	AUX_SAMPLE_WAITTING_TRIGGER,
    // 	AUX_SAMPLE_WAITTING_SAMPLE,
    // 	AUX_SAMPLE_TRIGGER_NEXT,
    // 	AUX_SAMPLE_WAITTING_TRIGGER_NEXT,

    // 	AUX_SAMPLE_RESERVE1,
    // 	AUX_SAMPLE_RESERVE2,
    AUX_SAMPLE_IDLE_ENUM = 0xC0,
    AUX_SAMPLE_WAITTING_TRIGGER,
    AUX_SAMPLE_WAITTING_SAMPLE,
    AUX_SAMPLE_TRIGGER_NEXT,
    AUX_SAMPLE_WAITTING_TRIGGER_NEXT,
    AUX_SAMPLE_WAITTING_POLL0,    //add by qjm20141128
    AUX_SAMPLE_WAITTING_POLL1,
    AUX_SAMPLE_WAITTING_POLL2,
    AUX_SAMPLE_WAITTING_POLL3,
    AUX_SAMPLE_WAITTING_READ,    //add by qjm20141128
    AUX_SAMPLE_PROCESSING_DATA, //add by qjm20141208

    AUX_SAMPLE_RESERVE1,
    AUX_SAMPLE_RESERVE2,
} AUX_SAMPLE_STATE;


typedef struct
{
    unsigned char BoardID;
    unsigned char ADCID;
    unsigned char ChannelID;
} ST_AUXSAMPLE;
typedef struct
{
    unsigned char m_Unit_ID;
    unsigned char m_Auxtype;
    unsigned char m_AuxChannelFromID;
    unsigned char m_AuxChannelToID;
} ST_AUXSYSTEMCFG; //Systemconfig����

typedef struct
{
    unsigned char SSPNum;
    //ssp �����ṹ
} ST_SSP; //SSP �ṹ����

typedef union
{
    ST_SSP  m_SSP;
    //ST_GPIO m_Gpio;
} UN_DRIVERCFG; //���ø������͵�����

typedef struct
{
    unsigned char m_DriverType;
    UN_DRIVERCFG  m_DriverCfg;
} ST_HARDWARECFG; //��ͬ���Ͱ������

typedef struct
{
    float gain;
    float offset;

    //others  ����������IVӦ��ͬ������
} ST_AUXCANNELCFG;
typedef struct
{
    //����Ϣ����:
    //1��������
    //2��ͨ������
    //3�����ߵ�����(������Ϣ)
    //4��У׼��Ϣ

    unsigned char m_BoardType;
    unsigned char m_ChannelCount;
    ST_HARDWARECFG m_HardWareCfg;
    //ST_AUXCANNELCFG    ����ͨ��У׼��������Ϣ
    //ST_AUXCANNELCFG m_Channel[AUX_ADC_MAX_CHN];    //У׼��Ϣ��  20131202

} ST_AUXBOARDCFG; //�����弶������Ϣ ������EEPROM��




typedef struct
{
    unsigned char  		m_valid;   //�ð��Ƿ���Ч���
    ST_ABI_ASSEMBLE    m_assemble;  //���ڻ����е������Ϣ
    ST_ABI_GLOBLE 			m_abi_globe;   //ȫ����Ϣ
    unsigned short     m_RSN;     //����AUXMC�е�ע����Ϣ
    unsigned char m_Start_InUnit; // Add Nov 25, 2013  ���ڴ洢�����ð���ͨ���ڸ���unit�ڵ�ͨ�����
    unsigned char m_End_InUnit;		//Add Nov. 25,2013

} ST_AUX_BOARD_INFO_SAVE;    //18�ֽ�


typedef struct
{
    unsigned char     m_ActualCounts;  //�ø��������ڸ�IVͨ���µĸ���ʵ��ͨ����
	unsigned char	  m_Counts_ByScheduleMap;	//�ø���������IVͨ�� schedule����ʵ�ʵ�MAP����
    St_MAP_AuxCh2IVCh m_MapInfo[MAX_AUXCH_PER_AUXMCU];
} ST_MAP_FROM_PC;  //���ڴ洢��PC�������й�ĳIVͨ���ĸ��ศ��ͨ����������Ϣ

typedef struct
{   unsigned char m_valid;  //���ڱ�Ǵ˸���ͨ���Ƿ���ı��ù�
    unsigned char m_IVUnit; //unit ID
    unsigned char m_IVCh;
    unsigned char m_AuxType;
    unsigned char m_Index_InIVCh;       // �ø������͵�ͨ���ڸ�IVͨ������������
    unsigned char m_Index_InAuxUnit;    // �ø����������丨��unit��ͨ����
} ST_DOWNLOAD_MAP_FROM_PC;



typedef struct
{
    bool            m_bValid;
    bool		        m_bOut;
    unsigned char   m_BID;
    unsigned char   m_CID;
} ST_Aux_oneDOCtrl;
typedef struct
{
    unsigned char 	 m_DoCount;
    ST_Aux_oneDOCtrl m_DOValue[MAX_DIDO_PIN_PER_BOARD];
} ST_AUX_DOCtrl;








typedef struct
{   unsigned char  		m_valid;   //�ð��Ƿ���Ч���
    ST_ABI_GLOBLE 			m_abi_globe;   //ȫ����Ϣ
    ST_ABI_CHN 				m_abi_chn[MAX_CH_IN_AUX_UNIT];  //���128��,���ǵ�Addon��һ������32������ذ���96��
    ST_ABI_CHN         m_abi_uart_chn [8]; // �ݶ����8������ͨ����ע����ָ��ֵͨ��������ָ8������UART��
    ST_ABI_CHN         m_abi_DAC_chn[8];  //�ݶ����8��DACͨ��
    ST_ABI_ASSEMBLE  	m_assemble;  //���ڻ����е������Ϣ
    unsigned short     m_RSN;     //����AUXMC�е�ע����Ϣ
}
ST_AUX_BOARD_INFO_IN_EEPROM;

typedef struct
{

    ST_AUX_BOARD_INFO_IN_EEPROM   m_BoardInfo;
    unsigned char    m_data_group_start; //�ð�Ķ�Ӧ����ʼ data_group_index;
    unsigned char    m_TotalADC;
    unsigned char    m_Start_InUnit; // Add Nov 25, 2013  ���ڴ洢�����ð���ͨ���ڸ���unit�ڵ�ͨ�����
    unsigned char    m_End_InUnit;		//Add Nov. 25,2013
    //һ����Ϊ�˱�����ǰ�Ľṹ��ʹ�����ܵ������Ժ�ɾ��
    //ST_AUX_BOARD_INFO_SAVE   m_BoardInfo;
    Un_AUXCHINFO  m_AuxSensor_ChInfo[MAX_CH_IN_AUX_UNIT];   //02. 24.2014
} ST_AUX_BOARD_INFO;


typedef struct
{
    unsigned char m_StartInSnake;      // �ڳ����е���ʼλ��
    unsigned char m_TotalChnNum;      //ĳ���͸���ͨ���������� �������128

} ST_AUX_IN_UNIT_LOOKUP_TABLE;

typedef struct
{   unsigned char  m_valid;
    unsigned char  m_data_group_idx;   //
    unsigned char  m_chn;   //Electric Channel In  ����ͨ�����
} ST_AUX_CHN_LOCATION;   //����ͨ��������bank�еĵ�λ�ýṹ


typedef struct
{
    unsigned char    m_LimitCheck:1;
    unsigned char    m_ReportValue:1;
    unsigned char    m_Log:1;
    //you can add other here!
    unsigned char    m_Updata:1;
    unsigned char    RVD:4;
} St_AUX_VALUE_BITS;

typedef union
{
    unsigned char      m_Char;    //�������񣬿��������︳ֵ0xFF;
    St_AUX_VALUE_BITS  m_Bits;
} St_AUX_VALUE;

typedef struct
{
    float f_gain;
    float f_offset;
    float f_extra;  // Add by DKQ 03.26.2014

    //
} ST_AUX_CONFIG_FOR_SAMPLE;   //chen add 20131213




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
    unsigned char     aux_data_error_count;
} ST_AUX_DATA;					//���ڴ����ADC��صĸ���������

typedef struct
{
    bool 							m_valid;
    TE_AUX_CHAN_TYPE   m_AuxType;
    unsigned char   aux_data;  //״ֵ̬
    bool  updated;			//�����Ƿ����

} ST_AUX_DIDO_DATA; //���ڴ洢��ADC�޹صĸ��������ݣ�����digital inupt digital output��


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
    WORD          m_limit_SN;  //limit ���к�
    unsigned char m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char m_IV_index;  //IVͨ����
    unsigned char m_aux_type;		//��������
    unsigned char m_chn_index;  //ͬһ�������͵�ͨ����
    unsigned char m_left_type;  //0=ԭʼ���ݣ�1=һ�׵����� 2= ���׵���
    unsigned char m_ComparsionSign; //0:>; 1<; 2 >=; 3<=  �ο�IV����
    float 		  m_right_value;
    unsigned char m_limit_Reached;
    unsigned char m_limit_alive;     //��Ǹ�limit�Ƿ��ѱ����ʧЧ
    unsigned char m_FirstLimit;
} ST_AUX_LIMIT;

typedef struct {
    unsigned char chn_1:1;
    unsigned char chn_2:1;
    unsigned char chn_3:1;
    unsigned char chn_4:1;
    unsigned char chn_5:1;
    unsigned char chn_6:1;
    unsigned char chn_7:1;
    unsigned char chn_8:1;
} ST_CHN2BIT_MAP;

typedef struct
{
    WORD 	        m_limit_SN;  					//limit ���к�
    unsigned char m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char m_IV_index;  	//IVͨ����
    unsigned char m_aux_type;		//��������
    unsigned char m_left_type;  //0=ԭʼ���ݣ�1=һ�׵����� 2= ���׵���
    unsigned char m_ComparsionSign; //0:>; 1<; 2 >=; 3<=  �ο�IV����
    float 				m_right_value;
    unsigned long long m_limit_Reached[2];  // �ù�128��bit��ÿ��bit����¼ÿ��ͨ����limit_Reached�ı仯�����
    unsigned char m_bReached;
    unsigned char m_limit_alive;     //��Ǹ�limit�Ƿ��ѱ����ʧЧ
    unsigned char m_FirstLimit;
} ST_AUX_IFANY_LIMIT;

//��ȫֵ���趨���������з��䵽��IVͨ���µ�����ͬ�͸���ͨ��
typedef struct
{
    unsigned char m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char m_IV_index;  	//IVͨ����
    unsigned char m_aux_type;		//��������
    float					m_safe_value[8];    //��ȫֵ  0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
    // unsigned long long m_beyond_SafeRange[2];  // �ù�128��bit��ÿ��bit����¼ÿ������ͨ���ĳ�Խ��ȫ��Χ�ı仯�����
    // unsigned char m_bUnsafe;
    unsigned char m_safety_alive[8];     //��Ǹ�safety�Ƿ��ѱ����ʧЧ;0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
    //unsigned char m_Start_Chn_InUnit;			//�ø�����������unit�е���ʼͨ����
    //unsigned char m_End_Chn_InUnit;
} ST_AUX_SAFETY;

typedef struct
{
    unsigned char m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char m_IV_index;  	//IVͨ����
    unsigned char m_aux_type;		//��������
    unsigned char m_High_or_Low; //�߶��趨���ǵͶ��趨// �涨 =0 �ǵͶ��趨�� =1 �Ǹ߶��趨//�������ܴ�CAN������ֵ�Ա���
    float					m_safe_value;    //��ȫֵ  0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
} ST_AUX_HALF_SAFETY;  //����ṹ��Ҫ���ڴ�CANParse��safety��Aux_DataManager

typedef struct
{
    unsigned char 		m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char     m_IV_index;         //ΪIVͨ����
    unsigned char     m_aux_type;		  //��������
    float             m_fLogDeltaRate;      //ΪPC��¼��������ֵ����
    UWord64           m_log_interval;    //ΪPC��¼���ݵļ��ֵ
    unsigned char     m_sdl_alive;       //ΪPC��Ǹ�sdl�Ƿ��ѱ����ʧЧ
} ST_AUX_SDL;

typedef struct
{
    unsigned char m_Source_CANID;  //Դ CANID�Ա�ظ�ʱ��
    unsigned char m_IV_index;  	//IVͨ����
    unsigned char m_aux_type;		//��������
    unsigned char m_aux_chn_index; //����ͨ����
    unsigned char m_data_type; //�������ͣ�ԭʼ����0�� һ��ʱ�䵼�� 1�� ����ʱ�䵼��2
    unsigned char m_single_or_repeat; //ֻҪһ���ػ����ظ�������ֻҪһ��Ҫȷ�Ϸ��ͳɹ����ظ������Ĳ���Ҫȷ��
    unsigned  char    m_TimeDelta;          //Ԥ��    �������ö���ȼ�10ms 100ms 1000ms�ȣ����Բ����˷���ô��bit��
    unsigned  char    m_MetaDelta;        //Ԥ��������delta ��������
} ST_AUX_DATA_REQUEST;

typedef struct
{
    unsigned char  ChnNo;
    unsigned char  Auxtype ;
    unsigned short  AuxChn;
    unsigned char  DestID;
    unsigned char  DataType;
    unsigned char  AckStatus;
    unsigned char  ErrCode;
    float          f_Value;
} ST_AUX_DATAPOINT;
typedef struct
{
    unsigned char m_IV_unitID;
    unsigned char m_AuxChnIdx;
    unsigned char m_AuxType;
    unsigned char m_AuxSensor;
    unsigned char m_IVChnIdx;
    unsigned char m_RptDest;  //Ԥ��
    MsTime    TrigTime;
} ST_AUX_DATA_TRIG;

typedef struct
{
    unsigned char  m_ID; //���ڱ�����Щ�õ��˱�����IV Uunit ��unit ID����û�õ�����������ѯ��ʱ������
    unsigned char  m_Valid; //�������MAP�������ЩIVMCU��Ч��ֻ��MAP���ĵ�Ԫ����Ч
} ST_IVMCUID_CFG;

typedef struct
{
    bool            m_bValid;
    //bool		      m_bOut;
    unsigned char   m_BID;
    unsigned char   m_CID;
    Aux_Ctrl_Value  m_Out;
} ST_Aux_oneCtrl;


#if DEBUG_TRACE_RECORD == 1
typedef struct
{
    unsigned short   m_u16RecordType;
    MsTime           m_RecordTime;

    unsigned long    m_u32Parameter1;
    unsigned long    m_u32Parameter2;
    unsigned long    m_u32Parameter3;
    unsigned long    m_u32Parameter4;

    float            m_fParameter1;
    float            m_fParameter2;
} St_DebugPoint;
typedef CIRCULAR_QUEUE(St_DebugPoint,DEBUG_TRACE_RECORD_DEPTH) St_DebugTraceRecord;
#endif
typedef struct
{
    unsigned  char  AUXTYPE ;
    unsigned  char AUX_SUBTYPE;
    unsigned  char Index_for_SameAuxtype;
} ST_ADC_CHANNEL;

typedef struct                   //�������ýṹ�������ܲ����
{
    unsigned char m_AdcType;    //ADC����
    unsigned char m_AdcSPIType; //ADC�õ�SPI/I2C����
    unsigned char m_AdcChnNum;//ADC֧�ֵĵ���ͨ����
    SSPI_Para     m_SSPCfg;   //����ͨ��ִ��SSP���ò���
    My_Func_Ptr  m_ADCSampleFunc; //��ͨ����ʹ�õĲ���оƬ��Ӧ�Ĳ�����������
    unsigned char BeUsed;
    ST_ADC_CHANNEL  m_adc_channel [MAX_AUX_CHANNEL_NUM_PER_ADC];  // MAX_AUX_CHANNEL_NUM_PER_ADCÿƬADC֧�ֵ����ͨ������Ŀǰ�ݶ�Ϊ16
} ST_ADCCFG;

typedef struct               //��Ϊ�п�������������͵İ壬���Բ�����Union
{
    ST_ADCCFG			m_AdcCfg[MAX_ADC_CHIPS];                //�������ýṹ
    //ST_DICFG		m_DICfg[MAXDICHANNEL];
    //ST_DOCFG		m_DOCfg[MAXDOCHANNEL];

} ST_SINGLEBOARDCFG;          //����������Ϣ�ṹ

typedef struct
{
    unsigned char BoardTYPE;     //������
    unsigned char BoardID;		  //����
    unsigned char BeUsed;       //�Ƿ�ʹ��
    unsigned char m_ChannelCount; //��Чͨ������
    unsigned char ChannelADC[16]; //����ÿ��ͨ����Ӧ��ADC ID��
    ST_SINGLEBOARDCFG          m_SingleBoardCFG;  //����������Ϣ
} ST_AUX_BOARDCFG;             //MCU֧�ֵĸ���������Ϣ

//�����MCU��Aux channel info�ṹ
typedef struct
{
    unsigned long AdcCode;
    unsigned char NewAdcCode;

    float PV;
} st_Aux_Chn_Info;

////////////����Ϊrenew
typedef struct
{
    bool exist;    //�Ƿ���ʵ����  ���ô��ڵ�
    bool inUsed;     //�Ƿ���Ӧ����  �������sch�е�
    bool fastChn;    //�Ƿ����ͨ��
    uint8 counter;    //�����������Ӧ�õĲ���

} st_Aux_SampleSetting;

typedef struct
{
    st_Aux_SampleSetting sampleSetting[17];    //17 �����¶�����ͨ��
    unsigned char sspChn;    //ssp chn
    unsigned char csIndex;                      //cs�ڣ�����cs����

    unsigned char ADCid;    //if 99 no used
    //	unsigned char type;       //Ԥ����ͬʱ���¶�������K��J�����ж�
    unsigned char type[MAX_CH_IN_AUX_UNIT];       //ÿ��ͨ����K������J ͬʱ���¶�������K��J�����ж�  03.19.2014

    unsigned char ADC_chn;          //��ǰtrigger��ͨ��
    unsigned char stateMachine;    //�ڲ�״̬��
} st_Aux_Sample;


typedef struct
{
    unsigned char   m_MapDIIdx;
    unsigned char   m_MapDOIdx;
    const St_Port * m_pSt_DIPort;
    const St_Port * m_pSt_DOPort;
} St_DIDOMapMgr;

typedef struct
{
    unsigned char   m_MapDOIdx;
    const St_Port * m_pSt_DOPort;
} St_DOMapMgr;

typedef struct
{
    unsigned char e_data_group_id;
    unsigned char e_chan_id;
} ST_AUX_P2E_IN_UNIT;	// Yifei2

