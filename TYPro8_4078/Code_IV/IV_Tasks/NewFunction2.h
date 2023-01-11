//����������
//comment out chen 20130423 ����

#ifndef _THIRD_PARTY_H_
#define _THIRD_PARTY_H_

// #define _THIRD_PARTY_MSG_MAX_NUM_    16

///////////////////////////////////����ṹ�ȶ���
//������״̬status enum
enum
{
    THIRD_PARTY_STATUS_INACTIVE = 0x10,
    THIRD_PARTY_STATUS_IDLE,                //0x11
    THIRD_PARTY_STATUS_TRAN,              //0x12
    THIRD_PARTY_STATUS_OUTPUT,          //0x13
};

//���������������������Զ�У׼ģ�������������������ͬ��
//comment out chen 20130516
//cmd enum
//��ͬ�ľ���IV������output���л�����У׼��output��ζ���е�����ʱ��Ӧ���ǲ��ܴ����л���
//��������ܱ�֤�Ļ�����������output�½���tran
//comment out chen 20130520    Ԥ������
enum
{
    THIRD_PARTY_SET_3_RD_MODE = 0x01,    //����
    THIRD_PARTY_CC,                        //����
    THIRD_PARTY_CV,                        //��ѹ
    THIRD_PARTY_CP,                        //�㹦��
    THIRD_PARTY_CL,                        //�㸺��
    THIRD_PARTY_SETCLAMPV,          //ǯѹ
    THIRD_PARTY_STOP,                    //ֹͣ���

    THIRD_PARTY_IV_SET,                  //IVset���

    THIRD_PARTY_CLR_3_RD_MODE,    //�˳�    0x09

    //AUX����
    THIRD_PARTY_AUX_TEMPERATURE,
    THIRD_PARTY_AUX_VOLTAGE,
};

//CALI У׼ģʽ
enum
{
    //comment out chen 20130423    IV need
    THIRD_PARTY_MANUAL = 0x10,   // �ֶ�
    THIRD_PARTY_AUTO = 0X20,     // �Զ�У׼
    THIRD_PARTY_AUTO_CALI_HP_3RD = 0X21,          //HP��ģʽ
    THIRD_PARTY_AUTO_CALI_SELF_3RD = 0X22,        //У׼����
    THIRD_PARTY_AUTO_CALI_ADC_3RD = 0X23,       //ADCģʽ

    THIRD_PARTY_AUTO_CALI_OFF,    //���鱣��
};

//���module enum ���ģʽ
// enum
// {
// 	AUTO_CALI_CELL_WORK_MODULE_NONE = 0x05, //���ά��
// 	AUTO_CALI_CELL_WORK_MODULE_IV,                   //����IV
// 	AUTO_CALI_CELL_WORK_MODULE_BY_PASS,         //��·
// 	AUTO_CALI_CELL_WORK_MODULE_DISCHARGE,    //�ŵ�
// 	AUTO_CALI_CELL_WORK_MODULE_CHARGE,         //���
// };
//COM V++enum ����ģʽ
// enum
// {
// 	AUTO_CALI_COM_MODULE = 0x15,
// 	AUTO_CALI_COM_VPP,
// };

//IV stuff enum
enum
{
    DISPOSABLE_MSG_CMD_OFF_TO_CC = 0xB0,
    DISPOSABLE_MSG_CMD_CC_TO_CC,
    DISPOSABLE_MSG_CMD_CC_TO_OFF,
    DISPOSABLE_MSG_CMD_OFF_TO_CV,    //��ѹ���
    DISPOSABLE_MSG_CMD_CV_TO_CV,
    DISPOSABLE_MSG_CMD_CV_TO_OFF
};

//chn status
enum
{
    CHANNEL_INFO_CLOSE =0x01,
    CHANNEL_INFO_CC,        //����
    CHANNEL_INFO_CV,        //��ѹ
    CHANNEL_INFO_CP,        //�㹦��
    CHANNEL_INFO_OPEN,
};

//��ѹmodule enum ��ѹģʽ
enum
{
    AUTO_CALI_VOLTAGE_MODULE_NONE = 0x70, // ��ѹ���ض����
    AUTO_CALI_VOLTAGE_MODULE_HARDWARE = 0x71, // ��ѹ�л�·
    AUTO_CALI_VOLTAGE_MODULE_DIGIT = 0X72,      // ��ѹû��·�����ֿ���
    AUTO_CALI_VOLTAGE_MODULE_POWER = 0X73,    // ��ѹ���õ�Դ
    AUTO_CALI_VOLTAGE_MODULE_RESISTANCE = 0X74,      // ��ѹ���õ���
};

typedef struct
{
    unsigned char BoardType;    //��һ��set-3rdʱ�����
    //����ͨУ׼ͨ��ʱ��THIRD_PARTY_MANUAL
    //���Զ�У׼ͨ��ʱ��Ԥ��

    unsigned char Battery_Module;    //���ģʽ
    unsigned char COM_Module;      //V++/COM
    unsigned char CaliType;    //��������ѹ ǯѹ

    unsigned char I_Range;    //����
    unsigned char V_Range;    //��ѹ����(Ԥ��)
    unsigned char ctrl_Type;    //��������  Ԥ��  comment out chen20130423

    unsigned char isVDAC;    //��ѹDAC�Ƿ����
    unsigned char Reserved1;  //Ԥ��

    float IDAC_Set;            //����DACֵ
    float VDAC_Set;           //��ѹDACֵ

    float Reserved2;    		 //Ԥ��
} st_Third_Party_ctl_Ex;    //������������չ

