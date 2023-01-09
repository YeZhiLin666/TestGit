#pragma once


#define MAX_BUFFER_MESSAGE     8 // ��Ϣ�������ݶ������8����Ϣ
#define MAX_INTERVAL_TIME      4*60*60*10000 // ���RSN���ݶ������ʱ��4Сʱ
#define MAX_REPORT_TIMEOUT     15*10000 // ���󱨸�PC�ݶ���೬ʱʱ��15��
#define MAX_I2C_TIMEOUT        10*10000 // I2C��д�ݶ���೬ʱʱ��10��
#define TIMEOUT_1s             1*10000 // I2C��д�ݶ���೬ʱʱ��1��

#define MAX_READ_RSN_TRY       10   // RSN�Ƚ�������




enum TE_RSN_BOARD
{
    OPERATIONAL,
    RSN_DIRTY_WO_INFO,
    RSN_DIRTY_READING_INFO,
    RSN_DIRTY_W_INFO,
    REPORT_TO_PC,
    REPORTED_TO_PC,
    REQUEST_WRITE_RSN,
    EXECUTE_WRITE_RSN,
    CONFIRM_WRITE_RSN,
};

enum TE_CALIB_CHANNEL
{
    READY,
    REQUEST_WRITE_CALIB,
    EXECUTE_WRITE_CALIB,
    CONFIRM_WRITE_CALIB,
};

enum TE_I2C_TASK_PROCESS
{
    I2C_TASK_WRITE_RSN = 11,
    I2C_TASK_WRITE_RSN_WAIT ,
    I2C_TASK_READ_RSN ,
    I2C_TASK_READ_RSN_WAIT ,

    I2C_TASK_W_BID = 21,
    I2C_TASK_R_BID ,
    I2C_TASK_R_B_INFO ,
    I2C_TASK_R_B_CH_INFO ,
    I2C_TASK_WR_WAIT,

    I2C_TASK_W_CH_CALIB = 31,
    I2C_TASK_W_CH_CALIB_WAIT ,
    I2C_TASK_R_CH_CALIB ,
    I2C_TASK_R_CH_CALIB_WAIT ,

    I2C_TASK_FRAM_STATA = 41,
    I2C_TASK_FRAM_RSN,
    I2C_TASK_R_RSN,
    I2C_TASK_R_RSN_WAIT ,

    I2C_TASK_R_SUCCESS = 98 ,
    I2C_TASK_R_ERROR = 99 ,

};

enum TE_RSN_COMPARE
{
    RSN_COMPARE_NEW   = 9999,
    RSN_COMPARE_SAME  = 0,
    RSN_COMPARE_DIFFE = 1,
};


enum
{
    MODEL_BOARD ,
    MODEL_CHANNEL ,
};

typedef struct
{
    unsigned char  uc_MsgNumber;
    unsigned char  uc_AuxType;
    unsigned char  uc_BoardID;
    unsigned short int  uw16_ChannelID;
    unsigned char  uc_MsgCode;
    unsigned char  uc_MsgSource;
} St_DataMessage;

typedef struct
{
    unsigned char    uc_MsgProducePointer;
    unsigned char    uc_MsgConsumePointer;
    unsigned char    uc_TotalMessageCount;
    St_DataMessage   st_MSG[MAX_BUFFER_MESSAGE];
} St_MSG_Manage;

typedef struct
{
    unsigned char  uc_BoardID;
    unsigned char  uc_I2CReadFlag;
    unsigned char  uc_FRAMReadFlag;
    unsigned char  uc_RSN_Stata;
    unsigned short int uw16_BoardRSN_I2C;
    unsigned short int uw16_BoardRSN_FRAM;
} St_Check_RSN;

