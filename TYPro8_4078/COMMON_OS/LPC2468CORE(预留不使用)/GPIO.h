
#ifndef _GPIO_
#define _GPIO_

#include "../../COMMON/Datatypes.h"

////user definition////
//��Ҫʹ�õ�ģ�鰴˳������Ϊ  1-255
//��ʹ�õ�ģ����Ϊ0

#define UART_ENABLE    0
#define CAN_ENABLE 	   0
#define I2S_ENABLE	   0
#define TIMER_ENABLE   0  //����CAP��MAT    ���������ⲿ����
#define AD_ENABLE	   0
#define DA_ENABLE	   0
#define USB_ENABLE	   0
#define SD_ENABLE	   0
#define PWM_ENABLE	   0
#define EXTIN_ENABLE   0

// used in this application.

#define SDRAM_ENABLE	1
#define SPI_ENABLE		2	 //SPI��SSP�������������
#define SSP_ENABLE		3
#define I2C_ENABLE	    4	//commnet out chen
#define CPLD_ENABLE     5
#define ENET_ENABLE     6


////end user definition////


//comment out chen
#define PM_IN		1
#define PM_OUT		2
#define PM_MOD		3

#define PORT_MAX    5     // �˿����ֵ P0 --- P5 // 
#define PIN_MAX     32    // �ܽ����ֵ  PX.00 --- PX.31 // 
#define MOD_MAX     4     // ģʽ���ֵ  I2S_ENABLE,CAN_ENABLE,TIMER_ENABLE,UART_ENABLE,and so on // 
#define PIN_ALL_TOTAL    133   // �ܽ�ȫ������ P0.00 --- P5.04 Address  4*32+5 // 


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

class eint    //�ⲿ�ж�
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

//�����Ļ����ǿ�SPI����ʼ��





////////GPIO////////
class GPIOFunc
{
public:


#ifdef _DEBUG

	static unsigned char GPIO_CheckResult[PORT_MAX][PIN_MAX];

#endif

	//������
	//��Ϊ�ֳ�ϵͳ��������輶��
	//static unsigned char GPIO_SysDefault[PORT_MAX][PIN_MAX];    //ϵͳ������
	static unsigned char GPIO_MOD[PORT_MAX][PIN_MAX];    //����mod������ͨgpio
	static unsigned char GPIO_InitValue[PORT_MAX][PIN_MAX];    //PW_OUT�µ�����߻��
	const static unsigned char GPIO_MODValue[PORT_MAX][PIN_MAX][MOD_MAX];    //mod�����ʹ�ܶ˿�value

	static unsigned char GPIO_MODValueError;

	static void SetModuleIO(unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName);
	static void GPIO_Set_n_Check(void);
	static void GPIO_Init(void);

	static void InitPort(unsigned char uc_Port, unsigned char uc_Pin, unsigned char uc_Mode, unsigned char uc_Value,unsigned char uc_Base);//�ǳ���Ҫ

	static void Set_G_Port(G_Port * g_port);
	static void Clr_G_Port(G_Port * g_port);
  static unsigned char RD_G_Port(G_Port *g_port);
	//��ʱԤ��
	//static void setHigh(stGPIO_Port * p_st_Port);   
	//check�Ƿ���PM_OUT�������PM_MOD ��Ҫֹͣ ͬʱ�����������PM_OUT setHigh
	//static void setLow(stGPIO_Port * p_st_Port);
	//ͬ��
	//ͬʱʹ�����ȡ��spi��

	//static void SetPin(int myPort, int myPin, int myValue);
	//static void SetPort(int myPort, UInt32 myValue, UInt32 myMask);
	//static void ReadPin(int myPort, int myPin);
	//static void ReadPort(int myPort, UInt32 myMask);
};

#endif

//ִ�й���
//ִ��GPIO_Set_n_Check()
//Set the settings of GPIO configs for all "hardware modules"
//Ȼ��debug�Ƿ����ظ�����
//GPIO_Init()���������InitPort���ձ����γ�ʼ��


