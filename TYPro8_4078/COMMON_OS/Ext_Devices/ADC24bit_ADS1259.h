#pragma once

#define Adc24bit_ADS1259_TIME_OUT 50000
#define Adc24bit_Ads1259_MAX  12
#define Adc24bit_Ads1259_READ  11

//¶ÌÑÓ³Ù
#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );

class Adc24bit_ADS1259
{
public:
    static unsigned char      CodeType;
    static       St_Port      Pin_Conver;
    static       St_Port      Pin_Busy;
    static       St_Port      Pin_CS;
    static       St_Port      Pin_Rst;
    static unsigned char      SPI_Idx;
    static unsigned short  Ads1259reg[Adc24bit_Ads1259_MAX];
    static unsigned short  ReadData[Adc24bit_Ads1259_READ];

public:
    static void            Init        (unsigned char ConvCodeType,unsigned short SPS);
#if  USE_AS_AUXMCU == 1
    static void            InitExtBase(unsigned char ConvCodeType,unsigned short SPS);
#endif
    static void            Convert     (void);
    static unsigned long   Read        (unsigned char sspId);
    //    static void			   ReadReg     (unsigned char sspId);
    static void            WriteReg	   (unsigned char sspId);
    static void            Reset       (void);
    //    static void			   SetChannel  (unsigned char uc_ChannelNo);
// 	static void            Trigger(void);  // uc_ChannelNo is not used.  This ADC has one channel only

};
