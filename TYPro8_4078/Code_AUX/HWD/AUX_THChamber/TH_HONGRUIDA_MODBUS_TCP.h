/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TH_HONGRUIDA_MODBUS_TCP_H
#define TH_HONGRUIDA_MODBUS_TCP_H

#define TH_HONGRUIDA_MODBUS_TCP_MIN_RESPONSE_CMD_LEN 11


class TH_HONGRUIDA_MODBUS_TCP
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
private:
		static unsigned char m_RecvDataType;
};

#endif
