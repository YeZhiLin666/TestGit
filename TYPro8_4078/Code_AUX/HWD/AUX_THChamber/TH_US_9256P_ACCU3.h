#ifndef _TH_US_9256P_ACCU3_H
#define _TH_US_9256P_ACCU3_H

#define TH_US_9256P_ACCU3_MIN_RESPONSE_CMD_LEN		7
#define TH_US_9256P_ACCU3_MAX_RESPONSE_CMD_LEN		40

class TH_US_9256P_ACCU3
{
private:
		static unsigned char chamber_state;
	
public:
		static unsigned char get_state(void);
		static void set_state(unsigned char state);
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};

#endif