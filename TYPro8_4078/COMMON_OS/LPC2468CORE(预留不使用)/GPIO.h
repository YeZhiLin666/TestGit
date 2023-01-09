
#ifndef _GPIO_
#define _GPIO_

#include "../../COMMON/Datatypes.h"

////user definition////
//需要使用的模块按顺序设置为  1-255
//不使用的模块置为0

#define UART_ENABLE    0
#define CAN_ENABLE 	   0
#define I2S_ENABLE	   0
#define TIMER_ENABLE   0  //包括CAP和MAT    基本用于外部捕获
#define AD_ENABLE	   0
#define DA_ENABLE	   0
#define USB_ENABLE	   0
#define SD_ENABLE	   0
#define PWM_ENABLE	   0
#define EXTIN_ENABLE   0

// used in this application.

#define SDRAM_ENABLE	1
#define SPI_ENABLE		2	 //SPI和SSP拆成了两个部分
#define SSP_ENABLE		3
#define I2C_ENABLE	    4	//commnet out chen
#define CPLD_ENABLE     5
#define ENET_ENABLE     6


////end user definition////


//comment out chen
#define PM_IN		1
#define PM_OUT		2
#define PM_MOD		3

#define PORT_MAX    5     // 端口最大值 P0 --- P5 // 
#define PIN_MAX     32    // 管脚最大值  PX.00 --- PX.31 // 
#define MOD_MAX     4     // 模式最大值  I2S_ENABLE,CAN_ENABLE,TIMER_ENABLE,UART_ENABLE,and so on // 
#define PIN_ALL_TOTAL    133   // 管脚全部总数 P0.00 --- P5.04 Address  4*32+5 // 


//define  control  PIOX //
#define   PIO0    0        //control  P0 out 
#define   PIO1    1        //control  P1 out 
#define   PIO2    2        //control  P2 out 
#define   PIO3    3        //control  P3 out 
#define   PIO4    4        //control  P4 out 
#define   PIOALL    5     //  ALL GPIO Sum


class sdram
{
	public:
	static unsigned char MyIOCount;
	static unsigned char MyIO[57][4];
};

class eint    //外部中断
{
	public:
	static unsigned char MyIOCount;
	static unsigned char MyIO[2][4];
};

class ethernet
{
	public:
	static unsigned char MyIOCount;
	static unsigned char MyIO[18][4];
};
class cpld
{
	public:
	static unsigned char MyIOCount;
	static unsigned char MyIO[1][4];
};

//其他的基本是靠SPI来初始化





////////GPIO////////
class GPIOFunc
{
public:


#ifdef _DEBUG

	static unsigned char GPIO_CheckResult[PORT_MAX][PIN_MAX];

#endif

	//几个表
	//因为分成系统级别和外设级别
	//static unsigned char GPIO_SysDefault[PORT_MAX][PIN_MAX];    //系统自身用
	static unsigned char GPIO_MOD[PORT_MAX][PIN_MAX];    //是用mod还是普通gpio
	static unsigned char GPIO_InitValue[PORT_MAX][PIN_MAX];    //PW_OUT下的输出高或低
	const static unsigned char GPIO_MODValue[PORT_MAX][PIN_MAX][MOD_MAX];    //mod具体的使能端口value

	static unsigned char GPIO_MODValueError;

	static void SetModuleIO(unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName);
	static void GPIO_Set_n_Check(void);
	static void GPIO_Init(void);

	static void InitPort(unsigned char uc_Port, unsigned char uc_Pin, unsigned char uc_Mode, unsigned char uc_Value,unsigned char uc_Base);//非常重要

	static void Set_G_Port(G_Port * g_port);
	static void Clr_G_Port(G_Port * g_port);
  static unsigned char RD_G_Port(G_Port *g_port);
	//暂时预留
	//static void setHigh(stGPIO_Port * p_st_Port);   
	//check是否是PM_OUT，如果是PM_MOD 就要停止 同时报个错，如果是PM_OUT setHigh
	//static void setLow(stGPIO_Port * p_st_Port);
	//同样
	//同时使用这个取代spi的

	//static void SetPin(int myPort, int myPin, int myValue);
	//static void SetPort(int myPort, UInt32 myValue, UInt32 myMask);
	//static void ReadPin(int myPort, int myPin);
	//static void ReadPort(int myPort, UInt32 myMask);
};

#endif

//执行过程
//执行GPIO_Set_n_Check()
//Set the settings of GPIO configs for all "hardware modules"
//然后debug是否有重复定义
//GPIO_Init()，它会调用InitPort按照表依次初始化


