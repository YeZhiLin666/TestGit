//第三方命令
//comment out chen 20130423 更新

#ifndef _THIRD_PARTY_H_
#define _THIRD_PARTY_H_

// #define _THIRD_PARTY_MSG_MAX_NUM_    16

///////////////////////////////////命令结构等定义
//第三方状态status enum
enum
{
    THIRD_PARTY_STATUS_INACTIVE = 0x10,
    THIRD_PARTY_STATUS_IDLE,                //0x11
    THIRD_PARTY_STATUS_TRAN,              //0x12
    THIRD_PARTY_STATUS_OUTPUT,          //0x13
};

//独立第三方控制命令与自动校准模块第三方控制命令是相同的
//comment out chen 20130516
//cmd enum
//不同的就是IV可以在output下切换，而校准板output意味着有电流这时候应该是不能带电切换的
//如果将来能保证的话，可以做成output下进入tran
//comment out chen 20130520    预留更新
enum
{
    THIRD_PARTY_SET_3_RD_MODE = 0x01,    //进入
    THIRD_PARTY_CC,                        //恒流
    THIRD_PARTY_CV,                        //恒压
    THIRD_PARTY_CP,                        //恒功率
    THIRD_PARTY_CL,                        //恒负载
    THIRD_PARTY_SETCLAMPV,          //钳压
    THIRD_PARTY_STOP,                    //停止输出

    THIRD_PARTY_IV_SET,                  //IVset输出

    THIRD_PARTY_CLR_3_RD_MODE,    //退出    0x09

    //AUX增加
    THIRD_PARTY_AUX_TEMPERATURE,
    THIRD_PARTY_AUX_VOLTAGE,
};

//CALI 校准模式
enum
{
    //comment out chen 20130423    IV need
    THIRD_PARTY_MANUAL = 0x10,   // 手动
    THIRD_PARTY_AUTO = 0X20,     // 自动校准
    THIRD_PARTY_AUTO_CALI_HP_3RD = 0X21,          //HP表模式
    THIRD_PARTY_AUTO_CALI_SELF_3RD = 0X22,        //校准自身
    THIRD_PARTY_AUTO_CALI_ADC_3RD = 0X23,       //ADC模式

    THIRD_PARTY_AUTO_CALI_OFF,    //建议保留
};

//电池module enum 电池模式
// enum
// {
// 	AUTO_CALI_CELL_WORK_MODULE_NONE = 0x05, //电池维护
// 	AUTO_CALI_CELL_WORK_MODULE_IV,                   //穿入IV
// 	AUTO_CALI_CELL_WORK_MODULE_BY_PASS,         //旁路
// 	AUTO_CALI_CELL_WORK_MODULE_DISCHARGE,    //放电
// 	AUTO_CALI_CELL_WORK_MODULE_CHARGE,         //充电
// };
//COM V++enum 公共模式
// enum
// {
// 	AUTO_CALI_COM_MODULE = 0x15,
// 	AUTO_CALI_COM_VPP,
// };

//IV stuff enum
enum
{
    DISPOSABLE_MSG_CMD_OFF_TO_CC = 0xB0,
    DISPOSABLE_MSG_CMD_CC_TO_CC,
    DISPOSABLE_MSG_CMD_CC_TO_OFF,
    DISPOSABLE_MSG_CMD_OFF_TO_CV,    //恒压输出
    DISPOSABLE_MSG_CMD_CV_TO_CV,
    DISPOSABLE_MSG_CMD_CV_TO_OFF
};

//chn status
enum
{
    CHANNEL_INFO_CLOSE =0x01,
    CHANNEL_INFO_CC,        //恒流
    CHANNEL_INFO_CV,        //恒压
    CHANNEL_INFO_CP,        //恒功率
    CHANNEL_INFO_OPEN,
};

//电压module enum 电压模式
enum
{
    AUTO_CALI_VOLTAGE_MODULE_NONE = 0x70, // 电压无特定情况
    AUTO_CALI_VOLTAGE_MODULE_HARDWARE = 0x71, // 电压有环路
    AUTO_CALI_VOLTAGE_MODULE_DIGIT = 0X72,      // 电压没环路，数字控制
    AUTO_CALI_VOLTAGE_MODULE_POWER = 0X73,    // 电压有用电源
    AUTO_CALI_VOLTAGE_MODULE_RESISTANCE = 0X74,      // 电压有用电阻
};

typedef struct
{
    unsigned char BoardType;    //在一次set-3rd时激活即可
    //当普通校准通道时，THIRD_PARTY_MANUAL
    //当自动校准通道时，预留

    unsigned char Battery_Module;    //电池模式
    unsigned char COM_Module;      //V++/COM
    unsigned char CaliType;    //电流，电压 钳压

    unsigned char I_Range;    //量程
    unsigned char V_Range;    //电压量程(预留)
    unsigned char ctrl_Type;    //控制类型  预留  comment out chen20130423

    unsigned char isVDAC;    //电压DAC是否输出
    unsigned char Reserved1;  //预留

    float IDAC_Set;            //电流DAC值
    float VDAC_Set;           //电压DAC值

    float Reserved2;    		 //预留
} st_Third_Party_ctl_Ex;    //第三方命令扩展

