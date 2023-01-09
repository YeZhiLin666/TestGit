
#include "../../COMMON/includes.h"

//��IBTͨ�� û��ǯѹ

/*
���ļ��ṩ��IBT���Ͱ��µ�ʱ�����������в���
comment out chen 20130216
IO������ʱû�в���
*/


//ע�⣬ʵ���ϲ����ܿ���ͨ��    
//cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
//�����й�CPLD ADD�Ĳ��������д��� comment out chen 20130106

/*Format
//�ṹ����  ÿ��ʱ����Ҫ����Сmove�����
// Disposable һ����
******************************************************************************/
//����line370+
St_DisposableTask IBT_Base::SetCtrlOpenCh[2];
St_DisposableTask IBT_Base::SetCtrlCloseCh[2];
St_DisposableTask IBT_Base::SetCCToCC[2];
St_DisposableTask IBT_Base::SetRstToOff[2]; 
St_DisposableTask IBT_Base::SetOpenISO[2];
St_DisposableTask IBT_Base::SetCloseISO[2];
St_DisposableTask IBT_Base::SetCVToCV[2];
unsigned char IBT_Base::SetCtrlOpenCh_Len = 2;
unsigned char IBT_Base::SetCtrlCloseCh_Len = 2;
unsigned char IBT_Base::SetCCToCC_Len = 2;
unsigned char IBT_Base::SetRstToOff_Len = 2;
unsigned char IBT_Base::SetOpenISO_Len = 2;
unsigned char IBT_Base::SetCloseISO_Len = 2;
unsigned char IBT_Base::SetCVToCV_Len = 2;

SSPI_Para  IBT_SSP0DAC,IBT_SSP0DAC_CLAMP,IBT_SSP1ADI,IBT_SSP1ADV,IBT_SSP0_FRAM;  


unsigned char IBT_Base::MyIO_IBT[6][4] = 
{
	{1,11,PM_OUT,1},
	{1,12,PM_OUT,1},
	{1,13,PM_OUT,1},
	{1,18,PM_OUT,1},
	{1,19,PM_IN,1},
	{0,31,PM_OUT,1},
};						


volatile unsigned short IBT_Base::IBT_relaystatus[MAXCHANNELNO];    //��¼�����relay״̬
//unsigned short IBT_Base::NBUS_WRITE_DCS_status;    //��¼����CS״̬     ����CPLD����Բ�ʹ��
// ���³�ʼ�������� 
void ReInitPullUP(void);



