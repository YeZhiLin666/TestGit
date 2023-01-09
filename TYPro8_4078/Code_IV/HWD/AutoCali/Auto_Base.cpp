
/*
�Զ�У׼��
�ײ�����
comment out chen 20130422
*/

#include "../../common/includes.h"
//�Զ�У׼ģ���ADC��������

ST_Auto_CPLD_Status Auto_Cali_Base::st_auto_cpld;    //������ʷ״̬
unsigned char Auto_Cali_Base::RangeStatus;    //����

float Auto_Cali_Base::ADC_Gain;
float Auto_Cali_Base::ADC_Offset;

unsigned char Auto_Cali_Base::m_controlledFlag;

//Ӳ����ʼ��
void Auto_Cali_Base::Hardware_Init(void)
{
	IO_Init();    //�ڲ�IO��ʼ��

	//AD DA
	ADC_Init();
	DAC_Init();

	ADC_Gain = 1.0f;
	ADC_Offset = 0.0f;

	m_controlledFlag = 0;

	//Ĭ��Ϊoffģʽ
	Auto_Cali_Base::gotoOFF();    
}
//�����ʼ��
void Auto_Cali_Base::Software_Init(void)
{
	ADC_Gain = 1;
	ADC_Offset = 0;

	Auto_Cali_SampleManage::Init();
}

//IO ,ADC ,DAC
void Auto_Cali_Base::IO_Init(void)
{
	//SPI SSP0
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount1, SSPIFunc::MyIO1, SPI_ENABLE);    //unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName
	//GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount2, SSPIFunc::MyIO2, SSP_ENABLE);
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount3, SSPIFunc::MyIO3, SSP_ENABLE);

	AUTO_GPIO::GPIO_Init();
}

void Auto_Cali_Base::ADC_Init(void)
{
	//MISO0    p1.23
	//SCK0      p1.20
	//DCS1      p1.21
	//CONV     p1.19
	//MOSI0    RST24??    Ǳ�������Ѿ�����  comment out chen 20130507
	GPIOFunc::InitPort(1, 21, PM_OUT, 1,0);    //DCS1
	GPIOFunc::InitPort(1, 19, PM_OUT, 1,0);    //CONV

	AutoADC5DAC::Adc_Init(0);

	//����AD0
	//comment out chen 20130604
	AD0_Init(ADC_CLK);

}

void Auto_Cali_Base::DAC_Init(void)
{
	//MOSI    p0.18
	//MISO    p0.17
	//SCK      p0.15
	//DCS3    p0.16
	//DAC_LOAD    p1.3
	//DAC_CLR       p1.5
	//when KDAC is close    ADC_V+ ADC_V- is connect to ADC part    comment out chen 20130507
	GPIOFunc::InitPort(0,16, PM_OUT, 1,0);    //DCS3
	GPIOFunc::InitPort(1, 3, PM_OUT, 1,0);    //LOAD
	GPIOFunc::InitPort(1, 5, PM_OUT, 1,0);    //CLR

	AutoADC5DAC::Dac_Init(0);
}
//end IO ,ADC ,DAC

//����ģ������
//offģʽ
void Auto_Cali_Base::gotoOFF(void)
{
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,(~AUTO_CALI_CPLD_OFF_ADD1));
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,(~AUTO_CALI_CPLD_OFF_ADD2));
	//����״̬
	st_auto_cpld.ADR1 = (~AUTO_CALI_CPLD_OFF_ADD1);
	st_auto_cpld.ADR2 = (~AUTO_CALI_CPLD_OFF_ADD2);

	//dac out
	Auto_Cali_Base::DAC_out(0,0x800000,0);    //���0��ƫ����

	ARBUS_DELAY(10);
}

