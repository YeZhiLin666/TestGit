
#include "../../COMMON/includes.h"

//此IBT通道 没有钳压

/*
此文件提供了IBT类型板新的时序驱动及公有操作
comment out chen 20130216
IO部分暂时没有补完
*/


//注意，实际上并不能控制通道    
//cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
//所有有关CPLD ADD的操作可能有错误 comment out chen 20130106

/*Format
//结构数组  每个时序需要几个小move来完成
// Disposable 一次性
******************************************************************************/
//定义line370+
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


volatile unsigned short IBT_Base::IBT_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
//unsigned short IBT_Base::NBUS_WRITE_DCS_status;    //记录自身CS状态     改完CPLD后可以不使用
// 重新初始化成上拉 
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
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif

	IO_Init();    //内部IO初始化

	//GPIO P   3.18 WI    0.6 DCS2  1.21 DCS1   1.28 CONV   作为GPIO output 且初始化并不相同  WI为低 其他应为高
	//PINSEL7 &= 0xFFFFFFCF;    //P3.18GPIO  喂狗信号
	//FIO3DIR |= 0x00040000;    //P3.18 output
	//FIO3CLR |= 0x00040000;    //P3.18 low电平
	GPIOFunc::InitPort(3,18,PM_OUT,1,0);    //需要更改GPIO_InitValue  如果高，则1，低则0    //代替上面的

	/*// 喂狗信号
	//{// 喂狗信号
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}*/
	WD(200);  // 喂狗信号

	cpu_iowr_16(NBUS_WIRTE_ARBIN,0x01);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0x01);
	cpu_iowr_16(NBUS_ARBINWR_RD,0);
	cpu_iowr_16(NBUS_WRITE_E_ADD,0);
	cpu_iowr_16(NBUS_WRITE_E_CON,0x03);    //默认为高    ADWRITE DOWIRTE均为高
	//cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x1F);    //默认为高
	cpu_iowr_16(NBUS_WRITE_DCS,0x1F);    //0x1F默认为高
	cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);    //默认为高    //commnet out chen    IA1 IA0 为低

	/*for(char i =0;i<16;i++)
	{
	IBT_relaystatus[i] = 0;    //保存继电器状态
	}*/
	memset((void *)(&IBT_relaystatus[0]), 0, MAXCHANNELNO);
	//NBUS_WRITE_DCS_status= 0x1F;    //20130218  chen

	//test
	//TIMERSFunc::DELAY_SYS(200);
	//static uint16 temp = cpu_iord_16(NBUS_WRITE_DCS);

	//end test


	////初始化SPI SSP0 SSP1接口    //在gpio内进行
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


	/*// 喂狗信号    comment out chen 20130104
	//{// 喂狗信号
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}*/
	WD(200);  // 喂狗信号

	for(char a_i = 0;a_i<MAXCHANNELNO;a_i++)
	{
		WriteDOBus(a_i,0x1000);    //ISO拉高
		IBT_relaystatus[a_i] = 0x1000;    //add 20130410
	}				

	//飞线后修改SSP,SSP0带两个DAC和FRAM，SSP1带两个ADC。

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
	//设置SPI SSP参数
	SSPIFunc::setPara(&IBT_SSP0DAC,0);
	SSPIFunc::setPara(&IBT_SSP0DAC_CLAMP,1);

	SSPIFunc::setPara(&IBT_SSP1ADI,2);
	SSPIFunc::setPara(&IBT_SSP1ADV,3);
	//初始化SPI SSP

	SSPIFunc::my_Init(0);
	SSPIFunc::my_Init(1);
	SSPIFunc::my_Init(2);//复用SSP1
	SSPIFunc::my_Init(3);


	TIMERSFunc::DELAY_SYS( 30000 );	//delay 5ms
	//这个应该在初始化SPI SSP接口之后
	/*//{//喂狗信号
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}*/
	WD(200);  // 喂狗信号

	ADC_Init();
	DAC_Init();

	//重新初始化IO为上拉电阻模式
	ReInitPullUP();

	/*//喂狗信号
	//{// 喂狗信号
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}*/
	WD(200);  // 喂狗信号

	for(char i =0;i<MAXCHANNELNO;i++)
	{
		IBT_Base::Adjust_CHN_Gain(i,1);    //使用单倍
		//IBT_Base::Adjust_CHN_Gain(1,1);    //使用单倍
	}
	IBT_Base::Adjust_ADC_Gain(1,IBT_ADC_TYPE_I); // 使用单倍
	IBT_Base::Adjust_ADC_Gain(1,IBT_ADC_TYPE_V); // 使用单倍
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
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	//GPIOFunc::SetModuleIO();     //设置MOD

	//SPI接口
	//GPIO_MODValue[][]设置好值
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount1, SSPIFunc::MyIO1, SPI_ENABLE);    //unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName
	GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount2, SSPIFunc::MyIO2, SSP_ENABLE);
	//GPIOFunc::SetModuleIO(SSPIFunc::MyIOCount3, SSPIFunc::MyIO3, SSP_ENABLE);


	IBT_GPIO::GPIO_Init();           //初始化

	//OS_ServeProcess::IOCheck();     //暂时不做check
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
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	//初始化P1.26
	//PINSEL3 &= 0xFFCFFFFF;
	//FIO1DIR |= ADC_CS5560_RST;    //IO为输出
	//FIO1SET |= ADC_CS5560_RST;    //设置为高
	GPIOFunc::InitPort(1, 26, PM_OUT, 1,0);     //ADC_CS5560_RST
	GPIOFunc::InitPort(1, 22, PM_OUT, 1,0);     //ADC_CS5560_RST
	//重置
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

	////方向
	//FIO1DIR |= (1<<28);        //1.28
	//FIO2DIR &= ~(1<<12);    //input  2.12
	//FIO1DIR |= (1<<21);        //1.21
	//FIO2DIR &= ~(1<<13);    //input 2.13
	//FIO0DIR |= (1<<6);    //P0.6

	////默认为高
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
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	IBT_ADC5DAC::Dac_Init(0);
	//IBT_ADC5DAC::Dac_Init(1);

	//AD5360初始化，在SPI初始化后执行
	//设置reset 输出    load1 输出
	//我猜想不用写pwoer-up也可以    暂不清楚
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
	//设置为输出
	GPIOFunc::InitPort(1, 5, PM_OUT, 1,0);  //DAC_CLR SET 1
	GPIOFunc::InitPort(1, 2, PM_IN, 0,0);  
	LPC_GPIO1->DIR |= DAC_LOAD1;
	LPC_GPIO1->DIR |= DAC_RST;
	LPC_GPIO1->CLR |= DAC_LOAD1;           //dac_load1  CLR

	LPC_GPIO1->SET |= DAC_RST;    //RST置高
	ARBUS_DELAY(100);
	LPC_GPIO1->CLR |= DAC_RST;    //RST低
	ARBUS_DELAY(100);
	LPC_GPIO1->SET |= DAC_RST;     //rising edge触发
	ARBUS_DELAY(10000);    //300us reset time  AD5360重置
	//reset 这个过程需要300us  然后CLRX M C寄存器全部恢复默认

	//输出power up    ??comment out chen 2012 11 29
	//拉低DCS3
	//输出命令SPI0
	//拉高DCS3
	//使能
	//不进行power也可以正常操作
	// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) &= ~(DCS3) ;
	// 	
	//	spi_2468::C_RxTx_Frames(SPI_Send_Buffer,3,0);
	SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,0);
	// 		
	// 	//clear 使能
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
	Func_Init();//函数指针初始化  四个基本操作
	EngineStuff();   //引擎结构指针初始化

	for(unsigned char i = 0; i < board_config::st_board_info.Channel_Number; i++)    //从0开始
	{
		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//注册chn info
		regEngine(i,board_config::st_channel_info[i].Channel_type);//注册引擎			
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

	//没有动作
	OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &IBT_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlRstToOff.stepLen = IBT_Base::SetRstToOff_Len;
	OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &IBT_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = IBT_Base::SetRstToOff_Len;

	OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];    //指向数组0
	OS_LogicProcess::SetCtrlOffToCur.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &IBT_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = IBT_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &IBT_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = IBT_Base::SetCCToCC_Len;

	//这里有问题
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
** 手工填充具体时序
******************************************************************************/
void IBT_Base::EngineStuff(void)
{
	//---------------------------------------------//
	SetCtrlOpenCh[0].uw32_DelayTime = 100;    //加大延迟，防止继电器对AD8253干扰  20130227  comment out chen
	SetCtrlOpenCh[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //|= ACTIVE_OUT_RELAY;    //test
	//SetCtrlOpenCh[0].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //输出量程继电器    20130227
	SetCtrlOpenCh[0].DacMove.DacFlag |= ACTIVE_DAC_CUR_ZERO;
	SetCtrlOpenCh[0].RangeChangedFlag = 0;
	SetCtrlOpenCh[1].uw32_DelayTime = 0;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //保持开关通道状态
	SetCtrlOpenCh[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //输出量程继电器
	SetCtrlOpenCh[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCtrlOpenCh[1].RangeChangedFlag = 0;    //开通到要考虑电流量程

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 10;
	SetCtrlCloseCh[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlCloseCh[0].RangeChangedFlag = 0;
	SetCtrlCloseCh[1].uw32_DelayTime = 0;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = ACTIVE_V_I_R;    //打开电流电压端检测
	SetCtrlCloseCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlCloseCh[1].RangeChangedFlag = 0;

	//---------------------------------------------//
	//comment out chen 20130128  暂时没有加入假的ADC切换过程
	SetCCToCC[0].uw32_DelayTime = 10;
	SetCCToCC[0].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //保持通道    //debug comment out chen 20130107
	SetCCToCC[0].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	//将来有可能是关掉
	SetCCToCC[0].RangeChangedFlag = 0;
	SetCCToCC[1].uw32_DelayTime = 0;
	SetCCToCC[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //保持通道    //debug comment out chen 20130107
	SetCCToCC[1].DacMove.DacFlag |= ACTIVE_DAC_CUR;
	SetCCToCC[1].RelayMove.RelaySet |= ACTIVE_OUT_NEXT_RELAY;    //直接输出下一量程
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
	SetCVToCV[1].RelayMove.ActiveFlag |= (ACTIVE_OUT_RELAY | ACTIVE_V_I_R);    //保持状态
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
	return 0;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
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
	unsigned long value = Data;    //强制保护
	unsigned long temp = 0;
	//comment out chen 20130104

#if DEMO_MODE == 1
	return 0;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif	

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		//加入DAC副本机制，以供第三方ms级别刷新    comment out chen 20130114
		ChannelInfo_Manager::ChannelInfo[chn].m_u32DacValue = value;    //此值为偏移码

		//因为特殊的控制  正负需要反向
		if( value > 0x800000 )    //正数
		{
			temp = value - 0x800000;
			value = 0x800000 - temp;
		}
		else                                           //负数
		{
			temp = 0x800000 - value;
			value = 0x800000 + temp;
		}
		//comment out chen 20130228  加入保护机制
		if(value > 0xFFFFFF)     //0x1000000
		{
			value = 0xFFFFFF;    //防止超出  0xFFFFFF
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
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;
	case ACTIVE_V_I_R:              //I/S/O
		temp |= 0x1000;
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;
	case ACTIVE_OUT_RELAY:     //X_CHL
		temp |= 0x8000;
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;

		//comment out chen 20130128    增加了假的切换时序处理
	case ACTIVE_CUR_RELAY_HIGH:
		//切换增益
		changeADCRange(chn,CURRENT_RANGE_HIGH);
		return 1;
	case ACTIVE_CUR_RELAY_MIDDLE:
		//切换增益
		changeADCRange(chn,CURRENT_RANGE_MED);
		return 1;
	case ACTIVE_CUR_RELAY_LOW:
		//切换增益
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
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;
	case ACTIVE_V_I_R:              //I/S/O
		temp &= ~(0x1000);
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;
	case ACTIVE_OUT_RELAY:     //X_CHL
		temp &= ~(0x8000);
		WriteDOBus(chn,temp);
		IBT_relaystatus[chn] = temp;   //记录
		return 1;
		//break;
	default:
		return 0;
		//break;
	}
	//return 0;
}


//按照DO输出锁存时序    控制的是relay
/*
1 ARBINDIR  写
2 ARBINWR  YD 0-15  继电器类型
3 ARBIN  拉低
4 ADD 0-4  选择通道
5 等待Yi输出
6 DOWRITE  拉低
7 YD 0-15输出  控制继电器
*/
//2012 11 29  comment out chen    is work
void IBT_Base::WriteDOBus(char chn,uint16 BD)
{
	if(chn >= MAXCHANNELNO)
		return;
	UWord32 ENback[2];
#if DEMO_MODE == 1
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	//Disable interrupts for now
	ENback[0] = NVIC->ISER[0];             // enable interrupt  //
	ENback[1] = (NVIC->ISER[1] & 0X01FF);   // enable interrupt  //
	NVIC->ICER[0] = 0XFFFFFFFF;                    // disable interrupt //
	NVIC->ICER[1] = 0X01FF;                    // disable interrupt //

	//  uint32 ENback = VICIntEnable;    //keep the old interrupt status
	// VICIntEnClr = 0xFFFFFFFF;    //disable 

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR拉低  锁住XD总线
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD, BD );    //输出BD    //此时数据被锁住
	//*(volatile unsigned short *)(NBUS_ARBINWR_RD) |= (BD) ; 
	ARBUS_DELAY(5);

	//选择通道
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn);  //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
	//这里是选择通道输出锁存
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低使能Yi
	ARBUS_DELAY(5);    //短delay

	//Yi根据XADD输出

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE拉低    ADWRITE为高
	ARBUS_DELAY(5);    //短delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE拉高    ADWRITE为高

	//Yi与DOWRITE组成了DOCS
	ARBUS_DELAY(5);

	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);    //拉高ARBIN
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR拉高     //2012 11 29  comment out chen

	////////////////////////////////////////////////////////////////////
	NVIC->ISER[0] = ENback[0];    //enable interrupt
	NVIC->ISER[1] = ENback[1];
}

//此操作暂时没完成
//comment out chen 20130216
unsigned short IBT_Base::ReadDOBus(void)
{

#if DEMO_MODE == 1
	return 1;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif

	unsigned short temp = 0;

	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0&NBUS_ARBINDIR);    //拉低DIR
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //拉高DIR
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD,0x00);  //拉低
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD,0x01);  //拉高
	//锁住BD

	temp = cpu_iord_16(NBUS_ARBINWR_RD);
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //拉低

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //再拉高DIR

	return temp;
}

