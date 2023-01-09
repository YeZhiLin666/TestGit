/*****************************************************************************
// �����ļ�      :TH_CSZ_EZT570I.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_CSZ_EZT570I.h **/

#ifndef TH_CSZ_EZT570I_H
#define TH_CSZ_EZT570I_H 1

//  ��Ӧ����������
#define TH_EZT570I_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_EZT570I_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_EZT570I_MIN_RESPONSE_CMD_LEN		(TH_EZT570I_MIN_RESPONSE_CMD_NUM*TH_EZT570I_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_EZT570I_MAX_RESPONSE_CMD_LEN		(TH_EZT570I_MAX_RESPONSE_CMD_NUM*TH_EZT570I_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TH_EZT570I_MAX_COM_WRITE_CMD	     (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_EZT570I_MAX_COM_WRITE_REPLY	     (6+2)   // REPLY(6 byte)+CRC(2 byte)   
#define TH_EZT570I_MAX_COM_READ_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_EZT570I_MAX_COM_READ_REPLY	     (5+2)   // REPLY(5 byte)+CRC(2 byte)  
#define TH_EZT570I_MAX_COM_RUN_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_EZT570I_MAX_COM_STOP_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    


class TH_EZT570I
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
