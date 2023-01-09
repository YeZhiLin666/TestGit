#ifndef TH_MEMMERTGMBH_RS232_H
#define TH_MEMMERTGMBH_RS232_H

#define TH_MEMMERTGMBH_RS232_MIN_RESPONSE_CMD_LEN		4
#define TH_MEMMERTGMBH_RS232_MAX_RESPONSE_CMD_LEN		18

#define TH_MEMMERTGMBH_RS232_START_CMD_LEN 15
#define TH_MEMMERTGMBH_RS232_STOP_CMD_LEN 15
#define TH_MEMMERTGMBH_RS232_SET_TMEP_CMD_LEN 18
#define TH_MEMMERTGMBH_RS232_SET_HUMI_CMD_LEN 14


#define TH_MEMMERTGMBH_RS232_MAX_COM_WRITE_REPLY	     (4)       
#define TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD	     (10)       
#define TH_MEMMERTGMBH_RS232_MAX_COM_READ_REPLY	     (12)    

class TH_MEMMERTGMBH_RS232
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
