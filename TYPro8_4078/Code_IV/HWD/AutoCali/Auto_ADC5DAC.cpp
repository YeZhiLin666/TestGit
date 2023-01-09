
#include "../../COMMON/includes.h"
//ADC  AD7190ģ�� ˫ͨ��
//DAC  AD5754R     �˲����Ѿ����


//ʹ��������AD7190
void AutoADC5DAC::Adc_Init(unsigned char Device_Number)
{
	unsigned char bOK = SSPIFunc::getModuleName(1);
	if(bOK  != AUTO_SSP0_MODULE_NAME)    //AD7190    SSP0
	{
		//���³�ʼ��
		//SPI

		//	S0SPCR = 0x00;    //��λSPI

		//	S0SPDR = 0x0000;
		pSSPI0->CR0 |= 0x0000;
		pSSPI0->DR = 0x0000;

		SSPI_Para Auto_SPP0;
		Auto_SPP0.uc_Module_Name = AUTO_SSP0_MODULE_NAME;    //name
		Auto_SPP0.uc_Speed = 5;    //5;
		Auto_SPP0.uc_CPOL = 1;
		Auto_SPP0.uc_CPHA = 1;
		Auto_SPP0.uc_MSTR = 1;    //master
		Auto_SPP0.uc_BITS = 8;
		Auto_SPP0.uc_LSBF = 0;
		Auto_SPP0.st_CS.Type = GP_MOD;     //gpio mod
		Auto_SPP0.st_CS.GPIO_PIN.P1 = 1;    
		Auto_SPP0.st_CS.GPIO_PIN.P2 = 21;    //AD5754ʹ��DCS3  P0.16

		SSPIFunc::setPara(&Auto_SPP0,1);    //���ò���
		SSPIFunc::my_Init(1);              //��ʼ��
	}

	Adc_AD7190::Adc_Init(0);
}

void AutoADC5DAC::Dac_Init(unsigned char Device_Number)
{
	//DCS3
	//DAC-LOAD1
	//DAC-CLR
	G_Port load = {1,3};
	GPIOFunc::InitPort(1, 3, PM_OUT, 0,0);     //LOAD ����
	GPIOFunc::Clr_G_Port(&load);

	unsigned char bOK = SSPIFunc::getModuleName(0);
	if(bOK  != AUTO_SPI_MODULE_NAME_AD5754)    //����������Teslaǯѹ����ͬ
	{
		//���³�ʼ��
		//SPI
		pSSPI0->CR0 |= 0x0080;       //��λSPI
		pSSPI0->DR = 0x0000;
		SSPI_Para Auto_SPI;
		Auto_SPI.uc_Module_Name = AUTO_SPI_MODULE_NAME_AD5754;    //name
		Auto_SPI.uc_Speed = 7;    //5;
		Auto_SPI.uc_CPOL = 0;
		Auto_SPI.uc_CPHA = 1;
		Auto_SPI.uc_MSTR = 1;    //master
		Auto_SPI.uc_BITS = 8;
		Auto_SPI.uc_LSBF = 0;
		Auto_SPI.st_CS.Type = GP_MOD;     //gpio mod
		Auto_SPI.st_CS.GPIO_PIN.P1 = 0;    
		Auto_SPI.st_CS.GPIO_PIN.P2 = 16;    //AD5754ʹ��DCS3  P0.16

		SSPIFunc::setPara(&Auto_SPI,0);    //���ò���
		SSPIFunc::my_Init(0);              //��ʼ��
	}
	DAC16bit_AD5754::Init(0);
}

//ʹ��AD5754���
//AutoCali����ʹ����VOUTA��Ϊ���
void AutoADC5DAC::Dac_out(unsigned long value)
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	unsigned long temp = value;

	unsigned char bOK = SSPIFunc::getModuleName(0);
	if(bOK  != AUTO_SPI_MODULE_NAME_AD5754)    //����������Teslaǯѹ����ͬ
	{
		//���³�ʼ��
		//SPI

		pSSPI0->CR0 |= 0x0080;       //��λSPI
		pSSPI0->DR = 0x0000;
		SSPI_Para Auto_SPI;
		Auto_SPI.uc_Module_Name = AUTO_SPI_MODULE_NAME_AD5754;    //name
		Auto_SPI.uc_Speed = 7;    //5;
		Auto_SPI.uc_CPOL = 0;
		Auto_SPI.uc_CPHA = 1;
		Auto_SPI.uc_MSTR = 1;    //master
		Auto_SPI.uc_BITS = 8;
		Auto_SPI.uc_LSBF = 0;
		Auto_SPI.st_CS.Type = GP_MOD;     //gpio mod
		Auto_SPI.st_CS.GPIO_PIN.P1 = 0;    
		Auto_SPI.st_CS.GPIO_PIN.P2 = 16;    //AD5754ʹ��DCS3  P0.16

		SSPIFunc::setPara(&Auto_SPI,0);    //���ò���
		SSPIFunc::my_Init(0);              //��ʼ��
	}

	ARBUS_DELAY(10);
	DAC16bit_AD5754::Dac_Out(0,temp);    //AD5754    VOUTA    0chn
	//000 DACA
	//001 DACB
	//010 DACC
	//011 DACD
	//100 ALLADC
	//comment out chen 20130410

}


