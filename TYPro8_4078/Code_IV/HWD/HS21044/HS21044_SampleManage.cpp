
#include "includes.h"
#define HS21044_FILTER1_DEPTH   200
#define HS21044_FILTER2_DEPTH   5

UInt16 HS21044_SampleManage::Priority=PRIORITY4;
UInt16 HS21044_SampleManage::TaskID=	HS21044_SAMPLE_TASK;

//for 接口函数
char HS21044_SampleManage::m_chn;
unsigned long int HS21044_SampleManage::m_value;
char HS21044_SampleManage::m_type;


char HS21044_SampleManage::m_ADC_chn;
char HS21044_SampleManage::chn_DoIR;
char HS21044_SampleManage::chn_history;
char HS21044_SampleManage::ADC_Status;
unsigned char HS21044_SampleManage::IR_CTRL_FLAG;

// unsigned long HS21044_SampleManage::CurrSum1 = 0; //当前ADc输出的和值
// unsigned long HS21044_SampleManage::Count1 = 0; //累计了多少个ADC值
// unsigned long HS21044_SampleManage::CurrMax1 = 0x0; //最大值
// unsigned long HS21044_SampleManage::CurrMin1 = 0xffffff; //最小值

// unsigned long HS21044_SampleManage::CurrSum2 = 0; //当前ADc输出的和值
// unsigned long HS21044_SampleManage::Count2 = 0; //累计了多少个ADC值

// unsigned short HS21044_SampleManage::TimeTicks;

void HS21044_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;

    chn_history = 0;
    //状态机
    ADC_Status = HS21044_ADC_IDLE;

    IR_CTRL_FLAG = 0;
}

void HS21044_SampleManage::DoWork(void)
{
    unsigned long VoltADC; //保存ADC读入的电压值
    unsigned long CurrADC; //保存ADC读入的电流值
    unsigned long BV_ADC;  //build in Voltage
    if(!Pulse::m_Info.m_bPulseSample)
    {   //只有慢脉冲的时候才进行TIMER0采样

        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
        IR_Read_ADC_BV(0,&VoltADC,&CurrADC,&BV_ADC);		 //读取ADC值存入voltADC和CurrADCIR_Read_ADC(0,&ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc,&ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc);//
        IRQFunc::EnableIRQ(IRQ1,IRQ2);//Normal_Read_ADC,加深滤波

        ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0] = BV_ADC;
        ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode[0] = 1;

// 		//判断是否工步的开始，如果是，则减少一阶的滤波深度
// 	    if (ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Steptime].m_fValue<5 ||
// 			  ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_I ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_V ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_P ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_L )
//         //1)当前工步时间<=5秒 2)工步状态是sim_I 3)工步状态是sim_V 4)工步状态是sim_P 5)工步状态是sim_L 则不滤波
//         {
        ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc=CurrADC;
        ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc =VoltADC ; //窗口滤波器的更新内容存储于ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc
        Sample_Pro::NewCodeSampled[0] = 1;  ////20150918 sunshine
        SampleProcess_Pulse::NewCodeSampled = 1; ////20150918 sunshine刷新电流和电压采样的窗口滤波器
        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
//             Count1=0;                            //深度滤波寄存器清空，否则在工步转换的时候，可能残留上个工步的采样值
//             CurrSum1 =0;                         //深度滤波寄存器清空，否则在工步转换的时候，可能残留上个工步的采样值
//             CurrMax1 = 0x0; //最大值             //深度滤波寄存器清空，否则在工步转换的时候，可能残留上个工步的采样值
//             CurrMin1 = 0xffffff; //最小值        //深度滤波寄存器清空，否则在工步转换的时候，可能残留上个工步的采样值
// 		}
// 		else //20150918 sunshine
// 		{//一阶滤波，去掉最高和最低
// 			if( CurrADC<CurrMin1 )//20150918 sunshine
// 				CurrMin1=CurrADC;
// 			else if (CurrADC>CurrMax1)
// 				CurrMax1=CurrADC;
// 			CurrSum1 += CurrADC;//累加求和
// 			Count1++; //加的次数加一

// 			if(Count1 >= HS21044_FILTER1_DEPTH)//20150918 sunshine
// 			{
// 				CurrADC=(CurrSum1-CurrMax1-CurrMin1)/(HS21044_FILTER1_DEPTH-2);//只有滤波深度达到了，才更新 ADC值
// 				Count1=0;
// 				CurrSum1 =0;
// 				CurrMax1 = 0x0; //最大值
// 				CurrMin1 = 0xffffff; //最小值
// 				ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc =CurrADC;//CurrSum2/HS21044_FILTER2_DEPTH;
// 				ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc =VoltADC ;//窗口滤波器的更新内容存储于ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc
// 				Sample_Pro::NewCodeSampled[0] = 1;
// 				SampleProcess_Pulse::NewCodeSampled = 1; //刷新电流和电压采样的窗口滤波器
// 				Count2=0;
// 				CurrSum2 =0;
// 			}
// 		}
    }
}

