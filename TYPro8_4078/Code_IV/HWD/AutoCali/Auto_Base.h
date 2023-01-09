//自动校准Hpp头文件
//comment out chen 20130422
//comment out chen 20130425 updata
//FRAM(FM25L256)  SCK1 MOSI1 MISO1
//DAC AD5754         SCK   MOSI  MISO
//ADC AD7190         SCK0 MOSI0 MISO0
//使用器件

#ifndef _AUTO_CALI_BOARD_H_
#define _AUTO_CALI_BOARD_H_

//xxx模式-off模式-xxx模式的转换等待时间
#define AUTO_CALI_TRAN_TIME    80    //10*100us = 1ms  10ms

//自动校准CPLD地址，32bit组成整个逻辑关系，详情请参考刘工写的《自动校准模块底层硬件驱动配置.xlsx》
#define AUTO_CALI_CPLD_ADD       (0x81000000)
#define AUTO_CALI_CPLD_ADD_2    (0x81000002)

//地址读写操作
#define cpu_iord_16(ADDR)       *((volatile uint16 *)(ADDR))
#define cpu_iowr_16(ADDR,DATA)  *((volatile uint16 *)(ADDR)) = (DATA)
#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );

//CPLD继电器位操作定义，详情请参考刘工写的《自动校准模块底层硬件驱动配置.xlsx》    comment out chen 20130424更新
//ADD1 0x81000000  ADD2 0x81000002
#define AUTO_CALI_CPLD_OFF_ADD1				   (0)
#define AUTO_CALI_CPLD_OFF_ADD2                  (0)    //(1<<9)
#define AUTO_CALI_CPLD_SELF_HIGH_ADD1        (1<<0 | 1<<2)    //is ok
#define AUTO_CALI_CPLD_SELF_HIGH_ADD2		   (1<<4 | 1<<10)
#define AUTO_CALI_CPLD_SELF_MID_ADD1         (1<<2 | 1<<6)
#define AUTO_CALI_CPLD_SELF_MID_ADD2         (1<<5 | 1<<10)
#define AUTO_CALI_CPLD_SELF_LOW_ADD1        (1<<2 | 1<<7)
#define AUTO_CALI_CPLD_SELF_LOW_ADD2        (1<<0 | 1<< 10)
#define AUTO_CALI_CPLD_SELF_DAC_ADD1         (1<<3)
#define AUTO_CALI_CPLD_SELF_DAC_ADD2         (1<<11 | 1<<12)

//comment out chen 20130815 test
//<<<LATER>>>
#define AUTO_CALI_CPLD_HP_HIGH_ADD1          (1<<0 | 1<<8 | 1<<14 | 1<<13)    //K30+K18
#define AUTO_CALI_CPLD_HP_HIGH_ADD2          (1<<4 | 1<<12)
#define AUTO_CALI_CPLD_HP_MID_ADD1            (1<<6 | 1<<8 | 1<<14 | 1<<13)
#define AUTO_CALI_CPLD_HP_MID_ADD2			   (1<<5 | 1<<12)
#define AUTO_CALI_CPLD_HP_LOW_ADD1           (1<<7 |1<<8 | 1<<14 | 1<<13) 
#define AUTO_CALI_CPLD_HP_LOW_ADD2           (1<<0  | 1<<12)
//<<<LATER>>>

#define AUTO_CALI_CPLD_HP_IVSET_ADD1          (1<<3 | 1<<5 | 1<<13 | 1<<14)    //modify comment out chen 20130717
#define AUTO_CALI_CPLD_HP_IVSET_ADD2          (1<<3) 
#define AUTO_CALI_CPLD_HP_DAC_ADD1			   (1<<3 | 1<<13)
#define AUTO_CALI_CPLD_HP_DAC_ADD2            (1<<12)
#define AUTO_CALI_CPLD_HP_CLV_ADD1             (1<<3 | 1<<4 | 1<<13 | 1<<14)
#define AUTO_CALI_CPLD_HP_CLV_ADD2             (1<<2)

#define AUTO_CALI_CPLD_ADC_HIGH_ADD1        (1<<0 | 1<<14)
#define AUTO_CALI_CPLD_ADC_HIGH_ADD2        (1<<4 | 1<<10)
#define AUTO_CALI_CPLD_ADC_MID_ADD1         (1<<6 | 1<<14)
#define AUTO_CALI_CPLD_ADC_MID_ADD2         (1<<5 | 1<<10)
#define AUTO_CALI_CPLD_ADC_LOW_ADD1         (1<<7 | 1<<14)
#define AUTO_CALI_CPLD_ADC_LOW_ADD2         (1<<0 | 1<<10)
#define AUTO_CALI_CPLD_ADC_IVSET_ADD1       (1<<5 | 1<<13)
#define AUTO_CALI_CPLD_ADC_IVSET_ADD2        (1<<3 | 1<<11)
#define AUTO_CALI_CPLD_ADC_DAC_ADD1          (1<<13)
#define AUTO_CALI_CPLD_ADC_DAC_ADD2          (1<<10 | 1<<11)
#define AUTO_CALI_CPLD_ADC_CLV_ADD1           (1<<4 | 1<<13 | 1<<14)
#define AUTO_CALI_CPLD_ADC_CLV_ADD2           (1<<2 | 1<<9)

#define AUTO_CALI_CPLD_CELL_BY_PASS_ADD1             (1<<15)
#define AUTO_CALI_CPLD_CELL_BY_PASS_ADD2             (0)

