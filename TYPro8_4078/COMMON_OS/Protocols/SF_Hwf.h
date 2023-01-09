/*****************************************************************************
// 程序文件                   :SF_Hwf.h     主要用于网络处理
// 文件描述                   :为MCU board服务
// 编写日期(MM/DD/YYYY)       :2014/2/14
*****************************************************************************/

#ifndef _SF_HARDWAREINTERFACE_H_
#define _SF_HARDWAREINTERFACE_H_
#pragma  once

/***** IV board type *****/
enum En_IV_BOARDTYPE
{
    BOARDTYPE_OLD_AUTO_CALI   =   0,
    BOARDTYPE_OLD_IBT         =   1,
    BOARDTYPE_OLD_TESLA_PWM   =   2,
    BOARDTYPE_OLD_BT2208      =   3,  //  (不再使用模块)
    BOARDTYPE_OLD_BT2401      =   4,
    BOARDTYPE_OLD_BT11        =   5,   //  (不再使用模块)
    BOARDTYPE_OLD_BT2204      =   6,
    BOARDTYPE_OLD_PWM         =   7,
    BOARDTYPE_OLD_LBT21024    =   8,
    BOARDTYPE_OLD_LBT2108     =   9,
    BOARDTYPE_OLD_LBT22043    =   10,
    BOARDTYPE_OLD_LBT21164    =   11,
    BOARDTYPE_OLD_LBT22013    =   12,
    BOARDTYPE_OLD_LBT20083    =   13,
    BOARDTYPE_OLD_HPT200      =   14,
    BOARDTYPE_OLD_LBT22024    =   16,
    BOARDTYPE_OLD_MSTAT21044  =   17,//对应广州MSTAT21044
    BOARDTYPE_OLD_LBT22013FR  =   18,
    BOARDTYPE_OLD_HS21044     =   19,
    BOARDTYPE_OLD_LBT22044    =   20,
    BOARDTYPE_OLD_HPM4        =   21,
    BOARDTYPE_OLD_LBT21084VB  =   22,
    BOARDTYPE_OLD_LBT21024FR  =   23,
    BOARDTYPE_OLD_IBT31162    =   24,  //[特斯拉8450项目PWM与广州线性LBT21162]
    BOARDTYPE_OLD_LBT20083VB  =   25,
    BOARDTYPE_OLD_LBT21024FRVB =  26,
    BOARDTYPE_OLD_LBT21162    =   27, //[天津线性]
    BOARDTYPE_OLD_LBT21044   =    28,  //天津LBT21044
    BOARDTYPE_OLD_LBT21014FRVB  = 29,
    BOARDTYPE_OLD_LBT20084    =   30,
    BOARDTYPE_OLD_LBT20162    =   31,
//以上old ver
    BOARDTYPE_TESLA_PWM       =   40,

    BOARDTYPE_LBT20083        =   41,
    BOARDTYPE_LBT20084        =   42,
    BOARDTYPE_LBT20162        =   43, //[天津线性]

    BOARDTYPE_LBT21014        =   44,
    BOARDTYPE_LBT21024        =   45,
    BOARDTYPE_HS21044         =   46,
    BOARDTYPE_MSTAT21044      =   47,//对应广州MSTAT21044
    BOARDTYPE_LBT21084        =   48,
    BOARDTYPE_LBT21162        =   49,

    BOARDTYPE_LBT22013        =   50,
    BOARDTYPE_LBT22023        =   51,
    BOARDTYPE_LBT22043        =   52,

    BOARDTYPE_HPT200          =   53,
    BOARDTYPE_HPM4            =   54,

    BOARDTYPE_PWM             =   55,
    BOARDTYPE_LBT21044        =   56,  //天津LBT21044
    BOARDTYPE_HPS21024        =   57,

    BOARDTYPE_RBT3X           =   58,
    BOARDTYPE_RBT4104         =   59,

};


//};  //be careful,same sequence with the following line:

/***** IV board version *****/
#define BOARDTYPE_VER_A     0
#define BOARDTYPE_VER_B     1
#define BOARDTYPE_VER_C     2
#define BOARDTYPE_VER_D     3
#define BOARDTYPE_VER_E     4
//*****OLD IV board version *****//03.09.2018 simba
#define BOARDTYPE_VER_20    5
#define BOARDTYPE_VER_HC    6
//*****OLD IV board version *****
#define BOARDTYPE_VER_F     5
#define BOARDTYPE_VER_G     6

#define POWER_LEVEL_HC      1
#define POWER_LEVEL_LC      2
/* be careful,same sequence with the following line */
#define REPORT_FRAM_EVENT                        1             //写FRAM功能
#define SOFTWARE_DOG_ENABLE                      1             //是否使用双通信通道软件看门狗
//#define POWER_OFF_ENABLE                         1             //是否加入Tesla/LBT21024/LBT2108 电源检查功能
#define PC_SET_TIME_INTERVAL                     5             //上位机每隔5秒向下位机发送一次狗对时
#define COM_CHANNEL1_TIMEOUT                     120            //下位机第一信道断开超时  60
#define MAXCHANNELNO                             16            //IV最大16通道
#define MAXADCHANNELNO                           8             //AD 最多8通道
#define MAX_BUILDIN_DI_COUNT                     2             // 8 单个IV通道最大能支持的内嵌DI数量
#define MAX_BUILDIN_2V_COUNT                     4             //单个IV通道最大能支持的内嵌2V数量
#define MAX_BUILTIN_COUNT                        8             //单个IV通道最大能支持的BUILTIN 通道总数
#define MAXIV4AUXCHANNELNO						 16            //IV最大支持多少专有辅助通道
#define MAXMCUUNIT															 100            //最大支持多少MCU
#define MAXMTCCHAMBERNO													 8              //一个MTC最多有多少CHAMBER
#define IV_MAX_LIMIT_NUMBER                      45 //16           //定义IV通道STEP LIMIT 最大支持数目
//#define IV_MAX_PULSE_STAGE_NUMBER                10            //定义IV通道PULSE STAGE最大支持数目
#define IV_MAX_CV_STAGE_NUMBER                   10            //定义IV通道CV STAGE最大支持数目   (循环伏安法)
#define IV_MAX_SCHEDULE_STEP_NUMBER              200           //定义每个Schedule缓冲区的最大step数目 //200 
#define IV_MAX_SCHEDULE_CV_NUMBER                10            //定义每个Schedule缓冲区的最大CV数目
#define IV_MAX_SCHEDULE_FORMULA_NUMBER           64            //定义每个Schedule缓冲区的最大Formula数目   
#define IV_MAX_SCHEDULE_BUFFER_NUMBER            20            //定义IV通道Schedule缓冲区个数，暂定为20个
#define MAX_PULSE_STAGE_NUM                      30            //临时定义3个脉冲 dirui 2014/08/06
#define IV_MAX_PULSE_BUFFER_NUMBER               20            //定义IV通道Pulse缓冲区个数，暂定为20个
#define IV_MAX_ASSIGN_MD5_NUMBER                 16            //定义每条ASSIGN命令最多能承载的MD5码个数
#define MAX_SIMU_BUFFER_COUNT                    32 //64            //定义下位机simulation BUFFER碎片个数==SIMULATION文件最大个数 
#define MAX_SIMU_PACK_BYTE_COUNT                 1280
#define MAX_SIMU_FRAGMENT_POINT_COUNT            20480         //定义下位机simulation BUFFER碎片模拟points容量
#define MAX_SIMU_PACK_COUNT_PER_FRAGMENT         (MAX_SIMU_FRAGMENT_POINT_COUNT*8/MAX_SIMU_PACK_BYTE_COUNT)
#define MAX_SIMU_POINT_COUNT_PER_PACK            (MAX_SIMU_PACK_BYTE_COUNT/8)
#define MAX_SIMU_REPORT_PACKINDEX                600
#define BATTERY_SIMU_LEN                         sizeof(St_BatterySimuPoint)  //190218 zyx
#define MAX_BATTERY_SIMU_BUFFER_COUNT            MAXCHANNELNO            //定义下位机simulation BUFFER碎片个数==SIMULATION文件最大个数 
#define MAX_BATTERY_SIMU_PACK_BYTE_COUNT         1284
#define MAX_BATTERY_SIMU_FRAGMENT_POINT_COUNT    2033         //定义下位机simulation BUFFER碎片模拟points容量
#define MAX_BATTERY_SIMU_PACK_COUNT_PER_FRAGMENT (MAX_SIMU_FRAGMENT_POINT_COUNT* BATTERY_SIMU_LEN /MAX_SIMU_PACK_BYTE_COUNT)
#define MAX_BATTERY_SIMU_POINT_COUNT_PER_PACK    (MAX_SIMU_PACK_BYTE_COUNT/ BATTERY_SIMU_LEN)
#define PACK_MAX_PULSE_POINT                     110           //定义上传脉冲每包中最大的记录点
//#define MAX_LEN_SMB_CONFIG						 20 //SmartBettery add 6.26.2017
#define MAXBYTENUM								 129//128//SmartBettery add 6.26.2017
//#define IV_MAX_SIMULATION_BUFFER_NUMBER      16         //Simulation Buffer Numbers  //12.22.2013ADD //03.05.2014
//#define IV_NORMAL_SIMULATION_POINT_NUMBER    20//200000   //I,V,P,L Simulation Points， //12.22.2013ADD //03.05.2014
//#define IV_BATTERY_SIMULATION_POINT_NUMBER   1       //Battery Simulation Points  //Comment out 12.22.2013ADD
#define IV_DEFAULT_DXDT_FACTOR_I                 0.01f
#define IV_DEFAULT_DXDT_FACTOR_V                 0.01f
#define IV_DEFAULT_SDL_FACTOR_I                  0.001f
#define IV_DEFAULT_SDL_FACTOR_V                  0.001f
#define IV_DEFAULT_SDL_FACTOR_CAPACITY           0.01f
#define IV_DEFAULT_SDL_FACTOR_ENERGY             0.01f
#define IV_DEFAULT_SDL_TIME                      60.0f
#define IV_DEFAULT_SDL_NORMINAL_CAPACITY         1
#define IV_DEFAULT_SDL_NORMINAL_VOLTAGE          4.2f
#define AUX_DEFAULT_SDL_FACTOR_2ndV              0.001f
#define AUX_DEFAULT_SDL_FACTOR_T                 1.0f
#define AUX_DEFAULT_SDL_FACTOR_H                 1.0f
#define AUX_DEFAULT_SDL_FACTOR_EC_Current        0.001f
#define AUX_DEFAULT_SDL_FACTOR_PRESSURE          0.1f
#define MAX_BOARD_PER_AUXMCU 										 (8)		       //一个辅助MCU最多可带8块辅助
#define SAFTY_LOOSEN_FACTOR                      1.15f         //通道系统安全检查范围放松系数
#define CONVERTER_BIT_COUNT                      24            //下位机软件层计算使用的ADC/DAC的BIT数
#define CONVERTER_MAX                            (0x01<<24)-1  //ADC/DAC最大码值（偏移码）
#define CONVERTER_MID                            0x01<<23      //ADC/DAC中间码值（偏移码）
#define CONVERTER_MIN                            0             //ADC/DAC最小码值（偏移码）          
#define NUM_OF_IV_RANGE_I                        8
#define NUM_OF_IV_RANGE_V                        4
#define MAX_CMD_BUFFER_SIZE                      (1536)
#define MAX_CMD_BUFFER                           (MAX_CMD_BUFFER_SIZE)
#define MAX_MULTPACK_SIZE                        (1000 * MAX_CMD_BUFFER_SIZE)  //  最大多包字节数  包数乘以单包最大字节值(N*1536)
#define MAX_MULTPACK_UPLOAD_SIZE                 (500 * MAX_CMD_BUFFER_SIZE) //最大上传多包字节 170816 zyx
#define MAX_SEND_METAVARIABLE                    (80)    //30 ===> 60 ==> 65 //180713 zyx
#define MAX_USERDEFINED_SAFETY                   16     //170122
#define MAX_USERDEFINED_AUX_CHAN_SAFETY          512     //11.11.2017
#define MULTI_CANMSG_COUNT                       (256)   //170828 zyx


#define MAX_CH_IN_AUX_UNIT                       (96)
#define MAXAUXTYPE                               (12)       //现在系统中共有11种辅助类型  TE_AUX_CHAN_TYPE      
#define MAX_AUX_TYPE_PER_MCU                      4  //辅助限制 
#define MAXAUXUNIT                               255//8 
#define	MAX_AUXCH_INDEX							 512//64
#define MAXVIRTUALCHANNEL                        512//64            

#define AUX_VALUE_ANY_INDEX                      (255)

#define MAX_BUILDIN_AUXCH                        (8)   // 最大内部辅助通道数量	;暂定为8个，目前以LPC4078内部ADC通道最多为8个
#define MAX_BUILDIN_AUXTYPE			MAX_BUILDIN_AUXCH  //最大内置辅助种类不能超过最大内置辅助通道数
#define MAXUNIT                                  (64)   // 网络中最大的接点
#define MAX_IVUNIT                               (32)  //16 // 网络中最多IV unit数，暂定16个。
#define MAX_ARBIN_VALUE             99000
#define MAX_CAN_BMS_SIGNAL                        (2500) //255     1705051 zyx                            
#define MAX_LEN_SMB_CONFIG 												(20)
#define MAX_SMB_BMS_SIGNAL												(128)
#define MAX_SMB_SIGNAL_STRING									    (512)
#define MAX_TEST_SETTING_ELEMENT_COUNT            (32) //01.21.2015
#define MAX_TEST_SETTING_COUNT                    (16) //01.21.2015

#define MAX_GROUP_PER_UNIT						   16
#define MAX_UNIT_PER_GROUP                       16
#define MAX_CHN_PER_UNIT                        16        //最大16通道
#define MAX_MAP_COUNT_PER_UNIT                 (512)      //03.06.2017  IV与Aux 单个Unit 最多Map的个数，原因是 IV 和Aux  Fram 空间限制
#define MAX_MAP_COUNT_PER_IV_CHAN              (512)

#define MAXAUXMCUCHANNE       (512)                  //12.23.2016
#define MAX_SUPER_GROUP_PARALLEL_SLAVE_COUNT    (15)    //08.01.2017
#define BMS_BROAD_CST_COUNT                      3 //每个CANconfig包含CANBMS广播报文数

#define   MAX_UPDATA_STEP_NUMBER    1    // 最多n步    // 09.11.2017
#define   MAX_UPDATA_LIMIT_NUMBER   1    // 每步最多m个Limit

#define   MAX_SDL_PAIRS_NUMBER   21 //29    //  每包中最大结构点数 48*29 + 13=1405  //mits8
#define MAX_UD                                 (16) //zyx 1804251

#define MAX_SMB_BLOCK_CONTENT_COUNT   (20)  //控制类型Write SMB Block的上限//03.29.2018
#define MAX_SMB_COMMAND_CONTENT_COUNT (128) //支持最大128个字节的块

#define MAX_DETAIL      (100)  //DAQ打印版本特殊内容时使用
#define MAX_ACIM_DATA_POINT_NUM	61	// Arbin ACIM板做一次EIS测试记录点数上限

#define MAX_CANBMS_PORT (1)    //支持的最大CAN口数
#define MAX_AUX_SDL_POINT     (109)

#define MAX_ADVFORMULA_COUNT	16	//最多支持16条新公式
#define MAX_ADV_NODE_COUNT		64	//一个新公式最多64个Node

#define MAX_METAVARIABLE_SINGLE_PACK (160)  //单包更新MV变量最大数

#define MAX_GET_METAVARIABLES_SINGLE_PACK (128)	//单包更新MV变量最大数	//03.15.2022

enum  CTI_GET_MV_ERR_CODE
{
		CTI_GET_MV_SUCCESS = 0x0,
		CTI_GET_MV_ERROR = 0x10,
		CTI_GET_MV_DATATYPE_NOTSUPPORT,
		CTI_GET_MV_METACODE_NOTEXIST,
		CTI_GET_MV_CHANNEL_INDEX_ERROR,
		CTI_GET_MV_AUX_INDEX_ERROR,
		CTI_GET_MV_AUX_NOTASSIGN,
		CTI_GET_MV_CANBMS_INDEX_ERROR,
		CTI_GET_MV_CANBMS_NOTEXIST,
		CTI_GET_MV_CANBMS_DISABLED,
		CTI_GET_MV_NOT_CONNECT_DAQ,
		CTI_GET_MV_TIMEOUT,
		CTI_GET_MV_MCU_ACK_FAILED,
};

/*enum UnsafeCode
{
	ALL_SAFE                  =  0,
	CURRENT_SYS_UNSAFE        = (1<<0),                //（0X00000001)
	CURRENT_SCH_UNSAFE        = (1<<1),                //（0X00000002)   Unsafe的m_uw32Safe_Code属第二层
	POWER_SYS_UNSAFE          = (1<<2),                //（0X00000004)
	POWER_SCH_UNSAFE          = (1<<3),                //（0X00000008)
	VOLTAGE_SYS_UNSAFE        = (1<<4),                //（0X00000010)
	VOLTAGE_SCH_UNSAFE        = (1<<5),                //（0X00000020)
	LIMIT_UNSAFE              = (1<<6),                //（0X00000040)
	SMB_LOST_UNSAFE            = (1<<7),                //（0X00000080)
	HARDWARE_UNSAFE           = (1<<8),                // （0X00000100)  硬件错误--PWM ERROR，TESLA POWER OFF，and so on
	CTRL_VALUE_CHK_UNSAFE     = (1<<9),                // （0X00000200)  控制值检查异常
	IRREGULAR_UNSAFE          = (1<<10),               // （0X00000400)  合理性检查异常
	REDUNDANT_SAFETY          = (1<<11),               // （0X00000800)
	NOTAVAIL_UNSAFE           = (1<<12),               //（0X00001000)   认为99可能按位与前面的重复，因为处理的时候可能是按位来处理

	//AUX_TEMPERATURE_UNSAFE    = (1<<13),               //（0X00002000)   IV 上报aux unsafe   32位按位确定
	AUX_UNSAFE                = (1<<13),
	//AUX_VOLTAGE_UNSAFE        = (1<<14),               //（0X00004000)
	AUX_ACK_FAILED            = (1<<15),               //（0X00008000)
	AUX_CAN_LOST              = (1<<16),               //（0X00010000)
	AUX_STATUS_ERROR          = (1<<17),               //（0X00020000)
	CAN_BMS_UNSAFE            = (1<<18),               //(0x00040000)

	PARA_SUB_CUR_UNSAFE       = (1<<19),               //(0x00080000)
	PARA_SUB_VOL_UNSAFE       = (1<<20),               //(0x00100000)
	PARA_SUB_IRREGULATION 	   = (1<<21),               //(0x00200000)
	CAN_PARA_ACK_UNSAFE       = (1<<22),               //(0x00400000)
	FAN_FAULT_UNSAFE          = (1<<23),               //(0x00800000)

    //AUX_FLOW_RATE_UNSAFE      = (1<<24),                 //(0x01000000)  // 用于温控器湿度SCH报警

	IGBT_DRIVER_ERROR         = (1<<25),                // (0x02000000)  IGBT驱动器的故障
	DCLINK_OVERVOLTAGE_ERROR  = (1<<26),                // (0x04000000)DCLink电压过压故障
	INVERTER_FUSE_ERROR       = (1<<27),                // (0x08000000)Inverter上报故障及fuse故障
	ACR_OVERTIME_UNSAFE	   = (1<<28),                // (0x10000000)  ACR超时报警，无法抢到定时器资源
	//AUX_EXTCHARGE_UNSAFE      = (1<<29),                //外部充电电流 SCH报警
	AMP_IR_UNSAFE	   = (1<<29),

	SCH_UserDefined_UNSAFE  = (1<<30),    //170122
};*/
enum UnsafeType
{
    IV_UNSAFE_TYPE,
    AUX_UNSAFE_TYPE,
    DCDC_UNSAFE_TYPE,
    CANBMS_UNSAFE_TYPE,
    SMB_UNSAFE_TYPE,
    INVERTER_UNSAFE_TYPE,
    TOTAL_UNSAFE_TYPE,
};
enum IV_UnsafeCode
{
    IV_ALL_SAFE                  =  0,
    CURRENT_SYS_UNSAFE        = (1<<0),
    CURRENT_SCH_UNSAFE        = (1<<1),
    POWER_SYS_UNSAFE          = (1<<2),
    POWER_SCH_UNSAFE          = (1<<3),
    VOLTAGE_SYS_UNSAFE        = (1<<4),
    VOLTAGE_SCH_UNSAFE        = (1<<5),
    LIMIT_UNSAFE              = (1<<6),
    IRREGULAR_UNSAFE          = (1<<7),
    REDUNDANT_SAFETY          = (1<<8),
    VCLAMP_SAFETY             = (1<<9),
    PARA_SUB_CUR_UNSAFE       = (1<<10),
    PARA_SUB_VOL_UNSAFE       = (1<<11),
    POWER_COMMUNICATION_FAIL  = (1<<12),
    CAN_PARA_ACK_UNSAFE       = (1<<13),
    FAN_FAULT_UNSAFE          = (1<<14),
    ACR_OVERTIME_UNSAFE       = (1<<15),
    AMP_IR_UNSAFE             = (1<<16),
    SCH_UserDefined_UNSAFE    = (1<<17),
    CTRL_VALUE_CHK_UNSAFE     = (1<<18),
    IV_TEMPERATURE_UNSAFE     = (1<<19),
    POWER_FEEDBACK_ERROR      = (1<<20),//电压跟随
    PARA_SUB_IRREGULATION 	   = (1<<21),               //(0x00200000)
    STEPTIME_SCH_UNSAFE = (1 << 22),//
    CAPACITY_SCH_UNSAFE  = (1 << 23), ////11.11.2017

    IV_CABLE_CONNECTION_UNSAFE= (1<< 24),
    IV_POWER_OFF_UNSAFE       = (1<< 25),
    PARA_POWER_OFF_UNSAFE     = (1<< 26),
    PARA_FAN_FAULT_UNSAFE     = (1<< 27),
};
enum AUX_UnsafeCode
{
    AUX_ALL_SAFE              =  0,
    AUX_UNSAFE                = (1<<0),
    AUX_ACK_FAILED            = (1<<1),
    AUX_STATUS_ERROR          = (1<<2),
    AUX_CAN_LOST              = (1<<3),
    MTC_FAN_UNSAFE            = (1<<4),
    MTC_TEMPERATURE_UNSAFE    = (1<<5),      //MTC温度报警  07.12.2018 by zc
    MTCI_RUN_FAIL			  = (1<<6),				//温控箱运行报警
    MTCI_SET_FAIL			  = (1<<7),	//温控箱设置值报警
    MTCI_STOP_FAIL		  	  = (1<<8),	//温控箱停止报警
    MTCI_READ_FAIL			  = (1<<9),	//温控箱读值报警
};
enum DCDC_UnsafeCode
{
    DCDC_ALL_SAFE             =  0,
    DCDC_DRIVER_ERROR         = (1<<0),
    HARDWARE_UNSAFE           = (1<<1),
    DCLINK_OVERVOLTAGE_ERROR  = (1<<2),
    DCDC_FUSE_ERROR       = (1<<3),
    IGBT_DRIVER_ERROR         = (1<<25),                // (0x02000000)  IGBT驱动器的故障
    INVERTER_FUSE_ERROR       = (1<<27),                // (0x08000000)Inverter上报故障及fuse故障
};

enum CANBMS_UnsafeCode
{
    CANBMS_ALL_SAFE           =  0,
    CAN_BMS_UNSAFE            = (1<<0),
};
enum SMB_UnsafeCode
{
    SMB_ALL_SAFE              =  0,
    SMB_LOST_UNSAFE           = (1<<0),
};
enum INVERTER_UnsafeCode
{
    INVERTER_ALL_SAFE             =  0,
    FreqUnder_SW                  = (1<<0),         //24     24-31 Byte1    first 8bit
    PhaseSeqRev_SW                = (1<<1),
    NoPll_SW                      = (1<<2),
    //rsvd3:5;                //27-31
    DcVoltOver_SW                 = (1<<8), //16        16-23Byte2
    DcVoltUnder_SW                = (1<<9),
    CurrAOver_SW                  = (1<<10),
    CurrBOver_SW                  = (1<<11),
    CurrCOver_SW                  = (1<<12),
    LineVoltOver_SW               = (1<<13),
    LineVoltUnder_SW              = (1<<14),
    FreqOver_SW                   = (1<<15),
    Contactor_Err                 = (1<<16),
    Start_Err                     = (1<<17),
    PowerOver_SW                  = (1<<18),
    Cap_Error                     = (1<<19),
    PhaseLoss_SW                  = (1<<20),  // 20191031 ZhCh
    //revd2:4;                //8-15    8-15 Byte3

    ErrAll_HW                     = (1<<24),
    DcVoltOver_HW                 = (1<<25),
    LineVoltUnder_HW              = (1<<26),
    CurrOver_HW                   = (1<<27),
    TemperatureOver_HW            = (1<<28),
    FanStop_HW                    = (1<<29),
    NoFreq_HW                     = (1<<30),
    //revd1:1;                //7
};

//枚举类型定义
enum  eParaMode
{
    eSingleMode,
    eIntraMode,
    eInterMode,
    eMixMode,
};
enum  eSimuType
{
    eNoRepeat,
    eNoRepeat_ExtraLog,
    eRepeat,
    eRepeat_ExtraLog,
};
enum  eCTRL_VALUE_ERROR
{
    CTRL_VALUE_ERROR_Null,
    CTRL_VALUE_ERROR_Current,
    CTRL_VALUE_ERROR_Voltage,
    CTRL_VALUE_ERROR_Power,
    CTRL_VALUE_ERROR_Load,
};
enum eIRREGULAR_ERROR
{
    IRREGULAR_ERROR_Null,
// 	IRREGULAR_ERROR_StepTime,    // Unsafe的m_btSubCode属第三层     //zyx 180705
// 	IRREGULAR_ERROR_Capacity,
// 	IRREGULAR_ERROR_Energy,
    IRREGULAR_ERROR_Voltage,
};
enum eIRCHECK_ERROR
{
    eIRCHECK_ERROR_Safe,
    eIRCHECK_ERROR_Unsafe,
};

enum eCANBUSWATCHDOG_ERROR
{
    eCANBUSWATCHDOG_ERROR_Null,
    eCANBUSWATCHDOG_ERROR_StatusSync,
    eCANBUSWATCHDOG_ERROR_Connection,
    eCANBUSWATCHDOG_ERROR_DoMasterAckCheck,
};

enum eEMERGENCYSTOP_ERROR
{
    eEMERGENCYSTOP_ERROR_Null,
    eEMERGENCYSTOP_ERROR_WatchDogOverTime,
    eEMERGENCYSTOP_ERROR_TokenBroken,
    eEMERGENCYSTOP_ERROR_SendEventBufFULL,
    eEMERGENCYSTOP_ERROR_SendEventBufNotEnough,
    eEMERGENCYSTOP_ERROR_SendDataBufFULL,
    eEMERGENCYSTOP_ERROR_SendDataBufNotEnough,
    eEMERGENCYSTOP_ERROR_SchOverTime,
    eEMERGENCYSTOP_ERROR_SchOnlineupdate,    // 8
    eEMERGENCYSTOP_ERROR_AdcCRCError,              //9
    eEMERGENCYSTOP_ERROR_PulseWaitOvertime,
    eEMERGENCYSTOP_ERROR_IrWaitTimeOut,
    eEMERGENCYSTOP_ERROR_WithoutIr,
    eEMERGENCYSTOP_ERROR_PulseStageTimeTooShort,
    eEMERGENCYSTOP_ERROR_PulseStageTimeCycle,//Need a stage longer than 4ms.
    eEMERGENCYSTOP_ERROR_SampleOverTime = 15,
    eEMERGENCYSTOP_ERROR_Leakage_Current = 16,    //  漏电流报警
    eEMERGENCYSTOP_ERROR_Channel_Relay_Close = 17,   //  闭合通道继电器报告
	eEMERGENCYSTOP_ERROR_FixedBoosterError = 18, 
    eEMERGENCYSTOP_ERROR_PulseSampleError,
};
enum TE_SDL_DATA_PURPOSE
{
    SDL_DATA_FOR_LOG,
    SDL_DATA_FOR_DISPLAY,
    SDL_DATA_FOR_IDLE,
};

enum TE_SDL_DATA_LOG_FORMAT  //mits8
{
//     SDL_DATA_TYPE_LOG_C_V_CAPACITY,    //  电流，电压，充电电量，放电电量
//     SDL_DATA_TYPE_LOG_C_V_ENERGY,      //  电流，电压，充电能量，放电能量
//     SDL_DATA_TYPE_LOG_C_V_DVDT_DVDQ,   //  电流，电压，DVDT， DVDQ
//     SDL_DATA_TYPE_LOG_C_V_IR,          //  电流，电压，实测内阻，CV控制用内阻
//     SDL_DATA_TYPE_LOG_C_V_DEBUG,       //  电流，电压， debug1， debug2
//     SDL_DATA_TYPE_LOG_C_V_SINGLE,      //  电流，电压，MetaVariable-code， value

    SDL_DATA_TYPE_LOG_C_V_CAP_ENG,      // //  电流，电压，充电电量，放电电量, charge energy, discharge energy
    SDL_DATA_TYPE_LOG_3_META_VAR,      //  MetaVariable-code， value,MetaVariable-code， value,MetaVariable-code， value
    SDL_DATA_TYPE_LOG_6_DEBUG_VAR,       //  debug1， debug2, ,,, debug6
    SDL_DATA_TYPE_LOG_CT_DT_DVDT_DVDQ_DIDT,   // Charge time, Discharge time, DVDT，DVDQ, DIDT
    SDL_DATA_TYPE_LOG_BuildInAux,
    SDL_DATA_TYPE_LOG_TCCOUNT_ACCESS,
};

enum TE_META_VAR_RESET_VALUE
{
    MP_MV_RESET_CHAN_CHARGE_CAP,
    MP_MV_RESET_CHAN_DISCHARGE_CAP,
    MP_MV_RESET_CHAN_CHARGE_ENERGY,
    MP_MV_RESET_CHAN_DISCHARGE_ENERGY,
    MP_MV_RESET_COUNTER_TIMER1,
    MP_MV_RESET_COUNTER_TIMER2,
    MP_MV_RESET_COUNTER_TIMER3,
    MP_MV_RESET_COUNTER_TIMER4,
    MP_MV_RESET_COUNTER_CHARGE_CAP1,
    MP_MV_RESET_COUNTER_CHARGE_CAP2,
    MP_MV_RESET_COUNTER_DISCHARGE_CAP1,
    MP_MV_RESET_COUNTER_DISCHARGE_CAP2,
    MP_MV_RESET_COUNTER_CHARGE_ENERGY1,
    MP_MV_RESET_COUNTER_CHARGE_ENERGY2,
    MP_MV_RESET_COUNTER_DISCHARGE_ENERGY1,
    MP_MV_RESET_COUNTER_DISCHARGE_ENERGY2,
    MP_MV_RESET_COUNTER_COUNTER1,
    MP_MV_RESET_COUNTER_COUNTER2,
    MP_MV_RESET_COUNTER_COUNTER3,
    MP_MV_RESET_COUNTER_COUNTER4,
    MP_NUM_OF_META_VAR_RESET_VALUE,
};

enum TE_META_VAR_INC_VALUE
{
    MP_MV_INC_CHAN_CYCLE_INDEX,
    MP_MV_INC_COUNTER_COUNTER1,
    MP_MV_INC_COUNTER_COUNTER2,
    MP_MV_INC_COUNTER_COUNTER3,
    MP_MV_INC_COUNTER_COUNTER4,
    MP_NUM_OF_META_VAR_INC_VALUE,
};

