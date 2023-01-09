//改名为SampleProcess
//data acquisition
//updata 2012 11 14 by chen

#include "../Entry/includes.h"

UInt16 Sample_Pro::Priority = PRIORITY4;
UInt16 Sample_Pro::TaskID = SAMPLE_TASK;			// should be a definition which positioning this task in OS_Task[]

//函数指针
// int (* Sample_Pro::pSample_Pro_ProcessADC) (char adc_device_chn);
// char (* Sample_Pro::pSample_Pro_getCHN)(char adc_device_chn);
// unsigned long int (* Sample_Pro::pSample_Pro_getValue)(char adc_device_chn);
// char (*  Sample_Pro::pSample_Pro_getType)(char adc_device_chn);

// char Sample_Pro::Sample_Pro_Adc_device_num = 0;    //adc器件通道数

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char Sample_Pro::NewCodeSampled[MAXCHANNELNO];
unsigned char Sample_Pro::NewIRSampled[MAXCHANNELNO];
MsTime        Sample_Pro::SampleOverTime[MAXCHANNELNO];

unsigned char Sample_Pro::Digital_SampleCounter[MAXCHANNELNO][MAX_BUILDIN_DI_COUNT];  //公用COUNTER，能节省RAM消耗
MsTime Sample_Pro::BulitInSampleErrorTime[2][MAX_AUXCH_PER_DATA_GROUP];
#pragma arm section //结束外存变量定义

//----// K型温度板-----------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在0(°C)到1372(°C)
//从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
const float  f_CoEA_Ktype[9 ]= {0.0f,  2.5173462e-2,    -1.1662878e-6,  -1.0833638e-9,   -8.977354e-13,
                                -3.7342377e-16,  -8.6632643e-20, -1.0450598e-23,  -5.1920577e-28
                                };
//温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
const float  f_CoEB_Ktype[10] = {    0.0f,     2.508355e-2,    7.860106e-8,   -2.503131e-10,   8.31527e-14,
                                     -1.228034e-17,     9.804036e-22,   -4.413030e-26,    1.057734e-30,   -1.052755e-35
                                     };
//温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
const float  f_CoEC_Ktype[7] = {  -1.318058e2, 4.830222e-2,  -1.646031e-6, 5.464731e-11, -9.650715e-16,  8.802193e-21,  -3.110810e-26  };

//---------// K型温度板-----------------------------------------------------------------------------------------------------

//--------// T型温度板-----------------------------------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在0(°C)到400(°C)        T type thermocouple
//T_type热电偶从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
const float    f_CoEA_Ttype[8] = {   0.0,  2.5949192e-2, -2.1316967e-7,  7.9018692e-10, 4.2527777e-13,
                                     1.3304473e-16,  2.0241446e-20, 1.2668171e-24
                                 };
//温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
const float    f_CoEB_Ttype[7] = {   0.0,   2.5928e-2, -7.602961e-7,  4.637791e-11, -2.165394e-15,
                                     6.048144e-20,  -7.293422e-25
                                 };

//-------// T型温度板-----------------------------------------------------------------------------------------------------------

//----// J型温度板-----------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在-210(°C)到1200 (°C)
//从微伏电压转化为摄时温度定义常数系数
//温度范围在-210(°C)到0(°C) ，微伏电压范围在-8095(μV)到0(μV)
const float    f_CoEA_Jtype[9] = {   0.0f,  1.9528268e-2, -1.2286185e-6,  -1.0752178e-9, -5.9086933e-13,
                                     -1.7256713e-16,  -2.8131513e-20, -2.3963370e-24,  -8.3823321e-29
                                     };
//温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
const float    f_CoEB_Jtype[8] = {    0.0f,     1.978425e-2,    -2.001204e-7,   1.036969e-11,   -2.549687e-16,
                                      3.585153e-21,     -5.344285e-26,   5.099890e-31
                                 };
//温度范围在760(°C)到1200(°C) ，微伏电压范围在42929(μV)到69553(μV)
const float    f_CoEC_Jtype[6] = {  -3.11358187e3, 3.00543684e-1,  -9.94773230e-6, 1.70276630e-10, -1.43033468e-15,4.73886084e-21 };

//---------// J型温度板-----------------------------------------------------------------------------------------------------

