/*****************************************************************************
// �����ļ�      :TC_SUN_EC1X.h    �����¿����и��ֿ�������ͷ�ļ�
// ��д����      :2019.05.14
*****************************************************************************/

#define TC_SUN_EC1X_MIN_RESPONSE_CMD_LEN		30
#define TC_SUN_EC1X_MAX_RESPONSE_CMD_LEN		90

class TC_SUN_EC1X
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};