class AuxConfigRSN
{
public:
    static unsigned short               Priority;
    static unsigned short               TaskID;
    static unsigned char                uc_Report_RSNError[MAX_BOARD_PER_AUXMCU];    // ����������
    static St_Check_RSN                 st_BoardRSN[MAX_BOARD_PER_AUXMCU];    // I2C����FRAM���RSN

private:
    static unsigned char                uc_FRAMRSN_Load[MAX_BOARD_PER_AUXMCU];  // RSN���Ѿ����ع�   // yy 20140912  ���T����Ϊ�����⡣
    static unsigned char                uc_BoardRSN_Counter;  // ȫ����ʹ��RSN�ŵļ�����
    static unsigned char                uc_Board_RSNStatus[MAX_BOARD_PER_AUXMCU];      // FRAM ���״̬
    static unsigned char                uc_Channel_CALIBtatus[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   // У�� ͨ����״̬
    static unsigned char                uc_RSN_JustWrite[MAX_BOARD_PER_AUXMCU];   // RSN�ճɹ�д��
    static unsigned char                uc_WaitI2C_WriteBoard[MAX_BOARD_PER_AUXMCU]; // �ȴ�I2Cд��RSN����
    static unsigned char                uc_WaitI2C_WriteCh[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // �ȴ�I2Cд����ͨ��У������
    static unsigned char                uc_ReadBoard_Count[MAX_BOARD_PER_AUXMCU];    // �ظ�I2C�������������
    static unsigned char                uc_ReadCh_Count[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD]; // �ظ�I2C������ͨ��У������������
    static unsigned char                uc_Check_BoardRSN[MAX_BOARD_PER_AUXMCU];     // ���RSN��־
    static unsigned char                uc_Case2_temp[MAX_BOARD_PER_AUXMCU];         // ��ʱ��Ϊ���ݱ�־�� ,���ڵڶ����¼�
    static unsigned char                uc_ScanBoardID;  //ɨ����
    static unsigned char                uc_ScanBChID;   //ɨ���ͨ����
    static unsigned short               uw16_I2CHandleID_BD[MAX_BOARD_PER_AUXMCU];     //  I2C��ѯ����Ϣ��
    static unsigned short               uw16_I2CHandleID_CH[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];   //  I2C��ѯͨ����Ϣ��
    static MsTime                       st_IntervalTime_RSN[MAX_BOARD_PER_AUXMCU];       // ���ʱ����RSN
    static MsTime                       st_IntervalTime_JustWriteRSN[MAX_BOARD_PER_AUXMCU];       // RSN�ճɹ�д�����ʱ��
    static MsTime                       st_Timeout_I2C_Board[MAX_BOARD_PER_AUXMCU];    // I2C��д��/�������ȴ���ʱʱ��
    static MsTime                       st_Timeout_I2C_Ch[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];       // I2C��дͨ���ȴ���ʱʱ��
    static St_MSG_Manage                st_RSNMSG_Manage[MAX_BOARD_PER_AUXMCU];      // ��MSG ����
    static St_MSG_Manage                st_CALIBCh_Manage[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];      // ͨ��MSG ����
    static unsigned short               uw16_CALIBCh_TotalMSGCount;
    static St_AUXChannelCFG             st_RSN_ReadCfg[MAX_AUXCH_PER_BOARD];  //  I2C��дͨ����Ϣ

public:
    static void Init(void);
    static void DoWork(void);

    static void PushMsg_RSN( unsigned char uc_Mode, St_DataMessage * st_Message );
    static void Check_RSN_Init( unsigned char uc_BoardID );

private:
    static void PreprocessMsg_RSN(unsigned char uc_BoardID);
    static void ProcessState_BoardRSN(unsigned char uc_BoardID);
    static void PreprocessMsg_ChCALIB(unsigned char uc_BoardID);
    static void ProcessState_ChCALIB(unsigned char uc_BoardID);
    static UWord16 Produce_RSN(unsigned char uc_BoardID);
    static void Check_BoardRSN_TaskCase(unsigned char uc_BoardID);
    static void Check_BoardRSN(St_Check_RSN * St_RSN);
    static void CleanMsg_RSN( unsigned char uc_Mode, unsigned char uc_Bid ,unsigned short int uw16_Chan);

};

