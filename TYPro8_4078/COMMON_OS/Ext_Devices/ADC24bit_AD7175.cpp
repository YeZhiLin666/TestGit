/*****************************************************************************
// 程序文件      :ADC24bit_AD7175.cpp     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片AD7175驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2015.10.27
*****************************************************************************/

#include "includes.h"

unsigned char Adc24bit_AD7175::CodeType;

St_Port	Adc24bit_AD7175::ADC_DRDY;
St_Port Adc24bit_AD7175::ADC_START;
St_Port	Adc24bit_AD7175::ADC_CS;
St_Port	Adc24bit_AD7175::ADC_Rst;

//unsigned char  Adc24bit_AD7175::SPI_Idx;

/*
void           Adc24bit_AD7175::Init2(unsigned char ConvCodeType)
{
  switch(ConvCodeType)
  {
  case eCONVCODETYPE_BIPOLAR:
    CodeType= eCONVCODETYPE_BIPOLAR;
    break;
  case eCONVCODETYPE_UNIPOLAR:
  default:
    CodeType = eCONVCODETYPE_UNIPOLAR;
    break;
  }

  Reset2();
  //return;
  unsigned short SPI_Send_Buffer[3];
  ControlPin::SetMeLow(ADC_CS);
  SPI_Send_Buffer[0] = 0x20;
  SPI_Send_Buffer[1] = 0x03;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
  SPI_Send_Buffer[2] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100
  SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
  ControlPin::SetMeHigh(ADC_CS);
  TIMERSFunc::DELAY_SYS(20, 0);

              ControlPin::SetMeLow(ADC_CS);
           SPI_Send_Buffer[0] = 0x01;    //Write ADC mode register
           SPI_Send_Buffer[1] = 0x20;    // 禁止内部参考，16us Delay Conversion,  SINF_CYC :Enable
           SPI_Send_Buffer[2] = 0x00;    //使用内部时钟,Single Conversion mode // qjm 0107
           SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,1);    //send data
           ControlPin::SetMeHigh(ADC_CS);
           TIMERSFunc::DELAY_SYS(20, 0);


  ControlPin::SetMeLow(ADC_CS);
  SPI_Send_Buffer[0] = 0x02;  //Write ADC mode register
  SPI_Send_Buffer[1] = 0x00;   // Alt_SYNC:  Disable  IOSTRENGGH Disable
  SPI_Send_Buffer[2] = 0x00;    //DATA_STAT: Disable  CRC_EN :Disable
  SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
  ControlPin::SetMeHigh(ADC_CS);
  TIMERSFunc::DELAY_SYS(20, 0);

}
*/    //don't know who write this code, and not used
void  Adc24bit_AD7175::Reset2(unsigned char spi_idx)
{
    unsigned short SPI_Send_Buffer[16];
    ControlPin::SetMeLow(ADC_CS);
    for (unsigned char i=0; i<16; i++)
    {
        SPI_Send_Buffer[i] = 0xFF;
    }

    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,16,spi_idx);    //send data
    ControlPin::SetMeHigh(ADC_CS);

    TIMERSFunc::DELAY_SYS(1000, 0);
}

void  Adc24bit_AD7175::ResetForBuiltIn(unsigned char spi_idx)
{
    unsigned short SPI_Send_Buffer[16];
//   ControlPin::SetMeLow(ADC_CS);
    for (unsigned char i=0; i<16; i++)
    {
        SPI_Send_Buffer[i] = 0xFF;
    }

    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,16,spi_idx);    //send data
//   ControlPin::SetMeHigh(ADC_CS);

    TIMERSFunc::DELAY_SYS(1000, 0);

}
// void           Adc24bit_AD7175::ChPinMap(unsigned char ChanIdx, unsigned char PosIdx, unsigned char NegIdx, bool bEnable,unsigned char SPI_Idx)
// {
//   if(ChanIdx >= 4)
//     return;
//
//   un_AD7175 myCh;
//   myCh.reg = 0;
//   myCh.ch.CH_EN0 = bEnable;
//   myCh.ch.SETUP_SEL = ChanIdx;
//   myCh.ch.AINNEG0 = NegIdx;
//   myCh.ch.AINPOS0 = PosIdx;
//
//   unsigned short SPI_Send_Buffer[3];
//   SPI_Send_Buffer[0] = eAD7175_CH0 + ChanIdx;
//   SPI_Send_Buffer[1] = myCh.reg >> 8;
//   SPI_Send_Buffer[2] = myCh.reg;
//   ControlPin::SetMeLow(ADC_CS);
//   SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);
//   ControlPin::SetMeHigh(ADC_CS);
// }

