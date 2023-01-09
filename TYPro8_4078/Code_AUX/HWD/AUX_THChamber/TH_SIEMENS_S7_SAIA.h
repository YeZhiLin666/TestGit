/*****************************************************************************
// �����ļ�      :TH_SIEMENS_S7_SAIA.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2017.10.10
*****************************************************************************/
/** TH_SIEMENS_S7_SAIA.h **/


//  ��Ӧ����������
#define TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_NUM        1 // 1֡   // ��Ӧ����С��������
#define TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_LEN		(TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_NUM*TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)    // 1֡*N  // ��Ӧ����С��������
#define TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_LEN		(TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_NUM*TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD	         (8+12)  // MBAP(8 byte)+CMD(12 byte)  
#define TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_REPLY	       (8+3)   // MBAP(8 byte)+REPLY(3 byte)
#define TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD	           (8+8)   // MBAP(8 byte)+CMD(8 byte)  
#define TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY	         (8+7)   // MBAP(8 byte)+REPLY(7 byte)
#define TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD	           (8+10)   // MBAP(8 byte)+CMD(10 byte) 
#define TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD	           (8+10)   // MBAP(8 byte)+CMD(10 byte)
//#define TH_SIEMENS_S7_SAIA_MAX_COM_RUNorSTOP_REPLY	   (8+3)   // MBAP(8 byte)+REPLY(3 byte)


class TH_SIEMENS_S7_SAIA
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char SetPresentValue_Check(unsigned char ChanNo, short int Value);
};

