/*****************************************************************************
// �����ļ�      :Mtci_ctrl.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

/******************************************************************************/
//�¿���ÿ�θ��µ�ֵ������������������У�
//static  short int gR_PresentTempValue[MAX_UART_NUM];
//static  short int gR_PresentHumiValue[MAX_UART_NUM];
//�¸������ʼ��ʱ���ɽ����ϸ���������ַָ���Ӧ��DataBankλ�ã�ע���������: short int -> float,ת��ʱ��Ҫ����10��
// By Yifei, 2015.7.20
/******************************************************************************/

#ifndef MTCI_CTRL_H
#define MTCI_CTRL_H 1


#include "../../COMMON_os/LPC4078CORE/UART.h"


//#define PUBLIC_REPEAT_NUM	            (1) //(5) // (20)   // ������󹫹��ظ�����
//#define CMD_INTERVALTIME	            (1000) //(50) //(25) //(1000) //(200)//(1000) // ����������ʱ�� 1��һ��


//   <>PUBLIC REPEAT NUM   <1=>1 <2=>2 <5=>5 <10=>10
//   <> REPEAT  1/2/5/10
//   <> Default:    1    // ÿ�η��������ظ�����
#define PUBLIC_REPEAT_NUM	      (1)    // ������󹫹��ظ�����

//   <o>CMD INTERVALTIME   <500=>50 <1000=>100 <2000=>200 <10000=>1000 <50000=>5000
//   <i> INTERVALTIME  50/100/200/1000/5000
//   <i> Default:    1000    // ÿ�η���������ʱ��1��
#define CMD_INTERVALTIME	     (200*10)   // ÿ�η���������ʱ��200���� 

#define FIRST_UART_NUM            1  // 1   ���ڰ��һ����ʼ�Ĵ��ڱ��

#define MAX_CHAMBER_NUM         8 //�������ڿ��Ƶ��¿�������

#define FIRST_CHAMBER_CHANNEL		0
#define SECOND_CHAMBER_CHANNEL		1
#define THIRD_CHAMBER_CHANNEL 2

// ���ڴ��������жϵ��޶�ֵ
//#define PUBLIC_REPEAT_NUM	            (1) // (20)   // ������󹫹��ظ�����
/*#define MAX_PREWR_CMD_REPEAT_NUM	    (PUBLIC_REPEAT_NUM)
#define MAX_WRITE_CMD_REPEAT_NUM	    (PUBLIC_REPEAT_NUM)
#define MAX_RUN_CMD_REPEAT_NUM		    (PUBLIC_REPEAT_NUM)
#define MAX_STOP_CMD_REPEAT_NUM		    (PUBLIC_REPEAT_NUM)
#define MAX_READ_CMD_REPEAT_NUM	        (PUBLIC_REPEAT_NUM)*/

//  ��Ӧ����������
#define MIN_RESPONSE_CMD_NUM        3 // 3֡   // ��Ӧ����С��������
#define MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define MIN_RESPONSE_CMD_LEN		(MIN_RESPONSE_CMD_NUM*MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define MAX_RESPONSE_CMD_LEN		(MAX_RESPONSE_CMD_NUM*MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����


//#define SEND_MAX_BUF      256    // 1 byte slave address + 253 byte of PDU + 2 CRC
//#define SEND_MAX_BUF      (2*MAX_COM_WRITE_CMD)    // ����ݴ滺����
#define SEND_MAX_BUF        300


#define ASCII_FIRST_UART_NUM         (FIRST_UART_NUM + 0X30)  // 1   ���ڰ��һ����ʼ�Ĵ��ڱ��


// ��һʹ�������շ��궨��
enum TE_COM_CMD
{
    COM_NONE_CMD = 0,	         // NONE CMD
    SEND_COM_MODE_CMD,	         // SEND MODE CMD
    SEND_COM_WRITE_CMD,	         // SEND WRITE CMD
    SEND_COM_READ_CMD,	         // SEND READ CMD
    SEND_COM_RUN_CMD,	         // SEND RUN CMD
    SEND_COM_STOP_CMD,	         // SEND STOP CMD
    RECEIVE_COM_MODE_CMD,	     // RECEIVE MODE CMD
    RECEIVE_COM_WRITE_CMD,	     // RECEIVE WRITE CMD
    RECEIVE_COM_READ_CMD,	     // RECEIVE READ CMD
    RECEIVE_COM_RUN_CMD,         // RECEIVE RUN CMD
    RECEIVE_COM_STOP_CMD,	     // RECEIVE STOP CMD
    MAX_COM_CMD_TOTAL,           // MAX TOTAL
};


