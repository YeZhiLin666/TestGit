#ifndef TH_HITACHI_EC15MHP_H
#define TH_HITACHI_EC15MHP_H

#define TH_HITACHI_EC15MHP_MIN_RESPONSE_CMD_LEN		6
#define TH_HITACHI_EC15MHP_MAX_RESPONSE_CMD_LEN		40

class TH_HITACHI_EC15MHP
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
		static char* itoa(int num,char* str,int radix);
};

#endif