//ֻ��gotoOFF������ֱ��д��
//��������������ȡ��״̬��Ȼ��������Ӧ����
//����off to xxxģʽ
//comment out chen 20130502
void Auto_Cali_Base::gotoSELFHigh(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_SELF_HIGH_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_SELF_HIGH_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoSELFMid(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_SELF_MID_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_SELF_MID_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoSELFLow(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_SELF_LOW_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_SELF_LOW_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

//DAC���    K30  relay
void Auto_Cali_Base::gotoSELFDac(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_SELF_DAC_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_SELF_DAC_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	//dac out
	DAC_out(0,thirdParty::Dac_set_value,0);

	ARBUS_DELAY(10);
}

///////////////////////////////////
void Auto_Cali_Base::gotoHPHigh(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_HIGH_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_HIGH_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoHPMid(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_MID_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_MID_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoHPLow(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_LOW_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_LOW_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoHPIVSet(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_IVSET_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_IVSET_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoHPDac(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_DAC_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_DAC_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);

	DAC_out(0,thirdParty::Dac_set_value,0);

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoHPCLV(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_HP_CLV_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_HP_CLV_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

///////////////////////////////////
void Auto_Cali_Base::gotoADCHigh(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_HIGH_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_HIGH_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoADCMid(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_MID_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_MID_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoADCLow(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_LOW_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_LOW_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoADCIVSet(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_IVSET_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_IVSET_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	//dac out
	Auto_Cali_Base::DAC_out(0,thirdParty::Dac_set_value,0);

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoADCDac(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_DAC_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_DAC_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	//dac out
	Auto_Cali_Base::DAC_out(0,thirdParty::Dac_set_value,0);

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoADCCLV(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_ADC_CLV_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_ADC_CLV_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	//dac out
	Auto_Cali_Base::DAC_out(0,thirdParty::Dac_set_value,0);

	ARBUS_DELAY(10);
}
void Auto_Cali_Base::gotoCOM(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_COM_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_COM_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoVPP(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_VPP_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_VPP_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

void Auto_Cali_Base::gotoCELL_Bypass(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_CELL_BY_PASS_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_CELL_BY_PASS_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}

//IV������
void Auto_Cali_Base::gotoCELL_IV(void)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	//ȡ��״̬
	temp1 = st_auto_cpld.ADR1;
	temp2 = st_auto_cpld.ADR2;
	//��Ӧ����
	temp1 &= (~(AUTO_CALI_CPLD_CELL_IV_ADD1));
	temp2 &= (~(AUTO_CALI_CPLD_CELL_IV_ADD2));
	//д��cpld
	cpu_iowr_16(AUTO_CALI_CPLD_ADD,temp1);
	cpu_iowr_16(AUTO_CALI_CPLD_ADD_2,temp2);
	//����״̬
	st_auto_cpld.ADR1 =  temp1;
	st_auto_cpld.ADR2 =  temp2;

	ARBUS_DELAY(10);
}
//
//��صĳ�ŵ����ά������
//

//end ģ������

//DAC +- 10V��Ӧ24bitƫ����
unsigned long Auto_Cali_Base::CalibrateDac(float fdac)
{
	float f_gain,f_offset,f_Bound;
	float f_mid;
	unsigned long u32_Current;
	f_gain = 1;
	f_offset = 0;
	f_Bound = 0xFFFFFF;
	f_mid = 0x800000;
	fdac = fdac*f_gain + f_offset;

	u32_Current = f_mid + (fdac/Auto_DAC_out_Range)*f_mid;
	if(u32_Current>f_Bound)
	{
		return f_Bound;
	}
	else
	{
		return u32_Current;
	}
}

//ADC5DAC    ���DAC
char Auto_Cali_Base::DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type)
{
	AutoADC5DAC::Dac_out(Data);
	return 1;
}


///////////////
unsigned char Auto_Cali_Base::getWorkModule(void)
{
	return 0;
} 

//������̣�ADC����float
unsigned char Auto_Cali_Base::getRange(void)
{
	return RangeStatus;
}

void Auto_Cali_Base::AD0_Init(unsigned long ADC_clk)    //����AD0 P0.23
{
	/* Enable CLOCK into ADC controller */

	LPC_SC->PCONP    |= (( 1UL << 12) |        /* enable power to ADC           */
		( 1UL << 15)  );      /* enable power to IOCON         */

	LPC_IOCON->P0_23  =  ( 1UL <<  0);         /* P0.23 is AD0.0                */

	LPC_ADC->CR       = (( 1UL <<  0) |        /* select AD0.0 pin              */
		( 4UL <<  8) |        /* ADC clock is 60MHz / 5        */
		( 1UL << 21)  );      /* enable ADC                    */

	LPC_ADC->INTEN    =  ( 1UL <<  8);         /* global enable interrupt       */

	NVIC_EnableIRQ(ADC_IRQn);                  /* enable ADC Interrupt          */
	//enable P0.23


}



//------------------------------------------
void Auto_Cali_Base::watchDog(void)
{
	if(m_controlledFlag == 0)
	{
		m_controlledFlag = 1;
		//�������Ź�

		WDCLKSEL = 0x01;                      //pclk��Ϊʱ��Դ    pclk 72M/4  = 18M      
		WDTC = 0x1017DF80;	                    //���Ź������Ĵ�����������ʱֵ    60s = 60*18M/4
		WDMOD = 0x01 | 0x02;          //���Ź�ģʽ�Ĵ���

		WDFEED = 0xAA;	//start
		WDFEED = 0x55;
	}
	else if(m_controlledFlag == 1)
	{
		//ι��
		WDFEED = 0xAA;	
		WDFEED = 0x55;

		//�����ʱ��ûι����λ
	}


}