enum TE_META_VAR_DEC_VALUE
{
    MP_MV_DEC_COUNTER_COUNTER1,
    MP_MV_DEC_COUNTER_COUNTER2,
    MP_MV_DEC_COUNTER_COUNTER3,
    MP_MV_DEC_COUNTER_COUNTER4,
    MP_NUM_OF_META_VAR_DEC_VALUE,
};

enum TE_MATH_OPERATOR
{
    MP_MO_ADD,
    MP_MO_SUBTRACT,
    MP_MO_MULTIPLY,
    MP_MO_DIVIDE,
    MP_MO_MODULUS,
    MP_NUM_OF_MATH_OPERATOR,
};

enum TE_MATH_FUNCTION
{
    MP_MF_NONE,
    MP_MF_ABS,
    MP_MF_ACOS,
    MP_MF_ASIN,
    MP_MF_ATAN,
    MP_MF_CEIL,
    MP_MF_COS,
    MP_MF_CUBIC,
    MP_MF_EVEN,
    MP_MF_EXP,
    MP_MF_FACT,
    MP_MF_FLOOR,
    MP_MF_INT,
    MP_MF_LN,
    MP_MF_LOG10,
    MP_MF_ODD,
    MP_MF_RANDOM,
    MP_MF_SIGN,
    MP_MF_SIN,
    MP_MF_SQR,
    MP_MF_SQRT,
    MP_MF_TAN,
    MP_MF_TRUNC,
    MP_NUM_OF_MATH_FUNCTION,
};

enum TE_MATH_COMPARE_SIGN
{
    MP_MCS_NONE,
    MP_MCS_GREATER_THAN,
    MP_MCS_LESS_THAN,
    MP_MCS_EQUAL_TO,
    MP_MCS_GREATER_THAN_OR_EQUAL_TO,
    MP_MCS_LESS_THAN_OR_EQUAL_TO,
    MP_NUM_OF_MATH_COMPARE_SIGN,
};

enum TE_SYS_SAFTY_TYPE
{
    MP_SYS_SAFTY_TYPE_CURRENT = 0,
    MP_SYS_SAFTY_TYPE_VOLTAGE = 1,
    MP_SYS_SAFTY_TYPE_POWER = 2,
    MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,
    MP_SYS_SAFTY_TYPE_TOTAL,
};

enum TE_SCH_SAFTY_TYPE
{
    MP_SCH_SAFTY_TYPE_CURRENT = 0,
    MP_SCH_SAFTY_TYPE_VOLTAGE = 1,
    MP_SCH_SAFTY_TYPE_POWER = 2,
    MP_SCH_SAFTY_TYPE_CAPACITY,
    MP_SCH_SAFTY_TYPE_STEPTIME,
    MP_SCH_SAFTY_TYPE_AuxiVoltage = 60,
    MP_SCH_SAFTY_TYPE_AuxiTemprature,
    MP_SCH_SAFTY_TYPE_AuxiPressure,
    MP_SCH_SAFTY_TYPE_AuxiExternalCharge,
    MP_SCH_SAFTY_TYPE_AuxiFlowRate,
    MP_SCH_SAFTY_TYPE_AuxiConcentration,
    MP_SCH_SAFTY_TYPE_AuxiPhValue,
    MP_SCH_SAFTY_TYPE_AuxiHumidity,
    MP_SCH_SAFTY_TYPE_AuxiSafetyVoltage,//辅助安全电压监测
    MP_SCH_SAFTY_TYPE_AuxiSafetyCurrent,//辅助安全电流监测
    MP_SCH_SAFTY_TYPE_AuxiSafetyTemperature,//辅助安全温度监测
		MP_SCH_SAFTY_TYPE_AuxiDI,
    MP_SCH_SAFTY_TYPE_TOTAL,
};

enum TE_DATA_TYPE
{
    MP_DATA_TYPE_FIGURE,
    MP_DATA_TYPE_METAVALUE,
    MP_DATA_TYPE_FORMULA,
    MP_DATA_TYPE_AUX_T             	=10,    //前面预留了几个，供IV的其他可能变量类型使用
    MP_DATA_TYPE_AUX_dTdt,
    MP_DATA_TYPE_AUX_V,
    MP_DATA_TYPE_AUX_dVdt,
    MP_DATA_TYPE_AUX_P,
    MP_DATA_TYPE_AUX_dPdt,
    MP_DATA_TYPE_AUX_PhValue,
    MP_DATA_TYPE_AUX_dPhVdt,
    MP_DATA_TYPE_AUX_FlowRate,
    MP_DATA_TYPE_AUX_dFRdt,
    MP_DATA_TYPE_AUX_Concentration,
    MP_DATA_TYPE_AUX_dConcentrationdt,
    MP_DATA_TYPE_AUX_DI,
    MP_DATA_TYPE_AUX_DO,
    MP_DATA_TYPE_CANBMS,
    MP_DATA_TYPE_EC,
    MP_DATA_TYPE_AUX_Humidity,
    MP_DATA_TYPE_AUX_dHumdt,
    MP_DATA_TYPE_SMBBMS,
    MP_DATA_TYPE_ADVFORMULA,	//逆波兰公式

};

enum TE_GOTO_STEP
{
    MCU_GOTOSTOPSTEP               	=60000,
    MCU_GOTONEXTSTEP               	=60001,
    MCU_GOTOSAMESTEP               	=60002,
    MCU_GOTOUNSAFE                 	=60010,
    MCU_GOTOWARNING                	=60020,
    MCU_GOTOANDCONDICTION          	=60029,
    MCU_GOTOGROUPWAITFORNEXT       	=60052,
    MCU_GOTOGROUPFOLLOWNEXT        	=60053,
    MCU_GOTOLOG                    	=60054,
    MCU_GOTOGLOBALUNSAFE           	=60051,
    MCU_GOTOGROUPUNSAFE            	=60050,
};

enum TE_IV_CTRL_TYPE
{
    CT_IVCH_STOP                   	=0,     //09.15.2011
    CT_IVCH_PAUSE                   =01,    //pause,   2015.03.16
    CT_IVCH_WRITECANOUTBOUND        =02,    //write can outbound, 190617,hpq
    CT_IVCH_CURRENT                	=11,    //current               N/A                           N/A
    CT_IVCH_VOLTAGE                	=12,    //voltage               N/A                           N/A
    CT_IVCH_CRATE                  	=13,    //C                     N/A
    CT_IVCH_REST                   	=14,    //N/A                   N/A                           N/A
    CT_IVCH_POWER                  	=15,    //power                 N/A                           N/A
    CT_IVCH_LOAD                   	=16,    //load                  N/A                           N/A
    CT_IVCH_SETVARIABLE            	=17,
    CT_IVCH_CRAMP                  	=18,    //start current(A)      ramp rate(A/s)                N/A
    CT_IVCH_VRAMP                  	=19,    //start voltage(V)      ramp rate(V/s)                N/A
    CT_IVCH_CSTAIRCASE             	=20,    //start current(A)      staircase rate(A/stair)       stair (s)
    CT_IVCH_VSTAIRCASE             	=21,    //start voltage(V)      staircase rate(V/stair)       stair (s)
    CT_IVCH_CPULSE                 	=22,
    CT_IVCH_VPULSE                 	=23,
    CT_IVCH_IR                     	=24,    //amplitude(A)          sample time(s)                offset
    CT_IVCH_ACI                    	=25,    //Peak-Peak current(A)  Frequency(Hz)                 offset
    CT_IVCH_ACIM                   	=26,
    CT_IVCH_CCCP                   	=27,    //current               voltage
    CT_IVCH_CCCV                   	=28,    //current               voltage
    CT_IVCH_CELLV                  	=29,
    CT_IVCH_ANALOGOUTPUT           	=30,
    CT_IVCH_DIGITALOUTPUT          	=31,
    CT_IVCH_EXTCHARGE              	=32,    //N/A                   N/A                            N/A
    CT_IVCH_EXTLOAD                	=33,
    CT_IVCH_ACS                    	=34,
    CT_IVCH_DIGII                  	=35,
    CT_IVCH_DIGIV                  	=36,
    CT_IVCH_DIGIPOWER              	=37,
    CT_IVCH_DIGILOAD               	=38,
    CT_IVCH_PRAMP                  	=39,
    CT_IVCH_PSTAIRCASE             	=40,
    CT_IVCH_ANALOG                  =41,
    CT_IVCH_SIMU_I              		=51,        //12.22.2013ADD
    CT_IVCH_SIMU_V              		=52,        //12.22.2013ADD //We Do Not Offer Voltage Simulation
    CT_IVCH_SIMU_P              		=53,        //12.22.2013ADD
    CT_IVCH_SIMU_L              		=54,        //12.22.2013ADD
    CT_IVCH_SIMU_Battery            =55,         //190218 zyx
    CT_IVCH_C_CV                   	=60,
    CT_IVCH_V_CV                   	=61,
    CT_IVCH_ACR                   	=62,
    CT_IVCH_ACIR                   	=63,    //01.24.2017 新添加一个ACR类型，包括了Amp 、Time和 offset,Time写死了 500us
    CT_IVCH_SETVALUE                =64,	//171109 zyx
    CT_IVCH_CCCV_TB             	=65,
    CT_IVCH_WRITE_SMB_BLOCK         =66,	//03.29.2018
    CT_IVCH_CCCV_WRM                =67,	// Add CCCV_WRM, Yifei, 2.6.2917
    CT_IVCH_CURRENT_TB              =68,  // DELL TURBO BOOST TEST 1A/1B  06.04.2018 by zc
    CTRL_TYPE_MCELL_ACI            	=80,    //Peak-Peak current(A)  Frequency(Hz)                  offset
    CTRL_TYPE_VOLT_METER           	=81,    //N/A                   N/A                            N/A

    CT_TYPE_CAN_SEND_MSG            =82,  //CAN BMS Send Messages
    CT_SPECIAL_IRTEST               =83,   //用于表明属于特殊IR测试步//zyx 2105291
    CT_TYPE_DO_SETTING              =84,   //DigitalOutput  in Unit
    CT_CALI_CaliV_WithR          	=85,
    CT_TYPE_SMB_WRITEREG_WORD       =86,
    CT_TYPE_SMB_WRITEREG_STRING			=87,
    CT_TYPE_SMB_OPTWORDADDRESS      =88,
    NUM_OF_MP_IV_CTRL_TYPE         	=89,    //84
};

enum TE_METAVARIABLE_CODE
{
    MetaCode_PV_StepIndex          	=1,
    MetaCode_PV_CycleIndex         	=2,
    MetaCode_PV_Steptime           	=3,
    MetaCode_PV_TestTime           	=4,
    MetaCode_PV_AC_Impedance        =5,
    MetaCode_PV_ACR 	            =6,
    MetaCode_PV_VmaxOnCycle         =7,
    MetaCode_MV_SpecificCapacity   	=8,
    MetaCode_MV_NorminalCapacity   	=9,
    MetaCode_LS_dVdt                =10,
    MetaCode_LS_dIdt                =11,
    MetaCode_LS_AC_Impedance        =12,
    MetaCode_LS_VmaxOnCycle         =13,
    MetaCode_PV_PulseStageAveVol   	=14,        //not supported
    MetaCode_PV_PulseAveVol        	=15,
    MetaCode_DV_Time               	=16,
    MetaCode_DV_Current            	=17,
    MetaCode_DV_Voltage            	=18,
    MetaCode_DV_PulseCycle         	=19,  //08.25.2014 add ozj pulse
    MetaCode_LC_dVdt                =20,
    MetaCode_PV_Voltage            	=21,
    MetaCode_PV_Current            	=22,
    MetaCode_PV_ChargeCapacity     	=23,
    MetaCode_PV_DischargeCapacity  	=24,
    MetaCode_PV_ChargeEnergy       	=25,
    MetaCode_PV_DischargeEnergy    	=26,
    MetaCode_PV_dVdt               	=27,
    MetaCode_PV_dIdt               	=28,
    MetaCode_MV_Mass               	=29,
    MetaCode_PV_InternalResistance 	=30,
    MetaCode_PV_PulseCycle        	=31,	//08.25.2014 add ozj pulse
    MetaCode_LC_dIdt                =32,
    MetaCode_MV_IHmin              	=33,
    MetaCode_MV_IHmax              	=34,
    MetaCode_MV_IMmin              	=35,
    MetaCode_MV_IMmax              	=36,
    MetaCode_MV_ILmin              	=37,
    MetaCode_MV_ILmax              	=38,
    MetaCode_MV_VHmin              	=39,
    MetaCode_MV_VHmax              	=40,
    MetaCode_LC_AC_Impedance        =41,
    MetaCode_LC_VmaxOnCycle         =42,
    MetaCode_MV_VLmin              	=43,
    MetaCode_MV_VLmax              	=44,
    MetaCode_PV_d2Vd2t             	=45,
    MetaCode_MV_NorminalVoltage    	=46,
    MetaCode_MV_NorminalIR          =47,  //171113,hpq,add for pro8
    MetaCode_PV_BuildIn_V1          =48,
    MetaCode_PV_BuildIn_V2          =49,
    MetaCode_PV_BuildIn_V3          =50,
    MetaCode_PV_BuildIn_V4          =51,
    MetaCode_MV_UD1                 =52,
    MetaCode_MV_UD2                 =53,
    MetaCode_MV_UD3                 =54,
    MetaCode_MV_UD4                 =55,
    MetaCode_MV_NorminalCapacitor   =56,  //171113,hpq,add for pro8
    MetaCode_PV_CV_StageTime        =77,  //161019 zyx
    MetaCode_PV_CV_StageCurrent     =78,  //161226 zyx
    MetaCode_PV_ChargeCapacityTime = 79,      //180224 zyx
    MetaCode_PV_DischargeCapacityTime = 80,	  //180224 zyx
    MetaCode_DV_CCCV_TB_PulseInterval  =81,	//08302017 by ZC for CCCV_TB //180224 zyx
    MetaCode_PV_dQdV               	=82,  // zyx 170801
    MetaCode_PV_dVdQ               	=83,  // zyx 170801
    MetaCode_PV_DataPoints           =84, //   Used to record how many data points were logged 180516 zyx

    MetaCode_PV_BuiltIn_Data1   =85,
    MetaCode_PV_BuiltIn_Data2   =86,
    MetaCode_PV_BuiltIn_Data3   =87,
    MetaCode_PV_BuiltIn_Data4   =88,
    MetaCode_PV_BuiltIn_Data5   =89,
    MetaCode_PV_BuiltIn_Data6   =90,
    MetaCode_PV_BuiltIn_Data7   =91,
    MetaCode_PV_BuiltIn_Data8   =92,
    MetaCode_TC_Timer1             	=61,
    MetaCode_TC_Timer2             	=62,
    MetaCode_TC_Timer3             	=63,
    MetaCode_TC_Timer4             	=64,
    MetaCode_TC_ChargeCapacity1    	=65,
    MetaCode_TC_ChargeCapacity2    	=66,
    MetaCode_TC_DischargeCapacity1 	=67,
    MetaCode_TC_DischargeCapacity2 	=68,
    MetaCode_TC_ChargeEnergy1      	=69,
    MetaCode_TC_ChargeEnergy2      	=70,
    MetaCode_TC_DischargeEnergy1   	=71,
    MetaCode_TC_DischargeEnergy2   	=72,
    MetaCode_TC_Counter1           	=73,
    MetaCode_TC_Counter2           	=74,
    MetaCode_TC_Counter3           	=75,
    MetaCode_TC_Counter4           	=76,

    MetaCode_PV_Error_IV        =94,
    MetaCode_PV_Error_AUX       =95,
    MetaCode_PV_Error_DCDC      =96,
    MetaCode_PV_Error_CANBMS    =97,
    MetaCode_PV_Error_SMB       =98,
    MetaCode_PV_Error_INVERTER	=99,

    MetaCode_LS_StepIndex          	=100,
    MetaCode_LS_CycleIndex         	=101,

    MetaCode_LS_StepTime           	=103,
    MetaCode_LS_TestTime           	=104,

    MetaCode_MV_UD5                 = 105, //1807131 zyx
    MetaCode_MV_UD6                 = 106,
    MetaCode_MV_UD7                 = 107,
    MetaCode_MV_UD8                 = 108,
    MetaCode_MV_UD9                 = 109,
    MetaCode_MV_UD10                = 110,
    MetaCode_MV_UD11                = 111,
    MetaCode_MV_UD12                = 112,
    MetaCode_MV_UD13                = 113,
    MetaCode_MV_UD14                = 114,
    MetaCode_MV_UD15                = 115,
    MetaCode_MV_UD16                = 116,

    MetaCode_MV_C_Rate              =120,    //09.10.2019
    MetaCode_LS_Voltage            	=121,
    MetaCode_LS_Current            	=122,
    MetaCode_LS_ChargeCapacity     	=123,
    MetaCode_LS_DischargeCapacity  	=124,
    MetaCode_LS_ChargeEnergy       	=125,
    MetaCode_LS_DischargeEnergy    	=126,

    MetaCode_LS_InternalResistance 	=130,
    MetaCode_LS_ACR 	            =131,
		
    MetaCode_Debug1              =150,
    MetaCode_Debug2              =151,
    MetaCode_Debug3              =152,
    MetaCode_Debug4              =153,
    MetaCode_Debug5              =154,
    MetaCode_Debug6              =155,

    MetaCode_LC_StepIndex          	=216,
    MetaCode_LC_CycleIndex         	=217,
    MetaCode_LC_TestTime           	=218,
    MetaCode_LC_InternalResistance 	=219,
    MetaCode_LC_ACR 	            =220,
    MetaCode_LC_Voltage            	=221,
    MetaCode_LC_Current            	=222,
    MetaCode_LC_ChargeCapacity     	=223,
    MetaCode_LC_DischargeCapacity  	=224,
    MetaCode_LC_ChargeEnergy       	=225,
    MetaCode_LC_DischargeEnergy    	=226,

    MetaCode_PV_EISTestID 	        =227,	//01.22.2019
    MetaCode_PV_EISZmod				=228,	//01.22.2019
    MetaCode_PV_EISZphz   			=229,	//01.22.2019
    MetaCode_PV_EISZreal   			=230,	//01.22.2019
    MetaCode_PV_EISZimg   			=231,	//01.22.2019

    MetaCode_LC_StepTime           	=252,
    MetaCode_PV_UNIT_DI1            =253,
    MetaCode_PV_UNIT_DI2            =254,
    MetaCode_MV_COUNT              	=255,
};


enum TE_CURRENT_RANGE
{
    CURRENT_RANGE_1,
    CURRENT_RANGE_2,
    CURRENT_RANGE_3,
    CURRENT_RANGE_4,
    CURRENT_RANGE_5,
    CURRENT_RANGE_6,
    CURRENT_RANGE_7,
    CURRENT_RANGE_8,

    CURRENT_RANGE_PARALLELHIGH = 20,
    CURRENT_RANGE_COUNT,
};

enum TE_VOLTAGE_RANGE
{
    VOLTAGE_RANGE_LOW,
    VOLTAGE_RANGE_MED,
    VOLTAGE_RANGE_HIGH,
    VOLTAGE_RANGE_COUNT,
};

enum TE_MD5CODE_TYPE
{
    MD5CODE_TYPE_SCHEDULE,
    MD5CODE_TYPE_SIMULATION,
    MD5CODE_TYPE_PULSE,
    MD5CODE_TYPE_BATTERY_SIMULATION,  //190218 zyx
};

enum TE_ERROR_CODE
{
    MCU_ERRCODE_STARTCMD_UNKNOW    	=1,     //不正确或者不对的START命令
    MCU_ERRCODE_STARTNPAUSE_UNKNOWCMD	=2,   //非法的STARTNPAUSE 命令
    MCU_ERRCODE_STARTCMD_LOADCODEERROR	=6,  //系统装载数据出错
    MCU_ERRCODE_STARTCMD_SYSUNSAFE 	=8,     //系统不安全无法启动
    MCU_ERRCODE_PAUSECMD_UNKNOWCMD 	=21,
    MCU_ERRCODE_PAUSECMD_HAVEPAUSECMD,
    MCU_ERRCODE_JUMPCMD_NOTRUNNING 	=31,
    MCU_ERRCODE_RESUMECMD_UNKNOWCMD	=41,
    MCU_ERRCODE_RESUMECMD_STEPRUNNING,
    MCU_ERRCODE_SCHEDULETOOLONG    	=51,    //SCH 超长
    MCU_ERRCODE_SCHEDULELENGTHNOTMATCH,     //52 SCH 长度不匹配(多包下载结束收不齐)
    MCU_ERRCODE_SCHEDULESTEPIDERROR,        //53 SCH 内STEPID越界
    MCU_ERRCODE_SCHEDULELOADERROR,          //54 SCH 内STEP加载错误
    MCU_ERRCODE_SIMULATIONDATAERROR	=61,    //Simulation Data Error
    //MCU_ERRCODE_EMERGENCYSTOP    	=81,
    MCU_ERRCODE_WATCHDOG_TIMEOUT   	=91,    //WatchDog Timeout
};

enum TE_ERROR_TYPE
{
    MP_ERROR_UNKNOWN               	=0,     //第一信道错误码(TE_CONFIRM_ERROR_TYPE)与第二信道错误码(TE_ERROR_TYPE)不能有交集
    //MP_ERROR_ASSIGN_FAIL         	=65,
    //MP_ERROR_START_FAIL          	=66,
    //MP_ERROR_STOP_FAIL           	=67,
    MP_ERROR_LOCK_TOSCH_FAIL       	=68,
    MP_ERROR_LOAD_SCH_FAIL         	=69,
    MP_ERROR_OVERTIME              	=70,
    //MP_ERROR_LOCK_SCH_SAFECHECK  	=71,
    MP_ERROR_LOCK_TOTHIRD_FAIL     	=72,
    MP_ERROR_TIME_CALI_FAIL					=73,
    MP_ERROR_ADCTYPE								=74,
		MP_ERROR_IV_BOARD_TYPE					=75,
};

enum TE_BTUPDATE_TYPE
{
    BTUPDATE_CHECK_FAIL            	=0,
    BTUPDATE_CHECK_SUCCESS         	=0X01,
    BTUPDATE_REQUEST               	=0X02,
    BTUPDATE_RECONNECT             	=0X04,
};

enum TE_CALIBRATE_CONTROLTYPE
{
    CALIBRATE_CONTROLTYPE_CURRENT  	=0X01,
    CALIBRATE_CONTROLTYPE_VOLTAGE,
    CALIBRATE_CONTROLTYPE_TEMPERATURE,
    CALIBRATE_CONTROLTYPE_2NDV,
    CALIBRATE_CONTROLTYPE_VCLAMP_HIGH,
    CALIBRATE_CONTROLTYPE_VCLAMP_LOW,
    CALIBRATE_CONTROLTYPE_ACR_GAIN,
    CALIBRATE_CONTROLTYPE_EXTCHARGE,
    CALIBRATE_CONTROLTYPE_BUILDIN_VOLTAGE,
    CALIBRATE_CONTROLTYPE_PRESSURE,
    CALIBRATE_CONTROLTYPE_OTHER,
};

enum TE_CALIBRATE_ADC5DAC
{
    CALIBRATE_ADC                  	=0X00,
    CALIBRATE_DAC                  	=0X01,
};

enum TE_COMM_SIGNAL
{
    MPCS_ACK                       	=0x06,
    MPCS_NACK                      	=0x15,
    MPCS_SENT                      	=0xCC,
};

enum TE_CONFIRM_RESULT
{
    CONFIRM_RESULT_FAIL            	=0,
    CONFIRM_RESULT_SUCCESS         	=1,
};

enum TE_SCHEDULE_EVENT_RUNNING
{
    ST_EVENT_UNKNOWN               	=0,
    ST_EVENT_STARTED               	=1,
    ST_EVENT_STOPPED               	=2,     //Stop第一层事件判断处理
    ST_EVENT_LIMITREACH            	=8,
    ST_EVENT_BACKTONORMAL          	=20,
    ST_EVENT_JUMP                  	=21,
    ST_EVENT_GOSTOP                	=22,
    ST_EVENT_LIMITCHANGE           	=23,
    ST_EVENT_ACIMRELAYON			=24,
    ST_EVENT_ACIMRELAYOFF			=25,
};

enum TE_STATUS_CHANNEL_STOP
{
    STOP_UNKNOWN                   	=0,     // Stop的m_btStopped_Status第二层判断处理
    STOP_FINISHED                  	=1,
    STOP_MANUAL                    	=2,
    STOP_UNSAFE                    	=3,     //Stop第三层入口
    STOP_EMERGENCY                 	=4,
    STOP_NOTSFTOSTART              	=5,
    STOP_POWER_OFF                 	=6,
    STOP_AUXUNSAFE                 	=7,
    STOP_AUXACKFAIL                 =8,
    STOP_AUXACKFAILINSTART			=9,
    STOP_CHANNELABNORMAL            =10,
    STOP_CAN_BMS_LOST               =11,
    STOP_ACR_OVERTIME               =12,   // ACR超时报警，无法抢到定时器资源
    STOP_SMBUS_LOST       	        =13,   //add by zc 12012017
};

enum TE_STATUS_UNSAVE_CLASS
{
    UNSAFE_CLASS_UNKNOWN           	=0,     //Stop第三层判断处理
    UNSAFE_SYSTEM                  	=1,
    UNSAFE_SCHEDULE                	=2,
    UNSAFE_CLAMPVOLTAGE            	=3,
    UNSAFE_LIMIT                   	=7,
    UNSAFE_LOGIC                   	=0X80,  //Stop第四层入口, 内容在TE_ERROR_TYPE
};

enum TE_CONFIRM_ERROR_TYPE
{
    MP_CONFIRM_ERROR_UNKNOWN       	=0,     //第一信道错误码(TE_CONFIRM_ERROR_TYPE)与第二信道错误码(TE_ERROR_TYPE)不能有交集
    MP_CONFIRM_ERROR_PACK_RESEND   	=1,
    MP_CONFIRM_ERROR_OVER_MAXCH    	=10,
    MP_CONFIRM_ERROR_IRANG_EMPTY   	=11,
    MP_CONFIRM_ERROR_OVER_MAXIRANG 	=12,
    MP_CONFIRM_ERROR_VRANG_EMPTY   	=13,
    MP_CONFIRM_ERROR_OVER_MAXVRANG 	=14,
    MP_CONFIRM_ERROR_SAVESCH       	=15,
    MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULERUNNING	=16, //Assign+Start 结果 ASSIGNFAIL 原因
    MP_CONFIRM_ERROR_ASSIGNFAIL_SCHEDULEUNSAGE	=17, //Assign+Start 结果 ASSIGNFAIL 原因
    MP_CONFIRM_ERROR_SAVESIMU    	=18,            //ASSIGN SIMULATION FAIL 原因  //12.22.2013ADD
    MP_CONFIRM_ERROR_AUX_WRITE_BOARD,
    MP_CONFIRM_ERROR_AUX_WRITE_BOARD_CHANNEL,
    MP_CONFIRM_ERROR_AUX_READ_BOARD,
    MP_CONFIRM_ERROR_AUX_READ_BOARD_CHANNEL,
    MP_CONFIRM_ERROR_MAP_SAVESPACE_OVER,             //  MAP表存储空间溢出
    MP_CONFIRM_ERROR_MAP_SENSOR_TYPE,
};

enum TE_FRAM_WRITE_TYPE
{
    WRITE_CURRENT                  	=1,
    WRITE_VOLTAGE                  	=2,
    WRITE_CLAMPVOLH                	=3,
    WRITE_CLAMPVOLL                	=4,
    WRITE_PID                      	=5,
    WRITE_CONFIG                   	=6,
    WRITE_ACR                       =7,
    WRITE_BUILDINVOLT               =8,
};

enum TE_SCH_SOURCE_EVENT
{
    UNKNOWN                        	=0,
    S_IDLE_RUNMSG_LOCK,
    S_IDLE_RESUMEMSG_LOCK,
    S_START_JUMPMMSG,
    S_START_ONUNSAFEMSG,
    S_RUNNING_JUMPAMSG,
    S_RUNNING_JUMPMMSG,
    S_RUNNING_LIMITARRIVEMSG_GOTONEXTSTEP,
    S_RUNNING_LIMITARRIVEMSG_GOTOSTEP,
    S_RUNNING_LIMITARRIVEMSG_SETVARIABLE_STARTED,
    S_RUNNING_LIMITARRIVEMSG_SETVARIBALE_GOTONEXTSTEP,
    S_RUNNING_LIMITARRIVEMSG_SETVARIABLE_GOTOSTEP,
    S_RUNNING_LIMITARRIVEMSG_SETVARIABLE_NOLIMITARRIVEANDGOTONEXT,
    S_RUNNING_LIMITSTATUSCHANGEMSG,
    S_RUNNING_STARTEDMSG,
    S_RUNNING_ONUNSAGEMSG,
    S_GOPAUSE_STOPPEDMSG,
    S_GOPAUSE_ONUNSAGEMSG,
    S_RESUME_ONUNSAFEMSG,
    S_GOSTOP_STOPPEDMSG,
    S_GOSTOP_ONUNSAFEMSG,
    S_GOSTOP_MAINTAINCE,
    S_FIXEDUNSAFE_STOPMSG,
    S_DYNAMICUNSAGE_RUNMSG,
    S_GOSTOP_OVERTIME,
};

enum TE_VALUE_CHECKPLACE_ENUM
{
    ValueCheckPlace_WHATEVER       	=0,
    ValueCheckPlace_IV             	=1,
    ValueCheckPlace_AUXI           	=2,
    ValueCheckPlace_PC             	=4,
};

enum TE_AUX_CHAN_TYPE
{
    AUX_VOLTAGE,                            //"AuxV"   // 上位机分两层：第一层板类型 Type
    AUX_TEMPERATURE,                        // "T"
    AUX_PRESSURE,	                          //"P"
    // 	AUX_PH,                                 //"pH"
    AUX_EXTERNALCHARGE,                     //"Ext_Charge"
    AUX_FLOW_RATE,                          // "FR"
    AUX_AO,
    //AUX_DENSITY,                            //"Conc"
    AUX_DI,                                 //"DI"
    AUX_DO,
    AUX_HUMIDITY,														//"Uart Humidity"
    AUX_CURRENT,
    AUX_SAFETY,
    AUX_ACIM,
    AUX_BATTERYCELLS,                       //"BATTERYCELLS"
    //NUM_OF_AUX_CHAN_TYPE_NULL,              //"NULL"
    ALLTYPE,
    //IV_TYPE,
};

// TE_AUX_CHAN_METER_DATA 继承于TE_AUX_CHAN_TYPE 注意同步更新修改
enum TE_AUX_CHAN_METER_DATA
{
    AUX_VOLTAGE_DATA,                            // "AuxV" data
    AUX_TEMPERATURE_DATA,                        // "AuxT" data
    AUX_PRESSURE_DATA,
    AUX_EXTERNALCHARGE_DATA,                     //"Ext_Charge"
    AUX_FLOW_RATE_DATA,                          // "FR"
    AUX_AO_DATA,
    AUX_DI_DATA,                                 //"DI"
    AUX_DO_DATA,
    AUX_HUMIDITY_DATA,														//"Uart Humidity"
    AUX_VOLTAGE_DVDT_DATA = 30,                  // "AuxV" dvdt data
    AUX_TEMPERATURE_DTDT_DATA = 31,              // "AuxT" dtdt data
    AUX_PRESSURE_DPDT_DATA = 32,
    AUX_EXTERNALCHARGE_DXDT_DATA,                     //"Ext_Charge"
    AUX_FLOW_RATE_DXDT_DATA,                           // "FR"
    AUX_AO_DXDT_DATA,
    AUX_DI_DXDT_DATA,                                  //"DI"
    AUX_DO_DXDT_DATA,
    AUX_HUMIDITY_DXDT_DATA,
};