#define READ_TIMEOVER_RATE           (120*10000) // (CMD_INTERVALTIME/1000) // ��ȡ���ݳ�ʱϵ������׼��1����һ��;2���ӡ� zyx 181019


// uc_Temp4Humi_Flag ��ֵ����ֵ��־�궨��  λ�����
#define SETPOINTVALUE_NONE           (0X00)
#define SETPOINTVALUE_TEMPERATURE    (0X01)
#define SETPOINTVALUE_HUMI           (0X02)
#define SETPOINTVALUE_MAX            (0X03)

//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
#define COM_RUN4READ_CMD_DIFFER	     (1) // ͣ��д���������һ��ʱ,������Open this define for Difference.   
#define COM_TEMP4HUMI_CMD	         (1) // �ȿ����¶��ֿ���ʪ��ʱ��Open this define temperature and humi.  
#define COM_CMD_CHANNEL_DIFFER       (1) // ʹ�ö����ͬͨ����ʱ������ʾ1��4������Ĭ��Ϊ1��ͨ����Open this define multi channel.  



//-------------------------------------------------------------
// MCU SPI  FM25L256  SSP1��������FM25L256 оƬ�洢����EEPROM    256K bit=32K Byte   ��ַ: 0X0000~0X7D00
// ID	        Name	             Bytes	      Comment            Address
// 1	Global Data Area	          64	      0 ~ 63           0X0000~0X003F
// 2	IV Channel Setting Area	      5056	      64 ~ 5119        0X0040~0X13FF
// 3	Aux Channel Setting Area	  3072	      5120 ~ 8191      0X1400~0X1FFF
// 4	Aux Channel Special Area	  64	      8192 ~ 8256	   0X2000~0X203F
// 5	TempChamber Channel Area      32	      8257 ~ 8289	   0X2040~0X205F
// 6	TempChamber PID Channel Area  320	      8290 ~ 8610	   0X2060~0X219F
// 6	TempChamber UART Channel Area  40	      8704 ~ 8744	   0X2200~0X2228

#define EEP_GLOBALBASE		        0x00           //  ȫ�ִ洢����ʼ��ַ
#define EEP_GBLARBINIDBASE	    0x00
#define EEP_GBLPCANOBASE	        0x04
#define EEP_PCIPBASE               0x10           //дԶ��PC�����ַ�Ŀ�ʼ��ַ
#define EEP_MCUPORTBASE           0x14           //д����MCU�˿ںŵ�ַ�Ŀ�ʼ��ַ   

#define EEP_IVCHBASE		        0x40           //  IVͨ����Ϣ�洢����ʼ��ַ
#define EEP_IVCHCOUNTBASE	        0x0E
#define EEP_IVCHIRNGCNTBASE	    0x1A
#define EEP_IVCHVRNGCNTBASE	    0x1B
#define EEP_IVCHIRNGBASE	        0x1C
#define EEP_IVCHVRNGBASE	        0x80
#define EEP_IVCHCLAMPBASE	        0xCB
#define EEP_IVCHCALI_R_BASE	    0x08
#define EEP_IVCHCALI_S_BASE	    0x10
#define EEP_IVCH_DTFBASE	        0x18

#define EEP_AUXCHBASE_FMSPI		0x1400         //  ����ͨ����Ϣ�洢����ʼ��ַ
#define EEP_AUXCHCOUNTBASE_FMSPI	0x0F
#define EEP_AUXRNGBASE_FMSPI	    0x0C
#define EEP_AUXCALI_R_BASE_FMSPI	0x0
#define EEP_AUXCALI_S_BASE_FMSPI	0x0C

#define EEP_AUXSPECIALBASE_FMSPI	0x2000        //����ͨ��������Ϣ���濪ʼ��ַ 
#define EEP_ELE_TCHAMBER_CHO_FMSPI	    0x2040        //�������¿���ѡ��������Ϣ���濪ʼ��ַ 
#define EEP_ELE_TCHAMBER_PID_FMSPI		0x2060        //�������¿���PIDͨ��������Ϣ���濪ʼ��ַ 
#define EEP_CHAMBER_UART_FMSPI		    0x2200        //�¿���UARTͨ��������Ϣ���濪ʼ��ַ 

