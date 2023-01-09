/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TC_BIA_CLIMATIC_H
#define TC_BIA_CLIMATIC_H

#define TC_BIA_CLIMATIC_MIN_RESPONSE_CMD_LEN 11


class TC_BIA_CLIMATIC
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
private:
		static bool IsNeedSendChamber1RunCmd2;
		static bool IsNeedSendChamber2RunCmd2;
		static bool IsNeedSendChamber1StopCmd2;
		static bool IsNeedSendChamber2StopCmd2;
};

#endif