/*
//方法类似于DO  控制的是可编程放大器    0x8100 000A write PGA gain
void IBT_Base::WritePGABus(char chn,uint16 PGABD)
{
if(chn > 16)
return;

//	// Disable interrupts for now
uint32 ENback = VICIntEnable; //keep the old interrupt status
VICIntEnClr = 0xFFFFFFFF; //disable 

ARBUS_DELAY(5);
cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR拉低
cpu_iowr_16(NBUS_WRITE_PGA_GAIN, PGABD );    //输出BD
ARBUS_DELAY(5);
cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR拉高

//这里是选择通道输出锁存
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低
//选择通道
cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //通道
ARBUS_DELAY(10);    //短delay
//Yi输出
cpu_iowr_16(NBUS_WRITE_E_CON, 0&NBUS_DOWRITE);    //DOWRITE拉低
ARBUS_DELAY(10);    //短delay
cpu_iowr_16(NBUS_WRITE_E_CON, 0|NBUS_DOWRITE);    //DOWRITE拉高

//YD输出
ARBUS_DELAY(5);
//拉高ARBIN
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 

////////////////////////////////////////////////////////////////////
VICIntEnable = ENback; //enable interrupt
}


//ADC开关时序
void IBT_Base::WriteADCBus(char chn)
{
////	// Disable interrupts for now
//uint32 ENback = VICIntEnable; //keep the old interrupt status
//VICIntEnClr = 0xFFFFFFFF; //disable 

cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低
//选择通道
cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //通道
ARBUS_DELAY(10);    //短delay
//Yi输出
cpu_iowr_16(NBUS_WRITE_E_CON, 0&NBUS_DOWRITE);    //DOWRITE拉低
ARBUS_DELAY(10);    //短delay
cpu_iowr_16(NBUS_WRITE_E_CON, 0|NBUS_DOWRITE);    //DOWRITE拉高

//ADCS拉低
cpu_iowr_16(NBUS_WRITE_DCS,0);
ARBUS_DELAY(10);    //短delay
//拉高ARBIN
cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 

//ADCS拉高

//ADRESET重置

//////////////////////////////////////////////////////////////////////
//VICIntEnable = ENback; //enable interrupt
}
*/