//start init
//hardware init
/******************************************************************************
** Function name:		Hardware_Init
**
** Descriptions:	        INitialize typeA IO/ADC/DAC
**			     
** parameters:		  None
** Returned value:		None
** 
******************************************************************************/
void IBT_Base::Hardware_Init(void)
{  
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	IO_Init();    //�ڲ�IO��ʼ��

	//GPIO P   3.18 WI    0.6 DCS2  1.21 DCS1   1.28 CONV   ��ΪGPIO output �ҳ�ʼ��������ͬ  WIΪ�� ����ӦΪ��
	//PINSEL7 &= 0xFFFFFFCF;    //P3.18GPIO  ι���ź�
	//FIO3DIR |= 0x00040000;    //P3.18 output
	//FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	GPIOFunc::InitPort(3,18,PM_OUT,1,0);    //��Ҫ����GPIO_InitValue  ����ߣ���1������0    //���������

	/*// ι���ź�
	//{// ι���ź�
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}*/
	WD(200);  // ι���ź�

	cpu_iowr_16(NBUS_WIRTE_ARBIN,0x01);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0x01);
	cpu_iowr_16(NBUS_ARBINWR_RD,0);
	cpu_iowr_16(NBUS_WRITE_E_ADD,0);
	cpu_iowr_16(NBUS_WRITE_E_CON,0x03);    //Ĭ��Ϊ��    ADWRITE DOWIRTE��Ϊ��
	//cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x1F);    //Ĭ��Ϊ��
	cpu_iowr_16(NBUS_WRITE_DCS,0x1F);    //0x1FĬ��Ϊ��
	cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);    //Ĭ��Ϊ��    //commnet out chen    IA1 IA0 Ϊ��

	/*for(char i =0;i<16;i++)
	{
	IBT_relaystatus[i] = 0;    //����̵���״̬
	}*/
	memset((void *)(&IBT_relaystatus[0]), 0, MAXCHANNELNO);
	//NBUS_WRITE_DCS_status= 0x1F;    //20130218  chen

	//test
	//TIMERSFunc::DELAY_SYS(200);
	//static uint16 temp = cpu_iord_16(NBUS_WRITE_DCS);

	//end test


	////��ʼ��SPI SSP0 SSP1�ӿ�    //��gpio�ڽ���
	////spi gpio
	//{
	//	PINSEL0 |= 0xC0000000;    //P0.15 SCK
	//	PINSEL1 |= 0x0000000C;    //P0.17 MISO
	//	PINSEL1 |= 0x00000030;    //P0.18 MOSI
	//}
	////SSP gpio
	//{
	//	PINSEL3 |= 0x0003C300;    //P1.20 SCK0  P1.23 P1.24MISO MOSI
	//	PINSEL0 |= 0x000A8000;    //P0.7 8 9   SCK MISO MOSI
	//}


	/*// ι���ź�    comment out chen 20130104
	//{// ι���ź�
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}*/
	WD(200);  // ι���ź�

	for(char a_i = 0;a_i<MAXCHANNELNO;a_i++)
	{
		WriteDOBus(a_i,0x1000);    //ISO����
		IBT_relaystatus[a_i] = 0x1000;    //add 20130410
	}				

	//���ߺ��޸�SSP,SSP0������DAC��FRAM��SSP1������ADC��

	///////SPI
	//SSPI_Para IBT_SSP0DAC,IBT_SSP0ADC,IBT_SSP1;    //

	//SSP0 for DAC    AD5360
	IBT_SSP0DAC.uc_Module_Name = IBT_SSP0_DAC_NAME;    //name
	IBT_SSP0DAC.uc_Speed = 5;
	IBT_SSP0DAC.uc_CPOL = 0;
	IBT_SSP0DAC.uc_CPHA = 1;
	IBT_SSP0DAC.uc_MSTR = 1;    //master
	IBT_SSP0DAC.uc_BITS = 8;
	IBT_SSP0DAC.uc_LSBF = 0;
	IBT_SSP0DAC.st_CS.Type = Address_MOD;
	IBT_SSP0DAC.st_CS.Address = NBUS_WRITE_DCS;    //CPLD
	IBT_SSP0DAC.st_CS.Value = DCS3;

	//SSP0 for DAC_CLAMP  AD5754
	IBT_SSP0DAC_CLAMP.uc_Module_Name = IBT_SSP0_DAC_CLAMP;    //name
	IBT_SSP0DAC_CLAMP.uc_Speed = 5;
	IBT_SSP0DAC_CLAMP.uc_CPOL = 0;
	IBT_SSP0DAC_CLAMP.uc_CPHA = 1;
	IBT_SSP0DAC_CLAMP.uc_MSTR = 1;    //master
	IBT_SSP0DAC_CLAMP.uc_BITS = 8;
	IBT_SSP0DAC_CLAMP.uc_LSBF = 0;
	IBT_SSP0DAC_CLAMP.st_CS.Type = Address_MOD;
	IBT_SSP0DAC_CLAMP.st_CS.Address = NBUS_WRITE_DCS;    //CPLD
	IBT_SSP0DAC_CLAMP.st_CS.Value = DCS3;

	//SSP1 for ADI
	IBT_SSP1ADI.uc_Module_Name = IBT_SSP1_ADI_NAME;    //name
	IBT_SSP1ADI.uc_Speed = 5;
	IBT_SSP1ADI.uc_CPOL = 0;
	IBT_SSP1ADI.uc_CPHA = 0;       //rising trig
	IBT_SSP1ADI.uc_MSTR = 1;    //master
	IBT_SSP1ADI.uc_BITS = 8;
	IBT_SSP1ADI.uc_LSBF = 0;
	IBT_SSP1ADI.st_CS.Type = GP_MOD;
	IBT_SSP1ADI.st_CS.GPIO_PIN.P1 = 1;    //DCS1  ADC for I
	IBT_SSP1ADI.st_CS.GPIO_PIN.P2 = 21;
	//SSP1   for ADV
	IBT_SSP1ADV.uc_Module_Name = IBT_SSP1_ADV_NAME;    //name
	IBT_SSP1ADV.uc_Speed = 5;
	IBT_SSP1ADV.uc_CPOL = 0;
	IBT_SSP1ADV.uc_CPHA = 0;       //rising trig
	IBT_SSP1ADV.uc_MSTR = 1;    //master
	IBT_SSP1ADV.uc_BITS = 8;
	IBT_SSP1ADV.uc_LSBF = 0;
	IBT_SSP1ADV.st_CS.Type = GP_MOD;
	IBT_SSP1ADV.st_CS.GPIO_PIN.P1 = 0;    //DCS2  ADC for V
	IBT_SSP1ADV.st_CS.GPIO_PIN.P2 = 6;
	/*
	IBT_SSP0_FRAM.uc_Module_Name = IBT_SSP0_FRAM;    //name
	IBT_SSP0_FRAM.uc_Speed = 5;//5
	IBT_SSP0_FRAM.uc_CPOL = 0;
	IBT_SSP0_FRAM.uc_CPHA = 0;
	IBT_SSP0_FRAM.uc_MSTR = 1;    //master
	IBT_SSP0_FRAM.uc_BITS = 8;
	IBT_SSP0_FRAM.uc_LSBF = 0;
	IBT_SSP0_FRAM.st_CS.Type = GP_MOD;
	IBT_SSP0_FRAM.st_CS.GPIO_PIN.P1 = 0;    //CS  SSP0 for FRAM
	IBT_SSP0_FRAM.st_CS.GPIO_PIN.P2 = 14;
	*/
	//����SPI SSP����
	SSPIFunc::setPara(&IBT_SSP0DAC,0);
	SSPIFunc::setPara(&IBT_SSP0DAC_CLAMP,1);

	SSPIFunc::setPara(&IBT_SSP1ADI,2);
	SSPIFunc::setPara(&IBT_SSP1ADV,3);
	//��ʼ��SPI SSP

	SSPIFunc::my_Init(0);
	SSPIFunc::my_Init(1);
	SSPIFunc::my_Init(2);//����SSP1
	SSPIFunc::my_Init(3);


	TIMERSFunc::DELAY_SYS( 30000 );	//delay 5ms
	//���Ӧ���ڳ�ʼ��SPI SSP�ӿ�֮��
	/*//{//ι���ź�
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}*/
	WD(200);  // ι���ź�

	ADC_Init();
	DAC_Init();

	//���³�ʼ��IOΪ��������ģʽ
	ReInitPullUP();

	/*//ι���ź�
	//{// ι���ź�
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}*/
	WD(200);  // ι���ź�

	for(char i =0;i<MAXCHANNELNO;i++)
	{
		IBT_Base::Adjust_CHN_Gain(i,1);    //ʹ�õ���
		//IBT_Base::Adjust_CHN_Gain(1,1);    //ʹ�õ���
	}
	IBT_Base::Adjust_ADC_Gain(1,IBT_ADC_TYPE_I); // ʹ�õ���
	IBT_Base::Adjust_ADC_Gain(1,IBT_ADC_TYPE_V); // ʹ�õ���
}

