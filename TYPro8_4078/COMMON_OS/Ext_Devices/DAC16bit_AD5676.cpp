#include "includes.h"

unsigned char DAC16bit_AD5676::CodeType;  //Offset Code Type or 2's Comp Code Type;
unsigned char DAC16bit_AD5676::SPI_Idx;

St_Port   DAC16bit_AD5676::Pin_DacCS;
St_Port   DAC16bit_AD5676::Pin_DacLoad;
St_Port   DAC16bit_AD5676::Pin_DacRst;

void DAC16bit_AD5676::Init(unsigned char ConvCodeType)
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

    ControlPin::SetMeLow(Pin_DacRst);
    TIMERSFunc::DELAY_SYS( 1000, 0 );    //20ns
    ControlPin::SetMeHigh(Pin_DacRst);

    //初始化，power up AD5676
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    comand = Power_up_and_down_AD5676;
    SPI_Send_Buffer[0] = comand;      // 高位先行，低位最后。3*8
    SPI_Send_Buffer[1] = 0x00;       //  数据
    SPI_Send_Buffer[2] = 0x00;       //  关断模式0为正常模式
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    ControlPin::SetMeHigh(Pin_DacCS);

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    comand = Internal_Ref_setup_register_AD5676;
    SPI_Send_Buffer[0] = comand;      //  高位先行，低位最后。
    SPI_Send_Buffer[1] = 0x00;        //  数据
    SPI_Send_Buffer[2] = 0x04;        //  Gain = 2;
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    ControlPin::SetMeHigh(Pin_DacCS);

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    ControlPin::SetMeLow(Pin_DacLoad);    //使用第二种模式同步模式

    comand = Update_DAC_ALL_channels_AD5676;
    SPI_Send_Buffer[0] = comand;      //  高位先行，低位最后。
    SPI_Send_Buffer[1] = 0x00;        //  数据
    SPI_Send_Buffer[2] = 0x00;        //  数据
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    ControlPin::SetMeHigh(Pin_DacCS);

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
}

///////////////////////////////////////////////////////////////DAC
/*****************************************************************************
** Function name:		Dac_Out
**
** Descriptions:		Set the DAC value, the first
**				        parameter is DAC channel No., the 2nd
**				        parameter is DAC value.
**
** parameters:		uc_Channel：通道号
**                              uw16_Value：数值
** Returned value:	  uc_Result: true 操作成功；false操作失败
**
*****************************************************************************/
// 同为输出数据，本程序最后需要关闭芯片片选信号。
UChar8 DAC16bit_AD5676::Dac_Out(unsigned char chn,unsigned long value)
{
    unsigned long DacCode = 0,TimeOut = 0;
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
        DacCode = (unsigned long)(sValue >> 8);     //PRO7下位机软件层的标准24BIT码制转化为
    }
    else
        DacCode = value >> 8;                       //PRO7下位机软件层的标准24BIT码制转化为

    comand = Write_and_Updata_DAC_n_AD5676 + chn;
    SPI_Send_Buffer[0] = comand;
    SPI_Send_Buffer[1] = (DacCode>>8);    //填充进去
    SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

    while(SSPIFunc::bBusy[SPI_Idx])    //block here to wait for SSP idle
    {
        TimeOut++;
        if(TimeOut > 0x0FFFFF)
            return 0;
    }
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //0号通道
    ControlPin::SetMeHigh(Pin_DacCS);

    return 1;
}

// 同为输出数据，本程序不对芯片片选信号操作。
void DAC16bit_AD5676::Dac_SendData(unsigned char chn,unsigned long value)
{
    unsigned long DacCode = 0,TimeOut = 0;
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
        DacCode = (unsigned long)(sValue >> 8);     //PRO7下位机软件层的标准24BIT码制转化为
    }
    else
        DacCode = value >> 8;                       //PRO7下位机软件层的标准24BIT码制转化为

    comand = Write_and_Updata_DAC_n_AD5676 + chn;
    SPI_Send_Buffer[0] = comand;
    SPI_Send_Buffer[1] = (DacCode>>8);    //填充进去
    SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

    while(SSPIFunc::bBusy[SPI_Idx])    //block here to wait for SSP idle
    {
        TimeOut++;
        if(TimeOut > 0x0FFFFF)
            return;
    }
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::WriteTxFIFO(SPI_Send_Buffer,3,SPI_Idx);
    //ControlPin::SetMeHigh(Pin_DacCS);
}
void DAC16bit_AD5676::Dac_SetLoad(void)
{
    ControlPin::SetMeHigh(Pin_DacCS);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

