/*****************************************************************************
// �����ļ�      :TH__VOETSCH.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TH_VOETSCH_H
#define TH_VOETSCH_H 1

//  ��Ӧ����������
#define TH_VOETSCH_MIN_RESPONSE_CMD_LEN		2*TH_VOETSCH_MAX_COM_READ_REPLY    // 15  // ��Ӧ����С��������
#define TH_VOETSCH_MAX_RESPONSE_CMD_LEN		6*TH_VOETSCH_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // ��Ӧ����󳤶�����



#define TH_VOETSCH_MAX_COM_WRITE_CMD	     (87)    // CMD(85 byte)+Head(2 byte)       
#define TH_VOETSCH_MAX_COM_WRITE_REPLY	     (82)    // REPLY(81byte)+Head(1 byte)    
#define TH_VOETSCH_MAX_COM_READ_CMD	         (2+3)   // CMD(3 byte)+Head(2 byte)        
#define TH_VOETSCH_MAX_COM_READ_REPLY	     (131)    // REPLY(130byte)+Head(1 byte)    

#define TH_VOETSCH_MAX_COM_STOP_CMD	         (87)    // CMD(85 byte)+Head(2 byte)      
#define TH_VOETSCH_MAX_COM_RUN_CMD	         (2+3)    // CMD(3 byte)+Head(2 byte)       

#define TH_VOETSCH_WRITE_CMD_POINT_VAlUE	 (4)    // Write Value point (4)       
#define TH_VOETSCH_READ_REPLY_POINT_VAlUE	 (6)    // Read REPLY Value point (6)   

/*
//  �����¿������к�ֹͣ���������д����һ�¡�
//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
#define COM_RUN4READ_CMD_DIFFER	     (1) // ͣ��д���������һ��ʱ,������Open this define for Difference.
//#define COM_TEMP4HUMI_CMD	         (1) // �ȿ����¶��ֿ���ʪ��ʱ��Open this define temperature and humi.
//#define COM_CMD_CHANNEL_DIFFER       (1) // ʹ�ö����ͬͨ����ʱ������ʾ1��4������Ĭ��Ϊ1��ͨ����Open this define multi channel.
*/

class TH_VOETSCH
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
