#pragma once

#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../common_os/protocols/SF_Hwf.h"
typedef struct
{
    bool              m_NeedPowerAdjFlag;  // 0: 不需要启动功率调节,   1: 开始启用功率调节
    bool              m_bNeedACK;          //标识需要回复ACK     回复后清零
    unsigned int      m_ACK_Code;
    unsigned  char    m_SourceID;      // IV IP号，记录M4的源ID
    unsigned  char    m_PacketLostCnt;      //丢包计数器，检测M4是否在刷新数据，没有收到就认为丢包了
// 	 unsigned  char    m_Updataflag;    //数据刷新的标志，收到M4的刷新命令后，该标志位置1
    float             m_VppValue;            //正向功率电电压值
    float             m_VmmValue;      // 负向功率电电压值

} ST_PowerAdj_Para;
//ST_PowerAdj_Para m_PowerAdjData[40];
typedef struct
{
    float m_Vpp_Adc;
    float m_Vmm_Adc;
} ST_M4_Value;

enum
{
    POWER_IDLE=0,    //空闲
    POWER_START,			//启动
    POWER_WAIT,
		POWER_WAIT_FOR_REST,
		POWER_SLOW_START_ON,
		POWER_SET_DAC,
    POWER_RUN,       //稳定运行，保持ADC采样处理
    POWER_ERROR,       //关闭
};

enum
{
    PADJ_IDLE =0,
    PADJ_START,
    PADJ_RUN,
    PADJ_ACK,
	PADJ_CAL_VALUE,
	PADJ_OUTPUT,
    PADJ_ERROR,
    PADJ_CAN_LOST,
};
enum
{
    ADJ_NO_NEED =0,
    ADJ_START,
    ADJ_WAIT_REFRESH,
    ADJ_REFRESH,
    ADJ_WAIT,
    ADJ_CAL_VALUE,
    ADJ_OUTPUT,
    ADJ_CAN_LOST,
};
enum
{
    ADJ_NO_NEED_ACK =0,
    ADJ_START_ACK,
    ADJ_STOP_ACK,
};

enum
{
    ADJ_Vpp_CHAN =0,
    ADJ_Vmm_CHAN,
    ADJ_Cpp_CHAN,
    ADJ_Cmm_CHAN,
};
class POWERADJ
{
public:
// 	static UWord32       uw32_GotoBootMode;	 //必须放在第一位
// 	static unsigned char uc_NOP[60];  //   空出60字节空间外存
    static UWord16       uw16_LedTimer;
    static UInt16        Priority;
    static UInt16        TaskID;
    static float Vpp_Set;
    static float Vmm_Set;
    static float Old_Vpp_Set;
    static float Old_Vmm_Set;
static float Vpp_adjust;
static float Vmm_adjust;
//   static float Vpp_Adc;
//   static float Vmm_Adc;

//  static MsTime LastdelayTime;
    static unsigned char BoardType;
    static unsigned int uc_SourceID;
    static unsigned char m_DataSaveID[128];
    static unsigned char m_UnitCnt;

    static unsigned char AdjStatus;   // 调节控制状态
//   static unsigned char Vol_TraceFlag;
//   static unsigned char NeebACKflag;    // 0:不需要回复   1： 回复Start_ACK      2:Stop_ACK
    static unsigned char RefreshFlag;
//   static unsigned char PowerAdjErrorFlag ;
//   static unsigned char RestarACK_ok;
    static unsigned char HV_OK;
    static float VppSET_VOL_MAX;
    static float VppSET_VOL_MIN;
    static float VmmSET_VOL_MAX;
    static float VmmSET_VOL_MIN;
    static ST_PowerAdj_Para m_PowerAdjData[40];
    static ST_M4_Value  m_M4_Value;
//    static unsigned short  TimeTicks;
//    static unsigned char CAN_StartFlag;
// 	 static bool CAN_LostFlag;
// 	static unsigned char uc_TimeLED;
    static const St_Port Pin_SWITCHA0;
    static const St_Port Pin_SWITCHA1;
    static const St_Port Pin_SWITCHA2;
    static const St_Port Pin_SWITCHA3;
    static const St_Port Pin_SWITCHA4;
    static const St_Port Pin_SWITCHA5;
    static const St_Port Pin_SWITCHA6;
    static const St_Port Pin_SWITCHA7;
    static const St_Port CAN_LED;

    static const St_Port ALM_LED;
    static const St_Port HV_OK1;
    static const St_Port HV_OK2;
    static const St_Port SW_TR;
    static MsTime SendDataTime;
    static bool   SendFeedBackFlag;    // 0：不需要额外反馈电源电压，  1：需要在200ms后立即反馈电源电压
   static unsigned char  AdjustCnt;
   static unsigned char  TimeCnt;
// static const St_Port Pin_SWITCHA7;
private:

public:
    static void Init(void);
    static void DoWork(void);
    static unsigned char GetPowerID(void);
    static unsigned char GetPowerBoardType(void);
//   static void PowerCrlProcess200A(void);
    static void PowerAdjProcess(void);
    static void ReportData(void);

  static unsigned char CaculateAdjustValue(void);
    static void SendDataToIV(void);
  static void Output(void);
    static My_Func_Ptr_void1 PowerRunningState;
    static    My_Func_Ptr_Ext6    Func_Voltage_Dac_Out;
    static My_Func_Ptr_Void  IndependentSample;
};