typedef struct
{
    unsigned char cmd;    //��������

    st_Third_Party_ctl_Ex ctl_cmd_ex;    //��չ����
} st_Third_Party_CMD;

//send msg struct
typedef struct
{
    unsigned char TaskID;    //just  ��ԴTask

    unsigned char chn;
    //the pointer or RAM
    st_Third_Party_CMD st_third_party_cmd;
} st_Third_Party_Msg;

//��ѯ���
typedef struct
{
    unsigned short handleID;
    unsigned char type;    //����,CC,CV or others
} st_Third_Handle;

///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////
#ifndef _AUTO_CALI_THIRD_PARTY_

//��
class thirdParty
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]


    //�Ƿ�ĳ�֧�ֶ�ͨ��

    static unsigned char UnsafeLockSwitch[MAXCHANNELNO];                       //�ǰ�ȫ״̬����ģʽ����  add by qjm 20140822

    //static MsTime WatchDog;  //��λû����Ӧʱ������״̬���� dirui 0828
    //static MsTime TimeTickForWatchDog;   //20130904,deleted by hepeiqing,we do watchdog in ethernetparse
public:
    static void Init(void);
    static void DoWork(void);
    /*
    1 ��parse���͵���task  ֮�������msg����
    2 ���䷢�͵�msg�ڵ�cmd��״̬������  ����һ����ʱ��
    3 ���������״̬���Թ�parse��ѯ
    */
// 	static void EndTask(void);
    static unsigned char sendMsgToMe(const st_Third_Party_Msg * st_third_party_msg);    //ѹ��msg
    static signed char getStatus(unsigned char chn);

    static void SoftWare_WatchDog(void);   //��λû����Ӧʱ������״̬���� dirui 0828

private:
    static unsigned char m_status[MAXCHANNELNO];    //״̬��    8bit
    static st_Third_Party_Msg st_third_party_msg[MAXCHANNELNO];
    static unsigned char m_MsgValid[MAXCHANNELNO];
    static st_Third_Handle st_third_handle[MAXCHANNELNO];

    static unsigned char manu_auto_Flag[MAXCHANNELNO];

  static float GetNewCurrentValue_FineTuning(unsigned char chn);
    static void processMSG(unsigned char chn);    //��һ������msg
    static void processFSM(unsigned char chn);    //�ڶ�������״̬��

    static unsigned char setStatus(unsigned char chn,unsigned char status);    //���Ĵ�ͨ����״̬
    static unsigned short stuffDisposableMsg(unsigned char chn,unsigned char type);    //��䲢����һ����ʱ������
    static int processDisposableHandle(unsigned char chn);

// 	static void clearMSG(char msgID);

    //lock sch
    //		static void lockSCH(unsigned char chn);
    //		static void unlockSCH(unsigned char chn);

    static unsigned char AutoCaliFlag;
};

#endif
///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////Auto Cali///////////////////////////////////////////////////////
#ifdef _AUTO_CALI_THIRD_PARTY_

class thirdParty
{
    //��Ա����
public:
    static UInt16 Priority;    //���ȼ�
    static UInt16 TaskID;	  //THIRDPARTY_TASK
    static unsigned long Dac_set_value;    //�����Զ�У׼DAC��ֵ����msg�ڻ�ã�������Auto_Base
private:
    static unsigned char m_status;    //״̬�� ֻ�е�chn 0ʹ��
    static unsigned char msgFlag;    //1bit����
    static unsigned char logicStatus;    //�����Զ�У׼������flag
    static st_Third_Party_Msg m_msg;

    static unsigned char Auto_Board_Work_Module;    //��ǰ����ģʽ
    static unsigned char m_BoardType;    //IVͨ��������    Ԥ��
    static unsigned char m_Battery_Work_Module;    //���״̬
    static unsigned char COM_V_Module;    //COM V++״̬
    static unsigned char Reserved1;       //Ԥ��1
    static unsigned char Reserved2;       //Ԥ��2

    static MsTime m_timer;

    static float auto_dac_out[4];

    //�ӿں���
public:
    static void Init(void);
    static void DoWork(void);    //������
    /*
    1 ����������յ�ǰ״̬�����Ƿ���Ӧ
    2 ִ����Ӧ���̣�����Tran�׶�
    */
    static void EndTask(void);

    static char sendMsgToMe(st_Third_Party_Msg * st_third_party_msg);    //ѹ��msg

    static unsigned char getStatus(unsigned char chn);    //��ȡ״̬
private:

    static unsigned char ProcessFSM(void);    //����״̬������msg��CMD
    static unsigned char ProcessLogic(void);
    static unsigned char StuffLogicStatus(unsigned char type);    //��ǰԤ������

    static unsigned char ProcessAutoDrive(unsigned char type);    //�����Զ�У׼��
    static unsigned char ProcessAutoDirveBase(void);    //��������ģʽ

    static unsigned char SetStatus(unsigned char state);    //����״̬


    static unsigned char processDAC(void);
};

#endif
///////////////////////////////////////////////////////Auto Cali///////////////////////////////////////////////////////

#endif


//ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx    //ͨ����step    ChannelStep[chn].m_ucActiveIdx��ǰ��
//��һ��  (!m_ucActiveIdx)

