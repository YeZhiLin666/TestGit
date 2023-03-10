/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TC_SIMPAC_H
#define TC_SIMPAC_H 1

//  回应包长度限制
#define TC_SIMPAC_MIN_RESPONSE_CMD_LEN		(68)
#define TC_SIMPAC_MAX_RESPONSE_CMD_LEN		(68)



#define TC_SIMPAC_MAX_COM_WRITE_CMD	     (37)
#define TC_SIMPAC_MAX_COM_WRITE_REPLY	   (82)
#define TC_SIMPAC_MAX_COM_READ_CMD	       (5)
#define TC_SIMPAC_MAX_COM_READ_REPLY	     (68)

#define TC_SIMPAC_MAX_COM_STOP_CMD	       (37)
#define TC_SIMPAC_MAX_COM_RUN_CMD	       (37)

#define TC_SIMPAC_WRITE_CMD_POINT_VAlUE	 (4)
#define TC_SIMPAC_READ_REPLY_POINT_VAlUE	 (6)

class TC_SIMPAC
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};

//bool TH_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);




#endif