//--------// E型温度板-----------------------------------------------------------------------------------------------------------
//从摄时温度转化为微伏电压定义常数系数
//温度范围在-200(°C)到1000(°C)        E type thermocouple
//T_type热电偶从微伏电压转化为摄时温度定义常数系数
//温度范围在-200(°C)到0(°C) ，微伏电压范围在-8825(μV)到0(μV)
const float    f_CoEA_Etype[9] = {   0.0,  1.6977288e-2, -4.3514970e-7,  -1.5859697e-10, -9.2502871e-14,
                                     -2.6084314e-17,  -4.1360199e-21, -3.4034030e-25, -1.1564890e-29
                                     };
//温度范围在0(°C)到1000(°C) ，微伏电压范围在0(μV)到76373(μV)
const float    f_CoEB_Etype[10] = {   0.0,   1.7057035e-2, -2.3301759e-7,  6.5435585e-12, -7.3562749e-17,
                                      -1.78960001e-21,  8.4036165e-26, -1.3735879e-30, 1.0629823e-35, -3.2447087e-41
                                      };

//初始化，必须在获得BoardType之后进行
void Sample_Pro::Init(void)
{
// 	switch(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
// 	{
// 	case BOARDTYPE_IBT:
// 		// 			pSample_Pro_ProcessADC = &(IBT_SampleManage::ProcessADC);    //IBT中的ADC处理函数
// 		// 			pSample_Pro_getCHN = &(IBT_SampleManage::getCHN);         //这三个返回参数可以合并成一个返回结构的方法
// 		// 			pSample_Pro_getValue = &(IBT_SampleManage::getValue);
// 		// 			pSample_Pro_getType = &(IBT_SampleManage::getType);
// 		// 			Sample_Pro_Adc_device_num = IBT_ADC_NUM;    //获得当前板类型的adc器件数
// 		break;
// 	case BOARDTYPE_TESLA_PWM:
// 		// 			pSample_Pro_ProcessADC = &(TESLA_SampleManage::ProcessADC);    //IBT中的ADC处理函数
// 		// 			pSample_Pro_getCHN = &(TESLA_SampleManage::getCHN);         //这三个返回参数可以合并成一个返回结构的方法
// 		// 			pSample_Pro_getValue = &(TESLA_SampleManage::getValue);
// 		// 			pSample_Pro_getType = &(TESLA_SampleManage::getType);
// 		// 			Sample_Pro_Adc_device_num = TESLA_ADC_NUM;    //获得当前板类型的adc器件数
// 		break;
// 	case BOARDTYPE_AUTO_CALI:
// 		// 			pSample_Pro_ProcessADC = &(Auto_Cali_SampleManage::ProcessADC);    //IBT中的ADC处理函数
// 		// 			pSample_Pro_getCHN = &(Auto_Cali_SampleManage::getCHN);         //这三个返回参数可以合并成一个返回结构的方法
// 		// 			pSample_Pro_getValue = &(Auto_Cali_SampleManage::getValue);
// 		// 			pSample_Pro_getType = &(Auto_Cali_SampleManage::getType);

// 		// 			Sample_Pro_Adc_device_num = 1;
// 		break;
// 	default:
// 		break;
// 	}
}

//主任务
//Sample_Pro是按照ADC器件来调用
//其实是最大合理化了硬件的使用频率，并没有考虑按照通道区分
//这里我们需要加入一个sample_ctrl_type采样类型  来决定是否滤波
//可以说是一个二次处理，我们称为之sample_Pro_Data;
void Sample_Pro::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAMPLE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_SAMPLE);
#endif

    char adc_device_chn;
    unsigned long int value = 0;
    char type = 0;

    for(adc_device_chn = 0; adc_device_chn < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; adc_device_chn++)  //遍历adc器件
    {
        if(NewCodeSampled[adc_device_chn])
        {
// 			value = ChannelInfo_Manager::ChannelInfo[adc_device_chn].m_u32CurrentAdc;
// 			type = SAMPLE_PROCESS_ADC_I;
// 			sample_Pro_Data(adc_device_chn,value,type);

// 			value = ChannelInfo_Manager::ChannelInfo[adc_device_chn].m_u32VoltageAdc;
// 			type = SAMPLE_PROCESS_ADC_V;
// 			sample_Pro_Data(adc_device_chn,value,type);

            unsigned long * uw32_Adc = &ChannelInfo_Manager::ChannelInfo[adc_device_chn].m_u32VoltageAdc;
            for(unsigned char uc_i=0; uc_i<2; uc_i++)
            {
                uw32_Adc +=uc_i;
                value = * uw32_Adc;
                type = SAMPLE_PROCESS_ADC_V + uc_i;
                sample_Pro_Data(adc_device_chn,value,type);
            }

            NewCodeSampled[adc_device_chn] = 0;
            Step_Manager_task::NewCodeRefreshed[adc_device_chn] = 1;
            SampleOverTime[adc_device_chn] = CommonFunc::GetSystemTime();
        }
        else
        {
            if(CommonFunc::GetTimeTicks(&SampleOverTime[adc_device_chn]) > 100000)
            {
                if(ChannelInfo_Manager::ChannelInfo[adc_device_chn].ScheduleStatus == RUNNING)
                {
                    ChannelInfo_Manager::ChannelInfo[adc_device_chn].ScheduleStatus = GOSTOP;
                    Schedule_Manager_task::SendFinishFlag[adc_device_chn].StopStatus = STOP_EMERGENCY;
                    Schedule_Manager_task::SendFinishFlag[adc_device_chn].SubStopCode = eEMERGENCYSTOP_ERROR_SampleOverTime;
                    SampleOverTime[adc_device_chn] = CommonFunc::GetSystemTime();
                }
            }

        }
    }

    if(board_config::Func_UnitDI_ReadAll != 0)
        (* board_config::Func_UnitDI_ReadAll)();

    if(board_config::Func_BuildInVolt_ReadAll != 0)
        (* board_config::Func_BuildInVolt_ReadAll)();

    if(OS_ServeProcess::BuiltIn_Flag)
        ProcessBuiltInData();
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAMPLE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_SAMPLE);
#endif

}


