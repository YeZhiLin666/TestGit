/*****************************************************************************
// �����ļ�      :TC_EUROTHERM_2204E.H    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TC_EUROTHERM_2204E.h **/

#ifndef TC_EUROTHERM_2204E_H
#define TC_EUROTHERM_2204E 1


#define TC_2204E_MIN_RESPONSE_CMD_LEN		15
#define TC_2204E_MAX_RESPONSE_CMD_LEN		40


#define TC_2204E_MAX_COM_WRITE_CMD	     (6+2)   // CMD(6 byte)+CRC(2 byte)  
#define TC_2204E_MAX_COM_WRITE_REPLY	 (6+2)   // REPLY(6 byte)+CRC(2 byte) 
#define TC_2204E_MAX_COM_READ_CMD	     (6+2)   // CMD(6 byte)+CRC(2 byte) 
#define TC_2204E_MAX_COM_READ_REPLY	     (5+2)   // REPLY(5 byte)+CRC(2 byte) 

class TC_2204E
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
};


#endif
