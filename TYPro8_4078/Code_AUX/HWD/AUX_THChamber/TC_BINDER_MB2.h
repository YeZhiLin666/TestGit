/*****************************************************************************
// �����ļ�      :TC_BINDER_MB2.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2016.12.12
*****************************************************************************/
/** TC_BINDER_MB2.h **/

#ifndef TC_BINDER_MB2_H
#define TC_BINDER_MB2_H 1

//  ��Ӧ����������
#define TC_MB2_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С�������� //1
#define TC_MB2_MAX_RESPONSE_CMD_NUM        1 // 9֡   // ��Ӧ�������������
#define TC_MB2_MIN_RESPONSE_CMD_LEN		(TC_MB2_MIN_RESPONSE_CMD_NUM*TC_MB2_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TC_MB2_MAX_RESPONSE_CMD_LEN		(TC_MB2_MAX_RESPONSE_CMD_NUM*TC_MB2_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TC_MB2_MAX_COM_WRITE_CMD	     (11+2)   // CMD(6 byte)+CRC(2 byte)    
#define TC_MB2_MAX_COM_WRITE_REPLY	     (6+2)   // REPLY(6 byte)+CRC(2 byte)   
#define TC_MB2_MAX_COM_READ_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TC_MB2_MAX_COM_READ_REPLY	     (7+2)   // REPLY(7 byte)+CRC(2 byte)  
#define TC_MB2_MAX_COM_RUN_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TC_MB2_MAX_COM_STOP_CMD	         (11+2)   // CMD(6 byte)+CRC(2 byte)   

class TC_MB2
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


#endif