typedef struct
{
    unsigned char cmd;    //控制命令

    st_Third_Party_ctl_Ex ctl_cmd_ex;    //扩展命令
} st_Third_Party_CMD;

//send msg struct
typedef struct
{
    unsigned char TaskID;    //just  来源Task

    unsigned char chn;
    //the pointer or RAM
    st_Third_Party_CMD st_third_party_cmd;
} st_Third_Party_Msg;

//查询句柄
typedef struct
{
    unsigned short handleID;
    unsigned char type;    //种类,CC,CV or others
} st_Third_Handle;

///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////
#ifndef _AUTO_CALI_THIRD_PARTY_

//类
class thirdParty
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]


    //是否改成支持多通道

    static unsigned char UnsafeLockSwitch[MAXCHANNELNO];                       //非安全状态锁定模式开关  add by qjm 20140822

    //static MsTime WatchDog;  //上位没有响应时第三方状态处理 dirui 0828
    //static MsTime TimeTickForWatchDog;   //20130904,deleted by hepeiqing,we do watchdog in ethernetparse
public:
    static void Init(void);
    static void DoWork(void);
    /*
    1 由parse发送到本task  之后解析其msg内容
    2 由其发送的msg内的cmd做状态机处理  调用一次性时序
    3 更改自身的状态机以供parse查询
    */
// 	static void EndTask(void);
    static unsigned char sendMsgToMe(const st_Third_Party_Msg * st_third_party_msg);    //压入msg
    static signed char getStatus(unsigned char chn);

    static void SoftWare_WatchDog(void);   //上位没有响应时第三方状态处理 dirui 0828

private:
    static unsigned char m_status[MAXCHANNELNO];    //状态机    8bit
    static st_Third_Party_Msg st_third_party_msg[MAXCHANNELNO];
    static unsigned char m_MsgValid[MAXCHANNELNO];
    static st_Third_Handle st_third_handle[MAXCHANNELNO];

    static unsigned char manu_auto_Flag[MAXCHANNELNO];

  static float GetNewCurrentValue_FineTuning(unsigned char chn);
    static void processMSG(unsigned char chn);    //第一步处理msg
    static void processFSM(unsigned char chn);    //第二部处里状态机

    static unsigned char setStatus(unsigned char chn,unsigned char status);    //更改此通道的状态
    static unsigned short stuffDisposableMsg(unsigned char chn,unsigned char type);    //填充并调用一次性时序引擎
    static int processDisposableHandle(unsigned char chn);

// 	static void clearMSG(char msgID);

    //lock sch
    //		static void lockSCH(unsigned char chn);
    //		static void unlockSCH(unsigned char chn);

    static unsigned char AutoCaliFlag;
};

#endif
///////////////////////////////////////////////////////IV Module///////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////Auto Cali///////////////////////////////////////////////////////
#ifdef _AUTO_CALI_THIRD_PARTY_

class thirdParty
{
    //成员变量
public:
    static UInt16 Priority;    //优先级
    static UInt16 TaskID;	  //THIRDPARTY_TASK
    static unsigned long Dac_set_value;    //赋给自动校准DAC得值，从msg内获得，交付给Auto_Base
private:
    static unsigned char m_status;    //状态机 只有单chn 0使用
    static unsigned char msgFlag;    //1bit即可
    static unsigned char logicStatus;    //连接自动校准驱动的flag
    static st_Third_Party_Msg m_msg;

    static unsigned char Auto_Board_Work_Module;    //当前工作模式
    static unsigned char m_BoardType;    //IV通道板类型    预留
    static unsigned char m_Battery_Work_Module;    //电池状态
    static unsigned char COM_V_Module;    //COM V++状态
    static unsigned char Reserved1;       //预留1
    static unsigned char Reserved2;       //预留2

    static MsTime m_timer;

    static float auto_dac_out[4];

    //接口函数
public:
    static void Init(void);
    static void DoWork(void);    //主函数
    /*
    1 接收命令，按照当前状态决定是否响应
    2 执行响应过程，比如Tran阶段
    */
    static void EndTask(void);

    static char sendMsgToMe(st_Third_Party_Msg * st_third_party_msg);    //压入msg

    static unsigned char getStatus(unsigned char chn);    //获取状态
private:

    static unsigned char ProcessFSM(void);    //根据状态机处理msg内CMD
    static unsigned char ProcessLogic(void);
    static unsigned char StuffLogicStatus(unsigned char type);    //提前预备处理

    static unsigned char ProcessAutoDrive(unsigned char type);    //驱动自动校准板
    static unsigned char ProcessAutoDirveBase(void);    //驱动特殊模式

    static unsigned char SetStatus(unsigned char state);    //更改状态


    static unsigned char processDAC(void);
};

#endif
///////////////////////////////////////////////////////Auto Cali///////////////////////////////////////////////////////

#endif


//ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx    //通道的step    ChannelStep[chn].m_ucActiveIdx当前的
//下一步  (!m_ucActiveIdx)