// please define SPI_Idx as an input parameter

/*void           Adc24bit_AD7175::Convert()
{
//   unsigned short SPI_Send_Buffer[3];
//   ControlPin::SetMeLow(ADC_CS);
//   SPI_Send_Buffer[0] = 0x01;    //Write ADC mode register
//   SPI_Send_Buffer[1] = 0x00;    // 禁止内部参考，16us Delay Conversion,  SINF_CYC :Disable
//   SPI_Send_Buffer[2] = 0x10;    //使用内部时钟,Single Conversion mode // qjm 0107
//   SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
//   ControlPin::SetMeHigh(ADC_CS);
//   TIMERSFunc::DELAY_SYS(20, 0);

  ControlPin::SetMeLow(ADC_START);
  TIMERSFunc::DELAY_SYS(10, 0);
  ControlPin::SetMeHigh(ADC_START);
}*/

/*
void           Adc24bit_AD7175::Trigger2(void)
{
  unsigned short Tx[4];//,Rx[3];

  Tx[0] = 0x40 + eAD7175_DATA;
  Tx[1] = 0;
  Tx[2] = 0;
  Tx[3] = 0;
  ControlPin::SetMeLow(ADC_CS);
  SSPIFunc::WriteTxFIFO(Tx,4,SPI_Idx);
}
*/    // dont't know who wrote this code, and never used

/*
unsigned long  Adc24bit_AD7175::ReadData2(void)
{
  unsigned long int  temp = 0;    //返回的值
  unsigned char Rx[4];

  SSPIFunc::ReadRxFIFO(4,SPI_Idx);
  ControlPin::SetMeHigh(ADC_CS);

  for(char i =0;i<4;i++)
    Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //接收数据

  //把Rx组成成为u32
  temp |= Rx[1]<<16;
  temp |= Rx[2]<<8;
  temp |= Rx[3];

  if(CodeType == eCONVCODETYPE_BIPOLAR)
    temp  = CommonFunc::Bipolar2Unipolar24BitCode(temp);
  //else 如果ADC本身读回来的就是Unipolar的，那么就什么也不用做了

  return temp;
}
*/   //don't know who wrote this code, and never used

void           Adc24bit_AD7175::Init        (unsigned char ConvCodeType,unsigned char SPI_Idx)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
    case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }

