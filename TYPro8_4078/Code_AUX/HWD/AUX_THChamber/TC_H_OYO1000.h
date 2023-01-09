/*****************************************************************************
// �����ļ�      :TC_H_OYO1000.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2022.10.10
*****************************************************************************/
/** TC_H_OYO1000.h **/

#ifndef TC_H_OYO1000_H
#define TC_H_OYO1000_H

#define TC_H_OYO1000_MIN_RESPONSE_CMD_LEN 7

class TC_H_OYO1000
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
