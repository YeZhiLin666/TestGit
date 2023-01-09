/*****************************************************************************
// �����ļ�      :TC_TM221CE24T.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2019.09.11
*****************************************************************************/
/** TC_TM221CE24T.h **/

#ifndef TC_TM221CE24T_H
#define TC_TM221CE24T_H 1

//  ��Ӧ����������
#define TC_TM221CE24T_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TC_TM221CE24T_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TC_TM221CE24T_MIN_RESPONSE_CMD_LEN		(TC_TM221CE24T_MIN_RESPONSE_CMD_NUM*TC_TM221CE24T_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TC_TM221CE24T_MAX_RESPONSE_CMD_LEN		(TC_TM221CE24T_MAX_RESPONSE_CMD_NUM*TC_TM221CE24T_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����




// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TC_TM221CE24T_MAX_COM_WRITE_CMD	     (11+2)   // CMD(11 byte)+CRC(2 byte)    
#define TC_TM221CE24T_MIN_COM_WRITE_CMD	     (9+2)   // CMD(9 byte)+CRC(2 byte)

#define TC_TM221CE24T_MAX_COM_WRITE_REPLY	     (6+2)   // REPLY(6 byte)+CRC(2 byte)   
#define TC_TM221CE24T_MAX_COM_READ_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TC_TM221CE24T_MAX_COM_READ_REPLY	     	 (7+2)   // REPLY(7 byte)+CRC(2 byte)  
#define TC_TM221CE24T_MAX_COM_RUN_CMD	         (9+2)   // CMD(9 byte)+CRC(2 byte)    
#define TC_TM221CE24T_MAX_COM_STOP_CMD	         (9+2)   // CMD(9 byte)+CRC(2 byte)    


class TC_TM221CE24T
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
