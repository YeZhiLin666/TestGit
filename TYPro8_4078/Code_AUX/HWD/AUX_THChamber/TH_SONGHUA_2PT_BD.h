/*****************************************************************************
// �����ļ�      :TH_SONGHUA_2PT_BD.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2018.12.04
*****************************************************************************/
/** TH_SONGHUA_2PT_BD.h **/



#ifndef TH_SONGHUA_2PT_BD_H
#define TH_SONGHUA_2PT_BD_H 1

//  ��Ӧ����������
#define TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_LEN		(TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_NUM*TH_SONGHUA_2PT_BD_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������

// ���ͳ��Ⱥͽ��ճ��ȶ��壺 
#define TH_SONGHUA_2PT_BD_MAX_COM_READ_REPLY	     (7)    // REPLY(6 byte)+DATA(16 byte)   


class TH_SONGHUA_2PT_BD
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
private:
	static unsigned char m_RunState;
};


#endif