enum En_AuxSubType      //Add by DKQ 05.11.2015
{
    // for Voltage type sensors
    Regular_2ndV=  100,   //100
    HIM_2ndV,
    BatteryPack_2ndV,
    Monitor_Vol,
    LC_HIM_2ndV,
    ADC_Power_Vol,


    // for Temperature type sensors
    J_ThermoCouple=200,     //200
    K_ThermoCouple,
    T_ThermoCouple,
    E_Thermocouple,
    Pt100_Thermistor,
    UART_TC_WF4,
    UART_TC_SCP200,
    TC_2ndV,
    Omega44006_Thermistor,
    T_Aux_Input, //use this as auxiliary AI input
    RTH44BS103F_Thermistor,
    // for Pressure type sensors

    Regular_Pressure=300,    //300
    P_Aux_Input, //use this as auxiliary AI input

    //
    EC_Current=400,
    Monitor_Current = 401,

    ACIM_Cur = 500,
    ACIM_Vol,
    ACIM_RMS,


    // for General Uart Chamber
    UART = 1000,
    //for DAC Channels
    DAC_Flowrate=1100,
    DAC_Pressure,
    DAC_Temperature,
    DAC_ACIM_Vol,
    DAC_Power_Vol,

};

enum En_BoardType
{
    Aux_2ndV_TR_509146 = 0,
    Aux_pH_TC_507074,
    Aux_MultiFunc_514194,
    Aux_P_TC_509130,
    Aux_2ndV_HIR_511898,
    Aux_2ndV_BS_601440,   //5
    Aux_Redun_2ndV_TC_601082,
    Aux_ExtCharge_601618,
    Aux_Monitor_601618,
    Aux_2ndV_LC_601310,
    Aux_TR_PT100_601314,  //10
    Aux_DIDO_601196,
    Aux_OnlineRSM_602026,
    Aux_AIO_DIO_602178,
    Aux_MADC_TC_602280,
    Aux_TEC_Chamber_601932,      //15 Addd by DKQ
    Aux_MADC_TC_HS_602176,
    Aux_WMADC_TC_HS_602484,
    Aux_WMADC_TC_602524,
    Aux_WMADC_DIDO_602518,
    Aux_WMADC_PT100_602532,  //20
    Aux_WMADC_2ndV_TR_602536,
    Aux_WMADC_AIO_DIO_602548,
    Aux_WMADC_2ndV_HIR_602540,
    Aux_WMADC_2ndV_LC_602528,
    Aux_WMADC_2ndVM_LC_603072,  //25
    Aux_WMADC_2ndVL_LC_603130,
    Aux_DDS_ACIM_ACR_408888,
    Aux_BulitIn_16V_409386,
    Aux_BulitIn_16T_409920,
    Aux_BuiltIn_8V8T_409924, //30
    Aux_DDS_ACIM_ACR_603612,
    Aux_WMADC_PT100_603616,
    Aux_MTC_Control_Board_415132,					//Add for Cold Plate by ZC 04/05/2018
    Aux_Sys_Control_PWM_416326,
    Aux_Sys_Control_Liner_416326,   //35
    Aux_WMADC_TC_415088,
    Aux_AOAI_ConstantVoltageSource,   //  2019.4.8  yy
    Aux_DDS_ACIM_415466,
    Aux_WMADC_PT100_416772,
    Aux_WMADC_DIDO_416714,//40
    Aux_WMultiFunc_415088,
    Aux_PowerADJ_200A_412206,
// 	Aux_SYSCTRLBOARD_415016,
    Aux_DDS_Sinwave_Current_Source_604700,
    Aux_MZTC_Control_Board_417118,    // Add for New 2nd version MZTC by ZC 12/013/2019
    Aux_PowerADJ_2US6K_422108,//45
    Aux_PowerADJ_2UD300A_422206,//46为stm32的Aux_PowerADJ_2UD300A_422206电源板，与listenfornet保持一致
    Aux_WMADC_2ndV_VL_430834,
    Aux_WMultiFunc_430938,
    Aux_WMADC_2ndV_HIR_441518,
    Aux_WMultiFunc_441518,//50
		Aux_PowerADJ_D300A_4_CHAN,//only stm32	MitsX			
	  Aux_BuiltIn_64_X,//only stm32	MitsX	
		Aux_MZTC_Control_Board_441324,
		Aux_WMADC_PT100_443948,
	Aux_MZTC_Control_Board_444136,//55
    NUM_OF_AUX_BOARD_NULL,

};
enum En_BuiltInSlaveBoardType
{
    Aux_BI_VL_16V_409932= 1,
    Aux_BI_VH_8V_409936,
    Aux_BI_TC_16T_409928,
    NUM_OF_Aux_BI_SLAVEBOARD_NULL,

};
enum TE_AUX_CHAN_T_SENSOR_TYPE
{
    //   	AUX_TCT_E,                              //"E-type Thermocouple"     // 上位机分两层：第二层板类型的传感器 Thermocouple
    //   	AUX_TCT_J,                              //"J-type Thermocouple"
    //   	AUX_TCT_K,                              //"K-type Thermocouple"
    //   	AUX_TCT_T,                              //"T-type Thermocouple"
    AUX_TEMP_SENSOR_THERMOCOUPLE,            // Thermocouple  类温度传感器
    AUX_TEMP_SENSOR_THERMISTOR,             //"Thermistor"
    AUX_TEMP_SENSOR_DEWPOINT,               //"Dew Point"
    AUX_TEMP_SENSOR_LINEAR,                 //"Linear"
    AUX_TEMP_SENSOR_SIGMADELTA,             //"Sigma Delta"
    NUM_OF_AUX_CHAN_TEMP_SENSOR_TYPE,       //"NULL"
};

enum TE_THERMOCOUPLE_TYPE
{
    TC_E,                              //"E-type Thermocouple"     // 辅助类型，温度传感器类型，热点偶类型  03.19.2014
    TC_J,                              //"J-type Thermocouple"    // 上位机分两层：第三层板类型的传感器 Sensor
    TC_K,                              //"K-type Thermocouple"
    TC_T,                              //"T-type Thermocouple"
    TC_NUM_OF_THERMOCOUPLE_TYPE,
};

enum TE_BOARD_PC_CONTROLTYPE
{
    CONTROLTYPE_USE_ALL            	=0,     //pCmd->m_btCtrl_Type;上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息 （预留开发 3：单独下载Map表 4：单独下载校正值）
    CONTROLTYPE_USE_BOARD,
    CONTROLTYPE_USE_CHANNEL,
    CONTROLTYPE_USE_MAP,
    CONTROLTYPE_USE_CALIBRATE,
};

enum TE_CAN_PORT_CONTROLTYPE
{
    CAN_PORT_USE_EXCLUSIVE         =0,           //  专属辅助通道
    CAN_PORT_USE_PUBLIC,                         //  公共辅助通道
    CAN_PORT_USE_BUILD_IN,                       //  内建辅助通道
};

enum TE_REPORT_I2CRSN_ERROR_MESSAGE
{
    ERROR_RSN_NONE,
    ERROR_RSN_NONEWRITEBOARD,
    ERROR_RSN_SYSTEMFINDBOARD,
    ERROR_RSN_OTHERSYSTEMUSEBOARD,
    ERROR_RSN_JUSTWRITEBOARD,
    ERROR_RSN_LOADBOARD2CH,
    ERROR_RSN_READ_CHCALIB,
    ERROR_RSN_WRITERSN_CMP,
    ERROR_RSN_RESET_CONFIG,
    ERROR_RSN_I2CFUN_TIMEOUT,
    ERROR_RSN_WAITPC_TIMEOUT,
    ERROR_RSN_HAVE_LOSTBOARD,
    ERROR_RSN_LOST,
};

enum TE_MESSAGE_CODE
{
    MESSAGE_UPDATA_RSN,
    MESSAGE_WRITE_RSN,
    MESSAGE_WRITE_CALIB,
    MESSAGE_STOP_REPORT,
};

enum TE_MESSAGE_SOURCE
{
    MESSAGE_SOURCE_NONE,
    MESSAGE_SOURCE_BOARD_RSN=1,    //   AUX MCU
    MESSAGE_SOURCE_CHANNEL_CALIB,
    MESSAGE_SOURCE_PC_ORDER,
    MESSAGE_SOURCE_I2CFUNTION,
    MESSAGE_SOURCE_IV_WARNNING,   //   IV MCU Warnning
};

enum TE_IV_ERROR_TYPE
{
    IV_ERROR_TYPE_NORMAL,
    IV_ERROR_TYPE_WARNNING,
};

enum TE_IV_ERROR_CODE
{
    IV_ERROR_WARNNING_NONE,
    IV_ERROR_WARNNING_CTRL_VALUE_CHK,
    IV_ERROR_WARNNING_IRREGULAR,
    IV_ERROR_WARNNING_IR,
    IV_ERROR_WANRING_MTC,
};

//Schedule 状态
enum
{
    NOTREADY,
    IDLE,
    START,
    RUNNING,
    GOPAUSE,
    RESUME,
    GOSTOP,
    FIXEDUNSAFE,
    DYNAMICUNSAFE,
    WAITFORAUX,
    WAITPowerReady,   //等待功率电调节完成
};

enum TE_IV_CH_STATUE
{
    CH_STATUE_IDLE,
    CH_STATUE_CHARGE,
    CH_STATUE_DISCHARGE,
    CH_STATUE_PULSE,
    CH_STATUE_SIMULATION,
    CH_STATUE_IR,
    CH_STATUE_ACR,
};

enum TE_COM_CHAMBERTYPE
{
    TC_NA,
    TC_ColeParmer,
    TC_570i,  //  TC_430i,
    TC_2204E,
    TC_SCP220,
    TC_P91,

    TC_PC200,
    TC_8200,
    TC_Voetsch,
    TC_WATLOW_F4,
    TC_WATLOW_96,

    TC_WATLOW_942,
    TC_SAMWONTECH_TEMP880,
    TC_THS2005,
    TC_3216,
    TC_TEMI2500,

    TC_S7_226CN,
    TC_TPC7062K,
    TC_SAMWONTECH_TEMP2500,
    TC_CLCD_9700,
    TC_SHANGHAI_PINDUN_TEMI880,

    TC_T1,
    TC_T2,
    TC_T3,
    TC_T4,

    TC_MB1,
    TC_430i,
    TC_TT_5166,  // =27,
    TC_GUANGZHOU_WUSUO,  // =28,
    TC_DONGGUAN_HUANCE_UMC1200,  // =29,
    TC_Voetsch_VC_L4006,  // =30,
    TC_CHONGQING_YINHE_MODBUSTCP, // =31,
    TC_HLT403P, // = 32,
    TC_HLT504P, // = 33,
    TC_JULABO_FP50MA,   // = 34,
    TC_CHONGQING_ATAIHE, // = 35,
    TC_JUFU_9700S, // = 36,
    TC_SUN_EC1X,// = 37
    TC_ESPEC_GPL_4, // = 38
    TC_JULABO_A45T, // = 39
    TC_TM221CE24T, // = 40
    TC_US_9256P_ACCU3, // = 41
    TC_SIEMENS_S7_SAIA, // = 42
    TC_9700S_2C, // = 43
    TC_SIEMENS_S7_XC1001A_SAIA,	// = 44
    TC_LAUDA_PRO_P2E, // = 45
    TC_HITACHI_EC15MHP, // = 46
    TC_Voetsch_Network,	// = 47
    TC_TEMI2500_MODBUS2,	// = 48
    TC_HLT504P_ONLY_TEMP, // = 49
    TH_MEMMERTGMBH_RS232,	// = 50
    TC_MT8071,// = 51
    TC_SCP220_ONLY_TEMP, // 52
    TC_ESPEC_GPL_4_ONLY_TEMP, //53
    TC_MB2, //54
    TC_U740, // 55
    TC_SIMPAC_TCP, //56
    TC_TT_5166_Sunwoda, //57
    TC_SONGHUA_2PT_BD, //58
    TC_9700S_2C_ONLY_TEMP, //59
    TC_DONGDA_TCY1023,//60
    TC_SIEMENS_S7_XC1001A_SAIA_ONLY_TEMP,//61
    TC_MT8071_WITH_HUMIDITY,//62
    TC_POLYSCIENCE_CA02A1P1,//63
    TC_BIA_CLIMATIC_MODBUS_TCP,//64
    TC_ZUNDAR_AZ1000,//65
    TC_HONGRUIDA_MODBUS_TCP,//66
    TC_SPI2_P1,//67
    TC_SIGMASYSTEMS_C4,//68
	TC_WATLOW_F4_MODBUS_TCP,//69
		TC_HONGRUIDA_MODBUS_RS485,//70
		TC_HONGRUIDA_CAN,//71
		TC_KRY_55W_CAN,//72
		TC_SIEMENS_S7_200,//73
		TC_HARDING_THREE_LAYERS_MODBUS_TCP,//74
		TC_PLC_700_ASCII,//75
		TC_V0_45A1,//76 哈丁双层
		TC_MT8071_DOUBLE_LAYER,//77三木双层
		TC_GT9700S,//78
		TC_WATLOW_F4T_MODBUS_FLOAT,//79
		TC_H_OYO1000,//80
};

enum TE_COM_BAUD_RATE
{
    TC_BR_1200,
    TC_BR_1800,
    TC_BR_2400,
    TC_BR_4800,
    TC_BR_9600,
    TC_BR_14400,

    TC_BR_19200,
    TC_BR_38400,
    TC_BR_57600,
    TC_BR_115200,
    NUM_OF_TC_BAUD_RATE,
};

enum TE_COM_DATA_BIT
{
    TC_DATA_BIT_5,
    TC_DATA_BIT_6,
    TC_DATA_BIT_7,
    TC_DATA_BIT_8,
    NUM_OF_TC_DATA_BIT,
};

enum TE_COM_PARITY
{
    TC_PARITY_EVEN,
    TC_PARITY_ODD,
    TC_PARITY_NONE,
    NUM_OF_TC_PARITY,
};

enum TE_COM_STOP_BIT
{
    TC_STOP_BIT_ONE,
    TC_STOP_BIT_TWO,
    NUM_OF_TC_STOP_BIT,
};

enum TE_STOP_SOURCE
{
    ST_UNKNOWN               	    =0,
    ST_SCHMANAGE               	    =1,
    ST_THIRDPARTY               	=2,     //Stop事件来源
    ST_STEPMANAGE            	    =3,
    ST_SAFECHECK         	        =4,
    ST_CHECKLIMIT                  	=5,
    ST_ETHERNETPARSE               	=6,
    ST_CANPARSE                   	=7,
    ST_AUXCHANNEL               	=8,
    ST_SMBPARSE                   	=9,
};

//type of board
enum BOARD_SORT
{
    IV_BOARD,
    AUX_BOARD,
    Cali_BOARD,
};

enum TEST_SETTING_ELEMENT_VALUE_TYPE
{
    TSEVT_NUMERICAL             =0,
    TSEVT_AI_V               	=1,
    TSEVT_AI_T               	=2,
    TSEVT_AI_P            	    =3,
};

enum eDampingOn
{
    eDampingOn_Null,
    eCook,
    eDampingOn_DampingOn,
    eDampingOn_DampingOff,
};

enum CANBMS_DATALOG_INTERVAL
{
    NO_DATALOG = 0,
    INTERVAL_500MS = 1,
    INTERVAL_1S = 2,
    INTERVAL_30S = 3,
    INTERVAL_Total = 4, //统计间隔数

}; //170606 zyx

enum VERSION_TYPE  //zyx 180305
{
    TY,
    ZY,
    RD,
    PV,
    ERROR_VERSION = 15,
};

enum AUX_MAP_STATE
{
    AUX_MAP_NA = 0,                      //预留
    AUX_MAP_NORMAL = 1,                  //只有常规辅助
    AUX_MAP_BUILT_IN = 2,                //只有BuiltIn辅助
    AUX_MAP_NORMAL_AND_BUILT_IN = 3,     //同时有常规辅助和BuiltIn 辅助
}; //12.23.2017

enum DataFlag          //SpecialData
{
    NORMAL_POINT        = 0,
    FIRST_POINT         = (1 << 0), //Step 首点
    LAST_POINT          = (1 << 1), //Step 尾点
    CC2CV_POINT         = (1 << 2), //CC转CV的点
    UNSAFE_POINT        = (1 << 3), //Unsafe 点
    WARNING_POINT       = (1 << 4), //Warning 触发点
    BMS_CONTORL_POINT   = (1 << 5), //CAN BMS控制请求点
    SMB_CONTORL_POINT   = (1 << 6), //SMB 控制请求点
    SIMLATION_POINT     = (1 << 7), //Simulation 步骤点
    PLUSE_POINT         = (1 << 8), //脉冲的点
    SDL_POINT           = (1 << 9), //无log limit，按SDL规则转成limit log的点
    IR_POINT            = (1 << 10),//测量IR的点
    BURSTMODE_POINT     = (1 << 11),//Burstmode的点
    RESUME_POINT        = (1 << 12),//resume
    SET_VALUABLE_POINT  = (1 << 13),
    SET_VALUE_POINT     = (1 << 14),
    STOP_POINT =	(1 << 15),
    DYNAMIC_CONTROL_ON = (1 << 16),         //dynamic control
    CURRENT_FILTER_OFF = (1 << 17),         // 滤波
    POWER_MANAGE_FUNCTION_ON = (1 << 18),   //功率管理
    CURRENT_COARSETUNE_OFF = (1 << 19),     //粗调
    CURRENT_FINETUNE_OFF = (1 << 20),       //微调
    IR_VALUE_NULL = (1 << 21),              //IR值
    SIMU_CLAMPDIGITVOLT_ON = (1 << 22),     //仿真钳位电压
    CURRENT_SOFTWARECLAMP_ON = (1 << 23),   //电流钳位
    POWER_SOFTWARECLAMP_ON = (1 << 24),     //功率钳位
}; //180308 zyx  //mits8

enum DEBUG_DATA_TYPE
{
    DATA_TYPE_CHAR,
    DATA_TYPE_UCHAR,
    DATA_TYPE_SHORT,
    DATA_TYPE_USHORT,
    DATA_TYPE_INT,
    DATA_TYPE_UINT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DEBUG_DATA_TYPE_MAX
};//191225 yzl

//bit
enum DEBUG_ERROR_TYPE
{
    DEBUG_NONE_ERROR,
    DEBUG_ADDR_ERROR = 0x01,
    DEBUG_RANG_ERROR = 0x02,
    DEBUG_TYPE_ERROR = 0x04
};//191225 yzl
#ifndef __MSTIME__
#define __MSTIME__
//字节对齐子类型结构体定义
#pragma pack(1)
typedef struct
{
    unsigned long      Second;
    unsigned short     Us100;
} MsTime;
typedef struct
{
    unsigned long     Second;
    unsigned long     Us;
} TIME2US;
#pragma pack()
#endif

#pragma pack(1)
typedef struct
{
    unsigned long      u32Second;
    unsigned short     u16Us100;
    unsigned char      ucType:5;
    unsigned char      bIV:1;
    unsigned char      ucSensor:2;
    unsigned char      ucValueType;
    float              fValue;
    unsigned char      ucChannelIdx;   // 结构共计:13字节
    //unsigned char	  ucIVChIdxTriggerer;
} St_SDL_Point_Pack;
#pragma pack()

#pragma pack(1)
typedef struct
{
    unsigned char ucValueType;
    float				fValue;
    unsigned char ucChanneldx;

} St_SDL_OnePoint;
#pragma pack()

#pragma pack(1)
typedef struct
{
    unsigned long u32Second;
    unsigned short u16Us100;
    unsigned char ucType:5;
    unsigned char bIV:1;
    //unsigned char ucIVChIdxTriggerer;
    unsigned char IvUnitIP;
    unsigned char IvChnIndex;
    bool 				lastPoint;
    unsigned char itemCount;
    St_SDL_OnePoint points[32];
} St_SDL_GroupPoints;

#pragma pack(1)
typedef union
{
    float fValue[6]; // 电流/电压   充电电量/放电电量，  充电能量/放电能量，  dV/dt,  dV/dQ等。 //4->6
    struct
    {
        BYTE ucType;
        float fValue;
    } dat[3];
} UN_DATA_POINT;
typedef struct  //mits8
{
    unsigned long      u32Second;
    unsigned short     u16Us100;
    unsigned char      ucType:2;
    unsigned char      ucDataLogFormat:3;
    unsigned char      bIV:1;
    unsigned char      ucSensor:2;
    unsigned char      ucChannelIdx;
    unsigned long	   u32TestID;
    unsigned long      u32DataPoint;
    double             dTestTime;
    double             dStepTime;
//     float              fTestTime;
//     float              fStepTime;
    unsigned short     u16Cycle_Idx;
    unsigned short     u16Step_Idx;
    unsigned long      u32DataBitFlag;  //Bit 位 标记意义0:Step 首点1:Step 尾点2:CC转CV的点3:Unsafe 点4:Warning 触发点 5:CAN BMS控制请求点6:SMB 控制请求点7:Simulation 步骤点8:脉冲的点9:无log limit，按SDL规则转成limit log的点10:测量IR的点11:Burstmode的点  其他等等
    unsigned char      ucDataCount;
    UN_DATA_POINT            un_data;
} St_Data_Pairs;     // 结构共计:48字节

typedef struct  //mits8
{
    unsigned long      u32Second;
    unsigned short     u16Us100;
    unsigned char      ucType:2;
    unsigned char      ucDataLogFormat:3;
    unsigned char      bIV:1;
    unsigned char      ucSensor:2;
    unsigned char      ucChannelIdx;
    unsigned long	   u32TestID;
    unsigned long      u32DataPoint;
    double             dTestTime;
    double             dStepTime;
    unsigned long      u32Cycle_Idx;
    unsigned short     u16Step_Idx;
    unsigned long      u32DataBitFlag;  //Bit 位 标记意义0:Step 首点1:Step 尾点2:CC转CV的点3:Unsafe 点4:Warning 触发点 5:CAN BMS控制请求点6:SMB 控制请求点7:Simulation 步骤点8:脉冲的点9:无log limit，按SDL规则转成limit log的点10:测量IR的点11:Burstmode的点  其他等等
    unsigned char      ucDataCount;
    UN_DATA_POINT            un_data;
} St_Data_Pairs_4BytesCycleIdx;
#pragma pack()

//子类型结构体定义
typedef unsigned short CANBMS_METACODE, CANBMS_METACOUNT, CANBMS_METAIdx; //170505 zyx 统一CAN BMS MV变量的数据类型

typedef struct
{
    unsigned short     Year;                      // Yearvalue-[0,4095]
    unsigned short     Day;                       // Dayofyearvalue-[1,365]
    unsigned char      Hour;                      // Hourvalue-[0,23]
    unsigned char      Minute;                    // Minutevalue-[0,59]
    unsigned char      Second;                    // Secondvalue-[0,59]
    unsigned char      reserved1;
    unsigned short     Us100;                     // 100usunitsvalue-[0,10000]
    unsigned short     reserved2;                 // 结构共计:12字节
} St_RecordTime;

// typedef struct
// {
// 	MsTime             Time;                      // FileTime?结构待定,6字节
// 	unsigned char      ucType;
// 	unsigned char      ucReserved[3];
// 	unsigned char      ucValueType;               // 值类型，有:fCurrent,fVoltage,fIR,fCCapacity,fDCapacity,fCEnergy,fDEnergy7种类型；
// 	unsigned char      bCriticalPoint;            // 是否为关键点，（在考虑压缩的时候不能删除的数据点）
// 	unsigned short     u16ChannelIdx;             // 数据点通道号
// 	float              fValue;                    // 数据项浮点值 //  结构共计:18字节
// } St_SDL_Point;

typedef struct
{
    MsTime             Time;                      // FileTime?结构待定,6字节
    unsigned char      ucType;
    unsigned char      ucReserved[3];
    unsigned char      ucDataLogFormat;                 // 值类型，有:SDL_DATA_TYPE_LOG_C_V_CAPACITY,SDL_DATA_TYPE_LOG_C_V_ENERGY,SDL_DATA_TYPE_LOG_C_V_DVDT_DVDQ；
    unsigned char      bCriticalPoint;            // 是否为关键点，（在考虑压缩的时候不能删除的数据点）
    unsigned short     u16ChannelIdx;             // 数据点通道号
    unsigned long	   u32TestID;
    unsigned long      u32DataPoint;
    double              dTestTime;
    double              dStepTime;
//     float              fTestTime;
//     float              fStepTime;
    unsigned short     u16Cycle_Idx;
    unsigned short     u16Step_Idx;
    unsigned long      u32DataBitFlag;
    unsigned char      ucDataCount;               //上传的fValue数量
//  	float              fValue[6];                 // 数据项浮点值 //  结构共计:62字节
    UN_DATA_POINT            un_data;
} St_SDL_Point;  //mits8

typedef struct
{
    MsTime             Time;                      // FileTime?结构待定,6字节
    unsigned char      ucType;
    unsigned char      ucReserved[3];
    unsigned char      ucDataLogFormat;                 // 值类型，有:SDL_DATA_TYPE_LOG_C_V_CAPACITY,SDL_DATA_TYPE_LOG_C_V_ENERGY,SDL_DATA_TYPE_LOG_C_V_DVDT_DVDQ；
    unsigned char      bCriticalPoint;            // 是否为关键点，（在考虑压缩的时候不能删除的数据点）
    unsigned short     u16ChannelIdx;             // 数据点通道号
    unsigned long	   u32TestID;
    unsigned long      u32DataPoint;
    double              dTestTime;
    double              dStepTime;
//     float              fTestTime;
//     float              fStepTime;
    unsigned long      u32Cycle_Idx;
    unsigned short     u16Step_Idx;
    unsigned long      u32DataBitFlag;
    unsigned char      ucDataCount;               //上传的fValue数量
//  	float              fValue[6];                 // 数据项浮点值 //  结构共计:62字节
    UN_DATA_POINT            un_data;
} St_SDL_Point_4BytesCycleIdx;  //mits8

typedef struct
{
    unsigned char      m_ucData[16];
} St_MD5;

typedef struct
{
    St_MD5             m_MD5;
    unsigned char      m_ucType;
    unsigned char      m_bOk;
    unsigned char      m_bError;
    unsigned char      m_bReserved;               // 结构共计:20字节
} St_MD5Assign;

typedef struct
{
    unsigned short     m_u16ChannelIdx;
    unsigned char      m_ucMD5Count;
    unsigned char      m_ucReserved;
    St_MD5Assign       m_MD5Assign[IV_MAX_ASSIGN_MD5_NUMBER];// 结构共计:324字节
} St_Assign;

typedef struct
{
    unsigned char  m_bOutboundMsg;//190617,hpq
    unsigned char  m_Reserve[3];  //190617,hpq
    unsigned int   m_Period;
    unsigned int   m_ID;
    unsigned char  m_Data[8];
    unsigned char  m_FrameType;
    unsigned char  m_Length;
    unsigned char  m_SignalStop;
    unsigned char  m_AllStop;
} St_CAN_SendMsg;
typedef struct
{
    unsigned char m_Register;
    unsigned char m_Len;
    unsigned char m_Data[16];
} St_SMB_SendMsgString;

typedef struct
{
    unsigned char m_Register;
    //unsigned char m_Len;
    unsigned short m_Data;
} St_SMB_SendMsgShort;

typedef union
{
    St_SMB_SendMsgShort 	m_SMB_SendMsShort;
    St_SMB_SendMsgString 	m_SMB_SendMsString;
} St_SMB_SendMsg;

typedef union
{
    St_MD5             m_MD5;                     // 对于控制值，如果控制方式是simulation,pulse，这里就使用MD5
    float              m_fControlValue[5];        // 如果是其他，这里的控制值就是5个浮点数,第五个预留为battery simulation的初始容量使用  // 结构共计:36字节
    St_CAN_SendMsg      m_CANValue;
    St_SMB_SendMsg      m_SMBValue;            //支持SMB寄存器写字节
} Un_CtrlValues;

typedef struct
{
    unsigned short     m_u16StepID;
    unsigned char      m_ucStepLimitNo;
    unsigned char      m_ucLogLimitNo;            // 可能不用
    unsigned short     m_u16ControlType;
    unsigned char      m_ucCurrentRange;
    unsigned char      m_ucVoltageRange;
    float              m_fMaxCurrent;//171113 hpq,add for pro8
    Un_CtrlValues      m_CtrlValues;
    unsigned char      m_CtrlValueOperator[5];
    unsigned char      m_ucCtrlValue_DataType[5]; // 4个控制值的数值类型：常数、MetaVariable、Formula...    // 结构共计:48字节   //changed by zc 06.05.2018
    float              m_CtrlValueCoefficients[5];
    short              m_ucTestSettingIdx;   //01.21.2015
    unsigned char      m_DampingOn;
    unsigned char      m_BurstModeSample:1;
    unsigned char      m_ucResveredBit:7;

} St_StepCtrl;

typedef struct
{
    float              m_fLeftValue;
    float              m_fRightValue;
    float              m_fLeftValueCoefficient;
    float              m_fRightValueCoefficient;
    unsigned char      m_ucLeftValue_Operator;
    unsigned char      m_ucRightValue_Operator;
    unsigned char      m_ucLeftValue_DataType;
    unsigned char      m_ucRightValue_DataType;
    unsigned short     m_u16GotoStepID;
    unsigned char      m_ucCompareSign;
    unsigned char      m_bStepLimit:1;
    unsigned char      m_bReached:1;
    unsigned char      m_bChecked:1;
    unsigned char      m_Reserved1:5;
    unsigned char      m_ucReserved[2];
    unsigned char      m_ucEquationID;            // 本limit在上位机的equation序号
    unsigned short     m_u16LimitID;              // 本limit在上位机的limit序号
    unsigned char      m_uCheckPlace;
    bool               m_bLimitFormulaHasAux;
    bool               m_bAnyValid;
    float              m_fReserved;               // 结构共计:24字节 + 3
    float              m_fLeft_Value;
    float              m_fRight_Value;            // for Limit  Unsafe 20140211
} St_Limit;

typedef struct
{
    St_StepCtrl        m_Ctrl;                    // 结构共计:72字节
    St_Limit           m_Limits[IV_MAX_LIMIT_NUMBER];
    bool               m_bAuxSDL ;
    bool               m_bNoLogLimit;
    bool               m_bIRTestStep;    //标识该步是作为IR判断加入的   Di 20140617
} St_StepData;

typedef struct
{
    float              m_fAmplitude;
    float              m_fStageTime;
    float              m_fLogInterval;
    unsigned char      m_ucControlType;
    unsigned char      m_ucReserved[3];           // 结构共计:16字节
} St_PulseStage;

typedef struct
{
    St_MD5 m_MD5;																			//Add by DKQ 08.20.2014
    //UINT   m_DataLogMode; 														//
    UINT   m_uStageNum;                               //总stage数
    float  m_StageWidth[MAX_PULSE_STAGE_NUM];         //m_MilliSeconds[MAX_PULSE_STAGE_NUM];
    float  m_StageAmplitude [MAX_PULSE_STAGE_NUM];    //m_Amplitude[MAX_PULSE_STAGE_NUM];
    //bool   m_bFastStage[MAX_PULSE_STAGE_NUM];         //标记是否FastStage，
    //UINT   m_uStageDataPoints[MAX_PULSE_STAGE_NUM];   //

    //以下为4.x版本下的，暂时保留备用
    //林工的设计方案应该是放到step定义里了，这里暂时 保留起来
    //bool   m_bSingleBurst;	                          //是否只爆发一次，// m_bSinglePulse 很混淆，容易被理解成单
    //  UINT   m_uType;                                   //Regular,/GSM/ CDMA /CDMA2 deng 等等，保持不变
    //float  m_fBase;
    //float  m_MilliSecondOfFirstPoint[MAX_PULSE_STAGE_NUM];
    //float  m_fEndingAmp;//07.22.2010 KL add single pulse ending stage
    //UINT   m_uSampleInterval;
} St_PulseData;

