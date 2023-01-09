#include "../../Entry/includes.h"

#define PWM_FILTER_COUNT   3

// char PWM_SampleManage::m_chn;
// char PWM_SampleManage::m_type;
char PWM_SampleManage::ADC_Status;
char PWM_SampleManage::ADC_Tick;
unsigned char PWM_SampleManage::CVdelayCounter;

unsigned long PWM_SampleManage::WaitForSampleCounter;
unsigned long PWM_SampleManage::SampleDelayCounter;
char PWM_SampleManage::ADC1_Status;
float PWM_SampleManage::m_InsideCtrlVoltageValue;
unsigned long PWM_SampleManage::m_u32ChannelVoltageAdc;

void PWM_SampleManage::Init(void)
{
// 	m_type = 0;

    ADC_Status = PWM_ADC_IDLE;
    ADC1_Status = PWM_ADC1_IDLE;
    SampleDelayCounter=0;
}

char PWM_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)
//		ChannelInfo_Manager::Set_TimeRecalculateSign();

    switch(ADC_Status)
    {
    case PWM_ADC_IDLE:
        ADC_Status = PWM_ADC_CONV;
        ADC_Tick   = 0;
        break;
    case PWM_ADC_CONV:
        ADC_Tick ++;
        //if(ADC_Tick >= (OS_TICK_INTERVAL/50))
        if(ADC_Tick >=(board_config::Feature_Enable_init.OS_Tick_Interval/50))
        {
            ADC_Status = PWM_ADC_Read;
        }
        break;
    case PWM_ADC_Read:
        unsigned long Voltage[PWM_FILTER_COUNT],Current[PWM_FILTER_COUNT];

        for(unsigned char i = 0; i < PWM_FILTER_COUNT; i++)
        {
            Adc16bit_AD7656::ReadA(&Current[i],&Voltage[i]);
// 			if (Current[i] > MaxI)
// 				MaxI = Current[i];
// 			if (Voltage[i] > MaxV)
// 				MaxV = Voltage[i];
// 			if (Current[i] < MinI)
// 				MinI = Current[i];
// 			if (Voltage[i] < MinV)
// 				MinV = Voltage[i];
// 			Vsum += Voltage[i];
// 			Isum += Current[i];
        }
        if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)//电流用7656采
        {
            ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc = CaculateFilterAverage(Current,PWM_FILTER_COUNT);
            ADC_Status = PWM_ADC_CONV;
        }
        else if((ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)||(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C))
        {
            ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc = CaculateFilterAverage(Current,PWM_FILTER_COUNT);
            ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc = CaculateFilterAverage(Voltage,PWM_FILTER_COUNT);
            Sample_Pro::NewCodeSampled[0] = 1;
            Sample_Pro::NewIRSampled[0] = 1;
            ADC_Status = PWM_ADC_CONV;
        }
        else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_D)
        {
            ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc = CaculateFilterAverage(Current,PWM_FILTER_COUNT);
// 			  ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc = (Vsum - MaxV - MinV)/(PWM_FILTER_COUNT - 2);
// 			  Sample_Pro::NewCodeSampled[0] = 1;
// 				Sample_Pro::NewIRSampled[0] = 1;
            ADC_Status = PWM_ADC_ReadC;
        }

        ADC_Tick = 0;
        break;
    case PWM_ADC_ReadC:
        unsigned long VoltageC[PWM_FILTER_COUNT],CurrentC[PWM_FILTER_COUNT];

        for(unsigned char i = 0; i < PWM_FILTER_COUNT; i++)
        {
            Adc16bit_AD7656::ReadC(&CurrentC[i],&VoltageC[i]);

// 			if (VoltageC[i] > MaxVC)
// 				MaxVC = VoltageC[i];
// 			if (VoltageC[i] < MinVC)
// 				MinVC = VoltageC[i];
// 			VsumC += VoltageC[i];

        }
        ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc = CaculateFilterAverage(VoltageC,PWM_FILTER_COUNT);
       Sample_Pro::NewCodeSampled[0] = 1;
				Sample_Pro::NewIRSampled[0] = 1;
        ADC_Status = PWM_ADC_CONV;
       ADC_Tick = 0;
        break;
	  case PWM_ADC_ReadB:
        unsigned long VoltageB[PWM_FILTER_COUNT],CurrentB[PWM_FILTER_COUNT];

        for(unsigned char i = 0; i < PWM_FILTER_COUNT; i++)
        {
            Adc16bit_AD7656::ReadB(&CurrentB[i],&VoltageB[i]);

// 			if (VoltageC[i] > MaxVC)
// 				MaxVC = VoltageC[i];
// 			if (VoltageC[i] < MinVC)
// 				MinVC = VoltageC[i];
// 			VsumC += VoltageC[i];

        }
        m_u32ChannelVoltageAdc = CaculateFilterAverage(VoltageB,PWM_FILTER_COUNT);
        Sample_Pro::NewCodeSampled[0] = 1;
        Sample_Pro::NewIRSampled[0] = 1;
        ADC_Status = PWM_ADC_CONV;
        ADC_Tick = 0;
        break;
    default:
        ADC_Status = PWM_ADC_IDLE;
        break;
    }
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion!=BOARDTYPE_VER_B)//ver_b才涉及到1259
        return 0;