//////////////////////////////////////////////////////////comment out chen 2012 12 24 移至ADC CS5560 这是24bit的adc//////////////////////////////////////////////////////////
////这里分别读取I V
////I ssp0    V ssp1    同时还要操作ad9253  ad9251控制增益才可读取
////还要控制ADC开关通道时序
//unsigned long int CS5560_Read(char chn,char IVtype)    //第二个参数预留 控制增益需要
//{
//	/*
//	//操作顺序
//    //1 控制可编程放大器，可变增益  IA0  IA1  VA1  VA2  //此步可免  新建函数  调整可编程放大器倍数  
//	//2 ADC开关操作  此时已经打开选通的通道
//	//3 读取ADC  也可以说触发
//	//4 ADRESET
//	//comment out chen 2012 12 03
//	*/
//    
//	//此步可免
//	////1  ///////////////////////////just for AD8253/////////////////////////////
//	////	// Disable interrupts for now
//	//// 	uint32 ENback = VICIntEnable; //keep the old interrupt status
//	//// 	VICIntEnClr = 0xFFFFFFFF; //disable 
//	//unsigned char BD = 0x12;    //WR+A0+A1  此时的BD for 控制可编程放大器10010B  comment out chen 此时只是简单test
//
//	//ARBUS_DELAY(5);
//	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR拉低  写  锁住XD总线
//	////cpu_iowr_16(NBUS_ARBINWR_RD, BD );    //输出BD    //此时数据被锁住
//	//*(volatile unsigned short *)(NBUS_ARBINWR_RD) |= BD;    //避免改写继电器
//	//ARBUS_DELAY(5);
//
//	////这里是选择通道输出锁存
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低
//	////选择通道
//	//cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
//	////这里是选择通道输出锁存
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低使能Yi
//	//ARBUS_DELAY(5);    //短delay
//	////Yi输出
//	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_DOWRITE);   //DOWRITE拉低
//	//ARBUS_DELAY(5);    //短delay
//	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_DOWRITE) ;   //DOWRITE拉高
//
//	////YD输出
//	//ARBUS_DELAY(5);
//	//cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); //拉高ARBIN
//	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR拉高     //2012 11 29  comment out chen
//	//////////////////////////////////////////////////////////////////////
//	////VICIntEnable = ENback; //enable interrupt
//	////1  ///////////////////////////end of just for AD8253/////////////////////////////
//
//
//// 	//2  ///////////////////////////ADC开关/////////////////////////////
//// 	//选择通道
//// 	cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
//// 	//这里是选择通道输出锁存
//// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低使能Yi
//// 	ARBUS_DELAY(5);    //短delay
//// 	//Yi输出
//// 	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_ADWRITE);   //ADWRITE拉低
//// 	//ARBUS_DELAY(5);    //短delay
//// 	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_ADWRITE) ;   //ADWRITE拉高
//// 	cpu_iowr_16(NBUS_WRITE_E_CON, 0x01);    //DOWRITE为高    ADWRITE拉低
//// 	ARBUS_DELAY(5);    //短delay
//// 	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE为高    ADWRITE拉高
////     //comment out chen 2012 12 04
//// 	
//// 	//AD+Yi -> ADCS    此时数据被保持
//// 	ARBUS_DELAY(5);
//// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 
//// 	//拉高ARBIN   我想这时拉高就是说U17的SN74HC154DW一直保持该状态  直到下一个OE使能被改变
////     //2  ///////////////////////////end of ADC开关/////////////////////////////
//
//	//3  ///////////////////////////读取 ADC/////////////////////////////
//	//触发CONV
//	FIO1CLR |= ADC_CONV;    //拉低
//	ARBUS_DELAY(5);
//	FIO1SET |= ADC_CONV;    //拉高
//	//等待READY
//	while(FIO2PIN & BUSY1);    //wait busy pin
//	//调用ssp接口来读取数据
//	unsigned short Tx[3],Rx[3];
//	Tx[0] = 0;    //发送出0即可
//	Tx[1] = 0;
//	Tx[2] = 0;
//	//FIO1CLR |= DCS1;    //触发CS
//	SSPIFunc::RxTx_Frames_1CS(Tx,3,1);    //读回来的数据在 spi的Rx缓冲区内    //发送数组  长度  通道
//	//FIO1SET |= DCS1;
//	for(char i =0;i<3;i++)
//	{
//		Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //接收数据
//	}
//	//spi_2468::SSP_RxTx_Frames(unsigned short *arry,unsigned char len,unsigned char chn);
//	//3  ///////////////////////////end of 读取 ADC/////////////////////////////
//	
//	//切换返回值
//	ARBUS_DELAY(200);
//	return;
//
//// 	//4  ///////////////////////////ADC RESET/////////////////////////////
////     //BUS1
//// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) &= ~(NBUS_BUS1);   //拉低
//// 	ARBUS_DELAY(5);
//// 	*(volatile unsigned short *)(NBUS_WRITE_DCS) |= NBUS_BUS1;   //拉高
//// 	//4  ///////////////////////////end of ADC RESET/////////////////////////////
//}