/******************************************************************************
** Function name:		IO_Init
**
** Descriptions:	        INitialize typeA Gpio
**			     
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void IBT_Base::IO_Init(void)
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	//GPIOFunc::SetModuleIO();     //����MOD

	//SPI�ӿ�
	//GPIO_MODValue[][]���ú�ֵ
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount1, SSPIFunc::MyIO1, SPI_ENABLE);    //unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount2, SSPIFunc::MyIO2, SSP_ENABLE);
	//GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount3, SSPIFunc::MyIO3, SSP_ENABLE);


	IBT_GPIO::GPIO_Init();           //��ʼ��

	//OS_ServeProcess::IOCheck();     //��ʱ����check
} 
/******************************************************************************
** Function name:		ADC_Init
**
** Descriptions:	        INitialize typeA ADC
**			     
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void IBT_Base::ADC_Init(void)
{	
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	//��ʼ��P1.26
	//PINSEL3 &= 0xFFCFFFFF;
	//FIO1DIR |= ADC_CS5560_RST;    //IOΪ���
	//FIO1SET |= ADC_CS5560_RST;    //����Ϊ��
	GPIOFunc::InitPort(1, 26, PM_OUT, 1,0);     //ADC_CS5560_RST
	GPIOFunc::InitPort(1, 22, PM_OUT, 1,0);     //ADC_CS5560_RST
	//����
	//FIO1CLR |= ADC_CS5560_RST; 
	//TIMERSFunc::DELAY_SYS(200);
	//FIO1SET |= ADC_CS5560_RST;
	Adc16bit_CS5560::Adc_reset(0);
	TIMERSFunc::DELAY_SYS( 30000 );	//delay 5ms

	IBT_ADC5DAC::Adc_Init(0);
	IBT_ADC5DAC::Adc_Init(1);

	//PINSEL3 &= ~(0x03000000);    //CONV   P1.28    output
	GPIOFunc::InitPort(1, 28, PM_OUT, 1,0);    // 1.28 output high
	//PINSEL4 &= ~(0x03000000);    //BUSY1   P2.12    intput

	GPIOFunc::InitPort(2, 12, PM_IN, 0,0);    // 2.12 intput
	//PINSEL3 &= ~(0x00000C00);    //ADC_CS   DCS1  P1.21   output
	GPIOFunc::InitPort(1, 21, PM_OUT, 1,0);    // 1.21 output high

	//PINSEL4 &= ~(0x0C000000);    	//BUSY2    input    P2.13
	GPIOFunc::InitPort(2, 13, PM_IN, 0,0);  
	//PINSEL0 &= ~(0x00003000);    //ADC  DCS2    output  P0.6
	GPIOFunc::InitPort(0, 6, PM_OUT, 1,0); 

	////����
	//FIO1DIR |= (1<<28);        //1.28
	//FIO2DIR &= ~(1<<12);    //input  2.12
	//FIO1DIR |= (1<<21);        //1.21
	//FIO2DIR &= ~(1<<13);    //input 2.13
	//FIO0DIR |= (1<<6);    //P0.6

	////Ĭ��Ϊ��
	//FIO1SET |= (1<<28);        //1.28
	//FIO1SET |= (1<<21);        //1.21
	//FIO0SET |= (1<<6);    //P0.6

	TIMERSFunc::DELAY_SYS( 10000 );	//delay 5ms
}
/******************************************************************************
** Function name:		DAC_Init
**
** Descriptions:	        INitialize typeA DAC
**			     
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void IBT_Base::DAC_Init(void)
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	IBT_ADC5DAC::Dac_Init(0);
	//IBT_ADC5DAC::Dac_Init(1);

	//AD5360��ʼ������SPI��ʼ����ִ��
	//����reset ���    load1 ���
	//�Ҳ��벻��дpwoer-upҲ����    �ݲ����
	unsigned short SPI_Send_Buffer[3];
	SPI_Send_Buffer[0] = DAC_SP_FUN_REG_AD5360+0x01;
	SPI_Send_Buffer[1] = DAC_SP_FUN_POWER_UP_AD5360;
	SPI_Send_Buffer[2] = DAC_SP_FUN_POWER_UP_AD5360;

	// 	PINSEL2 &= ~(0x000000C0);    //DAC_LOAD1 GPIO  p1.3
	// 	PINSEL3 &= ~(0x000C0000);    //DAC_RST GPIO    p1.25
	// 	PINSEL2 &= ~(0x00000C00);    //DAC_CLR GPIO  p1.5
	// 	PINSEL2 &= ~(0x00000030);    //DAC_BUSY3 GPIO  p1.2
	LPC_IOCON->P1_3 &= ~(0x00000007);
	LPC_IOCON->P1_25 &= ~(0x00000007);
	LPC_IOCON->P1_5 &= ~(0x00000007);
	LPC_IOCON->P1_2 &= ~(0x00000007);
	//����Ϊ���
	GPIOFunc::InitPort(1, 5, PM_OUT, 1,0);  //DAC_CLR SET 1
	GPIOFunc::InitPort(1, 2, PM_IN, 0,0);  
	LPC_GPIO1->DIR |= DAC_LOAD1;
	LPC_GPIO1->DIR |= DAC_RST;
	LPC_GPIO1->CLR |= DAC_LOAD1;           //dac_load1  CLR

	LPC_GPIO1->SET |= DAC_RST;    //RST�ø�
	ARBUS_DELAY(100);
	LPC_GPIO1->CLR |= DAC_RST;    //RST��
	ARBUS_DELAY(100);
	LPC_GPIO1->SET |= DAC_RST;     //rising edge����
	ARBUS_DELAY(10000);    //300us reset time  AD5360����
	//reset ���������Ҫ300us  Ȼ��CLRX M C�Ĵ���ȫ���ָ�Ĭ��

	//���power up    ??comment out chen 2012 11 29
	//����DCS3
	//�������SPI0
	//����DCS3
	//ʹ��
	//������powerҲ������������
	// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) &= ~(DCS3) ;
	// 	
	//	spi_2468::C_RxTx_Frames(SPI_Send_Buffer,3,0);
	SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,0);
	// 		
	// 	//clear ʹ��
	// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) |= DCS3 ;

}


//software init
/******************************************************************************
** Function name:		Software_Init
**
** Descriptions:	        INitialize type_A::Func_Init/type_A::Driver_Init
**			     
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void IBT_Base::Software_Init(void)
{
	Func_Init();//����ָ���ʼ��  �ĸ���������
	EngineStuff();   //����ṹָ���ʼ��

	for(unsigned char i = 0; i < board_config::st_board_info.Channel_Number; i++)    //��0��ʼ
	{
		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//ע��chn info
		regEngine(i,board_config::st_channel_info[i].Channel_type);//ע������			
	}

	IBT_SampleManage::Init();

}

/******************************************************************************
** Function name:		Func_Init
**				
** Descriptions:	        INitialize  type_A::Func_Adc_rd/Func_Dac_out/
** 															      Func_Relay_on/Func_Relay_off
**			     
** parameters:			none
** Returned value:  none
** 
******************************************************************************/
void IBT_Base::Func_Init(void)
{
	board_config::Func_Adc_rd=&IBT_Base::ADC_rd;
	board_config::Func_Dac_out=&IBT_Base::DAC_out;
	board_config::Func_Relay_on=&IBT_Base::Relay_on;
	board_config::Func_Relay_off=&IBT_Base::Relay_off;
}

//commnet out chen 20130216
void IBT_Base::Engine(void)
{
	OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToCur.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	//û�ж���
	OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &IBT_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlRstToOff.stepLen = IBT_Base::SetRstToOff_Len;
	OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &IBT_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = IBT_Base::SetRstToOff_Len;

	OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];    //ָ������0
	OS_LogicProcess::SetCtrlOffToCur.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &IBT_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = IBT_Base::SetCCToCC_Len;

	//����������
	OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &IBT_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = IBT_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &IBT_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToOff.stepLen = IBT_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &IBT_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToRst.stepLen = IBT_Base::SetCtrlCloseCh_Len ;

	//
	OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &IBT_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = IBT_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &IBT_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = IBT_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &IBT_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = IBT_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &IBT_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = IBT_Base::SetCtrlCloseCh_Len ;

	//SetCtrlRstToOff.st_Disposabletask_ptr = &IBT_Base::SetCtrlCloseCh[0];
	//SetCtrlRstToOff.stepLen = IBT_Base::SetCtrlCloseCh_Len ;
}

/******************************************************************************
** Function name:		regCHNInfo
**				
** Descriptions:	  INitialize
**			     
** parameters:			chn,type
** Returned value:  none
** 
******************************************************************************/
void IBT_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
{

}

