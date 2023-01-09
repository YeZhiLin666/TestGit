///////////////////////////////////////////////////////////////////////////////////
//File Name: AUXSampleProcess.h
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样任务处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

UInt16 AUX_SampleProcess::Priority = PRIORITY4;
UInt16 AUX_SampleProcess::TaskID = AUXSAMPLE_TASK;			// should be a definition which positioning this task in OS_Task[]

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned long AUX_SampleProcess::error_counter;
// bool          AUX_SampleProcess::b_AuxTempNeedFilterFlag;   //辅助温度是否要滤波的标志
MsTime        AUX_SampleProcess::m_AuxTempUpdataTime;
unsigned char AUX_SampleProcess::SampleErrorCount;
MsTime  AUX_SampleProcess::SampleErrorTime[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];
#pragma arm section //结束外存变量定义

//按照Aux_DataManager::m_AllAuxBoards
void AUX_SampleProcess::Init(void)
{
		SampleErrorCount = 10;
//     b_AuxTempNeedFilterFlag = true;
    //for (unsigned char i = 0;i<QUEUE_AUX_MAX_INDEX;i++)
    //{
    //	memset(&Aux_Data_History[i],0,sizeof(st_Aux_History));
    //}

}

//20131127  准备加入滤波
//滑动平均滤波，是比较适用的    20131128 chen
//把连续取N个采样值看成一个队列
//队列的长度固定为N
//每次采样到一个新数据放入队尾,并扔掉原来队首的一次数据.(先进先出原则)
//把队列中的N个数据进行算术平均运算,就可获得新的滤波结果
//N值的选取：流量，N=12；压力：N=4；液面，N=4~12；温度，N=1~4；电压，N=12；
void AUX_SampleProcess::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAMPLE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAMPLE);
#endif

    for(unsigned char data_group_idx=0; data_group_idx<MAX_DATA_GROUP; data_group_idx++)
    {
        if(!Aux_DataManager::m_DataGroupMgr[data_group_idx])    //no used
        {
            continue;
        }
        ProcessInChipTemperature (data_group_idx); //处理片内温度传感器的读数

        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {

            if(Aux_DataManager::m_AuxDataBank[data_group_idx][j].updated == true)
            {
                if(Aux_DataManager::m_AuxDataBank[data_group_idx][j].m_Chn_Info.m_AuxType == AUX_DO
                        || Aux_DataManager::m_AuxDataBank[data_group_idx][j].m_Chn_Info.m_AuxType == AUX_AO)
                    return;   // 20171116 yy
//===========add for cold plate by ZC 04.06.2018=====================
                if((Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_MTC_Control_Board_415132) 
									|| (Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_MZTC_Control_Board_417118)
									||(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_MZTC_Control_Board_441324)
								  ||(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_MZTC_Control_Board_444136))
                {
                    Aux_DataManager::m_AuxDataBank[data_group_idx][j].aux_data = Aux_DataManager::m_AuxDataBank[data_group_idx][j].aux_instant_code / 1000.0f;
//                    Aux_DataManager::m_AuxDataBank[data_group_idx][j].updated_aux_dxdt.m_Bits.m_Updata = true; // dxdt准备处理
                    Aux_DataManager::m_AuxDataBank[data_group_idx][j].value_update_time = CommonFunc::GetSystemTime();  //record time
									  MetaVariable_Task::Calculate_AuxDXDT(data_group_idx,j);
                    continue;
                }
//===================================================================

                //将原始数据放入历史que中，然后滤波，转换计算最后存入m_AuxDataBank的  aux_data
                ProcessSampledData(data_group_idx,j);
//                Aux_DataManager::m_AuxDataBank[data_group_idx][j].updated_aux_dxdt.m_Bits.m_Updata = true; // dxdt准备处理
                Aux_DataManager::m_AuxDataBank[data_group_idx][j].updated = false;    //clear
                Aux_DataManager::m_AuxDataBank[data_group_idx][j].value_update_time = CommonFunc::GetSystemTime();  //160429
				 MetaVariable_Task::Calculate_AuxDXDT(data_group_idx,j);
            }
        }

    }