//    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    //初始化 GPIO Reg.
    SPI_Send_Buffer[0] = 0x06;  //Write GPIO register
    SPI_Send_Buffer[1] = 0x08;  //MUX_IO: disable , SYNC_EN =1 ,
    SPI_Send_Buffer[2] = 0x00;   //  0x30;  //  IP_EN1=1,IP_EN0=1, 设置外部通道切换器，初始化为第0通道
    ControlPin::SetMeLow(ADC_CS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Channel 0 Reg.
    SPI_Send_Buffer[0] = 0x10;
    SPI_Send_Buffer[1] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[2] = 0x20;    // AIN - : AIN0,  AIN + :AIN1 ;  SETUP: Setup0  ; Channel Enable
    else
        SPI_Send_Buffer[2] = 0x04;    // AIN - : AIN4,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel Enable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Channel 1 Reg.
    SPI_Send_Buffer[0] = 0x11;
    SPI_Send_Buffer[1] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[2] = 0;
    else
        SPI_Send_Buffer[2] = 0x24;    // AIN - : AIN4,  AIN + :AIN1 ;  SETUP: Setup0  ; Channel Enable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Channel 2 Reg.
    SPI_Send_Buffer[0] = 0x12;
    SPI_Send_Buffer[1] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[2] = 0;
    else
        SPI_Send_Buffer[2] = 0x44;    // AIN - : AIN4,  AIN + :AIN2 ;  SETUP: Setup0  ; Channel Enable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Channel 3 Reg.
    SPI_Send_Buffer[0] = 0x13;
    SPI_Send_Buffer[1] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[2] = 0;
    else
        SPI_Send_Buffer[2] = 0x64;    // AIN - : AIN4,  AIN + :AIN3 ;  SETUP: Setup0  ; Channel Enable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Setup Configuration0  Reg.
    SPI_Send_Buffer[0] = 0x20;
    SPI_Send_Buffer[1] = 0x12;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
    SPI_Send_Buffer[2] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    //初始化Filter Configuration0  Reg.
    if(ConvCodeType != eCONVCODETYPE_UNIPOLAR)
    {
        SPI_Send_Buffer[0] = 0x28;
        SPI_Send_Buffer[1] = 0x02;     //Sinc3 Map disabled , Enhanced filter disabled,    27SPS, 47dB
#if  USE_AS_AUXMCU == 1
        SPI_Send_Buffer[2] = 0x67;//0x6E;     //filter type: sinc3,
#else
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPM4)
            SPI_Send_Buffer[2] = 0x6C; //133.30 SPS     //filter type: sinc3,
        else
            SPI_Send_Buffer[2] = 0x67;//0x6E;     //filter type: sinc3,
#endif
        SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    }
    //初始化 ADC Mode Reg.
    SPI_Send_Buffer[0] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x02;   // 禁止内部参考，16us Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[2] = 0x10;    //使用内部时钟,Single Conversion mode // qjm 0107
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data


    //初始化 IFMODE Reg.
    SPI_Send_Buffer[0] = 0x02;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x18;   // Alt_SYNC:  Enable  IOSTRENGGH enable
    SPI_Send_Buffer[2] = 0x40;    //DATA_STAT: Disable  CRC_EN :Disable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(ADC_CS);


}
#if (USE_AS_AUXMCU == 1)
//以下代码 为辅助ACIM 模块 7175 初始化 ， 单端模式
void Adc24bit_AD7175:: Init_Single_mode (unsigned char ConvCodeType, unsigned char SPI_Idx)
{
// 		switch(ConvCodeType)
// 	  {
// 	      case eCONVCODETYPE_BIPOLAR:
// 		         CodeType= eCONVCODETYPE_BIPOLAR;
// 		         break;
// 	     case eCONVCODETYPE_UNIPOLAR:
// 	     default:
// 		         CodeType = eCONVCODETYPE_UNIPOLAR;
// 		         break;
// 	  }
    unsigned short SPI_Send_Buffer[18];

    ControlPin::SetMeLow(ADC_CS);
    //初始化 GPIO Reg.
    SPI_Send_Buffer[0] = 0x06;  //Write GPIO register
    SPI_Send_Buffer[1] = 0x08;  //MUX_IO: disable , SYNC_EN =0 ,
    SPI_Send_Buffer[2] = 0x00;   //  0x30;  //  IP_EN1=1,IP_EN0=1, 设置外部通道切换器，初始化为第0通道

    //初始化Channel 0 Reg.
    SPI_Send_Buffer[3] = 0x10;
    SPI_Send_Buffer[4] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[5] = 0x20;    // AIN - : AIN0,  AIN + :AIN1 ;  SETUP: Setup0  ; Channel Enable
    else
        SPI_Send_Buffer[5] = 0x04;    // AIN - : AIN4,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel0 Disable

    //初始化Channel 1 Reg.
    SPI_Send_Buffer[6] = 0x11;
    SPI_Send_Buffer[7] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[8] = 0;
    else
        SPI_Send_Buffer[8] = 0x24;    // AIN - : AIN4,  AIN + :AIN1 ;  SETUP: Setup0  ; Channell Enable

    //初始化Channel 2 Reg.
    SPI_Send_Buffer[9] = 0x12;
    SPI_Send_Buffer[10] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[11] = 0;
    else
        SPI_Send_Buffer[11] = 0x44;    // AIN - : AIN4,  AIN + :AIN2 ;  SETUP: Setup0  ; Channel2 Enable

    //初始化Channel 3 Reg.
    SPI_Send_Buffer[12] = 0x13;
    SPI_Send_Buffer[13] = 0x00;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[14] = 0;
    else
        SPI_Send_Buffer[14] = 0x64;    // AIN - : AIN4,  AIN + :AIN3 ;  SETUP: Setup0  ; Channel3 Disable

    //初始化Setup Configuration0  Reg.
    SPI_Send_Buffer[15] = 0x20;
    SPI_Send_Buffer[16] = 0x12;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
    SPI_Send_Buffer[17] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,18,SPI_Idx);    //send data

    //初始化Filter Configuration0  Reg.
    if(ConvCodeType != eCONVCODETYPE_UNIPOLAR)
    {
        SPI_Send_Buffer[0] = 0x28;
        SPI_Send_Buffer[1] = 0x82;     //Sinc3 Map Enabled , Enhanced filter disabled,    27SPS, 47dB
        SPI_Send_Buffer[2] = 0x61;//0x62;     //filter type: sinc3,
        SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    }
    //初始化 ADC Mode Reg.
    SPI_Send_Buffer[0] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x00;   // 禁止内部参考，0us Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[2] = 0x10;    //使用内部时钟,Single Conversion mode // qjm 0107
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data


    //初始化 IFMODE Reg.
    SPI_Send_Buffer[0] = 0x02;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x18;   // Alt_SYNC:  Enable  IOSTRENGGH enable
    SPI_Send_Buffer[2] = 0x40;    //DATA_STAT: Disable  CRC_EN :Disable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(ADC_CS);
}

