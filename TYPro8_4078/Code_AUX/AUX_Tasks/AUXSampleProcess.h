///////////////////////////////////////////////////////////////////////////////////
//File Name: AUXSampleProcess.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样任务处理头文件
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#ifndef _AUXSAMPLEPROCESS_
#define _AUXSAMPLEPROCESS_

//滑动平均滤波N系数
//为了去掉最大最小值，所有滤波系数+2
//#define QUEUE_AUX_MAX_N    32    //最大
#define QUEUE_AUX_MAX_INDEX 8*17L    //由二维整成直线
#define QUEUE_AUXTEMP_N      2      //4+2    //温度
//#define QUEUE_AUX2NDV_N    12+2  //电压
#define QUEUE_AUXPRESS_N    4+2   //压力

class AUX_SampleProcess
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
	static unsigned char SampleErrorCount;
	static MsTime SampleErrorTime[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];
    static void Init(void);
    static void DoWork(void);
    static void EndTask(void);

    static unsigned long error_counter;
// 	static bool      b_AuxTempNeedFilterFlag;   //辅助温度是否要滤波的标志
    static  MsTime   m_AuxTempUpdataTime;
    static void enQueue( unsigned char data_group_idx, unsigned char chn);
    static void enQueue_InChipTemp( unsigned char data_group_idx);

    static unsigned long avgQueue(unsigned char data_group_idx, unsigned char chn, unsigned char filter_depth);    //求平均
    static unsigned long avgQueue_InChipTemp(unsigned char data_group_idx,unsigned char chn,unsigned char filter_depth);    //求平均
    static float Convert_Chn(unsigned char data_group_idx,unsigned char chn);
    static void Convert_InChipTemp(unsigned char data_group_idx);



private:

    static void ProcessSampledData(unsigned char data_group_idx,unsigned char chn);
    static void ProcessInChipTemperature(unsigned char data_group_idx);

    static void initQueue(unsigned char index,unsigned char n);    //初始化此队列

    static unsigned char caliIndex(unsigned char Bid,unsigned char CHNid);    //计算索引


    //电压
	static float Convert_ADC_CodeToVoltage(unsigned char data_group_idx, unsigned char chn, unsigned long code);
    static float Convert_Regular_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code);
    static float Convert_Internal_Voltage(unsigned char data_group_idx,unsigned char chn,unsigned long code);
    static float Convert_BatteryPack_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code);	//add bu Yifei,2015.06.26
    static float Convert_PT100_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code);
    static float ConVert_PT100_Temp( float resvalue);
    static float Pt100_Convert(float Res);
    static float Convert_Regular_Current(unsigned char data_group_idx,unsigned char chn,unsigned long code);

    static float cali2ndV(unsigned char Bid,unsigned char CHNid,unsigned long code);

    //温度等计算
    static float Convert_ThermoCouple(unsigned char TC_type, unsigned char data_group_idx,unsigned char chn, unsigned long code);
    static float Calculate_RefVoltage(unsigned char TC_type,float Ref_Temp); //根据参考温度计算参考电压
    static float Convert_TC_2ndV(unsigned char data_group_idx,unsigned char chn, unsigned long code);
	static float Convert_Voltage_ToTemperature(unsigned char data_group_idx,unsigned char chn, float voltage);
    static float Convert_Pressure_2ndV(unsigned char data_group_idx,unsigned char chn, unsigned long code);
    static float Convert_Voltage_ToPressure(unsigned char data_group_idx,unsigned char chn, float voltage);
    static float Convert_Omega44006_Thermistor_Res(unsigned char data_group_idx,unsigned char chn,unsigned long code);
    static float ConVert_Omega44006_Thermistor_Temp( float Voltage,unsigned char ResistanceType);
    static float ConVert_ACIM_Data(unsigned char data_group_idx, unsigned char chn, unsigned long code);  //add by qjm 20170
    //   	static float cali2ndT_KType(unsigned char Bid,unsigned char CHNid,unsigned long code);
    // 		static float cali2ndT_TType(unsigned char Bid,unsigned char CHNid,unsigned long code);
    // 		static float cali2ndT_KTypeRef(unsigned char Bid,unsigned char CHNid,unsigned long code);
    // 		static float cali2ndT_TTypeRef(unsigned char Bid,unsigned char CHNid,unsigned long code);    //计算

};
#endif