char HS21044_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
//	ChannelInfo_Manager::Set_TimeRecalculateSign();
    return 0;
}



char HS21044_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;

    IR_CTRL_FLAG = 1;
    return 1;
}

//恢复
char HS21044_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear

    ADC_Status = HS21044_ADC_IDLE;
    return 1;
}


///////////////////

void  HS21044_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    unsigned long Current[3];     //记录3次采样的电流码值
    unsigned long Voltage[3];     //记录3次采样的电压码值

    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy(&Current[0],&Voltage[0]);
    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy(&Current[1],&Voltage[1]);
    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy(&Current[2],&Voltage[2]);

    if((Current[0] > Current[1]) == (Current[0] < Current[2]))
        *SampleI = Current[0];
    else if((Current[1] > Current[0]) == (Current[1] < Current[2]))
        *SampleI = Current[1];
    else
        *SampleI = Current[2];

    if((Voltage[0] > Voltage[1]) == (Voltage[0] < Voltage[2]))
        *SampleV = Voltage[0];
    else if((Voltage[1] > Voltage[0]) == (Voltage[1] < Voltage[2]))
        *SampleV = Voltage[1];
    else
        *SampleV = Voltage[2];
}

void  HS21044_SampleManage::IR_Read_ADC_BV(char chn,unsigned long *SampleV,unsigned long *SampleI,unsigned long *SampleBV)
{
    unsigned long Current[3];     //记录3次采样的电流码值
    unsigned long Voltage[3];     //记录3次采样的电压码值
    unsigned long BV[3];
    unsigned long Value;

    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy4Ch(&Current[0],&Voltage[0],&BV[0],&Value);
    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy4Ch(&Current[1],&Voltage[1],&BV[1],&Value);
    Adc16bit_AD7606::Convert();
    Adc16bit_AD7606::ReadAfterBusy4Ch(&Current[2],&Voltage[2],&BV[2],&Value);

    if((Current[0] > Current[1]) == (Current[0] < Current[2]))
        *SampleI = Current[0];
    else if((Current[1] > Current[0]) == (Current[1] < Current[2]))
        *SampleI = Current[1];
    else
        *SampleI = Current[2];

    if((Voltage[0] > Voltage[1]) == (Voltage[0] < Voltage[2]))
        *SampleV = Voltage[0];
    else if((Voltage[1] > Voltage[0]) == (Voltage[1] < Voltage[2]))
        *SampleV = Voltage[1];
    else
        *SampleV = Voltage[2];

    if((BV[0] > BV[1]) == (BV[0] < BV[2]))
        *SampleBV = BV[0];
    else if((BV[1] > BV[0]) == (BV[1] < BV[2]))
        *SampleBV = BV[1];
    else
        *SampleBV = BV[2];
}

