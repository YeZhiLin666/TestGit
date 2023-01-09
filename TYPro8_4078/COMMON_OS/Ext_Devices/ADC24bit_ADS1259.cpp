
#include "includes.h"
unsigned char   Adc24bit_ADS1259::CodeType;  //Bipolar Code Type or Unipolar Code Type;

St_Port  Adc24bit_ADS1259::Pin_Conver;
St_Port  Adc24bit_ADS1259::Pin_Busy;
St_Port  Adc24bit_ADS1259::Pin_CS;
St_Port  Adc24bit_ADS1259::Pin_Rst;

unsigned short  Adc24bit_ADS1259::Ads1259reg[Adc24bit_Ads1259_MAX];
unsigned short  Adc24bit_ADS1259::ReadData[Adc24bit_Ads1259_READ];

unsigned char   Adc24bit_ADS1259::SPI_Idx;        //CS5560芯片使用的是SPI,SSP0,SSP1,这里告诉芯片驱动使用的是哪个功能模块

//put cs5560 chip into reset mode,and init all used CP pins
void Adc24bit_ADS1259::Init(unsigned char ConvCodeType,unsigned short SPS)
{
    unsigned short m_SPSValue=0;
// 	switch(ConvCodeType)
// 	{
// 	case eCONVCODETYPE_BIPOLAR:
// 		CodeType= eCONVCODETYPE_BIPOLAR;
// 		break;
// 		//case eCONVCODETYPE_UNIPOLAR:
// 	default:
// 		CodeType = eCONVCODETYPE_UNIPOLAR;
// 		break;
// 	}
    if(ConvCodeType == eCONVCODETYPE_BIPOLAR)
        CodeType= eCONVCODETYPE_BIPOLAR;
    //case eCONVCODETYPE_UNIPOLAR:
    else
        CodeType = eCONVCODETYPE_UNIPOLAR;

    m_SPSValue=SPS&0x000f;
#if  USE_AS_AUXMCU == 0
    ControlPin::SetMeHigh(Pin_CS);
#endif
    ControlPin::SetMeLow(Pin_Conver);
    Ads1259reg[0]=0x0011;  // 停止RDATAC模式，准备开始写寄存器
    Ads1259reg[1]=0x0040;  //发送写命令， 从寄存器0地址开始写 0100 0000
    Ads1259reg[2]=0x0008;  //写0~8个寄存器   0000 1000
    Ads1259reg[3]=0x0005;  //0x0005  使用内部基准
    Ads1259reg[4]=0x0051;   // 有校验 sin2 Filter  64个TCK 延时 大约8.68us
    Ads1259reg[5]=0x0010+m_SPSValue;  //Pulse Control mode  1200SPS=5  10sps=0
// 	Ads1259reg[6]=0x0000;
// 	Ads1259reg[7]=0x0000;
// 	Ads1259reg[8]=0x0000;  // no offset correct
// 	Ads1259reg[9]=0x0000;
// 	Ads1259reg[10]=0x0000;
    memset(&Ads1259reg[6],0,sizeof(unsigned short)*5);
    Ads1259reg[11]=0x0040;  // no full-scale correct
    Reset();
}
#if  USE_AS_AUXMCU == 1
//put cs5560 chip into reset mode,and init all used CP pins
void Adc24bit_ADS1259::InitExtBase(unsigned char ConvCodeType,unsigned short SPS)
{
    unsigned short m_SPSValue=0;
// 	switch(ConvCodeType)
// 	{
// 	case eCONVCODETYPE_BIPOLAR:
// 		CodeType= eCONVCODETYPE_BIPOLAR;
// 		break;
// 		//case eCONVCODETYPE_UNIPOLAR:
// 	default:
// 		CodeType = eCONVCODETYPE_UNIPOLAR;
// 		break;
// 	}
    if(ConvCodeType == eCONVCODETYPE_BIPOLAR)
        CodeType= eCONVCODETYPE_BIPOLAR;
    //case eCONVCODETYPE_UNIPOLAR:
    else
        CodeType = eCONVCODETYPE_UNIPOLAR;

    m_SPSValue=SPS&0x000f;
    ControlPin::SetMeLow(Pin_Conver);
    Ads1259reg[0]=0x0011;  // 停止RDATAC模式，准备开始写寄存器
    Ads1259reg[1]=0x0040;  //发送写命令， 从寄存器0地址开始写 0100 0000
    Ads1259reg[2]=0x0008;  //写0~8个寄存器   0000 1000
    Ads1259reg[3]=0x0001;  //0x0005  使用外部基准
    Ads1259reg[4]=0x0059;   // 有校验 sin2 Filter  64个TCK 延时 大约8.68us
    Ads1259reg[5]=0x0010+m_SPSValue;  //Pulse Control mode  1200SPS=5  10sps=0
// 	Ads1259reg[6]=0x0000;
// 	Ads1259reg[7]=0x0000;
// 	Ads1259reg[8]=0x0000;  // no offset correct
// 	Ads1259reg[9]=0x0000;
// 	Ads1259reg[10]=0x0000;
    memset(&Ads1259reg[6],0,sizeof(unsigned short)*5);
    Ads1259reg[11]=0x0040;  // no full-scale correct
    Reset();
}
#endif
void Adc24bit_ADS1259::Convert(void)  //use for test dirui 2014/10/23
{
    ControlPin::SetMeHigh(Pin_Conver);
    ARBUS_DELAY(50);
    ControlPin::SetMeLow(Pin_Conver);
}

