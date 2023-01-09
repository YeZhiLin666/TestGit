#ifndef TH_9700S_2C_H
#define TH_9700S_2C_H

#define TH_9700S_2C_MIN_RESPONSE_CMD_LEN		17
#define TH_9700S_2C_MAX_RESPONSE_CMD_LEN		40

class TH_9700S_2C
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};

#endif
