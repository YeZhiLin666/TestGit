


#ifndef _AUX_THIRD_PARTY_H_
#define _AUX_THIRD_PARTY_H_
#define _AUXTHIRD_PARTY_MSG_MAX_NUM_    64


///////////////////////////////////����ṹ�ȶ���
//������״̬status enum
enum
{
    AUXTHIRD_PARTY_STATUS_INACTIVE = 0x10,
    AUXTHIRD_PARTY_STATUS_IDLE,                //0x11
    AUXTHIRD_PARTY_STATUS_TRAN,              //0x12
    AUXTHIRD_PARTY_STATUS_OUTPUT,          //0x13
};


enum
{
    AUXTHIRD_PARTY_SET_3_RD_MODE = 0x01,    //����
    AUXTHIRD_PARTY_AUX_TEMPERATURE,
    AUXTHIRD_PARTY_AUX_VOLTAGE,
    AUXTHIRD_PARTY_CLR_3_RD_MODE,    //�˳�    0x04

    //AUX����

};

typedef struct
{

    float Reserved2;    		 //Ԥ��
} st_AUXThird_Party_ctl_Ex;    //������������չ


typedef struct
{
    unsigned char cmd;    //��������

    st_AUXThird_Party_ctl_Ex ctl_cmd_ex;    //��չ����
} st_AUXThird_Party_CMD;

//send msg struct
typedef struct
{
    unsigned char TaskID;    //just  ��ԴTask

    unsigned char chn;
    //the pointer or RAM
    st_AUXThird_Party_CMD st_Auxthird_party_cmd;
} st_AUXThird_Party_Msg;

//��ѯ���
typedef struct
{
    unsigned short handleID;
    unsigned char type;    //����,CC,CV or others
} st_AUXThird_Handle;

class AuxthirdParty
{
public:
    static unsigned short Priority;
    static unsigned short TaskID;			//  should be a definition which positioning this task in OS_Task[]




public:
    static void Init(void);
    static void DoWork(void);
    /*
    1 ��parse���͵���task  ֮�������msg����
    2 ���䷢�͵�msg�ڵ�cmd��״̬������  ����һ����ʱ��
    3 ���������״̬���Թ�parse��ѯ
    */
    static void EndTask(void);
    static unsigned char sendMsgToMe(const st_AUXThird_Party_Msg * msg);    //ѹ��msg
    static st_AUXThird_Handle st_Auxthird_handle[MAX_AUXCH_PER_AUXMCU];
    static signed char getStatus(unsigned char chn);

    static void SoftWare_WatchDog(void);   //��λû����Ӧʱ������״̬���� dirui 0828

private:
    static unsigned char m_ActualAuxChannel;
    static unsigned char m_Auxstatus[MAX_AUXCH_PER_AUXMCU];    //״̬��    8bit
    static st_AUXThird_Party_Msg st_Auxthird_party_msg[MAX_AUXCH_PER_AUXMCU];
    static unsigned char m_AuxmsgFlag[MAX_AUXCH_PER_AUXMCU];
    //static st_Third_Handle st_Auxthird_handle[MAX_AUXCH_PER_AUXMCU];



    static void processMSG(unsigned char chn);    //��һ������msg
    static void processFSM(unsigned char chn);    //�ڶ�������״̬��


    static unsigned char setStatus(unsigned char chn,unsigned char status);    //���Ĵ�ͨ����״̬



    static void clearMSG(char msgID);


};






#endif
