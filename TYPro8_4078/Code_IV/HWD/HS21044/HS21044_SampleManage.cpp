
#include "includes.h"
#define HS21044_FILTER1_DEPTH   200
#define HS21044_FILTER2_DEPTH   5

UInt16 HS21044_SampleManage::Priority=PRIORITY4;
UInt16 HS21044_SampleManage::TaskID=	HS21044_SAMPLE_TASK;

//for �ӿں���
char HS21044_SampleManage::m_chn;
unsigned long int HS21044_SampleManage::m_value;
char HS21044_SampleManage::m_type;


char HS21044_SampleManage::m_ADC_chn;
char HS21044_SampleManage::chn_DoIR;
char HS21044_SampleManage::chn_history;
char HS21044_SampleManage::ADC_Status;
unsigned char HS21044_SampleManage::IR_CTRL_FLAG;

// unsigned long HS21044_SampleManage::CurrSum1 = 0; //��ǰADc����ĺ�ֵ
// unsigned long HS21044_SampleManage::Count1 = 0; //�ۼ��˶��ٸ�ADCֵ
// unsigned long HS21044_SampleManage::CurrMax1 = 0x0; //���ֵ
// unsigned long HS21044_SampleManage::CurrMin1 = 0xffffff; //��Сֵ

// unsigned long HS21044_SampleManage::CurrSum2 = 0; //��ǰADc����ĺ�ֵ
// unsigned long HS21044_SampleManage::Count2 = 0; //�ۼ��˶��ٸ�ADCֵ

// unsigned short HS21044_SampleManage::TimeTicks;

void HS21044_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;

    chn_history = 0;
    //״̬��
    ADC_Status = HS21044_ADC_IDLE;

    IR_CTRL_FLAG = 0;
}

void HS21044_SampleManage::DoWork(void)
{
    unsigned long VoltADC; //����ADC����ĵ�ѹֵ
    unsigned long CurrADC; //����ADC����ĵ���ֵ
    unsigned long BV_ADC;  //build in Voltage
    if(!Pulse::m_Info.m_bPulseSample)
    {   //ֻ���������ʱ��Ž���TIMER0����

        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
        IR_Read_ADC_BV(0,&VoltADC,&CurrADC,&BV_ADC);		 //��ȡADCֵ����voltADC��CurrADCIR_Read_ADC(0,&ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc,&ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc);//
        IRQFunc::EnableIRQ(IRQ1,IRQ2);//Normal_Read_ADC,�����˲�

        ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0] = BV_ADC;
        ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode[0] = 1;

// 		//�ж��Ƿ񹤲��Ŀ�ʼ������ǣ������һ�׵��˲����
// 	    if (ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Steptime].m_fValue<5 ||
// 			  ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_I ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_V ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_P ||
//         ChannelInfo_Manager::ChannelStep[0].m_Info[ChannelInfo_Manager::ChannelStep[0].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType==CT_IVCH_SIMU_L )
//         //1)��ǰ����ʱ��<=5�� 2)����״̬��sim_I 3)����״̬��sim_V 4)����״̬��sim_P 5)����״̬��sim_L ���˲�
//         {
        ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc=CurrADC;
        ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc =VoltADC ; //�����˲����ĸ������ݴ洢��ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc
        Sample_Pro::NewCodeSampled[0] = 1;  ////20150918 sunshine
        SampleProcess_Pulse::NewCodeSampled = 1; ////20150918 sunshineˢ�µ����͵�ѹ�����Ĵ����˲���
        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
//             Count1=0;                            //����˲��Ĵ�����գ������ڹ���ת����ʱ�򣬿��ܲ����ϸ������Ĳ���ֵ
//             CurrSum1 =0;                         //����˲��Ĵ�����գ������ڹ���ת����ʱ�򣬿��ܲ����ϸ������Ĳ���ֵ
//             CurrMax1 = 0x0; //���ֵ             //����˲��Ĵ�����գ������ڹ���ת����ʱ�򣬿��ܲ����ϸ������Ĳ���ֵ
//             CurrMin1 = 0xffffff; //��Сֵ        //����˲��Ĵ�����գ������ڹ���ת����ʱ�򣬿��ܲ����ϸ������Ĳ���ֵ
// 		}
// 		else //20150918 sunshine
// 		{//һ���˲���ȥ����ߺ����
// 			if( CurrADC<CurrMin1 )//20150918 sunshine
// 				CurrMin1=CurrADC;
// 			else if (CurrADC>CurrMax1)
// 				CurrMax1=CurrADC;
// 			CurrSum1 += CurrADC;//�ۼ����
// 			Count1++; //�ӵĴ�����һ

// 			if(Count1 >= HS21044_FILTER1_DEPTH)//20150918 sunshine
// 			{
// 				CurrADC=(CurrSum1-CurrMax1-CurrMin1)/(HS21044_FILTER1_DEPTH-2);//ֻ���˲���ȴﵽ�ˣ��Ÿ��� ADCֵ
// 				Count1=0;
// 				CurrSum1 =0;
// 				CurrMax1 = 0x0; //���ֵ
// 				CurrMin1 = 0xffffff; //��Сֵ
// 				ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc =CurrADC;//CurrSum2/HS21044_FILTER2_DEPTH;
// 				ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc =VoltADC ;//�����˲����ĸ������ݴ洢��ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc
// 				Sample_Pro::NewCodeSampled[0] = 1;
// 				SampleProcess_Pulse::NewCodeSampled = 1; //ˢ�µ����͵�ѹ�����Ĵ����˲���
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
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;

    IR_CTRL_FLAG = 1;
    return 1;
}

//�ָ�
char HS21044_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear

    ADC_Status = HS21044_ADC_IDLE;
    return 1;
}


///////////////////

void  HS21044_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    unsigned long Current[3];     //��¼3�β����ĵ�����ֵ
    unsigned long Voltage[3];     //��¼3�β����ĵ�ѹ��ֵ

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
    unsigned long Current[3];     //��¼3�β����ĵ�����ֵ
    unsigned long Voltage[3];     //��¼3�β����ĵ�ѹ��ֵ
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

