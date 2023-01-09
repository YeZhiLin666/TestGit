/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TH_SIMENSE_S7_200_H
#define TH_SIMENSE_S7_200_H

#define TH_SIMENSE_S7_200_MIN_RESPONSE_CMD_LEN 9


class TH_SIMENSE_S7_200
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
