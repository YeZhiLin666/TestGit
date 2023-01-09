/*****************************************************************************
// �����ļ�      :TH_SHANGHAI_PINDUN_TEMI880.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SHANGHAI_PINDUN_TEMI880.h **/



#ifndef TH_SHANGHAI_PINDUN_TEMI880_H
#define TH_SHANGHAI_PINDUN_TEMI880_H 1


#define SHANGHAI_PINDUN_TEMI880_ASCII_FIRST         (0X01)  // '1'   ���ڰ��һ����ʼ�Ĵ��ڱ�Ŵ�1��ʼ


//  ��Ӧ����������
#define TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_LEN		(TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM*TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_LEN		(TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_NUM*TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD	     (14+6)   // CMD(14 byte)+DATA(6 byte)    
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_REPLY	     (14+6)   // REPLY(20 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD	         (6)     // CMD(6 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY	     (5+16)    // REPLY(5 byte)+DATA(16 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD	         (10)   // CMD(10 byte)    
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD	         (10)   // CMD(10 byte)     

#define HUMI_INTERVALTIME	     (6000)   // ʪ�ȼ��ʱ��6�룬����û�����Զ��л��¶�ģʽ

class TH_SHANGHAI_PINDUN_TEMI880
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);  // ���
    static void WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);  // ��֧
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char SetPresentValue_Check(unsigned char ChanNo, short int Value);
};


#endif
