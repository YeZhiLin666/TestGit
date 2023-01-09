
#include "includes.h"




//for �ӿں���
char HPT200_SampleManage::m_chn;
unsigned long int HPT200_SampleManage::m_value;
char HPT200_SampleManage::m_type;


char HPT200_SampleManage::m_ADC_chn;
char HPT200_SampleManage::chn_DoIR;
char HPT200_SampleManage::chn_history;
char HPT200_SampleManage::ADC_Status;
unsigned char HPT200_SampleManage::IR_CTRL_FLAG;

unsigned short HPT200_SampleManage::TimeTicks;
// MsTime HPT200_SampleManage::RecordTime;
// MsTime HPT200_SampleManage::LastTime;

unsigned long HPT200_SampleManage::SampleDelayCounter;
unsigned long HPT200_SampleManage::WaitForSampleCounter;

void HPT200_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;

    chn_history = 0;
    //״̬��
    ADC_Status = HPT200_ADC_IDLE;
    SampleDelayCounter=0;
    IR_CTRL_FLAG = 0;
}

char HPT200_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
//		ChannelInfo_Manager::Set_TimeRecalculateSign();
    TimeTicks++;

    //	unsigned char bOK;
    switch(ADC_Status)
    {
    case HPT200_ADC_IDLE:
        ADC_Status = HPT200_ADC_Trigger;
        m_ADC_chn = 0;
        if(HPT200_SampleManage::WaitForSampleCounter>MAXSAMPLETIMECOUNTER)
            HPT200_SampleManage::WaitForSampleCounter=MAXSAMPLETIMECOUNTER;
        if(HPT200_SampleManage::WaitForSampleCounter<MINSAMPLETIMECOUNTER)
            HPT200_SampleManage::WaitForSampleCounter=MINSAMPLETIMECOUNTER;
        Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];	//Ron 122115
        ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);
        Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
        ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);   //cs����
        break;
    case HPT200_ADC_Trigger:  //��һ��״̬�г���ͬʱ�������� �����ɹ����˻�����ע�͵��Ĵ���
        SSPIFunc::ConfirmDevice(1, &HPT200_Base::HPT200_SSP1ADC);
        //            if(SSPIFunc::getModuleName(1)  != SSP1_DEVICE1)
        //            {
        //                SSPIFunc::setPara(&HPT200_Base::HPT200_SSP1ADC,1);
        //                SSPIFunc::my_Init(1);              //��ʼ��
        //                ARBUS_DELAY(10);
        //            }
        SSPIFunc::ClearFIFO(1);

        //            Adc24bit_ADS1259::Pin_CS= HPT200_Base::Pin_CS_C;
        //            ControlPin::SetMeLow(Adc24bit_ADS1259::Pin_CS);
        //            Adc24bit_ADS1259::Pin_CS= HPT200_Base::Pin_CS_V;
        //            ControlPin::SetMeLow(Adc24bit_ADS1259::Pin_CS);  //cs����

        Adc24bit_ADS1259::Convert();
        //            Adc24bit_ADS1259::Pin_CS= HPT200_Base::Pin_CS_V;
        //            ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);
        //            Adc24bit_ADS1259::Pin_CS= HPT200_Base::Pin_CS_C;
        //            ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);   //cs����
        ADC_Status =  HPT200_ADC_WAIT;
        break;

    case HPT200_ADC_WAIT:    //�趨�ȴ�ʱ��
        SampleDelayCounter++;  //�ɴ���counter�ɣ��ȽϺü���
        if(SampleDelayCounter>HPT200_SampleManage::WaitForSampleCounter)
        {
            ADC_Status = HPT200_ADC_ReadI;
            SampleDelayCounter=0;
        }
        break;

    case	HPT200_ADC_ReadI:
        Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc = Adc24bit_ADS1259::Read(1);
        ADC_Status = HPT200_ADC_ReadV;
        break;
    case 	HPT200_ADC_ReadV:

        Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_ADS1259::Read(1);

        Sample_Pro::NewCodeSampled[0] = 1;
        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
        ADC_Status = HPT200_ADC_Trigger;
        break;
    default:
        ADC_Status = HPT200_ADC_IDLE;
        break;
    }
    return 0;    //error
}

// int HPT200_SampleManage::ProcessADC(char adc_device_chn)
// {

// 	return -1;    //error
// }




// char HPT200_SampleManage::getCHN(char adc_device_chn)
// {
// 	return m_chn;
// }

// //���ص�������
// unsigned long int HPT200_SampleManage::getValue(char adc_device_chn)
// {
// 	return m_value;
// }

// char HPT200_SampleManage::getType(char adc_device_chn)
// {
// 	return m_type;
// }



char HPT200_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;


    IR_CTRL_FLAG = 1;

    return 1;
}

//�ָ�
char HPT200_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear

    //HPT200_Base::ADG_Reset();    //�ر�
    ADC_Status = HPT200_ADC_IDLE;

    return 1;
}


///////////////////

void  HPT200_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{

    // 	*SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
    // 	*SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;
    SSPIFunc::ClearFIFO(1);

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
    ControlPin::SetMeLow(Adc24bit_ADS1259::Pin_CS);
    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
    ControlPin::SetMeLow(Adc24bit_ADS1259::Pin_CS);  //cs����

    Adc24bit_ADS1259::Convert();

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
    ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);
    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
    ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);   //cs����

    //*SampleI = Adc24bit_ADS1259::Read(1,0);
    *SampleI = Adc24bit_ADS1259::Read(1);

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
    //*SampleV = Adc24bit_ADS1259::Read(1,0);
    *SampleV = Adc24bit_ADS1259::Read(1);

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
    ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);
    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
    ControlPin::SetMeHigh(Adc24bit_ADS1259::Pin_CS);   //cs����
}

// MsTime  HPT200_SampleManage::TimeT(MsTime a,MsTime b)    // a<b
// {
// 	MsTime tempDiff;
// 	signed short temp;

// 	tempDiff.Second = b.Second - a.Second;
// 	temp = b.Us100 - a.Us100;
// 	if(temp < 0)
// 	{
// 		temp = temp + 10000;
// 		tempDiff.Second --;
// 	}
// 	tempDiff.Us100 = temp;
// 	return tempDiff;
// }    //delete by qjm 20171222
