/*****************************************************************************
// �����ļ�      :TH_GUANGZHOU_WUSUO.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_GUANGZHOU_WUSUO.h **/



#ifndef TH_GUANGZHOU_WUSUO_H
#define TH_GUANGZHOU_WUSUO_H 1


#define GUANGZHOU_WUSUO_ASCII_FIRST         (0X01)  // '1'   ���ڰ��һ����ʼ�Ĵ��ڱ�Ŵ�1��ʼ


//  ��Ӧ����������
#define TH_GUANGZHOU_WUSUO_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_GUANGZHOU_WUSUO_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_GUANGZHOU_WUSUO_MIN_RESPONSE_CMD_LEN		10 //(TH_GUANGZHOU_WUSUO_MIN_RESPONSE_CMD_NUM*TH_GUANGZHOU_WUSUO_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_GUANGZHOU_WUSUO_MAX_RESPONSE_CMD_LEN		(TH_GUANGZHOU_WUSUO_MAX_RESPONSE_CMD_NUM*TH_GUANGZHOU_WUSUO_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD	     (8+5)   // CMD(8 byte)+DATA(5 byte)    
#define TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_REPLY	     (8+5)   // REPLY(13 byte)  
#define TH_GUANGZHOU_WUSUO_MAX_COM_READ_CMD	         (7)     // CMD(7 byte)  
#define TH_GUANGZHOU_WUSUO_MAX_COM_READ_REPLY	     (23)    // REPLY(4*5+3=23 byte)  
#define TH_GUANGZHOU_WUSUO_MAX_COM_RUN_CMD	         (10)   // CMD(10 byte)    
#define TH_GUANGZHOU_WUSUO_MAX_COM_STOP_CMD	         (11)   // CMD(11 byte)     
#define TH_GUANGZHOU_WUSUO_MAX_COM_MODE_CMD	         (15)   // CMD(15 byte)    

#define HUMI_INTERVALTIME	     (6000)   // ʪ�ȼ��ʱ��6�룬����û�����Զ��л��¶�ģʽ

class TH_GUANGZHOU_WUSUO
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