typedef struct
{
    unsigned char      m_ucFUNCTION1;
    unsigned char      m_ucFUNCTION2;
    unsigned char      m_ucFUNCTIONX;
    unsigned char      m_ucFUNCTIONY;
    unsigned char      m_ucOPERATOR;
    unsigned char      m_ucReserved[3];
    unsigned char      m_ucDataType[8];
    float              m_fX1;
    float              m_fX2;
    float              m_fX3;
    float              m_fX4;
    float              m_fY1;
    float              m_fY2;
    float              m_fY3;
    float              m_fY4;                     // 结构共计:48字节
} St_FormulaData;


typedef	struct 	//逆波兰Token具体参数
{
    unsigned char m_ucOperator;
    unsigned char m_ucDataType;		//TE_DATA_TYPE
    unsigned char m_ucFuncID;
    unsigned char m_ucParamCount;
    float	m_fvaule;

} St_AdvFormulaData;	// 结构共计:8字节


typedef struct
{
    unsigned short m_u16DataCount;	//实际节点数
    unsigned short reserved[3];
    St_AdvFormulaData    m_AdvFormulaData[MAX_ADV_NODE_COUNT];	//512 Byte
} St_AdvFormula;	//每个schedule支持64个新Formula	//结构共计：520 Byte



enum FormulaOperator
{
    FormulaOperator_Number,
    FormulaOperator_Add,
    FormulaOperator_Sub,
    FormulaOperator_Neg,
    FormulaOperator_Mul,
    FormulaOperator_Div,
    FormulaOperator_Pow,
    FormulaOperator_Mod,
    FormulaOperator_Max,
    FormulaOperator_Min,
    FormulaOperator_Func,
    FormulaOperator_Variable,
};
typedef struct
{
    float              m_fStart;
    float              m_fEnd;
    float              m_fScanRate;               // 结构共计:12字节
} St_CvStage;

typedef struct
{
    unsigned char      m_ucStageCount;
    unsigned char      m_ucReserved[3];
    unsigned long      m_ucRepeatNumber;
    float              m_fBase;                   // 结构共计:12字节
} St_CvCtrl;

typedef struct
{
    St_CvCtrl          m_Ctrl;
    St_CvStage         m_Stages[IV_MAX_CV_STAGE_NUMBER];// 结构共计:132字节
} St_CvData;

typedef struct
{
    float              m_fMin;
    float              m_fMax;                    // 结构共计:8字节
// 	unsigned char      m_bEnabled;
    unsigned char m_nMinType : 4;   //0 数值 1是百分比
    unsigned char m_nMaxType : 4;   //0 数值 1是百分比
//     unsigned char      rvd[3];
	unsigned char 		m_ucDataTypeLow;  // 预留字节修改成 数据类型
	unsigned char 		m_ucDataTypeHigh;  // 预留字节修改成 数据类型
	unsigned char      rvd[1];   //
} St_SchSafeData;
typedef struct
{
    float              m_fDeltaTimeMax;
    float              m_fDeltaTimeMin;
    float              m_fDeltaXMin;
} St_SchdXdt; //171113 hpq,add for pro8
typedef struct
{
    float              m_fDelta;
    unsigned char      m_bEnabled;
    unsigned char      m_bByPercentage;
//     unsigned char      rvd[2];
	unsigned char 	   m_ucDataType;  // 预留字节修改成 数据类型
	unsigned char      rvd[1];   //
} St_SchGlbLogLimit; //171113 hpq,add for pro8
typedef struct
{
    float              m_fDelta;
    //float              m_DV_Time_Max;
    //float              m_DV_Time_Min;
    unsigned char      m_bEnabled;
    unsigned char      m_bByPercentage;
    unsigned short     m_RVD;
    unsigned short     m_SafetyStartIndex;
    unsigned short     m_SafetyChanCount;
} St_SchAux_Glb; //171113 hpq,add for pro8
typedef struct //St_SchClamp
{
    float                m_fHigh;
    float                m_fLow;
    unsigned char        m_ucDataTypeHigh;
    unsigned char        m_ucDataTypeLow;
    unsigned short       rvd;
} St_SchClamp; //171113 hpq add for pro8
typedef struct //St_SchTestObject
{
    float                m_fMass;
    float                m_fSpecificCapacity;
    float                m_fNorminalCapacity;
    float                m_fNorminalIR;
    float                m_fNorminalVoltage;
    float                m_fNorminalCapacitor;
    //float                m_fMaxCurrentDisch;
    float                m_fMaxCurrentCharge;
    float                m_fMinAllowVoltage;
    float                m_fMaxAllowVoltage;
// 	float                m_fVoltClampHigh;
// 	float                m_fVoltClampLow;
// 	St_SchClamp          m_ClampCurrent;
//   St_SchClamp          m_ClampPower;
// 	St_SchClamp          m_ClampVoltage_Simulation;
} St_SchTestObject; //171113 hpq add for pro8
enum
{
    AbnormalGoto_Stop,
    AbnormalGoto_Warning,
};
typedef struct
{
    float   CRM_Deviation; //control result monitoring deviation
    int     CRM_CheckCount; // control-result-monitoring's check count  //CRM_CheckTime //180808 zyx
    bool    CRM_IsNeedCheck; // control result monitoring function use or not
    bool    IIR_IsNeedCheck; // Irregular Impedance Check function use or not
    float   IIR_MaxIR; //If use, the max IR (mOhm) //180808 zyx
    float   IIR_MinIR;
    bool    VTL_IsNeedCheck; //Voltage too low: Voltage lower than % of battery. //1807131 zyx
    float   VTL_Threshold; // Voltage too low: Voltage lower threshold.
    int     VTL_CheckCount; //Voltage too low: check count
} St_AbnormalCheck;

enum
{
    MTCTempSafetyGoto_Stop,
    MTCTempSafetyGoto_Warning,
};

typedef struct		 //MTC Temperature safety check	   06.20.2018 by Chang Zhong
{
    bool                 m_bMTCTempNeedCheck;        //是否温度长时间为达到设定值？是否需要检查？
    unsigned short		   m_uMTCSafetyTimeValue;      //设定的时间
    unsigned char        m_MTCTempSafetyGoto;        //goto Stop/Warning when error checked;
} St_MTCSafetyCheck;

typedef struct //St_Safety_UD  170122, user defined global safety
{
//     float                m_fRvd;
	unsigned char m_ucDataTypeLow;  // 预留字节修改成 数据类型
	unsigned char m_ucDataTypeHigh;  // 预留字节修改成 数据类型
	unsigned char      rvd[2];   //

    bool                 m_bValid;
    unsigned char        m_DataType;
    CANBMS_METACODE      m_MetaCode; //char 170515 zyx
    unsigned char        m_uRvd;
    float                m_fLow;
    float                m_fHigh;
} St_Safety_UD;
typedef struct
{
    bool IsCheckPowerAdj;	//是否启用电阻负载
    float m_ResistorValue;	//电阻值
} St_SchPowerAdj;
typedef struct
{
    bool        bNeedCheck;
    unsigned char      m_Rvdbyte[3];
    float       m_Safety_RedundantVoltage;    // 100%     //07.23.2019
} St_SafetyRedundantVotlage;
typedef struct
{
    St_SchSafeData     m_SafetyCurrent;
    St_SchSafeData     m_SafetyVoltage;
    St_SchSafeData     m_SafetyPower;
    St_SchSafeData     m_SafetyCapacity;
    St_SchSafeData     m_SafetyStepTime;

    St_SchClamp     m_ClampVoltage;
    St_SchClamp     	m_ClampCurrent;       //1811081 zyx
    St_SchClamp     m_ClampPower;
    St_SchClamp    		 m_ClampVoltage_Simulation;

    St_Safety_UD       m_Safety_UD[MAX_USERDEFINED_SAFETY];   //170122, user defined global safety
    St_SafetyRedundantVotlage   m_Safety_RedundantVoltage;                   //07.23.2019
    unsigned char      m_bUseGroupTChamber:1; //zyx 1910101
    unsigned char      m_bResverd:7;
    unsigned char      m_Rvdbyte[248];  //170122, for reserve       //07.23.2019 256 => 248
    St_SchAux_Glb      m_SchAux_Glb_Voltage;
    St_SchAux_Glb      m_SchAux_Glb_Temperature;
    St_SchAux_Glb      m_SchAux_Glb_Pressure;
    St_SchAux_Glb      m_SchAux_Glb_ExtCurrent;
    St_SchAux_Glb      m_SchAux_Glb_FlowRate;
    St_SchAux_Glb      m_SchAux_Glb_Humidity;
    St_SchAux_Glb      m_SchAux_Glb_Condensity;
    St_SchAux_Glb      m_SchAux_Glb_PH;
    St_SchAux_Glb      m_SchAux_Glb_SafetyVoltage;
    St_SchAux_Glb      m_SchAux_Glb_SaftyCurrent;
    St_SchAux_Glb      m_SchAux_Glb_SafetyTemperature;
    St_SchAux_Glb      m_SchAux_Glb_AO;
    St_SchAux_Glb      m_SchAux_Glb_DI;
    St_SchAux_Glb      m_SchAux_Glb_DO;
    St_SchAux_Glb      m_SchAux_Glb_RVD[5];

    St_SchSafeData     m_AuxSafty[512];

    St_SchdXdt         m_dVdt;
    St_SchdXdt         m_dQdV; //09.18.2018
    St_SchdXdt         m_dVdQ;   //09.18.2018

    St_SchGlbLogLimit  m_DV_Voltage;
    St_SchGlbLogLimit  m_DV_Current;
    //St_SchGlbLogLimit  m_DV_Time_Max;
    float              m_DV_Time_Max;
    float              m_DV_Time_Min;

    unsigned char      m_bAllowCurrentRngAutoSwitch;

    //bool               m_bSimuExtraLog;            //普通simualtion步，是否需要LOG额外点？ //20140821,总部决定将这个定义放到STEP里ar
    St_AbnormalCheck   m_AbnormalCheck;            //异常检查：控制值检查、合理性检查
    St_MTCSafetyCheck  m_MTCSafetyCheck;           //MTC温度长时间未达到设定值保护
    St_SchPowerAdj    m_PowerAdjPara;              //使用电阻负载，进行电压跟随。
} St_ScheduleSetting;

typedef struct
{
    unsigned short     m_u16StepCount;
    unsigned short     m_u16FormulaCount;
    unsigned short     m_u16PulseCount;
    unsigned short     m_u16TestSettingCount;
    //unsigned short     m_u16Reserved0;
    unsigned short     m_u16SMBBlockCount;			//新增 //03.29.2018
    unsigned short     m_u16CvCount;

    unsigned short     m_u16AdvFormulaCount;	//新增09.08.2021

    unsigned short     m_u16Reserved[3];   	       // 要求是偶数，保持4字节对齐  // 结构共计:16字节
} St_ScheduleItemCount;
typedef struct											//03.29.2018
{
    UINT m_nElementNum;									//有效块数
    BYTE m_arrElement[MAX_SMB_COMMAND_CONTENT_COUNT];	// 结构共计:132字节
} St_SMBBlockData;

typedef struct
{
    unsigned char      m_ucBoardOperateType;      // 在一次set-3rd时激活即可
    unsigned char      m_ucBattery_Module;        // 电池模式1串入2旁路notcare0
    unsigned char      m_ucCOM_Module;            // V++1COM2notcare0
    unsigned char      m_ucReserved;
    float              m_fClampVH;
    float              m_fClampVL;                // 结构共计:12字节
} St_IndependantCtrl;

typedef struct
{
    float              m_High;
    float              m_Low;                     // 结构共计:8字节
} St_RangeBound;

typedef struct
{
    float              m_Gain;
    float              m_Offset;                  // 结构共计:8字节
} St_RangeCali;

typedef struct
{
    St_RangeBound      m_Bound;
    St_RangeCali       m_Adc;
    St_RangeCali       m_Dac;
    unsigned char      m_bHasDAC;
    unsigned char      m_Reserved[3];
    unsigned long      m_u32DacRampZeroDelayInUs; // maxwell.net机器使用，以后用不上
    float              m_fDacRampZeroOutput;
    float              m_fDacRampRate;            // 结构共计:40字节
} St_IRangeCFG;

typedef struct
{
    St_RangeBound      m_Bound;
    St_RangeCali       m_Adc;
    St_RangeCali       m_Dac;
    St_RangeCali       m_ClampDac;
    St_RangeCali       m_LowClampDac;
    unsigned char      m_bHasDAC;
    unsigned char      m_Reserved[3];             // 结构共计:28字节
} St_VRangeCFG;

typedef struct
{
    St_RangeCali       m_ClampDac;
    St_RangeCali       m_LowClampDac;             // 结构共计:16字节
} St_VClampCFG;

enum eSeriesBoosterRankType
{
    eSeriesBoosterRank_Master,//0
    eSeriesBoosterRank_SubMaster,//1
    eSeriesBoosterRank_Slave,//2
    eSeriesBoosterRank_Serial,//3
    eSeriesBoosterRank_Booster//4
};
typedef struct
{
    unsigned char            m_Group_Mode;            // eSeriesBoosterMode
    signed short             m_Group_MasterID;         //<0 表示没有此通道
    signed short             m_Group_SerialID;          //<0 表示没有此通道
    signed short             m_Group_BoosterID;        //<0 表示没有此通道
    unsigned char            m_SuperGroup_Mode;           // eSeriesBoosterMode
    signed short             m_SuperGroup_MasterID;         //<0 表示没有此通道
    signed short             m_SuperGroup_SerialID;          //<0 表示没有此通道
    signed short             m_SuperGroup_BoosterID;        //<0 表示没有此通道
    signed short             m_SuperGroup_ParallelSlaveChanCount;
    signed short             m_SuperGroup_ParallelSlaveID[MAX_SUPER_GROUP_PARALLEL_SLAVE_COUNT];    //最多支持16个group并联成一个super group
} St_SeriesBoosterMode;
typedef struct
{
    unsigned char      m_ucIRangeCount;
    unsigned char      m_ucVRangeCount;
    unsigned char      m_ucReserved[2];
    St_IRangeCFG       m_IRange[NUM_OF_IV_RANGE_I];
    St_VRangeCFG       m_VRange[NUM_OF_IV_RANGE_V];
    St_RangeCali       m_ACR_R;
    St_RangeCali       m_BuildInV[MAX_BUILDIN_2V_COUNT];
    float              m_fPowerSaftyLimitLow;
    float              m_fPowerSaftyLimitHigh;
    float              m_fCurrentSaftyLimitLow;
    float              m_fCurrentSaftyLimitHigh;
    float              m_fVpp;                    // V++
    float              m_fVmm;                    // V--
    float              m_fSdlFactor_I;
    float              m_fSdlFactor_V;
    float              m_fSdlFactor_Capacity;
    float              m_fSdlFactor_Energy;       // 结构共计:468字节
} St_ChannelCFG;

typedef struct
{
    unsigned char      m_AuxUnitCANID;
    unsigned char      m_ucReserved[3];
    unsigned short     m_AuxChCount[MAXAUXTYPE];  // 结构共计:4+2*8字节 = 20
} St_IVAuxUnitProperty;

typedef struct
{
    unsigned long      m_u32ArbinID;
    unsigned char      m_ucPCA[10];
    unsigned short     m_u16BoardType;
    unsigned short     m_u16ChannelType;
    unsigned short     m_u16IVChannelCount;       // 两个字节

    bool               m_bUseSDL;
    unsigned char      m_uBuildInVoltCount;       // build in voltage count,determined by PC config.
    unsigned char      m_uFilterDepth_I;          // add 20150401,hepeiqing,for curr filter depth setting
    unsigned char      m_uFilterDepth_V;          // add 20150401,hepeiqing,for volt filter depth setting
    unsigned short     m_u16Reserved1;
    unsigned char      m_u8AuxUnitCount;
    St_IVAuxUnitProperty    m_AuxUnits[8];
    float              m_fSdlTime;                // 新增的全局SDL时间间隔，每个CONFIG文件配置一个，下载的时候取出放入UNIT信息下载；
    //Built_in CANBMS  Start//
    unsigned char      m_bHaveVoltLowRng;         //171113,hpq,add for pro8
    unsigned char  m_bCAN1PortUsedAsCANBMS;
    unsigned char  m_bCAN2PortUsedAsCANBMS;
    unsigned char  m_bCAN1PortCANBMSChnIdx;
    unsigned char  m_bCAN2PortCANBMSChnIdx;
    bool               m_PowerRegulation;         // 新增电源功率跟随调节功能， 勾选时True, 不勾选为false
    unsigned char	   m_PowerCrlBoardUnits;     //simba 9.6.2018 功率电压跟随
    unsigned char 			bAutoRangeEnable;     	//允许自动量程切换开关
    unsigned char 			bOne2TwoWay;       		//单边/双边切换开关       0:单向   1：双向
//     unsigned char	   m_fReserved2[2];		      //simba 9.6.2018 功率电压跟随
    float              m_VoltageAdj;
    float              m_fReserved[1];            // 预留的浮点数  // 结构共计:44+160字节 = 204
    //Built_in CANBMS  End//
} St_BoardGlobalCFG;

typedef struct
{
    float              m_fKp;
    float              m_fKi;
    float              m_fKd;                     // 结构共计:12字节
} St_PID;

typedef struct
{
    St_PID             m_VoltagePID[NUM_OF_IV_RANGE_I];
// 	St_PID             m_CurrentPID[NUM_OF_IV_RANGE_I];    //170320,deleted by hepeiqing ,per USA's request
// 	St_PID             m_PowerPID[NUM_OF_IV_RANGE_I];
// 	St_PID             m_LoadPID[NUM_OF_IV_RANGE_I];// 结构共计:144字节
} St_IV_ChannelPID;


//20160906,add by hepeiqing for cali board
typedef struct
{
    unsigned char      bValid;
    float              fMaxValue;
    float              fMinValue;
    float              fShuntValue;   //ohm
    St_RangeCali       ADC_noHP;
    St_RangeCali       ADC_HP;
    St_RangeCali       reserved[4];
} St_CaliBoardRange;
typedef struct
{
    unsigned  long     BoardType;
    float              fBoosterSafetyHigh;
    float              fBoosterSafetyLow;
    float              fBoosterWorkingHigh;
    float              fBoosterWorkingPoint;
    float              fBoosterWorkingLow;
    float              fResistance;
    float              fResSafetyPower;
    float			   fVoltCalibGain;	 //VOLTAGE CALIBRATION GAIN TEXTEDIT  08.10.2017 BY ZC
    float			   fVoltCalibOffset; //VOLTAGE CALIBRATION OFFSET TEXTEDIT  09.22.2017 BY ZC
    float              reserved[96];
} St_CaliBoardGlobal;
typedef struct
{
    St_CaliBoardGlobal global;
    St_CaliBoardRange  CurrentRange[NUM_OF_IV_RANGE_I];
    St_CaliBoardRange  VoltageRange[NUM_OF_IV_RANGE_V];

    float              reserved[100];
} St_CaliBoardCfg;
typedef struct
{
    unsigned long long int    m_U64Time;          // 结构共计:8字节
} ST_GETTIME_DATA;

typedef struct
{
    WORD               m_wMultPackFlag;           // （多包信息管理第一区域)beginning
    WORD               m_wPackTotal;
    WORD               m_wPackNo;
    WORD               m_wNone_21;                // 预留reserved
    DWORD              m_dwTotalLength;
    DWORD              m_dwPack_BufferArrayAddress;
    DWORD              m_dwPackLength;
    DWORD              m_dwNone_41;               // 预留reservedend // 结构共计:24字节
} ST_MultPack_FirstField;

typedef struct
{
    St_MD5             m_StMD5Code;               // MD5128bit（多包信息管理第二区域)beginning
    WORD               m_wChannelNumber;          // （多包信息管理第二区域)
    WORD               m_wTotalChannelNumber;     // 结构共计:20字节
} ST_MultPack_SecondField;

typedef struct
{
    ST_MultPack_FirstField    m_stMFirstField;    // （多包信息管理第一区域)
    ST_MultPack_SecondField    m_stMSecondField;  // （多包信息管理第二区域)  // 结构共计:44字节
} ST_MultPack;

typedef struct
{
    WORD               m_wCtrlType;
    BYTE               m_btCurrentRange;
    BYTE               m_btVoltageRange;
    BYTE               m_btExtend_Def0;
    BYTE               m_btExtend_Def1;
    WORD               m_wNone_21;                // 预留reserved
    float              m_fCtrlVal[4];
    DWORD              m_dwNone_41;               // 预留reserved
    WORD               m_wCtrlVal_Type[4];
    BYTE               m_btNone_11[4];            // 预留reserved
    float              m_fDelayTime;
    DWORD              m_dwNone_42;               // 预留reserved
    DWORD              m_dwNone_43;               // 预留reserved  // 结构共计:52字节
} ST_STEPCONTROL_DATA;

typedef struct
{
    BYTE               m_btControlState;
    BYTE               m_btNone_11[3];            // 预留reserved
    float              m_fCurrent;
    float              m_fVoltage;                // 结构共计:12字节
    float              m_fBuildInVolt[MAX_BUILDIN_2V_COUNT];
} ST_READDATAORSTATE_VARIABLE;

typedef struct
{
    BYTE               m_btControlState;
    BYTE               m_btNone_11[3];            // 预留reserved
    float              m_PhysicalValue;
    DWORD              m_ADCValue;                // 结构共计:12字节
} ST_READAUXINFO_VARIABLE;

typedef struct
{
    WORD               m_wSafetyLimits_Type;
    WORD               m_wNone_21;                // 预留reserved
    St_SchSafeData     m_stCurrent;
    St_SchSafeData     m_stVoltage;
    St_SchSafeData     m_stPower;
    St_SchSafeData     m_stVoltageClamp;
    St_SchSafeData     m_AuxVoltSafty;
    St_SchSafeData     m_AuxTempSafty;
    St_SchSafeData     m_AuxPresSafy;
    St_SchSafeData     m_AuxECSafty;
    St_SchSafeData     m_AuxFRSafty;
    St_SchSafeData     m_AuxConcSafty;
    St_SchSafeData     m_PHSafty;
    St_SchSafeData     m_st[3];                   // 预留reserved
    float              m_fSafetyChecking_Count;
    float              m_fdVdt_Interval;
    DWORD              m_dwNone_41;               // 预留reserved
    WORD               m_wParallel_ChannelNumber;
    BYTE               m_btMaster_Flag;
    BYTE               m_btCalibartion_Flag;
    WORD               m_wWorkstation_ChannelNumber;
    WORD               m_wNone_22;                // 预留reserved  // 结构共计:88字节
} ST_SCHEDULE_SETTING_DATA;

typedef struct
{
    WORD               m_wCustomLogVariables_Count;
    WORD               m_wCustomerLogValue_Type[MAX_SEND_METAVARIABLE];// 结构共计:62字节
} ST_SCHEDULE_LOGSETTING_DATA;

typedef struct
{
    DWORD              m_dwStartAddress;
    WORD               m_wLength;                 // 结构共计:6字节
} ST_STEPARRDRESS_MANAGE;

typedef struct
{
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[2];            // 预留reserved
    St_MD5             m_St_LastMD5Code;          // MD5128bit
    WORD               m_wLast_StepID;
    WORD               m_wTarget_StepID;
    DWORD              m_dwLast_CycleID;
    DWORD              m_dwNew_CycleID;
    BYTE               m_btForceFlag;             // 强迫跳转
    BYTE               m_bt_Start_and_Pause;      // StartandPause
    BYTE               m_bt_ShareSchedule;        // ShareSchedule
    BYTE               m_btNone_12;               // 预留reserved
    DWORD              m_dwNone_41[2];            // 预留reserved  // 结构共计:40字节
} ST_Schedule_Jump;

typedef struct
{
    WORD               m_wVariable_Code;
    WORD               m_wNone_21;                // 预留reserved
    float              m_fVariable_Value;         // 结构共计:8字节
} ST_SCHEDULE_VARIABLE;

typedef struct
{
    float              m_fLeft_Value1;
    float              m_fRight_Value1;
    float              m_fLeft_Value2;
    float              m_fRight_Value2;
    float              m_fLeft_Value3;
    float              m_fRight_Value3;           // 24字节    //for Limit Unsafe 20140211
} St_UnsafeLimit;


typedef struct
{
	float			   f_UnsafeValue;
	float			   f_UnsafeChnID;
	float			   rsv;
}St_UnsafeInfo;

typedef union
{
	float              f_UnsafeSpecial[3];
	St_UnsafeInfo	   st_UnsafeInfo;			   
}Un_UnsafeSpecialInfo;

typedef struct
{
    float              f_UnsafeCurrent;
    float              f_UnsafeVoltage;
    float              f_UnsafePower;
    float              f_UnsafeCapacity;
    float              f_UnsafeStepTime;
    float              f_UnsafeAmp;
    WORD               f_UnsafeInverter_Fuse;
//     float              f_UnsafeSpecial[3];        // 24字节    //for  Limit Unsafe  20140211
    Un_UnsafeSpecialInfo   un_UnsafeSpecialInfo;
	float f_UnsafeSubmasterValue;

} St_UnsafeMetaValue;

typedef struct
{
    float              f_Value;
    float              f_Rvd;
} St_UnsafeUD_Value;

// typedef union
// {
//     St_UnsafeLimit     st_UnsafeLimit;            // 联合体24字节
//     St_UnsafeMetaValue    st_UnsafeMetaValue;     // for Limit Unsafe20140211
// } Un_UnsafeValue;

typedef struct
{
    St_UnsafeLimit        st_UnsafeLimit;            // 48字节
    St_UnsafeMetaValue    st_UnsafeMetaValue;     // for Limit Unsafe20140211
    St_UnsafeUD_Value     st_UnsafeUD_Value;
} St_UnsafeValue;

typedef struct
{
    unsigned char      m_ucType;   // AUX_VOLTAGE,AUX_TEMPERATURE,AUX_PRESSURE,......
    unsigned char       m_ucTurnOn;
    unsigned char       m_ucWaitUntilHit;
    unsigned short      m_uwAuxVirtIndx;  // AUX虚拟ID
    int                m_nCtrlValue3Type;   // 1:2nd;2:T;3:Pressure
    unsigned char      m_ucCtrlValue_DataType[1]; // 控制值的数值类型：常数、MetaVariable、Formula...  //  20190926 yy
    unsigned char      m_CtrlValueOperator[1];	  //  20190926 yy
    float              m_CtrlValueCoefficients[1];   //  20190926 yy
    float              m_fCtrlValue1;
    float              m_fCtrlValue2;
    float              m_fCtrlValue3;
    float              m_fTolerance;
    float              m_fP;
    float              m_fI;
    float              m_fD;  // Modify by qjm 20160816

} St_TestSettingElement; //01.21.2015 共20字节


typedef struct
{
    unsigned char         m_ucElementNum;
    unsigned char         m_ucResumeable;
    unsigned char         m_ucReserved[2] ;
    St_TestSettingElement m_TestSettingElement[MAX_TEST_SETTING_ELEMENT_COUNT];
} St_TestSetting; //01.21.2015  //4+ 32 * 20 = 644字节


typedef struct
{
    BYTE               m_btStopped_Status;
    BYTE               m_btCMD_Source;
    BYTE               m_btCMD_Result;
    BYTE               m_btErrorCode;
    BYTE               m_btSafe_Type;
// 	BYTE               m_btSafe_Count;
// 	BYTE               m_btUnSafe_LimitNo;
    BYTE               m_btSafetyUD_DataType;    //170122
    WORD               m_btSafetyUD_MetaCode;    //170122
    BYTE               m_btSubCode;               //
    BYTE               m_btLost_Unit;              //180305
    BYTE               m_bUnsafeType;
    unsigned long int  m_uw32Safe_Code;           // 12字节
    //Un_UnsafeValue     m_Value;                   // 24字节 //  结构共计:36字节
    St_UnsafeValue     m_Value;                   // 48字节 //  结构共计:60字节
} ST_EVENT_SCHEDULE_RUNNING;

typedef struct
{
    WORD               m_wLimit_Index;
    BYTE               m_btEquation_Index;
    BYTE               m_btIfReached;
    WORD               m_wNextStep_Index;
    BYTE               m_btNone_12[2];            // 预留reserved
    float              f_LimitReserve[3];         // 结构共计:20字节
} ST_EVENT_SCHEDULE_LIMITREACH;

typedef struct
{
    BYTE               m_btStopStatus;
    DWORD              m_dwNone_A1;               // 预留reserved // 结构共计:5字节
} ST_STATUS_CHANNEL_STOP;

typedef struct
{
    WORD               m_wVariable_Count;
    BYTE               m_btNone_12[2];            // 预留reserved
    ST_SCHEDULE_VARIABLE    m_st_Variable[MAX_SEND_METAVARIABLE];// 结构共计:4+8*30=244字节
} ST_METAVARIABLE_SYNCH;

typedef struct
{
    unsigned long      Second;                    // 结构共计:12字节
    unsigned short     Us100;
    unsigned char      ucReserved[6];             // 本为6字节Keil用了16字节对齐，只能补足6字节。在同一个结构中与上一个4字节配对。
} MsTime_Pack;


typedef struct
{
    float              Inoffset;                  // 温度补偿偏置
    float              V_filter;                  // 参考电压数字滤波系数
    float              T_RangeH;                  // 温度范围上限（预留）
    float              T_RangeL;                  // 温度范围下限（预留）
    float              V_RangeH;                  // 参考电压测量范围上限（预留）
    float              V_RangeL;                  // 参考电压测量范围下限（预留）
    float              ADCGain;                   // ADC Gain
    float              ADCoffset;                 // ADC Offset
    float              InExtra;                   // 扩展信息（预留）//热电偶温度计的配置信息结构 共计36字节
} St_AUXTHERMOCOUPLE_RANGE;

typedef struct
{
    BYTE               TempChamberType;          // 温控箱类型
    BYTE               COM_BaudRate;             // 串口波特率
    BYTE               COM_DataBit;              // 串口数据位
    BYTE               COM_Parity;               // 串口校验
    BYTE               COM_StopBit;              // 串口停止位   //温控箱串口参数 共计4字节
} St_AUXTEMPCHAMBER;

typedef union
{
    St_AUXTHERMOCOUPLE_RANGE    m_AuxThermoCouple_Range;      // 热电偶参数结构    //辅助温度   共计36字节
    St_AUXTEMPCHAMBER    m_AuxTempChamber;                    // 温控箱参数
} Un_AUXTEMP_PARAMETER;

typedef struct
{
    unsigned char      TempChType;                // 热电偶/热电阻/电阻温度计/半导体温度计
    BYTE               m_btAuxSubType;            // 辅助通道二级分类
    BYTE               SensorType;                // 热电偶温度传感器类型
    BYTE               ControlModel;              // 控制模式
    Un_AUXTEMP_PARAMETER    AuxTemp_Parameter;    // 热电偶温度参数/温控箱参数 共计40字节
} St_AUXTHERMOCOUPLE;


typedef struct
{
    unsigned char      TempChType;                // 热电偶/热电阻/电阻温度计/半导体温度计
    BYTE               m_btAuxSubType;            // 辅助通道二级分类
    unsigned short     Function;                  // 电阻温度计转换公式类型
    float              Inoffset;                  // 温度补偿偏置
    float              R0;                        // 电桥电阻R0值
    float              T_RangeH;                  // 温度范围上限（预留）
    float              T_RangeL;                  // 温度范围下限（预留）
    float              ADCGain;                   // ADC Gain
    float              ADCoffset;                 // ADC Offset
    float              InExtra;                   // 扩展信息（预留）//电阻温度计的配置信息结构 共计32字节
} St_RESISTANCETHERMOMETER;