//调整通道板上的adc gain
//操作这个时一定需要清楚当前relay的状态，否则relay将关闭
//WR下降沿才写入
void IBT_Base::Adjust_CHN_Gain(unsigned char chn,unsigned long type)    
{
	// 1  ///////////////////////////just for AD8253/////////////////////////////
	//	// Disable interrupts for now
	// 	uint32 ENback = VICIntEnable; //keep the old interrupt status
	// 	VICIntEnClr = 0xFFFFFFFF; //disable 

#if DEMO_MODE == 1
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
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
		BD1 = 0x0012;    //WR+A0+A1  此时的BD for 控制可编程放大器10010B  comment out chen 此时只是简单test
		BD2 = 0x0001;
	}
	temp = IBT_relaystatus[chn];    //从继电器内取回当前状态，避免改写继电器    //is it right?    
	temp |= BD1;
	temp &= ~(BD2);
	IBT_relaystatus[chn] = temp; 
	//准备数据 temp

	//使用WirteDOBus代替的方式
	//WriteDOBus(chn,temp);
	//ARBUS_DELAY(5);
	//temp &= 0xFFEF;    //拉低WR
	//WriteDOBus(chn,temp);
	//IBT_relaystatus[chn] = temp; 
	//return;

	//Disable interrupts for now
	IRQFunc::All_DisableIRQ();

	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR拉低  写  锁住XD总线  此为输出使能，如果想使
	//MC74HC574输出有效，必须拉低
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_ARBINWR_RD, temp);    //输出BD    //此时数据被锁住
	ARBUS_DELAY(5);
	//WR自动输出
	//XD此时输出temp

	//选择通道
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn);    //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
	//这里是选择通道输出锁存
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低使能Yi    //MC74HC541因为G1G2拉低，直接有输出
	ARBUS_DELAY(5);    //短delay  此时Yi通道板Yi输出
	//Yi输出

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE拉低    ADWRITE为高
	ARBUS_DELAY(10);    //短delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE拉高    ADWRITE为高    MC74HC574上升沿写入
	//Yi与DOWRITE组成DOCS  拉低DOCS
	ARBUS_DELAY(5);

	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //DIR拉低  写  锁住XD总线
	//重新做YD输出 以便使WR产生下降沿
	ARBUS_DELAY(5);
	temp &= 0xFFEF;    //拉低WR
	cpu_iowr_16(NBUS_ARBINWR_RD, temp);  //数据重新输出到总线
	ARBUS_DELAY(5);
	//cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR拉高     //2012 11 29  comment out chen

	cpu_iowr_16(NBUS_WRITE_E_CON, 0x02);    //DOWRITE拉低    ADWRITE为高
	ARBUS_DELAY(10);    //短delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE拉高    ADWRITE为高
	ARBUS_DELAY(5);
	//再次触发DOCS

	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); //拉高ARBIN  停止YI输出
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR拉高     //2012 11 29  comment out chen
	////////////////////////////////////////////////////////////////////
	//enable interrupt
	IRQFunc::All_EnableIRQ();
	// 1  ///////////////////////////end of just for AD8253/////////////////////////////

	IBT_relaystatus[chn] = temp; 

	//WriteDOBus(chn,temp);
	//ARBUS_DELAY(5);
	//temp &= 0xFFEF;    //拉低WR
	//WriteDOBus(chn,temp);
	ARBUS_DELAY(5);
}