/******************************************************************************
** Function name:		regEngine
**				
** Descriptions:	  
**			     
** parameters:			none
** Returned value:  none
** 
******************************************************************************/
void IBT_Base::regEngine(unsigned char CH_Number,unsigned long type)
{

}

/******************************************************************************
** Function name:		EngineStuff
**				
** Descriptions:	  
**			     
** parameters:			none
** Returned value:  none
** 
** �ֹ�������ʱ��
******************************************************************************/
void IBT_Base::EngineStuff(void)
{
	//---------------------------------------------//
	SetCtrlOpenCh[0].uw32_DelayTime = 100;    //�Ӵ��ӳ٣���ֹ�̵�����AD8253����  20130227  comment out chen
	SetCtrlOpenCh[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //|= ACTIVE_OUT_RELAY;    //test
	//SetCtrlOpenCh[0].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //������̵̼���    20130227
	SetCtrlOpenCh[0].DacMove.DacFlag |= ACTIVE_DAC_CUR_ZERO;
	SetCtrlOpenCh[0].RangeChangedFlag = 0;
	SetCtrlOpenCh[1].uw32_DelayTime = 0;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //���ֿ���ͨ��״̬
	SetCtrlOpenCh[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //������̵̼���
	SetCtrlOpenCh[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCtrlOpenCh[1].RangeChangedFlag = 0;    //��ͨ��Ҫ���ǵ�������

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 10;
	SetCtrlCloseCh[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlCloseCh[0].RangeChangedFlag = 0;
	SetCtrlCloseCh[1].uw32_DelayTime = 0;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = ACTIVE_V_I_R;    //�򿪵�����ѹ�˼��
	SetCtrlCloseCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlCloseCh[1].RangeChangedFlag = 0;

	//---------------------------------------------//
	//comment out chen 20130128  ��ʱû�м���ٵ�ADC�л�����
	SetCCToCC[0].uw32_DelayTime = 10;
	SetCCToCC[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //����ͨ��    //debug comment out chen 20130107
	SetCCToCC[0].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	//�����п����ǹص�
	SetCCToCC[0].RangeChangedFlag = 0;
	SetCCToCC[1].uw32_DelayTime = 0;
	SetCCToCC[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //����ͨ��    //debug comment out chen 20130107
	SetCCToCC[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCCToCC[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //ֱ�������һ����
	SetCCToCC[1].RangeChangedFlag = 1;

	//---------------------------------------------//
	SetRstToOff[0].uw32_DelayTime = 10;
	SetRstToOff[0].RelayMove.ActiveFlag |= ACTIVE_V_I_R;
	SetRstToOff[0].DacMove.DacFlag = 0;
	SetRstToOff[0].RangeChangedFlag = 0;

	SetRstToOff[1].uw32_DelayTime = 0;
	SetRstToOff[1].RelayMove.ActiveFlag |= ACTIVE_V_I_R;
	SetRstToOff[1].DacMove.DacFlag = 0;
	SetRstToOff[1].RangeChangedFlag = 0;

	SetOpenISO[0].uw32_DelayTime = 10;
	SetOpenISO[0].RelayMove.ActiveFlag |= ACTIVE_V_I_R;
	SetOpenISO[0].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetOpenISO[0].RangeChangedFlag = 0;

	SetOpenISO[1].uw32_DelayTime = 0;
	SetOpenISO[1].RelayMove.ActiveFlag = ACTIVE_V_I_R;
	SetOpenISO[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetOpenISO[1].RangeChangedFlag = 0;

	SetCloseISO[0].uw32_DelayTime = 10;
	SetCloseISO[0].RelayMove.ActiveFlag |= ACTIVE_V_I_R;
	SetCloseISO[0].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCloseISO[0].RangeChangedFlag = 0;

	SetCloseISO[1].uw32_DelayTime = 0;
	SetCloseISO[1].RelayMove.ActiveFlag = 0;
	SetCloseISO[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCloseISO[1].RangeChangedFlag = 0;

	SetCVToCV[0].uw32_DelayTime = 10;
	SetCVToCV[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);
	SetCVToCV[0].DacMove.DacFlag  |= ACTIVE_DAC_CUR;
	SetCVToCV[0].RangeChangedFlag = 0;

	SetCVToCV[1].uw32_DelayTime = 0;
	SetCVToCV[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //����״̬
	SetCVToCV[1].DacMove.DacFlag  |= ACTIVE_DAC_CUR;
	SetCVToCV[1].RangeChangedFlag = 0;

}

//end init


/******************************************************************************
** Function name:		ADC_rd
**
** Descriptions:	        INitialize typeA Gpio
**			     
** parameters:			CH_Number:0->Max Channel Number; type: short   EN_ADC_Current/EN_ADC_Voltage
** Returned value:		0/-1
** 
******************************************************************************/
char IBT_Base::ADC_rd(unsigned long CH_Number,unsigned long type)
{
#if DEMO_MODE == 1
	return 0;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	return 0;
}

/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char IBT_Base::DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type)
{
	char chn = (char)(CH_Number);
	unsigned long value = Data;    //ǿ�Ʊ���
	unsigned long temp = 0;
	//comment out chen 20130104

#if DEMO_MODE == 1
	return 0;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif	

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		//����DAC�������ƣ��Թ�������ms����ˢ��    comment out chen 20130114
		ChannelInfo_Manager::ChannelInfo[chn].m_u32DacValue = value;    //��ֵΪƫ����

		//��Ϊ����Ŀ���  ������Ҫ����
		if( value > 0x800000 )    //����
		{
			temp = value - 0x800000;
			value = 0x800000 - temp;
		}
		else                                           //����
		{
			temp = 0x800000 - value;
			value = 0x800000 + temp;
		}
		//comment out chen 20130228  ���뱣������
		if(value > 0xFFFFFF)     //0x1000000
		{
			value = 0xFFFFFF;    //��ֹ����  0xFFFFFF
		}

		IBT_ADC5DAC::Dac_out_Cur(chn,value);
		break;
	case ACTIVE_DAC_VLT:
		IBT_ADC5DAC::Dac_out_Vlt(chn,value);
		break;
	case ACTIVE_DAC_VH:
		IBT_ADC5DAC::Dac_out_Clamp_Vlt(chn,value);
		break;
	default:
		break;
	}
	return 1;
}

/******************************************************************************
** Function name:		Relay_on
**
** Descriptions:	         typeA Relay on move
**			     
** parameters:			CH_Number:0->Max Channel Number; type: Relay type
** Returned value:		0/1
** 
******************************************************************************/
char IBT_Base::Relay_on(unsigned long CH_Number,unsigned long type)
{
	unsigned char chn = CH_Number;
	unsigned short temp = IBT_relaystatus[chn];    

	switch(type)
	{
	case ACTIVE_VOL_RANGE:    //X_VIN
		temp |= 0x0800;
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;
	case ACTIVE_V_I_R:              //I/S/O
		temp |= 0x1000;
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;
	case ACTIVE_OUT_RELAY:     //X_CHL
		temp |= 0x8000;
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;

		//comment out chen 20130128    �����˼ٵ��л�ʱ����
	case ACTIVE_CUR_RELAY_HIGH:
		//�л�����
		changeADCRange(chn,CURRENT_RANGE_HIGH);
		return 1;
	case ACTIVE_CUR_RELAY_MIDDLE:
		//�л�����
		changeADCRange(chn,CURRENT_RANGE_MED);
		return 1;
	case ACTIVE_CUR_RELAY_LOW:
		//�л�����
		changeADCRange(chn,CURRENT_RANGE_LOW);
		return 1;

	default:
		return 0;
		//break;
	}
	//return 0; 

}

/******************************************************************************
** Function name:		Relay_off
**
** Descriptions:	       typeA Relay off move
**			     
** parameters:			CH_Number:0->Max Channel Number; type: Relay type
** Returned value:		0/1
** 
******************************************************************************/
char IBT_Base::Relay_off(unsigned long CH_Number,unsigned long type)
{
	unsigned char chn = CH_Number;
	unsigned short temp = IBT_relaystatus[chn];    

	switch(type)
	{
	case ACTIVE_VOL_RANGE:    //X_VIN
		temp &= ~(0x0800);
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;
	case ACTIVE_V_I_R:              //I/S/O
		temp &= ~(0x1000);
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;
	case ACTIVE_OUT_RELAY:     //X_CHL
		temp &= ~(0x8000);
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //��¼
		return 1;
		//break;
	default:
		return 0;
		//break;
	}
	//return 0;
}


//����DO�������ʱ��    ���Ƶ���relay
/*
1 ARBINDIR  д
2 ARBINWR  YD 0-15  �̵�������
3 ARBIN  ����
4 ADD 0-4  ѡ��ͨ��
5 �ȴ�Yi���
6 DOWRITE  ����
7 YD 0-15���  ���Ƽ̵���
*/
//2012 11 29  comment out chen    is work
void IBT_Base::WriteDOBus(char chn,uint16 BD)
{
	if(chn >= MAXCHANNELNO)
		return;
	UWord32 ENback[2];
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	//Disable interrupts for now
	ENback[0] = NVIC->ISER[0];             // enable interrupt  //
	ENback[1] = (NVIC->ISER[1] & 0X01FF);   // enable interrupt  //
	NVIC->ICER[0] = 0XFFFFFFFF;                    // disable interrupt //
	NVIC->ICER[1] = 0X01FF;                    // disable interrupt //

	//  uint32 ENback = VICIntEnable;    //keep the old interrupt status
	// VICIntEnClr = 0xFFFFFFFF;    //disable 

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR����  ��סXD����
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD, BD );    //���BD    //��ʱ���ݱ���ס
	//*(volatile unsigned short *)(NBUS_ARBINWR_RD) |= (BD) ; 
	ARBUS_DELAY(5);

	//ѡ��ͨ��
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn);  //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
	//������ѡ��ͨ���������
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
	ARBUS_DELAY(5);    //��delay

	//Yi����XADD���

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE����    ADWRITEΪ��
	ARBUS_DELAY(5);    //��delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE����    ADWRITEΪ��

	//Yi��DOWRITE�����DOCS
	ARBUS_DELAY(5);

	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);    //����ARBIN
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����     //2012 11 29  comment out chen

	////////////////////////////////////////////////////////////////////
	NVIC->ISER[0] = ENback[0];    //enable interrupt
	NVIC->ISER[1] = ENback[1];
}

//�˲�����ʱû���
//comment out chen 20130216
unsigned short IBT_Base::ReadDOBus(void)
{

#if DEMO_MODE == 1
	return 1;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	unsigned short temp = 0;

	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0&NBUS_ARBINDIR);    //����DIR
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //����DIR
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD,0x00);  //����
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD,0x01);  //����
	//��סBD

	temp = cpu_iord_16(NBUS_ARBINWR_RD);
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //����

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //������DIR

	return temp;
}

/*
//����������DO  ���Ƶ��ǿɱ�̷Ŵ���    0x8100 000A write PGA gain
void IBT_Base::WritePGABus(char chn,uint16 PGABD)
{
if(chn > 16)
return;

//	// Disable interrupts for now
uint32 ENback = VICIntEnable; //keep the old interrupt status
VICIntEnClr = 0xFFFFFFFF; //disable 

ARBUS_DELAY(5);
cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR����
cpu_iowr_16(NBUS_WRITE_PGA_GAIN, PGABD );    //���BD
ARBUS_DELAY(5);
cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����

//������ѡ��ͨ���������
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����
//ѡ��ͨ��
cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //ͨ��
ARBUS_DELAY(10);    //��delay
//Yi���
cpu_iowr_16(NBUS_WRITE_E_CON, 0&NBUS_DOWRITE);    //DOWRITE����
ARBUS_DELAY(10);    //��delay
cpu_iowr_16(NBUS_WRITE_E_CON, 0|NBUS_DOWRITE);    //DOWRITE����

//YD���
ARBUS_DELAY(5);
//����ARBIN
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 

////////////////////////////////////////////////////////////////////
VICIntEnable = ENback; //enable interrupt
}


//ADC����ʱ��
void IBT_Base::WriteADCBus(char chn)
{
////	// Disable interrupts for now
//uint32 ENback = VICIntEnable; //keep the old interrupt status
//VICIntEnClr = 0xFFFFFFFF; //disable 

cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����
//ѡ��ͨ��
cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //ͨ��
ARBUS_DELAY(10);    //��delay
//Yi���
cpu_iowr_16(NBUS_WRITE_E_CON, 0&NBUS_DOWRITE);    //DOWRITE����
ARBUS_DELAY(10);    //��delay
cpu_iowr_16(NBUS_WRITE_E_CON, 0|NBUS_DOWRITE);    //DOWRITE����

//ADCS����
cpu_iowr_16(NBUS_WRITE_DCS,0);
ARBUS_DELAY(10);    //��delay
//����ARBIN
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 

//ADCS����

//ADRESET����

//////////////////////////////////////////////////////////////////////
//VICIntEnable = ENback; //enable interrupt
}
*/


//////////////////////////////////////////////////////////comment out chen 2012 12 24 ����ADC CS5560 ����24bit��adc//////////////////////////////////////////////////////////
////����ֱ��ȡI V
////I ssp0    V ssp1    ͬʱ��Ҫ����ad9253  ad9251��������ſɶ�ȡ
////��Ҫ����ADC����ͨ��ʱ��
//unsigned long int CS5560_Read(char chn,char IVtype)    //�ڶ�������Ԥ�� ����������Ҫ
//{
//	/*
//	//����˳��
//    //1 ���ƿɱ�̷Ŵ������ɱ�����  IA0  IA1  VA1  VA2  //�˲�����  �½�����  �����ɱ�̷Ŵ�������  
//	//2 ADC���ز���  ��ʱ�Ѿ���ѡͨ��ͨ��
//	//3 ��ȡADC  Ҳ����˵����
//	//4 ADRESET
//	//comment out chen 2012 12 03
//	*/
//    
//	//�˲�����
//	////1  ///////////////////////////just for AD8253/////////////////////////////
//	////	// Disable interrupts for now
//	//// 	uint32 ENback = VICIntEnable; //keep the old interrupt status
//	//// 	VICIntEnClr = 0xFFFFFFFF; //disable 
//	//unsigned char BD = 0x12;    //WR+A0+A1  ��ʱ��BD for ���ƿɱ�̷Ŵ���10010B  comment out chen ��ʱֻ�Ǽ�test
//
//	//ARBUS_DELAY(5);
//	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR����  д  ��סXD����
//	////cpu_iowr_16(NBUS_ARBINWR_RD, BD );    //���BD    //��ʱ���ݱ���ס
//	//*(volatile unsigned short *)(NBUS_ARBINWR_RD) |= BD;    //�����д�̵���
//	//ARBUS_DELAY(5);
//
//	////������ѡ��ͨ���������
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����
//	////ѡ��ͨ��
//	//cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
//	////������ѡ��ͨ���������
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
//	//ARBUS_DELAY(5);    //��delay
//	////Yi���
//	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_DOWRITE);   //DOWRITE����
//	//ARBUS_DELAY(5);    //��delay
//	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_DOWRITE) ;   //DOWRITE����
//
//	////YD���
//	//ARBUS_DELAY(5);
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); //����ARBIN
//	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����     //2012 11 29  comment out chen
//	//////////////////////////////////////////////////////////////////////
//	////VICIntEnable = ENback; //enable interrupt
//	////1  ///////////////////////////end of just for AD8253/////////////////////////////
//
//
//// 	//2  ///////////////////////////ADC����/////////////////////////////
//// 	//ѡ��ͨ��
//// 	cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
//// 	//������ѡ��ͨ���������
//// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
//// 	ARBUS_DELAY(5);    //��delay
//// 	//Yi���
//// 	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_ADWRITE);   //ADWRITE����
//// 	//ARBUS_DELAY(5);    //��delay
//// 	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_ADWRITE) ;   //ADWRITE����
//// 	cpu_iowr_16(NBUS_WRITE_E_CON, 0x01);    //DOWRITEΪ��    ADWRITE����
//// 	ARBUS_DELAY(5);    //��delay
//// 	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITEΪ��    ADWRITE����
////     //comment out chen 2012 12 04
//// 	
//// 	//AD+Yi -> ADCS    ��ʱ���ݱ�����
//// 	ARBUS_DELAY(5);
//// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 
//// 	//����ARBIN   ������ʱ���߾���˵U17��SN74HC154DWһֱ���ָ�״̬  ֱ����һ��OEʹ�ܱ��ı�
////     //2  ///////////////////////////end of ADC����/////////////////////////////
//
//	//3  ///////////////////////////��ȡ ADC/////////////////////////////
//	//����CONV
//	FIO1CLR |= ADC_CONV;    //����
//	ARBUS_DELAY(5);
//	FIO1SET |= ADC_CONV;    //����
//	//�ȴ�READY
//	while(FIO2PIN & BUSY1);    //wait busy pin
//	//����ssp�ӿ�����ȡ����
//	unsigned short Tx[3],Rx[3];
//	Tx[0] = 0;    //���ͳ�0����
//	Tx[1] = 0;
//	Tx[2] = 0;
//	//FIO1CLR |= DCS1;    //����CS
//	SSPIFunc::RxTx_Frames_1CS(Tx,3,1);    //�������������� spi��Rx��������    //��������  ����  ͨ��
//	//FIO1SET |= DCS1;
//	for(char i =0;i<3;i++)
//	{
//		Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //��������
//	}
//	//spi_2468::SSP_RxTx_Frames(unsigned short *arry,unsigned char len,unsigned char chn);
//	//3  ///////////////////////////end of ��ȡ ADC/////////////////////////////
//	
//	//�л�����ֵ
//	ARBUS_DELAY(200);
//	return;
//
//// 	//4  ///////////////////////////ADC RESET/////////////////////////////
////     //BUS1
//// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) &= ~(NBUS_BUS1);   //����
//// 	ARBUS_DELAY(5);
//// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) |= NBUS_BUS1;   //����
//// 	//4  ///////////////////////////end of ADC RESET/////////////////////////////
//}


//����ͨ�����ϵ�adc gain
//�������ʱһ����Ҫ�����ǰrelay��״̬������relay���ر�
//WR�½��ز�д��
void IBT_Base::Adjust_CHN_Gain(unsigned char chn,unsigned long type)    
{
	// 1  ///////////////////////////just for AD8253/////////////////////////////
	//	// Disable interrupts for now
	// 	uint32 ENback = VICIntEnable; //keep the old interrupt status
	// 	VICIntEnClr = 0xFFFFFFFF; //disable 

#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	unsigned short BD1,BD2;
	unsigned short temp;
	//unsigned char chn_num = chn;
	if(type == 1)
	{
		BD1 = 0x0010;
		BD2 = 0x0003;
	}
	else if(type == 10)
	{
		ARBUS_DELAY(50);
		BD1 = 0x0011;
		BD2 = 0x0002;
	}
	else if(type == 100)
	{
		ARBUS_DELAY(50);
		BD1 = 0x0012;    //WR+A0+A1  ��ʱ��BD for ���ƿɱ�̷Ŵ���10010B  comment out chen ��ʱֻ�Ǽ�test
		BD2 = 0x0001;
	}
	temp = IBT_relaystatus[chn];    //�Ӽ̵�����ȡ�ص�ǰ״̬�������д�̵���    //is it right?    
	temp |= BD1;
	temp &= ~(BD2);
	IBT_relaystatus[chn] = temp; 
	//׼������ temp

	//ʹ��WirteDOBus����ķ�ʽ
	//WriteDOBus(chn,temp);
	//ARBUS_DELAY(5);
	//temp &= 0xFFEF;    //����WR
	//WriteDOBus(chn,temp);
	//IBT_relaystatus[chn] = temp; 
	//return;

	//Disable interrupts for now
	IRQFunc::All_DisableIRQ();

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR����  д  ��סXD����  ��Ϊ���ʹ�ܣ������ʹ
	//MC74HC574�����Ч����������
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD, temp);    //���BD    //��ʱ���ݱ���ס
	ARBUS_DELAY(5);
	//WR�Զ����
	//XD��ʱ���temp

	//ѡ��ͨ��
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn);    //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
	//������ѡ��ͨ���������
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi    //MC74HC541��ΪG1G2���ͣ�ֱ�������
	ARBUS_DELAY(5);    //��delay  ��ʱYiͨ����Yi���
	//Yi���

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE����    ADWRITEΪ��
	ARBUS_DELAY(10);    //��delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE����    ADWRITEΪ��    MC74HC574������д��
	//Yi��DOWRITE���DOCS  ����DOCS
	ARBUS_DELAY(5);

	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR����  д  ��סXD����
	//������YD��� �Ա�ʹWR�����½���
	ARBUS_DELAY(5);
	temp &= 0xFFEF;    //����WR
	cpu_iowr_16(NBUS_ARBINWR_RD, temp);  //�����������������
	ARBUS_DELAY(5);
	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����     //2012 11 29  comment out chen

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE����    ADWRITEΪ��
	ARBUS_DELAY(10);    //��delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE����    ADWRITEΪ��
	ARBUS_DELAY(5);
	//�ٴδ���DOCS

	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); //����ARBIN  ֹͣYI���
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����     //2012 11 29  comment out chen
	////////////////////////////////////////////////////////////////////
	//enable interrupt
	IRQFunc::All_EnableIRQ();
	// 1  ///////////////////////////end of just for AD8253/////////////////////////////

	IBT_relaystatus[chn] = temp; 

	//WriteDOBus(chn,temp);
	//ARBUS_DELAY(5);
	//temp &= 0xFFEF;    //����WR
	//WriteDOBus(chn,temp);
	ARBUS_DELAY(5);
}

