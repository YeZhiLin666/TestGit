/*****************************************************************************
// �����ļ�      :TH_SIEMENS_S7_226CN.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SIEMENS_S7_226CN.h **/

#ifndef TH_SIEMENS_S7_226CN_H
#define TH_SIEMENS_S7_226CN_H 1


#define S7_226CN_ASCII_FIRST         (0X30)  // '0'   ���ڰ��һ����ʼ�Ĵ��ڱ��  ������S7-200 �Ƚ��ر��0��ʼ


//  ��Ӧ����������
#define TH_S7_226CN_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_S7_226CN_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_S7_226CN_MIN_RESPONSE_CMD_LEN		(TH_S7_226CN_MIN_RESPONSE_CMD_NUM*TH_S7_226CN_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_S7_226CN_MAX_RESPONSE_CMD_LEN		(TH_S7_226CN_MAX_RESPONSE_CMD_NUM*TH_S7_226CN_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_S7_226CN_MAX_COM_WRITE_CMD	     (10+8)   // CMD(10 byte)+DATA(8 byte)    
#define TH_S7_226CN_MAX_COM_WRITE_REPLY	     (10)     // REPLY(10 byte)  
#define TH_S7_226CN_MAX_COM_READ_CMD	     (10)     // CMD(10 byte)  
#define TH_S7_226CN_MAX_COM_READ_REPLY	     (10+128)    // REPLY(10 byte)+DATA(128 byte)  
#define TH_S7_226CN_MAX_COM_RUN_CMD	         (10+8)   // CMD(10 byte)+DATA(8 byte)      
#define TH_S7_226CN_MAX_COM_STOP_CMD	     (10+8)   // CMD(10 byte)+DATA(8 byte)      

#define HUMI_INTERVALTIME	     (6000)   // ʪ�ȼ��ʱ��6�룬����û�����Զ��л��¶�ģʽ

class TH_S7_226CN
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