#define EEP_PCIPLENGTH             0x04          //дԶ��PC�����ַ����
#define EEP_MCUPORTLENGTH         0x02          //д����MCU�˿ںŵ�ַ���� 
#define EEP_GLOBALLENGTH	        0x10
#define EEP_IVCHLENGTH		        0xFF
#define EEP_IVCHBASICLENGTH	    0x1A
#define EEP_IVCHRNGLENGTH	        0x19
#define EEP_IVCHCALILENGTH	    0x08
#define EEP_IVCHOTHERBASE	        0xE4
#define EEP_IVCHOTHERLENGTH	    0x0A
#define EEP_AUXCHLENGTH_FMSPI		0x2C
#define EEP_AUXRNGLENTH_FMSPI		0x18
#define EEP_AUXCALILENGTH_FMSPI	0x0C
#define EEP_AUXSPECIALLENGTH_FMSPI	0x04        //����ͨ��������Ϣ�����ַ���� 
#define EEP_ELE_TCHAMBER_CHO_LENGTH	0X05       //�������¿���ѡ��������Ϣ�����ַ���� 
#define EEP_ELE_TCHAMBER_PID_LENGTH	0x21        //�������¿���PIDͨ��������Ϣ�����ַ���� 
#define EEP_TCHAMBER_UART_LENGTH	0x0A        //�¿���UARTͨ��������Ϣ�����ַ���� 

// MCU SPI  FM25L256  SSP1��������FM25L256 оƬ�洢����EEPROM   256K bit=32K Byte   ��ַ: 0X0000~0X7D00
//----------------------------------



// ���¶���Ϊuc_Aux_ChooseTaskPointer��״̬��
/*#define AUX_CHOOSETASK_IDLEDATA                   0
#define AUX_CHOOSETASK_SPECIALFUN_COM_WRITE       1
#define AUX_CHOOSETASK_SPECIALFUN_COM_READ        2
#define AUX_CHOOSETASK_SPECIALFUN_COM_RESULT      3*/

enum TE_AUX_CHOOSETASK
{
    AUX_CHOOSETASK_IDLEDATA =0,
    AUX_CHOOSETASK_SPECIALFUN_COM_WRITE,
    AUX_CHOOSETASK_SPECIALFUN_COM_READ,
    AUX_CHOOSETASK_SPECIALFUN_COM_WRITE_RESULT,
		AUX_CHOOSETASK_SPECIALFUN_COM_READ_RESULT,
};


typedef struct	//Yifei
{
    unsigned char TC_BrandName;	// �¿�������
    unsigned char BaudRate;
    unsigned char DataBit;
    unsigned char Parity;
    unsigned char StopBit;
	
    unsigned char uc_Chamber_TempCount;
    unsigned char uc_Chamber_HumiCount;	
    unsigned char T_in_group_id[MAX_CHAMBER_NUM];	// �ô��ڵ��¶ȶ�Ӧ DataBank �� Group Id
    unsigned char T_in_chan_id[MAX_CHAMBER_NUM];		// �ô��ڵ��¶ȶ�Ӧ DataBank �� Channel Id
    unsigned char H_in_group_id[MAX_CHAMBER_NUM];	// �ô��ڵ�ʪ�ȶ�Ӧ DataBank �� Group Id
    unsigned char H_in_chan_id[MAX_CHAMBER_NUM];		// �ô��ڵ��¶ȶ�Ӧ DataBank �� Channel Id
    unsigned char btNone[3];

    short int gR_PresentTempValue[MAX_CHAMBER_NUM];
    short int gR_TempLastValue[MAX_CHAMBER_NUM];
    short int gR_PresentHumiValue[MAX_CHAMBER_NUM];
    short int gR_HumiLastValue[MAX_CHAMBER_NUM];
} Uart_Operation;


typedef struct
{
    bool  IsSetTempValue;
    bool  IsSetHumiValue;
    float TempValue;
    float HumiValue;
}ChamberSetValue;

