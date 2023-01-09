/*****************************************************************************
// �����ļ�      :TH_VOETSCH_VC_L4006.h    �����¿����и��ִ���ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TC_SigmaSystemsC4_H
#define TC_SigmaSystemsC4_H 1

//  ��Ӧ����������
#define TC_SigmaSystemsC4_MIN_RESPONSE_CMD_LEN		(68)
#define TC_SigmaSystemsC4_MAX_RESPONSE_CMD_LEN		(68)



#define TC_SigmaSystemsC4_MAX_COM_WRITE_CMD	     (37)
#define TC_SigmaSystemsC4_MAX_COM_WRITE_REPLY	   (82)
#define TC_SigmaSystemsC4_MAX_COM_READ_CMD	       (5)
#define TC_SigmaSystemsC4_MAX_COM_READ_REPLY	     (68)

#define TC_SigmaSystemsC4_MAX_COM_STOP_CMD	       (37)
#define TC_SigmaSystemsC4_MAX_COM_RUN_CMD	       (37)

#define TC_SigmaSystemsC4_WRITE_CMD_POINT_VAlUE	 (4)
#define TC_SigmaSystemsC4_READ_REPLY_POINT_VAlUE	 (6)

class TC_SigmaSystemsC4
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
private:
		static bool SetModeFlag;
		static bool StopCompressorFlag;
};

//bool TH_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);




#endif