//调整adc板上的gain
void IBT_Base::Adjust_ADC_Gain(char type,char chnType)
{
#if DEMO_MODE == 1
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	if(chnType == IBT_ADC_TYPE_I)
	{
		if(type == 1)    //单倍
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
		if(type == 1)    //单倍
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
}


//ADC模拟门打开
void IBT_Base::ADC_ADG_open(char chn) 
{
#if DEMO_MODE == 1
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif
	///////////////////////////ADC开关/////////////////////////////
	//选择通道
	cpu_iowr_16(NBUS_WRITE_E_ADD, chn );    //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //控制CPLD，此时ADD有输出
	//这里是选择通道输出锁存
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN拉低使能Yi
	ARBUS_DELAY(5);    //短delay
	//Yi输出
	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) &= ~(NBUS_ADWRITE);   //ADWRITE拉低
	//ARBUS_DELAY(5);    //短delay
	//*(volatile unsigned short *)(NBUS_WRITE_E_CON) |= (NBUS_ADWRITE) ;   //ADWRITE拉高
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x01);    //DOWRITE为高    ADWRITE拉低
	ARBUS_DELAY(5);    //短delay
	cpu_iowr_16(NBUS_WRITE_E_CON, 0x03);    //DOWRITE为高    ADWRITE拉高
	//comment out chen 2012 12 04

	//AD+Yi -> ADCS    此时数据被保持
	ARBUS_DELAY(5);
	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN); 
	//拉高ARBIN   我想这时拉高就是说U17的SN74HC154DW一直保持该状态  直到下一个OE使能被改变
}