// void           Adc24bit_AD7175::Reset2(unsigned char spi_idx)
// {
//   unsigned short SPI_Send_Buffer[16];
//   ControlPin::SetMeLow(ADC_CS);
//   for (unsigned char i=0; i<16;i++)
// 	{
// 	SPI_Send_Buffer[i] = 0xFF;
// 	}
//
//   SSPIFunc::RxTx_Frames(SPI_Send_Buffer,16,spi_idx);    //send data
//   ControlPin::SetMeHigh(ADC_CS);
//
//   TIMERSFunc::DELAY_SYS(1000, 0);
// }

#endif
void           Adc24bit_AD7175::Init_diffmode        (unsigned char ConvCodeType,unsigned char SPI_Idx)
{
    unsigned short SPI_Send_Buffer[18];

    ControlPin::SetMeLow(ADC_CS);
    //初始化 GPIO Reg.
    SPI_Send_Buffer[0] = 0x06;  //Write GPIO register
    SPI_Send_Buffer[1] = 0x08;  //MUX_IO: disable , SYNC_EN =1 , // 20180803 yy
    SPI_Send_Buffer[2] = 0x00;   //  0x30;  //  IP_EN1=1,IP_EN0=1, 设置外部通道切换器，初始化为第0通道

    //初始化Channel 0 Reg.
    SPI_Send_Buffer[3] = 0x10;
    SPI_Send_Buffer[4] = 0x80;
    SPI_Send_Buffer[5] = 0x01;    // AIN - : AIN1,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel Enable

    //初始化Setup Configuration0  Reg.
    SPI_Send_Buffer[6] = 0x20;
    SPI_Send_Buffer[7] = 0x12;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
    SPI_Send_Buffer[8] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100

    //初始化Filter Configuration0  Reg.
    SPI_Send_Buffer[9] = 0x28;
    SPI_Send_Buffer[10] = 0x02;     //Sinc3 Map disabled , Enhanced filter disabled,    27SPS, 47dB
    SPI_Send_Buffer[11] = 0x63;//0x6E;     //filter type: sinc3,

    //初始化 ADC Mode Reg.
    SPI_Send_Buffer[12] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[13] = 0x02;   // 禁止内部参考，16us Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[14] = 0x10;    //使用内部时钟,Single Conversion mode // qjm 0107

    //初始化 IFMODE Reg.
    SPI_Send_Buffer[15] = 0x02;  //Write ADC mode register
    SPI_Send_Buffer[16] = 0x18;   // Alt_SYNC:  Enable  IOSTRENGGH enable
    SPI_Send_Buffer[17] = 0x40;    //DATA_STAT: Enable  CRC_EN :Disable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,18,SPI_Idx);    //send data

    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(ADC_CS);
}
void           Adc24bit_AD7175::Trigger     (unsigned char SPI_Idx)
{
    unsigned short SPI_Send_Buffer[3];

    SPI_Send_Buffer[0] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x00;   // 禁止内部参考，none Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[2] = 0x10;    //使用内部时钟; Single Conversion mode // DKQ 12.01
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
}

