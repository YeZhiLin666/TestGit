/*****************************************************************************
// �����ļ�      :TC_DONGDA_MEWTO.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2021.9.14
*****************************************************************************/
/** DONGDA.h **/



#ifndef T_DONGDA_H
#define T_DONGDA_H 1
//DONGDA  TCY1023

#define TC_DONGDA_ASCII_FIRST         (0X01)  // '1'   ���ڰ��һ����ʼ�Ĵ��ڱ�Ŵ�1��ʼ


//  ��Ӧ����������
#define TC_DONGDA_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TC_DONGDA_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TC_DONGDA_MIN_RESPONSE_CMD_LEN		(TC_DONGDA_MIN_RESPONSE_CMD_NUM*TC_DONGDA_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TC_DONGDA_MAX_RESPONSE_CMD_LEN		(TC_DONGDA_MAX_RESPONSE_CMD_NUM*TC_DONGDA_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TC_DONGDA_MAX_COM_WRITE_CMD	     (24)   // CMD(19 byte)+DATA(4 byte)    
#define TC_DONGDA_MAX_COM_WRITE_REPLY	     (23)   // REPLY(23 byte)  
#define TC_DONGDA_MAX_COM_READ_CMD	         (20)   // CMD(19 byte)  
#define TC_DONGDA_MAX_COM_READ_REPLY	     (12)   // REPLY(8 byte)+DATA(4 byte)  
#define TC_DONGDA_MAX_COM_RUN_CMD	         (24)   // CMD(23 byte)    
#define TC_DONGDA_MAX_COM_STOP_CMD	         (24)   // CMD(23 byte)     

#define HUMI_INTERVALTIME	     (6000)   // ʪ�ȼ��ʱ��6�룬����û�����Զ��л��¶�ģʽ

class TC_DONGDA
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
