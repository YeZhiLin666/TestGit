/*****************************************************************************
// �����ļ�      :TC_SAMWONTECH_TEMP880.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TC_SAMWONTECH_TEMP880_H
#define TC_SAMWONTECH_TEMP880_H 1



//  ��Ӧ����������
#define TC_TEMP880_MIN_RESPONSE_CMD_NUM        1 // 3֡   // ��Ӧ����С��������
#define TC_TEMP880_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TC_TEMP880_MIN_RESPONSE_CMD_LEN		(TC_TEMP880_MIN_RESPONSE_CMD_NUM*TC_TEMP880_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TC_TEMP880_MAX_RESPONSE_CMD_LEN		(TC_TEMP880_MAX_RESPONSE_CMD_NUM*TC_TEMP880_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����


#define TC_TEMP880_MAX_COM_WRITE_CMD	     (1+4+18)    // CMD(18 byte)+HEAD(1 byte)+End(4 byte)  
#define TC_TEMP880_MAX_COM_WRITE_REPLY	     (1+4+8)     // REPLY(8 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP880_MAX_COM_READ_CMD	         (1+4+13)    // CMD(13 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP880_MAX_COM_READ_REPLY	     (1+4+13)    // REPLY(13 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP880_MAX_COM_RUN_CMD	         (1+4+28)    // CMD(28 byte)+HEAD(1 byte)+End(4 byte)       
#define TC_TEMP880_MAX_COM_STOP_CMD	         (1+4+28)    // CMD(28 byte)+HEAD(1 byte)+End(4 byte)      

#define TC_TEMP880_WRITE_CMD_POINT_TEMPVAlUE	 (15)    // Write Value point (15)        
#define TC_TEMP880_READ_REPLY_POINT_TEMPVAlUE	 (10)    // Read REPLY Value point (10)   
#define TC_TEMP880_WRITE_CMD_POINT_HUMIVAlUE	 (25)    // Write Value point (25)         
#define TC_TEMP880_READ_REPLY_POINT_HUMIVAlUE	 (15)    // Read REPLY Value point (15)  

class TC_TEMP880
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);


    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);
};

//bool TC_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);

/*
void TC_TEMP880_SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
void TC_TEMP880_SendtwentyoneCmd(unsigned char ChanNo);
void TC_TEMP880_SendeightCmd(unsigned char ChanNo);
void TC_TEMP880_SendtwentythreeCmd(unsigned char ChanNo);
void TC_TEMP880_SendCheckStatusCmd(unsigned char ChanNo);
void TC_TEMP880_SendCheckSetpointCmd(unsigned char ChanNo);
void TC_TEMP880_ExecStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
*/


#endif