// void Sample_Pro::EndTask(void)
// {

// }

//updata 2012 12 14 chen
//对数据的二次处理，包括了滤波，gain offset的计算
//N的取值
//ChannelInfo_Manager::MetaValue; 和  int ChannelInfo_Manager::Read_SpecifiedMetavariable(unsigned char uc_ChannelNo,unsigned char uc_MetaCode)
//int ChannelInfo_Manager::Write_SpecifiedMetavariable(unsigned char uc_ChannelNo,unsigned char uc_MetaCode)
//用来读取数据

void Sample_Pro::sample_Pro_Data(char chn,unsigned long int value,char type)
{
    unsigned char Sample_Type = type;    //获取I V or others

// 	unsigned char sample_ctrl_type = SAMPLE_CTRL_IMMEDIATE;    //getSample_ctrl_type(chn);    //从大数据体内获得采样类型
// 	static unsigned char last_sample_ctrl_type = 0;    //默认为0

    unsigned char Range = 0;
    //float f_temp = 0;
// 	if(sample_ctrl_type != last_sample_ctrl_type)    //控制类型出现不同
// 	{
// 		//清除历史值与状态值
// 		//Init_HistroyData(chn);
// 		//counter_for_filter = 0;
// 		last_sample_ctrl_type = sample_ctrl_type;
// 	}


    //chen add
// 	chn = g_chn_order[chn];

// 	switch(sample_ctrl_type)    //作为不同的处理
// 	{
// 	case SAMPLE_CTRL_IMMEDIATE:    //立即数处理
    //转换成单极码字
    //转换成浮点数
    //gain offset
    //存入PV
    switch(Sample_Type)    //I or V or others
    {
    case SAMPLE_PROCESS_ADC_I:
//#if _BOARD_TYPE_  == BOARDTYPE_IBT
//#if _IBT_VERSION_ == IBT_VER_C
//         if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_IBT)
// 		{
//            if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_C)
// 		   {
// 			///////chen comment out 20130730
// 			    if(chn >7)
// 			    {
// 				    unsigned char temp = chn - 8;
// 				    chn = 15 - temp;
// 			    }
// 			}
// 		}
//#endif
//#endif

        Range =  ChannelInfo_Manager::ChannelInfo[chn].m_ucCurrentRange;   //获得当前range
        if( ChannelInfo_Manager::Insert_Current2HistoryData(chn,value,Range))
        {
// 				ChannelInfo_Manager::MarkSampleDataAsFresh( chn );  //04.23.2013
            if(ChannelInfo_Manager::ChannelInfo[chn].chn_State != LOGIC_PROCESS_TRAN)
            {
                ChannelInfo_Manager::m_DataFreshForPID[chn] = true;
                safety_check::I_Sample_Flag[chn] = 1;    //chen 20130812
                ChannelInfo_Manager::ChannelInfo[chn].Current_Sampleflg=1;  //dirui 20130917
                ChannelInfo_Manager::Set_CurrentRecalculateSign(chn);
            }
        }
        break;
    case SAMPLE_PROCESS_ADC_V:
        ///////chen comment out 20130730
//#if _BOARD_TYPE_  == BOARDTYPE_IBT
//#if _IBT_VERSION_ == IBT_VER_C
//         if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_IBT)
// 		{
//             if( ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion== BOARDTYPE_VER_C)
//             {
// 			    if(chn <8)
// 			    {
// 				    unsigned char temp = chn;
// 				    chn = 7 - temp;
// 			    }
// 			}
// 		}
//#endif
//#endif
//#if(_BOARD_TYPE_  == BOARDTYPE_LBT22024 )
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22023)
            Range = LBT22023_Base::VoltageRangeFlag[chn];
//#elif(_BOARD_TYPE_  == BOARDTYPE_LBT22013FR )
        else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22013)
            Range = LBT22013_Base::VoltageRangeFlag[chn];