//ADreset
void IBT_Base::ADC_Reset(void)
{
#if DEMO_MODE == 1
	return;    //在Demo模式下，防止没有ADC器件，导致过度等待耗时。
#endif

	//20130218  chen
	uint16 temp = *(volatile unsigned short *)(NBUS_WRITE_DCS);
	temp &= ~(NBUS_BUS1);   //拉低
	*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;
	ARBUS_DELAY(5);
	temp |= NBUS_BUS1;
	*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;    //拉高

	////可用    comment out chen 20130131
	//   uint16 temp = IBT_Base::NBUS_WRITE_DCS_status;    //取出状态
	//   temp &= ~(NBUS_BUS1);   //拉低
	//*(volatile unsigned short *)(NBUS_WRITE_DCS) = temp;
	//ARBUS_DELAY(5);
	//*(volatile unsigned short *)(NBUS_WRITE_DCS) = IBT_Base::NBUS_WRITE_DCS_status;    //恢复
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
PGA_Gain()更改增益 电流超高量程    电流高量程    1,10,100,1000

Relay 操作：
type包括：
电压量程切换         ACTIVE_VOL_RANGE
电流端电压测量      ACTIVE_V_I_R
输出继电器            ACTIVE_OUT_RELAY

relay操作时，因为此板类型十分特殊，需要有个relay_status来记录自身状态
volatile unsigned short IBT_relaystatus;

PGA_Gain change
1
10
100
1000

----------------------------------------------------------
Dac_out
输出通道
----------------------------------------------------------
Dac_clamp_out
钳压输出，此板钳压输出是公有的
----------------------------------------------------------
Adc_read
比较特殊


显然当前的Disposable(OneTime)不能满足此类型

*/


//把这部分转移到IBT_ADC5DAC中去
//comment out chen 20130216
/*
//此value为偏移码
void IBT_Base::Dac_out_Cur(char chn,unsigned long value)
{
//AD5360_DACout为二补码，需要转换
//由偏移码转到二补码
//第一位取反
//unsigned short temp = value;
//value>>15;
//value~=value;    //第一位取反
//value<<15;
//temp &= ~(0x8000);    //第一位置0
//temp |= value;

//加入DAC副本机制，以供第三方ms级别刷新    comment out chen 20130114
ChannelInfo_Manager::ChannelInfo[chn].m_u32DacValue = value;    //此值为偏移码

unsigned long temp = value;

if( (value & 0x800000) == 0x800000)    //判断第一位是否为1    第一位取反
{
temp &= ~(0x800000);
}
else
{
temp |= 0x800000;
}

//检查SPI接口
unsigned char bOK = SSPIFunc::getModuleName(0);
if(bOK  != IBT_SPI_MODULE_NAME)
{
//重新初始化
//SPI

S0SPCR = 0x00;    //复位SPI
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

SSPIFunc::setPara(&IBT_SPI,0);    //设置参数
SSPIFunc::my_Init(0);              //初始化
}

ARBUS_DELAY(10);
DAC16bit_AD5360::Dac_out(chn,temp);    //转换为二补码而输出

}

//此为pid电压
void IBT_Base::Dac_out_Vlt(char chn,unsigned long value)
{

}

//clamp voltage
void IBT_Base::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{

}

*/


//移动到SampleMagnage.cpp
/*
//处理ADC     自我调度运行
//ProcessADC本身是一个FSM
//  首先1 ADC_ADG_open  2 10us  3 CS5560_Read while busy 30us  4 ADC_Reset关闭
//实际上 一个周期 读I和V的 一个chn  此板最大的问题就是采样周期较慢  因为有重新打开的问题
//加入电压采样    20130109    comment out chen
char IBT_Base::ProcessADC(char adc_device_chn)
{
uint32 temp = 0;
uint32 temp_for_inverse = 0;
static uint32 temp_V = 0;
//读一次I  再读一次V
static char status = IBT_ADC_NO_OPEN;
static unsigned long conter = 0;

if(adc_device_chn == 0)    //I
{
switch(status)
{
case IBT_ADC_NO_OPEN:
ADC_ADG_open(I_chn);    //打开
return 0;    //not ready
case IBT_ADC_OPENING:
return 0;
case IBT_ADC_OPEN:
Adc16bit_CS5560::CS5560_CONV();
return 0;
case IBT_ADC_READY:
temp = Adc16bit_CS5560::CS5560_Read(I_chn,IBT_ADC_TYPE_I);    //value进行移位
temp_V = Adc16bit_CS5560::CS5560_Read(I_chn,IBT_ADC_TYPE_V);     //电压
//断点在此

//因为特殊的控制  正负需要反向
if( temp>0x800000 )    //正数    24bit
{
temp_for_inverse = temp - 0x800000;
temp = 0x800000 - temp_for_inverse;
}
else                                           //负数
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
return SAMPLE_PROCESS_SIGMA_DELTA_ADC;    //非零  为delta-sigma ADC
}
}
else if(adc_device_chn == 1)    //V  类似
{
switch(status)
{
case IBT_ADC_NO_OPEN:
status = IBT_ADC_OPENING;
return 0;
case IBT_ADC_OPENING:
//计算时间
conter++;
if(conter == IBT_ADC_DG_BUILT_UP_TIME_TICKS)    //建立时间不够
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

//关闭
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

//返回的是码字
unsigned long int IBT_Base::getValue(char adc_device_chn)
{
return value;
}

char IBT_Base::getType(char adc_device_chn)
{
return type;
}
*/


//for Relay_on  加入了假的切换量程
//comment out chen 20130216
void IBT_Base::changeADCRange(char chn,unsigned char range)
{
	//TIMERSFunc::DELAY_SYS(10000);
	ARBUS_DELAY(10);
	//切换量程
	switch(range)
	{
	case CURRENT_RANGE_HIGH:
		IBT_Base::Adjust_CHN_Gain(chn,1);    // 1倍
		break;
	case CURRENT_RANGE_MED:
		IBT_Base::Adjust_CHN_Gain(chn,10);    // 10倍
		break;
	case CURRENT_RANGE_LOW:
		IBT_Base::Adjust_CHN_Gain(chn,100);    // 100倍
		break;
	default:
		IBT_Base::Adjust_CHN_Gain(chn,1);    // 默认1倍
		break;			
	}

}

void IBT_Base::WD(UWord32 uw32_DelayTime)
{
	/*	//IBT watch dog    comment out chen
	//{
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(100);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}

	// 喂狗信号
	//{// 喂狗信号
	FIO3SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(200);
	FIO3CLR |= 0x00040000;    //P3.18 low电平
	//}*/

	LPC_GPIO3->SET |= 0x00040000;    //P3.18 高电平
	TIMERSFunc::DELAY_SYS(uw32_DelayTime);
	LPC_GPIO3->CLR |= 0x00040000;    //P3.18 low电平
}

//get 拨码开关    如果是ON的话，就是该位为1，OFF为0
//comment out chen 20130221 增加
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
		GPIOFunc::InitPort(bID[i][0], bID[i][1], PM_IN, 0,0);    //设置成为INPUT
	}
	//GPIOFunc::InitPort(2, 13, PM_IN, 0,0);  

	UWord32 get_Value = LPC_GPIO3->PIN;    //BS0-BS7    P3.23-P3.31
	get_Value >>= 24;

	return (unsigned char)get_Value;
}


//重新初始化IO为上拉电阻模式
void ReInitPullUP(void)
{
	unsigned char  uc_Bit;
	UWord32 * This_P = NULL;
	This_P = (UWord32 *) (&LPC_IOCON->P0_0); 
	for(uc_Bit =0;uc_Bit < PIN_ALL_TOTAL;uc_Bit++)  // P0.00 --P5.04 Address 5*32+5// 
	{
		*This_P |= 0x10;   // PX[X] — General purpose digital input/output pin.
		This_P++;
	}
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