typedef struct
{
    bool    m_IsOpenChamber;
    bool    m_UseGroupTChamber;
    bool    m_UseDefaultValue;//when Master Chn stop�� use new master chn control value. All chn stop, use default value.
    bool    m_IgnoreDefaultValue;//Ignore Default Value after all chn stop. Keep TChamber tempreature.    
    unsigned short m_UnitIP;
    unsigned short m_IVChnIndexInUnit;
    float   m_DefaultTempSetting;
    float   m_DefaultHumiSetting;
    ChamberSetValue m_ChamberSetValue[MAX_IVMCU_PER_SYS][MAXCHANNELNO];   
}MTCI_GROUP_CTRL;//1910101 zyx
class MTCI_CTRL
{
public:

    static Uart_Operation str_Uart[MAX_UART_TOTALNUM];
    static bool m_btRefreshMap;
    static   UInt16  Priority;
    static   UInt16  TaskID;

		static unsigned char PresentChamberWriteTempCount[MAX_UART_NUM];
		static unsigned char PresentChamberReadTempCount[MAX_UART_NUM];
		static unsigned char PresentChamberWriteHumiCount[MAX_UART_NUM];
		static unsigned char PresentChamberReadHumiCount[MAX_UART_NUM];
		static MsTime LastMtciSendTime[MAX_UART_NUM];


    static MTCI_GROUP_CTRL  st_GroupControler[MAX_UART_NUM]; //�¿�������ͨ��//1910101 zyx
    //  TemperatureChamber
    static  unsigned char uc_Uart_TChamber_Num;   // ɨ�贮��ͨ����
    static  unsigned short int uw16_PUBLIC_REPEAT_NUM; // ������󹫹��ظ�����
    static  unsigned long int uw32_CMD_INTERVALTIME; // ÿ�η���������ʱ��

