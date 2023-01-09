/*****************************************************************************
// �����ļ�      :TH_HARDING_TPC7062K.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2014.04.08
// ����ض��豸  :ARBIN������������Ƽ�TPC7062K˵��
*****************************************************************************/
/** TC_HARDING_TPC7062K.h **/

#ifndef TH_HARDING_TPC7062K_H
#define TH_HARDING_TPC7062K_H 1




//  ��Ӧ����������
#define TH_TPC7062K_MIN_RESPONSE_CMD_NUM        3 // 3֡   // ��Ӧ����С��������
#define TH_TPC7062K_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_TPC7062K_MIN_RESPONSE_CMD_LEN		(TH_TPC7062K_MIN_RESPONSE_CMD_NUM*TH_TPC7062K_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_TPC7062K_MAX_RESPONSE_CMD_LEN		(TH_TPC7062K_MAX_RESPONSE_CMD_NUM*TH_TPC7062K_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����



#define TH_TPC7062K_MAX_COM_MODE_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_MODE_REPLY	     (1+2+8)     // REPLY(8 byte)+HEAD(1 byte)+End(2 byte)   
#define TH_TPC7062K_MAX_COM_WRITE_CMD	     (1+2+18)    // CMD(18 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_WRITE_REPLY	     (1+2+8)     // REPLY(8 byte)+HEAD(1 byte)+End(2 byte)          
#define TH_TPC7062K_MAX_COM_READ_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_READ_REPLY	     (1+2+32)    // REPLY(32 byte)+HEAD(1 byte)+End(2 byte)     
#define TH_TPC7062K_MAX_COM_RUN_CMD	         (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_STOP_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)         

#define TH_TPC7062K_WRITE_CMD_POINT_TEMPVAlUE	 (9)    // Write TEMPVAlUE Value point (9)        
#define TH_TPC7062K_READ_REPLY_POINT_TEMPVAlUE	 (8)     // Read REPLY TEMPVAlUE Value point (11-3)    
#define TH_TPC7062K_WRITE_CMD_POINT_HUMIVAlUE	 (13)    // Write HUMIVAlUE Value point (13)        
#define TH_TPC7062K_READ_REPLY_POINT_HUMIVAlUE	 (16)    // Read REPLY HUMIVAlUE Value point (19-3)   

/*
//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
#define COM_RUN4READ_CMD_DIFFER	     (1) // ͣ��д���������һ��ʱ,������Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // �ȿ����¶��ֿ���ʪ��ʱ��Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // ʹ�ö����ͬͨ����ʱ������ʾ1��4������Ĭ��Ϊ1��ͨ����Open this define multi channel.
*/

class TH_TPC7062K
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);
};

#endif