//#elif(_BOARD_TYPE_  == BOARDTYPE_LBT22044 )
        else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22043)
            Range = LBT22043_Base::VoltageRangeFlag[chn];
//#else
        else
            Range =  ChannelInfo_Manager::ChannelInfo[chn].m_ucVoltageRange;   //获得当前range
//#endif
        if(  ChannelInfo_Manager::Insert_Voltage2HistoryData(chn,value,Range))
        {
// 					ChannelInfo_Manager::MarkSampleDataAsFresh( chn );  //04.23.2013
            if(ChannelInfo_Manager::ChannelInfo[chn].chn_State != LOGIC_PROCESS_TRAN)
            {
                ChannelInfo_Manager::m_DataFreshForPID[chn] = true;
                safety_check::V_Sample_Flag[chn] = 1;    //chen 20130812
                ChannelInfo_Manager::ChannelInfo[chn].Voltage_Sampleflg=1;//dirui 20130917
                ChannelInfo_Manager::Set_VoltageRecalculateSign(chn);
            }
        }
        break;
// 		case SAMPLE_PROCESS_ADC_T:
// 			break;
    default:
        break;
    }
// 		break;

// 	case SAMPLE_CTRL_MEDIAN_AVERAGE_FILTER:    //中位值滤波 这里就不支持了
// 		//插入历史数据
// 		//计算
// 		//放入PV
// 		break;

// 	case SAMPLE_CTRL_MOVING_AVERAGE:    //滑动平均滤波  但是没有历史值得时候咋办，用当前值来代替
// 		//转换成单极码字
// 		//插入历史数据
// 		//N的取值
// 		//把N个历史值 计算
// 		//转换成浮点数
// 		//gain offset
// 		//存入PV
// 		break;

// 	default:
// 		break;
// 	}
    return;
}

void Sample_Pro::ProcessBuiltInData(void)
{
//     for(unsigned char chn = 0;chn< AUX_BUILTIN_MAXCHANNELNO;chn++)
    for(unsigned char data_group_idx=0; data_group_idx<2; data_group_idx++)
    {
        if(!BuiltIn_DataManager::m_DataGroupMgr[data_group_idx])    //no used
        {
            continue;
        }
        ProcessInChipTemperature (data_group_idx); //处理片内温度传感器的读数
        for(unsigned char chn = 0; chn<MAX_AUXCH_PER_DATA_GROUP; chn++)
        {
            if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].updated == true)
            {
                enQueue(data_group_idx,chn);
                BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code = avgQueue( data_group_idx,chn, BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_avg_num);
                Convert_Chn(data_group_idx,chn);
                BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].updated = false;  //clear
                unsigned char CHN = BuiltIn_DataManager::m_PChn2EChn[data_group_idx][chn].e_data_group_id*16+chn;
// 								   unsigned char IV_ch = (CHN%ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount);
                unsigned char BuiltInCnt = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels/ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
                unsigned char IV_ch = CHN/BuiltInCnt;
                unsigned char BuiltInIdx = CHN%BuiltInCnt;
                ChannelInfo_Manager::ChannelInfo[IV_ch].m_fBulitInData[BuiltInIdx]=BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data;
                ChannelInfo_Manager::MetaVariable2[IV_ch][MetaCode_PV_BuiltIn_Data1 + BuiltInIdx].m_fValue = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data;
				MetaVariable_Task::Calculate_BuiltInAuxDXDT(data_group_idx,chn);
            }
        }
    }

}