    static  unsigned char gW_CmdBuf[MAX_UART_BUF_SIZE];  // ��ʱ���ڷ����������
    static  short  gW_TemperatureValue[MAX_UART_NUM][MAX_CHAMBER_NUM];   // д�����¶�ֵSV
    static  short  gW_TemperaLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];  // ���һ��д�����¶�ֵSV
    static  bool gW_IsPreWrCmdOk[MAX_UART_NUM];    // ׼��д��ǰֵ����ɹ���־
    static  bool gW_IsWriteCmdOk[MAX_UART_NUM];   // д����ֵ����ɹ���־
    static  bool gW_IsRunCmdOk[MAX_UART_NUM];     // ��������ɹ���־
    static  bool gW_IfNeedToCheckStatus[MAX_UART_NUM];      // ��Ҫ���ͼ��״̬�����־
    static  bool gW_IfNeedToCheckSetpoint[MAX_UART_NUM];    // ��Ҫ���͵������������־
    static  bool gC_IsRunning[MAX_UART_NUM];    // ��ʽ�������ÿ���ֵ��־

    static  unsigned int gW_PreWrRepeatNum[MAX_UART_NUM];   // ׼��д��ǰֵ�����ظ�������
    static  unsigned int gW_WriteRepeatNum[MAX_UART_NUM];   // д�����ظ�������
    static  unsigned int gW_RunRepeatNum[MAX_UART_NUM];     // д���������ظ�������
    static  unsigned int gW_ReadRepeatNum[MAX_UART_NUM];    // д�������ظ�������
    static  unsigned int gW_StopRepeatNum[MAX_UART_NUM];    // дֹͣ�����ظ�������


    static  unsigned char gR_CmdBuf[MAX_UART_BUF_SIZE];    // ��ʵ���ջ�������ת�����ݻ�����   // ��ʱ���ڽ��������������

    static  MsTime gR_ReadStamp[MAX_UART_NUM];    //  �ѷ�������������¶�ʱ��
    static  MsTime gR_ReadStampForAll[MAX_UART_NUM];   // ȫ��ͨ���ѷ�������������¶�ʱ��
    static  unsigned char gR_ReadDelayCnt;
    static  unsigned char gR_ChanCur;    // ͨ��ɨ�������

    static  MsTime gW_WriteStamp[MAX_UART_NUM];      // �Դ��ڷ��������������ʱ��
    static  MsTime gC_TimeoutStamp[MAX_UART_NUM];    //  ���ݸ��½��ճ�ʱʱ���¼
    static  unsigned long g_InitFlag;

    // 	extern  unsigned char gl_Tx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE]; // ��ʵ���ͻ�����
    // 	extern  unsigned int gl_Tx_first[MAX_UART_NUM];//  ȡ��ָ��
    // 	extern  unsigned int gl_Tx_last[MAX_UART_NUM]; //  ����ָ��
    //     extern  unsigned char gl_Tx_EmptyFlag[MAX_UART_NUM]; //

    // 	extern  unsigned char gl_Rx_buf [MAX_UART_NUM][MAX_UART_BUF_SIZE];   // ��ʵ���ջ�����
    // 	extern  unsigned int gl_Rx_first[MAX_UART_NUM];//  ȡ��ָ��
    // 	extern  unsigned int gl_Rx_last[MAX_UART_NUM]; //  ����ָ��


    static  short  gW_HumiValue[MAX_UART_NUM][MAX_CHAMBER_NUM];
    static  short  gW_HumiLastValue[MAX_UART_NUM][MAX_CHAMBER_NUM];


    static unsigned long int gR_PresentValue_DO32[MAX_UART_NUM];   // ����DOֵ  Unused digital output 0��1��2��Other digital outputs
    static unsigned long int gR_PresentValue_DI32[MAX_UART_NUM];   // ����DIֵ  Unused digital input 0��1��2��Other digital inputs

    static short int  gR_Zero_Counter[MAX_UART_NUM];     // ��������Ϊ��ֵ������
    //extern short int  gR_Updata_Counter[MAX_UART_NUM];    // �������ݸ��¼�����
    //extern short int gR_PresentValue_Set1[MAX_UART_NUM];   // ��������ֵ1  Set value 1

    static  unsigned char  g_Cmd_Flag[MAX_UART_NUM];         // ��ǰִ�д�������
    static  unsigned char  gW_WriteCmd_Flag[MAX_UART_NUM];   // д�¶�/ʪ�ȿ���ֵ�����־
    static  unsigned char  gW_ReadCmd_Flag[MAX_UART_NUM];    // ���¶�/ʪ�ȿ���ֵ�����־
    static  unsigned char  gW_StopCmd_Flag[MAX_UART_NUM];    // ֹͣ�¶�/ʪ�ȿ���ֵ�����־
    static  unsigned char  gW_RunCmd_Flag[MAX_UART_NUM];     // �����¶�/ʪ�ȿ���ֵ�����־
    static  unsigned char  g_WriteCmd_Counter[MAX_UART_NUM]; // д���������
    static  unsigned char  g_ReadCmd_Counter[MAX_UART_NUM];  // �����������
    static  unsigned char  g_StopCmd_Counter[MAX_UART_NUM];  // ֹͣ���������
    static  unsigned char  g_RunCmd_Counter[MAX_UART_NUM];   // �������������

    static  unsigned char  gR_ReadCmd_Flag[MAX_UART_NUM];    // ���¶�/ʪ�������־  �������ڶ�ȡ���ݴ����¶�/ʪ��״̬
    static  MsTime         gR_ReadCmd_Stamp[MAX_UART_NUM];   // ���¶�/ʪ���������ʱ��

    static  unsigned char  gW_HumiFlag[MAX_UART_NUM];         //  ���ڱ�־�Ƿ��Ѿ�����ʪ��
    static  MsTime  gW_HumiInterval[MAX_UART_NUM];     //  ��������ʪ�ȼ��ʱ��

    static  MsTime   Wait_delay[MAX_UART_NUM];        // д��������ʱ
    static  unsigned char   uc_If_Wait_delay[MAX_UART_NUM];  // д��������ʱ��־
    static  unsigned char   uc_OffLine[MAX_UART_NUM][MAX_CHAMBER_NUM];
    //------------------------------
    //  << PLCS7-200�¿��� >>  Begin
    static unsigned char uc_RunTH[MAX_UART_NUM];      // �����»�ʪģʽ
    static unsigned char uc_RunFrist[MAX_UART_NUM];   // ��һ�������»�ʪģʽ
    static unsigned char uc_RunCount[MAX_UART_NUM];   // ��һ�������»�ʪģʽ�������
    static unsigned char uc_ReadCount[MAX_UART_NUM];  // �»�ʪģʽ��ȡʱ�������
    static unsigned char uc_LockModleT[MAX_UART_NUM];  // �����¶�ģʽ
    static short  gW_T_Back[MAX_UART_NUM];  // д�����¶�ֵSV  ����
    static short  gR_PV_Back[MAX_UART_NUM];  // ���¶�ֵPV  ����
    //  << PLCS7-200�¿��� >>  End
    //----------------------------------

    //     extern struct FUNC{
    //     void (* Function) (void);		//function ����ָ��   //��ʽ  void function(void);
    //     }st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

    //  End  TemperatureChamber


    //------------------------------
    //  << Voetsch�¿��� >>  Begin
    static unsigned char gw_WarnLimit_Flag[MAX_UART_NUM];    // д�������Ʊ���
    static unsigned char gR_Updata_Flag[MAX_UART_NUM];    // �������ݸ��±���
    static short int  gR_Updata_Counter[MAX_UART_NUM];    // �������ݸ��¼�����
    //extern short int gR_PresentValue_Nominal2[MAX_UART_NUM];  // ��������ֵ2�������  Nominal value of parameter 2
    //extern short int gR_PresentValue_Set1[MAX_UART_NUM][12];   // ��������ֵ1  Set value 1
    static short int gR_PresentValue_Set1[MAX_UART_NUM];   // ��������ֵ1  Set value 1
    static unsigned long int gR_PV_DO32[MAX_UART_NUM];   // ����DOֵ  Unused digital output 0��1��2��Other digital outputs
    //  << Voetsch�¿��� >>  End
    //----------------------------------