//==================================add start by zc 01152018======================================
//	Life Cycle Chamber I2C temperature data -> m_AuxDataBank  ADD BY ZC 01152018
    if( (Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_TEC_Chamber_601932) && (Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_SlaveBoardType == 1) )
    {
        if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave)
        {
            Aux_DataManager::m_AuxDataBank[1][0].aux_data = Aux_Chamber_TEC_Base::Temperature; //上传I2C传感器采样温度数据
//             Aux_DataManager::m_AuxDataBank[1][0].updated_aux_dxdt.m_Bits.m_Updata = true; // dxdt准备处理
            Aux_DataManager::m_AuxDataBank[1][0].value_update_time = CommonFunc::GetSystemTime();  //record time
        }
        else
        {
            Aux_DataManager::m_AuxDataBank[0][8].aux_data = Aux_Chamber_TEC_Base::Temperature; //上传I2C传感器采样温度数据
//             Aux_DataManager::m_AuxDataBank[0][8].updated_aux_dxdt.m_Bits.m_Updata = true; // dxdt准备处理
            Aux_DataManager::m_AuxDataBank[0][8].value_update_time = CommonFunc::GetSystemTime();  //record time
        }
    }
//===================================add end by zc 01152018=======================================

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAMPLE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_SAMPLE);
#endif
}

void AUX_SampleProcess::EndTask(void)
{

}

//处理采样数据
//并做滤波处理
void AUX_SampleProcess::ProcessSampledData(unsigned char data_group_idx,unsigned char chn)
{

    //首先将刚采样的值存入历史que; 放入的时候尽量利用历史que的深度，以便充分利用
    //根据该板的该ADC的滤波需要计算平均值，平均的时候根据需要取不同的个数的历史值。
    //根据辅助类型转换成物理值
    //将转换成的物理值存入数据银行中
    enQueue( data_group_idx, chn);
    Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code = avgQueue(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_avg_num);
    Convert_Chn(data_group_idx,chn);




}

void AUX_SampleProcess::ProcessInChipTemperature (unsigned char data_group_idx)
{
    enQueue_InChipTemp(data_group_idx);
    //   Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_avg_code=avgQueue_InChipTemp(data_group_idx,5);
    Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_avg_code=Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code;
    Convert_InChipTemp(data_group_idx);

}

//20131202
void AUX_SampleProcess::initQueue(unsigned char index,unsigned char n)    //初始化此队列
{

}

void AUX_SampleProcess::enQueue_InChipTemp( unsigned char data_group_idx)
{
    Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.ul_data[Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start] = Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code;

    Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start ++;
    Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length ++;
    if(Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start >= MAX_ADC_CODE_QUE)
    {
        Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start = 0;
    }
    if(Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length >=MAX_ADC_CODE_QUE)
    {
        Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length = MAX_ADC_CODE_QUE;
    }
}

unsigned long AUX_SampleProcess::avgQueue_InChipTemp(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth)

{
    unsigned long long sum = 0;
    unsigned char counter = 0;
    unsigned char pointer = 0;
    if (filter_depth <=1) //不需滤波，直接返回当前码值
        return Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code ;
    else
    {
        if (Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length == 0 )
            return 0;
        if (filter_depth >Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length)
            counter = Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length;
        else
            counter = filter_depth;

        for (unsigned char i = 0; i<counter; i++)
        {
            if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start>=counter)
                pointer= (Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start - i-1) ;
            else
            {
                if(i<Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start)
                    pointer =i;
                else
                    pointer = MAX_ADC_CODE_QUE+Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start-i-1;
            }
            // 				pointer = (Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start-i-1) ;
            // 				 if (pointer >MAX_ADC_CODE_QUE)
            // 						pointer +=MAX_ADC_CODE_QUE;

            sum+=Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.ul_data[pointer]; //累加最近的几个数据
        }

        return sum/counter;
    }
}

void AUX_SampleProcess::Convert_InChipTemp(unsigned char data_group_idx) //这个是针对 LTC2498
{
    unsigned long code;
    code = Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_avg_code;  //获得平均码值

    code -= 0x800000;

    float pv = 0;

    pv =(float) (code*4.88f/1570.0f) - 273.0f;      //4.88是参考电压 V_Ref
    Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_data = pv;  //片内温度
    // return pv;
}

void AUX_SampleProcess::enQueue( unsigned char data_group_idx, unsigned char chn)
{
    Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.ul_data[Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start] = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_instant_code;

    Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start ++;
    Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_length ++;
    if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start >= MAX_ADC_CODE_QUE)
        // 	if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start >=Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_avg_num)
    {
        Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start = 0;
    }
    if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_length >=MAX_ADC_CODE_QUE)
    {
        Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_length = MAX_ADC_CODE_QUE;
    }
}



unsigned long AUX_SampleProcess::avgQueue(unsigned char data_group_id, unsigned char chn, unsigned char filter_depth)

