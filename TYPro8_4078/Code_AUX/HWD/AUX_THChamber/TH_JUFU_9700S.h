/*****************************************************************************
// �����ļ�      :TH_JUFU_9700S.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2018.12.04
*****************************************************************************/
/** TH_JUFU_9700S.h **/



#ifndef TH_JUFU_9700S_H
#define TH_JUFU_9700S_H 1


#define JUFU_9700S_ASCII_FIRST         (0X01)  // '1'   ���ڰ��һ����ʼ�Ĵ��ڱ�Ŵ�1��ʼ


//  ��Ӧ����������
#define TH_JUFU_9700S_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_JUFU_9700S_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_JUFU_9700S_MIN_RESPONSE_CMD_LEN		(TH_JUFU_9700S_MIN_RESPONSE_CMD_NUM*TH_JUFU_9700S_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_JUFU_9700S_MAX_RESPONSE_CMD_LEN		(TH_JUFU_9700S_MAX_RESPONSE_CMD_NUM*TH_JUFU_9700S_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����



// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_JUFU_9700S_MAX_COM_WRITE_CMD	     (14+6)   // CMD(15 byte)+DATA(6 byte)    
#define TH_JUFU_9700S_MAX_COM_WRITE_REPLY	     (14+6)   // REPLY(21 byte)  
#define TH_JUFU_9700S_MAX_COM_READ_CMD	         (6)     // CMD(6 byte)  
#define TH_JUFU_9700S_MAX_COM_READ_REPLY	     (6+8)    // REPLY(6 byte)+DATA(16 byte)  
#define TH_JUFU_9700S_MAX_COM_RUN_CMD	         (10)   // CMD(10 byte)    
#define TH_JUFU_9700S_MAX_COM_STOP_CMD	         (10)   // CMD(10 byte)     

#define HUMI_INTERVALTIME	     (6000)   // ʪ�ȼ��ʱ��6�룬����û�����Զ��л��¶�ģʽ

class TH_JUFU_9700S
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
