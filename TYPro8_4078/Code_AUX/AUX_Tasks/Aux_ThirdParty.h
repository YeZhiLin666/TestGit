


#ifndef _AUX_THIRD_PARTY_H_
#define _AUX_THIRD_PARTY_H_
#define _AUXTHIRD_PARTY_MSG_MAX_NUM_    64


///////////////////////////////////命令结构等定义
//第三方状态status enum
enum
{
    AUXTHIRD_PARTY_STATUS_INACTIVE = 0x10,
    AUXTHIRD_PARTY_STATUS_IDLE,                //0x11
    AUXTHIRD_PARTY_STATUS_TRAN,              //0x12
    AUXTHIRD_PARTY_STATUS_OUTPUT,          //0x13
};


enum
{
    AUXTHIRD_PARTY_SET_3_RD_MODE = 0x01,    //进入
    AUXTHIRD_PARTY_AUX_TEMPERATURE,
    AUXTHIRD_PARTY_AUX_VOLTAGE,
    AUXTHIRD_PARTY_CLR_3_RD_MODE,    //退出    0x04

    //AUX增加

};

typedef struct
{

    float Reserved2;    		 //预留
} st_AUXThird_Party_ctl_Ex;    //第三方命令扩展


typedef struct
{
    unsigned char cmd;    //控制命令

    st_AUXThird_Party_ctl_Ex ctl_cmd_ex;    //扩展命令
} st_AUXThird_Party_CMD;

//send msg struct
typedef struct
{
    unsigned char TaskID;    //just  来源Task

    unsigned char chn;
    //the pointer or RAM
    st_AUXThird_Party_CMD st_Auxthird_party_cmd;
} st_AUXThird_Party_Msg;

//查询句柄
typedef struct
{
    unsigned short handleID;
    unsigned char type;    //种类,CC,CV or others
} st_AUXThird_Handle;

class AuxthirdParty
{
public:
    static unsigned short Priority;
    static unsigned short TaskID;			//  should be a definition which positioning this task in OS_Task[]




public:
    static void Init(void);
    static void DoWork(void);
    /*
    1 由parse发送到本task  之后解析其msg内容
    2 由其发送的msg内的cmd做状态机处理  调用一次性时序
    3 更改自身的状态机以供parse查询
    */
    static void EndTask(void);
    static unsigned char sendMsgToMe(const st_AUXThird_Party_Msg * msg);    //压入msg
    static st_AUXThird_Handle st_Auxthird_handle[MAX_AUXCH_PER_AUXMCU];
    static signed char getStatus(unsigned char chn);

    static void SoftWare_WatchDog(void);   //上位没有响应时第三方状态处理 dirui 0828

private:
    static unsigned char m_ActualAuxChannel;
    static unsigned char m_Auxstatus[MAX_AUXCH_PER_AUXMCU];    //状态机    8bit
    static st_AUXThird_Party_Msg st_Auxthird_party_msg[MAX_AUXCH_PER_AUXMCU];
    static unsigned char m_AuxmsgFlag[MAX_AUXCH_PER_AUXMCU];
    //static st_Third_Handle st_Auxthird_handle[MAX_AUXCH_PER_AUXMCU];



    static void processMSG(unsigned char chn);    //第一步处理msg
    static void processFSM(unsigned char chn);    //第二部处里状态机


    static unsigned char setStatus(unsigned char chn,unsigned char status);    //更改此通道的状态



    static void clearMSG(char msgID);


};






#endif
