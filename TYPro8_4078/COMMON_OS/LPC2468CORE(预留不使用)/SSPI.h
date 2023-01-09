
//1018  comment out chen
//����֤SSPʱ�ӹ�������

#ifndef _SSPI_
#define _SSPI_
/*#include "../COMMON/Datatypes.h"
#include "../COMMON/GPIO.h"
#include "../COMMON/LPC2468.h"*/

#include "../../COMMON/includes.h"

////user definition//    
//�жϻ�����ѯ
#define    SSPI_POLL                          //��ѯ
//#define    SSPI_INTERRUPT                     //�ж�


//����ʹ��ģ�鲻ͬ��������Ҫ�������
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
#define    SSPIMAX_TIMEOUT		  0x00000FFF    //ʹ��SPIʱ�����ĳ�ʱʱ��1ms
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


//   uc_SelectSpeed: 1:1M,2:2M,......, 18:18M;    ѡ������ (SSP 0Hz---18MHz);
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
		
		unsigned char uc_CPOL;                             //SCK����
		//CPOL  0 SCKΪ�ߵ�ƽ��Ч
		//CPOL  1 SCKΪ�͵�ƽ��Ч

		unsigned char uc_CPHA;                             //SCK��λ
		//CPHA  0 ������SCK��һ��ʱ�Ӳ�������SSEL�źż���ʱ��ʼ������SSEL��Чʱ����
		//CPHA  1 ������SCK�ڶ���ʱ�Ӳ�������SSEL�źż���ʱ������ӵ�һ��ʱ���ؿ�ʼ�������һ������ʱ���ؽ���

		unsigned char uc_MSTR;                             //����ѡ��
		//0 ��ģʽ  
		//1 ��ģʽ

		unsigned char uc_BITS;                             //ÿ֡λ�� 8��16
	    
		unsigned char uc_LSBF;                             //�ֽ��ƶ�����
		//0 MSBλ7����
		//1 LSBλ0����

		//SSPI_CS uc_SPI_EN;                             //ʹ�ܶ˿�
		//UInt16 IfSingleCS;				// if = 1, means single port CS, if 0, means 3-8 CS
		//G_Port     st_CS;
		//G_Port st_CS38[3];				// save the ports of 138 inputs, A~C : [0]~[2]
		//G_Port st_CS38EN;				// save the port of 138's Enable line, Low active.
		//UInt16 CS_Value;				// value for CS_38 array. bit 0~2 is the value of port st_CS38[0]~[2]

		//G_Port     st_SSEL;
// 		//�ڴ�ģʽ�� SSEL�Ķ˿�λ
// 		//��ģʽ����
        D_Port st_CS;
		D_Port st_SSEL;
	}SSPI_Para;

class SSPIFunc
{
    private:////private////
    static SSPI_Para  SSPI_Para_me[SSPI_MAX_CHANNEL];    //����
	//SPIChannel:   0:SPI    1:SSP0    2:SSP1

	//static unsigned char myLen;    //������⵱ǰlen,���������    Comment out chen 0819

	static UWord32 TxCounter;
	//////// //////// //////// //////// //////// //////// //////// ////////
    public:////public////
    //static unsigned short SPIDataRXBuffer[SPI_MAX_BUFFER];

	//0928    �޸�  :  ����MyIO and SPI_ENABLE    �޸������Ľӿں���
	static unsigned char MyIOCount1;    //��Ϊ������ͨ��
	static unsigned char MyIO1[3][4];    //��Ϊ������ͨ��

	static unsigned char MyIOCount2;    //��Ϊ������ͨ��
	static unsigned char MyIO2[3][4];    //��Ϊ������ͨ��

	static unsigned char MyIOCount3;    //��Ϊ������ͨ��
	static unsigned char MyIO3[3][4];    //��Ϊ������ͨ��

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
	//����SSPI����    SSPIChannel:   0:SPI    1:SSP0    2:SSP1
	static void setCS(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
  
	static void my_Init(unsigned char SSPIChannel);
	//ע��SSPI�豸   SSPIChannel:    0:SPI    1:SSP0    2:SSP1
	//���⹦��  SSPI���ƼĴ���  CS���ź�

	static unsigned char Check(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel);
	//check: 0 ��ͬ    1 CS��ͬ��set CS    2 ȫ����ͬ��setPara

	static unsigned char getModuleName(unsigned char SSPIChannel);
	//��õ�ǰSPI ͨ�������ִ���

	static unsigned short RxTx_Frame(unsigned short Txdata,unsigned char SSPIChannel);
    //one byte duplex

    static unsigned char SSPI_SendData(UChar8 SSPIChannel, UWord32 uw32_Length );
	
	//��ʾ���Ƶ�buffer�ڣ�Ȼ����д��䣬���ص�ֵ����SSPIRxBuff�ڣ��lenΪ255��
	static void RxTx_Frames(unsigned short *SSPI_Tx,unsigned short len,unsigned char SSPIChannel);	    		

	static int RxTx_Frames_1CS(unsigned short *arry,unsigned short len,unsigned char SSPIChannel);

	static unsigned char SSPI_Write_Byte(unsigned char SSPIChannel,unsigned char uc_Byte)  ;
    static unsigned char SSPI_Read_Byte(unsigned char SSPIChannel);
	static void ClearFIFO(unsigned char SSPIChannel);

//SPI ��������
//static setCSIO(SPI_Port * p_SPI_Port);    //init
//check һ��
//static SPIEnable    //����IO ��    //����gpio��
//static SPIDisable    //����IO ��
//static RxTx_Frame    //����CS�����棬�ӷ�����
//

	//�ж�����
	//static void SSP0_IRQHandler(void);

};
//end class



#endif//end #ifndef _SPI_2468_




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////ready to move other files
////����ת����
////define macros for reg operate
//#define m_reg_read(reg) (*(volatile unsigned long *) (reg))
//#define m_reg_write(reg, data) ((*(volatile unsigned long *)(reg)) = (volatile unsigned long)(data))
//#define m_reg_bfs(reg, data) (*(volatile unsigned long *)(reg)) |= (data)                                                
//#define m_reg_bfc(reg, data) (*(volatile unsigned long *)(reg)) &=~ (data)                                            
////end define
/*
////SPI////
//SPIƵ�� = PCLK/S0SPCCR    PCLK 18M
//S0SPCCR = PCLK/SPIƵ��    S0SPCCR>=8�ұ�����ż��
//1M uc_Speed 18
//2M uc_Speed 9
//2.25M  uc_Speed 8  ���
////SSP////
//SSPƵ�� = PCLK/(CPSDVSR*[SCR+1])    PCLK 18M
//SCR�Ѿ������ó�0  PCLK/(CPSDVSR*1)    CPSDVSR����min = 2
//1M uc_Speed 18
//2M uc_Speed 9
//9M uc_Speed 2  ���
*/