unsigned long Adc24bit_AD7175::Read        (unsigned char sspId)
{

    unsigned short ReadComd[3];
    UInt32 datareceived=0;
    unsigned short Rx[4];
    unsigned char status;

    ReadComd[0] = 0x44;

    ControlPin::SetMeLow(ADC_CS);
    SSPIFunc::WriteTxFIFO(ReadComd,1,sspId);   //发送读状态命令
    ReadComd[0] = 0xFF;
    ReadComd[1] = 0xFF;
    ReadComd[2] = 0xFF;

// 	TIMERSFunc::DELAY_SYS(100);

    SSPIFunc::RxTx_Frames(ReadComd,4,sspId);
//  	  ControlPin::SetMeHigh(ADC_CS,1);    //拉高 Ron LI

    for(char i =0; i<4; i++)
        Rx[i] = SSPIFunc::SSPIRxBuff[sspId][i];    //接收数据

    datareceived |= Rx[0]<<16;
    datareceived |= Rx[1]<<8;
    datareceived |= Rx[2];

    status=Rx[3];
    status&=0x80;
    if(status!=0)
        datareceived =0;

    return datareceived ;

}

// void           Adc24bit_AD7175::Reset         (void)
// {
// 	ControlPin::SetMeLow(ADC_CS);
// 	TIMERSFunc::DELAY_SYS(100, 0);
// 	ControlPin::SetMeHigh(ADC_Rst);
//     TIMERSFunc::DELAY_SYS(1000, 0);
// 	TIMERSFunc::DELAY_SYS(1000, 0);    //64 SCLK  3MHz      //21.333us
// 	ControlPin::SetMeLow(ADC_Rst);
// }

/*void  Adc24bit_AD7175::Assign_adc_chn ( unsigned char adc_chn_num, unsigned char SPI_Idx)
{
	  unsigned short SPI_Send_Buffer[12];

		 SPI_Send_Buffer[0] = 0x10;
		 SPI_Send_Buffer[2] = 0x04;  // AIN - : AIN4,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel Enable
		 //初始化Channel 1 Reg.
		 SPI_Send_Buffer[3] = 0x11;
     SPI_Send_Buffer[5] = 0x24;    // AIN - : AIN4,  AIN + :AIN1 ;  SETUP: Setup0  ; Channel Enable
	  //初始化Channel 2 Reg.
      SPI_Send_Buffer[6] = 0x12;
 	    SPI_Send_Buffer[8] = 0x44;    // AIN - : AIN4,  AIN + :AIN2 ;  SETUP: Setup0  ; Channel Enable

	   SPI_Send_Buffer[9] = 0x13;
	   SPI_Send_Buffer[11] = 0x64;    // AIN - : AIN4,  AIN + :AIN3 ;  SETUP: Setup0  ; Channel Disable

	switch(adc_chn_num)
	{
		case 0:
		//初始化Channel 0 Reg.
		 SPI_Send_Buffer[1] = 0x80;
		 SPI_Send_Buffer[4] = 0x00;
		 SPI_Send_Buffer[7] = 0x00;
		 SPI_Send_Buffer[10] = 0x00;
	   break;

		case 1:
    //初始化Channel 1 Reg.
	   SPI_Send_Buffer[1] = 0x00;
		 SPI_Send_Buffer[4] = 0x80;
		 SPI_Send_Buffer[7] = 0x00;
		 SPI_Send_Buffer[10] = 0x00;
		  break;

		case 2:

    //初始化Channel 2 Reg.
     SPI_Send_Buffer[1] = 0x00;
		 SPI_Send_Buffer[4] = 0x00;
		 SPI_Send_Buffer[7] = 0x80;
		 SPI_Send_Buffer[10] = 0x00;
		  break;

		case 3:
    //初始化Channel 3 Reg.
	   SPI_Send_Buffer[1] = 0x00;
		 SPI_Send_Buffer[4] = 0x00;
		 SPI_Send_Buffer[7] = 0x00;
		 SPI_Send_Buffer[10] = 0x80;
      break;

		default:
		    break;

	}
	   ControlPin::SetMeLow(ADC_CS);
 	   SSPIFunc::RxTx_Frames(SPI_Send_Buffer,12,SPI_Idx);    //send data
	   TIMERSFunc::DELAY_SYS(20, 0);
	   ControlPin::SetMeHigh(ADC_CS);
}*/