//����adc���ϵ�gain
void IBT_Base::Adjust_ADC_Gain(char type,char chnType)
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	if(chnType == IBT_ADC_TYPE_I)
	{
		if(type == 1)    //����
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
	else if(chnType == IBT_ADC_TYPE_V)
	{
		if(type == 1)    //����
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
}


//ADCģ���Ŵ�
void IBT_Base::ADC_ADG_open(char chn) 
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif
	///////////////////////////ADC����/////////////////////////////
	//ѡ��ͨ��
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn );    //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
	//������ѡ��ͨ���������
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
	ARBUS_DELAY(5);    //��delay
	//Yi���
	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_ADWRITE);   //ADWRITE����
	//ARBUS_DELAY(5);    //��delay
	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_ADWRITE) ;   //ADWRITE����
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x01);    //DOWRITEΪ��    ADWRITE����
	ARBUS_DELAY(5);    //��delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITEΪ��    ADWRITE����
	//comment out chen 2012 12 04

	//AD+Yi -> ADCS    ��ʱ���ݱ�����
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 
	//����ARBIN   ������ʱ���߾���˵U17��SN74HC154DWһֱ���ָ�״̬  ֱ����һ��OEʹ�ܱ��ı�
}

//ADreset
void IBT_Base::ADC_Reset(void)
{
#if DEMO_MODE == 1
	return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

	//20130218  chen
	uint16 temp = *(volatile unsigned short *)(NBUS_WRITE_DCS);
	temp &= ~(NBUS_BUS1);   //����
	*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;
	ARBUS_DELAY(5);
	temp |= NBUS_BUS1;
	*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;    //����

	////����    comment out chen 20130131
	//   uint16 temp = IBT_Base::NBUS_WRITE_DCS_status;    //ȡ��״̬
	//   temp &= ~(NBUS_BUS1);   //����
	//*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;
	//ARBUS_DELAY(5);
	//*(volatile unsigned short *)(NBUS_WRITE_DCS) = IBT_Base::NBUS_WRITE_DCS_status;    //�ָ�
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////                                                  for ont time                                                  ///////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Relay On(chn,type)
Relay Off(chn,type)
Dac_out(chn,value)    
Dac_clamp_out();
Adc_read(chn)
PGA_Gain()�������� ������������    ����������    1,10,100,1000

Relay ������
type������
��ѹ�����л�         ACTIVE_VOL_RANGE
�����˵�ѹ����      ACTIVE_V_I_R
����̵���            ACTIVE_OUT_RELAY

relay����ʱ����Ϊ�˰�����ʮ�����⣬��Ҫ�и�relay_status����¼����״̬
volatile unsigned short IBT_relaystatus;

PGA_Gain change
1
10
100
1000

----------------------------------------------------------
Dac_out
���ͨ��
----------------------------------------------------------
Dac_clamp_out
ǯѹ������˰�ǯѹ����ǹ��е�
----------------------------------------------------------
Adc_read
�Ƚ�����


��Ȼ��ǰ��Disposable(OneTime)�������������

*/


//���ⲿ��ת�Ƶ�IBT_ADC5DAC��ȥ
//comment out chen 20130216
/*
//��valueΪƫ����
void IBT_Base::Dac_out_Cur(char chn,unsigned long value)
{
//AD5360_DACoutΪ�����룬��Ҫת��
//��ƫ����ת��������
//��һλȡ��
//unsigned short temp = value;
//value>>15;
//value~=value;    //��һλȡ��
//value<<15;
//temp &= ~(0x8000);    //��һλ��0
//temp |= value;

//����DAC�������ƣ��Թ�������ms����ˢ��    comment out chen 20130114
ChannelInfo_Manager::ChannelInfo[chn].m_u32DacValue = value;    //��ֵΪƫ����

unsigned long temp = value;

if( (value & 0x800000) == 0x800000)    //�жϵ�һλ�Ƿ�Ϊ1    ��һλȡ��
{
temp &= ~(0x800000);
}
else
{
temp |= 0x800000;
}

//���SPI�ӿ�
unsigned char bOK = SSPIFunc::getModuleName(0);
if(bOK  != IBT_SPI_MODULE_NAME)
{
//���³�ʼ��
//SPI

S0SPCR = 0x00;    //��λSPI
S0SPDR = 0x0000;
SSPI_Para IBT_SPI;
IBT_SPI.uc_Module_Name = IBT_SPI_MODULE_NAME;    //name
IBT_SPI.uc_Speed = 5;
IBT_SPI.uc_CPOL = 0;
IBT_SPI.uc_CPHA = 0;
IBT_SPI.uc_MSTR = 1;    //master
IBT_SPI.uc_BITS = 8;
IBT_SPI.uc_LSBF = 0;
IBT_SPI.st_CS.Type = Address_MOD;
IBT_SPI.st_CS.Address = NBUS_WRITE_DCS;    //CPLD
IBT_SPI.st_CS.Value = DCS3;

SSPIFunc::setPara(&IBT_SPI,0);    //���ò���
SSPIFunc::my_Init(0);              //��ʼ��
}

ARBUS_DELAY(10);
DAC16bit_AD5360::Dac_out(chn,temp);    //ת��Ϊ����������

}

//��Ϊpid��ѹ
void IBT_Base::Dac_out_Vlt(char chn,unsigned long value)
{

}

//clamp voltage
void IBT_Base::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{

}

*/


//�ƶ���SampleMagnage.cpp
/*
//����ADC     ���ҵ�������
//ProcessADC������һ��FSM
//  ����1 ADC_ADG_open  2 10us  3 CS5560_Read while busy 30us  4 ADC_Reset�ر�
//ʵ���� һ������ ��I��V�� һ��chn  �˰�����������ǲ������ڽ���  ��Ϊ�����´򿪵�����
//�����ѹ����    20130109    comment out chen
char IBT_Base::ProcessADC(char adc_device_chn)
{
uint32 temp = 0;
uint32 temp_for_inverse = 0;
static uint32 temp_V = 0;
//��һ��I  �ٶ�һ��V
static char status = IBT_ADC_NO_OPEN;
static unsigned long conter = 0;

if(adc_device_chn == 0)    //I
{
switch(status)
{
case IBT_ADC_NO_OPEN:
ADC_ADG_open(I_chn);    //��
return 0;    //not ready
case IBT_ADC_OPENING:
return 0;
case IBT_ADC_OPEN:
Adc16bit_CS5560::CS5560_CONV();
return 0;
case IBT_ADC_READY:
temp = Adc16bit_CS5560::CS5560_Read(I_chn,IBT_ADC_TYPE_I);    //value������λ
temp_V = Adc16bit_CS5560::CS5560_Read(I_chn,IBT_ADC_TYPE_V);     //��ѹ
//�ϵ��ڴ�

//��Ϊ����Ŀ���  ������Ҫ����
if( temp>0x800000 )    //����    24bit
{
temp_for_inverse = temp - 0x800000;
temp = 0x800000 - temp_for_inverse;
}
else                                           //����
{
temp_for_inverse = 0x800000 - temp;
temp = 0x800000 + temp_for_inverse;
}
chn = I_chn;
type = SAMPLE_PROCESS_ADC_I;    //comment out chen 20130104
value = temp;    //

I_chn++;    //debug comment out chen 20130107
if(I_chn == IBT_ADC_I_CHN_MAX)    //
{
I_chn = 0;
}
return SAMPLE_PROCESS_SIGMA_DELTA_ADC;    //����  Ϊdelta-sigma ADC
}
}
else if(adc_device_chn == 1)    //V  ����
{
switch(status)
{
case IBT_ADC_NO_OPEN:
status = IBT_ADC_OPENING;
return 0;
case IBT_ADC_OPENING:
//����ʱ��
conter++;
if(conter == IBT_ADC_DG_BUILT_UP_TIME_TICKS)    //����ʱ�䲻��
{
status = IBT_ADC_OPEN;
conter = 0;
}
return 0;
case IBT_ADC_OPEN:
status = IBT_ADC_READY;    //conv
return 0;
case IBT_ADC_READY:
chn = V_chn;
type = SAMPLE_PROCESS_ADC_V;
value = temp_V;

V_chn++;
if(V_chn == IBT_ADC_I_CHN_MAX)    //
{
V_chn = 0;
}

//�ر�
ADC_Reset();
status = IBT_ADC_NO_OPEN;    //status_I = IBT_ADC_NO_OPEN;
return SAMPLE_PROCESS_SIGMA_DELTA_ADC;
}
}

return -1;    //error
}


char IBT_Base::getCHN(char adc_device_chn)
{
return chn;
}

//���ص�������
unsigned long int IBT_Base::getValue(char adc_device_chn)
{
return value;
}

char IBT_Base::getType(char adc_device_chn)
{
return type;
}
*/


//for Relay_on  �����˼ٵ��л�����
//comment out chen 20130216
void IBT_Base::changeADCRange(char chn,unsigned char range)
{
	//TIMERSFunc::DELAY_SYS(10000);
	ARBUS_DELAY(10);
	//�л�����
	switch(range)
	{
	case CURRENT_RANGE_HIGH:
		IBT_Base::Adjust_CHN_Gain(chn,1);    // 1��
		break;
	case CURRENT_RANGE_MED:
		IBT_Base::Adjust_CHN_Gain(chn,10);    // 10��
		break;
	case CURRENT_RANGE_LOW:
		IBT_Base::Adjust_CHN_Gain(chn,100);    // 100��
		break;
	default:
		IBT_Base::Adjust_CHN_Gain(chn,1);    // Ĭ��1��
		break;			
	}

}

void IBT_Base::WD(UWord32 uw32_DelayTime)
{
	/*	//IBT watch dog    comment out chen
	//{
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(100);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}

	// ι���ź�
	//{// ι���ź�
	FIO3SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low��ƽ
	//}*/

	LPC_GPIO3->SET |= 0x00040000;    //P3.18 �ߵ�ƽ
	TIMERSFunc::DELAY_SYS(uw32_DelayTime);
	LPC_GPIO3->CLR |= 0x00040000;    //P3.18 low��ƽ
}

//get ���뿪��    �����ON�Ļ������Ǹ�λΪ1��OFFΪ0
//comment out chen 20130221 ����
unsigned char IBT_Base::getBID(void)
{
	//BS0-BS7    P3.24-P3.31
	unsigned char bID[8][2] = 
	{
		{3,24},
		{3,25},
		{3,26},
		{3,27},
		{3,28},
		{3,29},
		{3,30},
		{3,31},
	};
	for(unsigned char i = 0;i<8;i++)
	{
		GPIOFunc::InitPort(bID[i][0], bID[i][1], PM_IN, 0,0);    //���ó�ΪINPUT
	}
	//GPIOFunc::InitPort(2, 13, PM_IN, 0,0);  

	UWord32 get_Value = LPC_GPIO3->PIN;    //BS0-BS7    P3.23-P3.31
	get_Value >>= 24;

	return (unsigned char)get_Value;
}


//���³�ʼ��IOΪ��������ģʽ
void ReInitPullUP(void)
{
	unsigned char  uc_Bit;
	UWord32 * This_P = NULL;
	This_P = (UWord32 *) (&LPC_IOCON->P0_0); 
	for(uc_Bit =0;uc_Bit < PIN_ALL_TOTAL;uc_Bit++)  // P0.00 --P5.04 Address 5*32+5// 
	{
		*This_P |= 0x10;   // PX[X] �� General purpose digital input/output pin.
		This_P++;
	}
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