public:

    static void DoWork(void);
    static void MyInit(void);
    static void reset(unsigned char uc_ChNo,unsigned char t_controller_type);//DK
    static void DtReset(unsigned char uc_ChNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendtwentyoneCmd(unsigned char ChanNo);
    static void SendeightCmd(unsigned char ChanNo);
    static void SendtwentythreeCmd(unsigned char ChanNo);
    static void Clear_Counter(unsigned char ChanNo);



    static void SendCheckStatusCmd(unsigned char ChanNo);
    static void SendCheckSetpointCmd(unsigned char ChanNo);
    static bool IfCanSendReadCmd(unsigned char ChanNo);
    //static void SetPresentValue(unsigned char ChanNo, short int Value, unsigned char uc_Temp4Humi_Flag);
		static void SetPresentValue(unsigned char ChanNo, short int Value, unsigned char uc_Temp4Humi_Flag, unsigned char uc_Chamber);
    static void DoAllWriteCmdCtrl(unsigned char ChanNo);
    static void DoAllParseResultCtrl(unsigned char ChanNo);
    static void DoAllReadCmdCtrl(unsigned char ChanNo);
    static void SetPointValueCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag, unsigned short RunON);
    static void ExecStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    //short int ctrl_GetValueCmd(unsigned char ChanNo);
    //unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned int CheckSum(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned char CheckXorSum(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);

    //void ctrl_Value2String(short int * pvalue, unsigned char* Destn);
    //void ctrl_String2Value(unsigned char* string, short int* pValue);
    //bool ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len);
    static void Init_SetRun(void);
    static unsigned char Change_T_H(unsigned char ChanNo,unsigned char uc_CmdType);
    static unsigned char ExchangeCheckCounter(unsigned char Channel,unsigned char *CmdSource, unsigned char CmdDest,unsigned char * Counter);
    static unsigned char Get_SetPoinState(unsigned char ChanNo,unsigned char uc_CmdType);

    //unsigned char  CalcCheckSum_Byte(unsigned char *puchMsg, unsigned int usDataLen);
    static unsigned char*  String_MoveBuff(unsigned char* Source, unsigned char* Destn,unsigned int length);

    //short int Convert_FloatToInt(unsigned char* Source);
    //void Convert_IntToFloat(unsigned char* Source,short int w16_Value);
};


//����ָ��
typedef void (*My_Func_Ptr_Void)(void);
typedef char (*My_Func_Ptr)(unsigned long  Value_A,unsigned long Value_B);
typedef char (*My_Func_Ptr_Ext)(unsigned long  Value_A,unsigned long  Value_B,unsigned long Value_C);
typedef void (*My_Func_Ptr_Ext1)(unsigned char Value_A,unsigned char Value_B);
typedef void (*My_Func_Ptr_Ext2)(unsigned char Value_A,unsigned short Value_B,unsigned short Value_C,unsigned char Value_D);
typedef float (*My_Func_ptr_Ext3)(unsigned char Value_A);
//���õĻ�
//pf();    ����ɲο���C��ָ�롷


#endif