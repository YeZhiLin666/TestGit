/*****************************************************************************
// �����ļ�      :TH_CLCD_9700.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC4078 AUX-MPU board����
// ��д����      :2015.08.13
// ��д��        �������
*****************************************************************************/
/** TH_CLCD_9700.h **/

#ifndef TH_CLCD_9700_H
#define TH_CLCD_9700_H 1



//  ��Ӧ����������
#define TH_CLCD_9700_MIN_RESPONSE_CMD_NUM        3 // 3֡   // ��Ӧ����С��������
#define TH_CLCD_9700_MAX_RESPONSE_CMD_NUM        9 // 9֡   // ��Ӧ�������������
#define TH_CLCD_9700_MIN_RESPONSE_CMD_LEN		(TH_CLCD_9700_MIN_RESPONSE_CMD_NUM*TH_CLCD_9700_MAX_COM_READ_REPLY)    // 3֡*N  // ��Ӧ����С��������
#define TH_CLCD_9700_MAX_RESPONSE_CMD_LEN		(TH_CLCD_9700_MAX_RESPONSE_CMD_NUM*TH_CLCD_9700_MAX_COM_READ_REPLY)    // 9֡*N  // ��Ӧ����󳤶�����


#define TH_CLCD_9700_MAX_COM_WRITE_CMD	     		(35+1)    // CMD(35 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_WRITE_REPLY	     	(10+2)    // REPLY(10 byte)+End(2 byte)      
#define TH_CLCD_9700_MAX_COM_READ_CMD	     		(8+1)    // CMD(8 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_READ_REPLY	     		(78+2)    // REPLY(78 byte)+End(2 byte)      
#define TH_CLCD_9700_MAX_COM_RUN_CMD	         	(11+1)    // CMD(11 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_STOP_CMD_1	     		(11+1)    // CMD(11 byte)+End(1 byte)
#define TH_CLCD_9700_MAX_COM_STOP_CMD_2	     		(11+1)    // CMD(11 byte)+End(1 byte)


/*
//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
#define COM_RUN4READ_CMD_DIFFER	     (1) // ͣ��д���������һ��ʱ,������Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // �ȿ����¶��ֿ���ʪ��ʱ��Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // ʹ�ö����ͬͨ����ʱ������ʾ1��4������Ĭ��Ϊ1��ͨ����Open this define multi channel.
*/

class TH_CLCD_9700
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static short int asc2int(unsigned char asc1,unsigned char asc2,unsigned char asc3,unsigned char asc4);
    static void AddTemp(short int * pValue, unsigned char* str);
    static void AddCheck(unsigned int bits, unsigned char* str);

    //bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
};



#endif