typedef struct
{
    float              RangeH;                    // 二次电压高量程    //二次电压的配置信息结构 共计20字节
    float              RangeL;                    // 二次电压低量程
    float              InGain;                    // ADC Gain
    float              Inoffset;                  // ADC_Offset
    float              InExtra;                   // 扩展信息（预留）
} St_AUX2NDV;

typedef struct
{
    float              SetPoint;                  // 设置值    //DI的配置信息结构 共计4字节
} St_AUXDI;

typedef struct
{
    float              SetPoint;                  // 设置值    //D0的配置信息结构 共计4字节
} St_AUXDO;

typedef struct
{
    unsigned char m_BoardPN[10];             // 板P/N号 Ver:    共计20字节
    BYTE          m_btNone_11[2];            // 预留reserved
    unsigned char m_BID;                     // BID拨码
    unsigned char m_AuxType;                 // 辅助版类型  板类型枚举定义 enum  TE_AUX_CHAN_TYPE
    unsigned char m_btConsistent_Order;      // 面板通道号是否与电气特性排序的通道序号一致
    unsigned char m_btSlave_Goes_First;      // 有无副板时，面板通道号是主板在前还是副板在前
    unsigned char m_btWith_Slave_Board;      // 有无副板
    BYTE          m_btNone_12;               // 预留reserved
    unsigned short int m_u16Board_RSN;       // 注册序号RSN
} St_AuxBoardInfoType;

/*typedef struct
{
unsigned long      m_u32ArbinID;              // ArbinID       //板类型配置信息结构   共计44字节
BYTE               m_btCtrl_Type;             // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息           （预留开发 3：单独下载Map表 4：单独下载校正值）
BYTE               m_btAuxSubType;            // 辅助通道二级分类
BYTE               m_btSensor_Type;           // 传感器类型枚举定义enumTE_AUX_CHAN_T_SENSOR_TYPE
BYTE               m_btNone_11;               // 预留reserved
St_AuxBoardInfoType  m_BoardType;             // Board类型
BYTE               m_u8ChCountPerType[MAXAUXTYPE];// 每一种类型辅助通道数量;            // 用于上传写板小软件工具的每一种类型辅助通道数量
BYTE               m_btAuxOtherType;             // 辅助通道其他类型
BYTE               m_btNone_13[3];               // 预留reserved
float              m_fReserved;                  // 预留reserved
} St_AuxBoardGlobalCFG;*/

typedef struct
{
    BYTE               m_IVChIdx;                 // 这个命令告知 IV 通道其使用的各种辅助通道的范围。比如说一个 IV 通道 Map 了8个二次电压通道，但是只有5个 Cell，那么要设定二次电压使用范围为 5。
    unsigned short     m_AuxScopes[MAXAUXTYPE];
} St_SETAUXSCOPE;

typedef struct
{
    BYTE               m_DamagedMap[MAX_CH_IN_AUX_UNIT/8];// 16个字节，共128位，每位表示一个辅助通道，是否工作。0为正常，1为损坏。
} St_MARK_DAMAGED_CH;

typedef struct
{
    BYTE               m_btChannelType;                     // 通道类型。
    BYTE               m_ucSensor;                          // 温度中的传感器J,K,T,
    WORD               m_u16ChannelCount;                   // 通道数目。
    BYTE               m_ucChannelIdx[2];                   // 2个byte=16个bit，使用bit位来标识最多16个通道。
    BYTE               m_ucLogType;                         // 显示类型 display = 0;idle = 1;log = 2;
    BYTE               m_btNone_11;                         // 预留reserved
    float              m_fValue[MAXCHANNELNO];              // 8+4*16=72
} St_BUILDINAUX_VALUE;

//typedef union
// {
//     St_AUXTHERMOCOUPLE    m_AuxThermoCouple;      // 热电偶结构    //辅助温度   共计40字节
//     St_RESISTANCETHERMOMETER    m_ResistanceThermometer;// 电阻温度计结构    //其他类型逐步增补 把二次电压加进来
// } Un_AUXTEMP;

typedef union
{
    unsigned char         TempChType;                // 热电偶/热电阻/电阻温度计/半导体温度计
    St_AUXTHERMOCOUPLE    m_AuxThermoCouple;      // 热电偶结构    //辅助温度   共计40字节
    St_RESISTANCETHERMOMETER    m_ResistanceThermometer;// 电阻温度计结构    //其他类型逐步增补 把二次电压加进来
} Un_AUXTEMP;

typedef union
{
    Un_AUXTEMP         m_AuxTemp;                 // 辅助温度
    St_AUX2NDV         m_Aux2ndV;                 // 二次电压
    St_AUXDI           m_AuxDI;                   // 数字输入
    //St_AUXPH         m_AuxPH;                   // PH
    //St_AUXPRESSURE     m_Pressure;              // 压力
    St_AUXDO           m_AuxDO;                   // 数字输出
    //St_AUXAO         m_AuxAO;                   // 模拟输出
    //St_AUXFLOW       m_AuxFlow;                 // 流量
    //St_AUXSMB        m_AuxSmb;                  // SmartBattery
    //St_AUXI2C        m_AuxI2C;                  // I2C    //辅助通道配置信息定义 共计40字节
} Un_AUXCHINFO;

typedef struct
{
    float   m_PHY_RangeL;
    float   m_PHY_RangeH;
    float   m_ADC_Gain;
    float   m_ADC_Offset;
    float   m_PHY_Gain;
    float   m_PHY_Offset;
    float   m_Max_Output;
    float   m_Min_Output;

} ST_AUX_SENSOR_CORE_INFO;    //4*6=24字节

typedef struct
{
    unsigned char      m_BID;            //BID 拨码
    unsigned char  		m_BoardType;      //辅助板型　（注意不是辅助类型）   可以通过辅助板类型判断需要加载哪种任务
    unsigned char      m_AuxFuncType;   //0x01:ADC功能;0x02：DI功能；0x04:DO功能;0x08:CANBMS功能;0x10:UART功能;0x20:I2C功能
    unsigned char  		m_Total_Channels;   //实际有效的通道总数。这个用来计算data  group  index
    unsigned char      m_Max_Channels;    //该辅助板所支持的最大通道数（电气特性决定）
    unsigned char      m_Total_ADC;       //ADC 总数
    unsigned char 			m_Total_CANBMS;  //CANBMS通道总数， 1 or 0
    unsigned char 			m_Total_UART;    //UART 总数
    unsigned char 			m_Total_DI;       // DI 总数
    unsigned char 			m_Total_DO;      //DO 总数
    unsigned char      m_SlaveBoardType;     //BUILTIN 副板类型， 0x01: VL_16V  ;0x02: VH_8V;  0x03:ＴＣ＿１６Ｔ；
    unsigned char      m_Total_Safety;
    unsigned char      m_Total_DAC;     //DAC 通道总数
}
ST_ABI_GLOBLE;                        //13字节




typedef struct
{
    unsigned char  m_ConsistentOrder;    //该板电气通道号是否从左到右 1—>8 这么排的
    unsigned char   m_SlaveGoesFirst;  // 该板的取向是否在副板再前的取向
    unsigned char  m_WithSlave;      // 该板是否带有副板
} ST_ABI_ASSEMBLE;
typedef struct
{
    unsigned long        m_u32ArbinID;    // ArbinID       //板类型配置信息结构   共计44字节
    BYTE                 m_btCtrl_Type;   // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息           （预留开发 3：单独下载Map表 4：单独下载校正值）)
    ST_ABI_ASSEMBLE      m_assemble;       //板子在机箱中的组合信息
    BYTE                 m_valid;          //该板是否有效标记
    ST_ABI_GLOBLE        m_abi_globe;      //全局信息
    // BYTE                 m_reserved1;
    unsigned short int   m_u16Board_RSN;   // 注册序号RSN
    BYTE                 m_btNone_13;     // 对齐补零
    BYTE                 m_u8ChCountPerType[MAXAUXTYPE];// 每一种类型辅助通道数量;            // 用于上传写板小软件工具的每一种类型辅助通道数量
} St_AuxBoardGlobalCFG;                   //25+11=36个字节


// typedef struct
// {
//   unsigned char  m_Populated;    //该通道是否有效
//   unsigned char  m_AuxType;   //辅助类型， 比如是温度，电压，还是压力，pH,
//   unsigned short m_AuxSubType;  //辅助的次级类型，也就是传感器类型，比如是T，K，J 热电偶， Pt100热电阻。
// 																//d定义 一个特殊类型为 UART型，比如1000
//   ST_AUX_SENSOR_CORE_INFO  m_AuxSensorCoreInfo;  //用于转换数据的传感器相关的信息
//   unsigned char m_avg_num;  //平滑滤波的数据个数
// 	unsigned char m_Controllable ;   //是只读的传感器，还是可以控制的执行器，比如温控箱，流量控制器等。
// 	unsigned char m_UartPort;  //串口号  UART端口号
// }ST_ABI_CHN;

typedef  struct
{
    unsigned char m_UART_Port ;   //MCU Uart port number， 0 - 3
    unsigned char m_Work_Mode ;   //0: R232 1: RS485
    unsigned char m_Device_Add;   //如果是485，device 地址， 如果是232， 一律为0
    unsigned char m_Device_Type;  //设备类型, 详见 TE_COM_CHAMBERTYPE
    unsigned char m_Baud_Rate ;  //波特率: 1800, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
    unsigned char m_COM_DataBit;  // 串口数据位: 5, 6, 7, 8
    unsigned char m_COM_Parity;   // 串口校验: EVEN, ODD, NONE
    unsigned char m_COM_StopBit;  // 串口停止位: ONE, TWO
    float         m_Set_Value;		// 设定温度/湿度
    unsigned char m_None[20];  ///

} ST_UART_CHN_CORE_INFO ;

typedef  struct
{
    unsigned char m_Aux_Type ;     //辅助通道类型，目前仅冷板有I2C辅助通道，温度类型为1
    unsigned char m_I2C_Port ;     //MCU I2C port number， 0 - 2 ,目前只有冷板支持，为I2C0
    unsigned char m_Device_Add ;   //I2C sub address
    unsigned char m_Reserve ;
    float				  m_Raw_Data;      //Raw Data
    float         m_Physical_Value;//Physical Value
    float         m_Read_Value;    // Read Value
    unsigned char m_None[16];      //共32字节

} ST_I2C_CHN_CORE_INFO ;  //add by zc 04.17.2018
typedef union
{
    ST_AUX_SENSOR_CORE_INFO  m_AuxSensorCoreInfo;
    ST_UART_CHN_CORE_INFO    m_UartChnCoreInfo;
    ST_I2C_CHN_CORE_INFO     m_I2CChnCoreInfo;    //add by zc 04.17.2018
} UN_CHN_CORE_INFO;    //32字节


typedef struct
{
    unsigned char  m_Populated;    //该通道是否有效
    unsigned char  m_AuxType;   //辅助类型， 比如是温度，电压，还是压力，pH,
    unsigned short m_AuxSubType;  //辅助的次级类型，也就是传感器类型，比如是T，K，J 热电偶， Pt100热电阻。
    //d定义 一个特殊类型为 UART型，比如1000
    UN_CHN_CORE_INFO  m_ChnCoreInfo;  //用于转换数据的传感器相关信息。 也可放串口的信息
    unsigned char m_avg_num;  //平滑滤波的数据个数
    unsigned char m_Controllable ;   //是只读的传感器，还是可以控制的执行器，比如温控箱，流量控制器等。
    //unsigned char m_UartPort;  //串口号  UART端口号
} ST_ABI_CHN;




typedef struct
{
    BYTE                     m_valid;                //该通道是否有效
    BYTE                     m_ChannelNumber;        //通道编号
    BYTE                     m_AuxType;              // 通道辅助类型
    BYTE                     m_None;
    unsigned short           m_AuxSubType;           // 辅助的次级类型
    BYTE                     m_AVG_num;              //平滑滤波的数据个数
    BYTE                     m_Controllable;         //是只读的传感器，还是可以控制的执行器，比如温控箱，流量控制
    //	  ST_AUX_SENSOR_CORE_INFO  m_AuxSensorCoreInfo;    //用于转换数据的传感器相关的信息  //24字节
    UN_CHN_CORE_INFO  m_ChnCoreInfo;  //用于转换数据的传感器相关信息。 也可放串口的信息  //24字节


} St_AUXChannelCFG;                                  //该结构体共40个字节

typedef struct
{
    unsigned short m_Index_InIVCh;       // 该辅助类型的通道在该IV通道里的索引序号
    unsigned char m_Index_InAuxUnit;    // 该辅助类型在其辅助unit内通道号
    unsigned char m_valid:1;  //用于标记此辅助通道是否真的被用过
    unsigned char m_used:1;  //用于标记此辅助通道是否真的被用过
    unsigned char m_Index_InLongSnakeBuf; //该辅助通道的物理地址，BID,CID在长蛇阵中的位置
    unsigned char m_data_group_idx;    //直接将data_group_idx 和 电气通道号查好备用
    unsigned char m_chn;   //电气通道号

} St_MAP_AuxCh2IVCh;  // 共计7个字节

typedef struct
{
    St_MAP_AuxCh2IVCh   m_Map;       // 辅助通道对应号

    unsigned char m_Aux_UnitID;      // 辅助所在地的unit号
    unsigned char m_CAN_Port;        // IVMCU内CAN网,还是IVMCU间CAN网。
    unsigned char m_AuxType;         // 辅助类型  (冗余设计)
    unsigned char m_IVCh_Index;      // 在该unit内的IV通道序号  (冗余设计)
} St_OneAuxChInfo;  // 共计11个字节

//typedef struct
//{
//    unsigned char m_IVCh_Index;     // 在该unit内的IV通道序号
//    St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAX_MAP_COUNT_PER_UNIT];  // 一个IV通道分到的辅助通道  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
//    unsigned short m_Total_In_OneIVCh[MAXAUXTYPE];  // 同种辅助类型下通道总数量  	//--new add---新增加
//	unsigned short m_aux_type_ptr[MAXAUXTYPE];
//} St_PC2IVUnit4AuxMap;  //  From PC to IVUnit for (about) AuxMap

typedef struct
{
    unsigned char m_Index_InUnit;   // 该辅助类型在其unit内通道号

    unsigned char m_Index_InADC; // 物理通道号,也就是在片ADC的通道号
    unsigned char  m_AuxType ;  // 记住自己的辅助类型，也算冗余设计
} St_BuildInAuxCh;


typedef struct    //来自IV自建的内部辅助通道信息结构   <<辅助通道的硬件配置和通道分配>> 表4 物理映射小表
{
    St_BuildInAuxCh  m_BuildInAuxCh[MAX_BUILDIN_AUXTYPE][MAX_BUILDIN_AUXCH];  // 内部辅助通道类型

    unsigned char m_BuildInAuxTotalCh[MAX_BUILDIN_AUXTYPE];  // 同种辅助类型下通道总数量  	//--new add---新增加

} St_BuildInAuxInfo;

typedef struct
{
    unsigned char m_IV_UnitID;          // IVMCU的全局unit ID
    unsigned char m_IVCh_Index;         // 在该unit内的IV通道序号
    unsigned char m_Index_InIVCh;       // 该辅助类型的通道在该IV通道里的索引序号
    unsigned char m_Index_InAuxUnit;    // 该辅助类型在其辅助unit内通道号
} St_MAP_PCDOWN_AuxCh2IVCh;

typedef struct
{
		unsigned char m_valid:1;           			//用于标记此辅助通道是否真的被用过
		unsigned char m_Index_InIVCh_HighBit:7;	//当m_Index_InIVCh超出255时需要置位，否则置为0
    unsigned char m_IVUnit;         				//IVMCU的全局unit ID
    unsigned char m_IVCh;         					// 在该unit内的IV通道序号
    unsigned char m_AuxType;
    unsigned char m_Index_InIVCh;       		// 该辅助类型的通道在该IV通道里的索引序号
    unsigned char m_Index_InAuxUnit;    		// 该辅助类型在其辅助unit内通道号
} St_DOWNLOAD_MAP_FROM_PC;

typedef struct
{
    float              m_fCurrent;                          // 每点电流。这部分重复PointCount次每点相对时间单位为微秒，相对的是本脉冲第一点而不是本包第一点。
    float              m_fVoltage;                          // 每点电压。
    DWORD              m_dwPointTime;                       // 每点相对时间。
} St_PULSE_VCT;


// 信道命令头宏定义
#define CMD_SET_SYSTEMTIME                            (0X5912A100)  // 系统时间对时，更新系统时间
#define CMD_SET_SYSTEMTIME_FEEDBACK                   (0XA912A100)  // 系统时间对时，更新系统时间,回馈
#define CMD_SET_CHANNEL_BOARD                         (0X5981A100)  // 设置通道板信息
#define CMD_SET_CHANNEL_BOARD_FEEDBACK                (0XA981A100)  // 设置通道板信息,MCU回馈
#define CMD_SET_CHANNEL_BOARD_CALIB                   (0X5981C600)  // 设置自动校正板通道板信息
#define CMD_SET_CHANNEL_BOARD_CALIB_FEEDBACK          (0XA981C600)  // 设置自动校正板通道板信息,MCU回馈
#define CMD_SET_BUILTIN_CHANNEL_BOARD                 (0X5981A110)  // 设置BUILTIN 板信息
#define CMD_SET_BUILTIN_CHANNEL_BOARD_FEEDBACK        (0XA981A110)  // 设置BUILTIN 板信息 ，MCU 回馈
#define CMD_SET_CHANNEL_MAP                           (0X5981A300)  // 设置通道MAP信息
#define CMD_SET_CHANNEL_MAP_FEEDBACK                  (0XA981A300)  // 设置通道MAP信息,MCU回馈
#define CMD_SET_CHANNEL_CALIBRATE                     (0X5981B100)  // 设置通道校正参数信息
#define CMD_SET_CHANNEL_CALIBRATE_FEEDBACK            (0XA981B100)  // 设置通道校正参数信息,MCU回馈
#define CMD_SET_M0_CHANNEL_CALIBRATE                  (0X5981B101)  // 设置M0通道校正参数 信息 
#define CMD_SET_M0_CHANNEL_CALIBRATE_FEEDBACK         (0xA981B101)  //MCU 回馈
#define CMD_SET_CHANNEL_PID                           (0X5981A200)  // 设置通道PID参数信息
#define CMD_SET_CHANNEL_PID_FEEDBACK                  (0XA981A200)  // 设置通道PID参数信息,MCU回馈

#define  CMD_SET_CAN_CONFIG                           (0X5981A400) //设置CAN BMS CONFIG 表，MCU回馈
#define  CMD_SET_CAN_CONFIG_FEEDBACK                  (0XA981A400)
#define  CMD_SET_CAN_MAP                              (0X5981A500)
#define  CMD_SET_CAN_MAP_FEEDBACK                     (0XA981A500)

#define  CMD_SET_CAN_MESSAGE                          (0X5981A600)
#define  CMD_SET_CAN_MESSAGE_FEEDBACK                 (0XA981A600)
#define  CMD_ASSIGN_CAN_CONFIG                        (0X5981A700)
#define  CMD_ASSIGN_CAN_CONFIG_FEEDBACK               (0XA981A700)
#define  CMD_SET_MULTI_CAN_MESSAGE                    (0X5981A900)  //多CAN MSG自动发送 //170828 zyx
#define  CMD_SET_MULTI_CAN_MESSAGE_FEEDBACK           (0XA981A900)
#define  CMD_SET_SMB_CONFIG                           (0X5981AA00) //设置CAN BMS CONFIG 表，MCU回馈
#define  CMD_SET_SMB_CONFIG_FEEDBACK                  (0XA981AA00)
#define  CMD_ASSIGN_SMB_CONFIG                        (0X5981AB00)
#define  CMD_ASSIGN_SMB_CONFIG_FEEDBACK               (0XA981AB00)

#define CMD_GET_CHANNEL_BOARD                         (0X5982A100)  // 获取设置通道板信息
#define CMD_GET_CHANNEL_BOARD_FEEDBACK                (0XA982A100)  // 获取设置通道板信息,MCU回馈
#define CMD_GET_BUILTIN_CHANNEL_BOARD                 (0X5982A110)  // 获取设置通道板信息
#define CMD_GET_BUILTIN_CHANNEL_BOARD_FEEDBACK        (0XA982A110)  // 获取设置通道板信息,MCU回馈
#define CMD_INDEPENDENT_READ_BOARD_DETAIL             (0X5982A300)  // 获取设置通道板信息（包括板型和ver）
#define CMD_INDEPENDENT_READ_BOARD_DETAIL_FEEDBACK    (0XA982A300)  // 获取设置通道板信息,MCU回馈（包括板型和ver）
#define CMD_GET_CHANNEL_BOARD_CALIB                   (0X5982C600)  // 获取自动校准板设置通道板信息
#define CMD_GET_CHANNEL_BOARD_CALIB_FEEDBACK          (0XA982C600)  // 获取自动校准板设置通道板信息,MCU回馈
#define CMD_STEPLIMIT_UPDATE                          (0X59A18100)  // 步限制状态更新
#define CMD_STEPLIMIT_UPDATE_FEEDBACK                 (0XA9A18100)  // 步限制状态更新,MCU回馈
#define CMD_RSN_ERROR_ACK_EVENT                       (0X59A18200)  // 辅助通道RSN错误信息答复
#define CMD_RSN_ERROR_ACK_EVENT_FEEDBACK              (0XA9A18200)  // 辅助通道RSN错误信息答复,MCU回馈
#define CMD_SET_M0_ACTIONSWITCH                       (0X59A18201)  //M0 开通道行为开关
#define CMD_SET_M0_ACTIONSWITCH_FEEDBACK              (0XA9A18201)
#define CMD_INDEPENDENT_ACTIONSWITCH                  (0X59EDA100)  // 独立控制行为开关
#define CMD_INDEPENDENT_ACTIONSWITCH_FEEDBACK         (0XA9EDA100)  // 独立控制行为开关,MCU回馈
#define CMD_INDEPENDENT_ACTION                        (0X59EDA200)  // 独立控制行为
#define CMD_INDEPENDENT_ACTION_FEEDBACK               (0XA9EDA200)  // 独立控制行为,MCU回馈
#define CMD_INDEPENDENT_EMERGENCYSTOP                 (0X59EDA300)  // 独立控制紧急停止
#define CMD_INDEPENDENT_EMERGENCYSTOP_FEEDBACK        (0XA9EDA300)  // 独立控制紧急停止,MCU回馈
#define CMD_INDEPENDENT_READDATAORSTATE               (0X59EDA400)  // 独立控制行为读取数据或者状态
#define CMD_INDEPENDENT_READDATAORSTATE_FEEDBACK      (0XA9EDA400)  // 独立控制行为读取数据或者状态,MCU回馈
#define CMD_M0_READDATAORSTATE                        (0X59EDA401)   // M0 校准行为， 读取数据
#define CMD_M0_READDATAORSTATE_FEEDBACK               (0XA9EDA401)   //MCU 反馈
#define CMD_Read_M0_CalibPara                         (0X59EDA402)   //M0 读取校准参数
#define CMD_Read_M0_CalibPara_FEEDBACK                (0XA9EDA402)   //M0 读取校准参数 MCU 反馈
//#define CMD_INDEPENDENT_POWERON                       (0X59EDA500)  // 独立控制电源打开
//#define CMD_INDEPENDENT_POWERON_FEEDBACK              (0XA9EDA500)  // 独立控制电源打开,MCU回馈
// #define CMD_INDEPENDENT_RESETCAN                      (0X59EDA600)  // 独立控制复位CAN
// #define CMD_INDEPENDENT_RESETCAN_FEEDBACK             (0XA9EDA600)  // 独立控制复位CAN,MCU回馈
#define CMD_INDEPENDENT_READ_CFG                      (0X59EDA700)  // 独立控制  读配置
#define CMD_INDEPENDENT_READ_CFG_FEEDBACK             (0XA9EDA700)  // 独立控制  读配置,MCU回馈
#define CMD_INDEPENDENT_ACTION_AUTOCALIBRATE          (0X59EDE200)  // 独立控制行为,自动校准板
#define CMD_INDEPENDENT_ACTION_AUTOCALIBRATE_FEEDBACK (0XA9EDE200)  // 独立控制行为,自动校准板,MCU回馈
#define CMD_SCHEDULE_SET_SETTING                      (0X69A18100)  // 设置Schedule Setting信息
#define CMD_SCHEDULE_SET_SETTING_FEEDBACK             (0XB9A18100)  // 设置Schedule Setting信息,MCU回馈
#define CMD_SYNCHRO_METAVARIABLE                      (0X69A18200)  // 同步MetaVariable信息
#define CMD_SYNCHRO_METAVARIABLE_FEEDBACK             (0XB9A18200)  // 同步MetaVariable信息,MCU回馈
#define CMD_SCHEDULE_GET_STATUS                       (0X69A23100)  // 获取Schedule 状态信息
#define CMD_SCHEDULE_GET_STATUS_FEEDBACK              (0XB9A23100)  // 获取Schedule 状态信息,MCU回馈
#define CMD_SET_META_VARIABLE						  						(0X69A46300)
#define CMD_SET_META_VARIABLE_FEEDBACK				  			(0XB9A46300)
#define CMD_SCHEDULE_ACK_EVENT                        (0X69A23200)  // 确认Schedule 事件编号信息
#define CMD_SCHEDULE_DOWNLOAD                         (0X69A36100)  // Schedule全部步内容下载
#define CMD_SCHEDULE_DOWNLOAD_FEEDBACK                (0XB9A36100)  // Schedule全部步内容下载,MCU回馈
#define CMD_STEP_DOWNLOAD                             (0X69A36200)  // Step步内容下载
#define CMD_STEP_DOWNLOAD_FEEDBACK                    (0XB9A36200)  // Step步内容下载,MCU回馈
#define CMD_SIMULATION_DOWNLOAD                       (0X69A36900)  // Simulation内容下载         //12.22.2013ADD
#define CMD_SIMULATION_DOWNLOAD_FEEDBACK              (0XB9A36900)  // Simulation内容下载,MCU回馈 //12.22.2013ADD0
#define CMD_BATTERY_SIMULATION_DOWNLOAD               (0X69A36300)  // Battery Simulation内容下载     //171114 zyx 
#define CMD_BATTERY_SIMULATION_DOWNLOAD_FEEDBACK      (0XB9A36300)  // Battery Simulation内容下载,MCU回馈 //171114 zyx
#define CMD_PULSE_DOWNLOAD                            (0X69A36400)  // Pulse内容下载
#define CMD_PULSE_DOWNLOAD_FEEDBACK                   (0XB9A36400)  // Pulse内容下载,MCU回馈
#define CMD_SCHEDULE_ONLINEUPDATE                     (0X69A39100)  // Schedule在线更新命令  // 09.11.2017
#define CMD_SCHEDULE_ONLINEUPDATE_FEEDBACK            (0XB9A39100)  // Schedule在线更新命令,MCU回馈
#define CMD_SCHEDULE_START                            (0X69A45100)  // Schedule开始命令
#define CMD_SCHEDULE_START_FEEDBACK                   (0XB9A45100)  // Schedule开始命令,MCU回馈
#define CMD_SCHEDULE_STOP                             (0X69A45200)  // Schedule结束命令
#define CMD_SCHEDULE_STOP_FEEDBACK                    (0XB9A45200)  // Schedule结束命令,MCU回馈
#define CMD_SCHEDULE_CONTINUE                         (0X69A45201)  // Schedule继续命令
#define CMD_SCHEDULE_CONTINUE_FEEDBACK                (0XB9A45201)  // Schedule继续命令,MCU回馈
#define CMD_SCHEDULE_ACIM_ON                       		(0X69A45202)  // Schedule ACIM ON控制命令
#define CMD_SCHEDULE_ACIM_ON_FEEDBACK              		(0XB9A45202)  // Schedule ACIM ON控制命令,MCU回馈
#define CMD_SCHEDULE_ACIM_OFF                      		(0X69A45203)  // Schedule ACIM OFF控制命令
#define CMD_SCHEDULE_ACIM_OFF_FEEDBACK            		(0XB9A45203)  // Schedule ACIM OFF控制命令,MCU回馈
#define CMD_SCHEDULE_JUMP                             (0X69A45300)  // Schedule跳步命令
#define CMD_SCHEDULE_JUMP_FEEDBACK                    (0XB9A45300)  // Schedule跳步命令,MCU回馈
#define CMD_SCHEDULE_ASSIGN                           (0X69A45400)  // Schedule指派命令
#define CMD_SCHEDULE_ASSIGN_FEEDBACK                  (0XB9A45400)  // Schedule指派命令,MCU回馈
#define CMD_SCHEDULE_PAUSE                            (0X69A45500)  // Schedule暂停命令
#define CMD_SCHEDULE_PAUSE_FEEDBACK                   (0XB9A45500)  // Schedule暂停命令,MCU回馈
#define CMD_SCHEDULE_RESUME                           (0X69A45600)  // Schedule恢复命令
#define CMD_SCHEDULE_RESUME_FEEDBACK                  (0XB9A45600)  // Schedule恢复命令,MCU回馈
#define CMD_SIMULATION_ASSIGN                         (0X69A45900)  // Simulation指派命令         //12.22.2013ADD
#define CMD_SIMULATION_ASSIGN_FEEDBACK                (0XB9A45900)  // Simulation指派命令,MCU回馈 //12.22.2013ADD
#define CMD_MTCI_SET_GROUP_SETTING                    (0X69A45901)  ///共享温控箱/1910101 zyx
#define CMD_MTCI_SET_GROUP_SETTING_FEEDBACK           (0XB9A45901)
#define CMD_SCHEDULE_REPORT_EVENT                     (0XE931A100)  // Schedule状态上行    MP_REPORT_EVENT_SDL
#define CMD_SCHEDULE_REPORT_METAVARIABLESYNCH         (0XE931A200)  // Schedule同步Metavariable上行
#define CMD_SCHEDULE_REPORT_LOGDATA                   (0XD9418100)  // Schedule数据上行
#define CMD_SCHEDULE_REPORT_LOGDATA_TC_Counter        (0XD9418400)  // Access log data,with tc_counter
#define CMD_SCHEDULE_REPORT_LOGDATA_CYCLE             (0XD9418500)	// Schedule Cycle数据上行 //09.19.2017
#define CMD_SCHEDULE_REPORT_SDL                       (0XD9418200)  // SDL数据上行
#define CMD_SCHEDULE_REPORT_GROUPLOGDATA			(0XD9418600)
#define CMD_SCHEDULE_REPORT_PULSE_LOGDATA             (0XD9418300)  // PULSE数据上行
#define CMD_REPORT_REQUEST_SYSTEMTIME                 (0XF612A100)  // 系统时间对时请求
#define CMD_REPORT_WRITE_FRAM                         (0XF612A200)  // FRAM写事件上传
#define CMD_REPORT_GET_CHANNEL_BOARD                  (0XF612A300)  // 获取设置通道板信息上行
#define CMD_REPORT_TOPC_ERROR                         (0XF612A400)  // 错误信息上行
#define CMD_REPORT_GET_SCHSTATUS                      (0XF612A500)  // 获取Schedule 状态信息上行
#define CMD_REPORT_CANBMS_DATA                        (0XF612A600)  // 上传CAN BMS DATA
#define CMD_REPORT_SMB_DATA                        		(0XF612A900)  // 上传SMB DATA
#define CMD_REPORT_SMB_STRING													(0XF612AA00)  // 上传SMB STRING
#define CMD_REPORT_BUILDINAUX_LOGDATA                 (0XF612A700)  // 上传IV板内部辅助通道数据 如：温控箱温度值
#define CMD_REPORT_SIMULATION_REQUESTDATA             (0XF612A800)  // 上传仿真数据请求包报告
#define CMD_REPORT_REQUEST_NTPCALIB                   (0XF614A100)  // 请求NTP对时包
//#define CMD_SCHEDULE_ONLINEUPDATE                   (0X69A45700)  // Schedule在线更新命令
#define CMD_SET_VOLTAGE_RANGE                         (0X5912E100)  //下载电压量程
#define CMD_SENT_ACIMTEST_START                       (0X69A45204)  // 通知辅助AUX 开始ACIM 测试
#define CMD_SENT_ACIMTEST_START_FEEDBACK              (0XB9A45204)  // MCU回馈
#define CMD_REPORT_ACIM_DATA                          (0X69A45205)  // Aux 上传ACIM 测试结果
#define CMD_REPORT_ACIM_DATA_FEEDBACK                 (0XB9A45205)