/////////////////////////////////////AD1259采样/////////////////////////////
// 	unsigned long           TempCurrentAdc;
// 	unsigned long           TempVoltageAdc;
    switch(ADC1_Status)
    {
    case PWM_ADC1_IDLE:
        ADC1_Status = PWM_ADC1_Trigger;
        //m_ADC_chn = 0;
// 		if(PWM_SampleManage::WaitForSampleCounter>MAXSAMPLETIMECOUNTER)
// 			PWM_SampleManage::WaitForSampleCounter=MAXSAMPLETIMECOUNTER;
// 		if(PWM_SampleManage::WaitForSampleCounter<MINSAMPLETIMECOUNTER)
// 			PWM_SampleManage::WaitForSampleCounter=MINSAMPLETIMECOUNTER;
        Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_V];	//Ron 122115
        ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);
        Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_C];
        ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);   //cs操作
        break;
    case PWM_ADC1_Trigger:  //在一个状态中尝试同时触发操作 若不成功则退回下面注释掉的代码
        SSPIFunc::ConfirmDevice(1, &PWM_Base::PWM_SSP1ADC);
        SSPIFunc::ClearFIFO(1);
        Adc24bit_ADS1259::Convert();
        ADC1_Status =  PWM_ADC1_WAIT;
        break;

    case PWM_ADC1_WAIT:    //设定等待时间
        SampleDelayCounter++;  //干脆用counter吧，比较好计算
        if(SampleDelayCounter>PWM_SampleManage::WaitForSampleCounter)
        {
            ADC1_Status = PWM_ADC1_ReadI;
            SampleDelayCounter=0;
        }
        break;

    case	PWM_ADC1_ReadI:
        Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_C];
        //ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc = Adc24bit_ADS1259::Read(1);

        ADC1_Status = PWM_ADC1_ReadV;
        break;
    case 	PWM_ADC1_ReadV:

        Adc24bit_ADS1259::Pin_CS= PWM_Base::PinGroup[PWM_Pin_CS_V];
        ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc = Adc24bit_ADS1259::Read(1);

        Sample_Pro::NewCodeSampled[0] = 1;
        Sample_Pro::NewIRSampled[0] = 1;
        if(Step_Manager_task::bCV[0])
        {
            CVdelayCounter = 0;
            ADC1_Status = PWM_ADC1_CVdelay;
        }
        else
            ADC1_Status = PWM_ADC1_Trigger;
        break;
    case PWM_ADC1_CVdelay:
        CVdelayCounter++;
        if(CVdelayCounter >= 100)
        {
            CVdelayCounter = 0;
            ADC1_Status = PWM_ADC1_Trigger;
        }
        break;
    default:
        ADC1_Status = PWM_ADC1_IDLE;
        break;
    }
    /////////////////////////////////////AD1259采样/////////////////////////////
    return 0;
}
unsigned long PWM_SampleManage::CaculateFilterAverage(unsigned long *Array,unsigned char Lenth)
{
    unsigned long MaxVC  = 0;
    unsigned long	MinVC = CONVERTER_MAX;
    unsigned long	VsumC  = 0;
    if(Lenth<3)
        return 0;
    for(unsigned char i = 0; i < Lenth; i++)
    {

        if (Array[i] > MaxVC)
            MaxVC = Array[i];
        if (Array[i] < MinVC)
            MinVC = Array[i];
        VsumC += Array[i];
    }
    return (VsumC - MaxVC - MinVC)/(Lenth - 2);
}
char PWM_SampleManage::IR_ADC(char ChannelIdx)
{
    return 1;
}
char PWM_SampleManage::renew_IR_ADC(void)
{
    return 1;
}
void PWM_SampleManage::IR_Read_ADC(char ChannelIdx,unsigned long *SampleV,unsigned long *SampleI)
{
    *SampleV = ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc;
    *SampleI = ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc;
}