void Sample_Pro::enQueue(unsigned char data_group_idx,unsigned char chn)
{
    unsigned long data;
    ST_BuiltIn_DATA * p_Buf = &BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn];
    switch(p_Buf->m_udataCount)
    {
    case 2:
        unsigned long a,b,c;
        a = p_Buf->m_dataA;
        b = p_Buf->m_dataB;
        c = p_Buf->aux_instant_code;
        unsigned long ba,ca;
        if(b > a)       ba = b - a;
        else            ba = a - b;
        if(c > a)       ca = c - a;
        else            ca = a - c;
        if ((ba/ca > 20) && (ba > 20))  //b为野点
            p_Buf->m_dataB = c;                  //抛弃b点，
        else
        {
            p_Buf->m_dataA = b;                 //b不是野点
            p_Buf->m_dataB = c;
            p_Buf->m_udataAfterFilter = b;
        }
        break;
    case 0://buffer里没有采样值，则使用最新采样进行1级BUFFER
        p_Buf->m_dataA = p_Buf->aux_instant_code;
        p_Buf->m_udataCount = 1;
        p_Buf->m_udataAfterFilter = p_Buf->aux_instant_code;
        break;
    case 1://buffer里有一个采样值，因为新的采样值无法判断是否为野值，保持不变
        p_Buf->m_dataB = p_Buf->aux_instant_code;
        p_Buf->m_udataCount = 2;
        break;
    default://buffer复位
        p_Buf->m_udataCount = 1;
        p_Buf->m_dataA = p_Buf->aux_instant_code;
        p_Buf->m_udataAfterFilter = p_Buf->aux_instant_code;
        break;
    }
    data = p_Buf->m_udataAfterFilter;
    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.ul_data[BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start] =data;

    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start ++;
    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length ++;
    if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start >= MAX_ADC_CODE_QUE)
        // 	if(Aux_DataManager::m_AuxDataBank[data_group_idx][chn].aux_adc_history.uc_start >=Aux_DataManager::m_AuxDataBank[data_group_idx][chn].m_Chn_Info.m_avg_num)
    {
        BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start = 0;
    }
    if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length >=MAX_ADC_CODE_QUE)
    {
        BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length = MAX_ADC_CODE_QUE;
    }
}

unsigned long Sample_Pro::avgQueue(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth)
{
    unsigned long long sum = 0;
    unsigned char counter = 0;
    unsigned char pointer = 0;
    if (filter_depth <=1) //不需滤波，直接返回当前码值
        return BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_instant_code ;
    else
    {
        if (BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length == 0 )
            return 0;
        if (filter_depth >BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length)
            counter = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_length;
        else
            counter = filter_depth;

        for (char i = 0; i<counter; i++)
        {
            if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start>=counter)
                pointer= (BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start - i-1) ;
            else
            {
                if(i<BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start)
                    pointer =i;
                else
                    pointer = MAX_ADC_CODE_QUE+ BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start-i-1;
            }
            // 			   pointer = (Aux_DataManager::m_AuxDataBank[data_group_id][chn].aux_adc_history.uc_start - i-1) ;
            // 				 if (pointer >MAX_ADC_CODE_QUE)	// pointer是unsigned char型，当i>uc_start时，pointer会变成非常大的数，因此此处判断改为pointer >MAX_ADC_CODE_QUE, Yifei
            // 						pointer +=MAX_ADC_CODE_QUE;

            sum+=BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.ul_data[pointer]; //累加最近的几个数据
        }

        return sum/counter;
    }


}

void Sample_Pro::ProcessInChipTemperature (unsigned char data_group_idx)
{

    enQueue_InChipTemp(data_group_idx);
    BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_avg_code=BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code;
    Convert_InChipTemp(data_group_idx);


}
void Sample_Pro::enQueue_InChipTemp( unsigned char data_group_idx)
{
    BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.ul_data[BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start] = BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code;
    BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start ++;
    BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length ++;
    if(BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start >= MAX_ADC_CODE_QUE)
    {
        BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_start = 0;
    }
    if(BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length >=MAX_ADC_CODE_QUE)
    {
        BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length = MAX_ADC_CODE_QUE;
    }
}

