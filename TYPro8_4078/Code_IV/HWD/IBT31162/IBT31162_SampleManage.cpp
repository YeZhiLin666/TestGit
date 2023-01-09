
#include "../../Entry/includes.h"




//for �ӿں���

char IBT31162_SampleManage::m_chn;
unsigned long int IBT31162_SampleManage::m_value;
char IBT31162_SampleManage::m_type;


unsigned char IBT31162_SampleManage::m_ch_Index;
char IBT31162_SampleManage::m_ADC_chn;
char IBT31162_SampleManage::chn_DoIR;
char IBT31162_SampleManage::chn_history;
char IBT31162_SampleManage::ADC_Status;
unsigned char IBT31162_SampleManage::IR_CTRL_FLAG;

unsigned short IBT31162_SampleManage::TimeOut_Count;
unsigned short IBT31162_SampleManage::TimeTicks;
// unsigned long IBT31162_SampleManage::SampleDelayCounter;
// unsigned long IBT31162_SampleManage::WaitForSampleCounter;
const unsigned short IBT31162_SampleManage::DelayTickCount_ADG = 6;   //50*6+1=350us


void IBT31162_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	//�ڲ���ת
	//m_I_chn = 0;    //I��Vͨ����0��ʼ
	//m_V_chn = 0;
	m_ADC_chn = 0;
	m_ch_Index = 0;

	chn_history = 0;
	//״̬��
	ADC_Status = IBT31162_ADC_IDLE;
	IR_CTRL_FLAG = 0;
}

char IBT31162_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    unsigned char temp;
    unsigned long data;
    
	//if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
		ChannelInfo_Manager::Set_TimeRecalculateSign();

  switch(ADC_Status)
  {
    case IBT31162_ADC_IDLE:
        ADC_Status = IBT31162_ADC_OpenADG;
        m_ADC_chn = 0;
        m_ch_Index = 0;
        TimeTicks=0;
        Adc24bit_AD7175::ADC_CS= IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYC];  //����
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);  
        Adc24bit_AD7175::ADC_CS= IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYV];   //��ѹ
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);  	
        Adc24bit_AD7175::ADC_START = IBT31162_Base::PinGroup[IBT31162_Pin_ADCSTART];// 
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        break;
    case IBT31162_ADC_OpenADG:
        IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_I = IBT31162_PGA_G2  ;   // IA0�����λ����ͷ��     ������PGA281������Ϊ2
        IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_V = IBT31162_PGA_G1  ;   // VA0�����λ����ͷ��     ��ѹPGA281������Ϊ1
        CPLD_FUNC::WriteDOBus(16+m_ch_Index,IBT31162_Base::IBT31162_PGA_IVA.Reg);

        TimeTicks = 0;
        ADC_Status = IBT31162_ADC_OPENING;
        if(IR_CTRL_FLAG == 1)    //�����IR����
        {
            m_ch_Index = chn_DoIR;
            IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_I = IBT31162_PGA_G2  ;   // IA0�����λ����ͷ��     ������PGA281������Ϊ2
            IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_V = IBT31162_PGA_G1  ;   // VA0�����λ����ͷ��     ��ѹPGA281������Ϊ1
            CPLD_FUNC::WriteDOBus(16+m_ch_Index,IBT31162_Base::IBT31162_PGA_IVA.Reg);
            ADC_Status = IBT31162_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
        }
        break;
    case IBT31162_ADC_OPENING:
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            ADC_Status = IBT31162_ADC_TriggerI;
            TimeTicks = 0;
        }
        break;
    case IBT31162_ADC_TriggerI:
        SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); //Add by DKQ 03.08.2016
        Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_C];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
        Adc24bit_AD7175::Trigger(1);
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        ADC_Status = IBT31162_ADC_ReadI;
        TimeOut_Count = 0;
        break;
    case IBT31162_ADC_ReadI:
        Adc24bit_AD7175::ADC_DRDY= IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYC];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
	  	    SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); //Add by DKQ 03.08.2016
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
                ChannelInfo_Manager::ChannelInfo[m_ch_Index].m_u32CurrentAdc=data;
        }
        if(++TimeOut_Count > IBT31162_ADC_READTIMEOUT)  // 1ms
        {	
            temp = 0;
			IBT31162_ADC5DAC::Adc_Init();
			Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_C];
		}

        if(temp==0)
        {
            ADC_Status = IBT31162_ADC_TriggerV;
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);		
        }
        break;
    case IBT31162_ADC_TriggerV:
        SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); ////Add by DKQ 03.08.2016         
        Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_V];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
        Adc24bit_AD7175::Trigger(1);
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        ADC_Status = IBT31162_ADC_ReadV;
        TimeOut_Count = 0;
        break;
    case IBT31162_ADC_ReadV:
        Adc24bit_AD7175::ADC_DRDY = IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYV];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
		    SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); //Add by DKQ 03.08.2016
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                ChannelInfo_Manager::ChannelInfo[m_ch_Index].m_u32VoltageAdc=data;
                Sample_Pro::NewCodeSampled[m_ch_Index] = 1;
            }
        }

        if(++TimeOut_Count > IBT31162_ADC_READTIMEOUT)  // 1ms
        {
            temp = 0;
			IBT31162_ADC5DAC::Adc_Init();
			Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_V];
		}

        if(temp == 0)
        {
            ADC_Status = IBT31162_ADC_CloseADG;
            //ACR::Write_V(m_ADC_chn,ACR_OFF);
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);		
        }
        break;
    case IBT31162_ADC_CloseADG:
         if(IR_CTRL_FLAG == 0)    //���û��IR�������
         {
		    m_ADC_chn = 0;   // AD7175Ƭ��ͨ������1��
           m_ch_Index++;    // ���ʰ�ͨ��
           if(m_ch_Index>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
             m_ch_Index=0;
           ADC_Status = IBT31162_ADC_OpenADG;
         }
         else
           ADC_Status = IBT31162_ADC_OpenADG;
         break;
    case IBT31162_ADC_NoWork:
         break;
    default:
         ADC_Status = IBT31162_ADC_IDLE;
         break;
	}
	return 0;    //error	
}

