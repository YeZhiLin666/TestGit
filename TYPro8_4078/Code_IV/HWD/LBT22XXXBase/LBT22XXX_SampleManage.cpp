
#include "includes.h"


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

//for 接口函数
char LBT22XXX_SampleManage::m_chn;
unsigned long int LBT22XXX_SampleManage::m_value;
char LBT22XXX_SampleManage::m_type;


char LBT22XXX_SampleManage::m_ADC_chn;
char LBT22XXX_SampleManage::chn_DoIR;
char LBT22XXX_SampleManage::chn_history;
char LBT22XXX_SampleManage::ADC_Status;
unsigned char LBT22XXX_SampleManage::IR_CTRL_FLAG;

unsigned short LBT22XXX_SampleManage::TimeTicks;
#pragma arm section

void LBT22XXX_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;

    chn_history = 0;
    //状态机
    ADC_Status = LBT22XXX_ADC_IDLE;

    IR_CTRL_FLAG = 0;
}

char LBT22XXX_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
// 		ChannelInfo_Manager::Set_TimeRecalculateSign();
    TimeTicks++;

    switch(ADC_Status)
    {
    case LBT22XXX_ADC_IDLE:
        ADC_Status = LBT22XXX_ADC_WAIT;
        TimeTicks = 0;
        m_ADC_chn = 0;
        break;
    case LBT22XXX_ADC_WAIT:
        // 		if(TimeTicks >= (OS_TICK_INTERVAL / 50))
        // 		{
        ADC_Status = LBT22XXX_ADC_ReadIV;
        Adc16bit_AD7606::Convert();
        // 			TimeTicks = 0;
        // 		}
        break;
    case LBT22XXX_ADC_ReadIV:
        Adc16bit_AD7606::Read(&ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc,
                              &ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc);
        Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
        if(BurstModeSampling::uc_A_BurstMode[m_ADC_chn])
            BurstModeSampling::BurstSavePoint(m_ADC_chn);//  2017.6.8  yy

        ADC_Status = LBT22XXX_ADC_WAIT;
        break;
    default:
        Adc16bit_AD7606::Reset();
        ADC_Status = LBT22XXX_ADC_IDLE;
        break;
    }
    return 0;    //error
}

/*int LBT22XXX_SampleManage::ProcessADC(char adc_device_chn)
{

	return -1;    //error
}




char LBT22XXX_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//返回的是码字
unsigned long int LBT22XXX_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char LBT22XXX_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}*/



char LBT22XXX_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;


    IR_CTRL_FLAG = 1;

    return 1;
}

//恢复
char LBT22XXX_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear

    //LBT22XXX_Base::ADG_Reset();    //关闭
    ADC_Status = LBT22XXX_ADC_IDLE;

    return 1;
}


///////////////////

void  LBT22XXX_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    // 	Adc16bit_AD7606::Convert();    //conv

    // 	//ARBUS_DELAY(300);    //等待

    // 	*SampleI = LBT22XXX_ADC5DAC::ADC_rd(chn,BT2208_ADC_TYPE_I);
    // 	*SampleV = LBT22XXX_ADC5DAC::ADC_rd(chn,BT2208_ADC_TYPE_V);     //电压
    *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;
}
