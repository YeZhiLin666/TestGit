/*****************************************************************************
// 程序文件      :Adc24bit_AD7175.h     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片AD7606驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/
#pragma once
/*********************************************************************************
**                            Adc pin  Define
*********************************************************************************/


#define  ADC_DELAYTIME		100

typedef struct
{
    unsigned short    RA:6;
    unsigned short    RW:1;
    unsigned short    WEN:1;
    unsigned short    rvd:8;
} St_AD7175_COMMS;

typedef struct
{
    unsigned short    AINNEG0:5;
    unsigned short    AINPOS0:5;
    unsigned short    RVD:2;
    unsigned short    SETUP_SEL:2;
    unsigned short    RVD2:1;
    unsigned short    CH_EN0:1;
} St_AD7175_CH;

typedef struct
{
    unsigned short    RVD:4;
    unsigned short    REF_SEL0:2;
    unsigned short    RVD2:1;
    unsigned short    BURNOUT_EN0:1;
    unsigned short    AINBUF0NEG:1;
    unsigned short    AINBUF0POS:1;
    unsigned short    REFBUF0NEG:1;
    unsigned short    REFBUF0POS:1;
    unsigned short    BI_UNIPOLAR0:1;
    unsigned short    RVD3:3;
} St_AD7175_SETUPCON;

typedef struct
{
    unsigned short    ODR0:5;
    unsigned short    ORDER0:2;
    unsigned short    RVD:1;
    unsigned short    ENHFILT0:3;
    unsigned short    ENHFILTEN0:1;
    unsigned short    RVD2:3;
    unsigned short    SINC3_MAP0:1;
} St_AD7175_FILTCON;

typedef struct
{
    unsigned short    RVD:2;
    unsigned short    CLOCKSEL:2;
    unsigned short    MODE:3;
    unsigned short    RVD2:1;
    unsigned short    DELAY:3;
    unsigned short    RVD3:2;
    unsigned short    SING_CYC:1;
    unsigned short    HIDE_DELAY:1;
    unsigned short    REF_EN:1;
} St_AD7175_ADCMODE;

typedef struct
{
    unsigned short    WL16:1;
    unsigned short    RVD:1;
    unsigned short    CRC_EN:2;
    unsigned short    RVD2:1;
    unsigned short    REG_CHECK:1;
    unsigned short    DATA_STAT:1;
    unsigned short    CONTREAD:1;
    unsigned short    DOUT_RESET:1;
    unsigned short    RVD3:2;
    unsigned short    IOSTRENGTH:1;
    unsigned short    ALT_SYNC:1;
    unsigned short    RVD4:3;
} St_AD7175_IFMODE;

typedef union
{
    unsigned short      reg;

    St_AD7175_COMMS     comms;
    St_AD7175_CH        ch;
    St_AD7175_SETUPCON  setupcon;
    St_AD7175_FILTCON   filtcon;
    St_AD7175_ADCMODE   adcmode;
    St_AD7175_IFMODE    ifmode;
} un_AD7175;

enum
{
    eAD7175_COMMSorSTATUS  = 0,
    eAD7175_ADCMODE        = 1,
    eAD7175_IFMODE         = 2,
    eAD7175_REGCHECK       = 3,
    eAD7175_DATA           = 4,
    eAD7175_GPIOCON        = 6,
    eAD7175_ID             = 7,
    eAD7175_CH0            = 0x10,
    eAD7175_CH1            = 0x11,
    eAD7175_CH2            = 0x12,
    eAD7175_CH3            = 0x13,
    eAD7175_SETUPCON0      = 0x20,
    eAD7175_SETUPCON1      = 0x21,
    eAD7175_SETUPCON2      = 0x22,
    eAD7175_SETUPCON3      = 0x23,
    eAD7175_FILTCON0       = 0x28,
    eAD7175_FILTCON1       = 0x29,
    eAD7175_FILTCON2       = 0x2A,
    eAD7175_FILTCON3       = 0x2B,
    eAD7175_OFFSET0        = 0x30,
    eAD7175_OFFSET1        = 0x31,
    eAD7175_OFFSET2        = 0x32,
    eAD7175_OFFSET3        = 0x33,
    eAD7175_GAIN0          = 0x38,
    eAD7175_GAIN1          = 0x39,
    eAD7175_GAIN2          = 0x3A,
    eAD7175_GAIN3          = 0x3B,
};

class Adc24bit_AD7175
{
    //Attribute
public:
    static St_Port	ADC_DRDY;
    static St_Port 	ADC_START;
    static St_Port	ADC_CS;
    static St_Port	ADC_Rst;
    static unsigned char CodeType;
    static unsigned long Address;
//	static unsigned char SPI_Idx;

public:
    //static void          Init2(unsigned char ConvCodeType);
#if (USE_AS_AUXMCU == 1)
//  static void            Reset2(unsigned char spi_idx);
    static void            Init_Single_mode (unsigned char ConvCodeType,unsigned char SPI_Idx);
#endif
    static void            Reset2(unsigned char spi_idx);
    static void            ResetForBuiltIn(unsigned char spi_idx);
//   static void            ChPinMap(unsigned char ChanIdx, unsigned char PosIdx, unsigned char NegIdx, bool bEnable,unsigned char SPI_Idx);
//   static void            Convert();
    //static void            Trigger2(void);
    //static unsigned long   ReadData2(void);

    static void            Init          (unsigned char ConvCodeType ,unsigned char sspId );
    static void            Init_diffmode      (unsigned char ConvCodeType,unsigned char SPI_Idx);
    static void            Init_FFT_mode (unsigned char ConvCodeType,unsigned char SPI_Idx);
    static void            Init_diffmode_BUILTIN (unsigned char ConvCodeType,unsigned char SPI_Idx);
    static void            Trigger       (unsigned char sspId);
    static void            WaitForRead (unsigned char SPI_Idx);
    static unsigned long   Read          (unsigned char sspId);
// 	static unsigned long   Read_ADC_Mode (unsigned char sspId);

//     static void            ReadAfterBusy (unsigned long *Data1,unsigned long *Data2);
//     static void            Reset         (void);
// static void 						Assign_adc_chn ( unsigned char adc_chn_num, unsigned char SPI_Idx);
};

