/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TH_SAMWONTECH_TEMI2500_H
#define TH_SAMWONTECH_TEMI2500_H 1



//  ��Ӧ����������
#define TH_TEMI2500_MIN_RESPONSE_CMD_NUM        3 // 3֡   // ��Ӧ����С��������
#define TH_TEMI2500_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_TEMI2500_MIN_RESPONSE_CMD_LEN		(TH_TEMI2500_MIN_RESPONSE_CMD_NUM*TH_TEMI2500_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_TEMI2500_MAX_RESPONSE_CMD_LEN		(TH_TEMI2500_MAX_RESPONSE_CMD_NUM*TH_TEMI2500_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����


#define TH_TEMI2500_MAX_COM_WRITE_CMD	     (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_WRITE_REPLY	     (1+2+10)    // REPLY(10 byte)+HEAD(1 byte)+End(2 byte)      
#define TH_TEMI2500_MAX_COM_READ_CMD	     (1+2+20)    // CMD(20 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_READ_REPLY	     (1+2+20)    // REPLY(20 byte)+HEAD(1 byte)+End(2 byte)      
#define TH_TEMI2500_MAX_COM_RUN_CMD	         (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_STOP_CMD	     (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        

#define TH_TEMI2500_WRITE_CMD_POINT_TEMPVAlUE	 (15)    // Write Value point (15)       
#define TH_TEMI2500_READ_REPLY_POINT_TEMPVAlUE	 (10)    // Read REPLY Value point (10)   
#define TH_TEMI2500_WRITE_CMD_POINT_HUMIVAlUE	 (25)    // Write Value point (25)         
#define TH_TEMI2500_READ_REPLY_POINT_HUMIVAlUE	 (15)    // Read REPLY Value point (15)   

/*
//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
#define COM_RUN4READ_CMD_DIFFER	     (1) // ͣ��д���������һ��ʱ,������Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // �ȿ����¶��ֿ���ʪ��ʱ��Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // ʹ�ö����ͬͨ����ʱ������ʾ1��4������Ĭ��Ϊ1��ͨ����Open this define multi channel.
*/

class TH_TEMI2500
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};

//bool TH_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);




#endif
