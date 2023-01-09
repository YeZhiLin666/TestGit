
//1018  comment out chen
//不保证SSP时钟工作正常

#ifndef _SSPI_
#define _SSPI_
/*#include "../COMMON/Datatypes.h"
#include "../COMMON/GPIO.h"
#include "../COMMON/LPC2468.h"*/

#include "../../COMMON/includes.h"

////user definition//    
//中断还是轮询
#define    SSPI_POLL                          //轮询
//#define    SSPI_INTERRUPT                     //中断


//根据使用模块不同，将来需要自行添加
#define		ADC16BIT_SSPI		0x01
#define		ADC32BIT_LTC2498_SSPI		0x02
#define		ADC32BIT_LTC2449_SSPI		0x03
#define		EEPROM_SSPI			0x0A		//10
#define     Fram_SSPI           0x0B
#define		SMB_SSPI				0xFF



//....
//....


////end user definition////



//FM25L256
#define EEPROM_WREN				0X06		//write enable
#define EEPROM_WRDI				0X04		//write disable
#define EEPROM_RDSR				0x05		//read  the register
#define EEPROM_WRSR				0x01		//write the register 
#define EEPROM_READ				0x03		//write  command 
#define EEPROM_WRITE			0x02		//read   command


//data define
#define    SSPI_MAX_BUFFER       300
#define    SSPI_MAX_LEN          300
#define    SSPI_MAX_CHANNEL       3
#define    Dummy_Byte             0xa5
#define    SSPCLRAR               0
#define    SSPIMAX_TIMEOUT		  0x00000FFF    //使用SPI时，最大的超时时间1ms
#define    SSPI_FIFOSIZE           8

////define SPI user register////
	//define the SPI status Register
	#define S0SPSR_ABRT       0x0008    // slave abort .when 1, this bit indicates  that a slave abort has occurred .
	// This bit is cleared by reading this register
	#define S0SPSR_MODF       0x0010    // Mode fault. when 1, this bit indicates that a Mode fault error has occurred
	// This bit is cleared  by reading this register ,then writing the spio control register.
	#define S0SPSR_ROVR       0x0020    //Read overrun.when 1,this bit indicates that a read overrun hans occurred
	//This bit is cleared by reading this register.
	#define S0SPSR_WCOL       0x0040    //Write collision.When 1 ,this bit indicates that a write collision has occurred
	//This bit cleared by reading this register then accesing the spi data register
	#define S0SPSR_SPIF       0x0080    //spi transfre complete flag.
////end define SPI user register////


//   uc_SelectSpeed: 1:1M,2:2M,......, 18:18M;    选择速率 (SSP 0Hz---18MHz);
#define    SSPI_SelectSpeed      6
#define    SSPIChannel_0         0
#define    SSPIChannel_1         1
#define    SSPIChannel_2         2

////define SSP user register////
	/* SSP Status register */
	#define SSPSR_TFE		0x01
	#define SSPSR_TNF		0x02
	#define SSPSR_RNE		0x04
	#define SSPSR_RFF		0x08
	#define SSPSR_BSY		0x10
////end SSP user register////

//define spi macro definition
	#define DE_SPI_MASTER_MODE     0x20
	#define DE_SPI_CPOL     0x10
	#define DE_SPI_CPHA     0x08
	#define DE_SPI_LSBF     0x40

	#define DE_SPIF     0x80
//end define
  #define SPI0_SEL		  1 << 16	  // P0.16 is used as GPIO, CS signal to SPI ADC 2449

  #define SSPI_EEPROM     1 << 14     // p0.14 is used as GPIO, CS signal to SSPI EEPROM for CUSTOMER PROJECTION
  #define SSPI_SEL1		  1 << 16	  // P0.16 is used as GPIO, CS signal to SSPI ad5360 dac 1
  #define SSPI_SEL2		  1 << 11	  // P1.11 is used as GPIO, CS signal to SSPI ad5360 dac 2
//for SSP
	#define SSP0_REG_BASE  0xE0068000
	#define SSP1_REG_BASE  0xE0030000
	typedef struct
	{
		UWord32 CR0;//unsigned long CR0;
		UWord32 CR1;//unsigned long CR1;
		UWord32 DR;//unsigned long DR;
		UWord32 SR;//unsigned long SR;
		UWord32 CPSR;//unsigned long CPSR;
		UWord32 IMSC;//unsigned long IMSC;
		UWord32 RIS;//unsigned long RIS;
		UWord32 MIS;//unsigned long MIS;
		UWord32 ICR;//unsigned long ICR;
		UWord32 DMACR;//unsigned long DMACR;
	}Reg_SSPI;
	
	#define pSSP0 ((Reg_SSPI*)SSP0_REG_BASE)
	#define pSSP1 ((Reg_SSPI*)SSP1_REG_BASE)
//end for SSP

////////interrup////////
#ifdef SPI_INTERRUPT
#define SPI0_INT_FLAG	0x01

#define SPI0_ABORT		0x01		/* below two are SPI0 interrupt */
#define SPI0_MODE_FAULT	0x02
#define SPI0_OVERRUN	0x04
#define SPI0_COL		0x08
#define SPI0_TX_DONE	0x10
#endif

