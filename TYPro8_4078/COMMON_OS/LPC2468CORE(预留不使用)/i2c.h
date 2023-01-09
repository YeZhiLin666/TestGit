

#ifndef _I2C_H_
#define _I2C_H_

/*
暂时先考虑主机操作：主发送模式和主接收模式
SDA和SCL
准备加入3个通道的处理
*/

////EEPROM info////
/*
对于24LC128来讲    1 0 1 0 A2 A1 A0	R/W        slave address
read是1    write是0
*/
////end EEPROM info////


////user definition////
#define I2CENABLE 1//for gpio

#define MAX_I2C_BUFFER  64
#define MAX_I2C_COUNT    2

 //    uc_SelectSpeed: 1:100K, 2:200K, 3:300K, 4:400K;    选择速率 (I2Cbitfreq 0Hz---400KHz);
#define I2S_SelectSpeed			3   

//100 大概是30多K    改成300K 1E   还是能连上的    不过飞线原因    还是用慢点观察比较清楚    回来该回去    comment out chen 1104
#define I2SCLH_SCLH			0x00000100  /* I2C SCL Duty Cycle High Reg */    
#define I2SCLL_SCLL			0x00000100  /* I2C SCL Duty Cycle Low Reg */
//计算方法  ： pclk/(SCLH+SCLL)

////end user definition////


//channel
#define I2C_MAX_CHANNEL  3//i2c0 i2c1 i2c2:SDA0 SDA1 SDA2  SCL0 SCL1 SCL2

//just for I2C
#define I2C_IDLE  0
#define I2C_TRAN_DONE  1
#define I2C_DATA_ACK  2
#define I2C_SLAVE_NACK  4
#define I2C_REPEATED_START  8//比较特殊
//16
//32
#define I2C_ERROR  64
//128
//预留


#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020
//没有NACK

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040



//预留
#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */


//class
class I2CFunc
{
	//data
    private:
	//考虑分开 writeAddress和readAddress    Length同
	static unsigned char I2CG_Address[I2C_MAX_CHANNEL];

	static unsigned char I2CWrite_Length[I2C_MAX_CHANNEL];
	static unsigned char I2CRead_Length[I2C_MAX_CHANNEL];

	static unsigned char BlockFlag[I2C_MAX_CHANNEL];
	static unsigned char DataBuf[I2C_MAX_CHANNEL];

	volatile static unsigned char I2CMasterState[I2C_MAX_CHANNEL];
 	volatile static unsigned char I2CIsBusy[I2C_MAX_CHANNEL];
	volatile static unsigned char I2CIsBuffer[I2C_MAX_CHANNEL];
	volatile static unsigned char FirstWriteFlag[I2C_MAX_CHANNEL];
	//指向缓冲区数组的指针
	static unsigned char pReadBuf[I2C_MAX_CHANNEL];
	static unsigned char pWriteBuf[I2C_MAX_CHANNEL];


    public:
	//3个I2C接口放在一起注册
  static unsigned char MyIOCount;
	static unsigned char MyIO[][4];

	static unsigned char I2CReadBuffer[I2C_MAX_CHANNEL][MAX_I2C_BUFFER];
	static unsigned char I2CWriteBuffer[I2C_MAX_CHANNEL][MAX_I2C_BUFFER];
	static unsigned char ReadData[I2C_MAX_CHANNEL];//用于单个的

	//fuction
    private:

    public:
	
	static UWord32 my_Init(unsigned char chn);

	//中断函数
	static void I2C0HandlerISR(void)__irq;
	static void I2C1HandlerISR(void)__irq;
	static void I2C2HandlerISR(void)__irq;

	static UWord32 I2CReadWord(unsigned char addr,unsigned char chn);
	static UWord32 I2CReadBlock(unsigned char addr,unsigned char len,unsigned char chn);
	static UWord32 I2CWriteWord(unsigned char addr,unsigned char value,unsigned char chn);
	static UWord32 I2CWriteBlock(unsigned char addr,unsigned char array[],unsigned char len,unsigned char chn);

	
	static UWord32 checkStatus(unsigned char chn);
	//或许加入is busy？？
	static UWord32 isBusy(unsigned char chn);
	static UWord32 isUseBuf(unsigned char chn);
	static void clearBuf(unsigned char chn);

	static void clearI2C(unsigned char chn);
	//先写再读，中间没有STO标志，为了适应特殊的芯片
	//中断程序在0x28和0x10处加了些处理
	static UWord32 FirstWriteThenRead(unsigned char addr,unsigned char WriteBuf[],unsigned char writeLen,
														 unsigned char readLen,unsigned char chn);

};





//#ifndef 24LC128EEPROM
//#define 24LC128EEPROM
//
//#endif


#endif//end		 _I2C_H_