{
    unsigned long long sum = 0;
    unsigned char counter = 0;
    unsigned char pointer = 0;
    if (filter_depth <=1) //不需滤波，直接返回当前码值
        return Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_instant_code ;
    else
    {
        if (Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_length == 0 )
            return 0;
        if (filter_depth >Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_length)
            counter = Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_length;
        else
            counter = filter_depth;

        for (char i = 0; i<counter; i++)
        {
            if(Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start>=counter)
                pointer= (Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start - i-1) ;
            else
            {
                if(i<Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start)
                    pointer =i;
                else
                    pointer = MAX_ADC_CODE_QUE+Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start-i-1;
            }
            // 			   pointer = (Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start - i-1) ;
            // 				 if (pointer >MAX_ADC_CODE_QUE)	// pointer是unsigned char型，当i>uc_start时，pointer会变成非常大的数，因此此处判断改为pointer >MAX_ADC_CODE_QUE, Yifei
            // 						pointer +=MAX_ADC_CODE_QUE;

            sum+=Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.ul_data[pointer]; //累加最近的几个数据
        }

        return sum/counter;
    }
}

//
unsigned char AUX_SampleProcess::caliIndex(unsigned char Bid,unsigned char CHNid)
{
    unsigned char a1;
    a1 = Bid*17+CHNid;

    return a1;
}

