#include "includes.h"

unsigned char     DAC16bit_AD5360::CodeType;  //Offset Code Type or 2's Comp Code Type;

St_Port           DAC16bit_AD5360::Pin_DacCS;
St_Port           DAC16bit_AD5360::Pin_DacRst;
St_Port           DAC16bit_AD5360::Pin_DacLoad;
St_Port           DAC16bit_AD5360::Pin_DacBusy;

SSPI_Para         DAC16bit_AD5360::SPI_Para;
unsigned char     DAC16bit_AD5360::SPI_Idx;

//这里AD5360与FRAM共用  SPI总线
void DAC16bit_AD5360::Init(unsigned char ConvCodeType)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
        //case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }
    ControlPin::SetMeHigh(Pin_DacCS);
}
// unsigned char DAC16bit_AD5360::Set_DacGain(unsigned char chn,unsigned long value)
// {
// 	unsigned short comand = 0;
// 	unsigned short SPI_Send_Buffer[3];

// 	comand = DAC_GAIN_REG_AD5360+chn;
// 	SPI_Send_Buffer[0] = comand;
// 	SPI_Send_Buffer[1] = (value>>8);    //填充进去
// 	SPI_Send_Buffer[2] = (UChar8) (0x00FF & value);

// 	ControlPin::SetMeLow(Pin_DacCS);
// 	SSPIFunc::RxTx_Frames(&SPI_Send_Buffer[0],3,SPI_Idx);    //0号通道
// 	ControlPin::SetMeHigh(Pin_DacCS);

// 	value = 0x4000;
// 	comand = DAC_OFFSET_REG_AD5360+chn;
// 	SPI_Send_Buffer[0] = comand;
// 	SPI_Send_Buffer[1] = (value>>8);    //填充进去
// 	SPI_Send_Buffer[2] = (UChar8) (0x00FF & value);

// 	ControlPin::SetMeLow(Pin_DacCS);
// 	SSPIFunc::RxTx_Frames(&SPI_Send_Buffer[0],3,SPI_Idx);    //0号通道
// 	ControlPin::SetMeHigh(Pin_DacCS);

// 	return 1;
// }

// 同为输出数据，本程序最后需要关闭芯片片选信号。
unsigned char DAC16bit_AD5360::Dac_out(unsigned char chn,unsigned long value)
{
    unsigned long DacCode = 0;
    unsigned short comand = 0;
    unsigned short SPI_Send_Buffer[3];
    unsigned char re;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        if(BuiltIn_DataManager::m_BuiltInCSFlag ==1)
            return 0;
    }
    value = 0xFFFFFF - value;  //negtive;

    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
        DacCode = (unsigned long)(sValue >> 8);     //PRO7下位机软件层的标准24BIT码制转化为
    }
    else
        DacCode = value >> 8;                       //PRO7下位机软件层的标准24BIT码制转化为

    comand = DAC_REG_AD5360+chn;
    SPI_Send_Buffer[0] = comand;
    SPI_Send_Buffer[1] = (UChar8)(DacCode>>8);    //填充进去
    SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(&SPI_Send_Buffer[0],3,SPI_Idx);    //0号通道
    ControlPin::SetMeHigh(Pin_DacCS);

    return re;
}
// void DAC16bit_AD5360::Dac_Reset(void)
// {
// 	ControlPin::SetMeLow(Pin_DacRst);
// 	ARBUS_DELAY(100);
// 	ControlPin::SetMeHigh(Pin_DacRst);
// 	ARBUS_DELAY(10000);    //300us reset time  AD5360重置
// }

// 同为输出数据，本程序不对芯片片选信号操作。
// void DAC16bit_AD5360::SendDacData(unsigned char chn,unsigned long value)
// {
// 	unsigned long DacCode = 0;
// 	unsigned short comand = 0;
// 	unsigned short SPI_Send_Buffer[3];

// 	value = 0xFFFFFF - value;  //negtive;

// 	if(CodeType == eCONVCODETYPE_BIPOLAR)
// 	{
// 		signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
// 		DacCode = (unsigned long)(sValue >> 8);     //PRO7下位机软件层的标准24BIT码制转化为
// 	}
// 	else
// 		DacCode = value >> 8;                       //PRO7下位机软件层的标准24BIT码制转化为

// 	comand = DAC_REG_AD5360+chn;
// 	SPI_Send_Buffer[0] = comand;
// 	SPI_Send_Buffer[1] = (DacCode>>8);    //填充进去
// 	SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

// 	ControlPin::SetMeLow(Pin_DacCS);
// 	SSPIFunc::WriteTxFIFO(&SPI_Send_Buffer[0],3,SPI_Idx);    //0号通道
// 	//   ControlPin::SetMeHigh(Pin_DacCS);
// }
// void DAC16bit_AD5360::FinishSendData()
// {
// 	ControlPin::SetMeHigh(Pin_DacCS);
// }