unsigned long Sample_Pro::avgQueue_InChipTemp(unsigned char data_group_idx,unsigned char chn, unsigned char filter_depth)
{
    unsigned long long sum = 0;
    unsigned char counter = 0;
    unsigned char pointer = 0;
    if (filter_depth <=1) //不需滤波，直接返回当前码值
        return BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_instant_code ;
    else
    {
        if (BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length == 0 )
            return 0;
        if (filter_depth >BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length)
            counter = BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.uc_length;
        else
            counter = filter_depth;

        for (unsigned char i = 0; i<counter; i++)
        {
            if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start>=counter)
                pointer= (BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start - i-1) ;
            else
            {
                if(i<BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start)
                    pointer =i;
                else
                    pointer = MAX_ADC_CODE_QUE+BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_adc_history.uc_start-i-1;
            }
            sum+=BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_adc_history.ul_data[pointer]; //累加最近的几个数据
        }
        return sum/counter;
    }
}
float Sample_Pro::Convert_Chn(unsigned char data_group_idx,unsigned char chn)
{
    float temp = 0.0f;

    unsigned short AuxSubType = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_AuxSubType;
    switch (BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_AuxType)
    {
    case AUX_VOLTAGE:		//"AuxV"   // 上位机分两层：第一层板类型 Type
        switch (AuxSubType)
        {
        case Regular_2ndV :
        case HIM_2ndV:

            float voltage;
            voltage = Sample_Pro::Convert_Regular_2ndV(data_group_idx, chn, BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);
            BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_voltage = voltage;
            BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = voltage;
            break;

        default:
            break;
        };
        break;

    case AUX_TEMPERATURE :                        // "T"
        switch (AuxSubType)
        {
        case  T_ThermoCouple:
// 			                     temp=Sample_Pro::Convert_ThermoCouple(data_group_idx,T_ThermoCouple,chn,BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);
// 			                     BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;
// 			                     break;

        case  K_ThermoCouple:
// 			                     temp=Sample_Pro::Convert_ThermoCouple(data_group_idx,K_ThermoCouple,chn,BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);
// 			                     BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;
// 		                      	break;
        case  J_ThermoCouple:
// 			                     temp=Sample_Pro::Convert_ThermoCouple(data_group_idx,J_ThermoCouple,chn,BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);
// 			                     BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;
// 			                     break;
        case  E_Thermocouple:
// 			                     temp=Sample_Pro::Convert_ThermoCouple(data_group_idx,E_Thermocouple,chn,BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);
// 			                     BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;

            temp=Sample_Pro::Convert_ThermoCouple(data_group_idx,AuxSubType,chn,BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_code);

// 			BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_instant_data = temp;
            if(abs(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data - temp) < 5)
            {
                BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;
                BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data_error_count = 0;
                CommonFunc::GetTimeTicksUpdate(&BulitInSampleErrorTime[data_group_idx][chn]);
            }
            else
            {
                BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data_error_count++;
                if(BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data_error_count > TSample_ErrorCount \
                        || CommonFunc::GetTimeTicks(&BulitInSampleErrorTime[data_group_idx][chn]) > 80000)  //采样更新间隔>8s
                {
                    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data = temp;
                    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_data_error_count = 0;
                    CommonFunc::GetTimeTicksUpdate(&BulitInSampleErrorTime[data_group_idx][chn]);
                }
            }

            break;
// 		                 case  TC_2ndV:
// 			                     float voltage;
// 			                     temp = Sample_Pro::Convert_TC_2ndV(data_group_idx, chn, BuiltIn_DataManager::m_BuiltInDataBankchn].aux_avg_code);
// 			                     BuiltIn_DataManager::m_BuiltInDataBank[chn].aux_data = temp;
// 			                     break;

        default:
            break;

        };
        break;
    default:
        break;
    }

    return temp;
}
float Sample_Pro::Convert_Regular_2ndV(unsigned char data_group_idx,unsigned char chn,unsigned long code)
{
    float adc_gain,adc_offset,phy_gain, phy_offset;

    adc_gain  =  BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;

    adc_offset = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

    phy_gain =  BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;

    phy_offset = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

    float pv = 0;
    unsigned long bound = 0x800000;
    // 	float bound = 0x800000;
    if(code > bound)
    {
        pv = ((float)(code - bound)/bound) * 5.0f;  //AD7175 参考5V
    }
    else
    {
        pv = ((float)(bound - code)/bound) * 5.0f;
        pv = -pv;
    }
    pv = (pv*adc_gain + adc_offset)*phy_gain + phy_offset;  //ADC 校准

    return pv;
}