#define CMD_SENT_ACIMTEST_STOP                        (0X69A45206)  // 通知辅助AUX STOP ACIM 测试
#define CMD_SENT_ACIMTEST_STOP_FEEDBACK               (0XB9A45206)  // MCU回馈

#define CMD_BATCH_GROUP	                              (0X69A88800)
#define CMD_BATCH_GROUP_FEEDBACK 	                  (0XB9A88800)
#define CMD_SET_IV_BOARDTYPE                          (0X5981A800)
#define CMD_SET_IV_BOARDTYPE_FEEDBACK                 (0XA981A800)
#define CMD_GET_IV_BOARDTYPE                          (0X5982A200)
#define CMD_GET_IV_BOARDTYPE_FEEDBACK                 (0XA982A200)
#define CMD_REPORT_GET_IV_BOARDTYPE                   (0XF612A800)
#define CMD_WRITE_SERIESBOOSTER_MODE                  (0XF612A801)
#define CMD_WRITE_SERIESBOOSTER_MODE_FEEDBACK         (0XF612A802)
#define CMD_SET_CANAUX_BROADCAST_CONFIG               (0X5983A800)
#define CMD_SET_CANAUX_BROADCAST_CONFIG_FEEDBACK      (0XB983A800)
#define CMD_REPORT_BASIC_TABLE						  (0XDA418100)	//03.15.2018
#define CMD_REPORT_BASIC_TABLE_4BYTES_CYCLE_IDX	      (0XDA418300)	//09.13.2021 // cycle Index up to 4 Bytes.
#define  CMD_SET_DSP_BASIC_PARA                       (0X5983A100)
#define  CMD_SET_DSP_BASIC_PARA_FEEDBACK              (0XA983A100)
#define  CMD_SET_DSP_ADJUST_PARA                       (0X5983A200)
#define  CMD_SET_DSP_ADJUST_PARA_FEEDBACK              (0XA983A200)

#define  CMD_READ_DSP_BASIC_PARA                       (0X5983A300)
#define  CMD_READ_DSP_BASIC_PARA_FEEDBACK              (0XA983A300)
#define  CMD_READ_DSP_ADJUST_PARA                      (0X5983A400)
#define  CMD_READ_DSP_ADJUST_PARA_FEEDBACK             (0XA983A400)

#define  CMD_SET_DSP_INDENTITY_PARA                       (0X5983A500)
#define  CMD_SET_DSP_INDENTITY_PARA_FEEDBACK              (0XA983A500)
#define  CMD_READ_DSP_INDENTITY_PARA                      (0X5983A600)
#define  CMD_READ_DSP_INDENTITY_PARA_FEEDBACK             (0XA983A600)

#define CMD_GET_DEBUG_DATA													(0X5983A700)
#define CMD_GET_DEBUG_DATA_FEEDBACK											(0XA983A700)

#define CMD_UPDATE_META_VARIABLE_ADVANCED			(0X69A46303)
#define CMD_UPDATE_META_VARIABLE_ADVANCED_FEEDBACK	(0XB9A46303)

#define CMD_GET_META_VARIABLES								(0X69A56303)	//03.15.2022
#define CMD_GET_META_VARIABLES_FEEDBACK						(0XB9A56303)	//03.15.2022

#define CMD_SET_INTERVALTIME_LOGDATA						(0X69A56304)	
#define CMD_GET_INTERVALTIME_LOGDATA_FEEDBACK				(0XB9A56304)	
//命令字结构体定义
typedef struct
{
    DWORD              	m_dwCmd; 		//4bytes
    DWORD              	m_dwCmd_Extend;	//4bytes
    unsigned int 				m_src_addr;
    unsigned int 				m_src_addr_len;
    unsigned char 			m_src_data_type;
    BYTE               	m_btNone_11;
    BYTE               	m_btCheckSum[2];
} MP_GET_DEBUG_DATA;

typedef struct
{
    DWORD              	m_dwCmd; 		//4bytes
    DWORD              	m_dwCmd_Extend;	//4bytes
    unsigned int 				m_src_addr;
    unsigned int 				m_src_addr_len;
    unsigned char 			m_src_data_type;
    unsigned char 			m_error_code;
    WORD              	m_wNone_21;                         // reserved
    BYTE                m_btPackData[1400];
    WORD              	m_wNone_22;                         // reserved
    BYTE               	m_btCheckSum[2];
} MP_GET_DEBUG_DATA_FEEDBACK;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    MsTime             Time;
    BYTE               m_btUpdate;
    BYTE               m_btNone_12[1];                      // 预留reserved
    WORD                m_u16Token;
    //DWORD              m_dwNone_41;                         // 预留reserved
	float              f_SetIntervalTimeLog;  // s
    BYTE               m_btCheckSum[2];                     // 结构共计:24字节
} MP_SET_SYSTEMTIME;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_uVotalgeRange[MAXCHANNELNO];
    WORD               m_uNone_11;                         // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:
} MP_SET_VOLTAGE_RANGE;


typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    DWORD              m_dwCopyCmd;                         // 原命令备份执行命令码字
    DWORD              m_dwCopyCmd_Extend;                  // 原命令备份扩展命令码字
    WORD               m_wResult;                           // 0:Failed1:Success
    WORD               m_dwNone_22;                         // 预留reserved
    MsTime             Time;                                // 6字节
    BYTE               m_btCheckSum[2];                     // 结构共计:48字节
} MP_SET_SYSTEMTIME_FEEDBACK;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_XXX
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
    BYTE               m_btCheckSum[2];                     // 结构共计:54+N字节
} MP_SINGLE_PACKDATA;

typedef struct
{
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    unsigned short     m_u16Reserved;
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    St_BoardGlobalCFG    m_Global;
    St_ChannelCFG      m_Channel[128];                      // 结构共计:59K字节
} MP_UnitConfigDownload;

typedef struct
{
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    unsigned char      m_ucCommonSource ;  //命令来源 :0xAA  listenfornet ,   0xBB  MistPro 7.0.//1807131
    unsigned char      m_ucReserved ;
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通
    St_AuxBoardGlobalCFG    m_Global;                       // 44+4*128字节
    St_AUXChannelCFG    m_Channel[MAX_CH_IN_AUX_UNIT];      // 44*128字节    // 结构共计: 20+（44+4*128）+ (44*128) = 6.208K字节
} MP_AuxUnitConfigDownload;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=MP_SET_CHANNEL_CALIBRATE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btControlType;                     // 0:Current1:Voltage2:Temperature/2ndV3:Other
    BYTE               m_btRange;
    BYTE               m_btAuxIdx;
    BYTE               m_btNone_11;                      // 预留reserved
    St_RangeCali       m_st_ADC5DAC[MAXCHANNELNO][2];
    BYTE               m_btCheckSum[2];                     // 结构共计:274字节
} MP_SET_CHANNEL_CALIBRATE;

typedef struct
{
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    unsigned short     m_u16Reserved;
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    St_IV_ChannelPID    m_stChannelPID[128];                // 结构共计:20+144*128=18452字节
} MP_UnitPIDDownload;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_GET_CHANNEL_BOARD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
#if  USE_AS_AUXMCU==0
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
#else
    BYTE               m_btChannelCount;                    //数据包里下载的通道数目
    BYTE               m_btAuxFuncType;                     //读取的辅助功能类型 0x01:ADC,0x02:DI;0x04:DO;0x08:CANBMS;0x10:UART; 0x20:I2C
#endif
    BYTE               m_btCtrl_Type;                       /* 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息           （预留开发 3：单独下载Map表 4：单独下载校正值） //09.15.2017 COMMENT OUT
                                                               upload辅助时, 作为辅助类型（TE_AUX_CHAN_TYPE） */
    BYTE               m_btBoradID;                         //读取的板地址 //09.15.2017 upload辅助时, 对于上位机没有用用处，用作命令来源，如果m_btBoardID为0XBB则来源Mitspro
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    BYTE               m_btCheckSum[2];                     // 结构共计:74字节
} MP_GET_CHANNEL_BOARD;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=MP_GET_AUX_CHANNEL_BOARDD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCtrl_Type;                       // 上位机操作方式 0：没特别意义可全操作  1：仅写特定类型板号和通道数量，其它信息作废   2：写特定类型板号的通道信息           （预留开发 3：单独下载Map表 4：单独下载校正值）
    BYTE               m_btReserved;
    BYTE               m_btCheckSum[2];                     // 结构共计:12字节
} MP_GET_AUX_CHANNEL_BOARD;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_ACTIONSWITCH
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btOnSwitch;                        // 0x01 : Set   0x00 : turn off
    BYTE               m_btNone_11[3];                      // 预留:reserved
    BYTE               m_btCheckSum[2];                     //结构共计:66字节
} MP_SETM0_CalibMode;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_ACTIONSWITCH
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_bCaliBoard;                        // 是否作为Cali板使用？
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btOnSwitch;
    BYTE               m_ucBoardOperateType;                // 校准模式:1普通校准2自动校准
    BYTE               m_ucBattery_Module;                  // 电池模式:1串入2旁路0notcare
    BYTE               m_ucCOM_Module;                      // 公共模式:1COM2V++0notcare
    BYTE               m_btNone_12;                         // 预留reserved
    BYTE              m_btCheckSum[2];                     // 结构共计:66字节
} MP_INDEPENDENT_ACTIONSWITCH;


typedef struct
{
    BYTE m_AuxType;
    BYTE m_MessageSource;
} INDEPENDENT_ACTION_AUX_INFO;

typedef struct
{
    BYTE               m_btNone_11;                      // 预留reserved
    BYTE               m_bFineTuning:1;//190917 hpq
    BYTE               m_bNone_12:7;
} INDEPENDENT_ACTION_IV_INFO;

typedef union
{
    INDEPENDENT_ACTION_AUX_INFO IA_AuxInfo;
    INDEPENDENT_ACTION_IV_INFO  IA_IvInfo;
} IA_INFO;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_ACTION
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    WORD               m_wStep_ID;
    WORD               m_w41;                               // 预留reserved
    ST_STEPCONTROL_DATA    m_st_CtrlData;
    IA_INFO            m_IAInfo;
    BYTE               m_btAutoCali;                        // 冗余manu=0默认,AutoCali=1
    BYTE               m_btIfResetStepTime;
    float              m_fPreSetStepTime;
    float              m_fVclampHigh;
    float              m_fVclampLow;
    BYTE               m_btCheckSum[2];                     // 结构共计:114字节
} MP_INDEPENDENT_ACTION;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=MP_INDEPENDENT_EMERGENCYSTOP
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE				m_bt_Ctrl_Stop;											// 紧急停止温控箱
    BYTE				m_btNone_11[3];											// 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:58字节
} MP_INDEPENDENT_EMERGENCYSTOP;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_READDATAORSTATE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btIsReadData;
    BYTE               m_btIsState;
    BYTE               m_btCheckSum[2];                     // 结构共计:62字节
} MP_INDEPENDENT_READDATAORSTATE;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_READDATAORSTATE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btIsReadData;
    BYTE               m_btIsState;
    ST_READDATAORSTATE_VARIABLE    m_st_Read_Value[MAXCHANNELNO];
    BYTE                m_btCheckSum[2];                    // 结构共计:18+12*16=210字节
} MP_INDEPENDENT_READDATAORSTATE_FEEDBACK;
typedef struct
{
    DWORD              m_dwCmd;                         //执行命令码字=CMD_Read_M0_CalibPara
    DWORD              m_dwCmd_Extend;                      //扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[4];                      // 预留reserved
    BYTE               m_btCheckSum[2];                     //?结构共计:62字节
} MP_M0_READCalibPara;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_Read_M0_CalibPara_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[4];                      // 预留reserved
    St_RangeCali       m_st_Read_Para[MAXCHANNELNO];
    BYTE                m_btCheckSum[2];                    // 结构共计:18+8*16=146字节
} MP_M0_READCalibPara_FEEDBACK;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_M0_READDATAORSTATE _FEEDBACK
    DWORD              m_dwCmd_Extend;                      //扩展命令码字
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    float             m_ucRead_Value[8];
    BYTE               m_btCheckSum[2];                    //结构共计:78字节
} MP_M0_READDATAORSTATE_FEEDBACK;
typedef struct
{
    BYTE length;
    BYTE Detail[MAX_DETAIL]; //zyx 1804201
} St_DETAIL;
typedef struct
{
    unsigned short m_u16BoardType; //板型号
    BYTE m_btBoardTypeVersion;     // 版本信息   0：VerA,  1:VerB , 2:VerC,  3:VerD
    BYTE            m_btIs32M_SDRAM;          //170925 zyx
    WORD            m_wOutOf_FRAM_error; //检测FRAM是否越界 //1610142 zyx //170927 zyx
    BYTE            m_btIsMapIRange;          //170925 zyx
    BYTE            m_IRangeVir2PhyMap[NUM_OF_IV_RANGE_I];  //170925 zyx
    St_DETAIL       m_Detail;
} IV_BOARDInfo;
typedef struct
{
    BYTE				BoardCount;     //辅助板数目
    BYTE                m_btIs32M_SDRAM;          //170925 zyx
    ST_ABI_GLOBLE	    AuxBoard[MAX_BOARD_PER_AUXMCU];
    St_DETAIL           m_Detail;
} AUX_BOARDInfo;
typedef union
{
    IV_BOARDInfo   		IV_Info;
    AUX_BOARDInfo       AUX_Info;
    //Cali_version;
    char 				Reserved[256]; //保持该成员结构一直大于其他成员
} Un_BOARDInfo;

typedef struct
{
    BYTE               m_btYear;
    BYTE               m_btMonth;
    BYTE               m_btDay;
    BYTE               m_btBuildNumber:4;
    BYTE               m_btVersionType:4; //TY ZY RD //zyx 180305
    WORD               m_wCircuitType;
} UNIT_VER_INFO; //1610142 zyx

typedef struct
{
    BYTE               m_btUnitId;
    BYTE               m_btCmd;                             // =CMD_UNIT_INFO
    UNIT_VER_INFO			 m_unitVerInfo;
    BYTE               m_CheckSum[2];
} MP_UNIT_INFO_FEEDBACK;

typedef struct
{
    DWORD				m_dwCmd;
    DWORD				m_dwCmd_Extend;
    UNIT_VER_INFO       m_unitVerInfo;
    BYTE  				BoardType;
    BYTE 				Reserved[7];
    Un_BOARDInfo  		BoardInfo;
    BYTE				m_btCheckSum[2];
} MP_INDEPENDENT_READ_BOARD_VERSION,MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_READ_CFG
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:62字节
} MP_INDEPENDENT_READ_CFG;
typedef struct
{
    unsigned short     m_uBoardType;                       //通道板类型
    unsigned char      m_uCurrRngCount;                    //电流量程数目
    unsigned char      m_uVoltRngCount;                    //电压量程数目
    float              m_fCurrBound[NUM_OF_IV_RANGE_I];    //电流的单边范围
    float              m_fVoltBound[NUM_OF_IV_RANGE_V];    //电压的单边范围
} St_ChanBriefCfg;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_INDEPENDENT_READ_CFG_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wChannelNumber;
    WORD               m_wTotalChannelNumber;
    BYTE               m_btNone_11[2];                      // 预留reserved
    St_ChanBriefCfg    m_BriefCfg;
    BYTE               m_btCheckSum[2];                    // 结构共计:18+12*16=210字节
} MP_INDEPENDENT_READ_CFG_FEEDBACK;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_SET_SETTING
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    ST_SCHEDULE_SETTING_DATA    m_st_ScheduleSetting;       // FixedStruct
    ST_SCHEDULE_LOGSETTING_DATA    m_st_ScheduleLogSetting; // FixedStruct
    DWORD              m_dwNone_42;                         // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:208字节
} MP_SCHEDULE_SET_SETTING;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SYNCHRO_METAVARIABLE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wChannelNumber;
    St_MD5             m_StMD5Code;                         // MD5128bit
    DWORD              m_dwNone_41;                         // 预留reserved
    WORD               m_wStepID;
    WORD               m_wLimitCount;                       // MD5128bit
    WORD               m_wVariable_Count;
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
    BYTE               m_btCheckSum[2];                     // 结构共计:40+N字节
} MP_INDEPENDENT_METAVARIABLE;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字 = CMD_SCHEDULE_ACK_EVENT
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_FirstField    m_stMFirstField;              // （多包信息管理第一区域)
    BYTE               m_btTotal;                           // 确认事件编号总个数
    BYTE               m_btNone_11[3];                      //  预留  // reserved
    DWORD              m_dwRcvdSN[16];                      // 确认事件编号
    BYTE               m_btCheckSum[2];                     // 结构共计:102字节
} MP_SCHEDULE_ACK_EVENT;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_DOWNLOAD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
    BYTE               m_btCheckSum[2];                     // 结构共计:54+N字节
} MP_SCHEDULE_DOWNLOAD;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SIMULATION_DOWNLOAD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
    BYTE               m_btCheckSum[2];                     // 结构共计:54+N字节
} MP_SIMULATION_DOWNLOAD,MP_BATTERY_SIMULATION_DOWNLOAD;  //190218 zyx

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字 = CMD_SCHEDULE_GET_STATUS
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_Channel_Number;                    // 用最大码（0xFFFF）表示所有通道
    WORD               m_Interval;                          // 0表示退订，n表示间隔秒数
    BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:16字节
} MP_SCHEDULE_GET_STATUS;

/*typedef struct
{
DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_START
DWORD              m_dwCmd_Extend;                      // 扩展命令码字
ST_MultPack        m_stMPack;                           // （多包信息管理)
ST_Schedule_Jump    m_stStart;
float              m_fVclampHigh;
float              m_fVclampLow;
float              m_fCapacity;                         // 电池额定容量
WORD               m_wVariable_Count;
BYTE               m_btNone_112[2];                     // 预留reserved
BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
BYTE               m_btCheckSum[2];                     // 结构共计:110+N字节
} MP_SCHEDULE_START;*/

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_STOP
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    BYTE               m_btCheckSum[2];                     // 结构共计:54字节
} MP_SCHEDULE_STOP;

typedef struct
{
    float fMV_Value;
//	long lMV_Value;
//	double dMV_Value;
} METAVARIABLE_DATA;

typedef struct
{
    DWORD              	m_dwCmd;
    DWORD              	m_dwCmd_Extend;
    ST_MultPack        	m_stMPack;
    int				   				m_MV_Type;
    int				   				m_MV_MetaCode;
    int				   				m_MV_ValueType;
    METAVARIABLE_DATA  	m_MV_Data;
    DWORD				   			m_Channel;
    BYTE               	m_btCheckSum[2];

} MP_SET_META_VARIABLE;

#pragma pack(1)
typedef struct
{
    WORD  m_Channel;
    WORD  m_MV_MetaCode;  //隐藏第一层TE_DATA_TYPE中的MP_DATA_TYPE_METAVALUE应用，以及m_MV_ValueType为浮点数
    float fMV_Value;       // struct Length:8 Byte
} METAVARIABLE_DATA_CH_CODE;
#pragma pack()

typedef struct //07.20.2021
{
    DWORD               m_dwCmd;
    DWORD               m_dwCmd_Extend;
    WORD                m_wMV_Total;
    BYTE                m_btReserved2[18];
    METAVARIABLE_DATA_CH_CODE    m_MV_Data[MAX_METAVARIABLE_SINGLE_PACK];   // 8*X
    BYTE                m_btCheckSum[2];  // struct Length： 30+8*X
} MP_UPDATE_META_VARIABLE_ADVANCEMODEL;

typedef struct
{
    DWORD              m_dwCmd;
    DWORD              m_dwCmd_Extend;
    ST_MultPack_SecondField    m_stMSecondField;
    DWORD              m_dwCopyCmd;
    DWORD              m_dwCopyCmd_Extend;
    WORD               m_wResult;
    BYTE               m_btCheckSum[2];
} MP_SET_META_FEEDBACK;

#pragma pack(1)
typedef struct	//03.15.2022
{
	WORD  m_Channel;
	BYTE  m_MV_Error;
	BYTE  m_MV_DataType;
	WORD  m_MV_MetaCode;
	float m_Value;
} METAVARIABLE_DATA_CODE;
#pragma pack()

typedef struct	//03.15.2022
{
	DWORD                   m_dwCmd;
	DWORD                   m_dwCmd_Extend;
	WORD                    m_wMV_Total;
	BYTE                    m_btReserved2[18];
	METAVARIABLE_DATA_CODE  m_MV_Data[MAX_GET_METAVARIABLES_SINGLE_PACK];
	BYTE                    m_btCheckSum[2];
} MP_GET_META_VARIABLES;

typedef struct	
{
	DWORD                   m_dwCmd;
	DWORD                   m_dwCmd_Extend;
	BYTE                    m_btReserved2[20];
	float                   f_SetIntervalTimeLog;  // s
	BYTE                    m_btCheckSum[2];
} MP_SET_INTERVALTIME_LOGDATA;

typedef struct	//03.15.2022
{
	DWORD                            m_dwCmd; 
	DWORD                            m_dwCmd_Extend;
	WORD                             m_wMV_Total;
	BYTE                             m_btReserved2[18];
	METAVARIABLE_DATA_CODE			 m_MV_Data[MAX_GET_METAVARIABLES_SINGLE_PACK];
	BYTE                             m_btCheckSum[2]; 
} MP_GET_META_VARIABLES_FEEDBACK;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_CONTINUE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    BYTE               m_btCheckSum[2];                     // 结构共计:54字节
} MP_SCHEDULE_CONTINUE;   //hepeiqing,2015.03.16

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_JUMP
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    unsigned long      m_dwDataPoint;	                    //01.23.2018 下载增加DataPoint  //mits8
    unsigned long	   m_dwTestID;		                    //01.23.2018 下载增加TestID  //mits8
    ST_Schedule_Jump    m_stStart;
// 	float              m_fVclampHigh;//171113,hpq,add for pro8
// 	float              m_fVclampLow;//171113,hpq,add for pro8
// 	float              m_fCurrentSimulationVclampHigh;			//09.25.2017 add by zc//171113,hpq,add for pro8
// 	float              m_fCurrentSimulationVclampLow;			//09.25.2017 add by zc//171113,hpq,add for pro8
// 	float              m_fIclampHigh;       //170719 zyx//171113,hpq,add for pro8
// 	float              m_fIclampLow;        //170719 zyx//171113,hpq,add for pro8
// 	float              m_fPclampHigh;        //160518//171113,hpq,add for pro8
// 	float              m_fPclampLow;         //160518//171113,hpq,add for pro8
// 	float              m_fCapacity;                         // 电池额定容量//171113,hpq,add for pro8
// 	float              m_fUD1;
// 	float              m_fUD2;
// 	float              m_fUD3;
// 	float              m_fUD4;
    float              m_fUD[MAX_UD];        //180713 zyx
    WORD               m_wVariableValueCount;
//     BYTE               m_PclampHighControlType;             // 170609 zyx//171113,hpq,add for pro8
//     BYTE               m_PclampLowControlType;              // 170609 zyx//171113,hpq,add for pro8
// 	BYTE               m_IclampHighControlType;              //170719 zyx//171113,hpq,add for pro8

//	BYTE               m_btVoltage_Range;                   // 电压量程     //  20140416y//171113,hpq,add for pro8
    BYTE               m_btSyncStop:1;						// 温控箱同步停止标志
    BYTE               m_btNone_112:7;						// 预留reserved
    unsigned short int uw16_BurstModeDelayTime;      //  PC下载BurstMode延迟时间 1:100us 单位：100us [允许输入0-10000之间]
    unsigned long int  uw32_BurstModePointNum;       //  PC下载BurstMode打点总数  [允许输入100-1000之间]
    unsigned long int  uw32_BurstModeSampleSpeed;    //  PC下载BurstMode打点速度 1:100us记录一点 n:n*100us记录一点 [允许输入1-10之间]

    ST_SCHEDULE_VARIABLE    m_mvReset[MAX_SEND_METAVARIABLE];
    St_SchTestObject   m_datTestObject;
    BYTE               m_btCheckSum[2];                     // 结构共计:112+238=350字节
} MP_SCHEDULE_JUMP;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    St_Assign          m_st_Assign;
    BYTE               m_btCheckSum[2];                     // 结构共计:58+20*N=378字节
} MP_SCHEDULE_ASSIGN;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    BYTE               m_btNone_11[4];                      // 预留reserved
    St_Assign          m_st_Assign;
    BYTE               m_btCheckSum[2];                     // 结构共计:38+20*N=358字节
} MP_ASSIGN_FEEDBACK;

/*
typedef enum //group模式
{
eGroupMode_Single = 0,  //default. No grouping, signel channel running mode。单通道运行状态。
eGroupMode_ParaIntraUnit = 1,  // Parallel channels in a single unit unit。内部并联。如果是单通道运行，属于特殊的unit内部并联。
eGroupMode_ParaInterUnit = 2,  //paralle among serveral units。多unit并联
eGroupMode_Serial = 3,               //serial。串联
eGroupMode_Booster = 4            //dynamic booster。Booster模式
}En_GroupMode;

//09.22 redefine to:

#define eGroupMode_Single (0)  //default. No grouping, signel channel running mode。单通道运行状态。
#define eGroupMode_ParaIntraUnit (1)  // Parallel channels in a single unit unit。内部并联。如果是单通道运行，属于特殊的unit内部并联。
#define eGroupMode_ParaInterUnit (2)  //paralle among serveral units。多unit并联
#define eGroupMode_Serial (3)               //serial。串联
#define eGroupMode_Booster (4)


*/


typedef struct
{
    unsigned char m_Unit_Idx;     //unit 号
    unsigned char m_beSlaveUnit;     // unit 内通道号
} ST_UNIT_N_CHN;
typedef struct
{
    unsigned char m_Unit_Idx;     //unit 号
    unsigned char m_Chn_Idx;     // unit 内通道号
} ST_SUPERVIOSR;
typedef struct
{

    unsigned char		m_HeadChn;								//首通道号
    unsigned char		m_HeadChnRank;							//0 是 Master, 1; SubMaster, 2;Slave
    ST_SUPERVIOSR 	m_Supervisor;							//该通道的上级通道; 如果是Master，上级通道就是自己。如果是SubMaster，上级通道就是master。如果是 slave，上级通道就是 Sub master。
    unsigned char		m_InUnitGroupSize;						//在该unit内的通道数
    unsigned char       m_uGroupUnitCount;                      //作为主通道时支持的从Unit数量
    unsigned char		m_WholeGroupSize ;						//如果是master的话，整个group的通道数
    ST_UNIT_N_CHN		m_SubMasterChannels[MAX_UNIT_PER_GROUP];	//如果是master的话，自己的sub master通道位置
} ST_UNIT_GROUP_INFO;
typedef struct
{
    DWORD				m_dwCmd;								// 执行命令码字=CMD_BATCH_GROUP
    DWORD				m_dwCmd_Extend;							// 扩展命令码字
    ST_UNIT_GROUP_INFO	m_UnitGroupInfo[MAX_GROUP_PER_UNIT];
    unsigned char		m_GroupCounter;
} MP_BATCH_GROUP;

typedef struct //12.22.2013ADD
{
    DWORD              m_dwCmd;                             // 4 执行命令码字=CMD_SIMULATION_ASSIGN
    DWORD              m_dwCmd_Extend;                      // 4 扩展命令码字
    ST_MultPack        m_stMPack;                           // 44（多包信息管理)
    St_MD5Assign       m_simu_MD5Assign;                    // 20
    BYTE               m_btCheckSum[2];                     // 2
} MP_SIMULATION_ASSIGN;  //KL-SIMU  //结构共计: 74


typedef struct //12.22.2013ADD
{
    DWORD              m_dwCmd;                             // 4 执行命令码字=CMD_SIMULATION_ASSIGN_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 4 扩展命令码字
    St_MD5Assign       m_simu_MD5Assign;                    //20
    BYTE               m_btCheckSum[2];                     // 2  //结构共计: 30
} MP_SIMULATION_ASSIGN_FEEDBACK;  //KL-SIMU

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_EVENT
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:120字节
    //BYTE               m_btNone_11[2];                      // 预留reserved
    BYTE			   m_ACIM[2];							// ACIM请求标志
    St_MD5             m_StMD5Code;
    WORD               m_wChannelNumber;
    WORD               m_wStepID;
    BYTE               m_btEvent;                           // 总入口判定事件(上位机用)
    BYTE               m_ItemCount_SDL;
    BYTE               m_btNone_12[2];                      // 预留reserved
    DWORD              m_dwSendSN;                          // 发送事件的编号
    DWORD              m_dwCycleID;
    DWORD              m_TC_Counter[4];                     //170314,add by hepeiqing,for TC_Counter log in sql mode
    DWORD	           m_TestID;                             //mits8
    MsTime_Pack        m_stTime;                            // 12字节  注意这里为补齐使用6字节
    ST_GETTIME_DATA    m_st_TestTime;                       // 8字节
    ST_GETTIME_DATA    m_st_StepTime;                       // 8字节
    ST_EVENT_SCHEDULE_LIMITREACH    m_st_LimitReachStatus;  // 20字节
    ST_EVENT_SCHEDULE_RUNNING    m_st_RunningStatus;        // 24字节
    BYTE               m_btSCHEvent_Source;
    BYTE               m_btNone_13[3];                      // 预留reserved   注意这里为补齐使用4字节
} MP_REPORT_EVENT_HEADER;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_METAVARIABLESYNCH
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:40+240=280字节
    WORD               m_wChannelNumber;
    BYTE               m_btNone_11[2];                      // 预留reserved
    MsTime             Time ;                               // 6字节
    DWORD              m_dwSendSN;
    WORD               m_wVariable_Count;
    BYTE               m_btNone_12[5];                      // 预留reserved
    BYTE               m_ChannelStatus;	//zyx 171010					//170831 zyx
    unsigned long      m_DataPoint; //180511
    ST_SCHEDULE_VARIABLE    m_st_Variable[MAX_SEND_METAVARIABLE];// 8*30
} MP_REPORT_METAVARIABLESYNCH;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_LOGDATA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:96字节
    St_MD5             m_StMD5Code;
    WORD               m_wChannelNumber;
    WORD               m_wStepID;
    DWORD              m_dwCycleID;
    WORD               m_wVariable_Count;
    WORD               uw16_Setting;
    BYTE               uc_LogType;                          // LOG,DISPLAY,IDLE
    WORD               m_u16Token;
    ST_GETTIME_DATA    m_st_TestTime;                       // 8字节
    ST_GETTIME_DATA    m_st_StepTime;
    ST_GETTIME_DATA    m_st_SampleTime;
    float              fCurrent;
    float              fVoltage;
    float              fChargeCapacity;
    float              fDischargeCapacity;
    float              fChargeEnergy;
    float              fDischargeEnergy;
    float              fIR;
    float              fdVdt;
    float              fChargeCapacityTime;     //zyx 171010
    float              fDischargeCapacityTime;
    float              fBuildInVolt[MAX_BUILDIN_2V_COUNT];
    float              fBuiltInData[MAX_BUILTIN_COUNT];
} MP_REPORT_LOGDATA;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_LOGDATA_DELTA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:96字节
    St_MD5             m_StMD5Code;
    WORD               m_wChannelNumber;
    WORD               m_wStepID;
    DWORD              m_dwCycleID;
    WORD               m_wVariable_Count;
    WORD               uw16_Setting;
    BYTE               uc_LogType;                          // LOG,DISPLAY,IDLE
    WORD               m_u16Token;
    ST_GETTIME_DATA    m_st_TestTime;                       // 8字节
    ST_GETTIME_DATA    m_st_StepTime;
    ST_GETTIME_DATA    m_st_SampleTime;
    float              fCurrent;
    float              fVoltage;
    float              fChargeCapacity;
    float              fDischargeCapacity;
    float              fChargeEnergy;
    float              fDischargeEnergy;
    float              fIR;
    float              fdVdt;
    float              fChargeCapacityTime;     //zyx 171010
    float              fDischargeCapacityTime;
    float              fBuildInVolt[MAX_BUILDIN_2V_COUNT];
    float              fBuiltInData[MAX_BUILTIN_COUNT];
    float              fTC_Counter1;
    float              fTC_Counter2;
    float              fTC_Counter3;
    float              fTC_Counter4;
    float              fACIR;
    float			   fdQdV; //zyx 170801
    float              fdVdQ; //zyx 170801
} MP_REPORT_LOGDATA_TC_Counter;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_LOGDATA_DELTA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:1428字节
    WORD               m_u16Token;
    unsigned char      ItemCount;
    St_SDL_Point_Pack    Point[MAX_AUX_SDL_POINT];                        // 13*109=1417
} MP_REPORT_LOGDATA_SDL;