//parameter struct
	typedef struct 
	{
		unsigned char uc_Module_Name;
		//function name

		unsigned char uc_Speed;

		////SSP////
		//For SSPI 1:1M, 2:1.125M, 3:1.28M, 4:1.5M, 5:1.8M, 6:2.25M, 7:3M, 8:4.5M, 9:9M --- SSP Max Speed is PCLK_SSP/2 [1:9]
		
		unsigned char uc_CPOL;                             //SCK极性
		//CPOL  0 SCK为高电平有效
		//CPOL  1 SCK为低电平有效

		unsigned char uc_CPHA;                             //SCK相位
		//CPHA  0 数据在SCK第一个时钟采样，从SSEL信号激活时开始，并在SSEL无效时结束
		//CPHA  1 数据在SCK第二个时钟采样，当SSEL信号激活时，传输从第一个时钟沿开始并在最后一个采样时钟沿结束

		unsigned char uc_MSTR;                             //主从选择
		//0 从模式  
		//1 主模式

		unsigned char uc_BITS;                             //每帧位数 8～16
	    
		unsigned char uc_LSBF;                             //字节移动方向
		//0 MSB位7在先
		//1 LSB位0在先

		//SSPI_CS uc_SPI_EN;                             //使能端口
		//UInt16 IfSingleCS;				// if = 1, means single port CS, if 0, means 3-8 CS
		//G_Port     st_CS;
		//G_Port st_CS38[3];				// save the ports of 138 inputs, A~C : [0]~[2]
		//G_Port st_CS38EN;				// save the port of 138's Enable line, Low active.
		//UInt16 CS_Value;				// value for CS_38 array. bit 0~2 is the value of port st_CS38[0]~[2]

		//G_Port     st_SSEL;
// 		//在从模式下 SSEL的端口位
// 		//主模式忽略
        D_Port st_CS;
		D_Port st_SSEL;
	}SSPI_Para;

class SSPIFunc
{
    private:////private////
    static SSPI_Para  SSPI_Para_me[SSPI_MAX_CHANNEL];    //参数
	//SPIChannel:   0:SPI    1:SSP0    2:SSP1

	//static unsigned char myLen;    //用来检测当前len,避免错误发生    Comment out chen 0819

	static UWord32 TxCounter;
	//////// //////// //////// //////// //////// //////// //////// ////////
    public:////public////
    //static unsigned short SPIDataRXBuffer[SPI_MAX_BUFFER];

	//0928    修改  :  加入MyIO and SPI_ENABLE    修改其他的接口函数
	static unsigned char MyIOCount1;    //因为有三个通道
	static unsigned char MyIO1[3][4];    //因为有三个通道

	static unsigned char MyIOCount2;    //因为有三个通道
	static unsigned char MyIO2[3][4];    //因为有三个通道

	static unsigned char MyIOCount3;    //因为有三个通道
	static unsigned char MyIO3[3][4];    //因为有三个通道

	////////RX TX buffer////////
	static unsigned short SSPITxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];
	static unsigned short SSPIRxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];
  
	private:
	static void initSSPICLK(unsigned char SSPIChannel);

	//static void initSSPICS(unsigned char SSPIChannel);

	static void CS_Enable(unsigned char SSPIChannel);
	static void CS_Disable(unsigned char SSPIChannel);

	public:
	 ///////SSPI0 init///////////
	static void SSPI0_Init(void);
	 
	static void setPara(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
	//设置SSPI参数    SSPIChannel:   0:SPI    1:SSP0    2:SSP1
	static void setCS(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
  
	static void my_Init(unsigned char SSPIChannel);
	//注册SSPI设备   SSPIChannel:    0:SPI    1:SSP0    2:SSP1
	//特殊功能  SSPI控制寄存器  CS等信号

	static unsigned char Check(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
	//check: 0 相同    1 CS不同，set CS    2 全都不同，setPara

	static unsigned char getModuleName(unsigned char SSPIChannel);
	//获得当前SPI 通道的名字代码

	static unsigned short RxTx_Frame(unsigned short Txdata,unsigned char SSPIChannel);
    //one byte duplex

    static unsigned char SSPI_SendData(UChar8 SSPIChannel, UWord32 uw32_Length );
	
	//显示复制到buffer内，然后进行传输，返回的值存在SSPIRxBuff内，最长len为255；
	static void RxTx_Frames(unsigned short *SSPI_Tx,unsigned short len,unsigned char SSPIChannel);	    		

	static int RxTx_Frames_1CS(unsigned short *arry,unsigned short len,unsigned char SSPIChannel);

	static unsigned char SSPI_Write_Byte(unsigned char SSPIChannel,unsigned char uc_Byte)  ;
    static unsigned char SSPI_Read_Byte(unsigned char SSPIChannel);
	static void ClearFIFO(unsigned char SSPIChannel);

//SPI 复用问题
//static setCSIO(SPI_Port * p_SPI_Port);    //init
//check 一下
//static SPIEnable    //驱动IO 高    //放在gpio里
//static SPIDisable    //驱动IO 低
//static RxTx_Frame    //不把CS放里面，扔放外面
//

	//中断问题
	//static void SSP0_IRQHandler(void);

};
//end class



#endif//end #ifndef _SPI_2468_




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////ready to move other files
////将来转移走
////define macros for reg operate
//#define m_reg_read(reg) (*(volatile unsigned long *) (reg))
//#define m_reg_write(reg, data) ((*(volatile unsigned long *)(reg)) = (volatile unsigned long)(data))
//#define m_reg_bfs(reg, data) (*(volatile unsigned long *)(reg)) |= (data)                                                
//#define m_reg_bfc(reg, data) (*(volatile unsigned long *)(reg)) &=~ (data)                                            
////end define
/*
////SPI////
//SPI频率 = PCLK/S0SPCCR    PCLK 18M
//S0SPCCR = PCLK/SPI频率    S0SPCCR>=8且必须是偶数
//1M uc_Speed 18
//2M uc_Speed 9
//2.25M  uc_Speed 8  最快
////SSP////
//SSP频率 = PCLK/(CPSDVSR*[SCR+1])    PCLK 18M
//SCR已经被设置成0  PCLK/(CPSDVSR*1)    CPSDVSR主机min = 2
//1M uc_Speed 18
//2M uc_Speed 9
//9M uc_Speed 2  最快
*/