#define AUTO_CALI_CPLD_CELL_IV_ADD1            (0)
#define AUTO_CALI_CPLD_CELL_IV_ADD2            (1<<8)

#define AUTO_CALI_CPLD_COM_ADD1    (0)
#define AUTO_CALI_CPLD_COM_ADD2    (1<<7)
#define AUTO_CALI_CPLD_VPP_ADD1      (0)
#define AUTO_CALI_CPLD_VPP_ADD2      (1<<8)

//电池模式
#define AUTO_CALI_CPLD_CELL_MODULE_IV
#define AUTO_CALI_CPLD_CELL_MODULE_BY_PASS
#define AUTO_CALI_CPLD_CELL_MODULE_CHR
#define AUTO_CALI_CPLD_CELL_MODULE_DISCHR

//AD5754
#define Auto_Cali_DCS3              //p0.16
#define Auto_Cali_DAC_LOAD1    //p1.3
#define Auto_Cali_DAC_CLR        //p1.5
#define Auto_Cali_DAC_REST      //p1.6
#define Auto_Cali_BUSY3           //p1.2

//自动校准板量程范围
#define Auto_Cali_HIGH_Rang         30.0f
#define Auto_Cali_Mid_Rang           5.0f
#define Auto_Cali_Low_Rang           0.1f
#define Auto_Cali_Voltage_Rang     5.0f   //电压量程

//DAC输出比例    comment out chen 20130530
#define Auto_DAC_out_Value1    0.80f
#define Auto_DAC_out_Value2    0.20f
#define Auto_DAC_out_Value3   -0.80f
#define Auto_DAC_out_Value4   -0.20f
#define Auto_DAC_out_Range    10.0f

//
enum
{
	AUTO_SPI_MODULE_NAME_AD5754 = 0xA0,
	AUTO_SSP0_MODULE_NAME,
	AUTO_SSP1_MODULE_NAME,
	AUTO_SPI_MODULE_NAME_AD7190,    //DAC
};

//自动校准板工作模式，参考刘工的底层硬件驱动
enum
{
	AUTO_CALI_OFF = 0xA0,    //off模式
	AUTO_CALI_SELF_HIGH,
	AUTO_CALI_SELF_MID,
	AUTO_CALI_SELF_LOW,
	AUTO_CALI_SELF_DAC,
	AUTO_CALI_HP_HIGH,
	AUTO_CALI_HP_MID,
	AUTO_CALI_HP_LOW,
	AUTO_CALI_HP_IV_SET,
	AUTO_CALI_HP_DAC,
	AUTO_CALI_HP_CLV,
	AUTO_CALI_ADC_HIGH,
	AUTO_CALI_ADC_MID,
	AUTO_CALI_ADC_LOW,
	AUTO_CALI_ADC_IV_SET,
	AUTO_CALI_ADC_DAC,
	AUTO_CALI_ADC_CLV,    //CLV钳压

	//////////
	AUTO_CALI_BAT,    //电池模式
	AUTO_CALI_COM,  //v++ com
};

typedef struct  
{
	unsigned short ADR1;    //0x8100 0000
	unsigned short ADR2;    //0x8100 0002
}ST_Auto_CPLD_Status;

////////////////////
#define ADC_INTERRUPT_FLAG	1	/* 1 is interrupt driven, 0 is polling */

#define ADC_OFFSET		0x10
#define ADC_INDEX		4

#define ADC_DONE		0x80000000
#define ADC_OVERRUN		0x40000000
#define ADC_ADINT		0x00010000

#define ADC_NUM			8		/* for LPC23xx */
#define ADC_CLK			1000000		/* set to 1Mhz */
////////////////////

class Auto_Cali_Base
{
public:
	static void Hardware_Init(void);
	static void Software_Init(void);

	//各种模块驱动    //....
	static void gotoOFF(void);
	static void gotoSELFHigh(void);
	static void gotoSELFMid(void);
	static void gotoSELFLow(void);
	static void gotoSELFDac(void);

	static void gotoHPHigh(void);
	static void gotoHPMid(void);
	static void gotoHPLow(void);
	static void gotoHPIVSet(void);
	static void gotoHPDac(void);
	static void gotoHPCLV(void);

	static void gotoADCHigh(void);
	static void gotoADCMid(void);
	static void gotoADCLow(void);
	static void gotoADCIVSet(void);
	static void gotoADCDac(void);
	static void gotoADCCLV(void);

	static void gotoCOM(void);
	static void gotoVPP(void);
	static void gotoCELL_Bypass(void);    //电池旁路
	static void gotoCELL_IV(void);
	//and others
	//

	static void watchDog(void);

	static unsigned char getWorkModule(void);    //获取工作状态
	static unsigned char getRange(void);    //获取当前量程

	static unsigned long CalibrateDac(float fdac);
	//thirdParty::Dac_set_value
	static char DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type);

	//存放作为ADC使用时的参数
	static float ADC_Gain;
	static float ADC_Offset;


private:
	//无论是电池状态，com v++都可以通过他改写
	static ST_Auto_CPLD_Status st_auto_cpld;    //保存历史状态

	//hardware  把这部分扔到别的文件夹去
	static void IO_Init(void);
	static void ADC_Init(void);
	static void DAC_Init(void);

	static unsigned char RangeStatus;

	static void AD0_Init(unsigned long ADC_clk);    //板内AD0 P0.23


	static unsigned char m_controlledFlag;
};

#endif