typedef struct
{
    DWORD			m_dwCmd;
    DWORD			m_dwCmd_Extend;
    BYTE				m_btCheckSum[2];
    WORD				m_u16Token;
    St_SDL_GroupPoints groupedPoints;
    //unsigned char itemcount;
} MP_REPORT_GRPLOGDATA_SDL;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_BUILDINAUX_LOGDATA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:28+N*72
    WORD               m_u16Token;
    MsTime_Pack        m_stTime;                            // 12字节  注意这里为补齐使用6字节
    BYTE               ItemCount;
    BYTE               m_btNone_11[3];                      // 预留reserved
    St_BUILDINAUX_VALUE m_stBuildInAux_Value[8];            // 内建辅助通道数值。 // N*72=72
} MP_REPORT_BUILDINAUX_LOGDATA;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_PULSE_LOGDATA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:52+12*110字节
    WORD               m_u16Token;
    WORD               m_wChannelNumber;
    BYTE               m_btNone_11;                         // 预留reserved
    BYTE               m_btIfPartialPusle;                  // 是否分批脉冲。	// 0表示完整脉冲  // 1表示不完整脉冲
    WORD               m_Reservedx;
    unsigned long	   u32TestID;
    WORD               m_wStepID;
    DWORD              m_dwCycleID;                         //2014/12/15,add by hepeiqing,fix pulse step log bug
    MsTime             m_Time;
    MsTime_Pack        m_TestStartTime;
    MsTime_Pack        m_StepStartTime;                     //2014/12/19,add by hepeiqing,fix pulse step time < 0 bug
    DWORD              m_dwPulseCycleID;                    // 脉冲Cycle号。预留，将来可能进行记录
    MsTime_Pack        m_stPulseBeginTime;                  // 脉冲第一点绝对时间。这是脉冲的第一点的时间，而不是本包内第一点的时间。这个第一点的时间最好是这个脉冲第一个Stage的DAC输出的时间。第一个Stage的第一个点的采样时间也可以接受。 // 12字节  注意这里为补齐使用6字节
    unsigned long      u32DataPoint;
    BYTE               m_btPointCount;                      // 本包内点数。注意点数不是Stage数。有n个Stage的脉冲实际上有2n个点。
    BYTE               m_btFirstPointID;                    // 本包内第一点的点号。第k个Stage的第m个点，其点号为2(k-1)+m-1。举例来说，第一个Stage的第一个点其点号为0；第十个Stage的最后一点其点号为19。这里需要注意以后是否不限制每个Stage两个点，那样的话需要重新设计。
    BYTE               m_btIfHaveCapEng;                    // 本包是否包含电量能量数据。这个可以讨论，是否脉冲数据就不带电量能量，电量能量数据另外以普通数据点上传
    BYTE               m_btNone_12;                         // 预留reserved
    float              m_fBuildInVolt[MAX_BUILDIN_2V_COUNT];
    float              m_fCCapOfLastPoint;                  // 本包最后一点的电量。如果IfHaveCapEng为False那么就不存在
    float              m_fDCapOfLastPoint;                  // 本包最后一点的电量。
    float              m_fCEngOfLastPoint;                  // 本包最后一点的能量。
    float              m_fDEngOfLastPoint;                  // 本包最后一点的能量。
    St_PULSE_VCT       m_fV_C_T[PACK_MAX_PULSE_POINT];      // 每点电流、电压、相对时间。4*3=12字节，这部分重复PointCount次每点相对时间单位为微秒，相对的是本脉冲第一点而不是本包第一点。
} MP_REPORT_PULSE_LOGDATA;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=MP_SCHEDULE_REPORT_LOGDATA_DELTA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btNone_11[8];                      // 预留reserved
    ST_GETTIME_DATA    m_st_SampleTime;
    WORD               m_wChannelNumber;
    WORD               m_wVariable_Count;
    BYTE               m_btPackData_N[MAX_CMD_BUFFER-200];
    DWORD              m_dwCheckSum;
} MP_REPORT_LOGDATA_DELTA;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_REQUEST_NTPCALIB
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:20字节
    BYTE               m_Request_NTP;                       // 请求NTP对时
    BYTE               m_btNone_11[1];                     // 预留reserved
    signed long int    m_Offset_ms;                        // MCU与PC偏差值毫秒
    signed short       m_RTC_CalibSet;                     // RTC CalibSet Value
    BYTE               m_btNone_12[2];                      // 预留reserved
} MP_REPORT_REQUEST_NTPCALIB;


typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_FEEDBACK_DATA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:14字节
    DWORD              m_dwSendSN;                          // 发送事件的编号
} MP_REQUEST_SYSTEMTIME;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字  // 结构共计:8字节字
} MP_HEADER_NEW;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_CONFIRM_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    DWORD              m_dwCopyCmd;                         // 原命令备份执行命令码字
    DWORD              m_dwCopyCmd_Extend;                  // 原命令备份扩展命令码字
    WORD               m_btResult;                          // 0:Failed1:Success
    BYTE               m_btMultPack_OK;
    BYTE               m_btError_Code;                      // 配置错误代码，查配置错误代码表。
    BYTE               m_btNone_11[4];                      // 多包接收完成结束
    MsTime             Time;
    BYTE               m_btCheckSum[2];                     // 结构共计:52字节
} MP_CONFIRM_FEEDBACK;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_WRITE_FRAM
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:48字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    WORD               m_wChannelBitIdx;
    WORD               m_wChannelCount;
    WORD               m_wFramType;
    WORD               m_u16Length;
    UINT               m_StartAddress;
    MsTime             m_MsTime;
} MP_REPORT_FRAM_EVENT;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_CHANNEL_BOARD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:112+8K字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    unsigned short     m_u16Reserved;
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    St_BoardGlobalCFG    m_Global;                          // 结构:36字节
    St_ChannelCFG      m_Channel[16];                       // 结构:468*16=7488字节
    St_IV_ChannelPID    m_stChannelPID[16];                 // 结构:144*16=2304字节
} MP_REPORT_GET_CHANNEL_BOARD;
typedef struct //MP_REPORT_GET_CALI_CHANNEL_BOARD
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_CHANNEL_BOARD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:112+8K字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    St_CaliBoardCfg    m_config;
} MP_REPORT_GET_CALI_CHANNEL_BOARD;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_AUX_CHANNEL_BOARD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:76+6188字节    // 结构共计: （32+44）+（44+4*128）+ (44*128)= 6.264K字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    ST_MultPack        m_stMPack;                           // （多包信息管理)  44字节
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    unsigned short     m_u16Reserved;
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    St_AuxBoardGlobalCFG    m_Global;                       // 44+92*128字节
    St_AUXChannelCFG    m_Channel[MAX_CH_IN_AUX_UNIT];      // 96*128字节
} MP_REPORT_GET_AUX_CHANNEL_BOARD;

typedef struct
{
    MsTime_Pack        m_System_Time;                        // 12字节  注意这里为补齐使用6字节
    ST_GETTIME_DATA    m_Test_Time;                          // 8字节
    ST_GETTIME_DATA    m_Step_Time;                          // 8字节
    BYTE               m_Schedule_Status;
    BYTE               m_btNone_11;                          // 预留reserved
    WORD               m_Step_ID;
    DWORD              m_Cycle_ID;
    WORD               m_Channel_Status;
    WORD               m_Channel_Number;                      // 预留reserved // 结构：40字节
} REPORT_GET_STATUS;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_SCHSTATUS
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:68 + 40字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    ST_MultPack        m_stMPack;                           // （多包信息管理) 44字节
    WORD               m_Status_Count;                      // 数目。
    BYTE               m_btNone_12[2];                      // 预留reserved
    REPORT_GET_STATUS  m_Ch_Status[32];                     // 结构：40字节 * 32
} MP_REPORT_GET_SCHSTATUS;

typedef  struct
{
    BYTE               m_btBoard_ID;                        // I2C的板拔号ID: 0 ~ 7。
    BYTE               m_btAuxType;                         // 辅助类型一级分类AuxType
    BYTE               m_btAux_SubType;                     // 辅助类型二级分类 Aux_SubType
    BYTE               m_u8ChCount;                         // 辅助通道数量;
} St_AUXBOARD_ERROR;

typedef  struct
{
    BYTE               m_btChannel_ID;                      // 通道号。
    BYTE               m_btCode_Type;                       // 代码类型如警告。
    BYTE               m_btType_ErrorNum;                   // 代码类型中的错误号。
    BYTE               m_btSubCode;                         // 代码类型中的错误号子集。
    float              m_fIR_Value;                         // 内阻测试值
} St_IV_ERROR;

typedef  union
{
    St_AUXBOARD_ERROR  m_AuxBoard;    //  辅助
    St_IV_ERROR        m_IV_ERROR;    //  IV
    //BYTE             m_btNone_11[4];                      // 预留reserved
} Un_ERROR_MESSAGE;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_TOPC_ERROR
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字  16+1*16+4*16
    BYTE               m_btCheckSum[2];                     // 结构共计:  字节
    BYTE               m_btSendSource;                      // 发送事件的来源模块
    BYTE               m_btResult;                          // 0:Failed  1:Success
    BYTE               m_btNone_11[3];
    BYTE               m_btTotalCout;                        // 上报错误结构的总个数。
    BYTE               m_btError_Code[MAXIV4AUXCHANNELNO];   // 第二信道报告的错误代码，查错误代码表。
    Un_ERROR_MESSAGE   m_Error_Message[MAXIV4AUXCHANNELNO];  // 确认错误信息
} MP_REPORT_TOPC_ERROR;


typedef  struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_SIMULATION_REQUESTDATA
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字  32+2*600
    BYTE               m_btCheckSum[2];                     // 结构共计:  字节
    BYTE               m_btNone_11[2];
    St_MD5             m_simu_MD5;                          //  16
    WORD               m_wTotalCout;                        // 上报丢失的总个数。
    BYTE               m_btNone_12[2];
    WORD               m_wPackIndex[MAX_SIMU_REPORT_PACKINDEX];  // 第二信道报告丢失的包号表。
} MP_REPORT_SIMULATION_REQUESTDATA;

typedef  struct
{
    BYTE             m_btResult;                // 0:Failed  1:Success
    BYTE             m_btControl;               // 控制执行命令
    BYTE             m_btNone_11[2];            // 预留  // reserved
    Un_ERROR_MESSAGE   m_Error_Message;         // 原错误信息
} St_AUXCONFIRM;

typedef  union
{
    St_AUXCONFIRM     m_AuxConfirm;    //  辅助
    //Un_IV_X2        m_IV_X2;        //   IV
    //BYTE            m_btNone_11[4];                      // 预留reserved
} Un_CONFIRM_MESSAGE;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字 = CMD_RSN_ERROR_ACK_EVENT
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btTotal;                           // 确认事件编号总个数
    BYTE               m_btNone_11[3];                      //  预留  // reserved
    Un_CONFIRM_MESSAGE m_Confirm_Message[MAXIV4AUXCHANNELNO]; // 确认信息
    BYTE               m_btCheckSum[2];
} MP_RSN_ERROR_ACK_EVENT;


typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_LIMITSTATUS
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD               m_wSchID;                            // =0 Reserved for later use
    WORD               m_wStepID;                           // =2 Reserved for later use
    BYTE               m_btFirstChanIdx;                    // 4 first channel, zero based index
    BYTE               m_btTotalChanNum;                    // 5 Number of channels (FF = All)
    WORD               m_wFlag0;                            //  6/7 Reserved
    DWORD              m_wCycleIndex;                       // 8
    BYTE               m_bIfSelfJump;                       // 12  IV下位机是否可以自主跳步
    BYTE               m_Flag;                              // 13 Number of channels (FF = All)
    WORD               m_wLimitIndex;
    BYTE               m_btCheckSum[2];                     // 结构共计:26字节
} MP_LIMITSTATUS;

typedef struct
{
    unsigned short     m_u16ChannelCount;                   // 数据包里下载的通道数目。
    //unsigned short     m_u16Reserved;                     //12.23.2017 COMMENT OUT
    unsigned short     m_uAuxMapState;                       //12.23.2017 AUX_MAP_STATE
    unsigned char      m_ucChannelIdx[16];                  // 16个byte=128个bit，使用bit位来标识最多128个通道中，哪些通道被下载了？
    //St_PC2IVUnit4AuxMap      m_PC2IVUnit4AuxMap[MAXCHANNELNO];                      // 结构共计:59K字节
    unsigned short      m_Total_In_OneIVCh[MAXCHANNELNO];    // 一个IV通道分到的辅助通道总数量
    BYTE               m_btNone_11[4];                      // 预留reserved
    St_OneAuxChInfo    m_AuxInfo_In_OneIVCh[MAX_MAP_COUNT_PER_UNIT];  // 一个IV通道分到的辅助通道
} MP_IVUnit4AuxMap;

typedef struct
{
    unsigned char      m_ucUnitCount;             // 数据包里下载的单元数目。
    BYTE               m_btNone_11[3];            // 预留reserved
    unsigned char      m_ucIVChCount[MAX_IVUNIT];         //  数据包里下载的单元里IV数目。
    unsigned char      m_Total_In_ThisIVCh[MAX_IVUNIT][MAXCHANNELNO][MAXAUXTYPE];  // 同种辅助类型下该IV通道下的辅助通道总数量
    BYTE               m_btNone_12[4];            // 预留reserved
    //      St_MAP_PCDOWN_AuxCh2IVCh    m_AllIVMap[16*MAXCHANNELNO*MAXAUXTYPE*MAX_CH_IN_AUX_UNIT];                      // 结构共计:   字节
    St_DOWNLOAD_MAP_FROM_PC    m_AllIVMap[MAX_MAP_COUNT_PER_UNIT];  //
} MP_AllIVMap;


//继承子类型结构体定义
typedef struct MP_REPORT_EVENT: public MP_REPORT_EVENT_HEADER
{
} MP_REPORT_EVENT;

typedef struct MP_REPORT_EVENT_SDL: public MP_REPORT_EVENT_HEADER
{
    //St_SDL_Point_Pack    m_st_SDLData[7];         // 13*7=91     // 结构共计:216字节
    St_Data_Pairs      m_st_SDLData;               //mits8
    BYTE               m_btNone_14[5];            // 预留reserved   注意这里为补齐使用4字节
} MP_REPORT_EVENT_SDL;


typedef struct MP_SENT_ACIMTEST_START
{
    DWORD	m_dwCmd;                             //执行命令码字=CMD_SENT_ACIMTEST_START
    DWORD	m_dwCmd_Extend;                      //扩展命令码
    WORD	m_ChannelID;
    WORD	m_Cycle;
    BYTE	m_Status;
    BYTE	m_TestName[3];
    float	m_FREQINIT;
    float	m_FREQFINAL;
    float	m_Point_Decade;
    float	m_IACValue;
    float	m_IDCValue;
    float	m_Estimated_ZValue;
    BYTE	m_None[2];
    BYTE	m_btCheckSum[2];                     // 结构共计４４字节
} MP_SENT_ACIMTEST_START;  //齐静明 2017.07.12

typedef struct
{
    DWORD	m_dwCmd;                             //执行命令码字=CMD_SENT_ACIMTEST_START
    DWORD	m_dwCmd_Extend;                      //扩展命令码
    WORD	m_ChannelID;
    WORD	m_Cycle;
    BYTE	m_Status;
    BYTE	m_TestName[3];
    float	m_FREQINIT;
    float	m_FREQFINAL;
    float	m_Point_Decade;
    float	m_IACValue;
    float	m_IDCValue;
    float	m_Estimated_ZValue;
    BYTE	m_None[2];
    BYTE	m_btCheckSum[2];                     // 结构共计４４字节
} MP_SENT_ACIMTEST_STOP;  //DKQ 2017.09.21

typedef struct
{
    //MsTime	m_TestTime;
    float	m_Frequency;
    //float	m_Zreal;
    //float	m_Zimag;
    //float	m_Zsig;
    float	m_Zmod;
    float	m_Zphase;
    //float	m_Idc;
    //float	m_Vdc;
    //float	m_IERange;
} St_ACIM_Result_data;


typedef struct
{
    DWORD	m_dwCmd;                             //执行命令码字=CMD_SENT_ACIMTEST_START
    DWORD	m_dwCmd_Extend;                      //扩展命令码
    BYTE	m_btCheckSum[2];
    BYTE	m_btNone_11[2];
    DWORD   m_dwSendSN;
    bool			m_lastPoint;
    St_ACIM_Result_data	m_Point[MAX_ACIM_DATA_POINT_NUM];
} MP_REPORT_ACIM_DATA;  //齐静明 2017.07.12

enum en_CANMeta_DataType
{
    eType_Unsigned,
    eType_Signed,
    eType_Float,
};


typedef struct
{
    CANBMS_METACOUNT TotalItems;
    CANBMS_METACOUNT FastItems;
    CANBMS_METACOUNT MidItems;
    CANBMS_METACOUNT SlowItems;
} ST_CANBMS_ITEMS; //170606 zyx

typedef enum CAN_SPEED_ENUM
{
    CAN_SPEED_125K,
    CAN_SPEED_250K,
    CAN_SPEED_500K,
    CAN_SPEED_1M,
    CAN_SPEED_ENUM_NUM

} en_CANBAUD;

typedef struct
{

    CANBMS_METACODE  m_CAN_RX_Meta;   //enum char 1705051 zyx
    unsigned char    m_Enable;
    unsigned char    m_DataLog;
    unsigned char    m_DataType;

    unsigned char  m_DLC;
    unsigned int   m_StartDataIndex;   //64BIT ??Index
    unsigned int   m_EndDataIndex;    //64BIT ??Index
    unsigned char  m_ByteOrder;
    unsigned char  m_CanPort;
    unsigned int   m_CANMessageID;
    float          m_MinValue;
    float          m_MaxValue;
    float          m_Offset;
    float          m_ScaleFactor;

} ST_CAN_BMS_SIGNAL ; //28 byte
typedef struct
{
    bool                 CAN_IsSelectPort[MAX_CANBMS_PORT];
    unsigned int         m_BrdCst_ID; //广播ID
    unsigned char        m_FrameType;//标准帧还是扩展帧
    unsigned char        m_Endian;	//大小头选择
    unsigned int         m_BrdCst_interval; //广播时间间隔(ms)
    unsigned char        m_DataMode[2];//数据模式 0:figure,1:MetaVariable,2:Formula。
    float                m_BrdCstData[2];//数字、MetaVariable代码、公式序号。
} ST_CAN_BroadcastInfo; //

typedef struct
{
    unsigned short       m_u16OutboundSignalIndex;
    unsigned char        m_u8ValueType;
    unsigned char        m_u8Rvd;
    float                m_fValue;
    float                m_fRvd[2];
} ST_BMS_CFG_OutboundSignal;
typedef struct
{   //190617,hpq
    unsigned int     m_u32CANID;//191029 zyx
    unsigned short     m_u16OutboundSignalIndex;
    float              m_fGain;
    float              m_fOffset;
    float              m_fMinValue;
    float              m_fMaxValue;
    unsigned char      m_u8DLC;
    unsigned char      m_u8ByteOrder;
    unsigned char      m_u8DataType;
    unsigned char      m_u8RVD;
    unsigned char      m_u8StartByte;
    unsigned char      m_u8StartBit;
    unsigned char      m_u8EndByte;
    unsigned char      m_u8EndBit;
    float							 m_lineTranGain;
    float              m_lineTranOffset;
//    float              m_fRvd[2];          // ?????,??4????  // ????:16??
} St_BMS_CFG_OutboundMessage;
#define MAX_CAN_BMS_OUTBOUNDSIGNAL   32  //190617,hpq
#define MAX_CAN_BMS_OUTBOUNDMESSAGE  32  //190617,hpq
typedef struct
{
    bool 								 m_IsCANFD[MAX_CANBMS_PORT];
    unsigned char 			 m_BitRate[MAX_CANBMS_PORT];
    unsigned char        m_CANBMS_Count;
    unsigned char        m_BaudRate[MAX_CANBMS_PORT];        //CANBAUD250k = 2,CANBAUD500k = 3,CANBAUD1M = 4
    //unsigned char        m_FrameType;
    //unsigned char        m_Endian;
    ST_CANBMS_ITEMS      m_ItemCount; //char 1705051 zyx

    //unsigned int         m_BrdCst_ID;
    //unsigned int         m_BrdCst_interval;
    unsigned int         m_UnsafeTimeout[MAX_CANBMS_PORT];
    ST_CAN_BroadcastInfo      m_BrocastInfo[BMS_BROAD_CST_COUNT];//3组广播信息

    unsigned int         m_Reqst_MsgID;
    unsigned int         m_Ans_MsgID;

    St_FormulaData       m_FormulaData[16];//CAN config中16组公式
    unsigned char        m_BMS_OutboundSignal_Count;   //190617,hpq
    unsigned char        m_BMS_OutboundMessage_Count;  //190617,hpq
    unsigned char 	     m_rvd[30];                    //190617,hpq  32==>30
} ST_CAN_GlobalCfg;

typedef struct
{
    ST_CAN_GlobalCfg     m_GlobalCfg;
    ST_BMS_CFG_OutboundSignal  m_CAN_OutboundSignals[MAX_CAN_BMS_OUTBOUNDSIGNAL];    //190617,hpq
    St_BMS_CFG_OutboundMessage  m_CAN_OutboundMessages[MAX_CAN_BMS_OUTBOUNDMESSAGE];  //190617,hpq
    ST_CAN_BMS_SIGNAL    m_CAN_Signals[MAX_CAN_BMS_SIGNAL];  //max can bms signal256
} ST_CANBMS_CfgUnit;   // *CAN Number (332)  6*4 + 28*itemcount

typedef struct //170828 zyx
{
    unsigned char    FrameType;
    unsigned char    DLC;
    unsigned int     m_CANID;       //aux get this iv unit id
    unsigned char    CANData[8];
} ST_SEND_MULTI_CANBMS_MSG;

typedef struct //170828 zyx
{
    unsigned char               m_AllStop;      //can msg send global info
    int                         m_validMsgCnt;
    unsigned long long          m_set_SendInterval; // 全部msg发送后等待间隔
    unsigned long long          m_set_FrameInterval; //每帧msg发送后等待间隔
    ST_SEND_MULTI_CANBMS_MSG    m_Msg[MULTI_CANMSG_COUNT];
} ST_CANBMS_AuxMultiMsg;
typedef struct //170828 zyx
{
    bool                    bIsOneCycle;
    int                     MsgIdx;
    MsTime                  m_SendInterval;
    MsTime                  m_FrameInterval;
    ST_CANBMS_AuxMultiMsg   m_BMSAuxMultiMsg;
} ST_AuxMultiMsgManage;

typedef struct
{
    unsigned int       m_IVChnIdx;
    CANBMS_METACOUNT   m_ItemCount[MAXCHANNELNO]; //170505 zyx
    ST_CANBMS_CfgUnit  m_BMS_Chn[MAXCHANNELNO];
} ST_CANBMS_CONFIG;
typedef struct
{
    unsigned int      m_AllChnNum;
    struct
    {
        unsigned char       m_IVChnNo;
        bool 		            m_bIVActive;       // this CAN bms channel is disable?
        unsigned char       m_AuxUnitID;        //source CANID,transfer aux unit
        unsigned char                m_CANBMSChnIdx;
    } m_CANmap[MAXCHANNELNO];
} ST_CANBMS_MAP;

typedef struct
{
    unsigned char   m_IVChnNo;
    //unsigned char   m_IVUnitID;
    unsigned char   m_bBmsLost:1;      //170122 false:CAN bms signal OK,   true:CAN bms signal lost,
    unsigned char   m_bStepLastPoint:1;
    unsigned char   m_bReserved:6;
    unsigned short  m_Us100;
    unsigned long   m_Second;
    unsigned short    m_DataCnt;
    unsigned short    m_DataType;

    struct
    {
        unsigned char   b_IsLogData:1;
        unsigned char   m_LogDataInterval:7; // 170608 zyx short -> char
        CANBMS_METACODE m_MetaCode;
        float           m_Value;
    } m_Meta[MAX_CAN_BMS_SIGNAL];

} ST_CANBMS_RptData;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_CHANNEL_BOARD
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:56+1542字节
    BYTE               m_btNone_11[2];                      // 预留reserved
    ST_MultPack        m_stMPack;                           // （多包信息管理)  44
    ST_CANBMS_RptData  m_CANData;                           //  12+6*255 = 1542
} MP_REPORT_CANBMS_Data;
//命令联合体定义

//命令字结构体定义
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    St_CAN_SendMsg     m_CANMsg;
    BYTE               m_IVChnNo;
    BYTE               m_btNone_12[1];                      // 预留reserved
    WORD                m_u16Token;
    DWORD              m_dwNone_41;                         // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:24字节
} MP_SET_CANMSG;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    DWORD              m_dwCopyCmd;                         // 原命令备份执行命令码字
    DWORD              m_dwCopyCmd_Extend;                  // 原命令备份扩展命令码字
    WORD               m_wResult;                           // 0:Failed1:Success
    BYTE               m_dwNone_22[2];                         // 预留reserved
    BYTE               m_btCheckSum[2];                     // 结构共计:48字节
} MP_SET_CANMSG_FEEDBACK;

typedef struct
{
    unsigned short     m_u16ChannelIdx;
    unsigned char      m_ucMD5Count;
    unsigned char      m_ucReserved;
    St_MD5Assign       m_MD5Assign;// 结构共计:324字节
} St_AssignCAN;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    St_AssignCAN       m_st_AssignCAN;
    BYTE               m_btCheckSum[2];                     // 结构共计:58+20*N=378字节
} MP_CANCONFIG_ASSIGN;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    BYTE               m_btNone_11[4];                      // 预留reserved
    St_AssignCAN       m_st_Assign;
    BYTE               m_btCheckSum[2];                     // 结构共计:38+20*N=358字节
} MP_ASSIGN_CAN_FEEDBACK;




///////////////////SmartBattery
typedef enum
{
    MANUFACTURERACCESS=0x00,
    REMAININGCAPACITYALARM=0x01,
    RAMAININGTIMEALARM=0x02,
    BATTERYMODE=0x03,
    ATRATE,
    ATRATETIMETOFULL,
    ATRATETIMETOEMPTY,
    ATRATEOK,
    TEMPERATURE=0x08,
    VOLTAGE,
    CURRENT,
    AVERAGECURRENT,
    MAXERROR,
    RELATIVESTATEOFCHARGE,
    ABSOLUTESTATEOFCHARGE=0x0e,
    REMAININGCAPACITY,
    FULLCHARGECAPACITY,
    RUNTIMETOEMPTY,
    AVERAGETIMETOEMPTY,
    AVERAGETIMETOFULL,
    CHARGINGCURRENT,
    CHARGINGVOLTAGE,
    BATTERYSTATUS,
    CYCLECOUNT,
    DESIGNCAPACITY,
    DESIGNVOLTAGE=0x19,
    SPECIFICATIONINFO,
    MANUFACTUREDATE=0x1b,
    SERIALNUMBER,
    RESERVED1,
    RESERVED2,
    RESERVED3,
    MANUFACTURERNAME=0x20,
    DEVICENAME,
    DEVICECHEMISTRY,
    MANUFACTURERDATA=0x23,
    OPTIONALMFGFUNCTION5=0x2f,
    OPTIONALMFGFUNCTION4=0x3c,
    OPTIONALMFGFUNCTION3,
    OPTIONALMFGFUNCTION2,
    OPTIONALMFGFUNCTION1,

} SMARTBATTERY_FUNCTION_CODE;
typedef enum
{
    SMB_META_RX1 = 0,
    SMB_META_RX2,
    SMB_META_RX3,
    SMB_META_RX4,
    SMB_META_RX5,
    SMB_META_RX6,
    SMB_META_RX7,
    SMB_META_RX8,
    SMB_META_RX9,
    SMB_META_RX10,

    SMB_META_RX11,
    SMB_META_RX12,
    SMB_META_RX13,
    SMB_META_RX14,
    SMB_META_RX15,
    SMB_META_RX16,
    SMB_META_RX17,
    SMB_META_RX18,
    SMB_META_RX19,
    SMB_META_RX20,

    SMB_META_RX21,
    SMB_META_RX22,
    SMB_META_RX23,
    SMB_META_RX24,
    SMB_META_RX25,
    SMB_META_RX26,
    SMB_META_RX27,
    SMB_META_RX28,
    SMB_META_RX29,
    SMB_META_RX30,

    SMB_META_RX31,
    SMB_META_RX32,
    SMB_META_RX33,
    SMB_META_RX34,
    SMB_META_RX35,
    SMB_META_RX36,
    SMB_META_RX37,
    SMB_META_RX38,
    SMB_META_RX39,

    SMB_META_RX40,
    SMB_META_RX41,
    SMB_META_RX42,
    SMB_META_RX43,
    SMB_META_RX44,
    SMB_META_RX45,
    SMB_META_RX46,
    SMB_META_RX47,
    SMB_META_RX48,
    SMB_META_RX49,
    SMB_META_RX50,

    SMB_META_RX51,
    SMB_META_RX52,
    SMB_META_RX53,
    SMB_META_RX54,
    SMB_META_RX55,
    SMB_META_RX56,
    SMB_META_RX57,
    SMB_META_RX58,
    SMB_META_RX59,
    SMB_META_RX60,

    SMB_META_RX61,
    SMB_META_RX62,
    SMB_META_RX63,
    SMB_META_RX64,


} SMB_METACODE;