/*unsigned long Adc24bit_AD7175::Read_ADC_Mode        (unsigned char sspId)
{



	unsigned short ReadComd[3];
    UInt32 datareceived=0;
	unsigned short Rx[3];
	unsigned char status;
   // Init(eCONVCODETYPE_BIPOLAR,1);

	ReadComd[0] = 0x41;   //read ADC mode

    ControlPin::SetMeLow(ADC_CS);
    SSPIFunc::WriteTxFIFO(ReadComd,1,sspId);   //发送读状态命令
	ReadComd[0] = 0xFF;
	ReadComd[1] = 0xFF;
	ReadComd[2] = 0xFF;

// 	TIMERSFunc::DELAY_SYS(100);

   SSPIFunc::RxTx_Frames(ReadComd,3,sspId);
//  	  ControlPin::SetMeHigh(ADC_CS,1);    //拉高 Ron LI

	for(char i =0;i<3;i++)
		Rx[i] = SSPIFunc::SSPIRxBuff[sspId][i];    //接收数据

    datareceived |= Rx[0]<<16;
	 datareceived |= Rx[1]<<8;
	 datareceived |= Rx[2];

//	 status=Rx[3];
//	 status&=0x80;
//	 if(status!=0)
//		  datareceived =0;

    return datareceived ;

}
*/
void  Adc24bit_AD7175::Init_diffmode_BUILTIN (unsigned char ConvCodeType,unsigned char SPI_Idx)
{
    unsigned short SPI_Send_Buffer[18];
    //初始化 GPIO Reg.
    SPI_Send_Buffer[0] = 0x06;  //Write GPIO register
    SPI_Send_Buffer[1] = 0x00;  //MUX_IO: disable , SYNC_EN =0 ,
    SPI_Send_Buffer[2] = 0x00;   //  0x30;  //  IP_EN1=1,IP_EN0=1, 设置外部通道切换器，初始化为第0通道

    //初始化Channel 0 Reg.
    SPI_Send_Buffer[3] = 0x10;
    SPI_Send_Buffer[4] = 0x80;
    SPI_Send_Buffer[5] = 0x01;    // AIN - : AIN1,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel Enable

    //初始化Setup Configuration0  Reg.
    SPI_Send_Buffer[6] = 0x20;
    SPI_Send_Buffer[7] = 0x12;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
    SPI_Send_Buffer[8] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100

    //初始化Filter Configuration0  Reg.
    SPI_Send_Buffer[9] = 0x28;
    SPI_Send_Buffer[10] = 0x02;     //Sinc3 Map disabled , Enhanced filter disabled,    27SPS, 47dB
    SPI_Send_Buffer[11] = 0x63;//0x6E;     //filter type: sinc3,

    //初始化 ADC Mode Reg.
    SPI_Send_Buffer[12] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[13] = 0x02;   // 禁止内部参考，16us Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[14] = 0x10;    //使用内部时钟,Single Conversion mode // qjm 0107

    //初始化 IFMODE Reg.
    SPI_Send_Buffer[15] = 0x02;  //Write ADC mode register
    SPI_Send_Buffer[16] = 0x18;   // Alt_SYNC:  Enable  IOSTRENGGH enable
    SPI_Send_Buffer[17] = 0x40;    //DATA_STAT: Enable  CRC_EN :Disable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,18,SPI_Idx);    //send data

    TIMERSFunc::DELAY_SYS(20, 0);

}