//unsigned long Adc24bit_ADS1259::Read(unsigned char sspId,unsigned char uc_ChannelNo)
unsigned long Adc24bit_ADS1259::Read(unsigned char sspId)
{
    unsigned long int  temp = 0;    //返回的值
    //unsigned long uw32_timeout;

    unsigned short Tx[5],Rx[4];
    //uw32_timeout = 0;

    //ControlPin::SetMeLow(Pin_Conver); Ron 122115
#if  USE_AS_AUXMCU == 0
    ControlPin::SetMeLow(Pin_CS);
#endif


    Tx[0] = 0x0012;
// 	Tx[1] = 0;
// 	Tx[2] = 0;
// 	Tx[3] = 0;
// 	Tx[4] = 0;
    memset(&Tx[1],0,sizeof(unsigned short)*4);

    SSPIFunc::RxTx_Frames(Tx,5,sspId);    //这个函数本身不要引用CS信号
#if  USE_AS_AUXMCU == 0
    ControlPin::SetMeHigh(Pin_CS);
#endif
    for(char i =0; i<4; i++)
    {
        Rx[i] = SSPIFunc::SSPIRxBuff[sspId][i+1];    //接收数据
    }

    //把Rx组成成为u32
    temp |= Rx[0]<<16;
    temp |= Rx[1]<<8;
    temp |= Rx[2];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
        temp  = CommonFunc::Bipolar2Unipolar24BitCode(temp);
    //else 如果ADC本身读回来的就是Unipolar的，那么就什么也不用做了
    return temp;
}

// void Adc24bit_ADS1259::Trigger(unsigned char uc_ChannelNo)
// void Adc24bit_ADS1259::Trigger(void)
// {
// #if(USW_AS_AUXMCU == 0)
// 	ControlPin::SetMeLow(Pin_CS);
// #endif
// 	Convert();
// }

//void Adc24bit_ADS1259::WriteReg(unsigned char sspId,unsigned char uc_ChannelNo)
void Adc24bit_ADS1259::WriteReg(unsigned char sspId)

{
#if  USE_AS_AUXMCU == 1
    SSPIFunc::RxTx_Frames(&Ads1259reg[0],12,sspId);
#else
    ControlPin::SetMeLow(Pin_CS);
    SSPIFunc::RxTx_Frames(&Ads1259reg[0],12,sspId);
    ControlPin::SetMeHigh(Pin_CS);
#endif

}
//void Adc24bit_ADS1259::ReadReg(unsigned char sspId,unsigned char uc_ChannelNo)

// Comment out, DO NOT DELETE!
//void Adc24bit_ADS1259::ReadReg(unsigned char sspId)
//{
//	unsigned long uw32_timeout;

//	unsigned short Tx[12];
//	uw32_timeout = 0;

//	while(ControlPin::ReadMe(Pin_Busy))
//	{
//		if(uw32_timeout ++ >= Adc24bit_ADS1259_TIME_OUT)
//			return ;
//	}
//    Tx[0] = 0x0011;
//	Tx[1] = 0x0020;
//	Tx[2] = 0x0008;
//	Tx[3] = 0;
//	Tx[4] = 0;
//	Tx[5] = 0;
//	Tx[6] = 0;
//	Tx[7] = 0;
//	Tx[8] = 0;
//	Tx[9] = 0;
//	Tx[10] = 0;
//	Tx[11] = 0;
//
//    #if  USE_AS_AUXMCU == 0
//    ControlPin::SetMeLow(Pin_CS);
//    #endif
//    SSPIFunc::RxTx_Frames(&Tx[0],12,sspId);    //这个函数本身不要引用CS信号
//    #if  USE_AS_AUXMCU == 0
//    ControlPin::SetMeHigh(Pin_CS);
//    #endif
//	for(char i =0;i<9;i++)
//	{
//		Tx[i] = SSPIFunc::SSPIRxBuff[sspId][i+3];    //接收数据
//		ReadData[i]= SSPIFunc::SSPIRxBuff[sspId][i+3];
//	}
//}
void Adc24bit_ADS1259::Reset(void) //硬件设置死了,可通过软件设置
{
    unsigned short Tx[1];
    Tx[0] = 0x0006;
#if  USE_AS_AUXMCU == 0
    ControlPin::SetMeLow(Pin_CS);
// #endif
    SSPIFunc::RxTx_Frames(&Tx[0],1,1);
// #if  USE_AS_AUXMCU == 0
    ControlPin::SetMeHigh(Pin_CS);
#endif
}

// Comment out, DO NOT DELETE!
//void Adc24bit_ADS1259::SetChannel(unsigned char uc_ChannelNo)  //Only be used for 2ndV_HRNEW  dirui 2014/10/23
//{
//	if(uc_ChannelNo>=16)
//		return;
//	#if(USE_AS_AUXMCU==1)
//	#else
//	return;                                      //IV可以先不考虑该问题
//	#endif
//}