float Sample_Pro::Convert_ThermoCouple(unsigned char data_group_idx,unsigned char TC_type,unsigned char chn, unsigned long code)
{
    float ref_voltage=0,phy_gain=1, phy_offset=0;
    float adc_gain=1,adc_offset=0;
    float pv =0;
    float result, temperature;
    phy_gain = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    phy_offset = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;


    Convert_InChipTemp(data_group_idx); //获取热电偶参考端（冷端温度）
    ref_voltage = Calculate_RefVoltage(TC_type, BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_data);	 //换算出参考端电压


    //pv = Convert_Regular_2ndV(data_group_idx,chn,code); //将ADC码值转换成电压
    adc_gain  =  BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
    adc_offset = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

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
    BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].aux_avg_voltage = pv;   // 保存压。以微伏为单位的电压值。
    pv += ref_voltage;  //  参考电压叠加

    switch (TC_type)
    {
    case T_ThermoCouple:
    {
        temperature = T_t90_Convert_TType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0f) > 0.0001f )
        {
            temperature = T_t90_Convert_TType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case K_ThermoCouple:
    {
        temperature = T_t90_Convert_KType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0f) > 0.0001f )
        {
            temperature =  T_t90_Convert_KType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case J_ThermoCouple:
    {
        temperature = T_t90_Convert_JType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0f) > 0.0001f )
        {
            temperature = T_t90_Convert_JType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    case E_Thermocouple:
    {
        temperature = T_t90_Convert_EType(pv, CHECK_T_VOLTAGERANG);    // 仅检查一次电压范围
        if( abs(temperature + 273.0f) > 0.0001f )
        {
            temperature = T_t90_Convert_EType(pv, COUNT_TEMPERATURE);    //t90计算
        }
    }
    break;
    default:
        result = -273.0;
        break;

    };
    result = temperature*phy_gain + phy_offset;  //温度校准

    //检查数据合理性
    if (result > BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
        result = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    if (result < BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
        result = BuiltIn_DataManager::m_BuiltInDataBank[data_group_idx][chn].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    return result;
}

void Sample_Pro::Convert_InChipTemp(unsigned char data_group_idx)
{
    unsigned long code;

    code =  BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_avg_code;  //获得平均码值


    code -= 0x800000;

    float pv = 0;

    pv =(float) (code*4.88f/1570.0f) - 273.0f;      //4.88是参考电压 V_Ref

    BuiltIn_DataManager::m_InChipTemperature[data_group_idx].InChipTemp_data = pv;  //片内温度

}

float Sample_Pro::Calculate_RefVoltage(unsigned char TC_type,float Ref_Temp)
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
float Sample_Pro::T_t90_Convert_KType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    //温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
    //温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[K型温度范围:-270(°C)到1372(°C)]
    if( f_Value < -6458 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >54886)
        return ( 1372 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9

    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    {
        CoE = &f_CoEA_Ktype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20644)        //温度范围在0(°C)到500(°C) ，微伏电压范围在0(μV)到20644(μV)
    {
        CoE = &f_CoEB_Ktype[0];
        for(uc_i=1; uc_i<10; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=54886)       //温度范围在500(°C)到1372(°C) ，微伏电压范围在20644(μV)到54886(μV)
    {
        CoE = &f_CoEC_Ktype[0];
        f_TempE =(*CoE);
        for(uc_i=1; uc_i<7; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float  Sample_Pro::T_t90_Convert_TType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[T型温度范围:-270(°C)到400(°C)]
    if( f_Value < -6258 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >20872)
        return ( 400 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    {
        CoE = &f_CoEA_Ttype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    {
        CoE = &f_CoEB_Ttype[0];
        for(uc_i=1; uc_i<7; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float Sample_Pro::T_t90_Convert_JType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-210(°C)到0(°C) ，微伏电压范围在-8095(μV)到0(μV)
    //温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
    //温度范围在760(°C)到1200(°C) ，微伏电压范围在42929(μV)到69553(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[J型温度范围:-270(°C)到1200(°C)]
    if( f_Value < -8095 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >69553)
        return ( 1200 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7+ c8*E^8+ c9*E^9
    if(f_Value<0)       //温度范围在-210(°C)到0(°C) ，微伏电压范围在-5891(μV)到0(μV)
    {
        CoE = &f_CoEA_Jtype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=42929)        //温度范围在0(°C)到760(°C) ，微伏电压范围在0(μV)到42929(μV)
    {
        CoE = &f_CoEB_Jtype[0];
        for(uc_i=1; uc_i<8; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=69553)       //温度范围在500(°C)到1372(°C) ，微伏电压范围在42929(μV)到69553(μV)
    {

        CoE = &f_CoEC_Jtype[0];
        f_TempE = (*CoE);
        for(uc_i=1; uc_i<6; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    return ( f_TempE );
}

float Sample_Pro::T_t90_Convert_EType(float f_Value,unsigned char uc_CheckFlag)
{
    //从微伏电压转化为摄时温度
    //温度范围在-200(°C)到0(°C) ，微伏电压范围在-8825(μV)到0(μV)
    //温度范围在0(°C)到1000(°C) ，微伏电压范围在0(μV)到76373(μV)
    unsigned  char  uc_i = 0;
    double  f_TempA = 1;
    double  f_TempE = 0;
    const float  *CoE;
    // 不接热电耦时默认为22摄时度
    //[E型温度范围:-270(°C)到1000(°C)]
    if( f_Value < -8825 )        //计算中以  微伏电压为最小单位运算
        return ( -273.0 );
    if( f_Value >76373 )
        return ( 1000 );

    if(uc_CheckFlag == CHECK_T_VOLTAGERANG)
    {
        return ( 1.0 );   // 仅检查电压值是否落在规定范围内
    }

    //计算和
    // t90 = c0 + c1*E + c2*E^2+ c3*E^3+ c4*E^4+ c5*E^5+ c6*E^6+ c7*E^7
    if(f_Value<0)       //温度范围在-200(°C)到0(°C) ，微伏电压范围在-5603(μV)到0(μV)
    {
        CoE = &f_CoEA_Etype[0];
        for(uc_i=1; uc_i<9; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }
    else if(f_Value<=20872)        //温度范围在0(°C)到400(°C) ，微伏电压范围在0(μV)到20872(μV)
    {
        CoE = &f_CoEB_Etype[0];
        for(uc_i=1; uc_i<10; uc_i++)
        {
            CoE++;
            f_TempA*= f_Value;
            f_TempE +=f_TempA * (*CoE);
        }
    }

    return ( f_TempE );
}

//这些接口由大数据体来提供

////放入历史值
//Insert_ToHistoryData()
//{
//
//}
//
////计算中值平均
//cal_median_average()
//{
//
//}
//
////计算滑动平均
//cal_moving_average()
//{
//
//}



///////////////////////////////////////////////////////////////
/*
//for 接口函数
char LinearX::Chn;
float LinearX::Value;
char LinearX::Type;

//for 对外参数
char LinearX::ADC_DEVICE_NUM;    //ADC器件数量

void LinearX::Init(void)
{
LinearX::ADC_DEVICE_NUM = 1;    //有几个器件  在此例只有1个器件

LinearX_adc::Chn = 0;
LinearX_adc::Value = 0;
LinearX_adc::Type = 0;

//for 运行所需
char LinearX::adc_chn = 0;    //adc器件内物理通道
}


//ProcessADC本身是一个FSM
//这样把不同的板类型都交给板类型adc文件来执行，修改
//而DAQ_Task则是一个通用函数

char LinearX::Data_acq_ProcessADC(char adc_device_chn)    //核心处理    //因为此理只有1个adc器件    //如果有多个器件则要进行adc_device_chn处理
{

//此例只有1个器件，如果多个则需要处理数组并加switch判断器件，采用不同的trigger函数
//trigger是不同的器件库的引用


static char status = TRIGGER;
static OS_Time Time;

switch(status)
{
case TRIGGER:
//first trigger
trigger(adc_chn);    //这里只有1个器件，如果多个再后面另加
status = WAIT;
Time = OS_Time;    //记录时间
return 0;     //证明并未完成

case WAIT:
if( TimeDiff(Time) >= LinearX_ADC_Period )    //判断时间差，等待转换时间完成
{
LinearX_adc::Chn = adc_chn;    //先记录再增加
LinearX_adc::Value = trigger( adc_chn++);    //触发下一通道并采样
LinearX_adc::Type = ADC_FOR_V;    //标明此adc
Time = OS_Time;    //重新记录时间

if(adc_chn >= ADC_1_MAX_NUM)
{
adc_chn = 0;    //回滚
}

return SIGMA_DELTA_ADC;
}

//等待完成
return 0;
}
}

float LinearX::trigger(char adc_chn)    //按照当前板类型的捕获及触发函数
{
//参考ADC_LTC2449::Trigger_n_Get(bdID, Ptr_Channel[bdID]);
//这里属于器件库
//省略
}

//对外接口函数，与DAQ函数指针相匹配
char LinearX::Data_acq_getValue(adc_device_chn)
{
return LinearX::Value;
}

float LinearX::Data_acq_getCHN(adc_device_chn)
{
return LinearX::Chn;
}

char LinearX::Data_acq_getType(adc_device_chn)
{
return LinearX::Type;
}
*/