//以下代码 为辅助ACIM 模块 7175 初始化 ， 单端模式
void Adc24bit_AD7175:: Init_FFT_mode (unsigned char ConvCodeType, unsigned char SPI_Idx)
{
// 		switch(ConvCodeType)
// 	  {
// 	      case eCONVCODETYPE_BIPOLAR:
// 		         CodeType= eCONVCODETYPE_BIPOLAR;
// 		         break;
// 	     case eCONVCODETYPE_UNIPOLAR:
// 	     default:
// 		         CodeType = eCONVCODETYPE_UNIPOLAR;
// 		         break;
// 	  }
    unsigned short SPI_Send_Buffer[18];

    // ControlPin::SetMeLow(ADC_CS);  // by T. J. 12/20/2017

    //初始化 GPIO Reg.
    SPI_Send_Buffer[0] = 0x06;  //Write GPIO register
    SPI_Send_Buffer[1] = 0x08;  //MUX_IO: disable , SYNC_EN =0 ,
    SPI_Send_Buffer[2] = 0x00;   //  0x30;  //  IP_EN1=1,IP_EN0=1, 设置外部通道切换器，初始化为第0通道

    //初始化Channel 0 Reg.
    SPI_Send_Buffer[3] = 0x10;
    SPI_Send_Buffer[4] = 0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[5] = 0x20;    // AIN - : AIN0,  AIN + :AIN1 ;  SETUP: Setup0  ; Channel Enable
    else
        SPI_Send_Buffer[5] = 0x04;    // AIN - : AIN4,  AIN + :AIN0 ;  SETUP: Setup0  ; Channel0 Disable

    //初始化Channel 1 Reg.
    SPI_Send_Buffer[6] = 0x11;
    SPI_Send_Buffer[7] = 0x00;//0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[8] = 0;
    else
        SPI_Send_Buffer[8] = 0x24;    // AIN - : AIN4,  AIN + :AIN1 ;  SETUP: Setup0  ; Channell Enable

    //初始化Channel 2 Reg.
    SPI_Send_Buffer[9] = 0x12;
    SPI_Send_Buffer[10] = 0x00;//0x80;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[11] = 0;
    else
        SPI_Send_Buffer[11] = 0x44;    // AIN - : AIN4,  AIN + :AIN2 ;  SETUP: Setup0  ; Channel2 Enable

    //初始化Channel 3 Reg.
    SPI_Send_Buffer[12] = 0x13;
    SPI_Send_Buffer[13] = 0x00;
    if(ConvCodeType == eCONVCODETYPE_UNIPOLAR)
        SPI_Send_Buffer[14] = 0;
    else
        SPI_Send_Buffer[14] = 0x64;    // AIN - : AIN4,  AIN + :AIN3 ;  SETUP: Setup0  ; Channel3 Disable

    //初始化Setup Configuration0  Reg.
    SPI_Send_Buffer[15] = 0x20;
    SPI_Send_Buffer[16] = 0x10;//1/4/2018//0x12;     //Bipolar Coded ,Ref- buffer disabled, Ref + buffer disabled , AIN - buffer disabled  AIN + buffer enabled
    SPI_Send_Buffer[17] = 0x00;     //Referebce select External, Burnout Enable: Disabled  ODR: 100
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,18,SPI_Idx);    //send data

    //初始化Filter Configuration0  Reg.
    if(ConvCodeType != eCONVCODETYPE_UNIPOLAR)
    {
        SPI_Send_Buffer[0] = 0x28;
        SPI_Send_Buffer[1] = 0x02;     //Sinc3 Map Diabled , Enhanced filter disabled,    27SPS, 47dB
        SPI_Send_Buffer[2] = 0x06;//0x62;     //filter type: sinc1+sinc5, 15625 sps
        //SPI_Send_Buffer[2] = 0x03;  //filter type: sinc1+sinc5, 50000 sps, 01/30/2018
        SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    }
    //初始化 ADC Mode Reg.
    SPI_Send_Buffer[0] = 0x01;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x00;   // 禁止内部参考，0us Delay Conversion,  SINF_CYC :Disable
    SPI_Send_Buffer[2] = 0x08;    //使用内部时钟,continuous Conversion mode // qjm 0107
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data


    //初始化 IFMODE Reg.
    SPI_Send_Buffer[0] = 0x02;  //Write ADC mode register
    SPI_Send_Buffer[1] = 0x08;   // Alt_SYNC:  Enable  IOSTRENGGH enable
    SPI_Send_Buffer[2] = 0x40;    //DATA_STAT: Disable  CRC_EN :Disable
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data

    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(ADC_CS);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
