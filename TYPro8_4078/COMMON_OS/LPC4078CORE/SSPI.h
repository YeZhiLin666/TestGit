
//1018  comment out chen
//不保证SSP时钟工作正常

#ifndef _SSPI_H_
#define _SSPI_H_
#include "../Data_Manipulation/Datatypes.h"
#include "../LPC4078CORE/GPIO.h"
//#include "../COMMON/LPC177x_8x.h"
#include "../LPC4078CORE/LPC407x_8x_177x_8x.h"


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

enum  eSSP_DEVICE
{
    SSP_DEVICE_NULL,
    SSP0_DEVICE0 = 0xA0,
    SSP0_DEVICE1,          // IV-DAC  ;//  AUX-ADC-T
    SSP0_DEVICE2,          // FRAM
    SSP0_DEVICE3,
    SSP0_DEVICE4,          //BUILTIN Voltage  AD7175

    SSP1_DEVICE0 = 0xB0,
    SSP1_DEVICE1,           //IV-ADC  ;//  AUX-ADC-2VD
    SSP1_DEVICE2,
    SSP1_DEVICE3,
};
//
//#define SSP0_DEVICE 0x00
//#define SSP1_DEVICE 0x01
//#define SSP2_DEVICE 0x00

//data define
#define    SSPI_MAX_BUFFER       50
#define    SSPI_MAX_LEN          50
#define    SSPI_MAX_CHANNEL       3
#define    Dummy_Byte             0xa5
#define    SSPCLRAR               0
#define    SSPIMAX_TIMEOUT		  0x00001FFF    //使用SPI时，最大的超时时间1ms
#define    SSPI_FIFOSIZE           8

//   uc_SelectSpeed: 1:1M,2:2M,......, 18:18M;    选择速率 (SSP 0Hz---18MHz);
#define    SSPI_SelectSpeed      6
#define    SSPIChannel_0         0
#define    SSPIChannel_1         1
#define    SSPIChannel_2         2

////define SSP user register////
/* SSP Status register */
#define SSPSR_TFE		0x01
#define SSPSR_TNF		0x02
#define SSPSR_RNE		0x00000004
#define SSPSR_RFF		0x08
#define SSPSR_BSY		0x00000010
////end SSP user register////


#define SPI0_SEL		  1 << 16	  // P0.16 is used as GPIO, CS signal to SPI ADC 2449

#define SSPI_EEPROM     1 << 14     // p0.14 is used as GPIO, CS signal to SSPI EEPROM for CUSTOMER PROJECTION
#define SSPI_SEL1		  1 << 16	  // P0.16 is used as GPIO, CS signal to SSPI ad5360 dac 1
#define SSPI_SEL2		  1 << 11	  // P1.11 is used as GPIO, CS signal to SSPI ad5360 dac 2

//for SSP
//#define SSP0_REG_BASE  LPC_SSP0_BASE
//#define SSP1_REG_BASE  LPC_SSP1_BASE
//#define SSP2_REG_BASE  LPC_SSP2_BASE

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
} Reg_SSPI;

#define pSSPI0  ((Reg_SSPI*) LPC_SSP0_BASE)
#define pSSPI1  ((Reg_SSPI*) LPC_SSP1_BASE)
//	#define pSSPI2  ((Reg_SSPI*) LPC_SSP1_BASE)               //此处复用SSP0

//end for SSP

////////interrup////////
// #ifdef SSPI_INTERRUPT
// #define SSPI_INT_RORIC	    0x01        //  SSPn Interrupt Clear Register SSP0ICR   //
// #define SSPI_INT_RTIC	    0x02        //  SSPn Interrupt Clear Register SSP0ICR   //

// #define SSPI_RORMIS	        0x01		//   SSPn Masked Interrupt Status Register SSP0MIS  //
// #define SSPI_RTMIS	        0x02
// #define SSPI_RXMIS	        0x04
// #define SSPI_TXMIS		    0x08

// #endif

//parameter struct
struct SSPI_Para
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
    // 0 从模式
    // 1 主模式

    unsigned char uc_BITS;                             //每帧位数 8～16

    unsigned char uc_LSBF;                             //字节移动方向
    // 0 MSB位7在先
    // 1 LSB位0在先

    //SSPI_CS uc_SPI_EN;                             //使能端口
    //	UInt16 IfSingleCS;				// if = 1, means single port CS, if 0, means 3-8 CS
    //		G_Port     st_CS;
    //	G_Port st_CS38[3];				// save the ports of 138 inputs, A~C : [0]~[2]
    //	G_Port st_CS38EN;				// save the port of 138's Enable line, Low active.
    //	UInt16 CS_Value;				// value for CS_38 array. bit 0~2 is the value of port st_CS38[0]~[2]

    //	G_Port     st_SSEL;
    //在从模式下 SSEL的端口位
    //主模式忽略
// 	D_Port st_CS;
// 	D_Port st_SSEL;
};

class SSPIFunc
{
public:
    static SSPI_Para  SSPI_Para_me[SSPI_MAX_CHANNEL];    //参数
// 	static UWord32 TxCounter;
// 	static unsigned char  bBusy[SSPI_MAX_CHANNEL];
    static unsigned short SSPITxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];
    static unsigned short SSPIRxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];

// private:
// 	static void initSSPICLK(unsigned char SSPIChannel);

public:
// 	static void Init(void);
// 	static void SSPI0_Init(void);
    static void setPara(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
    static void my_Init(unsigned char SSPIChannel);
// 	static unsigned char Check(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
    static unsigned char getModuleName(unsigned char SSPIChannel);
    static unsigned short RxTx_Frame(unsigned short Txdata,unsigned char SSPIChannel);
    static void WriteTxFIFO(unsigned short *pTx,unsigned short len,unsigned char SSPIChannel);
    static void ReadRxFIFO(unsigned short len,unsigned char SSPIChannel);
    static unsigned char SSPI_SendData(UChar8 SSPIChannel, UWord32 uw32_Length );
    static void RxTx_Frames(unsigned short *pTx,unsigned short len,unsigned char SSPIChannel);
// 	static unsigned char SSPI_Write_Byte(unsigned char SSPIChannel,unsigned char uc_Byte)  ;
// 	static unsigned char SSPI_Read_Byte(unsigned char SSPIChannel);
    static void ClearFIFO(unsigned char SSPIChannel);
    static void ConfirmDevice( unsigned char SSPIChannel,SSPI_Para * p_SSPI_Para );
};
//end class

#endif//end #ifndef _SPI_2468_