typedef struct
{
    unsigned char          m_BeUsed;							//该命令是否被使用
    unsigned int m_WriteHeader;					//I2C写入口
    unsigned int m_COMM[2];						//写内容
    unsigned int m_ReadAddr;						//读入口
    unsigned char CtrlByteNum;						//控制字节数
} ST_COMMHEADER;
typedef struct
{

    ST_COMMHEADER 	m_HeaderBuf;
    unsigned char 	m_SignalID;                   //Smb信号编号
    unsigned char 	m_SMB_Meta;       //SMB变量值
    unsigned char 	m_Enable;                     //该信号是否有效
    unsigned char 	m_Datalog;                    //是否需要log数据
    unsigned char 	m_DataType;                   //数据类型
    unsigned char  					m_Sign;  // +/-	      符号
// 	unsigned char 	m_Code;
// 	unsigned char 	m_Access;
// 	unsigned char 	m_Initial;
// 	unsigned char 	m_unit;
    unsigned char   m_DataLen;                     //数据长度
    unsigned char   m_Endian;                      //大小端序
    unsigned int  	m_StartDataIndex;              //开始数据字节数
    unsigned int  	m_EndDataIndex;                //数据结束字节数
    unsigned char  	m_StartByteIndex;              //开始数据字节数  add by zc 08.10.2018
    unsigned char  	m_EndByteIndex;                //数据结束字节数  add by zc 08.10.2018
    unsigned char  	m_StartBitIndex;             	 //开始数据位数 	 add by zc 08.10.2018
    unsigned char  	m_EndBitIndex;               	 //数据结束位数	   add by zc 08.10.2018
    float 					m_Gain;                //增益
    float						m_Offset;          //便宜
    float  					m_Refresh;	                   //是否允许刷新
    unsigned char 	m_PECEnable;                   //是否支持PEC
    unsigned int  	m_Logging_Interval;              //SMBus打点速度 	add by zc 03.04.2019

} ST_SMB_BMS_SIGNAL ; //28 byte
typedef struct
{
    unsigned int        m_BaudRate;        //     SMB波特率 //char zyx 1908151
    unsigned char       m_ucReserved;       //    zyx 1908152
    unsigned char        m_FrameType;	    //	   SMB字符类型(WORD or String)
    unsigned char        m_Endian;			//	   端序
    unsigned char        m_ItemCount;       //信号数量

    //unsigned int         m_BrdCst_ID;
    //unsigned int         m_BrdCst_interval;
    unsigned int         m_UnsafeTimeout;			//ADDED BY ZC 12012017
    unsigned int         m_Reqst_MsgID;      //无用
    unsigned int         m_Ans_MsgID;        //无用
    unsigned int  	m_Refresh_Fast;    //FAST用户设定记录速度 	add by zc 03.04.2019
    unsigned int  	m_Refresh_Slow;    //SLOW用户设定记录速度 	add by zc 03.04.2019

} ST_SMB_GlobalCfg;
typedef struct
{
    unsigned char m_Enable;
    unsigned char m_Smb_OptionAddress;
    unsigned char m_Smb_SBSCommand;
    unsigned char m_Smb_BlockByteLen;
    unsigned char m_BlockByte[128];
} ST_SMB_BLOCK_BUF;
typedef struct
{
    ST_SMB_GlobalCfg     m_GlobalCfg;           //全局配置信息
    ST_SMB_BMS_SIGNAL    m_SMB_Signals[MAX_SMB_BMS_SIGNAL];  //max SMB bms signal256
//	ST_SMB_BLOCK_BUF     m_SMB_BlockBuf;
} ST_SMBBMS_CfgUnit;   // *SMB Number (332)  6*4 + 28*itemcount

typedef struct
{
    unsigned long int  m_IVChnIdx;              //对应的通道编号
    unsigned char      m_ItemCount[MAXCHANNELNO];
    ST_SMBBMS_CfgUnit  m_BMS_Chn;
} ST_SMBBMS_CONFIG;
typedef struct
{
    unsigned int      m_AllChnNum;
    struct
    {
        unsigned char       m_IVChnNo;
        unsigned char 		            m_bIVActive;       // this SMB bms channel is disable?
        unsigned char       m_AuxUnitID;        //source SMBID,transfer aux unit
        unsigned char                m_SMBBMSChnIdx;
    } m_SMBmap[MAXCHANNELNO];
} ST_SMBBMS_MAP;                                     //该部分后续扩展，暂时不用

typedef union
{
    float m_fValue[4];
    char m_SValue[64];
} UN_SMBDATA;

typedef struct
{
    unsigned char                m_bError; //m_bNeedCalculate  					//该信号无效
    unsigned char                m_bNeedRecal;       //这个变量需要重新计算了
    unsigned char                m_bNeedLog;         //是否上传
    unsigned char m_type;                            //数据类型
    unsigned char m_Len;                               //数据长度
    UN_SMBDATA 		m_Data;                          //数据
} ST_SMB_MetaValue;


typedef struct
{
    unsigned char   	m_IVChnNo;
    unsigned char   	m_IVUnitID;
    unsigned short    m_DataCnt:15;  //Count of field:m_Meta[];
    unsigned short		m_IsLastPoint:1;

    struct SmbFloatMeta
    {
        MsTime          m_DateTime;
        unsigned char     m_bError:1;
        unsigned char     m_bError_Offlinebit:1;
        unsigned char     m_bError_rvd:6;
        unsigned short  m_bLogData;
        unsigned short  m_MetaCode;
        float           m_fData;
    } m_Meta[MAX_SMB_BMS_SIGNAL];
} ST_SMB_RptData_Float;
typedef struct
{
    unsigned char   	m_IVChnNo;
    unsigned char   	m_IVUnitID;
    MsTime              m_DateTime;
    unsigned char     m_bError:1;
    unsigned char     m_bError_Offlinebit:1;
    unsigned char			m_IsLastPoint:1;
    unsigned char     m_bError_rvd:5;
    unsigned short      m_bLogData;
    unsigned short      m_MetaCode;
    unsigned short      m_DataLength;  //Count of field:m_sData[];
    unsigned char       m_sData[MAX_SMB_SIGNAL_STRING];  //string??SMB??????512???,???1?
} ST_SMB_RptData_String;


typedef struct
{
    DWORD              		    m_dwCmd;
    DWORD             		    m_dwCmd_Extend;
    BYTE               		    m_btCheckSum[2];
    BYTE               		    m_btNone_11[2];
    ST_MultPack        	        m_stMPack;
    ST_SMB_RptData_Float   	    m_SMBData;
} MP_REPORT_SMB_Data_Float;                //?????  SMB?????
typedef struct
{
    DWORD              		    m_dwCmd;
    DWORD             		    m_dwCmd_Extend;
    BYTE               		    m_btCheckSum[2];
    BYTE               		    m_btNone_11[2];
    ST_MultPack        	        m_stMPack;
    ST_SMB_RptData_String   	m_SMBData;
} MP_REPORT_SMB_Data_String;              //?????   SMB??????


// typedef struct
// {
// 	unsigned char   m_IVChnNo;
// 	unsigned char   m_IVUnitID;
// 	unsigned short  m_Us100;
// 	unsigned long   m_Second;
// 	unsigned short    m_DataCnt;
// 	unsigned short    m_DataType;
// 	struct
// 	{
// 		unsigned short 						m_bLogData;
// 		unsigned short 						m_MetaCode;
// 		ST_SMB_MetaValue         	m_Value;
// 	}m_Meta[MAX_SMB_SIGNAL_FLOAT];

// }ST_SMBBMS_RptData;									//SMB 上报数据格式

// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_CHANNEL_BOARD
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	BYTE               m_btCheckSum[2];                     // 结构共计:56+1542字节
// 	BYTE               m_btNone_11[2];                      // 预留reserved
// 	ST_MultPack        m_stMPack;                           // （多包信息管理)  44
// 	ST_SMBBMS_RptData  m_SMBData;                           //  12+6*255 = 1542
// } MP_REPORT_SMBBMS_Data;
//命令联合体定义

//命令字结构体定义
// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	St_SMB_SendMsg     m_SMBMsg;
// 	BYTE               m_IVChnNo;
// 	BYTE               m_btNone_12[1];                      // 预留reserved
// 	WORD                m_u16Token;
// 	DWORD              m_dwNone_41;                         // 预留reserved
// 	BYTE               m_btCheckSum[2];                     // 结构共计:24字节
// } MP_SET_SMBMSG;

// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_SET_SYSTEMTIME_FEEDBACK
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
// 	DWORD              m_dwCopyCmd;                         // 原命令备份执行命令码字
// 	DWORD              m_dwCopyCmd_Extend;                  // 原命令备份扩展命令码字
// 	WORD               m_wResult;                           // 0:Failed1:Success
// 	BYTE               m_dwNone_22[2];                         // 预留reserved
// 	BYTE               m_btCheckSum[2];                     // 结构共计:48字节
// } MP_SET_SMBMSG_FEEDBACK;

typedef struct
{
    unsigned short     m_u16ChannelIdx;
    unsigned char      m_ucMD5Count;
    unsigned char      m_ucReserved;
    St_MD5Assign       m_MD5Assign;// 结构共计:324字节
} St_AssignSMB;
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack        m_stMPack;                           // （多包信息管理)
    St_AssignSMB       m_st_AssignSMB;
    BYTE               m_btCheckSum[2];                     // 结构共计:58+20*N=378字节
} MP_SMBCONFIG_ASSIGN;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN_FEEDBACK
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
    BYTE               m_btNone_11[4];                      // 预留reserved
    St_AssignSMB       m_st_Assign;
    BYTE               m_btCheckSum[2];                     // 结构共计:38+20*N=358字节
} MP_ASSIGN_SMB_FEEDBACK;

// typedef  struct
// {
// 	unsigned char be_Enable;
// 	unsigned char m_BaudRate;
// 	unsigned int  m_DelayTime;
// 	unsigned char m_ConfigMsgNumber;
// 	unsigned char m_SignalNumber;
// 	unsigned char m_ItemCount;
//
// }ST_SMB_GloabalCfg;

// typedef struct
// {
// 	unsigned char       m_IVChnNo;
// 	int                 m_cfgBufIdx;
// 	int                 m_insertPos;
// 	St_MD5              m_MD5;
// }St_SMBConfigAssignTab;

// typedef struct
// {
// 	bool          m_BeUsed;
// 	unsigned char m_WriteHeader;
// 	unsigned char m_COMM[2];
// 	unsigned char m_ReadAddr;
// 	unsigned char CtrlByteNum;
// }ST_COMMHEADER;
// typedef struct
// {
// 	ST_COMMHEADER 	m_HeaderBuf;
// 	unsigned char 	m_SignalID;
// 	SMARTBATTERY_FUNCTION_CODE 	m_SMB_Meta;
// 	unsigned char 	m_Enable;
// 	unsigned char 	m_Datalog;
// 	unsigned char 	m_DataType;
//   bool   					m_Sign;  // +/-
// // 	unsigned char 	m_Code;
// // 	unsigned char 	m_Access;
// // 	unsigned char 	m_Initial;
// // 	unsigned char 	m_unit;
// 	unsigned char   m_DataLen;
// 	unsigned char   m_Endian;
// 	unsigned char  	m_StartDataIndex;
// 	unsigned char  	m_EndDataIndex;
//   float 					m_Gain;
// 	float						m_Offset;
// 	unsigned char  	m_Refresh;
// 	unsigned char 	m_PECEnable;
// }ST_SMB_SIGNALCFG;


// typedef  struct
// {
// 	St_MD5                m_MD5;
// 	ST_SMB_GloabalCfg     m_GlobalCfg;
// 	ST_SMB_SIGNALCFG     	m_SMB_Signals[MAX_SMB_SIGNAL];
// }ST_SMB_CfgUnit;

// typedef struct
// {
// 	unsigned char      	m_bufCnt;
// 	ST_SMB_CfgUnit   		m_buf[MAX_LEN_SMB_CONFIG];
// }St_SMB_ConfigBuf;
// typedef union
// {
// float m_fValue[4];
// char m_SValue[128];
// }UN_SMBDATA;

// typedef struct
// {
// 	unsigned char                m_bNeedCalculate;   //这个变量需要经过计算才能使用
// 	unsigned char                m_bNeedRecal;       //这个变量需要重新计算了
// 	unsigned char                m_bNeedLog;
// 	unsigned char m_type;
//   unsigned char m_Len;
//   UN_SMBDATA 		m_Data;
// } ST_SMB_MetaValue;

// typedef struct
// {
// 	unsigned char   m_IVChnNo;
// 	unsigned char   m_IVUnitID;
// 	unsigned short  m_Us100;
// 	unsigned long   m_Second;
// 	unsigned short    m_DataCnt;
// 	unsigned short    m_DataType;
// 	struct
// 	{
// 		unsigned short 						m_bLogData;
// 		unsigned short 						m_MetaCode;
// 		ST_SMB_MetaValue         	m_Value;
// 	}m_Meta[MAX_SMB_SIGNAL];

// }ST_SMB_RptData;
// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_GET_CHANNEL_BOARD
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	BYTE               m_btCheckSum[2];                     // 结构共计:56+1542字节
// 	BYTE               m_btNone_11[2];                      // 预留reserved
// 	ST_MultPack        m_stMPack;                           // （多包信息管理)  44
// 	ST_SMB_RptData  	 m_SMBData;                           //  12+6*255 = 1542
// } MP_REPORT_SMB_Data;




// typedef struct
// {
// 	unsigned short     m_u16ChannelIdx;
// 	unsigned char      m_ucMD5Count;
// 	unsigned char      m_ucReserved;
// 	St_MD5Assign       m_MD5Assign;// 结构共计:324字节
// } St_AssignSMB;
// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	ST_MultPack        m_stMPack;                           // （多包信息管理)
// 	St_AssignSMB       m_st_AssignSMB;
// 	BYTE               m_btCheckSum[2];                     // 结构共计:58+20*N=378字节
// } MP_SMBCONFIG_ASSIGN;
// typedef struct
// {
// 	DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_ASSIGN_FEEDBACK
// 	DWORD              m_dwCmd_Extend;                      // 扩展命令码字
// 	ST_MultPack_SecondField    m_stMSecondField;            // （多包信息管理第二区域)
// 	BYTE               m_btNone_11[4];                      // 预留reserved
// 	St_AssignSMB       m_st_Assign;
// 	BYTE               m_btCheckSum[2];                     // 结构共计:38+20*N=358字节
// } MP_ASSIGN_SMB_FEEDBACK;

//////////////////////////




typedef struct
{
    float m_High;
} St_IRange_BoardType;

typedef struct
{
    float m_High;
    float m_Low;
} St_VRange_BoardType;


typedef struct
{
    unsigned short m_u16BoardType;
    BYTE m_btBoardTypeVersion;
    BYTE m_PowerLevel;                      // reserved
    unsigned short m_u16BoardChCount;
    BYTE m_btDisCharge;
    BYTE m_btBUILTIN;                      // reserved
    BYTE m_btNone_23;                   // reserved [2] 20170428
    BYTE b_IsMapIRangeVir2Phy;
    BYTE m_ucIRangeCount;
    BYTE m_ucVRangeCount;
    St_IRange_BoardType m_IRange[NUM_OF_IV_RANGE_I];
    St_VRange_BoardType m_VRange[NUM_OF_IV_RANGE_V];
    unsigned long m_Rvd1;
    unsigned char m_Rvd[3];
    unsigned char m_CheckSum;  //m_CheckSum + m_u16BoardType + m_IRangeVirtual2PhysicalMap == 0xFF
    unsigned char m_IRangeVirtual2PhysicalMap[NUM_OF_IV_RANGE_I];  //170320
} IV_BOARDTYPE_CFG;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;                      //
    IV_BOARDTYPE_CFG   m_st_BoadType;
    BYTE               m_btNone_11[2];                      // reserved
    BYTE               m_btCheckSum[2];                     // 结构共? 8+60+4 = 72??

} MP_SET_IV_BOARDTYPE;

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    float              m_fSecondaryVoltage;
    float              m_fChannelVoltage;
    float              m_fDCVoltageOffset;
    BYTE               m_btDspBoardType;
    BYTE               m_btResult;
    BYTE               m_btReadSuccess;
    BYTE               m_btDspID;
    BYTE               m_btSw_Freq;
    BYTE               m_btHZ_XMFR;
    BYTE               m_btGrid_Voltage;
    BYTE               m_btDSPBoardType;
    WORD               m_wDSP_Version:14;//YYXX 1901,19年第1版
    WORD               m_wDSP_TYorZY:2;//00:TY,01:ZY
    BYTE               m_btCheckSum[2];
} MP_SET_DSP_BASIC_INFO; //32字节

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    float              m_fV_KP;
    float              m_fV_KI;
    float              m_fI_KP;
    float              m_fI_KI;
    BYTE               m_btResult;                       // 0:Failed1:Success
    BYTE               m_btReadSuccess;
    BYTE               m_btDspID;
    BYTE               m_btNone_11[3];
    BYTE               m_btCheckSum[2];
} MP_SET_DSP_ADJUST_INFO; //32字节

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    BYTE               m_btDspID;
    BYTE               m_btNone_11[1];
    BYTE               m_btCheckSum[2];
} MP_READ_DSP_BASIC_INFO; //12字节

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    BYTE               m_btDspID;
    BYTE               m_btNone_11[1];
    BYTE               m_btCheckSum[2];
} MP_READ_DSP_ADJUST_INFO;	//12字节

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    BYTE               m_DSP_Indentity;
    BYTE               m_DSP_SlaveCountOrMasterID;
    BYTE               m_DSP_SlaveID[8];
    BYTE               m_btResult;                       // 0:Failed1:Success
    BYTE               m_btReadSuccess;
    BYTE               m_btDspID;
    BYTE               m_btNone_11;
    BYTE               m_btCheckSum[2];
} MP_SET_DSP_INDENTITY_INFO; //24字节
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_SET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;
    BYTE               m_btDspID;
    BYTE               m_btNone_11[1];
    BYTE               m_btCheckSum[2];
} MP_READ_DSP_INDENTITY_INFO;	//12字节

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码= CMD_GET_ IV_ BOARDTYPE
    DWORD              m_dwCmd_Extend;                      //
    BYTE               m_btNone_11[2];                      // reserved
    BYTE               m_btCheckSum[2];                     // : 8+4 = 12
} MP_GET_IV_BOARDTYPE;

typedef struct
{
    DWORD m_dwCmd;  // 执行命令码= CMD_REPORT_GET_CHANNEL_BOARDTYPE
    DWORD m_dwCmd_Extend;  //
    BYTE m_btCheckSum[2];
    BYTE m_btNone_11[2];
    IV_BOARDTYPE_CFG  m_st_BoadType;
} MP_REPORT_GET_IV_BOARDTYPE; // :8+4+60?? = 72

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_LOGDATA_CYCLE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    WORD    m_uIvUnitIP;
    WORD    m_uIvChanIndexInUnit;
    WORD		m_uChamberIndex;
    float   m_fDefaultTemperatureSetting;
    BYTE    m_btUseGroupTChamber;
    BYTE    m_bUseDefaultValue; //when Master Chn stop， use new master chn control value. All chn stop, use default value.
    BYTE    m_bIgnoreDefaultValue;//Ignore Default Value after all chn stop. Keep TChamber tempreature.
    BYTE    m_btReserved[43];
    BYTE    m_btCheckSum[2];
} MP_MTCI_SET_GROUP_SETTING; //1910101 zyx
typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_SCHEDULE_REPORT_LOGDATA_CYCLE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:68+8*60字节
    WORD               m_u16Token;
    St_MD5             m_StMD5Code;
    unsigned long	   u32TestID;                            //mits8
    unsigned long      u32DataPoint;                          //mits8
    WORD               m_wChannelNumber;
    WORD               m_wStepID;
    DWORD              m_dwCycleID;
    MsTime_Pack        m_st_DataTime;
    ST_GETTIME_DATA    m_st_TestTime;                       // 8字节
    ST_GETTIME_DATA    m_st_StepTime;
    WORD               m_wVariable_Count;
    BYTE               m_btNone_12[6];                      // 预留reserved
    ST_SCHEDULE_VARIABLE    m_st_Variable[MAX_SEND_METAVARIABLE];// 8*60
} MP_REPORT_LOGDATA_SDL_CYCLE;      // 20170919 yy

typedef struct
{
    unsigned char        m_ucValid_StepCtrl:1; //以位表示变更有郊St_StepCtrl
    unsigned char        m_ucValid_Limit:7;  //以位表示变更Limit数量
    unsigned char        m_LimitsNum[MAX_UPDATA_LIMIT_NUMBER];      //变更Limit号列表最多m个

    St_StepCtrl          m_Ctrl;                    // 68字节
    St_Limit             m_Limits[MAX_UPDATA_LIMIT_NUMBER]; // m*34字节
} St_StepData_Onlineupdata;

typedef struct //St_Onlineupdata_Step
{
    St_MD5               m_OldID;
    St_MD5               m_NewID;
    unsigned char        m_ucValid_Stetting:1; //以位表示变更有郊St_ScheduleSetting
    unsigned char        m_ucValid_empty:7;

    unsigned char        m_ucStepCount;       //变更总步数
    unsigned char        m_ucStepNum[MAX_UPDATA_STEP_NUMBER];      //变更步号列表最多n步

    St_ScheduleSetting   m_Setting;
    St_StepData_Onlineupdata       m_Step_UpData[MAX_UPDATA_STEP_NUMBER];

} St_Onlineupdata_Step;

typedef struct //St_Sche_Onlineupdata
{
    unsigned long int uw32_Channelbit;  // 针对不同的Unit特殊处理
    St_Onlineupdata_Step  st_updata;
} St_Sche_Onlineupdata;

typedef struct
{
    unsigned char           m_CAN_Port;//报文广播端口
    unsigned char           m_FrameType;//标准帧还是扩展帧
    unsigned int            m_U32ID;//报文ID
    unsigned int            m_Period;//报文广播周期
    unsigned char           m_Endian;   //大小端结构	0：小端，1：大端
    unsigned char           m_DLC;  //报文数据长度
    unsigned char           m_Aux_Type1;//数据1辅助类型
    unsigned char           m_Channel_Index1;//数据1通道号（在该unit下该辅助类型的通道号）
    unsigned char           m_Aux_Type2; //数据2辅助类型
    unsigned char           m_Channel_Index2; //数据2通道号（在该unit下该辅助类型的通道号）
} St_CANAUX_Broadcast_MSG; //16字节

typedef struct
{
// 	DWORD                          m_dwCmd;
// 	DWORD                          m_dwCmd_Extend;
    BYTE                           m_Total_Message;    //该unit下广播报文总数
    BYTE                           m_btNone_12[3]; // 预留reserved
    St_CANAUX_Broadcast_MSG        m_AuxBroadcast_Message[48];//48条报文信息 16*48字节

    BYTE                           m_CAN2FrontBaudRate;//波特率，只辅助unit用，IV和CANBMS板不用。
    BYTE                           m_CAN1RearBaudRate;//波特率，只辅助unit用，IV和CANBMS板不用。

    BYTE                           m_btCheckSum[2];
} MP_SET_CANAUX_BROADCAST_CONFIG; //784字节


typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_BASIC_TABLE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:1428字节
    WORD               m_u16Token;
    unsigned char      ItemCount;
    St_Data_Pairs      Point[MAX_SDL_PAIRS_NUMBER];         // 48*29 + 13=1405
} MP_REPORT_SDL_PACK;  //mits8

typedef struct
{
    DWORD              m_dwCmd;                             // 执行命令码字=CMD_REPORT_BASIC_TABLE
    DWORD              m_dwCmd_Extend;                      // 扩展命令码字
    BYTE               m_btCheckSum[2];                     // 结构共计:1428字节
    WORD               m_u16Token;
    unsigned char      ItemCount;
    St_Data_Pairs_4BytesCycleIdx      Point[MAX_SDL_PAIRS_NUMBER];         // 48*29 + 13=1405
} MP_REPORT_SDL_4BYTESCYCLEIdx_PACK;  //mits8   //09.22.2021

typedef union
{
    MP_HEADER_NEW                            m_HeaderNew;
    MP_SET_SYSTEMTIME                        m_Set_Systemtime;
    MP_SINGLE_PACKDATA                       m_Set_Channel_Board;
    MP_GET_CHANNEL_BOARD                     m_Get_Channel_Board;
    //BMS
    MP_SINGLE_PACKDATA                       m_SetCANBMS_Map;
    MP_SINGLE_PACKDATA                       m_Set_CANSignal_Config;
    MP_CANCONFIG_ASSIGN                      m_CANConfig_Assign;
    MP_SET_CANMSG                            m_Set_CANMessage;
    MP_SINGLE_PACKDATA                       m_Set_CANBMSAuxMultiMsg; //170828 zyx
    //Smartbattery
    MP_SINGLE_PACKDATA                       m_Set_SMBSignal_Config;
    MP_SMBCONFIG_ASSIGN                      m_SMBConfig_Assign;
    MP_SINGLE_PACKDATA                       m_Set_Channel_PID;
    MP_SET_CHANNEL_CALIBRATE                 m_Set_Channel_Calibrate;
    MP_SETM0_CalibMode                       m_Set_M0_ActionSwitch;
    MP_M0_READCalibPara                      m_Read_M0_CalibPara;
    MP_INDEPENDENT_ACTIONSWITCH              m_Independent_ActionSwitch;
    MP_INDEPENDENT_ACTION                    m_Independent_Action;
    MP_INDEPENDENT_EMERGENCYSTOP             m_Independent_EmergencyStop;
    MP_INDEPENDENT_READDATAORSTATE           m_Independent_ReadDataOrState;
    MP_INDEPENDENT_READ_BOARD_VERSION		 m_Independent_ReadBoardVersion;
    MP_INDEPENDENT_READ_CFG                  m_Independant_Read_CFG;
    //	MP_SCHEDULE_SET_SETTING                  m_Schedule_Set_Setting;
    MP_SCHEDULE_GET_STATUS                   m_Schedule_Get_Status;
    MP_SINGLE_PACKDATA                       m_Schedule_Download;
    MP_SINGLE_PACKDATA                       m_Schedule_Online_Updata; // 09.11.2017
    // 	MP_SCHEDULE_START                        m_Schedule_Start;
    MP_SCHEDULE_STOP                         m_Schedule_Stop;
    MP_SCHEDULE_CONTINUE                     m_Schedule_Continue;
    MP_SCHEDULE_JUMP                         m_Schedule_Jump;
    MP_SCHEDULE_ASSIGN                       m_Schedule_Assign;
    MP_SIMULATION_DOWNLOAD                   m_Simulation_Download;  //12.22.2013ADD
    MP_BATTERY_SIMULATION_DOWNLOAD           m_Battery_Simulation_Download;   //190218 zyx
    MP_SIMULATION_ASSIGN					 m_Simulation_Assign;    //12.22.2013ADD
    MP_SCHEDULE_ACK_EVENT                    m_Schedule_Ack_Event;
    MP_RSN_ERROR_ACK_EVENT                   m_RSN_Error_Ack_Event;
    MP_BATCH_GROUP		m_Batch_Group;
    MP_SET_VOLTAGE_RANGE       m_Set_VoltageRang;
    MP_GET_IV_BOARDTYPE						 m_Get_IV_BoardType;
    MP_SET_IV_BOARDTYPE						 m_Set_IV_BoardType;
    MP_SENT_ACIMTEST_START         			 m_Start_ACIMTest;
    MP_SENT_ACIMTEST_STOP         			 m_Stop_ACIMTest;
    MP_SET_DSP_BASIC_INFO             m_SetDspBasicInfo;
    MP_SET_DSP_ADJUST_INFO            m_SetDspAdjustInfo;
    MP_SET_DSP_INDENTITY_INFO            m_SetDspIndentityInfo;
    MP_READ_DSP_BASIC_INFO            m_ReadDspBasicInfo;
    MP_READ_DSP_ADJUST_INFO           m_ReadDspAdjustInfo;
    MP_READ_DSP_INDENTITY_INFO           m_ReadDspIndentityInfo;
    MP_SINGLE_PACKDATA      m_CANAUX_Broadcast_Config_Set;//MP_SET_CANAUX_BROADCAST_CONFIG
    MP_SET_META_VARIABLE			 				m_SetMetaVariable;
    MP_UPDATE_META_VARIABLE_ADVANCEMODEL m_UpdateMetaVariableAdvMode;
    MP_MTCI_SET_GROUP_SETTING   m_MTCI_SetGroupSetting;//1910101 zyx
    MP_GET_DEBUG_DATA				m_GetDebugData;
		MP_GET_META_VARIABLES   m_GetMetaVariables;
    	MP_SET_INTERVALTIME_LOGDATA m_SetIntervalTimeLogData;
} Un_PC_MESSAGE;

typedef union
{
    MP_SET_SYSTEMTIME_FEEDBACK               			m_Set_Systemtime_Feedback;
    MP_SET_CANMSG_FEEDBACK                   			m_Set_CANMsg_Feedback;
    MP_CONFIRM_FEEDBACK                      			m_Confirm_Feedback;
    MP_INDEPENDENT_READDATAORSTATE_FEEDBACK  			m_Independent_ReadDataOrState_Feedback;
    MP_INDEPENDENT_READ_CFG_FEEDBACK         			m_Independant_Read_CFG;
    MP_ASSIGN_FEEDBACK                       			m_Assign_Feedback;
    MP_SIMULATION_ASSIGN_FEEDBACK						m_Simulation_Assign_Feedback;    //12.22.2013ADD
    MP_INDEPENDENT_READ_BOARD_VERSION_FEEDBACK			m_Independent_ReadBoardVersion_Feedback;
    MP_M0_READDATAORSTATE_FEEDBACK                      m_M0_ReadData_Feedback;
    MP_M0_READCalibPara_FEEDBACK                        m_M0_ReadCalib_Feedback;
    MP_SET_DSP_BASIC_INFO                           m_Read_Dsp_Basic_Parameter_Feedback;
    MP_SET_DSP_ADJUST_INFO                          m_Read_Dsp_Adjust_Parameter_Feedback;
    MP_SET_DSP_INDENTITY_INFO                          m_Read_Dsp_Indentity_Parameter_Feedback;
    MP_GET_DEBUG_DATA_FEEDBACK							m_DebugFeedBack;
		MP_GET_META_VARIABLES_FEEDBACK					m_GetMetaVariables_Feedback;
} Un_MC_FEEDBACK_MESSAGE;

typedef union
{
    MP_CONFIRM_FEEDBACK                      m_Report_Confirm;
    MP_REPORT_EVENT                          m_Report_Event;
    MP_REPORT_LOGDATA                        m_Report_LogData;
    MP_REPORT_LOGDATA_TC_Counter             m_Report_LogData_TC_Counter;
    MP_REPORT_METAVARIABLESYNCH              m_Report_Meta;
    MP_REQUEST_SYSTEMTIME                    m_Request_SystemTime;
    MP_REPORT_EVENT_SDL                      m_Report_Event_SDL;
    MP_REPORT_LOGDATA_SDL                    m_Report_LogData_SDL_AUX;//mits8
    MP_REPORT_SDL_4BYTESCYCLEIdx_PACK        m_Report_LogData_SDL;  //mits8
    MP_REPORT_FRAM_EVENT                     m_Report_Write_Fram;
    MP_REPORT_TOPC_ERROR                     m_Report_ToPC_Error;
    MP_REPORT_GET_SCHSTATUS                  m_Report_GetSCH_Status;
    MP_REPORT_PULSE_LOGDATA                  m_Report_Pulse_LogData;
    MP_REPORT_BUILDINAUX_LOGDATA             m_Report_BuildInAux_LogData;
    MP_REPORT_SIMULATION_REQUESTDATA         m_Report_Simulation_RequestData;
    MP_REPORT_ACIM_DATA                      m_Report_Acim_Data;
    MP_REPORT_LOGDATA_SDL_CYCLE              m_Report_Event_SDL_Cycle;
    MP_REPORT_GRPLOGDATA_SDL					m_Report_GroupLogData_SDL_AUX;
} Un_MC_REPORT;


#endif