////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char IBT31162_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//�ָ�
char IBT31162_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear
	ADC_Status = IBT31162_ADC_IDLE;

	return 1;
}

///////////////////
void  IBT31162_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    unsigned char temp = 1;
    unsigned long TimeOut_Count = 0;

    IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_I = IBT31162_PGA_G2  ;   // IA0�����λ����ͷ��     ������PGA281������Ϊ2
    IBT31162_Base::IBT31162_PGA_IVA.Bits.AdcGain_V = IBT31162_PGA_G1  ;   // VA0�����λ����ͷ��     ��ѹPGA281������Ϊ1
    CPLD_FUNC::WriteDOBus(16+chn,IBT31162_Base::IBT31162_PGA_IVA.Reg);

    SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); 
    Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_C];
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
    Adc24bit_AD7175::Trigger(1);
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);



    Adc24bit_AD7175::ADC_DRDY= IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYC];
    TimeOut_Count = 0;
    temp = 1;
    while(temp==1)
    {
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            ChannelInfo_Manager::ChannelInfo[chn].m_u32CurrentAdc = Adc24bit_AD7175::Read(1);
        }
        if(++TimeOut_Count > 24000)  // 1ms
        {	
            temp = 0;
			IBT31162_ADC5DAC::Adc_Init();
			Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_C];
		}

    }
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);

    SSPIFunc::ConfirmDevice(1, &IBT31162_Base::IBT31162_SSP1ADC); 	
    Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_V];
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
    Adc24bit_AD7175::Trigger(1);
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);

    Adc24bit_AD7175::ADC_DRDY = IBT31162_Base::PinGroup[IBT31162_Pin_ADCDRDYV];
    TimeOut_Count = 0;
    temp = 1;
    while(temp==1)
    {
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            ChannelInfo_Manager::ChannelInfo[chn].m_u32VoltageAdc = Adc24bit_AD7175::Read(1);
        }
        if(++TimeOut_Count > 24000)  // 1ms
        {
            temp = 0;
			IBT31162_ADC5DAC::Adc_Init();
			Adc24bit_AD7175::ADC_CS = IBT31162_Base::PinGroup[IBT31162_Pin_CS_V];
		}

    }
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);

    *SampleI = ChannelInfo_Manager::ChannelInfo[chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[chn].m_u32VoltageAdc;     //��ѹ
}