float AUX_SampleProcess::Convert_Chn(unsigned char data_group_idx,unsigned char chn)
{
		float temp = 0;
	
    switch (Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_AuxType)
    {
    case AUX_VOLTAGE:		//"AuxV"   // 上位机分两层：第一层板类型 Type
        switch (Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_AuxSubType)
        {
			case Regular_2ndV :
			case HIM_2ndV:
			case Monitor_Vol:
			case LC_HIM_2ndV:
				float voltage;
				voltage = AUX_SampleProcess::Convert_Regular_2ndV(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = voltage;
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = voltage;
				break;
			case ADC_Power_Vol:
				float InternalVoltage;
				InternalVoltage = AUX_SampleProcess::Convert_Internal_Voltage(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = InternalVoltage;
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = InternalVoltage;
				break;

				// 						case HIM_2ndV :
				// 							    break;
			case BatteryPack_2ndV :
				float voltage1;
				voltage1 = AUX_SampleProcess::Convert_BatteryPack_2ndV(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = voltage1;
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = voltage1;

				break;


			};

			break;
		case AUX_CURRENT :
			float voltage;
			voltage = AUX_SampleProcess::Convert_Regular_Current(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = voltage;
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = voltage;
			break;
		case AUX_TEMPERATURE :                        // "T"
			switch (Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_AuxSubType)

			{
			case  T_ThermoCouple:
				temp=AUX_SampleProcess::Convert_ThermoCouple(T_ThermoCouple,data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;

			case  K_ThermoCouple:
				temp=AUX_SampleProcess::Convert_ThermoCouple(K_ThermoCouple,data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;
			case  J_ThermoCouple:
				temp=AUX_SampleProcess::Convert_ThermoCouple(J_ThermoCouple,data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;
			case  E_Thermocouple:
				temp=AUX_SampleProcess::Convert_ThermoCouple(E_Thermocouple,data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;
			case  TC_2ndV:
				float voltage;
				temp = AUX_SampleProcess::Convert_TC_2ndV(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;
			case Pt100_Thermistor:
				float ResValue;
				ResValue=AUX_SampleProcess::Convert_PT100_2ndV(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = ResValue;// ResValue;
	// 			temp = ConVert_PT100_Temp(ResValue);   //(ResValue-100.0f)/0.385f;    //(ResValue-96.92027f)/0.3573784f;
				temp = Pt100_Convert(ResValue);

				break;
			case Omega44006_Thermistor:
			case RTH44BS103F_Thermistor:
				float Voltage1;
				Voltage1=AUX_SampleProcess::Convert_Omega44006_Thermistor_Res(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = Voltage1;

				temp = ConVert_Omega44006_Thermistor_Temp(Voltage1,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_AuxSubType);
				if (temp > Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
					temp = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
				if (temp < Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
					temp = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				break;
			case T_Aux_Input:
				float adc_voltage_reading;							
				adc_voltage_reading=AUX_SampleProcess::Convert_ADC_CodeToVoltage(data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = adc_voltage_reading;// ResValue; 
				//Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data =AUX_SampleProcess::Convert_Voltage_ToTemperature(data_group_idx,chn,adc_voltage_reading);	
				temp = AUX_SampleProcess::Convert_Voltage_ToTemperature(data_group_idx,chn,adc_voltage_reading);
				break;
			default:
				break;

			}
// 			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_instant_data = temp;
			
// 		if(b_AuxTempNeedFilterFlag)
//         {
			if(abs(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data - temp) < 5) //170509 zyx
			{
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
				Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data_error_count = 0;
				CommonFunc::GetTimeTicksUpdate(&SampleErrorTime[data_group_idx][chn]);
			}
			else
			{
            Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data_error_count++;
            if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data_error_count > SampleErrorCount \
				|| CommonFunc::GetTimeTicks(&SampleErrorTime[data_group_idx][chn]) > 80000)	//采样更新间隔>8s
            {
                Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
                Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data_error_count = 0;
				CommonFunc::GetTimeTicksUpdate(&SampleErrorTime[data_group_idx][chn]);
            }
			}
// 		}
//         else
//         {
//             Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = temp;
//             MsTime Timediff=  CommonFunc::TimeDiff(m_AuxTempUpdataTime);
//             if(Timediff.Second>300)
//                 b_AuxTempNeedFilterFlag =1;
//         }

			break;
		case AUX_PRESSURE :                          //"P"
			switch (Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_AuxSubType)
			{
				case  Regular_Pressure:
					float voltage;
					voltage=AUX_SampleProcess::Convert_Pressure_2ndV(data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
					Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = voltage;// ResValue;
					Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data =AUX_SampleProcess::Convert_Voltage_ToPressure(data_group_idx,chn,voltage);
					break;
				
				case P_Aux_Input:
					float adc_voltage_reading;							
					adc_voltage_reading=AUX_SampleProcess::Convert_ADC_CodeToVoltage(data_group_idx,chn,Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
					Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = adc_voltage_reading;// ResValue; 
					Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data =AUX_SampleProcess::Convert_Voltage_ToPressure(data_group_idx,chn,adc_voltage_reading);	
					break;
				
				default:
					break;
			}
			break;
	// 		case AUX_PH	:		//"pH"
	// 			    break;
	// 		case AUX_FLOW_RATE :                          // "FR"
	// 			    break;
	// 		case AUX_DENSITY:                           //"Conc"
	// 			    break;
		case AUX_HUMIDITY :                          // "humidity" zyx
			break;

		case AUX_EXTERNALCHARGE:
			float Current;
			Current= AUX_SampleProcess::Convert_Regular_2ndV(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = Current;
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = Current;
			break;
		case AUX_ACIM:
			float  data;
			data = AUX_SampleProcess::ConVert_ACIM_Data(data_group_idx, chn, Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_code);
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = data;
			Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_data = data;
			if((chn == 2) && (Aux_ACIMTest::b_ACIM_DC_StartMea_Flag))
			{
				Aux_ACIMTest::ACIM_I_Sum +=data;
				Aux_ACIMTest::ACIM_I_Cnt++;
				if(Aux_ACIMTest::ACIM_I_Cnt>7)
				{
					Aux_ACIMTest::b_ACIM_DC_StartMea_Flag = false;
				}
			}
			break;
		default:
			break;
    };
		
		return temp;
}
float AUX_SampleProcess::Convert_ADC_CodeToVoltage(unsigned char data_group_idx, unsigned char chn, unsigned long code)
{
	float adc_gain,adc_offset;


	adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain; 

	adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset; 
	
	float pv = 0;
	unsigned long bound = 0x800000;
    if(code > bound)
    {
        pv = ((float)(code - bound)/bound) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
    }
    else
    {
        pv = ((float)(bound - code)/bound) * 4.88f * 0.5f;
        pv = -pv;
    }
	pv = (pv*adc_gain + adc_offset);
	return pv;
}

float AUX_SampleProcess::Convert_Regular_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain, phy_offset;


    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;

    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    float pv = 0;
    unsigned long bound = 0x800000;
    // 	float bound = 0x800000;
    if(code > bound)
    {
        pv = ((float)(code - bound)/bound) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
    }
    else
    {
        pv = ((float)(bound - code)/bound) * 4.88f * 0.5f;
        pv = -pv;
    }

    pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准



    return pv;

}
float AUX_SampleProcess::Convert_Internal_Voltage(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain, phy_offset;


    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;

    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    float pv = 0;
    unsigned long bound = 0x1000;
    // 	float bound = 0x800000;
    if(code > bound)
    {
        code = bound;
    }
    pv = ((float)code/bound) * 3.3f ;

    pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准
    return pv;

}
float AUX_SampleProcess::Convert_Regular_Current(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain, phy_offset;


    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;

    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
    float pv = 0;
    unsigned long bound = 0x800000;
    // 	float bound = 0x800000;
    if(code > bound)
    {
        pv = ((float)(code - bound)/bound) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
    }
    else
    {
        pv = ((float)(bound - code)/bound) * 4.88f * 0.5f;
        pv = -pv;
    }

    pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准

    return pv;

}

float AUX_SampleProcess::Convert_PT100_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain,phy_offset;

    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
    float pv = (code*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准

    if(pv > 0)  // zc 04.24.2018
        return pv;
    else
        return 0;
}

float AUX_SampleProcess::ConVert_PT100_Temp( float resvalue)
{
    float fTemp, fResValue=resvalue;

    if (fResValue < 100) //-80-0
    {
        fTemp =  (fResValue -100.069762)/0.3958965;
    }
    else if  ( (fResValue >= 100) && (fResValue < 138.51) )//0-100
    {
        fTemp =  (fResValue - 100.093352)/0.385099305;
    }
    else if  ( (fResValue >= 138.51) && (fResValue < 175.86) )//100-200
    {
        fTemp =   (fResValue - 101.246369)/0.37353048;
    }
    else if  ( (fResValue >= 175.86) && (fResValue < 212.05  ) )//200-300
    {
        fTemp = (fResValue - 103.559185)/0.361957717 ;
    }
    else if  ( (fResValue >= 212.05) && (fResValue < 247.09 ) )//300-400
    {
        fTemp = (fResValue - 107.033394)/0.350383797;
    }
    else if  ( (fResValue >= 247.09) && (fResValue < 280.98 ) ) //400-500
    {
        fTemp = (fResValue - 111.641868)/0.338861636;
    }
    else if  ( (fResValue >= 280.98) && (fResValue < 313.71 ) ) //500-600
    {
        fTemp =  (fResValue - 117.388312)/0.327362472;
    }
    else if  ( fResValue >= 313.71) //600-660
    {
        fTemp =  (fResValue -122.795219)/0.318240294;
    }
    //02.09.2012 modification end.

    return fTemp;

}
float AUX_SampleProcess::Pt100_Convert(float Res)    // Input resistance,  return temperature
{
    double A= 0;
    double B= 0;
    double C= 0;

    float temp=0, error=0;
    float Normal_0=0,Normal_1=0, Normal_2=0;
    A = 3.9083e-3;  // ITS-90 standard
    B = -5.775E-7;
    C= -4.183E-12;

    if (Res > 0) //!= 负值会导致运算异常 //1807131
    {
        Normal_0 = Res/100.0;  //Pt 100 standard resistance = 100 at 0 degree C
        temp = (-A + sqrt(A * A - 4 * B * (1 - Normal_0))) / (2 * B);
// 			      temp = A*(A-1);
// 			      temp /= (2*B);
// 			      temp -=::sqrt(1-Normal_0);


        if (Res >=100)
            return temp ;   // return positive temperature
        else
        {
            error = Normal_0- C*(temp-100)*(pow(temp,3));
            while (::abs(error)>1e-5)
            {
                Normal_1 = Normal_0 - C*(temp-100)*(pow(temp,3));
                temp = (-A + sqrt(A * A - 4 * B * (1 - Normal_1))) / (2 * B);
                Normal_2 = Normal_0 - C * (temp - 100) * pow(temp, 3);
                error = Normal_2 - Normal_1;
            }
            return temp;   // return negative temperature
        }


    }
    else
        return -273.15 ;  // if resistance is 0

}
float AUX_SampleProcess::Convert_Omega44006_Thermistor_Res(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain,phy_offset;

    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
    float pv = 0;
    unsigned long bound = 0x800000;
    // 	float bound = 0x800000;
    if(code > bound)
    {
        pv = ((float)(code - bound)/bound) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
    }
    else
    {
        pv = ((float)(bound - code)/bound) * 4.88f * 0.5f;
        pv = -pv;
    }
    float Voltage = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准

    return Voltage;
}
float AUX_SampleProcess::ConVert_Omega44006_Thermistor_Temp( float Voltage,unsigned char ResistanceType)
{
    if(Voltage>=-1.5)//断路
    {
        return -80;
    }
    if(Voltage<=-2.5)//短路
    {
        return 150;
    }
    float  resvalue = ((Voltage+2.5)*10000.0)/(1.0-Voltage-2.5);
    resvalue = ::log(resvalue);
		float dDegree = 0;
		if(ResistanceType == Omega44006_Thermistor)
		{
			dDegree =	(1.032e-3) + 
			(2.387e-4 * resvalue) + 
			(1.580e-7 * resvalue * resvalue * resvalue);
		}
		else if(ResistanceType == RTH44BS103F_Thermistor)
		{
			dDegree =	(1.126068758413e-3) + 
			(2.34572594529e-4 * resvalue) + 
			(8.6312248e-8 * resvalue * resvalue * resvalue);
    }

    float fTemp =  (float) ( (1.0 / dDegree) - 273.15 );
    return fTemp;
}
float AUX_SampleProcess::Convert_BatteryPack_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code)//待校准
{
    float adc_gain,adc_offset,phy_gain, phy_offset;

    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;

    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
    float pv = 0;
    float bound = 10000;

    pv = ((float)(code)) / bound;

    pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准

    return pv;
}


float AUX_SampleProcess::Convert_ThermoCouple(unsigned char TC_type, unsigned char data_group_idx,unsigned char chn, unsigned long code)
{
    float ref_voltage=0,phy_gain=1, phy_offset=0;
    float adc_gain=1,adc_offset=0;
    float pv =0;
    float result, temperature;
    phy_gain = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;


    Convert_InChipTemp(data_group_idx); //获取热电偶参考端（冷端温度）
    ref_voltage = Calculate_RefVoltage(TC_type, Aux_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_data);	 //换算出参考端电压
    //pv = Convert_Regular_2ndV(data_group_idx,chn,code); //将ADC码值转换成电压
    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    unsigned long bound = 0x800000;
    if(code > bound)
    {
        pv = (((float)(code - bound))/((float)(bound))) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
    }
    else
    {
        pv = (((float)(bound - code))/((float)(bound))) * 4.88f * 0.5f;
        pv = -pv;
    }
    pv *= 1000000.0f;    //将电压转换成uv
    pv = pv*adc_gain + adc_offset;  // Add by DKQ 03.26.2014   应该先校正测得的以微伏为单位的电压，然后在算温度
    Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_avg_voltage = pv;   // 保存压。以微伏为单位的电压值。
    pv += ref_voltage;  //  参考电压叠加

    switch (TC_type)
    {
    case T_ThermoCouple:
    {
        temperature = Aux_DataManager::T_t90_Convert_TType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0) > 0.0001 )
        {
            temperature = Aux_DataManager::T_t90_Convert_TType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case K_ThermoCouple:
    {
        temperature = Aux_DataManager:: T_t90_Convert_KType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0) > 0.0001 )
        {
            temperature = Aux_DataManager:: T_t90_Convert_KType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case J_ThermoCouple:
    {
        temperature = Aux_DataManager::T_t90_Convert_JType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0) > 0.0001 )
        {
            temperature = Aux_DataManager::T_t90_Convert_JType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case E_Thermocouple:
    {
        temperature = Aux_DataManager::T_t90_Convert_EType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0) > 0.0001 )
        {
            temperature = Aux_DataManager::T_t90_Convert_EType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    default:
        result = -273.0;
        break;

    };
    result = temperature*phy_gain + phy_offset;  //温度校准

    //检查数据合理性
    if (result > Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
        result = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    if (result < Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
        result = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;


    return result;


}

float AUX_SampleProcess::Calculate_RefVoltage(unsigned char TC_type,float Ref_Temp) //根据参考温度计算参考电压
{
    float result;
    switch (TC_type)
    {
    case T_ThermoCouple:
        result = (float)Ec0_Ttype+(float)Ec1_Ttype*Ref_Temp+(float)Ec2_Ttype* Ref_Temp*Ref_Temp+(float)Ec3_Ttype*Ref_Temp*Ref_Temp* Ref_Temp;    //把参考温度变成电压E，单位微伏
        // E = c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3;
        break;

    case K_ThermoCouple:
        result = (float)Ea0_Ktype*exp((float)Ea1_Ktype*( Ref_Temp-126.9686f)*( Ref_Temp-126.9686f)) +((float)Ec0_Ktype+(float)Ec1_Ktype*Ref_Temp+Ec2_Ktype* Ref_Temp*Ref_Temp+(float)Ec3_Ktype* Ref_Temp*Ref_Temp* Ref_Temp);
        //  E = Alpha0 * exp ( Alpha1 * ( t90 - 126.9686 ) ^ 2) + c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3;
        break;
    case J_ThermoCouple:
        result = (float)Ec0_Jtype+(float)Ec1_Jtype*Ref_Temp+(float)Ec2_Jtype* Ref_Temp*Ref_Temp+(float)Ec3_Jtype*Ref_Temp*Ref_Temp* Ref_Temp;    //把参考温度变成电压E，单位微伏
        // E = c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3;
        break;
    case E_Thermocouple:
        result = ((float)Ec0_Etype+(float)Ec1_Etype*Ref_Temp+Ec2_Etype* Ref_Temp*Ref_Temp+(float)Ec3_Etype* Ref_Temp*Ref_Temp* Ref_Temp);
        // E = c0 + c1 * t90 + c2 * t90^2+ c3 * t90^3;
        break;
    default:
        break;
    };
    return result;
}

float AUX_SampleProcess::Convert_TC_2ndV(unsigned char data_group_idx,unsigned char chn, unsigned long code)
{
    float temperature;
    // 	float adc_gain,adc_offset,phy_gain, phy_offset;

    // 	adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    // 	adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;


    float pv = 0;
    pv = ((float)(code)/16777216) * 4.88f ;  //此处为单边检测
    pv *= 1000.0f;    //将电压转换成mv
    temperature = (pv/10);
    if (temperature > Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
        temperature = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    if (temperature < Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
        temperature = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    return temperature;
}

float AUX_SampleProcess::Convert_Voltage_ToTemperature(unsigned char data_group_idx,unsigned char chn, float voltage)
{
	 float phy_gain, phy_offset, RangeL, RangeH;
	  float temp;
    phy_gain = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
	  phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
	  RangeL = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
	  RangeH = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    temp =  voltage* phy_gain + phy_offset;  
	
    if (temp > RangeH)
         temp = RangeH;  //RangeH;
    if (temp < RangeL)
         temp = RangeL; // RangeL;			
	
	  return temp;
}
float AUX_SampleProcess::Convert_Pressure_2ndV(unsigned char data_group_idx,unsigned char chn, unsigned long code)
{
    float adc_gain,adc_offset,phy_gain,phy_offset;

    adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
    // phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    // phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
 	//  float pv = 0;
 	//  unsigned long bound = 0x800000;
 	//  if(code > bound)
 	//  {
 	//	    pv = ((float)(code - bound)/bound) * 4.88f * 0.5f;  //为何是4.88V而不是5V？参考电压是4.88V?
 	//  }
 	//  else
 	//  {
 	//	    pv = ((float)(bound - code)/bound) * 4.88f * 0.5f;
 	//	    pv = -pv;
 	// }
    float pv = 0, temp=0;
    pv = ((float)(code)/16777216) * 4.88f ;  //此处为单边检测

    //  pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准
    pv = (pv*adc_gain + adc_offset);  //DKQ 10.07.2016  此处不需要物理校正

    return pv;

}
float AUX_SampleProcess::Convert_Voltage_ToPressure(unsigned char data_group_idx,unsigned char chn, float voltage)
{
    float phy_gain, phy_offset, RangeL, RangeH;
    float temp;
    phy_gain = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;
    RangeL = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    RangeH = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    temp =  voltage* phy_gain + phy_offset;   
    if (temp > RangeH)
        temp = RangeH;
    if (temp <RangeL)
        temp = RangeL;

    return temp;
}
float AUX_SampleProcess::ConVert_ACIM_Data(unsigned char data_group_idx, unsigned char chn, unsigned long code)
{
    float phy_gain,phy_offset;
    float data;

// 	  adc_gain  =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
// 	  adc_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    phy_gain =  Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    if(code > CONVERTER_MAX)
        code = CONVERTER_MAX;

    data = phy_gain *code  + phy_offset;

    return data;
}
// float AUX_SampleProcess::cali2ndT_TType(unsigned char Bid,unsigned char CHNid,unsigned long code)
// {
// 	float pv = 0;
// //	float pv_ref = AUX_Base::get_SampleEx(Bid,AUX_TCT_T);
// 	float pv_ref = AUX_Base::get_SampleEx(Bid,TC_T);

// 	float gain,offset;
// 	float temperature;  //Add by DKQ 03.26.2014 用于记录温度，避免多次用 pv

// 	unsigned short int uw16_AuxChennal = 0;
// 	for(unsigned char uc_i=0; uc_i < (Bid-1); uc_i ++)
// 	{
// // 		uw16_AuxChennal += Aux_DataManager::m_AuxUnitCFG.m_u8AuxChCountPerAuxType[uc_i];
//         uw16_AuxChennal += Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_ChannelCount;  // 扫出一字列表中的通道参数
// 	}
// 	uw16_AuxChennal += CHNid;

// 	//gain = 1; //温度不要gain了  comment out by DKQ 03.26.2014
// 	//这个值与赖工沟通，等他更新上位机，从config里aux compensation offset adjustment里下载  20131211
// 	gain  =  Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_gain;  // Add by DKQ 03.26.2014
// 	offset = Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_offset;    //chen 先直接填死  20131213

// 	float bound = 0x800000;
// 	if(code > bound)
// 	{
// 		pv = ((code - bound)/bound) * 4.88f * 0.5f;
// 	}
// 	else
// 	{
// 		pv = ((bound - code)/bound) * 4.88f * 0.5f;
// 		pv = -pv;
// 	}
// 	//首先计算出mv级别pv
//
// 	pv *= 1000000.0f;    //转换成uv
// 	pv = pv*gain + offset;  // Add by DKQ 03.26.2014   应该先校正测得的以微伏为单位的电压，然后在算温度
//     Aux_DataManager::m_AuxDataBank[Bid][CHNid].aux_pv_Valtage = pv;   // 保存压。以微伏为单位的电压值。
//     pv += pv_ref;  //  参考电压叠加
// 	//pv = AUX_Base:: T_t90_Convert_TType(pv+pv_ref);    //t90计算
// 	//temperature =AUX_Base:: T_t90_Convert_TType(pv+pv_ref);    //t90计算
//     temperature = AUX_Base:: T_t90_Convert_TType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
//     if(temperature != 22.0)
//     {
//         temperature = AUX_Base:: T_t90_Convert_TType(pv, COUNT_TEMPERATURE);    //t90计算
//         temperature += Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_extra;   // Add by DKQ 03.26.2014  这是做温度调整
//     }
//
// 	return temperature;
//   //return pv
// }

// //温度只有offset     20131206
// //先计算电压
// //再加入多项式计算
// //chen 20131126
// //校准处理  20131204
// float AUX_SampleProcess::cali2ndT_KType(unsigned char Bid,unsigned char CHNid,unsigned long code)
// {
// 	float pv = 0;
// //	float pv_ref = AUX_Base::get_SampleEx(Bid,AUX_TCT_K);
// 	float pv_ref = AUX_Base::get_SampleEx(Bid,TC_K);

// 	float gain,offset;
// 	float temperature;  //Add by DKQ 03.26.2014 用于记录温度，避免多次用 pv

// 	unsigned short int uw16_AuxChennal = 0;
// 	for(unsigned char uc_i=0; uc_i < (Bid-1); uc_i ++)
// 	{
// 		uw16_AuxChennal += Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_ChannelCount;
// 	}
// 	uw16_AuxChennal += CHNid;
//
// 	//gain = 1; //温度不要gain了  comment out by DKQ 03.26.2014
// 	//这个值与赖工沟通，等他更新上位机，从config里aux compensation offset adjustment里下载  20131211
//
// 	gain  =  Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_gain;  // Add by DKQ 03.26.2014

// 	offset = Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_offset;    //chen 先直接填死  20131213
//


// 	float bound = 0x800000;
// 	if(code > bound)
// 	{
// 		pv = ((code - bound)/bound) * 4.88f * 0.5f;
// 	}
// 	else
// 	{
// 		pv = ((bound - code)/bound) * 4.88f * 0.5f;
// 		pv = -pv;
// 	}
// 	//首先计算出mv级别pv
// 	pv *= 1000000.0f;    //转换成uv
// 	pv = pv*gain + offset; // Add by DKQ 03.26.2014   应该先校正测得的以微伏为单位的电压，然后再算温度
//     Aux_DataManager::m_AuxDataBank[Bid][CHNid].aux_pv_Valtage = pv;   // 保存以微伏为单位的电压值。
//     pv += pv_ref;  //  参考电压叠加
// 	//pv =AUX_Base:: T_t90_Convert_KType(pv+pv_ref);    //t90计算
//     //temperature = AUX_Base:: T_t90_Convert_KType(pv+pv_ref);    //t90计算
//     temperature = AUX_Base:: T_t90_Convert_KType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
//     if(temperature != 22.0)
//     {
//         temperature = AUX_Base:: T_t90_Convert_KType(pv, COUNT_TEMPERATURE);    //t90计算
//         temperature += Aux_DataManager::m_AuxSampleConfig[Bid][CHNid].f_extra;   // Add by DKQ 03.26.2014  这是做温度调整
//     }
//
//     return temperature;
// 	//return pv;
// }
