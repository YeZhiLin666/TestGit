/*****************************************************************************
// �����ļ�      :TC_FUTURE_FDCP91.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TC_FUTURE_FDCP91.h **/

#ifndef TC_FUTURE_FDCP91_H
#define TC_FUTURE_FDCP91_H 1


#define TC_FDCP91_MIN_RESPONSE_CMD_LEN		2*TC_FDCP91_MAX_COM_READ_REPLY    // 15  // ��Ӧ����С��������
#define TC_FDCP91_MAX_RESPONSE_CMD_LEN		6*TC_FDCP91_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // ��Ӧ����󳤶�����

// ���ͳ��Ⱥͽ��ճ��ȶ��壺
#define TC_FDCP91_MAX_COM_WRITE_CMD	         (2+6)    // CMD(6 byte)+CRC(2 byte)        
#define TC_FDCP91_MAX_COM_WRITE_REPLY	     (2+6)    // REPLY(116byte)+CRC(2 byte)    
#define TC_FDCP91_MAX_COM_READ_CMD	         (2+6)    // CMD(3 byte)+CRC(2 byte)         
#define TC_FDCP91_MAX_COM_READ_REPLY	     (2+5)    // REPLY(116byte)+CRC(2 byte)    

#define TC_FDCP91_MAX_COM_STOP_CMD	         (2+6)    // CMD(6 byte)+CRC(2 byte)     
#define TC_FDCP91_MAX_COM_RUN_CMD	         (2+6)    // CMD(6 byte)+CRC(2 byte)     

#define TC_FDCP91_WRITE_CMD_POINT_VAlUE	 (4)    // Write Value point (4)       
#define TC_FDCP91_READ_REPLY_POINT_VAlUE	 (3)    // Read REPLY Value point (3)   

//  Future Design Controls P91�¿������к�ֹͣ���������д����һ�¡�

//  �ر�ע�⣺��ʹ�����к�ֹͣ���������д���һ��ʱ�����Ʊ����޸ġ���������塣
//#define COM_RUNANDSTOP_CMD_DIFFER	     (1) // ��һ��ʱ,������Open this define for Difference.

class TC_FDCP91
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};



#endif
