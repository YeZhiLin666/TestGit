#ifndef TC_SIEMENS_S7_XC1001A_SAIA_H
#define TC_SIEMENS_S7_XC1001A_SAIA_H

#define TC_SIEMENS_S7_XC1001A_SAIA_MIN_RESPONSE_CMD_LEN		13
#define TC_SIEMENS_S7_XC1001A_SAIA_MAX_RESPONSE_CMD_LEN		40

class TC_SIEMENS_S7_XC1001A_SAIA
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);

private:
	static float UcharToFloat(unsigned char *buf);
	static void FloatToUchar(unsigned char *buf,float value);	
};

#endif
